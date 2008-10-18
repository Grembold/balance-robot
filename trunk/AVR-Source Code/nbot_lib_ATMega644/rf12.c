/****************************************************************************/
/*!
* \file		rf12.c
*
* \brief	Stellt Funktionen zur Kommunikation mit dem RF12 Modul bereit.
*
* \version  V001	16.04.2007	Benedikt K.\n
*           first build\n
* \version  V---	04.05.2007	Olaf Petersen\n
*						Dokumentation für Doxygen.\n
*						SPI-Interface geändert\n
*****************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>

#include "rf12.h"


/*****    Makrodefinitionen *******/
#ifndef F_CPU
#define F_CPU 1105900UL			/*!< \brief Legt die Mikrokontroller Frequenz fest */
#endif
#include <util/delay.h>

#define DDR_SPI 	DDRB			/*!< \brief Register für Datenrichtung der SPI Anschlüße */
#define PORT_SPI 	PORTB			/*!< \brief Port der SPI Anschlüße */
#define PIN_SPI		PINB			/*!< \brief PIN der SPI Anschlüße */

#define SCK		7							/*!< \brief Pin des SPI-Taktes */
#define MOSI	5							/*!< \brief Pin für die MasterOutdaten */
#define MISO	6							/*!< \brief Pin für die MasterIndaten */
#define CS		4							/*!< \brief Chip Selekt Leitung */

#define CS_HIGH  PORT_SPI |= (1<<CS)	/*!< \brief Makro um ChipSelekt auf High zu ziehen */
#define CS_LOW 	 PORT_SPI &=~(1<<CS)	/*!< \brief Makro um ChipSelekt wieder freizugeben */


/****** Private Funktionen *********/
void spi_init(void);
uint8_t spi_putc(uint8_t data);

/****************************************************************************/
/*!
*	\brief Inizialisiert die SPI Schnittstelle
*  Das SPI Interface wird dadurch in den Master Modus geschaltet.
*
*****************************************************************************/
void spi_init(void)
{
	// Aktivieren der OUT Pins für das SPI Interface
	DDR_SPI |= (1<<SCK)|(1<<MOSI)|(1<<CS);
	PORT_SPI &=~((1<<SCK)|(1<<MOSI));
	PORT_SPI |=(1<<CS);
	
	// Aktivieren des SPI Master Interfaces, fosc = fclk /2
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(SPR1);
	//SPSR = (1<<SPI2X); Double SPI Speed as Master
}


/****************************************************************************/
/*!
*	\brief Senden eines Einzelenen Bytes
*
*	Da beim SPI Bus das Senden um Empfangen auf zwei verschiedenen Leitungen 
* läuft, kann gleichzeitig gesendet und empfangen werden. 
*
*	\param[in]
*	data Zu übertragendes Byte.
*
*	\return
* Gibt das empfangene Byte zurück.
*
* \par Beispiel:
*	\code
* // Daten per SPI senden
* spi_putc(0xaa);
*
* // Daten per SPI lesen
* data = spi_putc(0xff);
* \endcode
*
*****************************************************************************/
uint8_t spi_putc(uint8_t data)
{
	// Sendet ein Byte
	SPDR = data;
	
	// Wartet bis Byte gesendet wurde
	while (!(SPSR & (1<<SPIF)))
	;
	return SPDR;
}

/****************************************************************************/
/*!
*	\brief Initialisiert das RF12 Modul
*
*	Legt die Datenrichtung für die SPI-Pins Fest und wartet 100ms.\n
*	Anschliessend wird das Modul parametrisiert.
*
*****************************************************************************/
void rf12_init(void)
{
	spi_init();

	for (unsigned char i=0; i<10; i++)
		_delay_ms(10);				// wait until POR done

	// Stellt einen externen Clock mit 10Mhz bereit
	rf12_trans(0xC0E0);			// AVR CLK: 10MHz
	
	rf12_trans(0x80D7);			// Enable FIFO
	rf12_trans(0xC2AB);			// Data Filter: internal
	rf12_trans(0xCA81);			// Set FIFO mode
	rf12_trans(0xE000);			// disable wakeuptimer
	rf12_trans(0xC800);			// disable low duty cycle
	rf12_trans(0xC4F7);			// AFC settings: autotuning: -10kHz...+7,5kHz
	
	rf12_setfreq(RF12FREQ(433.92));		// Sende/Empfangsfrequenz auf 433,92MHz einstellen
	rf12_setbandwidth(4, 1, 4);				// 200kHz Bandbreite, -6dB Verstärkung, DRSSI threshold: -79dBm 
	rf12_setbaud(19200);							// 19200 baud
	rf12_setpower(0, 6);							// 1mW Ausgangangsleistung, 120kHz Frequenzshift
}



