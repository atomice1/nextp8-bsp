/*
 * hosted & unhosted io support.
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

#ifndef IO_H
#define IO_H

#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "ff.h"

#define _NR_FILES 8

struct _file;

struct _file_ops {
  int (*close)(struct _file *file);
  int (*fstat)(struct _file *file, struct stat *buf);
  int (*isatty)(struct _file *file);
  off_t (*lseek)(struct _file *file, off_t offset, int whence);
  ssize_t (*read)(struct _file *file, void *buf, size_t count);
  ssize_t (*write)(struct _file *file, const void *buf, size_t count);
};

struct _file {
  struct _file_ops *ops;
  union {
    int stdio;
    FIL fil;
  };
};

extern void _init_stdio();

extern void _init_fatfs();
extern int _fatfs_access(const char *pathname, int mode);
extern int _fatfs_open(struct _file *file, const char *filename, int flags, mode_t mode);
extern int _fatfs_rename (const char *oldpath, const char *newpath);
extern int _fatfs_mkdir(const char *pathname, mode_t mode);
extern int _fatfs_stat (const char *__restrict filename, struct stat *__restrict buf);
extern int _fatfs_unlink(const char *path);
extern DIR *_fatfs_opendir(const char *name);
extern struct dirent *_fatfs_readdir(DIR *dirp);
extern int _fatfs_closedir(DIR *dirp);

extern struct _file _files[_NR_FILES];

#endif
