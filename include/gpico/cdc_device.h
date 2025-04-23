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

	int write(std::span<const std::byte> data);

	int read(std::span<std::byte> buffer);

	/** Updates the connected status of the TinyUSB CDC device.
	 *
	 * This can be called from multiple threads, but it really needs to be
	 * called from the same thread/task as the main tud_task call.
	 */
	void update();


private:
	/// Atomic flag indicating whether the USB CDC device is connected.
	std::atomic_bool connected_ = false;
};

class cdc_file_descriptor : public file_descriptor
{
public:
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
