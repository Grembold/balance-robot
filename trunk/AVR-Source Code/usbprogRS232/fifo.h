/**************************************************************************/
/*!
* \file fifo.h
* \brief Definitionen und Funktionen eines FIFO Puffers
*
*	\par Quelle:
*	http://www.roboternetz.de/wissen/index.php/FIFO_mit_avr-gcc
*
*	\version	V001 -	25.02.2007 - Olaf Petersen\n
* 									first build\n
*										keine Inline Funktionen verwendet\n
***************************************************************************/


#ifndef _FIFO_H_
#define _FIFO_H_

#include <avr/io.h>
#include <avr/interrupt.h>

/**************************************************************************/
/*!	\brief Struct eines FIFO Puffers
***************************************************************************/
typedef struct
{
	uint8_t volatile count;		/*!< \brief n Zeichen im Puffer */
	uint8_t size;							/*!< \brief Puffer-Größe */
	uint8_t *pread;						/*!< \brief Lesezeiger */
	uint8_t *pwrite;					/*!< \brief Schreibzeiger */
	uint8_t read2end;					/*!< \brief n Zeichen bis zum Überlauf Lesezeiger */
	uint8_t write2end;				/*!< \brief n Zeichen bis zum Überlauf Schreibzeiger */
} fifo_t;

extern void fifo_init (fifo_t *f, uint8_t *buffer, const uint8_t size);
extern uint8_t fifo_put (fifo_t *f, const uint8_t data);
extern uint8_t fifo_get (fifo_t *f, uint8_t *data);


#endif /* _FIFO_H_ */
