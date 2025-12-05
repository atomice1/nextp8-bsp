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

#include <stddef.h>
#include <stdint.h>

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
#define _BUILD_TIMESTAMP_HI (_MEMIO_BASE + 0x14)
#define _BUILD_TIMESTAMP_LO (_MEMIO_BASE + 0x16)
#define _HW_VERSION_HI      (_MEMIO_BASE + 0x18)
#define _HW_VERSION_LO      (_MEMIO_BASE + 0x1a)
#define _I2C_DATA           (_MEMIO_BASE + 0x21)
#define _I2C_CTRL           (_MEMIO_BASE + 0x23)
#define _UART_CTRL          (_MEMIO_BASE + 0x24)
#define _UART_DATA          (_MEMIO_BASE + 0x25)
#define _UART_BAUD_DIV      (_MEMIO_BASE + 0x26)
#define _ESP_CTRL           (_MEMIO_BASE + 0x28)
#define _ESP_DATA           (_MEMIO_BASE + 0x29)
#define _ESP_BAUD_DIV       (_MEMIO_BASE + 0x2a)
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

#define _P8AUDIO_BASE          0x800100
#define _P8AUDIO_VERSION       (_P8AUDIO_BASE + 0x0)
#define _P8AUDIO_CTRL          (_P8AUDIO_BASE + 0x2)
#define _P8AUDIO_SFX_BASE_HI   (_P8AUDIO_BASE + 0x4)
#define _P8AUDIO_SFX_BASE_LO   (_P8AUDIO_BASE + 0x6)
#define _P8AUDIO_MUSIC_BASE_HI (_P8AUDIO_BASE + 0x8)
#define _P8AUDIO_MUSIC_BASE_LO (_P8AUDIO_BASE + 0xa)
#define _P8AUDIO_HWFX40        (_P8AUDIO_BASE + 0x0c)
#define _P8AUDIO_HWFX41        (_P8AUDIO_BASE + 0x0d)
#define _P8AUDIO_HWFX42        (_P8AUDIO_BASE + 0x0e)
#define _P8AUDIO_HWFX43        (_P8AUDIO_BASE + 0x0f)
#define _P8AUDIO_NOTE_ATK_TIME (_P8AUDIO_BASE + 0x10)
#define _P8AUDIO_NOTE_REL_TIME (_P8AUDIO_BASE + 0x12)
#define _P8AUDIO_SFX_CMD       (_P8AUDIO_BASE + 0x14)
#define _P8AUDIO_SFX_LEN       (_P8AUDIO_BASE + 0x16)
#define _P8AUDIO_MUSIC_CMD     (_P8AUDIO_BASE + 0x18)
#define _P8AUDIO_MUSIC_FADE    (_P8AUDIO_BASE + 0x1a)
#define _P8AUDIO_STAT46        (_P8AUDIO_BASE + 0x1c)
#define _P8AUDIO_STAT47        (_P8AUDIO_BASE + 0x1e)
#define _P8AUDIO_STAT48        (_P8AUDIO_BASE + 0x20)
#define _P8AUDIO_STAT49        (_P8AUDIO_BASE + 0x22)
#define _P8AUDIO_STAT50        (_P8AUDIO_BASE + 0x24)
#define _P8AUDIO_STAT51        (_P8AUDIO_BASE + 0x26)
#define _P8AUDIO_STAT52        (_P8AUDIO_BASE + 0x28)
#define _P8AUDIO_STAT53        (_P8AUDIO_BASE + 0x2a)
#define _P8AUDIO_STAT54        (_P8AUDIO_BASE + 0x2c)
#define _P8AUDIO_STAT55        (_P8AUDIO_BASE + 0x2e)
#define _P8AUDIO_STAT56        (_P8AUDIO_BASE + 0x30)

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
#define _FONT_CHAR_WIDTH    4
#define _FONT_CHAR_HEIGHT   5
#define _FONT_LINE_HEIGHT   (_FONT_CHAR_HEIGHT + 1)

#define _TUBE_STDOUT        0xfffffe
#define _TUBE_STDERR        0xffffff

#define _CONFIG_BASE_ROM    0x7c000
#ifdef ROM
#define _CONFIG_BASE        _CONFIG_BASE_ROM
#endif

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

struct _loader_data {
    uint32_t magic;
    uint32_t loader_version;
    uint32_t loader_timestamp;
    uintptr_t entry_point;
};

#define LOADER_MAGIC 0x12345432

struct _config_data {
    uint8_t video_timing;          // +0 video timing mode (0..7)
    uint8_t scandoubler;           // +1 scandoubler (0=off, 1=on)
    uint8_t frequency;             // +2 frequency (0=50Hz, 1=60Hz)
    uint8_t ps2_mode;              // +3 PS/2 mode (0=keyboard, 1=mouse)
    uint8_t scanline_weight;       // +4 scanline weight (0=off, 1=75%, 2=50%, 3=25%)
    uint8_t internal_speaker;      // +5 internal speaker (0=disabled, 1=enabled)
    uint8_t hdmi_sound;            // +6 HDMI sound (0=disabled, 1=enabled)
    uint8_t reserved1[9];          // +7..15 RESERVED
    uint8_t userfile_selected[16]; // +16..31 userfile selection status (0=not selected, 1=selected)
    char core_path[32];            // +32..63 null-terminated path of the core
    uint8_t reserved2[192];        // +64..255 reserved for future use
} __attribute__((packed));

extern uint32_t _bsp_timestamp;
extern uint32_t _bsp_version;
extern struct _config_data *_config_data;
#ifndef ROM
extern struct _loader_data *_loader_data;
#endif

extern void _set_postcode(int postcode);
extern void _clear_screen(int colour_index);
extern void _copy_front_to_back(void);
extern void _format_version(char *out, size_t size, const char *prefix, uint32_t version, uint32_t timestamp);
extern void _display_string(int x, int y, const char *s);
extern void _display_string_centered(int x, int y, const char *s);
extern void _flip(void);
extern uint32_t get_bsp_version(void);
extern void __attribute__ ((noreturn)) _halt(void);
extern void __attribute__ ((noreturn)) _warm_reset(void);
#ifdef ROM
extern void __attribute__ ((noreturn)) _fatal_error(const char *message);
extern void _recoverable_error(const char *message);
#else
extern void __attribute__ ((noreturn)) _fatal_error(const char *format, ...);
extern void _recoverable_error(const char *format, ...);
extern void _show_message(const char *format, ...);
#endif
extern void _uart_write(const char *buf, size_t count);
extern void _wait_for_any_key(void);

#endif
