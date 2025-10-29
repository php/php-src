/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Paul Panotzki - Bunyip Information Systems                   |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "zend_time.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef PHP_WIN32
#include "win32/getrusage.h"
#endif
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "ext/date/php_date.h"

static void _php_gettimeofday(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	bool get_as_float = 0;
	struct timespec ts;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(get_as_float)
	ZEND_PARSE_PARAMETERS_END();

	zend_time_real_spec(&ts);

	if (get_as_float) {
		RETURN_DOUBLE((double)(ts.tv_sec + ts.tv_nsec / 1000000000.0));
	}

	if (mode) {
		timelib_time_offset *offset;

		offset = timelib_get_time_zone_info(ts.tv_sec, get_timezone_info());

		array_init(return_value);
		add_assoc_long(return_value, "sec", ts.tv_sec);
		add_assoc_long(return_value, "usec", ts.tv_nsec / 1000);

		add_assoc_long(return_value, "minuteswest", -offset->offset / 60);
		add_assoc_long(return_value, "dsttime", offset->is_dst);

		timelib_time_offset_dtor(offset);
	} else {
		RETURN_NEW_STR(zend_strpprintf(0, "0.%08ld %lld", ts.tv_nsec / 10, (long long) ts.tv_sec));
	}
}

/* {{{ Returns either a string or a float containing the current time in seconds and microseconds */
PHP_FUNCTION(microtime)
{
	_php_gettimeofday(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Returns the current time as array */
PHP_FUNCTION(gettimeofday)
{
	_php_gettimeofday(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

#ifdef HAVE_GETRUSAGE
/* {{{ Returns an array of usage statistics */
PHP_FUNCTION(getrusage)
{
	struct rusage usg;
	zend_long pwho = 0;
	int who = RUSAGE_SELF;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(pwho)
	ZEND_PARSE_PARAMETERS_END();

	if (pwho == 1) {
		who = RUSAGE_CHILDREN;
	}

	memset(&usg, 0, sizeof(struct rusage));

	if (getrusage(who, &usg) == -1) {
		RETURN_FALSE;
	}

	array_init(return_value);

#define PHP_RUSAGE_PARA(a) \
		add_assoc_long(return_value, #a, usg.a)

#ifdef PHP_WIN32 /* Windows only implements a limited amount of fields from the rusage struct */
	PHP_RUSAGE_PARA(ru_majflt);
	PHP_RUSAGE_PARA(ru_maxrss);
#elif !defined(_OSD_POSIX) && !defined(__HAIKU__)
	PHP_RUSAGE_PARA(ru_oublock);
	PHP_RUSAGE_PARA(ru_inblock);
	PHP_RUSAGE_PARA(ru_msgsnd);
	PHP_RUSAGE_PARA(ru_msgrcv);
	PHP_RUSAGE_PARA(ru_maxrss);
	PHP_RUSAGE_PARA(ru_ixrss);
	PHP_RUSAGE_PARA(ru_idrss);
	PHP_RUSAGE_PARA(ru_minflt);
	PHP_RUSAGE_PARA(ru_majflt);
	PHP_RUSAGE_PARA(ru_nsignals);
	PHP_RUSAGE_PARA(ru_nvcsw);
	PHP_RUSAGE_PARA(ru_nivcsw);
	PHP_RUSAGE_PARA(ru_nswap);
#endif /*_OSD_POSIX*/
	PHP_RUSAGE_PARA(ru_utime.tv_usec);
	PHP_RUSAGE_PARA(ru_utime.tv_sec);
	PHP_RUSAGE_PARA(ru_stime.tv_usec);
	PHP_RUSAGE_PARA(ru_stime.tv_sec);

#undef PHP_RUSAGE_PARA
}
#endif /* HAVE_GETRUSAGE */

/* }}} */
