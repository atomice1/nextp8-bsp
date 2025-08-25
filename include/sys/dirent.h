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

#ifndef DIRENT_H
#define DIRENT_H

#include <stdint.h>

struct _DIR;
typedef struct _DIR DIR;

struct dirent {
	uint32_t	d_size;
	uint16_t	d_date;
	uint16_t	d_time;
	uint8_t	    d_attrib;
	char    	d_name[12 + 1];
};

#endif
