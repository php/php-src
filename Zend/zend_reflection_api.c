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
   | Authors: Timm Friebe <thekid@thekid.de>                              |
   |          George Schlossnagle <george@omniti.com>                     |
   |          Andrei Zmievski <andrei@gravitonic.com>                     |
   +----------------------------------------------------------------------+
*/

/* $Id$ */
#include "zend.h"
#include "zend_API.h"
#include "zend_default_classes.h"
#include "zend_operators.h"
#include "zend_constants.h"
#include "zend_ini.h"

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
	{ \
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
	EG(exception) = ex; \
	return; \
	}

typedef struct _property_reference {
	zend_class_entry *ce;
	zend_property_info *prop;
} property_reference;

zend_class_entry *reflection_function_ptr;
zend_class_entry *reflection_class_ptr;
zend_class_entry *reflection_method_ptr;
zend_class_entry *reflection_property_ptr;
zend_class_entry *reflection_extension_ptr;

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
	unsigned int free_ptr:1;
} reflection_object;

static void reflection_objects_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	reflection_object *intern = (reflection_object *) object;

	if (intern->free_ptr) {
		efree(intern->ptr);
	}
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
	(*intern_clone)->free_ptr = intern->free_ptr;
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
	retval.handlers = &reflection_object_handlers;
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
		efree(lcname);
		_DO_THROW("Function does not exist");
	}
	efree(lcname);
	intern->ptr = fptr;
	intern->free_ptr = 0;
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

	MAKE_STD_ZVAL(name);
	ZVAL_STRINGL(name, ce->name, ce->name_length, 1);
	reflection_instanciate(reflection_class_ptr, object TSRMLS_CC);
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	intern->ptr = ce;
	intern->free_ptr = 0;
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &name, sizeof(zval *), NULL);
}

void reflection_function_factory(zend_function *function, zval *object TSRMLS_DC)
{
	reflection_object *intern;
	zval *name;

	MAKE_STD_ZVAL(name);
	ZVAL_STRING(name, function->common.function_name, 1);

	reflection_instanciate(reflection_function_ptr, object TSRMLS_CC);
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	intern->ptr = function;
	intern->free_ptr = 0;
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &name, sizeof(zval *), NULL);
}

void reflection_method_factory(zend_class_entry *ce, zend_function *method, zval *object TSRMLS_DC)
{
	reflection_object *intern;
	zval *name;
	zval *classname;

	MAKE_STD_ZVAL(name);
	ZVAL_STRING(name, method->common.function_name, 1);
	MAKE_STD_ZVAL(classname);
	ZVAL_STRINGL(classname, ce->name, ce->name_length, 1);
	reflection_instanciate(reflection_method_ptr, object TSRMLS_CC);
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	intern->ptr = method;
	intern->free_ptr = 0;
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &name, sizeof(zval *), NULL);
	zend_hash_update(Z_OBJPROP_P(object), "class", sizeof("class"), (void **) &classname, sizeof(zval *), NULL);
}

void reflection_property_factory(zend_class_entry *ce, zend_property_info *prop, zval *object TSRMLS_DC)
{
	reflection_object *intern;
	zval *name;
	zval *classname;
	property_reference *reference;

	MAKE_STD_ZVAL(name);

	/* Unmangle the property name if necessary */
	if (prop->name[0] != 0) {
		ZVAL_STRINGL(name, prop->name, prop->name_length, 1);
	} else {
		char* tmp;

		tmp= prop->name+ 1;
		ZVAL_STRING(name, tmp + strlen(tmp) + 1, 1);
	}
	MAKE_STD_ZVAL(classname);
	ZVAL_STRINGL(classname, ce->name, ce->name_length, 1);
	reflection_instanciate(reflection_property_ptr, object TSRMLS_CC);
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	reference = (property_reference*) emalloc(sizeof(property_reference));
	reference->ce = ce;
	reference->prop = prop;
	intern->ptr = reference;
	intern->free_ptr = 1;
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &name, sizeof(zval *), NULL);
	zend_hash_update(Z_OBJPROP_P(object), "class", sizeof("class"), (void **) &classname, sizeof(zval *), NULL);
}

/* {{{ proto Reflection_Class Reflection_Method::__construct(mixed class, string name)
   Constructor. Throws an Exception in case the given method does not exist */
