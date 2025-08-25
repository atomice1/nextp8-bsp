#ifndef ROM
#include <assert.h>
#endif
#include <sys/types.h>
#define DIR DIRENT_DIR
#include <dirent.h>
#undef DIR
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ff.h"
#include "io.h"
#include "nextp8.h"

#include <stdio.h>

int fresult2errno(FRESULT fr) {
    switch (fr) {
        case FR_OK:
            return 0;
        case FR_DISK_ERR:
            return EIO;
        case FR_INT_ERR:
            return EIO;
        case FR_NOT_READY:
            return EIO;
        case FR_NO_FILE:
            return ENOENT;
        case FR_NO_PATH:
            return ENOENT;
        case FR_INVALID_NAME:
            return ENAMETOOLONG;
        case FR_DENIED:
            return EACCES;
        case FR_EXIST:
            return EEXIST;
        case FR_INVALID_OBJECT:
            return EIO;
        case FR_WRITE_PROTECTED:
            return EACCES;
        case FR_INVALID_DRIVE:
            return ENOENT;
        case FR_NOT_ENABLED:
            return ENOENT;
        case FR_NO_FILESYSTEM:
            return ENOENT;
        case FR_MKFS_ABORTED:
            return EIO;
        case FR_TIMEOUT:
            return EIO;
        case FR_LOCKED:
            return EACCES;
        case FR_NOT_ENOUGH_CORE:
            return ENOMEM;
        case FR_TOO_MANY_OPEN_FILES:
            return ENFILE;
        case FR_INVALID_PARAMETER:
            return ENOSYS;
        default:
            return -1;
    }
}

static int fatfs_initialized;
static FATFS fs[2];
static const char *volume_names[2] = {"0:", "1:"};

void _init_fatfs(void)
{
  FRESULT res;
  if (fatfs_initialized)
    return;
  fatfs_initialized = 1;
  for (int i=0;i<2;++i) {
    res = f_mount(&fs[i], volume_names[i], 1-i);
    if (res != FR_OK)
      {
        if (res == FR_NO_FILESYSTEM)
          {
            const char *msg = "No FAT32 filesystem on SD card.";
            if (i == 0)
              _fatal_error(msg);
            else
              _recoverable_error(msg);
          }
        else 
          {
#ifdef ROM
            char buf[10];
            strncpy(buf, "error: ", 7);
            buf[7] = '0' + ((res / 10) % 10);
            buf[8] = '0' + (res % 10);
            buf[9] = '\0';
            _fatal_error(buf);
#else
            if (i == 0)
              _fatal_error("%s: error %d", volume_names[i], res);
            else
              _recoverable_error("%s: error %d", volume_names[i], res);
#endif
        }
      }
    }
}

int _fatfs_access(const char *pathname, int mode)
{
#ifdef ROM
  errno = ENOSYS;
  return -1;
#else
  FRESULT res;
  FILINFO info;
  res = f_stat(pathname, &info);
  printf("f_stat(%s) = %d\n", pathname, res);
  if (res != FR_OK)
    {
      errno = fresult2errno(res);
      return -1;
    }
  return 0;
#endif
}

static int fatfs_close(struct _file *file)
{
  FRESULT res;
  res = f_close(&file->fil);
  if (res != FR_OK)
    {
      errno = fresult2errno(res);
      return -1;
    }
  return 0;
}

int _fatfs_closedir(DIR *dirp)
{
#ifdef ROM
  errno = ENOSYS;
  return -1;
#else
  FRESULT res = f_closedir(dirp);
  if (res != FR_OK)
    {
      errno = fresult2errno(res);
      return -1;
    }
  free(dirp);
  return 0;
#endif
}

static off_t fatfs_lseek(struct _file *file, off_t offset, int whence)
{
  FRESULT res;
  switch (whence) {
  case SEEK_SET:
    break;
  case SEEK_CUR:
    offset += f_tell(&file->fil);
    break;
  case SEEK_END:
    offset += f_size(&file->fil);
    break;
  }
  res = f_lseek(&file->fil, offset);
  if (res != FR_OK)
    {
      errno = fresult2errno(res);
      return -1;
    }
  return f_tell(&file->fil);
}

int _fatfs_mkdir(const char *pathname, mode_t mode)
{
#ifdef ROM
  errno = ENOSYS;
  return -1;
#else
  FRESULT res = f_mkdir(pathname);
  if (res != FR_OK)
    {
      errno = fresult2errno(res);
      return -1;
    }
  return 0;
#endif
}

DIR *_fatfs_opendir(const char *name)
{
#ifdef ROM
  errno = ENOSYS;
  return NULL;
#else
  FRESULT res;
  DIR *ret;
#ifndef ROM
  assert(sizeof(DIR) == sizeof(DIRENT_DIR));
  assert(sizeof(FILINFO) == sizeof(struct dirent));
#endif
  ret = calloc(1, sizeof(DIR) + sizeof(struct dirent));
  res = f_opendir(ret, name);
  if (res != FR_OK)
    {
      if (res == FR_NO_FILESYSTEM)
        {
          _recoverable_error("No FAT32 filesystem on SD card.");
        }
      errno = fresult2errno(res);
      return NULL;
    }
  return ret;
#endif
}

