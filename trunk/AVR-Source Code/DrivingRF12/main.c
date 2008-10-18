/****************************************************************************/
/*!
*	\file     main.c
*
*	\brief    Dieses Programm soll den Bot über die Rf12-Funkverbindng steuern
*
*	\version	V001 - 19.11.2007 - Olaf Petersen\n
*           first build
*
*****************************************************************************/

#include <avr/interrupt.h>
#include <avr/io.h>
#include "rf12.h"
#include "command.h"

//------------------------------------------------------------------------------

int main (void) 
{
	char buffer;

	DDRD = (1<<6)|(1<<4);

	PORTD |= (1<<6);
	PORTD |= (1<<4);
	
	rf12_init();					// ein paar Register setzen (z.B. CLK auf 10MHz)
	rf12_setfreq(RF12FREQ(433.92));	// Sende/Empfangsfrequenz auf 433,92MHz einstellen
	rf12_setbandwidth(5, 1, 4);		// 200kHz Bandbreite, -6dB Verstärkung, DRSSI threshold: -79dBm 
	rf12_setbaud(9600);				// 19200 baud
	rf12_setpower(0, 6);			// 1mW Ausgangangsleistung, 120k


	rf12_txdata("\rRF12 Hallo!\r\n", 14);
	
  while(1)
  { 
		
		
		/********* Bot **********/
		// Empfangen
		while(!(rf12_rxdata(&buffer)));
		
		// Kommando interpretieren
		command_put(buffer);
		command_prompt(command_get());
			
		// Pause
		for(long a = 0;a<1000;a++){asm("nop");};
		
  }
}


