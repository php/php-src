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

#ifndef ZEND_COMPILE_H
#define ZEND_COMPILE_H

#include "zend.h"
#include "zend_ast.h"

#include <stdarg.h>

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
#else
# define ZEND_USE_ABS_JMP_ADDR      0
# define ZEND_USE_ABS_CONST_ADDR    0
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
	unsigned char *ptr;
} zend_parser_stack_elem;

void zend_compile_top_stmt(zend_ast *ast);
void zend_compile_stmt(zend_ast *ast);
void zend_compile_expr(znode *node, zend_ast *ast);
zend_op *zend_compile_var(znode *node, zend_ast *ast, uint32_t type, int by_ref);
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
#define ZEND_LIVE_NEW     4
#define ZEND_LIVE_MASK    7

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
/* Visibility flags (public < protected < private)        |     |     |     */
#define ZEND_ACC_PUBLIC                  (1 <<  0) /*     |  X  |  X  |  X  */
#define ZEND_ACC_PROTECTED               (1 <<  1) /*     |  X  |  X  |  X  */
#define ZEND_ACC_PRIVATE                 (1 <<  2) /*     |  X  |  X  |  X  */
/*                                                        |     |     |     */
/* Property or method overrides private one               |     |     |     */
#define ZEND_ACC_CHANGED                 (1 <<  3) /*     |  X  |  X  |     */
/*                                                        |     |     |     */
/* Static method or property                              |     |     |     */
#define ZEND_ACC_STATIC                  (1 <<  4) /*     |  X  |  X  |     */
/*                                                        |     |     |     */
/* Final class or method                                  |     |     |     */
#define ZEND_ACC_FINAL                   (1 <<  5) /*  X  |  X  |     |     */
/*                                                        |     |     |     */
/* Abstract method                                        |     |     |     */
#define ZEND_ACC_ABSTRACT                (1 <<  6) /*  X  |  X  |     |     */
#define ZEND_ACC_EXPLICIT_ABSTRACT_CLASS (1 <<  6) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Immutable op_array and class_entries                   |     |     |     */
/* (implemented only for lazy loading of op_arrays)       |     |     |     */
#define ZEND_ACC_IMMUTABLE               (1 <<  7) /*  X  |  X  |     |     */
/*                                                        |     |     |     */
/* Function has typed arguments / class has typed props   |     |     |     */
#define ZEND_ACC_HAS_TYPE_HINTS          (1 <<  8) /*  X  |  X  |     |     */
/*                                                        |     |     |     */
/* Top-level class or function declaration                |     |     |     */
#define ZEND_ACC_TOP_LEVEL               (1 <<  9) /*  X  |  X  |     |     */
/*                                                        |     |     |     */
/* op_array or class is preloaded                         |     |     |     */
#define ZEND_ACC_PRELOADED               (1 << 10) /*  X  |  X  |     |     */
/*                                                        |     |     |     */
/* Class Flags (unused: 24...)                            |     |     |     */
/* ===========                                            |     |     |     */
/*                                                        |     |     |     */
/* Special class types                                    |     |     |     */
#define ZEND_ACC_INTERFACE               (1 <<  0) /*  X  |     |     |     */
#define ZEND_ACC_TRAIT                   (1 <<  1) /*  X  |     |     |     */
#define ZEND_ACC_ANON_CLASS              (1 <<  2) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Class linked with parent, interfaces and traits        |     |     |     */
#define ZEND_ACC_LINKED                  (1 <<  3) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Class is abstract, since it is set by any              |     |     |     */
/* abstract method                                        |     |     |     */
#define ZEND_ACC_IMPLICIT_ABSTRACT_CLASS (1 <<  4) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Class has magic methods __get/__set/__unset/           |     |     |     */
/* __isset that use guards                                |     |     |     */
#define ZEND_ACC_USE_GUARDS              (1 << 11) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Class constants updated                                |     |     |     */
#define ZEND_ACC_CONSTANTS_UPDATED       (1 << 12) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Class extends another class                            |     |     |     */
#define ZEND_ACC_INHERITED               (1 << 13) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Class implements interface(s)                          |     |     |     */
#define ZEND_ACC_IMPLEMENT_INTERFACES    (1 << 14) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Class uses trait(s)                                    |     |     |     */
#define ZEND_ACC_IMPLEMENT_TRAITS        (1 << 15) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* User class has methods with static variables           |     |     |     */
#define ZEND_HAS_STATIC_IN_METHODS       (1 << 16) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Whether all property types are resolved to CEs         |     |     |     */
#define ZEND_ACC_PROPERTY_TYPES_RESOLVED (1 << 17) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Children must reuse parent get_iterator()              |     |     |     */
#define ZEND_ACC_REUSE_GET_ITERATOR      (1 << 18) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Parent class is resolved (CE).                         |     |     |     */
#define ZEND_ACC_RESOLVED_PARENT         (1 << 19) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Interfaces are resolved (CEs).                         |     |     |     */
#define ZEND_ACC_RESOLVED_INTERFACES     (1 << 20) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Class has unresolved variance obligations.             |     |     |     */
#define ZEND_ACC_UNRESOLVED_VARIANCE     (1 << 21) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Class is linked apart from variance obligations.       |     |     |     */
#define ZEND_ACC_NEARLY_LINKED           (1 << 22) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Whether this class was used in its unlinked state.     |     |     |     */
#define ZEND_ACC_HAS_UNLINKED_USES       (1 << 23) /*  X  |     |     |     */
/*                                                        |     |     |     */
/* Function Flags (unused: 17, 23, 26)                    |     |     |     */
/* ==============                                         |     |     |     */
/*                                                        |     |     |     */
/* deprecation flag                                       |     |     |     */
#define ZEND_ACC_DEPRECATED              (1 << 11) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* Function returning by reference                        |     |     |     */
#define ZEND_ACC_RETURN_REFERENCE        (1 << 12) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* Function has a return type                             |     |     |     */
#define ZEND_ACC_HAS_RETURN_TYPE         (1 << 13) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* Function with variable number of arguments             |     |     |     */
#define ZEND_ACC_VARIADIC                (1 << 14) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* op_array has finally blocks (user only)                |     |     |     */
#define ZEND_ACC_HAS_FINALLY_BLOCK       (1 << 15) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* "main" op_array with                                   |     |     |     */
/* ZEND_DECLARE_CLASS_DELAYED opcodes                     |     |     |     */
#define ZEND_ACC_EARLY_BINDING           (1 << 16) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* call through user function trampoline. e.g.            |     |     |     */
/* __call, __callstatic                                   |     |     |     */
#define ZEND_ACC_CALL_VIA_TRAMPOLINE     (1 << 18) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* disable inline caching                                 |     |     |     */
#define ZEND_ACC_NEVER_CACHE             (1 << 19) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* Closure related                                        |     |     |     */
#define ZEND_ACC_CLOSURE                 (1 << 20) /*     |  X  |     |     */
#define ZEND_ACC_FAKE_CLOSURE            (1 << 21) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* run_time_cache allocated on heap (user only)           |     |     |     */
#define ZEND_ACC_HEAP_RT_CACHE           (1 << 22) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* method flag used by Closure::__invoke() (int only)     |     |     |     */
#define ZEND_ACC_USER_ARG_INFO           (1 << 22) /*     |  X  |     |     */
/*                                                        |     |     |     */
#define ZEND_ACC_GENERATOR               (1 << 24) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* function was processed by pass two (user only)         |     |     |     */
#define ZEND_ACC_DONE_PASS_TWO           (1 << 25) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* internal function is allocated at arena (int only)     |     |     |     */
#define ZEND_ACC_ARENA_ALLOCATED         (1 << 25) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* op_array is a clone of trait method                    |     |     |     */
#define ZEND_ACC_TRAIT_CLONE             (1 << 27) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* functions is a constructor                             |     |     |     */
#define ZEND_ACC_CTOR                    (1 << 28) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* function is a destructor                               |     |     |     */
#define ZEND_ACC_DTOR                    (1 << 29) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* closure uses $this                                     |     |     |     */
#define ZEND_ACC_USES_THIS               (1 << 30) /*     |  X  |     |     */
/*                                                        |     |     |     */
/* op_array uses strict mode types                        |     |     |     */
#define ZEND_ACC_STRICT_TYPES            (1U << 31) /*    |  X  |     |     */


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
	zend_type type;
} zend_property_info;

