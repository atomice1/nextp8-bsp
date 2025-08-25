/* 
 * Copyright (C) 2025 Chris January
 * Copyright (c) 2006-2013 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SD_BLOCK_DEVICE_H
#define SD_BLOCK_DEVICE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/** Enum of standard error codes
 *
 *  @enum sd_error
 */
enum sd_error {
    SD_BLOCK_DEVICE_OK    = 0,     /*!< no error */
    SD_BLOCK_DEVICE_ERROR = -4001, /*!< device specific error */
};

#define SD_BLOCK_DEVICE_ERROR_WOULD_BLOCK        -5001  /*!< operation would block */
#define SD_BLOCK_DEVICE_ERROR_UNSUPPORTED        -5002  /*!< unsupported operation */
#define SD_BLOCK_DEVICE_ERROR_PARAMETER          -5003  /*!< invalid parameter */
#define SD_BLOCK_DEVICE_ERROR_NO_INIT            -5004  /*!< uninitialized */
#define SD_BLOCK_DEVICE_ERROR_NO_DEVICE          -5005  /*!< device is missing or not connected */
#define SD_BLOCK_DEVICE_ERROR_WRITE_PROTECTED    -5006  /*!< write protected */
#define SD_BLOCK_DEVICE_ERROR_UNUSABLE           -5007  /*!< unusable card */
#define SD_BLOCK_DEVICE_ERROR_NO_RESPONSE        -5008  /*!< No response from device */
#define SD_BLOCK_DEVICE_ERROR_CRC                -5009  /*!< CRC error */
#define SD_BLOCK_DEVICE_ERROR_ERASE              -5010  /*!< Erase error: reset/sequence */
#define SD_BLOCK_DEVICE_ERROR_WRITE              -5011  /*!< SPI Write error: !SPI_DATA_ACCEPTED */

/** Type representing the address of a specific block
 */
typedef uint64_t sd_addr_t;

/** Type representing a quantity of 8-bit bytes
 */
typedef uint64_t sd_size_t;

#ifndef SD_INIT_FREQUENCY
#define SD_INIT_FREQUENCY 100000
#endif
#ifndef SD_TRX_FREQUENCY
#define SD_TRX_FREQUENCY  1000000
#endif
#ifndef SD_CRC_ENABLED
#define SD_CRC_ENABLED 0
#endif

struct _sd_block_device {
    uint8_t _card_type;

    size_t _sectors;

    /* SPI functions */
    int _spi_index;
    uint32_t _init_sck;             /**< Initial SPI frequency */
    uint32_t _transfer_sck;         /**< SPI frequency during data transfer/after initialization */

    //PlatformMutex _mutex;
    uint32_t _erase_size;
    bool _is_initialized;
    bool _dbg;
    uint32_t _init_ref_count;

#if SD_CRC_ENABLED
    bool _crc_on;
#endif
};

/** Creates an sd_block_device on a SPI bus specified by pins (using static pin-map)
 *
 *  @param spi_index  Index of SD card SPI (0 or 1).
 *  @param hz         Clock speed of the SPI bus (defaults to 1MHz)
 *  @param crc_on     Enable cyclic redundancy check (defaults to disabled)
 */
void _sd_construct(struct _sd_block_device *,
                   int spi_index,
                   uint64_t hz /* = SD_TRX_FREQUENCY */,
                   bool crc_on /* = SD_CRC_ENABLED */);

void _sd_destruct(struct _sd_block_device *device);

/** Initialize a block device
 *
 *  @return         SD_ERROR_OK(0) - success
 *                  SD_ERROR_DEVICE_ERROR - device driver transaction failed
 *                  SD_BLOCK_DEVICE_ERROR_NO_DEVICE - device (SD card) is missing or not connected
 *                  SD_BLOCK_DEVICE_ERROR_UNUSABLE - unusable card
 *                  SD_BLOCK_DEVICE_ERROR_CRC - crc error
 */
int _sd_init(struct _sd_block_device *device);

/** Deinitialize a block device
 *
 *  @return         SD_ERROR_OK(0) - success
 */
int _sd_deinit(struct _sd_block_device *device);

