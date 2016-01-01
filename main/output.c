/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |          Marcus Boerger <helly@php.net>                              |
   | New API: Michael Wallner <mike@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_OUTPUT_DEBUG
# define PHP_OUTPUT_DEBUG 0
#endif
#ifndef PHP_OUTPUT_NOINLINE
# define PHP_OUTPUT_NOINLINE 0
#endif

#include "php.h"
#include "ext/standard/head.h"
#include "ext/standard/url_scanner_ex.h"
#include "SAPI.h"
#include "zend_stack.h"
#include "php_output.h"

PHPAPI ZEND_DECLARE_MODULE_GLOBALS(output);

const char php_output_default_handler_name[sizeof("default output handler")] = "default output handler";
const char php_output_devnull_handler_name[sizeof("null output handler")] = "null output handler";

#if PHP_OUTPUT_NOINLINE || PHP_OUTPUT_DEBUG
# undef inline
# define inline
#endif

/* {{{ aliases, conflict and reverse conflict hash tables */
static HashTable php_output_handler_aliases;
static HashTable php_output_handler_conflicts;
static HashTable php_output_handler_reverse_conflicts;
/* }}} */

/* {{{ forward declarations */
static inline int php_output_lock_error(int op TSRMLS_DC);
static inline void php_output_op(int op, const char *str, size_t len TSRMLS_DC);

static inline php_output_handler *php_output_handler_init(const char *name, size_t name_len, size_t chunk_size, int flags TSRMLS_DC);
static inline php_output_handler_status_t php_output_handler_op(php_output_handler *handler, php_output_context *context);
static inline int php_output_handler_append(php_output_handler *handler, const php_output_buffer *buf TSRMLS_DC);
static inline zval *php_output_handler_status(php_output_handler *handler, zval *entry);

static inline php_output_context *php_output_context_init(php_output_context *context, int op TSRMLS_DC);
static inline void php_output_context_reset(php_output_context *context);
static inline void php_output_context_swap(php_output_context *context);
static inline void php_output_context_dtor(php_output_context *context);

static inline int php_output_stack_pop(int flags TSRMLS_DC);

static int php_output_stack_apply_op(void *h, void *c);
static int php_output_stack_apply_clean(void *h, void *c);
static int php_output_stack_apply_list(void *h, void *z);
static int php_output_stack_apply_status(void *h, void *z);

static int php_output_handler_compat_func(void **handler_context, php_output_context *output_context);
static int php_output_handler_default_func(void **handler_context, php_output_context *output_context);
static int php_output_handler_devnull_func(void **handler_context, php_output_context *output_context);
/* }}} */

/* {{{ static void php_output_init_globals(zend_output_globals *G)
 * Initialize the module globals on MINIT */
static inline void php_output_init_globals(zend_output_globals *G)
{
	memset(G, 0, sizeof(*G));
}
/* }}} */

/* {{{ stderr/stdout writer if not PHP_OUTPUT_ACTIVATED */
static int php_output_stdout(const char *str, size_t str_len)
{
	fwrite(str, 1, str_len, stdout);
	return str_len;
}
static int php_output_stderr(const char *str, size_t str_len)
{
	fwrite(str, 1, str_len, stderr);
/* See http://support.microsoft.com/kb/190351 */
#ifdef PHP_WIN32
	fflush(stderr);
#endif
	return str_len;
}
static int (*php_output_direct)(const char *str, size_t str_len) = php_output_stderr;
/* }}} */

/* {{{ void php_output_header(TSRMLS_D) */
static void php_output_header(TSRMLS_D)
{
	if (!SG(headers_sent)) {
		if (!OG(output_start_filename)) {
			if (zend_is_compiling(TSRMLS_C)) {
				OG(output_start_filename) = zend_get_compiled_filename(TSRMLS_C);
				OG(output_start_lineno) = zend_get_compiled_lineno(TSRMLS_C);
			} else if (zend_is_executing(TSRMLS_C)) {
				OG(output_start_filename) = zend_get_executed_filename(TSRMLS_C);
				OG(output_start_lineno) = zend_get_executed_lineno(TSRMLS_C);
			}
#if PHP_OUTPUT_DEBUG
			fprintf(stderr, "!!! output started at: %s (%d)\n", OG(output_start_filename), OG(output_start_lineno));
#endif
		}
		if (!php_header(TSRMLS_C)) {
			OG(flags) |= PHP_OUTPUT_DISABLED;
		}
	}
}
/* }}} */

/* {{{ void php_output_startup(void)
 * Set up module globals and initalize the conflict and reverse conflict hash tables */
PHPAPI void php_output_startup(void)
{
	ZEND_INIT_MODULE_GLOBALS(output, php_output_init_globals, NULL);
	zend_hash_init(&php_output_handler_aliases, 0, NULL, NULL, 1);
	zend_hash_init(&php_output_handler_conflicts, 0, NULL, NULL, 1);
	zend_hash_init(&php_output_handler_reverse_conflicts, 0, NULL, (void (*)(void *)) zend_hash_destroy, 1);
	php_output_direct = php_output_stdout;
}
/* }}} */

/* {{{ void php_output_shutdown(void)
 * Destroy module globals and the conflict and reverse conflict hash tables */
PHPAPI void php_output_shutdown(void)
{
	php_output_direct = php_output_stderr;
	zend_hash_destroy(&php_output_handler_aliases);
	zend_hash_destroy(&php_output_handler_conflicts);
	zend_hash_destroy(&php_output_handler_reverse_conflicts);
}
/* }}} */

/* {{{ SUCCESS|FAILURE php_output_activate(TSRMLS_D)
 * Reset output globals and setup the output handler stack */
PHPAPI int php_output_activate(TSRMLS_D)
{
#ifdef ZTS
	memset((*((void ***) tsrm_ls))[TSRM_UNSHUFFLE_RSRC_ID(output_globals_id)], 0, sizeof(zend_output_globals));
#else
	memset(&output_globals, 0, sizeof(zend_output_globals));
#endif

	zend_stack_init(&OG(handlers));
	OG(flags) |= PHP_OUTPUT_ACTIVATED;

	return SUCCESS;
}
/* }}} */

/* {{{ void php_output_deactivate(TSRMLS_D)
 * Destroy the output handler stack */
PHPAPI void php_output_deactivate(TSRMLS_D)
{
	php_output_handler **handler = NULL;

	if ((OG(flags) & PHP_OUTPUT_ACTIVATED)) {
		php_output_header(TSRMLS_C);

		OG(flags) ^= PHP_OUTPUT_ACTIVATED;
		OG(active) = NULL;
		OG(running) = NULL;

		/* release all output handlers */
		if (OG(handlers).elements) {
			while (SUCCESS == zend_stack_top(&OG(handlers), (void *) &handler)) {
				php_output_handler_free(handler TSRMLS_CC);
				zend_stack_del_top(&OG(handlers));
			}
			zend_stack_destroy(&OG(handlers));
		}
	}
}
/* }}} */

