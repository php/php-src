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

/* {{{ forward declarations */
static PHPDBG_COMMAND(exec);
static PHPDBG_COMMAND(compile);
static PHPDBG_COMMAND(step);
static PHPDBG_COMMAND(next);
static PHPDBG_COMMAND(run);
static PHPDBG_COMMAND(eval);
static PHPDBG_COMMAND(print);
static PHPDBG_COMMAND(break);
static PHPDBG_COMMAND(back);
static PHPDBG_COMMAND(list);
static PHPDBG_COMMAND(clean);
static PHPDBG_COMMAND(clear);
static PHPDBG_COMMAND(help);
static PHPDBG_COMMAND(quiet);
static PHPDBG_COMMAND(aliases);
static PHPDBG_COMMAND(quit); /* }}} */

/* {{{ command declarations */
static const phpdbg_command_t phpdbg_prompt_commands[] = {
	PHPDBG_COMMAND_EX_D(exec,       "set execution context", 'e'),
	PHPDBG_COMMAND_EX_D(compile,    "attempt to pre-compile execution context", 'c'),
	PHPDBG_COMMAND_EX_D(step,       "step through execution", 's'),
	PHPDBG_COMMAND_EX_D(next,       "continue execution", 'n'),
	PHPDBG_COMMAND_EX_D(run,        "attempt execution", 'r'),
	PHPDBG_COMMAND_EX_D(eval,       "evaluate some code", 'E'),
	PHPDBG_COMMAND_EX_D(print,      "print something", 'p'),
	PHPDBG_COMMAND_EX_D(break,      "set breakpoint", 'b'),
	PHPDBG_COMMAND_EX_D(back,       "show trace", 't'),
	PHPDBG_COMMAND_EX_D(list,       "list specified line or function", 'l'),
	PHPDBG_COMMAND_EX_D(clean,      "clean the execution environment", 'X'),
	PHPDBG_COMMAND_EX_D(clear,      "clear breakpoints", 'C'),
	PHPDBG_COMMAND_EX_D(help,       "show help menu", 'h'),
	PHPDBG_COMMAND_EX_D(quiet,      "silence some output", 'Q'),
	PHPDBG_COMMAND_EX_D(aliases,    "show alias list", 'a'),
	PHPDBG_COMMAND_EX_D(quit,       "exit phpdbg", 'q'),
	{NULL, 0, 0}
}; /* }}} */

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

void phpdbg_init(char *init_file, size_t init_file_len TSRMLS_DC) /* {{{ */
{
    if (init_file) {
        
        free(init_file);
    }
} /* }}} */

void phpdbg_welcome(zend_bool cleaning TSRMLS_DC) /* {{{ */
{
    /* print blurb */
    if (!cleaning) {
        phpdbg_notice("Welcome to phpdbg, the interactive PHP debugger, v%s",
            PHPDBG_VERSION);
        phpdbg_writeln("To get help using phpdbg type \"help\" and press enter");
        phpdbg_notice("Please report bugs to <%s>", PHPDBG_ISSUES);
    } else {
        phpdbg_notice("Clean Execution Environment");

        phpdbg_writeln("Classes\t\t\t%d", zend_hash_num_elements(EG(class_table)));
        phpdbg_writeln("Functions\t\t%d", zend_hash_num_elements(EG(function_table)));
        phpdbg_writeln("Constants\t\t%d", zend_hash_num_elements(EG(zend_constants)));
        phpdbg_writeln("Includes\t\t%d", zend_hash_num_elements(&EG(included_files)));
    }
} /* }}} */

static PHPDBG_COMMAND(exec) /* {{{ */
{
	if (expr_len == 0) {
		phpdbg_error("No expression provided");
		return SUCCESS;
	}
	if (PHPDBG_G(exec)) {
		phpdbg_notice("Unsetting old execution context: %s", PHPDBG_G(exec));
		efree(PHPDBG_G(exec));
		PHPDBG_G(exec) = NULL;
	}
	if (PHPDBG_G(ops)) {
		phpdbg_notice("Destroying compiled opcodes");
		phpdbg_clean(0 TSRMLS_CC);
	}

	PHPDBG_G(exec) = phpdbg_resolve_path(expr TSRMLS_CC);

	if (!PHPDBG_G(exec)) {
		phpdbg_error("Cannot get real file path");
		return FAILURE;
	}

	PHPDBG_G(exec_len) = strlen(PHPDBG_G(exec));

	phpdbg_notice("Set execution context: %s", PHPDBG_G(exec));
	return SUCCESS;
} /* }}} */

