/*****************************************************************************
 *                                                                           *
 * DH_TIME.C                                                                 *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *
 * Modified by Shane Caraveo for use with PHP
 *
 *****************************************************************************/

/* Include stuff ************************************************************ */

#include <config.w32.h>

#include "time.h"
#include "unistd.h"
#include "signal.h"
#include <windows.h>
#include <winbase.h>
#include <mmsystem.h>
#include <errno.h>
#include "php_win32_globals.h"
#include "zend_hrtime.h"

static zend_always_inline void getfilesystemtime(struct timeval *tv)
{/*{{{*/
	FILETIME ft;
	unsigned __int64 ff = 0;
	ULARGE_INTEGER fft;

	GetSystemTimePreciseAsFileTime(&ft);

	/*
	 * Do not cast a pointer to a FILETIME structure to either a
	 * ULARGE_INTEGER* or __int64* value because it can cause alignment faults on 64-bit Windows.
	 * via  http://technet.microsoft.com/en-us/library/ms724284(v=vs.85).aspx
	 */
	fft.HighPart = ft.dwHighDateTime;
	fft.LowPart = ft.dwLowDateTime;
	ff = fft.QuadPart;

	ff /= 10Ui64; /* convert to microseconds */
	ff -= 11644473600000000Ui64; /* convert to unix epoch */

	tv->tv_sec = (long)(ff / 1000000Ui64);
	tv->tv_usec = (long)(ff % 1000000Ui64);
}/*}}}*/

PHPAPI int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info)
{/*{{{*/
	/* Get the time, if they want it */
	if (time_Info != NULL) {
		getfilesystemtime(time_Info);
	}
	/* Get the timezone, if they want it */
	if (timezone_Info != NULL) {
		_tzset();
		timezone_Info->tz_minuteswest = _timezone;
		timezone_Info->tz_dsttime = _daylight;
	}
	/* And return */
	return 0;
}/*}}}*/

static DWORD wait_timer_alertable(HANDLE timer)
{/*{{{*/
	DWORD result;

#ifdef ZTS
	/* Threads that PHP does not manage have no executor globals, and their APCs cannot run PHP code */
	if (!tsrm_is_managed_thread()) {
		return WaitForSingleObjectEx(timer, INFINITE, TRUE);
	}
#endif

	/* APCs can run PHP code that bails out. Close the timer before propagating the bailout. */
	zend_try {
		result = WaitForSingleObjectEx(timer, INFINITE, TRUE);
	} zend_catch {
		CloseHandle(timer);
		zend_bailout();
	} zend_end_try();

	return result;
}/*}}}*/

PHPAPI int usleep(unsigned int useconds)
{/*{{{*/
	HANDLE timer;
	LARGE_INTEGER due;
	DWORD result;

	due.QuadPart = -(10 * (__int64)useconds);

	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	SetWaitableTimer(timer, &due, 0, NULL, NULL, 0);
	result = wait_timer_alertable(timer);
	CloseHandle(timer);
	if (result == WAIT_IO_COMPLETION) {
		errno = EINTR;
		return -1;
	}
	return 0;
}/*}}}*/

PHPAPI int nanosleep( const struct timespec * rqtp, struct timespec * rmtp )
{/*{{{*/
	if (rqtp->tv_nsec > 999999999) {
		/* The time interval specified 1,000,000 or more microseconds. */
		errno = EINVAL;
		return -1;
	}
	/* Run already queued APCs first, so that their runtime doesn't count as time slept */
	if (SleepEx(0, TRUE) == WAIT_IO_COMPLETION) {
		if (rmtp != NULL) {
			*rmtp = *rqtp;
		}
		errno = EINTR;
		return -1;
	}
	zend_hrtime_t start = zend_hrtime();
	int result = usleep(rqtp->tv_sec * 1000000 + rqtp->tv_nsec / 1000);
	if (result == -1 && errno == EINTR && rmtp != NULL) {
		zend_hrtime_t elapsed = zend_hrtime() - start;
		time_t seconds = rqtp->tv_sec - (time_t) (elapsed / ZEND_NANO_IN_SEC);
		long nanoseconds = rqtp->tv_nsec - (long) (elapsed % ZEND_NANO_IN_SEC);
		if (nanoseconds < 0) {
			seconds--;
			nanoseconds += (long) ZEND_NANO_IN_SEC;
		}
		if (seconds < 0) {
			seconds = 0;
			nanoseconds = 0;
		}
		rmtp->tv_sec = seconds;
		rmtp->tv_nsec = nanoseconds;
	}
	return result;
}/*}}}*/
