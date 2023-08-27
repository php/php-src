/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "zend_interfaces_arginfo.h"

ZEND_API zend_class_entry *zend_ce_traversable;
ZEND_API zend_class_entry *zend_ce_aggregate;
ZEND_API zend_class_entry *zend_ce_iterator;
ZEND_API zend_class_entry *zend_ce_arrayaccess;
ZEND_API zend_class_entry *zend_ce_serializable;
ZEND_API zend_class_entry *zend_ce_countable;
ZEND_API zend_class_entry *zend_ce_stringable;
ZEND_API zend_class_entry *zend_ce_internal_iterator;

static zend_object_handlers zend_internal_iterator_handlers;

/* {{{ zend_call_method
 Only returns the returned zval if retval_ptr != NULL */
ZEND_API zval* zend_call_method(zend_object *object, zend_class_entry *obj_ce, zend_function **fn_proxy, const char *function_name, size_t function_name_len, zval *retval_ptr, uint32_t param_count, zval* arg1, zval* arg2)
{
	zend_function *fn;
	zend_class_entry *called_scope;
	zval params[2];

	if (param_count > 0) {
		ZVAL_COPY_VALUE(&params[0], arg1);
	}
	if (param_count > 1) {
		ZVAL_COPY_VALUE(&params[1], arg2);
	}

	if (!obj_ce) {
		obj_ce = object ? object->ce : NULL;
	}
	if (!fn_proxy || !*fn_proxy) {
		if (EXPECTED(obj_ce)) {
			fn = zend_hash_str_find_ptr_lc(
				&obj_ce->function_table, function_name, function_name_len);
			if (UNEXPECTED(fn == NULL)) {
				/* error at c-level */
				zend_error_noreturn(E_CORE_ERROR, "Couldn't find implementation for method %s::%s", ZSTR_VAL(obj_ce->name), function_name);
			}
		} else {
			fn = zend_fetch_function_str(function_name, function_name_len);
			if (UNEXPECTED(fn == NULL)) {
				/* error at c-level */
				zend_error_noreturn(E_CORE_ERROR, "Couldn't find implementation for function %s", function_name);
			}
		}
		if (fn_proxy) {
			*fn_proxy = fn;
		}
	} else {
		fn = *fn_proxy;
	}

	if (object) {
		called_scope = object->ce;
	} else {
		called_scope = obj_ce;
	}

	zend_call_known_function(fn, object, called_scope, retval_ptr, param_count, params, NULL);
	return retval_ptr;
}
/* }}} */

/* iterator interface, c-level functions used by engine */

/* {{{ zend_user_it_new_iterator */
ZEND_API void zend_user_it_new_iterator(zend_class_entry *ce, zval *object, zval *retval)
{
	zend_call_known_instance_method_with_0_params(
		ce->iterator_funcs_ptr->zf_new_iterator, Z_OBJ_P(object), retval);
}
/* }}} */

/* {{{ zend_user_it_invalidate_current */
ZEND_API void zend_user_it_invalidate_current(zend_object_iterator *_iter)
{
	zend_user_iterator *iter = (zend_user_iterator*)_iter;

	if (!Z_ISUNDEF(iter->value)) {
		zval_ptr_dtor(&iter->value);
		ZVAL_UNDEF(&iter->value);
	}
}
/* }}} */

/* {{{ zend_user_it_dtor */
static void zend_user_it_dtor(zend_object_iterator *_iter)
{
	zend_user_iterator *iter = (zend_user_iterator*)_iter;
	zval *object = &iter->it.data;

	zend_user_it_invalidate_current(_iter);
	zval_ptr_dtor(object);
}
/* }}} */

/* {{{ zend_user_it_valid */
ZEND_API int zend_user_it_valid(zend_object_iterator *_iter)
{
	if (_iter) {
		zend_user_iterator *iter = (zend_user_iterator*)_iter;
		zval *object = &iter->it.data;
		zval more;
		bool result;

		zend_call_method_with_0_params(Z_OBJ_P(object), iter->ce, &iter->ce->iterator_funcs_ptr->zf_valid, "valid", &more);
		result = i_zend_is_true(&more);
		zval_ptr_dtor(&more);
		return result ? SUCCESS : FAILURE;
	}
	return FAILURE;
}
/* }}} */

/* {{{ zend_user_it_get_current_data */
ZEND_API zval *zend_user_it_get_current_data(zend_object_iterator *_iter)
{
	zend_user_iterator *iter = (zend_user_iterator*)_iter;
	zval *object = &iter->it.data;

	if (Z_ISUNDEF(iter->value)) {
		zend_call_method_with_0_params(Z_OBJ_P(object), iter->ce, &iter->ce->iterator_funcs_ptr->zf_current, "current", &iter->value);
	}
	return &iter->value;
}
/* }}} */

