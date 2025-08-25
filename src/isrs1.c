#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "nextp8.h"

static char hexchars[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f'
};

void _show_interrupt(const char *name, uintptr_t pc)
{
  char buf[128];
  strcpy(buf, name);
  strcat(buf, "\nPC=$");
  char *p = buf + strlen(buf);
  for (int i=28;i>=0;i-=4)
    *p++ = hexchars[(pc >> i) & 0xf];
  *p = '\0';
  _fatal_error(buf);
}
