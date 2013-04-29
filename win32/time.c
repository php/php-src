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

 /**
  *
  * 04-Feb-2001
  *   - Added patch by "Vanhanen, Reijo" <Reijo.Vanhanen@helsoft.fi>
  *     Improves accuracy of msec
  */

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

int getfilesystemtime(struct timeval *time_Info) 
{
    FILETIME ft;
    __int64 ff;
    ULARGE_INTEGER convFromft;

    GetSystemTimeAsFileTime(&ft);   /* 100 ns blocks since 01-Jan-1641 */
                                    /* resolution seems to be 0.01 sec */
    /*
     * Do not cast a pointer to a FILETIME structure to either a 
     * ULARGE_INTEGER* or __int64* value because it can cause alignment faults on 64-bit Windows.
     * via  http://technet.microsoft.com/en-us/library/ms724284(v=vs.85).aspx
     */
    convFromft.HighPart = ft.dwHighDateTime;
    convFromft.LowPart = ft.dwLowDateTime;
    ff = convFromft.QuadPart;

    time_Info->tv_sec = (int)(ff/(__int64)10000000-(__int64)11644473600);
    time_Info->tv_usec = (int)(ff % 10000000)/10;
    return 0;
}

 

PHPAPI int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info)
{
	__int64 timer;
	LARGE_INTEGER li;
	BOOL b;
	double dt;
	TSRMLS_FETCH();

	/* Get the time, if they want it */
	if (time_Info != NULL) {
		if (PW32G(starttime).tv_sec == 0) {
            b = QueryPerformanceFrequency(&li);
            if (!b) {
                PW32G(starttime).tv_sec = -1;
            }
            else {
                PW32G(freq) = li.QuadPart;
                b = QueryPerformanceCounter(&li);
                if (!b) {
                    PW32G(starttime).tv_sec = -1;
                }
                else {
                    getfilesystemtime(&PW32G(starttime));
                    timer = li.QuadPart;
                    dt = (double)timer/PW32G(freq);
                    PW32G(starttime).tv_usec -= (int)((dt-(int)dt)*1000000);
                    if (PW32G(starttime).tv_usec < 0) {
                        PW32G(starttime).tv_usec += 1000000;
                        --PW32G(starttime).tv_sec;
                    }
                    PW32G(starttime).tv_sec -= (int)dt;
                }
            }
        }
        if (PW32G(starttime).tv_sec > 0) {
            b = QueryPerformanceCounter(&li);
            if (!b) {
                PW32G(starttime).tv_sec = -1;
            }
            else {
                timer = li.QuadPart;
                if (timer < PW32G(lasttime)) {
                    getfilesystemtime(time_Info);
                    dt = (double)timer/PW32G(freq);
                    PW32G(starttime) = *time_Info;
                    PW32G(starttime).tv_usec -= (int)((dt-(int)dt)*1000000);
                    if (PW32G(starttime).tv_usec < 0) {
                        PW32G(starttime).tv_usec += 1000000;
                        --PW32G(starttime).tv_sec;
                    }
                    PW32G(starttime).tv_sec -= (int)dt;
                }
                else {
                    PW32G(lasttime) = timer;
                    dt = (double)timer/PW32G(freq);
                    time_Info->tv_sec = PW32G(starttime).tv_sec + (int)dt;
                    time_Info->tv_usec = PW32G(starttime).tv_usec + (int)((dt-(int)dt)*1000000);
                    if (time_Info->tv_usec >= 1000000) {
                        time_Info->tv_usec -= 1000000;
                        ++time_Info->tv_sec;
                    }
                }
            }
        }
        if (PW32G(starttime).tv_sec < 0) {
            getfilesystemtime(time_Info);
        }

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