#define OBJ_PROP(obj, offset) \
	((zval*)((char*)(obj) + offset))
#define OBJ_PROP_NUM(obj, num) \
	(&(obj)->properties_table[(num)])
#define OBJ_PROP_TO_OFFSET(num) \
	((uint32_t)(XtOffsetOf(zend_object, properties_table) + sizeof(zval) * (num)))
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
	ZEND_MAP_PTR_DEF(void **, run_time_cache);
	ZEND_MAP_PTR_DEF(HashTable *, static_variables_ptr);
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
		zend_function *prototype;
		uint32_t num_args;
		uint32_t required_num_args;
		zend_arg_info *arg_info;  /* index -1 represents the return value info, if any */
	} common;

	zend_op_array op_array;
	zend_internal_function internal_function;
};

struct _zend_execute_data {
	const zend_op       *opline;           /* executed opline                */
	zend_execute_data   *call;             /* current call                   */
	zval                *return_value;
	zend_function       *func;             /* executed function              */
	zval                 This;             /* this + call_info + num_args    */
	zend_execute_data   *prev_execute_data;
	zend_array          *symbol_table;
	void               **run_time_cache;   /* cache op_array->run_time_cache */
};

#define ZEND_CALL_HAS_THIS           IS_OBJECT_EX

/* Top 16 bits of Z_TYPE_INFO(EX(This)) are used as call_info flags */
#define ZEND_CALL_FUNCTION           (0 << 16)
#define ZEND_CALL_CODE               (1 << 16)
#define ZEND_CALL_NESTED             (0 << 17)
#define ZEND_CALL_TOP                (1 << 17)
#define ZEND_CALL_ALLOCATED          (1 << 18)
#define ZEND_CALL_FREE_EXTRA_ARGS    (1 << 19)
#define ZEND_CALL_HAS_SYMBOL_TABLE   (1 << 20)
#define ZEND_CALL_RELEASE_THIS       (1 << 21)
#define ZEND_CALL_CLOSURE            (1 << 22)
#define ZEND_CALL_FAKE_CLOSURE       (1 << 23)
#define ZEND_CALL_GENERATOR          (1 << 24)
#define ZEND_CALL_DYNAMIC            (1 << 25)
#define ZEND_CALL_SEND_ARG_BY_REF    (1u << 31)

