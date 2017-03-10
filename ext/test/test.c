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

PHP_MINIT_FUNCTION(test)
{
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
	PHP_FE_END
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

