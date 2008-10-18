/****************************************************************************/
/*!
*	\file     uart.c
*
*	\brief    Funktionen zur Kommunikation über die Serielle Schnittstelle\
*
*	\version	V001 - 25.02.2007 - Olaf Petersen\n
*           first build
*
*	\version	V002 -	30.03.2007 - Olaf Petersen\n
*						uart_puts korrigiert\n
*						uart_getc() hinzugefügt\n
*****************************************************************************/

#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include "uart.h"

#define BAUD_REG	12//((F_CPU / (16 * UART_BAUD))-1)						/*!< ((F_CPU)/((BAUD)*16)-1) */



/****************************************************************************/
/*!
*  \brief
*  Die Uart übertragung initialisieren.
*
*  \param
*  keine
*
*  \return
*  nichts
*
*  \par  Funktionsweise:
*  Stellt die gewünschte Baudrate in den entsprechenden register ein\n
*  Aktivieren der Interrupts den Empfänger, den Sender und des 
*  "Daten empfangen"-Interrupts\n
*  einstellen ders Datenformates auf\n
*	8 Datenbits\n
*	Keine Parität\n
*	1 Soppbit\n
*
*  \par  Beispiel:
*  (Nur zur Demonstration der Parameter/Returnwerte)
*  \code
*	int main(void){	
*		
*		uart_init();	// USART initialisieren 
*		
*		sei();			// global Interrupts Aktivieren
*		
*		uart_putc('*');									// senden eines einzelenen Zeichen
*		uart_puts(" UART inizialisierung ... OK\n");	// sendet einen string
*		
*		return 0;
*	}
*  \endcode
*****************************************************************************/
void uart_init(void)
{
	/* Baudrate einstellen ( Normaler Modus ) */
	UCSR0A |= (1<<U2X0);
	UBRR0H = ((uint8_t) (BAUD_REG>>8)) & 0x0F;
	UBRR0L = (uint8_t) BAUD_REG;	
	
	/* Einstellen des Datenformats: 8 Datenbits, 1 Stoppbit */
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
	
	/* Aktivieren des Empfängers, des Senders und des "Daten empfangen"-Interrupts */
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	
	uart_puts("Uart wurde richtig initialisiert\r\n");
	
	// FIFOs für Eingabe initialisieren 
// für reines senden wird der Puffer nicht benötigt
	//fifo_init (&infifo,   inbuf, BUF_SIZE);

}


/****************************************************************************/
/*!
*  \brief
*  Senden eine einzelnes Zeichen über die serielle Schnittstelle
*  
*  \param[in]
*  c zu sendendes Zeichen
*
*  \return
*  nichts
*  
*  \par  Beispiel:
*  (Nur zur Demonstration der Parameter/Returnwerte)
*  \code
*  // Einzerlne Zeichen senden
*  uart_putc ('H');
*  uart_putc ('e');
*  uart_putc ('l');
*  uart_putc ('l');
*  uart_putc ('o');
*  \endcode
*****************************************************************************/
void uart_putc(char c)
{
	/* warten bis der Sendepuffer leer ist */
	while ( !( UCSR0A & (1<<UDRE0)) );
	
	/* Zeichen senden */
	UDR0 = c;
}


/****************************************************************************/
/*!
*  \brief
*  Senden eines null-terminierten Strings
*  
*  \param[in]
*  s Pointer auf null-terminierten String
*
*  \return
*  nichts
*    
*  \par  Beispiel:
*  (Nur zur Demonstration der Parameter/Returnwerte)
*  \code
*  // Beispiel fuer uart_puts
*  uart_puts ("Hello World!\r\n");
*  \endcode
*****************************************************************************/
void uart_puts(char *s)
{
	while(*s) // so lange senden, bis das ende des Strings erreicht ist
	{
		uart_putc(*s++);
	}
}

/****************************************************************************/
/*!
*  \brief
*  Senden eines null-terminierten Strings der im Flash-Speicher steht
*  
*  \param[in]
*  text Pointer auf Flashadresse
*
*    
*  \par  Beispiel:
*  (Nur zur Demonstration der Parameter/Returnwerte)
*  \code
*  // Beispiel fuer uart_puts
*  uart_puts_p(PSTR("Hallo Herr Mustermann!\r\n"));
*  \endcode
*****************************************************************************/
void uart_puts_p(const char *text)
{
    char Zeichen;
 
    while ((Zeichen = pgm_read_byte(text)))
    {   /* so lange, wie mittels pgm_read_byte ein Zeichen vom Flash gelesen
           werden konnte, welches nicht das "String-Endezeichen" darstellt */
 
        /* Das gelesene Zeichen über die normalen Kanäle verschicken */
        uart_putc(Zeichen);
        text++;
    }
}

/****************************************************************************/
/*!
*  \brief
*  Ausgabe eines Integer Wertes als String ueber die serielle Schnittstelle.
*
*  \param[in]
*  i Auszugebender Integer Wert (16Bit)
*  
*  \return
*  nichts
*
*  \par  Beispiel:
*  (Nur zur Demonstration der Parameter/Returnwerte)
*  \code
*  // Beispiel ausgabe eines Wertes
*  uart_puti( wert[0] );
*  uart_puts("\r\n");
*  \endcode
*****************************************************************************/
void uart_puti(uint16_t i)
{
	char s[7];	// "-12345"
	
	/* Wandeln eines Integers in einen String */
	itoa(i, s, 10);		
	/* String versenden */
	uart_puts(s);			
}