#define ZEND_CALL_NESTED_FUNCTION    (ZEND_CALL_FUNCTION | ZEND_CALL_NESTED)
#define ZEND_CALL_NESTED_CODE        (ZEND_CALL_CODE | ZEND_CALL_NESTED)
#define ZEND_CALL_TOP_FUNCTION       (ZEND_CALL_TOP | ZEND_CALL_FUNCTION)
#define ZEND_CALL_TOP_CODE           (ZEND_CALL_CODE | ZEND_CALL_TOP)

#define ZEND_CALL_INFO(call) \
	Z_TYPE_INFO((call)->This)

#define ZEND_CALL_KIND_EX(call_info) \
	(call_info & (ZEND_CALL_CODE | ZEND_CALL_TOP))

#define ZEND_CALL_KIND(call) \
	ZEND_CALL_KIND_EX(ZEND_CALL_INFO(call))

#define ZEND_ADD_CALL_FLAG_EX(call_info, flag) do { \
		call_info |= (flag); \
	} while (0)

#define ZEND_DEL_CALL_FLAG_EX(call_info, flag) do { \
		call_info &= ~(flag); \
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

#define RUN_TIME_CACHE(op_array) \
	ZEND_MAP_PTR_GET((op_array)->run_time_cache)

#define ZEND_OP_ARRAY_EXTENSION(op_array, handle) \
	((void**)RUN_TIME_CACHE(op_array))[handle]

