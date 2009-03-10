/**  Header Datei für UART **/
#ifndef _UART_H
#define _UART_H 1

#define BAUD 		115200
#define UBRR_BAUD	5//(((F_CPU)/(BAUD*16))-1)



/* USART initialisieren */
extern void uart_init(void);			//Initialisierung
extern void uart_putc(char c);		//sendet ein Zeichen
extern void uart_puts(char* s);		//sendet ein String
extern void uart_puti(uint8_t* i);	//sendet ein integer V0.01
#endif

