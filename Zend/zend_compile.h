/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2002 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef ZEND_COMPILE_H
#define ZEND_COMPILE_H

#include "zend.h"

#ifdef HAVE_STDARG_H
# include <stdarg.h>
#endif

#include "zend_llist.h"

#define DEBUG_ZEND 0

#define FREE_PNODE(znode)	zval_dtor(&znode->u.constant);
#define FREE_OP(Ts, op, should_free) if (should_free) zval_dtor(&Ts[(op)->u.var].tmp_var);

#define SET_UNUSED(op)  (op).op_type = IS_UNUSED

#define INC_BPC(op_array)	if (CG(interactive)) { ((op_array)->backpatch_count++); }
#define DEC_BPC(op_array)	if (CG(interactive)) { ((op_array)->backpatch_count--); }
#define HANDLE_INTERACTIVE()  if (CG(interactive)) { execute_new_code(TSRMLS_C); }

typedef struct _zend_op_array zend_op_array;

typedef struct _znode {
	int op_type;
	zend_llist *throw_list; /* Try and save this space later on */
	union {
		zval constant;

		zend_uint var;
		zend_uint opline_num; /*  Needs to be signed */
		zend_op_array *op_array;
		zend_class_entry *previously_active_class_entry; /* Used at compile-time */
		struct {
			zend_uint var;	/* dummy */
			zend_uint type;
		} EA;
	} u;
} znode;


typedef struct _zend_op {
	zend_uchar opcode;
	znode result;
	znode op1;
	znode op2;
	ulong extended_value;
	uint lineno;
} zend_op;


typedef struct _zend_brk_cont_element {
	int cont;
	int brk;
	int parent;
} zend_brk_cont_element;


struct _zend_op_array {
	zend_uchar type;			/* MUST be the first element of this struct! */

	zend_uchar *arg_types;		/* MUST be the second element of this struct! */
	char *function_name;		/* MUST be the third element of this struct! */
	zend_class_entry *scope;	/* MUST be the fourth element of this struct! */

	zend_uint *refcount;

	zend_op *opcodes;
	zend_uint last, size;

	zend_uint T;

	zend_brk_cont_element *brk_cont_array;
	zend_uint last_brk_cont;
	zend_uint current_brk_cont;

	/* static variables support */
	HashTable *static_variables;

	zend_op *start_op;
	int backpatch_count;

	zend_bool return_reference;
	zend_bool done_pass_two;
	zend_bool uses_this;

	char *filename;

	void *reserved[ZEND_MAX_RESERVED_RESOURCES];
};


typedef struct _zend_internal_function {
	zend_uchar type;			/* MUST be the first element of this struct! */

	zend_uchar *arg_types;		/* MUST be the second element of this struct! */
	char *function_name;		/* MUST be the third element of this struct! */
	zend_class_entry *scope;	/* MUST be the fourth element of this struct! */

	void (*handler)(INTERNAL_FUNCTION_PARAMETERS);
} zend_internal_function;


typedef struct _zend_overloaded_function {
	zend_uchar type;			/* MUST be the first element of this struct! */

	zend_uchar *arg_types;		/* MUST be the second element of this struct! */
	char *function_name;		/* MUST be the third element of this struct! */
	zend_class_entry *scope;	/* MUST be the fourth element of this struct! */

	zend_uint var;
} zend_overloaded_function;


typedef union _zend_function {
	zend_uchar type;	/* MUST be the first element of this struct! */

	struct {
		zend_uchar type;  /* never used */
		zend_uchar *arg_types;
		char *function_name;
		zend_class_entry *scope;
	} common;
	
	zend_op_array op_array;
	zend_internal_function internal_function;
	zend_overloaded_function overloaded_function;
} zend_function;


typedef struct _zend_function_state {
	HashTable *function_symbol_table;
	zend_function *function;
	void *reserved[ZEND_MAX_RESERVED_RESOURCES];
} zend_function_state;


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


typedef struct _zend_file_handle {
	zend_uchar type;
	char *filename;
	char *opened_path;
	union {
		int fd;
		FILE *fp;
	} handle;
	zend_bool free_filename;
} zend_file_handle;

union _temp_variable;

