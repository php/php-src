/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_compile.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_array.h"

#define DELETE_ZVAL(z) \
	if ((z)->refcount < 2) { \
		zval_dtor(z); \
		FREE_ZVAL(z); /* maybe safe_free_zval_ptr is needed for the uninitialised things */ \
	}

#define DELETE_RET_ZVAL(z) \
	if ((z)->refcount < 3) { \
		zval_dtor(z); \
		FREE_ZVAL(z); /* maybe safe_free_zval_ptr is needed for the uninitialised things */ \
	}

#define AI_PTR_2_PTR_PTR(ai) \
	(ai).ptr_ptr = &((ai).ptr)

/* {{{ spl_fetch_dimension_address */
int spl_fetch_dimension_address(znode *result, znode *op1, znode *op2, temp_variable *Ts, int type TSRMLS_DC)
{
	zval **obj;
	zend_class_entry *obj_ce;
	spl_is_a is_a;

	obj = spl_get_zval_ptr_ptr(op1, Ts TSRMLS_CC);

	if (!obj || (obj_ce = spl_get_class_entry(*obj TSRMLS_CC)) == NULL) {
		return 1;
	}

	is_a = spl_implements(obj_ce);

	if (is_a & SPL_IS_A_ARRAY_READ) {
		zval **retval = &(T(result->u.var).var.ptr);
		zval *dim = spl_get_zval_ptr(op2, Ts, &EG(free_op2) TSRMLS_CC);
		zval *exists;

		spl_call_method_1(obj, obj_ce, NULL, "exists", sizeof("exists")-1, &exists, dim);
		if (!i_zend_is_true(exists)) {
			if (type == BP_VAR_R || type == BP_VAR_RW) {
				SEPARATE_ZVAL(&dim);
				convert_to_string_ex(&dim);
				zend_error(E_NOTICE, "Undefined index:  %s", Z_STRVAL_P(dim));
				DELETE_ZVAL(dim);
			}
			if (type == BP_VAR_R || type == BP_VAR_IS) {
				DELETE_RET_ZVAL(exists);
				*retval = &EG(error_zval);
				(*retval)->refcount++;
				FREE_OP(Ts, op2, EG(free_op2));
				SELECTIVE_PZVAL_LOCK(*retval, result);
				return 0;
			}
		}
		DELETE_RET_ZVAL(exists);
		if (type == BP_VAR_R || type == BP_VAR_IS) {
			spl_call_method_1(obj, obj_ce, NULL, "get", sizeof("get")-1, retval, dim);
		}
		FREE_OP(Ts, op2, EG(free_op2));
		return 0;
	}
	return 1;
}
/* }}} */

/* {{{ ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FETCH_DIM_R) */
#ifdef SPL_ARRAY_READ
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FETCH_DIM_R)
{
	if (!spl_fetch_dimension_address(&EX(opline)->result, &EX(opline)->op1, &EX(opline)->op2, EX(Ts), BP_VAR_R TSRMLS_CC))
	{
		if (EX(opline)->extended_value == ZEND_FETCH_ADD_LOCK) {
			spl_pzval_lock_func(*EX_T(EX(opline)->op1.u.var).var.ptr_ptr);
		}
		spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);

		AI_PTR_2_PTR_PTR(EX_T(EX(opline)->result.u.var).var);
		NEXT_OPCODE();
	}
	ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_FETCH_DIM_R);
}
#endif
/* }}} */

/* {{{ ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FETCH_DIM_W) */
#ifdef SPL_ARRAY_READ
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FETCH_DIM_W)
{
	if (!spl_fetch_dimension_address(&EX(opline)->result, &EX(opline)->op1, &EX(opline)->op2, EX(Ts), BP_VAR_W TSRMLS_CC))
	{
		spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);

		NEXT_OPCODE();
	}
	ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_FETCH_DIM_W);
}
#endif
/* }}} */

/* {{{ ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FETCH_DIM_RW) */
#ifdef SPL_ARRAY_READ
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FETCH_DIM_RW)
{
	if (!spl_fetch_dimension_address(&EX(opline)->result, &EX(opline)->op1, &EX(opline)->op2, EX(Ts), BP_VAR_RW TSRMLS_CC))
	{
		spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);

		NEXT_OPCODE();
	}
	ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_FETCH_DIM_RW);
}
#endif
/* }}} */

static inline zval **spl_get_obj_zval_ptr_ptr(znode *op, temp_variable *Ts, int type TSRMLS_DC)
{
	if (op->op_type == IS_UNUSED) {
		if (EG(This)) {
			/* this should actually never be modified, _ptr_ptr is modified only when
			   the object is empty */
			return &EG(This);
		} else {
			zend_error(E_ERROR, "Using $this when not in object context");
		}
	}
	return spl_get_zval_ptr_ptr(op, Ts TSRMLS_CC);
}

