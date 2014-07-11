/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
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

/* $Id$ */

#include "zend.h"
#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"

ZEND_API zend_class_entry *zend_ce_traversable;
ZEND_API zend_class_entry *zend_ce_aggregate;
ZEND_API zend_class_entry *zend_ce_iterator;
ZEND_API zend_class_entry *zend_ce_arrayaccess;
ZEND_API zend_class_entry *zend_ce_serializable;

/* {{{ zend_call_method
 Only returns the returned zval if retval_ptr != NULL */
ZEND_API zval* zend_call_method(zval **object_pp, zend_class_entry *obj_ce, zend_function **fn_proxy, const char *function_name, int function_name_len, zval **retval_ptr_ptr, int param_count, zval* arg1, zval* arg2 TSRMLS_DC)
{
	int result;
	zend_fcall_info fci;
	zval z_fname;
	zval *retval;
	HashTable *function_table;

	zval **params[2];

	params[0] = &arg1;
	params[1] = &arg2;

	fci.size = sizeof(fci);
	/*fci.function_table = NULL; will be read form zend_class_entry of object if needed */
	fci.object_ptr = object_pp ? *object_pp : NULL;
	fci.function_name = &z_fname;
	fci.retval_ptr_ptr = retval_ptr_ptr ? retval_ptr_ptr : &retval;
	fci.param_count = param_count;
	fci.params = params;
	fci.no_separation = 1;
	fci.symbol_table = NULL;

	if (!fn_proxy && !obj_ce) {
		/* no interest in caching and no information already present that is
		 * needed later inside zend_call_function. */
		ZVAL_STRINGL(&z_fname, function_name, function_name_len, 0);
		fci.function_table = !object_pp ? EG(function_table) : NULL;
		result = zend_call_function(&fci, NULL TSRMLS_CC);
	} else {
		zend_fcall_info_cache fcic;

		fcic.initialized = 1;
		if (!obj_ce) {
			obj_ce = object_pp ? Z_OBJCE_PP(object_pp) : NULL;
		}
		if (obj_ce) {
			function_table = &obj_ce->function_table;
		} else {
			function_table = EG(function_table);
		}
		if (!fn_proxy || !*fn_proxy) {
			if (zend_hash_find(function_table, function_name, function_name_len+1, (void **) &fcic.function_handler) == FAILURE) {
				/* error at c-level */
				zend_error(E_CORE_ERROR, "Couldn't find implementation for method %s%s%s", obj_ce ? obj_ce->name : "", obj_ce ? "::" : "", function_name);
			}
			if (fn_proxy) {
				*fn_proxy = fcic.function_handler;
			}
		} else {
			fcic.function_handler = *fn_proxy;
		}
		fcic.calling_scope = obj_ce;
		if (object_pp) {
			fcic.called_scope = Z_OBJCE_PP(object_pp);
		} else if (obj_ce &&
		           !(EG(called_scope) &&
		             instanceof_function(EG(called_scope), obj_ce TSRMLS_CC))) {
			fcic.called_scope = obj_ce;
		} else {
			fcic.called_scope = EG(called_scope);
		}
		fcic.object_ptr = object_pp ? *object_pp : NULL;
		result = zend_call_function(&fci, &fcic TSRMLS_CC);
	}
	if (result == FAILURE) {
		/* error at c-level */
		if (!obj_ce) {
			obj_ce = object_pp ? Z_OBJCE_PP(object_pp) : NULL;
		}
		if (!EG(exception)) {
			zend_error(E_CORE_ERROR, "Couldn't execute method %s%s%s", obj_ce ? obj_ce->name : "", obj_ce ? "::" : "", function_name);
		}
	}
	if (!retval_ptr_ptr) {
		if (retval) {
			zval_ptr_dtor(&retval);
		}
		return NULL;
	}
	return *retval_ptr_ptr;
}
/* }}} */

/* iterator interface, c-level functions used by engine */

/* {{{ zend_user_it_new_iterator */
ZEND_API zval *zend_user_it_new_iterator(zend_class_entry *ce, zval *object TSRMLS_DC)
{
	zval *retval;

	return zend_call_method_with_0_params(&object, ce, &ce->iterator_funcs.zf_new_iterator, "getiterator", &retval);

}
/* }}} */

