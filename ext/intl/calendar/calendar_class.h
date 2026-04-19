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
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
 */

#ifndef CALENDAR_CLASS_H
#define CALENDAR_CLASS_H

//redefinition of inline in PHP headers causes problems, so include this before
#include <math.h>

#include <php.h>
#include "intl_error.h"
#include "intl_data.h"

#ifndef USE_CALENDAR_POINTER
typedef void Calendar;
#else
using icu::Calendar;
#endif

typedef struct {
	// 	error handling
	intl_error  err;

	// ICU calendar
	Calendar*	ucal;

	zend_object	zo;
} Calendar_object;

static inline Calendar_object *php_intl_calendar_fetch_object(zend_object *obj) {
	return (Calendar_object *)((char*)(obj) - XtOffsetOf(Calendar_object, zo));
}
#define Z_INTL_CALENDAR_P(zv) php_intl_calendar_fetch_object(Z_OBJ_P(zv))

#define CALENDAR_ERROR(co)		(co)->err
#define CALENDAR_ERROR_P(co)	&(CALENDAR_ERROR(co))

#define CALENDAR_ERROR_CODE(co)		INTL_ERROR_CODE(CALENDAR_ERROR(co))
#define CALENDAR_ERROR_CODE_P(co)	&(INTL_ERROR_CODE(CALENDAR_ERROR(co)))

#define CALENDAR_METHOD_INIT_VARS		        INTL_METHOD_INIT_VARS(Calendar, co)
#define CALENDAR_METHOD_FETCH_OBJECT_NO_CHECK	INTL_METHOD_FETCH_OBJECT(INTL_CALENDAR, co)
#define CALENDAR_METHOD_FETCH_OBJECT \
	CALENDAR_METHOD_FETCH_OBJECT_NO_CHECK; \
	if (co->ucal == NULL) \
	{ \
		zend_throw_error(NULL, "Found unconstructed IntlCalendar"); \
		RETURN_THROWS(); \
	}

void calendar_object_create(zval *object, Calendar *calendar);

Calendar *calendar_fetch_native_calendar(zend_object *object);

void calendar_object_construct(zval *object, Calendar *calendar);

void calendar_register_IntlCalendar_class(void);

extern zend_class_entry *Calendar_ce_ptr, *GregorianCalendar_ce_ptr;

extern zend_object_handlers Calendar_handlers;

#endif /* #ifndef CALENDAR_CLASS_H */
