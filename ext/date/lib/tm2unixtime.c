/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
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

/*                                    jan  feb  mrt  apr  may  jun  jul  aug  sep  oct  nov  dec */
static int month_tab_leap[12]     = {  -1,  30,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334 };
static int month_tab[12]          = {   0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334 };

/*                                    dec  jan  feb  mrt  apr  may  jun  jul  aug  sep  oct  nov  dec */
static int days_in_month_leap[13] = {  31,  31,  29,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31 };
static int days_in_month[13]      = {  31,  31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31 };

static int do_range_limit(timelib_sll start, timelib_sll end, timelib_sll adj, timelib_sll *a, timelib_sll *b)
{
	if (*a < start) {
		*a += adj;
		(*b)--;
		return 1;
	}
	if (*a >= end) {
		if (start == 0) {
			(*b) += (*a / end);
			(*a) -= (end * (*a / end));
			return 0;
		}

		*a -= adj;
		(*b)++;
		return 1;
	}
	return 0;
}

static int do_range_limit_days(timelib_sll *y, timelib_sll *m, timelib_sll *d)
{
	timelib_sll leapyear;
	timelib_sll days_this_month;
	timelib_sll last_month, last_year;
	timelib_sll days_last_month;

	do_range_limit(1, 13, 12, m, y);

	leapyear = timelib_is_leap(*y);
	days_this_month = leapyear ? days_in_month_leap[*m] : days_in_month[*m];
	last_month = (*m) - 1;

	if (last_month < 1) {
		last_month += 12;
		last_year = (*y) - 1;
	} else {
		last_year = (*y);
	}
	leapyear = timelib_is_leap(last_year);
	days_last_month = leapyear ? days_in_month_leap[last_month] : days_in_month[last_month];

	if (*d <= 0) {
		*d += days_last_month;
		(*m)--;
		return 1;
	}
	if (*d > days_this_month) {
		*d -= days_this_month;
		(*m)++;
		return 1;
	}
	return 0;
}

static void do_adjust_for_weekday(timelib_time* time)
{
	timelib_sll current_dow, difference;

	current_dow = timelib_day_of_week(time->y, time->m, time->d);
	difference = time->relative.weekday - current_dow;
	if ((time->relative.d < 0 && difference < 0) || (time->relative.d >= 0 && difference <= -time->relative.weekday_behavior)) {
		difference += 7;
	}
	if (time->relative.weekday >= 0) {
		time->d += difference;
	} else {
		time->d -= (7 - (abs(time->relative.weekday) - current_dow));
	}
	time->have_weekday_relative = 0;
}

static void do_normalize(timelib_time* time)
{
	do {} while (do_range_limit(0, 60, 60, &time->s, &time->i));
	do {} while (do_range_limit(0, 60, 60, &time->i, &time->h));
	do {} while (do_range_limit(0, 24, 24, &time->h, &time->d));
	do {} while (do_range_limit(1, 13, 12, &time->m, &time->y));

	do {} while (do_range_limit_days(&time->y, &time->m, &time->d));
	do {} while (do_range_limit(1, 13, 12, &time->m, &time->y));
}

static void do_adjust_relative(timelib_time* time)
{
	if (time->have_weekday_relative) {
		do_adjust_for_weekday(time);
	}
	do_normalize(time);

	if (time->have_relative) {
		time->s += time->relative.s;
		time->i += time->relative.i;
		time->h += time->relative.h;

		time->d += time->relative.d;
		time->m += time->relative.m;
		time->y += time->relative.y;
	}
	do_normalize(time);

	memset(&(time->relative), 0, sizeof(time->relative));
	time->have_relative = 0;
}

static void do_adjust_special_weekday(timelib_time* time)
{
	timelib_sll current_dow, count;

	count = time->special.amount;

	current_dow = timelib_day_of_week(time->y, time->m, time->d);
	if (count == 0) {
		/* skip over saturday and sunday */
		if (current_dow == 6) {
			time->d += 2;
		}
		/* skip over sunday */
		if (current_dow == 0) {
			time->d += 1;
		}
	} else if (count > 0) {
		/* skip over saturday and sunday */
		if (current_dow == 5) {
			time->d += 2;
		}
		/* skip over sunday */
		if (current_dow == 6) {
			time->d += 1;
		}
		/* add increments of 5 weekdays as a week */
		time->d += (count / 5) * 7;
		/* if current DOW plus the remainder > 5, add two days */
		current_dow = timelib_day_of_week(time->y, time->m, time->d);
		time->d += (count % 5);
		if ((count % 5) + current_dow > 5) {
			time->d += 2;
		}
	} else if (count < 0) {
		/* skip over sunday and saturday */
		if (current_dow == 1) {
			time->d -= 2;
		}
		/* skip over satruday */
		if (current_dow == 0 ) {
			time->d -= 1;
		}
		/* subtract increments of 5 weekdays as a week */
		time->d += (count / 5) * 7;
		/* if current DOW minus the remainder < 0, subtract two days */
		current_dow = timelib_day_of_week(time->y, time->m, time->d);
		time->d += (count % 5);
		if ((count % 5) + current_dow < 1) {
			time->d -= 2;
		}
	}
}

