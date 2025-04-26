// SPDX-License-Identifier: GPL-2.0-or-later OR LGPL-2.1-or-later
// SPDX-FileCopyrightText: Gabriel Marcano, 2025
/// @file

#ifndef GPICO_FLASH_H_
#define GPICO_FLASH_H_

#include <hardware/spi.h>
#include <hardware/gpio.h>

#include <lfs.h>

#include <cstdint>
#include <array>
#include <span>
#include <expected>

namespace gpico
{

class flash
{
public:

	/** Initialize flash pins.
	 *
	 * The pins must agree with the spi instance in use (e.g. SPI1 with pins
	 * 10, 11, 12, and 13 for CLK, MOSI, MISO, and CS, respectively).
	 */
	flash(spi_inst_t *spi, uint8_t clk_pin, uint8_t mosi_pin, uint8_t miso_pin, uint8_t cs_pin)
	:spi(spi), clk_pin(clk_pin), mosi_pin(mosi_pin), miso_pin(miso_pin), cs_pin(cs_pin)
	{
		spi_init(spi, 10'000'000);
		gpio_set_function(clk_pin, GPIO_FUNC_SPI);
		gpio_set_function(mosi_pin, GPIO_FUNC_SPI);
		gpio_set_function(miso_pin, GPIO_FUNC_SPI);
		gpio_init(cs_pin);
		gpio_put(cs_pin, 1);
		gpio_set_dir(cs_pin, GPIO_OUT);
		//gpio_set_function(17, GPIO_FUNC_SPI);
	}

	flash(const flash&) = delete;
	flash& operator=(const flash&) = delete;

	void start_command()
	{
		gpio_put(cs_pin, 0);
	}

	void end_command()
	{
		gpio_put(cs_pin, 1);
	}

	void send_data(std::span<const uint8_t> data)
	{
        spi_write_blocking(spi, data.data(), data.size());
	}

	void get_data(std::span<uint8_t> data)
	{
		spi_read_blocking(spi, 0, data.data(), data.size());
	}

	uint16_t read_id()
	{
		uint16_t data = 0;
		std::array<uint8_t, 4> command_{{ 0x90, 0x00, 0x00, 0x00 }};
		start_command();
		send_data(std::span<const uint8_t>(command_));
		get_data(std::span(reinterpret_cast<unsigned char*>(&data), 2));
		end_command();
		return data;
	}

	void read(uint32_t address, std::span<uint8_t> output)
	{
		// FIXME what if output > 256 size?
		std::array<uint8_t, 4> command_{{
			0x03,
			static_cast<uint8_t>(address >> 16),
			static_cast<uint8_t>(address >> 8),
			static_cast<uint8_t>(address)
		}};
		start_command();
		send_data(std::span<const uint8_t>(command_));
		get_data(output);
		end_command();
	}

	uint8_t read_status1()
	{
		uint8_t result;
		start_command();
		send_data(std::span<const uint8_t>({{0x05}}));
		get_data(std::span<uint8_t>(&result, 1));
		end_command();
		return result;
	}

	uint8_t read_status2()
	{
		uint8_t result;
		start_command();
		send_data(std::span<const uint8_t>({{0x35}}));
		get_data(std::span<uint8_t>(&result, 1));
		end_command();
		return result;
	}

	void start_active_status_interrupt()
	{
		start_command();
		send_data(std::span<const uint8_t>({{0x25}}));
		end_command();
		// FIXME something about interrupts, not sure how to deal with them with both cores?
	}

	void end_active_status_interrupt()
	{
		// FIXME something about interrupts, disable?
	}

	void deep_powerdown()
	{
		start_command();
		send_data(std::span<const uint8_t>({{0x09}}));
		end_command();
	}

	void wake()
	{
		start_command();
		send_data(std::span<const uint8_t>({{0xAB}}));
	}

	void write_enable()
	{
		start_command();
		send_data({{ 0x06 }}); // Write Enable
		end_command();
	}

	void write(uint32_t address, std::span<const uint8_t> data)
	{
		write_enable();

		std::array<uint8_t, 4> command_{{
			0x02,
			static_cast<uint8_t>(address >> 16),
			static_cast<uint8_t>(address >> 8),
			static_cast<uint8_t>(address)
		}};

		start_command();
		send_data(std::span<const uint8_t>(command_));
		send_data(data);
		end_command();
	}

	void erase_page(uint32_t address)
	{
		write_enable();

		std::array<uint8_t, 4> command_{{
			0x81,
			static_cast<uint8_t>(address >> 16),
			static_cast<uint8_t>(address >> 8),
			static_cast<uint8_t>(address)
		}};
		start_command();
		send_data(std::span<const uint8_t>(command_));
		end_command();
	}

	void erase_sector(uint32_t address)
	{
		write_enable();

		std::array<uint8_t, 4> command_{{
			0x20,
			static_cast<uint8_t>(address >> 16),
			static_cast<uint8_t>(address >> 8),
			static_cast<uint8_t>(address)
		}};
		start_command();
		send_data(std::span<const uint8_t>(command_));
		end_command();
	}

