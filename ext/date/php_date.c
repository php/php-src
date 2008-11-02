/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
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
#include <time.h>
#include <unicode/udat.h>

/* {{{ arginfo */
static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_gmdate, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_idate, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_strtotime, 0, 0, 1)
	ZEND_ARG_INFO(0, time)
	ZEND_ARG_INFO(0, now)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_mktime, 0, 0, 0)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, sec)
	ZEND_ARG_INFO(0, mon)
	ZEND_ARG_INFO(0, day)
	ZEND_ARG_INFO(0, year)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_gmmktime, 0, 0, 0)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, sec)
	ZEND_ARG_INFO(0, mon)
	ZEND_ARG_INFO(0, day)
	ZEND_ARG_INFO(0, year)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_checkdate, 0)
	ZEND_ARG_INFO(0, month)
	ZEND_ARG_INFO(0, day)
	ZEND_ARG_INFO(0, year)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_strftime, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_gmstrftime, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_time, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_localtime, 0, 0, 0)
	ZEND_ARG_INFO(0, timestamp)
	ZEND_ARG_INFO(0, associative_array)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_getdate, 0, 0, 0)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_date_default_timezone_set, 0)
	ZEND_ARG_INFO(0, timezone_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_date_default_timezone_get, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_sunrise, 0, 0, 1)
	ZEND_ARG_INFO(0, time)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, latitude)
	ZEND_ARG_INFO(0, longitude)
	ZEND_ARG_INFO(0, zenith)
	ZEND_ARG_INFO(0, gmt_offset)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_sunset, 0, 0, 1)
	ZEND_ARG_INFO(0, time)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, latitude)
	ZEND_ARG_INFO(0, longitude)
	ZEND_ARG_INFO(0, zenith)
	ZEND_ARG_INFO(0, gmt_offset)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_date_sun_info, 0)
	ZEND_ARG_INFO(0, time)
	ZEND_ARG_INFO(0, latitude)
	ZEND_ARG_INFO(0, longitude)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_create, 0, 0, 0)
	ZEND_ARG_INFO(0, time)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_create_from_format, 0, 0, 2)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, time)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_parse, 0, 0, 1)
	ZEND_ARG_INFO(0, date)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_parse_from_format, 0, 0, 2)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, date)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_date_get_last_errors, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_format, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, format)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_format, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_modify, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, modify)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_modify, 0, 0, 1)
	ZEND_ARG_INFO(0, modify)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_add, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, interval)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_add, 0, 0, 1)
	ZEND_ARG_INFO(0, interval)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_sub, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, interval)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_sub, 0, 0, 1)
	ZEND_ARG_INFO(0, interval)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_timezone_get, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_date_method_timezone_get, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_timezone_set, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, timezone)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_timezone_set, 0, 0, 1)
	ZEND_ARG_INFO(0, timezone)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_offset_get, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_date_method_offset_get, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_diff, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, object2)
	ZEND_ARG_INFO(0, absolute)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_diff, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, absolute)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_time_set, 0, 0, 3)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_INFO(0, minute)
	ZEND_ARG_INFO(0, second)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_time_set, 0, 0, 2)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_INFO(0, minute)
	ZEND_ARG_INFO(0, second)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_date_set, 0, 0, 4)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, year)
	ZEND_ARG_INFO(0, month)
	ZEND_ARG_INFO(0, day)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_date_set, 0, 0, 3)
	ZEND_ARG_INFO(0, year)
	ZEND_ARG_INFO(0, month)
	ZEND_ARG_INFO(0, day)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_isodate_set, 0, 0, 3)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, year)
	ZEND_ARG_INFO(0, week)
	ZEND_ARG_INFO(0, day)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_isodate_set, 0, 0, 2)
	ZEND_ARG_INFO(0, year)
	ZEND_ARG_INFO(0, week)
	ZEND_ARG_INFO(0, day)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_timestamp_set, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, unixtimestamp)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_method_timestamp_set, 0, 0, 1)
	ZEND_ARG_INFO(0, unixtimestamp)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_timestamp_get, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_date_method_timestamp_get, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_timezone_open, 0, 0, 1)
	ZEND_ARG_INFO(0, timezone)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_timezone_name_get, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_timezone_method_name_get, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_timezone_name_from_abbr, 0, 0, 1)
	ZEND_ARG_INFO(0, abbr)
	ZEND_ARG_INFO(0, gmtoffset)
	ZEND_ARG_INFO(0, isdst)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_timezone_offset_get, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, datetime)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_timezone_method_offset_get, 0, 0, 1)
	ZEND_ARG_INFO(0, datetime)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_timezone_transitions_get, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, timestamp_begin)
	ZEND_ARG_INFO(0, timestamp_end)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_timezone_method_transitions_get, 0)
	ZEND_ARG_INFO(0, timestamp_begin)
	ZEND_ARG_INFO(0, timestamp_end)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_timezone_location_get, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_timezone_method_location_get, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_timezone_identifiers_list, 0, 0, 0)
	ZEND_ARG_INFO(0, what)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_timezone_abbreviations_list, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_interval_create_from_date_string, 0, 0, 1)
	ZEND_ARG_INFO(0, time)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_interval_format, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_date_method_interval_format, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_period_construct, 0, 0, 3)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, interval)
	ZEND_ARG_INFO(0, end)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_interval_construct, 0, 0, 0)
	ZEND_ARG_INFO(0, interval_spec)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_format_locale, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, format)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ Function table */
const zend_function_entry date_functions[] = {
	PHP_FE(strtotime, arginfo_strtotime)
	PHP_FE(date, arginfo_date)
	PHP_FE(idate, arginfo_idate)
	PHP_FE(gmdate, arginfo_gmdate)
	PHP_FE(mktime, arginfo_mktime)
	PHP_FE(gmmktime, arginfo_gmmktime)
	PHP_FE(checkdate, arginfo_checkdate)

#ifdef HAVE_STRFTIME
	PHP_FE(strftime, arginfo_strftime)
	PHP_FE(gmstrftime, arginfo_gmstrftime)
#endif

	PHP_FE(time, arginfo_time)
	PHP_FE(localtime, arginfo_localtime)
	PHP_FE(getdate, arginfo_getdate)

	/* Advanced Interface */
	PHP_FE(date_create, arginfo_date_create)
	PHP_FE(date_create_from_format, arginfo_date_create_from_format)
	PHP_FE(date_parse, arginfo_date_parse)
	PHP_FE(date_parse_from_format, arginfo_date_parse_from_format)
	PHP_FE(date_get_last_errors, arginfo_date_get_last_errors)
	PHP_FE(date_format, arginfo_date_format)
	PHP_FE(date_format_locale, arginfo_date_format_locale)
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

	PHP_FE(date_interval_create_from_date_string, arginfo_date_interval_create_from_date_string)
	PHP_FE(date_interval_format, arginfo_date_interval_format)

	/* Options and Configuration */
	PHP_FE(date_default_timezone_set, arginfo_date_default_timezone_set)
	PHP_FE(date_default_timezone_get, arginfo_date_default_timezone_get)

	/* Astronomical functions */
	PHP_FE(date_sunrise, arginfo_date_sunrise)
	PHP_FE(date_sunset, arginfo_date_sunset)
	PHP_FE(date_sun_info, arginfo_date_sun_info)
	{NULL, NULL, NULL}
};

