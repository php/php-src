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
#include <assert.h>

#include "zend.h"
#include "zend_API.h"
#include "zend_default_classes.h"
#include "zend_operators.h"

extern zend_class_entry *default_exception_ptr;
zend_class_entry *reflection_function_ptr;
zend_class_entry *reflection_class_ptr;

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

static zval * reflection_instanciate(zend_class_entry *pce, zval *object TSRMLS_DC)
{
	if (!object) {
		ALLOC_ZVAL(object);
	}
	Z_TYPE_P(object) = IS_OBJECT;
	object_init_ex(object, pce);
	object->refcount = 1;
	object->is_ref = 1;
	return object;
}

ZEND_FUNCTION(reflection_function)
{
	zval **name;
	zval  *object;
	char *lcname;
	reflection_object *intern;
	zend_function *fptr;

	int	argc = ZEND_NUM_ARGS();
	if (zend_get_parameters_ex(argc, &name) == FAILURE) {
			ZEND_WRONG_PARAM_COUNT();
	}

	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL) {
			return;
	}
	convert_to_string_ex(name);
	zval_add_ref(name);
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) name, sizeof(zval *), NULL);
	lcname = zend_str_tolower_dup((const char *)Z_STRVAL_PP(name), (int) Z_STRLEN_PP(name));
	if (zend_hash_find(EG(function_table), lcname, (int)(Z_STRLEN_PP(name) + 1), (void **)&fptr) == FAILURE) {
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
	efree(lcname);
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
	if (intern == NULL) {
			RETURN_FALSE;
	}
	if ((fptr = intern->ptr) == NULL) {
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
	if (intern == NULL) {
			return;
	}
	if ((fptr = intern->ptr) == NULL) {
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
	if (intern == NULL) {
			return;
	}
	if ((fptr = intern->ptr) == NULL) {
			return;
	}
	if (fptr->type == ZEND_USER_FUNCTION) {
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
	if (intern == NULL) {
			return;
	}
	if ((fptr = intern->ptr) == NULL) {
			return;
	}
	if (fptr->type == ZEND_USER_FUNCTION) {
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
	if (intern == NULL) {
			RETURN_FALSE;
	}
	if ((fptr = intern->ptr) == NULL) {
			RETURN_FALSE;
	}
	if (fptr->type == ZEND_USER_FUNCTION) {
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
	if (intern == NULL) {
			RETURN_FALSE;
	}
	if ((fptr = intern->ptr) == NULL) {
			RETURN_FALSE;
	}
	if (fptr->type == ZEND_USER_FUNCTION && fptr->op_array.doc_comment) {
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
	if (intern == NULL) {
		RETURN_FALSE;
	}
	if ((fptr = intern->ptr) == NULL || fptr->op_array.static_variables == NULL) {
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
	if (intern == NULL || (fptr = intern->ptr) == NULL) {
		efree(params);
		RETURN_FALSE;
	}
	if (fast_call_user_function(EG(function_table), NULL, NULL,
							   &retval_ptr, argc, params, 
							   1, NULL, &fptr TSRMLS_CC) == SUCCESS && retval_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
	}
	efree(params);
}

void reflection_class_factory(zend_class_entry *ce, zval *object TSRMLS_DC)
{
	reflection_object *intern;
	zval *name;
	ALLOC_ZVAL(name);
	ZVAL_STRINGL(name, ce->name, ce->name_length, 1);
	reflection_instanciate(reflection_class_ptr, object TSRMLS_CC);
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	intern->ptr = ce;
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &name, sizeof(zval *), NULL);
}

ZEND_FUNCTION(reflection_class)
{
	zval **name;
	zval  *object;
	reflection_object *intern;
	char *lcname;
	zend_class_entry **ce;

	int	argc = ZEND_NUM_ARGS();
	if (zend_get_parameters_ex(argc, &name) == FAILURE) {
			ZEND_WRONG_PARAM_COUNT();
	}

	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL) {
			return;
	}
	convert_to_string_ex(name);
	zval_add_ref(name);
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) name, sizeof(zval *), NULL);
	lcname = zend_str_tolower_dup((const char *)Z_STRVAL_PP(name), (int) Z_STRLEN_PP(name));
	if (zend_hash_find(EG(class_table), lcname, (int)(Z_STRLEN_PP(name) + 1), (void **)&ce) == FAILURE) {
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
	efree(lcname);
	intern->ptr = *ce;
}

ZEND_FUNCTION(reflection_class_getname)
{
	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	_default_get_entry(getThis(), "name", sizeof("name"), return_value TSRMLS_CC);
}

ZEND_FUNCTION(reflection_class_isinternal)
{
	zval  *object;
	reflection_object *intern;
	zend_class_entry *ce;

	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL || (ce = intern->ptr) == NULL) {
			RETURN_FALSE;
	}
	RETURN_BOOL(ce->type == ZEND_INTERNAL_CLASS);
}

ZEND_FUNCTION(reflection_class_isuserdefined)
{
	zval  *object;
	reflection_object *intern;
	zend_class_entry *ce;

	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL || (ce = intern->ptr) == NULL) {
			RETURN_FALSE;
	}
	RETURN_BOOL(ce->type == ZEND_USER_CLASS);
}

ZEND_FUNCTION(reflection_class_getfilename)
{
	zval  *object;
	reflection_object *intern;
	zend_class_entry *ce;

	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL || (ce = intern->ptr) == NULL) {
		RETURN_FALSE;
	}
	if (ce->type == ZEND_USER_CLASS) {
		RETURN_STRING(ce->filename, 1);
	}
	RETURN_FALSE;
}

ZEND_FUNCTION(reflection_class_getstartline)
{
	zval  *object;
	reflection_object *intern;
	zend_class_entry *ce;

	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL  || ((ce = intern->ptr) == NULL)) {
			RETURN_FALSE;
	}
	if (ce->type == ZEND_USER_FUNCTION) {
		RETURN_LONG(ce->line_start);
	}
	RETURN_FALSE;
}

