/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Thies C. Arntzen (thies@thieso.net)                          |
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
	char *default_callback;
	char *callback;
} php_assert_globals;

#ifdef ZTS
#define ASSERTLS_D php_assert_globals *assert_globals
#define ASSERTLS_DC , ASSERTLS_D
#define ASSERTLS_C assert_globals
#define ASSERTLS_CC , ASSERTLS_CC
#define ASSERT(v) (assert_globals->v)
#define ASSERTLS_FETCH() php_assert_globals *assert_globals = ts_resource(assert_globals_id)
int assert_globals_id;
#else
#define ASSERTLS_D
#define ASSERTLS_DC
#define ASSERTLS_C
#define ASSERTLS_CC
#define ASSERT(v) (assert_globals.v)
#define ASSERTLS_FETCH()
php_assert_globals assert_globals;
#endif

#define SAFE_STRING(s) ((s)?(s):"")

#define ASSERT_ACTIVE       1
#define ASSERT_CALLBACK     2
#define ASSERT_BAIL         3
#define ASSERT_WARNING      4
#define ASSERT_QUIET_EVAL   5

PHP_INI_BEGIN()
	 STD_PHP_INI_ENTRY("assert.active",	    "1",	PHP_INI_ALL,	OnUpdateInt,		active,	 			php_assert_globals,		assert_globals)
	 STD_PHP_INI_ENTRY("assert.bail",	    "0",	PHP_INI_ALL,	OnUpdateInt,		bail,	 			php_assert_globals,		assert_globals)
	 STD_PHP_INI_ENTRY("assert.warning",	"1",	PHP_INI_ALL,	OnUpdateInt,		warning, 			php_assert_globals,		assert_globals)
	 STD_PHP_INI_ENTRY("assert.callback",   NULL,	PHP_INI_ALL,	OnUpdateString,		default_callback, 	php_assert_globals,		assert_globals)
	 STD_PHP_INI_ENTRY("assert.quiet_eval", "0",	PHP_INI_ALL,	OnUpdateInt,		quiet_eval,		 	php_assert_globals,		assert_globals)
PHP_INI_END()

static void php_assert_init_globals(ASSERTLS_D)
{
	ASSERT(callback) = 0;
}

PHP_MINIT_FUNCTION(assert)
{

#ifdef ZTS
	assert_globals_id = ts_allocate_id(sizeof(php_assert_globals), (ts_allocate_ctor) php_assert_init_globals, NULL);
#else
	php_assert_init_globals(ASSERTLS_C);
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
	return SUCCESS;
}

