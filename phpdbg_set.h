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

#ifndef PHPDBG_SET_H
#define PHPDBG_SET_H

#include "phpdbg_cmd.h"

#define PHPDBG_SET(name) PHPDBG_COMMAND(set_##name)

void phpdbg_set_prompt(const char*, const char* TSRMLS_DC);
const char *phpdbg_get_prompt(TSRMLS_D);

void phpdbg_set_prompt_color(const char* TSRMLS_DC);
const char *phpdbg_get_prompt_color(TSRMLS_D);

PHPDBG_SET(prompt);
PHPDBG_SET(prompt_color);
PHPDBG_SET(oplog);

static const phpdbg_command_t phpdbg_set_commands[] = {
    PHPDBG_COMMAND_D_EX(prompt,       "set prompt",                'p', set_prompt,       NULL, 0),
    PHPDBG_COMMAND_D_EX(prompt_color, "set prompt color",          'c', set_prompt_color, NULL, 0),
    PHPDBG_COMMAND_D_EX(oplog,        "set oplog output",          'O', set_oplog,        NULL, 0),
    PHPDBG_END_COMMAND
};


#endif /* PHPDBG_SET_H */
