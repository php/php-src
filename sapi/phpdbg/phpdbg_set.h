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

#ifndef PHPDBG_SET_H
#define PHPDBG_SET_H

#include "phpdbg_cmd.h"

#define PHPDBG_SET(name) PHPDBG_COMMAND(set_##name)

PHPDBG_SET(prompt);
#ifndef _WIN32
PHPDBG_SET(color);
PHPDBG_SET(colors);
#endif
PHPDBG_SET(oplog);
PHPDBG_SET(break);

static const phpdbg_command_t phpdbg_set_commands[] = {
	PHPDBG_COMMAND_D_EX(prompt,       "usage: set prompt <string>",          'p', set_prompt,       NULL, 0),
#ifndef _WIN32
	PHPDBG_COMMAND_D_EX(color,        "usage: set color  <element> <color>", 'c', set_color,        NULL, 1),
	PHPDBG_COMMAND_D_EX(colors,       "usage: set colors <on|off>",			 'C', set_colors,       NULL, 1),
#endif
	PHPDBG_COMMAND_D_EX(oplog,        "usage: set oplog  <output>",          'O', set_oplog,        NULL, 0),
	PHPDBG_COMMAND_D_EX(break,        "usage: set break [id] <on|off>",      'b', set_break,        NULL, 0),
	PHPDBG_END_COMMAND
};

#endif /* PHPDBG_SET_H */
