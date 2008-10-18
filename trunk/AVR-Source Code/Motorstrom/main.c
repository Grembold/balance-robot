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
*/

#define VERSION_HIGH '0'
#define VERSION_LOW '1'



#include <avr/io.h> 	//I/O-Register 
#include <stdint.h>		//standart Integertypen
#include <stdlib.h> 	// itoa
#include <avr/interrupt.h>

#include "motorstrom.h"
#include "uart.h"

#define AD_PIN0 0x09
#define AD_PIN1 0x0D



uint16_t get_adc(uint8_t mux);

int main(void)
{
	uart_init();
	
	//LED Ausgabe
	DDRC = 0xff;
	PORTC = 0x00;

	sei();
	
	uart_puts("\r\nMotorstrom\r\n");
	uart_puts(" Programm zum Messen des Motorstoms \r\n");
	uart_puts("Verion ");
	uart_putc(VERSION_HIGH);
	uart_putc('.');
	uart_putc(VERSION_LOW);
	uart_puts("\r\n\r\n");
	
	for(;;)
	{
		uint16_t I_Motor1;
		uint16_t I_Motor2;
		char text[10];	// für 16bit zahlen
		

		I_Motor1 = get_Motorstrom(AD_PIN0);

		uart_puts("Motorstrom 1: ");
		/* convert interger into string */
		itoa( I_Motor1 , text, 10);
		uart_puts(text);
		uart_puts(" mA   \r");
	}
	
	return 0;
}

