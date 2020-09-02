/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

#include "php.h"
#include "phpdbg.h"
#include "phpdbg_utils.h"
#include "phpdbg_info.h"
#include "phpdbg_bp.h"
#include "phpdbg_prompt.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

#define PHPDBG_INFO_COMMAND_D(f, h, a, m, l, s, flags) \
	PHPDBG_COMMAND_D_EXP(f, h, a, m, l, s, &phpdbg_prompt_commands[13], flags)

const phpdbg_command_t phpdbg_info_commands[] = {
	PHPDBG_INFO_COMMAND_D(break,     "show breakpoints",              'b', info_break,     NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_INFO_COMMAND_D(files,     "show included files",           'F', info_files,     NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_INFO_COMMAND_D(classes,   "show loaded classes",           'c', info_classes,   NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_INFO_COMMAND_D(funcs,     "show loaded classes",           'f', info_funcs,     NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_INFO_COMMAND_D(error,     "show last error",               'e', info_error,     NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_INFO_COMMAND_D(constants, "show user defined constants",   'd', info_constants, NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_INFO_COMMAND_D(vars,      "show active variables",         'v', info_vars,      NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_INFO_COMMAND_D(globals,   "show superglobals",             'g', info_globals,   NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_INFO_COMMAND_D(literal,   "show active literal constants", 'l', info_literal,   NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_INFO_COMMAND_D(memory,    "show memory manager stats",     'm', info_memory,    NULL, 0, PHPDBG_ASYNC_SAFE),
	PHPDBG_END_COMMAND
};

PHPDBG_INFO(break) /* {{{ */
{
	phpdbg_print_breakpoints(PHPDBG_BREAK_FILE);
	phpdbg_print_breakpoints(PHPDBG_BREAK_SYM);
	phpdbg_print_breakpoints(PHPDBG_BREAK_METHOD);
	phpdbg_print_breakpoints(PHPDBG_BREAK_OPLINE);
	phpdbg_print_breakpoints(PHPDBG_BREAK_FILE_OPLINE);
	phpdbg_print_breakpoints(PHPDBG_BREAK_FUNCTION_OPLINE);
	phpdbg_print_breakpoints(PHPDBG_BREAK_METHOD_OPLINE);
	phpdbg_print_breakpoints(PHPDBG_BREAK_COND);
	phpdbg_print_breakpoints(PHPDBG_BREAK_OPCODE);

	return SUCCESS;
} /* }}} */

PHPDBG_INFO(files) /* {{{ */
{
	zend_string *fname;

	phpdbg_try_access {
		phpdbg_notice("includedfilecount", "num=\"%d\"", "Included files: %d", zend_hash_num_elements(&EG(included_files)));
	} phpdbg_catch_access {
		phpdbg_error("signalsegv", "", "Could not fetch included file count, invalid data source");
		return SUCCESS;
	} phpdbg_end_try_access();

	phpdbg_try_access {
		ZEND_HASH_FOREACH_STR_KEY(&EG(included_files), fname) {
			phpdbg_writeln("includedfile", "name=\"%s\"", "File: %s", ZSTR_VAL(fname));
		} ZEND_HASH_FOREACH_END();
	} phpdbg_catch_access {
		phpdbg_error("signalsegv", "", "Could not fetch file name, invalid data source, aborting included file listing");
	} phpdbg_end_try_access();

	return SUCCESS;
} /* }}} */

PHPDBG_INFO(error) /* {{{ */
{
	if (PG(last_error_message)) {
		phpdbg_try_access {
			phpdbg_writeln("lasterror", "error=\"%s\" file=\"%s\" line=\"%d\"", "Last error: %s at %s line %d", PG(last_error_message), PG(last_error_file), PG(last_error_lineno));
		} phpdbg_catch_access {
			phpdbg_notice("lasterror", "error=\"\"", "No error found!");
		} phpdbg_end_try_access();
	} else {
		phpdbg_notice("lasterror", "error=\"\"", "No error found!");
	}
	return SUCCESS;
} /* }}} */

PHPDBG_INFO(constants) /* {{{ */
{
	HashTable consts;
	zend_constant *data;

	zend_hash_init(&consts, 8, NULL, NULL, 0);

	if (EG(zend_constants)) {
		phpdbg_try_access {
			ZEND_HASH_FOREACH_PTR(EG(zend_constants), data) {
				if (ZEND_CONSTANT_MODULE_NUMBER(data) == PHP_USER_CONSTANT) {
					zend_hash_update_ptr(&consts, data->name, data);
				}
			} ZEND_HASH_FOREACH_END();
		} phpdbg_catch_access {
			phpdbg_error("signalsegv", "", "Cannot fetch all the constants, invalid data source");
		} phpdbg_end_try_access();
	}

	phpdbg_notice("constantinfo", "num=\"%d\"", "User-defined constants (%d)", zend_hash_num_elements(&consts));

	if (zend_hash_num_elements(&consts)) {
		phpdbg_out("Address            Refs    Type      Constant\n");
		ZEND_HASH_FOREACH_PTR(&consts, data) {

#define VARIABLEINFO(attrs, msg, ...) \
	phpdbg_writeln("constant", \
		"address=\"%p\" refcount=\"%d\" type=\"%s\" name=\"%.*s\" " attrs, \
		"%-18p %-7d %-9s %.*s" msg, &data->value, \
		Z_REFCOUNTED(data->value) ? Z_REFCOUNT(data->value) : 1, \
		zend_get_type_by_const(Z_TYPE(data->value)), \
		(int) ZSTR_LEN(data->name), ZSTR_VAL(data->name), ##__VA_ARGS__)

			switch (Z_TYPE(data->value)) {
				case IS_STRING:
					phpdbg_try_access {
						VARIABLEINFO("length=\"%zd\" value=\"%.*s\"", "\nstring (%zd) \"%.*s%s\"", Z_STRLEN(data->value), Z_STRLEN(data->value) < 255 ? (int) Z_STRLEN(data->value) : 255, Z_STRVAL(data->value), Z_STRLEN(data->value) > 255 ? "..." : "");
					} phpdbg_catch_access {
						VARIABLEINFO("", "");
					} phpdbg_end_try_access();
					break;
				case IS_TRUE:
					VARIABLEINFO("value=\"true\"", "\nbool (true)");
					break;
				case IS_FALSE:
					VARIABLEINFO("value=\"false\"", "\nbool (false)");
					break;
				case IS_LONG:
					VARIABLEINFO("value=\"%ld\"", "\nint (%ld)", Z_LVAL(data->value));
					break;
				case IS_DOUBLE:
					VARIABLEINFO("value=\"%lf\"", "\ndouble (%lf)", Z_DVAL(data->value));
					break;
				default:
					VARIABLEINFO("", "");

#undef VARIABLEINFO
			}
		} ZEND_HASH_FOREACH_END();
	}

	return SUCCESS;
} /* }}} */

static int phpdbg_arm_auto_global(zval *ptrzv) {
	zend_auto_global *auto_global = Z_PTR_P(ptrzv);

	if (auto_global->armed) {
		if (PHPDBG_G(flags) & PHPDBG_IN_SIGNAL_HANDLER) {
			phpdbg_notice("variableinfo", "unreachable=\"%.*s\"", "Cannot show information about superglobal variable %.*s", (int) ZSTR_LEN(auto_global->name), ZSTR_VAL(auto_global->name));
		} else {
			auto_global->armed = auto_global->auto_global_callback(auto_global->name);
		}
	}

	return 0;
}

static int phpdbg_print_symbols(zend_bool show_globals) {
	HashTable vars;
	zend_array *symtable;
	zend_string *var;
	zval *data;

	if (!EG(current_execute_data) || !EG(current_execute_data)->func) {
		phpdbg_error("inactive", "type=\"op_array\"", "No active op array!");
		return SUCCESS;
	}

	if (show_globals) {
		/* that array should only be manipulated during init, so safe for async access during execution */
		zend_hash_apply(CG(auto_globals), (apply_func_t) phpdbg_arm_auto_global);
		symtable = &EG(symbol_table);
	} else if (!(symtable = zend_rebuild_symbol_table())) {
		phpdbg_error("inactive", "type=\"symbol_table\"", "No active symbol table!");
		return SUCCESS;
	}

	zend_hash_init(&vars, 8, NULL, NULL, 0);

	phpdbg_try_access {
		ZEND_HASH_FOREACH_STR_KEY_VAL(symtable, var, data) {
			if (zend_is_auto_global(var) ^ !show_globals) {
				zend_hash_update(&vars, var, data);
			}
		} ZEND_HASH_FOREACH_END();
	} phpdbg_catch_access {
		phpdbg_error("signalsegv", "", "Cannot fetch all data from the symbol table, invalid data source");
	} phpdbg_end_try_access();

	if (show_globals) {
		phpdbg_notice("variableinfo", "num=\"%d\"", "Superglobal variables (%d)", zend_hash_num_elements(&vars));
	} else {
		zend_op_array *ops = &EG(current_execute_data)->func->op_array;

		if (ops->function_name) {
			if (ops->scope) {
				phpdbg_notice("variableinfo", "method=\"%s::%s\" num=\"%d\"", "Variables in %s::%s() (%d)", ops->scope->name->val, ops->function_name->val, zend_hash_num_elements(&vars));
			} else {
				phpdbg_notice("variableinfo", "function=\"%s\" num=\"%d\"", "Variables in %s() (%d)", ZSTR_VAL(ops->function_name), zend_hash_num_elements(&vars));
			}
		} else {
			if (ops->filename) {
				phpdbg_notice("variableinfo", "file=\"%s\" num=\"%d\"", "Variables in %s (%d)", ZSTR_VAL(ops->filename), zend_hash_num_elements(&vars));
			} else {
				phpdbg_notice("variableinfo", "opline=\"%p\" num=\"%d\"", "Variables @ %p (%d)", ops, zend_hash_num_elements(&vars));
			}
		}
	}

	if (zend_hash_num_elements(&vars)) {
		phpdbg_out("Address            Refs    Type      Variable\n");
		ZEND_HASH_FOREACH_STR_KEY_VAL(&vars, var, data) {
			phpdbg_try_access {
				const char *isref = "";
#define VARIABLEINFO(attrs, msg, ...) \
	phpdbg_writeln("variable", \
		"address=\"%p\" refcount=\"%d\" type=\"%s\" refstatus=\"%s\" name=\"%.*s\" " attrs, \
		"%-18p %-7d %-9s %s$%.*s" msg, data, Z_REFCOUNTED_P(data) ? Z_REFCOUNT_P(data) : 1, zend_get_type_by_const(Z_TYPE_P(data)), isref, (int) ZSTR_LEN(var), ZSTR_VAL(var), ##__VA_ARGS__)
retry_switch:
				switch (Z_TYPE_P(data)) {
					case IS_RESOURCE:
						phpdbg_try_access {
							const char *type = zend_rsrc_list_get_rsrc_type(Z_RES_P(data));
							VARIABLEINFO("type=\"%s\"", "\n|-------(typeof)------> (%s)\n", type ? type : "unknown");
						} phpdbg_catch_access {
							VARIABLEINFO("type=\"unknown\"", "\n|-------(typeof)------> (unknown)\n");
						} phpdbg_end_try_access();
						break;
					case IS_OBJECT:
						phpdbg_try_access {
							VARIABLEINFO("instanceof=\"%s\"", "\n|-----(instanceof)----> (%s)\n", ZSTR_VAL(Z_OBJCE_P(data)->name));
						} phpdbg_catch_access {
							VARIABLEINFO("instanceof=\"%s\"", "\n|-----(instanceof)----> (unknown)\n");
						} phpdbg_end_try_access();
						break;
					case IS_STRING:
						phpdbg_try_access {
							VARIABLEINFO("length=\"%zd\" value=\"%.*s\"", "\nstring (%zd) \"%.*s%s\"", Z_STRLEN_P(data), Z_STRLEN_P(data) < 255 ? (int) Z_STRLEN_P(data) : 255, Z_STRVAL_P(data), Z_STRLEN_P(data) > 255 ? "..." : "");
						} phpdbg_catch_access {
							VARIABLEINFO("", "");
						} phpdbg_end_try_access();
						break;
					case IS_TRUE:
						VARIABLEINFO("value=\"true\"", "\nbool (true)");
						break;
					case IS_FALSE:
						VARIABLEINFO("value=\"false\"", "\nbool (false)");
						break;
					case IS_LONG:
						VARIABLEINFO("value=\"%ld\"", "\nint (%ld)", Z_LVAL_P(data));
						break;
					case IS_DOUBLE:
						VARIABLEINFO("value=\"%lf\"", "\ndouble (%lf)", Z_DVAL_P(data));
						break;
					case IS_REFERENCE:
						isref = "&";
						data = Z_REFVAL_P(data);
						goto retry_switch;
					case IS_INDIRECT:
						data = Z_INDIRECT_P(data);
						goto retry_switch;
					default:
						VARIABLEINFO("", "");
				}

#undef VARIABLEINFO
			} phpdbg_catch_access {
				phpdbg_writeln("variable", "address=\"%p\" name=\"%s\"", "%p\tn/a\tn/a\t$%s", data, ZSTR_VAL(var));
			} phpdbg_end_try_access();
		} ZEND_HASH_FOREACH_END();
	}

	zend_hash_destroy(&vars);

	return SUCCESS;
} /* }}} */

PHPDBG_INFO(vars) /* {{{ */
{
	return phpdbg_print_symbols(0);
}

PHPDBG_INFO(globals) /* {{{ */
{
	return phpdbg_print_symbols(1);
}

PHPDBG_INFO(literal) /* {{{ */
{
	/* literals are assumed to not be manipulated during executing of their op_array and as such async safe */
	zend_bool in_executor = PHPDBG_G(in_execution) && EG(current_execute_data) && EG(current_execute_data)->func;
	if (in_executor || PHPDBG_G(ops)) {
		zend_op_array *ops = in_executor ? &EG(current_execute_data)->func->op_array : PHPDBG_G(ops);
		int literal = 0, count = ops->last_literal - 1;

		if (ops->function_name) {
			if (ops->scope) {
				phpdbg_notice("literalinfo", "method=\"%s::%s\" num=\"%d\"", "Literal Constants in %s::%s() (%d)", ops->scope->name->val, ops->function_name->val, count);
			} else {
				phpdbg_notice("literalinfo", "function=\"%s\" num=\"%d\"", "Literal Constants in %s() (%d)", ops->function_name->val, count);
			}
		} else {
			if (ops->filename) {
				phpdbg_notice("literalinfo", "file=\"%s\" num=\"%d\"", "Literal Constants in %s (%d)", ZSTR_VAL(ops->filename), count);
			} else {
				phpdbg_notice("literalinfo", "opline=\"%p\" num=\"%d\"", "Literal Constants @ %p (%d)", ops, count);
			}
		}

		while (literal < ops->last_literal) {
			if (Z_TYPE(ops->literals[literal]) != IS_NULL) {
				phpdbg_write("literal", "id=\"%u\"", "|-------- C%u -------> [", literal);
				zend_print_zval(&ops->literals[literal], 0);
				phpdbg_out("]\n");
			}
			literal++;
		}
	} else {
		phpdbg_error("inactive", "type=\"execution\"", "Not executing!");
	}

	return SUCCESS;
} /* }}} */

PHPDBG_INFO(memory) /* {{{ */
{
	size_t used, real, peak_used, peak_real;
	zend_mm_heap *orig_heap = NULL;
	zend_bool is_mm;

	if (PHPDBG_G(flags) & PHPDBG_IN_SIGNAL_HANDLER) {
		orig_heap = zend_mm_set_heap(phpdbg_original_heap_sigsafe_mem());
	}
	if ((is_mm = is_zend_mm())) {
		used = zend_memory_usage(0);
		real = zend_memory_usage(1);
		peak_used = zend_memory_peak_usage(0);
		peak_real = zend_memory_peak_usage(1);
	}
	if (orig_heap) {
		zend_mm_set_heap(orig_heap);
	}

	if (is_mm) {
		phpdbg_notice("meminfo", "", "Memory Manager Information");
		phpdbg_notice("current", "", "Current");
		phpdbg_writeln("used", "mem=\"%.3f\"", "|-------> Used:\t%.3f kB", (float) (used / 1024));
		phpdbg_writeln("real", "mem=\"%.3f\"", "|-------> Real:\t%.3f kB", (float) (real / 1024));
		phpdbg_notice("peak", "", "Peak");
		phpdbg_writeln("used", "mem=\"%.3f\"", "|-------> Used:\t%.3f kB", (float) (peak_used / 1024));
		phpdbg_writeln("real", "mem=\"%.3f\"", "|-------> Real:\t%.3f kB", (float) (peak_real / 1024));
	} else {
		phpdbg_error("inactive", "type=\"memory_manager\"", "Memory Manager Disabled!");
	}
	return SUCCESS;
} /* }}} */

static inline void phpdbg_print_class_name(zend_class_entry *ce) /* {{{ */
{
	const char *visibility = ce->type == ZEND_USER_CLASS ? "User" : "Internal";
	const char *type = (ce->ce_flags & ZEND_ACC_INTERFACE) ? "Interface" : (ce->ce_flags & ZEND_ACC_ABSTRACT) ? "Abstract Class" : "Class";

	phpdbg_writeln("class", "type=\"%s\" flags=\"%s\" name=\"%.*s\" methodcount=\"%d\"", "%s %s %.*s (%d)", visibility, type, (int) ZSTR_LEN(ce->name), ZSTR_VAL(ce->name), zend_hash_num_elements(&ce->function_table));
} /* }}} */

PHPDBG_INFO(classes) /* {{{ */
{
	zend_class_entry *ce;
	HashTable classes;

	zend_hash_init(&classes, 8, NULL, NULL, 0);

	phpdbg_try_access {
		ZEND_HASH_FOREACH_PTR(EG(class_table), ce) {
			if (ce->type == ZEND_USER_CLASS) {
				zend_hash_next_index_insert_ptr(&classes, ce);
			}
		} ZEND_HASH_FOREACH_END();
	} phpdbg_catch_access {
		phpdbg_notice("signalsegv", "", "Not all classes could be fetched, possibly invalid data source");
	} phpdbg_end_try_access();

	phpdbg_notice("classinfo", "num=\"%d\"", "User Classes (%d)", zend_hash_num_elements(&classes));

	/* once added, assume that classes are stable... until shutdown. */
	ZEND_HASH_FOREACH_PTR(&classes, ce) {
		phpdbg_print_class_name(ce);

		if (ce->parent) {
			zend_class_entry *pce;
			phpdbg_xml("<parents %r>");
			pce = ce->parent;
			do {
				phpdbg_out("|-------- ");
				phpdbg_print_class_name(pce);
			} while ((pce = pce->parent));
			phpdbg_xml("</parents>");
		}

		if (ce->info.user.filename) {
			phpdbg_writeln("classsource", "file=\"%s\" line=\"%u\"", "|---- in %s on line %u", ZSTR_VAL(ce->info.user.filename), ce->info.user.line_start);
		} else {
			phpdbg_writeln("classsource", "", "|---- no source code");
		}
	} ZEND_HASH_FOREACH_END();

	zend_hash_destroy(&classes);

	return SUCCESS;
} /* }}} */

PHPDBG_INFO(funcs) /* {{{ */
{
	zend_function *zf;
	HashTable functions;

	zend_hash_init(&functions, 8, NULL, NULL, 0);

	phpdbg_try_access {
		ZEND_HASH_FOREACH_PTR(EG(function_table), zf) {
			if (zf->type == ZEND_USER_FUNCTION) {
				zend_hash_next_index_insert_ptr(&functions, zf);
			}
		} ZEND_HASH_FOREACH_END();
	} phpdbg_catch_access {
		phpdbg_notice("signalsegv", "", "Not all functions could be fetched, possibly invalid data source");
	} phpdbg_end_try_access();

	phpdbg_notice("functioninfo", "num=\"%d\"", "User Functions (%d)", zend_hash_num_elements(&functions));

	ZEND_HASH_FOREACH_PTR(&functions, zf) {
		zend_op_array *op_array = &zf->op_array;

		phpdbg_write("function", "name=\"%s\"", "|-------- %s", op_array->function_name ? ZSTR_VAL(op_array->function_name) : "{main}");

		if (op_array->filename) {
			phpdbg_writeln("functionsource", "file=\"%s\" line=\"%d\"", " in %s on line %d", ZSTR_VAL(op_array->filename), op_array->line_start);
		} else {
			phpdbg_writeln("functionsource", "", " (no source code)");
		}
	} ZEND_HASH_FOREACH_END();

	zend_hash_destroy(&functions);

	return SUCCESS;
} /* }}} */
