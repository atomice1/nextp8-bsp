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

#ifndef ROM
#include <stdio.h>
#endif
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "ff.h"
#include "diskio.h"
#include "nextp8.h"
#include "sdblockdevice.h"

#ifdef ROM
extern void _rom_fatal_error(const char *fn, int res);
#endif

static struct _sd_block_device sd[2];
static bool sd_initialized[2];

#ifndef ROM
#define CACHE_NUM_WAYS      4
#define CACHE_NUM_SETS      8
#define CACHE_NUM_BLOCKS    (CACHE_NUM_WAYS * CACHE_NUM_SETS)
#define CACHE_SET_MASK      (CACHE_NUM_SETS - 1)

struct cache_entry {
    bool valid;
    BYTE pdrv;
    LBA_t sector;
    BYTE *data;
    unsigned int lru_counter;
};

static struct cache_entry cache[CACHE_NUM_SETS][CACHE_NUM_WAYS];
static unsigned int global_lru_counter = 0;
static bool cache_initialized = false;

static bool cache_intialize(sd_size_t sector_size)
{
    bool alloc_success = true;
    for (int set = 0; set < CACHE_NUM_SETS && alloc_success; set++) {
        for (int way = 0; way < CACHE_NUM_WAYS && alloc_success; way++) {
            cache[set][way].data = malloc(sector_size);
            if (cache[set][way].data == NULL) {
                alloc_success = false;
            } else {
                cache[set][way].valid = false;
                cache[set][way].lru_counter = 0;
            }
        }
    }

    if (!alloc_success) {
        for (int set = 0; set < CACHE_NUM_SETS; set++) {
            for (int way = 0; way < CACHE_NUM_WAYS; way++) {
                if (cache[set][way].data != NULL) {
                    free(cache[set][way].data);
                    cache[set][way].data = NULL;
                }
            }
        }
        return false;
    }

    cache_initialized = true;
    return true;
}
#endif

DSTATUS disk_status (
  BYTE pdrv     /* [IN] Physical drive number */
)
{
    if (!sd_initialized[pdrv])
        return STA_NOINIT;
    return 0;
}

DSTATUS disk_initialize (
  BYTE pdrv           /* [IN] Physical drive number */
)
{
    if (pdrv < 0 || pdrv > 1)
        return STA_NODISK;
    if (!sd_initialized[pdrv]) {
        _sd_construct(&sd[pdrv], pdrv, 1000000, false);
        int res = _sd_init(&sd[pdrv]);
        if (res == SD_BLOCK_DEVICE_OK) {
            sd_initialized[pdrv] = true;
#ifndef ROM
            if (!cache_initialized) {
                sd_size_t sector_size = _sd_get_read_size(&sd[pdrv]);
                if (!cache_intialize(sector_size))
                    return STA_NOINIT;
            }
#endif
        } else {
            const char *error_message = NULL;
            switch (res) {
            case SD_BLOCK_DEVICE_ERROR_NO_DEVICE:
                error_message = "SD card is missing or not connected.";
                break;
            case SD_BLOCK_DEVICE_ERROR_UNUSABLE:
                error_message =  "SD Card is unsable.";
                break;
            case SD_BLOCK_DEVICE_ERROR_NO_RESPONSE:
                error_message =  "No response from SD card.";
                break;
            default:
                error_message = "unknown SD card error";
#ifdef ROM
                _rom_fatal_error("disk_initialize", res);
#endif
                break;
            }
            if (pdrv == 0)
                _fatal_error(error_message);
            else
                _recoverable_error(error_message);
            return STA_NOINIT;
        }
    }
    return RES_OK;
}

#ifndef ROM
static DRESULT disk_read_sector (
  BYTE pdrv,     /* [IN] Physical drive number */
  BYTE* buff,    /* [OUT] Pointer to the read data buffer (sector_size bytes) */
  LBA_t sector   /* [IN] Sector number */
)
{
    sd_size_t sector_size = _sd_get_read_size(&sd[pdrv]);

    if (!cache_initialized)
        return RES_ERROR;

    unsigned int set = sector & CACHE_SET_MASK;

    for (int way = 0; way < CACHE_NUM_WAYS; way++) {
        struct cache_entry *entry = &cache[set][way];
        if (entry->valid && entry->pdrv == pdrv && entry->sector == sector) {
            memcpy(buff, entry->data, sector_size);
            entry->lru_counter = ++global_lru_counter;
            return RES_OK;
        }
    }

    int lru_way = 0;
    unsigned int min_lru = cache[set][0].lru_counter;
    for (int way = 1; way < CACHE_NUM_WAYS; way++) {
        if (!cache[set][way].valid) {
            lru_way = way;
            break;
        }
        if (cache[set][way].lru_counter < min_lru) {
            min_lru = cache[set][way].lru_counter;
            lru_way = way;
        }
    }

    struct cache_entry *entry = &cache[set][lru_way];
    int res = _sd_read(&sd[pdrv], entry->data,
                       (sd_size_t)sector * sector_size,
                       (sd_size_t)sector_size);
    if (res != SD_BLOCK_DEVICE_OK) {
        fprintf(stderr, "_sd_read: error %d\n", res);
        return RES_ERROR;
    }

    entry->valid = true;
    entry->pdrv = pdrv;
    entry->sector = sector;
    entry->lru_counter = ++global_lru_counter;

    memcpy(buff, entry->data, sector_size);

    return RES_OK;
}
#endif

