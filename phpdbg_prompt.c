/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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
   +----------------------------------------------------------------------+
*/

#include <stdio.h>
#include <string.h>
#include "zend.h"
#include "zend_compile.h"
#include "phpdbg.h"
#include "phpdbg_help.h"
#include "phpdbg_print.h"
#include "phpdbg_bp.h"
#include "phpdbg_opcode.h"
#include "phpdbg_list.h"
#include "phpdbg_utils.h"

static const phpdbg_command_t phpdbg_prompt_commands[];

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

static PHPDBG_COMMAND(exec) /* {{{ */
{
	if (PHPDBG_G(exec)) {
		printf(
		    "%sUnsetting old execution context: %s%s\n",
		    PHPDBG_BOLD_LINE(TSRMLS_C), PHPDBG_G(exec), PHPDBG_END_LINE(TSRMLS_C));
		efree(PHPDBG_G(exec));
		PHPDBG_G(exec) = NULL;
	}

	if (PHPDBG_G(ops)) {
		printf(
		    "%sDestroying compiled opcodes%s\n",
		    PHPDBG_BOLD_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));

   		phpdbg_clean(0 TSRMLS_CC);
	}

	PHPDBG_G(exec) = estrndup(expr, PHPDBG_G(exec_len) = expr_len);

	printf(
	    "%sSet execution context: %s%s\n",
	    PHPDBG_BOLD_LINE(TSRMLS_C), PHPDBG_G(exec), PHPDBG_END_LINE(TSRMLS_C));

	return SUCCESS;
} /* }}} */

static inline int phpdbg_compile(TSRMLS_D) /* {{{ */
{
	zend_file_handle fh;

	if (!EG(in_execution)) {
	    printf("%sAttempting compilation of %s%s\n", PHPDBG_BOLD_LINE(TSRMLS_C), PHPDBG_G(exec), PHPDBG_END_LINE(TSRMLS_C));

	    if (php_stream_open_for_zend_ex(PHPDBG_G(exec), &fh,
		        USE_PATH|STREAM_OPEN_FOR_INCLUDE TSRMLS_CC) == SUCCESS) {
		    PHPDBG_G(ops) = zend_compile_file(
		        &fh, ZEND_INCLUDE TSRMLS_CC);
		    zend_destroy_file_handle(&fh TSRMLS_CC);
		    printf(
		        "%sSuccess%s\n",
		        PHPDBG_BOLD_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));
		    return SUCCESS;
        } else {
            printf(
                "%sCould not open file %s%s\n",
                PHPDBG_RED_LINE(TSRMLS_C), PHPDBG_G(exec), PHPDBG_END_LINE(TSRMLS_C));
        }
	} else {
	    printf(
	        "%sCannot compile while in execution%s\n",
	        PHPDBG_RED_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));
	}

	return FAILURE;
} /* }}} */

static PHPDBG_COMMAND(compile) /* {{{ */
{
	if (PHPDBG_G(exec)) {
		if (!EG(in_execution)) {
		    if (PHPDBG_G(ops)) {
			    printf(
			        "%sDestroying previously compiled opcodes%s\n",
			        PHPDBG_BOLD_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));
			    phpdbg_clean(0 TSRMLS_CC);
		    }
		}

		return phpdbg_compile(TSRMLS_C);
	} else {
		printf(
		    "%sNo execution context%s\n",
		    PHPDBG_RED_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));
		return FAILURE;
	}
} /* }}} */

