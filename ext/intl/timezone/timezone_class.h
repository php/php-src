/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Gustavo Lopes <cataphract@netcabo.pt>                       |
   +----------------------------------------------------------------------+
 */

#ifndef TIMEZONE_CLASS_H
#define TIMEZONE_CLASS_H

//redefinition of inline in PHP headers causes problems, so include this before
#include <math.h>

//fixes the build on windows for old versions of ICU
#include <stdio.h>

#include <php.h>
#include "intl_error.h"
#include "intl_data.h"

#ifndef USE_TIMEZONE_POINTER
typedef void TimeZone;
#endif

typedef struct {
	zend_object		zo;

	// 	error handling
	intl_error		err;

	// ICU TimeZone
	const TimeZone	*utimezone;

	//whether to delete the timezone on object free
	zend_bool		should_delete;
} TimeZone_object;

#define TIMEZONE_ERROR(to)						(to)->err
#define TIMEZONE_ERROR_P(to)					&(TIMEZONE_ERROR(to))

#define TIMEZONE_ERROR_CODE(co)					INTL_ERROR_CODE(TIMEZONE_ERROR(to))
#define TIMEZONE_ERROR_CODE_P(co)				&(INTL_ERROR_CODE(TIMEZONE_ERROR(to)))

#define TIMEZONE_METHOD_INIT_VARS				INTL_METHOD_INIT_VARS(TimeZone, to)
#define TIMEZONE_METHOD_FETCH_OBJECT_NO_CHECK	INTL_METHOD_FETCH_OBJECT(TimeZone, to)
#define TIMEZONE_METHOD_FETCH_OBJECT\
	TIMEZONE_METHOD_FETCH_OBJECT_NO_CHECK; \
	if (to->utimezone == NULL) { \
		intl_errors_set(&to->err, U_ILLEGAL_ARGUMENT_ERROR, "Found unconstructed IntlTimeZone", 0 TSRMLS_CC); \
		RETURN_FALSE; \
	}

zval *timezone_convert_to_datetimezone(const TimeZone *timeZone, intl_error *outside_error, const char *func TSRMLS_DC);
TimeZone *timezone_process_timezone_argument(zval **zv_timezone, intl_error *error, const char *func TSRMLS_DC);

void timezone_object_construct(const TimeZone *zone, zval *object, int owned TSRMLS_DC);

void timezone_register_IntlTimeZone_class(TSRMLS_D);

extern zend_class_entry *TimeZone_ce_ptr;
extern zend_object_handlers TimeZone_handlers;

#endif /* #ifndef TIMEZONE_CLASS_H */
