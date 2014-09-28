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
   |          Nikita Popov <nikic@php.net>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include <zend_language_parser.h>
#include "zend.h"
#include "zend_compile.h"
#include "zend_constants.h"
#include "zend_llist.h"
#include "zend_API.h"
#include "zend_exceptions.h"
#include "zend_virtual_cwd.h"
#include "zend_multibyte.h"
#include "zend_language_scanner.h"
#include "zend_inheritance.h"

#define CONSTANT_EX(op_array, op) \
	(op_array)->literals[op]

#define CONSTANT(op) \
	CONSTANT_EX(CG(active_op_array), op)

#define SET_NODE(target, src) do { \
		target ## _type = (src)->op_type; \
		if ((src)->op_type == IS_CONST) { \
			target.constant = zend_add_literal(CG(active_op_array), &(src)->u.constant TSRMLS_CC); \
		} else { \
			target = (src)->u.op; \
		} \
	} while (0)

#define GET_NODE(target, src) do { \
		(target)->op_type = src ## _type; \
		if ((target)->op_type == IS_CONST) { \
			(target)->u.constant = CONSTANT(src.constant); \
		} else { \
			(target)->u.op = src; \
		} \
	} while (0)

static inline void zend_alloc_cache_slot(uint32_t literal TSRMLS_DC) {
	zend_op_array *op_array = CG(active_op_array);
	Z_CACHE_SLOT(op_array->literals[literal]) = op_array->last_cache_slot++;
}

#define POLYMORPHIC_CACHE_SLOT_SIZE 2

static inline void zend_alloc_polymorphic_cache_slot(uint32_t literal TSRMLS_DC) {
	zend_op_array *op_array = CG(active_op_array);
	Z_CACHE_SLOT(op_array->literals[literal]) = op_array->last_cache_slot;
	op_array->last_cache_slot += POLYMORPHIC_CACHE_SLOT_SIZE;
}

ZEND_API zend_op_array *(*zend_compile_file)(zend_file_handle *file_handle, int type TSRMLS_DC);
ZEND_API zend_op_array *(*zend_compile_string)(zval *source_string, char *filename TSRMLS_DC);

#ifndef ZTS
ZEND_API zend_compiler_globals compiler_globals;
ZEND_API zend_executor_globals executor_globals;
#endif

static void zend_destroy_property_info(zval *zv) /* {{{ */
{
	zend_property_info *property_info = Z_PTR_P(zv);

	zend_string_release(property_info->name);
	if (property_info->doc_comment) {
		zend_string_release(property_info->doc_comment);
	}
}
/* }}} */

static void zend_destroy_property_info_internal(zval *zv) /* {{{ */
{
	zend_property_info *property_info = Z_PTR_P(zv);

	zend_string_release(property_info->name);
	free(property_info);
}
/* }}} */

static zend_string *zend_new_interned_string_safe(zend_string *str TSRMLS_DC) /* {{{ */ {
	zend_string *interned_str;

	zend_string_addref(str);
	interned_str = zend_new_interned_string(str TSRMLS_CC);
	if (str != interned_str) {
		return interned_str;
	} else {
		zend_string_release(str);
		return str;
	}
}
/* }}} */

static zend_string *zend_build_runtime_definition_key(zend_string *name, unsigned char *lex_pos TSRMLS_DC) /* {{{ */
{
	zend_string *result;
	char char_pos_buf[32];
	size_t filename_len, char_pos_len = zend_sprintf(char_pos_buf, "%p", lex_pos);

	const char *filename;
	if (CG(active_op_array)->filename) {
		filename = CG(active_op_array)->filename->val;
		filename_len = CG(active_op_array)->filename->len;
	} else {
		filename = "-";
		filename_len = sizeof("-") - 1;
	}
	/* NULL, name length, filename length, last accepting char position length */
	result = zend_string_alloc(1 + name->len + filename_len + char_pos_len, 0);
 	sprintf(result->val, "%c%s%s%s", '\0', name->val, filename, char_pos_buf);
	return zend_new_interned_string(result TSRMLS_CC);
}
/* }}} */

static void init_compiler_declarables(TSRMLS_D) /* {{{ */
{
	ZVAL_LONG(&CG(declarables).ticks, 0);
}
/* }}} */

void zend_init_compiler_context(TSRMLS_D) /* {{{ */
{
	CG(context).opcodes_size = INITIAL_OP_ARRAY_SIZE;
	CG(context).vars_size = 0;
	CG(context).literals_size = 0;
	CG(context).current_brk_cont = -1;
	CG(context).backpatch_count = 0;
	CG(context).in_finally = 0;
	CG(context).labels = NULL;
}
/* }}} */

void zend_init_compiler_data_structures(TSRMLS_D) /* {{{ */
{
	zend_stack_init(&CG(loop_var_stack), sizeof(znode));
	zend_stack_init(&CG(delayed_oplines_stack), sizeof(zend_op));
	CG(active_class_entry) = NULL;
	CG(in_compilation) = 0;
	CG(start_lineno) = 0;
	CG(current_namespace) = NULL;
	CG(in_namespace) = 0;
	CG(has_bracketed_namespaces) = 0;
	CG(current_import) = NULL;
	CG(current_import_function) = NULL;
	CG(current_import_const) = NULL;
	zend_hash_init(&CG(const_filenames), 8, NULL, NULL, 0);
	init_compiler_declarables(TSRMLS_C);
	zend_stack_init(&CG(context_stack), sizeof(CG(context)));

	CG(encoding_declared) = 0;
}
/* }}} */

ZEND_API void file_handle_dtor(zend_file_handle *fh) /* {{{ */
{
	TSRMLS_FETCH();

	zend_file_handle_dtor(fh TSRMLS_CC);
}
/* }}} */

void init_compiler(TSRMLS_D) /* {{{ */
{
	CG(arena) = zend_arena_create(64 * 1024);
	CG(active_op_array) = NULL;
	memset(&CG(context), 0, sizeof(CG(context)));
	zend_init_compiler_data_structures(TSRMLS_C);
	zend_init_rsrc_list(TSRMLS_C);
	zend_hash_init(&CG(filenames_table), 8, NULL, free_string_zval, 0);
	zend_llist_init(&CG(open_files), sizeof(zend_file_handle), (void (*)(void *)) file_handle_dtor, 0);
	CG(unclean_shutdown) = 0;
}
/* }}} */

void shutdown_compiler(TSRMLS_D) /* {{{ */
{
	zend_stack_destroy(&CG(loop_var_stack));
	zend_stack_destroy(&CG(delayed_oplines_stack));
	zend_hash_destroy(&CG(filenames_table));
	zend_hash_destroy(&CG(const_filenames));
	zend_stack_destroy(&CG(context_stack));
	zend_arena_destroy(CG(arena));
}
/* }}} */

ZEND_API zend_string *zend_set_compiled_filename(zend_string *new_compiled_filename TSRMLS_DC) /* {{{ */
{
	zend_string *p;

	p = zend_hash_find_ptr(&CG(filenames_table), new_compiled_filename);
	if (p != NULL) {
		CG(compiled_filename) = p;
		return p;
	}
	p = zend_string_copy(new_compiled_filename);
	zend_hash_update_ptr(&CG(filenames_table), new_compiled_filename, p);
	CG(compiled_filename) = p;
	return p;
}
/* }}} */

ZEND_API void zend_restore_compiled_filename(zend_string *original_compiled_filename TSRMLS_DC) /* {{{ */
{
	CG(compiled_filename) = original_compiled_filename;
}
/* }}} */

ZEND_API zend_string *zend_get_compiled_filename(TSRMLS_D) /* {{{ */
{
	return CG(compiled_filename);
}
/* }}} */

ZEND_API int zend_get_compiled_lineno(TSRMLS_D) /* {{{ */
{
	return CG(zend_lineno);
}
/* }}} */

ZEND_API zend_bool zend_is_compiling(TSRMLS_D) /* {{{ */
{
	return CG(in_compilation);
}
/* }}} */

static uint32_t get_temporary_variable(zend_op_array *op_array) /* {{{ */
{
	return (uint32_t)op_array->T++;
}
/* }}} */

static int lookup_cv(zend_op_array *op_array, zend_string* name TSRMLS_DC) /* {{{ */{
	int i = 0;
	zend_ulong hash_value = zend_string_hash_val(name);

	while (i < op_array->last_var) {
		if (op_array->vars[i]->val == name->val ||
		    (op_array->vars[i]->h == hash_value &&
		     op_array->vars[i]->len == name->len &&
		     memcmp(op_array->vars[i]->val, name->val, name->len) == 0)) {
			zend_string_release(name);
			return (int)(zend_intptr_t)EX_VAR_NUM_2(NULL, i);
		}
		i++;
	}
	i = op_array->last_var;
	op_array->last_var++;
	if (op_array->last_var > CG(context).vars_size) {
		CG(context).vars_size += 16; /* FIXME */
		op_array->vars = erealloc(op_array->vars, CG(context).vars_size * sizeof(zend_string*));
	}

	op_array->vars[i] = zend_new_interned_string(name TSRMLS_CC);
	return (int)(zend_intptr_t)EX_VAR_NUM_2(NULL, i);
}
/* }}} */

void zend_del_literal(zend_op_array *op_array, int n) /* {{{ */
{
	zval_dtor(&CONSTANT_EX(op_array, n));
	if (n + 1 == op_array->last_literal) {
		op_array->last_literal--;
	} else {
		ZVAL_UNDEF(&CONSTANT_EX(op_array, n));
	}
}
/* }}} */

/* Common part of zend_add_literal and zend_append_individual_literal */
static inline void zend_insert_literal(zend_op_array *op_array, zval *zv, int literal_position TSRMLS_DC) /* {{{ */
{
	if (Z_TYPE_P(zv) == IS_STRING || Z_TYPE_P(zv) == IS_CONSTANT) {
		zend_string_hash_val(Z_STR_P(zv));
		Z_STR_P(zv) = zend_new_interned_string(Z_STR_P(zv) TSRMLS_CC);
		if (IS_INTERNED(Z_STR_P(zv))) {
			Z_TYPE_FLAGS_P(zv) &= ~ (IS_TYPE_REFCOUNTED | IS_TYPE_COPYABLE);
		}
	}
	ZVAL_COPY_VALUE(&CONSTANT_EX(op_array, literal_position), zv);
	Z_CACHE_SLOT(op_array->literals[literal_position]) = -1;
}
/* }}} */

/* Is used while compiling a function, using the context to keep track
   of an approximate size to avoid to relocate to often.
   Literals are truncated to actual size in the second compiler pass (pass_two()). */
int zend_add_literal(zend_op_array *op_array, zval *zv TSRMLS_DC) /* {{{ */
{
	int i = op_array->last_literal;
	op_array->last_literal++;
	if (i >= CG(context).literals_size) {
		while (i >= CG(context).literals_size) {
			CG(context).literals_size += 16; /* FIXME */
		}
		op_array->literals = (zval*)erealloc(op_array->literals, CG(context).literals_size * sizeof(zval));
	}
	zend_insert_literal(op_array, zv, i TSRMLS_CC);
	return i;
}
/* }}} */

static inline int zend_add_literal_string(zend_op_array *op_array, zend_string **str TSRMLS_DC) /* {{{ */
{
	int ret;
	zval zv;
	ZVAL_STR(&zv, *str);
	ret = zend_add_literal(op_array, &zv TSRMLS_CC);
	*str = Z_STR(zv);
	return ret;
}

static int zend_add_func_name_literal(zend_op_array *op_array, zend_string *name TSRMLS_DC) /* {{{ */
{
	int ret = zend_add_literal_string(op_array, &name TSRMLS_CC);

	zend_string *lc_name = zend_string_alloc(name->len, 0);
	zend_str_tolower_copy(lc_name->val, name->val, name->len);
	zend_add_literal_string(op_array, &lc_name TSRMLS_CC);

	return ret;
}
/* }}} */

static int zend_add_ns_func_name_literal(zend_op_array *op_array, zend_string *name TSRMLS_DC) /* {{{ */
{
	const char *ns_separator;

	int ret = zend_add_literal_string(op_array, &name TSRMLS_CC);

	zend_string *lc_name = zend_string_alloc(name->len, 0);
	zend_str_tolower_copy(lc_name->val, name->val, name->len);
	zend_add_literal_string(op_array, &lc_name TSRMLS_CC);

	ns_separator = zend_memrchr(name->val, '\\', name->len);
	if (ns_separator != NULL) {
		size_t len = name->len - (ns_separator - name->val + 1);
		lc_name = zend_string_alloc(len, 0);
		zend_str_tolower_copy(lc_name->val, ns_separator + 1, len);
		zend_add_literal_string(op_array, &lc_name TSRMLS_CC);
	}

	return ret;
}
/* }}} */

static int zend_add_class_name_literal(zend_op_array *op_array, zend_string *name TSRMLS_DC) /* {{{ */
{
	int ret = zend_add_literal_string(op_array, &name TSRMLS_CC);

	zend_string *lc_name = zend_string_alloc(name->len, 0);
	zend_str_tolower_copy(lc_name->val, name->val, name->len);
	zend_add_literal_string(op_array, &lc_name TSRMLS_CC);

	zend_alloc_cache_slot(ret TSRMLS_CC);

	return ret;
}
/* }}} */

static int zend_add_const_name_literal(zend_op_array *op_array, zend_string *name, zend_bool unqualified TSRMLS_DC) /* {{{ */
{
	zend_string *tmp_name;

	int ret = zend_add_literal_string(op_array, &name TSRMLS_CC);

	size_t ns_len = 0, after_ns_len = name->len;
	const char *after_ns = zend_memrchr(name->val, '\\', name->len);
	if (after_ns) {
		after_ns += 1;
		ns_len = after_ns - name->val - 1;
		after_ns_len = name->len - ns_len - 1;

		/* lowercased namespace name & original constant name */
		tmp_name = zend_string_init(name->val, name->len, 0);
		zend_str_tolower(tmp_name->val, ns_len);
		zend_add_literal_string(op_array, &tmp_name TSRMLS_CC);

		/* lowercased namespace name & lowercased constant name */
		tmp_name = zend_string_alloc(name->len, 0);
		zend_str_tolower_copy(tmp_name->val, name->val, name->len);
		zend_add_literal_string(op_array, &tmp_name TSRMLS_CC);

		if (!unqualified) {
			return ret;
		}
	} else {
		after_ns = name->val;
	}

	/* original unqualified constant name */
	tmp_name = zend_string_init(after_ns, after_ns_len, 0);
	zend_add_literal_string(op_array, &tmp_name TSRMLS_CC);

	/* lowercased unqualified constant name */
	tmp_name = zend_string_alloc(after_ns_len, 0);
	zend_str_tolower_copy(tmp_name->val, after_ns, after_ns_len);
	zend_add_literal_string(op_array, &tmp_name TSRMLS_CC);

	return ret;
}
/* }}} */

#define LITERAL_STR(op, str) do { \
		zval _c; \
		ZVAL_STR(&_c, str); \
		op.constant = zend_add_literal(CG(active_op_array), &_c TSRMLS_CC); \
	} while (0)

#define MAKE_NOP(opline) do { \
	opline->opcode = ZEND_NOP; \
	memset(&opline->result, 0, sizeof(opline->result)); \
	memset(&opline->op1, 0, sizeof(opline->op1)); \
	memset(&opline->op2, 0, sizeof(opline->op2)); \
	opline->result_type = opline->op1_type = opline->op2_type = IS_UNUSED; \
} while (0)

void zend_stop_lexing(TSRMLS_D) {
	LANG_SCNG(yy_cursor) = LANG_SCNG(yy_limit);
}

static inline void zend_begin_loop(TSRMLS_D) /* {{{ */
{
	zend_brk_cont_element *brk_cont_element;
	int parent;

	parent = CG(context).current_brk_cont;
	CG(context).current_brk_cont = CG(active_op_array)->last_brk_cont;
	brk_cont_element = get_next_brk_cont_element(CG(active_op_array));
	brk_cont_element->start = get_next_op_number(CG(active_op_array));
	brk_cont_element->parent = parent;
}
/* }}} */

static inline void zend_end_loop(int cont_addr, int has_loop_var TSRMLS_DC) /* {{{ */
{
	if (!has_loop_var) {
		/* The start fileld is used to free temporary variables in case of exceptions.
		 * We won't try to free something of we don't have loop variable.
		 */
		CG(active_op_array)->brk_cont_array[CG(context).current_brk_cont].start = -1;
	}
	CG(active_op_array)->brk_cont_array[CG(context).current_brk_cont].cont = cont_addr;
	CG(active_op_array)->brk_cont_array[CG(context).current_brk_cont].brk = get_next_op_number(CG(active_op_array));
	CG(context).current_brk_cont = CG(active_op_array)->brk_cont_array[CG(context).current_brk_cont].parent;
}
/* }}} */

void zend_do_free(znode *op1 TSRMLS_DC) /* {{{ */
{
	if (op1->op_type==IS_TMP_VAR) {
		zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

		opline->opcode = ZEND_FREE;
		SET_NODE(opline->op1, op1);
		SET_UNUSED(opline->op2);
	} else if (op1->op_type==IS_VAR) {
		zend_op *opline = &CG(active_op_array)->opcodes[CG(active_op_array)->last-1];

		while (opline->opcode == ZEND_END_SILENCE || opline->opcode == ZEND_EXT_FCALL_END || opline->opcode == ZEND_OP_DATA) {
			opline--;
		}
		if (opline->result_type == IS_VAR
			&& opline->result.var == op1->u.op.var) {
			if (opline->opcode == ZEND_FETCH_R ||
			    opline->opcode == ZEND_FETCH_DIM_R ||
			    opline->opcode == ZEND_FETCH_OBJ_R) {
				/* It's very rare and useless case. It's better to use
				   additional FREE opcode and simplify the FETCH handlers
				   their selves */
				opline = get_next_op(CG(active_op_array) TSRMLS_CC);
				opline->opcode = ZEND_FREE;
				SET_NODE(opline->op1, op1);
				SET_UNUSED(opline->op2);
			} else {
				opline->result_type |= EXT_TYPE_UNUSED;
			}
		} else {
			while (opline>CG(active_op_array)->opcodes) {
				if (opline->opcode == ZEND_FETCH_DIM_R
				    && opline->op1_type == IS_VAR
				    && opline->op1.var == op1->u.op.var) {
					/* This should the end of a list() construct
					 * Mark its result as unused
					 */
					opline->extended_value = ZEND_FETCH_STANDARD;
					break;
				} else if (opline->result_type==IS_VAR
					&& opline->result.var == op1->u.op.var) {
					if (opline->opcode == ZEND_NEW) {
						opline->result_type |= EXT_TYPE_UNUSED;
						opline = &CG(active_op_array)->opcodes[CG(active_op_array)->last-1];
						while (opline->opcode != ZEND_DO_FCALL || opline->op1.num != ZEND_CALL_CTOR) {
							opline--;
						}
						opline->op1.num |= ZEND_CALL_CTOR_RESULT_UNUSED;
					}
					break;
				}
				opline--;
			}
		}
	} else if (op1->op_type == IS_CONST) {
		/* Destroy value without using GC: When opcache moves arrays into SHM it will
		 * free the zend_array structure, so references to it from outside the op array
		 * become invalid. GC would cause such a reference in the root buffer. */
		zval_ptr_dtor_nogc(&op1->u.constant);
	}
}
/* }}} */

uint32_t zend_add_member_modifier(uint32_t flags, uint32_t new_flag) /* {{{ */
{
	uint32_t new_flags = flags | new_flag;
	if ((flags & ZEND_ACC_PPP_MASK) && (new_flag & ZEND_ACC_PPP_MASK)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Multiple access type modifiers are not allowed");
	}
	if ((flags & ZEND_ACC_ABSTRACT) && (new_flag & ZEND_ACC_ABSTRACT)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Multiple abstract modifiers are not allowed");
	}
	if ((flags & ZEND_ACC_STATIC) && (new_flag & ZEND_ACC_STATIC)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Multiple static modifiers are not allowed");
	}
	if ((flags & ZEND_ACC_FINAL) && (new_flag & ZEND_ACC_FINAL)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Multiple final modifiers are not allowed");
	}
	if ((new_flags & ZEND_ACC_ABSTRACT) && (new_flags & ZEND_ACC_FINAL)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use the final modifier on an abstract class member");
	}
	return new_flags;
}
/* }}} */

zend_string *zend_concat3(char *str1, size_t str1_len, char *str2, size_t str2_len, char *str3, size_t str3_len) /* {{{ */
{
	size_t len = str1_len + str2_len + str3_len;
	zend_string *res = zend_string_alloc(len, 0);

	memcpy(res->val, str1, str1_len);
	memcpy(res->val + str1_len, str2, str2_len);
	memcpy(res->val + str1_len + str2_len, str3, str3_len);
	res->val[len] = '\0';

	return res;
}

zend_string *zend_concat_names(char *name1, size_t name1_len, char *name2, size_t name2_len) {
	return zend_concat3(name1, name1_len, "\\", 1, name2, name2_len);
}

zend_string *zend_prefix_with_ns(zend_string *name TSRMLS_DC) {
	if (CG(current_namespace)) {
		zend_string *ns = CG(current_namespace);
		return zend_concat_names(ns->val, ns->len, name->val, name->len);
	} else {
		return zend_string_copy(name);
	}
}

void *zend_hash_find_ptr_lc(HashTable *ht, char *str, size_t len) {
	void *result;
	zend_string *lcname = zend_string_alloc(len, 0);
	zend_str_tolower_copy(lcname->val, str, len);
	result = zend_hash_find_ptr(ht, lcname);
	zend_string_free(lcname);
	return result;
}

zend_string *zend_resolve_non_class_name(
	zend_string *name, uint32_t type, zend_bool *is_fully_qualified,
	zend_bool case_sensitive, HashTable *current_import_sub TSRMLS_DC
) {
	char *compound;
	*is_fully_qualified = 0;

	if (name->val[0] == '\\') {
		/* Remove \ prefix (only relevant if this is a string rather than a label) */
		return zend_string_init(name->val + 1, name->len - 1, 0);
	}

	if (type == ZEND_NAME_FQ) {
		*is_fully_qualified = 1;
		return zend_string_copy(name);
	}

	if (type == ZEND_NAME_RELATIVE) {
		*is_fully_qualified = 1;
		return zend_prefix_with_ns(name TSRMLS_CC);
	}

	if (current_import_sub) {
		/* If an unqualified name is a function/const alias, replace it. */
		zend_string *import_name;
		if (case_sensitive) {
			import_name = zend_hash_find_ptr(current_import_sub, name);
		} else {
			import_name = zend_hash_find_ptr_lc(current_import_sub, name->val, name->len);
		}

		if (import_name) {
			*is_fully_qualified = 1;
			return zend_string_copy(import_name);
		}
	}

	compound = memchr(name->val, '\\', name->len);
	if (compound) {
		*is_fully_qualified = 1;
	}

	if (compound && CG(current_import)) {
		/* If the first part of a qualified name is an alias, substitute it. */
		size_t len = compound - name->val;
		zend_string *import_name = zend_hash_find_ptr_lc(CG(current_import), name->val, len);

		if (import_name) {
			return zend_concat_names(
				import_name->val, import_name->len, name->val + len + 1, name->len - len - 1);
		}
	}

	return zend_prefix_with_ns(name TSRMLS_CC);
}
/* }}} */

zend_string *zend_resolve_function_name(zend_string *name, uint32_t type, zend_bool *is_fully_qualified TSRMLS_DC) /* {{{ */
{
	return zend_resolve_non_class_name(
		name, type, is_fully_qualified, 0, CG(current_import_function) TSRMLS_CC);
}
/* }}} */

zend_string *zend_resolve_const_name(zend_string *name, uint32_t type, zend_bool *is_fully_qualified TSRMLS_DC) /* {{{ */ {
	return zend_resolve_non_class_name(
		name, type, is_fully_qualified, 1, CG(current_import_const) TSRMLS_CC);
}
/* }}} */

zend_string *zend_resolve_class_name(zend_string *name, uint32_t type TSRMLS_DC) /* {{{ */
{
	char *compound;

	if (type == ZEND_NAME_RELATIVE) {
		return zend_prefix_with_ns(name TSRMLS_CC);
	}

	if (type == ZEND_NAME_FQ || name->val[0] == '\\') {
		/* Remove \ prefix (only relevant if this is a string rather than a label) */
		if (name->val[0] == '\\') {
			name = zend_string_init(name->val + 1, name->len - 1, 0);
		} else {
			zend_string_addref(name);
		}
		/* Ensure that \self, \parent and \static are not used */
		if (ZEND_FETCH_CLASS_DEFAULT != zend_get_class_fetch_type(name)) {
			zend_error_noreturn(E_COMPILE_ERROR, "'\\%s' is an invalid class name", name->val);
		}
		return name;
	}

	if (CG(current_import)) {
		compound = memchr(name->val, '\\', name->len);
		if (compound) {
			/* If the first part of a qualified name is an alias, substitute it. */
			size_t len = compound - name->val;
			zend_string *import_name = zend_hash_find_ptr_lc(CG(current_import), name->val, len);

			if (import_name) {
				return zend_concat_names(
					import_name->val, import_name->len, name->val + len + 1, name->len - len - 1);
			}
		} else {
			/* If an unqualified name is an alias, replace it. */
			zend_string *import_name
				= zend_hash_find_ptr_lc(CG(current_import), name->val, name->len);

			if (import_name) {
				return zend_string_copy(import_name);
			}
		}
	}

	/* If not fully qualified and not an alias, prepend the current namespace */
	return zend_prefix_with_ns(name TSRMLS_CC);
}
/* }}} */

zend_string *zend_resolve_class_name_ast(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_string *name = zend_ast_get_str(ast);
	return zend_resolve_class_name(name, ast->attr TSRMLS_CC);
}
/* }}} */

static void ptr_dtor(zval *zv) /* {{{ */
{
	efree(Z_PTR_P(zv));
}
/* }}} */

static void str_dtor(zval *zv)  /* {{{ */ {
	zend_string_release(Z_STR_P(zv));
}
/* }}} */

void zend_resolve_goto_label(zend_op_array *op_array, zend_op *opline, int pass2 TSRMLS_DC) /* {{{ */
{
	zend_label *dest;
	zend_long current, distance;
	zval *label;

	if (pass2) {
		label = opline->op2.zv;
	} else {
		label = &CONSTANT_EX(op_array, opline->op2.constant);
	}
	if (CG(context).labels == NULL ||
	    (dest = zend_hash_find_ptr(CG(context).labels, Z_STR_P(label))) == NULL) {

		if (pass2) {
			CG(in_compilation) = 1;
			CG(active_op_array) = op_array;
			CG(zend_lineno) = opline->lineno;
			zend_error_noreturn(E_COMPILE_ERROR, "'goto' to undefined label '%s'", Z_STRVAL_P(label));
		} else {
			/* Label is not defined. Delay to pass 2. */
			return;
		}
	}

	opline->op1.opline_num = dest->opline_num;
	zval_dtor(label);
	ZVAL_NULL(label);

	/* Check that we are not moving into loop or switch */
	current = opline->extended_value;
	for (distance = 0; current != dest->brk_cont; distance++) {
		if (current == -1) {
			if (pass2) {
				CG(in_compilation) = 1;
				CG(active_op_array) = op_array;
				CG(zend_lineno) = opline->lineno;
			}
			zend_error_noreturn(E_COMPILE_ERROR, "'goto' into loop or switch statement is disallowed");
		}
		current = op_array->brk_cont_array[current].parent;
	}

	if (distance == 0) {
		/* Nothing to break out of, optimize to ZEND_JMP */
		opline->opcode = ZEND_JMP;
		opline->extended_value = 0;
		SET_UNUSED(opline->op2);
	} else {
		/* Set real break distance */
		ZVAL_LONG(label, distance);
	}
}
/* }}} */

void zend_release_labels(int temporary TSRMLS_DC) /* {{{ */
{
	if (CG(context).labels) {
		zend_hash_destroy(CG(context).labels);
		FREE_HASHTABLE(CG(context).labels);
		CG(context).labels = NULL;
	}
	if (!temporary && !zend_stack_is_empty(&CG(context_stack))) {
		zend_compiler_context *ctx = zend_stack_top(&CG(context_stack));
		CG(context) = *ctx;
		zend_stack_del_top(&CG(context_stack));
	}
}
/* }}} */

