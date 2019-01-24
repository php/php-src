/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 Zend Technologies Ltd. (http://www.zend.com) |
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

#ifndef ZEND_COMPILE_H
#define ZEND_COMPILE_H

#include "zend.h"
#include "zend_ast.h"

#ifdef HAVE_STDARG_H
# include <stdarg.h>
#endif

#include "zend_llist.h"

#define SET_UNUSED(op)  op ## _type = IS_UNUSED

#define MAKE_NOP(opline) do { \
	(opline)->op1.num = 0; \
	(opline)->op2.num = 0; \
	(opline)->result.num = 0; \
	(opline)->opcode = ZEND_NOP; \
	(opline)->op1_type =  IS_UNUSED; \
	(opline)->op2_type = IS_UNUSED; \
	(opline)->result_type = IS_UNUSED; \
} while (0)

#define RESET_DOC_COMMENT() do { \
	if (CG(doc_comment)) { \
		zend_string_release_ex(CG(doc_comment), 0); \
		CG(doc_comment) = NULL; \
	} \
} while (0)

typedef struct _zend_op_array zend_op_array;
typedef struct _zend_op zend_op;

/* On 64-bit systems less optimal, but more compact VM code leads to better
 * performance. So on 32-bit systems we use absolute addresses for jump
 * targets and constants, but on 64-bit systems realtive 32-bit offsets */
#if SIZEOF_SIZE_T == 4
# define ZEND_USE_ABS_JMP_ADDR      1
# define ZEND_USE_ABS_CONST_ADDR    1
# define ZEND_EX_USE_RUN_TIME_CACHE 1
#else
# define ZEND_USE_ABS_JMP_ADDR      0
# define ZEND_USE_ABS_CONST_ADDR    0
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
	zend_uchar op_type;
	zend_uchar flag;
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

ZEND_API zend_ast * ZEND_FASTCALL zend_ast_create_znode(znode *node);

static zend_always_inline znode *zend_ast_get_znode(zend_ast *ast) {
	return &((zend_ast_znode *) ast)->node;
}

typedef struct _zend_declarables {
	zend_long ticks;
} zend_declarables;

/* Compilation context that is different for each file, but shared between op arrays. */
typedef struct _zend_file_context {
	zend_declarables declarables;
	znode implementing_class;

	zend_string *current_namespace;
	zend_bool in_namespace;
	zend_bool has_bracketed_namespaces;

	HashTable *imports;
	HashTable *imports_function;
	HashTable *imports_const;

	HashTable seen_symbols;
} zend_file_context;

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

typedef int (*user_opcode_handler_t) (zend_execute_data *execute_data);

