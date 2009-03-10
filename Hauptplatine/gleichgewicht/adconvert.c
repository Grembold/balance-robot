/***
**		Routine zum auslesen eines Einzelnen Wertes			**
***/

#include <avr/io.h>
#include <stdint.h>		//standart Integertypen
#include "adconvert.h"

uint16_t ReadChannel(uint8_t mux)
{
  uint8_t i;
  uint16_t result = 0;         		//Initialisieren wichtig, da lokale Variablen
									//nicht automatisch initialisiert werden und
									//zufällige Werte haben. Sonst kann Quatsch rauskommen
  ADCSRA = (1<<ADEN) | AD_PRE;    	// Frequenzvorteiler 
									// setzen auf 8 (1) und ADC aktivieren (1)

  ADMUX = mux;                      // Kanal waehlen
  ADMUX |= AD_REF; 					// interne Referenzspannung nutzen 

  /* nach Aktivieren des ADC wird ein "Dummy-Readout" empfohlen, man liest
     also einen Wert und verwirft diesen, um den ADC "warmlaufen zu lassen" */
  ADCSRA |= (1<<ADSC);				// eine ADC-Wandlung 
  while ( ADCSRA & (1<<ADSC) );	// auf Abschluss der Konvertierung warten 

  /* Eigentliche Messung - Mittelwert aus 4 aufeinanderfolgenden Wandlungen */
  for(i=0;i<MITTELWERT;i++)
  {
    ADCSRA |= (1<<ADSC);         	// eine Wandlung "single conversion"
    while ( ADCSRA & (1<<ADSC) );	// auf Abschluss der Konvertierung warten
    result += ADCW;		    		// Wandlungsergebnisse aufaddieren
  }
  ADCSRA &= ~(1<<ADEN); 			// ADC deaktivieren (2)

  result /= MITTELWERT;        		// Summe durch acht teilen = arithm. Mittelwert

  return result;
}
