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
	zend_object		zo;
	intl_error		error;
} intl_object;

#define INTL_METHOD_INIT_VARS(oclass, obj)		\
	zval*             object  = NULL;			\
	oclass##_object*  obj     = NULL;			\
	intl_error_reset( NULL TSRMLS_CC );			

#define INTL_DATA_ERROR(obj)				(((intl_object *)(obj))->error)
#define INTL_DATA_ERROR_P(obj)				(&(INTL_DATA_ERROR((obj))))
#define INTL_DATA_ERROR_CODE(obj)			INTL_ERROR_CODE(INTL_DATA_ERROR((obj)))

#define INTL_METHOD_FETCH_OBJECT(oclass, obj)									\
	obj = (oclass##_object *) zend_object_store_get_object( object TSRMLS_CC );	\
    intl_error_reset( INTL_DATA_ERROR_P(obj) TSRMLS_CC );						\

#define INTL_METHOD_CHECK_STATUS(obj, msg)											\
    intl_error_set_code( NULL, INTL_DATA_ERROR_CODE((obj)) TSRMLS_CC );				\
    if( U_FAILURE( INTL_DATA_ERROR_CODE((obj)) ) )									\
    {																				\
        intl_errors_set_custom_msg( INTL_DATA_ERROR_P((obj)), msg, 0 TSRMLS_CC );	\
        RETURN_FALSE;										\
    }

#define INTL_MAX_LOCALE_LEN 64

// Check status, if error - destroy value and exit
#define INTL_CTOR_CHECK_STATUS(obj, msg)											\
    intl_error_set_code( NULL, INTL_DATA_ERROR_CODE((obj)) TSRMLS_CC );				\
    if( U_FAILURE( INTL_DATA_ERROR_CODE((obj)) ) )									\
    {																				\
        intl_errors_set_custom_msg( INTL_DATA_ERROR_P((obj)), msg, 0 TSRMLS_CC );	\
		zval_dtor(return_value);													\
        RETURN_NULL();																\
    }

#define INTL_CHECK_LOCALE_LEN(locale_len)												\
	if((locale_len) > INTL_MAX_LOCALE_LEN) {											\
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,									\
	"Locale string too long, should be no longer than 64 characters", 0 TSRMLS_CC );	\
		RETURN_NULL();																	\
	}

#define INTL_CHECK_LOCALE_LEN_OBJ(locale_len, object)									\
	if((locale_len) > INTL_MAX_LOCALE_LEN) {											\
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,									\
	"Locale string too long, should be no longer than 64 characters", 0 TSRMLS_CC );	\
		zval_dtor(object);																\
		ZVAL_NULL(object);																\
		RETURN_NULL();																	\
	}


#endif // INTL_DATA_H
