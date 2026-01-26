/*
 * Copyright (C) 2026 Chris January
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "mmio.h"
#include "nextp8.h"

/* I2C control bits (write) */
#define I2C_ENA_BIT  0x01        /* Enable/latch command */
#define I2C_RW_BIT   0x02        /* Read/Write select (0=Write, 1=Read) */

/* I2C status bits (read) */
#define I2C_BUSY_BIT 0x01        /* Transaction busy flag */
#define I2C_ERR_BIT  0x02        /* Acknowledge error flag */

#define I2C_TIMEOUT  1000000

#ifndef ROM
static int wait_i2c_busy_rise(void)
{
    unsigned char status;
    int timeout = I2C_TIMEOUT;

    while (timeout-- > 0) {
        status = MMIO_REG8(_I2C_STATUS);
        if ((status & I2C_BUSY_BIT) != 0) break;
    }
    if (timeout <= 0) {
        errno = ETIMEDOUT;
        return -1;
    }
    if ((status & I2C_ERR_BIT) != 0) {
        errno = EIO;
        return -1;
    }
    return 0;
}

static int wait_i2c_busy_fall(void)
{
    unsigned char status;
    int timeout = I2C_TIMEOUT;

    while (timeout-- > 0) {
        status = MMIO_REG8(_I2C_STATUS);
        if ((status & I2C_BUSY_BIT) == 0) break;
    }
    if (timeout <= 0) {
        errno = ETIMEDOUT;
        return -1;
    }
    if ((status & I2C_ERR_BIT) != 0) {
        errno = EIO;
        return -1;
    }
    return 0;
}

int _read_rtc(unsigned *date, unsigned *month, unsigned *year,
              unsigned *hours, unsigned *minutes, unsigned *seconds,
              unsigned *wday)
{
    int ret;
    unsigned char status;

    /* Wait for I2C to be idle before starting */
    ret = wait_i2c_busy_fall();
    if (ret != 0) return ret;

    /* Cmd1: write register address 0x0 (seconds) */
    MMIO_REG8(_I2C_DATA) = 0x00;
    MMIO_REG8(_I2C_CTRL) = I2C_ENA_BIT;

    /* Wait for busy to rise */
    ret = wait_i2c_busy_rise();
    if (ret != 0) return ret;

    /* Cmd2: read date (set rw=1) */
    MMIO_REG8(_I2C_CTRL) = I2C_ENA_BIT | I2C_RW_BIT;

    /* Wait for busy to clear */
    ret = wait_i2c_busy_fall();
    if (ret != 0) return ret;

    /* Wait for busy to rise (seconds byte ready) */
    ret = wait_i2c_busy_rise();
    if (ret != 0) return ret;

    /* Wait for busy to clear */
    ret = wait_i2c_busy_fall();
    if (ret != 0) return ret;

    /* Capture seconds */
    uint8_t data = MMIO_REG8(_I2C_DATA);
    *seconds = ((data >> 4) & 0x07) * 10 + (data & 0x0F);

    /* Wait for busy to rise (minutes byte ready) */
    ret = wait_i2c_busy_rise();
    if (ret != 0) return ret;

    /* Wait for busy to clear */
    ret = wait_i2c_busy_fall();
    if (ret != 0) return ret;

    /* Capture minutes */
    data = MMIO_REG8(_I2C_DATA);
    *minutes = ((data >> 4) & 0x07) * 10 + (data & 0x0F);

    /* Wait for busy to rise (hours byte ready) */
    ret = wait_i2c_busy_rise();
    if (ret != 0) return ret;

    /* Wait for busy to clear */
    ret = wait_i2c_busy_fall();
    if (ret != 0) return ret;

    /* Capture hours */
    data = MMIO_REG8(_I2C_DATA);
    if (data & (1 << 6))
        *hours = ((data & 0x0F) + 10 * ((data >> 4) & 0x01)) + ((data & (1 << 5)) ? 12 : 0);
    else
        *hours = (data & 0x0F) + 10 * ((data >> 4) & 0x03);

    /* Wait for busy to rise (wday byte ready) */
    ret = wait_i2c_busy_rise();
    if (ret != 0) return ret;

    /* Wait for busy to clear */
    ret = wait_i2c_busy_fall();
    if (ret != 0) return ret;

    /* Capture wday */
    data = MMIO_REG8(_I2C_DATA);
    *wday = data & 0x7;

    /* Wait for busy to rise (date byte ready) */
    ret = wait_i2c_busy_rise();
    if (ret != 0) return ret;

    /* Wait for busy to clear */
    ret = wait_i2c_busy_fall();
    if (ret != 0) return ret;

    /* Capture date */
    data = MMIO_REG8(_I2C_DATA);
    *date = (data & 0x0F) + 10 * ((data >> 4) & 0x03);

    /* Wait for busy to rise (month byte ready) */
    ret = wait_i2c_busy_rise();
    if (ret != 0) return ret;

    /* Wait for busy to clear */
    ret = wait_i2c_busy_fall();
    if (ret != 0) return ret;

    /* Capture month */
    data = MMIO_REG8(_I2C_DATA);
    *month = (data & 0x0F) + 10 * ((data >> 4) & 0x01);

    /* Wait for busy to rise (year byte ready) */
    ret = wait_i2c_busy_rise();
    if (ret != 0) return ret;

    /* Stop I2C transaction */
    MMIO_REG8(_I2C_CTRL) = 0x00;

    /* Wait for busy to clear */
    ret = wait_i2c_busy_fall();
    if (ret != 0) return ret;

    /* Capture year */
    data = MMIO_REG8(_I2C_DATA);
    *year = (data & 0x0F) + 10 * ((data >> 4) & 0x0F) + 2000;

    char buf[100];
    snprintf(buf, sizeof(buf), "RTC read: %02u/%02u/%04u %02u:%02u:%02u\n",
             *date, *month, *year, *hours, *minutes, *seconds);
    _uart_write(buf, strlen(buf));
    return 0;
}
#endif