struct _zend_op {
	const void *handler;
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
	zend_bool is_switch;
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

#define ZEND_LIVE_TMPVAR  0
#define ZEND_LIVE_LOOP    1
#define ZEND_LIVE_SILENCE 2
#define ZEND_LIVE_ROPE    3
#define ZEND_LIVE_MASK    3

typedef struct _zend_live_range {
	uint32_t var; /* low bits are used for variable type (ZEND_LIVE_* macros) */
	uint32_t start;
	uint32_t end;
} zend_live_range;

/* Compilation context that is different for each op array. */
typedef struct _zend_oparray_context {
	uint32_t   opcodes_size;
	int        vars_size;
	int        literals_size;
	int        backpatch_count;
	uint32_t   fast_call_var;
	uint32_t   try_catch_offset;
	int        current_brk_cont;
	int        last_brk_cont;
	zend_brk_cont_element *brk_cont_array;
	HashTable *labels;
} zend_oparray_context;

/* Class, property and method flags                  class|meth.|prop.|const*/
/*                                                        |     |     |     */
/* Common flags                                           |     |     |     */
/* ============                                           |     |     |     */
/*                                                        |     |     |     */
/* Staic method or property                               |     |     |     */
#define ZEND_ACC_STATIC                  (1 <<  0) /*     |  X  |  X  |     */
/*                                                        |     |     |     */
/* Final class or method                                  |     |     |     */
#define ZEND_ACC_FINAL                   (1 <<  2) /*  X  |  X  |     |     */
/*                                                        |     |     |     */
/* Visibility flags (public < protected < private)        |     |     |     */
#define ZEND_ACC_PUBLIC                  (1 <<  8) /*     |  X  |  X  |  X  */
#define ZEND_ACC_PROTECTED               (1 <<  9) /*     |  X  |  X  |  X  */
#define ZEND_ACC_PRIVATE                 (1 << 10) /*     |  X  |  X  |  X  */
/*                                                        |     |     |     */
/* TODO: explain the name ???                             |     |     |     */
#define ZEND_ACC_CHANGED                 (1 << 11) /*     |  X  |  X  |     */
/*                                                        |     |     |     */
/* TODO: used only by ext/reflection ???                  |     |     |     */
#define ZEND_ACC_IMPLICIT_PUBLIC         (1 << 12) /*     |  ?  |  ?  |  ?  */
/*                                                        |     |     |     */
/* Shadow of parent's private method/property             |     |     |     */
#define ZEND_ACC_SHADOW                  (1 << 17) /*     |  ?  |  X  |     */
/*                                                        |     |     |     */
/* Class Flags (unused: 0, 1, 3, 11-18, 21, 25...)        |     |     |     */
/* ===========                                            |     |     |     */
/*                                                        |     |     |     */
/* class is abstarct, since it is set by any              |     |     |     */
/* abstract method                                        |     |     |     */
#define ZEND_ACC_IMPLICIT_ABSTRACT_CLASS (1 <<  4) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Class is explicitly defined as abstract by using       |     |     |     */
/* the keyword.                                           |     |     |     */
#define ZEND_ACC_EXPLICIT_ABSTRACT_CLASS (1 <<  5) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Special class types                                    |     |     |     */
#define ZEND_ACC_INTERFACE               (1 <<  6) /*  X  |     |     |     */
#define ZEND_ACC_TRAIT                   (1 <<  7) /*  X  |     |     |     */
#define ZEND_ACC_ANON_CLASS              (1 <<  8) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Bound anonymous class                                  |     |     |     */
#define ZEND_ACC_ANON_BOUND              (1 <<  9) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Class extends another class                            |     |     |     */
#define ZEND_ACC_INHERITED               (1 << 10) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Class implements interface(s)                          |     |     |     */
#define ZEND_ACC_IMPLEMENT_INTERFACES    (1 << 19) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Class constants updated                                |     |     |     */
#define ZEND_ACC_CONSTANTS_UPDATED       (1 << 20) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Class uses trait(s)                                    |     |     |     */
#define ZEND_ACC_IMPLEMENT_TRAITS        (1 << 22) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* User class has methods with static variables           |     |     |     */
#define ZEND_HAS_STATIC_IN_METHODS       (1 << 23) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Class has magic methods __get/__set/__unset/           |     |     |     */
/* __isset that use guards                                |     |     |     */
#define ZEND_ACC_USE_GUARDS              (1 << 24) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Function Flags (unused: 4, 5, 17?)                     |     |     |     */
/* ==============                                         |     |     |     */
/*                                                        |     |     |     */
/* Abstarct method                                        |     |     |     */
#define ZEND_ACC_ABSTRACT                (1 <<  1) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* TODO: used only during inheritance ???                 |     |     |     */
#define ZEND_ACC_IMPLEMENTED_ABSTRACT    (1 <<  3) /*     |  X  |     |     */
/*                                                        |     |     |     */
#define ZEND_ACC_FAKE_CLOSURE            (1 <<  6) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* method flag used by Closure::__invoke()                |     |     |     */
#define ZEND_ACC_USER_ARG_INFO           (1 <<  7) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* method flags (special method detection)                |     |     |     */
#define ZEND_ACC_CTOR                    (1 << 13) /*     |  X  |     |     */
#define ZEND_ACC_DTOR                    (1 << 14) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* "main" op_array with                                   |     |     |     */
/* ZEND_DECLARE_INHERITED_CLASS_DELAYED opcodes           |     |     |     */
#define ZEND_ACC_EARLY_BINDING           (1 << 15) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* method flag (bc only), any method that has this        |     |     |     */
/* flag can be used statically and non statically.        |     |     |     */
#define ZEND_ACC_ALLOW_STATIC            (1 << 16) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* deprecation flag                                       |     |     |     */
#define ZEND_ACC_DEPRECATED              (1 << 18) /*     |  X  |     |     */
/*                                                        |     |     |     */
#define ZEND_ACC_NO_RT_ARENA             (1 << 19) /*     |  X  |     |     */
/*                                                        |     |     |     */
#define ZEND_ACC_CLOSURE                 (1 << 20) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* call through user function trampoline. e.g.            |     |     |     */
/* __call, __callstatic                                   |     |     |     */
#define ZEND_ACC_CALL_VIA_TRAMPOLINE     (1 << 21) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* disable inline caching                                 |     |     |     */
#define ZEND_ACC_NEVER_CACHE             (1 << 22) /*     |  X  |     |     */
/*                                                        |     |     |     */
#define ZEND_ACC_GENERATOR               (1 << 23) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* Function with varable number of arguments              |     |     |     */
#define ZEND_ACC_VARIADIC                (1 << 24) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* Immutable op_array (lazy loading)                      |     |     |     */
#define ZEND_ACC_IMMUTABLE               (1 << 25) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* Function returning by reference                        |     |     |     */
#define ZEND_ACC_RETURN_REFERENCE        (1 << 26) /*     |  X  |     |     */
/*                                                        |     |     |     */
#define ZEND_ACC_DONE_PASS_TWO           (1 << 27) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* Function has typed arguments                           |     |     |     */
#define ZEND_ACC_HAS_TYPE_HINTS          (1 << 28) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* op_array has finally blocks (user only)                |     |     |     */
#define ZEND_ACC_HAS_FINALLY_BLOCK       (1 << 29) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* internal function is allocated at arena (int only)     |     |     |     */
#define ZEND_ACC_ARENA_ALLOCATED         (1 << 29) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* Function has a return type                             |     |     |     */
#define ZEND_ACC_HAS_RETURN_TYPE         (1 << 30) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* op_array uses strict mode types                        |     |     |     */
#define ZEND_ACC_STRICT_TYPES            (1 << 31) /*     |  X  |     |     */


#define ZEND_ACC_PPP_MASK  (ZEND_ACC_PUBLIC | ZEND_ACC_PROTECTED | ZEND_ACC_PRIVATE)

/* call through internal function handler. e.g. Closure::invoke() */
#define ZEND_ACC_CALL_VIA_HANDLER     ZEND_ACC_CALL_VIA_TRAMPOLINE

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

typedef struct _zend_class_constant {
	zval value; /* access flags are stored in reserved: zval.u2.access_flags */
	zend_string *doc_comment;
	zend_class_entry *ce;
} zend_class_constant;

/* arg_info for internal functions */
typedef struct _zend_internal_arg_info {
	const char *name;
	zend_type type;
	zend_uchar pass_by_reference;
	zend_bool is_variadic;
} zend_internal_arg_info;

/* arg_info for user functions */
typedef struct _zend_arg_info {
	zend_string *name;
	zend_type type;
	zend_uchar pass_by_reference;
	zend_bool is_variadic;
} zend_arg_info;

/* the following structure repeats the layout of zend_internal_arg_info,
 * but its fields have different meaning. It's used as the first element of
 * arg_info array to define properties of internal functions.
 * It's also used for the return type.
 */
typedef struct _zend_internal_function_info {
	zend_uintptr_t required_num_args;
	zend_type type;
	zend_bool return_reference;
	zend_bool _is_variadic;
} zend_internal_function_info;

struct _zend_op_array {
	/* Common elements */
	zend_uchar type;
	zend_uchar arg_flags[3]; /* bitset of arg_info.pass_by_reference */
	uint32_t fn_flags;
	zend_string *function_name;
	zend_class_entry *scope;
	zend_function *prototype;
	uint32_t num_args;
	uint32_t required_num_args;
	zend_arg_info *arg_info;
	/* END of common elements */

