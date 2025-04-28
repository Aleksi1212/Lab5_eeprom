#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

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

void erase_log()
{
    for (uint32_t i = 0; i < MAX_LOG_ENTRIES; i++) {
        uint16_t log_addr = i * LOG_ENTRY_SIZE;
        eeprom_write_bytes(log_addr, 0);
    }
}

void write_log_entry(const char *log_entry)
{
    uint8_t buffer[LOG_ENTRY_SIZE];
    size_t len = strlen(log_entry);

    if (len > LOG_ENTRY_SIZE) {
        fprintf(stderr, "ERROR: Log message too long. Max size %d bytes\n", LOG_ENTRY_SIZE - 2);
        return;
    }
  
    memcpy(buffer, log_entry, len + 1);
  
    uint16_t crc = crc16(buffer, len + 1);
    buffer[LOG_ENTRY_SIZE - 2] = (crc >> 8) & 0xFF;
    buffer[LOG_ENTRY_SIZE - 1] = crc & 0xFF;
  
    uint16_t log_address = 0;
    for (uint32_t i = 0; i < MAX_LOG_ENTRIES; i++) {
        uint16_t current_address = i * LOG_ENTRY_SIZE;
        if (eeprom_read_byte(current_address) == 0) {
            log_address = current_address;
            break;
        }
    }
  
    if (log_address == 0) {
        erase_log();
        printf("Log full erasing\n");
    }
  
    for (uint32_t i = 0; i < LOG_ENTRY_SIZE; i++) {
        eeprom_write_bytes(log_address + i, buffer[i]);
    }
}

void read_log()
{
    char log_buffer[LOG_ENTRY_SIZE - 2];
    for (uint32_t i = 0; i < MAX_LOG_ENTRIES; i++) {
        uint16_t log_address = i * 64;
        if (eeprom_read_byte(log_address) == 0) {
            break;
        }
  
        for (uint32_t j = 0; j < LOG_ENTRY_SIZE - 3; j++) {
            log_buffer[j] = eeprom_read_byte(log_address + j);
            if (log_buffer[j] == 0) {
                log_buffer[j] = '\0';
                break;
            }
        }
  
        uint16_t stored_crc =
            (eeprom_read_byte(log_address + LOG_ENTRY_SIZE - 2) << 8) |
            eeprom_read_byte(log_address + LOG_ENTRY_SIZE - 1);

        uint16_t expected_crc = crc16((uint8_t *)log_buffer, strlen(log_buffer) + 1);
  
        if (expected_crc != stored_crc) {
            fprintf(stderr, "ERROR: Invalid log entry: CRC mismatch\n");
            break;
        }
  
        printf("> %s\n", log_buffer);
    }
}

void log_message(const char *fmt, ...)
{
    char log_buffer[128];
    va_list args;
    
    // Start processing variable arguments
    va_start(args, fmt);

    // Get the current time
    uint64_t time_seconds = time_us_64() / 1000000;

    // Write the initial part with the timestamp
    int len = snprintf(log_buffer, sizeof(log_buffer), "Time since boot (%lld s) --- ", time_seconds);

    if (len < 0 || (uint32_t)len >= sizeof(log_buffer)) {
        // snprintf failed or the initial part already overflowed
        log_buffer[sizeof(log_buffer) - 1] = '\0';
    } else {
        // Write the user message appended to the timestamp
        int remaining = sizeof(log_buffer) - len;
        if (remaining > 0) {
            int user_len = vsnprintf(log_buffer + len, remaining, fmt, args);
            if (user_len < 0 || (uint32_t)user_len >= (uint32_t)remaining) {
                // If vsnprintf indicates overflow, make sure it's null-terminated
                log_buffer[sizeof(log_buffer) - 1] = '\0';
            }
        }
    }

    va_end(args);

    printf("%s\n", log_buffer);
    write_log_entry(log_buffer);
}

bool read_user_input(char *dst, size_t dst_size)
{
    int32_t chr = getchar_timeout_us(0);
    if (chr == PICO_ERROR_TIMEOUT) {
        return false;
    }

    size_t i = 0;

    printf("%c", chr);

    if (chr == '\r' || chr == '\n') {
        return false;
    }

    dst[i++] = chr;

    while (i < dst_size)
    {
        chr = getchar();
        printf("%c", chr);

        if (chr == '\r' || chr == '\n') {
            break;
        }
        dst[i++] = chr;
    }

    dst[i] = '\0';
    return true;
}