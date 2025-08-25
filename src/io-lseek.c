/*
 * io-lseek.c -- 
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

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "io.h"

/*
 * lseek -- reposition a file descriptor
 * input parameters:
 *   0 : file descriptor
 *   1 : high word of offset
 *   2 : low word of offset
 *   3 : seek flag
 * output parameters:
 *   0 : high word of result
 *   1 : low word of result
 *   2 : errno
 */

off_t lseek (int fd, off_t offset, int whence)
{
  _init_stdio();
  if (fd < 0 || fd >= _NR_FILES || _files[fd].ops == NULL)
    {
      errno = EBADF;
      return -1;
    }
  if (_files[fd].ops->lseek == NULL)
    {
      errno = EINVAL;
      return -1;
    }
  return _files[fd].ops->lseek(&_files[fd], offset, whence);
}
