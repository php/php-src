/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend_sort.h"

#define VM_TRACE(op)     zend_vm_trace(execute_data, opline);
#define VM_TRACE_START() zend_vm_trace_init();
#define VM_TRACE_END()   zend_vm_trace_finish();

static FILE *vm_trace_file;

static void zend_vm_trace(const zend_execute_data *execute_data, const zend_op *opline)
{
	if (EX(func) && EX(func)->op_array.filename) {
		fprintf(vm_trace_file, "%s:%d\n", ZSTR_VAL(EX(func)->op_array.filename), opline->lineno);
	}
}

static void zend_vm_trace_finish(void)
{
	fclose(vm_trace_file);
}

static void zend_vm_trace_init(void)
{
	vm_trace_file = fopen("zend_vm_trace.log", "w+");
}
