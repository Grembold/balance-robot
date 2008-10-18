/*##############################################################################
 * This program is free software; you can redistribute it 
 * and/or modify it under the terms of the GNU General 
 * Public License as published by the Free Software 
 * Foundation; either version 2 of the License, or (at your 
 * option) any later version.  
 * This program is distributed in the hope that it will be  
 * useful, but WITHOUT ANY WARRANTY; without even the implied 
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  
 * PURPOSE. See the GNU General Public License for more details. 
 * You should have received a copy of the GNU General Public  
 * License along with this program; if not, write to the Free  
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 * MA 02111-1307, USA. 
 *   
 * @file        rf12.c 
 * @brief        
 * @author      Benedikt K.
 * @author      Juergen Eckert
 * @author  	Ulrich Radig (mail@ulrichradig.de) www.ulrichradig.de 
 * @date        04.06.2007  
##############################################################################*/

#include "rf12.h"



#ifdef RF12_INTERRUPT
struct RF12_stati
{
	unsigned char Rx:1;
	unsigned char Tx:1;
	unsigned char New:1;
};

	struct RF12_stati RF12_status;
	volatile unsigned char RF12_Index = 0;
	unsigned char RF12_Data[RF12_DataLength+10];	// +10 == paket overhead
#endif

//##############################################################################
//
#ifdef RF12_INTERRUPT
SIGNAL(SIG_INTERRUPT0)
//##############################################################################
{
	if(RF12_status.Rx)
	{
		if(RF12_Index < RF12_DataLength)
			RF12_Data[RF12_Index++] = rf12_trans(0xB000) & 0x00FF;
		else
		{
			rf12_trans(0x8208);
			RF12_status.Rx = 0;
		}
		if(RF12_Index >= RF12_Data[0] + 3)		//EOT
		{
			rf12_trans(0x8208);
			RF12_status.Rx = 0;
			RF12_status.New = 1;
			//GICR &= ~(1<<INT0);		//disable int0
		}
	}
	else if(RF12_status.Tx)
	{
		rf12_trans(0xB800 | RF12_Data[RF12_Index]);
		if(!RF12_Index)
		{
			RF12_status.Tx = 0;
			rf12_trans(0x8208);		// TX off
		}
		else
		{
			RF12_Index--;
		}
	}
	else
	{
		rf12_trans(0x0000);			//dummy read
							//TODO: what happend
	}
}
#endif

//##############################################################################
//
unsigned int crcUpdate(unsigned int crc, unsigned char serialData)
//##############################################################################
{
        unsigned int tmp;
        unsigned char j;

	tmp = serialData << 8;
        for (j=0; j<8; j++)
	{
                if((crc^tmp) & 0x8000)
			crc = (crc<<1) ^ 0x1021;
                else
			crc = crc << 1;
                tmp = tmp << 1;
        }
	return crc;
}

//##############################################################################
//
unsigned short rf12_trans(unsigned short wert)
//##############################################################################
{	
	unsigned short werti = 0;
	
	RF_PORT &=~(1<<CS);
	
#ifdef SPI_MODE	//Routine für Hardware SPI
	SPDR = (0xFF00 & wert)>>8;
	while(!(SPSR & (1<<SPIF))){};
	werti = (SPDR<<8);
	
	SPDR = (0x00ff & wert);
	while(!(SPSR & (1<<SPIF))){};
	werti = werti + SPDR;

#else			//Routine für Software SPI
	for (unsigned char i=0; i<16; i++)
	{	
		if (wert&32768)
			RF_PORT |=(1<<SDI);
		else
			RF_PORT &=~(1<<SDI);
		werti<<=1;
		if (RF_PIN&(1<<SDO))
			werti|=1;
		RF_PORT |=(1<<SCK);
		wert<<=1;
		asm("nop");
		RF_PORT &=~(1<<SCK);
	}
#endif
	RF_PORT |=(1<<CS);
	return werti;
}