/* {{{ zend_user_it_get_current_key */
ZEND_API void zend_user_it_get_current_key(zend_object_iterator *_iter, zval *key)
{
	zend_user_iterator *iter = (zend_user_iterator*)_iter;
	zval *object = &iter->it.data;
	zend_call_method_with_0_params(Z_OBJ_P(object), iter->ce, &iter->ce->iterator_funcs_ptr->zf_key, "key", key);
	if (UNEXPECTED(Z_ISREF_P(key))) {
		zend_unwrap_reference(key);
	}
}
/* }}} */

/* {{{ zend_user_it_move_forward */
ZEND_API void zend_user_it_move_forward(zend_object_iterator *_iter)
{
	zend_user_iterator *iter = (zend_user_iterator*)_iter;
	zval *object = &iter->it.data;

	zend_user_it_invalidate_current(_iter);
	zend_call_method_with_0_params(Z_OBJ_P(object), iter->ce, &iter->ce->iterator_funcs_ptr->zf_next, "next", NULL);
}
/* }}} */

/* {{{ zend_user_it_rewind */
ZEND_API void zend_user_it_rewind(zend_object_iterator *_iter)
{
	zend_user_iterator *iter = (zend_user_iterator*)_iter;
	zval *object = &iter->it.data;

	zend_user_it_invalidate_current(_iter);
	zend_call_method_with_0_params(Z_OBJ_P(object), iter->ce, &iter->ce->iterator_funcs_ptr->zf_rewind, "rewind", NULL);
}
/* }}} */

ZEND_API HashTable *zend_user_it_get_gc(zend_object_iterator *_iter, zval **table, int *n)
{
	zend_user_iterator *iter = (zend_user_iterator*)_iter;
	if (Z_ISUNDEF(iter->value)) {
		*table = &iter->it.data;
		*n = 1;
	} else {
		zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();
		zend_get_gc_buffer_add_zval(gc_buffer, &iter->it.data);
		zend_get_gc_buffer_add_zval(gc_buffer, &iter->value);
		zend_get_gc_buffer_use(gc_buffer, table, n);
	}
	return NULL;
}

static const zend_object_iterator_funcs zend_interface_iterator_funcs_iterator = {
	zend_user_it_dtor,
	zend_user_it_valid,
	zend_user_it_get_current_data,
	zend_user_it_get_current_key,
	zend_user_it_move_forward,
	zend_user_it_rewind,
	zend_user_it_invalidate_current,
	zend_user_it_get_gc,
};

/* {{{ zend_user_it_get_iterator */
/* by_ref is int due to Iterator API */
static zend_object_iterator *zend_user_it_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	zend_user_iterator *iterator;

	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(zend_user_iterator));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->it.data, Z_OBJ_P(object));
	iterator->it.funcs = &zend_interface_iterator_funcs_iterator;
	iterator->ce = Z_OBJCE_P(object);
	ZVAL_UNDEF(&iterator->value);
	return (zend_object_iterator*)iterator;
}
/* }}} */

/* {{{ zend_user_it_get_new_iterator */
/* by_ref is int due to Iterator API */
ZEND_API zend_object_iterator *zend_user_it_get_new_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	zval iterator;
	zend_object_iterator *new_iterator;
	zend_class_entry *ce_it;

	zend_user_it_new_iterator(ce, object, &iterator);
	ce_it = (Z_TYPE(iterator) == IS_OBJECT) ? Z_OBJCE(iterator) : NULL;

	if (!ce_it || !ce_it->get_iterator || (ce_it->get_iterator == zend_user_it_get_new_iterator && Z_OBJ(iterator) == Z_OBJ_P(object))) {
		if (!EG(exception)) {
			zend_throw_exception_ex(NULL, 0, "Objects returned by %s::getIterator() must be traversable or implement interface Iterator", ce ? ZSTR_VAL(ce->name) : ZSTR_VAL(Z_OBJCE_P(object)->name));
		}
		zval_ptr_dtor(&iterator);
		return NULL;
	}

	new_iterator = ce_it->get_iterator(ce_it, &iterator, by_ref);
	zval_ptr_dtor(&iterator);
	return new_iterator;
}
/* }}} */

