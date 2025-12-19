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

void __attribute__ ((noreturn)) _warm_reset(void)
{
#ifdef ROM
    __asm__("move.l (0).l,%sp\n"
            "move.l (4).l,%a0\n"
            "jmp    (%a0)\n");
#else
    __asm__("move.l %0,%%sp\n"
            "move.l %1,%%a0\n"
            "jmp    (%%a0)\n"
            :: "g"(__initial_stack), "g"(__start));
#endif
    __builtin_unreachable();
}
