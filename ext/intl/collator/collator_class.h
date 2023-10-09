/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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
	// error handling
	intl_error  err;

	// ICU collator
	UCollator*      ucoll;

	zend_object     zo;

} Collator_object;

#define COLLATOR_ERROR(co) (co)->err
#define COLLATOR_ERROR_P(co) &(COLLATOR_ERROR(co))

#define COLLATOR_ERROR_CODE(co)   INTL_ERROR_CODE(COLLATOR_ERROR(co))
#define COLLATOR_ERROR_CODE_P(co) &(INTL_ERROR_CODE(COLLATOR_ERROR(co)))

static inline Collator_object *php_intl_collator_fetch_object(zend_object *obj) {
	return (Collator_object *)((char*)(obj) - XtOffsetOf(Collator_object, zo));
}
#define Z_INTL_COLLATOR_P(zv) php_intl_collator_fetch_object(Z_OBJ_P(zv))

void collator_register_Collator_symbols(int module_number);
void collator_object_init( Collator_object* co );
void collator_object_destroy( Collator_object* co );

extern zend_class_entry *Collator_ce_ptr;

/* Auxiliary macros */

#define COLLATOR_METHOD_INIT_VARS       \
    zval*             object  = NULL;   \
    Collator_object*  co      = NULL;   \
    intl_error_reset( NULL ); \

#define COLLATOR_METHOD_FETCH_OBJECT	INTL_METHOD_FETCH_OBJECT(INTL_COLLATOR, co)

// Macro to check return value of a ucol_* function call.
#define COLLATOR_CHECK_STATUS( co, msg )                                        \
    intl_error_set_code( NULL, COLLATOR_ERROR_CODE( co ) );           \
    if( U_FAILURE( COLLATOR_ERROR_CODE( co ) ) )                                \
    {                                                                           \
        intl_errors_set_custom_msg( COLLATOR_ERROR_P( co ), msg, 0 ); \
        RETURN_FALSE;                                                           \
    }                                                                           \

#endif // #ifndef COLLATOR_CLASS_H