static zend_bool zend_is_call(zend_ast *ast);

static int generate_free_loop_var(znode *var TSRMLS_DC) /* {{{ */
{
	switch (var->op_type) {
		case IS_UNUSED:
			/* Stack separator on function boundary, stop applying */
			return 1;
		case IS_VAR:
		case IS_TMP_VAR:
		{
			zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

			opline->opcode = ZEND_FREE;
			SET_NODE(opline->op1, var);
			SET_UNUSED(opline->op2);
		}
	}

	return 0;
}
/* }}} */

static uint32_t zend_add_try_element(uint32_t try_op TSRMLS_DC) /* {{{ */
{
	zend_op_array *op_array = CG(active_op_array);
	uint32_t try_catch_offset = op_array->last_try_catch++;
	zend_try_catch_element *elem;

	op_array->try_catch_array = safe_erealloc(
		op_array->try_catch_array, sizeof(zend_try_catch_element), op_array->last_try_catch, 0);

	elem = &op_array->try_catch_array[try_catch_offset];
	elem->try_op = try_op;
	elem->catch_op = 0;
	elem->finally_op = 0;
	elem->finally_end = 0;

	return try_catch_offset;
}
/* }}} */

ZEND_API void function_add_ref(zend_function *function) /* {{{ */
{
	if (function->type == ZEND_USER_FUNCTION) {
		zend_op_array *op_array = &function->op_array;

		(*op_array->refcount)++;
		if (op_array->static_variables) {
			HashTable *static_variables = op_array->static_variables;

			ALLOC_HASHTABLE(op_array->static_variables);
			zend_array_dup(op_array->static_variables, static_variables);
		}
		op_array->run_time_cache = NULL;
	} else if (function->type == ZEND_INTERNAL_FUNCTION) {
		if (function->common.function_name) {
			zend_string_addref(function->common.function_name);
		}
	}
}
/* }}} */

ZEND_API int do_bind_function(const zend_op_array *op_array, const zend_op *opline, HashTable *function_table, zend_bool compile_time TSRMLS_DC) /* {{{ */
{
	zend_function *function, *new_function;
	zval *op1, *op2;

	if (compile_time) {
		op1 = &CONSTANT_EX(op_array, opline->op1.constant);
		op2 = &CONSTANT_EX(op_array, opline->op2.constant);
	} else {
		op1 = opline->op1.zv;
		op2 = opline->op2.zv;
	}

	function = zend_hash_find_ptr(function_table, Z_STR_P(op1));
	new_function = zend_arena_alloc(&CG(arena), sizeof(zend_op_array));
	memcpy(new_function, function, sizeof(zend_op_array));
	if (zend_hash_add_ptr(function_table, Z_STR_P(op2), new_function) == NULL) {
		int error_level = compile_time ? E_COMPILE_ERROR : E_ERROR;
		zend_function *old_function;

		efree_size(new_function, sizeof(zend_op_array));
		if ((old_function = zend_hash_find_ptr(function_table, Z_STR_P(op2))) != NULL
			&& old_function->type == ZEND_USER_FUNCTION
			&& old_function->op_array.last > 0) {
			zend_error(error_level, "Cannot redeclare %s() (previously declared in %s:%d)",
						function->common.function_name->val,
						old_function->op_array.filename->val,
						old_function->op_array.opcodes[0].lineno);
		} else {
			zend_error(error_level, "Cannot redeclare %s()", function->common.function_name->val);
		}
		return FAILURE;
	} else {
		(*function->op_array.refcount)++;
		function->op_array.static_variables = NULL; /* NULL out the unbound function */
		return SUCCESS;
	}
}
/* }}} */

ZEND_API zend_class_entry *do_bind_class(const zend_op_array* op_array, const zend_op *opline, HashTable *class_table, zend_bool compile_time TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce;
	zval *op1, *op2;

	if (compile_time) {
		op1 = &CONSTANT_EX(op_array, opline->op1.constant);
		op2 = &CONSTANT_EX(op_array, opline->op2.constant);
	} else {
		op1 = opline->op1.zv;
		op2 = opline->op2.zv;
	}
	if ((ce = zend_hash_find_ptr(class_table, Z_STR_P(op1))) == NULL) {
		zend_error_noreturn(E_COMPILE_ERROR, "Internal Zend error - Missing class information for %s", Z_STRVAL_P(op1));
		return NULL;
	}
	ce->refcount++;
	if (zend_hash_add_ptr(class_table, Z_STR_P(op2), ce) == NULL) {
		ce->refcount--;
		if (!compile_time) {
			/* If we're in compile time, in practice, it's quite possible
			 * that we'll never reach this class declaration at runtime,
			 * so we shut up about it.  This allows the if (!defined('FOO')) { return; }
			 * approach to work.
			 */
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare class %s", ce->name->val);
		}
		return NULL;
	} else {
		if (!(ce->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_IMPLEMENT_INTERFACES|ZEND_ACC_IMPLEMENT_TRAITS))) {
			zend_verify_abstract_class(ce TSRMLS_CC);
		}
		return ce;
	}
}
/* }}} */

ZEND_API zend_class_entry *do_bind_inherited_class(const zend_op_array *op_array, const zend_op *opline, HashTable *class_table, zend_class_entry *parent_ce, zend_bool compile_time TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce;
	zval *op1, *op2;

	if (compile_time) {
		op1 = &CONSTANT_EX(op_array, opline->op1.constant);
		op2 = &CONSTANT_EX(op_array, opline->op2.constant);
	} else {
		op1 = opline->op1.zv;
		op2 = opline->op2.zv;
	}

	ce = zend_hash_find_ptr(class_table, Z_STR_P(op1));

	if (!ce) {
		if (!compile_time) {
			/* If we're in compile time, in practice, it's quite possible
			 * that we'll never reach this class declaration at runtime,
			 * so we shut up about it.  This allows the if (!defined('FOO')) { return; }
			 * approach to work.
			 */
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare class %s", Z_STRVAL_P(op2));
		}
		return NULL;
	}

	if (parent_ce->ce_flags & ZEND_ACC_INTERFACE) {
		zend_error_noreturn(E_COMPILE_ERROR, "Class %s cannot extend from interface %s", ce->name->val, parent_ce->name->val);
	} else if ((parent_ce->ce_flags & ZEND_ACC_TRAIT) == ZEND_ACC_TRAIT) {
		zend_error_noreturn(E_COMPILE_ERROR, "Class %s cannot extend from trait %s", ce->name->val, parent_ce->name->val);
	}

	zend_do_inheritance(ce, parent_ce TSRMLS_CC);

	ce->refcount++;

	/* Register the derived class */
	if (zend_hash_add_ptr(class_table, Z_STR_P(op2), ce) == NULL) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare class %s", ce->name->val);
	}
	return ce;
}
/* }}} */

void zend_do_early_binding(TSRMLS_D) /* {{{ */
{
	zend_op *opline = &CG(active_op_array)->opcodes[CG(active_op_array)->last-1];
	HashTable *table;

	while (opline->opcode == ZEND_TICKS && opline > CG(active_op_array)->opcodes) {
		opline--;
	}

	switch (opline->opcode) {
		case ZEND_DECLARE_FUNCTION:
			if (do_bind_function(CG(active_op_array), opline, CG(function_table), 1 TSRMLS_CC) == FAILURE) {
				return;
			}
			table = CG(function_table);
			break;
		case ZEND_DECLARE_CLASS:
			if (do_bind_class(CG(active_op_array), opline, CG(class_table), 1 TSRMLS_CC) == NULL) {
				return;
			}
			table = CG(class_table);
			break;
		case ZEND_DECLARE_INHERITED_CLASS:
			{
				zend_op *fetch_class_opline = opline-1;
				zval *parent_name;
				zend_class_entry *ce;

				parent_name = &CONSTANT(fetch_class_opline->op2.constant);
				if (((ce = zend_lookup_class(Z_STR_P(parent_name) TSRMLS_CC)) == NULL) ||
				    ((CG(compiler_options) & ZEND_COMPILE_IGNORE_INTERNAL_CLASSES) &&
				     (ce->type == ZEND_INTERNAL_CLASS))) {
				    if (CG(compiler_options) & ZEND_COMPILE_DELAYED_BINDING) {
						uint32_t *opline_num = &CG(active_op_array)->early_binding;

						while (*opline_num != -1) {
							opline_num = &CG(active_op_array)->opcodes[*opline_num].result.opline_num;
						}
						*opline_num = opline - CG(active_op_array)->opcodes;
						opline->opcode = ZEND_DECLARE_INHERITED_CLASS_DELAYED;
						opline->result_type = IS_UNUSED;
						opline->result.opline_num = -1;
					}
					return;
				}
				if (do_bind_inherited_class(CG(active_op_array), opline, CG(class_table), ce, 1 TSRMLS_CC) == NULL) {
					return;
				}
				/* clear unnecessary ZEND_FETCH_CLASS opcode */
				zend_del_literal(CG(active_op_array), fetch_class_opline->op2.constant);
				MAKE_NOP(fetch_class_opline);

				table = CG(class_table);
				break;
			}
		case ZEND_VERIFY_ABSTRACT_CLASS:
		case ZEND_ADD_INTERFACE:
		case ZEND_ADD_TRAIT:
		case ZEND_BIND_TRAITS:
			/* We currently don't early-bind classes that implement interfaces */
			/* Classes with traits are handled exactly the same, no early-bind here */
			return;
		default:
			zend_error_noreturn(E_COMPILE_ERROR, "Invalid binding type");
			return;
	}

	zend_hash_del(table, Z_STR(CONSTANT(opline->op1.constant)));
	zend_del_literal(CG(active_op_array), opline->op1.constant);
	zend_del_literal(CG(active_op_array), opline->op2.constant);
	MAKE_NOP(opline);
}
/* }}} */

ZEND_API void zend_do_delayed_early_binding(const zend_op_array *op_array TSRMLS_DC) /* {{{ */
{
	if (op_array->early_binding != -1) {
		zend_bool orig_in_compilation = CG(in_compilation);
		uint32_t opline_num = op_array->early_binding;
		zend_class_entry *ce;

		CG(in_compilation) = 1;
		while (opline_num != -1) {
			if ((ce = zend_lookup_class(Z_STR_P(op_array->opcodes[opline_num-1].op2.zv) TSRMLS_CC)) != NULL) {
				do_bind_inherited_class(op_array, &op_array->opcodes[opline_num], EG(class_table), ce, 0 TSRMLS_CC);
			}
			opline_num = op_array->opcodes[opline_num].result.opline_num;
		}
		CG(in_compilation) = orig_in_compilation;
	}
}
/* }}} */

ZEND_API zend_string *zend_mangle_property_name(const char *src1, size_t src1_length, const char *src2, size_t src2_length, int internal) /* {{{ */
{
	size_t prop_name_length = 1 + src1_length + 1 + src2_length;
	zend_string *prop_name = zend_string_alloc(prop_name_length, internal);

	prop_name->val[0] = '\0';
	memcpy(prop_name->val + 1, src1, src1_length+1);
	memcpy(prop_name->val + 1 + src1_length + 1, src2, src2_length+1);
	return prop_name;
}
/* }}} */

static int zend_strnlen(const char* s, size_t maxlen) /* {{{ */
{
	size_t len = 0;
	while (*s++ && maxlen--) len++;
	return len;
}
/* }}} */

ZEND_API int zend_unmangle_property_name_ex(const zend_string *name, const char **class_name, const char **prop_name, size_t *prop_len) /* {{{ */
{
	size_t class_name_len;

	*class_name = NULL;

	if (name->val[0] != '\0') {
		*prop_name = name->val;
		if (prop_len) {
			*prop_len = name->len;
		}
		return SUCCESS;
	}
	if (name->len < 3 || name->val[1] == '\0') {
		zend_error(E_NOTICE, "Illegal member variable name");
		*prop_name = name->val;
		if (prop_len) {
			*prop_len = name->len;
		}
		return FAILURE;
	}

	class_name_len = zend_strnlen(name->val + 1, name->len - 2);
	if (class_name_len >= name->len - 2 || name->val[class_name_len + 1] != '\0') {
		zend_error(E_NOTICE, "Corrupt member variable name");
		*prop_name = name->val;
		if (prop_len) {
			*prop_len = name->len;
		}
		return FAILURE;
	}

	*class_name = name->val + 1;
	*prop_name = name->val + class_name_len + 2;
	if (prop_len) {
		*prop_len = name->len - class_name_len - 2;
	}
	return SUCCESS;
}
/* }}} */

static zend_constant *zend_get_ct_const(zend_string *name, int all_internal_constants_substitution TSRMLS_DC) /* {{{ */
{
	zend_constant *c = NULL;
	char *lookup_name;

	if (name->val[0] == '\\') {
		c = zend_hash_str_find_ptr(EG(zend_constants), name->val + 1, name->len - 1);
		if (!c) {
			lookup_name = zend_str_tolower_dup(name->val + 1, name->len - 1);
			c = zend_hash_str_find_ptr(EG(zend_constants), lookup_name, name->len - 1);
			efree(lookup_name);

			if (c && (c->flags & CONST_CT_SUBST) && !(c->flags & CONST_CS)) {
				return c;
			}
			return NULL;
		}
	} else if ((c = zend_hash_find_ptr(EG(zend_constants), name)) == NULL) {
		lookup_name = zend_str_tolower_dup(name->val, name->len);
		c = zend_hash_str_find_ptr(EG(zend_constants), lookup_name, name->len);
		efree(lookup_name);

		if (c && (c->flags & CONST_CT_SUBST) && !(c->flags & CONST_CS)) {
			return c;
		}
		return NULL;
	}

	if (c->flags & CONST_CT_SUBST) {
		return c;
	}
	if (all_internal_constants_substitution &&
	    (c->flags & CONST_PERSISTENT) &&
	    !(CG(compiler_options) & ZEND_COMPILE_NO_CONSTANT_SUBSTITUTION) &&
	    !Z_CONSTANT(c->value)) {
		return c;
	}
	return NULL;
}
/* }}} */

static int zend_constant_ct_subst(zval *result, zend_string *name, int all_internal_constants_substitution TSRMLS_DC) /* {{{ */
{
	zend_constant *c = zend_get_ct_const(name, all_internal_constants_substitution TSRMLS_CC);

	if (c) {
		ZVAL_DUP(result, &c->value);
		return 1;
	}
	return 0;
}
/* }}} */

void zend_init_list(void *result, void *item TSRMLS_DC) /* {{{ */
{
	void** list = emalloc(sizeof(void*) * 2);

	list[0] = item;
	list[1] = NULL;

	*(void**)result = list;
}
/* }}} */

void zend_add_to_list(void *result, void *item TSRMLS_DC) /* {{{ */
{
	void** list = *(void**)result;
	size_t n = 0;

	if (list) {
		while (list[n]) {
			n++;
		}
	}

	list = erealloc(list, sizeof(void*) * (n+2));

	list[n]   = item;
	list[n+1] = NULL;

	*(void**)result = list;
}
/* }}} */

