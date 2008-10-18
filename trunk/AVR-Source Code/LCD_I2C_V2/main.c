 /*************************************************************************/
/*!
* \file main.c
*
* \brief 
*
* \par Funktionsweise: LCD Menü anzeige über I2C gesteuert
*	
* \version	V--- - 13.08.2007 - Olaf Petersen\n
*			Created\n
* \version	V001 - 18.10.2007	-	Olaf	Peterse\n
*			Twi Communikation hinzugefügt.
*			Es lassen sich Pakete empfangen
*************************************************************************/

#include <avr/io.h> 			/// I/O-Register 
#include <avr/interrupt.h>
#include <util/delay.h>		// _delay_ms() und delay_us()  Funktionen
#include <stdint.h>				// standart Integertypen
#include <stdlib.h> 			// itoa, wandlung von Integer nach Ascii
#include <string.h>				// string methoden
#include <math.h>

#include "globals.h"
#include "twi_slave.h"

#include "../nbot_lib_ATMega644/lcd.h"
#include "../nbot_lib_ATMega644/timer.h"	// Leider hat der Mega644 andere Timer-Register, so dass ich die Lib in einen anderen ordner kopiert habe
#include "../nbot_lib_ATMega644/taste.h"


/* Defines und Makros */
#define ENTER 	PA0		///< PiN der Taste Enter
#define UP 		PA2     ///< PiN der Taste hoch
#define DOWN 	PA1     ///< PiN der Taste runter */


#define LED1 (1<<PB2)							///< Pin der LED 1
#define LED2 (1<<PB3)							///< Pin der LED 2
#define LED1_AN	PORTB &= ~LED1		///< Schaltet die LED 1 an
#define LED1_AUS	PORTB |= LED1		///< Schaltet die LED 1 aus
#define LED1_TOG	PORTB ^= LED1		///< wechselt den Zustand der LED 1
#define LED2_AN	PORTB &= ~LED2		///< Schaltet die LED 2 an
#define LED2_AUS	PORTB |= LED2		///< Schaltet die LED 2 aus
#define LED2_TOG	PORTB ^= LED2		///< wechselt den Zustand der LED 2

#define TITELKURZ	12		///< kurtzer Titel für Variablen
#define TITELLANG 	17	///< langer Titel für überschriften
 

 


/* Strukturen */
typedef struct tstVariable
{
	uint8_t id;										///< ID des Tabelleneintrages (muss eindeutig sein)
	char titel[TITELKURZ+1];			///< Titel oder Tagname des angezeigten Wertes
	union{																								
		uint8_t			flag:1;					///< bool			    				1/0
		uint8_t			uchar;					///< unsigned byte/char		0 ..255
		uint16_t 		ushort;					///< unsigned short				0 ..65535
		uint32_t 		ulong;					///< unsigned long 				0 ..4294967295
		int8_t			schar;					///< char 						 -127 .. 127
		int16_t			sshort;					///< short			     -32767 .. 32767
		int32_t			slong;					///< long       -2147483647 .. 2 147 483 647
		float				real;						///< real  Kommazahl
	} value;											///< Anzuzeigender Wert
	enum enVariablenTypen valueType;			///< Typ des Wertes
	unsigned  bWriteable:1;					///< überschreibbar
	unsigned  bChanged:1;						///< Wert wurde bearbeitet/verändert
}tstVariable;

typedef struct tstMenue
{
	uint8_t	id;										///< ID des Tabelleneintrages (muss eindeutig sein)
	char titel[TITELLANG+1]; 			///< Titel des Menüs
}tstMenue;

typedef struct tstText
{
	uint8_t	id;										///< ID des Tabelleneintrages (muss eindeutig sein)
	char titel[TITELLANG+1];			///< Titel des Textes
	char text[200];								///< text
}tstText;





