/*************************************************************************/
/*!
* \file main.c
*
* \brief 
*
* \par Funktionsweise: programm zum testen der UART kommunication
*	
* \version	V--- - 26.03.2007 - Olaf Petersen\n
*			Created\n
*************************************************************************/

#include <avr/io.h> 	//I/O-Register 
#include <stdint.h>		//standart Integertypen
#include <stdlib.h> 	// itoa
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "../nbot_lib/encoder.h"
#include "../nbot_lib/uart.h"
#include "../nbot_lib/timer.h"
#include "../nbot_lib/motor.h"
#include "../nbot_lib/command.h"

#define ki 2	/*!< \brief Integrationszeitkonstante */
#define kd 0	/*!< \brief Differenzierungszeitkonstante */
#define kp 2	/*!< \brief Verstärkung */

#define SM_DEFAULT	00
#define SM_MOVE 		01
#define SM_DEMO			02
#define SM_DEBUG		10
#define SM_DB_ENC		11
#define SM_DB_CMD		12
#define SM_DB_REG		13

uint32_t zeit = 0;
uint32_t startzeit = 0;
int16_t sollwert[] = {0,0};	
int16_t istwert[] = {0,0};
int16_t fehler[] = {0,0};
int16_t fehler_alt[] = {0, 0};
int16_t	integrator[] = {0, 0};
int16_t differenzierer[] = {0, 0};
int16_t stellwert[] = {0, 0};

void regelung(uint8_t motor);
void reglerwerte_ausgeben(uint8_t motor);
void encoder_ausgeben(void);

int main(void)
{
	uart_init();
	timer2_init();
	motor_init();
	encoder_init();
	
	DDRD |=(1<<PD2);
	PORTD &=~(1<<PD2);
	
	uart_puts_p(PSTR("\f *** Hallo! ***\r\n"));
	uart_puts_p(PSTR(" Ich bin ein kleiner Roboter.\r\n"));
	uart_puts_p(PSTR("BOT:\\>"));	
	
	uint8_t uart_buf;					// von Uart abgefragtes Zeichen	
	uint8_t sm = SM_DEFAULT;	// Statemashine, Roboterzustand
	uint8_t command;					// Befehl
	
	sei();
	
	for(;;)
	{
		if (Gettime() >= (zeit + 100))		// alle 100ms
		{			
			zeit = Gettime ();	// Zeit erfassen
			
			// *************  Motorregelung ***************
			regelung(LEFT);			// linken Motor regeln
			regelung(RIGHT);		// rechten Motor regeln		
			
			uart_buf = uart_getc();						// tastenabfrage
			command_put(uart_buf);						// fügt der Komandofunktion ein neues Zeichen hinzu
			command = command_get();					// gibt letzten Befehl zurück
			command_prompt(command);					// Textausgabe der Befehle
			
			switch (command)									// Command
			{
				case CMD_MOVE:
					sm = SM_MOVE;
					break;
				case CMD_DEMO:
					startzeit = zeit;
					sm = SM_DEMO;
					break;
				case CMD_DB_ENC: 
					sm = SM_DB_ENC;
					break;
				case CMD_DB_REG: 
					sm = SM_DB_REG;
					break;
				case CMD_UP:
					switch (sm)
					{
						case SM_MOVE:
							sollwert[LEFT] += 20;
							sollwert[RIGHT] += 20;
							break;
					}
					break;
				case CMD_DOWN:
					switch (sm)
					{
						case SM_MOVE:
							sollwert[LEFT] -= 20;
							sollwert[RIGHT] -= 20;
							break;
					}
					break;
				case CMD_LEFT:
					switch (sm)
					{
						case SM_MOVE:
							sollwert[LEFT] = 0;
							break;
					}
					break;
				case CMD_RIGHT:
					switch (sm)
					{
						case SM_MOVE:
							sollwert[RIGHT] = 0;
							break;
					}
					break;
				case CMD_ESCAPE:
					sollwert[LEFT] = 0;
					sollwert[RIGHT] = 0;
					sm = SM_DEFAULT;
					break;
				default: break;
			}	// END switch command
			
			switch (sm)											// Statemashine
			{
				case SM_DEFAULT: break;
				case SM_MOVE: break;
				case SM_DB_ENC:
					encoder_ausgeben();
					break;
				case SM_DB_REG:
					reglerwerte_ausgeben(LEFT);
					reglerwerte_ausgeben(RIGHT);
					break;
				case SM_DEMO:										// DEMO
					switch (zeit-startzeit) 
					{
						case 3000:
							sollwert[LEFT] = 50;
							sollwert[RIGHT] = 50;
							break;
						case 5500:
							sollwert[LEFT] = 0;
							sollwert[RIGHT] = 50;
							break;
						case 8000:
							sollwert[LEFT] = 30;
							sollwert[RIGHT] = 30;
							break;
						case 15000:
							sollwert[LEFT] = 0;
							sollwert[RIGHT] = 50;
							break;
						case 17500:
							sollwert[LEFT] = 60;
							sollwert[RIGHT] = 60;
							break;
						case 20000:
							sollwert[LEFT] = 50;
							sollwert[RIGHT] = 0;
							break;
						case 22500:
							sollwert[LEFT] = 80;
							sollwert[RIGHT] =80;
							break;
						case 25000:
							sollwert[LEFT] = 50;
							sollwert[RIGHT] = 0;
							break;
						case 27500:
							sollwert[LEFT] = 100;
							sollwert[RIGHT] =100;
							break;
						case 30000:
							sollwert[LEFT] = 0;
							sollwert[RIGHT] = 50;
							break;
						case 32500:
							sollwert[LEFT] = 150;
							sollwert[RIGHT] = 150;
							break;
						case 35000:
							sollwert[LEFT] = 0;
							sollwert[RIGHT] = 0;
							break;
					} // END demo					
					break;
				default: break;
			} // END swtich sm
		} // END if (Gettime() >= (zeit + 100))
		
	}
	return 0;
}





