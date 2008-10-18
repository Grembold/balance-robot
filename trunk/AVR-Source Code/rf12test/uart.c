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
//#include "timer.h"
#include "fifo.h"

#define BUF_SIZE 100					/*!< \brief Puffergröße für Lesepuffer [Byte]*/
uint8_t inbuf[BUF_SIZE];		/*!< \brief Speicher für FIFO Puffer */
fifo_t infifo;							/*!< \brief Fifo struct */
	

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
	UBRRH = (unsigned char) (UBRR_BAUD>>8);
	UBRRL = (unsigned char) UBRR_BAUD;	

	/* Aktivieren des Empfängers, des Senders und des "Daten empfangen"-Interrupts */
	UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);

	/* Einstellen des Datenformats: 8 Datenbits, 1 Stoppbit */
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
	
	uart_puts("Uart wurde richtig initialisiert\n");
	
	// FIFOs für Eingabe initialisieren 
	fifo_init (&infifo,   inbuf, BUF_SIZE);
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
	while ( !( UCSRA & (1<<UDRE)) );
	
	/* Zeichen senden */
	UDR = c;
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

/**************************************************************************/
/*! 
* \brief gibt die aktuelle Systemzeit im Format Min:Sec:mSec
*		über die UART verbindung aus\n
*
***************************************************************************/
void uart_printtime(void)
{
	uint32_t ms;
	uint32_t s;
	uint32_t min;
	uint8_t std;
	
//	ms = Gettime();
	if(ms == 0)
		return;
		
	s = ms/1000;
	min = s / 60;
	std = min / 60;
	min = min - (std*60);
	s = s - ((min*60)+(std*60*60));
	ms = ms - ((s*1000)+(min*60*1000)+(std*60*60*1000));
	if(std<10)
		uart_putc('0');
	uart_puti(std);
	uart_putc(':');
	if(min<10)
		uart_putc('0');
	uart_puti(min);
	uart_putc(':');
	if(s<10)
		uart_putc('0');
	uart_puti(s);
	uart_putc(':');
//	if(ms<100)
//		uart_putc('0');
//	if(ms<10 && ms!=0)
//		uart_putc('0');
//	uart_puti(ms);
	uart_puti(ms/100);
}

/****************************************************************************/
/*!
*  \brief Einzelnes Zeichen empfangen
*
*  \return
*	empfangenes Zeichen, ist der Empfangspuffer leer liefert die Funktion 0 zurück
*
*****************************************************************************/
uint8_t uart_getc(uint8_t *date)
{
	uint8_t data;
	
	if (fifo_get(&infifo, &data)==0){
		*date = data;
		return 1;
	} else {
		return 0;
	}
}


/****************************************************************************/
/*!
*  \brief
*  Interrupt wird ausgelöst sobald neue Daten im USART-Empfangspuffer liegen.
*  
*  Der Interrupt sendet das empfangene Zeichen gleich wieder zurück.\n
*  Als \c echo Funktion zur Fehlersuche.\n
*  
*****************************************************************************/
ISR(USART_RXC_vect)
{
	 
	unsigned char buffer;

	/* Daten aus dem Puffer lesen */
	buffer = UDR;
	fifo_put(&infifo, buffer);

	/* warten bis der Sendepuffer leer ist */
	while ( !( UCSRA & (1<<UDRE)) );
		
	/* ECHO und gleich wieder zurück schicken */
	//UDR = buffer;

}


