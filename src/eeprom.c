#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "constants.h"
#include "eeprom.h"

void eeprom_write_byte(uint16_t mem_addr, uint8_t data)
{
    uint8_t buff[3];

    buff[0] = mem_addr >> 8;
    buff[1] = mem_addr & 0xFF;
    
    buff[2] = data;

    i2c_write_blocking(EEPROM_PORT, EEPROM_ADDR, buff, 3, false);
    sleep_ms(5);
}

void eeprom_write_bytes(uint16_t addr, uint8_t *data, uint8_t size)
{
    uint8_t buff[2 + size];
    buff[0] = addr >> 8;
    buff[1] = addr & 0xFF;
    memcpy(&buff[2], data, size);

    i2c_write_blocking(EEPROM_PORT, EEPROM_ADDR, buff, 2 + size, false);
    sleep_ms(5);
}

uint8_t eeprom_read_byte(uint16_t mem_addr)
{
    uint8_t buff[2];

    buff[0] = mem_addr >> 8;
    buff[1] = mem_addr & 0xFF;

    i2c_write_blocking(EEPROM_PORT, EEPROM_ADDR, buff, 2, true);

    uint8_t result;
    i2c_read_blocking(EEPROM_PORT, EEPROM_ADDR, &result, 1, false);

    return result;
}
