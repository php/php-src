/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
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

#define FREE_PNODE(znode)	zval_dtor(&znode->u.constant);

#define SET_UNUSED(op)  op ## _type = IS_UNUSED

#define INC_BPC(op_array)	if (op_array->fn_flags & ZEND_ACC_INTERACTIVE) { (CG(context).backpatch_count++); }
#define DEC_BPC(op_array)	if (op_array->fn_flags & ZEND_ACC_INTERACTIVE) { (CG(context).backpatch_count--); }
#define HANDLE_INTERACTIVE()  if (CG(active_op_array)->fn_flags & ZEND_ACC_INTERACTIVE) { execute_new_code(TSRMLS_C); }
#define DO_TICKS()            if (Z_LVAL(CG(declarables).ticks)) { zend_do_ticks(TSRMLS_C); }

#define RESET_DOC_COMMENT()        \
    {                              \
        if (CG(doc_comment)) {     \
          STR_RELEASE(CG(doc_comment));  \
          CG(doc_comment) = NULL;  \
        }                          \
    }

typedef struct _zend_op_array zend_op_array;
typedef struct _zend_op zend_op;

typedef struct _zend_compiler_context {
	zend_uint  opcodes_size;
	int        vars_size;
	int        literals_size;
	int        current_brk_cont;
	int        backpatch_count;
	int        nested_calls;
	int        used_stack;
	int        in_finally;
	HashTable *labels;
} zend_compiler_context;

typedef union _znode_op {
	zend_uint      constant;
	zend_uint      var;
	zend_uint      num;
	zend_ulong     hash;
	zend_uint      opline_num; /*  Needs to be signed */
	zend_op       *jmp_addr;
	zval          *zv;
	void          *ptr;        /* Used for passing pointers from the compile to execution phase, currently used for traits */
} znode_op;

typedef struct _znode { /* used only during compilation */
	int op_type;
	union {
		znode_op op;
		zval constant; /* replaced by literal/zv */
		zend_op_array *op_array;
		zend_ast *ast;
	} u;
	zend_uint EA;      /* extended attributes */
} znode;

/* Temporarily defined here, to avoid header ordering issues */
typedef struct _zend_ast_znode {
	zend_ast_kind kind;
	zend_ast_attr attr;
	zend_uint lineno;
	znode node;
} zend_ast_znode;
ZEND_API zend_ast *zend_ast_create_znode(znode *node);

static inline znode *zend_ast_get_znode(zend_ast *ast) {
	return &((zend_ast_znode *) ast)->node;
}

void zend_ensure_writable_variable(const zend_ast *ast);
void zend_compile_stmt(zend_ast *ast TSRMLS_DC);
void zend_compile_expr(znode *node, zend_ast *ast TSRMLS_DC);
int zend_compile_expr_maybe_ct(znode *node, zend_ast *ast, zend_bool ct_required TSRMLS_DC);
void zend_compile_var(znode *node, zend_ast *ast, int type TSRMLS_DC);
void zend_eval_const_expr(zend_ast **ast_ptr TSRMLS_DC);

typedef struct _zend_execute_data zend_execute_data;

#define ZEND_OPCODE_HANDLER_ARGS zend_execute_data *execute_data TSRMLS_DC
#define ZEND_OPCODE_HANDLER_ARGS_PASSTHRU execute_data TSRMLS_CC

typedef int (*user_opcode_handler_t) (ZEND_OPCODE_HANDLER_ARGS);
typedef int (ZEND_FASTCALL *opcode_handler_t) (ZEND_OPCODE_HANDLER_ARGS);

extern ZEND_API opcode_handler_t *zend_opcode_handlers;

struct _zend_op {
	opcode_handler_t handler;
	znode_op op1;
	znode_op op2;
	znode_op result;
	ulong extended_value;
	uint lineno;
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
	zend_uint opline_num;
} zend_label;

