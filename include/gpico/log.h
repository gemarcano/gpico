// SPDX-License-Identifier: GPL-2.0-or-later OR LGPL-2.1-or-later
// SPDX-FileCopyrightText: Gabriel Marcano, 2024
/// @file

#ifndef GPICO_LOG_H_
#define GPICO_LOG_H_

#include <gpico/syslog.h>

namespace gpico
{
	extern safe_syslog<syslog<1024*128>> sys_log;
}

#endif//GPICO_LOG_H_
