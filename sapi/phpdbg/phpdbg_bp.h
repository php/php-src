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

#ifndef PHPDBG_BP_H
#define PHPDBG_BP_H

/* {{{ */
typedef struct _zend_op *phpdbg_opline_ptr_t; /* }}} */

/* {{{ breakpoint base structure */
#define phpdbg_breakbase(name) \
	int         id; \
	zend_uchar  type; \
	zend_ulong  hits; \
	zend_bool   disabled; \
	const char *name /* }}} */

/* {{{ breakpoint base */
typedef struct _phpdbg_breakbase_t {
	phpdbg_breakbase(name);
} phpdbg_breakbase_t; /* }}} */

/**
 * Breakpoint file-based representation
 */
typedef struct _phpdbg_breakfile_t {
	phpdbg_breakbase(filename);
	long        line;
} phpdbg_breakfile_t;

/**
 * Breakpoint symbol-based representation
 */
typedef struct _phpdbg_breaksymbol_t {
	phpdbg_breakbase(symbol);
} phpdbg_breaksymbol_t;

/**
 * Breakpoint method based representation
 */
typedef struct _phpdbg_breakmethod_t {
	phpdbg_breakbase(class_name);
	size_t      class_len;
	const char *func_name;
	size_t      func_len;
} phpdbg_breakmethod_t;

/**
 * Breakpoint opline num based representation
 */
typedef struct _phpdbg_breakopline_t {
	phpdbg_breakbase(func_name);
	size_t      func_len;
	const char *class_name;
	size_t      class_len;
	zend_ulong  opline_num;
	zend_ulong  opline;
} phpdbg_breakopline_t;

/**
 * Breakpoint opline based representation
 */
typedef struct _phpdbg_breakline_t {
	phpdbg_breakbase(name);
	zend_ulong opline;
	phpdbg_breakopline_t *base;
} phpdbg_breakline_t;

/**
 * Breakpoint opcode based representation
 */
typedef struct _phpdbg_breakop_t {
	phpdbg_breakbase(name);
	zend_ulong hash;
} phpdbg_breakop_t;

/**
 * Breakpoint condition based representation
 */
typedef struct _phpdbg_breakcond_t {
	phpdbg_breakbase(code);
	size_t          code_len;
	zend_bool       paramed;
	phpdbg_param_t  param;
	zend_ulong      hash;
	zend_op_array  *ops;
} phpdbg_breakcond_t;

/* {{{ Opline breaks API */
PHPDBG_API void phpdbg_resolve_op_array_breaks(zend_op_array *op_array TSRMLS_DC);
PHPDBG_API int phpdbg_resolve_op_array_break(phpdbg_breakopline_t *brake, zend_op_array *op_array TSRMLS_DC);
PHPDBG_API int phpdbg_resolve_opline_break(phpdbg_breakopline_t *new_break TSRMLS_DC); /* }}} */

/* {{{ Breakpoint Creation API */
PHPDBG_API void phpdbg_set_breakpoint_file(const char* filename, long lineno TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_symbol(const char* func_name, size_t func_name_len TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_method(const char* class_name, const char* func_name TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_opcode(const char* opname, size_t opname_len TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_opline(zend_ulong opline TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_opline_ex(phpdbg_opline_ptr_t opline TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_method_opline(const char *class, const char *method, zend_ulong opline TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_function_opline(const char *function, zend_ulong opline TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_file_opline(const char *file, zend_ulong opline TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_expression(const char* expression, size_t expression_len TSRMLS_DC);
PHPDBG_API void phpdbg_set_breakpoint_at(const phpdbg_param_t *param, const phpdbg_input_t *input TSRMLS_DC); /* }}} */

/* {{{ Breakpoint Detection API */
PHPDBG_API phpdbg_breakbase_t* phpdbg_find_breakpoint(zend_execute_data* TSRMLS_DC); /* }}} */

/* {{{ Misc Breakpoint API */
PHPDBG_API void phpdbg_hit_breakpoint(phpdbg_breakbase_t* brake, zend_bool output TSRMLS_DC);
PHPDBG_API void phpdbg_print_breakpoints(zend_ulong type TSRMLS_DC);
PHPDBG_API void phpdbg_print_breakpoint(phpdbg_breakbase_t* brake TSRMLS_DC);
PHPDBG_API void phpdbg_reset_breakpoints(TSRMLS_D);
PHPDBG_API void phpdbg_clear_breakpoints(TSRMLS_D);
PHPDBG_API void phpdbg_delete_breakpoint(zend_ulong num TSRMLS_DC);
PHPDBG_API void phpdbg_enable_breakpoints(TSRMLS_D);
PHPDBG_API void phpdbg_enable_breakpoint(zend_ulong id TSRMLS_DC);
PHPDBG_API void phpdbg_disable_breakpoint(zend_ulong id TSRMLS_DC);
PHPDBG_API void phpdbg_disable_breakpoints(TSRMLS_D); /* }}} */

/* {{{ Breakbase API */
PHPDBG_API phpdbg_breakbase_t *phpdbg_find_breakbase(zend_ulong id TSRMLS_DC);
PHPDBG_API phpdbg_breakbase_t *phpdbg_find_breakbase_ex(zend_ulong id, HashTable ***table, HashPosition *position TSRMLS_DC); /* }}} */

/* {{{ Breakpoint Exportation API */
PHPDBG_API void phpdbg_export_breakpoints(FILE *handle TSRMLS_DC); /* }}} */

#endif /* PHPDBG_BP_H */
