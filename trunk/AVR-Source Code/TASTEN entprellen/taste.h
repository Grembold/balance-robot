#ifndef _TASTE_H
#define _TASTE_H

#define KEY_INPUT	PINA
#define REPEAT_MASK		(1<<PD1^1<<PD2)	// repeat: key 1, 2


#define REPEAT_START	125				// after 500ms
#define REPEAT_NEXT		25				// every 100ms


extern void tasten(void);
extern char get_key_press( char key_mask );
extern char get_key_rpt( char key_mask );

#endif