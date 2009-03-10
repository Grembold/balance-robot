/*************************************************************************/
/*!
* \file motor.h
*
* \brief Steuert die Motoren �ber PWM und Drehrichtung.
*
* \par Funktionsweise:
*	Der Timer1 enth�lt zwei compare-Einheiten. Es lassen sich also zwei PWM
*	Signale damit erzeugen. Die PWM-Signale f�hren zu den Enable Leiungen der
* Motor H-Br�cke und erm�glichen die Einstellung der Motorspannung.\n\n 
* Die Drehrichtung der Motoren wird �ber je zwei Steuerleitungen bestimmt.
* Diese werden mit der Funktion motor_dir gesetzt.\n
* Zur einfacheren Bediehnung gibt es f�r die Drehrichtung definitionen.\n
* \li \c FWD vorw�rts\n
*	\li \c RWD r�ckw�rts\n
*	\li \c BREAK bremse\n
*	\li \c FREE ohne �nderung\n
*
*
* \version	V--- - 04.03.2007 - Olaf Petersen\n
*			Created\n
*************************************************************************/
#ifndef _MOTOR_H
#define _MOTOR_H 1

#define FWD		0		// forw�rts (forward)
#define RWD		1		// r�ckw�rts (reward)
#define BREAK	2		// bremst
#define FREE	3		// l�uft frei

/***************************************
*	Funktions Prototypen
***************************************/
extern void motor_init(void);							// Initialisiert die Motorsteuerung
extern void motor_speed(uint8_t left_speed, uint8_t right_speed);	// setzt die Drehzahl von 0.. 255
extern void motor_dir(uint8_t left_dir, uint8_t right_dir);				// setzt drehrichtung der Motoren FWD..RMD
#endif
