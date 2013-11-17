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

#ifndef PHPDBG_CMD_H
#define PHPDBG_CMD_H

#include "TSRM.h"

typedef struct _phpdbg_command_t phpdbg_command_t;

/* {{{ Command and Parameter */
enum {
	NO_ARG = 0,
	REQUIRED_ARG,
	OPTIONAL_ARG
};

typedef enum {
	EMPTY_PARAM = 0,
	ADDR_PARAM,
	FILE_PARAM,
	METHOD_PARAM,
	STR_PARAM,
	NUMERIC_PARAM
} phpdbg_param_type;

typedef struct _phpdbg_param {
    phpdbg_param_type type;
	long num;
	zend_ulong addr;
	struct {
		char *name;
		long line;
	} file;
	struct {
		char *class;
		char *name;
	} method;
	char *str;
	size_t len;
} phpdbg_param_t;

typedef int (*phpdbg_command_handler_t)(phpdbg_param_t *param TSRMLS_DC);

struct _phpdbg_command_t {
	const char *name;                   /* Command name */
	size_t name_len;                    /* Command name length */
	const char *tip;                    /* Menu tip */
	size_t tip_len;                     /* Menu tip length */
	char alias;                         /* Alias */
	phpdbg_command_handler_t handler;   /* Command handler */
	const phpdbg_command_t *subs;       /* Sub Commands */
	char has_args;                      /* Accept args? */
};
/* }}} */

#define PHPDBG_STRL(s) s, sizeof(s)-1

/**
 * Command Executor
 */
int phpdbg_do_cmd(const phpdbg_command_t*, phpdbg_command_t**, char *, size_t TSRMLS_DC);
phpdbg_param_type phpdbg_parse_param(const char*, size_t, phpdbg_param_t* TSRMLS_DC);
void phpdbg_clear_param(phpdbg_param_t * TSRMLS_DC);
const char* phpdbg_get_param_type(const phpdbg_param_t *param TSRMLS_DC);

/**
 * Command Declarators
 */
#define PHPDBG_COMMAND_HANDLER(name) phpdbg_do_##name

#define PHPDBG_COMMAND_D_EX(name, tip, alias, handler, children, has_args) \
	{PHPDBG_STRL(#name), tip, sizeof(tip)-1, alias, phpdbg_do_##handler, children, has_args}

#define PHPDBG_COMMAND_D(name, tip, alias, children, has_args) \
	{PHPDBG_STRL(#name), tip, sizeof(tip)-1, alias, phpdbg_do_##name, children, has_args}

#define PHPDBG_COMMAND(name) int phpdbg_do_##name(phpdbg_param_t *param TSRMLS_DC)

#define PHPDBG_END_COMMAND {NULL, 0, NULL, 0, '\0', NULL, NULL, '\0'}

#define phpdbg_default_switch_case() \
    default:\
        phpdbg_error(\
            "Unsupported parameter type (%s) for command", \
                phpdbg_get_param_type(param TSRMLS_CC)); \
    break

#endif /* PHPDBG_CMD_H */
