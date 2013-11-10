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
#include "phpdbg.h"
#include "phpdbg_help.h"

static const phpdbg_command_t phpdbg_prompt_commands[];

static PHPDBG_COMMAND(print) { /* {{{ */ 
  printf("%s", expr);
  
  return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(brake) { /* {{{ */
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
  PHPDBG_COMMAND_D(print, "print something"),
	PHPDBG_COMMAND_D(brake, "set brake point"),
	PHPDBG_COMMAND_D(help,  "show help menu"),
	PHPDBG_COMMAND_D(quit,  "exit phpdbg"),
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
