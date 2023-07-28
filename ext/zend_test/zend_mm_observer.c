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

#include "php.h"
#include "php_test.h"
#include "Zend/zend_alloc.h"
#include "ext/standard/info.h"
#include "ext/standard/php_var.h"
#include "zend_mm_observer.h"

static void zend_mm_test_observer_malloc(size_t len, void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	size_t block_len = 0;
	if (is_zend_ptr(ptr)) {
		block_len = zend_mm_block_size(zend_mm_get_heap(), ptr);
	}
	printf("malloc %p of size %zu (block: %zu)\n", ptr, len, block_len);
}

static void zend_mm_test_observer_free(void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	size_t block_len = 0;
	if (is_zend_ptr(ptr)) {
		block_len = zend_mm_block_size(zend_mm_get_heap(), ptr);
	}
	printf("freed %p of size %zu\n", ptr, block_len);
}

static void zend_mm_test_observer_realloc(void *ptr, size_t len, void *newptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	size_t block_len = 0;
	if (is_zend_ptr(ptr)) {
		block_len = zend_mm_block_size(zend_mm_get_heap(), ptr);
	}
	printf("realloc %p of size %zu (block: %zu, former %p)\n", newptr, len, block_len, ptr);
}

static PHP_INI_MH(OnUpdateZendTestMMObserverEnabled)
{
	if (new_value == NULL) {
		return FAILURE;
	}

	int int_value = zend_ini_parse_bool(new_value);

	if (int_value == 1) {
		if (ZT_G(observer) == NULL) {
			ZT_G(observer) = zend_mm_observer_register(zend_mm_get_heap(), zend_mm_test_observer_malloc, zend_mm_test_observer_free, zend_mm_test_observer_realloc);
		}
	} else {
		if (ZT_G(observer) != NULL) {
			zend_mm_observer_unregister(zend_mm_get_heap(), ZT_G(observer));
			ZT_G(observer) = NULL;
		}
	}
	return OnUpdateBool(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}

PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("zend_test.zend_mm_observer.enabled", "0", PHP_INI_ALL, OnUpdateZendTestMMObserverEnabled, zend_mm_observer_enabled, zend_zend_test_globals, zend_test_globals)
PHP_INI_END()

void zend_test_mm_observer_minit(INIT_FUNC_ARGS)
{
	if (type != MODULE_TEMPORARY) {
		REGISTER_INI_ENTRIES();
	} else {
		(void)ini_entries;
	}
}

void zend_test_mm_observer_rinit(void)
{
	if (ZT_G(zend_mm_observer_enabled)) {
		printf("ZendMM Observer enabled\n");
		ZT_G(observer) = zend_mm_observer_register(zend_mm_get_heap(), zend_mm_test_observer_malloc, zend_mm_test_observer_free, zend_mm_test_observer_realloc);
	}
}

void zend_test_mm_observer_rshutdown(void)
{
	if (ZT_G(observer)) {
		printf("ZendMM Observer disabled\n");
		zend_mm_observer_unregister(zend_mm_get_heap(), ZT_G(observer));
	}
	ZT_G(observer) = NULL;
}