/* {{{ void php_output_register_constants() */
PHPAPI void php_output_register_constants(TSRMLS_D)
{
	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_START", PHP_OUTPUT_HANDLER_START, CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_WRITE", PHP_OUTPUT_HANDLER_WRITE, CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_FLUSH", PHP_OUTPUT_HANDLER_FLUSH, CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_CLEAN", PHP_OUTPUT_HANDLER_CLEAN, CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_FINAL", PHP_OUTPUT_HANDLER_FINAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_CONT", PHP_OUTPUT_HANDLER_WRITE, CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_END", PHP_OUTPUT_HANDLER_FINAL, CONST_CS | CONST_PERSISTENT);

	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_CLEANABLE", PHP_OUTPUT_HANDLER_CLEANABLE, CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_FLUSHABLE", PHP_OUTPUT_HANDLER_FLUSHABLE, CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_REMOVABLE", PHP_OUTPUT_HANDLER_REMOVABLE, CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_STDFLAGS", PHP_OUTPUT_HANDLER_STDFLAGS, CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_STARTED", PHP_OUTPUT_HANDLER_STARTED, CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_DISABLED", PHP_OUTPUT_HANDLER_DISABLED, CONST_CS | CONST_PERSISTENT);
}
/* }}} */

/* {{{ void php_output_set_status(int status TSRMLS_DC)
 * Used by SAPIs to disable output */
PHPAPI void php_output_set_status(int status TSRMLS_DC)
{
	OG(flags) = (OG(flags) & ~0xf) | (status & 0xf);
}
/* }}} */

/* {{{ int php_output_get_status(TSRMLS_C)
 * Get output control status */
PHPAPI int php_output_get_status(TSRMLS_D)
{
	return (
		OG(flags)
		|	(OG(active) ? PHP_OUTPUT_ACTIVE : 0)
		|	(OG(running)? PHP_OUTPUT_LOCKED : 0)
	) & 0xff;
}
/* }}} */

/* {{{ int php_output_write_unbuffered(const char *str, size_t len TSRMLS_DC)
 * Unbuffered write */
PHPAPI int php_output_write_unbuffered(const char *str, size_t len TSRMLS_DC)
{
#if PHP_DEBUG
	if (len > UINT_MAX) {
		php_error(E_WARNING, "Attempt to output more than UINT_MAX bytes at once; "
				"output will be truncated %lu => %lu",
				(unsigned long) len, (unsigned long) (len % UINT_MAX));
	}
#endif
	if (OG(flags) & PHP_OUTPUT_DISABLED) {
		return 0;
	}
	if (OG(flags) & PHP_OUTPUT_ACTIVATED) {
		return sapi_module.ub_write(str, len TSRMLS_CC);
	}
	return php_output_direct(str, len);
}
/* }}} */

/* {{{ int php_output_write(const char *str, size_t len TSRMLS_DC)
 * Buffered write */
PHPAPI int php_output_write(const char *str, size_t len TSRMLS_DC)
{
#if PHP_DEBUG
	if (len > UINT_MAX) {
		php_error(E_WARNING, "Attempt to output more than UINT_MAX bytes at once; "
				"output will be truncated %lu => %lu",
				(unsigned long) len, (unsigned long) (len % UINT_MAX));
	}
#endif
	if (OG(flags) & PHP_OUTPUT_DISABLED) {
		return 0;
	}
	if (OG(flags) & PHP_OUTPUT_ACTIVATED) {
		php_output_op(PHP_OUTPUT_HANDLER_WRITE, str, len TSRMLS_CC);
		return (int) len;
	}
	return php_output_direct(str, len);
}
/* }}} */

/* {{{ SUCCESS|FAILURE php_output_flush(TSRMLS_D)
 * Flush the most recent output handlers buffer */
