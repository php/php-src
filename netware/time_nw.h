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
#ifdef USE_WINSOCK
/*#include <ws2nlm.h>*/
#include <novsock2.h>
#else
#include <sys/socket.h>
#endif
#include <time.h>

/*
Commented out most of the stuff for now, since we dunno the utility right now 
*/

/* Struct stuff ************************************************************* */
/*
struct {
	int tz_minuteswest;
	int tz_dsttime;
} timezone;
*/
/*
struct {
	struct timeval it_interval;	*//* next value *//*
	struct timeval it_value;	*//* current value *//*
} itimerval;
*/
#define ITIMER_REAL    0		/*generates sigalrm */
#define ITIMER_VIRTUAL 1		/*generates sigvtalrm */
#define ITIMER_VIRT    1		/*generates sigvtalrm */
#define ITIMER_PROF    2		/*generates sigprof */

/* Prototype stuff ********************************************************** */
/*
extern int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info);
*/

/* setitimer operates at 100 millisecond resolution */
/*
extern int setitimer(int which, const struct itimerval *value,
					 struct itimerval *ovalue);
*/

#endif