void zend_do_extended_info(TSRMLS_D) /* {{{ */
{
	zend_op *opline;

	if (!(CG(compiler_options) & ZEND_COMPILE_EXTENDED_INFO)) {
		return;
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_EXT_STMT;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}
/* }}} */

void zend_do_extended_fcall_begin(TSRMLS_D) /* {{{ */
{
	zend_op *opline;

	if (!(CG(compiler_options) & ZEND_COMPILE_EXTENDED_INFO)) {
		return;
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_EXT_FCALL_BEGIN;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}
/* }}} */

void zend_do_extended_fcall_end(TSRMLS_D) /* {{{ */
{
	zend_op *opline;

	if (!(CG(compiler_options) & ZEND_COMPILE_EXTENDED_INFO)) {
		return;
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_EXT_FCALL_END;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}
/* }}} */

zend_bool zend_is_auto_global(zend_string *name TSRMLS_DC) /* {{{ */
{
	zend_auto_global *auto_global;

	if ((auto_global = zend_hash_find_ptr(CG(auto_globals), name)) != NULL) {
		if (auto_global->armed) {
			auto_global->armed = auto_global->auto_global_callback(auto_global->name TSRMLS_CC);
		}
		return 1;
	}
	return 0;
}
/* }}} */

int zend_register_auto_global(zend_string *name, zend_bool jit, zend_auto_global_callback auto_global_callback TSRMLS_DC) /* {{{ */
{
	zend_auto_global auto_global;
	int retval;

	auto_global.name = zend_new_interned_string(name TSRMLS_CC);
	auto_global.auto_global_callback = auto_global_callback;
	auto_global.jit = jit;

	retval = zend_hash_add_mem(CG(auto_globals), name, &auto_global, sizeof(zend_auto_global)) != NULL ? SUCCESS : FAILURE;

	zend_string_release(auto_global.name);
	return retval;
}
/* }}} */

ZEND_API void zend_activate_auto_globals(TSRMLS_D) /* {{{ */
{
	zend_auto_global *auto_global;

	ZEND_HASH_FOREACH_PTR(CG(auto_globals), auto_global) {
		if (auto_global->jit) {
			auto_global->armed = 1;
		} else if (auto_global->auto_global_callback) {
			auto_global->armed = auto_global->auto_global_callback(auto_global->name TSRMLS_CC);
		} else {
			auto_global->armed = 0;
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

int zendlex(zend_parser_stack_elem *elem TSRMLS_DC) /* {{{ */
{
	zval zv;
	int retval;

	if (CG(increment_lineno)) {
		CG(zend_lineno)++;
		CG(increment_lineno) = 0;
	}

again:
	ZVAL_UNDEF(&zv);
	retval = lex_scan(&zv TSRMLS_CC);
	switch (retval) {
		case T_COMMENT:
		case T_DOC_COMMENT:
		case T_OPEN_TAG:
		case T_WHITESPACE:
			goto again;

		case T_CLOSE_TAG:
			if (LANG_SCNG(yy_text)[LANG_SCNG(yy_leng)-1] != '>') {
				CG(increment_lineno) = 1;
			}
			retval = ';'; /* implicit ; */
			break;
		case T_OPEN_TAG_WITH_ECHO:
			retval = T_ECHO;
			break;
	}
	if (Z_TYPE(zv) != IS_UNDEF) {
		elem->ast = zend_ast_create_zval(&zv);
	}

	return retval;
}
/* }}} */

ZEND_API void zend_initialize_class_data(zend_class_entry *ce, zend_bool nullify_handlers TSRMLS_DC) /* {{{ */
{
	zend_bool persistent_hashes = (ce->type == ZEND_INTERNAL_CLASS) ? 1 : 0;
	dtor_func_t zval_ptr_dtor_func = ((persistent_hashes) ? ZVAL_INTERNAL_PTR_DTOR : ZVAL_PTR_DTOR);

	ce->refcount = 1;
	ce->ce_flags = ZEND_ACC_CONSTANTS_UPDATED;

	ce->default_properties_table = NULL;
	ce->default_static_members_table = NULL;
	zend_hash_init_ex(&ce->properties_info, 8, NULL, (persistent_hashes ? zend_destroy_property_info_internal : zend_destroy_property_info), persistent_hashes, 0);
	zend_hash_init_ex(&ce->constants_table, 8, NULL, zval_ptr_dtor_func, persistent_hashes, 0);
	zend_hash_init_ex(&ce->function_table, 8, NULL, ZEND_FUNCTION_DTOR, persistent_hashes, 0);

	if (ce->type == ZEND_INTERNAL_CLASS) {
#ifdef ZTS
		int n = zend_hash_num_elements(CG(class_table));

		if (CG(static_members_table) && n >= CG(last_static_member)) {
			/* Support for run-time declaration: dl() */
			CG(last_static_member) = n+1;
			CG(static_members_table) = realloc(CG(static_members_table), (n+1)*sizeof(zval*));
			CG(static_members_table)[n] = NULL;
		}
		ce->static_members_table = (zval*)(zend_intptr_t)n;
#else
		ce->static_members_table = NULL;
#endif
	} else {
		ce->static_members_table = ce->default_static_members_table;
		ce->info.user.doc_comment = NULL;
	}

	ce->default_properties_count = 0;
	ce->default_static_members_count = 0;

	if (nullify_handlers) {
		ce->constructor = NULL;
		ce->destructor = NULL;
		ce->clone = NULL;
		ce->__get = NULL;
		ce->__set = NULL;
		ce->__unset = NULL;
		ce->__isset = NULL;
		ce->__call = NULL;
		ce->__callstatic = NULL;
		ce->__tostring = NULL;
		ce->create_object = NULL;
		ce->get_iterator = NULL;
		ce->iterator_funcs.funcs = NULL;
		ce->interface_gets_implemented = NULL;
		ce->get_static_method = NULL;
		ce->parent = NULL;
		ce->num_interfaces = 0;
		ce->interfaces = NULL;
		ce->num_traits = 0;
		ce->traits = NULL;
		ce->trait_aliases = NULL;
		ce->trait_precedences = NULL;
		ce->serialize = NULL;
		ce->unserialize = NULL;
		ce->serialize_func = NULL;
		ce->unserialize_func = NULL;
		ce->__debugInfo = NULL;
		if (ce->type == ZEND_INTERNAL_CLASS) {
			ce->info.internal.module = NULL;
			ce->info.internal.builtin_functions = NULL;
		}
	}
}
/* }}} */

uint32_t zend_get_class_fetch_type(zend_string *name) /* {{{ */ 
{
	if (zend_string_equals_literal_ci(name, "self")) {
		return ZEND_FETCH_CLASS_SELF;
	} else if (zend_string_equals_literal_ci(name, "parent")) {
		return ZEND_FETCH_CLASS_PARENT;
	} else if (zend_string_equals_literal_ci(name, "static")) {
		return ZEND_FETCH_CLASS_STATIC;
	} else {
		return ZEND_FETCH_CLASS_DEFAULT;
	}
}
/* }}} */

ZEND_API zend_string *zend_get_compiled_variable_name(const zend_op_array *op_array, uint32_t var) /* {{{ */
{
	return op_array->vars[EX_VAR_TO_NUM(var)];
}
/* }}} */

zend_ast *zend_ast_append_str(zend_ast *left_ast, zend_ast *right_ast) /* {{{ */
{
	zval *left_zv = zend_ast_get_zval(left_ast);
	zend_string *left = Z_STR_P(left_zv);
	zend_string *right = zend_ast_get_str(right_ast);

	zend_string *result;
	size_t left_len = left->len;
	size_t len = left_len + right->len + 1; /* left\right */

	result = zend_string_realloc(left, len, 0);
	result->val[left_len] = '\\';
	memcpy(&result->val[left_len + 1], right->val, right->len);
	result->val[len] = '\0';
	zend_string_release(right);

	ZVAL_STR(left_zv, result);
	return left_ast;
}
/* }}} */

/* A hacky way that is used to store the doc comment for properties */
zend_ast *zend_ast_append_doc_comment(zend_ast *list TSRMLS_DC) /* {{{ */
{
	if (CG(doc_comment)) {
		list = zend_ast_list_add(list, zend_ast_create_zval_from_str(CG(doc_comment)));
		CG(doc_comment) = NULL;
	}

	return list;
}
/* }}} */

void zend_verify_namespace(TSRMLS_D) /* {{{ */
{
	if (CG(has_bracketed_namespaces) && !CG(in_namespace)) {
		zend_error_noreturn(E_COMPILE_ERROR, "No code may exist outside of namespace {}");
	}
}
/* }}} */

static void zend_reset_import_tables(TSRMLS_D) /* {{{ */
{
	if (CG(current_import)) {
		zend_hash_destroy(CG(current_import));
		efree(CG(current_import));
		CG(current_import) = NULL;
	}

	if (CG(current_import_function)) {
		zend_hash_destroy(CG(current_import_function));
		efree(CG(current_import_function));
		CG(current_import_function) = NULL;
	}

	if (CG(current_import_const)) {
		zend_hash_destroy(CG(current_import_const));
		efree(CG(current_import_const));
		CG(current_import_const) = NULL;
	}
}
/* }}} */

static void zend_end_namespace(TSRMLS_D) /* {{{ */ {
	CG(in_namespace) = 0;
	zend_reset_import_tables(TSRMLS_C);
	if (CG(current_namespace)) {
		zend_string_release(CG(current_namespace));
		CG(current_namespace) = NULL;
	}
}
/* }}} */

void zend_do_end_compilation(TSRMLS_D) /* {{{ */
{
	CG(has_bracketed_namespaces) = 0;
	zend_end_namespace(TSRMLS_C);
}
/* }}} */

/* {{{ zend_dirname
   Returns directory name component of path */
ZEND_API size_t zend_dirname(char *path, size_t len)
{
	register char *end = path + len - 1;
	unsigned int len_adjust = 0;

#ifdef PHP_WIN32
	/* Note that on Win32 CWD is per drive (heritage from CP/M).
	 * This means dirname("c:foo") maps to "c:." or "c:" - which means CWD on C: drive.
	 */
	if ((2 <= len) && isalpha((int)((unsigned char *)path)[0]) && (':' == path[1])) {
		/* Skip over the drive spec (if any) so as not to change */
		path += 2;
		len_adjust += 2;
		if (2 == len) {
			/* Return "c:" on Win32 for dirname("c:").
			 * It would be more consistent to return "c:."
			 * but that would require making the string *longer*.
			 */
			return len;
		}
	}
#elif defined(NETWARE)
	/*
	 * Find the first occurrence of : from the left
	 * move the path pointer to the position just after :
	 * increment the len_adjust to the length of path till colon character(inclusive)
	 * If there is no character beyond : simple return len
	 */
	char *colonpos = NULL;
	colonpos = strchr(path, ':');
	if (colonpos != NULL) {
		len_adjust = ((colonpos - path) + 1);
		path += len_adjust;
		if (len_adjust == len) {
			return len;
		}
	}
#endif

	if (len == 0) {
		/* Illegal use of this function */
		return 0;
	}

	/* Strip trailing slashes */
	while (end >= path && IS_SLASH_P(end)) {
		end--;
	}
	if (end < path) {
		/* The path only contained slashes */
		path[0] = DEFAULT_SLASH;
		path[1] = '\0';
		return 1 + len_adjust;
	}

	/* Strip filename */
	while (end >= path && !IS_SLASH_P(end)) {
		end--;
	}
	if (end < path) {
		/* No slash found, therefore return '.' */
#ifdef NETWARE
		if (len_adjust == 0) {
			path[0] = '.';
			path[1] = '\0';
			return 1; /* only one character */
		} else {
			path[0] = '\0';
			return len_adjust;
		}
#else
		path[0] = '.';
		path[1] = '\0';
		return 1 + len_adjust;
#endif
	}

	/* Strip slashes which came before the file name */
	while (end >= path && IS_SLASH_P(end)) {
		end--;
	}
	if (end < path) {
		path[0] = DEFAULT_SLASH;
		path[1] = '\0';
		return 1 + len_adjust;
	}
	*(end+1) = '\0';

	return (size_t)(end + 1 - path) + len_adjust;
}
/* }}} */

static inline zend_bool zend_string_equals_str_ci(zend_string *str1, zend_string *str2) /* {{{ */
{
	return str1->len == str2->len
		&& !zend_binary_strcasecmp(str1->val, str1->len, str2->val, str2->len);
}
/* }}} */

static void zend_adjust_for_fetch_type(zend_op *opline, uint32_t type) /* {{{ */ 
{
	switch (type & BP_VAR_MASK) {
		case BP_VAR_R:
			return;
		case BP_VAR_W:
			opline->opcode += 3;
			return;
		case BP_VAR_REF:
			opline->opcode += 3;
			opline->extended_value |= ZEND_FETCH_MAKE_REF;
			return;
		case BP_VAR_RW:
			opline->opcode += 6;
			return;
		case BP_VAR_IS:
			opline->opcode += 9;
			return;
		case BP_VAR_FUNC_ARG:
			opline->opcode += 12;
			opline->extended_value |= type >> BP_VAR_SHIFT;
			return;
		case BP_VAR_UNSET:
			opline->opcode += 15;
			return;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

static inline void zend_make_var_result(znode *result, zend_op *opline TSRMLS_DC) /* {{{ */ 
{
	opline->result_type = IS_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	GET_NODE(result, opline->result);
}
/* }}} */

static inline void zend_make_tmp_result(znode *result, zend_op *opline TSRMLS_DC) /* {{{ */
{
	opline->result_type = IS_TMP_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	GET_NODE(result, opline->result);
}
/* }}} */

static zend_op *zend_emit_op(znode *result, zend_uchar opcode, znode *op1, znode *op2 TSRMLS_DC) /* {{{ */ 
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = opcode;

	if (op1 == NULL) {
		SET_UNUSED(opline->op1);
	} else {
		SET_NODE(opline->op1, op1);
	}

	if (op2 == NULL) {
		SET_UNUSED(opline->op2);
	} else {
		SET_NODE(opline->op2, op2);
	}

	if (result) {
		zend_make_var_result(result, opline TSRMLS_CC);
	}
	return opline;
}
/* }}} */

static zend_op *zend_emit_op_tmp(znode *result, zend_uchar opcode, znode *op1, znode *op2 TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = opcode;

	if (op1 == NULL) {
		SET_UNUSED(opline->op1);
	} else {
		SET_NODE(opline->op1, op1);
	}

	if (op2 == NULL) {
		SET_UNUSED(opline->op2);
	} else {
		SET_NODE(opline->op2, op2);
	}

	if (result) {
		zend_make_tmp_result(result, opline TSRMLS_CC);
	}

	return opline;
}
/* }}} */

static void zend_emit_tick(TSRMLS_D) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_TICKS;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
	opline->extended_value = Z_LVAL(CG(declarables).ticks);
}
/* }}} */

static inline zend_op *zend_emit_op_data(znode *value TSRMLS_DC) /* {{{ */
{
	return zend_emit_op(NULL, ZEND_OP_DATA, value, NULL TSRMLS_CC);
}
/* }}} */

static inline uint32_t zend_emit_jump(uint32_t opnum_target TSRMLS_DC) /* {{{ */
{
	uint32_t opnum = get_next_op_number(CG(active_op_array));
	zend_op *opline = zend_emit_op(NULL, ZEND_JMP, NULL, NULL TSRMLS_CC);
	opline->op1.opline_num = opnum_target;
	return opnum;
}
/* }}} */

static inline uint32_t zend_emit_cond_jump(zend_uchar opcode, znode *cond, uint32_t opnum_target TSRMLS_DC) /* {{{ */
{
	uint32_t opnum = get_next_op_number(CG(active_op_array));
	zend_op *opline = zend_emit_op(NULL, opcode, cond, NULL TSRMLS_CC);
	opline->op2.opline_num = opnum_target;
	return opnum;
}
/* }}} */

static inline void zend_update_jump_target(uint32_t opnum_jump, uint32_t opnum_target TSRMLS_DC) /* {{{ */
{
	zend_op *opline = &CG(active_op_array)->opcodes[opnum_jump];
	switch (opline->opcode) {
		case ZEND_JMP:
			opline->op1.opline_num = opnum_target;
			break;
		case ZEND_JMPZ:
		case ZEND_JMPNZ:
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
			opline->op2.opline_num = opnum_target;
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

static inline void zend_update_jump_target_to_next(uint32_t opnum_jump TSRMLS_DC) /* {{{ */ 
{
	zend_update_jump_target(opnum_jump, get_next_op_number(CG(active_op_array)) TSRMLS_CC);
}
/* }}} */

static inline zend_op *zend_delayed_emit_op(znode *result, zend_uchar opcode, znode *op1, znode *op2 TSRMLS_DC) /* {{{ */
{
	zend_op tmp_opline;
	init_op(&tmp_opline TSRMLS_CC);
	tmp_opline.opcode = opcode;
	SET_NODE(tmp_opline.op1, op1);
	SET_NODE(tmp_opline.op2, op2);
	if (result) {
		zend_make_var_result(result, &tmp_opline TSRMLS_CC);
	}

	zend_stack_push(&CG(delayed_oplines_stack), &tmp_opline);
	return zend_stack_top(&CG(delayed_oplines_stack));
}
/* }}} */

static inline uint32_t zend_delayed_compile_begin(TSRMLS_D) /* {{{ */
{
	return zend_stack_count(&CG(delayed_oplines_stack));
}
/* }}} */

static zend_op *zend_delayed_compile_end(uint32_t offset TSRMLS_DC) /* {{{ */
{
	zend_op *opline, *oplines = zend_stack_base(&CG(delayed_oplines_stack));
	uint32_t i, count = zend_stack_count(&CG(delayed_oplines_stack));

	ZEND_ASSERT(count > offset);
	for (i = offset; i < count; ++i) {
		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		memcpy(opline, &oplines[i], sizeof(zend_op));
	}
	CG(delayed_oplines_stack).top = offset;
	return opline;
}
/* }}} */

void zend_emit_final_return(zval *zv TSRMLS_DC) /* {{{ */
{
	znode zn;
	zend_bool returns_reference = (CG(active_op_array)->fn_flags & ZEND_ACC_RETURN_REFERENCE) != 0;

	zn.op_type = IS_CONST;
	if (zv) {
		ZVAL_COPY_VALUE(&zn.u.constant, zv);
	} else {
		ZVAL_NULL(&zn.u.constant);
	}

	zend_emit_op(NULL, returns_reference ? ZEND_RETURN_BY_REF : ZEND_RETURN, &zn, NULL TSRMLS_CC);
}
/* }}} */

static inline zend_bool zend_is_variable(zend_ast *ast) /* {{{ */
{
	return ast->kind == ZEND_AST_VAR || ast->kind == ZEND_AST_DIM
		|| ast->kind == ZEND_AST_PROP || ast->kind == ZEND_AST_STATIC_PROP
		|| ast->kind == ZEND_AST_CALL || ast->kind == ZEND_AST_METHOD_CALL
		|| ast->kind == ZEND_AST_STATIC_CALL;
}
/* }}} */

static inline zend_bool zend_is_call(zend_ast *ast) /* {{{ */
{
	return ast->kind == ZEND_AST_CALL
		|| ast->kind == ZEND_AST_METHOD_CALL
		|| ast->kind == ZEND_AST_STATIC_CALL;
}
/* }}} */

static inline zend_bool zend_is_unticked_stmt(zend_ast *ast) /* {{{ */
{
	return ast->kind == ZEND_AST_STMT_LIST || ast->kind == ZEND_AST_LABEL;
}
/* }}} */

static inline zend_bool zend_can_write_to_variable(zend_ast *ast) /* {{{ */
{
	while (ast->kind == ZEND_AST_DIM || ast->kind == ZEND_AST_PROP) {
		ast = ast->child[0];
	}

	return zend_is_variable(ast);
}
/* }}} */

static inline zend_bool zend_is_const_default_class_ref(zend_ast *name_ast) /* {{{ */
{
	zend_string *name;

	if (name_ast->kind != ZEND_AST_ZVAL) {
		return 0;
	}

	/* Fully qualified names are always default refs */
	if (!name_ast->attr) {
		return 1;
	}

	name = zend_ast_get_str(name_ast);
	return ZEND_FETCH_CLASS_DEFAULT == zend_get_class_fetch_type(name);
}
/* }}} */

static inline void zend_handle_numeric_op(znode *node TSRMLS_DC) /* {{{ */
{
	if (node->op_type == IS_CONST && Z_TYPE(node->u.constant) == IS_STRING) {
		zend_ulong index;

		if (ZEND_HANDLE_NUMERIC(Z_STR(node->u.constant), index)) {
			zval_ptr_dtor(&node->u.constant);
			ZVAL_LONG(&node->u.constant, index);
		}
	}
}
/* }}} */

static inline void zend_set_class_name_op1(zend_op *opline, znode *class_node TSRMLS_DC) /* {{{ */
{
	if (class_node->op_type == IS_CONST) {
		opline->op1_type = IS_CONST;
		opline->op1.constant = zend_add_class_name_literal(
			CG(active_op_array), Z_STR(class_node->u.constant) TSRMLS_CC);
	} else {
		SET_NODE(opline->op1, class_node);
	}
}
/* }}} */

static zend_op *zend_compile_class_ref(znode *result, zend_ast *name_ast TSRMLS_DC) /* {{{ */
{
	zend_op *opline;
	znode name_node;
	zend_compile_expr(&name_node, name_ast TSRMLS_CC);

	if (name_node.op_type == IS_CONST) {
		zend_string *name = Z_STR(name_node.u.constant);
		uint32_t fetch_type = zend_get_class_fetch_type(name);

		opline = zend_emit_op(result, ZEND_FETCH_CLASS, NULL, NULL TSRMLS_CC);
		opline->extended_value = fetch_type;

		if (fetch_type == ZEND_FETCH_CLASS_DEFAULT) {
			uint32_t type = name_ast->kind == ZEND_AST_ZVAL ? name_ast->attr : ZEND_NAME_FQ;
			opline->op2_type = IS_CONST;
			opline->op2.constant = zend_add_class_name_literal(CG(active_op_array),
				zend_resolve_class_name(name, type TSRMLS_CC) TSRMLS_CC);
		}

		zend_string_release(name);
	} else {
		opline = zend_emit_op(result, ZEND_FETCH_CLASS, NULL, &name_node TSRMLS_CC);
		opline->extended_value = ZEND_FETCH_CLASS_DEFAULT;
	}

	return opline;
}
/* }}} */

static int zend_try_compile_cv(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */ 
{
	zend_ast *name_ast = ast->child[0];
	if (name_ast->kind == ZEND_AST_ZVAL) {
		zend_string *name = zval_get_string(zend_ast_get_zval(name_ast));

		if (zend_is_auto_global(name TSRMLS_CC)) {
			zend_string_release(name);
			return FAILURE;
		}

		result->op_type = IS_CV;
		result->u.op.var = lookup_cv(CG(active_op_array), name TSRMLS_CC);

		if (zend_string_equals_literal(name, "this")) {
			CG(active_op_array)->this_var = result->u.op.var;
		}
		return SUCCESS;
	}

	return FAILURE;
}
/* }}} */

static zend_op *zend_compile_simple_var_no_cv(znode *result, zend_ast *ast, uint32_t type TSRMLS_DC) /* {{{ */
{
	zend_ast *name_ast = ast->child[0];
	znode name_node;
	zend_op *opline;

	/* there is a chance someone is accessing $this */
	if (ast->kind != ZEND_AST_ZVAL
		&& CG(active_op_array)->scope && CG(active_op_array)->this_var == -1
	) {
		zend_string *key = zend_string_init("this", sizeof("this") - 1, 0);
		CG(active_op_array)->this_var = lookup_cv(CG(active_op_array), key TSRMLS_CC);
	}

	zend_compile_expr(&name_node, name_ast TSRMLS_CC);

	opline = zend_emit_op(result, ZEND_FETCH_R, &name_node, NULL TSRMLS_CC);

	opline->extended_value = ZEND_FETCH_LOCAL;
	if (name_node.op_type == IS_CONST) {
		if (zend_is_auto_global(Z_STR(name_node.u.constant) TSRMLS_CC)) {
			opline->extended_value = ZEND_FETCH_GLOBAL;
		}
	}

	return opline;
}
/* }}} */

static void zend_compile_simple_var(znode *result, zend_ast *ast, uint32_t type TSRMLS_DC) /* {{{ */
{
	if (zend_try_compile_cv(result, ast TSRMLS_CC) == FAILURE) {
		zend_op *opline = zend_compile_simple_var_no_cv(result, ast, type TSRMLS_CC);
		zend_adjust_for_fetch_type(opline, type);
	}
}
/* }}} */

static void zend_separate_if_call_and_write(znode *node, zend_ast *ast, uint32_t type TSRMLS_DC) /* {{{ */
{
	if (type != BP_VAR_R && type != BP_VAR_IS && zend_is_call(ast)) {
		if (node->op_type == IS_VAR) {
			zend_op *opline = zend_emit_op(NULL, ZEND_SEPARATE, node, NULL TSRMLS_CC);
			opline->result_type = IS_VAR;
			opline->result.var = opline->op1.var;
		} else {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use result of built-in function in write context");
		}
	}
}
/* }}} */

void zend_delayed_compile_var(znode *result, zend_ast *ast, uint32_t type TSRMLS_DC);
void zend_compile_assign(znode *result, zend_ast *ast TSRMLS_DC);
static void zend_compile_list_assign(znode *result, zend_ast *ast, znode *expr_node TSRMLS_DC);

static inline void zend_emit_assign_znode(zend_ast *var_ast, znode *value_node TSRMLS_DC) /* {{{ */
{
	znode dummy_node;
	if (var_ast->kind == ZEND_AST_LIST) {
		zend_compile_list_assign(&dummy_node, var_ast, value_node TSRMLS_CC);
	} else {
		zend_ast *assign_ast = zend_ast_create(ZEND_AST_ASSIGN, var_ast,
			zend_ast_create_znode(value_node));
		zend_compile_assign(&dummy_node, assign_ast TSRMLS_CC);
	}
	zend_do_free(&dummy_node TSRMLS_CC);
}
/* }}} */

static zend_op *zend_delayed_compile_dim(znode *result, zend_ast *ast, uint32_t type TSRMLS_DC) /* {{{ */
{
	zend_ast *var_ast = ast->child[0];
	zend_ast *dim_ast = ast->child[1];

	znode var_node, dim_node;

	zend_delayed_compile_var(&var_node, var_ast, type TSRMLS_CC);
	zend_separate_if_call_and_write(&var_node, var_ast, type TSRMLS_CC);

	if (dim_ast == NULL) {
		if (type == BP_VAR_R || type == BP_VAR_IS) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use [] for reading");
		}
		if (type == BP_VAR_UNSET) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use [] for unsetting");
		}
		dim_node.op_type = IS_UNUSED;
	} else {
		zend_compile_expr(&dim_node, dim_ast TSRMLS_CC);
		zend_handle_numeric_op(&dim_node TSRMLS_CC);
	}

	return zend_delayed_emit_op(result, ZEND_FETCH_DIM_R, &var_node, &dim_node TSRMLS_CC);
}
/* }}} */

static inline zend_op *zend_compile_dim_common(znode *result, zend_ast *ast, uint32_t type TSRMLS_DC) /* {{{ */
{
	uint32_t offset = zend_delayed_compile_begin(TSRMLS_C);
	zend_delayed_compile_dim(result, ast, type TSRMLS_CC);
	return zend_delayed_compile_end(offset TSRMLS_CC);
}
/* }}} */

void zend_compile_dim(znode *result, zend_ast *ast, uint32_t type TSRMLS_DC) /* {{{ */
{
	zend_op *opline = zend_compile_dim_common(result, ast, type TSRMLS_CC);
	zend_adjust_for_fetch_type(opline, type);
}
/* }}} */

static zend_bool is_this_fetch(zend_ast *ast) /* {{{ */
{
	if (ast->kind == ZEND_AST_VAR && ast->child[0]->kind == ZEND_AST_ZVAL) {
		zval *name = zend_ast_get_zval(ast->child[0]);
		return Z_TYPE_P(name) == IS_STRING && zend_string_equals_literal(Z_STR_P(name), "this");
	}

	return 0;
}
/* }}} */

static zend_op *zend_delayed_compile_prop(znode *result, zend_ast *ast, uint32_t type TSRMLS_DC) /* {{{ */
{
	zend_ast *obj_ast = ast->child[0];
	zend_ast *prop_ast = ast->child[1];

	znode obj_node, prop_node;
	zend_op *opline;

	if (is_this_fetch(obj_ast)) {
		obj_node.op_type = IS_UNUSED;
	} else {
		zend_delayed_compile_var(&obj_node, obj_ast, type TSRMLS_CC);
		zend_separate_if_call_and_write(&obj_node, obj_ast, type TSRMLS_CC);
	}
	zend_compile_expr(&prop_node, prop_ast TSRMLS_CC);

	opline = zend_delayed_emit_op(result, ZEND_FETCH_OBJ_R, &obj_node, &prop_node TSRMLS_CC);
	if (opline->op2_type == IS_CONST && Z_TYPE(CONSTANT(opline->op2.constant)) == IS_STRING) {
		zend_alloc_polymorphic_cache_slot(opline->op2.constant TSRMLS_CC);
	}

	return opline;
}
/* }}} */

static zend_op *zend_compile_prop_common(znode *result, zend_ast *ast, uint32_t type TSRMLS_DC) /* {{{ */
{
	uint32_t offset = zend_delayed_compile_begin(TSRMLS_C);
	zend_delayed_compile_prop(result, ast, type TSRMLS_CC);
	return zend_delayed_compile_end(offset TSRMLS_CC);
}
/* }}} */

void zend_compile_prop(znode *result, zend_ast *ast, uint32_t type TSRMLS_DC) /* {{{ */
{
	zend_op *opline = zend_compile_prop_common(result, ast, type TSRMLS_CC);
	zend_adjust_for_fetch_type(opline, type);
}
/* }}} */

zend_op *zend_compile_static_prop_common(znode *result, zend_ast *ast, uint32_t type TSRMLS_DC) /* {{{ */
{
	zend_ast *class_ast = ast->child[0];
	zend_ast *prop_ast = ast->child[1];

	znode class_node, prop_node;
	zend_op *opline;

	if (zend_is_const_default_class_ref(class_ast)) {
		class_node.op_type = IS_CONST;
		ZVAL_STR(&class_node.u.constant, zend_resolve_class_name_ast(class_ast TSRMLS_CC));
	} else {
		zend_compile_class_ref(&class_node, class_ast TSRMLS_CC);
	}

	zend_compile_expr(&prop_node, prop_ast TSRMLS_CC);

	opline = zend_emit_op(result, ZEND_FETCH_R, &prop_node, NULL TSRMLS_CC);
	if (opline->op1_type == IS_CONST) {
		zend_alloc_polymorphic_cache_slot(opline->op1.constant TSRMLS_CC);
	}
	if (class_node.op_type == IS_CONST) {
		opline->op2_type = IS_CONST;
		opline->op2.constant = zend_add_class_name_literal(
			CG(active_op_array), Z_STR(class_node.u.constant) TSRMLS_CC);
	} else {
		SET_NODE(opline->op2, &class_node);
	}
	opline->extended_value |= ZEND_FETCH_STATIC_MEMBER;

	return opline;
}
/* }}} */

void zend_compile_static_prop(znode *result, zend_ast *ast, uint32_t type TSRMLS_DC) /* {{{ */
{
	zend_op *opline = zend_compile_static_prop_common(result, ast, type TSRMLS_CC);
	zend_adjust_for_fetch_type(opline, type);
}
/* }}} */

static inline zend_uchar get_list_fetch_opcode(zend_uchar op_type) /* {{{ */
{
	switch (op_type) {
		case IS_VAR:
		case IS_CV:
			return ZEND_FETCH_DIM_R;
		case IS_TMP_VAR:
		case IS_CONST:
			return ZEND_FETCH_DIM_TMP_VAR;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

static void zend_compile_list_assign(znode *result, zend_ast *ast, znode *expr_node TSRMLS_DC) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;

	if (list->children == 1 && !list->child[0]) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use empty list");
	}

	for (i = 0; i < list->children; ++i) {
		zend_ast *var_ast = list->child[i];
		znode fetch_result, dim_node;
		zend_op *opline;

		if (var_ast == NULL) {
			continue;
		}

		dim_node.op_type = IS_CONST;
		ZVAL_LONG(&dim_node.u.constant, i);

		if (expr_node->op_type == IS_CONST) {
			Z_TRY_ADDREF(expr_node->u.constant);
		}

		opline = zend_emit_op(&fetch_result,
			get_list_fetch_opcode(expr_node->op_type), expr_node, &dim_node TSRMLS_CC);
		opline->extended_value |= ZEND_FETCH_ADD_LOCK;

		zend_emit_assign_znode(var_ast, &fetch_result TSRMLS_CC);
	}
	*result = *expr_node;
}
/* }}} */

void zend_ensure_writable_variable(const zend_ast *ast) /* {{{ */
{
	if (ast->kind == ZEND_AST_CALL) {
		zend_error_noreturn(E_COMPILE_ERROR, "Can't use function return value in write context");
	}
	if (ast->kind == ZEND_AST_METHOD_CALL || ast->kind == ZEND_AST_STATIC_CALL) {
		zend_error_noreturn(E_COMPILE_ERROR, "Can't use method return value in write context");
	}
}
/* }}} */

/* Detects $a... = $a pattern */
zend_bool zend_is_assign_to_self(zend_ast *var_ast, zend_ast *expr_ast TSRMLS_DC) /* {{{ */
{
	if (expr_ast->kind != ZEND_AST_VAR || expr_ast->child[0]->kind != ZEND_AST_ZVAL) {
		return 0;
	}

	while (zend_is_variable(var_ast) && var_ast->kind != ZEND_AST_VAR) {
		var_ast = var_ast->child[0];
	}

	if (var_ast->kind != ZEND_AST_VAR || var_ast->child[0]->kind != ZEND_AST_ZVAL) {
		return 0;
	}

	{
		zend_string *name1 = zval_get_string(zend_ast_get_zval(var_ast->child[0]));
		zend_string *name2 = zval_get_string(zend_ast_get_zval(expr_ast->child[0]));
		zend_bool result = zend_string_equals(name1, name2);
		zend_string_release(name1);
		zend_string_release(name2);
		return result;
	}
}
/* }}} */

void zend_compile_assign(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *var_ast = ast->child[0];
	zend_ast *expr_ast = ast->child[1];

	znode var_node, expr_node;
	zend_op *opline;
	uint32_t offset;

	if (is_this_fetch(var_ast)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot re-assign $this");
	}

	zend_ensure_writable_variable(var_ast);

	switch (var_ast->kind) {
		case ZEND_AST_VAR:
		case ZEND_AST_STATIC_PROP:
			zend_compile_var(&var_node, var_ast, BP_VAR_W TSRMLS_CC);
			zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);
			zend_emit_op(result, ZEND_ASSIGN, &var_node, &expr_node TSRMLS_CC);
			return;
		case ZEND_AST_DIM:
			offset = zend_delayed_compile_begin(TSRMLS_C);
			zend_delayed_compile_dim(result, var_ast, BP_VAR_W TSRMLS_CC);

			if (zend_is_assign_to_self(var_ast, expr_ast TSRMLS_CC)) {
				/* $a[0] = $a should evaluate the right $a first */
				zend_compile_simple_var_no_cv(&expr_node, expr_ast, BP_VAR_R TSRMLS_CC);
			} else {
				zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);
			}

			opline = zend_delayed_compile_end(offset TSRMLS_CC);
			opline->opcode = ZEND_ASSIGN_DIM;

			opline = zend_emit_op_data(&expr_node TSRMLS_CC);
			opline->op2.var = get_temporary_variable(CG(active_op_array));
			opline->op2_type = IS_VAR;
			return;
		case ZEND_AST_PROP:
			offset = zend_delayed_compile_begin(TSRMLS_C);
			zend_delayed_compile_prop(result, var_ast, BP_VAR_W TSRMLS_CC);
			zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);

			opline = zend_delayed_compile_end(offset TSRMLS_CC);
			opline->opcode = ZEND_ASSIGN_OBJ;

			zend_emit_op_data(&expr_node TSRMLS_CC);
			return;
		case ZEND_AST_LIST:
			zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);
			zend_compile_list_assign(result, var_ast, &expr_node TSRMLS_CC);
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}
/* }}} */

void zend_compile_assign_ref(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *target_ast = ast->child[0];
	zend_ast *source_ast = ast->child[1];

	znode target_node, source_node;
	zend_op *opline;

	if (is_this_fetch(target_ast)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot re-assign $this");
	}
	zend_ensure_writable_variable(target_ast);

	zend_compile_var(&target_node, target_ast, BP_VAR_W TSRMLS_CC);
	zend_compile_var(&source_node, source_ast, BP_VAR_REF TSRMLS_CC);

	if (source_node.op_type != IS_VAR && zend_is_call(source_ast)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use result of built-in function in write context");
	}

	opline = zend_emit_op(result, ZEND_ASSIGN_REF, &target_node, &source_node TSRMLS_CC);
	if (!result) {
		opline->result_type |= EXT_TYPE_UNUSED;
	}

	if (zend_is_call(source_ast)) {
		opline->extended_value = ZEND_RETURNS_FUNCTION;
	} else if (source_ast->kind == ZEND_AST_NEW) {
		zend_error(E_DEPRECATED, "Assigning the return value of new by reference is deprecated");
		opline->extended_value = ZEND_RETURNS_NEW;
	}
}
/* }}} */

static inline void zend_emit_assign_ref_znode(zend_ast *var_ast, znode *value_node TSRMLS_DC) /* {{{ */
{
	zend_ast *assign_ast = zend_ast_create(ZEND_AST_ASSIGN_REF, var_ast,
		zend_ast_create_znode(value_node));
	zend_compile_assign_ref(NULL, assign_ast TSRMLS_CC);
}
/* }}} */

void zend_compile_compound_assign(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *var_ast = ast->child[0];
	zend_ast *expr_ast = ast->child[1];
	uint32_t opcode = ast->attr;

	znode var_node, expr_node;
	zend_op *opline;
	uint32_t offset;

	zend_ensure_writable_variable(var_ast);

	switch (var_ast->kind) {
		case ZEND_AST_VAR:
		case ZEND_AST_STATIC_PROP:
			zend_compile_var(&var_node, var_ast, BP_VAR_RW TSRMLS_CC);
			zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);
			zend_emit_op(result, opcode, &var_node, &expr_node TSRMLS_CC);
			return;
		case ZEND_AST_DIM:
			offset = zend_delayed_compile_begin(TSRMLS_C);
			zend_delayed_compile_dim(result, var_ast, BP_VAR_RW TSRMLS_CC);
			zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);

			opline = zend_delayed_compile_end(offset TSRMLS_CC);
			opline->opcode = opcode;
			opline->extended_value = ZEND_ASSIGN_DIM;

			opline = zend_emit_op_data(&expr_node TSRMLS_CC);
			opline->op2.var = get_temporary_variable(CG(active_op_array));
			opline->op2_type = IS_VAR;
			return;
		case ZEND_AST_PROP:
			offset = zend_delayed_compile_begin(TSRMLS_C);
			zend_delayed_compile_prop(result, var_ast, BP_VAR_RW TSRMLS_CC);
			zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);

			opline = zend_delayed_compile_end(offset TSRMLS_CC);
			opline->opcode = opcode;
			opline->extended_value = ZEND_ASSIGN_OBJ;

			zend_emit_op_data(&expr_node TSRMLS_CC);
			return;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

uint32_t zend_compile_args(zend_ast *ast, zend_function *fbc TSRMLS_DC) /* {{{ */
{
	/* TODO.AST &var error */
	zend_ast_list *args = zend_ast_get_list(ast);
	uint32_t i;
	zend_bool uses_arg_unpack = 0;
	uint32_t arg_count = 0; /* number of arguments not including unpacks */

	for (i = 0; i < args->children; ++i) {
		zend_ast *arg = args->child[i];
		uint32_t arg_num = i + 1;

		znode arg_node;
		zend_op *opline;
		zend_uchar opcode;
		zend_ulong flags = 0;

		if (arg->kind == ZEND_AST_UNPACK) {
			uses_arg_unpack = 1;
			fbc = NULL;
			
			zend_compile_expr(&arg_node, arg->child[0] TSRMLS_CC);
			opline = zend_emit_op(NULL, ZEND_SEND_UNPACK, &arg_node, NULL TSRMLS_CC);
			opline->op2.num = arg_count;
			continue;
		}

		if (uses_arg_unpack) {
			zend_error_noreturn(E_COMPILE_ERROR,
				"Cannot use positional argument after argument unpacking");
		}

		arg_count++;
		if (zend_is_variable(arg)) {
			if (zend_is_call(arg)) {
				zend_compile_var(&arg_node, arg, BP_VAR_R TSRMLS_CC);
				if (arg_node.op_type & (IS_CONST|IS_TMP_VAR)) {
					/* Function call was converted into builtin instruction */
					opcode = ZEND_SEND_VAL;
				} else {
					opcode = ZEND_SEND_VAR_NO_REF;
					flags |= ZEND_ARG_SEND_FUNCTION;
					if (fbc && ARG_SHOULD_BE_SENT_BY_REF(fbc, arg_num)) {
						flags |= ZEND_ARG_SEND_BY_REF;
						if (ARG_MAY_BE_SENT_BY_REF(fbc, arg_num)) {
							flags |= ZEND_ARG_SEND_SILENT;
						}
					}
				}
			} else if (fbc) {
				if (ARG_SHOULD_BE_SENT_BY_REF(fbc, arg_num)) {
					zend_compile_var(&arg_node, arg, BP_VAR_W TSRMLS_CC);
					opcode = ZEND_SEND_REF;
				} else {
					zend_compile_var(&arg_node, arg, BP_VAR_R TSRMLS_CC);
					opcode = ZEND_SEND_VAR;
				}
			} else {
				zend_compile_var(&arg_node, arg,
					BP_VAR_FUNC_ARG | (arg_num << BP_VAR_SHIFT) TSRMLS_CC);
				opcode = ZEND_SEND_VAR_EX;
			}
		} else {
			zend_compile_expr(&arg_node, arg TSRMLS_CC);
			if (arg_node.op_type & (IS_VAR|IS_CV)) {
				opcode = ZEND_SEND_VAR_NO_REF;
				if (fbc && ARG_MUST_BE_SENT_BY_REF(fbc, arg_num)) {
					flags |= ZEND_ARG_SEND_BY_REF;
				}
			} else {
				if (fbc) {
					opcode = ZEND_SEND_VAL;
					if (ARG_MUST_BE_SENT_BY_REF(fbc, arg_num)) {
						zend_error_noreturn(E_COMPILE_ERROR, "Only variables can be passed by reference");
					}
				} else {
					opcode = ZEND_SEND_VAL_EX;
				}
			}
		}

		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = opcode;
		SET_NODE(opline->op1, &arg_node);
		SET_UNUSED(opline->op2);
		opline->op2.opline_num = arg_num;

		if (opcode == ZEND_SEND_VAR_NO_REF) {
			if (fbc) {
				flags |= ZEND_ARG_COMPILE_TIME_BOUND;
			}
			opline->extended_value = flags;
		} else if (fbc) {
			opline->extended_value = ZEND_ARG_COMPILE_TIME_BOUND;
		}
	}

	return arg_count;
}
/* }}} */

void zend_compile_call_common(znode *result, zend_ast *args_ast, zend_function *fbc TSRMLS_DC) /* {{{ */
{
	zend_op *opline;
	uint32_t opnum_init = get_next_op_number(CG(active_op_array)) - 1;
	uint32_t arg_count;
	uint32_t call_flags;

	zend_do_extended_fcall_begin(TSRMLS_C);

	arg_count = zend_compile_args(args_ast, fbc TSRMLS_CC);

	opline = &CG(active_op_array)->opcodes[opnum_init];
	opline->extended_value = arg_count;

	call_flags = (opline->opcode == ZEND_NEW ? ZEND_CALL_CTOR : 0);
	opline = zend_emit_op(result, ZEND_DO_FCALL, NULL, NULL TSRMLS_CC);
	opline->op1.num = call_flags;

	zend_do_extended_fcall_end(TSRMLS_C);
}
/* }}} */

zend_bool zend_compile_function_name(znode *name_node, zend_ast *name_ast TSRMLS_DC) /* {{{ */
{
	zend_string *orig_name = zend_ast_get_str(name_ast);
	zend_bool is_fully_qualified;

	name_node->op_type = IS_CONST;
	ZVAL_STR(&name_node->u.constant, zend_resolve_function_name(
		orig_name, name_ast->attr, &is_fully_qualified TSRMLS_CC));

	return !is_fully_qualified && CG(current_namespace);
}
/* }}} */

void zend_compile_ns_call(znode *result, znode *name_node, zend_ast *args_ast TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_INIT_NS_FCALL_BY_NAME;
	SET_UNUSED(opline->op1);
	opline->op2_type = IS_CONST;
	opline->op2.constant = zend_add_ns_func_name_literal(
		CG(active_op_array), Z_STR(name_node->u.constant) TSRMLS_CC);
	zend_alloc_cache_slot(opline->op2.constant TSRMLS_CC);

	zend_compile_call_common(result, args_ast, NULL TSRMLS_CC);
}
/* }}} */

void zend_compile_dynamic_call(znode *result, znode *name_node, zend_ast *args_ast TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_INIT_FCALL_BY_NAME;
	SET_UNUSED(opline->op1);
	if (name_node->op_type == IS_CONST && Z_TYPE(name_node->u.constant) == IS_STRING) {
		opline->op2_type = IS_CONST;
		opline->op2.constant = zend_add_func_name_literal(CG(active_op_array),
			Z_STR(name_node->u.constant) TSRMLS_CC);
		zend_alloc_cache_slot(opline->op2.constant TSRMLS_CC);
	} else {
		SET_NODE(opline->op2, name_node);
	}

	zend_compile_call_common(result, args_ast, NULL TSRMLS_CC);
}
/* }}} */

static zend_bool zend_args_contain_unpack(zend_ast_list *args) /* {{{ */
{
	uint32_t i;
	for (i = 0; i < args->children; ++i) {
		if (args->child[i]->kind == ZEND_AST_UNPACK) {
			return 1;
		}
	}
	return 0;
}
/* }}} */

int zend_compile_func_strlen(znode *result, zend_ast_list *args TSRMLS_DC) /* {{{ */
{
	znode arg_node;

	if ((CG(compiler_options) & ZEND_COMPILE_NO_BUILTIN_STRLEN)
		|| args->children != 1 || args->child[0]->kind == ZEND_AST_UNPACK
	) {
		return FAILURE;
	}

	zend_compile_expr(&arg_node, args->child[0] TSRMLS_CC);
	zend_emit_op_tmp(result, ZEND_STRLEN, &arg_node, NULL TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

int zend_compile_func_typecheck(znode *result, zend_ast_list *args, uint32_t type TSRMLS_DC) /* {{{ */
{
	znode arg_node;
	zend_op *opline;

	if (args->children != 1 || args->child[0]->kind == ZEND_AST_UNPACK) {
		return FAILURE;
	}
	
	zend_compile_expr(&arg_node, args->child[0] TSRMLS_CC);
	opline = zend_emit_op_tmp(result, ZEND_TYPE_CHECK, &arg_node, NULL TSRMLS_CC);
	opline->extended_value = type;
	return SUCCESS;
}
/* }}} */

int zend_compile_func_defined(znode *result, zend_ast_list *args TSRMLS_DC) /* {{{ */
{
	zend_string *name;
	zend_op *opline;

	if (args->children != 1 || args->child[0]->kind != ZEND_AST_ZVAL) {
		return FAILURE;
	}

	name = zval_get_string(zend_ast_get_zval(args->child[0]));
	if (zend_memrchr(name->val, '\\', name->len) || zend_memrchr(name->val, ':', name->len)) {
		zend_string_release(name);
		return FAILURE;
	}

	opline = zend_emit_op_tmp(result, ZEND_DEFINED, NULL, NULL TSRMLS_CC);
	opline->op1_type = IS_CONST;
	LITERAL_STR(opline->op1, name);
	zend_alloc_cache_slot(opline->op1.constant TSRMLS_CC);

	/* Lowercase constant name in a separate literal */
	{
		zval c;
		zend_string *lcname = zend_string_alloc(name->len, 0);
		zend_str_tolower_copy(lcname->val, name->val, name->len);
		ZVAL_NEW_STR(&c, lcname);
		zend_add_literal(CG(active_op_array), &c TSRMLS_CC);
	}
	return SUCCESS;
}
/* }}} */

static int zend_try_compile_ct_bound_init_user_func(zend_ast *name_ast, uint32_t num_args TSRMLS_DC) /* {{{ */
{
	zend_string *name, *lcname;
	zend_function *fbc;
	zend_op *opline;

	if (name_ast->kind != ZEND_AST_ZVAL || Z_TYPE_P(zend_ast_get_zval(name_ast)) != IS_STRING) {
		return FAILURE;
	}

	name = zend_ast_get_str(name_ast);
	lcname = zend_string_alloc(name->len, 0);
	zend_str_tolower_copy(lcname->val, name->val, name->len);

	fbc = zend_hash_find_ptr(CG(function_table), lcname);
	if (!fbc || (fbc->type == ZEND_INTERNAL_FUNCTION &&
		(CG(compiler_options) & ZEND_COMPILE_IGNORE_INTERNAL_FUNCTIONS))
	) {
		zend_string_free(lcname);
		return FAILURE;
	}

	opline = zend_emit_op(NULL, ZEND_INIT_FCALL, NULL, NULL TSRMLS_CC);
	opline->extended_value = num_args;

	opline->op2_type = IS_CONST;
	LITERAL_STR(opline->op2, lcname);
	zend_alloc_cache_slot(opline->op2.constant TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

static void zend_compile_init_user_func(zend_ast *name_ast, uint32_t num_args, zend_string *orig_func_name TSRMLS_DC) /* {{{ */
{
	zend_op *opline;
	znode name_node;

	if (zend_try_compile_ct_bound_init_user_func(name_ast, num_args TSRMLS_CC) == SUCCESS) {
		return;
	}

	zend_compile_expr(&name_node, name_ast TSRMLS_CC);

	opline = zend_emit_op(NULL, ZEND_INIT_USER_CALL, NULL, &name_node TSRMLS_CC);
	opline->op1_type = IS_CONST;
	LITERAL_STR(opline->op1, zend_string_copy(orig_func_name));
	opline->extended_value = num_args;
}
/* }}} */

/* cufa = call_user_func_array */
int zend_compile_func_cufa(znode *result, zend_ast_list *args, zend_string *lcname TSRMLS_DC) /* {{{ */
{
	znode arg_node;

	if (args->children != 2 || zend_args_contain_unpack(args)) {
		return FAILURE;
	}

	zend_compile_init_user_func(args->child[0], 1, lcname TSRMLS_CC);
	zend_compile_expr(&arg_node, args->child[1] TSRMLS_CC);
	zend_emit_op(NULL, ZEND_SEND_ARRAY, &arg_node, NULL TSRMLS_CC);
	zend_emit_op(result, ZEND_DO_FCALL, NULL, NULL TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/* cuf = call_user_func */
int zend_compile_func_cuf(znode *result, zend_ast_list *args, zend_string *lcname TSRMLS_DC) /* {{{ */
{
	uint32_t i;

	if (args->children < 1 || zend_args_contain_unpack(args)) {
		return FAILURE;
	}

	zend_compile_init_user_func(args->child[0], args->children - 1, lcname TSRMLS_CC);
	for (i = 1; i < args->children; ++i) {
		zend_ast *arg_ast = args->child[i];
		znode arg_node;
		zend_op *opline;
		zend_bool send_user = 0;

		if (zend_is_variable(arg_ast) && !zend_is_call(arg_ast)) {
			zend_compile_var(&arg_node, arg_ast, BP_VAR_FUNC_ARG | (i << BP_VAR_SHIFT) TSRMLS_CC);
			send_user = 1;
		} else {
			zend_compile_expr(&arg_node, arg_ast TSRMLS_CC);
			if (arg_node.op_type & (IS_VAR|IS_CV)) {
				send_user = 1;
			}
		}

		if (send_user) {
			opline = zend_emit_op(NULL, ZEND_SEND_USER, &arg_node, NULL TSRMLS_CC);
		} else {
			opline = zend_emit_op(NULL, ZEND_SEND_VAL, &arg_node, NULL TSRMLS_CC);
		}

		opline->op2.opline_num = i;
	}
	zend_emit_op(result, ZEND_DO_FCALL, NULL, NULL TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

int zend_try_compile_special_func(znode *result, zend_string *lcname, zend_ast_list *args TSRMLS_DC) /* {{{ */
{
	if (zend_string_equals_literal(lcname, "strlen")) {
		return zend_compile_func_strlen(result, args TSRMLS_CC);
	} else if (zend_string_equals_literal(lcname, "is_null")) {
		return zend_compile_func_typecheck(result, args, IS_NULL TSRMLS_CC);
	} else if (zend_string_equals_literal(lcname, "is_bool")) {
		return zend_compile_func_typecheck(result, args, _IS_BOOL TSRMLS_CC);
	} else if (zend_string_equals_literal(lcname, "is_long")
		|| zend_string_equals_literal(lcname, "is_int")
		|| zend_string_equals_literal(lcname, "is_integer")
	) {
		return zend_compile_func_typecheck(result, args, IS_LONG TSRMLS_CC);
	} else if (zend_string_equals_literal(lcname, "is_float")
		|| zend_string_equals_literal(lcname, "is_double")
		|| zend_string_equals_literal(lcname, "is_real")
	) {
		return zend_compile_func_typecheck(result, args, IS_DOUBLE TSRMLS_CC);
	} else if (zend_string_equals_literal(lcname, "is_string")) {
		return zend_compile_func_typecheck(result, args, IS_STRING TSRMLS_CC);
	} else if (zend_string_equals_literal(lcname, "is_array")) {
		return zend_compile_func_typecheck(result, args, IS_ARRAY TSRMLS_CC);
	} else if (zend_string_equals_literal(lcname, "is_object")) {
		return zend_compile_func_typecheck(result, args, IS_OBJECT TSRMLS_CC);
	} else if (zend_string_equals_literal(lcname, "is_resource")) {
		return zend_compile_func_typecheck(result, args, IS_RESOURCE TSRMLS_CC);
	} else if (zend_string_equals_literal(lcname, "defined")) {
		return zend_compile_func_defined(result, args TSRMLS_CC);
	} else if (zend_string_equals_literal(lcname, "call_user_func_array")) {
		return zend_compile_func_cufa(result, args, lcname TSRMLS_CC);
	} else if (zend_string_equals_literal(lcname, "call_user_func")) {
		return zend_compile_func_cuf(result, args, lcname TSRMLS_CC);
	} else {
		return FAILURE;
	}
}
/* }}} */

void zend_compile_call(znode *result, zend_ast *ast, uint32_t type TSRMLS_DC) /* {{{ */
{
	zend_ast *name_ast = ast->child[0];
	zend_ast *args_ast = ast->child[1];

	znode name_node;

	if (name_ast->kind != ZEND_AST_ZVAL || Z_TYPE_P(zend_ast_get_zval(name_ast)) != IS_STRING) {
		zend_compile_expr(&name_node, name_ast TSRMLS_CC);
		zend_compile_dynamic_call(result, &name_node, args_ast TSRMLS_CC);
		return;
	}

	{
		zend_bool runtime_resolution = zend_compile_function_name(&name_node, name_ast TSRMLS_CC);
		if (runtime_resolution) {
			zend_compile_ns_call(result, &name_node, args_ast TSRMLS_CC);
			return;
		}
	}

	{
		zval *name = &name_node.u.constant;
		zend_string *lcname = zend_string_alloc(Z_STRLEN_P(name), 0);
		zend_function *fbc;
		zend_op *opline;

		zend_str_tolower_copy(lcname->val, Z_STRVAL_P(name), Z_STRLEN_P(name));

		fbc = zend_hash_find_ptr(CG(function_table), lcname);
		if (!fbc || (fbc->type == ZEND_INTERNAL_FUNCTION &&
			(CG(compiler_options) & ZEND_COMPILE_IGNORE_INTERNAL_FUNCTIONS))
		) {
			zend_string_release(lcname);
			zend_compile_dynamic_call(result, &name_node, args_ast TSRMLS_CC);
			return;
		}

		if (zend_try_compile_special_func(result, lcname,
				zend_ast_get_list(args_ast) TSRMLS_CC) == SUCCESS
		) {
			zend_string_release(lcname);
			zval_ptr_dtor(&name_node.u.constant);
			return;
		}

		zval_ptr_dtor(&name_node.u.constant);
		ZVAL_NEW_STR(&name_node.u.constant, lcname);

		opline = zend_emit_op(NULL, ZEND_INIT_FCALL, NULL, &name_node TSRMLS_CC);
		zend_alloc_cache_slot(opline->op2.constant TSRMLS_CC);

		zend_compile_call_common(result, args_ast, fbc TSRMLS_CC);
	}
}
/* }}} */

void zend_compile_method_call(znode *result, zend_ast *ast, uint32_t type TSRMLS_DC) /* {{{ */
{
	zend_ast *obj_ast = ast->child[0];
	zend_ast *method_ast = ast->child[1];
	zend_ast *args_ast = ast->child[2];

	znode obj_node, method_node;
	zend_op *opline;

	if (is_this_fetch(obj_ast)) {
		obj_node.op_type = IS_UNUSED;
	} else {
		zend_compile_expr(&obj_node, obj_ast TSRMLS_CC);
	}

	zend_compile_expr(&method_node, method_ast TSRMLS_CC);
	opline = zend_emit_op(NULL, ZEND_INIT_METHOD_CALL, &obj_node, NULL TSRMLS_CC);
	
	if (method_node.op_type == IS_CONST) {
		if (Z_TYPE(method_node.u.constant) != IS_STRING) {
			zend_error_noreturn(E_COMPILE_ERROR, "Method name must be a string");
		}

		opline->op2_type = IS_CONST;
		opline->op2.constant = zend_add_func_name_literal(CG(active_op_array),
			Z_STR(method_node.u.constant) TSRMLS_CC);
		zend_alloc_polymorphic_cache_slot(opline->op2.constant TSRMLS_CC);
	} else {
		SET_NODE(opline->op2, &method_node);
	}

	zend_compile_call_common(result, args_ast, NULL TSRMLS_CC);
}
/* }}} */

zend_bool zend_is_constructor(zend_string *name) /* {{{ */
{
	return zend_string_equals_literal_ci(name, ZEND_CONSTRUCTOR_FUNC_NAME);
}
/* }}} */

void zend_compile_static_call(znode *result, zend_ast *ast, uint32_t type TSRMLS_DC) /* {{{ */
{
	zend_ast *class_ast = ast->child[0];
	zend_ast *method_ast = ast->child[1];
	zend_ast *args_ast = ast->child[2];

	znode class_node, method_node;
	zend_op *opline;
	zend_ulong extended_value = 0;

	if (zend_is_const_default_class_ref(class_ast)) {
		class_node.op_type = IS_CONST;
		ZVAL_STR(&class_node.u.constant, zend_resolve_class_name_ast(class_ast TSRMLS_CC));
	} else {
		opline = zend_compile_class_ref(&class_node, class_ast TSRMLS_CC);
		extended_value = opline->extended_value;
	}

	zend_compile_expr(&method_node, method_ast TSRMLS_CC);
	if (method_node.op_type == IS_CONST) {
		zval *name = &method_node.u.constant;
		if (Z_TYPE_P(name) != IS_STRING) {
			zend_error_noreturn(E_COMPILE_ERROR, "Method name must be a string");
		}
		if (zend_is_constructor(Z_STR_P(name))) {
			zval_ptr_dtor(name);
			method_node.op_type = IS_UNUSED;
		}
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_INIT_STATIC_METHOD_CALL;
	opline->extended_value = extended_value;

	zend_set_class_name_op1(opline, &class_node TSRMLS_CC);

	if (method_node.op_type == IS_CONST) {
		opline->op2_type = IS_CONST;
		opline->op2.constant = zend_add_func_name_literal(CG(active_op_array),
			Z_STR(method_node.u.constant) TSRMLS_CC);
		if (opline->op1_type == IS_CONST) {
			zend_alloc_cache_slot(opline->op2.constant TSRMLS_CC);
		} else {
			zend_alloc_polymorphic_cache_slot(opline->op2.constant TSRMLS_CC);
		}
	} else {
		SET_NODE(opline->op2, &method_node);
	}

	zend_compile_call_common(result, args_ast, NULL TSRMLS_CC);
}
/* }}} */

void zend_compile_new(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *class_ast = ast->child[0];
	zend_ast *args_ast = ast->child[1];

	znode class_node, ctor_result;
	zend_op *opline;
	uint32_t opnum;

	zend_compile_class_ref(&class_node, class_ast TSRMLS_CC);

	opnum = get_next_op_number(CG(active_op_array));
	zend_emit_op(result, ZEND_NEW, &class_node, NULL TSRMLS_CC);

	zend_compile_call_common(&ctor_result, args_ast, NULL TSRMLS_CC);
	zend_do_free(&ctor_result TSRMLS_CC);

	/* New jumps over ctor call if ctor does not exist */
	opline = &CG(active_op_array)->opcodes[opnum];
	opline->op2.opline_num = get_next_op_number(CG(active_op_array));
}
/* }}} */

void zend_compile_clone(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *obj_ast = ast->child[0];

	znode obj_node;
	zend_compile_expr(&obj_node, obj_ast TSRMLS_CC);

	zend_emit_op(result, ZEND_CLONE, &obj_node, NULL TSRMLS_CC);
}
/* }}} */

void zend_compile_global_var(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *var_ast = ast->child[0];
	zend_ast *name_ast = var_ast->child[0];

	znode name_node, result;

	zend_compile_expr(&name_node, name_ast TSRMLS_CC);
	if (name_node.op_type == IS_CONST) {
		if (Z_TYPE(name_node.u.constant) != IS_STRING) {
			convert_to_string(&name_node.u.constant);
		}
	}

	if (zend_try_compile_cv(&result, var_ast TSRMLS_CC) == SUCCESS) {
		zend_op *opline = zend_emit_op(NULL, ZEND_BIND_GLOBAL, &result, &name_node TSRMLS_CC);
		zend_alloc_cache_slot(opline->op2.constant TSRMLS_CC);
	} else {
		zend_emit_op(&result, ZEND_FETCH_W, &name_node, NULL TSRMLS_CC);

		// TODO.AST Avoid double fetch
		//opline->extended_value = ZEND_FETCH_GLOBAL_LOCK;

		zend_emit_assign_ref_znode(var_ast, &result TSRMLS_CC);
	}
}
/* }}} */

static void zend_compile_static_var_common(zend_ast *var_ast, zval *value, zend_bool by_ref TSRMLS_DC) /* {{{ */
{
	znode var_node, result;
	zend_op *opline;

	zend_compile_expr(&var_node, var_ast TSRMLS_CC);

	if (!CG(active_op_array)->static_variables) {
		if (CG(active_op_array)->scope) {
			CG(active_op_array)->scope->ce_flags |= ZEND_HAS_STATIC_IN_METHODS;
		}
		ALLOC_HASHTABLE(CG(active_op_array)->static_variables);
		zend_hash_init(CG(active_op_array)->static_variables, 8, NULL, ZVAL_PTR_DTOR, 0);
	}

	zend_hash_update(CG(active_op_array)->static_variables, Z_STR(var_node.u.constant), value);

	opline = zend_emit_op(&result, by_ref ? ZEND_FETCH_W : ZEND_FETCH_R, &var_node, NULL TSRMLS_CC);
	opline->extended_value = ZEND_FETCH_STATIC;

	if (by_ref) {
		zend_ast *fetch_ast = zend_ast_create(ZEND_AST_VAR, var_ast);
		zend_emit_assign_ref_znode(fetch_ast, &result TSRMLS_CC);
	} else {
		zend_ast *fetch_ast = zend_ast_create(ZEND_AST_VAR, var_ast);
		zend_emit_assign_znode(fetch_ast, &result TSRMLS_CC);
	}
}
/* }}} */

void zend_compile_static_var(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *var_ast = ast->child[0];
	zend_ast *value_ast = ast->child[1];
	zval value_zv;

	if (value_ast) {
		zend_const_expr_to_zval(&value_zv, value_ast TSRMLS_CC);
	} else {
		ZVAL_NULL(&value_zv);
	}

	zend_compile_static_var_common(var_ast, &value_zv, 1 TSRMLS_CC);
}
/* }}} */

void zend_compile_unset(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *var_ast = ast->child[0];

	znode var_node;
	zend_op *opline;
	switch (var_ast->kind) {
		case ZEND_AST_VAR:
			if (zend_try_compile_cv(&var_node, var_ast TSRMLS_CC) == SUCCESS) {
				opline = zend_emit_op(NULL, ZEND_UNSET_VAR, &var_node, NULL TSRMLS_CC);
				opline->extended_value = ZEND_FETCH_LOCAL | ZEND_QUICK_SET;
			} else {
				opline = zend_compile_simple_var_no_cv(NULL, var_ast, BP_VAR_UNSET TSRMLS_CC);
				opline->opcode = ZEND_UNSET_VAR;
			}
			return;
		case ZEND_AST_DIM:
			opline = zend_compile_dim_common(NULL, var_ast, BP_VAR_UNSET TSRMLS_CC);
			opline->opcode = ZEND_UNSET_DIM;
			return;
		case ZEND_AST_PROP:
			opline = zend_compile_prop_common(NULL, var_ast, BP_VAR_UNSET TSRMLS_CC);
			opline->opcode = ZEND_UNSET_OBJ;
			return;
		case ZEND_AST_STATIC_PROP:
			opline = zend_compile_static_prop_common(NULL, var_ast, BP_VAR_UNSET TSRMLS_CC);
			opline->opcode = ZEND_UNSET_VAR;
			return;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

static void zend_free_foreach_and_switch_variables(TSRMLS_D) /* {{{ */
{
	uint32_t opnum_start, opnum_end, i;

	opnum_start = get_next_op_number(CG(active_op_array));

#ifdef ZTS
	zend_stack_apply_with_argument(&CG(loop_var_stack), ZEND_STACK_APPLY_TOPDOWN, (int (*)(void *element, void *)) generate_free_loop_var TSRMLS_CC);
#else
	zend_stack_apply(&CG(loop_var_stack), ZEND_STACK_APPLY_TOPDOWN, (int (*)(void *element)) generate_free_loop_var);
#endif

	opnum_end = get_next_op_number(CG(active_op_array));

	for (i = opnum_start; i < opnum_end; ++i) {
		CG(active_op_array)->opcodes[i].extended_value |= EXT_TYPE_FREE_ON_RETURN;
	}
}
/* }}} */

void zend_compile_return(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	zend_bool by_ref = (CG(active_op_array)->fn_flags & ZEND_ACC_RETURN_REFERENCE) != 0;

	znode expr_node;
	zend_op *opline;

	if (!expr_ast) {
		expr_node.op_type = IS_CONST;
		ZVAL_NULL(&expr_node.u.constant);
	} else if (by_ref && zend_is_variable(expr_ast) && !zend_is_call(expr_ast)) {
		zend_compile_var(&expr_node, expr_ast, BP_VAR_REF TSRMLS_CC);
	} else {
		zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);
	}

	zend_free_foreach_and_switch_variables(TSRMLS_C);

	if (CG(context).in_finally) {
		zend_emit_op(NULL, ZEND_DISCARD_EXCEPTION, NULL, NULL TSRMLS_CC);
	}

	opline = zend_emit_op(NULL, by_ref ? ZEND_RETURN_BY_REF : ZEND_RETURN,
		&expr_node, NULL TSRMLS_CC);

	if (expr_ast) {
		if (zend_is_call(expr_ast)) {
			opline->extended_value = ZEND_RETURNS_FUNCTION;
		} else if (!zend_is_variable(expr_ast)) {
			opline->extended_value = ZEND_RETURNS_VALUE;
		}
	}
}
/* }}} */

void zend_compile_echo(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];

	znode expr_node;
	zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);

	zend_emit_op(NULL, ZEND_ECHO, &expr_node, NULL TSRMLS_CC);
}
/* }}} */

void zend_compile_throw(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];

	znode expr_node;
	zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);

	zend_emit_op(NULL, ZEND_THROW, &expr_node, NULL TSRMLS_CC);
}
/* }}} */

void zend_compile_break_continue(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *depth_ast = ast->child[0];

	znode depth_node;
	zend_op *opline;

	ZEND_ASSERT(ast->kind == ZEND_AST_BREAK || ast->kind == ZEND_AST_CONTINUE);

	if (depth_ast) {
		if (depth_ast->kind != ZEND_AST_ZVAL) {
			zend_error_noreturn(E_COMPILE_ERROR, "'%s' operator with non-constant operand "
				"is no longer supported", ast->kind == ZEND_AST_BREAK ? "break" : "continue");
		}

		zend_compile_expr(&depth_node, depth_ast TSRMLS_CC);

		if (Z_TYPE(depth_node.u.constant) != IS_LONG || Z_LVAL(depth_node.u.constant) < 1) {
			zend_error_noreturn(E_COMPILE_ERROR, "'%s' operator accepts only positive numbers",
				ast->kind == ZEND_AST_BREAK ? "break" : "continue");
		}
	} else {
		depth_node.op_type = IS_CONST;
		ZVAL_LONG(&depth_node.u.constant, 1);
	}

	opline = zend_emit_op(NULL, ast->kind == ZEND_AST_BREAK ? ZEND_BRK : ZEND_CONT,
		NULL, &depth_node TSRMLS_CC);
	opline->op1.opline_num = CG(context).current_brk_cont;
}
/* }}} */

void zend_compile_goto(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *label_ast = ast->child[0];
	znode label_node;
	zend_op *opline;

	zend_compile_expr(&label_node, label_ast TSRMLS_CC);
	opline = zend_emit_op(NULL, ZEND_GOTO, NULL, &label_node TSRMLS_CC);
	opline->extended_value = CG(context).current_brk_cont;
	zend_resolve_goto_label(CG(active_op_array), opline, 0 TSRMLS_CC);
}
/* }}} */

void zend_compile_label(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_string *label = zend_ast_get_str(ast->child[0]);
	zend_label dest;

	if (!CG(context).labels) {
		ALLOC_HASHTABLE(CG(context).labels);
		zend_hash_init(CG(context).labels, 8, NULL, ptr_dtor, 0);
	}

	dest.brk_cont = CG(context).current_brk_cont;
	dest.opline_num = get_next_op_number(CG(active_op_array));

	if (!zend_hash_add_mem(CG(context).labels, label, &dest, sizeof(zend_label))) {
		zend_error_noreturn(E_COMPILE_ERROR, "Label '%s' already defined", label->val);
	}
}
/* }}} */

void zend_compile_while(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *cond_ast = ast->child[0];
	zend_ast *stmt_ast = ast->child[1];

	znode cond_node;
	uint32_t opnum_start, opnum_jmpz;

	opnum_start = get_next_op_number(CG(active_op_array));
	zend_compile_expr(&cond_node, cond_ast TSRMLS_CC);

	opnum_jmpz = zend_emit_cond_jump(ZEND_JMPZ, &cond_node, 0 TSRMLS_CC);
	zend_begin_loop(TSRMLS_C);

	zend_compile_stmt(stmt_ast TSRMLS_CC);

	zend_emit_jump(opnum_start TSRMLS_CC);

	zend_update_jump_target_to_next(opnum_jmpz TSRMLS_CC);

	zend_end_loop(opnum_start, 0 TSRMLS_CC);
}
/* }}} */

void zend_compile_do_while(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *stmt_ast = ast->child[0];
	zend_ast *cond_ast = ast->child[1];

	znode cond_node;
	uint32_t opnum_start, opnum_cond;

	zend_begin_loop(TSRMLS_C);

	opnum_start = get_next_op_number(CG(active_op_array));
	zend_compile_stmt(stmt_ast TSRMLS_CC);

	opnum_cond = get_next_op_number(CG(active_op_array));
	zend_compile_expr(&cond_node, cond_ast TSRMLS_CC);

	zend_emit_cond_jump(ZEND_JMPNZ, &cond_node, opnum_start TSRMLS_CC);

	zend_end_loop(opnum_cond, 0 TSRMLS_CC);
}
/* }}} */

void zend_compile_expr_list(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_list *list;
	uint32_t i;

	result->op_type = IS_CONST;
	ZVAL_TRUE(&result->u.constant);

	if (!ast) {
		return;
	}

	list = zend_ast_get_list(ast);
	for (i = 0; i < list->children; ++i) {
		zend_ast *expr_ast = list->child[i];

		zend_do_free(result TSRMLS_CC);
		zend_compile_expr(result, expr_ast TSRMLS_CC);
	}
}
/* }}} */

void zend_compile_for(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *init_ast = ast->child[0];
	zend_ast *cond_ast = ast->child[1];
	zend_ast *loop_ast = ast->child[2];
	zend_ast *stmt_ast = ast->child[3];

	znode result;
	uint32_t opnum_cond, opnum_jmpz, opnum_loop;

	zend_compile_expr_list(&result, init_ast TSRMLS_CC);
	zend_do_free(&result TSRMLS_CC);

	opnum_cond = get_next_op_number(CG(active_op_array));
	zend_compile_expr_list(&result, cond_ast TSRMLS_CC);
	zend_do_extended_info(TSRMLS_C);

	opnum_jmpz = zend_emit_cond_jump(ZEND_JMPZ, &result, 0 TSRMLS_CC);
	zend_begin_loop(TSRMLS_C);

	zend_compile_stmt(stmt_ast TSRMLS_CC);

	opnum_loop = get_next_op_number(CG(active_op_array));
	zend_compile_expr_list(&result, loop_ast TSRMLS_CC);
	zend_do_free(&result TSRMLS_CC);

	zend_emit_jump(opnum_cond TSRMLS_CC);

	zend_update_jump_target_to_next(opnum_jmpz TSRMLS_CC);

	zend_end_loop(opnum_loop, 0 TSRMLS_CC);
}
/* }}} */

void zend_compile_foreach(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	zend_ast *value_ast = ast->child[1];
	zend_ast *key_ast = ast->child[2];
	zend_ast *stmt_ast = ast->child[3];
	zend_bool by_ref = value_ast->kind == ZEND_AST_REF;
	zend_bool is_variable = zend_is_variable(expr_ast) && !zend_is_call(expr_ast)
		&& zend_can_write_to_variable(expr_ast);

	znode expr_node, reset_node, value_node, key_node;
	zend_op *opline;
	uint32_t opnum_reset, opnum_fetch;

	if (key_ast) {
		if (key_ast->kind == ZEND_AST_REF) {
			zend_error_noreturn(E_COMPILE_ERROR, "Key element cannot be a reference");
		}
		if (key_ast->kind == ZEND_AST_LIST) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use list as key element");
		}
	}

	if (by_ref) {
		value_ast = value_ast->child[0];
	}

	if (by_ref && is_variable) {
		zend_compile_var(&expr_node, expr_ast, BP_VAR_W TSRMLS_CC);
	} else {
		zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);
	}

	if (by_ref) {
		zend_separate_if_call_and_write(&expr_node, expr_ast, BP_VAR_W TSRMLS_CC);
	}

	opnum_reset = get_next_op_number(CG(active_op_array));
	opline = zend_emit_op(&reset_node, ZEND_FE_RESET, &expr_node, NULL TSRMLS_CC);
	if (by_ref && is_variable) {
		opline->extended_value = ZEND_FE_FETCH_BYREF;
	}

	zend_stack_push(&CG(loop_var_stack), &reset_node);

	opnum_fetch = get_next_op_number(CG(active_op_array));
	opline = zend_emit_op(&value_node, ZEND_FE_FETCH, &reset_node, NULL TSRMLS_CC);
	if (by_ref) {
		opline->extended_value |= ZEND_FE_FETCH_BYREF;
	}
	if (key_ast) {
		opline->extended_value |= ZEND_FE_FETCH_WITH_KEY;
	}

	opline = zend_emit_op(NULL, ZEND_OP_DATA, NULL, NULL TSRMLS_CC);

	/* Allocate enough space to keep HashPointer on VM stack */
	opline->op1_type = IS_TMP_VAR;
	opline->op1.var = get_temporary_variable(CG(active_op_array));
	if (sizeof(HashPointer) > sizeof(zval)) {
		/* Make sure 1 zval is enough for HashPointer (2 must be enough) */
		get_temporary_variable(CG(active_op_array));
	}

	if (key_ast) {
		zend_make_tmp_result(&key_node, opline TSRMLS_CC);
	}

	if (by_ref) {
		zend_emit_assign_ref_znode(value_ast, &value_node TSRMLS_CC);
	} else {
		zend_emit_assign_znode(value_ast, &value_node TSRMLS_CC);
	}

	if (key_ast) {
		zend_emit_assign_znode(key_ast, &key_node TSRMLS_CC);
	}

	zend_begin_loop(TSRMLS_C);

	zend_compile_stmt(stmt_ast TSRMLS_CC);

	zend_emit_jump(opnum_fetch TSRMLS_CC);

	opline = &CG(active_op_array)->opcodes[opnum_reset];
	opline->op2.opline_num = get_next_op_number(CG(active_op_array));

	opline = &CG(active_op_array)->opcodes[opnum_fetch];
	opline->op2.opline_num = get_next_op_number(CG(active_op_array));

	zend_end_loop(opnum_fetch, 1 TSRMLS_CC);

	generate_free_loop_var(&reset_node TSRMLS_CC);
	zend_stack_del_top(&CG(loop_var_stack));
}
/* }}} */

void zend_compile_if(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;
	uint32_t *jmp_opnums;
	
	if (list->children > 1) {
		jmp_opnums = safe_emalloc(sizeof(uint32_t), list->children - 1, 0);
	}

	for (i = 0; i < list->children; ++i) {
		zend_ast *elem_ast = list->child[i];
		zend_ast *cond_ast = elem_ast->child[0];
		zend_ast *stmt_ast = elem_ast->child[1];

		znode cond_node;
		uint32_t opnum_jmpz;
		if (cond_ast) {
			zend_compile_expr(&cond_node, cond_ast TSRMLS_CC);
			opnum_jmpz = zend_emit_cond_jump(ZEND_JMPZ, &cond_node, 0 TSRMLS_CC);
		}

		zend_compile_stmt(stmt_ast TSRMLS_CC);

		if (i != list->children - 1) {
			jmp_opnums[i] = zend_emit_jump(0 TSRMLS_CC);
		}

		if (cond_ast) {
			zend_update_jump_target_to_next(opnum_jmpz TSRMLS_CC);
		}
	}

	if (list->children > 1) {
		for (i = 0; i < list->children - 1; ++i) {
			zend_update_jump_target_to_next(jmp_opnums[i] TSRMLS_CC);
		}
		efree(jmp_opnums);
	}
}
/* }}} */

void zend_compile_switch(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	zend_ast_list *cases = zend_ast_get_list(ast->child[1]);

	uint32_t i;
	zend_bool has_default_case = 0;

	znode expr_node, case_node;
	zend_op *opline;
	uint32_t *jmpnz_opnums = safe_emalloc(sizeof(uint32_t), cases->children, 0);
	uint32_t opnum_default_jmp;

	zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);

	zend_stack_push(&CG(loop_var_stack), &expr_node);

	zend_begin_loop(TSRMLS_C);

	case_node.op_type = IS_TMP_VAR;
	case_node.u.op.var = get_temporary_variable(CG(active_op_array));

	for (i = 0; i < cases->children; ++i) {
		zend_ast *case_ast = cases->child[i];
		zend_ast *cond_ast = case_ast->child[0];
		znode cond_node;

		if (!cond_ast) {
			if (has_default_case) {
				CG(zend_lineno) = case_ast->lineno;
				zend_error_noreturn(E_COMPILE_ERROR,
					"Switch statements may only contain one default clause");
			}
			has_default_case = 1;
			continue;
		}

		zend_compile_expr(&cond_node, cond_ast TSRMLS_CC);

		opline = zend_emit_op(NULL, ZEND_CASE, &expr_node, &cond_node TSRMLS_CC);
		SET_NODE(opline->result, &case_node);
		if (opline->op1_type == IS_CONST) {
			zval_copy_ctor(&CONSTANT(opline->op1.constant));
		}

		jmpnz_opnums[i] = zend_emit_cond_jump(ZEND_JMPNZ, &case_node, 0 TSRMLS_CC);
	}

	opnum_default_jmp = zend_emit_jump(0 TSRMLS_CC);

	for (i = 0; i < cases->children; ++i) {
		zend_ast *case_ast = cases->child[i];
		zend_ast *cond_ast = case_ast->child[0];
		zend_ast *stmt_ast = case_ast->child[1];

		if (cond_ast) {
			zend_update_jump_target_to_next(jmpnz_opnums[i] TSRMLS_CC);
		} else {
			zend_update_jump_target_to_next(opnum_default_jmp TSRMLS_CC);
		}

		zend_compile_stmt(stmt_ast TSRMLS_CC);
	}

	if (!has_default_case) {
		zend_update_jump_target_to_next(opnum_default_jmp TSRMLS_CC);
	}

	zend_end_loop(get_next_op_number(CG(active_op_array)), 1 TSRMLS_CC);

	if (expr_node.op_type == IS_VAR || expr_node.op_type == IS_TMP_VAR) {
		zend_emit_op(NULL, ZEND_FREE,
			&expr_node, NULL TSRMLS_CC);
	} else if (expr_node.op_type == IS_CONST) {
		zval_dtor(&expr_node.u.constant);
	}

	zend_stack_del_top(&CG(loop_var_stack));
	efree(jmpnz_opnums);
}
/* }}} */

void zend_compile_try(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *try_ast = ast->child[0];
	zend_ast_list *catches = zend_ast_get_list(ast->child[1]);
	zend_ast *finally_ast = ast->child[2];

	uint32_t i;
	zend_op *opline;
	uint32_t try_catch_offset = zend_add_try_element(
		get_next_op_number(CG(active_op_array)) TSRMLS_CC);
	uint32_t *jmp_opnums = safe_emalloc(sizeof(uint32_t), catches->children, 0);

	if (catches->children == 0 && !finally_ast) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use try without catch or finally");
	}

	zend_compile_stmt(try_ast TSRMLS_CC);

	if (catches->children != 0) {
		jmp_opnums[0] = zend_emit_jump(0 TSRMLS_CC);
	}

	for (i = 0; i < catches->children; ++i) {
		zend_ast *catch_ast = catches->child[i];
		zend_ast *class_ast = catch_ast->child[0];
		zend_ast *var_ast = catch_ast->child[1];
		zend_ast *stmt_ast = catch_ast->child[2];
		zval *var_name = zend_ast_get_zval(var_ast);
		zend_bool is_last_catch = (i + 1 == catches->children);

		uint32_t opnum_catch;

		if (!zend_is_const_default_class_ref(class_ast)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Bad class name in the catch statement");
		}

		opnum_catch = get_next_op_number(CG(active_op_array));
		if (i == 0) {
			CG(active_op_array)->try_catch_array[try_catch_offset].catch_op = opnum_catch;
		}

		CG(zend_lineno) = catch_ast->lineno;

		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = ZEND_CATCH;
		opline->op1_type = IS_CONST;
		opline->op1.constant = zend_add_class_name_literal(CG(active_op_array),
			zend_resolve_class_name_ast(class_ast TSRMLS_CC) TSRMLS_CC);

		opline->op2_type = IS_CV;
		opline->op2.var = lookup_cv(CG(active_op_array), zend_string_copy(Z_STR_P(var_name)) TSRMLS_CC);
		opline->result.num = is_last_catch;

		zend_compile_stmt(stmt_ast TSRMLS_CC);

		if (!is_last_catch) {
			jmp_opnums[i + 1] = zend_emit_jump(0 TSRMLS_CC);
		}

		opline = &CG(active_op_array)->opcodes[opnum_catch];
		opline->extended_value = get_next_op_number(CG(active_op_array));
	}

	for (i = 0; i < catches->children; ++i) {
		zend_update_jump_target_to_next(jmp_opnums[i] TSRMLS_CC);
	}

	if (finally_ast) {
		uint32_t opnum_jmp = get_next_op_number(CG(active_op_array)) + 1;

		opline = zend_emit_op(NULL, ZEND_FAST_CALL, NULL, NULL TSRMLS_CC);
		opline->op1.opline_num = opnum_jmp + 1;

		zend_emit_op(NULL, ZEND_JMP, NULL, NULL TSRMLS_CC);

		CG(context).in_finally++;
		zend_compile_stmt(finally_ast TSRMLS_CC);
		CG(context).in_finally--;

		CG(active_op_array)->try_catch_array[try_catch_offset].finally_op = opnum_jmp + 1;
		CG(active_op_array)->try_catch_array[try_catch_offset].finally_end
			= get_next_op_number(CG(active_op_array));
		CG(active_op_array)->fn_flags |= ZEND_ACC_HAS_FINALLY_BLOCK;

		zend_emit_op(NULL, ZEND_FAST_RET, NULL, NULL TSRMLS_CC);

		zend_update_jump_target_to_next(opnum_jmp TSRMLS_CC);
	}

	efree(jmp_opnums);
}
/* }}} */

/* Encoding declarations must already be handled during parsing */
void zend_handle_encoding_declaration(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_list *declares = zend_ast_get_list(ast);
	uint32_t i;
	for (i = 0; i < declares->children; ++i) {
		zend_ast *declare_ast = declares->child[i];
		zend_ast *name_ast = declare_ast->child[0];
		zend_ast *value_ast = declare_ast->child[1];
		zend_string *name = zend_ast_get_str(name_ast);

		if (zend_string_equals_literal_ci(name, "encoding")) {
			if (value_ast->kind != ZEND_AST_ZVAL) {
				zend_error_noreturn(E_COMPILE_ERROR, "Encoding must be a literal");
			}

			if (CG(multibyte)) {
				zend_string *encoding_name = zval_get_string(zend_ast_get_zval(value_ast));

				const zend_encoding *new_encoding, *old_encoding;
				zend_encoding_filter old_input_filter;

				CG(encoding_declared) = 1;

				new_encoding = zend_multibyte_fetch_encoding(encoding_name->val TSRMLS_CC);
				if (!new_encoding) {
					zend_error(E_COMPILE_WARNING, "Unsupported encoding [%s]", encoding_name->val);
				} else {
					old_input_filter = LANG_SCNG(input_filter);
					old_encoding = LANG_SCNG(script_encoding);
					zend_multibyte_set_filter(new_encoding TSRMLS_CC);

					/* need to re-scan if input filter changed */
					if (old_input_filter != LANG_SCNG(input_filter) ||
						 (old_input_filter && new_encoding != old_encoding)) {
						zend_multibyte_yyinput_again(old_input_filter, old_encoding TSRMLS_CC);
					}
				}

				zend_string_release(encoding_name);
			} else {
				zend_error(E_COMPILE_WARNING, "declare(encoding=...) ignored because "
					"Zend multibyte feature is turned off by settings");
			}
		}
	}
}
/* }}} */

void zend_compile_declare(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_list *declares = zend_ast_get_list(ast->child[0]);
	zend_ast *stmt_ast = ast->child[1];
	zend_declarables orig_declarables = CG(declarables);
	uint32_t i;

	for (i = 0; i < declares->children; ++i) {
		zend_ast *declare_ast = declares->child[i];
		zend_ast *name_ast = declare_ast->child[0];
		zend_ast *value_ast = declare_ast->child[1];

		zend_string *name = zend_ast_get_str(name_ast);
		if (zend_string_equals_literal_ci(name, "ticks")) {
			zval value_zv;
			zend_const_expr_to_zval(&value_zv, value_ast TSRMLS_CC);
			convert_to_long(&value_zv);
			ZVAL_COPY_VALUE(&CG(declarables).ticks, &value_zv);
			zval_dtor(&value_zv);
		} else if (zend_string_equals_literal_ci(name, "encoding")) {
			/* Encoding declaration was already handled during parsing. Here we
			 * only check that it is the first statement in the file. */
			uint32_t num = CG(active_op_array)->last;
			while (num > 0 &&
				   (CG(active_op_array)->opcodes[num-1].opcode == ZEND_EXT_STMT ||
					CG(active_op_array)->opcodes[num-1].opcode == ZEND_TICKS)) {
				--num;
			}

			if (num > 0) {
				zend_error_noreturn(E_COMPILE_ERROR, "Encoding declaration pragma must be "
					"the very first statement in the script");
			}
		} else {
			zend_error(E_COMPILE_WARNING, "Unsupported declare '%s'", name->val);
		}
	}

	if (stmt_ast) {
		zend_compile_stmt(stmt_ast TSRMLS_CC);

		CG(declarables) = orig_declarables;
	}
}
/* }}} */

void zend_compile_stmt_list(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;
	for (i = 0; i < list->children; ++i) {
		zend_compile_stmt(list->child[i] TSRMLS_CC);
	}
}
/* }}} */

void zend_compile_params(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;
	zend_op_array *op_array = CG(active_op_array);
	zend_arg_info *arg_infos;

	if (list->children == 0) {
		return;
	}
	
	arg_infos = safe_emalloc(sizeof(zend_arg_info), list->children, 0);
	for (i = 0; i < list->children; ++i) {
		zend_ast *param_ast = list->child[i];
		zend_ast *type_ast = param_ast->child[0];
		zend_ast *var_ast = param_ast->child[1];
		zend_ast *default_ast = param_ast->child[2];
		zend_string *name = zend_ast_get_str(var_ast);
		zend_bool is_ref = (param_ast->attr & ZEND_PARAM_REF) != 0;
		zend_bool is_variadic = (param_ast->attr & ZEND_PARAM_VARIADIC) != 0;

		znode var_node, default_node;
		zend_uchar opcode;
		zend_op *opline;
		zend_arg_info *arg_info;

		if (zend_is_auto_global(name TSRMLS_CC)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot re-assign auto-global variable %s",
				name->val);
		}

		var_node.op_type = IS_CV;
		var_node.u.op.var = lookup_cv(CG(active_op_array), zend_string_copy(name) TSRMLS_CC);

		if (EX_VAR_TO_NUM(var_node.u.op.var) != i) {
			zend_error_noreturn(E_COMPILE_ERROR, "Redefinition of parameter $%s",
				name->val);
		} else if (zend_string_equals_literal(name, "this")) {
			if (op_array->scope && (op_array->fn_flags & ZEND_ACC_STATIC) == 0) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot re-assign $this");
			}
			op_array->this_var = var_node.u.op.var;
		}

		if (op_array->fn_flags & ZEND_ACC_VARIADIC) {
			zend_error_noreturn(E_COMPILE_ERROR, "Only the last parameter can be variadic");
		}

		if (is_variadic) {
			opcode = ZEND_RECV_VARIADIC;
			default_node.op_type = IS_UNUSED;
			op_array->fn_flags |= ZEND_ACC_VARIADIC;

			if (default_ast) {
				zend_error_noreturn(E_COMPILE_ERROR,
					"Variadic parameter cannot have a default value");
			}
		} else if (default_ast) {
			opcode = ZEND_RECV_INIT;
			default_node.op_type = IS_CONST;
			zend_const_expr_to_zval(&default_node.u.constant, default_ast TSRMLS_CC);
		} else {
			opcode = ZEND_RECV;
			default_node.op_type = IS_UNUSED;
			op_array->required_num_args = i + 1;
		}

		opline = zend_emit_op(NULL, opcode, NULL, &default_node TSRMLS_CC);
		SET_NODE(opline->result, &var_node);
		opline->op1.num = i + 1;

		arg_info = &arg_infos[i];
		arg_info->name = estrndup(name->val, name->len);
		arg_info->name_len = name->len;
		arg_info->pass_by_reference = is_ref;
		arg_info->is_variadic = is_variadic;
		arg_info->type_hint = 0;
		arg_info->allow_null = 1;
		arg_info->class_name = NULL;
		arg_info->class_name_len = 0;

		if (type_ast) {
			zend_bool has_null_default = default_ast
				&& (Z_TYPE(default_node.u.constant) == IS_NULL
					|| (Z_TYPE(default_node.u.constant) == IS_CONSTANT
						&& strcasecmp(Z_STRVAL(default_node.u.constant), "NULL") == 0)
					|| Z_TYPE(default_node.u.constant) == IS_CONSTANT_AST); // ???

			op_array->fn_flags |= ZEND_ACC_HAS_TYPE_HINTS;
			arg_info->allow_null = has_null_default;

			if (type_ast->kind == ZEND_AST_TYPE) {
				arg_info->type_hint = type_ast->attr;
				if (arg_info->type_hint == IS_ARRAY) {
					if (default_ast && !has_null_default
						&& Z_TYPE(default_node.u.constant) != IS_ARRAY
					) {
						zend_error_noreturn(E_COMPILE_ERROR, "Default value for parameters "
							"with array type hint can only be an array or NULL");
					}
				} else if (arg_info->type_hint == IS_CALLABLE && default_ast) {
					if (!has_null_default) {
						zend_error_noreturn(E_COMPILE_ERROR, "Default value for parameters "
							"with callable type hint can only be NULL");
					}
				}
			} else {
				zend_string *class_name = zend_ast_get_str(type_ast);

				if (zend_is_const_default_class_ref(type_ast)) {
					class_name = zend_resolve_class_name_ast(type_ast TSRMLS_CC);
				} else {
					zend_string_addref(class_name);
				}

				arg_info->type_hint = IS_OBJECT;
				arg_info->class_name = estrndup(class_name->val, class_name->len);
				arg_info->class_name_len = class_name->len;

				zend_string_release(class_name);

				if (default_ast && !has_null_default) {
						zend_error_noreturn(E_COMPILE_ERROR, "Default value for parameters "
							"with a class type hint can only be NULL");
				}
			}
		}
	}

	/* These are assigned at the end to avoid unitialized memory in case of an error */
	op_array->num_args = list->children;
	op_array->arg_info = arg_infos;
}
/* }}} */

void zend_compile_closure_uses(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;

	for (i = 0; i < list->children; ++i) {
		zend_ast *var_ast = list->child[i];
		zend_string *name = zend_ast_get_str(var_ast);
		zend_bool by_ref = var_ast->attr;
		zval zv;

		if (zend_string_equals_literal(name, "this")) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use $this as lexical variable");
		}

		ZVAL_NULL(&zv);
		Z_CONST_FLAGS(zv) = by_ref ? IS_LEXICAL_REF : IS_LEXICAL_VAR;

		zend_compile_static_var_common(var_ast, &zv, by_ref TSRMLS_CC);
	}
}
/* }}} */

void zend_begin_method_decl(zend_op_array *op_array, zend_string *name, zend_bool has_body TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce = CG(active_class_entry);
	zend_bool in_interface = (ce->ce_flags & ZEND_ACC_INTERFACE) != 0;
	zend_bool in_trait = ZEND_CE_IS_TRAIT(ce);
	zend_bool is_public = (op_array->fn_flags & ZEND_ACC_PUBLIC) != 0;
	zend_bool is_static = (op_array->fn_flags & ZEND_ACC_STATIC) != 0;

	zend_string *lcname;

	if (in_interface) {
		if ((op_array->fn_flags & ZEND_ACC_PPP_MASK) != ZEND_ACC_PUBLIC) {
			zend_error_noreturn(E_COMPILE_ERROR, "Access type for interface method "
				"%s::%s() must be omitted", ce->name->val, name->val);
		}
		op_array->fn_flags |= ZEND_ACC_ABSTRACT;
	} else if (is_static && (op_array->fn_flags & ZEND_ACC_ABSTRACT)) {
		zend_error(E_STRICT, "Static function %s::%s() should not be abstract",
			ce->name->val, name->val);
	}

	if (op_array->fn_flags & ZEND_ACC_ABSTRACT) {
		//zend_op *opline;

		if (op_array->fn_flags & ZEND_ACC_PRIVATE) {
			zend_error_noreturn(E_COMPILE_ERROR, "%s function %s::%s() cannot be declared private",
				in_interface ? "Interface" : "Abstract", ce->name->val, name->val);
		}

		if (has_body) {
			zend_error_noreturn(E_COMPILE_ERROR, "%s function %s::%s() cannot contain body",
				in_interface ? "Interface" : "Abstract", ce->name->val, name->val);
		}

		ce->ce_flags |= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;

		/*opline = get_next_op(op_array TSRMLS_CC);
		opline->opcode = ZEND_RAISE_ABSTRACT_ERROR;
		SET_UNUSED(opline->op1);
		SET_UNUSED(opline->op2);*/
	} else if (!has_body) {
		zend_error_noreturn(E_COMPILE_ERROR, "Non-abstract method %s::%s() must contain body",
			ce->name->val, name->val);
	}

	op_array->scope = ce;
	op_array->function_name = zend_string_copy(name);

	lcname = zend_string_alloc(name->len, 0);
	zend_str_tolower_copy(lcname->val, name->val, name->len);
	lcname = zend_new_interned_string(lcname TSRMLS_CC);

	if (zend_hash_add_ptr(&ce->function_table, lcname, op_array) == NULL) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare %s::%s()",
			ce->name->val, name->val);
	}

	if (in_interface) {
		if (zend_string_equals_literal(lcname, ZEND_CALL_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __call() must have "
					"public visibility and cannot be static");
			}
		} else if (zend_string_equals_literal(lcname, ZEND_CALLSTATIC_FUNC_NAME)) {
			if (!is_public || !is_static) {
				zend_error(E_WARNING, "The magic method __callStatic() must have "
					"public visibility and be static");
			}
		} else if (zend_string_equals_literal(lcname, ZEND_GET_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __get() must have "
					"public visibility and cannot be static");
			}
		} else if (zend_string_equals_literal(lcname, ZEND_SET_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __set() must have "
					"public visibility and cannot be static");
			}
		} else if (zend_string_equals_literal(lcname, ZEND_UNSET_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __unset() must have "
					"public visibility and cannot be static");
			}
		} else if (zend_string_equals_literal(lcname, ZEND_ISSET_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __isset() must have "
					"public visibility and cannot be static");
			}
		} else if (zend_string_equals_literal(lcname, ZEND_TOSTRING_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __toString() must have "
					"public visibility and cannot be static");
			}
		} else if (zend_string_equals_literal(lcname, ZEND_INVOKE_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __invoke() must have "
					"public visibility and cannot be static");
			}
		} else if (zend_string_equals_literal(lcname, ZEND_DEBUGINFO_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __debugInfo() must have "
					"public visibility and cannot be static");
			}
		}
	} else {
		if (!in_trait && zend_string_equals_str_ci(lcname, ce->name)) {
			if (!ce->constructor) {
				ce->constructor = (zend_function *) op_array;
			}
		} else if (zend_string_equals_literal(lcname, ZEND_CONSTRUCTOR_FUNC_NAME)) {
			if (CG(active_class_entry)->constructor) {
				zend_error(E_STRICT, "Redefining already defined constructor for class %s",
					ce->name->val);
			}
			ce->constructor = (zend_function *) op_array;
		} else if (zend_string_equals_literal(lcname, ZEND_DESTRUCTOR_FUNC_NAME)) {
			ce->destructor = (zend_function *) op_array;
		} else if (zend_string_equals_literal(lcname, ZEND_CLONE_FUNC_NAME)) {
			ce->clone = (zend_function *) op_array;
		} else if (zend_string_equals_literal(lcname, ZEND_CALL_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __call() must have "
					"public visibility and cannot be static");
			}
			ce->__call = (zend_function *) op_array;
		} else if (zend_string_equals_literal(lcname, ZEND_CALLSTATIC_FUNC_NAME)) {
			if (!is_public || !is_static) {
				zend_error(E_WARNING, "The magic method __callStatic() must have "
					"public visibility and be static");
			}
			ce->__callstatic = (zend_function *) op_array;
		} else if (zend_string_equals_literal(lcname, ZEND_GET_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __get() must have "
					"public visibility and cannot be static");
			}
			ce->__get = (zend_function *) op_array;
		} else if (zend_string_equals_literal(lcname, ZEND_SET_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __set() must have "
					"public visibility and cannot be static");
			}
			ce->__set = (zend_function *) op_array;
		} else if (zend_string_equals_literal(lcname, ZEND_UNSET_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __unset() must have "
					"public visibility and cannot be static");
			}
			ce->__unset = (zend_function *) op_array;
		} else if (zend_string_equals_literal(lcname, ZEND_ISSET_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __isset() must have "
					"public visibility and cannot be static");
			}
			ce->__isset = (zend_function *) op_array;
		} else if (zend_string_equals_literal(lcname, ZEND_TOSTRING_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __toString() must have "
					"public visibility and cannot be static");
			}
			ce->__tostring = (zend_function *) op_array;
		} else if (zend_string_equals_literal(lcname, ZEND_INVOKE_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __invoke() must have "
					"public visibility and cannot be static");
			}
		} else if (zend_string_equals_literal(lcname, ZEND_DEBUGINFO_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __debugInfo() must have "
					"public visibility and cannot be static");
			}
			ce->__debugInfo = (zend_function *) op_array;
		} else if (!is_static) {
			op_array->fn_flags |= ZEND_ACC_ALLOW_STATIC;
		}
	}

	zend_string_release(lcname);
}
/* }}} */

