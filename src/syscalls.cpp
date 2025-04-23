// SPDX-License-Identifier: GPL-2.0-or-later OR LGPL-2.1-or-later
// SPDX-FileCopyrightText: Gabriel Marcano, 2024
/// @file

#include <gpico/cdc_device.h>

#include <tusb.h>

#include <cerrno>
#include <span>
#include <cstdint>
#include <atomic>
#include <array>
#include <cstdio>
#include <tuple>
#include <memory>

#include <errno.h>
#undef errno
extern int errno;

#include <pico/rand.h>

static gpico::file_descriptor* files[10] = {
	&gpico::cdc_descriptor,
	&gpico::cdc_descriptor,
	&gpico::cdc_descriptor,
};

// Just hang if we call a pure virtual function, the default implementation
// relies on too much stuff
extern "C" void __cxa_pure_virtual()
{
	for(;;)
	{
		__asm__ __volatile__ ("bkpt #0");
	}
}

// Also just hang if we get to the __verbose_terminate_handler
namespace __gnu_cxx
{
	void __verbose_terminate_handler()
	{
		for (;;)
		{
			__asm__ __volatile__ ("bkpt #0");
		}
	}
}

// Allow registration of up to N devices
static std::optional<std::tuple<const char*, gpico::io_device&>> devices[10];

void register_device(size_t index, std::tuple<const char*, gpico::io_device&> device)
{
	devices[index] = device;
}

void unregister_device(size_t index)
{
	devices[index].reset();
}

// Apparently, if I don't declare this function as used, LTO gets rid of it...
extern "C" int _write(int fd, char *buf, int count) __attribute__ ((used));
extern "C" int _write(int fd, char *buf, int count)
{
	if (!files[fd])
	{
		errno = EBADF;
		return -1;
	}

	gpico::file_descriptor& desc = *files[fd];
	return desc.write(std::span<const std::byte>(reinterpret_cast<std::byte*>(buf), count));
}

extern "C" int _read(int fd, char *buf, int count) __attribute__ ((used));
extern "C" int _read(int fd, char *buf, int count)
{
	if (!files[fd])
	{
		errno = ENOENT;
		return -1;
	}
	gpico::file_descriptor& desc = *files[fd];
	return desc.read(std::span<std::byte>(reinterpret_cast<std::byte*>(buf), count));
}

extern "C" int _open(const char *name, int flags, int mode) __attribute__ ((used));
extern "C" int _open(const char *name, int /*flags*/, int /*mode*/)
{
	std::expected<gpico::file_descriptor*, int> desc;
	for (size_t i = 0; i < 10 && !desc; ++i)
	{
		if (devices[i] && (strcmp(std::get<const char*>(*devices[i]), name) == 0))
		{
			auto& device = std::get<1>(*devices[i]);
			desc = device.open(name);
		}
	}

	if (desc)
	{
		for (size_t i = 3; i < 10; ++i)
		{
			if (!files[i])
			{
				files[i] = *desc;
				return i;
			}
		}
	}
	errno = ENOENT;
	return -1;
}

extern "C" int _close(int fd) __attribute__ ((used));
extern "C" int _close(int fd)
{
	if (files[fd])
	{
		files[fd] = nullptr;
		return 0;
	}
	errno = EBADF;
	return -1;
}

extern "C" int getentropy(void *buffer, size_t length) __attribute__ ((used));
extern "C" int getentropy(void *buffer, size_t length)
{
	uint32_t random = get_rand_32();
	size_t i;
	for (i = 0; i < length - 3; i += 4)
	{
		memcpy(reinterpret_cast<unsigned char*>(buffer)+i, &random, 4);
		random = get_rand_32();
	}
	memcpy(reinterpret_cast<unsigned char*>(buffer) + i - 4, &random, length % 4);
	return 0;
}
