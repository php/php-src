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

#ifndef PHPDBG_LIST_H
#define PHPDBG_LIST_H

#include "TSRM.h"
#include "phpdbg_prompt.h"
#include "phpdbg_utils.h"

/**
 * Command Declarators
 */
#define PHPDBG_LIST_D(name, tip) \
	{PHPDBG_STRL(#name), tip, sizeof(tip)-1, 0, phpdbg_do_list_##name, NULL}
#define PHPDBG_LIST_EX_D(name, tip, alias) \
	{PHPDBG_STRL(#name), tip, sizeof(tip)-1, alias, phpdbg_do_list_##name, NULL}
#define PHPDBG_LIST(name) \
	int phpdbg_do_list_##name(phpdbg_param_t *param TSRMLS_DC)

PHPDBG_LIST(lines);
PHPDBG_LIST(class);
PHPDBG_LIST(method);
PHPDBG_LIST(func);

void phpdbg_list_function_byname(const char *, size_t TSRMLS_DC);
void phpdbg_list_function(const zend_function* TSRMLS_DC);
void phpdbg_list_file(const char*, long, long TSRMLS_DC);

static const phpdbg_command_t phpdbg_list_commands[] = {
    PHPDBG_LIST_EX_D(lines,     "lists the specified lines", 'l'),
    PHPDBG_LIST_EX_D(class,     "lists the specified class", 'c'),
    PHPDBG_LIST_EX_D(method,    "lists the specified method", 'm'),
    PHPDBG_LIST_EX_D(func,      "lists the specified function", 'f'),
    {NULL, 0, 0}
};

#endif /* PHPDBG_LIST_H */