ZEND_FUNCTION(reflection_method)
{
	zval **name, **class;
	zval *classname;
	zval *object;
	reflection_object *intern;
	char *lcname;
	zend_class_entry **pce;
	zend_class_entry *ce;
	zend_function *mptr;

	int	argc = ZEND_NUM_ARGS();
	if (zend_get_parameters_ex(argc, &class, &name) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL) {
		return;
	}
	
	/* Find the class entry */
	switch (Z_TYPE_PP(class)) {
		case IS_STRING:
			convert_to_string_ex(class);
			lcname = zend_str_tolower_dup((const char *)Z_STRVAL_PP(class), (int) Z_STRLEN_PP(class));
			if (zend_hash_find(EG(class_table), lcname, (int)(Z_STRLEN_PP(class) + 1), (void **) &pce) == FAILURE) {
				efree(lcname);
				_DO_THROW("Class does not exist");
				/* returns out of this function */
			}
			ce = *pce;
			efree(lcname);
			break;

		case IS_OBJECT:
			ce = Z_OBJCE_PP(class);
			break;
			
		default:
			_DO_THROW("The parameter class is expected to be either a string or an object");
			/* returns out of this function */
	}

	MAKE_STD_ZVAL(classname);
	ZVAL_STRINGL(classname, ce->name, ce->name_length, 1);
	zend_hash_update(Z_OBJPROP_P(object), "class", sizeof("class"), (void **) &classname, sizeof(zval *), NULL);
	
	convert_to_string_ex(name);
	zval_add_ref(name);
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) name, sizeof(zval *), NULL);
	lcname = zend_str_tolower_dup((const char *)Z_STRVAL_PP(name), (int) Z_STRLEN_PP(name));

	if (zend_hash_find(&ce->function_table, lcname, (int)(Z_STRLEN_PP(name) + 1), (void **) &mptr) == FAILURE) {
		efree(lcname);
		_DO_THROW("Method does not exist");
		/* returns out of this function */
	}
	efree(lcname);
	intern->ptr = mptr;
	intern->free_ptr = 0;
}
/* }}} */

/* {{{ proto mixed Reflection_Method::invoke(stdclass object, mixed* args)
   Invokes the function */
ZEND_FUNCTION(reflection_method_invoke)
{
	zval *retval_ptr;
	zval ***params;
	reflection_object *intern;
	zend_function *mptr;
	int argc = ZEND_NUM_ARGS();
	
	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(mptr);

	if ((mptr->common.fn_flags & ~ZEND_ACC_PUBLIC) ||
		(mptr->common.fn_flags & ZEND_ACC_ABSTRACT)) {
		_DO_THROW("Trying to invoke a non-public or abstract method");
		/* Returns from this function */
	}

	params = safe_emalloc(sizeof(zval **), argc, 0);
	if (zend_get_parameters_array_ex(argc, params) == FAILURE) {
		efree(params);
		RETURN_FALSE;
	}
	
	if (argc < 1 || Z_TYPE_PP(params[0]) != IS_OBJECT) {
		zend_error(E_WARNING, "First parameter is expected to be an instance of %s", mptr->common.scope->name);
		efree(params);
		RETURN_FALSE;
	}

	if (fast_call_user_function(EG(function_table), params[0], NULL,
							   &retval_ptr, argc - 1, params+ 1, 
							   1, NULL, &mptr TSRMLS_CC) == SUCCESS && retval_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
	}
	efree(params);
}
/* }}} */

static void _method_check_flag(INTERNAL_FUNCTION_PARAMETERS, int mask)
{
	reflection_object *intern;
	zend_function *mptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(mptr);
	RETURN_BOOL(mptr->common.fn_flags & mask);
}

/* {{{ proto bool Reflection_Method::isFinal()
   Returns whether this method is final */
ZEND_FUNCTION(reflection_method_isfinal)
{
	_method_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_FINAL);
}
/* }}} */

/* {{{ proto bool Reflection_Method::isAbstract()
   Returns whether this method is abstract */
ZEND_FUNCTION(reflection_method_isabstract)
{
	_method_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_ABSTRACT);
}
/* }}} */

/* {{{ proto bool Reflection_Method::isPublic()
   Returns whether this method is public */
ZEND_FUNCTION(reflection_method_ispublic)
{
	_method_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PUBLIC);
}
/* }}} */

/* {{{ proto bool Reflection_Method::isPrivate()
   Returns whether this method is private */
ZEND_FUNCTION(reflection_method_isprivate)
{
	_method_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PRIVATE);
}
/* }}} */

/* {{{ proto bool Reflection_Method::isProtected()
   Returns whether this method is protected */
ZEND_FUNCTION(reflection_method_isprotected)
{
	_method_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PROTECTED);
}
/* }}} */

/* {{{ proto bool Reflection_Method::isStatic()
   Returns whether this method is static */
ZEND_FUNCTION(reflection_method_isstatic)
{
	_method_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_STATIC);
}
/* }}} */

/* {{{ proto bool Reflection_Method::isConstructor()
   Returns whether this method is the constructor */
ZEND_FUNCTION(reflection_method_isconstructor)
{
	reflection_object *intern;
	zend_function *mptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(mptr);
	RETURN_BOOL(mptr->common.scope->constructor == mptr);
}
/* }}} */

/* {{{ proto bool Reflection_Method::getModifiers()
   Returns the access modifiers for this method */
ZEND_FUNCTION(reflection_method_getmodifiers)
{
	reflection_object *intern;
	zend_function *mptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(mptr);

	array_init(return_value);

	switch (mptr->common.fn_flags & ZEND_ACC_PPP_MASK) {
		case ZEND_ACC_PUBLIC:
			add_assoc_bool_ex(return_value, "public", sizeof("public"), 1);
			break;
		case ZEND_ACC_PRIVATE:
			add_assoc_bool_ex(return_value, "private", sizeof("private"), 1);
			break;
		case ZEND_ACC_PROTECTED:
			add_assoc_bool_ex(return_value, "protected", sizeof("protected"), 1);
			break;
	}
	if (mptr->common.fn_flags & ZEND_ACC_ABSTRACT) {
		add_assoc_bool_ex(return_value, "abstract", sizeof("abstract"), 1);
	}
	if (mptr->common.fn_flags & ZEND_ACC_STATIC) {
		add_assoc_bool_ex(return_value, "static", sizeof("static"), 1);
	}
	if (mptr->common.fn_flags & ZEND_ACC_FINAL) {
		add_assoc_bool_ex(return_value, "final", sizeof("final"), 1);
	}
}
/* }}} */

/* {{{ proto void Reflection_Method::getDeclaringClass()
   Get the declaring class */
ZEND_FUNCTION(reflection_method_getdeclaringclass)
{
	reflection_object *intern;
	zend_function *mptr;

	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(mptr);

	reflection_class_factory(mptr->common.scope, return_value TSRMLS_CC);
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
		efree(lcname);
		_DO_THROW("Class does not exist");
	}
	efree(lcname);
	intern->ptr = *ce;
	intern->free_ptr = 0;
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

/* {{{ proto PHP_Method Reflection_Class::getConstructor()
   Returns the class' constructor if there is one, NULL otherwise */
