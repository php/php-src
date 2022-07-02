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
   | Authors: Gustavo Lopes <cataphract@netcabo.pt>                       |
   +----------------------------------------------------------------------+
 */

#ifndef TRANSLITERATOR_CLASS_H
#define TRANSLITERATOR_CLASS_H

#include <php.h>

#include "intl_common.h"
#include "intl_error.h"

#include <unicode/utrans.h>

typedef struct {
	// 	error handling
	intl_error  err;

	// ICU transliterator
	UTransliterator* utrans;

	zend_object     zo;
} Transliterator_object;

static inline Transliterator_object *php_intl_transliterator_fetch_object(zend_object *obj) {
	return (Transliterator_object *)((char*)(obj) - XtOffsetOf(Transliterator_object, zo));
}
#define Z_INTL_TRANSLITERATOR_P(zv) php_intl_transliterator_fetch_object(Z_OBJ_P(zv))

#define TRANSLITERATOR_FORWARD UTRANS_FORWARD
#define TRANSLITERATOR_REVERSE UTRANS_REVERSE

#define TRANSLITERATOR_ERROR( co ) (co)->err
#define TRANSLITERATOR_ERROR_P( co ) &(TRANSLITERATOR_ERROR( co ))

#define TRANSLITERATOR_ERROR_CODE( co )   INTL_ERROR_CODE(TRANSLITERATOR_ERROR( co ))
#define TRANSLITERATOR_ERROR_CODE_P( co ) &(INTL_ERROR_CODE(TRANSLITERATOR_ERROR( co )))

#define TRANSLITERATOR_METHOD_INIT_VARS		         INTL_METHOD_INIT_VARS( Transliterator, to )
#define TRANSLITERATOR_METHOD_FETCH_OBJECT_NO_CHECK  INTL_METHOD_FETCH_OBJECT( INTL_TRANSLITERATOR, to )
#define TRANSLITERATOR_METHOD_FETCH_OBJECT\
	TRANSLITERATOR_METHOD_FETCH_OBJECT_NO_CHECK; \
	if( to->utrans == NULL ) \
	{ \
		zend_throw_error(NULL, "Found unconstructed transliterator"); \
		RETURN_THROWS(); \
	}

int transliterator_object_construct( zval *object,
									 UTransliterator *utrans,
									 UErrorCode *status );

void transliterator_register_Transliterator_class( void );

extern zend_class_entry *Transliterator_ce_ptr;
extern zend_object_handlers Transliterator_handlers;

#endif /* #ifndef TRANSLITERATOR_CLASS_H */
