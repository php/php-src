/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 MongoDB, Inc.
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

// This section adds the missing 'strndup' implementation on Windows.
#if TIMELIB_USE_BUILTIN_STRNDUP == 1
# include <stdlib.h>
# include <string.h>

/**
 * char* timelib_strndup(const char* s, size_t n)
 *
 * Returns a pointer to a copy of 's' with at most 'n' characters
 * in memory obtained from 'malloc', or 'NULL' if insufficient
 * memory was available.  The result is always 'NULL' terminated.
 */
static char* timelib_strndup(const char* s, size_t n)
{
	char* result;
	size_t len = strlen(s);

	if (n < len) {
		len = n;
	}

	result = (char*)malloc(len + 1);
	if (!result) {
		return 0;
	}

	result[len] = '\0';
	return (char*)memcpy(result, s, len);
}
#endif

/* Forwards declrations */
static timelib_posix_trans_info *timelib_posix_trans_info_ctor(void);
static void timelib_posix_trans_info_dtor(timelib_posix_trans_info* ts);

/* "<" [+-]? .+? ">" */
static char *read_description_numeric_abbr(char **ptr)
{
	const char *begin = *ptr + 1;

	// skip '<'
	(*ptr)++;

	while (**ptr != '\0' && **ptr != '>') {
		(*ptr)++;
	}

	if (**ptr == '\0') {
		return NULL;
	}

	if (**ptr == '>') {
		(*ptr)++;
	}

	// Abbreviation may not be empty
	if (*ptr - begin - 1 < 1) {
		return NULL;
	}

	return timelib_strndup(begin, *ptr - begin - 1);
}

/* [A-Z]+ */
static char *read_description_abbr(char **ptr)
{
	const char *begin = *ptr;

	// Find the end
	while ((**ptr >= 'A' && **ptr <= 'Z') || (**ptr >= 'a' && **ptr <= 'z')) {
		(*ptr)++;
	}

	// Abbreviation may not be empty
	if (*ptr - begin < 1) {
		return NULL;
	}

	return timelib_strndup(begin, *ptr - begin);
}

/* "<" [+-]? .+? ">" | [A-Z]+ */
static char *read_description(char **ptr)
{
	if (**ptr == '<') {
		return read_description_numeric_abbr(ptr);
	} else {
		return read_description_abbr(ptr);
	}
}

/* [+-]? */
static int read_sign(char **ptr)
{
	int bias = 1;

	if (**ptr == '+') {
		(*ptr)++;
	} else if (**ptr == '-') {
		bias = -1;
		(*ptr)++;
	}

	return bias;
}

/* [0-9]+ */
static timelib_sll read_number(char **ptr)
{
	const char *begin = *ptr;
	int acc = 0;

	// skip leading 0's
	while (**ptr == '0') {
		(*ptr)++;
	}

	while (**ptr >= '0' && **ptr <= '9') {
		acc = acc * 10;
		acc += (**ptr) - '0';
		(*ptr)++;
	}

	if (begin == *ptr) {
		return TIMELIB_UNSET;
	}

	return acc;
}

/* [+-]? [0-9]+ ( ":" [0-9]+ ( ":" [0-9]+ )? )? */
static timelib_sll read_offset(char **ptr)
{
	const char *begin;
	int bias = read_sign(ptr);
	int hours = 0;
	int minutes = 0;
	int seconds = 0;

	begin = *ptr;

	// read through to : or non-digit for hours
	hours = read_number(ptr);
	if (hours == TIMELIB_UNSET) {
		return hours;
	}

	// check for optional minutes
	if (**ptr == ':') {
		(*ptr)++; // skip ':'
		minutes = read_number(ptr);
		if (minutes == TIMELIB_UNSET) {
			return minutes;
		}
	}

	// check for optional seconds
	if (**ptr == ':') {
		(*ptr)++; // skip ':'
		seconds = read_number(ptr);
		if (seconds == TIMELIB_UNSET) {
			return seconds;
		}
	}

	if (begin == *ptr) {
		return TIMELIB_UNSET;
	}

	// multiplication with -1, because the offset in the identifier is the
	// 'wrong' way around as for example EST5 is UTC-5 (and not +5)
	return -1 * bias * (hours * 3600 + minutes * 60 + seconds);
}


