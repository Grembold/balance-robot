#include <avr/io.h>
#inclunde "taste.h"

volatile char key_state, key_press, key_rpt;


void tasten( void )
{
  static char ct0, ct1, rpt;
  char i;

  i = key_state ^ ~KEY_INPUT;	// key changed ?
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