/**************************************************************************/
/*!
* \brief Regelung der Motoren.
*
*	\par Funktionsweise:
*	die Funktion liest die Drehzahlwerte ein und erzeugt daraus einen neuen 
* Motorstellwert.
*
* \param [in]
*		motor	Motor auswahl: LEFT, RIGHT
*
***************************************************************************/
void regelung(uint8_t motor)
{
			// alle werte zurücksetzten wenn Motor still stehen soll
			if(sollwert[motor] == 0){
				fehler[motor]=0;
				fehler_alt[motor]=0;
				integrator[motor]=0;
				differenzierer[motor]=0;
				stellwert[motor]=0;			
			}
			
			istwert[motor] = encoderGet(motor);
			
			
			//****** linke Regelung ************************************************ 
			
			fehler[motor] = sollwert[motor] - istwert[motor];					// Sollwertabweichung
			
			integrator[motor] += fehler[motor];												// Integrator
			// Integrator begrenzen
			if(integrator[motor] > 1023)
				integrator[motor] = 1023;
			if(integrator[motor] < (-1023))
				integrator[motor] = (-1023);			
			
			differenzierer[motor] = fehler[motor] - fehler_alt[motor];	// Differenzierer
			
			stellwert[motor]  = integrator[motor]/ki;									// Stellgröße
			stellwert[motor] += kd * differenzierer[motor];
			stellwert[motor] += kp * fehler[motor];
			
			// Stellwertbegrenzung
			if(stellwert[motor]>255)
				stellwert[motor] = 255;
			if(stellwert[motor]< (-255))
				stellwert[motor] = (-255);
				
			fehler_alt[motor] = fehler[motor];													// Sollwertabweichung merken
		
			switch (motor)
			{
				case LEFT:
					// Motordrehrichtung
					if(stellwert[motor]>0)
						motor_dir(FWD,FREE);
					if(stellwert[motor]<0){
						stellwert[motor] = -stellwert[motor];
						motor_dir(RWD,FREE);
					}
					break;
				case RIGHT:
					// Motordrehrichtung
					if(stellwert[motor]>0)
						motor_dir(FREE,FWD);
					if(stellwert[motor]<0){
						stellwert[motor] = -stellwert[motor];
						motor_dir(FREE,RWD);
					}
					break;
			}
			
			// Motorspeed setzen 
			motor_speed(stellwert[LEFT], stellwert[RIGHT]);
}

/**************************************************************************/
/*!
* \brief Ausgabe der Reglerwerte
*
* \param [in]
*		motor	Motor auswahl: LEFT, RIGHT
*
***************************************************************************/
void reglerwerte_ausgeben(uint8_t motor){
			// UART AUSGABE REGELUNG links
			uart_puts("\f *** DEBUG Regler **");		// Bildschirm löschen
			uart_puts("soll:\t");
			uart_puti(sollwert[motor]);		// sollwert ausgeben
			uart_puts("\n\r");					
			uart_puts("e:\t");
			uart_puti(fehler[motor]);			// Sollwertabweichung augeben
			uart_puts("\n\r");			
			uart_puts("int:\t");
			uart_puti(integrator[motor]);	// Integratorwert ausgeben
			uart_puts("\n\r");			
			uart_puts("dif:\t");
			uart_puti(differenzierer[motor]);	// Differenzierer ausgeben
			uart_puts("\n\r");
			uart_puts("stell:\t");
			uart_puti(stellwert[motor]);		// neuen stellwert ausgeben
			uart_puts("\n\r");
			uart_puts("\r\n");	
}

void encoder_ausgeben(void){
			// *************  Ablaufsteuerung **************

					uart_puts("\f ** DEBUG Encoder **");		// Bildschirm löschen
	
				uart_puts("Zeit       Flanken\tPWM\r\n");
					uart_printtime();			
					uart_puts("    ");
					uart_puti(istwert[LEFT]);
					uart_puts("    \t");
					uart_puti(stellwert[LEFT]*100/255L);
					uart_puts("%    \r\n");
					uart_puts("\t   ");
					uart_puti(istwert[RIGHT]);
					uart_puts("    \t");
					uart_puti(stellwert[RIGHT]*100/255L);
					uart_puts("%   \r\n");
}
					