/* Globale Variablen */
uint8_t tabelSize = 0;
uint8_t aktivesElement = 0;		///< id des Aktiven elementes
uint8_t selectedElement = 0;	///< id des ausgewählten Elementes
int8_t  cursorPosition = 0;		///< Position des Cursors im Menü (1..255) (0=aus)
uint8_t menueOffset = 0;			///< Anzahl der Inhaltselemente, die durch das Scrollen nach oben verschwunden sind
uint8_t reDrawAll = 1;				///< lässt die gesammte Anzeige neuzeichnen
union{																								
	uint8_t			flag:1;					///< bool			    				1/0
	uint8_t			uchar;					///< unsigned byte/char		0 ..255
	uint16_t 		ushort;					///< unsigned short				0 ..65535
	uint32_t 		ulong;					///< unsigned long 				0 ..4294967295
	int8_t			schar;					///< char 						 -127 .. 127
	int16_t			sshort;					///< short			     -32767 .. 32767
	int32_t			slong;					///< long       -2147483647 .. 2 147 483 647
	float				real;						///< real  Kommazahl
} value;											///< kopie Anzuzeigender Wert




/* Funktions prototypen */
uint8_t 	get_type_of_element(uint8_t id); 	///< liefert den Typ(enElemtenTypen) des Elementes zurück 
void* 		get_element(uint8_t id);					///< liefert Pointer in dem das Element steht zurück 
uint8_t 	get_num_of_elements(uint8_t id);	///< liefert die Anzahl der Kinderelemente zurück 
void display_variable(void);								///< zeigt die Bearbeitungsansicht eines Variablenwertes
void display_menue(void);										///< zeigt ein Menü
void switch_to_variable(void);							///< wechelt die Anzeig zum Bearbeitungsansicht
void switch_to_menue(void);									///< wechelt die Anzeige zur Menüansicht
void enter_variable(void);									///< Verarbeitung von Entereingaben bei Variablen
void enter_menue(void);											///< verarbeitung von Entereingaben bei Menüs
void draw_inhalt(uint8_t* liste, uint8_t anz);	///< zeichnet den Inhalt(KinderElemente) eines Menüs
void draw_cursor(void);											///< zeichnet die Cursor für Menü und Variable
void draw_title(void);											///< zeichnet den Titel für Menü und Variable
void wait_until_keypressed(void);						///< wartet auf tastendruck 
void getTwiPaket(void);											///< empfängt Twi Pakete


/* Main(), hier geht es los */
int main(void)
{
	// Ein und Ausgägne Festlegen
	DDRA &=~((1<<ENTER)|(1<<UP)|(1<<DOWN));  // Tasten als Eingänge
	PORTA |=(1<<ENTER)|(1<<UP)|(1<<DOWN);    // pullup aktivieren
	
	DDRB |=LED1|LED2;					// LED1 und 2 ans Ausgang
	LED1_AUS;									// LED1 ausschalten
	LED2_AUS;									// LED2 ausschalten
	
	
	timer2_init();						// Timer2 zur Tastenentprellung starten
	lcd_init(LCD_DISP_ON);		// LCD initialisieren
	twi_slave_init(DEFAULT_SLAVE_ADD);	// Twi slave initialisieren
	

	// leere Tabelle für 5 Einträge erstellen
	tabelSize = ELEMENTE;
	Tabelle = (tstZeile*) calloc( tabelSize, sizeof(tstZeile));

	
	// Dummy Tabelle erstellen
	numOfElements = 0;
	tstMenue menue1;
	menue1.id = 0;
	strncpy(menue1.titel,"Hauptmenue",TITELLANG);	
	Tabelle[numOfElements].id = numOfElements;
	Tabelle[numOfElements].pos = 0;
	Tabelle[numOfElements].element = &menue1;
	Tabelle[numOfElements].elementType = MENUE;
	Tabelle[numOfElements].parent = 0;
	numOfElements++;
	
	tstVariable var1;
	var1.id = numOfElements;
	var1.value.uchar = 120;
	var1.valueType = UCHAR;
	strncpy(var1.titel,"Drehzahl",TITELKURZ);	
	Tabelle[numOfElements].id = numOfElements;
	Tabelle[numOfElements].pos = 1;
	Tabelle[numOfElements].element = &var1;
	Tabelle[numOfElements].elementType = VARIABLE;
	Tabelle[numOfElements].parent = 0;
	numOfElements++;
	
	tstVariable var2;
	var2.id = numOfElements;
	var2.value.sshort = 10;
	var2.valueType = SSHORT;
	strncpy(var2.titel,"Temperatur",TITELKURZ);	
	Tabelle[numOfElements].id = numOfElements;
	Tabelle[numOfElements].pos = 2;
	Tabelle[numOfElements].element = &var2;
	Tabelle[numOfElements].elementType = VARIABLE;
	Tabelle[numOfElements].parent = 0;
	numOfElements++;
	

	
	
	sei();
	draw_title();
	display_menue();
	
	while(1)
	{
		/****************** Taste  ENTER *********************/
		if(get_key_press(1<<ENTER))
		{	
			switch (get_type_of_element(aktivesElement))
			{
				case VARIABLE: enter_variable();	break;
				case MENUE:	enter_menue(); break;
				case TEXT: break;
			}
		}
		
		/****************** Taste  UP ************************/
		if(get_key_press(1<<UP) || get_key_rpt( 1<<UP ))
		{
			uint8_t k = get_num_of_elements(aktivesElement);
			
			switch (get_type_of_element(aktivesElement))
			{
				case VARIABLE:
					if(cursorPosition > 0)
						value.ulong += 1*pow(10,cursorPosition-1);
					else
						((tstVariable*) get_element(aktivesElement))->bChanged ^= 1;
						lcd_gotoxy(0,2); 
						lcd_command(LCD_CLEAR);
					break;
					
				case MENUE:
					cursorPosition--;
					
					if(cursorPosition <= 0)
						cursorPosition = k + 1;
					break;
				case TEXT: break;
			}
		}
		
		
		/****************** Taste  DOWN **********************/
		if(get_key_press(1<<DOWN) || get_key_rpt( 1<<DOWN))
		{
			uint8_t k = get_num_of_elements(aktivesElement);
		
			switch (get_type_of_element(aktivesElement))
			{
				case VARIABLE: 
					if(cursorPosition > 0)
						value.ulong -= 1*pow(10,cursorPosition-1); 
					else
						((tstVariable*) get_element(aktivesElement))->bChanged ^= 1;
						
					lcd_gotoxy(0,2); 
					lcd_command(LCD_CLEAR);	
					break;
					
				case MENUE:
					cursorPosition++;
					
					if(cursorPosition > k +1)
						cursorPosition = 1;
					break;
				case TEXT: break;
			}
		}
		
		/*************** Pakete lesen ***********************/
		if(twi_status.INDAV)
		{
			getTwiPaket();
		}
		
		
		/****************** ANZEIGE *************************/
		switch (get_type_of_element(aktivesElement))
		{
			case VARIABLE: display_variable();break;
			case MENUE: display_menue();break;
			case TEXT: break;
		}
		
	}	
}