/****************************************************************************/
/*!
*	\brief übertragt/empfängt zwei Byte zum/vom Modul
*
*	Legt die Datenrichtung für die SPI-Pins Fest und wartet 100ms.\n
*	Anschliessend wird das Modul parametrisiert.
*
*	\param[in]
*	wert zu übertragendes wort.
*
*	\return
* Gibt das empfangene Wort zurück.
*
*****************************************************************************/
unsigned short rf12_trans(unsigned short wert)
{	
	// Ruckgabewert
	uint16_t rv;	

	// /CS auf Low ziehen
	CS_LOW;
	
	// Daten übermitteln und Ruckgabewert speichern
	rv  = (uint16_t)(spi_putc((uint8_t)wert>>8) << 8);
	rv |= (uint16_t) spi_putc((uint8_t)wert);
	
	// /CS Leitung wieder freigeben
	CS_HIGH;	

	return rv;
}


/****************************************************************************/
/*!
*	\brief 
*
*****************************************************************************/
void rf12_setbandwidth(unsigned char bandwidth, unsigned char gain, unsigned char drssi)
{
	rf12_trans(0x9400|((bandwidth&7)<<5)|((gain&3)<<3)|(drssi&7));
}


/****************************************************************************/
/*!
*	\brief legt die Mittlere Frequenz fest
*
*****************************************************************************/
void rf12_setfreq(unsigned short freq)
{	if (freq<96)				// 430,2400MHz
		freq=96;
	else if (freq>3903)			// 439,7575MHz
		freq=3903;
	rf12_trans(0xA000|freq);
}

/****************************************************************************/
/*!
*	\brief
*
*****************************************************************************/
void rf12_setbaud(unsigned short baud)
{
	if (baud<663)
		return;
	if (baud<5400)					// Baudrate= 344827,58621/(R+1)/(1+CS*7)
		rf12_trans(0xC680|((43104/baud)-1));
	else
		rf12_trans(0xC600|((344828UL/baud)-1));
}

/****************************************************************************/
/*!
*	\brief
*
*****************************************************************************/
void rf12_setpower(unsigned char power, unsigned char mod)
{	
	rf12_trans(0x9800|(power&7)|((mod&15)<<4));
}

/****************************************************************************/
/*!
*	\brief 
*
*****************************************************************************/
void rf12_ready(void)
{	
	CS_LOW;
	//while (!(PIN_SPI&(1<<MISO))); // wait until FIFO ready
	while (!(PIN_SPI&(1<<PB3))); // wait until FIFO ready
	//CS_HIGH;
}

/****************************************************************************/
/*!
*	\brief Sendet einen Block von Daten
*
*	\param[in]
*	buffer Zeigt auf das erste Byte des zu sendenden Blocks.
*
*	\param[in]
*	length Gibt die Länge des Blocks in Byte an.
*
*****************************************************************************/
void rf12_txdata(unsigned char *buffer, unsigned char length)
{	unsigned char i;
	rf12_trans(0x8238);			// TX on
	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();
	rf12_trans(0xB82D);
	rf12_ready();
	rf12_trans(0xB8D4);
	for (i=0; i<length; i++)
	{		
		rf12_ready();
		rf12_trans(0xB800|(*buffer++));
	}
	rf12_ready();
	rf12_trans(0x8208);			// TX off
}

/****************************************************************************/
/*!
*	\brief Empfängt eine Block von Daten
*
* \param[out]
*	buffer Zeiger in den die Daten gespeichert werden
*
* \param[in]
* length Gibt die Länge des Buffers in Byte an
*
*****************************************************************************/
void rf12_rxdata(unsigned char *buffer, unsigned char length)
{	
	unsigned char i;
	rf12_trans(0x82C8);			// RX on
	rf12_trans(0xCA81);			// set FIFO mode
	rf12_trans(0xCA83);			// enable FIFO
	for (i=0; i<length; i++)
	{	
		rf12_ready();
		*buffer++=rf12_trans(0xB000);
	}
	rf12_trans(0x8208);			// RX off
}

