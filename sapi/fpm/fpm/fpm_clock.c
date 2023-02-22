	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#ifdef HAVE_CLOCK_GETTIME
#include <time.h> /* for CLOCK_MONOTONIC */
#endif

#include "fpm_clock.h"
#include "zlog.h"


/* posix monotonic clock - preferred source of time */
#if defined(HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)

static int monotonic_works;

zend_result fpm_clock_init(void)
{
	struct timespec ts;

	monotonic_works = 0;

	if (0 == clock_gettime(CLOCK_MONOTONIC, &ts)) {
		monotonic_works = 1;
	}

	return SUCCESS;
}

zend_result fpm_clock_get(struct timeval *tv) /* {{{ */
{
	if (monotonic_works) {
		struct timespec ts;

		if (0 > clock_gettime(CLOCK_MONOTONIC, &ts)) {
			zlog(ZLOG_SYSERROR, "clock_gettime() failed");
			return FAILURE;
		}

		tv->tv_sec = ts.tv_sec;
		tv->tv_usec = ts.tv_nsec / 1000;
		return SUCCESS;
	}

	return gettimeofday(tv, 0) == 0 ? SUCCESS : FAILURE;
}
/* }}} */

/* macOS clock */
#elif defined(HAVE_CLOCK_GET_TIME)

#include <mach/mach.h>
#include <mach/clock.h>
#include <mach/mach_error.h>

static clock_serv_t mach_clock;

/* this code borrowed from here: http://lists.apple.com/archives/Darwin-development/2002/Mar/msg00746.html */
/* mach_clock also should be re-initialized in child process after fork */
zend_result fpm_clock_init(void)
{
	kern_return_t ret;
	mach_timespec_t aTime;

	ret = host_get_clock_service(mach_host_self(), REALTIME_CLOCK, &mach_clock);

	if (ret != KERN_SUCCESS) {
		zlog(ZLOG_ERROR, "host_get_clock_service() failed: %s", mach_error_string(ret));
		return FAILURE;
	}

	/* test if it works */
	ret = clock_get_time(mach_clock, &aTime);

	if (ret != KERN_SUCCESS) {
		zlog(ZLOG_ERROR, "clock_get_time() failed: %s", mach_error_string(ret));
		return FAILURE;
	}

	return SUCCESS;
}

zend_result fpm_clock_get(struct timeval *tv) /* {{{ */
{
	kern_return_t ret;
	mach_timespec_t aTime;

	ret = clock_get_time(mach_clock, &aTime);

	if (ret != KERN_SUCCESS) {
		zlog(ZLOG_ERROR, "clock_get_time() failed: %s", mach_error_string(ret));
		return FAILURE;
	}

	tv->tv_sec = aTime.tv_sec;
	tv->tv_usec = aTime.tv_nsec / 1000;

	return SUCCESS;
}
/* }}} */

#else /* no clock */

zend_result fpm_clock_init(void)
{
	return SUCCESS;
}

zend_result fpm_clock_get(struct timeval *tv) /* {{{ */
{
	return gettimeofday(tv, 0) == 0 ? SUCCESS : FAILURE;
}
/* }}} */

#endif
