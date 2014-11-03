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

#include "zend.h"
#include "phpdbg.h"
#include "phpdbg_utils.h"
#include "phpdbg_frame.h"
#include "phpdbg_list.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

void phpdbg_restore_frame(TSRMLS_D) /* {{{ */
{
	if (PHPDBG_FRAME(num) == 0) {
		return;
	}

	PHPDBG_FRAME(num) = 0;

	/* move things back */
	EG(current_execute_data) = PHPDBG_FRAME(execute_data);

	EG(opline_ptr) = &PHPDBG_EX(opline);
	EG(active_op_array) = PHPDBG_EX(op_array);
	EG(return_value_ptr_ptr) = PHPDBG_EX(original_return_value);
	EG(active_symbol_table) = PHPDBG_EX(symbol_table);
	EG(This) = PHPDBG_EX(current_this);
	EG(scope) = PHPDBG_EX(current_scope);
	EG(called_scope) = PHPDBG_EX(current_called_scope);
} /* }}} */

void phpdbg_switch_frame(int frame TSRMLS_DC) /* {{{ */
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

	phpdbg_restore_frame(TSRMLS_C);

	if (frame > 0) {
		PHPDBG_FRAME(num) = frame;

		/* backup things and jump back */
		PHPDBG_FRAME(execute_data) = EG(current_execute_data);
		EG(current_execute_data) = execute_data;

		EG(opline_ptr) = &PHPDBG_EX(opline);
		EG(active_op_array) = PHPDBG_EX(op_array);
		PHPDBG_FRAME(execute_data)->original_return_value = EG(return_value_ptr_ptr);
		EG(return_value_ptr_ptr) = PHPDBG_EX(original_return_value);
		EG(active_symbol_table) = PHPDBG_EX(symbol_table);
		EG(This) = PHPDBG_EX(current_this);
		EG(scope) = PHPDBG_EX(current_scope);
		EG(called_scope) = PHPDBG_EX(current_called_scope);
	}

	phpdbg_notice("frame", "id=\"%d\"", "Switched to frame #%d", frame);
	phpdbg_list_file(
		zend_get_executed_filename(TSRMLS_C),
		3,
		zend_get_executed_lineno(TSRMLS_C)-1,
		zend_get_executed_lineno(TSRMLS_C)
		TSRMLS_CC
	);
} /* }}} */

static void phpdbg_dump_prototype(zval **tmp TSRMLS_DC) /* {{{ */
{
	zval **funcname, **class, **type, **args, **argstmp;
	char is_class;
	int has_args = FAILURE;

	zend_hash_find(Z_ARRVAL_PP(tmp), "function", sizeof("function"), (void **) &funcname);

	if ((is_class = zend_hash_find(Z_ARRVAL_PP(tmp), "object", sizeof("object"), (void **) &class)) == FAILURE) {
		is_class = zend_hash_find(Z_ARRVAL_PP(tmp), "class", sizeof("class"), (void **)&class);
	} else {
		zend_get_object_classname(*class, (const char **) &Z_STRVAL_PP(class), (zend_uint *) &Z_STRLEN_PP(class) TSRMLS_CC);
	}

	if (is_class == SUCCESS) {
		zend_hash_find(Z_ARRVAL_PP(tmp), "type", sizeof("type"), (void **)&type);
	}

	has_args = zend_hash_find(Z_ARRVAL_PP(tmp), "args", sizeof("args"), (void **)&args) == SUCCESS;

	phpdbg_xml(" symbol=\"%s%s%s\"",
		is_class == FAILURE?"":Z_STRVAL_PP(class),
		is_class == FAILURE?"":Z_STRVAL_PP(type),
		Z_STRVAL_PP(funcname)
	);
	if (has_args) {
		phpdbg_xml(">");
	} else {
		phpdbg_xml(" />");
	}

	phpdbg_out("%s%s%s(",
		is_class == FAILURE?"":Z_STRVAL_PP(class),
		is_class == FAILURE?"":Z_STRVAL_PP(type),
		Z_STRVAL_PP(funcname)
	);

	if (has_args) {
		HashPosition iterator;
		const zend_function *func = NULL;
		const zend_arg_info *arginfo = NULL;
		int j = 0, m;
		zend_bool is_variadic = 0;

		phpdbg_try_access {
			/* assuming no autoloader call is necessary, class should have been loaded if it's in backtrace ... */
			if ((func = phpdbg_get_function(Z_STRVAL_PP(funcname), is_class == FAILURE ? NULL : Z_STRVAL_PP(class) TSRMLS_CC))) {
				arginfo = func->common.arg_info;
			}
		} phpdbg_end_try_access();

		m = func ? func->common.num_args : 0;

		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(args), &iterator);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_PP(args), (void **) &argstmp, &iterator) == SUCCESS) {
			if (j) {
				phpdbg_out(", ");
			}
			phpdbg_xml("<arg %r");
			if (m && j < m) {
#if PHP_VERSION_ID >= 50600
				is_variadic = arginfo ? arginfo[j].is_variadic : 0;
#endif
				phpdbg_xml(" variadic=\"%s\" name=\"%s\">", is_variadic ? "variadic" : "", arginfo ? arginfo[j].name : "");
				phpdbg_out("%s=%s", arginfo ? arginfo[j].name : "?", is_variadic ? "[": "");

			} else {
				phpdbg_xml(">");
			}
			++j;

			zend_print_flat_zval_r(*argstmp TSRMLS_CC);
			zend_hash_move_forward_ex(Z_ARRVAL_PP(args), &iterator);

			phpdbg_xml("</arg>");
		}
		if (is_variadic) {
			phpdbg_out("]");
		}
		phpdbg_xml("</frame>");
	}
	phpdbg_out(")");
}

