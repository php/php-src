/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2008 Zend Technologies Ltd. (http://www.zend.com) |
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

#include <unicode/ucnv.h>
#include <unicode/ucol.h>
#include <unicode/usearch.h>

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

typedef struct _zend_vm_stack *zend_vm_stack;

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
	zval ***auto_globals_cache;

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

	zstr doc_comment;
	zend_uint doc_comment_len;

	zend_uchar literal_type;

	HashTable script_encodings_table;
	char *script_encoding;

	HashTable *labels;
	zend_stack labels_stack;

	zend_uint compiler_options; /* set of ZEND_COMPILE_* constants */

	zval      *current_namespace;
	HashTable *current_import;

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

	zend_objects_store objects_store;
	zval *exception;
	zend_op *opline_before_exception;
	zend_op exception_op[3];

	struct _zend_execute_data *current_execute_data;

	struct _zend_module_entry *current_module;

	zend_property_info std_property_info;

	zend_bool active; 

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
	
	zend_llist used_state_stacks;

	UConverter *input_conv;     /* converter for flex input */
	UConverter *output_conv;    /* converter for data from flex output */
	zend_bool encoding_checked;
	char* rest_str;
	int rest_len;
};

struct _zend_unicode_globals {
	zend_bool unicode;                   /* indicates whether full Unicode mode is enabled */

	UConverter *fallback_encoding_conv;  /* converter for default encoding for IS_STRING type */
	UConverter *runtime_encoding_conv;   /* runtime encoding converter */
	UConverter *output_encoding_conv;    /* output layer converter */
	UConverter *script_encoding_conv;    /* default script encoding converter */
	UConverter *http_input_encoding_conv;/* http input encoding converter */
	UConverter *filesystem_encoding_conv;/* default filesystem converter (entries, not contents) */ 
	UConverter *utf8_conv;				 /* all-purpose UTF-8 converter */
	UConverter *ascii_conv;				 /* all-purpose ASCII converter */

	char *stream_encoding;		    /* default stream encoding (contents, not FS entries)
						Uses name of encoding rather than a real converter
						because each stream needs its own instance */

	uint16_t from_error_mode;
	UChar from_subst_char[3];
	uint16_t to_error_mode;

	char *default_locale;
	zend_collator *default_collator;
	UCollator *root_collator;
	UStringSearch *root_search;

	HashTable flex_compatible;			 /* table of flex-compatible encodings */

	zval *conv_error_handler;			 /* user-defined conversion error handler */
	zend_ptr_stack conv_error_handlers;  /* stack for user error handlers */
};

#endif /* ZEND_GLOBALS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
