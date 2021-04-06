/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2019 Derick Rethans
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

#include "timelib.h"
#include "timelib_private.h"

/*                                    dec  jan  feb  mrt  apr  may  jun  jul  aug  sep  oct  nov  dec */
static int days_in_month_leap[13] = {  31,  31,  29,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31 };
static int days_in_month[13]      = {  31,  31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31 };

static void do_range_limit(timelib_sll start, timelib_sll end, timelib_sll adj, timelib_sll *a, timelib_sll *b)
{
	if (*a < start) {
		*b -= (start - *a - 1) / adj + 1;
		*a += adj * ((start - *a - 1) / adj + 1);
	}
	if (*a >= end) {
		*b += *a / adj;
		*a -= adj * (*a / adj);
	}
}

static void inc_month(timelib_sll *y, timelib_sll *m)
{
	(*m)++;
	if (*m > 12) {
		*m -= 12;
		(*y)++;
	}
}

static void dec_month(timelib_sll *y, timelib_sll *m)
{
	(*m)--;
	if (*m < 1) {
		*m += 12;
		(*y)--;
	}
}

static void do_range_limit_days_relative(timelib_sll *base_y, timelib_sll *base_m, timelib_sll *y, timelib_sll *m, timelib_sll *d, timelib_sll invert)
{
	timelib_sll leapyear;
	timelib_sll month, year;
	timelib_sll days;

	do_range_limit(1, 13, 12, base_m, base_y);

	year = *base_y;
	month = *base_m;

/*
	printf( "S: Y%d M%d   %d %d %d   %d\n", year, month, *y, *m, *d, days);
*/
	if (!invert) {
		while (*d < 0) {
			dec_month(&year, &month);
			leapyear = timelib_is_leap(year);
			days = leapyear ? days_in_month_leap[month] : days_in_month[month];

			/* printf( "I  Y%d M%d   %d %d %d   %d\n", year, month, *y, *m, *d, days); */

			*d += days;
			(*m)--;
		}
	} else {
		while (*d < 0) {
			leapyear = timelib_is_leap(year);
			days = leapyear ? days_in_month_leap[month] : days_in_month[month];

			/* printf( "I  Y%d M%d   %d %d %d   %d\n", year, month, *y, *m, *d, days); */

			*d += days;
			(*m)--;
			inc_month(&year, &month);
		}
	}
	/*
	printf( "E: Y%d M%d   %d %d %d   %d\n", year, month, *y, *m, *d, days);
	*/
}

static int do_range_limit_days(timelib_sll *y, timelib_sll *m, timelib_sll *d)
{
	timelib_sll leapyear;
	timelib_sll days_this_month;
	timelib_sll last_month, last_year;
	timelib_sll days_last_month;

	/* can jump an entire leap year period quickly */
	if (*d >= DAYS_PER_ERA || *d <= -DAYS_PER_ERA) {
		*y += YEARS_PER_ERA * (*d / DAYS_PER_ERA);
		*d -= DAYS_PER_ERA * (*d / DAYS_PER_ERA);
	}

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
	if (time->relative.weekday_behavior == 2)
	{
		/* To make "this week" work, where the current DOW is a "sunday" */
		if (current_dow == 0 && time->relative.weekday != 0) {
			time->relative.weekday -= 7;
		}

		/* To make "sunday this week" work, where the current DOW is not a
		 * "sunday" */
		if (time->relative.weekday == 0 && current_dow != 0) {
			time->relative.weekday = 7;
		}

		time->d -= current_dow;
		time->d += time->relative.weekday;
		return;
	}
	difference = time->relative.weekday - current_dow;
	if ((time->relative.d < 0 && difference < 0) || (time->relative.d >= 0 && difference <= -time->relative.weekday_behavior)) {
		difference += 7;
	}
	if (time->relative.weekday >= 0) {
		time->d += difference;
	} else {
		time->d -= (7 - (abs(time->relative.weekday) - current_dow));
	}
	time->relative.have_weekday_relative = 0;
}

void timelib_do_rel_normalize(timelib_time *base, timelib_rel_time *rt)
{
	do_range_limit(0, 1000000, 1000000, &rt->us, &rt->s);
	do_range_limit(0, 60, 60, &rt->s, &rt->i);
	do_range_limit(0, 60, 60, &rt->i, &rt->h);
	do_range_limit(0, 24, 24, &rt->h, &rt->d);
	do_range_limit(0, 12, 12, &rt->m, &rt->y);

	do_range_limit_days_relative(&base->y, &base->m, &rt->y, &rt->m, &rt->d, rt->invert);
	do_range_limit(0, 12, 12, &rt->m, &rt->y);
}