static PHPDBG_COMMAND(step) /* {{{ */
{
	if (atoi(expr)) {
	    PHPDBG_G(flags) |= PHPDBG_IS_STEPPING;
	} else {
	    PHPDBG_G(flags) &= ~PHPDBG_IS_STEPPING;
	}

	printf(
	    "%sStepping %s%s\n",
	    PHPDBG_BOLD_LINE(TSRMLS_C),
	    (PHPDBG_G(flags) & PHPDBG_IS_STEPPING) ? "on" : "off",
	    PHPDBG_END_LINE(TSRMLS_C));

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(next) /* {{{ */
{
	return PHPDBG_NEXT;
} /* }}} */

static PHPDBG_COMMAND(run) /* {{{ */
{
    if (EG(in_execution)) {
        printf("%sCannot start another execution while one is in progress%s\n", PHPDBG_RED_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));
        return FAILURE;
    }

	if (PHPDBG_G(ops) || PHPDBG_G(exec)) {
		if (!PHPDBG_G(ops)) {
			if (phpdbg_compile(TSRMLS_C) == FAILURE) {
				printf("%sFailed to compile %s, cannot run%s\n", PHPDBG_RED_LINE(TSRMLS_C), PHPDBG_G(exec), PHPDBG_END_LINE(TSRMLS_C));
				return FAILURE;
			}
		}

		EG(active_op_array) = PHPDBG_G(ops);
		EG(return_value_ptr_ptr) = &PHPDBG_G(retval);

		zend_try {
			zend_execute(EG(active_op_array) TSRMLS_CC);
		} zend_catch {
			if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
				printf("%sCaught excetion in VM%s\n", PHPDBG_RED_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));
				return FAILURE;
			} else return SUCCESS;
		} zend_end_try();

		return SUCCESS;
	} else {
		printf("%sNothing to execute !%s\n", PHPDBG_RED_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));
		return FAILURE;
	}
} /* }}} */

static PHPDBG_COMMAND(eval) /* {{{ */
{
	zval retval;

	if (expr_len) {
	    zend_bool stepping = (PHPDBG_G(flags) & PHPDBG_IS_STEPPING);

	    /* disable stepping while eval() in progress */
	    PHPDBG_G(flags) &= ~ PHPDBG_IS_STEPPING;

		if (zend_eval_stringl((char*)expr, expr_len-1,
			&retval, "eval()'d code" TSRMLS_CC) == SUCCESS) {
			zend_print_zval_r(
			    &retval, 0 TSRMLS_CC);
			zval_dtor(&retval);
			printf("\n");
		}

		/* switch stepping back on */
		if (stepping) {
		    PHPDBG_G(flags) |= PHPDBG_IS_STEPPING;
		}
	} else {
		printf("%sNo expression provided !%s\n", PHPDBG_RED_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));
		return FAILURE;
	}

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(back) /* {{{ */
{
	if (!EG(in_execution)) {
		printf("%sNot executing !%s\n", PHPDBG_RED_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));
		return FAILURE;
	}
	zval zbacktrace;
	zval **tmp;
	HashPosition position;
	int i = 0, limit = (expr != NULL) ? atoi(expr) : 0;

	zend_fetch_debug_backtrace(&zbacktrace, 0, 0, limit TSRMLS_CC);

	for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL(zbacktrace), &position);
		zend_hash_get_current_data_ex(Z_ARRVAL(zbacktrace), (void**)&tmp, &position) == SUCCESS;
		zend_hash_move_forward_ex(Z_ARRVAL(zbacktrace), &position)) {
		if (i++) {
			printf(",\n");
		}
		zend_print_flat_zval_r(*tmp TSRMLS_CC);
	}

	printf("\n");
	zval_dtor(&zbacktrace);

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(print) /* {{{ */
{
	if (expr_len > 0L) {
		if (phpdbg_do_cmd(phpdbg_print_commands, (char*)expr, expr_len TSRMLS_CC) == FAILURE) {
			printf(
			    "%sFailed to find print command: %s/%lu%s\n",
			    PHPDBG_RED_LINE(TSRMLS_C), expr, expr_len, PHPDBG_END_LINE(TSRMLS_C));
		}
		return SUCCESS;
	}

    printf("--------------------------------------\n");
	printf("Execution Context Information:\n");
#ifdef HAVE_LIBREADLINE
    printf("Readline\tyes\n");
#else
    printf("Readline\tno\n");
#endif
	printf("Exec\t\t%s\n", PHPDBG_G(exec) ? PHPDBG_G(exec) : "none");
	printf("Compiled\t%s\n", PHPDBG_G(ops) ? "yes" : "no");
	printf("Stepping\t%s\n", (PHPDBG_G(flags) & PHPDBG_IS_STEPPING) ? "on" : "off");
    printf("Quietness\t%s\n", (PHPDBG_G(flags) & PHPDBG_IS_QUIET) ? "on" : "off");

	if (PHPDBG_G(ops)) {
		printf("Opcodes\t\t%d\n", PHPDBG_G(ops)->last);

		if (PHPDBG_G(ops)->last_var) {
			printf("Variables\t%d\n", PHPDBG_G(ops)->last_var-1);
		} else {
			printf("Variables\tNone\n");
		}
	}
	printf("Executing\t%s\n", EG(in_execution) ? "yes" : "no");
	if (EG(in_execution)) {
		printf("VM Return\t%d\n", PHPDBG_G(vmret));
	}
	printf("Classes\t\t%d\n", zend_hash_num_elements(EG(class_table)));
    printf("Functions\t%d\n", zend_hash_num_elements(EG(function_table)));
    printf("Constants\t%d\n", zend_hash_num_elements(EG(zend_constants)));
    printf("Included\t%d\n", zend_hash_num_elements(&EG(included_files)));

    if ((PHPDBG_G(flags) & PHPDBG_HAS_FILE_BP)) {
        HashPosition position;
        zend_llist *points;

        printf("--------------------------------------\n");
        printf("File Break Point Information:\n");
        for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], &position);
             zend_hash_get_current_data_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], (void**) &points, &position) == SUCCESS;
             zend_hash_move_forward_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], &position)) {
             zend_llist_position lposition;
             phpdbg_breakfile_t *brake;

             if ((brake = zend_llist_get_first_ex(points, &lposition))) {
                printf("%s:\n", brake->filename);
                do {
                    printf("\t%lu\n", brake->line);
                } while ((brake = zend_llist_get_next_ex(points, &lposition)));
             }
        }
    }