const zend_function_entry date_funcs_date[] = {
	PHP_ME(DateTime,			__construct,		arginfo_date_create, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
	PHP_ME(DateTime,			__wakeup,			NULL, ZEND_ACC_PUBLIC)
	PHP_ME(DateTime,			__set_state,		NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
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
	PHP_ME_MAPPING(setISODate,	date_isodate_set,	arginfo_date_isodate_set, 0)
	PHP_ME_MAPPING(setTimestamp,	date_timestamp_set, arginfo_date_method_timestamp_set, 0)
	PHP_ME_MAPPING(getTimestamp,	date_timestamp_get, arginfo_date_method_timestamp_get, 0)
	PHP_ME_MAPPING(diff,			date_diff, arginfo_date_method_diff, 0)
	{NULL, NULL, NULL}
};

const zend_function_entry date_funcs_timezone[] = {
	PHP_ME(DateTimeZone,              __construct,                 arginfo_timezone_open, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getName,           timezone_name_get,           arginfo_timezone_method_name_get, 0)
	PHP_ME_MAPPING(getOffset,         timezone_offset_get,         arginfo_timezone_method_offset_get, 0)
	PHP_ME_MAPPING(getTransitions,    timezone_transitions_get,    arginfo_timezone_method_transitions_get, 0)
	PHP_ME_MAPPING(getLocation,       timezone_location_get,       arginfo_timezone_method_location_get, 0)
	PHP_ME_MAPPING(listAbbreviations, timezone_abbreviations_list, arginfo_timezone_abbreviations_list, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME_MAPPING(listIdentifiers,   timezone_identifiers_list,   arginfo_timezone_identifiers_list, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	{NULL, NULL, NULL}
};

const zend_function_entry date_funcs_interval[] = {
	PHP_ME(DateInterval,              __construct,                 arginfo_date_interval_construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(format,            date_interval_format,        arginfo_date_method_interval_format, 0)
	PHP_ME_MAPPING(createFromDateString, date_interval_create_from_date_string,	arginfo_date_interval_create_from_date_string, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	{NULL, NULL, NULL}
};

const zend_function_entry date_funcs_period[] = {
	PHP_ME(DatePeriod,                __construct,                 arginfo_date_period_construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static char* guess_timezone(const timelib_tzdb *tzdb TSRMLS_DC);
static void date_register_classes(TSRMLS_D);
static zval * date_instantiate(zend_class_entry *pce, zval *object TSRMLS_DC);
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

/* {{{ INI Settings */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("date.timezone", "", PHP_INI_ALL, OnUpdateString, default_timezone, zend_date_globals, date_globals)
	PHP_INI_ENTRY("date.default_latitude",           DATE_DEFAULT_LATITUDE,        PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("date.default_longitude",          DATE_DEFAULT_LONGITUDE,       PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("date.sunset_zenith",              DATE_SUNSET_ZENITH,           PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("date.sunrise_zenith",             DATE_SUNRISE_ZENITH,          PHP_INI_ALL, NULL)
PHP_INI_END()
/* }}} */

zend_class_entry *date_ce_date, *date_ce_timezone, *date_ce_interval, *date_ce_period;

static zend_object_handlers date_object_handlers_date;
static zend_object_handlers date_object_handlers_timezone;
static zend_object_handlers date_object_handlers_interval;
static zend_object_handlers date_object_handlers_period;

typedef struct _php_date_obj php_date_obj;
typedef struct _php_timezone_obj php_timezone_obj;
typedef struct _php_interval_obj php_interval_obj;
typedef struct _php_period_obj php_period_obj;

struct _php_date_obj {
	zend_object   std;
	timelib_time *time;
	HashTable    *props;
};

struct _php_timezone_obj {
	zend_object     std;
	int             initialized;
	int             type;
	union {
		timelib_tzinfo *tz; /* TIMELIB_ZONETYPE_ID; */
		timelib_sll     utc_offset; /* TIMELIB_ZONETYPE_OFFSET */
		struct                      /* TIMELIB_ZONETYPE_ABBR */
		{
			timelib_sll  utc_offset;
			char        *abbr;
			int          dst;
		} z;
	} tzi;
};

struct _php_interval_obj {
	zend_object       std;
	timelib_rel_time *diff;
	HashTable        *props;
	int               initialized;
};

struct _php_period_obj {
	zend_object       std;
	timelib_time     *start;
	timelib_time     *end;
	timelib_rel_time *interval;
	int               recurrences;
	int               initialized;
	int               include_start_date;
};

#define DATE_SET_CONTEXT \
	zval *object; \
	object = getThis(); \
   
#define DATE_FETCH_OBJECT	\
	php_date_obj *obj;	\
	DATE_SET_CONTEXT; \
	if (object) {	\
		if (zend_parse_parameters_none() == FAILURE) {	\
			return;	\
		}	\
	} else {	\
		if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, NULL, "O", &object, date_ce_date) == FAILURE) {	\
			RETURN_FALSE;	\
		}	\
	}	\
	obj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);	\

#define DATE_CHECK_INITIALIZED(member, class_name) \
	if (!(member)) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The " #class_name " object has not been correctly initialized by its constructor"); \
		RETURN_FALSE; \
	}

static void date_object_free_storage_date(void *object TSRMLS_DC);
static void date_object_free_storage_timezone(void *object TSRMLS_DC);
static void date_object_free_storage_interval(void *object TSRMLS_DC);
static void date_object_free_storage_period(void *object TSRMLS_DC);

static zend_object_value date_object_new_date(zend_class_entry *class_type TSRMLS_DC);
static zend_object_value date_object_new_timezone(zend_class_entry *class_type TSRMLS_DC);
static zend_object_value date_object_new_interval(zend_class_entry *class_type TSRMLS_DC);
static zend_object_value date_object_new_period(zend_class_entry *class_type TSRMLS_DC);

static zend_object_value date_object_clone_date(zval *this_ptr TSRMLS_DC);
static zend_object_value date_object_clone_timezone(zval *this_ptr TSRMLS_DC);
static zend_object_value date_object_clone_interval(zval *this_ptr TSRMLS_DC);
static zend_object_value date_object_clone_period(zval *this_ptr TSRMLS_DC);

static int date_object_compare_date(zval *d1, zval *d2 TSRMLS_DC);
static HashTable *date_object_get_properties(zval *object TSRMLS_DC);

zval *date_interval_read_property(zval *object, zval *member, int type TSRMLS_DC);
void date_interval_write_property(zval *object, zval *member, zval *value TSRMLS_DC);

/* This is need to ensure that session extension request shutdown occurs 1st, because it uses the date extension */ 
static const zend_module_dep date_deps[] = {
	ZEND_MOD_OPTIONAL("session")
	{NULL, NULL, NULL}
};

/* {{{ Module struct */
zend_module_entry date_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	date_deps,
	"date",                     /* extension name */
	date_functions,             /* function list */
	PHP_MINIT(date),            /* process startup */
	PHP_MSHUTDOWN(date),        /* process shutdown */
	PHP_RINIT(date),            /* request startup */
	PHP_RSHUTDOWN(date),        /* request shutdown */
	PHP_MINFO(date),            /* extension info */
	PHP_VERSION,                /* extension version */
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
}
/* }}} */


static void _php_date_tzinfo_dtor(void *tzinfo)
{
	timelib_tzinfo **tzi = (timelib_tzinfo **)tzinfo;

	timelib_tzinfo_dtor(*tzi);
}

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(date)
{
	if (DATEG(timezone)) {
		efree(DATEG(timezone));
	}
	DATEG(timezone) = NULL;
	zend_hash_init(&DATEG(tzcache), 4, NULL, _php_date_tzinfo_dtor, 0);

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
	zend_hash_destroy(&DATEG(tzcache));

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
 *  CDT, EST, EDT) and the +/-hhmm offset specifed in RFC-822 should be supported.
 */
#define DATE_FORMAT_RFC1036  "D, d M y H:i:s O"

/*
 * RFC1123, Section 5.2.14: http://www.ietf.org/rfc/rfc1123.txt
 *  RFC-822 Date and Time Specification: RFC-822 Section 5
 *  The syntax for the date is hereby changed to: date = 1*2DIGIT month 2*4DIGIT
 */
#define DATE_FORMAT_RFC1123  "D, d M Y H:i:s O"

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

#define DATE_TZ_ERRMSG \
	"It is not safe to rely on the system's timezone settings. Please use " \
	"the date.timezone setting, the TZ environment variable or the " \
	"date_default_timezone_set() function. In case you used any of those " \
	"methods and you are still getting this warning, you most likely " \
	"misspelled the timezone identifier. "

#define SUNFUNCS_RET_TIMESTAMP 0
#define SUNFUNCS_RET_STRING    1
#define SUNFUNCS_RET_DOUBLE    2


/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(date)
{
	REGISTER_INI_ENTRIES();
	date_register_classes(TSRMLS_C);
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
	REGISTER_STRING_CONSTANT("DATE_COOKIE",  DATE_FORMAT_RFC850,  CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_ISO8601", DATE_FORMAT_ISO8601, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_RFC822",  DATE_FORMAT_RFC822,  CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_RFC850",  DATE_FORMAT_RFC850,  CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_RFC1036", DATE_FORMAT_RFC1036, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_RFC1123", DATE_FORMAT_RFC1123, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_RFC2822", DATE_FORMAT_RFC2822, CONST_CS | CONST_PERSISTENT);
 	REGISTER_STRING_CONSTANT("DATE_RFC3339", DATE_FORMAT_RFC3339, CONST_CS | CONST_PERSISTENT);
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

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(date)
{
	const timelib_tzdb *tzdb = DATE_TIMEZONEDB;
	
	php_info_print_table_start();
	php_info_print_table_row(2, "date/time support", "enabled");
	php_info_print_table_row(2, "\"Olson\" Timezone Database Version", tzdb->version);
	php_info_print_table_row(2, "Timezone Database", php_date_global_timezone_db_enabled ? "external" : "internal");
	php_info_print_table_row(2, "Default timezone", guess_timezone(tzdb TSRMLS_CC));
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ Timezone Cache functions */
static timelib_tzinfo *php_date_parse_tzfile(char *formal_tzname, const timelib_tzdb *tzdb TSRMLS_DC)
{
	timelib_tzinfo *tzi, **ptzi;

	if (zend_hash_find(&DATEG(tzcache), formal_tzname, strlen(formal_tzname) + 1, (void **) &ptzi) == SUCCESS) {
		return *ptzi;
	}

	tzi = timelib_parse_tzfile(formal_tzname, tzdb);
	if (tzi) {
		zend_hash_add(&DATEG(tzcache), formal_tzname, strlen(formal_tzname) + 1, (void *) &tzi, sizeof(timelib_tzinfo*), NULL);
	}
	return tzi;
}
/* }}} */

/* {{{ Helper functions */
static char* guess_timezone(const timelib_tzdb *tzdb TSRMLS_DC)
{
	char *env;

	/* Checking configure timezone */
	if (DATEG(timezone) && (strlen(DATEG(timezone)) > 0)) {
		return DATEG(timezone);
	}
	/* Check environment variable */
	env = getenv("TZ");
	if (env && *env && timelib_timezone_id_is_valid(env, tzdb)) {
		return env;
	}
	/* Check config setting for default timezone */
	if (DATEG(default_timezone) && (strlen(DATEG(default_timezone)) > 0) && timelib_timezone_id_is_valid(DATEG(default_timezone), tzdb)) {
		return DATEG(default_timezone);
	}
#if HAVE_TM_ZONE
	/* Try to guess timezone from system information */
	{
		struct tm *ta, tmbuf;
		time_t     the_time;
		char      *tzid = NULL;
		
		the_time = time(NULL);
		ta = php_localtime_r(&the_time, &tmbuf);
		if (ta) {
			tzid = timelib_timezone_id_from_abbr(ta->tm_zone, ta->tm_gmtoff, ta->tm_isdst);
		}
		if (! tzid) {
			tzid = "UTC";
		}
		
		php_error_docref(NULL TSRMLS_CC, E_STRICT, DATE_TZ_ERRMSG "We selected '%s' for '%s/%.1f/%s' instead", tzid, ta ? ta->tm_zone : "Unknown", ta ? (float) (ta->tm_gmtoff / 3600) : 0, ta ? (ta->tm_isdst ? "DST" : "no DST") : "Unknown");
		return tzid;
	}
#endif
#ifdef PHP_WIN32
	{
		char *tzid;
		TIME_ZONE_INFORMATION tzi;

		switch (GetTimeZoneInformation(&tzi))
		{
			/* no DST or not in effect */
			case TIME_ZONE_ID_UNKNOWN:
			case TIME_ZONE_ID_STANDARD:
php_win_std_time:
				tzid = timelib_timezone_id_from_abbr("", (tzi.Bias + tzi.StandardBias) * -60, 0);
				if (! tzid) {
					tzid = "UTC";
				}
				php_error_docref(NULL TSRMLS_CC, E_STRICT, DATE_TZ_ERRMSG "We selected '%s' for '%.1f/no DST' instead", tzid, ((tzi.Bias + tzi.StandardBias) / -60.0));
				break;

			/* DST in effect */
			case TIME_ZONE_ID_DAYLIGHT:
				/* If user has disabled DST in the control panel, Windows returns 0 here */
				if (tzi.DaylightBias == 0) {
					goto php_win_std_time;
				}
				
				tzid = timelib_timezone_id_from_abbr("", (tzi.Bias + tzi.DaylightBias) * -60, 1);
				if (! tzid) {
					tzid = "UTC";
				}
				php_error_docref(NULL TSRMLS_CC, E_STRICT, DATE_TZ_ERRMSG "We selected '%s' for '%.1f/DST' instead", tzid, ((tzi.Bias + tzi.DaylightBias) / -60.0));
				break;
		}
		return tzid;
	}
#elif defined(NETWARE)
	/* Try to guess timezone from system information */
	{
		char *tzid = timelib_timezone_id_from_abbr("", ((_timezone * -1) + (daylightOffset * daylightOnOff)), daylightOnOff);
		if (tzid) {
			return tzid;
		}
	}
#endif
	/* Fallback to UTC */
	php_error_docref(NULL TSRMLS_CC, E_WARNING, DATE_TZ_ERRMSG "We had to select 'UTC' because your platform doesn't provide functionality for the guessing algorithm");
	return "UTC";
}

PHPAPI timelib_tzinfo *get_timezone_info(TSRMLS_D)
{
	char *tz;
	timelib_tzinfo *tzi;
	
	tz = guess_timezone(DATE_TIMEZONEDB TSRMLS_CC);
	tzi = php_date_parse_tzfile(tz, DATE_TIMEZONEDB TSRMLS_CC);
	if (! tzi) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Timezone database is corrupt - this should *never* happen!");
	}
	return tzi;
}
/* }}} */


/* {{{ date() and gmdate() data */
#include "ext/standard/php_smart_str.h"

static char *mon_full_names[] = {
	"January", "February", "March", "April",
	"May", "June", "July", "August",
	"September", "October", "November", "December"
};

static char *mon_short_names[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static char *day_full_names[] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

static char *day_short_names[] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static char *am_pm_lower_names[] = { "am", "pm" };
static char *am_pm_upper_names[] = { "AM", "PM" };

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
char *php_date_full_day_name(timelib_sll y, timelib_sll m, timelib_sll d)
{
	timelib_sll day_of_week = timelib_day_of_week(y, m, d);
	if (day_of_week < 0) {
		return "Unknown";
	} 
	return day_full_names[day_of_week];	
}

char *php_date_short_day_name(timelib_sll y, timelib_sll m, timelib_sll d)
{
	timelib_sll day_of_week = timelib_day_of_week(y, m, d);
	if (day_of_week < 0) {
		return "Unknown";
	} 
	return day_short_names[day_of_week];	
}
/* }}} */

/* {{{ date_format - (gm)date helper */

typedef struct {
	int day_shortname_lengths[7];
	int day_fullname_lengths[7];
	int month_shortname_lengths[12];
	int month_fullname_lengths[12];
	int am_pm_lenghts[2];

	char* day_shortname[7];
	char* day_fullname[7];
	char* month_shortname[12];
	char* month_fullname[12];
	char* am_pm_name[2];
} php_locale_data;

static const UChar sLongPat [] = { 0x004D, 0x004D, 0x004D, 0x004D, 0x0020,
	    0x0079, 0x0079, 0x0079, 0x0079 };


#define DATE_LOC_READ(type, var, cor)                                          \
	count = udat_countSymbols(fmt, (type));                                    \
	for (i = 0 - (cor); i < count; i++) {                                      \
		array[i] = (UChar *) malloc(sizeof(UChar) * 15);                       \
		                                                                       \
		status = U_ZERO_ERROR;                                                 \
		j = udat_getSymbols(fmt, (type), i, array[i], 15, &status);            \
		                                                                       \
		tmp->var[i + (cor)] = (char*)array[i];                                 \
	}

static php_locale_data* date_get_locale_data(char *locale)
{
	php_locale_data *tmp = malloc(sizeof(php_locale_data));
	UDateFormat *fmt;
	UErrorCode status = 0;
	int32_t count, i, j;
	UChar *array[15];
	UChar *pat = (UChar*)sLongPat;
	int32_t len = 9;

	fmt = udat_open(UDAT_IGNORE,UDAT_IGNORE, locale, NULL, 0, pat, len, &status);

	DATE_LOC_READ(UDAT_WEEKDAYS, day_fullname, -1);
	DATE_LOC_READ(UDAT_SHORT_WEEKDAYS, day_shortname, -1);
	DATE_LOC_READ(UDAT_MONTHS, month_fullname, 0);
	DATE_LOC_READ(UDAT_SHORT_MONTHS, month_shortname, 0);
	DATE_LOC_READ(UDAT_AM_PMS, am_pm_name, 0);

	udat_close(fmt);

	return tmp;
}

static void date_free_locale_data(php_locale_data *data)
{
	int i;
	for (i = 0; i < 7; ++i) {
		free(data->day_shortname[i]);
		free(data->day_fullname[i]);
	}
	for (i = 0; i < 12; ++i) {
		free(data->month_shortname[i]);
		free(data->month_fullname[i]);
	}
	free(data->am_pm_name[0]);
	free(data->am_pm_name[1]);
	free(data);
}

static inline int date_spprintf(char **str, size_t size TSRMLS_DC, const char *format, ...)
{
	int      c;
	va_list  ap;

	va_start(ap, format);

	if (UG(unicode)) {
		c = vuspprintf((UChar**)str, size, format, ap) * sizeof(UChar);
	} else {
		c = vspprintf(str, size, format, ap);
	}
	va_end(ap);
	return c;
}

#define dayname_short(s,l) s < 0 ? "Unknown" : (l ? loc_dat->day_shortname[s] : day_short_names[s])
#define dayname_full(s,l) s < 0 ? "Unknown" : (l ? loc_dat->day_fullname[s] : day_full_names[s])
#define monthname_short(s,l) l ? loc_dat->month_shortname[s] : mon_short_names[s]
#define monthname_full(s,l) l ? loc_dat->month_fullname[s] : mon_full_names[s]
#define am_pm_lower_full(s,l) l ? loc_dat->am_pm_name[s] : am_pm_lower_names[s]
#define am_pm_upper_full(s,l) l ? loc_dat->am_pm_name[s] : am_pm_upper_names[s]

static char *date_format(char *format, int format_len, int *return_len, timelib_time *t, int localtime, int localized TSRMLS_DC)
{
	smart_str            string = {0};
	int                  i, no_free, length;
	char                *buffer;
	timelib_time_offset *offset = NULL;
	timelib_sll          isoweek, isoyear;
	php_locale_data *loc_dat;
	int                  rfc_colon;

	if (!format_len) {
		if (UG(unicode)) {
			return (char*)eustrdup(EMPTY_STR);
		} else {		
			return estrdup("");
		}
	}

	loc_dat = date_get_locale_data(UG(default_locale));

	if (localtime) {
		if (t->zone_type == TIMELIB_ZONETYPE_ABBR) {
			offset = timelib_time_offset_ctor();
			offset->offset = (t->z - (t->dst * 60)) * -60;
			offset->leap_secs = 0;
			offset->is_dst = t->dst;
			offset->abbr = strdup(t->tz_abbr);
		} else if (t->zone_type == TIMELIB_ZONETYPE_OFFSET) {
			offset = timelib_time_offset_ctor();
			offset->offset = (t->z) * -60;
			offset->leap_secs = 0;
			offset->is_dst = 0;
			offset->abbr = malloc(9); /* GMT±xxxx\0 */
			snprintf(offset->abbr, 9, "GMT%c%02d%02d", 
			                          localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
			                          localtime ? abs(offset->offset / 3600) : 0,
			                          localtime ? abs((offset->offset % 3600) / 60) : 0 );
		} else {
			offset = timelib_get_time_zone_info(t->sse, t->tz_info);
		}
	}
	timelib_isoweek_from_date(t->y, t->m, t->d, &isoweek, &isoyear);

	for (i = 0; i < format_len; i++) {
		no_free = 0;
		rfc_colon = 0;
		switch (format[i]) {
			/* day */
			case 'd': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%02d", (int) t->d); break;
			case 'D': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%R", localized ? IS_UNICODE : IS_STRING, dayname_short(timelib_day_of_week(t->y, t->m, t->d), localized)); break;
			case 'j': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (int) t->d); break;
			case 'l': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%R", localized ? IS_UNICODE : IS_STRING, dayname_full(timelib_day_of_week(t->y, t->m, t->d), localized)); break;
			case 'S': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%s", english_suffix(t->d)); break;
			case 'w': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (int) timelib_day_of_week(t->y, t->m, t->d)); break;
			case 'N': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (int) timelib_iso_day_of_week(t->y, t->m, t->d)); break;
			case 'z': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (int) timelib_day_of_year(t->y, t->m, t->d)); break;

			/* week */
			case 'W': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%02d", (int) isoweek); break; /* iso weeknr */
			case 'o': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (int) isoyear); break; /* iso year */

			/* month */
			case 'F': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%R", localized ? IS_UNICODE : IS_STRING, monthname_full(t->m - 1, localized)); break;
			case 'm': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%02d", (int) t->m); break;
			case 'M': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%R", localized ? IS_UNICODE : IS_STRING, monthname_short(t->m - 1, localized)); break;
			case 'n': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (int) t->m); break;
			case 't': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (int) timelib_days_in_month(t->y, t->m)); break;

			/* year */
			case 'L': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", timelib_is_leap((int) t->y)); break;
			case 'y': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%02d", (int) t->y % 100); break;
			case 'Y': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%s%04d", t->y < 0 ? "-" : "", abs((int) t->y)); break;

			/* time */
			case 'a': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%R", localized ? IS_UNICODE : IS_STRING, am_pm_lower_full(t->h >= 12 ? 1 : 0, localized)); break;
			case 'A': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%R", localized ? IS_UNICODE : IS_STRING, am_pm_upper_full(t->h >= 12 ? 1 : 0, localized)); break;
			case 'B': {
				int retval = (((((long)t->sse)-(((long)t->sse) - ((((long)t->sse) % 86400) + 3600))) * 10) / 864);
				while (retval < 0) {
					retval += 1000;
				}
				retval = retval % 1000;
				length = date_spprintf(&buffer, 32 TSRMLS_CC, "%03d", retval);
				break;
			}
			case 'g': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (t->h % 12) ? (int) t->h % 12 : 12); break;
			case 'G': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (int) t->h); break;
			case 'h': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%02d", (t->h % 12) ? (int) t->h % 12 : 12); break;
			case 'H': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%02d", (int) t->h); break;
			case 'i': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%02d", (int) t->i); break;
			case 's': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%02d", (int) t->s); break;
			case 'u': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%06d", (int) floor(t->f * 1000000)); break;

			/* timezone */
			case 'I': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", localtime ? offset->is_dst : 0); break;
 			case 'P': rfc_colon = 1; /* break intentionally missing */
			case 'O': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%c%02d%s%02d",
											localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
											localtime ? abs(offset->offset / 3600) : 0,
 											rfc_colon ? ":" : "",
											localtime ? abs((offset->offset % 3600) / 60) : 0
							  );
					  break;
			case 'T': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%s", localtime ? offset->abbr : "GMT"); break;
			case 'e': if (!localtime) {
					      length = date_spprintf(&buffer, 32 TSRMLS_CC, "%s", "UTC");
					  } else {
						  switch (t->zone_type) {
							  case TIMELIB_ZONETYPE_ID:
								  length = date_spprintf(&buffer, 32 TSRMLS_CC, "%s", t->tz_info->name);
								  break;
							  case TIMELIB_ZONETYPE_ABBR:
								  length = date_spprintf(&buffer, 32 TSRMLS_CC, "%s", offset->abbr);
								  break;
							  case TIMELIB_ZONETYPE_OFFSET:
								  length = date_spprintf(&buffer, 32 TSRMLS_CC, "%c%02d:%02d",
												((offset->offset < 0) ? '-' : '+'),
												abs(offset->offset / 3600),
												abs((offset->offset % 3600) / 60)
										   );
								  break;
						  }
					  }
					  break;
			case 'Z': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", localtime ? offset->offset : 0); break;

			/* full date/time */
			case 'c': length = date_spprintf(&buffer, 96 TSRMLS_CC, "%04d-%02d-%02dT%02d:%02d:%02d%c%02d:%02d",
							                (int) t->y, (int) t->m, (int) t->d,
											(int) t->h, (int) t->i, (int) t->s,
											localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
											localtime ? abs(offset->offset / 3600) : 0,
											localtime ? abs((offset->offset % 3600) / 60) : 0
							  );
					  break;
			case 'r': length = date_spprintf(&buffer, 96 TSRMLS_CC, "%3s, %02d %3s %04d %02d:%02d:%02d %c%02d%02d",
							                php_date_short_day_name(t->y, t->m, t->d),
											(int) t->d, mon_short_names[t->m - 1],
											(int) t->y, (int) t->h, (int) t->i, (int) t->s,
											localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
											localtime ? abs(offset->offset / 3600) : 0,
											localtime ? abs((offset->offset % 3600) / 60) : 0
							  );
					  break;
			case 'U': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%lld", (timelib_sll) t->sse); break;

			case '\\': if (i < format_len) i++; length = date_spprintf(&buffer, 32 TSRMLS_CC, "%c", format[i]); break;

			default: length = date_spprintf(&buffer, 32 TSRMLS_CC, "%c", format[i]);
					 break;
		}
		smart_str_appendl(&string, buffer, length);
		if (!no_free) {
			efree(buffer);
		}
	}

	smart_str_0(&string);
	date_free_locale_data(loc_dat);

	if (localtime) {
		timelib_time_offset_dtor(offset);
	}

	if (UG(unicode)) {
		*return_len = string.len / 2;
	} else {
		*return_len = string.len;
	}
	return string.c;
}

