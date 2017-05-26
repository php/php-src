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

/* Check status by error code, if error return false */
#define INTL_CHECK_STATUS(err, msg)											\
    intl_error_set_code( NULL, (err) );							\
    if( U_FAILURE((err)) )													\
    {																		\
        intl_error_set_custom_msg( NULL, msg, 0 );				\
        RETURN_FALSE;														\
    }

/* Check status by error code, if error return null */
#define INTL_CHECK_STATUS_OR_NULL(err, msg)                     \
    intl_error_set_code( NULL, (err) );             \
    if( U_FAILURE((err)) )                          \
    {                                   \
        intl_error_set_custom_msg( NULL, msg, 0 );        \
        RETURN_NULL();                           \
    }


/* Check status in object, if error return false */
#define INTL_METHOD_CHECK_STATUS(obj, msg)											\
    intl_error_set_code( NULL, INTL_DATA_ERROR_CODE((obj)) );				\
    if( U_FAILURE( INTL_DATA_ERROR_CODE((obj)) ) )									\
    {																				\
        intl_errors_set_custom_msg( INTL_DATA_ERROR_P((obj)), msg, 0 );	\
        RETURN_FALSE;										\
    }

/* Check status in object, if error return null */
#define INTL_METHOD_CHECK_STATUS_OR_NULL(obj, msg)									\
    intl_error_set_code( NULL, INTL_DATA_ERROR_CODE((obj)) );						\
    if( U_FAILURE( INTL_DATA_ERROR_CODE((obj)) ) )									\
    {																				\
        intl_errors_set_custom_msg( INTL_DATA_ERROR_P((obj)), msg, 0 );				\
        zval_ptr_dtor(return_value);												\
        RETURN_NULL();																\
    }

/* Check status in object, if error return FAILURE */
#define INTL_CTOR_CHECK_STATUS(obj, msg)											\
    intl_error_set_code( NULL, INTL_DATA_ERROR_CODE((obj)) );						\
    if( U_FAILURE( INTL_DATA_ERROR_CODE((obj)) ) )									\
    {																				\
        intl_errors_set_custom_msg( INTL_DATA_ERROR_P((obj)), msg, 0 );				\
        return FAILURE;																\
    }

#define INTL_METHOD_RETVAL_UTF8(obj, ustring, ulen, free_it)									\
{																								\
	zend_string *u8str;																			\
	u8str = intl_convert_utf16_to_utf8(ustring, ulen, &INTL_DATA_ERROR_CODE((obj)));			\
	if((free_it)) {																				\
		efree(ustring);																			\
	}																							\
	INTL_METHOD_CHECK_STATUS((obj), "Error converting value to UTF-8");							\
	RETVAL_NEW_STR(u8str);																		\
}

#define INTL_MAX_LOCALE_LEN 80

#define INTL_CHECK_LOCALE_LEN(locale_len)												\
	if((locale_len) > INTL_MAX_LOCALE_LEN) {											\
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,									\
		"Locale string too long, should be no longer than 80 characters", 0 );			\
		RETURN_NULL();																	\
	}

#define INTL_CHECK_LOCALE_LEN_OR_FAILURE(locale_len)									\
	if((locale_len) > INTL_MAX_LOCALE_LEN) {											\
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,									\
		"Locale string too long, should be no longer than 80 characters", 0 );			\
		return FAILURE;																	\
	}

#endif // INTL_DATA_H
