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

void zoi_with_current_dtor(zend_object_iterator *iter TSRMLS_DC)
{
	zoi_with_current *zoiwc = (zoi_with_current*)iter;
	
	if (zoiwc->wrapping_obj) {
		/* we have to copy the pointer because zoiwc->wrapping_obj may be
		 * changed midway the execution of zval_ptr_dtor() */
		zval *zwo = zoiwc->wrapping_obj;
		
		/* object is still here, we can rely on it to call this again and
		 * destroy this object */
		zval_ptr_dtor(&zwo);
	} else {
		/* Object not here anymore (we've been called by the object free handler)
		 * Note that the iterator wrapper objects (that also depend on this
		 * structure) call this function earlier, in the destruction phase, which
		 * precedes the object free phase. Therefore there's no risk on this
		 * function being called by the iterator wrapper destructor function and
		 * not finding the memory of this iterator allocated anymore. */
		iter->funcs->invalidate_current(iter TSRMLS_CC);
		zoiwc->destroy_it(iter TSRMLS_CC);
		efree(iter);
	}
}

U_CFUNC int zoi_with_current_valid(zend_object_iterator *iter TSRMLS_DC)
{
	return ((zoi_with_current*)iter)->current != NULL ? SUCCESS : FAILURE;
}

U_CFUNC void zoi_with_current_get_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC)
{
	*data = &((zoi_with_current*)iter)->current;
}

U_CFUNC void zoi_with_current_invalidate_current(zend_object_iterator *iter TSRMLS_DC)
{
	zoi_with_current *zoi_iter = (zoi_with_current*)iter;
	if (zoi_iter->current) {
		zval_ptr_dtor(&zoi_iter->current);
		zoi_iter->current = NULL; //valid would return FAILURE now
	}
}

static void string_enum_current_move_forward(zend_object_iterator *iter TSRMLS_DC)
{
	zoi_with_current *zoi_iter = (zoi_with_current*)iter;
	INTLITERATOR_METHOD_INIT_VARS;

	iter->funcs->invalidate_current(iter TSRMLS_CC);

	object = zoi_iter->wrapping_obj;
	INTLITERATOR_METHOD_FETCH_OBJECT_NO_CHECK;

	int32_t result_length;
	const char *result = ((StringEnumeration*)iter->data)->next(
		&result_length, INTLITERATOR_ERROR_CODE(ii));

	intl_error_set_code(NULL, INTLITERATOR_ERROR_CODE(ii) TSRMLS_CC);
	if (U_FAILURE(INTLITERATOR_ERROR_CODE(ii))) {
		intl_errors_set_custom_msg(INTL_DATA_ERROR_P(ii),
			"Error fetching next iteration element", 0 TSRMLS_CC);
	} else if (result) {
		MAKE_STD_ZVAL(zoi_iter->current);
		ZVAL_STRINGL(zoi_iter->current, result, result_length, 1);
	} //else we've reached the end of the enum, nothing more is required
}

static void string_enum_rewind(zend_object_iterator *iter TSRMLS_DC)
{
	zoi_with_current *zoi_iter = (zoi_with_current*)iter;
	INTLITERATOR_METHOD_INIT_VARS;

	if (zoi_iter->current) {
		iter->funcs->invalidate_current(iter TSRMLS_CC);
	}

	object = zoi_iter->wrapping_obj;
	INTLITERATOR_METHOD_FETCH_OBJECT_NO_CHECK;

	((StringEnumeration*)iter->data)->reset(INTLITERATOR_ERROR_CODE(ii));

	intl_error_set_code(NULL, INTLITERATOR_ERROR_CODE(ii) TSRMLS_CC);
	if (U_FAILURE(INTLITERATOR_ERROR_CODE(ii))) {
		intl_errors_set_custom_msg(INTL_DATA_ERROR_P(ii),
			"Error resetting enumeration", 0 TSRMLS_CC);
	} else {
		iter->funcs->move_forward(iter TSRMLS_CC);
	}
}

static void string_enum_destroy_it(zend_object_iterator *iter TSRMLS_DC)
{
	delete (StringEnumeration*)iter->data;
}

static zend_object_iterator_funcs string_enum_object_iterator_funcs = {
	zoi_with_current_dtor,
	zoi_with_current_valid,
	zoi_with_current_get_current_data,
	NULL,
	string_enum_current_move_forward,
	string_enum_rewind,
	zoi_with_current_invalidate_current
};

U_CFUNC void IntlIterator_from_StringEnumeration(StringEnumeration *se, zval *object TSRMLS_DC)
{
	IntlIterator_object *ii;
	object_init_ex(object, IntlIterator_ce_ptr);
	ii = (IntlIterator_object*)zend_object_store_get_object(object TSRMLS_CC);
	ii->iterator = (zend_object_iterator*)emalloc(sizeof(zoi_with_current));
	ii->iterator->data = (void*)se;
	ii->iterator->funcs = &string_enum_object_iterator_funcs;
	ii->iterator->index = 0;
	((zoi_with_current*)ii->iterator)->destroy_it = string_enum_destroy_it;
	((zoi_with_current*)ii->iterator)->wrapping_obj = object;
	((zoi_with_current*)ii->iterator)->current = NULL;
}

