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
	                 switch (method->type) {
	                    case ZEND_USER_FUNCTION: {
	                        zend_op_array* op_array = &method->op_array;
   
                            if (op_array) {
                                zend_op     *opline = &op_array->opcodes[0];
                                zend_uint   opcode = 0,
                                            end =  op_array->last-1;

                                 phpdbg_writeln(
                                    "\t%s::%s() in %s:%d-%d", 
                                    (*ce)->name, method->common.function_name,
                                    op_array->filename ? op_array->filename : "unknown",
                                    op_array->line_start, op_array->line_end);
                                
                                do {
                                    char *decode = phpdbg_decode_opcode(opline->opcode);
                                    if (decode != NULL) {
                                        phpdbg_writeln(
                                            "\t\t%p:%s", opline, decode); 
                                    } else phpdbg_error("\tFailed to decode opline @ %ld", opline);
                                    
                                    opline++;
                                } while (++opcode < end);
                            }	
	                    } break;
	                    
	                    default: {
	                        phpdbg_writeln(
	                            "\tInternal Method %s::%s()", (*ce)->name, method->common.function_name);
	                    }
	                 }
	            }
	        }
	    } else {
	        phpdbg_error("Cannot find class %s/%lu", expr, expr_len);
	    }
	} else {
		phpdbg_error("No class name provided!");
	}

	return SUCCESS;
} /* }}} */
