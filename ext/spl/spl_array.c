/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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
#include "zend_execute_locks.h"

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

/* {{{ spl_array_writer_default stuff */
typedef struct {
	zval *obj;
	zval *idx;
} spl_array_writer_object;

static zend_class_entry *spl_array_writer_default_get_class(zval *object TSRMLS_DC)
{
#ifdef SPL_ARRAY_WRITE
	return spl_ce_array_writer_default;
#else
	return (zend_class_entry *)1; /* force an error here: this ensures not equal */
#endif
}

static zend_object_handlers spl_array_writer_default_handlers = {
	ZEND_OBJECTS_STORE_HANDLERS,
	
	NULL,     /* read_property */
	NULL,     /* write_property */
	NULL,     /* get_property_ptr */
	NULL,     /* get_property_zval_ptr */
	NULL,     /* get */
	NULL,     /* set */
	NULL,     /* has_property */
	NULL,     /* unset_property */
	NULL,     /* get_properties */
	NULL,     /* get_method */
	NULL,     /* call_method */
	NULL,     /* get_constructor */
	spl_array_writer_default_get_class,     /* get_class_entry */
	NULL,     /* get_class_name */
	NULL      /* compare_objects */
};
/* }}} */

/* {{{ spl_array_writer_dtor */
void spl_array_writer_default_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	spl_array_writer_object *writer = (spl_array_writer_object*) object;

	if (writer->obj)
	{
		writer->obj->refcount--;
/*		DELETE_ZVAL(writer->obj); */
	}
	if (writer->idx)
	{
		writer->idx->refcount--;
		DELETE_ZVAL(writer->idx);
	}
	efree(writer);
}
/* }}} */

/* {{{ spl_array_writer_default_create */
zend_object_value spl_array_writer_default_create(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	spl_array_writer_object *intern;

	intern = ecalloc(sizeof(spl_array_writer_object), 1);

	retval.handle = zend_objects_store_put(intern, spl_array_writer_default_dtor, NULL TSRMLS_CC);
	retval.handlers = &spl_array_writer_default_handlers;

	return retval;
}
/* }}} */

/* {{{ spl_array_writer_default_set */
void spl_array_writer_default_set(zval *object, zval *newval, zval **retval TSRMLS_DC)
{
	spl_array_writer_object *writer;

	writer = (spl_array_writer_object *) zend_object_store_get_object(object TSRMLS_CC);
	spl_begin_method_call_arg_ex2(&writer->obj, NULL, NULL, "set", sizeof("set")-1, retval, writer->idx, newval TSRMLS_CC);
}
/* }}} */

/* {{{ SPL_CLASS_FUNCTION(array_writer_default, __construct) */
SPL_CLASS_FUNCTION(array_writer_default, __construct)
{
	zval *object = getThis();
	zval *obj, *idx;
	spl_array_writer_object *writer;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &obj, &idx) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Failed to parse parameters");
		return;
	}
	writer = (spl_array_writer_object *) zend_object_store_get_object(object TSRMLS_CC);
	writer->obj = obj; obj->refcount++;
	writer->idx = idx; idx->refcount++;

}
/* }}} */

/* {{{ SPL_CLASS_FUNCTION(array_writer_default, set) */
SPL_CLASS_FUNCTION(array_writer_default, set)
{
	zval *object = getThis();
	zval *newval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &newval) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Failed to parse parameters");
		return;
	}
	spl_array_writer_default_set(object, newval, &return_value TSRMLS_CC);
}
/* }}} */