//##############################################################################
//
void rf12_init(void)
//##############################################################################
{
	unsigned char i;

	RF_DDR |= (1<<SDI)|(1<<SCK)|(1<<CS);
	RF_DDR &= ~(1<<SDO);
	RF_PORT |= (1<<CS);

#ifdef SPI_MODE
	//Aktiviren des SPI - Bus, Clock = Idel LOW
	//SPI Clock teilen durch 128, Enable SPI, SPI in Master Mode
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPR1);
	SPSR &= ~(0<<SPI2X);
#endif

	for (i=0; i<10; i++)
		_delay_ms(10);			// wait until POR done

	rf12_trans(0xC0E0);			// AVR CLK: 10MHz
	rf12_trans(0x80D7);			// Enable FIFO
	rf12_trans(0xC2AB);			// Data Filter: internal
	rf12_trans(0xCA81);			// Set FIFO mode
	rf12_trans(0xE000);			// disable wakeuptimer
	rf12_trans(0xC800);			// disable low duty cycle
	rf12_trans(0xC4F7);			// AFC settings: autotuning: -10kHz...+7,5kHz
	rf12_trans(0x0000);
		
#ifdef RF12_INTERRUPT
	RF12_status.Rx = 0;
	RF12_status.Tx = 0;
	RF12_status.New = 0;

	RF_IRQDDR &= ~(1<<IRQ);
	GICR |= (1<<INT0);
#endif
}

//##############################################################################
//
void rf12_setbandwidth(unsigned char bandwidth, unsigned char gain, 
						unsigned char drssi)
//##############################################################################
{
	rf12_trans(0x9400|((bandwidth&7)<<5)|((gain&3)<<3)|(drssi&7));
}

//##############################################################################
//
void rf12_setfreq(unsigned short freq)
//##############################################################################
{	
	if (freq<96)					// 430,2400MHz
		freq=96;
	else if (freq>3903)			// 439,7575MHz
		freq=3903;
	rf12_trans(0xA000|freq);
}

//##############################################################################
//
void rf12_setbaud(unsigned short baud)
//##############################################################################
{
	if (baud<663)
		return;
	if (baud<5400)					// Baudrate= 344827,58621/(R+1)/(1+CS*7)
		rf12_trans(0xC680|((43104/baud)-1));
	else
		rf12_trans(0xC600|((344828UL/baud)-1));
}

//##############################################################################
//
void rf12_setpower(unsigned char power, unsigned char mod)
//##############################################################################
{	
	rf12_trans(0x9800|(power&7)|((mod&15)<<4));
}

//##############################################################################
//
#ifdef RF12_INTERRUPT
unsigned char rf12_rxstart(void)
//##############################################################################
{
	if(RF12_status.New)
		return(1);			//buffer not yet empty
	if(RF12_status.Tx)
		return(2);			//tx in action
	if(RF12_status.Rx)
		return(3);			//rx already in action

	rf12_trans(0x82C8);			// RX on
	rf12_trans(0xCA81);			// set FIFO mode
	rf12_trans(0xCA83);			// enable FIFO

	RF12_Index = 0;
	RF12_status.Rx = 1;
	
	return(0);				//all went fine
}

//##############################################################################
//
unsigned char rf12_rxfinish(unsigned char *data)
//##############################################################################
{
	unsigned int crc, crc_chk = 0;
	unsigned char i;
	if(RF12_status.Rx)
		return(255);				//not finished yet
	if(!RF12_status.New)
		return(254);				//old buffer
	for(i=0; i<RF12_Data[0] +1 ; i++)
		crc_chk = crcUpdate(crc_chk, RF12_Data[i]);

	crc = RF12_Data[i++];
	crc |= RF12_Data[i] << 8;
	RF12_status.New = 0;
	if(crc != crc_chk)
		return(0);				//crc err -or- strsize
	else
	{
		unsigned char i;
		for(i=0; i<RF12_Data[0]; i++)
			data[i] = RF12_Data[i+1];
		return(RF12_Data[0]);			//strsize
	}
}