/* {{{ ZEND_EXECUTE_HOOK_FUNCTION(ZEND_ASSIGN_DIM) */
#ifdef SPL_ARRAY_WRITE
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_ASSIGN_DIM)
{
	zval **obj;
	zend_class_entry *obj_ce;
	spl_is_a is_a;

	obj = spl_get_obj_zval_ptr_ptr(&EX(opline)->op1, EX(Ts), 0 TSRMLS_CC);

	if (!obj || (obj_ce = spl_get_class_entry(*obj TSRMLS_CC)) == NULL) {
		ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_ASSIGN_DIM);
	}

	is_a = spl_implements(obj_ce);

	if (is_a & SPL_IS_A_ARRAY_ACCESS) {
		znode *op2 = &EX(opline)->op2;
		zval *index = spl_get_zval_ptr(op2, EX(Ts), &EG(free_op2), BP_VAR_R);
		zval *free_value;
		zend_op *value_op = EX(opline)+1;
		zval *value = spl_get_zval_ptr(&value_op->op1, EX(Ts), &free_value, BP_VAR_R);
		zval tmp;
		zval *retval;

		spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);

		/* here we are sure we are dealing with an object */
		switch (op2->op_type) {
			case IS_CONST:
				/* already a constant string */
				break;
			case IS_VAR:
				tmp = *index;
				zval_copy_ctor(&tmp);
				convert_to_string(&tmp);
				index = &tmp;
				break;
			case IS_TMP_VAR:
				convert_to_string(index);
				break;
		}

		/* separate our value if necessary */
		if (value_op->op1.op_type == IS_TMP_VAR) {
			zval *orig_value = value;
	
			ALLOC_ZVAL(value);
			*value = *orig_value;
			value->is_ref = 0;
			value->refcount = 0;
		}

		spl_call_method_2(obj, obj_ce, NULL, "set", sizeof("set")-1, &retval, index, value);

		if (index == &tmp) {
			zval_dtor(index);
		}

		FREE_OP(Ts, op2, EG(free_op2));
		if (&EX(opline)->result) {
			if (retval->refcount < 2) {
				zend_error(E_WARNING, "Method %s::set() did not return a value, using input value", obj_ce->name);
				EX_T(EX(opline)->result.u.var).var.ptr = value;
				SELECTIVE_PZVAL_LOCK(value, &EX(opline)->result);
				DELETE_RET_ZVAL(retval);			
			} else {
				SELECTIVE_PZVAL_LOCK(retval, &EX(opline)->result);
				EX_T(EX(opline)->result.u.var).var.ptr = retval;
				retval->refcount--;
			}
			EX_T(EX(opline)->result.u.var).var.ptr_ptr = NULL;
		} else {
			DELETE_RET_ZVAL(retval);			
		}

		EX(opline)++;
		NEXT_OPCODE();
	}
	ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_ASSIGN_DIM);
}
#endif
/* }}} */

/* {{{ ZEND_EXECUTE_HOOK_FUNCTION(ZEND_UNSET_DIM_OBJ) */
#ifdef SPL_ARRAY_WRITE
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_UNSET_DIM_OBJ)
{
	zval **obj;
	zend_class_entry *obj_ce;
	spl_is_a is_a;

	if (EX(opline)->extended_value != ZEND_UNSET_DIM) {
		ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_UNSET_DIM_OBJ);
	}

	obj = spl_get_obj_zval_ptr_ptr(&EX(opline)->op1, EX(Ts), 0 TSRMLS_CC);

	if (!obj || (obj_ce = spl_get_class_entry(*obj TSRMLS_CC)) == NULL) {
		ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_UNSET_DIM_OBJ);
	}

	is_a = spl_implements(obj_ce);

	if (is_a & SPL_IS_A_ARRAY_ACCESS) {
		znode *op2 = &EX(opline)->op2;
		zval *index = spl_get_zval_ptr(op2, EX(Ts), &EG(free_op2), BP_VAR_R);
		zval tmp;
		zval *retval;

		spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);

		/* here we are sure we are dealing with an object */
		switch (op2->op_type) {
			case IS_CONST:
				/* already a constant string */
				break;
			case IS_VAR:
				tmp = *index;
				zval_copy_ctor(&tmp);
				convert_to_string(&tmp);
				index = &tmp;
				break;
			case IS_TMP_VAR:
				convert_to_string(index);
				break;
		}

		spl_call_method_1(obj, obj_ce, NULL, "del", sizeof("del")-1, &retval, index);

		if (index == &tmp) {
			zval_dtor(index);
		}

		FREE_OP(Ts, op2, EG(free_op2));
		DELETE_RET_ZVAL(retval);			

		NEXT_OPCODE();
	}
	ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_UNSET_DIM_OBJ);
}
#endif
/* }}} */

