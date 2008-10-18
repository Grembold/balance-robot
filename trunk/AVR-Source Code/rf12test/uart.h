/**************************************************************************/
/*!
* \file uart.h
* \brief Definitionen und Funktionen der UART kommunikation
*
* $Revision: 0.1 $
* $Date: 25.02.2007 $
* $Author: Olaf Petersen $
*
*
* Description: new functions has been added:
*
* void uart_init(void)
* void uart_putc(char c)
* void uart_puts(char* s)
* void uart_puti(uint16_t i)
* void uart_printtime(void)
*
*
* Ver.     Date         Author           Comments
* -------  ----------   --------------   ------------------------------
* 1.00	   14.08.2003   Olaf Petersen	 build
*
*
***************************************************************************/

#ifndef _UART_H
#define _UART_H

#define BAUD 			9600			/*!< Baudrate der UART-Kommunikation */
//#define UBRR_BAUD	5						/*!< ((F_CPU)/((BAUD)*16)-1) */
#define UBRR_BAUD  (F_CPU / (BAUD * 16L) - 1)


/* USART Funktionen */
extern void uart_init(void);				// Initialisierung
extern void uart_putc(char c);			// sendet ein Zeichen
extern void uart_puts(char* s);			// sendet ein String
extern void uart_puts_p(const char *text);	// sendet einen String der im Flash steht
extern void uart_puti(uint16_t i);		// sendet ein integer
extern void uart_printtime(void);						// sendet die aktuelle Systemzeit

extern uint8_t uart_getc(uint8_t *date);

#endif /* _UART_H */

