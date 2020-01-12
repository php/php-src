/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_test.h"

static zend_class_entry *zend_test_interface;
static zend_class_entry *zend_test_class;
static zend_class_entry *zend_test_child_class;
static zend_class_entry *zend_test_trait;
static zend_object_handlers zend_test_class_handlers;

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(arginfo_zend_test_array_return, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(arginfo_zend_test_nullable_array_return, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(arginfo_zend_test_void_return, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_terminate_string, 0, 0, 1)
	ZEND_ARG_INFO(1, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_leak_variable, 0, 0, 1)
	ZEND_ARG_INFO(0, variable)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(zend_test_func)
{
	/* dummy */
}

ZEND_FUNCTION(zend_test_array_return)
{
	zval *arg1, *arg2;

	zend_parse_parameters(ZEND_NUM_ARGS(), "|zz", &arg1, &arg2);
}

ZEND_FUNCTION(zend_test_nullable_array_return)
{
	zval *arg1, *arg2;

	zend_parse_parameters(ZEND_NUM_ARGS(), "|zz", &arg1, &arg2);
}

ZEND_FUNCTION(zend_test_void_return)
{
	/* dummy */
}

/* Create a string without terminating null byte. Must be termined with
 * zend_terminate_string() before destruction, otherwise a warning is issued
 * in debug builds. */
ZEND_FUNCTION(zend_create_unterminated_string)
{
	zend_string *str, *res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		return;
	}

	res = zend_string_alloc(ZSTR_LEN(str), 0);
	memcpy(ZSTR_VAL(res), ZSTR_VAL(str), ZSTR_LEN(str));
	/* No trailing null byte */

	RETURN_STR(res);
}

/* Enforce terminate null byte on string. This avoids a warning in debug builds. */
ZEND_FUNCTION(zend_terminate_string)
{
	zend_string *str;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		return;
	}

	ZSTR_VAL(str)[ZSTR_LEN(str)] = '\0';
}

/* {{{ proto void zend_leak_bytes([int num_bytes])
   Cause an intentional memory leak, for testing/debugging purposes */
ZEND_FUNCTION(zend_leak_bytes)
{
	zend_long leakbytes = 3;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &leakbytes) == FAILURE) {
		return;
	}

	emalloc(leakbytes);
}
/* }}} */

/* {{{ proto void zend_leak_variable(mixed variable)
   Leak a refcounted variable */
ZEND_FUNCTION(zend_leak_variable)
{
	zval *zv;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zv) == FAILURE) {
		return;
	}

	if (!Z_REFCOUNTED_P(zv)) {
		zend_error(E_WARNING, "Cannot leak variable that is not refcounted");
		return;
	}

	Z_ADDREF_P(zv);
}
/* }}} */

static zend_object *zend_test_class_new(zend_class_entry *class_type) /* {{{ */ {
	zend_object *obj = zend_objects_new(class_type);
	object_properties_init(obj, class_type);
	obj->handlers = &zend_test_class_handlers;
	return obj;
}
/* }}} */

static zend_function *zend_test_class_method_get(zend_object **object, zend_string *name, const zval *key) /* {{{ */ {
	zend_internal_function *fptr = emalloc(sizeof(zend_internal_function));
	fptr->type = ZEND_OVERLOADED_FUNCTION_TEMPORARY;
	fptr->num_args = 1;
	fptr->arg_info = NULL;
	fptr->scope = (*object)->ce;
	fptr->fn_flags = ZEND_ACC_CALL_VIA_HANDLER;
	fptr->function_name = zend_string_copy(name);
	fptr->handler = ZEND_FN(zend_test_func);
	zend_set_function_arg_flags((zend_function*)fptr);

	return (zend_function*)fptr;
}
/* }}} */

static zend_function *zend_test_class_static_method_get(zend_class_entry *ce, zend_string *name) /* {{{ */ {
	if (zend_string_equals_literal_ci(name, "test")) {
		zend_internal_function *fptr = emalloc(sizeof(zend_internal_function));
		fptr->type = ZEND_OVERLOADED_FUNCTION;
		fptr->num_args = 1;
		fptr->arg_info = NULL;
		fptr->scope = ce;
		fptr->fn_flags = ZEND_ACC_CALL_VIA_HANDLER|ZEND_ACC_STATIC;
		fptr->function_name = name;
		fptr->handler = ZEND_FN(zend_test_func);
		zend_set_function_arg_flags((zend_function*)fptr);

		return (zend_function*)fptr;
	}
	return zend_std_get_static_method(ce, name, NULL);
}
/* }}} */

static int zend_test_class_call_method(zend_string *method, zend_object *object, INTERNAL_FUNCTION_PARAMETERS) /* {{{ */ {
	RETVAL_STR(zend_string_copy(method));
	return 0;
}
/* }}} */

/* Internal function returns bool, we return int. */
static ZEND_METHOD(_ZendTestClass, is_object) /* {{{ */ {
	RETURN_LONG(42);
}
/* }}} */

