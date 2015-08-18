/*****************************************************************************
 *                                                                           *
 * sys/time.h                                                                *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *
 * Modified by Shane Caraveo for PHP
 *
 *****************************************************************************/
#ifndef TIME_H
#define TIME_H

/* Include stuff ************************************************************ */
#include <time.h>
#include "php.h"

/* Struct stuff ************************************************************* */
struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};


struct itimerval {
	struct timeval it_interval;	/* next value */
	struct timeval it_value;	/* current value */
};

#if !defined(timespec) && _MSC_VER < 1900
struct timespec
{
	time_t   tv_sec;   /* seconds */
	long     tv_nsec;  /* nanoseconds */
};
#endif

#define ITIMER_REAL    0		/*generates sigalrm */
#define ITIMER_VIRTUAL 1		/*generates sigvtalrm */
#define ITIMER_VIRT    1		/*generates sigvtalrm */
#define ITIMER_PROF    2		/*generates sigprof */

/* Prototype stuff ********************************************************** */
PHPAPI extern int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info);

/* setitimer operates at 100 millisecond resolution */
PHPAPI extern int setitimer(int which, const struct itimerval *value,
					 struct itimerval *ovalue);

PHPAPI int nanosleep( const struct timespec * rqtp, struct timespec * rmtp );

PHPAPI int usleep(unsigned int useconds);

#ifdef PHP_EXPORTS
/* This symbols are needed only for the DllMain, but should not be exported 
	or be available when used with PHP binaries. */
BOOL php_win32_init_gettimeofday(void);
#endif

#endif
