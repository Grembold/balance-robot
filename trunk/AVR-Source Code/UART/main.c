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


//#include "../nbot_lib/encoder.h"
#include "../nbot_lib/uart.h"
#include "../nbot_lib/timer.h"
//#include "../nbot_lib/motor.h"
//#include "../nbot_lib/command.h"


int main(void)
{
	uart_init();
	timer2_init();
	
	uint32_t zeit = 0;
	
	//uart_puts("\f *** Hallo! ***\r\n");
	//uart_puts(" Ich bin ein kleiner Roboter. Was moechtest du mit mir machen?\r\n");
	//uart_puts("BOT:\\>");	
	
	uint16_t x;
	uint16_t y;
	

	sei();	

	for(;;)
	{
		if (Gettime() > (zeit + 100))		// alle 100ms
		{	
			zeit = Gettime ();
			
			x = Gettime();
			y = x*x;
			// encoder Links
			uart_putc(0x52);
			uart_putc(x<<8);
			uart_putc(x);
			// encoder Rechts
			uart_putc((16<<3)+2);
			uart_putc(y<<8);
			uart_putc(y);
			
		}
		x=0;
		y=0;
	}
	return 0;
}

