/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Go Kudo <zeriyoshi@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "php_config.h"
# undef HAVE_CONFIG_H
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sapi/embed/php_embed.h"
#include "zend_static_cache_internal.h"

#define TEST_KEY "retired_shared_graph_free_payload"

static const char opcache_test_ini[] =
	"html_errors=0\n"
	"implicit_flush=1\n"
	"output_buffering=0\n"
	"max_execution_time=0\n"
	"max_input_time=-1\n"
	"opcache.enable=1\n"
	"opcache.enable_cli=1\n"
	"opcache.memory_consumption=64\n"
	"opcache.max_accelerated_files=200\n"
	"opcache.static_cache.volatile_size_mb=32\n\0";

static const char seed_code[] =
	"(static function (): bool {"
	"    OPcache\\volatile_clear();"
	"    $payload = ['name' => 'retired-shared-graph-free', 'items' => []];"
	"    for ($i = 0; $i < 128; $i++) {"
	"        $payload['items'][] = ['id' => $i, 'path' => '/items/' . $i, 'flags' => [true, false, true]];"
	"    }"
	"    return OPcache\\volatile_store('" TEST_KEY "', $payload);"
	"})()";

static int zend_opcache_test_startup(int argc, char **argv)
{
#ifdef ZTS
	if (!php_tsrm_startup_ex(2)) {
		return FAILURE;
	}
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	zend_signal_startup();
	sapi_startup(&php_embed_module);
	php_embed_module.ini_entries = opcache_test_ini;
	if (argv != NULL) {
		php_embed_module.executable_location = argv[0];
	}

	if (php_embed_module.startup(&php_embed_module) == FAILURE) {
		sapi_shutdown();
#ifdef ZTS
		tsrm_shutdown();
#endif
		return FAILURE;
	}

	SG(options) |= SAPI_OPTION_NO_CHDIR;

	return SUCCESS;
}

static void zend_opcache_test_shutdown(void)
{
	php_module_shutdown();
	sapi_shutdown();
#ifdef ZTS
	tsrm_shutdown();
#endif
}

static void fail_with_message(const char *message)
{
	fprintf(stderr, "%s\n", message);
	exit(1);
}

static bool zend_opcache_test_request_startup(void)
{
#ifdef ZTS
	(void) ts_resource(0);
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	SG(request_info).argc = 0;
	SG(request_info).argv = NULL;
	SG(request_info).query_string = NULL;
	SG(request_info).request_uri = "-";
	SG(request_info).path_translated = NULL;
	SG(request_info).request_method = "GET";
	SG(request_info).no_headers = 1;

	if (php_request_startup() == FAILURE) {
		return false;
	}

	SG(headers_sent) = 1;
	SG(request_info).no_headers = 1;
	php_register_variable("PHP_SELF", "-", NULL);

	return true;
}

static bool zend_opcache_test_eval_bool(const char *code, const char *label)
{
	zval retval;
	bool result = false;

	ZVAL_UNDEF(&retval);
	if (zend_eval_stringl_ex(code, strlen(code), &retval, label, true) == SUCCESS && !EG(exception)) {
		result = zend_is_true(&retval);
	}

	if (EG(exception)) {
		zend_clear_exception();
	}
	if (!Z_ISUNDEF(retval)) {
		zval_ptr_dtor(&retval);
	}

	return result;
}

static bool zend_opcache_test_run_request_bool(const char *code, const char *label)
{
	bool result;

	if (!zend_opcache_test_request_startup()) {
		return false;
	}

	result = zend_opcache_test_eval_bool(code, label);
	php_request_shutdown(NULL);

	return result;
}

static bool zend_opcache_test_locate_payload(uint32_t *payload_offset)
{
	zend_opcache_static_cache_context *previous_context;
	zend_opcache_static_cache_header *header;
	zend_opcache_static_cache_entry *entries;
	zend_string *key;
	zend_ulong hash;
	uint32_t index;
	bool found = false;

	key = zend_string_init(TEST_KEY, sizeof(TEST_KEY) - 1, false);
	hash = zend_string_hash_val(key);
	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_volatile_context_state);

	if (zend_opcache_static_cache_rlock()) {
		if (zend_opcache_static_cache_header_is_initialized_locked()) {
			header = zend_opcache_static_cache_header_ptr();
			entries = zend_opcache_static_cache_entries(header);
			for (index = 0; index < header->capacity; index++) {
				if (zend_opcache_static_cache_key_equals(&entries[index], key, hash)) {
					if (entries[index].value_type == ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH) {
						*payload_offset = entries[index].value_offset;
						found = true;
					}
					break;
				}
			}
		}
		zend_opcache_static_cache_unlock();
	}

	zend_opcache_static_cache_restore_context(previous_context);
	zend_string_release(key);

	return found;
}

