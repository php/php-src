/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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
PHPDBG_BREAK(at);
PHPDBG_BREAK(del);

extern const phpdbg_command_t phpdbg_break_commands[];

#endif /* PHPDBG_BREAK_H */
