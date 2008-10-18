#include <avr/interrupt.h>
#include <avr/io.h>
#include "usart.h"
#include "rf12.h"
//------------------------------------------------------------------------------

int main (void) 
{
	unsigned char test[100]="\n";
	unsigned char pong[10]="pong";
	usart_init(); // Serielle Schnittstelle initialisieren
	DDRD = (1<<6)|(1<<4);

	PORTD |= (1<<6);
	PORTD |= (1<<4);
	
	//DDRB = (1<<3);
	//PORTB = (1<<3);
	
	usart_write("Usart Initialieisrt\r\n"); 

	rf12_init();					// ein paar Register setzen (z.B. CLK auf 10MHz)
	rf12_setfreq(RF12FREQ(433.92));	// Sende/Empfangsfrequenz auf 433,92MHz einstellen
	rf12_setbandwidth(5, 1, 4);		// 200kHz Bandbreite, -6dB Verstärkung, DRSSI threshold: -79dBm 
	rf12_setbaud(9600);				// 19200 baud
	rf12_setpower(0, 6);			// 1mW Ausgangangsleistung, 120k


	/* Endlosschleife */
  while(1)
  { 
	
		/********** PC ***********/
		// senden
		rf12_txdata("ping",13);
		usart_write("PC:\t");
		usart_write((char*)"ping");
		usart_write("\r\n");				
		// Empfangen
		usart_write("Bot:\t");
		while(!(rf12_rxdata(test)));
		// über Uart ausgeben		
		usart_write((char*)test);
		usart_write("\r\n");
		
		// Pause
		for(long a = 0;a<1000000;a++){asm("nop");};
		
		
		/********* Bot **********/
/*	
		// Empfangen
		while(!(rf12_rxdata(test)));
		
		// über Uart ausgeben
		usart_write((char*)test);
		usart_write("\r\n");
		
		// Pause
		for(long a = 0;a<1000000;a++){asm("nop");};
		
		// zurücksenden
		rf12_txdata(pong,10);
*/	

  }
}