static void zend_begin_func_decl(znode *result, zend_op_array *op_array, zend_ast_decl *decl TSRMLS_DC) /* {{{ */
{
	zend_ast *params_ast = decl->child[0];
	zend_string *name = decl->name, *lcname;
	zend_op *opline;

	op_array->function_name = name = zend_prefix_with_ns(name TSRMLS_CC);

	lcname = zend_string_alloc(name->len, 0);
	zend_str_tolower_copy(lcname->val, name->val, name->len);

	if (CG(current_import_function)) {
		zend_string *import_name = zend_hash_find_ptr(CG(current_import_function), lcname);
		if (import_name && !zend_string_equals_str_ci(lcname, import_name)) {
			zend_error(E_COMPILE_ERROR, "Cannot declare function %s "
				"because the name is already in use", name->val);
		}
	}

	if (zend_string_equals_literal(lcname, ZEND_AUTOLOAD_FUNC_NAME)
		&& zend_ast_get_list(params_ast)->children != 1
	) {
		zend_error_noreturn(E_COMPILE_ERROR, "%s() must take exactly 1 argument",
			ZEND_AUTOLOAD_FUNC_NAME);
	}

	if (op_array->fn_flags & ZEND_ACC_CLOSURE) {
		opline = zend_emit_op_tmp(result, ZEND_DECLARE_LAMBDA_FUNCTION, NULL, NULL TSRMLS_CC);
	} else {
		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = ZEND_DECLARE_FUNCTION;
		opline->op2_type = IS_CONST;
		LITERAL_STR(opline->op2, zend_string_copy(lcname));
	}

	{
		zend_string *key = zend_build_runtime_definition_key(lcname, decl->lex_pos TSRMLS_CC);

		opline->op1_type = IS_CONST;
		LITERAL_STR(opline->op1, key);

		zend_hash_update_ptr(CG(function_table), key, op_array);
	}

	zend_string_release(lcname);
}
/* }}} */

