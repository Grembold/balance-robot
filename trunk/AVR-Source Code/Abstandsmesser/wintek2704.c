#include <avr/io.h>
#include <avr/pgmspace.h>
#include "wintek2704.h"

#define PINOFPORT(x) (*(&x - 2))
#define DDROFPORT(x) (*(&x - 1))

#define LCD_FUNCTION_4BIT_2LINES 0x28   /* 4-bit interface, dual line,   5x7 dots */
#define LCD_BUSY              7      	/* DB7: LCD is busy                    */

#define lcd_e_delay()   __asm__ __volatile__( "rjmp 1f\n 1:" )
#define lcd_rw_high()   LCD_CTRL_PORT |= (1<<LCD_PIN_RW)
#define lcd_rw_low()    LCD_CTRL_PORT &= ~(1<<LCD_PIN_RW)
#define lcd_rs_high()   LCD_CTRL_PORT |= (1<<LCD_PIN_RS)
#define lcd_rs_low()    LCD_CTRL_PORT &= ~(1<<LCD_PIN_RS)
#define lcd_e_high()    LCD_CTRL_PORT |= (1<<active_e)
#define lcd_e_low()     LCD_CTRL_PORT &= ~(1<<active_e)

unsigned char active_e;



/*************************************************************************
 delay loop for small accurate delays: 16-bit counter, 4 cycles/loop
*************************************************************************/
static inline void _delayFourCycles(unsigned int __count)
{
    if ( __count == 0 )    
        __asm__ __volatile__( "rjmp 1f\n 1:" );    // 2 cycles
    else
        __asm__ __volatile__ (
    	    "1: sbiw %0,1" "\n\t"                  
    	    "brne 1b"                              // 4 cycles/loop
    	    : "=w" (__count)
    	    : "0" (__count)
    	   );
}


/************************************************************************* 
delay for a minimum of <us> microseconds
the number of loops is calculated at compile-time from MCU clock frequency
*************************************************************************/
#define delay(us)  _delayFourCycles( ( ( 1*(F_CPU/4000) )*us)/1000 )

static void toggle_e(void)
{
    lcd_e_high();
    lcd_e_delay();
    lcd_e_low();
}

static void lcd_write(uint8_t data,uint8_t rs) 
{
	if (rs)
	{   /* write data        (RS=1, RW=0) */
		lcd_rs_high();
	}
	else
	{    /* write instruction (RS=0, RW=0) */
		lcd_rs_low();
	}
  
	lcd_rw_low();

    
	/* configure data pins as output */
	DDROFPORT(LCD_DATA_PORT) |= 0x0F << LCD_DATA_OFFSET;

	/* output high nibble first */
	LCD_DATA_PORT = (LCD_DATA_PORT & (~(0x0F << LCD_DATA_OFFSET))) | ((data & 0xF0) >> (4-LCD_DATA_OFFSET));
	toggle_e();

	/* output low nibble */
	LCD_DATA_PORT = (LCD_DATA_PORT & (~(0x0F << LCD_DATA_OFFSET))) | ((data & 0x0F) << LCD_DATA_OFFSET);
	toggle_e();
  
	/* all data pins high (inactive) */
	LCD_DATA_PORT = (LCD_DATA_PORT & (~(0x0F << LCD_DATA_OFFSET))) | (0x0F << LCD_DATA_OFFSET);
}

static unsigned char lcd_read(unsigned char rs) 
{
    unsigned char data;
    
    if (rs)
      lcd_rs_high();                       /* RS=1: read data      */
		else
      lcd_rs_low();                        /* RS=0: read busy flag */
    
    lcd_rw_high();                           /* RW=1  read mode      */
    
    DDROFPORT(LCD_DATA_PORT)=DDROFPORT(LCD_DATA_PORT) & (~(0x0F << LCD_DATA_OFFSET)); /* configure data pins as input */
        
    lcd_e_high();
    lcd_e_delay();        
    data = (PINOFPORT(LCD_DATA_PORT) << (4-LCD_DATA_OFFSET)) & 0xF0;     /* read high nibble first */
    lcd_e_low();
       
    lcd_e_delay();                       /* Enable 500ns low       */
        
    lcd_e_high();
    lcd_e_delay();
    data |= (PINOFPORT(LCD_DATA_PORT) >> LCD_DATA_OFFSET) & 0x0F;    /* read low nibble        */
    lcd_e_low();
    
    return data;
}