typedef struct _zend_try_catch_element {
	zend_uint try_op;
	zend_uint catch_op;  /* ketchup! */
	zend_uint finally_op;
	zend_uint finally_end;
} zend_try_catch_element;

#if SIZEOF_LONG == 8
#define THIS_HASHVAL 6385726429UL
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
#define ZEND_ACC_FINAL_CLASS	            0x40
#define ZEND_ACC_INTERFACE		            0x80
#define ZEND_ACC_TRAIT						0x120

/* op_array flags */
#define ZEND_ACC_INTERACTIVE				0x10

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

char *zend_visibility_string(zend_uint fn_flags);


typedef struct _zend_property_info {
	zend_uint flags;
	zend_string *name;
	ulong h;
	int offset;
	zend_string *doc_comment;
	zend_class_entry *ce;
} zend_property_info;

typedef struct _zend_arg_info {
	const char *name;			// TODO: convert into zend_string ???
	zend_uint name_len;
	const char *class_name;		// TODO: convert into zend_string ???
	zend_uint class_name_len;
	zend_uchar type_hint;
	zend_uchar pass_by_reference;
	zend_bool allow_null;
	zend_bool is_variadic;
} zend_arg_info;

/* the following structure repeats the layout of zend_arg_info,
 * but its fields have different meaning. It's used as the first element of
 * arg_info array to define properties of internal functions.
 */
typedef struct _zend_internal_function_info {
	const char *_name;
	zend_uint _name_len;
	const char *_class_name;
	zend_uint required_num_args;
	zend_uchar _type_hint;
	zend_bool return_reference;
	zend_bool _allow_null;
	zend_bool _is_variadic;
} zend_internal_function_info;

struct _zend_op_array {
	/* Common elements */
	zend_uchar type;
	zend_string *function_name;
	zend_class_entry *scope;
	zend_uint fn_flags;
	zend_function *prototype;
	zend_uint num_args;
	zend_uint required_num_args;
	zend_arg_info *arg_info;
	/* END of common elements */

	zend_uint *refcount;

	zend_op *opcodes;
	zend_uint last;

	zend_string **vars;
	int last_var;

	zend_uint T;

	zend_uint nested_calls;
	zend_uint used_stack;

	zend_brk_cont_element *brk_cont_array;
	int last_brk_cont;

	zend_try_catch_element *try_catch_array;
	int last_try_catch;
	zend_bool has_finally_block;

	/* static variables support */
	HashTable *static_variables;

	zend_uint this_var;

	zend_string *filename;
	zend_uint line_start;
	zend_uint line_end;
	zend_string *doc_comment;
	zend_uint early_binding; /* the linked list of delayed declarations */

	zval *literals;
	int last_literal;

	void **run_time_cache;
	int  last_cache_slot;

	void *reserved[ZEND_MAX_RESERVED_RESOURCES];
};


#define ZEND_RETURN_VALUE				0
#define ZEND_RETURN_REFERENCE			1

typedef struct _zend_internal_function {
	/* Common elements */
	zend_uchar type;
	zend_string* function_name;
	zend_class_entry *scope;
	zend_uint fn_flags;
	zend_function *prototype;
	zend_uint num_args;
	zend_uint required_num_args;
	zend_arg_info *arg_info;
	/* END of common elements */

	void (*handler)(INTERNAL_FUNCTION_PARAMETERS);
	struct _zend_module_entry *module;
} zend_internal_function;

#define ZEND_FN_SCOPE_NAME(function)  ((function) && (function)->common.scope ? (function)->common.scope->name->val : "")

union _zend_function {
	zend_uchar type;	/* MUST be the first element of this struct! */

	struct {
		zend_uchar type;  /* never used */
		zend_string *function_name;
		zend_class_entry *scope;
		zend_uint fn_flags;
		union _zend_function *prototype;
		zend_uint num_args;
		zend_uint required_num_args;
		zend_arg_info *arg_info;
	} common;

