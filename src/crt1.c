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

#ifdef ROM
#define STACK_SIZE (8 << 10) /* 8KB */
#else
#define STACK_SIZE (256 << 10)  /* 256KB */
#endif

extern const int __interrupt_vector[];
extern void __reset (void);

extern const char __data_load[] __attribute__ ((aligned (4)));
extern char __data_start[] __attribute__ ((aligned (4)));
extern char __bss_start[] __attribute__ ((aligned (4)));
extern char __end[] __attribute__ ((aligned (4)));
char *__initial_stack;
void *__heap_limit;
struct _config_data *_config_data;
#ifndef ROM
struct _loader_data *_loader_data;
#endif
extern void software_init_hook (void) __attribute__ ((weak));
extern void hardware_init_hook (void) __attribute__ ((weak));
extern void _init (void);
extern void _fini (void);

extern int main (int, char **, char **);

/* This is called from a tiny assembly stub.  */
void __start1 (char *initial_stack)
{
  unsigned ix;
  
  _set_postcode(5);

  if (hardware_init_hook)
    hardware_init_hook ();
  
  _set_postcode(6);

  /* Initialize memory */
  if (__data_load != __data_start)
    memcpy (__data_start, __data_load, __bss_start - __data_start);
  memset (__bss_start, 0, __end - __bss_start);

  __initial_stack = initial_stack;
  __heap_limit = initial_stack - STACK_SIZE;

  _set_postcode(7);
  
  if (software_init_hook)
    software_init_hook ();

  _set_postcode(8);

  _init ();

  _set_postcode(9);

  /* I'm not sure how useful it is to have a fini_section in an
     embedded system.  */
  atexit (_fini);
  
  _set_postcode(10);

  ix = main (0, NULL, NULL);

  _set_postcode(62);

  exit (ix);

  _set_postcode(63);
  
  while (1)
    _warm_reset();
}

/* A default hardware init hook.  */
/* Important: this function must not use any global variables in .data. */
void __attribute__ ((weak)) hardware_init_hook (void)
{
#if __mc68020 || __mc68030 || __mc68040
  /* Set the VBR. */
  __asm__ __volatile__ ("movec.l %0,%/vbr" :: "r" (__interrupt_vector));
#endif

  /* Should we drop into user mode here? */
}

void __attribute__ ((weak)) software_init_hook (void)
{
  /* Check for hardware compatibility */
  uint32_t hw_version = *(uint32_t *)_HW_VERSION_HI;
  if (_EXTRACT_API(hw_version) != HW_API_VERSION) {
    _clear_screen(_RED); _flip();
    _fatal_error("Incompatible hardware version");
  }
#ifdef ROM
  _config_data = (struct _config_data *) _CONFIG_BASE_ROM;
#else
  _config_data = (struct _config_data *) __initial_stack;
  _loader_data = (struct _loader_data *) (__initial_stack + 256);
#endif
  unsigned ps2_mode = _config_data->ps2_mode;
  uint16_t params = (ps2_mode << 0);
  *(volatile uint16_t *)_PARAMS = params;

  /* Set boot screen to dark blue. */
  _clear_screen(_DARK_BLUE);
  _flip();

#ifndef ROM
  if (_loader_data->magic != LOADER_MAGIC) {
    _clear_screen(_RED); _flip();
    _fatal_error("Invalid loader magic");
  }
  if (_EXTRACT_API(_loader_data->loader_version) != LOADER_API_VERSION) {
    _clear_screen(_RED); _flip();
    _fatal_error("Incompatible loader version");
  }
#endif
}