DRESULT disk_read (
  BYTE pdrv,     /* [IN] Physical drive number */
  BYTE* buff,    /* [OUT] Pointer to the read data buffer */
  LBA_t sector,  /* [IN] Start sector number */
  UINT count     /* [IN] Number of sectros to read */
)
{
    if (pdrv < 0 || pdrv > 1 || !sd_initialized[pdrv])
        return RES_NOTRDY;

    sd_size_t sector_size = _sd_get_read_size(&sd[pdrv]);

#ifdef ROM
    int res = _sd_read(&sd[pdrv], buff,
                           (sd_size_t)sector * sector_size,
                           (sd_size_t)count * sector_size);
    if (res != SD_BLOCK_DEVICE_OK) {
        _rom_fatal_error("_sd_read", res);
        return RES_ERROR;
    }
    return RES_OK;
#else
    if (count > 1) {
        int res = _sd_read(&sd[pdrv], buff,
                           (sd_size_t)sector * sector_size,
                           (sd_size_t)count * sector_size);
        if (res != SD_BLOCK_DEVICE_OK) {
            fprintf(stderr, "_sd_read: error %d\n", res);
            return RES_ERROR;
        }
        return RES_OK;
    }

    return disk_read_sector(pdrv, buff, sector);
#endif
}

DRESULT disk_write (
  BYTE pdrv,        /* [IN] Physical drive number */
  const BYTE* buff, /* [IN] Pointer to the data to be written */
  LBA_t sector,     /* [IN] Sector number to write from */
  UINT count        /* [IN] Number of sectors to write */
)
{
    if (pdrv < 0 || pdrv > 1 || !sd_initialized[pdrv])
        return RES_NOTRDY;
    sd_size_t sector_size = _sd_get_read_size(&sd[pdrv]);
    int res = _sd_program(&sd[pdrv], buff,
                          (sd_size_t)sector * sector_size,
                          (sd_size_t)count * sector_size);
    if (res != SD_BLOCK_DEVICE_OK) {
#ifndef ROM
        fprintf(stderr, "_sd_program: error %d\n", res);
#endif
        return RES_ERROR;
    }
    return RES_OK;
}

DRESULT disk_ioctl (
  BYTE pdrv,     /* [IN] Drive number */
  BYTE cmd,      /* [IN] Control command code */
  void* buff     /* [I/O] Parameter and data buffer */
)
{
    if (pdrv < 0 || pdrv > 1 || !sd_initialized[pdrv])
        return RES_NOTRDY;
    switch (cmd) {
    case CTRL_SYNC:
        return RES_PARERR;
    case GET_SECTOR_COUNT:
        *(LBA_t *)buff = _sd_size(&sd[pdrv]) / _sd_get_read_size(&sd[pdrv]);
        return RES_OK;
    case GET_SECTOR_SIZE:
        *(WORD *)buff = _sd_get_read_size(&sd[pdrv]);
        return RES_OK;
    case GET_BLOCK_SIZE:
        *(DWORD *)buff = _sd_get_program_size(&sd[pdrv]);
        return RES_OK;
    case CTRL_TRIM: {
        sd_size_t sector_size = _sd_get_read_size(&sd[pdrv]);
        _sd_trim(&sd[pdrv],
                 (sd_size_t)((LBA_t *)buff)[0] * sector_size,
                 (sd_size_t)((LBA_t *)buff)[1] * sector_size);
        return RES_OK;
    }
    default:
        return RES_PARERR;
    }
}

DWORD get_fattime (void)
{
#ifdef ROM
    return 0;
#else
    unsigned date, month, year, hours, minutes, seconds, wday;
    int res = _read_rtc(&date, &month, &year, &hours, &minutes, &seconds, &wday);
    if (res != 0) {
        date = 1;
        month = 1;
        year = 1980;
        hours = 0;
        minutes = 0;
        seconds = 0;
    }
    return ((DWORD)(year - 1980) << 25) |
            ((DWORD)month << 21) |
            ((DWORD)date << 16) |
            ((DWORD)hours << 11) |
            ((DWORD)minutes << 5) |
            ((DWORD)(seconds / 2));
#endif
}