/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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

#include "../intl_cppshims.h"

// Fix build on Windows/old versions of ICU
#include <stdio.h>

#include "common_enum.h"

extern "C" {
#include <zend_interfaces.h>
#include <zend_exceptions.h>
}

zend_class_entry *IntlIterator_ce_ptr;
zend_object_handlers IntlIterator_handlers;

void zoi_with_current_dtor(zend_object_iterator *iter)
{
	zoi_with_current *zoiwc = (zoi_with_current*)iter;

	if (!Z_ISUNDEF(zoiwc->wrapping_obj)) {
		/* we have to copy the pointer because zoiwc->wrapping_obj may be
		 * changed midway the execution of zval_ptr_dtor() */
		zval *zwo = &zoiwc->wrapping_obj;

		/* object is still here, we can rely on it to call this again and
		 * destroy this object */
		zval_ptr_dtor(zwo);
	} else {
		/* Object not here anymore (we've been called by the object free handler)
		 * Note that the iterator wrapper objects (that also depend on this
		 * structure) call this function earlier, in the destruction phase, which
		 * precedes the object free phase. Therefore there's no risk on this
		 * function being called by the iterator wrapper destructor function and
		 * not finding the memory of this iterator allocated anymore. */
		iter->funcs->invalidate_current(iter);
		zoiwc->destroy_it(iter);
	}
}

U_CFUNC int zoi_with_current_valid(zend_object_iterator *iter)
{
	return Z_ISUNDEF(((zoi_with_current*)iter)->current)? FAILURE : SUCCESS;
}

U_CFUNC zval *zoi_with_current_get_current_data(zend_object_iterator *iter)
{
	return &((zoi_with_current*)iter)->current;
}

U_CFUNC void zoi_with_current_invalidate_current(zend_object_iterator *iter)
{
	zoi_with_current *zoi_iter = (zoi_with_current*)iter;
	if (!Z_ISUNDEF(zoi_iter->current)) {
		zval_ptr_dtor(&zoi_iter->current);
		ZVAL_UNDEF(&zoi_iter->current); //valid would return FAILURE now
	}
}

static void string_enum_current_move_forward(zend_object_iterator *iter)
{
	zoi_with_current *zoi_iter = (zoi_with_current*)iter;
	INTLITERATOR_METHOD_INIT_VARS;

	iter->funcs->invalidate_current(iter);

	object = &zoi_iter->wrapping_obj;
	INTLITERATOR_METHOD_FETCH_OBJECT_NO_CHECK;

	int32_t result_length;
	const char *result = ((StringEnumeration*)Z_PTR(iter->data))->next(
		&result_length, INTLITERATOR_ERROR_CODE(ii));

	intl_error_set_code(NULL, INTLITERATOR_ERROR_CODE(ii));
	if (U_FAILURE(INTLITERATOR_ERROR_CODE(ii))) {
		intl_errors_set_custom_msg(INTL_DATA_ERROR_P(ii),
			"Error fetching next iteration element", 0);
	} else if (result) {
		ZVAL_STRINGL(&zoi_iter->current, result, result_length);
	} //else we've reached the end of the enum, nothing more is required
}

static void string_enum_rewind(zend_object_iterator *iter)
{
	zoi_with_current *zoi_iter = (zoi_with_current*)iter;
	INTLITERATOR_METHOD_INIT_VARS;

	if (!Z_ISUNDEF(zoi_iter->current)) {
		iter->funcs->invalidate_current(iter);
	}

	object = &zoi_iter->wrapping_obj;
	INTLITERATOR_METHOD_FETCH_OBJECT_NO_CHECK;

	((StringEnumeration*)Z_PTR(iter->data))->reset(INTLITERATOR_ERROR_CODE(ii));

	intl_error_set_code(NULL, INTLITERATOR_ERROR_CODE(ii));
	if (U_FAILURE(INTLITERATOR_ERROR_CODE(ii))) {
		intl_errors_set_custom_msg(INTL_DATA_ERROR_P(ii),
			"Error resetting enumeration", 0);
	} else {
		iter->funcs->move_forward(iter);
	}
}

static void string_enum_destroy_it(zend_object_iterator *iter)
{
	delete (StringEnumeration*)Z_PTR(iter->data);
}

static const zend_object_iterator_funcs string_enum_object_iterator_funcs = {
	zoi_with_current_dtor,
	zoi_with_current_valid,
	zoi_with_current_get_current_data,
	NULL,
	string_enum_current_move_forward,
	string_enum_rewind,
	zoi_with_current_invalidate_current
};

U_CFUNC void IntlIterator_from_StringEnumeration(StringEnumeration *se, zval *object)
{
	IntlIterator_object *ii;
	object_init_ex(object, IntlIterator_ce_ptr);
	ii = Z_INTL_ITERATOR_P(object);
	ii->iterator = (zend_object_iterator*)emalloc(sizeof(zoi_with_current));
	zend_iterator_init(ii->iterator);
	ZVAL_PTR(&ii->iterator->data, se);
	ii->iterator->funcs = &string_enum_object_iterator_funcs;
	ii->iterator->index = 0;
	((zoi_with_current*)ii->iterator)->destroy_it = string_enum_destroy_it;
	ZVAL_COPY_VALUE(&((zoi_with_current*)ii->iterator)->wrapping_obj, object);
	ZVAL_UNDEF(&((zoi_with_current*)ii->iterator)->current);
}

