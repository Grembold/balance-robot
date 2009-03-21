/**************************************************************************/
/*!
* \file timer.h
* \brief Definitionen und Funktionen des Timer2
*
* $Revision: 0.1 $
* $Date: 25.02.2007 $
* $Author: Olaf Petersen $
*
*
* \par Description: 
*	new functions has been added:\n
* 	void timer2_init(void);\n 				
* 	unsigned long Gettime (void);\n			
* 	void sleep (unsigned char time36kHz);\n	
* 	void msleep (int dauer);\n				
*
* \version 	V--- - 25.02.2007 - Olaf Petersen\n
*			erste Portierung auf nBot\n
*
*
* \version  V--- - 10.11.2003 - Jan Grewe - DLR\n
*           Original Version von der ASURO CD\n
* \version  V--- - bis zum 07.01.2007 - \n
*           Bitte in Datei CHANGELOG nachsehen.\n
* \version  V001 - 13.01.2007 - m.a.r.v.i.n\n
*           +++ Alle Funktionen\n
*           Zerlegte Sourcen in einzelne Dateien fuer eine echte Library.
* \version  V002 - 22.01.2007 - Sternthaler\n
*           +++ Alle Funktionen\n
*           Kommentierte Version (KEINE Funktionsaenderung)
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

#ifndef _TIMER_H
#define _TIMER_H

/*** Variabelen */
volatile unsigned char count36kHz;
volatile unsigned long timebase;

/*** Funktionen */
extern void timer2_init(void); 				// timer2 initialisiere
extern unsigned long Gettime (void);			// Gibt die Aktuelle Zeit in ms zurück
extern void sleep (unsigned char time36kHz);	// wartet x/36 sec
extern void msleep (int dauer);				// waartet in ms
#endif /* _TIMER_H */

