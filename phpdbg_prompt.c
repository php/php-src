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
   +----------------------------------------------------------------------+
*/

#include <stdio.h>
#include <string.h>
#include "zend.h"
#include "phpdbg_prompt.h"

void do_quit(const char *params) /* {{{ */
{
	zend_bailout();
} /* }}} */

static const phpdbg_command prompt_commands[] = {
	{PHPDBG_STRL("quit"), do_quit},
	{NULL, 0, 0}
};

static void do_cmd(char *cmd_line) /* {{{ */
{
	const phpdbg_command *command = prompt_commands;
	char *params = NULL;
	const char *cmd = strtok_r(cmd_line, " ", &params);
	size_t cmd_len = cmd ? strlen(cmd) : 0;

	while (command && command->name) {
		if (command->name_len == cmd_len
			&& memcmp(cmd, command->name, cmd_len) == 0) {
			/* Command find */
			command->handler(params);
			return;
		}
		++command;
	}

	printf("command not found!\n");

} /* }}} */

void phpdbg_iteractive(int argc, char **argv) /* {{{ */
{
	char cmd[PHPDBG_MAX_CMD];

	printf("phpdbg> ");

    while (fgets(cmd, PHPDBG_MAX_CMD, stdin) != NULL) {
		size_t cmd_len = strlen(cmd) - 1;

		if (cmd[cmd_len] == '\n') {
			cmd[cmd_len] = 0;
		}
		if (cmd_len) {
			do_cmd(cmd);
		}
		printf("phpdbg> ");
	}
} /* }}} */