static inline int phpdbg_compile(TSRMLS_D) /* {{{ */
{
	zend_file_handle fh;

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

static PHPDBG_COMMAND(compile) /* {{{ */
{
	if (!PHPDBG_G(exec)) {
		phpdbg_error("No execution context");
		return FAILURE;
	}
	if (!EG(in_execution)) {
		if (PHPDBG_G(ops)) {
			phpdbg_error("Destroying previously compiled opcodes");
			phpdbg_clean(0 TSRMLS_CC);
		}
	}

	return phpdbg_compile(TSRMLS_C);
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
	zend_bool stepping = (PHPDBG_G(flags) & PHPDBG_IS_STEPPING);
	zval retval;

	if (expr_len == 0) {
		phpdbg_error("No expression provided!");
		return FAILURE;
	}

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

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(back) /* {{{ */
{
    zval zbacktrace;
	zval **tmp;
	HashPosition position;
	int i = 0, limit = 0;

	if (!EG(in_execution)) {
		phpdbg_error("Not executing!");
		return FAILURE;
	}

	limit = (expr != NULL) ? atoi(expr) : 0;

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
	phpdbg_param_t param;
    int type;
    
	if (expr_len == 0) {
		phpdbg_error("No expression found");
		return FAILURE;
	}

    /* allow advanced breakers to run */
    if (phpdbg_do_cmd(phpdbg_break_commands, (char*)expr, expr_len TSRMLS_CC) == SUCCESS) {
		return SUCCESS;
	}

	switch ((type=phpdbg_parse_param(expr, expr_len, &param TSRMLS_CC))) {
		case ADDR_PARAM:
			phpdbg_set_breakpoint_opline(param.addr TSRMLS_CC);
			break;
		case NUMERIC_PARAM:
			phpdbg_set_breakpoint_file(phpdbg_current_file(TSRMLS_C), param.num TSRMLS_CC);
			break;
		case METHOD_PARAM:
			phpdbg_set_breakpoint_method(param.method.class, param.method.name TSRMLS_CC);
			break;
		case FILE_PARAM:
			phpdbg_set_breakpoint_file(param.file.name, param.file.line TSRMLS_CC);
			break;
		case STR_PARAM:
			phpdbg_set_breakpoint_symbol(param.str TSRMLS_CC);
			break;
		default:
			break;
	}
	
	phpdbg_clear_param(type, &param TSRMLS_CC);

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(quit) /* {{{ */
{
    /* don't allow this to loop, ever ... */
    if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
    
        PHPDBG_G(flags) |= PHPDBG_IS_QUITTING;
	    zend_bailout();
    }

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(clean) /* {{{ */
{
    if (EG(in_execution)) {
        phpdbg_error("Cannot clean environment while executing");
        return FAILURE;
	}

	phpdbg_notice("Cleaning Execution Environment");

	phpdbg_writeln("Classes\t\t\t%d", zend_hash_num_elements(EG(class_table)));
	phpdbg_writeln("Functions\t\t%d", zend_hash_num_elements(EG(function_table)));
	phpdbg_writeln("Constants\t\t%d", zend_hash_num_elements(EG(zend_constants)));
	phpdbg_writeln("Includes\t\t%d", zend_hash_num_elements(&EG(included_files)));

	phpdbg_clean(1 TSRMLS_CC);

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

static PHPDBG_COMMAND(aliases) /* {{{ */
{
	const phpdbg_command_t *prompt_command = phpdbg_prompt_commands;

    phpdbg_notice("Welcome to phpdbg, the interactive PHP debugger, v%s",
		PHPDBG_VERSION);

    phpdbg_notice("Aliases");

	while (prompt_command && prompt_command->name) {
		if (prompt_command->alias) {
			phpdbg_writeln("\t%c -> %s\t%s", prompt_command->alias,
				prompt_command->name, prompt_command->tip);
		}
		++prompt_command;
	}

	phpdbg_notice("Please report bugs to <%s>", PHPDBG_ISSUES);
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
			phpdbg_writeln(
			    "\t%s\t%s", prompt_command->name, prompt_command->tip);
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
    phpdbg_param_t param;
    int type = 0;
    
    /* allow advanced listers to run */
    if (phpdbg_do_cmd(phpdbg_list_commands, (char*)expr, expr_len TSRMLS_CC) == SUCCESS) {
		return SUCCESS;
	}

	phpdbg_list_dispatch(
	    phpdbg_parse_param(expr, expr_len, &param TSRMLS_CC), 
	    &param TSRMLS_CC);
	
	phpdbg_clear_param(type, &param TSRMLS_CC);

	return SUCCESS;
} /* }}} */

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
		if ((command->name_len == expr_len
			    && memcmp(cmd, command->name, expr_len) == 0)
		   || ((expr_len == 1) && (command->alias && command->alias == cmd_line[0]))) {

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
		            
#ifdef HAVE_LIBREADLINE
                    if (cmd) {
                        free(cmd);
                    }
#endif
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
    /* this is implicitly required */
    if (PHPDBG_G(ops)) {
        destroy_op_array(PHPDBG_G(ops) TSRMLS_CC);
        efree(PHPDBG_G(ops));
        PHPDBG_G(ops) = NULL;
    }

    if (full) 
    {
        PHPDBG_G(flags) |= PHPDBG_IS_CLEANING;
        
        zend_bailout();
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

        /* allow conditional breakpoints to access the vm uninterrupted */
        if (!(PHPDBG_G(flags) & PHPDBG_IN_COND_BP)) {

            /* not while in conditionals */
            phpdbg_print_opline(
		        execute_data, 0 TSRMLS_CC);

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
        
        if (!(PHPDBG_G(flags) & PHPDBG_IN_COND_BP)) {
            if ((PHPDBG_G(flags) & PHPDBG_IS_STEPPING)) {
                DO_INTERACTIVE();
            }
        }

next:
        PHPDBG_G(vmret) = execute_data->opline->handler(execute_data TSRMLS_CC);

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