static ZEND_METHOD(_ZendTestTrait, testMethod) /* {{{ */ {
	RETURN_TRUE;
}
/* }}} */

static const zend_function_entry zend_test_class_methods[] = {
	ZEND_ME(_ZendTestClass, is_object, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static const zend_function_entry zend_test_trait_methods[] = {
    ZEND_ME(_ZendTestTrait, testMethod, NULL, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

PHP_MINIT_FUNCTION(zend_test)
{
	zend_class_entry class_entry;

	INIT_CLASS_ENTRY(class_entry, "_ZendTestInterface", NULL);
	zend_test_interface = zend_register_internal_interface(&class_entry);
	zend_declare_class_constant_long(zend_test_interface, ZEND_STRL("DUMMY"), 0);
	INIT_CLASS_ENTRY(class_entry, "_ZendTestClass", zend_test_class_methods);
	zend_test_class = zend_register_internal_class_ex(&class_entry, NULL);
	zend_class_implements(zend_test_class, 1, zend_test_interface);
	zend_test_class->create_object = zend_test_class_new;
	zend_test_class->get_static_method = zend_test_class_static_method_get;

	zend_declare_property_null(zend_test_class, "_StaticProp", sizeof("_StaticProp") - 1, ZEND_ACC_STATIC);

	{
		zend_string *name = zend_string_init("intProp", sizeof("intProp") - 1, 1);
		zval val;
		ZVAL_LONG(&val, 123);
		zend_declare_typed_property(
			zend_test_class, name, &val, ZEND_ACC_PUBLIC, NULL, ZEND_TYPE_ENCODE(IS_LONG, 0));
		zend_string_release(name);
	}

	{
		zend_string *name = zend_string_init("classProp", sizeof("classProp") - 1, 1);
		zend_string *class_name = zend_string_init("stdClass", sizeof("stdClass") - 1, 1);
		zval val;
		ZVAL_NULL(&val);
		zend_declare_typed_property(
			zend_test_class, name, &val, ZEND_ACC_PUBLIC, NULL,
			ZEND_TYPE_ENCODE_CLASS(class_name, 1));
		zend_string_release(name);
	}

	{
		zend_string *name = zend_string_init("staticIntProp", sizeof("staticIntProp") - 1, 1);
		zval val;
		ZVAL_LONG(&val, 123);
		zend_declare_typed_property(
			zend_test_class, name, &val, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC, NULL,
			ZEND_TYPE_ENCODE(IS_LONG, 0));
		zend_string_release(name);
	}

	INIT_CLASS_ENTRY(class_entry, "_ZendTestChildClass", NULL);
	zend_test_child_class = zend_register_internal_class_ex(&class_entry, zend_test_class);

	memcpy(&zend_test_class_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	zend_test_class_handlers.get_method = zend_test_class_method_get;
	zend_test_class_handlers.call_method = zend_test_class_call_method;

	INIT_CLASS_ENTRY(class_entry, "_ZendTestTrait", zend_test_trait_methods);
	zend_test_trait = zend_register_internal_class(&class_entry);
	zend_test_trait->ce_flags |= ZEND_ACC_TRAIT;
	zend_declare_property_null(zend_test_trait, "testProp", sizeof("testProp")-1, ZEND_ACC_PUBLIC);

	zend_register_class_alias("_ZendTestClassAlias", zend_test_class);
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(zend_test)
{
	return SUCCESS;
}

PHP_RINIT_FUNCTION(zend_test)
{
#if defined(COMPILE_DL_ZEND_TEST) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(zend_test)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(zend_test)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "zend-test extension", "enabled");
	php_info_print_table_end();
}

static const zend_function_entry zend_test_functions[] = {
	ZEND_FE(zend_test_array_return, arginfo_zend_test_array_return)
	ZEND_FE(zend_test_nullable_array_return, arginfo_zend_test_nullable_array_return)
	ZEND_FE(zend_test_void_return, arginfo_zend_test_void_return)
	ZEND_FE(zend_create_unterminated_string, NULL)
	ZEND_FE(zend_terminate_string, arginfo_zend_terminate_string)
	ZEND_FE(zend_leak_bytes, NULL)
	ZEND_FE(zend_leak_variable, arginfo_zend_leak_variable)
	ZEND_FE_END
};

zend_module_entry zend_test_module_entry = {
	STANDARD_MODULE_HEADER,
	"zend-test",
	zend_test_functions,
	PHP_MINIT(zend_test),
	PHP_MSHUTDOWN(zend_test),
	PHP_RINIT(zend_test),
	PHP_RSHUTDOWN(zend_test),
	PHP_MINFO(zend_test),
	PHP_ZEND_TEST_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_ZEND_TEST
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(zend_test)
#endif

struct bug79096 bug79096(void)
{
  struct bug79096 b;

  b.a = 1;
  b.b = 1;
  return b;
}
