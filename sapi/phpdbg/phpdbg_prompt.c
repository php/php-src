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
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include <stdio.h>
#include <string.h>
#include "zend.h"
#include "zend_compile.h"
#include "phpdbg.h"
#include "phpdbg_help.h"
#include "phpdbg_print.h"
#include "phpdbg_info.h"
#include "phpdbg_break.h"
#include "phpdbg_bp.h"
#include "phpdbg_opcode.h"
#include "phpdbg_list.h"
#include "phpdbg_utils.h"
#include "phpdbg_prompt.h"
#include "phpdbg_cmd.h"
#include "phpdbg_set.h"
#include "phpdbg_frame.h"
#include "phpdbg_lexer.h"
#include "phpdbg_parser.h"

/* {{{ command declarations */
const phpdbg_command_t phpdbg_prompt_commands[] = {
	PHPDBG_COMMAND_D(exec,    "set execution context",                    'e', NULL, "s"),
	PHPDBG_COMMAND_D(step,    "step through execution",                   's', NULL, 0),
	PHPDBG_COMMAND_D(continue,"continue execution",                       'c', NULL, 0),
	PHPDBG_COMMAND_D(run,     "attempt execution",                        'r', NULL, "|s"),
	PHPDBG_COMMAND_D(ev,      "evaluate some code",                        0, NULL, "i"),
	PHPDBG_COMMAND_D(until,   "continue past the current line",           'u', NULL, 0),
	PHPDBG_COMMAND_D(finish,  "continue past the end of the stack",       'F', NULL, 0),
	PHPDBG_COMMAND_D(leave,   "continue until the end of the stack",      'L', NULL, 0),
	PHPDBG_COMMAND_D(print,   "print something",                          'p', phpdbg_print_commands, 0),
	PHPDBG_COMMAND_D(break,   "set breakpoint",                           'b', phpdbg_break_commands, "|*c"),
	PHPDBG_COMMAND_D(back,    "show trace",                               't', NULL, "|n"),
	PHPDBG_COMMAND_D(frame,   "switch to a frame",                        'f', NULL, "|n"),
	PHPDBG_COMMAND_D(list,    "lists some code",                          'l', phpdbg_list_commands, "*"),
	PHPDBG_COMMAND_D(info,    "displays some informations",               'i', phpdbg_info_commands, "s"),
	PHPDBG_COMMAND_D(clean,   "clean the execution environment",          'X', NULL, 0),
	PHPDBG_COMMAND_D(clear,   "clear breakpoints",                        'C', NULL, 0),
	PHPDBG_COMMAND_D(help,    "show help menu",                           'h', phpdbg_help_commands, "|s"),
	PHPDBG_COMMAND_D(set,     "set phpdbg configuration",                 'S', phpdbg_set_commands,   "s"),
	PHPDBG_COMMAND_D(register,"register a function",                      'R', NULL, "s"),
	PHPDBG_COMMAND_D(source,  "execute a phpdbginit",                     '<', NULL, "s"),
	PHPDBG_COMMAND_D(export,  "export breaks to a .phpdbginit script",    '>', NULL, "s"),
	PHPDBG_COMMAND_D(sh,   	  "shell a command",                           0, NULL, "i"),
	PHPDBG_COMMAND_D(quit,    "exit phpdbg",                              'q', NULL, 0),
	PHPDBG_COMMAND_D(watch,   "set watchpoint",                           'w', phpdbg_watch_commands, "|ss"),
	PHPDBG_END_COMMAND
}; /* }}} */

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

static inline int phpdbg_call_register(phpdbg_param_t *stack TSRMLS_DC) /* {{{ */
{
	phpdbg_param_t *name = NULL;

	if (stack->type == STACK_PARAM) {
		name = stack->next;
		
		if (!name || name->type != STR_PARAM) {
			return FAILURE;
		}
		
		if (zend_hash_exists(
				&PHPDBG_G(registered), name->str, name->len+1)) {

			zval fname, *fretval;
			zend_fcall_info fci;

			ZVAL_STRINGL(&fname, name->str, name->len, 1);

			memset(&fci, 0, sizeof(zend_fcall_info));

			fci.size = sizeof(zend_fcall_info);
			fci.function_table = &PHPDBG_G(registered);
			fci.function_name = &fname;
			fci.symbol_table = EG(active_symbol_table);
			fci.object_ptr = NULL;
			fci.retval_ptr_ptr = &fretval;
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
							add_next_index_stringl(
								&params,
								next->str,
								next->len, 1);
						break;
						
						case NUMERIC_PARAM:
							add_next_index_long(&params, next->num);
						break;
						
						case METHOD_PARAM:
							spprintf(&buffered, 0, "%s::%s",
								next->method.class, next->method.name);
							add_next_index_string(&params, buffered, 0);
						break;
						
						case NUMERIC_METHOD_PARAM:
							spprintf(&buffered, 0, "%s::%s#%ld",
								next->method.class, next->method.name, next->num);
							add_next_index_string(&params, buffered, 0);
						break;
						
						case NUMERIC_FUNCTION_PARAM:
							spprintf(&buffered, 0, "%s#%ld",
								next->str, next->num);
							add_next_index_string(&params, buffered, 0);
						break;
							
						case FILE_PARAM:
							spprintf(&buffered, 0, "%s:%ld", 
								next->file.name, next->file.line);
							add_next_index_string(&params, buffered, 0);
						break;
						
						case NUMERIC_FILE_PARAM:
							spprintf(&buffered, 0, "%s:#%ld", 
								next->file.name, next->file.line);
							add_next_index_string(&params, buffered, 0);
						break;
						
						default: {
							/* not yet */
						}
					}
					
					next = next->next;	
				}

				zend_fcall_info_args(&fci, &params TSRMLS_CC);
			} else {
				fci.params = NULL;
				fci.param_count = 0;
			}

			phpdbg_debug(
				"created %d params from arguments",
				fci.param_count);

			zend_call_function(&fci, NULL TSRMLS_CC);

			if (fretval) {
				zend_print_zval_r(
					fretval, 0 TSRMLS_CC);
				phpdbg_writeln(EMPTY);
			}

			zval_dtor(&fname);

			return SUCCESS;
		}	
	}

	return FAILURE;
} /* }}} */

