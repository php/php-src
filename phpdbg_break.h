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
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_BREAK_H
#define PHPDBG_BREAK_H

#include "TSRM.h"
#include "phpdbg_cmd.h"

#define PHPDBG_BREAK(name) PHPDBG_COMMAND(break_##name)

/**
 * Printer Forward Declarations
 */
PHPDBG_BREAK(file);
PHPDBG_BREAK(func);
PHPDBG_BREAK(method);
PHPDBG_BREAK(address);
PHPDBG_BREAK(at);
PHPDBG_BREAK(op);
PHPDBG_BREAK(on);
PHPDBG_BREAK(lineno);
PHPDBG_BREAK(del);

/**
 * Commands
 */
static const phpdbg_command_t phpdbg_break_commands[] = {
	PHPDBG_COMMAND_D_EX(file,        "specify breakpoint by file:line",                        'F', break_file,    NULL, 1),
	PHPDBG_COMMAND_D_EX(func,        "specify breakpoint by global function name",             'f', break_func,    NULL, 1),
	PHPDBG_COMMAND_D_EX(method,      "specify breakpoint by class::method",                    'm', break_method,  NULL, 1),
	PHPDBG_COMMAND_D_EX(address,     "specify breakpoint by address",                          'a', break_address, NULL, 1),
	PHPDBG_COMMAND_D_EX(op,          "specify breakpoint by opcode",                           'O', break_op,      NULL, 1),
	PHPDBG_COMMAND_D_EX(on,          "specify breakpoint by condition",                        'o', break_on,      NULL, 1),
	PHPDBG_COMMAND_D_EX(at,          "specify breakpoint by location and condition",           'A', break_at,      NULL, 1),
	PHPDBG_COMMAND_D_EX(lineno,      "specify breakpoint by line of currently executing file", 'l', break_lineno,  NULL, 1),
	PHPDBG_COMMAND_D_EX(del,         "delete breakpoint by identifier number",                 'd', break_del,     NULL, 1),
	PHPDBG_END_COMMAND
};

#endif /* PHPDBG_BREAK_H */
