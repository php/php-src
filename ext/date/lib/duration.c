/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2026 Derick Rethans
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

int timelib_duration_ctor_static(
	timelib_duration *duration,
	uint64_t          seconds,
	uint32_t          nanoseconds,
	bool              negative
) {
	if (seconds > MAX_DURATION_SECONDS) {
		return TIMELIB_ERROR_SECONDS_OUT_OF_RANGE;
	}
	if (nanoseconds >= NSECS_PER_SEC) {
		return TIMELIB_ERROR_NANOSECONDS_OUT_OF_RANGE;
	}

	duration->seconds = seconds;
	duration->nanoseconds = nanoseconds;

	if (seconds != 0 || nanoseconds != 0) {
		duration->negative = negative;
	} else {
		duration->negative = false;
	}

	return TIMELIB_ERROR_NO_ERROR;
}

timelib_duration *timelib_duration_ctor(
	uint64_t               seconds,
	uint32_t               nanoseconds,
	bool                   negative,
	int                   *error_code
) {
	timelib_duration *tmp = calloc(1, sizeof(timelib_duration));

	*error_code = timelib_duration_ctor_static(tmp, seconds, nanoseconds, negative);

	if (*error_code != TIMELIB_ERROR_NO_ERROR) {
		free(tmp);
		return NULL;
	}

	return tmp;
}

timelib_duration *timelib_duration_create_from_iso8601string(
	const char *string,
	int        *error_code
) {
	timelib_time            *b = NULL, *e = NULL;
	timelib_rel_time        *p = NULL;
	int                      r = -1;
	timelib_error_container *errors;
	timelib_duration        *new_duration = NULL;

	timelib_strtointerval(string, strlen(string), &b, &e, &p, &r, &errors);

	if (errors->error_count > 0 || errors->warning_count > 0) {
		*error_code = TIMELIB_ERROR_ISO8601_DURATION_PARSE_FAILURE;
		goto free_elements;
	}

	if (!p) {
		*error_code = TIMELIB_ERROR_DURATION_MISSING_PERIOD;
		goto free_elements;
	}

	if (b != NULL || e != NULL || r != -1) {
		*error_code = TIMELIB_ERROR_DURATION_ONLY_PERIOD_ALLOWED;
		goto free_elements;
	}

	if (p->y > 0 || p->m >0 || p->d > 0) {
		*error_code = TIMELIB_ERROR_DURATION_DAYS_FOUND;
		goto free_elements;
	}

	new_duration = calloc(1, sizeof(timelib_duration));

	*error_code = timelib_duration_ctor_static(new_duration, p->h * 3600 + p->i * 60 + p->s, 0, false);

	if (*error_code != TIMELIB_ERROR_NO_ERROR) {
		free(new_duration);
		new_duration = NULL;
	}

free_elements:
	if (b) {
		timelib_time_dtor(b);
	}
	if (e) {
		timelib_time_dtor(e);
	}
	if (p) {
		timelib_rel_time_dtor(p);
	}
	timelib_error_container_dtor(errors);

	return new_duration;
}

void timelib_duration_dtor(timelib_duration *duration)
{
	free(duration);
}

static int timelib_duration_add_abs_internal(
	timelib_duration       *new_duration,
	const timelib_duration *original,
	const timelib_duration *additional
) {
	uint64_t seconds = original->seconds + additional->seconds;
	uint32_t nanoseconds = original->nanoseconds + additional->nanoseconds;

	if (nanoseconds >= NSECS_PER_SEC) {
		seconds++;
		nanoseconds -= NSECS_PER_SEC;
	}

	return timelib_duration_ctor_static(new_duration, seconds, nanoseconds, original->negative);
}


static int timelib_duration_sub_abs_internal(
	timelib_duration       *new_duration,
	const timelib_duration *original,
	const timelib_duration *minus
) {
	uint64_t seconds = original->seconds - minus->seconds;
	int32_t nanoseconds = (int32_t)original->nanoseconds - (int32_t)minus->nanoseconds;

	if (nanoseconds < 0) {
		seconds--;
		nanoseconds += NSECS_PER_SEC;
	}

	return timelib_duration_ctor_static(new_duration, seconds, nanoseconds, original->negative);
}

static int timelib_duration_null_abs_internal(timelib_duration *new_duration)
{
	new_duration->negative = false;
	new_duration->seconds = 0;
	new_duration->nanoseconds = 0;

	return TIMELIB_ERROR_NO_ERROR;
}


int timelib_duration_add_static(
	timelib_duration       *new_duration,
	const timelib_duration *original,
	const timelib_duration *additional
) {
	int c = 0;

	/* ++ / -- */
	if (original->negative == additional->negative) {
		return timelib_duration_add_abs_internal(new_duration, original, additional);
	}

	/* +- / -+ */
	c = timelib_duration_abs_compare(original, additional);

	switch (c)
	{
		case -1:
			/* additional has the larger value */
			return timelib_duration_sub_abs_internal(new_duration, additional, original);

		case 0:
			return timelib_duration_null_abs_internal(new_duration);

		case 1:
			/* original has the larger value */
			return timelib_duration_sub_abs_internal(new_duration, original, additional);
	}

	/* Should not be reachable due to semantics of timelib_duration_abs_compare() */
	return TIMELIB_ERROR_NO_ERROR;
}

timelib_duration *timelib_duration_add(
	const timelib_duration *original,
	const timelib_duration *additional,
	int                    *error_code
) {
	timelib_duration *tmp = calloc(1, sizeof(timelib_duration));

	*error_code = timelib_duration_add_static(tmp, original, additional);

	if (*error_code != TIMELIB_ERROR_NO_ERROR) {
		free(tmp);
		return NULL;
	}

	return tmp;
}


