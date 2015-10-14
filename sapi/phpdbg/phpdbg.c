/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#if !defined(ZEND_SIGNALS) || defined(_WIN32)
# include <signal.h>
#endif

#include "phpdbg.h"
#include "phpdbg_prompt.h"
#include "phpdbg_bp.h"
#include "phpdbg_break.h"
#include "phpdbg_list.h"
#include "phpdbg_utils.h"
#include "phpdbg_set.h"
#include "phpdbg_io.h"
#include "zend_alloc.h"
#include "phpdbg_eol.h"
#include "phpdbg_print.h"

#include "ext/standard/basic_functions.h"

/* {{{ remote console headers */
#ifndef _WIN32
#	include <sys/socket.h>
#	include <sys/select.h>
#	include <sys/time.h>
#	include <sys/types.h>
#	include <sys/poll.h>
#	include <netinet/in.h>
#	include <unistd.h>
#	include <arpa/inet.h>
#endif /* }}} */

#if defined(PHP_WIN32) && defined(HAVE_OPENSSL)
# include "openssl/applink.c"
#endif

ZEND_DECLARE_MODULE_GLOBALS(phpdbg);
int phpdbg_startup_run = 0;

static PHP_INI_MH(OnUpdateEol)
{
	if (!new_value) {
		return FAILURE;
	}

	return phpdbg_eol_global_update(ZSTR_VAL(new_value));
}

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("phpdbg.path", "", PHP_INI_SYSTEM | PHP_INI_PERDIR, OnUpdateString, socket_path, zend_phpdbg_globals, phpdbg_globals)
	STD_PHP_INI_ENTRY("phpdbg.eol", "2", PHP_INI_ALL, OnUpdateEol, socket_path, zend_phpdbg_globals, phpdbg_globals)
PHP_INI_END()

static zend_bool phpdbg_booted = 0;
static zend_bool phpdbg_fully_started = 0;

static inline void php_phpdbg_globals_ctor(zend_phpdbg_globals *pg) /* {{{ */
{
	pg->prompt[0] = NULL;
	pg->prompt[1] = NULL;

	pg->colors[0] = NULL;
	pg->colors[1] = NULL;
	pg->colors[2] = NULL;

	pg->exec = NULL;
	pg->exec_len = 0;
	pg->buffer = NULL;
	pg->last_was_newline = 1;
	pg->ops = NULL;
	pg->vmret = 0;
	pg->in_execution = 0;
	pg->bp_count = 0;
	pg->flags = PHPDBG_DEFAULT_FLAGS;
	pg->oplog = NULL;
	memset(pg->io, 0, sizeof(pg->io));
	pg->frame.num = 0;
	pg->sapi_name_ptr = NULL;
	pg->socket_fd = -1;
	pg->socket_server_fd = -1;
	pg->unclean_eval = 0;

	pg->req_id = 0;
	pg->err_buf.active = 0;
	pg->err_buf.type = 0;

	pg->input_buflen = 0;
	pg->sigsafe_mem.mem = NULL;
	pg->sigsegv_bailout = NULL;

	pg->oplog_list = NULL;

#ifdef PHP_WIN32
	pg->sigio_watcher_thread = INVALID_HANDLE_VALUE;
	memset(&pg->swd, 0, sizeof(struct win32_sigio_watcher_data));
#endif

	pg->eol = PHPDBG_EOL_LF;
} /* }}} */

static PHP_MINIT_FUNCTION(phpdbg) /* {{{ */
{
	ZEND_INIT_MODULE_GLOBALS(phpdbg, php_phpdbg_globals_ctor, NULL);
	REGISTER_INI_ENTRIES();

	zend_execute_ex = phpdbg_execute_ex;

	REGISTER_STRINGL_CONSTANT("PHPDBG_VERSION", PHPDBG_VERSION, sizeof(PHPDBG_VERSION)-1, CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("PHPDBG_FILE",   FILE_PARAM, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHPDBG_METHOD", METHOD_PARAM, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHPDBG_LINENO", NUMERIC_PARAM, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHPDBG_FUNC",   STR_PARAM, CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("PHPDBG_COLOR_PROMPT", PHPDBG_COLOR_PROMPT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHPDBG_COLOR_NOTICE", PHPDBG_COLOR_NOTICE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHPDBG_COLOR_ERROR",  PHPDBG_COLOR_ERROR, CONST_CS|CONST_PERSISTENT);

	return SUCCESS;
} /* }}} */

static void php_phpdbg_destroy_bp_file(zval *brake) /* {{{ */
{
	zend_hash_destroy(Z_ARRVAL_P(brake));
	efree(Z_ARRVAL_P(brake));
} /* }}} */

static void php_phpdbg_destroy_bp_symbol(zval *brake) /* {{{ */
{
	efree((char *) ((phpdbg_breaksymbol_t *) Z_PTR_P(brake))->symbol);
	efree(Z_PTR_P(brake));
} /* }}} */

static void php_phpdbg_destroy_bp_opcode(zval *brake) /* {{{ */
{
	efree((char *) ((phpdbg_breakop_t *) Z_PTR_P(brake))->name);
	efree(Z_PTR_P(brake));
} /* }}} */

static void php_phpdbg_destroy_bp_opline(zval *brake) /* {{{ */
{
	efree(Z_PTR_P(brake));
} /* }}} */

static void php_phpdbg_destroy_bp_methods(zval *brake) /* {{{ */
{
	zend_hash_destroy(Z_ARRVAL_P(brake));
	efree(Z_ARRVAL_P(brake));
} /* }}} */

static void php_phpdbg_destroy_bp_condition(zval *data) /* {{{ */
{
	phpdbg_breakcond_t *brake = (phpdbg_breakcond_t *) Z_PTR_P(data);

	if (brake->ops) {
		destroy_op_array(brake->ops);
		efree(brake->ops);
	}
	efree((char*) brake->code);
	efree(brake);
} /* }}} */

static void php_phpdbg_destroy_registered(zval *data) /* {{{ */
{
	zend_function *function = (zend_function *) Z_PTR_P(data);
	destroy_zend_function(function);
} /* }}} */


static PHP_RINIT_FUNCTION(phpdbg) /* {{{ */
{
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], 8, NULL, php_phpdbg_destroy_bp_file, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_PENDING], 8, NULL, php_phpdbg_destroy_bp_file, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], 8, NULL, php_phpdbg_destroy_bp_symbol, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_FUNCTION_OPLINE], 8, NULL, php_phpdbg_destroy_bp_methods, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD_OPLINE], 8, NULL, php_phpdbg_destroy_bp_methods, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_OPLINE], 8, NULL, php_phpdbg_destroy_bp_methods, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], 8, NULL, php_phpdbg_destroy_bp_opline, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE], 8, NULL, php_phpdbg_destroy_bp_opcode, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD], 8, NULL, php_phpdbg_destroy_bp_methods, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_COND], 8, NULL, php_phpdbg_destroy_bp_condition, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP], 8, NULL, NULL, 0);

	zend_hash_init(&PHPDBG_G(seek), 8, NULL, NULL, 0);
	zend_hash_init(&PHPDBG_G(registered), 8, NULL, php_phpdbg_destroy_registered, 0);

	return SUCCESS;
} /* }}} */

static PHP_RSHUTDOWN_FUNCTION(phpdbg) /* {{{ */
{
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_PENDING]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_FUNCTION_OPLINE]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD_OPLINE]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_OPLINE]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_COND]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP]);
	zend_hash_destroy(&PHPDBG_G(file_sources));
	zend_hash_destroy(&PHPDBG_G(seek));
	zend_hash_destroy(&PHPDBG_G(registered));
	zend_hash_destroy(&PHPDBG_G(watchpoints));
	zend_llist_destroy(&PHPDBG_G(watchlist_mem));

	if (PHPDBG_G(buffer)) {
		free(PHPDBG_G(buffer));
		PHPDBG_G(buffer) = NULL;
	}

	if (PHPDBG_G(exec)) {
		efree(PHPDBG_G(exec));
		PHPDBG_G(exec) = NULL;
	}

	if (PHPDBG_G(oplog)) {
		fclose(PHPDBG_G(oplog));
		PHPDBG_G(oplog) = NULL;
	}

	if (PHPDBG_G(ops)) {
		destroy_op_array(PHPDBG_G(ops));
		efree(PHPDBG_G(ops));
		PHPDBG_G(ops) = NULL;
	}

	if (PHPDBG_G(oplog_list)) {
		phpdbg_oplog_list *cur = PHPDBG_G(oplog_list);
		do {
			phpdbg_oplog_list *prev = cur->prev;
			efree(cur);
			cur = prev;
		} while (cur != NULL);

		zend_arena_destroy(PHPDBG_G(oplog_arena));
		PHPDBG_G(oplog_list) = NULL;
	}

	return SUCCESS;
} /* }}} */

/* {{{ proto mixed phpdbg_exec(string context)
	Attempt to set the execution context for phpdbg
	If the execution context was set previously it is returned
	If the execution context was not set previously boolean true is returned
	If the request to set the context fails, boolean false is returned, and an E_WARNING raised */