/* {{{ zend_user_it_invalidate_current */
ZEND_API void zend_user_it_invalidate_current(zend_object_iterator *_iter TSRMLS_DC)
{
	zend_user_iterator *iter = (zend_user_iterator*)_iter;

	if (iter->value) {
		zval_ptr_dtor(&iter->value);
		iter->value = NULL;
	}
}
/* }}} */

/* {{{ zend_user_it_dtor */
static void zend_user_it_dtor(zend_object_iterator *_iter TSRMLS_DC)
{
	zend_user_iterator *iter = (zend_user_iterator*)_iter;
	zval *object = (zval*)iter->it.data;

	zend_user_it_invalidate_current(_iter TSRMLS_CC);
	zval_ptr_dtor(&object);
	efree(iter);
}
/* }}} */

/* {{{ zend_user_it_valid */
ZEND_API int zend_user_it_valid(zend_object_iterator *_iter TSRMLS_DC)
{
	if (_iter) {
		zend_user_iterator *iter = (zend_user_iterator*)_iter;
		zval *object = (zval*)iter->it.data;
		zval *more;
		int result;

		zend_call_method_with_0_params(&object, iter->ce, &iter->ce->iterator_funcs.zf_valid, "valid", &more);
		if (more) {
			result = i_zend_is_true(more TSRMLS_CC);
			zval_ptr_dtor(&more);
			return result ? SUCCESS : FAILURE;
		}
	}
	return FAILURE;
}
/* }}} */

/* {{{ zend_user_it_get_current_data */
ZEND_API void zend_user_it_get_current_data(zend_object_iterator *_iter, zval ***data TSRMLS_DC)
{
	zend_user_iterator *iter = (zend_user_iterator*)_iter;
	zval *object = (zval*)iter->it.data;

	if (!iter->value) {
		zend_call_method_with_0_params(&object, iter->ce, &iter->ce->iterator_funcs.zf_current, "current", &iter->value);
	}
	*data = &iter->value;
}
/* }}} */

/* {{{ zend_user_it_get_current_key_default */
#if 0
static int zend_user_it_get_current_key_default(zend_object_iterator *_iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC)
{
	*int_key = _iter->index;
	return HASH_KEY_IS_LONG;
}
#endif
/* }}} */

/* {{{ zend_user_it_get_current_key */
ZEND_API void zend_user_it_get_current_key(zend_object_iterator *_iter, zval *key TSRMLS_DC)
{
	zend_user_iterator *iter = (zend_user_iterator*)_iter;
	zval *object = (zval*)iter->it.data;
	zval *retval;

	zend_call_method_with_0_params(&object, iter->ce, &iter->ce->iterator_funcs.zf_key, "key", &retval);

	if (retval) {
		ZVAL_ZVAL(key, retval, 1, 1);
	} else {
		if (!EG(exception)) {
			zend_error(E_WARNING, "Nothing returned from %s::key()", iter->ce->name);
		}

		ZVAL_LONG(key, 0);
	}
}

/* {{{ zend_user_it_move_forward */
ZEND_API void zend_user_it_move_forward(zend_object_iterator *_iter TSRMLS_DC)
{
	zend_user_iterator *iter = (zend_user_iterator*)_iter;
	zval *object = (zval*)iter->it.data;

	zend_user_it_invalidate_current(_iter TSRMLS_CC);
	zend_call_method_with_0_params(&object, iter->ce, &iter->ce->iterator_funcs.zf_next, "next", NULL);
}
/* }}} */

/* {{{ zend_user_it_rewind */
ZEND_API void zend_user_it_rewind(zend_object_iterator *_iter TSRMLS_DC)
{
	zend_user_iterator *iter = (zend_user_iterator*)_iter;
	zval *object = (zval*)iter->it.data;

	zend_user_it_invalidate_current(_iter TSRMLS_CC);
	zend_call_method_with_0_params(&object, iter->ce, &iter->ce->iterator_funcs.zf_rewind, "rewind", NULL);
}
/* }}} */

