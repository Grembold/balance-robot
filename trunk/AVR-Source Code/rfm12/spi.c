#include <avr/io.h>
#include "spi.h"



void spi_init()
{
	// Aktivieren der Pins für das SPI Interface
	DDR_SPI		|= (1<<SPI_SCLK)|(1<<SPI_MOSI);
	PORT_SPI 	&= ~((1<<SPI_SCLK)|(1<<SPI_MOSI)|(1<<SPI_MISO));
	
	DDR_SPI_CS	|= (1<<SPI_CS);
	PORT_SPI_CS &= ~(1<<SPI_CS);
	
	// Aktivieren des SPI Master Interfases, fosc = fclk / 2;
	SPCR = (1<<SPE)|(1<<MSTR);
	SPSR = (1<<SPI2X);
	
}

uint8_t spi_putc( uint8_t data)
{
	// Sendet ein Byte
	SPDR = data;
	
	// Wartet bis Byte gesendet wurde
	while( !( SPSR & (1<<SPIF)))
		;
	
	return SPDR;
}

