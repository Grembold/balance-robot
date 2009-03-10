/**    
* MOTOR
* Dieses Programm soll alle funktionen zu den Motoren beinhalten
*
* V0.1
* DEfines für die Motoren definiert
*
* V0.2
* Der Motorstrom kann ausgelesen werden
* Beim Richtungswechsel oder Speed 0 Blokieren die Motoren kurz
*
* V0.3
* PWM auf 21,6kHz geändert (nicht hörbar)
* PWM läst sich nun mit 9 bit einstellen
*
* V0.4
* Drehzahlmessung hinzugefügt
* Die Ausgabe Über UART lässt sich nun mit einer Taste starten/stopen
* Uart sendet PWM[in %]   Drehzahl[in Upm]  Strom[in mA]
* Fehler in der Drehzahlberechnung behoben
*
* V0.5
* PID-Drehzahlregler eingefügt
*/
#include <util/delay.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <stdint.h> 		//integerwerte wie uint8_t, uint16_t
#include <stdlib.h> 		// itoa() und utoa()

#include "motor.h"
#include "adwandler.h"
#include "timer0.h"

#include "uart.h"

#define DEBUG_STROM 0		// Giebt eine Debugmeldung über uart aus
#define DEBUG_DREHZAHL 0	// Giebt eine Debugmeldung über uart aus


int16_t motor_rechts_sollstrom;
int16_t drehzahl_rechts;			// Drehzahl in U/min

volatile uint16_t counter_alt;		// alter Timerstand
volatile uint16_t counter_neu;		// neuer Timerstand
volatile uint16_t time_delta;		// Zeitdifferens
uint8_t is_not_moving_rechts;		// Zeigt mir einen stehenden Motor an

//volatile int16_t sollstrom_rechts;		// Sollstrom für rechten Motor
volatile int32_t esum_strom_rechts;		// Integrierte Regelabweichung
volatile int32_t esum_drehzahl_rechts;		// Integrierte Regelabweichung
volatile int32_t ealt_drehzahl_rechts;		// vorhergehende Regelabweichung

int16_t m1,m2,m3,m4,m5,m6,m7,m8,m9,m0; 	//Mittelwertbildung Drehzahl


void motor_init(void) 
{ 
	/** DATENRICHTUNG und ANFANGSWERT **/
	// Motor links
	MOTOR_LINKS_PWM_DDR |= (1<<MOTOR_LINKS_PWM); 	// Ausgang
	MOTOR_LINKS_PWM_PORT &=~(1<<MOTOR_LINKS_PWM); 	//0 Anfangswert AUS
	MOTOR_LINKS_RICHTUNG_DDR  |= (1<<MOTOR_LINKS_RICHTUNG0)|(1<<MOTOR_LINKS_RICHTUNG1);	// Ausgang
	MOTOR_LINKS_RICHTUNG_PORT &=~((1<<MOTOR_LINKS_RICHTUNG0)|(1<<MOTOR_LINKS_RICHTUNG1));	//0 Anfangswert AUS
	MOTOR_LINKS_SHUNT_DDR &=~(1<<MOTOR_LINKS_SHUNT);	// Eingang KEIN PULLUP


	// Motor rechts
	MOTOR_RECHTS_PWM_DDR |= (1<<MOTOR_RECHTS_PWM);		// Ausgang
	MOTOR_RECHTS_PWM_PORT &=~(1<<MOTOR_RECHTS_PWM );	//0 Anfangswert AUS
	MOTOR_RECHTS_RICHTUNG_DDR |=(1<<MOTOR_RECHTS_RICHTUNG0)|(1<<MOTOR_RECHTS_RICHTUNG1);		// Ausgang
	MOTOR_RECHTS_RICHTUNG_PORT &=~((1<<MOTOR_RECHTS_RICHTUNG0)|(1<<MOTOR_RECHTS_RICHTUNG1)); 	//0 Anfangswert AUS
	MOTOR_RECHTS_SHUNT_DDR &=~(1<<MOTOR_RECHTS_SHUNT);	// Eingang KEIN PULLUP
	
	/* Impulsgeber rechts */	
	DDRD &= ~(1<<PD2);		//PORTD.2 als Eingang
	PORTD |= (1<<PD2);		//PORTD.2 mit Pullup
	GICR |= (1<<INT0);		// External Interrupt Request 0 Enable
	MCUCR |= (1<<ISC00)|(1<<ISC00);	// Die steigende Flanke an INT0 erzeugt einen Interrupt. 	
	
	/* fast 9-bit PWM aktivieren ( nicht invertiert ) */ 
	TCCR1A = (1<<WGM11); 
	/* Einstellen der PWM-Frequenz auf ca. 21,6kHz ( Prescaler = 1 ) */ 
	TCCR1B = (1<<CS10)|(1<<WGM12); 
	/* PWN einschalten -- > PIN mit PWM verbinden */ 
	PWM_LINKS_ON; 
	PWM_RECHTS_ON;

	/* Globale Variablen initialisieren */
	m1=m2=m3=m4=m5=m6=m7=m8=m9=m0=0x00;	//zum Mittelwertbilden der Drehzahl
	time_delta=0x00;
	counter_alt=0;							// alter Timerstand
	counter_neu=0;							// neuer Timerstand
	sollstrom_rechts=0;						// sollstrom
	esum_strom_rechts=0;					// Integrator Anfangswert		
	esum_drehzahl_rechts=0;					// Integrator Anfangswert
	ealt_drehzahl_rechts=0;					// alte Regelabweichung
	
} 

