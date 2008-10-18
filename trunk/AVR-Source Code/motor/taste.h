#ifndef _TASTE_H
#define _TASTE_H 1

#define TASTE_DDR DDRA
#define TASTE_PORT	PORTA
#define TASTE_PIN	PINA
#define TASTE0 PA0
#define TASTE1 PA1
#define TASTE2 PA2
#define REPEAT_MASK		(1<<TASTE0^1<<TASTE1)	// repeat: key 1, 2


#define REPEAT_START	125			// after 500ms
#define REPEAT_NEXT		25			// every 100ms

extern void taste_init(void); 		// Initialisiert die Tasten
extern void tasten(void);		// Tasten abrufen, vom Timer alle 4ms
extern char get_key_press( char key_mask );  	// Taste gedrückt
extern char get_key_rpt( char key_mask );		// Taste gehalten

#endif
