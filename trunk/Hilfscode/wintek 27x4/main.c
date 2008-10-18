#include <avr/io.h>
#include "wintek2704.h"

/*
	Test programm for wintek display
	2005 Stefan Seegel dahamm@gmx.net
*/

int main(void)
{
	lcd_init(LCD_DISP_ON);
	lcd_puts("Guten Tag\n2. Zeile\n3. Zeile");
	lcd_gotoxy(3, 6);
	lcd_puts("4. Zeile Mitte");
	
	for (;;)
	{
	
	}
	
	return 0;
}