static PHP_FUNCTION(phpdbg_exec)
{
	zend_string *exec;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &exec) == FAILURE) {
		return;
	}

	{
		zend_stat_t sb;
		zend_bool result = 1;

		if (VCWD_STAT(ZSTR_VAL(exec), &sb) != FAILURE) {
			if (sb.st_mode & (S_IFREG|S_IFLNK)) {
				if (PHPDBG_G(exec)) {
					ZVAL_STRINGL(return_value, PHPDBG_G(exec), PHPDBG_G(exec_len));
					efree(PHPDBG_G(exec));
					result = 0;
				}

				PHPDBG_G(exec) = estrndup(ZSTR_VAL(exec), ZSTR_LEN(exec));
				PHPDBG_G(exec_len) = ZSTR_LEN(exec);

				if (result) {
					ZVAL_TRUE(return_value);
				}
			} else {
				zend_error(E_WARNING, "Failed to set execution context (%s), not a regular file or symlink", ZSTR_VAL(exec));
				ZVAL_FALSE(return_value);
			}
		} else {
			zend_error(E_WARNING, "Failed to set execution context (%s) the file does not exist", ZSTR_VAL(exec));

			ZVAL_FALSE(return_value);
		}
	}
} /* }}} */

/* {{{ proto void phpdbg_break()
    instructs phpdbg to insert a breakpoint at the next opcode */
static PHP_FUNCTION(phpdbg_break_next)
{
	zend_execute_data *ex = EG(current_execute_data);

	while (ex && ex->func && !ZEND_USER_CODE(ex->func->type)) {
		ex = ex->prev_execute_data;
	}

	if (zend_parse_parameters_none() == FAILURE || !ex) {
		return;
	}

	phpdbg_set_breakpoint_opline_ex((phpdbg_opline_ptr_t) ex->opline + 1);
} /* }}} */

/* {{{ proto void phpdbg_break_file(string file, integer line) */
static PHP_FUNCTION(phpdbg_break_file)
{
	char *file;
	size_t flen;
	zend_long line;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl", &file, &flen, &line) == FAILURE) {
		return;
	}

	phpdbg_set_breakpoint_file(file, line);
} /* }}} */

/* {{{ proto void phpdbg_break_method(string class, string method) */
static PHP_FUNCTION(phpdbg_break_method)
{
	char *class = NULL, *method = NULL;
	size_t clen = 0, mlen = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &class, &clen, &method, &mlen) == FAILURE) {
		return;
	}

	phpdbg_set_breakpoint_method(class, method);
} /* }}} */

/* {{{ proto void phpdbg_break_function(string function) */
static PHP_FUNCTION(phpdbg_break_function)
{
	char    *function = NULL;
	size_t   function_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &function, &function_len) == FAILURE) {
		return;
	}

	phpdbg_set_breakpoint_symbol(function, function_len);
} /* }}} */

/* {{{ proto void phpdbg_clear(void)
   instructs phpdbg to clear breakpoints */
static PHP_FUNCTION(phpdbg_clear)
{
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_PENDING]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_FUNCTION_OPLINE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD_OPLINE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_OPLINE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_COND]);
} /* }}} */

/* {{{ proto void phpdbg_color(integer element, string color) */
static PHP_FUNCTION(phpdbg_color)
{
	zend_long element;
	char *color;
	size_t color_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ls", &element, &color, &color_len) == FAILURE) {
		return;
	}

	switch (element) {
		case PHPDBG_COLOR_NOTICE:
		case PHPDBG_COLOR_ERROR:
		case PHPDBG_COLOR_PROMPT:
			phpdbg_set_color_ex(element, color, color_len);
		break;

		default: zend_error(E_ERROR, "phpdbg detected an incorrect color constant");
	}
} /* }}} */

/* {{{ proto void phpdbg_prompt(string prompt) */
static PHP_FUNCTION(phpdbg_prompt)
{
	char *prompt = NULL;
	size_t prompt_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &prompt, &prompt_len) == FAILURE) {
		return;
	}

	phpdbg_set_prompt(prompt);
} /* }}} */

/* {{{ proto void phpdbg_start_oplog() */
static PHP_FUNCTION(phpdbg_start_oplog)
{
	phpdbg_oplog_list *prev;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	prev = PHPDBG_G(oplog_list);

	if (!prev) {
		PHPDBG_G(oplog_arena) = zend_arena_create(64 * 1024);

		PHPDBG_G(oplog_cur) = ((phpdbg_oplog_entry *) zend_arena_alloc(&PHPDBG_G(oplog_arena), sizeof(phpdbg_oplog_entry))) + 1;
		PHPDBG_G(oplog_cur)->next = NULL;
	}

	PHPDBG_G(oplog_list) = emalloc(sizeof(phpdbg_oplog_list));
	PHPDBG_G(oplog_list)->prev = prev;
	PHPDBG_G(oplog_list)->start = PHPDBG_G(oplog_cur);
}

static void phpdbg_oplog_fill_executable(zend_op_array *op_array, HashTable *insert_ht, zend_bool by_opcode) {
	/* ignore RECV_* opcodes */
	zend_op *cur = op_array->opcodes + op_array->num_args + !!(op_array->fn_flags & ZEND_ACC_VARIADIC);
	zend_op *end = op_array->opcodes + op_array->last;

	zend_long insert_idx;
	zval zero;
	ZVAL_LONG(&zero, 0);

	/* ignore autogenerated return (well, not too precise with finally branches, but that's okay) */
	if (op_array->last >= 1 && (((end - 1)->opcode == ZEND_RETURN || (end - 1)->opcode == ZEND_RETURN_BY_REF || (end - 1)->opcode == ZEND_GENERATOR_RETURN)
	 && ((op_array->last > 1 && ((end - 2)->opcode == ZEND_RETURN || (end - 2)->opcode == ZEND_RETURN_BY_REF || (end - 2)->opcode == ZEND_GENERATOR_RETURN || (end - 2)->opcode == ZEND_THROW))
	  || op_array->function_name == NULL || (end - 1)->extended_value == -1))) {
		end--;
	}

	for (; cur < end; cur++) {
		if (cur->opcode == ZEND_NOP || cur->opcode == ZEND_OP_DATA || cur->opcode == ZEND_FE_FREE || cur->opcode == ZEND_FREE || cur->opcode == ZEND_ASSERT_CHECK || cur->opcode == ZEND_VERIFY_RETURN_TYPE
		 || cur->opcode == ZEND_DECLARE_CONST || cur->opcode == ZEND_DECLARE_CLASS || cur->opcode == ZEND_DECLARE_INHERITED_CLASS || cur->opcode == ZEND_DECLARE_FUNCTION
		 || cur->opcode == ZEND_DECLARE_INHERITED_CLASS_DELAYED || cur->opcode == ZEND_VERIFY_ABSTRACT_CLASS || cur->opcode == ZEND_ADD_TRAIT || cur->opcode == ZEND_BIND_TRAITS
		 || cur->opcode == ZEND_DECLARE_ANON_CLASS || cur->opcode == ZEND_DECLARE_ANON_INHERITED_CLASS || cur->opcode == ZEND_FAST_RET || cur->opcode == ZEND_TICKS
		 || cur->opcode == ZEND_EXT_STMT || cur->opcode == ZEND_EXT_FCALL_BEGIN || cur->opcode == ZEND_EXT_FCALL_END || cur->opcode == ZEND_EXT_NOP || cur->opcode == ZEND_BIND_GLOBAL) {
			continue;
		}

		if (by_opcode) {
			insert_idx = cur - op_array->opcodes;
		} else {
			insert_idx = cur->lineno;
		}

		if (cur->opcode == ZEND_NEW && (cur + 1)->opcode == ZEND_DO_FCALL) {
			cur++;
		}

		zend_hash_index_update(insert_ht, insert_idx, &zero);
	}
}

static inline HashTable* phpdbg_add_empty_array(HashTable *ht, zend_string *name) {
	zval *ht_zv = zend_hash_find(ht, name);
	if (!ht_zv) {
		zval zv;
		array_init(&zv);
		ht_zv = zend_hash_add_new(ht, name, &zv);
	}
	return Z_ARR_P(ht_zv);
}