static void php_date(INTERNAL_FUNCTION_PARAMETERS, int localtime)
{
	char   *format;
	int     format_len;
	long    ts;
	char   *string;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &format, &format_len, &ts) == FAILURE) {
		RETURN_FALSE;
	}
	if (ZEND_NUM_ARGS() == 1) {
		ts = time(NULL);
	}

	string = php_format_date(format, format_len, ts, localtime TSRMLS_CC);

	if (UG(unicode)) {
		RETVAL_UNICODE((UChar*)string, 0);
	} else {
		RETVAL_STRING(string, 0);
	}
}
/* }}} */

PHPAPI char *php_format_date(char *format, int format_len, time_t ts, int localtime TSRMLS_DC) /* {{{ */
{
	timelib_time   *t;
	timelib_tzinfo *tzi;
	char           *string;
	int             return_len;

	t = timelib_time_ctor();

	if (localtime) {
		tzi = get_timezone_info(TSRMLS_C);
		t->tz_info = tzi;
		t->zone_type = TIMELIB_ZONETYPE_ID;
		timelib_unixtime2local(t, ts);
	} else {
		tzi = NULL;
		timelib_unixtime2gmt(t, ts);
	}

	string = date_format(format, format_len, &return_len, t, localtime, 0 TSRMLS_CC);
	
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
		TSRMLS_FETCH();
		tzi = get_timezone_info(TSRMLS_C);
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
			offset->offset = (t->z - (t->dst * 60)) * -60;
			offset->leap_secs = 0;
			offset->is_dst = t->dst;
			offset->abbr = strdup(t->tz_abbr);
		} else if (t->zone_type == TIMELIB_ZONETYPE_OFFSET) {
			offset = timelib_time_offset_ctor();
			offset->offset = (t->z - (t->dst * 60)) * -60;
			offset->leap_secs = 0;
			offset->is_dst = t->dst;
			offset->abbr = malloc(9); /* GMT±xxxx\0 */
			snprintf(offset->abbr, 9, "GMT%c%02d%02d", 
			                          !localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
			                          !localtime ? abs(offset->offset / 3600) : 0,
			                          !localtime ? abs((offset->offset % 3600) / 60) : 0 );
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
			retval = (((((long)t->sse)-(((long)t->sse) - ((((long)t->sse) % 86400) + 3600))) * 10) / 864);			
			while (retval < 0) {
				retval += 1000;
			}
			retval = retval % 1000;
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

/* {{{ proto string date(string format [, long timestamp]) U
   Format a local date/time */
PHP_FUNCTION(date)
{
	php_date(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto string gmdate(string format [, long timestamp]) U
   Format a GMT date/time */
PHP_FUNCTION(gmdate)
{
	php_date(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int idate(string format [, int timestamp]) U
   Format a local time/date as integer */
PHP_FUNCTION(idate)
{
	char   *format;
	int     format_len;
	long    ts = 0;
	int ret; 

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &format, &format_len, &ts) == FAILURE) {
		RETURN_FALSE;
	}

	if (format_len != 1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "idate format is one char");
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() == 1) {
		ts = time(NULL);
	}

	ret = php_idate(format[0], ts, 0);
	if (ret == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unrecognized date format token");
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

/* {{{ php_parse_date: Backwards compability function */
PHPAPI signed long php_parse_date(char *string, signed long *now)
{
	timelib_time *parsed_time;
	int           error2;
	signed long   retval;

	parsed_time = timelib_strtotime(string, strlen(string), NULL, DATE_TIMEZONEDB);
	timelib_update_ts(parsed_time, NULL);
	retval = timelib_date_to_int(parsed_time, &error2);
	timelib_time_dtor(parsed_time);
	if (error2) {
		return -1;
	}
	return retval;
}
/* }}} */


/* {{{ proto int strtotime(string time [, int now ]) U
   Convert string representation of date and time to a timestamp */
PHP_FUNCTION(strtotime)
{
	char *times, *initial_ts;
	int   time_len, error1, error2;
	struct timelib_error_container *error;
	long  preset_ts = 0, ts;

	timelib_time *t, *now;
	timelib_tzinfo *tzi;

	tzi = get_timezone_info(TSRMLS_C);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sl", &times, &time_len, &preset_ts) != FAILURE) {
		/* We have an initial timestamp */
		now = timelib_time_ctor();

		initial_ts = emalloc(25);
		snprintf(initial_ts, 24, "@%ld UTC", preset_ts);
		t = timelib_strtotime(initial_ts, strlen(initial_ts), NULL, DATE_TIMEZONEDB); /* we ignore the error here, as this should never fail */
		timelib_update_ts(t, tzi);
		now->tz_info = tzi;
		now->zone_type = TIMELIB_ZONETYPE_ID;
		timelib_unixtime2local(now, t->sse);
		timelib_time_dtor(t);
		efree(initial_ts);
	} else if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &times, &time_len, &preset_ts) != FAILURE) {
		/* We have no initial timestamp */
		now = timelib_time_ctor();
		now->tz_info = tzi;
		now->zone_type = TIMELIB_ZONETYPE_ID;
		timelib_unixtime2local(now, (timelib_sll) time(NULL));
	} else {
		RETURN_FALSE;
	}

	if (!time_len) {
		timelib_time_dtor(now);	
		RETURN_FALSE;
	}

	t = timelib_strtotime(times, time_len, &error, DATE_TIMEZONEDB);
	error1 = error->error_count;
	timelib_error_container_dtor(error);
	timelib_fill_holes(t, now, 0);
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
	long hou = 0, min = 0, sec = 0, mon = 0, day = 0, yea = 0, dst = -1;
	timelib_time *now;
	timelib_tzinfo *tzi = NULL;
	long ts, adjust_seconds = 0;
	int error;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|lllllll", &hou, &min, &sec, &mon, &day, &yea, &dst) == FAILURE) {
		RETURN_FALSE;
	}
	/* Initialize structure with current time */
	now = timelib_time_ctor();
	if (gmt) {
		timelib_unixtime2gmt(now, (timelib_sll) time(NULL));
	} else {
		tzi = get_timezone_info(TSRMLS_C);
		now->tz_info = tzi;
		now->zone_type = TIMELIB_ZONETYPE_ID;
		timelib_unixtime2local(now, (timelib_sll) time(NULL));
	}
	/* Fill in the new data */
	switch (ZEND_NUM_ARGS()) {
		case 7:
			/* break intentionally missing */
		case 6:
			if (yea >= 0 && yea < 70) {
				yea += 2000;
			} else if (yea >= 70 && yea <= 110) {
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
			php_error_docref(NULL TSRMLS_CC, E_STRICT, "You should be using the time() function instead");
	}
	/* Update the timestamp */
	if (gmt) {
		timelib_update_ts(now, NULL);
	} else {
		timelib_update_ts(now, tzi);
	}
	/* Support for the deprecated is_dst parameter */
	if (dst != -1) {
		php_error_docref(NULL TSRMLS_CC, E_DEPRECATED, "The is_dst parameter is deprecated");
		if (gmt) {
			/* GMT never uses DST */
			if (dst == 1) {
				adjust_seconds = -3600;
			}
		} else {
			/* Figure out is_dst for current TS */
			timelib_time_offset *tmp_offset;
			tmp_offset = timelib_get_time_zone_info(now->sse, tzi);
			if (dst == 1 && tmp_offset->is_dst == 0) {
				adjust_seconds = -3600;
			}
			if (dst == 0 && tmp_offset->is_dst == 1) {
				adjust_seconds = +3600;
			}
			timelib_time_offset_dtor(tmp_offset);
		}
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

/* {{{ proto int mktime([int hour [, int min [, int sec [, int mon [, int day [, int year]]]]]]) U
   Get UNIX timestamp for a date */
PHP_FUNCTION(mktime)
{
	php_mktime(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int gmmktime([int hour [, int min [, int sec [, int mon [, int day [, int year]]]]]]) U
   Get UNIX timestamp for a GMT date */
PHP_FUNCTION(gmmktime)
{
	php_mktime(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */


/* {{{ proto bool checkdate(int month, int day, int year) U
   Returns true(1) if it is a valid date in gregorian calendar */
PHP_FUNCTION(checkdate)
{
	long m, d, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &m, &d, &y) == FAILURE) {
		RETURN_FALSE;
	}

 	if (y < 1 || y > 32767 || !timelib_valid_date(y, m, d)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;	/* True : This month, day, year arguments are valid */
}
/* }}} */

#ifdef HAVE_STRFTIME
/* {{{ php_strftime - (gm)strftime helper */
PHPAPI void php_strftime(INTERNAL_FUNCTION_PARAMETERS, int gmt)
{
	char                *format, *buf;
	int                  format_len;
	long                 timestamp = 0;
	struct tm            ta;
	int                  max_reallocs = 5;
	size_t               buf_len = 64, real_len;
	timelib_time        *ts;
	timelib_tzinfo      *tzi;
	timelib_time_offset *offset = NULL;

	timestamp = (long) time(NULL);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &format, &format_len, &timestamp) == FAILURE) {
		RETURN_FALSE;
	}

	if (format_len == 0) {
		RETURN_FALSE;
	}

	ts = timelib_time_ctor();
	if (gmt) {
		tzi = NULL;
		timelib_unixtime2gmt(ts, (timelib_sll) timestamp);
	} else {
		tzi = get_timezone_info(TSRMLS_C);
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
#if HAVE_TM_GMTOFF
		ta.tm_gmtoff = 0;
#endif
#if HAVE_TM_ZONE
		ta.tm_zone = "GMT";
#endif
	} else {
		offset = timelib_get_time_zone_info(timestamp, tzi);

		ta.tm_isdst = offset->is_dst;
#if HAVE_TM_GMTOFF
		ta.tm_gmtoff = offset->offset;
#endif
#if HAVE_TM_ZONE
		ta.tm_zone = offset->abbr;
#endif
	}

	buf = (char *) emalloc(buf_len);
	while ((real_len=strftime(buf, buf_len, format, &ta))==buf_len || real_len==0) {
		buf_len *= 2;
		buf = (char *) erealloc(buf, buf_len);
		if (!--max_reallocs) {
			break;
		}
	}

	timelib_time_dtor(ts);
	if (!gmt) {
		timelib_time_offset_dtor(offset);
	}

	if (real_len && real_len != buf_len) {
		buf = (char *) erealloc(buf, real_len + 1);
		if (UG(unicode)) {
			RETVAL_RT_STRINGL(buf, real_len, 1);
			efree(buf);
			return;
		} else {
			RETURN_STRINGL(buf, real_len, 0);
		}
	}
	efree(buf);
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string strftime(string format [, int timestamp]) U
   Format a local time/date according to locale settings */
PHP_FUNCTION(strftime)
{
	php_strftime(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string gmstrftime(string format [, int timestamp]) U
   Format a GMT/UCT time/date according to locale settings */
PHP_FUNCTION(gmstrftime)
{
	php_strftime(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */
#endif

/* {{{ proto int time(void) U
   Return current UNIX timestamp */
PHP_FUNCTION(time)
{
	RETURN_LONG((long)time(NULL));
}
/* }}} */

/* {{{ proto array localtime([int timestamp [, bool associative_array]]) U
   Returns the results of the C system call localtime as an associative array if the associative_array argument is set to 1 other wise it is a regular array */
PHP_FUNCTION(localtime)
{
	long timestamp = (long)time(NULL);
	zend_bool associative = 0;
	timelib_tzinfo *tzi;
	timelib_time   *ts;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|lb", &timestamp, &associative) == FAILURE) {
		RETURN_FALSE;
	}

	tzi = get_timezone_info(TSRMLS_C);
	ts = timelib_time_ctor();
	ts->tz_info = tzi;
	ts->zone_type = TIMELIB_ZONETYPE_ID;
	timelib_unixtime2local(ts, (timelib_sll) timestamp);

	array_init(return_value);

	if (associative) {
		add_ascii_assoc_long(return_value, "tm_sec",   ts->s);
		add_ascii_assoc_long(return_value, "tm_min",   ts->i);
		add_ascii_assoc_long(return_value, "tm_hour",  ts->h);
		add_ascii_assoc_long(return_value, "tm_mday",  ts->d);
		add_ascii_assoc_long(return_value, "tm_mon",   ts->m - 1);
		add_ascii_assoc_long(return_value, "tm_year",  ts->y - 1900);
		add_ascii_assoc_long(return_value, "tm_wday",  timelib_day_of_week(ts->y, ts->m, ts->d));
		add_ascii_assoc_long(return_value, "tm_yday",  timelib_day_of_year(ts->y, ts->m, ts->d));
		add_ascii_assoc_long(return_value, "tm_isdst", ts->dst);
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

/* {{{ proto array getdate([int timestamp]) U
   Get date/time information */
PHP_FUNCTION(getdate)
{
	long timestamp = (long)time(NULL);
	timelib_tzinfo *tzi;
	timelib_time   *ts;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &timestamp) == FAILURE) {
		RETURN_FALSE;
	}

	tzi = get_timezone_info(TSRMLS_C);
	ts = timelib_time_ctor();
	ts->tz_info = tzi;
	ts->zone_type = TIMELIB_ZONETYPE_ID;
	timelib_unixtime2local(ts, (timelib_sll) timestamp);

	array_init(return_value);

	add_ascii_assoc_long(return_value, "seconds", ts->s);
	add_ascii_assoc_long(return_value, "minutes", ts->i);
	add_ascii_assoc_long(return_value, "hours", ts->h);
	add_ascii_assoc_long(return_value, "mday", ts->d);
	add_ascii_assoc_long(return_value, "wday", timelib_day_of_week(ts->y, ts->m, ts->d));
	add_ascii_assoc_long(return_value, "mon", ts->m);
	add_ascii_assoc_long(return_value, "year", ts->y);
	add_ascii_assoc_long(return_value, "yday", timelib_day_of_year(ts->y, ts->m, ts->d));
	add_ascii_assoc_ascii_string(return_value, "weekday", php_date_full_day_name(ts->y, ts->m, ts->d), 1);
	add_ascii_assoc_ascii_string(return_value, "month", mon_full_names[ts->m - 1], 1);
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
	zval                 *date_period_zval;
	zval                 *current;
	php_period_obj       *object;
	int                   current_index;
} date_period_it;

/* {{{ date_period_it_invalidate_current */
static void date_period_it_invalidate_current(zend_object_iterator *iter TSRMLS_DC)
{
	date_period_it *iterator = (date_period_it *)iter;

	if (iterator->current) {
		zval_ptr_dtor(&iterator->current);
		iterator->current = NULL;
	}
}
/* }}} */


/* {{{ date_period_it_dtor */
static void date_period_it_dtor(zend_object_iterator *iter TSRMLS_DC)
{
	date_period_it *iterator = (date_period_it *)iter;

	date_period_it_invalidate_current(iter TSRMLS_CC);

	zval_ptr_dtor(&iterator->date_period_zval);

	efree(iterator);
}
/* }}} */


/* {{{ date_period_it_has_more */
static int date_period_it_has_more(zend_object_iterator *iter TSRMLS_DC)
{
	date_period_it *iterator = (date_period_it *)iter;
	php_period_obj *object   = iterator->object;
	timelib_time   *it_time = object->start;

	/* apply modification if it's not the first iteration */
	if (!object->include_start_date || iterator->current_index > 0) {
		it_time->have_relative = 1;
		it_time->relative = *object->interval;
		it_time->sse_uptodate = 0;
		timelib_update_ts(it_time, NULL);
		timelib_update_from_sse(it_time);
	}

	if (object->end) {
		return object->start->sse < object->end->sse ? SUCCESS : FAILURE;
	} else {
		return (iterator->current_index < object->recurrences) ? SUCCESS : FAILURE;
	}
}
/* }}} */


/* {{{ date_period_it_current_data */
static void date_period_it_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC)
{
	date_period_it *iterator = (date_period_it *)iter;
	php_period_obj *object   = iterator->object;
	timelib_time   *it_time = object->start;
	php_date_obj   *newdateobj;

	/* Create new object */
	MAKE_STD_ZVAL(iterator->current);
	date_instantiate(date_ce_date, iterator->current TSRMLS_CC);
	newdateobj = (php_date_obj *) zend_object_store_get_object(iterator->current TSRMLS_CC);
	newdateobj->time = timelib_time_ctor();
	*newdateobj->time = *it_time;
	if (it_time->tz_abbr) {
		newdateobj->time->tz_abbr = strdup(it_time->tz_abbr);
	}
	if (it_time->tz_info) {
		newdateobj->time->tz_info = it_time->tz_info;
	}
	
	*data = &iterator->current;
}
/* }}} */


/* {{{ date_period_it_current_key */
static int date_period_it_current_key(zend_object_iterator *iter, zstr *str_key, uint *str_key_len, ulong *int_key TSRMLS_DC)
{
	date_period_it   *iterator = (date_period_it *)iter;
	*int_key = iterator->current_index;
	return HASH_KEY_IS_LONG;
}
/* }}} */


/* {{{ date_period_it_move_forward */
static void date_period_it_move_forward(zend_object_iterator *iter TSRMLS_DC)
{
	date_period_it   *iterator = (date_period_it *)iter;

	iterator->current_index++;
	date_period_it_invalidate_current(iter TSRMLS_CC);
}
/* }}} */


/* {{{ date_period_it_rewind */
static void date_period_it_rewind(zend_object_iterator *iter TSRMLS_DC)
{
	date_period_it   *iterator = (date_period_it *)iter;

	iterator->current_index = 0;
	date_period_it_invalidate_current(iter TSRMLS_CC);
}
/* }}} */


/* iterator handler table */
zend_object_iterator_funcs date_period_it_funcs = {
	date_period_it_dtor,
	date_period_it_has_more,
	date_period_it_current_data,
	date_period_it_current_key,
	date_period_it_move_forward,
	date_period_it_rewind,
	date_period_it_invalidate_current
};



zend_object_iterator *date_object_period_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC)
{
	date_period_it  *iterator = emalloc(sizeof(date_period_it));
	php_period_obj  *dpobj    = (php_period_obj *)zend_object_store_get_object(object TSRMLS_CC);

	if (by_ref) {
		zend_error(E_ERROR, "An iterator cannot be used with foreach by reference");
	}

	Z_ADDREF_P(object);
	iterator->intern.data = (void*) dpobj;
	iterator->intern.funcs = &date_period_it_funcs;
	iterator->date_period_zval = object;
	iterator->object = dpobj;
	iterator->current = NULL;

	return (zend_object_iterator*)iterator;
}

static void date_register_classes(TSRMLS_D)
{
	zend_class_entry ce_date, ce_timezone, ce_interval, ce_period;

	INIT_CLASS_ENTRY(ce_date, "DateTime", date_funcs_date);
	ce_date.create_object = date_object_new_date;
	date_ce_date = zend_register_internal_class_ex(&ce_date, NULL, NULL TSRMLS_CC);
	memcpy(&date_object_handlers_date, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	date_object_handlers_date.clone_obj = date_object_clone_date;
	date_object_handlers_date.compare_objects = date_object_compare_date;
	date_object_handlers_date.get_properties = date_object_get_properties;

#define REGISTER_DATE_CLASS_CONST_STRING(const_name, value) \
	zend_declare_class_constant_stringl(date_ce_date, const_name, sizeof(const_name)-1, value, sizeof(value)-1 TSRMLS_CC);

	REGISTER_DATE_CLASS_CONST_STRING("ATOM",    DATE_FORMAT_RFC3339);
	REGISTER_DATE_CLASS_CONST_STRING("COOKIE",  DATE_FORMAT_RFC850);
	REGISTER_DATE_CLASS_CONST_STRING("ISO8601", DATE_FORMAT_ISO8601);
	REGISTER_DATE_CLASS_CONST_STRING("RFC822",  DATE_FORMAT_RFC822);
	REGISTER_DATE_CLASS_CONST_STRING("RFC850",  DATE_FORMAT_RFC850);
	REGISTER_DATE_CLASS_CONST_STRING("RFC1036", DATE_FORMAT_RFC1036);
	REGISTER_DATE_CLASS_CONST_STRING("RFC1123", DATE_FORMAT_RFC1123);
	REGISTER_DATE_CLASS_CONST_STRING("RFC2822", DATE_FORMAT_RFC2822);
	REGISTER_DATE_CLASS_CONST_STRING("RFC3339", DATE_FORMAT_RFC3339);
	REGISTER_DATE_CLASS_CONST_STRING("RSS",     DATE_FORMAT_RFC1123);
	REGISTER_DATE_CLASS_CONST_STRING("W3C",     DATE_FORMAT_RFC3339);


	INIT_CLASS_ENTRY(ce_timezone, "DateTimeZone", date_funcs_timezone);
	ce_timezone.create_object = date_object_new_timezone;
	date_ce_timezone = zend_register_internal_class_ex(&ce_timezone, NULL, NULL TSRMLS_CC);
	memcpy(&date_object_handlers_timezone, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	date_object_handlers_timezone.clone_obj = date_object_clone_timezone;

#define REGISTER_TIMEZONE_CLASS_CONST_STRING(const_name, value) \
	zend_declare_class_constant_long(date_ce_timezone, const_name, sizeof(const_name)-1, value TSRMLS_CC);

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
	date_ce_interval = zend_register_internal_class_ex(&ce_interval, NULL, NULL TSRMLS_CC);
	memcpy(&date_object_handlers_interval, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	date_object_handlers_interval.clone_obj = date_object_clone_interval;
	date_object_handlers_interval.read_property = date_interval_read_property;
	date_object_handlers_interval.write_property = date_interval_write_property;

	INIT_CLASS_ENTRY(ce_period, "DatePeriod", date_funcs_period);
	ce_period.create_object = date_object_new_period;
	date_ce_period = zend_register_internal_class_ex(&ce_period, NULL, NULL TSRMLS_CC);
	date_ce_period->get_iterator = date_object_period_get_iterator;
	date_ce_period->iterator_funcs.funcs = &date_period_it_funcs;
	zend_class_implements(date_ce_period TSRMLS_CC, 1, zend_ce_traversable);
	memcpy(&date_object_handlers_period, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	date_object_handlers_period.clone_obj = date_object_clone_period;

#define REGISTER_PERIOD_CLASS_CONST_STRING(const_name, value) \
	zend_declare_class_constant_long(date_ce_period, const_name, sizeof(const_name)-1, value TSRMLS_CC);

	REGISTER_PERIOD_CLASS_CONST_STRING("EXCLUDE_START_DATE", PHP_DATE_PERIOD_EXCLUDE_START_DATE);
}

static inline zend_object_value date_object_new_date_ex(zend_class_entry *class_type, php_date_obj **ptr TSRMLS_DC)
{
	php_date_obj *intern;
	zend_object_value retval;
	zval *tmp;

	intern = emalloc(sizeof(php_date_obj));
	memset(intern, 0, sizeof(php_date_obj));
	if (ptr) {
		*ptr = intern;
	}
	
	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	
	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) date_object_free_storage_date, NULL TSRMLS_CC);
	retval.handlers = &date_object_handlers_date;
	
	return retval;
}

static zend_object_value date_object_new_date(zend_class_entry *class_type TSRMLS_DC)
{
	return date_object_new_date_ex(class_type, NULL TSRMLS_CC);
}

static zend_object_value date_object_clone_date(zval *this_ptr TSRMLS_DC)
{
	php_date_obj *new_obj = NULL;
	php_date_obj *old_obj = (php_date_obj *) zend_object_store_get_object(this_ptr TSRMLS_CC);
	zend_object_value new_ov = date_object_new_date_ex(old_obj->std.ce, &new_obj TSRMLS_CC);
	
	zend_objects_clone_members(&new_obj->std, new_ov, &old_obj->std, Z_OBJ_HANDLE_P(this_ptr) TSRMLS_CC);
	
	/* this should probably moved to a new `timelib_time *timelime_time_clone(timelib_time *)` */
	new_obj->time = timelib_time_ctor();
	*new_obj->time = *old_obj->time;
	if (old_obj->time->tz_abbr) {
		new_obj->time->tz_abbr = strdup(old_obj->time->tz_abbr);
	}
	if (old_obj->time->tz_info) {
		new_obj->time->tz_info = old_obj->time->tz_info;
	}
	
	return new_ov;
}

static int date_object_compare_date(zval *d1, zval *d2 TSRMLS_DC)
{
	if (Z_TYPE_P(d1) == IS_OBJECT && Z_TYPE_P(d2) == IS_OBJECT &&
		instanceof_function(Z_OBJCE_P(d1), date_ce_date TSRMLS_CC) &&
		instanceof_function(Z_OBJCE_P(d2), date_ce_date TSRMLS_CC)) {
		php_date_obj *o1 = zend_object_store_get_object(d1 TSRMLS_CC);
		php_date_obj *o2 = zend_object_store_get_object(d2 TSRMLS_CC);
		
		if (!o1->time->sse_uptodate) {
			timelib_update_ts(o1->time, o1->time->tz_info);
		}
		if (!o2->time->sse_uptodate) {
			timelib_update_ts(o2->time, o2->time->tz_info);
		}
		
		return (o1->time->sse == o2->time->sse) ? 0 : ((o1->time->sse < o2->time->sse) ? -1 : 1);
	}
	
	return 1;
}

static HashTable *date_object_get_properties(zval *object TSRMLS_DC)
{
	HashTable    *props;
	zval         *zv;
	php_date_obj *dateobj;
	char         *str;
	int           return_len;

	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);

	props = dateobj->std.properties;

	if (!dateobj->time) {
		return props;
	}

	/* first we add the date and time in ISO format */
	str = date_format("Y-m-d H:i:s", 12, &return_len, dateobj->time, 1, 0 TSRMLS_CC);
	MAKE_STD_ZVAL(zv);
	if (UG(unicode)) {
		ZVAL_UNICODEL(zv, (UChar*) str, return_len - 1, 0);
	} else {
		ZVAL_STRINGL(zv, str, return_len - 1, 0);
	}
	zend_hash_update(props, "date", 5, &zv, sizeof(zval), NULL);

	/* then we add the timezone name (or similar) */
	if (dateobj->time->is_localtime) {
		MAKE_STD_ZVAL(zv);
		ZVAL_LONG(zv, dateobj->time->zone_type);
		zend_hash_update(props, "timezone_type", 14, &zv, sizeof(zval), NULL);

		MAKE_STD_ZVAL(zv);
		switch (dateobj->time->zone_type) {
			case TIMELIB_ZONETYPE_ID:
				ZVAL_ASCII_STRING(zv, dateobj->time->tz_info->name, 1);
				break;
			case TIMELIB_ZONETYPE_OFFSET: {
				char *tmpstr = emalloc(sizeof("UTC+05:00"));
				timelib_sll utc_offset = dateobj->time->z;

				snprintf(tmpstr, sizeof("+05:00"), "%c%02d:%02d",
					utc_offset > 0 ? '-' : '+',
					abs(utc_offset / 60),
					abs((utc_offset % 60)));

				ZVAL_ASCII_STRING(zv, tmpstr, ZSTR_AUTOFREE);
				}
				break;
			case TIMELIB_ZONETYPE_ABBR:
				ZVAL_ASCII_STRING(zv, dateobj->time->tz_abbr, 1);
				break;
		}
		zend_hash_update(props, "timezone", 9, &zv, sizeof(zval), NULL);
	}

	return props;
}

static inline zend_object_value date_object_new_timezone_ex(zend_class_entry *class_type, php_timezone_obj **ptr TSRMLS_DC)
{
	php_timezone_obj *intern;
	zend_object_value retval;
	zval *tmp;

	intern = emalloc(sizeof(php_timezone_obj));
	memset(intern, 0, sizeof(php_timezone_obj));
	if (ptr) {
		*ptr = intern;
	}

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	
	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) date_object_free_storage_timezone, NULL TSRMLS_CC);
	retval.handlers = &date_object_handlers_timezone;
	
	return retval;
}

static zend_object_value date_object_new_timezone(zend_class_entry *class_type TSRMLS_DC)
{
	return date_object_new_timezone_ex(class_type, NULL TSRMLS_CC);
}

static zend_object_value date_object_clone_timezone(zval *this_ptr TSRMLS_DC)
{
	php_timezone_obj *new_obj = NULL;
	php_timezone_obj *old_obj = (php_timezone_obj *) zend_object_store_get_object(this_ptr TSRMLS_CC);
	zend_object_value new_ov = date_object_new_timezone_ex(old_obj->std.ce, &new_obj TSRMLS_CC);
	
	zend_objects_clone_members(&new_obj->std, new_ov, &old_obj->std, Z_OBJ_HANDLE_P(this_ptr) TSRMLS_CC);
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
			new_obj->tzi.z.abbr       = old_obj->tzi.z.abbr;
			break;
	}
	
	return new_ov;
}

static inline zend_object_value date_object_new_interval_ex(zend_class_entry *class_type, php_interval_obj **ptr TSRMLS_DC)
{
	php_interval_obj *intern;
	zend_object_value retval;
	zval *tmp;

	intern = emalloc(sizeof(php_interval_obj));
	memset(intern, 0, sizeof(php_interval_obj));
	if (ptr) {
		*ptr = intern;
	}

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	
	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) date_object_free_storage_interval, NULL TSRMLS_CC);
	retval.handlers = &date_object_handlers_interval;
	
	return retval;
}

static zend_object_value date_object_new_interval(zend_class_entry *class_type TSRMLS_DC)
{
	return date_object_new_interval_ex(class_type, NULL TSRMLS_CC);
}

static zend_object_value date_object_clone_interval(zval *this_ptr TSRMLS_DC)
{
	php_interval_obj *new_obj = NULL;
	php_interval_obj *old_obj = (php_interval_obj *) zend_object_store_get_object(this_ptr TSRMLS_CC);
	zend_object_value new_ov = date_object_new_interval_ex(old_obj->std.ce, &new_obj TSRMLS_CC);
	
	zend_objects_clone_members(&new_obj->std, new_ov, &old_obj->std, Z_OBJ_HANDLE_P(this_ptr) TSRMLS_CC);

	/** FIX ME ADD CLONE STUFF **/
	return new_ov;
}

static inline zend_object_value date_object_new_period_ex(zend_class_entry *class_type, php_period_obj **ptr TSRMLS_DC)
{
	php_period_obj *intern;
	zend_object_value retval;
	zval *tmp;

	intern = emalloc(sizeof(php_period_obj));
	memset(intern, 0, sizeof(php_period_obj));
	if (ptr) {
		*ptr = intern;
	}

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	
	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) date_object_free_storage_period, NULL TSRMLS_CC);
	retval.handlers = &date_object_handlers_period;
	
	return retval;
}

