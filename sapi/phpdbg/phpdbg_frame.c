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

#include "zend.h"
#include "phpdbg.h"
#include "phpdbg_utils.h"
#include "phpdbg_frame.h"
#include "phpdbg_list.h"
#include "zend_smart_str.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

static inline void phpdbg_append_individual_arg(smart_str *s, uint32_t i, zend_function *func, zval *arg) {
	const zend_arg_info *arginfo = func->common.arg_info;
	char *arg_name = NULL;

	if (i) {
		smart_str_appends(s, ", ");
	}
	if (i < func->common.num_args) {
		if (arginfo) {
			if (func->type == ZEND_INTERNAL_FUNCTION) {
				arg_name = (char *) ((zend_internal_arg_info *) &arginfo[i])->name;
			} else {
				arg_name = ZSTR_VAL(arginfo[i].name);
			}
		}
		smart_str_appends(s, arg_name ? arg_name : "?");
		smart_str_appendc(s, '=');
	}
	{
		char *arg_print = phpdbg_short_zval_print(arg, 40);
		smart_str_appends(s, arg_print);
		efree(arg_print);
	}
}

zend_string *phpdbg_compile_stackframe(zend_execute_data *ex) {
	smart_str s = {0};
	zend_op_array *op_array = &ex->func->op_array;
	uint32_t i = 0, first_extra_arg = op_array->num_args, num_args = ZEND_CALL_NUM_ARGS(ex);
	zval *p = ZEND_CALL_ARG(ex, 1);

	if (op_array->scope) {
		smart_str_append(&s, op_array->scope->name);
		smart_str_appends(&s, "::");
	}
	smart_str_append(&s, op_array->function_name);
	smart_str_appendc(&s, '(');
	if (ZEND_CALL_NUM_ARGS(ex) > first_extra_arg) {
		while (i < first_extra_arg) {
			phpdbg_append_individual_arg(&s, i, ex->func, p);
			p++;
			i++;
		}
		p = ZEND_CALL_VAR_NUM(ex, op_array->last_var + op_array->T);
	}
	while (i < num_args) {
		phpdbg_append_individual_arg(&s, i, ex->func, p);
		p++;
		i++;
	}
	smart_str_appendc(&s, ')');

	if (ex->func->type == ZEND_USER_FUNCTION) {
		smart_str_appends(&s, " at ");
		smart_str_append(&s, op_array->filename);
		smart_str_appendc(&s, ':');
		smart_str_append_unsigned(&s, ex->opline->lineno);
	} else {
		smart_str_appends(&s, " [internal function]");
	}

	return s.s;
}

void phpdbg_print_cur_frame_info(void) {
	const char *file_chr = zend_get_executed_filename();
	zend_string *file = zend_string_init(file_chr, strlen(file_chr), 0);

	phpdbg_list_file(file, 3, zend_get_executed_lineno() - 1, zend_get_executed_lineno());
	efree(file);
}

void phpdbg_restore_frame(void) /* {{{ */
{
	if (PHPDBG_FRAME(num) == 0) {
		return;
	}

	if (PHPDBG_FRAME(generator)) {
		if (PHPDBG_FRAME(generator)->execute_data->call) {
			PHPDBG_FRAME(generator)->frozen_call_stack = zend_generator_freeze_call_stack(PHPDBG_FRAME(generator)->execute_data);
		}
		PHPDBG_FRAME(generator) = NULL;
	}

	PHPDBG_FRAME(num) = 0;

	/* move things back */
	EG(current_execute_data) = PHPDBG_FRAME(execute_data);
} /* }}} */

void phpdbg_switch_frame(int frame) /* {{{ */
{
	zend_execute_data *execute_data = PHPDBG_FRAME(num) ? PHPDBG_FRAME(execute_data) : EG(current_execute_data);
	int i = 0;

	if (PHPDBG_FRAME(num) == frame) {
		phpdbg_notice("Already in frame #%d", frame);
		return;
	}

	phpdbg_try_access {
		while (execute_data) {
			if (i++ == frame) {
				break;
			}

			do {
				execute_data = execute_data->prev_execute_data;
			} while (execute_data && execute_data->opline == NULL);
		}
	} phpdbg_catch_access {
		phpdbg_error("Couldn't switch frames, invalid data source");
		return;
	} phpdbg_end_try_access();

	if (execute_data == NULL) {
		phpdbg_error("No frame #%d", frame);
		return;
	}

	phpdbg_restore_frame();

	if (frame > 0) {
		PHPDBG_FRAME(num) = frame;

		/* backup things and jump back */
		PHPDBG_FRAME(execute_data) = EG(current_execute_data);
		EG(current_execute_data) = execute_data;
	}

	phpdbg_try_access {
		zend_string *s = phpdbg_compile_stackframe(EG(current_execute_data));
		phpdbg_notice("Switched to frame #%d: %.*s", frame, (int) ZSTR_LEN(s), ZSTR_VAL(s));
		zend_string_release(s);
	} phpdbg_catch_access {
		phpdbg_notice("Switched to frame #%d", frame);
	} phpdbg_end_try_access();

	phpdbg_print_cur_frame_info();
} /* }}} */

