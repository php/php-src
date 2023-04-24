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
#include <math.h>

static void swap_times(timelib_time **one, timelib_time **two, timelib_rel_time *rt)
{
	timelib_time *swp;

	swp = *two;
	*two = *one;
	*one = swp;
	rt->invert = 1;
}

static void sort_old_to_new(timelib_time **one, timelib_time **two, timelib_rel_time *rt)
{
	/* Check whether date/times need to be inverted. If both times are
	 * TIMELIB_ZONETYPE_ID times with the same TZID, we use the y-s + us fields. */
	if (
		(*one)->zone_type == TIMELIB_ZONETYPE_ID &&
		(*two)->zone_type == TIMELIB_ZONETYPE_ID &&
		(strcmp((*one)->tz_info->name, (*two)->tz_info->name) == 0)
	) {
		if (
			((*one)->y > (*two)->y) ||
			((*one)->y == (*two)->y && (*one)->m > (*two)->m) ||
			((*one)->y == (*two)->y && (*one)->m == (*two)->m && (*one)->d > (*two)->d) ||
			((*one)->y == (*two)->y && (*one)->m == (*two)->m && (*one)->d == (*two)->d && (*one)->h > (*two)->h) ||
			((*one)->y == (*two)->y && (*one)->m == (*two)->m && (*one)->d == (*two)->d && (*one)->h == (*two)->h && (*one)->i > (*two)->i) ||
			((*one)->y == (*two)->y && (*one)->m == (*two)->m && (*one)->d == (*two)->d && (*one)->h == (*two)->h && (*one)->i == (*two)->i && (*one)->s > (*two)->s) ||
			((*one)->y == (*two)->y && (*one)->m == (*two)->m && (*one)->d == (*two)->d && (*one)->h == (*two)->h && (*one)->i == (*two)->i && (*one)->s == (*two)->s && (*one)->us > (*two)->us)
		) {
			swap_times(one, two, rt);
		}
		return;
	}

	/* Fall back to using the SSE instead to rearrange */
	if (
		((*one)->sse > (*two)->sse) ||
		((*one)->sse == (*two)->sse && (*one)->us > (*two)->us)
	) {
		swap_times(one, two, rt);
	}
}

static timelib_rel_time *timelib_diff_with_tzid(timelib_time *one, timelib_time *two)
{
	timelib_rel_time *rt;
	timelib_sll       dst_corr = 0, dst_h_corr = 0, dst_m_corr = 0;
	int32_t           trans_offset;
	timelib_sll       trans_transition_time;

	rt = timelib_rel_time_ctor();
	rt->invert = 0;

	sort_old_to_new(&one, &two, rt);

	/* Calculate correction for UTC offset changes between first and second SSE */
	dst_corr = two->z - one->z;
	dst_h_corr = dst_corr / 3600;
	dst_m_corr = (dst_corr % 3600) / 60;

	rt->y = two->y - one->y;
	rt->m = two->m - one->m;
	rt->d = two->d - one->d;
	rt->h = two->h - one->h;
	rt->i = two->i - one->i;
	rt->s = two->s - one->s;
	rt->us = two->us - one->us;

	rt->days = timelib_diff_days(one, two);

	/* Fall Back: Cater for transition period, where rt->invert is 0, but there are negative numbers */
	if (two->sse < one->sse) {
		timelib_sll flipped = llabs((rt->i * 60) + (rt->s) - dst_corr);
		rt->h = flipped / SECS_PER_HOUR;
		rt->i = (flipped - rt->h * SECS_PER_HOUR) / 60;
		rt->s = flipped % 60;

		rt->invert = 1 - rt->invert;
	}

	timelib_do_rel_normalize(rt->invert ? one : two, rt);

	if (one->dst == 1 && two->dst == 0) { /* Fall Back */
		if (two->tz_info) {
			if ((two->sse - one->sse + dst_corr) < SECS_PER_DAY) {
				rt->h -= dst_h_corr;
				rt->i -= dst_m_corr;
			}
		}
	} else if (one->dst == 0 && two->dst == 1) { /* Spring Forward */
		if (two->tz_info) {
			int success = timelib_get_time_zone_offset_info(two->sse, two->tz_info, &trans_offset, &trans_transition_time, NULL);

			if (
				success &&
				!((one->sse + SECS_PER_DAY > trans_transition_time) && (one->sse + SECS_PER_DAY <= (trans_transition_time + dst_corr))) &&
				two->sse >= trans_transition_time &&
				((two->sse - one->sse + dst_corr) % SECS_PER_DAY) > (two->sse - trans_transition_time)
			) {
				rt->h -= dst_h_corr;
				rt->i -= dst_m_corr;
			}
		}
	} else if (two->sse - one->sse >= SECS_PER_DAY) {
		/* Check whether we're in the period to the next transition time */
		if (timelib_get_time_zone_offset_info(two->sse - two->z, two->tz_info, &trans_offset, &trans_transition_time, NULL)) {
			dst_corr = one->z - trans_offset;

			if (two->sse >= trans_transition_time - dst_corr && two->sse < trans_transition_time) {
				rt->d--;
				rt->h = 24;
			}
		}
	}

	return rt;
}

