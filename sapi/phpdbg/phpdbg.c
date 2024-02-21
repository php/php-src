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
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "phpdbg.h"
#include "phpdbg_prompt.h"
#include "phpdbg_bp.h"
#include "phpdbg_break.h"
#include "phpdbg_list.h"
#include "phpdbg_utils.h"
#include "phpdbg_set.h"
#include "phpdbg_io.h"
#include "zend_alloc.h"
#include "phpdbg_print.h"
#include "phpdbg_help.h"
#include "phpdbg_arginfo.h"
#include "zend_vm.h"
#include "php_ini_builder.h"

#include "ext/standard/basic_functions.h"

#if defined(PHP_WIN32) && defined(HAVE_OPENSSL)
# include "openssl/applink.c"
#endif

#if defined(PHP_WIN32) && defined(ZTS)
ZEND_TSRMLS_CACHE_DEFINE()
#endif

ZEND_DECLARE_MODULE_GLOBALS(phpdbg)
int phpdbg_startup_run = 0;

static bool phpdbg_booted = 0;
static bool phpdbg_fully_started = 0;
bool use_mm_wrappers = 1;

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
	zend_function_dtor(data);
} /* }}} */

static void php_phpdbg_destroy_file_source(zval *data) /* {{{ */
{
	phpdbg_file_source *source = (phpdbg_file_source *) Z_PTR_P(data);
	destroy_op_array(&source->op_array);
	if (source->buf) {
		efree(source->buf);
	}
	efree(source);
} /* }}} */

static inline void php_phpdbg_globals_ctor(zend_phpdbg_globals *pg) /* {{{ */
{
	pg->prompt[0] = NULL;
	pg->prompt[1] = NULL;

	pg->colors[0] = NULL;
	pg->colors[1] = NULL;
	pg->colors[2] = NULL;

	pg->lines = phpdbg_get_terminal_height();
	pg->exec = NULL;
	pg->exec_len = 0;
	pg->buffer = NULL;
	pg->last_was_newline = 1;
	pg->ops = NULL;
	pg->vmret = 0;
	pg->in_execution = 0;
	pg->bp_count = 0;
	pg->flags = PHPDBG_DEFAULT_FLAGS;
	memset(pg->io, 0, sizeof(pg->io));
	pg->frame.num = 0;
	pg->sapi_name_ptr = NULL;
	pg->unclean_eval = 0;

	pg->req_id = 0;
	pg->err_buf.active = 0;
	pg->err_buf.type = 0;

	pg->input_buflen = 0;
	pg->sigsafe_mem.mem = NULL;
	pg->sigsegv_bailout = NULL;

	pg->oplog_list = NULL;
	pg->stdin_file = NULL;

	pg->cur_command = NULL;
	pg->last_line = 0;

#ifdef HAVE_USERFAULTFD_WRITEFAULT
	pg->watch_userfaultfd = 0;
	pg->watch_userfault_thread = 0;
#endif
} /* }}} */

static PHP_MINIT_FUNCTION(phpdbg) /* {{{ */
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

	zend_hash_init(&PHPDBG_G(file_sources), 0, NULL, php_phpdbg_destroy_file_source, 0);
	phpdbg_setup_watchpoints();

	zend_execute_ex = phpdbg_execute_ex;

	register_phpdbg_symbols(module_number);

	return SUCCESS;
} /* }}} */

static PHP_MSHUTDOWN_FUNCTION(phpdbg) /* {{{ */
{
	zend_hash_destroy(&PHPDBG_G(registered));
	phpdbg_destroy_watchpoints();

	if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
		phpdbg_notice("Script ended normally");
	}

	/* hack to restore mm_heap->use_custom_heap in order to receive memory leak info */
	if (use_mm_wrappers) {
		/* ASSUMING that mm_heap->use_custom_heap is the first element of the struct ... */
		*(int *) zend_mm_get_heap() = 0;
	}

	if (PHPDBG_G(buffer)) {
		free(PHPDBG_G(buffer));
		PHPDBG_G(buffer) = NULL;
	}

	if (PHPDBG_G(exec)) {
		free(PHPDBG_G(exec));
		PHPDBG_G(exec) = NULL;
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

	fflush(stdout);
	if (SG(request_info).argv0) {
		free(SG(request_info).argv0);
		SG(request_info).argv0 = NULL;
	}

	return SUCCESS;
}
/* }}} */

static PHP_RINIT_FUNCTION(phpdbg) /* {{{ */
{
	/* deactivate symbol table caching to have these properly destroyed upon stack leaving (especially important for watchpoints) */
	EG(symtable_cache_limit) = EG(symtable_cache);

	if (zend_vm_kind() != ZEND_VM_KIND_HYBRID) {
		/* phpdbg cannot work JIT-ed code */
		zend_string *key = zend_string_init(ZEND_STRL("opcache.jit"), 1);
		zend_string *value = zend_string_init(ZEND_STRL("off"), 1);

		zend_alter_ini_entry(key, value, ZEND_INI_SYSTEM, ZEND_INI_STAGE_STARTUP);

		zend_string_release(key);
		zend_string_release(value);
	}

	return SUCCESS;
} /* }}} */

static PHP_RSHUTDOWN_FUNCTION(phpdbg) /* {{{ */
{
	if (PHPDBG_G(stdin_file)) {
		fclose(PHPDBG_G(stdin_file));
		PHPDBG_G(stdin_file) = NULL;
	}

	return SUCCESS;
} /* }}} */