	int cache_size;     /* number of run_time_cache_slots * sizeof(void*) */
	int last_var;       /* number of CV variables */
	uint32_t T;         /* number of temporary variables */
	uint32_t last;      /* number of opcodes */

	zend_op *opcodes;
	void **run_time_cache;
	HashTable *static_variables;
	zend_string **vars; /* names of CV variables */

	uint32_t *refcount;

	int last_live_range;
	int last_try_catch;
	zend_live_range *live_range;
	zend_try_catch_element *try_catch_array;

	zend_string *filename;
	uint32_t line_start;
	uint32_t line_end;
	zend_string *doc_comment;

	int last_literal;
	zval *literals;

	void *reserved[ZEND_MAX_RESERVED_RESOURCES];
};


#define ZEND_RETURN_VALUE				0
#define ZEND_RETURN_REFERENCE			1

/* zend_internal_function_handler */
typedef void (ZEND_FASTCALL *zif_handler)(INTERNAL_FUNCTION_PARAMETERS);

typedef struct _zend_internal_function {
	/* Common elements */
	zend_uchar type;
	zend_uchar arg_flags[3]; /* bitset of arg_info.pass_by_reference */
	uint32_t fn_flags;
	zend_string* function_name;
	zend_class_entry *scope;
	zend_function *prototype;
	uint32_t num_args;
	uint32_t required_num_args;
	zend_internal_arg_info *arg_info;
	/* END of common elements */

	zif_handler handler;
	struct _zend_module_entry *module;
	void *reserved[ZEND_MAX_RESERVED_RESOURCES];
} zend_internal_function;

#define ZEND_FN_SCOPE_NAME(function)  ((function) && (function)->common.scope ? ZSTR_VAL((function)->common.scope->name) : "")

union _zend_function {
	zend_uchar type;	/* MUST be the first element of this struct! */
	uint32_t   quick_arg_flags;