static void do_adjust_special(timelib_time* time)
{
	if (time->have_special_relative) {
		switch (time->special.type) {
			case TIMELIB_SPECIAL_WEEKDAY:
				do_adjust_special_weekday(time);
				break;
		}
	}
	do_normalize(time);
	memset(&(time->special), 0, sizeof(time->special));
	time->have_relative = 0;
}

static timelib_sll do_years(timelib_sll year)
{
	timelib_sll i;
	timelib_sll res = 0;
	timelib_sll eras;

	eras = (year - 1970) / 40000;
	if (eras != 0) {
		year = year - (eras * 40000);
		res += (SECS_PER_ERA * eras * 100);
	}

	if (year >= 1970) {
		for (i = year - 1; i >= 1970; i--) {
			if (timelib_is_leap(i)) {
				res += (DAYS_PER_LYEAR * SECS_PER_DAY);
			} else {
				res += (DAYS_PER_YEAR * SECS_PER_DAY);
			}
		}
	} else {
		for (i = 1969; i >= year; i--) {
			if (timelib_is_leap(i)) {
				res -= (DAYS_PER_LYEAR * SECS_PER_DAY);
			} else {
				res -= (DAYS_PER_YEAR * SECS_PER_DAY);
			}
		}
	}
	return res;
}

static timelib_sll do_months(timelib_ull month, timelib_ull year)
{
	if (timelib_is_leap(year)) {
		return ((month_tab_leap[month - 1] + 1) * SECS_PER_DAY);
	} else {
		return ((month_tab[month - 1]) * SECS_PER_DAY);
	}
}

static timelib_sll do_days(timelib_ull day)
{
	return ((day - 1) * SECS_PER_DAY);
}

static timelib_sll do_time(timelib_ull hour, timelib_ull minute, timelib_ull second)
{
	timelib_sll res = 0;

	res += hour * 3600;
	res += minute * 60;
	res += second;
	return res;
}

static timelib_sll do_adjust_timezone(timelib_time *tz, timelib_tzinfo *tzi)
{
	switch (tz->zone_type) {
		case TIMELIB_ZONETYPE_OFFSET:

			tz->is_localtime = 1;
			return tz->z * 60;
			break;

		case TIMELIB_ZONETYPE_ABBR: {
			timelib_sll tmp;

			tz->is_localtime = 1;
			tmp = tz->z;
			tmp -= tz->dst * 60;
			tmp *= 60;
			return tmp;
			}
			break;

		case TIMELIB_ZONETYPE_ID:
			tzi = tz->tz_info;
			/* Break intentionally missing */

		default:
			/* No timezone in struct, fallback to reference if possible */
			if (tzi) {
				timelib_time_offset *before, *after;
				timelib_sll          tmp;
				int                  in_transistion;
				
				tz->is_localtime = 1;
				before = timelib_get_time_zone_info(tz->sse, tzi);
				after = timelib_get_time_zone_info(tz->sse - before->offset, tzi);
				timelib_set_timezone(tz, tzi);

				in_transistion = (
					((tz->sse - after->offset) >= (after->transistion_time + (before->offset - after->offset))) &&
					((tz->sse - after->offset) < after->transistion_time)
				);
				
				if ((before->offset != after->offset) && !in_transistion) {
					tmp = -after->offset;
				} else {
					tmp = -tz->z;
				}
				timelib_time_offset_dtor(before);
				timelib_time_offset_dtor(after);
				
				return tmp;
			}
	}
	return 0;
}

void timelib_update_ts(timelib_time* time, timelib_tzinfo* tzi)
{
	timelib_sll res = 0;

	do_adjust_relative(time);
	do_adjust_special(time);
	res += do_years(time->y);
	res += do_months(time->m, time->y);
	res += do_days(time->d);
	res += do_time(time->h, time->i, time->s);
	time->sse = res;

	res += do_adjust_timezone(time, tzi);
	time->sse = res;

	time->sse_uptodate = 1;
}

#if 0
int main(void)
{
	timelib_sll res;
	timelib_time time;

	time = timelib_strtotime("10 Feb 2005 06:07:03 PM CET"); /* 1108055223 */
	printf ("%04d-%02d-%02d %02d:%02d:%02d.%-5d %+04d %1d",
		time.y, time.m, time.d, time.h, time.i, time.s, time.f, time.z, time.dst);
	if (time.have_relative) {
		printf ("%3dY %3dM %3dD / %3dH %3dM %3dS", 
			time.relative.y, time.relative.m, time.relative.d, time.relative.h, time.relative.i, time.relative.s);
	}
	if (time.have_weekday_relative) {
		printf (" / %d", time.relative.weekday);
	}
	res = time2unixtime(&time);
	printf("%Ld\n", res);

	return 0;
}
#endif
