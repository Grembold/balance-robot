/*************************************************************************/
/*!
* \file motor.c
*
* \brief Steuert die Motoren über PWM und Drehrichtung.
*
* \par Funktionsweise:
*	Der Timer1 enthält zwei compare-Einheiten. Es lassen sich also zwei PWM
*	Signale damit erzeugen. Die PWM-Signale führen zu den Enable Leiungen der
* Motor H-Brücke und ermöglichen die Einstellung der Motorspannung.\n\n 
* Die Drehrichtung der Motoren wird über je zwei Steuerleitungen bestimmt.
* Diese werden mit der Funktion motor_dir gesetzt.\n
* Zur einfacheren Bediehnung gibt es für die Drehrichtung definitionen.\n
* \li \c FWD vorwärts\n
*	\li \c RWD rückwärts\n
*	\li \c BREAK bremse\n
*	\li \c FREE ohne änderung\n
*
*
* \version	V--- - 04.03.2007 - Olaf Petersen\n
*			Created\n
*************************************************************************/

#include <util/delay.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <stdint.h> 		//integerwerte wie uint8_t, uint16_t
#include <stdlib.h> 		// itoa() und utoa()

#include "motor.h"

/** Anschlussbelegung des linken MOTORs **/
#define MOTOR_LINKS_PWM_DDR 				DDRD 	/*!< DDRD Datenrichtung an port an dem die PWM für MOROT_Rechts ist */
#define MOTOR_LINKS_PWM_PORT 				PORTD		
#define MOTOR_LINKS_PWM 						PD5		/*!< PD5 Pin der PWM */
#define MOTOR_LINKS_RICHTUNG_DDR 		DDRC	/*!< DDRA	PORT an dem die Motorrichtung angeschlossen ist */
#define MOTOR_LINKS_RICHTUNG_PORT 	PORTC	/*!< PORTA  Port der Drehrichtung */
#define MOTOR_LINKS_RICHTUNG0 			PC6		/*!< PA4	PIN der MOTORRICHTUNG */
#define MOTOR_LINKS_RICHTUNG1 			PC7		/*!< PA5	die Motorrichtung wird mit zwei Signalen definiert */
	

/** Anschlussbelegung des rechten MOTORs **/
#define MOTOR_RECHTS_PWM_DDR 				DDRD 	/*!< DDRD Port an dem die PWM für MOROT_Rechts ist */
#define MOTOR_RECHTS_PWM_PORT 			PORTD	/*!< PORTD Port der PWM */
#define MOTOR_RECHTS_PWM 						PD4		/*!< PD4 Pin der PWM */
#define MOTOR_RECHTS_RICHTUNG_DDR 	DDRC	/*!< DDRA	Datenrichtung an dem die Motorrichtung angeschlossen ist */
#define MOTOR_RECHTS_RICHTUNG_PORT 	PORTC	/*!< PORTA PORT der Motorreichtung */
#define MOTOR_RECHTS_RICHTUNG0 			PC4		/*!< PA6	PIN der MOTORRICHTUNG */
#define MOTOR_RECHTS_RICHTUNG1 			PC5		/*!< PA7	die Motorrichtung wird mit zwei Signalen definiert */


/** PWM EInstellungen **/
#define PWM_MAX					0x01FF				/*!< 0x01FF (9bit) die PWM läßt sich mit 8, 9 oder 10bit einstellen */

#define PWM_LINKS				OCR1A 
#define PWM_LINKS_ON		TCCR1A |=  (1<<COM1A1) 
#define PWM_LINKS_OFF		TCCR1A &= ~(1<<COM1A1) 

#define PWM_RECHTS			OCR1B 
#define PWM_RECHTS_ON		TCCR1A |=  (1<<COM1B1) 
#define PWM_RECHTS_OFF	TCCR1A &= ~(1<<COM1B1) 

/**************************************************************************/
/*!
*	\brief Initialisieren der Motorsteuerung
*
* PIN Datenrichtungen, Timer  und PWM werden hier eingerichtet
*	
* \par Funkionsweise:
*	Richtet eine "fast 9-bit PWM"
*	
*	\param [in]
*	left_speed PWM des linken Motors im Bereich von 0..255
*
*	\param [in]
*	right_speed PWM des rechten Motots im Bereich von 0..255
*
***************************************************************************/
void motor_init(void) 
{ 
	/** DATENRICHTUNG und ANFANGSWERT **/
	// Motor links
	MOTOR_LINKS_PWM_DDR |= (1<<MOTOR_LINKS_PWM); 	// Ausgang
	MOTOR_LINKS_PWM_PORT &=~(1<<MOTOR_LINKS_PWM); 	//0 Anfangswert AUS
	MOTOR_LINKS_RICHTUNG_DDR  |= (1<<MOTOR_LINKS_RICHTUNG0) | (1<<MOTOR_LINKS_RICHTUNG1);	// Ausgang

	// Motor rechts
	MOTOR_RECHTS_PWM_DDR |= (1<<MOTOR_RECHTS_PWM);		// Ausgang
	MOTOR_RECHTS_PWM_PORT &=~(1<<MOTOR_RECHTS_PWM );	//0 Anfangswert AUS
	MOTOR_RECHTS_RICHTUNG_DDR |=(1<<MOTOR_RECHTS_RICHTUNG0) | (1<<MOTOR_RECHTS_RICHTUNG1);		// Ausgang

	motor_dir(FWD, FWD);
	motor_speed(0, 0);
	
	/* fast 9-bit PWM aktivieren ( nicht invertiert ) */ 
	TCCR1A = (1<<WGM11); 
	/* Einstellen der PWM-Frequenz auf ca. 21,6kHz ( Prescaler = 1 ) */ 
	TCCR1B = (1<<CS10)|(1<<WGM12); 
	/* PWN einschalten -- > PIN mit PWM verbinden */ 
	PWM_LINKS_ON; 
	PWM_RECHTS_ON;
	
} 