/* {{{ proto void phpdbg_end_oplog() */
static PHP_FUNCTION(phpdbg_get_executable)
{
	HashTable *options = NULL;
	zval *option_buffer;
	zend_bool by_function = 0;
	zend_bool by_opcode = 0;
	HashTable *insert_ht;

	zend_function *func;
	zend_class_entry *ce;
	zend_string *name;
	HashTable *files = &PHPDBG_G(file_sources);
	HashTable files_tmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|H", &options) == FAILURE) {
		return;
	}

	if (options && (option_buffer = zend_hash_str_find(options, ZEND_STRL("functions")))) {
		by_function = zend_is_true(option_buffer);
	}

	if (options && (option_buffer = zend_hash_str_find(options, ZEND_STRL("opcodes")))) {
		if (by_function) {
			by_opcode = zend_is_true(option_buffer);
		}
	}

	if (options && (option_buffer = zend_hash_str_find(options, ZEND_STRL("files")))) {
		ZVAL_DEREF(option_buffer);
		if (Z_TYPE_P(option_buffer) == IS_ARRAY && zend_hash_num_elements(Z_ARR_P(option_buffer)) > 0) {
			zval *filename;

			files = &files_tmp;
			zend_hash_init(files, 0, NULL, NULL, 0);

			ZEND_HASH_FOREACH_VAL(Z_ARR_P(option_buffer), filename) {
				zend_hash_add_empty_element(files, zval_get_string(filename));
			} ZEND_HASH_FOREACH_END();
		} else {
			GC_REFCOUNT(files)++;
		}
	} else {
		GC_REFCOUNT(files)++;
	}

	array_init(return_value);

	ZEND_HASH_FOREACH_STR_KEY_PTR(EG(function_table), name, func) {
		if (func->type == ZEND_USER_FUNCTION) {
			if (zend_hash_exists(files, func->op_array.filename)) {
				insert_ht = phpdbg_add_empty_array(Z_ARR_P(return_value), func->op_array.filename);

				if (by_function) {
					insert_ht = phpdbg_add_empty_array(insert_ht, name);
				}

				phpdbg_oplog_fill_executable(&func->op_array, insert_ht, by_opcode);
			}
		}
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_FOREACH_STR_KEY_PTR(EG(class_table), name, ce) {
		if (ce->type == ZEND_USER_CLASS) {
			if (zend_hash_exists(files, ce->info.user.filename)) {
				ZEND_HASH_FOREACH_PTR(&ce->function_table, func) {
					if (func->type == ZEND_USER_FUNCTION) {
						insert_ht = phpdbg_add_empty_array(Z_ARR_P(return_value), func->op_array.filename);

						if (by_function) {
							zend_string *fn_name = strpprintf(ZSTR_LEN(name) + ZSTR_LEN(func->op_array.function_name) + 2, "%.*s::%.*s", ZSTR_LEN(name), ZSTR_VAL(name), ZSTR_LEN(func->op_array.function_name), ZSTR_VAL(func->op_array.function_name));
							insert_ht = phpdbg_add_empty_array(insert_ht, fn_name);
							zend_string_release(fn_name);
						}

						phpdbg_oplog_fill_executable(&func->op_array, insert_ht, by_opcode);
					}
				} ZEND_HASH_FOREACH_END();
			}
		}
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_FOREACH_STR_KEY(files, name) {
		phpdbg_file_source *source = zend_hash_find_ptr(&PHPDBG_G(file_sources), name);
		if (source) {
			phpdbg_oplog_fill_executable(
				&source->op_array,
				phpdbg_add_empty_array(Z_ARR_P(return_value), source->op_array.filename),
				by_opcode);
		}
	} ZEND_HASH_FOREACH_END();

	if (!--GC_REFCOUNT(files)) {
		zend_hash_destroy(files);
	}
}

/* {{{ proto void phpdbg_end_oplog() */
static PHP_FUNCTION(phpdbg_end_oplog)
{
	phpdbg_oplog_entry *cur;
	phpdbg_oplog_list *prev;

	HashTable *options = NULL;
	zval *option_buffer;
	zend_bool by_function = 0;
	zend_bool by_opcode = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|H", &options) == FAILURE) {
		return;
	}

	if (!PHPDBG_G(oplog_list)) {
		zend_error(E_WARNING, "Can not end an oplog without starting it");
		return;
	}

	cur = PHPDBG_G(oplog_list)->start;
	prev = PHPDBG_G(oplog_list)->prev;

	efree(PHPDBG_G(oplog_list));
	PHPDBG_G(oplog_list) = prev;

	if (options && (option_buffer = zend_hash_str_find(options, ZEND_STRL("functions")))) {
		by_function = zend_is_true(option_buffer);
	}

	if (options && (option_buffer = zend_hash_str_find(options, ZEND_STRL("opcodes")))) {
		if (by_function) {
			by_opcode = zend_is_true(option_buffer);
		}
	}

	array_init(return_value);

	{
		zend_string *last_file = NULL;
		HashTable *file_ht;
		zend_string *last_function = (void *)~(uintptr_t)0;
		zend_class_entry *last_scope = NULL;

		HashTable *insert_ht;
		zend_long insert_idx;

		do {
			zval zero;
			ZVAL_LONG(&zero, 0);

			if (cur->filename != last_file) {
				last_file = cur->filename;
				file_ht = insert_ht = phpdbg_add_empty_array(Z_ARR_P(return_value), last_file);
			}

			if (by_function) {
				if (cur->function_name == NULL) {
					if (last_function != NULL) {
						insert_ht = file_ht;
					}
					last_function = NULL;
				} else if (cur->function_name != last_function || cur->scope != last_scope) {
					zend_string *fn_name;
					last_function = cur->function_name;
					last_scope = cur->scope;
					if (last_scope == NULL) {
						fn_name = zend_string_copy(last_function);
					} else {
						fn_name = strpprintf(ZSTR_LEN(last_function) + ZSTR_LEN(last_scope->name) + 2, "%.*s::%.*s", ZSTR_LEN(last_scope->name), ZSTR_VAL(last_scope->name), ZSTR_LEN(last_function), ZSTR_VAL(last_function));
					}
					insert_ht = phpdbg_add_empty_array(Z_ARR_P(return_value), fn_name);
					zend_string_release(fn_name);
				}
			}

			if (by_opcode) {
				insert_idx = cur->op - cur->opcodes;
			} else {
				insert_idx = cur->op->lineno;
			}

			{
				zval *num = zend_hash_index_find(insert_ht, insert_idx);
				if (!num) {
					num = zend_hash_index_add_new(insert_ht, insert_idx, &zero);
				}
				Z_LVAL_P(num)++;
			}

			cur = cur->next;
		} while (cur != NULL);
	}

	if (!prev) {
		zend_arena_destroy(PHPDBG_G(oplog_arena));
	}
}

ZEND_BEGIN_ARG_INFO_EX(phpdbg_break_next_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpdbg_break_file_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, file)
	ZEND_ARG_INFO(0, line)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpdbg_break_method_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, class)
	ZEND_ARG_INFO(0, method)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpdbg_break_function_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, function)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpdbg_color_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, element)
	ZEND_ARG_INFO(0, color)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpdbg_prompt_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpdbg_exec_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpdbg_clear_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpdbg_start_oplog_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpdbg_end_oplog_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpdbg_get_executable_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

zend_function_entry phpdbg_user_functions[] = {
	PHP_FE(phpdbg_clear, phpdbg_clear_arginfo)
	PHP_FE(phpdbg_break_next, phpdbg_break_next_arginfo)
	PHP_FE(phpdbg_break_file, phpdbg_break_file_arginfo)
	PHP_FE(phpdbg_break_method, phpdbg_break_method_arginfo)
	PHP_FE(phpdbg_break_function, phpdbg_break_function_arginfo)
	PHP_FE(phpdbg_exec,  phpdbg_exec_arginfo)
	PHP_FE(phpdbg_color, phpdbg_color_arginfo)
	PHP_FE(phpdbg_prompt, phpdbg_prompt_arginfo)
	PHP_FE(phpdbg_start_oplog, phpdbg_start_oplog_arginfo)
	PHP_FE(phpdbg_end_oplog, phpdbg_end_oplog_arginfo)
	PHP_FE(phpdbg_get_executable, phpdbg_get_executable_arginfo)
#ifdef  PHP_FE_END
	PHP_FE_END
#else
	{NULL,NULL,NULL}
#endif
};

static zend_module_entry sapi_phpdbg_module_entry = {
	STANDARD_MODULE_HEADER,
	PHPDBG_NAME,
	phpdbg_user_functions,
	PHP_MINIT(phpdbg),
	NULL,
	PHP_RINIT(phpdbg),
	PHP_RSHUTDOWN(phpdbg),
	NULL,
	PHPDBG_VERSION,
	STANDARD_MODULE_PROPERTIES
};

static inline int php_sapi_phpdbg_module_startup(sapi_module_struct *module) /* {{{ */
{
	if (php_module_startup(module, &sapi_phpdbg_module_entry, 1) == FAILURE) {
		return FAILURE;
	}

	phpdbg_booted=1;

	return SUCCESS;
} /* }}} */

static char* php_sapi_phpdbg_read_cookies(void) /* {{{ */
{
	return NULL;
} /* }}} */

static int php_sapi_phpdbg_header_handler(sapi_header_struct *h, sapi_header_op_enum op, sapi_headers_struct *s) /* {{{ */
{
	return 0;
}
/* }}} */

static int php_sapi_phpdbg_send_headers(sapi_headers_struct *sapi_headers) /* {{{ */
{
	/* We do nothing here, this function is needed to prevent that the fallback
	 * header handling is called. */
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}
/* }}} */

static void php_sapi_phpdbg_send_header(sapi_header_struct *sapi_header, void *server_context) /* {{{ */
{
}
/* }}} */

