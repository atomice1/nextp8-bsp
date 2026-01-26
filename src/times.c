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
#include "mmio.h"
#include "nextp8.h"

clock_t times(struct tms *buf)
{
    uint64_t now = MMIO_REG64(_UTIMER_1MHZ);
    buf->tms_utime = now / (UINT64_C(1000000) / CLOCKS_PER_SEC);
    buf->tms_stime = now / (UINT64_C(1000000) / CLOCKS_PER_SEC);
    buf->tms_cutime = now / (UINT64_C(1000000) / CLOCKS_PER_SEC);
    buf->tms_cstime = now / (UINT64_C(1000000) / CLOCKS_PER_SEC);
    return 0;
}