	struct {
		zend_uchar type;  /* never used */
		zend_uchar arg_flags[3]; /* bitset of arg_info.pass_by_reference */
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
	zend_function       *func;             /* executed function              */
	zval                 This;             /* this + call_info + num_args    */
	zend_execute_data   *prev_execute_data;
	zend_array          *symbol_table;
#if ZEND_EX_USE_RUN_TIME_CACHE
	void               **run_time_cache;   /* cache op_array->run_time_cache */
#endif
};

#define ZEND_CALL_FUNCTION           (0 << 0)
#define ZEND_CALL_CODE               (1 << 0)
#define ZEND_CALL_NESTED             (0 << 1)
#define ZEND_CALL_TOP                (1 << 1)
#define ZEND_CALL_FREE_EXTRA_ARGS    (1 << 2)
#define ZEND_CALL_CTOR               (1 << 3)
#define ZEND_CALL_HAS_SYMBOL_TABLE   (1 << 4)
#define ZEND_CALL_CLOSURE            (1 << 5)
#define ZEND_CALL_RELEASE_THIS       (1 << 6)
#define ZEND_CALL_ALLOCATED          (1 << 7)
#define ZEND_CALL_GENERATOR          (1 << 8)
#define ZEND_CALL_DYNAMIC            (1 << 9)
#define ZEND_CALL_FAKE_CLOSURE       (1 << 10)
#define ZEND_CALL_SEND_ARG_BY_REF    (1 << 11)

#define ZEND_CALL_INFO_SHIFT         16

#define ZEND_CALL_INFO(call) \
	(Z_TYPE_INFO((call)->This) >> ZEND_CALL_INFO_SHIFT)

#define ZEND_CALL_KIND_EX(call_info) \
	(call_info & (ZEND_CALL_CODE | ZEND_CALL_TOP))

#define ZEND_CALL_KIND(call) \
	ZEND_CALL_KIND_EX(ZEND_CALL_INFO(call))

#define ZEND_SET_CALL_INFO(call, object, info) do { \
		Z_TYPE_INFO((call)->This) = ((object) ? IS_OBJECT_EX : IS_UNDEF) | ((info) << ZEND_CALL_INFO_SHIFT); \
	} while (0)

#define ZEND_ADD_CALL_FLAG_EX(call_info, flag) do { \
		call_info |= ((flag) << ZEND_CALL_INFO_SHIFT); \
	} while (0)