/* {{{ zend_implement_traversable */
static int zend_implement_traversable(zend_class_entry *interface, zend_class_entry *class_type)
{
	/* Abstract class can implement Traversable only, in which case the extending class must
	 * implement Iterator or IteratorAggregate. */
	if (class_type->ce_flags & ZEND_ACC_EXPLICIT_ABSTRACT_CLASS) {
		return SUCCESS;
	}

	/* Check that class_type implements at least one of 'IteratorAggregate' or 'Iterator' */
	if (class_type->num_interfaces) {
		ZEND_ASSERT(class_type->ce_flags & ZEND_ACC_RESOLVED_INTERFACES);
		for (uint32_t i = 0; i < class_type->num_interfaces; i++) {
			if (class_type->interfaces[i] == zend_ce_aggregate || class_type->interfaces[i] == zend_ce_iterator) {
				return SUCCESS;
			}
		}
	}
	zend_error_noreturn(E_CORE_ERROR, "Class %s must implement interface %s as part of either %s or %s",
		ZSTR_VAL(class_type->name),
		ZSTR_VAL(zend_ce_traversable->name),
		ZSTR_VAL(zend_ce_iterator->name),
		ZSTR_VAL(zend_ce_aggregate->name));
	return FAILURE;
}
/* }}} */

/* {{{ zend_implement_aggregate */
static int zend_implement_aggregate(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (zend_class_implements_interface(class_type, zend_ce_iterator)) {
		zend_error_noreturn(E_ERROR,
			"Class %s cannot implement both Iterator and IteratorAggregate at the same time",
			ZSTR_VAL(class_type->name));
	}

	zend_function *zf = zend_hash_str_find_ptr(
		&class_type->function_table, "getiterator", sizeof("getiterator") - 1);
	if (class_type->get_iterator && class_type->get_iterator != zend_user_it_get_new_iterator) {
		/* get_iterator was explicitly assigned for an internal class. */
		if (!class_type->parent || class_type->parent->get_iterator != class_type->get_iterator) {
			ZEND_ASSERT(class_type->type == ZEND_INTERNAL_CLASS);
			return SUCCESS;
		}

		/* The getIterator() method has not been overwritten, use inherited get_iterator(). */
		if (zf->common.scope != class_type) {
			return SUCCESS;
		}

		/* getIterator() has been overwritten, switch to zend_user_it_get_new_iterator. */
	}

	ZEND_ASSERT(!class_type->iterator_funcs_ptr && "Iterator funcs already set?");
	zend_class_iterator_funcs *funcs_ptr = class_type->type == ZEND_INTERNAL_CLASS
		? pemalloc(sizeof(zend_class_iterator_funcs), 1)
		: zend_arena_alloc(&CG(arena), sizeof(zend_class_iterator_funcs));
	class_type->get_iterator = zend_user_it_get_new_iterator;
	class_type->iterator_funcs_ptr = funcs_ptr;

	memset(funcs_ptr, 0, sizeof(zend_class_iterator_funcs));
	funcs_ptr->zf_new_iterator = zf;

	return SUCCESS;
}
/* }}} */

/* {{{ zend_implement_iterator */
static int zend_implement_iterator(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (zend_class_implements_interface(class_type, zend_ce_aggregate)) {
		zend_error_noreturn(E_ERROR,
			"Class %s cannot implement both Iterator and IteratorAggregate at the same time",
			ZSTR_VAL(class_type->name));
	}

	if (class_type->get_iterator && class_type->get_iterator != zend_user_it_get_iterator) {
		if (!class_type->parent || class_type->parent->get_iterator != class_type->get_iterator) {
			/* get_iterator was explicitly assigned for an internal class. */
			ZEND_ASSERT(class_type->type == ZEND_INTERNAL_CLASS);
			return SUCCESS;
		}
		/* Otherwise get_iterator was inherited from the parent by default. */
	}

	if (class_type->parent && (class_type->parent->ce_flags & ZEND_ACC_REUSE_GET_ITERATOR)) {
		/* Keep the inherited get_iterator handler. */
		class_type->ce_flags |= ZEND_ACC_REUSE_GET_ITERATOR;
	} else {
		class_type->get_iterator = zend_user_it_get_iterator;
	}

	ZEND_ASSERT(!class_type->iterator_funcs_ptr && "Iterator funcs already set?");
	zend_class_iterator_funcs *funcs_ptr = class_type->type == ZEND_INTERNAL_CLASS
		? pemalloc(sizeof(zend_class_iterator_funcs), 1)
		: zend_arena_alloc(&CG(arena), sizeof(zend_class_iterator_funcs));
	memset(funcs_ptr, 0, sizeof(zend_class_iterator_funcs));
	class_type->iterator_funcs_ptr = funcs_ptr;

	return SUCCESS;
}
/* }}} */

