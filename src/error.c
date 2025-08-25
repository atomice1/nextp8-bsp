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

static void write_tube(const char *message)
{
  char *p =  (char *)_TUBE_STDERR;
  char c;
  while ((c = *message++))
    *p = c;
  *p = '\n';
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
  char message[200];
#endif
  int vfront = *(uint8_t *)_VFRONT;
  int vback = 1 - vfront;
  _clear_screen(_RED);
#ifndef ROM
   vsnprintf(message, sizeof(message), format, ap);
#endif
  _display_string_centered(_SCREEN_WIDTH / 2,
                           _SCREEN_HEIGHT / 2,
                           message);
  write_tube(message);
  usleep(1000000);
  _display_string_centered(_SCREEN_WIDTH / 2,
                           _SCREEN_HEIGHT * 3 / 4,
                           "Press any key to continue...");
  _flip();
  _wait_for_any_key();
#ifndef ROM
  va_end(ap);
#endif
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
  char message[200];
#endif
  _clear_screen(_RED);
#ifndef ROM
  vsnprintf(message, sizeof(message), format, ap);
#endif
  _display_string_centered(_SCREEN_WIDTH / 2,
                           _SCREEN_HEIGHT / 2,
                           message);
  write_tube(message);
  usleep(1000000);
  _display_string_centered(_SCREEN_WIDTH / 2,
                           _SCREEN_HEIGHT * 3 / 4,
                           "Press any key to reset...");
  _flip();
  _wait_for_any_key();
  _warm_reset();
#ifndef ROM
  va_end(ap);
#endif
}