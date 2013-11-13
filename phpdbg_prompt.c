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
#include "phpdbg_break.h"
#include "phpdbg_bp.h"
#include "phpdbg_opcode.h"
#include "phpdbg_list.h"
#include "phpdbg_utils.h"

static const phpdbg_command_t phpdbg_prompt_commands[];

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

static PHPDBG_COMMAND(exec) /* {{{ */
{
	if (PHPDBG_G(exec)) {
		phpdbg_notice("Unsetting old execution context: %s", PHPDBG_G(exec));
		efree(PHPDBG_G(exec));
		PHPDBG_G(exec) = NULL;
	}

	if (PHPDBG_G(ops)) {
		phpdbg_notice("Destroying compiled opcodes");
   		phpdbg_clean(0 TSRMLS_CC);
	}

	PHPDBG_G(exec) = estrndup(expr, PHPDBG_G(exec_len) = expr_len);

	phpdbg_notice("Set execution context: %s", PHPDBG_G(exec));

	return SUCCESS;
} /* }}} */

static inline int phpdbg_compile(TSRMLS_D) /* {{{ */
{
	zend_file_handle fh;

	if (!EG(in_execution)) {
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
	} else {
		phpdbg_error("Cannot compile while in execution");
	}

	return FAILURE;
} /* }}} */

