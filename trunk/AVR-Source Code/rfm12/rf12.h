/**************************************************************************/
/*!
* \file rf12.h
* \brief Definitionen und Funktionen für das RFM12-Funkmodul
*
* \par Description: 
*
* \version  V001	16.04.2007	Benedikt K.\n
*           first build\n
* \version  V---	04.05.2007	Olaf Petersen\n
*						Dokumentation für Doxygen.\n
***************************************************************************/
/***************************************************************************
*
*	it is not allowed to remove the nicknames of the contributers to this 
*	software from the function header
***************************************************************************/
/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   any later version.                                                    *
***************************************************************************/

extern uint16_t  rf12_trans(uint16_t wert);		// transfer 1 word to/from module
extern void rf12_init(void);																// initialize module
extern void rf12_setfreq(unsigned short freq);							// set center frequency
extern void rf12_setbaud(unsigned short baud);							// set baudrate
extern void rf12_setpower(unsigned char power, unsigned char mod);			// set transmission settings
extern void rf12_setbandwidth(unsigned char bandwidth, unsigned char gain, unsigned char drssi);	// set receiver settings
extern void rf12_txdata(unsigned char *data, unsigned char number);		// transmit number of bytes from array
extern void rf12_rxdata(unsigned char *data, unsigned char number);		// receive number of bytes into array
extern void rf12_ready(void);																// wait until FIFO ready (to transmit/read data)

#define RF12FREQ(freq)	((freq-430.0)/0.0025)									// macro for calculating frequency value out of frequency in MHz

