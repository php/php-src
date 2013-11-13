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

static inline phpdbg_print_function_helper(zend_function *method TSRMLS_DC) {
    switch (method->type) {
        case ZEND_USER_FUNCTION: {
            zend_op_array* op_array = &method->op_array;

            if (op_array) {
                zend_op     *opline = &op_array->opcodes[0];
                zend_uint   opcode = 0,
                            end =  op_array->last-1;

                if (method->common.scope) {
                    phpdbg_writeln(
                        "\t#%d-%d %s::%s() %s",
                        op_array->line_start, op_array->line_end,
                        method->common.scope->name, 
                        method->common.function_name,
                        op_array->filename ? op_array->filename : "unknown");
                } else {
                    phpdbg_writeln(
                        "\t#%d-%d %s() %s",
                        op_array->line_start, op_array->line_end,
                        method->common.function_name,
                        op_array->filename ? op_array->filename : "unknown");
                }
                
                
                do {
                    char *decode = phpdbg_decode_opcode(opline->opcode);
                    if (decode != NULL) {
                        phpdbg_writeln(
                            "\t\t#%lu\t%p %s", opline->lineno, opline, decode); 
                    } else phpdbg_error("\tFailed to decode opline @ %ld", opline);
                    
                    opline++;
                } while (++opcode < end);
            }
        } break;
        
        default: {
            if (method->common.scope) {
                phpdbg_writeln(
                    "\tInternal %s::%s()", method->common.scope->name, method->common.function_name);
            } else {
                phpdbg_writeln(
                    "\tInternal %s()", method->common.function_name);
            }
        }
     }
}

PHPDBG_PRINT(class) /* {{{ */
{
    zend_class_entry **ce;
    
	if (expr && expr_len > 0L) {
	    if (zend_lookup_class(expr, strlen(expr), &ce TSRMLS_CC) == SUCCESS) {
	        phpdbg_notice(
	            "%s %s: %s", 
	            ((*ce)->type == ZEND_USER_CLASS) ? 
	                "User" : "Internal",
	            ((*ce)->ce_flags & ZEND_ACC_INTERFACE) ? 
	                "Interface" :
	                    ((*ce)->ce_flags & ZEND_ACC_ABSTRACT) ?
	                        "Abstract Class" :
	                            "Class", 
	            (*ce)->name);
	        
	        phpdbg_writeln("Methods (%d):", zend_hash_num_elements(&(*ce)->function_table));
	        if (zend_hash_num_elements(&(*ce)->function_table)) {
	            HashPosition position;
	            zend_function *method;
	            
	            for (zend_hash_internal_pointer_reset_ex(&(*ce)->function_table, &position);
	                 zend_hash_get_current_data_ex(&(*ce)->function_table, (void**) &method, &position) == SUCCESS;
	                 zend_hash_move_forward_ex(&(*ce)->function_table, &position)) {
	                 phpdbg_print_function_helper(method TSRMLS_CC);
	            }
	        }
	    } else {
	        phpdbg_error("Cannot find class %s", expr);
	    }
	} else {
		phpdbg_error("No class name provided!");
	}

	return SUCCESS;
} /* }}} */

PHPDBG_PRINT(method) /* {{{ */
{
    if (expr && expr_len > 0L) {
        char *class_name = NULL;
        char *func_name = NULL;
        
        if (phpdbg_is_class_method(expr, expr_len, &class_name, &func_name)) {
            zend_class_entry **ce;
            
            if (zend_lookup_class(class_name, strlen(class_name), &ce TSRMLS_CC) == SUCCESS) {
                zend_function *fbc;
                
                if (zend_hash_find(&(*ce)->function_table, func_name, strlen(func_name)+1, (void**)&fbc) == SUCCESS) {
                    phpdbg_notice(
	                    "%s Method %s", 
	                    (fbc->type == ZEND_USER_FUNCTION) ? "User" : "Internal", 
	                    fbc->common.function_name);
	                    
			        phpdbg_print_function_helper(fbc TSRMLS_CC);
                } else {
                    phpdbg_error("The method %s could not be found", func_name);
                }
            }
            
            efree(class_name);
            efree(func_name);
        } else {
            phpdbg_error("The expression provided is not a valid method %s", expr);
        }
    } else {
        phpdbg_error("No expression provided");
    }
    return SUCCESS;
} /* }}} */

PHPDBG_PRINT(func) /* {{{ */
{
    if (expr && expr_len > 0L) {
        HashTable *func_table = EG(function_table);
		zend_function* fbc;
        const char *func_name = expr;
        size_t func_name_len = expr_len;
        
        /* search active scope if begins with period */
        if (func_name[0] == '.') {
           if (EG(scope)) {
               func_name++;
               func_name_len--;

               func_table = &EG(scope)->function_table;
           } else {
               phpdbg_error("No active class");
               return FAILURE;
           }
        } else if (!EG(function_table)) {
			phpdbg_error("No function table loaded");
			return SUCCESS;
		} else {
		    func_table = EG(function_table);
		}

		if (zend_hash_find(func_table, func_name, func_name_len+1, (void**)&fbc) == SUCCESS) {
		    phpdbg_notice(
	            "%s %s %s",
	            (fbc->type == ZEND_USER_FUNCTION) ? "User" : "Internal", 
	            (fbc->common.scope) ? "Method" : "Function",
	            fbc->common.function_name);
	            
			phpdbg_print_function_helper(fbc TSRMLS_CC);
		} else {
			phpdbg_error("Function %s not found", func_name);
		}
    } else {
        phpdbg_error("No function name provided");
    }
    return SUCCESS;
} /* }}} */
