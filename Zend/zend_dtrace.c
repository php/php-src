/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
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

#include "zend.h"
#include "zend_API.h"
#include "zend_dtrace.h"

#ifdef HAVE_DTRACE

ZEND_API zend_op_array *(*zend_dtrace_compile_file)(zend_file_handle *file_handle, int type);
ZEND_API void (*zend_dtrace_execute)(zend_op_array *op_array);
ZEND_API void (*zend_dtrace_execute_internal)(zend_execute_data *execute_data, zval *return_value);

/* PHP DTrace probes {{{ */
static inline const char *dtrace_get_executed_filename(void)
{
	zend_execute_data *ex = EG(current_execute_data);

	while (ex && (!ex->func || !ZEND_USER_CODE(ex->func->type))) {
		ex = ex->prev_execute_data;
	}
	if (ex) {
		return ZSTR_VAL(ex->func->op_array.filename);
	} else {
		return zend_get_executed_filename();
	}
}

ZEND_API zend_op_array *dtrace_compile_file(zend_file_handle *file_handle, int type)
{
	zend_op_array *res;
	DTRACE_COMPILE_FILE_ENTRY(ZSTR_VAL(file_handle->opened_path), (char *)file_handle->filename);
	res = compile_file(file_handle, type);
	DTRACE_COMPILE_FILE_RETURN(ZSTR_VAL(file_handle->opened_path), (char *)file_handle->filename);

	return res;
}

/* We wrap the execute function to have fire the execute-entry/return and function-entry/return probes */
ZEND_API void dtrace_execute_ex(zend_execute_data *execute_data)
{
	int lineno;
	const char *scope, *filename, *funcname, *classname;
	scope = filename = funcname = classname = NULL;

	/* we need filename and lineno for both execute and function probes */
	if (DTRACE_EXECUTE_ENTRY_ENABLED() || DTRACE_EXECUTE_RETURN_ENABLED()
		|| DTRACE_FUNCTION_ENTRY_ENABLED() || DTRACE_FUNCTION_RETURN_ENABLED()) {
		filename = dtrace_get_executed_filename();
		lineno = zend_get_executed_lineno();
	}

	if (DTRACE_FUNCTION_ENTRY_ENABLED() || DTRACE_FUNCTION_RETURN_ENABLED()) {
		classname = get_active_class_name(&scope);
		funcname = get_active_function_name();
	}

	if (DTRACE_EXECUTE_ENTRY_ENABLED()) {
		DTRACE_EXECUTE_ENTRY((char *)filename, lineno);
	}

	if (DTRACE_FUNCTION_ENTRY_ENABLED() && funcname != NULL) {
		DTRACE_FUNCTION_ENTRY((char *)funcname, (char *)filename, lineno, (char *)classname, (char *)scope);
	}

	execute_ex(execute_data);

	if (DTRACE_FUNCTION_RETURN_ENABLED() && funcname != NULL) {
		DTRACE_FUNCTION_RETURN((char *)funcname, (char *)filename, lineno, (char *)classname, (char *)scope);
	}

	if (DTRACE_EXECUTE_RETURN_ENABLED()) {
		DTRACE_EXECUTE_RETURN((char *)filename, lineno);
	}
}

ZEND_API void dtrace_execute_internal(zend_execute_data *execute_data, zval *return_value)
{
	int lineno;
	const char *filename;
	if (DTRACE_EXECUTE_ENTRY_ENABLED() || DTRACE_EXECUTE_RETURN_ENABLED()) {
		filename = dtrace_get_executed_filename();
		lineno = zend_get_executed_lineno();
	}

	if (DTRACE_EXECUTE_ENTRY_ENABLED()) {
		DTRACE_EXECUTE_ENTRY((char *)filename, lineno);
	}

	execute_internal(execute_data, return_value);

	if (DTRACE_EXECUTE_RETURN_ENABLED()) {
		DTRACE_EXECUTE_RETURN((char *)filename, lineno);
	}
}

void dtrace_error_notify_cb(int type, const char *error_filename, uint32_t error_lineno, zend_string *message)
{
	if (DTRACE_ERROR_ENABLED()) {
		DTRACE_ERROR(ZSTR_VAL(message), (char *)error_filename, error_lineno);
	}
}

/* }}} */

#endif /* HAVE_DTRACE */