#define IS_UNUSED	0		/* Unused operand */
#define IS_CONST	(1<<0)
#define IS_TMP_VAR	(1<<1)
#define IS_VAR		(1<<2)
#define IS_CV		(1<<3)	/* Compiled variable */

/* Used for result.type of smart branch instructions */
#define IS_SMART_BRANCH_JMPZ  (1<<4)
#define IS_SMART_BRANCH_JMPNZ (1<<5)

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
extern ZEND_API zend_op_array *(*zend_compile_string)(zval *source_string, const char *filename);

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

ZEND_API int do_bind_function(zval *lcname);
ZEND_API int do_bind_class(zval *lcname, zend_string *lc_parent_name);
ZEND_API uint32_t zend_build_delayed_early_binding_list(const zend_op_array *op_array);
ZEND_API void zend_do_delayed_early_binding(zend_op_array *op_array, uint32_t first_early_binding_opline);

void zend_do_extended_info(void);
void zend_do_extended_fcall_begin(void);
void zend_do_extended_fcall_end(void);

void zend_verify_namespace(void);

void zend_resolve_goto_label(zend_op_array *op_array, zend_op *opline);

ZEND_API void function_add_ref(zend_function *function);

#define INITIAL_OP_ARRAY_SIZE 64


/* helper functions in zend_language_scanner.l */
ZEND_API zend_op_array *compile_file(zend_file_handle *file_handle, int type);
ZEND_API zend_op_array *compile_string(zval *source_string, const char *filename);
ZEND_API zend_op_array *compile_filename(int type, zval *filename);
ZEND_API int zend_execute_scripts(int type, zval *retval, int file_count, ...);
ZEND_API int open_file_for_scanning(zend_file_handle *file_handle);
ZEND_API void init_op_array(zend_op_array *op_array, zend_uchar type, int initial_ops_size);
ZEND_API void destroy_op_array(zend_op_array *op_array);
ZEND_API void zend_destroy_file_handle(zend_file_handle *file_handle);
ZEND_API void zend_cleanup_internal_class_data(zend_class_entry *ce);
ZEND_API void zend_cleanup_internal_classes(void);

ZEND_API ZEND_COLD void zend_user_exception_handler(void);

#define zend_try_exception_handler() do { \
		if (UNEXPECTED(EG(exception))) { \
			if (Z_TYPE(EG(user_exception_handler)) != IS_UNDEF) { \
				zend_user_exception_handler(); \
			} \
		} \
	} while (0)

void zend_free_internal_arg_info(zend_internal_function *function);
ZEND_API void destroy_zend_function(zend_function *function);
ZEND_API void zend_function_dtor(zval *zv);
ZEND_API void destroy_zend_class(zval *zv);
void zend_class_add_ref(zval *zv);

ZEND_API zend_string *zend_mangle_property_name(const char *src1, size_t src1_length, const char *src2, size_t src2_length, int internal);
#define zend_unmangle_property_name(mangled_property, class_name, prop_name) \
        zend_unmangle_property_name_ex(mangled_property, class_name, prop_name, NULL)
ZEND_API int zend_unmangle_property_name_ex(const zend_string *name, const char **class_name, const char **prop_name, size_t *prop_len);

static zend_always_inline const char *zend_get_unmangled_property_name(const zend_string *mangled_prop) {
	const char *class_name, *prop_name;
	zend_unmangle_property_name(mangled_prop, &class_name, &prop_name);
	return prop_name;
}

