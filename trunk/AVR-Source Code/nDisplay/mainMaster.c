/****************************************************************************
Title:    Access serial EEPROM 24C02 using I2C interace
Author:   Peter Fleury <pfleury@gmx.ch> http://jump.to/fleury
File:     $Id: test_i2cmaster.c,v 1.2 2003/12/06 17:07:18 peter Exp $
Software: AVR-GCC 3.3
Hardware: AT90S8515 at 4 Mhz, any AVR device can be used

Description:
    This example shows how the I2C library i2cmaster.S can be used to 
    access a serial eeprom.
    Based on Atmel Application Note AVR300, adapted to AVR-GCC C interface

*****************************************************************************/
#include <avr/io.h>
#include <string.h>				// string methoden
#include <util/delay.h>		// _delay_ms() fuktionen
#include "i2cmaster.h"

#define NDISPLAY_ADD  0x40      // device address of EEPROM 24C64, see datasheet
 #define TITELKURZ	12	// kurtzer Titel für Variablen
 #define TITELLANG 	17	// langer Titel für überschriften
 
enum enTWIRegisterAdr { INBUFFER=1, OUTBUFFER, INCOUNTER, OUTCoOUNTER, PAKETREQID, STATUS, GPIO };// Adressen der Register
enum enElemtenTypen	{VARIABLE, MENUE, TEXT};
enum enVariablenTypen {FLAG, UCHAR, USHORT, ULONG, SCHAR, SSHORT, SLONG, REAL};

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

typedef struct
{
	uint8_t	id;										// ID des Tabelleneintrages (muss eindeutig sein)
	char titel[TITELLANG+1]; 			// Titel des Menüs
}tstMenue;

typedef struct
{
	uint8_t id;									// ID des Elementes
	uint8_t	pos;								// Position innerhalb des Menüs
	void*	element;							// Pointer auf das Element
	enum 	enElemtenTypen elementType;		// Typ des Elementes
	uint8_t	parent;								// übergeortnetes Elemente
}tstZeile;


void i2c_writeData(uint8_t* data, uint8_t size);

int main(void)
{
    unsigned char ret;
		uint8_t i,t;

    DDRB  = 0xff;                              // use all pins on port B for output 
    PORTB = 0b00000011;                        // (active low LED's )

    i2c_init();                                // init I2C interface
	
		
	tstMenue menue;
	menue.id = 3;
	memcpy(menue.titel,"I2C Einstellungen", 18);
	tstZeile tabZeile;
	tabZeile.id = 3;
	tabZeile.pos = 3;
	tabZeile.element = 0;
	tabZeile.elementType = MENUE;
	tabZeile.parent = 0;
	
	tstVariable var1;
	var1.id = 4;
	var1.value.uchar = 64;
	var1.valueType = UCHAR;
	var1.bWriteable = 0;					
	memcpy(var1.titel,"Twi Adresse", 12);
	tstZeile tabZeile2;	
	tabZeile2.id = 4;
	tabZeile2.pos = 1;
	tabZeile2.element = &var1;
	tabZeile2.elementType = VARIABLE;
	tabZeile2.parent = 3;
	
		/* schreibe Daten in den INFifo des DisplayControllers */
    ret = i2c_start(NDISPLAY_ADD + I2C_WRITE);       // set device address and write mode
		PORTB = 0b00000110;
    if ( ret ) {
				PORTB = 0b00001110;
        /* failed to issue start condition, possibly no device found */
        i2c_stop();
        PORTB = 0b00011100;                            // activate all 8 LED to show error */
    }else {
				PORTB = 0b00111000;
        /* issuing start condition ok, device accessible */
				i2c_write(INBUFFER);
				i2c_writeData((uint8_t*)&tabZeile, sizeof(tabZeile));
				i2c_writeData((uint8_t*) &menue, sizeof(menue));
        i2c_stop();                            // set stop conditon = release bus
				PORTB = 0b01110000;
		}
    
		/* schreibe die Anzahl der gesendeten Daten */
		i2c_start_wait(NDISPLAY_ADD + I2C_WRITE);       // set device address and write mode
		i2c_write(INCOUNTER);
		i2c_write(sizeof(tabZeile) + sizeof(menue));
		i2c_stop();

		// eine sekunde nichts tun
		for (t=1; t<=100; t++)         
				_delay_ms(10);   

// schreibe das zweite paket
		ret = i2c_start(NDISPLAY_ADD + I2C_WRITE);
		if ( ret ){
			i2c_stop();
		}
		else{
				// schreibe Paket in Fifo
				i2c_write(INBUFFER);
				i2c_writeData((uint8_t*)&tabZeile2, sizeof(tabZeile));
				i2c_writeData((uint8_t*) &var1, sizeof(tstVariable));
        i2c_stop(); 
		}
			/* schreibe die Anzahl der gesendeten Daten */
		i2c_start_wait(NDISPLAY_ADD + I2C_WRITE);       // set device address and write mode
		i2c_write(INCOUNTER);
		i2c_write(sizeof(tabZeile) + sizeof(tstVariable));
		i2c_stop();



tstVariable variable;
tstZeile tabellenZeile;
for(i =1; i <= 20; i++)
{
	variable.id = i+4;
	variable.value.ushort = 10*i;
	variable.valueType = USHORT;
	variable.bWriteable = 0;
	memcpy(variable.titel,"SPEED", 6);
	tabellenZeile.id = i+4;
	tabellenZeile.pos = i+1;
	tabellenZeile.element = 0;
	tabellenZeile.elementType = VARIABLE;
	tabellenZeile.parent = 3;
	
	// zwei sekunde nichts tun
		for (t=1; t<=20; t++)         
				_delay_ms(10);   

// schreibe das dritte paket
		ret = i2c_start(NDISPLAY_ADD + I2C_WRITE);
		if ( ret ){
			i2c_stop();
		}
		else{
				// schreibe Paket in Fifo
				i2c_write(INBUFFER);
				i2c_writeData((uint8_t*)&tabellenZeile, sizeof(tabZeile));
				i2c_writeData((uint8_t*) &variable, sizeof(tstVariable));
        i2c_stop(); 
		}
			/* schreibe die Anzahl der gesendeten Daten */
		i2c_start_wait(NDISPLAY_ADD + I2C_WRITE);       // set device address and write mode
		i2c_write(INCOUNTER);
		i2c_write(sizeof(tabZeile) + sizeof(tstVariable));
		i2c_stop();
}


    for(;;);	
}

void i2c_writeData(uint8_t* data, uint8_t size)
{
	uint8_t i=0;

	for( i=0; i<size; i++)
	{
		i2c_write(*data++);
	}

}
