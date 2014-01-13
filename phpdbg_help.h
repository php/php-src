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

#ifndef PHPDBG_HELP_H
#define PHPDBG_HELP_H

#include "TSRM.h"
#include "phpdbg.h"
#include "phpdbg_cmd.h"

#define PHPDBG_HELP(name) PHPDBG_COMMAND(help_##name)

/**
 * Helper Forward Declarations
 */
PHPDBG_HELP(exec);
PHPDBG_HELP(compile);
PHPDBG_HELP(step);
PHPDBG_HELP(next);
PHPDBG_HELP(run);
PHPDBG_HELP(eval);
PHPDBG_HELP(until);
PHPDBG_HELP(finish);
PHPDBG_HELP(leave);
PHPDBG_HELP(print);
PHPDBG_HELP(break);
PHPDBG_HELP(clean);
PHPDBG_HELP(clear);
PHPDBG_HELP(info);
PHPDBG_HELP(back);
PHPDBG_HELP(frame);
PHPDBG_HELP(quiet);
PHPDBG_HELP(list);
PHPDBG_HELP(set);
PHPDBG_HELP(register);
PHPDBG_HELP(options);
PHPDBG_HELP(source);
PHPDBG_HELP(shell);

/**
 * Commands
 */
static const phpdbg_command_t phpdbg_help_commands[] = {
	PHPDBG_COMMAND_D_EX(exec,     "the execution context should be a valid path",                                    'e', help_exec,    NULL, 0),
	PHPDBG_COMMAND_D_EX(compile,  "allow inspection of code before execution",                                       'c', help_compile, NULL, 0),
	PHPDBG_COMMAND_D_EX(step,     "step through execution to break at every opcode",                                 's', help_step,    NULL, 0),
	PHPDBG_COMMAND_D_EX(next,     "continue executing while stepping or after breaking",                             'n', help_next,    NULL, 0),
	PHPDBG_COMMAND_D_EX(run,      "execute inside the phpdbg vm",                                                    'r', help_run,     NULL, 0),
	PHPDBG_COMMAND_D_EX(eval,     "access to eval() allows affecting the environment",                               'E', help_eval,    NULL, 0),
	PHPDBG_COMMAND_D_EX(until,    "continue until the current line is executed",                                     'u', help_until,   NULL, 0),
	PHPDBG_COMMAND_D_EX(finish,   "continue until the current function has returned",                                'F', help_finish,  NULL, 0),
	PHPDBG_COMMAND_D_EX(leave,    "continue until the current function is returning",                                'L', help_leave,   NULL, 0),
	PHPDBG_COMMAND_D_EX(print,    "print context information or instructions",                                       'p', help_print,   NULL, 0),
	PHPDBG_COMMAND_D_EX(break,    "breakpoints allow execution interruption",                                        'b', help_break,   NULL, 0),
	PHPDBG_COMMAND_D_EX(clean,    "resetting the environment is useful while debugging",                             'X', help_clean,   NULL, 0),
	PHPDBG_COMMAND_D_EX(clear,    "reset breakpoints to execute without interruption",                               'c', help_clear,   NULL, 0),
	PHPDBG_COMMAND_D_EX(info,     "quick access to useful information on the console",                               'i', help_info,    NULL, 0),
	PHPDBG_COMMAND_D_EX(back,     "show debug backtrace information during execution",                               't', help_back,    NULL, 0),
	PHPDBG_COMMAND_D_EX(frame,    "switch to a frame in the current stack for inspection",                           'f', help_frame,   NULL, 0),
	PHPDBG_COMMAND_D_EX(quiet,    "be quiet during execution",                                                       'Q', help_quiet,   NULL, 0),
	PHPDBG_COMMAND_D_EX(list,     "list code gives you quick access to code",                                        'l', help_list,    NULL, 0),
	PHPDBG_COMMAND_D_EX(set,      "configure how phpdbg looks and behaves",                                          'S', help_set,     NULL, 0),
	PHPDBG_COMMAND_D_EX(register, "register a function for use as a command",                                        'R', help_register,NULL, 0),
	PHPDBG_COMMAND_D_EX(options,  "show information about command line options",                                     'o', help_options, NULL, 0),
	PHPDBG_COMMAND_D_EX(source,   "load a phpdbginit file at the console",                                           '.', help_source,  NULL, 0),
	PHPDBG_COMMAND_D_EX(shell,    "execute system commands with direct shell access",                                '-', help_shell,   NULL, 0),
	PHPDBG_END_COMMAND
};

#define phpdbg_help_header() \
	phpdbg_notice("Welcome to phpdbg, the interactive PHP debugger, v%s", PHPDBG_VERSION);
#define phpdbg_help_footer() \
	phpdbg_notice("Please report bugs to <%s>", PHPDBG_ISSUES);

#endif /* PHPDBG_HELP_H */
