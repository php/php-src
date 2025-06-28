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

#ifndef PHPDBG_SET_H
#define PHPDBG_SET_H

#include "phpdbg_cmd.h"

#define PHPDBG_SET(name) PHPDBG_COMMAND(set_##name)

PHPDBG_SET(prompt);
#ifndef _WIN32
PHPDBG_SET(color);
PHPDBG_SET(colors);
#endif
PHPDBG_SET(break);
PHPDBG_SET(breaks);
PHPDBG_SET(quiet);
PHPDBG_SET(stepping);
PHPDBG_SET(refcount);
PHPDBG_SET(pagination);
PHPDBG_SET(lines);

extern const phpdbg_command_t phpdbg_set_commands[];

#endif /* PHPDBG_SET_H */
