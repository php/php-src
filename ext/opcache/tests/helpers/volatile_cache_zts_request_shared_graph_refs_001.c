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
#include <stdio.h>
#include <string.h>

#include "Zend/zend_atomic.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_execute.h"
#include "Zend/zend_portability.h"

#include "sapi/embed/php_embed.h"
#include "zend_static_cache_internal.h"

#ifndef ZTS
# error "This helper requires a ZTS build"
#endif

#define REQUEST_COUNT 4

typedef struct _zend_opcache_test_accel_directives {
	zend_long memory_consumption;
	zend_long static_cache_volatile_size_mb;
	zend_long max_accelerated_files;
	double max_wasted_percentage;
	char *user_blacklist_filename;
	zend_long force_restart_timeout;
	bool use_cwd;
	bool ignore_dups;
	bool validate_timestamps;
	bool revalidate_path;
	bool save_comments;
	bool record_warnings;
	bool protect_memory;
	bool file_override_enabled;
	bool enable_cli;
	bool validate_permission;
#ifndef ZEND_WIN32
	bool validate_root;
#endif
	zend_ulong revalidate_freq;
	zend_ulong file_update_protection;
	char *error_log;
#ifdef ZEND_WIN32
	char *mmap_base;
#endif
	char *memory_model;
	zend_long log_verbosity_level;
	zend_long optimization_level;
	zend_long opt_debug_level;
	zend_long max_file_size;
	zend_long interned_strings_buffer;
	char *restrict_api;
#ifndef ZEND_WIN32
	char *lockfile_path;
#endif
	char *file_cache;
	bool file_cache_read_only;
	bool file_cache_only;
	bool file_cache_consistency_checks;
#if ENABLE_FILE_CACHE_FALLBACK
	bool file_cache_fallback;
#endif
#ifdef HAVE_HUGE_CODE_PAGES
	bool huge_code_pages;
#endif
	char *preload;
#ifndef ZEND_WIN32
	char *preload_user;
#endif
#ifdef ZEND_WIN32
	char *cache_id;
#endif
} zend_opcache_test_accel_directives;

typedef struct _zend_opcache_test_globals {
	bool counted;
	bool enabled;
	bool locked;
	bool accelerator_enabled;
	bool pcre_reseted;
	zend_opcache_test_accel_directives accel_directives;
} zend_opcache_test_globals;

typedef struct _zend_opcache_ref_thread_ctx {
	int result;
	char message[256];
} zend_opcache_ref_thread_ctx;

extern size_t accel_globals_offset;

#define ZEND_OPCACHE_TEST_CG(v) ZEND_TSRMG_FAST(accel_globals_offset, zend_opcache_test_globals *, v)

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

static const char clear_code[] =
	"(static function (): bool {"
	"    OPcache\\volatile_clear();"
	"    return true;"
	"})()";

static const char worker_code[] =
	"(static function (): bool {"
	"    $payload = ['name' => 'compiled-route-table', 'routes' => [], 'generators' => []];"
	"    for ($i = 0; $i < 192; $i++) {"
	"        $path = '/catalog/books/' . $i;"
	"        $payload['routes'][$path] = ["
	"            'controller' => 'CatalogController::show',"
	"            'methods' => ['GET', 'POST'],"
	"            'flags' => [true, false, true],"
	"            'score' => $i,"
	"        ];"
	"        $payload['generators'][$path] = ["
	"            'pattern' => $path,"
	"            'variables' => ['id' => $i],"
	"        ];"
	"    }"
	"    for ($i = 0; $i < 16; $i++) {"
	"        if (!OPcache\\volatile_store('zts_request_shared_graph_refs_payload', $payload)) {"
	"            return false;"
	"        }"
	"    }"
	"    for ($i = 0; $i < 64; $i++) {"
	"        $fetched = OPcache\\volatile_fetch('zts_request_shared_graph_refs_payload');"
	"        if (!is_array($fetched)"
	"            || !isset($fetched['routes'], $fetched['generators'])"
	"            || count($fetched['routes']) !== 192) {"
	"            return false;"
	"        }"
	"    }"
	"    return true;"
	"})()";

static zend_opcache_test_accel_directives zend_opcache_thread_accel_directives;
static bool zend_opcache_thread_enabled;

