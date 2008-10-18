#include <avr/interrupt.h>
#include <avr/io.h>
#include "uart.h"
#include "rf12.h"
//------------------------------------------------------------------------------


#define LED1 (1<<PB2)							///< Pin der LED 1
#define LED2 (1<<PB3)							///< Pin der LED 2
#define LED1_AN	PORTB &= ~LED1		///< Schaltet die LED 1 an
#define LED1_AUS	PORTB |= LED1		///< Schaltet die LED 1 aus
#define LED1_TOG	PORTB ^= LED1		///< wechselt den Zustand der LED 1
#define LED2_AN	PORTB &= ~LED2		///< Schaltet die LED 2 an
#define LED2_AUS	PORTB |= LED2		///< Schaltet die LED 2 aus
#define LED2_TOG	PORTB ^= LED2		///< wechselt den Zustand der LED 2

int main (void) 
{
	unsigned char test[100]="\n";

	uart_init(); // Serielle Schnittstelle initialisieren

	
	DDRB |= LED1 | LED2;
	LED1_AUS;
	LED2_AUS;

	
	uart_puts("\rUSB TEST\r\n"); 
	
	rf12_init();					// ein paar Register setzen (z.B. CLK auf 10MHz)
	rf12_setfreq(RF12FREQ(433.92));	// Sende/Empfangsfrequenz auf 433,92MHz einstellen
	rf12_setbandwidth(5, 1, 4);		// 200kHz Bandbreite, -6dB Verstärkung, DRSSI threshold: -79dBm 
	rf12_setbaud(9600);				// 19200 baud
	rf12_setpower(0, 6);			// 1mW Ausgangangsleistung, 120k

	// globale Interrups aktivieren
	sei();
	
  while(1)
  { 


		/********** PC ***********/
		// von Bot Empfangen
		if((rf12_rxdata(test)))
		{			
			// über Uart ausgeben		
			uart_puts((char*)test);
			LED2_TOG;
		}


		// von Terminal einlesen
		if (uart_getc(test) == 1)
		{
			// an Bot senden
			rf12_txdata(test,1);
			LED1_TOG;
		}
				
		// Pause
		//for(long a = 0;a<1000;a++){asm("nop");};
		
		
		/********* Bot **********/	
/*
		// Empfangen
		if(rf12_rxdata(test))
		{
		
			// über Uart ausgeben
			uart_puts((char*)test);
			uart_puts("\r\n");
			LED1_TOG;
			// Pause
			for(long a = 0;a<1000;a++){asm("nop");};
			
			// zurücksenden
			rf12_txdata(test,13);
			LED1_TOG;
		}
*/
  }
}



