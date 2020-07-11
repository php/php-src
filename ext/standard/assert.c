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
   | Author: Thies C. Arntzen <thies@thieso.net>                          |
   +----------------------------------------------------------------------+
*/

/* {{{ includes */
#include "php.h"
#include "php_assert.h"
#include "php_ini.h"
#include "zend_exceptions.h"
/* }}} */

ZEND_BEGIN_MODULE_GLOBALS(assert)
	zval callback;
	char *cb;
	zend_bool active;
	zend_bool bail;
	zend_bool warning;
	zend_bool exception;
ZEND_END_MODULE_GLOBALS(assert)

ZEND_DECLARE_MODULE_GLOBALS(assert)

static zend_class_entry *assertion_error_ce;

#define ASSERTG(v) ZEND_MODULE_GLOBALS_ACCESSOR(assert, v)

#define SAFE_STRING(s) ((s)?(s):"")

enum {
	ASSERT_ACTIVE=1,
	ASSERT_CALLBACK,
	ASSERT_BAIL,
	ASSERT_WARNING,
	ASSERT_EXCEPTION
};

static PHP_INI_MH(OnChangeCallback) /* {{{ */
{
	if (EG(current_execute_data)) {
		if (Z_TYPE(ASSERTG(callback)) != IS_UNDEF) {
			zval_ptr_dtor(&ASSERTG(callback));
			ZVAL_UNDEF(&ASSERTG(callback));
		}
		if (new_value && (Z_TYPE(ASSERTG(callback)) != IS_UNDEF || ZSTR_LEN(new_value))) {
			ZVAL_STR_COPY(&ASSERTG(callback), new_value);
		}
	} else {
		if (ASSERTG(cb)) {
			pefree(ASSERTG(cb), 1);
		}
		if (new_value && ZSTR_LEN(new_value)) {
			ASSERTG(cb) = pemalloc(ZSTR_LEN(new_value) + 1, 1);
			memcpy(ASSERTG(cb), ZSTR_VAL(new_value), ZSTR_LEN(new_value));
			ASSERTG(cb)[ZSTR_LEN(new_value)] = '\0';
		} else {
			ASSERTG(cb) = NULL;
		}
	}
	return SUCCESS;
}
/* }}} */

PHP_INI_BEGIN()
	 STD_PHP_INI_BOOLEAN("assert.active",		"1",	PHP_INI_ALL,	OnUpdateBool,		active,	 			zend_assert_globals,		assert_globals)
	 STD_PHP_INI_BOOLEAN("assert.bail",		"0",	PHP_INI_ALL,	OnUpdateBool,		bail,	 			zend_assert_globals,		assert_globals)
	 STD_PHP_INI_BOOLEAN("assert.warning",	"1",	PHP_INI_ALL,	OnUpdateBool,		warning, 			zend_assert_globals,		assert_globals)
	 PHP_INI_ENTRY("assert.callback",		NULL,	PHP_INI_ALL,	OnChangeCallback)
	 STD_PHP_INI_BOOLEAN("assert.exception",	"0",	PHP_INI_ALL,	OnUpdateBool,		exception, 			zend_assert_globals,		assert_globals)
PHP_INI_END()

static void php_assert_init_globals(zend_assert_globals *assert_globals_p) /* {{{ */
{
	ZVAL_UNDEF(&assert_globals_p->callback);
	assert_globals_p->cb = NULL;
}
/* }}} */

PHP_MINIT_FUNCTION(assert) /* {{{ */
{
	zend_class_entry ce;

	ZEND_INIT_MODULE_GLOBALS(assert, php_assert_init_globals, NULL);

	REGISTER_INI_ENTRIES();

	REGISTER_LONG_CONSTANT("ASSERT_ACTIVE", ASSERT_ACTIVE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ASSERT_CALLBACK", ASSERT_CALLBACK, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ASSERT_BAIL", ASSERT_BAIL, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ASSERT_WARNING", ASSERT_WARNING, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ASSERT_EXCEPTION", ASSERT_EXCEPTION, CONST_CS|CONST_PERSISTENT);

	INIT_CLASS_ENTRY(ce, "AssertionError", NULL);
	assertion_error_ce = zend_register_internal_class_ex(&ce, zend_ce_error);

	return SUCCESS;
}
/* }}} */

PHP_MSHUTDOWN_FUNCTION(assert) /* {{{ */
{
	if (ASSERTG(cb)) {
		pefree(ASSERTG(cb), 1);
		ASSERTG(cb) = NULL;
	}
	return SUCCESS;
}
/* }}} */

PHP_RSHUTDOWN_FUNCTION(assert) /* {{{ */
{
	if (Z_TYPE(ASSERTG(callback)) != IS_UNDEF) {
		zval_ptr_dtor(&ASSERTG(callback));
		ZVAL_UNDEF(&ASSERTG(callback));
	}

	return SUCCESS;
}
/* }}} */