//------------------------ Variable -------------------------------------------
/// \todo 
// - wechseln des aktivenElementes
// - Dastellung der Variablenbearbeitung
// - Wert ändern mit Cursor
// - Wert speichern
// - 

void enter_variable(void)
{
	//Abspeichern/Abbrechen und zurück
	if(cursorPosition == -3)
	{
		// Abspeichern
		if(((tstVariable*) get_element(aktivesElement))->bChanged)
			((tstVariable*) get_element(aktivesElement))->value.ulong = value.ulong;
		switch_to_menue();
	}
	else
	{
		cursorPosition--;
		if(cursorPosition <= 0)
			cursorPosition = -3;
	}
}
void display_variable(void)
{
	char strValue[30];
	
	// cursorzeichnen
	if(cursorPosition < 20)
	{
		lcd_gotoxy(19 - cursorPosition , 1);
		lcd_putc(' ');
		lcd_putc(0x5E);
		lcd_gotoxy(19 - cursorPosition , 3);
		lcd_putc(' ');
		lcd_putc('v');
	}
	else
	{
		lcd_gotoxy(20 - cursorPosition , 1);
		lcd_putc(0x5E);
		lcd_gotoxy(20 - cursorPosition , 3);
		lcd_putc('v');
	}
	if(cursorPosition == -3)
	{
		lcd_gotoxy(19 , 1);
		lcd_putc(' ');
		lcd_gotoxy(19 , 3);
		lcd_putc(' ');
	}

	// CursorPosZeichnen
	char text[50];
	itoa(cursorPosition,text,10);
	lcd_gotoxy(25,0);
	lcd_puts(text);
	
	// gloabel kopie des Variablen wertes
	switch (((tstVariable*) get_element(aktivesElement))->valueType)
	{
		case FLAG: 
			if( (uint8_t) value.flag == 0)
				strcpy(strValue,"OFF");	
			else
				strcpy(strValue,"ON");	
			break;
		case UCHAR:
			itoa(((uint8_t) value.uchar), strValue, 10);
			break;
		case SCHAR:
			itoa(((int8_t) value.schar), strValue, 10);
			break;
		case USHORT:
			itoa(((uint16_t) value.ushort), strValue, 10);
			break;
		case SSHORT:
			itoa(((int16_t) value.sshort), strValue, 10);
			break;
		case ULONG:
			ultoa(((uint32_t) value.ulong), strValue, 10);
			break;
		case SLONG:
			ltoa(((int32_t) value.slong), strValue, 10);
			break;						
		
		default:
			strcpy(strValue,"unknown type");
	}
	lcd_gotoxy(17 - strlen(strValue) ,2);
	lcd_puts("   ");
	lcd_puts(strValue);	
	
	// OK / Cancel
	lcd_gotoxy(20, 2);
	lcd_puts(" ");
	if(((tstVariable*) get_element(aktivesElement))->bChanged)
		lcd_puts("  OK  ");
	else
		lcd_puts("cancel");
}



