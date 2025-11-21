/*
 * Copyright (C) 2025 Chris January
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nextp8.h"
#include "font.h"

static int cursor_x = 0, cursor_y = 0;
static int bg_index = _DARK_BLUE;

void _clear_screen(int colour_index)
{
    memset((char *)_BACK_BUFFER_BASE, (colour_index & 15) | ((colour_index & 15) << 4), _FRAME_BUFFER_SIZE);
    cursor_x = 0;
    cursor_y = 0;
    bg_index = colour_index;
}

static void display_char(int left_margin, int *x, int *y, char c)
{
  if (*y < _SCREEN_HEIGHT)
    {
      if (c >= 32 && c < 127)
        {
          for (int yy=0;yy<5;++yy)
            {
              for (int xx=0;xx<2;++xx)
                {
                  if (*y + yy >= 0 && *y + yy < _SCREEN_HEIGHT &&
                      *x + xx >= 0 && *x + xx + 1 < _SCREEN_WIDTH)
                    {
                      *(uint8_t *)(_BACK_BUFFER_BASE + (*y + yy) * 64 + *x / 2 + xx) |= __font[c-32][yy][xx];
                    }
                }
              }
        }
      *x += _FONT_CHAR_WIDTH;
      if (*x >= _SCREEN_WIDTH || c == '\n')
        {
          *x = left_margin;
          *y += _FONT_LINE_HEIGHT;
        }
    }
}

void _display_string(int x, int y, const char *message)
{
  int left_margin = x;
  while (*message)
    {
      display_char(left_margin, &x, &y, *message++);
    }
}

#if 0
static void scroll(int amount)
{
  memmove((char *)_BACK_BUFFER_BASE, (char *)_BACK_BUFFER_BASE + amount*64, _FRAME_BUFFER_SIZE - amount*64);
  memset((char *)_BACK_BUFFER_BASE + (_SCREEN_HEIGHT-amount)*64, (bg_index & 15) | ((bg_index & 15) << 4), amount*64);
}

void _print_char(char c)
{
  if (cursor_y + _FONT_CHAR_HEIGHT > _SCREEN_HEIGHT)
    {
      scroll(cursor_y + _FONT_CHAR_HEIGHT - _SCREEN_HEIGHT);
      cursor_y = _SCREEN_HEIGHT - FONT_CHAR_HEIGHT;
    }
  if (c == '\n')
    {
      cursor_x = 0;
      cursor_y += _FONT_LINE_HEIGHT;
    } 
  else
    {
      display_char(0, &cursor_x, &cursor_y, c);
    }
}

void _print_string(const char *message)
{
  while (*message)
    _print_char(*message++);
}
#endif

void _display_string_centered(int centre_x, int centre_y, const char *message)
{
  int ll=0, maxll=0, lc=0;
  const char *p = message;
  for (;;)
   {
     if (*p == '\0' || *p == '\n') {
        if (ll > maxll) maxll = ll;
        lc++;
        if (*p == '\0')
          break;
        ll = 0;
     } else {
        ll++;
     }
     p++;
   }
  int w = maxll * _FONT_CHAR_WIDTH;
  if (w > _SCREEN_WIDTH)
    w = _SCREEN_WIDTH;
  int h = lc * _FONT_LINE_HEIGHT;
  int x = centre_x - w / 2;
  int y = centre_y - h / 2;
  _display_string(x, y, message);
}

void _flip(void)
{
  int vfront = *(volatile uint8_t *) _VFRONT;
  int vback = 1 - vfront;
  int vfrontreq = vback;
  *(volatile uint8_t *) _VFRONTREQ = vfrontreq;
  while (*(volatile uint8_t *) _VFRONT != vfrontreq) {
    // wait for flip to complete
  }
}

void _copy_front_to_back(void)
{
  int vfront = *(volatile uint8_t *) _VFRONT;
  memcpy((uint8_t *)_BACK_BUFFER_BASE, (uint8_t *)_FRAME_BUFFER1_BASE + vfront * _FRAME_BUFFER_SIZE, _FRAME_BUFFER_SIZE);
}