static void phpdbg_dump_prototype(zval *tmp) /* {{{ */
{
	zval *funcname, *class, class_zv, *args, *argstmp;

	funcname = zend_hash_find(Z_ARRVAL_P(tmp), ZSTR_KNOWN(ZEND_STR_FUNCTION));

	if ((class = zend_hash_find(Z_ARRVAL_P(tmp), ZSTR_KNOWN(ZEND_STR_OBJECT)))) {
		ZVAL_NEW_STR(&class_zv, Z_OBJCE_P(class)->name);
		class = &class_zv;
	} else {
		class = zend_hash_find(Z_ARRVAL_P(tmp), ZSTR_KNOWN(ZEND_STR_CLASS));
	}

	if (class) {
		zval *type = zend_hash_find(Z_ARRVAL_P(tmp), ZSTR_KNOWN(ZEND_STR_TYPE));

		phpdbg_out("%s%s%s(", Z_STRVAL_P(class), Z_STRVAL_P(type), Z_STRVAL_P(funcname));
	} else {
		phpdbg_out("%s(", Z_STRVAL_P(funcname));
	}

	args = zend_hash_find(Z_ARRVAL_P(tmp), ZSTR_KNOWN(ZEND_STR_ARGS));

	if (args) {
		const zend_function *func = NULL;
		const zend_arg_info *arginfo = NULL;
		bool is_variadic = 0;
		int j = 0, m;

		phpdbg_try_access {
			/* assuming no autoloader call is necessary, class should have been loaded if it's in backtrace ... */
			if ((func = phpdbg_get_function(Z_STRVAL_P(funcname), class ? Z_STRVAL_P(class) : NULL))) {
				arginfo = func->common.arg_info;
			}
		} phpdbg_end_try_access();

		m = func ? func->common.num_args : 0;

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(args), argstmp) {
			if (j) {
				phpdbg_out(", ");
			}
			if (m && j < m) {
				char *arg_name = NULL;

				if (arginfo) {
					if (func->type == ZEND_INTERNAL_FUNCTION) {
						arg_name = (char *)((zend_internal_arg_info *)&arginfo[j])->name;
					} else {
						arg_name = ZSTR_VAL(arginfo[j].name);
					}
				}

				if (!is_variadic) {
					is_variadic = arginfo ? ZEND_ARG_IS_VARIADIC(&arginfo[j]) : 0;
				}

				phpdbg_out("%s=%s", arg_name ? arg_name : "?", is_variadic ? "[": "");

			}
			++j;

			{
				char *arg_print = phpdbg_short_zval_print(argstmp, 40);
				php_printf("%s", arg_print);
				efree(arg_print);
			}
		} ZEND_HASH_FOREACH_END();

		if (is_variadic) {
			phpdbg_out("]");
		}
	}
	phpdbg_out(")");
}

void phpdbg_dump_backtrace(size_t num) /* {{{ */
{
	HashPosition position;
	zval zbacktrace;
	zval *tmp;
	zval startline, startfile;
	const char *startfilename;
	zval *file = &startfile, *line = &startline;
	int i = 0, limit = num;

	PHPDBG_OUTPUT_BACKUP();

	if (limit < 0) {
		phpdbg_error("Invalid backtrace size %d", limit);

		PHPDBG_OUTPUT_BACKUP_RESTORE();
		return;
	}

	phpdbg_try_access {
		zend_fetch_debug_backtrace(&zbacktrace, 0, 0, limit);
	} phpdbg_catch_access {
		phpdbg_error("Couldn't fetch backtrace, invalid data source");
		return;
	} phpdbg_end_try_access();

	Z_LVAL(startline) = zend_get_executed_lineno();
	startfilename = zend_get_executed_filename();
	Z_STR(startfile) = zend_string_init(startfilename, strlen(startfilename), 0);

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL(zbacktrace), &position);
	tmp = zend_hash_get_current_data_ex(Z_ARRVAL(zbacktrace), &position);
	while ((tmp = zend_hash_get_current_data_ex(Z_ARRVAL(zbacktrace), &position))) {
		if (file) { /* userland */
			phpdbg_out("frame #%d: ", i);
			phpdbg_dump_prototype(tmp);
			phpdbg_out(" at %s:"ZEND_LONG_FMT"\n", Z_STRVAL_P(file), Z_LVAL_P(line));
			i++;
		} else {
			phpdbg_out(" => ");
			phpdbg_dump_prototype(tmp);
			phpdbg_out(" (internal function)\n");
		}

		file = zend_hash_find(Z_ARRVAL_P(tmp), ZSTR_KNOWN(ZEND_STR_FILE));
		line = zend_hash_find(Z_ARRVAL_P(tmp), ZSTR_KNOWN(ZEND_STR_LINE));
		zend_hash_move_forward_ex(Z_ARRVAL(zbacktrace), &position);
	}

	phpdbg_writeln("frame #%d: {main} at %s:"ZEND_LONG_FMT, i, Z_STRVAL_P(file), Z_LVAL_P(line));

	zval_ptr_dtor_nogc(&zbacktrace);
	zend_string_release(Z_STR(startfile));

	PHPDBG_OUTPUT_BACKUP_RESTORE();
} /* }}} */

void phpdbg_open_generator_frame(zend_generator *gen) {
	zend_string *s;

	if (EG(current_execute_data) == gen->execute_data) {
		return;
	}

	phpdbg_restore_frame();

	PHPDBG_FRAME(num) = -1;
	PHPDBG_FRAME(generator) = gen;

	EG(current_execute_data) = gen->execute_data;
	if (gen->frozen_call_stack) {
		zend_generator_restore_call_stack(gen);
	}
	gen->execute_data->prev_execute_data = NULL;

	s = phpdbg_compile_stackframe(EG(current_execute_data));
	phpdbg_notice("Switched to generator with handle #%d: %.*s", gen->std.handle, (int) ZSTR_LEN(s), ZSTR_VAL(s));
	zend_string_release(s);
	phpdbg_print_cur_frame_info();
}