PHPAPI int php_output_flush(TSRMLS_D)
{
	php_output_context context;

	if (OG(active) && (OG(active)->flags & PHP_OUTPUT_HANDLER_FLUSHABLE)) {
		php_output_context_init(&context, PHP_OUTPUT_HANDLER_FLUSH TSRMLS_CC);
		php_output_handler_op(OG(active), &context);
		if (context.out.data && context.out.used) {
			zend_stack_del_top(&OG(handlers));
			php_output_write(context.out.data, context.out.used TSRMLS_CC);
			zend_stack_push(&OG(handlers), &OG(active), sizeof(php_output_handler *));
		}
		php_output_context_dtor(&context);
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

/* {{{ void php_output_flush_all(TSRMLS_C)
 * Flush all output buffers subsequently */
PHPAPI void php_output_flush_all(TSRMLS_D)
{
	if (OG(active)) {
		php_output_op(PHP_OUTPUT_HANDLER_FLUSH, NULL, 0 TSRMLS_CC);
	}
}
/* }}} */

/* {{{ SUCCESS|FAILURE php_output_clean(TSRMLS_D)
 * Cleans the most recent output handlers buffer if the handler is cleanable */
PHPAPI int php_output_clean(TSRMLS_D)
{
	php_output_context context;

	if (OG(active) && (OG(active)->flags & PHP_OUTPUT_HANDLER_CLEANABLE)) {
		php_output_context_init(&context, PHP_OUTPUT_HANDLER_CLEAN TSRMLS_CC);
		php_output_handler_op(OG(active), &context);
		php_output_context_dtor(&context);
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

/* {{{ void php_output_clean_all(TSRMLS_D)
 * Cleans all output handler buffers, without regard whether the handler is cleanable */
PHPAPI void php_output_clean_all(TSRMLS_D)
{
	php_output_context context;

	if (OG(active)) {
		php_output_context_init(&context, PHP_OUTPUT_HANDLER_CLEAN TSRMLS_CC);
		zend_stack_apply_with_argument(&OG(handlers), ZEND_STACK_APPLY_TOPDOWN, php_output_stack_apply_clean, &context);
	}
}

/* {{{ SUCCESS|FAILURE php_output_end(TSRMLS_D)
 * Finalizes the most recent output handler at pops it off the stack if the handler is removable */
PHPAPI int php_output_end(TSRMLS_D)
{
	if (php_output_stack_pop(PHP_OUTPUT_POP_TRY TSRMLS_CC)) {
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

/* {{{ void php_output_end_all(TSRMLS_D)
 * Finalizes all output handlers and ends output buffering without regard whether a handler is removable */
PHPAPI void php_output_end_all(TSRMLS_D)
{
	while (OG(active) && php_output_stack_pop(PHP_OUTPUT_POP_FORCE TSRMLS_CC));
}
/* }}} */

/* {{{ SUCCESS|FAILURE php_output_discard(TSRMLS_D)
 * Discards the most recent output handlers buffer and pops it off the stack if the handler is removable */
PHPAPI int php_output_discard(TSRMLS_D)
{
	if (php_output_stack_pop(PHP_OUTPUT_POP_DISCARD|PHP_OUTPUT_POP_TRY TSRMLS_CC)) {
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

/* {{{ void php_output_discard_all(TSRMLS_D)
 * Discard all output handlers and buffers without regard whether a handler is removable */
PHPAPI void php_output_discard_all(TSRMLS_D)
{
	while (OG(active)) {
		php_output_stack_pop(PHP_OUTPUT_POP_DISCARD|PHP_OUTPUT_POP_FORCE TSRMLS_CC);
	}
}
/* }}} */

/* {{{ int php_output_get_level(TSRMLS_D)
 * Get output buffering level, ie. how many output handlers the stack contains */
PHPAPI int php_output_get_level(TSRMLS_D)
{
	return OG(active) ? zend_stack_count(&OG(handlers)) : 0;
}
/* }}} */

/* {{{ SUCCESS|FAILURE php_output_get_contents(zval *z TSRMLS_DC)
 * Get the contents of the active output handlers buffer */
PHPAPI int php_output_get_contents(zval *p TSRMLS_DC)
{
	if (OG(active)) {
		ZVAL_STRINGL(p, OG(active)->buffer.data, OG(active)->buffer.used, 1);
		return SUCCESS;
	} else {
		ZVAL_NULL(p);
		return FAILURE;
	}
}

/* {{{ SUCCESS|FAILURE php_output_get_length(zval *z TSRMLS_DC)
 * Get the length of the active output handlers buffer */
PHPAPI int php_output_get_length(zval *p TSRMLS_DC)
{
	if (OG(active)) {
		ZVAL_LONG(p, OG(active)->buffer.used);
		return SUCCESS;
	} else {
		ZVAL_NULL(p);
		return FAILURE;
	}
}
/* }}} */

/* {{{ php_output_handler* php_output_get_active_handler(TSRMLS_D)
 * Get active output handler */
PHPAPI php_output_handler* php_output_get_active_handler(TSRMLS_D)
{
	return OG(active);
}
/* }}} */

/* {{{ SUCCESS|FAILURE php_output_handler_start_default(TSRMLS_D)
 * Start a "default output handler" */
PHPAPI int php_output_start_default(TSRMLS_D)
{
	php_output_handler *handler;

	handler = php_output_handler_create_internal(ZEND_STRL(php_output_default_handler_name), php_output_handler_default_func, 0, PHP_OUTPUT_HANDLER_STDFLAGS TSRMLS_CC);
	if (SUCCESS == php_output_handler_start(handler TSRMLS_CC)) {
		return SUCCESS;
	}
	php_output_handler_free(&handler TSRMLS_CC);
	return FAILURE;
}
/* }}} */

/* {{{ SUCCESS|FAILURE php_output_handler_start_devnull(TSRMLS_D)
 * Start a "null output handler" */
PHPAPI int php_output_start_devnull(TSRMLS_D)
{
	php_output_handler *handler;

	handler = php_output_handler_create_internal(ZEND_STRL(php_output_devnull_handler_name), php_output_handler_devnull_func, PHP_OUTPUT_HANDLER_DEFAULT_SIZE, 0 TSRMLS_CC);
	if (SUCCESS == php_output_handler_start(handler TSRMLS_CC)) {
		return SUCCESS;
	}
	php_output_handler_free(&handler TSRMLS_CC);
	return FAILURE;
}
/* }}} */

/* {{{ SUCCESS|FAILURE php_output_start_user(zval *handler, size_t chunk_size, int flags TSRMLS_DC)
 * Start a user level output handler */
PHPAPI int php_output_start_user(zval *output_handler, size_t chunk_size, int flags TSRMLS_DC)
{
	php_output_handler *handler;

	if (output_handler) {
		handler = php_output_handler_create_user(output_handler, chunk_size, flags TSRMLS_CC);
	} else {
		handler = php_output_handler_create_internal(ZEND_STRL(php_output_default_handler_name), php_output_handler_default_func, chunk_size, flags TSRMLS_CC);
	}
	if (SUCCESS == php_output_handler_start(handler TSRMLS_CC)) {
		return SUCCESS;
	}
	php_output_handler_free(&handler TSRMLS_CC);
	return FAILURE;
}
/* }}} */

/* {{{ SUCCESS|FAILURE php_output_start_internal(zval *name, php_output_handler_func_t handler, size_t chunk_size, int flags TSRMLS_DC)
 * Start an internal output handler that does not have to maintain a non-global state */
PHPAPI int php_output_start_internal(const char *name, size_t name_len, php_output_handler_func_t output_handler, size_t chunk_size, int flags TSRMLS_DC)
{
	php_output_handler *handler;

	handler = php_output_handler_create_internal(name, name_len, php_output_handler_compat_func, chunk_size, flags TSRMLS_CC);
	php_output_handler_set_context(handler, output_handler, NULL TSRMLS_CC);
	if (SUCCESS == php_output_handler_start(handler TSRMLS_CC)) {
		return SUCCESS;
	}
	php_output_handler_free(&handler TSRMLS_CC);
	return FAILURE;
}
/* }}} */

/* {{{ php_output_handler *php_output_handler_create_user(zval *handler, size_t chunk_size, int flags TSRMLS_DC)
 * Create a user level output handler */
PHPAPI php_output_handler *php_output_handler_create_user(zval *output_handler, size_t chunk_size, int flags TSRMLS_DC)
{
	char *handler_name = NULL, *error = NULL;
	php_output_handler *handler = NULL;
	php_output_handler_alias_ctor_t *alias = NULL;
	php_output_handler_user_func_t *user = NULL;

	switch (Z_TYPE_P(output_handler)) {
		case IS_NULL:
			handler = php_output_handler_create_internal(ZEND_STRL(php_output_default_handler_name), php_output_handler_default_func, chunk_size, flags TSRMLS_CC);
			break;
		case IS_STRING:
			if (Z_STRLEN_P(output_handler) && (alias = php_output_handler_alias(Z_STRVAL_P(output_handler), Z_STRLEN_P(output_handler) TSRMLS_CC))) {
				handler = (*alias)(Z_STRVAL_P(output_handler), Z_STRLEN_P(output_handler), chunk_size, flags TSRMLS_CC);
				break;
			}
		default:
			user = ecalloc(1, sizeof(php_output_handler_user_func_t));
			if (SUCCESS == zend_fcall_info_init(output_handler, 0, &user->fci, &user->fcc, &handler_name, &error TSRMLS_CC)) {
				handler = php_output_handler_init(handler_name, strlen(handler_name), chunk_size, (flags & ~0xf) | PHP_OUTPUT_HANDLER_USER TSRMLS_CC);
				Z_ADDREF_P(output_handler);
				user->zoh = output_handler;
				handler->func.user = user;
			} else {
				efree(user);
			}
			if (error) {
				php_error_docref("ref.outcontrol" TSRMLS_CC, E_WARNING, "%s", error);
				efree(error);
			}
			if (handler_name) {
				efree(handler_name);
			}
	}

	return handler;
}
/* }}} */

/* {{{ php_output_handler *php_output_handler_create_internal(zval *name, php_output_handler_context_func_t handler, size_t chunk_size, int flags TSRMLS_DC)
 * Create an internal output handler that can maintain a non-global state */
PHPAPI php_output_handler *php_output_handler_create_internal(const char *name, size_t name_len, php_output_handler_context_func_t output_handler, size_t chunk_size, int flags TSRMLS_DC)
{
	php_output_handler *handler;

	handler = php_output_handler_init(name, name_len, chunk_size, (flags & ~0xf) | PHP_OUTPUT_HANDLER_INTERNAL TSRMLS_CC);
	handler->func.internal = output_handler;

	return handler;
}
/* }}} */

/* {{{ void php_output_handler_set_context(php_output_handler *handler, void *opaq, void (*dtor)(void* TSRMLS_DC) TSRMLS_DC)
 * Set the context/state of an output handler. Calls the dtor of the previous context if there is one */
PHPAPI void php_output_handler_set_context(php_output_handler *handler, void *opaq, void (*dtor)(void* TSRMLS_DC) TSRMLS_DC)
{
	if (handler->dtor && handler->opaq) {
		handler->dtor(handler->opaq TSRMLS_CC);
	}
	handler->dtor = dtor;
	handler->opaq = opaq;
}
/* }}} */

/* {{{ SUCCESS|FAILURE php_output_handler_start(php_output_handler *handler TSRMLS_DC)
 * Starts the set up output handler and pushes it on top of the stack. Checks for any conflicts regarding the output handler to start */
PHPAPI int php_output_handler_start(php_output_handler *handler TSRMLS_DC)
{
	HashPosition pos;
	HashTable *rconflicts;
	php_output_handler_conflict_check_t *conflict;

	if (php_output_lock_error(PHP_OUTPUT_HANDLER_START TSRMLS_CC) || !handler) {
		return FAILURE;
	}
	if (SUCCESS == zend_hash_find(&php_output_handler_conflicts, handler->name, handler->name_len+1, (void *) &conflict)) {
		if (SUCCESS != (*conflict)(handler->name, handler->name_len TSRMLS_CC)) {
			return FAILURE;
		}
	}
	if (SUCCESS == zend_hash_find(&php_output_handler_reverse_conflicts, handler->name, handler->name_len+1, (void *) &rconflicts)) {
		for (zend_hash_internal_pointer_reset_ex(rconflicts, &pos);
			zend_hash_get_current_data_ex(rconflicts, (void *) &conflict, &pos) == SUCCESS;
			zend_hash_move_forward_ex(rconflicts, &pos)
		) {
			if (SUCCESS != (*conflict)(handler->name, handler->name_len TSRMLS_CC)) {
				return FAILURE;
			}
		}
	}
	/* zend_stack_push never returns SUCCESS but FAILURE or stack level */
	if (FAILURE == (handler->level = zend_stack_push(&OG(handlers), &handler, sizeof(php_output_handler *)))) {
		return FAILURE;
	}
	OG(active) = handler;
	return SUCCESS;
}
/* }}} */

/* {{{ int php_output_handler_started(zval *name TSRMLS_DC)
 * Check whether a certain output handler is in use */
PHPAPI int php_output_handler_started(const char *name, size_t name_len TSRMLS_DC)
{
	php_output_handler ***handlers;
	int i, count = php_output_get_level(TSRMLS_C);

	if (count) {
		handlers = (php_output_handler ***) zend_stack_base(&OG(handlers));

		for (i = 0; i < count; ++i) {
			if (name_len == (*(handlers[i]))->name_len && !memcmp((*(handlers[i]))->name, name, name_len)) {
				return 1;
			}
		}
	}

	return 0;
}
/* }}} */

/* {{{ int php_output_handler_conflict(zval *handler_new, zval *handler_old TSRMLS_DC)
 * Check whether a certain handler is in use and issue a warning that the new handler would conflict with the already used one */
PHPAPI int php_output_handler_conflict(const char *handler_new, size_t handler_new_len, const char *handler_set, size_t handler_set_len TSRMLS_DC)
{
	if (php_output_handler_started(handler_set, handler_set_len TSRMLS_CC)) {
		if (handler_new_len != handler_set_len || memcmp(handler_new, handler_set, handler_set_len)) {
			php_error_docref("ref.outcontrol" TSRMLS_CC, E_WARNING, "output handler '%s' conflicts with '%s'", handler_new, handler_set);
		} else {
			php_error_docref("ref.outcontrol" TSRMLS_CC, E_WARNING, "output handler '%s' cannot be used twice", handler_new);
		}
		return 1;
	}
	return 0;
}
/* }}} */

/* {{{ SUCCESS|FAILURE php_output_handler_conflict_register(zval *name, php_output_handler_conflict_check_t check_func TSRMLS_DC)
 * Register a conflict checking function on MINIT */
PHPAPI int php_output_handler_conflict_register(const char *name, size_t name_len, php_output_handler_conflict_check_t check_func TSRMLS_DC)
{
	if (!EG(current_module)) {
		zend_error(E_ERROR, "Cannot register an output handler conflict outside of MINIT");
		return FAILURE;
	}
	return zend_hash_update(&php_output_handler_conflicts, name, name_len+1, &check_func, sizeof(php_output_handler_conflict_check_t *), NULL);
}
/* }}} */

/* {{{ SUCCESS|FAILURE php_output_handler_reverse_conflict_register(zval *name, php_output_handler_conflict_check_t check_func TSRMLS_DC)
 * Register a reverse conflict checking function on MINIT */
PHPAPI int php_output_handler_reverse_conflict_register(const char *name, size_t name_len, php_output_handler_conflict_check_t check_func TSRMLS_DC)
{
	HashTable rev, *rev_ptr = NULL;

	if (!EG(current_module)) {
		zend_error(E_ERROR, "Cannot register a reverse output handler conflict outside of MINIT");
		return FAILURE;
	}

	if (SUCCESS == zend_hash_find(&php_output_handler_reverse_conflicts, name, name_len+1, (void *) &rev_ptr)) {
		return zend_hash_next_index_insert(rev_ptr, &check_func, sizeof(php_output_handler_conflict_check_t *), NULL);
	} else {
		zend_hash_init(&rev, 1, NULL, NULL, 1);
		if (SUCCESS != zend_hash_next_index_insert(&rev, &check_func, sizeof(php_output_handler_conflict_check_t *), NULL)) {
			zend_hash_destroy(&rev);
			return FAILURE;
		}
		if (SUCCESS != zend_hash_update(&php_output_handler_reverse_conflicts, name, name_len+1, &rev, sizeof(HashTable), NULL)) {
			zend_hash_destroy(&rev);
			return FAILURE;
		}
		return SUCCESS;
	}
}
/* }}} */

/* {{{ php_output_handler_alias_ctor_t php_output_handler_alias(zval *name TSRMLS_DC)
 * Get an internal output handler for a user handler if it exists */
PHPAPI php_output_handler_alias_ctor_t *php_output_handler_alias(const char *name, size_t name_len TSRMLS_DC)
{
	php_output_handler_alias_ctor_t *func = NULL;

	zend_hash_find(&php_output_handler_aliases, name, name_len+1, (void *) &func);
	return func;
}
/* }}} */

/* {{{ SUCCESS|FAILURE php_output_handler_alias_register(zval *name, php_output_handler_alias_ctor_t func TSRMLS_DC)
 * Registers an internal output handler as alias for a user handler */
PHPAPI int php_output_handler_alias_register(const char *name, size_t name_len, php_output_handler_alias_ctor_t func TSRMLS_DC)
{
	if (!EG(current_module)) {
		zend_error(E_ERROR, "Cannot register an output handler alias outside of MINIT");
		return FAILURE;
	}
	return zend_hash_update(&php_output_handler_aliases, name, name_len+1, &func, sizeof(php_output_handler_alias_ctor_t *), NULL);
}
/* }}} */

/* {{{ SUCCESS|FAILURE php_output_handler_hook(php_output_handler_hook_t type, void *arg TSMRLS_DC)
 * Output handler hook for output handler functions to check/modify the current handlers abilities */
PHPAPI int php_output_handler_hook(php_output_handler_hook_t type, void *arg TSRMLS_DC)
{
	if (OG(running)) {
		switch (type) {
			case PHP_OUTPUT_HANDLER_HOOK_GET_OPAQ:
				*(void ***) arg = &OG(running)->opaq;
				return SUCCESS;
			case PHP_OUTPUT_HANDLER_HOOK_GET_FLAGS:
				*(int *) arg = OG(running)->flags;
				return SUCCESS;
			case PHP_OUTPUT_HANDLER_HOOK_GET_LEVEL:
				*(int *) arg = OG(running)->level;
                return SUCCESS;
			case PHP_OUTPUT_HANDLER_HOOK_IMMUTABLE:
				OG(running)->flags &= ~(PHP_OUTPUT_HANDLER_REMOVABLE|PHP_OUTPUT_HANDLER_CLEANABLE);
				return SUCCESS;
			case PHP_OUTPUT_HANDLER_HOOK_DISABLE:
				OG(running)->flags |= PHP_OUTPUT_HANDLER_DISABLED;
				return SUCCESS;
			default:
				break;
		}
	}
	return FAILURE;
}
/* }}} */

/* {{{ void php_output_handler_dtor(php_output_handler *handler TSRMLS_DC)
 * Destroy an output handler */
PHPAPI void php_output_handler_dtor(php_output_handler *handler TSRMLS_DC)
{
	STR_FREE(handler->name);
	STR_FREE(handler->buffer.data);
	if (handler->flags & PHP_OUTPUT_HANDLER_USER) {
		zval_ptr_dtor(&handler->func.user->zoh);
		efree(handler->func.user);
	}
	if (handler->dtor && handler->opaq) {
		handler->dtor(handler->opaq TSRMLS_CC);
	}
	memset(handler, 0, sizeof(*handler));
}
/* }}} */

/* {{{ void php_output_handler_free(php_output_handler **handler TSMRLS_DC)
 * Destroy and free an output handler */
PHPAPI void php_output_handler_free(php_output_handler **h TSRMLS_DC)
{
	if (*h) {
		php_output_handler_dtor(*h TSRMLS_CC);
		efree(*h);
		*h = NULL;
	}
}
/* }}} */

/* void php_output_set_implicit_flush(int enabled TSRMLS_DC)
 * Enable or disable implicit flush */
PHPAPI void php_output_set_implicit_flush(int flush TSRMLS_DC)
{
	if (flush) {
		OG(flags) |= PHP_OUTPUT_IMPLICITFLUSH;
	} else {
		OG(flags) &= ~PHP_OUTPUT_IMPLICITFLUSH;
	}
}
/* }}} */

/* {{{ char *php_output_get_start_filename(TSRMLS_D)
 * Get the file name where output has started */
PHPAPI const char *php_output_get_start_filename(TSRMLS_D)
{
	return OG(output_start_filename);
}
/* }}} */

/* {{{ int php_output_get_start_lineno(TSRMLS_D)
 * Get the line number where output has started */
PHPAPI int php_output_get_start_lineno(TSRMLS_D)
{
	return OG(output_start_lineno);
}
/* }}} */

/* {{{ static int php_output_lock_error(int op TSRMLS_DC)
 * Checks whether an unallowed operation is attempted from within the output handler and issues a fatal error */
static inline int php_output_lock_error(int op TSRMLS_DC)
{
	/* if there's no ob active, ob has been stopped */
	if (op && OG(active) && OG(running)) {
		/* fatal error */
		php_output_deactivate(TSRMLS_C);
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_ERROR, "Cannot use output buffering in output buffering display handlers");
		return 1;
	}
	return 0;
}
/* }}} */

/* {{{ static php_output_context *php_output_context_init(php_output_context *context, int op TSRMLS_DC)
 * Initialize a new output context */
static inline php_output_context *php_output_context_init(php_output_context *context, int op TSRMLS_DC)
{
	if (!context) {
		context = emalloc(sizeof(php_output_context));
	}

	memset(context, 0, sizeof(php_output_context));
	TSRMLS_SET_CTX(context->tsrm_ls);
	context->op = op;

	return context;
}
/* }}} */

/* {{{ static void php_output_context_reset(php_output_context *context)
 * Reset an output context */
static inline void php_output_context_reset(php_output_context *context)
{
	int op = context->op;
	php_output_context_dtor(context);
	memset(context, 0, sizeof(php_output_context));
	context->op = op;
}
/* }}} */

/* {{{ static void php_output_context_feed(php_output_context *context, char *, size_t, size_t)
 * Feed output contexts input buffer */
static inline void php_output_context_feed(php_output_context *context, char *data, size_t size, size_t used, zend_bool free)
{
	if (context->in.free && context->in.data) {
		efree(context->in.data);
	}
	context->in.data = data;
	context->in.used = used;
	context->in.free = free;
	context->in.size = size;
}
/* }}} */

/* {{{ static void php_output_context_swap(php_output_context *context)
 * Swap output contexts buffers */
static inline void php_output_context_swap(php_output_context *context)
{
	if (context->in.free && context->in.data) {
		efree(context->in.data);
	}
	context->in.data = context->out.data;
	context->in.used = context->out.used;
	context->in.free = context->out.free;
	context->in.size = context->out.size;
	context->out.data = NULL;
	context->out.used = 0;
	context->out.free = 0;
	context->out.size = 0;
}
/* }}} */

/* {{{ static void php_output_context_pass(php_output_context *context)
 * Pass input to output buffer */
static inline void php_output_context_pass(php_output_context *context)
{
	context->out.data = context->in.data;
	context->out.used = context->in.used;
	context->out.size = context->in.size;
	context->out.free = context->in.free;
	context->in.data = NULL;
	context->in.used = 0;
	context->in.free = 0;
	context->in.size = 0;
}
/* }}} */

/* {{{ static void php_output_context_dtor(php_output_context *context)
 * Destroy the contents of an output context */
static inline void php_output_context_dtor(php_output_context *context)
{
	if (context->in.free && context->in.data) {
		efree(context->in.data);
		context->in.data = NULL;
	}
	if (context->out.free && context->out.data) {
		efree(context->out.data);
		context->out.data = NULL;
	}
}
/* }}} */

/* {{{ static php_output_handler *php_output_handler_init(zval *name, size_t chunk_size, int flags TSRMLS_DC)
 * Allocates and initializes a php_output_handler structure */
static inline php_output_handler *php_output_handler_init(const char *name, size_t name_len, size_t chunk_size, int flags TSRMLS_DC)
{
	php_output_handler *handler;

	handler = ecalloc(1, sizeof(php_output_handler));
	handler->name = estrndup(name, name_len);
	handler->name_len = name_len;
	handler->size = chunk_size;
	handler->flags = flags;
	handler->buffer.size = PHP_OUTPUT_HANDLER_INITBUF_SIZE(chunk_size);
	handler->buffer.data = emalloc(handler->buffer.size);

	return handler;
}
/* }}} */

/* {{{ static int php_output_handler_appen(php_output_handler *handler, const php_output_buffer *buf TSRMLS_DC)
 * Appends input to the output handlers buffer and indicates whether the buffer does not have to be processed by the output handler */
static inline int php_output_handler_append(php_output_handler *handler, const php_output_buffer *buf TSRMLS_DC)
{
	if (buf->used) {
		OG(flags) |= PHP_OUTPUT_WRITTEN;
		/* store it away */
		if ((handler->buffer.size - handler->buffer.used) <= buf->used) {
			size_t grow_int = PHP_OUTPUT_HANDLER_INITBUF_SIZE(handler->size);
			size_t grow_buf = PHP_OUTPUT_HANDLER_INITBUF_SIZE(buf->used - (handler->buffer.size - handler->buffer.used));
			size_t grow_max = MAX(grow_int, grow_buf);

			handler->buffer.data = erealloc(handler->buffer.data, handler->buffer.size + grow_max);
			handler->buffer.size += grow_max;
		}
		memcpy(handler->buffer.data + handler->buffer.used, buf->data, buf->used);
		handler->buffer.used += buf->used;

		/* chunked buffering */
		if (handler->size && (handler->buffer.used >= handler->size)) {
			/* store away errors and/or any intermediate output */
			return OG(running) ? 1 : 0;
		}
	}
	return 1;
}
/* }}} */

/* {{{ static php_output_handler_status_t php_output_handler_op(php_output_handler *handler, php_output_context *context)
 * Output handler operation dispatcher, applying context op to the php_output_handler handler */
static inline php_output_handler_status_t php_output_handler_op(php_output_handler *handler, php_output_context *context)
{
	php_output_handler_status_t status;
	int original_op = context->op;
	PHP_OUTPUT_TSRMLS(context);

#if PHP_OUTPUT_DEBUG
	fprintf(stderr, ">>> op(%d, "
					"handler=%p, "
					"name=%s, "
					"flags=%d, "
					"buffer.data=%s, "
					"buffer.used=%zu, "
					"buffer.size=%zu, "
					"in.data=%s, "
					"in.used=%zu)\n",
			context->op,
			handler,
			handler->name,
			handler->flags,
			handler->buffer.used?handler->buffer.data:"",
			handler->buffer.used,
			handler->buffer.size,
			context->in.used?context->in.data:"",
			context->in.used
	);
#endif

	if (php_output_lock_error(context->op TSRMLS_CC)) {
		/* fatal error */
		return PHP_OUTPUT_HANDLER_FAILURE;
	}

	/* storable? */
	if (php_output_handler_append(handler, &context->in TSRMLS_CC) && !context->op) {
		context->op = original_op;
		return PHP_OUTPUT_HANDLER_NO_DATA;
	} else {
		/* need to start? */
		if (!(handler->flags & PHP_OUTPUT_HANDLER_STARTED)) {
			context->op |= PHP_OUTPUT_HANDLER_START;
		}

		OG(running) = handler;
		if (handler->flags & PHP_OUTPUT_HANDLER_USER) {
			zval *retval = NULL, *ob_data, *ob_mode;

			MAKE_STD_ZVAL(ob_data);
			ZVAL_STRINGL(ob_data, handler->buffer.data, handler->buffer.used, 1);
			MAKE_STD_ZVAL(ob_mode);
			ZVAL_LONG(ob_mode, (long) context->op);
			zend_fcall_info_argn(&handler->func.user->fci TSRMLS_CC, 2, &ob_data, &ob_mode);

#define PHP_OUTPUT_USER_SUCCESS(retval) (retval && !(Z_TYPE_P(retval) == IS_BOOL && Z_BVAL_P(retval)==0))
			if (SUCCESS == zend_fcall_info_call(&handler->func.user->fci, &handler->func.user->fcc, &retval, NULL TSRMLS_CC) && PHP_OUTPUT_USER_SUCCESS(retval)) {
				/* user handler may have returned TRUE */
				status = PHP_OUTPUT_HANDLER_NO_DATA;
				if (Z_TYPE_P(retval) != IS_BOOL) {
					convert_to_string_ex(&retval);
					if (Z_STRLEN_P(retval)) {
						context->out.data = estrndup(Z_STRVAL_P(retval), Z_STRLEN_P(retval));
						context->out.used = Z_STRLEN_P(retval);
						context->out.free = 1;
						status = PHP_OUTPUT_HANDLER_SUCCESS;
					}
				}
			} else {
				/* call failed, pass internal buffer along */
				status = PHP_OUTPUT_HANDLER_FAILURE;
			}

			zend_fcall_info_argn(&handler->func.user->fci TSRMLS_CC, 0);
			zval_ptr_dtor(&ob_data);
			zval_ptr_dtor(&ob_mode);
			if (retval) {
				zval_ptr_dtor(&retval);
			}

		} else {

			php_output_context_feed(context, handler->buffer.data, handler->buffer.size, handler->buffer.used, 0);

			if (SUCCESS == handler->func.internal(&handler->opaq, context)) {
				if (context->out.used) {
					status = PHP_OUTPUT_HANDLER_SUCCESS;
				} else {
					status = PHP_OUTPUT_HANDLER_NO_DATA;
				}
			} else {
				status = PHP_OUTPUT_HANDLER_FAILURE;
			}
		}
		handler->flags |= PHP_OUTPUT_HANDLER_STARTED;
		OG(running) = NULL;
	}

	switch (status) {
		case PHP_OUTPUT_HANDLER_FAILURE:
			/* disable this handler */
			handler->flags |= PHP_OUTPUT_HANDLER_DISABLED;
			/* discard any output */
			if (context->out.data && context->out.free) {
				efree(context->out.data);
			}
			/* returns handlers buffer */
			context->out.data = handler->buffer.data;
			context->out.used = handler->buffer.used;
			context->out.free = 1;
			handler->buffer.data = NULL;
			handler->buffer.used = 0;
			handler->buffer.size = 0;
			break;
		case PHP_OUTPUT_HANDLER_NO_DATA:
			/* handler ate all */
			php_output_context_reset(context);
			/* no break */
		case PHP_OUTPUT_HANDLER_SUCCESS:
			/* no more buffered data */
			handler->buffer.used = 0;
			handler->flags |= PHP_OUTPUT_HANDLER_PROCESSED;
			break;
	}

	context->op = original_op;
	return status;
}
/* }}} */


/* {{{ static void php_output_op(int op, const char *str, size_t len TSRMLS_DC)
 * Output op dispatcher, passes input and output handlers output through the output handler stack until it gets written to the SAPI */
static inline void php_output_op(int op, const char *str, size_t len TSRMLS_DC)
{
	php_output_context context;
	php_output_handler **active;
	int obh_cnt;

	if (php_output_lock_error(op TSRMLS_CC)) {
		return;
	}

	php_output_context_init(&context, op TSRMLS_CC);

	/*
	 * broken up for better performance:
	 *  - apply op to the one active handler; note that OG(active) might be popped off the stack on a flush
	 *  - or apply op to the handler stack
	 */
	if (OG(active) && (obh_cnt = zend_stack_count(&OG(handlers)))) {
		context.in.data = (char *) str;
		context.in.used = len;

		if (obh_cnt > 1) {
			zend_stack_apply_with_argument(&OG(handlers), ZEND_STACK_APPLY_TOPDOWN, php_output_stack_apply_op, &context);
		} else if ((SUCCESS == zend_stack_top(&OG(handlers), (void *) &active)) && (!((*active)->flags & PHP_OUTPUT_HANDLER_DISABLED))) {
			php_output_handler_op(*active, &context);
		} else {
			php_output_context_pass(&context);
		}
	} else {
		context.out.data = (char *) str;
		context.out.used = len;
	}

	if (context.out.data && context.out.used) {
		php_output_header(TSRMLS_C);

		if (!(OG(flags) & PHP_OUTPUT_DISABLED)) {
#if PHP_OUTPUT_DEBUG
			fprintf(stderr, "::: sapi_write('%s', %zu)\n", context.out.data, context.out.used);
#endif
			sapi_module.ub_write(context.out.data, context.out.used TSRMLS_CC);

			if (OG(flags) & PHP_OUTPUT_IMPLICITFLUSH) {
				sapi_flush(TSRMLS_C);
			}

			OG(flags) |= PHP_OUTPUT_SENT;
		}
	}
	php_output_context_dtor(&context);
}
/* }}} */

/* {{{ static int php_output_stack_apply_op(void *h, void *c)
 * Operation callback for the stack apply function */
static int php_output_stack_apply_op(void *h, void *c)
{
	int was_disabled;
	php_output_handler_status_t status;
	php_output_handler *handler = *(php_output_handler **) h;
	php_output_context *context = (php_output_context *) c;

	if ((was_disabled = (handler->flags & PHP_OUTPUT_HANDLER_DISABLED))) {
		status = PHP_OUTPUT_HANDLER_FAILURE;
	} else {
		status = php_output_handler_op(handler, context);
	}

	/*
	 * handler ate all => break
	 * handler returned data or failed resp. is disabled => continue
	 */
	switch (status) {
		case PHP_OUTPUT_HANDLER_NO_DATA:
			return 1;

		case PHP_OUTPUT_HANDLER_SUCCESS:
			/* swap contexts buffers, unless this is the last handler in the stack */
			if (handler->level) {
				php_output_context_swap(context);
			}
			return 0;

		case PHP_OUTPUT_HANDLER_FAILURE:
		default:
			if (was_disabled) {
				/* pass input along, if it's the last handler in the stack */
				if (!handler->level) {
					php_output_context_pass(context);
				}
			} else {
				/* swap buffers, unless this is the last handler */
				if (handler->level) {
					php_output_context_swap(context);
				}
			}
			return 0;
	}
}
/* }}} */

/* {{{ static int php_output_stack_apply_clean(void *h, void *c)
 * Clean callback for the stack apply function */
static int php_output_stack_apply_clean(void *h, void *c)
{
	php_output_handler *handler = *(php_output_handler **) h;
	php_output_context *context = (php_output_context *) c;

	handler->buffer.used = 0;
	php_output_handler_op(handler, context);
	php_output_context_reset(context);
	return 0;
}
/* }}} */

/* {{{ static int php_output_stack_apply_list(void *h, void *z)
 * List callback for the stack apply function */
static int php_output_stack_apply_list(void *h, void *z)
{
	php_output_handler *handler = *(php_output_handler **) h;
	zval *array = (zval *) z;

	add_next_index_stringl(array, handler->name, handler->name_len, 1);
	return 0;
}
/* }}} */

/* {{{ static int php_output_stack_apply_status(void *h, void *z)
 * Status callback for the stack apply function */
static int php_output_stack_apply_status(void *h, void *z)
{
	php_output_handler *handler = *(php_output_handler **) h;
	zval *array = (zval *) z;

	add_next_index_zval(array, php_output_handler_status(handler, NULL));

	return 0;
}

/* {{{ static zval *php_output_handler_status(php_output_handler *handler, zval *entry)
 * Returns an array with the status of the output handler */
static inline zval *php_output_handler_status(php_output_handler *handler, zval *entry)
{
	if (!entry) {
		MAKE_STD_ZVAL(entry);
		array_init(entry);
	}

	add_assoc_stringl(entry, "name", handler->name, handler->name_len, 1);
	add_assoc_long(entry, "type", (long) (handler->flags & 0xf));
	add_assoc_long(entry, "flags", (long) handler->flags);
	add_assoc_long(entry, "level", (long) handler->level);
	add_assoc_long(entry, "chunk_size", (long) handler->size);
	add_assoc_long(entry, "buffer_size", (long) handler->buffer.size);
	add_assoc_long(entry, "buffer_used", (long) handler->buffer.used);

	return entry;
}
/* }}} */

/* {{{ static int php_output_stack_pop(int flags TSRMLS_DC)
 * Pops an output handler off the stack */
static inline int php_output_stack_pop(int flags TSRMLS_DC)
{
	php_output_context context;
	php_output_handler **current, *orphan = OG(active);

	if (!orphan) {
		if (!(flags & PHP_OUTPUT_POP_SILENT)) {
			php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to %s buffer. No buffer to %s", (flags&PHP_OUTPUT_POP_DISCARD)?"discard":"send", (flags&PHP_OUTPUT_POP_DISCARD)?"discard":"send");
		}
		return 0;
	} else if (!(flags & PHP_OUTPUT_POP_FORCE) && !(orphan->flags & PHP_OUTPUT_HANDLER_REMOVABLE)) {
		if (!(flags & PHP_OUTPUT_POP_SILENT)) {
			php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to %s buffer of %s (%d)", (flags&PHP_OUTPUT_POP_DISCARD)?"discard":"send", orphan->name, orphan->level);
		}
		return 0;
	} else {
		php_output_context_init(&context, PHP_OUTPUT_HANDLER_FINAL TSRMLS_CC);

		/* don't run the output handler if it's disabled */
		if (!(orphan->flags & PHP_OUTPUT_HANDLER_DISABLED)) {
			/* didn't it start yet? */
			if (!(orphan->flags & PHP_OUTPUT_HANDLER_STARTED)) {
				context.op |= PHP_OUTPUT_HANDLER_START;
			}
			/* signal that we're cleaning up */
			if (flags & PHP_OUTPUT_POP_DISCARD) {
				context.op |= PHP_OUTPUT_HANDLER_CLEAN;
			}
			php_output_handler_op(orphan, &context);
		}

		/* pop it off the stack */
		zend_stack_del_top(&OG(handlers));
		if (SUCCESS == zend_stack_top(&OG(handlers), (void *) &current)) {
			OG(active) = *current;
		} else {
			OG(active) = NULL;
		}

		/* pass output along */
		if (context.out.data && context.out.used && !(flags & PHP_OUTPUT_POP_DISCARD)) {
			php_output_write(context.out.data, context.out.used TSRMLS_CC);
		}

		/* destroy the handler (after write!) */
		php_output_handler_free(&orphan TSRMLS_CC);
		php_output_context_dtor(&context);

		return 1;
	}
}
/* }}} */

/* {{{ static SUCCESS|FAILURE php_output_handler_compat_func(void *ctx, php_output_context *)
 * php_output_handler_context_func_t for php_output_handler_func_t output handlers */
static int php_output_handler_compat_func(void **handler_context, php_output_context *output_context)
{
	php_output_handler_func_t func = *(php_output_handler_func_t *) handler_context;
	PHP_OUTPUT_TSRMLS(output_context);

	if (func) {
		char *out_str = NULL;
		uint out_len = 0;

		func(output_context->in.data, output_context->in.used, &out_str, &out_len, output_context->op TSRMLS_CC);

		if (out_str) {
			output_context->out.data = out_str;
			output_context->out.used = out_len;
			output_context->out.free = 1;
		} else {
			php_output_context_pass(output_context);
		}

		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

/* {{{ static SUCCESS|FAILURE php_output_handler_default_func(void *ctx, php_output_context *)
 * Default output handler */
static int php_output_handler_default_func(void **handler_context, php_output_context *output_context)
{
	php_output_context_pass(output_context);
	return SUCCESS;
}
/* }}} */

/* {{{ static SUCCESS|FAILURE php_output_handler_devnull_func(void *ctx, php_output_context *)
 * Null output handler */
static int php_output_handler_devnull_func(void **handler_context, php_output_context *output_context)
{
	return SUCCESS;
}
/* }}} */

/*
 * USERLAND (nearly 1:1 of old output.c)
 */

/* {{{ proto bool ob_start([string|array user_function [, int chunk_size [, int flags]]])
   Turn on Output Buffering (specifying an optional output handler). */
PHP_FUNCTION(ob_start)
{
	zval *output_handler = NULL;
	long chunk_size = 0;
	long flags = PHP_OUTPUT_HANDLER_STDFLAGS;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z/ll", &output_handler, &chunk_size, &flags) == FAILURE) {
		return;
	}

	if (chunk_size < 0) {
		chunk_size = 0;
	}

	if (php_output_start_user(output_handler, chunk_size, flags TSRMLS_CC) == FAILURE) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to create buffer");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ob_flush(void)
   Flush (send) contents of the output buffer. The last buffer content is sent to next buffer */
PHP_FUNCTION(ob_flush)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!OG(active)) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to flush buffer. No buffer to flush");
		RETURN_FALSE;
	}

	if (SUCCESS != php_output_flush(TSRMLS_C)) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to flush buffer of %s (%d)", OG(active)->name, OG(active)->level);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ob_clean(void)
   Clean (delete) the current output buffer */
PHP_FUNCTION(ob_clean)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!OG(active)) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete buffer. No buffer to delete");
		RETURN_FALSE;
	}

	if (SUCCESS != php_output_clean(TSRMLS_C)) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete buffer of %s (%d)", OG(active)->name, OG(active)->level);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ob_end_flush(void)
   Flush (send) the output buffer, and delete current output buffer */
PHP_FUNCTION(ob_end_flush)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!OG(active)) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete and flush buffer. No buffer to delete or flush");
		RETURN_FALSE;
	}

	RETURN_BOOL(SUCCESS == php_output_end(TSRMLS_C));
}
/* }}} */

/* {{{ proto bool ob_end_clean(void)
   Clean the output buffer, and delete current output buffer */
PHP_FUNCTION(ob_end_clean)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!OG(active)) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete buffer. No buffer to delete");
		RETURN_FALSE;
	}

	RETURN_BOOL(SUCCESS == php_output_discard(TSRMLS_C));
}
/* }}} */

