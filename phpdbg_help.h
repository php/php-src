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

#ifndef PHPDBG_HELP_H
#define PHPDBG_HELP_H

#include "TSRM.h"
#include "phpdbg_prompt.h"

/**
 * Command Declarators
 */
#define PHPDBG_HELP_D(name, tip) \
	{PHPDBG_STRL(#name), tip, sizeof(tip)-1, 0, phpdbg_do_help_##name}
#define PHPDBG_HELP(name) \
	int phpdbg_do_help_##name(const char *expr, size_t expr_len TSRMLS_DC)

/**
 * Helper Forward Declarations
 */
PHPDBG_HELP(exec);
PHPDBG_HELP(compile);
PHPDBG_HELP(step);
PHPDBG_HELP(next);
PHPDBG_HELP(run);
PHPDBG_HELP(eval);
PHPDBG_HELP(print);
PHPDBG_HELP(break);
PHPDBG_HELP(clean);
PHPDBG_HELP(clear);
PHPDBG_HELP(back);
PHPDBG_HELP(quiet);
PHPDBG_HELP(list);
PHPDBG_HELP(oplog);

/**
 * Commands
 */
static const phpdbg_command_t phpdbg_help_commands[] = {
	PHPDBG_HELP_D(exec,     "the execution context should be a valid path"),
	PHPDBG_HELP_D(compile,  "pre-compilation allows inspection of code before execution"),
	PHPDBG_HELP_D(step,     "stepping through execution allows inspection of the opline as it is executed"),
	PHPDBG_HELP_D(next,     "continue executing while stepping or after breaking"),
	PHPDBG_HELP_D(run,      "execution inside the phpdbg vm allows detailed inspection and debugging"),
	PHPDBG_HELP_D(eval,     "access to eval() allows you to affect the environment during execution"),
	PHPDBG_HELP_D(print,    "printing allows inspection of the execution environment"),
	PHPDBG_HELP_D(break,    "breakpoints allow execution interruption"),
	PHPDBG_HELP_D(clean,    "resetting the environment is useful while debugging and recompiling"),
	PHPDBG_HELP_D(clear,    "clearing breakpoints allows you to run code without interruption"),
	PHPDBG_HELP_D(back,     "show debug backtrace information during execution"),
    PHPDBG_HELP_D(quiet,    "be quiet during execution"),
	PHPDBG_HELP_D(list,     "listing code gives you quick access to code while executing"),
	PHPDBG_HELP_D(oplog,    "keep clutter off the screen by sending oplogs to a file"),
	{NULL, 0, 0}
};

#endif /* PHPDBG_HELP_H */
