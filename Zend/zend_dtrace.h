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

#ifdef HAVE_DTRACE
ZEND_API zend_op_array *(*zend_dtrace_compile_file)(zend_file_handle *file_handle, int type TSRMLS_DC);
ZEND_API void (*zend_dtrace_execute)(zend_op_array *op_array TSRMLS_DC);
ZEND_API void (*zend_dtrace_execute_internal)(zend_execute_data *execute_data_ptr, zend_fcall_info *fci, int return_value_used TSRMLS_DC);

ZEND_API zend_op_array *dtrace_compile_file(zend_file_handle *file_handle, int type TSRMLS_DC);
ZEND_API void dtrace_execute_ex(zend_execute_data *execute_data TSRMLS_DC);
ZEND_API void dtrace_execute_internal(zend_execute_data *execute_data_ptr, zend_fcall_info *fci, int return_value_used TSRMLS_DC);
#include <zend_dtrace_gen.h>

#endif /* HAVE_DTRACE */

#ifdef	__cplusplus
}
#endif

#endif	/* _ZEND_DTRACE_H */
