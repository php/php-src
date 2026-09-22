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

#include "codepointiterator_internal.h"

extern "C" {
#define USE_BREAKITERATOR_POINTER 1
#include "breakiterator_class.h"
}

using PHP::CodePointBreakIterator;

static inline CodePointBreakIterator *fetch_cpbi(BreakIterator_object *bio) {
	return (CodePointBreakIterator*)bio->biter;
}

U_CFUNC PHP_METHOD(IntlCodePointBreakIterator, getLastCodePoint)
{
	BREAKITER_METHOD_INIT_VARS;
	object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	BREAKITER_METHOD_FETCH_OBJECT;

	RETURN_LONG(fetch_cpbi(bio)->getLastCodePoint());
}
