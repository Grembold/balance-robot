/**************************************************************************/
/*!
* \file fifo.c
* \brief Funktionen eines FIFO Puffers
*
*	\par Quelle:
*	http://www.roboternetz.de/wissen/index.php/FIFO_mit_avr-gcc
*
*	\version	V001 -	25.02.2007 - Olaf Petersen\n
* 									first build
***************************************************************************/



#include "fifo.h"


/**************************************************************************/
/*!	\brief Initialisiert einen FIFO puffer aus dem Speicherbereich buffer
*
*	\par Beispiel:
*	\code
* #include "fifo.h"
* #define BUF_SIZE 10
* uint8_t buffer[BUF_SIZE];
* fifo_t fifo;
* fifo_init (&fifo, buffer, BUF_SIZE);
* \endcode
***************************************************************************/
void fifo_init (fifo_t *f, uint8_t *buffer, const uint8_t size)
{
	f->count = 0;		// kein Eintrag im Puffer
	f->pread = f->pwrite = buffer;	// lessezeiger auf Anfang setzen
	f->read2end = f->write2end = f->size = size;	// schreibzeiger auf anfang setzen
}

/**************************************************************************/
/*!	\brief Funktion fügt dem Puffer einen weiteren Wert hinzu
*
*			Schreibt das Byte data in die FIFO. Liefert 1 bei Erfolg und 
*			0, falls die FIFO voll ist.
***************************************************************************/
uint8_t fifo_put (fifo_t *f, const uint8_t data)
{
	if (f->count >= f->size)
		return 0;
		
	uint8_t sreg = SREG;
	cli();
	
	uint8_t * pwrite = f->pwrite;
	
	*(pwrite++) = data;
	
	uint8_t write2end = f->write2end;
	
	if (--write2end == 0)
	{
		write2end = f->size;
		pwrite -= write2end;
	}
	
	f->write2end = write2end;
	f->pwrite = pwrite;


	f->count++;
	SREG = sreg;
	
	return 1;
}


/**************************************************************************/
/*!	\brief Liest den nächsten Wert aus dem Puffer
*
* 		Schreibt in data das nächste Byte aus der FIFO.
*			Liefrt 1 bei Erfolg und 0 falls die FIFO leer ist.
***************************************************************************/
uint8_t fifo_get (fifo_t *f, uint8_t *data)
{
	if (!f->count)		return 1;
	
	uint8_t sreg = SREG;				// Interruptregister speichern
	cli();											// Interrupts verbieten (atomarer code)
	
	uint8_t *pread = f->pread;	// lesezeiger speichern
	uint8_t read2end = f->read2end;	// Einträge bis Bufferende
	*data = *(pread++);				// daten auslesen
	
	if (--read2end == 0)				// Zeiger hat ende erreicht
	{
		read2end = f->size;				// noch size einträge bis bufferende
		pread -= read2end;				// lese Zeiger beginnt von vorn
	}
	
	f->pread = pread;						// speichern des neuen Lesezeigers
	f->read2end = read2end;			// anzahl der einträge bis bufferende

	f->count--;									// ein Eintrag weniger im Buffer
	SREG = sreg;								// Interrupts wiederherstellen

	return 0;
}
