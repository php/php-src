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


#ifndef _COMPILE_H
#define _COMPILE_H

#include "zend.h"

#ifdef HAVE_STDARG_H
# include <stdarg.h>
#endif

#include "zend_llist.h"
#define YYSTYPE znode
#include "zend-parser.h"

#define DEBUG_ZEND 0

#ifndef ZTS
# define SUPPORT_INTERACTIVE 1
#else
# define SUPPORT_INTERACTIVE 0
#endif

#define FREE_PNODE(znode)	zval_dtor(&znode->u.constant);
#define FREE_OP(op, should_free) if (should_free) zval_dtor(&Ts[(op)->u.var].tmp_var);


#if SUPPORT_INTERACTIVE
#define INC_BPC(op_array)	((op_array)->backpatch_count++)
#define DEC_BPC(op_array)	((op_array)->backpatch_count--)
#define HANDLE_INTERACTIVE()  if (EG(interactive)) { execute_new_code(CLS_C); }
#else
#define INC_BPC(op_array)
#define DEC_BPC(op_array)
#define HANDLE_INTERACTIVE()
#endif

typedef struct _zend_op_array zend_op_array;

typedef struct _znode {
	int op_type;
	union {
		zval constant;

		int var;
		int opline_num;
		int fetch_type;
		zend_op_array *op_array;
		struct {
			int var;	/* dummy */
			int type;
		} EA;
	} u;
} znode;


typedef struct _zend_op {
	int opcode;
	znode result;
	znode op1;
	znode op2;
	ulong extended_value;
	char *filename;
	uint lineno;
} zend_op;


typedef struct _zend_brk_cont_element {
	int cont;
	int brk;
	int parent;
} zend_brk_cont_element;


struct _zend_op_array {
	int type;	/* MUST be the first element of this struct! */

	unsigned char *arg_types;		/* MUST be the second element of this struct! */
	char *function_name;			/* MUST be the third element of this struct! */

	int *refcount;

	zend_op *opcodes;
	int last, size;

	int T;


	zend_brk_cont_element *brk_cont_array;
	int last_brk_cont;
	int current_brk_cont;
	unsigned char uses_globals;

	/* static variables support */
	HashTable *static_variables;

#if SUPPORT_INTERACTIVE
	int start_op_number, end_op_number;
	int last_executed_op_number;
	int backpatch_count;
#endif

	void *reserved[ZEND_MAX_RESERVED_RESOURCES];
};


typedef struct _zend_internal_function {
	int type;	/* MUST be the first element of this struct! */

	unsigned char *arg_types;		/* MUST be the second element of this struct */
	char *function_name;			/* MUST be the third element of this struct */

	void (*handler)(INTERNAL_FUNCTION_PARAMETERS);
} zend_internal_function;


typedef union _zend_function {
	int type;	/* MUST be the first element of this struct! */
	struct {
		int type;  /* never used */
		unsigned char *arg_types;
		char *function_name;
	} common;
	
	zend_op_array op_array;
	zend_internal_function internal_function;
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
	int type;
	char *filename;
	char *opened_path;
	union {
		int fd;
		FILE *fp;
#ifdef __cplusplus
		istream *is;
#endif
	} handle;
} zend_file_handle;


#include "zend_globals.h"

#define IS_CONST	(1<<0)
#define IS_TMP_VAR	(1<<1)
#define IS_VAR		(1<<2)
#define IS_UNUSED	(1<<3)	/* Unused variable */


#define EXT_TYPE_UNUSED		(1<<0)

void init_compiler(CLS_D ELS_DC);
void shutdown_compiler(CLS_D);

BEGIN_EXTERN_C()
extern ZEND_API zend_op_array *(*zend_compile_files)(int mark_as_ref CLS_DC, int file_count, ...);

void zend_activate(CLS_D ELS_DC);
void zend_deactivate(CLS_D ELS_DC);

int lex_scan(zval *zendlval CLS_DC);
void startup_scanner(CLS_D);
void shutdown_scanner(CLS_D);

ZEND_API char *zend_set_compiled_filename(char *new_compiled_filename);
ZEND_API void zend_restore_compiled_filename(char *original_compiled_filename);
ZEND_API char *zend_get_compiled_filename(void);

