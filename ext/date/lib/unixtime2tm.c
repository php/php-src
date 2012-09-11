/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Derick Rethans <derick@derickrethans.nl>                    |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "timelib.h"

#include <stdio.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

static int month_tab_leap[12] = { -1, 30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
static int month_tab[12] =      { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };


/* Converts a Unix timestamp value into broken down time, in GMT */
void timelib_unixtime2gmt(timelib_time* tm, timelib_sll ts)
{
	timelib_sll days, remainder, tmp_days;
	timelib_sll cur_year = 1970;
	timelib_sll i;
	timelib_sll hours, minutes, seconds;
	int *months;

	days = ts / SECS_PER_DAY;
	remainder = ts - (days * SECS_PER_DAY);
	if (ts < 0 && remainder == 0) {
		days++;
		remainder -= SECS_PER_DAY;
	}
	TIMELIB_DEBUG(printf("days=%lld, rem=%lld\n", days, remainder););

	if (ts >= 0) {
		tmp_days = days + 1;

		if (tmp_days >= DAYS_PER_LYEAR_PERIOD || tmp_days <= -DAYS_PER_LYEAR_PERIOD) {
			cur_year += YEARS_PER_LYEAR_PERIOD * (tmp_days / DAYS_PER_LYEAR_PERIOD);
			tmp_days -= DAYS_PER_LYEAR_PERIOD * (tmp_days / DAYS_PER_LYEAR_PERIOD);
		}

		while (tmp_days >= DAYS_PER_LYEAR) {
			cur_year++;
			if (timelib_is_leap(cur_year)) {
				tmp_days -= DAYS_PER_LYEAR;
			} else {
				tmp_days -= DAYS_PER_YEAR;
			}
		}
	} else {
		tmp_days = days;

		/* Guess why this might be for, it has to do with a pope ;-). It's also
		 * only valid for Great Brittain and it's colonies. It needs fixing for
		 * other locales. *sigh*, why is this crap so complex! */
		/*
		if (ts <= TIMELIB_LL_CONST(-6857352000)) {
			tmp_days -= 11;
		}
		*/

		while (tmp_days <= 0) {
			if (tmp_days < -1460970) {
				cur_year -= 4000;
				TIMELIB_DEBUG(printf("tmp_days=%lld, year=%lld\n", tmp_days, cur_year););
				tmp_days += 1460970;
			} else {
				cur_year--;
				TIMELIB_DEBUG(printf("tmp_days=%lld, year=%lld\n", tmp_days, cur_year););
				if (timelib_is_leap(cur_year)) {
					tmp_days += DAYS_PER_LYEAR;
				} else {
					tmp_days += DAYS_PER_YEAR;
				}
			}
		}
		remainder += SECS_PER_DAY;
	}
	TIMELIB_DEBUG(printf("tmp_days=%lld, year=%lld\n", tmp_days, cur_year););

	months = timelib_is_leap(cur_year) ? month_tab_leap : month_tab;
	if (timelib_is_leap(cur_year) && cur_year < 1970) {
		tmp_days--;
	}
	i = 11;
	while (i > 0) {
		TIMELIB_DEBUG(printf("month=%lld (%d)\n", i, months[i]););
		if (tmp_days > months[i]) {
			break;
		}
		i--;
	}
	TIMELIB_DEBUG(printf("A: ts=%lld, year=%lld, month=%lld, day=%lld,", ts, cur_year, i + 1, tmp_days - months[i]););

	/* That was the date, now we do the tiiiime */
	hours = remainder / 3600;
	minutes = (remainder - hours * 3600) / 60;
	seconds = remainder % 60;
	TIMELIB_DEBUG(printf(" hour=%lld, minute=%lld, second=%lld\n", hours, minutes, seconds););

	tm->y = cur_year;
	tm->m = i + 1;
	tm->d = tmp_days - months[i];
	tm->h = hours;
	tm->i = minutes;
	tm->s = seconds;
	tm->z = 0;
	tm->dst = 0;
	tm->sse = ts;
	tm->sse_uptodate = 1;
	tm->tim_uptodate = 1;
	tm->is_localtime = 0;
}

void timelib_update_from_sse(timelib_time *tm)
{
	timelib_sll sse;

	sse = tm->sse;
	
	switch (tm->zone_type) {
		case TIMELIB_ZONETYPE_ABBR:
		case TIMELIB_ZONETYPE_OFFSET: {
			int z = tm->z;
			signed int dst = tm->dst;
			
			timelib_unixtime2gmt(tm, tm->sse - (tm->z * 60) + (tm->dst * 3600));

			tm->z = z;
			tm->dst = dst;
			goto cleanup;
		}

		case TIMELIB_ZONETYPE_ID: {
			timelib_time_offset *gmt_offset;
			
			gmt_offset = timelib_get_time_zone_info(tm->sse, tm->tz_info);
			timelib_unixtime2gmt(tm, tm->sse + gmt_offset->offset);
			timelib_time_offset_dtor(gmt_offset);
			
			goto cleanup;
		}

		default:
			timelib_unixtime2gmt(tm, tm->sse);
			goto cleanup;
	}
cleanup:
	tm->sse = sse;
	tm->is_localtime = 1;
	tm->have_zone = 1;
}

void timelib_unixtime2local(timelib_time *tm, timelib_sll ts)
{
	timelib_time_offset *gmt_offset;
	timelib_tzinfo      *tz = tm->tz_info;

	switch (tm->zone_type) {
		case TIMELIB_ZONETYPE_ABBR:
		case TIMELIB_ZONETYPE_OFFSET: {
			int z = tm->z;
			signed int dst = tm->dst;
			
			timelib_unixtime2gmt(tm, ts - (tm->z * 60) + (tm->dst * 3600));

			tm->z = z;
			tm->dst = dst;
			break;
		}

		case TIMELIB_ZONETYPE_ID:
			gmt_offset = timelib_get_time_zone_info(ts, tz);
			timelib_unixtime2gmt(tm, ts + gmt_offset->offset);

			/* we need to reset the sse here as unixtime2gmt modifies it */
			tm->sse = ts; 
			tm->dst = gmt_offset->is_dst;
			tm->z = gmt_offset->offset;
			tm->tz_info = tz;

			timelib_time_tz_abbr_update(tm, gmt_offset->abbr);
			timelib_time_offset_dtor(gmt_offset);
			break;

		default:
			tm->is_localtime = 0;
			tm->have_zone = 0;
			return;
	}

	tm->is_localtime = 1;
	tm->have_zone = 1;
}

void timelib_set_timezone(timelib_time *t, timelib_tzinfo *tz)
{
	timelib_time_offset *gmt_offset;

	gmt_offset = timelib_get_time_zone_info(t->sse, tz);
	t->z = gmt_offset->offset;
/*
	if (t->dst != gmt_offset->is_dst) {
		printf("ERROR (%d, %d)\n", t->dst, gmt_offset->is_dst);
		exit(1);
	}
*/
	t->dst = gmt_offset->is_dst;
	t->tz_info = tz;
	if (t->tz_abbr) {
		free(t->tz_abbr);
	}
	t->tz_abbr = strdup(gmt_offset->abbr);
	timelib_time_offset_dtor(gmt_offset);

	t->have_zone = 1;
	t->zone_type = TIMELIB_ZONETYPE_ID;
}

/* Converts the time stored in the struct to localtime if localtime = true,
 * otherwise it converts it to gmttime. This is only done when necessary
 * ofcourse. */
int timelib_apply_localtime(timelib_time *t, unsigned int localtime)
{
	if (localtime) {
		/* Converting from GMT time to local time */
		TIMELIB_DEBUG(printf("Converting from GMT time to local time\n"););

		/* Check if TZ is set */
		if (!t->tz_info) {
			TIMELIB_DEBUG(printf("E: No timezone configured, can't switch to local time\n"););
			return -1;
		}

		timelib_unixtime2local(t, t->sse);
	} else {
		/* Converting from local time to GMT time */
		TIMELIB_DEBUG(printf("Converting from local time to GMT time\n"););

		timelib_unixtime2gmt(t, t->sse);
	}
	return 0;
}