static PHPDBG_COMMAND(compile) /* {{{ */
{
	if (PHPDBG_G(exec)) {
		if (!EG(in_execution)) {
		    if (PHPDBG_G(ops)) {
				phpdbg_error("Destroying previously compiled opcodes");
			    phpdbg_clean(0 TSRMLS_CC);
		    }
		}

		return phpdbg_compile(TSRMLS_C);
	} else {
		phpdbg_error("No execution context");
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

	phpdbg_notice("Stepping %s",
	    (PHPDBG_G(flags) & PHPDBG_IS_STEPPING) ? "on" : "off");

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(next) /* {{{ */
{
	return PHPDBG_NEXT;
} /* }}} */

static PHPDBG_COMMAND(run) /* {{{ */
{
    if (EG(in_execution)) {
		phpdbg_error("Cannot start another execution while one is in progress");
        return FAILURE;
    }

	if (PHPDBG_G(ops) || PHPDBG_G(exec)) {
		zend_op **orig_opline = EG(opline_ptr);
		zend_op_array *orig_op_array = EG(active_op_array);
		zval **orig_retval_ptr = EG(return_value_ptr_ptr);
		
		if (!PHPDBG_G(ops)) {
			if (phpdbg_compile(TSRMLS_C) == FAILURE) {
				phpdbg_error("Failed to compile %s, cannot run", PHPDBG_G(exec));
				return FAILURE;
			}
		}

		EG(active_op_array) = PHPDBG_G(ops);
		EG(return_value_ptr_ptr) = &PHPDBG_G(retval);
        if (!EG(active_symbol_table)) {
            zend_rebuild_symbol_table(TSRMLS_C);
        }
        
		zend_try {
			zend_execute(
			    EG(active_op_array) TSRMLS_CC);
		} zend_catch {
		    EG(active_op_array) = orig_op_array;
		    EG(opline_ptr) = orig_opline;
		    EG(return_value_ptr_ptr) = orig_retval_ptr;
		    
			if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
				phpdbg_error("Caught excetion in VM");
				return FAILURE;
			} else return SUCCESS;
		} zend_end_try();

        EG(active_op_array) = orig_op_array;
	    EG(opline_ptr) = orig_opline;
	    EG(return_value_ptr_ptr) = orig_retval_ptr;
        
		return SUCCESS;
	} else {
		phpdbg_error("Nothing to execute!");
		return FAILURE;
	}
} /* }}} */

static PHPDBG_COMMAND(eval) /* {{{ */
{
	zval retval;

	if (expr_len) {
	    zend_bool stepping = (PHPDBG_G(flags) & PHPDBG_IS_STEPPING);

	    PHPDBG_G(flags) &= ~ PHPDBG_IS_STEPPING;
	    
	    /* disable stepping while eval() in progress */
        PHPDBG_G(flags) |= PHPDBG_IN_EVAL;
		if (zend_eval_stringl((char*)expr, expr_len,
			&retval, "eval()'d code" TSRMLS_CC) == SUCCESS) {
			zend_print_zval_r(&retval, 0 TSRMLS_CC);
			zval_dtor(&retval);
			phpdbg_writeln(EMPTY);
		}
        PHPDBG_G(flags) &= ~PHPDBG_IN_EVAL;
        
		/* switch stepping back on */
		if (stepping) {
		    PHPDBG_G(flags) |= PHPDBG_IS_STEPPING;
		}
	} else {
		phpdbg_error("No expression provided!");
		return FAILURE;
	}

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(back) /* {{{ */
{
	if (!EG(in_execution)) {
		phpdbg_error("Not executing!");
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
			phpdbg_writeln(",");
		}
		zend_print_flat_zval_r(*tmp TSRMLS_CC);
	}

	phpdbg_writeln(EMPTY);
	zval_dtor(&zbacktrace);

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(print) /* {{{ */
{
	if (expr && expr_len > 0L) {
		if (phpdbg_do_cmd(phpdbg_print_commands, (char*)expr, expr_len TSRMLS_CC) == FAILURE) {
			phpdbg_error("Failed to find print command %s", expr);
		}
		return SUCCESS;
	}

    phpdbg_writeln(SEPARATE);
	phpdbg_notice("Execution Context Information");
#ifdef HAVE_LIBREADLINE
    phpdbg_writeln("Readline\tyes");
#else
    phpdbg_writeln("Readline\tno");
#endif
	phpdbg_writeln("Exec\t\t%s", PHPDBG_G(exec) ? PHPDBG_G(exec) : "none");
	phpdbg_writeln("Compiled\t%s", PHPDBG_G(ops) ? "yes" : "no");
	phpdbg_writeln("Stepping\t%s", (PHPDBG_G(flags) & PHPDBG_IS_STEPPING) ? "on" : "off");
    phpdbg_writeln("Quietness\t%s", (PHPDBG_G(flags) & PHPDBG_IS_QUIET) ? "on" : "off");

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

    phpdbg_print_breakpoints(PHPDBG_BREAK_FILE TSRMLS_CC);
    phpdbg_print_breakpoints(PHPDBG_BREAK_SYM TSRMLS_CC);
    phpdbg_print_breakpoints(PHPDBG_BREAK_METHOD TSRMLS_CC);
    phpdbg_print_breakpoints(PHPDBG_BREAK_OPLINE TSRMLS_CC);

    phpdbg_writeln(SEPARATE);

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(break) /* {{{ */
{
	char *line_pos;
	
	if (expr_len > 0L) {
	    /* allow advanced breakers to run */
	    if (phpdbg_do_cmd(phpdbg_break_commands, (char*)expr, expr_len TSRMLS_CC) == SUCCESS) {
			return SUCCESS;
		}
	}

    if (expr_len <= 0L) {
        phpdbg_error("No expression found");
        return FAILURE;
    }

    line_pos = strchr(expr, ':');

	if (line_pos) {
		char *class;
		char *func;

		/* break class::method */
		if (phpdbg_is_class_method(expr, expr_len, &class, &func)) {
			phpdbg_set_breakpoint_method(class, func TSRMLS_CC);
		} else {
			/* break file:line */
	        char path[MAXPATHLEN], resolved_name[MAXPATHLEN];
		    long line_num = strtol(line_pos+1, NULL, 0);

		    if (line_num) {
		        memcpy(path, expr, line_pos - expr);
		        path[line_pos - expr] = 0;

		        if (expand_filepath(path, resolved_name TSRMLS_CC) == NULL) {
			        phpdbg_error("Failed to expand path %s", path);
			        return FAILURE;
		        }

		        phpdbg_set_breakpoint_file(resolved_name, line_num TSRMLS_CC);
		    } else {
		        phpdbg_error("No line specified in expression %s", expr);
		        return FAILURE;
		    }
	    }
	} else {
		/* break 0xc0ffee */
		if (phpdbg_is_addr(expr)) {
			zend_ulong opline = strtoul(expr, 0, 16);

			phpdbg_set_breakpoint_opline(opline TSRMLS_CC);
		} else if (phpdbg_is_numeric(expr)) {
			/* break 1337 */
			const char *filename = zend_get_executed_filename(TSRMLS_C);
			long line_num = strtol(expr, NULL, 0);

			if (!filename) {
				phpdbg_error("No file context found");
				return FAILURE;
			}

			phpdbg_set_breakpoint_file(filename, line_num TSRMLS_CC);
		} else {
			/* break symbol */
			char name[200];
			size_t name_len = strlen(expr);

			if (name_len) {
				name_len = MIN(name_len, 200);
				memcpy(name, expr, name_len);
				name[name_len] = 0;

				phpdbg_set_breakpoint_symbol(name TSRMLS_CC);
			} else {
				phpdbg_error("Malformed break command found");
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
        phpdbg_notice("Cleaning Execution Environment");
        
        phpdbg_writeln("Classes\t\t\t%d", zend_hash_num_elements(EG(class_table)));
        phpdbg_writeln("Functions\t\t%d", zend_hash_num_elements(EG(function_table)));
        phpdbg_writeln("Constants\t\t%d", zend_hash_num_elements(EG(zend_constants)));
        phpdbg_writeln("Includes\t\t%d", zend_hash_num_elements(&EG(included_files)));   

        phpdbg_clean(1 TSRMLS_CC);

        phpdbg_notice("Clean Execution Environment");
        
        phpdbg_writeln("Classes\t\t\t%d", zend_hash_num_elements(EG(class_table)));
        phpdbg_writeln("Functions\t\t%d", zend_hash_num_elements(EG(function_table)));
        phpdbg_writeln("Constants\t\t%d", zend_hash_num_elements(EG(zend_constants)));
        phpdbg_writeln("Includes\t\t%d", zend_hash_num_elements(&EG(included_files))); 
    } else {
        phpdbg_error("Cannot clean environment while executing");
        return FAILURE;
    }

    return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(clear) /* {{{ */
{
    phpdbg_notice("Clearing Breakpoints");
    
    phpdbg_writeln("File\t\t\t%d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE]));
    phpdbg_writeln("Functions\t\t%d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM]));
    phpdbg_writeln("Methods\t\t\t%d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD]));
    phpdbg_writeln("Oplines\t\t\t%d", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]));
    
    phpdbg_clear_breakpoints(TSRMLS_C);

    return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(help) /* {{{ */
{
	phpdbg_notice("Welcome to phpdbg, the interactive PHP debugger, v%s",
		PHPDBG_VERSION);

	if (expr_len > 0L) {
		if (phpdbg_do_cmd(phpdbg_help_commands, (char*)expr, expr_len TSRMLS_CC) == FAILURE) {
			phpdbg_error("Failed to find help command: %s", expr);
		}
	} else {
		const phpdbg_command_t *prompt_command = phpdbg_prompt_commands;
		const phpdbg_command_t *help_command = phpdbg_help_commands;

		phpdbg_writeln("To get help regarding a specific command type \"help command\"");

		phpdbg_notice("Commands");

		while (prompt_command && prompt_command->name) {
			phpdbg_writeln("\t%s\t%s", prompt_command->name, prompt_command->tip);
			++prompt_command;
		}

		phpdbg_notice("Helpers Loaded");

		while (help_command && help_command->name) {
			phpdbg_writeln("\t%s\t%s", help_command->name, help_command->tip);
			++help_command;
		}
	}

	phpdbg_notice("Please report bugs to <%s>", PHPDBG_ISSUES);

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(quiet) { /* {{{ */
    if (atoi(expr)) {
        PHPDBG_G(flags) |= PHPDBG_IS_QUIET;
    } else {
        PHPDBG_G(flags) &= ~PHPDBG_IS_QUIET;
    }

	phpdbg_notice("Quietness %s",
        (PHPDBG_G(flags) & PHPDBG_IS_QUIET) ? "enabled" : "disabled");

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
			phpdbg_error("No file to list");
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
               phpdbg_error("No active class");
               return FAILURE;
           }
        } else if (!EG(function_table)) {
			phpdbg_error("No function table loaded");
			return SUCCESS;
		} else {
		    func_table = EG(function_table);
		}

		if (zend_hash_find(func_table, func_name, func_name_len+1,
			(void**)&fbc) == SUCCESS) {
			phpdbg_list_function(fbc TSRMLS_CC);
		} else {
			phpdbg_error("Function %s not found", func_name);
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
	char *expr = NULL;
#ifndef _WIN32
	const char *cmd = strtok_r(cmd_line, " ", &expr);
#else
	const char *cmd = strtok_s(cmd_line, " ", &expr);
#endif
	size_t expr_len = (cmd != NULL) ? strlen(cmd) : 0;
    
	while (command && command->name && command->handler) {
		if (command->name_len == expr_len
			    && memcmp(cmd, command->name, expr_len) == 0) {
			
			PHPDBG_G(last) = (phpdbg_command_t*) command;
			PHPDBG_G(last_params) = expr;
			PHPDBG_G(last_params_len) = (cmd_len - expr_len) ?
			                                (((cmd_len - expr_len) - sizeof(" "))+1) : 0;
			
			return command->handler(
			    PHPDBG_G(last_params), PHPDBG_G(last_params_len) TSRMLS_CC);
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

	while (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING) &&
	        phpdbg_write(PROMPT) &&
	       (fgets(cmd, PHPDBG_MAX_CMD, stdin) != NULL)) {
	    cmd_len = strlen(cmd) - 1;
#else
    char *cmd = NULL;

    while (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
        cmd = readline(PROMPT);

        if (cmd) {
            cmd_len = strlen(cmd);
        } else cmd_len = 0L;
#endif

		/* trim space from end of input */
		while (*cmd && isspace(cmd[cmd_len-1]))
		    cmd_len--;

		/* ensure string is null terminated */
		cmd[cmd_len] = '\0';

		if (*cmd && cmd_len > 0L) {
#ifdef HAVE_LIBREADLINE
            add_history(cmd);
#endif

		    switch (phpdbg_do_cmd(phpdbg_prompt_commands, cmd, cmd_len TSRMLS_CC)) {
		        case FAILURE:
		            if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
						phpdbg_error("Failed to execute %s!", cmd);
		            }
		        break;

		        case PHPDBG_NEXT: {
		            if (!EG(in_execution)) {
						phpdbg_error("Not running");
		            }
		            return PHPDBG_NEXT;
		        }
		    }
		    
#ifdef HAVE_LIBREADLINE
            if (cmd) {
                free(cmd);
                cmd = NULL;
            }
#endif
		} else if (PHPDBG_G(last)) {
		    PHPDBG_G(last)->handler(
		        PHPDBG_G(last_params), PHPDBG_G(last_params_len) TSRMLS_CC);
		}
	}
	
#ifdef HAVE_LIBREADLINE
    if (cmd) {
        free(cmd);
    }
#endif

	return SUCCESS;
} /* }}} */

void phpdbg_print_opline(zend_execute_data *execute_data, zend_bool ignore_flags TSRMLS_DC) /* {{{ */
{
    /* force out a line while stepping so the user knows what is happening */
    if (ignore_flags ||
        (!(PHPDBG_G(flags) & PHPDBG_IS_QUIET) ||
        (PHPDBG_G(flags) & PHPDBG_IS_STEPPING))) {

        zend_op *opline = execute_data->opline;
        /* output line info */
		phpdbg_notice("#%lu %p %s %s",
           opline->lineno,
           opline, phpdbg_decode_opcode(opline->opcode), 
           execute_data->op_array->filename ? execute_data->op_array->filename : "unknown");
    }
} /* }}} */

void phpdbg_clean(zend_bool full TSRMLS_DC) /* {{{ */
{
    zend_objects_store_call_destructors(&EG(objects_store) TSRMLS_CC);

    /* this is implicitly required */
    if (PHPDBG_G(ops)) {
        destroy_op_array(PHPDBG_G(ops) TSRMLS_CC);
        efree(PHPDBG_G(ops));
        PHPDBG_G(ops) = NULL;
    }

    if (full) {
        zend_hash_reverse_apply(EG(function_table),
			(apply_func_t) clean_non_persistent_function_full TSRMLS_CC);
        zend_hash_reverse_apply(EG(class_table),
			(apply_func_t) clean_non_persistent_class_full TSRMLS_CC);
        zend_hash_reverse_apply(EG(zend_constants),
			(apply_func_t) clean_non_persistent_constant_full TSRMLS_CC);
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

#define DO_INTERACTIVE() do {\
    switch (phpdbg_interactive(TSRMLS_C)) {\
        case PHPDBG_NEXT:\
            goto next;\
    }\
} while(!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING))

        phpdbg_print_opline(
		    execute_data, 0 TSRMLS_CC);

        /* allow conditional breakpoints to access the vm uninterrupted */
        if (!(PHPDBG_G(flags) & PHPDBG_IN_COND_BP)) {
            
            /* conditions cannot be executed by eval()'d code */
            if (!(PHPDBG_G(flags) & PHPDBG_IN_EVAL) 
                && (PHPDBG_G(flags) & PHPDBG_HAS_COND_BP)
                && phpdbg_find_conditional_breakpoint(TSRMLS_C) == SUCCESS) {
                DO_INTERACTIVE();
            }

            if ((PHPDBG_G(flags) & PHPDBG_HAS_FILE_BP)
                && phpdbg_find_breakpoint_file(execute_data->op_array TSRMLS_CC) == SUCCESS) {
                DO_INTERACTIVE();
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
                                    DO_INTERACTIVE();
                                }
                            } break;
                        }
                    }
                }
            }

            if ((PHPDBG_G(flags) & PHPDBG_HAS_OPLINE_BP)
                && phpdbg_find_breakpoint_opline(execute_data->opline TSRMLS_CC) == SUCCESS) {
                DO_INTERACTIVE();
            }
        }

next:
        PHPDBG_G(vmret) = execute_data->opline->handler(execute_data TSRMLS_CC);

        if (!(PHPDBG_G(flags) & PHPDBG_IN_COND_BP)) {
            if ((PHPDBG_G(flags) & PHPDBG_IS_STEPPING)) {
                DO_INTERACTIVE();
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