void zend_compile_func_decl(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_decl *decl = (zend_ast_decl *) ast;
	zend_ast *params_ast = decl->child[0];
	zend_ast *uses_ast = decl->child[1];
	zend_ast *stmt_ast = decl->child[2];
	zend_bool is_method = decl->kind == ZEND_AST_METHOD;

	zend_op_array *orig_op_array = CG(active_op_array);
	zend_op_array *op_array = zend_arena_alloc(&CG(arena), sizeof(zend_op_array));

	// TODO.AST interactive (not just here - also bpc etc!)
	
	init_op_array(op_array, ZEND_USER_FUNCTION, INITIAL_OP_ARRAY_SIZE TSRMLS_CC);

	op_array->fn_flags |= decl->flags;
	op_array->line_start = decl->start_lineno;
	op_array->line_end = decl->end_lineno;
	if (decl->doc_comment) {
		op_array->doc_comment = zend_string_copy(decl->doc_comment);
	}
	if (decl->kind == ZEND_AST_CLOSURE) {
		op_array->fn_flags |= ZEND_ACC_CLOSURE;
	}

	if (is_method) {
		zend_bool has_body = stmt_ast != NULL;
		zend_begin_method_decl(op_array, decl->name, has_body TSRMLS_CC);
	} else {
		zend_begin_func_decl(result, op_array, decl TSRMLS_CC);
	}

	CG(active_op_array) = op_array;
	zend_stack_push(&CG(context_stack), (void *) &CG(context));
	zend_init_compiler_context(TSRMLS_C);

	if (CG(compiler_options) & ZEND_COMPILE_EXTENDED_INFO) {
		zend_op *opline_ext = zend_emit_op(NULL, ZEND_EXT_NOP, NULL, NULL TSRMLS_CC);
		opline_ext->lineno = decl->start_lineno;
	}

	{
		/* Push a separator to the loop variable stack */
		znode dummy_var;
		dummy_var.op_type = IS_UNUSED;

		zend_stack_push(&CG(loop_var_stack), (void *) &dummy_var);
	}

	zend_compile_params(params_ast TSRMLS_CC);
	if (uses_ast) {
		zend_compile_closure_uses(uses_ast TSRMLS_CC);
	}
	zend_compile_stmt(stmt_ast TSRMLS_CC);

	if (is_method) {
		zend_check_magic_method_implementation(
			CG(active_class_entry), (zend_function *) op_array, E_COMPILE_ERROR TSRMLS_CC);
	}

	zend_do_extended_info(TSRMLS_C);
	zend_emit_final_return(NULL TSRMLS_CC);

	pass_two(CG(active_op_array) TSRMLS_CC);
	zend_release_labels(0 TSRMLS_CC);

	/* Pop the loop variable stack separator */
	zend_stack_del_top(&CG(loop_var_stack));

	CG(active_op_array) = orig_op_array;
}
/* }}} */