//------------------------ Menue ----------------------------------------------
// TODO:
// - Elementwechseln()		x
// - Anzeige des Menüs()	x  
// - Inhalt ermitteln			x
// - Inhalt anzeigen()		x
// - cursor darstellen()	x
// - zurückbutton					x
// - Werte von Variablen anzeigen 	x


void enter_menue(void)
{
	switch (get_type_of_element(selectedElement))
	{
		case VARIABLE: switch_to_variable(); break;	
		case MENUE: switch_to_menue();	break;
		case TEXT:break;
	}
}

void switch_to_menue(void)
{
	// Display löschen
	lcd_gotoxy(0,2);
	lcd_command(LCD_CLEAR);	
	lcd_gotoxy(0,0);
	lcd_command(LCD_CLEAR);	
	
	// aktives Element wechseln
	aktivesElement = selectedElement;
	selectedElement = 0;
	cursorPosition = 0;
	menueOffset = 0;
	
	// Titel schreiben
	draw_title();
}

void switch_to_variable(void)
{
	char strValue[30];
	uint8_t parentElement;
	// Display löschen
	lcd_gotoxy(0,2);
	lcd_command(LCD_CLEAR);	
	lcd_gotoxy(0,0);
	lcd_command(LCD_CLEAR);
	
	// aktives Element wechseln
	parentElement = aktivesElement;
	aktivesElement = selectedElement;
	selectedElement = parentElement;
	cursorPosition = 20;
	// enum enVariablenTypen {FLAG, UCHAR, USHORT, ULONG, SCHAR, SSHORT, SLONG, REAL};
	switch (((tstVariable*) get_element(aktivesElement))->valueType)
	{
		case FLAG: 		value.flag 	= (uint8_t) ((tstVariable*) get_element(aktivesElement))->value.flag; 	itoa(value.flag,strValue,10); break;
		case UCHAR:		value.uchar = (uint8_t) ((tstVariable*) get_element(aktivesElement))->value.uchar; 	itoa(value.uchar,strValue,10);break;
		case SCHAR:		value.schar = ( int8_t) ((tstVariable*) get_element(aktivesElement))->value.schar;	itoa(value.schar,strValue,10); break;
		case USHORT:	value.ushort = (uint16_t) ((tstVariable*) get_element(aktivesElement))->value.ushort;	itoa(value.ushort,strValue,10); break;
		case SSHORT:	value.sshort = ( int16_t) ((tstVariable*) get_element(aktivesElement))->value.sshort;	itoa(value.sshort,strValue,10); break;
		case ULONG:		value.ulong = (uint32_t) ((tstVariable*) get_element(aktivesElement))->value.ulong;	ultoa(value.ulong,strValue,10); break;
		case SLONG:		value.slong = ( int32_t) ((tstVariable*) get_element(aktivesElement))->value.slong;	ltoa(value.slong,strValue,10); break;
		case REAL:
		default: break;
	}
	cursorPosition = strlen(strValue);
	// Titel schreiben
	draw_title();
}