#ifdef ZTS
const char *zend_get_zendtext(CLS_D);
int zend_get_zendleng(CLS_D);
#endif

END_EXTERN_C()

/* parser-driven code generators */
void do_binary_op(int op, znode *result, znode *op1, znode *op2 CLS_DC);
void do_unary_op(int op, znode *result, znode *op1 CLS_DC);
void do_binary_assign_op(int op, znode *result, znode *op1, znode *op2 CLS_DC);
void do_assign(znode *result, znode *variable, znode *value CLS_DC);
void do_assign_ref(znode *result, znode *lvar, znode *rvar CLS_DC);
void fetch_simple_variable(znode *result, znode *varname, int bp CLS_DC);
void fetch_simple_variable_ex(znode *result, znode *varname, int bp, int op CLS_DC);
void do_indirect_references(znode *result, znode *num_references, znode *variable CLS_DC);
void do_fetch_global_or_static_variable(znode *varname, znode *static_assignment, int fetch_type CLS_DC);
void do_fetch_globals(znode *varname CLS_DC);

void fetch_array_begin(znode *result, znode *varname, znode *first_dim CLS_DC);
void fetch_array_dim(znode *result, znode *parent, znode *dim CLS_DC);
void do_print(znode *result, znode *arg CLS_DC);
void do_echo(znode *arg CLS_DC);
typedef int (*unary_op_type)(zval *, zval *);
ZEND_API unary_op_type get_unary_op(int opcode);
ZEND_API void *get_binary_op(int opcode);

void do_while_cond(znode *expr, znode *close_bracket_token CLS_DC);
void do_while_end(znode *while_token, znode *close_bracket_token CLS_DC);
void do_do_while_begin(CLS_D);
void do_do_while_end(znode *do_token, znode *expr CLS_DC);


void do_if_cond(znode *cond, znode *closing_bracket_token CLS_DC);
void do_if_after_statement(znode *closing_bracket_token, unsigned char initialize CLS_DC);
void do_if_end(CLS_D);

void do_for_cond(znode *expr, znode *second_semicolon_token CLS_DC);
void do_for_before_statement(znode *cond_start, znode *second_semicolon_token CLS_DC);
void do_for_end(znode *second_semicolon_token CLS_DC);

void do_pre_incdec(znode *result, znode *op1, int op CLS_DC);
void do_post_incdec(znode *result, znode *op1, int op CLS_DC);

void do_begin_variable_parse(CLS_D);
void do_end_variable_parse(int type, int arg_offset CLS_DC);

void do_free(znode *op1 CLS_DC);

void do_init_string(znode *result CLS_DC);
void do_add_char(znode *result, znode *op1, znode *op2 CLS_DC);
void do_add_string(znode *result, znode *op1, znode *op2 CLS_DC);
void do_add_variable(znode *result, znode *op1, znode *op2 CLS_DC);

void do_begin_function_declaration(znode *function_token, znode *function_name, int is_method CLS_DC);
void do_end_function_declaration(znode *function_token CLS_DC);
void do_receive_arg(int op, znode *var, znode *offset, znode *initialization, unsigned char pass_type CLS_DC);
int do_begin_function_call(znode *function_name CLS_DC);
void do_begin_dynamic_function_call(znode *function_name CLS_DC);
void do_begin_class_member_function_call(znode *class_name, znode *function_name CLS_DC);
void do_end_function_call(znode *function_name, znode *result, znode *argument_list, int is_method, int is_dynamic_fcall CLS_DC);
void do_return(znode *expr CLS_DC);
ZEND_API int do_bind_function_or_class(zend_op *opline, HashTable *function_table, HashTable *class_table, int compile_time);
void do_early_binding(CLS_D);

void do_pass_param(znode *param, int op, int offset CLS_DC);


void do_boolean_or_begin(znode *expr1, znode *op_token CLS_DC);
void do_boolean_or_end(znode *result, znode *expr1, znode *expr2, znode *op_token CLS_DC);
void do_boolean_and_begin(znode *expr1, znode *op_token CLS_DC);               
void do_boolean_and_end(znode *result, znode *expr1, znode *expr2, znode *op_token CLS_DC);

