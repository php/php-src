/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

#define PHPDBG_BREAK_COMMAND_D(f, h, a, m, l, s) \
	PHPDBG_COMMAND_D_EXP(f, h, a, m, l, s, &phpdbg_prompt_commands[10])

/**
 * Commands
 */
const phpdbg_command_t phpdbg_break_commands[] = {
	PHPDBG_BREAK_COMMAND_D(file,        "specify breakpoint by file:line",                        'F', break_file,    NULL, "f"),
	PHPDBG_BREAK_COMMAND_D(func,        "specify breakpoint by global function name",             'f', break_func,    NULL, "s"),
	PHPDBG_BREAK_COMMAND_D(method,      "specify breakpoint by class::method",                    'm', break_method,  NULL, "m"),
	PHPDBG_BREAK_COMMAND_D(address,     "specify breakpoint by address",                          'a', break_address, NULL, "a"),
	PHPDBG_BREAK_COMMAND_D(op,          "specify breakpoint by opcode",                           'O', break_op,      NULL, "s"),
	PHPDBG_BREAK_COMMAND_D(on,          "specify breakpoint by condition",                        'o', break_on,      NULL, "c"),
	PHPDBG_BREAK_COMMAND_D(at,         "specify breakpoint by location and condition",            'A', break_at,      NULL, "*c"),
	PHPDBG_BREAK_COMMAND_D(lineno,      "specify breakpoint by line of currently executing file", 'l', break_lineno,  NULL, "n"),
	PHPDBG_BREAK_COMMAND_D(del,         "delete breakpoint by identifier number",                 'd', break_del,     NULL, "n"),
	PHPDBG_END_COMMAND
};

PHPDBG_BREAK(file) /* {{{ */
{
	switch (param->type) {
		case FILE_PARAM:
			phpdbg_set_breakpoint_file(param->file.name, param->file.line TSRMLS_CC);
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_BREAK(method) /* {{{ */
{
	switch (param->type) {
		case METHOD_PARAM:
			phpdbg_set_breakpoint_method(param->method.class, param->method.name TSRMLS_CC);
			break;

		phpdbg_default_switch_case();
	}
	
	return SUCCESS;
} /* }}} */

PHPDBG_BREAK(address) /* {{{ */
{
	switch (param->type) {
		case ADDR_PARAM:
			phpdbg_set_breakpoint_opline(param->addr TSRMLS_CC);
			break;

		case NUMERIC_METHOD_PARAM:
			phpdbg_set_breakpoint_method_opline(param->method.class, param->method.name, param->num TSRMLS_CC);
			break;

		case NUMERIC_FUNCTION_PARAM:
			phpdbg_set_breakpoint_function_opline(param->str, param->num TSRMLS_CC);
			break;			

		case FILE_PARAM:
			phpdbg_set_breakpoint_file_opline(param->file.name, param->file.line TSRMLS_CC);
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_BREAK(on) /* {{{ */
{
	switch (param->type) {
		case STR_PARAM:
			phpdbg_set_breakpoint_expression(param->str, param->len TSRMLS_CC);
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_BREAK(at) /* {{{ */
{
	phpdbg_set_breakpoint_at(param TSRMLS_CC);

	return SUCCESS;
} /* }}} */

PHPDBG_BREAK(lineno) /* {{{ */
{
	switch (param->type) {
		case NUMERIC_PARAM: {
			if (PHPDBG_G(exec)) {
				phpdbg_set_breakpoint_file(phpdbg_current_file(TSRMLS_C), param->num TSRMLS_CC);
			} else {
				phpdbg_error("Execution context not set!");
			}
		} break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_BREAK(func) /* {{{ */
{
	switch (param->type) {
		case STR_PARAM:
			phpdbg_set_breakpoint_symbol(param->str, param->len TSRMLS_CC);
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_BREAK(op) /* {{{ */
{
	switch (param->type) {
		case STR_PARAM:
			phpdbg_set_breakpoint_opcode(param->str, param->len TSRMLS_CC);
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_BREAK(del) /* {{{ */
{
	switch (param->type) {
		case NUMERIC_PARAM: {
			phpdbg_delete_breakpoint(param->num TSRMLS_CC);
		} break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */
