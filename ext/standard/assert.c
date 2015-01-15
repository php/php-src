/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
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

/* $Id$ */

/* {{{ includes */
#include "php.h"
#include "php_assert.h"
#include "php_ini.h"
/* }}} */

ZEND_BEGIN_MODULE_GLOBALS(assert)
	zval callback;
	char *cb;
	zend_bool active;
	zend_bool bail;
	zend_bool warning;
	zend_bool quiet_eval;
ZEND_END_MODULE_GLOBALS(assert)

ZEND_DECLARE_MODULE_GLOBALS(assert)

#ifdef ZTS
#define ASSERTG(v) ZEND_TSRMG(assert_globals_id, zend_assert_globals *, v)
#else
#define ASSERTG(v) (assert_globals.v)
#endif

#define SAFE_STRING(s) ((s)?(s):"")

enum {
	ASSERT_ACTIVE=1,
	ASSERT_CALLBACK,
	ASSERT_BAIL,
	ASSERT_WARNING,
	ASSERT_QUIET_EVAL
};

static PHP_INI_MH(OnChangeCallback) /* {{{ */
{
	if (EG(current_execute_data)) {
		if (Z_TYPE(ASSERTG(callback)) != IS_UNDEF) {
			zval_ptr_dtor(&ASSERTG(callback));
			ZVAL_UNDEF(&ASSERTG(callback));
		}
		if (new_value && (Z_TYPE(ASSERTG(callback)) != IS_UNDEF || new_value->len)) {
			ZVAL_STR_COPY(&ASSERTG(callback), new_value);
		}
	} else {
		if (ASSERTG(cb)) {
			pefree(ASSERTG(cb), 1);
		}
		if (new_value && new_value->len) {
			ASSERTG(cb) = pemalloc(new_value->len + 1, 1);
			memcpy(ASSERTG(cb), new_value->val, new_value->len);
			ASSERTG(cb)[new_value->len] = '\0';
		} else {
			ASSERTG(cb) = NULL;
		}
	}
	return SUCCESS;
}
/* }}} */

PHP_INI_BEGIN()
	 STD_PHP_INI_ENTRY("assert.active",		"1",	PHP_INI_ALL,	OnUpdateBool,		active,	 			zend_assert_globals,		assert_globals)
	 STD_PHP_INI_ENTRY("assert.bail",		"0",	PHP_INI_ALL,	OnUpdateBool,		bail,	 			zend_assert_globals,		assert_globals)
	 STD_PHP_INI_ENTRY("assert.warning",	"1",	PHP_INI_ALL,	OnUpdateBool,		warning, 			zend_assert_globals,		assert_globals)
	 PHP_INI_ENTRY("assert.callback",		NULL,	PHP_INI_ALL,	OnChangeCallback)
	 STD_PHP_INI_ENTRY("assert.quiet_eval", "0",	PHP_INI_ALL,	OnUpdateBool,		quiet_eval,		 	zend_assert_globals,		assert_globals)
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
	REGISTER_LONG_CONSTANT("ASSERT_QUIET_EVAL", ASSERT_QUIET_EVAL, CONST_CS|CONST_PERSISTENT);

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

/* {{{ proto int assert(string|bool assertion[, string description])
   Checks if assertion is false */