#define ZEND_DEL_CALL_FLAG_EX(call_info, flag) do { \
		call_info &= ~((flag) << ZEND_CALL_INFO_SHIFT); \
	} while (0)

#define ZEND_ADD_CALL_FLAG(call, flag) do { \
		ZEND_ADD_CALL_FLAG_EX(Z_TYPE_INFO((call)->This), flag); \
	} while (0)

#define ZEND_DEL_CALL_FLAG(call, flag) do { \
		ZEND_DEL_CALL_FLAG_EX(Z_TYPE_INFO((call)->This), flag); \
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

#define ZEND_CALL_USES_STRICT_TYPES(call) \
	(((call)->func->common.fn_flags & ZEND_ACC_STRICT_TYPES) != 0)

#define EX_USES_STRICT_TYPES() \
	ZEND_CALL_USES_STRICT_TYPES(execute_data)

#define ZEND_ARG_USES_STRICT_TYPES() \
	(EG(current_execute_data)->prev_execute_data && \
	 EG(current_execute_data)->prev_execute_data->func && \
	 ZEND_CALL_USES_STRICT_TYPES(EG(current_execute_data)->prev_execute_data))

#define ZEND_RET_USES_STRICT_TYPES() \
	ZEND_CALL_USES_STRICT_TYPES(EG(current_execute_data))

#define EX_VAR(n)				ZEND_CALL_VAR(execute_data, n)
#define EX_VAR_NUM(n)			ZEND_CALL_VAR_NUM(execute_data, n)

#define EX_VAR_TO_NUM(n)		((uint32_t)(ZEND_CALL_VAR(NULL, n) - ZEND_CALL_VAR_NUM(NULL, 0)))

#define ZEND_OPLINE_TO_OFFSET(opline, target) \
	((char*)(target) - (char*)(opline))

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

# define ZEND_SET_OP_JMP_ADDR(opline, node, val) do { \
		(node).jmp_addr = (val); \
	} while (0)

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

# define ZEND_SET_OP_JMP_ADDR(opline, node, val) do { \
		(node).jmp_offset = ZEND_OPLINE_TO_OFFSET(opline, val); \
	} while (0)

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
# define RT_CONSTANT(opline, node) \
	(node).zv

/* convert constant from compile-time to run-time */
# define ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, opline, node) do { \
		(node).zv = CT_CONSTANT_EX(op_array, (node).constant); \
	} while (0)

#else

/* At run-time, constants are allocated together with op_array->opcodes
 * and addressed relatively to current opline.
 */

/* run-time constant */
# define RT_CONSTANT(opline, node) \
	((zval*)(((char*)(opline)) + (int32_t)(node).constant))

/* convert constant from compile-time to run-time */
# define ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, opline, node) do { \
		(node).constant = \
			(((char*)CT_CONSTANT_EX(op_array, (node).constant)) - \
			((char*)opline)); \
	} while (0)

#endif

/* convert constant back from run-time to compile-time */
#define ZEND_PASS_TWO_UNDO_CONSTANT(op_array, opline, node) do { \
		(node).constant = RT_CONSTANT(opline, node) - (op_array)->literals; \
	} while (0)

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

