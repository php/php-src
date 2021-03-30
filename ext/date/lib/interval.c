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

timelib_rel_time *timelib_diff(timelib_time *one, timelib_time *two)
{
	timelib_rel_time *rt;
	timelib_time *swp;
	timelib_sll dst_corr = 0, dst_h_corr = 0, dst_m_corr = 0;
	timelib_time_offset *trans = NULL;


	rt = timelib_rel_time_ctor();
	rt->invert = 0;
	if (
		(one->sse > two->sse) ||
		(one->sse == two->sse && one->us > two->us)
	) {
		swp = two;
		two = one;
		one = swp;
		rt->invert = 1;
	}

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

	rt->days = fabs(floor((one->sse - two->sse - (dst_h_corr * 3600) - (dst_m_corr * 60)) / 86400));

	/* Fall Back: Cater for transition period, where rt->invert is 0, but there are negative numbers */
	if (one->dst == 1 && two->dst == 0) {
		/* First for two "Type 3" times */
		if (one->zone_type == 3 && two->zone_type == 3) {
			trans = timelib_get_time_zone_info(two->sse, two->tz_info);
			if (trans) {
				if (one->sse >= trans->transition_time + dst_corr && one->sse < trans->transition_time) {
					timelib_sll flipped = SECS_PER_HOUR + (rt->i * 60) + (rt->s);
					rt->h = flipped / SECS_PER_HOUR;
					rt->i = (flipped - rt->h * SECS_PER_HOUR) / 60;
					rt->s = flipped % 60;
				}
				timelib_time_offset_dtor(trans);
				trans = NULL;
			}
		} else if (rt->h == 0 && (rt->i < 0 || rt->s < 0)) {
			/* Then for all the others */
			timelib_sll flipped = SECS_PER_HOUR + (rt->i * 60) + (rt->s);
			rt->h = flipped / SECS_PER_HOUR;
			rt->i = (flipped - rt->h * SECS_PER_HOUR) / 60;
			rt->s = flipped % 60;
			dst_corr += SECS_PER_HOUR;
			dst_h_corr++;
		}
	}

	timelib_do_rel_normalize(rt->invert ? one : two, rt);

	/* Do corrections for "Type 3" times */
	if (one->zone_type == 3 && two->zone_type == 3) {
		if (one->dst == 1 && two->dst == 0) { /* Fall Back */
			if (two->tz_info) {
				trans = timelib_get_time_zone_info(two->sse, two->tz_info);

				if (
					trans &&
					two->sse >= trans->transition_time &&
					((two->sse - one->sse + dst_corr) % SECS_PER_DAY) > (two->sse - trans->transition_time)
				) {
					rt->h -= dst_h_corr;
					rt->i -= dst_m_corr;
				}
			}
		} else if (one->dst == 0 && two->dst == 1) { /* Spring Forward */
			if (two->tz_info) {
				trans = timelib_get_time_zone_info(two->sse, two->tz_info);

				if (
					trans &&
					!((one->sse + SECS_PER_DAY > trans->transition_time) && (one->sse + SECS_PER_DAY <= (trans->transition_time + dst_corr))) &&
					two->sse >= trans->transition_time &&
					((two->sse - one->sse + dst_corr) % SECS_PER_DAY) > (two->sse - trans->transition_time)
				) {
					rt->h -= dst_h_corr;
					rt->i -= dst_m_corr;
				}
			}
		} else if (two->sse - one->sse >= SECS_PER_DAY) {
			/* Check whether we're in the period to the next transition time */
			trans = timelib_get_time_zone_info(two->sse - two->z, two->tz_info);
			dst_corr = one->z - trans->offset;

			if (two->sse >= trans->transition_time - dst_corr && two->sse < trans->transition_time) {
				rt->d--;
				rt->h = 24;
			}
		}
	} else {
		/* Then for all the others */
		rt->h -= dst_h_corr + (two->dst - one->dst);
		rt->i -= dst_m_corr;

		timelib_do_rel_normalize(rt->invert ? one : two, rt);
	}

	if (trans) {
		timelib_time_offset_dtor(trans);
	}

	return rt;
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

timelib_time *timelib_add_wall(timelib_time *old_time, timelib_rel_time *interval)
{
	int bias = 1;
	timelib_time *t = timelib_time_clone(old_time);

	t->have_relative = 1;
	t->sse_uptodate = 0;

	if (interval->have_weekday_relative || interval->have_special_relative) {
		memcpy(&t->relative, interval, sizeof(timelib_rel_time));

		timelib_update_ts(t, NULL);
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

		t->sse += bias * timelib_hms_to_seconds(interval->h, interval->i, interval->s);
		t->us += interval->us * bias;
		timelib_do_normalize(t);
	}

	timelib_update_from_sse(t);
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

		t->sse -= bias * timelib_hms_to_seconds(interval->h, interval->i, interval->s);
		t->us -= interval->us * bias;
		timelib_do_normalize(t);
	}

	timelib_update_from_sse(t);
	if (t->zone_type == TIMELIB_ZONETYPE_ID) {
		timelib_set_timezone(t, t->tz_info);
	}
	t->have_relative = 0;

	return t;
}
