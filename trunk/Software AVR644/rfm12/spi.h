

#define DDR_SPI			DDRB
#define PORT_SPI		PORTB
#define PORT_SPI_CS	PORTB
#define DDR_SPI_CS	DDRB

#define SPI_CS			PB4
#define SPI_MOSI		PB5
#define SPI_MISO		PB6
#define SPI_SCLK		PB7


void spi_init(void);
uint8_t spi_putc( uint8_t data);
