/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Derick Rethans <derick@derickrethans.nl>                    |
   |          Tim Düsterhus <timwolla@php.net>                            |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "Zend/zend_exceptions.h"

#include "php_date.h"
#include "php_time.h"

#define NANOS_IN_SEC 1000000000
#define NANOS_IN_MICRO 1000
#define MICROS_IN_SEC 1000000
#define NANOS_IN_MILLI 1000000
#define MILLIS_IN_SEC 1000

ZEND_STATIC_ASSERT(NANOS_IN_MICRO * MICROS_IN_SEC == NANOS_IN_SEC, "");
ZEND_STATIC_ASSERT(NANOS_IN_MILLI * MILLIS_IN_SEC == NANOS_IN_SEC, "");

#define Z_PARAM_ULONG(l) { \
		zend_long __##l; \
		Z_PARAM_LONG(__##l); \
		if (__##l < 0) { \
			zend_argument_value_error(_i, "must be greater than or equal to 0"); \
			_error_code = ZPP_ERROR_FAILURE; \
			break; \
		} \
		l = __##l; \
	}

ZEND_COLD static void throw_out_of_range_exception(void)
{
#if SIZEOF_ZEND_LONG != 4
	zend_throw_exception(php_date_ce_time_timeexception, "The maximum representable range is 9_223_372_035 seconds (roughly 292 years)", 0);
#else
	zend_throw_exception(php_date_ce_time_timeexception, "The maximum representable range is 2_147_483_647 seconds (roughly 68 years)", 0);
#endif
}

ZEND_COLD static void throw_timelib_error(int error)
{
	switch (error) {
		case TIMELIB_ERROR_SECONDS_OUT_OF_RANGE:
			throw_out_of_range_exception();
			break;
		case TIMELIB_ERROR_DIVISION_BY_ZERO:
			zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Division by zero");
			break;
		case TIMELIB_ERROR_ISO8601_DURATION_PARSE_FAILURE:
		case TIMELIB_ERROR_DURATION_MISSING_PERIOD:
		case TIMELIB_ERROR_DURATION_ONLY_PERIOD_ALLOWED:
		case TIMELIB_ERROR_DURATION_DAYS_FOUND:
			zend_throw_exception(php_date_ce_time_timeexception, timelib_get_error_message(error), 0);
			break;
		default:
			/* This should be unreachable in practice. */
			zend_throw_exception_ex(php_date_ce_time_timeexception, 0, "Failed to create a Time\\Duration: %s", timelib_get_error_message(error));
			break;
	}
}

static inline php_date_time_duration *create_duration_shell(zval *target)
{
	object_init_ex(target, php_date_ce_time_duration);

	return Z_DATE_TIME_DURATION_P(target);
}

ZEND_ATTRIBUTE_NODISCARD static inline zend_result sync_properties(php_date_time_duration *object)
{
	if (
		/* Check if the duration would overflow the $seconds property. */
		object->duration.seconds > ((uint64_t)ZEND_LONG_MAX)
		/* This constraint is an explicit part of PHP's API: It is the maximum $seconds
		 * value that allows storing the entire duration as a single int64_t counting
		 * nanoseconds, which might be desirable in the future when userland `int` is
		 * consistently 64 bits.
		 *
		 * While it is currently also enforced by timelib, this might change
		 * in a future version of timelib, thus we also enforce it manually. */
		|| object->duration.seconds > UINT64_C(9223372035)
	) {
		throw_out_of_range_exception();
		return FAILURE;
	}

	ZEND_ASSERT(Z_ISUNDEF_P(OBJ_PROP_NUM(&object->std, 0)));
	ZEND_ASSERT(Z_ISUNDEF_P(OBJ_PROP_NUM(&object->std, 1)));
	ZEND_ASSERT(Z_ISUNDEF_P(OBJ_PROP_NUM(&object->std, 2)));

	ZVAL_LONG(OBJ_PROP_NUM(&object->std, 0), object->duration.seconds);
	Z_PROP_FLAG_P(OBJ_PROP_NUM(&object->std, 0)) &= ~(IS_PROP_UNINIT|IS_PROP_REINITABLE);
	ZVAL_LONG(OBJ_PROP_NUM(&object->std, 1), object->duration.nanoseconds);
	Z_PROP_FLAG_P(OBJ_PROP_NUM(&object->std, 1)) &= ~(IS_PROP_UNINIT|IS_PROP_REINITABLE);
	ZVAL_BOOL(OBJ_PROP_NUM(&object->std, 2), object->duration.negative);
	Z_PROP_FLAG_P(OBJ_PROP_NUM(&object->std, 2)) &= ~(IS_PROP_UNINIT|IS_PROP_REINITABLE);

	return SUCCESS;
}

