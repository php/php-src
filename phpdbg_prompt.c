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

static const phpdbg_command_t phpdbg_prompt_commands[];

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

static PHPDBG_COMMAND(exec) { /* {{{ */
  if (PHPDBG_G(exec)) {
    printf(
      "Unsetting old execution context: %s\n", PHPDBG_G(exec));
    efree(PHPDBG_G(exec));
    PHPDBG_G(exec) = NULL;
  }

  if (PHPDBG_G(ops)) {
    printf(
      "Destroying compiled opcodes\n");
    destroy_op_array(PHPDBG_G(ops) TSRMLS_CC);
    efree(PHPDBG_G(ops));
    PHPDBG_G(ops) = NULL;
  }

  PHPDBG_G(exec) = estrndup(
    expr, PHPDBG_G(exec_len)=expr_len);

  printf(
    "Set execution context: %s\n", PHPDBG_G(exec));

  return SUCCESS;
} /* }}} */

static inline int phpdbg_compile(TSRMLS_D) {
    zend_file_handle fh;

    printf("Attempting compilation of %s\n", PHPDBG_G(exec));
    if (php_stream_open_for_zend_ex(PHPDBG_G(exec), &fh, USE_PATH|STREAM_OPEN_FOR_INCLUDE TSRMLS_CC) == SUCCESS) {
        PHPDBG_G(ops) = zend_compile_file(
            &fh, ZEND_INCLUDE TSRMLS_CC);
        zend_destroy_file_handle(&fh TSRMLS_CC);
        printf("Success\n");
        return SUCCESS;
    } else {
        printf("Could not open file %s\n", PHPDBG_G(exec));
        return FAILURE;
    }
}

static PHPDBG_COMMAND(compile) { /* {{{ */
  if (PHPDBG_G(exec)) {

    if (PHPDBG_G(ops)) {
      printf("Destroying compiled opcodes\n");
      destroy_op_array(PHPDBG_G(ops) TSRMLS_CC);
      efree(PHPDBG_G(ops));
    }

    return phpdbg_compile(TSRMLS_C);
  } else {
    printf("No execution context\n");
    return FAILURE;
  }
} /* }}} */

static PHPDBG_COMMAND(run) { /* {{{ */
    if (PHPDBG_G(ops) || PHPDBG_G(exec)) {
        if (!PHPDBG_G(ops)) {
            if (phpdbg_compile(TSRMLS_C) == FAILURE) {
                printf("Failed to compile %s, cannot run\n", PHPDBG_G(exec));
                return FAILURE;
            }
        }

        EG(active_op_array) = PHPDBG_G(ops);
        EG(return_value_ptr_ptr) = &PHPDBG_G(retval);

        zend_try {
            zend_execute(EG(active_op_array) TSRMLS_CC);
        } zend_catch {
            printf("Caught excetion in VM\n");
            return FAILURE;
        } zend_end_try();

        return SUCCESS;
    } else {
        printf("Nothing to execute !");
        return FAILURE;
    }
} /* }}} */