typedef struct _zend_execute_data {
	struct _zend_op *opline;
	zend_function_state function_state;
	zend_function *fbc; /* Function Being Called */
	zend_function *fbc_constructor;
	zend_op_array *op_array;
	zval *object;
	union _temp_variable *Ts;
	zend_bool original_in_execution;
	zend_class_entry *calling_scope;
	struct _zend_execute_data *prev_execute_data;
} zend_execute_data;

#define EX(element) execute_data.element


#define IS_CONST	(1<<0)
#define IS_TMP_VAR	(1<<1)
#define IS_VAR		(1<<2)
#define IS_UNUSED	(1<<3)	/* Unused variable */


#define EXT_TYPE_UNUSED		(1<<0)

#include "zend_globals.h"

BEGIN_EXTERN_C()

void init_compiler(TSRMLS_D);
void shutdown_compiler(TSRMLS_D);
void zend_init_compiler_data_structures(TSRMLS_D);

extern ZEND_API zend_op_array *(*zend_compile_file)(zend_file_handle *file_handle, int type TSRMLS_DC);

void zend_activate(TSRMLS_D);
void zend_deactivate(TSRMLS_D);
void zend_activate_modules(TSRMLS_D);
void zend_deactivate_modules(TSRMLS_D);


ZEND_API int lex_scan(zval *zendlval TSRMLS_DC);
void startup_scanner(TSRMLS_D);
void shutdown_scanner(TSRMLS_D);

ZEND_API char *zend_set_compiled_filename(char *new_compiled_filename TSRMLS_DC);
ZEND_API void zend_restore_compiled_filename(char *original_compiled_filename TSRMLS_DC);
ZEND_API char *zend_get_compiled_filename(TSRMLS_D);
ZEND_API int zend_get_compiled_lineno(TSRMLS_D);

#ifdef ZTS
const char *zend_get_zendtext(TSRMLS_D);
int zend_get_zendleng(TSRMLS_D);
#endif


/* parser-driven code generators */
void zend_do_binary_op(zend_uchar op, znode *result, znode *op1, znode *op2 TSRMLS_DC);
void zend_do_unary_op(zend_uchar op, znode *result, znode *op1 TSRMLS_DC);
void zend_do_binary_assign_op(zend_uchar op, znode *result, znode *op1, znode *op2 TSRMLS_DC);
void zend_do_assign(znode *result, znode *variable, znode *value TSRMLS_DC);
void zend_do_assign_ref(znode *result, znode *lvar, znode *rvar TSRMLS_DC);
void fetch_simple_variable(znode *result, znode *varname, int bp TSRMLS_DC);
void fetch_simple_variable_ex(znode *result, znode *varname, int bp, zend_uchar op TSRMLS_DC);
void zend_do_indirect_references(znode *result, znode *num_references, znode *variable TSRMLS_DC);
void zend_do_fetch_global_or_static_variable(znode *varname, znode *static_assignment, int fetch_type TSRMLS_DC);

void fetch_array_begin(znode *result, znode *varname, znode *first_dim TSRMLS_DC);
void fetch_array_dim(znode *result, znode *parent, znode *dim TSRMLS_DC);
void fetch_string_offset(znode *result, znode *parent, znode *offset TSRMLS_DC);
void zend_do_fetch_static_member(znode *class TSRMLS_DC);
void zend_do_print(znode *result, znode *arg TSRMLS_DC);
void zend_do_echo(znode *arg TSRMLS_DC);
typedef int (*unary_op_type)(zval *, zval *);
ZEND_API unary_op_type get_unary_op(int opcode);
ZEND_API void *get_binary_op(int opcode);

void zend_do_while_cond(znode *expr, znode *close_bracket_token TSRMLS_DC);
void zend_do_while_end(znode *while_token, znode *close_bracket_token TSRMLS_DC);
void zend_do_do_while_begin(TSRMLS_D);
void zend_do_do_while_end(znode *do_token, znode *expr_open_bracket, znode *expr TSRMLS_DC);


void zend_do_if_cond(znode *cond, znode *closing_bracket_token TSRMLS_DC);
void zend_do_if_after_statement(znode *closing_bracket_token, unsigned char initialize TSRMLS_DC);
void zend_do_if_end(TSRMLS_D);

