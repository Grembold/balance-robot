/*************************************************************************/
/*!
* \file pcc.c
*
* \brief Dieses Programm diehnt als Kommandointerpreter zur kommunikation
*	beispielswerie eine Serielle verbindung
*
* \par Funktionsweise:
*		Dem Programm werden einzelne empfangene Zeichen übergeben und fügt 
*		diese zu einer Nachricht zusammen. Anhand des Kommandos der Nachricht werden 
*		entsprechende Aktionen ausgeführt.
*
*	\par Kommandos: voflgende Kommandos sind implementiert:
*		-- Board Port Status 					(01)
*		-- Board Batterie Spannung		(03)
*		-- Board Systemzeit						(05)
*		-- Motor Links Encoder				(10)
*		-- Motor Links Istdrehzahl		(12)
*		-- Motor Links Solldrehzahl		(13)
*		-- Motor Rechts Encoder				(10)
*		-- Motor Rechts Istdrehzahl		(12)
*		-- Motor Rechts Solldrehzahl	(13)
*
* \version	V--- - 16.04.2007 - Olaf Petersen\n
*			Created\n
*\version		V--- - 22.06.2007	-	Olaf Petersen\n
*			PCC_BOARD_AKKU added\n
*************************************************************************/

#include <avr/io.h> 	//I/O-Register 
#include <stdint.h>		//standart Integertypen
#include <stdlib.h> 	// itoa

#include "globals.h"
#include "pcc.h"
#include "uart.h"
#include "rf12.h"

// Defines
#define pcc_rx_ready					1
#define pcc_rx_befehlsempfang 2
#define pcc_rx_fehler					3
#define pcc_rx_done						4






// Private Funktionen
void pcc_reset(void);
void pcc_sendError(void);

// Variablen
const pcc_t *ppcc;

tunHeader header;
tstMessage receivedMessage;
tstMessage transmitMessage;

uint8_t		empfangeneBytes;
uint8_t		versendeteBytes;
uint8_t sm_rx;			// statemachine receive

/**************************************************************************/
/*!
*	\brief Initialisiert die PC Kommunikation
*
***************************************************************************/
void pcc_init(const pcc_t *daten)
{
	ppcc = daten;	// datenpoiter übergeben
	sm_rx = pcc_rx_ready;
	pcc_reset();
}

/**************************************************************************/
/*!
*	\brief zurücksetzen aller Inhalte
*
***************************************************************************/
void pcc_reset(void)
{
	header.y = 0;
	receivedMessage.header.y = 0;
	transmitMessage.header.y = 0;
	for(int i = 0; i<8;i++)
	{
		receivedMessage.message[i] = 0;
		transmitMessage.message[i] = 0;
	}
	empfangeneBytes = 0;
}

/**************************************************************************/
/*!
*	\brief Hier werden die empfangenen Daten ausgewertet
*
***************************************************************************/
void pcc_rx(uint8_t buf)
{
	if(sm_rx == pcc_rx_ready || sm_rx == pcc_rx_befehlsempfang)
	{
		sm_rx = pcc_rx_befehlsempfang;
		
		if(empfangeneBytes == 0)																					// Header wird empfangen
		{
			receivedMessage.header.y = buf;
		} else if((receivedMessage.header.length) >= empfangeneBytes) 	// Daten werden empfangen
		{
			receivedMessage.message[(empfangeneBytes-1)] = buf;			
		} 
		if((receivedMessage.header.length) == empfangeneBytes)						// Nachricht vollständig empfangen
		{
			sm_rx = pcc_rx_done;									
		}
	} else 																															// alter Befehl noch nicht abgearbeitet
	{
		sm_rx = pcc_rx_fehler;						
	}
	empfangeneBytes++;
}