	zend_op_array op_array;
	zend_internal_function internal_function;
};


typedef struct _zend_function_state {
	zend_function *function;
	zval *arguments;
} zend_function_state;

typedef struct _zend_function_call_entry {
	zend_function *fbc;
	zend_uint arg_num;
	zend_bool uses_argument_unpacking;
} zend_function_call_entry;

typedef struct _zend_switch_entry {
	znode cond;
	int default_case;
	int control_var;
} zend_switch_entry;


typedef struct _list_llist_element {
	znode var;
	zend_llist dimensions;
	znode value;
} list_llist_element;

typedef struct _call_slot {
	zend_function     *fbc;
	zend_class_entry  *called_scope;
	zend_object       *object;
	zend_uint          num_additional_args;
	zend_bool          is_ctor_call;
	zend_bool          is_ctor_result_used;
} call_slot;

typedef enum _vm_frame_kind {
	VM_FRAME_NESTED_FUNCTION,	/* stackless VM call to function */
	VM_FRAME_NESTED_CODE,		/* stackless VM call to include/require/eval */
	VM_FRAME_TOP_FUNCTION,		/* direct VM call to function from external C code */
	VM_FRAME_TOP_CODE			/* direct VM call to "main" code from external C code */
} vm_frame_kind;

struct _zend_execute_data {
	struct _zend_op     *opline;           /* executed opline                */
	zend_op_array       *op_array;         /* executed op_array              */
	zend_function_state  function_state;   /* called function and arguments  */
	zend_object         *object;           /* current $this                  */
	zend_class_entry    *scope;            /* function scope (self)          */
	zend_class_entry    *called_scope;     /* function called scope (static) */
	zend_array          *symbol_table;
	void               **run_time_cache;
	zend_execute_data   *prev_execute_data;
	zval                *return_value;
	vm_frame_kind        frame_kind;
	// TODO: simplify call sequence and remove call_* ???
	zval old_error_reporting;
	struct _zend_op *fast_ret; /* used by FAST_CALL/FAST_RET (finally keyword) */
	zend_object *delayed_exception;
	call_slot *call_slots;
	call_slot *call;
};

#define EX(element) execute_data.element

#define EX_VAR_2(ex, n)			((zval*)(((char*)(ex)) + ((int)(n))))
#define EX_VAR_NUM_2(ex, n)     (((zval*)(((char*)(ex))+ZEND_MM_ALIGNED_SIZE(sizeof(zend_execute_data))))+(n))

#define EX_VAR(n)				EX_VAR_2(execute_data, n)
#define EX_VAR_NUM(n)			EX_VAR_NUM_2(execute_data, n)

#define EX_VAR_TO_NUM(n)		(EX_VAR_2(NULL, n) - EX_VAR_NUM_2(NULL, 0))

#define IS_CONST	(1<<0)
#define IS_TMP_VAR	(1<<1)
#define IS_VAR		(1<<2)
#define IS_UNUSED	(1<<3)	/* Unused variable */
#define IS_CV		(1<<4)	/* Compiled variable */

#define EXT_TYPE_UNUSED	(1<<5)

#include "zend_globals.h"

BEGIN_EXTERN_C()

void init_compiler(TSRMLS_D);
void shutdown_compiler(TSRMLS_D);
void zend_init_compiler_data_structures(TSRMLS_D);
void zend_init_compiler_context(TSRMLS_D);

extern ZEND_API zend_op_array *(*zend_compile_file)(zend_file_handle *file_handle, int type TSRMLS_DC);
extern ZEND_API zend_op_array *(*zend_compile_string)(zval *source_string, char *filename TSRMLS_DC);

ZEND_API int lex_scan(zval *zendlval TSRMLS_DC);
void startup_scanner(TSRMLS_D);
void shutdown_scanner(TSRMLS_D);

