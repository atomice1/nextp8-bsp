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

#include <stddef.h>
#include <stdint.h>
#include "timestamp_macros.h"
#include "version_macros.h"

#define API_VERSION   0
#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define PATCH_VERSION 0

uint32_t _bsp_version = _MAKE_VERSION(API_VERSION, MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);
uint32_t _bsp_timestamp = _TIMESTAMP;
