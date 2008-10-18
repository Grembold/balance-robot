/**************************************************************************/
/*!
* \file encoder.h
* \brief Definitionen und Funktionen für die Encoder Auswertung
*
* $Revision: 0.1 $
* $Date: 05.03.2007 $
* $Author: Olaf Petersen $
*
*
* \par Description: new functions has been added:
*
* void Encoder_init()
* int16_t EncoderGet(uint8_t motor)
* int32_t EncoderToMM(int32_t encoder)
*
*
* \version	V1.0 - 18.08.2007 - Olaf Petersen\n
*						created
*
***************************************************************************/

#ifndef _ENCODER_H
#define _ENCODER_H

#define LEFT 0
#define RIGHT 1

/**************************************************************************/
/*!
* \brief Umrechnungsfaktor von Flanken -> mm
*
* \par Funktionsweise:
* Mit diesem Faktor wird aus der Flangenzahl des Encoders die zurückgelegte 
* Strecke der Reifen umgerechnet.\n Die Flankenzahl des encoders muss zunächst 
* mit 10000 Multipliziert werden und dann mit diesem Wert dividiert.\n
*
* \par Beispiel:
*	\code
*   distance = enc_count * 10000;
*   distance /= UMRECHNUG_ECNODER_MM;
* \endcode 
***************************************************************************/
#define UMRECHNUG_ECNODER_MM 4898L	


/**************	GFUNKTIONS PROTOTYPEN ************************************/

void encoder_init(void);
int16_t encoderGet(uint8_t motor);
int32_t encoderToMM(int32_t encoder);

#endif /* _UART_H */
