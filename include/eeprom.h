#include <stdint.h>

#ifndef EEPROM_H
#define EEPROM_H

void eeprom_write_byte(uint16_t mem_addr, uint8_t data);
void eeprom_write_bytes(uint16_t addr, uint8_t *data, uint8_t size);
uint8_t eeprom_read_byte(uint16_t mem_addr);

#endif