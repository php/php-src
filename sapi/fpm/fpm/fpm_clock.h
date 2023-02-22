	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_CLOCK_H
#define FPM_CLOCK_H 1

#include "zend_result.h"

#include <sys/time.h>

zend_result fpm_clock_init(void);
zend_result fpm_clock_get(struct timeval *tv);

#endif
