/****************************************************************************/
/*!
* \file lcd.h
*
* \brief Definitionen der Pinbelegung des Displays\n
*
*	Library for LC Display WINTEK WD-C2704M-1HNN
*				27 x 4 Characters 
*			www.pollin.de (Nr. 120 232)
*	
*	based on LCD implementation from Peter Fleury
*	
*	2005 Stefan Seegel dahamm@gmx.net
*	
*	Thanks to Peter Fleury for hd44780 lib !
****************************************************************************/

#ifndef F_CPU 11059000
#define F_CPU 11059000
#endif

// Hardware pin definitions

#define LCD_DATA_PORT		PORTC	/*!< Port on which the 4 LCD datapins are connected to */
#define LCD_DATA_OFFSET		4		/*!< Offset for data pins (e.g. 0 = PIN0..PIN3, 2 = PIN2..PIN5) */
#define LCD_CTRL_PORT		PORTC	/*!< PORT on which E1, E2, RS and RW are connected to */
#define LCD_PIN_RW			1		/*!< PIN an Read/Write */
#define LCD_PIN_RS			0		/*!< PIN an RESET */
#define LCD_PIN_E1			3		/*!< PIN an Enable1 */
#define LCD_PIN_E2			2		/*!< PIN an Enable2 */

#define LCD_WORD_WRAP 		1		/*!< Zeilenumbruch */


/*-------------------------------------------------------------------------------------
	LCD Commands, use with lcd_command()
-------------------------------------------------------------------------------------*/
#define LCD_CLEAR				0x01	/*!< Clear Display */
#define LCD_HOME				0x02	/*!< Cursor Home */

// set entry mode: display shift on/off, dec/inc cursor move direction 
#define LCD_ENTRY_CRSR_LEFT		0x04	/*!< move cursor to left after every char written */
#define LCD_ENTRY_DSPL_LEFT		0x05	/*!< move display to left after every char written */
#define LCD_ENTRY_CRSR_RIGHT	0x06	/*!< move cursor to right after every char written */
#define LCD_ENTRY_DSPL_RIGHT	0x07	/*!< move display to right after every char written */

// display on/off, cursor on/off, blinking char at cursor position
#define LCD_DISP_OFF			0x08	/*!< display off */
#define LCD_DISP_ON				0x0C	/*!< display on, cursor off */
#define LCD_DISP_ON_CRSR		0x0E	/*!< display on, cursor on */
#define LCD_DISP_ON_BLINK		0x0F	/*!< display on, cursor on, blinking cursor */

// move cursor/shift display
#define LCD_MOVE_CRSR_LEFT		0x10	/*!< move cursor left */
#define LCD_MOVE_CRSR_RIGHT		0x14	/*!< move cursor right */
#define LCD_MOVE_DSPL_LEFT		0x18	/*!< shift display left */
#define LCD_MOVE_DSPL_RIGHT		0x1C	/*!< shift display right */

// set RAM addresses
#define LCD_SET_CGRAM			0x40	/*!< set CG-Ram address, add address to this value */
#define LCD_SET_DDRAM			0x80	/*!< set DD-Ram address, add address to this value */

#define LCD_MODE_DEFAULT		LCD_ENTRY_CRSR_RIGHT


extern void lcd_init(unsigned char cmd);
extern void lcd_command(unsigned char cmd);
extern void lcd_puts(const char *s);
extern void lcd_puts_P(const char *progmem_s);
extern void lcd_putc(char c);
extern void lcd_clrscr(void);
extern void lcd_gotoxy(unsigned char x, unsigned char y);
