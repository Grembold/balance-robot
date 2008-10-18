/*************************************************************************/
/*!
* \file main.c
*
* \brief 
*
* \par Funktionsweise: programm zum testen der UART kommunication
*	
* \version	V--- - 26.03.2007 - Olaf Petersen\n
*			Created\n
*************************************************************************/

#include <avr/io.h> 	//I/O-Register 
#include <stdint.h>		//standart Integertypen
#include <stdlib.h> 	// itoa
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#include "../nbot_lib_ATMega644/lcd.h"
#include "../nbot_lib_ATMega644/timer.h"	// Leider hat der Mega644 andere Timer-Register, so dass ich die Lib in einen anderen ordner kopiert habe
#include "../nbot_lib_ATMega644/taste.h"



 #define TASTE0 PA0     // PiN der Taste 0
 #define TASTE1 PA1     // PiN der Taste 1
 #define TASTE2 PA2     // PiN der Taste 2 
  
	
/* Strukturen */
typedef struct
{
	char 		titel[28];			// Menütitel, maximal 27 Zeichen
	uint8_t		titelLaenge;		// länge des Titels
	char 		eintrag[7][16];		// Array von Menüeintragen; maximal 7 Einträge
									// Die Einträge werden mit leerzeichen auf 15 Zeichen aufgefüllt
	uint8_t		*pVerweis[7];		// Pointer zum passenden Unterpunkt
	uint8_t 	anzEintr;			// Anzahl der Menüeinträge maximal 7	
	uint8_t		offset;				// zum "rollen" des Menues
}menue;


/* Globale Variablen */
int8_t arrow = 0;						// Pfeile auf den Menüeintrag
uint8_t *window;						// Aktuelle Anzeige

/* Funktions prototypen */
void drawMenue(menue *m);
void reDrawMenue(menue *m);


