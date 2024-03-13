/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_test.h"
#include "observer.h"
#include "fiber.h"
#include "iterators.h"
#include "object_handlers.h"
#include "zend_attributes.h"
#include "zend_enum.h"
#include "zend_interfaces.h"
#include "zend_weakrefs.h"
#include "Zend/Optimizer/zend_optimizer.h"
#include "Zend/zend_alloc.h"
#include "test_arginfo.h"
#include "zend_call_stack.h"
#include "zend_exceptions.h"

// `php.h` sets `NDEBUG` when not `PHP_DEBUG` which will make `assert()` from
// assert.h a no-op. In order to have `assert()` working on NDEBUG builds, we
// undefine `NDEBUG` and re-include assert.h
#undef NDEBUG
#include "assert.h"

#if defined(HAVE_LIBXML) && !defined(PHP_WIN32)
# include <libxml/globals.h>
# include <libxml/parser.h>
#endif

ZEND_DECLARE_MODULE_GLOBALS(zend_test)

static zend_class_entry *zend_test_interface;
static zend_class_entry *zend_test_class;
static zend_class_entry *zend_test_child_class;
static zend_class_entry *zend_attribute_test_class;
static zend_class_entry *zend_test_trait;
static zend_class_entry *zend_test_attribute;
static zend_class_entry *zend_test_repeatable_attribute;
static zend_class_entry *zend_test_parameter_attribute;
static zend_class_entry *zend_test_property_attribute;
static zend_class_entry *zend_test_class_with_method_with_parameter_attribute;
static zend_class_entry *zend_test_child_class_with_method_with_parameter_attribute;
static zend_class_entry *zend_test_class_with_property_attribute;
static zend_class_entry *zend_test_forbid_dynamic_call;
static zend_class_entry *zend_test_ns_foo_class;
static zend_class_entry *zend_test_ns_unlikely_compile_error_class;
static zend_class_entry *zend_test_ns_not_unlikely_compile_error_class;
static zend_class_entry *zend_test_ns2_foo_class;
static zend_class_entry *zend_test_ns2_ns_foo_class;
static zend_class_entry *zend_test_unit_enum;
static zend_class_entry *zend_test_string_enum;
static zend_class_entry *zend_test_int_enum;
static zend_class_entry *zend_test_magic_call;
static zend_object_handlers zend_test_class_handlers;

static int le_throwing_resource;

static ZEND_FUNCTION(zend_test_func)
{
	RETVAL_STR_COPY(EX(func)->common.function_name);

	/* Cleanup trampoline */
	ZEND_ASSERT(EX(func)->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE);
	zend_string_release(EX(func)->common.function_name);
	zend_free_trampoline(EX(func));
	EX(func) = NULL;
}

static ZEND_FUNCTION(zend_test_array_return)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

static ZEND_FUNCTION(zend_test_nullable_array_return)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_NULL();
}

static ZEND_FUNCTION(zend_test_void_return)
{
	/* dummy */
	ZEND_PARSE_PARAMETERS_NONE();
}

static void pass1(zend_script *script, void *context)
{
	php_printf("pass1\n");
}

static void pass2(zend_script *script, void *context)
{
	php_printf("pass2\n");
}

static ZEND_FUNCTION(zend_test_deprecated)
{
	zval *arg1;

	zend_parse_parameters(ZEND_NUM_ARGS(), "|z", &arg1);
}

/* Create a string without terminating null byte. Must be terminated with
 * zend_terminate_string() before destruction, otherwise a warning is issued
 * in debug builds. */
static ZEND_FUNCTION(zend_create_unterminated_string)
{
	zend_string *str, *res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		RETURN_THROWS();
	}

	res = zend_string_alloc(ZSTR_LEN(str), 0);
	memcpy(ZSTR_VAL(res), ZSTR_VAL(str), ZSTR_LEN(str));
	/* No trailing null byte */

	RETURN_STR(res);
}

/* Enforce terminate null byte on string. This avoids a warning in debug builds. */
static ZEND_FUNCTION(zend_terminate_string)
{
	zend_string *str;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		RETURN_THROWS();
	}

	ZSTR_VAL(str)[ZSTR_LEN(str)] = '\0';
}

/* Cause an intentional memory leak, for testing/debugging purposes */
static ZEND_FUNCTION(zend_leak_bytes)
{
	zend_long leakbytes = 3;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &leakbytes) == FAILURE) {
		RETURN_THROWS();
	}

	emalloc(leakbytes);
}

/* Leak a refcounted variable */
static ZEND_FUNCTION(zend_leak_variable)
{
	zval *zv;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zv) == FAILURE) {
		RETURN_THROWS();
	}

	if (!Z_REFCOUNTED_P(zv)) {
		zend_error(E_WARNING, "Cannot leak variable that is not refcounted");
		return;
	}

	Z_ADDREF_P(zv);
}

/* Tests Z_PARAM_OBJ_OR_STR */
static ZEND_FUNCTION(zend_string_or_object)
{
	zend_string *str;
	zend_object *object;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OR_STR(object, str)
	ZEND_PARSE_PARAMETERS_END();

	if (str) {
		RETURN_STR_COPY(str);
	} else {
		RETURN_OBJ_COPY(object);
	}
}

