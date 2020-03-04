/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

#include "php.h"
#include "php_streams.h"
#include "php_main.h"
#include "php_globals.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_versioning.h"
#include "ext/standard/php_math.h"
#include "php_date.h"
#include "zend_interfaces.h"
#include "lib/timelib.h"
#include "lib/timelib_private.h"
#ifndef PHP_WIN32
#include <time.h>
#else
#include "win32/time.h"
#endif

#ifdef PHP_WIN32
static __inline __int64 php_date_llabs( __int64 i ) { return i >= 0? i: -i; }
#elif defined(__GNUC__) && __GNUC__ < 3
static __inline __int64_t php_date_llabs( __int64_t i ) { return i >= 0 ? i : -i; }
#else
static inline long long php_date_llabs( long long i ) { return i >= 0 ? i : -i; }
#endif

#ifdef PHP_WIN32
#define DATE_I64_BUF_LEN 65
# define DATE_I64A(i, s, len) _i64toa_s(i, s, len, 10)
# define DATE_A64I(i, s) i = _atoi64(s)
#else
#define DATE_I64_BUF_LEN 65
# define DATE_I64A(i, s, len) \
	do { \
		int st = snprintf(s, len, "%lld", i); \
		s[st] = '\0'; \
	} while (0);
#ifdef HAVE_ATOLL
# define DATE_A64I(i, s) i = atoll(s)
#else
# define DATE_A64I(i, s) i = strtoll(s, NULL, 10)
#endif
#endif

PHPAPI time_t php_time()
{
#ifdef HAVE_GETTIMEOFDAY
    struct timeval tm;

    if (UNEXPECTED(gettimeofday(&tm, NULL) != SUCCESS)) {
        /* fallback, can't reasonably happen */
        return time(NULL);
    }

    return tm.tv_sec;
#else
    return time(NULL);
#endif
}

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_date, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmdate, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_idate, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_strtotime, 0, 0, 1)
	ZEND_ARG_INFO(0, time)
	ZEND_ARG_INFO(0, now)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mktime, 0, 0, 0)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, sec)
	ZEND_ARG_INFO(0, mon)
	ZEND_ARG_INFO(0, day)
	ZEND_ARG_INFO(0, year)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmmktime, 0, 0, 0)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, sec)
	ZEND_ARG_INFO(0, mon)
	ZEND_ARG_INFO(0, day)
	ZEND_ARG_INFO(0, year)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_checkdate, 0)
	ZEND_ARG_INFO(0, month)
	ZEND_ARG_INFO(0, day)
	ZEND_ARG_INFO(0, year)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_strftime, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmstrftime, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_time, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_localtime, 0, 0, 0)
	ZEND_ARG_INFO(0, timestamp)
	ZEND_ARG_INFO(0, associative_array)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_getdate, 0, 0, 0)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_date_default_timezone_set, 0)
	ZEND_ARG_INFO(0, timezone_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_date_default_timezone_get, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_sunrise, 0, 0, 1)
	ZEND_ARG_INFO(0, time)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, latitude)
	ZEND_ARG_INFO(0, longitude)
	ZEND_ARG_INFO(0, zenith)
	ZEND_ARG_INFO(0, gmt_offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_sunset, 0, 0, 1)
	ZEND_ARG_INFO(0, time)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, latitude)
	ZEND_ARG_INFO(0, longitude)
	ZEND_ARG_INFO(0, zenith)
	ZEND_ARG_INFO(0, gmt_offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_date_sun_info, 0)
	ZEND_ARG_INFO(0, time)
	ZEND_ARG_INFO(0, latitude)
	ZEND_ARG_INFO(0, longitude)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_create, 0, 0, 0)
	ZEND_ARG_INFO(0, time)
	ZEND_ARG_INFO(0, timezone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_create_from_format, 0, 0, 2)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, time)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeZone, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_parse, 0, 0, 1)
	ZEND_ARG_INFO(0, date)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_parse_from_format, 0, 0, 2)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, date)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_date_get_last_errors, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_format, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, format)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_format, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_modify, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, modify)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_modify, 0, 0, 1)
	ZEND_ARG_INFO(0, modify)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_add, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, interval)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_add, 0, 0, 1)
	ZEND_ARG_INFO(0, interval)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_sub, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, interval)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_sub, 0, 0, 1)
	ZEND_ARG_INFO(0, interval)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_timezone_get, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_date_method_timezone_get, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_timezone_set, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, timezone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_timezone_set, 0, 0, 1)
	ZEND_ARG_INFO(0, timezone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_offset_get, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_date_method_offset_get, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_diff, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, object2)
	ZEND_ARG_INFO(0, absolute)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_diff, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, absolute)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_time_set, 0, 0, 3)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_INFO(0, minute)
	ZEND_ARG_INFO(0, second)
	ZEND_ARG_INFO(0, microseconds)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_time_set, 0, 0, 2)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_INFO(0, minute)
	ZEND_ARG_INFO(0, second)
	ZEND_ARG_INFO(0, microseconds)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_date_set, 0, 0, 4)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, year)
	ZEND_ARG_INFO(0, month)
	ZEND_ARG_INFO(0, day)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_date_set, 0, 0, 3)
	ZEND_ARG_INFO(0, year)
	ZEND_ARG_INFO(0, month)
	ZEND_ARG_INFO(0, day)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_isodate_set, 0, 0, 3)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, year)
	ZEND_ARG_INFO(0, week)
	ZEND_ARG_INFO(0, day)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_isodate_set, 0, 0, 2)
	ZEND_ARG_INFO(0, year)
	ZEND_ARG_INFO(0, week)
	ZEND_ARG_INFO(0, day)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_timestamp_set, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, unixtimestamp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_timestamp_set, 0, 0, 1)
	ZEND_ARG_INFO(0, unixtimestamp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_timestamp_get, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_date_method_timestamp_get, 0)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_create_from_immutable, 0, 0, 1)
	ZEND_ARG_INFO(0, DateTimeImmutable)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_create_from_mutable, 0, 0, 1)
	ZEND_ARG_INFO(0, DateTime)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timezone_open, 0, 0, 1)
	ZEND_ARG_INFO(0, timezone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timezone_name_get, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_timezone_method_name_get, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timezone_name_from_abbr, 0, 0, 1)
	ZEND_ARG_INFO(0, abbr)
	ZEND_ARG_INFO(0, gmtoffset)
	ZEND_ARG_INFO(0, isdst)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timezone_offset_get, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeZone, 0)
	ZEND_ARG_OBJ_INFO(0, datetime, DateTimeInterface, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timezone_method_offset_get, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timezone_transitions_get, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, timestamp_begin)
	ZEND_ARG_INFO(0, timestamp_end)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timezone_method_transitions_get, 0, 0, 0)
	ZEND_ARG_INFO(0, timestamp_begin)
	ZEND_ARG_INFO(0, timestamp_end)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timezone_location_get, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_timezone_method_location_get, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timezone_identifiers_list, 0, 0, 0)
	ZEND_ARG_INFO(0, what)
	ZEND_ARG_INFO(0, country)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_timezone_abbreviations_list, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_timezone_version_get, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_interval_create_from_date_string, 0, 0, 1)
	ZEND_ARG_INFO(0, time)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_interval_format, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, format)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_date_method_interval_format, 0)
	ZEND_ARG_INFO(0, format)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_period_construct, 0, 0, 3)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, interval)
	ZEND_ARG_INFO(0, end)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_set_state, 0, 0, 1)
	ZEND_ARG_ARRAY_INFO(0, array, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_date_interval_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, interval_spec)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ Function table */
static const zend_function_entry date_functions[] = {
	PHP_FE(strtotime, arginfo_strtotime)
	PHP_FE(date, arginfo_date)
	PHP_FE(idate, arginfo_idate)
	PHP_FE(gmdate, arginfo_gmdate)
	PHP_FE(mktime, arginfo_mktime)
	PHP_FE(gmmktime, arginfo_gmmktime)
	PHP_FE(checkdate, arginfo_checkdate)
	PHP_FE(strftime, arginfo_strftime)
	PHP_FE(gmstrftime, arginfo_gmstrftime)
	PHP_FE(time, arginfo_time)
	PHP_FE(localtime, arginfo_localtime)
	PHP_FE(getdate, arginfo_getdate)

	/* Advanced Interface */
	PHP_FE(date_create, arginfo_date_create)
	PHP_FE(date_create_immutable, arginfo_date_create)
	PHP_FE(date_create_from_format, arginfo_date_create_from_format)
	PHP_FE(date_create_immutable_from_format, arginfo_date_create_from_format)
	PHP_FE(date_parse, arginfo_date_parse)
	PHP_FE(date_parse_from_format, arginfo_date_parse_from_format)
	PHP_FE(date_get_last_errors, arginfo_date_get_last_errors)
	PHP_FE(date_format, arginfo_date_format)
	PHP_FE(date_modify, arginfo_date_modify)
	PHP_FE(date_add, arginfo_date_add)
	PHP_FE(date_sub, arginfo_date_sub)
	PHP_FE(date_timezone_get, arginfo_date_timezone_get)
	PHP_FE(date_timezone_set, arginfo_date_timezone_set)
	PHP_FE(date_offset_get, arginfo_date_offset_get)
	PHP_FE(date_diff, arginfo_date_diff)

	PHP_FE(date_time_set, arginfo_date_time_set)
	PHP_FE(date_date_set, arginfo_date_date_set)
	PHP_FE(date_isodate_set, arginfo_date_isodate_set)
	PHP_FE(date_timestamp_set, arginfo_date_timestamp_set)
	PHP_FE(date_timestamp_get, arginfo_date_timestamp_get)

	PHP_FE(timezone_open, arginfo_timezone_open)
	PHP_FE(timezone_name_get, arginfo_timezone_name_get)
	PHP_FE(timezone_name_from_abbr, arginfo_timezone_name_from_abbr)
	PHP_FE(timezone_offset_get, arginfo_timezone_offset_get)
	PHP_FE(timezone_transitions_get, arginfo_timezone_transitions_get)
	PHP_FE(timezone_location_get, arginfo_timezone_location_get)
	PHP_FE(timezone_identifiers_list, arginfo_timezone_identifiers_list)
	PHP_FE(timezone_abbreviations_list, arginfo_timezone_abbreviations_list)
	PHP_FE(timezone_version_get, arginfo_timezone_version_get)

	PHP_FE(date_interval_create_from_date_string, arginfo_date_interval_create_from_date_string)
	PHP_FE(date_interval_format, arginfo_date_interval_format)

	/* Options and Configuration */
	PHP_FE(date_default_timezone_set, arginfo_date_default_timezone_set)
	PHP_FE(date_default_timezone_get, arginfo_date_default_timezone_get)

	/* Astronomical functions */
	PHP_FE(date_sunrise, arginfo_date_sunrise)
	PHP_FE(date_sunset, arginfo_date_sunset)
	PHP_FE(date_sun_info, arginfo_date_sun_info)
	PHP_FE_END
};

static const zend_function_entry date_funcs_interface[] = {
	PHP_ABSTRACT_ME(DateTimeInterface, format, arginfo_date_method_format)
	PHP_ABSTRACT_ME(DateTimeInterface, getTimezone, arginfo_date_method_timezone_get)
	PHP_ABSTRACT_ME(DateTimeInterface, getOffset, arginfo_date_method_offset_get)
	PHP_ABSTRACT_ME(DateTimeInterface, getTimestamp, arginfo_date_method_timestamp_get)
	PHP_ABSTRACT_ME(DateTimeInterface, diff, arginfo_date_method_diff)
	PHP_ABSTRACT_ME(DateTimeInterface, __wakeup, NULL)
	PHP_FE_END
};

