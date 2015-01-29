/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
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

#ifndef ZEND_COMPILE_H
#define ZEND_COMPILE_H

#include "zend.h"
#include "zend_ast.h"

#ifdef HAVE_STDARG_H
# include <stdarg.h>
#endif

#include "zend_llist.h"

#define DEBUG_ZEND 0

#define SET_UNUSED(op)  op ## _type = IS_UNUSED

#define MAKE_NOP(opline) do { \
	opline->opcode = ZEND_NOP; \
	memset(&opline->result, 0, sizeof(opline->result)); \
	memset(&opline->op1, 0, sizeof(opline->op1)); \
	memset(&opline->op2, 0, sizeof(opline->op2)); \
	opline->result_type = opline->op1_type = opline->op2_type = IS_UNUSED; \
} while (0)

#define RESET_DOC_COMMENT() do { \
	if (CG(doc_comment)) { \
		zend_string_release(CG(doc_comment)); \
		CG(doc_comment) = NULL; \
	} \
} while (0)

typedef struct _zend_op_array zend_op_array;
typedef struct _zend_op zend_op;

typedef struct _zend_compiler_context {
	uint32_t  opcodes_size;
	int        vars_size;
	int        literals_size;
	int        current_brk_cont;
	int        backpatch_count;
	int        in_finally;
	uint32_t   fast_call_var;
	HashTable *labels;
} zend_compiler_context;

/* On 64-bi systems less optimal, but more compact VM code leads to better
 * performance. So on 32-bit systems we use absolute addresses for jump
 * targets and constants, but on 64-bit systems realtive 32-bit offsets */
#if SIZEOF_SIZE_T == 4
# define ZEND_USE_ABS_JMP_ADDR      1
# define ZEND_USE_ABS_CONST_ADDR    1
# define ZEND_EX_USE_LITERALS       0
# define ZEND_EX_USE_RUN_TIME_CACHE 1
#else
# define ZEND_USE_ABS_JMP_ADDR      0
# define ZEND_USE_ABS_CONST_ADDR    0
# define ZEND_EX_USE_LITERALS       1
# define ZEND_EX_USE_RUN_TIME_CACHE 1
#endif

typedef union _znode_op {
	uint32_t      constant;
	uint32_t      var;
	uint32_t      num;
	uint32_t      opline_num; /*  Needs to be signed */
#if ZEND_USE_ABS_JMP_ADDR
	zend_op       *jmp_addr;
#else
	uint32_t      jmp_offset;
#endif
#if ZEND_USE_ABS_CONST_ADDR
	zval          *zv;
#endif
} znode_op;

typedef struct _znode { /* used only during compilation */
	int op_type;
	union {
		znode_op op;
		zval constant; /* replaced by literal/zv */
	} u;
} znode;

/* Temporarily defined here, to avoid header ordering issues */
typedef struct _zend_ast_znode {
	zend_ast_kind kind;
	zend_ast_attr attr;
	uint32_t lineno;
	znode node;
} zend_ast_znode;
ZEND_API zend_ast *zend_ast_create_znode(znode *node);

static zend_always_inline znode *zend_ast_get_znode(zend_ast *ast) {
	return &((zend_ast_znode *) ast)->node;
}

typedef union _zend_parser_stack_elem {
	zend_ast *ast;
	zend_string *str;
	zend_ulong num;
} zend_parser_stack_elem;

void zend_compile_top_stmt(zend_ast *ast);
void zend_compile_stmt(zend_ast *ast);
void zend_compile_expr(znode *node, zend_ast *ast);
void zend_compile_var(znode *node, zend_ast *ast, uint32_t type);
void zend_eval_const_expr(zend_ast **ast_ptr);
void zend_const_expr_to_zval(zval *result, zend_ast *ast);

#define ZEND_OPCODE_HANDLER_ARGS zend_execute_data *execute_data
#define ZEND_OPCODE_HANDLER_ARGS_PASSTHRU execute_data

typedef int (*user_opcode_handler_t) (ZEND_OPCODE_HANDLER_ARGS);
typedef int (ZEND_FASTCALL *opcode_handler_t) (ZEND_OPCODE_HANDLER_ARGS);

extern ZEND_API opcode_handler_t *zend_opcode_handlers;

struct _zend_op {
	opcode_handler_t handler;
	znode_op op1;
	znode_op op2;
	znode_op result;
	uint32_t extended_value;
	uint32_t lineno;
	zend_uchar opcode;
	zend_uchar op1_type;
	zend_uchar op2_type;
	zend_uchar result_type;
};


