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

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

PHPDBG_PRINT(opline) /* {{{ */
{
	if (EG(in_execution) && EG(current_execute_data)) {
	    phpdbg_print_opline(
	        EG(current_execute_data), 1 TSRMLS_CC);
	} else {
		phpdbg_error("Not Executing!");
	}

	return SUCCESS;
} /* }}} */

PHPDBG_PRINT(class) /* {{{ */
{
    zend_class_entry **ce;
    
	if (expr && expr_len > 0L) {
	    if (zend_lookup_class(expr, strlen(expr), &ce TSRMLS_CC) == SUCCESS) {
	        
	    } else {
	        phpdbg_error("Cannot find class %s/%lu", expr, expr_len);
	    }
	} else {
		phpdbg_error("No class name provided!");
	}

	return SUCCESS;
} /* }}} */
