
/**         Drehzahl2
* Mit hilfe dieses Programmes soll die Drehzal 
* �ber ein UART interfase am PC ansgegeben werden.
* 
* Hierzu wird jede sekunde ein interrupt ausgel�st und der 
* Z�hlerstand der Timer1 �ber UART ausgegeben.
*
**/


#include <avr/io.h> 
#include <avr/interrupt.h>
#include <stdint.h> //integerwerte wie uint8_t, uint16_t
#include <stdlib.h> // itoa

#include "timer0.h"
#include "uart.h" 

void auswerten(void);
void timer1_init(void);
void init(void);


int main(void){	
	init();			// Allgemeine Port initialisierung
	uart_init();	// USART initialisieren
	timer0_init();	// Initialisiert Timer1
	timer1_init();	// Initialisiert Timer1
	
	sei();			//global Interrupts Aktivieren
	
	uart_putc('*');		//senden eines einzelenen Zeichen
	uart_puts(" UART inizialisierung ... OK \n\r\n\r");	//sendet einen string
	uart_puts("/**         Drehzahl2\r\n* Mit hilfe dieses Programmes soll die Drehzal\r\n");
	uart_puts("* �ber ein UART interfase am PC ansgegeben werden.\r\n*\r\n* Hierzu wird jede sekunde ein interrupt ausgel�st und der\r\n");
	uart_puts("* Z�hlerstand der Timer1 �ber UART ausgegeben.\r\n\r\n\r");
	
	while(1)
	{
	}
	//never reached   i hope ;)
}

void init(void)
{
}

/** Wird durch ein externes Signal getacktet und gibt die anzah der Impulse an **/
void timer1_init(void)
{
	// Diese 3 Bits bestimmen die Quelle f�r den Timer/Counter:
	// 1/1/1 => Externer Pin T1, steigende Flanke 
	TCCR1B = (1<<CS10)|(1<<CS11)|(1<<CS12);

	/* Interrupts f�r Timer1 �berlauf aktivieren */	
	TIMSK |= (1<<TOIE1);
	
	/* Pin TO1 (DDRB.1) wird als eingang definiert und mit pullup versehen */
	DDRB &= ~((1 << PB1));		// Pin 1 als eingang
	PORTB |= (1 << PB1);		// Pin 1 mit pullup	
}


/** Wertet die Impulse des Timers aus und gibt diese �ber UART aus **/
void auswerten(void)
{
	uint16_t takte;				// Impulse des Timer1
	takte = TCNT1;	
	TCNT1 = 0;					// Timer 1 zur�cksetzen
	char s[7];					// String variable
	itoa( takte, s, 10 ); 		// 10 fuer radix -> Dezimalsystem
	uart_puts( s );				// �bermittelt es an Terminal
	uart_puts(" Pulse/s      \r");	// Beschriftug und Zeilenvorschub	
}
