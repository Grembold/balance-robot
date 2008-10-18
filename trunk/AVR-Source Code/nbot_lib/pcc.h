/**************************************************************************/
/*!
* \file pcc.h
* \brief Definitionen und Funktionen PC-Communication
*
* $Revision: 0.1 $
* $Date: 16.04.2007 $
* $Author: Olaf Petersen $
*
*
*
* \version	V1.0 - 16.04.2007 - Olaf Petersen\n
*						created
*
***************************************************************************/

#ifndef _PCC_H
#define _PCC_H

#define LEFT 0
#define RIGHT 1

/*************** Typ definitionen und Strukturen *************************/
/*! \brief liste von Pointern zu lesender variablen */
typedef struct
{
	uint32_t 	*time;					/*!< \brief Pointer zu systemzeit */
	int16_t		*encoder[2];		/*!< \brief Pointer zu den Encoderwerten */
	int16_t 	*sollwert[2];		/*!< \brief Pointer zum sollwert der Motoren */
	int16_t		*istwert[2];		/*!< \brief Pointer zur momentanen Drehzahl der Motoren */
	uint8_t		*rfm;						//
}pcc_t;

/*! \brief header der Nachricht */
typedef union{
	uint8_t y;
	struct{
	  unsigned length:3;		/*!< \brief Anzahl der DatenBytes (0..7) */
		unsigned cmdid:5;			/*!< \brief Kommando Id */
	};	
}tunHeader;

/*! \brief struktur einer Nachricht */
typedef struct {
	tunHeader header;				/*!< \brief Nachrichtenkopf mit Kommando und Anzahl der Nutzdaten */
	uint8_t		message[8];		/*!< \brief Datenbytes */
} tstMessage;

// Komandoidentifikation
#define PCC_SYNC							00
// BOARD
#define PCC_BOARD_GET 				01			/*!< \brief Pins abfragen */
#define PCC_BOARD_SET 				02  		/*!< \brief PINs setzen */
#define PCC_BOARD_AKKU 				03			/*!< \brief Akku Spannung abfragen */
#define PCC_BOARD_I2C 				04			/*!< \brief reserviert für I²C , OHNE FUNKTION */
#define PCC_BOARD_TIME				05			/*!< \brief Sytemzeit abfragen */
#define PCC_BOARD_RFM					06			/*!< \brief Funktnachrichten abfragen */
// MOTOR LINKS
#define PCC_MOT_L_ENC					10			/*!< \brief Encoderwert */
#define PCC_MOT_L_CUR					11			/*!< \brief Motorstrom */
#define PCC_MOT_L_ACS 				12			/*!< \brief Istdrehzahl (Actual Speed)  */
#define PCC_MOT_L_SRS 				13			/*!< \brief Solldrehzahl (Set Rotation Speed) */
#define PCC_MOT_L_TMP 				14			/*!< \brief reserviert für Tempreratur, OHNE FUNKTION */
//MOTOR RECHTS
#define PCC_MOT_R_ENC					16			/*!< \brief Encoderwert */
#define PCC_MOT_R_CUR					17			/*!< \brief Motorstrom */
#define PCC_MOT_R_ACS 				18			/*!< \brief Istdrehzahl (Actual Speed)  */
#define PCC_MOT_R_SRS 				19			/*!< \brief Solldrehzahl (Set Rotation Speed) */
#define PCC_MOT_R_TMP 				20			/*!< \brief reserviert für Tempreratur, OHNE FUNKTION */
// SENSOREN
#define PCC_SENS_IR1 					21			/*!< \brief Infrarot Abstandssensor(SHARP) 1 */
#define PCC_SENS_IR2 					22			/*!< \brief Infrarot Abstandssensor(SHARP) 2 */
#define PCC_SENS_ACC 					23			/*!< \brief reserviert für Beschleunigugssensor(acceleration), OHNE FUNKTION */
#define PCC_SENS_ORI 					24			/*!< \brief reserviert für Kompass(orientation), OHNE FUNKTION */
// AKTION
#define PCC_ACTION_MOVE 			31			/*!< \brief Fahre geradeaus */
#define PCC_ACTION_TURN 			30			/*!< \brief Fahre Kurve */


#define MAX_BUF 100	// maximal 255 zeichen speichern

extern void pcc_init(const pcc_t *daten);		// Initialisiert die PC Kommunikation
extern void pcc_rx(uint8_t buf);						// Kommandoempfang
extern void pcc(void);											// Kommando abaarbeitung
extern void pcc_send(tstMessage *message);	// Versenden einer nachricht
#endif /* _PCC_H */