ZEND_ATTRIBUTE_NODISCARD static zend_result create_duration(zval *target, zend_ulong seconds, zend_ulong nanoseconds)
{
	ZEND_ASSERT(nanoseconds < NANOS_IN_SEC);

	if (EXPECTED(DATEG(duration_cache))) {
		php_date_time_duration *cached = php_date_time_duration_from_obj(DATEG(duration_cache));
		ZEND_ASSERT(!cached->duration.negative);

		if (cached->duration.seconds == seconds && cached->duration.nanoseconds == nanoseconds) {
			ZVAL_OBJ_COPY(target, &cached->std);
			return SUCCESS;
		}
	}

	php_date_time_duration *obj = create_duration_shell(target);

	int error = timelib_duration_ctor_static(&obj->duration, seconds, nanoseconds, /* negative */ false);
	if (error != TIMELIB_ERROR_NO_ERROR) {
		throw_timelib_error(error);
		return FAILURE;
	}

	if (sync_properties(obj) == FAILURE) {
		return FAILURE;
	}

	if (DATEG(duration_cache)) {
		zend_object_release(DATEG(duration_cache));
	}
	GC_ADDREF(&obj->std);
	DATEG(duration_cache) = &obj->std;

	return SUCCESS;
}

PHP_METHOD(Time_Duration, __construct)
{
	zend_throw_error(NULL, "Cannot directly construct Time\\Duration, use Time\\Duration::from*() methods instead");
}

