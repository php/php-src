/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
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

#endif /* PHPDBG_PRINT_H */
