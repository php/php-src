
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

#include "time_nw.h"
#include "unistd.h"
#include "signal.h"
/*#include <winbase.h>
#include <mmsystem.h>*/
#include <errno.h>
/*
int getfilesystemtime(struct timeval *time_Info) 
{
FILETIME ft;
__int64 ff;

    GetSystemTimeAsFileTime(&ft);   *//* 100 ns blocks since 01-Jan-1641 *//*
                                    *//* resolution seems to be 0.01 sec *//*
    ff = *(__int64*)(&ft);
    time_Info->tv_sec = (int)(ff/(__int64)10000000-(__int64)11644473600);
    time_Info->tv_usec = (int)(ff % 10000000)/10;
    return 0;
}
*/
 

int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info)
{
#ifdef NETWARE
    return 0;
#else
	static struct timeval starttime = {0, 0};
	static __int64 lasttime = 0;
	static __int64 freq = 0;
	__int64 timer;
	LARGE_INTEGER li;
	BOOL b;
	double dt;

	/* Get the time, if they want it */
	if (time_Info != NULL) {
		if (starttime.tv_sec == 0) {
            b = QueryPerformanceFrequency(&li);
            if (!b) {
                starttime.tv_sec = -1;
            }
            else {
                freq = li.QuadPart;
                b = QueryPerformanceCounter(&li);
                if (!b) {
                    starttime.tv_sec = -1;
                }
                else {
                    getfilesystemtime(&starttime);
                    timer = li.QuadPart;
                    dt = (double)timer/freq;
                    starttime.tv_usec -= (int)((dt-(int)dt)*1000000);
                    if (starttime.tv_usec < 0) {
                        starttime.tv_usec += 1000000;
                        --starttime.tv_sec;
                    }
                    starttime.tv_sec -= (int)dt;
                }
            }
        }
        if (starttime.tv_sec > 0) {
            b = QueryPerformanceCounter(&li);
            if (!b) {
                starttime.tv_sec = -1;
            }
            else {
                timer = li.QuadPart;
                if (timer < lasttime) {
                    getfilesystemtime(time_Info);
                    dt = (double)timer/freq;
                    starttime = *time_Info;
                    starttime.tv_usec -= (int)((dt-(int)dt)*1000000);
                    if (starttime.tv_usec < 0) {
                        starttime.tv_usec += 1000000;
                        --starttime.tv_sec;
                    }
                    starttime.tv_sec -= (int)dt;
                }
                else {
                    lasttime = timer;
                    dt = (double)timer/freq;
                    time_Info->tv_sec = starttime.tv_sec + (int)dt;
                    time_Info->tv_usec = starttime.tv_usec + (int)((dt-(int)dt)*1000000);
                    if (time_Info->tv_usec > 1000000) {
                        time_Info->tv_usec -= 1000000;
                        ++time_Info->tv_sec;
                    }
                }
            }
        }
        if (starttime.tv_sec < 0) {
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
#endif
}


/* this usleep isnt exactly accurate but should do ok */
/*
void usleep(unsigned int useconds)
{
struct timeval tnow, tthen, t0;

	gettimeofday(&tthen, NULL);
    t0 = tthen;
    tthen.tv_usec += useconds;
    while (tthen.tv_usec > 1000000) {
        tthen.tv_usec -= 1000000;
        tthen.tv_sec++;
    }
    
	if (useconds > 10000) {
        useconds -= 10000;
        Sleep(useconds/1000);
    }
    
	while (1) {
        gettimeofday(&tnow, NULL);
        if (tnow.tv_sec > tthen.tv_sec) {
            break;
        }
        if (tnow.tv_sec == tthen.tv_sec) {
            if (tnow.tv_usec > tthen.tv_usec) {
                break;
            }
        }
    }
}
*/

#ifdef HAVE_SETITIMER

/*
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
*/
	/*make sure the message queue is initialized *//*
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
*/
#endif
