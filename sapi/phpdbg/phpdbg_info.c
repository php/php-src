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

#include "php.h"
#include "phpdbg.h"
#include "phpdbg_utils.h"
#include "phpdbg_info.h"
#include "phpdbg_bp.h"
#include "phpdbg_prompt.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

#define PHPDBG_INFO_COMMAND_D(f, h, a, m, l, s, flags) \
	PHPDBG_COMMAND_D_EXP(f, h, a, m, l, s, &phpdbg_prompt_commands[14], flags)

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
	phpdbg_print_breakpoints(PHPDBG_BREAK_FILE TSRMLS_CC);
	phpdbg_print_breakpoints(PHPDBG_BREAK_SYM TSRMLS_CC);
	phpdbg_print_breakpoints(PHPDBG_BREAK_METHOD TSRMLS_CC);
	phpdbg_print_breakpoints(PHPDBG_BREAK_OPLINE TSRMLS_CC);
	phpdbg_print_breakpoints(PHPDBG_BREAK_FILE_OPLINE TSRMLS_CC);
	phpdbg_print_breakpoints(PHPDBG_BREAK_FUNCTION_OPLINE TSRMLS_CC);
	phpdbg_print_breakpoints(PHPDBG_BREAK_METHOD_OPLINE TSRMLS_CC);
	phpdbg_print_breakpoints(PHPDBG_BREAK_COND TSRMLS_CC);
	phpdbg_print_breakpoints(PHPDBG_BREAK_OPCODE TSRMLS_CC);

	return SUCCESS;
} /* }}} */

