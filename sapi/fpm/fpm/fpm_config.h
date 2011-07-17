/* $Id: fpm_config.h,v 1.16 2008/05/25 00:30:43 anight Exp $ */
/* (c) 2007,2008 Andrei Nigmatulin */

#include <php_config.h>

/* Solaris does not have it */
#ifndef INADDR_NONE
# define INADDR_NONE (-1)
#endif


/* If we're not using GNU C, elide __attribute__ */
#ifndef __GNUC__
# define __attribute__(x)  /*NOTHING*/
#endif

/* Missing timer* macros (for solaris) */
#ifndef timerisset
# define timerisset(tvp) ((tvp)->tv_sec || (tvp)->tv_usec)
#endif

#ifndef timerclear
# define timerclear(tvp) ((tvp)->tv_sec = (tvp)->tv_usec = 0)
#endif

#ifndef timersub
# define timersub(tvp, uvp, vvp)                          \
	do {                                                  \
		(vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;    \
		(vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec; \
		if ((vvp)->tv_usec < 0) {                         \
			(vvp)->tv_sec--;                              \
			(vvp)->tv_usec += 1000000;                    \
		}                                                 \
	} while (0)
#endif

#ifndef timeradd
# define timeradd(a, b, result)                          \
	do {                                                 \
		(result)->tv_sec = (a)->tv_sec + (b)->tv_sec;    \
		(result)->tv_usec = (a)->tv_usec + (b)->tv_usec; \
		if ((result)->tv_usec >= 1000000)                \
		{                                                \
			++(result)->tv_sec;                          \
			(result)->tv_usec -= 1000000;                \
		}                                                \
	} while (0)
#endif

#ifndef timercmp
/* does not work for >= and <= */
# define timercmp(a, b, CMP)         \
  (((a)->tv_sec == (b)->tv_sec) ?    \
  ((a)->tv_usec CMP (b)->tv_usec) :  \
    ((a)->tv_sec CMP (b)->tv_sec))
#endif
/* endof timer* macros */

#ifndef MIN
# define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
# define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#if defined(HAVE_PTRACE) || defined(PROC_MEM_FILE) || defined(HAVE_MACH_VM_READ)
# define HAVE_FPM_TRACE 1
#else
# define HAVE_FPM_TRACE 0
#endif

#if defined(HAVE_LQ_TCP_INFO) || defined(HAVE_LQ_SO_LISTENQ)
# define HAVE_FPM_LQ 1
#else
# define HAVE_FPM_LQ 0
#endif

