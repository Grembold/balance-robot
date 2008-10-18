/****************************************************************************
Title:    Access serial EEPROM 24C02 using I2C interace
Author:   Peter Fleury <pfleury@gmx.ch> http://jump.to/fleury
File:     $Id: test_i2cmaster.c,v 1.2 2003/12/06 17:07:18 peter Exp $
Software: AVR-GCC 3.3
Hardware: AT90S8515 at 4 Mhz, any AVR device can be used

Description:
    This example shows how the I2C library i2cmaster.S can be used to 
    access a serial eeprom.
    Based on Atmel Application Note AVR300, adapted to AVR-GCC C interface

*****************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h> 	// dient zur behandlung der Interrupts
#include <util/twi.h> 			// enth�lt z.B. die Bezeichnungen f�r die Statuscodes in TWSR
#include <stdint.h> 				// definiert den Datentyp uint8_t

/********************
* 	   DEFINES 	    *
********************/
#define SLAVE_ADD  0x50      // device address
#define REGISTER_ANZ 7			 // anzahl der Slaveregister


//Je nach Statuscode in TWSR m�ssen verschiedene Bitmuster in TWCR geschreiben werden(siehe Tabellen im Datenblatt!). 
//Makros f�r die verwendeten Bitmuster:
//ACK nach empfangenen Daten senden/ ACK nach gesendeten Daten erwarten
#define TWCR_ACK TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC);  
//NACK nach empfangenen Daten senden/ NACK nach gesendeten Daten erwarten     
#define TWCR_NACK TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC);
//switched to the non adressed slave mode...
#define TWCR_RESET TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC);
//Die Bitmuster f�r TWCR_ACK und TWCR_RESET sind gleich. Dies ist kein Fehler und dient nur der �bersicht!

/****************************
* private globale Variablen *
****************************/
volatile uint8_t register_adr; //"Adressregister" mit adresse des gew�hlten registers

// statusregister f�r TWI
volatile struct {
   unsigned bFirstAccess:1; 	// erster Zugriff, Registerposition setzen
} twi_status;

// Adressen der Register
enum enRegisterAdr { INBuffer=1, OUTBuffer, INCounter, OUTCounter, PaketReqID, Status, GPIO };

typedef struct
{
	uint8_t	id;										// ID des Tabelleneintrages (muss eindeutig sein)
	char titel[TITELLANG+1]; 			// Titel des Men�s
}tstMenue;

typedef struct
{
	uint8_t id;									// ID des Elementes
	uint8_t	pos;								// Position innerhalb des Men�s
	void*	element;							// Pointer auf das Element
	enum 	enElemtenTypen elementType;		// Typ des Elementes
	uint8_t	parent;								// �bergeortnetes Elemente
}tstZeile;

/*******************************
* private Funktions prototypen *
********************************/
void init_twi_slave (uint8_t adr);

/****************
*     main      *
****************/
int main(void)
{
	init_twi_slave(SLAVE_ADD);
	sei();
	while(1);
}


/****************************************************************************/
/*!
*  \brief
*  Initaliserung des TWI-Inteface. Muss zu Beginn aufgerufen werden, sowie bei
*	 einem Wechsel der Slave Adresse 
*	\param adr gew�nschte Slave-Adresse
*  
*****************************************************************************/
void init_twi_slave (uint8_t adr)
{
	TWAR= adr; 																		// Adresse setzen
	TWCR &= ~(1<<TWSTA)|(1<<TWSTO);							// START und STOP deaktivieren
	TWCR|= (1<<TWEA) | (1<<TWEN) | (1<<TWIE);		// aktivieren von: Acknowledge, Interrupt und TWI 
	register_adr=0x00;														// registeradresse zur�cksetzen
	twi_status.bFirstAccess = 1;									// erster zugriff nach stop
	
	DDRB = 0xff;
	PORTB = 0x00;
	DDRA = 0xff;
	PORTA = 0x00;
}


/****************************************************************************/
/*!
*  \brief
*  Interrupt wird ausgel�st ein Ereigniss auf dem Bus ausgel�st wird.
*  Im Register TWSR befindet sich dann ein Statuscode, anhand dessen die 
*  Situation festgestellt werden kann.
*****************************************************************************/
ISR(TWI_vect)
{
	uint8_t data = 0;
	switch (TW_STATUS) 			// TWI-Statusregister pr�fen und n�tige Aktion bestimmen 
	{	
		case TW_SR_SLA_ACK: 		// 0x60 Slave Receiver, wurde adressiert	
			TWCR_ACK; 						// n�chstes Datenbyte empfangen, ACK danach
			PORTA = 0xFC;
			break;
			
		/***   Daten Empfangen   ****/	
		case TW_SR_DATA_ACK: 		// 0x80 Slave Receiver,Daten empfangen
			data=TWDR; 						// Empfangene Daten auslesen
			if(twi_status.bFirstAccess)	// erte Adressierung nach STOP
			{
				register_adr = data;// Empfangene Register Adresse speciern
				PORTA = register_adr;
				twi_status.bFirstAccess = 0;
			}
			else
			{
				switch( register_adr)
				{
					case INBuffer:
					case OUTBuffer:
					case INCounter:
					case OUTCounter:
					case PaketReqID:
					case Status:
					case GPIO:
						PORTB = data;
						break;
					default:
						PORTB = 0xff;
				}
				register_adr++;
			}								
			TWCR_ACK;
			break;
			
		/***   Daten versenden   ***/
		case TW_ST_SLA_ACK: 		// 0xA8 Slave Transmitter wurde adressiert
		case TW_ST_DATA_ACK: 		// 0xB8 Slave Transmitter, weitere Daten wurden angefordert
			TWCR_ACK;
			break;
			
		case TW_ST_DATA_NACK:		// 0xC0 Keine Daten mehr gefordert 
		case TW_SR_DATA_NACK:		// 0x88 Keine Daten weiteren empfangen
		case TW_ST_LAST_DATA:		// 0xC8  Last data byte in TWDR has been transmitted (TWEA = �0�); ACK has been received
		case TW_SR_STOP: 				// 0xA0 STOP empfangen
		default:								// alle anderen codes
				twi_status.bFirstAccess = 1;
				TWCR_RESET; 				// �bertragung beenden, warten bis zur n�chsten Adressierung
	} // end switch
}// end ISR


