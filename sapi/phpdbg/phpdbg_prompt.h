/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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

#ifndef PHPDBG_PROMPT_H
#define PHPDBG_PROMPT_H

/* {{{ */
void phpdbg_init(char *init_file, size_t init_file_len, zend_bool use_default TSRMLS_DC);
void phpdbg_try_file_init(char *init_file, size_t init_file_len, zend_bool free_init TSRMLS_DC);
int phpdbg_interactive(TSRMLS_D);
int phpdbg_compile(TSRMLS_D);
void phpdbg_clean(zend_bool full TSRMLS_DC); /* }}} */

/* {{{ phpdbg command handlers */
PHPDBG_COMMAND(exec);
PHPDBG_COMMAND(compile);
PHPDBG_COMMAND(step);
PHPDBG_COMMAND(next);
PHPDBG_COMMAND(run);
PHPDBG_COMMAND(eval);
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
PHPDBG_COMMAND(quiet);
PHPDBG_COMMAND(aliases);
PHPDBG_COMMAND(shell);
PHPDBG_COMMAND(set);
PHPDBG_COMMAND(source);
PHPDBG_COMMAND(register);
PHPDBG_COMMAND(quit); /* }}} */

/* {{{ prompt commands */
extern const phpdbg_command_t phpdbg_prompt_commands[]; /* }}} */

/* {{{ */
#if PHP_VERSION_ID >= 50500
void phpdbg_execute_ex(zend_execute_data *execute_data TSRMLS_DC);
#else
void phpdbg_execute_ex(zend_op_array *op_array TSRMLS_DC);
#endif /* }}} */

#endif /* PHPDBG_PROMPT_H */
