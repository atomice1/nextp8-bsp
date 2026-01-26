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

int clock_getres(clockid_t clockid, struct timespec *res)
{
    switch (clockid) {
#ifndef ROM
    case CLOCK_REALTIME:
#endif
    case CLOCK_MONOTONIC:
        if (res) {
            res->tv_sec = 0;
            res->tv_nsec = 1000; /* 1 microsecond resolution */
        }
        return 0;
    default:
        errno = EINVAL;
        return -1;
    }
}