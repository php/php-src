/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_GLOBALS_H
#define ZEND_GLOBALS_H


#include <setjmp.h>

#include "zend_globals_macros.h"

#include "zend_stack.h"
#include "zend_ptr_stack.h"
#include "zend_hash.h"
#include "zend_llist.h"
#include "zend_objects.h"
#include "zend_objects_API.h"
#include "zend_modules.h"
#include "zend_float.h"
#include "zend_multibyte.h"

/* Define ZTS if you want a thread-safe Zend */
/*#undef ZTS*/

#ifdef ZTS

BEGIN_EXTERN_C()
ZEND_API extern int compiler_globals_id;
ZEND_API extern int executor_globals_id;
END_EXTERN_C()

#endif

#define SYMTABLE_CACHE_SIZE 32


#include "zend_compile.h"

/* excpt.h on Digital Unix 4.0 defines function_table */
#undef function_table

#define ZEND_EARLY_BINDING_COMPILE_TIME 0
#define ZEND_EARLY_BINDING_DELAYED      1
#define ZEND_EARLY_BINDING_DELAYED_ALL  2

typedef struct _zend_declarables {
	zval ticks;
} zend_declarables;

typedef struct _zend_vm_stack *zend_vm_stack;
typedef struct _zend_ini_entry zend_ini_entry;


struct _zend_compiler_globals {
	zend_stack bp_stack;
	zend_stack switch_cond_stack;
	zend_stack foreach_copy_stack;
	zend_stack object_stack;
	zend_stack declare_stack;

	zend_class_entry *active_class_entry;

	/* variables for list() compilation */
	zend_llist list_llist;
	zend_llist dimension_llist;
	zend_stack list_stack;

	zend_stack function_call_stack;

	char *compiled_filename;

	int zend_lineno;

	char *heredoc;
	int heredoc_len;

	zend_op_array *active_op_array;

	HashTable *function_table;	/* function symbol table */
	HashTable *class_table;		/* class table */

	HashTable filenames_table;

	HashTable *auto_globals;

	zend_bool parse_error;
	zend_bool in_compilation;
	zend_bool short_tags;
	zend_bool asp_tags;

	zend_declarables declarables;

	zend_bool unclean_shutdown;

	zend_bool ini_parser_unbuffered_errors;

	zend_llist open_files;

	long catch_begin;

	struct _zend_ini_parser_param *ini_parser_param;

	int interactive;

	zend_uint start_lineno;
	zend_bool increment_lineno;

	znode implementing_class;

	zend_uint access_type;

	char *doc_comment;
	zend_uint doc_comment_len;

	zend_uint compiler_options; /* set of ZEND_COMPILE_* constants */

	zval      *current_namespace;
	HashTable *current_import;
	zend_bool  in_namespace;
	zend_bool  has_bracketed_namespaces;

	zend_compiler_context context;
	zend_stack context_stack;

	/* interned strings */
	char *interned_strings_start;
	char *interned_strings_end;
	char *interned_strings_top;
	char *interned_strings_snapshot_top;

	HashTable interned_strings;

	const zend_encoding **script_encoding_list;
	size_t script_encoding_list_size;
	zend_bool multibyte;
	zend_bool detect_unicode;
	zend_bool encoding_declared;

#ifdef ZTS
	zval ***static_members_table;
	int last_static_member;
#endif
};


struct _zend_executor_globals {
	zval **return_value_ptr_ptr;

	zval uninitialized_zval;
	zval *uninitialized_zval_ptr;

	zval error_zval;
	zval *error_zval_ptr;

	zend_ptr_stack arg_types_stack;

	/* symbol table cache */
	HashTable *symtable_cache[SYMTABLE_CACHE_SIZE];
	HashTable **symtable_cache_limit;
	HashTable **symtable_cache_ptr;

	zend_op **opline_ptr;

	HashTable *active_symbol_table;
	HashTable symbol_table;		/* main symbol table */

	HashTable included_files;	/* files already included */

	JMP_BUF *bailout;

	int error_reporting;
	int orig_error_reporting;
	int exit_status;

	zend_op_array *active_op_array;

	HashTable *function_table;	/* function symbol table */
	HashTable *class_table;		/* class table */
	HashTable *zend_constants;	/* constants table */

	zend_class_entry *scope;
	zend_class_entry *called_scope; /* Scope of the calling class */

	zval *This;

	long precision;

	int ticks_count;

	zend_bool in_execution;
	HashTable *in_autoload;
	zend_function *autoload_func;
	zend_bool full_tables_cleanup;

	/* for extended information support */
	zend_bool no_extensions;

#ifdef ZEND_WIN32
	zend_bool timed_out;
	OSVERSIONINFOEX windows_version_info;
#endif

	HashTable regular_list;
	HashTable persistent_list;

	zend_vm_stack argument_stack;

	int user_error_handler_error_reporting;
	zval *user_error_handler;
	zval *user_exception_handler;
	zend_stack user_error_handlers_error_reporting;
	zend_ptr_stack user_error_handlers;
	zend_ptr_stack user_exception_handlers;

	zend_error_handling_t  error_handling;
	zend_class_entry      *exception_class;

	/* timeout support */
	int timeout_seconds;

	int lambda_count;

	HashTable *ini_directives;
	HashTable *modified_ini_directives;
	zend_ini_entry *error_reporting_ini_entry;	                

	zend_objects_store objects_store;
	zval *exception, *prev_exception;
	zend_op *opline_before_exception;
	zend_op exception_op[3];

	struct _zend_execute_data *current_execute_data;

	struct _zend_module_entry *current_module;

	zend_property_info std_property_info;

	zend_bool active; 

	zend_op *start_op;

	void *saved_fpu_cw_ptr;
#if XPFPA_HAVE_CW
	XPFPA_CW_DATATYPE saved_fpu_cw;
#endif

	void *reserved[ZEND_MAX_RESERVED_RESOURCES];
};

struct _zend_ini_scanner_globals {
	zend_file_handle *yy_in;
	zend_file_handle *yy_out;

	unsigned int yy_leng;
	unsigned char *yy_start;
	unsigned char *yy_text;
	unsigned char *yy_cursor;
	unsigned char *yy_marker;
	unsigned char *yy_limit;
	int yy_state;
	zend_stack state_stack;

	char *filename;
	int lineno;

	/* Modes are: ZEND_INI_SCANNER_NORMAL, ZEND_INI_SCANNER_RAW */
	int scanner_mode;
};

struct _zend_php_scanner_globals {
	zend_file_handle *yy_in;
	zend_file_handle *yy_out;

	unsigned int yy_leng;
	unsigned char *yy_start;
	unsigned char *yy_text;
	unsigned char *yy_cursor;
	unsigned char *yy_marker;
	unsigned char *yy_limit;
	int yy_state;
	zend_stack state_stack;
	
	/* original (unfiltered) script */
	unsigned char *script_org;
	size_t script_org_size;

	/* filtered script */
	unsigned char *script_filtered;
	size_t script_filtered_size;

	/* input/ouput filters */
	zend_encoding_filter input_filter;
	zend_encoding_filter output_filter;
	const zend_encoding *script_encoding;
};

#endif /* ZEND_GLOBALS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
