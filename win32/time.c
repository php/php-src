
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

#include "time.h"
#include "unistd.h"
#include "signal.h"
#include <winbase.h>
#include <mmsystem.h>
#include <errno.h>

int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info)
{
	_int64 mstimer, freq;
	/* Get the time, if they want it */
	if (time_Info != NULL) {
		time_Info->tv_sec = time(NULL);
		/* get ticks-per-second of the performance counter
		   Note the necessary typecast to a LARGE_INTEGER structure 
		 */
		if (!QueryPerformanceFrequency((LARGE_INTEGER *) & freq)) {
			time_Info->tv_usec = 0;
		} else {
			QueryPerformanceCounter((LARGE_INTEGER *) & mstimer);
			mstimer = (__int64) (mstimer * .8);
			time_Info->tv_usec = (long) (mstimer % 0x0FFFFFFF);
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


/* this usleep isnt exactly accurate but should do ok */
void usleep(unsigned int useconds)
{
	__int64 mstimer, freq;
	long now, then;
	if (QueryPerformanceFrequency((LARGE_INTEGER *) & freq)) {
		QueryPerformanceCounter((LARGE_INTEGER *) & mstimer);
		now = (long) (((__int64) (mstimer * .8)) % 0x0FFFFFFF);
		then = now + useconds;
		while (now < then) {
			QueryPerformanceCounter((LARGE_INTEGER *) & mstimer);
			now = (long) (((__int64) (mstimer * .8)) % 0x0FFFFFFF);
		}
	} else {
		/*workaround for systems without performance counter
		   this is actualy a millisecond sleep */
		Sleep((int) (useconds / 1000));
	}
}


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

int setitimer(int which, const struct itimerval *value, struct itimerval *ovalue)
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