static const zend_opcache_static_cache_shared_graph_header *zend_opcache_locate_shared_graph_header(uint32_t payload_offset)
{
	const unsigned char *buffer;
	size_t buffer_len;
	size_t padding;
	uintptr_t raw_address, aligned_address;
	const zend_opcache_static_cache_shared_graph_header *header;

	buffer_len = zend_opcache_static_cache_block_payload_capacity(payload_offset);
	if (buffer_len == 0) {
		return NULL;
	}

	buffer = (const unsigned char *) zend_opcache_static_cache_ptr(payload_offset);
	raw_address = (uintptr_t) buffer;
	aligned_address = (uintptr_t) ZEND_MM_ALIGNED_SIZE(raw_address);
	padding = (size_t) (aligned_address - raw_address);
	if (padding > buffer_len || buffer_len - padding < sizeof(*header)) {
		return NULL;
	}

	buffer += padding;
	header = (const zend_opcache_static_cache_shared_graph_header *) buffer;
	if (header->magic != ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_MAGIC ||
		header->version != ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VERSION) {
		return NULL;
	}

	return header;
}

static int zend_opcache_test_startup(int argc, char **argv)
{
	if (!php_tsrm_startup_ex(3)) {
		return FAILURE;
	}
	ZEND_TSRMLS_CACHE_UPDATE();

	zend_signal_startup();
	sapi_startup(&php_embed_module);
	php_embed_module.ini_entries = opcache_test_ini;
	if (argv != NULL) {
		php_embed_module.executable_location = argv[0];
	}

	if (php_embed_module.startup(&php_embed_module) == FAILURE) {
		sapi_shutdown();
		tsrm_shutdown();
		return FAILURE;
	}

	zend_opcache_thread_accel_directives = ZEND_OPCACHE_TEST_CG(accel_directives);
	zend_opcache_thread_enabled = ZEND_OPCACHE_TEST_CG(enabled);

	SG(options) |= SAPI_OPTION_NO_CHDIR;
	return SUCCESS;
}

static void zend_opcache_test_shutdown(void)
{
	php_module_shutdown();
	sapi_shutdown();
	tsrm_shutdown();
}

static bool zend_opcache_thread_request_startup(void)
{
	(void) ts_resource(0);
	ZEND_TSRMLS_CACHE_UPDATE();
	ZEND_OPCACHE_TEST_CG(accel_directives) = zend_opcache_thread_accel_directives;
	ZEND_OPCACHE_TEST_CG(enabled) = zend_opcache_thread_enabled;

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

static bool zend_opcache_thread_eval(const char *code, zval *retval, const char *label)
{
	ZVAL_UNDEF(retval);
	if (zend_eval_stringl_ex(code, strlen(code), retval, label, true) == FAILURE) {
		if (EG(exception)) {
			zend_clear_exception();
		}
		return false;
	}

	if (EG(exception)) {
		zend_clear_exception();
		if (!Z_ISUNDEF_P(retval)) {
			zval_ptr_dtor(retval);
			ZVAL_UNDEF(retval);
		}
		return false;
	}

	return true;
}

static bool zend_opcache_run_request_code(const char *code, const char *label, bool expect_single_shared_graph_ref, char *message, size_t message_size)
{
	zval retval;

	if (!zend_opcache_thread_request_startup()) {
		snprintf(message, message_size, "%s: request startup failed", label);
		return false;
	}

	if (!zend_opcache_thread_eval(code, &retval, label)) {
		snprintf(message, message_size, "%s: eval failed", label);
		php_request_shutdown(NULL);
		return false;
	}

	if (!zend_is_true(&retval)) {
		snprintf(message, message_size, "%s: returned false", label);
		if (!Z_ISUNDEF(retval)) {
			zval_ptr_dtor(&retval);
		}
		php_request_shutdown(NULL);
		return false;
	}

	if (expect_single_shared_graph_ref && zend_opcache_static_cache_shared_graph_ref_count != 1) {
		snprintf(
			message,
			message_size,
			"%s: expected one request shared graph ref, got %u",
			label,
			zend_opcache_static_cache_shared_graph_ref_count
		);
		if (!Z_ISUNDEF(retval)) {
			zval_ptr_dtor(&retval);
		}
		php_request_shutdown(NULL);
		return false;
	}

	if (!Z_ISUNDEF(retval)) {
		zval_ptr_dtor(&retval);
	}
	php_request_shutdown(NULL);
	return true;
}

static bool zend_opcache_inspect_current_payload_state(
		uint32_t *value_offset_out,
		uint32_t *next_free_out,
		int *refcount_out,
		char *message,
		size_t message_size)
{
	static const char key_name[] = "zts_request_shared_graph_refs_payload";
	zend_opcache_static_cache_context *previous_context;
	zend_opcache_static_cache_header *header;
	zend_opcache_static_cache_entry *entries, *entry = NULL;
	const zend_opcache_static_cache_shared_graph_header *graph_header;
	zend_string *key;
	zend_ulong hash;
	uint32_t index;
	int state;
	bool ok = false;

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_volatile_context_state);
	key = zend_string_init(key_name, sizeof(key_name) - 1, 0);
	hash = zend_string_hash_val(key);

	if (!zend_opcache_static_cache_rlock()) {
		snprintf(message, message_size, "unable to acquire volatile cache read lock");
		goto done;
	}

	if (!zend_opcache_static_cache_header_is_initialized_locked()) {
		snprintf(message, message_size, "volatile cache header is not initialized");
		zend_opcache_static_cache_unlock();
		goto done;
	}

	header = zend_opcache_static_cache_header_ptr();
	entries = zend_opcache_static_cache_entries(header);
	for (index = 0; index < header->capacity; index++) {
		if (zend_opcache_static_cache_key_equals(&entries[index], key, hash)) {
			entry = &entries[index];
			break;
		}
	}

	if (entry == NULL) {
		snprintf(message, message_size, "volatile cache entry was not found after request shutdown");
		zend_opcache_static_cache_unlock();
		goto done;
	}

	if (entry->value_type != ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH) {
		snprintf(message, message_size, "unexpected cached value type %u", (unsigned int) entry->value_type);
		zend_opcache_static_cache_unlock();
		goto done;
	}

	graph_header = zend_opcache_locate_shared_graph_header(entry->value_offset);
	if (graph_header == NULL) {
		snprintf(message, message_size, "shared graph payload header is invalid");
		zend_opcache_static_cache_unlock();
		goto done;
	}

	state = zend_atomic_int_load_ex(&graph_header->ref_state);
	*value_offset_out = entry->value_offset;
	*next_free_out = header->next_free;
	*refcount_out = state & ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_REFCOUNT_MASK;

	zend_opcache_static_cache_unlock();
	ok = true;

done:
	zend_string_release(key);
	zend_opcache_static_cache_restore_context(previous_context);
	return ok;
}