/* Tests Z_PARAM_OBJ_OR_STR_OR_NULL */
static ZEND_FUNCTION(zend_string_or_object_or_null)
{
	zend_string *str;
	zend_object *object;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OR_STR_OR_NULL(object, str)
	ZEND_PARSE_PARAMETERS_END();

	if (str) {
		RETURN_STR_COPY(str);
	} else if (object) {
		RETURN_OBJ_COPY(object);
	} else {
		RETURN_NULL();
	}
}

/* Tests Z_PARAM_OBJ_OF_CLASS_OR_STR */
static ZEND_FUNCTION(zend_string_or_stdclass)
{
	zend_string *str;
	zend_object *object;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(object, zend_standard_class_def, str)
	ZEND_PARSE_PARAMETERS_END();

	if (str) {
		RETURN_STR_COPY(str);
	} else {
		RETURN_OBJ_COPY(object);
	}
}

static ZEND_FUNCTION(zend_test_compile_string)
{
	zend_string *source_string = NULL;
	zend_string *filename = NULL;
	zend_long position = ZEND_COMPILE_POSITION_AT_OPEN_TAG;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_STR(source_string)
		Z_PARAM_STR(filename)
		Z_PARAM_LONG(position)
	ZEND_PARSE_PARAMETERS_END();

	zend_op_array *op_array = NULL;

	op_array = compile_string(source_string, ZSTR_VAL(filename), position);

	if (op_array) {
		zval retval;

		zend_try {
			ZVAL_UNDEF(&retval);
			zend_execute(op_array, &retval);
		} zend_catch {
			destroy_op_array(op_array);
			efree_size(op_array, sizeof(zend_op_array));
			zend_bailout();
		} zend_end_try();

		destroy_op_array(op_array);
		efree_size(op_array, sizeof(zend_op_array));
	}

	return;
}

/* Tests Z_PARAM_OBJ_OF_CLASS_OR_STR_OR_NULL */
static ZEND_FUNCTION(zend_string_or_stdclass_or_null)
{
	zend_string *str;
	zend_object *object;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OF_CLASS_OR_STR_OR_NULL(object, zend_standard_class_def, str)
	ZEND_PARSE_PARAMETERS_END();

	if (str) {
		RETURN_STR_COPY(str);
	} else if (object) {
		RETURN_OBJ_COPY(object);
	} else {
		RETURN_NULL();
	}
}