static void php_sapi_phpdbg_log_message(char *message) /* {{{ */
{
	/*
	* We must not request TSRM before being booted
	*/
	if (phpdbg_booted) {
		if (PHPDBG_G(flags) & PHPDBG_IN_EVAL) {
			phpdbg_error("eval", "msg=\"%s\"", "%s", message);
			return;
		}

		phpdbg_error("php", "msg=\"%s\"", "%s", message);

		if (PHPDBG_G(flags) & PHPDBG_PREVENT_INTERACTIVE) {
			return;
		}

		switch (PG(last_error_type)) {
			case E_ERROR:
			case E_CORE_ERROR:
			case E_COMPILE_ERROR:
			case E_USER_ERROR:
			case E_PARSE:
			case E_RECOVERABLE_ERROR: {
				const char *file_char = zend_get_executed_filename();
				zend_string *file = zend_string_init(file_char, strlen(file_char), 0);
				phpdbg_list_file(file, 3, zend_get_executed_lineno() - 1, zend_get_executed_lineno());
				zend_string_release(file);

				if (!phpdbg_fully_started) {
					return;
				}

				do {
					switch (phpdbg_interactive(1)) {
						case PHPDBG_LEAVE:
						case PHPDBG_FINISH:
						case PHPDBG_UNTIL:
						case PHPDBG_NEXT:
							return;
					}
				} while (!(PHPDBG_G(flags) & PHPDBG_IS_STOPPING));
			}
		}
	} else {
		fprintf(stdout, "%s\n", message);
	}
}
/* }}} */

static int php_sapi_phpdbg_deactivate(void) /* {{{ */
{
	fflush(stdout);
	if (SG(request_info).argv0) {
		free(SG(request_info).argv0);
		SG(request_info).argv0 = NULL;
	}

	return SUCCESS;
}
/* }}} */

static void php_sapi_phpdbg_register_vars(zval *track_vars_array) /* {{{ */
{
	size_t len;
	char  *docroot = "";

	/* In phpdbg mode, we consider the environment to be a part of the server variables
	*/
	php_import_environment_variables(track_vars_array);

	if (PHPDBG_G(exec)) {
		len = PHPDBG_G(exec_len);
		if (sapi_module.input_filter(PARSE_SERVER, "PHP_SELF", &PHPDBG_G(exec), PHPDBG_G(exec_len), &len)) {
			php_register_variable("PHP_SELF", PHPDBG_G(exec), track_vars_array);
		}
		if (sapi_module.input_filter(PARSE_SERVER, "SCRIPT_NAME", &PHPDBG_G(exec), PHPDBG_G(exec_len), &len)) {
			php_register_variable("SCRIPT_NAME", PHPDBG_G(exec), track_vars_array);
		}

		if (sapi_module.input_filter(PARSE_SERVER, "SCRIPT_FILENAME", &PHPDBG_G(exec), PHPDBG_G(exec_len), &len)) {
			php_register_variable("SCRIPT_FILENAME", PHPDBG_G(exec), track_vars_array);
		}
		if (sapi_module.input_filter(PARSE_SERVER, "PATH_TRANSLATED", &PHPDBG_G(exec), PHPDBG_G(exec_len), &len)) {
			php_register_variable("PATH_TRANSLATED", PHPDBG_G(exec), track_vars_array);
		}
	}

	/* any old docroot will do */
	len = 0;
	if (sapi_module.input_filter(PARSE_SERVER, "DOCUMENT_ROOT", &docroot, len, &len)) {
		php_register_variable("DOCUMENT_ROOT", docroot, track_vars_array);
	}
}
/* }}} */

static inline size_t php_sapi_phpdbg_ub_write(const char *message, size_t length) /* {{{ */
{
	if (PHPDBG_G(socket_fd) != -1 && !(PHPDBG_G(flags) & PHPDBG_IS_INTERACTIVE)) {
		send(PHPDBG_G(socket_fd), message, length, 0);
	}
	return phpdbg_script(P_STDOUT, "%.*s", length, message);
} /* }}} */

/* beginning of struct, see main/streams/plain_wrapper.c line 111 */
typedef struct {
	FILE *file;
	int fd;
} php_stdio_stream_data;

static size_t phpdbg_stdiop_write(php_stream *stream, const char *buf, size_t count) {
	php_stdio_stream_data *data = (php_stdio_stream_data*)stream->abstract;

	while (data->fd >= 0) {
		struct stat stat[3];
		memset(stat, 0, sizeof(stat));
		if (((fstat(fileno(stderr), &stat[2]) < 0) & (fstat(fileno(stdout), &stat[0]) < 0)) | (fstat(data->fd, &stat[1]) < 0)) {
			break;
		}

		if (stat[0].st_dev == stat[1].st_dev && stat[0].st_ino == stat[1].st_ino) {
			phpdbg_script(P_STDOUT, "%.*s", (int) count, buf);
			return count;
		}
		if (stat[2].st_dev == stat[1].st_dev && stat[2].st_ino == stat[1].st_ino) {
			phpdbg_script(P_STDERR, "%.*s", (int) count, buf);
			return count;
		}
		break;
	}

	return PHPDBG_G(php_stdiop_write)(stream, buf, count);
}

static inline void php_sapi_phpdbg_flush(void *context)  /* {{{ */
{
	if (!phpdbg_active_sigsafe_mem()) {
		fflush(PHPDBG_G(io)[PHPDBG_STDOUT].ptr);
	}
} /* }}} */

/* copied from sapi/cli/php_cli.c cli_register_file_handles */
static void phpdbg_register_file_handles(void) /* {{{ */
{
	zval zin, zout, zerr;
	php_stream *s_in, *s_out, *s_err;
	php_stream_context *sc_in=NULL, *sc_out=NULL, *sc_err=NULL;
	zend_constant ic, oc, ec;

	s_in  = php_stream_open_wrapper_ex("php://stdin",  "rb", 0, NULL, sc_in);
	s_out = php_stream_open_wrapper_ex("php://stdout", "wb", 0, NULL, sc_out);
	s_err = php_stream_open_wrapper_ex("php://stderr", "wb", 0, NULL, sc_err);

	if (s_in==NULL || s_out==NULL || s_err==NULL) {
		if (s_in) php_stream_close(s_in);
		if (s_out) php_stream_close(s_out);
		if (s_err) php_stream_close(s_err);
		return;
	}

#if PHP_DEBUG
	/* do not close stdout and stderr */
	s_out->flags |= PHP_STREAM_FLAG_NO_CLOSE;
	s_err->flags |= PHP_STREAM_FLAG_NO_CLOSE;
#endif

	php_stream_to_zval(s_in,  &zin);
	php_stream_to_zval(s_out, &zout);
	php_stream_to_zval(s_err, &zerr);

	ic.value = zin;
	ic.flags = CONST_CS;
	ic.name = zend_string_init(ZEND_STRL("STDIN"), 0);
	ic.module_number = 0;
	zend_register_constant(&ic);

	oc.value = zout;
	oc.flags = CONST_CS;
	oc.name = zend_string_init(ZEND_STRL("STDOUT"), 0);
	oc.module_number = 0;
	zend_register_constant(&oc);

	ec.value = zerr;
	ec.flags = CONST_CS;
	ec.name = zend_string_init(ZEND_STRL("STDERR"), 0);
	ec.module_number = 0;
	zend_register_constant(&ec);
}
/* }}} */

/* {{{ sapi_module_struct phpdbg_sapi_module
*/
static sapi_module_struct phpdbg_sapi_module = {
	"phpdbg",                       /* name */
	"phpdbg",                       /* pretty name */

	php_sapi_phpdbg_module_startup, /* startup */
	php_module_shutdown_wrapper,    /* shutdown */

	NULL,                           /* activate */
	php_sapi_phpdbg_deactivate,     /* deactivate */

	php_sapi_phpdbg_ub_write,       /* unbuffered write */
	php_sapi_phpdbg_flush,          /* flush */
	NULL,                           /* get uid */
	NULL,                           /* getenv */

	php_error,                      /* error handler */

	php_sapi_phpdbg_header_handler, /* header handler */
	php_sapi_phpdbg_send_headers,   /* send headers handler */
	php_sapi_phpdbg_send_header,    /* send header handler */

	NULL,                           /* read POST data */
	php_sapi_phpdbg_read_cookies,   /* read Cookies */

	php_sapi_phpdbg_register_vars,  /* register server variables */
	php_sapi_phpdbg_log_message,    /* Log message */
	NULL,                           /* Get request time */
	NULL,                           /* Child terminate */
	STANDARD_SAPI_MODULE_PROPERTIES
};
/* }}} */

