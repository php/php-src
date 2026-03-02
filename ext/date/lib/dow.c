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

static int m_table_common[13] = { -1, 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 }; /* 1 = jan */
static int m_table_leap[13] =   { -1, 6, 2, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 }; /* 1 = jan */

static timelib_sll positive_mod(timelib_sll x, timelib_sll y)
{
	timelib_sll tmp;

	tmp = x % y;
	if (tmp < 0) {
		tmp += y;
	}

	return tmp;
}

static timelib_sll century_value(timelib_sll j)
{
	return 6 - positive_mod(j, 4) * 2;
}

static timelib_sll timelib_day_of_week_ex(timelib_sll y, timelib_sll m, timelib_sll d, int iso)
{
	timelib_sll c1, y1, m1, dow;

	/* Only valid for Gregorian calendar, commented out as we don't handle
	 * Julian calendar. We just return the 'wrong' day of week to be
	 * consistent. */
	c1 = century_value(positive_mod(y, 400) / 100);
	y1 = positive_mod(y, 100);
	m1 = timelib_is_leap(y) ? m_table_leap[m] : m_table_common[m];
	dow = positive_mod((c1 + y1 + m1 + (y1 / 4) + d), 7);
	if (iso) {
		if (dow == 0) {
			dow = 7;
		}
	}
	return dow;
}

timelib_sll timelib_day_of_week(timelib_sll y, timelib_sll m, timelib_sll d)
{
	return timelib_day_of_week_ex(y, m, d, 0);
}

timelib_sll timelib_iso_day_of_week(timelib_sll y, timelib_sll m, timelib_sll d)
{
	return timelib_day_of_week_ex(y, m, d, 1);
}

                                /*     jan  feb  mar  apr  may  jun  jul  aug  sep  oct  nov  dec */
static int d_table_common[13]  = {  0,   0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334 };
static int d_table_leap[13]    = {  0,   0,  31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335 };
static int ml_table_common[13] = {  0,  31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31 };
static int ml_table_leap[13]   = {  0,  31,  29,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31 };

timelib_sll timelib_day_of_year(timelib_sll y, timelib_sll m, timelib_sll d)
{
	return (timelib_is_leap(y) ? d_table_leap[m] : d_table_common[m]) + d - 1;
}

timelib_sll timelib_days_in_month(timelib_sll y, timelib_sll m)
{
	return timelib_is_leap(y) ? ml_table_leap[m] : ml_table_common[m];
}

void timelib_isoweek_from_date(timelib_sll y, timelib_sll m, timelib_sll d, timelib_sll *iw, timelib_sll *iy)
{
	int y_leap, prev_y_leap, doy, jan1weekday, weekday;

	y_leap = timelib_is_leap(y);
	prev_y_leap = timelib_is_leap(y-1);
	doy = timelib_day_of_year(y, m, d) + 1;
	if (y_leap && m > 2) {
		doy++;
	}
	jan1weekday = timelib_day_of_week(y, 1, 1);
	weekday = timelib_day_of_week(y, m, d);
	if (weekday == 0) weekday = 7;
	if (jan1weekday == 0) jan1weekday = 7;
	/* Find if Y M D falls in YearNumber Y-1, WeekNumber 52 or 53 */
	if (doy <= (8 - jan1weekday) && jan1weekday > 4) {
		*iy = y - 1;
		if (jan1weekday == 5 || (jan1weekday == 6 && prev_y_leap)) {
			*iw = 53;
		} else {
			*iw = 52;
		}
	} else {
		*iy = y;
	}
	/* 8. Find if Y M D falls in YearNumber Y+1, WeekNumber 1 */
	if (*iy == y) {
		int i;

		i = y_leap ? 366 : 365;
		if ((i - (doy - y_leap)) < (4 - weekday)) {
			*iy = y + 1;
			*iw = 1;
			return;
		}
	}
	/* 9. Find if Y M D falls in YearNumber Y, WeekNumber 1 through 53 */
	if (*iy == y) {
		int j;

		j = doy + (7 - weekday) + (jan1weekday - 1);
		*iw = j / 7;
		if (jan1weekday > 4) {
			*iw -= 1;
		}
	}
}

void timelib_isodate_from_date(timelib_sll y, timelib_sll m, timelib_sll d, timelib_sll *iy, timelib_sll *iw, timelib_sll *id)
{
	timelib_isoweek_from_date(y, m, d, iw, iy);
	*id = timelib_day_of_week_ex(y, m, d, 1);
}

timelib_sll timelib_daynr_from_weeknr(timelib_sll iy, timelib_sll iw, timelib_sll id)
{
	timelib_sll dow, day;

	/* Figure out the dayofweek for y-1-1 */
	dow = timelib_day_of_week(iy, 1, 1);
	/* then use that to figure out the offset for day 1 of week 1 */
	day = 0 - (dow > 4 ? dow - 7 : dow);

	/* Add weeks and days */
	return day + ((iw - 1) * 7) + id;
}

void timelib_date_from_isodate(timelib_sll iy, timelib_sll iw, timelib_sll id, timelib_sll *y, timelib_sll *m, timelib_sll *d)
{
	timelib_sll daynr = timelib_daynr_from_weeknr(iy, iw, id) + 1;
	int *table;
	bool is_leap_year;

	// Invariant: is_leap_year == timelib_is_leap(*y)
	*y = iy;
	is_leap_year = timelib_is_leap(*y);

	// Establish invariant that daynr >= 0
	while (daynr <= 0) {
		*y -= 1;
		daynr += (is_leap_year = timelib_is_leap(*y)) ? 366 : 365;
	}

	// Establish invariant that daynr <= number of days in *yr
	while (daynr > (is_leap_year ? 366 : 365)) {
		daynr -= is_leap_year ? 366 : 365;
		*y += 1;
		is_leap_year = timelib_is_leap(*y);
	}

	table = is_leap_year ? ml_table_leap : ml_table_common;

	// Establish invariant that daynr <= number of days in *m
	*m = 1;
	while (daynr > table[*m]) {
		daynr -= table[*m];
		*m += 1;
	}

	*d = daynr;
}

int timelib_valid_time(timelib_sll h, timelib_sll i, timelib_sll s)
{
	if (h < 0 || h > 23 || i < 0 || i > 59 || s < 0 || s > 59) {
		return 0;
	}
	return 1;
}

int timelib_valid_date(timelib_sll y, timelib_sll m, timelib_sll d)
{
	if (m < 1 || m > 12 || d < 1 || d > timelib_days_in_month(y, m)) {
		return 0;
	}
	return 1;
}
#if 0
int main(void)
{
	printf("dow = %d\n", timelib_day_of_week(1978, 12, 22)); /* 5 */
	printf("dow = %d\n", timelib_day_of_week(2005,  2, 19)); /* 6 */
}
#endif