ZEND_FUNCTION(reflection_class_getendline)
{
	zval  *object;
	reflection_object *intern;
	zend_class_entry *ce;

	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL  || (ce = intern->ptr) == NULL) {
			RETURN_FALSE;
	}
	if (ce->type == ZEND_USER_CLASS) {
		RETURN_LONG(ce->line_end);
	}
	RETURN_FALSE;
}

ZEND_FUNCTION(reflection_class_getdoccomment)
{
	zval  *object;
	reflection_object *intern;
	zend_class_entry *ce;

	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL  && (ce = intern->ptr) == NULL) {
			RETURN_FALSE;
	}
	if (ce->type == ZEND_USER_CLASS && ce->doc_comment) {
		RETURN_STRINGL(ce->doc_comment, ce->doc_comment_len, 1);
	}
	RETURN_FALSE;
}

ZEND_FUNCTION(reflection_class_getconstants)
{
	zval *object, *tmp_copy;
	reflection_object *intern;
	zend_class_entry *ce;
	
	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL  || (ce = intern->ptr) == NULL) {
		RETURN_FALSE;
	}
	array_init(return_value);
	zend_hash_copy(Z_ARRVAL_P(return_value), &ce->constants_table, (copy_ctor_func_t) zval_add_ref, (void *) &tmp_copy, sizeof(zval *));
}

ZEND_FUNCTION(reflection_class_getconstant)
{
	zval *object;
	reflection_object *intern;
	zend_class_entry *ce;
	zval **value;
	zval **name; 
	int argc = ZEND_NUM_ARGS();

	if (zend_get_parameters_ex(argc, &name) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL  || (ce = intern->ptr) == NULL) {
		RETURN_FALSE;
	}
	if (zend_hash_find(&ce->constants_table, Z_STRVAL_PP(name), Z_STRLEN_PP(name) + 1, (void **) &value) == FAILURE) {
		RETURN_FALSE;
	}
	*return_value = **value;
	zval_copy_ctor(return_value);
}

ZEND_FUNCTION(reflection_class_isinterface)
{
	zval  *object;
	reflection_object *intern;
	zend_class_entry *ce;

	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL || (ce = intern->ptr) == NULL) {
			RETURN_FALSE;
	}
	RETURN_BOOL(ce->type & ZEND_ACC_INTERFACE);
}

ZEND_FUNCTION(reflection_class_getinterfaces)
{
	zval  *object;
	reflection_object *intern;
	zend_class_entry *ce;

	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL || (ce = intern->ptr) == NULL) {
			RETURN_FALSE;
	}
	if (ce->num_interfaces) {
		int i;
		array_init(return_value);	
	   	for(i=0; i < ce->num_interfaces; i++) {
			zval *interface;
			ALLOC_ZVAL(interface);
			reflection_class_factory(ce->interfaces[i], interface TSRMLS_CC);
			add_next_index_zval(return_value, interface);
		}
	}
}

ZEND_FUNCTION(reflection_class_getparentclass)
{
	zval  *object;
	reflection_object *intern;
	zend_class_entry *ce;

	if (ZEND_NUM_ARGS() > 0) { 
		ZEND_WRONG_PARAM_COUNT();
	}
	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern && (ce = intern->ptr)  && ce->parent) {
		reflection_class_factory(ce->parent, return_value TSRMLS_CC);
	} 
	else {
		RETURN_FALSE;
	}
}

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

static zend_function_entry reflection_class_functions[] = {
	ZEND_FE(reflection_class, NULL)
	ZEND_NAMED_FE(getname, ZEND_FN(reflection_class_getname), NULL)
	ZEND_NAMED_FE(isinternal, ZEND_FN(reflection_class_isinternal), NULL)
	ZEND_NAMED_FE(isuserdefined, ZEND_FN(reflection_class_isuserdefined), NULL)
	ZEND_NAMED_FE(getfilename, ZEND_FN(reflection_class_getfilename), NULL)
	ZEND_NAMED_FE(getstartline, ZEND_FN(reflection_class_getstartline), NULL)
	ZEND_NAMED_FE(getendline, ZEND_FN(reflection_class_getendline), NULL)
	ZEND_NAMED_FE(getdoccomment, ZEND_FN(reflection_class_getdoccomment), NULL)
	ZEND_NAMED_FE(getconstants, ZEND_FN(reflection_class_getconstants), NULL)
	ZEND_NAMED_FE(getconstant, ZEND_FN(reflection_class_getconstant), NULL)
	ZEND_NAMED_FE(getinterfaces, ZEND_FN(reflection_class_getinterfaces), NULL)
	ZEND_NAMED_FE(isinterface, ZEND_FN(reflection_class_isinterface), NULL)
	ZEND_NAMED_FE(getparentclass, ZEND_FN(reflection_class_getparentclass), NULL)
	{NULL, NULL, NULL}
};

ZEND_API void zend_register_reflection_api(TSRMLS_D) {
	zend_class_entry _reflection_entry;
	memcpy(&reflection_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	INIT_CLASS_ENTRY(_reflection_entry, "reflection_function", reflection_function_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_function_ptr = zend_register_internal_class(&_reflection_entry TSRMLS_CC);
	INIT_CLASS_ENTRY(_reflection_entry, "reflection_class", reflection_class_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_class_ptr = zend_register_internal_class(&_reflection_entry TSRMLS_CC);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
