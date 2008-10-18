

#ifndef _COMMAND_H
#define _COMMAND_H

#define CMD_NOP 				00			/*!< \brief normales Zeichen empfangen */
#define CMD_NOC 				01  		/*!< \brief Kein gültiger Befehl */
#define CMD_CLS					02			/*!< \brief lösche Bildschirm */
#define CMD_STATUS 			03			/*!< \brief Systemstatus */
#define CMD_HELP				04			/*!< \brief Hiflemeldung */
#define CMD_MOVE 				05			/*!< \brief Fahren */
#define CMD_DEMO				06			/*!< \brief Demoprogramm abspielen */
#define CMD_DEBUG				10			/*!< \brief DEBUG informationen */
#define CMD_DB_CMD 			11			/*!< \brief DEBUG Commandline */
#define CMD_DB_ENC 			12			/*!< \brief DEBUG Encoder */
#define CMD_DB_REG 			13			/*!< \brief DEBUG Regler */
#define CMD_ESCAPE 			27			/*!< \brief Escape */
#define CMD_UP 					65			/*!< \brief Pfeil hoch */
#define CMD_DOWN 				66			/*!< \brief Pfeil runter */
#define CMD_LEFT 				68			/*!< \brief Pfeil links */
#define CMD_RIGHT 			69			/*!< \brief Pfeil rechts */


#define MAX_BUF 100	// maximal 255 zeichen speichern


extern void command_put(char last_c);
extern uint8_t command_get(void);
extern char *command_text(void);
extern void command_prompt(char command);

#endif /* _COMMAND_H */
