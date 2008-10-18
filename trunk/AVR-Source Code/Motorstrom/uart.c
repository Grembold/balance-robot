/*****************************************************************/
/**																**/
/**	Funktionen für eine Ausgabe über die Serielle Scnittstelle	**/
/** 															**/
/**	Implementierung der uart_puti nicht abgeschlossen			**/
/** 															**/
/*****************************************************************/

#include <avr/interrupt.h>
#include <stdlib.h>
#include "uart.h"


void uart_init(void)
{
	/* Baudrate einstellen ( Normaler Modus ) */
	UBRRH = (unsigned char) (UBRR_BAUD>>8);
	UBRRL = (unsigned char) UBRR_BAUD;

	/* Aktivieren des Empfängers, des Senders und des "Daten empfangen"-Interrupts */
	UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);

	/* Einstellen des Datenformats: 8 Datenbits, 1 Stoppbit */
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
	
	uart_puts("Uart wurde richtig initialisiert\n");
}

/** sendet ein einzelnes Zeichen **/
void uart_putc(char c)
{
	/* warten bis der Sendepuffer leer ist */
	while ( !( UCSRA & (1<<UDRE)) );
	
	/* Zeichen senden */
	UDR = c;
}

/** sendet einen string **/
void uart_puts(char *s)
{
	while (*s) // so lange senden, bis das ende des Strings erreicht ist
	{
		uart_putc(*s);
		s++;
	}
	//uart_putc('\r');
}

/** sendet einen Integer **/
void uart_puti(uint8_t *i)
{
	unsigned char s[5];
	itoa(*i, s,10);
	uart_puts(s);
}



/* Interrupt wird ausgelöst sobald neue Daten im USART-Empfangspuffer liegen */
ISR(USART_RXC_vect)
{
	 
	unsigned char buffer;

	/* Daten aus dem Puffer lesen */
	buffer = UDR;

	/* warten bis der Sendepuffer leer ist */
	while ( !( UCSRA & (1<<UDRE)) );
		
	/* und gleich wieder zurück schicken */
	UDR = buffer;
}


