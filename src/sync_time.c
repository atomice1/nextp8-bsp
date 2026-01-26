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

#include <time.h>
#include "mmio.h"
#include "nextp8.h"

#ifndef ROM
time_t _boot_time_realtime_s;
uint64_t _boot_time_monotonic_us;

int _sync_time(void)
{
  unsigned date = 0, month = 0, year = 0, hours = 0, minutes = 0, seconds = 0, wday = 0;
  struct tm tm;
  uint64_t us;
  time_t realtime;
  int res = _read_rtc(&date, &month, &year, &hours, &minutes, &seconds, &wday);
  if (res != 0)
    return -1;
  us = MMIO_REG64(_UTIMER_1MHZ);
  tm.tm_mday = date;
  tm.tm_mon = month - 1;
  tm.tm_year = year - 1900;
  tm.tm_wday = (wday == 7) ? 0 : wday;
  tm.tm_hour = hours;
  tm.tm_min = minutes;
  tm.tm_sec = seconds;
  tm.tm_isdst = -1;
  realtime = mktime(&tm);
  _boot_time_realtime_s = (realtime == (time_t) -1) ? 0 : realtime;
  _boot_time_monotonic_us = us;
  return 0;
}
#endif