typedef struct _zend_brk_cont_element {
	int start;
	int cont;
	int brk;
	int parent;
} zend_brk_cont_element;

typedef struct _zend_label {
	int brk_cont;
	uint32_t opline_num;
} zend_label;

typedef struct _zend_try_catch_element {
	uint32_t try_op;
	uint32_t catch_op;  /* ketchup! */
	uint32_t finally_op;
	uint32_t finally_end;
} zend_try_catch_element;

#if SIZEOF_ZEND_LONG == 8
# ifdef _WIN32
#  define THIS_HASHVAL 6385726429Ui64
# else
#  define THIS_HASHVAL 6385726429ULL
# endif
#else
#define THIS_HASHVAL 2090759133UL
#endif

/* method flags (types) */
#define ZEND_ACC_STATIC			0x01
#define ZEND_ACC_ABSTRACT		0x02
#define ZEND_ACC_FINAL			0x04
#define ZEND_ACC_IMPLEMENTED_ABSTRACT		0x08

/* class flags (types) */
/* ZEND_ACC_IMPLICIT_ABSTRACT_CLASS is used for abstract classes (since it is set by any abstract method even interfaces MAY have it set, too). */
/* ZEND_ACC_EXPLICIT_ABSTRACT_CLASS denotes that a class was explicitly defined as abstract by using the keyword. */
#define ZEND_ACC_IMPLICIT_ABSTRACT_CLASS	0x10
#define ZEND_ACC_EXPLICIT_ABSTRACT_CLASS	0x20
#define ZEND_ACC_INTERFACE		            0x80
#define ZEND_ACC_TRAIT						0x120

/* method flags (visibility) */
/* The order of those must be kept - public < protected < private */
#define ZEND_ACC_PUBLIC		0x100
#define ZEND_ACC_PROTECTED	0x200
#define ZEND_ACC_PRIVATE	0x400
#define ZEND_ACC_PPP_MASK  (ZEND_ACC_PUBLIC | ZEND_ACC_PROTECTED | ZEND_ACC_PRIVATE)

#define ZEND_ACC_CHANGED	0x800
#define ZEND_ACC_IMPLICIT_PUBLIC	0x1000

/* method flags (special method detection) */
#define ZEND_ACC_CTOR		0x2000
#define ZEND_ACC_DTOR		0x4000
#define ZEND_ACC_CLONE		0x8000

/* method flag (bc only), any method that has this flag can be used statically and non statically. */
#define ZEND_ACC_ALLOW_STATIC	0x10000

/* shadow of parent's private method/property */
#define ZEND_ACC_SHADOW 0x20000

/* deprecation flag */
#define ZEND_ACC_DEPRECATED 0x40000

/* class implement interface(s) flag */
#define ZEND_ACC_IMPLEMENT_INTERFACES 0x80000
#define ZEND_ACC_IMPLEMENT_TRAITS	  0x400000

/* class constants updated */
#define ZEND_ACC_CONSTANTS_UPDATED	  0x100000

/* user class has methods with static variables */
#define ZEND_HAS_STATIC_IN_METHODS    0x800000


#define ZEND_ACC_CLOSURE              0x100000
#define ZEND_ACC_GENERATOR            0x800000

/* function flag for internal user call handlers __call, __callstatic */
#define ZEND_ACC_CALL_VIA_HANDLER     0x200000

/* disable inline caching */
#define ZEND_ACC_NEVER_CACHE          0x400000

#define ZEND_ACC_VARIADIC				0x1000000

#define ZEND_ACC_RETURN_REFERENCE		0x4000000
#define ZEND_ACC_DONE_PASS_TWO			0x8000000

/* function has arguments with type hinting */
#define ZEND_ACC_HAS_TYPE_HINTS			0x10000000

/* op_array has finally blocks */
#define ZEND_ACC_HAS_FINALLY_BLOCK		0x20000000

/* internal function is allocated at arena */
#define ZEND_ACC_ARENA_ALLOCATED		0x20000000

/* Function has a return type hint (or class has such non-private function) */
#define ZEND_ACC_HAS_RETURN_TYPE		0x40000000

#define ZEND_CE_IS_TRAIT(ce) (((ce)->ce_flags & ZEND_ACC_TRAIT) == ZEND_ACC_TRAIT)

char *zend_visibility_string(uint32_t fn_flags);

typedef struct _zend_property_info {
	uint32_t offset; /* property offset for object properties or
	                      property index for static properties */
	uint32_t flags;
	zend_string *name;
	zend_string *doc_comment;
	zend_class_entry *ce;
} zend_property_info;

#define OBJ_PROP(obj, offset) \
	((zval*)((char*)(obj) + offset))
