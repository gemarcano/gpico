// SPDX-License-Identifier: GPL-2.0-or-later OR LGPL-2.1-or-later
// SPDX-FileCopyrightText: Gabriel Marcano, 2024
/// @file

#ifndef GPICO_CDC_H_
#define GPICO_CDC_H_

#include <gpico/io_device.h>

#include <atomic>
#include <span>
#include <expected>

namespace gpico
{

/** IO device representing a TinyUSB CDC serial connection.
 */
class cdc_device : public io_device
{
public:
	bool probe() override;

	bool unload() override;

	std::expected<file_descriptor*, int> open(const char *path) override;

	/** Sends data over the CDC device.
	 *
	 * @param[in] data Data to send.
	 *
	 * @returns The number of bytes sent, or -1 if an error occurred. On an
	 *  error errno is set.
	 */
	int write(std::span<const std::byte> data);

	/** Receives data over the CDC device.
	 *
	 * @param[in] data Buffer for incoming data.
	 *
	 * @returns The number of bytes received, or -1 if an error occurred. On an
	 *  error errno is set.
	 */

	int read(std::span<std::byte> buffer);
};

/** File descriptor representing a CDC device.
 */
class cdc_file_descriptor : public file_descriptor
{
public:
	/** Constructor.
	 *
	 * @param[in,out] device CDC device to associate with this descriptor.
	 */
	cdc_file_descriptor(cdc_device& device);

	int write(std::span<const std::byte> data) override;

	int read(std::span<std::byte> buffer) override;
private:
	cdc_device& device;
};

extern cdc_device cdc;
extern cdc_file_descriptor cdc_descriptor;

}

#endif//GPICO_CDC_H_
