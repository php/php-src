/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: George Schlossnagle <george@omniti.com>                     |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_API.h"
#include "zend_default_classes.h"

extern zend_class_entry *default_exception_ptr;
zend_class_entry *reflection_function_ptr;

static zend_object_handlers reflection_object_handlers;

static void _default_get_entry(zval *object, char *name, int name_len, zval *return_value TSRMLS_DC)
{
	zval **value;

	if (zend_hash_find(Z_OBJPROP_P(object), name, name_len, (void **) &value) == FAILURE) {
		RETURN_FALSE;
	}

	*return_value = **value;
	zval_copy_ctor(return_value);
}

typedef struct {
	zend_object zo;
	void *ptr;
} reflection_object;

static void reflection_objects_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	zend_objects_destroy_object(object, handle TSRMLS_CC);
}

static void reflection_objects_clone(void *object, void **object_clone TSRMLS_DC)
{
	reflection_object *intern = (reflection_object *) object;
	reflection_object **intern_clone = (reflection_object **) object_clone;

	*intern_clone = emalloc(sizeof(reflection_object));
	(*intern_clone)->zo.ce = intern->zo.ce;
	(*intern_clone)->zo.in_get = 0;
	(*intern_clone)->zo.in_set = 0;
	ALLOC_HASHTABLE((*intern_clone)->zo.properties);
	(*intern_clone)->ptr = intern->ptr;
}

ZEND_API zend_object_value reflection_objects_new(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	reflection_object *intern;

	intern = emalloc(sizeof(reflection_object));
	intern->zo.ce = class_type;
	intern->zo.in_get = 0;
	intern->zo.in_set = 0;

	ALLOC_HASHTABLE(intern->zo.properties);
	zend_hash_init(intern->zo.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	retval.handle = zend_objects_store_put(intern, reflection_objects_dtor, reflection_objects_clone TSRMLS_CC);
	retval.handlers =	&reflection_object_handlers;
	return retval;
}

ZEND_FUNCTION(reflection_function)
{
	zval **name;
	zval  *object;
	reflection_object *intern;
	zend_function *fptr;

	int	argc = ZEND_NUM_ARGS();
	if (zend_get_parameters_ex(argc, &name) == FAILURE) {
			ZEND_WRONG_PARAM_COUNT();
	}

	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if(intern == NULL) {
			return;
	}
	convert_to_string_ex(name);
	zval_add_ref(name);
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) name, sizeof(zval *), NULL);
	if(zend_hash_find(EG(function_table), (char *)Z_STRVAL_PP(name), (int)(Z_STRLEN_PP(name) + 1), (void **)&fptr) == FAILURE) {
		zval *ex;
		zval *tmp;
		MAKE_STD_ZVAL(ex);
		object_init_ex(ex, default_exception_ptr);

		MAKE_STD_ZVAL(tmp);
		ZVAL_STRING(tmp, "Function does not exist", 1);
		zend_hash_update(Z_OBJPROP_P(ex), "message", sizeof("message"), (void **) &tmp, sizeof(zval *), NULL);
		tmp = NULL;

		MAKE_STD_ZVAL(tmp);
		ZVAL_STRING(tmp, zend_get_executed_filename(TSRMLS_C), 1);
		zend_hash_update(Z_OBJPROP_P(ex), "file", sizeof("file"), (void **) &tmp, sizeof(zval *), NULL);
		tmp = NULL;

		MAKE_STD_ZVAL(tmp);
		ZVAL_LONG(tmp, zend_get_executed_lineno(TSRMLS_C));
		zend_hash_update(Z_OBJPROP_P(ex), "line", sizeof("line"), (void **) &tmp, sizeof(zval *), NULL);
		tmp = NULL;

		EG(exception) = ex;
	}
	intern->ptr = fptr;
}

ZEND_FUNCTION(reflection_function_getname)
{
	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	_default_get_entry(getThis(), "name", sizeof("name"), return_value TSRMLS_CC);
}

ZEND_FUNCTION(reflection_function_isinternal)
{
	zval  *object;
	reflection_object *intern;
	zend_function *fptr;

	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if(intern == NULL) {
			RETURN_FALSE;
	}
	if((fptr = intern->ptr) == NULL) {
			RETURN_FALSE;
	}
	RETURN_BOOL(fptr->type == ZEND_INTERNAL_FUNCTION);
}

ZEND_FUNCTION(reflection_function_isuserdefined)
{
	zval  *object;
	reflection_object *intern;
	zend_function *fptr;

	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if(intern == NULL) {
			return;
	}
	if((fptr = intern->ptr) == NULL) {
			return;
	}
	RETURN_BOOL(fptr->type == ZEND_USER_FUNCTION);
}

