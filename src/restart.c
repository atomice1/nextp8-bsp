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

#include "nextp8.h"

extern char *__initial_stack;
extern void __start(void);

#ifndef ROM
void __attribute__ ((noreturn)) _restart(void)
{
  _loader_data->reset_type = _RESET_TYPE_APP_RESTART;
  __asm__("move.l %0,%%sp\n"
          "move.l %1,%%a0\n"
          "jmp    (%%a0)\n"
          :: "g"(__initial_stack), "g"(__start));
  __builtin_unreachable();
}
#endif
