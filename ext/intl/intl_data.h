/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   |          Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifndef INTL_DATA_H
#define INTL_DATA_H

#include <unicode/utypes.h>

#include "intl_error.h"

/* Mock object to generalize error handling in sub-modules.
   Sub-module data structures should always have error as first element
   for this to work!
*/
typedef struct _intl_data {
	intl_error		error;
	zend_object		zo;
} intl_object;

#define INTL_METHOD_INIT_VARS(oclass, obj)		\
	zval*             object  = NULL;			\
	oclass##_object*  obj     = NULL;			\
	intl_error_reset( NULL );

#define INTL_DATA_ERROR(obj)				(((intl_object *)(obj))->error)
#define INTL_DATA_ERROR_P(obj)				(&(INTL_DATA_ERROR((obj))))
#define INTL_DATA_ERROR_CODE(obj)			INTL_ERROR_CODE(INTL_DATA_ERROR((obj)))

#define INTL_METHOD_FETCH_OBJECT(oclass, obj)									\
	obj = Z_##oclass##_P( object );												\
    intl_error_reset( INTL_DATA_ERROR_P(obj) );						\

/* Check status by error code, if error - exit */
#define INTL_CHECK_STATUS(err, msg)											\
    intl_error_set_code( NULL, (err) );							\
    if( U_FAILURE((err)) )													\
    {																		\
        intl_error_set_custom_msg( NULL, msg, 0 );				\
        RETURN_FALSE;														\
    }

/* Check status in object, if error - exit */
#define INTL_METHOD_CHECK_STATUS(obj, msg)											\
    intl_error_set_code( NULL, INTL_DATA_ERROR_CODE((obj)) );				\
    if( U_FAILURE( INTL_DATA_ERROR_CODE((obj)) ) )									\
    {																				\
        intl_errors_set_custom_msg( INTL_DATA_ERROR_P((obj)), msg, 0 );	\
        RETURN_FALSE;										\
    }

/* Check status, if error - destroy value and exit */
#define INTL_CTOR_CHECK_STATUS(obj, msg)											\
    intl_error_set_code( NULL, INTL_DATA_ERROR_CODE((obj)) );				\
    if( U_FAILURE( INTL_DATA_ERROR_CODE((obj)) ) )									\
    {																				\
        intl_errors_set_custom_msg( INTL_DATA_ERROR_P((obj)), msg, 0 );	\
		/* yes, this is ugly, but it alreay is */									\
		if (return_value != getThis()) {											\
			zval_dtor(return_value);												\
			RETURN_NULL();															\
		}																			\
		Z_OBJ_P(return_value) = NULL;												\
		return;																		\
    }

#define INTL_METHOD_RETVAL_UTF8(obj, ustring, ulen, free_it)									\
{																								\
	char *u8value;																				\
	size_t u8len;																				\
	intl_convert_utf16_to_utf8(&u8value, &u8len, ustring, ulen, &INTL_DATA_ERROR_CODE((obj)));	\
	if((free_it)) {																				\
		efree(ustring);																			\
	}																							\
	INTL_METHOD_CHECK_STATUS((obj), "Error converting value to UTF-8");							\
	RETVAL_STRINGL(u8value, u8len);																\
	efree(u8value);																				\
}

#define INTL_MAX_LOCALE_LEN 80

#define INTL_CHECK_LOCALE_LEN(locale_len)												\
	if((locale_len) > INTL_MAX_LOCALE_LEN) {											\
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,									\
		"Locale string too long, should be no longer than 80 characters", 0 );			\
		RETURN_NULL();																	\
	}

#define INTL_CHECK_LOCALE_LEN_OBJ(locale_len, object)									\
	if((locale_len) > INTL_MAX_LOCALE_LEN) {											\
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,									\
		"Locale string too long, should be no longer than 80 characters", 0 );			\
		zval_dtor(object);																\
		ZVAL_NULL(object);																\
		RETURN_NULL();																	\
	}


#endif // INTL_DATA_H
