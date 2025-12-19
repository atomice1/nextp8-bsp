/*
 * Copyright (C) 2025 Chris January
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

#include <stddef.h>
#include <stdint.h>
#include "nextp8.h"

void _uart_write(const char *buf, size_t count)
{
  const char *src = (const char *) buf;
  for (size_t i=0;i<count;++i)
    {
      char c = *src++;
      *(volatile uint8_t *)_UART_DATA = c;
      while (*(volatile uint8_t *)_UART_CTRL & 2 == 0) { }
      *(volatile uint8_t *)_UART_CTRL = 1;
      *(volatile uint8_t *)_UART_CTRL = 1;
      *(volatile uint8_t *)_UART_CTRL = 1;
      *(volatile uint8_t *)_UART_CTRL = 0;
    }
 }