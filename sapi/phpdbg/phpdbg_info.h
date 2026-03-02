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

#ifndef PHPDBG_INFO_H
#define PHPDBG_INFO_H

#include "phpdbg_cmd.h"

#define PHPDBG_INFO(name) PHPDBG_COMMAND(info_##name)

PHPDBG_INFO(files);
PHPDBG_INFO(break);
PHPDBG_INFO(classes);
PHPDBG_INFO(funcs);
PHPDBG_INFO(error);
PHPDBG_INFO(constants);
PHPDBG_INFO(vars);
PHPDBG_INFO(globals);
PHPDBG_INFO(literal);
PHPDBG_INFO(memory);

extern const phpdbg_command_t phpdbg_info_commands[];

#endif /* PHPDBG_INFO_H */
