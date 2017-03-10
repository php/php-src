/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
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
static zend_class_entry *zend_test_trait;
static zend_object_handlers zend_test_class_handlers;

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(arginfo_zend_test_func, IS_ARRAY, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(arginfo_zend_test_func2, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(zend_test_func)
{
	zval *arg1, *arg2;

	zend_parse_parameters(ZEND_NUM_ARGS(), "|zz", &arg1, &arg2);
}

ZEND_FUNCTION(zend_test_func2)
{
	zval *arg1, *arg2;

	zend_parse_parameters(ZEND_NUM_ARGS(), "|zz", &arg1, &arg2);
}

static zend_object *zend_test_class_new(zend_class_entry *class_type) /* {{{ */ {
	zend_object *obj = zend_objects_new(class_type);
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
/* }}} */

static int zend_test_class_call_method(zend_string *method, zend_object *object, INTERNAL_FUNCTION_PARAMETERS) /* {{{ */ {
	RETVAL_STR(zend_string_copy(method));
	return 0;
}
/* }}} */

static ZEND_METHOD(_ZendTestTrait, testMethod) /* {{{ */ {
	RETURN_TRUE;
}
/* }}} */

static zend_function_entry zend_test_trait_methods[] = {
    ZEND_ME(_ZendTestTrait, testMethod, NULL, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

PHP_MINIT_FUNCTION(test)
{
	zend_class_entry class_entry;

	INIT_CLASS_ENTRY(class_entry, "_ZendTestInterface", NULL);
	zend_test_interface = zend_register_internal_interface(&class_entry);
	zend_declare_class_constant_long(zend_test_interface, ZEND_STRL("DUMMY"), 0);
	INIT_CLASS_ENTRY(class_entry, "_ZendTestClass", NULL);
	zend_test_class = zend_register_internal_class_ex(&class_entry, NULL);
	zend_class_implements(zend_test_class, 1, zend_test_interface);
	zend_test_class->create_object = zend_test_class_new;
	zend_test_class->get_static_method = zend_test_class_static_method_get;

	memcpy(&zend_test_class_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	zend_test_class_handlers.get_method = zend_test_class_method_get;
	zend_test_class_handlers.call_method = zend_test_class_call_method;

	INIT_CLASS_ENTRY(class_entry, "_ZendTestTrait", zend_test_trait_methods);
	zend_test_trait = zend_register_internal_class(&class_entry);
	zend_test_trait->ce_flags |= ZEND_ACC_TRAIT;
	zend_declare_property_null(zend_test_trait, "testProp", sizeof("testProp")-1, ZEND_ACC_PUBLIC);
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(test)
{
	return SUCCESS;
}

PHP_RINIT_FUNCTION(test)
{
#if defined(COMPILE_DL_TEST) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(test)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(test)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "test extension", "enabled");
	php_info_print_table_end();
}

const zend_function_entry test_functions[] = {
	ZEND_FE(zend_test_func,		arginfo_zend_test_func)
	ZEND_FE(zend_test_func2,	arginfo_zend_test_func2)
	ZEND_FE_END
};

zend_module_entry test_module_entry = {
	STANDARD_MODULE_HEADER,
	"test",
	test_functions,
	PHP_MINIT(test),
	PHP_MSHUTDOWN(test),
	PHP_RINIT(test),	
	PHP_RSHUTDOWN(test),
	PHP_MINFO(test),
	PHP_TEST_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_TEST
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(test)
#endif

