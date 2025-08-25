/* 
 * Copyright (C) 2025 Chris January
 * Copyright (c) 2006-2019 ARM Limited
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

#ifndef SPI_H
#define SPI_H

#define SPI_FILL_CHAR         (0xFF)

/** Configure the data transmission format.
 *
 *  @param bits Number of bits per SPI frame (4 - 32, target dependent).
 *  @param mode Clock polarity and phase mode (0 - 3).
 *
 * @code
 * mode | POL PHA
 * -----+--------
 *   0  |  0   0
 *   1  |  0   1
 *   2  |  1   0
 *   3  |  1   1
 * @endcode
 */
void _spi_format(int bits, int mode /* = 0 */);

/** Set the SPI bus clock frequency.
 *
 *  @param hz Clock frequency in Hz (default = 1MHz).
 */
void _spi_frequency(int hz /* = 1000000 */);

/** Write to the SPI Slave and return the response.
 *
 *  @param value Data to be sent to the SPI slave.
 *
 *  @return Response from the SPI slave.
 */
int _spi_write(int value);

/** Write to the SPI Slave and obtain the response.
 *
 *  The total number of bytes sent and received will be the maximum of
 *  tx_length and rx_length. The bytes written will be padded with the
 *  value 0xff.
 *
 *  @param tx_buffer Pointer to the byte-array of data to write to the device.
 *  @param tx_length Number of bytes to write, may be zero.
 *  @param rx_buffer Pointer to the byte-array of data to read from the device.
 *  @param rx_length Number of bytes to read, may be zero.
 *  @return
 *      The number of bytes written and read from the device. This is
 *      maximum of tx_length and rx_length.
 */
int _spi_block_write(const char *tx_buffer, int tx_length, char *rx_buffer, int rx_length);

/** Acquire exclusive access to this SPI bus.
 */
void _spi_lock(void);

/** Release exclusive access to this SPI bus.
 */
void _spi_unlock(void);

/** Assert the Slave Select line, acquiring exclusive access to this SPI bus.
 *
 * If use_gpio_ssel was not passed to the constructor, this only acquires
 * exclusive access; it cannot assert the Slave Select line.
 */
void _spi_select(int index);

/** Deassert the Slave Select line, releasing exclusive access to this SPI bus.
 */
void _spi_deselect();

/** Set default write data.
     * SPI requires the master to send some data during a read operation.
     * Different devices may require different default byte values.
     * For example: A SD Card requires default bytes to be 0xFF.
     *
     * @param data Default character to be transmitted during a read operation.
     */
void _spi_set_default_write_value(char data);

#endif