ZEND_API zend_string *zend_set_compiled_filename(zend_string *new_compiled_filename TSRMLS_DC);
ZEND_API void zend_restore_compiled_filename(zend_string *original_compiled_filename TSRMLS_DC);
ZEND_API zend_string *zend_get_compiled_filename(TSRMLS_D);
ZEND_API int zend_get_compiled_lineno(TSRMLS_D);
ZEND_API size_t zend_get_scanned_file_offset(TSRMLS_D);

zend_string *zend_resolve_non_class_name(zend_string *name, zend_bool *is_fully_qualified, zend_bool case_sensitive, HashTable *current_import_sub TSRMLS_DC);
zend_string *zend_resolve_function_name(zend_string *name, zend_bool *is_fully_qualified TSRMLS_DC);
zend_string *zend_resolve_const_name(zend_string *name, zend_bool *is_fully_qualified TSRMLS_DC);
zend_string *zend_resolve_class_name(zend_string *name, zend_bool is_fully_qualified TSRMLS_DC);
ZEND_API zend_string *zend_get_compiled_variable_name(const zend_op_array *op_array, zend_uint var);

void zend_resolve_class_name_old(znode *class_name TSRMLS_DC);

#ifdef ZTS
const char *zend_get_zendtext(TSRMLS_D);
int zend_get_zendleng(TSRMLS_D);
#endif


/* parser-driven code generators */
void zend_do_assign(znode *result, znode *variable, znode *value TSRMLS_DC);
void zend_do_assign_ref(znode *result, znode *lvar, znode *rvar TSRMLS_DC);
void fetch_simple_variable(znode *result, znode *varname, int bp TSRMLS_DC);
void fetch_simple_variable_ex(znode *result, znode *varname, int bp, zend_uchar op TSRMLS_DC);
void zend_do_fetch_static_variable(znode *varname, znode *static_assignment, int fetch_type TSRMLS_DC);
void zend_do_fetch_global_variable(znode *varname, const znode *static_assignment, int fetch_type TSRMLS_DC);

typedef int (*unary_op_type)(zval *, zval * TSRMLS_DC);
typedef int (*binary_op_type)(zval *, zval *, zval * TSRMLS_DC);
ZEND_API unary_op_type get_unary_op(int opcode);
ZEND_API binary_op_type get_binary_op(int opcode);
ZEND_API void zend_make_immutable_array(zval *zv TSRMLS_DC);

void zend_do_while_cond(znode *expr, znode *close_bracket_token TSRMLS_DC);
void zend_do_while_end(const znode *while_token, const znode *close_bracket_token TSRMLS_DC);
void zend_do_do_while_begin(TSRMLS_D);
void zend_do_do_while_end(const znode *do_token, const znode *expr_open_bracket, znode *expr TSRMLS_DC);


void zend_do_if_cond(znode *cond, znode *closing_bracket_token TSRMLS_DC);
void zend_do_if_after_statement(const znode *closing_bracket_token, unsigned char initialize TSRMLS_DC);
void zend_do_if_end(TSRMLS_D);

void zend_do_for_cond(znode *expr, znode *second_semicolon_token TSRMLS_DC);
void zend_do_for_before_statement(const znode *cond_start, const znode *second_semicolon_token TSRMLS_DC);
void zend_do_for_end(const znode *second_semicolon_token TSRMLS_DC);

void zend_do_pre_incdec(znode *result, znode *op1, zend_uchar op TSRMLS_DC);
void zend_do_post_incdec(znode *result, znode *op1, zend_uchar op TSRMLS_DC);

void zend_do_begin_variable_parse(TSRMLS_D);
void zend_do_end_variable_parse(znode *variable, int type, int arg_offset TSRMLS_DC);

void zend_check_writable_variable(const znode *variable);

void zend_do_free(znode *op1 TSRMLS_DC);

