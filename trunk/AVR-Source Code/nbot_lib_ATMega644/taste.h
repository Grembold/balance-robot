/**************************************************************************/
/*!
* \file taste.h
* 
* \brief Definitionen für die Tastenentprellung
***************************************************************************/

#ifndef _TASTE_H
#define _TASTE_H


#define KEY_INPUT		PINA			/*!< PORT an dem die Tastenzusände überprüft werden */
#define REPEAT_MASK		(1<<PD1^1<<PD2)	/*!< Tasten die  widerholt werden sollen: key 1, 2 */

#define REPEAT_START	75				/*!< nach 125 Aufrufen der taste() Funktion */
#define REPEAT_NEXT		10				/*!< nach 25 Aufrufen der taste() Funktion */


extern void tasten(void);
extern char get_key_press( char key_mask );
extern char get_key_rpt( char key_mask );

#endif
