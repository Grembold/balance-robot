/****************************************************************************/
/*!
* \file     taste.c
*
* \brief    Bis zu 8 Tasten entprellen, mit gedr�ckt halten und wiederholungs 
*			Funktion.
*
*			Es werden 4 gleiche Zust�nde hintereinander ben�tigt, um als
*			g�ltiges Dr�cken oder Loslassen erkannt zu werden.
*			Dadurch hat sich die Ausf�hrungszeit von 10 auf 12 Zyklen verl�ngert.
* 
* \par  Funktionsweise:
*           Die Bits der Register CT0, CT1 bilden f�r jede Taste einen Z�hler 0..3.
*			Ist der Tastenzustand mit dem entprellten Zustand identisch, wird dieser
*			2-Bit Z�hler permanent zur�ckgesetzt.\n\n
*			Nur wenn also 4-mal hintereinander der entgegengesetzte Tastenpegel
*			anliegt, kann dieser Z�hler einmal rum z�hlen (= 4 Zust�nde 0..3) und
*			wenn dann immer noch der entgegengesetzte Tastenpegel anliegt, wird er
*			von dem entprellten Register (key_state) �bernommen.\n
*			Da die Tasten low-activ sind, bedeutet dort eine 1 = Taste gedr�ckt.
*
*
* \see      timer.c
*
* \version  V001 - 31.12.2005 - Peter Dannegger\n
*			Created http://www.mikrocontroller.net/attachment.php/252480/C_TAST.C\n
* \version 	V001 - 25.02.2007 - Olaf Petersen\n
*			Dokumentiert\n
*			Global Interrupt Flag wird nun auf den Urspr�nglichen Wert gestzt\n
*
* \par Beispiel:
* (Nur zur Demonstration der Parameter/Returnwerte)
* \code 
* #define TASTE0 PA0	// PiN der Taste 0
* #define TASTE1 PA1	// PiN der Taste 1
* #define TASTE2 PA2	// PiN der Taste 2	
*
* int main(void) 
* { 
*   DDRA &=~((1<<TASTE0)|(1<<TASTE1)|(1<<TASTE2));  // Tasten als Eing�nge
*   PORTA |=(1<<TASTE0)|(1<<TASTE1)|(1<<TASTE2);    // pullup aktivieren
*   timer0_init();                                  // timer initialisieren
*
*   while(1) 
*   { 
*     // druecke Taste 0
*     if( get_key_press( 1<<TAST0))
*     {
*       //TO DO
*     }
*     // repeat on long press:
*     if( get_key_press( 1<<TASTE1) || get_key_rpt( 1<<TASTE1 ))
*     {
*       //TO DO
*     }
*   }
* }
* \endcode
*
*****************************************************************************/

#include <avr/io.h>
#include "taste.h"

volatile char key_state;
volatile char key_press;
volatile char key_rpt;


/****************************************************************************/
/*!
  \brief	Tasten auf zusandswechsel �berpr�fen
			Die Funktion tasten() sollte etwas alle 4ms aufgerufen werden.

  \par Funktionsweise:
			Alle Tasten am PORT KEY_INPUT, die nach viermaligem Aufruf den 
			gleichen Zustand 0(LOW aktiv) haben, werden als key_press makiert. 
  
  \param
  keine

  \return
  nichts
*****************************************************************************/
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


/****************************************************************************/
/*!
  \brief	�berpr�ft, ob eine Taste bet�tigt worden ist.\n
			Jede gedr�ckte Taste wird nur einmal gemeldet.
  
  \param
  key_mask	Maske der zu �berpr�fenden Tasten.

  \return
  Wurde die Makierte Taste bet�tigt steht an entsprechender Stelle 
			eine 1.
*****************************************************************************/
char get_key_press( char key_mask )
{
  uint8_t tmp_sreg;
  tmp_sreg = SREG;
  cli();
  key_mask &= key_press;                        // read key(s)
  key_press ^= key_mask;                        // clear key(s)
  SREG = tmp_sreg;
  
  return key_mask;
}


/****************************************************************************/
/*!
  \brief	�berpr�ft, ob eine Taste lang genug bet�tigt worden ist, da� die
			tastenwiederholungs Funktion eintritt. Nach einer kleinen 
			Verz�gerung wird die gedr�ckte Taste regelm�ssig als erneut 
			bet�tigt Makiert.
			Diese simuliert dem Benutzer ein wiederholendes dr�cken und 
			loslassen der Taste.
  
  \param
  key_mask	Maske der zu �berpr�fenden Tasten.

  \return
  Wurde die Makierte Taste  lange bet�tigt steht an entsprechender Stelle 
			eine 1.
*****************************************************************************/
char get_key_rpt( char key_mask )
{
  uint8_t tmp_sreg;
  tmp_sreg = SREG;
  cli();
  key_mask &= key_rpt;                        	// read key(s)
  key_rpt ^= key_mask;                        	// clear key(s)
  SREG = tmp_sreg;
  return key_mask;
}
