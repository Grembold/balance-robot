#ifndef _TASTE_H
#define _TASTE_H 1

#define TASTE0 		PA0			// 
#define TASTE1 		PA1			//
#define TASTE2 		PA2			// 
#define KEY_INPUT	PINA		// Register für abfrage
#define REPEAT_MASK	(1<<PD1^1<<PD2)	// repeat: key 1, 2


#define REPEAT_START	125				// after 500ms
#define REPEAT_NEXT		25				// every 100ms

extern void tasten_init(void);
extern void tasten(void);
extern char get_key_press( char key_mask );
extern char get_key_rpt( char key_mask );

#endif
