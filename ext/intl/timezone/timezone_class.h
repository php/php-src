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
#else
using icu::TimeZone;
#endif

typedef struct {
	// 	error handling
	intl_error		err;

	// ICU TimeZone
	// TODO?: a direct change isn't possible due to C inclusion (also it s a const)
	// but see later it can be made possible through different ICU class usages
	const TimeZone	*utimezone;

	//whether to delete the timezone on object free
	bool		should_delete;

	zend_object		zo;
} TimeZone_object;

static inline TimeZone_object *php_intl_timezone_fetch_object(zend_object *obj) {
	return (TimeZone_object *)((char*)(obj) - XtOffsetOf(TimeZone_object, zo));
}
#define Z_INTL_TIMEZONE_P(zv) php_intl_timezone_fetch_object(Z_OBJ_P(zv))

#define TIMEZONE_ERROR(to)						(to)->err
#define TIMEZONE_ERROR_P(to)					&(TIMEZONE_ERROR(to))

#define TIMEZONE_ERROR_CODE(co)					INTL_ERROR_CODE(TIMEZONE_ERROR(to))
#define TIMEZONE_ERROR_CODE_P(co)				&(INTL_ERROR_CODE(TIMEZONE_ERROR(to)))

#define TIMEZONE_METHOD_INIT_VARS				INTL_METHOD_INIT_VARS(TimeZone, to)
#define TIMEZONE_METHOD_FETCH_OBJECT_NO_CHECK	INTL_METHOD_FETCH_OBJECT(INTL_TIMEZONE, to)
#define TIMEZONE_METHOD_FETCH_OBJECT\
	TIMEZONE_METHOD_FETCH_OBJECT_NO_CHECK; \
	if (to->utimezone == NULL) { \
		zend_throw_error(NULL, "Found unconstructed IntlTimeZone"); \
		RETURN_THROWS(); \
	}

zval *timezone_convert_to_datetimezone(const TimeZone *timeZone, intl_error *outside_error, zval *ret);
TimeZone *timezone_process_timezone_argument(zend_object *timezone_object, zend_string *timezone_string, intl_error *error);

void timezone_object_construct(const TimeZone *zone, zval *object, int owned);

void timezone_register_IntlTimeZone_class(void);

extern zend_class_entry *TimeZone_ce_ptr;
extern zend_object_handlers TimeZone_handlers;

#endif /* #ifndef TIMEZONE_CLASS_H */
