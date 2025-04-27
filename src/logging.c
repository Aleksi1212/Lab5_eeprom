#include <stdio.h>
#include <string.h>

#include "structs.h"
#include "logging.h"
#include "eeprom.h"
#include "constants.h"

void clear_terminal()
{
    printf("\033[2J\033[H");
    fflush(stdout);
}

static uint16_t crc16(const uint8_t *data_p, size_t length)
{
    uint8_t x;
    uint16_t crc = 0xFFFF;

    while (length--) {
        x = crc >> 8 ^ *data_p++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t) (x << 12)) ^ ((uint16_t) (x << 5)) ^ ((uint16_t) x);
    }

    return crc;
}

void write_log_entry_to_eeprom(const char *log_entry)
{
    uint8_t buffer[LOG_ENTRY_SIZE];
    size_t log_entry_len = strlen(log_entry);

    if (log_entry_len > LOG_ENTRY_SIZE) {
        fprintf(stderr, "ERROR: Log message too long. Max size %d bytes\n", LOG_ENTRY_SIZE - 2);
        return;
    }

    memcpy(buffer, log_entry, log_entry_len + 1);

    uint16_t crc = crc16(buffer, log_entry_len + 1);
    buffer[LOG_ENTRY_SIZE - 2] = (crc >> 8) & 0xFF;
    buffer[LOG_ENTRY_SIZE - 1] = crc & 0xFF;

    uint16_t log_addr = 0;
    for (uint32_t i = 0; i < MAX_LOG_ENTRIES; i++) {
        uint16_t current_addr = i * LOG_ENTRY_SIZE;

        if (eeprom_read_byte(current_addr) == 0) {
            log_addr = current_addr;
            break;
        }
    }

    if (log_addr == 0) {
        erase_log();
        printf("Log full erasing...\n");
    }

    for (uint32_t i = 0; i < MAX_LOG_ENTRIES; i++) {
        eeprom_write_bytes(log_addr + i, buffer[i]);
    }
}

void read_log_from_eeprom()
{
    char log_buffer[LOG_ENTRY_SIZE - 2];
    for (uint32_t i = 0; i < MAX_LOG_ENTRIES; i++) {
        uint16_t log_addr = i * LOG_ENTRY_SIZE;
        if (eeprom_read_byte(log_addr) == 0) {
            break;
        }

        for (uint32_t j = 0; j < LOG_ENTRY_SIZE; j++) {
            log_buffer[j] = eeprom_read_byte(log_addr + j);
            if (log_buffer[j] == 0) {
                log_buffer[j] = '\0';
                break;
            }
        }

        uint16_t stored_crc =
            (eeprom_read_byte(log_addr + LOG_ENTRY_SIZE - 2) << 8) |
            (eeprom_read_byte(log_addr + LOG_ENTRY_SIZE - 1));

        uint16_t expected_crc = crc16((uint8_t *)log_buffer, strlen(log_buffer) + 1);

        if (stored_crc != expected_crc) {
            fprintf(stderr, "ERROR: Invalid log entry: CRC mismatch\n");
            break;
        }

        printf("> %s\n", log_buffer);
    }

}

static void erase_log()
{
    for (uint32_t i = 0; i < MAX_LOG_ENTRIES; i++) {
        uint16_t log_addr = i * LOG_ENTRY_SIZE;
        eeprom_write_bytes(log_addr, 0);
    }
}