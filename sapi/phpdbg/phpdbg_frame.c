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
	EG(active_symbol_table) = PHPDBG_EX(symbol_table);
	Z_OBJ(EG(This)) = PHPDBG_EX(object);
	EG(scope) = PHPDBG_EX(scope);
	EG(called_scope) = PHPDBG_EX(called_scope);
} /* }}} */

void phpdbg_switch_frame(int frame TSRMLS_DC) /* {{{ */
{
	zend_execute_data *execute_data = PHPDBG_FRAME(num)?PHPDBG_FRAME(execute_data):EG(current_execute_data);
	int i = 0;

	if (PHPDBG_FRAME(num) == frame) {
		phpdbg_notice("Already in frame #%d", frame);
		return;
	}

	while (execute_data) {
		if (i++ == frame) {
			break;
		}

		do {
			execute_data = execute_data->prev_execute_data;
		} while (execute_data && execute_data->opline == NULL);
	}

	if (execute_data == NULL) {
		phpdbg_error("No frame #%d", frame);
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
		EG(active_symbol_table) = PHPDBG_EX(symbol_table);
		Z_OBJ(EG(This)) = PHPDBG_EX(object);
		EG(scope) = PHPDBG_EX(scope);
		EG(called_scope) = PHPDBG_EX(called_scope);
	}

	phpdbg_notice("Switched to frame #%d", frame);
	phpdbg_list_file(
		zend_get_executed_filename(TSRMLS_C),
		3,
		zend_get_executed_lineno(TSRMLS_C)-1,
		zend_get_executed_lineno(TSRMLS_C)
		TSRMLS_CC
	);
} /* }}} */

static void phpdbg_dump_prototype(zval *tmp TSRMLS_DC) /* {{{ */
{
	zval *funcname, *class, *type, *args, *argstmp;
	zend_string *class_name;

	funcname = zend_hash_str_find(Z_ARRVAL_P(tmp), "function", sizeof("function") - 1);

	if ((class = zend_hash_str_find(Z_ARRVAL_P(tmp), "object", sizeof("object") - 1)) == NULL) {
		class = zend_hash_str_find(Z_ARRVAL_P(tmp), "class", sizeof("class") - 1);
	} else {
		class_name = zend_get_object_classname(Z_OBJ_P(class) TSRMLS_CC);
	}

	if (class) {
		type = zend_hash_str_find(Z_ARRVAL_P(tmp), "type", sizeof("type") - 1);
	}

	phpdbg_write("%s%s%s(",
		class == NULL?"":Z_STRVAL_P(class),
		class == NULL?"":Z_STRVAL_P(type),
		Z_STRVAL_P(funcname));

	if ((args = zend_hash_str_find(Z_ARRVAL_P(tmp), "args", sizeof("args") - 1)) != NULL) {
		HashPosition iterator;
		const zend_function *func = phpdbg_get_function(
			Z_STRVAL_P(funcname), class == NULL ? NULL : Z_STRVAL_P(class) TSRMLS_CC);
		const zend_arg_info *arginfo = func ? func->common.arg_info : NULL;
		int j = 0, m = func ? func->common.num_args : 0;
		zend_bool is_variadic = 0;

		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(args), &iterator);
		while ((argstmp = zend_hash_get_current_data_ex(Z_ARRVAL_P(args), &iterator)) != NULL) {
			if (j) {
				phpdbg_write(", ");
			}
			if (m && j < m) {
#if PHP_VERSION_ID >= 50600
				is_variadic = arginfo[j].is_variadic;
#endif
				phpdbg_write("%s=%s",
					arginfo[j].name, is_variadic ? "[": "");
			}
			++j;

			zend_print_flat_zval_r(argstmp TSRMLS_CC);
			zend_hash_move_forward_ex(Z_ARRVAL_P(args), &iterator);
		}
		if (is_variadic) {
			phpdbg_write("]");
		}
	}
	phpdbg_write(")");
}
/* }}} */

void phpdbg_dump_backtrace(size_t num TSRMLS_DC) /* {{{ */
{
	zval zbacktrace;
	zval *tmp;
	zval *file, *line;
	HashPosition position;
	int i = 1, limit = num;

	if (limit < 0) {
		phpdbg_error("Invalid backtrace size %d", limit);
	}

	zend_fetch_debug_backtrace(
		&zbacktrace, 0, 0, limit TSRMLS_CC);

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL(zbacktrace), &position);
	tmp = zend_hash_get_current_data_ex(Z_ARRVAL(zbacktrace), &position);
	while (1) {
		file = zend_hash_str_find(Z_ARRVAL_P(tmp), "file", sizeof("file") - 1);
		line = zend_hash_str_find(Z_ARRVAL_P(tmp), "line", sizeof("line") - 1);
		zend_hash_move_forward_ex(Z_ARRVAL(zbacktrace), &position);

		if ((tmp = zend_hash_get_current_data_ex(Z_ARRVAL(zbacktrace), &position)) == NULL) {
			phpdbg_write("frame #0: {main} at %s:%ld", Z_STRVAL_P(file), Z_LVAL_P(line));
			break;
		}

		if (file) {
			phpdbg_write("frame #%d: ", i++);
			phpdbg_dump_prototype(tmp TSRMLS_CC);
			phpdbg_writeln(" at %s:%ld", Z_STRVAL_P(file), Z_LVAL_P(line));
		} else {
			phpdbg_write(" => ");
			phpdbg_dump_prototype(tmp TSRMLS_CC);
			phpdbg_writeln(" (internal function)");
		}
	}

	phpdbg_writeln(EMPTY);
	zval_ptr_dtor(&zbacktrace);
} /* }}} */
