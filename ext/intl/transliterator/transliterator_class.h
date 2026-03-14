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

#ifndef TRANSLITERATOR_CLASS_H
#define TRANSLITERATOR_CLASS_H

#include <php.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "intl_common.h"
#include "intl_error.h"
#ifdef __cplusplus
}
#endif

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

#ifdef __cplusplus
extern "C" {
#endif
int transliterator_object_construct( zval *object,
									 UTransliterator *utrans,
									 UErrorCode *status );

void transliterator_register_Transliterator_class( void );

extern zend_class_entry *Transliterator_ce_ptr;
extern zend_object_handlers Transliterator_handlers;
#ifdef __cplusplus
}
#endif

#endif /* #ifndef TRANSLITERATOR_CLASS_H */