void zend_do_for_cond(znode *expr, znode *second_semicolon_token TSRMLS_DC);
void zend_do_for_before_statement(znode *cond_start, znode *second_semicolon_token TSRMLS_DC);
void zend_do_for_end(znode *second_semicolon_token TSRMLS_DC);

void zend_do_pre_incdec(znode *result, znode *op1, zend_uchar op TSRMLS_DC);
void zend_do_post_incdec(znode *result, znode *op1, zend_uchar op TSRMLS_DC);

void zend_do_begin_variable_parse(TSRMLS_D);
void zend_do_end_variable_parse(int type, int arg_offset TSRMLS_DC);

void zend_check_writable_variable(znode *variable);

void zend_do_free(znode *op1 TSRMLS_DC);

void zend_do_init_string(znode *result TSRMLS_DC);
void zend_do_add_char(znode *result, znode *op1, znode *op2 TSRMLS_DC);
void zend_do_add_string(znode *result, znode *op1, znode *op2 TSRMLS_DC);
void zend_do_add_variable(znode *result, znode *op1, znode *op2 TSRMLS_DC);

void zend_do_begin_function_declaration(znode *function_token, znode *function_name, int is_method, int return_reference TSRMLS_DC);
void zend_do_end_function_declaration(znode *function_token TSRMLS_DC);
void zend_do_receive_arg(zend_uchar op, znode *var, znode *offset, znode *initialization, zend_uchar pass_type TSRMLS_DC);
int zend_do_begin_function_call(znode *function_name TSRMLS_DC);
void zend_do_begin_method_call(znode *left_bracket TSRMLS_DC);
void zend_do_begin_dynamic_function_call(znode *function_name TSRMLS_DC);
void do_fetch_class(znode *result, znode *class_entry, znode *class_name TSRMLS_DC);
void do_fetch_class_name(znode *result, znode *class_entry, znode *class_name, zend_bool case_sensitive TSRMLS_DC);
void zend_do_begin_class_member_function_call(znode *class_name, znode *function_name TSRMLS_DC);
void zend_do_end_function_call(znode *function_name, znode *result, znode *argument_list, int is_method, int is_dynamic_fcall TSRMLS_DC);
void zend_do_return(znode *expr, int do_end_vparse TSRMLS_DC);

void zend_do_try(znode *try_token TSRMLS_DC);
void zend_do_begin_catch(znode *try_token, znode *catch_class, znode *catch_var, zend_bool first_catch TSRMLS_DC);
void zend_do_end_catch(znode *try_token TSRMLS_DC);
void zend_do_throw(znode *expr TSRMLS_DC);

ZEND_API int do_bind_function(zend_op *opline, HashTable *function_table, HashTable *class_table, int compile_time);
ZEND_API int do_bind_class(zend_op *opline, HashTable *function_table, HashTable *class_table);
ZEND_API int do_bind_inherited_class(zend_op *opline, HashTable *function_table, HashTable *class_table, zend_class_entry *parent_ce);

void zend_do_inheritance(zend_class_entry *ce, zend_class_entry *parent_ce);
void zend_do_early_binding(TSRMLS_D);

void zend_do_pass_param(znode *param, zend_uchar op, int offset TSRMLS_DC);


void zend_do_boolean_or_begin(znode *expr1, znode *op_token TSRMLS_DC);
void zend_do_boolean_or_end(znode *result, znode *expr1, znode *expr2, znode *op_token TSRMLS_DC);
void zend_do_boolean_and_begin(znode *expr1, znode *op_token TSRMLS_DC);               
void zend_do_boolean_and_end(znode *result, znode *expr1, znode *expr2, znode *op_token TSRMLS_DC);

void zend_do_brk_cont(zend_uchar op, znode *expr TSRMLS_DC);

void zend_do_switch_cond(znode *cond TSRMLS_DC);
void zend_do_switch_end(znode *case_list TSRMLS_DC);
void zend_do_case_before_statement(znode *case_list, znode *case_token, znode *case_expr TSRMLS_DC);
void zend_do_case_after_statement(znode *result, znode *case_token TSRMLS_DC);
void zend_do_default_before_statement(znode *case_list, znode *default_token TSRMLS_DC);

