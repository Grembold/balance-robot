/****************************************************************************/
/*!
  \file     globals.c

  \brief    Globale Variablen der nbot Library.

  \par      Hinweis:
  Diese Variablen sind immer in allen Stellen im Programm nutzbar.

  \version  V001 - 25.02.07 - Olaf
			Erste Verion
*****************************************************************************/

/****************************************************************************/
/*!
  \brief
  Counter fuer 36kHz.
*****************************************************************************/
volatile unsigned char count36kHz;

/****************************************************************************/
/*!
  \brief
  Sytemzeit in ms.
*****************************************************************************/
volatile unsigned long timebase;


/****************************************************************************/
/*!
  \brief
  Odometriesensor Zaehler bei Interrupt Betrieb.\n
  encoder[0] links, encoder[1] = rechts.
*****************************************************************************/
int encoder [2];

/****************************************************************************/
/*!
  \brief
  Steuert, ob die Odometrie-Sensor Abfrage im Interrupt Betrieb laufen soll.
*****************************************************************************/
volatile int autoencode;