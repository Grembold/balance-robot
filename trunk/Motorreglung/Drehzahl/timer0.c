#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer0.h"

/* Timer0 initialisieren */
void timer0_init(void)
{
	/* Prescaler auf 64: Clock alle 5,78us (172,8KHz). */
	/* �berlauf alle 1,48ms (675 Hz)  */
	TCCR0 |= ((1<<CS01) | (1<<CS00));// Prescaler 64, entspricht 5,8usec und 1,48ms bis zum �berlauf
	
	/* Prescaler auf 8: Clock alle 0,72us (1386kHz). */
	/* �berlauf alle 185us ms (5400 Hz)  */
	//TCCR0 |= (1<<CS01);// Prescaler 8, entspricht 0,72usec und 185us bis zum �berlauf

	/* Interrupts f�r Timer0 aktivieren */	
	TIMSK |= (1<<TOIE0);
	
	DDRB |= (1<<PB3);		// toggel led
	
	timer0_counter_D1=0;	// sezte den Anfangswert
	timer0_counter=0;		// sezte den Anfangswert

}	

/* Interrupt Service Rountine bei �berlauf des Timer0 */
ISR(TIMER0_OVF_vect) /* veraltet: SIGNAL(SIG_OVERFLOW1) */
{
    /* Interrupt Code */
	timer0_counter++;			// Merker f�r �berl�ufe des Timers
	timer0_counter_D1++;		// Merker f�r �berl�ufe des Timers, f�r die Drehzahl1

	if(!(timer0_counter % 15)){ 	// alle 44ms 
		auswerten();			// den Wert �ber UART ausgeben
	}	

	if( (timer0_counter >= 255)){	// alle 380ms 
		PORTB ^=(1<<PB3);			// eine LED getoggelt
	}
	
}