#if 0
    if (PHPDBG_G(has_sym_bp)) {
        HashPosition position;
        zend_llist *points;

        printf("--------------------------------------\n");
        printf("Symbol Break Point Information:\n");
        for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], &position);
             zend_hash_get_current_data_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], (void**) &points, &position) == SUCCESS;
             zend_hash_move_forward_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], &position)) {
             zend_llist_position lposition;
             phpdbg_breaksymbol_t *brake;

             if ((brake = zend_llist_get_first_ex(points, &lposition))) {
                printf("%s:\n", brake->symbol);
                do {
                    printf("\t%d\n", brake->id);
                } while ((brake = zend_llist_get_next_ex(points, &lposition)));
             }
        }
    }
#endif

    if ((PHPDBG_G(flags) & PHPDBG_HAS_OPLINE_BP)) {
        HashPosition position;
        phpdbg_breakline_t *brake;

        printf("--------------------------------------\n");
        printf("Opline Break Point Information:\n");
        for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], &position);
             zend_hash_get_current_data_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], (void**) &brake, &position) == SUCCESS;
             zend_hash_move_forward_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], &position)) {
             printf("#%d\t%s\n", brake->id, brake->name);
        }
    }

    printf("--------------------------------------\n");
	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(break) /* {{{ */
{
	char *line_pos = NULL;
    char *func_pos = NULL;

    if (expr_len <= 0L) {
        printf(
            "%sNo expression found%s\n", PHPDBG_RED_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));
        return FAILURE;
    }

    line_pos  = strchr(expr, ':');

	if (line_pos) {
	    if (!(func_pos=strchr(line_pos+1, ':'))) {
	        char path[MAXPATHLEN], resolved_name[MAXPATHLEN];
		    long line_num = strtol(line_pos+1, NULL, 0);

		    if (line_num) {
		        memcpy(path, expr, line_pos - expr);
		        path[line_pos - expr] = 0;

		        if (expand_filepath(path, resolved_name TSRMLS_CC) == NULL) {
			        printf("%sFailed to expand path %s%s\n", PHPDBG_RED_LINE(TSRMLS_C), path, PHPDBG_END_LINE(TSRMLS_C));
			        return FAILURE;
		        }

		        phpdbg_set_breakpoint_file(resolved_name, line_num TSRMLS_CC);
		    } else {
		        printf("%sNo line specified in expression %s%s\n", PHPDBG_RED_LINE(TSRMLS_C), expr, PHPDBG_END_LINE(TSRMLS_C));
		        return FAILURE;
		    }
	    } else {
		    char *class;
		    char *func;

		    size_t func_len = strlen(func_pos+1),
		           class_len = (line_pos - expr);

		    if (func_len) {
		        class = emalloc(class_len+1);
		        func = emalloc(func_len+1);

		        memcpy(class, expr, class_len);
		        class[class_len]='\0';
		        memcpy(func, func_pos+1, func_len);
		        func[func_len]='\0';

		        phpdbg_set_breakpoint_method(class, class_len, func, func_len TSRMLS_CC);
		    } else {
		        printf("%sNo function found in method expression %s%s\n", PHPDBG_RED_LINE(TSRMLS_C), expr, PHPDBG_END_LINE(TSRMLS_C));
		        return FAILURE;
		    }
		}
	} else {
		if (phpdbg_is_addr(expr)) {
			zend_ulong opline = strtoul(expr, 0, 16);

			phpdbg_set_breakpoint_opline(opline TSRMLS_CC);
		} else if (phpdbg_is_numeric(expr)) {
			const char *filename = zend_get_executed_filename(TSRMLS_C);
			long line_num = strtol(expr, NULL, 0);

			if (!filename) {
				printf("%sNo file context found%s\n",
					PHPDBG_RED_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));
				return FAILURE;
			}

			phpdbg_set_breakpoint_file(filename, line_num TSRMLS_CC);
		} else {
			char name[200];
			size_t name_len = strlen(expr);

			if (name_len) {
				name_len = MIN(name_len, 200);
				memcpy(name, expr, name_len);
				name[name_len] = 0;

				phpdbg_set_breakpoint_symbol(name TSRMLS_CC);
			} else {
				printf("%sMalformed break command found%s\n",
					PHPDBG_RED_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));
				return FAILURE;
			}
		}
	}

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(quit) /* {{{ */
{
    PHPDBG_G(flags) |= PHPDBG_IS_QUITTING;

	zend_bailout();

	return SUCCESS;
} /* }}} */

