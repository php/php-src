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
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Zend/zend_exceptions.h"
#include "Zend/zend_execute.h"
#include "Zend/zend_portability.h"
#include "sapi/embed/php_embed.h"

#ifndef ZTS
# error "This helper requires a ZTS build"
#endif

#define TEST_KEY "zts-thread-shared-key"
#define TEST_VALUE "writer-thread-payload-0123456789"

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

typedef enum {
	THREAD_MODE_STORE,
	THREAD_MODE_FETCH
} zend_opcache_thread_mode;

typedef struct _zend_opcache_thread_ctx {
	zend_opcache_thread_mode mode;
	int result;
	char message[128];
} zend_opcache_thread_ctx;

static void zend_opcache_thread_set_failure(zend_opcache_thread_ctx *ctx, const char *message)
{
	ctx->result = 1;
	snprintf(ctx->message, sizeof(ctx->message), "%s", message);
}

static int zend_opcache_test_startup(int argc, char **argv)
{
	if (!php_tsrm_startup_ex(4)) {
		return FAILURE;
	}
	ZEND_TSRMLS_CACHE_UPDATE();

	zend_signal_startup();
	sapi_startup(&php_embed_module);
	/* Static Cache is opt-in per SAPI; this embed-based test enables it. */
	extern void zend_opcache_static_cache_opt_in(void);
	zend_opcache_static_cache_opt_in();
	php_embed_module.ini_entries = opcache_test_ini;
	if (argv != NULL) {
		php_embed_module.executable_location = argv[0];
	}

	if (php_embed_module.startup(&php_embed_module) == FAILURE) {
		sapi_shutdown();
		tsrm_shutdown();
		return FAILURE;
	}

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

static void *zend_opcache_thread_main(void *arg)
{
	static const char store_code[] =
		"OPcache\\VolatileCache::set('" TEST_KEY "', '" TEST_VALUE "');";
	static const char fetch_code[] =
		"OPcache\\VolatileCache::get('" TEST_KEY "');";
	zend_opcache_thread_ctx *ctx;
	zval retval;
	bool request_started = false;

	ctx = (zend_opcache_thread_ctx *) arg;
	ctx->result = 0;
	ctx->message[0] = '\0';
	ZVAL_UNDEF(&retval);

	if (!zend_opcache_thread_request_startup()) {
		zend_opcache_thread_set_failure(ctx, "request startup failed");
		goto cleanup;
	}
	request_started = true;

	if (ctx->mode == THREAD_MODE_STORE) {
		if (!zend_opcache_thread_eval(store_code, &retval, "zts volatile cache store")) {
			zend_opcache_thread_set_failure(ctx, "VolatileCache::set failed");
			goto cleanup;
		}

		if (!zend_is_true(&retval)) {
			zend_opcache_thread_set_failure(ctx, "VolatileCache::set returned false");
		}
	} else {
		if (!zend_opcache_thread_eval(fetch_code, &retval, "zts volatile cache fetch")) {
			zend_opcache_thread_set_failure(ctx, "VolatileCache::get failed");
			goto cleanup;
		}

		if (Z_TYPE(retval) != IS_STRING || strcmp(Z_STRVAL(retval), TEST_VALUE) != 0) {
			zend_opcache_thread_set_failure(ctx, "VolatileCache::get returned unexpected value");
		}
	}

cleanup:
	if (!Z_ISUNDEF(retval)) {
		zval_ptr_dtor(&retval);
	}
	if (request_started) {
		php_request_shutdown(NULL);
	}
	ts_free_thread();
	return NULL;
}

int main(int argc, char **argv)
{
	static const char clear_code[] = "OPcache\\VolatileCache::clear();";
	static const char fetch_code[] =
		"OPcache\\VolatileCache::get('" TEST_KEY "');";
	zend_thread_t writer_thread;
	zend_thread_t reader_thread;
	zend_opcache_thread_ctx writer_ctx;
	zend_opcache_thread_ctx reader_ctx;
	zval retval;
	int exit_code;

	if (zend_opcache_test_startup(argc, argv) == FAILURE) {
		fprintf(stderr, "startup failed\n");
		return 1;
	}

	exit_code = 1;
	if (!zend_opcache_thread_request_startup()) {
		fprintf(stderr, "VolatileCache::clear request startup failed\n");
		goto cleanup;
	}
	if (!zend_opcache_thread_eval(clear_code, &retval, "zts volatile cache clear")) {
		fprintf(stderr, "VolatileCache::clear failed\n");
		php_request_shutdown(NULL);
		goto cleanup;
	}
	if (!Z_ISUNDEF(retval)) {
		zval_ptr_dtor(&retval);
	}
	php_request_shutdown(NULL);

	writer_ctx.mode = THREAD_MODE_STORE;
	if (!zend_thread_start(&writer_thread, zend_opcache_thread_main, &writer_ctx)) {
		fprintf(stderr, "writer thread creation failed\n");
		goto cleanup;
	}
	if (!zend_thread_join(&writer_thread)) {
		fprintf(stderr, "writer thread join failed\n");
		goto cleanup;
	}
	if (writer_ctx.result != 0) {
		fprintf(stderr, "%s\n", writer_ctx.message);
		goto cleanup;
	}

	reader_ctx.mode = THREAD_MODE_FETCH;
	if (!zend_thread_start(&reader_thread, zend_opcache_thread_main, &reader_ctx)) {
		fprintf(stderr, "reader thread creation failed\n");
		goto cleanup;
	}
	if (!zend_thread_join(&reader_thread)) {
		fprintf(stderr, "reader thread join failed\n");
		goto cleanup;
	}
	if (reader_ctx.result != 0) {
		fprintf(stderr, "%s\n", reader_ctx.message);
		goto cleanup;
	}

	if (!zend_opcache_thread_request_startup()) {
		fprintf(stderr, "main thread request startup failed\n");
		goto cleanup;
	}
	if (!zend_opcache_thread_eval(fetch_code, &retval, "zts volatile cache main fetch")) {
		fprintf(stderr, "main thread fetch failed\n");
		php_request_shutdown(NULL);
		goto cleanup;
	}
	if (Z_TYPE(retval) != IS_STRING || strcmp(Z_STRVAL(retval), TEST_VALUE) != 0) {
		fprintf(stderr, "main thread fetch returned unexpected value\n");
		if (!Z_ISUNDEF(retval)) {
			zval_ptr_dtor(&retval);
		}
		php_request_shutdown(NULL);
		goto cleanup;
	}
	if (!Z_ISUNDEF(retval)) {
		zval_ptr_dtor(&retval);
	}
	php_request_shutdown(NULL);

	printf("ok\n");
	exit_code = 0;

cleanup:
	zend_opcache_test_shutdown();
	return exit_code;
}