#define IS_UNUSED	0		/* Unused operand */
#define IS_CONST	(1<<0)
#define IS_TMP_VAR	(1<<1)
#define IS_VAR		(1<<2)
#define IS_CV		(1<<3)	/* Compiled variable */

#define ZEND_EXTRA_VALUE 1

#include "zend_globals.h"

BEGIN_EXTERN_C()

void init_compiler(void);
void shutdown_compiler(void);
void zend_init_compiler_data_structures(void);

void zend_oparray_context_begin(zend_oparray_context *prev_context);
void zend_oparray_context_end(zend_oparray_context *prev_context);
void zend_file_context_begin(zend_file_context *prev_context);
void zend_file_context_end(zend_file_context *prev_context);

extern ZEND_API zend_op_array *(*zend_compile_file)(zend_file_handle *file_handle, int type);
extern ZEND_API zend_op_array *(*zend_compile_string)(zval *source_string, char *filename);

ZEND_API int ZEND_FASTCALL lex_scan(zval *zendlval, zend_parser_stack_elem *elem);
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

typedef int (ZEND_FASTCALL *unary_op_type)(zval *, zval *);
typedef int (ZEND_FASTCALL *binary_op_type)(zval *, zval *, zval *);

ZEND_API unary_op_type get_unary_op(int opcode);
ZEND_API binary_op_type get_binary_op(int opcode);

void zend_stop_lexing(void);
void zend_emit_final_return(int return_one);

/* Used during AST construction */
zend_ast *zend_ast_append_str(zend_ast *left, zend_ast *right);
zend_ast *zend_negate_num_string(zend_ast *ast);
uint32_t zend_add_class_modifier(uint32_t flags, uint32_t new_flag);
uint32_t zend_add_member_modifier(uint32_t flags, uint32_t new_flag);
zend_bool zend_handle_encoding_declaration(zend_ast *ast);

/* parser-driven code generators */
void zend_do_free(znode *op1);

ZEND_API int do_bind_function(const zend_op_array *op_array, const zend_op *opline, HashTable *function_table, zend_bool compile_time);
ZEND_API zend_class_entry *do_bind_class(const zend_op_array *op_array, const zend_op *opline, HashTable *class_table, zend_bool compile_time);
ZEND_API zend_class_entry *do_bind_inherited_class(const zend_op_array *op_array, const zend_op *opline, HashTable *class_table, zend_class_entry *parent_ce, zend_bool compile_time);
ZEND_API uint32_t zend_build_delayed_early_binding_list(const zend_op_array *op_array);
ZEND_API void zend_do_delayed_early_binding(const zend_op_array *op_array, uint32_t first_early_binding_opline);

void zend_do_extended_info(void);
void zend_do_extended_fcall_begin(void);
void zend_do_extended_fcall_end(void);

void zend_verify_namespace(void);

void zend_resolve_goto_label(zend_op_array *op_array, zend_op *opline);

ZEND_API void function_add_ref(zend_function *function);

#define INITIAL_OP_ARRAY_SIZE 64


/* helper functions in zend_language_scanner.l */
ZEND_API zend_op_array *compile_file(zend_file_handle *file_handle, int type);
ZEND_API zend_op_array *compile_string(zval *source_string, char *filename);
ZEND_API zend_op_array *compile_filename(int type, zval *filename);
ZEND_API void zend_try_exception_handler();
ZEND_API int zend_execute_scripts(int type, zval *retval, int file_count, ...);
ZEND_API int open_file_for_scanning(zend_file_handle *file_handle);
ZEND_API void init_op_array(zend_op_array *op_array, zend_uchar type, int initial_ops_size);
ZEND_API void destroy_op_array(zend_op_array *op_array);
ZEND_API void zend_destroy_file_handle(zend_file_handle *file_handle);
ZEND_API void zend_cleanup_internal_class_data(zend_class_entry *ce);
ZEND_API void zend_cleanup_internal_classes(void);

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