int zend_do_verify_access_types(const znode *current_access_type, const znode *new_modifier);
void zend_do_begin_function_declaration(znode *function_token, znode *function_name, int is_method, int return_reference, znode *fn_flags_znode TSRMLS_DC);
void zend_do_end_function_declaration(const znode *function_token TSRMLS_DC);
void zend_do_receive_param(zend_uchar op, znode *varname, znode *initialization, znode *class_type, zend_bool pass_by_reference, zend_bool is_variadic TSRMLS_DC);
void zend_do_fetch_class(znode *result, znode *class_name TSRMLS_DC);
void zend_do_build_full_name(znode *result, znode *prefix, znode *name, int is_class_member TSRMLS_DC);
void zend_do_return(znode *expr, int do_end_vparse TSRMLS_DC);
void zend_do_handle_exception(TSRMLS_D);

void zend_do_begin_lambda_function_declaration(znode *result, znode *function_token, int return_reference, int is_static TSRMLS_DC);
void zend_do_fetch_lexical_variable(znode *varname, zend_bool is_ref TSRMLS_DC);

void zend_do_try(znode *try_token TSRMLS_DC);
void zend_do_begin_catch(znode *try_token, znode *catch_class, znode *catch_var, znode *first_catch TSRMLS_DC);
void zend_do_bind_catch(znode *try_token, znode *catch_token TSRMLS_DC);
void zend_do_end_catch(znode *catch_token TSRMLS_DC);
void zend_do_finally(znode *finally_token TSRMLS_DC);
void zend_do_end_finally(znode *try_token, znode* catch_token, znode *finally_token TSRMLS_DC);
void zend_do_throw(znode *expr TSRMLS_DC);

ZEND_API int do_bind_function(const zend_op_array *op_array, zend_op *opline, HashTable *function_table, zend_bool compile_time);
ZEND_API zend_class_entry *do_bind_class(const zend_op_array *op_array, const zend_op *opline, HashTable *class_table, zend_bool compile_time TSRMLS_DC);
ZEND_API zend_class_entry *do_bind_inherited_class(const zend_op_array *op_array, const zend_op *opline, HashTable *class_table, zend_class_entry *parent_ce, zend_bool compile_time TSRMLS_DC);
ZEND_API void zend_do_inherit_interfaces(zend_class_entry *ce, const zend_class_entry *iface TSRMLS_DC);
ZEND_API void zend_do_implement_interface(zend_class_entry *ce, zend_class_entry *iface TSRMLS_DC);
void zend_do_implements_interface(znode *interface_znode TSRMLS_DC);

/* Trait related functions */
void zend_do_use_trait(znode *trait_znode TSRMLS_DC);
void zend_prepare_reference(znode *result, znode *class_name, znode *method_name TSRMLS_DC);
void zend_add_trait_precedence(znode *method_reference, znode *trait_list TSRMLS_DC);
void zend_add_trait_alias(znode *method_reference, znode *modifiers, znode *alias TSRMLS_DC);

ZEND_API void zend_do_implement_trait(zend_class_entry *ce, zend_class_entry *trait TSRMLS_DC);
ZEND_API void zend_do_bind_traits(zend_class_entry *ce TSRMLS_DC);

ZEND_API void zend_do_inheritance(zend_class_entry *ce, zend_class_entry *parent_ce TSRMLS_DC);
void zend_do_early_binding(TSRMLS_D);
ZEND_API void zend_do_delayed_early_binding(const zend_op_array *op_array TSRMLS_DC);

void zend_do_brk_cont(zend_uchar op, znode *expr TSRMLS_DC);

void zend_do_switch_cond(const znode *cond TSRMLS_DC);
void zend_do_switch_end(const znode *case_list TSRMLS_DC);
void zend_do_case_before_statement(const znode *case_list, znode *case_token, znode *case_expr TSRMLS_DC);
void zend_do_case_after_statement(znode *result, const znode *case_token TSRMLS_DC);
void zend_do_default_before_statement(const znode *case_list, znode *default_token TSRMLS_DC);