/** setzt die Richtung und die Geschwindigkeit des linken Motors **/
void motor_l(int16_t speed) // Linker Motor(A)
{
	if(speed<0) 			//bei negativen zahlen rückwärts fahren
	{
		speed= (~speed)+1;	// bildet das zweierkomplement zurück
		MOTOR_LINKS_RICHTUNG_PORT &= ~(1<<MOTOR_LINKS_RICHTUNG1);	//0
		MOTOR_LINKS_RICHTUNG_PORT |=(1<<MOTOR_LINKS_RICHTUNG0);	//1
	}
	else{					//bei positiven, vorwärts
		MOTOR_LINKS_RICHTUNG_PORT &= ~(1<<MOTOR_LINKS_RICHTUNG0);	//0	
		MOTOR_LINKS_RICHTUNG_PORT |=(1<<MOTOR_LINKS_RICHTUNG1);	//1		
	}
	if(speed>(PWM_MAX)) speed=PWM_MAX;	// begrenzt den speed auf maximum
	PWM_LINKS = speed; 					// PWM mit 9Bit
}

/** setzt die Richtung und die Geschwindigkeit des rechten Motors **/
void motor_r(int16_t speed) // Rechter Motor(B)
{
	if(speed<0) 		//bei negativen zahlen rückwärts fahren
	{
		speed= (~speed)+1;	// bildet das zweierkomplement zurück
		MOTOR_RECHTS_RICHTUNG_PORT &= ~(1<<MOTOR_RECHTS_RICHTUNG1);//0
		MOTOR_RECHTS_RICHTUNG_PORT |=(1<<MOTOR_RECHTS_RICHTUNG0);	//1
		
	} else {			//bei 0 oder positiven, vorwärts
		MOTOR_RECHTS_RICHTUNG_PORT &= ~(1<<MOTOR_RECHTS_RICHTUNG0);//0
		MOTOR_RECHTS_RICHTUNG_PORT |=(1<<MOTOR_RECHTS_RICHTUNG1);	//1
	}
	if(speed>(PWM_MAX)) speed=PWM_MAX;	// begrenzt den speed auf maximum
	PWM_RECHTS = speed; 				// PWM mit 9Bit
}

/* Ermittelt den Motorstrom */
uint16_t get_Motorstrom(uint8_t motor_shunt)
{
	uint16_t data;
	
	/* liest den Aktuellen Wert am AD */
	data = get_adc(motor_shunt);

	/* Umrechnen in mA */
	data = (data * 24)/10;
	
	return(data);
}



/* Ermittelt die Motordrehzahl */
/* BUG: keine Unterscheidung der Drehrichtungen */
uint16_t get_Motordrehzahl()
{
	uint32_t tmp;
	

	/* wenn 4 mal kein Interrupt kam, steht der Motor */
	if(is_not_moving_rechts<5) 
	{
		/* gleitender Mittelwert über 8 Messugen */
		m8=m7;m7=m6;m6=m5;m5=m4;m4=m3;m3=m2;m2=m1;m1=time_delta;
		tmp=(m1+m2+m3+m4+m5+m6+m7+m8)/8;	
	
		/* Umrechnung in Upm und begrenzung */
		tmp=tmp*256*2;	//
		if(tmp!=0)
			drehzahl_rechts=(11059000*60)/tmp;	//
		if(drehzahl_rechts>7000) drehzahl_rechts=0;
	}
	else
	{
		/* vier mal kein interrupt setzt die Drehzahl auf 0 */
		//m1=m2=m3=m4=m5=m6=m7=m8=m9=m0=0xff; //Mittelwertbidlung zurücksetzen
		drehzahl_rechts=0;
	}
	if(is_not_moving_rechts<0xff)	is_not_moving_rechts++;	//Motor dreht nicht ++
	
	return drehzahl_rechts;
}



