/* Initialization code for nextp8.
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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "nextp8.h"
#include "version_macros.h"

#define HW_API_VERSION     0
#define LOADER_API_VERSION 0

extern const int __interrupt_vector[];
extern void __reset (void);

extern const char __data_load[] __attribute__ ((aligned (4)));
extern char __data_start[] __attribute__ ((aligned (4)));
extern char __bss_start[] __attribute__ ((aligned (4)));
extern char __end[] __attribute__ ((aligned (4)));
void *__heap_limit;
extern void software_init_hook (void) __attribute__ ((weak));
extern void hardware_init_hook (void) __attribute__ ((weak));
extern void _init (void);
extern void _fini (void);

extern int main (int, char **, char **);

/* This is called from a tiny assembly stub.  */
void __start1 (void *heap_limit)
{
  unsigned ix;
  
  _set_postcode(4);

  if (hardware_init_hook)
    hardware_init_hook ();
  
  _set_postcode(5);

  /* Initialize memory */
  if (__data_load != __data_start)
    memcpy (__data_start, __data_load, __bss_start - __data_start);
  memset (__bss_start, 0, __end - __bss_start);

  __heap_limit = heap_limit;

  _set_postcode(6);
  
  if (software_init_hook)
    software_init_hook ();

  _set_postcode(7);

  _init ();

  _set_postcode(8);

  /* I'm not sure how useful it is to have a fini_section in an
     embedded system.  */
  atexit (_fini);
  
  _set_postcode(9);

  ix = main (0, NULL, NULL);

  _set_postcode(62);

  exit (ix);

  _set_postcode(63);
  
  while (1)
    _warm_reset();
}

/* A default hardware init hook.  */

void __attribute__ ((weak)) hardware_init_hook (void)
{
#if __mc68020 || __mc68030 || __mc68040
  /* Set the VBR. */
  __asm__ __volatile__ ("movec.l %0,%/vbr" :: "r" (__interrupt_vector));
#endif

  /* Check for hardware compatibility */
  uint32_t hw_version = *(uint32_t *)_HW_VERSION_HI;
  if (_EXTRACT_API(hw_version) != HW_API_VERSION) {
    _clear_screen(_RED); _flip();
    _fatal_error("Incompatible hardware version");
  }

  /* Set boot screen to dark blue. */
  _clear_screen(_DARK_BLUE);
  _flip();

  /* Should we drop into user mode here? */
}

void __attribute__ ((weak)) software_init_hook (void)
{
    unsigned ps2_mode = *(volatile uint8_t *)(_CONFIG_BASE + 3);
    uint16_t params = (ps2_mode << 0);
    *(volatile uint16_t *)_PARAMS = params;

#ifndef ROM
  struct _loader_data *loader_data = (struct _loader_data *) _LOADER_DATA;
  if (_EXTRACT_API(loader_data->loader_version) != LOADER_API_VERSION) {
    _clear_screen(_RED); _flip();
    _fatal_error("Incompatible loader version");
  }
#endif
}
