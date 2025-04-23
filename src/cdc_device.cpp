// SPDX-License-Identifier: GPL-2.0-or-later OR LGPL-2.1-or-later
// SPDX-FileCopyrightText: Gabriel Marcano, 2024
/// @file

#include <gpico/cdc_device.h>

#include <tusb.h>

#include <span>

#include <errno.h>
#undef errno
extern int errno;

namespace gpico
{

cdc_file_descriptor cdc_descriptor(gpico::cdc);

bool cdc_device::probe()
{
	while(!connected_);
	return true;
}

bool cdc_device::unload()
{
	return true;
}

void cdc_device::update()
{
	connected_ = tud_cdc_connected();
}

std::expected<file_descriptor*, int> cdc_device::open(const char * /*path*/)
{
	return &cdc_descriptor;
}

cdc_file_descriptor::cdc_file_descriptor(cdc_device& device)
:device(device)
{}

int cdc_file_descriptor::write(std::span<const std::byte> data)
{
	return device.write(data);
}

int cdc_file_descriptor::read(std::span<std::byte> buffer)
{
	return device.read(buffer);
}

int cdc_device::write(std::span<const std::byte> data)
{
	if (!connected_)
	{
		errno = ENXIO;
		return -1;
	}

	if (tud_cdc_write_available() == 0)
	{
		tud_task();
		tud_cdc_write_flush();
	}

	uint32_t result = tud_cdc_write(
		reinterpret_cast<const unsigned char*>(data.data()), data.size());
	tud_cdc_write_flush();
	return static_cast<int>(result);
}

int cdc_device::read(std::span<std::byte> buffer)
{
	if (!connected_)
	{
		errno = ENXIO;
		return -1;
	}

	while(!tud_cdc_available())
	{
		// FIXME some kind of yield?
	}

	// There is data available
	size_t read_;
	for (read_ = 0; read_ < buffer.size() && tud_cdc_available();)
	{
		// read and echo back
		read_ += tud_cdc_read(
			reinterpret_cast<unsigned char*>(buffer.data()) + read_,
			buffer.size() - read_);
	}
	return static_cast<int>(read_);
}

cdc_device cdc;

}

