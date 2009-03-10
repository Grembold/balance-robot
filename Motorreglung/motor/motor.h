#ifndef _MOTOR_H
#define _MOTOR_H 1


/** Anschlussbelegung des linken MOTORs **/
#define MOTOR_LINKS_PWM_DDR 		DDRD 	/*!< DDRD Datenrichtung an port an dem die PWM für MOROT_Rechts ist */
#define MOTOR_LINKS_PWM_PORT 		PORTD		
#define MOTOR_LINKS_PWM 				PD5		/*!< PD5 Pin der PWM */
#define MOTOR_LINKS_SHUNT_DDR 	DDRA	/*!< DDRA Port an dem die Strommessungdes Shunt-widerstandes gemacht wird */
#define MOTOR_LINKS_SHUNT 			PA6		/*!< PA6 Pin des Messtroms */
#define MOTOR_LINKS_RICHTUNG_DDR 		DDRC	/*!< DDRA	PORT an dem die Motorrichtung angeschlossen ist */
#define MOTOR_LINKS_RICHTUNG_PORT 	PORTC	/*!< PORTA  Port der Drehrichtung */
#define MOTOR_LINKS_RICHTUNG0 		PC6		/*!< PA4	PIN der MOTORRICHTUNG */
#define MOTOR_LINKS_RICHTUNG1 		PC7		/*!< PA5	die Motorrichtung wird mit zwei Signalen definiert */
	

/** Anschlussbelegung des rechten MOTORs **/
#define MOTOR_RECHTS_PWM_DDR 		DDRD 	/*!< DDRD Port an dem die PWM für MOROT_Rechts ist */
#define MOTOR_RECHTS_PWM_PORT 		PORTD	/*!< PORTD Port der PWM */
#define MOTOR_RECHTS_PWM 			PD4		/*!< PD4 Pin der PWM */
#define MOTOR_RECHTS_SHUNT_DDR 		DDRA	/*!< DDRA Port an dem die Strommessungdes Shunt-widerstandes gemacht wird */
#define MOTOR_RECHTS_SHUNT 			PA7		/*!< PA7 */
#define MOTOR_RECHTS_RICHTUNG_DDR 	DDRC	/*!< DDRA	Datenrichtung an dem die Motorrichtung angeschlossen ist */
#define MOTOR_RECHTS_RICHTUNG_PORT 	PORTC	/*!< PORTA PORT der Motorreichtung */
#define MOTOR_RECHTS_RICHTUNG0 		PC4		/*!< PA6	PIN der MOTORRICHTUNG */
#define MOTOR_RECHTS_RICHTUNG1 		PC5		/*!< PA7	die Motorrichtung wird mit zwei Signalen definiert */

/** Allgemeine Motorenwerte **/
#define MOTOR_SHUNT_R		220				/*!< 220 mOhm Widerstand des Shunts (Messwiderstand) */
#define MOTOR_STROM_MAX		1500			/*!< 1500mA Maximalstrom der Motoren in mA */


/** PWM EInstellungen **/
#define PWM_MAX			0x01FF				/*!< 0x01FF (9bit) die PWM läßt sich mit 8, 9 oder 10bit einstellen */

#define PWM_LINKS		OCR1A 
#define PWM_LINKS_ON	TCCR1A |=  (1<<COM1A1) 
#define PWM_LINKS_OFF   TCCR1A &= ~(1<<COM1A1) 

#define PWM_RECHTS		OCR1B 
#define PWM_RECHTS_ON	TCCR1A |=  (1<<COM1B1) 
#define PWM_RECHTS_OFF  TCCR1A &= ~(1<<COM1B1) 

//#define PWM_TYPE (1<<WGM10)|(1<<WGM12) 				// fast 8-bit PWM aktivieren ( nicht invertiert ) */ 
//#define PWM_TYPE (1<<WGM11)|(1<<WGM12) 				// fast 9-bit PWM aktivieren ( nicht invertiert ) */ 
//#define PWM_TYPE (1<<WGM10)|(1<<WGM11)|(1<<WGM12) 	// fast 10-bit PWM aktivieren ( nicht invertiert ) */ 

//#define PWM_PRE (1<<CS10)								// 1 Timer Prescaler
//#define PWM_PRE (1<<CS11)								// 8 Timer Prescaler
//#define PWM_PRE (1<<CS11)|(1<<CS10)					// 64 Timer Prescaler
//#define PWM_PRE (1<<CS12)								// 256 Timer Prescaler
//#define PWM_PRE (1<<CS12)|(1<<CS10)					// 1024 Timer Prescaler
	
	
/***************************************
**	Funktions Prototypen
/***************************************/
extern void motor_init(void);							// Initialisiert die Motorsteuerung
extern void motor_l(int16_t speed);					// 9bit plus richtung setst die Solldrehzahl des linken Motor
extern uint16_t get_Motorstrom(uint8_t motor_shunt);	// gibt den Motorstrom zurück
extern uint16_t get_Motordrehzahl(void);				// gibt die Drehzahl in Upm des Motors zurück.
extern void drehzahlregelung_rechts(uint16_t solldrehzahl);	// Setzt die Solldrehzahl
volatile int16_t sollstrom_rechts;						// Sollstrom für rechten Motor
void stromreglung_rechts(void);						// Stromregler
void motor_r(int16_t speed);							// 9bit plus richtung setzt die solldrehzahl des rechten Motor
#endif
