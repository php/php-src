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
   | Authors: Scott MacVicar <scottmac@php.net>                           |
   +----------------------------------------------------------------------+
 */

#ifndef SPOOFCHECKER_CLASS_H
#define SPOOFCHECKER_CLASS_H

#include <php.h>

#include "intl_common.h"
#include "spoofchecker_create.h"
#include "intl_error.h"

#include <unicode/uspoof.h>

typedef struct {
	zend_object     zo;

	// error handling
	intl_error  err;

	// ICU Spoofchecker
	USpoofChecker*      uspoof;
} Spoofchecker_object;

#define SPOOFCHECKER_ERROR(co) (co)->err
#define SPOOFCHECKER_ERROR_P(co) &(SPOOFCHECKER_ERROR(co))

#define SPOOFCHECKER_ERROR_CODE(co)   INTL_ERROR_CODE(SPOOFCHECKER_ERROR(co))
#define SPOOFCHECKER_ERROR_CODE_P(co) &(INTL_ERROR_CODE(SPOOFCHECKER_ERROR(co)))

void spoofchecker_register_Spoofchecker_class(TSRMLS_D);

void spoofchecker_object_init(Spoofchecker_object* co TSRMLS_DC);
void spoofchecker_object_destroy(Spoofchecker_object* co TSRMLS_DC);

extern zend_class_entry *Spoofchecker_ce_ptr;

/* Auxiliary macros */

#define SPOOFCHECKER_METHOD_INIT_VARS       \
    zval*             object  = getThis();   \
    Spoofchecker_object*  co  = NULL;   \
    intl_error_reset(NULL TSRMLS_CC); \

#define SPOOFCHECKER_METHOD_FETCH_OBJECT                                           \
    co = (Spoofchecker_object *) zend_object_store_get_object(object TSRMLS_CC); \
    intl_error_reset(SPOOFCHECKER_ERROR_P(co) TSRMLS_CC);                      \

// Macro to check return value of a ucol_* function call.
#define SPOOFCHECKER_CHECK_STATUS(co, msg)                                        \
    intl_error_set_code(NULL, SPOOFCHECKER_ERROR_CODE(co) TSRMLS_CC);           \
    if (U_FAILURE(SPOOFCHECKER_ERROR_CODE(co))) {                                  \
        intl_errors_set_custom_msg(SPOOFCHECKER_ERROR_P(co), msg, 0 TSRMLS_CC); \
        RETURN_FALSE;                                                           \
    }                                                                           \

#endif // #ifndef SPOOFCHECKER_CLASS_H
