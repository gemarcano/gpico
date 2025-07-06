// SPDX-License-Identifier: GPL-2.0-or-later OR LGPL-2.1-or-later
// SPDX-FileCopyrightText: Gabriel Marcano, 2024
/// @file

#include <gpico/watchdog.h>

#include <hardware/structs/mpu.h>
#include <hardware/watchdog.h>
#include <pico/multicore.h>
#include <pico/bootrom.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <utility>

namespace gpico
{
	[[noreturn]] static void reset_task(void* kind)
	{
		bool bootsel = static_cast<bool>(kind);
		// To do a graceful bootsel reset, stop everything FREERTOS is doing.
		// This should only every run on core 0
		vTaskSuspendAll();
        taskENTER_CRITICAL();
		if (bootsel)
		{
			mpu_hw->ctrl &= ~1; // disable MPU, just in case it's blocking ROM
			multicore_reset_core1();
        	reset_usb_boot(0,0);
			std::unreachable();
		}
		else
		{
			watchdog_enable(0, true);
		}
        taskEXIT_CRITICAL();
        xTaskResumeAll();
		for(;;);
		std::unreachable();
	}

	[[noreturn]] void bootsel_reset()
	{
		xTaskCreateAffinitySet(
			reset_task,
			"gpico_reset",
			configMINIMAL_STACK_SIZE,
			reinterpret_cast<void*>(true),
			tskIDLE_PRIORITY+3,
			(1 << 0),
			nullptr);
		for(;;);
	}

	[[noreturn]] void flash_reset()
	{
		xTaskCreateAffinitySet(
			reset_task,
			"gpico_reset",
			configMINIMAL_STACK_SIZE,
			reinterpret_cast<void*>(false),
			tskIDLE_PRIORITY+3,
			(1 << 0),
			nullptr);
        for(;;);
	}
}
