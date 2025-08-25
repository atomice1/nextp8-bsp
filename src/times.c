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

#include <stdint.h>
#include <sys/times.h>
#include <time.h>
#include "nextp8.h"

clock_t times(struct tms *buf)
{
    uint32_t utimer;
    if (CLOCKS_PER_SEC == 1000)
        utimer = *(volatile uint32_t *)_UTIMER_1KHZ_HI;
    else
        utimer = *(volatile uint32_t *)_UTIMER_1MHZ_HI;
    buf->tms_utime = utimer;
    buf->tms_stime = utimer;
    buf->tms_cutime = utimer;
    buf->tms_cstime = utimer;
    return 0;
}
