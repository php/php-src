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
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
*/
#ifndef INTL_BREAKITERATOR_ITERATORS_H
#define INTL_BREAKITERATOR_ITERATORS_H

#include <unicode/umachine.h>

U_CDECL_BEGIN
#include <math.h>
#include <php.h>
U_CDECL_END

typedef enum {
	PARTS_ITERATOR_KEY_SEQUENTIAL,
	PARTS_ITERATOR_KEY_LEFT,
	PARTS_ITERATOR_KEY_RIGHT,
} parts_iter_key_type;

#ifdef __cplusplus
void IntlIterator_from_BreakIterator_parts(zval *break_iter_zv,
										   zval *object,
										   parts_iter_key_type key_type);
#endif

U_CFUNC zend_object_iterator *_breakiterator_get_iterator(
		zend_class_entry *ce, zval *object, int by_ref);
U_CFUNC void breakiterator_register_IntlPartsIterator_class(void);

#endif