timelib_rel_time *timelib_diff(timelib_time *one, timelib_time *two)
{
	timelib_rel_time *rt;

	if (one->zone_type == TIMELIB_ZONETYPE_ID && two->zone_type == TIMELIB_ZONETYPE_ID && strcmp(one->tz_info->name, two->tz_info->name) == 0) {
		return timelib_diff_with_tzid(one, two);
	}

	rt = timelib_rel_time_ctor();
	rt->invert = 0;

	sort_old_to_new(&one, &two, rt);

	rt->y = two->y - one->y;
	rt->m = two->m - one->m;
	rt->d = two->d - one->d;
	rt->h = two->h - one->h;
	if (one->zone_type != TIMELIB_ZONETYPE_ID) {
		rt->h = rt->h + one->dst;
	}
	if (two->zone_type != TIMELIB_ZONETYPE_ID) {
		rt->h = rt->h - two->dst;
	}
	rt->i = two->i - one->i;
	rt->s = two->s - one->s - two->z + one->z;
	rt->us = two->us - one->us;

	rt->days = timelib_diff_days(one, two);

	timelib_do_rel_normalize(rt->invert ? one : two, rt);

	return rt;
}


int timelib_diff_days(timelib_time *one, timelib_time *two)
{
	int days = 0;

	if (timelib_same_timezone(one, two)) {
		timelib_time *earliest, *latest;
		double earliest_time, latest_time;

		if (timelib_time_compare(one, two) < 0) {
			earliest = one;
			latest = two;
		} else {
			earliest = two;
			latest = one;
		}
		timelib_hmsf_to_decimal_hour(earliest->h, earliest->i, earliest->s, earliest->us, &earliest_time);
		timelib_hmsf_to_decimal_hour(latest->h, latest->i, latest->s, latest->us, &latest_time);

		days = llabs(timelib_epoch_days_from_time(one) - timelib_epoch_days_from_time(two));
		if (latest_time < earliest_time && days > 0) {
			days--;
		}
	} else {
		days = fabs(floor(one->sse - two->sse) / 86400);
	}

	return days;
}


timelib_time *timelib_add(timelib_time *old_time, timelib_rel_time *interval)
{
	int bias = 1;
	timelib_time *t = timelib_time_clone(old_time);

	if (interval->have_weekday_relative || interval->have_special_relative) {
		memcpy(&t->relative, interval, sizeof(timelib_rel_time));
	} else {
		if (interval->invert) {
			bias = -1;
		}
		memset(&t->relative, 0, sizeof(timelib_rel_time));
		t->relative.y = interval->y * bias;
		t->relative.m = interval->m * bias;
		t->relative.d = interval->d * bias;
		t->relative.h = interval->h * bias;
		t->relative.i = interval->i * bias;
		t->relative.s = interval->s * bias;
		t->relative.us = interval->us * bias;
	}
	t->have_relative = 1;
	t->sse_uptodate = 0;

	timelib_update_ts(t, NULL);

	timelib_update_from_sse(t);
	t->have_relative = 0;

	return t;
}

