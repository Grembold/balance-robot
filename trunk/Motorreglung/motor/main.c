/**    
* Motorstrom
* Dieses Programm soll den Motorstrom an einem Pin Messen und
* Über Uart ausgeben
*
* V0.1
* programm giebt Spannung und Strom über uart aus
*
* V0.2
* Die Referenzspannungwird auf 0.5V gesenkt und der AD-Wandler im Normalen
* Modus betrieben(nicht Gain 10x)
*
* V0.3
* Der Spannungsabfall am Shunt wird nun mit einem OP 11fach verstärkt.
* Die Referenzspannung wurde wieder auf interne 2.56V gesetz
* der A/D arbeitet mit 172KHz  und mittelt 8 Messungen
* die PWM arbeitet mit 21,6kHz und lässt sich mit 9Bit(max 01FF) einstellen
*
* V0.4
* Motorstrom wird nun in mA umgerechnet und PWM in prozent
* Die Referenzspannung des A/D-Wandlers ist nun die Versorgungsspannug Vcc (5V)
*
* V0.5
* Drehzahl der Motoren wird ausgegeben
*
* V0.6
* Eine einfache Odometrie wurde eigefügt. Nun kann die zurückgelegte 
* Strecke der Motoren ermittelt werden
*/

#include <avr/io.h> 	// I/O PINS des AVR
#include <util/delay.h>	// _delay_ms() bis max 200
#include <stdint.h> 	// integerwerte wie uint8_t, uint16_t, int8_t
#include <stdlib.h> 	// itoa() und utoa()

/** MOTOR	**/
#include "motor.h"

/** UART	**/
#include "uart.h"

/** timer	**/
#include "timer0.h"

/** Tasten	**/
#include "taste.h"

/** LEDs 	**/
#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED1 PB2	// LED2

/** Fahrgestell **/
#define GETRIEBE	90		// Übersetzung des aller Getriebe
#define RADUMFANG_R 312		// Radumfang eines Reifen

/** DEBUG **/
#define DEBUG 1