/**************************************************************************/
/*!
*	\brief Setzt die Motor PWM
*
* Hier wird die PWM für den linken und rechten Motor gestetzt.
*	
* \par Funkionsweise:
*	Zu dem Übergabe Parameter werden 255 addiert, da eine PWM kleiner als 50%
*	keine auswirkung auf die Motoren hat und so der obere bereich keinauer 
*	eingestellt werden kann.\n
*	Die Funktion Setzt den Vergleichswert des Timer1 für die jeweilige PWM 
*	(left, right). Der Programmkode wird atomar (nicht unterbrechbar) ausgeführt
* damit es keine komplikationen mit den Timern gibt.
*	
*	\param [in]
*	left_speed PWM des linken Motors im Bereich von 0..255
*
*	\param [in]
*	right_speed PWM des rechten Motots im Bereich von 0..255
*
***************************************************************************/
void motor_speed(uint8_t left_speed, uint8_t right_speed)
{
	uint8_t tmp_sreg;			// temporaerer Speicher fuer das Statusregister
 
	tmp_sreg = SREG;			// Statusregister (also auch das I-Flag darin) sichern
	cli();
	
	if(left_speed != 0){
		PWM_LINKS = (uint16_t)(left_speed+0xff);
	}	else {
		PWM_LINKS = 0;
	}
	
	if(right_speed != 0){
		PWM_RECHTS = (uint16_t)(right_speed+0xff);
	}	else {
		PWM_RECHTS = 0;
	}
	
	SREG = tmp_sreg;			// Statusregister zurückschreiben
}

/**************************************************************************/
/*!
*	\brief Drehrichtung der Motoren
*
* Diese Funktion setzt die Drehrichtung der Motoren. Hierführ stehen vier
* Möglichkeiten zur verfügung:\n
* \c FWD vorwärts\n
*	\c RWD rückwärts\n
*	\c BREAK bremsen\n
*	\c FREE ohne änderung\n
*
*	\param [in]
*	left_dir Drehrichtung des linken Motors
*	\param [in]
*	right_dir Drehrichtung des rechten Motors
***************************************************************************/
void motor_dir(uint8_t left_dir, uint8_t right_dir)
{
	switch (left_dir)
	{
		case FWD:
		  MOTOR_LINKS_RICHTUNG_PORT &= ~(1<<MOTOR_LINKS_RICHTUNG1);	//0
			MOTOR_LINKS_RICHTUNG_PORT |=(1<<MOTOR_LINKS_RICHTUNG0);		//1
			break;
		case RWD:
			MOTOR_LINKS_RICHTUNG_PORT &= ~(1<<MOTOR_LINKS_RICHTUNG0);	//0	
			MOTOR_LINKS_RICHTUNG_PORT |=(1<<MOTOR_LINKS_RICHTUNG1);		//1
			break;
		case BREAK:
			MOTOR_LINKS_RICHTUNG_PORT &= ~(1<<MOTOR_LINKS_RICHTUNG0);	//0	
			MOTOR_LINKS_RICHTUNG_PORT &= ~(1<<MOTOR_LINKS_RICHTUNG1);	//0	
			break;
		case FREE:
			break;
	}
	
	switch (right_dir)
	{
	case FWD:
		MOTOR_RECHTS_RICHTUNG_PORT &= ~(1<<MOTOR_RECHTS_RICHTUNG0);	//0
		MOTOR_RECHTS_RICHTUNG_PORT |=(1<<MOTOR_RECHTS_RICHTUNG1);		//1
		break;
	case RWD:
		MOTOR_RECHTS_RICHTUNG_PORT &= ~(1<<MOTOR_RECHTS_RICHTUNG1);	//0
		MOTOR_RECHTS_RICHTUNG_PORT |=(1<<MOTOR_RECHTS_RICHTUNG0);		//1
		break;
	case BREAK:
		MOTOR_RECHTS_RICHTUNG_PORT &= ~(1<<MOTOR_RECHTS_RICHTUNG0);	//0
		MOTOR_RECHTS_RICHTUNG_PORT &= ~(1<<MOTOR_RECHTS_RICHTUNG1);	//0
		break;
	case FREE:
		break;
	}
}


