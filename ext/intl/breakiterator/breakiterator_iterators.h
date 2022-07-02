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
