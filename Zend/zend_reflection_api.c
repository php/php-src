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

#define METHOD_NOTSTATIC                                                                             \
	if (!this_ptr) {                                                                                 \
		zend_error(E_ERROR, "%s() cannot be called statically", get_active_function_name(TSRMLS_C)); \
		return;                                                                                      \
	}                                                                                                \

#define METHOD_NOTSTATIC_NUMPARAMS(c) METHOD_NOTSTATIC                                               \
	if (ZEND_NUM_ARGS() > c) {                                                                       \
		ZEND_WRONG_PARAM_COUNT();                                                                    \
	}                                                                                                \

#define GET_REFLECTION_OBJECT_PTR(target)                                                            \
	intern = (reflection_object *) zend_object_store_get_object(getThis() TSRMLS_CC);                \
	if (intern == NULL || (target = intern->ptr) == NULL) {                                          \
		zend_error(E_ERROR, "Internal error: Failed to retrieve the reflection object");             \
	}                                                                                                \

#define _DO_THROW(msg) \
    extern zend_class_entry *default_exception_ptr; \
	zval *ex; \
	zval *tmp; \
	MAKE_STD_ZVAL(ex); \
	object_init_ex(ex, default_exception_ptr); \
\
	MAKE_STD_ZVAL(tmp); \
	ZVAL_STRING(tmp, (msg), 1); \
	zend_hash_update(Z_OBJPROP_P(ex), "message", sizeof("message"), (void **) &tmp, sizeof(zval *), NULL); \
	tmp = NULL; \
\
	MAKE_STD_ZVAL(tmp); \
	ZVAL_STRING(tmp, zend_get_executed_filename(TSRMLS_C), 1); \
	zend_hash_update(Z_OBJPROP_P(ex), "file", sizeof("file"), (void **) &tmp, sizeof(zval *), NULL); \
	tmp = NULL; \
\
	MAKE_STD_ZVAL(tmp); \
	ZVAL_LONG(tmp, zend_get_executed_lineno(TSRMLS_C)); \
	zend_hash_update(Z_OBJPROP_P(ex), "line", sizeof("line"), (void **) &tmp, sizeof(zval *), NULL); \
	tmp = NULL; \
\
	EG(exception) = ex;


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

/* {{{ proto Reflection_Function Reflection_Function::__construct(string name)
   Constructor. Throws an Exception in case the given function does not exist */
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
		_DO_THROW("Function does not exist");
	}
	efree(lcname);
	intern->ptr = fptr;
}
/* }}} */

/* {{{ proto string Reflection_Function::getName()
   Returns this function's name */
ZEND_FUNCTION(reflection_function_getname)
{
	METHOD_NOTSTATIC_NUMPARAMS(0);
	_default_get_entry(getThis(), "name", sizeof("name"), return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto bool Reflection_Function::isInternal()
   Returns whether this is an internal function */
ZEND_FUNCTION(reflection_function_isinternal)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(fptr);
	RETURN_BOOL(fptr->type == ZEND_INTERNAL_FUNCTION);
}
/* }}} */

/* {{{ proto bool Reflection_Function::isUserDefined()
   Returns whether this is an user-defined function */
ZEND_FUNCTION(reflection_function_isuserdefined)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(fptr);
	RETURN_BOOL(fptr->type == ZEND_USER_FUNCTION);
}
/* }}} */

/* {{{ proto string Reflection_Function::getFileName()
   Returns the filename of the file this function was declared in */