int main(void) 
{ 

	/** Motoren **/
	motor_init();   //motoren initialisieren
	motor_l(0);		//linker motor aus (keine bremse)
	motor_r(0);		//rechter motor aus
	
	volatile int16_t speed_l = 0;  // motordrehzahl von -6000 ... 6000
	volatile int16_t speed_r = 0;  // motordrehzahl von -6000 ... 6000
	int32_t	strecke_l = 0;			// zurückgelegte strecke in mm
	int32_t	strecke_r = 0;			// zurückgelegte strecke in mm
	
	/** UART  	**/
	uart_init();	
	
	/** Timer **/
	timer0_init();
	
	/** steuerung der Tasten **/
	taste_init();
	
	/** LEDs	**/
	LED_DDR |=(1<<LED1); // LED2 auf dem Bord
	LED_PORT |=(1<<LED1);// LED aus (low aktive)
	

	uint8_t sm = 0;		// state machine (verschiedene programmzustände)
	char buffer[10];	// String
	uint8_t alt, alt2, alt3, alt4, alt5;
	alt=alt2=alt3=alt4=alt5=0;
	
	
	_delay_ms(150);
	
	
	while(1) 
	{ 
		/** Überprüfen der Tasten **/
 		if( get_key_press( 1<<TASTE0))			// START/STOP
		{
			//LED_PORT ^=(1<<LED1);				//LED blinken, ist immer gut
			
			motor_l(0);							// linker motor aus (keine bremse)
			motor_r(0);							// rechter motor aus
			
			speed_l = 0; 						// motordrehzahl von -6000 ... 6000
			speed_r = 0; 						// motordrehzahl
			
			if (sm & 0x01) sm&=~0x01;			// Programmteil 1 (motor) starten/stoppen
			else sm |=0x01;
		}
		if( get_key_press( 1<<TASTE1))			// SEND UART
		{
			if (sm & 0x02) sm&=~0x02; 			// Programmteil 2 (UART) starten/stoppen
			else sm|=0x02;
		}
 		if( get_key_press( 1<<TASTE2))			// Vorwärts/Rückwärts
		{
			//LED_PORT ^=(1<<LED1);				//LED blinken, ist immer gut
			
			if (sm & 0x04) sm&=~0x04;			// Programmteil 1 (motor) starten/stoppen
			else sm |=0x04;
		}	
	
	
	
		/** Programmteil 1 **/
		/** Mit PWM rumspielen **/
		if((alt2!=(timer0_counter&128))&&sm & 0x01)
		{ 			
			alt2 = (timer0_counter&128);
			
			//LED_PORT ^=(1<<LED1);	// LED blinken, ist immer gut
			if(strecke_r < 2000) speed_r = 1000;
			if((strecke_r >= 2000) && (strecke_r < 6000)) speed_r = 2000; 
			if((strecke_r >= 6000) && (strecke_r < 11000)) speed_r = 5000;
			if((strecke_r >= 11000) && (strecke_r < 15000)) speed_r = 1000;
			if((strecke_r >= 15000) && (strecke_r < 20000)) speed_r = 2500;
			if((strecke_r >= 20000) && (strecke_r < 24000)) speed_r = 700;
			if((strecke_r >= 24000) && (strecke_r < 30000)) speed_r = 3500;
			if(strecke_r >= 30000) speed_r = 2200;
		}
		
		
		
		/** Programmteil 2 **/
		/** status über UARt ausgeben **/
		/* Timer0_counter hat sich geändert und sm2 ist aktiv */
		if((alt!=(timer0_counter&4))&&(sm & 0x02))		
		{
			alt = (timer0_counter&4);	
			//LED_PORT ^=(1<<LED1);	//LED blinken, ist immer gut
			
			uint16_t strom;
			uint16_t drehzahl;
			strom = get_Motorstrom(MOTOR_RECHTS_SHUNT);		// ermittelt dden Strom
			drehzahl= get_Motordrehzahl();					// ermittelt die Drehzahl
#ifndef DEBUG			
			uart_putc(0x0c);								// terminal clearsceen (bildschirm löschen)
			
			
			uart_puts("\r\n");
			uart_puts("Statusmeldung\t\trechter Motor\r\n\r\n");
			uart_puts("PWM   Drehzahl   Motorstrom\r\n ");
			
#endif			
			/* Gefahrene Strecke */
			ltoa(strecke_r, buffer, 10);
			uart_puts(buffer);
			uart_puts("\t");
			/* Wert der sollDrehzahl ausgeben */
			itoa(speed_r, buffer, 10);
			uart_puts(buffer);
			uart_puts("\t");
			/* istDrehzahl ausgeben */
			utoa(drehzahl, buffer, 10);
			uart_puts(buffer);
			uart_puts("\t");
			/* Strom ausgeben */
			utoa(strom, buffer, 10);
			uart_puts(buffer);
			uart_puts("\t");
			/* Stromsolwert ausgeben */
			utoa(sollstrom_rechts, buffer, 10);
			uart_puts(buffer);
			uart_puts("\r\n");
		}
		
		
		
		/** Stromregelung **/	
		if((alt3!=(timer0_counter&4)))		//alle 6ms
		{	
			alt3 = (timer0_counter&4);
			//LED_PORT ^=(1<<LED1);			// LED blinken, ist immer gut
			
			//stromreglung_rechts();
		}
		
		
		
		/** Drehzahlregelung **/
		if(alt4!=(timer0_counter&22))		// alle 33ms
		{
			alt4 = (timer0_counter&22);
			//LED_PORT ^=(1<<LED1);			// LED blinken, ist immer gut
			
			drehzahlregelung_rechts(speed_r);
		}
		
		
		
		/** Odometrie **/
		if(alt5!=(timer0_counter&128))		// alle 96ms
		{
			int32_t tmp;
			char buffer[11];
			alt5 = (timer0_counter&128);
			LED_PORT ^=(1<<LED1);			// LED blinken, ist immer gut
			
			tmp = get_Motordrehzahl();			
			tmp = (int32_t)(tmp * RADUMFANG_R * 473);
			tmp = (int32_t)(tmp / 13500000);
			strecke_r +=tmp;			
		}
   } 
   return(0); 
}


