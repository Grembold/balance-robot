#include <avr/io.h> 
#include <avr/interrupt.h>
#include <stdint.h> //integerwerte wie uint8_t, uint16_t


#include "taste.h" 
#include "timer0.h"

#define TASTE0 PA0	//
#define TASTE1 PA1	//
#define TASTE2 PA2	//

#define PWM_LINKS		OCR1A 
#define PWM_LINKS_ON	TCCR1A |=  (1<<COM1A1) 
#define PWM_LINKS_OFF   TCCR1A &= ~(1<<COM1A1) 

#define PWM_RECHTS		OCR1B 
#define PWM_RECHTS_ON	TCCR1A |=  (1<<COM1B1) 
#define PWM_RECHTS_OFF  TCCR1A &= ~(1<<COM1B1) 


void motor_init(void);
void motor_l(int8_t speed);
void motor_r(int8_t speed);


int main(void) 
{ 
	/** steuerung der Tasten **/
	DDRA &=~((1<<TASTE0)|(1<<TASTE1)|(1<<TASTE2)); // Tasten als Eingänge
	PORTA |=(1<<TASTE0)|(1<<TASTE1)|(1<<TASTE2);	//pullup aktivieren
	timer0_init();
	
	
	
	motor_init();   //motoren initialisieren
	motor_l(0);		//linker motor aus (keine bremse)
	motor_r(0);		//rechter motor aus
	
	int8_t speed_l = 0; // motordrehzahl von -128 ... 127
	int8_t speed_r = 0; // motordrehzahl


   
   while(1) 
   { 
		if( get_key_press( 1<<TASTE2)|| get_key_rpt( 1<<TASTE2 ))			// press Key 0:
			{
				speed_l++;
	
			}
		if( get_key_press( 1<<TASTE0))			// press Key 0:
			{
				speed_l ^=~(1<<8);
				speed_r ^=~(1<<8);
			}
		// repeat on long press:
		if( get_key_press( 1<<TASTE1) || get_key_rpt( 1<<TASTE1 ))			// long press Key 1:
			{
				speed_r++;	
			}
	
		motor_l(speed_l);
		motor_r(speed_r);
   } 
   return(0); 
}




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
	} else {			//bei 0 oder positiven, vorwärts
		PORTD |=(1<<PD2);
		PORTD &= ~(1<<PD3);
	}
	
	PWM_RECHTS = speed<<3; //10 bit für pwm und das 11. gegen die neagation
}

 
