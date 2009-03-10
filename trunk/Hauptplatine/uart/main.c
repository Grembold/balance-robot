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

#include "../nbot_lib/encoder.h"
#include "../nbot_lib/uart.h"
#include "../nbot_lib/timer.h"
#include "../nbot_lib/motor.h"
#include "../nbot_lib/command.h"


int main(void)
{
	uart_init();
	timer2_init();
	
	uart_puts("\f *** Hallo! ***\r\n");
	uart_puts(" Ich bin ein kleiner Roboter. Was moechtest du mit mir machen?\r\n");
	uart_puts("BOT:\\>");	
	
	uint8_t uart_buf;				// von Uart abgefragtes Zeichen	
	uint8_t command;
	
	sei();
	for(;;)
	{
		uart_buf = uart_getc();						// tastenabfrage
		command_put(uart_buf);						// fügt der Komandofunktion ein neues Zeichen hinzu
		command = command_get();					// gibt letzten Befehl zurück
		command_prompt(command);					// Textausgabe der Befehle
		switch (command)
		{
			case CMD_MOVE:
			// Hier kann ich was machen
			break;
		}
		msleep(2);
	}
	return 0;
}

