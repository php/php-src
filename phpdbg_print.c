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
#include "phpdbg_prompt.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

#define PHPDBG_PRINT_COMMAND_D(f, h, a, m, l, s, flags) \
	PHPDBG_COMMAND_D_EXP(f, h, a, m, l, s, &phpdbg_prompt_commands[9], flags)

const phpdbg_command_t phpdbg_print_commands[] = {
	PHPDBG_PRINT_COMMAND_D(exec,       "print out the instructions in the execution context",  'e', print_exec,   NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_PRINT_COMMAND_D(opline,     "print out the instruction in the current opline",      'o', print_opline, NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_PRINT_COMMAND_D(class,      "print out the instructions in the specified class",    'c', print_class,  NULL, "s", PHPDBG_ASYNC_SAFE),
	PHPDBG_PRINT_COMMAND_D(method,     "print out the instructions in the specified method",   'm', print_method, NULL, "m", PHPDBG_ASYNC_SAFE),
	PHPDBG_PRINT_COMMAND_D(func,       "print out the instructions in the specified function", 'f', print_func,   NULL, "s", PHPDBG_ASYNC_SAFE),
	PHPDBG_PRINT_COMMAND_D(stack,      "print out the instructions in the current stack",      's', print_stack,  NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_END_COMMAND
};

PHPDBG_PRINT(opline) /* {{{ */
{
	if (EG(in_execution) && EG(current_execute_data)) {
		phpdbg_print_opline(EG(current_execute_data), 1 TSRMLS_CC);
	} else {
		phpdbg_error("inactive", "type=\"execution\"", "Not Executing!");
	}

	return SUCCESS;
} /* }}} */

static inline void phpdbg_print_function_helper(zend_function *method TSRMLS_DC) /* {{{ */
{
	switch (method->type) {
		case ZEND_USER_FUNCTION: {
			zend_op_array* op_array = &(method->op_array);
			HashTable vars;

			if (op_array) {
				zend_op *opline = &(op_array->opcodes[0]);
				zend_uint opcode = 0,
				end = op_array->last-1;

				if (method->common.scope) {
					phpdbg_writeln("printoplineinfo", "type=\"User\" startline=\"%d\" endline=\"%d\" method=\"%s::%s\" file=\"%s\"", "\tL%d-%d %s::%s() %s",
						op_array->line_start,
						op_array->line_end,
						method->common.scope->name,
						method->common.function_name,
						op_array->filename ? op_array->filename : "unknown");
				} else {
					phpdbg_writeln("printoplineinfo", "type=\"User\" startline=\"%d\" endline=\"%d\" function=\"%s\" file=\"%s\"", "\tL%d-%d %s() %s",
						method->common.function_name ? op_array->line_start : 0,
						method->common.function_name ? op_array->line_end : 0,
						method->common.function_name ? method->common.function_name : "{main}",
						op_array->filename ? op_array->filename : "unknown");
				}

				zend_hash_init(&vars, op_array->last, NULL, NULL, 0);
				do {
					char *decode = phpdbg_decode_opline(op_array, opline, &vars TSRMLS_CC);
					if (decode != NULL) {
						phpdbg_writeln("print", "line=\"%u\" opline=\"%p\" opcode=\"%s\" op=\"%s\"", "\t\tL%u\t%p %-30s %s",
							opline->lineno,
							opline,
							phpdbg_decode_opcode(opline->opcode),
							decode);
						free(decode);
					} else {
						phpdbg_error("print", "type=\"decodefailure\" opline=\"%16p\"", "\tFailed to decode opline %16p", opline);
					}
					opline++;
				} while (opcode++ < end);
				zend_hash_destroy(&vars);
			}
		} break;

		default: {
			if (method->common.scope) {
				phpdbg_writeln("printoplineinfo", "type=\"Internal\" method=\"%s::%s\"", "Internal %s::%s()", method->common.scope->name, method->common.function_name);
			} else {
				phpdbg_writeln("printoplineinfo", "type=\"Internal\" function=\"%s\"", "\tInternal %s()", method->common.function_name);
			}
		}
	}
} /* }}} */

PHPDBG_PRINT(exec) /* {{{ */
{
	if (PHPDBG_G(exec)) {
		if (!PHPDBG_G(ops) && !(PHPDBG_G(flags) & PHPDBG_IN_SIGNAL_HANDLER)) {
			phpdbg_compile(TSRMLS_C);
		}

		if (PHPDBG_G(ops)) {
			phpdbg_notice("printinfo", "file=\"%s\" num=\"%d\"", "Context %s (%d ops)", PHPDBG_G(exec), PHPDBG_G(ops)->last);

			phpdbg_print_function_helper((zend_function*) PHPDBG_G(ops) TSRMLS_CC);
		}
	} else {
		phpdbg_error("inactive", "type=\"nocontext\"", "No execution context set");
	}

return SUCCESS;
} /* }}} */

PHPDBG_PRINT(stack) /* {{{ */
{
	zend_op_array *ops = EG(active_op_array);

	if (EG(in_execution) && ops) {
		if (ops->function_name) {
			if (ops->scope) {
				phpdbg_notice("printinfo", "method=\"%s::%s\" num=\"%d\"", "Stack in %s::%s() (%d ops)", ops->scope->name, ops->function_name, ops->last);
			} else {
				phpdbg_notice("printinfo", "function=\"%s\" num=\"%d\"", "Stack in %s() (%d ops)", ops->function_name, ops->last);
			}
		} else {
			if (ops->filename) {
				phpdbg_notice("printinfo", "file=\"%s\" num=\"%d\"", "Stack in %s (%d ops)", ops->filename, ops->last);
			} else {
				phpdbg_notice("printinfo", "opline=\"%p\" num=\"%d\"", "Stack @ %p (%d ops)", ops, ops->last);
			}
		}
		phpdbg_print_function_helper((zend_function*) ops TSRMLS_CC);
	} else {
		phpdbg_error("inactive", "type=\"execution\"", "Not Executing!");
	}

	return SUCCESS;
} /* }}} */

PHPDBG_PRINT(class) /* {{{ */
{
	zend_class_entry **ce;

	if (phpdbg_safe_class_lookup(param->str, param->len, &ce TSRMLS_CC) == SUCCESS) {
		phpdbg_notice("printinfo", "type=\"%s\" flag=\"%s\" class=\"%s\" num=\"%d\"", "%s %s: %s (%d methods)",
			((*ce)->type == ZEND_USER_CLASS) ?
				"User" : "Internal",
			((*ce)->ce_flags & ZEND_ACC_INTERFACE) ?
				"Interface" :
				((*ce)->ce_flags & ZEND_ACC_ABSTRACT) ?
					"Abstract Class" :
					"Class",
			(*ce)->name,
			zend_hash_num_elements(&(*ce)->function_table));

		phpdbg_xml("<printmethods %r>");

		if (zend_hash_num_elements(&(*ce)->function_table)) {
			HashPosition position;
			zend_function *method;

			for (zend_hash_internal_pointer_reset_ex(&(*ce)->function_table, &position);
			     zend_hash_get_current_data_ex(&(*ce)->function_table, (void**) &method, &position) == SUCCESS;
			     zend_hash_move_forward_ex(&(*ce)->function_table, &position)) {
				phpdbg_print_function_helper(method TSRMLS_CC);
			}
		}

		phpdbg_xml("</printmethods>");
	} else {
		phpdbg_error("print", "type=\"noclass\" class=\"%s\"", "The class %s could not be found", param->str);
	}

	return SUCCESS;
} /* }}} */

PHPDBG_PRINT(method) /* {{{ */
{
	zend_class_entry **ce;

	if (phpdbg_safe_class_lookup(param->method.class, strlen(param->method.class), &ce TSRMLS_CC) == SUCCESS) {
		zend_function *fbc;
		char *lcname = zend_str_tolower_dup(param->method.name, strlen(param->method.name));

		if (zend_hash_find(&(*ce)->function_table, lcname, strlen(lcname)+1, (void**)&fbc) == SUCCESS) {
			phpdbg_notice("printinfo", "type=\"%s\" flags=\"Method\" symbol=\"%s\" num=\"%d\"", "%s Method %s (%d ops)",
				(fbc->type == ZEND_USER_FUNCTION) ? "User" : "Internal",
				fbc->common.function_name,
				(fbc->type == ZEND_USER_FUNCTION) ? fbc->op_array.last : 0);

			phpdbg_print_function_helper(fbc TSRMLS_CC);
		} else {
			phpdbg_error("print", "type=\"nomethod\" method=\"%s::%s\"", "The method %s::%s could not be found", param->method.class, param->method.name);
		}

		efree(lcname);
	} else {
		phpdbg_error("print", "type=\"noclass\" class=\"%s\"", "The class %s could not be found", param->method.class);
	}

	return SUCCESS;
} /* }}} */

PHPDBG_PRINT(func) /* {{{ */
{
	HashTable *func_table = EG(function_table);
	zend_function* fbc;
	const char *func_name = param->str;
	size_t func_name_len = param->len;
	char *lcname;
	/* search active scope if begins with period */
	if (func_name[0] == '.') {
		if (EG(scope)) {
			func_name++;
			func_name_len--;

			func_table = &EG(scope)->function_table;
		} else {
			phpdbg_error("inactive", "type=\"noclasses\"", "No active class");
			return SUCCESS;
		}
	} else if (!EG(function_table)) {
		phpdbg_error("inactive", "type=\"function_table\"", "No function table loaded");
		return SUCCESS;
	} else {
		func_table = EG(function_table);
	}

	lcname = zend_str_tolower_dup(func_name, func_name_len);

	phpdbg_try_access {
		if (zend_hash_find(func_table, lcname, func_name_len + 1, (void **) &fbc) == SUCCESS) {
			phpdbg_notice("printinfo", "type=\"%s\" flags=\"%s\" symbol=\"%s\" num=\"%d\"", "%s %s %s (%d ops)",
				(fbc->type == ZEND_USER_FUNCTION) ? "User" : "Internal",
				(fbc->common.scope) ? "Method" : "Function",
				fbc->common.function_name,
				(fbc->type == ZEND_USER_FUNCTION) ? fbc->op_array.last : 0);

			phpdbg_print_function_helper(fbc TSRMLS_CC);
		} else {
			phpdbg_error("print", "type=\"nofunction\" function=\"%s\"", "The function %s could not be found", func_name);
		}
	} phpdbg_catch_access {
		phpdbg_error("signalsegv", "function=\"%.*s\"", "Couldn't fetch function %.*s, invalid data source", (int) func_name_len, func_name);
	} phpdbg_end_try_access();

	efree(lcname);

	return SUCCESS;
} /* }}} */