void do_brk_cont(int op, znode *expr CLS_DC);

void do_switch_cond(znode *cond CLS_DC);
void do_switch_end(znode *case_list CLS_DC);
void do_case_before_statement(znode *case_list, znode *case_token, znode *case_expr CLS_DC);
void do_case_after_statement(znode *result, znode *case_token CLS_DC);
void do_default_before_statement(znode *case_list, znode *default_token CLS_DC);

void do_begin_class_declaration(znode *class_name, znode *parent_class_name CLS_DC);
void do_end_class_declaration(CLS_D);
void do_declare_property(znode *var_name, znode *value CLS_DC);

void do_fetch_property(znode *result, znode *object, znode *property CLS_DC);


void do_push_object(znode *object CLS_DC);
void do_pop_object(znode *object CLS_DC);


void do_begin_new_object(znode *new_token, znode *class_name CLS_DC);
void do_end_new_object(znode *result, znode *class_name, znode *new_token, znode *argument_list CLS_DC);

void do_fetch_constant(znode *result, znode *constant_name, int mode CLS_DC);

void do_shell_exec(znode *result, znode *cmd CLS_DC);

void do_init_array(znode *result, znode *expr, znode *offset, int is_ref CLS_DC);
void do_add_array_element(znode *result, znode *expr, znode *offset, int is_ref CLS_DC);
void do_add_static_array_element(znode *result, znode *offset, znode *expr);
void do_list_init(void);
void do_list_end(znode *result, znode *expr CLS_DC);
void do_add_list_element(znode *element CLS_DC);
void do_new_list_begin(CLS_D);
void do_new_list_end(CLS_D);

void do_cast(znode *result, znode *expr, int type CLS_DC);
void do_include_or_eval(int type, znode *result, znode *op1 CLS_DC);

void do_unset(znode *variable CLS_DC);
void do_isset_or_isempty(int type, znode *result, znode *variable CLS_DC);

void do_foreach_begin(znode *foreach_token, znode *array, znode *open_brackets_token, znode *as_token CLS_DC);
void do_foreach_cont(znode *value, znode *key, znode *as_token CLS_DC);
void do_foreach_end(znode *foreach_token, znode *open_brackets_token CLS_DC);

void do_end_heredoc(CLS_D);

void do_exit(znode *result, znode *message CLS_DC);

void do_begin_silence(znode *strudel_token CLS_DC);
void do_end_silence(znode *strudel_token CLS_DC);

void do_begin_qm_op(znode *cond, znode *qm_token CLS_DC);
void do_qm_true(znode *true_value, znode *qm_token, znode *colon_token CLS_DC);
void do_qm_false(znode *result, znode *false_value, znode *qm_token, znode *colon_token CLS_DC);

void do_extended_info(CLS_D);
void do_extended_fcall_begin(CLS_D);
void do_extended_fcall_end(CLS_D);

#define INITIAL_OP_ARRAY_SIZE 64


/* helper functions in zend-scanner.l */
BEGIN_EXTERN_C()
ZEND_API int require_file(zend_file_handle *file_handle, zend_bool unique CLS_DC);	
ZEND_API int require_filename(char *filename, zend_bool unique CLS_DC);
ZEND_API zend_op_array *compile_files(int mark_as_ref CLS_DC, int file_count, ...);
ZEND_API zend_op_array *v_compile_files(int mark_as_ref CLS_DC, int file_count, va_list files);
ZEND_API zend_op_array *compile_string(zval *source_string CLS_DC);	
ZEND_API zend_op_array *compile_filename(zval *filename, zend_bool unique CLS_DC);
ZEND_API int open_file_for_scanning(zend_file_handle *file_handle CLS_DC);
ZEND_API void init_op_array(zend_op_array *op_array, int initial_ops_size);
ZEND_API void destroy_op_array(zend_op_array *op_array);
ZEND_API void zend_close_file_handle(zend_file_handle *file_handle CLS_DC);
ZEND_API void zend_open_file_dtor(zend_file_handle *fh);
END_EXTERN_C()

