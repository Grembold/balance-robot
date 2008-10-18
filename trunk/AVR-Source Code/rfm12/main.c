/****************************************************************************/
/*!
* \file		main.c
*
* \brief	
*
* \version  V001	16.04.2007	Benedikt K.\n
*           first build\n
*****************************************************************************/
#ifndef F_CPU
#error "F_CPU not defined"
#endif

#include <avr/io.h>
//#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include "spi.h"   
#include "../nbot_lib/uart.h" 




#define SENDER			0


#define CS_LOW	PORT_SPI_CS &= ~(1<<SPI_CS)
#define CS_HIGH PORT_SPI_CS |= (1<<SPI_CS)

#define CSC 0b10000000	// Configuration Setting Command (CSC) 	8008h
#define PMC 0b10000010	// Power Management Commane (PMC)				8208h
#define FSC	0b10100000	// Frequency Setting Command (FSC)			A680h
#define DRC 0b11000110	// Data Rate Command (DRC)							C623h
#define RCC	0b10010000	// Receiver Control Command (RCC)				9080h
#define DFC	0b11000010	// Data Filter Command (DFC)						C22Ch
#define RMC 0b11001010	// FIFO and Reset Mode Command (RMC)		CA80h
#define FRC	0b10110000	// Reseiver FIFO Read Command	(FRC)			B000h
#define AFC	0b11000100	// AFC Command (AFC)										C4F7h
#define TCC	0b10011000	// TX Configuration Control Command (TCC)		9800h
#define TXR	0b10111000	// Transmitter Register Write Command (TXR)	B8AAh
#define WUP	0b11100000	// Wake-UP Timer Comand (WUP)						E196h
#define	LDC	0b11001000	// Low Duty-Cycle Command (LDC)					C80Eh
												// Low Battery Detector and 
#define CDC	0b11000000	// MµC Clock Divider Command (CDC)			C000h
	
// Configuration Setting Command (CSC)
#define el	7
#define ef	6
#define b1	5
#define b0	4
#define x3	3
#define x2	2
#define x1	1
#define x0	0

// Power Management Commane (PMC)
#define er	7
#define ebb	6
#define et	5
#define es	4
#define ex	3
#define eb	2
#define ew	1
#define dc	0

// Data Rate Command (DRC)
#define cs	7

// Receiver Control Command (RCC)
#define p20	10
#define d1	9
#define d0	8
#define i2	7
#define i1	6
#define i0	5
#define g1	4
#define g0	3
#define r2	2
#define r1	1
#define r0	0

// Data Filter Command (DFC)
#define cr	7
#define ml	6
#define s		4
#define f2	2
#define f1	1
#define f0	0

// FIFO and Reset Mode Command (RMC)
#define al	2
#define ff	1
#define dr	0


/* Funktionen */
uint16_t rfm12_readStatus(void);
uint16_t rfm12_setReg(uint8_t reg, uint16_t val);
uint16_t	rfm12_wrt_cmd(uint16_t data);
void rfm12_ready(void);
void rfm12_txdata(char *buffer, unsigned char length);
void rfm12_rxdata(char *buffer, unsigned char length);

//*** main ***
int main(void)
{
	DDRB |=(1<<PB3);
	PORTB &=~(1<<PB3);	

	int i;
	
	uart_init();
	CS_HIGH;
	spi_init();


	uint16_t ret = 0;

	
	ret = rfm12_readStatus();
	
	for(i=15;i>=0;i--)
	{
		uart_puti( (ret>>i) & 0x0001);
	}
	uart_puts("\n\r");
	

	// Aktiviert FIFO und das interne daten register, setzt das Frequenzband auf 433MHz und 
	// die Externe Quarzkapazität auf 12pF
	rfm12_wrt_cmd(0x80D7);		// set el and ef																											x

	
	// setzt die Kanalfrequenz auf 434MHz
	rfm12_wrt_cmd(0xA620);		//0xA620 = 433,92MHz //0xA640 = 434Mhz  //0xA140 = 430.8MHz															x
	
	
	// Setzt die Baudrate auf 9600bps
	rfm12_wrt_cmd(0xC621);		// 0xC647 = 19.2kbps	// 0xc621 = 9600bps																												x
	
	// setzt die Funktion von Pin20(nInt/VDI) auf VDI (Valid data indicator)
	// VDI Signal ansprechzeit auf FAST
	// setzt Empfängerbandbreite auf 200 kHz
	// setzt die LNA verstärkung auf -6dBm
	// die Stärke des ankommenden Signals wird bei -79dBm gemeldet
	rfm12_wrt_cmd(0x948C);		// VDI, FAST, 200kHz, -6dBm, -79dBm																	x
	
	// Aktiviert Clock recovery
	// aktiviert internen Datenfilter 
	rfm12_wrt_cmd(0xC2AB);		// CR, !ml, DIG, DQD4																								x
	
	// Setzt den FIFO auf 8 Bit größe
	rfm12_wrt_cmd(0xCA81);		// FIFO8, SYNC, !ff, DR																							x
	
	// setzt die Ausgangsleistung auf Maximum
	// setzt die Frequenz modulationsparameter 150kHz
	rfm12_wrt_cmd(0x9830);		// !mp, 980=30kHz, MAX OUT																					x
	
	// Setzt die Clock Output Frequenc auf 10Mhz und 
	// setzt die schwell Spannung für den Low Battery Detector auf 5V															x
	rfm12_wrt_cmd(0xC0FC);


	// deaktiviert low duty cycle																																	x
	rfm12_wrt_cmd(0xC800);

	// Aktiviert die automatische Frequenz kontrolle
	// AFC settings: autotuning: -10kHz...+7,5kHz
	// aktiviert high accuracy Modus ohne VDI
	rfm12_wrt_cmd(0xC4F7);		// AFC, RL, !st, DI, OE, EN																					x	
	
	
	ret = rfm12_readStatus();
	
	for(i=15;i>=0;i--)
	{
		uart_puti( (ret>>i) & 0x0001);
	}
	uart_puts("\n\r");
	
	
	rfm12_wrt_cmd(0xB8AA);
	ret = rfm12_readStatus();
	
	for(i=15;i>=0;i--)
	{
		uart_puti( (ret>>i) & 0x0001);
	}
	uart_puts("\n\r");
	for(i=0;i<=200;i++)					// delay
	{
		asm("nop");
	}
	ret = rfm12_readStatus();
	
	for(i=15;i>=0;i--)
	{
		uart_puti( (ret>>i) & 0x0001);
	}
	uart_puts("\n\r");	
	rfm12_wrt_cmd(0xB8AA);
	rfm12_wrt_cmd(0xB8AA);
	
	
	/*------------------------------------------------------------*/
	rfm12_wrt_cmd(0x82C8);			// RX on
	rfm12_wrt_cmd(0xCA81);			// set FIFO mode
	rfm12_wrt_cmd(0xCA83);			// enable FIFO
	uint32_t k,j;
	char text[] = "";
	while(1)
	{
		PORTB ^=(1<<PB3);
	 
		ret = rfm12_readStatus();
		for(i=15;i>=0;i--)
		{
			uart_puti( (ret>>i) & 0x0001);
		}
		uart_puts("\n\r");	

		rfm12_rxdata(text,1);
		uart_puts(text);
		uart_puts("\n\r");
#ifdef SENDER
//		rfm12_txdata(text,7);
#endif
		
		/**
		 *for(k=0;k<=1000;k++)					// delay
		 *{
		 *	asm("nop");
		 *	for(j=0;j<=200;j++)					// delay
		 *	{
		 *		asm("nop");
		 *	}
		 *}	

		 */

 	}/*-----------------------------------------------------------*/
}