ZEND_API int pass_two(zend_op_array *op_array);
ZEND_API zend_bool zend_is_compiling(void);
ZEND_API char *zend_make_compiled_string_description(const char *name);
ZEND_API void zend_initialize_class_data(zend_class_entry *ce, zend_bool nullify_handlers);
uint32_t zend_get_class_fetch_type(zend_string *name);
ZEND_API zend_uchar zend_get_call_op(const zend_op *init_op, zend_function *fbc);
ZEND_API int zend_is_smart_branch(zend_op *opline);

static zend_always_inline uint32_t get_next_op_number(zend_op_array *op_array)
{
	return op_array->last;
}

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
ZEND_API zend_bool zend_is_auto_global_str(char *name, size_t len);
ZEND_API size_t zend_dirname(char *path, size_t len);
ZEND_API void zend_set_function_arg_flags(zend_function *func);

int ZEND_FASTCALL zendlex(zend_parser_stack_elem *elem);

int zend_add_literal(zend_op_array *op_array, zval *zv);

void zend_assert_valid_class_name(const zend_string *const_name);

/* BEGIN: OPCODES */

#include "zend_vm_opcodes.h"

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
#define ZEND_FETCH_CLASS_EXCEPTION   0x0200

#define ZEND_PARAM_REF      (1<<0)
#define ZEND_PARAM_VARIADIC (1<<1)

#define ZEND_NAME_FQ       0
#define ZEND_NAME_NOT_FQ   1
#define ZEND_NAME_RELATIVE 2

#define ZEND_TYPE_NULLABLE (1<<8)

#define ZEND_ARRAY_SYNTAX_LIST 1  /* list() */
#define ZEND_ARRAY_SYNTAX_LONG 2  /* array() */
#define ZEND_ARRAY_SYNTAX_SHORT 3 /* [] */

/* var status for backpatching */
#define BP_VAR_R			0
#define BP_VAR_W			1
#define BP_VAR_RW			2
#define BP_VAR_IS			3
#define BP_VAR_FUNC_ARG		4
#define BP_VAR_UNSET		5

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
#define ZEND_FETCH_GLOBAL		(1<<1)
#define ZEND_FETCH_LOCAL		(1<<2)
#define ZEND_FETCH_GLOBAL_LOCK	(1<<3)

#define ZEND_FETCH_TYPE_MASK	0xe

#define ZEND_ISEMPTY			(1<<0)

#define ZEND_LAST_CATCH			(1<<0)

#define ZEND_FREE_ON_RETURN     (1<<0)

#define ZEND_SEND_BY_VAL     0
#define ZEND_SEND_BY_REF     1
#define ZEND_SEND_PREFER_REF 2

#define ZEND_DIM_IS 1

#define IS_CONSTANT_UNQUALIFIED     0x010
#define IS_CONSTANT_CLASS           0x080  /* __CLASS__ in trait */
#define IS_CONSTANT_IN_NAMESPACE    0x100

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

/* Quick API to check firat 12 arguments */
#define MAX_ARG_FLAG_NUM 12

#ifdef WORDS_BIGENDIAN
# define ZEND_SET_ARG_FLAG(zf, arg_num, mask) do { \
		(zf)->quick_arg_flags |= ((mask) << ((arg_num) - 1) * 2); \
	} while (0)
# define ZEND_CHECK_ARG_FLAG(zf, arg_num, mask) \
	(((zf)->quick_arg_flags >> (((arg_num) - 1) * 2)) & (mask))
#else
# define ZEND_SET_ARG_FLAG(zf, arg_num, mask) do { \
		(zf)->quick_arg_flags |= (((mask) << 6) << (arg_num) * 2); \
	} while (0)
# define ZEND_CHECK_ARG_FLAG(zf, arg_num, mask) \
	(((zf)->quick_arg_flags >> (((arg_num) + 3) * 2)) & (mask))
#endif

#define QUICK_ARG_MUST_BE_SENT_BY_REF(zf, arg_num) \
	ZEND_CHECK_ARG_FLAG(zf, arg_num, ZEND_SEND_BY_REF)

