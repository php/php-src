/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Derick Rethans <derick@derickrethans.nl>                    |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_DATE_H
#define PHP_DATE_H

#include "lib/timelib.h"
#include "Zend/zend_hash.h"

#include "php_version.h"
#define PHP_DATE_VERSION PHP_VERSION

extern zend_module_entry date_module_entry;
#define phpext_date_ptr &date_module_entry

PHP_RINIT_FUNCTION(date);
PHP_RSHUTDOWN_FUNCTION(date);
PHP_MINIT_FUNCTION(date);
PHP_MSHUTDOWN_FUNCTION(date);
PHP_MINFO_FUNCTION(date);
ZEND_MODULE_POST_ZEND_DEACTIVATE_D(date);

typedef struct _php_date_obj php_date_obj;
typedef struct _php_timezone_obj php_timezone_obj;
typedef struct _php_interval_obj php_interval_obj;
typedef struct _php_period_obj php_period_obj;

struct _php_date_obj {
	timelib_time *time;
	zend_object   std;
};

static inline php_date_obj *php_date_obj_from_obj(zend_object *obj) {
	return (php_date_obj*)((char*)(obj) - XtOffsetOf(php_date_obj, std));
}

#define Z_PHPDATE_P(zv)  php_date_obj_from_obj(Z_OBJ_P((zv)))

struct _php_timezone_obj {
	bool            initialized;
	int             type;
	union {
		timelib_tzinfo   *tz;         /* TIMELIB_ZONETYPE_ID */
		timelib_sll       utc_offset; /* TIMELIB_ZONETYPE_OFFSET */
		timelib_abbr_info z;          /* TIMELIB_ZONETYPE_ABBR */
	} tzi;
	zend_object std;
};

static inline php_timezone_obj *php_timezone_obj_from_obj(zend_object *obj) {
	return (php_timezone_obj*)((char*)(obj) - XtOffsetOf(php_timezone_obj, std));
}

#define Z_PHPTIMEZONE_P(zv)  php_timezone_obj_from_obj(Z_OBJ_P((zv)))

#define PHP_DATE_CIVIL   1
#define PHP_DATE_WALL    2

struct _php_interval_obj {
	timelib_rel_time *diff;
	int               civil_or_wall;
	bool              from_string;
	zend_string      *date_string;
	bool              initialized;
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
	bool              initialized;
	bool              include_start_date;
	bool              include_end_date;
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
ZEND_END_MODULE_GLOBALS(date)

#define DATEG(v) ZEND_MODULE_GLOBALS_ACCESSOR(date, v)

PHPAPI time_t php_time(void);

/* Backwards compatibility wrapper */
PHPAPI zend_long php_parse_date(const char *string, zend_long *now);
PHPAPI void php_mktime(INTERNAL_FUNCTION_PARAMETERS, bool gmt);
PHPAPI int php_idate(char format, time_t ts, bool localtime);

#define _php_strftime php_strftime

PHPAPI void php_strftime(INTERNAL_FUNCTION_PARAMETERS, bool gm);
PHPAPI zend_string *php_format_date(const char *format, size_t format_len, time_t ts, bool localtime);
PHPAPI zend_string *php_format_date_obj(const char *format, size_t format_len, php_date_obj *date_obj);

/* Mechanism to set new TZ database */
PHPAPI void php_date_set_tzdb(timelib_tzdb *tzdb);
PHPAPI timelib_tzinfo *get_timezone_info(void);

/* Grabbing CE's so that other exts can use the date objects too */
PHPAPI zend_class_entry *php_date_get_date_ce(void);
PHPAPI zend_class_entry *php_date_get_immutable_ce(void);
PHPAPI zend_class_entry *php_date_get_interface_ce(void);
PHPAPI zend_class_entry *php_date_get_timezone_ce(void);
PHPAPI zend_class_entry *php_date_get_interval_ce(void);
PHPAPI zend_class_entry *php_date_get_period_ce(void);

/* Functions for creating DateTime objects, and initializing them from a string */
#define PHP_DATE_INIT_CTOR   0x01
#define PHP_DATE_INIT_FORMAT 0x02

PHPAPI zval *php_date_instantiate(zend_class_entry *pce, zval *object);
PHPAPI bool php_date_initialize(php_date_obj *dateobj, const char *time_str, size_t time_str_len, const char *format, zval *timezone_object, int flags);
PHPAPI void php_date_initialize_from_ts_long(php_date_obj *dateobj, zend_long sec, int usec);
PHPAPI bool php_date_initialize_from_ts_double(php_date_obj *dateobj, double ts);

#endif /* PHP_DATE_H */