// Mw.m.d
static timelib_posix_trans_info* read_trans_spec_mwd(char **ptr)
{
	timelib_posix_trans_info *tmp = timelib_posix_trans_info_ctor();

	tmp->type = TIMELIB_POSIX_TRANS_TYPE_MWD;

	// Skip 'M'
	(*ptr)++;

	tmp->mwd.month = read_number(ptr);
	if (tmp->mwd.month == TIMELIB_UNSET) {
		goto fail;
	}

	// check for '.' and skip it
	if (**ptr != '.') {
		goto fail;
	}
	(*ptr)++;

	tmp->mwd.week = read_number(ptr);
	if (tmp->mwd.week == TIMELIB_UNSET) {
		goto fail;
	}

	// check for '.' and skip it
	if (**ptr != '.') {
		goto fail;
	}
	(*ptr)++;

	tmp->mwd.dow = read_number(ptr);
	if (tmp->mwd.dow == TIMELIB_UNSET) {
		goto fail;
	}

	return tmp;

fail:
	timelib_posix_trans_info_dtor(tmp);
	return NULL;
}

// (Jn | n | Mw.m.d) ( /time )?
static timelib_posix_trans_info* read_transition_spec(char **ptr)
{
	timelib_posix_trans_info *tmp;

	if (**ptr == 'M') {
		tmp = read_trans_spec_mwd(ptr);
		if (!tmp) {
			return NULL;
		}
	} else {
		tmp = timelib_posix_trans_info_ctor();

		if (**ptr == 'J') {
			tmp->type = TIMELIB_POSIX_TRANS_TYPE_JULIAN_NO_FEB29;
			(*ptr)++;
		}

		tmp->days = read_number(ptr);
		if (tmp->days == TIMELIB_UNSET) {
			goto fail;
		}
	}

	// Check for the optional hour
	if (**ptr == '/') {
		(*ptr)++;
		tmp->hour = read_offset(ptr);
		if (tmp->hour == TIMELIB_UNSET) {
			goto fail;
		}
		// as the bias for normal offsets = -1, we need to reverse it here
		tmp->hour = -tmp->hour;
	}

	return tmp;

fail:
	timelib_posix_trans_info_dtor(tmp);
	return NULL;
}

static timelib_posix_trans_info* timelib_posix_trans_info_ctor(void)
{
	timelib_posix_trans_info *tmp;

	tmp = timelib_calloc(1, sizeof(timelib_posix_trans_info));
	tmp->type = TIMELIB_POSIX_TRANS_TYPE_JULIAN_FEB29;
	tmp->hour = 2 * 3600;

	return tmp;
}

static void timelib_posix_trans_info_dtor(timelib_posix_trans_info* ts)
{
	timelib_free(ts);
}

void timelib_posix_str_dtor(timelib_posix_str *ps)
{
	if (ps->std) {
		timelib_free(ps->std);
	}
	if (ps->dst) {
		timelib_free(ps->dst);
	}
	if (ps->dst_begin) {
		timelib_posix_trans_info_dtor(ps->dst_begin);
	}
	if (ps->dst_end) {
		timelib_posix_trans_info_dtor(ps->dst_end);
	}

	timelib_free(ps);
}

