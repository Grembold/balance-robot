#include <avr/io.h> 	//I/O-Register 
#include <stdint.h>		//standart Integertypen
#include <avr/interrupt.h>

#include "adconvert.h"

#define ADC_PIN 3

int main(void)
{
	//LED Ausgabe
	DDRC = 0xff;
	PORTC = 0x00;
	
	// Status LEDs
	DDRB |=(1<<PB3)|(1<<PB2);
	PORTB |=(1<<PB3)|(1<<PB2);
	
	//Pins für Taste und ADC
	DDRA &=~((1<<PA0)|(1<<PA3));
	PORTA = (1<<PA0);
		
	// ADwandler initialisieren

	sei();	//global interrupts aktivieren
	
	for(;;)
	{
		
		PORTC = (uint8_t)get_adc(ADC_PIN); //messwert auf den LEDs anzeigen

	}
 	
}

/***/
/**		Routine zum auslesen eines einzelnen A/D-Wertes	*/		
/***/
uint16_t get_adc(uint8_t adc_pin)
{
  uint8_t i;
  uint16_t result = 0;         		//Initialisieren wichtig, da lokale Variablen
																	//nicht automatisch initialisiert werden und
																	//zufällige Werte haben. Sonst kann Quatsch rauskommen
  ADCSRA = (1<<ADEN) | AD_PRE;    	// Frequenzvorteiler 
																	// setzen auf 8 (1) und ADC aktivieren (1)

  ADMUX = adc_pin;                      // Kanal waehlen
  ADMUX |= AD_REF; 					// interne Referenzspannung nutzen 

  /* nach Aktivieren des ADC wird ein "Dummy-Readout" empfohlen, man liest
     also einen Wert und verwirft diesen, um den ADC "warmlaufen zu lassen" */
  ADCSRA |= (1<<ADSC);				// eine ADC-Wandlung 
  while ( ADCSRA & (1<<ADSC) );	// auf Abschluss der Konvertierung warten 

  /* Eigentliche Messung: Mittelwertbildung aus MITTELWERT aufeinanderfolgenden Wandlungen */
  for(i=0;i<MITTELWERT;i++)
  {
    ADCSRA |= (1<<ADSC);         	// eine Wandlung "single conversion"
    while ( ADCSRA & (1<<ADSC) );	// auf Abschluss der Konvertierung warten
    result += ADCH;		    		// Wandlungsergebnisse aufaddieren  result += ADCW;
  }
  ADCSRA &= ~(1<<ADEN); 			// ADC deaktivieren (2)

  result /= MITTELWERT;        		// Summe durch MITTELWERT teilen = arithm. Mittelwert

  return result;
	
}

