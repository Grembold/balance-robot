#include <avr/io.h> 	//I/O-Register 
#include <stdint.h>		//standart Integertypen
#include <avr/interrupt.h>

#include "wintek2704.h" //LCD Routinen


#define AD_PIN PA3
#define TASTE PA0
#define MAXLENGTH 30

void wait_until_key_pressed(void);
uint16_t ReadChannel(uint8_t mux);

int main(void)
{
	DDRA &=~((1<<AD_PIN)|(1<<TASTE));
	PORTA |=(1<<TASTE);
	
	lcd_init(LCD_DISP_ON);
	sei();
	
	for(;;)
	{
		uint16_t data;
		char text[MAXLENGTH];
		
		/* liset den Aktuellen Wert am AD */
		data = ReadChannel(AD_PIN);
        
		/* convert interger into string */
        itoa( (data>>2) , text, 10);
		
		lcd_puts(text);
		
		/* wait until push button PD2 (INT0) is pressed */
        wait_until_key_pressed();
		
		lcd_clrscr();
	}
	
	return 0;
}


void wait_until_key_pressed(void)
{
    unsigned char temp1, temp2;
    unsigned int i;
    
    do {
        temp1 = PINA;                  // read input
        for(i=0;i<65535;i++);
        temp2 = PINA;                  // read input
        temp1 = (temp1 & temp2);       // debounce input
    } while ( temp1 & _BV(PINA0) );
    
    loop_until_bit_is_set(PINA,PINA0);            /* wait until key is released */
}

uint16_t ReadChannel(uint8_t mux)
{
  uint8_t i;
  uint16_t result = 0;         //Initialisieren wichtig, da lokale Variablen
                               //nicht automatisch initialisiert werden und
                               //zufällige Werte haben. Sonst kann Quatsch rauskommen
  ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);    // Frequenzvorteiler 
                               // setzen auf 8 (1) und ADC aktivieren (1)

  ADMUX = mux;                      // Kanal waehlen
  ADMUX |= (1<<REFS0); // interne Referenzspannung nutzen 

  /* nach Aktivieren des ADC wird ein "Dummy-Readout" empfohlen, man liest
     also einen Wert und verwirft diesen, um den ADC "warmlaufen zu lassen" */
  ADCSRA |= (1<<ADSC);              // eine ADC-Wandlung 
  while ( ADCSRA & (1<<ADSC) );     // auf Abschluss der Konvertierung warten 
  result += ADCW;
  
  /* Eigentliche Messung - Mittelwert aus 4 aufeinanderfolgenden Wandlungen */
  for(i=0;i<4;i++)
  {
    ADCSRA |= (1<<ADSC);            // eine Wandlung "single conversion"
    while ( ADCSRA & (1<<ADSC) );   // auf Abschluss der Konvertierung warten
    result += ADCW;		    // Wandlungsergebnisse aufaddieren
  }
  ADCSRA &= ~(1<<ADEN);             // ADC deaktivieren (2)

  result /= 4;                     // Summe durch vier teilen = arithm. Mittelwert

  return result;
}