ZEND_API int destroy_zend_function(zend_function *function);
ZEND_API int destroy_zend_class(zend_class_entry *ce);
void zend_class_add_ref(zend_class_entry *ce);

#define ZEND_FUNCTION_DTOR (int (*)(void *)) destroy_zend_function
#define ZEND_CLASS_DTOR (int (*)(void *)) destroy_zend_class

zend_op *get_next_op(zend_op_array *op_array CLS_DC);
void init_op(zend_op *op CLS_DC);
int get_next_op_number(zend_op_array *op_array);
int print_class(zend_class_entry *class_entry);
void print_op_array(zend_op_array *op_array, int optimizations);
BEGIN_EXTERN_C()
int pass_two(zend_op_array *op_array);
void pass_include_eval(zend_op_array *op_array);
END_EXTERN_C()
zend_brk_cont_element *get_next_brk_cont_element(zend_op_array *op_array);


int zendlex(znode *zendlval CLS_DC);


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
#define ZEND_IS_EQUAL				16
#define ZEND_IS_NOT_EQUAL			17
#define ZEND_IS_SMALLER				18
#define ZEND_IS_SMALLER_OR_EQUAL	19
#define ZEND_CAST					20
#define ZEND_QM_ASSIGN				21

#define ZEND_ASSIGN_ADD				22
#define ZEND_ASSIGN_SUB				23
#define ZEND_ASSIGN_MUL				24
#define ZEND_ASSIGN_DIV				25
#define ZEND_ASSIGN_MOD				26
#define ZEND_ASSIGN_SL				27
#define ZEND_ASSIGN_SR				28
#define ZEND_ASSIGN_CONCAT			29
#define ZEND_ASSIGN_BW_OR			30
#define ZEND_ASSIGN_BW_AND			31
#define ZEND_ASSIGN_BW_XOR			32
									
#define ZEND_PRE_INC				33
#define ZEND_PRE_DEC				34
#define ZEND_POST_INC				35
#define ZEND_POST_DEC				36
									
#define ZEND_ASSIGN					37
#define ZEND_ASSIGN_REF				38

#define ZEND_ECHO					39
#define ZEND_PRINT					40

#define ZEND_JMP					41
#define ZEND_JMPZ					42
#define ZEND_JMPNZ					43
#define ZEND_JMPZNZ					44
#define ZEND_JMPZ_EX				45
#define ZEND_JMPNZ_EX				46
#define ZEND_CASE					47
#define ZEND_SWITCH_FREE			48
#define ZEND_BRK					49
#define ZEND_CONT					50
#define ZEND_BOOL					51

#define ZEND_INIT_STRING			52
#define ZEND_ADD_CHAR				53
#define ZEND_ADD_STRING				54
#define ZEND_ADD_VAR				55

#define ZEND_BEGIN_SILENCE			56
#define ZEND_END_SILENCE			57

#define ZEND_INIT_FCALL_BY_NAME		58
#define ZEND_DO_FCALL				59
#define ZEND_DO_FCALL_BY_NAME		60
#define ZEND_RETURN					61

#define ZEND_RECV					62
#define ZEND_RECV_INIT				63
									
#define ZEND_SEND_VAL				64
#define ZEND_SEND_VAR				65
#define ZEND_SEND_REF				66

#define ZEND_NEW 					67
#define ZEND_JMP_NO_CTOR			68
#define ZEND_FREE					69
									
#define ZEND_INIT_ARRAY				70
#define ZEND_ADD_ARRAY_ELEMENT		71
									
#define ZEND_INCLUDE_OR_EVAL		72
									
#define ZEND_UNSET_VAR				73
#define ZEND_UNSET_DIM_OBJ			74
#define ZEND_ISSET_ISEMPTY			75
									
#define ZEND_FE_RESET				76
#define ZEND_FE_FETCH				77
									
#define ZEND_EXIT					78


/* the following 15 opcodes are 5 groups of 3 opcodes each, and must
 * remain in that order!
 */