/* {{{ zend_user_serialize */
ZEND_API int zend_user_serialize(zval *object, unsigned char **buffer, size_t *buf_len, zend_serialize_data *data)
{
	zend_class_entry * ce = Z_OBJCE_P(object);
	zval retval;
	zend_result result;

	zend_call_method_with_0_params(
		Z_OBJ_P(object), Z_OBJCE_P(object), NULL, "serialize", &retval);

	if (Z_TYPE(retval) == IS_UNDEF || EG(exception)) {
		result = FAILURE;
	} else {
		switch(Z_TYPE(retval)) {
		case IS_NULL:
			/* we could also make this '*buf_len = 0' but this allows to skip variables */
			zval_ptr_dtor(&retval);
			return FAILURE;
		case IS_STRING:
			*buffer = (unsigned char*)estrndup(Z_STRVAL(retval), Z_STRLEN(retval));
			*buf_len = Z_STRLEN(retval);
			result = SUCCESS;
			break;
		default: /* failure */
			result = FAILURE;
			break;
		}
		zval_ptr_dtor(&retval);
	}

	if (result == FAILURE && !EG(exception)) {
		zend_throw_exception_ex(NULL, 0, "%s::serialize() must return a string or NULL", ZSTR_VAL(ce->name));
	}
	return result;
}
/* }}} */

/* {{{ zend_user_unserialize */
ZEND_API int zend_user_unserialize(zval *object, zend_class_entry *ce, const unsigned char *buf, size_t buf_len, zend_unserialize_data *data)
{
	zval zdata;

	if (UNEXPECTED(object_init_ex(object, ce) != SUCCESS)) {
		return FAILURE;
	}

	ZVAL_STRINGL(&zdata, (char*)buf, buf_len);
	zend_call_method_with_1_params(
		Z_OBJ_P(object), Z_OBJCE_P(object), NULL, "unserialize", NULL, &zdata);
	zval_ptr_dtor(&zdata);

	if (EG(exception)) {
		return FAILURE;
	} else {
		return SUCCESS;
	}
}
/* }}} */

/* {{{ zend_implement_serializable */
static int zend_implement_serializable(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (class_type->parent
		&& (class_type->parent->serialize || class_type->parent->unserialize)
		&& !zend_class_implements_interface(class_type->parent, zend_ce_serializable)) {
		return FAILURE;
	}
	if (!class_type->serialize) {
		class_type->serialize = zend_user_serialize;
	}
	if (!class_type->unserialize) {
		class_type->unserialize = zend_user_unserialize;
	}
	if (!(class_type->ce_flags & ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)
			&& (!class_type->__serialize || !class_type->__unserialize)) {
		zend_error(E_DEPRECATED, "%s implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary)", ZSTR_VAL(class_type->name));
	}
	return SUCCESS;
}
/* }}}*/

typedef struct {
	zend_object std;
	zend_object_iterator *iter;
	bool rewind_called;
} zend_internal_iterator;

static zend_object *zend_internal_iterator_create(zend_class_entry *ce) {
	zend_internal_iterator *intern = emalloc(sizeof(zend_internal_iterator));
	zend_object_std_init(&intern->std, ce);
	intern->std.handlers = &zend_internal_iterator_handlers;
	intern->iter = NULL;
	intern->rewind_called = 0;
	return &intern->std;
}

ZEND_API zend_result zend_create_internal_iterator_zval(zval *return_value, zval *obj) {
	zend_class_entry *scope = EG(current_execute_data)->func->common.scope;
	ZEND_ASSERT(scope->get_iterator != zend_user_it_get_new_iterator);
	zend_object_iterator *iter = scope->get_iterator(Z_OBJCE_P(obj), obj, /* by_ref */ 0);
	if (!iter) {
		return FAILURE;
	}

	zend_internal_iterator *intern =
		(zend_internal_iterator *) zend_internal_iterator_create(zend_ce_internal_iterator);
	intern->iter = iter;
	intern->iter->index = 0;
	ZVAL_OBJ(return_value, &intern->std);
	return SUCCESS;
}

static void zend_internal_iterator_free(zend_object *obj) {
	zend_internal_iterator *intern = (zend_internal_iterator *) obj;
	if (intern->iter) {
		zend_iterator_dtor(intern->iter);
	}
	zend_object_std_dtor(&intern->std);
}

static zend_internal_iterator *zend_internal_iterator_fetch(zval *This) {
	zend_internal_iterator *intern = (zend_internal_iterator *) Z_OBJ_P(This);
	if (!intern->iter) {
		zend_throw_error(NULL, "The InternalIterator object has not been properly initialized");
		return NULL;
	}
	return intern;
}

