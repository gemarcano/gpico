// SPDX-License-Identifier: GPL-2.0-or-later OR LGPL-2.1-or-later
// SPDX-FileCopyrightText: Gabriel Marcano, 2024
/// @file

#ifndef GPICO_IO_DEVICE_H_
#define GPICO_IO_DEVICE_H_

#include <span>
#include <memory>
#include <expected>

namespace gpico
{

struct file_descriptor
{
	virtual ~file_descriptor() = default;

	/** Writes the span of data to the file.
	 *
	 * The actual number of bytes written may be less than the number
	 * requested.
	 *
	 * @param[in] data Span of data to write to the file.
	 *
	 * @returns The actual number of bytes written, or -1 on an error
	 *  (errno should also be set to something sensible).
	 */
	virtual int write(std::span<const std::byte> data) = 0;

	/** Reads data from the file.
	 *
	 * The actual number of bytes read may be less than the size of the
	 * span, due to EOF or the device not having any more data at that
	 * time.
	 *
	 * @param[in] data Span of data to receive data from the file.
	 *
	 * @returns The actual number of bytes read, or -1 on an error
	 *  (errno should also be set to something sensible).
	 */
	virtual int read(std::span<std::byte> buffer) = 0;
};

/**Abstract class representing IO devices.
 */
class io_device
{
public:
	virtual ~io_device() = default;
	/** Open the device.
	 *
	 * This is meant to initialize the hardware or anything else required
	 * to enable use of write and read. This may block until the device is
	 * ready.
	 *
	 * @returns True on success, false otherwise.
	 */
	virtual bool probe() = 0;

	/** Close the device.
	 *
	 * This frees up any resources taken during open.
	 *
	 * @returns True on success, false otherwise.
	 */
	virtual bool unload() = 0;

	virtual std::expected<file_descriptor*, int> open(const char *path) = 0;
};

}

#endif//GPICO_IO_DEVICE_H_
