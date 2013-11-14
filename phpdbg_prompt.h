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

#ifndef PHPDBG_PROMPT_H
#define PHPDBG_PROMPT_H

/**
 * Maximum command length
 */
#define PHPDBG_MAX_CMD 500

#define PHPDBG_STRL(s) s, sizeof(s)-1

#define PHPDBG_NEXT 2

/**
 * Command handler
 */
typedef int (*phpdbg_command_handler_t)(const char* expr, size_t expr_len TSRMLS_DC);

/**
 * Command representation
 */
struct _phpdbg_command_t {
	const char *name;                   /* Command name */
	size_t name_len;                    /* Command name length */
	const char *tip;                    /* Menu tip */
	size_t tip_len;                     /* Menu tip length */
	char alias;                         /* Alias */
	phpdbg_command_handler_t handler;   /* Command handler */
} ;

/**
 * Command Executor
 */
int phpdbg_do_cmd(const phpdbg_command_t *command, char *cmd_line, size_t cmd_len TSRMLS_DC);

/**
 * Command Declarators
 */
#define PHPDBG_COMMAND_D(name, tip) \
	{PHPDBG_STRL(#name), tip, sizeof(tip)-1, 0, phpdbg_do_##name}
#define PHPDBG_COMMAND_EX_D(name, tip, alias) \
	{PHPDBG_STRL(#name), tip, sizeof(tip)-1, alias, phpdbg_do_##name}
#define PHPDBG_COMMAND(name) \
	int phpdbg_do_##name(const char *expr, size_t expr_len TSRMLS_DC)

void phpdbg_init(char *init_file, size_t init_file_len, zend_bool use_default TSRMLS_DC);
void phpdbg_welcome(zend_bool cleaning TSRMLS_DC);
int phpdbg_interactive(TSRMLS_D);
void phpdbg_execute_ex(zend_execute_data *execute_data TSRMLS_DC);
void phpdbg_print_opline(zend_execute_data *execute_data, zend_bool ignore_flags TSRMLS_DC);
void phpdbg_clean(zend_bool full TSRMLS_DC);

#endif /* PHPDBG_PROMPT_H */
