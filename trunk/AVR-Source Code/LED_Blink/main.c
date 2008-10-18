#include <avr/io.h> 	//I/O-Register 
#include <stdint.h>		//standart Integertypen
#include <avr/interrupt.h>
#include <util/delay.h>
#define ADC_PIN 3


void long_delay(uint16_t ms) {
    for(; ms>0; ms--) _delay_ms(1);
}


int main(void)
{
	//LED Ausgabe
	DDRC = 0xFF;
	PORTC = 0x00;

	
	for(;;)
	{
		
		PORTC++;//(uint8_t)get_adc(ADC_PIN); //messwert auf den LEDs anzeigen
		//_delay_ms(100);
		long_delay(1000);
	}
 	
}



