/*
 * io-stat.c -- 
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

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "io.h"

/*
 * stat -- get file information
 * input parameters:
 *   0 : filename ptr
 *   1 : filename length
 *   2 : stat buf ptr
 * output parameters:
 *   0 : result
 *   1 : errno
 */


int stat (const char *__restrict filename, struct stat *__restrict buf)
{
  return _fatfs_stat(filename, buf);
}
