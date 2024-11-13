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
  | Author: Florian Engelhardt <florian@engelhardt.tc>                   |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_test.h"
#include "Zend/zend_alloc.h"
#include "zend_portability.h"
#include <stddef.h>
#include <stdio.h>

void* observe_malloc(size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	void *ptr = NULL;
	if (ZT_G(custom_malloc)) {
		zend_mm_set_heap(ZT_G(original_heap));
		ptr = ZT_G(custom_malloc)(size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		zend_mm_set_heap(ZT_G(observed_heap));
	} else {
		ptr = _zend_mm_alloc(ZT_G(original_heap), size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	}
	printf("Allocated %zu bytes at %p\n", size, ptr);
	return ptr;
}

void observe_free(void* ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	if (ZT_G(custom_free))
	{
		zend_mm_set_heap(ZT_G(original_heap));
		ZT_G(custom_free)(ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		zend_mm_set_heap(ZT_G(observed_heap));
	} else {
		_zend_mm_free(ZT_G(original_heap), ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	}
	printf("Freed memory at %p\n", ptr);
}

void* observe_realloc(void* ptr, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	void * new_ptr;
	if (ZT_G(custom_realloc))
	{
		zend_mm_set_heap(ZT_G(original_heap));
		new_ptr = ZT_G(custom_realloc)(ptr, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		zend_mm_set_heap(ZT_G(observed_heap));
	} else {
		new_ptr = _zend_mm_realloc(ZT_G(original_heap), ptr, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	}
	printf("Realloc of %zu bytes from %p to %p\n", size, ptr, new_ptr);
	return new_ptr;
}

size_t observe_gc(void)
{
	size_t size = 0;
	if (ZT_G(custom_gc)) {
		zend_mm_set_heap(ZT_G(original_heap));
		size = ZT_G(custom_gc)();
		zend_mm_set_heap(ZT_G(observed_heap));
	} else {
		size = zend_mm_gc(ZT_G(original_heap));
	}
	printf("ZendMM GC freed %zu bytes", size);
	return size;
}

void observe_shutdown(bool full, bool silent)
{
	if (ZT_G(custom_shutdown)) {
		zend_mm_set_heap(ZT_G(original_heap));
		ZT_G(custom_shutdown)(full, silent);
		zend_mm_set_heap(ZT_G(observed_heap));
	} else {
		zend_mm_shutdown(ZT_G(original_heap), full, silent);
	}
	printf("Shutdown happened: full -> %d, silent -> %d\n", full, silent);
}

void zend_test_mm_custom_handlers_init(void)
{
	if (ZT_G(observed_heap) != NULL) {
		return;
	}
	ZT_G(original_heap) = zend_mm_get_heap();
	if (zend_mm_is_custom_heap(ZT_G(original_heap))) {
		zend_mm_get_custom_handlers_ex(
			ZT_G(original_heap),
			&ZT_G(custom_malloc),
			&ZT_G(custom_free),
			&ZT_G(custom_realloc),
			&ZT_G(custom_gc),
			&ZT_G(custom_shutdown)
		);
	}
	printf("Prev handlers at %p, %p, %p, %p, %p\n", ZT_G(custom_malloc), ZT_G(custom_free), ZT_G(custom_realloc), ZT_G(custom_gc), ZT_G(custom_shutdown));
	ZT_G(observed_heap) = zend_mm_startup();
	zend_mm_set_custom_handlers_ex(
		ZT_G(observed_heap),
		observe_malloc,
		observe_free,
		observe_realloc,
		observe_gc,
		observe_shutdown
	);
	zend_mm_set_heap(ZT_G(observed_heap));
	printf("Heap at %p installed in ZendMM (orig at %p)\n", ZT_G(observed_heap), ZT_G(original_heap));
}

void zend_test_mm_custom_handlers_shutdown(void)
{
	if (ZT_G(observed_heap) == NULL) {
		return;
	}
	zend_mm_set_heap(ZT_G(original_heap));
	zend_mm_set_custom_handlers_ex(
		ZT_G(observed_heap),
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	);
	zend_mm_shutdown(ZT_G(observed_heap), true, true);
	ZT_G(observed_heap) = NULL;
	printf("Prev heap at %p restored in ZendMM\n", ZT_G(original_heap));
}

void zend_test_mm_custom_handlers_rshutdown(void)
{
	zend_test_mm_custom_handlers_shutdown();
}

void zend_test_mm_custom_handlers_rinit(void)
{
	if (ZT_G(zend_mm_custom_handlers_enabled)) {
		zend_test_mm_custom_handlers_init();
	}
}

static PHP_INI_MH(OnUpdateZendTestMMCustomHandlersEnabled)
{
	if (new_value == NULL) {
		return FAILURE;
	}

	int int_value = zend_ini_parse_bool(new_value);

	if (int_value == 1) {
		zend_test_mm_custom_handlers_init();
	} else {
		zend_test_mm_custom_handlers_shutdown();
	}
	return OnUpdateBool(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}

PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("zend_test.zend_mm_custom_handlers.enabled", "0", PHP_INI_USER, OnUpdateZendTestMMCustomHandlersEnabled, zend_mm_custom_handlers_enabled, zend_zend_test_globals, zend_test_globals)
PHP_INI_END()

void zend_test_mm_custom_handlers_minit(INIT_FUNC_ARGS)
{
	if (type != MODULE_TEMPORARY) {
		REGISTER_INI_ENTRIES();
	} else {
		(void)ini_entries;
	}
}
