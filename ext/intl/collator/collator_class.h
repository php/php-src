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
   +----------------------------------------------------------------------+
 */

#ifndef COLLATOR_CLASS_H
#define COLLATOR_CLASS_H

#include <php.h>

#include "../intl_common.h"
#include "../intl_error.h"
#include "../intl_data.h"

#include <unicode/ucol.h>

typedef struct {
	zend_object     zo;

	// error handling
	intl_error  err;

	// ICU collator
	UCollator*      ucoll;
} Collator_object;

#define COLLATOR_ERROR(co) (co)->err
#define COLLATOR_ERROR_P(co) &(COLLATOR_ERROR(co))

#define COLLATOR_ERROR_CODE(co)   INTL_ERROR_CODE(COLLATOR_ERROR(co))
#define COLLATOR_ERROR_CODE_P(co) &(INTL_ERROR_CODE(COLLATOR_ERROR(co)))

void collator_register_Collator_class( TSRMLS_D );
void collator_object_init( Collator_object* co TSRMLS_DC );
void collator_object_destroy( Collator_object* co TSRMLS_DC );

extern zend_class_entry *Collator_ce_ptr;

/* Auxiliary macros */

#define COLLATOR_METHOD_INIT_VARS       \
    zval*             object  = NULL;   \
    Collator_object*  co      = NULL;   \
    intl_error_reset( NULL TSRMLS_CC ); \

#define COLLATOR_METHOD_FETCH_OBJECT	INTL_METHOD_FETCH_OBJECT(Collator, co)

// Macro to check return value of a ucol_* function call.
#define COLLATOR_CHECK_STATUS( co, msg )                                        \
    intl_error_set_code( NULL, COLLATOR_ERROR_CODE( co ) TSRMLS_CC );           \
    if( U_FAILURE( COLLATOR_ERROR_CODE( co ) ) )                                \
    {                                                                           \
        intl_errors_set_custom_msg( COLLATOR_ERROR_P( co ), msg, 0 TSRMLS_CC ); \
        RETURN_FALSE;                                                           \
    }                                                                           \

#endif // #ifndef COLLATOR_CLASS_H