void zend_do_begin_class_declaration(znode *class_token, znode *class_name, znode *parent_class_name TSRMLS_DC);
void zend_do_end_class_declaration(znode *class_token TSRMLS_DC);
void zend_do_declare_property(znode *var_name, znode *value, int declaration_type TSRMLS_DC);

void zend_do_fetch_property(znode *result, znode *object, znode *property TSRMLS_DC);


void zend_do_push_object(znode *object TSRMLS_DC);
void zend_do_pop_object(znode *object TSRMLS_DC);


void zend_do_begin_new_object(znode *new_token, znode *class_type TSRMLS_DC);
void zend_do_end_new_object(znode *result, znode *new_token, znode *argument_list TSRMLS_DC);

void zend_do_fetch_constant(znode *result, znode *constant_container, znode *constant_name, int mode TSRMLS_DC);

void zend_do_shell_exec(znode *result, znode *cmd TSRMLS_DC);

void zend_do_init_array(znode *result, znode *expr, znode *offset, zend_bool is_ref TSRMLS_DC);
void zend_do_add_array_element(znode *result, znode *expr, znode *offset, zend_bool is_ref TSRMLS_DC);
void zend_do_add_static_array_element(znode *result, znode *offset, znode *expr);
void zend_do_list_init(TSRMLS_D);
void zend_do_list_end(znode *result, znode *expr TSRMLS_DC);
void zend_do_add_list_element(znode *element TSRMLS_DC);
void zend_do_new_list_begin(TSRMLS_D);
void zend_do_new_list_end(TSRMLS_D);

void zend_do_cast(znode *result, znode *expr, int type TSRMLS_DC);
void zend_do_include_or_eval(int type, znode *result, znode *op1 TSRMLS_DC);

void zend_do_unset(znode *variable TSRMLS_DC);
void zend_do_isset_or_isempty(int type, znode *result, znode *variable TSRMLS_DC);

void zend_do_is_class(znode *result, znode *expr, znode *class, int type TSRMLS_DC);

void zend_do_foreach_begin(znode *foreach_token, znode *array, znode *open_brackets_token, znode *as_token, int variable TSRMLS_DC);
void zend_do_foreach_cont(znode *value, znode *key, znode *as_token TSRMLS_DC);
void zend_do_foreach_end(znode *foreach_token, znode *open_brackets_token TSRMLS_DC);

void zend_do_declare_begin(TSRMLS_D);
void zend_do_declare_stmt(znode *var, znode *val TSRMLS_DC);
void zend_do_declare_end(znode *declare_token TSRMLS_DC);

void zend_do_end_heredoc(TSRMLS_D);

void zend_do_exit(znode *result, znode *message TSRMLS_DC);

void zend_do_begin_silence(znode *strudel_token TSRMLS_DC);
void zend_do_end_silence(znode *strudel_token TSRMLS_DC);

void zend_do_begin_qm_op(znode *cond, znode *qm_token TSRMLS_DC);
void zend_do_qm_true(znode *true_value, znode *qm_token, znode *colon_token TSRMLS_DC);
void zend_do_qm_false(znode *result, znode *false_value, znode *qm_token, znode *colon_token TSRMLS_DC);

void zend_do_extended_info(TSRMLS_D);
void zend_do_extended_fcall_begin(TSRMLS_D);
void zend_do_extended_fcall_end(TSRMLS_D);

void zend_do_ticks(TSRMLS_D);

void zend_do_begin_import(TSRMLS_D);
void zend_do_import(int type, znode *what TSRMLS_DC);
void zend_do_end_import(znode *import_from TSRMLS_DC);

ZEND_API void function_add_ref(zend_function *function);

#define INITIAL_OP_ARRAY_SIZE 64