static PHPDBG_COMMAND(print) { /* {{{ */
  if (!expr_len) {
    printf("Showing Execution Context Information:\n");
    printf("Exec\t\t%s\n", PHPDBG_G(exec) ? PHPDBG_G(exec) : "none");
    printf("Compiled\t%s\n", PHPDBG_G(ops) ? "yes" : "no");
    if (PHPDBG_G(ops)) {
      printf("Opcodes\t\t%d\n", PHPDBG_G(ops)->last-1);
      printf("Variables\t%d\n", PHPDBG_G(ops)->last_var-1);
    }
  } else {
    printf(
      "%s\n", expr);
  }

  return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(break) /* {{{ */
{
	const char *line_pos = zend_memrchr(expr, ':', expr_len);

	if (line_pos) {
		long line_num = strtol(line_pos+1, NULL, 0);
		phpdbg_breakfile_t new_break;
		zend_llist break_files;

		new_break.filename = estrndup(expr, line_pos - expr);
		new_break.line = line_num;

		PHPDBG_G(has_breakpoint) = 1;

		if (zend_hash_find(&PHPDBG_G(break_files), new_break.filename, line_pos - expr, &break_files) == FAILURE) {
			zend_llist_init(&break_files, sizeof(phpdbg_breakfile_t), NULL, 0);
		}
		zend_llist_add_element(&break_files, &new_break);
	}

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(quit) /* {{{ */
{
	zend_bailout();

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(help) /* {{{ */
{
  printf("Welcome to phpdbg, the interactive PHP debugger.\n");
  if (!expr_len) {
    printf("To get help regarding a specific command type \"help command\"\n");
    printf("Commands:\n");
    {
      const phpdbg_command_t *command = phpdbg_prompt_commands;
      while (command && command->name) {
        printf(
          "\t%s\t%s\n", command->name, command->tip);
        command++;
      }
    }
    printf("Helpers Loaded:\n");
    {
      const phpdbg_command_t *command = phpdbg_help_commands;
      while (command && command->name) {
        printf(
          "\t%s\t%s\n", command->name, command->tip);
        command++;
      }
    }
  } else {
    if (phpdbg_do_cmd(phpdbg_help_commands, expr, expr_len TSRMLS_CC) == FAILURE) {
      printf("failed to find help command: %s\n", expr);
    }
  }
  printf("Please report bugs to <http://theman.in/themoon>\n");

  return SUCCESS;
} /* }}} */

static const phpdbg_command_t phpdbg_prompt_commands[] = {
	PHPDBG_COMMAND_D(exec,      "set execution context"),
	PHPDBG_COMMAND_D(compile,   "attempt to pre-compile execution context"),
	PHPDBG_COMMAND_D(run,       "attempt execution"),
	PHPDBG_COMMAND_D(print,     "print something"),
	PHPDBG_COMMAND_D(break,     "set breakpoint"),
	PHPDBG_COMMAND_D(help,      "show help menu"),
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
			return command->handler(params, cmd_len - expr_len TSRMLS_CC);
		}
		++command;
	}

	return FAILURE;
} /* }}} */

void phpdbg_breakpoint(zend_op_array *op_array) /* {{{ */
{
	printf(">> %s\n", op_array->filename);
} /* }}} */

void phpdbg_interactive(int argc, char **argv TSRMLS_DC) /* {{{ */
{
	char cmd[PHPDBG_MAX_CMD];

	printf("phpdbg> ");

	while (fgets(cmd, PHPDBG_MAX_CMD, stdin) != NULL) {
		size_t cmd_len = strlen(cmd) - 1;

		while (cmd[cmd_len] == '\n') {
			cmd[cmd_len] = 0;
		}

		if (cmd_len) {
			if (phpdbg_do_cmd(phpdbg_prompt_commands, cmd, cmd_len TSRMLS_CC) == FAILURE) {
			  printf("error executing %s !\n", cmd);
			}
		}

		printf("phpdbg> ");
	}
} /* }}} */

void phpdbg_execute_ex(zend_execute_data *execute_data TSRMLS_DC)
{
	zend_bool original_in_execution;

	original_in_execution = EG(in_execution);
	EG(in_execution) = 1;

	if (0) {
zend_vm_enter:
		execute_data = zend_create_execute_data_from_op_array(EG(active_op_array), 1 TSRMLS_CC);
	}

	while (1) {
    	int ret;
#ifdef ZEND_WIN32
		if (EG(timed_out)) {
			zend_timeout(0);
		}
#endif

        printf("[OPLINE: %p]\n", execute_data->opline);

        if (PHPDBG_G(has_breakpoint)) {
			phpdbg_breakpoint(execute_data->op_array);
		}

		if ((ret = execute_data->opline->handler(execute_data TSRMLS_CC)) > 0) {
			switch (ret) {
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
}