static int clean_non_persistent_constant_full(const zend_constant *c TSRMLS_DC) /* {{{ */
{
    return (c->flags & CONST_PERSISTENT) ? 0 : 1;
} /* }}} */

static int clean_non_persistent_class_full(zend_class_entry **ce TSRMLS_DC) /* {{{ */
{
    return ((*ce)->type == ZEND_INTERNAL_CLASS) ? ZEND_HASH_APPLY_KEEP : ZEND_HASH_APPLY_REMOVE;
} /* }}} */

static int clean_non_persistent_function_full(zend_function *function TSRMLS_DC) /* {{{ */
{
    return (function->type == ZEND_INTERNAL_FUNCTION) ? ZEND_HASH_APPLY_KEEP : ZEND_HASH_APPLY_REMOVE;
} /* }}} */

static PHPDBG_COMMAND(clean) /* {{{ */
{
    if (!EG(in_execution)) {
        printf("[Cleaning Environment:]\n");
        printf("[\tClasses: %d]\n", zend_hash_num_elements(EG(class_table)));
        printf("[\tFunctions: %d]\n", zend_hash_num_elements(EG(function_table)));
        printf("[\tConstants: %d]\n", zend_hash_num_elements(EG(zend_constants)));
        printf("[\tIncluded: %d]\n", zend_hash_num_elements(&EG(included_files)));

        phpdbg_clean(1 TSRMLS_CC);

        printf("[Clean Environment:]\n");
        printf("[\tClasses: %d]\n", zend_hash_num_elements(EG(class_table)));
        printf("[\tFunctions: %d]\n", zend_hash_num_elements(EG(function_table)));
        printf("[\tConstants: %d]\n", zend_hash_num_elements(EG(zend_constants)));
        printf("[\tIncluded: %d]\n", zend_hash_num_elements(&EG(included_files)));
    } else {
        printf(
            "%sCannot clean environment while executing%s\n", PHPDBG_RED_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));
        return FAILURE;
    }

    return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(clear) /* {{{ */
{
    printf("[Clearing Breakpoints:]\n");
    printf("[\tFile\t%d]\n", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE]));
    printf("[\tSymbols\t%d]\n", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM]));
    printf("[\tOplines\t%d]\n", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]));
    printf("[\tMethods\t%d]\n", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD]));

    phpdbg_clear_breakpoints(TSRMLS_C);

    return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(help) /* {{{ */
{
	printf(
	    "%sWelcome to phpdbg, the interactive PHP debugger, v%s%s\n",
	    PHPDBG_BOLD_LINE(TSRMLS_C), PHPDBG_VERSION, PHPDBG_END_LINE(TSRMLS_C));

	if (expr_len > 0L) {
		if (phpdbg_do_cmd(phpdbg_help_commands, (char*)expr, expr_len TSRMLS_CC) == FAILURE) {
			printf(
			    "%sFailed to find help command: %s/%lu%s\n",
			    PHPDBG_RED_LINE(TSRMLS_C), expr, expr_len, PHPDBG_END_LINE(TSRMLS_C));
		}
	} else {
		const phpdbg_command_t *prompt_command = phpdbg_prompt_commands;
		const phpdbg_command_t *help_command = phpdbg_help_commands;

		printf("To get help regarding a specific command type \"help command\"\n");

		printf(
		    "%sCommands%s\n",
		    PHPDBG_BOLD_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));

		while (prompt_command && prompt_command->name) {
			printf("\t%s\t%s\n", prompt_command->name, prompt_command->tip);
			++prompt_command;
		}

		printf(
		    "%sHelpers Loaded%s\n",
		    PHPDBG_BOLD_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));

		while (help_command && help_command->name) {
			printf("\t%s\t%s\n", help_command->name, help_command->tip);
			++help_command;
		}
	}

	printf(
	    "%sPlease report bugs to <%s>%s\n",
	    PHPDBG_BOLD_LINE(TSRMLS_C), PHPDBG_ISSUES, PHPDBG_END_LINE(TSRMLS_C));

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(quiet) { /* {{{ */
    if (atoi(expr)) {
        PHPDBG_G(flags) |= PHPDBG_IS_QUIET;
    } else {
        PHPDBG_G(flags) &= ~PHPDBG_IS_QUIET;
    }

    printf(
        "%sQuietness %s%s\n", PHPDBG_BOLD_LINE(TSRMLS_C), (PHPDBG_G(flags) & PHPDBG_IS_QUIET) ? "enabled" : "disabled", PHPDBG_END_LINE(TSRMLS_C));

    return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(list) /* {{{ */
{
	if (phpdbg_is_empty(expr) || phpdbg_is_numeric(expr)) {
		long offset = 0, count = strtol(expr, NULL, 0);
		const char *filename = PHPDBG_G(exec);

		if (zend_is_executing(TSRMLS_C)) {
			filename = zend_get_executed_filename(TSRMLS_C);
			offset = zend_get_executed_lineno(TSRMLS_C);
		} else if (!filename) {
			printf(
			    "%sNo file to list%s\n",
			    PHPDBG_RED_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));
			return SUCCESS;
		}

		phpdbg_list_file(filename, count, offset TSRMLS_CC);
	} else {
	    HashTable *func_table = EG(function_table);
		zend_function* fbc;
        const char *func_name = expr;
        size_t func_name_len = expr_len;

        /* search active scope if begins with period */
        if (func_name[0] == '.') {
           if (EG(scope)) {
               func_name++;
               func_name_len--;

               func_table = &EG(scope)->function_table;
           } else {
               printf(
                   "%sNo active class%s\n",
                   PHPDBG_RED_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));
               return FAILURE;
           }
        } else if (!EG(function_table)) {
			printf(
			    "%sNo function table loaded%s\n",
			    PHPDBG_RED_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));
			return SUCCESS;
		} else {
		    func_table = EG(function_table);
		}

		if (zend_hash_find(func_table, func_name, func_name_len,
			(void**)&fbc) == SUCCESS) {
			phpdbg_list_function(fbc TSRMLS_CC);
		} else {
		    printf(
		        "%sFunction %s not found%s\n",
		        PHPDBG_RED_LINE(TSRMLS_C), func_name, PHPDBG_END_LINE(TSRMLS_C));
		}
	}

	return SUCCESS;
} /* }}} */