SPL_CLASS_FUNCTION(array, __construct);
SPL_CLASS_FUNCTION(array, newiterator);
SPL_CLASS_FUNCTION(array, rewind);
SPL_CLASS_FUNCTION(array, current);
SPL_CLASS_FUNCTION(array, key);
SPL_CLASS_FUNCTION(array, next);
SPL_CLASS_FUNCTION(array, hasmore);

static
ZEND_BEGIN_ARG_INFO(arginfo_array___construct, 0)
	ZEND_ARG_INFO(0, array)
ZEND_END_ARG_INFO();

static zend_function_entry spl_array_class_functions[] = {
	SPL_CLASS_FE(array, __construct,   arginfo_array___construct, ZEND_ACC_PUBLIC)
	SPL_CLASS_FE(array, newiterator,   NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static zend_function_entry spl_array_it_class_functions[] = {
	SPL_CLASS_FE(array, __construct,   arginfo_array___construct, ZEND_ACC_PRIVATE)
	SPL_CLASS_FE(array, rewind,        NULL, ZEND_ACC_PUBLIC)
	SPL_CLASS_FE(array, current,       NULL, ZEND_ACC_PUBLIC)
	SPL_CLASS_FE(array, key,           NULL, ZEND_ACC_PUBLIC)
	SPL_CLASS_FE(array, next,          NULL, ZEND_ACC_PUBLIC)
	SPL_CLASS_FE(array, hasmore,       NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static zend_object_handlers spl_array_handlers;
static zend_class_entry *   spl_ce_array;

static zend_object_handlers spl_array_it_handlers;
static zend_class_entry *   spl_ce_array_it;

typedef struct _spl_array_object {
	zend_object       std;
	zval              *array;
	HashPosition      pos;
} spl_array_object;

/* {{{ spl_array_object_dtor */
static void spl_array_object_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	spl_array_object *intern = (spl_array_object *)object;

	zend_hash_destroy(intern->std.properties);
	FREE_HASHTABLE(intern->std.properties);

	if (!ZVAL_DELREF(intern->array)) {
		zval_dtor(intern->array);
		FREE_ZVAL(intern->array);
	}

	efree(object);
}
/* }}} */

/* {{{ spl_array_object_new */
static zend_object_value spl_array_object_new_ex(zend_class_entry *class_type, spl_array_object **obj, spl_array_object *orig TSRMLS_DC)
{
	zend_object_value retval;
	spl_array_object *intern;
	zval *tmp;

	intern = emalloc(sizeof(spl_array_object));
	memset(intern, 0, sizeof(spl_array_object));
	intern->std.ce = class_type;
	*obj = intern;

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	if (orig) {
		intern->array = orig->array;
		ZVAL_ADDREF(intern->array);
	} else {
		MAKE_STD_ZVAL(intern->array);
		array_init(intern->array);
	}
	zend_hash_internal_pointer_reset_ex(HASH_OF(intern->array), &intern->pos);

	retval.handle = zend_objects_store_put(intern, spl_array_object_dtor, NULL TSRMLS_CC);
	if (class_type == spl_ce_array_it) {
		retval.handlers = &spl_array_it_handlers;
	} else {
		retval.handlers = &spl_array_handlers;
	}
	return retval;
}
/* }}} */

/* {{{ spl_array_object_new */
static zend_object_value spl_array_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	spl_array_object *tmp;
	return spl_array_object_new_ex(class_type, &tmp, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ spl_array_object_clone */
static zend_object_value spl_array_object_clone(zval *zobject TSRMLS_DC)
{
	zend_object_value new_obj_val;
	zend_object *old_object;
	zend_object *new_object;
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);
	spl_array_object *intern;

	old_object = zend_objects_get_address(zobject TSRMLS_CC);
	new_obj_val = spl_array_object_new_ex(old_object->ce, &intern, (spl_array_object*)old_object TSRMLS_CC);
	new_object = &intern->std;

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	return new_obj_val;
}
/* }}} */

/* {{{ spl_array_get_ce */
static zend_class_entry *spl_array_get_ce(zval *object TSRMLS_DC)
{
	return spl_ce_array;
}
/* }}} */

/* {{{ spl_array_it_get_ce */
static zend_class_entry *spl_array_it_get_ce(zval *object TSRMLS_DC)
{
	return spl_ce_array_it;
}
/* }}} */

/* {{{ spl_array_read_dimension */
static zval *spl_array_read_dimension(zval *object, zval *offset TSRMLS_DC)
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	zval **retval;
	long index;

	switch(Z_TYPE_P(offset)) {
	case IS_STRING:
		if (zend_symtable_find(HASH_OF(intern->array), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, (void **) &retval) == FAILURE) {
			zend_error(E_NOTICE,"Undefined index:  %s", Z_STRVAL_P(offset));
			return EG(uninitialized_zval_ptr);
		} else {
			return *retval;
		}
	case IS_DOUBLE:
	case IS_RESOURCE:
	case IS_BOOL: 
	case IS_LONG: 
		if (offset->type == IS_DOUBLE) {
			index = (long)Z_DVAL_P(offset);
		} else {
			index = Z_LVAL_P(offset);
		}
		if (zend_hash_index_find(HASH_OF(intern->array), index, (void **) &retval) == FAILURE) {
			zend_error(E_NOTICE,"Undefined offset:  %ld", Z_LVAL_P(offset));
			return EG(uninitialized_zval_ptr);
		} else {
			return *retval;
		}
		break;
	default:
		zend_error(E_WARNING, "Illegal offset type");
		return EG(uninitialized_zval_ptr);
	}
}
/* }}} */

/* {{{ spl_array_write_dimension */
static void spl_array_write_dimension(zval *object, zval *offset, zval *value TSRMLS_DC)
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	long index;

	switch(Z_TYPE_P(offset)) {
	case IS_STRING:
		zend_symtable_update(HASH_OF(intern->array), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, (void**)&value, sizeof(void*), NULL);
		return;
	case IS_DOUBLE:
	case IS_RESOURCE:
	case IS_BOOL: 
	case IS_LONG: 
		if (offset->type == IS_DOUBLE) {
			index = (long)Z_DVAL_P(offset);
		} else {
			index = Z_LVAL_P(offset);
		}
		add_index_zval(intern->array, index, value);
		return;
	default:
		zend_error(E_WARNING, "Illegal offset type");
		return;
	}
}
/* }}} */

/* {{{ spl_array_unset_dimension */
static void spl_array_unset_dimension(zval *object, zval *offset TSRMLS_DC)
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	long index;

	switch(Z_TYPE_P(offset)) {
	case IS_STRING:
		if (zend_symtable_del(HASH_OF(intern->array), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1) == FAILURE) {
			zend_error(E_NOTICE,"Undefined index:  %s", Z_STRVAL_P(offset));
		}
		return;
	case IS_DOUBLE:
	case IS_RESOURCE:
	case IS_BOOL: 
	case IS_LONG: 
		if (offset->type == IS_DOUBLE) {
			index = (long)Z_DVAL_P(offset);
		} else {
			index = Z_LVAL_P(offset);
		}
		if (zend_hash_index_del(HASH_OF(intern->array), index) == FAILURE) {
			zend_error(E_NOTICE,"Undefined offset:  %ld", Z_LVAL_P(offset));
		}
		return;
	default:
		zend_error(E_WARNING, "Illegal offset type");
		return;
	}
}
/* }}} */

