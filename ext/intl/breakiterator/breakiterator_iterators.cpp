/*
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
#include "breakiterator_arginfo.h"
#include "../intl_convert.h"
#include "../locale/locale.h"
#include <zend_exceptions.h>
}

static zend_class_entry *IntlPartsIterator_ce_ptr;
static zend_object_handlers IntlPartsIterator_handlers;

/* BreakIterator's iterator */

inline BreakIterator *_breakiter_prolog(zend_object_iterator *iter)
{
	BreakIterator_object *bio;
	bio = Z_INTL_BREAKITERATOR_P(&iter->data);
	intl_errors_reset(BREAKITER_ERROR_P(bio));
	if (bio->biter == NULL) {
		intl_errors_set(BREAKITER_ERROR_P(bio), U_INVALID_STATE_ERROR,
			"The BreakIterator object backing the PHP iterator is not "
			"properly constructed", 0);
	}
	return bio->biter;
}

static void _breakiterator_destroy_it(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void _breakiterator_move_forward(zend_object_iterator *iter)
{
	BreakIterator *biter = _breakiter_prolog(iter);
	zoi_with_current *zoi_iter = (zoi_with_current*)iter;

	iter->funcs->invalidate_current(iter);

	if (biter == NULL) {
		return;
	}

	int32_t pos = biter->next();
	if (pos != BreakIterator::DONE) {
		ZVAL_LONG(&zoi_iter->current, (zend_long)pos);
	} //else we've reached the end of the enum, nothing more is required
}

static void _breakiterator_rewind(zend_object_iterator *iter)
{
	BreakIterator *biter = _breakiter_prolog(iter);
	zoi_with_current *zoi_iter = (zoi_with_current*)iter;

	int32_t pos = biter->first();
	ZVAL_LONG(&zoi_iter->current, (zend_long)pos);
}

static const zend_object_iterator_funcs breakiterator_iterator_funcs = {
	zoi_with_current_dtor,
	zoi_with_current_valid,
	zoi_with_current_get_current_data,
	NULL,
	_breakiterator_move_forward,
	_breakiterator_rewind,
	zoi_with_current_invalidate_current
};

U_CFUNC zend_object_iterator *_breakiterator_get_iterator(
	zend_class_entry *ce, zval *object, int by_ref)
{
	BreakIterator_object *bio;
	if (by_ref) {
		zend_throw_exception(NULL,
			"Iteration by reference is not supported", 0);
		return NULL;
	}

	bio = Z_INTL_BREAKITERATOR_P(object);
	BreakIterator *biter = bio->biter;

	if (biter == NULL) {
		zend_throw_exception(NULL,
			"The BreakIterator is not properly constructed", 0);
		return NULL;
	}

	zoi_with_current *zoi_iter = static_cast<zoi_with_current*>(emalloc(sizeof *zoi_iter));
	zend_iterator_init(&zoi_iter->zoi);
	ZVAL_OBJ_COPY(&zoi_iter->zoi.data, Z_OBJ_P(object));
	zoi_iter->zoi.funcs = &breakiterator_iterator_funcs;
	zoi_iter->zoi.index = 0;
	zoi_iter->destroy_it = _breakiterator_destroy_it;
	ZVAL_UNDEF(&zoi_iter->wrapping_obj); /* not used; object is in zoi.data */
	ZVAL_UNDEF(&zoi_iter->current);

	return reinterpret_cast<zend_object_iterator *>(zoi_iter);
}

/* BreakIterator parts iterator */

typedef struct zoi_break_iter_parts {
	zoi_with_current zoi_cur;
	parts_iter_key_type key_type;
	BreakIterator_object *bio; /* so we don't have to fetch it all the time */
} zoi_break_iter_parts;

static void _breakiterator_parts_destroy_it(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void _breakiterator_parts_get_current_key(zend_object_iterator *iter, zval *key)
{
	/* the actual work is done in move_forward and rewind */
	ZVAL_LONG(key, iter->index);
}

static void _breakiterator_parts_move_forward(zend_object_iterator *iter)
{
	zoi_break_iter_parts *zoi_bit = (zoi_break_iter_parts*)iter;
	BreakIterator_object *bio = zoi_bit->bio;

	iter->funcs->invalidate_current(iter);

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

	const char	*s = Z_STRVAL(bio->text);
	zend_string	*res;

	assert(next <= Z_STRLEN(bio->text) && next >= cur);
	res = zend_string_alloc(next - cur, 0);

	memcpy(ZSTR_VAL(res), &s[cur], ZSTR_LEN(res));
	ZSTR_VAL(res)[ZSTR_LEN(res)] = '\0';

	ZVAL_STR(&zoi_bit->zoi_cur.current, res);
}

static void _breakiterator_parts_rewind(zend_object_iterator *iter)
{
	zoi_break_iter_parts *zoi_bit = (zoi_break_iter_parts*)iter;
	BreakIterator_object *bio = zoi_bit->bio;

	if (!Z_ISUNDEF(zoi_bit->zoi_cur.current)) {
		iter->funcs->invalidate_current(iter);
	}

	bio->biter->first();

	iter->funcs->move_forward(iter);
}

static const zend_object_iterator_funcs breakiterator_parts_it_funcs = {
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
										   parts_iter_key_type key_type)
{
	IntlIterator_object *ii;

	object_init_ex(object, IntlPartsIterator_ce_ptr);
	ii = Z_INTL_ITERATOR_P(object);

	ii->iterator = (zend_object_iterator*)emalloc(sizeof(zoi_break_iter_parts));
	zend_iterator_init(ii->iterator);

	ZVAL_COPY(&ii->iterator->data, break_iter_zv);
	ii->iterator->funcs = &breakiterator_parts_it_funcs;
	ii->iterator->index = 0;

	((zoi_with_current*)ii->iterator)->destroy_it = _breakiterator_parts_destroy_it;
	ZVAL_OBJ(&((zoi_with_current*)ii->iterator)->wrapping_obj, Z_OBJ_P(object));
	ZVAL_UNDEF(&((zoi_with_current*)ii->iterator)->current);

	((zoi_break_iter_parts*)ii->iterator)->bio = Z_INTL_BREAKITERATOR_P(break_iter_zv);

	assert(((zoi_break_iter_parts*)ii->iterator)->bio->biter != NULL);

	((zoi_break_iter_parts*)ii->iterator)->key_type = key_type;
}

U_CFUNC zend_object *IntlPartsIterator_object_create(zend_class_entry *ce)
{
	zend_object *retval = IntlIterator_ce_ptr->create_object(ce);
	retval->handlers = &IntlPartsIterator_handlers;

	return retval;
}

U_CFUNC zend_function *IntlPartsIterator_get_method(zend_object **object_ptr, zend_string *method, const zval *key)
{
	zend_function *ret;
	zend_string *lc_method_name;
	ALLOCA_FLAG(use_heap);

	if (key == NULL) {
		ZSTR_ALLOCA_ALLOC(lc_method_name, ZSTR_LEN(method), use_heap);
		zend_str_tolower_copy(ZSTR_VAL(lc_method_name), ZSTR_VAL(method), ZSTR_LEN(method));
	} else {
		lc_method_name = Z_STR_P(key);
	}

	if (ZSTR_LEN(method) == sizeof("getrulestatus") - 1
			&& memcmp("getrulestatus", ZSTR_VAL(lc_method_name), ZSTR_LEN(lc_method_name)) == 0) {
		IntlIterator_object *obj = php_intl_iterator_fetch_object(*object_ptr);
		if (obj->iterator && !Z_ISUNDEF(obj->iterator->data)) {
			zval *break_iter_zv = &obj->iterator->data;
			*object_ptr = Z_OBJ_P(break_iter_zv);
			ret = Z_OBJ_HANDLER_P(break_iter_zv, get_method)(object_ptr, method, key);
			goto end;
		}
	}

	ret = zend_std_get_method(object_ptr, method, key);

end:
	if (key == NULL) {
	 	ZSTR_ALLOCA_FREE(lc_method_name, use_heap);
	}

	return ret;
}

U_CFUNC PHP_METHOD(IntlPartsIterator, getBreakIterator)
{
	INTLITERATOR_METHOD_INIT_VARS;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	INTLITERATOR_METHOD_FETCH_OBJECT;

	zval *biter_zval = &ii->iterator->data;
	ZVAL_COPY_DEREF(return_value, biter_zval);
}

U_CFUNC void breakiterator_register_IntlPartsIterator_class(void)
{
	zend_class_entry ce;

	/* Create and register 'BreakIterator' class. */
	INIT_CLASS_ENTRY(ce, "IntlPartsIterator", class_IntlPartsIterator_methods);
	IntlPartsIterator_ce_ptr = zend_register_internal_class_ex(&ce,
			IntlIterator_ce_ptr);
	IntlPartsIterator_ce_ptr->create_object = IntlPartsIterator_object_create;

	memcpy(&IntlPartsIterator_handlers, &IntlIterator_handlers,
			sizeof IntlPartsIterator_handlers);
	IntlPartsIterator_handlers.get_method = IntlPartsIterator_get_method;

#define PARTSITER_DECL_LONG_CONST(name) \
	zend_declare_class_constant_long(IntlPartsIterator_ce_ptr, #name, \
		sizeof(#name) - 1, PARTS_ITERATOR_ ## name)

	PARTSITER_DECL_LONG_CONST(KEY_SEQUENTIAL);
	PARTSITER_DECL_LONG_CONST(KEY_LEFT);
	PARTSITER_DECL_LONG_CONST(KEY_RIGHT);

#undef PARTSITER_DECL_LONG_CONST
}
