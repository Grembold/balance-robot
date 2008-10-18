#include <avr/io.h> 			//I/O-Register 
#include <avr/interrupt.h>
#include <util/twi.h> 		// enthält z.B. die Bezeichnungen für die Statuscodes in TWSR

#include <stdlib.h> 			// itoa, wandlung von Integer nach Ascii

#include "globals.h"
#include "twi_slave.h"

//ACK nach empfangenen Daten senden/ ACK nach gesendeten Daten erwarten
#define TWCR_ACK TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC);  
//NACK nach empfangenen Daten senden/ NACK nach gesendeten Daten erwarten     
#define TWCR_NACK TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC);
//switched to the non adressed slave mode...
#define TWCR_RESET TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC);


/****************************************************************************/
/*!
*  \brief
*  Initaliserung des TWI-Inteface. Muss zu Beginn aufgerufen werden, sowie bei
*	 einem Wechsel der Slave Adresse 
*	\param adr gewünschte Slave-Adresse
*  
*****************************************************************************/
void twi_slave_init (uint8_t adr)
{
	fifo_init(&inFifo, inBuffer, BUF_SIZE);
	fifo_init(&outFifo, outBuffer, BUF_SIZE);

	TWAR= adr; 																		// Adresse setzen
	TWCR &= ~(1<<TWSTA)|(1<<TWSTO);							// START und STOP deaktivieren
	TWCR|= (1<<TWEA) | (1<<TWEN) | (1<<TWIE);		// aktivieren von: Acknowledge, Interrupt und TWI 
	twi_status.register_adr=0x00;									// registeradresse zurücksetzen
	twi_status.bFirstAccess = 1;									// erster zugriff nach stop
	twi_status.INDAV = 0;
	twi_register.status.INBAV = 1;

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
			
		/***   Daten Empfangen   ****/	
		case TW_SR_DATA_ACK: 		// 0x80 Slave Receiver,Daten empfangen
			data = TWDR; 					// Empfangene Daten auslesen
			
			if(twi_status.bFirstAccess)					// erte Adressierung nach STOP
			{				
				twi_status.register_adr = data;		// Empfangene Register Adresse speciern
				twi_status.bFirstAccess = 0;
				
				// Prüfen ob der IN-Fifo bereit ist				
				if(data == INBUFFER)
				{
					if(twi_status.INDAV == 0 && twi_register.status.INBAV == 1)
					{	
						twi_register.status.INBAV = 0;
						TWCR_ACK;
					}else {
						TWCR_NACK;
					}
				}					
			}
			else	// weiterer schreibzugriff mit Daten
			{
				// Daten in Register schreiben
				switch( twi_status.register_adr)
				{
					case INBUFFER:
						twi_register.INBuffer= data;
						fifo_put(&inFifo, data);
						TWCR_ACK;					
						break;
						
					case INCOUNTER:
						twi_register.INCounter = data;
						// entsprichte die empfangene anzahl mit der angegebenen wir DAV auf 1 gesetzt
						if(data == inFifo.count)
							twi_status.INDAV = 1;
						TWCR_ACK;
						break;
						
					case STATUS:
						twi_register.status.ucStatus = data;
						TWCR_ACK;
						break;
						
					case OUTBUFFER:
					case OUTCOUNTER:
					case PAKETREQID:
					case GPIO:
					default:
						TWCR_NACK;
						break;
				}	// End switch				
			}	// End If			
			break;
			
		/***   Daten versenden   ***/
		case TW_ST_SLA_ACK: 		// 0xA8 Slave Transmitter wurde adressiert
		case TW_ST_DATA_ACK: 		// 0xB8 Slave Transmitter, weitere Daten wurden angefordert
			TWCR_ACK;
			break;
			
		case TW_ST_DATA_NACK:		// 0xC0 Keine Daten mehr gefordert 
		case TW_SR_DATA_NACK:		// 0x88 Keine Daten weiteren empfangen
		case TW_ST_LAST_DATA:		// 0xC8  Last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received
		case TW_SR_STOP: 				// 0xA0 STOP empfangen
		default:								// alle anderen codes
				twi_status.bFirstAccess = 1;
				TWCR_RESET; 				// Übertragung beenden, warten bis zur nächsten Adressierung
	} // end switch
}// end ISR


/*						if(twi_status.nbData < sizeof(tstZeile))
						{
							*(((uint8_t *)(&Tabelle[1])) + twi_status.nbData) = data;
						}
						twi_status.nbData++;
						twi_status.INDAV = 1;
*/