/*****************************************************************************/
/*!	\brief das aktive Menü wird auf dem Display dargestellt
* 
*	Dabei wird der Inhalt je nach Elemententyp unterschiedlich dargestellt
* und an das ende ein **Zurueck** angefügt. Im Display erscheinen nur drei
* Elemente, die der Cursorposition entsprechen.
*
******************************************************************************/
// stellt das aktive Menü dar
void display_menue(void)
{
	uint8_t idListe[ELEMENTE];
	uint8_t i, anzInhalt;
	
	// anzahl der Kinderelemente
	anzInhalt = get_num_of_elements(aktivesElement);

	// Inhalt ermitteln
	for(i = 0; i < numOfElements; i++)
	{
		if((Tabelle[i].parent == aktivesElement) && (Tabelle[i].id != Tabelle[i].parent))
		{
			idListe[Tabelle[i].pos] = i;
		}
	}	
	// Fügt Zurück hinzu
	idListe[anzInhalt + 1] = Tabelle[aktivesElement].parent;
	anzInhalt++;
	
	// Menünummerierung aktualisieren
	char text[50];
	itoa(anzInhalt,text,10);
	lcd_gotoxy(26,0);
	lcd_puts(text);		
	itoa(selectedElement,text,10);
	lcd_gotoxy(24,0);
	lcd_puts(text);
	lcd_putc('/');
	switch (get_type_of_element(selectedElement))
	{
		case VARIABLE:	
			lcd_gotoxy(23,0);
			lcd_putc('V');
			break;	
		case MENUE:
			lcd_gotoxy(23,0);
			lcd_putc('M');
			break;
		case TEXT:
			lcd_gotoxy(23,0);
			lcd_putc('T');
			break;
	}
	
	// Zeichnen des Inhalts
	if(cursorPosition == 0)
	{
		draw_inhalt(idListe, anzInhalt);
		cursorPosition = 1;
	}
	else
	{
		selectedElement = idListe[cursorPosition];
	}
	if((cursorPosition - menueOffset) > 3)
	{
		menueOffset = cursorPosition  - 3;
		draw_inhalt(idListe, anzInhalt);
	}
	if((cursorPosition - menueOffset) <= 0)
	{
		menueOffset = cursorPosition - 1;
		draw_inhalt(idListe, anzInhalt);
	}
	if(reDrawAll)
	{
		draw_title();
		draw_inhalt(idListe, anzInhalt);
		reDrawAll = 0;
	}
	// Zeichnet die Pfeile
	draw_cursor();
}


/*****************************************************************************/
/*!	\brief Schreibt den Menüinhalt auf das Display
* 
*	Dabei wird der Inhalt je nach Elemententyp unterschiedlich dargestellt
* und an das ende ein **Zurueck** angefügt. Im Display erscheinen nur drei
* Elemente, die der Cursorposition entsprechen.
*
*	\param[in] liste ist eine Liste von NULL terminierten Strings aller Menü-
*				   		einträge.
*	\param[in] anz ist die Anzahl der Menüeinträge
******************************************************************************/
void draw_inhalt(uint8_t* liste, uint8_t anz)
{

	int i;
	char strValue[20];

	for( i = 1; i<=3; i++)
	{
		if(( i + menueOffset) > anz)
			break;
		
		lcd_gotoxy(3,i);
		lcd_putc(0x7C);

		if(( i + menueOffset) == anz)
		{
			lcd_puts("*****ZURUECK*******");
		}
		else {		
			switch (get_type_of_element(liste[i+menueOffset]))
			{
				case VARIABLE:	
					lcd_puts((char*) ((tstVariable*) get_element(liste[i+menueOffset]))->titel);
					for(uint8_t k = strlen((char*) ((tstVariable*) get_element(liste[i+menueOffset]))->titel ); k<=19;k++)
					{
						lcd_putc(' ');
					}
					
					// TODO: VALUE schreiben
					// enum enVariablenTypen {FLAG, UCHAR, USHORT, ULONG, SCHAR, SSHORT, SLONG, REAL};
					switch (((tstVariable*) get_element(liste[i+menueOffset]))->valueType)
					{
						case FLAG: 
							if( (uint8_t) ((tstVariable*) get_element(liste[i+menueOffset]))->value.flag == 0)
								strcpy(strValue,"OFF");	
							else
								strcpy(strValue,"ON");	
							break;
						case UCHAR:
							itoa(((uint8_t) ((tstVariable*) get_element(liste[i+menueOffset]))->value.uchar), strValue, 10);
							break;
						case SCHAR:
							itoa(((int8_t) ((tstVariable*) get_element(liste[i+menueOffset]))->value.schar), strValue, 10);
							break;
						case USHORT:
							itoa(((uint16_t) ((tstVariable*) get_element(liste[i+menueOffset]))->value.ushort), strValue, 10);
							break;
						case SSHORT:
							itoa(((int16_t) ((tstVariable*) get_element(liste[i+menueOffset]))->value.sshort), strValue, 10);
							break;
						case ULONG:
							ultoa(((uint32_t) ((tstVariable*) get_element(liste[i+menueOffset]))->value.ulong), strValue, 10);
							break;
						case SLONG:
							ltoa(((int32_t) ((tstVariable*) get_element(liste[i+menueOffset]))->value.slong), strValue, 10);
							break;						
						
						default:
							strcpy(strValue,"unknown type");
					}
					lcd_gotoxy(22 - strlen(strValue) ,i);
					lcd_putc(' ');
					lcd_puts(strValue);					
					break;
					
				case MENUE:
					lcd_puts("**               **");
					lcd_gotoxy( 13 - strlen((char*) ((tstMenue*) get_element(liste[i+menueOffset]))->titel )/2,i);
					lcd_puts((char*) ((tstMenue*) get_element(liste[i+menueOffset]))->titel );
					break;
				case TEXT:
					lcd_puts((char*) ((tstText*) get_element(liste[i+menueOffset]))->titel);
					break;
				default:
					lcd_puts( "Fehler in draw_inhalt()!");
			}
		}
		lcd_gotoxy(23,i);
		lcd_putc(0x7C);
	}	
}