//##############################################################################
//
unsigned char rf12_txstart(unsigned char *data, unsigned char size)
//##############################################################################
{
	unsigned char i, l;
	unsigned int crc;
	if(RF12_status.Tx)
		return(2);			//tx in action
	if(RF12_status.Rx)
		return(3);			//rx already in action
	if(size > RF12_DataLength)
		return(4);			//str to big to transmit

	RF12_status.Tx = 1;
	RF12_Index = size + 9;			//act -10 

	i = RF12_Index;				
	RF12_Data[i--] = 0xAA;
	RF12_Data[i--] = 0xAA;
	RF12_Data[i--] = 0xAA;
	RF12_Data[i--] = 0x2D;
	RF12_Data[i--] = 0xD4;
	RF12_Data[i--] = size;
	crc = crcUpdate(0, size);
	for(l=0; l<size; l++)
	{
		RF12_Data[i--] = data[l];
		crc = crcUpdate(crc, data[l]);
	}	
	RF12_Data[i--] = (crc & 0x00FF);
	RF12_Data[i--] = (crc >> 8);
	RF12_Data[i--] = 0xAA;
	RF12_Data[i--] = 0xAA;

	rf12_trans(0x8238);			// TX on

	return(0);				//all went fine
}

//##############################################################################
//
unsigned char rf12_txfinished(void)
//##############################################################################
{
	if(RF12_status.Tx)
		return(255);			//not yet finished
	return(0);
}

//##############################################################################
//
void rf12_allstop(void)
//##############################################################################
{
	//GICR &= ~(1<<INT0);		//disable int0	
	RF12_status.Rx = 0;
	RF12_status.Tx = 0;
	RF12_status.New = 0;
	rf12_trans(0x8208);		//shutdown all
	rf12_trans(0x0000);		//dummy read
}

//##############################################################################
//
#else

void rf12_ready(void)
//##############################################################################
{
	unsigned long timeout = 0;
	RF_PORT &=~(1<<CS);
	asm("nop");
	asm("nop");
	asm("nop");
	//wait until FIFO ready or timeout
	while (!(RF_PIN & (1<<SDO)))
	{
		if ((timeout++)>100000){break;};
	};
	RF_PORT |=(1<<CS);
}

//##############################################################################
//
void rf12_txdata(unsigned char *data, unsigned char number)
//##############################################################################
{	
	unsigned char i;
	unsigned int crc;
	rf12_trans(0x8238);			// TX on

	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();
	rf12_trans(0xB82D);
	rf12_ready();
	rf12_trans(0xB8D4);
	rf12_ready();
	rf12_trans(0xB800 | number);	
	crc = crcUpdate(0, number);
	for (i=0; i<number; i++)
	{	
		rf12_ready();
		rf12_trans(0xB800 | data[i]);
		crc = crcUpdate(crc, data[i]);
	}
	rf12_ready();
	rf12_trans(0xB800 | (crc & 0x00FF));
	rf12_ready();
	rf12_trans(0xB800 | (crc >> 8));
	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();

	rf12_trans(0x8208);			// TX off
}

//##############################################################################
//
unsigned char rf12_rxdata(unsigned char *data)
//##############################################################################
{	
	unsigned char i, number;
	unsigned int crc, crc_chk;
	rf12_trans(0x82C8);			// RX on
	rf12_trans(0xCA81);			// set FIFO mode
	rf12_trans(0xCA83);			// enable FIFO
	
	rf12_ready();
	number = rf12_trans(0xB000) & 0x00FF;
	if (number > 200) number = 200;
	crc_chk = crcUpdate(0, number);

	for (i=0; i<number; i++)
	{
		rf12_ready();
		data[i] = (unsigned char) (rf12_trans(0xB000) & 0x00FF);
		crc_chk = crcUpdate(crc_chk, data[i]);
	}
	data[number+1] = 0;
	rf12_ready();
	crc = rf12_trans(0xB000) & 0x00FF;
	rf12_ready();
	crc |=  rf12_trans(0xB000) << 8;

	rf12_trans(0x8208);			// RX off

	if (crc != crc_chk)
		number = 0;

	return number;
}

#endif