zend_object_iterator_funcs zend_interface_iterator_funcs_iterator = {
	zend_user_it_dtor,
	zend_user_it_valid,
	zend_user_it_get_current_data,
	zend_user_it_get_current_key,
	zend_user_it_move_forward,
	zend_user_it_rewind,
	zend_user_it_invalidate_current
};

/* {{{ zend_user_it_get_iterator */
static zend_object_iterator *zend_user_it_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC)
{
	zend_user_iterator *iterator;

	if (by_ref) {
		zend_error(E_ERROR, "An iterator cannot be used with foreach by reference");
	}

	iterator = emalloc(sizeof(zend_user_iterator));

	Z_ADDREF_P(object);
	iterator->it.data = (void*)object;
	iterator->it.funcs = ce->iterator_funcs.funcs;
	iterator->ce = Z_OBJCE_P(object);
	iterator->value = NULL;
	return (zend_object_iterator*)iterator;
}
/* }}} */

/* {{{ zend_user_it_get_new_iterator */
ZEND_API zend_object_iterator *zend_user_it_get_new_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC)
{
	zval *iterator = zend_user_it_new_iterator(ce, object TSRMLS_CC);
	zend_object_iterator *new_iterator;

	zend_class_entry *ce_it = iterator && Z_TYPE_P(iterator) == IS_OBJECT ? Z_OBJCE_P(iterator) : NULL;

	if (!ce_it || !ce_it->get_iterator || (ce_it->get_iterator == zend_user_it_get_new_iterator && iterator == object)) {
		if (!EG(exception)) {
			zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "Objects returned by %s::getIterator() must be traversable or implement interface Iterator", ce ? ce->name : Z_OBJCE_P(object)->name);
		}
		if (iterator) {
			zval_ptr_dtor(&iterator);
		}
		return NULL;
	}

	new_iterator = ce_it->get_iterator(ce_it, iterator, by_ref TSRMLS_CC);
	zval_ptr_dtor(&iterator);
	return new_iterator;
}
/* }}} */

/* {{{ zend_implement_traversable */
static int zend_implement_traversable(zend_class_entry *interface, zend_class_entry *class_type TSRMLS_DC)
{
	/* check that class_type is traversable at c-level or implements at least one of 'aggregate' and 'Iterator' */
	zend_uint i;

	if (class_type->get_iterator || (class_type->parent && class_type->parent->get_iterator)) {
		return SUCCESS;
	}
	for (i = 0; i < class_type->num_interfaces; i++) {
		if (class_type->interfaces[i] == zend_ce_aggregate || class_type->interfaces[i] == zend_ce_iterator) {
			return SUCCESS;
		}
	}
	zend_error(E_CORE_ERROR, "Class %s must implement interface %s as part of either %s or %s",
		class_type->name,
		zend_ce_traversable->name,
		zend_ce_iterator->name,
		zend_ce_aggregate->name);
	return FAILURE;
}
/* }}} */

/* {{{ zend_implement_aggregate */
static int zend_implement_aggregate(zend_class_entry *interface, zend_class_entry *class_type TSRMLS_DC)
{
	int i, t = -1;

	if (class_type->get_iterator) {
		if (class_type->type == ZEND_INTERNAL_CLASS) {
			/* inheritance ensures the class has necessary userland methods */
			return SUCCESS;
		} else if (class_type->get_iterator != zend_user_it_get_new_iterator) {
			/* c-level get_iterator cannot be changed (exception being only Traversable is implmented) */
			if (class_type->num_interfaces) {
				for (i = 0; i < class_type->num_interfaces; i++) {
					if (class_type->interfaces[i] == zend_ce_iterator) {
						zend_error(E_ERROR, "Class %s cannot implement both %s and %s at the same time",
									class_type->name,
									interface->name,
									zend_ce_iterator->name);
						return FAILURE;
					}
					if (class_type->interfaces[i] == zend_ce_traversable) {
						t = i;
					}
				}
			}
			if (t == -1) {
				return FAILURE;
			}
		}
	}
	class_type->iterator_funcs.zf_new_iterator = NULL;
	class_type->get_iterator = zend_user_it_get_new_iterator;
	return SUCCESS;
}
/* }}} */

