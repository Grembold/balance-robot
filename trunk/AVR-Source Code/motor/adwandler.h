/***
**		Ports und pins und einstellungen für 	**
**		den AD Wandler							**
***/
#ifndef _ADCONVERT_H
#define _ADCONVERT_H 1

#define DDR_AD DDRA
#define PORT_AD PORTA

// Referenzspannungsquelle Inter/Extern
//#define AD_REF  0						// externe Referenz (min. 2V)
#define AD_REF (1<<REFS0)				// AVCC mit externer Kapazität an AREF pin
//#define AD_REF (1<<REFS0)|(1<<REFS1) 	// interne 2.56V Referenzspannung mit externer Kapazität an AREF pin



// Frequenzvorteiler F_CPU/Vorteiler
//#define AD_PRE (1<<ADPS0) 							// 2
//#define AD_PRE (1<<ADPS1)								// 4
//#define AD_PRE (1<<ADPS1)|(1<<ADPS0) 					// 8
//#define AD_PRE (1<<ADPS2)) 							// 16
//#define AD_PRE (1<<ADPS2)|(1<<ADPS0)					// 32
#define AD_PRE (1<<ADPS2)|(1<<ADPS1) 					// 64
//#define AD_PRE (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0) 		// 128

// Anzahl aus dem der Mittelwert gebildet wird
// soll keiner gebildet werden, dann MITTELWERT 1
// Mittelwert aus maximal 8 Werten
#define MITTELWERT 8	// 8

extern uint16_t get_adc(uint8_t adc_pin);
#endif