static zend_object_value date_object_new_period(zend_class_entry *class_type TSRMLS_DC)
{
	return date_object_new_period_ex(class_type, NULL TSRMLS_CC);
}

static zend_object_value date_object_clone_period(zval *this_ptr TSRMLS_DC)
{
	php_period_obj *new_obj = NULL;
	php_period_obj *old_obj = (php_period_obj *) zend_object_store_get_object(this_ptr TSRMLS_CC);
	zend_object_value new_ov = date_object_new_period_ex(old_obj->std.ce, &new_obj TSRMLS_CC);
	
	zend_objects_clone_members(&new_obj->std, new_ov, &old_obj->std, Z_OBJ_HANDLE_P(this_ptr) TSRMLS_CC);

	/** FIX ME ADD CLONE STUFF **/
	return new_ov;
}

static void date_object_free_storage_date(void *object TSRMLS_DC)
{
	php_date_obj *intern = (php_date_obj *)object;

	if (intern->time) {
		timelib_time_dtor(intern->time);
	}

	zend_object_std_dtor(&intern->std TSRMLS_CC);
	efree(object);
}

static void date_object_free_storage_timezone(void *object TSRMLS_DC)
{
	php_timezone_obj *intern = (php_timezone_obj *)object;

	if (intern->type == TIMELIB_ZONETYPE_ABBR) {
		free(intern->tzi.z.abbr);
	}
	zend_object_std_dtor(&intern->std TSRMLS_CC);
	efree(object);
}

