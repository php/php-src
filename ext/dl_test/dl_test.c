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
  | Author: Arnaud Le Blanc <arnaud.lb@gmail.com>                        |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_dl_test.h"
#include "dl_test_arginfo.h"

ZEND_DECLARE_MODULE_GLOBALS(dl_test)

/* {{{ void dl_test_test1() */
PHP_FUNCTION(dl_test_test1)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_printf("The extension %s is loaded and working!\r\n", "dl_test");
}
/* }}} */

/* {{{ string dl_test_test2( [ string $var ] ) */
PHP_FUNCTION(dl_test_test2)
{
	char *var = "World";
	size_t var_len = sizeof("World") - 1;
	zend_string *retval;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(var, var_len)
	ZEND_PARSE_PARAMETERS_END();

	retval = strpprintf(0, "Hello %s", var);

	RETURN_STR(retval);
}
/* }}}*/

/* {{{ PHP_DL_TEST_USE_REGISTER_FUNCTIONS_DIRECTLY */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dl_test_use_register_functions_directly, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_FUNCTION(dl_test_use_register_functions_directly)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_STRING("OK");
}

static const zend_function_entry php_dl_test_use_register_functions_directly_functions[] = {
	ZEND_FENTRY(dl_test_use_register_functions_directly, ZEND_FN(dl_test_use_register_functions_directly), arginfo_dl_test_use_register_functions_directly, 0)
	ZEND_FE_END
};
/* }}} */

/* {{{ INI */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("dl_test.long",       "0", PHP_INI_ALL, OnUpdateLong,   long_value,   zend_dl_test_globals, dl_test_globals)
	STD_PHP_INI_ENTRY("dl_test.string", "hello", PHP_INI_ALL, OnUpdateString, string_value, zend_dl_test_globals, dl_test_globals)
PHP_INI_END()
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(dl_test)
{
	/* Test backwards compatibility */
	if (getenv("PHP_DL_TEST_USE_OLD_REGISTER_INI_ENTRIES")) {
		zend_register_ini_entries(ini_entries, module_number);
	} else {
		REGISTER_INI_ENTRIES();
	}

	if (getenv("PHP_DL_TEST_USE_REGISTER_FUNCTIONS_DIRECTLY")) {
		zend_register_functions(NULL, php_dl_test_use_register_functions_directly_functions, NULL, type);
	}

	if (getenv("PHP_DL_TEST_MODULE_DEBUG")) {
		fprintf(stderr, "DL TEST MINIT\n");
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
static PHP_MSHUTDOWN_FUNCTION(dl_test)
{
	/* Test backwards compatibility */
	if (getenv("PHP_DL_TEST_USE_OLD_REGISTER_INI_ENTRIES")) {
		zend_unregister_ini_entries(module_number);
	} else {
		UNREGISTER_INI_ENTRIES();
	}

	if (getenv("PHP_DL_TEST_MODULE_DEBUG")) {
		fprintf(stderr, "DL TEST MSHUTDOWN\n");
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(dl_test)
{
#if defined(ZTS) && defined(COMPILE_DL_DL_TEST)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	if (getenv("PHP_DL_TEST_MODULE_DEBUG")) {
		fprintf(stderr, "DL TEST RINIT\n");
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(dl_test)
{
	if (getenv("PHP_DL_TEST_MODULE_DEBUG")) {
		fprintf(stderr, "DL TEST RSHUTDOWN\n");
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(dl_test)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "dl_test support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(dl_test)
{
#if defined(COMPILE_DL_DL_TEST) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	memset(dl_test_globals, 0, sizeof(*dl_test_globals));
}
/* }}} */

/* {{{ dl_test_module_entry */
zend_module_entry dl_test_module_entry = {
	STANDARD_MODULE_HEADER,
	"dl_test",
	ext_functions,
	PHP_MINIT(dl_test),
	PHP_MSHUTDOWN(dl_test),
	PHP_RINIT(dl_test),
	PHP_RSHUTDOWN(dl_test),
	PHP_MINFO(dl_test),
	PHP_DL_TEST_VERSION,
	PHP_MODULE_GLOBALS(dl_test),
	PHP_GINIT(dl_test),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_DL_TEST
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(dl_test)
#endif
