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

#include <unicode/brkiter.h>

#include "breakiterator_iterators.h"
#include "../common/common_enum.h"

extern "C" {
#define USE_BREAKITERATOR_POINTER
#include "breakiterator_class.h"
#include "../intl_convert.h"
#include "../locale/locale.h"
#include <zend_exceptions.h>
}

static zend_class_entry *IntlPartsIterator_ce_ptr;
static zend_object_handlers IntlPartsIterator_handlers;

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
	parts_iter_key_type key_type;
	BreakIterator_object *bio; /* so we don't have to fetch it all the time */
} zoi_break_iter_parts;

static void _breakiterator_parts_destroy_it(zend_object_iterator *iter TSRMLS_DC)
{
	zval_ptr_dtor(reinterpret_cast<zval**>(&iter->data));
}

static void _breakiterator_parts_get_current_key(zend_object_iterator *iter, zval *key TSRMLS_DC)
{
	/* the actual work is done in move_forward and rewind */
	ZVAL_LONG(key, iter->index);
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

	if (zoi_bit->key_type == PARTS_ITERATOR_KEY_LEFT) {
		iter->index = cur;
	} else if (zoi_bit->key_type == PARTS_ITERATOR_KEY_RIGHT) {
		iter->index = next;
	}
	/* else zoi_bit->key_type == PARTS_ITERATOR_KEY_SEQUENTIAL
	 * No need to do anything, the engine increments ->index */

	const char	*s = Z_STRVAL_P(bio->text);
	int32_t		slen = Z_STRLEN_P(bio->text),
				len;
	char		*res;

	if (next == BreakIterator::DONE) {
		next = slen;
	}
	assert(next <= slen && next >= cur);
	len = next - cur;
	res = static_cast<char*>(safe_emalloc(len, 1, 1));

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
	_breakiterator_parts_get_current_key,
	_breakiterator_parts_move_forward,
	_breakiterator_parts_rewind,
	zoi_with_current_invalidate_current
};

void IntlIterator_from_BreakIterator_parts(zval *break_iter_zv,
										   zval *object,
										   parts_iter_key_type key_type TSRMLS_DC)
{
	IntlIterator_object *ii;

	zval_add_ref(&break_iter_zv);

	object_init_ex(object, IntlPartsIterator_ce_ptr);
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
	((zoi_break_iter_parts*)ii->iterator)->key_type = key_type;
}

U_CFUNC zend_object_value IntlPartsIterator_object_create(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value		retval;

	retval = IntlIterator_ce_ptr->create_object(ce TSRMLS_CC);
	retval.handlers = &IntlPartsIterator_handlers;

	return retval;
}

U_CFUNC zend_function *IntlPartsIterator_get_method(zval **object_ptr,
		char *method, int method_len, const zend_literal *key TSRMLS_DC)
{
	zend_literal local_literal = {0};
	zend_function *ret;
	ALLOCA_FLAG(use_heap)

	if (key == NULL) {
		Z_STRVAL(local_literal.constant) = static_cast<char*>(
				do_alloca(method_len + 1, use_heap));
		zend_str_tolower_copy(Z_STRVAL(local_literal.constant),
				method, method_len);
		local_literal.hash_value = zend_hash_func(
				Z_STRVAL(local_literal.constant), method_len + 1);
		key = &local_literal;
	}

	if ((key->hash_value & 0xFFFFFFFF) == 0xA2B486A1 /* hash of getrulestatus\0 */
			&& method_len == sizeof("getrulestatus") - 1
			&& memcmp("getrulestatus", Z_STRVAL(key->constant),	method_len) == 0) {
		IntlIterator_object *obj = (IntlIterator_object*)
				zend_object_store_get_object(*object_ptr TSRMLS_CC);
		if (obj->iterator && obj->iterator->data) {
			zval *break_iter_zv = static_cast<zval*>(obj->iterator->data);
			*object_ptr = break_iter_zv;
			ret = Z_OBJ_HANDLER_P(break_iter_zv, get_method)(object_ptr,
					method, method_len, key TSRMLS_CC);
			goto end;
		}
	}

	ret = std_object_handlers.get_method(object_ptr,
			method, method_len, key TSRMLS_CC);

end:
	if (key == &local_literal) {
		free_alloca(Z_STRVAL(local_literal.constant), use_heap);
	}

	return ret;
}

U_CFUNC PHP_METHOD(IntlPartsIterator, getBreakIterator)
{
	INTLITERATOR_METHOD_INIT_VARS;

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"IntlPartsIterator::getBreakIterator: bad arguments", 0 TSRMLS_CC);
		return;
	}

	INTLITERATOR_METHOD_FETCH_OBJECT;

	zval *biter_zval = static_cast<zval*>(ii->iterator->data);
	RETURN_ZVAL(biter_zval, 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(ainfo_parts_it_void, 0, 0, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry IntlPartsIterator_class_functions[] = {
	PHP_ME(IntlPartsIterator,	getBreakIterator,	ainfo_parts_it_void,	ZEND_ACC_PUBLIC)
	PHP_FE_END
};

U_CFUNC void breakiterator_register_IntlPartsIterator_class(TSRMLS_D)
{
	zend_class_entry ce;

	/* Create and register 'BreakIterator' class. */
	INIT_CLASS_ENTRY(ce, "IntlPartsIterator", IntlPartsIterator_class_functions);
	IntlPartsIterator_ce_ptr = zend_register_internal_class_ex(&ce,
			IntlIterator_ce_ptr, NULL TSRMLS_CC);
	IntlPartsIterator_ce_ptr->create_object = IntlPartsIterator_object_create;

	memcpy(&IntlPartsIterator_handlers, &IntlIterator_handlers,
			sizeof IntlPartsIterator_handlers);
	IntlPartsIterator_handlers.get_method = IntlPartsIterator_get_method;

#define PARTSITER_DECL_LONG_CONST(name) \
	zend_declare_class_constant_long(IntlPartsIterator_ce_ptr, #name, \
		sizeof(#name) - 1, PARTS_ITERATOR_ ## name TSRMLS_CC)

	PARTSITER_DECL_LONG_CONST(KEY_SEQUENTIAL);
	PARTSITER_DECL_LONG_CONST(KEY_LEFT);
	PARTSITER_DECL_LONG_CONST(KEY_RIGHT);

#undef PARTSITER_DECL_LONG_CONST
}