#define OBJ_PROP_NUM(obj, num) \
	(&(obj)->properties_table[(num)])
#define OBJ_PROP_TO_OFFSET(num) \
	((uint32_t)(zend_uintptr_t)OBJ_PROP_NUM(((zend_object*)NULL), num))
#define OBJ_PROP_TO_NUM(offset) \
	((offset - OBJ_PROP_TO_OFFSET(0)) / sizeof(zval))

/* arg_info for internal functions */
typedef struct _zend_internal_arg_info {
	const char *name;
	const char *class_name;
	zend_uchar type_hint;
	zend_uchar pass_by_reference;
	zend_bool allow_null;
	zend_bool is_variadic;
} zend_internal_arg_info;

/* arg_info for user functions */
typedef struct _zend_arg_info {
	zend_string *name;
	zend_string *class_name;
	zend_uchar type_hint;
	zend_uchar pass_by_reference;
	zend_bool allow_null;
	zend_bool is_variadic;
} zend_arg_info;

/* the following structure repeats the layout of zend_internal_arg_info,
 * but its fields have different meaning. It's used as the first element of
 * arg_info array to define properties of internal functions.
 * It's also used for return type hinting.
 */
typedef struct _zend_internal_function_info {
	zend_uintptr_t required_num_args;
	const char *class_name;
	zend_uchar type_hint;
	zend_bool return_reference;
	zend_bool allow_null;
	zend_bool _is_variadic;
} zend_internal_function_info;

struct _zend_op_array {
	/* Common elements */
	zend_uchar type;
	uint32_t fn_flags;
	zend_string *function_name;
	zend_class_entry *scope;
	zend_function *prototype;
	uint32_t num_args;
	uint32_t required_num_args;
	zend_arg_info *arg_info;
	/* END of common elements */

	uint32_t *refcount;

	uint32_t this_var;

	uint32_t last;
	zend_op *opcodes;

	int last_var;
	uint32_t T;
	zend_string **vars;

	int last_brk_cont;
	int last_try_catch;
	zend_brk_cont_element *brk_cont_array;
	zend_try_catch_element *try_catch_array;

	/* static variables support */
	HashTable *static_variables;

	zend_string *filename;
	uint32_t line_start;
	uint32_t line_end;
	zend_string *doc_comment;
	uint32_t early_binding; /* the linked list of delayed declarations */

	int last_literal;
	zval *literals;

	int  last_cache_slot;
	void **run_time_cache;

	void *reserved[ZEND_MAX_RESERVED_RESOURCES];
};


#define ZEND_RETURN_VALUE				0
#define ZEND_RETURN_REFERENCE			1

typedef struct _zend_internal_function {
	/* Common elements */
	zend_uchar type;
	uint32_t fn_flags;
	zend_string* function_name;
	zend_class_entry *scope;
	zend_function *prototype;
	uint32_t num_args;
	uint32_t required_num_args;
	zend_internal_arg_info *arg_info;
	/* END of common elements */

	void (*handler)(INTERNAL_FUNCTION_PARAMETERS);
	struct _zend_module_entry *module;
} zend_internal_function;

#define ZEND_FN_SCOPE_NAME(function)  ((function) && (function)->common.scope ? (function)->common.scope->name->val : "")

union _zend_function {
	zend_uchar type;	/* MUST be the first element of this struct! */

	struct {
		zend_uchar type;  /* never used */
		uint32_t fn_flags;
		zend_string *function_name;
		zend_class_entry *scope;
		union _zend_function *prototype;
		uint32_t num_args;
		uint32_t required_num_args;
		zend_arg_info *arg_info;
	} common;

	zend_op_array op_array;
	zend_internal_function internal_function;
};

typedef enum _zend_call_kind {
	ZEND_CALL_NESTED_FUNCTION,	/* stackless VM call to function */
	ZEND_CALL_NESTED_CODE,		/* stackless VM call to include/require/eval */
	ZEND_CALL_TOP_FUNCTION,		/* direct VM call to function from external C code */
	ZEND_CALL_TOP_CODE			/* direct VM call to "main" code from external C code */
} zend_call_kind;

struct _zend_execute_data {
	const zend_op       *opline;           /* executed opline                */
	zend_execute_data   *call;             /* current call                   */
	zval                *return_value;
	zend_function       *func;             /* executed op_array              */
	zval                 This;
#if ZEND_EX_USE_RUN_TIME_CACHE
	void               **run_time_cache;
#endif
#if ZEND_EX_USE_LITERALS
	zval                *literals;
#endif
	zend_class_entry    *called_scope;
	zend_execute_data   *prev_execute_data;
	zend_array          *symbol_table;
};

