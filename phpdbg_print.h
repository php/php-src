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

#ifndef PHPDBG_PRINT_H
#define PHPDBG_PRINT_H

#include "TSRM.h"
#include "phpdbg_prompt.h"

/**
 * Command Declarators
 */
#define PHPDBG_PRINT_D(name, tip, alias) \
	{PHPDBG_STRL(#name), tip, sizeof(tip)-1, alias, phpdbg_do_print_##name}
#define PHPDBG_PRINT(name) \
	int phpdbg_do_print_##name(const char *expr, size_t expr_len TSRMLS_DC)

/**
 * Printer Forward Declarations
 */
PHPDBG_PRINT(opline);
PHPDBG_PRINT(class);
PHPDBG_PRINT(method);
PHPDBG_PRINT(func);

/**
 * Commands
 */
static const phpdbg_command_t phpdbg_print_commands[] = {
	PHPDBG_PRINT_D(opline,     "print the current opline information", 'o'),
	PHPDBG_PRINT_D(class,      "print out the instructions in the specified class", 'c'),
	PHPDBG_PRINT_D(method,     "print out the instructions in the specified method", 'm'),
	PHPDBG_PRINT_D(func,       "print out the instructions in the specified function", 'f'),
	{0, 0, 0, 0}
};

#endif /* PHPDBG_PRINT_H */