/* helper functions in zend_language_scanner.l */
ZEND_API zend_op_array *compile_file(zend_file_handle *file_handle, int type TSRMLS_DC);
ZEND_API zend_op_array *compile_string(zval *source_string, char *filename TSRMLS_DC);	
ZEND_API zend_op_array *compile_filename(int type, zval *filename TSRMLS_DC);
ZEND_API int zend_execute_scripts(int type TSRMLS_DC, zval **retval, int file_count, ...);
ZEND_API int open_file_for_scanning(zend_file_handle *file_handle TSRMLS_DC);
ZEND_API void init_op_array(zend_op_array *op_array, zend_uchar type, int initial_ops_size TSRMLS_DC);
ZEND_API void destroy_op_array(zend_op_array *op_array);
ZEND_API void zend_destroy_file_handle(zend_file_handle *file_handle TSRMLS_DC);
ZEND_API void zend_file_handle_dtor(zend_file_handle *fh);

ZEND_API void destroy_zend_function(zend_function *function);
ZEND_API void destroy_zend_class(zend_class_entry **pce);
void zend_class_add_ref(zend_class_entry **ce);

#define ZEND_FUNCTION_DTOR (void (*)(void *)) destroy_zend_function
#define ZEND_CLASS_DTOR (void (*)(void *)) destroy_zend_class

zend_op *get_next_op(zend_op_array *op_array TSRMLS_DC);
void init_op(zend_op *op TSRMLS_DC);
int get_next_op_number(zend_op_array *op_array);
int print_class(zend_class_entry *class_entry TSRMLS_DC);
void print_op_array(zend_op_array *op_array, int optimizations);
int pass_two(zend_op_array *op_array TSRMLS_DC);
zend_brk_cont_element *get_next_brk_cont_element(zend_op_array *op_array);
ZEND_API zend_bool zend_is_compiling(TSRMLS_D);
ZEND_API char *zend_make_compiled_string_description(char *name TSRMLS_DC);

int zend_register_auto_global(char *name, uint name_len TSRMLS_DC);

int zendlex(znode *zendlval TSRMLS_DC);

#define ZEND_NOP					0
									
#define ZEND_ADD					1
#define ZEND_SUB					2
#define ZEND_MUL					3
#define ZEND_DIV					4
#define ZEND_MOD					5
#define ZEND_SL						6
#define ZEND_SR						7
#define ZEND_CONCAT					8
#define ZEND_BW_OR					9
#define ZEND_BW_AND					10
#define ZEND_BW_XOR					11
#define ZEND_BW_NOT					12
#define ZEND_BOOL_NOT				13
#define ZEND_BOOL_XOR				14
#define ZEND_IS_IDENTICAL			15
#define ZEND_IS_NOT_IDENTICAL       16
#define ZEND_IS_EQUAL				17
#define ZEND_IS_NOT_EQUAL			18
#define ZEND_IS_SMALLER				19
#define ZEND_IS_SMALLER_OR_EQUAL	20
#define ZEND_CAST					21
#define ZEND_QM_ASSIGN				22

#define ZEND_ASSIGN_ADD				23
#define ZEND_ASSIGN_SUB				24
#define ZEND_ASSIGN_MUL				25
#define ZEND_ASSIGN_DIV				26
#define ZEND_ASSIGN_MOD				27
#define ZEND_ASSIGN_SL				28
#define ZEND_ASSIGN_SR				29
#define ZEND_ASSIGN_CONCAT			30
#define ZEND_ASSIGN_BW_OR			31
#define ZEND_ASSIGN_BW_AND			32
#define ZEND_ASSIGN_BW_XOR			33
									
#define ZEND_PRE_INC				34
#define ZEND_PRE_DEC				35
#define ZEND_POST_INC				36
#define ZEND_POST_DEC				37
								 	
#define ZEND_ASSIGN					38
#define ZEND_ASSIGN_REF				39

#define ZEND_ECHO					40
#define ZEND_PRINT					41

#define ZEND_JMP					42
#define ZEND_JMPZ					43
#define ZEND_JMPNZ					44
#define ZEND_JMPZNZ					45
#define ZEND_JMPZ_EX				46
#define ZEND_JMPNZ_EX				47
#define ZEND_CASE					48
#define ZEND_SWITCH_FREE			49
#define ZEND_BRK					50
#define ZEND_CONT					51
#define ZEND_BOOL					52

#define ZEND_INIT_STRING			53
#define ZEND_ADD_CHAR				54
#define ZEND_ADD_STRING				55
#define ZEND_ADD_VAR				56

#define ZEND_BEGIN_SILENCE			57
#define ZEND_END_SILENCE			58

