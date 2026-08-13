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

/*
 * _exit -- Exit from the application.
 */

void __attribute__ ((noreturn)) _exit (int code)
{
  _set_postcode(9);
  if (code != 0)
    {
      _set_postcode(10);
      if (last_error) {
        _set_postcode(11);
        _fatal_error(last_error);
      }
      else {
        _set_postcode(12);
        _fatal_error("fatal error");
      }
    }
  else
    {
#ifdef ROM
      if (_config_data && _config_data->exit_action == 1)
        {
          _set_postcode(13);
          _shutdown();
          _set_postcode(14);
        }
      else
        {
          _set_postcode(15);
          _warm_reset();
          _set_postcode(16);
        }
#else
      if (_config_data && _config_data->exit_action == 1)
        {
          _set_postcode(17);
          _shutdown();
          _set_postcode(118);
        }
      else
        {
          _set_postcode(19);
          _restart();
          _set_postcode(20);
        }
#endif
    }
    _set_postcode(21);
}