/* {{{ Attempt to set the execution context for phpdbg
	If the execution context was set previously it is returned
	If the execution context was not set previously boolean true is returned
	If the request to set the context fails, boolean false is returned, and an E_WARNING raised */
PHP_FUNCTION(phpdbg_exec)
{
	zend_string *exec;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &exec) == FAILURE) {
		RETURN_THROWS();
	}

	{
		zend_stat_t sb = {0};
		bool result = 1;

		if (VCWD_STAT(ZSTR_VAL(exec), &sb) != FAILURE) {
			if (sb.st_mode & (S_IFREG|S_IFLNK)) {
				if (PHPDBG_G(exec)) {
					ZVAL_STRINGL(return_value, PHPDBG_G(exec), PHPDBG_G(exec_len));
					free(PHPDBG_G(exec));
					result = 0;
				}

				PHPDBG_G(exec) = zend_strndup(ZSTR_VAL(exec), ZSTR_LEN(exec));
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

/* {{{ instructs phpdbg to insert a breakpoint at the next opcode */
PHP_FUNCTION(phpdbg_break_next)
{
	zend_execute_data *ex;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	ex = EG(current_execute_data);
	while (ex && ex->func && !ZEND_USER_CODE(ex->func->type)) {
		ex = ex->prev_execute_data;
	}

	if (!ex) {
		return;
	}

	phpdbg_set_breakpoint_opline_ex((phpdbg_opline_ptr_t) ex->opline + 1);
} /* }}} */

/* {{{ */
PHP_FUNCTION(phpdbg_break_file)
{
	char *file;
	size_t flen;
	zend_long line;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl", &file, &flen, &line) == FAILURE) {
		RETURN_THROWS();
	}

	phpdbg_set_breakpoint_file(file, 0, line);
} /* }}} */

/* {{{ */
PHP_FUNCTION(phpdbg_break_method)
{
	char *class, *method;
	size_t clen, mlen;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &class, &clen, &method, &mlen) == FAILURE) {
		RETURN_THROWS();
	}

	phpdbg_set_breakpoint_method(class, method);
} /* }}} */

/* {{{ */
PHP_FUNCTION(phpdbg_break_function)
{
	char    *function;
	size_t   function_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &function, &function_len) == FAILURE) {
		RETURN_THROWS();
	}

	phpdbg_set_breakpoint_symbol(function, function_len);
} /* }}} */

/* {{{ instructs phpdbg to clear breakpoints */
PHP_FUNCTION(phpdbg_clear)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

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

/* {{{ */
PHP_FUNCTION(phpdbg_color)
{
	zend_long element;
	char *color;
	size_t color_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ls", &element, &color, &color_len) == FAILURE) {
		RETURN_THROWS();
	}

	switch (element) {
		case PHPDBG_COLOR_NOTICE:
		case PHPDBG_COLOR_ERROR:
		case PHPDBG_COLOR_PROMPT:
			phpdbg_set_color_ex(element, color, color_len);
		break;

		default:
			zend_argument_value_error(1, "must be one of PHPDBG_COLOR_PROMPT, PHPDBG_COLOR_NOTICE, or PHPDBG_COLOR_ERROR");
	}
} /* }}} */

/* {{{ */
PHP_FUNCTION(phpdbg_prompt)
{
	char *prompt = NULL;
	size_t prompt_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &prompt, &prompt_len) == FAILURE) {
		RETURN_THROWS();
	}

	phpdbg_set_prompt(prompt);
} /* }}} */

/* {{{ */
PHP_FUNCTION(phpdbg_start_oplog)
{
	phpdbg_oplog_list *prev;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	prev = PHPDBG_G(oplog_list);

	if (!prev) {
		PHPDBG_G(oplog_arena) = zend_arena_create(64 * 1024);
	}

	PHPDBG_G(oplog_list) = emalloc(sizeof(phpdbg_oplog_list));
	PHPDBG_G(oplog_list)->prev = prev;
	PHPDBG_G(oplog_cur) = &PHPDBG_G(oplog_list)->start;
	PHPDBG_G(oplog_cur)->next = NULL;
}

static zend_always_inline bool phpdbg_is_ignored_opcode(uint8_t opcode) {
	return
	    opcode == ZEND_NOP || opcode == ZEND_OP_DATA || opcode == ZEND_FE_FREE || opcode == ZEND_FREE || opcode == ZEND_ASSERT_CHECK || opcode == ZEND_VERIFY_RETURN_TYPE
	 || opcode == ZEND_DECLARE_CONST || opcode == ZEND_DECLARE_CLASS || opcode == ZEND_DECLARE_FUNCTION
	 || opcode == ZEND_DECLARE_CLASS_DELAYED
	 || opcode == ZEND_DECLARE_ANON_CLASS || opcode == ZEND_FAST_RET || opcode == ZEND_TICKS
	 || opcode == ZEND_EXT_STMT || opcode == ZEND_EXT_FCALL_BEGIN || opcode == ZEND_EXT_FCALL_END
	 || opcode == ZEND_BIND_GLOBAL || opcode == ZEND_BIND_INIT_STATIC_OR_JMP
	;
}

