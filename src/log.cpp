// SPDX-License-Identifier: GPL-2.0-or-later OR LGPL-2.1-or-later
// SPDX-FileCopyrightText: Gabriel Marcano, 2024
/// @file

#include <gpico/log.h>
#include <gpico/syslog.h>

namespace gpico
{
	safe_syslog<syslog<1024*4>> sys_log;
}