timelib_posix_str* timelib_parse_posix_str(const char *posix)
{
	timelib_posix_str *tmp = timelib_calloc(1, sizeof(timelib_posix_str));
	char *ptr = (char*) posix;

	// read standard description (ie. EST or <-03>)
	tmp->std = read_description(&ptr);
	if (!tmp->std) {
		timelib_posix_str_dtor(tmp);
		return NULL;
	}

	// read required offset
	tmp->std_offset = read_offset(&ptr);
	if (tmp->std_offset == TIMELIB_UNSET) {
		timelib_posix_str_dtor(tmp);
		return NULL;
	}

	// if we're at the end return, otherwise we'll continue to try to parse
	// the dst abbreviation and spec
	if (*ptr == '\0') {
		return tmp;
	}

	// assume dst is there, and initialise offset
	tmp->dst_offset = tmp->std_offset + 3600;

	tmp->dst = read_description(&ptr);
	if (!tmp->dst) {
		timelib_posix_str_dtor(tmp);
		return NULL;
	}

	// if we have a "," here, then the dst offset is the standard offset +
	// 3600 seconds, otherwise, try to parse the dst offset
	if (*ptr != ',' && *ptr != '\0') {
		tmp->dst_offset = read_offset(&ptr);
		if (tmp->dst_offset == TIMELIB_UNSET) {
			timelib_posix_str_dtor(tmp);
			return NULL;
		}
	}

	// if we *don't* have a "," here, we're missing the dst transitions
	// ,start[/time],end[/time]
	if (*ptr != ',') {
		timelib_posix_str_dtor(tmp);
		return NULL;
	}

	ptr++; // skip ','

	// start[/time]
	tmp->dst_begin = read_transition_spec(&ptr);
	if (!tmp->dst_begin) {
		timelib_posix_str_dtor(tmp);
		return NULL;
	}

	// if we *don't* have a "," here, we're missing the dst end transition
	// ,end[/time]
	if (*ptr != ',') {
		timelib_posix_str_dtor(tmp);
		return NULL;
	}

	ptr++; // skip ','

	// end[/time]
	tmp->dst_end = read_transition_spec(&ptr);
	if (!tmp->dst_end) {
		timelib_posix_str_dtor(tmp);
		return NULL;
	}

	// make sure there is no trailing data
	if (*ptr != '\0') {
		timelib_posix_str_dtor(tmp);
		return NULL;
	}

	return tmp;
}

static const int month_lengths[2][MONTHS_PER_YEAR] = {
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }, // normal year
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }  // leap year
};

/* This function is adapted from the 'localtime.c' function 'transtime' as bundled with the 'tzcode' project
 * from IANA, and is public domain licensed. */
static timelib_sll calc_transition(timelib_posix_trans_info *psi, timelib_sll year)
{
	int leap_year = timelib_is_leap(year);

	switch (psi->type) {
		case TIMELIB_POSIX_TRANS_TYPE_JULIAN_NO_FEB29: {
			timelib_sll value = (psi->days - 1);

			if (leap_year && psi->days >= 60) {
				value++;
			}

			return value * SECS_PER_DAY;
		}

		case TIMELIB_POSIX_TRANS_TYPE_JULIAN_FEB29: {
			return psi->days * SECS_PER_DAY;
		}

		case TIMELIB_POSIX_TRANS_TYPE_MWD: {
			/*
			 * Mm.n.d - nth "dth day" of month m.
			 */

			int i, d, m1, yy0, yy1, yy2, dow;
			timelib_sll value = 0;

			/* Use Zeller's Congruence to get day-of-week of first day of
			 * month. */
			m1 = (psi->mwd.month + 9) % 12 + 1;
			yy0 = (psi->mwd.month <= 2) ? (year - 1) : year;
			yy1 = yy0 / 100;
			yy2 = yy0 % 100;
			dow = ((26 * m1 - 2) / 10 + 1 + yy2 + yy2 / 4 + yy1 / 4 - 2 * yy1) % 7;
			if (dow < 0) {
				dow += DAYS_PER_WEEK;
			}

			/* "dow" is the day-of-week of the first day of the month. Get the
			 * day-of-month (zero-origin) of the first "dow" day of the month. */
			d = psi->mwd.dow - dow;
			if (d < 0) {
				d += DAYS_PER_WEEK;
			}
			for (i = 1; i < psi->mwd.week; ++i) {
				if (d + DAYS_PER_WEEK >= month_lengths[leap_year][psi->mwd.month - 1]) {
					break;
				}
				d += DAYS_PER_WEEK;
			}

			/* "d" is the day-of-month (zero-origin) of the day we want. */
			value = d * SECS_PER_DAY;
			for (i = 0; i < psi->mwd.month - 1; ++i) {
				value += month_lengths[leap_year][i] * SECS_PER_DAY;
			}

			return value;
		} break;
	}

	return 0;
}

