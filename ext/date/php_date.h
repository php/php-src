/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
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

#ifndef PHP_DATE_H
#define PHP_DATE_H

#include "lib/timelib.h"
#include "Zend/zend_hash.h"

#include "php_version.h"
#define PHP_DATE_VERSION PHP_VERSION

extern zend_module_entry date_module_entry;
#define phpext_date_ptr &date_module_entry

PHP_FUNCTION(date);
PHP_FUNCTION(idate);
PHP_FUNCTION(gmdate);
PHP_FUNCTION(strtotime);

PHP_FUNCTION(mktime);
PHP_FUNCTION(gmmktime);

PHP_FUNCTION(checkdate);

#ifdef HAVE_STRFTIME
PHP_FUNCTION(strftime);
PHP_FUNCTION(gmstrftime);
#endif

PHP_FUNCTION(time);
PHP_FUNCTION(localtime);
PHP_FUNCTION(getdate);

/* Advanced Interface */
PHP_METHOD(DateTime, __construct);
PHP_METHOD(DateTime, __wakeup);
PHP_METHOD(DateTime, __set_state);
PHP_FUNCTION(date_create);
PHP_FUNCTION(date_create_immutable);
PHP_FUNCTION(date_create_from_format);
PHP_FUNCTION(date_create_immutable_from_format);
PHP_FUNCTION(date_parse);
PHP_FUNCTION(date_parse_from_format);
PHP_FUNCTION(date_get_last_errors);
PHP_FUNCTION(date_format);
PHP_FUNCTION(date_modify);
PHP_FUNCTION(date_add);
PHP_FUNCTION(date_sub);
PHP_FUNCTION(date_timezone_get);
PHP_FUNCTION(date_timezone_set);
PHP_FUNCTION(date_offset_get);
PHP_FUNCTION(date_diff);

PHP_FUNCTION(date_time_set);
PHP_FUNCTION(date_date_set);
PHP_FUNCTION(date_isodate_set);
PHP_FUNCTION(date_timestamp_set);
PHP_FUNCTION(date_timestamp_get);

PHP_METHOD(DateTimeImmutable, __construct);
PHP_METHOD(DateTimeImmutable, __set_state);
PHP_METHOD(DateTimeImmutable, modify);
PHP_METHOD(DateTimeImmutable, add);
PHP_METHOD(DateTimeImmutable, sub);
PHP_METHOD(DateTimeImmutable, setTimezone);
PHP_METHOD(DateTimeImmutable, setTime);
PHP_METHOD(DateTimeImmutable, setDate);
PHP_METHOD(DateTimeImmutable, setISODate);
PHP_METHOD(DateTimeImmutable, setTimestamp);
PHP_METHOD(DateTimeImmutable, createFromMutable);

PHP_METHOD(DateTimeZone, __construct);
PHP_METHOD(DateTimeZone, __wakeup);
PHP_METHOD(DateTimeZone, __set_state);
PHP_FUNCTION(timezone_open);
PHP_FUNCTION(timezone_name_get);
PHP_FUNCTION(timezone_name_from_abbr);
PHP_FUNCTION(timezone_offset_get);
PHP_FUNCTION(timezone_transitions_get);
PHP_FUNCTION(timezone_location_get);
PHP_FUNCTION(timezone_identifiers_list);
PHP_FUNCTION(timezone_abbreviations_list);
PHP_FUNCTION(timezone_version_get);

PHP_METHOD(DateInterval, __construct);
PHP_METHOD(DateInterval, __wakeup);
PHP_METHOD(DateInterval, __set_state);
PHP_FUNCTION(date_interval_format);
PHP_FUNCTION(date_interval_create_from_date_string);

PHP_METHOD(DatePeriod, __construct);
PHP_METHOD(DatePeriod, __wakeup);
PHP_METHOD(DatePeriod, __set_state);
PHP_METHOD(DatePeriod, getStartDate);
PHP_METHOD(DatePeriod, getEndDate);
PHP_METHOD(DatePeriod, getDateInterval);

/* Options and Configuration */
PHP_FUNCTION(date_default_timezone_set);
PHP_FUNCTION(date_default_timezone_get);

/* Astro functions */
PHP_FUNCTION(date_sunrise);
PHP_FUNCTION(date_sunset);
PHP_FUNCTION(date_sun_info);

