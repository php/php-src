/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
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

#include <stdio.h>
#include <string.h>
#include "zend.h"
#include "zend_compile.h"
#include "zend_exceptions.h"
#include "phpdbg.h"

#include "phpdbg_help.h"
#include "phpdbg_print.h"
#include "phpdbg_info.h"
#include "phpdbg_break.h"
#include "phpdbg_opcode.h"
#include "phpdbg_list.h"
#include "phpdbg_utils.h"
#include "phpdbg_prompt.h"
#include "phpdbg_cmd.h"
#include "phpdbg_set.h"
#include "phpdbg_frame.h"
#include "phpdbg_lexer.h"
#include "phpdbg_parser.h"
#include "phpdbg_wait.h"
#include "phpdbg_eol.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);
extern int phpdbg_startup_run;

#ifdef HAVE_LIBDL
#ifdef PHP_WIN32
#include "win32/param.h"
#include "win32/winutil.h"
#define GET_DL_ERROR()  php_win_err()
#elif defined(NETWARE)
#include <sys/param.h>
#define GET_DL_ERROR()  dlerror()
#else
#include <sys/param.h>
#define GET_DL_ERROR()  DL_ERROR()
#endif
#endif

/* {{{ command declarations */
const phpdbg_command_t phpdbg_prompt_commands[] = {
	PHPDBG_COMMAND_D(exec,    "set execution context",                    'e', NULL, "s", 0),
	PHPDBG_COMMAND_D(step,    "step through execution",                   's', NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(continue,"continue execution",                       'c', NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(run,     "attempt execution",                        'r', NULL, "|s", 0),
	PHPDBG_COMMAND_D(ev,      "evaluate some code",                        0 , NULL, "i", PHPDBG_ASYNC_SAFE), /* restricted ASYNC_SAFE */
	PHPDBG_COMMAND_D(until,   "continue past the current line",           'u', NULL, 0, 0),
	PHPDBG_COMMAND_D(finish,  "continue past the end of the stack",       'F', NULL, 0, 0),
	PHPDBG_COMMAND_D(leave,   "continue until the end of the stack",      'L', NULL, 0, 0),
	PHPDBG_COMMAND_D(print,   "print something",                          'p', phpdbg_print_commands, 0, 0),
	PHPDBG_COMMAND_D(break,   "set breakpoint",                           'b', phpdbg_break_commands, "|*c", 0),
	PHPDBG_COMMAND_D(back,    "show trace",                               't', NULL, "|n", PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(frame,   "switch to a frame",                        'f', NULL, "|n", PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(list,    "lists some code",                          'l', phpdbg_list_commands, "*", PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(info,    "displays some informations",               'i', phpdbg_info_commands, "s", PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(clean,   "clean the execution environment",          'X', NULL, 0, 0),
	PHPDBG_COMMAND_D(clear,   "clear breakpoints",                        'C', NULL, 0, 0),
	PHPDBG_COMMAND_D(help,    "show help menu",                           'h', phpdbg_help_commands, "|s", PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(set,     "set phpdbg configuration",                 'S', phpdbg_set_commands,   "s", PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(register,"register a function",                      'R', NULL, "s", 0),
	PHPDBG_COMMAND_D(source,  "execute a phpdbginit",                     '<', NULL, "s", 0),
	PHPDBG_COMMAND_D(export,  "export breaks to a .phpdbginit script",    '>', NULL, "s", PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(sh,   	  "shell a command",                           0 , NULL, "i", 0),
	PHPDBG_COMMAND_D(quit,    "exit phpdbg",                              'q', NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_COMMAND_D(wait,    "wait for other process",                   'W', NULL, 0, 0),
	PHPDBG_COMMAND_D(watch,   "set watchpoint",                           'w', phpdbg_watch_commands, "|ss", 0),
	PHPDBG_COMMAND_D(eol,     "set EOL",                                  'E', NULL, "|s", 0),
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
			fci.function_table = &PHPDBG_G(registered);
			fci.symbol_table = zend_rebuild_symbol_table();
			fci.object = NULL;
			fci.retval = &fretval;
			fci.no_separation = 1;

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
							spprintf(&buffered, 0, "%s::%s#%ld", next->method.class, next->method.name, next->num);
							add_next_index_string(&params, buffered);
						break;

						case NUMERIC_FUNCTION_PARAM:
							spprintf(&buffered, 0, "%s#%ld", next->str, next->num);
							add_next_index_string(&params, buffered);
						break;

						case FILE_PARAM:
							spprintf(&buffered, 0, "%s:%ld", next->file.name, next->file.line);
							add_next_index_string(&params, buffered);
						break;

						case NUMERIC_FILE_PARAM:
							spprintf(&buffered, 0, "%s:#%ld", next->file.name, next->file.line);
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

			zval_dtor(&fci.function_name);
			efree(lc_name);

			return SUCCESS;
		}

		efree(lc_name);
	}

	return FAILURE;
} /* }}} */

struct phpdbg_init_state {
	int line;
	zend_bool in_code;
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
								phpdbg_output_err_buf("initfailure", "%b file=\"%s\" line=\"%d\" input=\"%s\"", "Unrecognized command in %s:%d: %s, %b!", state->init_file, state->line, input);
							} else {
								phpdbg_output_err_buf("initfailure", "%b line=\"%d\" input=\"%s\"", "Unrecognized command on line %d: %s, %b!", state->line, input);
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

void phpdbg_try_file_init(char *init_file, size_t init_file_len, zend_bool free_init) /* {{{ */
{
	zend_stat_t sb;

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
			phpdbg_error("initfailure", "type=\"openfile\" file=\"%s\"", "Failed to open %s for initialization", init_file);
		}

		if (free_init) {
			free(init_file);
		}
	}
} /* }}} */

void phpdbg_init(char *init_file, size_t init_file_len, zend_bool use_default) /* {{{ */
{
	if (!init_file && use_default) {
		char *scan_dir = getenv("PHP_INI_SCAN_DIR");
		int i;

		phpdbg_try_file_init(PHPDBG_STRL(PHP_CONFIG_FILE_PATH "/" PHPDBG_INIT_FILENAME), 0);

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

			asprintf(&init_file, "%s/%s", scan_dir, PHPDBG_INIT_FILENAME);
			phpdbg_try_file_init(init_file, strlen(init_file), 1);
			if (i == -1) {
				break;
			}
			scan_dir += i + 1;
		}

		phpdbg_try_file_init(PHPDBG_STRL(PHPDBG_INIT_FILENAME), 0);
	} else {
		phpdbg_try_file_init(init_file, init_file_len, 1);
	}
}

PHPDBG_COMMAND(exec) /* {{{ */
{
	zend_stat_t sb;

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
					phpdbg_notice("exec", "type=\"unset\" context=\"%s\"", "Unsetting old execution context: %s", PHPDBG_G(exec));
					efree(PHPDBG_G(exec));
					PHPDBG_G(exec) = NULL;
					PHPDBG_G(exec_len) = 0L;
				}

				if (PHPDBG_G(ops)) {
					phpdbg_notice("exec", "type=\"unsetops\"", "Destroying compiled opcodes");
					phpdbg_clean(0);
				}

				PHPDBG_G(exec) = res;
				PHPDBG_G(exec_len) = res_len;

				VCWD_CHDIR_FILE(res);

				*SG(request_info).argv = PHPDBG_G(exec);
				php_hash_environment();

				phpdbg_notice("exec", "type=\"set\" context=\"%s\"", "Set execution context: %s", PHPDBG_G(exec));

				if (PHPDBG_G(in_execution)) {
					phpdbg_clean(1);
				}

				phpdbg_compile();
			} else {
				phpdbg_notice("exec", "type=\"unchanged\"", "Execution context not changed");
			}
		} else {
			phpdbg_error("exec", "type=\"invalid\" context=\"%s\"", "Cannot use %s as execution context, not a valid file or symlink", param->str);
		}
	} else {
		phpdbg_error("exec", "type=\"notfound\" context=\"%s\"", "Cannot stat %s, ensure the file exists", param->str);
	}
	return SUCCESS;
} /* }}} */

int phpdbg_compile(void) /* {{{ */
{
	zend_file_handle fh;

	if (!PHPDBG_G(exec)) {
		phpdbg_error("inactive", "type=\"nocontext\"", "No execution context");
		return FAILURE;
	}

	if (php_stream_open_for_zend_ex(PHPDBG_G(exec), &fh, USE_PATH|STREAM_OPEN_FOR_INCLUDE) == SUCCESS) {
		PHPDBG_G(ops) = zend_compile_file(&fh, ZEND_INCLUDE);
		zend_destroy_file_handle(&fh);

		phpdbg_notice("compile", "context=\"%s\"", "Successful compilation of %s", PHPDBG_G(exec));

		return SUCCESS;
	} else {
		phpdbg_error("compile", "type=\"openfailure\" context=\"%s\"", "Could not open file %s", PHPDBG_G(exec));
	}

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

PHPDBG_COMMAND(until) /* {{{ */
{
	if (!PHPDBG_G(in_execution)) {
		phpdbg_error("inactive", "type=\"noexec\"", "Not executing");
		return SUCCESS;
	}

	PHPDBG_G(flags) |= PHPDBG_IN_UNTIL;
	{
		const zend_op *opline = EG(current_execute_data)->opline;
		const zend_op_array *op_array = &EG(current_execute_data)->func->op_array;

		while (++opline < op_array->opcodes + op_array->last) {
			if (opline->lineno != EG(current_execute_data)->opline->lineno) {
				zend_hash_index_update_ptr(&PHPDBG_G(seek), (zend_ulong) opline, (void *) opline);
				break;
			}
		}
	}

	return PHPDBG_UNTIL;
} /* }}} */

static void phpdbg_seek_to_end(void) {
	const zend_op *opline = EG(current_execute_data)->opline;
	const zend_op_array *op_array = &EG(current_execute_data)->func->op_array - 1;

	while (++opline < op_array->opcodes + op_array->last) {
		switch (opline->opcode) {
			case ZEND_RETURN:
			case ZEND_THROW:
			case ZEND_EXIT:
#ifdef ZEND_YIELD
			case ZEND_YIELD:
#endif
				zend_hash_index_update_ptr(&PHPDBG_G(seek), (zend_ulong) opline, (void *) opline);
			return;
		}
	}
}

PHPDBG_COMMAND(finish) /* {{{ */
{
	if (!PHPDBG_G(in_execution)) {
		phpdbg_error("inactive", "type=\"noexec\"", "Not executing");
		return SUCCESS;
	}

	PHPDBG_G(flags) |= PHPDBG_IN_FINISH;
	phpdbg_seek_to_end();

	return PHPDBG_FINISH;
} /* }}} */

PHPDBG_COMMAND(leave) /* {{{ */
{
	if (!PHPDBG_G(in_execution)) {
		phpdbg_error("inactive", "type=\"noexec\"", "Not executing");
		return SUCCESS;
	}

	PHPDBG_G(flags) |= PHPDBG_IN_LEAVE;
	phpdbg_seek_to_end();

	return PHPDBG_LEAVE;
} /* }}} */

PHPDBG_COMMAND(frame) /* {{{ */
{
	if (!param) {
		phpdbg_notice("frame", "id=\"%d\"", "Currently in frame #%d", PHPDBG_G(frame).num);
	} else {
		phpdbg_switch_frame(param->num);
	}

	return SUCCESS;
} /* }}} */

static inline void phpdbg_handle_exception(void) /* }}} */
{
	zend_fcall_info fci;
	zval trace;

	/* get filename and linenumber before unsetting exception */
	const char *filename = zend_get_executed_filename();
	uint32_t lineno = zend_get_executed_lineno();

	/* call __toString */
	ZVAL_STRINGL(&fci.function_name, "__tostring", sizeof("__tostring") - 1);
	fci.size = sizeof(fci);
	fci.function_table = &EG(exception)->ce->function_table;
	fci.symbol_table = NULL;
	fci.object = EG(exception);
	fci.retval = &trace;
	fci.param_count = 0;
	fci.params = NULL;
	fci.no_separation = 1;
	if (zend_call_function(&fci, NULL) == SUCCESS) {
		phpdbg_writeln("exception", "name=\"%s\" trace=\"%.*s\"", "Uncaught %s!\n%.*s", EG(exception)->ce->name->val, Z_STRLEN(trace), Z_STRVAL(trace));

		zval_ptr_dtor(&trace);
	} else {
		phpdbg_error("exception", "name=\"%s\"" "Uncaught %s!", EG(exception)->ce->name->val);
	}

	/* output useful information about address */
	phpdbg_writeln("exception", "opline=\"%p\" file=\"%s\" line=\"%u\"", "Stack entered at %p in %s on line %u", EG(current_execute_data)->func->op_array.opcodes, filename, lineno);

	zval_dtor(&fci.function_name);
	zend_clear_exception();
} /* }}} */

PHPDBG_COMMAND(run) /* {{{ */
{
	if (PHPDBG_G(ops) || PHPDBG_G(exec)) {
		zend_execute_data *ex = EG(current_execute_data);
		zend_bool restore = 1;

		if (PHPDBG_G(in_execution)) {
			if (phpdbg_ask_user_permission("Do you really want to restart execution?") == SUCCESS) {
				phpdbg_startup_run++;
				phpdbg_clean(1);
			}
			return SUCCESS;
		}

		if (!PHPDBG_G(ops)) {
			if (phpdbg_compile() == FAILURE) {
				phpdbg_error("compile", "type=\"compilefailure\" context=\"%s\"", "Failed to compile %s, cannot run", PHPDBG_G(exec));
				goto out;
			}
		}

		/* clean up from last execution */
		if (ex && ex->symbol_table) {
			zend_hash_clean(&ex->symbol_table->ht);
		} else {
			zend_rebuild_symbol_table();
		}

		/* clean seek state */
		PHPDBG_G(flags) &= ~PHPDBG_SEEK_MASK;
		zend_hash_clean(&PHPDBG_G(seek));

		/* reset hit counters */
		phpdbg_reset_breakpoints();

		if (param && param->type != EMPTY_PARAM && param->len != 0) {
			char **argv = emalloc(5 * sizeof(char *));
			int argc = 0;
			int i;
			char *argv_str = strtok(param->str, " ");

			while (argv_str) {
				if (argc >= 4 && argc == (argc & -argc)) {
					argv = erealloc(argv, (argc * 2 + 1) * sizeof(char *));
				}
				argv[++argc] = argv_str;
				argv_str = strtok(0, " ");
				argv[argc] = estrdup(argv[argc]);
			}
			argv[0] = SG(request_info).argv[0];
			for (i = SG(request_info).argc; --i;) {
				efree(SG(request_info).argv[i]);
			}
			efree(SG(request_info).argv);
			SG(request_info).argv = erealloc(argv, ++argc * sizeof(char *));
			SG(request_info).argc = argc;

			php_hash_environment();
		}

		zend_try {
			PHPDBG_G(flags) ^= PHPDBG_IS_INTERACTIVE;
			PHPDBG_G(flags) |= PHPDBG_IS_RUNNING;
			zend_execute(PHPDBG_G(ops), &PHPDBG_G(retval));
			PHPDBG_G(flags) ^= PHPDBG_IS_INTERACTIVE;
		} zend_catch {
			PHPDBG_G(in_execution) = 0;

			if (PHPDBG_G(flags) & PHPDBG_IS_QUITTING) {
				zend_bailout();
			}

			if (!(PHPDBG_G(flags) & PHPDBG_IS_STOPPING)) {
				phpdbg_error("stop", "type=\"bailout\"", "Caught exit/error from VM");
				restore = 0;
			}
		} zend_end_try();

		if (PHPDBG_G(socket_fd) != -1) {
			close(PHPDBG_G(socket_fd));
			PHPDBG_G(socket_fd) = -1;
		}

		if (restore) {
			if (EG(exception)) {
				phpdbg_handle_exception();
			}
		}

		phpdbg_clean(1);

		PHPDBG_G(flags) &= ~PHPDBG_IS_RUNNING;
	} else {
		phpdbg_error("inactive", "type=\"nocontext\"", "Nothing to execute!");
	}

out:
	PHPDBG_FRAME(num) = 0;
	return SUCCESS;
} /* }}} */

int phpdbg_output_ev_variable(char *name, size_t len, char *keyname, size_t keylen, HashTable *parent, zval *zv) {
	phpdbg_notice("eval", "variable=\"%.*s\"", "Printing variable %.*s", (int) len, name);
	phpdbg_xml("<eval %r>");
	zend_print_zval_r(zv, 0);
	phpdbg_xml("</eval>");
	phpdbg_out("\n");

	efree(name);
	efree(keyname);

	return SUCCESS;
}

PHPDBG_COMMAND(ev) /* {{{ */
{
	zend_bool stepping = ((PHPDBG_G(flags) & PHPDBG_IS_STEPPING) == PHPDBG_IS_STEPPING);
	zval retval;

	zend_execute_data *original_execute_data = EG(current_execute_data);
	zend_class_entry *original_scope = EG(scope);
	zend_vm_stack original_stack = EG(vm_stack);

	PHPDBG_OUTPUT_BACKUP();

	original_stack->top = EG(vm_stack_top);

	if (PHPDBG_G(flags) & PHPDBG_IN_SIGNAL_HANDLER) {
		phpdbg_try_access {
			phpdbg_parse_variable(param->str, param->len, &EG(symbol_table).ht, 0, phpdbg_output_ev_variable, 0);
		} phpdbg_catch_access {
			phpdbg_error("signalsegv", "", "Could not fetch data, invalid data source");
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
			phpdbg_xml("<eval %r>");
			if (PHPDBG_G(flags) & PHPDBG_WRITE_XML) {
				zval *zvp = &retval;
				phpdbg_xml_var_dump(zvp);
			}
			zend_print_zval_r(&retval, 0);
			phpdbg_xml("</eval>");
			phpdbg_out("\n");
			zval_ptr_dtor(&retval);
		}
	} zend_catch {
		EG(current_execute_data) = original_execute_data;
		EG(scope) = original_scope;
		EG(vm_stack_top) = original_stack->top;
		EG(vm_stack_end) = original_stack->end;
		EG(vm_stack) = original_stack;
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
		phpdbg_error("inactive", "type=\"noexec\"", "Not executing!");
		return SUCCESS;
	}

	if (!param) {
		phpdbg_dump_backtrace(0);
	} else {
		phpdbg_dump_backtrace(param->num);
	}

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(print) /* {{{ */
{
	phpdbg_out("Execution Context Information\n\n");
	phpdbg_xml("<printinfo %r>");
#ifdef HAVE_LIBREADLINE
	phpdbg_writeln("print", "readline=\"yes\"", "Readline   yes");
#else
	phpdbg_writeln("print", "readline=\"no\"", "Readline   no");
#endif
#ifdef HAVE_LIBEDIT
	phpdbg_writeln("print", "libedit=\"yes\"", "Libedit    yes");
#else
	phpdbg_writeln("print", "libedit=\"no\"", "Libedit    no");
#endif

	phpdbg_writeln("print", "context=\"%s\"", "Exec       %s", PHPDBG_G(exec) ? PHPDBG_G(exec) : "none");
	phpdbg_writeln("print", "compiled=\"%s\"", "Compiled   %s", PHPDBG_G(ops) ? "yes" : "no");
	phpdbg_writeln("print", "stepping=\"%s\"", "Stepping   %s", (PHPDBG_G(flags) & PHPDBG_IS_STEPPING) ? "on" : "off");
	phpdbg_writeln("print", "quiet=\"%s\"", "Quietness  %s", (PHPDBG_G(flags) & PHPDBG_IS_QUIET) ? "on" : "off");
	phpdbg_writeln("print", "oplog=\"%s\"", "Oplog      %s", PHPDBG_G(oplog) ? "on" : "off");

	if (PHPDBG_G(ops)) {
		phpdbg_writeln("print", "ops=\"%d\"", "Opcodes    %d", PHPDBG_G(ops)->last);
		phpdbg_writeln("print", "vars=\"%d\"", "Variables  %d", PHPDBG_G(ops)->last_var ? PHPDBG_G(ops)->last_var - 1 : 0);
	}

	phpdbg_writeln("print", "executing=\"%d\"", "Executing  %s", PHPDBG_G(in_execution) ? "yes" : "no");
	if (PHPDBG_G(in_execution)) {
		phpdbg_writeln("print", "vmret=\"%d\"", "VM Return  %d", PHPDBG_G(vmret));
	}

	phpdbg_writeln("print", "classes=\"%d\"", "Classes    %d", zend_hash_num_elements(EG(class_table)));
	phpdbg_writeln("print", "functions=\"%d\"", "Functions  %d", zend_hash_num_elements(EG(function_table)));
	phpdbg_writeln("print", "constants=\"%d\"", "Constants  %d", zend_hash_num_elements(EG(zend_constants)));
	phpdbg_writeln("print", "includes=\"%d\"", "Included   %d", zend_hash_num_elements(&EG(included_files)));
	phpdbg_xml("</printinfo>");

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(info) /* {{{ */
{
	phpdbg_error("info", "type=\"toofewargs\" expected=\"1\"", "No information command selected!");

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(set) /* {{{ */
{
	phpdbg_error("set", "type=\"toofewargs\" expected=\"1\"", "No set command selected!");

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(break) /* {{{ */
{
	if (!param) {
		phpdbg_set_breakpoint_file(
			zend_get_executed_filename(),
			zend_get_executed_lineno());
	} else switch (param->type) {
		case ADDR_PARAM:
			phpdbg_set_breakpoint_opline(param->addr);
			break;
		case NUMERIC_PARAM:
			if (PHPDBG_G(exec)) {
				phpdbg_set_breakpoint_file(phpdbg_current_file(), param->num);
			} else {
				phpdbg_error("inactive", "type=\"noexec\"", "Execution context not set!");
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
			phpdbg_set_breakpoint_file(param->file.name, param->file.line);
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
		phpdbg_error("sh", "type=\"failure\" smd=\"%s\"", "Failed to execute %s", param->str);
	}

	return SUCCESS;
} /* }}} */

static int add_module_info(zend_module_entry *module) {
	phpdbg_write("module", "name=\"%s\"", "%s\n", module->name);
	return 0;
}

static int add_zendext_info(zend_extension *ext) {
	phpdbg_write("extension", "name=\"%s\"", "%s\n", ext->name);
	return 0;
}

PHPDBG_API const char *phpdbg_load_module_or_extension(char **path, char **name) {
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
		phpdbg_error("dl", "type=\"relpath\"", "Not a full path given or extension_dir ini setting is not set");

		return NULL;
	}

	handle = DL_LOAD(*path);

	if (!handle) {
#if PHP_WIN32
		char *err = GET_DL_ERROR();
		if (err && *err != "") {
			phpdbg_error("dl", "type=\"unknown\"", "%s", err);
			LocalFree(err);
		} else {
			phpdbg_error("dl", "type=\"unknown\"", "Unknown reason");
		}
#else
		phpdbg_error("dl", "type=\"unknown\"", "%s", GET_DL_ERROR());
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
			phpdbg_error("dl", "type=\"wrongapi\" extension=\"%s\" apineeded=\"%d\" apiinstalled=\"%d\"", "%s requires Zend Engine API version %d, which does not match the installed Zend Engine API version %d", new_extension->name, extension_version_info->zend_extension_api_no, ZEND_EXTENSION_API_NO);

			goto quit;
		} else if (strcmp(ZEND_EXTENSION_BUILD_ID, extension_version_info->build_id) && (!new_extension->build_id_check || new_extension->build_id_check(ZEND_EXTENSION_BUILD_ID) != SUCCESS)) {
			phpdbg_error("dl", "type=\"wrongbuild\" extension=\"%s\" buildneeded=\"%s\" buildinstalled=\"%s\"", "%s was built with configuration %s, whereas running engine is %s", new_extension->name, extension_version_info->build_id, ZEND_EXTENSION_BUILD_ID);

			goto quit;
		}

		*name = new_extension->name;

		zend_register_extension(new_extension, handle);

		if (new_extension->startup) {
			if (new_extension->startup(new_extension) != SUCCESS) {
				phpdbg_error("dl", "type=\"startupfailure\" extension=\"%s\"", "Unable to startup Zend extension %s", new_extension->name);

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
		*name = (char *) module_entry->name;

		if (strcmp(ZEND_EXTENSION_BUILD_ID, module_entry->build_id)) {
			phpdbg_error("dl", "type=\"wrongbuild\" module=\"%s\" buildneeded=\"%s\" buildinstalled=\"%s\"",  "%s was built with configuration %s, whereas running engine is %s", module_entry->name, module_entry->build_id, ZEND_EXTENSION_BUILD_ID);

			goto quit;
		}

		module_entry->type = MODULE_PERSISTENT;
		module_entry->module_number = zend_next_free_module();
		module_entry->handle = handle;

		if ((module_entry = zend_register_module_ex(module_entry)) == NULL) {
			phpdbg_error("dl", "type=\"registerfailure\" module=\"%s\"", "Unable to register module %s", module_entry->name);

			goto quit;
		}

		if (zend_startup_module_ex(module_entry) == FAILURE) {
			phpdbg_error("dl", "type=\"startupfailure\" module=\"%s\"", "Unable to startup module %s", module_entry->name);

			goto quit;
		}

		if (module_entry->request_startup_func) {
			if (module_entry->request_startup_func(MODULE_PERSISTENT, module_entry->module_number) == FAILURE) {
				phpdbg_error("dl", "type=\"initfailure\" module=\"%s\"", "Unable to initialize module %s", module_entry->name);

				goto quit;
			}
		}

		return "module";
	} while (0);

	phpdbg_error("dl", "type=\"nophpso\"", "This shared object is nor a Zend extension nor a module");

quit:
	DL_UNLOAD(handle);
	return NULL;
}

PHPDBG_COMMAND(dl) /* {{{ */
{
	const char *type;
	char *name, *path;

	if (!param || param->type == EMPTY_PARAM) {
		phpdbg_notice("dl", "extensiontype=\"Zend extension\"", "Zend extensions");
		zend_llist_apply(&zend_extensions, (llist_apply_func_t) add_zendext_info);
		phpdbg_out("\n");
		phpdbg_notice("dl", "extensiontype=\"module\"", "Modules");
		zend_hash_apply(&module_registry, (apply_func_t) add_module_info);
	} else switch (param->type) {
		case STR_PARAM:
#ifdef HAVE_LIBDL
			path = estrndup(param->str, param->len);

			phpdbg_activate_err_buf(1);
			if ((type = phpdbg_load_module_or_extension(&path, &name)) == NULL) {
				phpdbg_error("dl", "path=\"%s\" %b", "Could not load %s, not found or invalid zend extension / module: %b", path);
				efree(name);
			} else {
				phpdbg_notice("dl", "extensiontype=\"%s\" name=\"%s\" path=\"%s\"", "Successfully loaded the %s %s at path %s", type, name, path);
			}
			phpdbg_activate_err_buf(0);
			phpdbg_free_err_buf();
			efree(path);
#else
			phpdbg_error("dl", "type=\"unsupported\" path=\"%.*s\"", "Cannot dynamically load %.*s - dynamic modules are not supported", (int) param->len, param->str);
#endif
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(source) /* {{{ */
{
	zend_stat_t sb;

	if (VCWD_STAT(param->str, &sb) != -1) {
		phpdbg_try_file_init(param->str, param->len, 0);
	} else {
		phpdbg_error("source", "type=\"notfound\" file=\"%s\"", "Failed to stat %s, file does not exist", param->str);
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
		phpdbg_error("export", "type=\"openfailure\" file=\"%s\"", "Failed to open or create %s, check path and permissions", param->str);
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

			phpdbg_notice("register", "function=\"%s\"", "Registered %s", lcname);
		} else {
			phpdbg_error("register", "type=\"notfound\" function=\"%s\"", "The requested function (%s) could not be found", param->str);
		}
	} else {
		phpdbg_error("register", "type=\"inuse\" function=\"%s\"", "The requested name (%s) is already in use", lcname);
	}

	efree(lcname);
	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(quit) /* {{{ */
{
	/* don't allow this to loop, ever ... */
	if (!(PHPDBG_G(flags) & PHPDBG_IS_STOPPING)) {
		PHPDBG_G(flags) |= PHPDBG_IS_QUITTING;
		PHPDBG_G(flags) &= ~(PHPDBG_IS_RUNNING | PHPDBG_IS_CLEANING);
		zend_bailout();
	}

	return PHPDBG_NEXT;
} /* }}} */

PHPDBG_COMMAND(clean) /* {{{ */
{
	if (PHPDBG_G(in_execution)) {
		if (phpdbg_ask_user_permission("Do you really want to clean your current environment?") == FAILURE) {
			return SUCCESS;
		}
	}

	phpdbg_out("Cleaning Execution Environment\n");
	phpdbg_xml("<cleaninfo %r>");

	phpdbg_writeln("clean", "classes=\"%d\"", "Classes    %d", zend_hash_num_elements(EG(class_table)));
	phpdbg_writeln("clean", "functions=\"%d\"", "Functions  %d", zend_hash_num_elements(EG(function_table)));
	phpdbg_writeln("clean", "constants=\"%d\"", "Constants  %d", zend_hash_num_elements(EG(zend_constants)));
	phpdbg_writeln("clean", "includes=\"%d\"", "Includes   %d", zend_hash_num_elements(&EG(included_files)));

	PHPDBG_G(flags) &= ~PHPDBG_IS_RUNNING;

	phpdbg_clean(1);

	phpdbg_xml("</cleaninfo>");

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(clear) /* {{{ */
{
	phpdbg_out("Clearing Breakpoints\n");
	phpdbg_xml("<clearinfo %r>");

	phpdbg_writeln("clear", "files=\"%d\"", "File              %d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE]));
	phpdbg_writeln("clear", "functions=\"%d\"", "Functions         %d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM]));
	phpdbg_writeln("clear", "methods=\"%d\"", "Methods           %d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD]));
	phpdbg_writeln("clear", "oplines=\"%d\"", "Oplines           %d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]));
	phpdbg_writeln("clear", "fileoplines=\"%d\"", "File oplines      %d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_OPLINE]));
	phpdbg_writeln("clear", "functionoplines=\"%d\"", "Function oplines  %d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_FUNCTION_OPLINE]));
	phpdbg_writeln("clear", "methodoplines=\"%d\"", "Method oplines    %d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD_OPLINE]));
	phpdbg_writeln("clear", "eval=\"%d\"", "Conditionals      %d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_COND]));

	phpdbg_clear_breakpoints();

	phpdbg_xml("</clearinfo>");

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
			if (phpdbg_create_var_watchpoint(param->str, param->len) != FAILURE) {
				phpdbg_notice("watch", "variable=\"%.*s\"", "Set watchpoint on %.*s", (int) param->len, param->str);
			}
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

int phpdbg_interactive(zend_bool allow_async_unsafe) /* {{{ */
{
	int ret = SUCCESS;
	char *input = NULL;
	phpdbg_param_t stack;

	PHPDBG_G(flags) |= PHPDBG_IS_INTERACTIVE;

	while (ret == SUCCESS || ret == FAILURE) {
		if ((PHPDBG_G(flags) & (PHPDBG_IS_STOPPING | PHPDBG_IS_RUNNING)) == PHPDBG_IS_STOPPING) {
			zend_bailout();
		}

		if (!(input = phpdbg_read_input(NULL))) {
			break;
		}


		phpdbg_init_param(&stack, STACK_PARAM);

		if (phpdbg_do_parse(&stack, input) <= 0) {
			phpdbg_activate_err_buf(1);

#ifdef PHP_WIN32
#define PARA ((phpdbg_param_t *)stack.next)->type
			if (PHPDBG_G(flags) & PHPDBG_IS_REMOTE && (RUN_PARAM == PARA || EVAL_PARAM == PARA)) {
				sigio_watcher_start();
			}
#endif
			switch (ret = phpdbg_stack_execute(&stack, allow_async_unsafe)) {
				case FAILURE:
					if (!(PHPDBG_G(flags) & PHPDBG_IS_STOPPING)) {
						if (!allow_async_unsafe || phpdbg_call_register(&stack) == FAILURE) {
							phpdbg_output_err_buf(NULL, "%b", "%b");
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
						phpdbg_error("command", "type=\"noexec\"", "Not running");
					}
					break;
				}
			}

			phpdbg_activate_err_buf(0);
			phpdbg_free_err_buf();
#ifdef PHP_WIN32
			if (PHPDBG_G(flags) & PHPDBG_IS_REMOTE && (RUN_PARAM == PARA || EVAL_PARAM == PARA)) {
				sigio_watcher_stop();
			}
#undef PARA
#endif
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

void phpdbg_clean(zend_bool full) /* {{{ */
{
	/* this is implicitly required */
	if (PHPDBG_G(ops)) {
		destroy_op_array(PHPDBG_G(ops));
		efree(PHPDBG_G(ops));
		PHPDBG_G(ops) = NULL;
	}

	if (full) {
		PHPDBG_G(flags) |= PHPDBG_IS_CLEANING;

		zend_bailout();
	}
} /* }}} */

#define DO_INTERACTIVE(allow_async_unsafe) do { \
	if (!(PHPDBG_G(flags) & PHPDBG_IN_EVAL)) { \
		const char *file_char = zend_get_executed_filename(); \
		zend_string *file = zend_string_init(file_char, strlen(file_char), 0); \
		phpdbg_list_file(file, 3, zend_get_executed_lineno()-1, zend_get_executed_lineno()); \
		efree(file); \
	} \
	\
	switch (phpdbg_interactive(allow_async_unsafe)) { \
		case PHPDBG_LEAVE: \
		case PHPDBG_FINISH: \
		case PHPDBG_UNTIL: \
		case PHPDBG_NEXT:{ \
			goto next; \
		} \
	} \
} while (0)

void phpdbg_execute_ex(zend_execute_data *execute_data) /* {{{ */
{
	zend_bool original_in_execution = PHPDBG_G(in_execution);
	HashTable vars;

	zend_hash_init(&vars, execute_data->func->op_array.last, NULL, NULL, 0);

	if ((PHPDBG_G(flags) & PHPDBG_IS_STOPPING) && !(PHPDBG_G(flags) & PHPDBG_IS_RUNNING)) {
		zend_bailout();
	}

	PHPDBG_G(in_execution) = 1;

	while (1) {
		if ((PHPDBG_G(flags) & PHPDBG_BP_RESOLVE_MASK)) {
			/* resolve nth opline breakpoints */
			phpdbg_resolve_op_array_breaks(&execute_data->func->op_array);
		}

#ifdef ZEND_WIN32
		if (EG(timed_out)) {
			zend_timeout(0);
		}
#endif

		/* allow conditional breakpoints and
			initialization to access the vm uninterrupted */
		if ((PHPDBG_G(flags) & PHPDBG_IN_COND_BP) ||
			(PHPDBG_G(flags) & PHPDBG_IS_INITIALIZING)) {
			/* skip possible breakpoints */
			goto next;
		}

		/* perform seek operation */
		if (PHPDBG_G(flags) & PHPDBG_SEEK_MASK) {
			/* current address */
			zend_ulong address = (zend_ulong) execute_data->opline;

			/* run to next line */
			if (PHPDBG_G(flags) & PHPDBG_IN_UNTIL) {
				if (zend_hash_index_exists(&PHPDBG_G(seek), address)) {
					PHPDBG_G(flags) &= ~PHPDBG_IN_UNTIL;
					zend_hash_clean(&PHPDBG_G(seek));
				} else {
					/* skip possible breakpoints */
					goto next;
				}
			}

			/* run to finish */
			if (PHPDBG_G(flags) & PHPDBG_IN_FINISH) {
				if (zend_hash_index_exists(&PHPDBG_G(seek), address)) {
					PHPDBG_G(flags) &= ~PHPDBG_IN_FINISH;
					zend_hash_clean(&PHPDBG_G(seek));
				}
				/* skip possible breakpoints */
				goto next;
			}

			/* break for leave */
			if (PHPDBG_G(flags) & PHPDBG_IN_LEAVE) {
				if (zend_hash_index_exists(&PHPDBG_G(seek), address)) {
					PHPDBG_G(flags) &= ~PHPDBG_IN_LEAVE;
					zend_hash_clean(&PHPDBG_G(seek));
					phpdbg_notice("breakpoint", "id=\"leave\" file=\"%s\" line=\"%u\"", "Breaking for leave at %s:%u",
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

		/* not while in conditionals */
		phpdbg_print_opline_ex(execute_data, &vars, 0);

		if (PHPDBG_G(flags) & PHPDBG_IS_STEPPING && (PHPDBG_G(flags) & PHPDBG_STEP_OPCODE || execute_data->opline->lineno != PHPDBG_G(last_line))) {
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
			phpdbg_notice("signal", "type=\"SIGINT\"", "Program received signal SIGINT");
			DO_INTERACTIVE(1);
		}

next:

		PHPDBG_G(last_line) = execute_data->opline->lineno;

		/* stupid hack to make zend_do_fcall_common_helper return ZEND_VM_ENTER() instead of recursively calling zend_execute() and eventually segfaulting */
		if (execute_data->opline->opcode == ZEND_DO_FCALL && execute_data->func->type == ZEND_USER_FUNCTION) {
			zend_execute_ex = execute_ex;
		}
		PHPDBG_G(vmret) = execute_data->opline->handler(execute_data);
		zend_execute_ex = phpdbg_execute_ex;

		if (PHPDBG_G(vmret) != 0) {
			if (PHPDBG_G(vmret) < 0) {
				zend_hash_destroy(&vars);
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
void phpdbg_force_interruption(void) {
	zend_execute_data *data = EG(current_execute_data); /* should be always readable if not NULL */

	PHPDBG_G(flags) |= PHPDBG_IN_SIGNAL_HANDLER;

	if (data) {
		if (data->func) {
			phpdbg_notice("hardinterrupt", "opline=\"%p\" num=\"%lu\" file=\"%s\" line=\"%u\"", "Current opline: %p (op #%lu) in %s:%u", data->opline, (data->opline - data->func->op_array.opcodes) / sizeof(data->opline), data->func->op_array.filename, data->opline->lineno);
		} else {
			phpdbg_notice("hardinterrupt", "opline=\"%p\"", "Current opline: %p (op_array information unavailable)", data->opline);
		}
	} else {
		phpdbg_notice("hardinterrupt", "", "No information available about executing context");
	}

	DO_INTERACTIVE(0);

next:
	PHPDBG_G(flags) &= ~PHPDBG_IN_SIGNAL_HANDLER;

	if (PHPDBG_G(flags) & PHPDBG_IS_STOPPING) {
		zend_bailout();
	}
}

PHPDBG_COMMAND(eol) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		phpdbg_notice("eol", "argument required", "argument required");
	} else switch (param->type) {
		case STR_PARAM:
			if (FAILURE == phpdbg_eol_global_update(param->str)) {
				phpdbg_notice("eol", "unknown EOL name '%s', give crlf, lf, cr", "unknown EOL name '%s', give  crlf, lf, cr", param->str);
			}
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