/**************************************************************************/
/*!
*	\brief Server
*
***************************************************************************/
void pcc(void)
{
	
	switch (sm_rx)
	{
		case pcc_rx_ready:
		case pcc_rx_befehlsempfang: 
			break;
		case pcc_rx_fehler:
			pcc_sendError();
			pcc_reset();
			sm_rx = pcc_rx_ready;
			break;
		case pcc_rx_done:
			switch (receivedMessage.header.cmdid)
			{
				case PCC_SYNC: 
					pcc_reset();
					break;
				case PCC_BOARD_GET:
					if(receivedMessage.header.length == 1)
					{
						transmitMessage.header.cmdid = PCC_BOARD_GET;
						transmitMessage.header.length = 2;
						switch (receivedMessage.message[0])
						{
							case 'A': 
								transmitMessage.message[0] = DDRA;
								transmitMessage.message[1] = PINA;
								break;
							case 'B': 
								transmitMessage.message[0] = DDRB;
								transmitMessage.message[1] = PINB;
								break;
							case 'C': 
								transmitMessage.message[0] = DDRC;
								transmitMessage.message[1] = PINC;
								break;
							case 'D': 
								transmitMessage.message[0] = DDRD;
								transmitMessage.message[1] = PIND;
								break;
							default: 
								pcc_sendError();
						}
						pcc_send(&transmitMessage);
					} else 
						pcc_sendError();
					break;
				case PCC_BOARD_SET:
					if(receivedMessage.header.length == 3)
					{
						transmitMessage.header.cmdid = PCC_BOARD_GET;
						transmitMessage.header.length = 1;
						switch (receivedMessage.message[0])
						{
							case 'A': 
								PORTA |= ((DDRA & receivedMessage.message[2]) & receivedMessage.message[1]);	// Bits setzen
								PORTA &=~((DDRA & receivedMessage.message[2]) & ~receivedMessage.message[1]); // Bits löschen
								break;
							case 'B':
								PORTB |= ((DDRB & receivedMessage.message[2]) & receivedMessage.message[1]);	// Bits setzen
								PORTB &=~((DDRB & receivedMessage.message[2]) & ~receivedMessage.message[1]); // Bits löschen
								transmitMessage.message[0] = 0;
								transmitMessage.message[0] |= ((DDRB & receivedMessage.message[2]) & receivedMessage.message[1]);
								transmitMessage.message[0] &=~((DDRB & receivedMessage.message[2]) & ~receivedMessage.message[1]);
								pcc_send(&transmitMessage);
								break;
							case 'C': 
								break;
							case 'D': 
								break;
							default: 
								pcc_sendError();
						}
					}
					else
						pcc_sendError();
					break;
				case PCC_BOARD_AKKU:
					if(receivedMessage.header.length == 0)
					{
						transmitMessage.header.cmdid = PCC_BOARD_AKKU;
						transmitMessage.header.length = 2;
						transmitMessage.message[0] = (uint8_t)(batteryVoltage>>8);
						transmitMessage.message[1] = (uint8_t)(batteryVoltage & 0x00ff);
						pcc_send(&transmitMessage);
					}
					else
						pcc_sendError();
					break;
				case PCC_BOARD_I2C:
				case PCC_BOARD_TIME:
					if(receivedMessage.header.length == 0)
					{
						transmitMessage.header.cmdid = PCC_BOARD_TIME;
						transmitMessage.header.length = 4;
						transmitMessage.message[0] = *((*ppcc).time)>>24;
						transmitMessage.message[1] = *((*ppcc).time)>>16;
						transmitMessage.message[2] = *((*ppcc).time)>>8;
						transmitMessage.message[3] = *((*ppcc).time);
						pcc_send(&transmitMessage);
					}
					else
						pcc_sendError();
					break;
				case PCC_BOARD_RFM:
					if(receivedMessage.header.length == 0)
					{
						if((*ppcc).rfm!=0)
						{
							uint8_t *dummy = (*ppcc).rfm;
							int i;
							transmitMessage.header.cmdid = PCC_BOARD_RFM;
							transmitMessage.header.length = 7;
							for(i=0;i<7;i++)
							{
								transmitMessage.message[i] = *dummy;
								dummy++;
							}
							pcc_send(&transmitMessage);						
						}
					}
					break;
				case PCC_MOT_L_ENC:
					if(receivedMessage.header.length == 0)
					{
						transmitMessage.header.cmdid = PCC_MOT_L_ENC;
						transmitMessage.header.length = 2;	
						transmitMessage.message[0] = *((*ppcc).encoder[LEFT])>>8;
						transmitMessage.message[1] = *((*ppcc).encoder[LEFT]);
						pcc_send(&transmitMessage);					
					}
					else
						pcc_sendError();
					break;
				case PCC_MOT_L_CUR:
				case PCC_MOT_L_ACS:
					if(receivedMessage.header.length == 0)
					{
						transmitMessage.header.cmdid = PCC_MOT_L_ACS;
						transmitMessage.header.length = 2;	
						transmitMessage.message[0] = *((*ppcc).istwert[LEFT])>>8;
						transmitMessage.message[1] = *((*ppcc).istwert[LEFT]);
						pcc_send(&transmitMessage);					
					}
					else
						pcc_sendError();
					break;
				case PCC_MOT_L_SRS:
					if(receivedMessage.header.length == 0)
					{
						transmitMessage.header.cmdid = PCC_MOT_L_SRS;
						transmitMessage.header.length = 2;	
						transmitMessage.message[0] = *((*ppcc).sollwert[LEFT])>>8;
						transmitMessage.message[1] = *((*ppcc).sollwert[LEFT]);
						pcc_send(&transmitMessage);					
					}
					else
						pcc_sendError();
					break;
				case PCC_MOT_L_TMP:
				case PCC_MOT_R_ENC:
					if(receivedMessage.header.length == 0)
					{
						transmitMessage.header.cmdid = PCC_MOT_R_ENC;
						transmitMessage.header.length = 2;	
						transmitMessage.message[0] = *((*ppcc).encoder[RIGHT])>>8;
						transmitMessage.message[1] = *((*ppcc).encoder[RIGHT]);
						pcc_send(&transmitMessage);					
					}
					else
						pcc_sendError();
					break;
				case PCC_MOT_R_CUR:
				case PCC_MOT_R_ACS:
					if(receivedMessage.header.length == 0)
					{
						transmitMessage.header.cmdid = PCC_MOT_L_ACS;
						transmitMessage.header.length = 2;	
						transmitMessage.message[0] = *((*ppcc).istwert[RIGHT])>>8;
						transmitMessage.message[1] = *((*ppcc).istwert[RIGHT]);
						pcc_send(&transmitMessage);					
					}
					else
						pcc_sendError();
					break;
				case PCC_MOT_R_SRS:
					if(receivedMessage.header.length == 0)
					{
						transmitMessage.header.cmdid = PCC_MOT_L_SRS;
						transmitMessage.header.length = 2;	
						transmitMessage.message[0] = *((*ppcc).sollwert[RIGHT])>>8;
						transmitMessage.message[1] = *((*ppcc).sollwert[RIGHT]);
						pcc_send(&transmitMessage);					
					}
					else
						pcc_sendError();
					break;
				case PCC_MOT_R_TMP:
				case PCC_SENS_IR1:
				case PCC_SENS_IR2:
				case PCC_SENS_ACC:
				case PCC_SENS_ORI:
				case PCC_ACTION_MOVE:
				case PCC_ACTION_TURN: 
					break;
				default:
					pcc_sendError();
			}
			pcc_reset();
			sm_rx = pcc_rx_ready;
			break;
	}
}

void pcc_send(tstMessage *message)
{
	uint8_t *buf = (uint8_t *)message;
	uint8_t versendeteBytes = 0;
	
	char text[] = "test";
	
	do 
	{
		//uart_putc(*buf++);
		rf12_txdata(text, 5); //buf++, 1);
		versendeteBytes++;
	}while(versendeteBytes < ((*message).header.length)+1);
}

void pcc_sendError(void)
{
	transmitMessage.header.cmdid = PCC_SYNC;
	transmitMessage.header.length = 0;
	pcc_send(&transmitMessage);
}
