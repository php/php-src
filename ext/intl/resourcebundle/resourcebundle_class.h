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
   | Authors: Hans-Peter Oeri (University of St.Gallen) <hp@oeri.ch>      |
   +----------------------------------------------------------------------+
 */

#ifndef RESOURCEBUNDLE_CLASS_H
#define RESOURCEBUNDLE_CLASS_H

#include <unicode/ures.h>

#include <zend.h>
#ifdef __cplusplus
extern "C" {
#endif
#include "php.h"
#include "intl_error.h"
#ifdef __cplusplus
}
#endif

typedef struct {
	intl_error      error;

	UResourceBundle *me;
	UResourceBundle *child;
	zend_object     zend;
} ResourceBundle_object;

static inline ResourceBundle_object *php_intl_resourcebundle_fetch_object(zend_object *obj) {
	return (ResourceBundle_object *)((char*)(obj) - XtOffsetOf(ResourceBundle_object, zend));
}
#define Z_INTL_RESOURCEBUNDLE_P(zv) php_intl_resourcebundle_fetch_object(Z_OBJ_P(zv))

#define RESOURCEBUNDLE_METHOD_INIT_VARS		INTL_METHOD_INIT_VARS(ResourceBundle, rb)
#define RESOURCEBUNDLE_METHOD_FETCH_OBJECT_NO_CHECK	INTL_METHOD_FETCH_OBJECT(INTL_RESOURCEBUNDLE, rb)
#define RESOURCEBUNDLE_METHOD_FETCH_OBJECT							\
	INTL_METHOD_FETCH_OBJECT(INTL_RESOURCEBUNDLE, rb);				\
	if (RESOURCEBUNDLE_OBJECT(rb) == NULL) {						\
		zend_throw_error(NULL, "Found unconstructed ResourceBundle");	\
		RETURN_THROWS();											\
	}

#define RESOURCEBUNDLE_OBJECT(rb)			(rb)->me

#ifdef __cplusplus
extern "C" {
#endif
void resourcebundle_register_class( void );
extern zend_class_entry *ResourceBundle_ce_ptr;
#ifdef __cplusplus
}
#endif

#endif // #ifndef RESOURCEBUNDLE_CLASS_H