static const phpdbg_command_t phpdbg_prompt_commands[] = {
	PHPDBG_COMMAND_D(exec,      "set execution context"),
	PHPDBG_COMMAND_D(compile,   "attempt to pre-compile execution context"),
	PHPDBG_COMMAND_D(step,      "step through execution"),
	PHPDBG_COMMAND_D(next,      "continue execution"),
	PHPDBG_COMMAND_D(run,       "attempt execution"),
	PHPDBG_COMMAND_D(eval,      "evaluate some code"),
	PHPDBG_COMMAND_D(print,     "print something"),
	PHPDBG_COMMAND_D(break,     "set breakpoint"),
	PHPDBG_COMMAND_D(back,      "show backtrace"),
	PHPDBG_COMMAND_D(list,      "list specified line or function"),
	PHPDBG_COMMAND_D(clean,     "clean the execution environment"),
	PHPDBG_COMMAND_D(clear,     "clear breakpoints"),
	PHPDBG_COMMAND_D(help,      "show help menu"),
	PHPDBG_COMMAND_D(quiet,     "silence some output"),
	PHPDBG_COMMAND_D(quit,      "exit phpdbg"),
	{NULL, 0, 0}
};

int phpdbg_do_cmd(const phpdbg_command_t *command, char *cmd_line, size_t cmd_len TSRMLS_DC) /* {{{ */
{
	char *params = NULL;
	const char *cmd = strtok_r(cmd_line, " ", &params);
	size_t expr_len = cmd != NULL ? strlen(cmd) : 0;

	while (command && command->name) {
		if (command->name_len == expr_len
			    && memcmp(cmd, command->name, expr_len) == 0) {
			PHPDBG_G(last) = (phpdbg_command_t*) command;
			PHPDBG_G(last_params) = params;
			PHPDBG_G(last_params_len) = cmd_len - expr_len;
			return command->handler(params, cmd_len - expr_len TSRMLS_CC);
		}
		++command;
	}

	return FAILURE;
} /* }}} */