PHP_METHOD(Time_Duration, fromSeconds)
{
	zend_ulong seconds;
	zend_ulong nanoseconds = 0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ULONG(seconds);
		Z_PARAM_OPTIONAL;
		Z_PARAM_ULONG(nanoseconds);
	ZEND_PARSE_PARAMETERS_END();

	if (nanoseconds >= NANOS_IN_SEC) {
		zend_argument_value_error(2, "must be less than 1_000_000_000");
		RETURN_THROWS();
	}

	if (create_duration(return_value, seconds, nanoseconds) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Time_Duration, fromNanoseconds)
{
	zend_ulong nanoseconds;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ULONG(nanoseconds);
	ZEND_PARSE_PARAMETERS_END();

	zend_ulong seconds = nanoseconds / NANOS_IN_SEC;
	nanoseconds %= NANOS_IN_SEC;

	if (create_duration(return_value, seconds, nanoseconds) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Time_Duration, fromMicroseconds)
{
	zend_ulong microseconds;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ULONG(microseconds);
	ZEND_PARSE_PARAMETERS_END();

	zend_ulong seconds = microseconds / MICROS_IN_SEC;
	zend_ulong nanoseconds = (microseconds % MICROS_IN_SEC) * NANOS_IN_MICRO;

	if (create_duration(return_value, seconds, nanoseconds) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Time_Duration, fromMilliseconds)
{
	zend_ulong milliseconds;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ULONG(milliseconds);
	ZEND_PARSE_PARAMETERS_END();

	zend_ulong seconds = milliseconds / MILLIS_IN_SEC;
	zend_ulong nanoseconds = (milliseconds % MILLIS_IN_SEC) * NANOS_IN_MILLI;

	if (create_duration(return_value, seconds, nanoseconds) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Time_Duration, fromMinutes)
{
	zend_ulong minutes;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ULONG(minutes);
	ZEND_PARSE_PARAMETERS_END();

	if (minutes > (ZEND_ULONG_MAX / 60)) {
		throw_out_of_range_exception();
		RETURN_THROWS();
	}

	if (create_duration(return_value, minutes * 60, /* nanoseconds */ 0) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Time_Duration, fromHours)
{
	zend_ulong hours;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ULONG(hours);
	ZEND_PARSE_PARAMETERS_END();

	if (hours > (ZEND_ULONG_MAX / 3600)) {
		throw_out_of_range_exception();
		RETURN_THROWS();
	}

	if (create_duration(return_value, hours * 3600, /* nanoseconds */ 0) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Time_Duration, fromIso8601DurationString)
{
	zend_string *specification;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(specification);
	ZEND_PARSE_PARAMETERS_END();

	int error;
	timelib_duration *d = timelib_duration_create_from_iso8601string(ZSTR_VAL(specification), &error);
	if (error != TIMELIB_ERROR_NO_ERROR) {
		throw_timelib_error(error);
		RETURN_THROWS();
	}

	php_date_time_duration *new = create_duration_shell(return_value);
	new->duration = *d;
	timelib_duration_dtor(d);

	if (sync_properties(new) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Time_Duration, negate)
{
	const php_date_time_duration *original = Z_DATE_TIME_DURATION_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	php_date_time_duration *new = create_duration_shell(return_value);

	int error = timelib_duration_negate_static(&new->duration, &original->duration);
	if (error != TIMELIB_ERROR_NO_ERROR) {
		throw_timelib_error(error);
		RETURN_THROWS();
	}

	if (sync_properties(new) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Time_Duration, absolute)
{
	const php_date_time_duration *original = Z_DATE_TIME_DURATION_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	if (!original->duration.negative) {
		RETURN_COPY(ZEND_THIS);
	}

	if (create_duration(return_value, original->duration.seconds, original->duration.nanoseconds) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Time_Duration, add)
{
	const php_date_time_duration *original = Z_DATE_TIME_DURATION_P(ZEND_THIS);

	const php_date_time_duration *additional;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DATE_TIME_DURATION(additional);
	ZEND_PARSE_PARAMETERS_END();

	php_date_time_duration *new = create_duration_shell(return_value);

	int error = timelib_duration_add_static(&new->duration, &original->duration, &additional->duration);
	if (error != TIMELIB_ERROR_NO_ERROR) {
		throw_timelib_error(error);
		RETURN_THROWS();
	}

	if (sync_properties(new) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Time_Duration, sub)
{
	const php_date_time_duration *original = Z_DATE_TIME_DURATION_P(ZEND_THIS);

	const php_date_time_duration *minus;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DATE_TIME_DURATION(minus);
	ZEND_PARSE_PARAMETERS_END();

	php_date_time_duration *new = create_duration_shell(return_value);

	int error = timelib_duration_sub_static(&new->duration, &original->duration, &minus->duration);
	if (error != TIMELIB_ERROR_NO_ERROR) {
		throw_timelib_error(error);
		RETURN_THROWS();
	}

	if (sync_properties(new) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Time_Duration, multiplyBy)
{
	const php_date_time_duration *original = Z_DATE_TIME_DURATION_P(ZEND_THIS);

	zend_ulong factor;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ULONG(factor);
	ZEND_PARSE_PARAMETERS_END();

	php_date_time_duration *new = create_duration_shell(return_value);

	int error = timelib_duration_mul_static(&new->duration, &original->duration, factor);
	if (error != TIMELIB_ERROR_NO_ERROR) {
		throw_timelib_error(error);
		RETURN_THROWS();
	}

	if (sync_properties(new) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Time_Duration, divideBy)
{
	const php_date_time_duration *original = Z_DATE_TIME_DURATION_P(ZEND_THIS);

	zend_ulong divisor;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ULONG(divisor);
	ZEND_PARSE_PARAMETERS_END();

	php_date_time_duration *new = create_duration_shell(return_value);

	int error = timelib_duration_div_static(&new->duration, &original->duration, divisor);
	if (error != TIMELIB_ERROR_NO_ERROR) {
		throw_timelib_error(error);
		RETURN_THROWS();
	}

	if (sync_properties(new) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Time_Duration, compare)
{
	const php_date_time_duration *a;
	const php_date_time_duration *b;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_DATE_TIME_DURATION(a);
		Z_PARAM_DATE_TIME_DURATION(b);
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(timelib_duration_compare(&a->duration, &b->duration));
}
