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

#ifndef PHPDBG_PRINT_H
#define PHPDBG_PRINT_H

#include "phpdbg_cmd.h"

#define PHPDBG_PRINT(name) PHPDBG_COMMAND(print_##name)

/**
 * Printer Forward Declarations
 */
PHPDBG_PRINT(exec);
PHPDBG_PRINT(opline);
PHPDBG_PRINT(class);
PHPDBG_PRINT(method);
PHPDBG_PRINT(func);
PHPDBG_PRINT(stack);

extern const phpdbg_command_t phpdbg_print_commands[];

void phpdbg_print_opcodes(const char *function);

void phpdbg_print_opline(zend_execute_data *execute_data, bool ignore_flags);
#endif /* PHPDBG_PRINT_H */
