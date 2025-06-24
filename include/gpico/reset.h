// SPDX-License-Identifier: GPL-2.0-or-later OR LGPL-2.1-or-later
// SPDX-FileCopyrightText: Gabriel Marcano, 2025
/// @file

#ifndef GPICO_RESET_H_
#define GPICO_RESET_H_

namespace gpico
{

/** Launches a FreeRTOS task to reset the device into programming mode.
 *
 * The internal task always runs in core 0. This reset task stops FreeRTOS,
 * resets core1, and asks bootrom to reset.
 */
void bootsel_reset();

/** Launcher a FreeRTOS task to reset the device into flash.
 *
 * The internal task always runs in core 0. This reset task relies on the
 * watchdog tasks being initialized, as it kills them to trigger a watchdog
 * reset.
 */
void flash_reset();

}

#endif//GPICO_RESET_H_
