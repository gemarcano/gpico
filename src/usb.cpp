// SPDX-License-Identifier: GPL-2.0-or-later OR LGPL-2.1-or-later
// SPDX-FileCopyrightText: Gabriel Marcano, 2025
/// @file

#include <tusb_config.h>
#include <tusb.h>
#include <bsp/board_api.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <gpico/usb.h>
#include <gpico/cdc_device.h>

#include <atomic>

static std::atomic_bool cdc_connected = false;

namespace gpico
{

bool usb_cdc_connected()
{
	return cdc_connected;
}

// FreeRTOS task to handle USB tasks
static void usb_device_task(void*)
{
	tusb_init();
	for(;;)
	{
		tud_task();
		// FIXME does the pico-sdk tinyusb functionality not honor FreeRTOS
		// blocking?
		vTaskDelay(1);
		// tud_cdc_connected() must be called in the same task as tud_task, as
		// an internal data structure is shared without locking between both
		// functions. See https://github.com/hathach/tinyusb/issues/1472
		// As a workaround, use an atomic variable to get the result of this
		// function, and read from it elsewhere
		cdc_connected = tud_cdc_connected();
	}
}

void initialize_usb_task()
{
	// Only install this in CPU 1
	xTaskCreateAffinitySet(
		usb_device_task,
		"gpico_usb",
		configMINIMAL_STACK_SIZE,
		nullptr,
		tskIDLE_PRIORITY+2,
		(1 << 1),
		nullptr);
}

}