static void phpdbg_oplog_fill_executable(zend_op_array *op_array, HashTable *insert_ht, bool by_opcode) {
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
		uint8_t opcode = cur->opcode;
		if (phpdbg_is_ignored_opcode(opcode)) {
			continue;
		}

		if (by_opcode) {
			insert_idx = cur - op_array->opcodes;
		} else {
			insert_idx = cur->lineno;
		}

		if (opcode == ZEND_NEW && cur[1].opcode == ZEND_DO_FCALL) {
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

/* {{{ */
PHP_FUNCTION(phpdbg_get_executable)
{
	HashTable *options = NULL;
	zval *option_buffer;
	bool by_function = 0;
	bool by_opcode = 0;
	HashTable *insert_ht;

	zend_function *func;
	zend_class_entry *ce;
	zend_string *name;
	HashTable *files = &PHPDBG_G(file_sources);
	HashTable files_tmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|H", &options) == FAILURE) {
		RETURN_THROWS();
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
			GC_ADDREF(files);
		}
	} else {
		GC_ADDREF(files);
	}

	array_init(return_value);

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(EG(function_table), name, func) {
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

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(EG(class_table), name, ce) {
		if (ce->type == ZEND_USER_CLASS) {
			if (zend_hash_exists(files, ce->info.user.filename)) {
				ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, func) {
					if (func->type == ZEND_USER_FUNCTION && zend_hash_exists(files, func->op_array.filename)) {
						insert_ht = phpdbg_add_empty_array(Z_ARR_P(return_value), func->op_array.filename);

						if (by_function) {
							zend_string *fn_name = strpprintf(ZSTR_LEN(name) + ZSTR_LEN(func->op_array.function_name) + 2, "%.*s::%.*s", (int) ZSTR_LEN(name), ZSTR_VAL(name), (int) ZSTR_LEN(func->op_array.function_name), ZSTR_VAL(func->op_array.function_name));
							insert_ht = phpdbg_add_empty_array(insert_ht, fn_name);
							zend_string_release(fn_name);
						}

						phpdbg_oplog_fill_executable(&func->op_array, insert_ht, by_opcode);
					}
				} ZEND_HASH_FOREACH_END();
			}
		}
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_MAP_FOREACH_STR_KEY(files, name) {
		phpdbg_file_source *source = zend_hash_find_ptr(&PHPDBG_G(file_sources), name);
		if (source) {
			phpdbg_oplog_fill_executable(
				&source->op_array,
				phpdbg_add_empty_array(Z_ARR_P(return_value), source->op_array.filename),
				by_opcode);
		}
	} ZEND_HASH_FOREACH_END();

	if (!GC_DELREF(files)) {
		zend_hash_destroy(files);
	}
}

/* {{{ */
PHP_FUNCTION(phpdbg_end_oplog)
{
	phpdbg_oplog_entry *cur;
	phpdbg_oplog_list *prev;

	HashTable *options = NULL;
	zval *option_buffer;
	bool by_function = 0;
	bool by_opcode = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|H", &options) == FAILURE) {
		RETURN_THROWS();
	}

	if (!PHPDBG_G(oplog_list)) {
		zend_error(E_WARNING, "Cannot end an oplog without starting it");
		return;
	}

	cur = PHPDBG_G(oplog_list)->start.next;
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
		HashTable *file_ht = NULL;
		zend_string *last_function = (void *)~(uintptr_t)0;
		zend_class_entry *last_scope = NULL;

		HashTable *insert_ht = NULL;
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
						fn_name = strpprintf(ZSTR_LEN(last_function) + ZSTR_LEN(last_scope->name) + 2, "%.*s::%.*s", (int) ZSTR_LEN(last_scope->name), ZSTR_VAL(last_scope->name), (int) ZSTR_LEN(last_function), ZSTR_VAL(last_function));
					}
					insert_ht = phpdbg_add_empty_array(Z_ARR_P(return_value), fn_name);
					zend_string_release(fn_name);
				}
			}

			if (by_opcode) {
				insert_idx = cur->op - cur->opcodes;
			} else {
				if (phpdbg_is_ignored_opcode(cur->op->opcode)) {
					continue;
				}

				insert_idx = cur->op->lineno;
			}

			ZEND_ASSERT(insert_ht && file_ht);
			{
				zval *num = zend_hash_index_find(insert_ht, insert_idx);
				if (!num) {
					num = zend_hash_index_add_new(insert_ht, insert_idx, &zero);
				}
				Z_LVAL_P(num)++;
			}

		} while ((cur = cur->next));
	}

	if (!prev) {
		zend_arena_destroy(PHPDBG_G(oplog_arena));
	}
}

static zend_module_entry sapi_phpdbg_module_entry = {
	STANDARD_MODULE_HEADER,
	PHPDBG_NAME,
	ext_functions,
	PHP_MINIT(phpdbg),
	PHP_MSHUTDOWN(phpdbg),
	PHP_RINIT(phpdbg),
	PHP_RSHUTDOWN(phpdbg),
	NULL,
	PHPDBG_VERSION,
	STANDARD_MODULE_PROPERTIES
};

