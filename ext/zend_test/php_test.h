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
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_TEST_H
#define PHP_TEST_H

extern zend_module_entry zend_test_module_entry;
#define phpext_zend_test_ptr &zend_test_module_entry

#define PHP_ZEND_TEST_VERSION "0.1.0"

#ifdef ZTS
#include "TSRM.h"
#endif

#if defined(ZTS) && defined(COMPILE_DL_ZEND_TEST)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

struct bug79096 {
	uint64_t a;
	uint64_t b;
};

#ifdef PHP_WIN32
#	define PHP_ZEND_TEST_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_ZEND_TEST_API __attribute__ ((visibility("default")))
#else
#	define PHP_ZEND_TEST_API
#endif

PHP_ZEND_TEST_API struct bug79096 bug79096(void);
PHP_ZEND_TEST_API void bug79532(off_t *array, size_t elems);

extern PHP_ZEND_TEST_API int *(*bug79177_cb)(void);
PHP_ZEND_TEST_API void bug79177(void);

#endif
