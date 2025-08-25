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

#include <stdio.h>
#include <unistd.h>

#include "ff.h"
#include "diskio.h"
#include "nextp8.h"
#include "sdblockdevice.h"

static struct _sd_block_device sd[2];
static bool sd_initialized[2];

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
                break;
            }
            if (pdrv == 0)
                _fatal_error(error_message);
            else
                _recoverable_error(error_message);
        }
    }
    return RES_OK;
}

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
    int res = _sd_read(&sd[pdrv], buff,
                       (sd_size_t)sector * sector_size,
                       (sd_size_t)count * sector_size);
    if (res != SD_BLOCK_DEVICE_OK) {
#ifndef ROM
        fprintf(stderr, "_sd_read: error %d\n", res);
#endif
        return RES_ERROR;
    }
    return RES_OK;
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
    return 0;
}