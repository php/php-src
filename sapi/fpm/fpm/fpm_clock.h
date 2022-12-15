	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_CLOCK_H
#define FPM_CLOCK_H 1

#include <sys/time.h>
#include <stdbool.h>

bool fpm_clock_init(void);
int fpm_clock_get(struct timeval *tv);

#endif