int phpdbg_interactive(TSRMLS_D) /* {{{ */
{
    size_t cmd_len;

#ifndef HAVE_LIBREADLINE
    char cmd[PHPDBG_MAX_CMD];

phpdbg_interactive_enter:
    printf(PHPDBG_PROMPT_LINE(TSRMLS_C));

	while (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING) &&
	       fgets(cmd, PHPDBG_MAX_CMD, stdin) != NULL) {
	    cmd_len = strlen(cmd) - 1;
#else
    char *cmd = NULL;

phpdbg_interactive_enter:
    while (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
        cmd = readline(PHPDBG_PROMPT_LINE(TSRMLS_C));

        cmd_len = strlen(cmd);
#endif

		/* trim space from end of input */
		while (isspace(cmd[cmd_len-1]))
		    cmd_len--;

		/* ensure string is null terminated */
		cmd[cmd_len] = '\0';

		if (cmd && cmd_len > 0L) {
#ifdef HAVE_LIBREADLINE
            add_history(cmd);
#endif

		    switch (phpdbg_do_cmd(phpdbg_prompt_commands, cmd, cmd_len TSRMLS_CC)) {
		        case FAILURE:
		            if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
		                printf(
		                    "%sFailed to execute %s !%s\n",
		                    PHPDBG_RED_LINE(TSRMLS_C), cmd, PHPDBG_END_LINE(TSRMLS_C));
		            }
		        break;

		        case PHPDBG_NEXT: {
		            if (!EG(in_execution)) {
		                printf(
		                    "%sNot running%s\n",
		                    PHPDBG_RED_LINE(TSRMLS_C), PHPDBG_END_LINE(TSRMLS_C));
		            }
		            return PHPDBG_NEXT;
		        }
		    }
		} else if (PHPDBG_G(last)) {
		    PHPDBG_G(last)->handler(
		        PHPDBG_G(last_params), PHPDBG_G(last_params_len) TSRMLS_CC);
		}

		if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
            goto phpdbg_interactive_enter;
		}
	}

	return SUCCESS;
} /* }}} */

