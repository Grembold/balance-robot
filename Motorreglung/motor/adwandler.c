/**    
* AD Wandler
* Dieses Programm enthält funktionen zu auslesen der AD-Wandler
*
* V0.1
*/
#include <avr/io.h> 	//I/O-Register 
#include <stdint.h>		//standart Integertypen

#include "adwandler.h"

/***/
/**		Routine zum auslesen eines einzelnen A/D-Wertes	*/		
/***/
uint16_t get_adc(uint8_t adc_pin)
{
  uint8_t i;
  uint32_t result = 0;         		//Initialisieren wichtig, da lokale Variablen
									//nicht automatisch initialisiert werden und
									//zufällige Werte haben. Sonst kann Quatsch rauskommen
									
  ADCSRA = (1<<ADEN) | AD_PRE;    	// Frequenzvorteiler 
									// setzen auf 8 (1) und ADC aktivieren (1)

  ADMUX = adc_pin;                  // Kanal waehlen
  ADMUX |= AD_REF; 					// Referenzspannung nutzen 
  //ADMUX |=(1<<ADLAR);				// Werte linksbündig schreiben
  
  /* nach Aktivieren des ADC wird ein "Dummy-Readout" empfohlen, man liest
     also einen Wert und verwirft diesen, um den ADC "warmlaufen zu lassen" */
  ADCSRA |= (1<<ADSC);				// eine ADC-Wandlung 
  while ( ADCSRA & (1<<ADSC) );	// auf Abschluss der Konvertierung warten 

  /* Eigentliche Messung: Mittelwertbildung aus MITTELWERT aufeinanderfolgenden Wandlungen */
  for(i=0;i<MITTELWERT;i++)
  {
    ADCSRA |= (1<<ADSC);         	// eine Wandlung "single conversion"
    while ( ADCSRA & (1<<ADSC) );	// auf Abschluss der Konvertierung warten
    result += ADCW;		    		// Wandlungsergebnisse aufaddieren
  }
  ADCSRA &= ~(1<<ADEN); 			// ADC deaktivieren (2)

  result /= MITTELWERT;        		// Summe durch MITTELWERT teilen = arithm. Mittelwert

  return result;	
}