static inline int php_sapi_phpdbg_module_startup(sapi_module_struct *module) /* {{{ */
{
	if (php_module_startup(module, &sapi_phpdbg_module_entry) == FAILURE) {
		return FAILURE;
	}

	phpdbg_booted = 1;

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

static void php_sapi_phpdbg_log_message(const char *message, int syslog_type_int) /* {{{ */
{
	/*
	* We must not request TSRM before being booted
	*/
	if (phpdbg_booted) {
		if (PHPDBG_G(flags) & PHPDBG_IN_EVAL) {
			phpdbg_error("%s", message);
			return;
		}

		phpdbg_error("%s", message);

		if (PHPDBG_G(flags) & PHPDBG_PREVENT_INTERACTIVE) {
			return;
		}

		if (PG(last_error_type) & E_FATAL_ERRORS) {
			const char *file_char = zend_get_executed_filename();
			zend_string *file = zend_string_init(file_char, strlen(file_char), 0);
			phpdbg_list_file(file, 3, zend_get_executed_lineno() - 1, zend_get_executed_lineno());
			zend_string_release(file);

			if (!phpdbg_fully_started) {
				return;
			}

			do {
				switch (phpdbg_interactive(1, NULL)) {
					case PHPDBG_LEAVE:
					case PHPDBG_FINISH:
					case PHPDBG_UNTIL:
					case PHPDBG_NEXT:
						return;
				}
			} while (!(PHPDBG_G(flags) & PHPDBG_IS_STOPPING));
		}
	} else {
		fprintf(stdout, "%s\n", message);
	}
}
/* }}} */

static int php_sapi_phpdbg_activate(void) /* {{{ */
{
	return SUCCESS;
}

static int php_sapi_phpdbg_deactivate(void) /* {{{ */
{
	/* Everything using ZMM should be freed here... */
	zend_hash_destroy(&PHPDBG_G(file_sources));
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
	zend_hash_destroy(&PHPDBG_G(seek));

	if (PHPDBG_G(ops)) {
		destroy_op_array(PHPDBG_G(ops));
		efree(PHPDBG_G(ops));
		PHPDBG_G(ops) = NULL;
	}

	return SUCCESS;
}

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
	return phpdbg_script(P_STDOUT, "%.*s", (int) length, message);
} /* }}} */

/* beginning of struct, see main/streams/plain_wrapper.c line 111 */
typedef struct {
	FILE *file;
	int fd;
} php_stdio_stream_data;

