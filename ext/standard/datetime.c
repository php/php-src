/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2005 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Rasmus Lerdorf <rasmus@php.net>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#if HAVE_STRPTIME
#define _XOPEN_SOURCE
#endif

#include "php.h"
#include "zend_operators.h"
#include "datetime.h"
#include "php_globals.h"

#include <time.h>
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif
#include <stdio.h>

char *mon_full_names[] = {
	"January", "February", "March", "April",
	"May", "June", "July", "August",
	"September", "October", "November", "December"
};

char *mon_short_names[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

char *day_full_names[] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

char *day_short_names[] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

#if !defined(HAVE_TM_ZONE) && !defined(_TIMEZONE) && !defined(HAVE_DECLARED_TIMEZONE)
#ifdef NETWARE
#define timezone    _timezone   /* timezone is called '_timezone' in new version of LibC */
#endif
extern time_t timezone;
extern int daylight;
#endif

static int phpday_tab[2][12] = {
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

#define isleap(year) ((((year) % 4) == 0 && ((year) % 100) != 0) || ((year) % 400)==0)
#define YEAR_BASE 1900

/* {{{ php_idate
 */
PHPAPI int php_idate(char format, time_t timestamp, int gm)
{
	time_t the_time;
	struct tm *ta, tmbuf;
	int h, beat, fd, wd, yd, wk;
#if !HAVE_TM_GMTOFF
	long tzone;
	char *tname[2]= {"GMT Standard Time", "BST"};
#endif
	
	the_time = timestamp;

	if (gm) {
		ta = php_gmtime_r(&the_time, &tmbuf);
#if !HAVE_TM_GMTOFF
		tzone = 0;
#endif
	} else {
		ta = php_localtime_r(&the_time, &tmbuf);
#if !HAVE_TM_GMTOFF
#ifdef __CYGWIN__
		tzone = _timezone;
#else
		tzone = timezone;
#endif
		tname[0] = tzname[0];
#endif
	}
	
	switch (format) {
		case 'U':		/* seconds since the epoch */
			return (long)the_time;
		case 'Y':		/* year, numeric, 4 digits */
			return  ta->tm_year + YEAR_BASE;
		case 'z':		/* day (of the year) */
			return ta->tm_yday;
		case 'y':		/* year, numeric, 2 digits */
			return (ta->tm_year) % 100;
		case 'm':		/* month, numeric */
		case 'n':
			return ta->tm_mon + 1;
		case 'd':		/* day of the month, numeric */
		case 'j':
			return ta->tm_mday;
		case 'H':		/* hour, numeric, 24 hour format */
		case 'G':
			return ta->tm_hour;
		case 'h':		/* hour, numeric, 12 hour format */
		case 'g':
			h = ta->tm_hour % 12;
			if (h == 0) {
				h = 12;
			}
			return h;
		case 'i':		/* minutes, numeric */
			return ta->tm_min;
		case 's':		/* seconds, numeric */
			return  ta->tm_sec;
		case 't':		/* days in current month */
			return phpday_tab[isleap((ta->tm_year + YEAR_BASE))][ta->tm_mon];
		case 'w':		/* day of the week, numeric EXTENSION */
			return ta->tm_wday;
		case 'Z':		/* timezone offset in seconds */
#if HAVE_TM_GMTOFF
			return ta->tm_gmtoff;
#else
			return ta->tm_isdst ? -(tzone - 3600) : -tzone;
#endif
		case 'L':		/* boolean for leapyear */
				return isleap(ta->tm_year + YEAR_BASE) ? 1 : 0;
		case 'B':	/* Swatch Beat a.k.a. Internet Time */
			beat =  (((((long)the_time) - (((long)the_time) - ((((long)the_time) % 86400) + 3600))) * 10) / 864);
			while (beat < 0) {
				beat += 1000;
			}
			beat = beat % 1000;
			return beat;
		case 'I':
			return ta->tm_isdst;
		case 'W':		/* ISO-8601 week number of year, weeks starting on Monday */
			wd = (ta->tm_wday == 0) ? 6 : ta->tm_wday - 1; /* weekday */
			yd = ta->tm_yday + 1;					/* days since January 1st */
			fd = (7 + wd - yd % 7+ 1) % 7;			/* weekday (1st January) */
			if ((yd <= 7 - fd) && fd > 3) {			/* week is a last year week (52 or 53) */
				wk = (fd == 4 || (fd == 5 && isleap((ta->tm_year + YEAR_BASE - 1)))) ? 53 : 52;
			}
			/* week is a next year week (1) */
			else if (isleap((ta->tm_year + YEAR_BASE)) + 365 - yd < 3 - wd) {
				wk = 1;
			}
			/* normal week */
			else {
				wk = (yd + 6 - wd + fd) / 7 - (fd > 3);
			}
			return wk;
			break;
		default:
			return 0;
	}
}
/* }}} */

/* {{{ proto int idate(string format [, int timestamp])
   Format a local time/date as integer */
PHP_FUNCTION(idate)
{
	zval **format, **timestamp;
	int ret;
	time_t t;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &format) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			t = time(NULL);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &format, &timestamp) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(timestamp);
			t = Z_LVAL_PP(timestamp);
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(format);
	
	if (Z_STRLEN_PP(format) != 1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "idate format is one char");
		RETURN_FALSE;
	}

	ret = php_idate(Z_STRVAL_PP(format)[0], t, 0);
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ php_std_date
   Return date string in standard format for http headers */
