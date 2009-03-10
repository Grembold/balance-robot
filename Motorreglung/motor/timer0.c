#include <avr/io.h>
#include <avr/interrupt.h>
#include "taste.h"
#include "timer0.h"

/* Timer0 initialisieren */
void timer0_init(void)
{
	/* Prescaler auf 64: Clock alle 5,78us (172,8KHz). */
	/* Überlauf alle 1,48ms (675 Hz)  */
	TCCR0 = (1<<CS01)|(1<<CS00);	// (1<<CS01)|(1<<CS00);

	/* Interrupts für Timer0 aktivieren */	
	TIMSK |= (1<<TOIE0);
	
	/* LED als Ausgang deklarieren */
	DDRB |= (1<<PB3); 
	
	
	/* Anfangswerte */ 
	timer0_counter_D1=0;	
	timer0_counter=0;
	d1_error=0;
}	

ISR(TIMER0_OVF_vect) /* veraltet: SIGNAL(SIG_OVERFLOW1) */
{
    /* Interrupt Code */
	timer0_counter++;			// Merker für Überläufe des Timers
	timer0_counter_D1++;		// Merker für Überläufe des Timers, für die Drehzahl1
	
	if(timer0_counter_D1==0xff)
	{
		timer0_counter_D1=0;
		if (d1_error<0xff) d1_error++;
	}
	
	if( (timer0_counter == 0)){	// alle 380ms 
		PORTB ^=(1<<PB3);				// eine LED getoggelt, blinken ist immer gut
	}
	
	if(!(timer0_counter % 4)){ 	// alle 6ms 
		tasten();					// Tasten entprellen und status ermitteln
	}
	
		
}