#define ZEND_FUNCTION_DTOR zend_function_dtor
#define ZEND_CLASS_DTOR destroy_zend_class

typedef zend_bool (*zend_needs_live_range_cb)(zend_op_array *op_array, zend_op *opline);
ZEND_API void zend_recalc_live_ranges(
	zend_op_array *op_array, zend_needs_live_range_cb needs_live_range);

ZEND_API int pass_two(zend_op_array *op_array);
ZEND_API zend_bool zend_is_compiling(void);
ZEND_API char *zend_make_compiled_string_description(const char *name);
ZEND_API void zend_initialize_class_data(zend_class_entry *ce, zend_bool nullify_handlers);
uint32_t zend_get_class_fetch_type(zend_string *name);
ZEND_API zend_uchar zend_get_call_op(const zend_op *init_op, zend_function *fbc);
ZEND_API int zend_is_smart_branch(const zend_op *opline);

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

void zend_assert_valid_class_name(const zend_string *const_name);

zend_string *zend_type_to_string_resolved(zend_type type, zend_class_entry *scope);
zend_string *zend_type_to_string(zend_type type);

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
#define ZEND_FETCH_CLASS_ALLOW_UNLINKED 0x0400
#define ZEND_FETCH_CLASS_ALLOW_NEARLY_LINKED 0x0800

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

#define ZEND_INTERNAL_FUNCTION		1
#define ZEND_USER_FUNCTION			2
#define ZEND_EVAL_CODE				4

#define ZEND_USER_CODE(type)		((type) != ZEND_INTERNAL_FUNCTION)

#define ZEND_INTERNAL_CLASS         1
#define ZEND_USER_CLASS             2

#define ZEND_EVAL				(1<<0)
#define ZEND_INCLUDE			(1<<1)
#define ZEND_INCLUDE_ONCE		(1<<2)
#define ZEND_REQUIRE			(1<<3)
#define ZEND_REQUIRE_ONCE		(1<<4)

/* global/local fetches */
#define ZEND_FETCH_GLOBAL		(1<<1)
#define ZEND_FETCH_LOCAL		(1<<2)
#define ZEND_FETCH_GLOBAL_LOCK	(1<<3)

#define ZEND_FETCH_TYPE_MASK	0xe

/* Only one of these can ever be in use */
#define ZEND_FETCH_REF			1
#define ZEND_FETCH_DIM_WRITE	2
#define ZEND_FETCH_OBJ_FLAGS	3

#define ZEND_ISEMPTY			(1<<0)

#define ZEND_LAST_CATCH			(1<<0)

#define ZEND_FREE_ON_RETURN     (1<<0)
#define ZEND_FREE_SWITCH        (1<<1)

#define ZEND_SEND_BY_VAL     0u
#define ZEND_SEND_BY_REF     1u
#define ZEND_SEND_PREFER_REF 2u

#define ZEND_DIM_IS					(1 << 0) /* isset fetch needed for null coalesce */
#define ZEND_DIM_ALTERNATIVE_SYNTAX	(1 << 1) /* deprecated curly brace usage */

/* Make sure these don't clash with ZEND_FETCH_CLASS_* flags. */
#define IS_CONSTANT_CLASS                    0x400 /* __CLASS__ in trait */
#define IS_CONSTANT_UNQUALIFIED_IN_NAMESPACE 0x800

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

/* Quick API to check first 12 arguments */
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

#define ZEND_BIND_VAL      0
#define ZEND_BIND_REF      1
#define ZEND_BIND_IMPLICIT 2

#define ZEND_RETURNS_FUNCTION (1<<0)
#define ZEND_RETURNS_VALUE    (1<<1)

#define ZEND_ARRAY_ELEMENT_REF		(1<<0)
#define ZEND_ARRAY_NOT_PACKED		(1<<1)
#define ZEND_ARRAY_SIZE_SHIFT		2

