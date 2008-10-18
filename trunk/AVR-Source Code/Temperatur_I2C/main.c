/*************************************************************************/
/*!
* \file main.c
*
* \brief 
*
* \par Funktionsweise: programm zum auslesen der Temperatur über I2C
*	
* als Vorlage diente das Datenblatt des Atmel Mega 644
*
* \version	V--- - 20.06.2007 - Arne Petersen\n
*			Created\n
*************************************************************************/

#include <avr/io.h> 	//I/O-Register 
#include <stdint.h>		//standart Integertypen
#include <i2cmaster.h>



#define AD7414  0x90      // device address of AD7414 Temperatur see Datasheet
int ret;

 /* Main(), hier geht es los */
int main(void)
{

    DDRB  = 0xff;                              // use all pins on port B for output 
    PORTB = 0x00;   
    DDRA  = 0xFF;                              // use all pins on port A for output 
    PORTA = 0x00;
	
	i2c_init();                                // init I2C interface
	ret = i2c_start(AD7414+I2C_WRITE);       // set device address and read mode
	
	if ( ret ) {
		/* failed to issue start condition, possibly no device found */
		i2c_stop();
		PORTB=0xff;												// activate all 8 LED to show error */
				//PORTA =  ret;                           
	}else {
		i2c_write(0x00);
		i2c_rep_start(AD7414+I2C_READ); 
				ret = i2c_readAck();                       //  "    "    "    "     "    1
				PORTA =  ret;
		ret = i2c_readNak();                       //  "    "    "    "     "    2
		i2c_stop();                             // set stop condition = release bus    
		PORTB =  ret;                            // output byte on the LED's        
	}
	
}