void phpdbg_try_file_init(char *init_file, size_t init_file_len, zend_bool free_init TSRMLS_DC) /* {{{ */
{
	struct stat sb;
	
	if (init_file && VCWD_STAT(init_file, &sb) != -1) {
		FILE *fp = fopen(init_file, "r");
		if (fp) {
			int line = 1;

			char cmd[PHPDBG_MAX_CMD];
			size_t cmd_len = 0L;
			char *code = NULL;
			size_t code_len = 0L;
			zend_bool in_code = 0;

			while (fgets(cmd, PHPDBG_MAX_CMD, fp) != NULL) {
				cmd_len = strlen(cmd)-1;

				while (cmd_len > 0L && isspace(cmd[cmd_len-1]))
					cmd_len--;

				cmd[cmd_len] = '\0';

				if (*cmd && cmd_len > 0L && cmd[0] != '#') {
					if (cmd_len == 2) {
						if (memcmp(cmd, "<:", sizeof("<:")-1) == SUCCESS) {
							in_code = 1;
							goto next_line;
						} else {
							if (memcmp(cmd, ":>", sizeof(":>")-1) == SUCCESS) {
								in_code = 0;
								code[code_len] = '\0';
								{
									zend_eval_stringl(
										code, code_len, NULL, "phpdbginit code" TSRMLS_CC);
								}
								free(code);
								code = NULL;
								goto next_line;
							}
						}
					}

					if (in_code) {
						if (code == NULL) {
							code = malloc(cmd_len + 1);
						} else code = realloc(code, code_len + cmd_len + 1);

						if (code) {
							memcpy(
								&code[code_len], cmd, cmd_len);
							code_len += cmd_len;
						}
						goto next_line;
					}

					{
						char *why = NULL;
						char *input = phpdbg_read_input(cmd TSRMLS_CC);
						phpdbg_param_t stack;

						phpdbg_init_param(&stack, STACK_PARAM);

						if (phpdbg_do_parse(&stack, input TSRMLS_CC) <= 0) {
							switch (phpdbg_stack_execute(&stack, &why TSRMLS_CC)) {
								case FAILURE:
//									if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
										if (phpdbg_call_register(&stack TSRMLS_CC) == FAILURE) {
											phpdbg_error(
												"Unrecognized command in %s:%d: %s, %s!", 
												init_file, line, input, why);
										}
//									}
								break;
							}
						}

						if (why) {
							free(why);
							why = NULL;
						}

						phpdbg_stack_free(&stack);
						phpdbg_destroy_input(&input TSRMLS_CC);
					}
				}
next_line:
				line++;
			}

			if (code) {
				free(code);
			}

			fclose(fp);
		} else {
			phpdbg_error(
				"Failed to open %s for initialization", init_file);
		}

		if (free_init) {
			free(init_file);
		}
	}
} /* }}} */

void phpdbg_init(char *init_file, size_t init_file_len, zend_bool use_default TSRMLS_DC) /* {{{ */
{
	if (!init_file && use_default) {
		char *scan_dir = getenv("PHP_INI_SCAN_DIR");
		int i;

		phpdbg_try_file_init(PHPDBG_STRL(PHP_CONFIG_FILE_PATH "/" PHPDBG_INIT_FILENAME), 0 TSRMLS_CC);

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

			asprintf(
				&init_file, "%s/%s", scan_dir, PHPDBG_INIT_FILENAME);
			phpdbg_try_file_init(init_file, strlen(init_file), 1 TSRMLS_CC);
			if (i == -1) {
				break;
			}
			scan_dir += i + 1;
		}

		phpdbg_try_file_init(PHPDBG_STRL(PHPDBG_INIT_FILENAME), 0 TSRMLS_CC);
	} else {
		phpdbg_try_file_init(init_file, init_file_len, 1 TSRMLS_CC);
	}
}

