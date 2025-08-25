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

#ifndef NEXTP8_H
#define NEXTP8_H

#define _MEMIO_BASE         0x800000
#define _SDSPI_WRITE_ENABLE (_MEMIO_BASE + 0x0)
#define _SDSPI_DIVIDER      (_MEMIO_BASE + 0x2)
#define _SDSPI_DATA_IN      (_MEMIO_BASE + 0x4)
#define _SDSPI_DATA_OUT     (_MEMIO_BASE + 0x6)
#define _SDSPI_READY        (_MEMIO_BASE + 0x8)
#define _SDSPI_CHIP_SELECT  (_MEMIO_BASE + 0xa)
#define _POST_CODE          (_MEMIO_BASE + 0xc)
#define _VFRONT             (_MEMIO_BASE + 0xe)
#define _VFRONTREQ          (_MEMIO_BASE + 0xe)
#define _PARAMS             (_MEMIO_BASE + 0x12)
#define _UTIMER_1MHZ_HI     (_MEMIO_BASE + 0x2E)
#define _UTIMER_1MHZ_LO     (_MEMIO_BASE + 0x30)
#define _UTIMER_1KHZ_HI     (_MEMIO_BASE + 0x32)
#define _UTIMER_1KHZ_LO     (_MEMIO_BASE + 0x34)
#define _DA_CONTROL         (_MEMIO_BASE + 0x36)
#define _DA_PERIOD          (_MEMIO_BASE + 0x38)
#define _KEYBOARD_MATRIX    (_MEMIO_BASE + 0x40)
#define _JOYSTICK0          (_MEMIO_BASE + 0x60)
#define _JOYSTICK1          (_MEMIO_BASE + 0x61)
#define _DEBUG_REG_HI       (_MEMIO_BASE + 0x62)
#define _DEBUG_REG_LO       (_MEMIO_BASE + 0x64)

#define _DA_MEMORY_BASE     0xc0c000
#define _DA_MEMORY_SIZE     16384
#define _DA_CLKS_PER_SECOND 11000000

#define _BACK_BUFFER_BASE   0xc00000
#define _FRONT_BUFFER_BASE  0xc02000
#define _FRAME_BUFFER1_BASE 0xc04000
#define _FRAME_BUFFER2_BASE 0xc06000
#define _PALETTE_BASE       0xc08000
#define _FRAME_BUFFER_SIZE  8192 // 0x2000
#define _PALETTE_SIZE       16
#define _SCREEN_WIDTH       128
#define _SCREEN_HEIGHT      128

#define _TUBE_STDOUT        0xfffffe
#define _TUBE_STDERR        0xffffff

#define _CONFIG_BASE        0x7c000

enum {
    _BLACK = 0,
    _DARK_BLUE,
    _DARK_PURPLE,
    _DARK_GREEN,
    _BROWN,
    _DARK_GREY,
    _LIGHT_GREY,
    _WHITE,
    _RED,
    _ORANGE,
    _YELLOW,
    _GREEN,
    _BLUE,
    _LAVENDER,
    _PINK,
    _LIGHT_PEACH
};

extern void _set_postcode(int postcode);
extern void _clear_screen(int colour_index);
extern void _copy_front_to_back(void);
extern void _display_string(int x, int y, const char *s);
extern void _display_string_centered(int x, int y, const char *s);
extern void _display_error(const char *message);
extern void _flip(void);
extern void __attribute__ ((noreturn)) _halt(void);
extern void __attribute__ ((noreturn)) _warm_reset(void);
#ifdef ROM
extern void __attribute__ ((noreturn)) _fatal_error(const char *message);
extern void _recoverable_error(const char *message);
#else
extern void __attribute__ ((noreturn)) _fatal_error(const char *format, ...);
extern void _recoverable_error(const char *format, ...);
#endif
extern void _wait_for_any_key(void);

#endif