#define ZEND_CALL_FUNCTION           (0 << 0)
#define ZEND_CALL_CODE               (1 << 0)
#define ZEND_CALL_NESTED             (0 << 1)
#define ZEND_CALL_TOP                (1 << 1)
#define ZEND_CALL_FREE_EXTRA_ARGS    (1 << 2) /* equal to IS_TYPE_REFCOUNTED */
#define ZEND_CALL_CTOR               (1 << 3)
#define ZEND_CALL_CTOR_RESULT_UNUSED (1 << 4)

#define ZEND_CALL_INFO(call) \
	(Z_TYPE_INFO((call)->This) >> 24)

#define ZEND_CALL_KIND(call) \
	(ZEND_CALL_INFO(call) & (ZEND_CALL_CODE | ZEND_CALL_TOP))

#define ZEND_SET_CALL_INFO(call, info) do { \
		Z_TYPE_INFO((call)->This) = IS_OBJECT_EX | ((info) << 24); \
	} while (0)

#define ZEND_ADD_CALL_FLAG(call, info) do { \
		Z_TYPE_INFO((call)->This) |= ((info) << 24); \
	} while (0)

#define ZEND_CALL_NUM_ARGS(call) \
	(call)->This.u2.num_args

#define ZEND_CALL_FRAME_SLOT \
	((int)((ZEND_MM_ALIGNED_SIZE(sizeof(zend_execute_data)) + ZEND_MM_ALIGNED_SIZE(sizeof(zval)) - 1) / ZEND_MM_ALIGNED_SIZE(sizeof(zval))))

#define ZEND_CALL_VAR(call, n) \
	((zval*)(((char*)(call)) + ((int)(n))))

#define ZEND_CALL_VAR_NUM(call, n) \
	(((zval*)(call)) + (ZEND_CALL_FRAME_SLOT + ((int)(n))))

#define ZEND_CALL_ARG(call, n) \
	ZEND_CALL_VAR_NUM(call, ((int)(n)) - 1)

#define EX(element) 			((execute_data)->element)

#define EX_CALL_INFO()			ZEND_CALL_INFO(execute_data)
#define EX_CALL_KIND()			ZEND_CALL_KIND(execute_data)
#define EX_NUM_ARGS()			ZEND_CALL_NUM_ARGS(execute_data)

#define EX_VAR(n)				ZEND_CALL_VAR(execute_data, n)
#define EX_VAR_NUM(n)			ZEND_CALL_VAR_NUM(execute_data, n)

#define EX_VAR_TO_NUM(n)		(ZEND_CALL_VAR(NULL, n) - ZEND_CALL_VAR_NUM(NULL, 0))

#define ZEND_OPLINE_NUM_TO_OFFSET(op_array, opline, opline_num) \
	((char*)&(op_array)->opcodes[opline_num] - (char*)(opline))

#define ZEND_OFFSET_TO_OPLINE(base, offset) \
	((zend_op*)(((char*)(base)) + (int)offset))

#define ZEND_OFFSET_TO_OPLINE_NUM(op_array, base, offset) \
	(ZEND_OFFSET_TO_OPLINE(base, offset) - op_array->opcodes)

#if ZEND_USE_ABS_JMP_ADDR

/* run-time jump target */
# define OP_JMP_ADDR(opline, node) \
	(node).jmp_addr

/* convert jump target from compile-time to run-time */
# define ZEND_PASS_TWO_UPDATE_JMP_TARGET(op_array, opline, node) do { \
		(node).jmp_addr = (op_array)->opcodes + (node).opline_num; \
	} while (0)

/* convert jump target back from run-time to compile-time */
# define ZEND_PASS_TWO_UNDO_JMP_TARGET(op_array, opline, node) do { \
		(node).opline_num = (node).jmp_addr - (op_array)->opcodes; \
	} while (0)

#else

/* run-time jump target */
# define OP_JMP_ADDR(opline, node) \
	ZEND_OFFSET_TO_OPLINE(opline, (node).jmp_offset)

/* convert jump target from compile-time to run-time */
# define ZEND_PASS_TWO_UPDATE_JMP_TARGET(op_array, opline, node) do { \
		(node).jmp_offset = ZEND_OPLINE_NUM_TO_OFFSET(op_array, opline, (node).opline_num); \
	} while (0)

/* convert jump target back from run-time to compile-time */
# define ZEND_PASS_TWO_UNDO_JMP_TARGET(op_array, opline, node) do { \
		(node).opline_num = ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, (node).jmp_offset); \
	} while (0)

