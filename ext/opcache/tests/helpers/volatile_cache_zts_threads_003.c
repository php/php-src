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

typedef struct _zend_opcache_thread_ctx {
	const char *mode;
	const char *scenario_path;
	const char *label;
	int result;
	char message[256];
} zend_opcache_thread_ctx;

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

static zend_opcache_test_accel_directives zend_opcache_thread_accel_directives;
static bool zend_opcache_thread_enabled;

static char *zend_opcache_build_scenario_code(const char *mode, const char *scenario_path);

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

static char *zend_opcache_build_scenario_code(const char *mode, const char *scenario_path)
{
	char *code;
	size_t code_len;

	code_len = strlen(mode) + strlen(scenario_path) + sizeof("$mode=''; require '';\0");
	code = malloc(code_len);
	if (code == NULL) {
		return NULL;
	}

	snprintf(code, code_len, "$mode='%s'; require '%s';", mode, scenario_path);
	return code;
}

static void *zend_opcache_thread_main(void *arg)
{
	char *code;
	zend_opcache_thread_ctx *ctx;

	ctx = (zend_opcache_thread_ctx *) arg;
	ctx->result = 0;
	ctx->message[0] = '\0';

	code = zend_opcache_build_scenario_code(ctx->mode, ctx->scenario_path);
	if (code == NULL) {
		ctx->result = 1;
		snprintf(ctx->message, sizeof(ctx->message), "failed to build scenario code");
		ts_free_thread();
		return NULL;
	}

	if (!zend_opcache_thread_run_request_code(code, ctx->label, ctx->message, sizeof(ctx->message))) {
		ctx->result = 1;
	}

	free(code);
	ts_free_thread();
	return NULL;
}

static bool zend_opcache_run_mode_request(const char *mode, const char *scenario_path, const char *label, char *message, size_t message_size)
{
	char *code;
	bool success;

	code = zend_opcache_build_scenario_code(mode, scenario_path);
	if (code == NULL) {
		snprintf(message, message_size, "%s: failed to build scenario code", label);
		return false;
	}

	success = zend_opcache_thread_run_request_code(code, label, message, message_size);
	free(code);
	return success;
}

static bool zend_opcache_run_threaded_scenario(const char *scenario_path, char *message, size_t message_size)
{
	zend_thread_t reader_thread;
	zend_thread_t writer_thread;
	zend_opcache_thread_ctx reader_ctx;
	zend_opcache_thread_ctx writer_ctx;

	if (!zend_opcache_run_mode_request("init", scenario_path, "zts lookup init", message, message_size)) {
		return false;
	}

	reader_ctx.mode = "reader";
	reader_ctx.scenario_path = scenario_path;
	reader_ctx.label = "zts lookup reader";
	if (!zend_thread_start(&reader_thread, zend_opcache_thread_main, &reader_ctx)) {
		snprintf(message, message_size, "reader thread creation failed");
		return false;
	}

	writer_ctx.mode = "writer";
	writer_ctx.scenario_path = scenario_path;
	writer_ctx.label = "zts lookup writer";
	if (!zend_thread_start(&writer_thread, zend_opcache_thread_main, &writer_ctx)) {
		snprintf(message, message_size, "writer thread creation failed");
		zend_thread_join(&reader_thread);
		return false;
	}

	if (!zend_thread_join(&reader_thread)) {
		snprintf(message, message_size, "reader thread join failed");
		zend_thread_join(&writer_thread);
		return false;
	}
	if (!zend_thread_join(&writer_thread)) {
		snprintf(message, message_size, "writer thread join failed");
		return false;
	}
	if (reader_ctx.result != 0) {
		snprintf(message, message_size, "%s", reader_ctx.message);
		return false;
	}
	if (writer_ctx.result != 0) {
		snprintf(message, message_size, "%s", writer_ctx.message);
		return false;
	}

	return true;
}

int main(int argc, char **argv)
{
	char message[256];
	int exit_code;

	if (argc < 2) {
		fprintf(stderr, "scenario path required\n");
		return 1;
	}

	if (zend_opcache_test_startup(argc, argv) == FAILURE) {
		fprintf(stderr, "startup failed\n");
		return 1;
	}

	exit_code = 1;
	message[0] = '\0';

	if (!zend_opcache_run_threaded_scenario(argv[1], message, sizeof(message))) {
		fprintf(stderr, "%s\n", message);
		goto cleanup;
	}

	printf("ok\n");
	exit_code = 0;

cleanup:
	zend_opcache_test_shutdown();
	return exit_code;
}
