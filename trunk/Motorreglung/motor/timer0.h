

#ifndef _timre0_H
#define _timer0_H

volatile uint8_t timer0_counter;
volatile uint8_t timer0_counter_D1;		// zählt nur bis 255
volatile uint8_t d1_error;					// auswertung ist fehlerhaft

extern void timer0_init(void);

#endif