#endif

/* constant-time constant */
# define CT_CONSTANT_EX(op_array, num) \
	((op_array)->literals + (num))

# define CT_CONSTANT(node) \
	CT_CONSTANT_EX(CG(active_op_array), (node).constant)

#if ZEND_USE_ABS_CONST_ADDR

/* run-time constant */
# define RT_CONSTANT_EX(base, node) \
	(node).zv

/* convert constant from compile-time to run-time */
# define ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, node) do { \
		(node).zv = CT_CONSTANT_EX(op_array, (node).constant); \
	} while (0)

/* convert constant back from run-time to compile-time */
# define ZEND_PASS_TWO_UNDO_CONSTANT(op_array, node) do { \
		(node).constant = (node).zv - (op_array)->literals; \
	} while (0)

#else

/* run-time constant */
# define RT_CONSTANT_EX(base, node) \
	((zval*)(((char*)(base)) + (node).constant))

/* convert constant from compile-time to run-time */
# define ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, node) do { \
		(node).constant *= sizeof(zval); \
	} while (0)

/* convert constant back from run-time to compile-time (do nothing) */
# define ZEND_PASS_TWO_UNDO_CONSTANT(op_array, node) do { \
		(node).constant /= sizeof(zval); \
	} while (0)

#endif

#if ZEND_EX_USE_LITERALS

# define EX_LITERALS() \
	EX(literals)

# define EX_LOAD_LITERALS(op_array) do { \
		EX(literals) = (op_array)->literals; \
	} while (0)

#else

# define EX_LITERALS() \
	EX(func)->op_array.literals

# define EX_LOAD_LITERALS(op_array) do { \
	} while (0)

#endif

/* run-time constant */
#define RT_CONSTANT(op_array, node) \
	RT_CONSTANT_EX((op_array)->literals, node)

/* constant in currently executed function */
#define EX_CONSTANT(node) \
	RT_CONSTANT_EX(EX_LITERALS(), node)

#if ZEND_EX_USE_RUN_TIME_CACHE

# define EX_RUN_TIME_CACHE() \
	EX(run_time_cache)

# define EX_LOAD_RUN_TIME_CACHE(op_array) do { \
		EX(run_time_cache) = (op_array)->run_time_cache; \
	} while (0)

#else

# define EX_RUN_TIME_CACHE() \
	EX(func)->op_array.run_time_cache

# define EX_LOAD_RUN_TIME_CACHE(op_array) do { \
	} while (0)

#endif

#define IS_CONST	(1<<0)
#define IS_TMP_VAR	(1<<1)
#define IS_VAR		(1<<2)
#define IS_UNUSED	(1<<3)	/* Unused variable */
#define IS_CV		(1<<4)	/* Compiled variable */

#define EXT_TYPE_UNUSED	(1<<5)

#include "zend_globals.h"

BEGIN_EXTERN_C()

void init_compiler(void);
void shutdown_compiler(void);
void zend_init_compiler_data_structures(void);
void zend_init_compiler_context(void);

extern ZEND_API zend_op_array *(*zend_compile_file)(zend_file_handle *file_handle, int type);
extern ZEND_API zend_op_array *(*zend_compile_string)(zval *source_string, char *filename);

ZEND_API int lex_scan(zval *zendlval);
void startup_scanner(void);
void shutdown_scanner(void);

ZEND_API zend_string *zend_set_compiled_filename(zend_string *new_compiled_filename);
ZEND_API void zend_restore_compiled_filename(zend_string *original_compiled_filename);
ZEND_API zend_string *zend_get_compiled_filename(void);
ZEND_API int zend_get_compiled_lineno(void);
ZEND_API size_t zend_get_scanned_file_offset(void);

ZEND_API zend_string *zend_get_compiled_variable_name(const zend_op_array *op_array, uint32_t var);

#ifdef ZTS
const char *zend_get_zendtext(void);
int zend_get_zendleng(void);
#endif


typedef int (*unary_op_type)(zval *, zval *);
typedef int (*binary_op_type)(zval *, zval *, zval *);
ZEND_API unary_op_type get_unary_op(int opcode);
ZEND_API binary_op_type get_binary_op(int opcode);

void zend_stop_lexing(void);
void zend_emit_final_return(zval *zv);
zend_ast *zend_ast_append_str(zend_ast *left, zend_ast *right);
uint32_t zend_add_class_modifier(uint32_t flags, uint32_t new_flag);
uint32_t zend_add_member_modifier(uint32_t flags, uint32_t new_flag);
zend_ast *zend_ast_append_doc_comment(zend_ast *list);
void zend_handle_encoding_declaration(zend_ast *ast);