/* der Strom soll über einen PI-Glied geregelt werden */
void stromreglung_rechts()	// alle 6ms
{
	int16_t iststrom;			// Istwert
	int32_t e = 0;				// Regelabwerichung
	uint8_t Kr = 2;				// Verstärkung in Tausenstel
	uint8_t Ki = 2;				// Verstärkung Integrator
	uint8_t Ta = 6;				// Abtastzeit in ms 
	int32_t y = 0;
	
#if DEBUG_STROM
	char buffer[20];
	uart_puts("*\t");
	ltoa(sollstrom_rechts, buffer, 10);
	uart_puts(buffer);
	uart_puts("\t");
#endif	

	/* Iststrom ermitteln */
	iststrom = get_Motorstrom(MOTOR_RECHTS_SHUNT);
	
#if DEBUG_STROM
	ltoa(iststrom, buffer, 10);
	uart_puts(buffer);
	uart_puts("\t");
#endif

	/* Regelabweichung */
	e = (int16_t) (sollstrom_rechts - iststrom);
	if(e<10 && e>-10) e=0;
	
#if DEBUG_STROM
	ltoa(e, buffer, 10);
	uart_puts(buffer);
	uart_puts("\t");
#endif

	/* Integrieren */
	esum_strom_rechts += e;
	if(esum_strom_rechts < 0) { esum_strom_rechts = 0;} 	// Begrenzung I-Anteil
	if(esum_strom_rechts > 60000){ esum_strom_rechts = 60000;}
	
#if DEBUG_STROM
	ltoa(esum_strom_rechts, buffer, 10);
	uart_puts(buffer);
	uart_puts("\t");
#endif

	/* Reglergleichung PI*/
	y = Kr * e + Ki * Ta * esum_strom_rechts / 1000;
	if(y < 0){ y = 0;}
	if(y > PWM_MAX){ y = PWM_MAX;}
	
#if DEBUG_STROM	
	ltoa(y, buffer, 10);
	uart_puts(buffer);
	uart_puts("\r\n");
#endif

	/* neue Stellgrößen */
	motor_r(y);
}



/* Der Drehzahlregler sollte als PID Regler aufgebaut werden */
void drehzahlregelung_rechts(uint16_t solldrehzahl)
{
	uint16_t istdrehzahl;		// Istwert
	int32_t e;					// Regelabwerichung
	uint8_t Kr = 3;				// Verstärkung in 1/Kr P-Anteil
	uint8_t Ki = 3;				// Verstärkung in 1/Kr Integrator
	uint8_t Kd = 2;				// Verstärkung in 1/Kr Differenzierer
	uint8_t Ta = 33;			// Abtastzeit
	int32_t y = 0;			

#if DEBUG_DREHZAHL
	char buffer[10];
	
	uart_puts("#\t");
	ltoa(solldrehzahl, buffer, 10);
	uart_puts(buffer);
	uart_puts("\t");
#endif	

	/* Istdrehzahl ermitteln */
	istdrehzahl= get_Motordrehzahl();
	
#if DEBUG_DREHZAHL	
	ltoa(istdrehzahl, buffer, 10);
	uart_puts(buffer);
	uart_puts("\t");
#endif

	/* Regelabweichung */
	e = (int16_t)(solldrehzahl - istdrehzahl);
	
#if DEBUG_DREHZAHL
	ltoa(e, buffer, 10);
	uart_puts(buffer);
	uart_puts("\t");
#endif

	/* Integrieren */
	esum_drehzahl_rechts +=e;
	if(solldrehzahl==0) esum_drehzahl_rechts -=20;
	if(esum_drehzahl_rechts < 0){ esum_drehzahl_rechts = 0;}		// Begrenzung I-Anteil
	if(esum_drehzahl_rechts > 60000){ esum_drehzahl_rechts = 60000;}
	
#if DEBUG_DREHZAHL
 	ltoa(esum_drehzahl_rechts, buffer, 10);
	uart_puts(buffer);
	uart_puts("\t");
#endif
	
	/* Reglergleichung PID */
	y = e/Kr + Ta * esum_drehzahl_rechts/(1000*Ki) + Kd/Ta * (e - ealt_drehzahl_rechts);
	if(y < 0){ y = 0;}
	if(y > PWM_MAX){ y = PWM_MAX;}	// sollstrombegrenzung
	ealt_drehzahl_rechts = e;	// alte Regelabweichung merken
	
#if DEBUG_DREHZAHL
 	ltoa(y, buffer, 10);
	uart_puts(buffer);
	uart_puts("\r\n");
#endif	
	/* neue Stellgröße */
	//sollstrom_rechts = y;
	motor_r(y);
}



// Interrupt Service Routine bei Flanke an int0(extern) */
ISR(INT0_vect)
{
	//PORTB ^=(1<<PB2);								// Toggel Led, wildes blinken ist immer gut
	counter_neu = (timer0_counter_D1*256) + TCNT0;	// Merke Dir die Uhrzeit
	if(d1_error==0) 								// Wenn kein Timerfehler aufgetreten ist
	{
		if(counter_neu > counter_alt){
			time_delta = counter_neu - counter_alt;}			// ermittel die Zeitdifferenz
		else{
			time_delta = (65535-counter_alt) + counter_neu;}	// ermittel die Zeitdifferenz
	}	
	counter_alt = counter_neu;						// Merke mir die Uhrzeit für die nächste Messung
	is_not_moving_rechts=0;							// Motor hat sich bewegt
	d1_error=0;
}