static void magic_date_calc(timelib_time *time)
{
	timelib_sll y, ddd, mi, mm, dd, g;

	/* The algorithm doesn't work before the year 1 */
	if (time->d < -719498) {
		return;
	}

	g = time->d + HINNANT_EPOCH_SHIFT - 1;

	y = (10000 * g + 14780) / 3652425;
	ddd = g - ((365*y) + (y/4) - (y/100) + (y/400));
	if (ddd < 0) {
		y--;
		ddd = g - ((365*y) + (y/4) - (y/100) + (y/400));
	}
	mi = (100 * ddd + 52) / 3060;
	mm = ((mi + 2) % 12) + 1;
	y = y + (mi + 2) / 12;
	dd = ddd - ((mi * 306 + 5) / 10) + 1;
	time->y = y;
	time->m = mm;
	time->d = dd;
}

void timelib_do_normalize(timelib_time* time)
{
	if (time->us != TIMELIB_UNSET) do_range_limit(0, 1000000, 1000000, &time->us, &time->s);
	if (time->s != TIMELIB_UNSET) do_range_limit(0, 60, 60, &time->s, &time->i);
	if (time->s != TIMELIB_UNSET) do_range_limit(0, 60, 60, &time->i, &time->h);
	if (time->s != TIMELIB_UNSET) do_range_limit(0, 24, 24, &time->h, &time->d);
	do_range_limit(1, 13, 12, &time->m, &time->y);

	/* Short cut if we're doing things against the Epoch */
	if (time->y == 1970 && time->m == 1 && time->d != 1) {
		magic_date_calc(time);
	}

	do {} while (do_range_limit_days(&time->y, &time->m, &time->d));
	do_range_limit(1, 13, 12, &time->m, &time->y);
}

static void do_adjust_relative(timelib_time* time)
{
	if (time->relative.have_weekday_relative) {
		do_adjust_for_weekday(time);
	}
	timelib_do_normalize(time);

	if (time->have_relative) {
		time->us += time->relative.us;

		time->s += time->relative.s;
		time->i += time->relative.i;
		time->h += time->relative.h;

		time->d += time->relative.d;
		time->m += time->relative.m;
		time->y += time->relative.y;
	}

	switch (time->relative.first_last_day_of) {
		case TIMELIB_SPECIAL_FIRST_DAY_OF_MONTH: /* first */
			time->d = 1;
			break;
		case TIMELIB_SPECIAL_LAST_DAY_OF_MONTH: /* last */
			time->d = 0;
			time->m++;
			break;
	}

	timelib_do_normalize(time);
}

static void do_adjust_special_weekday(timelib_time* time)
{
	timelib_sll count, dow, rem;

	count = time->relative.special.amount;
	dow = timelib_day_of_week(time->y, time->m, time->d);

	/* Add increments of 5 weekdays as a week, leaving the DOW unchanged. */
	time->d += (count / 5) * 7;

	/* Deal with the remainder. */
	rem = (count % 5);

	if (count > 0) {
		if (rem == 0) {
			/* Head back to Friday if we stop on the weekend. */
			if (dow == 0) {
				time->d -= 2;
			} else if (dow == 6) {
				time->d -= 1;
			}
		} else if (dow == 6) {
			/* We ended up on Saturday, but there's still work to do, so move
			 * to Sunday and continue from there. */
			time->d += 1;
		} else if (dow + rem > 5) {
			/* We're on a weekday, but we're going past Friday, so skip right
			 * over the weekend. */
			time->d += 2;
		}
	} else {
		/* Completely mirror the forward direction. This also covers the 0
		 * case, since if we start on the weekend, we want to move forward as
		 * if we stopped there while going backwards. */
		if (rem == 0) {
			if (dow == 6) {
				time->d += 2;
			} else if (dow == 0) {
				time->d += 1;
			}
		} else if (dow == 0) {
			time->d -= 1;
		} else if (dow + rem < 1) {
			time->d -= 2;
		}
	}

	time->d += rem;
}