static void *zend_opcache_ref_thread_main(void *arg)
{
	zend_opcache_ref_thread_ctx *ctx = (zend_opcache_ref_thread_ctx *) arg;
	uint32_t expected_offset = 0;
	uint32_t value_offset, next_free;
	int refcount;
	int iteration;

	ctx->result = 0;
	ctx->message[0] = '\0';

	if (!zend_opcache_run_request_code(clear_code, "zts shared graph clear", false, ctx->message, sizeof(ctx->message))) {
		ctx->result = 1;
		ts_free_thread();
		return NULL;
	}

	for (iteration = 0; iteration < REQUEST_COUNT; iteration++) {
		if (!zend_opcache_run_request_code(worker_code, "zts shared graph worker", true, ctx->message, sizeof(ctx->message))) {
			ctx->result = 1;
			break;
		}

		if (!zend_opcache_inspect_current_payload_state(&value_offset, &next_free, &refcount, ctx->message, sizeof(ctx->message))) {
			ctx->result = 1;
			break;
		}

		if (refcount != 0) {
			ctx->result = 1;
			snprintf(
				ctx->message,
				sizeof(ctx->message),
				"shared graph refcount leaked after request %d (offset=%u next_free=%u refcount=%d)",
				iteration + 1,
				value_offset,
				next_free,
				refcount
			);
			break;
		}

		if (iteration == 0) {
			expected_offset = value_offset;
		} else if (value_offset != expected_offset) {
			ctx->result = 1;
			snprintf(
				ctx->message,
				sizeof(ctx->message),
				"shared graph payload offset changed between requests (%u -> %u, next_free=%u)",
				expected_offset,
				value_offset,
				next_free
			);
			break;
		}
	}

	ts_free_thread();
	return NULL;
}

int main(int argc, char **argv)
{
	zend_thread_t thread;
	zend_opcache_ref_thread_ctx ctx;

	if (zend_opcache_test_startup(argc, argv) == FAILURE) {
		fprintf(stderr, "startup failed\n");
		return 1;
	}

	if (!zend_thread_start(&thread, zend_opcache_ref_thread_main, &ctx)) {
		fprintf(stderr, "thread creation failed\n");
		zend_opcache_test_shutdown();
		return 1;
	}

	if (!zend_thread_join(&thread)) {
		fprintf(stderr, "thread join failed\n");
		zend_opcache_test_shutdown();
		return 1;
	}

	if (ctx.result != 0) {
		fprintf(stderr, "%s\n", ctx.message);
		zend_opcache_test_shutdown();
		return 1;
	}

	printf("ok\n");
	zend_opcache_test_shutdown();
	return 0;
}
