#ifndef CONSTANTS_H
#define CONSTANTS_H

#define LED_COUNT 3
#define SW_COUNT LED_COUNT

#define LED_1 20
#define LED_2 21
#define LED_3 22

#define SW_0 9
#define SW_1 8
#define SW_2 7

#define EEPROM_ADDR 0x50
#define EEPROM_PORT i2c0
#define EEPROM_SDA_PIN 16
#define EEPROM_SCL_PIN 17
#define EEPROM_BAUDRATE 100 * 1000
#define EEPROM_MAX_ADDRESS 0x7FFF

#endif