static ssize_t fatfs_read(struct _file *file, void *buf, size_t count)
{
  FRESULT res;
  unsigned bytes_read = 0;
  res = f_read(&file->fil, buf, count, &bytes_read);
  if (res != FR_OK)
    {
      errno = fresult2errno(res);
      return -1;
    }
  return bytes_read;
}

struct dirent *_fatfs_readdir(DIR *dirp)
{
#ifdef ROM
  errno = ENOSYS;
  return NULL;
#else
  struct dirent *dirent = (struct dirent *)(dirp + 1);
  if (dirent->d_name[0] == '\0') {
    strcpy(dirent->d_name, "..");
    dirent->d_attrib = AM_DIR;
    return dirent;
  }
  FRESULT res = f_readdir(dirp, (FILINFO *)dirent);
  if (res != FR_OK)
    {
      errno = fresult2errno(res);
      return NULL;
    }
  if (dirent->d_name[0] == '\0')
    return NULL;
  return dirent;
#endif
}

int _fatfs_rename (const char *oldpath, const char *newpath)
{
#ifdef FF_FS_READONLY
  errno = EROFS;
  return -1;
#else
  FRESULT res;
  res = f_rename(oldpath, newpath);
  if (res != FR_OK)
    {
      errno = fresult2errno(res);
      return -1;
    }
  return 0;
#endif
}

int _fatfs_stat (const char *__restrict filename, struct stat *__restrict buf)
{
#ifdef ROM
  errno = ENOSYS;
  return -1;
#else
  FRESULT res;
  FILINFO info;
  res = f_stat(filename, &info);
  if (res != FR_OK)
    {
      errno = fresult2errno(res);
      return -1;
    }
  printf("filename: %s attrib: %d 0x%x\n", filename, info.fattrib, info.fattrib);
  buf->st_dev = 0;
  buf->st_ino = 0;
  buf->st_mode = ((info.fattrib & AM_RDO) ? 0444 : 0666) | ((info.fattrib & AM_DIR) ? 0040111 : 0);
  buf->st_nlink = 1;
  buf->st_uid = 0;
  buf->st_gid = 0;
  buf->st_rdev = 0;
  buf->st_size = info.fsize;
  memset(&buf->st_atim, 0, sizeof(buf->st_atim));
  memset(&buf->st_mtim, 0, sizeof(buf->st_mtim));
  memset(&buf->st_ctim, 0, sizeof(buf->st_ctim));
  buf->st_blksize = 0;
  buf->st_blocks = 0;
  return 0;
#endif
}

int _fatfs_unlink(const char *path)
{
#ifdef ROM
  errno = EROFS;
  return -1;
#else
  FRESULT res;
  res = f_unlink(path);
  if (res != FR_OK)
    {
      errno = fresult2errno(res);
      return -1;
    }
  return 0;
#endif
}

static ssize_t fatfs_write(struct _file *file, const void *buf, size_t count)
{
#ifdef ROM
  errno = EROFS;
  return -1;
#else
  FRESULT res;
  unsigned bytes_written = 0;
  res = f_write(&file->fil, buf, count, &bytes_written);
  if (res != FR_OK)
    {
      errno = fresult2errno(res);
      return -1;
    }
  return bytes_written;
#endif
}

static struct _file_ops fatfs_ops = {
    .close = fatfs_close,
    .lseek = fatfs_lseek,
    .read = fatfs_read,
    .write = fatfs_write,
};

int _fatfs_open(struct _file *file, const char *filename, int flags, mode_t mode)
{
  FRESULT res;
  _init_fatfs();
  memset(&file->fil, 0, sizeof(file->fil));
  int f_mode = 0;
  if ((flags & (O_CREAT | O_APPEND)) == (O_CREAT | O_APPEND))
    f_mode |= FA_OPEN_APPEND;
  else if ((flags & (O_CREAT | O_TRUNC)) == (O_CREAT | O_TRUNC))
    f_mode |= FA_CREATE_ALWAYS;
  else if ((flags & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL))
    f_mode |= FA_CREATE_NEW;
  else if ((flags & O_CREAT) == O_CREAT)
    f_mode |= FA_OPEN_ALWAYS;
  else
    f_mode |= FA_OPEN_EXISTING;
  if ((flags & O_RDWR) == O_RDWR)
    f_mode |= FA_READ | FA_WRITE;
  else if ((flags & O_WRONLY) == O_WRONLY)
    f_mode |= FA_WRITE;
  else
    f_mode |= FA_READ;
  res = f_open(&file->fil, filename, f_mode);
  if (res != FR_OK)
    {
      if (res == FR_NO_FILESYSTEM)
        {
          _fatal_error("No FAT32 filesystem on SD card.");
        }
      errno = fresult2errno(res);
      return -1;
    }
  file->ops = &fatfs_ops;
  return 0;
}
