// SPDX-License-Identifier: GPL-2.0-or-later OR LGPL-2.1-or-later
// SPDX-FileCopyrightText: Gabriel Marcano, 2025
/// @file

namespace gpico
{

/** Returns whether the USB CDC device is connected to a host or not.
 */
bool usb_cdc_connected();

/** Initializes a FreeRTOS task to process USB events in Core 1 at a priority
 * level 2 higher than IDLE.
 */
void initialize_usb_task();

}
