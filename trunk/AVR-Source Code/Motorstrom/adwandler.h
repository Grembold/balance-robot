/***
**		Ports und pins und einstellungen für 	**
**		den AD Wandler							**
***/
#ifndef _ADCONVERT_H
#define _ADCONVERT_H

#define DDR_AD DDRA
#define PORT_AD PORTA

// Externe Referenzspannung
#define REF_U	5	//5V Referenzspannung
#define AD_REF 	0	//(1<<REFS0)|(1<<REFS1) interne 2.56V Referenzspannung

// Frequenzvorteiler CPUFreq % 32
#define AD_PRE (1<<ADPS2)|(1<<ADPS0)	//(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0) 128

// Anzahl aus dem der Mittelwert gebildet wird
// soll keiner gebildet werden, dann MITTELWERT 1
#define MITTELWERT 8	// 8

extern uint16_t get_adc(uint8_t adc_pin);
#endif