static void date_object_free_storage_interval(void *object TSRMLS_DC)
{
	php_interval_obj *intern = (php_interval_obj *)object;

	timelib_rel_time_dtor(intern->diff);
	zend_object_std_dtor(&intern->std TSRMLS_CC);
	efree(object);
}

static void date_object_free_storage_period(void *object TSRMLS_DC)
{
	php_period_obj *intern = (php_period_obj *)object;

	if (intern->start) {
		timelib_time_dtor(intern->start);
	}

	if (intern->end) {
		timelib_time_dtor(intern->end);
	}

	timelib_rel_time_dtor(intern->interval);
	zend_object_std_dtor(&intern->std TSRMLS_CC);
	efree(object);
}

/* Advanced Interface */
static zval * date_instantiate(zend_class_entry *pce, zval *object TSRMLS_DC)
{
	if (!object) {
		ALLOC_ZVAL(object);
	}

	Z_TYPE_P(object) = IS_OBJECT;
	object_init_ex(object, pce);
	Z_SET_REFCOUNT_P(object, 1);
	Z_UNSET_ISREF_P(object);
	return object;
}

/* Helper function used to store the latest found warnings and errors while
 * parsing, from either strtotime or parse_from_format. */
static void update_errors_warnings(timelib_error_container *last_errors TSRMLS_DC)
{
	if (DATEG(last_errors)) {
		timelib_error_container_dtor(DATEG(last_errors));
		DATEG(last_errors) = NULL;
	}
	DATEG(last_errors) = last_errors;
}

static int date_initialize(php_date_obj *dateobj, /*const*/ char *time_str, int time_str_len, char *format, zval *timezone_object, int ctor TSRMLS_DC)
{
	timelib_time   *now;
	timelib_tzinfo *tzi;
	timelib_error_container *err = NULL;
	int type = TIMELIB_ZONETYPE_ID, new_dst;
	char *new_abbr;
	timelib_sll     new_offset;
	
	if (dateobj->time) {
		timelib_time_dtor(dateobj->time);
	}
	if (format) {
		dateobj->time = timelib_parse_from_format(format, time_str_len ? time_str : "", time_str_len ? time_str_len : 0, &err, DATE_TIMEZONEDB);
	} else {
		dateobj->time = timelib_strtotime(time_str_len ? time_str : "now", time_str_len ? time_str_len : sizeof("now") -1, &err, DATE_TIMEZONEDB);
	}

	/* update last errors and warnings */
	update_errors_warnings(err TSRMLS_CC);


	if (ctor && err && err->error_count) {
		/* spit out the first library error message, at least */
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to parse time string (%s) at position %d (%c): %s", time_str,
			err->error_messages[0].position, err->error_messages[0].character, err->error_messages[0].message);
	}
	if (err && err->error_count) {
		return 0;
	}

	if (timezone_object) {
		php_timezone_obj *tzobj;

		tzobj = (php_timezone_obj *) zend_object_store_get_object(timezone_object TSRMLS_CC);
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
				new_abbr   = strdup(tzobj->tzi.z.abbr);
				break;
		}
		type = tzobj->type;
	} else if (dateobj->time->tz_info) {
		tzi = dateobj->time->tz_info;
	} else {
		tzi = get_timezone_info(TSRMLS_C);
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
	timelib_unixtime2local(now, (timelib_sll) time(NULL));

	timelib_fill_holes(dateobj->time, now, 0);
	timelib_update_ts(dateobj->time, tzi);

	dateobj->time->have_relative = 0;

	timelib_time_dtor(now);

	return 1;
}

/* {{{ proto DateTime date_create([string time[, DateTimeZone object]]) U
   Returns new DateTime object
*/
PHP_FUNCTION(date_create)
{
	zval           *timezone_object = NULL;
	char           *time_str = NULL;
	int             time_str_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sO", &time_str, &time_str_len, &timezone_object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}

	date_instantiate(date_ce_date, return_value TSRMLS_CC);
	if (!date_initialize(zend_object_store_get_object(return_value TSRMLS_CC), time_str, time_str_len, NULL, timezone_object, 0 TSRMLS_CC)) {
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
	int             time_str_len = 0, format_str_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|O", &format_str, &format_str_len, &time_str, &time_str_len, &timezone_object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}

	date_instantiate(date_ce_date, return_value TSRMLS_CC);
	if (!date_initialize(zend_object_store_get_object(return_value TSRMLS_CC), time_str, time_str_len, format_str, timezone_object, 0 TSRMLS_CC)) {
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
	int time_str_len = 0;
	
	zend_replace_error_handling(EH_THROW, NULL, NULL TSRMLS_CC);
	if (SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sO", &time_str, &time_str_len, &timezone_object, date_ce_timezone)) {
		date_initialize(zend_object_store_get_object(getThis() TSRMLS_CC), time_str, time_str_len, NULL, timezone_object, 1 TSRMLS_CC);
	}
}
/* }}} */

static int php_date_initialize_from_hash(zval **return_value, php_date_obj **dateobj, HashTable *myht TSRMLS_DC)
{
	zval            **z_date = NULL;
	zval            **z_timezone = NULL;
	zval            **z_timezone_type = NULL;
	zval             *tmp_obj = NULL;
	timelib_tzinfo   *tzi;
	php_timezone_obj *tzobj;

	if (zend_hash_find(myht, "date", 5, (void**) &z_date) == SUCCESS) {
		convert_to_string(*z_date);
		if (zend_hash_find(myht, "timezone_type", 14, (void**) &z_timezone_type) == SUCCESS) {
			convert_to_long(*z_timezone_type);
			if (zend_hash_find(myht, "timezone", 9, (void**) &z_timezone) == SUCCESS) {
				convert_to_string(*z_timezone);

				switch (Z_LVAL_PP(z_timezone_type)) {
					case TIMELIB_ZONETYPE_OFFSET:
					case TIMELIB_ZONETYPE_ABBR: {
						char *tmp = emalloc(Z_STRLEN_PP(z_date) + Z_STRLEN_PP(z_timezone) + 2);
						snprintf(tmp, Z_STRLEN_PP(z_date) + Z_STRLEN_PP(z_timezone) + 2, "%s %s", Z_STRVAL_PP(z_date), Z_STRVAL_PP(z_timezone));
						date_initialize(*dateobj, tmp, Z_STRLEN_PP(z_date) + Z_STRLEN_PP(z_timezone) + 1, NULL, NULL, 0 TSRMLS_CC);
						efree(tmp);
						return 1;
					}

					case TIMELIB_ZONETYPE_ID:
						convert_to_string(*z_timezone);

						tzi = php_date_parse_tzfile(Z_STRVAL_PP(z_timezone), DATE_TIMEZONEDB TSRMLS_CC);

						tzobj = zend_object_store_get_object(tmp_obj = date_instantiate(date_ce_timezone, tmp_obj TSRMLS_CC) TSRMLS_CC);
						tzobj->type = TIMELIB_ZONETYPE_ID;
						tzobj->tzi.tz = tzi;
						tzobj->initialized = 1;

						date_initialize(*dateobj, Z_STRVAL_PP(z_date), Z_STRLEN_PP(z_date), NULL, tmp_obj, 0 TSRMLS_CC);
						return 1;
				}
			}
		}
	}
	return 0;
}

/* {{{ proto DateTime::__set_state()
*/
PHP_METHOD(DateTime, __set_state)
{
	php_date_obj     *dateobj;
	zval             *array;
	HashTable        *myht;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &array) == FAILURE) {
		RETURN_FALSE;
	}

	myht = HASH_OF(array);

	date_instantiate(date_ce_date, return_value TSRMLS_CC);
	dateobj = (php_date_obj *) zend_object_store_get_object(return_value TSRMLS_CC);
	php_date_initialize_from_hash(&return_value, &dateobj, myht TSRMLS_CC);
}
/* }}} */

/* {{{ proto DateTime::__wakeup()
*/
PHP_METHOD(DateTime, __wakeup)
{
	zval             *object = getThis();
	php_date_obj     *dateobj;
	HashTable        *myht;

	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);

	myht = Z_OBJPROP_P(object);

	php_date_initialize_from_hash(&return_value, &dateobj, myht TSRMLS_CC);
}
/* }}} */

/* Helper function used to add an associative array of warnings and errors to a zval */
void zval_from_error_container(zval *z, timelib_error_container *error)
{
	int   i;
	zval *element;
	TSRMLS_FETCH();

	add_ascii_assoc_long(z, "warning_count", error->warning_count);
	MAKE_STD_ZVAL(element);
	array_init(element);
	for (i = 0; i < error->warning_count; i++) {
		add_index_ascii_string(element, error->warning_messages[i].position, error->warning_messages[i].message, 1);
	}
	add_ascii_assoc_zval(z, "warnings", element);

	add_ascii_assoc_long(z, "error_count", error->error_count);
	MAKE_STD_ZVAL(element);
	array_init(element);
	for (i = 0; i < error->error_count; i++) {
		add_index_ascii_string(element, error->error_messages[i].position, error->error_messages[i].message, 1);
	}
	add_ascii_assoc_zval(z, "errors", element);
}

