/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Paul Panotzki - Bunyip Information Systems                   |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef PHP_WIN32
#include "win32/time.h"
#else
#include <sys/time.h>
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

#include "microtime.h"

#define NUL  '\0'
#define MICRO_IN_SEC 1000000.00
#define SEC_IN_MIN 60

/* {{{ proto string microtime(void)
   Returns a string containing the current time in seconds and microseconds */
PHP_FUNCTION(microtime)
{
#ifdef HAVE_GETTIMEOFDAY
	struct timeval tp;
	long sec = 0L;
	double msec = 0.0;
	char ret[100];
	
	if (gettimeofday((struct timeval *) &tp, (NUL)) == 0) {
		msec = (double) (tp.tv_usec / MICRO_IN_SEC);
		sec = tp.tv_sec;
	
		if (msec >= 1.0) msec -= (long) msec;
		snprintf(ret, 100, "%.8f %ld", msec, sec);
		RETVAL_STRING(ret,1);
	} else
#endif
		RETURN_FALSE;
}
/* }}} */

/* {{{ proto array gettimeofday(void)
   Returns the current time as array */
PHP_FUNCTION(gettimeofday)
{
#ifdef HAVE_GETTIMEOFDAY
	struct timeval tp;
	struct timezone tz;
	
	memset(&tp, 0, sizeof(tp));
	memset(&tz, 0, sizeof(tz));
	if(gettimeofday(&tp, &tz) == 0) {
		array_init(return_value);
		add_assoc_long(return_value, "sec", tp.tv_sec);
		add_assoc_long(return_value, "usec", tp.tv_usec);
#ifdef PHP_WIN32
		add_assoc_long(return_value, "minuteswest", tz.tz_minuteswest/SEC_IN_MIN);
#else
		add_assoc_long(return_value, "minuteswest", tz.tz_minuteswest);
#endif			
		add_assoc_long(return_value, "dsttime", tz.tz_dsttime);
		return;
	} else
#endif
	RETURN_FALSE;
}
/* }}} */

#ifdef HAVE_GETRUSAGE
/* {{{ proto array getrusage([int who])
   Returns an array of usage statistics */
PHP_FUNCTION(getrusage)
{
	struct rusage usg;
	int ac = ZEND_NUM_ARGS();
	pval **pwho;
	int who = RUSAGE_SELF;

	if(ac == 1 &&
		zend_get_parameters_ex(ac, &pwho) != FAILURE) {
		convert_to_long_ex(pwho);
		if(Z_LVAL_PP(pwho) == 1)
			who = RUSAGE_CHILDREN;
	}

	memset(&usg, 0, sizeof(usg));
	if(getrusage(who, &usg) == -1) {
		RETURN_FALSE;
	}

	array_init(return_value);
#define PHP_RUSAGE_PARA(a) \
		add_assoc_long(return_value, #a, usg.a)
#if !defined( _OSD_POSIX) && !defined(__BEOS__) /* BS2000 has only a few fields in the rusage struct */
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
#endif /*_OSD_POSIX*/
	PHP_RUSAGE_PARA(ru_utime.tv_usec);
	PHP_RUSAGE_PARA(ru_utime.tv_sec);
	PHP_RUSAGE_PARA(ru_stime.tv_usec);
	PHP_RUSAGE_PARA(ru_stime.tv_sec);
#undef PHP_RUSAGE_PARA
}
#endif /* HAVE_GETRUSAGE */

/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
