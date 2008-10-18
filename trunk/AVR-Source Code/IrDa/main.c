#include <avr/io.h> 	//I/O-Register 
#include <stdint.h>		//standart Integertypen
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart.h"

void long_delay(uint16_t ms) {
    for(; ms>0; ms--) _delay_ms(1);
}

int main(void)
{
	uint8_t test;
	
	//LED Ausgabe
	DDRC = 0xFF;
	PORTC = 0xAA;
	
	
	//uart_init();
 /*

sei();


 
	for(;;){
	if(uart_getc (&test)==1){
			//long_delay(1000);
			uart_putc(test);
	}

	}
	*/
	DDRD = 0x00;
	PORTD = 0x00;
}



