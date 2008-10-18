/** Tasten Entprellen und Timer **/


#include <avr/io.h>
#include <avr/interrupt.h>

#define LED_OUTPUT  PORTD

#define KEY_INPUT	PINA

#define REPEAT_MASK		(1<<PD1^1<<PD2)	// repeat: key 1, 2
#define REPEAT_START	125				// after 500ms
#define REPEAT_NEXT		25				// every 100ms


volatile char key_state, key_press, key_rpt;
volatile uint8_t timer0_counter;


void timer0_init(void); 	//timer0 inittialisiere
void tasten(void);


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



int main(void)
{
	//hier ist eine LED ausgabe dran 
	DDRD=0xff;
	PORTD = 0x00;
	
	DDRB |=(1<<PB3)|(1<<PB2);
	PORTB |=((1<<PB3)|(1<<PB2));
	
	DDRC = 0xff;
	PORTC = 0x00;
	
	DDRA = 0x00; //alle eingang
	DDRA &=~((1<<PA0)|(1<<PA1)|(1<<PA2));
	PORTA |=(1<<PA0)|(1<<PA1)|(1<<PA2);
	
		
	timer0_counter = 0x00;
	timer0_init();
	
	
	sei();
	
	for(;;)
	{
	
		// single press:

    if( get_key_press( 1<<PA0 ))		// Key 0:
	{
      LED_OUTPUT ^= 1<<PD0;			// toggle LED 0	an
	  PORTB ^=1<<PB2;
	}
	  

		// single long press:

    if( get_key_rpt( 1<<PA1 )			// long press key 1
      && get_key_press( 1<<PA1 ))		// after short press:
      LED_OUTPUT ^= 1<<PD1;			// toggle LED 1


		// repeat on long press:

    if( get_key_press( 1<<PA2 )			// Key 2 or
      || get_key_rpt( 1<<PA2 ))			// long press Key 2:
      LED_OUTPUT++;				// LEDs count up


	}
	return 0;
}

/* Timer0 initialisieren */
void timer0_init(void)
{
	/* alle 6,35msEinstellen der Frequenz auf 157Hz ( Prescaler = 256  * 256 bis Überlauf ) */
	TCCR0 = (1<<CS02);

	/* Interrupts für Timer0 aktivieren */	
	TIMSK |= (1<<TOIE0);
}	

ISR(TIMER0_OVF_vect) /* veraltet: SIGNAL(SIG_OVERFLOW1) */
{
    /* Interrupt Code */
	timer0_counter++;
	if(timer0_counter == 84){
		timer0_counter = 0;
		PORTB ^=(1<<PB3);
	}
	tasten();
}


