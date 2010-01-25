/*-
 * Copyright (c) 2008 Christos Zoulas
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "file.h"

#ifndef lint
FILE_RCSID("@(#)$File: cdf_time.c,v 1.6 2009/03/10 11:44:29 christos Exp $")
#endif

#include <time.h>
#ifdef TEST
#include <err.h>
#endif
#include <string.h>

#include "cdf.h"

#define isleap(y) ((((y) % 4) == 0) && \
    ((((y) % 100) != 0) || (((y) % 400) == 0)))

static const int mdays[] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

/*
 * Return the number of days between jan 01 1601 and jan 01 of year.
 */
static int
cdf_getdays(int year)
{
	int days = 0;
	int y;

	for (y = CDF_BASE_YEAR; y < year; y++)
		days += isleap(y) + 365;
		
	return days;
}

/*
 * Return the day within the month
 */
static int
cdf_getday(int year, int days)
{
	size_t m;

	for (m = 0; m < sizeof(mdays) / sizeof(mdays[0]); m++) {
		int sub = mdays[m] + (m == 1 && isleap(year));
		if (days < sub)
			return days;
		days -= sub;
	}
	return days;
}

/* 
 * Return the 0...11 month number.
 */
static int
cdf_getmonth(int year, int days)
{
	size_t m;

	for (m = 0; m < sizeof(mdays) / sizeof(mdays[0]); m++) {
		days -= mdays[m];
		if (m == 1 && isleap(year))
			days--;
		if (days <= 0)
			return (int)m;
	}
	return (int)m;
}

int
cdf_timestamp_to_timespec(struct timeval *ts, cdf_timestamp_t t)
{
	struct tm tm;
#ifdef HAVE_STRUCT_TM_TM_ZONE
	static char UTC[] = "UTC";
#endif
	int rdays;

	/* Time interval, in microseconds */
	ts->tv_usec = (t % CDF_TIME_PREC) * CDF_TIME_PREC;

	t /= CDF_TIME_PREC;
	tm.tm_sec = t % 60;
	t /= 60;

	tm.tm_min = t % 60;
	t /= 60;

	tm.tm_hour = t % 24;
	t /= 24;

	// XXX: Approx
	tm.tm_year = CDF_BASE_YEAR + (t / 365);

	rdays = cdf_getdays(tm.tm_year);
	t -= rdays;
	tm.tm_mday = cdf_getday(tm.tm_year, t);
	tm.tm_mon = cdf_getmonth(tm.tm_year, t);
	tm.tm_wday = 0;
	tm.tm_yday = 0;
	tm.tm_isdst = 0;
#ifdef HAVE_STRUCT_TM_TM_GMTOFF
	tm.tm_gmtoff = 0;
#endif
#ifdef HAVE_STRUCT_TM_TM_ZONE
	tm.tm_zone = UTC;
#endif
	tm.tm_year -= 1900;
	ts->tv_sec = mktime(&tm);
	if (ts->tv_sec == -1) {
		errno = EINVAL;
		return -1;
	}
	return 0;
}

int
cdf_timespec_to_timestamp(cdf_timestamp_t *t, const struct timeval *ts)
{
	(void)&t;
	(void)&ts;
#ifdef notyet
	struct tm tm;
	if (gmtime_r(&ts->ts_sec, &tm) == NULL) {
		errno = EINVAL;
		return -1;
	}
	*t = (ts->ts_usec / CDF_TIME_PREC) * CDF_TIME_PREC;
	*t = tm.tm_sec;
	*t += tm.tm_min * 60;
	*t += tm.tm_hour * 60 * 60;
	*t += tm.tm_mday * 60 * 60 * 24;
#endif
	return 0;
}


#ifdef TEST
int
main(int argc, char *argv[])
{
	struct timeval ts;
	static const cdf_timestamp_t tst = 0x01A5E403C2D59C00ULL;
	static const char *ref = "Sat Apr 23 01:30:00 1977";
	char *p, *q;

	cdf_timestamp_to_timespec(&ts, tst);
	p = ctime(&ts.tv_sec);
	if ((q = strchr(p, '\n')) != NULL)
		*q = '\0';
	if (strcmp(ref, p) != 0)
		errx(1, "Error date %s != %s\n", ref, p);
	return 0;
}
#endif
