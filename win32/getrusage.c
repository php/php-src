/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Kalle Sommer Nielsen <kalle@php.net>                        |
   +----------------------------------------------------------------------+
 */

#include <php.h>
#include <psapi.h>
#include "getrusage.h"

/*
 * Parts of this file is based on code from the OpenVSwitch project, that
 * is released under the Apache 2.0 license and is copyright 2014 Nicira, Inc.
 * and have been modified to work with PHP.
 */

static zend_always_inline void usage_to_timeval(FILETIME *ft, struct timeval *tv)
{
	ULARGE_INTEGER time;

	time.LowPart = ft->dwLowDateTime;
	time.HighPart = ft->dwHighDateTime;

	tv->tv_sec = (zend_long) (time.QuadPart / 10000000);
	tv->tv_usec = (zend_long) ((time.QuadPart % 10000000) / 10);
}

PHPAPI int getrusage(int who, struct rusage *usage)
{
	FILETIME ctime, etime, stime, utime;

	memset(usage, 0, sizeof(struct rusage));

	if (who == RUSAGE_SELF) {
		PROCESS_MEMORY_COUNTERS pmc;
		HANDLE proc = GetCurrentProcess();

		if (!GetProcessTimes(proc, &ctime, &etime, &stime, &utime)) {
			return -1;
		} else if(!GetProcessMemoryInfo(proc, &pmc, sizeof(pmc))) {
			return -1;
		}

		usage_to_timeval(&stime, &usage->ru_stime);
		usage_to_timeval(&utime, &usage->ru_utime);

		usage->ru_majflt = pmc.PageFaultCount;
		usage->ru_maxrss = pmc.PeakWorkingSetSize / 1024;

		return 0;
	} else if (who == RUSAGE_THREAD) {
		if (!GetThreadTimes(GetCurrentThread(), &ctime, &etime, &stime, &utime)) {
			return -1;
		}

		usage_to_timeval(&stime, &usage->ru_stime);
		usage_to_timeval(&utime, &usage->ru_utime);

		return 0;
	} else {
		return -1;
	}
}