PHPDBG_COMMAND(exec) /* {{{ */
{
	struct stat sb;

	if (VCWD_STAT(param->str, &sb) != FAILURE) {
		if (sb.st_mode & (S_IFREG|S_IFLNK)) {
			char *res = phpdbg_resolve_path(param->str TSRMLS_CC);
			size_t res_len = strlen(res);

			if ((res_len != PHPDBG_G(exec_len)) || (memcmp(res, PHPDBG_G(exec), res_len) != SUCCESS)) {

				if (PHPDBG_G(exec)) {
					phpdbg_notice("Unsetting old execution context: %s", PHPDBG_G(exec));
					efree(PHPDBG_G(exec));
					PHPDBG_G(exec) = NULL;
					PHPDBG_G(exec_len) = 0L;
				}

				if (PHPDBG_G(ops)) {
					phpdbg_notice("Destroying compiled opcodes");
					phpdbg_clean(0 TSRMLS_CC);
				}

				PHPDBG_G(exec) = res;
				PHPDBG_G(exec_len) = res_len;
				
				*SG(request_info).argv = PHPDBG_G(exec);
				php_hash_environment(TSRMLS_C);

				phpdbg_notice("Set execution context: %s", PHPDBG_G(exec));

				if (phpdbg_compile(TSRMLS_C) == FAILURE) {
					phpdbg_error("Failed to compile %s", PHPDBG_G(exec));
				}
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

int phpdbg_compile(TSRMLS_D) /* {{{ */
{
	zend_file_handle fh;

	if (!PHPDBG_G(exec)) {
		phpdbg_error("No execution context");
		return SUCCESS;
	}

	if (EG(in_execution)) {
		phpdbg_error("Cannot compile while in execution");
		return FAILURE;
	}

	phpdbg_notice("Attempting compilation of %s", PHPDBG_G(exec));

	if (php_stream_open_for_zend_ex(PHPDBG_G(exec), &fh,
		USE_PATH|STREAM_OPEN_FOR_INCLUDE TSRMLS_CC) == SUCCESS) {

		PHPDBG_G(ops) = zend_compile_file(&fh, ZEND_INCLUDE TSRMLS_CC);
		zend_destroy_file_handle(&fh TSRMLS_CC);

		phpdbg_notice("Success");
		return SUCCESS;
	} else {
		phpdbg_error("Could not open file %s", PHPDBG_G(exec));
	}

	return FAILURE;
} /* }}} */

PHPDBG_COMMAND(step) /* {{{ */
{
	if (EG(in_execution)) {
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
	if (!EG(in_execution)) {
		phpdbg_error("Not executing");
		return SUCCESS;
	}

	PHPDBG_G(flags) |= PHPDBG_IN_UNTIL;
	{
		zend_uint next = 0,
				  self = (EG(current_execute_data)->opline - EG(active_op_array)->opcodes);
		zend_op  *opline = &EG(active_op_array)->opcodes[self];

		for (next = self; next < EG(active_op_array)->last; next++) {
			if (EG(active_op_array)->opcodes[next].lineno != opline->lineno) {
				zend_hash_index_update(
					&PHPDBG_G(seek),
					(zend_ulong) &EG(active_op_array)->opcodes[next],
					&EG(active_op_array)->opcodes[next],
					sizeof(zend_op), NULL);
				break;
			}
		}
	}

	return PHPDBG_UNTIL;
} /* }}} */

PHPDBG_COMMAND(finish) /* {{{ */
{
	if (!EG(in_execution)) {
		phpdbg_error("Not executing");
		return SUCCESS;
	}

	PHPDBG_G(flags) |= PHPDBG_IN_FINISH;
	{
		zend_uint next = 0,
				  self = (EG(current_execute_data)->opline - EG(active_op_array)->opcodes);

		for (next = self; next < EG(active_op_array)->last; next++) {
			switch (EG(active_op_array)->opcodes[next].opcode) {
				case ZEND_RETURN:
				case ZEND_THROW:
				case ZEND_EXIT:
#ifdef ZEND_YIELD
				case ZEND_YIELD:
#endif
					zend_hash_index_update(
						&PHPDBG_G(seek),
						(zend_ulong) &EG(active_op_array)->opcodes[next],
						&EG(active_op_array)->opcodes[next],
						sizeof(zend_op), NULL);
				break;
			}
		}
	}

	return PHPDBG_FINISH;
} /* }}} */

PHPDBG_COMMAND(leave) /* {{{ */
{
	if (!EG(in_execution)) {
		phpdbg_error("Not executing");
		return SUCCESS;
	}

	PHPDBG_G(flags) |= PHPDBG_IN_LEAVE;
	{
		zend_uint next = 0,
				  self = (EG(current_execute_data)->opline - EG(active_op_array)->opcodes);

		for (next = self; next < EG(active_op_array)->last; next++) {
			switch (EG(active_op_array)->opcodes[next].opcode) {
				case ZEND_RETURN:
				case ZEND_THROW:
				case ZEND_EXIT:
#ifdef ZEND_YIELD
				case ZEND_YIELD:
#endif
					zend_hash_index_update(
						&PHPDBG_G(seek),
						(zend_ulong) &EG(active_op_array)->opcodes[next],
						&EG(active_op_array)->opcodes[next],
						sizeof(zend_op), NULL);
				break;
			}
		}
	}

	return PHPDBG_LEAVE;
} /* }}} */

PHPDBG_COMMAND(frame) /* {{{ */
{
	if (!param) {
		phpdbg_notice("Currently in frame #%d", PHPDBG_G(frame).num);
	} else phpdbg_switch_frame(param->num TSRMLS_CC);

	return SUCCESS;
} /* }}} */

static inline void phpdbg_handle_exception(TSRMLS_D) /* }}} */
{
	zend_fcall_info fci;

	zval fname,
		 *trace,
		 exception;

	/* get filename and linenumber before unsetting exception */
	const char *filename = zend_get_executed_filename(TSRMLS_C);
	zend_uint lineno = zend_get_executed_lineno(TSRMLS_C);

	/* copy exception */
	exception = *EG(exception);
	zval_copy_ctor(&exception);
	EG(exception) = NULL;

	phpdbg_error(
		"Uncaught %s!",
		Z_OBJCE(exception)->name);

	/* call __toString */
	ZVAL_STRINGL(&fname, "__tostring", sizeof("__tostring")-1, 1);
	fci.size = sizeof(fci);
	fci.function_table = &Z_OBJCE(exception)->function_table;
	fci.function_name = &fname;
	fci.symbol_table = NULL;
	fci.object_ptr = &exception;
	fci.retval_ptr_ptr = &trace;
	fci.param_count = 0;
	fci.params = NULL;
	fci.no_separation = 1;
	zend_call_function(&fci, NULL TSRMLS_CC);

	if (trace) {
		phpdbg_writeln(
			"Uncaught %s", Z_STRVAL_P(trace));
		/* remember to dtor trace */
		zval_ptr_dtor(&trace);
	}

	/* output useful information about address */
	phpdbg_writeln(
		"Stacked entered at %p in %s on line %u",
		EG(active_op_array)->opcodes, filename, lineno);

	zval_dtor(&fname);
	zval_dtor(&exception);
} /* }}} */

PHPDBG_COMMAND(run) /* {{{ */
{
	if (EG(in_execution)) {
		phpdbg_error("Cannot start another execution while one is in progress");
		return SUCCESS;
	}

	if (PHPDBG_G(ops) || PHPDBG_G(exec)) {
		zend_op **orig_opline = EG(opline_ptr);
		zend_op_array *orig_op_array = EG(active_op_array);
		zval **orig_retval_ptr = EG(return_value_ptr_ptr);
		zend_bool restore = 1;
		zend_execute_data *ex = EG(current_execute_data);
		
		if (!PHPDBG_G(ops)) {
			if (phpdbg_compile(TSRMLS_C) == FAILURE) {
				phpdbg_error("Failed to compile %s, cannot run", PHPDBG_G(exec));
				goto out;
			}
		}

		EG(active_op_array) = PHPDBG_G(ops);
		EG(return_value_ptr_ptr) = &PHPDBG_G(retval);
		if (!EG(active_symbol_table)) {
			zend_rebuild_symbol_table(TSRMLS_C);
		}

		/* clean up from last execution */
		if (ex && ex->symbol_table) {
			zend_hash_clean(ex->symbol_table);
		}

		/* clean seek state */
		PHPDBG_G(flags) &= ~PHPDBG_SEEK_MASK;
		zend_hash_clean(
			&PHPDBG_G(seek));

		/* reset hit counters */
		phpdbg_reset_breakpoints(TSRMLS_C);

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
			
			php_hash_environment(TSRMLS_C);
		}

		zend_try {
			php_output_activate(TSRMLS_C);
			PHPDBG_G(flags) ^= PHPDBG_IS_INTERACTIVE;
			zend_execute(EG(active_op_array) TSRMLS_CC);
			PHPDBG_G(flags) ^= PHPDBG_IS_INTERACTIVE;
			php_output_deactivate(TSRMLS_C);
		} zend_catch {
			EG(active_op_array) = orig_op_array;
			EG(opline_ptr) = orig_opline;
			EG(return_value_ptr_ptr) = orig_retval_ptr;

			if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
				phpdbg_error("Caught exit/error from VM");
				restore = 0;
			}
		} zend_end_try();

		if (restore) {
			if (EG(exception)) {
				phpdbg_handle_exception(TSRMLS_C);
			}

			EG(active_op_array) = orig_op_array;
			EG(opline_ptr) = orig_opline;
			EG(return_value_ptr_ptr) = orig_retval_ptr;
		}
	} else {
		phpdbg_error("Nothing to execute!");
	}

out:
	PHPDBG_FRAME(num) = 0;
	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(ev) /* {{{ */
{
	zend_bool stepping = ((PHPDBG_G(flags) & PHPDBG_IS_STEPPING)==PHPDBG_IS_STEPPING);
	zval retval;

	if (!(PHPDBG_G(flags) & PHPDBG_IS_STEPONEVAL)) {
		PHPDBG_G(flags) &= ~ PHPDBG_IS_STEPPING;
	}

	/* disable stepping while eval() in progress */
	PHPDBG_G(flags) |= PHPDBG_IN_EVAL;
	zend_try {
		if (zend_eval_stringl(param->str, param->len,
			&retval, "eval()'d code" TSRMLS_CC) == SUCCESS) {
			zend_print_zval_r(
				&retval, 0 TSRMLS_CC);
			phpdbg_writeln(EMPTY);
			zval_dtor(&retval);
		}
	} zend_end_try();
	PHPDBG_G(flags) &= ~PHPDBG_IN_EVAL;

	/* switch stepping back on */
	if (stepping &&
		!(PHPDBG_G(flags) & PHPDBG_IS_STEPONEVAL)) {
		PHPDBG_G(flags) |= PHPDBG_IS_STEPPING;
	}

	CG(unclean_shutdown) = 0;

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(back) /* {{{ */
{
	if (!EG(in_execution)) {
		phpdbg_error("Not executing!");
		return SUCCESS;
	}

	if (!param) {
		phpdbg_dump_backtrace(0 TSRMLS_CC);
	} else {
		phpdbg_dump_backtrace(param->num TSRMLS_CC);
	}

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(print) /* {{{ */
{
	phpdbg_writeln(SEPARATE);
	phpdbg_notice("Execution Context Information");
#ifdef HAVE_LIBREADLINE
	phpdbg_writeln("Readline\tyes");
#else
	phpdbg_writeln("Readline\tno");
#endif
#ifdef HAVE_LIBEDIT
	phpdbg_writeln("Libedit\t\tyes");
#else
	phpdbg_writeln("Libedit\t\tno");
#endif

	phpdbg_writeln("Exec\t\t%s", PHPDBG_G(exec) ? PHPDBG_G(exec) : "none");
	phpdbg_writeln("Compiled\t%s", PHPDBG_G(ops) ? "yes" : "no");
	phpdbg_writeln("Stepping\t%s", (PHPDBG_G(flags) & PHPDBG_IS_STEPPING) ? "on" : "off");
	phpdbg_writeln("Quietness\t%s", (PHPDBG_G(flags) & PHPDBG_IS_QUIET) ? "on" : "off");
	phpdbg_writeln("Oplog\t\t%s", PHPDBG_G(oplog) ? "on" : "off");

	if (PHPDBG_G(ops)) {
		phpdbg_writeln("Opcodes\t\t%d", PHPDBG_G(ops)->last);

		if (PHPDBG_G(ops)->last_var) {
			phpdbg_writeln("Variables\t%d", PHPDBG_G(ops)->last_var-1);
		} else {
			phpdbg_writeln("Variables\tNone");
		}
	}

	phpdbg_writeln("Executing\t%s", EG(in_execution) ? "yes" : "no");
	if (EG(in_execution)) {
		phpdbg_writeln("VM Return\t%d", PHPDBG_G(vmret));
	}

	phpdbg_writeln("Classes\t\t%d", zend_hash_num_elements(EG(class_table)));
	phpdbg_writeln("Functions\t%d", zend_hash_num_elements(EG(function_table)));
	phpdbg_writeln("Constants\t%d", zend_hash_num_elements(EG(zend_constants)));
	phpdbg_writeln("Included\t%d", zend_hash_num_elements(&EG(included_files)));

	phpdbg_writeln(SEPARATE);

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(info) /* {{{ */
{
	phpdbg_error(
		"No information command selected!");

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(set) /* {{{ */
{
	phpdbg_error(
		"No set command selected!");

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(break) /* {{{ */
{
	if (!param) {
		phpdbg_set_breakpoint_file(
				zend_get_executed_filename(TSRMLS_C),
				zend_get_executed_lineno(TSRMLS_C) TSRMLS_CC);
	} else switch (param->type) {
		case ADDR_PARAM:
			phpdbg_set_breakpoint_opline(param->addr TSRMLS_CC);
			break;
		case NUMERIC_PARAM:
			if (PHPDBG_G(exec)) {
				phpdbg_set_breakpoint_file(phpdbg_current_file(TSRMLS_C), param->num TSRMLS_CC);
			} else {
				phpdbg_error("Execution context not set!");
			}
			break;
		case METHOD_PARAM:
			phpdbg_set_breakpoint_method(param->method.class, param->method.name TSRMLS_CC);
			break;
		case NUMERIC_METHOD_PARAM:
			phpdbg_set_breakpoint_method_opline(param->method.class, param->method.name, param->num TSRMLS_CC);
			break;
		case NUMERIC_FUNCTION_PARAM:
			phpdbg_set_breakpoint_function_opline(param->str, param->num TSRMLS_CC);
			break;
		case FILE_PARAM:
			phpdbg_set_breakpoint_file(param->file.name, param->file.line TSRMLS_CC);
			break;
		case NUMERIC_FILE_PARAM:
			phpdbg_set_breakpoint_file_opline(param->file.name, param->file.line TSRMLS_CC);
			break;
		case COND_PARAM:
			phpdbg_set_breakpoint_expression(param->str, param->len TSRMLS_CC);
			break;
		case STR_PARAM:
			phpdbg_set_breakpoint_symbol(param->str, param->len TSRMLS_CC);
			break;
		case OP_PARAM:
			phpdbg_set_breakpoint_opcode(param->str, param->len TSRMLS_CC);
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(sh) /* {{{ */
{
	FILE *fd = NULL;
	if ((fd=VCWD_POPEN((char*)param->str, "w"))) {
		/* do something perhaps ?? do we want input ?? */
		fclose(fd);
	} else {
		phpdbg_error(
			"Failed to execute %s", param->str);
	}
	
	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(source) /* {{{ */
{
	struct stat sb;
	
	if (VCWD_STAT(param->str, &sb) != -1) {
		phpdbg_try_file_init(param->str, param->len, 0 TSRMLS_CC);
	} else {
		phpdbg_error(
			"Failed to stat %s, file does not exist", param->str);
	}
			
	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(export) /* {{{ */
{
	FILE *handle = VCWD_FOPEN(param->str, "w+");
	
	if (handle) {
		phpdbg_export_breakpoints(handle TSRMLS_CC);
		fclose(handle);
	} else {
		phpdbg_error(
			"Failed to open or create %s, check path and permissions", param->str);
	}
	
	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(register) /* {{{ */
{
	zend_function *function;
	char *lcname = zend_str_tolower_dup(param->str, param->len);
	size_t lcname_len = strlen(lcname);

	if (!zend_hash_exists(&PHPDBG_G(registered), lcname, lcname_len+1)) {
		if (zend_hash_find(EG(function_table), lcname, lcname_len+1, (void**) &function) == SUCCESS) {
			zend_hash_update(
				&PHPDBG_G(registered), lcname, lcname_len+1, (void*)&function, sizeof(zend_function), NULL);
			function_add_ref(function);

			phpdbg_notice(
				"Registered %s", lcname);
		} else {
			phpdbg_error("The requested function (%s) could not be found", param->str);
		}
	} else {
		phpdbg_error(
			"The requested name (%s) is already in use", lcname);
	}

	efree(lcname);
	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(quit) /* {{{ */
{
	/* don't allow this to loop, ever ... */
	if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
		PHPDBG_G(flags) |= PHPDBG_IS_QUITTING;
		zend_bailout();
	}

	return PHPDBG_NEXT;
} /* }}} */

PHPDBG_COMMAND(clean) /* {{{ */
{
	if (EG(in_execution)) {
		phpdbg_error("Cannot clean environment while executing");
		return SUCCESS;
	}

	phpdbg_notice("Cleaning Execution Environment");

	phpdbg_writeln("Classes\t\t\t%d", zend_hash_num_elements(EG(class_table)));
	phpdbg_writeln("Functions\t\t%d", zend_hash_num_elements(EG(function_table)));
	phpdbg_writeln("Constants\t\t%d", zend_hash_num_elements(EG(zend_constants)));
	phpdbg_writeln("Includes\t\t%d", zend_hash_num_elements(&EG(included_files)));

	phpdbg_clean(1 TSRMLS_CC);

	return SUCCESS;
} /* }}} */

PHPDBG_COMMAND(clear) /* {{{ */
{
	phpdbg_notice("Clearing Breakpoints");

	phpdbg_writeln("File\t\t\t%d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE]));
	phpdbg_writeln("Functions\t\t%d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM]));
	phpdbg_writeln("Methods\t\t\t%d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD]));
	phpdbg_writeln("Oplines\t\t\t%d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]));
	phpdbg_writeln("File oplines\t\t\t%d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_OPLINE]));
	phpdbg_writeln("Function oplines\t\t\t%d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_FUNCTION_OPLINE]));
	phpdbg_writeln("Method oplines\t\t\t%d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD_OPLINE]));
	phpdbg_writeln("Conditionals\t\t%d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_COND]));

	phpdbg_clear_breakpoints(TSRMLS_C);

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
			phpdbg_list_function_byname(param->str, param->len TSRMLS_CC);
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
		phpdbg_list_watchpoints(TSRMLS_C);
	} else switch (param->type) {
		case STR_PARAM:
			if (phpdbg_create_var_watchpoint(param->str, param->len TSRMLS_CC) != FAILURE) {
				phpdbg_notice("Set watchpoint on %.*s", (int)param->len, param->str);
			}
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

int phpdbg_interactive(TSRMLS_D) /* {{{ */
{
	int ret = SUCCESS;
	char *why = NULL;
	char *input = NULL;
	phpdbg_param_t stack;

	PHPDBG_G(flags) |= PHPDBG_IS_INTERACTIVE;

	input = phpdbg_read_input(NULL TSRMLS_CC);

	if (input) {
		do {
			phpdbg_init_param(&stack, STACK_PARAM);

			if (phpdbg_do_parse(&stack, input TSRMLS_CC) <= 0) {
				switch (ret = phpdbg_stack_execute(&stack, &why TSRMLS_CC)) {
					case FAILURE:
						if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
							if (phpdbg_call_register(&stack TSRMLS_CC) == FAILURE) {
								if (why) {
									phpdbg_error("%s", why);
								}
							}
						}

						if (why) {
							free(why);
							why = NULL;
						}
					break;

					case PHPDBG_LEAVE:
					case PHPDBG_FINISH:
					case PHPDBG_UNTIL:
					case PHPDBG_NEXT: {
						if (!EG(in_execution) && !(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
							phpdbg_error("Not running");
						}
						goto out;
					}
				}
			}

			if (why) {
				free(why);
				why = NULL;
			}

			phpdbg_stack_free(&stack);
			phpdbg_destroy_input(&input TSRMLS_CC);

		} while ((input = phpdbg_read_input(NULL TSRMLS_CC)));
	}

out:
	if (input) {
		phpdbg_stack_free(&stack);
		phpdbg_destroy_input(&input TSRMLS_CC);
	}

	if (why) {
		free(why);
	}

	if (EG(in_execution)) {
		phpdbg_restore_frame(TSRMLS_C);
	}

	PHPDBG_G(flags) &= ~PHPDBG_IS_INTERACTIVE;

	phpdbg_print_changed_zvals(TSRMLS_C);

	return ret;
} /* }}} */

void phpdbg_clean(zend_bool full TSRMLS_DC) /* {{{ */
{
	/* this is implicitly required */
	if (PHPDBG_G(ops)) {
		destroy_op_array(PHPDBG_G(ops) TSRMLS_CC);
		efree(PHPDBG_G(ops));
		PHPDBG_G(ops) = NULL;
	}

	if (full) {
		PHPDBG_G(flags) |= PHPDBG_IS_CLEANING;

		zend_bailout();
	}
} /* }}} */

static inline zend_execute_data *phpdbg_create_execute_data(zend_op_array *op_array, zend_bool nested TSRMLS_DC) /* {{{ */
{
#if PHP_VERSION_ID >= 50500
	return zend_create_execute_data_from_op_array(op_array, nested TSRMLS_CC);
#else

#undef EX
#define EX(element) execute_data->element
#undef EX_CV
#define EX_CV(var) EX(CVs)[var]
#undef EX_CVs
#define EX_CVs() EX(CVs)
#undef EX_T
#define EX_T(offset) (*(temp_variable *)((char *) EX(Ts) + offset))
#undef EX_Ts
#define EX_Ts() EX(Ts)

	zend_execute_data *execute_data = (zend_execute_data *)zend_vm_stack_alloc(
		ZEND_MM_ALIGNED_SIZE(sizeof(zend_execute_data)) +
		ZEND_MM_ALIGNED_SIZE(sizeof(zval**) * op_array->last_var * (EG(active_symbol_table) ? 1 : 2)) +
		ZEND_MM_ALIGNED_SIZE(sizeof(temp_variable)) * op_array->T TSRMLS_CC);

	EX(CVs) = (zval***)((char*)execute_data + ZEND_MM_ALIGNED_SIZE(sizeof(zend_execute_data)));
	memset(EX(CVs), 0, sizeof(zval**) * op_array->last_var);
	EX(Ts) = (temp_variable *)(((char*)EX(CVs)) + ZEND_MM_ALIGNED_SIZE(sizeof(zval**) * op_array->last_var * (EG(active_symbol_table) ? 1 : 2)));
	EX(fbc) = NULL;
	EX(called_scope) = NULL;
	EX(object) = NULL;
	EX(old_error_reporting) = NULL;
	EX(op_array) = op_array;
	EX(symbol_table) = EG(active_symbol_table);
	EX(prev_execute_data) = EG(current_execute_data);
	EG(current_execute_data) = execute_data;
	EX(nested) = nested;

	if (!op_array->run_time_cache && op_array->last_cache_slot) {
		op_array->run_time_cache = ecalloc(op_array->last_cache_slot, sizeof(void*));
	}

	if (op_array->this_var != -1 && EG(This)) {
 		Z_ADDREF_P(EG(This)); /* For $this pointer */
		if (!EG(active_symbol_table)) {
			EX_CV(op_array->this_var) = (zval**)EX_CVs() + (op_array->last_var + op_array->this_var);
			*EX_CV(op_array->this_var) = EG(This);
		} else {
			if (zend_hash_add(EG(active_symbol_table), "this", sizeof("this"), &EG(This), sizeof(zval *), (void**)&EX_CV(op_array->this_var))==FAILURE) {
				Z_DELREF_P(EG(This));
			}
		}
	}

	EX(opline) = UNEXPECTED((op_array->fn_flags & ZEND_ACC_INTERACTIVE) != 0) && EG(start_op) ? EG(start_op) : op_array->opcodes;
	EG(opline_ptr) = &EX(opline);

	EX(function_state).function = (zend_function *) op_array;
	EX(function_state).arguments = NULL;

	return execute_data;
#endif
} /* }}} */

#if PHP_VERSION_ID >= 50500
void phpdbg_execute_ex(zend_execute_data *execute_data TSRMLS_DC) /* {{{ */
{
#else
void phpdbg_execute_ex(zend_op_array *op_array TSRMLS_DC) /* {{{ */
{
	long long flags = 0;
	zend_ulong address = 0L;
	zend_execute_data *execute_data;
	zend_bool nested = 0;
#endif
	zend_bool original_in_execution = EG(in_execution);
	HashTable vars;

#if PHP_VERSION_ID < 50500
	if (EG(exception)) {
		return;
	}
#endif

	EG(in_execution) = 1;

#if PHP_VERSION_ID >= 50500
	if (0) {
zend_vm_enter:
		execute_data = phpdbg_create_execute_data(EG(active_op_array), 1 TSRMLS_CC);
	}
	zend_hash_init(&vars, EG(active_op_array)->last, NULL, NULL, 0);
#else
zend_vm_enter:
	execute_data = phpdbg_create_execute_data(op_array, nested TSRMLS_CC);
	nested = 1;
	zend_hash_init(&vars, EG(active_op_array)->last, NULL, NULL, 0);
#endif

	while (1) {
	
		if ((PHPDBG_G(flags) & PHPDBG_BP_RESOLVE_MASK)) {
			/* resolve nth opline breakpoints */
			phpdbg_resolve_op_array_breaks(EG(active_op_array) TSRMLS_CC);
		}
		
#ifdef ZEND_WIN32
		if (EG(timed_out)) {
			zend_timeout(0);
		}
#endif

#define DO_INTERACTIVE() do { \
	if (!(PHPDBG_G(flags) & PHPDBG_IN_EVAL)) { \
		phpdbg_list_file( \
			zend_get_executed_filename(TSRMLS_C), \
			3, \
			zend_get_executed_lineno(TSRMLS_C)-1, \
			zend_get_executed_lineno(TSRMLS_C) \
			TSRMLS_CC \
		); \
	} \
	\
/*	do { */\
		switch (phpdbg_interactive(TSRMLS_C)) { \
			case PHPDBG_LEAVE: \
			case PHPDBG_FINISH: \
			case PHPDBG_UNTIL: \
			case PHPDBG_NEXT:{ \
				goto next; \
			} \
		} \
/*	} while (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)); */\
} while (0)

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
					zend_hash_clean(
						&PHPDBG_G(seek));
				} else {
					/* skip possible breakpoints */
					goto next;
				}
			}

			/* run to finish */
			if (PHPDBG_G(flags) & PHPDBG_IN_FINISH) {
				if (zend_hash_index_exists(&PHPDBG_G(seek), address)) {
					PHPDBG_G(flags) &= ~PHPDBG_IN_FINISH;
					zend_hash_clean(
						&PHPDBG_G(seek));
				}
				/* skip possible breakpoints */
				goto next;
			}

			/* break for leave */
			if (PHPDBG_G(flags) & PHPDBG_IN_LEAVE) {
				if (zend_hash_index_exists(&PHPDBG_G(seek), address)) {
					PHPDBG_G(flags) &= ~PHPDBG_IN_LEAVE;
					zend_hash_clean(
						&PHPDBG_G(seek));
					phpdbg_notice(
						"Breaking for leave at %s:%u",
						zend_get_executed_filename(TSRMLS_C),
						zend_get_executed_lineno(TSRMLS_C)
					);
					DO_INTERACTIVE();
				} else {
					/* skip possible breakpoints */
					goto next;
				}
			}
		}

		/* not while in conditionals */
		phpdbg_print_opline_ex(
			execute_data, &vars, 0 TSRMLS_CC);

		if (PHPDBG_G(flags) & PHPDBG_IS_STEPPING && (PHPDBG_G(flags) & PHPDBG_STEP_OPCODE || execute_data->opline->lineno != PHPDBG_G(last_line))) {
			PHPDBG_G(flags) &= ~PHPDBG_IS_STEPPING;
			DO_INTERACTIVE();
		}

		/* check if some watchpoint was hit */
		{
			if (phpdbg_print_changed_zvals(TSRMLS_C) == SUCCESS) {
				DO_INTERACTIVE();
			}
		}

		/* search for breakpoints */
		{
			phpdbg_breakbase_t *brake;

			if ((PHPDBG_G(flags) & PHPDBG_BP_MASK)
			    && (brake = phpdbg_find_breakpoint(execute_data TSRMLS_CC))
			    && (brake->type != PHPDBG_BREAK_FILE || execute_data->opline->lineno != PHPDBG_G(last_line))) {
				phpdbg_hit_breakpoint(brake, 1 TSRMLS_CC);
				DO_INTERACTIVE();
			}
		}

next:
		if (PHPDBG_G(flags) & PHPDBG_IS_SIGNALED) {
			phpdbg_writeln(EMPTY);
			phpdbg_notice("Program received signal SIGINT");
			PHPDBG_G(flags) &= ~PHPDBG_IS_SIGNALED;
			DO_INTERACTIVE();
		}

		PHPDBG_G(last_line) = execute_data->opline->lineno;

		PHPDBG_G(vmret) = execute_data->opline->handler(execute_data TSRMLS_CC);

		if (PHPDBG_G(vmret) > 0) {
			switch (PHPDBG_G(vmret)) {
				case 1:
					EG(in_execution) = original_in_execution;
					zend_hash_destroy(&vars);
					return;
				case 2:
#if PHP_VERSION_ID < 50500
					op_array = EG(active_op_array);
#endif
					zend_hash_destroy(&vars);
					goto zend_vm_enter;
					break;
				case 3:
					execute_data = EG(current_execute_data);
					break;
				default:
					break;
			}
		}
	}
	zend_error_noreturn(E_ERROR, "Arrived at end of main loop which shouldn't happen");
} /* }}} */
