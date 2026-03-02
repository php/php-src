/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_PROMPT_H
#define PHPDBG_PROMPT_H

/* {{{ */
void phpdbg_string_init(char *buffer);
void phpdbg_init(char *init_file, size_t init_file_len, bool use_default);
void phpdbg_try_file_init(char *init_file, size_t init_file_len, bool free_init);
int phpdbg_interactive(bool allow_async_unsafe, char *input);
int phpdbg_compile(void);
int phpdbg_compile_stdin(zend_string *code);
void phpdbg_force_interruption(void);
/* }}} */

/* {{{ phpdbg command handlers */
PHPDBG_COMMAND(exec);
PHPDBG_COMMAND(stdin);
PHPDBG_COMMAND(step);
PHPDBG_COMMAND(continue);
PHPDBG_COMMAND(run);
PHPDBG_COMMAND(ev);
PHPDBG_COMMAND(until);
PHPDBG_COMMAND(finish);
PHPDBG_COMMAND(leave);
PHPDBG_COMMAND(frame);
PHPDBG_COMMAND(print);
PHPDBG_COMMAND(break);
PHPDBG_COMMAND(back);
PHPDBG_COMMAND(list);
PHPDBG_COMMAND(info);
PHPDBG_COMMAND(clean);
PHPDBG_COMMAND(clear);
PHPDBG_COMMAND(help);
PHPDBG_COMMAND(sh);
PHPDBG_COMMAND(dl);
PHPDBG_COMMAND(generator);
PHPDBG_COMMAND(set);
PHPDBG_COMMAND(source);
PHPDBG_COMMAND(export);
PHPDBG_COMMAND(register);
PHPDBG_COMMAND(quit);
PHPDBG_COMMAND(watch);
PHPDBG_COMMAND(next);
PHPDBG_COMMAND(eol); /* }}} */

/* {{{ prompt commands */
extern const phpdbg_command_t phpdbg_prompt_commands[]; /* }}} */

void phpdbg_execute_ex(zend_execute_data *execute_data);

#endif /* PHPDBG_PROMPT_H */