/* parser-driven code generators */
void zend_do_free(znode *op1);

ZEND_API int do_bind_function(const zend_op_array *op_array, const zend_op *opline, HashTable *function_table, zend_bool compile_time);
ZEND_API zend_class_entry *do_bind_class(const zend_op_array *op_array, const zend_op *opline, HashTable *class_table, zend_bool compile_time);
ZEND_API zend_class_entry *do_bind_inherited_class(const zend_op_array *op_array, const zend_op *opline, HashTable *class_table, zend_class_entry *parent_ce, zend_bool compile_time);
ZEND_API void zend_do_delayed_early_binding(const zend_op_array *op_array);

/* Functions for a null terminated pointer list, used for traits parsing and compilation */
void zend_init_list(void *result, void *item);
void zend_add_to_list(void *result, void *item);

void zend_do_extended_info(void);
void zend_do_extended_fcall_begin(void);
void zend_do_extended_fcall_end(void);

void zend_verify_namespace(void);
void zend_do_end_compilation(void);

void zend_resolve_goto_label(zend_op_array *op_array, zend_op *opline, int pass2);
void zend_release_labels(int temporary);

ZEND_API void function_add_ref(zend_function *function);

#define INITIAL_OP_ARRAY_SIZE 64


/* helper functions in zend_language_scanner.l */
ZEND_API zend_op_array *compile_file(zend_file_handle *file_handle, int type);
ZEND_API zend_op_array *compile_string(zval *source_string, char *filename);
ZEND_API zend_op_array *compile_filename(int type, zval *filename);
ZEND_API int zend_execute_scripts(int type, zval *retval, int file_count, ...);
ZEND_API int open_file_for_scanning(zend_file_handle *file_handle);
ZEND_API void init_op_array(zend_op_array *op_array, zend_uchar type, int initial_ops_size);
ZEND_API void destroy_op_array(zend_op_array *op_array);
ZEND_API void zend_destroy_file_handle(zend_file_handle *file_handle);
ZEND_API void zend_cleanup_user_class_data(zend_class_entry *ce);
ZEND_API void zend_cleanup_internal_class_data(zend_class_entry *ce);
ZEND_API void zend_cleanup_internal_classes(void);
ZEND_API void zend_cleanup_op_array_data(zend_op_array *op_array);
ZEND_API int clean_non_persistent_function_full(zval *zv);
ZEND_API int clean_non_persistent_class_full(zval *zv);

ZEND_API void destroy_zend_function(zend_function *function);
ZEND_API void zend_function_dtor(zval *zv);
ZEND_API void destroy_zend_class(zval *zv);
void zend_class_add_ref(zval *zv);

ZEND_API zend_string *zend_mangle_property_name(const char *src1, size_t src1_length, const char *src2, size_t src2_length, int internal);
#define zend_unmangle_property_name(mangled_property, class_name, prop_name) \
        zend_unmangle_property_name_ex(mangled_property, class_name, prop_name, NULL)
ZEND_API int zend_unmangle_property_name_ex(const zend_string *name, const char **class_name, const char **prop_name, size_t *prop_len);

#define ZEND_FUNCTION_DTOR zend_function_dtor
#define ZEND_CLASS_DTOR destroy_zend_class

zend_op *get_next_op(zend_op_array *op_array);
void init_op(zend_op *op);
int get_next_op_number(zend_op_array *op_array);
int print_class(zend_class_entry *class_entry);
void print_op_array(zend_op_array *op_array, int optimizations);
ZEND_API int pass_two(zend_op_array *op_array);
zend_brk_cont_element *get_next_brk_cont_element(zend_op_array *op_array);
ZEND_API zend_bool zend_is_compiling(void);
ZEND_API char *zend_make_compiled_string_description(const char *name);
ZEND_API void zend_initialize_class_data(zend_class_entry *ce, zend_bool nullify_handlers);
uint32_t zend_get_class_fetch_type(zend_string *name);

typedef zend_bool (*zend_auto_global_callback)(zend_string *name);
typedef struct _zend_auto_global {
	zend_string *name;
	zend_auto_global_callback auto_global_callback;
	zend_bool jit;
	zend_bool armed;
} zend_auto_global;

ZEND_API int zend_register_auto_global(zend_string *name, zend_bool jit, zend_auto_global_callback auto_global_callback);
ZEND_API void zend_activate_auto_globals(void);
ZEND_API zend_bool zend_is_auto_global(zend_string *name);
ZEND_API size_t zend_dirname(char *path, size_t len);

int zendlex(zend_parser_stack_elem *elem);

