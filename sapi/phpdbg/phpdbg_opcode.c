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
#include "zend_vm_opcodes.h"
#include "zend_compile.h"
#include "phpdbg_opcode.h"
#include "phpdbg_utils.h"
#include "ext/standard/php_string.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

void phpdbg_print_opline_ex(zend_execute_data *execute_data, bool ignore_flags) /* {{{ */
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

void phpdbg_print_opline(zend_execute_data *execute_data, bool ignore_flags) /* {{{ */
{
	phpdbg_print_opline_ex(execute_data, ignore_flags);
} /* }}} */