/*****************************************************************************/
/*!	\brief Zeichnet die Cursorpfeile
* 
*	Die Funktion draw_cursor zeichnet die Curosrpfeile an die Position die
* in der globalen Variable cursorPosition steht.
* Dies reduziert ein Flackern auf dem Display.
*
******************************************************************************/
void draw_cursor(void)
{
	uint8_t i;
	for(i=1;i<=3;i++)
	{	
		if(cursorPosition == i + menueOffset)
		{
			lcd_gotoxy(2,i);
			lcd_putc(0x7E);
			lcd_gotoxy(24,i);
			lcd_putc(0x7F);
		}
		else
		{
			lcd_gotoxy(2,i);
			lcd_putc(' ');
			lcd_gotoxy(24,i);
			lcd_putc(' ');
		}		
	}
}

/*****************************************************************************/
/*!	\brief Zeichnet den Titel des AktivenElements
* 
*	Die Funktion draw_title zeichnet den Titel des aktivenElements.
*
******************************************************************************/
void draw_title(void)
{
	char* titel;
	uint8_t i;
	
	switch (get_type_of_element(aktivesElement))
	{
		case VARIABLE:	
			titel = ((tstVariable*) get_element(aktivesElement))->titel;
			break;
		case MENUE:
			titel = ((tstMenue*) get_element(aktivesElement))->titel;
			break;
		case TEXT:
			titel = ((tstText*) get_element(aktivesElement))->titel;
			break;
		default:
			lcd_gotoxy(0,0);
			titel = "Fehler in draw_title()!";
	}	

	lcd_gotoxy(0,0);
	for( i = 0; i < 27; i++)
	{		
		lcd_putc(0xA5);
	}
	lcd_gotoxy(13-(strlen(titel)/2),0);
	lcd_puts((char *)titel);
}

/*****************************************************************************/
/*!	\brief gibt den Typ eines Elementes zurück
* 
*	Die Funktion type_of_element liefert den Typ des angegebenen Elementes
* zurück. Dabei wird die ElementenID angegeben.
*
*	\param[in] id ist die ID des gesuchten Elementes.
*	\return lierfert den ELementenTyp zurück. /see enum
******************************************************************************/
uint8_t get_type_of_element(uint8_t id)
{
	for(int i = 0; i<numOfElements; i++)
	{
		if(Tabelle[i].id == id)
		{
			return Tabelle[i].elementType;
		}
	}
	return 0;
}

/*****************************************************************************/
/*!	\brief gibt den Pointer des Elementes zurück
* 
*	Die Funktion get_element liefert den pointer auf das gesuchte Element 
* zurück. Wurde das Element nicht gefunden liefert die Funktio 0 zurück;
*
*	\param[in] id ist die ID des gesuchten Elementes.
*	\return Pointer des gesuchten Elementes oder null
******************************************************************************/
// gibt den Pointer des Elementes zurück
void* get_element(uint8_t id)
{
	for(int i = 0; i<numOfElements; i++)
	{
		if(Tabelle[i].id == id)
		{
			return Tabelle[i].element;
		}
	}
	return 0;
}