void zend_compile_prop_decl(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t flags = list->attr;
	zend_class_entry *ce = CG(active_class_entry);
	uint32_t i, children = list->children;
	zend_string *doc_comment = NULL;

	if (ce->ce_flags & ZEND_ACC_INTERFACE) {
		zend_error_noreturn(E_COMPILE_ERROR, "Interfaces may not include member variables");
	}

	if (flags & ZEND_ACC_ABSTRACT) {
		zend_error_noreturn(E_COMPILE_ERROR, "Properties cannot be declared abstract");
	}

	/* Doc comment has been appended as last element in property list */
	if (list->child[children - 1]->kind == ZEND_AST_ZVAL) {
		doc_comment = zend_string_copy(zend_ast_get_str(list->child[children - 1]));
		children -= 1;
	}

	for (i = 0; i < children; ++i) {
		zend_ast *prop_ast = list->child[i];
		zend_ast *name_ast = prop_ast->child[0];
		zend_ast *value_ast = prop_ast->child[1];
		zend_string *name = zend_ast_get_str(name_ast);
		zval value_zv;

		if (flags & ZEND_ACC_FINAL) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot declare property %s::$%s final, "
				"the final modifier is allowed only for methods and classes",
				ce->name->val, name->val);
		}

		if (zend_hash_exists(&ce->properties_info, name)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare %s::$%s",
				ce->name->val, name->val);
		}

		if (value_ast) {
			zend_const_expr_to_zval(&value_zv, value_ast TSRMLS_CC);
		} else {
			ZVAL_NULL(&value_zv);
		}

		name = zend_new_interned_string_safe(name TSRMLS_CC);
		zend_declare_property_ex(ce, name, &value_zv, flags, doc_comment TSRMLS_CC);

		/* Doc comment is only assigned to first property */
		doc_comment = NULL;
	}
}
/* }}} */

void zend_compile_class_const_decl(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	zend_class_entry *ce = CG(active_class_entry);
	uint32_t i;

	for (i = 0; i < list->children; ++i) {
		zend_ast *const_ast = list->child[i];
		zend_ast *name_ast = const_ast->child[0];
		zend_ast *value_ast = const_ast->child[1];
		zend_string *name = zend_ast_get_str(name_ast);
		zval value_zv;

		if (ZEND_CE_IS_TRAIT(ce)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Traits cannot have constants");
			return;
		}

		zend_const_expr_to_zval(&value_zv, value_ast TSRMLS_CC);

		name = zend_new_interned_string_safe(name TSRMLS_CC);
		if (zend_hash_add(&ce->constants_table, name, &value_zv) == NULL) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot redefine class constant %s::%s",
				ce->name->val, name->val);
		}

		if (Z_CONSTANT(value_zv)) {
			ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
		}
	}
}
/* }}} */

static zend_trait_method_reference *zend_compile_method_ref(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *class_ast = ast->child[0];
	zend_ast *method_ast = ast->child[1];

	zend_trait_method_reference *method_ref = emalloc(sizeof(zend_trait_method_reference));
	method_ref->ce = NULL;
	method_ref->method_name = zend_string_copy(zend_ast_get_str(method_ast));

	if (class_ast) {
		method_ref->class_name = zend_resolve_class_name_ast(class_ast TSRMLS_CC);
	} else {
		method_ref->class_name = NULL;
	}

	return method_ref;
}
/* }}} */

static zend_string **zend_compile_name_list(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	zend_string **names = safe_emalloc(sizeof(zend_string *), list->children + 1, 0);
	uint32_t i;

	for (i = 0; i < list->children; ++i) {
		zend_ast *name_ast = list->child[i];
		names[i] = zend_resolve_class_name_ast(name_ast TSRMLS_CC);
	}

	names[list->children] = NULL;

	return names;
}
/* }}} */

static void zend_compile_trait_precedence(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *method_ref_ast = ast->child[0];
	zend_ast *insteadof_ast = ast->child[1];
	
	zend_trait_precedence *precedence = emalloc(sizeof(zend_trait_precedence));
	precedence->trait_method = zend_compile_method_ref(method_ref_ast TSRMLS_CC);
	precedence->exclude_from_classes
		= (void *) zend_compile_name_list(insteadof_ast TSRMLS_CC);

	zend_add_to_list(&CG(active_class_entry)->trait_precedences, precedence TSRMLS_CC);
}
/* }}} */

static void zend_compile_trait_alias(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *method_ref_ast = ast->child[0];
	zend_ast *alias_ast = ast->child[1];
	uint32_t modifiers = ast->attr;

	zend_trait_alias *alias;

	if (modifiers == ZEND_ACC_STATIC) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use 'static' as method modifier");
	} else if (modifiers == ZEND_ACC_ABSTRACT) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use 'abstract' as method modifier");
	} else if (modifiers == ZEND_ACC_FINAL) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use 'final' as method modifier");
	}

	alias = emalloc(sizeof(zend_trait_alias));
	alias->trait_method = zend_compile_method_ref(method_ref_ast TSRMLS_CC);
	alias->modifiers = modifiers;

	if (alias_ast) {
		alias->alias = zend_string_copy(zend_ast_get_str(alias_ast));
	} else {
		alias->alias = NULL;
	}

	zend_add_to_list(&CG(active_class_entry)->trait_aliases, alias TSRMLS_CC);
}
/* }}} */

void zend_compile_use_trait(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_list *traits = zend_ast_get_list(ast->child[0]);
	zend_ast_list *adaptations = ast->child[1] ? zend_ast_get_list(ast->child[1]) : NULL;
	zend_class_entry *ce = CG(active_class_entry);
	zend_op *opline;
	uint32_t i;

	for (i = 0; i < traits->children; ++i) {
		zend_ast *trait_ast = traits->child[i];
		zend_string *name = zend_ast_get_str(trait_ast);

		if (ce->ce_flags & ZEND_ACC_INTERFACE) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use traits inside of interfaces. "
				"%s is used in %s", name->val, ce->name->val);
		}

		switch (zend_get_class_fetch_type(name)) {
			case ZEND_FETCH_CLASS_SELF:
			case ZEND_FETCH_CLASS_PARENT:
			case ZEND_FETCH_CLASS_STATIC:
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot use '%s' as trait name "
					"as it is reserved", name->val);
				break;
		}

		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = ZEND_ADD_TRAIT;
		SET_NODE(opline->op1, &CG(implementing_class));
		opline->extended_value = ZEND_FETCH_CLASS_TRAIT;
		opline->op2_type = IS_CONST;
		opline->op2.constant = zend_add_class_name_literal(CG(active_op_array),
			zend_resolve_class_name_ast(trait_ast TSRMLS_CC) TSRMLS_CC);

		ce->num_traits++;
	}

	if (!adaptations) {
		return;
	}

	for (i = 0; i < adaptations->children; ++i) {
		zend_ast *adaptation_ast = adaptations->child[i];
		switch (adaptation_ast->kind) {
			case ZEND_AST_TRAIT_PRECEDENCE:
				zend_compile_trait_precedence(adaptation_ast TSRMLS_CC);
				break;
			case ZEND_AST_TRAIT_ALIAS:
				zend_compile_trait_alias(adaptation_ast TSRMLS_CC);
				break;
			EMPTY_SWITCH_DEFAULT_CASE()
		}
	}
}
/* }}} */

void zend_compile_implements(znode *class_node, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;
	for (i = 0; i < list->children; ++i) {
		zend_ast *class_ast = list->child[i];
		zend_string *name = zend_ast_get_str(class_ast);

		zend_op *opline;

		/* Traits can not implement interfaces */
		if (ZEND_CE_IS_TRAIT(CG(active_class_entry))) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use '%s' as interface on '%s' "
				"since it is a Trait", name->val, CG(active_class_entry)->name->val);
		}

		if (!zend_is_const_default_class_ref(class_ast)) {
			zend_error_noreturn(E_COMPILE_ERROR,
				"Cannot use '%s' as interface name as it is reserved", name->val);
		}

		opline = zend_emit_op(NULL, ZEND_ADD_INTERFACE, class_node, NULL TSRMLS_CC);
		opline->extended_value = ZEND_FETCH_CLASS_INTERFACE;
		opline->op2_type = IS_CONST;
		opline->op2.constant = zend_add_class_name_literal(CG(active_op_array),
			zend_resolve_class_name_ast(class_ast TSRMLS_CC) TSRMLS_CC);

		CG(active_class_entry)->num_interfaces++;
	}
}
/* }}} */

void zend_compile_class_decl(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_decl *decl = (zend_ast_decl *) ast;
	zend_ast *extends_ast = decl->child[0];
	zend_ast *implements_ast = decl->child[1];
	zend_ast *stmt_ast = decl->child[2];

	zend_string *name = decl->name, *lcname, *import_name = NULL;
	zend_class_entry *ce = zend_arena_alloc(&CG(arena), sizeof(zend_class_entry));
	zend_op *opline;
	znode declare_node, extends_node;

	if (CG(active_class_entry)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Class declarations may not be nested");
		return;
	}

	if (ZEND_FETCH_CLASS_DEFAULT != zend_get_class_fetch_type(name)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use '%s' as class name as it is reserved",
			name->val);
	}

	lcname = zend_string_alloc(name->len, 0);
	zend_str_tolower_copy(lcname->val, name->val, name->len);

	if (CG(current_import)) {
		import_name = zend_hash_find_ptr(CG(current_import), lcname);
	}

	if (CG(current_namespace)) {
		name = zend_prefix_with_ns(name TSRMLS_CC);

		zend_string_release(lcname);
		lcname = zend_string_alloc(name->len, 0);
		zend_str_tolower_copy(lcname->val, name->val, name->len);
	} else {
		zend_string_addref(name);
	}

	if (import_name && !zend_string_equals_str_ci(lcname, import_name)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot declare class %s "
			"because the name is already in use", name->val);
	}

	name = zend_new_interned_string(name TSRMLS_CC);
	lcname = zend_new_interned_string(lcname TSRMLS_CC);

	ce->type = ZEND_USER_CLASS;
	ce->name = name;
	zend_initialize_class_data(ce, 1 TSRMLS_CC);

	ce->ce_flags |= decl->flags;
	ce->info.user.filename = zend_get_compiled_filename(TSRMLS_C);
	ce->info.user.line_start = decl->start_lineno;
	ce->info.user.line_end = decl->end_lineno;
	if (decl->doc_comment) {
		ce->info.user.doc_comment = zend_string_copy(decl->doc_comment);
	}

	if (extends_ast) {
		if (ZEND_CE_IS_TRAIT(ce)) {
			zend_error_noreturn(E_COMPILE_ERROR, "A trait (%s) cannot extend a class. "
				"Traits can only be composed from other traits with the 'use' keyword. Error",
				name->val);
		}

		if (!zend_is_const_default_class_ref(extends_ast)) {
			zend_string *extends_name = zend_ast_get_str(extends_ast);
			zend_error_noreturn(E_COMPILE_ERROR,
				"Cannot use '%s' as class name as it is reserved", extends_name->val);
		}

		zend_compile_class_ref(&extends_node, extends_ast TSRMLS_CC);
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	zend_make_var_result(&declare_node, opline TSRMLS_CC);

	// TODO.AST drop this
	GET_NODE(&CG(implementing_class), opline->result);

	opline->op2_type = IS_CONST;
	LITERAL_STR(opline->op2, lcname);

	if (extends_ast) {
		opline->opcode = ZEND_DECLARE_INHERITED_CLASS;
		opline->extended_value = extends_node.u.op.var;
	} else {
		opline->opcode = ZEND_DECLARE_CLASS;
	}

	{
		zend_string *key = zend_build_runtime_definition_key(lcname, decl->lex_pos TSRMLS_CC);

		opline->op1_type = IS_CONST;
		LITERAL_STR(opline->op1, key);

		zend_hash_update_ptr(CG(class_table), key, ce);
	}

	CG(active_class_entry) = ce;

	if (implements_ast) {
		zend_compile_implements(&declare_node, implements_ast TSRMLS_CC);
	}

	zend_compile_stmt(stmt_ast TSRMLS_CC);

	if (ce->constructor) {
		ce->constructor->common.fn_flags |= ZEND_ACC_CTOR;
		if (ce->constructor->common.fn_flags & ZEND_ACC_STATIC) {
			zend_error_noreturn(E_COMPILE_ERROR, "Constructor %s::%s() cannot be static",
				ce->name->val, ce->constructor->common.function_name->val);
		}
	}
	if (ce->destructor) {
		ce->destructor->common.fn_flags |= ZEND_ACC_DTOR;
		if (ce->destructor->common.fn_flags & ZEND_ACC_STATIC) {
			zend_error_noreturn(E_COMPILE_ERROR, "Destructor %s::%s() cannot be static",
				ce->name->val, ce->destructor->common.function_name->val);
		}
	}
	if (ce->clone) {
		ce->clone->common.fn_flags |= ZEND_ACC_CLONE;
		if (ce->clone->common.fn_flags & ZEND_ACC_STATIC) {
			zend_error_noreturn(E_COMPILE_ERROR, "Clone method %s::%s() cannot be static",
				ce->name->val, ce->clone->common.function_name->val);
		}
	}

	/* Check for traits and proceed like with interfaces.
	 * The only difference will be a combined handling of them in the end.
	 * Thus, we need another opcode here. */
	if (ce->num_traits > 0) {
		ce->traits = NULL;
		ce->num_traits = 0;
		ce->ce_flags |= ZEND_ACC_IMPLEMENT_TRAITS;

		zend_emit_op(NULL, ZEND_BIND_TRAITS, &declare_node, NULL TSRMLS_CC);
	}

	if (!(ce->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS))
		&& (extends_ast || ce->num_interfaces > 0)
	) {
		zend_verify_abstract_class(ce TSRMLS_CC);
		if (ce->num_interfaces && !(ce->ce_flags & ZEND_ACC_IMPLEMENT_TRAITS)) {
			zend_emit_op(NULL, ZEND_VERIFY_ABSTRACT_CLASS, &declare_node, NULL TSRMLS_CC);
		}
	}

	/* Inherit interfaces; reset number to zero, we need it for above check and
	 * will restore it during actual implementation.
	 * The ZEND_ACC_IMPLEMENT_INTERFACES flag disables double call to
	 * zend_verify_abstract_class() */
	if (ce->num_interfaces > 0) {
		ce->interfaces = NULL;
		ce->num_interfaces = 0;
		ce->ce_flags |= ZEND_ACC_IMPLEMENT_INTERFACES;
	}

	CG(active_class_entry) = NULL;
}
/* }}} */

static HashTable *zend_get_import_ht(uint32_t type TSRMLS_DC) /* {{{ */
{
	switch (type) {
		case T_CLASS:
			if (!CG(current_import)) {
				CG(current_import) = emalloc(sizeof(HashTable));
				zend_hash_init(CG(current_import), 8, NULL, str_dtor, 0);
			}
			return CG(current_import);
		case T_FUNCTION:
			if (!CG(current_import_function)) {
				CG(current_import_function) = emalloc(sizeof(HashTable));
				zend_hash_init(CG(current_import_function), 8, NULL, str_dtor, 0);
			}
			return CG(current_import_function);
		case T_CONST:
			if (!CG(current_import_const)) {
				CG(current_import_const) = emalloc(sizeof(HashTable));
				zend_hash_init(CG(current_import_const), 8, NULL, str_dtor, 0);
			}
			return CG(current_import_const);
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

static char *zend_get_use_type_str(uint32_t type) /* {{{ */
{
	switch (type) {
		case T_CLASS:
			return "";
		case T_FUNCTION:
			return " function";
		case T_CONST:
			return " const";
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

static void zend_check_already_in_use(uint32_t type, zend_string *old_name, zend_string *new_name, zend_string *check_name) /* {{{ */
{
	if (zend_string_equals_str_ci(old_name, check_name)) {
		return;
	}

	zend_error_noreturn(E_COMPILE_ERROR, "Cannot use%s %s as %s because the name "
		"is already in use", zend_get_use_type_str(type), old_name->val, new_name->val);
}
/* }}} */

void zend_compile_use(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;
	zend_string *current_ns = CG(current_namespace);
	uint32_t type = ast->attr;
	HashTable *current_import = zend_get_import_ht(type TSRMLS_CC);
	zend_bool case_sensitive = type == T_CONST;

	for (i = 0; i < list->children; ++i) {
		zend_ast *use_ast = list->child[i];
		zend_ast *old_name_ast = use_ast->child[0];
		zend_ast *new_name_ast = use_ast->child[1];
		zend_string *old_name = zend_ast_get_str(old_name_ast);
		zend_string *new_name, *lookup_name;

		if (new_name_ast) {
			new_name = zend_string_copy(zend_ast_get_str(new_name_ast));
		} else {
			/* The form "use A\B" is eqivalent to "use A\B as B" */
			const char *p = zend_memrchr(old_name->val, '\\', old_name->len);
			if (p) {
				new_name = zend_string_init(p + 1, old_name->len - (p - old_name->val + 1), 0);
			} else {
				new_name = zend_string_copy(old_name);

				if (!current_ns) {
					if (type == T_CLASS && zend_string_equals_literal(new_name, "strict")) {
						zend_error_noreturn(E_COMPILE_ERROR,
							"You seem to be trying to use a different language...");
					}

					zend_error(E_WARNING, "The use statement with non-compound name '%s' "
						"has no effect", new_name->val);
				}
			}
		}

		if (case_sensitive) {
			lookup_name = zend_string_copy(new_name);
		} else {
			lookup_name = zend_string_alloc(new_name->len, 0);
			zend_str_tolower_copy(lookup_name->val, new_name->val, new_name->len);
		}

		if (type == T_CLASS && (zend_string_equals_literal(lookup_name, "self")
			|| zend_string_equals_literal(lookup_name, "parent"))
		) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use %s as %s because '%s' "
				"is a special class name", old_name->val, new_name->val, new_name->val);
		}

		if (current_ns) {
			zend_string *ns_name = zend_string_alloc(current_ns->len + 1 + new_name->len, 0);
			zend_str_tolower_copy(ns_name->val, current_ns->val, current_ns->len);
			ns_name->val[current_ns->len] = '\\';
			memcpy(ns_name->val + current_ns->len + 1, lookup_name->val, lookup_name->len);

			if (zend_hash_exists(CG(class_table), ns_name)) {
				zend_check_already_in_use(type, old_name, new_name, ns_name);
			}

			zend_string_free(ns_name);
		} else {
			switch (type) {
				case T_CLASS:
				{
					zend_class_entry *ce = zend_hash_find_ptr(CG(class_table), lookup_name);
					if (ce && ce->type == ZEND_USER_CLASS
						&& ce->info.user.filename == CG(compiled_filename)
					) {
						zend_check_already_in_use(type, old_name, new_name, lookup_name);
					}
					break;
				}
				case T_FUNCTION:
				{
					zend_function *fn = zend_hash_find_ptr(CG(function_table), lookup_name);
					if (fn && fn->type == ZEND_USER_FUNCTION
						&& fn->op_array.filename == CG(compiled_filename)
					) {
						zend_check_already_in_use(type, old_name, new_name, lookup_name);
					}
					break;
				}
				case T_CONST:
				{
					zend_string *filename = zend_hash_find_ptr(&CG(const_filenames), lookup_name);
					if (filename && filename == CG(compiled_filename)) {
						zend_check_already_in_use(type, old_name, new_name, lookup_name);
					}
					break;
				}
				EMPTY_SWITCH_DEFAULT_CASE()
			}
		}

		zend_string_addref(old_name);
		if (!zend_hash_add_ptr(current_import, lookup_name, old_name)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use%s %s as %s because the name "
				"is already in use", zend_get_use_type_str(type), old_name->val, new_name->val);
		}

		zend_string_release(lookup_name);
		zend_string_release(new_name);
	}
}
/* }}} */

void zend_compile_const_decl(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;
	for (i = 0; i < list->children; ++i) {
		zend_ast *const_ast = list->child[i];
		zend_ast *name_ast = const_ast->child[0];
		zend_ast *value_ast = const_ast->child[1];
		zend_string *name = zend_ast_get_str(name_ast);

		zend_string *import_name;
		znode name_node, value_node;
		zval *value_zv = &value_node.u.constant;

		value_node.op_type = IS_CONST;
		zend_const_expr_to_zval(value_zv, value_ast TSRMLS_CC);

		if (zend_get_ct_const(name, 0 TSRMLS_CC)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare constant '%s'", name->val);
		}

		name = zend_prefix_with_ns(name TSRMLS_CC);
		name = zend_new_interned_string(name TSRMLS_CC);

		if (CG(current_import_const)
			&& (import_name = zend_hash_find_ptr(CG(current_import_const), name))
		) {
			if (!zend_string_equals(import_name, name)) {
				zend_error(E_COMPILE_ERROR, "Cannot declare const %s because "
					"the name is already in use", name->val);
			}
		}

		name_node.op_type = IS_CONST;
		ZVAL_STR(&name_node.u.constant, name);

		zend_emit_op(NULL, ZEND_DECLARE_CONST, &name_node, &value_node TSRMLS_CC);

		zend_hash_add_ptr(&CG(const_filenames), name, CG(compiled_filename));
	}
}
/* }}}*/

void zend_compile_namespace(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *name_ast = ast->child[0];
	zend_ast *stmt_ast = ast->child[1];
	zend_string *name;
	zend_bool with_bracket = stmt_ast != NULL;

	/* handle mixed syntax declaration or nested namespaces */
	if (!CG(has_bracketed_namespaces)) {
		if (CG(current_namespace)) {
			/* previous namespace declarations were unbracketed */
			if (with_bracket) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot mix bracketed namespace declarations "
					"with unbracketed namespace declarations");
			}
		}
	} else {
		/* previous namespace declarations were bracketed */
		if (!with_bracket) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot mix bracketed namespace declarations "
				"with unbracketed namespace declarations");
		} else if (CG(current_namespace) || CG(in_namespace)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Namespace declarations cannot be nested");
		}
	}

	if (((!with_bracket && !CG(current_namespace))
		 || (with_bracket && !CG(has_bracketed_namespaces))) && CG(active_op_array)->last > 0
	) {
		/* ignore ZEND_EXT_STMT and ZEND_TICKS */
		uint32_t num = CG(active_op_array)->last;
		while (num > 0 &&
		       (CG(active_op_array)->opcodes[num-1].opcode == ZEND_EXT_STMT ||
		        CG(active_op_array)->opcodes[num-1].opcode == ZEND_TICKS)) {
			--num;
		}
		if (num > 0) {
			zend_error_noreturn(E_COMPILE_ERROR, "Namespace declaration statement has to be "
				"the very first statement in the script");
		}
	}

	if (CG(current_namespace)) {
		zend_string_release(CG(current_namespace));
	}

	if (name_ast) {
		name = zend_ast_get_str(name_ast);

		if (ZEND_FETCH_CLASS_DEFAULT != zend_get_class_fetch_type(name)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use '%s' as namespace name", name->val);
		}

		CG(current_namespace) = zend_string_copy(name);
	} else {
		CG(current_namespace) = NULL;
	}

	zend_reset_import_tables(TSRMLS_C);

	CG(in_namespace) = 1;
	if (with_bracket) {
		CG(has_bracketed_namespaces) = 1;
	}

	if (stmt_ast) {
		zend_compile_top_stmt(stmt_ast TSRMLS_CC);
		zend_end_namespace(TSRMLS_C);
	}
}
/* }}} */

