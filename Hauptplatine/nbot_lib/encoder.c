/*************************************************************************/
/*!
* \file encoder.c
*
* \brief Dieses Programm soll zwei Motor mit je zwei 90° phasenverschobenen 
*   Signalen auswerten und über uart ausgeben
*
* \par Funktionsweise:
*		Ein Timer liest mit 5,4kHz die Eingänge der Encoder und wertet sie aus.
*		enc_delta liefert dann die drei Zustände -128..0..127 je nach Drehrichtung, 
*		wobei -128/127 bedeutet der Motor hat sich links-/rechtsherum gedreht und
*		0 bedeutet keine änderung.\n
*		Dieses delta wird zu einem Zähler addiert und gibt Zahl der Flanken an.
*
*
* \version	V--- - 04.03.2007 - Olaf Petersen\n
*			Created\n
*************************************************************************/

#include <avr/io.h> 	//I/O-Register 
#include <stdint.h>		//standart Integertypen
#include <stdlib.h> 	// itoa
#include <avr/interrupt.h>

#include "encoder.h"	
#include "uart.h"			
#include "timer.h"
#include "motor.h"


/*! \brief Anschlussbelegung des Encoders Phase A für den linken Motor   */
#define PHASE_LEFT_A		(PINC & 1<<PINC2)
/*! \brief Anschlussbelegung des Encoders Phase B für den linken Motor   */
#define PHASE_LEFT_B 		(PINC & 1<<PINC3)
/*! \brief Anschlussbelegung des Encoders Phase A für den rechten Motor  */
#define PHASE_RIGHT_A		(PINC & 1<<PINC0)
/*! \brief Anschlussbelegung des Encoders Phase B für den rechten Motor  */
#define PHASE_RIGHT_B		(PINC & 1<<PINC1)


volatile int16_t	enc_delta[2];		/*!< Encoderflangen seit dem letzten TimerIRQ */


/**************************************************************************/
/*!
*	\brief Initialisiert die Encoderauswertung.
*
*	Hier wird die Datenrichtung der Angeschlossenen Pins und die einstellungen 
*	für den Timer0 vorgenommen.
***************************************************************************/
void encoder_init()
{
	DDRC &= ~((1<<PC0) | (1<<PC1) | (1<<PC2) | (1<<PC3));		// Phasen des rechten Motors als Input
	PORTC |= ((1<<PC0) | (1<<PC1) | (1<<PC2) | (1<<PC3));		// Interne Pull-Up für beide einschalten
	encoderGet(LEFT);	
	encoderGet(RIGHT);
	// Timer0 einrichten und Überlauf Interrupt erlauben
	TCCR0 = 1<<CS01;			// CPU_FP divitiert durch 8 * 256 -> 5,4kHz
	TIMSK |= 1<<TOIE0;		// enable timer interrupt
}



/**************************************************************************/
/*!
* \brief Liefert den Zählerstand seit dem letzten Auslesen des Encoders.
*
* Der Zählerstand wird nach dem Auslesen wieder gelöscht. Dieser 
*	Programmteil ist atomar( nicht unterbrechbar) da auch die Interruptrotiene
* auf die Variable zugreift.
*
* \param [in]
*		motor	Motor auswahl: LEFT, RIGHT
*
* \return
*	Zählerdifferenz seit dem letzten Auslesen
***************************************************************************/
int16_t encoderGet(uint8_t motor)
{
	int16_t r;							// rückgabewert
	uint8_t tmp_sreg;				// temporaerer Speicher fuer das Statusregister
	
	tmp_sreg = SREG;				// Statusregister (also auch das I-Flag darin) sichern
	cli();									// Interrupts global deaktivieren
	r = enc_delta[motor];		// Encoderwert speichern
	enc_delta[motor]	= 0;	// enc_delta zurücksetzen
	SREG = tmp_sreg;				// Status-Register wieder herstellen 
	
	return r;							// Encoderwert zurückgeben
}


/**************************************************************************/
/*!
*	\brief Umrechner von EncoderFlanken nach mm
*
*	\param [in]
*	encoder	Wert der Umgerechnet werden soll
*
*	\return
*	Liefert die gefahrene strecke in mm zurück
***************************************************************************/
int32_t encoderToMM(int32_t encoder)
{
	return ((int32_t)(encoder*1000L)/UMRECHNUG_ECNODER_MM);
}


/**************************************************************************/
/*!
* \brief timer mit 5,4kHz zur Encoder auswertung
*
* Es werden die Einganszustände des rechten Motor Eingelesen uns 
* Ausgewertet.
***************************************************************************/
ISR(TIMER0_OVF_vect)
{
	static int8_t enc_last[] = {0x01, 0x01};
	int8_t i[] = {0, 0};

	// Einlesen der Signalzustände für den linken Encoder
	if( PHASE_LEFT_A )
		i[LEFT] = 1;

	if( PHASE_LEFT_B )
		i[LEFT] ^= 3;											// convert gray to binary
		
		// Einlesen der Signalzustände für den rechten Encoder
	if( PHASE_RIGHT_A )
		i[RIGHT] = 1;

	if( PHASE_RIGHT_B )
		i[RIGHT] ^= 3;											// convert gray to binary	
		
	// Auswerten des LINKEN Motor Encoders
	i[LEFT] -= enc_last[LEFT];					// difference new - last

	if( i[LEFT] & 1 ){									// bit 0 = value (1)
		enc_last[LEFT] += i[LEFT];				// store new as next last

		enc_delta[LEFT] += (i[LEFT] & 2) - 1;			// bit 1 = direction (+/-)
	}	
	
	// Auswerten des RECHTEN Motor Encoders
	i[RIGHT] -= enc_last[RIGHT];				// difference new - last

	if( i[RIGHT] & 1 ){								// bit 0 = value (1)
		enc_last[RIGHT] += i[RIGHT];			// store new as next last

		enc_delta[RIGHT] += (i[RIGHT] & 2) - 1;		// bit 1 = direction (+/-)
	}
}