uint16_t rfm12_readStatus(void)
{
	uint8_t	retL = 0;
	uint8_t retH = 0;
	CS_LOW;
		retH = spi_putc(0x00);
		retL = spi_putc(0x00);
	CS_HIGH;
	
	return ((uint16_t) (retH<<8)) + retL;
}


uint16_t	rfm12_wrt_cmd(uint16_t data)
{
	uint8_t	retL = 0;
	uint8_t retH = 0;
	CS_LOW;
		retH = spi_putc( ((uint8_t)(data>>8)));
		retL = spi_putc((uint8_t) data);
	CS_HIGH;
	
	return ((uint16_t) (retH<<8)) + retL;
}


/****************************************************************************/
/*!
*	\brief 
*
*****************************************************************************/
void rfm12_ready(void)
{	
	uint16_t timeout = 60000;
	
	CS_LOW;
	asm("nop");
	while ((rfm12_readStatus() < 0x8000))// && timeout) // wait until FIFO ready
	{
		//timeout--;
		_delay_us(10);	
	}
	
	CS_HIGH;
}

/****************************************************************************/
/*!
*	\brief Sendet einen Block von Daten
*
*	\param[in]
*	buffer Zeigt auf das erste Byte des zu sendenden Blocks.
*
*	\param[in]
*	length Gibt die Länge des Blocks in Byte an.
*
*****************************************************************************/
void rfm12_txdata(char *buffer, uint8_t length)
{	
  unsigned char i;
	// ET: aktiviert power amplifierund und PLL, startet transmission wenn TX register aktiviert
	// ES: aktiviert den synthesizer
	// EX: aktiviert den crystal oscillator
	// EB: aktiviert Low battery detector
	rfm12_wrt_cmd(0x8238);		// !er, !ebb, ET, ES, EX, !eb, !ew, !DC	
	rfm12_ready();
	rfm12_wrt_cmd(0xB8AA);
	rfm12_ready();
	rfm12_wrt_cmd(0xB8AA);
	rfm12_ready();
	rfm12_wrt_cmd(0xB8AA);
	rfm12_ready();
	rfm12_wrt_cmd(0xB82D);
	rfm12_ready();
	rfm12_wrt_cmd(0xB8D4);
	for (i=0; i<length; i++)
	{		
		rfm12_ready();
		rfm12_wrt_cmd(0xB800|(*buffer++));
	}
	rfm12_ready();	
	rfm12_wrt_cmd(0xB800);			// dummy data
	rfm12_ready();
	rfm12_wrt_cmd(0xB800);			// dummy dat
	rfm12_ready();
	rfm12_wrt_cmd(0x8208);			// TX off
}


/****************************************************************************/
/*!
*	\brief Empfängt eine Block von Daten
*
* \param[out]
*	buffer Zeiger in den die Daten gespeichert werden
*
* \param[in]
* length Gibt die Länge des Buffers in Byte an
*
*****************************************************************************/
void rfm12_rxdata(char *buffer, unsigned char length)
{	
	// ET: aktiviert power amplifierund und PLL, startet transmission wenn TX register aktiviert
	// ES: aktiviert den synthesizer
	// EX: aktiviert den crystal oscillator
	// EB: aktiviert Low battery detector
	//rfm12_wrt_cmd(0x82D9);			//  ER, EBB, !et, ES, EX, !eb, !ew, DC
	unsigned char i;
	//rfm12_wrt_cmd(0x82C8);			// RX on
	//rfm12_wrt_cmd(0xCA81);			// set FIFO mode
	//rfm12_wrt_cmd(0xCA83);			// enable FIFO
	for (i=0; i<length; i++)
	{	
		rfm12_ready();
		*buffer++=rfm12_wrt_cmd(0xB000);
	}
	//rfm12_wrt_cmd(0x8208);			// RX off

}