void zend_do_begin_class_declaration(const znode *class_token, znode *class_name, const znode *parent_class_name TSRMLS_DC);
void zend_do_end_class_declaration(const znode *class_token, const znode *parent_token TSRMLS_DC);
void zend_do_declare_property(znode *var_name, znode *value, zend_uint access_type TSRMLS_DC);
void zend_do_declare_class_constant(znode *var_name, znode *value TSRMLS_DC);

void zend_do_halt_compiler_register(TSRMLS_D);

/* Functions for a null terminated pointer list, used for traits parsing and compilation */
void zend_init_list(void *result, void *item TSRMLS_DC);
void zend_add_to_list(void *result, void *item TSRMLS_DC);

void zend_do_foreach_begin(znode *foreach_token, znode *open_brackets_token, znode *array, znode *as_token, int variable TSRMLS_DC);
void zend_do_foreach_cont(znode *foreach_token, const znode *open_brackets_token, const znode *as_token, znode *value, znode *key TSRMLS_DC);
void zend_do_foreach_end(const znode *foreach_token, const znode *as_token TSRMLS_DC);

void zend_do_declare_begin(TSRMLS_D);
void zend_do_declare_stmt(znode *var, znode *val TSRMLS_DC);
void zend_do_declare_end(const znode *declare_token TSRMLS_DC);

void zend_do_extended_info(TSRMLS_D);
void zend_do_extended_fcall_begin(TSRMLS_D);
void zend_do_extended_fcall_end(TSRMLS_D);

void zend_do_ticks(TSRMLS_D);

void zend_do_abstract_method(const znode *function_name, znode *modifiers, const znode *body TSRMLS_DC);

void zend_do_declare_constant(znode *name, znode *value TSRMLS_DC);
void zend_do_build_namespace_name(znode *result, znode *prefix, znode *name TSRMLS_DC);
void zend_do_begin_namespace(znode *name, zend_bool with_brackets TSRMLS_DC);
void zend_do_end_namespace(TSRMLS_D);
void zend_verify_namespace(TSRMLS_D);
void zend_do_use(znode *name, znode *new_name, int is_global TSRMLS_DC);
void zend_do_use_non_class(znode *ns_name, znode *new_name, int is_global, int is_function, zend_bool case_sensitive, HashTable *current_import_sub, HashTable *lookup_table TSRMLS_DC);
void zend_do_use_function(znode *name, znode *new_name, int is_global TSRMLS_DC);
void zend_do_use_const(znode *name, znode *new_name, int is_global TSRMLS_DC);
void zend_do_end_compilation(TSRMLS_D);
void zend_do_constant_expression(znode *result, zend_ast *ast TSRMLS_DC);

void zend_do_label(znode *label TSRMLS_DC);
void zend_do_goto(znode *label TSRMLS_DC);
void zend_resolve_goto_label(zend_op_array *op_array, zend_op *opline, int pass2 TSRMLS_DC);
void zend_release_labels(int temporary TSRMLS_DC);

ZEND_API void function_add_ref(zend_function *function);

#define INITIAL_OP_ARRAY_SIZE 64
#define INITIAL_INTERACTIVE_OP_ARRAY_SIZE 8192


/* helper functions in zend_language_scanner.l */
ZEND_API zend_op_array *compile_file(zend_file_handle *file_handle, int type TSRMLS_DC);
ZEND_API zend_op_array *compile_string(zval *source_string, char *filename TSRMLS_DC);
ZEND_API zend_op_array *compile_filename(int type, zval *filename TSRMLS_DC);
ZEND_API int zend_execute_scripts(int type TSRMLS_DC, zval *retval, int file_count, ...);
ZEND_API int open_file_for_scanning(zend_file_handle *file_handle TSRMLS_DC);
ZEND_API void init_op_array(zend_op_array *op_array, zend_uchar type, int initial_ops_size TSRMLS_DC);
ZEND_API void destroy_op_array(zend_op_array *op_array TSRMLS_DC);
ZEND_API void zend_destroy_file_handle(zend_file_handle *file_handle TSRMLS_DC);
ZEND_API void zend_cleanup_user_class_data(zend_class_entry *ce TSRMLS_DC);
ZEND_API void zend_cleanup_internal_class_data(zend_class_entry *ce TSRMLS_DC);
ZEND_API void zend_cleanup_internal_classes(TSRMLS_D);
ZEND_API void zend_cleanup_op_array_data(zend_op_array *op_array);