int timelib_duration_sub_static(
	timelib_duration       *new_duration,
	const timelib_duration *original,
	const timelib_duration *minus
) {
	int c = 0;

	/* +- / -+ */
	if (original->negative != minus->negative) {
		return timelib_duration_add_abs_internal(new_duration, original, minus);
	}

	/* ++ / -- */
	c = timelib_duration_abs_compare(original, minus);

	switch (c)
	{
		case -1: {
			/* minus has the larger value */
			timelib_duration tmp_duration;

			int result = timelib_duration_sub_abs_internal(&tmp_duration, minus, original);
			if (result != TIMELIB_ERROR_NO_ERROR) {
				return result;
			}

			return timelib_duration_negate_static(new_duration, &tmp_duration);
		}

		case 0:
			return timelib_duration_null_abs_internal(new_duration);

		case 1:
			/* original has the larger value */
			return timelib_duration_sub_abs_internal(new_duration, original, minus);
	}

	/* Should not be reachable due to semantics of comparisons above */
	return TIMELIB_ERROR_NO_ERROR;
}

timelib_duration *timelib_duration_sub(
	const timelib_duration *original,
	const timelib_duration *minus,
	int                    *error_code
) {
	timelib_duration *tmp = calloc(1, sizeof(timelib_duration));

	*error_code = timelib_duration_sub_static(tmp, original, minus);

	if (*error_code != TIMELIB_ERROR_NO_ERROR) {
		free(tmp);
		return NULL;
	}

	return tmp;
}

int timelib_duration_mul_static(
	timelib_duration       *new_duration,
	const timelib_duration *original,
	uint64_t                factor
) {
	uint64_t seconds, extra_seconds, nanoseconds;

	if (factor == 0) {
		return timelib_duration_null_abs_internal(new_duration);
	}

	if (original->seconds > UINT64_MAX / factor) {
		return TIMELIB_ERROR_OVERFLOW;
	}

	seconds = original->seconds * factor;

	/* Calculate the number of whole seconds in the nanoseconds product.
	 *
	 * extra_seconds is guaranteed to be smaller than factor, because
	 * original->nanoseconds is smaller than NSECS_PER_SEC. */
	extra_seconds = original->nanoseconds * (factor / NSECS_PER_SEC);

	/* This cannot overflow either, because NSECS_PER_SEC * NSECS_PER_SEC fits uint64_t.
	 *
	 * (nanoseconds * factor) % NSECS_PER_SEC is mathematically equivalent
	 * to (nanoseconds * (factor % NSECS_PER_SEC)) % NSECS_PER_SEC. */
	nanoseconds = original->nanoseconds * (factor % NSECS_PER_SEC);
	extra_seconds += nanoseconds / NSECS_PER_SEC;
	nanoseconds %= NSECS_PER_SEC;

	if (UINT64_MAX - extra_seconds < seconds) {
		return TIMELIB_ERROR_OVERFLOW;
	}

	seconds += extra_seconds;

	return timelib_duration_ctor_static(new_duration, seconds, nanoseconds, original->negative);
}

timelib_duration *timelib_duration_mul(
	const timelib_duration *original,
	uint64_t                factor,
	int                    *error_code
) {
	timelib_duration *tmp = calloc(1, sizeof(timelib_duration));

	*error_code = timelib_duration_mul_static(tmp, original, factor);

	if (*error_code != TIMELIB_ERROR_NO_ERROR) {
		free(tmp);
		return NULL;
	}

	return tmp;
}

int timelib_duration_div_static(
	timelib_duration       *new_duration,
	const timelib_duration *original,
	uint64_t                divisor
) {
	uint64_t seconds;
	uint64_t nanoseconds;

	if (divisor < 1) {
		return TIMELIB_ERROR_DIVISION_BY_ZERO;
	}

	seconds = original->seconds / divisor;
	nanoseconds = original->nanoseconds + ((original->seconds % divisor) * NSECS_PER_SEC);
	nanoseconds /= divisor;

	return timelib_duration_ctor_static(new_duration, seconds, nanoseconds, original->negative);
}

timelib_duration *timelib_duration_div(
	const timelib_duration *original,
	uint64_t                divisor,
	int                    *error_code
) {
	timelib_duration *tmp = calloc(1, sizeof(timelib_duration));

	*error_code = timelib_duration_div_static(tmp, original, divisor);

	if (*error_code != TIMELIB_ERROR_NO_ERROR) {
		free(tmp);
		return NULL;
	}

	return tmp;
}

int timelib_duration_negate_static(timelib_duration *new_duration, const timelib_duration *original)
{
	return timelib_duration_ctor_static(new_duration, original->seconds, original->nanoseconds, !original->negative);
}

timelib_duration *timelib_duration_negate(const timelib_duration *original, int *error_code)
{
	timelib_duration *tmp = calloc(1, sizeof(timelib_duration));

	*error_code = timelib_duration_negate_static(tmp, original);

	if (*error_code != TIMELIB_ERROR_NO_ERROR) {
		free(tmp);
		return NULL;
	}

	return tmp;
}

int timelib_duration_abs_compare(const timelib_duration *one, const timelib_duration *two)
{
	if (one->seconds < two->seconds) {
		return -1;
	}
	if (one->seconds > two->seconds) {
		return 1;
	}

	if (one->nanoseconds < two->nanoseconds) {
		return -1;
	}
	if (one->nanoseconds > two->nanoseconds) {
		return 1;
	}

	return 0;
}

int timelib_duration_compare(const timelib_duration *one, const timelib_duration *two)
{
	if (one->negative && !two->negative) {
		return -1;
	}
	if (!one->negative && two->negative) {
		return 1;
	}

	if (one->negative && two->negative) {
		return timelib_duration_abs_compare(two, one);
	}

	return timelib_duration_abs_compare(one, two);
}
