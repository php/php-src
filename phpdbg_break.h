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

#ifndef PHPDBG_BREAK_H
#define PHPDBG_BREAK_H

#include "TSRM.h"

/**
 * Command Declarators
 */
#define PHPDBG_BREAK_D(name, tip) \
	{PHPDBG_STRL(#name), tip, sizeof(tip)-1, 0, phpdbg_do_break_##name}
#define PHPDBG_BREAK_EX_D(name, tip, alias) \
	{PHPDBG_STRL(#name), tip, sizeof(tip)-1, alias, phpdbg_do_break_##name}
#define PHPDBG_BREAK(name) \
	int phpdbg_do_break_##name(phpdbg_param_t *param TSRMLS_DC)

/**
 * Printer Forward Declarations
 */
PHPDBG_BREAK(file);
PHPDBG_BREAK(method);
PHPDBG_BREAK(address);
PHPDBG_BREAK(on);
PHPDBG_BREAK(lineno);
PHPDBG_BREAK(func);

/**
 * Commands
 */
static const phpdbg_command_t phpdbg_break_commands[] = {
    PHPDBG_BREAK_EX_D(file,        "specify breakpoint by file:line", 'F'),
    PHPDBG_BREAK_EX_D(method,      "specify breakpoint by class::method", 'm'),
    PHPDBG_BREAK_EX_D(address,     "specify breakpoint by address", 'a'),
	PHPDBG_BREAK_EX_D(on,          "specify breakpoint by expression", 'o'),
	PHPDBG_BREAK_EX_D(lineno,      "specify breakpoint by line of currently executing file", 'l'),
	PHPDBG_BREAK_EX_D(func,        "specify breakpoint by global function name", 'f'),
	{NULL, 0, 0}
};

#endif /* PHPDBG_BREAK_H */
