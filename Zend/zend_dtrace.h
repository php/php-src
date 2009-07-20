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

#ifndef	_ZEND_DTRACE_H
#define	_ZEND_DTRACE_H

#ifndef ZEND_WIN32
# include <unistd.h>
#endif

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef HAVE_SYS_SDT_H
ZEND_API zend_op_array *(*zend_dtrace_compile_file)(zend_file_handle *file_handle, int type TSRMLS_DC);
ZEND_API void (*zend_dtrace_execute)(zend_op_array *op_array TSRMLS_DC);
ZEND_API void (*zend_dtrace_execute_internal)(zend_execute_data *execute_data_ptr, int return_value_used TSRMLS_DC);

ZEND_API zend_op_array *dtrace_compile_file(zend_file_handle *file_handle, int type TSRMLS_DC);
ZEND_API void dtrace_execute(zend_op_array *op_array TSRMLS_DC);
ZEND_API void dtrace_execute_internal(zend_execute_data *execute_data_ptr, int return_value_used TSRMLS_DC);
#include <zend_dtrace_gen.h>

#else

#define DTRACE_COMPILE_FILE_ENTRY(arg0, arg1)
#define DTRACE_COMPILE_FILE_ENTRY_ENABLED() (0)
#define DTRACE_COMPILE_FILE_RETURN(arg0, arg1)
#define DTRACE_COMPILE_FILE_RETURN_ENABLED() (0)
#define DTRACE_ERROR(arg0, arg1, arg2)
#define DTRACE_ERROR_ENABLED() (0)
#define DTRACE_EXCEPTION_CAUGHT(arg0)
#define DTRACE_EXCEPTION_CAUGHT_ENABLED() (0)
#define DTRACE_EXCEPTION_THROWN(arg0)
#define DTRACE_EXCEPTION_THROWN_ENABLED() (0)
#define DTRACE_EXECUTE_ENTRY(arg0, arg1)
#define DTRACE_EXECUTE_ENTRY_ENABLED() (0)
#define DTRACE_EXECUTE_RETURN(arg0, arg1)
#define DTRACE_EXECUTE_RETURN_ENABLED() (0)
#define DTRACE_FUNCTION_ENTRY(arg0, arg1, arg2, arg3, arg4)
#define DTRACE_FUNCTION_ENTRY_ENABLED() (0)
#define DTRACE_FUNCTION_RETURN(arg0, arg1, arg2, arg3, arg4)
#define DTRACE_FUNCTION_RETURN_ENABLED() (0)
#define DTRACE_REQUEST_SHUTDOWN(arg0, arg1, arg2)
#define DTRACE_REQUEST_SHUTDOWN_ENABLED() (0)
#define DTRACE_REQUEST_STARTUP(arg0, arg1, arg2)
#define DTRACE_REQUEST_STARTUP_ENABLED() (0)

#endif /* HAVE_SYS_SDT */

#ifdef	__cplusplus
}
#endif

#endif	/* _ZEND_DTRACE_H */