/* {{{ zend_implement_iterator */
static int zend_implement_iterator(zend_class_entry *interface, zend_class_entry *class_type TSRMLS_DC)
{
	if (class_type->get_iterator && class_type->get_iterator != zend_user_it_get_iterator) {
		if (class_type->type == ZEND_INTERNAL_CLASS) {
			/* inheritance ensures the class has the necessary userland methods */
			return SUCCESS;
		} else {
			/* c-level get_iterator cannot be changed */
			if (class_type->get_iterator == zend_user_it_get_new_iterator) {
				zend_error(E_ERROR, "Class %s cannot implement both %s and %s at the same time",
							class_type->name,
							interface->name,
							zend_ce_aggregate->name);
			}
			return FAILURE;
		}
	}
	class_type->get_iterator = zend_user_it_get_iterator;
	class_type->iterator_funcs.zf_valid = NULL;
	class_type->iterator_funcs.zf_current = NULL;
	class_type->iterator_funcs.zf_key = NULL;
	class_type->iterator_funcs.zf_next = NULL;
	class_type->iterator_funcs.zf_rewind = NULL;
	if (!class_type->iterator_funcs.funcs) {
		class_type->iterator_funcs.funcs = &zend_interface_iterator_funcs_iterator;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ zend_implement_arrayaccess */
static int zend_implement_arrayaccess(zend_class_entry *interface, zend_class_entry *class_type TSRMLS_DC)
{
#if 0
	/* get ht from ce */
	if (ht->read_dimension != zend_std_read_dimension
	||  ht->write_dimension != zend_std_write_dimension
	||  ht->has_dimension != zend_std_has_dimension
	||  ht->unset_dimension != zend_std_unset_dimension) {
		return FAILURE;
	}
#endif
	return SUCCESS;
}
/* }}}*/

/* {{{ zend_user_serialize */
ZEND_API int zend_user_serialize(zval *object, unsigned char **buffer, zend_uint *buf_len, zend_serialize_data *data TSRMLS_DC)
{
	zend_class_entry * ce = Z_OBJCE_P(object);
	zval *retval;
	int result;

	zend_call_method_with_0_params(&object, ce, &ce->serialize_func, "serialize", &retval);


	if (!retval || EG(exception)) {
		result = FAILURE;
	} else {
		switch(Z_TYPE_P(retval)) {
		case IS_NULL:
			/* we could also make this '*buf_len = 0' but this allows to skip variables */
			zval_ptr_dtor(&retval);
			return FAILURE;
		case IS_STRING:
			*buffer = (unsigned char*)estrndup(Z_STRVAL_P(retval), Z_STRLEN_P(retval));
			*buf_len = Z_STRLEN_P(retval);
			result = SUCCESS;
			break;
		default: /* failure */
			result = FAILURE;
			break;
		}
		zval_ptr_dtor(&retval);
	}

	if (result == FAILURE && !EG(exception)) {
		zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "%s::serialize() must return a string or NULL", ce->name);
	}
	return result;
}
/* }}} */

/* {{{ zend_user_unserialize */
ZEND_API int zend_user_unserialize(zval **object, zend_class_entry *ce, const unsigned char *buf, zend_uint buf_len, zend_unserialize_data *data TSRMLS_DC)
{
	zval * zdata;

	object_init_ex(*object, ce);

	MAKE_STD_ZVAL(zdata);
	ZVAL_STRINGL(zdata, (char*)buf, buf_len, 1);

	zend_call_method_with_1_params(object, ce, &ce->unserialize_func, "unserialize", NULL, zdata);

	zval_ptr_dtor(&zdata);

	if (EG(exception)) {
		return FAILURE;
	} else {
		return SUCCESS;
	}
}
/* }}} */

ZEND_API int zend_class_serialize_deny(zval *object, unsigned char **buffer, zend_uint *buf_len, zend_serialize_data *data TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce = Z_OBJCE_P(object);
	zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "Serialization of '%s' is not allowed", ce->name);
	return FAILURE;
}
/* }}} */

ZEND_API int zend_class_unserialize_deny(zval **object, zend_class_entry *ce, const unsigned char *buf, zend_uint buf_len, zend_unserialize_data *data TSRMLS_DC) /* {{{ */
{
	zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "Unserialization of '%s' is not allowed", ce->name);
	return FAILURE;
}
/* }}} */

