#include <errno.h>
#include <malloc.h>
#include <string.h>
#include "io.h"
#include "nextp8.h"

extern char *last_error;

static int stdio_initialized;

static int stdio_isatty(struct _file *file)
{
  return 1;
}

static ssize_t stdio_read (struct _file *file, void *buf, size_t count)
{
  if (file->stdio == 0)
    {
      return 0;
    }
  else
    {
      errno = EINVAL;
      return -1;
    }
}

#ifndef ROM
static void append_to_last_error(const void *buf, size_t count)
{
  size_t oldlen = (last_error != NULL) ? strlen(last_error) : 0;
  last_error = realloc(last_error, oldlen + count + 1);
  char last = (oldlen > 0) ? last_error[oldlen - 1] : '\n';
  const char *src = buf;
  char *dst = last_error + oldlen;
  for (int i=0;i<count;++i) {
    char c = *src++;
    if (last == '\n')
       dst = last_error;
    *dst++ = c;
    last = c;
  }
  *dst++ = '\0';
}
#endif

static ssize_t stdio_write(struct _file *file, const void *buf, size_t count)
{
  if (file->stdio == 0)
    {
      errno = EINVAL;
      return -1;
    }
  else
    {
#ifndef ROM
      if (file->stdio == 2)
        append_to_last_error(buf, count);
#endif
      const char *src = (const char *) buf;
      char *dst = (char *)((file->stdio == 2) ? _TUBE_STDERR : _TUBE_STDOUT);
      for (size_t i=0;i<count;++i)
        {
          char c = *src++;
          *dst = c;
        }
      return count;
    }
}

static struct _file_ops stdio_ops = {
    .isatty = stdio_isatty,
    .read = stdio_read,
    .write = stdio_write,
};

void _init_stdio(void)
{
  if (stdio_initialized)
    return;
  stdio_initialized = 1;
  _files[0].ops = &stdio_ops;
  _files[0].stdio = 0;
  _files[1].ops = &stdio_ops;
  _files[1].stdio = 1;
  _files[2].ops = &stdio_ops;
  _files[2].stdio = 2;
}
