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

typedef VOID (WINAPI *MyGetSystemTimeAsFileTime)(LPFILETIME lpSystemTimeAsFileTime);

static MyGetSystemTimeAsFileTime timefunc = NULL;

#ifdef PHP_EXPORTS
static zend_always_inline MyGetSystemTimeAsFileTime get_time_func(void)
{/*{{{*/
	MyGetSystemTimeAsFileTime timefunc = NULL;
	HMODULE hMod = GetModuleHandle("kernel32.dll");

	if (hMod) {
		/* Max possible resolution <1us, win8/server2012 */
		timefunc = (MyGetSystemTimeAsFileTime)GetProcAddress(hMod, "GetSystemTimePreciseAsFileTime");
	}

	if(!timefunc) {
		/* 100ns blocks since 01-Jan-1641 */
		timefunc = (MyGetSystemTimeAsFileTime) GetSystemTimeAsFileTime;
	}

	return timefunc;
}/*}}}*/

void php_win32_init_gettimeofday(void)
{/*{{{*/
	timefunc = get_time_func();
}/*}}}*/
#endif

static zend_always_inline int getfilesystemtime(struct timeval *tv)
{/*{{{*/
	FILETIME ft;
	unsigned __int64 ff = 0;
	ULARGE_INTEGER fft;

	timefunc(&ft);

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

	return 0;
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

PHPAPI int usleep(unsigned int useconds)
{/*{{{*/
	HANDLE timer;
	LARGE_INTEGER due;

	due.QuadPart = -(10 * (__int64)useconds);

	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	SetWaitableTimer(timer, &due, 0, NULL, NULL, 0);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
	return 0;
}/*}}}*/

PHPAPI int nanosleep( const struct timespec * rqtp, struct timespec * rmtp )
{/*{{{*/
	if (rqtp->tv_nsec > 999999999) {
		/* The time interval specified 1,000,000 or more microseconds. */
		errno = EINVAL;
		return -1;
	}
	return usleep( rqtp->tv_sec * 1000000 + rqtp->tv_nsec / 1000  );
}/*}}}*/

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
