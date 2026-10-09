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
   | Authors: Sara Golemon <pollita@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_INTL_CONVERTER_CLASS_H
#define PHP_INTL_CONVERTER_CLASS_H

#include "php.h"
#include <unicode/ucnv.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "../intl_error.h"
#ifdef __cplusplus
}
#endif

typedef struct _php_converter_object {
	UConverter *src, *dest;
	zend_fcall_info_cache to_cache, from_cache;
	intl_error error;
	zend_object obj;
} php_converter_object;

static inline php_converter_object *php_converter_fetch_object(zend_object *obj) {
	return (php_converter_object *)((char*)(obj) - offsetof(php_converter_object, obj));
}
#define Z_INTL_CONVERTER_P(zv) php_converter_fetch_object(Z_OBJ_P(zv))

extern zend_class_entry *php_converter_ce;

bool php_converter_set_callbacks(php_converter_object *objval, UConverter *cnv);

#endif /* PHP_INTL_CONVERTER_CLASS_H */
