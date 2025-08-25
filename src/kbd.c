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

#include <stdbool.h>
#include <stdint.h>
#include "nextp8.h"

void _wait_for_any_key(void)
{
    bool key_pressed = false;
    do {
        for (int i=0;i<32;++i) {
            if (*(uint8_t *)(_KEYBOARD_MATRIX + i) != 0)
                key_pressed = true;
        }
        if (*(uint8_t *)_JOYSTICK0 != 0)
            key_pressed = true;
        if (*(uint8_t *)_JOYSTICK1 != 0)
            key_pressed = true;
    } while (!key_pressed);
}
