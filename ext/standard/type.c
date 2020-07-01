/*
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
   | Author: Rasmus Lerdorf <rasmus@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_incomplete_class.h"

/* {{{ Returns the type of the variable */
PHP_FUNCTION(gettype)
{
	zval *arg;
	zend_string *type;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(arg)
	ZEND_PARSE_PARAMETERS_END();

	type = zend_zval_get_legacy_type(arg);
	if (EXPECTED(type)) {
		RETURN_INTERNED_STR(type);
	} else {
		RETURN_STRING("unknown type");
	}
}
/* }}} */

/* {{{ Returns the type of the variable resolving class names */
PHP_FUNCTION(get_debug_type)
{
	zval *arg;
	const char *name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(arg)
	ZEND_PARSE_PARAMETERS_END();

	switch (Z_TYPE_P(arg)) {
		case IS_NULL:
			RETURN_INTERNED_STR(ZSTR_KNOWN(ZEND_STR_NULL_LOWERCASE));
		case IS_FALSE:
		case IS_TRUE:
			RETURN_INTERNED_STR(ZSTR_KNOWN(ZEND_STR_BOOL));
		case IS_LONG:
			RETURN_INTERNED_STR(ZSTR_KNOWN(ZEND_STR_INT));
		case IS_DOUBLE:
			RETURN_INTERNED_STR(ZSTR_KNOWN(ZEND_STR_FLOAT));
		case IS_STRING:
			RETURN_INTERNED_STR(ZSTR_KNOWN(ZEND_STR_STRING));
		case IS_ARRAY:
			RETURN_INTERNED_STR(ZSTR_KNOWN(ZEND_STR_ARRAY));
		case IS_OBJECT:
			if (Z_OBJ_P(arg)->ce->ce_flags & ZEND_ACC_ANON_CLASS) {
				name = ZSTR_VAL(Z_OBJ_P(arg)->ce->name);
				RETURN_NEW_STR(zend_string_init(name, strlen(name), 0));
			} else {
				RETURN_STR_COPY(Z_OBJ_P(arg)->ce->name);
			}
		case IS_RESOURCE:
			name = zend_rsrc_list_get_rsrc_type(Z_RES_P(arg));
			if (name) {
				RETURN_NEW_STR(zend_strpprintf(0, "resource (%s)", name));
			} else {
				RETURN_INTERNED_STR(ZSTR_KNOWN(ZEND_STR_CLOSED_RESOURCE));
			}
		default:
			RETURN_INTERNED_STR(ZSTR_KNOWN(ZEND_STR_UNKNOWN));
	}
}
/* }}} */


/* {{{ Set the type of the variable */
PHP_FUNCTION(settype)
{
	zval *var;
	zend_string *type;
	zval tmp, *ptr;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(var)
		Z_PARAM_STR(type)
	ZEND_PARSE_PARAMETERS_END();

	ZEND_ASSERT(Z_ISREF_P(var));
	if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(Z_REF_P(var)))) {
		ZVAL_COPY(&tmp, Z_REFVAL_P(var));
		ptr = &tmp;
	} else {
		ptr = Z_REFVAL_P(var);
	}
	if (zend_string_equals_literal_ci(type, "integer")) {
		convert_to_long(ptr);
	} else if (zend_string_equals_literal_ci(type, "int")) {
		convert_to_long(ptr);
	} else if (zend_string_equals_literal_ci(type, "float")) {
		convert_to_double(ptr);
	} else if (zend_string_equals_literal_ci(type, "double")) { /* deprecated */
		convert_to_double(ptr);
	} else if (zend_string_equals_literal_ci(type, "string")) {
		convert_to_string(ptr);
	} else if (zend_string_equals_literal_ci(type, "array")) {
		convert_to_array(ptr);
	} else if (zend_string_equals_literal_ci(type, "object")) {
		convert_to_object(ptr);
	} else if (zend_string_equals_literal_ci(type, "bool")) {
		convert_to_boolean(ptr);
	} else if (zend_string_equals_literal_ci(type, "boolean")) {
		convert_to_boolean(ptr);
	} else if (zend_string_equals_literal_ci(type, "null")) {
		convert_to_null(ptr);
	} else {
		if (ptr == &tmp) {
			zval_ptr_dtor(&tmp);
		}
		if (zend_string_equals_literal_ci(type, "resource")) {
			zend_value_error("Cannot convert to resource type");
		} else {
			zend_argument_value_error(2, "must be a valid type");
		}
		RETURN_THROWS();
	}

	if (ptr == &tmp) {
		zend_try_assign_typed_ref(Z_REF_P(var), &tmp);
	}
	RETVAL_TRUE;
}
/* }}} */

/* {{{ Get the integer value of a variable using the optional base for the conversion */
PHP_FUNCTION(intval)
{
	zval *num;
	zend_long base = 10;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(num)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(base)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(num) != IS_STRING || base == 10) {
		RETVAL_LONG(zval_get_long(num));
		return;
	}


	if (base == 0 || base == 2) {
		char *strval = Z_STRVAL_P(num);
		size_t strlen = Z_STRLEN_P(num);

		while (isspace(*strval) && strlen) {
			strval++;
			strlen--;
		}

		/* Length of 3+ covers "0b#" and "-0b" (which results in 0) */
		if (strlen > 2) {
			int offset = 0;
			if (strval[0] == '-' || strval[0] == '+') {
				offset = 1;
			}

			if (strval[offset] == '0' && (strval[offset + 1] == 'b' || strval[offset + 1] == 'B')) {
				char *tmpval;
				strlen -= 2; /* Removing "0b" */
				tmpval = emalloc(strlen + 1);

				/* Place the unary symbol at pos 0 if there was one */
				if (offset) {
					tmpval[0] = strval[0];
				}

				/* Copy the data from after "0b" to the end of the buffer */
				memcpy(tmpval + offset, strval + offset + 2, strlen - offset);
				tmpval[strlen] = 0;

				RETVAL_LONG(ZEND_STRTOL(tmpval, NULL, 2));
				efree(tmpval);
				return;
			}
		}
	}

	RETVAL_LONG(ZEND_STRTOL(Z_STRVAL_P(num), NULL, base));
}
/* }}} */

