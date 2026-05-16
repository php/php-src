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

typedef enum {
	THREAD_MODE_SEED,
	THREAD_MODE_DELETE
} zend_opcache_thread_mode;

typedef struct _zend_opcache_thread_ctx {
	zend_opcache_thread_mode mode;
	int result;
	char message[128];
} zend_opcache_thread_ctx;

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
	"opcache.static_cache.volatile_size_mb=8\n"
	"opcache.static_cache.persistent_size_mb=8\n\0";

static const char init_code[] =
	"(static function (): bool {"
	"    OPcache\\volatile_clear();"
	"    OPcache\\persistent_clear();"
	"    return true;"
	"})()";

static const char seed_code[] =
	"(static function (): bool {"
	"    if (!OPcache\\volatile_store('zts_v_first', str_repeat('A', 1800000))) return false;"
	"    if (!OPcache\\volatile_store('zts_v_second', str_repeat('B', 1800000))) return false;"
	"    if (!OPcache\\volatile_store('zts_v_third', str_repeat('C', 1800000))) return false;"
	"    OPcache\\persistent_store('zts_p_first', str_repeat('A', 1800000));"
	"    OPcache\\persistent_store('zts_p_second', str_repeat('B', 1800000));"
	"    OPcache\\persistent_store('zts_p_third', str_repeat('C', 1800000));"
	"    return true;"
	"})()";

static const char delete_code[] =
	"(static function (): bool {"
	"    OPcache\\volatile_delete('zts_v_second');"
	"    OPcache\\persistent_delete('zts_p_second');"
	"    return OPcache\\volatile_fetch('zts_v_second', 'missing') === 'missing'"
	"        && OPcache\\persistent_fetch('zts_p_second', 'missing') === 'missing';"
	"})()";

static const char refill_code[] =
	"(static function (): bool {"
	"    if (!OPcache\\volatile_store('zts_v_replacement', str_repeat('R', 1500000))) return false;"
	"    OPcache\\persistent_store('zts_p_replacement', str_repeat('R', 1500000));"
	"    return strlen(OPcache\\volatile_fetch('zts_v_first')) === 1800000"
	"        && strlen(OPcache\\volatile_fetch('zts_v_third')) === 1800000"
	"        && strlen(OPcache\\volatile_fetch('zts_v_replacement')) === 1500000"
	"        && strlen(OPcache\\persistent_fetch('zts_p_first')) === 1800000"
	"        && strlen(OPcache\\persistent_fetch('zts_p_third')) === 1800000"
	"        && strlen(OPcache\\persistent_fetch('zts_p_replacement')) === 1500000;"
	"})()";

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

static bool zend_opcache_thread_run_request_code(const char *code, const char *label, char *message, size_t message_size)
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

	if (!Z_ISUNDEF(retval)) {
		zval_ptr_dtor(&retval);
	}
	php_request_shutdown(NULL);
	return true;
}

static void *zend_opcache_thread_main(void *arg)
{
	zend_opcache_thread_ctx *ctx = (zend_opcache_thread_ctx *) arg;
	const char *code = ctx->mode == THREAD_MODE_SEED ? seed_code : delete_code;
	const char *label = ctx->mode == THREAD_MODE_SEED ? "zts explicit cache seed" : "zts explicit cache delete";

	ctx->result = 0;
	ctx->message[0] = '\0';
	if (!zend_opcache_thread_run_request_code(code, label, ctx->message, sizeof(ctx->message))) {
		ctx->result = 1;
	}

	ts_free_thread();
	return NULL;
}

static bool zend_opcache_run_thread(zend_opcache_thread_mode mode, char *message, size_t message_size)
{
	zend_thread_t thread;
	zend_opcache_thread_ctx ctx;

	ctx.mode = mode;
	if (!zend_thread_start(&thread, zend_opcache_thread_main, &ctx)) {
		snprintf(message, message_size, "thread creation failed");
		return false;
	}
	if (!zend_thread_join(&thread)) {
		snprintf(message, message_size, "thread join failed");
		return false;
	}
	if (ctx.result != 0) {
		snprintf(message, message_size, "%s", ctx.message);
		return false;
	}

	return true;
}

int main(int argc, char **argv)
{
	char message[128];
	int exit_code;

	if (zend_opcache_test_startup(argc, argv) == FAILURE) {
		fprintf(stderr, "startup failed\n");
		return 1;
	}

	exit_code = 1;
	message[0] = '\0';

	if (!zend_opcache_thread_run_request_code(init_code, "zts explicit cache init", message, sizeof(message))) {
		fprintf(stderr, "%s\n", message);
		goto cleanup;
	}

	if (!zend_opcache_run_thread(THREAD_MODE_SEED, message, sizeof(message))) {
		fprintf(stderr, "%s\n", message);
		goto cleanup;
	}

	if (!zend_opcache_run_thread(THREAD_MODE_DELETE, message, sizeof(message))) {
		fprintf(stderr, "%s\n", message);
		goto cleanup;
	}

	if (!zend_opcache_thread_run_request_code(refill_code, "zts explicit cache refill", message, sizeof(message))) {
		fprintf(stderr, "%s\n", message);
		goto cleanup;
	}

	printf("ok\n");
	exit_code = 0;

cleanup:
	zend_opcache_test_shutdown();
	return exit_code;
}