ZEND_API void destroy_zend_function(zend_function *function TSRMLS_DC);
ZEND_API void zend_function_dtor(zval *zv);
ZEND_API void destroy_zend_class(zval *zv);
void zend_class_add_ref(zval *zv);

ZEND_API zend_string *zend_mangle_property_name(const char *src1, int src1_length, const char *src2, int src2_length, int internal);
#define zend_unmangle_property_name(mangled_property, mangled_property_len, class_name, prop_name) \
        zend_unmangle_property_name_ex(mangled_property, mangled_property_len, class_name, prop_name, NULL)
ZEND_API int zend_unmangle_property_name_ex(const char *mangled_property, int mangled_property_len, const char **class_name, const char **prop_name, int *prop_len);

#define ZEND_FUNCTION_DTOR zend_function_dtor
#define ZEND_CLASS_DTOR destroy_zend_class

zend_op *get_next_op(zend_op_array *op_array TSRMLS_DC);
void init_op(zend_op *op TSRMLS_DC);
int get_next_op_number(zend_op_array *op_array);
int print_class(zend_class_entry *class_entry TSRMLS_DC);
void print_op_array(zend_op_array *op_array, int optimizations);
ZEND_API int pass_two(zend_op_array *op_array TSRMLS_DC);
zend_brk_cont_element *get_next_brk_cont_element(zend_op_array *op_array);
void zend_do_first_catch(znode *open_parentheses TSRMLS_DC);
void zend_initialize_try_catch_element(znode *catch_token TSRMLS_DC);
void zend_do_mark_last_catch(const znode *first_catch, const znode *last_additional_catch TSRMLS_DC);
ZEND_API zend_bool zend_is_compiling(TSRMLS_D);
ZEND_API char *zend_make_compiled_string_description(const char *name TSRMLS_DC);
ZEND_API void zend_initialize_class_data(zend_class_entry *ce, zend_bool nullify_handlers TSRMLS_DC);
int zend_get_class_fetch_type(const char *class_name, uint class_name_len);

typedef zend_bool (*zend_auto_global_callback)(zend_string *name TSRMLS_DC);
typedef struct _zend_auto_global {
	zend_string *name;
	zend_auto_global_callback auto_global_callback;
	zend_bool jit;
	zend_bool armed;
} zend_auto_global;

ZEND_API int zend_register_auto_global(zend_string *name, zend_bool jit, zend_auto_global_callback auto_global_callback TSRMLS_DC);
ZEND_API void zend_activate_auto_globals(TSRMLS_D);
ZEND_API zend_bool zend_is_auto_global(zend_string *name TSRMLS_DC);
ZEND_API size_t zend_dirname(char *path, size_t len);

int zendlex(znode *zendlval TSRMLS_DC);

int zend_add_literal(zend_op_array *op_array, zval *zv TSRMLS_DC);

/* BEGIN: OPCODES */

#include "zend_vm_opcodes.h"

#define ZEND_OP_DATA				137

/* END: OPCODES */

/* class fetches */
#define ZEND_FETCH_CLASS_DEFAULT	0
#define ZEND_FETCH_CLASS_SELF		1
#define ZEND_FETCH_CLASS_PARENT		2
#define ZEND_FETCH_CLASS_MAIN		3	/* unused */
#define ZEND_FETCH_CLASS_GLOBAL		4	/* unused */
#define ZEND_FETCH_CLASS_AUTO		5
#define ZEND_FETCH_CLASS_INTERFACE	6
#define ZEND_FETCH_CLASS_STATIC		7
#define ZEND_FETCH_CLASS_TRAIT		14
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
#define ZEND_FETCH_ADD_LOCK		    0x08000000
#define ZEND_FETCH_MAKE_REF		    0x04000000

