/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.91 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_91.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef _T_GLOBALS_H
#define _T_GLOBALS_H

#include <setjmp.h>

#include "zend_stack.h"
#include "zend_ptr_stack.h"
#include "zend_hash.h"
#include "zend_llist.h"

/* Define ZTS if you want a thread-safe Zend */
/*#undef ZTS*/

#ifdef ZTS
#include "../TSRM/TSRM.h"

#ifdef __cplusplus
class ZendFlexLexer;
#endif

BEGIN_EXTERN_C()
ZEND_API extern int compiler_globals_id;
ZEND_API extern int executor_globals_id;
extern int alloc_globals_id;
END_EXTERN_C()

#endif

typedef struct _zend_compiler_globals zend_compiler_globals;
typedef struct _zend_executor_globals zend_executor_globals;
typedef struct _zend_alloc_globals zend_alloc_globals;

#define SYMTABLE_CACHE_SIZE 32


/* Compiler */
#ifdef ZTS
# define CLS_D	zend_compiler_globals *compiler_globals
# define CLS_DC	, CLS_D
# define CLS_C	compiler_globals
# define CLS_CC , CLS_C
# define CG(v) (((zend_compiler_globals *) compiler_globals)->v)
# define CLS_FETCH()	zend_compiler_globals *compiler_globals = (zend_compiler_globals *) ts_resource(compiler_globals_id)
# define YYPARSE_PARAM compiler_globals
# define YYLEX_PARAM compiler_globals
BEGIN_EXTERN_C()
int zendparse(void *compiler_globals);
END_EXTERN_C()
#else
# define CLS_D
# define CLS_DC
# define CLS_C
# define CLS_CC
# define CG(v) (compiler_globals.v)
# define CLS_FETCH()
extern ZEND_API struct _zend_compiler_globals compiler_globals;
int zendparse();
#endif


/* Executor */
#ifdef ZTS
# define ELS_D	zend_executor_globals *executor_globals
# define ELS_DC	, ELS_D
# define ELS_C	executor_globals
# define ELS_CC , ELS_C
# define EG(v) (executor_globals->v)
# define ELS_FETCH()	zend_executor_globals *executor_globals = (zend_executor_globals *) ts_resource(executor_globals_id)
#else
# define ELS_D
# define ELS_DC
# define ELS_C
# define ELS_CC
# define EG(v) (executor_globals.v)
# define ELS_FETCH()
extern ZEND_API zend_executor_globals executor_globals;
#endif


/* Memory Manager */
#ifdef ZTS
# define ALS_D	zend_alloc_globals *alloc_globals
# define ALS_DC	, ALS_D
# define ALS_C	alloc_globals
# define ALS_CC , ALS_C
# define AG(v) (((zend_alloc_globals *) alloc_globals)->v)
# define ALS_FETCH()	zend_alloc_globals *alloc_globals = (zend_alloc_globals *) ts_resource(alloc_globals_id)
#else
# define ALS_D
# define ALS_DC
# define ALS_C
# define ALS_CC
# define AG(v) (alloc_globals.v)
# define ALS_FETCH()
#endif

#include "zend_compile.h"
#include "zend_execute.h"

struct _zend_compiler_globals {
	zend_stack bp_stack;
	zend_stack switch_cond_stack;
	zend_stack object_stack;

	zend_class_entry class_entry, *active_class_entry;

	/* variables for list() compilation */
	zend_llist list_llist;
	zend_llist dimension_llist;

	zend_stack function_call_stack;

	char *compiled_filename;

	int zend_lineno;
	char *heredoc;
	int heredoc_len;

	zend_op_array *active_op_array;

	HashTable *function_table;	/* function symbol table */
	HashTable *class_table;		/* class table */

	zend_llist filenames_list;

	zend_bool short_tags;
	zend_bool asp_tags;

	/* For extensions support */
	zend_bool extended_info;	/* generate extension information for debugger/profiler */
	zend_bool handle_op_arrays;	/* run op_arrays through op_array handlers */

	zend_bool unclean_shutdown;

	zend_llist open_files;
#ifdef ZTS
#ifdef __cplusplus
	ZendFlexLexer *ZFL;
#else
	void *ZFL;
#endif
#endif
};


struct _zend_executor_globals {
	zval *return_value;

	zval uninitialized_zval;
	zval *uninitialized_zval_ptr;

	zval error_zval;
	zval *error_zval_ptr;

	zend_function_state *function_state_ptr;
	zend_ptr_stack arg_types_stack;
	zend_stack overloaded_objects_stack;
	zval global_return_value;

	/* symbol table cache */
	HashTable *symtable_cache[SYMTABLE_CACHE_SIZE];
	HashTable **symtable_cache_limit;
	HashTable **symtable_cache_ptr;

	zend_op **opline_ptr;

	HashTable *active_symbol_table;
	HashTable symbol_table;		/* main symbol table */

	jmp_buf bailout;

	int error_reporting;

	zend_op_array *active_op_array;
	zend_op_array *main_op_array;

	HashTable *function_table;	/* function symbol table */
	HashTable *class_table;		/* class table */
	HashTable *zend_constants;	/* constants table */

	long precision;
	zend_bool destroying_function_symbol_table;

	/* for extended information support */
	zend_bool no_extensions;

	HashTable regular_list;
	HashTable persistent_list;

	zend_ptr_stack argument_stack;
	int free_op1, free_op2;
	int (*unary_op)(zval *result, zval *op1);
	int (*binary_op)(zval *result, zval *op1, zval *op2);

	void *reserved[ZEND_MAX_RESERVED_RESOURCES];
#if SUPPORT_INTERACTIVE
	int interactive;
#endif
};



struct _zend_alloc_globals {
	mem_header *head;		/* standard list */
	mem_header *phead;		/* persistent list */
	void *cache[MAX_CACHED_MEMORY][MAX_CACHED_ENTRIES];
	unsigned char cache_count[MAX_CACHED_MEMORY];

#if MEMORY_LIMIT
	unsigned int memory_limit;
	unsigned int allocated_memory;
	unsigned char memory_exhausted;
#endif
};


#endif /* _T_GLOBALS_H */
