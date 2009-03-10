/**        
* Mit hilfe dieses Programmes soll die Drehzal 
* über ein UART interfase am PC ansgegeben werden.
* Dazu wird die Drehzal wie folgt ermittelt:
*
* Zeit(Timer0) zwischen zwei Impulsen
* 
* 
* Hierzu wird bei einen Externen Interrupt(INT0), 
* die bisher vergangene Zeit des Timers0 über Uart ausgegeben
*
**/

#include <avr/interrupt.h>
#include <stdint.h> // integerwerte wie uint8_t, uint16_t
#include <stdlib.h> // itoa
#include "timer0.h"	// einbinden der Header datei für Timer0
#include "uart.h"	// einbinden der Header datei für UART
//nicht vergessen die Passenden C Dateien in der Makefile zu schreiben

#define STATUS_LED1 PB3		// PIN der on-board Status-Led
#define STATUS_LED2 PB2		// Pin der on-board Status_Led


void init(void);			// allgemeine Initialisierung
void auswerten(void);		// Status Ausgabe über Uart

volatile uint16_t counter_alt;		// alter Timerstand
volatile uint16_t counter_neu;		// neuer Timerstand
volatile uint16_t time_delta;		// Zeitdifferens

uint16_t m1,m2,m3,m4,m5,m6,m7,m8,m9,m0; //Mittelwertbildung
uint8_t sm;
uint8_t alt;

int main(void){	
	
	init();			// Allgemeine Port initialisierung
	timer0_init();	// Timer0 initialisieren
	uart_init();	// UART initialisieren
	
	sei();	//global Interrupts Aktivieren
	
	uart_putc('*');			//senden eines einzelenen Zeichen
	uart_puts(" UART inizialisierung ... OK \n\r\n\r");	//sendet einen string
uart_puts("* Drehzahl1\r\n* Mit hilfe dieses Programmes soll die Drehzal \r\n");
uart_puts("* ueber ein UART interfase am PC ansgegeben werden.\r\n* Hierfuer wird die Drehzal wie folgt ermittelt. \r\n");
uart_puts("* Zeit(Timer0) zwischen zwei Impulsen\r\n");
uart_puts("* Hierzu wird alle durch einen Externen Interrupt(INT0) \r\n");
uart_puts("* Die Bisher vergangene Zeit des Timers0 über Uart ausgegeben\r\n\r\n");


	while(1)
	{	}
	//never reached   i hope ;)
}


void init(void)
{
	/* STATUS_LED1 und 2 als Ausgang  */
	DDRB |= (1 << STATUS_LED1) | (1 << STATUS_LED2);
	PORTB &= ~((1 << STATUS_LED1) | (1 << STATUS_LED2));

	/* INT0 init */	
	DDRD &= ~(1<<PD2);		//PORTD.2 als Eingang
	PORTD |= (1<<PD2);		//PORTD.2 mit Pullup
	GICR |= (1<<INT0);		// External Interrupt Request 0 Enable
	MCUCR |= (1<<ISC00)|(1<<ISC00);	// Die steigende Flanke an INT0 erzeugt einen Interrupt. 
	
	/* globale Variablen vorbelegen */
	counter_alt = 0; 		
	counter_neu = 0;
	m1=m2=m3=m4=m5=m6=m7=m8=m9=m0=0xffff;
	alt=0;
}

void auswerten(void)
{
	/* gibt Zählerstand über UART aus */
	unsigned char s[20]; 		// max -65536 entspricht 7 Zeichen
	uint32_t drehzahl;			// Drehzahl in U/min

	if(alt<4)
	{
		/* gleitender Mittelwert über 10 Messugen */
		m9=m8;m8=m7;m7=m6;m6=m5;m5=m4;m4=m3;m3=m2;m2=m1;m1=m0;m0=time_delta;
		drehzahl=(m1+m2+m3+m4+m5+m6+m7+m8+m9+m0)/10;	
	
		drehzahl=(uint32_t)drehzahl*256*2;	//
		if(drehzahl!=0)
			drehzahl=(11059000*60)/drehzahl;	//
	}
	else
	{
		m1=m2=m3=m4=m5=m6=m7=m8=m9=m0=0xffff;
		drehzahl=0;
	}
	
	
//	uart_puts("Drehzahl: ");	// Textausgabe
//	utoa(time_delta, s, 10);	// Uint_16 in Asci-String wandeln
//	uart_puts(s);				// Textausgabe	
//	uart_putc('\t');			// 
	ultoa(drehzahl, s, 10);		// Uint_16 in Asci-String wandeln
	uart_puts(s);				// Textausgabe	
	uart_puts("    \r\n");		// Textausgabe		
	
	if(alt<0xfe)	alt++;
}


// Interrupt Service Routine bei Flanke an int0(extern) */
ISR(INT0_vect)
{
	PORTB ^=(1<<PB2);								// Toggel Led, wildes blinken ist immer gut
	counter_neu = (timer0_counter_D1*256) + TCNT0;	// Merke mir die Uhrzeit
	//timer0_counter_D1=0;							// sezte den Umrundungsmerker zurück
	if(counter_neu > counter_alt){
		time_delta = counter_neu - counter_alt;}	// ermittel die Zeitdifferenz
	else{
		time_delta = (65535-counter_alt) + counter_neu;}	// ermittel die Zeitdifferenz
	
	counter_alt = counter_neu;						// Merke mir die Uhrzeit für die nächste Messung
	alt=0;
}