/* {{{ proto array date_get_last_errors() U
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

void php_date_do_return_parsed_time(INTERNAL_FUNCTION_PARAMETERS, timelib_time *parsed_time, struct timelib_error_container *error)
{
	zval *element;

	array_init(return_value);
#define PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(name, elem) \
	if (parsed_time->elem == -99999) {               \
		add_ascii_assoc_bool(return_value, #name, 0); \
	} else {                                       \
		add_ascii_assoc_long(return_value, #name, parsed_time->elem); \
	}
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(year,      y);
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(month,     m);
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(day,       d);
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(hour,      h);
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(minute,    i);
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(second,    s);
	
	if (parsed_time->f == -99999) {
		add_ascii_assoc_bool(return_value, "fraction", 0);
	} else {
		add_ascii_assoc_double(return_value, "fraction", parsed_time->f);
	}

	zval_from_error_container(return_value, error);

	timelib_error_container_dtor(error);

	add_ascii_assoc_bool(return_value, "is_localtime", parsed_time->is_localtime);

	if (parsed_time->is_localtime) {
		PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(zone_type, zone_type);
		switch (parsed_time->zone_type) {
			case TIMELIB_ZONETYPE_OFFSET:
				PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(zone, z);
				add_ascii_assoc_bool(return_value, "is_dst", parsed_time->dst);
				break;
			case TIMELIB_ZONETYPE_ID:
				if (parsed_time->tz_abbr) {
					add_ascii_assoc_ascii_string(return_value, "tz_abbr", parsed_time->tz_abbr, 1);
				}
				if (parsed_time->tz_info) {
					add_ascii_assoc_ascii_string(return_value, "tz_id", parsed_time->tz_info->name, 1);
				}
				break;
			case TIMELIB_ZONETYPE_ABBR:
				PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(zone, z);
				add_ascii_assoc_bool(return_value, "is_dst", parsed_time->dst);
				add_ascii_assoc_ascii_string(return_value, "tz_abbr", parsed_time->tz_abbr, 1);
				break;
		}
	}
	if (parsed_time->have_relative) {
		MAKE_STD_ZVAL(element);
		array_init(element);
		add_ascii_assoc_long(element, "year",   parsed_time->relative.y);
		add_ascii_assoc_long(element, "month",  parsed_time->relative.m);
		add_ascii_assoc_long(element, "day",    parsed_time->relative.d);
		add_ascii_assoc_long(element, "hour",   parsed_time->relative.h);
		add_ascii_assoc_long(element, "minute", parsed_time->relative.i);
		add_ascii_assoc_long(element, "second", parsed_time->relative.s);
		if (parsed_time->relative.have_weekday_relative) {
			add_ascii_assoc_long(element, "weekday", parsed_time->relative.weekday);
		}
		if (parsed_time->relative.have_special_relative && (parsed_time->relative.special.type == TIMELIB_SPECIAL_WEEKDAY)) {
			add_ascii_assoc_long(element, "weekdays", parsed_time->relative.special.amount);
		}
		if (parsed_time->relative.first_last_day_of) {
			add_ascii_assoc_bool(element, parsed_time->relative.first_last_day_of == 1 ? "first_day_of_month" : "last_day_of_month", 1);
		}
		add_ascii_assoc_zval(return_value, "relative", element);
	}
	timelib_time_dtor(parsed_time);
}

/* {{{ proto array date_parse(string date) U
   Returns associative array with detailed info about given date
*/
PHP_FUNCTION(date_parse)
{
	char                           *date;
	int                             date_len;
	struct timelib_error_container *error;
	timelib_time                   *parsed_time;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &date, &date_len) == FAILURE) {
		RETURN_FALSE;
	}

	parsed_time = timelib_strtotime(date, date_len, &error, DATE_TIMEZONEDB);
	php_date_do_return_parsed_time(INTERNAL_FUNCTION_PARAM_PASSTHRU, parsed_time, error);
}
/* }}} */

/* {{{ proto array date_parse_from_format(string format, string date) U
   Returns associative array with detailed info about given date
*/
PHP_FUNCTION(date_parse_from_format)
{
	char                           *date, *format;
	int                             date_len, format_len;
	struct timelib_error_container *error;
	timelib_time                   *parsed_time;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &format, &format_len, &date, &date_len) == FAILURE) {
		RETURN_FALSE;
	}

	parsed_time = timelib_parse_from_format(format, date, date_len, &error, DATE_TIMEZONEDB);
	php_date_do_return_parsed_time(INTERNAL_FUNCTION_PARAM_PASSTHRU, parsed_time, error);
}
/* }}} */

/* {{{ proto string date_format(DateTime object, string format) U
   Returns date formatted according to given format
*/
PHP_FUNCTION(date_format)
{
	zval         *object;
	php_date_obj *dateobj;
	char         *format, *str;
	int           format_len, length;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &object, date_ce_date, &format, &format_len) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	str = date_format(format, format_len, &length, dateobj->time, dateobj->time->is_localtime, 0 TSRMLS_CC);
	if (UG(unicode)) {
		RETURN_UNICODEL((UChar*) str, length, 0);
	} else {
		RETURN_STRINGL(str, length, 0);
	}
}
/* }}} */

/* {{{ proto string date_format_locale(DateTime object, string format) U
*/
PHP_FUNCTION(date_format_locale)
{
	zval         *object;
	php_date_obj *dateobj;
	char         *format, *str;
	int           format_len, length;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &object, date_ce_date, &format, &format_len) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);

	str = date_format(format, format_len, &length, dateobj->time, dateobj->time->is_localtime, 1 TSRMLS_CC);
	if (UG(unicode)) {
		RETURN_UNICODEL((UChar*)str, length, 0);
	} else {
		RETURN_STRINGL(str, length, 0);
	}
}
/* }}} */

/* {{{ proto void date_modify(DateTime object, string modify) U
   Alters the timestamp.
*/
PHP_FUNCTION(date_modify)
{
	zval         *object;
	php_date_obj *dateobj;
	char         *modify;
	int           modify_len;
	timelib_time *tmp_time;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &object, date_ce_date, &modify, &modify_len) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);

	tmp_time = timelib_strtotime(modify, modify_len, NULL, DATE_TIMEZONEDB);
	memcpy(&dateobj->time->relative, &tmp_time->relative, sizeof(struct timelib_rel_time));
	dateobj->time->have_relative = tmp_time->have_relative;
	dateobj->time->sse_uptodate = 0;
	timelib_time_dtor(tmp_time);

	timelib_update_ts(dateobj->time, NULL);
	timelib_update_from_sse(dateobj->time);

	RETURN_ZVAL(object, 1, 0);
}
/* }}} */

/* {{{ proto void date_add(DateTime object, DateInterval interval) U
   Adds an interval to the current date in object.
*/
PHP_FUNCTION(date_add)
{
	zval         *object, *interval;
	php_date_obj *dateobj;
	php_interval_obj *intobj;
	int               bias = 1;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OO", &object, date_ce_date, &interval, date_ce_interval) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	intobj = (php_interval_obj *) zend_object_store_get_object(interval TSRMLS_CC);
	DATE_CHECK_INITIALIZED(intobj->initialized, DateInterval);


	if (intobj->diff->have_weekday_relative || intobj->diff->have_special_relative) {
		memcpy(&dateobj->time->relative, intobj->diff, sizeof(struct timelib_rel_time));
	} else {
		if (intobj->diff->invert) {
			bias = -1;
		}
		dateobj->time->relative.y = intobj->diff->y * bias;
		dateobj->time->relative.m = intobj->diff->m * bias;
		dateobj->time->relative.d = intobj->diff->d * bias;
		dateobj->time->relative.h = intobj->diff->h * bias;
		dateobj->time->relative.i = intobj->diff->i * bias;
		dateobj->time->relative.s = intobj->diff->s * bias;
		dateobj->time->relative.weekday = 0;
		dateobj->time->relative.have_weekday_relative = 0;
	}
	dateobj->time->have_relative = 1;
	dateobj->time->sse_uptodate = 0;

	timelib_update_ts(dateobj->time, NULL);
	timelib_update_from_sse(dateobj->time);

	RETURN_ZVAL(object, 1, 0);
}
/* }}} */

/* {{{ proto void date_sub(DateTime object, DateInterval interval) U
   Subtracts an interval to the current date in object.
*/
PHP_FUNCTION(date_sub)
{
	zval         *object, *interval;
	php_date_obj *dateobj;
	php_interval_obj *intobj;
	int               bias = 1;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OO", &object, date_ce_date, &interval, date_ce_interval) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	intobj = (php_interval_obj *) zend_object_store_get_object(interval TSRMLS_CC);
	DATE_CHECK_INITIALIZED(intobj->initialized, DateInterval);

	if (intobj->diff->invert) {
		bias = -1;
	}

	dateobj->time->relative.y = 0 - (intobj->diff->y * bias);
	dateobj->time->relative.m = 0 - (intobj->diff->m * bias);
	dateobj->time->relative.d = 0 - (intobj->diff->d * bias);
	dateobj->time->relative.h = 0 - (intobj->diff->h * bias);
	dateobj->time->relative.i = 0 - (intobj->diff->i * bias);
	dateobj->time->relative.s = 0 - (intobj->diff->s * bias);
	dateobj->time->have_relative = 1;
	dateobj->time->relative.weekday = 0;
	dateobj->time->relative.have_weekday_relative = 0;
	dateobj->time->sse_uptodate = 0;

	timelib_update_ts(dateobj->time, NULL);
	timelib_update_from_sse(dateobj->time);

	RETURN_ZVAL(object, 1, 0);
}
/* }}} */

/* {{{ proto DateTimeZone date_timezone_get(DateTime object) U
   Return new DateTimeZone object relative to give DateTime
*/
PHP_FUNCTION(date_timezone_get)
{
	zval             *object;
	php_date_obj     *dateobj;
	php_timezone_obj *tzobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, date_ce_date) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	if (dateobj->time->is_localtime/* && dateobj->time->tz_info*/) {
		date_instantiate(date_ce_timezone, return_value TSRMLS_CC);
		tzobj = (php_timezone_obj *) zend_object_store_get_object(return_value TSRMLS_CC);
		tzobj->initialized = 1;
		tzobj->type = dateobj->time->zone_type;
		switch (dateobj->time->zone_type) {
			case TIMELIB_ZONETYPE_ID:
				tzobj->tzi.tz = dateobj->time->tz_info;
				break;
			case TIMELIB_ZONETYPE_OFFSET:
				tzobj->tzi.utc_offset = dateobj->time->z;
				break;
			case TIMELIB_ZONETYPE_ABBR:
				tzobj->tzi.z.utc_offset = dateobj->time->z;
				tzobj->tzi.z.dst = dateobj->time->dst;
				tzobj->tzi.z.abbr = strdup(dateobj->time->tz_abbr);
				break;
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void date_timezone_set(DateTime object, DateTimeZone object)
   Sets the timezone for the DateTime object.
*/
PHP_FUNCTION(date_timezone_set)
{
	zval             *object;
	zval             *timezone_object;
	php_date_obj     *dateobj;
	php_timezone_obj *tzobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OO", &object, date_ce_date, &timezone_object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	tzobj = (php_timezone_obj *) zend_object_store_get_object(timezone_object TSRMLS_CC);
	if (tzobj->type != TIMELIB_ZONETYPE_ID) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can only do this for zones with ID for now");
		return;
	}
	timelib_set_timezone(dateobj->time, tzobj->tzi.tz);
	timelib_unixtime2local(dateobj->time, dateobj->time->sse);

	RETURN_ZVAL(object, 1, 0);
}
/* }}} */

/* {{{ proto long date_offset_get(DateTime object) U
   Returns the DST offset.
*/
PHP_FUNCTION(date_offset_get)
{
	zval                *object;
	php_date_obj        *dateobj;
	timelib_time_offset *offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, date_ce_date) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	if (dateobj->time->is_localtime/* && dateobj->time->tz_info*/) {
		switch (dateobj->time->zone_type) {
			case TIMELIB_ZONETYPE_ID:
				offset = timelib_get_time_zone_info(dateobj->time->sse, dateobj->time->tz_info);
				RETVAL_LONG(offset->offset);
				timelib_time_offset_dtor(offset);
				break;
			case TIMELIB_ZONETYPE_OFFSET:
				RETVAL_LONG(dateobj->time->z * -60);
				break;
			case TIMELIB_ZONETYPE_ABBR:
				RETVAL_LONG((dateobj->time->z - (60 * dateobj->time->dst)) * -60);
				break;
		}
		return;
	} else {
		RETURN_LONG(0);
	}
}
/* }}} */

/* {{{ proto void date_time_set(DateTime object, long hour, long minute[, long second]) U
   Sets the time.
*/
PHP_FUNCTION(date_time_set)
{
	zval         *object;
	php_date_obj *dateobj;
	long          h, i, s = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oll|l", &object, date_ce_date, &h, &i, &s) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	dateobj->time->h = h;
	dateobj->time->i = i;
	dateobj->time->s = s;
	timelib_update_ts(dateobj->time, NULL);

	RETURN_ZVAL(object, 1, 0);
}
/* }}} */

/* {{{ proto void date_date_set(DateTime object, long year, long month, long day) U
   Sets the date.
*/
PHP_FUNCTION(date_date_set)
{
	zval         *object;
	php_date_obj *dateobj;
	long          y, m, d;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Olll", &object, date_ce_date, &y, &m, &d) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	dateobj->time->y = y;
	dateobj->time->m = m;
	dateobj->time->d = d;
	timelib_update_ts(dateobj->time, NULL);

	RETURN_ZVAL(object, 1, 0);
}
/* }}} */

/* {{{ proto void date_isodate_set(DateTime object, long year, long week[, long day]) U
   Sets the ISO date.
*/
PHP_FUNCTION(date_isodate_set)
{
	zval         *object;
	php_date_obj *dateobj;
	long          y, w, d = 1;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oll|l", &object, date_ce_date, &y, &w, &d) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	dateobj->time->y = y;
	dateobj->time->m = 1;
	dateobj->time->d = 1;
	dateobj->time->relative.d = timelib_daynr_from_weeknr(y, w, d);
	dateobj->time->have_relative = 1;
	
	timelib_update_ts(dateobj->time, NULL);

	RETURN_ZVAL(object, 1, 0);
}
/* }}} */

/* {{{ proto void date_timestamp_set(DateTime object, long unixTimestamp) U
   Sets the date and time based on an Unix timestamp.
*/
PHP_FUNCTION(date_timestamp_set)
{
	zval         *object;
	php_date_obj *dateobj;
	long          timestamp;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &object, date_ce_date, &timestamp) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	timelib_unixtime2local(dateobj->time, (timelib_sll)timestamp);
	timelib_update_ts(dateobj->time, NULL);

	RETURN_ZVAL(object, 1, 0);
}
/* }}} */

/* {{{ proto long date_timestamp_get(DateTime object) U
   Gets the Unix timestamp.
*/
PHP_FUNCTION(date_timestamp_get)
{
	zval         *object;
	php_date_obj *dateobj;
	long          timestamp;
	int           error;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, date_ce_date) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
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

/* {{{ proto DateInterval date_diff(DateTime object [, bool absolute]) U
   Returns the difference between two DateTime objects.
*/
PHP_FUNCTION(date_diff)
{
	zval         *object1, *object2;
	php_date_obj *dateobj1, *dateobj2;
	php_interval_obj *interval;
	long          absolute = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OO|l", &object1, date_ce_date, &object2, date_ce_date, &absolute) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj1 = (php_date_obj *) zend_object_store_get_object(object1 TSRMLS_CC);
	dateobj2 = (php_date_obj *) zend_object_store_get_object(object2 TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj1->time, DateTime);
	DATE_CHECK_INITIALIZED(dateobj2->time, DateTime);
	timelib_update_ts(dateobj1->time, NULL);
	timelib_update_ts(dateobj2->time, NULL);

	date_instantiate(date_ce_interval, return_value TSRMLS_CC);
	interval = zend_object_store_get_object(return_value TSRMLS_CC);
	interval->diff = timelib_diff(dateobj1->time, dateobj2->time);
	if (absolute) {
		interval->diff->invert = 0;
	}
	interval->initialized = 1;
}
/* }}} */

