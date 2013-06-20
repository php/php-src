/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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
#include <math.h>

timelib_rel_time *timelib_diff(timelib_time *one, timelib_time *two)
{
	timelib_rel_time *rt;
	timelib_time *swp;
	timelib_sll dst_corr = 0 ,dst_h_corr = 0, dst_m_corr = 0;
	timelib_time one_backup, two_backup;

	rt = timelib_rel_time_ctor();
	rt->invert = 0;
	if (one->sse > two->sse) {
		swp = two;
		two = one;
		one = swp;
		rt->invert = 1;
	}

	/* Calculate correction for DST change over, but only if the TZ type is ID
	 * and it's the same */
	if (one->zone_type == 3 && two->zone_type == 3
		&& (strcmp(one->tz_info->name, two->tz_info->name) == 0)
		&& (one->z != two->z))
	{
		dst_corr = two->z - one->z;
		dst_h_corr = dst_corr / 3600;
		dst_m_corr = (dst_corr % 3600) / 60;
	}

	/* Save old TZ info */
	memcpy(&one_backup, one, sizeof(one_backup));
	memcpy(&two_backup, two, sizeof(two_backup));

    timelib_apply_localtime(one, 0);
    timelib_apply_localtime(two, 0);

	rt->y = two->y - one->y;
	rt->m = two->m - one->m;
	rt->d = two->d - one->d;
	rt->h = two->h - one->h;
	rt->i = two->i - one->i;
	rt->s = two->s - one->s;
	if (one_backup.dst == 0 && two_backup.dst == 1 && two->sse >= one->sse + 86400 - dst_corr) {
		rt->h += dst_h_corr;
		rt->i += dst_m_corr;
	}

	rt->days = abs(floor((one->sse - two->sse - (dst_h_corr * 3600) - (dst_m_corr * 60)) / 86400));

	timelib_do_rel_normalize(rt->invert ? one : two, rt);

	/* We need to do this after normalisation otherwise we can't get "24H" */
	if (one_backup.dst == 1 && two_backup.dst == 0 && two->sse >= one->sse + 86400) {
		if (two->sse < one->sse + 86400 - dst_corr) {
			rt->d--;
			rt->h = 24;
		} else {
			rt->h += dst_h_corr;
			rt->i += dst_m_corr;
		}
	}

	/* Restore old TZ info */
	memcpy(one, &one_backup, sizeof(one_backup));
	memcpy(two, &two_backup, sizeof(two_backup));

	return rt;
}

timelib_time *timelib_add(timelib_time *old_time, timelib_rel_time *interval)
{
	int bias = 1;
	timelib_time *t = timelib_time_clone(old_time);

	if (interval->have_weekday_relative || interval->have_special_relative) {
		memcpy(&t->relative, interval, sizeof(struct timelib_rel_time));
	} else {
		if (interval->invert) {
			bias = -1;
		}
		memset(&t->relative, 0, sizeof(struct timelib_rel_time));
		t->relative.y = interval->y * bias;
		t->relative.m = interval->m * bias;
		t->relative.d = interval->d * bias;
		t->relative.h = interval->h * bias;
		t->relative.i = interval->i * bias;
		t->relative.s = interval->s * bias;
	}
	t->have_relative = 1;
	t->sse_uptodate = 0;

	timelib_update_ts(t, NULL);

//	printf("%lld %lld %d\n", old_time->dst, t->dst, (t->sse - old_time->sse));
	/* Adjust for backwards DST changeover */
	if (old_time->dst == 1 && t->dst == 0 && !interval->y && !interval->m && !interval->d) {
		t->sse -= old_time->z;
		t->sse += t->z;
	}

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

	memset(&t->relative, 0, sizeof(struct timelib_rel_time));
	t->relative.y = 0 - (interval->y * bias);
	t->relative.m = 0 - (interval->m * bias);
	t->relative.d = 0 - (interval->d * bias);
	t->relative.h = 0 - (interval->h * bias);
	t->relative.i = 0 - (interval->i * bias);
	t->relative.s = 0 - (interval->s * bias);
	t->have_relative = 1;
	t->sse_uptodate = 0;

	timelib_update_ts(t, NULL);

	/* Adjust for backwards DST changeover */
	if (old_time->dst == 1 && t->dst == 0 && !interval->y && !interval->m && !interval->d) {
		t->sse -= old_time->z;
		t->sse += t->z;
	}
	/* Adjust for forwards DST changeover */
	if (old_time->dst == 0 && t->dst == 1 && !interval->y && !interval->m && !interval->d ) {
		t->sse -= old_time->z;
		t->sse += t->z;
	}

	timelib_update_from_sse(t);

	t->have_relative = 0;

	return t;
}
