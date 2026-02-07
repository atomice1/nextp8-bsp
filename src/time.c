/*
 * io-time.c --
 *
 * Copyright (C) 2025 Chris January
 * Copyright (c) 2006 CodeSourcery Inc
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

#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include "io.h"
#include "mmio.h"
#include "nextp8.h"

extern time_t _boot_time_realtime_s;
extern uint64_t _boot_time_monotonic_us;
extern int _sync_time(void);

/*
 * time -- get the current time
 * input parameters:
 *   0 : timeval ptr
 * output parameters:
 *   0 : result
 *   1 : errno
 */

time_t time (time_t *t)
{
  if (_boot_time_monotonic_us == 0) {
      int res = _sync_time();
      if (res != 0)
          return -1;
  }
  uint64_t microseconds = MMIO_REG64(_UTIMER_1MHZ) - _boot_time_monotonic_us;
  microseconds += _boot_time_realtime_s * UINT64_C(1000000);
  time_t seconds = microseconds / UINT64_C(1000000);
  if (seconds)
    *t = seconds;
  return seconds;
}
