/**    
* TASTE
* Dieses Unterprogramm soll alle Funktionen zur bediehnung der Tasten enthalten
*
* V0.1
* Tasten werden entprellt und zwischen tippen und gedrückthalten unterschieden
*
* V0.2
* Die Anpassung an die Pinkonfiguration wurde verbessert
* Initialisierung hinzugefügt
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "taste.h"

volatile char key_state, key_press, key_rpt;


void taste_init(void)
{
	/** TASTEN als Eingänge deffinieren plus pullup **/
	TASTE_DDR &=~((1<<TASTE0)|(1<<TASTE1)|(1<<TASTE2)); 	// Tasten als Eingänge
	TASTE_PORT |=(1<<TASTE0)|(1<<TASTE1)|(1<<TASTE2);		//pullup aktivieren
}

void tasten( void )
{
  static char ct0, ct1, rpt;
  char i;

  i = key_state ^ ~TASTE_PIN;	// key changed ?
  ct0 = ~( ct0 & i );			// reset or count ct0
  ct1 = (ct0 ^ ct1) & i;		// reset or count ct1
  i &= ct0 & ct1;				// count until roll over 
  key_state ^= i;				// then toggle debounced state
  key_press |= key_state & i;	// 0->1: key pressing detect

  if( (key_state & REPEAT_MASK) == 0 )	// check repeat function
    rpt = REPEAT_START;			// start delay
  if( --rpt == 0 ){
    rpt = REPEAT_NEXT;			// repeat delay
    key_rpt |= key_state & REPEAT_MASK;
  }
}



char get_key_press( char key_mask )
{
  cli();
  key_mask &= key_press;                        // read key(s)
  key_press ^= key_mask;                        // clear key(s)
  sei();
  return key_mask;
}
char get_key_rpt( char key_mask )
{
  cli();
  key_mask &= key_rpt;                        	// read key(s)
  key_rpt ^= key_mask;                        	// clear key(s)
  sei();
  return key_mask;
}
