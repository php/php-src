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

#ifndef PHPDBG_BP_H
#define PHPDBG_BP_H

/* {{{ defines */
#define PHPDBG_BREAK_FILE            0
#define PHPDBG_BREAK_FILE_PENDING    1
#define PHPDBG_BREAK_SYM             2
#define PHPDBG_BREAK_OPLINE          3
#define PHPDBG_BREAK_METHOD          4
#define PHPDBG_BREAK_COND            5
#define PHPDBG_BREAK_OPCODE          6
#define PHPDBG_BREAK_FUNCTION_OPLINE 7
#define PHPDBG_BREAK_METHOD_OPLINE   8
#define PHPDBG_BREAK_FILE_OPLINE     9
#define PHPDBG_BREAK_MAP             10
#define PHPDBG_BREAK_TABLES          11 /* }}} */

/* {{{ */
typedef struct _zend_op *phpdbg_opline_ptr_t; /* }}} */

/* {{{ breakpoint base structure */
#define phpdbg_breakbase(name) \
	int         id; \
	zend_uchar  type; \
	zend_ulong  hits; \
	bool   disabled; \
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
	zend_ulong line;
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
	bool       paramed;
	phpdbg_param_t  param;
	zend_ulong      hash;
	zend_op_array  *ops;
} phpdbg_breakcond_t;

/* {{{ Resolving breaks API */
PHPDBG_API void phpdbg_resolve_op_array_breaks(zend_op_array *op_array);
PHPDBG_API int phpdbg_resolve_op_array_break(phpdbg_breakopline_t *brake, zend_op_array *op_array);
PHPDBG_API int phpdbg_resolve_opline_break(phpdbg_breakopline_t *new_break);
PHPDBG_API HashTable *phpdbg_resolve_pending_file_break_ex(const char *file, uint32_t filelen, zend_string *cur, HashTable *fileht);
PHPDBG_API void phpdbg_resolve_pending_file_break(const char *file); /* }}} */

/* {{{ Breakpoint Creation API */
PHPDBG_API void phpdbg_set_breakpoint_file(const char* filename, size_t path_len, zend_ulong lineno);
PHPDBG_API void phpdbg_set_breakpoint_symbol(const char* func_name, size_t func_name_len);
PHPDBG_API void phpdbg_set_breakpoint_method(const char* class_name, const char* func_name);
PHPDBG_API void phpdbg_set_breakpoint_opcode(const char* opname, size_t opname_len);
PHPDBG_API void phpdbg_set_breakpoint_opline(zend_ulong opline);
PHPDBG_API void phpdbg_set_breakpoint_opline_ex(phpdbg_opline_ptr_t opline);
PHPDBG_API void phpdbg_set_breakpoint_method_opline(const char *class, const char *method, zend_ulong opline);
PHPDBG_API void phpdbg_set_breakpoint_function_opline(const char *function, zend_ulong opline);
PHPDBG_API void phpdbg_set_breakpoint_file_opline(const char *file, zend_ulong opline);
PHPDBG_API void phpdbg_set_breakpoint_expression(const char* expression, size_t expression_len);
PHPDBG_API void phpdbg_set_breakpoint_at(const phpdbg_param_t *param); /* }}} */

/* {{{ Breakpoint Detection API */
PHPDBG_API phpdbg_breakbase_t* phpdbg_find_breakpoint(zend_execute_data*); /* }}} */

/* {{{ Misc Breakpoint API */
PHPDBG_API void phpdbg_hit_breakpoint(phpdbg_breakbase_t* brake, bool output);
PHPDBG_API void phpdbg_print_breakpoints(zend_ulong type);
PHPDBG_API void phpdbg_print_breakpoint(phpdbg_breakbase_t* brake);
PHPDBG_API void phpdbg_reset_breakpoints(void);
PHPDBG_API void phpdbg_clear_breakpoints(void);
PHPDBG_API void phpdbg_delete_breakpoint(zend_ulong num);
PHPDBG_API void phpdbg_enable_breakpoints(void);
PHPDBG_API void phpdbg_enable_breakpoint(zend_ulong id);
PHPDBG_API void phpdbg_disable_breakpoint(zend_ulong id);
PHPDBG_API void phpdbg_disable_breakpoints(void); /* }}} */

/* {{{ Breakbase API */
PHPDBG_API phpdbg_breakbase_t *phpdbg_find_breakbase(zend_ulong id);
PHPDBG_API phpdbg_breakbase_t *phpdbg_find_breakbase_ex(zend_ulong id, HashTable **table, zend_ulong *numkey, zend_string **strkey); /* }}} */

/* {{{ Breakpoint Exportation API */
PHPDBG_API void phpdbg_export_breakpoints(FILE *handle);
PHPDBG_API void phpdbg_export_breakpoints_to_string(char **str); /* }}} */

#endif /* PHPDBG_BP_H */
