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

#ifdef ZEND_MULTIBYTE
#include "zend_multibyte.h"
#endif /* ZEND_MULTIBYTE */

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


typedef struct _zend_declarables {
	zval ticks;
} zend_declarables;


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
	int comment_start_line;
	char *heredoc;
	int heredoc_len;

	zend_op_array *active_op_array;

	HashTable *function_table;	/* function symbol table */
	HashTable *class_table;		/* class table */

	HashTable filenames_table;

	HashTable *auto_globals;

	zend_bool in_compilation;
	zend_bool short_tags;
	zend_bool asp_tags;
	zend_bool allow_call_time_pass_reference;

	zend_declarables declarables;

	/* For extensions support */
	zend_bool extended_info;	/* generate extension information for debugger/profiler */
	zend_bool handle_op_arrays;	/* run op_arrays through op_array handlers */

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

#ifdef ZEND_MULTIBYTE
	zend_encoding **script_encoding_list;
	int script_encoding_list_size;
	zend_bool detect_unicode;

	zend_encoding *internal_encoding;

	/* multibyte utility functions */
	zend_encoding_detector encoding_detector;
	zend_encoding_converter encoding_converter;
	zend_encoding_oddlen encoding_oddlen;
#endif /* ZEND_MULTIBYTE */

#ifdef ZTS
	HashTable **static_members;
	int last_static_member;
#endif
};


struct _zend_executor_globals {
	zval **return_value_ptr_ptr;

	zval uninitialized_zval;
	zval *uninitialized_zval_ptr;

	zval error_zval;
	zval *error_zval_ptr;

	zend_function_state *function_state_ptr;
	zend_ptr_stack arg_types_stack;

	/* symbol table cache */
	HashTable *symtable_cache[SYMTABLE_CACHE_SIZE];
	HashTable **symtable_cache_limit;
	HashTable **symtable_cache_ptr;

	zend_op **opline_ptr;

	HashTable *active_symbol_table;
	HashTable symbol_table;		/* main symbol table */

	HashTable included_files;	/* files already included */

	jmp_buf *bailout;

	int error_reporting;
	int orig_error_reporting;
	int exit_status;

	zend_op_array *active_op_array;

	HashTable *function_table;	/* function symbol table */
	HashTable *class_table;		/* class table */
	HashTable *zend_constants;	/* constants table */

	zend_class_entry *scope;

	zval *This;

	long precision;

	int ticks_count;

	zend_bool in_execution;
	HashTable *in_autoload;
	zend_function *autoload_func;
	zend_bool full_tables_cleanup;
	zend_bool ze1_compatibility_mode;

	/* for extended information support */
	zend_bool no_extensions;

#ifdef ZEND_WIN32
	zend_bool timed_out;
#endif

	HashTable regular_list;
	HashTable persistent_list;

	zend_ptr_stack argument_stack;

	int user_error_handler_error_reporting;
	zval *user_error_handler;
	zval *user_exception_handler;
	zend_stack user_error_handlers_error_reporting;
	zend_ptr_stack user_error_handlers;
	zend_ptr_stack user_exception_handlers;

	/* timeout support */
	int timeout_seconds;

	int lambda_count;

	HashTable *ini_directives;
	HashTable *modified_ini_directives;

	zend_objects_store objects_store;
	zval *exception;
	zend_op *opline_before_exception;

	struct _zend_execute_data *current_execute_data;

	struct _zend_module_entry *current_module;

	zend_property_info std_property_info;

	zend_bool active; 

	void *reserved[ZEND_MAX_RESERVED_RESOURCES];
};

struct _zend_scanner_globals {
	zend_file_handle *yy_in;
	zend_file_handle *yy_out;
	int yy_leng;
	char *yy_text;
	struct yy_buffer_state *current_buffer;
	char *c_buf_p;
	int init;
	int start;
	int lineno;
	char _yy_hold_char;
	int yy_n_chars;
	int _yy_did_buffer_switch_on_eof;
	int _yy_last_accepting_state; /* Must be of the same type as yy_state_type,
								   * if for whatever reason it's no longer int!
								   */
	char *_yy_last_accepting_cpos;
	int _yy_more_flag;
	int _yy_more_len;
	int yy_start_stack_ptr;
	int yy_start_stack_depth;
	int *yy_start_stack;

#ifdef ZEND_MULTIBYTE
	/* original (unfiltered) script */
	char *script_org;
	int script_org_size;

	/* filtered script */
	char *script_filtered;
	int script_filtered_size;

	/* input/ouput filters */
	zend_encoding_filter input_filter;
	zend_encoding_filter output_filter;
	zend_encoding *script_encoding;
	zend_encoding *internal_encoding;
#endif /* ZEND_MULTIBYTE */
};

#endif /* ZEND_GLOBALS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
