#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "uart.h"
#include "i2cMaster.h"

#define Dev24C02  0xA0      // device address of EEPROM 24C64, see datasheet

#define MAXLENGTH 27

void eeprom_put_block(void);

typedef union {
		char lcd_text[4][MAXLENGTH];
}unter_menue;

typedef struct {
		uint8_t unterpunkte;	//anzahl der Unterpunkte die ein Menue hat
		unter_menue uMenue[2];		
}LCDMenue;
#define MENUEPUNKTE 2
LCDMenue menue[MENUEPUNKTE];
LCDMenue lcd;

/**Ziel und Quelle müssen vom gleichen TYP sein **/
void eeprom_put_block(void)
{
	uint8_t ret;
	
	ret = i2c_start(Dev24C02+I2C_WRITE);       // set device address and write mode
    if ( ret ) {
        // failed to issue start condition, possibly no device found
        i2c_stop();
		uart_puts("Fehler!! EEPROM wurde nicht gefunden");
    }else {
		uart_puts("EEPROM richtig initialisiert");
		/* issuing start condition ok, device accessible */
		i2c_write(0x00);
        i2c_write(0x05);                       // write address = 5
        i2c_write(0xAC);                       // ret=0 -> Ok, ret=1 -> no ACK 
        i2c_stop();                            // set stop conditon = release bus

        /* write ok, read value back from eeprom address 0x05, wait until 
           the device is no longer busy from the previous write operation */
        i2c_start_wait(Dev24C02+I2C_WRITE);     // set device address and write mode
		i2c_write(0x00);                        // write address = 5
        i2c_write(0x05);                        // write address = 5
        i2c_rep_start(Dev24C02+I2C_READ);       // set device address and read mode
        ret = i2c_readNak();                    // read one byte
        i2c_stop();
		uint8_t i = 139;
		uart_puti(&i);
		
	}
} 


int main(void)
{
	uart_init();
	i2c_init();
	
	
	eeprom_put_block();
	
	sei();
	return 0;
}