static void do_adjust_special(timelib_time* time)
{
	if (time->relative.have_special_relative) {
		switch (time->relative.special.type) {
			case TIMELIB_SPECIAL_WEEKDAY:
				do_adjust_special_weekday(time);
				break;
		}
	}
	timelib_do_normalize(time);
	memset(&(time->relative.special), 0, sizeof(time->relative.special));
}

static void do_adjust_special_early(timelib_time* time)
{
	if (time->relative.have_special_relative) {
		switch (time->relative.special.type) {
			case TIMELIB_SPECIAL_DAY_OF_WEEK_IN_MONTH:
				time->d = 1;
				time->m += time->relative.m;
				time->relative.m = 0;
				break;
			case TIMELIB_SPECIAL_LAST_DAY_OF_WEEK_IN_MONTH:
				time->d = 1;
				time->m += time->relative.m + 1;
				time->relative.m = 0;
				break;
		}
	}
	switch (time->relative.first_last_day_of) {
		case TIMELIB_SPECIAL_FIRST_DAY_OF_MONTH: /* first */
			time->d = 1;
			break;
		case TIMELIB_SPECIAL_LAST_DAY_OF_MONTH: /* last */
			time->d = 0;
			time->m++;
			break;
	}
	timelib_do_normalize(time);
}

static void do_adjust_timezone(timelib_time *tz, timelib_tzinfo *tzi)
{
	switch (tz->zone_type) {
		case TIMELIB_ZONETYPE_OFFSET:

			tz->is_localtime = 1;
			tz->sse += -tz->z;
			return;

		case TIMELIB_ZONETYPE_ABBR: {
			timelib_sll tmp;

			tz->is_localtime = 1;
			tmp = -tz->z;
			tmp -= tz->dst * 3600;
			tz->sse += (-tz->z - tz->dst * SECS_PER_HOUR);
			return;
		}

		case TIMELIB_ZONETYPE_ID:
			tzi = tz->tz_info;
			/* Break intentionally missing */

		default: {
			/* No timezone in struct, fallback to reference if possible */
			timelib_time_offset *current, *after;
			timelib_sll          adjustment;
			int                  in_transition;

			if (!tzi) {
				return;
			}

			current = timelib_get_time_zone_info(tz->sse, tzi);
			after = timelib_get_time_zone_info(tz->sse - current->offset, tzi);
			tz->is_localtime = 1;

			in_transition = (
				((tz->sse - after->offset) >= (after->transition_time + (current->offset - after->offset))) &&
				((tz->sse - after->offset) < after->transition_time)
			);

			if ((current->offset != after->offset) && !in_transition) {
				adjustment = -after->offset;
			} else {
				adjustment = -current->offset;
			}
			timelib_time_offset_dtor(current);
			timelib_time_offset_dtor(after);

			tz->sse += adjustment;
			timelib_set_timezone(tz, tzi);
			return;
		}
	}
	return;
}

timelib_sll timelib_epoch_days_from_time(timelib_time *time)
{
	timelib_sll y = time->y; // Make copy, as we don't want to change the original one
	timelib_sll era, year_of_era, day_of_year, day_of_era;

	y -= time->m <= 2;
	era = (y >= 0 ? y : y - 399) / YEARS_PER_ERA;
	year_of_era = y - era * YEARS_PER_ERA;                                                        // [0, 399]
	day_of_year = (153 * (time->m + (time->m > 2 ? -3 : 9)) + 2)/5 + time->d - 1;                 // [0, 365]
	day_of_era = year_of_era * DAYS_PER_YEAR + year_of_era / 4 - year_of_era / 100 + day_of_year; // [0, 146096]

	return era * DAYS_PER_ERA + day_of_era - HINNANT_EPOCH_SHIFT;
}

void timelib_update_ts(timelib_time* time, timelib_tzinfo* tzi)
{
	do_adjust_special_early(time);
	do_adjust_relative(time);
	do_adjust_special(time);

	time->sse =
		(timelib_epoch_days_from_time(time) * SECS_PER_DAY) +
		timelib_hms_to_seconds(time->h, time->i, time->s);

	// This modifies time->sse, if needed
	do_adjust_timezone(time, tzi);

	time->sse_uptodate = 1;
	time->have_relative = time->relative.have_weekday_relative = time->relative.have_special_relative = time->relative.first_last_day_of = 0;
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
