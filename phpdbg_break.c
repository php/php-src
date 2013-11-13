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
   if (expr && expr_len > 0L) {
        if (!phpdbg_is_class_method(expr, expr_len, NULL, NULL)) {
	        char path[MAXPATHLEN], resolved_name[MAXPATHLEN];
	        const char *line_pos = strchr(expr, ':');
		    long line_num = strtol(line_pos+1, NULL, 0);
		    
		    if (line_num) {
		        memcpy(path, expr, line_pos - expr);
		        path[line_pos - expr] = 0;

		        if (expand_filepath(path, resolved_name TSRMLS_CC) == NULL) {
			        phpdbg_error("Failed to expand path %s", path);
			        return SUCCESS;
		        }

		        phpdbg_set_breakpoint_file(resolved_name, line_num TSRMLS_CC);
		    } else {
		        phpdbg_error("No line specified in expression %s", expr);
		        return SUCCESS;
		    }
        }
   } else {
        phpdbg_error(
            "No expression provided");
   }
   return SUCCESS;
} /* }}} */

PHPDBG_BREAK(method) /* {{{ */
{
    char *class_name;
    char *func_name;
    
    if (expr && expr_len >0L) {
        if (phpdbg_is_class_method(expr, expr_len+1, &class_name, &func_name)) {
            phpdbg_set_breakpoint_method(
                class_name, func_name TSRMLS_CC);
        } else {
            phpdbg_error(
                "The expression provided is not a method name: %s", expr);
        }
    } else {
        phpdbg_error("No expression provided");
    }
    
    return SUCCESS;
} /* }}} */

PHPDBG_BREAK(address) /* {{{ */
{
    if (expr && expr_len > 0L) {
        if (phpdbg_is_addr(expr)) {
            phpdbg_set_breakpoint_opline(
                strtoul(expr, 0, 16) TSRMLS_CC);
        } else {
            phpdbg_error("The expression provided is not an address: %s", expr);
        }
    } else {
        phpdbg_error("No expression provided");
    }
    return SUCCESS;
} /* }}} */

PHPDBG_BREAK(on) /* {{{ */
{
	if (expr && expr_len > 0L) {
	    phpdbg_set_breakpoint_expression(
	        expr, expr_len TSRMLS_CC);
	} else {
	    phpdbg_error(
	        "No expression provided!");
	}
	
	return SUCCESS;
} /* }}} */

PHPDBG_BREAK(lineno) /* {{{ */
{
    /* note: this can break on [no active file] */
    if (expr && expr_len > 0L) {
	   if (phpdbg_is_numeric(expr)) {
	        const char *filename = zend_get_executed_filename(TSRMLS_C);
		    
		    if (filename) {
		        phpdbg_set_breakpoint_file(
		            filename, strtol(expr, NULL, 0) TSRMLS_CC);
		    } else {
		        phpdbg_error("No file context found");
		    }
	   } else {
	      phpdbg_error(
	        "The expression provided is not a valid line number %s", expr);
	   }
	} else {
	    phpdbg_error(
	        "No expression provided!");
	}
	
	return SUCCESS;
} /* }}} */

PHPDBG_BREAK(func) /* {{{ */
{
    if (expr && expr_len > 0L) {
        char name[200];
        size_t name_len = expr_len;
        
	    name_len = MIN(name_len, 200);
	    memcpy(name, expr, name_len);
	    name[name_len] = 0;

	    phpdbg_set_breakpoint_symbol(name TSRMLS_CC);
    } else {
        phpdbg_error("No expression provided");
    }
    return SUCCESS;
} /* }}} */