/* Main(), hier geht es los */
int main(void)
{

  DDRA &=~((1<<TASTE0)|(1<<TASTE1)|(1<<TASTE2));  // Tasten als Eingänge
  PORTA |=(1<<TASTE0)|(1<<TASTE1)|(1<<TASTE2);    // pullup aktivieren
	
	DDRC = 0xff;
	PORTC = 0xff;
	DDRB |=(1<<PB3);
	PORTB &=~(1<<PB3);	
	
	for(int32_t i = 30000;i>=0;i--)
	{
		asm("nop");
	}	
	PORTB ^=(1<<PB3);	


	timer2_init();			// Timer2 zur Tastenentprellung starten
	lcd_init(LCD_DISP_ON);	// LCD initialisieren
	
	sei(); // Global Interrupts aktivieren
	lcd_init(LCD_DISP_ON);
	lcd_gotoxy(0, 0);
	lcd_putc(0x7C);
	lcd_putc(0x7E);
	lcd_gotoxy(0, 1);
	lcd_putc(0x7C);
	lcd_putc(0x7E);;
	lcd_gotoxy(0, 2);
	lcd_putc(0x7C);
	lcd_putc(0x7E);;
	lcd_gotoxy(0, 3);
	lcd_putc(0x7C);
	lcd_putc(0x7E);

	lcd_gotoxy(25, 1);
	lcd_puts("<=");
	lcd_gotoxy(25, 2);
	lcd_puts("<=");
	lcd_gotoxy(25, 3);
	lcd_puts("<=");
	lcd_gotoxy(0, 0);

	lcd_gotoxy(0, 0);
	lcd_putc(0xA5);
	lcd_putc(0xA5);
	lcd_putc(0xA5);
	lcd_putc(0xA5);
	lcd_putc(0xA5);
	lcd_putc(0xA5);
	lcd_putc(0xA5);
	lcd_putc(0xA5);
	lcd_puts("Hello World");
	lcd_putc(0xA5);
	lcd_putc(0xA5);
	lcd_putc(0xA5);
	lcd_putc(0xA5);
	lcd_putc(0xA5);
	lcd_putc(0xA5);
	lcd_putc(0xA5);
	lcd_putc(0xA5);

	lcd_gotoxy(13-2, 1);
	lcd_puts("World");

	lcd_gotoxy(5, 3);
	lcd_putc(0xFF);
	lcd_putc(0x7C);
	lcd_putc(0x7E);
	lcd_putc(0x7F);
	lcd_putc(0x7C);
	lcd_putc(0xFF);

	while(1) //----------------------------until keypressed---------------------------------------
	{ 
		// druecke Taste 0
		if( get_key_press( 1<<TASTE0))
		{
			break;
		}
		asm("nop");
	}
	
	
	
	//------------------------------- anlegen der Meüs -------------------------------------------
	//Menüs Anlegen
	menue Hauptmenue;
	menue Info;
	menue Temperatur;
	
		// MENÜ Hauptmenü
	strcpy((char *)Hauptmenue.titel,"nBot Hauptmenue");				// Titel des Menüs
	Hauptmenue.titelLaenge 	= 15;																// länge des Menütitels
																			 //"       |       "  	// mittellinie
	strcpy((char *)Hauptmenue.eintrag[0], "     Info      ");	// Menüeintrag auf 15 Zeichen aufgefüllt
	strcpy((char *)Hauptmenue.eintrag[1], "  Temperatur   ");
	strcpy((char *)Hauptmenue.eintrag[2], "    Uhrzeit    ");
	strcpy((char *)Hauptmenue.eintrag[3], " Einstellungen ");
	strcpy((char *)Hauptmenue.eintrag[4], "   Diagnose    ");
	strcpy((char *)Hauptmenue.eintrag[5], "..:: Ende ::.. ");
	Hauptmenue.pVerweis[0] 	= (uint8_t *)&Info;								//Pointer auf des Untermenü zum Eintrag
	Hauptmenue.pVerweis[1] 	= (uint8_t *)&Temperatur;
	Hauptmenue.pVerweis[2] 	= 0;
	Hauptmenue.pVerweis[3] 	= 0;
	Hauptmenue.pVerweis[4] 	= 0;
	Hauptmenue.pVerweis[5] 	= 0;
	Hauptmenue.anzEintr			= 6;	// Anzahl der Menüeinträge
	Hauptmenue.offset				= 0;	// Offset der drei angezeigten Einträge
	
	// MENÜ  Info	
	strcpy((char *)Info.titel,"Info");											// Titel des Menüs
	Info.titelLaenge = 4;																		// länge des Menütitels
	strcpy((char *)Info.eintrag[0]," Version   0.01");			// Menüeintrag
	strcpy((char *)Info.eintrag[1],"----Zurueck----");	
	Info.pVerweis[0] 	= 0;																	//Pointer auf des Untermenü zum Eintrag
	Info.pVerweis[1] 	= (uint8_t *) &Hauptmenue;
	Info.anzEintr 		= 2;				// Anzahl der Menüeinträge
	Info.offset 			= 0;				// Offset der drei angezeigten Einträge
	
		// MENÜ  Temperatur	
	strcpy((char *)Temperatur.titel,"Temperatur");
	Temperatur.titelLaenge = 10;
	strcpy((char *)Temperatur.eintrag[0],"Hallo du da.Ich");
	strcpy((char *)Temperatur.eintrag[1],"kann hier lei- ");
	strcpy((char *)Temperatur.eintrag[2],"der nur 15 Zei-");
	strcpy((char *)Temperatur.eintrag[3],"chen schreiben ");
	strcpy((char *)Temperatur.eintrag[4],"und das ist zi-");
	strcpy((char *)Temperatur.eintrag[5],"ehmlich doof.  ");
	strcpy((char *)Temperatur.eintrag[6],"----Zurueck----");
	Temperatur.pVerweis[0] 	= 0;
	Temperatur.pVerweis[1] 	= 0;
	Temperatur.pVerweis[2] 	= 0;
	Temperatur.pVerweis[3] 	= 0;
	Temperatur.pVerweis[4] 	= 0;	
	Temperatur.pVerweis[5] 	= 0;
	Temperatur.pVerweis[6] 	= (uint8_t *) &Hauptmenue;
	Temperatur.anzEintr 		= 7;
	Temperatur.offset 			= 0;
	

	
	
	
	//----------------------------------Hauptprogram -----------------------------------------
	
	window = (uint8_t *) &Hauptmenue;			// Startmenue
	drawMenue((menue *)window);				// Zeichnen des Menüs
	
	while(1) 
	{ 
		// druecke Taste 0
		// repeat on long press:	
		if( get_key_press( 1<<TASTE0))																		// 'OK'
		{
			// TODO: Hier hier bestätigen und Menü wechseln
			if((((menue *)window)->pVerweis[arrow-1] != 0)&&(arrow != 0))
			{
				window = (uint8_t *) ((menue *)window)->pVerweis[arrow-1];
				arrow = 0;
				((menue *)window)->offset = 0;
				drawMenue((menue *)window);
			}
			PORTB |=(1<<PB2);
		}	
		
		// druecke Taste 1
		// repeat on long press:
    if( get_key_press( 1<<TASTE1) || get_key_rpt( 1<<TASTE1 ))				// Pfeil runter
		{
			arrow++;
			if(arrow > ((menue *)window)->anzEintr)
				arrow=1;
			reDrawMenue((menue *)window);
		}
		
		
		// druecke Taste 2
		// repeat on long press:	
		if( get_key_press( 1<<TASTE2) || get_key_rpt( 1<<TASTE2 ))				// Pfeil hoch
		{
			arrow--;
			if(arrow <= 0)
				arrow = ((menue *)window)->anzEintr;
			reDrawMenue((menue *)window);
		}
	}

	return 0;
}

/*
*	Zeichnet das Komplette Menü neu
*/
void drawMenue(menue *m)		//Zeichnet das Display neu
{
	int i;
	lcd_gotoxy(0,2);
	lcd_command(LCD_CLEAR);	
	lcd_gotoxy(0,0);
	lcd_command(LCD_CLEAR);	
	
	for(i=0;i<27;i++)
	{		
		lcd_putc(0xA5);
	}
	lcd_gotoxy(13-(m->titelLaenge/2),0);
	lcd_puts((char *)m->titel);
	
	for(i=1; i<4;i++)
	{
		if(i+m->offset > m->anzEintr)
			break;
		lcd_gotoxy(5,i);
		lcd_putc(0x7C);
		lcd_puts(m->eintrag[(i+m->offset)-1]);
		lcd_putc(0x7C);
	}
}


/*
*	Zechnet die Auswahlpfeile und rollt bei Bedarf das Menü
*/
void reDrawMenue(menue *m)
{
	int i;
	
	// Rollt das menue
	if((arrow - m->offset) > 3)
	{
		m->offset = arrow - 3;
		drawMenue(m);
	}
	if((arrow - m->offset) <= 0)
	{
		m->offset = arrow - 1;
		drawMenue(m);
	}
	
		
	// Zeichnet die Pfeile
	for(i=1;i<4;i++)
	{
		lcd_gotoxy(4,i);
		if(arrow == i+m->offset)
			lcd_putc(0x7E);
		else
			lcd_putc(' ');
		
		lcd_gotoxy(22,i);
		if(arrow == i+m->offset)
			lcd_putc(0x7F);
		else
			lcd_putc(' ');
	}
}
