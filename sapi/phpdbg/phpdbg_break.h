/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
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
