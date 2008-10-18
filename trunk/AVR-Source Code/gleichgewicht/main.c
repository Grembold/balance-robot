#include <avr/io.h>
#include <avr/interrupt.h>  
#include <util/delay.h>
#include <stdint.h> 		//integerwerte wie uint8_t, uint16_t
#include "timer0.h" 		// überprüft alle 4ms die Tasten


/** Motor 		**/
#include "motor.h"			//Motorensteuerung
/*****************/

/** Sensor	A	**/
#include "adconvert.h"			// AD-Wandler
#define SENSOR_A 	PA3			// Pin an den der vordere Sensor angeschlossen ist
#define SENSORA_A 	52436		// Umrechnungswert A
#define SENSORA_B 	-40			// Umrechnungswert B siehe kreatives-chaos.com
#define SENSORA_STEIGUNG	1	// Steigungs korrektur
#define	SENSORA_OFFSET		14	// Offset 
/*****************/

/** Sensor	B	**/
#include "adconvert.h"			// AD-Wandler
#define SENSOR_B 	PA4			// Pin an den der vordere Sensor angeschlossen ist
#define SENSORB_A 	52436		// Umrechnungswert A
#define SENSORB_B 	-40			// Umrechnungswert B siehe kreatives-chaos.com
#define SENSORB_STEIGUNG	1	// Steigungs korrektur
#define	SENSORB_OFFSET		14	// Offset siehe Exeldiagram
/*****************/

/** Tastatur 	**/
#include "taste.h"  		// Tastenentprellen und langdrücken
/*****************/

/** RS232		**/
#include "uart.h"			// Uart
/*****************/

/** LED			**/
#define LEDS_DDR DDRC		// Port für 8-LED
#define LEDS 	PORTC		// Port für 8-LED
#define STATUS_LED1 3		// PIN der on-board Status-Led
#define STATUS_LED2 2		// Pin der on-board Status_Led
/*****************/

int main(void) 
{ 
	/****** PIN I/O   ***********/
	DDRA &=~((1<<SENSOR_A)|(1<<SENSOR_B));	//Anschluss des Sensors als eingang definieren
	PORTA &=~(1<<SENSOR_B);
	LEDS_DDR = 0xff;	//LEDS zur Kippanzeige
	LEDS = 0xff;		//LEDS
	
	/******  Inits   ***********/
	tasten_init();
	timer0_init();
	uart_init();
	motor_init();   //motoren initialisieren

	// Motor startwerte
	motor_l(0);		//linker motor aus (keine bremse)
	motor_r(0);		//rechter motor aus
	int8_t speed_l = 0; // motordrehzahl von -128 ... 127
	int8_t speed_r = 0; // motordrehzahl
	
	/***** Variablen    ***********/
	// Sensor startwerte
	uint16_t data;							// Rohwert vom A/D Wandler
	uint16_t abstand_a, abstand_b;			// Umgerechneter Abstand
	int16_t differenz;
	int16_t tmp;
	int16_t	bla=30;
	
	// Text Variable
	char text1[15], text2[15];
	
	uint8_t start = 0;	//startet programm und stopt es auch wieder
	
	sei();				// Global Interrupts zulassen
	
	
	while(1) 
	{ 
		if( get_key_press( 1<<TASTE2))			// START/STOP
			{
				motor_init();   //motoren initialisieren
				motor_l(0);		//linker motor aus (keine bremse)
				motor_r(0);		//rechter motor aus
				
				speed_l = 0; // motordrehzahl von -128 ... 127
				speed_r = 0; // motordrehzahl
				
				if (start==0) start=1;
				else start=0;
			}
		if( get_key_press( 1<<TASTE0))			// START/STOP
			{
				bla ++;
			}
		if( get_key_press( 1<<TASTE1))			// START/STOP
			{
				bla --;
			}
			
		
// Sensor A auslesen
		data = ReadChannel(SENSOR_A); 		// liest den Aktuellen Wert am AD 	
		abstand_a = SENSORA_A/((data*SENSORA_STEIGUNG)-SENSORA_B)-SENSORA_OFFSET; //Rechnet den Wert in milimeter um
		itoa(abstand_a,text1,10);
		
// Sensor B auslesen
		data = ReadChannel(SENSOR_B); 		// liest den Aktuellen Wert am AD 	
		abstand_b = SENSORB_A/((data*SENSORB_STEIGUNG)-SENSORB_B)-SENSORB_OFFSET; // Rechnet den Wert in milimeter um
		//abstand_b= abstand_b +8;
		itoa(abstand_b,text2,10);
		
		differenz = abstand_a - abstand_b ;
		
		if(differenz >= bla) 
		{
			differenz = bla;
		} else 
			if(differenz <= -bla)
			{
				differenz= -bla;
			}
		if(differenz >= 45) 
		{
			differenz = 45;
		} else 
			if(differenz <= -45)
			{
				differenz=-45;
			}
		
		tmp = differenz * differenz;
			itoa(tmp,text1,10);
			uart_puts(text1);		// Text ausgabe
			uart_putc(';');
		tmp = (tmp * differenz)/2500;
			itoa(tmp,text1,10);
			uart_puts(text1);		// Text ausgabe
			uart_putc(';');
		tmp = tmp+(differenz*2);
			itoa(tmp,text1,10);
			uart_puts(text1);		// Text ausgabe
			uart_putc(';');			
		

		speed_l = tmp;
		speed_r = tmp;
		

		
		if(abstand_a > 1000){				// überschreitet der Wert 100cm
			PORTB &= ~(1<<STATUS_LED2);	// wird eine warn LED gesezt
		}			
		else {
							// Sensorabstand ausgeben
			PORTB |=(1<<STATUS_LED2);		// Warn Led löschen
		}
		
		
		// LED Kipp Anzeige bei 7cm
		if(abstand_a <=68){ 
			LEDS = 15;
		}
		if(abstand_a ==72){
			LEDS = 14;
		}
		if(abstand_a ==73){
			LEDS = 12;
		}
		if(abstand_a ==74){		
			LEDS = 8;
		}
		if(abstand_a ==75){// Mitte
			LEDS = 0;
		}
		if(abstand_a ==76){
			LEDS = 16;
		}
		if(abstand_a ==77){
			LEDS = 48;
		}
		if(abstand_a ==78){
			LEDS = 112;	
		}


		if(start){
			motor_l(speed_l);
			motor_r(speed_r);			
		}
		
		// UART Ausgabe
		uart_puts(text1);		// Sensorwert A
		uart_puts(";");			// Trennzeichen
		uart_puts(text2);		// Sensorwert B
		uart_puts(";");			// Trennzeichen
		itoa(speed_l,text1,10);
		uart_puts(text1);		// Text ausgabe
		uart_putc('\t');

		itoa(differenz,text1,10);
		uart_puts(text1);		// Text ausgabe

		uart_putc('\r');

	} 
	return(0); 
}
