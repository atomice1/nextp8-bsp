/**
 * Copyright (C) 2026 Chris January
 *
 * ESP8266 AT command interface for nextp8
 */

#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "nextp8.h"
#include "mmio.h"

/* ESP_CTRL register bits */
/* Read bits: 1=dr (data ready), 2=rd (ready), 4=ra, 8=wa */
/* Write bits: 1=w (write strobe), 2=r (read strobe) */
#define ESP_CTRL_DATA_READY  0x01  /* Read: data ready */
#define ESP_CTRL_READY       0x02  /* Read: ready */
#define ESP_CTRL_WRITE_STROBE 0x01 /* Write: write strobe */
#define ESP_CTRL_READ_STROBE  0x02 /* Write: read strobe */

/* AT command timeout (in microseconds) */
#define AT_TIMEOUT_US 5000000  /* 5 seconds */

/* Buffer size for AT responses */
#define AT_RESPONSE_BUF_SIZE 512

/* ESP8266 initialization state */
static bool esp_initialized = false;

/* Write a single byte to ESP8266 UART */
int _esp_write_byte(unsigned char byte)
{
    unsigned timeout = 10000;

    /* Write data */
    MMIO_REG8(_ESP_DATA) = byte;

    /* Wait for ready (bit 2) */
    while (!(MMIO_REG8(_ESP_CTRL) & ESP_CTRL_READY)) {
        if (--timeout == 0) {
            errno = EIO;
            return -1;
        }
    }

    /* Pulse write strobe (bit 0) */
    MMIO_REG8(_ESP_CTRL) = ESP_CTRL_WRITE_STROBE;
    MMIO_REG8(_ESP_CTRL) = ESP_CTRL_WRITE_STROBE;
    MMIO_REG8(_ESP_CTRL) = ESP_CTRL_WRITE_STROBE;
    MMIO_REG8(_ESP_CTRL) = 0;

    return 0;
}

/* Read a single byte from ESP8266 UART */
int _esp_read_byte(unsigned char *byte, unsigned timeout_us)
{
    unsigned start_time = MMIO_REG32(_UTIMER_1MHZ);

    /* Wait for data ready (bit 1) */
    while (1) {
        if (MMIO_REG8(_ESP_CTRL) & ESP_CTRL_DATA_READY) {
            *byte = MMIO_REG8(_ESP_DATA);

            /* Pulse read strobe (bit 1) */
            MMIO_REG8(_ESP_CTRL) = ESP_CTRL_READ_STROBE;
            MMIO_REG8(_ESP_CTRL) = ESP_CTRL_READ_STROBE;
            MMIO_REG8(_ESP_CTRL) = ESP_CTRL_READ_STROBE;
            MMIO_REG8(_ESP_CTRL) = 0;

            return 0;
        }

        unsigned elapsed = MMIO_REG32(_UTIMER_1MHZ) - start_time;
        if (elapsed >= timeout_us) {
            errno = ETIMEDOUT;
            return -1;
        }
    }
}

/* Write a string to ESP8266 */
int _esp_write_string(const char *str)
{
    while (*str) {
        if (_esp_write_byte(*str++) < 0) {
            return -1;
        }
    }
    return 0;
}

/* Read a line from ESP8266 (terminated by \r\n) */
int _esp_read_line(char *buffer, size_t buf_size, unsigned timeout_us)
{
    size_t pos = 0;
    unsigned char ch;
    unsigned start_time = MMIO_REG32(_UTIMER_1MHZ);

    while (pos < buf_size - 1) {
        unsigned elapsed = MMIO_REG32(_UTIMER_1MHZ) - start_time;
        if (_esp_read_byte(&ch, timeout_us - elapsed) < 0) {
            buffer[pos] = '\0';
            return -1;
        }

        if (ch == '\n') {
            /* Strip trailing \r if present */
            if (pos > 0 && buffer[pos - 1] == '\r') {
                buffer[pos - 1] = '\0';
            } else {
                buffer[pos] = '\0';
            }
            return pos;
        }

        buffer[pos++] = ch;
    }

    buffer[buf_size - 1] = '\0';
    errno = EOVERFLOW;
    return -1;
}

/* Send AT command and wait for expected response */
int _esp_send_at_command(const char *cmd, const char *expected_response, unsigned timeout_us)
{
    char response[AT_RESPONSE_BUF_SIZE];

    /* Send command */
    if (_esp_write_string(cmd) < 0) {
        return -1;
    }
    if (_esp_write_string("\r\n") < 0) {
        return -1;
    }

    /* Read response lines until we find the expected response or timeout */
    unsigned start_time = MMIO_REG32(_UTIMER_1MHZ);

    while (1) {
        unsigned elapsed = MMIO_REG32(_UTIMER_1MHZ) - start_time;
        if (elapsed >= timeout_us) {
            errno = ETIMEDOUT;
            return -1;
        }

        if (_esp_read_line(response, sizeof(response), timeout_us - elapsed) < 0) {
            return -1;
        }

        /* Check for expected response */
        if (expected_response && strstr(response, expected_response) != NULL) {
            return 0;
        }

        /* Check for error */
        if (strcmp(response, "ERROR") == 0 || strcmp(response, "FAIL") == 0) {
            errno = EIO;
            return -1;
        }
    }
}

/* Wait for a specific prompt from ESP8266 */
int _esp_wait_for_prompt(const char *prompt, unsigned timeout_us)
{
    size_t prompt_len = strlen(prompt);
    size_t pos = 0;
    unsigned char ch;
    unsigned start_time = MMIO_REG32(_UTIMER_1MHZ);

    while (pos < prompt_len) {
        unsigned elapsed = MMIO_REG32(_UTIMER_1MHZ) - start_time;
        if (_esp_read_byte(&ch, timeout_us - elapsed) < 0) {
            return -1;
        }

        if (ch == prompt[pos]) {
            pos++;
        } else {
            pos = 0;
        }
    }

    return 0;
}

/* Initialize ESP8266 module */
int _esp_init(void)
{
    if (esp_initialized) {
        return 0;
    }

    /* Set baud rate to 115200 (assuming 33MHz system clock) */
    /* Baud rate divisor = clock / (16 * baud_rate) */
    /* For 115200: divisor = 33000000 / (16 * 115200) ≈ 17.9, use 18 */
    MMIO_REG16(_ESP_BAUD_DIV) = 18;

    /* Test AT communication */
    if (_esp_send_at_command("AT", "OK", AT_TIMEOUT_US) < 0) {
        errno = ENODEV;
        return -1;
    }

    /* Disable echo */
    if (_esp_send_at_command("ATE0", "OK", AT_TIMEOUT_US) < 0) {
        return -1;
    }

    /* Set Wi-Fi mode to station (1) */
    if (_esp_send_at_command("AT+CWMODE=1", "OK", AT_TIMEOUT_US) < 0) {
        return -1;
    }

    /* Enable single connection mode */
    if (_esp_send_at_command("AT+CIPMUX=0", "OK", AT_TIMEOUT_US) < 0) {
        return -1;
    }

    esp_initialized = true;
    return 0;
}