const opt_struct OPTIONS[] = { /* {{{ */
	{'c', 1, "ini path override"},
	{'d', 1, "define ini entry on command line"},
	{'n', 0, "no php.ini"},
	{'z', 1, "load zend_extension"},
	/* phpdbg options */
	{'q', 0, "no banner"},
	{'v', 0, "disable quietness"},
	{'b', 0, "boring colours"},
	{'i', 1, "specify init"},
	{'I', 0, "ignore init"},
	{'O', 1, "opline log"},
	{'r', 0, "run"},
	{'e', 0, "generate ext_stmt opcodes"},
	{'E', 0, "step-through-eval"},
	{'S', 1, "sapi-name"},
#ifndef _WIN32
	{'l', 1, "listen"},
	{'a', 1, "address-or-any"},
#endif
	{'x', 0, "xml output"},
	{'p', 2, "show opcodes"},
	{'h', 0, "help"},
	{'V', 0, "version"},
	{'-', 0, NULL}
}; /* }}} */

const char phpdbg_ini_hardcoded[] =
"html_errors=Off\n"
"register_argc_argv=On\n"
"implicit_flush=On\n"
"display_errors=Off\n"
"log_errors=On\n"
"max_execution_time=0\n"
"max_input_time=-1\n"
"error_log=\n"
"output_buffering=off\n\0";

/* overwriteable ini defaults must be set in phpdbg_ini_defaults() */
#define INI_DEFAULT(name, value) \
	ZVAL_STRINGL(&tmp, value, sizeof(value) - 1); \
	zend_hash_str_update(configuration_hash, name, sizeof(name) - 1, &tmp);

void phpdbg_ini_defaults(HashTable *configuration_hash) /* {{{ */
{
	zval tmp;
	INI_DEFAULT("report_zend_debug", "0");
} /* }}} */

static void phpdbg_welcome(zend_bool cleaning) /* {{{ */
{
	/* print blurb */
	if (!cleaning) {
		phpdbg_xml("<intros>");
		phpdbg_notice("intro", "version=\"%s\"", "Welcome to phpdbg, the interactive PHP debugger, v%s", PHPDBG_VERSION);
		phpdbg_writeln("intro", "help=\"help\"", "To get help using phpdbg type \"help\" and press enter");
		phpdbg_notice("intro", "report=\"%s\"", "Please report bugs to <%s>", PHPDBG_ISSUES);
		phpdbg_xml("</intros>");
	} else if (phpdbg_startup_run == 0) {
		if (!(PHPDBG_G(flags) & PHPDBG_WRITE_XML)) {
			phpdbg_notice(NULL, NULL, "Clean Execution Environment");
		}

		phpdbg_write("cleaninfo", "classes=\"%d\" functions=\"%d\" constants=\"%d\" includes=\"%d\"",
			"Classes              %d\n"
			"Functions            %d\n"
			"Constants            %d\n"
			"Includes             %d\n",
			zend_hash_num_elements(EG(class_table)),
			zend_hash_num_elements(EG(function_table)),
			zend_hash_num_elements(EG(zend_constants)),
			zend_hash_num_elements(&EG(included_files)));
	}
} /* }}} */

static inline void phpdbg_sigint_handler(int signo) /* {{{ */
{

	if (PHPDBG_G(flags) & PHPDBG_IS_INTERACTIVE) {
		/* we quit remote consoles on recv SIGINT */
		if (PHPDBG_G(flags) & PHPDBG_IS_REMOTE) {
			PHPDBG_G(flags) |= PHPDBG_IS_STOPPING;
			zend_bailout();
		}
	} else {
		/* set signalled only when not interactive */
		if (PHPDBG_G(flags) & PHPDBG_IS_SIGNALED) {
			char mem[PHPDBG_SIGSAFE_MEM_SIZE + 1];

			phpdbg_set_sigsafe_mem(mem);
			zend_try {
				phpdbg_force_interruption();
			} zend_end_try()
			phpdbg_clear_sigsafe_mem();

			PHPDBG_G(flags) &= ~PHPDBG_IS_SIGNALED;

			if (PHPDBG_G(flags) & PHPDBG_IS_STOPPING) {
				zend_bailout();
			}
		} else {
			PHPDBG_G(flags) |= PHPDBG_IS_SIGNALED;
		}
	}
} /* }}} */

static void phpdbg_remote_close(int socket, FILE *stream) {
	if (socket >= 0) {
		phpdbg_close_socket(socket);
	}

	if (stream) {
		fclose(stream);
	}
}

/* don't inline this, want to debug it easily, will inline when done */
static int phpdbg_remote_init(const char* address, unsigned short port, int server, int *socket, FILE **stream) {
	phpdbg_remote_close(*socket, *stream);

	if (server < 0) {
		phpdbg_rlog(fileno(stderr), "Initializing connection on %s:%u failed", address, port);

		return FAILURE;
	}

	phpdbg_rlog(fileno(stderr), "accepting connections on %s:%u", address, port);
	{
		struct sockaddr_storage address;
		socklen_t size = sizeof(address);
		char buffer[20] = {0};
		/* XXX error checks */
		memset(&address, 0, size);
		*socket = accept(server, (struct sockaddr *) &address, &size);
		inet_ntop(AF_INET, &(((struct sockaddr_in *)&address)->sin_addr), buffer, sizeof(buffer));

		phpdbg_rlog(fileno(stderr), "connection established from %s", buffer);
	}

#ifndef _WIN32
	dup2(*socket, fileno(stdout));
	dup2(*socket, fileno(stdin));

	setbuf(stdout, NULL);

	*stream = fdopen(*socket, "r+");

	phpdbg_set_async_io(*socket);
#endif
	return SUCCESS;
}

#ifndef _WIN32
/* This function *strictly* assumes that SIGIO is *only* used on the remote connection stream */
void phpdbg_sigio_handler(int sig, siginfo_t *info, void *context) /* {{{ */
{
	int flags;
	size_t newlen;
	size_t i/*, last_nl*/;

//	if (!(info->si_band & POLLIN)) {
//		return; /* Not interested in writeablility etc., just interested in incoming data */
//	}

	/* only non-blocking reading, avoid non-blocking writing */
	flags = fcntl(PHPDBG_G(io)[PHPDBG_STDIN].fd, F_GETFL, 0);
	fcntl(PHPDBG_G(io)[PHPDBG_STDIN].fd, F_SETFL, flags | O_NONBLOCK);

	do {
		char mem[PHPDBG_SIGSAFE_MEM_SIZE + 1];
		size_t off = 0;

		if ((newlen = recv(PHPDBG_G(io)[PHPDBG_STDIN].fd, mem, PHPDBG_SIGSAFE_MEM_SIZE, MSG_PEEK)) == (size_t) -1) {
			break;
		}
		for (i = 0; i < newlen; i++) {
			switch (mem[off + i]) {
				case '\x03': /* ^C char */
					if (PHPDBG_G(flags) & PHPDBG_IS_INTERACTIVE) {
						break; /* or quit ??? */
					}
					if (PHPDBG_G(flags) & PHPDBG_IS_SIGNALED) {
						phpdbg_set_sigsafe_mem(mem);
						zend_try {
							phpdbg_force_interruption();
						} zend_end_try();
						phpdbg_clear_sigsafe_mem();

						PHPDBG_G(flags) &= ~PHPDBG_IS_SIGNALED;

						if (PHPDBG_G(flags) & PHPDBG_IS_STOPPING) {
							zend_bailout();
						}
					} else if (!(PHPDBG_G(flags) & PHPDBG_IS_INTERACTIVE)) {
						PHPDBG_G(flags) |= PHPDBG_IS_SIGNALED;
					}
					break;
/*				case '\n':
					zend_llist_add_element(PHPDBG_G(stdin), strndup()
					last_nl = PHPDBG_G(stdin_buf).len + i;
					break;
*/			}
		}
		off += i;
	} while (0);


	fcntl(PHPDBG_G(io)[PHPDBG_STDIN].fd, F_SETFL, flags);
} /* }}} */

void phpdbg_signal_handler(int sig, siginfo_t *info, void *context) /* {{{ */
{
	int is_handled = FAILURE;

	switch (sig) {
		case SIGBUS:
		case SIGSEGV:
			if (PHPDBG_G(sigsegv_bailout)) {
				LONGJMP(*PHPDBG_G(sigsegv_bailout), FAILURE);
			}
			is_handled = phpdbg_watchpoint_segfault_handler(info, context);
			if (is_handled == FAILURE) {
#ifdef ZEND_SIGNALS
				zend_sigaction(sig, &PHPDBG_G(old_sigsegv_signal), NULL);
#else
				sigaction(sig, &PHPDBG_G(old_sigsegv_signal), NULL);
#endif
			}
			break;
	}

} /* }}} */
#endif

void phpdbg_sighup_handler(int sig) /* {{{ */
{
	exit(0);
} /* }}} */