	void erase_32kblock(uint32_t address)
	{
		write_enable();

		std::array<uint8_t, 4> command_{{
			0x52,
			static_cast<uint8_t>(address >> 16),
			static_cast<uint8_t>(address >> 8),
			static_cast<uint8_t>(address)
		}};
		start_command();
		send_data(std::span<const uint8_t>(command_));
		end_command();
	}

	void erase_64kblock(uint32_t address)
	{
		write_enable();

		std::array<uint8_t, 4> command_{{
			0xD8,
			static_cast<uint8_t>(address >> 16),
			static_cast<uint8_t>(address >> 8),
			static_cast<uint8_t>(address)
		}};
		start_command();
		send_data(std::span<const uint8_t>(command_));
		end_command();
	}

	void erase_chip()
	{
		write_enable();
		start_command();
		send_data({{0xC7}});
		end_command();
	}

	void suspend_operation()
	{
		start_command();
		send_data({{0x75}});
		end_command();
	}

	void resume_operation()
	{
		start_command();
		send_data({{0x7A}});
		end_command();
	}

	void reset()
	{
		start_command();
		send_data({{0x66}});
		end_command();
		start_command();
		send_data({{0x99}});
		end_command();
	}

	void read_blocking(uint32_t address, std::span<uint8_t> output)
	{
		read(address, output);
		while (read_status1() & 0x1);
	}

	void write_blocking(uint32_t address, std::span<const uint8_t> data)
	{
		write(address, data);
		while (read_status1() & 0x1);
	}

	void erase_page_blocking(uint32_t address)
	{
		erase_page(address);
		while (read_status1() & 0x1);
	}

private:
	spi_inst_t *spi;
	uint8_t clk_pin;
	uint8_t mosi_pin;
	uint8_t miso_pin;
	uint8_t cs_pin;
};

class littlefs_file
{
public:
	littlefs_file(lfs_t& lfs_)
	:lfs_(lfs_), open_(false)
	{}

	~littlefs_file()
	{
		if (open_)
		{
			close();
		}
	}

	littlefs_file(const littlefs_file&) = delete;
	littlefs_file& operator=(const littlefs_file&) = delete;

	littlefs_file(littlefs_file&& other)
	:lfs_(other.lfs_), file(other.file), open_(other.open_)
	{
		other.open_ = false;
	}

	int open(const char *path, int flags)
	{
		int result = lfs_file_open(&lfs_, &file, path, flags);
		open_ = result == 0;
		return result;
	}

	int close()
	{
		int result = lfs_file_close(&lfs_, &file);
		open_ = open && result != 0;
		return result;
	}

	int write(std::span<const std::byte> data)
	{
		return lfs_file_write(&lfs_, &file, data.data(), data.size());
	}

	int read(std::span<std::byte> data)
	{
		return lfs_file_read(&lfs_, &file, data.data(), data.size());
	}

private:
	lfs& lfs_;
	lfs_file file;
	bool open_;
};

class littlefs
{
public:
	littlefs(flash& f)
	:f(f), mounted(false)
	{}

	~littlefs()
	{
		if (mounted)
		{
			lfs_unmount(&lfs);
		}
	}

	littlefs(littlefs&& other)
	:f(other.f), mounted(other.mounted)
	{
		other.mounted = false;
	}

	int init()
	{
		int err = lfs_mount(&lfs, &cfg);
		if (err)
		{
			lfs_format(&lfs, &cfg);
			err = lfs_mount(&lfs, &cfg);
		}
		mounted = err == 0;
		return err;
	}

	std::expected<littlefs_file, int> open_file(const char *path, int flags)
	{
		littlefs_file file(lfs);
		int result = file.open(path, flags);
		if (result == 0)
		{
			return file;
		}
		return std::unexpected(result);
	}

private:
	flash& f;
	lfs_t lfs;
	bool mounted;

	static int lfs_read(const lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
	{
		flash &f = reinterpret_cast<littlefs*>(c->context)->f;
		f.read_blocking(block * c->block_size + off, std::span<uint8_t>(reinterpret_cast<uint8_t*>(buffer), size));
		return 0;
	}

	static int lfs_prog(const lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
	{
		flash &f = reinterpret_cast<littlefs*>(c->context)->f;
		f.write_blocking(block * c->block_size + off, std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(buffer), size));
		return 0;
	}

	static int lfs_erase(const lfs_config *c, lfs_block_t block)
	{
		flash &f = reinterpret_cast<littlefs*>(c->context)->f;
		f.erase_page_blocking(block * c->block_size);
		return 0;
	}

	static int lfs_sync(const lfs_config * /*c*/)
	{
		return 0;
	}

	struct lfs_config cfg = {
		.context = reinterpret_cast<void*>(this),
		.read = lfs_read,
		.prog = lfs_prog,
		.erase = lfs_erase,
		.sync = lfs_sync,

		.read_size = 1,
		.prog_size = 1,
		.block_size = 256,
		.block_count = 2048,
		.block_cycles = 500,
		.cache_size = 256,
		.lookahead_size = 64,
		.compact_thresh = 0,

		.read_buffer = nullptr,
		.prog_buffer = nullptr,
		.lookahead_buffer = nullptr,
		.name_max = 0,
		.file_max = 0,
		.attr_max = 0,
		.metadata_max = 0,
		.inline_max = 0
	};
};

}

#endif//GPICO_FLASH_H_
