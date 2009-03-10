/***
**		Einstellungen für 	**
**		Motorstrom			**
***/
#ifndef _MOTORSTROM_H
#define _MOTORSTROM_H

// Wert des Messwiderstandes
#define SHUNT_R 220		// 220 mOhm der des Messwiderstandes

extern uint16_t get_Motorstrom(uint8_t adc_pin);

#endif
