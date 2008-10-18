#ifndef _TASTE_H
#define _TASTE_H 1

#define KEY_INPUT		PINA			/*!< Port der Tasten */
#define REPEAT_MASK		(1<<PD1^1<<PD2)	/*!< Tasten die bei längerem drücken wiederholt werden: Taste 1, 2 */

#define REPEAT_START	125				/*!< Verzögerung 	500ms */
#define REPEAT_NEXT		25				/*!< Wiederholrate 	100ms */


/*** Funktions prototypen */
extern void tasten(void);
extern char get_key_press( char key_mask );
extern char get_key_rpt( char key_mask );
#endif

