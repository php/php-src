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
#include "phpdbg_bp.h"
#include "phpdbg_opcode.h"
#include "phpdbg_list.h"

static const phpdbg_command_t phpdbg_prompt_commands[];

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

static PHPDBG_COMMAND(exec) /* {{{ */
{
	if (PHPDBG_G(exec)) {
		printf("[Unsetting old execution context: %s]\n", PHPDBG_G(exec));
		efree(PHPDBG_G(exec));
		PHPDBG_G(exec) = NULL;
	}

	if (PHPDBG_G(ops)) {
		printf("[Destroying compiled opcodes]\n");
		destroy_op_array(PHPDBG_G(ops) TSRMLS_CC);
		efree(PHPDBG_G(ops));
		PHPDBG_G(ops) = NULL;
	}

	PHPDBG_G(exec) = estrndup(expr, PHPDBG_G(exec_len) = expr_len);

	printf("[Set execution context: %s]\n", PHPDBG_G(exec));

	return SUCCESS;
} /* }}} */

static inline int phpdbg_compile(TSRMLS_D) /* {{{ */
{
	zend_file_handle fh;

	if (!EG(in_execution)) {
	    printf("[Attempting compilation of %s]\n", PHPDBG_G(exec));

	    if (php_stream_open_for_zend_ex(PHPDBG_G(exec), &fh,
		        USE_PATH|STREAM_OPEN_FOR_INCLUDE TSRMLS_CC) == SUCCESS) {
		    PHPDBG_G(ops) = zend_compile_file(
		        &fh, ZEND_INCLUDE TSRMLS_CC);
		    zend_destroy_file_handle(&fh TSRMLS_CC);
		    printf("[Success]\n");
		    return SUCCESS;
        } else {
            printf("[Could not open file %s]\n", PHPDBG_G(exec));
        }
	} else {
	    printf("[Cannot compile while in execution]\n");
	}

	return FAILURE;
} /* }}} */