/* Tests Z_PARAM_NUMBER_OR_STR */
static ZEND_FUNCTION(zend_number_or_string)
{
	zval *input;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_NUMBER_OR_STR(input)
	ZEND_PARSE_PARAMETERS_END();

	switch (Z_TYPE_P(input)) {
		case IS_LONG:
			RETURN_LONG(Z_LVAL_P(input));
		case IS_DOUBLE:
			RETURN_DOUBLE(Z_DVAL_P(input));
		case IS_STRING:
			RETURN_STR_COPY(Z_STR_P(input));
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

/* Tests Z_PARAM_NUMBER_OR_STR_OR_NULL */
static ZEND_FUNCTION(zend_number_or_string_or_null)
{
	zval *input;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_NUMBER_OR_STR_OR_NULL(input)
	ZEND_PARSE_PARAMETERS_END();

	if (!input) {
		RETURN_NULL();
	}

	switch (Z_TYPE_P(input)) {
		case IS_LONG:
			RETURN_LONG(Z_LVAL_P(input));
		case IS_DOUBLE:
			RETURN_DOUBLE(Z_DVAL_P(input));
		case IS_STRING:
			RETURN_STR_COPY(Z_STR_P(input));
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

static ZEND_FUNCTION(zend_weakmap_attach)
{
	zval *value;
	zend_object *obj;

	ZEND_PARSE_PARAMETERS_START(2, 2)
			Z_PARAM_OBJ(obj)
			Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_weakrefs_hash_add(&ZT_G(global_weakmap), obj, value)) {
		Z_TRY_ADDREF_P(value);
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

static ZEND_FUNCTION(zend_weakmap_remove)
{
	zend_object *obj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_OBJ(obj)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(zend_weakrefs_hash_del(&ZT_G(global_weakmap), obj) == SUCCESS);
}

static ZEND_FUNCTION(zend_weakmap_dump)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_ARR(zend_array_dup(&ZT_G(global_weakmap)));
}

static ZEND_FUNCTION(zend_get_current_func_name)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zend_string *function_name = get_function_or_method_name(EG(current_execute_data)->prev_execute_data->func);

    RETURN_STR(function_name);
}

#if defined(HAVE_LIBXML) && !defined(PHP_WIN32)
static ZEND_FUNCTION(zend_test_override_libxml_global_state)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ZEND_DIAGNOSTIC_IGNORED_START("-Wdeprecated-declarations")
	xmlLoadExtDtdDefaultValue = 1;
	xmlDoValidityCheckingDefaultValue = 1;
	(void) xmlPedanticParserDefault(1);
	(void) xmlSubstituteEntitiesDefault(1);
	(void) xmlLineNumbersDefault(1);
	(void) xmlKeepBlanksDefault(0);
	ZEND_DIAGNOSTIC_IGNORED_END
}
#endif

/* TESTS Z_PARAM_ITERABLE and Z_PARAM_ITERABLE_OR_NULL */
static ZEND_FUNCTION(zend_iterable)
{
	zval *arg1, *arg2;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ITERABLE(arg1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE_OR_NULL(arg2)
	ZEND_PARSE_PARAMETERS_END();
}

static ZEND_FUNCTION(zend_iterable_legacy)
{
	zval *arg1, *arg2;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ITERABLE(arg1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE_OR_NULL(arg2)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_COPY(arg1);
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_iterable_legacy, 0, 1, IS_ITERABLE, 0)
	ZEND_ARG_TYPE_INFO(0, arg1, IS_ITERABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, arg2, IS_ITERABLE, 1, "null")
ZEND_END_ARG_INFO()

static const zend_function_entry ext_function_legacy[] = {
	ZEND_FE(zend_iterable_legacy, arginfo_zend_iterable_legacy)
	ZEND_FE_END
};

/* Call a method on a class or object using zend_call_method() */
static ZEND_FUNCTION(zend_call_method)
{
	zend_string *method_name;
	zval *class_or_object, *arg1 = NULL, *arg2 = NULL;
	zend_object *obj = NULL;
	zend_class_entry *ce = NULL;
	int argc = ZEND_NUM_ARGS();

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_ZVAL(class_or_object)
		Z_PARAM_STR(method_name)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(arg1)
		Z_PARAM_ZVAL(arg2)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(class_or_object) == IS_OBJECT) {
		obj = Z_OBJ_P(class_or_object);
		ce = obj->ce;
	} else if (Z_TYPE_P(class_or_object) == IS_STRING) {
		ce = zend_lookup_class(Z_STR_P(class_or_object));
		if (!ce) {
			zend_error_noreturn(E_ERROR, "Unknown class '%s'", Z_STRVAL_P(class_or_object));
			return;
		}
	} else {
		zend_argument_type_error(1, "must be of type object|string, %s given", zend_zval_value_name(class_or_object));
		return;
	}

	ZEND_ASSERT((argc >= 2) && (argc <= 4));
	zend_call_method(obj, ce, NULL, ZSTR_VAL(method_name), ZSTR_LEN(method_name), return_value, argc - 2, arg1, arg2);
}

static ZEND_FUNCTION(zend_get_unit_enum)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_OBJ_COPY(zend_enum_get_case_cstr(zend_test_unit_enum, "Foo"));
}

static ZEND_FUNCTION(zend_test_zend_ini_parse_quantity)
{
	zend_string *str;
	zend_string *errstr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_LONG(zend_ini_parse_quantity(str, &errstr));

	if (errstr) {
		zend_error(E_WARNING, "%s", ZSTR_VAL(errstr));
		zend_string_release(errstr);
	}
}

static ZEND_FUNCTION(zend_test_zend_ini_parse_uquantity)
{
	zend_string *str;
	zend_string *errstr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_LONG((zend_long)zend_ini_parse_uquantity(str, &errstr));

	if (errstr) {
		zend_error(E_WARNING, "%s", ZSTR_VAL(errstr));
		zend_string_release(errstr);
	}
}

static ZEND_FUNCTION(zend_test_zend_ini_str)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_STR(ZT_G(str_test));
}

static ZEND_FUNCTION(zend_test_is_string_marked_as_valid_utf8)
{
	zend_string *str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(ZSTR_IS_VALID_UTF8(str));
}

static ZEND_FUNCTION(ZendTestNS2_namespaced_func)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_TRUE;
}

static ZEND_FUNCTION(ZendTestNS2_namespaced_deprecated_func)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

static ZEND_FUNCTION(ZendTestNS2_ZendSubNS_namespaced_func)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_TRUE;
}

static ZEND_FUNCTION(ZendTestNS2_ZendSubNS_namespaced_deprecated_func)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

static ZEND_FUNCTION(zend_test_parameter_with_attribute)
{
	zend_string *parameter;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(parameter)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(1);
}

#ifdef ZEND_CHECK_STACK_LIMIT
static ZEND_FUNCTION(zend_test_zend_call_stack_get)
{
	zend_call_stack stack;

	ZEND_PARSE_PARAMETERS_NONE();

	if (zend_call_stack_get(&stack)) {
		zend_string *str;

		array_init(return_value);

		str = strpprintf(0, "%p", stack.base);
		add_assoc_str(return_value, "base", str);

		str = strpprintf(0, "0x%zx", stack.max_size);
		add_assoc_str(return_value, "max_size", str);

		str = strpprintf(0, "%p", zend_call_stack_position());
		add_assoc_str(return_value, "position", str);

		str = strpprintf(0, "%p", EG(stack_limit));
		add_assoc_str(return_value, "EG(stack_limit)", str);

		return;
	}

	RETURN_NULL();
}

zend_long (*volatile zend_call_stack_use_all_fun)(void *limit);

static zend_long zend_call_stack_use_all(void *limit)
{
	if (zend_call_stack_overflowed(limit)) {
		return 1;
	}

	return 1 + zend_call_stack_use_all_fun(limit);
}

static ZEND_FUNCTION(zend_test_zend_call_stack_use_all)
{
	zend_call_stack stack;

	ZEND_PARSE_PARAMETERS_NONE();

	if (!zend_call_stack_get(&stack)) {
		return;
	}

	zend_call_stack_use_all_fun = zend_call_stack_use_all;

	void *limit = zend_call_stack_limit(stack.base, stack.max_size, 4096);

	RETURN_LONG(zend_call_stack_use_all(limit));
}
#endif /* ZEND_CHECK_STACK_LIMIT */

