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

#include "phpdbg.h"
#include "phpdbg_print.h"
#include "phpdbg_utils.h"
#include "phpdbg_opcode.h"
#include "phpdbg_break.h"
#include "phpdbg_bp.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

PHPDBG_BREAK(file) /* {{{ */
{
	phpdbg_param_t param;

	switch (phpdbg_parse_param(expr, expr_len, &param TSRMLS_CC)) {
		case EMPTY_PARAM:
			phpdbg_error("No expression provided");
			break;
		case FILE_PARAM:
			phpdbg_set_breakpoint_file(param.file.name, param.file.line TSRMLS_CC);
			break;
		default:
			phpdbg_error("Unsupported parameter type (%s) for function", phpdbg_get_param_type(&param TSRMLS_CC));
			break;
	}

	phpdbg_clear_param(&param TSRMLS_CC);

	return SUCCESS;
} /* }}} */

PHPDBG_BREAK(method) /* {{{ */
{
	phpdbg_param_t param;

	switch (phpdbg_parse_param(expr, expr_len, &param TSRMLS_CC)) {
		case EMPTY_PARAM:
			phpdbg_error("No expression provided");
			break;
		case METHOD_PARAM:
            phpdbg_set_breakpoint_method(param.method.class, param.method.name TSRMLS_CC);
            break;
            
		default:
			phpdbg_error("Unsupported parameter type (%s) for function", phpdbg_get_param_type(&param TSRMLS_CC));
			break;
    }

    phpdbg_clear_param(&param TSRMLS_CC);

    return SUCCESS;
} /* }}} */

PHPDBG_BREAK(address) /* {{{ */
{
	phpdbg_param_t param;
    
	switch (phpdbg_parse_param(expr, expr_len, &param TSRMLS_CC)) {
		case EMPTY_PARAM:
			phpdbg_error("No expression provided");
			break;
		case ADDR_PARAM:
            phpdbg_set_breakpoint_opline(param.addr TSRMLS_CC);
            break;
		default:
			phpdbg_error("Unsupported parameter type (%s) for function", phpdbg_get_param_type(&param TSRMLS_CC));
			break;
    }

    phpdbg_clear_param(&param TSRMLS_CC);

    return SUCCESS;
} /* }}} */

PHPDBG_BREAK(on) /* {{{ */
{
	if (expr_len == 0) {
		phpdbg_error("No expression provided!");
	} else {
		phpdbg_set_breakpoint_expression(expr, expr_len TSRMLS_CC);
	}
	return SUCCESS;
} /* }}} */

PHPDBG_BREAK(lineno) /* {{{ */
{
	phpdbg_param_t param;

	if (!PHPDBG_G(exec)) {
		phpdbg_error("Not file context found!");
		return SUCCESS;
	}

	switch (phpdbg_parse_param(expr, expr_len, &param TSRMLS_CC)) {
		case EMPTY_PARAM:
			phpdbg_error("No expression provided!");
			break;
		case NUMERIC_PARAM:
			phpdbg_set_breakpoint_file(phpdbg_current_file(TSRMLS_C), param.num TSRMLS_CC);
			break;
		default:
			phpdbg_error("Unsupported parameter type (%s) for function", phpdbg_get_param_type(&param TSRMLS_CC));
			break;
	}

	phpdbg_clear_param(&param TSRMLS_CC);

	return SUCCESS;
} /* }}} */

PHPDBG_BREAK(func) /* {{{ */
{
	phpdbg_param_t param;
    
	switch (phpdbg_parse_param(expr, expr_len, &param TSRMLS_CC)) {
		case EMPTY_PARAM:
			phpdbg_error("No expression provided!");
			break;
		case STR_PARAM:
			phpdbg_set_breakpoint_symbol(param.str TSRMLS_CC);
			break;
		default:
			phpdbg_error("Unsupported parameter type (%s) for function", phpdbg_get_param_type(&param TSRMLS_CC));
			break;
	}

	phpdbg_clear_param(&param TSRMLS_CC);

    return SUCCESS;
} /* }}} */
