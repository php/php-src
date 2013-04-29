/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2009 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: David Soria Parra <david.soriaparra@sun.com>                |
   +----------------------------------------------------------------------+
*/

/* $Id: $ */

#include "zend.h"
#include "zend_API.h"
#include "zend_dtrace.h"

#ifdef HAVE_DTRACE
/* PHP DTrace probes {{{ */
static inline char *dtrace_get_executed_filename(TSRMLS_D)
{
	if (EG(current_execute_data) && EG(current_execute_data)->op_array) {
		return EG(current_execute_data)->op_array->filename;
	} else {
		return zend_get_executed_filename(TSRMLS_C);
	}
}

ZEND_API zend_op_array *dtrace_compile_file(zend_file_handle *file_handle, int type TSRMLS_DC)
{
	zend_op_array *res;
	DTRACE_COMPILE_FILE_ENTRY(file_handle->opened_path, file_handle->filename);
	res = compile_file(file_handle, type TSRMLS_CC);
	DTRACE_COMPILE_FILE_RETURN(file_handle->opened_path, file_handle->filename);

	return res;
}

/* We wrap the execute function to have fire the execute-entry/return and function-entry/return probes */
ZEND_API void dtrace_execute_ex(zend_execute_data *execute_data TSRMLS_DC)
{
	int lineno;
	char *scope, *filename, *funcname, *classname;
	scope = filename = funcname = classname = NULL;

	/* we need filename and lineno for both execute and function probes */
	if (DTRACE_EXECUTE_ENTRY_ENABLED() || DTRACE_EXECUTE_RETURN_ENABLED()
		|| DTRACE_FUNCTION_ENTRY_ENABLED() || DTRACE_FUNCTION_RETURN_ENABLED()) {
		filename = dtrace_get_executed_filename(TSRMLS_C);
		lineno = zend_get_executed_lineno(TSRMLS_C);
	}

	if (DTRACE_FUNCTION_ENTRY_ENABLED() || DTRACE_FUNCTION_RETURN_ENABLED()) {
		filename = dtrace_get_executed_filename(TSRMLS_C);
		classname = get_active_class_name(&scope TSRMLS_CC);
		funcname = get_active_function_name(TSRMLS_C);
		lineno = zend_get_executed_lineno(TSRMLS_C);
	}

	if (DTRACE_EXECUTE_ENTRY_ENABLED()) {
		DTRACE_EXECUTE_ENTRY(filename, lineno);
	}

	if (DTRACE_FUNCTION_ENTRY_ENABLED() && funcname != NULL) {
		DTRACE_FUNCTION_ENTRY(funcname, filename, lineno, classname, scope);
	}

	execute_ex(execute_data TSRMLS_CC);

	if (DTRACE_FUNCTION_RETURN_ENABLED() && funcname != NULL) {
		DTRACE_FUNCTION_RETURN(funcname, filename, lineno, classname, scope);
	}

	if (DTRACE_EXECUTE_RETURN_ENABLED()) {
		DTRACE_EXECUTE_RETURN(filename, lineno);
	}
}

ZEND_API void dtrace_execute_internal(zend_execute_data *execute_data_ptr, zend_fcall_info *fci, int return_value_used TSRMLS_DC)
{
	int lineno;
	char *filename;
	if (DTRACE_EXECUTE_ENTRY_ENABLED() || DTRACE_EXECUTE_RETURN_ENABLED()) {
		filename = dtrace_get_executed_filename(TSRMLS_C);
		lineno = zend_get_executed_lineno(TSRMLS_C);
	}

	if (DTRACE_EXECUTE_ENTRY_ENABLED()) {
		DTRACE_EXECUTE_ENTRY(filename, lineno);
	}

	execute_internal(execute_data_ptr, fci, return_value_used TSRMLS_CC);

	if (DTRACE_EXECUTE_RETURN_ENABLED()) {
		DTRACE_EXECUTE_RETURN(filename, lineno);
	}
}

/* }}} */
#endif /* HAVE_DTRACE */