static int timezone_initialize(timelib_tzinfo **tzi, /*const*/ char *tz TSRMLS_DC)
{
	char *tzid;
	
	*tzi = NULL;
	
	if ((tzid = timelib_timezone_id_from_abbr(tz, -1, 0))) {
		*tzi = php_date_parse_tzfile(tzid, DATE_TIMEZONEDB TSRMLS_CC);
	} else {
		*tzi = php_date_parse_tzfile(tz, DATE_TIMEZONEDB TSRMLS_CC);
	}
	
	if (*tzi) {
		return SUCCESS;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown or bad timezone (%s)", tz);
		return FAILURE;
	}
}

/* {{{ proto DateTimeZone timezone_open(string timezone) U
   Returns new DateTimeZone object
*/
PHP_FUNCTION(timezone_open)
{
	char *tz;
	int   tz_len;
	timelib_tzinfo *tzi = NULL;
	php_timezone_obj *tzobj;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tz, &tz_len) == FAILURE) {
		RETURN_FALSE;
	}
	if (SUCCESS != timezone_initialize(&tzi, tz TSRMLS_CC)) {
		RETURN_FALSE;
	}
	tzobj = zend_object_store_get_object(date_instantiate(date_ce_timezone, return_value TSRMLS_CC) TSRMLS_CC);
	tzobj->type = TIMELIB_ZONETYPE_ID;
	tzobj->tzi.tz = tzi;
	tzobj->initialized = 1;
}
/* }}} */

/* {{{ proto DateTimeZone::__construct(string timezone)
   Creates new DateTimeZone object.
*/
PHP_METHOD(DateTimeZone, __construct)
{
	char *tz;
	int tz_len;
	timelib_tzinfo *tzi = NULL;
	php_timezone_obj *tzobj;
	
	zend_replace_error_handling(EH_THROW, NULL, NULL TSRMLS_CC);
	if (SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tz, &tz_len)) {
		if (SUCCESS == timezone_initialize(&tzi, tz TSRMLS_CC)) {
			tzobj = zend_object_store_get_object(getThis() TSRMLS_CC);
			tzobj->type = TIMELIB_ZONETYPE_ID;
			tzobj->tzi.tz = tzi;
			tzobj->initialized = 1;
		} else {
			ZVAL_NULL(getThis());
		}
	}
}
/* }}} */

/* {{{ proto string timezone_name_get(DateTimeZone object) U
   Returns the name of the timezone.
*/
PHP_FUNCTION(timezone_name_get)
{
	zval             *object;
	php_timezone_obj *tzobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}
	tzobj = (php_timezone_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(tzobj->initialized, DateTimeZone);

	switch (tzobj->type) {
		case TIMELIB_ZONETYPE_ID:
			RETURN_ASCII_STRING(tzobj->tzi.tz->name, 1);
			break;
		case TIMELIB_ZONETYPE_OFFSET: {
			char *tmpstr = emalloc(sizeof("UTC+05:00"));
			timelib_sll utc_offset = tzobj->tzi.utc_offset;

			snprintf(tmpstr, sizeof("+05:00"), "%c%02d:%02d",
				utc_offset > 0 ? '-' : '+',
				abs(utc_offset / 60),
				abs((utc_offset % 60)));

			RETURN_ASCII_STRING(tmpstr, ZSTR_AUTOFREE);
			}
			break;
		case TIMELIB_ZONETYPE_ABBR:
			RETURN_ASCII_STRING(tzobj->tzi.z.abbr, 1);
			break;
	}
}
/* }}} */

/* {{{ proto string timezone_name_from_abbr(string abbr[, long gmtOffset[, long isdst]]) U
   Returns the timezone name from abbrevation
*/
PHP_FUNCTION(timezone_name_from_abbr)
{
	char    *abbr;
	char    *tzid;
	int      abbr_len;
	long     gmtoffset = -1;
	long     isdst = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ll", &abbr, &abbr_len, &gmtoffset, &isdst) == FAILURE) {
		RETURN_FALSE;
	}
	tzid = timelib_timezone_id_from_abbr(abbr, gmtoffset, isdst);

	if (tzid) {
		RETURN_ASCII_STRING(tzid, 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto long timezone_offset_get(DateTimeZone object, DateTime object) U
   Returns the timezone offset.
*/
PHP_FUNCTION(timezone_offset_get)
{
	zval                *object, *dateobject;
	php_timezone_obj    *tzobj;
	php_date_obj        *dateobj;
	timelib_time_offset *offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OO", &object, date_ce_timezone, &dateobject, date_ce_date) == FAILURE) {
		RETURN_FALSE;
	}
	tzobj = (php_timezone_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(tzobj->initialized, DateTimeZone);
	dateobj = (php_date_obj *) zend_object_store_get_object(dateobject TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);

	switch (tzobj->type) {
		case TIMELIB_ZONETYPE_ID:
			offset = timelib_get_time_zone_info(dateobj->time->sse, tzobj->tzi.tz);
			RETVAL_LONG(offset->offset);
			timelib_time_offset_dtor(offset);
			break;
		case TIMELIB_ZONETYPE_OFFSET:
			RETURN_LONG(tzobj->tzi.utc_offset * -60);
			break;
		case TIMELIB_ZONETYPE_ABBR:
			RETURN_LONG((tzobj->tzi.z.utc_offset - (tzobj->tzi.z.dst*60)) * -60);
			break;
	}
}
/* }}} */

/* {{{ proto array timezone_transitions_get(DateTimeZone object [, long timestamp_begin [, long timestamp_end ]]) U
   Returns numerically indexed array containing associative array for all transitions in the specified range for the timezone.
*/
PHP_FUNCTION(timezone_transitions_get)
{
	zval                *object, *element;
	php_timezone_obj    *tzobj;
	int                  i, begin = 0, found;
	long                 timestamp_begin = LONG_MIN, timestamp_end = LONG_MAX;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|ll", &object, date_ce_timezone, &timestamp_begin, &timestamp_end) == FAILURE) {
		RETURN_FALSE;
	}
	tzobj = (php_timezone_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(tzobj->initialized, DateTimeZone);
	if (tzobj->type != TIMELIB_ZONETYPE_ID) {
		RETURN_FALSE;
	}

#define add_nominal() \
		MAKE_STD_ZVAL(element); \
		array_init(element); \
		add_ascii_assoc_long(element, "ts",     timestamp_begin); \
		if (UG(unicode)) { \
			add_ascii_assoc_unicode(element, "time", (UChar*) php_format_date(DATE_FORMAT_ISO8601, 13, timestamp_begin, 0 TSRMLS_CC), 0); \
		} else { \
			add_assoc_string(element, "time", php_format_date(DATE_FORMAT_ISO8601, 13, timestamp_begin, 0 TSRMLS_CC), 0); \
		} \
		add_ascii_assoc_long(element, "offset", tzobj->tzi.tz->type[0].offset); \
		add_ascii_assoc_bool(element, "isdst",  tzobj->tzi.tz->type[0].isdst); \
		add_ascii_assoc_ascii_string(element, "abbr", &tzobj->tzi.tz->timezone_abbr[tzobj->tzi.tz->type[0].abbr_idx], 1); \
		add_next_index_zval(return_value, element);

#define add(i,ts) \
		MAKE_STD_ZVAL(element); \
		array_init(element); \
		add_ascii_assoc_long(element, "ts",     ts); \
		if (UG(unicode)) { \
			add_ascii_assoc_unicode(element, "time", (UChar*) php_format_date(DATE_FORMAT_ISO8601, 13, ts, 0 TSRMLS_CC), 0); \
		} else { \
			add_assoc_string(element, "time", php_format_date(DATE_FORMAT_ISO8601, 13, ts, 0 TSRMLS_CC), 0); \
		} \
		add_ascii_assoc_long(element, "offset", tzobj->tzi.tz->type[tzobj->tzi.tz->trans_idx[i]].offset); \
		add_ascii_assoc_bool(element, "isdst",  tzobj->tzi.tz->type[tzobj->tzi.tz->trans_idx[i]].isdst); \
		add_ascii_assoc_ascii_string(element, "abbr", &tzobj->tzi.tz->timezone_abbr[tzobj->tzi.tz->type[tzobj->tzi.tz->trans_idx[i]].abbr_idx], 1); \
		add_next_index_zval(return_value, element);

#define add_last() add(tzobj->tzi.tz->timecnt - 1, timestamp_begin)

	array_init(return_value);

	if (timestamp_begin == LONG_MIN) {
		add_nominal();
		begin = 0;
		found = 1;
	} else {
		begin = 0;
		found = 0;
		if (tzobj->tzi.tz->timecnt > 0) {
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
			} while (begin < tzobj->tzi.tz->timecnt);
		}
	}

	if (!found) {
		if (tzobj->tzi.tz->timecnt > 0) {
			add_last();
		} else {
			add_nominal();
		}
	} else {
		for (i = begin; i < tzobj->tzi.tz->timecnt; ++i) {
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

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}
	tzobj = (php_timezone_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(tzobj->initialized, DateTimeZone);
	if (tzobj->type != TIMELIB_ZONETYPE_ID) {
		RETURN_FALSE;
	}

	array_init(return_value);
	add_assoc_string(return_value, "country_code", tzobj->tzi.tz->location.country_code, 1);
	add_assoc_double(return_value, "latitude", tzobj->tzi.tz->location.latitude);
	add_assoc_double(return_value, "longitude", tzobj->tzi.tz->location.longitude);
	add_assoc_string(return_value, "comments", tzobj->tzi.tz->location.comments, 1);
}
/* }}} */

static int date_interval_initialize(timelib_rel_time **rt, /*const*/ char *format, int format_length TSRMLS_DC)
{
	timelib_time     *b = NULL, *e = NULL;
	timelib_rel_time *p = NULL;
	int               r = 0;
	int               retval = 0;
	struct timelib_error_container *errors;

	timelib_strtointerval(format, format_length, &b, &e, &p, &r, &errors);
	
	if (errors->error_count > 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown or bad format (%s)", format);
		retval = FAILURE;
	} else {
		*rt = p;
		retval = SUCCESS;
	}
	timelib_error_container_dtor(errors);
	return retval;
}

/* {{{ date_interval_read_property */
zval *date_interval_read_property(zval *object, zval *member, int type TSRMLS_DC)
{
	php_interval_obj *obj;
	zval *retval;
	zval tmp_member;
	timelib_sll value = -1;

 	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	obj = (php_interval_obj *)zend_objects_get_address(object TSRMLS_CC);

#define GET_VALUE_FROM_STRUCT(n,m)            \
	if (strcmp(Z_STRVAL_P(member), m) == 0) { \
		value = obj->diff->n;                 \
	}
	GET_VALUE_FROM_STRUCT(y, "y");
	GET_VALUE_FROM_STRUCT(m, "m");
	GET_VALUE_FROM_STRUCT(d, "d");
	GET_VALUE_FROM_STRUCT(h, "h");
	GET_VALUE_FROM_STRUCT(i, "i");
	GET_VALUE_FROM_STRUCT(s, "s");
	GET_VALUE_FROM_STRUCT(invert, "invert");
	GET_VALUE_FROM_STRUCT(days, "days");

	ALLOC_INIT_ZVAL(retval);
	Z_SET_REFCOUNT_P(retval, 0);

	if (value == -1) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unknown property (%s)", Z_STRVAL_P(member));
	}

	ZVAL_LONG(retval, value);

	if (member == &tmp_member) {
		zval_dtor(member);
	}

	return retval;
}
/* }}} */

/* {{{ date_interval_write_property */
void date_interval_write_property(zval *object, zval *member, zval *value TSRMLS_DC)
{
	php_interval_obj *obj;
	zval tmp_member, tmp_value;
	int found = 0;

 	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}
	obj = (php_interval_obj *)zend_objects_get_address(object TSRMLS_CC);

#define SET_VALUE_FROM_STRUCT(n,m)            \
	if (strcmp(Z_STRVAL_P(member), m) == 0) { \
		if (value->type != IS_LONG) {         \
			tmp_value = *value;               \
			zval_copy_ctor(&tmp_value);       \
			convert_to_long(&tmp_value);      \
			value = &tmp_value;               \
		}                                     \
		found = 1;                            \
		obj->diff->n = Z_LVAL_P(value); \
		if (value == &tmp_value) {         \
			zval_dtor(value);              \
		}                                  \
	}

	SET_VALUE_FROM_STRUCT(y, "y");
	SET_VALUE_FROM_STRUCT(m, "m");
	SET_VALUE_FROM_STRUCT(d, "d");
	SET_VALUE_FROM_STRUCT(h, "h");
	SET_VALUE_FROM_STRUCT(i, "i");
	SET_VALUE_FROM_STRUCT(s, "s");
	SET_VALUE_FROM_STRUCT(invert, "invert");

	if (!found) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unknown property (%s)", Z_STRVAL_P(member));
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
}
/* }}} */


/* {{{ proto DateInterval::__construct([string interval_spec])
   Creates new DateInterval object.
*/
PHP_METHOD(DateInterval, __construct)
{
	char *interval_string = NULL;
	int   interval_string_length;
	php_interval_obj *diobj;
	timelib_rel_time *reltime;
	
	zend_replace_error_handling(EH_THROW, NULL, NULL TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &interval_string, &interval_string_length) == SUCCESS) {
		if (date_interval_initialize(&reltime, interval_string, interval_string_length TSRMLS_CC) == SUCCESS) {
			diobj = zend_object_store_get_object(getThis() TSRMLS_CC);
			diobj->diff = reltime;
			diobj->initialized = 1;
		} else {
			ZVAL_NULL(getThis());
		}
	}
}
/* }}} */

/* {{{ proto DateInterval date_interval_create_from_date_string(string time)
   Uses the normal date parsers and sets up a DateInterval from the relative parts of the parsed string
*/
PHP_FUNCTION(date_interval_create_from_date_string)
{
	char           *time_str = NULL;
	int             time_str_len = 0;
	timelib_time   *time;
	timelib_error_container *err = NULL;
	php_interval_obj *diobj;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &time_str, &time_str_len) == FAILURE) {
		RETURN_FALSE;
	}

	date_instantiate(date_ce_interval, return_value TSRMLS_CC);

	time = timelib_strtotime(time_str, time_str_len, &err, DATE_TIMEZONEDB);
	diobj = (php_interval_obj *) zend_object_store_get_object(return_value TSRMLS_CC);
	diobj->diff = timelib_rel_time_clone(&time->relative);
	diobj->initialized = 1;
	timelib_time_dtor(time);
	timelib_error_container_dtor(err);
}
/* }}} */