/* {{{ proto bool ob_get_flush(void)
   Get current buffer contents, flush (send) the output buffer, and delete current output buffer */
PHP_FUNCTION(ob_get_flush)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (php_output_get_contents(return_value TSRMLS_CC) == FAILURE) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete and flush buffer. No buffer to delete or flush");
		RETURN_FALSE;
	}

	if (SUCCESS != php_output_end(TSRMLS_C)) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete buffer of %s (%d)", OG(active)->name, OG(active)->level);
	}
}
/* }}} */

/* {{{ proto bool ob_get_clean(void)
   Get current buffer contents and delete current output buffer */
PHP_FUNCTION(ob_get_clean)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if(!OG(active)) {
		RETURN_FALSE;
	}

	if (php_output_get_contents(return_value TSRMLS_CC) == FAILURE) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete buffer. No buffer to delete");
		RETURN_FALSE;
	}

	if (SUCCESS != php_output_discard(TSRMLS_C)) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete buffer of %s (%d)", OG(active)->name, OG(active)->level);
	}
}
/* }}} */

/* {{{ proto string ob_get_contents(void)
   Return the contents of the output buffer */
PHP_FUNCTION(ob_get_contents)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (php_output_get_contents(return_value TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int ob_get_level(void)
   Return the nesting level of the output buffer */
PHP_FUNCTION(ob_get_level)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(php_output_get_level(TSRMLS_C));
}
/* }}} */

/* {{{ proto int ob_get_length(void)
   Return the length of the output buffer */
PHP_FUNCTION(ob_get_length)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (php_output_get_length(return_value TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto false|array ob_list_handlers()
   List all output_buffers in an array */
PHP_FUNCTION(ob_list_handlers)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);

	if (!OG(active)) {
		return;
	}

	zend_stack_apply_with_argument(&OG(handlers), ZEND_STACK_APPLY_BOTTOMUP, php_output_stack_apply_list, return_value);
}
/* }}} */

/* {{{ proto false|array ob_get_status([bool full_status])
   Return the status of the active or all output buffers */
PHP_FUNCTION(ob_get_status)
{
	zend_bool full_status = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &full_status) == FAILURE) {
		return;
	}
	
	array_init(return_value);

	if (!OG(active)) {
		return;
	}

	if (full_status) {
		zend_stack_apply_with_argument(&OG(handlers), ZEND_STACK_APPLY_BOTTOMUP, php_output_stack_apply_status, return_value);
	} else {
		php_output_handler_status(OG(active), return_value);
	}
}
/* }}} */

/* {{{ proto void ob_implicit_flush([int flag])
   Turn implicit flush on/off and is equivalent to calling flush() after every output call */
PHP_FUNCTION(ob_implicit_flush)
{
	long flag = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &flag) == FAILURE) {
		return;
	}

	php_output_set_implicit_flush(flag TSRMLS_CC);
}
/* }}} */

/* {{{ proto bool output_reset_rewrite_vars(void)
   Reset(clear) URL rewriter values */
PHP_FUNCTION(output_reset_rewrite_vars)
{
	if (php_url_scanner_reset_vars(TSRMLS_C) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool output_add_rewrite_var(string name, string value)
   Add URL rewriter values */
PHP_FUNCTION(output_add_rewrite_var)
{
	char *name, *value;
	int name_len, value_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &name_len, &value, &value_len) == FAILURE) {
		return;
	}

	if (php_url_scanner_add_var(name, name_len, value, value_len, 1 TSRMLS_CC) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
