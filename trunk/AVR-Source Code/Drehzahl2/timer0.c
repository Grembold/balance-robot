#include <avr/io.h>
#include <avr/interrupt.h>
//#include "taste.h"
#include "timer0.h"

/* Timer0 initialisieren */
void timer0_init(void)
{
	/* alle 6,35msEinstellen der Frequenz auf 157Hz ( Prescaler = 256  * 256 bis Überlauf ) */
	TCCR0 = (1<<CS02);

	/* Interrupts für Timer0 aktivieren */	
	TIMSK |= (1<<TOIE0);
	
	DDRB |= (1<<PB3);

}	

ISR(TIMER0_OVF_vect) /* veraltet: SIGNAL(SIG_OVERFLOW1) */
{
    /* Interrupt Code */
	timer0_counter++;
	if(timer0_counter == 169){	// alle 533,4ms wied die LED getoggelt
		timer0_counter=0;
		PORTB ^=(1<<PB3);
		auswerten();
	}
	//tasten(); // Aufruf zur tasten entprellung
}