int zend_add_literal(zend_op_array *op_array, zval *zv);

/* BEGIN: OPCODES */

#include "zend_vm_opcodes.h"

#define ZEND_OP_DATA				137

/* END: OPCODES */

/* class fetches */
#define ZEND_FETCH_CLASS_DEFAULT	0
#define ZEND_FETCH_CLASS_SELF		1
#define ZEND_FETCH_CLASS_PARENT		2
#define ZEND_FETCH_CLASS_STATIC		3
#define ZEND_FETCH_CLASS_AUTO		4
#define ZEND_FETCH_CLASS_INTERFACE	5
#define ZEND_FETCH_CLASS_TRAIT		6
#define ZEND_FETCH_CLASS_MASK        0x0f
#define ZEND_FETCH_CLASS_NO_AUTOLOAD 0x80
#define ZEND_FETCH_CLASS_SILENT      0x0100

/* variable parsing type (compile-time) */
#define ZEND_PARSED_MEMBER				(1<<0)
#define ZEND_PARSED_METHOD_CALL			(1<<1)
#define ZEND_PARSED_STATIC_MEMBER		(1<<2)
#define ZEND_PARSED_FUNCTION_CALL		(1<<3)
#define ZEND_PARSED_VARIABLE			(1<<4)
#define ZEND_PARSED_REFERENCE_VARIABLE	(1<<5)
#define ZEND_PARSED_NEW					(1<<6)
#define ZEND_PARSED_LIST_EXPR			(1<<7)

#define ZEND_PARAM_REF      (1<<0)
#define ZEND_PARAM_VARIADIC (1<<1)

#define ZEND_NAME_FQ       0
#define ZEND_NAME_NOT_FQ   1
#define ZEND_NAME_RELATIVE 2

/* unset types */
#define ZEND_UNSET_REG 0

/* var status for backpatching */
#define BP_VAR_R			0
#define BP_VAR_W			1
#define BP_VAR_RW			2
#define BP_VAR_IS			3
#define BP_VAR_FUNC_ARG		4
#define BP_VAR_UNSET		5
#define BP_VAR_REF          6   /* right-hand side of by-ref assignment */

/* Bottom 3 bits are the type, top bits are arg num for BP_VAR_FUNC_ARG */
#define BP_VAR_SHIFT 3
#define BP_VAR_MASK  7


#define ZEND_INTERNAL_FUNCTION				1
#define ZEND_USER_FUNCTION					2
#define ZEND_OVERLOADED_FUNCTION			3
#define	ZEND_EVAL_CODE						4
#define ZEND_OVERLOADED_FUNCTION_TEMPORARY	5

/* A quick check (type == ZEND_USER_FUNCTION || type == ZEND_EVAL_CODE) */
#define ZEND_USER_CODE(type) ((type & 1) == 0)

#define ZEND_INTERNAL_CLASS         1
#define ZEND_USER_CLASS             2

#define ZEND_EVAL				(1<<0)
#define ZEND_INCLUDE			(1<<1)
#define ZEND_INCLUDE_ONCE		(1<<2)
#define ZEND_REQUIRE			(1<<3)
#define ZEND_REQUIRE_ONCE		(1<<4)

#define ZEND_CT	(1<<0)
#define ZEND_RT (1<<1)

/* global/local fetches */
#define ZEND_FETCH_GLOBAL			0x00000000
#define ZEND_FETCH_LOCAL			0x10000000
#define ZEND_FETCH_STATIC			0x20000000
#define ZEND_FETCH_STATIC_MEMBER	0x30000000
#define ZEND_FETCH_GLOBAL_LOCK		0x40000000
#define ZEND_FETCH_LEXICAL			0x50000000

#define ZEND_FETCH_TYPE_MASK		0x70000000

#define ZEND_FETCH_STANDARD		    0x00000000

#define ZEND_ISSET				    0x02000000
#define ZEND_ISEMPTY			    0x01000000
#define ZEND_ISSET_ISEMPTY_MASK	    (ZEND_ISSET | ZEND_ISEMPTY)
#define ZEND_QUICK_SET			    0x00800000

#define ZEND_FETCH_ARG_MASK         0x000fffff

#define ZEND_FE_FETCH_BYREF	1
#define ZEND_FE_FETCH_WITH_KEY	2

#define EXT_TYPE_FREE_ON_RETURN		(1<<2)

#define ZEND_MEMBER_FUNC_CALL	1<<0

#define ZEND_ARG_SEND_BY_REF (1<<0)
#define ZEND_ARG_COMPILE_TIME_BOUND (1<<1)
#define ZEND_ARG_SEND_FUNCTION (1<<2)
#define ZEND_ARG_SEND_SILENT   (1<<3)

