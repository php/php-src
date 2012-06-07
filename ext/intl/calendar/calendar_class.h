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
#endif

typedef struct {
	zend_object	zo;

	// 	error handling
	intl_error  err;

	// ICU calendar
	Calendar*	ucal;
} Calendar_object;

#define CALENDAR_ERROR(co)		(co)->err
#define CALENDAR_ERROR_P(co)	&(CALENDAR_ERROR(co))

#define CALENDAR_ERROR_CODE(co)		INTL_ERROR_CODE(CALENDAR_ERROR(co))
#define CALENDAR_ERROR_CODE_P(co)	&(INTL_ERROR_CODE(CALENDAR_ERROR(co)))

#define CALENDAR_METHOD_INIT_VARS		        INTL_METHOD_INIT_VARS(Calendar, co)
#define CALENDAR_METHOD_FETCH_OBJECT_NO_CHECK	INTL_METHOD_FETCH_OBJECT(Calendar, co)
#define CALENDAR_METHOD_FETCH_OBJECT \
	CALENDAR_METHOD_FETCH_OBJECT_NO_CHECK; \
	if (co->ucal == NULL) \
	{ \
		intl_errors_set(&co->err, U_ILLEGAL_ARGUMENT_ERROR, "Found unconstructed IntlCalendar", 0 TSRMLS_CC); \
		RETURN_FALSE; \
	}

void calendar_object_create(zval *object, Calendar *calendar TSRMLS_DC);

Calendar *calendar_fetch_native_calendar(zval *object TSRMLS_DC);

void calendar_object_construct(zval *object, Calendar *calendar TSRMLS_DC);

void calendar_register_IntlCalendar_class(TSRMLS_D);

extern zend_class_entry *Calendar_ce_ptr,
						*GregorianCalendar_ce_ptr;

extern zend_object_handlers Calendar_handlers;

#endif /* #ifndef CALENDAR_CLASS_H */