void zend_compile_halt_compiler(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *offset_ast = ast->child[0];
	zend_long offset = Z_LVAL_P(zend_ast_get_zval(offset_ast));

	zend_string *filename, *name;
	const char const_name[] = "__COMPILER_HALT_OFFSET__";

	if (CG(has_bracketed_namespaces) && CG(in_namespace)) {
		zend_error_noreturn(E_COMPILE_ERROR,
			"__HALT_COMPILER() can only be used from the outermost scope");
	}

	filename = zend_get_compiled_filename(TSRMLS_C);
	name = zend_mangle_property_name(const_name, sizeof(const_name) - 1,
		filename->val, filename->len, 0);

	zend_register_long_constant(name->val, name->len, offset, CONST_CS, 0 TSRMLS_CC);
	zend_string_release(name);
}
/* }}} */

static zend_bool zend_try_ct_eval_magic_const(zval *zv, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_op_array *op_array = CG(active_op_array);
	zend_class_entry *ce = CG(active_class_entry);

	switch (ast->attr) {
		case T_LINE:
			ZVAL_LONG(zv, CG(zend_lineno));
			break;
		case T_FILE:
			ZVAL_STR_COPY(zv, CG(compiled_filename));
			break;
		case T_DIR:
		{
			zend_string *filename = CG(compiled_filename);
			zend_string *dirname = zend_string_init(filename->val, filename->len, 0);
			zend_dirname(dirname->val, dirname->len);

			if (strcmp(dirname->val, ".") == 0) {
				dirname = zend_string_realloc(dirname, MAXPATHLEN, 0);
#if HAVE_GETCWD
				VCWD_GETCWD(dirname->val, MAXPATHLEN);
#elif HAVE_GETWD
				VCWD_GETWD(dirname->val);
#endif
			}

			dirname->len = strlen(dirname->val);
			ZVAL_STR(zv, dirname);
			break;
		}
		case T_FUNC_C:
			if (op_array && op_array->function_name) {
				ZVAL_STR_COPY(zv, op_array->function_name);
			} else {
				ZVAL_EMPTY_STRING(zv);
			}
			break;
		case T_METHOD_C:
			if (ce) {
				if (op_array && op_array->function_name) {
					ZVAL_NEW_STR(zv, zend_concat3(ce->name->val, ce->name->len, "::", 2,
						op_array->function_name->val, op_array->function_name->len));
				} else {
					ZVAL_STR_COPY(zv, ce->name);
				}
			} else if (op_array && op_array->function_name) {
				ZVAL_STR_COPY(zv, op_array->function_name);
			} else {
				ZVAL_EMPTY_STRING(zv);
			}
			break;
		case T_CLASS_C:
			if (ce) {
				if (ZEND_CE_IS_TRAIT(ce)) {
					return 0;
				} else {
					ZVAL_STR_COPY(zv, ce->name);
				}
			} else {
				ZVAL_EMPTY_STRING(zv);
			}
			break;
		case T_TRAIT_C:
			if (ce && ZEND_CE_IS_TRAIT(ce)) {
				ZVAL_STR_COPY(zv, ce->name);
			} else {
				ZVAL_EMPTY_STRING(zv);
			}
			break;
		case T_NS_C:
			if (CG(current_namespace)) {
				ZVAL_STR_COPY(zv, CG(current_namespace));
			} else {
				ZVAL_EMPTY_STRING(zv);
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	return 1;
}
/* }}} */

static inline void zend_ct_eval_binary_op(zval *result, uint32_t opcode, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	binary_op_type fn = get_binary_op(opcode);
	fn(result, op1, op2 TSRMLS_CC);
}
/* }}} */

static inline void zend_ct_eval_unary_pm(zval *result, zend_ast_kind kind, zval *op TSRMLS_DC) /* {{{ */
{
	binary_op_type fn = kind == ZEND_AST_UNARY_PLUS
		? add_function : sub_function;

	zval left;
	ZVAL_LONG(&left, 0);
	fn(result, &left, op TSRMLS_CC);
}
/* }}} */

static inline void zend_ct_eval_greater(zval *result, zend_ast_kind kind, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	binary_op_type fn = kind == ZEND_AST_GREATER
		? is_smaller_function : is_smaller_or_equal_function;
	fn(result, op2, op1 TSRMLS_CC);
}
/* }}} */

static zend_bool zend_try_ct_eval_array(zval *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;

	/* First ensure that *all* child nodes are constant and by-val */
	for (i = 0; i < list->children; ++i) {
		zend_ast *elem_ast = list->child[i];
		zend_bool by_ref = elem_ast->attr;
		zend_eval_const_expr(&elem_ast->child[0] TSRMLS_CC);
		zend_eval_const_expr(&elem_ast->child[1] TSRMLS_CC);

		if (by_ref || elem_ast->child[0]->kind != ZEND_AST_ZVAL
			|| (elem_ast->child[1] && elem_ast->child[1]->kind != ZEND_AST_ZVAL)
		) {
			return 0;
		}
	}

	array_init_size(result, list->children);
	for (i = 0; i < list->children; ++i) {
		zend_ast *elem_ast = list->child[i];
		zend_ast *value_ast = elem_ast->child[0];
		zend_ast *key_ast = elem_ast->child[1];

		zval *value = zend_ast_get_zval(value_ast);
		if (Z_REFCOUNTED_P(value)) Z_ADDREF_P(value);

		if (key_ast) {
			zval *key = zend_ast_get_zval(key_ast);
			switch (Z_TYPE_P(key)) {
				case IS_LONG:
					zend_hash_index_update(Z_ARRVAL_P(result), Z_LVAL_P(key), value);
					break;
				case IS_STRING:
					zend_symtable_update(Z_ARRVAL_P(result), Z_STR_P(key), value);
					break;
				case IS_DOUBLE:
					zend_hash_index_update(Z_ARRVAL_P(result),
						zend_dval_to_lval(Z_DVAL_P(key)), value);
					break;
				case IS_FALSE:
					zend_hash_index_update(Z_ARRVAL_P(result), 0, value);
					break;
				case IS_TRUE:
					zend_hash_index_update(Z_ARRVAL_P(result), 1, value);
					break;
				case IS_NULL:
					zend_hash_update(Z_ARRVAL_P(result), STR_EMPTY_ALLOC(), value);
					break;
				default:
					zend_error(E_COMPILE_ERROR, "Illegal offset type");
					break;
			}
		} else {
			zend_hash_next_index_insert(Z_ARRVAL_P(result), value);
		}
	}

	return 1;
}
/* }}} */

void zend_compile_binary_op(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *left_ast = ast->child[0];
	zend_ast *right_ast = ast->child[1];
	uint32_t opcode = ast->attr;

	znode left_node, right_node;
	zend_compile_expr(&left_node, left_ast TSRMLS_CC);
	zend_compile_expr(&right_node, right_ast TSRMLS_CC);

	if (left_node.op_type == IS_CONST && right_node.op_type == IS_CONST) {
		result->op_type = IS_CONST;
		zend_ct_eval_binary_op(&result->u.constant, opcode,
			&left_node.u.constant, &right_node.u.constant TSRMLS_CC);
		zval_ptr_dtor(&left_node.u.constant);
		zval_ptr_dtor(&right_node.u.constant);
		return;
	}

	zend_emit_op_tmp(result, opcode, &left_node, &right_node TSRMLS_CC);
}
/* }}} */

/* We do not use zend_compile_binary_op for this because we want to retain the left-to-right
 * evaluation order. */
void zend_compile_greater(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *left_ast = ast->child[0];
	zend_ast *right_ast = ast->child[1];
	znode left_node, right_node;

	ZEND_ASSERT(ast->kind == ZEND_AST_GREATER || ast->kind == ZEND_AST_GREATER_EQUAL);

	zend_compile_expr(&left_node, left_ast TSRMLS_CC);
	zend_compile_expr(&right_node, right_ast TSRMLS_CC);

	if (left_node.op_type == IS_CONST && right_node.op_type == IS_CONST) {
		result->op_type = IS_CONST;
		zend_ct_eval_greater(&result->u.constant, ast->kind,
			&left_node.u.constant, &right_node.u.constant TSRMLS_CC);
		zval_ptr_dtor(&left_node.u.constant);
		zval_ptr_dtor(&right_node.u.constant);
		return;
	}

	zend_emit_op_tmp(result,
		ast->kind == ZEND_AST_GREATER ? ZEND_IS_SMALLER : ZEND_IS_SMALLER_OR_EQUAL,
		&right_node, &left_node TSRMLS_CC);
}
/* }}} */

void zend_compile_unary_op(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	uint32_t opcode = ast->attr;

	znode expr_node;
	zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);

	zend_emit_op_tmp(result, opcode, &expr_node, NULL TSRMLS_CC);
}
/* }}} */

void zend_compile_unary_pm(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	znode zero_node, expr_node;

	ZEND_ASSERT(ast->kind == ZEND_AST_UNARY_PLUS || ast->kind == ZEND_AST_UNARY_MINUS);

	zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);

	if (expr_node.op_type == IS_CONST) {
		result->op_type = IS_CONST;
		zend_ct_eval_unary_pm(&result->u.constant, ast->kind, &expr_node.u.constant TSRMLS_CC);
		zval_ptr_dtor(&expr_node.u.constant);
		return;
	}

	zero_node.op_type = IS_CONST;
	ZVAL_LONG(&zero_node.u.constant, 0);

	zend_emit_op_tmp(result, ast->kind == ZEND_AST_UNARY_PLUS ? ZEND_ADD : ZEND_SUB,
		&zero_node, &expr_node TSRMLS_CC);
}
/* }}} */

void zend_compile_short_circuiting(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *left_ast = ast->child[0];
	zend_ast *right_ast = ast->child[1];

	znode left_node, right_node;
	zend_op *opline_jmpz, *opline_bool;
	uint32_t opnum_jmpz;

	ZEND_ASSERT(ast->kind == ZEND_AST_AND || ast->kind == ZEND_AST_OR);

	zend_compile_expr(&left_node, left_ast TSRMLS_CC);

	opnum_jmpz = get_next_op_number(CG(active_op_array));
	opline_jmpz = zend_emit_op(NULL, ast->kind == ZEND_AST_AND ? ZEND_JMPZ_EX : ZEND_JMPNZ_EX,
		&left_node, NULL TSRMLS_CC);

	if (left_node.op_type == IS_TMP_VAR) {
		SET_NODE(opline_jmpz->result, &left_node);
	} else {
		opline_jmpz->result.var = get_temporary_variable(CG(active_op_array));
		opline_jmpz->result_type = IS_TMP_VAR;
	}
	GET_NODE(result, opline_jmpz->result);

	zend_compile_expr(&right_node, right_ast TSRMLS_CC);

	opline_bool = zend_emit_op(NULL, ZEND_BOOL, &right_node, NULL TSRMLS_CC);
	SET_NODE(opline_bool->result, result);

	zend_update_jump_target_to_next(opnum_jmpz TSRMLS_CC);
}
/* }}} */

void zend_compile_post_incdec(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *var_ast = ast->child[0];
	ZEND_ASSERT(ast->kind == ZEND_AST_POST_INC || ast->kind == ZEND_AST_POST_DEC);

	if (var_ast->kind == ZEND_AST_PROP) {
		zend_op *opline = zend_compile_prop_common(NULL, var_ast, BP_VAR_RW TSRMLS_CC);
		opline->opcode = ast->kind == ZEND_AST_POST_INC ? ZEND_POST_INC_OBJ : ZEND_POST_DEC_OBJ;
		zend_make_tmp_result(result, opline TSRMLS_CC);
	} else {
		znode var_node;
		zend_compile_var(&var_node, var_ast, BP_VAR_RW TSRMLS_CC);
		zend_emit_op_tmp(result, ast->kind == ZEND_AST_POST_INC ? ZEND_POST_INC : ZEND_POST_DEC,
			&var_node, NULL TSRMLS_CC);
	}
}
/* }}} */

void zend_compile_pre_incdec(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *var_ast = ast->child[0];
	ZEND_ASSERT(ast->kind == ZEND_AST_PRE_INC || ast->kind == ZEND_AST_PRE_DEC);

	if (var_ast->kind == ZEND_AST_PROP) {
		zend_op *opline = zend_compile_prop_common(result, var_ast, BP_VAR_RW TSRMLS_CC);
		opline->opcode = ast->kind == ZEND_AST_PRE_INC ? ZEND_PRE_INC_OBJ : ZEND_PRE_DEC_OBJ;
	} else {
		znode var_node;
		zend_compile_var(&var_node, var_ast, BP_VAR_RW TSRMLS_CC);
		zend_emit_op(result, ast->kind == ZEND_AST_PRE_INC ? ZEND_PRE_INC : ZEND_PRE_DEC,
			&var_node, NULL TSRMLS_CC);
	}
}
/* }}} */

void zend_compile_cast(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	znode expr_node;
	zend_op *opline;

	zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);

	opline = zend_emit_op_tmp(result, ZEND_CAST, &expr_node, NULL TSRMLS_CC);
	opline->extended_value = ast->attr;
}
/* }}} */

static void zend_compile_shorthand_conditional(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *cond_ast = ast->child[0];
	zend_ast *false_ast = ast->child[2];

	znode cond_node, false_node;
	zend_op *opline_jmp_set, *opline_qm_assign;
	uint32_t opnum_jmp_set;

	ZEND_ASSERT(ast->child[1] == NULL);

	zend_compile_expr(&cond_node, cond_ast TSRMLS_CC);

	opnum_jmp_set = get_next_op_number(CG(active_op_array));
	zend_emit_op_tmp(result, ZEND_JMP_SET, &cond_node, NULL TSRMLS_CC);

	zend_compile_expr(&false_node, false_ast TSRMLS_CC);

	opline_jmp_set = &CG(active_op_array)->opcodes[opnum_jmp_set];
	opline_jmp_set->op2.opline_num = get_next_op_number(CG(active_op_array)) + 1;
	opline_qm_assign = zend_emit_op_tmp(NULL, ZEND_QM_ASSIGN, &false_node, NULL TSRMLS_CC);
	SET_NODE(opline_qm_assign->result, result);
}
/* }}} */

void zend_compile_conditional(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *cond_ast = ast->child[0];
	zend_ast *true_ast = ast->child[1];
	zend_ast *false_ast = ast->child[2];

	znode cond_node, true_node, false_node;
	zend_op *opline_qm_assign1, *opline_qm_assign2;
	uint32_t opnum_jmpz, opnum_jmp, opnum_qm_assign1;

	if (!true_ast) {
		zend_compile_shorthand_conditional(result, ast TSRMLS_CC);
		return;
	}
	
	zend_compile_expr(&cond_node, cond_ast TSRMLS_CC);

	opnum_jmpz = zend_emit_cond_jump(ZEND_JMPZ, &cond_node, 0 TSRMLS_CC);

	zend_compile_expr(&true_node, true_ast TSRMLS_CC);

	opnum_qm_assign1 = get_next_op_number(CG(active_op_array));
	zend_emit_op_tmp(result, ZEND_QM_ASSIGN, &true_node, NULL TSRMLS_CC);

	opnum_jmp = zend_emit_jump(0 TSRMLS_CC);

	zend_update_jump_target_to_next(opnum_jmpz TSRMLS_CC);

	zend_compile_expr(&false_node, false_ast TSRMLS_CC);

	opline_qm_assign1 = &CG(active_op_array)->opcodes[opnum_qm_assign1];
	opline_qm_assign2 = zend_emit_op(NULL, opline_qm_assign1->opcode, &false_node, NULL TSRMLS_CC);
	SET_NODE(opline_qm_assign2->result, result);

	zend_update_jump_target_to_next(opnum_jmp TSRMLS_CC);
}
/* }}} */

void zend_compile_coalesce(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	zend_ast *default_ast = ast->child[1];

	znode expr_node, default_node;
	zend_op *opline;
	uint32_t opnum;

	zend_compile_var(&expr_node, expr_ast, BP_VAR_IS TSRMLS_CC);

	opnum = get_next_op_number(CG(active_op_array));
	zend_emit_op_tmp(result, ZEND_COALESCE, &expr_node, NULL TSRMLS_CC);

	zend_compile_expr(&default_node, default_ast TSRMLS_CC);

	opline = zend_emit_op_tmp(NULL, ZEND_QM_ASSIGN, &default_node, NULL TSRMLS_CC);
	SET_NODE(opline->result, result);

	opline = &CG(active_op_array)->opcodes[opnum];
	opline->op2.opline_num = get_next_op_number(CG(active_op_array));
}
/* }}} */

void zend_compile_print(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];

	znode expr_node;
	zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);

	zend_emit_op_tmp(result, ZEND_PRINT, &expr_node, NULL TSRMLS_CC);
}
/* }}} */

void zend_compile_exit(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];

	if (expr_ast) {
		znode expr_node;
		zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);
		zend_emit_op(NULL, ZEND_EXIT, &expr_node, NULL TSRMLS_CC);
	} else {
		zend_emit_op(NULL, ZEND_EXIT, NULL, NULL TSRMLS_CC);
	}

	result->op_type = IS_CONST;
	ZVAL_BOOL(&result->u.constant, 1);
}
/* }}} */

void zend_compile_yield(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *value_ast = ast->child[0];
	zend_ast *key_ast = ast->child[1];

	znode value_node, key_node;
	znode *value_node_ptr = NULL, *key_node_ptr = NULL;
	zend_op *opline;
	zend_bool returns_by_ref = (CG(active_op_array)->fn_flags & ZEND_ACC_RETURN_REFERENCE) != 0;

	if (!CG(active_op_array)->function_name) {
		zend_error_noreturn(E_COMPILE_ERROR,
			"The \"yield\" expression can only be used inside a function");
	}

	CG(active_op_array)->fn_flags |= ZEND_ACC_GENERATOR;

	if (key_ast) {
		zend_compile_expr(&key_node, key_ast TSRMLS_CC);
		key_node_ptr = &key_node;
	}

	if (value_ast) {
		if (returns_by_ref && zend_is_variable(value_ast) && !zend_is_call(value_ast)) {
			zend_compile_var(&value_node, value_ast, BP_VAR_REF TSRMLS_CC);
		} else {
			zend_compile_expr(&value_node, value_ast TSRMLS_CC);
		}
		value_node_ptr = &value_node;
	}

	opline = zend_emit_op(result, ZEND_YIELD, value_node_ptr, key_node_ptr TSRMLS_CC);

	if (value_ast && returns_by_ref && zend_is_call(value_ast)) {
		opline->extended_value = ZEND_RETURNS_FUNCTION;
	}
}
/* }}} */

void zend_compile_instanceof(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *obj_ast = ast->child[0];
	zend_ast *class_ast = ast->child[1];

	znode obj_node, class_node;
	zend_op *opline;

	zend_compile_expr(&obj_node, obj_ast TSRMLS_CC);
	if (obj_node.op_type == IS_CONST) {
		zend_error_noreturn(E_COMPILE_ERROR,
			"instanceof expects an object instance, constant given");
	}

	opline = zend_compile_class_ref(&class_node, class_ast TSRMLS_CC);
	opline->extended_value |= ZEND_FETCH_CLASS_NO_AUTOLOAD;

	zend_emit_op_tmp(result, ZEND_INSTANCEOF, &obj_node, &class_node TSRMLS_CC);
}
/* }}} */

void zend_compile_include_or_eval(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	znode expr_node;
	zend_op *opline;

	zend_do_extended_fcall_begin(TSRMLS_C);
	zend_compile_expr(&expr_node, expr_ast TSRMLS_CC);

	opline = zend_emit_op(result, ZEND_INCLUDE_OR_EVAL, &expr_node, NULL TSRMLS_CC);
	opline->extended_value = ast->attr;

	zend_do_extended_fcall_end(TSRMLS_C);
}
/* }}} */

void zend_compile_isset_or_empty(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *var_ast = ast->child[0];

	znode var_node;
	zend_op *opline;

	ZEND_ASSERT(ast->kind == ZEND_AST_ISSET || ast->kind == ZEND_AST_EMPTY);

	if (!zend_is_variable(var_ast) || zend_is_call(var_ast)) {
		if (ast->kind == ZEND_AST_EMPTY) {
			/* empty(expr) can be transformed to !expr */
			zend_ast *not_ast = zend_ast_create_ex(ZEND_AST_UNARY_OP, ZEND_BOOL_NOT, var_ast);
			zend_compile_expr(result, not_ast TSRMLS_CC);
			return;
		} else {
			zend_error_noreturn(E_COMPILE_ERROR,
				"Cannot use isset() on the result of an expression "
				"(you can use \"null !== expression\" instead)");
		}
	}

	switch (var_ast->kind) {
		case ZEND_AST_VAR:
			if (zend_try_compile_cv(&var_node, var_ast TSRMLS_CC) == SUCCESS) {
				opline = zend_emit_op(result, ZEND_ISSET_ISEMPTY_VAR, &var_node, NULL TSRMLS_CC);
				opline->extended_value = ZEND_FETCH_LOCAL | ZEND_QUICK_SET;
			} else {
				opline = zend_compile_simple_var_no_cv(result, var_ast, BP_VAR_IS TSRMLS_CC);
				opline->opcode = ZEND_ISSET_ISEMPTY_VAR;
			}
			break;
		case ZEND_AST_DIM:
			opline = zend_compile_dim_common(result, var_ast, BP_VAR_IS TSRMLS_CC);
			opline->opcode = ZEND_ISSET_ISEMPTY_DIM_OBJ;
			break;
		case ZEND_AST_PROP:
			opline = zend_compile_prop_common(result, var_ast, BP_VAR_IS TSRMLS_CC);
			opline->opcode = ZEND_ISSET_ISEMPTY_PROP_OBJ;
			break;
		case ZEND_AST_STATIC_PROP:
			opline = zend_compile_static_prop_common(result, var_ast, BP_VAR_IS TSRMLS_CC);
			opline->opcode = ZEND_ISSET_ISEMPTY_VAR;
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	result->op_type = opline->result_type = IS_TMP_VAR;
	opline->extended_value |= ast->kind == ZEND_AST_ISSET ? ZEND_ISSET : ZEND_ISEMPTY;
}
/* }}} */

void zend_compile_silence(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	znode silence_node;
	uint32_t opline_num;
	zend_op *begin_silence, *end_silence;

	opline_num = get_next_op_number(CG(active_op_array));
	begin_silence = zend_emit_op_tmp(&silence_node, ZEND_BEGIN_SILENCE, NULL, NULL TSRMLS_CC);
	/* pair BEGIN_SILENCE and END_SILENCE opcodes */
	begin_silence->op2.num = opline_num;

	if (expr_ast->kind == ZEND_AST_VAR) {
		/* For @$var we need to force a FETCH instruction, otherwise the CV access will
		 * happen outside the silenced section. */
		zend_compile_simple_var_no_cv(result, expr_ast, BP_VAR_R TSRMLS_CC);
	} else {
		zend_compile_expr(result, expr_ast TSRMLS_CC);
	}

	end_silence = zend_emit_op(NULL, ZEND_END_SILENCE, &silence_node, NULL TSRMLS_CC);
	/* pair BEGIN_SILENCE and END_SILENCE opcodes */
	end_silence->op2.num = opline_num;
}
/* }}} */

void zend_compile_shell_exec(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];

	zval fn_name;
	zend_ast *name_ast, *args_ast, *call_ast;

	ZVAL_STRING(&fn_name, "shell_exec");
	name_ast = zend_ast_create_zval(&fn_name);
	args_ast = zend_ast_create_list(1, ZEND_AST_ARG_LIST, expr_ast);
	call_ast = zend_ast_create(ZEND_AST_CALL, name_ast, args_ast);

	zend_compile_expr(result, call_ast TSRMLS_CC);

	zval_ptr_dtor(&fn_name);
}
/* }}} */