static ZEND_FUNCTION(zend_get_map_ptr_last)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG(CG(map_ptr_last));
}

static ZEND_FUNCTION(zend_test_crash)
{
	zend_string *message = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(message)
	ZEND_PARSE_PARAMETERS_END();

	if (message) {
		php_printf("%s", ZSTR_VAL(message));
	}

	char *invalid = (char *) 1;
	php_printf("%s", invalid);
}

static bool has_opline(zend_execute_data *execute_data)
{
	return execute_data
		&& execute_data->func
		&& ZEND_USER_CODE(execute_data->func->type)
		&& execute_data->opline
	;
}

void * zend_test_custom_malloc(size_t len ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	if (has_opline(EG(current_execute_data))) {
		assert(EG(current_execute_data)->opline->lineno != (uint32_t)-1);
	}
	return _zend_mm_alloc(ZT_G(zend_orig_heap), len ZEND_FILE_LINE_EMPTY_CC ZEND_FILE_LINE_EMPTY_CC);
}

void zend_test_custom_free(void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	if (has_opline(EG(current_execute_data))) {
		assert(EG(current_execute_data)->opline->lineno != (uint32_t)-1);
	}
	_zend_mm_free(ZT_G(zend_orig_heap), ptr ZEND_FILE_LINE_EMPTY_CC ZEND_FILE_LINE_EMPTY_CC);
}

void * zend_test_custom_realloc(void * ptr, size_t len ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	if (has_opline(EG(current_execute_data))) {
		assert(EG(current_execute_data)->opline->lineno != (uint32_t)-1);
	}
	return _zend_mm_realloc(ZT_G(zend_orig_heap), ptr, len ZEND_FILE_LINE_EMPTY_CC ZEND_FILE_LINE_EMPTY_CC);
}

static PHP_INI_MH(OnUpdateZendTestObserveOplineInZendMM)
{
	if (new_value == NULL) {
		return FAILURE;
	}

	int int_value = zend_ini_parse_bool(new_value);

	if (int_value == 1) {
		// `zend_mm_heap` is a private struct, so we have not way to find the
		// actual size, but 4096 bytes should be enough
		ZT_G(zend_test_heap) = malloc(4096);
		memset(ZT_G(zend_test_heap), 0, 4096);
		zend_mm_set_custom_handlers(
			ZT_G(zend_test_heap),
			zend_test_custom_malloc,
			zend_test_custom_free,
			zend_test_custom_realloc
		);
		ZT_G(zend_orig_heap) = zend_mm_get_heap();
		zend_mm_set_heap(ZT_G(zend_test_heap));
	} else if (ZT_G(zend_test_heap))  {
		free(ZT_G(zend_test_heap));
		ZT_G(zend_test_heap) = NULL;
		zend_mm_set_heap(ZT_G(zend_orig_heap));
	}
	return OnUpdateBool(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}

static ZEND_FUNCTION(zend_test_fill_packed_array)
{
	HashTable *parameter;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT_EX(parameter, 0, 1)
	ZEND_PARSE_PARAMETERS_END();

	if (!HT_IS_PACKED(parameter)) {
		zend_argument_value_error(1, "must be a packed array");
		RETURN_THROWS();
	}

	zend_hash_extend(parameter, parameter->nNumUsed + 10, true);
	ZEND_HASH_FILL_PACKED(parameter) {
		for (int i = 0; i < 10; i++) {
			zval value;
			ZVAL_LONG(&value, i);
			ZEND_HASH_FILL_ADD(&value);
		}
	} ZEND_HASH_FILL_END();
}

static ZEND_FUNCTION(get_open_basedir)
{
	ZEND_PARSE_PARAMETERS_NONE();
	if (PG(open_basedir)) {
		RETURN_STRING(PG(open_basedir));
	} else {
		RETURN_NULL();
	}
}

static ZEND_FUNCTION(zend_test_is_pcre_bundled)
{
	ZEND_PARSE_PARAMETERS_NONE();
#if HAVE_BUNDLED_PCRE
	RETURN_TRUE;
#else
	RETURN_FALSE;
#endif
}

static zend_object *zend_test_class_new(zend_class_entry *class_type)
{
	zend_object *obj = zend_objects_new(class_type);
	object_properties_init(obj, class_type);
	obj->handlers = &zend_test_class_handlers;
	return obj;
}

static zend_function *zend_test_class_method_get(zend_object **object, zend_string *name, const zval *key)
{
	if (zend_string_equals_literal_ci(name, "test")) {
		zend_internal_function *fptr;

		if (EXPECTED(EG(trampoline).common.function_name == NULL)) {
			fptr = (zend_internal_function *) &EG(trampoline);
		} else {
			fptr = emalloc(sizeof(zend_internal_function));
	    }
		memset(fptr, 0, sizeof(zend_internal_function));
		fptr->type = ZEND_INTERNAL_FUNCTION;
		fptr->num_args = 1;
		fptr->scope = (*object)->ce;
		fptr->fn_flags = ZEND_ACC_CALL_VIA_HANDLER;
		fptr->function_name = zend_string_copy(name);
		fptr->handler = ZEND_FN(zend_test_func);
		fptr->doc_comment = NULL;

		return (zend_function*)fptr;
	}
	return zend_std_get_method(object, name, key);
}

static zend_function *zend_test_class_static_method_get(zend_class_entry *ce, zend_string *name)
{
	if (zend_string_equals_literal_ci(name, "test")) {
		zend_internal_function *fptr;

		if (EXPECTED(EG(trampoline).common.function_name == NULL)) {
			fptr = (zend_internal_function *) &EG(trampoline);
		} else {
			fptr = emalloc(sizeof(zend_internal_function));
		}
		memset(fptr, 0, sizeof(zend_internal_function));
		fptr->type = ZEND_INTERNAL_FUNCTION;
		fptr->num_args = 1;
		fptr->scope = ce;
		fptr->fn_flags = ZEND_ACC_CALL_VIA_HANDLER|ZEND_ACC_STATIC;
		fptr->function_name = zend_string_copy(name);
		fptr->handler = ZEND_FN(zend_test_func);
		fptr->doc_comment = NULL;

		return (zend_function*)fptr;
	}
	return zend_std_get_static_method(ce, name, NULL);
}

void zend_attribute_validate_zendtestattribute(zend_attribute *attr, uint32_t target, zend_class_entry *scope)
{
	if (target != ZEND_ATTRIBUTE_TARGET_CLASS) {
		zend_error(E_COMPILE_ERROR, "Only classes can be marked with #[ZendTestAttribute]");
	}
}

static ZEND_METHOD(_ZendTestClass, __toString)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_EMPTY_STRING();
}