#define ZEND_INIT_FCALL_BY_NAME		59
#define ZEND_DO_FCALL				60
#define ZEND_DO_FCALL_BY_NAME		61
#define ZEND_RETURN					62

#define ZEND_RECV					63
#define ZEND_RECV_INIT				64
									
#define ZEND_SEND_VAL				65
#define ZEND_SEND_VAR				66
#define ZEND_SEND_REF				67

#define ZEND_NEW 					68
#define ZEND_JMP_NO_CTOR			69
#define ZEND_FREE					70
									
#define ZEND_INIT_ARRAY				71
#define ZEND_ADD_ARRAY_ELEMENT		72
									
#define ZEND_INCLUDE_OR_EVAL		73
									
#define ZEND_UNSET_VAR				74
#define ZEND_UNSET_DIM_OBJ			75
									
#define ZEND_FE_RESET				77
#define ZEND_FE_FETCH				78
									
#define ZEND_EXIT					79


/* the following 18 opcodes are 6 groups of 3 opcodes each, and must
 * remain in that order!
 */
#define ZEND_FETCH_R				80
#define ZEND_FETCH_DIM_R			81
#define ZEND_FETCH_OBJ_R			82
#define ZEND_FETCH_W				83
#define ZEND_FETCH_DIM_W			84
#define ZEND_FETCH_OBJ_W			85
#define ZEND_FETCH_RW				86
#define ZEND_FETCH_DIM_RW			87
#define ZEND_FETCH_OBJ_RW			88
#define ZEND_FETCH_IS				89
#define ZEND_FETCH_DIM_IS			90
#define ZEND_FETCH_OBJ_IS			91
#define ZEND_FETCH_FUNC_ARG			92
#define ZEND_FETCH_DIM_FUNC_ARG		93
#define ZEND_FETCH_OBJ_FUNC_ARG		94
#define ZEND_FETCH_UNSET			95
#define ZEND_FETCH_DIM_UNSET		96
#define ZEND_FETCH_OBJ_UNSET		97

#define ZEND_FETCH_DIM_TMP_VAR		98
#define ZEND_FETCH_CONSTANT			99

/* Hole - 100 */

#define ZEND_EXT_STMT				101
#define ZEND_EXT_FCALL_BEGIN		102
#define ZEND_EXT_FCALL_END			103
#define ZEND_EXT_NOP				104

#define ZEND_TICKS					105

#define ZEND_SEND_VAR_NO_REF		106

#define ZEND_CATCH					107
#define ZEND_THROW					108

#define ZEND_FETCH_CLASS			109

#define ZEND_CLONE					110

#define ZEND_INIT_CTOR_CALL			111
#define ZEND_INIT_METHOD_CALL		112
#define ZEND_INIT_STATIC_METHOD_CALL 113

#define ZEND_ISSET_ISEMPTY_VAR		114
#define ZEND_ISSET_ISEMPTY_DIM_OBJ	115

#define	ZEND_IMPORT_FUNCTION		116
#define	ZEND_IMPORT_CLASS			117
#define	ZEND_IMPORT_CONST			118

#define ZEND_ASSIGN_ADD_OBJ			121
#define ZEND_ASSIGN_SUB_OBJ			122
#define ZEND_ASSIGN_MUL_OBJ			123
#define ZEND_ASSIGN_DIV_OBJ			124
#define ZEND_ASSIGN_MOD_OBJ			125
#define ZEND_ASSIGN_SL_OBJ			126
#define ZEND_ASSIGN_SR_OBJ			127
#define ZEND_ASSIGN_CONCAT_OBJ		128
#define ZEND_ASSIGN_BW_OR_OBJ		129
#define ZEND_ASSIGN_BW_AND_OBJ		130
#define ZEND_ASSIGN_BW_XOR_OBJ		131
									
#define ZEND_PRE_INC_OBJ			132
#define ZEND_PRE_DEC_OBJ			133
#define ZEND_POST_INC_OBJ			134
#define ZEND_POST_DEC_OBJ			135
								 	
#define ZEND_ASSIGN_OBJ				136
#define ZEND_MAKE_VAR				137

#define ZEND_IS_CLASS				138