static timelib_sll count_leap_years(timelib_sll y)
{
	/* Because we want this for Jan 1, the leap day hasn't happend yet, so
	 * subtract one of year before we calculate */
	y--;

	return (y/4) - (y/100) + (y/400);
}

timelib_sll timelib_ts_at_start_of_year(timelib_sll year)
{
	timelib_sll epoch_leap_years = count_leap_years(1970);
	timelib_sll current_leap_years = count_leap_years(year);

	return SECS_PER_DAY * (
		((year-1970) * DAYS_PER_YEAR)
		+ current_leap_years
		- epoch_leap_years
	);
}

void timelib_get_transitions_for_year(timelib_tzinfo *tz, timelib_sll year, timelib_posix_transitions *transitions)
{
	timelib_sll trans_begin; /* Since start of the year */
	timelib_sll trans_end;
	timelib_sll year_begin_ts = timelib_ts_at_start_of_year(year);

	trans_begin = year_begin_ts;
	trans_begin += calc_transition(tz->posix_info->dst_begin, year);
	trans_begin += tz->posix_info->dst_begin->hour;
	trans_begin -= tz->posix_info->std_offset;

	trans_end = year_begin_ts;
	trans_end += calc_transition(tz->posix_info->dst_end, year);
	trans_end += tz->posix_info->dst_end->hour;
	trans_end -= tz->posix_info->dst_offset;

	if (trans_begin < trans_end) {
		transitions->times[transitions->count  ] = trans_begin;
		transitions->times[transitions->count+1] = trans_end;
		transitions->types[transitions->count  ] = tz->posix_info->type_index_dst_type;
		transitions->types[transitions->count+1] = tz->posix_info->type_index_std_type;
	} else {
		transitions->times[transitions->count+1] = trans_begin;
		transitions->times[transitions->count  ] = trans_end;
		transitions->types[transitions->count+1] = tz->posix_info->type_index_dst_type;
		transitions->types[transitions->count  ] = tz->posix_info->type_index_std_type;
	}

	transitions->count += 2;
}

ttinfo* timelib_fetch_posix_timezone_offset(timelib_tzinfo *tz, timelib_sll ts, timelib_sll *transition_time)
{
	timelib_sll               year;
	timelib_time              dummy;
	timelib_posix_transitions transitions = { 0 };
	size_t            i;

	/* If there is no second (dst_end) information, the UTC offset is valid for the whole year, so no need to
	 * do clever logic */
	if (!tz->posix_info->dst_end) {
		if (transition_time) {
			*transition_time = tz->trans[tz->bit64.timecnt - 1];
		}
		return &(tz->type[tz->posix_info->type_index_std_type]);
	}

	/* Find 'year' (UTC) for 'ts' */
	timelib_unixtime2gmt(&dummy, ts);
	year = dummy.y;

	/* Calculate transition times for 'year-1', 'year', and 'year+1' */
	timelib_get_transitions_for_year(tz, year - 1, &transitions);
	timelib_get_transitions_for_year(tz, year,     &transitions);
	timelib_get_transitions_for_year(tz, year + 1, &transitions);

	/* Check where the 'ts' falls in the 4 transitions */
	for (i = 1; i < transitions.count; i++) {
		if (ts < transitions.times[i]) {
			if (transition_time) {
				*transition_time = transitions.times[i - 1];
			}
			return &(tz->type[transitions.types[i - 1]]);
		}
	}

	return NULL;
}