/* {{{ spl_fetch_dimension_address */
int spl_fetch_dimension_address(znode *result, znode *op1, znode *op2, temp_variable *Ts, int type TSRMLS_DC)
{
	zval **container_ptr = spl_get_zval_ptr_ptr(op1, Ts TSRMLS_CC);

	if (spl_is_instance_of(container_ptr, spl_ce_array_read TSRMLS_CC)) {
		zval **retval = &(T(result->u.var).var.ptr);
		zval *dim = spl_get_zval_ptr(op2, Ts, &EG(free_op2) TSRMLS_CC);
		zval *exists;

		/*ALLOC_ZVAL(exists); not needed */
		spl_begin_method_call_arg_ex1(container_ptr, NULL, NULL, "exists", sizeof("exists")-1, &exists, dim TSRMLS_CC);
		if (!i_zend_is_true(exists)) {
			if (type == BP_VAR_R || type == BP_VAR_RW) {
				SEPARATE_ZVAL(&dim);
				convert_to_string_ex(&dim);
				zend_error(E_NOTICE,"Undefined index:  %s", Z_STRVAL_P(dim));
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
			spl_begin_method_call_arg_ex1(container_ptr, NULL, NULL, "get", sizeof("get")-1, retval, dim TSRMLS_CC);
			(*retval)->refcount--;
		} else 
#ifdef SPL_ARRAY_WRITE
		if (spl_is_instance_of(container_ptr, spl_ce_array_access_ex TSRMLS_CC)) {
			/* array_access_ex instaces have their own way of creating an access_writer */
			spl_begin_method_call_arg_ex1(container_ptr, NULL, NULL, "new_writer", sizeof("new_writer")-1, retval, dim TSRMLS_CC);
			T(result->u.var).var.ptr = *retval;
			AI_PTR_2_PTR_PTR(T(result->u.var).var);
			SELECTIVE_PZVAL_LOCK(*retval, result);
		} else if (spl_is_instance_of(container_ptr, spl_ce_array_access TSRMLS_CC)) {
			/* array_access instances create the default array_writer: array_write */
			spl_array_writer_object *writer;
			spl_instanciate(spl_ce_array_writer_default, retval TSRMLS_CC);
			T(result->u.var).var.ptr = *retval;
			AI_PTR_2_PTR_PTR(T(result->u.var).var);
			writer = (spl_array_writer_object *) zend_object_store_get_object(*retval TSRMLS_CC);
			writer->obj = *container_ptr; writer->obj->refcount++;
			writer->idx = dim;            writer->idx->refcount++;
			SELECTIVE_PZVAL_LOCK(*retval, result);
		} else {
			zend_error(E_ERROR, "Object must implement spl_array_access for write access");
			retval = &EG(error_zval_ptr);
		}
		SELECTIVE_PZVAL_LOCK(*retval, result);
#else
		zend_error(E_ERROR, "SPL compiled without array write hook");
#endif
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
			PZVAL_LOCK(*EX_T(EX(opline)->op1.u.var).var.ptr_ptr);
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

/* {{{ ZEND_EXECUTE_HOOK_FUNCTION(ZEND_ASSIGN) */
#ifdef SPL_ARRAY_WRITE
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_ASSIGN)
{
	zval **writer = spl_get_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);
	zval *newval, *retval, *target;
	znode *result;

	if (writer && *writer && Z_TYPE_PP(writer) == IS_OBJECT) {
		/* optimization: do pre checks and only test for handlers in case of
		 * spl_array_writer_default, for spl_array_writer we must use the 
		 * long way of calling spl_instance
		 * if (spl_is_instance_of(writer, spl_ce_array_writer_default TSRMLS_CC))
		 */
		if ((*writer)->value.obj.handlers == &spl_array_writer_default_handlers) {
			newval = spl_get_zval_ptr(&EX(opline)->op2, EX(Ts), &EG(free_op2) TSRMLS_CC);
			spl_array_writer_default_set(*writer, newval, &retval TSRMLS_CC);
		} else if (spl_is_instance_of(writer, spl_ce_array_writer TSRMLS_CC)) {
			newval = spl_get_zval_ptr(&EX(opline)->op2, EX(Ts), &EG(free_op2) TSRMLS_CC);
			spl_begin_method_call_arg_ex1(writer, NULL, NULL, "set", sizeof("set")-1, &retval, newval TSRMLS_CC);
		} else {
			ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_ASSIGN);
		}
	} else {
		ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_ASSIGN);
	}
	spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);

	result = &EX(opline)->result;
	if (result) {
		if (retval->refcount<2) {
			if ((*writer)->value.obj.handlers == &spl_array_writer_default_handlers) {
				spl_array_writer_object *object = (spl_array_writer_object *) zend_object_store_get_object(*writer TSRMLS_CC);
				target = object->obj;
			} else {
				target = *writer;
			}
			zend_error(E_WARNING, "Method %s::set() did not return a value, using NULL", Z_OBJCE_P(target)->name);
			DELETE_ZVAL(retval);
			DELETE_ZVAL(newval);
			/* Unfortunately it doesn't work when trying to return newval.
			 * But anyhow it wouldn't make sense...and confuse reference counting and such.
			 */
			retval = &EG(uninitialized_zval);
		} else {
			retval->refcount--;
		}
		EX_T(EX(opline)->result.u.var).var.ptr = retval;
		AI_PTR_2_PTR_PTR(EX_T(EX(opline)->result.u.var).var);
		SELECTIVE_PZVAL_LOCK(retval, result);
	} else {
		retval->refcount = 1;
		DELETE_ZVAL(retval);
	}

	(*writer)->refcount = 1;
	DELETE_ZVAL(*writer);
	FREE_OP(EX(Ts), &EX(opline)->op2, EG(free_op2));

	NEXT_OPCODE();
}
#endif
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
