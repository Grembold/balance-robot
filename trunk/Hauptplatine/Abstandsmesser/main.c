#include <avr/io.h> 	//I/O-Register 
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>		//standart Integertypen


#include "taste.h"		//Tastenendprellung
#include "adconvert.h"		//AD-Wandler
#include "uart.h"			//Uart



#define AD_PIN PA3
#define SENSORWERT_A 42436
#define SENSORWERT_B -20

#define TASTE0 PA0	//Enter
#define TASTE1 PA1	//blättern
#define MAXLENGTH 30
#define STATUS_LED PB3

#define time0 	4
#define tflag0 	0
#define time1 	30
#define tflag1 	1
#define time2	100
#define tflag2 	2

// EEMEM wird bei aktuellen Versionen der avr-lib in eerpom.h definiert
// hier: definiere falls noch nicht bekannt ("alte" avr-libc)
#ifndef EEMEM
// alle Textstellen EEMEM im Quellcode durch __attribute__ ... ersetzen
#define EEMEM  __attribute__ ((section (".eeprom")))
#endif


void wait_until_key_pressed(void);
uint16_t ReadChannel(uint8_t mux);
void timer0_init(void);
ISR(TIMER0_OVF_vect);


volatile uint8_t timer0_counter;
volatile uint8_t timerflag;




int main(void)
{
	DDRA &=~((1<<AD_PIN)|(1<<TASTE1)|(1<<TASTE0));
	PORTA |=(1<<TASTE1)|(1<<TASTE0);
	
		
	DDRB |=(1<<STATUS_LED);
	PORTB |=(1<<STATUS_LED);
	
	timer0_init();
	
	sei();
	
	for(;;)
	{
		uint16_t data;
		uint16_t abstand;
		char text[MAXLENGTH];

		if( get_key_press( 1<<TASTE0))			// press Key 0:
		{
			PORTB ^=(1<<STATUS_LED);
			

		}
		// repeat on long press:
		if( get_key_press( 1<<TASTE1)|| get_key_rpt( 1<<TASTE1 ))			// long press Key 1:
		{
			
			PORTB ^=(1<<STATUS_LED);
			
		}
		
		
		
		
		if(timerflag &(1<<tflag2))
		{
			
			/* liset den Aktuellen Wert am AD */
			data = ReadChannel(AD_PIN);

			
		
			abstand = SENSORWERT_A/(data-SENSORWERT_B);
			itoa(abstand, text,10);

			
		}
		
	// USART initialisieren 
	uart_init();
	
	sei();	//global Interrupts Aktivieren
	 
	uart_puts("\r ***         balance Bot             ***\r");
	uart_puts(  " ** Hier werden zu Debugzwecken       **\r");
	uart_puts(  " ** einige statusmeldungen ausgegeben **\r\r");
	
	uart_puts(" UART inizialisierung ... OK \r");
	// Nichts tun. Die Interrupts erledigten den Rest 
		
		
		
	}
	
	return 0;
}





/* Timer0 initialisieren */
void timer0_init(void)
{
	/* alle 6,35msEinstellen der Frequenz auf 157Hz ( Prescaler = 256  * 256 bis Überlauf ) */
	TCCR0 = (1<<CS02);

	/* Interrupts für Timer0 aktivieren */	
	TIMSK |= (1<<TOIE0);
}	

ISR(TIMER0_OVF_vect) /* veraltet: SIGNAL(SIG_OVERFLOW1) */
{
    /* Interrupt Code */
	timer0_counter++;
	if(timer0_counter % time0)
		timerflag &= ~(1<<tflag0);
	else
		timerflag |=(1<<tflag0);
	if(timer0_counter % time1)
		timerflag &= ~(1<<tflag1);
	else 
		timerflag |=(1<<tflag1);
	if(timer0_counter % time2)
		timerflag &= ~(1<<tflag2);
	else 
		timerflag |=(1<<tflag2);

	

	tasten();
}