#define QUICK_ARG_SHOULD_BE_SENT_BY_REF(zf, arg_num) \
	ZEND_CHECK_ARG_FLAG(zf, arg_num, ZEND_SEND_BY_REF|ZEND_SEND_PREFER_REF)

#define QUICK_ARG_MAY_BE_SENT_BY_REF(zf, arg_num) \
	ZEND_CHECK_ARG_FLAG(zf, arg_num, ZEND_SEND_PREFER_REF)

#define ZEND_RETURN_VAL 0
#define ZEND_RETURN_REF 1

#define ZEND_BIND_VAL 0
#define ZEND_BIND_REF 1

#define ZEND_RETURNS_FUNCTION (1<<0)
#define ZEND_RETURNS_VALUE    (1<<1)

#define ZEND_ARRAY_ELEMENT_REF		(1<<0)
#define ZEND_ARRAY_NOT_PACKED		(1<<1)
#define ZEND_ARRAY_SIZE_SHIFT		2

/* For "use" AST nodes and the seen symbol table */
#define ZEND_SYMBOL_CLASS    (1<<0)
#define ZEND_SYMBOL_FUNCTION (1<<1)
#define ZEND_SYMBOL_CONST    (1<<2)

/* Pseudo-opcodes that are used only temporarily during compilation */
#define ZEND_GOTO  253
#define ZEND_BRK   254
#define ZEND_CONT  255


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
#define ZEND_COMPILE_EXTENDED_INFO              (1<<0)

/* call op_array handler of extendions */
#define ZEND_COMPILE_HANDLE_OP_ARRAY            (1<<1)

/* generate ZEND_INIT_FCALL_BY_NAME for internal functions instead of ZEND_INIT_FCALL */
#define ZEND_COMPILE_IGNORE_INTERNAL_FUNCTIONS  (1<<2)

/* don't perform early binding for classes inherited form internal ones;
 * in namespaces assume that internal class that doesn't exist at compile-time
 * may apper in run-time */
#define ZEND_COMPILE_IGNORE_INTERNAL_CLASSES    (1<<3)

/* generate ZEND_DECLARE_INHERITED_CLASS_DELAYED opcode to delay early binding */
#define ZEND_COMPILE_DELAYED_BINDING            (1<<4)

/* disable constant substitution at compile-time */
#define ZEND_COMPILE_NO_CONSTANT_SUBSTITUTION   (1<<5)

/* disable usage of builtin instruction for strlen() */
#define ZEND_COMPILE_NO_BUILTIN_STRLEN          (1<<6)

/* disable substitution of persistent constants at compile-time */
#define ZEND_COMPILE_NO_PERSISTENT_CONSTANT_SUBSTITUTION	(1<<7)

/* generate ZEND_INIT_FCALL_BY_NAME for userland functions instead of ZEND_INIT_FCALL */
#define ZEND_COMPILE_IGNORE_USER_FUNCTIONS      (1<<8)

/* force ZEND_ACC_USE_GUARDS for all classes */
#define ZEND_COMPILE_GUARDS						(1<<9)

/* disable builtin special case function calls */
#define ZEND_COMPILE_NO_BUILTINS				(1<<10)

/* result of compilation may be stored in file cache */
#define ZEND_COMPILE_WITH_FILE_CACHE			(1<<11)

/* disable jumptable optimization for switch statements */
#define ZEND_COMPILE_NO_JUMPTABLES				(1<<12)

/* The default value for CG(compiler_options) */
#define ZEND_COMPILE_DEFAULT					ZEND_COMPILE_HANDLE_OP_ARRAY

/* The default value for CG(compiler_options) during eval() */
#define ZEND_COMPILE_DEFAULT_FOR_EVAL			0

ZEND_API zend_bool zend_binary_op_produces_numeric_string_error(uint32_t opcode, zval *op1, zval *op2);

#endif /* ZEND_COMPILE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
