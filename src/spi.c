/* 
 * Copyright (C) 2025 Chris January
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <stdio.h>

#include "spi.h"
#include "nextp8.h"

static uint8_t write_fill = SPI_FILL_CHAR;
static int select_count = 0;

#define SDSPI_CLOCK 11000000

void _spi_format(int bits, int mode /* = 0 */)
{
}

void _spi_frequency(int hz)
{
  *(volatile uint8_t *)_SDSPI_DIVIDER = SDSPI_CLOCK / hz;
}

int _spi_write(int value)
{
  while (*(volatile uint8_t *)_SDSPI_READY) {}; /* Wait for ready */
  *(volatile uint8_t *)_SDSPI_DATA_IN = value;
  *(volatile uint8_t *)_SDSPI_WRITE_ENABLE = 1;
  while (!*(volatile uint8_t *)_SDSPI_READY) {}; /* Wait for latch */
  *(volatile uint8_t *)_SDSPI_WRITE_ENABLE = 0;
  while (*(volatile uint8_t *)_SDSPI_READY) {}; /* Wait for send to finish */
  int ret = *(volatile uint8_t *)_SDSPI_DATA_OUT;
  return ret;
}

int _spi_block_write(const char *tx_buffer, int tx_length, char *rx_buffer, int rx_length)
{
  int total = (tx_length > rx_length) ? tx_length : rx_length;

  for (int i = 0; i < total; i++)
    {
      char out = (i < tx_length) ? tx_buffer[i] : write_fill;
      char in = _spi_write(out);
      if (i < rx_length)
        {
          rx_buffer[i] = in;
        }
    }

  return total;
}

void _spi_lock(void)
{
}

void _spi_unlock(void)
{
}

void _spi_select(int index)
{
  if (select_count++ == 0)
    *(volatile uint8_t *)_SDSPI_CHIP_SELECT = 0xff & ~(1 << index);
}

void _spi_deselect()
{
  if (--select_count == 0)
    *(volatile uint8_t *)_SDSPI_CHIP_SELECT = 0xff;
}

void _spi_set_default_write_value(char data)
{
  write_fill = data;
}
