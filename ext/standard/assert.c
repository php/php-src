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
	bool active;
	bool bail;
	bool warning;
	bool exception;
ZEND_END_MODULE_GLOBALS(assert)

ZEND_DECLARE_MODULE_GLOBALS(assert)

#define ASSERTG(v) ZEND_MODULE_GLOBALS_ACCESSOR(assert, v)

#define SAFE_STRING(s) ((s)?(s):"")

enum {
	ASSERT_ACTIVE=1,
	ASSERT_CALLBACK,
	ASSERT_BAIL,
	ASSERT_WARNING,
	ASSERT_EXCEPTION
};

PHPAPI zend_class_entry *assertion_error_ce;

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
	 STD_PHP_INI_BOOLEAN("assert.exception",	"1",	PHP_INI_ALL,	OnUpdateBool,		exception, 			zend_assert_globals,		assert_globals)
PHP_INI_END()

static void php_assert_init_globals(zend_assert_globals *assert_globals_p) /* {{{ */
{
	ZVAL_UNDEF(&assert_globals_p->callback);
	assert_globals_p->cb = NULL;
}
/* }}} */

PHP_MINIT_FUNCTION(assert) /* {{{ */
{
	ZEND_INIT_MODULE_GLOBALS(assert, php_assert_init_globals, NULL);

	REGISTER_INI_ENTRIES();

	REGISTER_LONG_CONSTANT("ASSERT_ACTIVE", ASSERT_ACTIVE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ASSERT_CALLBACK", ASSERT_CALLBACK, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ASSERT_BAIL", ASSERT_BAIL, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ASSERT_WARNING", ASSERT_WARNING, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ASSERT_EXCEPTION", ASSERT_EXCEPTION, CONST_CS|CONST_PERSISTENT);

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
	zend_string *description_str = NULL;
	zend_object *description_obj = NULL;

	if (!ASSERTG(active)) {
		RETURN_TRUE;
	}

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(assertion)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OF_CLASS_OR_STR_OR_NULL(description_obj, zend_ce_throwable, description_str)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_is_true(assertion)) {
		RETURN_TRUE;
	}

	if (description_obj) {
		GC_ADDREF(description_obj);
		zend_throw_exception_internal(description_obj);
		RETURN_THROWS();
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

		if (description_str) {
			ZVAL_STR(&args[3], description_str);
			call_user_function(NULL, NULL, &ASSERTG(callback), &retval, 4, args);
		} else {
			call_user_function(NULL, NULL, &ASSERTG(callback), &retval, 3, args);
		}

		zval_ptr_dtor(&args[0]);
		zval_ptr_dtor(&retval);
	}

	if (ASSERTG(exception)) {
		zend_throw_exception(assertion_error_ce, description_str ? ZSTR_VAL(description_str) : NULL, E_ERROR);
		if (ASSERTG(bail)) {
			/* When bail is turned on, the exception will not be caught. */
			zend_exception_error(EG(exception), E_ERROR);
		}
	} else if (ASSERTG(warning)) {
		php_error_docref(NULL, E_WARNING, "%s failed", description_str ? ZSTR_VAL(description_str) : "Assertion failed");
	}

	if (ASSERTG(bail)) {
		zend_throw_unwind_exit();
		RETURN_THROWS();
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Set/get the various assert flags */
PHP_FUNCTION(assert_options)
{
	zval *value = NULL;
	zend_long what;
	bool oldint;
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
				RETURN_THROWS();
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
				RETURN_THROWS();
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
				RETURN_THROWS();
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
			if (Z_TYPE_P(value) == IS_NULL) {
				ZVAL_UNDEF(&ASSERTG(callback));
			} else {
				ZVAL_COPY(&ASSERTG(callback), value);
			}
		}
		return;

	case ASSERT_EXCEPTION:
		oldint = ASSERTG(exception);
		if (ac == 2) {
			zend_string *val = zval_try_get_string(value);
			if (UNEXPECTED(!val)) {
				RETURN_THROWS();
			}

			key = zend_string_init("assert.exception", sizeof("assert.exception")-1, 0);
			zend_alter_ini_entry_ex(key, val, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0);
			zend_string_release_ex(val, 0);
			zend_string_release_ex(key, 0);
		}
		RETURN_LONG(oldint);
		break;

	default:
		zend_argument_value_error(1, "must be an ASSERT_* constant");
		RETURN_THROWS();
	}
}
/* }}} */
