/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2001 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.92 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_92.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
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
#include "zend_fast_cache.h"

/* Define ZTS if you want a thread-safe Zend */
/*#undef ZTS*/

#ifdef ZTS
#ifdef __cplusplus
class ZendFlexLexer;
class ZendIniFlexLexer;
#endif

BEGIN_EXTERN_C()
ZEND_API extern int compiler_globals_id;
ZEND_API extern int executor_globals_id;
ZEND_API extern int alloc_globals_id;
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

	zend_class_entry class_entry, *active_class_entry;
	zval active_ce_parent_class_name;

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

#if defined(ZTS) && defined(__cplusplus)
	ZendFlexLexer *ZFL;
	ZendIniFlexLexer *ini_scanner;
#else
	void *ZFL;
	void *ini_parser;
#endif

	struct _zend_ini_parser_param *ini_parser_param;

	int interactive;

	zend_bool increment_lineno;
};


struct _zend_executor_globals {
	zval **return_value_ptr_ptr;

	zval uninitialized_zval;
	zval *uninitialized_zval_ptr;

	zval error_zval;
	zval *error_zval_ptr;

	zend_function_state *function_state_ptr;
	zend_ptr_stack arg_types_stack;

	/* for global return() support */
	zval *global_return_value_ptr;

	/* symbol table cache */
	HashTable *symtable_cache[SYMTABLE_CACHE_SIZE];
	HashTable **symtable_cache_limit;
	HashTable **symtable_cache_ptr;

	zend_op **opline_ptr;

	HashTable *active_symbol_table;
	HashTable symbol_table;		/* main symbol table */

	HashTable included_files;	/* files already included */

	jmp_buf bailout;

	int error_reporting;
	int orig_error_reporting;
	int exit_status;

	zend_op_array *active_op_array;

	HashTable *function_table;	/* function symbol table */
	HashTable *class_table;		/* class table */
	HashTable *zend_constants;	/* constants table */

	long precision;

	int ticks_count;

	zend_bool in_execution;
	zend_bool bailout_set;

	/* for extended information support */
	zend_bool no_extensions;

#ifdef ZEND_WIN32
	zend_bool timed_out;
#endif

	HashTable regular_list;
	HashTable persistent_list;

	zend_ptr_stack argument_stack;
	int free_op1, free_op2;
	int (*unary_op)(zval *result, zval *op1);
	int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC);

	zval *garbage[2];
	int garbage_ptr;

	zval *user_error_handler;
	zend_ptr_stack user_error_handlers;

	/* timeout support */
	int timeout_seconds;

	int lambda_count;

	HashTable ini_directives;
	zend_objects objects;

	void *reserved[ZEND_MAX_RESERVED_RESOURCES];
};


struct _zend_alloc_globals {
	zend_mem_header *head;		/* standard list */
	zend_mem_header *phead;		/* persistent list */
	void *cache[MAX_CACHED_MEMORY][MAX_CACHED_ENTRIES];
	unsigned int cache_count[MAX_CACHED_MEMORY];
	void *fast_cache_list_head[MAX_FAST_CACHE_TYPES];

#ifdef ZEND_WIN32
	HANDLE memory_heap;
#endif

#if ZEND_DEBUG
	/* for performance tuning */
	int cache_stats[MAX_CACHED_MEMORY][2];
	int fast_cache_stats[MAX_FAST_CACHE_TYPES][2];
#endif
#if MEMORY_LIMIT
	unsigned int memory_limit;
	unsigned int allocated_memory;
	unsigned int allocated_memory_peak;
	unsigned char memory_exhausted;
#endif
};


struct _zend_scanner_globals {
	FILE *yyin;
	FILE *yyout;
	int yy_leng;
	char *yy_text;
	struct yy_buffer_state *current_buffer;
	char *c_buf_p;
	int init;
	int start;
};

#endif /* ZEND_GLOBALS_H */
