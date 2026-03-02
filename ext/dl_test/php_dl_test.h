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

#ifndef PHP_DL_TEST_H
# define PHP_DL_TEST_H

extern zend_module_entry dl_test_module_entry;
# define phpext_dl_test_ptr &dl_test_module_entry

# define PHP_DL_TEST_VERSION PHP_VERSION

# if defined(ZTS) && defined(COMPILE_DL_DL_TEST)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

ZEND_BEGIN_MODULE_GLOBALS(dl_test)
	zend_long long_value;
	char *string_value;
ZEND_END_MODULE_GLOBALS(dl_test);

#define DT_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(dl_test, v)

#endif	/* PHP_DL_TEST_H */