/* {{{ zend_implement_serializable */
static int zend_implement_serializable(zend_class_entry *interface, zend_class_entry *class_type TSRMLS_DC)
{
	if (class_type->parent
		&& (class_type->parent->serialize || class_type->parent->unserialize)
		&& !instanceof_function_ex(class_type->parent, zend_ce_serializable, 1 TSRMLS_CC)) {
		return FAILURE;
	}
	if (!class_type->serialize) {
		class_type->serialize = zend_user_serialize;
	}
	if (!class_type->unserialize) {
		class_type->unserialize = zend_user_unserialize;
	}
	return SUCCESS;
}
/* }}}*/

/* {{{ function tables */
const zend_function_entry zend_funcs_aggregate[] = {
	ZEND_ABSTRACT_ME(iterator, getIterator, NULL)
	{NULL, NULL, NULL}
};

const zend_function_entry zend_funcs_iterator[] = {
	ZEND_ABSTRACT_ME(iterator, current,  NULL)
	ZEND_ABSTRACT_ME(iterator, next,     NULL)
	ZEND_ABSTRACT_ME(iterator, key,      NULL)
	ZEND_ABSTRACT_ME(iterator, valid,    NULL)
	ZEND_ABSTRACT_ME(iterator, rewind,   NULL)
	{NULL, NULL, NULL}
};

const zend_function_entry *zend_funcs_traversable    = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_arrayaccess_offset, 0, 0, 1)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_arrayaccess_offset_get, 0, 0, 1) /* actually this should be return by ref but atm cannot be */
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_arrayaccess_offset_value, 0, 0, 2)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

const zend_function_entry zend_funcs_arrayaccess[] = {
	ZEND_ABSTRACT_ME(arrayaccess, offsetExists, arginfo_arrayaccess_offset)
	ZEND_ABSTRACT_ME(arrayaccess, offsetGet,    arginfo_arrayaccess_offset_get)
	ZEND_ABSTRACT_ME(arrayaccess, offsetSet,    arginfo_arrayaccess_offset_value)
	ZEND_ABSTRACT_ME(arrayaccess, offsetUnset,  arginfo_arrayaccess_offset)
	{NULL, NULL, NULL}
};

ZEND_BEGIN_ARG_INFO(arginfo_serializable_serialize, 0)
	ZEND_ARG_INFO(0, serialized)
ZEND_END_ARG_INFO()

const zend_function_entry zend_funcs_serializable[] = {
	ZEND_ABSTRACT_ME(serializable, serialize,   NULL)
	ZEND_FENTRY(unserialize, NULL, arginfo_serializable_serialize, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT|ZEND_ACC_CTOR)
	{NULL, NULL, NULL}
};
/* }}} */

#define REGISTER_ITERATOR_INTERFACE(class_name, class_name_str) \
	{\
		zend_class_entry ce;\
		INIT_CLASS_ENTRY(ce, # class_name_str, zend_funcs_ ## class_name) \
		zend_ce_ ## class_name = zend_register_internal_interface(&ce TSRMLS_CC);\
		zend_ce_ ## class_name->interface_gets_implemented = zend_implement_ ## class_name;\
	}

#define REGISTER_ITERATOR_IMPLEMENT(class_name, interface_name) \
	zend_class_implements(zend_ce_ ## class_name TSRMLS_CC, 1, zend_ce_ ## interface_name)

/* {{{ zend_register_interfaces */
ZEND_API void zend_register_interfaces(TSRMLS_D)
{
	REGISTER_ITERATOR_INTERFACE(traversable, Traversable);

	REGISTER_ITERATOR_INTERFACE(aggregate, IteratorAggregate);
	REGISTER_ITERATOR_IMPLEMENT(aggregate, traversable);

	REGISTER_ITERATOR_INTERFACE(iterator, Iterator);
	REGISTER_ITERATOR_IMPLEMENT(iterator, traversable);

	REGISTER_ITERATOR_INTERFACE(arrayaccess, ArrayAccess);

	REGISTER_ITERATOR_INTERFACE(serializable, Serializable)
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
