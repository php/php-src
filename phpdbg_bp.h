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
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_BP_H
#define PHPDBG_BP_H

/* {{{ */
typedef struct _zend_op *phpdbg_opline_ptr_t; /* }}} */

/**
 * Breakpoint file-based representation
 */
typedef struct _phpdbg_breakfile_t {
	const char *filename;
	long        line;
	int         id;
} phpdbg_breakfile_t;

/**
 * Breakpoint symbol-based representation
 */
typedef struct _phpdbg_breaksymbol_t {
	const char *symbol;
	int         id;
} phpdbg_breaksymbol_t;

/**
 * Breakpoint method based representation
 */
typedef struct _phpdbg_breakmethod_t {
	const char *class_name;
	size_t      class_len;
	const char *func_name;
	size_t      func_len;
	int         id;
} phpdbg_breakmethod_t;

/**
 * Breakpoint opline num based representation
 */
typedef struct _phpdbg_breakopline_t {
	const char *class_name;
	size_t      class_len;
	const char *func_name;
	size_t      func_len;
	int         opline;
	int         id;
} phpdbg_breakopline_t;

/**
 * Breakpoint opline based representation
 */
typedef struct _phpdbg_breakline_t {
	const char *name;
	zend_ulong  opline;
	zend_uchar  type;
	int         id;
} phpdbg_breakline_t;

/**
 * Breakpoint opcode based representation
 */
typedef struct _phpdbg_breakop_t {
	zend_ulong  hash;
	const char *name;
	int         id;
} phpdbg_breakop_t;

/**
 * Breakpoint condition based representation
 */
typedef struct _phpdbg_breakcond_t {
	zend_ulong      hash;
	zval            code;
	zend_op_array  *ops;
	int             id;
} phpdbg_breakcond_t;

PHPDBG_API void phpdbg_resolve_op_array_breaks(zend_op_array *op_array TSRMLS_DC);
PHPDBG_API int phpdbg_resolve_op_array_break(phpdbg_breakopline_t *brake, zend_op_array *op_array TSRMLS_DC);
PHPDBG_API int phpdbg_resolve_opline_break(phpdbg_breakopline_t *new_break TSRMLS_DC);

PHPDBG_API void phpdbg_set_breakpoint_file(const char*, long TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_symbol(const char*, size_t TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_method(const char*, const char* TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_opcode(const char*, size_t TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_opline(zend_ulong TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_opline_ex(phpdbg_opline_ptr_t TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_method_opline(const char *class, const char *method, int opline TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_function_opline(const char *function, int opline TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_expression(const char*, size_t TSRMLS_DC);

int phpdbg_find_breakpoint_file(zend_op_array* TSRMLS_DC);
int phpdbg_find_breakpoint_symbol(zend_function* TSRMLS_DC);
int phpdbg_find_breakpoint_method(zend_op_array* TSRMLS_DC);
int phpdbg_find_breakpoint_opline(phpdbg_opline_ptr_t TSRMLS_DC);
int phpdbg_find_breakpoint_opcode(zend_uchar TSRMLS_DC);
int phpdbg_find_conditional_breakpoint(TSRMLS_D);
int phpdbg_find_breakpoint(zend_execute_data* TSRMLS_DC);

PHPDBG_API void phpdbg_delete_breakpoint(zend_ulong num TSRMLS_DC);
PHPDBG_API void phpdbg_clear_breakpoints(TSRMLS_D);
PHPDBG_API void phpdbg_print_breakpoints(zend_ulong type TSRMLS_DC);

PHPDBG_API void phpdbg_export_breakpoints(FILE *handle TSRMLS_DC);

#endif /* PHPDBG_BP_H */