void phpdbg_print_opline(zend_execute_data *execute_data, zend_bool ignore_flags TSRMLS_DC) /* {{{ */
{
    /* force out a line while stepping so the user knows what is happening */
    if (ignore_flags ||
        (!(PHPDBG_G(flags) & PHPDBG_IS_QUIET) ||
        (PHPDBG_G(flags) & PHPDBG_IS_STEPPING))) {

        zend_op *opline = execute_data->opline;

        printf(
            "%sOPLINE: %p:%s%s\n",
            PHPDBG_BOLD_LINE(TSRMLS_C),
            opline,
            phpdbg_decode_opcode(opline->opcode),
            PHPDBG_END_LINE(TSRMLS_C));
    }
} /* }}} */

void phpdbg_clean(zend_bool full TSRMLS_DC) /* {{{ */
{
    zend_objects_store_call_destructors(&EG(objects_store) TSRMLS_CC);

    /* this is implicitly required */
    if (PHPDBG_G(ops)) {
        destroy_op_array(
            PHPDBG_G(ops) TSRMLS_CC);
        efree(PHPDBG_G(ops));
        PHPDBG_G(ops) = NULL;
    }

    if (full) {
        zend_hash_reverse_apply(EG(function_table), (apply_func_t) clean_non_persistent_function_full TSRMLS_CC);
        zend_hash_reverse_apply(EG(class_table), (apply_func_t) clean_non_persistent_class_full TSRMLS_CC);
        zend_hash_reverse_apply(EG(zend_constants), (apply_func_t) clean_non_persistent_constant_full TSRMLS_CC);
        zend_hash_clean(&EG(included_files));
    }
} /* }}} */

void phpdbg_execute_ex(zend_execute_data *execute_data TSRMLS_DC) /* {{{ */
{
	zend_bool original_in_execution = EG(in_execution);

	EG(in_execution) = 1;

	if (0) {
zend_vm_enter:
		execute_data = zend_create_execute_data_from_op_array(EG(active_op_array), 1 TSRMLS_CC);
	}

	while (1) {
#ifdef ZEND_WIN32
		if (EG(timed_out)) {
			zend_timeout(0);
		}
#endif

        phpdbg_print_opline(
		    execute_data, 0 TSRMLS_CC);

        if ((PHPDBG_G(flags) & PHPDBG_HAS_FILE_BP)
            && phpdbg_find_breakpoint_file(execute_data->op_array TSRMLS_CC) == SUCCESS) {
            while (phpdbg_interactive(TSRMLS_C) != PHPDBG_NEXT) {
                if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
                    continue;
                }
            }
        }

        if ((PHPDBG_G(flags) & (PHPDBG_HAS_METHOD_BP|PHPDBG_HAS_SYM_BP))) {
            zend_execute_data *previous = execute_data->prev_execute_data;
            if (previous && previous != execute_data && previous->opline) {
                /* check we are the beginning of a function entry */
                if (execute_data->opline == EG(active_op_array)->opcodes) {
                    switch (previous->opline->opcode) {
                        case ZEND_DO_FCALL:
                        case ZEND_DO_FCALL_BY_NAME:
                        case ZEND_INIT_STATIC_METHOD_CALL: {
                            if (phpdbg_find_breakpoint_symbol(previous->function_state.function TSRMLS_CC) == SUCCESS) {
                                while (phpdbg_interactive(TSRMLS_C) != PHPDBG_NEXT) {
                                    if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
                                        continue;
                                    }
                                }
                            }
                        } break;
                    }
                }
            }
        }

        if ((PHPDBG_G(flags) & PHPDBG_HAS_OPLINE_BP)
            && phpdbg_find_breakpoint_opline(execute_data->opline TSRMLS_CC) == SUCCESS) {
            while (phpdbg_interactive(TSRMLS_C) != PHPDBG_NEXT) {
                if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
                    continue;
                }
            }
        }

        PHPDBG_G(vmret) = execute_data->opline->handler(execute_data TSRMLS_CC);

        if ((PHPDBG_G(flags) & PHPDBG_IS_STEPPING)) {
            while (phpdbg_interactive(TSRMLS_C) != PHPDBG_NEXT) {
                if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
                    continue;
                }
            }
        }

        if (PHPDBG_G(vmret) > 0) {
            switch (PHPDBG_G(vmret)) {
                case 1:
                    EG(in_execution) = original_in_execution;
                    return;
                case 2:
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
