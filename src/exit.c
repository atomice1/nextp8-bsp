/*
 * exit.c --
 *
 * Copyright (c) 2025 Chris January
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
#include "nextp8.h"

char *last_error = NULL;

extern void __reset (void);

/*
 * _exit -- Exit from the application.
 */

void __attribute__ ((noreturn)) _exit (int code)
{
  if (code != 0)
    {
      if (last_error)
        _fatal_error(last_error);
      else
        _fatal_error("fatal error");
    }
  else
    {
      while (1)
        {
          _warm_reset();
        }
    }
}
