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
#include <time.h>
#include "nextp8.h"
#include "mmio.h"

extern time_t _boot_time_realtime_s;
extern uint64_t _boot_time_monotonic_us;
extern int _sync_time(void);

int clock_gettime(clockid_t clockid, struct timespec *ts)
{
    uint64_t microseconds;
    switch (clockid) {
#ifndef ROM
    case CLOCK_REALTIME:
        if (_boot_time_monotonic_us == 0) {
            int res = _sync_time();
            if (res != 0)
                return -1;
        }
        microseconds = MMIO_REG64(_UTIMER_1MHZ) - _boot_time_monotonic_us;
        microseconds += _boot_time_realtime_s * UINT64_C(1000000);
        ts->tv_sec = microseconds / UINT64_C(1000000);
        ts->tv_nsec = (microseconds % UINT64_C(1000000)) * UINT64_C(1000);
        return 0;
#endif
    case CLOCK_MONOTONIC:
        microseconds = MMIO_REG64(_UTIMER_1MHZ);
        ts->tv_sec = microseconds / UINT64_C(1000000);
        ts->tv_nsec = (microseconds % UINT64_C(1000000)) * UINT64_C(1000);
        return 0;
    default:
        errno = EINVAL;
        return -1;
    }
}