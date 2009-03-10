#include <avr/io.h>
#include "i2cmaster.h"
#include "uart.h"
#include "24C64P.h"

#define __need_size_t


int main(void)
{
	uint8_t status;
	uint8_t data;
	

	status = eeprom_init();
	if(status)
		uart_puts("lalalal");
	eeprom_write_byte(0x0005, 47);
	data = eeprom_read_byte(0x0005);
	uart_puti(&data);
}


uint8_t eeprom_init()
{
	i2c_init(); 		// init des I2C interfaces
#if USE_UART
	uart_init();
#endif

	uint8_t ret;		//returnwert
	/* write 0x75 to eeprom address 0x05 (Byte Write) */
    ret = i2c_start(Dev24C64+I2C_READ);       // set device address and write mode
    if ( ret ) {
        // Arbittierung fehlgeschlagen, vileicht devicenummer falsch 
        i2c_stop();
#ifdef USE_UART
		uart_puts("EEPROM initialisiert ... FEHLER!");   	// Fehlermeldung EEprom konnte nicht gefunden werden
#endif
		return 1;
    }else {
#ifdef USE_UART
		uart_puts("EEPROM initialisiert ... OK"); 				//alles geklappt
#endif
        i2c_stop();
		return 0;
	}

	
	return 1;		// bis hier sollte es nicht kommen
}

/** liest ein Byte aus dem externen EEPROM **/
uint8_t eeprom_read_byte (const uint16_t addr) 
{
	uint8_t addrH = addr;
	uint8_t addrL = addr>>8;
	uint8_t result;
	
    /* wait until the device is no longer busy from the previous operation 
		write adress and return Byte */
	i2c_start_wait(Dev24C64+I2C_WRITE);     // set device address and write mode
	i2c_write(addrH);						// set address
	i2c_write(addrL);                       
    i2c_rep_start(Dev24C64+I2C_READ);       // set device address and read mode
    result = i2c_readNak();                 // read one byte
    i2c_stop();
	return result;
}


uint16_t eeprom_read_word (uint16_t addr)
{
	uint8_t addrH = addr;
	uint8_t addrL = addr>>8;
	uint16_t result;
  
  /* wait until the device is no longer busy from the previous operation 
	write adress and return Byte*/
	i2c_start_wait(Dev24C64+I2C_WRITE);     // set device address and write mode
	i2c_write(addrH);
	i2c_write(addrL);                        // write address = 5
    i2c_rep_start(Dev24C64+I2C_READ);       // set device address and read mode
	result = (i2c_readAck()<<8);
    result |= i2c_readNak();                    // read one byte
    i2c_stop();

	return result;
}

/** lschreibt ein Byte in dem externen EEPROM **/
void eeprom_write_byte (uint16_t addr, uint8_t data) 
{
	uint8_t addrH = addr;
	uint8_t addrL = addr>>8;
	
    /* wait until the device is no longer busy from the previous operation 
		write adress and return Byte */
	i2c_start_wait(Dev24C64+I2C_WRITE);     // Ruft den Speicher im schreibmodus auf
	i2c_write(addrH);			// setzt die Speicher addresse
	i2c_write(addrL);
	i2c_write(data);			//schreibt das Byte in den Speicher
    i2c_stop();
}