/* {{{ spl_array_get_properties */
static HashTable *spl_array_get_properties(zval *object TSRMLS_DC)
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	return HASH_OF(intern->array);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(spl_array) */
PHP_MINIT_FUNCTION(spl_array)
{
	REGISTER_SPL_STD_CLASS_EX(array, spl_array_object_new, spl_array_class_functions);
	REGISTER_SPL_IMPLEMENT(array, iterator);
	memcpy(&spl_array_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	spl_array_handlers.clone_obj = spl_array_object_clone;
	spl_array_handlers.get_class_entry = spl_array_get_ce;
	spl_array_handlers.read_dimension = spl_array_read_dimension;
	spl_array_handlers.write_dimension = spl_array_write_dimension;
	spl_array_handlers.unset_dimension = spl_array_unset_dimension;
	spl_array_handlers.get_properties = spl_array_get_properties;

	REGISTER_SPL_STD_CLASS_EX(array_it, spl_array_object_new, spl_array_it_class_functions);
	REGISTER_SPL_IMPLEMENT(array_it, sequence_assoc);
	memcpy(&spl_array_it_handlers, &spl_array_handlers, sizeof(zend_object_handlers));
	spl_array_it_handlers.get_class_entry = spl_array_it_get_ce;

	return SUCCESS;
}
/* }}} */

/* {{{ proto void spl_array::__construct(array|object ar = array())
       proto void spl_array_it::__construct(array|object ar = array())
 Cronstructs a new array iterator from a path. */
SPL_CLASS_FUNCTION(array, __construct)
{
	zval *object = getThis();
	spl_array_object *intern;
	zval **array;

	if (ZEND_NUM_ARGS() == 0) {
		return; /* nothing to do */
	}
/* exceptions do not work yet
	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);*/

	if (ZEND_NUM_ARGS() > 1 || zend_get_parameters_ex(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (!HASH_OF(*array)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Passed variable is not an array or object, using empty array instead");
		php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
		return;
	}
	intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	zval_dtor(intern->array);
	FREE_ZVAL(intern->array);
	intern->array = *array;
	ZVAL_ADDREF(intern->array);

	zend_hash_internal_pointer_reset_ex(HASH_OF(intern->array), &intern->pos);

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ proto spl_array_it|NULL spl_array::newIterator()
   Create a new iterator from a spl_array instance */
SPL_CLASS_FUNCTION(array, newiterator)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	spl_array_object *iterator;
	HashTable *aht = HASH_OF(intern->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	return_value->type = IS_OBJECT;
	return_value->value.obj = spl_array_object_new_ex(spl_ce_array_it, &iterator, intern TSRMLS_CC);
	return_value->refcount = 1;
	return_value->is_ref = 1;
}
/* }}} */

/* {{{ spl_hash_pos_exists */
ZEND_API int spl_hash_pos_exists(spl_array_object * intern TSRMLS_DC)
{
	HashTable *ht = HASH_OF(intern->array);
	Bucket *p;

/*	IS_CONSISTENT(ht);*/

/*	HASH_PROTECT_RECURSION(ht);*/
	p = ht->pListHead;
	while (p != NULL) {
		if (p == intern->pos) {
			return SUCCESS;
		}
		p = p->pListNext;
	}
/*	HASH_UNPROTECT_RECURSION(ht); */
	zend_hash_internal_pointer_reset_ex(HASH_OF(intern->array), &intern->pos);
	return FAILURE;
}
/* }}} */

/* {{{ proto void spl_array_it::rewind()
   Rewind array back to the start */
SPL_CLASS_FUNCTION(array, rewind)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	HashTable *aht = HASH_OF(intern->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	zend_hash_internal_pointer_reset_ex(aht, &intern->pos);
}
/* }}} */

/* {{{ proto mixed|false spl_array_it::current()
   Return current array entry */
SPL_CLASS_FUNCTION(array, current)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	zval **entry;
	HashTable *aht = HASH_OF(intern->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	if (intern->array->is_ref && spl_hash_pos_exists(intern TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and internal position is no longer valid");
		RETURN_FALSE;
	}

	if (zend_hash_get_current_data_ex(aht, (void **) &entry, &intern->pos) == FAILURE) {
		RETURN_FALSE;
	}
	*return_value = **entry;
	zval_copy_ctor(return_value);
}
/* }}} */

/* {{{ proto mixed|false spl_array_it::key()
   Return current array key */
SPL_CLASS_FUNCTION(array, key)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	char *string_key;
	uint string_length;
	ulong num_key;
	HashTable *aht = HASH_OF(intern->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	if (intern->array->is_ref && spl_hash_pos_exists(intern TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and internal position is no longer valid");
		RETURN_FALSE;
	}

	switch (zend_hash_get_current_key_ex(aht, &string_key, &string_length, &num_key, 0, &intern->pos)) {
		case HASH_KEY_IS_STRING:
			RETVAL_STRINGL(string_key, string_length - 1, 1);
			break;
		case HASH_KEY_IS_LONG:
			RETVAL_LONG(num_key);
			break;
		case HASH_KEY_NON_EXISTANT:
			return;
	}
}
/* }}} */

/* {{{ proto void spl_array_it::next()
   Move to next entry */
SPL_CLASS_FUNCTION(array, next)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	HashTable *aht = HASH_OF(intern->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	if (intern->array->is_ref && spl_hash_pos_exists(intern TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and internal position is no longer valid");
	} else {
		zend_hash_move_forward_ex(aht, &intern->pos);
	}
}
/* }}} */

/* {{{ proto bool spl_array_it::hasMore()
   Check whether array contains more entries */
SPL_CLASS_FUNCTION(array, hasmore)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	HashTable *aht = HASH_OF(intern->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	if (intern->pos && intern->array->is_ref && spl_hash_pos_exists(intern TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and internal position is no longer valid");
		RETURN_FALSE;
	} else {
		RETURN_BOOL(zend_hash_has_more_elements_ex(aht, &intern->pos) == SUCCESS);
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