/* {{{ date_interval_format -  */
static char *date_interval_format(char *format, int format_len, timelib_rel_time *t)
{
	smart_str            string = {0};
	int                  i, length, have_format_spec = 0;
	char                 buffer[33];

	if (!format_len) {
		return estrdup("");
	}

	for (i = 0; i < format_len; i++) {
		if (have_format_spec) {
			switch (format[i]) {
				case 'Y': length = slprintf(buffer, 32, "%02d", (int) t->y); break;
				case 'y': length = slprintf(buffer, 32, "%d", (int) t->y); break;

				case 'M': length = slprintf(buffer, 32, "%02d", (int) t->m); break;
				case 'm': length = slprintf(buffer, 32, "%d", (int) t->m); break;

				case 'D': length = slprintf(buffer, 32, "%02d", (int) t->d); break;
				case 'd': length = slprintf(buffer, 32, "%d", (int) t->d); break;

				case 'H': length = slprintf(buffer, 32, "%02d", (int) t->h); break;
				case 'h': length = slprintf(buffer, 32, "%d", (int) t->h); break;

				case 'I': length = slprintf(buffer, 32, "%02d", (int) t->i); break;
				case 'i': length = slprintf(buffer, 32, "%d", (int) t->i); break;

				case 'S': length = slprintf(buffer, 32, "%02d", (int) t->s); break;
				case 's': length = slprintf(buffer, 32, "%d", (int) t->s); break;

				case 'a': length = slprintf(buffer, 32, "%d", (int) t->days); break;
				case 'r': length = slprintf(buffer, 32, "%s", t->invert ? "-" : ""); break;
				case 'R': length = slprintf(buffer, 32, "%c", t->invert ? '-' : '+'); break;

				case '%': length = slprintf(buffer, 32, "%%"); break;
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

	return string.c;
}
/* }}} */

/* {{{ proto string date_interval_format(DateInterval object, string format) U
   Formats the interval.
*/
PHP_FUNCTION(date_interval_format)
{
	zval             *object;
	php_interval_obj *diobj;
	char             *format;
	int               format_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &object, date_ce_interval, &format, &format_len) == FAILURE) {
		RETURN_FALSE;
	}
	diobj = (php_interval_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(diobj->initialized, DateInterval);

	RETURN_ASCII_STRING(date_interval_format(format, format_len, diobj->diff), ZSTR_AUTOFREE);
}
/* }}} */

static int date_period_initialize(timelib_time **st, timelib_time **et, timelib_rel_time **d, int *recurrences, /*const*/ char *format, int format_length TSRMLS_DC)
{
	timelib_time     *b = NULL, *e = NULL;
	timelib_rel_time *p = NULL;
	int               r = 0;
	int               retval = 0;
	struct timelib_error_container *errors;

	timelib_strtointerval(format, format_length, &b, &e, &p, &r, &errors);
	
	if (errors->error_count > 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown or bad format (%s)", format);
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
}

/* {{{ proto DatePeriod::__construct(DateTime $start, DateInterval $interval, int recurrences|DateTime $end)
   Creates new DatePeriod object.
*/
PHP_METHOD(DatePeriod, __construct)
{
	php_period_obj   *dpobj;
	php_date_obj     *dateobj;
	php_interval_obj *intobj;
	zval *start, *end = NULL, *interval;
	long  recurrences = 0, options = 0;
	char *isostr = NULL;
	int   isostr_len = 0;
	timelib_time *clone;
	
	zend_replace_error_handling(EH_THROW, NULL, NULL TSRMLS_CC);
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "OOl|l", &start, date_ce_date, &interval, date_ce_interval, &recurrences, &options) == FAILURE) {
		if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "OOO|l", &start, date_ce_date, &interval, date_ce_interval, &end, date_ce_date, &options) == FAILURE) {
			if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &isostr, &isostr_len, &options) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "This constructor accepts either (DateTime, DateInterval, int) OR (DateTime, DateInterval, DateTime) OR (string) as arguments.");
				return;
			}
		}
	}

	dpobj = zend_object_store_get_object(getThis() TSRMLS_CC);

	if (isostr_len) {
		date_period_initialize(&(dpobj->start), &(dpobj->end), &(dpobj->interval), (int*) &recurrences, isostr, isostr_len TSRMLS_CC);
		if (dpobj->start == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The ISO interval '%s' did not contain a start date.", isostr);
		}
		if (dpobj->interval == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The ISO interval '%s' did not contain an interval.", isostr);
		}
		if (dpobj->end == NULL && recurrences == 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The ISO interval '%s' did not contain an end date or a recurrence count.", isostr);
		}

		if (dpobj->start) {
			timelib_update_ts(dpobj->start, NULL);
		}
		if (dpobj->end) {
			timelib_update_ts(dpobj->end, NULL);
		}
	} else {
		/* init */
		intobj  = (php_interval_obj *) zend_object_store_get_object(interval TSRMLS_CC);

		/* start date */
		dateobj = (php_date_obj *) zend_object_store_get_object(start TSRMLS_CC);
		clone = timelib_time_ctor();
		memcpy(clone, dateobj->time, sizeof(timelib_time));
		if (dateobj->time->tz_abbr) {
			clone->tz_abbr = strdup(dateobj->time->tz_abbr);
		}
		if (dateobj->time->tz_info) {
			clone->tz_info = dateobj->time->tz_info;
		}
		dpobj->start = clone;

		/* interval */
		dpobj->interval = timelib_rel_time_clone(intobj->diff);

		/* end date */
		if (end) {
			dateobj = (php_date_obj *) zend_object_store_get_object(end TSRMLS_CC);
			clone = timelib_time_ctor();
			memcpy(clone, dateobj->time, sizeof(timelib_time));
			if (dateobj->time->tz_abbr) {
				clone->tz_abbr = strdup(dateobj->time->tz_abbr);
			}
			if (dateobj->time->tz_info) {
				clone->tz_info = dateobj->time->tz_info;
			}
			dpobj->end = clone;
		}
	}

	/* options */
	dpobj->include_start_date = !(options & PHP_DATE_PERIOD_EXCLUDE_START_DATE);

	/* recurrrences */
	dpobj->recurrences = recurrences + dpobj->include_start_date;

	dpobj->initialized = 1;
}
/* }}} */

static int check_id_allowed(char *id, long what)
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
}

/* {{{ proto array timezone_identifiers_list([long what]) U
   Returns numerically index array with all timezone identifiers.
*/
PHP_FUNCTION(timezone_identifiers_list)
{
	const timelib_tzdb             *tzdb;
	const timelib_tzdb_index_entry *table;
	int                             i, item_count;
	long                            what = PHP_DATE_TIMEZONE_GROUP_ALL;
	char                           *option = NULL;
	int                             option_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ls", &what, &option, &option_len) == FAILURE) {
		RETURN_FALSE;
	}

	/* Extra validation */
	if (what == PHP_DATE_TIMEZONE_PER_COUNTRY && option_len != 2) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "A two-letter ISO 3166-1 compatible country code is expected");
		RETURN_FALSE;
	}

	tzdb = DATE_TIMEZONEDB;
	item_count = tzdb->index_size;
	table = tzdb->index;
	
	array_init(return_value);

	for (i = 0; i < item_count; ++i) {
		if (what == PHP_DATE_TIMEZONE_PER_COUNTRY) {
			if (tzdb->data[table[i].pos + 5] == option[0] && tzdb->data[table[i].pos + 6] == option[1]) {
				add_next_index_ascii_string(return_value, table[i].id, 1);
			}
		} else if (what == PHP_DATE_TIMEZONE_GROUP_ALL_W_BC || (check_id_allowed(table[i].id, what) && (tzdb->data[table[i].pos + 4] == '\1'))) {
			add_next_index_ascii_string(return_value, table[i].id, 1);
		}
	};
}
/* }}} */

/* {{{ proto array timezone_abbreviations_list() U
   Returns associative array containing dst, offset and the timezone name
*/
PHP_FUNCTION(timezone_abbreviations_list)
{
	const timelib_tz_lookup_table *table, *entry;
	zval                          *element, **abbr_array_pp, *abbr_array;
	
	table = timelib_timezone_abbreviations_list();
	array_init(return_value);
	entry = table;

	do {
		MAKE_STD_ZVAL(element);
		array_init(element);
		add_ascii_assoc_bool(element, "dst", entry->type);
		add_ascii_assoc_long(element, "offset", entry->gmtoffset);
		if (entry->full_tz_name) {
			add_ascii_assoc_ascii_string(element, "timezone_id", entry->full_tz_name, 1);
		} else {
			add_ascii_assoc_null(element, "timezone_id");
		}

		if (zend_ascii_hash_find(HASH_OF(return_value), entry->name, strlen(entry->name) + 1, (void **) &abbr_array_pp) == FAILURE) {
			MAKE_STD_ZVAL(abbr_array);
			array_init(abbr_array);
			add_ascii_assoc_zval(return_value, entry->name, abbr_array);
		} else {
			abbr_array = *abbr_array_pp;
		}
		add_next_index_zval(abbr_array, element);
		entry++;
	} while (entry->name);
}
/* }}} */

/* {{{ proto bool date_default_timezone_set(string timezone_identifier) U
   Sets the default timezone used by all date/time functions in a script */
PHP_FUNCTION(date_default_timezone_set)
{
	char *zone;
	int   zone_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &zone, &zone_len) == FAILURE) {
		RETURN_FALSE;
	}
	if (!timelib_timezone_id_is_valid(zone, DATE_TIMEZONEDB)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Timezone ID '%s' is invalid", zone);
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

/* {{{ proto string date_default_timezone_get() U
   Gets the default timezone used by all date/time functions in a script */
PHP_FUNCTION(date_default_timezone_get)
{
	timelib_tzinfo *default_tz;

	default_tz = get_timezone_info(TSRMLS_C);
	RETVAL_ASCII_STRING(default_tz->name, 1);
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
	long time, retformat = 0;
	int             rs;
	timelib_time   *t;
	timelib_tzinfo *tzi;
	char            retstr[6];
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|ldddd", &time, &retformat, &latitude, &longitude, &zenith, &gmt_offset) == FAILURE) {
		RETURN_FALSE;
	}
	
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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid format");
			RETURN_FALSE;
			break;
	}
	if (retformat != SUNFUNCS_RET_TIMESTAMP &&
		retformat != SUNFUNCS_RET_STRING &&
		retformat != SUNFUNCS_RET_DOUBLE)
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong return format given, pick one of SUNFUNCS_RET_TIMESTAMP, SUNFUNCS_RET_STRING or SUNFUNCS_RET_DOUBLE");
		RETURN_FALSE;
	}
	altitude = 90 - zenith;

	/* Initialize time struct */
	t = timelib_time_ctor();
	tzi = get_timezone_info(TSRMLS_C);
	t->tz_info = tzi;
	t->zone_type = TIMELIB_ZONETYPE_ID;

	if (ZEND_NUM_ARGS() <= 5) {
		gmt_offset = timelib_get_current_offset(t) / 3600;
	}

	timelib_unixtime2local(t, time);
	rs = timelib_astro_rise_set_altitude(t, longitude, latitude, altitude, altitude > -1 ? 1 : 0, &h_rise, &h_set, &rise, &set, &transit);
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
			sprintf(retstr, "%02d:%02d", (int) N, (int) (60 * (N - (int) N)));
			RETURN_RT_STRINGL(retstr, 5, 1);
			break;
		case SUNFUNCS_RET_DOUBLE:
			RETURN_DOUBLE(N);
			break;
	}
}
/* }}} */

/* {{{ proto mixed date_sunrise(mixed time [, int format [, float latitude [, float longitude [, float zenith [, float gmt_offset]]]]]) U
   Returns time of sunrise for a given day and location */
PHP_FUNCTION(date_sunrise)
{
	php_do_date_sunrise_sunset(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto mixed date_sunset(mixed time [, int format [, float latitude [, float longitude [, float zenith [, float gmt_offset]]]]]) U
   Returns time of sunset for a given day and location */
PHP_FUNCTION(date_sunset)
{
	php_do_date_sunrise_sunset(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto array date_sun_info(long time, float latitude, float longitude) U
   Returns an array with information about sun set/rise and twilight begin/end */
PHP_FUNCTION(date_sun_info)
{
	long            time;
	double          latitude, longitude;
	timelib_time   *t, *t2;
	timelib_tzinfo *tzi;
	int             rs;
	timelib_sll     rise, set, transit;
	int             dummy;
	double          ddummy;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ldd", &time, &latitude, &longitude) == FAILURE) {
		RETURN_FALSE;
	}
	/* Initialize time struct */
	t = timelib_time_ctor();
	tzi = get_timezone_info(TSRMLS_C);
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
			add_ascii_assoc_bool(return_value, "sunrise", 0);
			add_ascii_assoc_bool(return_value, "sunset", 0);
			break;
		case 1: /* always above */
			add_ascii_assoc_bool(return_value, "sunrise", 1);
			add_ascii_assoc_bool(return_value, "sunset", 1);
			break;
		default:
			t2->sse = rise;
			add_ascii_assoc_long(return_value, "sunrise", timelib_date_to_int(t2, &dummy));
			t2->sse = set;
			add_ascii_assoc_long(return_value, "sunset", timelib_date_to_int(t2, &dummy));
	}
	t2->sse = transit;
	add_ascii_assoc_long(return_value, "transit", timelib_date_to_int(t2, &dummy));

	/* Get civil twilight */
	rs = timelib_astro_rise_set_altitude(t, longitude, latitude, -6.0, 0, &ddummy, &ddummy, &rise, &set, &transit);
	switch (rs) {
		case -1: /* always below */
			add_ascii_assoc_bool(return_value, "civil_twilight_begin", 0);
			add_ascii_assoc_bool(return_value, "civil_twilight_end", 0);
			break;
		case 1: /* always above */
			add_ascii_assoc_bool(return_value, "civil_twilight_begin", 1);
			add_ascii_assoc_bool(return_value, "civil_twilight_end", 1);
			break;
		default:
			t2->sse = rise;
			add_ascii_assoc_long(return_value, "civil_twilight_begin", timelib_date_to_int(t2, &dummy));
			t2->sse = set;
			add_ascii_assoc_long(return_value, "civil_twilight_end", timelib_date_to_int(t2, &dummy));
	}

	/* Get nautical twilight */
	rs = timelib_astro_rise_set_altitude(t, longitude, latitude, -12.0, 0, &ddummy, &ddummy, &rise, &set, &transit);
	switch (rs) {
		case -1: /* always below */
			add_ascii_assoc_bool(return_value, "nautical_twilight_begin", 0);
			add_ascii_assoc_bool(return_value, "nautical_twilight_end", 0);
			break;
		case 1: /* always above */
			add_ascii_assoc_bool(return_value, "nautical_twilight_begin", 1);
			add_ascii_assoc_bool(return_value, "nautical_twilight_end", 1);
			break;
		default:
			t2->sse = rise;
			add_ascii_assoc_long(return_value, "nautical_twilight_begin", timelib_date_to_int(t2, &dummy));
			t2->sse = set;
			add_ascii_assoc_long(return_value, "nautical_twilight_end", timelib_date_to_int(t2, &dummy));
	}

	/* Get astronomical twilight */
	rs = timelib_astro_rise_set_altitude(t, longitude, latitude, -18.0, 0, &ddummy, &ddummy, &rise, &set, &transit);
	switch (rs) {
		case -1: /* always below */
			add_ascii_assoc_bool(return_value, "astronomical_twilight_begin", 0);
			add_ascii_assoc_bool(return_value, "astronomical_twilight_end", 0);
			break;
		case 1: /* always above */
			add_ascii_assoc_bool(return_value, "astronomical_twilight_begin", 1);
			add_ascii_assoc_bool(return_value, "astronomical_twilight_end", 1);
			break;
		default:
			t2->sse = rise;
			add_ascii_assoc_long(return_value, "astronomical_twilight_begin", timelib_date_to_int(t2, &dummy));
			t2->sse = set;
			add_ascii_assoc_long(return_value, "astronomical_twilight_end", timelib_date_to_int(t2, &dummy));
	}
	timelib_time_dtor(t);
	timelib_time_dtor(t2);
}
/* }}} */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