ZEND_FUNCTION(reflection_class_getconstructor)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);	
	GET_REFLECTION_OBJECT_PTR(ce);

	if (ce->constructor) {
		reflection_method_factory(ce, ce->constructor, return_value TSRMLS_CC);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto PHP_Method Reflection_Class::getMethod(string name)
   Returns the class' method specified by it's name */
ZEND_FUNCTION(reflection_class_getmethod)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_function *mptr;
	zval **name; 
	int argc = ZEND_NUM_ARGS();

	METHOD_NOTSTATIC;
	if (zend_get_parameters_ex(argc, &name) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	if (zend_hash_find(&ce->function_table, Z_STRVAL_PP(name), Z_STRLEN_PP(name) + 1, (void**) &mptr) == SUCCESS) {
		reflection_method_factory(ce, mptr, return_value TSRMLS_CC);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

static int _addmethod(zend_function *mptr, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval *method;
	zend_class_entry *ce = *va_arg(args, zend_class_entry**);
	zval *retval = va_arg(args, zval*);
	long filter = va_arg(args, long);

	if (mptr->common.fn_flags & filter) {
	    TSRMLS_FETCH();
		ALLOC_ZVAL(method);
		reflection_method_factory(ce, mptr, method TSRMLS_CC);
		add_next_index_zval(retval, method);
	}
	return 0;
}

/* {{{ proto Reflection_Method[] Reflection_Class::getMethods()
   Returns an array of this class' methods */
ZEND_FUNCTION(reflection_class_getmethods)
{
	reflection_object *intern;
	zend_class_entry *ce;
	long filter = 0;
	int argc = ZEND_NUM_ARGS();

	METHOD_NOTSTATIC;
	if (argc) {
		if (zend_parse_parameters(argc TSRMLS_CC, "|l", &filter) == FAILURE) {
			return;
		}
	} else {
		/* No parameters given, default to "return all" */
		filter = ZEND_ACC_PPP_MASK | ZEND_ACC_ABSTRACT | ZEND_ACC_FINAL | ZEND_ACC_STATIC;
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	array_init(return_value);
	zend_hash_apply_with_arguments(&ce->function_table, (apply_func_args_t) _addmethod, 3, &ce, return_value, filter);
}
/* }}} */

/* {{{ proto Reflection_Property Reflection_Class::getProperty(string name)
   Returns the class' property specified by it's name */
ZEND_FUNCTION(reflection_class_getproperty)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_property_info *property_info;
	zval **name; 
	int argc = ZEND_NUM_ARGS();

	METHOD_NOTSTATIC;
	if (zend_get_parameters_ex(argc, &name) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	if (zend_hash_find(&ce->properties_info, Z_STRVAL_PP(name), Z_STRLEN_PP(name) + 1, (void**) &property_info) == SUCCESS) {
		reflection_property_factory(ce, property_info, return_value TSRMLS_CC);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

static int _addproperty(zend_property_info *pptr, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval *property;
	zend_class_entry *ce = *va_arg(args, zend_class_entry**);
	zval *retval = va_arg(args, zval*);
	long filter = va_arg(args, long);

	if (pptr->flags	& filter) {
	    TSRMLS_FETCH();
		ALLOC_ZVAL(property);
		reflection_property_factory(ce, pptr, property TSRMLS_CC);
		add_next_index_zval(retval, property);
	}
	return 0;
}

/* {{{ proto Reflection_Property[] Reflection_Class::getProperties()
   Returns an array of this class' properties */
ZEND_FUNCTION(reflection_class_getproperties)
{
	reflection_object *intern;
	zend_class_entry *ce;
	long filter = 0;
	int argc = ZEND_NUM_ARGS();

	METHOD_NOTSTATIC;
	if (argc) {
		if (zend_parse_parameters(argc TSRMLS_CC, "|l", &filter) == FAILURE) {
			return;
		}
	} else {
		/* No parameters given, default to "return all" */
		filter = ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC;
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	array_init(return_value);
	zend_hash_apply_with_arguments(&ce->properties_info, (apply_func_args_t) _addproperty, 3, &ce, return_value, filter);
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

static void _class_check_flag(INTERNAL_FUNCTION_PARAMETERS, int mask)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	RETVAL_BOOL(ce->ce_flags & mask);
}

/* {{{ proto bool Reflection_Class::isInstantiable()
   Returns whether this class is instantiable */
ZEND_FUNCTION(reflection_class_isinstantiable)
{
	_class_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, (ZEND_ACC_INTERFACE | ZEND_ACC_ABSTRACT));
	Z_BVAL_P(return_value) = !Z_BVAL_P(return_value);   /* Ugly? */
}
/* }}} */


/* {{{ proto bool Reflection_Class::isInterface()
   Returns whether this is an interface or a class */
ZEND_FUNCTION(reflection_class_isinterface)
{
	_class_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_INTERFACE);
}
/* }}} */

/* {{{ proto bool Reflection_Class::isFinal()
   Returns whether this class is final */
ZEND_FUNCTION(reflection_class_isfinal)
{
	_class_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_FINAL_CLASS);
}
/* }}} */

/* {{{ proto bool Reflection_Class::isAbstract()
   Returns whether this class is abstract */
ZEND_FUNCTION(reflection_class_isabstract)
{
	_class_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_ABSTRACT_CLASS);
}
/* }}} */

/* {{{ proto bool Reflection_Class::getModifiers()
   Returns the access modifiers for this class */
ZEND_FUNCTION(reflection_class_getmodifiers)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);

	array_init(return_value);

	if (ce->ce_flags & ZEND_ACC_ABSTRACT_CLASS) {
		add_assoc_bool_ex(return_value, "abstract", sizeof("abstract"), 1);
	}
	if (ce->ce_flags & ZEND_ACC_FINAL_CLASS) {
		add_assoc_bool_ex(return_value, "final", sizeof("final"), 1);
	}
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

/* {{{ proto stdclass Reflection_Class::newInstance(mixed* args)
   Returns an instance of this class */
ZEND_FUNCTION(reflection_class_newinstance)
{
	zval *retval_ptr;
	reflection_object *intern;
	zend_class_entry *ce;
	int argc = ZEND_NUM_ARGS();
	
	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(ce);

	object_init_ex(return_value, ce);

	/* Run the constructor if there is one */
	if (ce->constructor) {
		zval ***params;

		params = safe_emalloc(sizeof(zval **), argc, 0);
		if (zend_get_parameters_array_ex(argc, params) == FAILURE) {
			efree(params);
			RETURN_FALSE;
		}

		if (fast_call_user_function(EG(function_table), &return_value, NULL,
								   &retval_ptr, argc, params, 
								   1, NULL, &ce->constructor TSRMLS_CC) == FAILURE) {
			efree(params);
			zend_error(E_WARNING, "Invokation of %s's constructor failed\n", ce->name);
			RETURN_NULL();
		}
		if (retval_ptr) {
			zval_ptr_dtor(&retval_ptr);
		}
		efree(params);
	}
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

/* {{{ proto Reflection_Property Reflection_Property::__construct(mixed class, string name)
   Constructor. Throws an Exception in case the given property does not exist */
ZEND_FUNCTION(reflection_property)
{
	zval **name, **class;
	zval *classname;
	zval *object;
	reflection_object *intern;
	char *lcname;
	zend_class_entry **pce;
	zend_class_entry *ce;
	zend_property_info *property_info;
	property_reference *reference;

	int	argc = ZEND_NUM_ARGS();
	if (zend_get_parameters_ex(argc, &class, &name) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL) {
		return;
	}
	
	/* Find the class entry */
	switch (Z_TYPE_PP(class)) {
		case IS_STRING:
			convert_to_string_ex(class);
			lcname = zend_str_tolower_dup((const char *)Z_STRVAL_PP(class), (int) Z_STRLEN_PP(class));
			if (zend_hash_find(EG(class_table), lcname, (int)(Z_STRLEN_PP(class) + 1), (void **) &pce) == FAILURE) {
				efree(lcname);
				_DO_THROW("Class does not exist");
				/* returns out of this function */
			}
			ce = *pce;
			efree(lcname);
			break;

		case IS_OBJECT:
			ce = Z_OBJCE_PP(class);
			break;
			
		default:
			_DO_THROW("The parameter class is expected to be either a string or an object");
			/* returns out of this function */
	}

	MAKE_STD_ZVAL(classname);
	ZVAL_STRINGL(classname, ce->name, ce->name_length, 1);
	zend_hash_update(Z_OBJPROP_P(object), "class", sizeof("class"), (void **) &classname, sizeof(zval *), NULL);
	
	convert_to_string_ex(name);
	zval_add_ref(name);
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) name, sizeof(zval *), NULL);
	lcname = zend_str_tolower_dup((const char *)Z_STRVAL_PP(name), (int) Z_STRLEN_PP(name));

	if (zend_hash_find(&ce->properties_info, lcname, (int)(Z_STRLEN_PP(name) + 1), (void **) &property_info) == FAILURE) {
		efree(lcname);
		_DO_THROW("Property does not exist");
		/* returns out of this function */
	}
	efree(lcname);

	reference = (property_reference*) emalloc(sizeof(property_reference));
	reference->ce = ce;
	reference->prop = property_info;
	intern->ptr = reference;
	intern->free_ptr = 1;
}
/* }}} */

/* {{{ proto string Reflection_Property::getName()
   Returns the class' name */
ZEND_FUNCTION(reflection_property_getname)
{
	METHOD_NOTSTATIC_NUMPARAMS(0);
	_default_get_entry(getThis(), "name", sizeof("name"), return_value TSRMLS_CC);
}
/* }}} */

static void _property_check_flag(INTERNAL_FUNCTION_PARAMETERS, int mask)
{
	reflection_object *intern;
	property_reference *ref;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ref);
	RETURN_BOOL(ref->prop->flags & mask);
}