PHP_RINIT_FUNCTION(date);
PHP_RSHUTDOWN_FUNCTION(date);
PHP_MINIT_FUNCTION(date);
PHP_MSHUTDOWN_FUNCTION(date);
PHP_MINFO_FUNCTION(date);

typedef struct _php_date_obj php_date_obj;
typedef struct _php_timezone_obj php_timezone_obj;
typedef struct _php_interval_obj php_interval_obj;
typedef struct _php_period_obj php_period_obj;

struct _php_date_obj {
	timelib_time *time;
	HashTable    *props;
	zend_object   std;
};

static inline php_date_obj *php_date_obj_from_obj(zend_object *obj) {
	return (php_date_obj*)((char*)(obj) - XtOffsetOf(php_date_obj, std));
}

#define Z_PHPDATE_P(zv)  php_date_obj_from_obj(Z_OBJ_P((zv)))

struct _php_timezone_obj {
	int             initialized;
	int             type;
	union {
		timelib_tzinfo   *tz;         /* TIMELIB_ZONETYPE_ID */
		timelib_sll       utc_offset; /* TIMELIB_ZONETYPE_OFFSET */
		timelib_abbr_info z;          /* TIMELIB_ZONETYPE_ABBR */
	} tzi;
	HashTable *props;
	zend_object std;
};

static inline php_timezone_obj *php_timezone_obj_from_obj(zend_object *obj) {
	return (php_timezone_obj*)((char*)(obj) - XtOffsetOf(php_timezone_obj, std));
}

#define Z_PHPTIMEZONE_P(zv)  php_timezone_obj_from_obj(Z_OBJ_P((zv)))

struct _php_interval_obj {
	timelib_rel_time *diff;
	HashTable        *props;
	int               initialized;
	zend_object       std;
};

static inline php_interval_obj *php_interval_obj_from_obj(zend_object *obj) {
	return (php_interval_obj*)((char*)(obj) - XtOffsetOf(php_interval_obj, std));
}

#define Z_PHPINTERVAL_P(zv)  php_interval_obj_from_obj(Z_OBJ_P((zv)))

struct _php_period_obj {
	timelib_time     *start;
	zend_class_entry *start_ce;
	timelib_time     *current;
	timelib_time     *end;
	timelib_rel_time *interval;
	int               recurrences;
	int               initialized;
	int               include_start_date;
	zend_object       std;
};

static inline php_period_obj *php_period_obj_from_obj(zend_object *obj) {
	return (php_period_obj*)((char*)(obj) - XtOffsetOf(php_period_obj, std));
}

#define Z_PHPPERIOD_P(zv)  php_period_obj_from_obj(Z_OBJ_P((zv)))

ZEND_BEGIN_MODULE_GLOBALS(date)
	char                    *default_timezone;
	char                    *timezone;
	HashTable               *tzcache;
	timelib_error_container *last_errors;
	int                     timezone_valid;
ZEND_END_MODULE_GLOBALS(date)

#define DATEG(v) ZEND_MODULE_GLOBALS_ACCESSOR(date, v)

/* Backwards compatibility wrapper */
PHPAPI zend_long php_parse_date(char *string, zend_long *now);
PHPAPI void php_mktime(INTERNAL_FUNCTION_PARAMETERS, int gmt);
PHPAPI int php_idate(char format, time_t ts, int localtime);
#if HAVE_STRFTIME
#define _php_strftime php_strftime
PHPAPI void php_strftime(INTERNAL_FUNCTION_PARAMETERS, int gm);
#endif
PHPAPI zend_string *php_format_date(char *format, size_t format_len, time_t ts, int localtime);

/* Mechanism to set new TZ database */
PHPAPI void php_date_set_tzdb(timelib_tzdb *tzdb);
PHPAPI timelib_tzinfo *get_timezone_info(void);

/* Grabbing CE's so that other exts can use the date objects too */
PHPAPI zend_class_entry *php_date_get_date_ce(void);
PHPAPI zend_class_entry *php_date_get_immutable_ce(void);
PHPAPI zend_class_entry *php_date_get_interface_ce(void);
PHPAPI zend_class_entry *php_date_get_timezone_ce(void);

/* Functions for creating DateTime objects, and initializing them from a string */
PHPAPI zval *php_date_instantiate(zend_class_entry *pce, zval *object);
PHPAPI int php_date_initialize(php_date_obj *dateobj, /*const*/ char *time_str, size_t time_str_len, char *format, zval *timezone_object, int ctor);


#endif /* PHP_DATE_H */
