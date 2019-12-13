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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

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
#include "zend_multiply.h"
#include "zend_arena.h"

/* Define ZTS if you want a thread-safe Zend */
/*#undef ZTS*/

#ifdef ZTS

BEGIN_EXTERN_C()
ZEND_API extern int compiler_globals_id;
ZEND_API extern int executor_globals_id;
ZEND_API extern size_t compiler_globals_offset;
ZEND_API extern size_t executor_globals_offset;
END_EXTERN_C()

#endif

#define SYMTABLE_CACHE_SIZE 32


#include "zend_compile.h"

/* excpt.h on Digital Unix 4.0 defines function_table */
#undef function_table

typedef struct _zend_vm_stack *zend_vm_stack;
typedef struct _zend_ini_entry zend_ini_entry;


struct _zend_compiler_globals {
	zend_stack loop_var_stack;

	zend_class_entry *active_class_entry;

	zend_string *compiled_filename;

	int zend_lineno;

	zend_op_array *active_op_array;

	HashTable *function_table;	/* function symbol table */
	HashTable *class_table;		/* class table */

	HashTable filenames_table;

	HashTable *auto_globals;

	zend_bool parse_error;
	zend_bool in_compilation;
	zend_bool short_tags;

	zend_bool unclean_shutdown;

	zend_bool ini_parser_unbuffered_errors;

	zend_llist open_files;

	struct _zend_ini_parser_param *ini_parser_param;

	zend_bool skip_shebang;
	zend_bool increment_lineno;

	zend_string *doc_comment;
	uint32_t extra_fn_flags;

	uint32_t compiler_options; /* set of ZEND_COMPILE_* constants */

	zend_oparray_context context;
	zend_file_context file_context;

	zend_arena *arena;

	HashTable interned_strings;

	const zend_encoding **script_encoding_list;
	size_t script_encoding_list_size;
	zend_bool multibyte;
	zend_bool detect_unicode;
	zend_bool encoding_declared;

	zend_ast *ast;
	zend_arena *ast_arena;

	zend_stack delayed_oplines_stack;
	HashTable *memoized_exprs;
	int memoize_mode;

	void   *map_ptr_base;
	size_t  map_ptr_size;
	size_t  map_ptr_last;

	HashTable *delayed_variance_obligations;
	HashTable *delayed_autoloads;

	uint32_t rtd_key_counter;
};


struct _zend_executor_globals {
	zval uninitialized_zval;
	zval error_zval;

	/* symbol table cache */
	zend_array *symtable_cache[SYMTABLE_CACHE_SIZE];
	/* Pointer to one past the end of the symtable_cache */
	zend_array **symtable_cache_limit;
	/* Pointer to first unused symtable_cache slot */
	zend_array **symtable_cache_ptr;

	zend_array symbol_table;		/* main symbol table */

	HashTable included_files;	/* files already included */

	JMP_BUF *bailout;

	int error_reporting;
	int exit_status;

	HashTable *function_table;	/* function symbol table */
	HashTable *class_table;		/* class table */
	HashTable *zend_constants;	/* constants table */

	zval          *vm_stack_top;
	zval          *vm_stack_end;
	zend_vm_stack  vm_stack;
	size_t         vm_stack_page_size;

	struct _zend_execute_data *current_execute_data;
	zend_class_entry *fake_scope; /* used to avoid checks accessing properties */

	zend_long precision;

	int ticks_count;

	uint32_t persistent_constants_count;
	uint32_t persistent_functions_count;
	uint32_t persistent_classes_count;

	HashTable *in_autoload;
	zend_function *autoload_func;
	zend_bool full_tables_cleanup;

	/* for extended information support */
	zend_bool no_extensions;

	zend_bool vm_interrupt;
	zend_bool timed_out;
	zend_long hard_timeout;

#ifdef ZEND_WIN32
	OSVERSIONINFOEX windows_version_info;
#endif

	HashTable regular_list;
	HashTable persistent_list;

	int user_error_handler_error_reporting;
	zval user_error_handler;
	zval user_exception_handler;
	zend_stack user_error_handlers_error_reporting;
	zend_stack user_error_handlers;
	zend_stack user_exception_handlers;

	zend_error_handling_t  error_handling;
	zend_class_entry      *exception_class;

	/* timeout support */
	zend_long timeout_seconds;

	int lambda_count;

	HashTable *ini_directives;
	HashTable *modified_ini_directives;
	zend_ini_entry *error_reporting_ini_entry;

	zend_objects_store objects_store;
	zend_object *exception, *prev_exception;
	const zend_op *opline_before_exception;
	zend_op exception_op[3];

	struct _zend_module_entry *current_module;

	zend_bool active;
	zend_uchar flags;

	zend_long assertions;

	uint32_t           ht_iterators_count;     /* number of allocatd slots */
	uint32_t           ht_iterators_used;      /* number of used slots */
	HashTableIterator *ht_iterators;
	HashTableIterator  ht_iterators_slots[16];

	void *saved_fpu_cw_ptr;
#if XPFPA_HAVE_CW
	XPFPA_CW_DATATYPE saved_fpu_cw;
#endif

	zend_function trampoline;
	zend_op       call_trampoline_op;

	zend_bool each_deprecation_thrown;

	HashTable weakrefs;

	zend_bool exception_ignore_args;

	void *reserved[ZEND_MAX_RESERVED_RESOURCES];
};

#define EG_FLAGS_INITIAL				(0)
#define EG_FLAGS_IN_SHUTDOWN			(1<<0)
#define EG_FLAGS_OBJECT_STORE_NO_REUSE	(1<<1)
#define EG_FLAGS_IN_RESOURCE_SHUTDOWN	(1<<2)

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

	/* Modes are: ZEND_INI_SCANNER_NORMAL, ZEND_INI_SCANNER_RAW, ZEND_INI_SCANNER_TYPED */
	int scanner_mode;
};

typedef enum {
	ON_TOKEN,
	ON_FEEDBACK,
	ON_STOP
} zend_php_scanner_event;

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
	zend_ptr_stack heredoc_label_stack;
	zend_bool heredoc_scan_ahead;
	int heredoc_indentation;
	zend_bool heredoc_indentation_uses_spaces;

	/* original (unfiltered) script */
	unsigned char *script_org;
	size_t script_org_size;

	/* filtered script */
	unsigned char *script_filtered;
	size_t script_filtered_size;

	/* input/output filters */
	zend_encoding_filter input_filter;
	zend_encoding_filter output_filter;
	const zend_encoding *script_encoding;

	/* initial string length after scanning to first variable */
	int scanned_string_len;

	/* hooks */
	void (*on_event)(zend_php_scanner_event event, int token, int line, void *context);
	void *on_event_context;
};

#endif /* ZEND_GLOBALS_H */