/* {{{ proto bool Reflection_Property::isPublic()
   Returns whether this property is public */
ZEND_FUNCTION(reflection_property_ispublic)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PUBLIC);
}
/* }}} */

/* {{{ proto bool Reflection_Property::isPrivate()
   Returns whether this property is private */
ZEND_FUNCTION(reflection_property_isprivate)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PRIVATE);
}
/* }}} */

/* {{{ proto bool Reflection_Property::isProtected()
   Returns whether this property is protected */
ZEND_FUNCTION(reflection_property_isprotected)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PROTECTED);
}
/* }}} */

/* {{{ proto bool Reflection_Property::isStatic()
   Returns whether this property is static */
ZEND_FUNCTION(reflection_property_isstatic)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_STATIC);
}
/* }}} */

/* {{{ proto bool Reflection_Property::isDefault()
   Returns whether this property is default (declared at compilation time). */
ZEND_FUNCTION(reflection_property_isdefault)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ~ZEND_ACC_IMPLICIT_PUBLIC);
}
/* }}} */

/* {{{ proto bool Reflection_Property::getModifiers()
   Returns the access modifiers for this property */
ZEND_FUNCTION(reflection_property_getmodifiers)
{
	reflection_object *intern;
	property_reference *ref;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ref);

	array_init(return_value);

	switch (ref->prop->flags & ZEND_ACC_PPP_MASK) {
		case ZEND_ACC_PUBLIC:
			add_assoc_bool_ex(return_value, "public", sizeof("public"), 1);
			break;
		case ZEND_ACC_PRIVATE:
			add_assoc_bool_ex(return_value, "private", sizeof("private"), 1);
			break;
		case ZEND_ACC_PROTECTED:
			add_assoc_bool_ex(return_value, "protected", sizeof("protected"), 1);
			break;
	}
	if (ref->prop->flags & ZEND_ACC_STATIC) {
		add_assoc_bool_ex(return_value, "static", sizeof("static"), 1);
	}
}
/* }}} */

/* {{{ proto mixed Reflection_Property::getValue(stdclass object)
   Returns this property's value */