/* {{{ Get the float value of a variable */
PHP_FUNCTION(floatval)
{
	zval *num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(num)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_DOUBLE(zval_get_double(num));
}
/* }}} */

/* {{{ Get the boolean value of a variable */
PHP_FUNCTION(boolval)
{
	zval *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(zend_is_true(value));
}
/* }}} */

/* {{{ Get the string value of a variable */
PHP_FUNCTION(strval)
{
	zval *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_STR(zval_get_string(value));
}
/* }}} */

static inline void php_is_type(INTERNAL_FUNCTION_PARAMETERS, int type)
{
	zval *arg;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(arg)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(arg) == type) {
		if (type == IS_RESOURCE) {
			const char *type_name = zend_rsrc_list_get_rsrc_type(Z_RES_P(arg));
			if (!type_name) {
				RETURN_FALSE;
			}
		}
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}


/* {{{ Returns true if variable is null
   Warning: This function is special-cased by zend_compile.c and so is usually bypassed */
PHP_FUNCTION(is_null)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_NULL);
}
/* }}} */

/* {{{ Returns true if variable is a resource
   Warning: This function is special-cased by zend_compile.c and so is usually bypassed */
PHP_FUNCTION(is_resource)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_RESOURCE);
}
/* }}} */

/* {{{ Returns true if variable is a boolean
   Warning: This function is special-cased by zend_compile.c and so is usually bypassed */
PHP_FUNCTION(is_bool)
{
	zval *arg;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(arg)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(Z_TYPE_P(arg) == IS_FALSE || Z_TYPE_P(arg) == IS_TRUE);
}
/* }}} */

/* {{{ Returns true if variable is an integer
   Warning: This function is special-cased by zend_compile.c and so is usually bypassed */
PHP_FUNCTION(is_int)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_LONG);
}
/* }}} */

/* {{{ Returns true if variable is float point
   Warning: This function is special-cased by zend_compile.c and so is usually bypassed */
PHP_FUNCTION(is_float)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_DOUBLE);
}
/* }}} */

/* {{{ Returns true if variable is a string
   Warning: This function is special-cased by zend_compile.c and so is usually bypassed */
PHP_FUNCTION(is_string)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_STRING);
}
/* }}} */

/* {{{ Returns true if variable is an array
   Warning: This function is special-cased by zend_compile.c and so is usually bypassed */
PHP_FUNCTION(is_array)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_ARRAY);
}
/* }}} */

/* {{{ Returns true if variable is an object
   Warning: This function is special-cased by zend_compile.c and so is usually bypassed */
PHP_FUNCTION(is_object)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_OBJECT);
}
/* }}} */

/* {{{ Returns true if value is a number or a numeric string */
PHP_FUNCTION(is_numeric)
{
	zval *arg;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(arg)
	ZEND_PARSE_PARAMETERS_END();

	switch (Z_TYPE_P(arg)) {
		case IS_LONG:
		case IS_DOUBLE:
			RETURN_TRUE;
			break;

		case IS_STRING:
			if (is_numeric_string(Z_STRVAL_P(arg), Z_STRLEN_P(arg), NULL, NULL, 0)) {
				RETURN_TRUE;
			} else {
				RETURN_FALSE;
			}
			break;

		default:
			RETURN_FALSE;
			break;
	}
}
/* }}} */

/* {{{ Returns true if value is a scalar */
PHP_FUNCTION(is_scalar)
{
	zval *arg;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(arg)
	ZEND_PARSE_PARAMETERS_END();

	switch (Z_TYPE_P(arg)) {
		case IS_FALSE:
		case IS_TRUE:
		case IS_DOUBLE:
		case IS_LONG:
		case IS_STRING:
			RETURN_TRUE;
			break;

		default:
			RETURN_FALSE;
			break;
	}
}
/* }}} */

/* {{{ Returns true if var is callable. */
PHP_FUNCTION(is_callable)
{
	zval *var, *callable_name = NULL;
	zend_string *name;
	char *error;
	zend_bool retval;
	zend_bool syntax_only = 0;
	int check_flags = 0;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_ZVAL(var)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(syntax_only)
		Z_PARAM_ZVAL(callable_name)
	ZEND_PARSE_PARAMETERS_END();

	if (syntax_only) {
		check_flags |= IS_CALLABLE_CHECK_SYNTAX_ONLY;
	}
	if (ZEND_NUM_ARGS() > 2) {
		retval = zend_is_callable_ex(var, NULL, check_flags, &name, NULL, &error);
		ZEND_TRY_ASSIGN_REF_STR(callable_name, name);
	} else {
		retval = zend_is_callable_ex(var, NULL, check_flags, NULL, NULL, &error);
	}
	if (error) {
		/* ignore errors */
		efree(error);
	}

	RETURN_BOOL(retval);
}
/* }}} */

/* {{{ Returns true if var is iterable (array or instance of Traversable). */
PHP_FUNCTION(is_iterable)
{
	zval *var;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(var)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(zend_is_iterable(var));
}
/* }}} */

/* {{{ Returns true if var is countable (array or instance of Countable). */
PHP_FUNCTION(is_countable)
{
	zval *var;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(var)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(zend_is_countable(var));
}
/* }}} */
