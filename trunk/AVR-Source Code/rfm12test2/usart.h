/*----------------------------------------------------------------------------
 Copyright:      Radig Ulrich  mailto: mail@ulrichradig.de
 Author:         Radig Ulrich
 Remarks:        
 known Problems: none
 Version:        14.06.2007
 Description:    RS232 Routinen
----------------------------------------------------------------------------*/

#ifndef _UART_H
	#define _UART_H

#define BUFFER_SIZE	100

volatile unsigned char buffercounter;
char usart_rx_buffer[BUFFER_SIZE];

struct {
		volatile unsigned char usart_ready:1;
		volatile unsigned char usart_rx_ovl:1;
		}usart_status ;
		
//----------------------------------------------------------------------------

#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <avr/io.h>

//----------------------------------------------------------------------------

//Der Quarz auf dem Experimentierboard
#define SYSCLK	11059000	//Quarz Frequenz in Hz
//#define SYSCLK	14318180	//Quarz Frequenz in Hz
//#define SYSCLK	16000000	//Quarz Frequenz in Hz

//Die Baud_Rate der Seriellen Schnittstelle ist 9600 Baud
#define BAUD_RATE 9600		//Baud Rate für die Serielle Schnittstelle	
//#define BAUD_RATE 14400		//Baud Rate für die Serielle Schnittstelle	
//#define BAUD_RATE 28800		//Baud Rate für die Serielle Schnittstelle	
//#define BAUD_RATE 38400		//Baud Rate für die Serielle Schnittstelle	
//#define BAUD_RATE 57600		//Baud Rate für die Serielle Schnittstelle	

//Anpassen der seriellen Schnittstellen Register wenn ein ATMega128 benutzt wird
#if defined (__AVR_ATmega128__)
#	define USR UCSR0A
#	define UCR UCSR0B
#	define UDR UDR0
#	define UBRR UBRR0L
#	define EICR EICRB
#endif

#if defined (__AVR_ATmega32__)
#	define USR UCSRA
#	define UCR UCSRB
#	define UBRR UBRRL
#	define EICR EICRB
#endif

#if defined (__AVR_ATmega16__)
#	define USR UCSRA
#	define UCR UCSRB
#	define UBRR UBRRL
#	define EICR EICRB
#endif

#if defined (__AVR_ATmega8__)
#	define USR UCSRA
#	define UCR UCSRB
#	define UBRR UBRRL
#endif

#if defined (__AVR_ATmega88__)
#	define USR UCSR0A
#	define UCR UCSR0B
#	define UBRR UBRR0L
#	define TXEN TXEN0
#	define UDR UDR0
#	define UDRE UDRE0
#endif
//----------------------------------------------------------------------------

void usart_init(void); 
int usart_write_char(char c);
void usart_write_str(char *str);
void usart_write (char *Buffer,...);

//----------------------------------------------------------------------------

#endif //_UART_H