ZEND_FUNCTION(reflection_property_getvalue)
{
	reflection_object *intern;
	property_reference *ref;
	zval *object;
	zval **member= NULL;

	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(ref);

	if (ref->prop->flags & ~ZEND_ACC_PUBLIC) {
		_DO_THROW("Cannot access non-public member");
		/* Returns from this function */
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &object) == FAILURE) {
		return;
	}

	if (zend_hash_quick_find(Z_OBJPROP_P(object), ref->prop->name, ref->prop->name_length + 1, ref->prop->h, (void **) &member) == FAILURE) {
		zend_error(E_ERROR, "Internal error: Could not find the property %s", ref->prop->name);
		/* Bails out */
	}

	*return_value= **member;
	zval_copy_ctor(return_value);
}
/* }}} */

/* {{{ proto void Reflection_Property::setValue(stdclass object, mixed value)
   Sets this property's value */
ZEND_FUNCTION(reflection_property_setvalue)
{
	reflection_object *intern;
	property_reference *ref;
	zval **variable_ptr;
	zval *object;
	zval *value;
	int setter_done = 0;

	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(ref);

	if (ref->prop->flags & ~ZEND_ACC_PUBLIC) {
		_DO_THROW("Cannot access non-public member");
		/* Returns from this function */
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "oz", &object, &value) == FAILURE) {
		return;
	}

	if (zend_hash_quick_find(Z_OBJPROP_P(object), ref->prop->name, ref->prop->name_length + 1, ref->prop->h, (void **) &variable_ptr) == SUCCESS) {
		if (*variable_ptr == value) {
			setter_done = 1;
		} else {
			if (PZVAL_IS_REF(*variable_ptr)) {
				zval_dtor(*variable_ptr);
				(*variable_ptr)->type = value->type;
				(*variable_ptr)->value = value->value;
				if (value->refcount > 0) {
					zval_copy_ctor(*variable_ptr);
				}
				setter_done = 1;
			}
		}
	} else {
		zend_error(E_ERROR, "Internal error: Could not find the property %s", ref->prop->name);
		/* Bails out */
	}

	if (!setter_done) {
		zval **foo;

		value->refcount++;
		if (PZVAL_IS_REF(value)) {
			SEPARATE_ZVAL(&value);
		}
		zend_hash_quick_update(Z_OBJPROP_P(object), ref->prop->name, ref->prop->name_length+1, ref->prop->h, &value, sizeof(zval *), (void **) &foo);
	}
}
/* }}} */

/* {{{ proto void Reflection_Property::getDeclaringClass()
   Get the declaring class */
ZEND_FUNCTION(reflection_property_getdeclaringclass)
{
	reflection_object *intern;
	property_reference *ref;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ref);

	reflection_class_factory(ref->ce, return_value TSRMLS_CC);
}

/* {{{ proto Reflection_Extension Reflection_Extension::__construct(string name)
   Constructor. Throws an Exception in case the given extension does not exist */
ZEND_FUNCTION(reflection_extension)
{
	zval **name;
	zval  *object;
	char *lcname;
	reflection_object *intern;
	zend_module_entry *module;

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
	if (zend_hash_find(&module_registry, lcname, (int)(Z_STRLEN_PP(name) + 1), (void **)&module) == FAILURE) {
		efree(lcname);
		_DO_THROW("Extension does not exist");
		/* returns from this function */
	}
	efree(lcname);
	intern->ptr = module;
	intern->free_ptr = 0;
}
/* }}} */

/* {{{ proto string Reflection_Extension::getName()
   Returns this extension's name */
