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
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifndef FORMATTER_CLASS_H
#define FORMATTER_CLASS_H

#include <php.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "intl_common.h"
#include "intl_error.h"
#include "intl_data.h"
#include "formatter_data.h"
#ifdef __cplusplus
}
#endif

typedef struct {
	formatter_data  nf_data;
	zend_object     zo;
} NumberFormatter_object;

static inline NumberFormatter_object *php_intl_number_format_fetch_object(zend_object *obj) {
	return (NumberFormatter_object *)((char*)(obj) - XtOffsetOf(NumberFormatter_object, zo));
}
#define Z_INTL_NUMBERFORMATTER_P(zv) php_intl_number_format_fetch_object(Z_OBJ_P(zv))

#ifdef __cplusplus
extern "C" {
#endif
void formatter_register_class( void );
extern zend_class_entry *NumberFormatter_ce_ptr;
#ifdef __cplusplus
}
#endif

/* Auxiliary macros */

#define FORMATTER_METHOD_INIT_VARS				INTL_METHOD_INIT_VARS(NumberFormatter, nfo)
#define FORMATTER_OBJECT(nfo)					(nfo)->nf_data.unum
#define FORMATTER_METHOD_FETCH_OBJECT_NO_CHECK	INTL_METHOD_FETCH_OBJECT(INTL_NUMBERFORMATTER, nfo)
#define FORMATTER_METHOD_FETCH_OBJECT \
	FORMATTER_METHOD_FETCH_OBJECT_NO_CHECK; \
	if (FORMATTER_OBJECT(nfo) == NULL) \
	{ \
		zend_throw_error(NULL, "Found unconstructed NumberFormatter"); \
		RETURN_THROWS(); \
	}


#endif // #ifndef FORMATTER_CLASS_H
