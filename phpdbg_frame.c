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
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "phpdbg.h"
#include "phpdbg_frame.h"
#include "phpdbg_list.h"
#include "phpdbg_utils.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

#define PHPDBG_FRAME(v) (PHPDBG_G(frame).v)
#define PHPDBG_EX(v) (EG(current_execute_data)->v)

void switch_to_frame(int frame) {
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

	restore_frame();

	if (frame > 0) {
		PHPDBG_FRAME(num) = frame;

		/* backup things and jump back */
		PHPDBG_FRAME(execute_data) = EG(current_execute_data);
		EG(current_execute_data) = execute_data;

		EG(opline_ptr) = &PHPDBG_EX(opline);
		EG(active_op_array) = PHPDBG_EX(op_array);
		EG(return_value_ptr_ptr) = PHPDBG_EX(original_return_value);
		EG(active_symbol_table) = PHPDBG_EX(symbol_table);
		EG(This) = PHPDBG_EX(current_this);
		EG(scope) = PHPDBG_EX(current_scope);
		EG(called_scope) = PHPDBG_EX(current_called_scope);
	}

	phpdbg_notice("Switched to frame #%d", frame);
	phpdbg_list_file(
		zend_get_executed_filename(TSRMLS_C),
		3,
		zend_get_executed_lineno(TSRMLS_C)-1,
		zend_get_executed_lineno(TSRMLS_C)
		TSRMLS_CC
	);
}

void restore_frame() {
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
}
