	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_CLOCK_H
#define FPM_CLOCK_H 1

#include <sys/time.h>

int fpm_clock_init();
int fpm_clock_get(struct timeval *tv);

#endif
