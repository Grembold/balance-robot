/*************************************************************************/
/*!
 * \file main.c
 *
 * \brief
 *
 * \par Funktionsweise: Programm zur Inbetriebnahme des MotorControlers
 *
 * \version	V--- - 04.01.2009 - Olaf Petersen und Arne Petersen\n
 *			Created\n
 *************************************************************************/

#include <avr/io.h> 	//I/O-Register
#include <stdint.h>		//standart Integertypen
#include <stdlib.h> 	// itoa
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart.h"
#include "timer.h"

#ifndef F_CPU
/* prevent compiler error by supplying a default */
#warning "F_CPU not defined"
#endif

#define BAUD 19200
#define MYUBRR 64 //#define MYUBRR F_CPU/16/BAUD-1

#define false 0
#define true 1
#define ENC_INV -128
#define ENC	144		//Encoderstriche * 4

#define K_P 1		//Verstärkung
#define K_I 12		//Verstärkung des Integrators
#define T_A 20		//Abtastzeit in ms


typedef union {
	struct {
		unsigned Signal_A :1; 		// 1 Bit für bStatus_1
		unsigned Signal_B :1; 		// 1 Bit für bStatus_2
		unsigned AltSignal_A :1; 	// Und hier noch mal ein Bit
		unsigned AltSignal_B :1; 	// Dieses Feld ist 2 Bits breit
	} x;
	uint8_t bRow;
} tPhase;

volatile uint16_t lNumOfEncFehler;
volatile int32_t lCounter;
volatile int32_t lCounterAlt;

int32_t integrator;
uint16_t flanken_soll;

void init_pwm_TC0() {
	//PortD5 und PortD6 als Output konfigurieren
	DDRD |= _BV(PORTD5);
	DDRD |= _BV(PORTD6);
	//Modes of Operation Fast PWM Mode WGM02:0 = 3
	TCCR0A |= _BV(WGM00) | _BV(WGM01);
	//Compare Output Mode Clear on compare match, set on bottom
	TCCR0A |= _BV(COM0A1);
	//Clock Select Prescaler factor 8 ~ PWM Periode von ca. 9,7kHz @ 20MHz fCPU
	TCCR0B |= _BV(CS01);
	//PWM aus
	OCR0A = 0x00;
}



void init_uart() {
	/*** uart_init(); ***/
	/*Set baud rate */
	unsigned char ubrr = MYUBRR;
	UBRR0H = (unsigned char) (ubrr >> 8);
	UBRR0L = (unsigned char) ubrr;
	/*Enable receiver and transmitter */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	/* Set frame format: 8data, 2stop bit (1<<USBS0)|*/
	UCSR0C = (3 << UCSZ00);
}

void init_encoder() {
	/*** ENCODER INTERRUPT INIT ***/
	// Maske welche Pins einen Interrupt auslösen sollen
	PCMSK0 |= (1 << PCINT0) | (1 << PCINT1);
	// Interne Pull-Up Widerstände aktivieren
	//PORTB |= (1<<PCINT0) | (1<<PCINT1);
	// Pin Change Interrupt 0 Enablee
	PCICR = (1 << PCIE0);
	// Encoder Conter auf null setzen
	lCounter = 0;
}

// Unterprogramm mit Zeitschleife fuer ATMega16 ohne Quarz
void long_delay(uint16_t ms) {
	for(;ms>0;ms--) _delay_ms(1);
}

void regler(int32_t e){
	integrator += e;

	if(integrator > 900)
		integrator = 900;
	if(integrator < -900)
		integrator = -900;

	int32_t u_p = e * K_P;
	int32_t u_i = integrator * K_I * (T_A /1000.0);
	uint8_t u;

	//Stellwertbegrenzung
	if ((u_p + u_i) > 255)
		u = 255;
	else if ((u_p + u_i) < 0)
		u = 0;
	else
		u = (u_p + u_i);


	uart_puti(e);			// Reglerabweichung
	uart_puts("\t");
	uart_puti(u_p);			// P-Stellwert
	uart_puts("\t");
	uart_puti(u_i);			// I-Stellwert
	uart_puts("\t");
	uart_puti(u);			// Stellwert
	uart_puts("\t");
	OCR0A = u;		//Setze PWM für Motor A
}

