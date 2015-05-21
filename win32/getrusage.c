/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
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

static void usage_to_timeval(FILETIME *ft, struct timeval *tv)
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
