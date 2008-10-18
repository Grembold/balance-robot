#include <string.h>
#include <stdint.h>		//standart Integertypen
#include <avr/pgmspace.h>
#include "command.h"
#include "uart.h"


uint8_t text[MAX_BUF];	// Zeichenkette	(String) von Uart
uint8_t *ptext = text;	// Hilfspointer auf Zeichenkette
uint8_t nText;					// Anzahl der bisherigen Zeichen im string 
uint8_t command = CMD_NOP;				// Letztes Komando


// Häufige Texte im Flash
const char Str_CR[] PROGMEM = "\r\n"; 
const char Str_BOT[] PROGMEM = "BOT:\\>";

void command_put(uint8_t last_c)
{
	if(last_c != (uint8_t)(-1)){				// Zeichen empfangen
		if(last_c == 13){								// ENTER wurde gedrückt
			*ptext++ = '\0';								// String mit NULL terminieren
			ptext = text;										// Pointer wieder auf Abfang setzen
			nText=0;												// 
			if((strcmp(text, "?")==0) || 
				(strcmp(text, "help")==0)){								// ? oder help
				command = CMD_HELP;
				return;
			} else if(strcmp(text, "status")==0){				// status
				command = CMD_STATUS;
				return;
			} else if(strcmp(text, "cls")==0){					// cls
				command = CMD_CLS;
				return;
			} else if(strcmp(text, "move")==0){					// move
				command = CMD_MOVE;
				return;
			} else if(strcmp(text, "demo")==0){					// demo
				command = CMD_DEMO;
				return;
			} else if(strcmp(text, "debug")==0){				// debug
				command = CMD_DEBUG;
				return;
			} else if(strcmp(text, "debug cmd")==0){	// debug encoder
				command = CMD_DB_CMD;
				return;
			} else if(strcmp(text, "debug enc")==0){	// debug commandline
				command = CMD_DB_ENC;
				return;			
			} else if(strcmp(text, "debug reg")==0){		// debug regler
				command = CMD_DB_REG;
				return;
			} else {																			// No Command
				command = CMD_NOC;
				return;
			}
		}	else if(last_c ==27){
			uart_getc(&last_c);
			if(last_c=='['){
				 uart_getc(&last_c);
				if(last_c == 'A'){												// Pfeil hoch
					command = CMD_UP;
					return;
				}	 else if(last_c == 'B'){								// Pfeil runter
					command = CMD_DOWN;
					return;
				} else if(last_c == 'C'){								// Pfeil rechts
					command = CMD_RIGHT;
					return;
				} else if(last_c == 'D'){								// Pfeil links
					command = CMD_LEFT;
					return;
				}
			} else {																		// Escape
				command = CMD_ESCAPE;
				return;
			}
		} else {
				if((last_c == 10) ||(last_c == 8) || (nText == (MAX_BUF-4)) ){	// Zeichen ignorieren
					command = CMD_NOP;
					return;
				}
				*ptext++ = last_c;						// Zeichen Speichern
				uart_putc(last_c);						// ausgeben
				nText++;			
				command = CMD_NOP;
				return;
		}
	} 
	command = CMD_NOP;
	return;
}


uint8_t command_get(void)
{
	return command;
}

uint8_t *command_text(void)
{
	return text;
}

void command_prompt(uint8_t command)
{
		switch (command) {
			case CMD_NOP: break;
			case CMD_NOC: 
				uart_puts_p(PSTR("\r\n Diesen Befehl kenne ich nicht.\r\n"));
				uart_puts_p(PSTR(" Du hast \""));
				uart_puts(command_text());
				uart_puts_p(PSTR("\" eingegeben. "));
				uart_puti((uint8_t)*command_text());
				uart_puts_p(Str_CR);
				uart_puts_p(Str_BOT);	
				break;
			case CMD_HELP: 
				uart_puts_p(PSTR("\f *** Hallo! ***"));
				uart_puts_p(Str_CR);
				uart_puts_p(PSTR(" Ich bin ein kleiner Roboter. Was moechtest du mit mir machen?"));
				uart_puts_p(Str_CR);
				uart_puts_p(PSTR(" Mit dem Befehl:"));
				uart_puts_p(Str_CR);
				uart_puts_p(PSTR(" - ?       Zeige ich die alle Befehle die ich verstehe."));
				uart_puts_p(Str_CR);
				uart_puts_p(PSTR(" - status  kannst du sehen, wie ich mich fuehle."));
				uart_puts_p(Str_CR);
				uart_puts_p(PSTR(" - cls     loescht du den Bildschirm."));
				uart_puts_p(Str_CR);
				uart_puts_p(PSTR(" - move    kannst du mich fahren lassen."));
				uart_puts_p(Str_CR);
				uart_puts_p(PSTR(" - demo    fahre ich ein wenig hin und her."));
				uart_puts_p(Str_CR);
				uart_puts_p(PSTR(" - debug   Zeige ich dir Aktuelle Statusmeldungen."));
				uart_puts_p(Str_CR);
				uart_puts_p(Str_BOT);	
				break;
			case CMD_STATUS:
				uart_puts_p(PSTR("\f *** Hi! ***\r\n"));
				uart_puts_p(PSTR(" Ich bin heute schon seit "));
				uart_printtime();
				uart_puts_p(PSTR(" unterwegs.\r\n"));
				uart_puts_p(PSTR(" Ich fuehle mich gut und meine Akkuspannung betraegt noch:"));
				uart_puts_p(PSTR(" 12V\r\n"));
				uart_puts_p(Str_BOT);	
				break;
			case CMD_MOVE: 
				uart_puts_p(PSTR("\f *** Gib Gas! ***\r\n"));
				uart_puts_p(PSTR(" Mit den Pfeiltasten kannst du mich nun steuern.\r\n"));
				uart_puts_p(PSTR(" Mit Esc beendest du die Fahrt."));	
				break;
			case CMD_DEMO: 
				uart_puts_p(PSTR("\f *** DEMO *** \r\n"));
				uart_puts_p(PSTR(" Ich fahre ein wenig hin und her.\r\n"));
				uart_puts_p(PSTR(" Mit Esc beendest du die Fahrt."));	
				break;					
			case CMD_DEBUG: 
				uart_puts_p(PSTR("\f *** DEBUG *** \r\n"));
				uart_puts_p(PSTR(" Ich kann dir verschiedene DEBUGinfomationen ausgeben:\r\n"));
				uart_puts_p(PSTR(" Mit dem Befehl:\r\n"));
				uart_puts_p(PSTR(" - debug cmd      gebe ich dir alle gesendete Zeichen aus.\r\n"));
				uart_puts_p(PSTR(" - debug enc      gebe ich die die aktuellen Encoderwerte aus.\r\n"));
				uart_puts_p(PSTR(" - debug reg      gebe ich dir aktuelle Reglerwerte aus.\r\n"));
				uart_puts_p(PSTR(" - Die Anzeige beendest du mit escape.\r\n"));
				uart_puts_p(Str_BOT);	break;
			case CMD_CLS: 
				uart_puts_p(PSTR("\f"));
				uart_puts_p(Str_BOT);		
				break;
			case CMD_ESCAPE:
				uart_puts_p(Str_CR);	
				uart_puts_p(Str_BOT);	 
				break;
			case CMD_UP: 	 
				break;
			case CMD_DOWN: 
				break;
			case CMD_LEFT: break;
			case CMD_RIGHT: break;
			case CMD_DB_CMD: break;
			case CMD_DB_ENC: break;
			case CMD_DB_REG: break;
			default: uart_puts_p(PSTR("\f Fehler in der Commandline!!\r\n"));
		}
	}
