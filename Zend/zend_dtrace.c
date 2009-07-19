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

/* $Id$ */

#include "zend.h"
#include "zend_API.h"
#include "zend_dtrace.h"

#ifdef HAVE_SYS_SDT_H
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
ZEND_API void dtrace_execute(zend_op_array *op_array TSRMLS_DC)
{
	int lineno, s_funcname_len, s_classname_len;
	char *scope, *filename, *s_funcname, *s_classname;
	zstr classname, funcname;

	scope = filename = s_funcname = s_classname = NULL;
	classname = funcname = EMPTY_ZSTR;

	/* we need filename and lineno for both execute and function probes */
	if (DTRACE_EXECUTE_ENTRY_ENABLED() || DTRACE_EXECUTE_RETURN_ENABLED()
		|| DTRACE_FUNCTION_ENTRY_ENABLED() || DTRACE_FUNCTION_RETURN_ENABLED()) {
		filename = dtrace_get_executed_filename(TSRMLS_C);
		lineno = zend_get_executed_lineno(TSRMLS_C);
	}

	if (DTRACE_FUNCTION_ENTRY_ENABLED() || DTRACE_FUNCTION_RETURN_ENABLED()) {
		funcname = get_active_function_name(TSRMLS_C);
		classname = get_active_class_name(&scope TSRMLS_CC);

		/* We encode everything to utf8 so that we have a predictable output */
		if (funcname.u != NULL)
			zend_unicode_to_string(ZEND_U_CONVERTER(UG(utf8_conv)), &s_funcname, &s_funcname_len, funcname.u, u_strlen(funcname.u) TSRMLS_CC);

		/* classname might be EMPTY_STR, in that case we have pass NULL to the probe, so that
		   we use appropriate predicates in our dtrace scripts to detect if we are in class context */
		if (u_strlen(classname.u) > 0)
			zend_unicode_to_string(ZEND_U_CONVERTER(UG(utf8_conv)), &s_classname, &s_classname_len, classname.u, u_strlen(classname.u) TSRMLS_CC);
	}

	if (DTRACE_EXECUTE_ENTRY_ENABLED()) {
		DTRACE_EXECUTE_ENTRY(filename, lineno);
	}

	if (DTRACE_FUNCTION_ENTRY_ENABLED() && funcname.u != NULL) {
		DTRACE_FUNCTION_ENTRY(s_funcname, filename, lineno, s_classname, scope);
	}

	execute(op_array TSRMLS_CC);

	if (DTRACE_FUNCTION_RETURN_ENABLED() && funcname.u != NULL) {
		DTRACE_FUNCTION_RETURN(s_funcname, filename, lineno, s_classname, scope);
	}

	if (DTRACE_EXECUTE_RETURN_ENABLED()) {
		DTRACE_EXECUTE_RETURN(filename, lineno);
	}

	if (DTRACE_FUNCTION_ENTRY_ENABLED() || DTRACE_FUNCTION_RETURN_ENABLED()) {
		/* s_funcname and s_classname were allocated by zend_unicode_to_string */
		if (s_funcname != NULL)
			efree(s_funcname);
		if (s_classname != NULL)
			efree(s_classname);
	}
}

ZEND_API void dtrace_execute_internal(zend_execute_data *execute_data_ptr, int return_value_used TSRMLS_DC)
{
	int lineno;
	char *filename;
	if (DTRACE_EXECUTE_ENTRY_ENABLED() || DTRACE_EXECUTE_RETURN_ENABLED()) {
		filename = dtrace_get_executed_filename(TSRMLS_C);
		lineno = zend_get_executed_lineno(TSRMLS_C);
	}

	DTRACE_EXECUTE_ENTRY(filename, lineno);

	execute_internal(execute_data_ptr, return_value_used TSRMLS_CC);

	DTRACE_EXECUTE_RETURN(filename, lineno);
}

/* }}} */
#endif

