#ifndef _TWI_SLAVE_H
#define _TWI_SLAVE_H   1

#include "../nbot_lib_ATMega644/fifo.h"

#define DEFAULT_SLAVE_ADD  	0x40      // device address
#define NUM_OF_REGISTERS 		7			 		// anzahl der Slaveregister

enum enTWIRegisterAdr { INBUFFER=1, OUTBUFFER, INCOUNTER, OUTCOUNTER, PAKETREQID, STATUS, GPIO };// Adressen der Register

// Twi Register
volatile struct
{
	uint8_t INBuffer;
	uint8_t OUTBuffer;
	uint8_t INCounter;
	uint8_t OUTCounter;
	uint8_t PaketReqID;
	union
	{
		struct
		{
			uint8_t INBAV:1;
			uint8_t OUTDAV:1;
			uint8_t PAKREQ:1;
		};
		uint8_t ucStatus;
	} status;
} twi_register;

// statusregister für TWI
volatile struct {
	volatile uint8_t INDAV:1;
  volatile unsigned bFirstAccess:1; 	// erster Zugriff, Registerposition setzen
	volatile uint8_t register_adr; //"Adressregister" mit Adresse des ausgewählten Registers
} twi_status;

// I/O Buffer
#define BUF_SIZE 255
uint8_t inBuffer[BUF_SIZE];
uint8_t outBuffer[BUF_SIZE];
fifo_t inFifo;
fifo_t outFifo;

// Funktionsprototyp
extern void twi_slave_init (uint8_t adr);


#endif
