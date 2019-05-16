/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Derick Rethans
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __TIMELIB_PRIVATE_H__
#define __TIMELIB_PRIVATE_H__

#ifdef HAVE_TIMELIB_CONFIG_H
# include "timelib_config.h"
#endif

#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#ifdef _WIN32
# ifdef HAVE_WINSOCK2_H
#  include <winsock2.h>
# endif
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#endif

#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#if defined(HAVE_STDINT_H)
# include <stdint.h>
#endif

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#if HAVE_IO_H
# include <io.h>
#endif

#if HAVE_DIRENT_H
# include <dirent.h>
#endif

#include <stdio.h>

#if HAVE_LIMITS_H
#include <limits.h>
#endif

#define TIMELIB_SECOND   1
#define TIMELIB_MINUTE   2
#define TIMELIB_HOUR     3
#define TIMELIB_DAY      4
#define TIMELIB_MONTH    5
#define TIMELIB_YEAR     6
#define TIMELIB_WEEKDAY  7
#define TIMELIB_SPECIAL  8
#define TIMELIB_MICROSEC 9

#define TIMELIB_SPECIAL_WEEKDAY                   0x01
#define TIMELIB_SPECIAL_DAY_OF_WEEK_IN_MONTH      0x02
#define TIMELIB_SPECIAL_LAST_DAY_OF_WEEK_IN_MONTH 0x03

#define TIMELIB_SPECIAL_FIRST_DAY_OF_MONTH        0x01
#define TIMELIB_SPECIAL_LAST_DAY_OF_MONTH         0x02

#define SECS_PER_ERA   TIMELIB_LL_CONST(12622780800)
#define SECS_PER_DAY   86400
#define DAYS_PER_YEAR    365
#define DAYS_PER_LYEAR   366
/* 400*365 days + 97 leap days */
#define DAYS_PER_LYEAR_PERIOD 146097
#define YEARS_PER_LYEAR_PERIOD 400

#define TIMELIB_TZINFO_PHP       0x01
#define TIMELIB_TZINFO_ZONEINFO  0x02

#define timelib_is_leap(y) ((y) % 4 == 0 && ((y) % 100 != 0 || (y) % 400 == 0))

#define TIMELIB_DEBUG(s)  if (0) { s }

#define TIMELIB_TIME_FREE(m)    \
	if (m) {        \
		timelib_free(m);    \
		m = NULL;   \
	}

struct _ttinfo
{
	int32_t      offset;
	int          isdst;
	unsigned int abbr_idx;

	unsigned int isstdcnt;
	unsigned int isgmtcnt;
};

struct _tlinfo
{
	int64_t  trans;
	int32_t  offset;
};


#ifndef LONG_MAX
#define LONG_MAX 2147483647L
#endif

#ifndef LONG_MIN
#define LONG_MIN (- LONG_MAX - 1)
#endif

/* From unixtime2tm.c */
int timelib_apply_localtime(timelib_time *t, unsigned int localtime);

/* From parse_tz.c */
void timelib_time_tz_abbr_update(timelib_time* tm, char* tz_abbr);

/* From timelib.c */
int timelib_strcasecmp(const char *s1, const char *s2);
int timelib_strncasecmp(const char *s1, const char *s2, size_t n);

#endif
