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
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
*/
#ifndef INTL_BREAKITERATOR_ITERATORS_H
#define INTL_BREAKITERATOR_ITERATORS_H

#ifndef __cplusplus
#error Header for C++ only
#endif

#include <unicode/brkiter.h>
#include <unicode/umachine.h>

#include "../common/common_enum.h"

extern "C" {
#include <math.h>
#include <php.h>
}

void IntlIterator_from_BreakIterator_parts(zval *break_iter_zv,
										   zval *object TSRMLS_DC);

U_CFUNC zend_object_iterator *_breakiterator_get_iterator(
	zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC);

#endif