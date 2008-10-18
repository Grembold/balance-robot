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
#include <avr/interrupt.h> 	//dient zur behandlung der Interrupts
#include <util/twi.h> 			//enthält z.B. die Bezeichnungen für die Statuscodes in TWSR
#include <stdint.h> 				//definiert den Datentyp uint8_t

/********************
* 	   DEFINES 	    *
********************/
#define SLAVE_ADD  0x47      // device address

//Je nach Statuscode in TWSR müssen verschiedene Bitmuster in TWCR geschreiben werden(siehe Tabellen im Datenblatt!). 
//Makros für die verwendeten Bitmuster:

//ACK nach empfangenen Daten senden/ ACK nach gesendeten Daten erwarten
#define TWCR_ACK TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC);  
//NACK nach empfangenen Daten senden/ NACK nach gesendeten Daten erwarten     
#define TWCR_NACK TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC);
//switched to the non adressed slave mode...
#define TWCR_RESET TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC);  

//Die Bitmuster für TWCR_ACK und TWCR_RESET sind gleich. Dies ist kein Fehler und dient nur der Übersicht!

/****************************
* private globale Variablen *
****************************/
volatile uint8_t register_adr; //"Adressregister" mit adresse des gewählten registers

// statusregister für TWI
volatile struct {
   unsigned bFirstAccess:1; 	// erster Zugriff, Registerposition setzen
} twi_status;

/*******************************
* private Funktions prototypen *
********************************/
void init_twi_slave (uint8_t adr);

/****************
*     main      *
****************/
int main(void)
{
	init_twi_slave(SLAVE_ADD);
	sei();
	while(1);
}


/****************************************************************************/
/*!
*  \brief
*  Initaliserung des TWI-Inteface. Muss zu Beginn aufgerufen werden, sowie bei
*	 einem Wechsel der Slave Adresse 
*	\param adr gewünschte Slave-Adresse
*  
*****************************************************************************/
void init_twi_slave (uint8_t adr)
{
	TWAR= adr; 																		// Adresse setzen
	TWCR &= ~(1<<TWSTA)|(1<<TWSTO);							// START und STOP deaktivieren
	TWCR|= (1<<TWEA) | (1<<TWEN) | (1<<TWIE);		// aktivieren von: Acknowledge, Interrupt und TWI 
	register_adr=0x00;														// registeradresse zurücksetzen
	twi_status.bFirstAccess = 1;									// erster zugriff nach stop
	
	DDRB = 0xff;
	PORTB = 0b00110011;
}


/****************************************************************************/
/*!
*  \brief
*  Interrupt wird ausgelöst ein Ereigniss auf dem Bus ausgelöst wird.
*  Im Register TWSR befindet sich dann ein Statuscode, anhand dessen die 
*  Situation festgestellt werden kann.
*****************************************************************************/
ISR(TWI_vect)
{
	uint8_t data = 0;
	
	switch (TW_STATUS) 			// TWI-Statusregister prüfen und nötige Aktion bestimmen 
	{	
		case TW_SR_SLA_ACK: 		// 0x60 Slave Receiver, wurde adressiert	
			TWCR_ACK; 						// nächstes Datenbyte empfangen, ACK danach
			break;
		case TW_SR_DATA_ACK: 		// 0x80 Slave Receiver,Daten empfangen
			data=TWDR; 						// Empfangene Daten auslesen
			if (twi_status.bFirstAccess)	// erster Zugriff, Registeradresse setzen
			{
				twi_status.bFirstAccess = 0;	// zweiter zugriff nach stop
				register_adr = data; // Registeradresse setzen
				PORTB = data;					
				TWCR_ACK;
			}
			else	// weiterer Zugriff, Daten empfangen
			{
			}
			break;
		case TW_ST_SLA_ACK: 		// ?!?
		case TW_ST_DATA_ACK: 		// 0xB8 Slave Transmitter, weitere Daten wurden angefordert
			break;
		case TW_ST_DATA_NACK:		// 0xC0 Keine Daten mehr gefordert 
		case TW_SR_DATA_NACK:		// 0x88 
		case TW_ST_LAST_DATA:		// 0xC8  Last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received
		case TW_SR_STOP: 				// 0xA0 STOP empfangen
		default:								// alle anderen codes
				twi_status.bFirstAccess = 1;
				TWCR_RESET; 				// Übertragung beenden, warten bis zur nächsten Adressierung
		break;
	} // end switch
}// end ISR


