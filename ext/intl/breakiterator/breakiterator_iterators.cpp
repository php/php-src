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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "breakiterator_iterators.h"

extern "C" {
#define USE_BREAKITERATOR_POINTER
#include "breakiterator_class.h"
#include "../intl_convert.h"
#include "../locale/locale.h"
#include <zend_exceptions.h>
}

/* BreakIterator's iterator */

inline BreakIterator *_breakiter_prolog(zend_object_iterator *iter TSRMLS_DC)
{
	BreakIterator_object *bio;
	bio = (BreakIterator_object*)zend_object_store_get_object(
			(const zval*)iter->data TSRMLS_CC);
	intl_errors_reset(BREAKITER_ERROR_P(bio) TSRMLS_CC);
	if (bio->biter == NULL) {
		intl_errors_set(BREAKITER_ERROR_P(bio), U_INVALID_STATE_ERROR,
			"The BreakIterator object backing the PHP iterator is not "
			"properly constructed", 0 TSRMLS_CC);
	}
	return bio->biter;
}

static void _breakiterator_destroy_it(zend_object_iterator *iter TSRMLS_DC)
{
	zval_ptr_dtor((zval**)&iter->data);
}

static void _breakiterator_move_forward(zend_object_iterator *iter TSRMLS_DC)
{
	BreakIterator *biter = _breakiter_prolog(iter TSRMLS_CC);
	zoi_with_current *zoi_iter = (zoi_with_current*)iter;

	iter->funcs->invalidate_current(iter TSRMLS_CC);

	if (biter == NULL) {
		return;
	}

	int32_t pos = biter->next();
	if (pos != BreakIterator::DONE) {
		MAKE_STD_ZVAL(zoi_iter->current);
		ZVAL_LONG(zoi_iter->current, (long)pos);
	} //else we've reached the end of the enum, nothing more is required
}

static void _breakiterator_rewind(zend_object_iterator *iter TSRMLS_DC)
{
	BreakIterator *biter = _breakiter_prolog(iter TSRMLS_CC);
	zoi_with_current *zoi_iter = (zoi_with_current*)iter;

	int32_t pos = biter->first();
	MAKE_STD_ZVAL(zoi_iter->current);
	ZVAL_LONG(zoi_iter->current, (long)pos);
}

static zend_object_iterator_funcs breakiterator_iterator_funcs = {
	zoi_with_current_dtor,
	zoi_with_current_valid,
	zoi_with_current_get_current_data,
	NULL,
	_breakiterator_move_forward,
	_breakiterator_rewind,
	zoi_with_current_invalidate_current
};

U_CFUNC zend_object_iterator *_breakiterator_get_iterator(
	zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC)
{
	BreakIterator_object *bio;
	if (by_ref) {
		zend_throw_exception(NULL,
			"Iteration by reference is not supported", 0 TSRMLS_CC);
		return NULL;
	}

	bio = (BreakIterator_object*)zend_object_store_get_object(object TSRMLS_CC);
	BreakIterator *biter = bio->biter;

	if (biter == NULL) {
		zend_throw_exception(NULL,
			"The BreakIterator is not properly constructed", 0 TSRMLS_CC);
		return NULL;
	}

	zoi_with_current *zoi_iter =
		static_cast<zoi_with_current*>(emalloc(sizeof *zoi_iter));
	zoi_iter->zoi.data = static_cast<void*>(object);
	zoi_iter->zoi.funcs = &breakiterator_iterator_funcs;
	zoi_iter->zoi.index = 0;
	zoi_iter->destroy_it = _breakiterator_destroy_it;
	zoi_iter->wrapping_obj = NULL; /* not used; object is in zoi.data */
	zoi_iter->current = NULL;

	zval_add_ref(&object);

	return reinterpret_cast<zend_object_iterator *>(zoi_iter);
}

/* BreakIterator parts iterator */

typedef struct zoi_break_iter_parts {
	zoi_with_current zoi_cur;
	BreakIterator_object *bio; /* so we don't have to fetch it all the time */
} zoi_break_iter_parts;

static void _breakiterator_parts_destroy_it(zend_object_iterator *iter TSRMLS_DC)
{
	zval_ptr_dtor(reinterpret_cast<zval**>(&iter->data));
}

static void _breakiterator_parts_move_forward(zend_object_iterator *iter TSRMLS_DC)
{
	zoi_break_iter_parts *zoi_bit = (zoi_break_iter_parts*)iter;
	BreakIterator_object *bio = zoi_bit->bio;

	iter->funcs->invalidate_current(iter TSRMLS_CC);

	int32_t cur,
			next;

	cur = bio->biter->current();
	if (cur == BreakIterator::DONE) {
		return;
	}
	next = bio->biter->next();
	if (next == BreakIterator::DONE) {
		return;
	}

	const char	*s = Z_STRVAL_P(bio->text);
	int32_t		slen = Z_STRLEN_P(bio->text),
				len;
	char		*res;

	if (next == BreakIterator::DONE) {
		next = slen;
	}
	assert(next <= slen && next >= cur);
	len = next - cur;
	res = static_cast<char*>(emalloc(len + 1));

	memcpy(res, &s[cur], len);
	res[len] = '\0';

	MAKE_STD_ZVAL(zoi_bit->zoi_cur.current);
	ZVAL_STRINGL(zoi_bit->zoi_cur.current, res, len, 0);
}

static void _breakiterator_parts_rewind(zend_object_iterator *iter TSRMLS_DC)
{
	zoi_break_iter_parts *zoi_bit = (zoi_break_iter_parts*)iter;
	BreakIterator_object *bio = zoi_bit->bio;

	if (zoi_bit->zoi_cur.current) {
		iter->funcs->invalidate_current(iter TSRMLS_CC);
	}

	bio->biter->first();

	iter->funcs->move_forward(iter TSRMLS_CC);
}

static zend_object_iterator_funcs breakiterator_parts_it_funcs = {
	zoi_with_current_dtor,
	zoi_with_current_valid,
	zoi_with_current_get_current_data,
	NULL,
	_breakiterator_parts_move_forward,
	_breakiterator_parts_rewind,
	zoi_with_current_invalidate_current
};

void IntlIterator_from_BreakIterator_parts(zval *break_iter_zv,
										   zval *object TSRMLS_DC)
{
	IntlIterator_object *ii;

	zval_add_ref(&break_iter_zv);

	object_init_ex(object, IntlIterator_ce_ptr);
	ii = (IntlIterator_object*)zend_object_store_get_object(object TSRMLS_CC);

	ii->iterator = (zend_object_iterator*)emalloc(sizeof(zoi_break_iter_parts));
	ii->iterator->data = break_iter_zv;
	ii->iterator->funcs = &breakiterator_parts_it_funcs;
	ii->iterator->index = 0;
	((zoi_with_current*)ii->iterator)->destroy_it = _breakiterator_parts_destroy_it;
	((zoi_with_current*)ii->iterator)->wrapping_obj = object;
	((zoi_with_current*)ii->iterator)->current = NULL;

	((zoi_break_iter_parts*)ii->iterator)->bio = (BreakIterator_object*)
		zend_object_store_get_object(break_iter_zv TSRMLS_CC);
	assert(((zoi_break_iter_parts*)ii->iterator)->bio->biter != NULL);
}