static void IntlIterator_objects_free(zend_object *object)
{
	IntlIterator_object	*ii = php_intl_iterator_fetch_object(object);

	if (ii->iterator) {
		zval *wrapping_objp = &((zoi_with_current*)ii->iterator)->wrapping_obj;
		ZVAL_UNDEF(wrapping_objp);
		zend_iterator_dtor(ii->iterator);
	}
	intl_error_reset(INTLITERATOR_ERROR_P(ii));

	zend_object_std_dtor(&ii->zo);
}

static zend_object_iterator *IntlIterator_get_iterator(
	zend_class_entry *ce, zval *object, int by_ref)
{
	if (by_ref) {
		zend_throw_exception(NULL,
			"Iteration by reference is not supported", 0);
		return NULL;
	}

	IntlIterator_object *ii = Z_INTL_ITERATOR_P(object);

	if (ii->iterator == NULL) {
		zend_throw_exception(NULL,
			"The IntlIterator is not properly constructed", 0);
		return NULL;
	}

	GC_ADDREF(&ii->iterator->std);

	return ii->iterator;
}

static zend_object *IntlIterator_object_create(zend_class_entry *ce)
{
	IntlIterator_object	*intern;

	intern = (IntlIterator_object*)ecalloc(1, sizeof(IntlIterator_object) + sizeof(zval) * (ce->default_properties_count - 1));

	zend_object_std_init(&intern->zo, ce);
    object_properties_init(&intern->zo, ce);
	intl_error_init(INTLITERATOR_ERROR_P(intern));

	intern->iterator = NULL;

	intern->zo.handlers = &IntlIterator_handlers;

	return &intern->zo;
}

static PHP_METHOD(IntlIterator, current)
{
	zval *data;
	INTLITERATOR_METHOD_INIT_VARS;

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"IntlIterator::current: bad arguments", 0);
		return;
	}

	INTLITERATOR_METHOD_FETCH_OBJECT;
	data = ii->iterator->funcs->get_current_data(ii->iterator);
	if (data) {
		ZVAL_COPY_DEREF(return_value, data);
	}
}

static PHP_METHOD(IntlIterator, key)
{
	INTLITERATOR_METHOD_INIT_VARS;

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"IntlIterator::key: bad arguments", 0);
		return;
	}

	INTLITERATOR_METHOD_FETCH_OBJECT;

	if (ii->iterator->funcs->get_current_key) {
		ii->iterator->funcs->get_current_key(ii->iterator, return_value);
	} else {
		RETURN_LONG(ii->iterator->index);
	}
}

static PHP_METHOD(IntlIterator, next)
{
	INTLITERATOR_METHOD_INIT_VARS;

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"IntlIterator::next: bad arguments", 0);
		return;
	}

	INTLITERATOR_METHOD_FETCH_OBJECT;
	ii->iterator->funcs->move_forward(ii->iterator);
	/* foreach also advances the index after the last iteration,
	 * so I see no problem in incrementing the index here unconditionally */
	ii->iterator->index++;
}

static PHP_METHOD(IntlIterator, rewind)
{
	INTLITERATOR_METHOD_INIT_VARS;

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"IntlIterator::rewind: bad arguments", 0);
		return;
	}

	INTLITERATOR_METHOD_FETCH_OBJECT;
	if (ii->iterator->funcs->rewind) {
		ii->iterator->funcs->rewind(ii->iterator);
	} else {
		intl_errors_set(INTLITERATOR_ERROR_P(ii), U_UNSUPPORTED_ERROR,
			"IntlIterator::rewind: rewind not supported", 0);
	}
}

static PHP_METHOD(IntlIterator, valid)
{
	INTLITERATOR_METHOD_INIT_VARS;

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"IntlIterator::valid: bad arguments", 0);
		return;
	}

	INTLITERATOR_METHOD_FETCH_OBJECT;
	RETURN_BOOL(ii->iterator->funcs->valid(ii->iterator) == SUCCESS);
}

ZEND_BEGIN_ARG_INFO_EX(ainfo_se_void, 0, 0, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry IntlIterator_class_functions[] = {
	PHP_ME(IntlIterator,	current,	ainfo_se_void,			ZEND_ACC_PUBLIC)
	PHP_ME(IntlIterator,	key,		ainfo_se_void,			ZEND_ACC_PUBLIC)
	PHP_ME(IntlIterator,	next,		ainfo_se_void,			ZEND_ACC_PUBLIC)
	PHP_ME(IntlIterator,	rewind,		ainfo_se_void,			ZEND_ACC_PUBLIC)
	PHP_ME(IntlIterator,	valid,		ainfo_se_void,			ZEND_ACC_PUBLIC)
	PHP_FE_END
};


/* {{{ intl_register_IntlIterator_class
 * Initialize 'IntlIterator' class
 */
U_CFUNC void intl_register_IntlIterator_class(void)
{
	zend_class_entry ce;

	/* Create and register 'IntlIterator' class. */
	INIT_CLASS_ENTRY(ce, "IntlIterator", IntlIterator_class_functions);
	ce.create_object = IntlIterator_object_create;
	IntlIterator_ce_ptr = zend_register_internal_class(&ce);
	IntlIterator_ce_ptr->get_iterator = IntlIterator_get_iterator;
	zend_class_implements(IntlIterator_ce_ptr, 1,
		zend_ce_iterator);

	memcpy(&IntlIterator_handlers, &std_object_handlers,
		sizeof IntlIterator_handlers);
	IntlIterator_handlers.offset = XtOffsetOf(IntlIterator_object, zo);
	IntlIterator_handlers.clone_obj = NULL;
	IntlIterator_handlers.dtor_obj = zend_objects_destroy_object;
	IntlIterator_handlers.free_obj = IntlIterator_objects_free;

}