/** Read blocks from a block device
 *
 *  @param buffer   Buffer to write blocks to
 *  @param addr     Address of block to begin reading from
 *  @param size     Size to read in bytes, must be a multiple of read block size
 *  @return         SD_ERROR_OK(0) - success
 *                  SD_BLOCK_DEVICE_ERROR_NO_DEVICE - device (SD card) is missing or not connected
 *                  SD_BLOCK_DEVICE_ERROR_CRC - crc error
 *                  SD_BLOCK_DEVICE_ERROR_PARAMETER - invalid parameter
 *                  SD_BLOCK_DEVICE_ERROR_NO_RESPONSE - no response from device
 *                  SD_BLOCK_DEVICE_ERROR_UNSUPPORTED - unsupported command
 */
int _sd_read(struct _sd_block_device *device, void *buffer, sd_addr_t addr, sd_size_t size);

/** Program blocks to a block device
 *
 *  @note The blocks must be erased prior to programming
 *
 *  @param buffer   Buffer of data to write to blocks
 *  @param addr     Address of block to begin writing to
 *  @param size     Size to write in bytes. Must be a multiple of program block size
 *  @return         SD_ERROR_OK(0) - success
 *                  SD_BLOCK_DEVICE_ERROR_NO_DEVICE - device (SD card) is missing or not connected
 *                  SD_BLOCK_DEVICE_ERROR_CRC - crc error
 *                  SD_BLOCK_DEVICE_ERROR_PARAMETER - invalid parameter
 *                  SD_BLOCK_DEVICE_ERROR_UNSUPPORTED - unsupported command
 *                  SD_BLOCK_DEVICE_ERROR_NO_INIT - device is not initialized
 *                  SD_BLOCK_DEVICE_ERROR_WRITE - SPI write error
 *                  SD_BLOCK_DEVICE_ERROR_ERASE - erase error
 */
int _sd_program(struct _sd_block_device *device, const void *buffer, sd_addr_t addr, sd_size_t size);

/** Mark blocks as no longer in use
 *
 *  This function provides a hint to the underlying block device that a region of blocks
 *  is no longer in use and may be erased without side effects. Erase must still be called
 *  before programming, but trimming allows flash-translation-layers to schedule erases when
 *  the device is not busy.
 *
 *  @param addr     Address of block to mark as unused
 *  @param size     Size to mark as unused in bytes, must be a multiple of erase block size
 *  @return         SD_ERROR_OK(0) - success
 *                  SD_BLOCK_DEVICE_ERROR_NO_DEVICE - device (SD card) is missing or not connected
 *                  SD_BLOCK_DEVICE_ERROR_CRC - crc error
 *                  SD_BLOCK_DEVICE_ERROR_PARAMETER - invalid parameter
 *                  SD_BLOCK_DEVICE_ERROR_UNSUPPORTED - unsupported command
 *                  SD_BLOCK_DEVICE_ERROR_NO_INIT - device is not initialized
 *                  SD_BLOCK_DEVICE_ERROR_ERASE - erase error
 */
int _sd_trim(struct _sd_block_device *device, sd_addr_t addr, sd_size_t size);

/** Get the size of a readable block
 *
 *  @return         Size of a readable block in bytes
 */
sd_size_t _sd_get_read_size(struct _sd_block_device *device);

/** Get the size of a programmable block
 *
 *  @return         Size of a programmable block in bytes
 *  @note Must be a multiple of the read size
 */
sd_size_t _sd_get_program_size(struct _sd_block_device *device);

/** Get the total size of the underlying device
 *
 *  @return         Size of the underlying device in bytes
 */
sd_size_t _sd_size(struct _sd_block_device *device);

/** Enable or disable debugging
 *
 *  @param dbg        State of debugging
 */
void _sd_debug(struct _sd_block_device *device, bool dbg);

/** Set the transfer frequency
 *
 *  @param freq     Transfer frequency
 *  @note Max frequency supported is 25MHZ
 */
int _sd_frequency(struct _sd_block_device *device, uint64_t freq);

/** Get the BlockDevice class type.
 *
 *  @return         A string representation of the BlockDevice class type.
 */
const char *_sd_get_type(struct _sd_block_device *device);

#endif  /* SD_BLOCK_DEVICE_H */