ZEND_FUNCTION(reflection_function_getfilename)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(fptr);
	if (fptr->type == ZEND_USER_FUNCTION) {
		RETURN_STRING(fptr->op_array.filename, 1);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int Reflection_Function::getStartLine()
   Returns the line this function's declaration starts at */
ZEND_FUNCTION(reflection_function_getstartline)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(fptr);
	if (fptr->type == ZEND_USER_FUNCTION) {
		RETURN_LONG(fptr->op_array.line_start);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int Reflection_Function::getEndLine()
   Returns the line this function's declaration ends at */
ZEND_FUNCTION(reflection_function_getendline)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(fptr);
	if (fptr->type == ZEND_USER_FUNCTION) {
		RETURN_LONG(fptr->op_array.line_end);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string Reflection_Function::getDocComment()
   Returns the doc comment for this function */
ZEND_FUNCTION(reflection_function_getdoccomment)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(fptr);
	if (fptr->type == ZEND_USER_FUNCTION && fptr->op_array.doc_comment) {
		RETURN_STRINGL(fptr->op_array.doc_comment, fptr->op_array.doc_comment_len, 1);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Reflection_Function::getStaticVariables()
   Returns an associative array containing this function's static variables and their values */
ZEND_FUNCTION(reflection_function_getstaticvariables)
{
	zval *tmp_copy;
	reflection_object *intern;
	zend_function *fptr;
	
	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(fptr);

	/* Return an empty array in case no static variables exist */
	array_init(return_value);
	if (fptr->op_array.static_variables != NULL) {
		zend_hash_copy(Z_ARRVAL_P(return_value), fptr->op_array.static_variables, (copy_ctor_func_t) zval_add_ref, (void *) &tmp_copy, sizeof(zval *));
	}
}
/* }}} */

/* {{{ proto mixed Reflection_Function::invoke(mixed* args)
   Invokes the function */
ZEND_FUNCTION(reflection_function_invoke)
{
	zval *retval_ptr;
	zval ***params;
	reflection_object *intern;
	zend_function *fptr;
	int argc = ZEND_NUM_ARGS();
	
	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(fptr);

	params = safe_emalloc(sizeof(zval **), argc, 0);
	if (zend_get_parameters_array_ex(argc, params) == FAILURE) {
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
/* }}} */

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

/* {{{ proto Reflection_Class Reflection_Class::__construct(string name)
   Constructor. Throws an Exception in case the given class does not exist */
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
		_DO_THROW("Class does not exist");
	}
	efree(lcname);
	intern->ptr = *ce;
}
/* }}} */

/* {{{ proto string Reflection_Class::getName()
   Returns the class' name */
ZEND_FUNCTION(reflection_class_getname)
{
	METHOD_NOTSTATIC_NUMPARAMS(0);
	_default_get_entry(getThis(), "name", sizeof("name"), return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto bool Reflection_Class::isInternal()
   Returns whether this class is an internal class */
ZEND_FUNCTION(reflection_class_isinternal)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	RETURN_BOOL(ce->type == ZEND_INTERNAL_CLASS);
}
/* }}} */

/* {{{ proto bool Reflection_Class::isUserDefined()
   Returns whether this class is user-defined */
ZEND_FUNCTION(reflection_class_isuserdefined)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	RETURN_BOOL(ce->type == ZEND_USER_CLASS);
}
/* }}} */

/* {{{ proto string Reflection_Class::getFileName()
   Returns the filename of the file this class was declared in */
ZEND_FUNCTION(reflection_class_getfilename)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->type == ZEND_USER_CLASS) {
		RETURN_STRING(ce->filename, 1);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int Reflection_Class::getStartLine()
   Returns the line this class' declaration starts at */
ZEND_FUNCTION(reflection_class_getstartline)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->type == ZEND_USER_FUNCTION) {
		RETURN_LONG(ce->line_start);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int Reflection_Class::getEndLine()
   Returns the line this class' declaration ends at */
ZEND_FUNCTION(reflection_class_getendline)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->type == ZEND_USER_CLASS) {
		RETURN_LONG(ce->line_end);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string Reflection_Class::getDocComment()
   Returns the doc comment for this class */
ZEND_FUNCTION(reflection_class_getdoccomment)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->type == ZEND_USER_CLASS && ce->doc_comment) {
		RETURN_STRINGL(ce->doc_comment, ce->doc_comment_len, 1);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Reflection_Class::getConstants()
   Returns an associative array containing this class' constants and their values */
ZEND_FUNCTION(reflection_class_getconstants)
{
	zval *tmp_copy;
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);	
	GET_REFLECTION_OBJECT_PTR(ce);
	array_init(return_value);
	zend_hash_copy(Z_ARRVAL_P(return_value), &ce->constants_table, (copy_ctor_func_t) zval_add_ref, (void *) &tmp_copy, sizeof(zval *));
}
/* }}} */

/* {{{ proto mixed Reflection_Class::getConstant(string name)
   Returns the class' constant specified by its name */
ZEND_FUNCTION(reflection_class_getconstant)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zval **value;
	zval **name; 
	int argc = ZEND_NUM_ARGS();

	METHOD_NOTSTATIC;
	if (zend_get_parameters_ex(argc, &name) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	if (zend_hash_find(&ce->constants_table, Z_STRVAL_PP(name), Z_STRLEN_PP(name) + 1, (void **) &value) == FAILURE) {
		RETURN_FALSE;
	}
	*return_value = **value;
	zval_copy_ctor(return_value);
}
/* }}} */

static void _check_flag(INTERNAL_FUNCTION_PARAMETERS, int mask)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	RETURN_BOOL(ce->ce_flags & mask);
}

/* {{{ proto bool Reflection_Class::isInterface()
   Returns whether this is an interface or a class */
ZEND_FUNCTION(reflection_class_isinterface)
{
	_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_INTERFACE);
}
/* }}} */

/* {{{ proto bool Reflection_Class::isFinal()
   Returns whether this class is final */
ZEND_FUNCTION(reflection_class_isfinal)
{
	_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_FINAL_CLASS);
}
/* }}} */

/* {{{ proto bool Reflection_Class::isAbstract()
   Returns whether this class is abstract */
ZEND_FUNCTION(reflection_class_isabstract)
{
	_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_ABSTRACT_CLASS);
}
/* }}} */

/* {{{ proto bool Reflection_Class::isInstance(stdclass object)
   Returns whether the given object is an instance of this class */
ZEND_FUNCTION(reflection_class_isinstance)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zval *object;

	METHOD_NOTSTATIC;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &object) == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);	
	RETURN_BOOL(ce == Z_OBJCE_P(object));
}
/* }}} */


/* {{{ proto Reflection_Class[] Reflection_Class::getInterfaces()
   Returns an array of interfaces this class implements */
ZEND_FUNCTION(reflection_class_getinterfaces)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);

	/* Return an empty array if this class implements no interfaces */
	array_init(return_value);

	if (ce->num_interfaces) {
		int i;

	   	for(i=0; i < ce->num_interfaces; i++) {
			zval *interface;
			ALLOC_ZVAL(interface);
			reflection_class_factory(ce->interfaces[i], interface TSRMLS_CC);
			add_next_index_zval(return_value, interface);
		}
	}
}
/* }}} */

/* {{{ proto Reflection_Class Reflection_Class::getParentClass()
   Returns the class' parent class, or, if none exists, FALSE */
ZEND_FUNCTION(reflection_class_getparentclass)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	
	if (ce->parent) {
		reflection_class_factory(ce->parent, return_value TSRMLS_CC);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

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
	ZEND_NAMED_FE(isabstract, ZEND_FN(reflection_class_isabstract), NULL)
	ZEND_NAMED_FE(isfinal, ZEND_FN(reflection_class_isfinal), NULL)
	ZEND_NAMED_FE(isinstance, ZEND_FN(reflection_class_isinstance), NULL)
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