/*****************************************************************************/
/*!	\brief gibt Anzahl der KinderElemente die zu einer ID gehören zurück
* 
* 
*	\param[in] id ist die ID des gesuchten Elementes.
*	\return anzahl der "Kinderelemente"
******************************************************************************/
uint8_t get_num_of_elements(uint8_t id)
{
	uint8_t k, i;

	k = 0;
	for(i = 0; i < numOfElements; i++)
	{
		if((Tabelle[i].parent == id) && (Tabelle[i].id != Tabelle[i].parent))
		{
			k++;
		}
	}
	return k;
}	

/*****************************************************************************/
/*!	\brief blockiert das Programm, bis eine Taste(UP, DOWN oder ENTER)
* 	gedrückt wurde.
* 
*	
******************************************************************************/
void wait_until_keypressed(void)
{
	while(1) 
	{ 
		// druecke Taste 0
		if( get_key_press( 1<<ENTER) || get_key_press( 1<<UP) 
				|| get_key_press( 1<<DOWN))
		{
			break;
		}
		asm("nop");
	}
}

/*****************************************************************************/
/*!	\brief Paket aus Twi INFifo lesen 
* 
*	
******************************************************************************/
void getTwiPaket(void)
{
	tstZeile Zeile;
	uint8_t* pZeile = (uint8_t*) &Zeile;	// pointer auf die Zeile aus der Tabelle
	uint8_t	sizeOfElement = 0;	// größe des Elementes in byte
	uint8_t* pElement = 0;			// pointer auf ein später allociertes Element
	uint8_t* pTemp = 0;					// temporärer pointer um Daten zu kopieren
	uint8_t data;
	uint8_t i;
	
	// Tabellenzeile auslesen
	for( i=0; i < sizeof(tstZeile); i++)
	{
		fifo_get(&inFifo, pZeile++);
	}
	
	// wenn es diese ID bereits gibt, wird das alte element gelöscht
	if(get_element(Zeile.id))
	{
		//free(Tabelle[Zeile.id].element);
		Tabelle[Zeile.id].element = 0;
	}
	else
	{
		if(tabelSize < (numOfElements + (ELEMENTE/2)))
		{
			tabelSize = tabelSize + ELEMENTE;
			Tabelle = (tstZeile*) realloc( Tabelle, tabelSize * sizeof( tstZeile));
			memset(Tabelle + ((tabelSize - ELEMENTE)*sizeof( tstZeile)), 0, (tabelSize - ELEMENTE) * sizeof( tstZeile));
		}
		numOfElements++;
	}
	
	// größe des neuen Elementes ermitteln
	switch(Zeile.elementType)
	{
		case VARIABLE: sizeOfElement = sizeof(tstVariable);
			break;
		case MENUE: sizeOfElement = sizeof(tstMenue);
			break;
		case TEXT: sizeOfElement = sizeof(tstText);
			break;
	}
	
	// speicher allocieren
	pElement = malloc( sizeOfElement );
	pTemp = pElement;
	
	//  speicher konnte erfolgreich allociert werden.
	if(pElement != 0)	
	{
		// Element aus Fifo kopieren
		for( i=0; i < sizeOfElement; i++)
		{
			fifo_get(&inFifo, pTemp++);
		}
	}

	// Einträge in Tabelle machen
	Tabelle[Zeile.id].id 					= Zeile.id;
	Tabelle[Zeile.id].element 		= pElement;
	Tabelle[Zeile.id].pos 		 		= Zeile.pos;
	Tabelle[Zeile.id].elementType	= Zeile.elementType;
	Tabelle[Zeile.id].parent			= Zeile.parent; 			


	// wenn noch daten im Fifo sind, diesen leeren
	while(inFifo.count !=0)
	{
		fifo_get(&inFifo, &data);
	}
	
	// TWI Fifo wieder als bereit signalisieren
	twi_status.INDAV = 0;
	twi_register.status.INBAV = 1;
	
	// Display neu zeichenen
	reDrawAll = 1;
}
