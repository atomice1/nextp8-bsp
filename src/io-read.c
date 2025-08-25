/*
 * io-read.c -- 
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

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "io.h"

/*
 * read -- read from a file descriptor
 * input parameters:
 *   0 : file descriptor
 *   1 : buf ptr
 *   2 : count
 * output parameters:
 *   0 : result
 *   1 : errno
 */

ssize_t read (int fd, void *buf, size_t count)
{
  _init_stdio();
  if (fd < 0 || fd >= _NR_FILES || _files[fd].ops == NULL)
    {
      errno = EBADF;
      return -1;
    }
  if (_files[fd].ops->read == NULL)
    {
      errno = EINVAL;
      return -1;
    }
  return _files[fd].ops->read(&_files[fd], buf, count);
}
