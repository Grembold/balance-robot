/*************************************************************************/
/*!
* \file motor.h
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
#ifndef _MOTOR_H
#define _MOTOR_H 1

#define FWD		0		// forwärts (forward)
#define RWD		1		// rückwärts (reward)
#define BREAK	2		// bremst
#define FREE	3		// läuft frei

/***************************************
*	Funktions Prototypen
***************************************/
extern void motor_init(void);							// Initialisiert die Motorsteuerung
extern void motor_speed(uint8_t left_speed, uint8_t right_speed);	// setzt die Drehzahl von 0.. 255
extern void motor_dir(uint8_t left_dir, uint8_t right_dir);				// setzt drehrichtung der Motoren FWD..RMD
#endif
