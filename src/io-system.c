/*
 * io-system.c -- 
 *
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
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include "io.h"

/*
 * system: execute command on (remote) host
 * input parameters:
 *   0 : command ptr
 *   1 : command length
 * output parameters:
 *   0 : result
 *   1 : errno
 */

int _system (const char *command)
{
  if (!command)
    return 0;
  errno = ENOSYS;
  return -1;
}

/* Emit an object that causes a gnu linker warning.  */
static const char __warn_system[]
__attribute__ ((section (".gnu.warning"), used)) =
"IO function 'system' used";