#define ZEND_DECLARE_CLASS				139
#define ZEND_DECLARE_INHERITED_CLASS	140
#define ZEND_DECLARE_FUNCTION			141

/* end of block */




/* global/local fetches */
#define ZEND_FETCH_GLOBAL			0
#define ZEND_FETCH_LOCAL			1
#define ZEND_FETCH_STATIC			2
#define ZEND_FETCH_STATIC_MEMBER	3
#define ZEND_FETCH_FROM_THIS		4

/* class fetches */
#define ZEND_FETCH_CLASS_DEFAULT	0
#define ZEND_FETCH_CLASS_SELF		1
#define ZEND_FETCH_CLASS_PARENT		2
#define ZEND_FETCH_CLASS_MAIN		3


/* variable parsing type (compile-time) */
#define ZEND_PARSED_MEMBER			(1<<0)
#define ZEND_PARSED_METHOD_CALL		(1<<1)
#define ZEND_PARSED_STATIC_MEMBER	(1<<2)
#define ZEND_PARSED_FUNCTION_CALL	(1<<3)
#define ZEND_PARSED_VARIABLE		(1<<4)


/* unset types */
#define ZEND_UNSET_REG 0

/* var status for backpatching */
#define BP_VAR_R			0
#define BP_VAR_W			1
#define BP_VAR_RW			2
#define BP_VAR_IS			3
#define BP_VAR_NA			4	/* if not applicable */
#define BP_VAR_FUNC_ARG		5
#define BP_VAR_UNSET		6


#define ZEND_INTERNAL_FUNCTION		1
#define ZEND_USER_FUNCTION			2
#define ZEND_OVERLOADED_FUNCTION	3
#define	ZEND_EVAL_CODE				4

#define ZEND_INTERNAL_CLASS		1
#define ZEND_USER_CLASS			2

#define ZEND_EVAL				(1<<0)
#define ZEND_INCLUDE			(1<<1)
#define ZEND_INCLUDE_ONCE		(1<<2)
#define ZEND_REQUIRE			(1<<3)
#define ZEND_REQUIRE_ONCE		(1<<4)

#define ZEND_ISSET				(1<<0)
#define ZEND_ISEMPTY			(1<<1)

#define ZEND_CT	(1<<0)
#define ZEND_RT (1<<1)


#define ZEND_HANDLE_FILENAME		0
#define ZEND_HANDLE_FD				1
#define ZEND_HANDLE_FP				2
#define ZEND_HANDLE_STDIOSTREAM		3
#define ZEND_HANDLE_FSTREAM			4

#define ZEND_FETCH_STANDARD		0
#define ZEND_FETCH_ADD_LOCK		1

#define ZEND_MEMBER_FUNC_CALL	1<<0

#define ZEND_ARG_SEND_BY_REF (1<<0)
#define ZEND_ARG_COMPILE_TIME_BOUND (1<<1)

#define AI_USE_PTR(ai) \
	if ((ai).ptr_ptr) { \
		(ai).ptr = *((ai).ptr_ptr); \
		(ai).ptr_ptr = &((ai).ptr); \
	} else { \
		(ai).ptr = NULL; \
	}

/* Lost In Stupid Parentheses */
#define ARG_SHOULD_BE_SENT_BY_REF(offset, conduct_check, arg_types)	\
	(																\
		conduct_check												\
		&& arg_types												\
		&&															\
		(															\
			(														\
				offset<=arg_types[0]								\
				&& arg_types[offset]==BYREF_FORCE					\
			)														\
		||	(														\
				offset>=arg_types[0]								\
				&& arg_types[arg_types[0]]==BYREF_FORCE_REST		\
			)														\
		)															\
	)

#define ZEND_RETURN_VAL 0
#define ZEND_RETURN_REF 1

END_EXTERN_C()

#define ZEND_CLONE_FUNC_NAME		"__clone"
#define ZEND_CONSTRUCTOR_FUNC_NAME	"__construct"
#define ZEND_DESTRUCTOR_FUNC_NAME	"__destruct"
#define ZEND_GET_FUNC_NAME          "__get"
#define ZEND_SET_FUNC_NAME          "__set"
#define ZEND_CALL_FUNC_NAME         "__call"

#endif /* ZEND_COMPILE_H */