/* Attribute for ternary inside parentheses */
#define ZEND_PARENTHESIZED_CONDITIONAL 1

/* For "use" AST nodes and the seen symbol table */
#define ZEND_SYMBOL_CLASS    (1<<0)
#define ZEND_SYMBOL_FUNCTION (1<<1)
#define ZEND_SYMBOL_CONST    (1<<2)

/* All increment opcodes are even (decrement are odd) */
#define ZEND_IS_INCREMENT(opcode) (((opcode) & 1) == 0)

#define ZEND_IS_BINARY_ASSIGN_OP_OPCODE(opcode) \
	(((opcode) >= ZEND_ADD) && ((opcode) <= ZEND_POW))

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
#define ZEND_INVOKE_FUNC_NAME       "__invoke"
#define ZEND_DEBUGINFO_FUNC_NAME    "__debuginfo"

/* The following constants may be combined in CG(compiler_options)
 * to change the default compiler behavior */

/* generate extended debug information */
#define ZEND_COMPILE_EXTENDED_STMT              (1<<0)
#define ZEND_COMPILE_EXTENDED_FCALL             (1<<1)
#define ZEND_COMPILE_EXTENDED_INFO              (ZEND_COMPILE_EXTENDED_STMT|ZEND_COMPILE_EXTENDED_FCALL)

/* call op_array handler of extendions */
#define ZEND_COMPILE_HANDLE_OP_ARRAY            (1<<2)

/* generate ZEND_INIT_FCALL_BY_NAME for internal functions instead of ZEND_INIT_FCALL */
#define ZEND_COMPILE_IGNORE_INTERNAL_FUNCTIONS  (1<<3)

/* don't perform early binding for classes inherited form internal ones;
 * in namespaces assume that internal class that doesn't exist at compile-time
 * may apper in run-time */
#define ZEND_COMPILE_IGNORE_INTERNAL_CLASSES    (1<<4)

/* generate ZEND_DECLARE_CLASS_DELAYED opcode to delay early binding */
#define ZEND_COMPILE_DELAYED_BINDING            (1<<5)

/* disable constant substitution at compile-time */
#define ZEND_COMPILE_NO_CONSTANT_SUBSTITUTION   (1<<6)

/* disable substitution of persistent constants at compile-time */
#define ZEND_COMPILE_NO_PERSISTENT_CONSTANT_SUBSTITUTION	(1<<8)

/* generate ZEND_INIT_FCALL_BY_NAME for userland functions instead of ZEND_INIT_FCALL */
#define ZEND_COMPILE_IGNORE_USER_FUNCTIONS      (1<<9)

/* force ZEND_ACC_USE_GUARDS for all classes */
#define ZEND_COMPILE_GUARDS						(1<<10)

/* disable builtin special case function calls */
#define ZEND_COMPILE_NO_BUILTINS				(1<<11)

/* result of compilation may be stored in file cache */
#define ZEND_COMPILE_WITH_FILE_CACHE			(1<<12)

/* ignore functions and classes declared in other files */
#define ZEND_COMPILE_IGNORE_OTHER_FILES			(1<<13)

/* this flag is set when compiler invoked by opcache_compile_file() */
#define ZEND_COMPILE_WITHOUT_EXECUTION          (1<<14)

/* this flag is set when compiler invoked during preloading */
#define ZEND_COMPILE_PRELOAD                    (1<<15)

/* disable jumptable optimization for switch statements */
#define ZEND_COMPILE_NO_JUMPTABLES				(1<<16)

/* The default value for CG(compiler_options) */
#define ZEND_COMPILE_DEFAULT					ZEND_COMPILE_HANDLE_OP_ARRAY

/* The default value for CG(compiler_options) during eval() */
#define ZEND_COMPILE_DEFAULT_FOR_EVAL			0

ZEND_API zend_bool zend_binary_op_produces_numeric_string_error(uint32_t opcode, zval *op1, zval *op2);

#endif /* ZEND_COMPILE_H */
