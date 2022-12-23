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

#include <stdio.h>
#include <string.h>
#include "zend.h"
#include "zend_compile.h"
#include "zend_exceptions.h"
#include "zend_vm.h"
#include "zend_generators.h"
#include "zend_interfaces.h"
#include "zend_smart_str.h"
#include "phpdbg.h"
#include "phpdbg_io.h"

#include "phpdbg_help.h"
#include "phpdbg_print.h"
#include "phpdbg_info.h"
#include "phpdbg_break.h"
#include "phpdbg_list.h"
#include "phpdbg_utils.h"
#include "phpdbg_prompt.h"
#include "phpdbg_cmd.h"
#include "phpdbg_set.h"
#include "phpdbg_frame.h"
#include "phpdbg_lexer.h"
#include "phpdbg_parser.h"

#if ZEND_VM_KIND != ZEND_VM_KIND_CALL && ZEND_VM_KIND != ZEND_VM_KIND_HYBRID
#error "phpdbg can only be built with CALL zend vm kind"
#endif

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)
extern int phpdbg_startup_run;

#ifdef HAVE_LIBDL
#ifdef PHP_WIN32
#include "win32/param.h"
#include "win32/winutil.h"
#define GET_DL_ERROR()  php_win_err()
#else
#include <sys/param.h>
#define GET_DL_ERROR()  DL_ERROR()
#endif
#endif