void phpdbg_dump_backtrace(size_t num TSRMLS_DC) /* {{{ */
{
	zval zbacktrace;
	zval **tmp;
	zval **file, **line;
	HashPosition position;
	int i = 0, limit = num;
	int user_defined;

	PHPDBG_OUTPUT_BACKUP();

	if (limit < 0) {
		phpdbg_error("backtrace", "type=\"minnum\"", "Invalid backtrace size %d", limit);

		PHPDBG_OUTPUT_BACKUP_RESTORE();
		return;
	}

	phpdbg_try_access {
		zend_fetch_debug_backtrace(&zbacktrace, 0, 0, limit TSRMLS_CC);
	} phpdbg_catch_access {
		phpdbg_error("signalsegv", "", "Couldn't fetch backtrace, invalid data source");
		return;
	} phpdbg_end_try_access();

	phpdbg_xml("<backtrace %r>");

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL(zbacktrace), &position);
	zend_hash_get_current_data_ex(Z_ARRVAL(zbacktrace), (void **) &tmp, &position);
	while (1) {
		user_defined = zend_hash_find(Z_ARRVAL_PP(tmp), "file", sizeof("file"), (void **) &file);
		zend_hash_find(Z_ARRVAL_PP(tmp), "line", sizeof("line"), (void **) &line);
		zend_hash_move_forward_ex(Z_ARRVAL(zbacktrace), &position);

		if (zend_hash_get_current_data_ex(Z_ARRVAL(zbacktrace), (void **) &tmp, &position) == FAILURE) {
			phpdbg_write("frame", "id=\"%d\" symbol=\"{main}\" file=\"%s\" line=\"%d\"", "frame #%d: {main} at %s:%ld", i, Z_STRVAL_PP(file), Z_LVAL_PP(line));
			break;
		}

		if (user_defined == SUCCESS) {
			phpdbg_out("frame #%d: ", i);
			phpdbg_xml("<frame %r id=\"%d\" file=\"%s\" line=\"%d\"", i, Z_STRVAL_PP(file), Z_LVAL_PP(line));
			phpdbg_dump_prototype(tmp TSRMLS_CC);
			phpdbg_out(" at %s:%ld\n", Z_STRVAL_PP(file), Z_LVAL_PP(line));
			i++;
		} else {
			phpdbg_out(" => ");
			phpdbg_xml("<frame %r id=\"%d\" internal=\"internal\"", i);
			phpdbg_dump_prototype(tmp TSRMLS_CC);
			phpdbg_out(" (internal function)\n");
		}
	}

	phpdbg_out("\n");
	phpdbg_xml("</backtrace>");

	zval_dtor(&zbacktrace);

	PHPDBG_OUTPUT_BACKUP_RESTORE();
} /* }}} */
