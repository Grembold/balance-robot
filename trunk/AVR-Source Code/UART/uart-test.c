#include <avr/interrupt.h>
#include "uart.h"

int main(void)
{	
	// USART initialisieren 
	uart_init();
	
	sei();	//global Interrupts Aktivieren
	 
	uart_puts("\r ***         balance Bot             ***\r");
	uart_puts(  " ** Hier werden zu Debugzwecken       **\r");
	uart_puts(  " ** einige statusmeldungen ausgegeben **\r\r");
	
	uart_puts(" UART inizialisierung ... OK \r");
	// Nichts tun. Die Interrupts erledigten den Rest 
	while (1)
	{	
	}
}
