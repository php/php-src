/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
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
PHP_FUNCTION(date_create);
PHP_FUNCTION(date_parse);
PHP_FUNCTION(date_format);
PHP_FUNCTION(date_modify);
PHP_FUNCTION(date_timezone_get);
PHP_FUNCTION(date_timezone_set);
PHP_FUNCTION(date_offset_get);

PHP_FUNCTION(date_time_set);
PHP_FUNCTION(date_date_set);
PHP_FUNCTION(date_isodate_set);

PHP_METHOD(DateTimeZone, __construct);
PHP_FUNCTION(timezone_open);
PHP_FUNCTION(timezone_name_get);
PHP_FUNCTION(timezone_name_from_abbr);
PHP_FUNCTION(timezone_offset_get);
PHP_FUNCTION(timezone_transitions_get);
PHP_FUNCTION(timezone_identifiers_list);
PHP_FUNCTION(timezone_abbreviations_list);

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

ZEND_BEGIN_MODULE_GLOBALS(date)
	char      *default_timezone;
	char      *timezone;
	HashTable  tzcache;
ZEND_END_MODULE_GLOBALS(date)

#ifdef ZTS
#define DATEG(v) TSRMG(date_globals_id, zend_date_globals *, v)
#else
#define DATEG(v) (date_globals.v)
#endif

/* Backwards compability wrapper */
PHPAPI signed long php_parse_date(char *string, signed long *now);
PHPAPI void php_mktime(INTERNAL_FUNCTION_PARAMETERS, int gmt);
PHPAPI int php_idate(char format, time_t ts, int localtime);
#if HAVE_STRFTIME
#define _php_strftime php_strftime
PHPAPI void php_strftime(INTERNAL_FUNCTION_PARAMETERS, int gm);
#endif
PHPAPI char *php_format_date(char *format, int format_len, time_t ts, int localtime TSRMLS_DC);

/* Mechanism to set new TZ database */
PHPAPI void php_date_set_tzdb(timelib_tzdb *tzdb);
PHPAPI timelib_tzinfo *get_timezone_info(TSRMLS_D);

#endif /* PHP_DATE_H */