PHP_FUNCTION(assert)
{
	zval *assertion;
	int val;
	size_t description_len = 0;
	char *myeval = NULL;
	char *compiled_string_description, *description = NULL;

	if (! ASSERTG(active)) {
		RETURN_TRUE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|s", &assertion, &description, &description_len) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(assertion) == IS_STRING) {
		zval retval;
		int old_error_reporting = 0; /* shut up gcc! */

		myeval = Z_STRVAL_P(assertion);

		if (ASSERTG(quiet_eval)) {
			old_error_reporting = EG(error_reporting);
			EG(error_reporting) = 0;
		}

		compiled_string_description = zend_make_compiled_string_description("assert code");
		if (zend_eval_stringl(myeval, Z_STRLEN_P(assertion), &retval, compiled_string_description) == FAILURE) {
			efree(compiled_string_description);
			if (description_len == 0) {
				php_error_docref(NULL, E_RECOVERABLE_ERROR, "Failure evaluating code: %s%s", PHP_EOL, myeval);
			} else {
				php_error_docref(NULL, E_RECOVERABLE_ERROR, "Failure evaluating code: %s%s:\"%s\"", PHP_EOL, description, myeval);
			}
			if (ASSERTG(bail)) {
				zend_bailout();
			}
			RETURN_FALSE;
		}
		efree(compiled_string_description);

		if (ASSERTG(quiet_eval)) {
			EG(error_reporting) = old_error_reporting;
		}

		convert_to_boolean(&retval);
		val = Z_TYPE(retval) == IS_TRUE;
	} else {
		convert_to_boolean_ex(assertion);
		val = Z_TYPE_P(assertion) == IS_TRUE;
	}

	if (val) {
		RETURN_TRUE;
	}

	if (Z_TYPE(ASSERTG(callback)) == IS_UNDEF && ASSERTG(cb)) {
		ZVAL_STRING(&ASSERTG(callback), ASSERTG(cb));
	}

	if (Z_TYPE(ASSERTG(callback)) != IS_UNDEF) {
		zval *args = safe_emalloc(description_len == 0 ? 3 : 4, sizeof(zval), 0);
		zval retval;
		int i;
		uint lineno = zend_get_executed_lineno();
		const char *filename = zend_get_executed_filename();

		ZVAL_STRING(&args[0], SAFE_STRING(filename));
		ZVAL_LONG (&args[1], lineno);
		ZVAL_STRING(&args[2], SAFE_STRING(myeval));

		ZVAL_FALSE(&retval);

		/* XXX do we want to check for error here? */
		if (description_len == 0) {
			call_user_function(CG(function_table), NULL, &ASSERTG(callback), &retval, 3, args);
			for (i = 0; i <= 2; i++) {
				zval_ptr_dtor(&(args[i]));
			}
		} else {
			ZVAL_STRINGL(&args[3], SAFE_STRING(description), description_len);

			call_user_function(CG(function_table), NULL, &ASSERTG(callback), &retval, 4, args);
			for (i = 0; i <= 3; i++) {
				zval_ptr_dtor(&(args[i]));
			}
		}

		efree(args);
		zval_ptr_dtor(&retval);
	}

	if (ASSERTG(warning)) {
		if (description_len == 0) {
			if (myeval) {
				php_error_docref(NULL, E_WARNING, "Assertion \"%s\" failed", myeval);
			} else {
				php_error_docref(NULL, E_WARNING, "Assertion failed");
			}
		} else {
			if (myeval) {
				php_error_docref(NULL, E_WARNING, "%s: \"%s\" failed", description, myeval);
			} else {
				php_error_docref(NULL, E_WARNING, "%s failed", description);
			}
		}
	}

	if (ASSERTG(bail)) {
		zend_bailout();
	}
}
/* }}} */

/* {{{ proto mixed assert_options(int what [, mixed value])
   Set/get the various assert flags */
PHP_FUNCTION(assert_options)
{
	zval *value = NULL;
	zend_long what;
	zend_bool oldint;
	int ac = ZEND_NUM_ARGS();
	zend_string *key;

	if (zend_parse_parameters(ac, "l|z", &what, &value) == FAILURE) {
		return;
	}

	switch (what) {
	case ASSERT_ACTIVE:
		oldint = ASSERTG(active);
		if (ac == 2) {
			zend_string *value_str = zval_get_string(value);
			key = zend_string_init("assert.active", sizeof("assert.active")-1, 0);
			zend_alter_ini_entry_ex(key, value_str, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0);
			zend_string_release(key);
			zend_string_release(value_str);
		}
		RETURN_LONG(oldint);
		break;

	case ASSERT_BAIL:
		oldint = ASSERTG(bail);
		if (ac == 2) {
			zend_string *value_str = zval_get_string(value);
			key = zend_string_init("assert.bail", sizeof("assert.bail")-1, 0);
			zend_alter_ini_entry_ex(key, value_str, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0);
			zend_string_release(key);
			zend_string_release(value_str);
		}
		RETURN_LONG(oldint);
		break;

	case ASSERT_QUIET_EVAL:
		oldint = ASSERTG(quiet_eval);
		if (ac == 2) {
			zend_string *value_str = zval_get_string(value);
			key = zend_string_init("assert.quiet_eval", sizeof("assert.quiet_eval")-1, 0);
			zend_alter_ini_entry_ex(key, value_str, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0);
			zend_string_release(key);
			zend_string_release(value_str);
		}
		RETURN_LONG(oldint);
		break;

	case ASSERT_WARNING:
		oldint = ASSERTG(warning);
		if (ac == 2) {
			zend_string *value_str = zval_get_string(value);
			key = zend_string_init("assert.warning", sizeof("assert.warning")-1, 0);
			zend_alter_ini_entry_ex(key, value_str, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0);
			zend_string_release(key);
			zend_string_release(value_str);
		}
		RETURN_LONG(oldint);
		break;

	case ASSERT_CALLBACK:
		if (Z_TYPE(ASSERTG(callback)) != IS_UNDEF) {
			RETVAL_ZVAL(&ASSERTG(callback), 1, 0);
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

	default:
		php_error_docref(NULL, E_WARNING, "Unknown value %pd", what);
		break;
	}

	RETURN_FALSE;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */

