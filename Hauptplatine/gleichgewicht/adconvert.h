/***
**		Ports und pins und einstellungen für 	**
**		den AD Wandler							**
***/
#ifndef _ADCONVERT_H
#define _ADCONVERT_H 1

#define DDR_AD DDRA
#define PORT_AD PORTA

// interne 2.56V Referenzspannung
#define AD_REF (1<<REFS0)|(1<<REFS1)

// frequenzvorteiler soll zweischen 50khz und 200Khz liegen
// hier 64 -> 172kHz
#define AD_PRE (1<<ADPS2)|(1<<ADPS1)

// Anzahl aus dem der Mittelwert gebildet wird
#define MITTELWERT 8

extern uint16_t ReadChannel(uint8_t mux);
#endif
