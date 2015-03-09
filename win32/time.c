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

/* $Id$ */

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

static zend_always_inline MyGetSystemTimeAsFileTime get_time_func(void)
{
	MyGetSystemTimeAsFileTime timefunc = NULL;
	HMODULE hMod = LoadLibrary("kernel32.dll");

	if (hMod) {
		/* Max possible resolution <1us, win8/server2012 */
		timefunc = (MyGetSystemTimeAsFileTime)GetProcAddress(hMod, "GetSystemTimePreciseAsFileTime");

		if(!timefunc) {
			/* 100ns blocks since 01-Jan-1641 */
			timefunc = (MyGetSystemTimeAsFileTime)GetProcAddress(hMod, "GetSystemTimeAsFileTime");
		}
	}

	return timefunc;
}

static zend_always_inline int getfilesystemtime(struct timeval *tv)
{
	FILETIME ft;
	unsigned __int64 ff = 0;
	ULARGE_INTEGER fft;

	if (!timefunc) {
		timefunc = get_time_func();
	}
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
}

PHPAPI int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info)
{
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
}

PHPAPI int usleep(unsigned int useconds)
{
	HANDLE timer;
	LARGE_INTEGER due;

	due.QuadPart = -(10 * (__int64)useconds);

	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	SetWaitableTimer(timer, &due, 0, NULL, NULL, 0);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
	return 0;
}

PHPAPI int nanosleep( const struct timespec * rqtp, struct timespec * rmtp )
{
	if (rqtp->tv_nsec > 999999999) {
		/* The time interval specified 1,000,000 or more microseconds. */
		errno = EINVAL;
		return -1;
	}
	return usleep( rqtp->tv_sec * 1000000 + rqtp->tv_nsec / 1000  );
}

#if 0 /* looks pretty ropey in here */
#ifdef HAVE_SETITIMER


#ifndef THREAD_SAFE
unsigned int proftimer, virttimer, realtimer;
extern LPMSG phpmsg;
#endif

struct timer_msg {
	int signal;
	unsigned int threadid;
};


LPTIMECALLBACK setitimer_timeout(UINT uTimerID, UINT info, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	struct timer_msg *msg = (struct timer_msg *) info;

	if (msg) {
		raise((int) msg->signal);
		PostThreadMessage(msg->threadid,
						  WM_NOTIFY, msg->signal, 0);
		free(msg);
	}
	return 0;
}

PHPAPI int setitimer(int which, const struct itimerval *value, struct itimerval *ovalue)
{
	int timeout = value->it_value.tv_sec * 1000 + value->it_value.tv_usec;
	int repeat = TIME_ONESHOT;

	/*make sure the message queue is initialized */
	PeekMessage(phpmsg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	if (timeout > 0) {
		struct timer_msg *msg = malloc(sizeof(struct timer_msg));
		msg->threadid = GetCurrentThreadId();
		if (!ovalue) {
			repeat = TIME_PERIODIC;
		}
		switch (which) {
			case ITIMER_REAL:
				msg->signal = SIGALRM;
				realtimer = timeSetEvent(timeout, 100, (LPTIMECALLBACK) setitimer_timeout, (UINT) msg, repeat);
				break;
			case ITIMER_VIRT:
				msg->signal = SIGVTALRM;
				virttimer = timeSetEvent(timeout, 100, (LPTIMECALLBACK) setitimer_timeout, (UINT) msg, repeat);
				break;
			case ITIMER_PROF:
				msg->signal = SIGPROF;
				proftimer = timeSetEvent(timeout, 100, (LPTIMECALLBACK) setitimer_timeout, (UINT) msg, repeat);
				break;
			default:
				errno = EINVAL;
				return -1;
				break;
		}
	} else {
		switch (which) {
			case ITIMER_REAL:
				timeKillEvent(realtimer);
				break;
			case ITIMER_VIRT:
				timeKillEvent(virttimer);
				break;
			case ITIMER_PROF:
				timeKillEvent(proftimer);
				break;
			default:
				errno = EINVAL;
				return -1;
				break;
		}
	}


	return 0;
}

#endif
#endif