int main(void) {
	//Init TC0 to using the Fast PWM Mode
	init_pwm_TC0();

	unsigned long lZeit;
	timer2_init();
	init_uart();
	init_encoder();

	DDRD |= (1 << 7);
	PORTD |= (1 << 7);

	lCounter = 0;
	lCounterAlt = 0;
	lNumOfEncFehler = 0;
	integrator = 0;
	flanken_soll = 0;

	uart_puts("Reglerabweichung\tP-Anteil\tI-Anteil\tStellwert\tSum_Flanken\tSum_Enc_Fehler\tIst-Wert\tSoll-Wert\r\n");
	//uint8_t uart_buf;				// von Uart abgefragtes Zeichen


	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1 << UDRE0)))
		;
	/* Put data into buffer, sends the data */
	//UDR0 = 'd';
	//uart_putc('C');

	// interrupts allgemein erlauben
	sei();

	// Aktuelle Zeit Speichern
	lZeit = Gettime();

	uint8_t rampensau = 0;
	uint8_t rampensau_pre = 0;
	uint8_t var_sau = 0;
	uint8_t phase = 0;


	// endlosprogram
	for (;;) {
		if (Gettime() > (lZeit + T_A)) // alle 100ms Ausführen
		{
			uint8_t sreg_temp;
			sreg_temp = SREG;
			cli();
			int32_t flanken = lCounter;
			int32_t ist_speed = (flanken - lCounterAlt);
			int32_t fehler = (flanken_soll - ist_speed);
			lCounterAlt = flanken;
			lZeit = Gettime();
			SREG = sreg_temp;
			//uart_buf = uart_getc();						// tastenabfrage

			PORTD ^= (1 << 7); // Toggle PB0 z.B. angeschlossene LED

			regler(fehler);
			//OCR0A = 127;

/*			if(rampensau == 255 || rampensau == 0){
				rampensau_pre++;
				if(rampensau_pre == 80){
					rampensau = 0;
				}else if(rampensau_pre == 100){
					rampensau = 1;
					rampensau_pre = 0;
				}
			}else
				rampensau++;
*/
			if (rampensau_pre == 2) {
				switch (phase) {
					case 0:
						rampensau = 0;
						if (var_sau == 40) {
							phase = 1;
							var_sau = 0;
						}
						var_sau++;
						break;
					case 1:
						rampensau++;
						if (rampensau >= 255) {
							phase = 2;
						}
						break;
					case 2:
						rampensau = 255;
						if (var_sau == 40) {
							phase = 3;
							var_sau = 0;
						}
						var_sau++;
						break;
					case 3:
						rampensau = 110;
						if (var_sau == 60) {
							phase = 4;
							var_sau = 0;
						}
						var_sau++;
						break;
					case 4:
						rampensau--;
						if (rampensau <= 30) {
							rampensau = 0;
							phase = 0;
						}
						break;
				}
				rampensau_pre = 0;
			} else {
				rampensau_pre++;
			}
			flanken_soll = rampensau;

			uart_puti(flanken); // Encoderwerte ausgeben
			uart_puts("\t");
			uart_puti(lNumOfEncFehler);
			uart_puts("\t");
			uart_puti(ist_speed); // Encoderwerte ausgeben
			uart_puts("\t");
			uart_puti(rampensau);
			uart_puts("\r\n");

		}
	}

	return 0;
}

ISR(PCINT0_vect)
{
	static const int8_t aSteps[] = { 	0, -1, 1, ENC_INV,
										1, 0, ENC_INV, -1,
										-1, ENC_INV, 0, 1,
										ENC_INV, 1, -1, 0 };
	static tPhase bPhase;
	int8_t iStep = 0;

	bPhase.bRow = (bPhase.bRow << 2);
	bPhase.bRow &= 0x0C;				// 0000 1100 = 0x0C
	bPhase.x.Signal_A = (PINB & (1 << PCINT0)) ? true : false;
	bPhase.x.Signal_B = (PINB & (1 << PCINT1)) ? true : false;

	iStep = aSteps[bPhase.bRow];
	if (iStep != ENC_INV)
	{
		lCounter += iStep;
	}
	else
	{
		lNumOfEncFehler++;
	}
}

