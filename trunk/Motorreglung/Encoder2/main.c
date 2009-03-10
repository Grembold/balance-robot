/*************************************************************************/
/*!
* \file main.c
*
* \brief
*
* \par Funktionsweise: programm zum testen der UART kommunication
*
* \version	V--- - 26.03.2007 - Olaf Petersen\n
*			Created\n
*************************************************************************/

#include <avr/io.h> 	//I/O-Register
#include <stdint.h>		//standart Integertypen
#include <stdlib.h> 	// itoa
#include <avr/interrupt.h>

#include "../nbot_lib/uart.h"
#include "../nbot_lib/timer.h"

#define FOSC 8000000 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

#define false 0
#define true 1
#define ENC_INV -128

typedef union
{
struct {
   unsigned Signal_A:1; // 1 Bit für bStatus_1
   unsigned Signal_B:1; // 1 Bit für bStatus_2
   unsigned AltSignal_A:1; // Und hier noch mal ein Bit
   unsigned AltSignal_B:1;    // Dieses Feld ist 1 Bits breit
} x;
uint8_t bRow;
} tPhase;

volatile int32_t lCounter;

int main(void)
{

	unsigned long lZeit;

/***	timer2_init(); ***/
	/* Timer2 auf 36 kHz eingestellt. */
	TCCR2A = (0 << WGM20) | (0 << WGM21) | (0 << COM2A0) | (0 << COM2A1);
	TCCR2B = (1 << CS20);

	/*
	// Timer2 Überlauf auf eine Pin legen, um 36KHz genau einzustellen
	OCR2B = 255;
	DDRD |= (1<<PIN3);
	TCCR2A |= (1<<COM2B0);
	*/

	/* Interrupts für Timer2 aktivieren */
	TIMSK2|= (1<<TOIE2);
/*** END TIMER2 INIT ***/

/*** uart_init(); ***/
	/*Set baud rate */
	unsigned char ubrr = MYUBRR;
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/*Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit (1<<USBS0)|*/
	UCSR0C = (3<<UCSZ00);
/*** END UART INI ***/

/*** ENCODER INTERRUPT INIT ***/
	// Maske welche Pins einen Interrupt auslösen sollen
	PCMSK0 |= (1<<PCINT0) | (1<<PCINT1);

	// Interne Pull-Up Widerstände aktivieren
	//PORTB |= (1<<PCINT0) | (1<<PCINT1);

	// Pin Change Interrupt 0 Enablee
	PCICR = (1<<PCIE0);

	// Encoder Conter auf null setzen
	lCounter = 0;
/*** END ENCODER ISR INIT ***/

DDRD |= (1<<7);
PORTD |= (1<<7);

	uart_puts("\f *** Hallo! ***\r\n");
	uart_puts(" Ich bin ein kleiner Roboter. Was moechtest du mit mir machen?\r\n");
	uart_puts("BOT:\\>");
	//uint8_t uart_buf;				// von Uart abgefragtes Zeichen


	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) )
	;
	/* Put data into buffer, sends the data */
	UDR0 = 'd';
	uart_putc('C');

	// interrupts allgemein erlauben
	sei();

	// Aktuelle Zeit Speichern
	lZeit = Gettime();

	// endlosprogram
	for(;;)
	{
		if(Gettime() > (lZeit + 100) ) // alle 100ms Ausführen
		{
			lZeit = Gettime();

			//uart_buf = uart_getc();						// tastenabfrage

			PORTD ^= ( 1 << 7 );  // Toggle PB0 z.B. angeschlossene LED

			uart_puti(lCounter);	// Encoderwerte ausgeben
			uart_puts("\r\n");

		}
	}

	return 0;
}



ISR(PCINT0_vect)
{

	static const int8_t aSteps[] =
	{
			    0,      -1,       1, ENC_INV,
				  1,       0, ENC_INV,      -1,
				 -1, ENC_INV,       0,       1,
		ENC_INV,       1,      -1,       0
	};
	static tPhase bPhase;
	int8_t iStep = 0;

	bPhase.bRow = (bPhase.bRow<<2);
	bPhase.bRow &=(3<<2);
	bPhase.x.Signal_A = (PINB & (1<< PCINT0)) ? true : false;
	bPhase.x.Signal_B = (PINB & (1<< PCINT1)) ? true : false;

	iStep = aSteps[bPhase.bRow];
	if(iStep != ENC_INV)
		lCounter += iStep;

}