static ssize_t phpdbg_stdiop_write(php_stream *stream, const char *buf, size_t count) {
	php_stdio_stream_data *data = (php_stdio_stream_data*)stream->abstract;

	while (data->fd >= 0) {
		struct stat stat[3];
		memset(stat, 0, sizeof(stat));
		int stat_stderr = fstat(fileno(stderr), &stat[2]);
		int stat_stdout = fstat(fileno(stdout), &stat[0]);
		int stat_datafd = fstat(data->fd, &stat[1]);
		if ((stat_stderr < 0 && stat_stdout < 0) || stat_datafd < 0) {
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

/* copied from sapi/cli/php_cli.c cli_register_file_handles */
void phpdbg_register_file_handles(void) /* {{{ */
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
	Z_CONSTANT_FLAGS(ic.value) = 0;
	ic.name = zend_string_init(ZEND_STRL("STDIN"), 0);
	zend_hash_del(EG(zend_constants), ic.name);
	zend_register_constant(&ic);

	oc.value = zout;
	Z_CONSTANT_FLAGS(oc.value) = 0;
	oc.name = zend_string_init(ZEND_STRL("STDOUT"), 0);
	zend_hash_del(EG(zend_constants), oc.name);
	zend_register_constant(&oc);

	ec.value = zerr;
	Z_CONSTANT_FLAGS(ec.value) = 0;
	ec.name = zend_string_init(ZEND_STRL("STDERR"), 0);
	zend_hash_del(EG(zend_constants), ec.name);
	zend_register_constant(&ec);
}
/* }}} */

/* {{{ sapi_module_struct phpdbg_sapi_module */
static sapi_module_struct phpdbg_sapi_module = {
	"phpdbg",                       /* name */
	"phpdbg",                       /* pretty name */

	php_sapi_phpdbg_module_startup, /* startup */
	php_module_shutdown_wrapper,    /* shutdown */

	php_sapi_phpdbg_activate,       /* activate */
	php_sapi_phpdbg_deactivate,     /* deactivate */

	php_sapi_phpdbg_ub_write,       /* unbuffered write */
	NULL,                           /* flush */
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

static const opt_struct OPTIONS[] = { /* {{{ */
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
	{'s', 1, "script from stdin"},
	{'S', 1, "sapi-name"},
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
"output_buffering=off\n";

static void phpdbg_welcome(bool cleaning) /* {{{ */
{
	/* print blurb */
	if (!cleaning) {
		phpdbg_notice("Welcome to phpdbg, the interactive PHP debugger, v%s", PHPDBG_VERSION);
		phpdbg_writeln("To get help using phpdbg type \"help\" and press enter");
		phpdbg_notice("Please report bugs to <%s>", PHPDBG_ISSUES);
	} else if (phpdbg_startup_run == 0) {
		phpdbg_write(
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
	if (!(PHPDBG_G(flags) & PHPDBG_IS_INTERACTIVE)) {
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
			if (PHPDBG_G(flags) & PHPDBG_PREVENT_INTERACTIVE) {
				PHPDBG_G(flags) |= PHPDBG_HAS_PAGINATION;
				PHPDBG_G(flags) &= ~PHPDBG_PREVENT_INTERACTIVE;
			}
		}
	}
} /* }}} */

#ifndef _WIN32
void phpdbg_signal_handler(int sig, siginfo_t *info, void *context) /* {{{ */
{
	int is_handled = FAILURE;

	switch (sig) {
		case SIGBUS:
		case SIGSEGV:
			is_handled = phpdbg_watchpoint_segfault_handler(info, context);
			if (is_handled == FAILURE) {
				if (PHPDBG_G(sigsegv_bailout)) {
					LONGJMP(*PHPDBG_G(sigsegv_bailout), FAILURE);
				}
				zend_sigaction(sig, &PHPDBG_G(old_sigsegv_signal), NULL);
			}
			break;
	}

} /* }}} */


void phpdbg_sighup_handler(int sig) /* {{{ */
{
	exit(0);
} /* }}} */
#endif

ZEND_ATTRIBUTE_MALLOC ZEND_ATTRIBUTE_ALLOC_SIZE(1) void *phpdbg_malloc_wrapper(size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) /* {{{ */
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
		phpdbg_watch_efree(p ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		_zend_mm_free(heap, p ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	}
} /* }}} */

void *phpdbg_realloc_wrapper(void *ptr, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) /* {{{ */
{
	return _zend_mm_realloc(zend_mm_get_heap(), ptr, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
} /* }}} */

php_stream *phpdbg_stream_url_wrap_php(php_stream_wrapper *wrapper, const char *path, const char *mode, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC) /* {{{ */
{
	if (!strncasecmp(path, "php://", 6)) {
		path += 6;
	}

	if (!strncasecmp(path, "stdin", 6) && PHPDBG_G(stdin_file)) {
		php_stream *stream = php_stream_fopen_from_fd(dup(fileno(PHPDBG_G(stdin_file))), "r", NULL);
#ifdef PHP_WIN32
		if (context != NULL) {
			zval *blocking_pipes = php_stream_context_get_option(context, "pipe", "blocking");
			if (blocking_pipes) {
				convert_to_long(blocking_pipes);
				php_stream_set_option(stream, PHP_STREAM_OPTION_PIPE_BLOCKING, Z_LVAL_P(blocking_pipes), NULL);
			}
		}
#endif
		return stream;
	}

	return PHPDBG_G(orig_url_wrap_php)->wops->stream_opener(wrapper, path, mode, options, opened_path, context STREAMS_CC);
} /* }}} */

int main(int argc, char **argv) /* {{{ */
{
	sapi_module_struct *phpdbg = &phpdbg_sapi_module;
	char *sapi_name;
	struct php_ini_builder ini_builder;
	char **zend_extensions = NULL;
	zend_ulong zend_extensions_len = 0L;
	bool ini_ignore;
	char *ini_override;
	char *exec = NULL;
	char *first_command = NULL;
	char *init_file;
	size_t init_file_len;
	bool init_file_default;
	uint64_t flags;
	char *php_optarg;
	int php_optind, opt, show_banner = 1;
	long cleaning = -1;
	volatile bool quit_immediately = 0; /* somehow some gcc release builds will play a bit around with order in combination with setjmp..., hence volatile */
	zend_phpdbg_globals *settings = NULL;
	char *bp_tmp = NULL;
	char *print_opline_func;
	bool ext_stmt = 0;
	bool is_exit;
	int exit_status;
	char *read_from_stdin = NULL;
	zend_string *backup_phpdbg_compile = NULL;
	bool show_help = 0, show_version = 0;
	void* (*_malloc)(size_t ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
	void (*_free)(void* ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
	void* (*_realloc)(void*, size_t ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
	php_stream_wrapper wrapper;
	php_stream_wrapper_ops wops;

#ifdef PHP_WIN32
	_fmode = _O_BINARY;                 /* sets default for file streams to binary */
	setmode(_fileno(stdin), O_BINARY);  /* make the stdio mode be binary */
	setmode(_fileno(stdout), O_BINARY); /* make the stdio mode be binary */
	setmode(_fileno(stderr), O_BINARY); /* make the stdio mode be binary */
#else
	struct sigaction signal_struct;
	signal_struct.sa_sigaction = phpdbg_signal_handler;
	signal_struct.sa_flags = SA_SIGINFO | SA_NODEFER;
#endif

phpdbg_main:
#ifdef ZTS
	php_tsrm_startup();
# ifdef PHP_WIN32
	ZEND_TSRMLS_CACHE_UPDATE();
# endif
#endif

	zend_signal_startup();

	php_ini_builder_init(&ini_builder);
	ini_ignore = 0;
	ini_override = NULL;
	zend_extensions = NULL;
	zend_extensions_len = 0L;
	init_file = NULL;
	init_file_len = 0;
	init_file_default = 1;
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
			case 'd':
				/* define ini entries on command line */
				php_ini_builder_define(&ini_builder, php_optarg);
				break;

			case 'z':
				zend_extensions_len++;
				if (zend_extensions) {
					zend_extensions = realloc(zend_extensions, sizeof(char*) * zend_extensions_len);
				} else zend_extensions = malloc(sizeof(char*) * zend_extensions_len);
				zend_extensions[zend_extensions_len-1] = strdup(php_optarg);
			break;

			/* begin phpdbg options */

			case 's': { /* read script from stdin */
				if (settings == NULL) {
					read_from_stdin = strdup(php_optarg);
				}
			} break;

			case 'S': { /* set SAPI name */
				sapi_name = strdup(php_optarg);
			} break;

			case 'I': { /* ignore .phpdbginit */
				init_file_default = 0;
			} break;

			case 'i': { /* set init file */
				if (init_file) {
					free(init_file);
					init_file = NULL;
				}

				init_file_len = strlen(php_optarg);
				if (init_file_len) {
					init_file = strdup(php_optarg);
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

			case 'p': {
				print_opline_func = php_optarg;
				show_banner = 0;
				settings = (void *) 0x1;
			} break;

			case 'h': {
				show_help = 1;
			} break;

			case 'V': {
				show_version = 1;
			} break;
		}

		php_optarg = NULL;
	}

	quit_immediately = phpdbg_startup_run > 1;

	/* set exec if present on command line */
	if (!read_from_stdin && argc > php_optind) {
		if (!exec && strlen(argv[php_optind])) {
			exec = strdup(argv[php_optind]);
		}
		php_optind++;
	}

	if (sapi_name) {
		phpdbg->name = sapi_name;
	}

	phpdbg->ini_defaults = NULL;
	phpdbg->phpinfo_as_text = 1;
	phpdbg->php_ini_ignore_cwd = 1;

	sapi_startup(phpdbg);

	phpdbg->executable_location = argv[0];
	phpdbg->phpinfo_as_text = 1;
	phpdbg->php_ini_ignore = ini_ignore;
	phpdbg->php_ini_path_override = ini_override;

	php_ini_builder_prepend_literal(&ini_builder, phpdbg_ini_hardcoded);

	if (zend_extensions_len) {
		zend_ulong zend_extension = 0L;

		while (zend_extension < zend_extensions_len) {
			const char *ze = zend_extensions[zend_extension];
			size_t ze_len = strlen(ze);

			php_ini_builder_unquoted(&ini_builder, "zend_extension", strlen("zend_extension"), ze, ze_len);

			free(zend_extensions[zend_extension]);
			zend_extension++;
		}

		free(zend_extensions);
	}

	phpdbg->ini_entries = php_ini_builder_finish(&ini_builder);

	ZEND_INIT_MODULE_GLOBALS(phpdbg, php_phpdbg_globals_ctor, NULL);

	/* set default colors */
	phpdbg_set_color_ex(PHPDBG_COLOR_PROMPT,  PHPDBG_STRL("white-bold"));
	phpdbg_set_color_ex(PHPDBG_COLOR_ERROR,   PHPDBG_STRL("red-bold"));
	phpdbg_set_color_ex(PHPDBG_COLOR_NOTICE,  PHPDBG_STRL("green"));

	if (settings > (zend_phpdbg_globals *) 0x2) {
#ifdef ZTS
		zend_phpdbg_globals *ptr = TSRMG_BULK_STATIC(phpdbg_globals_id, zend_phpdbg_globals *);
		*ptr = *settings;
#else
		phpdbg_globals = *settings;
#endif
		free(settings);
	} else {
		/* set default prompt */
		phpdbg_set_prompt(PHPDBG_DEFAULT_PROMPT);
	}

	/* set flags from command line */
	PHPDBG_G(flags) = flags;

	if (phpdbg->startup(phpdbg) == SUCCESS) {
		zend_mm_heap *mm_heap;
#ifdef _WIN32
	EXCEPTION_POINTERS *xp;
	__try {
#endif

		if (show_version || show_help) {
			/* It ain't gonna proceed to real execution anyway,
				but the correct descriptor is needed already. */
			PHPDBG_G(io)[PHPDBG_STDOUT].fd = fileno(stdout);
			if (show_help) {
				phpdbg_do_help_cmd(exec);
			} else if (show_version) {
				phpdbg_out(
					"phpdbg %s (built: %s %s)\nPHP %s, Copyright (c) The PHP Group\n%s",
					PHPDBG_VERSION,
					__DATE__,
					__TIME__,
					PHP_VERSION,
					get_zend_version()
				);
			}
			PHPDBG_G(flags) |= PHPDBG_IS_QUITTING;
			php_module_shutdown();
			sapi_deactivate();
			sapi_shutdown();
			php_ini_builder_deinit(&ini_builder);
			if (ini_override) {
				free(ini_override);
			}
			if (exec) {
				free(exec);
			}
			if (init_file) {
				free(init_file);
			}
			goto free_and_return;
		}

		zend_try {
			zend_signal_activate();
		} zend_end_try();

#ifndef _WIN32
		zend_signal(SIGHUP, phpdbg_sighup_handler);
#endif

		mm_heap = zend_mm_get_heap();
		zend_mm_get_custom_handlers(mm_heap, &_malloc, &_free, &_realloc);

		use_mm_wrappers = !_malloc && !_realloc && !_free;

		PHPDBG_G(original_free_function) = _free;
		_free = phpdbg_watch_efree;

		if (use_mm_wrappers) {
			zend_mm_set_custom_handlers(mm_heap, phpdbg_malloc_wrapper, phpdbg_free_wrapper, phpdbg_realloc_wrapper);
		} else {
			zend_mm_set_custom_handlers(mm_heap, _malloc, _free, _realloc);
		}

		_free = PHPDBG_G(original_free_function);


		phpdbg_init_list();

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

#ifndef _WIN32
#ifdef HAVE_USERFAULTFD_WRITEFAULT
		if (!PHPDBG_G(watch_userfaultfd))
#endif
		{
			zend_try { zend_sigaction(SIGSEGV, &signal_struct, &PHPDBG_G(old_sigsegv_signal)); } zend_end_try();
			zend_try { zend_sigaction(SIGBUS, &signal_struct, &PHPDBG_G(old_sigsegv_signal)); } zend_end_try();
		}
#endif
		zend_try { zend_signal(SIGINT, phpdbg_sigint_handler); } zend_end_try();


		PHPDBG_G(io)[PHPDBG_STDIN].fd = fileno(stdin);
		PHPDBG_G(io)[PHPDBG_STDOUT].fd = fileno(stdout);
		PHPDBG_G(io)[PHPDBG_STDERR].fd = fileno(stderr);

#ifndef _WIN32
		PHPDBG_G(php_stdiop_write) = php_stream_stdio_ops.write;
		php_stream_stdio_ops.write = phpdbg_stdiop_write;
#endif

		{
			zval *zv = zend_hash_str_find(php_stream_get_url_stream_wrappers_hash(), ZEND_STRL("php"));
			php_stream_wrapper *tmp_wrapper = Z_PTR_P(zv);
			PHPDBG_G(orig_url_wrap_php) = tmp_wrapper;
			memcpy(&wrapper, tmp_wrapper, sizeof(wrapper));
			memcpy(&wops, tmp_wrapper->wops, sizeof(wops));
			wops.stream_opener = phpdbg_stream_url_wrap_php;
			wrapper.wops = (const php_stream_wrapper_ops*)&wops;
			Z_PTR_P(zv) = &wrapper;
		}

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
		} zend_end_try();
		PHPDBG_G(flags) &= ~PHPDBG_IS_INITIALIZING;

		/* quit if init says so */
		if (PHPDBG_G(flags) & PHPDBG_IS_QUITTING) {
			goto phpdbg_out;
		}

		/* auto compile */
		if (read_from_stdin) {
			if (!read_from_stdin[0]) {
				if (!quit_immediately) {
					phpdbg_error("Impossible to not specify a stdin delimiter without -rr");
					PHPDBG_G(flags) |= PHPDBG_IS_QUITTING;
					goto phpdbg_out;
				}
			}
			if (show_banner || read_from_stdin[0]) {
				phpdbg_notice("Reading input from stdin; put '%s' followed by a newline on an own line after code to end input", read_from_stdin);
			}

			if (phpdbg_startup_run > 0) {
				PHPDBG_G(flags) |= PHPDBG_DISCARD_OUTPUT;
			}

			zend_try {
				phpdbg_param_t cmd;
				cmd.str = read_from_stdin;
				cmd.len = strlen(read_from_stdin);
				PHPDBG_COMMAND_HANDLER(stdin)(&cmd);
			} zend_end_try();

			PHPDBG_G(flags) &= ~PHPDBG_DISCARD_OUTPUT;
		} else if (PHPDBG_G(exec)) {
			if (settings || phpdbg_startup_run > 0) {
				PHPDBG_G(flags) |= PHPDBG_DISCARD_OUTPUT;
			}

			zend_try {
				if (backup_phpdbg_compile) {
					phpdbg_compile_stdin(backup_phpdbg_compile);
				} else {
					phpdbg_compile();
				}
			} zend_end_try();
			backup_phpdbg_compile = NULL;

			PHPDBG_G(flags) &= ~PHPDBG_DISCARD_OUTPUT;
		}

		if (bp_tmp) {
			PHPDBG_G(flags) |= PHPDBG_DISCARD_OUTPUT | PHPDBG_IS_INITIALIZING;
			phpdbg_string_init(bp_tmp);
			free(bp_tmp);
			bp_tmp = NULL;
			PHPDBG_G(flags) &= ~PHPDBG_DISCARD_OUTPUT & ~PHPDBG_IS_INITIALIZING;
		}

		if (settings == (void *) 0x1) {
			if (PHPDBG_G(ops)) {
				phpdbg_print_opcodes(print_opline_func);
			} else {
				zend_quiet_write(PHPDBG_G(io)[PHPDBG_STDERR].fd, ZEND_STRL("No opcodes could be compiled | No file specified or compilation failed?\n"));
			}
			goto phpdbg_out;
		}

		PG(during_request_startup) = 0;

		phpdbg_fully_started = 1;

		/* phpdbg main() */
		do {
			zend_try {
				if (phpdbg_startup_run) {
					phpdbg_startup_run = 0;
					if (quit_immediately) {
						PHPDBG_G(flags) = (PHPDBG_G(flags) & ~PHPDBG_HAS_PAGINATION) | PHPDBG_IS_INTERACTIVE | PHPDBG_PREVENT_INTERACTIVE;
					} else {
						PHPDBG_G(flags) |= PHPDBG_IS_INTERACTIVE;
					}
					zend_try {
						if (first_command) {
							phpdbg_interactive(1, estrdup(first_command));
						} else {
							PHPDBG_COMMAND_HANDLER(run)(NULL);
						}
					} zend_end_try();
					if (quit_immediately) {
						/* if -r is on the command line more than once just quit */
						EG(bailout) = __orig_bailout; /* reset zend_try */
						exit_status = EG(exit_status);
						break;
					}
				}

				CG(unclean_shutdown) = 0;
				phpdbg_interactive(1, NULL);
			} zend_catch {
				if ((PHPDBG_G(flags) & PHPDBG_IS_CLEANING)) {
					char *bp_tmp_str;
					PHPDBG_G(flags) |= PHPDBG_DISCARD_OUTPUT;
					phpdbg_export_breakpoints_to_string(&bp_tmp_str);
					PHPDBG_G(flags) &= ~PHPDBG_DISCARD_OUTPUT;
					if (bp_tmp_str) {
						bp_tmp = strdup(bp_tmp_str);
						free(bp_tmp_str);
					}
					cleaning = 1;
				} else {
					cleaning = 0;
				}
			} zend_end_try();
		} while (!(PHPDBG_G(flags) & PHPDBG_IS_STOPPING));

#ifdef _WIN32
	} __except(phpdbg_exception_handler_win32(xp = GetExceptionInformation())) {
		phpdbg_error("Access violation (Segmentation fault) encountered\ntrying to abort cleanly...");
	}
#endif
phpdbg_out:

		phpdbg_purge_watchpoint_tree();

		if (first_command) {
			free(first_command);
			first_command = NULL;
		}

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

		php_ini_builder_deinit(&ini_builder);

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

		if (PHPDBG_G(exec) && strcmp("Standard input code", PHPDBG_G(exec)) == SUCCESS) { /* i.e. execution context has been read from stdin - back it up */
			phpdbg_file_source *data = zend_hash_str_find_ptr(&PHPDBG_G(file_sources), PHPDBG_G(exec), PHPDBG_G(exec_len));
			backup_phpdbg_compile = zend_string_alloc(data->len + 2, 1);
			GC_MAKE_PERSISTENT_LOCAL(backup_phpdbg_compile);
			sprintf(ZSTR_VAL(backup_phpdbg_compile), "?>%.*s", (int) data->len, data->buf);
		}

		zend_try {
			php_request_shutdown(NULL);
		} zend_end_try();

		/* backup globals when cleaning */
		if ((cleaning > 0) && !quit_immediately) {
			settings = calloc(1, sizeof(zend_phpdbg_globals));

			php_phpdbg_globals_ctor(settings);

			if (PHPDBG_G(exec)) {
				settings->exec = zend_strndup(PHPDBG_G(exec), PHPDBG_G(exec_len));
				settings->exec_len = PHPDBG_G(exec_len);
			}
			settings->prompt[0] = PHPDBG_G(prompt)[0];
			settings->prompt[1] = PHPDBG_G(prompt)[1];
			memcpy(ZEND_VOIDP(settings->colors), PHPDBG_G(colors), sizeof(settings->colors));
			settings->input_buflen = PHPDBG_G(input_buflen);
			memcpy(settings->input_buffer, PHPDBG_G(input_buffer), settings->input_buflen);
			settings->flags = PHPDBG_G(flags) & PHPDBG_PRESERVE_FLAGS_MASK;
			first_command = PHPDBG_G(cur_command);
		} else {
			if (PHPDBG_G(prompt)[0]) {
				free(PHPDBG_G(prompt)[0]);
			}
			if (PHPDBG_G(prompt)[1]) {
				free(PHPDBG_G(prompt)[1]);
			}
			if (PHPDBG_G(cur_command)) {
				free(PHPDBG_G(cur_command));
			}
		}

		if (exit_status == 0) {
			exit_status = EG(exit_status);
		}

		php_output_deactivate();

		if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
			PHPDBG_G(flags) |= PHPDBG_IS_QUITTING;
			if (PHPDBG_G(in_execution) || is_exit) {
				if (!quit_immediately && !phpdbg_startup_run) {
					PHPDBG_G(flags) -= PHPDBG_IS_QUITTING;
					cleaning++;
				}
			}
		}

		{
			zval *zv = zend_hash_str_find(php_stream_get_url_stream_wrappers_hash(), ZEND_STRL("php"));
			Z_PTR_P(zv) = (void*)PHPDBG_G(orig_url_wrap_php);
		}

#ifndef _WIN32
		/* force override (no zend_signals) to prevent crashes due to signal recursion in SIGSEGV/SIGBUS handlers */
		signal(SIGSEGV, SIG_DFL);
		signal(SIGBUS, SIG_DFL);

		/* reset it... else we risk a stack overflow upon next run (when clean'ing) */
		php_stream_stdio_ops.write = PHPDBG_G(php_stdiop_write);
#endif
	}

	php_module_shutdown();

	sapi_shutdown();

	if (sapi_name) {
		free(sapi_name);
	}

free_and_return:
	if (read_from_stdin) {
		free(read_from_stdin);
		read_from_stdin = NULL;
	}

#ifdef ZTS
	/* reset to original handlers - otherwise PHPDBG_G() in phpdbg_watch_efree will be segfaulty (with e.g. USE_ZEND_ALLOC=0) */
	if (!use_mm_wrappers) {
		zend_mm_set_custom_handlers(zend_mm_get_heap(), _malloc, _free, _realloc);
	}

	ts_free_id(phpdbg_globals_id);

	tsrm_shutdown();
#endif

	if ((cleaning > 0) && !quit_immediately) {
		/* reset internal php_getopt state */
		php_getopt(-1, argv, OPTIONS, NULL, &php_optind, 0, 0);

		goto phpdbg_main;
	}

	if (backup_phpdbg_compile) {
		zend_string_free(backup_phpdbg_compile);
	}

	/* usually 0; just for -rr */
	return exit_status;
} /* }}} */
