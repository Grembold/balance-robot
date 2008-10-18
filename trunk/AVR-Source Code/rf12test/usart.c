/*----------------------------------------------------------------------------
 Copyright:      Radig Ulrich  mailto: mail@ulrichradig.de
 Author:         Radig Ulrich
 Remarks:        
 known Problems: none
 Version:        14.06.2007
 Description:    RS232 Routinen
----------------------------------------------------------------------------*/

#include "usart.h"

volatile unsigned char buffercounter = 0;
char usart_rx_buffer[BUFFER_SIZE];

//----------------------------------------------------------------------------
//Init serielle Schnittstelle
void usart_init(void) 
{ 
  	//Enable TXEN im Register UCR TX-Data Enable
	UCR =(1 << TXEN);
	//Teiler wird gesetzt 
	UBRR=(SYSCLK / (BAUD_RATE * 16L) - 1);
}

//----------------------------------------------------------------------------
//Routine für die Serielle Ausgabe eines Zeichens
int usart_write_char(char c)
{
	if (c == '\n')
		usart_write_char('\r');
	//Warten solange bis Zeichen gesendet wurde
	while(!(USR & (1<<UDRE)));
	//Ausgabe des Zeichens
	UDR = c;
	return(0);
}
//------------------------------------------------------------------------------
void usart_write (char *Buffer,...)
{
	va_list ap;
	va_start (ap, Buffer);	
	
	int format_flag;
	char str_buffer[10];
	char str_null_buffer[10];
	char move = 0;
	char Base = 0;
	int tmp = 0;
		
	//Ausgabe der Zeichen
	while (*Buffer != 0)
		{
		if (*Buffer == '%')
			{
			*Buffer++;
			if (isdigit(*Buffer)>0)
				{
				str_null_buffer[0] = *Buffer++;
				str_null_buffer[1] = '\0';
				move = atoi(str_null_buffer);
				}
			switch (format_flag = *Buffer++)
				{
				case 'b':
					Base = 2;
					goto ConversionLoop;
				case 'c':
					//Int to char
					format_flag = va_arg(ap,int);
					usart_write_char(format_flag++); 
					break;
				case 'i':
					Base = 10;
					goto ConversionLoop;
				case 'o':
					Base = 8;
					goto ConversionLoop;
				case 'x':
					Base = 16;
					//****************************
					ConversionLoop:
					//****************************
					itoa(va_arg(ap,int),str_buffer,Base);
					int b=0;
					while (str_buffer[b++] != 0){};
					b--;
					if (b<move)
						{
						move -=b;
						for (tmp = 0;tmp<move;tmp++)
							{
							str_null_buffer[tmp] = '0';
							}
						//tmp ++;
						str_null_buffer[tmp] = '\0';
						strcat(str_null_buffer,str_buffer);
						strcpy(str_buffer,str_null_buffer);
						}
					usart_write_str(str_buffer);
					move =0;
					break;
				}
			
			}	
		else
			{
			usart_write_char (*Buffer++);
			}
		}
	va_end(ap);
}

//----------------------------------------------------------------------------
//Ausgabe eines Strings
void usart_write_str(char *str)
{
	while (*str)
	{
		usart_write_char(*str++);
	}
}

//----------------------------------------------------------------------------
//Empfang eines Zeichens
SIGNAL (SIG_USART_RECV)
{
	unsigned char receive_char;
	receive_char = (UDR);

	if (usart_status.usart_ready)
	{
		usart_status.usart_rx_ovl = 1;
		return; 
	}

	if (receive_char == '\r')
	{
		usart_rx_buffer[buffercounter] = 0;
		buffercounter = 0;
		usart_status.usart_ready = 1;
		return;    
	}

	if (buffercounter < BUFFER_SIZE - 1)
	{
		usart_rx_buffer[buffercounter++] = receive_char;    
	}
return;
}