ZEND_FUNCTION(reflection_extension_getname)
{
	METHOD_NOTSTATIC_NUMPARAMS(0);
	_default_get_entry(getThis(), "name", sizeof("name"), return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto string Reflection_Extension::getVersion()
   Returns this extension's version */
ZEND_FUNCTION(reflection_extension_getversion)
{
	reflection_object *intern;
	zend_module_entry *module;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(module);

	/* An extension does not necessarily have a version number */
	if (module->version == NO_VERSION_YET) {
		RETURN_NULL();
	} else {
		RETURN_STRING(module->version, 1);
	}
}
/* }}} */

/* {{{ proto Reflection_Function[] Reflection_Extension::getMethods()
   Returns an array of this extension's fuctions */
ZEND_FUNCTION(reflection_extension_getfunctions)
{
	reflection_object *intern;
	zend_module_entry *module;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	if (module->functions) {
		zval *function;
		zend_function *fptr;
		zend_function_entry *func = module->functions;

		/* Is there a better way of doing this? */
		while (func->fname) {
			if (zend_hash_find(EG(function_table), func->fname, strlen(func->fname) + 1, (void**) &fptr) == FAILURE) {
				zend_error(E_WARNING, "Internal error: Cannot find extension function %s in global function table", func->fname);
				continue;
			}
			
			ALLOC_ZVAL(function);
			reflection_function_factory(fptr, function TSRMLS_CC);
			add_next_index_zval(return_value, function);
			func++;
		}
	}
}
/* }}} */

static int _addconstant(zend_constant *constant, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval *const_val;
	zval *retval = va_arg(args, zval*);
	int number = va_arg(args, int);

	if (number == constant->module_number) {
		MAKE_STD_ZVAL(const_val);
		*const_val = constant->value;
		zval_copy_ctor(const_val);
		INIT_PZVAL(const_val);
		add_assoc_zval_ex(retval, constant->name, constant->name_len, const_val);
	}
	return 0;
}

/* {{{ proto array Reflection_Extension::getConstants()
   Returns an associative array containing this extension's constants and their values */
ZEND_FUNCTION(reflection_extension_getconstants)
{
	reflection_object *intern;
	zend_module_entry *module;

	METHOD_NOTSTATIC_NUMPARAMS(0);	
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(zend_constants), (apply_func_args_t) _addconstant, 2, return_value, module->module_number);
}
/* }}} */

static int _addinientry(zend_ini_entry *ini_entry, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval *retval = va_arg(args, zval*);
	int number = va_arg(args, int);

	if (number == ini_entry->module_number) {
		if (ini_entry->value) {
			add_assoc_stringl(retval, ini_entry->name, ini_entry->value, ini_entry->value_length, 1);
		} else {
			add_assoc_null(retval, ini_entry->name);
		}
	}
	return 0;
}

/* {{{ proto array Reflection_Extension::getINIEntries()
   Returns an associative array containing this extension's INI entries and their values */
ZEND_FUNCTION(reflection_extension_getinientries)
{
	reflection_object *intern;
	zend_module_entry *module;

	METHOD_NOTSTATIC_NUMPARAMS(0);	
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(ini_directives), (apply_func_args_t) _addinientry, 2, return_value, module->module_number);
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

static zend_function_entry reflection_method_functions[] = {
	ZEND_FE(reflection_method, NULL)
	ZEND_NAMED_FE(ispublic, ZEND_FN(reflection_method_ispublic), NULL)
	ZEND_NAMED_FE(isprivate, ZEND_FN(reflection_method_isprivate), NULL)
	ZEND_NAMED_FE(isprotected, ZEND_FN(reflection_method_isprotected), NULL)
	ZEND_NAMED_FE(isabstract, ZEND_FN(reflection_method_isabstract), NULL)
	ZEND_NAMED_FE(isfinal, ZEND_FN(reflection_method_isfinal), NULL)
	ZEND_NAMED_FE(isstatic, ZEND_FN(reflection_method_isstatic), NULL)
	ZEND_NAMED_FE(getmodifiers, ZEND_FN(reflection_method_getmodifiers), NULL)
	ZEND_NAMED_FE(isconstructor, ZEND_FN(reflection_method_isconstructor), NULL)
	ZEND_NAMED_FE(invoke, ZEND_FN(reflection_method_invoke), NULL)
	ZEND_NAMED_FE(getdeclaringclass, ZEND_FN(reflection_method_getdeclaringclass), NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry reflection_class_functions[] = {
	ZEND_FE(reflection_class, NULL)
	ZEND_NAMED_FE(getname, ZEND_FN(reflection_class_getname), NULL)
	ZEND_NAMED_FE(isinternal, ZEND_FN(reflection_class_isinternal), NULL)
	ZEND_NAMED_FE(isuserdefined, ZEND_FN(reflection_class_isuserdefined), NULL)
	ZEND_NAMED_FE(isinstantiable, ZEND_FN(reflection_class_isinstantiable), NULL)
	ZEND_NAMED_FE(getfilename, ZEND_FN(reflection_class_getfilename), NULL)
	ZEND_NAMED_FE(getstartline, ZEND_FN(reflection_class_getstartline), NULL)
	ZEND_NAMED_FE(getendline, ZEND_FN(reflection_class_getendline), NULL)
	ZEND_NAMED_FE(getdoccomment, ZEND_FN(reflection_class_getdoccomment), NULL)
	ZEND_NAMED_FE(getconstructor, ZEND_FN(reflection_class_getconstructor), NULL)
	ZEND_NAMED_FE(getmethod, ZEND_FN(reflection_class_getmethod), NULL)
	ZEND_NAMED_FE(getmethods, ZEND_FN(reflection_class_getmethods), NULL)
	ZEND_NAMED_FE(getproperty, ZEND_FN(reflection_class_getproperty), NULL)
	ZEND_NAMED_FE(getproperties, ZEND_FN(reflection_class_getproperties), NULL)
	ZEND_NAMED_FE(getconstants, ZEND_FN(reflection_class_getconstants), NULL)
	ZEND_NAMED_FE(getconstant, ZEND_FN(reflection_class_getconstant), NULL)
	ZEND_NAMED_FE(getinterfaces, ZEND_FN(reflection_class_getinterfaces), NULL)
	ZEND_NAMED_FE(isinterface, ZEND_FN(reflection_class_isinterface), NULL)
	ZEND_NAMED_FE(isabstract, ZEND_FN(reflection_class_isabstract), NULL)
	ZEND_NAMED_FE(isfinal, ZEND_FN(reflection_class_isfinal), NULL)
	ZEND_NAMED_FE(getmodifiers, ZEND_FN(reflection_class_getmodifiers), NULL)
	ZEND_NAMED_FE(isinstance, ZEND_FN(reflection_class_isinstance), NULL)
	ZEND_NAMED_FE(newinstance, ZEND_FN(reflection_class_newinstance), NULL)
	ZEND_NAMED_FE(getparentclass, ZEND_FN(reflection_class_getparentclass), NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry reflection_property_functions[] = {
	ZEND_FE(reflection_property, NULL)
	ZEND_NAMED_FE(getname, ZEND_FN(reflection_property_getname), NULL)
	ZEND_NAMED_FE(getvalue, ZEND_FN(reflection_property_getvalue), NULL)
	ZEND_NAMED_FE(setvalue, ZEND_FN(reflection_property_setvalue), NULL)
	ZEND_NAMED_FE(ispublic, ZEND_FN(reflection_property_ispublic), NULL)
	ZEND_NAMED_FE(isprivate, ZEND_FN(reflection_property_isprivate), NULL)
	ZEND_NAMED_FE(isprotected, ZEND_FN(reflection_property_isprotected), NULL)
	ZEND_NAMED_FE(isstatic, ZEND_FN(reflection_property_isstatic), NULL)
	ZEND_NAMED_FE(isdefault, ZEND_FN(reflection_property_isdefault), NULL)
	ZEND_NAMED_FE(getmodifiers, ZEND_FN(reflection_property_getmodifiers), NULL)
	ZEND_NAMED_FE(getdeclaringclass, ZEND_FN(reflection_property_getdeclaringclass), NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry reflection_extension_functions[] = {
	ZEND_FE(reflection_extension, NULL)
	ZEND_NAMED_FE(getname, ZEND_FN(reflection_extension_getname), NULL)
	ZEND_NAMED_FE(getversion, ZEND_FN(reflection_extension_getversion), NULL)
	ZEND_NAMED_FE(getfunctions, ZEND_FN(reflection_extension_getfunctions), NULL)
	ZEND_NAMED_FE(getconstants, ZEND_FN(reflection_extension_getconstants), NULL)
	ZEND_NAMED_FE(getinientries, ZEND_FN(reflection_extension_getinientries), NULL)
	{NULL, NULL, NULL}
};

ZEND_API void zend_register_reflection_api(TSRMLS_D) {
	zend_class_entry _reflection_entry;

	memcpy(&reflection_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	INIT_CLASS_ENTRY(_reflection_entry, "reflection_function", reflection_function_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_function_ptr = zend_register_internal_class(&_reflection_entry TSRMLS_CC);

	INIT_CLASS_ENTRY(_reflection_entry, "reflection_method", reflection_method_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_method_ptr = zend_register_internal_class_ex(&_reflection_entry, reflection_function_ptr, NULL TSRMLS_CC);

	INIT_CLASS_ENTRY(_reflection_entry, "reflection_class", reflection_class_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_class_ptr = zend_register_internal_class(&_reflection_entry TSRMLS_CC);

	INIT_CLASS_ENTRY(_reflection_entry, "reflection_property", reflection_property_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_property_ptr = zend_register_internal_class(&_reflection_entry TSRMLS_CC);

	INIT_CLASS_ENTRY(_reflection_entry, "reflection_extension", reflection_extension_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_extension_ptr = zend_register_internal_class(&_reflection_entry TSRMLS_CC);

	/* Constants for Reflection_Class::getMethods() and Reflection_Class::getProperties() */
	REGISTER_MAIN_LONG_CONSTANT("ACC_STATIC", ZEND_ACC_STATIC, CONST_PERSISTENT|CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("ACC_PUBLIC", ZEND_ACC_PUBLIC, CONST_PERSISTENT|CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("ACC_PROTECTED", ZEND_ACC_PROTECTED, CONST_PERSISTENT|CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("ACC_PRIVATE", ZEND_ACC_PRIVATE, CONST_PERSISTENT|CONST_CS);

	/* Constants for Reflection_Class::getMethods() only */
	REGISTER_MAIN_LONG_CONSTANT("ACC_ABSTRACT", ZEND_ACC_ABSTRACT, CONST_PERSISTENT|CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("ACC_FINAL", ZEND_ACC_FINAL, CONST_PERSISTENT|CONST_CS);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
