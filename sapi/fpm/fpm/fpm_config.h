
	/* $Id: fpm_config.h,v 1.16 2008/05/25 00:30:43 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#include <php_config.h>
#ifdef FPM_AUTOCONFIG_H
#include <fpm_autoconfig.h>
#endif

/* Solaris does not have it */
#ifndef INADDR_NONE
#define INADDR_NONE (-1)
#endif


/* If we're not using GNU C, elide __attribute__ */
#ifndef __GNUC__
#  define  __attribute__(x)  /*NOTHING*/
#endif


/* Solaris does not have it */
#ifndef timersub
#define	timersub(tvp, uvp, vvp)						\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;	\
		if ((vvp)->tv_usec < 0) {				\
			(vvp)->tv_sec--;				\
			(vvp)->tv_usec += 1000000;			\
		}							\
	} while (0)
#endif

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#if defined(HAVE_PTRACE) || defined(PROC_MEM_FILE) || defined(HAVE_MACH_VM_READ)
#define HAVE_FPM_TRACE 1
#else
#define HAVE_FPM_TRACE 0
#endif

