/*************************************************************************/
/*!
* \file pcc.c
*
* \brief Dieses Programm soll eine PC Kommunikation herstellen
*
* \par Funktionsweise:
*
*
* \version	V--- - 16.04.2007 - Olaf Petersen\n
*			Created\n
*************************************************************************/

#include <avr/io.h> 	//I/O-Register 
#include <stdint.h>		//standart Integertypen
#include <stdlib.h> 	// itoa

#include "pcc.h"
#include "../nbot_lib/uart.h"


typedef union{
	uint8_t y;
	struct{
		unsigned cmdid:5;
		unsigned length:3;
	};	
}tunFirstByte;


typedef struct {
	tunFirstByte header;
	uint8_t		message[7];
} tstMessage;


tunFirstByte header;
const pcc_t *ppcc;

tstMessage receivedMessage;
tstMessage transmitMessage;


/**************************************************************************/
/*!
*	\brief Initialisiert die PC Kommunikation
*
***************************************************************************/
void pcc_init(pcc_t *daten)
{
	ppcc=daten;
	firstbyte.y = 0;
	receivedMessage.header.y = 0;	
	transmitMessage.header.y = 0;
}
/**************************************************************************/
/*!
*	\brief Hier werden die empfangenen Daten ausgewertet
*
***************************************************************************/
void pcc(uint8_t buf)
{
	if(reseavedMessage.header.length == 0) 	// neuer befehl kann empfangen werden
	{
		header.y = buf;
	
		if(header.length == 0)
			reseavedMessage.header
	} else	{												// Datum gehört zum letzten empfangenen Befehl
		reseavedMessage.message
	}

}