static bool zend_opcache_test_payload_region_is_free_locked(uint32_t payload_offset, bool *is_free)
{
	zend_opcache_static_cache_header *header;
	zend_opcache_static_cache_block *block;
	uint32_t block_offset, free_offset;

	if (payload_offset < sizeof(zend_opcache_static_cache_block)) {
		return false;
	}

	header = zend_opcache_static_cache_header_ptr();
	if (header == NULL) {
		return false;
	}

	block_offset = payload_offset - (uint32_t) sizeof(zend_opcache_static_cache_block);
	*is_free = false;
	if (block_offset >= header->data_offset + header->next_free) {
		*is_free = true;

		return true;
	}

	free_offset = header->free_list;
	while (free_offset != 0) {
		block = zend_opcache_static_cache_block_ptr(free_offset);
		if (zend_opcache_static_cache_block_is_free(block) &&
			block_offset >= free_offset &&
			block_offset < free_offset + block->size
		) {
			*is_free = true;
			break;
		}
		free_offset = block->next_free;
	}

	return true;
}

static bool zend_opcache_test_payload_region_is_free(uint32_t payload_offset, bool *is_free)
{
	zend_opcache_static_cache_context *previous_context;
	bool result = false;

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_volatile_context_state);
	if (zend_opcache_static_cache_rlock()) {
		if (zend_opcache_static_cache_header_is_initialized_locked()) {
			result = zend_opcache_test_payload_region_is_free_locked(payload_offset, is_free);
		}
		zend_opcache_static_cache_unlock();
	}
	zend_opcache_static_cache_restore_context(previous_context);

	return result;
}

static bool zend_opcache_test_queue_deferred_free(uint32_t payload_offset)
{
	zend_opcache_static_cache_context *previous_context;
	bool is_free = false;
	bool result = false;

	if (!zend_opcache_test_request_startup()) {
		return false;
	}

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_volatile_context_state);
	if (zend_opcache_static_cache_rlock()) {
		if (zend_opcache_static_cache_header_is_initialized_locked()) {
			zend_opcache_static_cache_defer_retired_shared_graph_free(payload_offset);
			result = zend_opcache_test_payload_region_is_free_locked(payload_offset, &is_free) && !is_free;
		}
		zend_opcache_static_cache_unlock();
	}
	zend_opcache_static_cache_restore_context(previous_context);

	if (result) {
		zend_opcache_static_cache_release_request_shared_graph_refs();
		result = zend_opcache_test_payload_region_is_free(payload_offset, &is_free) && is_free;
	}

	php_request_shutdown(NULL);

	return result;
}

int main(int argc, char **argv)
{
	uint32_t payload_offset = 0;
	bool is_free = true;

	if (zend_opcache_test_startup(argc, argv) == FAILURE) {
		fail_with_message("startup failed");
	}

	if (!zend_opcache_test_run_request_bool(seed_code, "seed shared graph")) {
		zend_opcache_test_shutdown();
		fail_with_message("failed to seed shared graph payload");
	}

	if (!zend_opcache_test_locate_payload(&payload_offset)) {
		zend_opcache_test_shutdown();
		fail_with_message("failed to locate shared graph payload");
	}

	if (!zend_opcache_test_payload_region_is_free(payload_offset, &is_free) || is_free) {
		zend_opcache_test_shutdown();
		fail_with_message("shared graph payload was unexpectedly free before deferred cleanup");
	}

	if (!zend_opcache_test_queue_deferred_free(payload_offset)) {
		zend_opcache_test_shutdown();
		fail_with_message("deferred cleanup did not free payload after the read lock was released");
	}

	if (!zend_opcache_test_payload_region_is_free(payload_offset, &is_free) || !is_free) {
		zend_opcache_test_shutdown();
		fail_with_message("deferred cleanup did not leave the payload block free");
	}

	zend_opcache_test_shutdown();
	puts("ok");

	return 0;
}
