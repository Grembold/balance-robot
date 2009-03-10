#include <avr/io.h> 	//I/O-Register 
#include <stdint.h>		//standart Integertypen

#include "adwandler.h"	//benötigt die funktionen aus adwandler.c
#include "motorstrom.h"




uint16_t get_Motorstrom(uint8_t adc_pin)
{
	uint16_t data;
	/* liest den Aktuellen Wert am AD */
	data = get_adc(adc_pin);
	data = (data * 128);		
	data = data/132;			// AD-WERT umrechnen in mV
	data = data*100;
	data = data/(SHUNT_R/10);	// mV umrechnen in mA
	return(data);
}