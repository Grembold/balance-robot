#include <avr/io.h> 	// I/O-Register 
#include <avr/interrupt.h>
#include <string.h>
#include <stdint.h>		// standart Integertypen


#include "taste.h"			// Tastenendprellung
#include "adconvert.h"		// AD-Wandler
#include "uart.h"			// Uart


// Sensor
#define AD_PIN PA3			// Pin an den der Sensor angeschlossen ist
#define SENSORWERT_A 37725	// Umrechnungswert A
#define SENSORWERT_B 24		// Umrechnungswert B siehe kreatives-chaos.com

#define TASTE0 PA0			// Pin von Taste0
#define TASTE1 PA1			// Pin von Taste1
#define STATUS_LED1 3		// PIN der on-board Status-Led
#define STATUS_LED2 2		// Pin der on-board Status_Led
#define LEDS_DDR DDRC		// Port für 8-LED
#define LEDS 	PORTC		// Port für 8-LED
#define MAXLENGTH 30		// maximale Länge einer String Variablen

#define time0 	4			// Flagge wird alle 4ms ausgeführt (Tasten entprellung)
#define tflag0 	0			// nummer der Flagge
#define time1 	30			// flagge wird alle 30ms gesetzt
#define tflag1 	1			// nummer der Flagge
#define time2	20			// flagge wird alle 20ms gesetzt
#define tflag2 	2			// nummer der Flagge

/** Funktion Prototypen  **/
uint16_t ReadChannel(uint8_t mux);	// Liest einen analogwert eines A/D Chanals aus
void timer0_init(void);			// Initialisiert die Timerfunktionen
ISR(TIMER0_OVF_vect);				// Interrupt Service Routine für den Timer0 überlauf


volatile uint8_t timer0_counter;	// Zähler für die Timer0 überläufe
volatile uint8_t timerflag;		// enthält die Timerflaggen




int main(void)
{
	DDRA &=~((1<<AD_PIN)|(1<<TASTE1)|(1<<TASTE0));	// Setzt die Pins als Eingang
	PORTA |=(1<<TASTE1)|(1<<TASTE0);	// setzt bei den Tasten den pullup Widerstand
	
	DDRB |=(1<<STATUS_LED1)|(1<<STATUS_LED2);			// Status LEDs als Ausgang definieren
	PORTB |=(1<<STATUS_LED1)|(1<<STATUS_LED2);			// status LEDs Auschalten ('1')
	
	LEDS_DDR = 0xff;	//LEDS zur Kippanzeige
	LEDS = 0xff;			//LEDS
	
	timer0_init();		// Timer initialisieren
	uart_init(); 		// USART initialisieren 
	
	sei();				// Global Interrupts zulassen
	
	for(;;)			// entlosschleife für das Hauptprogramm
	{
		uint16_t data;							// Rohwert vom A/D Wandler
		uint16_t abstand;						// Umgerechneter Abstand
		char text[MAXLENGTH];					// String   Achtung mit vorsicht verwenden

		// eimaliger tastendruck
		if( get_key_press( 1<<TASTE0))			// Taste0 wurde gedrückt
		{
			PORTB ^=(1<<STATUS_LED1);			// Toggle LED
		}
		
		// wiederholung bei langem drücken der Taste1:
		if( get_key_press( 1<<TASTE1)|| get_key_rpt( 1<<TASTE1 ))	// Taste1 wurde lange gedrückt:
		{
			PORTB ^=(1<<STATUS_LED1);			// Toggle LED
		}
		
		
		if(timerflag &(1<<tflag2))		//wird alle 30ms ausgeführt
		{
			
			
			data = ReadChannel(AD_PIN); 		// liest den Aktuellen Wert am AD 	

			abstand = SENSORWERT_A/(data-SENSORWERT_B); //Rechnet den Wert in einen Abstand um
			itoa(abstand, text,10);				//nach String convertieren
			
			if(abstand > 1000){				//überschreitet der Wert 100cm
				uart_puts("NA");				//wird NA ausgegeben
				PORTB &= ~(1<<STATUS_LED2);	//wird eine warn LED gesezt
			}
			
			else {
				uart_puts(text);				//Sensorabstand ausgeben
				PORTB |=(1<<STATUS_LED2);		//Warn Led löschen
			}
			
			if(abstand <=47) LEDS = 15;		//LED Kipp Anzeige bei 5cm
			if(abstand ==48) LEDS = 14;
			if(abstand ==49) LEDS = 12;
			if(abstand ==50) LEDS = 8;
			if(abstand ==51) LEDS = 16;
			if(abstand ==52) LEDS = 48;
			if(abstand ==53) LEDS = 112;
			if(abstand >=54) LEDS = 240;			
		}
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
    /* Setzt die einzelnen Timerflags um Funktionen mit */
	/* unterschiedlichen Periodendauern auszuführen		*/
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

	
	tasten();	//tasten entprellen, vorzugsweise alle etwas 4ms
}
