/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
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

#include "zend.h"
#include "phpdbg.h"
#include "phpdbg_utils.h"
#include "phpdbg_frame.h"
#include "phpdbg_list.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

void phpdbg_restore_frame(void) /* {{{ */
{
	if (PHPDBG_FRAME(num) == 0) {
		return;
	}

	PHPDBG_FRAME(num) = 0;

	/* move things back */
	EG(current_execute_data) = PHPDBG_FRAME(execute_data);

	EG(scope) = PHPDBG_EX(func)->op_array.scope;
} /* }}} */

void phpdbg_switch_frame(int frame) /* {{{ */
{
	zend_execute_data *execute_data = PHPDBG_FRAME(num)?PHPDBG_FRAME(execute_data):EG(current_execute_data);
	int i = 0;

	if (PHPDBG_FRAME(num) == frame) {
		phpdbg_notice("frame", "id=\"%d\"", "Already in frame #%d", frame);
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
		phpdbg_error("signalsegv", "", "Couldn't switch frames, invalid data source");
		return;
	} phpdbg_end_try_access();

	if (execute_data == NULL) {
		phpdbg_error("frame", "type=\"maxnum\" id=\"%d\"", "No frame #%d", frame);
		return;
	}

	phpdbg_restore_frame();

	if (frame > 0) {
		PHPDBG_FRAME(num) = frame;

		/* backup things and jump back */
		PHPDBG_FRAME(execute_data) = EG(current_execute_data);
		EG(current_execute_data) = execute_data;

		EG(scope) = PHPDBG_EX(func)->op_array.scope;
	}

	phpdbg_notice("frame", "id=\"%d\"", "Switched to frame #%d", frame);

	{
		const char *file_chr = zend_get_executed_filename();
		zend_string *file = zend_string_init(file_chr, strlen(file_chr), 0);
		phpdbg_list_file(file, 3, zend_get_executed_lineno() - 1, zend_get_executed_lineno());
		efree(file);
	}
} /* }}} */

static void phpdbg_dump_prototype(zval *tmp) /* {{{ */
{
	zval *funcname, *class, class_zv, *type, *args, *argstmp;

	funcname = zend_hash_str_find(Z_ARRVAL_P(tmp), ZEND_STRL("function"));

	if ((class = zend_hash_str_find(Z_ARRVAL_P(tmp), ZEND_STRL("object")))) {
		ZVAL_NEW_STR(&class_zv, Z_OBJCE_P(class)->name);
		class = &class_zv;
	} else {
		class = zend_hash_str_find(Z_ARRVAL_P(tmp), ZEND_STRL("class"));
	}

	if (class) {
		type = zend_hash_str_find(Z_ARRVAL_P(tmp), ZEND_STRL("type"));
	}

	args = zend_hash_str_find(Z_ARRVAL_P(tmp), ZEND_STRL("args"));

	phpdbg_xml(" symbol=\"%s%s%s\"", class ? Z_STRVAL_P(class) : "", class ? Z_STRVAL_P(type) : "", Z_STRVAL_P(funcname));

	if (args) {
		phpdbg_xml(">");
	} else {
		phpdbg_xml(" />");
	}

	phpdbg_out("%s%s%s(", class ? Z_STRVAL_P(class) : "", class ? Z_STRVAL_P(type) : "", Z_STRVAL_P(funcname));

	if (args) {
		const zend_function *func = NULL;
		const zend_arg_info *arginfo = NULL;
		zend_bool is_variadic = 0;
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
			phpdbg_xml("<arg %r");
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
					is_variadic = arginfo ? arginfo[j].is_variadic : 0;
				}

				phpdbg_xml(" variadic=\"%s\" name=\"%s\">", is_variadic ? "variadic" : "", arg_name ? arg_name : "");
				phpdbg_out("%s=%s", arg_name ? arg_name : "?", is_variadic ? "[": "");

			} else {
				phpdbg_xml(">");
			}
			++j;

			{
				char *arg_print = phpdbg_short_zval_print(argstmp, 40);
				php_printf("%s", arg_print);
				efree(arg_print);
			}

			phpdbg_xml("</arg>");
		} ZEND_HASH_FOREACH_END();

		if (is_variadic) {
			phpdbg_out("]");
		}
		phpdbg_xml("</frame>");
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
		phpdbg_error("backtrace", "type=\"minnum\"", "Invalid backtrace size %d", limit);

		PHPDBG_OUTPUT_BACKUP_RESTORE();
		return;
	}

	phpdbg_try_access {
		zend_fetch_debug_backtrace(&zbacktrace, 0, 0, limit);
	} phpdbg_catch_access {
		phpdbg_error("signalsegv", "", "Couldn't fetch backtrace, invalid data source");
		return;
	} phpdbg_end_try_access();

	phpdbg_xml("<backtrace %r>");

	Z_LVAL(startline) = zend_get_executed_lineno();
	startfilename = zend_get_executed_filename();
	Z_STR(startfile) = zend_string_init(startfilename, strlen(startfilename), 0);

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL(zbacktrace), &position);
	tmp = zend_hash_get_current_data_ex(Z_ARRVAL(zbacktrace), &position);
	while ((tmp = zend_hash_get_current_data_ex(Z_ARRVAL(zbacktrace), &position))) {
		if (file) { /* userland */
			phpdbg_out("frame #%d: ", i);
			phpdbg_xml("<frame %r id=\"%d\" file=\"%s\" line=\"%d\"", i, Z_STRVAL_P(file), Z_LVAL_P(line));
			phpdbg_dump_prototype(tmp);
			phpdbg_out(" at %s:%ld\n", Z_STRVAL_P(file), Z_LVAL_P(line));
			i++;
		} else {
			phpdbg_out(" => ");
			phpdbg_xml("<frame %r id=\"%d\" internal=\"internal\"", i);
			phpdbg_dump_prototype(tmp);
			phpdbg_out(" (internal function)\n");
		}

		file = zend_hash_str_find(Z_ARRVAL_P(tmp), ZEND_STRL("file"));
		line = zend_hash_str_find(Z_ARRVAL_P(tmp), ZEND_STRL("line"));
		zend_hash_move_forward_ex(Z_ARRVAL(zbacktrace), &position);
	}

	phpdbg_writeln("frame", "id=\"%d\" symbol=\"{main}\" file=\"%s\" line=\"%d\"", "frame #%d: {main} at %s:%ld", i, Z_STRVAL_P(file), Z_LVAL_P(line));
	phpdbg_xml("</backtrace>");

	zval_dtor(&zbacktrace);
	zend_string_release(Z_STR(startfile));

	PHPDBG_OUTPUT_BACKUP_RESTORE();
} /* }}} */
