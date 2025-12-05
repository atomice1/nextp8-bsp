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

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "nextp8.h"

enum message_type {
  FATAL_ERROR,
  RECOVERABLE_ERROR,
  MESSAGE
};

static void _show_message_common(enum message_type message_type, const char *message)
{
  int vfront = *(uint8_t *)_VFRONT;
  int vback = 1 - vfront;
  _clear_screen(message_type == MESSAGE ? _DARK_BLUE : _RED);
  _display_string_centered(_SCREEN_WIDTH / 2,
                           _SCREEN_HEIGHT / 2,
                           message);
  _uart_write(message, strlen(message));
  _uart_write("\n", 1);
  usleep(1000000);
  _display_string_centered(_SCREEN_WIDTH / 2,
                           _SCREEN_HEIGHT - _FONT_LINE_HEIGHT * 2,
                           (message_type == FATAL_ERROR) ?
                           "Press any key to reset..." :
                           "Press any key to continue...");
  _flip();
  _wait_for_any_key();
}

#ifdef ROM
void _recoverable_error(const char *message)
#else
void _recoverable_error(const char *format, ...)
#endif
{
#ifndef ROM
  va_list ap;
  va_start(ap, format);
  char message[256];
  vsnprintf(message, sizeof(message), format, ap);
  va_end(ap);
#endif
  _show_message_common(RECOVERABLE_ERROR, message);
}

#ifdef ROM
void __attribute__ ((noreturn)) _fatal_error(const char *message)
#else
void __attribute__ ((noreturn)) _fatal_error(const char *format, ...)
#endif
{
#ifndef ROM
  va_list ap;
  va_start(ap, format);
  char message[256];
  vsnprintf(message, sizeof(message), format, ap);
  va_end(ap);
#endif
  _show_message_common(FATAL_ERROR, message);
  _warm_reset();
}

#ifndef ROM
void _show_message(const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  char message[256];
  va_end(ap);
  vsnprintf(message, sizeof(message), format, ap);
  _show_message_common(MESSAGE, message);
}
#endif

#ifdef ROM
void _rom_fatal_error(const char *fn, int res)
{
  char buf[40];
  size_t fn_len = strlen(fn);
  memcpy(buf, fn, fn_len);
  memcpy(buf + fn_len, ": error ", 8);
  buf[fn_len+8] = (res < 0) ? '-' : ' ';
  if (res < 0) res = -res;
  buf[fn_len+9] = '0' + ((res / 1000) % 10);
  buf[fn_len+10] = '0' + ((res / 100) % 10);
  buf[fn_len+11] = '0' + ((res / 10) % 10);
  buf[fn_len+12] = '0' + (res % 10);
  buf[fn_len+13] = '\0';
  _fatal_error(buf);
}
#endif
