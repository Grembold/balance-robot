/*************************************************************************/
/*!
* \file main.c
*
* \brief 
*
* \par Funktionsweise: programm zum testen der UART kommunication
*	
* \version	V--- - 26.03.2007 - Olaf Petersen\n
*			Created\n
*************************************************************************/

#include <avr/io.h> 	//I/O-Register 
#include <stdint.h>		//standart Integertypen
#include <stdlib.h> 	// itoa
#include <avr/interrupt.h>


#include "../nbot_lib/uart.h"
#include "../nbot_lib/timer.h"
#include "pcc.h"



int main(void)
{
	uart_init();
	timer2_init();
	
	uint32_t zeit = 0;
	
	uint16_t x;
	uint16_t y;
	
	sei();
	for(;;)
	{
		if (Gettime() >= (zeit + 100))		// alle 100ms
		{	
			pcc(uart_getc());		// PC Kommunikation
			x = Gettime();
			
			y = x*x;
			
			// encoder Links
			uart_putc(10);
			uart_putc(x>>8);
			uart_putc(x);
			// encoder Rechts
			uart_putc(16);
			uart_putc((y+5)>>8);
			uart_putc(y+5);
			
		}
	}
	return 0;
}

