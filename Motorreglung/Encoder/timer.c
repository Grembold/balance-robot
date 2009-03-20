/****************************************************************************/
/*!
* \file 	timer.c
*
* \brief    Zeit-, Delay- und Entprell Funktionen.
*
*           Erzeugt Zeiteinheiten anhand des laufenden Timer2-Interrupts.\n
*           Dieser ist auf 36kHz eingestellt. \n
*           Die globalen Variablen \b count36kHz und \b timebase werden hier
*           bearbeitet.\n
*           In den hier vorhandenen Funktionen werden diese Variablen genutzt\n
*           um Zeiten und Verzoegerungen zu erzeugen.
*			Für die Entprellung ist die taste.c notwendig
*
* \see      taste.c
*
* \version  V001 - 25.02.2007 - Olaf Petersen\n
*           first build\n
*****************************************************************************/

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer.h"

#define baseTimeValue (255UL-(F_CPU/36000UL))
/****************************************************************************/
/*!
  \brief
  Initialiseiert Timer2

  Timer2 löst alle 36kHz ein interrupt aus
  und fürt alle 250Hz eine Tastenentprellung durch

  \param
  keine

  \return
  nichts

*****************************************************************************/
void timer2_init(void)
{
	/***	timer2_init(); ***/
	/* Timer2 auf 36 kHz eingestellt. */
	TCCR2A = (0 << WGM20) | (0 << WGM21) | (0 << COM2A0) | (0 << COM2A1);
	TCCR2B = (1 << CS21);
	/*
	 // Timer2 Überlauf auf eine Pin legen, um 36KHz genau einzustellen
	 OCR2B = 255;
	 DDRD |= (1<<PIN3);
	 TCCR2A |= (1<<COM2B0);
	 */
	/* Interrupts für Timer2 aktivieren */
	TIMSK2 |= (1 << TOIE2);

}

/****************************************************************************/
/*!
  \brief
  Gibt die aktuelle Zeit in ms zurueck.

  Die vergangene Zeit seit dem Einschalten\n
  Genauer: nachdem der Interrupt Timer2 aktiviert wurde.\n

  \param
  keine

  \return
  Einschaltzeit in Millisekunden (Bereich: unsigned long 0..286331153)\n
  Das sind ca. 79.5 Stunden. Diese Zeitangabe reicht bis der Accu leer ist.

  \par  Beispiel:
  (Nur zur Demonstration der Parameter/Returnwerte)
  \code
  // Alle 500 ms die Front-LED umschalten.
  unsigned long zeit;
  unsigned char on_off = TRUE;

  zeit = Gettime ();
  while (1)
  {
    if (Gettime () > zeit + 500)
    {
      zeit = Gettime ();
      FrontLED (on_off);
      on_off ^= 1;
    }
  }
  \endcode
*****************************************************************************/
unsigned long Gettime (void)
{
  return ((timebase * 256) + count36kHz) / 36;
}

/****************************************************************************/
/*!
  \brief
  Wartefunktion.

  Die maximale Wartezeit betraegt 7ms. Fuer laengere Wartezeiten siehe Msleep().\n
  Diese Funktion nutzt den Timer 2-Interrupt um ein 'zeitgefuehl' zu erhalten.\n
  Der Interrupt wird mit 36 kHz, durch die Init()-Funktion initialisiert,\n
  aufgerufen und zaehlt dort die globale Variablen \b count36kHz weiter.\n
  Diese Funktion nutzt diesen Zaehler und berechnet daraus mit dem uebergeben\n
  Parameter den Zeitpunkt wann die Pausenzeit erreicht ist, Danach bricht sie\n
  ab, und im Hauptprogramm ist eben eine Wartezeit eingelegt worden.

  \param[in]
  time36kHz Wartezeit x/36kHz (sec)

  \return
  nichts

  \par  Beispiel:
  (Nur zur Demonstration der Parameter/Returnwerte)
  \code
  // 1 Millisekunde warten
  Sleep (36);
  \endcode
*****************************************************************************/
void sleep ( unsigned char time36kHz)
{
  unsigned char ziel = (time36kHz + count36kHz) & 0x00FF;

  while (count36kHz != ziel)
    ;
}



/****************************************************************************/
/*!
  \brief
  Wartefunktion in ms.

  Diese Funktion nutzt die Sleep()-Funktion um mit dem uebergeben Parameter\n
  Pausen in ms-Einheiten zu erhalten.

  \param [in]
  dauer Wartezeit in Millisekunden.

  \return
  nichts

  \par  Beispiel:
  (Nur zur Demonstration der Parameter/Returnwerte)
  \code
  // 1.5 Sekunde warten
  Msleep (1500);
  \endcode
*****************************************************************************/
void msleep (int dauer)
{
  int z;
  for (z = 0; z < dauer; z++)	// z-mal ..
    sleep (36);					// ..eine ms warten
}

/****************************************************************************/
/*!
  \brief
  Interrupt-Funktion fuer Timer-2-Ueberlauf.

  \param
  keine

  \return
  nichts

  \see
  count36kHz, timebase

  \par
  Der zum Timer gehoerende Zaehler TCNT2 wird so justiert, dass damit die\n
  gewuenschten 36 kHz erreicht werden.\n
  Fuer die Zeitfunktionen werden die globalen Variablen count36kHz und\n
  timebase hochgezaehlt.
*****************************************************************************/
ISR(TIMER2_OVF_vect) /* veraltet: SIGNAL(SIG_OVERFLOW1) */
{
    /* erhöht den Wert des Timers um Ihn alle 36kHz überlaufen zu lassen */
	TCNT2 += 0xBB;

	/* Globalen counter erhöhen */
	count36kHz++;

	/* beim Überlauf globale Zeit erhöhen */
	if(!count36kHz){
		timebase++;
	}
}