static PHPDBG_COMMAND(compile) /* {{{ */
{
	if (PHPDBG_G(exec)) {
		if (!EG(in_execution)) {
		    if (PHPDBG_G(ops)) {
			    printf("[Destroying previously compiled opcodes]\n");
			    destroy_op_array(PHPDBG_G(ops) TSRMLS_CC);
			    efree(PHPDBG_G(ops));
			    PHPDBG_G(ops)=NULL;
		    }
		}

		return phpdbg_compile(TSRMLS_C);
	} else {
		printf("[No execution context]\n");
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
	    "[Stepping %s]\n", (PHPDBG_G(flags) & PHPDBG_IS_STEPPING) ? "on" : "off");
	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(next) /* {{{ */
{
	return PHPDBG_NEXT;
} /* }}} */

static PHPDBG_COMMAND(run) /* {{{ */
{
    if (EG(in_execution)) {
        printf("[Cannot start another execution while one is in progress]\n");
        return FAILURE;
    }

	if (PHPDBG_G(ops) || PHPDBG_G(exec)) {
		if (!PHPDBG_G(ops)) {
			if (phpdbg_compile(TSRMLS_C) == FAILURE) {
				printf("[Failed to compile %s, cannot run]\n", PHPDBG_G(exec));
				return FAILURE;
			}
		}

		EG(active_op_array) = PHPDBG_G(ops);
		EG(return_value_ptr_ptr) = &PHPDBG_G(retval);

		zend_try {
			zend_execute(EG(active_op_array) TSRMLS_CC);
		} zend_catch {
			if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
				printf("[Caught excetion in VM]\n");
				return FAILURE;
			} else return SUCCESS;
		} zend_end_try();

		return SUCCESS;
	} else {
		printf("[Nothing to execute !]\n");
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
			printf("\n");
		}
		
		/* switch stepping back on */
		if (stepping) {
		    PHPDBG_G(flags) |= PHPDBG_IS_STEPPING;
		}
	} else {
		printf("[No expression provided !]\n");
		return FAILURE;
	}

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(back) /* {{{ */
{
	if (!EG(in_execution)) {
		printf("[Not executing !]\n");
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
	if (expr_len) {
		printf("%s\n", expr);
		return SUCCESS;
	}
#ifdef HAVE_READLINE_H
    printf("HAVE READLINE\n");
#endif

    printf("--------------------------------------\n");
	printf("Execution Context Information:\n");
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
        for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp_files), &position);
             zend_hash_get_current_data_ex(&PHPDBG_G(bp_files), (void**) &points, &position) == SUCCESS;
             zend_hash_move_forward_ex(&PHPDBG_G(bp_files), &position)) {
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
        for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp_symbols), &position);
             zend_hash_get_current_data_ex(&PHPDBG_G(bp_symbols), (void**) &points, &position) == SUCCESS;
             zend_hash_move_forward_ex(&PHPDBG_G(bp_symbols), &position)) {
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
        for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp_oplines), &position);
             zend_hash_get_current_data_ex(&PHPDBG_G(bp_oplines), (void**) &brake, &position) == SUCCESS;
             zend_hash_move_forward_ex(&PHPDBG_G(bp_oplines), &position)) {
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
    
    if (!expr_len) {
        printf(
            "[No expression found]\n");
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
			        printf("[Failed to expand path %s]\n", path);
			        return FAILURE;
		        }

		        phpdbg_set_breakpoint_file(resolved_name, line_num TSRMLS_CC);
		    } else {
		        printf("[No line specified in expression %s]\n", expr);
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
		        printf("[No function found in method expression %s]\n", expr);
		        return FAILURE;
		    }
		}
	} else {
		if (expr_len > 2 && expr[0] == '0' && expr[1] == 'x') {
		    phpdbg_set_breakpoint_opline(expr TSRMLS_CC);
		} else {
		    char name[200];
		    size_t name_len = strlen(expr);

		    if (name_len) {
		        name_len = MIN(name_len, 200);
		        memcpy(name, expr, name_len);
		        name[name_len] = 0;

		        phpdbg_set_breakpoint_symbol(name TSRMLS_CC);
		    } else {
		        printf("[Malformed break command found]\n");
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

        zend_hash_reverse_apply(EG(function_table), (apply_func_t) clean_non_persistent_function_full TSRMLS_CC);
        zend_hash_reverse_apply(EG(class_table), (apply_func_t) clean_non_persistent_class_full TSRMLS_CC);
        zend_hash_reverse_apply(EG(zend_constants), (apply_func_t) clean_non_persistent_constant_full TSRMLS_CC);
        zend_hash_clean(&EG(included_files));

        printf("[Clean Environment:]\n");
        printf("[\tClasses: %d]\n", zend_hash_num_elements(EG(class_table)));
        printf("[\tFunctions: %d]\n", zend_hash_num_elements(EG(function_table)));
        printf("[\tConstants: %d]\n", zend_hash_num_elements(EG(zend_constants)));
        printf("[\tIncluded: %d]\n", zend_hash_num_elements(&EG(included_files)));
    } else {
        printf(
            "[Cannot clean environment while executing]\n");
        return FAILURE;
    }

    return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(clear) /* {{{ */
{
    printf("[Clearing Breakpoints:]\n");
    printf("[\tFile\t%d]\n", zend_hash_num_elements(&PHPDBG_G(bp_files)));
    printf("[\tSymbols\t%d]\n", zend_hash_num_elements(&PHPDBG_G(bp_symbols)));
    printf("[\tOplines\t%d]\n", zend_hash_num_elements(&PHPDBG_G(bp_oplines)));
    printf("[\tMethods\t%d]\n", zend_hash_num_elements(&PHPDBG_G(bp_methods)));
    
    phpdbg_clear_breakpoints(TSRMLS_C);

    return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(help) /* {{{ */
{
	printf("[Welcome to phpdbg, the interactive PHP debugger.]\n");

	if (!expr_len) {
		const phpdbg_command_t *prompt_command = phpdbg_prompt_commands;
		const phpdbg_command_t *help_command = phpdbg_help_commands;

		printf("To get help regarding a specific command type \"help command\"\n");

		printf("Commands:\n");
		while (prompt_command && prompt_command->name) {
			printf("\t%s\t%s\n", prompt_command->name, prompt_command->tip);
			++prompt_command;
		}

		printf("Helpers Loaded:\n");
		while (help_command && help_command->name) {
			printf("\t%s\t%s\n", help_command->name, help_command->tip);
			++help_command;
		}
	} else {
		if (phpdbg_do_cmd(phpdbg_help_commands, (char*)expr, expr_len TSRMLS_CC) == FAILURE) {
			printf("failed to find help command: %s\n", expr);
		}
	}
	printf("[Please report bugs to <https://github.com/krakjoe/phpdbg/issues>]\n");

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(quiet) { /* {{{ */
    if (atoi(expr)) {
        PHPDBG_G(flags) |= PHPDBG_IS_QUIET;
    } else {
        PHPDBG_G(flags) &= ~PHPDBG_IS_QUIET;
    }

    printf(
        "[Quietness %s]\n", (PHPDBG_G(flags) & PHPDBG_IS_QUIET) ? "enabled" : "disabled");

    return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(list) /* {{{ */
{
	long offset = 0, count = strtol(expr, NULL, 0);
	const char *filename = PHPDBG_G(exec);

	if (zend_is_executing(TSRMLS_C)) {
		filename = zend_get_executed_filename(TSRMLS_C);
		offset = zend_get_executed_lineno(TSRMLS_C);
	} else if (!filename) {
		printf("[No file to list]\n");
		return SUCCESS;
	}

	phpdbg_list_file(filename, count, offset);

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
	PHPDBG_COMMAND_D(list,      "list specified line"),
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
	char cmd[PHPDBG_MAX_CMD];

	printf("phpdbg> ");

	while (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING) &&
	       fgets(cmd, PHPDBG_MAX_CMD, stdin) != NULL) {
		size_t cmd_len = strlen(cmd) - 1;

		if (cmd[cmd_len] == '\n') {
			cmd[cmd_len] = 0;
		}

		if (cmd_len) {
		    switch (phpdbg_do_cmd(phpdbg_prompt_commands, cmd, cmd_len TSRMLS_CC)) {
		        case FAILURE:
		            if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
		                printf(
		                    "[Failed to execute %s !]\n", cmd);
		            }
		        break;

		        case PHPDBG_NEXT: {
		            if (!EG(in_execution)) {
		                printf("[Not running]\n");
		            }
		            return PHPDBG_NEXT;
		        }
		            

		    }
		} else if (PHPDBG_G(last)) {
		    PHPDBG_G(last)->handler(
		        PHPDBG_G(last_params), PHPDBG_G(last_params_len) TSRMLS_CC);
		}

		if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
		    printf("phpdbg> ");
		}

	}

	return SUCCESS;
} /* }}} */

static void phpdbg_print_opline(zend_execute_data *execute_data TSRMLS_DC) /* {{{ */
{
    /* force out a line while stepping so the user knows what is happening */
    if (!(PHPDBG_G(flags) & PHPDBG_IS_QUIET) || (PHPDBG_G(flags) & PHPDBG_IS_STEPPING)) {
        zend_op *opline = execute_data->opline;

        printf(
            "[OPLINE: %p:%s]\n", opline, phpdbg_decode_opcode(opline->opcode));
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
		    execute_data TSRMLS_CC);

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
