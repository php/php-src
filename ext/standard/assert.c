/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Thies C. Arntzen (thies@digicol.de)                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* {{{ includes/startup/misc */

#include "php.h"
#include "php_assert.h"

typedef struct {
	int active;
	int exit;
	char *callback;
} php_assert_globals;

#ifdef ZTS
#define ASSERT(v) (assert_globals->v)
#define ASSERTLS_FETCH() php_assert_globals *assert_globals = ts_resource(assert_globals_id)
int assert_globals_id;
#else
#define ASSERT(v) (assert_globals.v)
#define ASSERTLS_FETCH()
php_assert_globals assert_globals;
#endif

PHP_MINIT_FUNCTION(assert);
PHP_MSHUTDOWN_FUNCTION(assert);
PHP_RINIT_FUNCTION(assert);
PHP_RSHUTDOWN_FUNCTION(assert);
PHP_MINFO_FUNCTION(assert);

PHP_FUNCTION(assert);
PHP_FUNCTION(assert_options);

static zend_function_entry php_assert_functions[] = {
	PHP_FE(assert,          NULL)
	PHP_FE(assert_options,	NULL)
	{NULL, NULL, NULL}
};


zend_module_entry assert_module_entry = {
	"Assertion", 
	php_assert_functions, 
	PHP_MINIT(assert),
	PHP_MSHUTDOWN(assert),
	PHP_RINIT(assert),
	PHP_RSHUTDOWN(assert),
	PHP_MINFO(assert),
    STANDARD_MODULE_PROPERTIES
};

#define ASSERT_ACTIVE       1
#define ASSERT_CALLBACK     2
#define ASSERT_EXIT         3

#ifdef ZTS
static void php_assert_init_globals(php_assert_globals *assert_globals)
{
	ASSERT(active)   = 0;
	ASSERT(exit)     = 0;
	ASSERT(callback) = 0;
}
#endif

PHP_MINIT_FUNCTION(assert)
{

#ifdef ZTS
	assert_globals_id = ts_allocate_id(sizeof(php_assert_globals), (ts_allocate_ctor) php_assert_init_globals, NULL);
#else
	ASSERT(active)   = 0;
	ASSERT(exit)     = 0;
	ASSERT(callback) = 0;
#endif

	REGISTER_LONG_CONSTANT("ASSERT_ACTIVE", ASSERT_ACTIVE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ASSERT_CALLBACK", ASSERT_CALLBACK, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ASSERT_EXIT", ASSERT_EXIT, CONST_CS|CONST_PERSISTENT);

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(assert)
{
	return SUCCESS;
}

PHP_RINIT_FUNCTION(assert)
{
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(assert)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(assert)
{
}

/* }}} */
/* {{{ internal functions */
/* }}} */
/* {{{ proto int assert(string|int assertion)
   checks if assertion is false */

PHP_FUNCTION(assert)
{
	pval **assertion;	
	int val;
	char *myeval = empty_string;
	ASSERTLS_FETCH();
	
	if (! ASSERT(active)) {
		RETURN_TRUE;
	}

	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &assertion) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if ((*assertion)->type == IS_STRING) {
		zval retval;
		CLS_FETCH();
		ELS_FETCH();

		myeval = (*assertion)->value.str.val;
		zend_eval_string(myeval, &retval CLS_CC ELS_CC);
		convert_to_boolean(&retval);
		val = retval.value.lval;
	} else {
		convert_to_boolean_ex(assertion);
		val = (*assertion)->value.lval;
	}

	if (val) {
		RETURN_TRUE;
	}
	php_error(E_WARNING,"Assertion \"%s\" failed",myeval);

	if (ASSERT(exit)) {
		zend_bailout();
	}
}

/* }}} */
/* {{{ proto mixed assert_options(int what,mixed value)
   set/get the various assert flags. */

PHP_FUNCTION(assert_options)
{
	pval **what,**value;
	int oldint;
	char *oldstr;
	int ac = ARG_COUNT(ht);
	ASSERTLS_FETCH();
	
	if (ac < 1 || ac > 2 || getParametersEx(ac, &what, &value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(what);

	switch ((*what)->value.lval) {
	case ASSERT_ACTIVE:
		oldint = ASSERT(active);
		if (ac == 2) {
			convert_to_long_ex(value);
			ASSERT(active) = (*value)->value.lval;
		}
		RETURN_LONG(oldint);
		break;

	case ASSERT_EXIT:
		oldint = ASSERT(exit);
		if (ac == 2) {
			convert_to_long_ex(value);
			ASSERT(exit) = (*value)->value.lval;
		}
		RETURN_LONG(oldint);
		break;

	case ASSERT_CALLBACK:
		oldstr = ASSERT(callback);
		RETVAL_STRING(oldstr,1);

		if (ac == 2) {
			convert_to_string_ex(value);
			ASSERT(callback) = estrndup((*value)->value.str.val,(*value)->value.str.len);
		}
		if (oldstr) {
			efree(oldstr);
		} 
		return;
		break;

	default:
		php_error(E_WARNING,"Unknown value %d.",(*what)->value.lval);
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
 */
