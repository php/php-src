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

#ifndef PHP_TEST_H
#define PHP_TEST_H

#include "fiber.h"

extern zend_module_entry zend_test_module_entry;
#define phpext_zend_test_ptr &zend_test_module_entry

#define PHP_ZEND_TEST_VERSION "0.1.0"

#ifdef ZTS
#include "TSRM.h"
#endif

#if defined(ZTS) && defined(COMPILE_DL_ZEND_TEST)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

ZEND_BEGIN_MODULE_GLOBALS(zend_test)
	int observer_enabled;
	int observer_show_output;
	int observer_observe_all;
	int observer_observe_includes;
	int observer_observe_functions;
	int observer_observe_declaring;
	zend_array *observer_observe_function_names;
	int observer_show_return_type;
	int observer_show_return_value;
	int observer_show_init_backtrace;
	int observer_show_opcode;
	char *observer_show_opcode_in_user_handler;
	int observer_nesting_depth;
	int observer_fiber_init;
	int observer_fiber_switch;
	int observer_fiber_destroy;
	int observer_execute_internal;
	HashTable global_weakmap;
	int replace_zend_execute_ex;
	int register_passes;
	bool print_stderr_mshutdown;
	zend_long limit_copy_file_range;
	int observe_opline_in_zendmm;
	zend_mm_heap* zend_orig_heap;
	zend_mm_heap* zend_test_heap;
	zend_test_fiber *active_fiber;
	zend_long quantity_value;
	zend_string *str_test;
	zend_string *not_empty_str_test;
ZEND_END_MODULE_GLOBALS(zend_test)

extern ZEND_DECLARE_MODULE_GLOBALS(zend_test)

#define ZT_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(zend_test, v)

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

PHP_ZEND_TEST_API int ZEND_FASTCALL bug78270(const char *str, size_t str_len);

PHP_ZEND_TEST_API struct bug79096 bug79096(void);
PHP_ZEND_TEST_API void bug79532(off_t *array, size_t elems);

extern PHP_ZEND_TEST_API int *(*bug79177_cb)(void);
PHP_ZEND_TEST_API void bug79177(void);

#endif