#define ZEND_SEND_BY_VAL     0
#define ZEND_SEND_BY_REF     1
#define ZEND_SEND_PREFER_REF 2

static zend_always_inline int zend_check_arg_send_type(const zend_function *zf, uint32_t arg_num, uint32_t mask)
{
	arg_num--;
	if (UNEXPECTED(arg_num >= zf->common.num_args)) {
		if (EXPECTED((zf->common.fn_flags & ZEND_ACC_VARIADIC) == 0)) {
			return 0;
		}
		arg_num = zf->common.num_args;
	}
	return UNEXPECTED((zf->common.arg_info[arg_num].pass_by_reference & mask) != 0);
}

#define ARG_MUST_BE_SENT_BY_REF(zf, arg_num) \
	zend_check_arg_send_type(zf, arg_num, ZEND_SEND_BY_REF)

#define ARG_SHOULD_BE_SENT_BY_REF(zf, arg_num) \
	zend_check_arg_send_type(zf, arg_num, ZEND_SEND_BY_REF|ZEND_SEND_PREFER_REF)

#define ARG_MAY_BE_SENT_BY_REF(zf, arg_num) \
	zend_check_arg_send_type(zf, arg_num, ZEND_SEND_PREFER_REF)

#define ZEND_RETURN_VAL 0
#define ZEND_RETURN_REF 1


#define ZEND_RETURNS_FUNCTION 1<<0
#define ZEND_RETURNS_VALUE    1<<1

#define ZEND_FAST_RET_TO_CATCH		1
#define ZEND_FAST_RET_TO_FINALLY	2

#define ZEND_FAST_CALL_FROM_CATCH	1
#define ZEND_FAST_CALL_FROM_FINALLY	2

#define ZEND_ARRAY_ELEMENT_REF		(1<<0)
#define ZEND_ARRAY_NOT_PACKED		(1<<1)
#define ZEND_ARRAY_SIZE_SHIFT		2

END_EXTERN_C()

#define ZEND_CLONE_FUNC_NAME		"__clone"
#define ZEND_CONSTRUCTOR_FUNC_NAME	"__construct"
#define ZEND_DESTRUCTOR_FUNC_NAME	"__destruct"
#define ZEND_GET_FUNC_NAME          "__get"
#define ZEND_SET_FUNC_NAME          "__set"
#define ZEND_UNSET_FUNC_NAME        "__unset"
#define ZEND_ISSET_FUNC_NAME        "__isset"
#define ZEND_CALL_FUNC_NAME         "__call"
#define ZEND_CALLSTATIC_FUNC_NAME   "__callstatic"
#define ZEND_TOSTRING_FUNC_NAME     "__tostring"
#define ZEND_AUTOLOAD_FUNC_NAME     "__autoload"
#define ZEND_INVOKE_FUNC_NAME       "__invoke"
#define ZEND_DEBUGINFO_FUNC_NAME    "__debuginfo"

/* The following constants may be combined in CG(compiler_options)
 * to change the default compiler behavior */

/* generate extended debug information */
#define ZEND_COMPILE_EXTENDED_INFO				(1<<0)

/* call op_array handler of extendions */
#define ZEND_COMPILE_HANDLE_OP_ARRAY            (1<<1)

/* generate ZEND_INIT_FCALL_BY_NAME for internal functions instead of ZEND_INIT_FCALL */
#define ZEND_COMPILE_IGNORE_INTERNAL_FUNCTIONS	(1<<2)

/* don't perform early binding for classes inherited form internal ones;
 * in namespaces assume that internal class that doesn't exist at compile-time
 * may apper in run-time */
#define ZEND_COMPILE_IGNORE_INTERNAL_CLASSES	(1<<3)

/* generate ZEND_DECLARE_INHERITED_CLASS_DELAYED opcode to delay early binding */
#define ZEND_COMPILE_DELAYED_BINDING			(1<<4)

/* disable constant substitution at compile-time */
#define ZEND_COMPILE_NO_CONSTANT_SUBSTITUTION	(1<<5)

/* disable usage of builtin instruction for strlen() */
#define ZEND_COMPILE_NO_BUILTIN_STRLEN			(1<<6)

/* The default value for CG(compiler_options) */
#define ZEND_COMPILE_DEFAULT					ZEND_COMPILE_HANDLE_OP_ARRAY

/* The default value for CG(compiler_options) during eval() */
#define ZEND_COMPILE_DEFAULT_FOR_EVAL			0

#endif /* ZEND_COMPILE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