#define ZEND_FETCH_R				79
#define ZEND_FETCH_DIM_R			80
#define ZEND_FETCH_OBJ_R			81
#define ZEND_FETCH_W				82
#define ZEND_FETCH_DIM_W			83
#define ZEND_FETCH_OBJ_W			84
#define ZEND_FETCH_RW				85
#define ZEND_FETCH_DIM_RW			86
#define ZEND_FETCH_OBJ_RW			87
#define ZEND_FETCH_IS				88
#define ZEND_FETCH_DIM_IS			89
#define ZEND_FETCH_OBJ_IS			90
#define ZEND_FETCH_FUNC_ARG			91
#define ZEND_FETCH_DIM_FUNC_ARG		92
#define ZEND_FETCH_OBJ_FUNC_ARG		93

#define ZEND_FETCH_DIM_TMP_VAR		94
#define ZEND_FETCH_CONSTANT			95

#define ZEND_DECLARE_FUNCTION_OR_CLASS	96

#define ZEND_EXT_STMT				97
#define ZEND_EXT_FCALL_BEGIN		98
#define ZEND_EXT_FCALL_END			99
#define ZEND_EXT_NOP				100

/* end of block */




/* global/local fetches */
#define ZEND_FETCH_GLOBAL	0
#define ZEND_FETCH_LOCAL	1
#define ZEND_FETCH_STATIC	2

/* var status for backpatching */
#define BP_VAR_R			0
#define BP_VAR_W			1
#define BP_VAR_RW			2
#define BP_VAR_IS			3
#define BP_VAR_NA			4	/* if not applicable */
#define BP_VAR_FUNC_ARG		5


#define ZEND_INTERNAL_FUNCTION		1
#define ZEND_USER_FUNCTION			2
#define ZEND_OVERLOADED_FUNCTION	3
#define	ZEND_EVAL_CODE				4

#define ZEND_INTERNAL_CLASS		1
#define ZEND_USER_CLASS			2

#define ZEND_EVAL				(1<<0)
#define ZEND_INCLUDE			(1<<1)
#define ZEND_IMPORT				(1<<2)

#define ZEND_ISSET				(1<<0)
#define ZEND_ISEMPTY			(1<<1)

#define ZEND_CT	(1<<0)
#define ZEND_RT (1<<1)


#define ZEND_HANDLE_FILENAME		0
#define ZEND_HANDLE_FD				1
#define ZEND_HANDLE_FP				2
#define ZEND_HANDLE_STDIOSTREAM		3
#define ZEND_HANDLE_FSTREAM			4

#define ZEND_DECLARE_CLASS				1
#define ZEND_DECLARE_FUNCTION			2
#define ZEND_DECLARE_INHERITED_CLASS	3

#define ZEND_FETCH_STANDARD		0
#define ZEND_FETCH_ADD_LOCK		1

#define ZEND_MEMBER_FUNC_CALL	1<<0
#define ZEND_CTOR_CALL			1<<1


#define AI_USE_PTR(ai) \
	if ((ai).ptr_ptr) { \
		(ai).ptr = *((ai).ptr_ptr); \
		(ai).ptr_ptr = &((ai).ptr); \
	} else { \
		(ai).ptr = NULL; \
	}

#define PZVAL_IS_REF(z)		((z)->is_ref)

#define PZVAL_LOCK(z)	((z)->refcount++)
#define PZVAL_UNLOCK(z)	{ ((z)->refcount--);							\
							if (!(z)->refcount) {						\
								EG(garbage)[EG(garbage_ptr)++] = (z);	\
								if (EG(garbage_ptr) == 4) {				\
									zval_dtor(EG(garbage)[0]);			\
									efree(EG(garbage)[0]);				\
									zval_dtor(EG(garbage)[1]);			\
									efree(EG(garbage)[1]);				\
									EG(garbage)[0] = EG(garbage)[2];	\
									EG(garbage)[1] = EG(garbage)[3];	\
									EG(garbage_ptr) -= 2;				\
								}										\
							}											\
						}

#define SELECTIVE_PZVAL_LOCK(pzv, pzn)		if (!((pzn)->u.EA.type & EXT_TYPE_UNUSED)) { PZVAL_LOCK(pzv); }


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

#endif /* _COMPILE_H */