static const zend_function_entry date_funcs_date[] = {
	PHP_ME(DateTime,			__construct,		arginfo_date_create, ZEND_ACC_PUBLIC)
	PHP_ME(DateTime,			__wakeup,			NULL, ZEND_ACC_PUBLIC)
	PHP_ME(DateTime,			__set_state,		arginfo_date_set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(DateTime,			createFromImmutable,	arginfo_date_method_create_from_immutable, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME_MAPPING(createFromFormat, date_create_from_format,	arginfo_date_create_from_format, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME_MAPPING(getLastErrors, date_get_last_errors,	arginfo_date_get_last_errors, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME_MAPPING(format,		date_format,		arginfo_date_method_format, 0)
	PHP_ME_MAPPING(modify,		date_modify,		arginfo_date_method_modify, 0)
	PHP_ME_MAPPING(add,			date_add,			arginfo_date_method_add, 0)
	PHP_ME_MAPPING(sub,			date_sub,			arginfo_date_method_sub, 0)
	PHP_ME_MAPPING(getTimezone, date_timezone_get,	arginfo_date_method_timezone_get, 0)
	PHP_ME_MAPPING(setTimezone, date_timezone_set,	arginfo_date_method_timezone_set, 0)
	PHP_ME_MAPPING(getOffset,	date_offset_get,	arginfo_date_method_offset_get, 0)
	PHP_ME_MAPPING(setTime,		date_time_set,		arginfo_date_method_time_set, 0)
	PHP_ME_MAPPING(setDate,		date_date_set,		arginfo_date_method_date_set, 0)
	PHP_ME_MAPPING(setISODate,	date_isodate_set,	arginfo_date_method_isodate_set, 0)
	PHP_ME_MAPPING(setTimestamp,	date_timestamp_set, arginfo_date_method_timestamp_set, 0)
	PHP_ME_MAPPING(getTimestamp,	date_timestamp_get, arginfo_date_method_timestamp_get, 0)
	PHP_ME_MAPPING(diff,			date_diff, arginfo_date_method_diff, 0)
	PHP_FE_END
};

static const zend_function_entry date_funcs_immutable[] = {
	PHP_ME(DateTimeImmutable, __construct,   arginfo_date_create, ZEND_ACC_PUBLIC)
	PHP_ME(DateTime, __wakeup,       NULL, ZEND_ACC_PUBLIC)
	PHP_ME(DateTimeImmutable, __set_state,   arginfo_date_set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME_MAPPING(createFromFormat, date_create_immutable_from_format, arginfo_date_create_from_format, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME_MAPPING(getLastErrors,    date_get_last_errors,    arginfo_date_get_last_errors, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME_MAPPING(format,           date_format,             arginfo_date_method_format, 0)
	PHP_ME_MAPPING(getTimezone, date_timezone_get,	arginfo_date_method_timezone_get, 0)
	PHP_ME_MAPPING(getOffset,	date_offset_get,	arginfo_date_method_offset_get, 0)
	PHP_ME_MAPPING(getTimestamp,	date_timestamp_get, arginfo_date_method_timestamp_get, 0)
	PHP_ME_MAPPING(diff,			date_diff, arginfo_date_method_diff, 0)
	PHP_ME(DateTimeImmutable, modify,        arginfo_date_method_modify, 0)
	PHP_ME(DateTimeImmutable, add,           arginfo_date_method_add, 0)
	PHP_ME(DateTimeImmutable, sub,           arginfo_date_method_sub, 0)
	PHP_ME(DateTimeImmutable, setTimezone,   arginfo_date_method_timezone_set, 0)
	PHP_ME(DateTimeImmutable, setTime,       arginfo_date_method_time_set, 0)
	PHP_ME(DateTimeImmutable, setDate,       arginfo_date_method_date_set, 0)
	PHP_ME(DateTimeImmutable, setISODate,    arginfo_date_method_isodate_set, 0)
	PHP_ME(DateTimeImmutable, setTimestamp,  arginfo_date_method_timestamp_set, 0)
	PHP_ME(DateTimeImmutable, createFromMutable, arginfo_date_method_create_from_mutable, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END
};

static const zend_function_entry date_funcs_timezone[] = {
	PHP_ME(DateTimeZone,              __construct,                 arginfo_timezone_open, ZEND_ACC_PUBLIC)
	PHP_ME(DateTimeZone,              __wakeup,                    NULL, ZEND_ACC_PUBLIC)
	PHP_ME(DateTimeZone,              __set_state,                 arginfo_date_set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME_MAPPING(getName,           timezone_name_get,           arginfo_timezone_method_name_get, 0)
	PHP_ME_MAPPING(getOffset,         timezone_offset_get,         arginfo_timezone_method_offset_get, 0)
	PHP_ME_MAPPING(getTransitions,    timezone_transitions_get,    arginfo_timezone_method_transitions_get, 0)
	PHP_ME_MAPPING(getLocation,       timezone_location_get,       arginfo_timezone_method_location_get, 0)
	PHP_ME_MAPPING(listAbbreviations, timezone_abbreviations_list, arginfo_timezone_abbreviations_list, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME_MAPPING(listIdentifiers,   timezone_identifiers_list,   arginfo_timezone_identifiers_list, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END
};

static const zend_function_entry date_funcs_interval[] = {
	PHP_ME(DateInterval,              __construct,                 arginfo_date_interval_construct, ZEND_ACC_PUBLIC)
	PHP_ME(DateInterval,              __wakeup,                    NULL, ZEND_ACC_PUBLIC)
	PHP_ME(DateInterval,              __set_state,                 arginfo_date_set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME_MAPPING(format,            date_interval_format,        arginfo_date_method_interval_format, 0)
	PHP_ME_MAPPING(createFromDateString, date_interval_create_from_date_string,	arginfo_date_interval_create_from_date_string, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END
};

static const zend_function_entry date_funcs_period[] = {
	PHP_ME(DatePeriod,                __construct,                 arginfo_date_period_construct, ZEND_ACC_PUBLIC)
	PHP_ME(DatePeriod,                __wakeup,                    NULL, ZEND_ACC_PUBLIC)
	PHP_ME(DatePeriod,                __set_state,                 arginfo_date_set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(DatePeriod,                getStartDate,                NULL, ZEND_ACC_PUBLIC)
	PHP_ME(DatePeriod,                getEndDate,                  NULL, ZEND_ACC_PUBLIC)
	PHP_ME(DatePeriod,                getDateInterval,             NULL, ZEND_ACC_PUBLIC)
	PHP_ME(DatePeriod,                getRecurrences,              NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

static char* guess_timezone(const timelib_tzdb *tzdb);
static void date_register_classes(void);
/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(date)
static PHP_GINIT_FUNCTION(date);

/* True global */
timelib_tzdb *php_date_global_timezone_db;
int php_date_global_timezone_db_enabled;

#define DATE_DEFAULT_LATITUDE "31.7667"
#define DATE_DEFAULT_LONGITUDE "35.2333"

/* on 90'35; common sunset declaration (start of sun body appear) */
#define DATE_SUNSET_ZENITH "90.583333"

/* on 90'35; common sunrise declaration (sun body disappeared) */
#define DATE_SUNRISE_ZENITH "90.583333"

static PHP_INI_MH(OnUpdate_date_timezone);

/* {{{ INI Settings */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("date.timezone", "", PHP_INI_ALL, OnUpdate_date_timezone, default_timezone, zend_date_globals, date_globals)
	PHP_INI_ENTRY("date.default_latitude",           DATE_DEFAULT_LATITUDE,        PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("date.default_longitude",          DATE_DEFAULT_LONGITUDE,       PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("date.sunset_zenith",              DATE_SUNSET_ZENITH,           PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("date.sunrise_zenith",             DATE_SUNRISE_ZENITH,          PHP_INI_ALL, NULL)
PHP_INI_END()
/* }}} */

zend_class_entry *date_ce_date, *date_ce_timezone, *date_ce_interval, *date_ce_period;
zend_class_entry *date_ce_immutable, *date_ce_interface;


PHPAPI zend_class_entry *php_date_get_date_ce(void)
{
	return date_ce_date;
}

PHPAPI zend_class_entry *php_date_get_immutable_ce(void)
{
	return date_ce_immutable;
}

PHPAPI zend_class_entry *php_date_get_interface_ce(void)
{
	return date_ce_interface;
}

PHPAPI zend_class_entry *php_date_get_timezone_ce(void)
{
	return date_ce_timezone;
}

PHPAPI zend_class_entry *php_date_get_interval_ce(void)
{
	return date_ce_interval;
}

PHPAPI zend_class_entry *php_date_get_period_ce(void)
{
	return date_ce_period;
}

static zend_object_handlers date_object_handlers_date;
static zend_object_handlers date_object_handlers_immutable;
static zend_object_handlers date_object_handlers_timezone;
static zend_object_handlers date_object_handlers_interval;
static zend_object_handlers date_object_handlers_period;

#define DATE_CHECK_INITIALIZED(member, class_name) \
	if (!(member)) { \
		php_error_docref(NULL, E_WARNING, "The " #class_name " object has not been correctly initialized by its constructor"); \
		RETURN_FALSE; \
	}

static void date_object_free_storage_date(zend_object *object);
static void date_object_free_storage_timezone(zend_object *object);
static void date_object_free_storage_interval(zend_object *object);
static void date_object_free_storage_period(zend_object *object);

static zend_object *date_object_new_date(zend_class_entry *class_type);
static zend_object *date_object_new_timezone(zend_class_entry *class_type);
static zend_object *date_object_new_interval(zend_class_entry *class_type);
static zend_object *date_object_new_period(zend_class_entry *class_type);

static zend_object *date_object_clone_date(zval *this_ptr);
static zend_object *date_object_clone_timezone(zval *this_ptr);
static zend_object *date_object_clone_interval(zval *this_ptr);
static zend_object *date_object_clone_period(zval *this_ptr);

static int date_object_compare_date(zval *d1, zval *d2);
static HashTable *date_object_get_gc(zval *object, zval **table, int *n);
static HashTable *date_object_get_properties_for(zval *object, zend_prop_purpose purpose);
static HashTable *date_object_get_gc_interval(zval *object, zval **table, int *n);
static HashTable *date_object_get_properties_interval(zval *object);
static HashTable *date_object_get_gc_period(zval *object, zval **table, int *n);
static HashTable *date_object_get_properties_period(zval *object);
static HashTable *date_object_get_properties_for_timezone(zval *object, zend_prop_purpose purpose);
static HashTable *date_object_get_gc_timezone(zval *object, zval **table, int *n);
static HashTable *date_object_get_debug_info_timezone(zval *object, int *is_temp);
static void php_timezone_to_string(php_timezone_obj *tzobj, zval *zv);

static int date_interval_compare_objects(zval *o1, zval *o2);
static zval *date_interval_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv);
static zval *date_interval_write_property(zval *object, zval *member, zval *value, void **cache_slot);
static zval *date_interval_get_property_ptr_ptr(zval *object, zval *member, int type, void **cache_slot);
static zval *date_period_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv);
static zval *date_period_write_property(zval *object, zval *member, zval *value, void **cache_slot);
static zval *date_period_get_property_ptr_ptr(zval *object, zval *member, int type, void **cache_slot);

static int date_object_compare_timezone(zval *tz1, zval *tz2);

/* {{{ Module struct */
zend_module_entry date_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	NULL,
	"date",                     /* extension name */
	date_functions,             /* function list */
	PHP_MINIT(date),            /* process startup */
	PHP_MSHUTDOWN(date),        /* process shutdown */
	PHP_RINIT(date),            /* request startup */
	PHP_RSHUTDOWN(date),        /* request shutdown */
	PHP_MINFO(date),            /* extension info */
	PHP_DATE_VERSION,                /* extension version */
	PHP_MODULE_GLOBALS(date),   /* globals descriptor */
	PHP_GINIT(date),            /* globals ctor */
	NULL,                       /* globals dtor */
	NULL,                       /* post deactivate */
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */


/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(date)
{
	date_globals->default_timezone = NULL;
	date_globals->timezone = NULL;
	date_globals->tzcache = NULL;
	date_globals->timezone_valid = 0;
}
/* }}} */


static void _php_date_tzinfo_dtor(zval *zv) /* {{{ */
{
	timelib_tzinfo *tzi = (timelib_tzinfo*)Z_PTR_P(zv);

	timelib_tzinfo_dtor(tzi);
} /* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(date)
{
	if (DATEG(timezone)) {
		efree(DATEG(timezone));
	}
	DATEG(timezone) = NULL;
	DATEG(tzcache) = NULL;
	DATEG(last_errors) = NULL;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(date)
{
	if (DATEG(timezone)) {
		efree(DATEG(timezone));
	}
	DATEG(timezone) = NULL;
	if(DATEG(tzcache)) {
		zend_hash_destroy(DATEG(tzcache));
		FREE_HASHTABLE(DATEG(tzcache));
		DATEG(tzcache) = NULL;
	}
	if (DATEG(last_errors)) {
		timelib_error_container_dtor(DATEG(last_errors));
		DATEG(last_errors) = NULL;
	}

	return SUCCESS;
}
/* }}} */

#define DATE_TIMEZONEDB      php_date_global_timezone_db ? php_date_global_timezone_db : timelib_builtin_db()

/*
 * RFC822, Section 5.1: http://www.ietf.org/rfc/rfc822.txt
 *  date-time   =  [ day "," ] date time        ; dd mm yy hh:mm:ss zzz
 *  day         =  "Mon"  / "Tue" /  "Wed"  / "Thu"  /  "Fri"  / "Sat" /  "Sun"
 *  date        =  1*2DIGIT month 2DIGIT        ; day month year e.g. 20 Jun 82
 *  month       =  "Jan"  /  "Feb" /  "Mar"  /  "Apr"  /  "May"  /  "Jun" /  "Jul"  /  "Aug"  /  "Sep"  /  "Oct" /  "Nov"  /  "Dec"
 *  time        =  hour zone                    ; ANSI and Military
 *  hour        =  2DIGIT ":" 2DIGIT [":" 2DIGIT] ; 00:00:00 - 23:59:59
 *  zone        =  "UT"  / "GMT"  /  "EST" / "EDT"  /  "CST" / "CDT"  /  "MST" / "MDT"  /  "PST" / "PDT"  /  1ALPHA  / ( ("+" / "-") 4DIGIT )
 */
#define DATE_FORMAT_RFC822   "D, d M y H:i:s O"

/*
 * RFC850, Section 2.1.4: http://www.ietf.org/rfc/rfc850.txt
 *  Format must be acceptable both to the ARPANET and to the getdate routine.
 *  One format that is acceptable to both is Weekday, DD-Mon-YY HH:MM:SS TIMEZONE
 *  TIMEZONE can be any timezone name (3 or more letters)
 */
#define DATE_FORMAT_RFC850   "l, d-M-y H:i:s T"

/*
 * RFC1036, Section 2.1.2: http://www.ietf.org/rfc/rfc1036.txt
 *  Its format must be acceptable both in RFC-822 and to the getdate(3)
 *  Wdy, DD Mon YY HH:MM:SS TIMEZONE
 *  There is no hope of having a complete list of timezones.  Universal
 *  Time (GMT), the North American timezones (PST, PDT, MST, MDT, CST,
 *  CDT, EST, EDT) and the +/-hhmm offset specified in RFC-822 should be supported.
 */
#define DATE_FORMAT_RFC1036  "D, d M y H:i:s O"

/*
 * RFC1123, Section 5.2.14: http://www.ietf.org/rfc/rfc1123.txt
 *  RFC-822 Date and Time Specification: RFC-822 Section 5
 *  The syntax for the date is hereby changed to: date = 1*2DIGIT month 2*4DIGIT
 */
#define DATE_FORMAT_RFC1123  "D, d M Y H:i:s O"

/*
 * RFC7231, Section 7.1.1: http://tools.ietf.org/html/rfc7231
 */
#define DATE_FORMAT_RFC7231  "D, d M Y H:i:s \\G\\M\\T"

/*
 * RFC2822, Section 3.3: http://www.ietf.org/rfc/rfc2822.txt
 *  FWS             =       ([*WSP CRLF] 1*WSP) /   ; Folding white space
 *  CFWS            =       *([FWS] comment) (([FWS] comment) / FWS)
 *
 *  date-time       =       [ day-of-week "," ] date FWS time [CFWS]
 *  day-of-week     =       ([FWS] day-name)
 *  day-name        =       "Mon" / "Tue" / "Wed" / "Thu" / "Fri" / "Sat" / "Sun"
 *  date            =       day month year
 *  year            =       4*DIGIT
 *  month           =       (FWS month-name FWS)
 *  month-name      =       "Jan" / "Feb" / "Mar" / "Apr" / "May" / "Jun" / "Jul" / "Aug" / "Sep" / "Oct" / "Nov" / "Dec"
 *  day             =       ([FWS] 1*2DIGIT)
 *  time            =       time-of-day FWS zone
 *  time-of-day     =       hour ":" minute [ ":" second ]
 *  hour            =       2DIGIT
 *  minute          =       2DIGIT
 *  second          =       2DIGIT
 *  zone            =       (( "+" / "-" ) 4DIGIT)
 */
#define DATE_FORMAT_RFC2822  "D, d M Y H:i:s O"
/*
 * RFC3339, Section 5.6: http://www.ietf.org/rfc/rfc3339.txt
 *  date-fullyear   = 4DIGIT
 *  date-month      = 2DIGIT  ; 01-12
 *  date-mday       = 2DIGIT  ; 01-28, 01-29, 01-30, 01-31 based on month/year
 *
 *  time-hour       = 2DIGIT  ; 00-23
 *  time-minute     = 2DIGIT  ; 00-59
 *  time-second     = 2DIGIT  ; 00-58, 00-59, 00-60 based on leap second rules
 *
 *  time-secfrac    = "." 1*DIGIT
 *  time-numoffset  = ("+" / "-") time-hour ":" time-minute
 *  time-offset     = "Z" / time-numoffset
 *
 *  partial-time    = time-hour ":" time-minute ":" time-second [time-secfrac]
 *  full-date       = date-fullyear "-" date-month "-" date-mday
 *  full-time       = partial-time time-offset
 *
 *  date-time       = full-date "T" full-time
 */
#define DATE_FORMAT_RFC3339  "Y-m-d\\TH:i:sP"

#define DATE_FORMAT_ISO8601  "Y-m-d\\TH:i:sO"

/*
 * RFC3339, Appendix A: http://www.ietf.org/rfc/rfc3339.txt
 *  ISO 8601 also requires (in section 5.3.1.3) that a decimal fraction
 *  be proceeded by a "0" if less than unity.  Annex B.2 of ISO 8601
 *  gives examples where the decimal fractions are not preceded by a "0".
 *  This grammar assumes section 5.3.1.3 is correct and that Annex B.2 is
 *  in error.
 */
#define DATE_FORMAT_RFC3339_EXTENDED  "Y-m-d\\TH:i:s.vP"

/*
 * This comes from various sources that like to contradict. I'm going with the
 * format here because of:
 * http://msdn.microsoft.com/en-us/library/windows/desktop/aa384321%28v=vs.85%29.aspx
 * and http://curl.haxx.se/rfc/cookie_spec.html
 */
#define DATE_FORMAT_COOKIE   "l, d-M-Y H:i:s T"

#define SUNFUNCS_RET_TIMESTAMP 0
#define SUNFUNCS_RET_STRING    1
#define SUNFUNCS_RET_DOUBLE    2

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(date)
{
	REGISTER_INI_ENTRIES();
	date_register_classes();
/*
 * RFC4287, Section 3.3: http://www.ietf.org/rfc/rfc4287.txt
 *   A Date construct is an element whose content MUST conform to the
 *   "date-time" production in [RFC3339].  In addition, an uppercase "T"
 *   character MUST be used to separate date and time, and an uppercase
 *   "Z" character MUST be present in the absence of a numeric time zone offset.
 */
	REGISTER_STRING_CONSTANT("DATE_ATOM",    DATE_FORMAT_RFC3339, CONST_CS | CONST_PERSISTENT);
/*
 * Preliminary specification: http://wp.netscape.com/newsref/std/cookie_spec.html
 *   "This is based on RFC 822, RFC 850,  RFC 1036, and  RFC 1123,
 *   with the variations that the only legal time zone is GMT
 *   and the separators between the elements of the date must be dashes."
 */
	REGISTER_STRING_CONSTANT("DATE_COOKIE",  DATE_FORMAT_COOKIE,  CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_ISO8601", DATE_FORMAT_ISO8601, CONST_CS | CONST_PERSISTENT);

	REGISTER_STRING_CONSTANT("DATE_RFC822",  DATE_FORMAT_RFC822,  CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_RFC850",  DATE_FORMAT_RFC850,  CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_RFC1036", DATE_FORMAT_RFC1036, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_RFC1123", DATE_FORMAT_RFC1123, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_RFC7231", DATE_FORMAT_RFC7231, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_RFC2822", DATE_FORMAT_RFC2822, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_RFC3339", DATE_FORMAT_RFC3339, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_RFC3339_EXTENDED", DATE_FORMAT_RFC3339_EXTENDED, CONST_CS | CONST_PERSISTENT);

/*
 * RSS 2.0 Specification: http://blogs.law.harvard.edu/tech/rss
 *   "All date-times in RSS conform to the Date and Time Specification of RFC 822,
 *   with the exception that the year may be expressed with two characters or four characters (four preferred)"
 */
	REGISTER_STRING_CONSTANT("DATE_RSS",     DATE_FORMAT_RFC1123, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_W3C",     DATE_FORMAT_RFC3339, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SUNFUNCS_RET_TIMESTAMP", SUNFUNCS_RET_TIMESTAMP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SUNFUNCS_RET_STRING", SUNFUNCS_RET_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SUNFUNCS_RET_DOUBLE", SUNFUNCS_RET_DOUBLE, CONST_CS | CONST_PERSISTENT);

	php_date_global_timezone_db = NULL;
	php_date_global_timezone_db_enabled = 0;
	DATEG(last_errors) = NULL;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(date)
{
	UNREGISTER_INI_ENTRIES();

	if (DATEG(last_errors)) {
		timelib_error_container_dtor(DATEG(last_errors));
	}

#ifndef ZTS
	DATEG(default_timezone) = NULL;
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(date)
{
	const timelib_tzdb *tzdb = DATE_TIMEZONEDB;

	php_info_print_table_start();
	php_info_print_table_row(2, "date/time support", "enabled");
	php_info_print_table_row(2, "timelib version", TIMELIB_ASCII_VERSION);
	php_info_print_table_row(2, "\"Olson\" Timezone Database Version", tzdb->version);
	php_info_print_table_row(2, "Timezone Database", php_date_global_timezone_db_enabled ? "external" : "internal");
	php_info_print_table_row(2, "Default timezone", guess_timezone(tzdb));
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ Timezone Cache functions */
static timelib_tzinfo *php_date_parse_tzfile(char *formal_tzname, const timelib_tzdb *tzdb)
{
	timelib_tzinfo *tzi;
	int dummy_error_code;

	if(!DATEG(tzcache)) {
		ALLOC_HASHTABLE(DATEG(tzcache));
		zend_hash_init(DATEG(tzcache), 4, NULL, _php_date_tzinfo_dtor, 0);
	}

	if ((tzi = zend_hash_str_find_ptr(DATEG(tzcache), formal_tzname, strlen(formal_tzname))) != NULL) {
		return tzi;
	}

	tzi = timelib_parse_tzfile(formal_tzname, tzdb, &dummy_error_code);
	if (tzi) {
		zend_hash_str_add_ptr(DATEG(tzcache), formal_tzname, strlen(formal_tzname), tzi);
	}
	return tzi;
}

timelib_tzinfo *php_date_parse_tzfile_wrapper(char *formal_tzname, const timelib_tzdb *tzdb, int *dummy_error_code)
{
	return php_date_parse_tzfile(formal_tzname, tzdb);
}
/* }}} */

/* Callback to check the date.timezone only when changed increases performance */
/* {{{ static PHP_INI_MH(OnUpdate_date_timezone) */
static PHP_INI_MH(OnUpdate_date_timezone)
{
	if (OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage) == FAILURE) {
		return FAILURE;
	}

	DATEG(timezone_valid) = 0;
	if (stage == PHP_INI_STAGE_RUNTIME) {
		if (!timelib_timezone_id_is_valid(DATEG(default_timezone), DATE_TIMEZONEDB)) {
			if (DATEG(default_timezone) && *DATEG(default_timezone)) {
				php_error_docref(NULL, E_WARNING, "Invalid date.timezone value '%s', we selected the timezone 'UTC' for now.", DATEG(default_timezone));
			}
		} else {
			DATEG(timezone_valid) = 1;
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ Helper functions */
static char* guess_timezone(const timelib_tzdb *tzdb)
{
	/* Checking configure timezone */
	if (DATEG(timezone) && (strlen(DATEG(timezone))) > 0) {
		return DATEG(timezone);
	}
	/* Check config setting for default timezone */
	if (!DATEG(default_timezone)) {
		/* Special case: ext/date wasn't initialized yet */
		zval *ztz;

		if (NULL != (ztz = cfg_get_entry("date.timezone", sizeof("date.timezone")))
			&& Z_TYPE_P(ztz) == IS_STRING && Z_STRLEN_P(ztz) > 0 && timelib_timezone_id_is_valid(Z_STRVAL_P(ztz), tzdb)) {
			return Z_STRVAL_P(ztz);
		}
	} else if (*DATEG(default_timezone)) {
		if (DATEG(timezone_valid) == 1) {
			return DATEG(default_timezone);
		}

		if (!timelib_timezone_id_is_valid(DATEG(default_timezone), tzdb)) {
			php_error_docref(NULL, E_WARNING, "Invalid date.timezone value '%s', we selected the timezone 'UTC' for now.", DATEG(default_timezone));
			return "UTC";
		}

		DATEG(timezone_valid) = 1;
		return DATEG(default_timezone);
	}
	/* Fallback to UTC */
	return "UTC";
}

PHPAPI timelib_tzinfo *get_timezone_info(void)
{
	char *tz;
	timelib_tzinfo *tzi;

	tz = guess_timezone(DATE_TIMEZONEDB);
	tzi = php_date_parse_tzfile(tz, DATE_TIMEZONEDB);
	if (! tzi) {
		php_error_docref(NULL, E_ERROR, "Timezone database is corrupt - this should *never* happen!");
	}
	return tzi;
}
/* }}} */


/* {{{ date() and gmdate() data */
#include "zend_smart_str.h"

static const char * const mon_full_names[] = {
	"January", "February", "March", "April",
	"May", "June", "July", "August",
	"September", "October", "November", "December"
};

static const char * const mon_short_names[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const char * const day_full_names[] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

static const char * const day_short_names[] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static char *english_suffix(timelib_sll number)
{
	if (number >= 10 && number <= 19) {
		return "th";
	} else {
		switch (number % 10) {
			case 1: return "st";
			case 2: return "nd";
			case 3: return "rd";
		}
	}
	return "th";
}
/* }}} */

/* {{{ day of week helpers */
static const char *php_date_full_day_name(timelib_sll y, timelib_sll m, timelib_sll d)
{
	timelib_sll day_of_week = timelib_day_of_week(y, m, d);
	if (day_of_week < 0) {
		return "Unknown";
	}
	return day_full_names[day_of_week];
}

static const char *php_date_short_day_name(timelib_sll y, timelib_sll m, timelib_sll d)
{
	timelib_sll day_of_week = timelib_day_of_week(y, m, d);
	if (day_of_week < 0) {
		return "Unknown";
	}
	return day_short_names[day_of_week];
}
/* }}} */

/* {{{ date_format - (gm)date helper */
static zend_string *date_format(char *format, size_t format_len, timelib_time *t, int localtime)
{
	smart_str            string = {0};
	size_t               i;
	int                  length = 0;
	char                 buffer[97];
	timelib_time_offset *offset = NULL;
	timelib_sll          isoweek, isoyear;
	int                  rfc_colon;
	int                  weekYearSet = 0;

	if (!format_len) {
		return ZSTR_EMPTY_ALLOC();
	}

	if (localtime) {
		if (t->zone_type == TIMELIB_ZONETYPE_ABBR) {
			offset = timelib_time_offset_ctor();
			offset->offset = (t->z + (t->dst * 3600));
			offset->leap_secs = 0;
			offset->is_dst = t->dst;
			offset->abbr = timelib_strdup(t->tz_abbr);
		} else if (t->zone_type == TIMELIB_ZONETYPE_OFFSET) {
			offset = timelib_time_offset_ctor();
			offset->offset = (t->z);
			offset->leap_secs = 0;
			offset->is_dst = 0;
			offset->abbr = timelib_malloc(9); /* GMT±xxxx\0 */
			snprintf(offset->abbr, 9, "GMT%c%02d%02d",
			                          (offset->offset < 0) ? '-' : '+',
			                          abs(offset->offset / 3600),
			                          abs((offset->offset % 3600) / 60));
		} else {
			offset = timelib_get_time_zone_info(t->sse, t->tz_info);
		}
	}

	for (i = 0; i < format_len; i++) {
		rfc_colon = 0;
		switch (format[i]) {
			/* day */
			case 'd': length = slprintf(buffer, sizeof(buffer), "%02d", (int) t->d); break;
			case 'D': length = slprintf(buffer, sizeof(buffer), "%s", php_date_short_day_name(t->y, t->m, t->d)); break;
			case 'j': length = slprintf(buffer, sizeof(buffer), "%d", (int) t->d); break;
			case 'l': length = slprintf(buffer, sizeof(buffer), "%s", php_date_full_day_name(t->y, t->m, t->d)); break;
			case 'S': length = slprintf(buffer, sizeof(buffer), "%s", english_suffix(t->d)); break;
			case 'w': length = slprintf(buffer, sizeof(buffer), "%d", (int) timelib_day_of_week(t->y, t->m, t->d)); break;
			case 'N': length = slprintf(buffer, sizeof(buffer), "%d", (int) timelib_iso_day_of_week(t->y, t->m, t->d)); break;
			case 'z': length = slprintf(buffer, sizeof(buffer), "%d", (int) timelib_day_of_year(t->y, t->m, t->d)); break;

			/* week */
			case 'W':
				if(!weekYearSet) { timelib_isoweek_from_date(t->y, t->m, t->d, &isoweek, &isoyear); weekYearSet = 1; }
				length = slprintf(buffer, sizeof(buffer), "%02d", (int) isoweek); break; /* iso weeknr */
			case 'o':
				if(!weekYearSet) { timelib_isoweek_from_date(t->y, t->m, t->d, &isoweek, &isoyear); weekYearSet = 1; }
				length = slprintf(buffer, sizeof(buffer), ZEND_LONG_FMT, (zend_long) isoyear); break; /* iso year */

			/* month */
			case 'F': length = slprintf(buffer, sizeof(buffer), "%s", mon_full_names[t->m - 1]); break;
			case 'm': length = slprintf(buffer, sizeof(buffer), "%02d", (int) t->m); break;
			case 'M': length = slprintf(buffer, sizeof(buffer), "%s", mon_short_names[t->m - 1]); break;
			case 'n': length = slprintf(buffer, sizeof(buffer), "%d", (int) t->m); break;
			case 't': length = slprintf(buffer, sizeof(buffer), "%d", (int) timelib_days_in_month(t->y, t->m)); break;

			/* year */
			case 'L': length = slprintf(buffer, sizeof(buffer), "%d", timelib_is_leap((int) t->y)); break;
			case 'y': length = slprintf(buffer, sizeof(buffer), "%02d", (int) (t->y % 100)); break;
			case 'Y': length = slprintf(buffer, sizeof(buffer), "%s%04lld", t->y < 0 ? "-" : "", php_date_llabs((timelib_sll) t->y)); break;

			/* time */
			case 'a': length = slprintf(buffer, sizeof(buffer), "%s", t->h >= 12 ? "pm" : "am"); break;
			case 'A': length = slprintf(buffer, sizeof(buffer), "%s", t->h >= 12 ? "PM" : "AM"); break;
			case 'B': {
				int retval = ((((long)t->sse)-(((long)t->sse) - ((((long)t->sse) % 86400) + 3600))) * 10);
				if (retval < 0) {
					retval += 864000;
				}
				/* Make sure to do this on a positive int to avoid rounding errors */
				retval = (retval / 864)  % 1000;
				length = slprintf(buffer, sizeof(buffer), "%03d", retval);
				break;
			}
			case 'g': length = slprintf(buffer, sizeof(buffer), "%d", (t->h % 12) ? (int) t->h % 12 : 12); break;
			case 'G': length = slprintf(buffer, sizeof(buffer), "%d", (int) t->h); break;
			case 'h': length = slprintf(buffer, sizeof(buffer), "%02d", (t->h % 12) ? (int) t->h % 12 : 12); break;
			case 'H': length = slprintf(buffer, sizeof(buffer), "%02d", (int) t->h); break;
			case 'i': length = slprintf(buffer, sizeof(buffer), "%02d", (int) t->i); break;
			case 's': length = slprintf(buffer, sizeof(buffer), "%02d", (int) t->s); break;
			case 'u': length = slprintf(buffer, sizeof(buffer), "%06d", (int) floor(t->us)); break;
			case 'v': length = slprintf(buffer, sizeof(buffer), "%03d", (int) floor(t->us / 1000)); break;

			/* timezone */
			case 'I': length = slprintf(buffer, sizeof(buffer), "%d", localtime ? offset->is_dst : 0); break;
			case 'P': rfc_colon = 1; /* break intentionally missing */
			case 'O': length = slprintf(buffer, sizeof(buffer), "%c%02d%s%02d",
											localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
											localtime ? abs(offset->offset / 3600) : 0,
											rfc_colon ? ":" : "",
											localtime ? abs((offset->offset % 3600) / 60) : 0
							  );
					  break;
			case 'T': length = slprintf(buffer, sizeof(buffer), "%s", localtime ? offset->abbr : "GMT"); break;
			case 'e': if (!localtime) {
					      length = slprintf(buffer, sizeof(buffer), "%s", "UTC");
					  } else {
						  switch (t->zone_type) {
							  case TIMELIB_ZONETYPE_ID:
								  length = slprintf(buffer, sizeof(buffer), "%s", t->tz_info->name);
								  break;
							  case TIMELIB_ZONETYPE_ABBR:
								  length = slprintf(buffer, sizeof(buffer), "%s", offset->abbr);
								  break;
							  case TIMELIB_ZONETYPE_OFFSET:
								  length = slprintf(buffer, sizeof(buffer), "%c%02d:%02d",
												((offset->offset < 0) ? '-' : '+'),
												abs(offset->offset / 3600),
												abs((offset->offset % 3600) / 60)
										   );
								  break;
						  }
					  }
					  break;
			case 'Z': length = slprintf(buffer, sizeof(buffer), "%d", localtime ? offset->offset : 0); break;

			/* full date/time */
			case 'c': length = slprintf(buffer, sizeof(buffer), "%04" ZEND_LONG_FMT_SPEC "-%02d-%02dT%02d:%02d:%02d%c%02d:%02d",
							                (zend_long) t->y, (int) t->m, (int) t->d,
											(int) t->h, (int) t->i, (int) t->s,
											localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
											localtime ? abs(offset->offset / 3600) : 0,
											localtime ? abs((offset->offset % 3600) / 60) : 0
							  );
					  break;
			case 'r': length = slprintf(buffer, sizeof(buffer), "%3s, %02d %3s %04" ZEND_LONG_FMT_SPEC " %02d:%02d:%02d %c%02d%02d",
							                php_date_short_day_name(t->y, t->m, t->d),
											(int) t->d, mon_short_names[t->m - 1],
											(zend_long) t->y, (int) t->h, (int) t->i, (int) t->s,
											localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
											localtime ? abs(offset->offset / 3600) : 0,
											localtime ? abs((offset->offset % 3600) / 60) : 0
							  );
					  break;
			case 'U': length = slprintf(buffer, sizeof(buffer), "%lld", (timelib_sll) t->sse); break;

			case '\\': if (i < format_len) i++; /* break intentionally missing */

			default: buffer[0] = format[i]; buffer[1] = '\0'; length = 1; break;
		}
		smart_str_appendl(&string, buffer, length);
	}

	smart_str_0(&string);

	if (localtime) {
		timelib_time_offset_dtor(offset);
	}

	return string.s;
}

static void php_date(INTERNAL_FUNCTION_PARAMETERS, int localtime)
{
	zend_string *format;
	zend_long    ts;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(format)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(ts)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (ZEND_NUM_ARGS() == 1) {
		ts = php_time();
	}

	RETURN_STR(php_format_date(ZSTR_VAL(format), ZSTR_LEN(format), ts, localtime));
}
/* }}} */

PHPAPI zend_string *php_format_date(char *format, size_t format_len, time_t ts, int localtime) /* {{{ */
{
	timelib_time   *t;
	timelib_tzinfo *tzi;
	zend_string *string;

	t = timelib_time_ctor();

	if (localtime) {
		tzi = get_timezone_info();
		t->tz_info = tzi;
		t->zone_type = TIMELIB_ZONETYPE_ID;
		timelib_unixtime2local(t, ts);
	} else {
		tzi = NULL;
		timelib_unixtime2gmt(t, ts);
	}

	string = date_format(format, format_len, t, localtime);

	timelib_time_dtor(t);
	return string;
}
/* }}} */

/* {{{ php_idate
 */
PHPAPI int php_idate(char format, time_t ts, int localtime)
{
	timelib_time   *t;
	timelib_tzinfo *tzi;
	int retval = -1;
	timelib_time_offset *offset = NULL;
	timelib_sll isoweek, isoyear;

	t = timelib_time_ctor();

	if (!localtime) {
		tzi = get_timezone_info();
		t->tz_info = tzi;
		t->zone_type = TIMELIB_ZONETYPE_ID;
		timelib_unixtime2local(t, ts);
	} else {
		tzi = NULL;
		timelib_unixtime2gmt(t, ts);
	}

	if (!localtime) {
		if (t->zone_type == TIMELIB_ZONETYPE_ABBR) {
			offset = timelib_time_offset_ctor();
			offset->offset = (t->z + (t->dst * 3600));
			offset->leap_secs = 0;
			offset->is_dst = t->dst;
			offset->abbr = timelib_strdup(t->tz_abbr);
		} else if (t->zone_type == TIMELIB_ZONETYPE_OFFSET) {
			offset = timelib_time_offset_ctor();
			offset->offset = (t->z + (t->dst * 3600));
			offset->leap_secs = 0;
			offset->is_dst = t->dst;
			offset->abbr = timelib_malloc(9); /* GMT±xxxx\0 */
			snprintf(offset->abbr, 9, "GMT%c%02d%02d",
			                          (offset->offset < 0) ? '-' : '+',
			                          abs(offset->offset / 3600),
			                          abs((offset->offset % 3600) / 60));
		} else {
			offset = timelib_get_time_zone_info(t->sse, t->tz_info);
		}
	}

	timelib_isoweek_from_date(t->y, t->m, t->d, &isoweek, &isoyear);

	switch (format) {
		/* day */
		case 'd': case 'j': retval = (int) t->d; break;

		case 'w': retval = (int) timelib_day_of_week(t->y, t->m, t->d); break;
		case 'z': retval = (int) timelib_day_of_year(t->y, t->m, t->d); break;

		/* week */
		case 'W': retval = (int) isoweek; break; /* iso weeknr */

		/* month */
		case 'm': case 'n': retval = (int) t->m; break;
		case 't': retval = (int) timelib_days_in_month(t->y, t->m); break;

		/* year */
		case 'L': retval = (int) timelib_is_leap((int) t->y); break;
		case 'y': retval = (int) (t->y % 100); break;
		case 'Y': retval = (int) t->y; break;

		/* Swatch Beat a.k.a. Internet Time */
		case 'B':
			retval = ((((long)t->sse)-(((long)t->sse) - ((((long)t->sse) % 86400) + 3600))) * 10);
			if (retval < 0) {
				retval += 864000;
			}
			/* Make sure to do this on a positive int to avoid rounding errors */
			retval = (retval / 864) % 1000;
			break;

		/* time */
		case 'g': case 'h': retval = (int) ((t->h % 12) ? (int) t->h % 12 : 12); break;
		case 'H': case 'G': retval = (int) t->h; break;
		case 'i': retval = (int) t->i; break;
		case 's': retval = (int) t->s; break;

		/* timezone */
		case 'I': retval = (int) (!localtime ? offset->is_dst : 0); break;
		case 'Z': retval = (int) (!localtime ? offset->offset : 0); break;

		case 'U': retval = (int) t->sse; break;
	}

	if (!localtime) {
		timelib_time_offset_dtor(offset);
	}
	timelib_time_dtor(t);

	return retval;
}
/* }}} */

/* {{{ proto string date(string format [, int timestamp])
   Format a local date/time */
PHP_FUNCTION(date)
{
	php_date(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto string gmdate(string format [, int timestamp])
   Format a GMT date/time */
PHP_FUNCTION(gmdate)
{
	php_date(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int idate(string format [, int timestamp])
   Format a local time/date as integer */
PHP_FUNCTION(idate)
{
	zend_string *format;
	zend_long    ts = 0;
	int ret;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(format)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(ts)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (ZSTR_LEN(format) != 1) {
		php_error_docref(NULL, E_WARNING, "idate format is one char");
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() == 1) {
		ts = php_time();
	}

	ret = php_idate(ZSTR_VAL(format)[0], ts, 0);
	if (ret == -1) {
		php_error_docref(NULL, E_WARNING, "Unrecognized date format token.");
		RETURN_FALSE;
	}
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ php_date_set_tzdb - NOT THREADSAFE */
PHPAPI void php_date_set_tzdb(timelib_tzdb *tzdb)
{
	const timelib_tzdb *builtin = timelib_builtin_db();

	if (php_version_compare(tzdb->version, builtin->version) > 0) {
		php_date_global_timezone_db = tzdb;
		php_date_global_timezone_db_enabled = 1;
	}
}
/* }}} */

/* {{{ php_parse_date: Backwards compatibility function */
PHPAPI zend_long php_parse_date(char *string, zend_long *now)
{
	timelib_time *parsed_time;
	timelib_error_container *error = NULL;
	int           error2;
	zend_long   retval;

	parsed_time = timelib_strtotime(string, strlen(string), &error, DATE_TIMEZONEDB, php_date_parse_tzfile_wrapper);
	if (error->error_count) {
		timelib_time_dtor(parsed_time);
		timelib_error_container_dtor(error);
		return -1;
	}
	timelib_error_container_dtor(error);
	timelib_update_ts(parsed_time, NULL);
	retval = timelib_date_to_int(parsed_time, &error2);
	timelib_time_dtor(parsed_time);
	if (error2) {
		return -1;
	}
	return retval;
}
/* }}} */

/* {{{ proto int strtotime(string time [, int now ])
   Convert string representation of date and time to a timestamp */
PHP_FUNCTION(strtotime)
{
	zend_string *times;
	int error1, error2;
	timelib_error_container *error;
	zend_long preset_ts = 0, ts;
	timelib_time *t, *now;
	timelib_tzinfo *tzi;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(times)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(preset_ts)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	tzi = get_timezone_info();

	now = timelib_time_ctor();
	now->tz_info = tzi;
	now->zone_type = TIMELIB_ZONETYPE_ID;
	timelib_unixtime2local(now,
		(ZEND_NUM_ARGS() == 2) ? (timelib_sll) preset_ts : (timelib_sll) php_time());

	t = timelib_strtotime(ZSTR_VAL(times), ZSTR_LEN(times), &error,
		DATE_TIMEZONEDB, php_date_parse_tzfile_wrapper);
	error1 = error->error_count;
	timelib_error_container_dtor(error);
	timelib_fill_holes(t, now, TIMELIB_NO_CLONE);
	timelib_update_ts(t, tzi);
	ts = timelib_date_to_int(t, &error2);

	timelib_time_dtor(now);
	timelib_time_dtor(t);

	if (error1 || error2) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(ts);
	}
}
/* }}} */

/* {{{ php_mktime - (gm)mktime helper */
PHPAPI void php_mktime(INTERNAL_FUNCTION_PARAMETERS, int gmt)
{
	zend_long hou = 0, min = 0, sec = 0, mon = 0, day = 0, yea = 0;
	timelib_time *now;
	timelib_tzinfo *tzi = NULL;
	zend_long ts, adjust_seconds = 0;
	int error;

	ZEND_PARSE_PARAMETERS_START(0, 6)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(hou)
		Z_PARAM_LONG(min)
		Z_PARAM_LONG(sec)
		Z_PARAM_LONG(mon)
		Z_PARAM_LONG(day)
		Z_PARAM_LONG(yea)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	/* Initialize structure with current time */
	now = timelib_time_ctor();
	if (gmt) {
		timelib_unixtime2gmt(now, (timelib_sll) php_time());
	} else {
		tzi = get_timezone_info();
		now->tz_info = tzi;
		now->zone_type = TIMELIB_ZONETYPE_ID;
		timelib_unixtime2local(now, (timelib_sll) php_time());
	}
	/* Fill in the new data */
	switch (ZEND_NUM_ARGS()) {
		case 7:
			/* break intentionally missing */
		case 6:
			if (yea >= 0 && yea < 70) {
				yea += 2000;
			} else if (yea >= 70 && yea <= 100) {
				yea += 1900;
			}
			now->y = yea;
			/* break intentionally missing again */
		case 5:
			now->d = day;
			/* break missing intentionally here too */
		case 4:
			now->m = mon;
			/* and here */
		case 3:
			now->s = sec;
			/* yup, this break isn't here on purpose too */
		case 2:
			now->i = min;
			/* last intentionally missing break */
		case 1:
			now->h = hou;
			break;
		default:
			php_error_docref(NULL, E_DEPRECATED, "You should be using the time() function instead");
	}
	/* Update the timestamp */
	if (gmt) {
		timelib_update_ts(now, NULL);
	} else {
		timelib_update_ts(now, tzi);
	}

	/* Clean up and return */
	ts = timelib_date_to_int(now, &error);
	ts += adjust_seconds;
	timelib_time_dtor(now);

	if (error) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(ts);
	}
}
/* }}} */

/* {{{ proto int mktime([int hour [, int min [, int sec [, int mon [, int day [, int year]]]]]])
   Get UNIX timestamp for a date */
PHP_FUNCTION(mktime)
{
	php_mktime(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int gmmktime([int hour [, int min [, int sec [, int mon [, int day [, int year]]]]]])
   Get UNIX timestamp for a GMT date */
PHP_FUNCTION(gmmktime)
{
	php_mktime(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto bool checkdate(int month, int day, int year)
   Returns true(1) if it is a valid date in gregorian calendar */
PHP_FUNCTION(checkdate)
{
	zend_long m, d, y;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(m)
		Z_PARAM_LONG(d)
		Z_PARAM_LONG(y)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (y < 1 || y > 32767 || !timelib_valid_date(y, m, d)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;	/* True : This month, day, year arguments are valid */
}
/* }}} */

/* {{{ php_strftime - (gm)strftime helper */
PHPAPI void php_strftime(INTERNAL_FUNCTION_PARAMETERS, int gmt)
{
	zend_string         *format;
	zend_long            timestamp = 0;
	struct tm            ta;
	int                  max_reallocs = 5;
	size_t               buf_len = 256, real_len;
	timelib_time        *ts;
	timelib_tzinfo      *tzi;
	timelib_time_offset *offset = NULL;
	zend_string 		*buf;

	timestamp = (zend_long) php_time();

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(format)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(timestamp)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (ZSTR_LEN(format) == 0) {
		RETURN_FALSE;
	}

	ts = timelib_time_ctor();
	if (gmt) {
		tzi = NULL;
		timelib_unixtime2gmt(ts, (timelib_sll) timestamp);
	} else {
		tzi = get_timezone_info();
		ts->tz_info = tzi;
		ts->zone_type = TIMELIB_ZONETYPE_ID;
		timelib_unixtime2local(ts, (timelib_sll) timestamp);
	}
	ta.tm_sec   = ts->s;
	ta.tm_min   = ts->i;
	ta.tm_hour  = ts->h;
	ta.tm_mday  = ts->d;
	ta.tm_mon   = ts->m - 1;
	ta.tm_year  = ts->y - 1900;
	ta.tm_wday  = timelib_day_of_week(ts->y, ts->m, ts->d);
	ta.tm_yday  = timelib_day_of_year(ts->y, ts->m, ts->d);
	if (gmt) {
		ta.tm_isdst = 0;
#if HAVE_STRUCT_TM_TM_GMTOFF
		ta.tm_gmtoff = 0;
#endif
#if HAVE_STRUCT_TM_TM_ZONE
		ta.tm_zone = "GMT";
#endif
	} else {
		offset = timelib_get_time_zone_info(timestamp, tzi);

		ta.tm_isdst = offset->is_dst;
#if HAVE_STRUCT_TM_TM_GMTOFF
		ta.tm_gmtoff = offset->offset;
#endif
#if HAVE_STRUCT_TM_TM_ZONE
		ta.tm_zone = offset->abbr;
#endif
	}

	/* VS2012 crt has a bug where strftime crash with %z and %Z format when the
	   initial buffer is too small. See
	   http://connect.microsoft.com/VisualStudio/feedback/details/759720/vs2012-strftime-crash-with-z-formatting-code */
	buf = zend_string_alloc(buf_len, 0);
	while ((real_len = strftime(ZSTR_VAL(buf), buf_len, ZSTR_VAL(format), &ta)) == buf_len || real_len == 0) {
		buf_len *= 2;
		buf = zend_string_extend(buf, buf_len, 0);
		if (!--max_reallocs) {
			break;
		}
	}
#ifdef PHP_WIN32
	/* VS2012 strftime() returns number of characters, not bytes.
		See VC++11 bug id 766205. */
	if (real_len > 0) {
		real_len = strlen(buf->val);
	}
#endif

	timelib_time_dtor(ts);
	if (!gmt) {
		timelib_time_offset_dtor(offset);
	}

	if (real_len && real_len != buf_len) {
		buf = zend_string_truncate(buf, real_len, 0);
		RETURN_NEW_STR(buf);
	}
	zend_string_efree(buf);
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string strftime(string format [, int timestamp])
   Format a local time/date according to locale settings */
PHP_FUNCTION(strftime)
{
	php_strftime(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string gmstrftime(string format [, int timestamp])
   Format a GMT/UCT time/date according to locale settings */
PHP_FUNCTION(gmstrftime)
{
	php_strftime(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int time(void)
   Return current UNIX timestamp */
PHP_FUNCTION(time)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG((zend_long)php_time());
}
/* }}} */

/* {{{ proto array localtime([int timestamp [, bool associative_array]])
   Returns the results of the C system call localtime as an associative array if the associative_array argument is set to 1 other wise it is a regular array */
PHP_FUNCTION(localtime)
{
	zend_long timestamp;
	zend_bool associative = 0;
	timelib_tzinfo *tzi;
	timelib_time   *ts;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(timestamp)
		Z_PARAM_BOOL(associative)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (ZEND_NUM_ARGS() == 0) {
		timestamp = (zend_long) php_time();
	}

	tzi = get_timezone_info();
	ts = timelib_time_ctor();
	ts->tz_info = tzi;
	ts->zone_type = TIMELIB_ZONETYPE_ID;
	timelib_unixtime2local(ts, (timelib_sll) timestamp);

	array_init(return_value);

	if (associative) {
		add_assoc_long(return_value, "tm_sec",   ts->s);
		add_assoc_long(return_value, "tm_min",   ts->i);
		add_assoc_long(return_value, "tm_hour",  ts->h);
		add_assoc_long(return_value, "tm_mday",  ts->d);
		add_assoc_long(return_value, "tm_mon",   ts->m - 1);
		add_assoc_long(return_value, "tm_year",  ts->y - 1900);
		add_assoc_long(return_value, "tm_wday",  timelib_day_of_week(ts->y, ts->m, ts->d));
		add_assoc_long(return_value, "tm_yday",  timelib_day_of_year(ts->y, ts->m, ts->d));
		add_assoc_long(return_value, "tm_isdst", ts->dst);
	} else {
		add_next_index_long(return_value, ts->s);
		add_next_index_long(return_value, ts->i);
		add_next_index_long(return_value, ts->h);
		add_next_index_long(return_value, ts->d);
		add_next_index_long(return_value, ts->m - 1);
		add_next_index_long(return_value, ts->y- 1900);
		add_next_index_long(return_value, timelib_day_of_week(ts->y, ts->m, ts->d));
		add_next_index_long(return_value, timelib_day_of_year(ts->y, ts->m, ts->d));
		add_next_index_long(return_value, ts->dst);
	}

	timelib_time_dtor(ts);
}
/* }}} */

/* {{{ proto array getdate([int timestamp])
   Get date/time information */
PHP_FUNCTION(getdate)
{
	zend_long timestamp;
	timelib_tzinfo *tzi;
	timelib_time   *ts;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(timestamp)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (ZEND_NUM_ARGS() == 0) {
		timestamp = (zend_long) php_time();
	}

	tzi = get_timezone_info();
	ts = timelib_time_ctor();
	ts->tz_info = tzi;
	ts->zone_type = TIMELIB_ZONETYPE_ID;
	timelib_unixtime2local(ts, (timelib_sll) timestamp);

	array_init(return_value);

	add_assoc_long(return_value, "seconds", ts->s);
	add_assoc_long(return_value, "minutes", ts->i);
	add_assoc_long(return_value, "hours", ts->h);
	add_assoc_long(return_value, "mday", ts->d);
	add_assoc_long(return_value, "wday", timelib_day_of_week(ts->y, ts->m, ts->d));
	add_assoc_long(return_value, "mon", ts->m);
	add_assoc_long(return_value, "year", ts->y);
	add_assoc_long(return_value, "yday", timelib_day_of_year(ts->y, ts->m, ts->d));
	add_assoc_string(return_value, "weekday", php_date_full_day_name(ts->y, ts->m, ts->d));
	add_assoc_string(return_value, "month", mon_full_names[ts->m - 1]);
	add_index_long(return_value, 0, timestamp);

	timelib_time_dtor(ts);
}
/* }}} */

#define PHP_DATE_TIMEZONE_GROUP_AFRICA     0x0001
#define PHP_DATE_TIMEZONE_GROUP_AMERICA    0x0002
#define PHP_DATE_TIMEZONE_GROUP_ANTARCTICA 0x0004
#define PHP_DATE_TIMEZONE_GROUP_ARCTIC     0x0008
#define PHP_DATE_TIMEZONE_GROUP_ASIA       0x0010
#define PHP_DATE_TIMEZONE_GROUP_ATLANTIC   0x0020
#define PHP_DATE_TIMEZONE_GROUP_AUSTRALIA  0x0040
#define PHP_DATE_TIMEZONE_GROUP_EUROPE     0x0080
#define PHP_DATE_TIMEZONE_GROUP_INDIAN     0x0100
#define PHP_DATE_TIMEZONE_GROUP_PACIFIC    0x0200
#define PHP_DATE_TIMEZONE_GROUP_UTC        0x0400
#define PHP_DATE_TIMEZONE_GROUP_ALL        0x07FF
#define PHP_DATE_TIMEZONE_GROUP_ALL_W_BC   0x0FFF
#define PHP_DATE_TIMEZONE_PER_COUNTRY      0x1000

#define PHP_DATE_PERIOD_EXCLUDE_START_DATE 0x0001


/* define an overloaded iterator structure */
typedef struct {
	zend_object_iterator  intern;
	zval                  current;
	php_period_obj       *object;
	int                   current_index;
} date_period_it;

/* {{{ date_period_it_invalidate_current */
static void date_period_it_invalidate_current(zend_object_iterator *iter)
{
	date_period_it *iterator = (date_period_it *)iter;

	if (Z_TYPE(iterator->current) != IS_UNDEF) {
		zval_ptr_dtor(&iterator->current);
		ZVAL_UNDEF(&iterator->current);
	}
}
/* }}} */

/* {{{ date_period_it_dtor */
static void date_period_it_dtor(zend_object_iterator *iter)
{
	date_period_it *iterator = (date_period_it *)iter;

	date_period_it_invalidate_current(iter);

	zval_ptr_dtor(&iterator->intern.data);
}
/* }}} */

/* {{{ date_period_it_has_more */
static int date_period_it_has_more(zend_object_iterator *iter)
{
	date_period_it *iterator = (date_period_it *)iter;
	php_period_obj *object   = Z_PHPPERIOD_P(&iterator->intern.data);
	timelib_time   *it_time = object->current;

	/* apply modification if it's not the first iteration */
	if (!object->include_start_date || iterator->current_index > 0) {
		it_time->have_relative = 1;
		it_time->relative = *object->interval;
		it_time->sse_uptodate = 0;
		timelib_update_ts(it_time, NULL);
		timelib_update_from_sse(it_time);
	}

	if (object->end) {
		return object->current->sse < object->end->sse ? SUCCESS : FAILURE;
	} else {
		return (iterator->current_index < object->recurrences) ? SUCCESS : FAILURE;
	}
}
/* }}} */

/* {{{ date_period_it_current_data */
static zval *date_period_it_current_data(zend_object_iterator *iter)
{
	date_period_it *iterator = (date_period_it *)iter;
	php_period_obj *object   = Z_PHPPERIOD_P(&iterator->intern.data);
	timelib_time   *it_time = object->current;
	php_date_obj   *newdateobj;

	/* Create new object */
	php_date_instantiate(object->start_ce, &iterator->current);
	newdateobj = Z_PHPDATE_P(&iterator->current);
	newdateobj->time = timelib_time_ctor();
	*newdateobj->time = *it_time;
	if (it_time->tz_abbr) {
		newdateobj->time->tz_abbr = timelib_strdup(it_time->tz_abbr);
	}
	if (it_time->tz_info) {
		newdateobj->time->tz_info = it_time->tz_info;
	}

	return &iterator->current;
}
/* }}} */

/* {{{ date_period_it_current_key */
static void date_period_it_current_key(zend_object_iterator *iter, zval *key)
{
	date_period_it *iterator = (date_period_it *)iter;
	ZVAL_LONG(key, iterator->current_index);
}
/* }}} */

/* {{{ date_period_it_move_forward */
static void date_period_it_move_forward(zend_object_iterator *iter)
{
	date_period_it   *iterator = (date_period_it *)iter;

	iterator->current_index++;
	date_period_it_invalidate_current(iter);
}
/* }}} */

/* {{{ date_period_it_rewind */
static void date_period_it_rewind(zend_object_iterator *iter)
{
	date_period_it *iterator = (date_period_it *)iter;

	iterator->current_index = 0;
	if (iterator->object->current) {
		timelib_time_dtor(iterator->object->current);
	}
	if (!iterator->object->start) {
		zend_throw_error(NULL, "DatePeriod has not been initialized correctly");
		return;
	}
	iterator->object->current = timelib_time_clone(iterator->object->start);
	date_period_it_invalidate_current(iter);
}
/* }}} */

/* iterator handler table */
static const zend_object_iterator_funcs date_period_it_funcs = {
	date_period_it_dtor,
	date_period_it_has_more,
	date_period_it_current_data,
	date_period_it_current_key,
	date_period_it_move_forward,
	date_period_it_rewind,
	date_period_it_invalidate_current
};

zend_object_iterator *date_object_period_get_iterator(zend_class_entry *ce, zval *object, int by_ref) /* {{{ */
{
	date_period_it *iterator;

	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(date_period_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	Z_ADDREF_P(object);
	ZVAL_OBJ(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &date_period_it_funcs;
	iterator->object = Z_PHPPERIOD_P(object);
	ZVAL_UNDEF(&iterator->current);

	return (zend_object_iterator*)iterator;
} /* }}} */

static int implement_date_interface_handler(zend_class_entry *interface, zend_class_entry *implementor) /* {{{ */
{
	if (implementor->type == ZEND_USER_CLASS &&
		!instanceof_function(implementor, date_ce_date) &&
		!instanceof_function(implementor, date_ce_immutable)
	) {
		zend_error(E_ERROR, "DateTimeInterface can't be implemented by user classes");
	}

	return SUCCESS;
} /* }}} */

static int date_interval_has_property(zval *object, zval *member, int type, void **cache_slot) /* {{{ */
{
	php_interval_obj *obj;
	zval tmp_member;
	zval rv;
	zval *prop;
	int retval = 0;

	if (UNEXPECTED(Z_TYPE_P(member) != IS_STRING)) {
		zend_string *str = zval_try_get_string_func(member);
		if (UNEXPECTED(!str)) {
			return 0;
		}
		ZVAL_STR(&tmp_member, str);
		member = &tmp_member;
		cache_slot = NULL;
	}

	obj = Z_PHPINTERVAL_P(object);

	if (!obj->initialized) {
		retval = zend_std_has_property(object, member, type, cache_slot);
		if (member == &tmp_member) {
			zval_ptr_dtor_str(&tmp_member);
		}
		return retval;
	}

	prop = date_interval_read_property(object, member, BP_VAR_IS, cache_slot, &rv);

	if (prop != &EG(uninitialized_zval)) {
		if (type == 2) {
			retval = 1;
		} else if (type == 1) {
			retval = zend_is_true(prop);
		} else if (type == 0) {
			retval = (Z_TYPE_P(prop) != IS_NULL);
		}
	} else {
		retval = zend_std_has_property(object, member, type, cache_slot);
	}

	if (member == &tmp_member) {
		zval_ptr_dtor_str(&tmp_member);
	}

	return retval;

}
/* }}} */

static void date_register_classes(void) /* {{{ */
{
	zend_class_entry ce_date, ce_immutable, ce_timezone, ce_interval, ce_period, ce_interface;

	INIT_CLASS_ENTRY(ce_interface, "DateTimeInterface", date_funcs_interface);
	date_ce_interface = zend_register_internal_interface(&ce_interface);
	date_ce_interface->interface_gets_implemented = implement_date_interface_handler;

#define REGISTER_DATE_INTERFACE_CONST_STRING(const_name, value) \
	zend_declare_class_constant_stringl(date_ce_interface, const_name, sizeof(const_name)-1, value, sizeof(value)-1);

	REGISTER_DATE_INTERFACE_CONST_STRING("ATOM",             DATE_FORMAT_RFC3339);
	REGISTER_DATE_INTERFACE_CONST_STRING("COOKIE",           DATE_FORMAT_COOKIE);
	REGISTER_DATE_INTERFACE_CONST_STRING("ISO8601",          DATE_FORMAT_ISO8601);
	REGISTER_DATE_INTERFACE_CONST_STRING("RFC822",           DATE_FORMAT_RFC822);
	REGISTER_DATE_INTERFACE_CONST_STRING("RFC850",           DATE_FORMAT_RFC850);
	REGISTER_DATE_INTERFACE_CONST_STRING("RFC1036",          DATE_FORMAT_RFC1036);
	REGISTER_DATE_INTERFACE_CONST_STRING("RFC1123",          DATE_FORMAT_RFC1123);
	REGISTER_DATE_INTERFACE_CONST_STRING("RFC7231",          DATE_FORMAT_RFC7231);
	REGISTER_DATE_INTERFACE_CONST_STRING("RFC2822",          DATE_FORMAT_RFC2822);
	REGISTER_DATE_INTERFACE_CONST_STRING("RFC3339",          DATE_FORMAT_RFC3339);
	REGISTER_DATE_INTERFACE_CONST_STRING("RFC3339_EXTENDED", DATE_FORMAT_RFC3339_EXTENDED);
	REGISTER_DATE_INTERFACE_CONST_STRING("RSS",              DATE_FORMAT_RFC1123);
	REGISTER_DATE_INTERFACE_CONST_STRING("W3C",              DATE_FORMAT_RFC3339);

	INIT_CLASS_ENTRY(ce_date, "DateTime", date_funcs_date);
	ce_date.create_object = date_object_new_date;
	date_ce_date = zend_register_internal_class_ex(&ce_date, NULL);
	memcpy(&date_object_handlers_date, &std_object_handlers, sizeof(zend_object_handlers));
	date_object_handlers_date.offset = XtOffsetOf(php_date_obj, std);
	date_object_handlers_date.free_obj = date_object_free_storage_date;
	date_object_handlers_date.clone_obj = date_object_clone_date;
	date_object_handlers_date.compare_objects = date_object_compare_date;
	date_object_handlers_date.get_properties_for = date_object_get_properties_for;
	date_object_handlers_date.get_gc = date_object_get_gc;
	zend_class_implements(date_ce_date, 1, date_ce_interface);

	INIT_CLASS_ENTRY(ce_immutable, "DateTimeImmutable", date_funcs_immutable);
	ce_immutable.create_object = date_object_new_date;
	date_ce_immutable = zend_register_internal_class_ex(&ce_immutable, NULL);
	memcpy(&date_object_handlers_immutable, &std_object_handlers, sizeof(zend_object_handlers));
	date_object_handlers_immutable.clone_obj = date_object_clone_date;
	date_object_handlers_immutable.compare_objects = date_object_compare_date;
	date_object_handlers_immutable.get_properties_for = date_object_get_properties_for;
	date_object_handlers_immutable.get_gc = date_object_get_gc;
	zend_class_implements(date_ce_immutable, 1, date_ce_interface);

	INIT_CLASS_ENTRY(ce_timezone, "DateTimeZone", date_funcs_timezone);
	ce_timezone.create_object = date_object_new_timezone;
	date_ce_timezone = zend_register_internal_class_ex(&ce_timezone, NULL);
	memcpy(&date_object_handlers_timezone, &std_object_handlers, sizeof(zend_object_handlers));
	date_object_handlers_timezone.offset = XtOffsetOf(php_timezone_obj, std);
	date_object_handlers_timezone.free_obj = date_object_free_storage_timezone;
	date_object_handlers_timezone.clone_obj = date_object_clone_timezone;
	date_object_handlers_timezone.compare_objects = date_object_compare_timezone;
	date_object_handlers_timezone.get_properties_for = date_object_get_properties_for_timezone;
	date_object_handlers_timezone.get_gc = date_object_get_gc_timezone;
	date_object_handlers_timezone.get_debug_info = date_object_get_debug_info_timezone;

#define REGISTER_TIMEZONE_CLASS_CONST_STRING(const_name, value) \
	zend_declare_class_constant_long(date_ce_timezone, const_name, sizeof(const_name)-1, value);

	REGISTER_TIMEZONE_CLASS_CONST_STRING("AFRICA",      PHP_DATE_TIMEZONE_GROUP_AFRICA);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("AMERICA",     PHP_DATE_TIMEZONE_GROUP_AMERICA);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("ANTARCTICA",  PHP_DATE_TIMEZONE_GROUP_ANTARCTICA);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("ARCTIC",      PHP_DATE_TIMEZONE_GROUP_ARCTIC);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("ASIA",        PHP_DATE_TIMEZONE_GROUP_ASIA);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("ATLANTIC",    PHP_DATE_TIMEZONE_GROUP_ATLANTIC);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("AUSTRALIA",   PHP_DATE_TIMEZONE_GROUP_AUSTRALIA);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("EUROPE",      PHP_DATE_TIMEZONE_GROUP_EUROPE);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("INDIAN",      PHP_DATE_TIMEZONE_GROUP_INDIAN);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("PACIFIC",     PHP_DATE_TIMEZONE_GROUP_PACIFIC);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("UTC",         PHP_DATE_TIMEZONE_GROUP_UTC);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("ALL",         PHP_DATE_TIMEZONE_GROUP_ALL);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("ALL_WITH_BC", PHP_DATE_TIMEZONE_GROUP_ALL_W_BC);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("PER_COUNTRY", PHP_DATE_TIMEZONE_PER_COUNTRY);

	INIT_CLASS_ENTRY(ce_interval, "DateInterval", date_funcs_interval);
	ce_interval.create_object = date_object_new_interval;
	date_ce_interval = zend_register_internal_class_ex(&ce_interval, NULL);
	memcpy(&date_object_handlers_interval, &std_object_handlers, sizeof(zend_object_handlers));
	date_object_handlers_interval.offset = XtOffsetOf(php_interval_obj, std);
	date_object_handlers_interval.free_obj = date_object_free_storage_interval;
	date_object_handlers_interval.clone_obj = date_object_clone_interval;
	date_object_handlers_interval.has_property = date_interval_has_property;
	date_object_handlers_interval.read_property = date_interval_read_property;
	date_object_handlers_interval.write_property = date_interval_write_property;
	date_object_handlers_interval.get_properties = date_object_get_properties_interval;
	date_object_handlers_interval.get_property_ptr_ptr = date_interval_get_property_ptr_ptr;
	date_object_handlers_interval.get_gc = date_object_get_gc_interval;
	date_object_handlers_interval.compare_objects = date_interval_compare_objects;

	INIT_CLASS_ENTRY(ce_period, "DatePeriod", date_funcs_period);
	ce_period.create_object = date_object_new_period;
	date_ce_period = zend_register_internal_class_ex(&ce_period, NULL);
	date_ce_period->get_iterator = date_object_period_get_iterator;
	zend_class_implements(date_ce_period, 1, zend_ce_traversable);
	memcpy(&date_object_handlers_period, &std_object_handlers, sizeof(zend_object_handlers));
	date_object_handlers_period.offset = XtOffsetOf(php_period_obj, std);
	date_object_handlers_period.free_obj = date_object_free_storage_period;
	date_object_handlers_period.clone_obj = date_object_clone_period;
	date_object_handlers_period.get_properties = date_object_get_properties_period;
	date_object_handlers_period.get_property_ptr_ptr = date_period_get_property_ptr_ptr;
	date_object_handlers_period.get_gc = date_object_get_gc_period;
	date_object_handlers_period.read_property = date_period_read_property;
	date_object_handlers_period.write_property = date_period_write_property;

#define REGISTER_PERIOD_CLASS_CONST_STRING(const_name, value) \
	zend_declare_class_constant_long(date_ce_period, const_name, sizeof(const_name)-1, value);

	REGISTER_PERIOD_CLASS_CONST_STRING("EXCLUDE_START_DATE", PHP_DATE_PERIOD_EXCLUDE_START_DATE);
} /* }}} */

static zend_object *date_object_new_date(zend_class_entry *class_type) /* {{{ */
{
	php_date_obj *intern = zend_object_alloc(sizeof(php_date_obj), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &date_object_handlers_date;

	return &intern->std;
} /* }}} */

static zend_object *date_object_clone_date(zval *this_ptr) /* {{{ */
{
	php_date_obj *old_obj = Z_PHPDATE_P(this_ptr);
	php_date_obj *new_obj = php_date_obj_from_obj(date_object_new_date(old_obj->std.ce));

	zend_objects_clone_members(&new_obj->std, &old_obj->std);
	if (!old_obj->time) {
		return &new_obj->std;
	}

	/* this should probably moved to a new `timelib_time *timelime_time_clone(timelib_time *)` */
	new_obj->time = timelib_time_ctor();
	*new_obj->time = *old_obj->time;
	if (old_obj->time->tz_abbr) {
		new_obj->time->tz_abbr = timelib_strdup(old_obj->time->tz_abbr);
	}
	if (old_obj->time->tz_info) {
		new_obj->time->tz_info = old_obj->time->tz_info;
	}

	return &new_obj->std;
} /* }}} */

static void date_clone_immutable(zval *object, zval *new_object) /* {{{ */
{
	ZVAL_OBJ(new_object, date_object_clone_date(object));
} /* }}} */

static int date_object_compare_date(zval *d1, zval *d2) /* {{{ */
{
	php_date_obj *o1 = Z_PHPDATE_P(d1);
	php_date_obj *o2 = Z_PHPDATE_P(d2);

	if (!o1->time || !o2->time) {
		php_error_docref(NULL, E_WARNING, "Trying to compare an incomplete DateTime or DateTimeImmutable object");
		return 1;
	}
	if (!o1->time->sse_uptodate) {
		timelib_update_ts(o1->time, o1->time->tz_info);
	}
	if (!o2->time->sse_uptodate) {
		timelib_update_ts(o2->time, o2->time->tz_info);
	}

	return timelib_time_compare(o1->time, o2->time);
} /* }}} */

static HashTable *date_object_get_gc(zval *object, zval **table, int *n) /* {{{ */
{
	*table = NULL;
	*n = 0;
	return zend_std_get_properties(object);
} /* }}} */

static HashTable *date_object_get_gc_timezone(zval *object, zval **table, int *n) /* {{{ */
{
       *table = NULL;
       *n = 0;
       return zend_std_get_properties(object);
} /* }}} */

static HashTable *date_object_get_properties_for(zval *object, zend_prop_purpose purpose) /* {{{ */
{
	HashTable *props;
	zval zv;
	php_date_obj *dateobj;

	switch (purpose) {
		case ZEND_PROP_PURPOSE_DEBUG:
		case ZEND_PROP_PURPOSE_SERIALIZE:
		case ZEND_PROP_PURPOSE_VAR_EXPORT:
		case ZEND_PROP_PURPOSE_JSON:
		case ZEND_PROP_PURPOSE_ARRAY_CAST:
			break;
		default:
			return zend_std_get_properties_for(object, purpose);
	}

	dateobj = Z_PHPDATE_P(object);
	props = zend_array_dup(zend_std_get_properties(object));
	if (!dateobj->time) {
		return props;
	}

	/* first we add the date and time in ISO format */
	ZVAL_STR(&zv, date_format("Y-m-d H:i:s.u", sizeof("Y-m-d H:i:s.u")-1, dateobj->time, 1));
	zend_hash_str_update(props, "date", sizeof("date")-1, &zv);

	/* then we add the timezone name (or similar) */
	if (dateobj->time->is_localtime) {
		ZVAL_LONG(&zv, dateobj->time->zone_type);
		zend_hash_str_update(props, "timezone_type", sizeof("timezone_type")-1, &zv);

		switch (dateobj->time->zone_type) {
			case TIMELIB_ZONETYPE_ID:
				ZVAL_STRING(&zv, dateobj->time->tz_info->name);
				break;
			case TIMELIB_ZONETYPE_OFFSET: {
				zend_string *tmpstr = zend_string_alloc(sizeof("UTC+05:00")-1, 0);
				int utc_offset = dateobj->time->z;

				ZSTR_LEN(tmpstr) = snprintf(ZSTR_VAL(tmpstr), sizeof("+05:00"), "%c%02d:%02d",
					utc_offset < 0 ? '-' : '+',
					abs(utc_offset / 3600),
					abs(((utc_offset % 3600) / 60)));

				ZVAL_NEW_STR(&zv, tmpstr);
				}
				break;
			case TIMELIB_ZONETYPE_ABBR:
				ZVAL_STRING(&zv, dateobj->time->tz_abbr);
				break;
		}
		zend_hash_str_update(props, "timezone", sizeof("timezone")-1, &zv);
	}

	return props;
} /* }}} */

static zend_object *date_object_new_timezone(zend_class_entry *class_type) /* {{{ */
{
	php_timezone_obj *intern = zend_object_alloc(sizeof(php_timezone_obj), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &date_object_handlers_timezone;

	return &intern->std;
} /* }}} */

static zend_object *date_object_clone_timezone(zval *this_ptr) /* {{{ */
{
	php_timezone_obj *old_obj = Z_PHPTIMEZONE_P(this_ptr);
	php_timezone_obj *new_obj = php_timezone_obj_from_obj(date_object_new_timezone(old_obj->std.ce));

	zend_objects_clone_members(&new_obj->std, &old_obj->std);
	if (!old_obj->initialized) {
		return &new_obj->std;
	}

	new_obj->type = old_obj->type;
	new_obj->initialized = 1;
	switch (new_obj->type) {
		case TIMELIB_ZONETYPE_ID:
			new_obj->tzi.tz = old_obj->tzi.tz;
			break;
		case TIMELIB_ZONETYPE_OFFSET:
			new_obj->tzi.utc_offset = old_obj->tzi.utc_offset;
			break;
		case TIMELIB_ZONETYPE_ABBR:
			new_obj->tzi.z.utc_offset = old_obj->tzi.z.utc_offset;
			new_obj->tzi.z.dst        = old_obj->tzi.z.dst;
			new_obj->tzi.z.abbr       = timelib_strdup(old_obj->tzi.z.abbr);
			break;
	}

	return &new_obj->std;
} /* }}} */

static int date_object_compare_timezone(zval *tz1, zval *tz2) /* {{{ */
{
	php_timezone_obj *o1, *o2;

	o1 = Z_PHPTIMEZONE_P(tz1);
	o2 = Z_PHPTIMEZONE_P(tz2);

	ZEND_ASSERT(o1->initialized && o2->initialized);

	if (o1->type != o2->type) {
		php_error_docref(NULL, E_WARNING, "Trying to compare different kinds of DateTimeZone objects");
		return 1;
	}

	switch (o1->type) {
		case TIMELIB_ZONETYPE_OFFSET:
			return o1->tzi.utc_offset == o2->tzi.utc_offset ? 0 : 1;
		case TIMELIB_ZONETYPE_ABBR:
			return strcmp(o1->tzi.z.abbr, o2->tzi.z.abbr) ? 1 : 0;
		case TIMELIB_ZONETYPE_ID:
			return strcmp(o1->tzi.tz->name, o2->tzi.tz->name) ? 1 : 0;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
} /* }}} */

static void php_timezone_to_string(php_timezone_obj *tzobj, zval *zv)
{
	switch (tzobj->type) {
		case TIMELIB_ZONETYPE_ID:
			ZVAL_STRING(zv, tzobj->tzi.tz->name);
			break;
		case TIMELIB_ZONETYPE_OFFSET: {
			zend_string *tmpstr = zend_string_alloc(sizeof("UTC+05:00")-1, 0);
			timelib_sll utc_offset = tzobj->tzi.utc_offset;

			ZSTR_LEN(tmpstr) = snprintf(ZSTR_VAL(tmpstr), sizeof("+05:00"), "%c%02d:%02d",
				utc_offset < 0 ? '-' : '+',
				abs((int)(utc_offset / 3600)),
				abs((int)(utc_offset % 3600) / 60));

			ZVAL_NEW_STR(zv, tmpstr);
			}
			break;
		case TIMELIB_ZONETYPE_ABBR:
			ZVAL_STRING(zv, tzobj->tzi.z.abbr);
			break;
	}
}

static HashTable *date_object_get_properties_for_timezone(zval *object, zend_prop_purpose purpose) /* {{{ */
{
	HashTable *props;
	zval zv;
	php_timezone_obj *tzobj;

	switch (purpose) {
		case ZEND_PROP_PURPOSE_DEBUG:
		case ZEND_PROP_PURPOSE_SERIALIZE:
		case ZEND_PROP_PURPOSE_VAR_EXPORT:
		case ZEND_PROP_PURPOSE_JSON:
		case ZEND_PROP_PURPOSE_ARRAY_CAST:
			break;
		default:
			return zend_std_get_properties_for(object, purpose);
	}

	tzobj = Z_PHPTIMEZONE_P(object);
	props = zend_array_dup(zend_std_get_properties(object));
	if (!tzobj->initialized) {
		return props;
	}

	ZVAL_LONG(&zv, tzobj->type);
	zend_hash_str_update(props, "timezone_type", sizeof("timezone_type")-1, &zv);

	php_timezone_to_string(tzobj, &zv);
	zend_hash_str_update(props, "timezone", sizeof("timezone")-1, &zv);

	return props;
} /* }}} */

static HashTable *date_object_get_debug_info_timezone(zval *object, int *is_temp) /* {{{ */
{
	HashTable *ht, *props;
	zval zv;
	php_timezone_obj *tzobj;

	tzobj = Z_PHPTIMEZONE_P(object);
	props = zend_std_get_properties(object);

	*is_temp = 1;
	ht = zend_array_dup(props);

	ZVAL_LONG(&zv, tzobj->type);
	zend_hash_str_update(ht, "timezone_type", sizeof("timezone_type")-1, &zv);

	php_timezone_to_string(tzobj, &zv);
	zend_hash_str_update(ht, "timezone", sizeof("timezone")-1, &zv);

	return ht;
} /* }}} */

static zend_object *date_object_new_interval(zend_class_entry *class_type) /* {{{ */
{
	php_interval_obj *intern = zend_object_alloc(sizeof(php_interval_obj), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &date_object_handlers_interval;

	return &intern->std;
} /* }}} */

static zend_object *date_object_clone_interval(zval *this_ptr) /* {{{ */
{
	php_interval_obj *old_obj = Z_PHPINTERVAL_P(this_ptr);
	php_interval_obj *new_obj = php_interval_obj_from_obj(date_object_new_interval(old_obj->std.ce));

	zend_objects_clone_members(&new_obj->std, &old_obj->std);
	new_obj->initialized = old_obj->initialized;
	if (old_obj->diff) {
		new_obj->diff = timelib_rel_time_clone(old_obj->diff);
	}

	return &new_obj->std;
} /* }}} */

static HashTable *date_object_get_gc_interval(zval *object, zval **table, int *n) /* {{{ */
{

	*table = NULL;
	*n = 0;
	return zend_std_get_properties(object);
} /* }}} */

static HashTable *date_object_get_properties_interval(zval *object) /* {{{ */
{
	HashTable *props;
	zval zv;
	php_interval_obj *intervalobj;

	intervalobj = Z_PHPINTERVAL_P(object);
	props = zend_std_get_properties(object);
	if (!intervalobj->initialized) {
		return props;
	}

#define PHP_DATE_INTERVAL_ADD_PROPERTY(n,f) \
	ZVAL_LONG(&zv, (zend_long)intervalobj->diff->f); \
	zend_hash_str_update(props, n, sizeof(n)-1, &zv);

	PHP_DATE_INTERVAL_ADD_PROPERTY("y", y);
	PHP_DATE_INTERVAL_ADD_PROPERTY("m", m);
	PHP_DATE_INTERVAL_ADD_PROPERTY("d", d);
	PHP_DATE_INTERVAL_ADD_PROPERTY("h", h);
	PHP_DATE_INTERVAL_ADD_PROPERTY("i", i);
	PHP_DATE_INTERVAL_ADD_PROPERTY("s", s);
	ZVAL_DOUBLE(&zv, (double)intervalobj->diff->us / 1000000.0);
	zend_hash_str_update(props, "f", sizeof("f") - 1, &zv);
	PHP_DATE_INTERVAL_ADD_PROPERTY("weekday", weekday);
	PHP_DATE_INTERVAL_ADD_PROPERTY("weekday_behavior", weekday_behavior);
	PHP_DATE_INTERVAL_ADD_PROPERTY("first_last_day_of", first_last_day_of);
	PHP_DATE_INTERVAL_ADD_PROPERTY("invert", invert);
	if (intervalobj->diff->days != -99999) {
		PHP_DATE_INTERVAL_ADD_PROPERTY("days", days);
	} else {
		ZVAL_FALSE(&zv);
		zend_hash_str_update(props, "days", sizeof("days")-1, &zv);
	}
	PHP_DATE_INTERVAL_ADD_PROPERTY("special_type", special.type);
	PHP_DATE_INTERVAL_ADD_PROPERTY("special_amount", special.amount);
	PHP_DATE_INTERVAL_ADD_PROPERTY("have_weekday_relative", have_weekday_relative);
	PHP_DATE_INTERVAL_ADD_PROPERTY("have_special_relative", have_special_relative);

	return props;
} /* }}} */

static zend_object *date_object_new_period(zend_class_entry *class_type) /* {{{ */
{
	php_period_obj *intern = zend_object_alloc(sizeof(php_period_obj), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	intern->std.handlers = &date_object_handlers_period;

	return &intern->std;
} /* }}} */

static zend_object *date_object_clone_period(zval *this_ptr) /* {{{ */
{
	php_period_obj *old_obj = Z_PHPPERIOD_P(this_ptr);
	php_period_obj *new_obj = php_period_obj_from_obj(date_object_new_period(old_obj->std.ce));

	zend_objects_clone_members(&new_obj->std, &old_obj->std);
	new_obj->initialized = old_obj->initialized;
	new_obj->recurrences = old_obj->recurrences;
	new_obj->include_start_date = old_obj->include_start_date;
	new_obj->start_ce = old_obj->start_ce;

	if (old_obj->start) {
		new_obj->start = timelib_time_clone(old_obj->start);
	}
	if (old_obj->current) {
		new_obj->current = timelib_time_clone(old_obj->current);
	}
	if (old_obj->end) {
        new_obj->end = timelib_time_clone(old_obj->end);
    }
    if (old_obj->interval) {
        new_obj->interval = timelib_rel_time_clone(old_obj->interval);
    }
	return &new_obj->std;
} /* }}} */

static void date_object_free_storage_date(zend_object *object) /* {{{ */
{
	php_date_obj *intern = php_date_obj_from_obj(object);

	if (intern->time) {
		timelib_time_dtor(intern->time);
	}

	zend_object_std_dtor(&intern->std);
} /* }}} */

static void date_object_free_storage_timezone(zend_object *object) /* {{{ */
{
	php_timezone_obj *intern = php_timezone_obj_from_obj(object);

	if (intern->type == TIMELIB_ZONETYPE_ABBR) {
		timelib_free(intern->tzi.z.abbr);
	}
	zend_object_std_dtor(&intern->std);
} /* }}} */

static void date_object_free_storage_interval(zend_object *object) /* {{{ */
{
	php_interval_obj *intern = php_interval_obj_from_obj(object);

	timelib_rel_time_dtor(intern->diff);
	zend_object_std_dtor(&intern->std);
} /* }}} */

static void date_object_free_storage_period(zend_object *object) /* {{{ */
{
	php_period_obj *intern = php_period_obj_from_obj(object);

	if (intern->start) {
		timelib_time_dtor(intern->start);
	}

	if (intern->current) {
		timelib_time_dtor(intern->current);
	}

	if (intern->end) {
		timelib_time_dtor(intern->end);
	}

	timelib_rel_time_dtor(intern->interval);
	zend_object_std_dtor(&intern->std);
} /* }}} */

/* Advanced Interface */
PHPAPI zval *php_date_instantiate(zend_class_entry *pce, zval *object) /* {{{ */
{
	object_init_ex(object, pce);
	return object;
} /* }}} */

/* Helper function used to store the latest found warnings and errors while
 * parsing, from either strtotime or parse_from_format. */
static void update_errors_warnings(timelib_error_container *last_errors) /* {{{ */
{
	if (DATEG(last_errors)) {
		timelib_error_container_dtor(DATEG(last_errors));
		DATEG(last_errors) = NULL;
	}
	DATEG(last_errors) = last_errors;
} /* }}} */

static void php_date_set_time_fraction(timelib_time *time, int microseconds)
{
	time->us = microseconds;
}

static void php_date_get_current_time_with_fraction(time_t *sec, suseconds_t *usec)
{
#if HAVE_GETTIMEOFDAY
	struct timeval tp = {0}; /* For setting microseconds */

	gettimeofday(&tp, NULL);
	*sec = tp.tv_sec;
	*usec = tp.tv_usec;
#else
	*sec = time(NULL);
	*usec = 0;
#endif
}

PHPAPI int php_date_initialize(php_date_obj *dateobj, /*const*/ char *time_str, size_t time_str_len, char *format, zval *timezone_object, int ctor) /* {{{ */
{
	timelib_time   *now;
	timelib_tzinfo *tzi = NULL;
	timelib_error_container *err = NULL;
	int type = TIMELIB_ZONETYPE_ID, new_dst = 0;
	char *new_abbr = NULL;
	timelib_sll new_offset = 0;
	time_t sec;
	suseconds_t usec;

	if (dateobj->time) {
		timelib_time_dtor(dateobj->time);
	}
	if (format) {
		dateobj->time = timelib_parse_from_format(format, time_str_len ? time_str : "", time_str_len ? time_str_len : 0, &err, DATE_TIMEZONEDB, php_date_parse_tzfile_wrapper);
	} else {
		dateobj->time = timelib_strtotime(time_str_len ? time_str : "now", time_str_len ? time_str_len : sizeof("now") -1, &err, DATE_TIMEZONEDB, php_date_parse_tzfile_wrapper);
	}

	/* update last errors and warnings */
	update_errors_warnings(err);


	if (ctor && err && err->error_count) {
		/* spit out the first library error message, at least */
		php_error_docref(NULL, E_WARNING, "Failed to parse time string (%s) at position %d (%c): %s", time_str,
			err->error_messages[0].position, err->error_messages[0].character, err->error_messages[0].message);
	}
	if (err && err->error_count) {
		timelib_time_dtor(dateobj->time);
		dateobj->time = 0;
		return 0;
	}

	if (timezone_object) {
		php_timezone_obj *tzobj;

		tzobj = Z_PHPTIMEZONE_P(timezone_object);
		switch (tzobj->type) {
			case TIMELIB_ZONETYPE_ID:
				tzi = tzobj->tzi.tz;
				break;
			case TIMELIB_ZONETYPE_OFFSET:
				new_offset = tzobj->tzi.utc_offset;
				break;
			case TIMELIB_ZONETYPE_ABBR:
				new_offset = tzobj->tzi.z.utc_offset;
				new_dst    = tzobj->tzi.z.dst;
				new_abbr   = timelib_strdup(tzobj->tzi.z.abbr);
				break;
		}
		type = tzobj->type;
	} else if (dateobj->time->tz_info) {
		tzi = dateobj->time->tz_info;
	} else {
		tzi = get_timezone_info();
	}

	now = timelib_time_ctor();
	now->zone_type = type;
	switch (type) {
		case TIMELIB_ZONETYPE_ID:
			now->tz_info = tzi;
			break;
		case TIMELIB_ZONETYPE_OFFSET:
			now->z = new_offset;
			break;
		case TIMELIB_ZONETYPE_ABBR:
			now->z = new_offset;
			now->dst = new_dst;
			now->tz_abbr = new_abbr;
			break;
	}
	php_date_get_current_time_with_fraction(&sec, &usec);
	timelib_unixtime2local(now, (timelib_sll) sec);
	php_date_set_time_fraction(now, usec);
	timelib_fill_holes(dateobj->time, now, TIMELIB_NO_CLONE);
	timelib_update_ts(dateobj->time, tzi);
	timelib_update_from_sse(dateobj->time);

	dateobj->time->have_relative = 0;

	timelib_time_dtor(now);

	return 1;
} /* }}} */

/* {{{ proto DateTime date_create([string time[, DateTimeZone object]])
   Returns new DateTime object
*/
PHP_FUNCTION(date_create)
{
	zval           *timezone_object = NULL;
	char           *time_str = NULL;
	size_t          time_str_len = 0;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(time_str, time_str_len)
		Z_PARAM_OBJECT_OF_CLASS_EX(timezone_object, date_ce_timezone, 1, 0)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	php_date_instantiate(date_ce_date, return_value);
	if (!php_date_initialize(Z_PHPDATE_P(return_value), time_str, time_str_len, NULL, timezone_object, 0)) {
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto DateTime date_create_immutable([string time[, DateTimeZone object]])
   Returns new DateTime object
*/
PHP_FUNCTION(date_create_immutable)
{
	zval           *timezone_object = NULL;
	char           *time_str = NULL;
	size_t          time_str_len = 0;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(time_str, time_str_len)
		Z_PARAM_OBJECT_OF_CLASS_EX(timezone_object, date_ce_timezone, 1, 0)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	php_date_instantiate(date_ce_immutable, return_value);
	if (!php_date_initialize(Z_PHPDATE_P(return_value), time_str, time_str_len, NULL, timezone_object, 0)) {
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto DateTime date_create_from_format(string format, string time[, DateTimeZone object])
   Returns new DateTime object formatted according to the specified format
*/
PHP_FUNCTION(date_create_from_format)
{
	zval           *timezone_object = NULL;
	char           *time_str = NULL, *format_str = NULL;
	size_t          time_str_len = 0, format_str_len = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(format_str, format_str_len)
		Z_PARAM_STRING(time_str, time_str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS_EX(timezone_object, date_ce_timezone, 1, 0)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	php_date_instantiate(date_ce_date, return_value);
	if (!php_date_initialize(Z_PHPDATE_P(return_value), time_str, time_str_len, format_str, timezone_object, 0)) {
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto DateTime date_create_immutable_from_format(string format, string time[, DateTimeZone object])
   Returns new DateTime object formatted according to the specified format
*/
PHP_FUNCTION(date_create_immutable_from_format)
{
	zval           *timezone_object = NULL;
	char           *time_str = NULL, *format_str = NULL;
	size_t          time_str_len = 0, format_str_len = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(format_str, format_str_len)
		Z_PARAM_STRING(time_str, time_str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS_EX(timezone_object, date_ce_timezone, 1, 0)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	php_date_instantiate(date_ce_immutable, return_value);
	if (!php_date_initialize(Z_PHPDATE_P(return_value), time_str, time_str_len, format_str, timezone_object, 0)) {
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto DateTime::__construct([string time[, DateTimeZone object]])
   Creates new DateTime object
*/
PHP_METHOD(DateTime, __construct)
{
	zval *timezone_object = NULL;
	char *time_str = NULL;
	size_t time_str_len = 0;
	zend_error_handling error_handling;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(time_str, time_str_len)
		Z_PARAM_OBJECT_OF_CLASS_EX(timezone_object, date_ce_timezone, 1, 0)
	ZEND_PARSE_PARAMETERS_END();

	zend_replace_error_handling(EH_THROW, NULL, &error_handling);
	php_date_initialize(Z_PHPDATE_P(ZEND_THIS), time_str, time_str_len, NULL, timezone_object, 1);
	zend_restore_error_handling(&error_handling);
}
/* }}} */

/* {{{ proto DateTimeImmutable::__construct([string time[, DateTimeZone object]])
   Creates new DateTimeImmutable object
*/
PHP_METHOD(DateTimeImmutable, __construct)
{
	zval *timezone_object = NULL;
	char *time_str = NULL;
	size_t time_str_len = 0;
	zend_error_handling error_handling;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(time_str, time_str_len)
		Z_PARAM_OBJECT_OF_CLASS_EX(timezone_object, date_ce_timezone, 1, 0)
	ZEND_PARSE_PARAMETERS_END();

	zend_replace_error_handling(EH_THROW, NULL, &error_handling);
	php_date_initialize(Z_PHPDATE_P(ZEND_THIS), time_str, time_str_len, NULL, timezone_object, 1);
	zend_restore_error_handling(&error_handling);
}
/* }}} */

/* {{{ proto DateTime::createFromImmutable(DateTimeImmutable object)
   Creates new DateTime object from an existing immutable DateTimeImmutable object.
*/
PHP_METHOD(DateTime, createFromImmutable)
{
	zval *datetimeimmutable_object = NULL;
	php_date_obj *new_obj = NULL;
	php_date_obj *old_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(datetimeimmutable_object, date_ce_immutable)
	ZEND_PARSE_PARAMETERS_END();

	php_date_instantiate(date_ce_date, return_value);
	old_obj = Z_PHPDATE_P(datetimeimmutable_object);
	new_obj = Z_PHPDATE_P(return_value);

	new_obj->time = timelib_time_clone(old_obj->time);
}
/* }}} */

/* {{{ proto DateTimeImmutable::createFromMutable(DateTime object)
   Creates new DateTimeImmutable object from an existing mutable DateTime object.
*/
PHP_METHOD(DateTimeImmutable, createFromMutable)
{
	zval *datetime_object = NULL;
	php_date_obj *new_obj = NULL;
	php_date_obj *old_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(datetime_object, date_ce_date)
	ZEND_PARSE_PARAMETERS_END();

	php_date_instantiate(date_ce_immutable, return_value);
	old_obj = Z_PHPDATE_P(datetime_object);
	new_obj = Z_PHPDATE_P(return_value);

	new_obj->time = timelib_time_clone(old_obj->time);
}
/* }}} */

static int php_date_initialize_from_hash(php_date_obj **dateobj, HashTable *myht)
{
	zval             *z_date;
	zval              tmp_obj;
	timelib_tzinfo   *tzi;

	z_date = zend_hash_str_find(myht, "date", sizeof("date")-1);
	if (z_date && Z_TYPE_P(z_date) == IS_STRING) {
		zval *z_timezone_type = zend_hash_str_find(myht, "timezone_type", sizeof("timezone_type")-1);
		if (z_timezone_type && Z_TYPE_P(z_timezone_type) == IS_LONG) {
			zval *z_timezone = zend_hash_str_find(myht, "timezone", sizeof("timezone")-1);
			if (z_timezone && Z_TYPE_P(z_timezone) == IS_STRING) {
				switch (Z_LVAL_P(z_timezone_type)) {
					case TIMELIB_ZONETYPE_OFFSET:
					case TIMELIB_ZONETYPE_ABBR: {
						char *tmp = emalloc(Z_STRLEN_P(z_date) + Z_STRLEN_P(z_timezone) + 2);
						int ret;
						snprintf(tmp, Z_STRLEN_P(z_date) + Z_STRLEN_P(z_timezone) + 2, "%s %s", Z_STRVAL_P(z_date), Z_STRVAL_P(z_timezone));
						ret = php_date_initialize(*dateobj, tmp, Z_STRLEN_P(z_date) + Z_STRLEN_P(z_timezone) + 1, NULL, NULL, 0);
						efree(tmp);
						return 1 == ret;
					}

					case TIMELIB_ZONETYPE_ID: {
						int ret;
						php_timezone_obj *tzobj;

						tzi = php_date_parse_tzfile(Z_STRVAL_P(z_timezone), DATE_TIMEZONEDB);

						if (tzi == NULL) {
							return 0;
						}

						tzobj = Z_PHPTIMEZONE_P(php_date_instantiate(date_ce_timezone, &tmp_obj));
						tzobj->type = TIMELIB_ZONETYPE_ID;
						tzobj->tzi.tz = tzi;
						tzobj->initialized = 1;

						ret = php_date_initialize(*dateobj, Z_STRVAL_P(z_date), Z_STRLEN_P(z_date), NULL, &tmp_obj, 0);
						zval_ptr_dtor(&tmp_obj);
						return 1 == ret;
					}
				}
			}
		}
	}
	return 0;
} /* }}} */

/* {{{ proto DateTime::__set_state(array array)
*/
PHP_METHOD(DateTime, __set_state)
{
	php_date_obj     *dateobj;
	zval             *array;
	HashTable        *myht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(array)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	myht = Z_ARRVAL_P(array);

	php_date_instantiate(date_ce_date, return_value);
	dateobj = Z_PHPDATE_P(return_value);
	if (!php_date_initialize_from_hash(&dateobj, myht)) {
		zend_throw_error(NULL, "Invalid serialization data for DateTime object");
	}
}
/* }}} */

/* {{{ proto DateTimeImmutable::__set_state(array array)
*/
PHP_METHOD(DateTimeImmutable, __set_state)
{
	php_date_obj     *dateobj;
	zval             *array;
	HashTable        *myht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(array)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	myht = Z_ARRVAL_P(array);

	php_date_instantiate(date_ce_immutable, return_value);
	dateobj = Z_PHPDATE_P(return_value);
	if (!php_date_initialize_from_hash(&dateobj, myht)) {
		zend_throw_error(NULL, "Invalid serialization data for DateTimeImmutable object");
	}
}
/* }}} */

/* {{{ proto DateTime::__wakeup()
*/
PHP_METHOD(DateTime, __wakeup)
{
	zval             *object = ZEND_THIS;
	php_date_obj     *dateobj;
	HashTable        *myht;

	dateobj = Z_PHPDATE_P(object);

	myht = Z_OBJPROP_P(object);

	if (!php_date_initialize_from_hash(&dateobj, myht)) {
		zend_throw_error(NULL, "Invalid serialization data for DateTime object");
	}
}
/* }}} */

/* Helper function used to add an associative array of warnings and errors to a zval */
static void zval_from_error_container(zval *z, timelib_error_container *error) /* {{{ */
{
	int   i;
	zval element;

	add_assoc_long(z, "warning_count", error->warning_count);
	array_init(&element);
	for (i = 0; i < error->warning_count; i++) {
		add_index_string(&element, error->warning_messages[i].position, error->warning_messages[i].message);
	}
	add_assoc_zval(z, "warnings", &element);

	add_assoc_long(z, "error_count", error->error_count);
	array_init(&element);
	for (i = 0; i < error->error_count; i++) {
		add_index_string(&element, error->error_messages[i].position, error->error_messages[i].message);
	}
	add_assoc_zval(z, "errors", &element);
} /* }}} */

/* {{{ proto array date_get_last_errors()
   Returns the warnings and errors found while parsing a date/time string.
*/
PHP_FUNCTION(date_get_last_errors)
{
	if (DATEG(last_errors)) {
		array_init(return_value);
		zval_from_error_container(return_value, DATEG(last_errors));
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

void php_date_do_return_parsed_time(INTERNAL_FUNCTION_PARAMETERS, timelib_time *parsed_time, timelib_error_container *error) /* {{{ */
{
	zval element;

	array_init(return_value);
#define PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(name, elem) \
	if (parsed_time->elem == -99999) {               \
		add_assoc_bool(return_value, #name, 0); \
	} else {                                       \
		add_assoc_long(return_value, #name, parsed_time->elem); \
	}
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(year,      y);
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(month,     m);
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(day,       d);
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(hour,      h);
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(minute,    i);
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(second,    s);

	if (parsed_time->us == -99999) {
		add_assoc_bool(return_value, "fraction", 0);
	} else {
		add_assoc_double(return_value, "fraction", (double)parsed_time->us / 1000000.0);
	}

	zval_from_error_container(return_value, error);

	timelib_error_container_dtor(error);

	add_assoc_bool(return_value, "is_localtime", parsed_time->is_localtime);

	if (parsed_time->is_localtime) {
		PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(zone_type, zone_type);
		switch (parsed_time->zone_type) {
			case TIMELIB_ZONETYPE_OFFSET:
				PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(zone, z);
				add_assoc_bool(return_value, "is_dst", parsed_time->dst);
				break;
			case TIMELIB_ZONETYPE_ID:
				if (parsed_time->tz_abbr) {
					add_assoc_string(return_value, "tz_abbr", parsed_time->tz_abbr);
				}
				if (parsed_time->tz_info) {
					add_assoc_string(return_value, "tz_id", parsed_time->tz_info->name);
				}
				break;
			case TIMELIB_ZONETYPE_ABBR:
				PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(zone, z);
				add_assoc_bool(return_value, "is_dst", parsed_time->dst);
				add_assoc_string(return_value, "tz_abbr", parsed_time->tz_abbr);
				break;
		}
	}
	if (parsed_time->have_relative) {
		array_init(&element);
		add_assoc_long(&element, "year",   parsed_time->relative.y);
		add_assoc_long(&element, "month",  parsed_time->relative.m);
		add_assoc_long(&element, "day",    parsed_time->relative.d);
		add_assoc_long(&element, "hour",   parsed_time->relative.h);
		add_assoc_long(&element, "minute", parsed_time->relative.i);
		add_assoc_long(&element, "second", parsed_time->relative.s);
		if (parsed_time->relative.have_weekday_relative) {
			add_assoc_long(&element, "weekday", parsed_time->relative.weekday);
		}
		if (parsed_time->relative.have_special_relative && (parsed_time->relative.special.type == TIMELIB_SPECIAL_WEEKDAY)) {
			add_assoc_long(&element, "weekdays", parsed_time->relative.special.amount);
		}
		if (parsed_time->relative.first_last_day_of) {
			add_assoc_bool(&element, parsed_time->relative.first_last_day_of == TIMELIB_SPECIAL_FIRST_DAY_OF_MONTH ? "first_day_of_month" : "last_day_of_month", 1);
		}
		add_assoc_zval(return_value, "relative", &element);
	}
	timelib_time_dtor(parsed_time);
} /* }}} */

/* {{{ proto array date_parse(string date)
   Returns associative array with detailed info about given date
*/
PHP_FUNCTION(date_parse)
{
	zend_string                    *date;
	timelib_error_container *error;
	timelib_time                   *parsed_time;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(date)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	parsed_time = timelib_strtotime(ZSTR_VAL(date), ZSTR_LEN(date), &error, DATE_TIMEZONEDB, php_date_parse_tzfile_wrapper);
	php_date_do_return_parsed_time(INTERNAL_FUNCTION_PARAM_PASSTHRU, parsed_time, error);
}
/* }}} */

/* {{{ proto array date_parse_from_format(string format, string date)
   Returns associative array with detailed info about given date
*/
PHP_FUNCTION(date_parse_from_format)
{
	zend_string                    *date, *format;
	timelib_error_container *error;
	timelib_time                   *parsed_time;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(format)
		Z_PARAM_STR(date)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	parsed_time = timelib_parse_from_format(ZSTR_VAL(format), ZSTR_VAL(date), ZSTR_LEN(date), &error, DATE_TIMEZONEDB, php_date_parse_tzfile_wrapper);
	php_date_do_return_parsed_time(INTERNAL_FUNCTION_PARAM_PASSTHRU, parsed_time, error);
}
/* }}} */

/* {{{ proto string date_format(DateTimeInterface object, string format)
   Returns date formatted according to given format
*/
PHP_FUNCTION(date_format)
{
	zval         *object;
	php_date_obj *dateobj;
	char         *format;
	size_t       format_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &object, date_ce_interface, &format, &format_len) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = Z_PHPDATE_P(object);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	RETURN_STR(date_format(format, format_len, dateobj->time, dateobj->time->is_localtime));
}
/* }}} */

static int php_date_modify(zval *object, char *modify, size_t modify_len) /* {{{ */
{
	php_date_obj *dateobj;
	timelib_time *tmp_time;
	timelib_error_container *err = NULL;

	dateobj = Z_PHPDATE_P(object);

	if (!(dateobj->time)) {
		php_error_docref(NULL, E_WARNING, "The DateTime object has not been correctly initialized by its constructor");
		return 0;
	}

	tmp_time = timelib_strtotime(modify, modify_len, &err, DATE_TIMEZONEDB, php_date_parse_tzfile_wrapper);

	/* update last errors and warnings */
	update_errors_warnings(err);
	if (err && err->error_count) {
		/* spit out the first library error message, at least */
		php_error_docref(NULL, E_WARNING, "Failed to parse time string (%s) at position %d (%c): %s", modify,
			err->error_messages[0].position, err->error_messages[0].character, err->error_messages[0].message);
		timelib_time_dtor(tmp_time);
		return 0;
	}

	memcpy(&dateobj->time->relative, &tmp_time->relative, sizeof(timelib_rel_time));
	dateobj->time->have_relative = tmp_time->have_relative;
	dateobj->time->sse_uptodate = 0;

	if (tmp_time->y != -99999) {
		dateobj->time->y = tmp_time->y;
	}
	if (tmp_time->m != -99999) {
		dateobj->time->m = tmp_time->m;
	}
	if (tmp_time->d != -99999) {
		dateobj->time->d = tmp_time->d;
	}

	if (tmp_time->h != -99999) {
		dateobj->time->h = tmp_time->h;
		if (tmp_time->i != -99999) {
			dateobj->time->i = tmp_time->i;
			if (tmp_time->s != -99999) {
				dateobj->time->s = tmp_time->s;
			} else {
				dateobj->time->s = 0;
			}
		} else {
			dateobj->time->i = 0;
			dateobj->time->s = 0;
		}
	}

	if (tmp_time->us != -99999) {
		dateobj->time->us = tmp_time->us;
	}

	timelib_time_dtor(tmp_time);

	timelib_update_ts(dateobj->time, NULL);
	timelib_update_from_sse(dateobj->time);
	dateobj->time->have_relative = 0;
	memset(&dateobj->time->relative, 0, sizeof(dateobj->time->relative));

	return 1;
} /* }}} */

/* {{{ proto DateTime date_modify(DateTime object, string modify)
   Alters the timestamp.
*/
PHP_FUNCTION(date_modify)
{
	zval         *object;
	char         *modify;
	size_t        modify_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &object, date_ce_date, &modify, &modify_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (!php_date_modify(object, modify, modify_len)) {
		RETURN_FALSE;
	}

	Z_ADDREF_P(object);
	ZVAL_OBJ(return_value, Z_OBJ_P(object));
}
/* }}} */

/* {{{ proto DateTimeImmutable::modify()
*/
PHP_METHOD(DateTimeImmutable, modify)
{
	zval *object, new_object;
	char *modify;
	size_t   modify_len;

	object = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &modify, &modify_len) == FAILURE) {
		RETURN_FALSE;
	}

	date_clone_immutable(object, &new_object);
	if (!php_date_modify(&new_object, modify, modify_len)) {
		zval_ptr_dtor(&new_object);
		RETURN_FALSE;
	}

	ZVAL_OBJ(return_value, Z_OBJ(new_object));
}
/* }}} */

static void php_date_add(zval *object, zval *interval, zval *return_value) /* {{{ */
{
	php_date_obj     *dateobj;
	php_interval_obj *intobj;
	timelib_time     *new_time;

	dateobj = Z_PHPDATE_P(object);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	intobj = Z_PHPINTERVAL_P(interval);
	DATE_CHECK_INITIALIZED(intobj->initialized, DateInterval);

	new_time = timelib_add(dateobj->time, intobj->diff);
	timelib_time_dtor(dateobj->time);
	dateobj->time = new_time;
} /* }}} */

/* {{{ proto DateTime date_add(DateTime object, DateInterval interval)
   Adds an interval to the current date in object.
*/
PHP_FUNCTION(date_add)
{
	zval *object, *interval;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "OO", &object, date_ce_date, &interval, date_ce_interval) == FAILURE) {
		RETURN_FALSE;
	}

	php_date_add(object, interval, return_value);

	Z_ADDREF_P(object);
	ZVAL_OBJ(return_value, Z_OBJ_P(object));
}
/* }}} */

/* {{{ proto DateTimeImmutable::add()
*/
PHP_METHOD(DateTimeImmutable, add)
{
	zval *object, *interval, new_object;

	object = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &interval, date_ce_interval) == FAILURE) {
		RETURN_FALSE;
	}

	date_clone_immutable(object, &new_object);
	php_date_add(&new_object, interval, return_value);

	ZVAL_OBJ(return_value, Z_OBJ(new_object));
}
/* }}} */

static void php_date_sub(zval *object, zval *interval, zval *return_value) /* {{{ */
{
	php_date_obj     *dateobj;
	php_interval_obj *intobj;
	timelib_time     *new_time;

	dateobj = Z_PHPDATE_P(object);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	intobj = Z_PHPINTERVAL_P(interval);
	DATE_CHECK_INITIALIZED(intobj->initialized, DateInterval);

	if (intobj->diff->have_special_relative) {
		php_error_docref(NULL, E_WARNING, "Only non-special relative time specifications are supported for subtraction");
		return;
	}

	new_time = timelib_sub(dateobj->time, intobj->diff);
	timelib_time_dtor(dateobj->time);
	dateobj->time = new_time;
} /* }}} */

/* {{{ proto DateTime date_sub(DateTime object, DateInterval interval)
   Subtracts an interval to the current date in object.
*/
PHP_FUNCTION(date_sub)
{
	zval *object, *interval;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "OO", &object, date_ce_date, &interval, date_ce_interval) == FAILURE) {
		RETURN_FALSE;
	}

	php_date_sub(object, interval, return_value);

	Z_ADDREF_P(object);
	ZVAL_OBJ(return_value, Z_OBJ_P(object));
}
/* }}} */

/* {{{ proto DateTimeImmutable::sub()
*/
PHP_METHOD(DateTimeImmutable, sub)
{
	zval *object, *interval, new_object;

	object = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &interval, date_ce_interval) == FAILURE) {
		RETURN_FALSE;
	}

	date_clone_immutable(object, &new_object);
	php_date_sub(&new_object, interval, return_value);

	ZVAL_OBJ(return_value, Z_OBJ(new_object));
}
/* }}} */

static void set_timezone_from_timelib_time(php_timezone_obj *tzobj, timelib_time *t)
{
       tzobj->initialized = 1;
       tzobj->type = t->zone_type;
       switch (t->zone_type) {
               case TIMELIB_ZONETYPE_ID:
                       tzobj->tzi.tz = t->tz_info;
                       break;
               case TIMELIB_ZONETYPE_OFFSET:
                       tzobj->tzi.utc_offset = t->z;
                       break;
               case TIMELIB_ZONETYPE_ABBR:
                       tzobj->tzi.z.utc_offset = t->z;
                       tzobj->tzi.z.dst = t->dst;
                       tzobj->tzi.z.abbr = timelib_strdup(t->tz_abbr);
                       break;
       }
}


/* {{{ proto DateTimeZone date_timezone_get(DateTimeInterface object)
   Return new DateTimeZone object relative to give DateTime
*/
PHP_FUNCTION(date_timezone_get)
{
	zval             *object;
	php_date_obj     *dateobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &object, date_ce_interface) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = Z_PHPDATE_P(object);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	if (dateobj->time->is_localtime) {
		php_timezone_obj *tzobj;
		php_date_instantiate(date_ce_timezone, return_value);
		tzobj = Z_PHPTIMEZONE_P(return_value);
		set_timezone_from_timelib_time(tzobj, dateobj->time);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

static void php_date_timezone_set(zval *object, zval *timezone_object, zval *return_value) /* {{{ */
{
	php_date_obj     *dateobj;
	php_timezone_obj *tzobj;

	dateobj = Z_PHPDATE_P(object);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	tzobj = Z_PHPTIMEZONE_P(timezone_object);

	switch (tzobj->type) {
		case TIMELIB_ZONETYPE_OFFSET:
			timelib_set_timezone_from_offset(dateobj->time, tzobj->tzi.utc_offset);
			break;
		case TIMELIB_ZONETYPE_ABBR:
			timelib_set_timezone_from_abbr(dateobj->time, tzobj->tzi.z);
			break;
		case TIMELIB_ZONETYPE_ID:
			timelib_set_timezone(dateobj->time, tzobj->tzi.tz);
			break;
	}
	timelib_unixtime2local(dateobj->time, dateobj->time->sse);
} /* }}} */

/* {{{ proto DateTime date_timezone_set(DateTime object, DateTimeZone object)
   Sets the timezone for the DateTime object.
*/
PHP_FUNCTION(date_timezone_set)
{
	zval *object;
	zval *timezone_object;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "OO", &object, date_ce_date, &timezone_object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}

	php_date_timezone_set(object, timezone_object, return_value);

	Z_ADDREF_P(object);
	ZVAL_OBJ(return_value, Z_OBJ_P(object));
}
/* }}} */

/* {{{ proto DateTimeImmutable::setTimezone()
*/
PHP_METHOD(DateTimeImmutable, setTimezone)
{
	zval *object, new_object;
	zval *timezone_object;

	object = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &timezone_object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}

	date_clone_immutable(object, &new_object);
	php_date_timezone_set(&new_object, timezone_object, return_value);

	ZVAL_OBJ(return_value, Z_OBJ(new_object));
}
/* }}} */

/* {{{ proto int date_offset_get(DateTimeInterface object)
   Returns the DST offset.
*/
PHP_FUNCTION(date_offset_get)
{
	zval                *object;
	php_date_obj        *dateobj;
	timelib_time_offset *offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &object, date_ce_interface) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = Z_PHPDATE_P(object);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	if (dateobj->time->is_localtime) {
		switch (dateobj->time->zone_type) {
			case TIMELIB_ZONETYPE_ID:
				offset = timelib_get_time_zone_info(dateobj->time->sse, dateobj->time->tz_info);
				RETVAL_LONG(offset->offset);
				timelib_time_offset_dtor(offset);
				break;
			case TIMELIB_ZONETYPE_OFFSET:
				RETVAL_LONG(dateobj->time->z);
				break;
			case TIMELIB_ZONETYPE_ABBR:
				RETVAL_LONG((dateobj->time->z + (3600 * dateobj->time->dst)));
				break;
		}
		return;
	} else {
		RETURN_LONG(0);
	}
}
/* }}} */

static void php_date_time_set(zval *object, zend_long h, zend_long i, zend_long s, zend_long ms, zval *return_value) /* {{{ */
{
	php_date_obj *dateobj;

	dateobj = Z_PHPDATE_P(object);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	dateobj->time->h = h;
	dateobj->time->i = i;
	dateobj->time->s = s;
	dateobj->time->us = ms;
	timelib_update_ts(dateobj->time, NULL);
	timelib_update_from_sse(dateobj->time);
} /* }}} */

/* {{{ proto DateTime date_time_set(DateTime object, int hour, int minute[, int second[, int microseconds]])
   Sets the time.
*/
PHP_FUNCTION(date_time_set)
{
	zval *object;
	zend_long  h, i, s = 0, ms = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oll|ll", &object, date_ce_date, &h, &i, &s, &ms) == FAILURE) {
		RETURN_FALSE;
	}

	php_date_time_set(object, h, i, s, ms, return_value);

	Z_ADDREF_P(object);
	ZVAL_OBJ(return_value, Z_OBJ_P(object));
}
/* }}} */

/* {{{ proto DateTimeImmutable::setTime()
*/
PHP_METHOD(DateTimeImmutable, setTime)
{
	zval *object, new_object;
	zend_long  h, i, s = 0, ms = 0;

	object = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll|ll", &h, &i, &s, &ms) == FAILURE) {
		RETURN_FALSE;
	}

	date_clone_immutable(object, &new_object);
	php_date_time_set(&new_object, h, i, s, ms, return_value);

	ZVAL_OBJ(return_value, Z_OBJ(new_object));
}
/* }}} */

static void php_date_date_set(zval *object, zend_long y, zend_long m, zend_long d, zval *return_value) /* {{{ */
{
	php_date_obj *dateobj;

	dateobj = Z_PHPDATE_P(object);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	dateobj->time->y = y;
	dateobj->time->m = m;
	dateobj->time->d = d;
	timelib_update_ts(dateobj->time, NULL);
} /* }}} */

/* {{{ proto DateTime date_date_set(DateTime object, int year, int month, int day)
   Sets the date.
*/
PHP_FUNCTION(date_date_set)
{
	zval *object;
	zend_long  y, m, d;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Olll", &object, date_ce_date, &y, &m, &d) == FAILURE) {
		RETURN_FALSE;
	}

	php_date_date_set(object, y, m, d, return_value);

	Z_ADDREF_P(object);
	ZVAL_OBJ(return_value, Z_OBJ_P(object));
}
/* }}} */

/* {{{ proto DateTimeImmutable::setDate()
*/
PHP_METHOD(DateTimeImmutable, setDate)
{
	zval *object, new_object;
	zend_long  y, m, d;

	object = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lll", &y, &m, &d) == FAILURE) {
		RETURN_FALSE;
	}

	date_clone_immutable(object, &new_object);
	php_date_date_set(&new_object, y, m, d, return_value);

	ZVAL_OBJ(return_value, Z_OBJ(new_object));
}
/* }}} */

static void php_date_isodate_set(zval *object, zend_long y, zend_long w, zend_long d, zval *return_value) /* {{{ */
{
	php_date_obj *dateobj;

	dateobj = Z_PHPDATE_P(object);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	dateobj->time->y = y;
	dateobj->time->m = 1;
	dateobj->time->d = 1;
	memset(&dateobj->time->relative, 0, sizeof(dateobj->time->relative));
	dateobj->time->relative.d = timelib_daynr_from_weeknr(y, w, d);
	dateobj->time->have_relative = 1;

	timelib_update_ts(dateobj->time, NULL);
} /* }}} */

/* {{{ proto DateTime date_isodate_set(DateTime object, int year, int week[, int day])
   Sets the ISO date.
*/
PHP_FUNCTION(date_isodate_set)
{
	zval *object;
	zend_long  y, w, d = 1;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oll|l", &object, date_ce_date, &y, &w, &d) == FAILURE) {
		RETURN_FALSE;
	}

	php_date_isodate_set(object, y, w, d, return_value);

	Z_ADDREF_P(object);
	ZVAL_OBJ(return_value, Z_OBJ_P(object));
}
/* }}} */

/* {{{ proto DateTimeImmutable::setISODate()
*/
PHP_METHOD(DateTimeImmutable, setISODate)
{
	zval *object, new_object;
	zend_long  y, w, d = 1;

	object = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll|l", &y, &w, &d) == FAILURE) {
		RETURN_FALSE;
	}

	date_clone_immutable(object, &new_object);
	php_date_isodate_set(&new_object, y, w, d, return_value);

	ZVAL_OBJ(return_value, Z_OBJ(new_object));
}
/* }}} */

static void php_date_timestamp_set(zval *object, zend_long timestamp, zval *return_value) /* {{{ */
{
	php_date_obj *dateobj;

	dateobj = Z_PHPDATE_P(object);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	timelib_unixtime2local(dateobj->time, (timelib_sll)timestamp);
	timelib_update_ts(dateobj->time, NULL);
	php_date_set_time_fraction(dateobj->time, 0);
} /* }}} */

/* {{{ proto DateTime date_timestamp_set(DateTime object, int unixTimestamp)
   Sets the date and time based on an Unix timestamp.
*/
PHP_FUNCTION(date_timestamp_set)
{
	zval *object;
	zend_long  timestamp;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &object, date_ce_date, &timestamp) == FAILURE) {
		RETURN_FALSE;
	}

	php_date_timestamp_set(object, timestamp, return_value);

	Z_ADDREF_P(object);
	ZVAL_OBJ(return_value, Z_OBJ_P(object));
}
/* }}} */

/* {{{ proto DateTimeImmutable::setTimestamp()
*/
PHP_METHOD(DateTimeImmutable, setTimestamp)
{
	zval *object, new_object;
	zend_long  timestamp;

	object = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &timestamp) == FAILURE) {
		RETURN_FALSE;
	}

	date_clone_immutable(object, &new_object);
	php_date_timestamp_set(&new_object, timestamp, return_value);

	ZVAL_OBJ(return_value, Z_OBJ(new_object));
}
/* }}} */

/* {{{ proto int date_timestamp_get(DateTimeInterface object)
   Gets the Unix timestamp.
*/
PHP_FUNCTION(date_timestamp_get)
{
	zval         *object;
	php_date_obj *dateobj;
	zend_long          timestamp;
	int           error;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &object, date_ce_interface) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = Z_PHPDATE_P(object);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	timelib_update_ts(dateobj->time, NULL);

	timestamp = timelib_date_to_int(dateobj->time, &error);
	if (error) {
		RETURN_FALSE;
	} else {
		RETVAL_LONG(timestamp);
	}
}
/* }}} */

/* {{{ proto DateInterval date_diff(DateTime object [, bool absolute])
   Returns the difference between two DateTime objects.
*/
PHP_FUNCTION(date_diff)
{
	zval         *object1, *object2;
	php_date_obj *dateobj1, *dateobj2;
	php_interval_obj *interval;
	zend_bool      absolute = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "OO|b", &object1, date_ce_interface, &object2, date_ce_interface, &absolute) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj1 = Z_PHPDATE_P(object1);
	dateobj2 = Z_PHPDATE_P(object2);
	DATE_CHECK_INITIALIZED(dateobj1->time, DateTimeInterface);
	DATE_CHECK_INITIALIZED(dateobj2->time, DateTimeInterface);
	timelib_update_ts(dateobj1->time, NULL);
	timelib_update_ts(dateobj2->time, NULL);

	php_date_instantiate(date_ce_interval, return_value);
	interval = Z_PHPINTERVAL_P(return_value);
	interval->diff = timelib_diff(dateobj1->time, dateobj2->time);
	if (absolute) {
		interval->diff->invert = 0;
	}
	interval->initialized = 1;
}
/* }}} */

static int timezone_initialize(php_timezone_obj *tzobj, /*const*/ char *tz, size_t tz_len) /* {{{ */
{
	timelib_time *dummy_t = ecalloc(1, sizeof(timelib_time));
	int           dst, not_found;
	char         *orig_tz = tz;

	if (strlen(tz) != tz_len) {
		php_error_docref(NULL, E_WARNING, "Timezone must not contain null bytes");
		efree(dummy_t);
		return FAILURE;
	}

	dummy_t->z = timelib_parse_zone(&tz, &dst, dummy_t, &not_found, DATE_TIMEZONEDB, php_date_parse_tzfile_wrapper);
	if (not_found) {
		php_error_docref(NULL, E_WARNING, "Unknown or bad timezone (%s)", orig_tz);
		efree(dummy_t);
		return FAILURE;
	} else {
		set_timezone_from_timelib_time(tzobj, dummy_t);
		timelib_free(dummy_t->tz_abbr);
		efree(dummy_t);
		return SUCCESS;
	}
} /* }}} */

/* {{{ proto DateTimeZone timezone_open(string timezone)
   Returns new DateTimeZone object
*/
PHP_FUNCTION(timezone_open)
{
	zend_string *tz;
	php_timezone_obj *tzobj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(tz)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	tzobj = Z_PHPTIMEZONE_P(php_date_instantiate(date_ce_timezone, return_value));
	if (SUCCESS != timezone_initialize(tzobj, ZSTR_VAL(tz), ZSTR_LEN(tz))) {
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto DateTimeZone::__construct(string timezone)
   Creates new DateTimeZone object.
*/
PHP_METHOD(DateTimeZone, __construct)
{
	zend_string *tz;
	php_timezone_obj *tzobj;
	zend_error_handling error_handling;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_STR(tz)
	ZEND_PARSE_PARAMETERS_END();

	zend_replace_error_handling(EH_THROW, NULL, &error_handling);
	tzobj = Z_PHPTIMEZONE_P(ZEND_THIS);
	timezone_initialize(tzobj, ZSTR_VAL(tz), ZSTR_LEN(tz));
	zend_restore_error_handling(&error_handling);
}
/* }}} */

static int php_date_timezone_initialize_from_hash(zval **return_value, php_timezone_obj **tzobj, HashTable *myht) /* {{{ */
{
	zval            *z_timezone_type;

	if ((z_timezone_type = zend_hash_str_find(myht, "timezone_type", sizeof("timezone_type") - 1)) != NULL) {
		zval *z_timezone;

		if ((z_timezone = zend_hash_str_find(myht, "timezone", sizeof("timezone") - 1)) != NULL) {
			if (Z_TYPE_P(z_timezone_type) != IS_LONG) {
				return FAILURE;
			}
			if (Z_TYPE_P(z_timezone) != IS_STRING) {
				return FAILURE;
			}
			if (SUCCESS == timezone_initialize(*tzobj, Z_STRVAL_P(z_timezone), Z_STRLEN_P(z_timezone))) {
				return SUCCESS;
			}
		}
	}
	return FAILURE;
} /* }}} */

/* {{{ proto DateTimeZone::__set_state(array array)
 *  */
PHP_METHOD(DateTimeZone, __set_state)
{
	php_timezone_obj *tzobj;
	zval             *array;
	HashTable        *myht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(array)
	ZEND_PARSE_PARAMETERS_END();

	myht = Z_ARRVAL_P(array);

	php_date_instantiate(date_ce_timezone, return_value);
	tzobj = Z_PHPTIMEZONE_P(return_value);
	if(php_date_timezone_initialize_from_hash(&return_value, &tzobj, myht) != SUCCESS) {
		zend_throw_error(NULL, "Timezone initialization failed");
		zval_ptr_dtor(return_value);
	}
}
/* }}} */

/* {{{ proto DateTimeZone::__wakeup()
 *  */
PHP_METHOD(DateTimeZone, __wakeup)
{
	zval             *object = ZEND_THIS;
	php_timezone_obj *tzobj;
	HashTable        *myht;

	tzobj = Z_PHPTIMEZONE_P(object);

	myht = Z_OBJPROP_P(object);

	if(php_date_timezone_initialize_from_hash(&return_value, &tzobj, myht) != SUCCESS) {
		zend_throw_error(NULL, "Timezone initialization failed");
	}
}
/* }}} */

/* {{{ proto string timezone_name_get(DateTimeZone object)
   Returns the name of the timezone.
*/
PHP_FUNCTION(timezone_name_get)
{
	zval             *object;
	php_timezone_obj *tzobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}
	tzobj = Z_PHPTIMEZONE_P(object);
	DATE_CHECK_INITIALIZED(tzobj->initialized, DateTimeZone);
	php_timezone_to_string(tzobj, return_value);
}
/* }}} */

/* {{{ proto string timezone_name_from_abbr(string abbr[, int gmtOffset[, int isdst]])
   Returns the timezone name from abbreviation
*/
PHP_FUNCTION(timezone_name_from_abbr)
{
	zend_string  *abbr;
	char         *tzid;
	zend_long     gmtoffset = -1;
	zend_long     isdst = -1;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STR(abbr)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(gmtoffset)
		Z_PARAM_LONG(isdst)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	tzid = timelib_timezone_id_from_abbr(ZSTR_VAL(abbr), gmtoffset, isdst);

	if (tzid) {
		RETURN_STRING(tzid);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int timezone_offset_get(DateTimeZone object, DateTimeInterface datetime)
   Returns the timezone offset.
*/
PHP_FUNCTION(timezone_offset_get)
{
	zval                *object, *dateobject;
	php_timezone_obj    *tzobj;
	php_date_obj        *dateobj;
	timelib_time_offset *offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "OO", &object, date_ce_timezone, &dateobject, date_ce_interface) == FAILURE) {
		RETURN_FALSE;
	}
	tzobj = Z_PHPTIMEZONE_P(object);
	DATE_CHECK_INITIALIZED(tzobj->initialized, DateTimeZone);
	dateobj = Z_PHPDATE_P(dateobject);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTimeInterface);

	switch (tzobj->type) {
		case TIMELIB_ZONETYPE_ID:
			offset = timelib_get_time_zone_info(dateobj->time->sse, tzobj->tzi.tz);
			RETVAL_LONG(offset->offset);
			timelib_time_offset_dtor(offset);
			break;
		case TIMELIB_ZONETYPE_OFFSET:
			RETURN_LONG(tzobj->tzi.utc_offset);
			break;
		case TIMELIB_ZONETYPE_ABBR:
			RETURN_LONG(tzobj->tzi.z.utc_offset + (tzobj->tzi.z.dst * 3600));
			break;
	}
}
/* }}} */

/* {{{ proto array timezone_transitions_get(DateTimeZone object [, int timestamp_begin [, int timestamp_end ]])
   Returns numerically indexed array containing associative array for all transitions in the specified range for the timezone.
*/
PHP_FUNCTION(timezone_transitions_get)
{
	zval                *object, element;
	php_timezone_obj    *tzobj;
	unsigned int         begin = 0, found;
	zend_long            timestamp_begin = ZEND_LONG_MIN, timestamp_end = ZEND_LONG_MAX;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|ll", &object, date_ce_timezone, &timestamp_begin, &timestamp_end) == FAILURE) {
		RETURN_FALSE;
	}
	tzobj = Z_PHPTIMEZONE_P(object);
	DATE_CHECK_INITIALIZED(tzobj->initialized, DateTimeZone);
	if (tzobj->type != TIMELIB_ZONETYPE_ID) {
		RETURN_FALSE;
	}

#define add_nominal() \
		array_init(&element); \
		add_assoc_long(&element, "ts",     timestamp_begin); \
		add_assoc_str(&element, "time", php_format_date(DATE_FORMAT_ISO8601, 13, timestamp_begin, 0)); \
		add_assoc_long(&element, "offset", tzobj->tzi.tz->type[0].offset); \
		add_assoc_bool(&element, "isdst",  tzobj->tzi.tz->type[0].isdst); \
		add_assoc_string(&element, "abbr", &tzobj->tzi.tz->timezone_abbr[tzobj->tzi.tz->type[0].abbr_idx]); \
		add_next_index_zval(return_value, &element);

#define add(i,ts) \
		array_init(&element); \
		add_assoc_long(&element, "ts",     ts); \
		add_assoc_str(&element, "time", php_format_date(DATE_FORMAT_ISO8601, 13, ts, 0)); \
		add_assoc_long(&element, "offset", tzobj->tzi.tz->type[tzobj->tzi.tz->trans_idx[i]].offset); \
		add_assoc_bool(&element, "isdst",  tzobj->tzi.tz->type[tzobj->tzi.tz->trans_idx[i]].isdst); \
		add_assoc_string(&element, "abbr", &tzobj->tzi.tz->timezone_abbr[tzobj->tzi.tz->type[tzobj->tzi.tz->trans_idx[i]].abbr_idx]); \
		add_next_index_zval(return_value, &element);

#define add_last() add(tzobj->tzi.tz->bit64.timecnt - 1, timestamp_begin)

	array_init(return_value);

	if (timestamp_begin == ZEND_LONG_MIN) {
		add_nominal();
		begin = 0;
		found = 1;
	} else {
		begin = 0;
		found = 0;
		if (tzobj->tzi.tz->bit64.timecnt > 0) {
			do {
				if (tzobj->tzi.tz->trans[begin] > timestamp_begin) {
					if (begin > 0) {
						add(begin - 1, timestamp_begin);
					} else {
						add_nominal();
					}
					found = 1;
					break;
				}
				begin++;
			} while (begin < tzobj->tzi.tz->bit64.timecnt);
		}
	}

	if (!found) {
		if (tzobj->tzi.tz->bit64.timecnt > 0) {
			add_last();
		} else {
			add_nominal();
		}
	} else {
		unsigned int i;
		for (i = begin; i < tzobj->tzi.tz->bit64.timecnt; ++i) {
			if (tzobj->tzi.tz->trans[i] < timestamp_end) {
				add(i, tzobj->tzi.tz->trans[i]);
			}
		}
	}
}
/* }}} */

/* {{{ proto array timezone_location_get()
   Returns location information for a timezone, including country code, latitude/longitude and comments
*/
PHP_FUNCTION(timezone_location_get)
{
	zval                *object;
	php_timezone_obj    *tzobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}
	tzobj = Z_PHPTIMEZONE_P(object);
	DATE_CHECK_INITIALIZED(tzobj->initialized, DateTimeZone);
	if (tzobj->type != TIMELIB_ZONETYPE_ID) {
		RETURN_FALSE;
	}

	array_init(return_value);
	add_assoc_string(return_value, "country_code", tzobj->tzi.tz->location.country_code);
	add_assoc_double(return_value, "latitude", tzobj->tzi.tz->location.latitude);
	add_assoc_double(return_value, "longitude", tzobj->tzi.tz->location.longitude);
	add_assoc_string(return_value, "comments", tzobj->tzi.tz->location.comments);
}
/* }}} */

static int date_interval_initialize(timelib_rel_time **rt, /*const*/ char *format, size_t format_length) /* {{{ */
{
	timelib_time     *b = NULL, *e = NULL;
	timelib_rel_time *p = NULL;
	int               r = 0;
	int               retval = 0;
	timelib_error_container *errors;

	timelib_strtointerval(format, format_length, &b, &e, &p, &r, &errors);

	if (errors->error_count > 0) {
		php_error_docref(NULL, E_WARNING, "Unknown or bad format (%s)", format);
		retval = FAILURE;
	} else {
		if(p) {
			*rt = p;
			retval = SUCCESS;
		} else {
			if(b && e) {
				timelib_update_ts(b, NULL);
				timelib_update_ts(e, NULL);
				*rt = timelib_diff(b, e);
				retval = SUCCESS;
			} else {
				php_error_docref(NULL, E_WARNING, "Failed to parse interval (%s)", format);
				retval = FAILURE;
			}
		}
	}
	timelib_error_container_dtor(errors);
	timelib_free(b);
	timelib_free(e);
	return retval;
} /* }}} */

static int date_interval_compare_objects(zval *o1, zval *o2) {
	/* There is no well defined way to compare intervals like P1M and P30D, which may compare
	 * smaller, equal or greater depending on the point in time at which the interval starts. As
	 * such, we treat DateInterval objects are non-comparable and emit a warning. */
	zend_error(E_WARNING, "Cannot compare DateInterval objects");
	return 1;
}

/* {{{ date_interval_read_property */
static zval *date_interval_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv)
{
	php_interval_obj *obj;
	zval *retval;
	zval tmp_member;
	timelib_sll value = -1;
	double      fvalue = -1;

 	if (Z_TYPE_P(member) != IS_STRING) {
		zend_string *str = zval_try_get_string_func(member);
		if (UNEXPECTED(!str)) {
			return &EG(uninitialized_zval);
		}
		ZVAL_STR(&tmp_member, str);
		member = &tmp_member;
		cache_slot = NULL;
	}

	obj = Z_PHPINTERVAL_P(object);

	if (!obj->initialized) {
		retval = zend_std_read_property(object, member, type, cache_slot, rv);
		if (member == &tmp_member) {
			zval_ptr_dtor_str(&tmp_member);
		}
		return retval;
	}

#define GET_VALUE_FROM_STRUCT(n,m)            \
	if (strcmp(Z_STRVAL_P(member), m) == 0) { \
		value = obj->diff->n;                 \
		break;								  \
	}
	do {
		GET_VALUE_FROM_STRUCT(y, "y");
		GET_VALUE_FROM_STRUCT(m, "m");
		GET_VALUE_FROM_STRUCT(d, "d");
		GET_VALUE_FROM_STRUCT(h, "h");
		GET_VALUE_FROM_STRUCT(i, "i");
		GET_VALUE_FROM_STRUCT(s, "s");
		if (strcmp(Z_STRVAL_P(member), "f") == 0) {
			fvalue = obj->diff->us / 1000000.0;
			break;
		}
		GET_VALUE_FROM_STRUCT(invert, "invert");
		GET_VALUE_FROM_STRUCT(days, "days");
		/* didn't find any */
		retval = zend_std_read_property(object, member, type, cache_slot, rv);

		if (member == &tmp_member) {
			zval_ptr_dtor_str(&tmp_member);
		}

		return retval;
	} while(0);

	retval = rv;

	if (fvalue != -1) {
		ZVAL_DOUBLE(retval, fvalue);
	} else if (value != -99999) {
		ZVAL_LONG(retval, value);
	} else {
		ZVAL_FALSE(retval);
	}

	if (member == &tmp_member) {
		zval_ptr_dtor_str(&tmp_member);
	}

	return retval;
}
/* }}} */

/* {{{ date_interval_write_property */
static zval *date_interval_write_property(zval *object, zval *member, zval *value, void **cache_slot)
{
	php_interval_obj *obj;
	zval tmp_member;

 	if (Z_TYPE_P(member) != IS_STRING) {
		zend_string *str = zval_try_get_string_func(member);
		if (UNEXPECTED(!str)) {
			return value;
		}
		ZVAL_STR(&tmp_member, str);
		member = &tmp_member;
		cache_slot = NULL;
	}

	obj = Z_PHPINTERVAL_P(object);

	if (!obj->initialized) {
		value = zend_std_write_property(object, member, value, cache_slot);
		if (member == &tmp_member) {
			zval_ptr_dtor_str(&tmp_member);
		}
		return value;
	}

#define SET_VALUE_FROM_STRUCT(n,m)            \
	if (strcmp(Z_STRVAL_P(member), m) == 0) { \
		obj->diff->n = zval_get_long(value); \
		break; \
	}

	do {
		SET_VALUE_FROM_STRUCT(y, "y");
		SET_VALUE_FROM_STRUCT(m, "m");
		SET_VALUE_FROM_STRUCT(d, "d");
		SET_VALUE_FROM_STRUCT(h, "h");
		SET_VALUE_FROM_STRUCT(i, "i");
		SET_VALUE_FROM_STRUCT(s, "s");
		if (strcmp(Z_STRVAL_P(member), "f") == 0) {
			obj->diff->us = zval_get_double(value) * 1000000;
			break;
		}
		SET_VALUE_FROM_STRUCT(invert, "invert");
		/* didn't find any */
		value = zend_std_write_property(object, member, value, cache_slot);
	} while(0);

	if (member == &tmp_member) {
		zval_ptr_dtor_str(&tmp_member);
	}

	return value;
}
/* }}} */

/* {{{ date_interval_get_property_ptr_ptr */
static zval *date_interval_get_property_ptr_ptr(zval *object, zval *member, int type, void **cache_slot)
{
	zval tmp_member, *ret;

	if (Z_TYPE_P(member) != IS_STRING) {
		zend_string *str = zval_try_get_string_func(member);
		if (UNEXPECTED(!str)) {
			return NULL;
		}
		ZVAL_STR(&tmp_member, str);
		member = &tmp_member;
		cache_slot = NULL;
	}

	if(zend_binary_strcmp("y", sizeof("y") - 1, Z_STRVAL_P(member), Z_STRLEN_P(member)) == 0 ||
		zend_binary_strcmp("m", sizeof("m") - 1, Z_STRVAL_P(member), Z_STRLEN_P(member)) == 0 ||
		zend_binary_strcmp("d", sizeof("d") - 1, Z_STRVAL_P(member), Z_STRLEN_P(member)) == 0 ||
		zend_binary_strcmp("h", sizeof("h") - 1, Z_STRVAL_P(member), Z_STRLEN_P(member)) == 0 ||
		zend_binary_strcmp("i", sizeof("i") - 1, Z_STRVAL_P(member), Z_STRLEN_P(member)) == 0 ||
		zend_binary_strcmp("s", sizeof("s") - 1, Z_STRVAL_P(member), Z_STRLEN_P(member)) == 0 ||
		zend_binary_strcmp("f", sizeof("f") - 1, Z_STRVAL_P(member), Z_STRLEN_P(member)) == 0 ||
		zend_binary_strcmp("days", sizeof("days") - 1, Z_STRVAL_P(member), Z_STRLEN_P(member)) == 0 ||
		zend_binary_strcmp("invert", sizeof("invert") - 1, Z_STRVAL_P(member), Z_STRLEN_P(member)) == 0) {
		/* Fallback to read_property. */
		ret = NULL;
	} else {
		ret = zend_std_get_property_ptr_ptr(object, member, type, cache_slot);
	}

	if (member == &tmp_member) {
		zval_ptr_dtor_str(&tmp_member);
	}

	return ret;
}
/* }}} */

/* {{{ proto DateInterval::__construct([string interval_spec])
   Creates new DateInterval object.
*/
PHP_METHOD(DateInterval, __construct)
{
	zend_string *interval_string = NULL;
	timelib_rel_time *reltime;
	zend_error_handling error_handling;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_STR(interval_string)
	ZEND_PARSE_PARAMETERS_END();

	zend_replace_error_handling(EH_THROW, NULL, &error_handling);
	if (date_interval_initialize(&reltime, ZSTR_VAL(interval_string), ZSTR_LEN(interval_string)) == SUCCESS) {
		php_interval_obj *diobj = Z_PHPINTERVAL_P(ZEND_THIS);
		diobj->diff = reltime;
		diobj->initialized = 1;
	}
	zend_restore_error_handling(&error_handling);
}
/* }}} */


static int php_date_interval_initialize_from_hash(zval **return_value, php_interval_obj **intobj, HashTable *myht) /* {{{ */
{
	(*intobj)->diff = timelib_rel_time_ctor();

#define PHP_DATE_INTERVAL_READ_PROPERTY(element, member, itype, def) \
	do { \
		zval *z_arg = zend_hash_str_find(myht, element, sizeof(element) - 1); \
		if (z_arg && Z_TYPE_P(z_arg) <= IS_STRING) { \
			(*intobj)->diff->member = (itype)zval_get_long(z_arg); \
		} else { \
			(*intobj)->diff->member = (itype)def; \
		} \
	} while (0);

#define PHP_DATE_INTERVAL_READ_PROPERTY_I64(element, member) \
	do { \
		zval *z_arg = zend_hash_str_find(myht, element, sizeof(element) - 1); \
		if (z_arg && Z_TYPE_P(z_arg) <= IS_STRING) { \
			zend_string *tmp_str; \
			zend_string *str = zval_get_tmp_string(z_arg, &tmp_str); \
			DATE_A64I((*intobj)->diff->member, ZSTR_VAL(str)); \
			zend_tmp_string_release(tmp_str); \
		} else { \
			(*intobj)->diff->member = -1LL; \
		} \
	} while (0);

#define PHP_DATE_INTERVAL_READ_PROPERTY_DAYS(member) \
	do { \
		zval *z_arg = zend_hash_str_find(myht, "days", sizeof("days") - 1); \
		if (z_arg && Z_TYPE_P(z_arg) == IS_FALSE) { \
			(*intobj)->diff->member = -99999; \
		} else if (z_arg && Z_TYPE_P(z_arg) <= IS_STRING) { \
			zend_string *str = zval_get_string(z_arg); \
			DATE_A64I((*intobj)->diff->member, ZSTR_VAL(str)); \
			zend_string_release(str); \
		} else { \
			(*intobj)->diff->member = -1LL; \
		} \
	} while (0);

#define PHP_DATE_INTERVAL_READ_PROPERTY_DOUBLE(element, member, def) \
	do { \
		zval *z_arg = zend_hash_str_find(myht, element, sizeof(element) - 1); \
		if (z_arg) { \
			(*intobj)->diff->member = (double)zval_get_double(z_arg); \
		} else { \
			(*intobj)->diff->member = (double)def; \
		} \
	} while (0);

	PHP_DATE_INTERVAL_READ_PROPERTY("y", y, timelib_sll, -1)
	PHP_DATE_INTERVAL_READ_PROPERTY("m", m, timelib_sll, -1)
	PHP_DATE_INTERVAL_READ_PROPERTY("d", d, timelib_sll, -1)
	PHP_DATE_INTERVAL_READ_PROPERTY("h", h, timelib_sll, -1)
	PHP_DATE_INTERVAL_READ_PROPERTY("i", i, timelib_sll, -1)
	PHP_DATE_INTERVAL_READ_PROPERTY("s", s, timelib_sll, -1)
	{
		zval *z_arg = zend_hash_str_find(myht, "f", sizeof("f") - 1);
		(*intobj)->diff->us = -1000000;
		if (z_arg) {
			double val = zval_get_double(z_arg) * 1000000;
			if (val >= 0 && val < 1000000) {
				(*intobj)->diff->us = val;
			}
		}
	}
	PHP_DATE_INTERVAL_READ_PROPERTY("weekday", weekday, int, -1)
	PHP_DATE_INTERVAL_READ_PROPERTY("weekday_behavior", weekday_behavior, int, -1)
	PHP_DATE_INTERVAL_READ_PROPERTY("first_last_day_of", first_last_day_of, int, -1)
	PHP_DATE_INTERVAL_READ_PROPERTY("invert", invert, int, 0);
	PHP_DATE_INTERVAL_READ_PROPERTY_DAYS(days);
	PHP_DATE_INTERVAL_READ_PROPERTY("special_type", special.type, unsigned int, 0);
	PHP_DATE_INTERVAL_READ_PROPERTY_I64("special_amount", special.amount);
	PHP_DATE_INTERVAL_READ_PROPERTY("have_weekday_relative", have_weekday_relative, unsigned int, 0);
	PHP_DATE_INTERVAL_READ_PROPERTY("have_special_relative", have_special_relative, unsigned int, 0);
	(*intobj)->initialized = 1;

	return 0;
} /* }}} */

/* {{{ proto DateInterval::__set_state(array array)
*/
PHP_METHOD(DateInterval, __set_state)
{
	php_interval_obj *intobj;
	zval             *array;
	HashTable        *myht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(array)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	myht = Z_ARRVAL_P(array);

	php_date_instantiate(date_ce_interval, return_value);
	intobj = Z_PHPINTERVAL_P(return_value);
	php_date_interval_initialize_from_hash(&return_value, &intobj, myht);
}
/* }}} */

/* {{{ proto DateInterval::__wakeup()
*/
PHP_METHOD(DateInterval, __wakeup)
{
	zval             *object = ZEND_THIS;
	php_interval_obj *intobj;
	HashTable        *myht;

	intobj = Z_PHPINTERVAL_P(object);

	myht = Z_OBJPROP_P(object);

	php_date_interval_initialize_from_hash(&return_value, &intobj, myht);
}
/* }}} */

/* {{{ proto DateInterval date_interval_create_from_date_string(string time)
   Uses the normal date parsers and sets up a DateInterval from the relative parts of the parsed string
*/
PHP_FUNCTION(date_interval_create_from_date_string)
{
	zend_string    *time_str = NULL;
	timelib_time   *time;
	timelib_error_container *err = NULL;
	php_interval_obj *diobj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(time_str)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	time = timelib_strtotime(ZSTR_VAL(time_str), ZSTR_LEN(time_str), &err, DATE_TIMEZONEDB, php_date_parse_tzfile_wrapper);

	if (err->error_count > 0)  {
		php_error_docref(NULL, E_WARNING, "Unknown or bad format (%s) at position %d (%c): %s", ZSTR_VAL(time_str),
			err->error_messages[0].position, err->error_messages[0].character ? err->error_messages[0].character : ' ', err->error_messages[0].message);
		RETVAL_FALSE;
		goto cleanup;
	}

	php_date_instantiate(date_ce_interval, return_value);
	diobj = Z_PHPINTERVAL_P(return_value);
	diobj->diff = timelib_rel_time_clone(&time->relative);
	diobj->initialized = 1;

cleanup:
	timelib_time_dtor(time);
	timelib_error_container_dtor(err);
}
/* }}} */

/* {{{ date_interval_format -  */
static zend_string *date_interval_format(char *format, size_t format_len, timelib_rel_time *t)
{
	smart_str            string = {0};
	size_t               i;
	int                  length, have_format_spec = 0;
	char                 buffer[33];

	if (!format_len) {
		return ZSTR_EMPTY_ALLOC();
	}

	for (i = 0; i < format_len; i++) {
		if (have_format_spec) {
			switch (format[i]) {
				case 'Y': length = slprintf(buffer, sizeof(buffer), "%02d", (int) t->y); break;
				case 'y': length = slprintf(buffer, sizeof(buffer), "%d", (int) t->y); break;

				case 'M': length = slprintf(buffer, sizeof(buffer), "%02d", (int) t->m); break;
				case 'm': length = slprintf(buffer, sizeof(buffer), "%d", (int) t->m); break;

				case 'D': length = slprintf(buffer, sizeof(buffer), "%02d", (int) t->d); break;
				case 'd': length = slprintf(buffer, sizeof(buffer), "%d", (int) t->d); break;

				case 'H': length = slprintf(buffer, sizeof(buffer), "%02d", (int) t->h); break;
				case 'h': length = slprintf(buffer, sizeof(buffer), "%d", (int) t->h); break;

				case 'I': length = slprintf(buffer, sizeof(buffer), "%02d", (int) t->i); break;
				case 'i': length = slprintf(buffer, sizeof(buffer), "%d", (int) t->i); break;

				case 'S': length = slprintf(buffer, sizeof(buffer), "%02" ZEND_LONG_FMT_SPEC, (zend_long) t->s); break;
				case 's': length = slprintf(buffer, sizeof(buffer), ZEND_LONG_FMT, (zend_long) t->s); break;

				case 'F': length = slprintf(buffer, sizeof(buffer), "%06" ZEND_LONG_FMT_SPEC, (zend_long) t->us); break;
				case 'f': length = slprintf(buffer, sizeof(buffer), ZEND_LONG_FMT, (zend_long) t->us); break;

				case 'a': {
					if ((int) t->days != -99999) {
						length = slprintf(buffer, sizeof(buffer), "%d", (int) t->days);
					} else {
						length = slprintf(buffer, sizeof(buffer), "(unknown)");
					}
				} break;
				case 'r': length = slprintf(buffer, sizeof(buffer), "%s", t->invert ? "-" : ""); break;
				case 'R': length = slprintf(buffer, sizeof(buffer), "%c", t->invert ? '-' : '+'); break;

				case '%': length = slprintf(buffer, sizeof(buffer), "%%"); break;
				default: buffer[0] = '%'; buffer[1] = format[i]; buffer[2] = '\0'; length = 2; break;
			}
			smart_str_appendl(&string, buffer, length);
			have_format_spec = 0;
		} else {
			if (format[i] == '%') {
				have_format_spec = 1;
			} else {
				smart_str_appendc(&string, format[i]);
			}
		}
	}

	smart_str_0(&string);

	if (string.s == NULL) {
		return ZSTR_EMPTY_ALLOC();
	}

	return string.s;
}
/* }}} */

/* {{{ proto string date_interval_format(DateInterval object, string format)
   Formats the interval.
*/
PHP_FUNCTION(date_interval_format)
{
	zval             *object;
	php_interval_obj *diobj;
	char             *format;
	size_t            format_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &object, date_ce_interval, &format, &format_len) == FAILURE) {
		RETURN_FALSE;
	}
	diobj = Z_PHPINTERVAL_P(object);
	DATE_CHECK_INITIALIZED(diobj->initialized, DateInterval);

	RETURN_STR(date_interval_format(format, format_len, diobj->diff));
}
/* }}} */

static int date_period_initialize(timelib_time **st, timelib_time **et, timelib_rel_time **d, zend_long *recurrences, /*const*/ char *format, size_t format_length) /* {{{ */
{
	timelib_time     *b = NULL, *e = NULL;
	timelib_rel_time *p = NULL;
	int               r = 0;
	int               retval = 0;
	timelib_error_container *errors;

	timelib_strtointerval(format, format_length, &b, &e, &p, &r, &errors);

	if (errors->error_count > 0) {
		php_error_docref(NULL, E_WARNING, "Unknown or bad format (%s)", format);
		retval = FAILURE;
	} else {
		*st = b;
		*et = e;
		*d  = p;
		*recurrences = r;
		retval = SUCCESS;
	}
	timelib_error_container_dtor(errors);
	return retval;
} /* }}} */

/* {{{ proto DatePeriod::__construct(DateTime $start, DateInterval $interval, int recurrences|DateTime $end)
   Creates new DatePeriod object.
*/
PHP_METHOD(DatePeriod, __construct)
{
	php_period_obj   *dpobj;
	php_date_obj     *dateobj;
	zval *start, *end = NULL, *interval;
	zend_long  recurrences = 0, options = 0;
	char *isostr = NULL;
	size_t   isostr_len = 0;
	timelib_time *clone;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, NULL, &error_handling);
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "OOl|l", &start, date_ce_interface, &interval, date_ce_interval, &recurrences, &options) == FAILURE) {
		if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "OOO|l", &start, date_ce_interface, &interval, date_ce_interval, &end, date_ce_interface, &options) == FAILURE) {
			if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "s|l", &isostr, &isostr_len, &options) == FAILURE) {
				php_error_docref(NULL, E_WARNING, "This constructor accepts either (DateTimeInterface, DateInterval, int) OR (DateTimeInterface, DateInterval, DateTime) OR (string) as arguments.");
				zend_restore_error_handling(&error_handling);
				return;
			}
		}
	}

	dpobj = Z_PHPPERIOD_P(ZEND_THIS);
	dpobj->current = NULL;

	if (isostr) {
		date_period_initialize(&(dpobj->start), &(dpobj->end), &(dpobj->interval), &recurrences, isostr, isostr_len);
		if (dpobj->start == NULL) {
			php_error_docref(NULL, E_WARNING, "The ISO interval '%s' did not contain a start date.", isostr);
		}
		if (dpobj->interval == NULL) {
			php_error_docref(NULL, E_WARNING, "The ISO interval '%s' did not contain an interval.", isostr);
		}
		if (dpobj->end == NULL && recurrences == 0) {
			php_error_docref(NULL, E_WARNING, "The ISO interval '%s' did not contain an end date or a recurrence count.", isostr);
		}

		if (dpobj->start) {
			timelib_update_ts(dpobj->start, NULL);
		}
		if (dpobj->end) {
			timelib_update_ts(dpobj->end, NULL);
		}
		dpobj->start_ce = date_ce_date;
	} else {
		/* init */
		php_interval_obj *intobj = Z_PHPINTERVAL_P(interval);

		/* start date */
		dateobj = Z_PHPDATE_P(start);
		clone = timelib_time_ctor();
		memcpy(clone, dateobj->time, sizeof(timelib_time));
		if (dateobj->time->tz_abbr) {
			clone->tz_abbr = timelib_strdup(dateobj->time->tz_abbr);
		}
		if (dateobj->time->tz_info) {
			clone->tz_info = dateobj->time->tz_info;
		}
		dpobj->start = clone;
		dpobj->start_ce = Z_OBJCE_P(start);

		/* interval */
		dpobj->interval = timelib_rel_time_clone(intobj->diff);

		/* end date */
		if (end) {
			dateobj = Z_PHPDATE_P(end);
			clone = timelib_time_clone(dateobj->time);
			dpobj->end = clone;
		}
	}

	if (dpobj->end == NULL && recurrences < 1) {
		php_error_docref(NULL, E_WARNING, "The recurrence count '%d' is invalid. Needs to be > 0", (int) recurrences);
	}

	/* options */
	dpobj->include_start_date = !(options & PHP_DATE_PERIOD_EXCLUDE_START_DATE);

	/* recurrrences */
	dpobj->recurrences = recurrences + dpobj->include_start_date;

	dpobj->initialized = 1;

	zend_restore_error_handling(&error_handling);
}
/* }}} */

/* {{{ proto DatePeriod::getStartDate()
   Get start date.
*/
PHP_METHOD(DatePeriod, getStartDate)
{
	php_period_obj   *dpobj;
	php_date_obj     *dateobj;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	dpobj = Z_PHPPERIOD_P(ZEND_THIS);

        php_date_instantiate(dpobj->start_ce, return_value);
	dateobj = Z_PHPDATE_P(return_value);
	dateobj->time = timelib_time_ctor();
	*dateobj->time = *dpobj->start;
	if (dpobj->start->tz_abbr) {
		dateobj->time->tz_abbr = timelib_strdup(dpobj->start->tz_abbr);
	}
	if (dpobj->start->tz_info) {
		dateobj->time->tz_info = dpobj->start->tz_info;
	}
}
/* }}} */

/* {{{ proto DatePeriod::getEndDate()
   Get end date.
*/
PHP_METHOD(DatePeriod, getEndDate)
{
        php_period_obj   *dpobj;
        php_date_obj     *dateobj;

        if (zend_parse_parameters_none() == FAILURE) {
                return;
        }

        dpobj = Z_PHPPERIOD_P(ZEND_THIS);

        if (!dpobj->end) {
                return;
        }

        php_date_instantiate(dpobj->start_ce, return_value);
        dateobj = Z_PHPDATE_P(return_value);
        dateobj->time = timelib_time_ctor();
        *dateobj->time = *dpobj->end;
        if (dpobj->end->tz_abbr) {
                dateobj->time->tz_abbr = timelib_strdup(dpobj->end->tz_abbr);
        }
        if (dpobj->end->tz_info) {
                dateobj->time->tz_info = dpobj->end->tz_info;
        }
}
/* }}} */

/* {{{ proto DatePeriod::getDateInterval()
   Get date interval.
*/
PHP_METHOD(DatePeriod, getDateInterval)
{
	php_period_obj   *dpobj;
	php_interval_obj *diobj;

        if (zend_parse_parameters_none() == FAILURE) {
                return;
        }

	dpobj = Z_PHPPERIOD_P(ZEND_THIS);

	php_date_instantiate(date_ce_interval, return_value);
	diobj = Z_PHPINTERVAL_P(return_value);
	diobj->diff = timelib_rel_time_clone(dpobj->interval);
	diobj->initialized = 1;
}
/* }}} */

/* {{{ proto int DatePeriod::getRecurrences()
   Get recurrences.
*/
PHP_METHOD(DatePeriod, getRecurrences)
{
	php_period_obj   *dpobj;

	if (zend_parse_parameters_none() == FAILURE) {
			return;
	}

	dpobj = Z_PHPPERIOD_P(ZEND_THIS);

	if (0 == dpobj->recurrences - dpobj->include_start_date) {
			return;
	}

	RETURN_LONG(dpobj->recurrences - dpobj->include_start_date);
}
/* }}} */

static int check_id_allowed(char *id, zend_long what) /* {{{ */
{
	if (what & PHP_DATE_TIMEZONE_GROUP_AFRICA     && strncasecmp(id, "Africa/",      7) == 0) return 1;
	if (what & PHP_DATE_TIMEZONE_GROUP_AMERICA    && strncasecmp(id, "America/",     8) == 0) return 1;
	if (what & PHP_DATE_TIMEZONE_GROUP_ANTARCTICA && strncasecmp(id, "Antarctica/", 11) == 0) return 1;
	if (what & PHP_DATE_TIMEZONE_GROUP_ARCTIC     && strncasecmp(id, "Arctic/",      7) == 0) return 1;
	if (what & PHP_DATE_TIMEZONE_GROUP_ASIA       && strncasecmp(id, "Asia/",        5) == 0) return 1;
	if (what & PHP_DATE_TIMEZONE_GROUP_ATLANTIC   && strncasecmp(id, "Atlantic/",    9) == 0) return 1;
	if (what & PHP_DATE_TIMEZONE_GROUP_AUSTRALIA  && strncasecmp(id, "Australia/",  10) == 0) return 1;
	if (what & PHP_DATE_TIMEZONE_GROUP_EUROPE     && strncasecmp(id, "Europe/",      7) == 0) return 1;
	if (what & PHP_DATE_TIMEZONE_GROUP_INDIAN     && strncasecmp(id, "Indian/",      7) == 0) return 1;
	if (what & PHP_DATE_TIMEZONE_GROUP_PACIFIC    && strncasecmp(id, "Pacific/",     8) == 0) return 1;
	if (what & PHP_DATE_TIMEZONE_GROUP_UTC        && strncasecmp(id, "UTC",          3) == 0) return 1;
	return 0;
} /* }}} */

/* {{{ proto array timezone_identifiers_list([long what[, string country]])
   Returns numerically index array with all timezone identifiers.
*/
PHP_FUNCTION(timezone_identifiers_list)
{
	const timelib_tzdb             *tzdb;
	const timelib_tzdb_index_entry *table;
	int                             i, item_count;
	zend_long                       what = PHP_DATE_TIMEZONE_GROUP_ALL;
	char                           *option = NULL;
	size_t                          option_len = 0;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(what)
		Z_PARAM_STRING_EX(option, option_len, 1, 0)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	/* Extra validation */
	if (what == PHP_DATE_TIMEZONE_PER_COUNTRY && option_len != 2) {
		php_error_docref(NULL, E_NOTICE, "A two-letter ISO 3166-1 compatible country code is expected");
		RETURN_FALSE;
	}

	tzdb = DATE_TIMEZONEDB;
	table = timelib_timezone_identifiers_list((timelib_tzdb*) tzdb, &item_count);

	array_init(return_value);

	for (i = 0; i < item_count; ++i) {
		if (what == PHP_DATE_TIMEZONE_PER_COUNTRY) {
			if (tzdb->data[table[i].pos + 5] == option[0] && tzdb->data[table[i].pos + 6] == option[1]) {
				add_next_index_string(return_value, table[i].id);
			}
		} else if (what == PHP_DATE_TIMEZONE_GROUP_ALL_W_BC || (check_id_allowed(table[i].id, what) && (tzdb->data[table[i].pos + 4] == '\1'))) {
			add_next_index_string(return_value, table[i].id);
		}
	};
}
/* }}} */

/* {{{ proto array timezone_version_get()
   Returns the Olson database version number.
*/
PHP_FUNCTION(timezone_version_get)
{
	const timelib_tzdb *tzdb;

	tzdb = DATE_TIMEZONEDB;
	RETURN_STRING(tzdb->version);
}
/* }}} */

/* {{{ proto array timezone_abbreviations_list()
   Returns associative array containing dst, offset and the timezone name
*/
PHP_FUNCTION(timezone_abbreviations_list)
{
	const timelib_tz_lookup_table *table, *entry;
	zval                          element, *abbr_array_p, abbr_array;

	table = timelib_timezone_abbreviations_list();
	array_init(return_value);
	entry = table;

	do {
		array_init(&element);
		add_assoc_bool_ex(&element, "dst", sizeof("dst") -1, entry->type);
		add_assoc_long_ex(&element, "offset", sizeof("offset") - 1, entry->gmtoffset);
		if (entry->full_tz_name) {
			add_assoc_string_ex(&element, "timezone_id", sizeof("timezone_id") - 1, entry->full_tz_name);
		} else {
			add_assoc_null_ex(&element, "timezone_id", sizeof("timezone_id") - 1);
		}

		abbr_array_p = zend_hash_str_find(Z_ARRVAL_P(return_value), entry->name, strlen(entry->name));
		if (!abbr_array_p) {
			array_init(&abbr_array);
			add_assoc_zval(return_value, entry->name, &abbr_array);
		} else {
			ZVAL_COPY_VALUE(&abbr_array, abbr_array_p);
		}
		add_next_index_zval(&abbr_array, &element);
		entry++;
	} while (entry->name);
}
/* }}} */

/* {{{ proto bool date_default_timezone_set(string timezone_identifier)
   Sets the default timezone used by all date/time functions in a script */
PHP_FUNCTION(date_default_timezone_set)
{
	char *zone;
	size_t   zone_len;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(zone, zone_len)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (!timelib_timezone_id_is_valid(zone, DATE_TIMEZONEDB)) {
		php_error_docref(NULL, E_NOTICE, "Timezone ID '%s' is invalid", zone);
		RETURN_FALSE;
	}
	if (DATEG(timezone)) {
		efree(DATEG(timezone));
		DATEG(timezone) = NULL;
	}
	DATEG(timezone) = estrndup(zone, zone_len);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string date_default_timezone_get()
   Gets the default timezone used by all date/time functions in a script */
PHP_FUNCTION(date_default_timezone_get)
{
	timelib_tzinfo *default_tz;
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	default_tz = get_timezone_info();
	RETVAL_STRING(default_tz->name);
}
/* }}} */

/* {{{ php_do_date_sunrise_sunset
 *  Common for date_sunrise() and date_sunset() functions
 */
static void php_do_date_sunrise_sunset(INTERNAL_FUNCTION_PARAMETERS, int calc_sunset)
{
	double latitude = 0.0, longitude = 0.0, zenith = 0.0, gmt_offset = 0, altitude;
	double h_rise, h_set, N;
	timelib_sll rise, set, transit;
	zend_long time, retformat = 0;
	int             rs;
	timelib_time   *t;
	timelib_tzinfo *tzi;
	zend_string    *retstr;

	ZEND_PARSE_PARAMETERS_START(1, 6)
		Z_PARAM_LONG(time)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(retformat)
		Z_PARAM_DOUBLE(latitude)
		Z_PARAM_DOUBLE(longitude)
		Z_PARAM_DOUBLE(zenith)
		Z_PARAM_DOUBLE(gmt_offset)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	switch (ZEND_NUM_ARGS()) {
		case 1:
			retformat = SUNFUNCS_RET_STRING;
		case 2:
			latitude = INI_FLT("date.default_latitude");
		case 3:
			longitude = INI_FLT("date.default_longitude");
		case 4:
			if (calc_sunset) {
				zenith = INI_FLT("date.sunset_zenith");
			} else {
				zenith = INI_FLT("date.sunrise_zenith");
			}
		case 5:
		case 6:
			break;
		default:
			php_error_docref(NULL, E_WARNING, "invalid format");
			RETURN_FALSE;
			break;
	}
	if (retformat != SUNFUNCS_RET_TIMESTAMP &&
		retformat != SUNFUNCS_RET_STRING &&
		retformat != SUNFUNCS_RET_DOUBLE)
	{
		php_error_docref(NULL, E_WARNING, "Wrong return format given, pick one of SUNFUNCS_RET_TIMESTAMP, SUNFUNCS_RET_STRING or SUNFUNCS_RET_DOUBLE");
		RETURN_FALSE;
	}
	altitude = 90 - zenith;

	/* Initialize time struct */
	t = timelib_time_ctor();
	tzi = get_timezone_info();
	t->tz_info = tzi;
	t->zone_type = TIMELIB_ZONETYPE_ID;

	if (ZEND_NUM_ARGS() <= 5) {
		gmt_offset = timelib_get_current_offset(t) / 3600;
	}

	timelib_unixtime2local(t, time);
	rs = timelib_astro_rise_set_altitude(t, longitude, latitude, altitude, 1, &h_rise, &h_set, &rise, &set, &transit);
	timelib_time_dtor(t);

	if (rs != 0) {
		RETURN_FALSE;
	}

	if (retformat == SUNFUNCS_RET_TIMESTAMP) {
		RETURN_LONG(calc_sunset ? set : rise);
	}
	N = (calc_sunset ? h_set : h_rise) + gmt_offset;

	if (N > 24 || N < 0) {
		N -= floor(N / 24) * 24;
	}

	switch (retformat) {
		case SUNFUNCS_RET_STRING:
			retstr = strpprintf(0, "%02d:%02d", (int) N, (int) (60 * (N - (int) N)));
			RETURN_NEW_STR(retstr);
			break;
		case SUNFUNCS_RET_DOUBLE:
			RETURN_DOUBLE(N);
			break;
	}
}
/* }}} */

/* {{{ proto mixed date_sunrise(mixed time [, int format [, float latitude [, float longitude [, float zenith [, float gmt_offset]]]]])
   Returns time of sunrise for a given day and location */
PHP_FUNCTION(date_sunrise)
{
	php_do_date_sunrise_sunset(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto mixed date_sunset(mixed time [, int format [, float latitude [, float longitude [, float zenith [, float gmt_offset]]]]])
   Returns time of sunset for a given day and location */
PHP_FUNCTION(date_sunset)
{
	php_do_date_sunrise_sunset(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto array date_sun_info(int time, float latitude, float longitude)
   Returns an array with information about sun set/rise and twilight begin/end */
PHP_FUNCTION(date_sun_info)
{
	zend_long       time;
	double          latitude, longitude;
	timelib_time   *t, *t2;
	timelib_tzinfo *tzi;
	int             rs;
	timelib_sll     rise, set, transit;
	int             dummy;
	double          ddummy;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(time)
		Z_PARAM_DOUBLE(latitude)
		Z_PARAM_DOUBLE(longitude)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	/* Initialize time struct */
	t = timelib_time_ctor();
	tzi = get_timezone_info();
	t->tz_info = tzi;
	t->zone_type = TIMELIB_ZONETYPE_ID;
	timelib_unixtime2local(t, time);

	/* Setup */
	t2 = timelib_time_ctor();
	array_init(return_value);

	/* Get sun up/down and transit */
	rs = timelib_astro_rise_set_altitude(t, longitude, latitude, -35.0/60, 1, &ddummy, &ddummy, &rise, &set, &transit);
	switch (rs) {
		case -1: /* always below */
			add_assoc_bool(return_value, "sunrise", 0);
			add_assoc_bool(return_value, "sunset", 0);
			break;
		case 1: /* always above */
			add_assoc_bool(return_value, "sunrise", 1);
			add_assoc_bool(return_value, "sunset", 1);
			break;
		default:
			t2->sse = rise;
			add_assoc_long(return_value, "sunrise", timelib_date_to_int(t2, &dummy));
			t2->sse = set;
			add_assoc_long(return_value, "sunset", timelib_date_to_int(t2, &dummy));
	}
	t2->sse = transit;
	add_assoc_long(return_value, "transit", timelib_date_to_int(t2, &dummy));

	/* Get civil twilight */
	rs = timelib_astro_rise_set_altitude(t, longitude, latitude, -6.0, 0, &ddummy, &ddummy, &rise, &set, &transit);
	switch (rs) {
		case -1: /* always below */
			add_assoc_bool(return_value, "civil_twilight_begin", 0);
			add_assoc_bool(return_value, "civil_twilight_end", 0);
			break;
		case 1: /* always above */
			add_assoc_bool(return_value, "civil_twilight_begin", 1);
			add_assoc_bool(return_value, "civil_twilight_end", 1);
			break;
		default:
			t2->sse = rise;
			add_assoc_long(return_value, "civil_twilight_begin", timelib_date_to_int(t2, &dummy));
			t2->sse = set;
			add_assoc_long(return_value, "civil_twilight_end", timelib_date_to_int(t2, &dummy));
	}

	/* Get nautical twilight */
	rs = timelib_astro_rise_set_altitude(t, longitude, latitude, -12.0, 0, &ddummy, &ddummy, &rise, &set, &transit);
	switch (rs) {
		case -1: /* always below */
			add_assoc_bool(return_value, "nautical_twilight_begin", 0);
			add_assoc_bool(return_value, "nautical_twilight_end", 0);
			break;
		case 1: /* always above */
			add_assoc_bool(return_value, "nautical_twilight_begin", 1);
			add_assoc_bool(return_value, "nautical_twilight_end", 1);
			break;
		default:
			t2->sse = rise;
			add_assoc_long(return_value, "nautical_twilight_begin", timelib_date_to_int(t2, &dummy));
			t2->sse = set;
			add_assoc_long(return_value, "nautical_twilight_end", timelib_date_to_int(t2, &dummy));
	}

	/* Get astronomical twilight */
	rs = timelib_astro_rise_set_altitude(t, longitude, latitude, -18.0, 0, &ddummy, &ddummy, &rise, &set, &transit);
	switch (rs) {
		case -1: /* always below */
			add_assoc_bool(return_value, "astronomical_twilight_begin", 0);
			add_assoc_bool(return_value, "astronomical_twilight_end", 0);
			break;
		case 1: /* always above */
			add_assoc_bool(return_value, "astronomical_twilight_begin", 1);
			add_assoc_bool(return_value, "astronomical_twilight_end", 1);
			break;
		default:
			t2->sse = rise;
			add_assoc_long(return_value, "astronomical_twilight_begin", timelib_date_to_int(t2, &dummy));
			t2->sse = set;
			add_assoc_long(return_value, "astronomical_twilight_end", timelib_date_to_int(t2, &dummy));
	}
	timelib_time_dtor(t);
	timelib_time_dtor(t2);
}
/* }}} */

static HashTable *date_object_get_gc_period(zval *object, zval **table, int *n) /* {{{ */
{
	*table = NULL;
	*n = 0;
	return zend_std_get_properties(object);
} /* }}} */

static HashTable *date_object_get_properties_period(zval *object) /* {{{ */
{
	HashTable		*props;
	zval			 zv;
	php_period_obj	*period_obj;

	period_obj = Z_PHPPERIOD_P(object);
	props = zend_std_get_properties(object);
	if (!period_obj->start) {
		return props;
	}

	if (period_obj->start) {
		php_date_obj *date_obj;
		object_init_ex(&zv, period_obj->start_ce);
		date_obj = Z_PHPDATE_P(&zv);
		date_obj->time = timelib_time_clone(period_obj->start);
	} else {
		ZVAL_NULL(&zv);
	}
	zend_hash_str_update(props, "start", sizeof("start")-1, &zv);

	if (period_obj->current) {
		php_date_obj *date_obj;
		object_init_ex(&zv, period_obj->start_ce);
		date_obj = Z_PHPDATE_P(&zv);
		date_obj->time = timelib_time_clone(period_obj->current);
	} else {
		ZVAL_NULL(&zv);
	}
	zend_hash_str_update(props, "current", sizeof("current")-1, &zv);

	if (period_obj->end) {
		php_date_obj *date_obj;
		object_init_ex(&zv, period_obj->start_ce);
		date_obj = Z_PHPDATE_P(&zv);
		date_obj->time = timelib_time_clone(period_obj->end);
	} else {
		ZVAL_NULL(&zv);
	}
	zend_hash_str_update(props, "end", sizeof("end")-1, &zv);

	if (period_obj->interval) {
		php_interval_obj *interval_obj;
		object_init_ex(&zv, date_ce_interval);
		interval_obj = Z_PHPINTERVAL_P(&zv);
		interval_obj->diff = timelib_rel_time_clone(period_obj->interval);
		interval_obj->initialized = 1;
	} else {
		ZVAL_NULL(&zv);
	}
	zend_hash_str_update(props, "interval", sizeof("interval")-1, &zv);

	/* converted to larger type (int->long); must check when unserializing */
	ZVAL_LONG(&zv, (zend_long) period_obj->recurrences);
	zend_hash_str_update(props, "recurrences", sizeof("recurrences")-1, &zv);

	ZVAL_BOOL(&zv, period_obj->include_start_date);
	zend_hash_str_update(props, "include_start_date", sizeof("include_start_date")-1, &zv);

	return props;
} /* }}} */

static int php_date_period_initialize_from_hash(php_period_obj *period_obj, HashTable *myht) /* {{{ */
{
	zval *ht_entry;

	/* this function does no rollback on error */

	ht_entry = zend_hash_str_find(myht, "start", sizeof("start")-1);
	if (ht_entry) {
		if (Z_TYPE_P(ht_entry) == IS_OBJECT && instanceof_function(Z_OBJCE_P(ht_entry), date_ce_interface)) {
			php_date_obj *date_obj;
			date_obj = Z_PHPDATE_P(ht_entry);
			period_obj->start = timelib_time_clone(date_obj->time);
			period_obj->start_ce = Z_OBJCE_P(ht_entry);
		} else if (Z_TYPE_P(ht_entry) != IS_NULL) {
			return 0;
		}
	} else {
		return 0;
	}

	ht_entry = zend_hash_str_find(myht, "end", sizeof("end")-1);
	if (ht_entry) {
		if (Z_TYPE_P(ht_entry) == IS_OBJECT && instanceof_function(Z_OBJCE_P(ht_entry), date_ce_interface)) {
			php_date_obj *date_obj;
			date_obj = Z_PHPDATE_P(ht_entry);
			period_obj->end = timelib_time_clone(date_obj->time);
		} else if (Z_TYPE_P(ht_entry) != IS_NULL) {
			return 0;
		}
	} else {
		return 0;
	}

	ht_entry = zend_hash_str_find(myht, "current", sizeof("current")-1);
	if (ht_entry) {
		if (Z_TYPE_P(ht_entry) == IS_OBJECT && instanceof_function(Z_OBJCE_P(ht_entry), date_ce_interface)) {
			php_date_obj *date_obj;
			date_obj = Z_PHPDATE_P(ht_entry);
			period_obj->current = timelib_time_clone(date_obj->time);
		} else if (Z_TYPE_P(ht_entry) != IS_NULL)  {
			return 0;
		}
	} else {
		return 0;
	}

	ht_entry = zend_hash_str_find(myht, "interval", sizeof("interval")-1);
	if (ht_entry) {
		if (Z_TYPE_P(ht_entry) == IS_OBJECT && Z_OBJCE_P(ht_entry) == date_ce_interval) {
			php_interval_obj *interval_obj;
			interval_obj = Z_PHPINTERVAL_P(ht_entry);
			period_obj->interval = timelib_rel_time_clone(interval_obj->diff);
		} else { /* interval is required */
			return 0;
		}
	} else {
		return 0;
	}

	ht_entry = zend_hash_str_find(myht, "recurrences", sizeof("recurrences")-1);
	if (ht_entry &&
			Z_TYPE_P(ht_entry) == IS_LONG && Z_LVAL_P(ht_entry) >= 0 && Z_LVAL_P(ht_entry) <= INT_MAX) {
		period_obj->recurrences = Z_LVAL_P(ht_entry);
	} else {
		return 0;
	}

	ht_entry = zend_hash_str_find(myht, "include_start_date", sizeof("include_start_date")-1);
	if (ht_entry &&
			(Z_TYPE_P(ht_entry) == IS_FALSE || Z_TYPE_P(ht_entry) == IS_TRUE)) {
		period_obj->include_start_date = (Z_TYPE_P(ht_entry) == IS_TRUE);
	} else {
		return 0;
	}

	period_obj->initialized = 1;

	return 1;
} /* }}} */

/* {{{ proto DatePeriod::__set_state(array array)
*/
PHP_METHOD(DatePeriod, __set_state)
{
	php_period_obj   *period_obj;
	zval             *array;
	HashTable        *myht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(array)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	myht = Z_ARRVAL_P(array);

	object_init_ex(return_value, date_ce_period);
	period_obj = Z_PHPPERIOD_P(return_value);
	if (!php_date_period_initialize_from_hash(period_obj, myht)) {
		zend_throw_error(NULL, "Invalid serialization data for DatePeriod object");
	}
}
/* }}} */

/* {{{ proto DatePeriod::__wakeup()
*/
PHP_METHOD(DatePeriod, __wakeup)
{
	zval             *object = ZEND_THIS;
	php_period_obj   *period_obj;
	HashTable        *myht;

	period_obj = Z_PHPPERIOD_P(object);

	myht = Z_OBJPROP_P(object);

	if (!php_date_period_initialize_from_hash(period_obj, myht)) {
		zend_throw_error(NULL, "Invalid serialization data for DatePeriod object");
	}
}
/* }}} */

/* {{{ date_period_is_magic_property
 *  Common for date_period_read_property() and date_period_write_property() functions
 */
static int date_period_is_magic_property(zend_string *name)
{
	if (zend_string_equals_literal(name, "recurrences")
		|| zend_string_equals_literal(name, "include_start_date")
		|| zend_string_equals_literal(name, "start")
		|| zend_string_equals_literal(name, "current")
		|| zend_string_equals_literal(name, "end")
		|| zend_string_equals_literal(name, "interval")
	) {
		return 1;
	}
	return 0;
}
/* }}} */

/* {{{ date_period_read_property */
static zval *date_period_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv)
{
	if (type != BP_VAR_IS && type != BP_VAR_R) {
		zend_string *name = zval_get_string(member);
		if (date_period_is_magic_property(name)) {
			zend_throw_error(NULL, "Retrieval of DatePeriod->%s for modification is unsupported", ZSTR_VAL(name));
			zend_string_release(name);
			return &EG(uninitialized_zval);
		}
		zend_string_release(name);
	}

	Z_OBJPROP_P(object); /* build properties hash table */

	return zend_std_read_property(object, member, type, cache_slot, rv);
}
/* }}} */

/* {{{ date_period_write_property */
static zval *date_period_write_property(zval *object, zval *member, zval *value, void **cache_slot)
{
	zend_string *name = zval_get_string(member);
	if (date_period_is_magic_property(name)) {
		zend_throw_error(NULL, "Writing to DatePeriod->%s is unsupported", ZSTR_VAL(name));
		zend_string_release(name);
		return value;
	}
	zend_string_release(name);

	return zend_std_write_property(object, member, value, cache_slot);
}
/* }}} */

/* {{{ date_period_get_property_ptr_ptr */
static zval *date_period_get_property_ptr_ptr(zval *object, zval *member, int type, void **cache_slot)
{
	zend_string *name = zval_get_string(member);
	if (date_period_is_magic_property(name)) {
		zend_throw_error(NULL, "Retrieval of DatePeriod->%s for modification is unsupported", ZSTR_VAL(name));
		zend_string_release(name);
		return &EG(error_zval);
	}
	zend_string_release(name);

	return zend_std_get_property_ptr_ptr(object, member, type, cache_slot);
}
/* }}} */
