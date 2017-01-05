/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
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

#include "phpdbg.h"
#include "phpdbg_print.h"
#include "phpdbg_utils.h"
#include "phpdbg_opcode.h"
#include "phpdbg_break.h"
#include "phpdbg_bp.h"
#include "phpdbg_prompt.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

#define PHPDBG_BREAK_COMMAND_D(f, h, a, m, l, s, flags) \
	PHPDBG_COMMAND_D_EXP(f, h, a, m, l, s, &phpdbg_prompt_commands[9], flags)

/**
 * Commands
 */
const phpdbg_command_t phpdbg_break_commands[] = {
	PHPDBG_BREAK_COMMAND_D(at,         "specify breakpoint by location and condition",           '@', break_at,      NULL, "*c", 0),
	PHPDBG_BREAK_COMMAND_D(del,        "delete breakpoint by identifier number",                 '~', break_del,     NULL, "n",  0),
	PHPDBG_END_COMMAND
};

PHPDBG_BREAK(at) /* {{{ */
{
	phpdbg_set_breakpoint_at(param);

	return SUCCESS;
} /* }}} */

PHPDBG_BREAK(del) /* {{{ */
{
	phpdbg_delete_breakpoint(param->num);

	return SUCCESS;
} /* }}} */