static unsigned char lcd_waitbusy(void)
{
    register unsigned char c;
    
    /* wait until busy flag is cleared */
    while ( (c=lcd_read(0)) & (1<<LCD_BUSY)) {}
    
    /* the address counter is updated 4us after the busy flag is cleared */
    delay(2);

    /* now read the address counter */
    return (lcd_read(0));  // return address counter
}

static inline void lcd_newline(uint8_t pos)
{
    register uint8_t addressCounter;
	
	if (pos < 27)
        addressCounter = 64;
    else
	{
		addressCounter = 0;
		if (active_e == LCD_PIN_E1)
			active_e=LCD_PIN_E2;
		else
			active_e=LCD_PIN_E1;
	}
    lcd_command(LCD_SET_DDRAM + addressCounter);
}


void lcd_command(unsigned char cmd)
{
    lcd_waitbusy();
    lcd_write(cmd, 0);
}


void lcd_gotoxy(uint8_t x, uint8_t y)
{
	if (y > 1)
	{
		active_e=LCD_PIN_E1;
		y-=2;
	}
	else
		active_e=LCD_PIN_E2;
		
	lcd_command(LCD_SET_DDRAM + x + (y * 64));	
}


void lcd_putc(char c)
{
    uint8_t pos;


    pos = lcd_waitbusy();   // read busy-flag and address counter
    if (c=='\n')
    {
        lcd_newline(pos);
    }
    else
    {
#if LCD_WORD_WRAP==1
		if (pos == 27)
		{
			lcd_write(LCD_SET_DDRAM + 64, 0);
		}
		else
		{
			if (pos == (64 + 27))
			{
				if (active_e == LCD_PIN_E1)
				{
					active_e=LCD_PIN_E2;
				}
				else
				{
					active_e=LCD_PIN_E1;
				}
				lcd_write(LCD_SET_DDRAM + 0, 0);
			}
		}
		
        lcd_waitbusy();
#endif
        lcd_write(c, 1);
    }
}


void lcd_puts(const char *s)
{
  register char c;

  while ( (c = *s++) )
  {
    lcd_putc(c);
  }
}


void lcd_puts_P(const char *progmem_s)
{
    register char c;

    while ( (c = pgm_read_byte(progmem_s++)) ) {
        lcd_putc(c);
    }
}


void lcd_clrscr(void)
{
	active_e=LCD_PIN_E2;
	lcd_command(LCD_CLEAR);
	active_e=LCD_PIN_E1;
	lcd_command(LCD_CLEAR);
}


void init_controller(unsigned char cmd)
{
/* initial write to lcd is 8bit */
	LCD_DATA_PORT=(LCD_DATA_PORT & (~(0x0F << LCD_DATA_OFFSET))) | (0x03 << LCD_DATA_OFFSET);
	toggle_e();
	delay(4992);         /* delay, busy flag can't be checked here */

	/* repeat last command */ 
	toggle_e();      
	delay(64);           /* delay, busy flag can't be checked here */
    
	/* repeat last command a third time */
	toggle_e();      
	delay(64);           /* delay, busy flag can't be checked here */

	/* now configure for 4bit mode */
	LCD_DATA_PORT=(LCD_DATA_PORT & (~(0x0F << LCD_DATA_OFFSET))) | (0x02 << LCD_DATA_OFFSET); // LCD_FUNCTION_4BIT_1LINE>>4
	toggle_e();
	delay(64);           /* some displays need this additional delay */
	
	/* from now the LCD only accepts 4 bit I/O, we can use lcd_command() */    
	lcd_command(LCD_FUNCTION_4BIT_2LINES);
	lcd_command(LCD_DISP_OFF);              /* display off                  */
	lcd_command(LCD_CLEAR);                 /* display clear                */ 
	lcd_command(LCD_MODE_DEFAULT);          /* set entry mode               */
	lcd_command(cmd);		                /* display/cursor control       */
}

void lcd_init(unsigned char cmd)
{
	//set IO directions
	DDROFPORT(LCD_DATA_PORT) |= 0x0F << LCD_DATA_OFFSET;
	DDROFPORT(LCD_CTRL_PORT) |= 1<<LCD_PIN_RS;
	DDROFPORT(LCD_CTRL_PORT) |= 1<<LCD_PIN_RW;
	DDROFPORT(LCD_CTRL_PORT) |= 1<<LCD_PIN_E1;
	DDROFPORT(LCD_CTRL_PORT) |= 1<<LCD_PIN_E2;
	  
	  
	//initialize lcd in 4 bit mode
	delay(16000);
	
	active_e = LCD_PIN_E1;
	init_controller(cmd);
	active_e = LCD_PIN_E2;
	init_controller(cmd);
}
