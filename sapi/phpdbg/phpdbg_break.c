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
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "phpdbg.h"
#include "phpdbg_print.h"
#include "phpdbg_utils.h"
#include "phpdbg_opcode.h"
#include "phpdbg_break.h"
#include "phpdbg_bp.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

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
	phpdbg_set_breakpoint_at(param, input TSRMLS_CC);

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