/* Internal function returns bool, we return int. */
static ZEND_METHOD(_ZendTestClass, is_object)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG(42);
}

static ZEND_METHOD(_ZendTestClass, returnsStatic) {
	ZEND_PARSE_PARAMETERS_NONE();
	object_init_ex(return_value, zend_get_called_scope(execute_data));
}

static ZEND_METHOD(_ZendTestClass, returnsThrowable)
{
	ZEND_PARSE_PARAMETERS_NONE();
	zend_throw_error(NULL, "Dummy");
}

static ZEND_METHOD(_ZendTestClass, variadicTest) {
	int      argc, i;
	zval    *args = NULL;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC('*', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	for (i = 0; i < argc; i++) {
		zval *arg = args + i;

		if (Z_TYPE_P(arg) == IS_STRING) {
			continue;
		}
		if (Z_TYPE_P(arg) == IS_OBJECT && instanceof_function(Z_OBJ_P(arg)->ce, zend_ce_iterator)) {
			continue;
		}

		zend_argument_type_error(i + 1, "must be of class Iterator or a string, %s given", zend_zval_type_name(arg));
		RETURN_THROWS();
	}

	object_init_ex(return_value, zend_get_called_scope(execute_data));
}

static ZEND_METHOD(_ZendTestChildClass, returnsThrowable)
{
	ZEND_PARSE_PARAMETERS_NONE();
	zend_throw_error(NULL, "Dummy");
}

static ZEND_METHOD(ZendAttributeTest, testMethod)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_TRUE;
}

static ZEND_METHOD(_ZendTestTrait, testMethod)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_TRUE;
}

static ZEND_METHOD(ZendTestNS_Foo, method)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(0);
}

static ZEND_METHOD(ZendTestNS_UnlikelyCompileError, method)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_NULL();
}

static ZEND_METHOD(ZendTestNS_NotUnlikelyCompileError, method)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_NULL();
}

static ZEND_METHOD(ZendTestNS2_Foo, method)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

static ZEND_METHOD(ZendTestNS2_ZendSubNS_Foo, method)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

static ZEND_METHOD(ZendTestParameterAttribute, __construct)
{
	zend_string *parameter;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(parameter)
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_STR_COPY(OBJ_PROP_NUM(Z_OBJ_P(ZEND_THIS), 0), parameter);
}

static ZEND_METHOD(ZendTestPropertyAttribute, __construct)
{
	zend_string *parameter;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(parameter)
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_STR_COPY(OBJ_PROP_NUM(Z_OBJ_P(ZEND_THIS), 0), parameter);
}

static ZEND_METHOD(ZendTestClassWithMethodWithParameterAttribute, no_override)
{
	zend_string *parameter;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(parameter)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(2);
}

static ZEND_METHOD(ZendTestClassWithMethodWithParameterAttribute, override)
{
	zend_string *parameter;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(parameter)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(3);
}

static ZEND_METHOD(ZendTestChildClassWithMethodWithParameterAttribute, override)
{
	zend_string *parameter;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(parameter)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(4);
}

static ZEND_METHOD(ZendTestForbidDynamicCall, call)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_forbid_dynamic_call();
}

static ZEND_METHOD(ZendTestForbidDynamicCall, callStatic)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_forbid_dynamic_call();
}

static ZEND_METHOD(_ZendTestMagicCall, __call)
{
	zend_string *name;
	zval *arguments;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(name)
		Z_PARAM_ARRAY(arguments)
	ZEND_PARSE_PARAMETERS_END();

	zval name_zv;
	ZVAL_STR(&name_zv, name);

	zend_string_addref(name);
	Z_TRY_ADDREF_P(arguments);
	RETURN_ARR(zend_new_pair(&name_zv, arguments));
}

PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("zend_test.replace_zend_execute_ex", "0", PHP_INI_SYSTEM, OnUpdateBool, replace_zend_execute_ex, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.register_passes", "0", PHP_INI_SYSTEM, OnUpdateBool, register_passes, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.print_stderr_mshutdown", "0", PHP_INI_SYSTEM, OnUpdateBool, print_stderr_mshutdown, zend_zend_test_globals, zend_test_globals)
#ifdef HAVE_COPY_FILE_RANGE
	STD_PHP_INI_ENTRY("zend_test.limit_copy_file_range", "-1", PHP_INI_ALL, OnUpdateLong, limit_copy_file_range, zend_zend_test_globals, zend_test_globals)
#endif
	STD_PHP_INI_ENTRY("zend_test.quantity_value", "0", PHP_INI_ALL, OnUpdateLong, quantity_value, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_ENTRY("zend_test.str_test", "", PHP_INI_ALL, OnUpdateStr, str_test, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_ENTRY("zend_test.not_empty_str_test", "val", PHP_INI_ALL, OnUpdateStrNotEmpty, not_empty_str_test, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observe_opline_in_zendmm", "0", PHP_INI_ALL, OnUpdateZendTestObserveOplineInZendMM, observe_opline_in_zendmm, zend_zend_test_globals, zend_test_globals)
PHP_INI_END()

void (*old_zend_execute_ex)(zend_execute_data *execute_data);
static void custom_zend_execute_ex(zend_execute_data *execute_data)
{
	old_zend_execute_ex(execute_data);
}

static void le_throwing_resource_dtor(zend_resource *rsrc)
{
	zend_throw_exception(NULL, "Throwing resource destructor called", 0);
}

static ZEND_METHOD(_ZendTestClass, takesUnionType)
{
	zend_object *obj;
	ZEND_PARSE_PARAMETERS_START(1, 1);
		Z_PARAM_OBJ(obj)
	ZEND_PARSE_PARAMETERS_END();
	// we have to perform type-checking to avoid arginfo/zpp mismatch error
	bool type_matches = (
		instanceof_function(obj->ce, zend_standard_class_def)
		||
		instanceof_function(obj->ce, zend_ce_iterator)
	);
	if (!type_matches) {
		zend_string *ty = zend_type_to_string(execute_data->func->internal_function.arg_info->type);
		zend_argument_type_error(1, "must be of type %s, %s given", ty->val, obj->ce->name->val);
		zend_string_release(ty);
		RETURN_THROWS();
	}

	RETURN_NULL();
}

// Returns a newly allocated DNF type `Iterator|(Traversable&Countable)`.
//
// We need to generate it "manually" because gen_stubs.php does not support codegen for DNF types ATM.
static zend_type create_test_dnf_type(void) {
	zend_string *class_Iterator = zend_string_init_interned("Iterator", sizeof("Iterator") - 1, true);
	zend_alloc_ce_cache(class_Iterator);
	zend_string *class_Traversable = ZSTR_KNOWN(ZEND_STR_TRAVERSABLE);
	zend_string *class_Countable = zend_string_init_interned("Countable", sizeof("Countable") - 1, true);
	zend_alloc_ce_cache(class_Countable);
	//
	zend_type_list *intersection_list = malloc(ZEND_TYPE_LIST_SIZE(2));
	intersection_list->num_types = 2;
	intersection_list->types[0] = (zend_type) ZEND_TYPE_INIT_CLASS(class_Traversable, 0, 0);
	intersection_list->types[1] = (zend_type) ZEND_TYPE_INIT_CLASS(class_Countable, 0, 0);
	zend_type_list *union_list = malloc(ZEND_TYPE_LIST_SIZE(2));
	union_list->num_types = 2;
	union_list->types[0] = (zend_type) ZEND_TYPE_INIT_CLASS(class_Iterator, 0, 0);
	union_list->types[1] = (zend_type) ZEND_TYPE_INIT_INTERSECTION(intersection_list, 0);
	return (zend_type) ZEND_TYPE_INIT_UNION(union_list, 0);
}

static void register_ZendTestClass_dnf_property(zend_class_entry *ce) {
	zend_string *prop_name = zend_string_init_interned("dnfProperty", sizeof("dnfProperty") - 1, true);
	zval default_value;
	ZVAL_UNDEF(&default_value);
	zend_type type = create_test_dnf_type();
	zend_declare_typed_property(ce, prop_name, &default_value, ZEND_ACC_PUBLIC, NULL, type);
}

// arg_info for `zend_test_internal_dnf_arguments`
// The types are upgraded to DNF types in `register_dynamic_function_entries()`
static zend_internal_arg_info arginfo_zend_test_internal_dnf_arguments[] = {
	// first entry is a zend_internal_function_info (see zend_compile.h): {argument_count, return_type, unused}
	{(const char*)(uintptr_t)(1), {0}, NULL},
	{"arg", {0}, NULL}
};

static ZEND_NAMED_FUNCTION(zend_test_internal_dnf_arguments)
{
	zend_object *obj;
	ZEND_PARSE_PARAMETERS_START(1, 1);
		Z_PARAM_OBJ(obj)
	ZEND_PARSE_PARAMETERS_END();
	// we have to perform type-checking to avoid arginfo/zpp mismatch error
	bool type_matches = (
		instanceof_function(obj->ce, zend_ce_iterator)
		|| (
			instanceof_function(obj->ce, zend_ce_traversable)
			&& instanceof_function(obj->ce, zend_ce_countable)
		)
	);
	if (!type_matches) {
		zend_string *ty = zend_type_to_string(arginfo_zend_test_internal_dnf_arguments[1].type);
		zend_argument_type_error(1, "must be of type %s, %s given", ty->val, obj->ce->name->val);
		zend_string_release(ty);
		RETURN_THROWS();
	}

	RETURN_OBJ_COPY(obj);
}

static const zend_function_entry dynamic_function_entries[] = {
	{
		.fname = "zend_test_internal_dnf_arguments",
		.handler = zend_test_internal_dnf_arguments,
		.arg_info = arginfo_zend_test_internal_dnf_arguments,
		.num_args = 1,
		.flags = 0,
	},
	ZEND_FE_END,
};

static void register_dynamic_function_entries(int module_type) {
	// return-type is at index 0
	arginfo_zend_test_internal_dnf_arguments[0].type = create_test_dnf_type();
	arginfo_zend_test_internal_dnf_arguments[1].type = create_test_dnf_type();
	//
	zend_register_functions(NULL, dynamic_function_entries, NULL, module_type);
}

PHP_MINIT_FUNCTION(zend_test)
{
	register_dynamic_function_entries(type);

	zend_test_interface = register_class__ZendTestInterface();

	zend_test_class = register_class__ZendTestClass(zend_test_interface);
	register_ZendTestClass_dnf_property(zend_test_class);
	zend_test_class->create_object = zend_test_class_new;
	zend_test_class->get_static_method = zend_test_class_static_method_get;

	zend_test_child_class = register_class__ZendTestChildClass(zend_test_class);

	memcpy(&zend_test_class_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	zend_test_class_handlers.get_method = zend_test_class_method_get;

	zend_attribute_test_class = register_class_ZendAttributeTest();

	zend_test_trait = register_class__ZendTestTrait();

	register_test_symbols(module_number);

	zend_test_attribute = register_class_ZendTestAttribute();
	{
		zend_internal_attribute *attr = zend_mark_internal_attribute(zend_test_attribute);
		attr->validator = zend_attribute_validate_zendtestattribute;
	}

	zend_test_repeatable_attribute = register_class_ZendTestRepeatableAttribute();
	zend_mark_internal_attribute(zend_test_repeatable_attribute);

	zend_test_parameter_attribute = register_class_ZendTestParameterAttribute();
	zend_mark_internal_attribute(zend_test_parameter_attribute);

	zend_test_property_attribute = register_class_ZendTestPropertyAttribute();
	zend_mark_internal_attribute(zend_test_property_attribute);

	zend_test_class_with_method_with_parameter_attribute = register_class_ZendTestClassWithMethodWithParameterAttribute();
	zend_test_child_class_with_method_with_parameter_attribute = register_class_ZendTestChildClassWithMethodWithParameterAttribute(zend_test_class_with_method_with_parameter_attribute);

	zend_test_class_with_property_attribute = register_class_ZendTestClassWithPropertyAttribute();
	{
		zend_property_info *prop_info = zend_hash_str_find_ptr(&zend_test_class_with_property_attribute->properties_info, "attributed", sizeof("attributed") - 1);
		zend_add_property_attribute(zend_test_class_with_property_attribute, prop_info, zend_test_attribute->name, 0);
	}

	zend_test_forbid_dynamic_call = register_class_ZendTestForbidDynamicCall();

	zend_test_ns_foo_class = register_class_ZendTestNS_Foo();
	zend_test_ns_unlikely_compile_error_class = register_class_ZendTestNS_UnlikelyCompileError();
	zend_test_ns_not_unlikely_compile_error_class = register_class_ZendTestNS_NotUnlikelyCompileError();
	zend_test_ns2_foo_class = register_class_ZendTestNS2_Foo();
	zend_test_ns2_ns_foo_class = register_class_ZendTestNS2_ZendSubNS_Foo();

	zend_test_unit_enum = register_class_ZendTestUnitEnum();
	zend_test_string_enum = register_class_ZendTestStringEnum();
	zend_test_int_enum = register_class_ZendTestIntEnum();

	zend_test_magic_call = register_class__ZendTestMagicCall();

	zend_register_functions(NULL, ext_function_legacy, NULL, EG(current_module)->type);

	// Loading via dl() not supported with the observer API
	if (type != MODULE_TEMPORARY) {
		REGISTER_INI_ENTRIES();
	} else {
		(void)ini_entries;
	}

	if (ZT_G(replace_zend_execute_ex)) {
		old_zend_execute_ex = zend_execute_ex;
		zend_execute_ex = custom_zend_execute_ex;
	}

	if (ZT_G(register_passes)) {
		zend_optimizer_register_pass(pass1);
		zend_optimizer_register_pass(pass2);
	}

	zend_test_observer_init(INIT_FUNC_ARGS_PASSTHRU);
	zend_test_fiber_init();
	zend_test_iterators_init();
	zend_test_object_handlers_init();

	le_throwing_resource = zend_register_list_destructors_ex(le_throwing_resource_dtor, NULL, "throwing resource", module_number);

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(zend_test)
{
	if (type != MODULE_TEMPORARY) {
		UNREGISTER_INI_ENTRIES();
	}

	zend_test_observer_shutdown(SHUTDOWN_FUNC_ARGS_PASSTHRU);

	if (ZT_G(print_stderr_mshutdown)) {
		fprintf(stderr, "[zend_test] MSHUTDOWN\n");
	}

	return SUCCESS;
}

PHP_RINIT_FUNCTION(zend_test)
{
	zend_hash_init(&ZT_G(global_weakmap), 8, NULL, ZVAL_PTR_DTOR, 0);
	ZT_G(observer_nesting_depth) = 0;
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(zend_test)
{
	zend_ulong obj_key;
	ZEND_HASH_FOREACH_NUM_KEY(&ZT_G(global_weakmap), obj_key) {
		zend_weakrefs_hash_del(&ZT_G(global_weakmap), zend_weakref_key_to_object(obj_key));
	} ZEND_HASH_FOREACH_END();
	zend_hash_destroy(&ZT_G(global_weakmap));

	if (ZT_G(zend_test_heap))  {
		free(ZT_G(zend_test_heap));
		ZT_G(zend_test_heap) = NULL;
		zend_mm_set_heap(ZT_G(zend_orig_heap));
	}

	return SUCCESS;
}

static PHP_GINIT_FUNCTION(zend_test)
{
#if defined(COMPILE_DL_ZEND_TEST) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	memset(zend_test_globals, 0, sizeof(*zend_test_globals));

	zend_test_observer_ginit(zend_test_globals);
}

static PHP_GSHUTDOWN_FUNCTION(zend_test)
{
	zend_test_observer_gshutdown(zend_test_globals);
}

PHP_MINFO_FUNCTION(zend_test)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "zend_test extension", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

zend_module_entry zend_test_module_entry = {
	STANDARD_MODULE_HEADER,
	"zend_test",
	ext_functions,
	PHP_MINIT(zend_test),
	PHP_MSHUTDOWN(zend_test),
	PHP_RINIT(zend_test),
	PHP_RSHUTDOWN(zend_test),
	PHP_MINFO(zend_test),
	PHP_ZEND_TEST_VERSION,
	PHP_MODULE_GLOBALS(zend_test),
	PHP_GINIT(zend_test),
	PHP_GSHUTDOWN(zend_test),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_ZEND_TEST
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(zend_test)
#endif

/* The important part here is the ZEND_FASTCALL. */
PHP_ZEND_TEST_API int ZEND_FASTCALL bug78270(const char *str, size_t str_len)
{
	char * copy = zend_strndup(str, str_len);
	int r = (int) ZEND_ATOL(copy);
	free(copy);
	return r;
}

PHP_ZEND_TEST_API struct bug79096 bug79096(void)
{
	struct bug79096 b;

	b.a = 1;
	b.b = 1;
	return b;
}

PHP_ZEND_TEST_API void bug79532(off_t *array, size_t elems)
{
	int i;
	for (i = 0; i < elems; i++) {
		array[i] = i;
	}
}

PHP_ZEND_TEST_API int *(*bug79177_cb)(void);
void bug79177(void)
{
	bug79177_cb();
}

typedef struct bug80847_01 {
	uint64_t b;
	double c;
} bug80847_01;
typedef struct bug80847_02 {
	bug80847_01 a;
} bug80847_02;

PHP_ZEND_TEST_API bug80847_02 ffi_bug80847(bug80847_02 s) {
	s.a.b += 10;
	s.a.c -= 10.0;
	return s;
}

PHP_ZEND_TEST_API void (*bug_gh9090_void_none_ptr)(void) = NULL;
PHP_ZEND_TEST_API void (*bug_gh9090_void_int_char_ptr)(int, char *) = NULL;
PHP_ZEND_TEST_API void (*bug_gh9090_void_int_char_var_ptr)(int, char *, ...) = NULL;
PHP_ZEND_TEST_API void (*bug_gh9090_void_char_int_ptr)(char *, int) = NULL;
PHP_ZEND_TEST_API int (*bug_gh9090_int_int_char_ptr)(int, char *) = NULL;

PHP_ZEND_TEST_API void bug_gh9090_void_none(void) {
    php_printf("bug_gh9090_none\n");
}

PHP_ZEND_TEST_API void bug_gh9090_void_int_char(int i, char *s) {
    php_printf("bug_gh9090_int_char %d %s\n", i, s);
}

PHP_ZEND_TEST_API void bug_gh9090_void_int_char_var(int i, char *fmt, ...) {
    va_list args;
    char *buffer;

    va_start(args, fmt);

    zend_vspprintf(&buffer, 0, fmt, args);
    php_printf("bug_gh9090_void_int_char_var %s\n", buffer);
    efree(buffer);

    va_end(args);
}

PHP_ZEND_TEST_API int gh11934b_ffi_var_test_cdata;

#ifdef HAVE_COPY_FILE_RANGE
/**
 * This function allows us to simulate early return of copy_file_range by setting the limit_copy_file_range ini setting.
 */
PHP_ZEND_TEST_API ssize_t copy_file_range(int fd_in, off_t *off_in, int fd_out, off_t *off_out, size_t len, unsigned int flags)
{
	ssize_t (*original_copy_file_range)(int, off_t *, int, off_t *, size_t, unsigned int) = dlsym(RTLD_NEXT, "copy_file_range");
	if (ZT_G(limit_copy_file_range) >= Z_L(0)) {
		len = ZT_G(limit_copy_file_range);
	}
	return original_copy_file_range(fd_in, off_in, fd_out, off_out, len, flags);
}
#endif


static PHP_FUNCTION(zend_test_create_throwing_resource)
{
	ZEND_PARSE_PARAMETERS_NONE();
	zend_resource *res = zend_register_resource(NULL, le_throwing_resource);
	ZVAL_RES(return_value, res);
}