PHP_RINIT_FUNCTION(assert)
{
	ASSERTLS_FETCH();

	if (ASSERT(callback)) { 
		efree(ASSERT(callback));
		ASSERT(callback) = NULL;
	}

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(assert)
{
	ASSERTLS_FETCH();

	if (ASSERT(callback)) { 
		efree(ASSERT(callback));
		ASSERT(callback) = NULL;
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
	pval **assertion;	
	int val;
	char *myeval = NULL;
	char *cbfunc;
	char *compiled_string_description;
	CLS_FETCH();
	ASSERTLS_FETCH();
	
	if (! ASSERT(active)) {
		RETURN_TRUE;
	}

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &assertion) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if ((*assertion)->type == IS_STRING) {
		zval retval;
		int old_error_reporting = 0; /* shut up gcc! */

		myeval = (*assertion)->value.str.val;

		if (ASSERT(quiet_eval)) {
			old_error_reporting = EG(error_reporting);
			EG(error_reporting) = 0;
		}

		compiled_string_description = zend_make_compiled_string_description("assert code");
		if (zend_eval_string(myeval, &retval, compiled_string_description CLS_CC ELS_CC) == FAILURE) {
			efree(compiled_string_description);
			zend_error(E_ERROR, "Failure evaluating code:\n%s\n", myeval);
			/* zend_error() does not return in this case. */
		}
		efree(compiled_string_description);

		if (ASSERT(quiet_eval)) {
			EG(error_reporting) = old_error_reporting;
		}

		convert_to_boolean(&retval);
		val = retval.value.lval;
	} else {
		convert_to_boolean_ex(assertion);
		val = (*assertion)->value.lval;
	}

	if (val) {
		RETURN_TRUE;
	}

	if (ASSERT(callback)) {
		cbfunc = ASSERT(callback);
	} else if (ASSERT(default_callback)) {
		cbfunc = ASSERT(default_callback);
	} else {
		cbfunc = NULL;
	}

	if (cbfunc) {
		zval *args[5];
		zval *retval;
		int i;
		uint lineno = zend_get_executed_lineno(ELS_C);
		char *filename = zend_get_executed_filename(ELS_C);
		/*
		char *function = get_active_function_name();
		*/

		MAKE_STD_ZVAL(args[0]);
		MAKE_STD_ZVAL(args[1]);
		MAKE_STD_ZVAL(args[2]);
		MAKE_STD_ZVAL(args[3]);
		/*
		MAKE_STD_ZVAL(args[4]);
		*/

		args[0]->type = IS_STRING; args[0]->value.str.val = estrdup(SAFE_STRING(cbfunc)); 	args[0]->value.str.len = strlen(args[0]->value.str.val);
		args[1]->type = IS_STRING; args[1]->value.str.val = estrdup(SAFE_STRING(filename)); args[1]->value.str.len = strlen(args[1]->value.str.val);
		args[2]->type = IS_LONG;   args[2]->value.lval    = lineno;      
		args[3]->type = IS_STRING; args[3]->value.str.val = estrdup(SAFE_STRING(myeval));   args[3]->value.str.len = strlen(args[3]->value.str.val);
		/*
		  this is always "assert" so it's useless
		  args[4]->type = IS_STRING; args[4]->value.str.val = estrdup(SAFE_STRING(function));         args[4]->value.str.len = strlen(args[4]->value.str.val);
		*/
		
		MAKE_STD_ZVAL(retval);
		retval->type = IS_BOOL;
		retval->value.lval = 0;

		/* XXX do we want to check for error here? */
		call_user_function(CG(function_table), NULL, args[0], retval, 3, args+1);

		for (i = 0; i < 4; i++) {
			zval_ptr_dtor(&(args[i]));
		}
		zval_ptr_dtor(&retval);
	}

	if (ASSERT(warning)) {
		if (myeval) {
			php_error(E_WARNING,"Assertion \"%s\" failed",myeval);
		} else {
			php_error(E_WARNING,"Assertion failed");
		}
	}

	if (ASSERT(bail)) {
		zend_bailout();
	}
}

/* }}} */
/* {{{ proto mixed assert_options(int what [, mixed value])
   Set/get the various assert flags */

PHP_FUNCTION(assert_options)
{
	pval **what,**value;
	int oldint;
	char *oldstr;
	int ac = ZEND_NUM_ARGS();
	ASSERTLS_FETCH();
	
	if (ac < 1 || ac > 2 || zend_get_parameters_ex(ac, &what, &value) == FAILURE) {
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

	case ASSERT_BAIL:
		oldint = ASSERT(bail);
		if (ac == 2) {
			convert_to_long_ex(value);
			ASSERT(bail) = (*value)->value.lval;
		}
		RETURN_LONG(oldint);
		break;

	case ASSERT_QUIET_EVAL:
		oldint = ASSERT(quiet_eval);
		if (ac == 2) {
			convert_to_long_ex(value);
			ASSERT(quiet_eval) = (*value)->value.lval;
		}
		RETURN_LONG(oldint);
		break;

	case ASSERT_WARNING:
		oldint = ASSERT(warning);
		if (ac == 2) {
			convert_to_long_ex(value);
			ASSERT(warning) = (*value)->value.lval;
		}
		RETURN_LONG(oldint);
		break;

	case ASSERT_CALLBACK:
		oldstr = ASSERT(callback);
		RETVAL_STRING(SAFE_STRING(oldstr),1);

		if (ac == 2) {
			if (oldstr) {
				efree(oldstr);
			} 
			convert_to_string_ex(value);
			ASSERT(callback) = estrndup((*value)->value.str.val,(*value)->value.str.len);
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
