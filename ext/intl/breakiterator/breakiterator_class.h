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

#ifndef BREAKITERATOR_CLASS_H
#define BREAKITERATOR_CLASS_H

//redefinition of inline in PHP headers causes problems, so include this before
#include <math.h>

#include <php.h>
#include "../intl_error.h"
#include "../intl_data.h"

#ifndef USE_BREAKITERATOR_POINTER
typedef void BreakIterator;
#endif

typedef struct {
	zend_object	zo;

	// 	error handling
	intl_error  err;

	// ICU break iterator
	BreakIterator*	biter;

	// current text
	zval *text;
} BreakIterator_object;

#define BREAKITER_ERROR(bio)		(bio)->err
#define BREAKITER_ERROR_P(bio)		&(BREAKITER_ERROR(bio))

#define BREAKITER_ERROR_CODE(bio)	INTL_ERROR_CODE(BREAKITER_ERROR(bio))
#define BREAKITER_ERROR_CODE_P(bio)	&(INTL_ERROR_CODE(BREAKITER_ERROR(bio)))

#define BREAKITER_METHOD_INIT_VARS		        INTL_METHOD_INIT_VARS(BreakIterator, bio)
#define BREAKITER_METHOD_FETCH_OBJECT_NO_CHECK	INTL_METHOD_FETCH_OBJECT(BreakIterator, bio)
#define BREAKITER_METHOD_FETCH_OBJECT \
	BREAKITER_METHOD_FETCH_OBJECT_NO_CHECK; \
	if (bio->biter == NULL) \
	{ \
		intl_errors_set(&bio->err, U_ILLEGAL_ARGUMENT_ERROR, "Found unconstructed BreakIterator", 0 TSRMLS_CC); \
		RETURN_FALSE; \
	}

void breakiterator_object_create(zval *object, BreakIterator *break_iter TSRMLS_DC);

void breakiterator_object_construct(zval *object, BreakIterator *break_iter TSRMLS_DC);

void breakiterator_register_BreakIterator_class(TSRMLS_D);

extern zend_class_entry *BreakIterator_ce_ptr,
						*RuleBasedBreakIterator_ce_ptr;

extern zend_object_handlers BreakIterator_handlers;

#endif /* #ifndef BREAKITERATOR_CLASS_H */
