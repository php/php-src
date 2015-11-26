
	/* $Id: fpm_clock.h,v 1.2 2008/05/24 17:38:47 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_CLOCK_H
#define FPM_CLOCK_H 1

#include <sys/time.h>

int fpm_clock_init();
int fpm_clock_get(struct timeval *tv);

#endif
