/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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
#include "phpdbg_prompt.h"

#include "Optimizer/zend_dump.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

#define PHPDBG_PRINT_COMMAND_D(f, h, a, m, l, s, flags) \
	PHPDBG_COMMAND_D_EXP(f, h, a, m, l, s, &phpdbg_prompt_commands[8], flags)

const phpdbg_command_t phpdbg_print_commands[] = {
	PHPDBG_PRINT_COMMAND_D(exec,       "print out the instructions in the main execution context", 'e', print_exec,   NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_PRINT_COMMAND_D(opline,     "print out the instruction in the current opline",          'o', print_opline, NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_PRINT_COMMAND_D(class,      "print out the instructions in the specified class",        'c', print_class,  NULL, "s", PHPDBG_ASYNC_SAFE),
	PHPDBG_PRINT_COMMAND_D(method,     "print out the instructions in the specified method",       'm', print_method, NULL, "m", PHPDBG_ASYNC_SAFE),
	PHPDBG_PRINT_COMMAND_D(func,       "print out the instructions in the specified function",     'f', print_func,   NULL, "s", PHPDBG_ASYNC_SAFE),
	PHPDBG_PRINT_COMMAND_D(stack,      "print out the instructions in the current stack",          's', print_stack,  NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_END_COMMAND
};

PHPDBG_PRINT(opline) /* {{{ */
{
	if (PHPDBG_G(in_execution) && EG(current_execute_data)) {
		phpdbg_print_opline(phpdbg_user_execute_data(EG(current_execute_data)), 1);
	} else {
		phpdbg_error("Not Executing!");
	}

	return SUCCESS;
} /* }}} */

static inline void phpdbg_print_function_helper(zend_function *method) /* {{{ */
{
	switch (method->type) {
		case ZEND_USER_FUNCTION: {
			zend_op_array* op_array = &(method->op_array);

			if (op_array) {
				zend_dump_op_array(op_array, ZEND_DUMP_LINE_NUMBERS, NULL, NULL);

				for (uint32_t i = 0; i < op_array->num_dynamic_func_defs; i++) {
					zend_op_array *def = op_array->dynamic_func_defs[i];
					phpdbg_out("\ndynamic def: %i, function name: %.*s\n",
						i, (int) ZSTR_LEN(def->function_name), ZSTR_VAL(def->function_name));
					zend_dump_op_array(def, ZEND_DUMP_LINE_NUMBERS, NULL, NULL);
				}
			}
		} break;

		default: {
			if (method->common.scope) {
				phpdbg_writeln("\tInternal %s::%s()", ZSTR_VAL(method->common.scope->name), ZSTR_VAL(method->common.function_name));
			} else {
				phpdbg_writeln("\tInternal %s()", ZSTR_VAL(method->common.function_name));
			}
		}
	}
} /* }}} */

PHPDBG_PRINT(exec) /* {{{ */
{
	if (PHPDBG_G(exec)) {
		if (!PHPDBG_G(ops) && !(PHPDBG_G(flags) & PHPDBG_IN_SIGNAL_HANDLER)) {
			phpdbg_compile();
		}

		if (PHPDBG_G(ops)) {
			phpdbg_notice("Context %s (%d ops)", PHPDBG_G(exec), PHPDBG_G(ops)->last);

			phpdbg_print_function_helper((zend_function*) PHPDBG_G(ops));
		}
	} else {
		phpdbg_error("No execution context set");
	}

	return SUCCESS;
} /* }}} */

PHPDBG_PRINT(stack) /* {{{ */
{
	if (PHPDBG_G(in_execution) && EG(current_execute_data)) {
		zend_op_array *ops = &phpdbg_user_execute_data(EG(current_execute_data))->func->op_array;
		if (ops->function_name) {
			if (ops->scope) {
				phpdbg_notice("Stack in %s::%s() (%d ops)", ZSTR_VAL(ops->scope->name), ZSTR_VAL(ops->function_name), ops->last);
			} else {
				phpdbg_notice("Stack in %s() (%d ops)", ZSTR_VAL(ops->function_name), ops->last);
			}
		} else {
			if (ops->filename) {
				phpdbg_notice("Stack in %s (%d ops)", ZSTR_VAL(ops->filename), ops->last);
			} else {
				phpdbg_notice("Stack @ %p (%d ops)", ops, ops->last);
			}
		}
		phpdbg_print_function_helper((zend_function*) ops);
	} else {
		phpdbg_error("Not Executing!");
	}

	return SUCCESS;
} /* }}} */

PHPDBG_PRINT(class) /* {{{ */
{
	zend_class_entry *ce;

	if (phpdbg_safe_class_lookup(param->str, param->len, &ce) == SUCCESS) {
		phpdbg_notice("%s %s: %s (%d methods)",
			(ce->type == ZEND_USER_CLASS) ?
				"User" : "Internal",
			(ce->ce_flags & ZEND_ACC_INTERFACE) ?
				"Interface" :
				(ce->ce_flags & ZEND_ACC_ABSTRACT) ?
					"Abstract Class" :
					"Class",
			ZSTR_VAL(ce->name),
			zend_hash_num_elements(&ce->function_table));

		if (zend_hash_num_elements(&ce->function_table)) {
			zend_function *method;

			ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, method) {
				phpdbg_print_function_helper(method);
			} ZEND_HASH_FOREACH_END();
		}
	} else {
		phpdbg_error("The class %s could not be found", param->str);
	}

	return SUCCESS;
} /* }}} */

PHPDBG_PRINT(method) /* {{{ */
{
	zend_class_entry *ce;

	if (phpdbg_safe_class_lookup(param->method.class, strlen(param->method.class), &ce) == SUCCESS) {
		zend_function *fbc;
		zend_string *lcname = zend_string_alloc(strlen(param->method.name), 0);
		zend_str_tolower_copy(ZSTR_VAL(lcname), param->method.name, ZSTR_LEN(lcname));

		if ((fbc = zend_hash_find_ptr(&ce->function_table, lcname))) {
			phpdbg_notice("%s Method %s (%d ops)",
				(fbc->type == ZEND_USER_FUNCTION) ? "User" : "Internal",
				ZSTR_VAL(fbc->common.function_name),
				(fbc->type == ZEND_USER_FUNCTION) ? fbc->op_array.last : 0);

			phpdbg_print_function_helper(fbc);
		} else {
			phpdbg_error("The method %s::%s could not be found", param->method.class, param->method.name);
		}

		zend_string_release(lcname);
	} else {
		phpdbg_error("The class %s could not be found", param->method.class);
	}

	return SUCCESS;
} /* }}} */

PHPDBG_PRINT(func) /* {{{ */
{
	HashTable *func_table = EG(function_table);
	zend_function* fbc;
	const char *func_name = param->str;
	size_t func_name_len = param->len;
	zend_string *lcname;
	/* search active scope if begins with period */
	if (func_name[0] == '.') {
		zend_class_entry *scope = zend_get_executed_scope();

		if (scope) {
			func_name++;
			func_name_len--;

			func_table = &scope->function_table;
		} else {
			phpdbg_error("No active class");
			return SUCCESS;
		}
	} else if (!EG(function_table)) {
		phpdbg_error("No function table loaded");
		return SUCCESS;
	} else {
		func_table = EG(function_table);
	}

	lcname = zend_string_alloc(func_name_len, 0);
	zend_str_tolower_copy(ZSTR_VAL(lcname), func_name, ZSTR_LEN(lcname));

	phpdbg_try_access {
		if ((fbc = zend_hash_find_ptr(func_table, lcname))) {
			phpdbg_notice("%s %s %s (%d ops)",
				(fbc->type == ZEND_USER_FUNCTION) ? "User" : "Internal",
				(fbc->common.scope) ? "Method" : "Function",
				ZSTR_VAL(fbc->common.function_name),
				(fbc->type == ZEND_USER_FUNCTION) ? fbc->op_array.last : 0);

			phpdbg_print_function_helper(fbc);
		} else {
			phpdbg_error("The function %s could not be found", func_name);
		}
	} phpdbg_catch_access {
		phpdbg_error("Couldn't fetch function %.*s, invalid data source", (int) func_name_len, func_name);
	} phpdbg_end_try_access();

	efree(lcname);

	return SUCCESS;
} /* }}} */

void phpdbg_print_opcodes_main(void) {
	phpdbg_print_function_helper((zend_function *) PHPDBG_G(ops));
}

void phpdbg_print_opcodes_function(const char *function, size_t len) {
	zend_function *func = zend_hash_str_find_ptr(EG(function_table), function, len);

	if (!func) {
		phpdbg_error("The function %s could not be found", function);
		return;
	}

	phpdbg_print_function_helper(func);
}

static void phpdbg_print_opcodes_method_ce(zend_class_entry *ce, const char *function) {
	zend_function *func;

	if (ce->type != ZEND_USER_CLASS) {
		phpdbg_out("function name: %s::%s (internal)\n", ce->name->val, function);
		return;
	}

	if (!(func = zend_hash_str_find_ptr(&ce->function_table, function, strlen(function)))) {
		phpdbg_error("The method %s::%s could not be found", ZSTR_VAL(ce->name), function);
		return;
	}

	phpdbg_print_function_helper(func);
}

void phpdbg_print_opcodes_method(const char *class, const char *function) {
	zend_class_entry *ce;

	if (phpdbg_safe_class_lookup(class, strlen(class), &ce) != SUCCESS) {
		phpdbg_error("The class %s could not be found", class);
		return;
	}

	phpdbg_print_opcodes_method_ce(ce, function);
}

static void phpdbg_print_opcodes_ce(zend_class_entry *ce) {
	zend_function *method;
	bool first = 1;

	phpdbg_out("%s %s: %s\n",
		(ce->type == ZEND_USER_CLASS) ?
			"user" : "internal",
		(ce->ce_flags & ZEND_ACC_INTERFACE) ?
			"interface" :
			(ce->ce_flags & ZEND_ACC_ABSTRACT) ?
				"abstract Class" :
				"class",
		ZSTR_VAL(ce->name));

	if (ce->type != ZEND_USER_CLASS) {
		return;
	}

	phpdbg_out("%d methods: ", zend_hash_num_elements(&ce->function_table));
	ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, method) {
		if (first) {
			first = 0;
		} else {
			phpdbg_out(", ");
		}
		phpdbg_out("%s", ZSTR_VAL(method->common.function_name));
	} ZEND_HASH_FOREACH_END();
	if (first) {
		phpdbg_out("-");
	}
	phpdbg_out("\n");

	ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, method) {
		phpdbg_print_function_helper(method);
	} ZEND_HASH_FOREACH_END();
}

