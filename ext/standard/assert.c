/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Thies C. Arntzen <thies@thieso.net>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* {{{ includes/startup/misc */

#include "php.h"
#include "php_assert.h"
#include "php_ini.h"

typedef struct {
	long active;
	long bail;
	long warning;
	long quiet_eval;
	zval *callback;
} php_assert_globals;

#ifdef ZTS
#define ASSERTG(v) TSRMG(assert_globals_id, php_assert_globals *, v)
int assert_globals_id;
#else
#define ASSERTG(v) (assert_globals.v)
php_assert_globals assert_globals;
#endif

#define SAFE_STRING(s) ((s)?(s):"")

enum {
	ASSERT_ACTIVE=1,
	ASSERT_CALLBACK,
	ASSERT_BAIL,
	ASSERT_WARNING,
	ASSERT_QUIET_EVAL
};

static PHP_INI_MH(OnChangeCallback)
{
	if (ASSERTG(callback)) {
		zval_ptr_dtor(&ASSERTG(callback));
	}

	MAKE_STD_ZVAL(ASSERTG(callback));

	if (new_value) {
		ZVAL_STRINGL(ASSERTG(callback), new_value, new_value_length, 1);
	} else {
		ZVAL_EMPTY_STRING(ASSERTG(callback));
	}

	return SUCCESS;
}

PHP_INI_BEGIN()
	 STD_PHP_INI_ENTRY("assert.active",	    "1",	PHP_INI_ALL,	OnUpdateInt,		active,	 			php_assert_globals,		assert_globals)
	 STD_PHP_INI_ENTRY("assert.bail",	    "0",	PHP_INI_ALL,	OnUpdateInt,		bail,	 			php_assert_globals,		assert_globals)
	 STD_PHP_INI_ENTRY("assert.warning",	"1",	PHP_INI_ALL,	OnUpdateInt,		warning, 			php_assert_globals,		assert_globals)
	 PHP_INI_ENTRY    ("assert.callback",   NULL,   PHP_INI_ALL,    OnChangeCallback)
	 STD_PHP_INI_ENTRY("assert.quiet_eval", "0",	PHP_INI_ALL,	OnUpdateInt,		quiet_eval,		 	php_assert_globals,		assert_globals)
PHP_INI_END()

static void php_assert_init_globals(php_assert_globals *assert_globals_p TSRMLS_DC)
{
	ASSERTG(callback) = NULL;
}

PHP_MINIT_FUNCTION(assert)
{
#ifdef ZTS
	ts_allocate_id(&assert_globals_id, sizeof(php_assert_globals), (ts_allocate_ctor) php_assert_init_globals, NULL);
#else
	php_assert_init_globals(&assert_globals TSRMLS_CC);
#endif

	REGISTER_INI_ENTRIES();

	REGISTER_LONG_CONSTANT("ASSERT_ACTIVE", ASSERT_ACTIVE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ASSERT_CALLBACK", ASSERT_CALLBACK, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ASSERT_BAIL", ASSERT_BAIL, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ASSERT_WARNING", ASSERT_WARNING, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ASSERT_QUIET_EVAL", ASSERT_QUIET_EVAL, CONST_CS|CONST_PERSISTENT);

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(assert)
{
	if (ASSERTG(callback)) {
		zval_ptr_dtor(&ASSERTG(callback));
	}
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(assert)
{
	if (ASSERTG(callback)) { 
		zval_ptr_dtor(&ASSERTG(callback));
		ASSERTG(callback) = NULL;
	}

	return SUCCESS;
}

PHP_MINFO_FUNCTION(assert)
{
	DISPLAY_INI_ENTRIES();
}

/* }}} */
/* {{{ internal functions */
/* }}} */
/* {{{ proto int assert(string|bool assertion)
   Checks if assertion is false */

PHP_FUNCTION(assert)
{
	zval **assertion;	
	int val;
	char *myeval = NULL;
	char *compiled_string_description;
	
	if (! ASSERTG(active)) {
		RETURN_TRUE;
	}

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &assertion) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(assertion) == IS_STRING) {
		zval retval;
		int old_error_reporting = 0; /* shut up gcc! */

		myeval = Z_STRVAL_PP(assertion);

		if (ASSERTG(quiet_eval)) {
			old_error_reporting = EG(error_reporting);
			EG(error_reporting) = 0;
		}

		compiled_string_description = zend_make_compiled_string_description("assert code" TSRMLS_CC);
		if (zend_eval_string(myeval, &retval, compiled_string_description TSRMLS_CC) == FAILURE) {
			efree(compiled_string_description);
			zend_error(E_ERROR, "Failure evaluating code:\n%s\n", myeval);
			/* zend_error() does not return in this case. */
		}
		efree(compiled_string_description);

		if (ASSERTG(quiet_eval)) {
			EG(error_reporting) = old_error_reporting;
		}

		convert_to_boolean(&retval);
		val = Z_LVAL(retval);
	} else {
		convert_to_boolean_ex(assertion);
		val = Z_LVAL_PP(assertion);
	}

	if (val) {
		RETURN_TRUE;
	}

	if (ASSERTG(callback)) {
		zval *args[3];
		zval *retval;
		int i;
		uint lineno = zend_get_executed_lineno(TSRMLS_C);
		char *filename = zend_get_executed_filename(TSRMLS_C);

		MAKE_STD_ZVAL(args[0]);
		MAKE_STD_ZVAL(args[1]);
		MAKE_STD_ZVAL(args[2]);

		ZVAL_STRING(args[0], SAFE_STRING(filename), 1);
		ZVAL_LONG (args[1], lineno);
		ZVAL_STRING(args[2], SAFE_STRING(myeval), 1);
		
		MAKE_STD_ZVAL(retval);
		ZVAL_FALSE(retval);

		/* XXX do we want to check for error here? */
		call_user_function(CG(function_table), NULL, ASSERTG(callback), retval, 3, args TSRMLS_CC);

		for (i = 0; i <= 2; i++) {
			zval_ptr_dtor(&(args[i]));
		}
		zval_ptr_dtor(&retval);
	}

	if (ASSERTG(warning)) {
		if (myeval) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Assertion \"%s\" failed", myeval);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Assertion failed");
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
	pval **what, **value;
	int oldint;
	int ac = ZEND_NUM_ARGS();
	
	if (ac < 1 || ac > 2 || zend_get_parameters_ex(ac, &what, &value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(what);

	switch (Z_LVAL_PP(what)) {
	case ASSERT_ACTIVE:
		oldint = ASSERTG(active);
		if (ac == 2) {
			convert_to_long_ex(value);
			ASSERTG(active) = Z_LVAL_PP(value);
		}
		RETURN_LONG(oldint);
		break;

	case ASSERT_BAIL:
		oldint = ASSERTG(bail);
		if (ac == 2) {
			convert_to_long_ex(value);
			ASSERTG(bail) = Z_LVAL_PP(value);
		}
		RETURN_LONG(oldint);
		break;

	case ASSERT_QUIET_EVAL:
		oldint = ASSERTG(quiet_eval);
		if (ac == 2) {
			convert_to_long_ex(value);
			ASSERTG(quiet_eval) = Z_LVAL_PP(value);
		}
		RETURN_LONG(oldint);
		break;

	case ASSERT_WARNING:
		oldint = ASSERTG(warning);
		if (ac == 2) {
			convert_to_long_ex(value);
			ASSERTG(warning) = Z_LVAL_PP(value);
		}
		RETURN_LONG(oldint);
		break;

	case ASSERT_CALLBACK:
		if (ac == 2) {
			if (ASSERTG(callback)) {
				zval_ptr_dtor(&ASSERTG(callback));
			}
			ASSERTG(callback) = *value;
			zval_add_ref(value);
		}
		RETURN_TRUE;
		break;

	default:
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown value %d", Z_LVAL_PP(what));
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
