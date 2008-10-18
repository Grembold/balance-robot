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
 * @file        rf12.h 
 * @brief        
 * @author      Benedikt K.
 * @author      Juergen Eckert
 * @author  	Ulrich Radig (mail@ulrichradig.de) www.ulrichradig.de 
 * @date        04.06.2007  
##############################################################################*/

#ifndef __RFM12_H
	#define __RFM12_H

#include <avr/io.h>
#include <avr/interrupt.h>

//#define F_CPU 800000UL
#include <util/delay.h>

//##############################################################################
/* config */
#define SPI_MODE
//#define RF12_INTERRUPT

#ifdef RF12_INTERRUPT
	#define RF_IRQDDR	DDRD
	#define RF_IRQPIN	PIND
	//only used if async mode. Had to be INT0
	//or INT1/2 when change something in rf12.c
	#define IRQ		2		
	//RF12 Buffer length (max length 244)
	#define RF12_DataLength	100		
#endif

#if defined (__AVR_ATmega88__)
	#define RF_PORT	PORTB
	#define RF_DDR	DDRB
	#define RF_PIN	PINB
	#define SDI		3
	#define SCK		5
	#define CS		2
	#define SDO		4
#endif

#if defined (__AVR_ATmega8__)
	#define RF_PORT	PORTB
	#define RF_DDR	DDRB
	#define RF_PIN	PINB
	#define SDI		3
	#define SCK		5
	#define CS		2
	#define SDO		4
#endif

#if defined (__AVR_ATmega32__)
	#define RF_PORT	PORTB
	#define RF_DDR	DDRB
	#define RF_PIN	PINB
	#define SDI		5
	#define SCK		7
	#define CS		4
	#define SDO		6
#endif

#if defined (__AVR_ATmega16__)
	#define RF_PORT	PORTB
	#define RF_DDR	DDRB
	#define RF_PIN	PINB
	#define SDI		5
	#define SCK		7
	#define CS		4
	#define SDO		6
#endif

//##############################################################################

#define RxBW400		1
#define RxBW340		2
#define RxBW270		3
#define RxBW200		4
#define RxBW134		5
#define RxBW67		6

#define TxBW15		0
#define TxBW30		1
#define TxBW45		2
#define TxBW60		3
#define TxBW75		4
#define TxBW90		5
#define TxBW105		6
#define TxBW120		7

#define LNA_0		0
#define LNA_6		1
#define LNA_14		2
#define LNA_20		3

#define RSSI_103	0
#define RSSI_97		1
#define RSSI_91		2
#define RSSI_85		3
#define RSSI_79		4
#define RSSI_73		5
#define RSSI_67		6
#define	RSSI_61		7

#define PWRdB_0		0
#define PWRdB_3		1
#define PWRdB_6		2
#define PWRdB_9		3
#define PWRdB_12	4
#define PWRdB_15	5
#define PWRdB_18	6
#define PWRdB_21	7

//##############################################################################

#define RF12TxBDW(kfrq)	((unsigned char)(kfrq/15)-1)
// macro for calculating frequency value out of frequency in MHz
#define RF12FREQ(freq)	((freq-430.0)/0.0025)	



// initialize module
void rf12_init(void);

// transfer 1 word to/from module
unsigned short rf12_trans(unsigned short wert);

// set center frequency
void rf12_setfreq(unsigned short freq);

// set baudrate
void rf12_setbaud(unsigned short baud);

// set transmission settings
void rf12_setpower(unsigned char power, unsigned char mod);

// set receiver settings
void rf12_setbandwidth(unsigned char bandwidth, unsigned char gain, 
						unsigned char drssi);


//##############################################################################
#ifdef RF12_INTERRUPT
//##############################################################################
// start receiving a package
unsigned char rf12_rxstart(void);

// readout the package, if one arrived
unsigned char rf12_rxfinish(unsigned char *data);

// start transmitting a package of size size
unsigned char rf12_txstart(unsigned char *data, unsigned char size);

// check whether the package is already transmitted
unsigned char rf12_txfinished(void);

// stop all Rx and Tx operations
void rf12_allstop(void);

//##############################################################################
#else

// transmit number of bytes from array
void rf12_txdata(unsigned char *data, unsigned char number);

// receive number of bytes into array
unsigned char rf12_rxdata(unsigned char *data);

// wait until FIFO ready (to transmit/read data)
void rf12_ready(void);

#endif //RF12_INTERRUPT

#endif //__RFM12_H