void phpdbg_print_opcodes_class(const char *class) {
	zend_class_entry *ce;

	if (phpdbg_safe_class_lookup(class, strlen(class), &ce) != SUCCESS) {
		phpdbg_error("The class %s could not be found", class);
		return;
	}

	phpdbg_print_opcodes_ce(ce);
}

void phpdbg_print_opcodes(const char *function)
{
	if (function == NULL) {
		phpdbg_print_opcodes_main();
	} else if (function[0] == '*' && function[1] == 0) {
		/* all */
		zend_string *name;
		zend_function *func;
		zend_class_entry *ce;

		phpdbg_print_opcodes_main();

		ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(EG(function_table), name, func) {
			if (func->type == ZEND_USER_FUNCTION) {
				phpdbg_print_opcodes_function(ZSTR_VAL(name), ZSTR_LEN(name));
			}
		} ZEND_HASH_FOREACH_END();

		ZEND_HASH_MAP_FOREACH_PTR(EG(class_table), ce) {
			if (ce->type == ZEND_USER_CLASS) {
				phpdbg_out("\n");
				phpdbg_print_opcodes_ce(ce);
			}
		} ZEND_HASH_FOREACH_END();
	} else {
		char *function_lowercase = zend_str_tolower_dup(function, strlen(function));

		if (strstr(function_lowercase, "::") == NULL) {
			phpdbg_print_opcodes_function(function_lowercase, strlen(function_lowercase));
		} else {
			char *method_name, *class_name = strtok(function_lowercase, "::");
			if ((method_name = strtok(NULL, "::")) == NULL) {
				phpdbg_print_opcodes_class(class_name);
			} else {
				phpdbg_print_opcodes_method(class_name, method_name);
			}
		}

		efree(function_lowercase);
	}
}

void phpdbg_print_opline(zend_execute_data *execute_data, bool ignore_flags) /* {{{ */
{
	if (ignore_flags || (!(PHPDBG_G(flags) & PHPDBG_IS_QUIET) && (PHPDBG_G(flags) & PHPDBG_IS_STEPPING))) {
		zend_dump_op_line(&EX(func)->op_array, NULL, EX(opline), ZEND_DUMP_LINE_NUMBERS, NULL);
	}

	if (PHPDBG_G(oplog_list)) {
		phpdbg_oplog_entry *cur = zend_arena_alloc(&PHPDBG_G(oplog_arena), sizeof(phpdbg_oplog_entry));
		zend_op_array *op_array = &EX(func)->op_array;
		cur->op = (zend_op *) EX(opline);
		cur->opcodes = op_array->opcodes;
		cur->filename = op_array->filename;
		cur->scope = op_array->scope;
		cur->function_name = op_array->function_name;
		cur->next = NULL;
		PHPDBG_G(oplog_cur)->next = cur;
		PHPDBG_G(oplog_cur) = cur;
	}
} /* }}} */