/* Many iterators will not behave correctly if rewind() is not called, make sure it happens. */
static zend_result zend_internal_iterator_ensure_rewound(zend_internal_iterator *intern) {
	if (!intern->rewind_called) {
		zend_object_iterator *iter = intern->iter;
		intern->rewind_called = 1;
		if (iter->funcs->rewind) {
			iter->funcs->rewind(iter);
			if (UNEXPECTED(EG(exception))) {
				return FAILURE;
			}
		}
	}
	return SUCCESS;
}


ZEND_METHOD(InternalIterator, __construct) {
	zend_throw_error(NULL, "Cannot manually construct InternalIterator");
}

ZEND_METHOD(InternalIterator, current) {
	ZEND_PARSE_PARAMETERS_NONE();

	zend_internal_iterator *intern = zend_internal_iterator_fetch(ZEND_THIS);
	if (!intern) {
		RETURN_THROWS();
	}

	if (zend_internal_iterator_ensure_rewound(intern) == FAILURE) {
		RETURN_THROWS();
	}

	zval *data = intern->iter->funcs->get_current_data(intern->iter);
	if (data) {
		RETURN_COPY_DEREF(data);
	}
}

ZEND_METHOD(InternalIterator, key) {
	ZEND_PARSE_PARAMETERS_NONE();

	zend_internal_iterator *intern = zend_internal_iterator_fetch(ZEND_THIS);
	if (!intern) {
		RETURN_THROWS();
	}

	if (zend_internal_iterator_ensure_rewound(intern) == FAILURE) {
		RETURN_THROWS();
	}

	if (intern->iter->funcs->get_current_key) {
		intern->iter->funcs->get_current_key(intern->iter, return_value);
	} else {
		RETURN_LONG(intern->iter->index);
	}
}

ZEND_METHOD(InternalIterator, next) {
	ZEND_PARSE_PARAMETERS_NONE();

	zend_internal_iterator *intern = zend_internal_iterator_fetch(ZEND_THIS);
	if (!intern) {
		RETURN_THROWS();
	}

	if (zend_internal_iterator_ensure_rewound(intern) == FAILURE) {
		RETURN_THROWS();
	}

	/* Advance index first to match foreach behavior. */
	intern->iter->index++;
	intern->iter->funcs->move_forward(intern->iter);
}

ZEND_METHOD(InternalIterator, valid) {
	ZEND_PARSE_PARAMETERS_NONE();

	zend_internal_iterator *intern = zend_internal_iterator_fetch(ZEND_THIS);
	if (!intern) {
		RETURN_THROWS();
	}

	if (zend_internal_iterator_ensure_rewound(intern) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(intern->iter->funcs->valid(intern->iter) == SUCCESS);
}

ZEND_METHOD(InternalIterator, rewind) {
	ZEND_PARSE_PARAMETERS_NONE();

	zend_internal_iterator *intern = zend_internal_iterator_fetch(ZEND_THIS);
	if (!intern) {
		RETURN_THROWS();
	}

	intern->rewind_called = 1;
	if (!intern->iter->funcs->rewind) {
		/* Allow calling rewind() if no iteration has happened yet,
		 * even if the iterator does not support rewinding. */
		if (intern->iter->index != 0) {
			zend_throw_error(NULL, "Iterator does not support rewinding");
			RETURN_THROWS();
		}
		intern->iter->index = 0;
		return;
	}

	intern->iter->funcs->rewind(intern->iter);
	intern->iter->index = 0;
}

/* {{{ zend_register_interfaces */
ZEND_API void zend_register_interfaces(void)
{
	zend_ce_traversable = register_class_Traversable();
	zend_ce_traversable->interface_gets_implemented = zend_implement_traversable;

	zend_ce_aggregate = register_class_IteratorAggregate(zend_ce_traversable);
	zend_ce_aggregate->interface_gets_implemented = zend_implement_aggregate;

	zend_ce_iterator = register_class_Iterator(zend_ce_traversable);
	zend_ce_iterator->interface_gets_implemented = zend_implement_iterator;

	zend_ce_serializable = register_class_Serializable();
	zend_ce_serializable->interface_gets_implemented = zend_implement_serializable;

	zend_ce_arrayaccess = register_class_ArrayAccess();

	zend_ce_countable = register_class_Countable();

	zend_ce_stringable = register_class_Stringable();

	zend_ce_internal_iterator = register_class_InternalIterator(zend_ce_iterator);
	zend_ce_internal_iterator->create_object = zend_internal_iterator_create;

	memcpy(&zend_internal_iterator_handlers, zend_get_std_object_handlers(),
		sizeof(zend_object_handlers));
	zend_internal_iterator_handlers.free_obj = zend_internal_iterator_free;
}
/* }}} */