void *phpdbg_malloc_wrapper(size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) /* {{{ */
{
	return _zend_mm_alloc(zend_mm_get_heap(), size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
} /* }}} */

void phpdbg_free_wrapper(void *p ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) /* {{{ */
{
	zend_mm_heap *heap = zend_mm_get_heap();
	if (UNEXPECTED(heap == p)) {
		/* TODO: heap maybe allocated by mmap(zend_mm_init) or malloc(USE_ZEND_ALLOC=0) 
		 * let's prevent it from segfault for now
		 */
	} else {
		phpdbg_watch_efree(p);
		return _zend_mm_free(heap, p ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	}
} /* }}} */

void *phpdbg_realloc_wrapper(void *ptr, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) /* {{{ */
{
	return _zend_mm_realloc(zend_mm_get_heap(), ptr, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
} /* }}} */

int main(int argc, char **argv) /* {{{ */
{
	sapi_module_struct *phpdbg = &phpdbg_sapi_module;
	char *sapi_name;
	char *ini_entries;
	int   ini_entries_len;
	char **zend_extensions = NULL;
	zend_ulong zend_extensions_len = 0L;
	zend_bool ini_ignore;
	char *ini_override;
	char *exec = NULL;
	char *init_file;
	size_t init_file_len;
	zend_bool init_file_default;
	char *oplog_file;
	size_t oplog_file_len;
	zend_ulong flags;
	char *php_optarg;
	int php_optind, opt, show_banner = 1;
	long cleaning = -1;
	volatile zend_bool quit_immediately = 0; /* somehow some gcc release builds will play a bit around with order in combination with setjmp..., hence volatile */
	zend_bool remote = 0;
	zend_phpdbg_globals *settings = NULL;
	char *bp_tmp = NULL;
	char *address;
	int listen = -1;
	int server = -1;
	int socket = -1;
	FILE* stream = NULL;
	char *print_opline_func;
	zend_bool ext_stmt = 0;
	zend_bool use_mm_wrappers = 0;
	zend_bool is_exit;
	int exit_status;

#ifdef ZTS
	void ***tsrm_ls;
#endif

#ifndef _WIN32
	struct sigaction sigio_struct;
	struct sigaction signal_struct;
	signal_struct.sa_sigaction = phpdbg_signal_handler;
	signal_struct.sa_flags = SA_SIGINFO | SA_NODEFER;
	sigemptyset(&signal_struct.sa_mask);
	sigio_struct.sa_sigaction = phpdbg_sigio_handler;
	sigio_struct.sa_flags = SA_SIGINFO;
	sigemptyset(&sigio_struct.sa_mask);

	address = strdup("127.0.0.1");
#endif

#ifdef PHP_WIN32
	_fmode = _O_BINARY;                 /* sets default for file streams to binary */
	setmode(_fileno(stdin), O_BINARY);  /* make the stdio mode be binary */
	setmode(_fileno(stdout), O_BINARY); /* make the stdio mode be binary */
	setmode(_fileno(stderr), O_BINARY); /* make the stdio mode be binary */
#endif

#ifdef ZTS
	tsrm_startup(1, 1, 0, NULL);

	tsrm_ls = ts_resource(0);
#endif

#ifdef ZEND_SIGNALS
	zend_signal_startup();
#endif

phpdbg_main:
	ini_entries = NULL;
	ini_entries_len = 0;
	ini_ignore = 0;
	ini_override = NULL;
	zend_extensions = NULL;
	zend_extensions_len = 0L;
	init_file = NULL;
	init_file_len = 0;
	init_file_default = 1;
	oplog_file = NULL;
	oplog_file_len = 0;
	flags = PHPDBG_DEFAULT_FLAGS;
	is_exit = 0;
	php_optarg = NULL;
	php_optind = 1;
	opt = 0;
	sapi_name = NULL;
	exit_status = 0;
	if (settings) {
		exec = settings->exec;
	}

	while ((opt = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 0, 2)) != -1) {
		switch (opt) {
			case 'r':
				if (settings == NULL) {
					phpdbg_startup_run++;
				}
				break;
			case 'n':
				ini_ignore = 1;
				break;
			case 'c':
				if (ini_override) {
					free(ini_override);
				}
				ini_override = strdup(php_optarg);
				break;
			case 'd': {
				int len = strlen(php_optarg);
				char *val;

				if ((val = strchr(php_optarg, '='))) {
				  val++;
				  if (!isalnum(*val) && *val != '"' && *val != '\'' && *val != '\0') {
					  ini_entries = realloc(ini_entries, ini_entries_len + len + sizeof("\"\"\n\0"));
					  memcpy(ini_entries + ini_entries_len, php_optarg, (val - php_optarg));
					  ini_entries_len += (val - php_optarg);
					  memcpy(ini_entries + ini_entries_len, "\"", 1);
					  ini_entries_len++;
					  memcpy(ini_entries + ini_entries_len, val, len - (val - php_optarg));
					  ini_entries_len += len - (val - php_optarg);
					  memcpy(ini_entries + ini_entries_len, "\"\n\0", sizeof("\"\n\0"));
					  ini_entries_len += sizeof("\n\0\"") - 2;
				  } else {
					  ini_entries = realloc(ini_entries, ini_entries_len + len + sizeof("\n\0"));
					  memcpy(ini_entries + ini_entries_len, php_optarg, len);
					  memcpy(ini_entries + ini_entries_len + len, "\n\0", sizeof("\n\0"));
					  ini_entries_len += len + sizeof("\n\0") - 2;
				  }
				} else {
				  ini_entries = realloc(ini_entries, ini_entries_len + len + sizeof("=1\n\0"));
				  memcpy(ini_entries + ini_entries_len, php_optarg, len);
				  memcpy(ini_entries + ini_entries_len + len, "=1\n\0", sizeof("=1\n\0"));
				  ini_entries_len += len + sizeof("=1\n\0") - 2;
				}
			} break;

			case 'z':
				zend_extensions_len++;
				if (zend_extensions) {
					zend_extensions = realloc(zend_extensions, sizeof(char*) * zend_extensions_len);
				} else zend_extensions = malloc(sizeof(char*) * zend_extensions_len);
				zend_extensions[zend_extensions_len-1] = strdup(php_optarg);
			break;

			/* begin phpdbg options */

			case 'S': { /* set SAPI name */
				if (sapi_name) {
					free(sapi_name);
				}
				sapi_name = strdup(php_optarg);
			} break;

			case 'I': { /* ignore .phpdbginit */
				init_file_default = 0;
			} break;

			case 'i': { /* set init file */
				if (init_file) {
					free(init_file);
				}

				init_file_len = strlen(php_optarg);
				if (init_file_len) {
					init_file = strdup(php_optarg);
				}
			} break;

			case 'O': { /* set oplog output */
				oplog_file_len = strlen(php_optarg);
				if (oplog_file_len) {
					oplog_file = strdup(php_optarg);
				}
			} break;

			case 'v': /* set quietness off */
				flags &= ~PHPDBG_IS_QUIET;
			break;

			case 'e':
				ext_stmt = 1;
			break;

			case 'E': /* stepping through eval on */
				flags |= PHPDBG_IS_STEPONEVAL;
			break;

			case 'b': /* set colours off */
				flags &= ~PHPDBG_IS_COLOURED;
			break;

			case 'q': /* hide banner */
				show_banner = 0;
			break;

#ifndef _WIN32
			/* if you pass a listen port, we will read and write on listen port */
			case 'l': /* set listen ports */
				if (sscanf(php_optarg, "%d", &listen) != 1) {
					listen = 8000;
				}
			break;

			case 'a': { /* set bind address */
				free(address);
				if (!php_optarg) {
					address = strdup("*");
				} else address = strdup(php_optarg);
			} break;
#endif

			case 'x':
				flags |= PHPDBG_WRITE_XML;
			break;


			case 'p': {
				print_opline_func = php_optarg;
				show_banner = 0;
				settings = (void *) 0x1;
			} break;

			case 'h': {
				sapi_startup(phpdbg);
				phpdbg->startup(phpdbg);
				PHPDBG_G(flags) = 0;
				/* It ain't gonna proceed to real execution anyway,
					but the correct descriptor is needed already. */
				PHPDBG_G(io)[PHPDBG_STDOUT].ptr = stdout;
				PHPDBG_G(io)[PHPDBG_STDOUT].fd = fileno(stdout);
				phpdbg_set_prompt(PHPDBG_DEFAULT_PROMPT);
				phpdbg_do_help(NULL);
				sapi_deactivate();
				sapi_shutdown();
				return 0;
			} break;

			case 'V': {
				sapi_startup(phpdbg);
				phpdbg->startup(phpdbg);
				printf(
					"phpdbg %s (built: %s %s)\nPHP %s, Copyright (c) 1997-2015 The PHP Group\n%s",
					PHPDBG_VERSION,
					__DATE__,
					__TIME__,
					PHP_VERSION,
					get_zend_version()
				);
				sapi_deactivate();
				sapi_shutdown();
				return 0;
			} break;
		}

		php_optarg = NULL;
	}

	/* set exec if present on command line */
	if (argc > php_optind && (strcmp(argv[php_optind-1], "--") != SUCCESS)) {
		if (!exec && strlen(argv[php_optind])) {
			exec = strdup(argv[php_optind]);
		}
		php_optind++;
	}

	if (sapi_name) {
		phpdbg->name = sapi_name;
	}

	phpdbg->ini_defaults = phpdbg_ini_defaults;
	phpdbg->phpinfo_as_text = 1;
	phpdbg->php_ini_ignore_cwd = 1;

	sapi_startup(phpdbg);

	phpdbg->executable_location = argv[0];
	phpdbg->phpinfo_as_text = 1;
	phpdbg->php_ini_ignore = ini_ignore;
	phpdbg->php_ini_path_override = ini_override;

	if (ini_entries) {
		ini_entries = realloc(ini_entries, ini_entries_len + sizeof(phpdbg_ini_hardcoded));
		memmove(ini_entries + sizeof(phpdbg_ini_hardcoded) - 2, ini_entries, ini_entries_len + 1);
		memcpy(ini_entries, phpdbg_ini_hardcoded, sizeof(phpdbg_ini_hardcoded) - 2);
	} else {
		ini_entries = malloc(sizeof(phpdbg_ini_hardcoded));
		memcpy(ini_entries, phpdbg_ini_hardcoded, sizeof(phpdbg_ini_hardcoded));
	}
	ini_entries_len += sizeof(phpdbg_ini_hardcoded) - 2;

	if (zend_extensions_len) {
		zend_ulong zend_extension = 0L;

		while (zend_extension < zend_extensions_len) {
			const char *ze = zend_extensions[zend_extension];
			size_t ze_len = strlen(ze);

			ini_entries = realloc(
				ini_entries, ini_entries_len + (ze_len + (sizeof("zend_extension=\n"))));
			memcpy(&ini_entries[ini_entries_len], "zend_extension=", (sizeof("zend_extension=\n")-1));
			ini_entries_len += (sizeof("zend_extension=")-1);
			memcpy(&ini_entries[ini_entries_len], ze, ze_len);
			ini_entries_len += ze_len;
			memcpy(&ini_entries[ini_entries_len], "\n", (sizeof("\n") - 1));

			free(zend_extensions[zend_extension]);
			zend_extension++;
		}

		free(zend_extensions);
	}

	phpdbg->ini_entries = ini_entries;

	if (phpdbg->startup(phpdbg) == SUCCESS) {
		zend_mm_heap *mm_heap;
#ifdef _WIN32
    EXCEPTION_POINTERS *xp;
    __try {
#endif
		void* (*_malloc)(size_t);
		void (*_free)(void*);
		void* (*_realloc)(void*, size_t);

		/* set flags from command line */
		PHPDBG_G(flags) = flags;

		if (settings > (zend_phpdbg_globals *) 0x2) {
#ifdef ZTS
			*((zend_phpdbg_globals *) (*((void ***) tsrm_ls))[TSRM_UNSHUFFLE_RSRC_ID(phpdbg_globals_id)]) = *settings;
#else
			phpdbg_globals = *settings;
#endif
			free(settings);
		}

		/* setup remote server if necessary */
		if (cleaning <= 0 && listen > 0) {
			server = phpdbg_open_socket(address, listen);
			if (-1 > server || phpdbg_remote_init(address, listen, server, &socket, &stream) == FAILURE) {
				exit(0);
			}

#ifndef _WIN32
			sigaction(SIGIO, &sigio_struct, NULL);
#endif

			/* set remote flag to stop service shutting down upon quit */
			remote = 1;
#ifndef _WIN32
		} else {

			signal(SIGHUP, phpdbg_sighup_handler);
#endif
		}

		mm_heap = zend_mm_get_heap();
		zend_mm_get_custom_handlers(mm_heap, &_malloc, &_free, &_realloc);

		use_mm_wrappers = !_malloc && !_realloc && !_free;

		phpdbg_init_list();

		PHPDBG_G(original_free_function) = _free;
		_free = phpdbg_watch_efree;

		if (use_mm_wrappers) {
#if ZEND_DEBUG
			zend_mm_set_custom_debug_handlers(mm_heap, phpdbg_malloc_wrapper, phpdbg_free_wrapper, phpdbg_realloc_wrapper);
#else
			zend_mm_set_custom_handlers(mm_heap, phpdbg_malloc_wrapper, phpdbg_free_wrapper, phpdbg_realloc_wrapper);
#endif
		} else {
			zend_mm_set_custom_handlers(mm_heap, _malloc, _free, _realloc);
		}

		phpdbg_setup_watchpoints();

#if defined(ZEND_SIGNALS) && !defined(_WIN32)
		zend_try {
			zend_signal_activate();
		} zend_end_try();
#endif

#if defined(ZEND_SIGNALS) && !defined(_WIN32)
		zend_try { zend_sigaction(SIGSEGV, &signal_struct, &PHPDBG_G(old_sigsegv_signal)); } zend_end_try();
		zend_try { zend_sigaction(SIGBUS, &signal_struct, &PHPDBG_G(old_sigsegv_signal)); } zend_end_try();
#elif !defined(_WIN32)
		sigaction(SIGSEGV, &signal_struct, &PHPDBG_G(old_sigsegv_signal));
		sigaction(SIGBUS, &signal_struct, &PHPDBG_G(old_sigsegv_signal));
#endif

		PHPDBG_G(sapi_name_ptr) = sapi_name;

		if (exec) { /* set execution context */
			PHPDBG_G(exec) = phpdbg_resolve_path(exec);
			PHPDBG_G(exec_len) = PHPDBG_G(exec) ? strlen(PHPDBG_G(exec)) : 0;

			free(exec);
			exec = NULL;
		}

		php_output_activate();
		php_output_deactivate();

		if (SG(sapi_headers).mimetype) {
			efree(SG(sapi_headers).mimetype);
			SG(sapi_headers).mimetype = NULL;
		}

		php_output_activate();

		{
			int i;

			SG(request_info).argc = argc - php_optind + 1;
			SG(request_info).argv = emalloc(SG(request_info).argc * sizeof(char *));
			for (i = SG(request_info).argc; --i;) {
				SG(request_info).argv[i] = estrdup(argv[php_optind - 1 + i]);
			}
			SG(request_info).argv[0] = PHPDBG_G(exec) ? estrdup(PHPDBG_G(exec)) : estrdup("");
		}

		if (php_request_startup() == FAILURE) {
			PUTS("Could not startup");
			return 1;
		}

		/* do not install sigint handlers for remote consoles */
		/* sending SIGINT then provides a decent way of shutting down the server */
#ifndef _WIN32
		if (listen < 0) {
#endif
#if defined(ZEND_SIGNALS) && !defined(_WIN32)
			zend_try { zend_signal(SIGINT, phpdbg_sigint_handler); } zend_end_try();
#else
			signal(SIGINT, phpdbg_sigint_handler);
#endif
#ifndef _WIN32
		}

		/* setup io here */
		if (remote) {
			PHPDBG_G(flags) |= PHPDBG_IS_REMOTE;

			signal(SIGPIPE, SIG_IGN);
		}
		PHPDBG_G(io)[PHPDBG_STDIN].ptr = stdin;
		PHPDBG_G(io)[PHPDBG_STDIN].fd = fileno(stdin);
		PHPDBG_G(io)[PHPDBG_STDOUT].ptr = stdout;
		PHPDBG_G(io)[PHPDBG_STDOUT].fd = fileno(stdout);
#else
		/* XXX this is a complete mess here with FILE/fd/SOCKET,
			we should let only one to survive probably. Need
			a clean separation whether it's a remote or local
			prompt. And what is supposed to go as user interaction,
			error log, etc. */
		if (remote) {
			PHPDBG_G(io)[PHPDBG_STDIN].ptr = stdin;
			PHPDBG_G(io)[PHPDBG_STDIN].fd = socket;
			PHPDBG_G(io)[PHPDBG_STDOUT].ptr = stdout;
			PHPDBG_G(io)[PHPDBG_STDOUT].fd = socket;
		} else {
			PHPDBG_G(io)[PHPDBG_STDIN].ptr = stdin;
			PHPDBG_G(io)[PHPDBG_STDIN].fd = fileno(stdin);
			PHPDBG_G(io)[PHPDBG_STDOUT].ptr = stdout;
			PHPDBG_G(io)[PHPDBG_STDOUT].fd = fileno(stdout);
		}
#endif
		PHPDBG_G(io)[PHPDBG_STDERR].ptr = stderr;
		PHPDBG_G(io)[PHPDBG_STDERR].fd = fileno(stderr);

#ifndef _WIN32
		PHPDBG_G(php_stdiop_write) = php_stream_stdio_ops.write;
		php_stream_stdio_ops.write = phpdbg_stdiop_write;
#endif

		if (oplog_file) { /* open oplog */
			PHPDBG_G(oplog) = fopen(oplog_file, "w+");
			if (!PHPDBG_G(oplog)) {
				phpdbg_error("oplog", "path=\"%s\"", "Failed to open oplog %s", oplog_file);
			}
			free(oplog_file);
			oplog_file = NULL;
		}

		/* set default colors */
		phpdbg_set_color_ex(PHPDBG_COLOR_PROMPT,  PHPDBG_STRL("white-bold"));
		phpdbg_set_color_ex(PHPDBG_COLOR_ERROR,   PHPDBG_STRL("red-bold"));
		phpdbg_set_color_ex(PHPDBG_COLOR_NOTICE,  PHPDBG_STRL("green"));

		/* set default prompt */
		phpdbg_set_prompt(PHPDBG_DEFAULT_PROMPT);

		/* Make stdin, stdout and stderr accessible from PHP scripts */
		phpdbg_register_file_handles();

		phpdbg_list_update();

		if (show_banner && cleaning < 2) {
			/* print blurb */
			phpdbg_welcome(cleaning == 1);
		}

		cleaning = -1;

		if (ext_stmt) {
			CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;
		}

		/* initialize from file */
		PHPDBG_G(flags) |= PHPDBG_IS_INITIALIZING;
		zend_try {
			phpdbg_init(init_file, init_file_len, init_file_default);
			if (bp_tmp) {
				PHPDBG_G(flags) |= PHPDBG_DISCARD_OUTPUT;
				phpdbg_string_init(bp_tmp);
				free(bp_tmp);
				bp_tmp = NULL;
				PHPDBG_G(flags) &= ~PHPDBG_DISCARD_OUTPUT;
			}
		} zend_end_try();
		PHPDBG_G(flags) &= ~PHPDBG_IS_INITIALIZING;

		/* quit if init says so */
		if (PHPDBG_G(flags) & PHPDBG_IS_QUITTING) {
			goto phpdbg_out;
		}

		/* auto compile */
		if (PHPDBG_G(exec)) {
			if (settings || phpdbg_startup_run > 0) {
				PHPDBG_G(flags) |= PHPDBG_DISCARD_OUTPUT;
			}

			zend_try {
				phpdbg_compile();
			} zend_end_try();

			PHPDBG_G(flags) &= ~PHPDBG_DISCARD_OUTPUT;
		}

		if (settings == (void *) 0x1) {
			if (PHPDBG_G(ops)) {
				phpdbg_print_opcodes(print_opline_func);
			} else {
				quiet_write(PHPDBG_G(io)[PHPDBG_STDERR].fd, ZEND_STRL("No opcodes could be compiled | No file specified or compilation failed?\n"));
			}
			goto phpdbg_out;
		}

		PG(during_request_startup) = 0;

		phpdbg_fully_started = 1;

/* #ifndef for making compiler shutting up */
#ifndef _WIN32
phpdbg_interact:
#endif
		/* phpdbg main() */
		do {
			zend_try {
				if (phpdbg_startup_run) {
					quit_immediately = phpdbg_startup_run > 1;
					phpdbg_startup_run = 0;
					if (quit_immediately) {
						PHPDBG_G(flags) |= PHPDBG_IS_INTERACTIVE | PHPDBG_PREVENT_INTERACTIVE;
					} else {
						PHPDBG_G(flags) |= PHPDBG_IS_INTERACTIVE;
					}
					zend_try {
						PHPDBG_COMMAND_HANDLER(run)(NULL);
					} zend_end_try();
					if (quit_immediately) {
						/* if -r is on the command line more than once just quit */
						EG(bailout) = __orig_bailout; /* reset zend_try */
						exit_status = EG(exit_status);
						break;
					}
				}

				CG(unclean_shutdown) = 0;
				phpdbg_interactive(1);
			} zend_catch {
				if ((PHPDBG_G(flags) & PHPDBG_IS_CLEANING)) {
					char *bp_tmp_str;
					PHPDBG_G(flags) |= PHPDBG_DISCARD_OUTPUT;
					phpdbg_export_breakpoints_to_string(&bp_tmp_str);
					PHPDBG_G(flags) &= ~PHPDBG_DISCARD_OUTPUT;
					if (bp_tmp_str) {
						bp_tmp = strdup(bp_tmp_str);
						efree(bp_tmp_str);
					}
					cleaning = 1;
				} else {
					cleaning = 0;
				}

#ifndef _WIN32
				if (!cleaning) {
					/* remote client disconnected */
					if ((PHPDBG_G(flags) & PHPDBG_IS_DISCONNECTED)) {

						if (PHPDBG_G(flags) & PHPDBG_IS_REMOTE) {
							/* renegociate connections */
							phpdbg_remote_init(address, listen, server, &socket, &stream);

							/* set streams */
							if (stream) {
								PHPDBG_G(flags) &= ~PHPDBG_IS_QUITTING;
							}

							/* this must be forced */
							CG(unclean_shutdown) = 0;
						} else {
							/* local consoles cannot disconnect, ignore EOF */
							PHPDBG_G(flags) &= ~PHPDBG_IS_DISCONNECTED;
						}
					}
				}
#endif
			} zend_end_try();
		} while (!(PHPDBG_G(flags) & PHPDBG_IS_STOPPING));


#ifndef _WIN32
phpdbg_out:
		if ((PHPDBG_G(flags) & PHPDBG_IS_DISCONNECTED)) {
			PHPDBG_G(flags) &= ~PHPDBG_IS_DISCONNECTED;
			goto phpdbg_interact;
		}
#endif

#ifdef _WIN32
	} __except(phpdbg_exception_handler_win32(xp = GetExceptionInformation())) {
		phpdbg_error("segfault", "", "Access violation (Segmentation fault) encountered\ntrying to abort cleanly...");
	}
phpdbg_out:
#endif

		if (cleaning <= 0) {
			PHPDBG_G(flags) &= ~PHPDBG_IS_CLEANING;
			cleaning = -1;
		}

		{
			int i;
			/* free argv */
			for (i = SG(request_info).argc; i--;) {
				efree(SG(request_info).argv[i]);
			}
			efree(SG(request_info).argv);
		}

		if (ini_entries) {
			free(ini_entries);
		}

		if (ini_override) {
			free(ini_override);
		}

		/* In case we aborted during script execution, we may not reset CG(unclean_shutdown) */
		if (!(PHPDBG_G(flags) & PHPDBG_IS_RUNNING)) {
			is_exit = !PHPDBG_G(in_execution);
			CG(unclean_shutdown) = is_exit || PHPDBG_G(unclean_eval);
		}

		if ((PHPDBG_G(flags) & (PHPDBG_IS_CLEANING | PHPDBG_IS_RUNNING)) == PHPDBG_IS_CLEANING) {
			php_free_shutdown_functions();
			zend_objects_store_mark_destructed(&EG(objects_store));
		}

		/* backup globals when cleaning */
		if ((cleaning > 0 || remote) && !quit_immediately) {
			settings = calloc(1, sizeof(zend_phpdbg_globals));

			php_phpdbg_globals_ctor(settings);

			if (PHPDBG_G(exec)) {
				settings->exec = zend_strndup(PHPDBG_G(exec), PHPDBG_G(exec_len));
				settings->exec_len = PHPDBG_G(exec_len);
			}
			settings->oplog = PHPDBG_G(oplog);
			settings->prompt[0] = PHPDBG_G(prompt)[0];
			settings->prompt[1] = PHPDBG_G(prompt)[1];
			memcpy(settings->colors, PHPDBG_G(colors), sizeof(settings->colors));
			settings->eol = PHPDBG_G(eol);
			settings->input_buflen = PHPDBG_G(input_buflen);
			memcpy(settings->input_buffer, PHPDBG_G(input_buffer), settings->input_buflen);
			settings->flags = PHPDBG_G(flags) & PHPDBG_PRESERVE_FLAGS_MASK;
		} else {
			if (PHPDBG_G(prompt)[0]) {
				free(PHPDBG_G(prompt)[0]);
			}
			if (PHPDBG_G(prompt)[1]) {
				free(PHPDBG_G(prompt)[1]);
			}
		}

		/* hack to restore mm_heap->use_custom_heap in order to receive memory leak info */
		if (use_mm_wrappers) {
			/* ASSUMING that mm_heap->use_custom_heap is the first element of the struct ... */
			*(int *) mm_heap = 0;
		}
		zend_try {
			php_request_shutdown(NULL);
		} zend_end_try();

		if (exit_status == 0) {
			exit_status = EG(exit_status);
		}

		if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
			if (PHPDBG_G(in_execution) || is_exit) {
				if (!quit_immediately && !phpdbg_startup_run) {
					phpdbg_notice("stop", "type=\"normal\"", "Script ended normally");
					cleaning++;
				}
			}
		}
		php_output_deactivate();

		zend_try {
			php_module_shutdown();
		} zend_end_try();

#ifndef _WIN32
		/* reset it... else we risk a stack overflow upon next run (when clean'ing) */
		php_stream_stdio_ops.write = PHPDBG_G(php_stdiop_write);
#endif

		sapi_shutdown();
	}

	if ((cleaning > 0 || remote) && !quit_immediately) {
		/* reset internal php_getopt state */
		php_getopt(-1, argv, OPTIONS, NULL, &php_optind, 0, 0);

		goto phpdbg_main;
	}

#ifdef ZTS
	/* bugggy */
	/* tsrm_shutdown(); */
#endif

#ifndef _WIN32
	if (address) {
		free(address);
	}
#endif

	if (PHPDBG_G(sapi_name_ptr)) {
		free(PHPDBG_G(sapi_name_ptr));
	}

	/* usually 0; just for -rr */
	return exit_status;
} /* }}} */
