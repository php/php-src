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

/* {{{ breakpoint base */
typedef struct _phpdbg_breakbase_t {
	int			id;
	zend_uchar  type;
	const char  *name;
} phpdbg_breakbase_t; /* }}} */

/**
 * Breakpoint file-based representation
 */
typedef struct _phpdbg_breakfile_t {
	int         id;
	zend_uchar  type;
	const char *filename;
	long        line;
} phpdbg_breakfile_t;

/**
 * Breakpoint symbol-based representation
 */
typedef struct _phpdbg_breaksymbol_t {
	int         id;
	zend_uchar  type;
	const char *symbol;
} phpdbg_breaksymbol_t;

/**
 * Breakpoint method based representation
 */
typedef struct _phpdbg_breakmethod_t {
	int         id;
	zend_uchar  type;
	const char *class_name;
	size_t      class_len;
	const char *func_name;
	size_t      func_len;
} phpdbg_breakmethod_t;

/**
 * Breakpoint opline based representation
 */
typedef struct _phpdbg_breakline_t {
	int         id;
	zend_uchar  type;
	const char *name;
	zend_ulong  opline;
} phpdbg_breakline_t;

/**
 * Breakpoint opcode based representation
 */
typedef struct _phpdbg_breakop_t {
	int         id;
	zend_uchar  type;
	const char *name;
	zend_ulong  hash;
} phpdbg_breakop_t;

/**
 * Breakpoint condition based representation
 */
typedef struct _phpdbg_breakcond_t {
	int             id;
	zend_uchar  	type;
	zend_ulong      hash;
	zval            code;
	zend_op_array  *ops;
} phpdbg_breakcond_t;

PHPDBG_API void phpdbg_set_breakpoint_file(const char*, long TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_symbol(const char*, size_t TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_method(const char*, const char* TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_opcode(const char*, size_t TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_opline(zend_ulong TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_opline_ex(phpdbg_opline_ptr_t TSRMLS_DC);
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
