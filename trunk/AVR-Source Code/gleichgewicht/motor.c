#include <avr/io.h> 
#include <avr/interrupt.h>
#include <stdint.h> //integerwerte wie uint8_t, uint16_t

#include "motor.h"


void motor_init(void) 
{ 

	// Achtung!! an PD0 und PD1 ist UART angeschlossen 
	DDRD |=(1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7); 
	PORTD &=~((1<<PD2)|(1<<PD3)|(1<<PD6)|(1<<PD7));

	
	/* normale 10-bit PWM aktivieren ( nicht invertiert ) */ 
	TCCR1A = (1<<WGM10)|(1<<WGM11); 
	/* Einstellen der PWM-Frequenz auf ca. 172kHz kHz ( Prescaler = 64 ) */ 
	TCCR1B = (1<<CS11)|(1<<CS10); 
	/* PWN einschalten -- > PIN mit PWM verbinden */ 
	PWM_LINKS_ON; 
	PWM_RECHTS_ON; 
} 

/** setzt die Richtung und die Geschwindigkeit des linken Motors **/
void motor_l(int8_t speed) // Linker Motor(A)
{
	if(speed<0) 		//bei negativen zahlen rückwärts fahren
	{
		PORTD |=(1<<PD7);
		PORTD &= ~(1<<PD6);
		
		speed = speed -1;
		speed = ~speed;
	} else {			//bei 0 oder positiven, vorwärts
		PORTD &= ~(1<<PD7);
		PORTD |=(1<<PD6);
	}
	
	PWM_LINKS = speed<<3; //10 bit für pwm und das 11. gegen die neagation
}

/** setzt die Richtung und die Geschwindigkeit des rechten Motors **/
void motor_r(int8_t speed) // Rechter Motor(B)
{
	if(speed<0) 		//bei negativen zahlen rückwärts fahren
	{
		PORTD &= ~(1<<PD2);
		PORTD |=(1<<PD3);
		
		speed = speed -1; //  absulutwert
		speed = ~speed;
	} else {			//bei 0 oder positiven, vorwärts
		PORTD |=(1<<PD2);
		PORTD &= ~(1<<PD3);
	}
	
	PWM_RECHTS = speed<<3; //10 bit für pwm und das 11. gegen die neagation
}
