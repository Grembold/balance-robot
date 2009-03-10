#ifndef _24C64P_H
#define _24C64P_H 1

#define Dev24C64  0xA0      // device address of EEPROM 24C64, see datasheet

#define USE_UART 1			// zum Debuggen wird über Uart meldungen ausgegeben


#define __need_size_t
extern uint8_t eeprom_init(void);

extern inline uint8_t eeprom_read_byte (const uint16_t addr);
extern inline uint16_t eeprom_read_word (const uint16_t addr);
extern inline void eeprom_read_block (void *pointer_ram, const void *pointer_eeprom, uint16_t size);

extern inline void eeprom_write_byte (uint16_t addr,uint8_t value);
extern inline void eeprom_write_word (uint16_t *addr,uint16_t value);
extern inline void eeprom_write_block (const void *pointer_ram, void *pointer_eeprom, uint16_t size);

#endif