ZEND_FUNCTION(reflection_function_getfilename)
{
	zval  *object;
	reflection_object *intern;
	zend_function *fptr;

	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if(intern == NULL) {
			return;
	}
	if((fptr = intern->ptr) == NULL) {
			return;
	}
	if(fptr->type == ZEND_USER_FUNCTION) {
		RETURN_STRING(fptr->op_array.filename, 1);
	}
	RETURN_FALSE;
}

ZEND_FUNCTION(reflection_function_getstartline)
{
	zval  *object;
	reflection_object *intern;
	zend_function *fptr;

	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if(intern == NULL) {
			return;
	}
	if((fptr = intern->ptr) == NULL) {
			return;
	}
	if(fptr->type == ZEND_USER_FUNCTION) {
		RETURN_LONG(fptr->op_array.line_start);
	}
	RETURN_FALSE;
}

ZEND_FUNCTION(reflection_function_getendline)
{
	zval  *object;
	reflection_object *intern;
	zend_function *fptr;

	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if(intern == NULL) {
			RETURN_FALSE;
	}
	if((fptr = intern->ptr) == NULL) {
			RETURN_FALSE;
	}
	if(fptr->type == ZEND_USER_FUNCTION) {
		RETURN_LONG(fptr->op_array.line_end);
	}
	RETURN_FALSE;
}

ZEND_FUNCTION(reflection_function_getdoccomment)
{
	zval  *object;
	reflection_object *intern;
	zend_function *fptr;

	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if(intern == NULL) {
			RETURN_FALSE;
	}
	if((fptr = intern->ptr) == NULL) {
			RETURN_FALSE;
	}
	if(fptr->type == ZEND_USER_FUNCTION && fptr->op_array.doc_comment) {
		RETURN_STRINGL(fptr->op_array.doc_comment, fptr->op_array.doc_comment_len, 1);
	}
	RETURN_FALSE;
}

ZEND_FUNCTION(reflection_function_getstaticvariables)
{
	zval *object, *tmp_copy;
	reflection_object *intern;
	zend_function *fptr;
	
	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if(intern == NULL) {
		RETURN_FALSE;
	}
	if((fptr = intern->ptr) == NULL || fptr->op_array.static_variables == NULL) {
		RETURN_FALSE;
	}
	array_init(return_value);
	zend_hash_copy(Z_ARRVAL_P(return_value), fptr->op_array.static_variables, (copy_ctor_func_t) zval_add_ref, (void *) &tmp_copy, sizeof(zval *));
}

ZEND_FUNCTION(reflection_function_invoke)
{
	zval *object, *retval_ptr;
	zval ***params;
	reflection_object *intern;
	zend_function *fptr;
	int argc = ZEND_NUM_ARGS();
	
	object = getThis();
	params = safe_emalloc(sizeof(zval **), argc, 0);
	if (zend_get_parameters_array_ex(argc, params) == FAILURE) {
		efree(params);
		RETURN_FALSE;
	}

	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if(intern == NULL || (fptr = intern->ptr) == NULL) {
		efree(params);
		RETURN_FALSE;
	}
	if(fast_call_user_function(EG(function_table), NULL, NULL,
							   &retval_ptr, argc, params, 
							   1, NULL, &fptr TSRMLS_CC) == SUCCESS && retval_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
	}
	efree(params);
}
#define PHP_ME_MAPPING(name, func_name, arg_types) \
    ZEND_NAMED_FE(name, ZEND_FN(func_name), arg_types)

static zend_function_entry reflection_function_functions[] = {
	ZEND_FE(reflection_function, NULL)
	ZEND_NAMED_FE(isinternal, ZEND_FN(reflection_function_isinternal), NULL)
	ZEND_NAMED_FE(isuserdefined, ZEND_FN(reflection_function_isuserdefined), NULL)
	ZEND_NAMED_FE(getname, ZEND_FN(reflection_function_getname), NULL)
	ZEND_NAMED_FE(getfilename, ZEND_FN(reflection_function_getfilename), NULL)
	ZEND_NAMED_FE(getstartline, ZEND_FN(reflection_function_getstartline), NULL)
	ZEND_NAMED_FE(getendline, ZEND_FN(reflection_function_getendline), NULL)
	ZEND_NAMED_FE(getdoccomment, ZEND_FN(reflection_function_getdoccomment), NULL)
	ZEND_NAMED_FE(getstaticvariables, ZEND_FN(reflection_function_getstaticvariables), NULL)
	ZEND_NAMED_FE(invoke, ZEND_FN(reflection_function_invoke), NULL)
	{NULL, NULL, NULL}
};

ZEND_API void zend_register_reflection_api(TSRMLS_D) {
	zend_class_entry _reflection_function;
	memcpy(&reflection_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	INIT_CLASS_ENTRY(_reflection_function, "reflection_function", reflection_function_functions);
	_reflection_function.create_object = reflection_objects_new;
	reflection_function_ptr = zend_register_internal_class(&_reflection_function TSRMLS_CC);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