PHPDBG_INFO(files) /* {{{ */
{
	HashPosition pos;
	char *fname;

	phpdbg_try_access {
		phpdbg_notice("includedfilecount", "num=\"%d\"", "Included files: %d", zend_hash_num_elements(&EG(included_files)));
	} phpdbg_catch_access {
		phpdbg_error("signalsegv", "", "Could not fetch included file count, invalid data source");
	} phpdbg_end_try_access();

	phpdbg_try_access {
		zend_hash_internal_pointer_reset_ex(&EG(included_files), &pos);
		while (zend_hash_get_current_key_ex(&EG(included_files), &fname, NULL, NULL, 0, &pos) == HASH_KEY_IS_STRING) {
			phpdbg_writeln("includedfile", "name=\"%s\"", "File: %s", fname);
			zend_hash_move_forward_ex(&EG(included_files), &pos);
		}
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
	HashPosition pos;
	HashTable consts;
	zend_constant *data;

	zend_hash_init(&consts, 8, NULL, NULL, 0);

	if (EG(zend_constants)) {
		phpdbg_try_access {
			zend_hash_internal_pointer_reset_ex(EG(zend_constants), &pos);
			while (zend_hash_get_current_data_ex(EG(zend_constants), (void **) &data, &pos) == SUCCESS) {
				if (data->module_number == PHP_USER_CONSTANT) {
					zend_hash_update(&consts, data->name, data->name_len, (void **) &data, sizeof(zend_constant *), NULL);
				}
				zend_hash_move_forward_ex(EG(zend_constants), &pos);
			}
		} phpdbg_catch_access {
			phpdbg_error("signalsegv", "", "Cannot fetch all the constants, invalid data source");
		} phpdbg_end_try_access();
	}

	phpdbg_notice("constantinfo", "num=\"%d\"", "User-defined constants (%d)", zend_hash_num_elements(&consts));

	if (zend_hash_num_elements(&consts)) {
		phpdbg_out("Address            Refs    Type      Constant\n");
		for (zend_hash_internal_pointer_reset_ex(&consts, &pos);
		     zend_hash_get_current_data_ex(&consts, (void **) &data, &pos) == SUCCESS;
		     zend_hash_move_forward_ex(&consts, &pos)) {
			data = *(zend_constant **) data;

#define VARIABLEINFO(attrs, msg, ...) phpdbg_writeln("constant", "address=\"%p\" refcount=\"%d\" type=\"%s\" name=\"%.*s\" " attrs, "%-18p %-7d %-9s %.*s" msg, &data->value, Z_REFCOUNT(data->value), zend_zval_type_name(&data->value), data->name_len - 1, data->name, ##__VA_ARGS__)

			switch (Z_TYPE(data->value)) {
				case IS_STRING:
					phpdbg_try_access {
						VARIABLEINFO("length=\"%d\" value=\"%.*s\"", "\nstring (%d) \"%.*s%s\"", Z_STRLEN(data->value), Z_STRLEN(data->value) < 255 ? Z_STRLEN(data->value) : 255, Z_STRVAL(data->value), Z_STRLEN(data->value) > 255 ? "..." : "");
					} phpdbg_catch_access {
						VARIABLEINFO("", "");
					} phpdbg_end_try_access();
					break;
				case IS_BOOL:
					VARIABLEINFO("value=\"%s\"", "\nbool (%s)", Z_LVAL(data->value) ? "true" : "false");
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
		}
	}

	return SUCCESS;
} /* }}} */

static int phpdbg_arm_auto_global(zend_auto_global *auto_global TSRMLS_DC) {
	if (auto_global->armed) {
		if (PHPDBG_G(flags) & PHPDBG_IN_SIGNAL_HANDLER) {
			phpdbg_notice("variableinfo", "unreachable=\"%.*s\"", "Cannot show information about superglobal variable %.*s", auto_global->name_len, auto_global->name);
		} else {
			auto_global->armed = auto_global->auto_global_callback(auto_global->name, auto_global->name_len TSRMLS_CC);
		}
	}

	return 0;
}

static int phpdbg_print_symbols(zend_bool show_globals TSRMLS_DC) {
	HashTable vars, *symtable;
	HashPosition pos;
	char *var;
	zval **data;

	if (!EG(active_op_array)) {
		phpdbg_error("inactive", "type=\"op_array\"", "No active op array!");
		return SUCCESS;
	}

	if (!EG(active_symbol_table)) {
		zend_rebuild_symbol_table(TSRMLS_C);

		if (!EG(active_symbol_table)) {
			phpdbg_error("inactive", "type=\"symbol_table\"", "No active symbol table!");
			return SUCCESS;
		}
	}


	if (show_globals) {
		/* that array should only be manipulated during init, so safe for async access during execution */
		zend_hash_apply(CG(auto_globals), (apply_func_t) phpdbg_arm_auto_global TSRMLS_CC);
		symtable = &EG(symbol_table);
	} else {
		symtable = EG(active_symbol_table);
	}

	zend_hash_init(&vars, 8, NULL, NULL, 0);

	phpdbg_try_access {
		zend_hash_internal_pointer_reset_ex(symtable, &pos);
		while (zend_hash_get_current_key_ex(symtable, &var, NULL, NULL, 0, &pos) == HASH_KEY_IS_STRING) {
			zend_hash_get_current_data_ex(symtable, (void **)&data, &pos);
			if (zend_is_auto_global(var, strlen(var) TSRMLS_CC) ^ !show_globals) {
				zend_hash_update(&vars, var, strlen(var)+1, (void**)data, sizeof(zval*), NULL);
			}
			zend_hash_move_forward_ex(symtable, &pos);
		}
	} phpdbg_catch_access {
		phpdbg_error("signalsegv", "", "Cannot fetch all data from the symbol table, invalid data source");
	} phpdbg_end_try_access();

	if (show_globals) {
		phpdbg_notice("variableinfo", "num=\"%d\"", "Superglobal variables (%d)", zend_hash_num_elements(&vars));
	} else {
		zend_op_array *ops = EG(active_op_array);

		if (ops->function_name) {
			if (ops->scope) {
				phpdbg_notice("variableinfo", "method=\"%s::%s\" num=\"%d\"", "Variables in %s::%s() (%d)", ops->scope->name, ops->function_name, zend_hash_num_elements(&vars));
			} else {
				phpdbg_notice("variableinfo", "function=\"%s\" num=\"%d\"", "Variables in %s() (%d)", ops->function_name, zend_hash_num_elements(&vars));
			}
		} else {
			if (ops->filename) {
				phpdbg_notice("variableinfo", "file=\"%s\" num=\"%d\"", "Variables in %s (%d)", ops->filename, zend_hash_num_elements(&vars));
			} else {
				phpdbg_notice("variableinfo", "opline=\"%p\" num=\"%d\"", "Variables @ %p (%d)", ops, zend_hash_num_elements(&vars));
			}
		}
	}

	if (zend_hash_num_elements(&vars)) {
		phpdbg_out("Address            Refs    Type      Variable\n");
		for (zend_hash_internal_pointer_reset_ex(&vars, &pos);
		     zend_hash_get_current_data_ex(&vars, (void**) &data, &pos) == SUCCESS;
		     zend_hash_move_forward_ex(&vars, &pos)) {
			char *var;
			zend_bool invalid_data = 1;

			zend_hash_get_current_key_ex(&vars, &var, NULL, NULL, 0, &pos);

			phpdbg_try_access {
				if (!(invalid_data = !*data)) {
#define VARIABLEINFO(attrs, msg, ...) phpdbg_writeln("variable", "address=\"%p\" refcount=\"%d\" type=\"%s\" refstatus=\"%s\" name=\"%s\" " attrs, "%-18p %-7d %-9s %s$%s" msg, *data, Z_REFCOUNT_PP(data), zend_zval_type_name(*data), Z_ISREF_PP(data) ? "&": "", var, ##__VA_ARGS__)

					switch (Z_TYPE_PP(data)) {
						case IS_RESOURCE:
							phpdbg_try_access {
								int type;
								VARIABLEINFO("type=\"%s\"", "\n|-------(typeof)------> (%s)\n", zend_list_find(Z_RESVAL_PP(data), &type) ? zend_rsrc_list_get_rsrc_type(type TSRMLS_CC) : "unknown");
							} phpdbg_catch_access {
								VARIABLEINFO("type=\"unknown\"", "\n|-------(typeof)------> (unknown)\n");
							} phpdbg_end_try_access();
							break;
						case IS_OBJECT:
							phpdbg_try_access {
								VARIABLEINFO("instanceof=\"%s\"", "\n|-----(instanceof)----> (%s)\n", Z_OBJCE_PP(data)->name);
							} phpdbg_catch_access {
								VARIABLEINFO("instanceof=\"%s\"", "\n|-----(instanceof)----> (unknown)\n");
							} phpdbg_end_try_access();
							break;
						case IS_STRING:
							phpdbg_try_access {
								VARIABLEINFO("length=\"%d\" value=\"%.*s\"", "\nstring (%d) \"%.*s%s\"", Z_STRLEN_PP(data), Z_STRLEN_PP(data) < 255 ? Z_STRLEN_PP(data) : 255, Z_STRVAL_PP(data), Z_STRLEN_PP(data) > 255 ? "..." : "");
							} phpdbg_catch_access {
								VARIABLEINFO("", "");
							} phpdbg_end_try_access();
							break;
						case IS_BOOL:
							VARIABLEINFO("value=\"%s\"", "\nbool (%s)", Z_LVAL_PP(data) ? "true" : "false");
							break;
						case IS_LONG:
							VARIABLEINFO("value=\"%ld\"", "\nint (%ld)", Z_LVAL_PP(data));
							break;
						case IS_DOUBLE:
							VARIABLEINFO("value=\"%lf\"", "\ndouble (%lf)", Z_DVAL_PP(data));
							break;
						default:
							VARIABLEINFO("", "");
					}
				}

#undef VARIABLEINFO
			} phpdbg_end_try_access();

			if (invalid_data) {
				phpdbg_writeln("variable", "name=\"%s\"", "n/a\tn/a\tn/a\t$%s", var);
			}
		}
	}

	zend_hash_destroy(&vars);

	return SUCCESS;
} /* }}} */

PHPDBG_INFO(vars) /* {{{ */
{
	return phpdbg_print_symbols(0 TSRMLS_CC);
}

PHPDBG_INFO(globals) /* {{{ */
{
	return phpdbg_print_symbols(1 TSRMLS_CC);
}

PHPDBG_INFO(literal) /* {{{ */
{
	/* literals are assumed to not be manipulated during executing of their op_array and as such async safe */
	if ((EG(in_execution) && EG(active_op_array)) || PHPDBG_G(ops)) {
		zend_op_array *ops = EG(active_op_array) ? EG(active_op_array) : PHPDBG_G(ops);
		int literal = 0, count = ops->last_literal-1;

		if (ops->function_name) {
			if (ops->scope) {
				phpdbg_notice("literalinfo", "method=\"%s::%s\" num=\"%d\"", "Literal Constants in %s::%s() (%d)", ops->scope->name, ops->function_name, count);
			} else {
				phpdbg_notice("literalinfo", "function=\"%s\" num=\"%d\"", "Literal Constants in %s() (%d)", ops->function_name, count);
			}
		} else {
			if (ops->filename) {
				phpdbg_notice("literalinfo", "file=\"%s\" num=\"%d\"", "Literal Constants in %s (%d)", ops->filename, count);
			} else {
				phpdbg_notice("literalinfo", "opline=\"%p\" num=\"%d\"", "Literal Constants @ %p (%d)", ops, count);
			}
		}

		while (literal < ops->last_literal) {
			if (Z_TYPE(ops->literals[literal].constant) != IS_NULL) {
				phpdbg_write("literal", "id=\"%u\"", "|-------- C%u -------> [", literal);
				zend_print_zval(&ops->literals[literal].constant, 0);
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
	zend_mm_heap *heap;
	zend_bool is_mm;

	if (PHPDBG_G(flags) & PHPDBG_IN_SIGNAL_HANDLER) {
		heap = zend_mm_set_heap(phpdbg_original_heap_sigsafe_mem(TSRMLS_C) TSRMLS_CC);
	}
	if ((is_mm = is_zend_mm(TSRMLS_C))) {
		used = zend_memory_usage(0 TSRMLS_CC);
		real = zend_memory_usage(1 TSRMLS_CC);
		peak_used = zend_memory_peak_usage(0 TSRMLS_CC);
		peak_real = zend_memory_peak_usage(1 TSRMLS_CC);
	}
	if (PHPDBG_G(flags) & PHPDBG_IN_SIGNAL_HANDLER) {
		zend_mm_set_heap(heap TSRMLS_CC);
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

static inline void phpdbg_print_class_name(zend_class_entry **ce TSRMLS_DC) /* {{{ */
{
	phpdbg_writeln("class", "type=\"%s\" flags=\"%s\" name=\"%s\" methodcount=\"%d\"", "%s %s %s (%d)",
		((*ce)->type == ZEND_USER_CLASS) ?
			"User" : "Internal",
		((*ce)->ce_flags & ZEND_ACC_INTERFACE) ?
			"Interface" :
			((*ce)->ce_flags & ZEND_ACC_ABSTRACT) ?
				"Abstract Class" :
					"Class",
		(*ce)->name, zend_hash_num_elements(&(*ce)->function_table));
} /* }}} */

PHPDBG_INFO(classes) /* {{{ */
{
	HashPosition position;
	zend_class_entry **ce;
	HashTable classes;

	zend_hash_init(&classes, 8, NULL, NULL, 0);

	phpdbg_try_access {
		for (zend_hash_internal_pointer_reset_ex(EG(class_table), &position);
		     zend_hash_get_current_data_ex(EG(class_table), (void**)&ce, &position) == SUCCESS;
		     zend_hash_move_forward_ex(EG(class_table), &position)) {
			if ((*ce)->type == ZEND_USER_CLASS) {
				zend_hash_next_index_insert(&classes, ce, sizeof(ce), NULL);
			}
		}
	} phpdbg_catch_access {
		phpdbg_notice("signalsegv", "", "Not all classes could be fetched, possibly invalid data source");
	} phpdbg_end_try_access();

	phpdbg_notice("classinfo", "num=\"%d\"", "User Classes (%d)", zend_hash_num_elements(&classes));

	/* once added, assume that classes are stable... until shutdown. */
	for (zend_hash_internal_pointer_reset_ex(&classes, &position);
	     zend_hash_get_current_data_ex(&classes, (void**)&ce, &position) == SUCCESS;
	     zend_hash_move_forward_ex(&classes, &position)) {

		phpdbg_print_class_name(ce TSRMLS_CC);

		if ((*ce)->parent) {
			zend_class_entry *pce;

			phpdbg_xml("<parents %r>");
			pce = (*ce)->parent;
			do {
				phpdbg_out("|-------- ");
				phpdbg_print_class_name(&pce TSRMLS_CC);
			} while ((pce = pce->parent));
			phpdbg_xml("</parents>");
		}

		if ((*ce)->info.user.filename) {
			phpdbg_writeln("classsource", "file=\"%s\" line=\"%u\"", "|---- in %s on line %u", (*ce)->info.user.filename, (*ce)->info.user.line_start);
		} else {
			phpdbg_writeln("classsource", "", "|---- no source code");
		}
	}

	zend_hash_destroy(&classes);

	return SUCCESS;
} /* }}} */

PHPDBG_INFO(funcs) /* {{{ */
{
	HashPosition position;
	zend_function *zf, **pzf;
	HashTable functions;

	zend_hash_init(&functions, 8, NULL, NULL, 0);

	phpdbg_try_access {
		for (zend_hash_internal_pointer_reset_ex(EG(function_table), &position);
		     zend_hash_get_current_data_ex(EG(function_table), (void**)&zf, &position) == SUCCESS;
		     zend_hash_move_forward_ex(EG(function_table), &position)) {
			if (zf->type == ZEND_USER_FUNCTION) {
				zend_hash_next_index_insert(&functions, (void**) &zf, sizeof(zend_function), NULL);
			}
		}
	} phpdbg_catch_access {
		phpdbg_notice("signalsegv", "", "Not all functions could be fetched, possibly invalid data source");
	} phpdbg_end_try_access();

	phpdbg_notice("functioninfo", "num=\"%d\"", "User Functions (%d)", zend_hash_num_elements(&functions));

	for (zend_hash_internal_pointer_reset_ex(&functions, &position);
		zend_hash_get_current_data_ex(&functions, (void**)&pzf, &position) == SUCCESS;
		zend_hash_move_forward_ex(&functions, &position)) {
		zend_op_array *op_array = &((*pzf)->op_array);

		phpdbg_write("function", "name=\"%s\"", "|-------- %s", op_array->function_name ? op_array->function_name : "{main}");

		if (op_array->filename) {
			phpdbg_writeln("functionsource", "file=\"%s\" line=\"%d\"", " in %s on line %d",
				op_array->filename,
				op_array->line_start);
		} else {
			phpdbg_writeln("functionsource", "", " (no source code)");
		}
	}

	zend_hash_destroy(&functions);

	return SUCCESS;
} /* }}} */