timelib_time *timelib_sub(timelib_time *old_time, timelib_rel_time *interval)
{
	int bias = 1;
	timelib_time *t = timelib_time_clone(old_time);

	if (interval->invert) {
		bias = -1;
	}

	memset(&t->relative, 0, sizeof(timelib_rel_time));
	t->relative.y = 0 - (interval->y * bias);
	t->relative.m = 0 - (interval->m * bias);
	t->relative.d = 0 - (interval->d * bias);
	t->relative.h = 0 - (interval->h * bias);
	t->relative.i = 0 - (interval->i * bias);
	t->relative.s = 0 - (interval->s * bias);
	t->relative.us = 0 - (interval->us * bias);
	t->have_relative = 1;
	t->sse_uptodate = 0;

	timelib_update_ts(t, NULL);

	timelib_update_from_sse(t);

	t->have_relative = 0;

	return t;
}

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


timelib_time *timelib_add_wall(timelib_time *old_time, timelib_rel_time *interval)
{
	int bias = 1;
	timelib_time *t = timelib_time_clone(old_time);

	t->have_relative = 1;
	t->sse_uptodate = 0;

	if (interval->have_weekday_relative || interval->have_special_relative) {
		memcpy(&t->relative, interval, sizeof(timelib_rel_time));

		timelib_update_ts(t, NULL);

		timelib_update_from_sse(t);
	} else {
		if (interval->invert) {
			bias = -1;
		}
		memset(&t->relative, 0, sizeof(timelib_rel_time));
		t->relative.y = interval->y * bias;
		t->relative.m = interval->m * bias;
		t->relative.d = interval->d * bias;

		if (t->relative.y || t->relative.m || t->relative.d) {
			timelib_update_ts(t, NULL);
		}

		if (interval->us == 0) {
			t->sse += bias * timelib_hms_to_seconds(interval->h, interval->i, interval->s);
			timelib_update_from_sse(t);
		} else {
			timelib_rel_time *temp_interval = timelib_rel_time_clone(interval);

			do_range_limit(0, 1000000, 1000000, &temp_interval->us, &temp_interval->s);
			t->sse += bias * timelib_hms_to_seconds(temp_interval->h, temp_interval->i, temp_interval->s);
			timelib_update_from_sse(t);
			t->us += temp_interval->us * bias;

			timelib_do_normalize(t);
			timelib_update_ts(t, NULL);

			timelib_rel_time_dtor(temp_interval);
		}
		timelib_do_normalize(t);
	}

	if (t->zone_type == TIMELIB_ZONETYPE_ID) {
		timelib_set_timezone(t, t->tz_info);
	}
	t->have_relative = 0;

	return t;
}

timelib_time *timelib_sub_wall(timelib_time *old_time, timelib_rel_time *interval)
{
	int bias = 1;
	timelib_time *t = timelib_time_clone(old_time);

	t->have_relative = 1;
	t->sse_uptodate = 0;

	if (interval->have_weekday_relative || interval->have_special_relative) {
		memcpy(&t->relative, interval, sizeof(timelib_rel_time));

		timelib_update_ts(t, NULL);
		timelib_update_from_sse(t);
	} else {
		if (interval->invert) {
			bias = -1;
		}
		memset(&t->relative, 0, sizeof(timelib_rel_time));
		t->relative.y = 0 - (interval->y * bias);
		t->relative.m = 0 - (interval->m * bias);
		t->relative.d = 0 - (interval->d * bias);

		if (t->relative.y || t->relative.m || t->relative.d) {
			timelib_update_ts(t, NULL);
		}

		if (interval->us == 0) {
			t->sse -= bias * timelib_hms_to_seconds(interval->h, interval->i, interval->s);
			timelib_update_from_sse(t);
		} else {
			timelib_rel_time *temp_interval = timelib_rel_time_clone(interval);

			do_range_limit(0, 1000000, 1000000, &temp_interval->us, &temp_interval->s);
			t->sse -= bias * timelib_hms_to_seconds(temp_interval->h, temp_interval->i, temp_interval->s);
			timelib_update_from_sse(t);
			t->us -= temp_interval->us * bias;

			timelib_do_normalize(t);
			timelib_update_ts(t, NULL);

			timelib_rel_time_dtor(temp_interval);
		}
		timelib_do_normalize(t);
	}

	if (t->zone_type == TIMELIB_ZONETYPE_ID) {
		timelib_set_timezone(t, t->tz_info);
	}
	t->have_relative = 0;

	return t;
}