PHP_MINFO_FUNCTION(assert) /* {{{ */
{
	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ Checks if assertion is false */
PHP_FUNCTION(assert)
{
	zval *assertion;
	zval *description = NULL;

	if (! ASSERTG(active)) {
		RETURN_TRUE;
	}

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(assertion)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(description)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_is_true(assertion)) {
		RETURN_TRUE;
	}

	if (Z_TYPE(ASSERTG(callback)) == IS_UNDEF && ASSERTG(cb)) {
		ZVAL_STRING(&ASSERTG(callback), ASSERTG(cb));
	}

	if (Z_TYPE(ASSERTG(callback)) != IS_UNDEF) {
		zval args[4];
		zval retval;
		uint32_t lineno = zend_get_executed_lineno();
		const char *filename = zend_get_executed_filename();

		ZVAL_STRING(&args[0], SAFE_STRING(filename));
		ZVAL_LONG(&args[1], lineno);
		ZVAL_NULL(&args[2]);

		ZVAL_FALSE(&retval);

		/* XXX do we want to check for error here? */
		if (!description) {
			call_user_function(NULL, NULL, &ASSERTG(callback), &retval, 3, args);
			zval_ptr_dtor(&(args[2]));
			zval_ptr_dtor(&(args[0]));
		} else {
			ZVAL_STR(&args[3], zval_get_string(description));
			call_user_function(NULL, NULL, &ASSERTG(callback), &retval, 4, args);
			zval_ptr_dtor(&(args[3]));
			zval_ptr_dtor(&(args[2]));
			zval_ptr_dtor(&(args[0]));
		}

		zval_ptr_dtor(&retval);
	}

	if (ASSERTG(exception)) {
		if (!description) {
			zend_throw_exception(assertion_error_ce, NULL, E_ERROR);
		} else if (Z_TYPE_P(description) == IS_OBJECT &&
			instanceof_function(Z_OBJCE_P(description), zend_ce_throwable)) {
			Z_ADDREF_P(description);
			zend_throw_exception_object(description);
		} else {
			zend_string *str = zval_get_string(description);
			zend_throw_exception(assertion_error_ce, ZSTR_VAL(str), E_ERROR);
			zend_string_release_ex(str, 0);
		}
	} else if (ASSERTG(warning)) {
		if (!description) {
			php_error_docref(NULL, E_WARNING, "Assertion failed");
		} else {
			zend_string *str = zval_get_string(description);
			php_error_docref(NULL, E_WARNING, "%s failed", ZSTR_VAL(str));
			zend_string_release_ex(str, 0);
		}
	}

	if (ASSERTG(bail)) {
		zend_bailout();
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ Set/get the various assert flags */
PHP_FUNCTION(assert_options)
{
	zval *value = NULL;
	zend_long what;
	zend_bool oldint;
	int ac = ZEND_NUM_ARGS();
	zend_string *key;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(what)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	switch (what) {
	case ASSERT_ACTIVE:
		oldint = ASSERTG(active);
		if (ac == 2) {
			zend_string *value_str = zval_try_get_string(value);
			if (UNEXPECTED(!value_str)) {
				return;
			}

			key = zend_string_init("assert.active", sizeof("assert.active")-1, 0);
			zend_alter_ini_entry_ex(key, value_str, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0);
			zend_string_release_ex(key, 0);
			zend_string_release_ex(value_str, 0);
		}
		RETURN_LONG(oldint);
		break;

	case ASSERT_BAIL:
		oldint = ASSERTG(bail);
		if (ac == 2) {
			zend_string *value_str = zval_try_get_string(value);
			if (UNEXPECTED(!value_str)) {
				return;
			}

			key = zend_string_init("assert.bail", sizeof("assert.bail")-1, 0);
			zend_alter_ini_entry_ex(key, value_str, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0);
			zend_string_release_ex(key, 0);
			zend_string_release_ex(value_str, 0);
		}
		RETURN_LONG(oldint);
		break;

	case ASSERT_WARNING:
		oldint = ASSERTG(warning);
		if (ac == 2) {
			zend_string *value_str = zval_try_get_string(value);
			if (UNEXPECTED(!value_str)) {
				return;
			}

			key = zend_string_init("assert.warning", sizeof("assert.warning")-1, 0);
			zend_alter_ini_entry_ex(key, value_str, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0);
			zend_string_release_ex(key, 0);
			zend_string_release_ex(value_str, 0);
		}
		RETURN_LONG(oldint);
		break;

	case ASSERT_CALLBACK:
		if (Z_TYPE(ASSERTG(callback)) != IS_UNDEF) {
			ZVAL_COPY(return_value, &ASSERTG(callback));
		} else if (ASSERTG(cb)) {
			RETVAL_STRING(ASSERTG(cb));
		} else {
			RETVAL_NULL();
		}
		if (ac == 2) {
			zval_ptr_dtor(&ASSERTG(callback));
			ZVAL_COPY(&ASSERTG(callback), value);
		}
		return;

	case ASSERT_EXCEPTION:
		oldint = ASSERTG(exception);
		if (ac == 2) {
			zend_string *val = zval_try_get_string(value);
			if (UNEXPECTED(!val)) {
				return;
			}

			key = zend_string_init("assert.exception", sizeof("assert.exception")-1, 0);
			zend_alter_ini_entry_ex(key, val, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0);
			zend_string_release_ex(val, 0);
			zend_string_release_ex(key, 0);
		}
		RETURN_LONG(oldint);
		break;

	default:
		zend_argument_value_error(1, "must have a valid value");
		break;
	}

	return;
}
/* }}} */
