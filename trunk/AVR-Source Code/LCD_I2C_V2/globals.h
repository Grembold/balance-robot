/****************************************************************************/
/*!
  \file     globals.h

  \brief    Globale Variablen der nDisplay Library.

  \par      Hinweis:
  Diese Variablen sind immer in allen Stellen im Programm nutzbar.

  \version  V001 - 08.10.07 - Olaf
			Erste Verion
*****************************************************************************/
#ifndef _GLOBALS_H
#define _GLOBALS_H   1

#define ELEMENTE		5	// Anzahl der ingesammt anzeigbaren Elemente/ der Zeilen in der Strukturtabelle, 

#pragma once

/* Enumeration */ 
enum enVariablenTypen {FLAG, UCHAR, USHORT, ULONG, SCHAR, SSHORT, SLONG, REAL};
enum enElemtenTypen	{VARIABLE, MENUE, TEXT};


typedef struct
{
	uint8_t id;									// ID des Elementes
	uint8_t	pos;								// Position innerhalb des Menüs
	void*	element;							// Pointer auf das Element
	enum 	enElemtenTypen elementType;		// Typ des Elementes
	uint8_t	parent;								// übergeortnetes Elemente
}tstZeile;

/****************************************************************************/
/*!
  \brief
  Tabelle mit allen Elementen und deren Adressen
*****************************************************************************/
tstZeile* Tabelle;				// Strukturtabelle
uint8_t numOfElements;		// anzahl der Elemente die die Tabelle enthält


#endif