static void IntlIterator_objects_free(zend_object *object TSRMLS_DC)
{
	IntlIterator_object	*ii = (IntlIterator_object*) object;

	if (ii->iterator) {
		zval **wrapping_objp = &((zoi_with_current*)ii->iterator)->wrapping_obj;
		*wrapping_objp = NULL;
		ii->iterator->funcs->dtor(ii->iterator TSRMLS_CC);
	}
	intl_error_reset(INTLITERATOR_ERROR_P(ii) TSRMLS_CC);

	zend_object_std_dtor(&ii->zo TSRMLS_CC);

	efree(ii);
}

static zend_object_iterator *IntlIterator_get_iterator(
	zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC)
{
	if (by_ref) {
		zend_throw_exception(NULL,
			"Iteration by reference is not supported", 0 TSRMLS_CC);
		return NULL;
	}

	IntlIterator_object *ii = (IntlIterator_object*)
		zend_object_store_get_object(object TSRMLS_CC);

	if (ii->iterator == NULL) {
		zend_throw_exception(NULL,
			"The IntlIterator is not properly constructed", 0 TSRMLS_CC);
		return NULL;
	}

	zval_add_ref(&object);

	return ii->iterator;
}

static zend_object_value IntlIterator_object_create(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value	retval;
	IntlIterator_object	*intern;

	intern = (IntlIterator_object*)ecalloc(1, sizeof(IntlIterator_object));
	
	zend_object_std_init(&intern->zo, ce TSRMLS_CC);
#if PHP_VERSION_ID < 50399
    zend_hash_copy(intern->zo.properties, &(ce->default_properties),
        (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval*));
#else
    object_properties_init((zend_object*) intern, ce);
#endif
	intl_error_init(INTLITERATOR_ERROR_P(intern) TSRMLS_CC);
	intern->iterator = NULL;

	retval.handle = zend_objects_store_put(
		intern,
		(zend_objects_store_dtor_t)zend_objects_destroy_object,
		(zend_objects_free_object_storage_t)IntlIterator_objects_free,
		NULL TSRMLS_CC);

	retval.handlers = &IntlIterator_handlers;

	return retval;
}

static PHP_METHOD(IntlIterator, current)
{
	zval **data;
	INTLITERATOR_METHOD_INIT_VARS;

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"IntlIterator::current: bad arguments", 0 TSRMLS_CC);
		return;
	}

	INTLITERATOR_METHOD_FETCH_OBJECT;
	ii->iterator->funcs->get_current_data(ii->iterator, &data TSRMLS_CC);
	if (data && *data) {
		RETURN_ZVAL(*data, 1, 0);
	}
}

static PHP_METHOD(IntlIterator, key)
{
	INTLITERATOR_METHOD_INIT_VARS;

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"IntlIterator::key: bad arguments", 0 TSRMLS_CC);
		return;
	}

	INTLITERATOR_METHOD_FETCH_OBJECT;

	if (ii->iterator->funcs->get_current_key) {
		ii->iterator->funcs->get_current_key(ii->iterator, return_value TSRMLS_CC);
	} else {
		RETURN_LONG(ii->iterator->index);
	}
}

static PHP_METHOD(IntlIterator, next)
{
	INTLITERATOR_METHOD_INIT_VARS;

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"IntlIterator::next: bad arguments", 0 TSRMLS_CC);
		return;
	}

	INTLITERATOR_METHOD_FETCH_OBJECT;
	ii->iterator->funcs->move_forward(ii->iterator TSRMLS_CC);
	/* foreach also advances the index after the last iteration,
	 * so I see no problem in incrementing the index here unconditionally */
	ii->iterator->index++;
}

static PHP_METHOD(IntlIterator, rewind)
{
	INTLITERATOR_METHOD_INIT_VARS;

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"IntlIterator::rewind: bad arguments", 0 TSRMLS_CC);
		return;
	}

	INTLITERATOR_METHOD_FETCH_OBJECT;
	if (ii->iterator->funcs->rewind) {
		ii->iterator->funcs->rewind(ii->iterator TSRMLS_CC);
	} else {
		intl_errors_set(INTLITERATOR_ERROR_P(ii), U_UNSUPPORTED_ERROR,
			"IntlIterator::rewind: rewind not supported", 0 TSRMLS_CC);
	}
}

static PHP_METHOD(IntlIterator, valid)
{
	INTLITERATOR_METHOD_INIT_VARS;

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"IntlIterator::valid: bad arguments", 0 TSRMLS_CC);
		return;
	}

	INTLITERATOR_METHOD_FETCH_OBJECT;
	RETURN_BOOL(ii->iterator->funcs->valid(ii->iterator TSRMLS_CC) == SUCCESS);
}

ZEND_BEGIN_ARG_INFO_EX(ainfo_se_void, 0, 0, 0)
ZEND_END_ARG_INFO()

static zend_function_entry IntlIterator_class_functions[] = {
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
U_CFUNC void intl_register_IntlIterator_class(TSRMLS_D)
{
	zend_class_entry ce;

	/* Create and register 'IntlIterator' class. */
	INIT_CLASS_ENTRY(ce, "IntlIterator", IntlIterator_class_functions);
	ce.create_object = IntlIterator_object_create;
	IntlIterator_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);
	IntlIterator_ce_ptr->get_iterator = IntlIterator_get_iterator;
	zend_class_implements(IntlIterator_ce_ptr TSRMLS_CC, 1,
		zend_ce_iterator);

	memcpy(&IntlIterator_handlers, zend_get_std_object_handlers(),
		sizeof IntlIterator_handlers);
	IntlIterator_handlers.clone_obj = NULL;

}