/* {{{ command declarations */
const phpdbg_command_t phpdbg_prompt_commands[] = {
	PHPDBG_COMMAND_D(exec,      "set execution context",                    'e', NULL, "s", 0),
	PHPDBG_COMMAND_D(stdin,     "read script from stdin",                    0 , NULL, "s", 0),
	PHPDBG_COMMAND_D(step,      "step through execution",                   's', NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(continue,  "continue execution",                       'c', NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(run,       "attempt execution",                        'r', NULL, "|s", 0),
	PHPDBG_COMMAND_D(ev,        "evaluate some code",                        0 , NULL, "i", PHPDBG_ASYNC_SAFE), /* restricted ASYNC_SAFE */
	PHPDBG_COMMAND_D(until,     "continue past the current line",           'u', NULL, 0, 0),
	PHPDBG_COMMAND_D(finish,    "continue past the end of the stack",       'F', NULL, 0, 0),
	PHPDBG_COMMAND_D(leave,     "continue until the end of the stack",      'L', NULL, 0, 0),
	PHPDBG_COMMAND_D(generator, "inspect or switch to a generator",         'g', NULL, "|n", 0),
	PHPDBG_COMMAND_D(print,     "print something",                          'p', phpdbg_print_commands, "|*c", 0),
	PHPDBG_COMMAND_D(break,     "set breakpoint",                           'b', phpdbg_break_commands, "|*c", 0),
	PHPDBG_COMMAND_D(back,      "show trace",                               't', NULL, "|n", PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(frame,     "switch to a frame",                        'f', NULL, "|n", PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(list,      "lists some code",                          'l', phpdbg_list_commands,  "*", PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(info,      "displays some information",               'i', phpdbg_info_commands, "|s", PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(clean,     "clean the execution environment",          'X', NULL, 0, 0),
	PHPDBG_COMMAND_D(clear,     "clear breakpoints",                        'C', NULL, 0, 0),
	PHPDBG_COMMAND_D(help,      "show help menu",                           'h', phpdbg_help_commands, "|s", PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(set,       "set phpdbg configuration",                 'S', phpdbg_set_commands,   "s", PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(register,  "register a function",                      'R', NULL, "s", 0),
	PHPDBG_COMMAND_D(source,    "execute a phpdbginit",                     '<', NULL, "s", 0),
	PHPDBG_COMMAND_D(export,    "export breaks to a .phpdbginit script",    '>', NULL, "s", PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(sh,   	    "shell a command",                           0 , NULL, "i", 0),
	PHPDBG_COMMAND_D(quit,      "exit phpdbg",                              'q', NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(watch,     "set watchpoint",                           'w', phpdbg_watch_commands, "|ss", 0),
	PHPDBG_COMMAND_D(next,      "step over next line",                      'n', NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_END_COMMAND
}; /* }}} */

static inline int phpdbg_call_register(phpdbg_param_t *stack) /* {{{ */
{
	phpdbg_param_t *name = NULL;

	if (stack->type == STACK_PARAM) {
		char *lc_name;

		name = stack->next;

		if (!name || name->type != STR_PARAM) {
			return FAILURE;
		}

		lc_name = zend_str_tolower_dup(name->str, name->len);

		if (zend_hash_str_exists(&PHPDBG_G(registered), lc_name, name->len)) {
			zval fretval;
			zend_fcall_info fci;

			memset(&fci, 0, sizeof(zend_fcall_info));

			ZVAL_STRINGL(&fci.function_name, lc_name, name->len);
			fci.size = sizeof(zend_fcall_info);
			//???fci.symbol_table = zend_rebuild_symbol_table();
			fci.object = NULL;
			fci.retval = &fretval;

			if (name->next) {
				zval params;
				phpdbg_param_t *next = name->next;

				array_init(&params);

				while (next) {
					char *buffered = NULL;

					switch (next->type) {
						case OP_PARAM:
						case COND_PARAM:
						case STR_PARAM:
							add_next_index_stringl(&params, next->str, next->len);
						break;

						case NUMERIC_PARAM:
							add_next_index_long(&params, next->num);
						break;

						case METHOD_PARAM:
							spprintf(&buffered, 0, "%s::%s", next->method.class, next->method.name);
							add_next_index_string(&params, buffered);
						break;

						case NUMERIC_METHOD_PARAM:
							spprintf(&buffered, 0, "%s::%s#"ZEND_LONG_FMT, next->method.class, next->method.name, next->num);
							add_next_index_string(&params, buffered);
						break;

						case NUMERIC_FUNCTION_PARAM:
							spprintf(&buffered, 0, "%s#"ZEND_LONG_FMT, next->str, next->num);
							add_next_index_string(&params, buffered);
						break;

						case FILE_PARAM:
							spprintf(&buffered, 0, "%s:"ZEND_ULONG_FMT, next->file.name, next->file.line);
							add_next_index_string(&params, buffered);
						break;

						case NUMERIC_FILE_PARAM:
							spprintf(&buffered, 0, "%s:#"ZEND_ULONG_FMT, next->file.name, next->file.line);
							add_next_index_string(&params, buffered);
						break;

						default: {
							/* not yet */
						}
					}

					next = next->next;
				}

				zend_fcall_info_args(&fci, &params);
			} else {
				fci.params = NULL;
				fci.param_count = 0;
			}

			phpdbg_activate_err_buf(0);
			phpdbg_free_err_buf();

			phpdbg_debug("created %d params from arguments", fci.param_count);

			if (zend_call_function(&fci, NULL) == SUCCESS) {
				zend_print_zval_r(&fretval, 0);
				phpdbg_out("\n");
				zval_ptr_dtor(&fretval);
			}

			zval_ptr_dtor_str(&fci.function_name);
			efree(lc_name);

			return SUCCESS;
		}

		efree(lc_name);
	}

	return FAILURE;
} /* }}} */

struct phpdbg_init_state {
	int line;
	bool in_code;
	char *code;
	size_t code_len;
	const char *init_file;
};

static void phpdbg_line_init(char *cmd, struct phpdbg_init_state *state) {
	size_t cmd_len = strlen(cmd);

	state->line++;

	while (cmd_len > 0L && isspace(cmd[cmd_len-1])) {
		cmd_len--;
	}

	cmd[cmd_len] = '\0';

	if (*cmd && cmd_len > 0L && cmd[0] != '#') {
		if (cmd_len == 2) {
			if (memcmp(cmd, "<:", sizeof("<:")-1) == SUCCESS) {
				state->in_code = 1;
				return;
			} else {
				if (memcmp(cmd, ":>", sizeof(":>")-1) == SUCCESS) {
					state->in_code = 0;
					state->code[state->code_len] = '\0';
					zend_eval_stringl(state->code, state->code_len, NULL, "phpdbginit code");
					free(state->code);
					state->code = NULL;
					return;
				}
			}
		}

		if (state->in_code) {
			if (state->code == NULL) {
				state->code = malloc(cmd_len + 1);
			} else {
				state->code = realloc(state->code, state->code_len + cmd_len + 1);
			}

			if (state->code) {
				memcpy(&state->code[state->code_len], cmd, cmd_len);
				state->code_len += cmd_len;
			}

			return;
		}

		zend_try {
			char *input = phpdbg_read_input(cmd);
			phpdbg_param_t stack;

			phpdbg_init_param(&stack, STACK_PARAM);

			phpdbg_activate_err_buf(1);

			if (phpdbg_do_parse(&stack, input) <= 0) {
				switch (phpdbg_stack_execute(&stack, 1 /* allow_async_unsafe == 1 */)) {
					case FAILURE:
						phpdbg_activate_err_buf(0);
						if (phpdbg_call_register(&stack) == FAILURE) {
							if (state->init_file) {
								phpdbg_output_err_buf("Unrecognized command in %s:%d: %s, %s!", state->init_file, state->line, input, PHPDBG_G(err_buf).msg);
							} else {
								phpdbg_output_err_buf("Unrecognized command on line %d: %s, %s!", state->line, input, PHPDBG_G(err_buf).msg);
							}
						}
					break;
				}
			}

			phpdbg_activate_err_buf(0);
			phpdbg_free_err_buf();

			phpdbg_stack_free(&stack);
			phpdbg_destroy_input(&input);
		} zend_catch {
			PHPDBG_G(flags) &= ~(PHPDBG_IS_RUNNING | PHPDBG_IS_CLEANING);
			if (PHPDBG_G(flags) & PHPDBG_IS_QUITTING) {
				zend_bailout();
			}
		} zend_end_try();
	}

}

void phpdbg_string_init(char *buffer) {
	struct phpdbg_init_state state = {0};
	char *str = strtok(buffer, "\n");

	while (str) {
		phpdbg_line_init(str, &state);

		str = strtok(NULL, "\n");
	}

	if (state.code) {
		free(state.code);
	}
}

void phpdbg_try_file_init(char *init_file, size_t init_file_len, bool free_init) /* {{{ */
{
	zend_stat_t sb = {0};

	if (init_file && VCWD_STAT(init_file, &sb) != -1) {
		FILE *fp = fopen(init_file, "r");
		if (fp) {
			char cmd[PHPDBG_MAX_CMD];
			struct phpdbg_init_state state = {0};

			state.init_file = init_file;

			while (fgets(cmd, PHPDBG_MAX_CMD, fp) != NULL) {
				phpdbg_line_init(cmd, &state);
			}

			if (state.code) {
				free(state.code);
			}

			fclose(fp);
		} else {
			phpdbg_error("Failed to open %s for initialization", init_file);
		}

		if (free_init) {
			free(init_file);
		}
	}
} /* }}} */

void phpdbg_init(char *init_file, size_t init_file_len, bool use_default) /* {{{ */
{
	if (init_file) {
		phpdbg_try_file_init(init_file, init_file_len, 1);
	} else if (use_default) {
		char *scan_dir = getenv("PHP_INI_SCAN_DIR");
		char *sys_ini;
		int i;

		ZEND_IGNORE_VALUE(asprintf(&sys_ini, "%s/" PHPDBG_INIT_FILENAME, PHP_CONFIG_FILE_PATH));
		phpdbg_try_file_init(sys_ini, strlen(sys_ini), 0);
		free(sys_ini);

		if (!scan_dir) {
			scan_dir = PHP_CONFIG_FILE_SCAN_DIR;
		}
		while (*scan_dir != 0) {
			i = 0;
			while (scan_dir[i] != ':') {
				if (scan_dir[i++] == 0) {
					i = -1;
					break;
				}
			}
			if (i != -1) {
				scan_dir[i] = 0;
			}

			ZEND_IGNORE_VALUE(asprintf(&init_file, "%s/%s", scan_dir, PHPDBG_INIT_FILENAME));
			phpdbg_try_file_init(init_file, strlen(init_file), 1);
			free(init_file);
			if (i == -1) {
				break;
			}
			scan_dir += i + 1;
		}

		phpdbg_try_file_init(PHPDBG_STRL(PHPDBG_INIT_FILENAME), 0);
	}
}
/* }}} */

void phpdbg_clean(bool full, bool resubmit) /* {{{ */
{
	/* this is implicitly required */
	if (PHPDBG_G(ops)) {
		destroy_op_array(PHPDBG_G(ops));
		efree(PHPDBG_G(ops));
		PHPDBG_G(ops) = NULL;
	}

	if (!resubmit && PHPDBG_G(cur_command)) {
		free(PHPDBG_G(cur_command));
		PHPDBG_G(cur_command) = NULL;
	}

	if (full) {
		PHPDBG_G(flags) |= PHPDBG_IS_CLEANING;
	}
} /* }}} */

PHPDBG_COMMAND(exec) /* {{{ */
{
	zend_stat_t sb = {0};

	if (VCWD_STAT(param->str, &sb) != FAILURE) {
		if (sb.st_mode & (S_IFREG|S_IFLNK)) {
			char *res = phpdbg_resolve_path(param->str);
			size_t res_len = strlen(res);

			if ((res_len != PHPDBG_G(exec_len)) || (memcmp(res, PHPDBG_G(exec), res_len) != SUCCESS)) {
				if (PHPDBG_G(in_execution)) {
					if (phpdbg_ask_user_permission("Do you really want to stop execution to set a new execution context?") == FAILURE) {
						return FAILURE;
					}
				}

				if (PHPDBG_G(exec)) {
					phpdbg_notice("Unsetting old execution context: %s", PHPDBG_G(exec));
					free(PHPDBG_G(exec));
					PHPDBG_G(exec) = NULL;
					PHPDBG_G(exec_len) = 0L;
				}

				if (PHPDBG_G(ops)) {
					phpdbg_notice("Destroying compiled opcodes");
					phpdbg_clean(0, 0);
				}

				PHPDBG_G(exec) = res;
				PHPDBG_G(exec_len) = res_len;

				VCWD_CHDIR_FILE(res);

				*SG(request_info).argv = estrndup(PHPDBG_G(exec), PHPDBG_G(exec_len));
				php_build_argv(NULL, &PG(http_globals)[TRACK_VARS_SERVER]);

				phpdbg_notice("Set execution context: %s", PHPDBG_G(exec));

				if (PHPDBG_G(in_execution)) {
					phpdbg_clean(1, 0);
					return SUCCESS;
				}

				phpdbg_compile();
			} else {
				phpdbg_notice("Execution context not changed");
			}
		} else {
			phpdbg_error("Cannot use %s as execution context, not a valid file or symlink", param->str);
		}
	} else {
		phpdbg_error("Cannot stat %s, ensure the file exists", param->str);
	}
	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(stdin)
{
	smart_str code = {0};
	char *buf;
	char *sep = param->str;
	int seplen = param->len;
	int bytes = 0;

	smart_str_appends(&code, "?>");

	do {
		PHPDBG_G(input_buflen) += bytes;
		if (PHPDBG_G(input_buflen) <= 0) {
			continue;
		}

		if (sep && seplen) {
			char *nl = buf = PHPDBG_G(input_buffer);
			do {
				if (buf == nl + seplen) {
					if (!memcmp(sep, nl, seplen) && (*buf == '\n' || (*buf == '\r' && buf[1] == '\n'))) {
						smart_str_appendl(&code, PHPDBG_G(input_buffer), nl - PHPDBG_G(input_buffer));
						memmove(PHPDBG_G(input_buffer), ++buf, --PHPDBG_G(input_buflen));
						goto exec_code;
					}
				}
				if (*buf == '\n') {
					nl = buf + 1;
				}
				buf++;
			} while (--PHPDBG_G(input_buflen));
			if (buf != nl && buf <= nl + seplen) {
				smart_str_appendl(&code, PHPDBG_G(input_buffer), nl - PHPDBG_G(input_buffer));
				PHPDBG_G(input_buflen) = buf - nl;
				memmove(PHPDBG_G(input_buffer), nl, PHPDBG_G(input_buflen));
			} else {
				PHPDBG_G(input_buflen) = 0;
				smart_str_appendl(&code, PHPDBG_G(input_buffer), buf - PHPDBG_G(input_buffer));
			}
		} else {
			smart_str_appendl(&code, PHPDBG_G(input_buffer), PHPDBG_G(input_buflen));
			PHPDBG_G(input_buflen) = 0;
		}
	} while ((bytes = phpdbg_mixed_read(PHPDBG_G(io)[PHPDBG_STDIN].fd, PHPDBG_G(input_buffer) + PHPDBG_G(input_buflen), PHPDBG_MAX_CMD - PHPDBG_G(input_buflen), -1)) > 0);

	if (bytes < 0) {
		PHPDBG_G(flags) |= PHPDBG_IS_QUITTING;
		zend_bailout();
	}

exec_code:
	smart_str_0(&code);

	if (phpdbg_compile_stdin(code.s) == FAILURE) {
		zend_exception_error(EG(exception), E_ERROR);
		zend_bailout();
	}

	return SUCCESS;
} /* }}} */

int phpdbg_compile_stdin(zend_string *code) {
	PHPDBG_G(ops) = zend_compile_string(code, "Standard input code", ZEND_COMPILE_POSITION_AFTER_OPEN_TAG);
	zend_string_release(code);

	if (EG(exception)) {
		return FAILURE;
	}

	if (PHPDBG_G(exec)) {
		free(PHPDBG_G(exec));
	}
	PHPDBG_G(exec) = strdup("Standard input code");
	PHPDBG_G(exec_len) = sizeof("Standard input code") - 1;
	{ /* remove leading ?> from source */
		int i;
		/* remove trailing data after zero byte, used for avoiding conflicts in eval()'ed code snippets */
		zend_string *source_path = strpprintf(0, "Standard input code%c%p", 0, PHPDBG_G(ops)->opcodes);
		phpdbg_file_source *data = zend_hash_find_ptr(&PHPDBG_G(file_sources), source_path);
		dtor_func_t dtor = PHPDBG_G(file_sources).pDestructor;
		PHPDBG_G(file_sources).pDestructor = NULL;
		zend_hash_del(&PHPDBG_G(file_sources), source_path);
		PHPDBG_G(file_sources).pDestructor = dtor;
		zend_hash_str_update_ptr(&PHPDBG_G(file_sources), "Standard input code", sizeof("Standard input code")-1, data);
		zend_string_release(source_path);

		for (i = 1; i <= data->lines; i++) {
			data->line[i] -= 2;
		}
		data->len -= 2;
		memmove(data->buf, data->buf + 2, data->len);
	}

	phpdbg_notice("Successful compilation of stdin input");

	return SUCCESS;
}

int phpdbg_compile(void) /* {{{ */
{
	zend_file_handle fh;
	char *buf;
	size_t len;

	if (!PHPDBG_G(exec)) {
		phpdbg_error("No execution context");
		return FAILURE;
	}

	zend_stream_init_filename(&fh, PHPDBG_G(exec));
	if (php_stream_open_for_zend_ex(&fh, USE_PATH|STREAM_OPEN_FOR_INCLUDE) == SUCCESS && zend_stream_fixup(&fh, &buf, &len) == SUCCESS) {
		CG(skip_shebang) = 1;
		PHPDBG_G(ops) = zend_compile_file(&fh, ZEND_INCLUDE);
		zend_destroy_file_handle(&fh);
		if (EG(exception)) {
			zend_exception_error(EG(exception), E_ERROR);
			zend_bailout();
		}

		phpdbg_notice("Successful compilation of %s", PHPDBG_G(exec));

		return SUCCESS;
	} else {
		phpdbg_error("Could not open file %s", PHPDBG_G(exec));
	}
	zend_destroy_file_handle(&fh);
	return FAILURE;
} /* }}} */

PHPDBG_COMMAND(step) /* {{{ */
{
	if (PHPDBG_G(in_execution)) {
		PHPDBG_G(flags) |= PHPDBG_IS_STEPPING;
	}

	return PHPDBG_NEXT;
} /* }}} */

PHPDBG_COMMAND(continue) /* {{{ */
{
	return PHPDBG_NEXT;
} /* }}} */

int phpdbg_skip_line_helper(void) /* {{{ */ {
	zend_execute_data *ex = phpdbg_user_execute_data(EG(current_execute_data));
	const zend_op_array *op_array = &ex->func->op_array;
	const zend_op *opline = op_array->opcodes;

	PHPDBG_G(flags) |= PHPDBG_IN_UNTIL;
	PHPDBG_G(seek_ex) = ex;
	do {
		if (opline->lineno != ex->opline->lineno
		 || opline->opcode == ZEND_RETURN
		 || opline->opcode == ZEND_FAST_RET
		 || opline->opcode == ZEND_GENERATOR_RETURN
		 || opline->opcode == ZEND_EXIT
		 || opline->opcode == ZEND_YIELD
		 || opline->opcode == ZEND_YIELD_FROM
		) {
			zend_hash_index_update_ptr(&PHPDBG_G(seek), (zend_ulong) opline, (void *) opline);
		}
	} while (++opline < op_array->opcodes + op_array->last);

	return PHPDBG_UNTIL;
}
/* }}} */

PHPDBG_COMMAND(until) /* {{{ */
{
	if (!PHPDBG_G(in_execution)) {
		phpdbg_error("Not executing");
		return SUCCESS;
	}

	return phpdbg_skip_line_helper();
} /* }}} */

PHPDBG_COMMAND(next) /* {{{ */
{
	if (!PHPDBG_G(in_execution)) {
		phpdbg_error("Not executing");
		return SUCCESS;
	}

	PHPDBG_G(flags) |= PHPDBG_IS_STEPPING;
	return phpdbg_skip_line_helper();
} /* }}} */

static void phpdbg_seek_to_end(void) /* {{{ */ {
	zend_execute_data *ex = phpdbg_user_execute_data(EG(current_execute_data));
	const zend_op_array *op_array = &ex->func->op_array;
	const zend_op *opline = op_array->opcodes;

	PHPDBG_G(seek_ex) = ex;
	do {
		switch (opline->opcode) {
			case ZEND_RETURN:
			case ZEND_FAST_RET:
			case ZEND_GENERATOR_RETURN:
			case ZEND_EXIT:
			case ZEND_YIELD:
			case ZEND_YIELD_FROM:
				zend_hash_index_update_ptr(&PHPDBG_G(seek), (zend_ulong) opline, (void *) opline);
		}
	} while (++opline < op_array->opcodes + op_array->last);
}
/* }}} */

PHPDBG_COMMAND(finish) /* {{{ */
{
	if (!PHPDBG_G(in_execution)) {
		phpdbg_error("Not executing");
		return SUCCESS;
	}

	phpdbg_seek_to_end();
	if (zend_hash_index_exists(&PHPDBG_G(seek), (zend_ulong) phpdbg_user_execute_data(EG(current_execute_data))->opline)) {
		zend_hash_clean(&PHPDBG_G(seek));
	} else {
		PHPDBG_G(flags) |= PHPDBG_IN_FINISH;
	}

	return PHPDBG_FINISH;
} /* }}} */

PHPDBG_COMMAND(leave) /* {{{ */
{
	if (!PHPDBG_G(in_execution)) {
		phpdbg_error("Not executing");
		return SUCCESS;
	}

	phpdbg_seek_to_end();
	if (zend_hash_index_exists(&PHPDBG_G(seek), (zend_ulong) phpdbg_user_execute_data(EG(current_execute_data))->opline)) {
		zend_hash_clean(&PHPDBG_G(seek));
		phpdbg_notice("Already at the end of the function");
		return SUCCESS;
	} else {
		PHPDBG_G(flags) |= PHPDBG_IN_LEAVE;
		return PHPDBG_LEAVE;
	}
} /* }}} */

PHPDBG_COMMAND(frame) /* {{{ */
{
	if (!param) {
		phpdbg_notice("Currently in frame #%d", PHPDBG_G(frame).num);
	} else {
		phpdbg_switch_frame(param->num);
	}

	return SUCCESS;
} /* }}} */

static inline void phpdbg_handle_exception(void) /* {{{ */
{
	zend_object *ex = EG(exception);
	zend_string *msg, *file;
	zend_long line;
	zval rv, tmp;

	EG(exception) = NULL;

	zend_call_known_instance_method_with_0_params(ex->ce->__tostring, ex, &tmp);
	file = zval_get_string(zend_read_property(zend_get_exception_base(ex), ex, ZEND_STRL("file"), 1, &rv));
	line = zval_get_long(zend_read_property(zend_get_exception_base(ex), ex, ZEND_STRL("line"), 1, &rv));

	if (EG(exception)) {
		EG(exception) = NULL;
		msg = ZSTR_EMPTY_ALLOC();
	} else {
		zend_update_property_string(zend_get_exception_base(ex), ex, ZEND_STRL("string"), Z_STRVAL(tmp));
		zval_ptr_dtor(&tmp);
		msg = zval_get_string(zend_read_property(zend_get_exception_base(ex), ex, ZEND_STRL("string"), 1, &rv));
	}

	phpdbg_error("Uncaught %s in %s on line " ZEND_LONG_FMT, ZSTR_VAL(ex->ce->name), ZSTR_VAL(file), line);
	zend_string_release(file);
	phpdbg_writeln("%s", ZSTR_VAL(msg));
	zend_string_release(msg);

	if (EG(prev_exception)) {
		OBJ_RELEASE(EG(prev_exception));
		EG(prev_exception) = 0;
	}
	OBJ_RELEASE(ex);
	EG(opline_before_exception) = NULL;

	EG(exit_status) = 255;
} /* }}} */

PHPDBG_COMMAND(run) /* {{{ */
{
	if (PHPDBG_G(ops) || PHPDBG_G(exec)) {
		zend_execute_data *ex = EG(current_execute_data);
		bool restore = 1;

		if (PHPDBG_G(in_execution)) {
			if (phpdbg_ask_user_permission("Do you really want to restart execution?") == SUCCESS) {
				phpdbg_startup_run++;
				phpdbg_clean(1, 1);
			}
			return SUCCESS;
		}

		if (!PHPDBG_G(ops)) {
			if (phpdbg_compile() == FAILURE) {
				phpdbg_error("Failed to compile %s, cannot run", PHPDBG_G(exec));
				EG(exit_status) = FAILURE;
				goto out;
			}
		}

		if (param && param->type != EMPTY_PARAM && param->len != 0) {
			char **argv = emalloc(5 * sizeof(char *));
			char *end = param->str + param->len, *p = param->str;
			char last_byte;
			int argc = 0;
			int i;

			while (*end == '\r' || *end == '\n') *(end--) = 0;
			last_byte = end[1];
			end[1] = 0;

			while (*p == ' ') p++;
			while (*p) {
				char sep = ' ';
				char *buf = emalloc(end - p + 2), *q = buf;

				if (*p == '<') {
					/* use as STDIN */
					do p++; while (*p == ' ');

					if (*p == '\'' || *p == '"') {
						sep = *(p++);
					}
					while (*p && *p != sep) {
						if (*p == '\\' && (p[1] == sep || p[1] == '\\')) {
							p++;
						}
						*(q++) = *(p++);
					}
					*(q++) = 0;
					if (*p) {
						do p++; while (*p == ' ');
					}

					if (*p) {
						phpdbg_error("Invalid run command, cannot put further arguments after stdin");
						goto free_cmd;
					}

					PHPDBG_G(stdin_file) = fopen(buf, "r");
					if (PHPDBG_G(stdin_file) == NULL) {
						phpdbg_error("Could not open '%s' for reading from stdin", buf);
						goto free_cmd;
					}
					efree(buf);
					phpdbg_register_file_handles();
					break;
				}

				if (argc >= 4 && argc == (argc & -argc)) {
					argv = erealloc(argv, (argc * 2 + 1) * sizeof(char *));
				}

				if (*p == '\'' || *p == '"') {
					sep = *(p++);
				}
				if (*p == '\\' && (p[1] == '<' || p[1] == '\'' || p[1] == '"')) {
					p++;
				}
				while (*p && *p != sep) {
					if (*p == '\\' && (p[1] == sep || p[1] == '\\' || (p[1] == '#' && sep == ' '))) {
						p++;
					}
					*(q++) = *(p++);
				}
				if (!*p && sep != ' ') {
					phpdbg_error("Invalid run command, unterminated escape sequence");
free_cmd:
					efree(buf);
					for (i = 0; i < argc; i++) {
						efree(argv[i]);
					}
					efree(argv);
					end[1] = last_byte;
					return SUCCESS;
				}

				*(q++) = 0;
				argv[++argc] = erealloc(buf, q - buf);

				if (*p) {
					do p++; while (*p == ' ');
				}
			}
			end[1] = last_byte;

			argv[0] = SG(request_info).argv[0];
			for (i = SG(request_info).argc; --i;) {
				efree(SG(request_info).argv[i]);
			}
			efree(SG(request_info).argv);
			SG(request_info).argv = erealloc(argv, ++argc * sizeof(char *));
			SG(request_info).argc = argc;

			php_build_argv(NULL, &PG(http_globals)[TRACK_VARS_SERVER]);
		}

		/* clean up from last execution */
		if (ex && (ZEND_CALL_INFO(ex) & ZEND_CALL_HAS_SYMBOL_TABLE)) {
			zend_hash_clean(ex->symbol_table);
		} else {
			zend_rebuild_symbol_table();
		}
		PHPDBG_G(handled_exception) = NULL;

		/* clean seek state */
		PHPDBG_G(flags) &= ~PHPDBG_SEEK_MASK;
		zend_hash_clean(&PHPDBG_G(seek));

		/* reset hit counters */
		phpdbg_reset_breakpoints();

		zend_try {
			PHPDBG_G(flags) ^= PHPDBG_IS_INTERACTIVE;
			PHPDBG_G(flags) |= PHPDBG_IS_RUNNING;
			zend_execute(PHPDBG_G(ops), &PHPDBG_G(retval));
			PHPDBG_G(flags) ^= PHPDBG_IS_INTERACTIVE;
		} zend_catch {
			PHPDBG_G(in_execution) = 0;

			if (!(PHPDBG_G(flags) & PHPDBG_IS_STOPPING)) {
				restore = 0;
			} else {
				zend_bailout();
			}
		} zend_end_try();

		if (restore) {
			zend_exception_restore();
			zend_try {
				zend_try_exception_handler();
				PHPDBG_G(in_execution) = 1;
			} zend_catch {
				PHPDBG_G(in_execution) = 0;

				if (PHPDBG_G(flags) & PHPDBG_IS_STOPPING) {
					zend_bailout();
				}
			} zend_end_try();

			if (EG(exception)) {
				phpdbg_handle_exception();
			}
		}

		PHPDBG_G(flags) &= ~PHPDBG_IS_RUNNING;

		phpdbg_clean(1, 0);
	} else {
		phpdbg_error("Nothing to execute!");
	}

out:
	PHPDBG_FRAME(num) = 0;
	return SUCCESS;
} /* }}} */

int phpdbg_output_ev_variable(char *name, size_t len, char *keyname, size_t keylen, HashTable *parent, zval *zv) /* {{{ */ {
	phpdbg_notice("Printing variable %.*s", (int) len, name);

	zend_print_zval_r(zv, 0);

	phpdbg_out("\n");

	efree(name);
	efree(keyname);

	return SUCCESS;
}
/* }}} */

PHPDBG_COMMAND(ev) /* {{{ */
{
	bool stepping = ((PHPDBG_G(flags) & PHPDBG_IS_STEPPING) == PHPDBG_IS_STEPPING);
	zval retval;

	zend_execute_data *original_execute_data = EG(current_execute_data);
	zend_vm_stack original_stack = EG(vm_stack);
	zend_object *ex = NULL;

	PHPDBG_OUTPUT_BACKUP();

	original_stack->top = EG(vm_stack_top);

	if (PHPDBG_G(flags) & PHPDBG_IN_SIGNAL_HANDLER) {
		phpdbg_try_access {
			phpdbg_parse_variable(param->str, param->len, &EG(symbol_table), 0, phpdbg_output_ev_variable, 0);
		} phpdbg_catch_access {
			phpdbg_error("Could not fetch data, invalid data source");
		} phpdbg_end_try_access();

		PHPDBG_OUTPUT_BACKUP_RESTORE();
		return SUCCESS;
	}

	if (!(PHPDBG_G(flags) & PHPDBG_IS_STEPONEVAL)) {
		PHPDBG_G(flags) &= ~PHPDBG_IS_STEPPING;
	}

	/* disable stepping while eval() in progress */
	PHPDBG_G(flags) |= PHPDBG_IN_EVAL;
	zend_try {
		if (zend_eval_stringl(param->str, param->len, &retval, "eval()'d code") == SUCCESS) {
			if (EG(exception)) {
				ex = EG(exception);
				zend_exception_error(EG(exception), E_ERROR);
			} else {
				zend_print_zval_r(&retval, 0);
				phpdbg_out("\n");
				zval_ptr_dtor(&retval);
			}
		}
	} zend_catch {
		PHPDBG_G(unclean_eval) = 1;
		if (ex) {
			OBJ_RELEASE(ex);
		}
		EG(current_execute_data) = original_execute_data;
		EG(vm_stack_top) = original_stack->top;
		EG(vm_stack_end) = original_stack->end;
		EG(vm_stack) = original_stack;
		EG(exit_status) = 0;
	} zend_end_try();

	PHPDBG_G(flags) &= ~PHPDBG_IN_EVAL;

	/* switch stepping back on */
	if (stepping && !(PHPDBG_G(flags) & PHPDBG_IS_STEPONEVAL)) {
		PHPDBG_G(flags) |= PHPDBG_IS_STEPPING;
	}

	CG(unclean_shutdown) = 0;

	PHPDBG_OUTPUT_BACKUP_RESTORE();

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(back) /* {{{ */
{
	if (!PHPDBG_G(in_execution)) {
		phpdbg_error("Not executing!");
		return SUCCESS;
	}

	if (!param) {
		phpdbg_dump_backtrace(0);
	} else {
		phpdbg_dump_backtrace(param->num);
	}

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(generator) /* {{{ */
{
	int i;

	if (!PHPDBG_G(in_execution)) {
		phpdbg_error("Not executing!");
		return SUCCESS;
	}

	if (param) {
		i = param->num;
		zend_object **obj = EG(objects_store).object_buckets + i;
		if (i < EG(objects_store).top && *obj && IS_OBJ_VALID(*obj) && (*obj)->ce == zend_ce_generator) {
			zend_generator *gen = (zend_generator *) *obj;
			if (gen->execute_data) {
				if (zend_generator_get_current(gen)->flags & ZEND_GENERATOR_CURRENTLY_RUNNING) {
					phpdbg_error("Generator currently running");
				} else {
					phpdbg_open_generator_frame(gen);
				}
			} else {
				phpdbg_error("Generator already closed");
			}
		} else {
			phpdbg_error("Invalid object handle");
		}
	} else {
		for (i = 0; i < EG(objects_store).top; i++) {
			zend_object *obj = EG(objects_store).object_buckets[i];
			if (obj && IS_OBJ_VALID(obj) && obj->ce == zend_ce_generator) {
				zend_generator *gen = (zend_generator *) obj, *current = zend_generator_get_current(gen);
				if (gen->execute_data) {
					zend_string *s = phpdbg_compile_stackframe(gen->execute_data);
					phpdbg_out("#%d: %.*s", i, (int) ZSTR_LEN(s), ZSTR_VAL(s));
					zend_string_release(s);
					if (gen != current) {
						if (gen->node.parent != current) {
							phpdbg_out(" with direct parent #%d and", gen->node.parent->std.handle);
						}
						phpdbg_out(" executing #%d currently", current->std.handle);
					}
					phpdbg_out("\n");
				}
			}
		}
	}

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(print) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		return phpdbg_do_print_stack(param);
	} else switch (param->type) {
		case STR_PARAM:
			return phpdbg_do_print_func(param);
		case METHOD_PARAM:
			return phpdbg_do_print_method(param);
		default:
			phpdbg_error("Invalid arguments to print, expected nothing, function name or method name");
			return SUCCESS;
	}
} /* }}} */

PHPDBG_COMMAND(info) /* {{{ */
{
	phpdbg_out("Execution Context Information\n\n");
#ifdef HAVE_PHPDBG_READLINE
# ifdef HAVE_LIBREADLINE
	 phpdbg_writeln( "Readline   yes");
# else
	 phpdbg_writeln("Readline   no");
# endif
# ifdef HAVE_LIBEDIT
	 phpdbg_writeln("Libedit    yes");
# else
	 phpdbg_writeln("Libedit    no");
# endif
#else
	phpdbg_writeln("Readline   unavailable");
#endif

	phpdbg_writeln("Exec       %s", PHPDBG_G(exec) ? PHPDBG_G(exec) : "none");
	phpdbg_writeln("Compiled   %s", PHPDBG_G(ops) ? "yes" : "no");
	phpdbg_writeln("Stepping   %s", (PHPDBG_G(flags) & PHPDBG_IS_STEPPING) ? "on" : "off");
	phpdbg_writeln("Quietness  %s", (PHPDBG_G(flags) & PHPDBG_IS_QUIET) ? "on" : "off");

	if (PHPDBG_G(ops)) {
		phpdbg_writeln("Opcodes    %d", PHPDBG_G(ops)->last);
		phpdbg_writeln("Variables  %d", PHPDBG_G(ops)->last_var ? PHPDBG_G(ops)->last_var - 1 : 0);
	}

	phpdbg_writeln("Executing  %s", PHPDBG_G(in_execution) ? "yes" : "no");
	if (PHPDBG_G(in_execution)) {
		phpdbg_writeln("VM Return  %d", PHPDBG_G(vmret));
	}

	phpdbg_writeln("Classes    %d", zend_hash_num_elements(EG(class_table)));
	phpdbg_writeln("Functions  %d", zend_hash_num_elements(EG(function_table)));
	phpdbg_writeln("Constants  %d", zend_hash_num_elements(EG(zend_constants)));
	phpdbg_writeln("Included   %d", zend_hash_num_elements(&EG(included_files)));

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(set) /* {{{ */
{
	phpdbg_error("No set command selected!");

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(break) /* {{{ */
{
	if (!param) {
		if (PHPDBG_G(exec)) {
			phpdbg_set_breakpoint_file(
				zend_get_executed_filename(),
				strlen(zend_get_executed_filename()),
				zend_get_executed_lineno());
		} else {
			phpdbg_error("Execution context not set!");
		}
	} else switch (param->type) {
		case ADDR_PARAM:
			phpdbg_set_breakpoint_opline(param->addr);
			break;
		case NUMERIC_PARAM:
			if (PHPDBG_G(exec)) {
				phpdbg_set_breakpoint_file(phpdbg_current_file(), strlen(phpdbg_current_file()), param->num);
			} else {
				phpdbg_error("Execution context not set!");
			}
			break;
		case METHOD_PARAM:
			phpdbg_set_breakpoint_method(param->method.class, param->method.name);
			break;
		case NUMERIC_METHOD_PARAM:
			phpdbg_set_breakpoint_method_opline(param->method.class, param->method.name, param->num);
			break;
		case NUMERIC_FUNCTION_PARAM:
			phpdbg_set_breakpoint_function_opline(param->str, param->num);
			break;
		case FILE_PARAM:
			phpdbg_set_breakpoint_file(param->file.name, 0, param->file.line);
			break;
		case NUMERIC_FILE_PARAM:
			phpdbg_set_breakpoint_file_opline(param->file.name, param->file.line);
			break;
		case COND_PARAM:
			phpdbg_set_breakpoint_expression(param->str, param->len);
			break;
		case STR_PARAM:
			phpdbg_set_breakpoint_symbol(param->str, param->len);
			break;
		case OP_PARAM:
			phpdbg_set_breakpoint_opcode(param->str, param->len);
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(sh) /* {{{ */
{
	FILE *fd = NULL;
	if ((fd=VCWD_POPEN((char*)param->str, "w"))) {
		/* TODO: do something perhaps ?? do we want input ?? */
		pclose(fd);
	} else {
		phpdbg_error("Failed to execute %s", param->str);
	}

	return SUCCESS;
} /* }}} */

static int add_module_info(zend_module_entry *module) /* {{{ */ {
	phpdbg_write("%s\n", module->name);
	return 0;
}
/* }}} */

static void add_zendext_info(zend_extension *ext) /* {{{ */ {
	phpdbg_write("%s\n", ext->name);
}
/* }}} */

#ifdef HAVE_LIBDL
PHPDBG_API const char *phpdbg_load_module_or_extension(char **path, const char **name) /* {{{ */ {
	DL_HANDLE handle;
	char *extension_dir;

	extension_dir = INI_STR("extension_dir");

	if (strchr(*path, '/') != NULL || strchr(*path, DEFAULT_SLASH) != NULL) {
		/* path is fine */
	} else if (extension_dir && extension_dir[0]) {
		char *libpath;
		int extension_dir_len = strlen(extension_dir);
		if (IS_SLASH(extension_dir[extension_dir_len-1])) {
			spprintf(&libpath, 0, "%s%s", extension_dir, *path); /* SAFE */
		} else {
			spprintf(&libpath, 0, "%s%c%s", extension_dir, DEFAULT_SLASH, *path); /* SAFE */
		}
		efree(*path);
		*path = libpath;
	} else {
		phpdbg_error("Not a full path given or extension_dir ini setting is not set");

		return NULL;
	}

	handle = DL_LOAD(*path);

	if (!handle) {
#ifdef PHP_WIN32
		char *err = GET_DL_ERROR();
		if (err && err[0]) {
			phpdbg_error("%s", err);
			php_win32_error_msg_free(err);
		} else {
			phpdbg_error("Unknown reason");
		}
#else
		phpdbg_error("%s", GET_DL_ERROR());
#endif
		return NULL;
	}

#if ZEND_EXTENSIONS_SUPPORT
	do {
		zend_extension *new_extension;
		zend_extension_version_info *extension_version_info;

		extension_version_info = (zend_extension_version_info *) DL_FETCH_SYMBOL(handle, "extension_version_info");
		if (!extension_version_info) {
			extension_version_info = (zend_extension_version_info *) DL_FETCH_SYMBOL(handle, "_extension_version_info");
		}
		new_extension = (zend_extension *) DL_FETCH_SYMBOL(handle, "zend_extension_entry");
		if (!new_extension) {
			new_extension = (zend_extension *) DL_FETCH_SYMBOL(handle, "_zend_extension_entry");
		}
		if (!extension_version_info || !new_extension) {
			break;
		}
		if (extension_version_info->zend_extension_api_no != ZEND_EXTENSION_API_NO &&(!new_extension->api_no_check || new_extension->api_no_check(ZEND_EXTENSION_API_NO) != SUCCESS)) {
			phpdbg_error("%s requires Zend Engine API version %d, which does not match the installed Zend Engine API version %d", new_extension->name, extension_version_info->zend_extension_api_no, ZEND_EXTENSION_API_NO);

			goto quit;
		} else if (strcmp(ZEND_EXTENSION_BUILD_ID, extension_version_info->build_id) && (!new_extension->build_id_check || new_extension->build_id_check(ZEND_EXTENSION_BUILD_ID) != SUCCESS)) {
			phpdbg_error("%s was built with configuration %s, whereas running engine is %s", new_extension->name, extension_version_info->build_id, ZEND_EXTENSION_BUILD_ID);

			goto quit;
		}

		*name = new_extension->name;

		zend_register_extension(new_extension, handle);

		if (new_extension->startup) {
			if (new_extension->startup(new_extension) != SUCCESS) {
				phpdbg_error("Unable to startup Zend extension %s", new_extension->name);

				goto quit;
			}
			zend_append_version_info(new_extension);
		}

		return "Zend extension";
	} while (0);
#endif

	do {
		zend_module_entry *module_entry;
		zend_module_entry *(*get_module)(void);

		get_module = (zend_module_entry *(*)(void)) DL_FETCH_SYMBOL(handle, "get_module");
		if (!get_module) {
			get_module = (zend_module_entry *(*)(void)) DL_FETCH_SYMBOL(handle, "_get_module");
		}

		if (!get_module) {
			break;
		}

		module_entry = get_module();
		*name = module_entry->name;

		if (strcmp(ZEND_EXTENSION_BUILD_ID, module_entry->build_id)) {
			phpdbg_error("%s was built with configuration %s, whereas running engine is %s", module_entry->name, module_entry->build_id, ZEND_EXTENSION_BUILD_ID);

			goto quit;
		}

		module_entry->type = MODULE_PERSISTENT;
		module_entry->module_number = zend_next_free_module();
		module_entry->handle = handle;

		if ((module_entry = zend_register_module_ex(module_entry)) == NULL) {
			phpdbg_error("Unable to register module %s", *name);

			goto quit;
		}

		if (zend_startup_module_ex(module_entry) == FAILURE) {
			phpdbg_error("Unable to startup module %s", module_entry->name);

			goto quit;
		}

		if (module_entry->request_startup_func) {
			if (module_entry->request_startup_func(MODULE_PERSISTENT, module_entry->module_number) == FAILURE) {
				phpdbg_error("Unable to initialize module %s", module_entry->name);

				goto quit;
			}
		}

		return "module";
	} while (0);

	phpdbg_error("This shared object is nor a Zend extension nor a module");

quit:
	DL_UNLOAD(handle);
	return NULL;
}
/* }}} */
#endif

PHPDBG_COMMAND(dl) /* {{{ */
{
	const char *type, *name;
	char *path;

	if (!param || param->type == EMPTY_PARAM) {
		phpdbg_notice("Zend extensions");
		zend_llist_apply(&zend_extensions, (llist_apply_func_t) add_zendext_info);
		phpdbg_out("\n");
		phpdbg_notice("Modules");
		zend_hash_apply(&module_registry, (apply_func_t) add_module_info);
	} else switch (param->type) {
		case STR_PARAM:
#ifdef HAVE_LIBDL
			path = estrndup(param->str, param->len);

			phpdbg_activate_err_buf(1);
			if ((type = phpdbg_load_module_or_extension(&path, &name)) == NULL) {
				phpdbg_error("Could not load %s, not found or invalid zend extension / module: %s", path, PHPDBG_G(err_buf).msg);
			} else {
				phpdbg_notice("Successfully loaded the %s %s at path %s", type, name, path);
			}
			phpdbg_activate_err_buf(0);
			phpdbg_free_err_buf();
			efree(path);
#else
			phpdbg_error("Cannot dynamically load %.*s - dynamic modules are not supported", (int) param->len, param->str);
#endif
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(source) /* {{{ */
{
	zend_stat_t sb = {0};

	if (VCWD_STAT(param->str, &sb) != -1) {
		phpdbg_try_file_init(param->str, param->len, 0);
	} else {
		phpdbg_error("Failed to stat %s, file does not exist", param->str);
	}

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(export) /* {{{ */
{
	FILE *handle = VCWD_FOPEN(param->str, "w+");

	if (handle) {
		phpdbg_export_breakpoints(handle);
		fclose(handle);
	} else {
		phpdbg_error("Failed to open or create %s, check path and permissions", param->str);
	}

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(register) /* {{{ */
{
	zend_function *function;
	char *lcname = zend_str_tolower_dup(param->str, param->len);
	size_t lcname_len = strlen(lcname);

	if (!zend_hash_str_exists(&PHPDBG_G(registered), lcname, lcname_len)) {
		if ((function = zend_hash_str_find_ptr(EG(function_table), lcname, lcname_len))) {
			zend_hash_str_update_ptr(&PHPDBG_G(registered), lcname, lcname_len, function);
			function_add_ref(function);

			phpdbg_notice("Registered %s", lcname);
		} else {
			phpdbg_error("The requested function (%s) could not be found", param->str);
		}
	} else {
		phpdbg_error("The requested name (%s) is already in use", lcname);
	}

	efree(lcname);
	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(quit) /* {{{ */
{
	PHPDBG_G(flags) |= PHPDBG_IS_QUITTING;
	PHPDBG_G(flags) &= ~PHPDBG_IS_CLEANING;

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(clean) /* {{{ */
{
	if (PHPDBG_G(in_execution)) {
		if (phpdbg_ask_user_permission("Do you really want to clean your current environment?") == FAILURE) {
			return SUCCESS;
		}
	}

	phpdbg_out("Cleaning Execution Environment\n");

	phpdbg_writeln("Classes    %d", zend_hash_num_elements(EG(class_table)));
	phpdbg_writeln("Functions  %d", zend_hash_num_elements(EG(function_table)));
	phpdbg_writeln("Constants  %d", zend_hash_num_elements(EG(zend_constants)));
	phpdbg_writeln("Includes   %d", zend_hash_num_elements(&EG(included_files)));

	phpdbg_clean(1, 0);

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(clear) /* {{{ */
{
	phpdbg_out("Clearing Breakpoints\n");

	phpdbg_writeln("File              %d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE]));
	phpdbg_writeln("Functions         %d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM]));
	phpdbg_writeln("Methods           %d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD]));
	phpdbg_writeln("Oplines           %d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]));
	phpdbg_writeln("File oplines      %d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_OPLINE]));
	phpdbg_writeln("Function oplines  %d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_FUNCTION_OPLINE]));
	phpdbg_writeln("Method oplines    %d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD_OPLINE]));
	phpdbg_writeln("Conditionals      %d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_COND]));

	phpdbg_clear_breakpoints();

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(list) /* {{{ */
{
	if (!param) {
		return PHPDBG_LIST_HANDLER(lines)(PHPDBG_COMMAND_ARGS);
	} else switch (param->type) {
		case NUMERIC_PARAM:
			return PHPDBG_LIST_HANDLER(lines)(PHPDBG_COMMAND_ARGS);

		case FILE_PARAM:
			return PHPDBG_LIST_HANDLER(lines)(PHPDBG_COMMAND_ARGS);

		case STR_PARAM:
			phpdbg_list_function_byname(param->str, param->len);
			break;

		case METHOD_PARAM:
			return PHPDBG_LIST_HANDLER(method)(PHPDBG_COMMAND_ARGS);

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(watch) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		phpdbg_list_watchpoints();
	} else switch (param->type) {
		case STR_PARAM:
			phpdbg_create_var_watchpoint(param->str, param->len);
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

int phpdbg_interactive(bool allow_async_unsafe, char *input) /* {{{ */
{
	int ret = SUCCESS;
	phpdbg_param_t stack;

	PHPDBG_G(flags) |= PHPDBG_IS_INTERACTIVE;

	while (ret == SUCCESS || ret == FAILURE) {
		if (PHPDBG_G(flags) & PHPDBG_IS_STOPPING) {
			zend_bailout();
		}

		if (!input && !(input = phpdbg_read_input(NULL))) {
			break;
		}


		phpdbg_init_param(&stack, STACK_PARAM);

		if (phpdbg_do_parse(&stack, input) <= 0) {
			phpdbg_activate_err_buf(1);

			zend_try {
				ret = phpdbg_stack_execute(&stack, allow_async_unsafe);
			} zend_catch {
				phpdbg_stack_free(&stack);
				zend_bailout();
			} zend_end_try();

			switch (ret) {
				case FAILURE:
					if (!(PHPDBG_G(flags) & PHPDBG_IS_STOPPING)) {
						if (!allow_async_unsafe || phpdbg_call_register(&stack) == FAILURE) {
							if (PHPDBG_G(err_buf).active) {
							    phpdbg_output_err_buf("%s", PHPDBG_G(err_buf).msg);
							}
						}
					}
				break;

				case PHPDBG_LEAVE:
				case PHPDBG_FINISH:
				case PHPDBG_UNTIL:
				case PHPDBG_NEXT: {
					phpdbg_activate_err_buf(0);
					phpdbg_free_err_buf();
					if (!PHPDBG_G(in_execution) && !(PHPDBG_G(flags) & PHPDBG_IS_STOPPING)) {
						phpdbg_error("Not running");
					}
					break;
				}
			}

			phpdbg_activate_err_buf(0);
			phpdbg_free_err_buf();
		}

		phpdbg_stack_free(&stack);
		phpdbg_destroy_input(&input);
		PHPDBG_G(req_id) = 0;
		input = NULL;
	}

	if (input) {
		phpdbg_stack_free(&stack);
		phpdbg_destroy_input(&input);
		PHPDBG_G(req_id) = 0;
	}

	if (PHPDBG_G(in_execution)) {
		phpdbg_restore_frame();
	}

	PHPDBG_G(flags) &= ~PHPDBG_IS_INTERACTIVE;

	phpdbg_print_changed_zvals();

	return ret;
} /* }}} */

static inline void list_code(void) {
	if (!(PHPDBG_G(flags) & PHPDBG_IN_EVAL)) {
		const char *file_char = zend_get_executed_filename();
		zend_string *file = zend_string_init(file_char, strlen(file_char), 0);
		phpdbg_list_file(file, 3, zend_get_executed_lineno()-1, zend_get_executed_lineno());
		efree(file);
	}
}

/* code may behave weirdly if EG(exception) is set; thus backup it */
#define DO_INTERACTIVE(allow_async_unsafe) do { \
	if (exception) { \
		const zend_op *before_ex = EG(opline_before_exception); \
		const zend_op *backup_opline = NULL; \
		if (EG(current_execute_data) && EG(current_execute_data)->func && ZEND_USER_CODE(EG(current_execute_data)->func->common.type)) { \
			backup_opline = EG(current_execute_data)->opline; \
		} \
		GC_ADDREF(exception); \
		zend_clear_exception(); \
		list_code(); \
		switch (phpdbg_interactive(allow_async_unsafe, NULL)) { \
			case PHPDBG_LEAVE: \
			case PHPDBG_FINISH: \
			case PHPDBG_UNTIL: \
			case PHPDBG_NEXT: \
				if (backup_opline \
				 && (backup_opline->opcode == ZEND_HANDLE_EXCEPTION || backup_opline->opcode == ZEND_CATCH)) { \
					EG(current_execute_data)->opline = backup_opline; \
					EG(exception) = exception; \
				} else { \
					zend_throw_exception_internal(exception); \
				} \
				EG(opline_before_exception) = before_ex; \
		} \
	} else { \
		list_code(); \
		phpdbg_interactive(allow_async_unsafe, NULL); \
	} \
	goto next; \
} while (0)

void phpdbg_execute_ex(zend_execute_data *execute_data) /* {{{ */
{
	bool original_in_execution = PHPDBG_G(in_execution);

	if ((PHPDBG_G(flags) & PHPDBG_IS_STOPPING) && !(PHPDBG_G(flags) & PHPDBG_IS_RUNNING)) {
		zend_bailout();
	}

	PHPDBG_G(in_execution) = 1;

	while (1) {
		zend_object *exception = EG(exception);

		if ((PHPDBG_G(flags) & PHPDBG_BP_RESOLVE_MASK)) {
			/* resolve nth opline breakpoints */
			phpdbg_resolve_op_array_breaks(&execute_data->func->op_array);
		}

#ifdef ZEND_WIN32
		if (zend_atomic_bool_load_ex(&EG(timed_out))) {
			zend_timeout();
		}
#endif

		if (exception && zend_is_unwind_exit(exception)) {
			/* Restore bailout based exit. */
			zend_bailout();
		}

		if (PHPDBG_G(flags) & PHPDBG_PREVENT_INTERACTIVE) {
			phpdbg_print_opline(execute_data, 0);
			goto next;
		}

		/* check for uncaught exceptions */
		if (exception && PHPDBG_G(handled_exception) != exception && !(PHPDBG_G(flags) & PHPDBG_IN_EVAL)) {
			zend_execute_data *prev_ex = execute_data;

			do {
				prev_ex = zend_generator_check_placeholder_frame(prev_ex);
				/* assuming that no internal functions will silently swallow exceptions ... */
				if (!prev_ex->func || !ZEND_USER_CODE(prev_ex->func->common.type)) {
					continue;
				}

				if (phpdbg_check_caught_ex(prev_ex, exception)) {
					goto ex_is_caught;
				}
			} while ((prev_ex = prev_ex->prev_execute_data));

			PHPDBG_G(handled_exception) = exception;

			zval rv;
			zend_string *file = zval_get_string(zend_read_property(zend_get_exception_base(exception), exception, ZEND_STRL("file"), 1, &rv));
			zend_long line = zval_get_long(zend_read_property(zend_get_exception_base(exception), exception, ZEND_STRL("line"), 1, &rv));
			zend_string *msg = zval_get_string(zend_read_property(zend_get_exception_base(exception), exception, ZEND_STRL("message"), 1, &rv));

			phpdbg_error("Uncaught %s in %s on line " ZEND_LONG_FMT ": %.*s",
				ZSTR_VAL(exception->ce->name), ZSTR_VAL(file), line,
				ZSTR_LEN(msg) < 80 ? (int) ZSTR_LEN(msg) : 80, ZSTR_VAL(msg));
			zend_string_release(msg);
			zend_string_release(file);

			DO_INTERACTIVE(1);
		}
ex_is_caught:

		/* allow conditional breakpoints and initialization to access the vm uninterrupted */
		if (PHPDBG_G(flags) & (PHPDBG_IN_COND_BP | PHPDBG_IS_INITIALIZING)) {
			/* skip possible breakpoints */
			goto next;
		}

		/* not while in conditionals */
		phpdbg_print_opline(execute_data, 0);

		/* perform seek operation */
		if ((PHPDBG_G(flags) & PHPDBG_SEEK_MASK) && !(PHPDBG_G(flags) & PHPDBG_IN_EVAL)) {
			/* current address */
			zend_ulong address = (zend_ulong) execute_data->opline;

			if (PHPDBG_G(seek_ex) != execute_data) {
				if (PHPDBG_G(flags) & PHPDBG_IS_STEPPING) {
					goto stepping;
				}
				goto next;
			}

#define INDEX_EXISTS_CHECK (zend_hash_index_exists(&PHPDBG_G(seek), address) || (exception && phpdbg_check_caught_ex(execute_data, exception) == 0))

			/* run to next line */
			if (PHPDBG_G(flags) & PHPDBG_IN_UNTIL) {
				if (INDEX_EXISTS_CHECK) {
					PHPDBG_G(flags) &= ~PHPDBG_IN_UNTIL;
					zend_hash_clean(&PHPDBG_G(seek));
				} else {
					/* skip possible breakpoints */
					goto next;
				}
			}

			/* run to finish */
			if (PHPDBG_G(flags) & PHPDBG_IN_FINISH) {
				if (INDEX_EXISTS_CHECK) {
					PHPDBG_G(flags) &= ~PHPDBG_IN_FINISH;
					zend_hash_clean(&PHPDBG_G(seek));
				}
				/* skip possible breakpoints */
				goto next;
			}

			/* break for leave */
			if (PHPDBG_G(flags) & PHPDBG_IN_LEAVE) {
				if (INDEX_EXISTS_CHECK) {
					PHPDBG_G(flags) &= ~PHPDBG_IN_LEAVE;
					zend_hash_clean(&PHPDBG_G(seek));
					phpdbg_notice("Breaking for leave at %s:%u",
						zend_get_executed_filename(),
						zend_get_executed_lineno()
					);
					DO_INTERACTIVE(1);
				} else {
					/* skip possible breakpoints */
					goto next;
				}
			}
		}

		if (PHPDBG_G(flags) & PHPDBG_IS_STEPPING && (PHPDBG_G(flags) & PHPDBG_STEP_OPCODE || execute_data->opline->lineno != PHPDBG_G(last_line))) {
stepping:
			PHPDBG_G(flags) &= ~PHPDBG_IS_STEPPING;
			DO_INTERACTIVE(1);
		}

		/* check if some watchpoint was hit */
		{
			if (phpdbg_print_changed_zvals() == SUCCESS) {
				DO_INTERACTIVE(1);
			}
		}

		/* search for breakpoints */
		{
			phpdbg_breakbase_t *brake;

			if ((PHPDBG_G(flags) & PHPDBG_BP_MASK)
			    && (brake = phpdbg_find_breakpoint(execute_data))
			    && (brake->type != PHPDBG_BREAK_FILE || execute_data->opline->lineno != PHPDBG_G(last_line))) {
				phpdbg_hit_breakpoint(brake, 1);
				DO_INTERACTIVE(1);
			}
		}

		if (PHPDBG_G(flags) & PHPDBG_IS_SIGNALED) {
			PHPDBG_G(flags) &= ~PHPDBG_IS_SIGNALED;

			phpdbg_out("\n");
			phpdbg_notice("Program received signal SIGINT");
			DO_INTERACTIVE(1);
		}

next:

		PHPDBG_G(last_line) = execute_data->opline->lineno;

		/* stupid hack to make zend_do_fcall_common_helper return ZEND_VM_ENTER() instead of recursively calling zend_execute() and eventually segfaulting */
		if ((execute_data->opline->opcode == ZEND_DO_FCALL ||
		     execute_data->opline->opcode == ZEND_DO_UCALL ||
		     execute_data->opline->opcode == ZEND_DO_FCALL_BY_NAME) &&
		     execute_data->call->func->type == ZEND_USER_FUNCTION) {
			zend_execute_ex = execute_ex;
		}
		PHPDBG_G(vmret) = zend_vm_call_opcode_handler(execute_data);
		zend_execute_ex = phpdbg_execute_ex;

		if (PHPDBG_G(vmret) != 0) {
			if (PHPDBG_G(vmret) < 0) {
				PHPDBG_G(in_execution) = original_in_execution;
				return;
			} else {
				execute_data = EG(current_execute_data);
			}
		}
	}
	zend_error_noreturn(E_ERROR, "Arrived at end of main loop which shouldn't happen");
} /* }}} */

/* only if *not* interactive and while executing */
void phpdbg_force_interruption(void) /* {{{ */ {
	zend_object *exception = EG(exception);
	zend_execute_data *data = EG(current_execute_data); /* should be always readable if not NULL */

	PHPDBG_G(flags) |= PHPDBG_IN_SIGNAL_HANDLER;

	if (data) {
		if (data->func) {
			if (ZEND_USER_CODE(data->func->type)) {
				phpdbg_notice("Current opline: %p (op #%u) in %s:%u",
				    data->opline,
				    (uint32_t) (data->opline - data->func->op_array.opcodes),
				    data->func->op_array.filename->val,
				    data->opline->lineno);
			} else if (data->func->internal_function.function_name) {
				phpdbg_notice("Current opline: in internal function %s",
				    data->func->internal_function.function_name->val);
			} else {
				phpdbg_notice("Current opline: executing internal code");
			}
		} else {
			phpdbg_notice("Current opline: %p (op_array information unavailable)",
			    data->opline);
		}
	} else {
		phpdbg_notice("No information available about executing context");
	}

	DO_INTERACTIVE(0);

next:
	PHPDBG_G(flags) &= ~PHPDBG_IN_SIGNAL_HANDLER;

	if (PHPDBG_G(flags) & PHPDBG_IS_STOPPING) {
		zend_bailout();
	}
}
/* }}} */
