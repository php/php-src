/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
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