PHPAPI char *php_std_date(time_t t TSRMLS_DC)
{
	struct tm *tm1, tmbuf;
	char *str;

	tm1 = php_gmtime_r(&t, &tmbuf);
	str = emalloc(81);
	if (PG(y2k_compliance)) {
		snprintf(str, 80, "%s, %02d %s %04d %02d:%02d:%02d GMT",
				day_short_names[tm1->tm_wday],
				tm1->tm_mday,
				mon_short_names[tm1->tm_mon],
				tm1->tm_year + 1900,
				tm1->tm_hour, tm1->tm_min, tm1->tm_sec);
	} else {
		snprintf(str, 80, "%s, %02d-%s-%02d %02d:%02d:%02d GMT",
				day_full_names[tm1->tm_wday],
				tm1->tm_mday,
				mon_short_names[tm1->tm_mon],
				((tm1->tm_year) % 100),
				tm1->tm_hour, tm1->tm_min, tm1->tm_sec);
	}
	
	str[79] = 0;
	return (str);
}
/* }}} */


#if HAVE_STRPTIME
/* {{{ proto string strptime(string timestamp, string format)
   Parse a time/date generated with strftime() */
PHP_FUNCTION(strptime)
{
	char      *ts;
	int        ts_length;
	char      *format;
	int        format_length;
	struct tm  parsed_time;
	char      *unparsed_part;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", 
		&ts, &ts_length, &format, &format_length) == FAILURE) {
		return;
	}

	unparsed_part = strptime(ts, format, &parsed_time);
	if (unparsed_part == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	add_assoc_long(return_value, "tm_sec",   parsed_time.tm_sec);
	add_assoc_long(return_value, "tm_min",   parsed_time.tm_min);
	add_assoc_long(return_value, "tm_hour",  parsed_time.tm_hour);
	add_assoc_long(return_value, "tm_mday",  parsed_time.tm_mday);
	add_assoc_long(return_value, "tm_mon",   parsed_time.tm_mon);
	add_assoc_long(return_value, "tm_year",  parsed_time.tm_year);
	add_assoc_long(return_value, "tm_wday",  parsed_time.tm_wday);
	add_assoc_long(return_value, "tm_yday",  parsed_time.tm_yday);
	add_assoc_string(return_value, "unparsed", unparsed_part, 1);
}
/* }}} */
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