void zend_compile_array(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	zend_op *opline;
	uint32_t i, opnum_init;
	zend_bool packed = 1;

	if (zend_try_ct_eval_array(&result->u.constant, ast TSRMLS_CC)) {
		result->op_type = IS_CONST;
		return;
	}

	opnum_init = get_next_op_number(CG(active_op_array));

	for (i = 0; i < list->children; ++i) {
		zend_ast *elem_ast = list->child[i];
		zend_ast *value_ast = elem_ast->child[0];
		zend_ast *key_ast = elem_ast->child[1];
		zend_bool by_ref = elem_ast->attr;

		znode value_node, key_node, *key_node_ptr = NULL;

		if (key_ast) {
			zend_compile_expr(&key_node, key_ast TSRMLS_CC);
			zend_handle_numeric_op(&key_node TSRMLS_CC);
			key_node_ptr = &key_node;
		}

		if (by_ref) {
			zend_ensure_writable_variable(value_ast);
			zend_compile_var(&value_node, value_ast, BP_VAR_W TSRMLS_CC);
		} else {
			zend_compile_expr(&value_node, value_ast TSRMLS_CC);
		}

		if (i == 0) {
			opline = zend_emit_op_tmp(result, ZEND_INIT_ARRAY, &value_node, key_node_ptr TSRMLS_CC);
			opline->extended_value = list->children << ZEND_ARRAY_SIZE_SHIFT;
		} else {
			opline = zend_emit_op(NULL, ZEND_ADD_ARRAY_ELEMENT,
				&value_node, key_node_ptr TSRMLS_CC);
			SET_NODE(opline->result, result);
		}
		opline->extended_value |= by_ref;

		if (key_ast && key_node.op_type == IS_CONST && Z_TYPE(key_node.u.constant) == IS_STRING) {
			packed = 0;
		}
	}

	/* Handle empty array */
	if (!list->children) {
		zend_emit_op_tmp(result, ZEND_INIT_ARRAY, NULL, NULL TSRMLS_CC);
	}

	/* Add a flag to INIT_ARRAY if we know this array cannot be packed */
	if (!packed) {
		opline = &CG(active_op_array)->opcodes[opnum_init];
		opline->extended_value |= ZEND_ARRAY_NOT_PACKED;
	}
}
/* }}} */

void zend_compile_const(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *name_ast = ast->child[0];

	zend_op *opline;

	zend_bool is_fully_qualified;
	zend_string *orig_name = zend_ast_get_str(name_ast);
	zend_string *resolved_name = zend_resolve_const_name(
		orig_name, name_ast->attr, &is_fully_qualified TSRMLS_CC);

	if (zend_constant_ct_subst(&result->u.constant, resolved_name, 1 TSRMLS_CC)) {
		result->op_type = IS_CONST;
		zend_string_release(resolved_name);
		return;
	}

	opline = zend_emit_op_tmp(result, ZEND_FETCH_CONSTANT, NULL, NULL TSRMLS_CC);
	opline->op2_type = IS_CONST;

	if (is_fully_qualified) {
		opline->op2.constant = zend_add_const_name_literal(
			CG(active_op_array), resolved_name, 0 TSRMLS_CC);
	} else {
		opline->extended_value = IS_CONSTANT_UNQUALIFIED;
		if (CG(current_namespace)) {
			opline->extended_value |= IS_CONSTANT_IN_NAMESPACE;
			opline->op2.constant = zend_add_const_name_literal(
				CG(active_op_array), resolved_name, 1 TSRMLS_CC);
		} else {
			opline->op2.constant = zend_add_const_name_literal(
				CG(active_op_array), resolved_name, 0 TSRMLS_CC);
		}
	}
	zend_alloc_cache_slot(opline->op2.constant TSRMLS_CC);
}
/* }}} */

void zend_compile_class_const(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *class_ast = ast->child[0];
	zend_ast *const_ast = ast->child[1];

	znode class_node, const_node;
	zend_op *opline;

	if (zend_is_const_default_class_ref(class_ast)) {
		class_node.op_type = IS_CONST;
		ZVAL_STR(&class_node.u.constant, zend_resolve_class_name_ast(class_ast TSRMLS_CC));
	} else {
		zend_compile_class_ref(&class_node, class_ast TSRMLS_CC);
	}

	zend_compile_expr(&const_node, const_ast TSRMLS_CC);

	opline = zend_emit_op_tmp(result, ZEND_FETCH_CONSTANT, NULL, &const_node TSRMLS_CC);

	zend_set_class_name_op1(opline, &class_node TSRMLS_CC);

	if (opline->op1_type == IS_CONST) {
		zend_alloc_cache_slot(opline->op2.constant TSRMLS_CC);
	} else {
		zend_alloc_polymorphic_cache_slot(opline->op2.constant TSRMLS_CC);
	}
}
/* }}} */

void zend_compile_resolve_class_name(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *name_ast = ast->child[0];
	uint32_t fetch_type = zend_get_class_fetch_type(zend_ast_get_str(name_ast));

	switch (fetch_type) {
		case ZEND_FETCH_CLASS_SELF:
			if (!CG(active_class_entry)) {
				zend_error_noreturn(E_COMPILE_ERROR,
					"Cannot access self::class when no class scope is active");
			}
			result->op_type = IS_CONST;
			ZVAL_STR_COPY(&result->u.constant, CG(active_class_entry)->name);
			break;
        case ZEND_FETCH_CLASS_STATIC:
        case ZEND_FETCH_CLASS_PARENT:
			if (!CG(active_class_entry)) {
				zend_error_noreturn(E_COMPILE_ERROR,
					"Cannot access %s::class when no class scope is active",
					fetch_type == ZEND_FETCH_CLASS_STATIC ? "static" : "parent");
			} else {
				zval class_str_zv;
				zend_ast *class_str_ast, *class_const_ast;

				ZVAL_STRING(&class_str_zv, "class");
				class_str_ast = zend_ast_create_zval(&class_str_zv);
				class_const_ast = zend_ast_create(
					ZEND_AST_CLASS_CONST, name_ast, class_str_ast);

				zend_compile_expr(result, class_const_ast TSRMLS_CC);

				zval_ptr_dtor(&class_str_zv);
			}
			break;
		case ZEND_FETCH_CLASS_DEFAULT:
			result->op_type = IS_CONST;
			ZVAL_STR(&result->u.constant, zend_resolve_class_name_ast(name_ast TSRMLS_CC));
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

void zend_compile_encaps_list(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;

	ZEND_ASSERT(list->children > 0);

	result->op_type = IS_TMP_VAR;
	result->u.op.var = get_temporary_variable(CG(active_op_array));

	for (i = 0; i < list->children; ++i) {
		zend_ast *elem_ast = list->child[i];
		znode elem_node;
		zend_op *opline;

		zend_compile_expr(&elem_node, elem_ast TSRMLS_CC);

		if (elem_ast->kind == ZEND_AST_ZVAL) {
			zval *zv = &elem_node.u.constant;
			ZEND_ASSERT(Z_TYPE_P(zv) == IS_STRING);

			if (Z_STRLEN_P(zv) > 1) {
				opline = get_next_op(CG(active_op_array) TSRMLS_CC);
				opline->opcode = ZEND_ADD_STRING;
			} else if (Z_STRLEN_P(zv) == 1) {
				char ch = *Z_STRVAL_P(zv);
				zend_string_release(Z_STR_P(zv));
				ZVAL_LONG(zv, ch);

				opline = get_next_op(CG(active_op_array) TSRMLS_CC);
				opline->opcode = ZEND_ADD_CHAR;
			} else {
				/* String can be empty after a variable at the end of a heredoc */
				zend_string_release(Z_STR_P(zv));
				continue;
			}
		} else {
			opline = get_next_op(CG(active_op_array) TSRMLS_CC);
			opline->opcode = ZEND_ADD_VAR;
			ZEND_ASSERT(elem_node.op_type != IS_CONST);
		}

		if (i == 0) {
			SET_UNUSED(opline->op1);
		} else {
			SET_NODE(opline->op1, result);
		}
		SET_NODE(opline->op2, &elem_node);
		SET_NODE(opline->result, result);
	}
}
/* }}} */

void zend_compile_magic_const(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce = CG(active_class_entry);

	if (zend_try_ct_eval_magic_const(&result->u.constant, ast TSRMLS_CC)) {
		result->op_type = IS_CONST;
		return;
	}

	ZEND_ASSERT(ast->attr == T_CLASS_C && ce && ZEND_CE_IS_TRAIT(ce));

	{
		zend_ast *const_ast = zend_ast_create(ZEND_AST_CONST,
			zend_ast_create_zval_from_str(zend_string_init("__CLASS__", sizeof("__CLASS__") - 1, 0)));
		zend_compile_const(result, const_ast TSRMLS_CC);
		zend_ast_destroy(const_ast);
	}
}
/* }}} */

zend_bool zend_is_allowed_in_const_expr(zend_ast_kind kind) /* {{{ */
{
	return kind == ZEND_AST_ZVAL || kind == ZEND_AST_BINARY_OP
		|| kind == ZEND_AST_GREATER || kind == ZEND_AST_GREATER_EQUAL
		|| kind == ZEND_AST_AND || kind == ZEND_AST_OR
		|| kind == ZEND_AST_UNARY_OP
		|| kind == ZEND_AST_UNARY_PLUS || kind == ZEND_AST_UNARY_MINUS
		|| kind == ZEND_AST_CONDITIONAL || kind == ZEND_AST_DIM
		|| kind == ZEND_AST_ARRAY || kind == ZEND_AST_ARRAY_ELEM
		|| kind == ZEND_AST_CONST || kind == ZEND_AST_CLASS_CONST
		|| kind == ZEND_AST_RESOLVE_CLASS_NAME || kind == ZEND_AST_MAGIC_CONST;
}
/* }}} */

void zend_compile_const_expr_class_const(zend_ast **ast_ptr TSRMLS_DC) /* {{{ */
{
	zend_ast *ast = *ast_ptr;
	zend_ast *class_ast = ast->child[0];
	zend_ast *const_ast = ast->child[1];
	zend_string *class_name;
	zend_string *const_name = zend_ast_get_str(const_ast);
	zval result;
	int fetch_type;

	if (class_ast->kind != ZEND_AST_ZVAL) {
		zend_error_noreturn(E_COMPILE_ERROR,
			"Dynamic class names are not allowed in compile-time class constant references");
	}

	class_name = zend_ast_get_str(class_ast);
	fetch_type = zend_get_class_fetch_type(class_name);

	if (ZEND_FETCH_CLASS_STATIC == fetch_type) {
		zend_error_noreturn(E_COMPILE_ERROR,
			"\"static::\" is not allowed in compile-time constants");
	}
	
	if (ZEND_FETCH_CLASS_DEFAULT == fetch_type) {
		class_name = zend_resolve_class_name_ast(class_ast TSRMLS_CC);
	} else {
		zend_string_addref(class_name);
	}

	Z_STR(result) = zend_concat3(
		class_name->val, class_name->len, "::", 2, const_name->val, const_name->len);

	Z_TYPE_INFO(result) = IS_CONSTANT_EX;
	Z_CONST_FLAGS(result) = fetch_type;

	zend_ast_destroy(ast);
	zend_string_release(class_name);

	*ast_ptr = zend_ast_create_zval(&result);
}
/* }}} */

void zend_compile_const_expr_const(zend_ast **ast_ptr TSRMLS_DC) /* {{{ */
{
	zend_ast *ast = *ast_ptr;
	zend_ast *name_ast = ast->child[0];
	zend_string *orig_name = zend_ast_get_str(name_ast);
	zend_bool is_fully_qualified;

	zval result, resolved_name;

	if (zend_constant_ct_subst(&result, orig_name, 0 TSRMLS_CC)) {
		zend_ast_destroy(ast);
		*ast_ptr = zend_ast_create_zval(&result);
		return;
	}

	ZVAL_STR(&resolved_name, zend_resolve_const_name(
		orig_name, name_ast->attr, &is_fully_qualified TSRMLS_CC));

	Z_TYPE_INFO(resolved_name) = IS_CONSTANT_EX;
	if (!is_fully_qualified) {
		Z_CONST_FLAGS(resolved_name) = IS_CONSTANT_UNQUALIFIED;
	}

	zend_ast_destroy(ast);
	*ast_ptr = zend_ast_create_zval(&resolved_name);
}
/* }}} */

void zend_compile_const_expr_resolve_class_name(zend_ast **ast_ptr TSRMLS_DC) /* {{{ */
{
	zend_ast *ast = *ast_ptr;
	zend_ast *name_ast = ast->child[0];
	uint32_t fetch_type = zend_get_class_fetch_type(zend_ast_get_str(name_ast));
	zval result;

	switch (fetch_type) {
		case ZEND_FETCH_CLASS_SELF:
			if (!CG(active_class_entry)) {
				zend_error_noreturn(E_COMPILE_ERROR,
					"Cannot access self::class when no class scope is active");
			}
			ZVAL_STR_COPY(&result, CG(active_class_entry)->name);
			break;
        case ZEND_FETCH_CLASS_STATIC:
        case ZEND_FETCH_CLASS_PARENT:
			zend_error_noreturn(E_COMPILE_ERROR,
				"%s::class cannot be used for compile-time class name resolution",
				fetch_type == ZEND_FETCH_CLASS_STATIC ? "static" : "parent"
			);
			break;
		case ZEND_FETCH_CLASS_DEFAULT:
			ZVAL_STR(&result, zend_resolve_class_name_ast(name_ast TSRMLS_CC));
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	zend_ast_destroy(ast);
	*ast_ptr = zend_ast_create_zval(&result);
}
/* }}} */

void zend_compile_const_expr_magic_const(zend_ast **ast_ptr TSRMLS_DC) /* {{{ */
{
	zend_ast *ast = *ast_ptr;
	zend_class_entry *ce = CG(active_class_entry);

	/* Other cases already resolved by constant folding */
	ZEND_ASSERT(ast->attr == T_CLASS_C && ce && ZEND_CE_IS_TRAIT(ce));

	{
		zval const_zv;
		ZVAL_STRING(&const_zv, "__CLASS__");
		Z_TYPE_INFO(const_zv) = IS_CONSTANT_EX;

		zend_ast_destroy(ast);
		*ast_ptr = zend_ast_create_zval(&const_zv);
	}
}
/* }}} */

void zend_compile_const_expr(zend_ast **ast_ptr TSRMLS_DC) /* {{{ */
{
	zend_ast *ast = *ast_ptr;
	if (ast == NULL || ast->kind == ZEND_AST_ZVAL) {
		return;
	}

	if (!zend_is_allowed_in_const_expr(ast->kind)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Constant expression contains invalid operations");
	}

	switch (ast->kind) {
		case ZEND_AST_CLASS_CONST:
			zend_compile_const_expr_class_const(ast_ptr TSRMLS_CC);
			break;
		case ZEND_AST_CONST:
			zend_compile_const_expr_const(ast_ptr TSRMLS_CC);
			break;
		case ZEND_AST_RESOLVE_CLASS_NAME:
			zend_compile_const_expr_resolve_class_name(ast_ptr TSRMLS_CC);
			break;
		case ZEND_AST_MAGIC_CONST:
			zend_compile_const_expr_magic_const(ast_ptr TSRMLS_CC);
			break;
		default:
			zend_ast_apply(ast, zend_compile_const_expr TSRMLS_CC);
			break;
	}
}
/* }}} */

void zend_const_expr_to_zval(zval *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	zend_ast *orig_ast = ast;
	zend_eval_const_expr(&ast TSRMLS_CC);
	zend_compile_const_expr(&ast TSRMLS_CC);
	if (ast->kind == ZEND_AST_ZVAL) {
		ZVAL_COPY_VALUE(result, zend_ast_get_zval(ast));

		/* Kill this branch of the original AST, as it was already destroyed.
		 * It would be nice to find a better solution to this problem in the
		 * future. */
		orig_ast->kind = 0;
	} else {
		ZVAL_NEW_AST(result, zend_ast_copy(ast));
	}
}
/* }}} */

/* Same as compile_stmt, but with early binding */
void zend_compile_top_stmt(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	if (!ast) {
		return;
	}

	if (ast->kind == ZEND_AST_STMT_LIST) {
		zend_ast_list *list = zend_ast_get_list(ast);
		uint32_t i;
		for (i = 0; i < list->children; ++i) {
			zend_compile_top_stmt(list->child[i] TSRMLS_CC);
		}
		return;
	}

	zend_compile_stmt(ast TSRMLS_CC);

	if (ast->kind != ZEND_AST_NAMESPACE && ast->kind != ZEND_AST_HALT_COMPILER) {
		zend_verify_namespace(TSRMLS_C);
	}
	if (ast->kind == ZEND_AST_FUNC_DECL || ast->kind == ZEND_AST_CLASS) {
		CG(zend_lineno) = ((zend_ast_decl *) ast)->end_lineno;
		zend_do_early_binding(TSRMLS_C);
	}
}
/* }}} */

void zend_compile_stmt(zend_ast *ast TSRMLS_DC) /* {{{ */
{
	if (!ast) {
		return;
	}

	CG(zend_lineno) = ast->lineno;

	switch (ast->kind) {
		case ZEND_AST_STMT_LIST:
			zend_compile_stmt_list(ast TSRMLS_CC);
			break;
		case ZEND_AST_GLOBAL:
			zend_compile_global_var(ast TSRMLS_CC);
			break;
		case ZEND_AST_STATIC:
			zend_compile_static_var(ast TSRMLS_CC);
			break;
		case ZEND_AST_UNSET:
			zend_compile_unset(ast TSRMLS_CC);
			break;
		case ZEND_AST_RETURN:
			zend_compile_return(ast TSRMLS_CC);
			break;
		case ZEND_AST_ECHO:
			zend_compile_echo(ast TSRMLS_CC);
			break;
		case ZEND_AST_THROW:
			zend_compile_throw(ast TSRMLS_CC);
			break;
		case ZEND_AST_BREAK:
		case ZEND_AST_CONTINUE:
			zend_compile_break_continue(ast TSRMLS_CC);
			break;
		case ZEND_AST_GOTO:
			zend_compile_goto(ast TSRMLS_CC);
			break;
		case ZEND_AST_LABEL:
			zend_compile_label(ast TSRMLS_CC);
			break;
		case ZEND_AST_WHILE:
			zend_compile_while(ast TSRMLS_CC);
			break;
		case ZEND_AST_DO_WHILE:
			zend_compile_do_while(ast TSRMLS_CC);
			break;
		case ZEND_AST_FOR:
			zend_compile_for(ast TSRMLS_CC);
			break;
		case ZEND_AST_FOREACH:
			zend_compile_foreach(ast TSRMLS_CC);
			break;
		case ZEND_AST_IF:
			zend_compile_if(ast TSRMLS_CC);
			break;
		case ZEND_AST_SWITCH:
			zend_compile_switch(ast TSRMLS_CC);
			break;
		case ZEND_AST_TRY:
			zend_compile_try(ast TSRMLS_CC);
			break;
		case ZEND_AST_DECLARE:
			zend_compile_declare(ast TSRMLS_CC);
			break;
		case ZEND_AST_FUNC_DECL:
		case ZEND_AST_METHOD:
			zend_compile_func_decl(NULL, ast TSRMLS_CC);
			break;
		case ZEND_AST_PROP_DECL:
			zend_compile_prop_decl(ast TSRMLS_CC);
			break;
		case ZEND_AST_CLASS_CONST_DECL:
			zend_compile_class_const_decl(ast TSRMLS_CC);
			break;
		case ZEND_AST_USE_TRAIT:
			zend_compile_use_trait(ast TSRMLS_CC);
			break;
		case ZEND_AST_CLASS:
			zend_compile_class_decl(ast TSRMLS_CC);
			break;
		case ZEND_AST_USE:
			zend_compile_use(ast TSRMLS_CC);
			break;
		case ZEND_AST_CONST_DECL:
			zend_compile_const_decl(ast TSRMLS_CC);
			break;
		case ZEND_AST_NAMESPACE:
			zend_compile_namespace(ast TSRMLS_CC);
			break;
		case ZEND_AST_HALT_COMPILER:
			zend_compile_halt_compiler(ast TSRMLS_CC);
			break;
		default:
		{
			znode result;
			zend_compile_expr(&result, ast TSRMLS_CC);
			zend_do_free(&result TSRMLS_CC);
		}
	}

	if (Z_LVAL(CG(declarables).ticks) && !zend_is_unticked_stmt(ast)) {
		zend_emit_tick(TSRMLS_C);
	}
}
/* }}} */

void zend_compile_expr(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	/* CG(zend_lineno) = ast->lineno; */
	CG(zend_lineno) = zend_ast_get_lineno(ast);

	switch (ast->kind) {
		case ZEND_AST_ZVAL:
			ZVAL_COPY(&result->u.constant, zend_ast_get_zval(ast));
			result->op_type = IS_CONST;
			return;
		case ZEND_AST_ZNODE:
			*result = *zend_ast_get_znode(ast);
			return;
		case ZEND_AST_VAR:
		case ZEND_AST_DIM:
		case ZEND_AST_PROP:
		case ZEND_AST_STATIC_PROP:
		case ZEND_AST_CALL:
		case ZEND_AST_METHOD_CALL:
		case ZEND_AST_STATIC_CALL:
			zend_compile_var(result, ast, BP_VAR_R TSRMLS_CC);
			return;
		case ZEND_AST_ASSIGN:
			zend_compile_assign(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_ASSIGN_REF:
			zend_compile_assign_ref(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_NEW:
			zend_compile_new(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_CLONE:
			zend_compile_clone(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_ASSIGN_OP:
			zend_compile_compound_assign(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_BINARY_OP:
			zend_compile_binary_op(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_GREATER:
		case ZEND_AST_GREATER_EQUAL:
			zend_compile_greater(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_UNARY_OP:
			zend_compile_unary_op(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_UNARY_PLUS:
		case ZEND_AST_UNARY_MINUS:
			zend_compile_unary_pm(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_AND:
		case ZEND_AST_OR:
			zend_compile_short_circuiting(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_POST_INC:
		case ZEND_AST_POST_DEC:
			zend_compile_post_incdec(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_PRE_INC:
		case ZEND_AST_PRE_DEC:
			zend_compile_pre_incdec(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_CAST:
			zend_compile_cast(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_CONDITIONAL:
			zend_compile_conditional(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_COALESCE:
			zend_compile_coalesce(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_PRINT:
			zend_compile_print(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_EXIT:
			zend_compile_exit(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_YIELD:
			zend_compile_yield(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_INSTANCEOF:
			zend_compile_instanceof(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_INCLUDE_OR_EVAL:
			zend_compile_include_or_eval(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_ISSET:
		case ZEND_AST_EMPTY:
			zend_compile_isset_or_empty(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_SILENCE:
			zend_compile_silence(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_SHELL_EXEC:
			zend_compile_shell_exec(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_ARRAY:
			zend_compile_array(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_CONST:
			zend_compile_const(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_CLASS_CONST:
			zend_compile_class_const(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_RESOLVE_CLASS_NAME:
			zend_compile_resolve_class_name(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_ENCAPS_LIST:
			zend_compile_encaps_list(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_MAGIC_CONST:
			zend_compile_magic_const(result, ast TSRMLS_CC);
			return;
		case ZEND_AST_CLOSURE:
			zend_compile_func_decl(result, ast TSRMLS_CC);
			return;
		default:
			ZEND_ASSERT(0 /* not supported */);
	}
}
/* }}} */

void zend_compile_var(znode *result, zend_ast *ast, uint32_t type TSRMLS_DC) /* {{{ */
{
	switch (ast->kind) {
		case ZEND_AST_VAR:
			zend_compile_simple_var(result, ast, type TSRMLS_CC);
			return;
		case ZEND_AST_DIM:
			zend_compile_dim(result, ast, type TSRMLS_CC);
			return;
		case ZEND_AST_PROP:
			zend_compile_prop(result, ast, type TSRMLS_CC);
			return;
		case ZEND_AST_STATIC_PROP:
			zend_compile_static_prop(result, ast, type TSRMLS_CC);
			return;
		case ZEND_AST_CALL:
			zend_compile_call(result, ast, type TSRMLS_CC);
			return;
		case ZEND_AST_METHOD_CALL:
			zend_compile_method_call(result, ast, type TSRMLS_CC);
			return;
		case ZEND_AST_STATIC_CALL:
			zend_compile_static_call(result, ast, type TSRMLS_CC);
			return;
		case ZEND_AST_ZNODE:
			*result = *zend_ast_get_znode(ast);
			return;
		default:
			if (type == BP_VAR_W || type == BP_VAR_REF
				|| type == BP_VAR_RW || type == BP_VAR_UNSET
			) {
				/* For BC reasons =& new Foo is allowed */
				if (type != BP_VAR_REF || ast->kind != ZEND_AST_NEW) {
					zend_error_noreturn(E_COMPILE_ERROR,
						"Cannot use temporary expression in write context");
				}
			}

			zend_compile_expr(result, ast TSRMLS_CC);
			return;
	}
}
/* }}} */

void zend_delayed_compile_var(znode *result, zend_ast *ast, uint32_t type TSRMLS_DC) /* {{{ */
{
	zend_op *opline;
	switch (ast->kind) {
		case ZEND_AST_DIM:
			opline = zend_delayed_compile_dim(result, ast, type TSRMLS_CC);
			zend_adjust_for_fetch_type(opline, type);
			return;
		case ZEND_AST_PROP:
			opline = zend_delayed_compile_prop(result, ast, type TSRMLS_CC);
			zend_adjust_for_fetch_type(opline, type);
			return;
		default:
			zend_compile_var(result, ast, type TSRMLS_CC);
			return;
	}
}
/* }}} */

void zend_eval_const_expr(zend_ast **ast_ptr TSRMLS_DC) /* {{{ */
{
	zend_ast *ast = *ast_ptr;
	zval result;

	if (!ast) {
		return;
	}

	switch (ast->kind) {
		case ZEND_AST_BINARY_OP:
			zend_eval_const_expr(&ast->child[0] TSRMLS_CC);
			zend_eval_const_expr(&ast->child[1] TSRMLS_CC);
			if (ast->child[0]->kind != ZEND_AST_ZVAL || ast->child[1]->kind != ZEND_AST_ZVAL) {
				return;
			}

			zend_ct_eval_binary_op(&result, ast->attr,
				zend_ast_get_zval(ast->child[0]), zend_ast_get_zval(ast->child[1]) TSRMLS_CC);
			break;
		case ZEND_AST_GREATER:
		case ZEND_AST_GREATER_EQUAL:
			zend_eval_const_expr(&ast->child[0] TSRMLS_CC);
			zend_eval_const_expr(&ast->child[1] TSRMLS_CC);
			if (ast->child[0]->kind != ZEND_AST_ZVAL || ast->child[1]->kind != ZEND_AST_ZVAL) {
				return;
			}

			zend_ct_eval_greater(&result, ast->kind,
				zend_ast_get_zval(ast->child[0]), zend_ast_get_zval(ast->child[1]) TSRMLS_CC);
			break;
		case ZEND_AST_UNARY_PLUS:
		case ZEND_AST_UNARY_MINUS:
			zend_eval_const_expr(&ast->child[0] TSRMLS_CC);
			if (ast->child[0]->kind != ZEND_AST_ZVAL) {
				return;
			}

			zend_ct_eval_unary_pm(&result, ast->kind,
				zend_ast_get_zval(ast->child[0]) TSRMLS_CC);
			break;
		case ZEND_AST_ARRAY:
			if (!zend_try_ct_eval_array(&result, ast TSRMLS_CC)) {
				return;
			}
			break;
		case ZEND_AST_MAGIC_CONST:
			if (!zend_try_ct_eval_magic_const(&result, ast TSRMLS_CC)) {
				return;
			}
			break;
		case ZEND_AST_CONST:
			if (!zend_constant_ct_subst(&result, zend_ast_get_str(ast->child[0]), 0 TSRMLS_CC)) {
				return;
			}
			break;
		default:
			return;
	}

	zend_ast_destroy(ast);
	*ast_ptr = zend_ast_create_zval(&result);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