#define ZEND_ISSET				    0x02000000
#define ZEND_ISEMPTY			    0x01000000
#define ZEND_ISSET_ISEMPTY_MASK	    (ZEND_ISSET | ZEND_ISEMPTY)
#define ZEND_QUICK_SET			    0x00800000

#define ZEND_FETCH_ARG_MASK         0x000fffff

#define ZEND_FE_FETCH_BYREF	1
#define ZEND_FE_FETCH_WITH_KEY	2

#define ZEND_FE_RESET_VARIABLE 		(1<<0)
#define ZEND_FE_RESET_REFERENCE		(1<<1)
#define EXT_TYPE_FREE_ON_RETURN		(1<<2)

#define ZEND_MEMBER_FUNC_CALL	1<<0

#define ZEND_ARG_SEND_BY_REF (1<<0)
#define ZEND_ARG_COMPILE_TIME_BOUND (1<<1)
#define ZEND_ARG_SEND_FUNCTION (1<<2)
#define ZEND_ARG_SEND_SILENT   (1<<3)

#define ZEND_SEND_BY_VAL     0
#define ZEND_SEND_BY_REF     1
#define ZEND_SEND_PREFER_REF 2

#define CHECK_ARG_SEND_TYPE(zf, arg_num, m) \
	(EXPECTED((zf)->common.arg_info != NULL) && \
	(EXPECTED(arg_num <= (zf)->common.num_args) \
		? ((zf)->common.arg_info[arg_num-1].pass_by_reference & (m)) \
		: (UNEXPECTED((zf)->common.fn_flags & ZEND_ACC_VARIADIC) != 0) && \
		   ((zf)->common.arg_info[(zf)->common.num_args-1].pass_by_reference & (m))))

#define ARG_MUST_BE_SENT_BY_REF(zf, arg_num) \
	CHECK_ARG_SEND_TYPE(zf, arg_num, ZEND_SEND_BY_REF)

#define ARG_SHOULD_BE_SENT_BY_REF(zf, arg_num) \
	CHECK_ARG_SEND_TYPE(zf, arg_num, ZEND_SEND_BY_REF|ZEND_SEND_PREFER_REF)

#define ARG_MAY_BE_SENT_BY_REF(zf, arg_num) \
	CHECK_ARG_SEND_TYPE(zf, arg_num, ZEND_SEND_PREFER_REF)

#define ZEND_RETURN_VAL 0
#define ZEND_RETURN_REF 1


#define ZEND_RETURNS_FUNCTION 1<<0
#define ZEND_RETURNS_NEW      1<<1
#define ZEND_RETURNS_VALUE    1<<2

#define ZEND_FAST_RET_TO_CATCH		1
#define ZEND_FAST_RET_TO_FINALLY	2

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

/* generate ZEND_DO_FCALL_BY_NAME for internal functions instead of ZEND_DO_FCALL */
#define ZEND_COMPILE_IGNORE_INTERNAL_FUNCTIONS	(1<<2)

/* don't perform early binding for classes inherited form internal ones;
 * in namespaces assume that internal class that doesn't exist at compile-time
 * may apper in run-time */
#define ZEND_COMPILE_IGNORE_INTERNAL_CLASSES	(1<<3)

/* generate ZEND_DECLARE_INHERITED_CLASS_DELAYED opcode to delay early binding */
#define ZEND_COMPILE_DELAYED_BINDING			(1<<4)

/* disable constant substitution at compile-time */
#define ZEND_COMPILE_NO_CONSTANT_SUBSTITUTION	(1<<5)

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
