/***
**		Ports und pins und einstellungen für 	**
**		den AD Wandler							**
***/
#ifndef _ADCONVERT_H
#define _ADCONVERT_H

#define DDR_AD DDRA
#define PORT_AD PORTA

// interne 2.56V Referenzspannung
#define AD_REF (1<<REFS0)|(1<<REFS1)	//(1<<REFS0)|(1<<REFS1)

// frequenzvorteiler
#define AD_PRE (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)	//(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)

// Anzahl aus dem der Mittelwert gebildet wird
#define MITTELWERT 8	// 1

extern uint16_t get_adc(uint8_t adc_pin);
#endif
