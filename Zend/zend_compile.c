/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
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

#define CONSTANT_EX(op_array, op) \
	(op_array)->literals[op].constant

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
			(target)->EA = 0; \
		} \
	} while (0)

#define COPY_NODE(target, src) do { \
		target ## _type = src ## _type; \
		target = src; \
	} while (0)

#define CALCULATE_LITERAL_HASH(num) do { \
		zval *c = &CONSTANT(num); \
		Z_HASH_P(c) = str_hash(Z_STRVAL_P(c), Z_STRLEN_P(c)); \
	} while (0)

#define GET_CACHE_SLOT(literal) do { \
		CG(active_op_array)->literals[literal].cache_slot = CG(active_op_array)->last_cache_slot++; \
		if ((CG(active_op_array)->fn_flags & ZEND_ACC_INTERACTIVE) && CG(active_op_array)->run_time_cache) { \
			CG(active_op_array)->run_time_cache = erealloc(CG(active_op_array)->run_time_cache, CG(active_op_array)->last_cache_slot * sizeof(void*)); \
			CG(active_op_array)->run_time_cache[CG(active_op_array)->last_cache_slot - 1] = NULL; \
		} \
	} while (0)

#define POLYMORPHIC_CACHE_SLOT_SIZE 2

#define GET_POLYMORPHIC_CACHE_SLOT(literal) do { \
		CG(active_op_array)->literals[literal].cache_slot = CG(active_op_array)->last_cache_slot; \
		CG(active_op_array)->last_cache_slot += POLYMORPHIC_CACHE_SLOT_SIZE; \
		if ((CG(active_op_array)->fn_flags & ZEND_ACC_INTERACTIVE) && CG(active_op_array)->run_time_cache) { \
			CG(active_op_array)->run_time_cache = erealloc(CG(active_op_array)->run_time_cache, CG(active_op_array)->last_cache_slot * sizeof(void*)); \
			CG(active_op_array)->run_time_cache[CG(active_op_array)->last_cache_slot - 1] = NULL; \
			CG(active_op_array)->run_time_cache[CG(active_op_array)->last_cache_slot - 2] = NULL; \
		} \
	} while (0)

#define FREE_POLYMORPHIC_CACHE_SLOT(literal) do { \
		if (CG(active_op_array)->literals[literal].cache_slot != -1 && \
		    CG(active_op_array)->literals[literal].cache_slot == \
		    CG(active_op_array)->last_cache_slot - POLYMORPHIC_CACHE_SLOT_SIZE) { \
			CG(active_op_array)->literals[literal].cache_slot = -1; \
			CG(active_op_array)->last_cache_slot -= POLYMORPHIC_CACHE_SLOT_SIZE; \
		} \
	} while (0)

ZEND_API zend_op_array *(*zend_compile_file)(zend_file_handle *file_handle, int type TSRMLS_DC);
ZEND_API zend_op_array *(*zend_compile_string)(zval *source_string, char *filename TSRMLS_DC);

#ifndef ZTS
ZEND_API zend_compiler_globals compiler_globals;
ZEND_API zend_executor_globals executor_globals;
#endif

static void zend_push_function_call_entry(zend_function *fbc TSRMLS_DC) /* {{{ */
{
	zend_function_call_entry fcall = { fbc };
	zend_stack_push(&CG(function_call_stack), &fcall, sizeof(zend_function_call_entry));
}
/* }}} */

static void zend_duplicate_property_info(zend_property_info *property_info) /* {{{ */
{
	property_info->name = str_estrndup(property_info->name, property_info->name_length);
	if (property_info->doc_comment) {
		property_info->doc_comment = estrndup(property_info->doc_comment, property_info->doc_comment_len);
	}
}
/* }}} */

static void zend_duplicate_property_info_internal(zend_property_info *property_info) /* {{{ */
{
	property_info->name = str_strndup(property_info->name, property_info->name_length);
}
/* }}} */

static void zend_destroy_property_info(zend_property_info *property_info) /* {{{ */
{
	str_efree(property_info->name);
	if (property_info->doc_comment) {
		efree((char*)property_info->doc_comment);
	}
}
/* }}} */

static void zend_destroy_property_info_internal(zend_property_info *property_info) /* {{{ */
{
	str_free((char*)property_info->name);
}
/* }}} */

static void build_runtime_defined_function_key(zval *result, const char *name, int name_length TSRMLS_DC) /* {{{ */
{
	char char_pos_buf[32];
	uint char_pos_len;
	const char *filename;

	char_pos_len = zend_sprintf(char_pos_buf, "%p", LANG_SCNG(yy_text));
	if (CG(active_op_array)->filename) {
		filename = CG(active_op_array)->filename;
	} else {
		filename = "-";
	}

	/* NULL, name length, filename length, last accepting char position length */
	Z_STRLEN_P(result) = 1+name_length+strlen(filename)+char_pos_len;

 	/* must be binary safe */
 	Z_STRVAL_P(result) = (char *) safe_emalloc(Z_STRLEN_P(result), 1, 1);
 	Z_STRVAL_P(result)[0] = '\0';
 	sprintf(Z_STRVAL_P(result)+1, "%s%s%s", name, filename, char_pos_buf);

	result->type = IS_STRING;
	Z_SET_REFCOUNT_P(result, 1);
}
/* }}} */

static void init_compiler_declarables(TSRMLS_D) /* {{{ */
{
	Z_TYPE(CG(declarables).ticks) = IS_LONG;
	Z_LVAL(CG(declarables).ticks) = 0;
}
/* }}} */

void zend_init_compiler_context(TSRMLS_D) /* {{{ */
{
	CG(context).opcodes_size = (CG(active_op_array)->fn_flags & ZEND_ACC_INTERACTIVE) ? INITIAL_INTERACTIVE_OP_ARRAY_SIZE : INITIAL_OP_ARRAY_SIZE;
	CG(context).vars_size = 0;
	CG(context).literals_size = 0;
	CG(context).current_brk_cont = -1;
	CG(context).backpatch_count = 0;
	CG(context).nested_calls = 0;
	CG(context).used_stack = 0;
	CG(context).in_finally = 0;
	CG(context).labels = NULL;
}
/* }}} */

void zend_init_compiler_data_structures(TSRMLS_D) /* {{{ */
{
	zend_stack_init(&CG(bp_stack));
	zend_stack_init(&CG(function_call_stack));
	zend_stack_init(&CG(switch_cond_stack));
	zend_stack_init(&CG(foreach_copy_stack));
	zend_stack_init(&CG(object_stack));
	zend_stack_init(&CG(declare_stack));
	CG(active_class_entry) = NULL;
	zend_llist_init(&CG(list_llist), sizeof(list_llist_element), NULL, 0);
	zend_llist_init(&CG(dimension_llist), sizeof(int), NULL, 0);
	zend_stack_init(&CG(list_stack));
	CG(in_compilation) = 0;
	CG(start_lineno) = 0;
	CG(current_namespace) = NULL;
	CG(in_namespace) = 0;
	CG(has_bracketed_namespaces) = 0;
	CG(current_import) = NULL;
	CG(current_import_function) = NULL;
	CG(current_import_const) = NULL;
	zend_hash_init(&CG(const_filenames), 0, NULL, NULL, 0);
	init_compiler_declarables(TSRMLS_C);
	zend_stack_init(&CG(context_stack));

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
	CG(active_op_array) = NULL;
	memset(&CG(context), 0, sizeof(CG(context)));
	zend_init_compiler_data_structures(TSRMLS_C);
	zend_init_rsrc_list(TSRMLS_C);
	zend_hash_init(&CG(filenames_table), 5, NULL, (dtor_func_t) free_estring, 0);
	zend_llist_init(&CG(open_files), sizeof(zend_file_handle), (void (*)(void *)) file_handle_dtor, 0);
	CG(unclean_shutdown) = 0;
}
/* }}} */

void shutdown_compiler(TSRMLS_D) /* {{{ */
{
	zend_stack_destroy(&CG(bp_stack));
	zend_stack_destroy(&CG(function_call_stack));
	zend_stack_destroy(&CG(switch_cond_stack));
	zend_stack_destroy(&CG(foreach_copy_stack));
	zend_stack_destroy(&CG(object_stack));
	zend_stack_destroy(&CG(declare_stack));
	zend_stack_destroy(&CG(list_stack));
	zend_hash_destroy(&CG(filenames_table));
	zend_llist_destroy(&CG(open_files));
	zend_hash_destroy(&CG(const_filenames));
	zend_stack_destroy(&CG(context_stack));
}
/* }}} */

ZEND_API char *zend_set_compiled_filename(const char *new_compiled_filename TSRMLS_DC) /* {{{ */
{
	char **pp, *p;
	int length = strlen(new_compiled_filename);

	if (zend_hash_find(&CG(filenames_table), new_compiled_filename, length+1, (void **) &pp) == SUCCESS) {
		CG(compiled_filename) = *pp;
		return *pp;
	}
	p = estrndup(new_compiled_filename, length);
	zend_hash_update(&CG(filenames_table), new_compiled_filename, length+1, &p, sizeof(char *), (void **) &pp);
	CG(compiled_filename) = p;
	return p;
}
/* }}} */

ZEND_API void zend_restore_compiled_filename(char *original_compiled_filename TSRMLS_DC) /* {{{ */
{
	CG(compiled_filename) = original_compiled_filename;
}
/* }}} */

ZEND_API char *zend_get_compiled_filename(TSRMLS_D) /* {{{ */
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

static zend_uint get_temporary_variable(zend_op_array *op_array) /* {{{ */
{
	return (zend_uint)(zend_uintptr_t)EX_TMP_VAR_NUM(0, (op_array->T)++);
}
/* }}} */

static int lookup_cv(zend_op_array *op_array, char* name, int name_len, ulong hash TSRMLS_DC) /* {{{ */
{
	int i = 0;
	ulong hash_value = hash ? hash : zend_inline_hash_func(name, name_len+1);

	while (i < op_array->last_var) {
		if (op_array->vars[i].name == name ||
		    (op_array->vars[i].hash_value == hash_value &&
		     op_array->vars[i].name_len == name_len &&
		     memcmp(op_array->vars[i].name, name, name_len) == 0)) {
			str_efree(name);
			return i;
		}
		i++;
	}
	i = op_array->last_var;
	op_array->last_var++;
	if (op_array->last_var > CG(context).vars_size) {
		CG(context).vars_size += 16; /* FIXME */
		op_array->vars = erealloc(op_array->vars, CG(context).vars_size * sizeof(zend_compiled_variable));
	}
	op_array->vars[i].name = zend_new_interned_string(name, name_len + 1, 1 TSRMLS_CC);
	op_array->vars[i].name_len = name_len;
	op_array->vars[i].hash_value = hash_value;
	return i;
}
/* }}} */

void zend_del_literal(zend_op_array *op_array, int n) /* {{{ */
{
	zval_dtor(&CONSTANT_EX(op_array, n));
	if (n + 1 == op_array->last_literal) {
		op_array->last_literal--;
	} else {
		Z_TYPE(CONSTANT_EX(op_array, n)) = IS_NULL;
	}
}
/* }}} */

/* Common part of zend_add_literal and zend_append_individual_literal */
static inline void zend_insert_literal(zend_op_array *op_array, const zval *zv, int literal_position TSRMLS_DC) /* {{{ */
{
	if (Z_TYPE_P(zv) == IS_STRING || Z_TYPE_P(zv) == IS_CONSTANT) {
		zval *z = (zval*)zv;
		Z_STRVAL_P(z) = (char*)zend_new_interned_string(Z_STRVAL_P(zv), Z_STRLEN_P(zv) + 1, 1 TSRMLS_CC);
	}
	CONSTANT_EX(op_array, literal_position) = *zv;
	Z_SET_REFCOUNT(CONSTANT_EX(op_array, literal_position), 2);
	Z_SET_ISREF(CONSTANT_EX(op_array, literal_position));
	op_array->literals[literal_position].hash_value = 0;
	op_array->literals[literal_position].cache_slot = -1;
}
/* }}} */

/* Is used while compiling a function, using the context to keep track
   of an approximate size to avoid to relocate to often.
   Literals are truncated to actual size in the second compiler pass (pass_two()). */
int zend_add_literal(zend_op_array *op_array, const zval *zv TSRMLS_DC) /* {{{ */
{
	int i = op_array->last_literal;
	op_array->last_literal++;
	if (i >= CG(context).literals_size) {
		while (i >= CG(context).literals_size) {
			CG(context).literals_size += 16; /* FIXME */
		}
		op_array->literals = (zend_literal*)erealloc(op_array->literals, CG(context).literals_size * sizeof(zend_literal));
	}
	zend_insert_literal(op_array, zv, i TSRMLS_CC);
	return i;
}
/* }}} */

/* Is used after normal compilation to append an additional literal.
   Allocation is done precisely here. */
int zend_append_individual_literal(zend_op_array *op_array, const zval *zv TSRMLS_DC) /* {{{ */
{
	int i = op_array->last_literal;
	op_array->last_literal++;
	op_array->literals = (zend_literal*)erealloc(op_array->literals, (i + 1) * sizeof(zend_literal));
	zend_insert_literal(op_array, zv, i TSRMLS_CC);
	return i;
}
/* }}} */

int zend_add_func_name_literal(zend_op_array *op_array, const zval *zv TSRMLS_DC) /* {{{ */
{
	int ret;
	char *lc_name;
	zval c;
	int lc_literal;

	if (op_array->last_literal > 0 &&
	    &op_array->literals[op_array->last_literal - 1].constant == zv &&
	    op_array->literals[op_array->last_literal - 1].cache_slot == -1) {
		/* we already have function name as last literal (do nothing) */
		ret = op_array->last_literal - 1;
	} else {
		ret = zend_add_literal(op_array, zv TSRMLS_CC);
	}

	lc_name = zend_str_tolower_dup(Z_STRVAL_P(zv), Z_STRLEN_P(zv));
	ZVAL_STRINGL(&c, lc_name, Z_STRLEN_P(zv), 0);
	lc_literal = zend_add_literal(CG(active_op_array), &c TSRMLS_CC);
	CALCULATE_LITERAL_HASH(lc_literal);

	return ret;
}
/* }}} */

int zend_add_ns_func_name_literal(zend_op_array *op_array, const zval *zv TSRMLS_DC) /* {{{ */
{
	int ret;
	char *lc_name;
	const char *ns_separator;
	int lc_len;
	zval c;
	int lc_literal;

	if (op_array->last_literal > 0 &&
	    &op_array->literals[op_array->last_literal - 1].constant == zv &&
	    op_array->literals[op_array->last_literal - 1].cache_slot == -1) {
		/* we already have function name as last literal (do nothing) */
		ret = op_array->last_literal - 1;
	} else {
		ret = zend_add_literal(op_array, zv TSRMLS_CC);
	}

	lc_name = zend_str_tolower_dup(Z_STRVAL_P(zv), Z_STRLEN_P(zv));
	ZVAL_STRINGL(&c, lc_name, Z_STRLEN_P(zv), 0);
	lc_literal = zend_add_literal(CG(active_op_array), &c TSRMLS_CC);
	CALCULATE_LITERAL_HASH(lc_literal);

	ns_separator = (const char*)zend_memrchr(Z_STRVAL_P(zv), '\\', Z_STRLEN_P(zv));

	if (ns_separator != NULL) {
		ns_separator += 1;
		lc_len = Z_STRLEN_P(zv) - (ns_separator - Z_STRVAL_P(zv));
		lc_name = zend_str_tolower_dup(ns_separator, lc_len);
		ZVAL_STRINGL(&c, lc_name, lc_len, 0);
		lc_literal = zend_add_literal(CG(active_op_array), &c TSRMLS_CC);
		CALCULATE_LITERAL_HASH(lc_literal);
	}

	return ret;
}
/* }}} */

int zend_add_class_name_literal(zend_op_array *op_array, const zval *zv TSRMLS_DC) /* {{{ */
{
	int ret;
	char *lc_name;
	int lc_len;
	zval c;
	int lc_literal;

	if (op_array->last_literal > 0 &&
	    &op_array->literals[op_array->last_literal - 1].constant == zv &&
	    op_array->literals[op_array->last_literal - 1].cache_slot == -1) {
		/* we already have function name as last literal (do nothing) */
		ret = op_array->last_literal - 1;
	} else {
		ret = zend_add_literal(op_array, zv TSRMLS_CC);
	}

	if (Z_STRVAL_P(zv)[0] == '\\') {
		lc_len = Z_STRLEN_P(zv) - 1;
		lc_name = zend_str_tolower_dup(Z_STRVAL_P(zv) + 1, lc_len);
	} else {
		lc_len = Z_STRLEN_P(zv);
		lc_name = zend_str_tolower_dup(Z_STRVAL_P(zv), lc_len);
	}
	ZVAL_STRINGL(&c, lc_name, lc_len, 0);
	lc_literal = zend_add_literal(CG(active_op_array), &c TSRMLS_CC);
	CALCULATE_LITERAL_HASH(lc_literal);

	GET_CACHE_SLOT(ret);

	return ret;
}
/* }}} */

int zend_add_const_name_literal(zend_op_array *op_array, const zval *zv, int unqualified TSRMLS_DC) /* {{{ */
{
	int ret, tmp_literal;
	char *name, *tmp_name;
	const char *ns_separator;
	int name_len, ns_len;
	zval c;

	if (op_array->last_literal > 0 &&
	    &op_array->literals[op_array->last_literal - 1].constant == zv &&
	    op_array->literals[op_array->last_literal - 1].cache_slot == -1) {
		/* we already have function name as last literal (do nothing) */
		ret = op_array->last_literal - 1;
	} else {
		ret = zend_add_literal(op_array, zv TSRMLS_CC);
	}

	/* skip leading '\\' */
	if (Z_STRVAL_P(zv)[0] == '\\') {
		name_len = Z_STRLEN_P(zv) - 1;
		name = Z_STRVAL_P(zv) + 1;
	} else {
		name_len = Z_STRLEN_P(zv);
		name = Z_STRVAL_P(zv);
	}
	ns_separator = zend_memrchr(name, '\\', name_len);
	if (ns_separator) {
		ns_len = ns_separator - name;
	} else {
		ns_len = 0;
	}

	if (ns_len) {
		/* lowercased namespace name & original constant name */
		tmp_name = estrndup(name, name_len);
		zend_str_tolower(tmp_name, ns_len);
		ZVAL_STRINGL(&c, tmp_name, name_len, 0);
		tmp_literal = zend_add_literal(CG(active_op_array), &c TSRMLS_CC);
		CALCULATE_LITERAL_HASH(tmp_literal);

		/* lowercased namespace name & lowercased constant name */
		tmp_name = zend_str_tolower_dup(name, name_len);
		ZVAL_STRINGL(&c, tmp_name, name_len, 0);
		tmp_literal = zend_add_literal(CG(active_op_array), &c TSRMLS_CC);
		CALCULATE_LITERAL_HASH(tmp_literal);
	}

	if (ns_len) {
		if (!unqualified) {
			return ret;
		}
		ns_len++;
		name += ns_len;
		name_len -= ns_len;
	}

	/* original constant name */
	tmp_name = estrndup(name, name_len);
	ZVAL_STRINGL(&c, tmp_name, name_len, 0);
	tmp_literal = zend_add_literal(CG(active_op_array), &c TSRMLS_CC);
	CALCULATE_LITERAL_HASH(tmp_literal);

	/* lowercased constant name */
	tmp_name = zend_str_tolower_dup(name, name_len);
	ZVAL_STRINGL(&c, tmp_name, name_len, 0);
	tmp_literal = zend_add_literal(CG(active_op_array), &c TSRMLS_CC);
	CALCULATE_LITERAL_HASH(tmp_literal);

	return ret;
}
/* }}} */

#define LITERAL_STRINGL(op, str, len, copy) do { \
		zval _c; \
		ZVAL_STRINGL(&_c, str, len, copy); \
		op.constant = zend_add_literal(CG(active_op_array), &_c TSRMLS_CC); \
	} while (0)

#define LITERAL_LONG(op, val) do { \
		zval _c; \
		ZVAL_LONG(&_c, val); \
		op.constant = zend_add_literal(CG(active_op_array), &_c TSRMLS_CC); \
	} while (0)

#define LITERAL_LONG_EX(op_array, op, val) do { \
		zval _c; \
		ZVAL_LONG(&_c, val); \
		op.constant = zend_add_literal(op_array, &_c TSRMLS_CC); \
	} while (0)

#define LITERAL_NULL(op) do { \
		zval _c; \
		INIT_ZVAL(	_c); \
		op.constant = zend_add_literal(CG(active_op_array), &_c TSRMLS_CC); \
	} while (0)

static inline zend_bool zend_is_function_or_method_call(const znode *variable) /* {{{ */
{
	zend_uint type = variable->EA;

	return  ((type & ZEND_PARSED_METHOD_CALL) || (type == ZEND_PARSED_FUNCTION_CALL));
}
/* }}} */

void zend_do_binary_op(zend_uchar op, znode *result, const znode *op1, const znode *op2 TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = op;
	opline->result_type = IS_TMP_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	SET_NODE(opline->op1, op1);
	SET_NODE(opline->op2, op2);
	GET_NODE(result, opline->result);
}
/* }}} */

void zend_do_unary_op(zend_uchar op, znode *result, const znode *op1 TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = op;
	opline->result_type = IS_TMP_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	SET_NODE(opline->op1, op1);
	GET_NODE(result, opline->result);
	SET_UNUSED(opline->op2);
}
/* }}} */

#define MAKE_NOP(opline)	{ opline->opcode = ZEND_NOP;  memset(&opline->result,0,sizeof(opline->result)); memset(&opline->op1,0,sizeof(opline->op1)); memset(&opline->op2,0,sizeof(opline->op2)); opline->result_type=opline->op1_type=opline->op2_type=IS_UNUSED;  }

static void zend_do_op_data(zend_op *data_op, const znode *value TSRMLS_DC) /* {{{ */
{
	data_op->opcode = ZEND_OP_DATA;
	SET_NODE(data_op->op1, value);
	SET_UNUSED(data_op->op2);
}
/* }}} */

void zend_do_binary_assign_op(zend_uchar op, znode *result, const znode *op1, const znode *op2 TSRMLS_DC) /* {{{ */
{
	int last_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	if (last_op_number > 0) {
		zend_op *last_op = &CG(active_op_array)->opcodes[last_op_number-1];

		switch (last_op->opcode) {
			case ZEND_FETCH_OBJ_RW:
				last_op->opcode = op;
				last_op->extended_value = ZEND_ASSIGN_OBJ;

				zend_do_op_data(opline, op2 TSRMLS_CC);
				SET_UNUSED(opline->result);
				GET_NODE(result, last_op->result);
				return;
			case ZEND_FETCH_DIM_RW:
				last_op->opcode = op;
				last_op->extended_value = ZEND_ASSIGN_DIM;

				zend_do_op_data(opline, op2 TSRMLS_CC);
				opline->op2.var = get_temporary_variable(CG(active_op_array));
				opline->op2_type = IS_VAR;
				SET_UNUSED(opline->result);
				GET_NODE(result,last_op->result);
				return;
			default:
				break;
		}
	}

	opline->opcode = op;
	SET_NODE(opline->op1, op1);
	SET_NODE(opline->op2, op2);
	opline->result_type = IS_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	GET_NODE(result, opline->result);
}
/* }}} */

void fetch_simple_variable_ex(znode *result, znode *varname, int bp, zend_uchar op TSRMLS_DC) /* {{{ */
{
	zend_op opline;
	zend_op *opline_ptr;
	zend_llist *fetch_list_ptr;

	if (varname->op_type == IS_CONST) {
		ulong hash;

		if (Z_TYPE(varname->u.constant) != IS_STRING) {
			convert_to_string(&varname->u.constant);
		}

		hash = str_hash(Z_STRVAL(varname->u.constant), Z_STRLEN(varname->u.constant));
		if (!zend_is_auto_global_quick(Z_STRVAL(varname->u.constant), Z_STRLEN(varname->u.constant), hash TSRMLS_CC) &&
		    !(Z_STRLEN(varname->u.constant) == (sizeof("this")-1) &&
		      !memcmp(Z_STRVAL(varname->u.constant), "this", sizeof("this") - 1)) &&
		    (CG(active_op_array)->last == 0 ||
		     CG(active_op_array)->opcodes[CG(active_op_array)->last-1].opcode != ZEND_BEGIN_SILENCE)) {
			result->op_type = IS_CV;
			result->u.op.var = lookup_cv(CG(active_op_array), Z_STRVAL(varname->u.constant), Z_STRLEN(varname->u.constant), hash TSRMLS_CC);
			Z_STRVAL(varname->u.constant) = (char*)CG(active_op_array)->vars[result->u.op.var].name;
			result->EA = 0;
			return;
		}
	}

	if (bp) {
		opline_ptr = &opline;
		init_op(opline_ptr TSRMLS_CC);
	} else {
		opline_ptr = get_next_op(CG(active_op_array) TSRMLS_CC);
	}

	opline_ptr->opcode = op;
	opline_ptr->result_type = IS_VAR;
	opline_ptr->result.var = get_temporary_variable(CG(active_op_array));
	SET_NODE(opline_ptr->op1, varname);
	GET_NODE(result, opline_ptr->result);
	SET_UNUSED(opline_ptr->op2);
	opline_ptr->extended_value = ZEND_FETCH_LOCAL;

	if (varname->op_type == IS_CONST) {
		CALCULATE_LITERAL_HASH(opline_ptr->op1.constant);
		if (zend_is_auto_global_quick(Z_STRVAL(varname->u.constant), Z_STRLEN(varname->u.constant), Z_HASH_P(&CONSTANT(opline_ptr->op1.constant)) TSRMLS_CC)) {
			opline_ptr->extended_value = ZEND_FETCH_GLOBAL;
		}
	}

	if (bp) {
		zend_stack_top(&CG(bp_stack), (void **) &fetch_list_ptr);
		zend_llist_add_element(fetch_list_ptr, opline_ptr);
	}
}
/* }}} */

void fetch_simple_variable(znode *result, znode *varname, int bp TSRMLS_DC) /* {{{ */
{
	/* the default mode must be Write, since fetch_simple_variable() is used to define function arguments */
	fetch_simple_variable_ex(result, varname, bp, ZEND_FETCH_W TSRMLS_CC);
}
/* }}} */

void zend_do_fetch_static_member(znode *result, znode *class_name TSRMLS_DC) /* {{{ */
{
	znode class_node;
	zend_llist *fetch_list_ptr;
	zend_llist_element *le;
	zend_op *opline_ptr;
	zend_op opline;

	if (class_name->op_type == IS_CONST &&
	    ZEND_FETCH_CLASS_DEFAULT == zend_get_class_fetch_type(Z_STRVAL(class_name->u.constant), Z_STRLEN(class_name->u.constant))) {
		zend_resolve_class_name(class_name TSRMLS_CC);
		class_node = *class_name;
	} else {
		zend_do_fetch_class(&class_node, class_name TSRMLS_CC);
	}
	zend_stack_top(&CG(bp_stack), (void **) &fetch_list_ptr);
	if (result->op_type == IS_CV) {
		init_op(&opline TSRMLS_CC);

		opline.opcode = ZEND_FETCH_W;
		opline.result_type = IS_VAR;
		opline.result.var = get_temporary_variable(CG(active_op_array));
		opline.op1_type = IS_CONST;
		LITERAL_STRINGL(opline.op1, estrdup(CG(active_op_array)->vars[result->u.op.var].name), CG(active_op_array)->vars[result->u.op.var].name_len, 0);
		CALCULATE_LITERAL_HASH(opline.op1.constant);
		GET_POLYMORPHIC_CACHE_SLOT(opline.op1.constant);
		if (class_node.op_type == IS_CONST) {
			opline.op2_type = IS_CONST;
			opline.op2.constant =
				zend_add_class_name_literal(CG(active_op_array), &class_node.u.constant TSRMLS_CC);
		} else {
			SET_NODE(opline.op2, &class_node);
		}
		GET_NODE(result,opline.result);
		opline.extended_value |= ZEND_FETCH_STATIC_MEMBER;
		opline_ptr = &opline;

		zend_llist_add_element(fetch_list_ptr, &opline);
	} else {
		le = fetch_list_ptr->head;

		opline_ptr = (zend_op *)le->data;
		if (opline_ptr->opcode != ZEND_FETCH_W && opline_ptr->op1_type == IS_CV) {
			init_op(&opline TSRMLS_CC);
			opline.opcode = ZEND_FETCH_W;
			opline.result_type = IS_VAR;
			opline.result.var = get_temporary_variable(CG(active_op_array));
			opline.op1_type = IS_CONST;
			LITERAL_STRINGL(opline.op1, estrdup(CG(active_op_array)->vars[opline_ptr->op1.var].name), CG(active_op_array)->vars[opline_ptr->op1.var].name_len, 0);
			CALCULATE_LITERAL_HASH(opline.op1.constant);
			GET_POLYMORPHIC_CACHE_SLOT(opline.op1.constant);
			if (class_node.op_type == IS_CONST) {
				opline.op2_type = IS_CONST;
				opline.op2.constant =
					zend_add_class_name_literal(CG(active_op_array), &class_node.u.constant TSRMLS_CC);
			} else {
				SET_NODE(opline.op2, &class_node);
			}
			opline.extended_value |= ZEND_FETCH_STATIC_MEMBER;
			COPY_NODE(opline_ptr->op1, opline.result);

			zend_llist_prepend_element(fetch_list_ptr, &opline);
		} else {
			if (opline_ptr->op1_type == IS_CONST) {
				GET_POLYMORPHIC_CACHE_SLOT(opline_ptr->op1.constant);
			}
			if (class_node.op_type == IS_CONST) {
				opline_ptr->op2_type = IS_CONST;
				opline_ptr->op2.constant =
					zend_add_class_name_literal(CG(active_op_array), &class_node.u.constant TSRMLS_CC);
			} else {
				SET_NODE(opline_ptr->op2, &class_node);
			}
			opline_ptr->extended_value |= ZEND_FETCH_STATIC_MEMBER;
		}
	}
}
/* }}} */

void fetch_array_begin(znode *result, znode *varname, znode *first_dim TSRMLS_DC) /* {{{ */
{
	fetch_simple_variable(result, varname, 1 TSRMLS_CC);

	fetch_array_dim(result, result, first_dim TSRMLS_CC);
}
/* }}} */

void fetch_array_dim(znode *result, const znode *parent, const znode *dim TSRMLS_DC) /* {{{ */
{
	zend_op opline;
	zend_llist *fetch_list_ptr;

	zend_stack_top(&CG(bp_stack), (void **) &fetch_list_ptr);

	if (zend_is_function_or_method_call(parent)) {
		init_op(&opline TSRMLS_CC);
		opline.opcode = ZEND_SEPARATE;
		SET_NODE(opline.op1, parent);
		SET_UNUSED(opline.op2);
		opline.result_type = IS_VAR;
		opline.result.var = opline.op1.var;
		zend_llist_add_element(fetch_list_ptr, &opline);
	}

	init_op(&opline TSRMLS_CC);
	opline.opcode = ZEND_FETCH_DIM_W;	/* the backpatching routine assumes W */
	opline.result_type = IS_VAR;
	opline.result.var = get_temporary_variable(CG(active_op_array));
	SET_NODE(opline.op1, parent);
	SET_NODE(opline.op2, dim);
	if (opline.op2_type == IS_CONST && Z_TYPE(CONSTANT(opline.op2.constant)) == IS_STRING) {
		ulong index;
		int numeric = 0;

		ZEND_HANDLE_NUMERIC_EX(Z_STRVAL(CONSTANT(opline.op2.constant)), Z_STRLEN(CONSTANT(opline.op2.constant))+1, index, numeric = 1);
		if (numeric) {
			zval_dtor(&CONSTANT(opline.op2.constant));
			ZVAL_LONG(&CONSTANT(opline.op2.constant), index);
		} else {
			CALCULATE_LITERAL_HASH(opline.op2.constant);
		}
	}

	GET_NODE(result, opline.result);

	zend_llist_add_element(fetch_list_ptr, &opline);
}
/* }}} */

void fetch_string_offset(znode *result, const znode *parent, const znode *offset TSRMLS_DC) /* {{{ */
{
	fetch_array_dim(result, parent, offset TSRMLS_CC);
}
/* }}} */

void zend_do_print(znode *result, const znode *arg TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->result_type = IS_TMP_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	opline->opcode = ZEND_PRINT;
	SET_NODE(opline->op1, arg);
	SET_UNUSED(opline->op2);
	GET_NODE(result, opline->result);
}
/* }}} */

void zend_do_echo(const znode *arg TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_ECHO;
	SET_NODE(opline->op1, arg);
	SET_UNUSED(opline->op2);
}
/* }}} */

void zend_do_abstract_method(const znode *function_name, znode *modifiers, const znode *body TSRMLS_DC) /* {{{ */
{
	char *method_type;

	if (CG(active_class_entry)->ce_flags & ZEND_ACC_INTERFACE) {
		Z_LVAL(modifiers->u.constant) |= ZEND_ACC_ABSTRACT;
		method_type = "Interface";
	} else {
		method_type = "Abstract";
	}

	if (Z_LVAL(modifiers->u.constant) & ZEND_ACC_ABSTRACT) {
		if(Z_LVAL(modifiers->u.constant) & ZEND_ACC_PRIVATE) {
			zend_error_noreturn(E_COMPILE_ERROR, "%s function %s::%s() cannot be declared private", method_type, CG(active_class_entry)->name, Z_STRVAL(function_name->u.constant));
		}
		if (Z_LVAL(body->u.constant) == ZEND_ACC_ABSTRACT) {
			zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

			opline->opcode = ZEND_RAISE_ABSTRACT_ERROR;
			SET_UNUSED(opline->op1);
			SET_UNUSED(opline->op2);
		} else {
			/* we had code in the function body */
			zend_error_noreturn(E_COMPILE_ERROR, "%s function %s::%s() cannot contain body", method_type, CG(active_class_entry)->name, Z_STRVAL(function_name->u.constant));
		}
	} else {
		if (Z_LVAL(body->u.constant) == ZEND_ACC_ABSTRACT) {
			zend_error_noreturn(E_COMPILE_ERROR, "Non-abstract method %s::%s() must contain body", CG(active_class_entry)->name, Z_STRVAL(function_name->u.constant));
		}
	}
}
/* }}} */

static zend_bool opline_is_fetch_this(const zend_op *opline TSRMLS_DC) /* {{{ */
{
	if ((opline->opcode == ZEND_FETCH_W) && (opline->op1_type == IS_CONST)
		&& (Z_TYPE(CONSTANT(opline->op1.constant)) == IS_STRING)
		&& ((opline->extended_value & ZEND_FETCH_STATIC_MEMBER) != ZEND_FETCH_STATIC_MEMBER)
		&& (Z_HASH_P(&CONSTANT(opline->op1.constant)) == THIS_HASHVAL)
		&& (Z_STRLEN(CONSTANT(opline->op1.constant)) == (sizeof("this")-1))
		&& !memcmp(Z_STRVAL(CONSTANT(opline->op1.constant)), "this", sizeof("this") - 1)) {
		return 1;
	} else {
		return 0;
	}
}
/* }}} */

void zend_do_assign(znode *result, znode *variable, znode *value TSRMLS_DC) /* {{{ */
{
	int last_op_number;
	zend_op *opline;

	if (value->op_type == IS_CV) {
		zend_llist *fetch_list_ptr;

		zend_stack_top(&CG(bp_stack), (void **) &fetch_list_ptr);
		if (fetch_list_ptr && fetch_list_ptr->head) {
			opline = (zend_op *)fetch_list_ptr->head->data;

			if (opline->opcode == ZEND_FETCH_DIM_W &&
			    opline->op1_type == IS_CV &&
			    opline->op1.var == value->u.op.var) {

				opline = get_next_op(CG(active_op_array) TSRMLS_CC);
				opline->opcode = ZEND_FETCH_R;
				opline->result_type = IS_VAR;
				opline->result.var = get_temporary_variable(CG(active_op_array));
				opline->op1_type = IS_CONST;
				LITERAL_STRINGL(opline->op1,
					CG(active_op_array)->vars[value->u.op.var].name,
					CG(active_op_array)->vars[value->u.op.var].name_len, 1);
				CALCULATE_LITERAL_HASH(opline->op1.constant);
				SET_UNUSED(opline->op2);
				opline->extended_value = ZEND_FETCH_LOCAL;
				GET_NODE(value, opline->result);
			}
		}
	}

	zend_do_end_variable_parse(variable, BP_VAR_W, 0 TSRMLS_CC);

	last_op_number = get_next_op_number(CG(active_op_array));
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	if (variable->op_type == IS_CV) {
		if (variable->u.op.var == CG(active_op_array)->this_var) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot re-assign $this");
		}
	} else if (variable->op_type == IS_VAR) {
		int n = 0;

		while (last_op_number - n > 0) {
			zend_op *last_op;

			last_op = &CG(active_op_array)->opcodes[last_op_number-n-1];

			if (last_op->result_type == IS_VAR &&
			    last_op->result.var == variable->u.op.var) {
				if (last_op->opcode == ZEND_FETCH_OBJ_W) {
					if (n > 0) {
						int opline_no = (opline-CG(active_op_array)->opcodes)/sizeof(*opline);
						*opline = *last_op;
						MAKE_NOP(last_op);
						/* last_op = opline; */
						opline = get_next_op(CG(active_op_array) TSRMLS_CC);
						/* get_next_op can realloc, we need to move last_op */
						last_op = &CG(active_op_array)->opcodes[opline_no];
					}
					last_op->opcode = ZEND_ASSIGN_OBJ;
					zend_do_op_data(opline, value TSRMLS_CC);
					SET_UNUSED(opline->result);
					GET_NODE(result, last_op->result);
					return;
				} else if (last_op->opcode == ZEND_FETCH_DIM_W) {
					if (n > 0) {
						int opline_no = (opline-CG(active_op_array)->opcodes)/sizeof(*opline);
						*opline = *last_op;
						MAKE_NOP(last_op);
						/* last_op = opline; */
						/* TBFixed: this can realloc opcodes, leaving last_op pointing wrong */
						opline = get_next_op(CG(active_op_array) TSRMLS_CC);
						/* get_next_op can realloc, we need to move last_op */
						last_op = &CG(active_op_array)->opcodes[opline_no];
					}
					last_op->opcode = ZEND_ASSIGN_DIM;
					zend_do_op_data(opline, value TSRMLS_CC);
					opline->op2.var = get_temporary_variable(CG(active_op_array));
					opline->op2_type = IS_VAR;
					SET_UNUSED(opline->result);
					GET_NODE(result, last_op->result);
					return;
				} else if (opline_is_fetch_this(last_op TSRMLS_CC)) {
					zend_error_noreturn(E_COMPILE_ERROR, "Cannot re-assign $this");
				} else {
					break;
				}
			}
			n++;
		}
	}

	opline->opcode = ZEND_ASSIGN;
	SET_NODE(opline->op1, variable);
	SET_NODE(opline->op2, value);
	opline->result_type = IS_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	GET_NODE(result, opline->result);
}
/* }}} */

void zend_do_assign_ref(znode *result, const znode *lvar, const znode *rvar TSRMLS_DC) /* {{{ */
{
	zend_op *opline;

	if (lvar->op_type == IS_CV) {
		if (lvar->u.op.var == CG(active_op_array)->this_var) {
 			zend_error_noreturn(E_COMPILE_ERROR, "Cannot re-assign $this");
		}
	} else if (lvar->op_type == IS_VAR) {
		int last_op_number = get_next_op_number(CG(active_op_array));

		if (last_op_number > 0) {
			opline = &CG(active_op_array)->opcodes[last_op_number-1];
			if (opline_is_fetch_this(opline TSRMLS_CC)) {
	 			zend_error_noreturn(E_COMPILE_ERROR, "Cannot re-assign $this");
			}
 		}
 	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_ASSIGN_REF;
	if (zend_is_function_or_method_call(rvar)) {
		opline->extended_value = ZEND_RETURNS_FUNCTION;
	} else if (rvar->EA & ZEND_PARSED_NEW) {
		opline->extended_value = ZEND_RETURNS_NEW;
	} else {
		opline->extended_value = 0;
	}
	if (result) {
		opline->result_type = IS_VAR;
		opline->result.var = get_temporary_variable(CG(active_op_array));
		GET_NODE(result, opline->result);
	} else {
		opline->result_type = IS_UNUSED | EXT_TYPE_UNUSED;
	}
	SET_NODE(opline->op1, lvar);
	SET_NODE(opline->op2, rvar);
}
/* }}} */

static inline void do_begin_loop(TSRMLS_D) /* {{{ */
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

static inline void do_end_loop(int cont_addr, int has_loop_var TSRMLS_DC) /* {{{ */
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

void zend_do_while_cond(const znode *expr, znode *close_bracket_token TSRMLS_DC) /* {{{ */
{
	int while_cond_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMPZ;
	SET_NODE(opline->op1, expr);
	close_bracket_token->u.op.opline_num = while_cond_op_number;
	SET_UNUSED(opline->op2);

	do_begin_loop(TSRMLS_C);
	INC_BPC(CG(active_op_array));
}
/* }}} */

void zend_do_while_end(const znode *while_token, const znode *close_bracket_token TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	/* add unconditional jump */
	opline->opcode = ZEND_JMP;
	opline->op1.opline_num = while_token->u.op.opline_num;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);

	/* update while's conditional jmp */
	CG(active_op_array)->opcodes[close_bracket_token->u.op.opline_num].op2.opline_num = get_next_op_number(CG(active_op_array));

	do_end_loop(while_token->u.op.opline_num, 0 TSRMLS_CC);

	DEC_BPC(CG(active_op_array));
}
/* }}} */

void zend_do_for_cond(const znode *expr, znode *second_semicolon_token TSRMLS_DC) /* {{{ */
{
	int for_cond_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMPZNZ;
	SET_NODE(opline->op1, expr);  /* the conditional expression */
	second_semicolon_token->u.op.opline_num = for_cond_op_number;
	SET_UNUSED(opline->op2);
}
/* }}} */

void zend_do_for_before_statement(const znode *cond_start, const znode *second_semicolon_token TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMP;
	opline->op1.opline_num = cond_start->u.op.opline_num;
	CG(active_op_array)->opcodes[second_semicolon_token->u.op.opline_num].extended_value = get_next_op_number(CG(active_op_array));
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);

	do_begin_loop(TSRMLS_C);

	INC_BPC(CG(active_op_array));
}
/* }}} */

void zend_do_for_end(const znode *second_semicolon_token TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMP;
	opline->op1.opline_num = second_semicolon_token->u.op.opline_num+1;
	CG(active_op_array)->opcodes[second_semicolon_token->u.op.opline_num].op2.opline_num = get_next_op_number(CG(active_op_array));
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);

	do_end_loop(second_semicolon_token->u.op.opline_num+1, 0 TSRMLS_CC);

	DEC_BPC(CG(active_op_array));
}
/* }}} */

void zend_do_pre_incdec(znode *result, const znode *op1, zend_uchar op TSRMLS_DC) /* {{{ */
{
	int last_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline;

	if (last_op_number > 0) {
		zend_op *last_op = &CG(active_op_array)->opcodes[last_op_number-1];

		if (last_op->opcode == ZEND_FETCH_OBJ_RW) {
			last_op->opcode = (op==ZEND_PRE_INC)?ZEND_PRE_INC_OBJ:ZEND_PRE_DEC_OBJ;
			last_op->result_type = IS_VAR;
			last_op->result.var = get_temporary_variable(CG(active_op_array));
			GET_NODE(result, last_op->result);
			return;
		}
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = op;
	SET_NODE(opline->op1, op1);
	SET_UNUSED(opline->op2);
	opline->result_type = IS_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	GET_NODE(result, opline->result);
}
/* }}} */

void zend_do_post_incdec(znode *result, const znode *op1, zend_uchar op TSRMLS_DC) /* {{{ */
{
	int last_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline;

	if (last_op_number > 0) {
		zend_op *last_op = &CG(active_op_array)->opcodes[last_op_number-1];

		if (last_op->opcode == ZEND_FETCH_OBJ_RW) {
			last_op->opcode = (op==ZEND_POST_INC)?ZEND_POST_INC_OBJ:ZEND_POST_DEC_OBJ;
			last_op->result_type = IS_TMP_VAR;
			last_op->result.var = get_temporary_variable(CG(active_op_array));
			GET_NODE(result, last_op->result);
			return;
		}
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = op;
	SET_NODE(opline->op1, op1);
	SET_UNUSED(opline->op2);
	opline->result_type = IS_TMP_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	GET_NODE(result, opline->result);
}
/* }}} */

void zend_do_if_cond(const znode *cond, znode *closing_bracket_token TSRMLS_DC) /* {{{ */
{
	int if_cond_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMPZ;
	SET_NODE(opline->op1, cond);
	closing_bracket_token->u.op.opline_num = if_cond_op_number;
	SET_UNUSED(opline->op2);
	INC_BPC(CG(active_op_array));
}
/* }}} */

void zend_do_if_after_statement(const znode *closing_bracket_token, unsigned char initialize TSRMLS_DC) /* {{{ */
{
	int if_end_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	zend_llist *jmp_list_ptr;

	opline->opcode = ZEND_JMP;
	/* save for backpatching */
	if (initialize) {
		zend_llist jmp_list;

		zend_llist_init(&jmp_list, sizeof(int), NULL, 0);
		zend_stack_push(&CG(bp_stack), (void *) &jmp_list, sizeof(zend_llist));
	}
	zend_stack_top(&CG(bp_stack), (void **) &jmp_list_ptr);
	zend_llist_add_element(jmp_list_ptr, &if_end_op_number);

	CG(active_op_array)->opcodes[closing_bracket_token->u.op.opline_num].op2.opline_num = if_end_op_number+1;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}
/* }}} */

void zend_do_if_end(TSRMLS_D) /* {{{ */
{
	int next_op_number = get_next_op_number(CG(active_op_array));
	zend_llist *jmp_list_ptr;
	zend_llist_element *le;

	zend_stack_top(&CG(bp_stack), (void **) &jmp_list_ptr);
	for (le=jmp_list_ptr->head; le; le = le->next) {
		CG(active_op_array)->opcodes[*((int *) le->data)].op1.opline_num = next_op_number;
	}
	zend_llist_destroy(jmp_list_ptr);
	zend_stack_del_top(&CG(bp_stack));
	DEC_BPC(CG(active_op_array));
}
/* }}} */

void zend_check_writable_variable(const znode *variable) /* {{{ */
{
	zend_uint type = variable->EA;

	if (type & ZEND_PARSED_METHOD_CALL) {
		zend_error_noreturn(E_COMPILE_ERROR, "Can't use method return value in write context");
	}
	if (type == ZEND_PARSED_FUNCTION_CALL) {
		zend_error_noreturn(E_COMPILE_ERROR, "Can't use function return value in write context");
	}
}
/* }}} */

void zend_do_begin_variable_parse(TSRMLS_D) /* {{{ */
{
	zend_llist fetch_list;

	zend_llist_init(&fetch_list, sizeof(zend_op), NULL, 0);
	zend_stack_push(&CG(bp_stack), (void *) &fetch_list, sizeof(zend_llist));
}
/* }}} */

void zend_do_end_variable_parse(znode *variable, int type, int arg_offset TSRMLS_DC) /* {{{ */
{
	zend_llist *fetch_list_ptr;
	zend_llist_element *le;
	zend_op *opline = NULL;
	zend_op *opline_ptr;
	zend_uint this_var = -1;

	zend_stack_top(&CG(bp_stack), (void **) &fetch_list_ptr);

	le = fetch_list_ptr->head;

	/* TODO: $foo->x->y->z = 1 should fetch "x" and "y" for R or RW, not just W */

	if (le) {
		opline_ptr = (zend_op *)le->data;
		if (opline_is_fetch_this(opline_ptr TSRMLS_CC)) {
			/* convert to FETCH_?(this) into IS_CV */
			if (CG(active_op_array)->last == 0 ||
			    CG(active_op_array)->opcodes[CG(active_op_array)->last-1].opcode != ZEND_BEGIN_SILENCE) {

				this_var = opline_ptr->result.var;
				if (CG(active_op_array)->this_var == -1) {
					CG(active_op_array)->this_var = lookup_cv(CG(active_op_array), Z_STRVAL(CONSTANT(opline_ptr->op1.constant)), Z_STRLEN(CONSTANT(opline_ptr->op1.constant)), Z_HASH_P(&CONSTANT(opline_ptr->op1.constant)) TSRMLS_CC);
					Z_TYPE(CONSTANT(opline_ptr->op1.constant)) = IS_NULL;
				} else {
					zend_del_literal(CG(active_op_array), opline_ptr->op1.constant);
				}
				le = le->next;
				if (variable->op_type == IS_VAR &&
				    variable->u.op.var == this_var) {
					variable->op_type = IS_CV;
					variable->u.op.var = CG(active_op_array)->this_var;
				}
			} else if (CG(active_op_array)->this_var == -1) {
				CG(active_op_array)->this_var = lookup_cv(CG(active_op_array), estrndup("this", sizeof("this")-1), sizeof("this")-1, THIS_HASHVAL TSRMLS_CC);
			}
		}

		while (le) {
			opline_ptr = (zend_op *)le->data;
			if (opline_ptr->opcode == ZEND_SEPARATE) {
				if (type != BP_VAR_R && type != BP_VAR_IS) {
					opline = get_next_op(CG(active_op_array) TSRMLS_CC);
					memcpy(opline, opline_ptr, sizeof(zend_op));
				}
				le = le->next;
				continue;
			}
			opline = get_next_op(CG(active_op_array) TSRMLS_CC);
			memcpy(opline, opline_ptr, sizeof(zend_op));
			if (opline->op1_type == IS_VAR &&
			    opline->op1.var == this_var) {
				opline->op1_type = IS_CV;
				opline->op1.var = CG(active_op_array)->this_var;
			}
			switch (type) {
				case BP_VAR_R:
					if (opline->opcode == ZEND_FETCH_DIM_W && opline->op2_type == IS_UNUSED) {
						zend_error_noreturn(E_COMPILE_ERROR, "Cannot use [] for reading");
					}
					opline->opcode -= 3;
					break;
				case BP_VAR_W:
					break;
				case BP_VAR_RW:
					opline->opcode += 3;
					break;
				case BP_VAR_IS:
					if (opline->opcode == ZEND_FETCH_DIM_W && opline->op2_type == IS_UNUSED) {
						zend_error_noreturn(E_COMPILE_ERROR, "Cannot use [] for reading");
					}
					opline->opcode += 6; /* 3+3 */
					break;
				case BP_VAR_FUNC_ARG:
					opline->opcode += 9; /* 3+3+3 */
					opline->extended_value |= arg_offset;
					break;
				case BP_VAR_UNSET:
					if (opline->opcode == ZEND_FETCH_DIM_W && opline->op2_type == IS_UNUSED) {
						zend_error_noreturn(E_COMPILE_ERROR, "Cannot use [] for unsetting");
					}
					opline->opcode += 12; /* 3+3+3+3 */
					break;
			}
			le = le->next;
		}
		if (opline && type == BP_VAR_W && arg_offset) {
			opline->extended_value |= ZEND_FETCH_MAKE_REF;
		}
	}
	zend_llist_destroy(fetch_list_ptr);
	zend_stack_del_top(&CG(bp_stack));
}
/* }}} */

void zend_do_add_string(znode *result, const znode *op1, znode *op2 TSRMLS_DC) /* {{{ */
{
	zend_op *opline;

	if (Z_STRLEN(op2->u.constant) > 1) {
		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = ZEND_ADD_STRING;
	} else if (Z_STRLEN(op2->u.constant) == 1) {
		int ch = *Z_STRVAL(op2->u.constant);

		/* Free memory and use ZEND_ADD_CHAR in case of 1 character strings */
		efree(Z_STRVAL(op2->u.constant));
		ZVAL_LONG(&op2->u.constant, ch);
		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = ZEND_ADD_CHAR;
	} else { /* String can be empty after a variable at the end of a heredoc */
		efree(Z_STRVAL(op2->u.constant));
		return;
	}

	if (op1) {
		SET_NODE(opline->op1, op1);
		SET_NODE(opline->result, op1);
	} else {
		SET_UNUSED(opline->op1);
		opline->result_type = IS_TMP_VAR;
		opline->result.var = get_temporary_variable(CG(active_op_array));
	}
	SET_NODE(opline->op2, op2);
	GET_NODE(result, opline->result);
}
/* }}} */

void zend_do_add_variable(znode *result, const znode *op1, const znode *op2 TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_ADD_VAR;

	if (op1) {
		SET_NODE(opline->op1, op1);
		SET_NODE(opline->result, op1);
	} else {
		SET_UNUSED(opline->op1);
		opline->result_type = IS_TMP_VAR;
		opline->result.var = get_temporary_variable(CG(active_op_array));
	}
	SET_NODE(opline->op2, op2);
	GET_NODE(result, opline->result);
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
			    opline->opcode == ZEND_FETCH_OBJ_R ||
			    opline->opcode == ZEND_QM_ASSIGN_VAR) {
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
					}
					break;
				}
				opline--;
			}
		}
	} else if (op1->op_type == IS_CONST) {
		zval_dtor(&op1->u.constant);
	}
}
/* }}} */

int zend_do_verify_access_types(const znode *current_access_type, const znode *new_modifier) /* {{{ */
{
	if ((Z_LVAL(current_access_type->u.constant) & ZEND_ACC_PPP_MASK)
		&& (Z_LVAL(new_modifier->u.constant) & ZEND_ACC_PPP_MASK)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Multiple access type modifiers are not allowed");
	}
	if ((Z_LVAL(current_access_type->u.constant) & ZEND_ACC_ABSTRACT)
		&& (Z_LVAL(new_modifier->u.constant) & ZEND_ACC_ABSTRACT)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Multiple abstract modifiers are not allowed");
	}
	if ((Z_LVAL(current_access_type->u.constant) & ZEND_ACC_STATIC)
		&& (Z_LVAL(new_modifier->u.constant) & ZEND_ACC_STATIC)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Multiple static modifiers are not allowed");
	}
	if ((Z_LVAL(current_access_type->u.constant) & ZEND_ACC_FINAL)
		&& (Z_LVAL(new_modifier->u.constant) & ZEND_ACC_FINAL)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Multiple final modifiers are not allowed");
	}
	if (((Z_LVAL(current_access_type->u.constant) | Z_LVAL(new_modifier->u.constant)) & (ZEND_ACC_ABSTRACT | ZEND_ACC_FINAL)) == (ZEND_ACC_ABSTRACT | ZEND_ACC_FINAL)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use the final modifier on an abstract class member");
	}
	return (Z_LVAL(current_access_type->u.constant) | Z_LVAL(new_modifier->u.constant));
}
/* }}} */

void zend_do_begin_function_declaration(znode *function_token, znode *function_name, int is_method, int return_reference, znode *fn_flags_znode TSRMLS_DC) /* {{{ */
{
	zend_op_array op_array;
	char *name = Z_STRVAL(function_name->u.constant);
	int name_len = Z_STRLEN(function_name->u.constant);
	int function_begin_line = function_token->u.op.opline_num;
	zend_uint fn_flags;
	const char *lcname;
	zend_bool orig_interactive;
	ALLOCA_FLAG(use_heap)

	if (is_method) {
		if (CG(active_class_entry)->ce_flags & ZEND_ACC_INTERFACE) {
			if ((Z_LVAL(fn_flags_znode->u.constant) & ~(ZEND_ACC_STATIC|ZEND_ACC_PUBLIC))) {
				zend_error_noreturn(E_COMPILE_ERROR, "Access type for interface method %s::%s() must be omitted", CG(active_class_entry)->name, Z_STRVAL(function_name->u.constant));
			}
			Z_LVAL(fn_flags_znode->u.constant) |= ZEND_ACC_ABSTRACT; /* propagates to the rest of the parser */
		}
		fn_flags = Z_LVAL(fn_flags_znode->u.constant); /* must be done *after* the above check */
	} else {
		fn_flags = 0;
	}
	if ((fn_flags & ZEND_ACC_STATIC) && (fn_flags & ZEND_ACC_ABSTRACT) && !(CG(active_class_entry)->ce_flags & ZEND_ACC_INTERFACE)) {
		zend_error(E_STRICT, "Static function %s%s%s() should not be abstract", is_method ? CG(active_class_entry)->name : "", is_method ? "::" : "", Z_STRVAL(function_name->u.constant));
	}

	function_token->u.op_array = CG(active_op_array);

	orig_interactive = CG(interactive);
	CG(interactive) = 0;
	init_op_array(&op_array, ZEND_USER_FUNCTION, INITIAL_OP_ARRAY_SIZE TSRMLS_CC);
	CG(interactive) = orig_interactive;

	op_array.function_name = name;
	if (return_reference) {
		op_array.fn_flags |= ZEND_ACC_RETURN_REFERENCE;
	}
	op_array.fn_flags |= fn_flags;

	op_array.scope = is_method?CG(active_class_entry):NULL;
	op_array.prototype = NULL;

	op_array.line_start = zend_get_compiled_lineno(TSRMLS_C);

	if (is_method) {
		zend_ulong hash;

		lcname = zend_new_interned_string(zend_str_tolower_dup(name, name_len), name_len + 1, 1 TSRMLS_CC);
		hash = str_hash(lcname, name_len);
		if (zend_hash_quick_add(&CG(active_class_entry)->function_table, lcname, name_len+1, hash, &op_array, sizeof(zend_op_array), (void **) &CG(active_op_array)) == FAILURE) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare %s::%s()", CG(active_class_entry)->name, name);
		}

		zend_stack_push(&CG(context_stack), (void *) &CG(context), sizeof(CG(context)));
		zend_init_compiler_context(TSRMLS_C);

		if (fn_flags & ZEND_ACC_ABSTRACT) {
			CG(active_class_entry)->ce_flags |= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
		}

		if (!(fn_flags & ZEND_ACC_PPP_MASK)) {
			fn_flags |= ZEND_ACC_PUBLIC;
		}

		if (CG(active_class_entry)->ce_flags & ZEND_ACC_INTERFACE) {
			if ((name_len == sizeof(ZEND_CALL_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_CALL_FUNC_NAME, sizeof(ZEND_CALL_FUNC_NAME)-1))) {
				if (fn_flags & ((ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC) ^ ZEND_ACC_PUBLIC)) {
					zend_error(E_WARNING, "The magic method __call() must have public visibility and cannot be static");
				}
			} else if ((name_len == sizeof(ZEND_CALLSTATIC_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_CALLSTATIC_FUNC_NAME, sizeof(ZEND_CALLSTATIC_FUNC_NAME)-1))) {
				if ((fn_flags & (ZEND_ACC_PPP_MASK ^ ZEND_ACC_PUBLIC)) || (fn_flags & ZEND_ACC_STATIC) == 0) {
					zend_error(E_WARNING, "The magic method __callStatic() must have public visibility and be static");
				}
			} else if ((name_len == sizeof(ZEND_GET_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_GET_FUNC_NAME, sizeof(ZEND_GET_FUNC_NAME)-1))) {
				if (fn_flags & ((ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC) ^ ZEND_ACC_PUBLIC)) {
					zend_error(E_WARNING, "The magic method __get() must have public visibility and cannot be static");
				}
			} else if ((name_len == sizeof(ZEND_SET_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_SET_FUNC_NAME, sizeof(ZEND_SET_FUNC_NAME)-1))) {
				if (fn_flags & ((ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC) ^ ZEND_ACC_PUBLIC)) {
					zend_error(E_WARNING, "The magic method __set() must have public visibility and cannot be static");
				}
			} else if ((name_len == sizeof(ZEND_UNSET_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_UNSET_FUNC_NAME, sizeof(ZEND_UNSET_FUNC_NAME)-1))) {
				if (fn_flags & ((ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC) ^ ZEND_ACC_PUBLIC)) {
					zend_error(E_WARNING, "The magic method __unset() must have public visibility and cannot be static");
				}
			} else if ((name_len == sizeof(ZEND_ISSET_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_ISSET_FUNC_NAME, sizeof(ZEND_ISSET_FUNC_NAME)-1))) {
				if (fn_flags & ((ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC) ^ ZEND_ACC_PUBLIC)) {
					zend_error(E_WARNING, "The magic method __isset() must have public visibility and cannot be static");
				}
			} else if ((name_len == sizeof(ZEND_TOSTRING_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_TOSTRING_FUNC_NAME, sizeof(ZEND_TOSTRING_FUNC_NAME)-1))) {
				if (fn_flags & ((ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC) ^ ZEND_ACC_PUBLIC)) {
					zend_error(E_WARNING, "The magic method __toString() must have public visibility and cannot be static");
				}
			} else if ((name_len == sizeof(ZEND_INVOKE_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_INVOKE_FUNC_NAME, sizeof(ZEND_INVOKE_FUNC_NAME)-1))) {
				if (fn_flags & ((ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC) ^ ZEND_ACC_PUBLIC)) {
					zend_error(E_WARNING, "The magic method __invoke() must have public visibility and cannot be static");
				}

			} else if ((name_len == sizeof(ZEND_DEBUGINFO_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_DEBUGINFO_FUNC_NAME, sizeof(ZEND_DEBUGINFO_FUNC_NAME)-1))) {
				if (fn_flags & ((ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC) ^ ZEND_ACC_PUBLIC)) {
					zend_error(E_WARNING, "The magic method __debugInfo() must have public visibility and cannot be static");
				}
			}
		} else {
			char *class_lcname;

			class_lcname = do_alloca(CG(active_class_entry)->name_length + 1, use_heap);
			zend_str_tolower_copy(class_lcname, CG(active_class_entry)->name, CG(active_class_entry)->name_length);
			/* Improve after RC: cache the lowercase class name */

			if ((CG(active_class_entry)->name_length == name_len) && ((CG(active_class_entry)->ce_flags & ZEND_ACC_TRAIT) != ZEND_ACC_TRAIT) && (!memcmp(class_lcname, lcname, name_len))) {
				if (!CG(active_class_entry)->constructor) {
					CG(active_class_entry)->constructor = (zend_function *) CG(active_op_array);
				}
			} else if ((name_len == sizeof(ZEND_CONSTRUCTOR_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_CONSTRUCTOR_FUNC_NAME, sizeof(ZEND_CONSTRUCTOR_FUNC_NAME)))) {
				if (CG(active_class_entry)->constructor) {
					zend_error(E_STRICT, "Redefining already defined constructor for class %s", CG(active_class_entry)->name);
				}
				CG(active_class_entry)->constructor = (zend_function *) CG(active_op_array);
			} else if ((name_len == sizeof(ZEND_DESTRUCTOR_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_DESTRUCTOR_FUNC_NAME, sizeof(ZEND_DESTRUCTOR_FUNC_NAME)-1))) {
				CG(active_class_entry)->destructor = (zend_function *) CG(active_op_array);
			} else if ((name_len == sizeof(ZEND_CLONE_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_CLONE_FUNC_NAME, sizeof(ZEND_CLONE_FUNC_NAME)-1))) {
				CG(active_class_entry)->clone = (zend_function *) CG(active_op_array);
			} else if ((name_len == sizeof(ZEND_CALL_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_CALL_FUNC_NAME, sizeof(ZEND_CALL_FUNC_NAME)-1))) {
				if (fn_flags & ((ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC) ^ ZEND_ACC_PUBLIC)) {
					zend_error(E_WARNING, "The magic method __call() must have public visibility and cannot be static");
				}
				CG(active_class_entry)->__call = (zend_function *) CG(active_op_array);
			} else if ((name_len == sizeof(ZEND_CALLSTATIC_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_CALLSTATIC_FUNC_NAME, sizeof(ZEND_CALLSTATIC_FUNC_NAME)-1))) {
				if ((fn_flags & (ZEND_ACC_PPP_MASK ^ ZEND_ACC_PUBLIC)) || (fn_flags & ZEND_ACC_STATIC) == 0) {
					zend_error(E_WARNING, "The magic method __callStatic() must have public visibility and be static");
				}
				CG(active_class_entry)->__callstatic = (zend_function *) CG(active_op_array);
			} else if ((name_len == sizeof(ZEND_GET_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_GET_FUNC_NAME, sizeof(ZEND_GET_FUNC_NAME)-1))) {
				if (fn_flags & ((ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC) ^ ZEND_ACC_PUBLIC)) {
					zend_error(E_WARNING, "The magic method __get() must have public visibility and cannot be static");
				}
				CG(active_class_entry)->__get = (zend_function *) CG(active_op_array);
			} else if ((name_len == sizeof(ZEND_SET_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_SET_FUNC_NAME, sizeof(ZEND_SET_FUNC_NAME)-1))) {
				if (fn_flags & ((ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC) ^ ZEND_ACC_PUBLIC)) {
					zend_error(E_WARNING, "The magic method __set() must have public visibility and cannot be static");
				}
				CG(active_class_entry)->__set = (zend_function *) CG(active_op_array);
			} else if ((name_len == sizeof(ZEND_UNSET_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_UNSET_FUNC_NAME, sizeof(ZEND_UNSET_FUNC_NAME)-1))) {
				if (fn_flags & ((ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC) ^ ZEND_ACC_PUBLIC)) {
					zend_error(E_WARNING, "The magic method __unset() must have public visibility and cannot be static");
				}
				CG(active_class_entry)->__unset = (zend_function *) CG(active_op_array);
			} else if ((name_len == sizeof(ZEND_ISSET_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_ISSET_FUNC_NAME, sizeof(ZEND_ISSET_FUNC_NAME)-1))) {
				if (fn_flags & ((ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC) ^ ZEND_ACC_PUBLIC)) {
					zend_error(E_WARNING, "The magic method __isset() must have public visibility and cannot be static");
				}
				CG(active_class_entry)->__isset = (zend_function *) CG(active_op_array);
			} else if ((name_len == sizeof(ZEND_TOSTRING_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_TOSTRING_FUNC_NAME, sizeof(ZEND_TOSTRING_FUNC_NAME)-1))) {
				if (fn_flags & ((ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC) ^ ZEND_ACC_PUBLIC)) {
					zend_error(E_WARNING, "The magic method __toString() must have public visibility and cannot be static");
				}
				CG(active_class_entry)->__tostring = (zend_function *) CG(active_op_array);
			} else if ((name_len == sizeof(ZEND_INVOKE_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_INVOKE_FUNC_NAME, sizeof(ZEND_INVOKE_FUNC_NAME)-1))) {
				if (fn_flags & ((ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC) ^ ZEND_ACC_PUBLIC)) {
					zend_error(E_WARNING, "The magic method __invoke() must have public visibility and cannot be static");
				}
			} else if ((name_len == sizeof(ZEND_DEBUGINFO_FUNC_NAME)-1) && (!memcmp(lcname, ZEND_DEBUGINFO_FUNC_NAME, sizeof(ZEND_DEBUGINFO_FUNC_NAME)-1))) {
				if (fn_flags & ((ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC) ^ ZEND_ACC_PUBLIC)) {
					zend_error(E_WARNING, "The magic method __debugInfo() must have public visibility and cannot be static");
				}
				CG(active_class_entry)->__debugInfo = (zend_function *) CG(active_op_array);
			} else if (!(fn_flags & ZEND_ACC_STATIC)) {
				CG(active_op_array)->fn_flags |= ZEND_ACC_ALLOW_STATIC;
			}
			free_alloca(class_lcname, use_heap);
		}

		str_efree(lcname);
	} else {
		zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		zval key;
		zval **ns_name;

		if (CG(current_namespace)) {
			/* Prefix function name with current namespace name */
			znode tmp;

			tmp.u.constant = *CG(current_namespace);
			zval_copy_ctor(&tmp.u.constant);
			zend_do_build_namespace_name(&tmp, &tmp, function_name TSRMLS_CC);
			op_array.function_name = Z_STRVAL(tmp.u.constant);
			name_len = Z_STRLEN(tmp.u.constant);
			lcname = zend_str_tolower_dup(Z_STRVAL(tmp.u.constant), name_len);
		} else {
			lcname = zend_str_tolower_dup(name, name_len);
		}

		/* Function name must not conflict with import names */
		if (CG(current_import_function) &&
		    zend_hash_find(CG(current_import_function), lcname, Z_STRLEN(function_name->u.constant)+1, (void**)&ns_name) == SUCCESS) {

			char *tmp = zend_str_tolower_dup(Z_STRVAL_PP(ns_name), Z_STRLEN_PP(ns_name));

			if (Z_STRLEN_PP(ns_name) != Z_STRLEN(function_name->u.constant) ||
				memcmp(tmp, lcname, Z_STRLEN(function_name->u.constant))) {
				zend_error(E_COMPILE_ERROR, "Cannot declare function %s because the name is already in use", Z_STRVAL(function_name->u.constant));
			}
			efree(tmp);
		}

		opline->opcode = ZEND_DECLARE_FUNCTION;
		opline->op1_type = IS_CONST;
		build_runtime_defined_function_key(&key, lcname, name_len TSRMLS_CC);
		opline->op1.constant = zend_add_literal(CG(active_op_array), &key TSRMLS_CC);
		Z_HASH_P(&CONSTANT(opline->op1.constant)) = zend_hash_func(Z_STRVAL(CONSTANT(opline->op1.constant)), Z_STRLEN(CONSTANT(opline->op1.constant)));
		opline->op2_type = IS_CONST;
		LITERAL_STRINGL(opline->op2, lcname, name_len, 1);
		CALCULATE_LITERAL_HASH(opline->op2.constant);
		opline->extended_value = ZEND_DECLARE_FUNCTION;
		zend_hash_quick_update(CG(function_table), Z_STRVAL(key), Z_STRLEN(key), Z_HASH_P(&CONSTANT(opline->op1.constant)), &op_array, sizeof(zend_op_array), (void **) &CG(active_op_array));
		zend_stack_push(&CG(context_stack), (void *) &CG(context), sizeof(CG(context)));
		zend_init_compiler_context(TSRMLS_C);
		str_efree(lcname);
	}

	if (CG(compiler_options) & ZEND_COMPILE_EXTENDED_INFO) {
		zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

		opline->opcode = ZEND_EXT_NOP;
		opline->lineno = function_begin_line;
		SET_UNUSED(opline->op1);
		SET_UNUSED(opline->op2);
	}

	{
		/* Push a separator to the switch stack */
		zend_switch_entry switch_entry;

		switch_entry.cond.op_type = IS_UNUSED;
		switch_entry.default_case = 0;
		switch_entry.control_var = 0;

		zend_stack_push(&CG(switch_cond_stack), (void *) &switch_entry, sizeof(switch_entry));
	}

	{
		/* Push a separator to the foreach stack */
		zend_op dummy_opline;

		dummy_opline.result_type = IS_UNUSED;

		zend_stack_push(&CG(foreach_copy_stack), (void *) &dummy_opline, sizeof(zend_op));
	}

	if (CG(doc_comment)) {
		CG(active_op_array)->doc_comment = CG(doc_comment);
		CG(active_op_array)->doc_comment_len = CG(doc_comment_len);
		CG(doc_comment) = NULL;
		CG(doc_comment_len) = 0;
	}
}
/* }}} */

void zend_do_begin_lambda_function_declaration(znode *result, znode *function_token, int return_reference, int is_static TSRMLS_DC) /* {{{ */
{
	znode          function_name;
	zend_op_array *current_op_array = CG(active_op_array);
	int            current_op_number = get_next_op_number(CG(active_op_array));
	zend_op       *current_op;

	function_name.op_type = IS_CONST;
	ZVAL_STRINGL(&function_name.u.constant, "{closure}", sizeof("{closure}")-1, 1);

	zend_do_begin_function_declaration(function_token, &function_name, 0, return_reference, NULL TSRMLS_CC);

	result->op_type = IS_TMP_VAR;
	result->u.op.var = get_temporary_variable(current_op_array);

	current_op = &current_op_array->opcodes[current_op_number];
	current_op->opcode = ZEND_DECLARE_LAMBDA_FUNCTION;
	zend_del_literal(current_op_array, current_op->op2.constant);
	SET_UNUSED(current_op->op2);
	SET_NODE(current_op->result, result);
	if (is_static) {
		CG(active_op_array)->fn_flags |= ZEND_ACC_STATIC;
	}
	CG(active_op_array)->fn_flags |= ZEND_ACC_CLOSURE;
}
/* }}} */

void zend_do_handle_exception(TSRMLS_D) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_HANDLE_EXCEPTION;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}
/* }}} */

void zend_do_end_function_declaration(const znode *function_token TSRMLS_DC) /* {{{ */
{
	char lcname[16];
	int name_len;

	zend_do_extended_info(TSRMLS_C);
	zend_do_return(NULL, 0 TSRMLS_CC);

	pass_two(CG(active_op_array) TSRMLS_CC);
	zend_release_labels(0 TSRMLS_CC);

	if (CG(active_class_entry)) {
		zend_check_magic_method_implementation(CG(active_class_entry), (zend_function*)CG(active_op_array), E_COMPILE_ERROR TSRMLS_CC);
	} else {
		/* we don't care if the function name is longer, in fact lowercasing only
		 * the beginning of the name speeds up the check process */
		name_len = strlen(CG(active_op_array)->function_name);
		zend_str_tolower_copy(lcname, CG(active_op_array)->function_name, MIN(name_len, sizeof(lcname)-1));
		lcname[sizeof(lcname)-1] = '\0'; /* zend_str_tolower_copy won't necessarily set the zero byte */
		if (name_len == sizeof(ZEND_AUTOLOAD_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_AUTOLOAD_FUNC_NAME, sizeof(ZEND_AUTOLOAD_FUNC_NAME)) && CG(active_op_array)->num_args != 1) {
			zend_error_noreturn(E_COMPILE_ERROR, "%s() must take exactly 1 argument", ZEND_AUTOLOAD_FUNC_NAME);
		}
	}

	CG(active_op_array)->line_end = zend_get_compiled_lineno(TSRMLS_C);
	CG(active_op_array) = function_token->u.op_array;


	/* Pop the switch and foreach separators */
	zend_stack_del_top(&CG(switch_cond_stack));
	zend_stack_del_top(&CG(foreach_copy_stack));
}
/* }}} */

void zend_do_receive_param(zend_uchar op, znode *varname, const znode *initialization, znode *class_type, zend_uchar pass_by_reference, zend_bool is_variadic TSRMLS_DC) /* {{{ */
{
	zend_op *opline;
	zend_arg_info *cur_arg_info;
	znode var;

	if (zend_is_auto_global(Z_STRVAL(varname->u.constant), Z_STRLEN(varname->u.constant) TSRMLS_CC)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot re-assign auto-global variable %s", Z_STRVAL(varname->u.constant));
	} else {
		var.op_type = IS_CV;
		var.u.op.var = lookup_cv(CG(active_op_array), Z_STRVAL(varname->u.constant), Z_STRLEN(varname->u.constant), 0 TSRMLS_CC);
		Z_STRVAL(varname->u.constant) = (char*)CG(active_op_array)->vars[var.u.op.var].name;
		var.EA = 0;
		if (CG(active_op_array)->vars[var.u.op.var].hash_value == THIS_HASHVAL &&
			Z_STRLEN(varname->u.constant) == sizeof("this")-1 &&
		    !memcmp(Z_STRVAL(varname->u.constant), "this", sizeof("this")-1)) {
			if (CG(active_op_array)->scope &&
			    (CG(active_op_array)->fn_flags & ZEND_ACC_STATIC) == 0) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot re-assign $this");
			}
			CG(active_op_array)->this_var = var.u.op.var;
		}
	}

	if (CG(active_op_array)->fn_flags & ZEND_ACC_VARIADIC) {
		zend_error_noreturn(E_COMPILE_ERROR, "Only the last parameter can be variadic");
	}

	if (is_variadic) {
		if (op == ZEND_RECV_INIT) {
			zend_error_noreturn(E_COMPILE_ERROR, "Variadic parameter cannot have a default value");
		}

		op = ZEND_RECV_VARIADIC;
		CG(active_op_array)->fn_flags |= ZEND_ACC_VARIADIC;
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	CG(active_op_array)->num_args++;
	opline->opcode = op;
	SET_NODE(opline->result, &var);
	opline->op1_type = IS_UNUSED;
	opline->op1.num = CG(active_op_array)->num_args;
	if (op == ZEND_RECV_INIT) {
		SET_NODE(opline->op2, initialization);
	} else {
		SET_UNUSED(opline->op2);
		if (!is_variadic) {
			CG(active_op_array)->required_num_args = CG(active_op_array)->num_args;
		}
	}
	CG(active_op_array)->arg_info = erealloc(CG(active_op_array)->arg_info, sizeof(zend_arg_info)*(CG(active_op_array)->num_args));
	cur_arg_info = &CG(active_op_array)->arg_info[CG(active_op_array)->num_args-1];
	cur_arg_info->name = zend_new_interned_string(estrndup(Z_STRVAL(varname->u.constant), Z_STRLEN(varname->u.constant)), Z_STRLEN(varname->u.constant) + 1, 1 TSRMLS_CC);
	cur_arg_info->name_len = Z_STRLEN(varname->u.constant);
	cur_arg_info->type_hint = 0;
	cur_arg_info->pass_by_reference = pass_by_reference;
	cur_arg_info->allow_null = 1;
	cur_arg_info->is_variadic = is_variadic;
	cur_arg_info->class_name = NULL;
	cur_arg_info->class_name_len = 0;

	if (class_type->op_type != IS_UNUSED) {
		cur_arg_info->allow_null = 0;

		if (class_type->u.constant.type != IS_NULL) {
			if (class_type->u.constant.type == IS_ARRAY) {
				cur_arg_info->type_hint = IS_ARRAY;
				if (op == ZEND_RECV_INIT) {
					if (Z_TYPE(initialization->u.constant) == IS_NULL || (Z_TYPE(initialization->u.constant) == IS_CONSTANT && !strcasecmp(Z_STRVAL(initialization->u.constant), "NULL"))) {
						cur_arg_info->allow_null = 1;
					} else if (IS_CONSTANT_TYPE(Z_TYPE(initialization->u.constant))) {
						/* delay constant resolution and check to run-time */
						cur_arg_info->allow_null = 0;
					} else if (Z_TYPE(initialization->u.constant) != IS_ARRAY) {
						zend_error_noreturn(E_COMPILE_ERROR, "Default value for parameters with array type hint can only be an array or NULL");
					}
				}
			} else if (class_type->u.constant.type == IS_CALLABLE) {
				cur_arg_info->type_hint = IS_CALLABLE;
				if (op == ZEND_RECV_INIT) {
					if (Z_TYPE(initialization->u.constant) == IS_NULL || (Z_TYPE(initialization->u.constant) == IS_CONSTANT && !strcasecmp(Z_STRVAL(initialization->u.constant), "NULL"))) {
						cur_arg_info->allow_null = 1;
					} else if (IS_CONSTANT_TYPE(Z_TYPE(initialization->u.constant))) {
						/* delay constant resolution and check to run-time */
						cur_arg_info->allow_null = 0;
					} else {
						zend_error_noreturn(E_COMPILE_ERROR, "Default value for parameters with callable type hint can only be NULL");
					}
				}
			} else {
				cur_arg_info->type_hint = IS_OBJECT;
				if (ZEND_FETCH_CLASS_DEFAULT == zend_get_class_fetch_type(Z_STRVAL(class_type->u.constant), Z_STRLEN(class_type->u.constant))) {
					zend_resolve_class_name(class_type TSRMLS_CC);
				}
				Z_STRVAL(class_type->u.constant) = (char*)zend_new_interned_string(Z_STRVAL(class_type->u.constant), Z_STRLEN(class_type->u.constant) + 1, 1 TSRMLS_CC);
				cur_arg_info->class_name = Z_STRVAL(class_type->u.constant);
				cur_arg_info->class_name_len = Z_STRLEN(class_type->u.constant);
				if (op == ZEND_RECV_INIT) {
					if (Z_TYPE(initialization->u.constant) == IS_NULL || (Z_TYPE(initialization->u.constant) == IS_CONSTANT && !strcasecmp(Z_STRVAL(initialization->u.constant), "NULL"))) {
						cur_arg_info->allow_null = 1;
					} else if (IS_CONSTANT_TYPE(Z_TYPE(initialization->u.constant))) {
						/* delay constant resolution and check to run-time */
						cur_arg_info->allow_null = 0;
					} else {
						zend_error_noreturn(E_COMPILE_ERROR, "Default value for parameters with a class type hint can only be NULL");
					}
				}
			}
		}
	}
}
/* }}} */

int zend_do_begin_function_call(znode *function_name, zend_bool check_namespace TSRMLS_DC) /* {{{ */
{
	zend_function *function;
	char *lcname;
	char *is_compound = memchr(Z_STRVAL(function_name->u.constant), '\\', Z_STRLEN(function_name->u.constant));

	zend_resolve_function_name(function_name, &check_namespace TSRMLS_CC);

	if (check_namespace && CG(current_namespace) && !is_compound) {
			/* We assume we call function from the current namespace
			if it is not prefixed. */

			/* In run-time PHP will check for function with full name and
			internal function with short name */
			zend_do_begin_dynamic_function_call(function_name, 1 TSRMLS_CC);
			return 1;
	}

	lcname = zend_str_tolower_dup(Z_STRVAL(function_name->u.constant), Z_STRLEN(function_name->u.constant));
	if ((zend_hash_find(CG(function_table), lcname, Z_STRLEN(function_name->u.constant)+1, (void **) &function)==FAILURE) ||
	 	((CG(compiler_options) & ZEND_COMPILE_IGNORE_INTERNAL_FUNCTIONS) &&
 		(function->type == ZEND_INTERNAL_FUNCTION))) {
 			zend_do_begin_dynamic_function_call(function_name, 0 TSRMLS_CC);
 			efree(lcname);
 			return 1; /* Dynamic */
 	}
	efree(Z_STRVAL(function_name->u.constant));
	Z_STRVAL(function_name->u.constant) = lcname;

	zend_push_function_call_entry(function TSRMLS_CC);
	if (CG(context).nested_calls + 1 > CG(active_op_array)->nested_calls) {
		CG(active_op_array)->nested_calls = CG(context).nested_calls + 1;
	}
	zend_do_extended_fcall_begin(TSRMLS_C);
	return 0;
}
/* }}} */

void zend_do_begin_method_call(znode *left_bracket TSRMLS_DC) /* {{{ */
{
	zend_op *last_op;
	int last_op_number;

	zend_do_end_variable_parse(left_bracket, BP_VAR_R, 0 TSRMLS_CC);
	zend_do_begin_variable_parse(TSRMLS_C);

	last_op_number = get_next_op_number(CG(active_op_array))-1;
	last_op = &CG(active_op_array)->opcodes[last_op_number];

	if ((last_op->op2_type == IS_CONST) && (Z_TYPE(CONSTANT(last_op->op2.constant)) == IS_STRING) && (Z_STRLEN(CONSTANT(last_op->op2.constant)) == sizeof(ZEND_CLONE_FUNC_NAME)-1)
		&& !zend_binary_strcasecmp(Z_STRVAL(CONSTANT(last_op->op2.constant)), Z_STRLEN(CONSTANT(last_op->op2.constant)), ZEND_CLONE_FUNC_NAME, sizeof(ZEND_CLONE_FUNC_NAME)-1)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot call __clone() method on objects - use 'clone $obj' instead");
	}

	if (last_op->opcode == ZEND_FETCH_OBJ_R) {
		if (last_op->op2_type == IS_CONST) {
			zval name;
			name = CONSTANT(last_op->op2.constant);
			if (Z_TYPE(name) != IS_STRING) {
				zend_error_noreturn(E_COMPILE_ERROR, "Method name must be a string");
			}
			Z_STRVAL(name) = str_estrndup(Z_STRVAL(name), Z_STRLEN(name));
			FREE_POLYMORPHIC_CACHE_SLOT(last_op->op2.constant);
			last_op->op2.constant =
				zend_add_func_name_literal(CG(active_op_array), &name TSRMLS_CC);
			GET_POLYMORPHIC_CACHE_SLOT(last_op->op2.constant);
		}
		last_op->opcode = ZEND_INIT_METHOD_CALL;
		last_op->result_type = IS_UNUSED;
		last_op->result.num = CG(context).nested_calls;
		Z_LVAL(left_bracket->u.constant) = ZEND_INIT_FCALL_BY_NAME;
	} else {
		zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = ZEND_INIT_FCALL_BY_NAME;
		opline->result.num = CG(context).nested_calls;
		SET_UNUSED(opline->op1);
		if (left_bracket->op_type == IS_CONST) {
			opline->op2_type = IS_CONST;
			opline->op2.constant = zend_add_func_name_literal(CG(active_op_array), &left_bracket->u.constant TSRMLS_CC);
			GET_CACHE_SLOT(opline->op2.constant);
		} else {
			SET_NODE(opline->op2, left_bracket);
		}
	}

	zend_push_function_call_entry(NULL TSRMLS_CC);
	if (++CG(context).nested_calls > CG(active_op_array)->nested_calls) {
		CG(active_op_array)->nested_calls = CG(context).nested_calls;
	}
	zend_do_extended_fcall_begin(TSRMLS_C);
}
/* }}} */

void zend_do_clone(znode *result, const znode *expr TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_CLONE;
	SET_NODE(opline->op1, expr);
	SET_UNUSED(opline->op2);
	opline->result_type = IS_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	GET_NODE(result, opline->result);
}
/* }}} */

void zend_do_begin_dynamic_function_call(znode *function_name, int ns_call TSRMLS_DC) /* {{{ */
{
	zend_op *opline;

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	if (ns_call) {
		/* In run-time PHP will check for function with full name and
		   internal function with short name */
		opline->opcode = ZEND_INIT_NS_FCALL_BY_NAME;
		opline->result.num = CG(context).nested_calls;
		SET_UNUSED(opline->op1);
		opline->op2_type = IS_CONST;
		opline->op2.constant = zend_add_ns_func_name_literal(CG(active_op_array), &function_name->u.constant TSRMLS_CC);
		GET_CACHE_SLOT(opline->op2.constant);
	} else {
		opline->opcode = ZEND_INIT_FCALL_BY_NAME;
		opline->result.num = CG(context).nested_calls;
		SET_UNUSED(opline->op1);
		if (function_name->op_type == IS_CONST) {
			opline->op2_type = IS_CONST;
			opline->op2.constant = zend_add_func_name_literal(CG(active_op_array), &function_name->u.constant TSRMLS_CC);
			GET_CACHE_SLOT(opline->op2.constant);
		} else {
			SET_NODE(opline->op2, function_name);
		}
	}

	zend_push_function_call_entry(NULL TSRMLS_CC);
	if (++CG(context).nested_calls > CG(active_op_array)->nested_calls) {
		CG(active_op_array)->nested_calls = CG(context).nested_calls;
	}
	zend_do_extended_fcall_begin(TSRMLS_C);
}
/* }}} */

void zend_resolve_non_class_name(znode *element_name, zend_bool *check_namespace, zend_bool case_sensitive, HashTable *current_import_sub TSRMLS_DC) /* {{{ */
{
	znode tmp;
	int len;
	zval **ns;
	char *lookup_name, *compound = memchr(Z_STRVAL(element_name->u.constant), '\\', Z_STRLEN(element_name->u.constant));

	if (Z_STRVAL(element_name->u.constant)[0] == '\\') {
		/* name starts with \ so it is known and unambiguos, nothing to do here but shorten it */
		memmove(Z_STRVAL(element_name->u.constant), Z_STRVAL(element_name->u.constant)+1, Z_STRLEN(element_name->u.constant));
		--Z_STRLEN(element_name->u.constant);
		return;
	}

	if(!*check_namespace) {
		return;
	}

	if (current_import_sub) {
		len = Z_STRLEN(element_name->u.constant)+1;
		if (case_sensitive) {
			lookup_name = estrndup(Z_STRVAL(element_name->u.constant), len);
		} else {
			lookup_name = zend_str_tolower_dup(Z_STRVAL(element_name->u.constant), len);
		}
		/* Check if function/const matches imported name */
		if (zend_hash_find(current_import_sub, lookup_name, len, (void**)&ns) == SUCCESS) {
			zval_dtor(&element_name->u.constant);
			element_name->u.constant = **ns;
			zval_copy_ctor(&element_name->u.constant);
			efree(lookup_name);
			*check_namespace = 0;
			return;
		}
		efree(lookup_name);
	}

	if (compound && CG(current_import)) {
		len = compound - Z_STRVAL(element_name->u.constant);
		/* namespace is always lowercase */
		lookup_name = zend_str_tolower_dup(Z_STRVAL(element_name->u.constant), len);
		/* Check if first part of compound name is an import name */
		if (zend_hash_find(CG(current_import), lookup_name, len+1, (void**)&ns) == SUCCESS) {
			/* Substitute import name */
			tmp.op_type = IS_CONST;
			tmp.u.constant = **ns;
			zval_copy_ctor(&tmp.u.constant);
			len += 1;
			Z_STRLEN(element_name->u.constant) -= len;
			memmove(Z_STRVAL(element_name->u.constant), Z_STRVAL(element_name->u.constant)+len, Z_STRLEN(element_name->u.constant)+1);
			zend_do_build_namespace_name(&tmp, &tmp, element_name TSRMLS_CC);
			*element_name = tmp;
			efree(lookup_name);
			*check_namespace = 0;
			return;
		}
		efree(lookup_name);
	}

	if (CG(current_namespace)) {
		tmp = *element_name;
		Z_STRLEN(tmp.u.constant) = sizeof("\\")-1 + Z_STRLEN(element_name->u.constant) + Z_STRLEN_P(CG(current_namespace));
		Z_STRVAL(tmp.u.constant) = (char *) emalloc(Z_STRLEN(tmp.u.constant)+1);
		memcpy(Z_STRVAL(tmp.u.constant), Z_STRVAL_P(CG(current_namespace)), Z_STRLEN_P(CG(current_namespace)));
		memcpy(&(Z_STRVAL(tmp.u.constant)[Z_STRLEN_P(CG(current_namespace))]), "\\", sizeof("\\")-1);
		memcpy(&(Z_STRVAL(tmp.u.constant)[Z_STRLEN_P(CG(current_namespace)) + sizeof("\\")-1]), Z_STRVAL(element_name->u.constant), Z_STRLEN(element_name->u.constant)+1);
		str_efree(Z_STRVAL(element_name->u.constant));
		*element_name = tmp;
	}
}
/* }}} */

void zend_resolve_function_name(znode *element_name, zend_bool *check_namespace TSRMLS_DC) /* {{{ */
{
	zend_resolve_non_class_name(element_name, check_namespace, 0, CG(current_import_function) TSRMLS_CC);
}
/* }}} */

void zend_resolve_const_name(znode *element_name, zend_bool *check_namespace TSRMLS_DC) /* {{{ */
{
	zend_resolve_non_class_name(element_name, check_namespace, 1, CG(current_import_const) TSRMLS_CC);
}
/* }}} */

void zend_do_resolve_class_name(znode *result, znode *class_name, int is_static TSRMLS_DC) /* {{{ */
{
	char *lcname;
	int lctype;
	znode constant_name;

	lcname = zend_str_tolower_dup(Z_STRVAL(class_name->u.constant), Z_STRLEN(class_name->u.constant));
	lctype = zend_get_class_fetch_type(lcname, strlen(lcname));
	switch (lctype) {
		case ZEND_FETCH_CLASS_SELF:
			if (!CG(active_class_entry)) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot access self::class when no class scope is active");
			}
			if ((CG(active_class_entry)->ce_flags & ZEND_ACC_TRAIT) == ZEND_ACC_TRAIT) {
				zval_dtor(&class_name->u.constant);
				constant_name.op_type = IS_CONST;
				ZVAL_STRINGL(&constant_name.u.constant, "__CLASS__", sizeof("__CLASS__")-1, 1);
				if (is_static) {
					*result = constant_name;
					result->u.constant.type = IS_CONSTANT;
				} else {
					zend_do_fetch_constant(result, NULL, &constant_name, ZEND_RT, 1 TSRMLS_CC);
				}
				break;
			}
			zval_dtor(&class_name->u.constant);
			class_name->op_type = IS_CONST;
			ZVAL_STRINGL(&class_name->u.constant, CG(active_class_entry)->name, CG(active_class_entry)->name_length, 1);
			*result = *class_name;
			break;
        case ZEND_FETCH_CLASS_STATIC:
        case ZEND_FETCH_CLASS_PARENT:
			if (is_static) {
				zend_error_noreturn(E_COMPILE_ERROR,
					"%s::class cannot be used for compile-time class name resolution",
					lctype == ZEND_FETCH_CLASS_STATIC ? "static" : "parent"
					);
			}
			if (!CG(active_class_entry)) {
				zend_error_noreturn(E_COMPILE_ERROR,
					"Cannot access %s::class when no class scope is active",
					lctype == ZEND_FETCH_CLASS_STATIC ? "static" : "parent"
					);
			}
			constant_name.op_type = IS_CONST;
			ZVAL_STRINGL(&constant_name.u.constant, "class", sizeof("class")-1, 1);
			zend_do_fetch_constant(result, class_name, &constant_name, ZEND_RT, 1 TSRMLS_CC);
			break;
		case ZEND_FETCH_CLASS_DEFAULT:
			zend_resolve_class_name(class_name TSRMLS_CC);
			*result = *class_name;
			break;
	}

	efree(lcname);

}
/* }}} */

void zend_resolve_class_name(znode *class_name TSRMLS_DC) /* {{{ */
{
	char *compound;
	char *lcname;
	zval **ns;
	znode tmp;
	int len;

	compound = memchr(Z_STRVAL(class_name->u.constant), '\\', Z_STRLEN(class_name->u.constant));
	if (compound) {
		/* This is a compound class name that contains namespace prefix */
		if (Z_STRVAL(class_name->u.constant)[0] == '\\') {
			/* The STRING name has "\" prefix */
			Z_STRLEN(class_name->u.constant) -= 1;
			memmove(Z_STRVAL(class_name->u.constant), Z_STRVAL(class_name->u.constant)+1, Z_STRLEN(class_name->u.constant)+1);
			Z_STRVAL(class_name->u.constant) = erealloc(
			Z_STRVAL(class_name->u.constant),
			Z_STRLEN(class_name->u.constant) + 1);

			if (ZEND_FETCH_CLASS_DEFAULT != zend_get_class_fetch_type(Z_STRVAL(class_name->u.constant), Z_STRLEN(class_name->u.constant))) {
				zend_error_noreturn(E_COMPILE_ERROR, "'\\%s' is an invalid class name", Z_STRVAL(class_name->u.constant));
			}
		} else {
			if (CG(current_import)) {
				len = compound - Z_STRVAL(class_name->u.constant);
				lcname = zend_str_tolower_dup(Z_STRVAL(class_name->u.constant), len);
				/* Check if first part of compound name is an import name */
				if (zend_hash_find(CG(current_import), lcname, len+1, (void**)&ns) == SUCCESS) {
					/* Substitute import name */
					tmp.op_type = IS_CONST;
					tmp.u.constant = **ns;
					zval_copy_ctor(&tmp.u.constant);
					len += 1;
					Z_STRLEN(class_name->u.constant) -= len;
					memmove(Z_STRVAL(class_name->u.constant), Z_STRVAL(class_name->u.constant)+len, Z_STRLEN(class_name->u.constant)+1);
					zend_do_build_namespace_name(&tmp, &tmp, class_name TSRMLS_CC);
					*class_name = tmp;
					efree(lcname);
					return;
				}
				efree(lcname);
			}
			/* Here name is not prefixed with \ and not imported */
			if (CG(current_namespace)) {
				tmp.op_type = IS_CONST;
				tmp.u.constant = *CG(current_namespace);
				zval_copy_ctor(&tmp.u.constant);
				zend_do_build_namespace_name(&tmp, &tmp, class_name TSRMLS_CC);
				*class_name = tmp;
			}
		}
	} else if (CG(current_import) || CG(current_namespace)) {
		/* this is a plain name (without \) */
		lcname = zend_str_tolower_dup(Z_STRVAL(class_name->u.constant), Z_STRLEN(class_name->u.constant));

		if (CG(current_import) &&
		    zend_hash_find(CG(current_import), lcname, Z_STRLEN(class_name->u.constant)+1, (void**)&ns) == SUCCESS) {
		    /* The given name is an import name. Substitute it. */
			zval_dtor(&class_name->u.constant);
			class_name->u.constant = **ns;
			zval_copy_ctor(&class_name->u.constant);
		} else if (CG(current_namespace)) {
			/* plain name, no import - prepend current namespace to it */
			tmp.op_type = IS_CONST;
			tmp.u.constant = *CG(current_namespace);
			zval_copy_ctor(&tmp.u.constant);
			zend_do_build_namespace_name(&tmp, &tmp, class_name TSRMLS_CC);
			*class_name = tmp;
		}
		efree(lcname);
	}
}
/* }}} */

void zend_do_fetch_class(znode *result, znode *class_name TSRMLS_DC) /* {{{ */
{
	long fetch_class_op_number;
	zend_op *opline;

	fetch_class_op_number = get_next_op_number(CG(active_op_array));
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_FETCH_CLASS;
	SET_UNUSED(opline->op1);
	opline->extended_value = ZEND_FETCH_CLASS_DEFAULT;
	CG(catch_begin) = fetch_class_op_number;
	if (class_name->op_type == IS_CONST) {
		int fetch_type;

		fetch_type = zend_get_class_fetch_type(Z_STRVAL(class_name->u.constant), Z_STRLEN(class_name->u.constant));
		switch (fetch_type) {
			case ZEND_FETCH_CLASS_SELF:
			case ZEND_FETCH_CLASS_PARENT:
			case ZEND_FETCH_CLASS_STATIC:
				SET_UNUSED(opline->op2);
				opline->extended_value = fetch_type;
				zval_dtor(&class_name->u.constant);
				break;
			default:
				zend_resolve_class_name(class_name TSRMLS_CC);
				opline->op2_type = IS_CONST;
				opline->op2.constant =
					zend_add_class_name_literal(CG(active_op_array), &class_name->u.constant TSRMLS_CC);
				break;
		}
	} else {
		SET_NODE(opline->op2, class_name);
	}
	opline->result.var = get_temporary_variable(CG(active_op_array));
	opline->result_type = IS_VAR; /* FIXME: Hack so that INIT_FCALL_BY_NAME still knows this is a class */
	GET_NODE(result, opline->result);
	result->EA = opline->extended_value;
}
/* }}} */

void zend_do_label(znode *label TSRMLS_DC) /* {{{ */
{
	zend_label dest;

	if (!CG(context).labels) {
		ALLOC_HASHTABLE(CG(context).labels);
		zend_hash_init(CG(context).labels, 4, NULL, NULL, 0);
	}

	dest.brk_cont = CG(context).current_brk_cont;
	dest.opline_num = get_next_op_number(CG(active_op_array));

	if (zend_hash_add(CG(context).labels, Z_STRVAL(label->u.constant), Z_STRLEN(label->u.constant) + 1, (void**)&dest, sizeof(zend_label), NULL) == FAILURE) {
		zend_error_noreturn(E_COMPILE_ERROR, "Label '%s' already defined", Z_STRVAL(label->u.constant));
	}

	/* Done with label now */
	zval_dtor(&label->u.constant);
}
/* }}} */

void zend_resolve_goto_label(zend_op_array *op_array, zend_op *opline, int pass2 TSRMLS_DC) /* {{{ */
{
	zend_label *dest;
	long current, distance;
	zval *label;

	if (pass2) {
		label = opline->op2.zv;
	} else {
		label = &CONSTANT_EX(op_array, opline->op2.constant);
	}
	if (CG(context).labels == NULL ||
	    zend_hash_find(CG(context).labels, Z_STRVAL_P(label), Z_STRLEN_P(label)+1, (void**)&dest) == FAILURE) {

		if (pass2) {
			CG(in_compilation) = 1;
			CG(active_op_array) = op_array;
			CG(zend_lineno) = opline->lineno;
			zend_error_noreturn(E_COMPILE_ERROR, "'goto' to undefined label '%s'", Z_STRVAL_P(label));
		} else {
			/* Label is not defined. Delay to pass 2. */
			INC_BPC(op_array);
			return;
		}
	}

	opline->op1.opline_num = dest->opline_num;
	zval_dtor(label);
	Z_TYPE_P(label) = IS_NULL;

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

	if (pass2) {
		DEC_BPC(op_array);
	}
}
/* }}} */

void zend_do_goto(const znode *label TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_GOTO;
	opline->extended_value = CG(context).current_brk_cont;
	SET_UNUSED(opline->op1);
	SET_NODE(opline->op2, label);
	zend_resolve_goto_label(CG(active_op_array), opline, 0 TSRMLS_CC);
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
		zend_compiler_context *ctx;

		zend_stack_top(&CG(context_stack), (void**)&ctx);
		CG(context) = *ctx;
		zend_stack_del_top(&CG(context_stack));
	}
}
/* }}} */

void zend_do_build_full_name(znode *result, znode *prefix, znode *name, int is_class_member TSRMLS_DC) /* {{{ */
{
	zend_uint length;

	if (!result) {
		result = prefix;
	} else {
		*result = *prefix;
	}

	if (is_class_member) {
		length = sizeof("::")-1 + Z_STRLEN(result->u.constant) + Z_STRLEN(name->u.constant);
		Z_STRVAL(result->u.constant) = str_erealloc(Z_STRVAL(result->u.constant), length+1);
		memcpy(&Z_STRVAL(result->u.constant)[Z_STRLEN(result->u.constant)], "::", sizeof("::")-1);
		memcpy(&Z_STRVAL(result->u.constant)[Z_STRLEN(result->u.constant) + sizeof("::")-1], Z_STRVAL(name->u.constant), Z_STRLEN(name->u.constant)+1);
		str_efree(Z_STRVAL(name->u.constant));
		Z_STRLEN(result->u.constant) = length;
	} else {
		length = sizeof("\\")-1 + Z_STRLEN(result->u.constant) + Z_STRLEN(name->u.constant);
		Z_STRVAL(result->u.constant) = str_erealloc(Z_STRVAL(result->u.constant), length+1);
		memcpy(&Z_STRVAL(result->u.constant)[Z_STRLEN(result->u.constant)], "\\", sizeof("\\")-1);
		memcpy(&Z_STRVAL(result->u.constant)[Z_STRLEN(result->u.constant) + sizeof("\\")-1], Z_STRVAL(name->u.constant), Z_STRLEN(name->u.constant)+1);
		str_efree(Z_STRVAL(name->u.constant));
		Z_STRLEN(result->u.constant) = length;
	}
}
/* }}} */

int zend_do_begin_class_member_function_call(znode *class_name, znode *method_name TSRMLS_DC) /* {{{ */
{
	znode class_node;
	zend_op *opline;

	if (method_name->op_type == IS_CONST) {
		char *lcname;
		if (Z_TYPE(method_name->u.constant) != IS_STRING) {
			zend_error_noreturn(E_COMPILE_ERROR, "Method name must be a string");
		}
		lcname = zend_str_tolower_dup(Z_STRVAL(method_name->u.constant), Z_STRLEN(method_name->u.constant));
		if ((sizeof(ZEND_CONSTRUCTOR_FUNC_NAME)-1) == Z_STRLEN(method_name->u.constant) &&
		    memcmp(lcname, ZEND_CONSTRUCTOR_FUNC_NAME, sizeof(ZEND_CONSTRUCTOR_FUNC_NAME)-1) == 0) {
			zval_dtor(&method_name->u.constant);
			method_name->op_type = IS_UNUSED;
		}
		efree(lcname);
	}

	if (class_name->op_type == IS_CONST &&
	    ZEND_FETCH_CLASS_DEFAULT == zend_get_class_fetch_type(Z_STRVAL(class_name->u.constant), Z_STRLEN(class_name->u.constant))) {
		zend_resolve_class_name(class_name TSRMLS_CC);
		class_node = *class_name;
		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	} else {
		zend_do_fetch_class(&class_node, class_name TSRMLS_CC);
		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->extended_value = class_node.EA	;
	}
	opline->opcode = ZEND_INIT_STATIC_METHOD_CALL;
	opline->result.num = CG(context).nested_calls;
	if (class_node.op_type == IS_CONST) {
		opline->op1_type = IS_CONST;
		opline->op1.constant =
			zend_add_class_name_literal(CG(active_op_array), &class_node.u.constant TSRMLS_CC);
	} else {
		SET_NODE(opline->op1, &class_node);
	}
	if (method_name->op_type == IS_CONST) {
		opline->op2_type = IS_CONST;
		opline->op2.constant =
			zend_add_func_name_literal(CG(active_op_array), &method_name->u.constant TSRMLS_CC);
		if (opline->op1_type == IS_CONST) {
			GET_CACHE_SLOT(opline->op2.constant);
		} else {
			GET_POLYMORPHIC_CACHE_SLOT(opline->op2.constant);
		}
	} else {
		SET_NODE(opline->op2, method_name);
	}

	zend_push_function_call_entry(NULL TSRMLS_CC);
	if (++CG(context).nested_calls > CG(active_op_array)->nested_calls) {
		CG(active_op_array)->nested_calls = CG(context).nested_calls;
	}
	zend_do_extended_fcall_begin(TSRMLS_C);
	return 1; /* Dynamic */
}
/* }}} */

void zend_do_end_function_call(znode *function_name, znode *result, int is_method, int is_dynamic_fcall TSRMLS_DC) /* {{{ */
{
	zend_op *opline;
	zend_function_call_entry *fcall;
	zend_stack_top(&CG(function_call_stack), (void **) &fcall);

	if (is_method && function_name && function_name->op_type == IS_UNUSED) {
		/* clone */
		if (fcall->arg_num != 0) {
			zend_error(E_WARNING, "Clone method does not require arguments");
		}
		opline = &CG(active_op_array)->opcodes[Z_LVAL(function_name->u.constant)];
	} else {
		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		if (fcall->fbc) {
			opline->opcode = ZEND_DO_FCALL;
			SET_NODE(opline->op1, function_name);
			SET_UNUSED(opline->op2);
			opline->op2.num = CG(context).nested_calls;
			CALCULATE_LITERAL_HASH(opline->op1.constant);
			GET_CACHE_SLOT(opline->op1.constant);
		} else {
			opline->opcode = ZEND_DO_FCALL_BY_NAME;
			SET_UNUSED(opline->op1);
			SET_UNUSED(opline->op2);
			opline->op2.num = --CG(context).nested_calls;

			/* This would normally be a ZEND_DO_FCALL, but was forced to use
			 * ZEND_DO_FCALL_BY_NAME due to a ... argument. In this case we need to
			 * free the function_name */
			if (!is_method && !is_dynamic_fcall && function_name->op_type==IS_CONST) {
				zval_dtor(&function_name->u.constant);
			}
		}
	}

	opline->result.var = get_temporary_variable(CG(active_op_array));
	opline->result_type = IS_VAR;
	GET_NODE(result, opline->result);
	opline->extended_value = fcall->arg_num;

	if (CG(context).used_stack + 1 > CG(active_op_array)->used_stack) {
		CG(active_op_array)->used_stack = CG(context).used_stack + 1;
	}
	CG(context).used_stack -= fcall->arg_num;
	zend_stack_del_top(&CG(function_call_stack));
}
/* }}} */

void zend_do_pass_param(znode *param, zend_uchar op TSRMLS_DC) /* {{{ */
{
	zend_op *opline;
	int original_op = op;
	zend_function_call_entry *fcall;
	zend_function *function_ptr;
	int send_by_reference = 0;
	int send_function = 0;

	zend_stack_top(&CG(function_call_stack), (void **) &fcall);
	function_ptr = fcall->fbc;
	fcall->arg_num++;

	if (fcall->uses_argument_unpacking) {
		zend_error_noreturn(E_COMPILE_ERROR,
			"Cannot use positional argument after argument unpacking");
	}

	if (original_op == ZEND_SEND_REF) {
		if (function_ptr &&
		    function_ptr->common.function_name &&
		    function_ptr->common.type == ZEND_USER_FUNCTION &&
		    !ARG_SHOULD_BE_SENT_BY_REF(function_ptr, fcall->arg_num)) {
			zend_error_noreturn(E_COMPILE_ERROR,
						"Call-time pass-by-reference has been removed; "
						"If you would like to pass argument by reference, modify the declaration of %s().",
						function_ptr->common.function_name);
		} else {
			zend_error_noreturn(E_COMPILE_ERROR, "Call-time pass-by-reference has been removed");
		}
		return;
	}

	if (function_ptr) {
		if (ARG_MAY_BE_SENT_BY_REF(function_ptr, fcall->arg_num)) {
			if (op == ZEND_SEND_VAR && param->op_type & (IS_VAR|IS_CV)) {
				send_by_reference = ZEND_ARG_SEND_BY_REF;
				if (zend_is_function_or_method_call(param)) {
					/* Method call */
					op = ZEND_SEND_VAR_NO_REF;
					send_function = ZEND_ARG_SEND_FUNCTION | ZEND_ARG_SEND_SILENT;
				}
			} else {
				op = ZEND_SEND_VAL;
			}
		} else if (ARG_SHOULD_BE_SENT_BY_REF(function_ptr, fcall->arg_num)) {
			send_by_reference = ZEND_ARG_SEND_BY_REF;
		}
	}

	if (op == ZEND_SEND_VAR && zend_is_function_or_method_call(param)) {
		/* Method call */
		op = ZEND_SEND_VAR_NO_REF;
		send_function = ZEND_ARG_SEND_FUNCTION;
	} else if (op == ZEND_SEND_VAL && (param->op_type & (IS_VAR|IS_CV))) {
		op = ZEND_SEND_VAR_NO_REF;
	}

	if (op!=ZEND_SEND_VAR_NO_REF && send_by_reference==ZEND_ARG_SEND_BY_REF) {
		/* change to passing by reference */
		switch (param->op_type) {
			case IS_VAR:
			case IS_CV:
				op = ZEND_SEND_REF;
				break;
			default:
				zend_error_noreturn(E_COMPILE_ERROR, "Only variables can be passed by reference");
				break;
		}
	}

	if (original_op == ZEND_SEND_VAR) {
		switch (op) {
			case ZEND_SEND_VAR_NO_REF:
				zend_do_end_variable_parse(param, BP_VAR_R, 0 TSRMLS_CC);
				break;
			case ZEND_SEND_VAR:
				if (function_ptr) {
					zend_do_end_variable_parse(param, BP_VAR_R, 0 TSRMLS_CC);
				} else {
					zend_do_end_variable_parse(param, BP_VAR_FUNC_ARG, fcall->arg_num TSRMLS_CC);
				}
				break;
			case ZEND_SEND_REF:
				zend_do_end_variable_parse(param, BP_VAR_W, 0 TSRMLS_CC);
				break;
		}
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	if (op == ZEND_SEND_VAR_NO_REF) {
		if (function_ptr) {
			opline->extended_value = ZEND_ARG_COMPILE_TIME_BOUND | send_by_reference | send_function;
		} else {
			opline->extended_value = send_function;
		}
	} else {
		if (function_ptr) {
			opline->extended_value = ZEND_DO_FCALL;
		} else {
			opline->extended_value = ZEND_DO_FCALL_BY_NAME;
		}
	}
	opline->opcode = op;
	SET_NODE(opline->op1, param);
	opline->op2.opline_num = fcall->arg_num;
	SET_UNUSED(opline->op2);

	if (++CG(context).used_stack > CG(active_op_array)->used_stack) {
		CG(active_op_array)->used_stack = CG(context).used_stack;
	}
}
/* }}} */

void zend_do_unpack_params(znode *params TSRMLS_DC) /* {{{ */
{
	zend_op *opline;
	zend_function_call_entry *fcall;

	zend_stack_top(&CG(function_call_stack), (void **) &fcall);
	fcall->uses_argument_unpacking = 1;

	if (fcall->fbc) {
		/* If argument unpacking is used argument numbers and sending modes can no longer be
		 * computed at compile time, thus we need access to EX(call). In order to have it we
		 * retroactively emit a ZEND_INIT_FCALL_BY_NAME opcode. */
		zval func_name;
		ZVAL_STRING(&func_name, fcall->fbc->common.function_name, 1);

		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = ZEND_INIT_FCALL_BY_NAME;
		opline->result.num = CG(context).nested_calls;
		SET_UNUSED(opline->op1);
		opline->op2_type = IS_CONST;
		opline->op2.constant = zend_add_func_name_literal(CG(active_op_array), &func_name TSRMLS_CC);
		GET_CACHE_SLOT(opline->op2.constant);

		++CG(context).nested_calls;
		fcall->fbc = NULL;
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_SEND_UNPACK;
	SET_NODE(opline->op1, params);
	SET_UNUSED(opline->op2);
	opline->op2.num = fcall->arg_num;
}
/* }}} */

static int generate_free_switch_expr(const zend_switch_entry *switch_entry TSRMLS_DC) /* {{{ */
{
	zend_op *opline;

	if (switch_entry->cond.op_type != IS_VAR && switch_entry->cond.op_type != IS_TMP_VAR) {
		return (switch_entry->cond.op_type == IS_UNUSED);
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = (switch_entry->cond.op_type == IS_TMP_VAR) ? ZEND_FREE : ZEND_SWITCH_FREE;
	SET_NODE(opline->op1, &switch_entry->cond);
	SET_UNUSED(opline->op2);

	return 0;
}
/* }}} */

static int generate_free_foreach_copy(const zend_op *foreach_copy TSRMLS_DC) /* {{{ */
{
	zend_op *opline;

	/* If we reach the separator then stop applying the stack */
	if (foreach_copy->result_type == IS_UNUSED) {
		return 1;
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = (foreach_copy->result_type == IS_TMP_VAR) ? ZEND_FREE : ZEND_SWITCH_FREE;
	COPY_NODE(opline->op1, foreach_copy->result);
	SET_UNUSED(opline->op2);

	return 0;
}
/* }}} */

void zend_do_return(znode *expr, int do_end_vparse TSRMLS_DC) /* {{{ */
{
	zend_op *opline;
	int start_op_number, end_op_number;
	zend_bool returns_reference = (CG(active_op_array)->fn_flags & ZEND_ACC_RETURN_REFERENCE) != 0;

	/* The error for use of return inside a generator is thrown in pass_two. */

	if (do_end_vparse) {
		if (returns_reference && !zend_is_function_or_method_call(expr)) {
			zend_do_end_variable_parse(expr, BP_VAR_W, 0 TSRMLS_CC);
		} else {
			zend_do_end_variable_parse(expr, BP_VAR_R, 0 TSRMLS_CC);
		}
	}

	start_op_number = get_next_op_number(CG(active_op_array));

#ifdef ZTS
	zend_stack_apply_with_argument(&CG(switch_cond_stack), ZEND_STACK_APPLY_TOPDOWN, (int (*)(void *element, void *)) generate_free_switch_expr TSRMLS_CC);
	zend_stack_apply_with_argument(&CG(foreach_copy_stack), ZEND_STACK_APPLY_TOPDOWN, (int (*)(void *element, void *)) generate_free_foreach_copy TSRMLS_CC);
#else
	zend_stack_apply(&CG(switch_cond_stack), ZEND_STACK_APPLY_TOPDOWN, (int (*)(void *element)) generate_free_switch_expr);
	zend_stack_apply(&CG(foreach_copy_stack), ZEND_STACK_APPLY_TOPDOWN, (int (*)(void *element)) generate_free_foreach_copy);
#endif

	end_op_number = get_next_op_number(CG(active_op_array));
	while (start_op_number < end_op_number) {
		CG(active_op_array)->opcodes[start_op_number].extended_value |= EXT_TYPE_FREE_ON_RETURN;
		start_op_number++;
	}

	if (CG(context).in_finally) {
		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = ZEND_DISCARD_EXCEPTION;
		SET_UNUSED(opline->op1);
		SET_UNUSED(opline->op2);
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = returns_reference ? ZEND_RETURN_BY_REF : ZEND_RETURN;

	if (expr) {
		SET_NODE(opline->op1, expr);

		if (!do_end_vparse) {
			opline->extended_value = ZEND_RETURNS_VALUE;
		} else if (zend_is_function_or_method_call(expr)) {
			opline->extended_value = ZEND_RETURNS_FUNCTION;
		}
	} else {
		opline->op1_type = IS_CONST;
		LITERAL_NULL(opline->op1);
	}

	SET_UNUSED(opline->op2);
}
/* }}} */

void zend_do_yield(znode *result, znode *value, const znode *key, zend_bool is_variable TSRMLS_DC) /* {{{ */
{
	zend_op *opline;

	if (!CG(active_op_array)->function_name) {
		zend_error_noreturn(E_COMPILE_ERROR, "The \"yield\" expression can only be used inside a function");
	}

	CG(active_op_array)->fn_flags |= ZEND_ACC_GENERATOR;

	if (is_variable) {
		if ((CG(active_op_array)->fn_flags & ZEND_ACC_RETURN_REFERENCE) && !zend_is_function_or_method_call(value)) {
			zend_do_end_variable_parse(value, BP_VAR_W, 0 TSRMLS_CC);
		} else {
			zend_do_end_variable_parse(value, BP_VAR_R, 0 TSRMLS_CC);
		}
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_YIELD;

	if (value) {
		SET_NODE(opline->op1, value);

		if (is_variable && zend_is_function_or_method_call(value)) {
			opline->extended_value = ZEND_RETURNS_FUNCTION;
		}
	} else {
		SET_UNUSED(opline->op1);
	}

	if (key) {
		SET_NODE(opline->op2, key);
	} else {
		SET_UNUSED(opline->op2);
	}

	opline->result_type = IS_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	GET_NODE(result, opline->result);
}
/* }}} */

static int zend_add_try_element(zend_uint try_op TSRMLS_DC) /* {{{ */
{
	int try_catch_offset = CG(active_op_array)->last_try_catch++;

	CG(active_op_array)->try_catch_array = erealloc(CG(active_op_array)->try_catch_array, sizeof(zend_try_catch_element)*CG(active_op_array)->last_try_catch);
	CG(active_op_array)->try_catch_array[try_catch_offset].try_op = try_op;
	CG(active_op_array)->try_catch_array[try_catch_offset].catch_op = 0;
	CG(active_op_array)->try_catch_array[try_catch_offset].finally_op = 0;
	CG(active_op_array)->try_catch_array[try_catch_offset].finally_end = 0;
	return try_catch_offset;
}
/* }}} */

static void zend_add_catch_element(int offset, zend_uint catch_op TSRMLS_DC) /* {{{ */
{
	CG(active_op_array)->try_catch_array[offset].catch_op = catch_op;
}
/* }}} */

void zend_do_first_catch(znode *open_parentheses TSRMLS_DC) /* {{{ */
{
	open_parentheses->u.op.opline_num = get_next_op_number(CG(active_op_array));
}
/* }}} */

void zend_initialize_try_catch_element(znode *catch_token TSRMLS_DC) /* {{{ */
{
	int jmp_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	zend_llist jmp_list;
	zend_llist *jmp_list_ptr;

	opline->opcode = ZEND_JMP;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
	/* save for backpatching */

	zend_llist_init(&jmp_list, sizeof(int), NULL, 0);
	zend_stack_push(&CG(bp_stack), (void *) &jmp_list, sizeof(zend_llist));
	zend_stack_top(&CG(bp_stack), (void **) &jmp_list_ptr);
	zend_llist_add_element(jmp_list_ptr, &jmp_op_number);

	catch_token->EA = get_next_op_number(CG(active_op_array));
}
/* }}} */

void zend_do_mark_last_catch(const znode *first_catch, const znode *last_additional_catch TSRMLS_DC) /* {{{ */
{
	CG(active_op_array)->last--;
	zend_do_if_end(TSRMLS_C);
	if (last_additional_catch->u.op.opline_num == -1) {
		CG(active_op_array)->opcodes[first_catch->u.op.opline_num].result.num = 1;
		CG(active_op_array)->opcodes[first_catch->u.op.opline_num].extended_value = get_next_op_number(CG(active_op_array));
	} else {
		CG(active_op_array)->opcodes[last_additional_catch->u.op.opline_num].result.num = 1;
		CG(active_op_array)->opcodes[last_additional_catch->u.op.opline_num].extended_value = get_next_op_number(CG(active_op_array));
	}
	DEC_BPC(CG(active_op_array));
}
/* }}} */

void zend_do_try(znode *try_token TSRMLS_DC) /* {{{ */
{
	try_token->u.op.opline_num = zend_add_try_element(get_next_op_number(CG(active_op_array)) TSRMLS_CC);
	INC_BPC(CG(active_op_array));
}
/* }}} */

void zend_do_finally(znode *finally_token TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	finally_token->u.op.opline_num = get_next_op_number(CG(active_op_array));
	/* call the the "finally" block */
	opline->opcode = ZEND_FAST_CALL;
	SET_UNUSED(opline->op1);
	opline->op1.opline_num = finally_token->u.op.opline_num + 1;
	SET_UNUSED(opline->op2);
	/* jump to code after the "finally" block,
	 * the actual jump address is going to be set in zend_do_end_finally()
	 */
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_JMP;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);

	CG(context).in_finally++;
}
/* }}} */

void zend_do_begin_catch(znode *catch_token, znode *class_name, znode *catch_var, znode *first_catch TSRMLS_DC) /* {{{ */
{
	long catch_op_number;
	zend_op *opline;
	znode catch_class;

	if (class_name->op_type == IS_CONST &&
	    ZEND_FETCH_CLASS_DEFAULT == zend_get_class_fetch_type(Z_STRVAL(class_name->u.constant), Z_STRLEN(class_name->u.constant))) {
		zend_resolve_class_name(class_name TSRMLS_CC);
		catch_class = *class_name;
	} else {
		zend_error_noreturn(E_COMPILE_ERROR, "Bad class name in the catch statement");
	}

	catch_op_number = get_next_op_number(CG(active_op_array));
	if (first_catch) {
		first_catch->u.op.opline_num = catch_op_number;
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_CATCH;
	opline->op1_type = IS_CONST;
	opline->op1.constant = zend_add_class_name_literal(CG(active_op_array), &catch_class.u.constant TSRMLS_CC);
	opline->op2_type = IS_CV;
	opline->op2.var = lookup_cv(CG(active_op_array), Z_STRVAL(catch_var->u.constant), Z_STRLEN(catch_var->u.constant), 0 TSRMLS_CC);
	Z_STRVAL(catch_var->u.constant) = (char*)CG(active_op_array)->vars[opline->op2.var].name;
	opline->result.num = 0; /* 1 means it's the last catch in the block */

	catch_token->u.op.opline_num = catch_op_number;
}
/* }}} */

void zend_do_end_catch(znode *catch_token TSRMLS_DC) /* {{{ */
{
	int jmp_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	zend_llist *jmp_list_ptr;

	opline->opcode = ZEND_JMP;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
	/* save for backpatching */

	zend_stack_top(&CG(bp_stack), (void **) &jmp_list_ptr);
	zend_llist_add_element(jmp_list_ptr, &jmp_op_number);

	CG(active_op_array)->opcodes[catch_token->u.op.opline_num].extended_value = get_next_op_number(CG(active_op_array));
}
/* }}} */

void zend_do_bind_catch(znode *try_token, znode *catch_token TSRMLS_DC) /* {{{ */ {
	if (catch_token->op_type != IS_UNUSED) {
		zend_add_catch_element(try_token->u.op.opline_num, catch_token->EA TSRMLS_CC);
	}
}
/* }}} */

void zend_do_end_finally(znode *try_token, znode* catch_token, znode *finally_token TSRMLS_DC) /* {{{ */
{
	if (catch_token->op_type == IS_UNUSED && finally_token->op_type == IS_UNUSED) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use try without catch or finally");
	}
	if (finally_token->op_type != IS_UNUSED) {
		zend_op *opline;

		CG(active_op_array)->try_catch_array[try_token->u.op.opline_num].finally_op = finally_token->u.op.opline_num + 1;
		CG(active_op_array)->try_catch_array[try_token->u.op.opline_num].finally_end = get_next_op_number(CG(active_op_array));
		CG(active_op_array)->has_finally_block = 1;

		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = ZEND_FAST_RET;
		SET_UNUSED(opline->op1);
		SET_UNUSED(opline->op2);

		CG(active_op_array)->opcodes[finally_token->u.op.opline_num].op1.opline_num = get_next_op_number(CG(active_op_array));

		CG(context).in_finally--;
	}
}
/* }}} */

void zend_do_throw(const znode *expr TSRMLS_DC) /* {{{ */
{
	zend_op *opline;

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_THROW;
	SET_NODE(opline->op1, expr);
	SET_UNUSED(opline->op2);
}
/* }}} */

ZEND_API void function_add_ref(zend_function *function) /* {{{ */
{
	if (function->type == ZEND_USER_FUNCTION) {
		zend_op_array *op_array = &function->op_array;

		(*op_array->refcount)++;
		if (op_array->static_variables) {
			HashTable *static_variables = op_array->static_variables;
			zval *tmp_zval;

			ALLOC_HASHTABLE(op_array->static_variables);
			zend_hash_init(op_array->static_variables, zend_hash_num_elements(static_variables), NULL, ZVAL_PTR_DTOR, 0);
			zend_hash_copy(op_array->static_variables, static_variables, (copy_ctor_func_t) zval_add_ref, (void *) &tmp_zval, sizeof(zval *));
		}
		op_array->run_time_cache = NULL;
	}
}
/* }}} */

static void do_inherit_parent_constructor(zend_class_entry *ce) /* {{{ */
{
	zend_function *function, *new_function;

	if (!ce->parent) {
		return;
	}

	/* You cannot change create_object */
	ce->create_object = ce->parent->create_object;

	/* Inherit special functions if needed */
	if (!ce->get_iterator) {
		ce->get_iterator = ce->parent->get_iterator;
	}
	if (!ce->iterator_funcs.funcs) {
		ce->iterator_funcs.funcs = ce->parent->iterator_funcs.funcs;
	}
	if (!ce->__get) {
		ce->__get   = ce->parent->__get;
	}
	if (!ce->__set) {
		ce->__set = ce->parent->__set;
	}
	if (!ce->__unset) {
		ce->__unset = ce->parent->__unset;
	}
	if (!ce->__isset) {
		ce->__isset = ce->parent->__isset;
	}
	if (!ce->__call) {
		ce->__call = ce->parent->__call;
	}
	if (!ce->__callstatic) {
		ce->__callstatic = ce->parent->__callstatic;
	}
	if (!ce->__tostring) {
		ce->__tostring = ce->parent->__tostring;
	}
	if (!ce->clone) {
		ce->clone = ce->parent->clone;
	}
	if(!ce->serialize) {
		ce->serialize = ce->parent->serialize;
	}
	if(!ce->unserialize) {
		ce->unserialize = ce->parent->unserialize;
	}
	if (!ce->destructor) {
		ce->destructor   = ce->parent->destructor;
	}
	if (!ce->__debugInfo) {
		ce->__debugInfo = ce->parent->__debugInfo;
	}
	if (ce->constructor) {
		if (ce->parent->constructor && ce->parent->constructor->common.fn_flags & ZEND_ACC_FINAL) {
			zend_error(E_ERROR, "Cannot override final %s::%s() with %s::%s()",
				ce->parent->name, ce->parent->constructor->common.function_name,
				ce->name, ce->constructor->common.function_name
				);
		}
		return;
	}

	if (zend_hash_find(&ce->parent->function_table, ZEND_CONSTRUCTOR_FUNC_NAME, sizeof(ZEND_CONSTRUCTOR_FUNC_NAME), (void **)&function)==SUCCESS) {
		/* inherit parent's constructor */
		zend_hash_update(&ce->function_table, ZEND_CONSTRUCTOR_FUNC_NAME, sizeof(ZEND_CONSTRUCTOR_FUNC_NAME), function, sizeof(zend_function), (void**)&new_function);
		function_add_ref(new_function);
	} else {
		/* Don't inherit the old style constructor if we already have the new style constructor */
		char *lc_class_name;
		char *lc_parent_class_name;

		lc_class_name = zend_str_tolower_dup(ce->name, ce->name_length);
		if (!zend_hash_exists(&ce->function_table, lc_class_name, ce->name_length+1)) {
			lc_parent_class_name = zend_str_tolower_dup(ce->parent->name, ce->parent->name_length);
			if (!zend_hash_exists(&ce->function_table, lc_parent_class_name, ce->parent->name_length+1) &&
					zend_hash_find(&ce->parent->function_table, lc_parent_class_name, ce->parent->name_length+1, (void **)&function)==SUCCESS) {
				if (function->common.fn_flags & ZEND_ACC_CTOR) {
					/* inherit parent's constructor */
					zend_hash_update(&ce->function_table, lc_parent_class_name, ce->parent->name_length+1, function, sizeof(zend_function), (void**)&new_function);
					function_add_ref(new_function);
				}
			}
			efree(lc_parent_class_name);
		}
		efree(lc_class_name);
	}
	ce->constructor = ce->parent->constructor;
}
/* }}} */

char *zend_visibility_string(zend_uint fn_flags) /* {{{ */
{
	if (fn_flags & ZEND_ACC_PRIVATE) {
		return "private";
	}
	if (fn_flags & ZEND_ACC_PROTECTED) {
		return "protected";
	}
	if (fn_flags & ZEND_ACC_PUBLIC) {
		return "public";
	}
	return "";
}
/* }}} */

static void do_inherit_method(zend_function *function) /* {{{ */
{
	/* The class entry of the derived function intentionally remains the same
	 * as that of the parent class.  That allows us to know in which context
	 * we're running, and handle private method calls properly.
	 */
	function_add_ref(function);
}
/* }}} */

static zend_bool zend_do_perform_implementation_check(const zend_function *fe, const zend_function *proto TSRMLS_DC) /* {{{ */
{
	zend_uint i, num_args;

	/* If it's a user function then arg_info == NULL means we don't have any parameters but
	 * we still need to do the arg number checks.  We are only willing to ignore this for internal
	 * functions because extensions don't always define arg_info.
	 */
	if (!proto || (!proto->common.arg_info && proto->common.type != ZEND_USER_FUNCTION)) {
		return 1;
	}

	/* Checks for constructors only if they are declared in an interface,
	 * or explicitly marked as abstract
	 */
	if ((fe->common.fn_flags & ZEND_ACC_CTOR)
		&& ((proto->common.scope->ce_flags & ZEND_ACC_INTERFACE) == 0
			&& (proto->common.fn_flags & ZEND_ACC_ABSTRACT) == 0)) {
		return 1;
	}

	/* If the prototype method is private do not enforce a signature */
	if (proto->common.fn_flags & ZEND_ACC_PRIVATE) {
		return 1;
	}

	/* check number of arguments */
	if (proto->common.required_num_args < fe->common.required_num_args
		|| proto->common.num_args > fe->common.num_args) {
		return 0;
	}

	/* by-ref constraints on return values are covariant */
	if ((proto->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)
		&& !(fe->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)) {
		return 0;
	}

	if ((proto->common.fn_flags & ZEND_ACC_VARIADIC)
		&& !(fe->common.fn_flags & ZEND_ACC_VARIADIC)) {
		return 0;
	}

	/* For variadic functions any additional (optional) arguments that were added must be
	 * checked against the signature of the variadic argument, so in this case we have to
	 * go through all the parameters of the function and not just those present in the
	 * prototype. */
	num_args = proto->common.num_args;
	if ((proto->common.fn_flags & ZEND_ACC_VARIADIC)
		&& fe->common.num_args > proto->common.num_args) {
		num_args = fe->common.num_args;
	}

	for (i = 0; i < num_args; i++) {
		zend_arg_info *fe_arg_info = &fe->common.arg_info[i];

		zend_arg_info *proto_arg_info;
		if (i < proto->common.num_args) {
			proto_arg_info = &proto->common.arg_info[i];
		} else {
			proto_arg_info = &proto->common.arg_info[proto->common.num_args-1];
		}

		if (ZEND_LOG_XOR(fe_arg_info->class_name, proto_arg_info->class_name)) {
			/* Only one has a type hint and the other one doesn't */
			return 0;
		}

		if (fe_arg_info->class_name) {
			const char *fe_class_name, *proto_class_name;
			zend_uint fe_class_name_len, proto_class_name_len;

			if (!strcasecmp(fe_arg_info->class_name, "parent") && proto->common.scope) {
				fe_class_name = proto->common.scope->name;
				fe_class_name_len = proto->common.scope->name_length;
			} else if (!strcasecmp(fe_arg_info->class_name, "self") && fe->common.scope) {
				fe_class_name = fe->common.scope->name;
				fe_class_name_len = fe->common.scope->name_length;
			} else {
				fe_class_name = fe_arg_info->class_name;
				fe_class_name_len = fe_arg_info->class_name_len;
			}

			if (!strcasecmp(proto_arg_info->class_name, "parent") && proto->common.scope && proto->common.scope->parent) {
				proto_class_name = proto->common.scope->parent->name;
				proto_class_name_len = proto->common.scope->parent->name_length;
			} else if (!strcasecmp(proto_arg_info->class_name, "self") && proto->common.scope) {
				proto_class_name = proto->common.scope->name;
				proto_class_name_len = proto->common.scope->name_length;
			} else {
				proto_class_name = proto_arg_info->class_name;
				proto_class_name_len = proto_arg_info->class_name_len;
			}

			if (strcasecmp(fe_class_name, proto_class_name)!=0) {
				const char *colon;

				if (fe->common.type != ZEND_USER_FUNCTION) {
					return 0;
			    } else if (strchr(proto_class_name, '\\') != NULL ||
						(colon = zend_memrchr(fe_class_name, '\\', fe_class_name_len)) == NULL ||
						strcasecmp(colon+1, proto_class_name) != 0) {
					zend_class_entry **fe_ce, **proto_ce;
					int found, found2;

					found = zend_lookup_class(fe_class_name, fe_class_name_len, &fe_ce TSRMLS_CC);
					found2 = zend_lookup_class(proto_class_name, proto_class_name_len, &proto_ce TSRMLS_CC);

					/* Check for class alias */
					if (found != SUCCESS || found2 != SUCCESS ||
							(*fe_ce)->type == ZEND_INTERNAL_CLASS ||
							(*proto_ce)->type == ZEND_INTERNAL_CLASS ||
							*fe_ce != *proto_ce) {
						return 0;
					}
				}
			}
		}
		if (fe_arg_info->type_hint != proto_arg_info->type_hint) {
			/* Incompatible type hint */
			return 0;
		}

		/* by-ref constraints on arguments are invariant */
		if (fe_arg_info->pass_by_reference != proto_arg_info->pass_by_reference) {
			return 0;
		}
	}

	return 1;
}
/* }}} */

#define REALLOC_BUF_IF_EXCEED(buf, offset, length, size) \
	if (UNEXPECTED(offset - buf + size >= length)) { 	\
		length += size + 1; 				\
		buf = erealloc(buf, length); 		\
	}

static char * zend_get_function_declaration(zend_function *fptr TSRMLS_DC) /* {{{ */
{
	char *offset, *buf;
	zend_uint length = 1024;

	offset = buf = (char *)emalloc(length * sizeof(char));
	if (fptr->op_array.fn_flags & ZEND_ACC_RETURN_REFERENCE) {
		*(offset++) = '&';
		*(offset++) = ' ';
	}

	if (fptr->common.scope) {
		memcpy(offset, fptr->common.scope->name, fptr->common.scope->name_length);
		offset += fptr->common.scope->name_length;
		*(offset++) = ':';
		*(offset++) = ':';
	}

	{
		size_t name_len = strlen(fptr->common.function_name);
		REALLOC_BUF_IF_EXCEED(buf, offset, length, name_len);
		memcpy(offset, fptr->common.function_name, name_len);
		offset += name_len;
	}

	*(offset++) = '(';
	if (fptr->common.arg_info) {
		zend_uint i, required;
		zend_arg_info *arg_info = fptr->common.arg_info;

		required = fptr->common.required_num_args;
		for (i = 0; i < fptr->common.num_args;) {
			if (arg_info->class_name) {
				const char *class_name;
				zend_uint class_name_len;
				if (!strcasecmp(arg_info->class_name, "self") && fptr->common.scope ) {
					class_name = fptr->common.scope->name;
					class_name_len = fptr->common.scope->name_length;
				} else if (!strcasecmp(arg_info->class_name, "parent") && fptr->common.scope->parent) {
					class_name = fptr->common.scope->parent->name;
					class_name_len = fptr->common.scope->parent->name_length;
				} else {
					class_name = arg_info->class_name;
					class_name_len = arg_info->class_name_len;
				}
				REALLOC_BUF_IF_EXCEED(buf, offset, length, class_name_len);
				memcpy(offset, class_name, class_name_len);
				offset += class_name_len;
				*(offset++) = ' ';
			} else if (arg_info->type_hint) {
				zend_uint type_name_len;
				char *type_name = zend_get_type_by_const(arg_info->type_hint);
				type_name_len = strlen(type_name);
				REALLOC_BUF_IF_EXCEED(buf, offset, length, type_name_len);
				memcpy(offset, type_name, type_name_len);
				offset += type_name_len;
				*(offset++) = ' ';
			}

			if (arg_info->pass_by_reference) {
				*(offset++) = '&';
			}

			if (arg_info->is_variadic) {
				*(offset++) = '.';
				*(offset++) = '.';
				*(offset++) = '.';
			}

			*(offset++) = '$';

			if (arg_info->name) {
				REALLOC_BUF_IF_EXCEED(buf, offset, length, arg_info->name_len);
				memcpy(offset, arg_info->name, arg_info->name_len);
				offset += arg_info->name_len;
			} else {
				zend_uint idx = i;
				memcpy(offset, "param", 5);
				offset += 5;
				do {
					*(offset++) = (char) (idx % 10) + '0';
					idx /= 10;
				} while (idx > 0);
			}
			if (i >= required && !arg_info->is_variadic) {
				*(offset++) = ' ';
				*(offset++) = '=';
				*(offset++) = ' ';
				if (fptr->type == ZEND_USER_FUNCTION) {
					zend_op *precv = NULL;
					{
						zend_uint idx  = i;
						zend_op *op = ((zend_op_array *)fptr)->opcodes;
						zend_op *end = op + ((zend_op_array *)fptr)->last;

						++idx;
						while (op < end) {
							if ((op->opcode == ZEND_RECV || op->opcode == ZEND_RECV_INIT)
									&& op->op1.num == (long)idx)
							{
								precv = op;
							}
							++op;
						}
					}
					if (precv && precv->opcode == ZEND_RECV_INIT && precv->op2_type != IS_UNUSED) {
						zval *zv, zv_copy;
						int use_copy;
						zv = precv->op2.zv;
						if ((Z_TYPE_P(zv) & IS_CONSTANT_TYPE_MASK) == IS_CONSTANT) {
							REALLOC_BUF_IF_EXCEED(buf, offset, length, Z_STRLEN_P(zv));
							memcpy(offset, Z_STRVAL_P(zv), Z_STRLEN_P(zv));
							offset += Z_STRLEN_P(zv);
						} else if (Z_TYPE_P(zv) == IS_BOOL) {
							if (Z_LVAL_P(zv)) {
								memcpy(offset, "true", 4);
								offset += 4;
							} else {
								memcpy(offset, "false", 5);
								offset += 5;
							}
						} else if (Z_TYPE_P(zv) == IS_NULL) {
							memcpy(offset, "NULL", 4);
							offset += 4;
						} else if (Z_TYPE_P(zv) == IS_STRING) {
							*(offset++) = '\'';
							REALLOC_BUF_IF_EXCEED(buf, offset, length, MIN(Z_STRLEN_P(zv), 10));
							memcpy(offset, Z_STRVAL_P(zv), MIN(Z_STRLEN_P(zv), 10));
							offset += MIN(Z_STRLEN_P(zv), 10);
							if (Z_STRLEN_P(zv) > 10) {
								*(offset++) = '.';
								*(offset++) = '.';
								*(offset++) = '.';
							}
							*(offset++) = '\'';
						} else if (Z_TYPE_P(zv) == IS_ARRAY) {
							memcpy(offset, "Array", 5);
							offset += 5;
						} else if ((Z_TYPE_P(zv) & IS_CONSTANT_TYPE_MASK) == IS_CONSTANT_AST) {
							memcpy(offset, "<expression>", 12);
							offset += 12;
						} else {
							zend_make_printable_zval(zv, &zv_copy, &use_copy);
							REALLOC_BUF_IF_EXCEED(buf, offset, length, Z_STRLEN(zv_copy));
							memcpy(offset, Z_STRVAL(zv_copy), Z_STRLEN(zv_copy));
							offset += Z_STRLEN(zv_copy);
							if (use_copy) {
								zval_dtor(&zv_copy);
							}
						}
					}
				} else {
					memcpy(offset, "NULL", 4);
					offset += 4;
				}
			}

			if (++i < fptr->common.num_args) {
				*(offset++) = ',';
				*(offset++) = ' ';
			}
			arg_info++;
			REALLOC_BUF_IF_EXCEED(buf, offset, length, 32);
		}
	}
	*(offset++) = ')';
	*offset = '\0';

	return buf;
}
/* }}} */

static void do_inheritance_check_on_method(zend_function *child, zend_function *parent TSRMLS_DC) /* {{{ */
{
	zend_uint child_flags;
	zend_uint parent_flags = parent->common.fn_flags;

	if ((parent->common.scope->ce_flags & ZEND_ACC_INTERFACE) == 0
		&& parent->common.fn_flags & ZEND_ACC_ABSTRACT
		&& parent->common.scope != (child->common.prototype ? child->common.prototype->common.scope : child->common.scope)
		&& child->common.fn_flags & (ZEND_ACC_ABSTRACT|ZEND_ACC_IMPLEMENTED_ABSTRACT)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Can't inherit abstract function %s::%s() (previously declared abstract in %s)",
			parent->common.scope->name,
			child->common.function_name,
			child->common.prototype ? child->common.prototype->common.scope->name : child->common.scope->name);
	}

	if (parent_flags & ZEND_ACC_FINAL) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot override final method %s::%s()", ZEND_FN_SCOPE_NAME(parent), child->common.function_name);
	}

	child_flags	= child->common.fn_flags;
	/* You cannot change from static to non static and vice versa.
	 */
	if ((child_flags & ZEND_ACC_STATIC) != (parent_flags & ZEND_ACC_STATIC)) {
		if (child->common.fn_flags & ZEND_ACC_STATIC) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot make non static method %s::%s() static in class %s", ZEND_FN_SCOPE_NAME(parent), child->common.function_name, ZEND_FN_SCOPE_NAME(child));
		} else {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot make static method %s::%s() non static in class %s", ZEND_FN_SCOPE_NAME(parent), child->common.function_name, ZEND_FN_SCOPE_NAME(child));
		}
	}

	/* Disallow making an inherited method abstract. */
	if ((child_flags & ZEND_ACC_ABSTRACT) && !(parent_flags & ZEND_ACC_ABSTRACT)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot make non abstract method %s::%s() abstract in class %s", ZEND_FN_SCOPE_NAME(parent), child->common.function_name, ZEND_FN_SCOPE_NAME(child));
	}

	if (parent_flags & ZEND_ACC_CHANGED) {
		child->common.fn_flags |= ZEND_ACC_CHANGED;
	} else {
		/* Prevent derived classes from restricting access that was available in parent classes
		 */
		if ((child_flags & ZEND_ACC_PPP_MASK) > (parent_flags & ZEND_ACC_PPP_MASK)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Access level to %s::%s() must be %s (as in class %s)%s", ZEND_FN_SCOPE_NAME(child), child->common.function_name, zend_visibility_string(parent_flags), ZEND_FN_SCOPE_NAME(parent), (parent_flags&ZEND_ACC_PUBLIC) ? "" : " or weaker");
		} else if (((child_flags & ZEND_ACC_PPP_MASK) < (parent_flags & ZEND_ACC_PPP_MASK))
			&& ((parent_flags & ZEND_ACC_PPP_MASK) & ZEND_ACC_PRIVATE)) {
			child->common.fn_flags |= ZEND_ACC_CHANGED;
		}
	}

	if (parent_flags & ZEND_ACC_PRIVATE) {
		child->common.prototype = NULL;
	} else if (parent_flags & ZEND_ACC_ABSTRACT) {
		child->common.fn_flags |= ZEND_ACC_IMPLEMENTED_ABSTRACT;
		child->common.prototype = parent;
	} else if (!(parent->common.fn_flags & ZEND_ACC_CTOR) || (parent->common.prototype && (parent->common.prototype->common.scope->ce_flags & ZEND_ACC_INTERFACE))) {
		/* ctors only have a prototype if it comes from an interface */
		child->common.prototype = parent->common.prototype ? parent->common.prototype : parent;
	}

	if (child->common.prototype && (child->common.prototype->common.fn_flags & ZEND_ACC_ABSTRACT)) {
		if (!zend_do_perform_implementation_check(child, child->common.prototype TSRMLS_CC)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Declaration of %s::%s() must be compatible with %s", ZEND_FN_SCOPE_NAME(child), child->common.function_name, zend_get_function_declaration(child->common.prototype TSRMLS_CC));
		}
	} else if (EG(error_reporting) & E_STRICT || EG(user_error_handler)) { /* Check E_STRICT (or custom error handler) before the check so that we save some time */
		if (!zend_do_perform_implementation_check(child, parent TSRMLS_CC)) {
			char *method_prototype = zend_get_function_declaration(parent TSRMLS_CC);
			zend_error(E_STRICT, "Declaration of %s::%s() should be compatible with %s", ZEND_FN_SCOPE_NAME(child), child->common.function_name, method_prototype);
			efree(method_prototype);
		}
	}
}
/* }}} */

static zend_bool do_inherit_method_check(HashTable *child_function_table, zend_function *parent, const zend_hash_key *hash_key, zend_class_entry *child_ce) /* {{{ */
{
	zend_uint parent_flags = parent->common.fn_flags;
	zend_function *child;
	TSRMLS_FETCH();

	if (zend_hash_quick_find(child_function_table, hash_key->arKey, hash_key->nKeyLength, hash_key->h, (void **) &child)==FAILURE) {
		if (parent_flags & (ZEND_ACC_ABSTRACT)) {
			child_ce->ce_flags |= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
		}
		return 1; /* method doesn't exist in child, copy from parent */
	}

	do_inheritance_check_on_method(child, parent TSRMLS_CC);

	return 0;
}
/* }}} */

static zend_bool do_inherit_property_access_check(HashTable *target_ht, zend_property_info *parent_info, const zend_hash_key *hash_key, zend_class_entry *ce) /* {{{ */
{
	zend_property_info *child_info;
	zend_class_entry *parent_ce = ce->parent;
	TSRMLS_FETCH();

	if (parent_info->flags & (ZEND_ACC_PRIVATE|ZEND_ACC_SHADOW)) {
		if (zend_hash_quick_find(&ce->properties_info, hash_key->arKey, hash_key->nKeyLength, hash_key->h, (void **) &child_info)==SUCCESS) {
			child_info->flags |= ZEND_ACC_CHANGED;
		} else {
			zend_hash_quick_update(&ce->properties_info, hash_key->arKey, hash_key->nKeyLength, hash_key->h, parent_info, sizeof(zend_property_info), (void **) &child_info);
			if(ce->type & ZEND_INTERNAL_CLASS) {
				zend_duplicate_property_info_internal(child_info);
			} else {
				zend_duplicate_property_info(child_info);
			}
			child_info->flags &= ~ZEND_ACC_PRIVATE; /* it's not private anymore */
			child_info->flags |= ZEND_ACC_SHADOW; /* but it's a shadow of private */
		}
		return 0; /* don't copy access information to child */
	}

	if (zend_hash_quick_find(&ce->properties_info, hash_key->arKey, hash_key->nKeyLength, hash_key->h, (void **) &child_info)==SUCCESS) {
		if ((parent_info->flags & ZEND_ACC_STATIC) != (child_info->flags & ZEND_ACC_STATIC)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare %s%s::$%s as %s%s::$%s",
				(parent_info->flags & ZEND_ACC_STATIC) ? "static " : "non static ", parent_ce->name, hash_key->arKey,
				(child_info->flags & ZEND_ACC_STATIC) ? "static " : "non static ", ce->name, hash_key->arKey);

		}

		if(parent_info->flags & ZEND_ACC_CHANGED) {
			child_info->flags |= ZEND_ACC_CHANGED;
		}

		if ((child_info->flags & ZEND_ACC_PPP_MASK) > (parent_info->flags & ZEND_ACC_PPP_MASK)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Access level to %s::$%s must be %s (as in class %s)%s", ce->name, hash_key->arKey, zend_visibility_string(parent_info->flags), parent_ce->name, (parent_info->flags&ZEND_ACC_PUBLIC) ? "" : " or weaker");
		} else if ((child_info->flags & ZEND_ACC_STATIC) == 0) {
			/* Don't keep default properties in GC (thry may be freed by opcache) */
			i_zval_ptr_dtor_nogc(ce->default_properties_table[parent_info->offset] ZEND_FILE_LINE_CC TSRMLS_CC);
			ce->default_properties_table[parent_info->offset] = ce->default_properties_table[child_info->offset];
			ce->default_properties_table[child_info->offset] = NULL;
			child_info->offset = parent_info->offset;
		}
		return 0;	/* Don't copy from parent */
	} else {
		return 1;	/* Copy from parent */
	}
}
/* }}} */

static inline void do_implement_interface(zend_class_entry *ce, zend_class_entry *iface TSRMLS_DC) /* {{{ */
{
	if (!(ce->ce_flags & ZEND_ACC_INTERFACE) && iface->interface_gets_implemented && iface->interface_gets_implemented(iface, ce TSRMLS_CC) == FAILURE) {
		zend_error(E_CORE_ERROR, "Class %s could not implement interface %s", ce->name, iface->name);
	}
	if (ce == iface) {
		zend_error(E_ERROR, "Interface %s cannot implement itself", ce->name);
	}
}
/* }}} */

ZEND_API void zend_do_inherit_interfaces(zend_class_entry *ce, const zend_class_entry *iface TSRMLS_DC) /* {{{ */
{
	/* expects interface to be contained in ce's interface list already */
	zend_uint i, ce_num, if_num = iface->num_interfaces;
	zend_class_entry *entry;

	if (if_num==0) {
		return;
	}
	ce_num = ce->num_interfaces;

	if (ce->type == ZEND_INTERNAL_CLASS) {
		ce->interfaces = (zend_class_entry **) realloc(ce->interfaces, sizeof(zend_class_entry *) * (ce_num + if_num));
	} else {
		ce->interfaces = (zend_class_entry **) erealloc(ce->interfaces, sizeof(zend_class_entry *) * (ce_num + if_num));
	}

	/* Inherit the interfaces, only if they're not already inherited by the class */
	while (if_num--) {
		entry = iface->interfaces[if_num];
		for (i = 0; i < ce_num; i++) {
			if (ce->interfaces[i] == entry) {
				break;
			}
		}
		if (i == ce_num) {
			ce->interfaces[ce->num_interfaces++] = entry;
		}
	}

	/* and now call the implementing handlers */
	while (ce_num < ce->num_interfaces) {
		do_implement_interface(ce, ce->interfaces[ce_num++] TSRMLS_CC);
	}
}
/* }}} */

#ifdef ZTS
static void zval_internal_ctor(zval **p) /* {{{ */
{
	zval *orig_ptr = *p;

	ALLOC_ZVAL(*p);
	MAKE_COPY_ZVAL(&orig_ptr, *p);
}
/* }}} */

# define zval_property_ctor(parent_ce, ce) \
	((void (*)(void *)) (((parent_ce)->type != (ce)->type) ? zval_internal_ctor : zval_add_ref))
#else
# define zval_property_ctor(parent_ce, ce) \
	((void (*)(void *)) zval_add_ref)
#endif

ZEND_API void zend_do_inheritance(zend_class_entry *ce, zend_class_entry *parent_ce TSRMLS_DC) /* {{{ */
{
	zend_property_info *property_info;

	if ((ce->ce_flags & ZEND_ACC_INTERFACE)
		&& !(parent_ce->ce_flags & ZEND_ACC_INTERFACE)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Interface %s may not inherit from class (%s)", ce->name, parent_ce->name);
	}
	if (parent_ce->ce_flags & ZEND_ACC_FINAL_CLASS) {
		zend_error_noreturn(E_COMPILE_ERROR, "Class %s may not inherit from final class (%s)", ce->name, parent_ce->name);
	}

	ce->parent = parent_ce;
	/* Copy serialize/unserialize callbacks */
	if (!ce->serialize) {
		ce->serialize   = parent_ce->serialize;
	}
	if (!ce->unserialize) {
		ce->unserialize = parent_ce->unserialize;
	}

	/* Inherit interfaces */
	zend_do_inherit_interfaces(ce, parent_ce TSRMLS_CC);

	/* Inherit properties */
	if (parent_ce->default_properties_count) {
		int i = ce->default_properties_count + parent_ce->default_properties_count;

		ce->default_properties_table = perealloc(ce->default_properties_table, sizeof(void*) * i, ce->type == ZEND_INTERNAL_CLASS);
		if (ce->default_properties_count) {
			while (i-- > parent_ce->default_properties_count) {
				ce->default_properties_table[i] = ce->default_properties_table[i - parent_ce->default_properties_count];
			}
		}
		for (i = 0; i < parent_ce->default_properties_count; i++) {
			ce->default_properties_table[i] = parent_ce->default_properties_table[i];
			if (ce->default_properties_table[i]) {
#ifdef ZTS
				if (parent_ce->type != ce->type) {
					zval *p;

					ALLOC_ZVAL(p);
					MAKE_COPY_ZVAL(&ce->default_properties_table[i], p);
					ce->default_properties_table[i] = p;
				} else {
					Z_ADDREF_P(ce->default_properties_table[i]);
				}
#else
				Z_ADDREF_P(ce->default_properties_table[i]);
#endif
			}
		}
		ce->default_properties_count += parent_ce->default_properties_count;
	}

	if (parent_ce->type != ce->type) {
		/* User class extends internal class */
		zend_update_class_constants(parent_ce  TSRMLS_CC);
		if (parent_ce->default_static_members_count) {
			int i = ce->default_static_members_count + parent_ce->default_static_members_count;

			ce->default_static_members_table = erealloc(ce->default_static_members_table, sizeof(void*) * i);
			if (ce->default_static_members_count) {
				while (i-- > parent_ce->default_static_members_count) {
					ce->default_static_members_table[i] = ce->default_static_members_table[i - parent_ce->default_static_members_count];
				}
			}
			for (i = 0; i < parent_ce->default_static_members_count; i++) {
				SEPARATE_ZVAL_TO_MAKE_IS_REF(&CE_STATIC_MEMBERS(parent_ce)[i]);
				ce->default_static_members_table[i] = CE_STATIC_MEMBERS(parent_ce)[i];
				Z_ADDREF_P(ce->default_static_members_table[i]);
			}
			ce->default_static_members_count += parent_ce->default_static_members_count;
			ce->static_members_table = ce->default_static_members_table;
		}
	} else {
		if (parent_ce->default_static_members_count) {
			int i = ce->default_static_members_count + parent_ce->default_static_members_count;

			ce->default_static_members_table = perealloc(ce->default_static_members_table, sizeof(void*) * i, ce->type == ZEND_INTERNAL_CLASS);
			if (ce->default_static_members_count) {
				while (i-- > parent_ce->default_static_members_count) {
					ce->default_static_members_table[i] = ce->default_static_members_table[i - parent_ce->default_static_members_count];
				}
			}
			for (i = 0; i < parent_ce->default_static_members_count; i++) {
				SEPARATE_ZVAL_TO_MAKE_IS_REF(&parent_ce->default_static_members_table[i]);
				ce->default_static_members_table[i] = parent_ce->default_static_members_table[i];
				Z_ADDREF_P(ce->default_static_members_table[i]);
			}
			ce->default_static_members_count += parent_ce->default_static_members_count;
			if (ce->type == ZEND_USER_CLASS) {
				ce->static_members_table = ce->default_static_members_table;
			}
		}
	}

	for (zend_hash_internal_pointer_reset(&ce->properties_info);
	zend_hash_get_current_data(&ce->properties_info, (void *) &property_info) == SUCCESS;
	zend_hash_move_forward(&ce->properties_info)) {
		if (property_info->ce == ce) {
			if (property_info->flags & ZEND_ACC_STATIC) {
				property_info->offset += parent_ce->default_static_members_count;
			} else {
				property_info->offset += parent_ce->default_properties_count;
			}
		}
	}

	zend_hash_merge_ex(&ce->properties_info, &parent_ce->properties_info, (copy_ctor_func_t) (ce->type & ZEND_INTERNAL_CLASS ? zend_duplicate_property_info_internal : zend_duplicate_property_info), sizeof(zend_property_info), (merge_checker_func_t) do_inherit_property_access_check, ce);

	zend_hash_merge(&ce->constants_table, &parent_ce->constants_table, zval_property_ctor(parent_ce, ce), NULL, sizeof(zval *), 0);
	zend_hash_merge_ex(&ce->function_table, &parent_ce->function_table, (copy_ctor_func_t) do_inherit_method, sizeof(zend_function), (merge_checker_func_t) do_inherit_method_check, ce);
	do_inherit_parent_constructor(ce);

	if (ce->ce_flags & ZEND_ACC_IMPLICIT_ABSTRACT_CLASS && ce->type == ZEND_INTERNAL_CLASS) {
		ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
	} else if (!(ce->ce_flags & (ZEND_ACC_IMPLEMENT_INTERFACES|ZEND_ACC_IMPLEMENT_TRAITS))) {
		/* The verification will be done in runtime by ZEND_VERIFY_ABSTRACT_CLASS */
		zend_verify_abstract_class(ce TSRMLS_CC);
	}
	ce->ce_flags |= parent_ce->ce_flags & ZEND_HAS_STATIC_IN_METHODS;
}
/* }}} */

static zend_bool do_inherit_constant_check(HashTable *child_constants_table, const zval **parent_constant, const zend_hash_key *hash_key, const zend_class_entry *iface) /* {{{ */
{
	zval **old_constant;

	if (zend_hash_quick_find(child_constants_table, hash_key->arKey, hash_key->nKeyLength, hash_key->h, (void**)&old_constant) == SUCCESS) {
		if (*old_constant != *parent_constant) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot inherit previously-inherited or override constant %s from interface %s", hash_key->arKey, iface->name);
		}
		return 0;
	}
	return 1;
}
/* }}} */

static int do_interface_constant_check(zval **val TSRMLS_DC, int num_args, va_list args, const zend_hash_key *key) /* {{{ */
{
	zend_class_entry **iface = va_arg(args, zend_class_entry**);

	do_inherit_constant_check(&(*iface)->constants_table, (const zval **) val, key, *iface);

	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

ZEND_API void zend_do_implement_interface(zend_class_entry *ce, zend_class_entry *iface TSRMLS_DC) /* {{{ */
{
	zend_uint i, ignore = 0;
	zend_uint current_iface_num = ce->num_interfaces;
	zend_uint parent_iface_num  = ce->parent ? ce->parent->num_interfaces : 0;

	for (i = 0; i < ce->num_interfaces; i++) {
		if (ce->interfaces[i] == NULL) {
			memmove(ce->interfaces + i, ce->interfaces + i + 1, sizeof(zend_class_entry*) * (--ce->num_interfaces - i));
			i--;
		} else if (ce->interfaces[i] == iface) {
			if (i < parent_iface_num) {
				ignore = 1;
			} else {
				zend_error_noreturn(E_COMPILE_ERROR, "Class %s cannot implement previously implemented interface %s", ce->name, iface->name);
			}
		}
	}
	if (ignore) {
		/* Check for attempt to redeclare interface constants */
		zend_hash_apply_with_arguments(&ce->constants_table TSRMLS_CC, (apply_func_args_t) do_interface_constant_check, 1, &iface);
	} else {
		if (ce->num_interfaces >= current_iface_num) {
			if (ce->type == ZEND_INTERNAL_CLASS) {
				ce->interfaces = (zend_class_entry **) realloc(ce->interfaces, sizeof(zend_class_entry *) * (++current_iface_num));
			} else {
				ce->interfaces = (zend_class_entry **) erealloc(ce->interfaces, sizeof(zend_class_entry *) * (++current_iface_num));
			}
		}
		ce->interfaces[ce->num_interfaces++] = iface;

		zend_hash_merge_ex(&ce->constants_table, &iface->constants_table, (copy_ctor_func_t) zval_add_ref, sizeof(zval *), (merge_checker_func_t) do_inherit_constant_check, iface);
		zend_hash_merge_ex(&ce->function_table, &iface->function_table, (copy_ctor_func_t) do_inherit_method, sizeof(zend_function), (merge_checker_func_t) do_inherit_method_check, ce);

		do_implement_interface(ce, iface TSRMLS_CC);
		zend_do_inherit_interfaces(ce, iface TSRMLS_CC);
	}
}
/* }}} */

ZEND_API void zend_do_implement_trait(zend_class_entry *ce, zend_class_entry *trait TSRMLS_DC) /* {{{ */
{
	zend_uint i, ignore = 0;
	zend_uint current_trait_num = ce->num_traits;
	zend_uint parent_trait_num  = ce->parent ? ce->parent->num_traits : 0;

	for (i = 0; i < ce->num_traits; i++) {
		if (ce->traits[i] == NULL) {
			memmove(ce->traits + i, ce->traits + i + 1, sizeof(zend_class_entry*) * (--ce->num_traits - i));
			i--;
		} else if (ce->traits[i] == trait) {
			if (i < parent_trait_num) {
				ignore = 1;
			}
		}
	}
	if (!ignore) {
		if (ce->num_traits >= current_trait_num) {
			if (ce->type == ZEND_INTERNAL_CLASS) {
				ce->traits = (zend_class_entry **) realloc(ce->traits, sizeof(zend_class_entry *) * (++current_trait_num));
			} else {
				ce->traits = (zend_class_entry **) erealloc(ce->traits, sizeof(zend_class_entry *) * (++current_trait_num));
			}
		}
		ce->traits[ce->num_traits++] = trait;
	}
}
/* }}} */

static zend_bool zend_traits_method_compatibility_check(zend_function *fn, zend_function *other_fn TSRMLS_DC) /* {{{ */
{
	zend_uint    fn_flags = fn->common.scope->ce_flags;
	zend_uint other_flags = other_fn->common.scope->ce_flags;

	return zend_do_perform_implementation_check(fn, other_fn TSRMLS_CC)
		&& ((other_fn->common.scope->ce_flags & ZEND_ACC_INTERFACE) || zend_do_perform_implementation_check(other_fn, fn TSRMLS_CC))
		&& ((fn_flags & (ZEND_ACC_FINAL|ZEND_ACC_STATIC)) ==
		    (other_flags & (ZEND_ACC_FINAL|ZEND_ACC_STATIC))); /* equal final and static qualifier */
}
/* }}} */

static void zend_add_magic_methods(zend_class_entry* ce, const char* mname, uint mname_len, zend_function* fe TSRMLS_DC) /* {{{ */
{
	if (!strncmp(mname, ZEND_CLONE_FUNC_NAME, mname_len)) {
		ce->clone = fe; fe->common.fn_flags |= ZEND_ACC_CLONE;
	} else if (!strncmp(mname, ZEND_CONSTRUCTOR_FUNC_NAME, mname_len)) {
		if (ce->constructor && (!ce->parent || ce->constructor != ce->parent->constructor)) {
			zend_error_noreturn(E_COMPILE_ERROR, "%s has colliding constructor definitions coming from traits", ce->name);
		}
		ce->constructor = fe; fe->common.fn_flags |= ZEND_ACC_CTOR;
	} else if (!strncmp(mname, ZEND_DESTRUCTOR_FUNC_NAME,  mname_len)) {
		ce->destructor = fe; fe->common.fn_flags |= ZEND_ACC_DTOR;
	} else if (!strncmp(mname, ZEND_GET_FUNC_NAME, mname_len)) {
		ce->__get = fe;
	} else if (!strncmp(mname, ZEND_SET_FUNC_NAME, mname_len)) {
		ce->__set = fe;
	} else if (!strncmp(mname, ZEND_CALL_FUNC_NAME, mname_len)) {
		ce->__call = fe;
	} else if (!strncmp(mname, ZEND_UNSET_FUNC_NAME, mname_len)) {
		ce->__unset = fe;
	} else if (!strncmp(mname, ZEND_ISSET_FUNC_NAME, mname_len)) {
		ce->__isset = fe;
	} else if (!strncmp(mname, ZEND_CALLSTATIC_FUNC_NAME, mname_len)) {
		ce->__callstatic = fe;
	} else if (!strncmp(mname, ZEND_TOSTRING_FUNC_NAME, mname_len)) {
		ce->__tostring = fe;
	} else if (!strncmp(mname, ZEND_DEBUGINFO_FUNC_NAME, mname_len)) {
		ce->__debugInfo = fe;
	} else if (ce->name_length + 1 == mname_len) {
		char *lowercase_name = emalloc(ce->name_length + 1);
		zend_str_tolower_copy(lowercase_name, ce->name, ce->name_length);
		lowercase_name = (char*)zend_new_interned_string(lowercase_name, ce->name_length + 1, 1 TSRMLS_CC);
		if (!memcmp(mname, lowercase_name, mname_len)) {
			if (ce->constructor && (!ce->parent || ce->constructor != ce->parent->constructor)) {
				zend_error_noreturn(E_COMPILE_ERROR, "%s has colliding constructor definitions coming from traits", ce->name);
			}
			ce->constructor = fe;
			fe->common.fn_flags |= ZEND_ACC_CTOR;
		}
		str_efree(lowercase_name);
	}
}
/* }}} */

static void zend_add_trait_method(zend_class_entry *ce, const char *name, const char *arKey, uint nKeyLength, zend_function *fn, HashTable **overriden TSRMLS_DC) /* {{{ */
{
	zend_function *existing_fn = NULL;
	ulong h = zend_hash_func(arKey, nKeyLength);

	if (zend_hash_quick_find(&ce->function_table, arKey, nKeyLength, h, (void**) &existing_fn) == SUCCESS) {
		if (existing_fn->common.scope == ce) {
			/* members from the current class override trait methods */
			/* use temporary *overriden HashTable to detect hidden conflict */
			if (*overriden) {
				if (zend_hash_quick_find(*overriden, arKey, nKeyLength, h, (void**) &existing_fn) == SUCCESS) {
					if (existing_fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
						/* Make sure the trait method is compatible with previosly declared abstract method */
						if (!zend_traits_method_compatibility_check(fn, existing_fn TSRMLS_CC)) {
							zend_error_noreturn(E_COMPILE_ERROR, "Declaration of %s must be compatible with %s",
								zend_get_function_declaration(fn TSRMLS_CC),
								zend_get_function_declaration(existing_fn TSRMLS_CC));
						}
					} else if (fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
						/* Make sure the abstract declaration is compatible with previous declaration */
						if (!zend_traits_method_compatibility_check(existing_fn, fn TSRMLS_CC)) {
							zend_error_noreturn(E_COMPILE_ERROR, "Declaration of %s must be compatible with %s",
								zend_get_function_declaration(fn TSRMLS_CC),
								zend_get_function_declaration(existing_fn TSRMLS_CC));
						}
						return;
					}
				}
			} else {
				ALLOC_HASHTABLE(*overriden);
				zend_hash_init_ex(*overriden, 2, NULL, NULL, 0, 0);
			}
			zend_hash_quick_update(*overriden, arKey, nKeyLength, h, fn, sizeof(zend_function), (void**)&fn);
			return;
		} else if (existing_fn->common.fn_flags & ZEND_ACC_ABSTRACT &&
				(existing_fn->common.scope->ce_flags & ZEND_ACC_INTERFACE) == 0) {
			/* Make sure the trait method is compatible with previosly declared abstract method */
			if (!zend_traits_method_compatibility_check(fn, existing_fn TSRMLS_CC)) {
				zend_error_noreturn(E_COMPILE_ERROR, "Declaration of %s must be compatible with %s",
					zend_get_function_declaration(fn TSRMLS_CC),
					zend_get_function_declaration(existing_fn TSRMLS_CC));
			}
		} else if (fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
			/* Make sure the abstract declaration is compatible with previous declaration */
			if (!zend_traits_method_compatibility_check(existing_fn, fn TSRMLS_CC)) {
				zend_error_noreturn(E_COMPILE_ERROR, "Declaration of %s must be compatible with %s",
					zend_get_function_declaration(fn TSRMLS_CC),
					zend_get_function_declaration(existing_fn TSRMLS_CC));
			}
			return;
		} else if ((existing_fn->common.scope->ce_flags & ZEND_ACC_TRAIT) == ZEND_ACC_TRAIT) {
			/* two traits can't define the same non-abstract method */
#if 1
			zend_error_noreturn(E_COMPILE_ERROR, "Trait method %s has not been applied, because there are collisions with other trait methods on %s",
				name, ce->name);
#else		/* TODO: better error message */
			zend_error_noreturn(E_COMPILE_ERROR, "Trait method %s::%s has not been applied as %s::%s, because of collision with %s::%s",
				fn->common.scope->name, fn->common.function_name,
				ce->name, name,
				existing_fn->common.scope->name, existing_fn->common.function_name);
#endif
		} else {
			/* inherited members are overridden by members inserted by traits */
			/* check whether the trait method fulfills the inheritance requirements */
			do_inheritance_check_on_method(fn, existing_fn TSRMLS_CC);
			fn->common.prototype = NULL;
		}
	}

	function_add_ref(fn);
	zend_hash_quick_update(&ce->function_table, arKey, nKeyLength, h, fn, sizeof(zend_function), (void**)&fn);
	zend_add_magic_methods(ce, arKey, nKeyLength, fn TSRMLS_CC);
}
/* }}} */

static int zend_fixup_trait_method(zend_function *fn, zend_class_entry *ce TSRMLS_DC) /* {{{ */
{
	if ((fn->common.scope->ce_flags & ZEND_ACC_TRAIT) == ZEND_ACC_TRAIT) {

		fn->common.scope = ce;

		if (fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
			ce->ce_flags |= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
		}
		if (fn->type == ZEND_USER_FUNCTION && fn->op_array.static_variables) {
			ce->ce_flags |= ZEND_HAS_STATIC_IN_METHODS;
		}
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static int zend_traits_copy_functions(zend_function *fn TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	zend_class_entry  *ce;
	HashTable         **overriden;
	zend_trait_alias  *alias, **alias_ptr;
	HashTable         *exclude_table;
	char              *lcname;
	unsigned int       fnname_len;
	zend_function      fn_copy;
	void              *dummy;

	ce            = va_arg(args, zend_class_entry*);
	overriden     = va_arg(args, HashTable**);
	exclude_table = va_arg(args, HashTable*);

	fnname_len = hash_key->nKeyLength - 1;

	/* apply aliases which are qualified with a class name, there should not be any ambiguity */
	if (ce->trait_aliases) {
		alias_ptr = ce->trait_aliases;
		alias = *alias_ptr;
		while (alias) {
			/* Scope unset or equal to the function we compare to, and the alias applies to fn */
			if (alias->alias != NULL
				&& (!alias->trait_method->ce || fn->common.scope == alias->trait_method->ce)
				&& alias->trait_method->mname_len == fnname_len
				&& (zend_binary_strcasecmp(alias->trait_method->method_name, alias->trait_method->mname_len, hash_key->arKey, fnname_len) == 0)) {
				fn_copy = *fn;

				/* if it is 0, no modifieres has been changed */
				if (alias->modifiers) {
					fn_copy.common.fn_flags = alias->modifiers | (fn->common.fn_flags ^ (fn->common.fn_flags & ZEND_ACC_PPP_MASK));
				}

				lcname = zend_str_tolower_dup(alias->alias, alias->alias_len);
				zend_add_trait_method(ce, alias->alias, lcname, alias->alias_len+1, &fn_copy, overriden TSRMLS_CC);
				efree(lcname);

				/* Record the trait from which this alias was resolved. */
				if (!alias->trait_method->ce) {
					alias->trait_method->ce = fn->common.scope;
				}
			}
			alias_ptr++;
			alias = *alias_ptr;
		}
	}

	lcname = (char *) hash_key->arKey;

	if (exclude_table == NULL || zend_hash_find(exclude_table, lcname, fnname_len, &dummy) == FAILURE) {
		/* is not in hashtable, thus, function is not to be excluded */
		fn_copy = *fn;

		/* apply aliases which have not alias name, just setting visibility */
		if (ce->trait_aliases) {
			alias_ptr = ce->trait_aliases;
			alias = *alias_ptr;
			while (alias) {
				/* Scope unset or equal to the function we compare to, and the alias applies to fn */
				if (alias->alias == NULL && alias->modifiers != 0
					&& (!alias->trait_method->ce || fn->common.scope == alias->trait_method->ce)
					&& (alias->trait_method->mname_len == fnname_len)
					&& (zend_binary_strcasecmp(alias->trait_method->method_name, alias->trait_method->mname_len, lcname, fnname_len) == 0)) {

					fn_copy.common.fn_flags = alias->modifiers | (fn->common.fn_flags ^ (fn->common.fn_flags & ZEND_ACC_PPP_MASK));

					/** Record the trait from which this alias was resolved. */
					if (!alias->trait_method->ce) {
						alias->trait_method->ce = fn->common.scope;
					}
				}
				alias_ptr++;
				alias = *alias_ptr;
			}
		}

		zend_add_trait_method(ce, fn->common.function_name, lcname, fnname_len+1, &fn_copy, overriden TSRMLS_CC);
	}

	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static void zend_check_trait_usage(zend_class_entry *ce, zend_class_entry *trait TSRMLS_DC) /* {{{ */
{
	zend_uint i;

	if ((trait->ce_flags & ZEND_ACC_TRAIT) != ZEND_ACC_TRAIT) {
		zend_error_noreturn(E_COMPILE_ERROR, "Class %s is not a trait, Only traits may be used in 'as' and 'insteadof' statements", trait->name);
	}

	for (i = 0; i < ce->num_traits; i++) {
		if (ce->traits[i] == trait) {
			return;
		}
	}
	zend_error_noreturn(E_COMPILE_ERROR, "Required Trait %s wasn't added to %s", trait->name, ce->name);
}
/* }}} */

static void zend_traits_init_trait_structures(zend_class_entry *ce TSRMLS_DC) /* {{{ */
{
	size_t i, j = 0;
	zend_trait_precedence **precedences;
	zend_trait_precedence *cur_precedence;
	zend_trait_method_reference *cur_method_ref;
	char *lcname;
	zend_bool method_exists;

	/* resolve class references */
	if (ce->trait_precedences) {
		i = 0;
		precedences = ce->trait_precedences;
		ce->trait_precedences = NULL;
		while ((cur_precedence = precedences[i])) {
			/** Resolve classes for all precedence operations. */
			if (cur_precedence->exclude_from_classes) {
				cur_method_ref = cur_precedence->trait_method;
				if (!(cur_precedence->trait_method->ce = zend_fetch_class(cur_method_ref->class_name, cur_method_ref->cname_len,
								ZEND_FETCH_CLASS_TRAIT|ZEND_FETCH_CLASS_NO_AUTOLOAD TSRMLS_CC))) {
					zend_error_noreturn(E_COMPILE_ERROR, "Could not find trait %s", cur_method_ref->class_name);
				}
				zend_check_trait_usage(ce, cur_precedence->trait_method->ce TSRMLS_CC);

				/** Ensure that the prefered method is actually available. */
				lcname = zend_str_tolower_dup(cur_method_ref->method_name,
											  cur_method_ref->mname_len);
				method_exists = zend_hash_exists(&cur_method_ref->ce->function_table,
												 lcname,
												 cur_method_ref->mname_len + 1);
				efree(lcname);
				if (!method_exists) {
					zend_error_noreturn(E_COMPILE_ERROR,
							   "A precedence rule was defined for %s::%s but this method does not exist",
							   cur_method_ref->ce->name,
							   cur_method_ref->method_name);
				}

				/** With the other traits, we are more permissive.
					We do not give errors for those. This allows to be more
					defensive in such definitions.
					However, we want to make sure that the insteadof declaration
					is consistent in itself.
				 */
				j = 0;
				while (cur_precedence->exclude_from_classes[j]) {
					char* class_name = (char*)cur_precedence->exclude_from_classes[j];
					zend_uint name_length = strlen(class_name);

					if (!(cur_precedence->exclude_from_classes[j] = zend_fetch_class(class_name, name_length, ZEND_FETCH_CLASS_TRAIT |ZEND_FETCH_CLASS_NO_AUTOLOAD TSRMLS_CC))) {
						zend_error_noreturn(E_COMPILE_ERROR, "Could not find trait %s", class_name);
					}
					zend_check_trait_usage(ce, cur_precedence->exclude_from_classes[j] TSRMLS_CC);

					/* make sure that the trait method is not from a class mentioned in
					 exclude_from_classes, for consistency */
					if (cur_precedence->trait_method->ce == cur_precedence->exclude_from_classes[j]) {
						zend_error(E_COMPILE_ERROR,
								   "Inconsistent insteadof definition. "
								   "The method %s is to be used from %s, but %s is also on the exclude list",
								   cur_method_ref->method_name,
								   cur_precedence->trait_method->ce->name,
								   cur_precedence->trait_method->ce->name);
					}

					efree(class_name);
					j++;
				}
			}
			i++;
		}
		ce->trait_precedences = precedences;
	}

	if (ce->trait_aliases) {
		i = 0;
		while (ce->trait_aliases[i]) {
			/** For all aliases with an explicit class name, resolve the class now. */
			if (ce->trait_aliases[i]->trait_method->class_name) {
				cur_method_ref = ce->trait_aliases[i]->trait_method;
				if (!(cur_method_ref->ce = zend_fetch_class(cur_method_ref->class_name, cur_method_ref->cname_len, ZEND_FETCH_CLASS_TRAIT|ZEND_FETCH_CLASS_NO_AUTOLOAD TSRMLS_CC))) {
					zend_error_noreturn(E_COMPILE_ERROR, "Could not find trait %s", cur_method_ref->class_name);
				}
				zend_check_trait_usage(ce, cur_method_ref->ce TSRMLS_CC);

				/** And, ensure that the referenced method is resolvable, too. */
				lcname = zend_str_tolower_dup(cur_method_ref->method_name,
						cur_method_ref->mname_len);
				method_exists = zend_hash_exists(&cur_method_ref->ce->function_table,
						lcname, cur_method_ref->mname_len + 1);
				efree(lcname);

				if (!method_exists) {
					zend_error_noreturn(E_COMPILE_ERROR, "An alias was defined for %s::%s but this method does not exist", cur_method_ref->ce->name, cur_method_ref->method_name);
				}
			}
			i++;
		}
	}
}
/* }}} */

static void zend_traits_compile_exclude_table(HashTable* exclude_table, zend_trait_precedence **precedences, zend_class_entry *trait) /* {{{ */
{
	size_t i = 0, j;

	if (!precedences) {
		return;
	}
	while (precedences[i]) {
		if (precedences[i]->exclude_from_classes) {
			j = 0;
			while (precedences[i]->exclude_from_classes[j]) {
				if (precedences[i]->exclude_from_classes[j] == trait) {
					zend_uint lcname_len = precedences[i]->trait_method->mname_len;
					char *lcname = zend_str_tolower_dup(precedences[i]->trait_method->method_name, lcname_len);

					if (zend_hash_add(exclude_table, lcname, lcname_len, NULL, 0, NULL) == FAILURE) {
						efree(lcname);
						zend_error_noreturn(E_COMPILE_ERROR, "Failed to evaluate a trait precedence (%s). Method of trait %s was defined to be excluded multiple times", precedences[i]->trait_method->method_name, trait->name);
					}
					efree(lcname);
				}
				++j;
			}
		}
		++i;
	}
}
/* }}} */

static void zend_do_traits_method_binding(zend_class_entry *ce TSRMLS_DC) /* {{{ */
{
	zend_uint i;
	HashTable *overriden = NULL;

	for (i = 0; i < ce->num_traits; i++) {
		if (ce->trait_precedences) {
			HashTable exclude_table;
			zend_trait_precedence **precedences;

			/* TODO: revisit this start size, may be its not optimal */
			zend_hash_init_ex(&exclude_table, 2, NULL, NULL, 0, 0);

			precedences = ce->trait_precedences;
			ce->trait_precedences = NULL;
			zend_traits_compile_exclude_table(&exclude_table, precedences, ce->traits[i]);

			/* copies functions, applies defined aliasing, and excludes unused trait methods */
			zend_hash_apply_with_arguments(&ce->traits[i]->function_table TSRMLS_CC, (apply_func_args_t)zend_traits_copy_functions, 3, ce, &overriden, &exclude_table);

			zend_hash_destroy(&exclude_table);
			ce->trait_precedences = precedences;
		} else {
			zend_hash_apply_with_arguments(&ce->traits[i]->function_table TSRMLS_CC, (apply_func_args_t)zend_traits_copy_functions, 3, ce, &overriden, NULL);
		}
	}

    zend_hash_apply_with_argument(&ce->function_table, (apply_func_arg_t)zend_fixup_trait_method, ce TSRMLS_CC);

	if (ce->trait_precedences) {
		i = 0;
		while (ce->trait_precedences[i]) {
			if (ce->trait_precedences[i]->exclude_from_classes) {
				efree(ce->trait_precedences[i]->exclude_from_classes);
				ce->trait_precedences[i]->exclude_from_classes = NULL;
			}
			i++;
		}
	}

	if (overriden) {
		zend_hash_destroy(overriden);
		FREE_HASHTABLE(overriden);
	}
}
/* }}} */

static zend_class_entry* find_first_definition(zend_class_entry *ce, size_t current_trait, const char* prop_name, int prop_name_length, ulong prop_hash, zend_class_entry *coliding_ce) /* {{{ */
{
	size_t i;

	if (coliding_ce == ce) {
		for (i = 0; i < current_trait; i++) {
			if (zend_hash_quick_exists(&ce->traits[i]->properties_info, prop_name, prop_name_length+1, prop_hash)) {
				return ce->traits[i];
			}
		}
	}

	return coliding_ce;
}
/* }}} */

static void zend_do_traits_property_binding(zend_class_entry *ce TSRMLS_DC) /* {{{ */
{
	size_t i;
	zend_property_info *property_info;
	zend_property_info *coliding_prop;
	zval compare_result;
	const char* prop_name;
	int   prop_name_length;
	ulong prop_hash;
	const char* class_name_unused;
	zend_bool not_compatible;
	zval* prop_value;
	char* doc_comment;
	zend_uint flags;

	/* In the following steps the properties are inserted into the property table
	 * for that, a very strict approach is applied:
	 * - check for compatibility, if not compatible with any property in class -> fatal
	 * - if compatible, then strict notice
	 */
	for (i = 0; i < ce->num_traits; i++) {
		for (zend_hash_internal_pointer_reset(&ce->traits[i]->properties_info);
			 zend_hash_get_current_data(&ce->traits[i]->properties_info, (void *) &property_info) == SUCCESS;
			 zend_hash_move_forward(&ce->traits[i]->properties_info)) {
			/* first get the unmangeld name if necessary,
			 * then check whether the property is already there
			 */
			flags = property_info->flags;
			if ((flags & ZEND_ACC_PPP_MASK) == ZEND_ACC_PUBLIC) {
				prop_hash = property_info->h;
				prop_name = property_info->name;
				prop_name_length = property_info->name_length;
			} else {
				/* for private and protected we need to unmangle the names */
				zend_unmangle_property_name_ex(property_info->name, property_info->name_length,
											&class_name_unused, &prop_name, &prop_name_length);
				prop_hash = zend_get_hash_value(prop_name, prop_name_length + 1);
			}

			/* next: check for conflicts with current class */
			if (zend_hash_quick_find(&ce->properties_info, prop_name, prop_name_length+1, prop_hash, (void **) &coliding_prop) == SUCCESS) {
				if (coliding_prop->flags & ZEND_ACC_SHADOW) {
					zend_hash_quick_del(&ce->properties_info, prop_name, prop_name_length+1, prop_hash);
					flags |= ZEND_ACC_CHANGED;
				} else {
					if ((coliding_prop->flags & (ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC))
						== (flags & (ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC))) {
						/* flags are identical, now the value needs to be checked */
						if (flags & ZEND_ACC_STATIC) {
							not_compatible = (FAILURE == compare_function(&compare_result,
											  ce->default_static_members_table[coliding_prop->offset],
											  ce->traits[i]->default_static_members_table[property_info->offset] TSRMLS_CC))
								  || (Z_LVAL(compare_result) != 0);
						} else {
							not_compatible = (FAILURE == compare_function(&compare_result,
											  ce->default_properties_table[coliding_prop->offset],
											  ce->traits[i]->default_properties_table[property_info->offset] TSRMLS_CC))
								  || (Z_LVAL(compare_result) != 0);
						}
					} else {
						/* the flags are not identical, thus, we assume properties are not compatible */
						not_compatible = 1;
					}

					if (not_compatible) {
						zend_error_noreturn(E_COMPILE_ERROR,
							   "%s and %s define the same property ($%s) in the composition of %s. However, the definition differs and is considered incompatible. Class was composed",
								find_first_definition(ce, i, prop_name, prop_name_length, prop_hash, coliding_prop->ce)->name,
								property_info->ce->name,
								prop_name,
								ce->name);
					} else {
						zend_error(E_STRICT,
							   "%s and %s define the same property ($%s) in the composition of %s. This might be incompatible, to improve maintainability consider using accessor methods in traits instead. Class was composed",
								find_first_definition(ce, i, prop_name, prop_name_length, prop_hash, coliding_prop->ce)->name,
								property_info->ce->name,
								prop_name,
								ce->name);
						continue;
					}
				}
			}

			/* property not found, so lets add it */
			if (flags & ZEND_ACC_STATIC) {
				prop_value = ce->traits[i]->default_static_members_table[property_info->offset];
			} else {
				prop_value = ce->traits[i]->default_properties_table[property_info->offset];
			}
			Z_ADDREF_P(prop_value);

			doc_comment = property_info->doc_comment ? estrndup(property_info->doc_comment, property_info->doc_comment_len) : NULL;
			zend_declare_property_ex(ce, prop_name, prop_name_length,
									 prop_value, flags,
								     doc_comment, property_info->doc_comment_len TSRMLS_CC);
		}
	}
}
/* }}} */

static void zend_do_check_for_inconsistent_traits_aliasing(zend_class_entry *ce TSRMLS_DC) /* {{{ */
{
	int i = 0;
	zend_trait_alias* cur_alias;
	char* lc_method_name;

	if (ce->trait_aliases) {
		while (ce->trait_aliases[i]) {
			cur_alias = ce->trait_aliases[i];
			/** The trait for this alias has not been resolved, this means, this
				alias was not applied. Abort with an error. */
			if (!cur_alias->trait_method->ce) {
				if (cur_alias->alias) {
					/** Plain old inconsistency/typo/bug */
					zend_error_noreturn(E_COMPILE_ERROR,
							   "An alias (%s) was defined for method %s(), but this method does not exist",
							   cur_alias->alias,
							   cur_alias->trait_method->method_name);
				} else {
					/** Here are two possible cases:
						1) this is an attempt to modifiy the visibility
						   of a method introduce as part of another alias.
						   Since that seems to violate the DRY principle,
						   we check against it and abort.
						2) it is just a plain old inconsitency/typo/bug
						   as in the case where alias is set. */

					lc_method_name = zend_str_tolower_dup(cur_alias->trait_method->method_name,
														  cur_alias->trait_method->mname_len);
					if (zend_hash_exists(&ce->function_table,
										 lc_method_name,
										 cur_alias->trait_method->mname_len+1)) {
						efree(lc_method_name);
						zend_error_noreturn(E_COMPILE_ERROR,
								   "The modifiers for the trait alias %s() need to be changed in the same statment in which the alias is defined. Error",
								   cur_alias->trait_method->method_name);
					} else {
						efree(lc_method_name);
						zend_error_noreturn(E_COMPILE_ERROR,
								   "The modifiers of the trait method %s() are changed, but this method does not exist. Error",
								   cur_alias->trait_method->method_name);

					}
				}
			}
			i++;
		}
	}
}
/* }}} */

ZEND_API void zend_do_bind_traits(zend_class_entry *ce TSRMLS_DC) /* {{{ */
{

	if (ce->num_traits <= 0) {
		return;
	}

	/* complete initialization of trait strutures in ce */
	zend_traits_init_trait_structures(ce TSRMLS_CC);

	/* first care about all methods to be flattened into the class */
	zend_do_traits_method_binding(ce TSRMLS_CC);

	/* Aliases which have not been applied indicate typos/bugs. */
	zend_do_check_for_inconsistent_traits_aliasing(ce TSRMLS_CC);

	/* then flatten the properties into it, to, mostly to notfiy developer about problems */
	zend_do_traits_property_binding(ce TSRMLS_CC);

	/* verify that all abstract methods from traits have been implemented */
	zend_verify_abstract_class(ce TSRMLS_CC);

	/* now everything should be fine and an added ZEND_ACC_IMPLICIT_ABSTRACT_CLASS should be removed */
	if (ce->ce_flags & ZEND_ACC_IMPLICIT_ABSTRACT_CLASS) {
		ce->ce_flags -= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
	}
}
/* }}} */

ZEND_API int do_bind_function(const zend_op_array *op_array, zend_op *opline, HashTable *function_table, zend_bool compile_time) /* {{{ */
{
	zend_function *function;
	zval *op1, *op2;

	if (compile_time) {
		op1 = &CONSTANT_EX(op_array, opline->op1.constant);
		op2 = &CONSTANT_EX(op_array, opline->op2.constant);
	} else {
		op1 = opline->op1.zv;
		op2 = opline->op2.zv;
	}

	zend_hash_quick_find(function_table, Z_STRVAL_P(op1), Z_STRLEN_P(op1), Z_HASH_P(op1), (void *) &function);
	if (zend_hash_quick_add(function_table, Z_STRVAL_P(op2), Z_STRLEN_P(op2)+1, Z_HASH_P(op2), function, sizeof(zend_function), NULL)==FAILURE) {
		int error_level = compile_time ? E_COMPILE_ERROR : E_ERROR;
		zend_function *old_function;

		if (zend_hash_quick_find(function_table, Z_STRVAL_P(op2), Z_STRLEN_P(op2)+1, Z_HASH_P(op2), (void *) &old_function)==SUCCESS
			&& old_function->type == ZEND_USER_FUNCTION
			&& old_function->op_array.last > 0) {
			zend_error(error_level, "Cannot redeclare %s() (previously declared in %s:%d)",
						function->common.function_name,
						old_function->op_array.filename,
						old_function->op_array.opcodes[0].lineno);
		} else {
			zend_error(error_level, "Cannot redeclare %s()", function->common.function_name);
		}
		return FAILURE;
	} else {
		(*function->op_array.refcount)++;
		function->op_array.static_variables = NULL; /* NULL out the unbound function */
		return SUCCESS;
	}
}
/* }}} */

void zend_prepare_reference(znode *result, znode *class_name, znode *method_name TSRMLS_DC) /* {{{ */
{
	zend_trait_method_reference *method_ref = emalloc(sizeof(zend_trait_method_reference));
	method_ref->ce = NULL;

	/* REM: There should not be a need for copying,
	   zend_do_begin_class_declaration is also just using that string */
	if (class_name) {
		zend_resolve_class_name(class_name TSRMLS_CC);
		method_ref->class_name = Z_STRVAL(class_name->u.constant);
		method_ref->cname_len  = Z_STRLEN(class_name->u.constant);
	} else {
		method_ref->class_name = NULL;
		method_ref->cname_len  = 0;
	}

	method_ref->method_name = Z_STRVAL(method_name->u.constant);
	method_ref->mname_len   = Z_STRLEN(method_name->u.constant);

	result->u.op.ptr = method_ref;
	result->op_type = IS_TMP_VAR;
}
/* }}} */

void zend_add_trait_alias(znode *method_reference, znode *modifiers, znode *alias TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce = CG(active_class_entry);
	zend_trait_alias *trait_alias;

	if (Z_LVAL(modifiers->u.constant) == ZEND_ACC_STATIC) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use 'static' as method modifier");
		return;
	} else if (Z_LVAL(modifiers->u.constant) == ZEND_ACC_ABSTRACT) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use 'abstract' as method modifier");
		return;
	} else if (Z_LVAL(modifiers->u.constant) == ZEND_ACC_FINAL) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use 'final' as method modifier");
		return;
	}

	trait_alias = emalloc(sizeof(zend_trait_alias));
	trait_alias->trait_method = (zend_trait_method_reference*)method_reference->u.op.ptr;
	trait_alias->modifiers = Z_LVAL(modifiers->u.constant);
	if (alias) {
		trait_alias->alias = Z_STRVAL(alias->u.constant);
		trait_alias->alias_len = Z_STRLEN(alias->u.constant);
	} else {
		trait_alias->alias = NULL;
	}
	zend_add_to_list(&ce->trait_aliases, trait_alias TSRMLS_CC);
}
/* }}} */

void zend_add_trait_precedence(znode *method_reference, znode *trait_list TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce = CG(active_class_entry);
	zend_trait_precedence *trait_precedence = emalloc(sizeof(zend_trait_precedence));

	trait_precedence->trait_method = (zend_trait_method_reference*)method_reference->u.op.ptr;
	trait_precedence->exclude_from_classes = (zend_class_entry**) trait_list->u.op.ptr;

	zend_add_to_list(&ce->trait_precedences, trait_precedence TSRMLS_CC);
}
/* }}} */

ZEND_API zend_class_entry *do_bind_class(const zend_op_array* op_array, const zend_op *opline, HashTable *class_table, zend_bool compile_time TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce, **pce;
	zval *op1, *op2;

	if (compile_time) {
		op1 = &CONSTANT_EX(op_array, opline->op1.constant);
		op2 = &CONSTANT_EX(op_array, opline->op2.constant);
	} else {
		op1 = opline->op1.zv;
		op2 = opline->op2.zv;
	}
	if (zend_hash_quick_find(class_table, Z_STRVAL_P(op1), Z_STRLEN_P(op1), Z_HASH_P(op1), (void **) &pce)==FAILURE) {
		zend_error_noreturn(E_COMPILE_ERROR, "Internal Zend error - Missing class information for %s", Z_STRVAL_P(op1));
		return NULL;
	} else {
		ce = *pce;
	}
	ce->refcount++;
	if (zend_hash_quick_add(class_table, Z_STRVAL_P(op2), Z_STRLEN_P(op2)+1, Z_HASH_P(op2), &ce, sizeof(zend_class_entry *), NULL)==FAILURE) {
		ce->refcount--;
		if (!compile_time) {
			/* If we're in compile time, in practice, it's quite possible
			 * that we'll never reach this class declaration at runtime,
			 * so we shut up about it.  This allows the if (!defined('FOO')) { return; }
			 * approach to work.
			 */
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare class %s", ce->name);
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
	zend_class_entry *ce, **pce;
	int found_ce;
	zval *op1, *op2;

	if (compile_time) {
		op1 = &CONSTANT_EX(op_array, opline->op1.constant);
		op2 = &CONSTANT_EX(op_array, opline->op2.constant);
	} else {
		op1 = opline->op1.zv;
		op2 = opline->op2.zv;
	}

	found_ce = zend_hash_quick_find(class_table, Z_STRVAL_P(op1), Z_STRLEN_P(op1), Z_HASH_P(op1), (void **) &pce);

	if (found_ce == FAILURE) {
		if (!compile_time) {
			/* If we're in compile time, in practice, it's quite possible
			 * that we'll never reach this class declaration at runtime,
			 * so we shut up about it.  This allows the if (!defined('FOO')) { return; }
			 * approach to work.
			 */
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare class %s", Z_STRVAL_P(op2));
		}
		return NULL;
	} else {
		ce = *pce;
	}

	if (parent_ce->ce_flags & ZEND_ACC_INTERFACE) {
		zend_error_noreturn(E_COMPILE_ERROR, "Class %s cannot extend from interface %s", ce->name, parent_ce->name);
	} else if ((parent_ce->ce_flags & ZEND_ACC_TRAIT) == ZEND_ACC_TRAIT) {
		zend_error_noreturn(E_COMPILE_ERROR, "Class %s cannot extend from trait %s", ce->name, parent_ce->name);
	}

	zend_do_inheritance(ce, parent_ce TSRMLS_CC);

	ce->refcount++;

	/* Register the derived class */
	if (zend_hash_quick_add(class_table, Z_STRVAL_P(op2), Z_STRLEN_P(op2)+1, Z_HASH_P(op2), pce, sizeof(zend_class_entry *), NULL)==FAILURE) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare class %s", ce->name);
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
			if (do_bind_function(CG(active_op_array), opline, CG(function_table), 1) == FAILURE) {
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
				zend_class_entry **pce;

				parent_name = &CONSTANT(fetch_class_opline->op2.constant);
				if ((zend_lookup_class(Z_STRVAL_P(parent_name), Z_STRLEN_P(parent_name), &pce TSRMLS_CC) == FAILURE) ||
				    ((CG(compiler_options) & ZEND_COMPILE_IGNORE_INTERNAL_CLASSES) &&
				     ((*pce)->type == ZEND_INTERNAL_CLASS))) {
				    if (CG(compiler_options) & ZEND_COMPILE_DELAYED_BINDING) {
						zend_uint *opline_num = &CG(active_op_array)->early_binding;

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
				if (do_bind_inherited_class(CG(active_op_array), opline, CG(class_table), *pce, 1 TSRMLS_CC) == NULL) {
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

	zend_hash_quick_del(table, Z_STRVAL(CONSTANT(opline->op1.constant)), Z_STRLEN(CONSTANT(opline->op1.constant)), Z_HASH_P(&CONSTANT(opline->op1.constant)));
	zend_del_literal(CG(active_op_array), opline->op1.constant);
	zend_del_literal(CG(active_op_array), opline->op2.constant);
	MAKE_NOP(opline);
}
/* }}} */

ZEND_API void zend_do_delayed_early_binding(const zend_op_array *op_array TSRMLS_DC) /* {{{ */
{
	if (op_array->early_binding != -1) {
		zend_bool orig_in_compilation = CG(in_compilation);
		zend_uint opline_num = op_array->early_binding;
		zend_class_entry **pce;

		CG(in_compilation) = 1;
		while (opline_num != -1) {
			if (zend_lookup_class(Z_STRVAL_P(op_array->opcodes[opline_num-1].op2.zv), Z_STRLEN_P(op_array->opcodes[opline_num-1].op2.zv), &pce TSRMLS_CC) == SUCCESS) {
				do_bind_inherited_class(op_array, &op_array->opcodes[opline_num], EG(class_table), *pce, 0 TSRMLS_CC);
			}
			opline_num = op_array->opcodes[opline_num].result.opline_num;
		}
		CG(in_compilation) = orig_in_compilation;
	}
}
/* }}} */

void zend_do_boolean_or_begin(znode *expr1, znode *op_token TSRMLS_DC) /* {{{ */
{
	int next_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMPNZ_EX;
	if (expr1->op_type == IS_TMP_VAR) {
		SET_NODE(opline->result, expr1);
	} else {
		opline->result.var = get_temporary_variable(CG(active_op_array));
		opline->result_type = IS_TMP_VAR;
	}
	SET_NODE(opline->op1, expr1);
	SET_UNUSED(opline->op2);

	op_token->u.op.opline_num = next_op_number;

	GET_NODE(expr1, opline->result);
}
/* }}} */

void zend_do_boolean_or_end(znode *result, const znode *expr1, const znode *expr2, znode *op_token TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	*result = *expr1; /* we saved the original result in expr1 */
	opline->opcode = ZEND_BOOL;
	SET_NODE(opline->result, result);
	SET_NODE(opline->op1, expr2);
	SET_UNUSED(opline->op2);

	CG(active_op_array)->opcodes[op_token->u.op.opline_num].op2.opline_num = get_next_op_number(CG(active_op_array));
}
/* }}} */

void zend_do_boolean_and_begin(znode *expr1, znode *op_token TSRMLS_DC) /* {{{ */
{
	int next_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMPZ_EX;
	if (expr1->op_type == IS_TMP_VAR) {
		SET_NODE(opline->result, expr1);
	} else {
		opline->result.var = get_temporary_variable(CG(active_op_array));
		opline->result_type = IS_TMP_VAR;
	}
	SET_NODE(opline->op1, expr1);
	SET_UNUSED(opline->op2);

	op_token->u.op.opline_num = next_op_number;

	GET_NODE(expr1, opline->result);
}
/* }}} */

void zend_do_boolean_and_end(znode *result, const znode *expr1, const znode *expr2, const znode *op_token TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	*result = *expr1; /* we saved the original result in expr1 */
	opline->opcode = ZEND_BOOL;
	SET_NODE(opline->result, result);
	SET_NODE(opline->op1, expr2);
	SET_UNUSED(opline->op2);

	CG(active_op_array)->opcodes[op_token->u.op.opline_num].op2.opline_num = get_next_op_number(CG(active_op_array));
}
/* }}} */

void zend_do_do_while_begin(TSRMLS_D) /* {{{ */
{
	do_begin_loop(TSRMLS_C);
	INC_BPC(CG(active_op_array));
}
/* }}} */

void zend_do_do_while_end(const znode *do_token, const znode *expr_open_bracket, const znode *expr TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMPNZ;
	SET_NODE(opline->op1, expr);
	opline->op2.opline_num = do_token->u.op.opline_num;
	SET_UNUSED(opline->op2);

	do_end_loop(expr_open_bracket->u.op.opline_num, 0 TSRMLS_CC);

	DEC_BPC(CG(active_op_array));
}
/* }}} */

void zend_do_brk_cont(zend_uchar op, const znode *expr TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = op;
	opline->op1.opline_num = CG(context).current_brk_cont;
	SET_UNUSED(opline->op1);
	if (expr) {
		if (expr->op_type != IS_CONST) {
			zend_error_noreturn(E_COMPILE_ERROR, "'%s' operator with non-constant operand is no longer supported", op == ZEND_BRK ? "break" : "continue");
		} else if (Z_TYPE(expr->u.constant) != IS_LONG || Z_LVAL(expr->u.constant) < 1) {
			zend_error_noreturn(E_COMPILE_ERROR, "'%s' operator accepts only positive numbers", op == ZEND_BRK ? "break" : "continue");
		}
		SET_NODE(opline->op2, expr);
	} else {
		LITERAL_LONG(opline->op2, 1);
		opline->op2_type = IS_CONST;
	}
}
/* }}} */

void zend_do_switch_cond(const znode *cond TSRMLS_DC) /* {{{ */
{
	zend_switch_entry switch_entry;

	switch_entry.cond = *cond;
	switch_entry.default_case = -1;
	switch_entry.control_var = -1;
	zend_stack_push(&CG(switch_cond_stack), (void *) &switch_entry, sizeof(switch_entry));

	do_begin_loop(TSRMLS_C);

	INC_BPC(CG(active_op_array));
}
/* }}} */

void zend_do_switch_end(const znode *case_list TSRMLS_DC) /* {{{ */
{
	zend_op *opline;
	zend_switch_entry *switch_entry_ptr;

	zend_stack_top(&CG(switch_cond_stack), (void **) &switch_entry_ptr);

	/* add code to jmp to default case */
	if (switch_entry_ptr->default_case != -1) {
		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = ZEND_JMP;
		SET_UNUSED(opline->op1);
		SET_UNUSED(opline->op2);
		opline->op1.opline_num = switch_entry_ptr->default_case;
	}

	if (case_list->op_type != IS_UNUSED) { /* non-empty switch */
		int next_op_number = get_next_op_number(CG(active_op_array));

		CG(active_op_array)->opcodes[case_list->u.op.opline_num].op1.opline_num = next_op_number;
	}

	/* remember break/continue loop information */
	CG(active_op_array)->brk_cont_array[CG(context).current_brk_cont].cont = CG(active_op_array)->brk_cont_array[CG(context).current_brk_cont].brk = get_next_op_number(CG(active_op_array));
	CG(context).current_brk_cont = CG(active_op_array)->brk_cont_array[CG(context).current_brk_cont].parent;

	if (switch_entry_ptr->cond.op_type==IS_VAR || switch_entry_ptr->cond.op_type==IS_TMP_VAR) {
		/* emit free for the switch condition*/
		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = (switch_entry_ptr->cond.op_type == IS_TMP_VAR) ? ZEND_FREE : ZEND_SWITCH_FREE;
		SET_NODE(opline->op1, &switch_entry_ptr->cond);
		SET_UNUSED(opline->op2);
	}
	if (switch_entry_ptr->cond.op_type == IS_CONST) {
		zval_dtor(&switch_entry_ptr->cond.u.constant);
	}

	zend_stack_del_top(&CG(switch_cond_stack));

	DEC_BPC(CG(active_op_array));
}
/* }}} */

void zend_do_case_before_statement(const znode *case_list, znode *case_token, const znode *case_expr TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	int next_op_number;
	zend_switch_entry *switch_entry_ptr;
	znode result;

	zend_stack_top(&CG(switch_cond_stack), (void **) &switch_entry_ptr);

	if (switch_entry_ptr->control_var == -1) {
		switch_entry_ptr->control_var = get_temporary_variable(CG(active_op_array));
	}
	opline->opcode = ZEND_CASE;
	opline->result.var = switch_entry_ptr->control_var;
	opline->result_type = IS_TMP_VAR;
	SET_NODE(opline->op1, &switch_entry_ptr->cond);
	SET_NODE(opline->op2, case_expr);
	if (opline->op1_type == IS_CONST) {
		zval_copy_ctor(&CONSTANT(opline->op1.constant));
	}
	GET_NODE(&result, opline->result);

	next_op_number = get_next_op_number(CG(active_op_array));
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_JMPZ;
	SET_NODE(opline->op1, &result);
	SET_UNUSED(opline->op2);
	case_token->u.op.opline_num = next_op_number;

	if (case_list->op_type==IS_UNUSED) {
		return;
	}
	next_op_number = get_next_op_number(CG(active_op_array));
	CG(active_op_array)->opcodes[case_list->u.op.opline_num].op1.opline_num = next_op_number;
}
/* }}} */

void zend_do_case_after_statement(znode *result, const znode *case_token TSRMLS_DC) /* {{{ */
{
	int next_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMP;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
	result->u.op.opline_num = next_op_number;

	switch (CG(active_op_array)->opcodes[case_token->u.op.opline_num].opcode) {
		case ZEND_JMP:
			CG(active_op_array)->opcodes[case_token->u.op.opline_num].op1.opline_num = get_next_op_number(CG(active_op_array));
			break;
		case ZEND_JMPZ:
			CG(active_op_array)->opcodes[case_token->u.op.opline_num].op2.opline_num = get_next_op_number(CG(active_op_array));
			break;
	}
}
/* }}} */

void zend_do_default_before_statement(const znode *case_list, znode *default_token TSRMLS_DC) /* {{{ */
{
	int next_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	zend_switch_entry *switch_entry_ptr;

	zend_stack_top(&CG(switch_cond_stack), (void **) &switch_entry_ptr);

	opline->opcode = ZEND_JMP;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
	default_token->u.op.opline_num = next_op_number;

	next_op_number = get_next_op_number(CG(active_op_array));
	switch_entry_ptr->default_case = next_op_number;

	if (case_list->op_type==IS_UNUSED) {
		return;
	}
	CG(active_op_array)->opcodes[case_list->u.op.opline_num].op1.opline_num = next_op_number;
}
/* }}} */

void zend_do_begin_class_declaration(const znode *class_token, znode *class_name, const znode *parent_class_name TSRMLS_DC) /* {{{ */
{
	zend_op *opline;
	int doing_inheritance = 0;
	zend_class_entry *new_class_entry;
	char *lcname;
	int error = 0;
	zval **ns_name, key;

	if (CG(active_class_entry)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Class declarations may not be nested");
		return;
	}

	lcname = zend_str_tolower_dup(Z_STRVAL(class_name->u.constant), Z_STRLEN(class_name->u.constant));

	if (!(strcmp(lcname, "self") && strcmp(lcname, "parent"))) {
		efree(lcname);
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use '%s' as class name as it is reserved", Z_STRVAL(class_name->u.constant));
	}

	/* Class name must not conflict with import names */
	if (CG(current_import) &&
	    zend_hash_find(CG(current_import), lcname, Z_STRLEN(class_name->u.constant)+1, (void**)&ns_name) == SUCCESS) {
		error = 1;
	}

	if (CG(current_namespace)) {
		/* Prefix class name with name of current namespace */
		znode tmp;

		tmp.op_type = IS_CONST;
		tmp.u.constant = *CG(current_namespace);
		zval_copy_ctor(&tmp.u.constant);
		zend_do_build_namespace_name(&tmp, &tmp, class_name TSRMLS_CC);
		*class_name = tmp;
		efree(lcname);
		lcname = zend_str_tolower_dup(Z_STRVAL(class_name->u.constant), Z_STRLEN(class_name->u.constant));
	}

	if (error) {
		char *tmp = zend_str_tolower_dup(Z_STRVAL_PP(ns_name), Z_STRLEN_PP(ns_name));

		if (Z_STRLEN_PP(ns_name) != Z_STRLEN(class_name->u.constant) ||
			memcmp(tmp, lcname, Z_STRLEN(class_name->u.constant))) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot declare class %s because the name is already in use", Z_STRVAL(class_name->u.constant));
		}
		efree(tmp);
	}

	new_class_entry = emalloc(sizeof(zend_class_entry));
	new_class_entry->type = ZEND_USER_CLASS;
	new_class_entry->name = zend_new_interned_string(Z_STRVAL(class_name->u.constant), Z_STRLEN(class_name->u.constant) + 1, 1 TSRMLS_CC);
	new_class_entry->name_length = Z_STRLEN(class_name->u.constant);

	zend_initialize_class_data(new_class_entry, 1 TSRMLS_CC);
	new_class_entry->info.user.filename = zend_get_compiled_filename(TSRMLS_C);
	new_class_entry->info.user.line_start = class_token->u.op.opline_num;
	new_class_entry->ce_flags |= class_token->EA;

	if (parent_class_name && parent_class_name->op_type != IS_UNUSED) {
		switch (parent_class_name->EA) {
			case ZEND_FETCH_CLASS_SELF:
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot use 'self' as class name as it is reserved");
				break;
			case ZEND_FETCH_CLASS_PARENT:
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot use 'parent' as class name as it is reserved");
				break;
			case ZEND_FETCH_CLASS_STATIC:
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot use 'static' as class name as it is reserved");
				break;
			default:
				break;
		}
		doing_inheritance = 1;
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->op1_type = IS_CONST;
	build_runtime_defined_function_key(&key, lcname, new_class_entry->name_length TSRMLS_CC);
	opline->op1.constant = zend_add_literal(CG(active_op_array), &key TSRMLS_CC);
	Z_HASH_P(&CONSTANT(opline->op1.constant)) = zend_hash_func(Z_STRVAL(CONSTANT(opline->op1.constant)), Z_STRLEN(CONSTANT(opline->op1.constant)));

	opline->op2_type = IS_CONST;

	if (doing_inheritance) {
		/* Make sure a trait does not try to extend a class */
		if ((new_class_entry->ce_flags & ZEND_ACC_TRAIT) == ZEND_ACC_TRAIT) {
			zend_error_noreturn(E_COMPILE_ERROR, "A trait (%s) cannot extend a class. Traits can only be composed from other traits with the 'use' keyword. Error", new_class_entry->name);
		}

		opline->extended_value = parent_class_name->u.op.var;
		opline->opcode = ZEND_DECLARE_INHERITED_CLASS;
	} else {
		opline->opcode = ZEND_DECLARE_CLASS;
	}

	LITERAL_STRINGL(opline->op2, lcname, new_class_entry->name_length, 0);
	CALCULATE_LITERAL_HASH(opline->op2.constant);

	zend_hash_quick_update(CG(class_table), Z_STRVAL(key), Z_STRLEN(key), Z_HASH_P(&CONSTANT(opline->op1.constant)), &new_class_entry, sizeof(zend_class_entry *), NULL);
	CG(active_class_entry) = new_class_entry;

	opline->result.var = get_temporary_variable(CG(active_op_array));
	opline->result_type = IS_VAR;
	GET_NODE(&CG(implementing_class), opline->result);

	if (CG(doc_comment)) {
		CG(active_class_entry)->info.user.doc_comment = CG(doc_comment);
		CG(active_class_entry)->info.user.doc_comment_len = CG(doc_comment_len);
		CG(doc_comment) = NULL;
		CG(doc_comment_len) = 0;
	}
}
/* }}} */

static void do_verify_abstract_class(TSRMLS_D) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_VERIFY_ABSTRACT_CLASS;
	SET_NODE(opline->op1, &CG(implementing_class));
	SET_UNUSED(opline->op2);
}
/* }}} */

void zend_do_end_class_declaration(const znode *class_token, const znode *parent_token TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce = CG(active_class_entry);

	if (ce->constructor) {
		ce->constructor->common.fn_flags |= ZEND_ACC_CTOR;
		if (ce->constructor->common.fn_flags & ZEND_ACC_STATIC) {
			zend_error_noreturn(E_COMPILE_ERROR, "Constructor %s::%s() cannot be static", ce->name, ce->constructor->common.function_name);
		}
	}
	if (ce->destructor) {
		ce->destructor->common.fn_flags |= ZEND_ACC_DTOR;
		if (ce->destructor->common.fn_flags & ZEND_ACC_STATIC) {
			zend_error_noreturn(E_COMPILE_ERROR, "Destructor %s::%s() cannot be static", ce->name, ce->destructor->common.function_name);
		}
	}
	if (ce->clone) {
		ce->clone->common.fn_flags |= ZEND_ACC_CLONE;
		if (ce->clone->common.fn_flags & ZEND_ACC_STATIC) {
			zend_error_noreturn(E_COMPILE_ERROR, "Clone method %s::%s() cannot be static", ce->name, ce->clone->common.function_name);
		}
	}

	ce->info.user.line_end = zend_get_compiled_lineno(TSRMLS_C);

	/* Check for traits and proceed like with interfaces.
	 * The only difference will be a combined handling of them in the end.
	 * Thus, we need another opcode here. */
	if (ce->num_traits > 0) {
		zend_op *opline;

		ce->traits = NULL;
		ce->num_traits = 0;
		ce->ce_flags |= ZEND_ACC_IMPLEMENT_TRAITS;

		/* opcode generation: */
		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = ZEND_BIND_TRAITS;
		SET_NODE(opline->op1, &CG(implementing_class));
	}

	if (!(ce->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS))
		&& (parent_token || (ce->num_interfaces > 0))) {
		zend_verify_abstract_class(ce TSRMLS_CC);
		if (ce->num_interfaces && !(ce->ce_flags & ZEND_ACC_IMPLEMENT_TRAITS)) {
			do_verify_abstract_class(TSRMLS_C);
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

void zend_do_implements_interface(znode *interface_name TSRMLS_DC) /* {{{ */
{
	zend_op *opline;

	/* Traits can not implement interfaces */
	if ((CG(active_class_entry)->ce_flags & ZEND_ACC_TRAIT) == ZEND_ACC_TRAIT) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use '%s' as interface on '%s' since it is a Trait",
							 Z_STRVAL(interface_name->u.constant),
							 CG(active_class_entry)->name);
	}

	switch (zend_get_class_fetch_type(Z_STRVAL(interface_name->u.constant), Z_STRLEN(interface_name->u.constant))) {
		case ZEND_FETCH_CLASS_SELF:
		case ZEND_FETCH_CLASS_PARENT:
		case ZEND_FETCH_CLASS_STATIC:
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use '%s' as interface name as it is reserved", Z_STRVAL(interface_name->u.constant));
			break;
		default:
			break;
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_ADD_INTERFACE;
	SET_NODE(opline->op1, &CG(implementing_class));
	zend_resolve_class_name(interface_name TSRMLS_CC);
	opline->extended_value = (opline->extended_value & ~ZEND_FETCH_CLASS_MASK) | ZEND_FETCH_CLASS_INTERFACE;
	opline->op2_type = IS_CONST;
	opline->op2.constant = zend_add_class_name_literal(CG(active_op_array), &interface_name->u.constant TSRMLS_CC);
	CG(active_class_entry)->num_interfaces++;
}
/* }}} */

void zend_do_use_trait(znode *trait_name TSRMLS_DC) /* {{{ */
{
	zend_op *opline;

	if ((CG(active_class_entry)->ce_flags & ZEND_ACC_INTERFACE)) {
		zend_error_noreturn(E_COMPILE_ERROR,
				"Cannot use traits inside of interfaces. %s is used in %s",
				Z_STRVAL(trait_name->u.constant), CG(active_class_entry)->name);
	}


	switch (zend_get_class_fetch_type(Z_STRVAL(trait_name->u.constant), Z_STRLEN(trait_name->u.constant))) {
		case ZEND_FETCH_CLASS_SELF:
		case ZEND_FETCH_CLASS_PARENT:
		case ZEND_FETCH_CLASS_STATIC:
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use '%s' as trait name as it is reserved", Z_STRVAL(trait_name->u.constant));
			break;
		default:
			break;
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_ADD_TRAIT;
	SET_NODE(opline->op1, &CG(implementing_class));
	zend_resolve_class_name(trait_name TSRMLS_CC);
	opline->extended_value = ZEND_FETCH_CLASS_TRAIT;
	opline->op2_type = IS_CONST;
	opline->op2.constant = zend_add_class_name_literal(CG(active_op_array), &trait_name->u.constant TSRMLS_CC);
	CG(active_class_entry)->num_traits++;
}
/* }}} */

ZEND_API void zend_mangle_property_name(char **dest, int *dest_length, const char *src1, int src1_length, const char *src2, int src2_length, int internal) /* {{{ */
{
	char *prop_name;
	int prop_name_length;

	prop_name_length = 1 + src1_length + 1 + src2_length;
	prop_name = pemalloc(prop_name_length + 1, internal);
	prop_name[0] = '\0';
	memcpy(prop_name + 1, src1, src1_length+1);
	memcpy(prop_name + 1 + src1_length + 1, src2, src2_length+1);

	*dest = prop_name;
	*dest_length = prop_name_length;
}
/* }}} */

static int zend_strnlen(const char* s, int maxlen) /* {{{ */
{
	int len = 0;
	while (*s++ && maxlen--) len++;
	return len;
}
/* }}} */

ZEND_API int zend_unmangle_property_name_ex(const char *mangled_property, int len, const char **class_name, const char **prop_name, int *prop_len) /* {{{ */
{
	int class_name_len;

	*class_name = NULL;

	if (!len || mangled_property[0] != 0) {
		*prop_name = mangled_property;
		if (prop_len) {
			*prop_len = len;
		}
		return SUCCESS;
	}
	if (len < 3 || mangled_property[1]==0) {
		zend_error(E_NOTICE, "Illegal member variable name");
		*prop_name = mangled_property;
		if (prop_len) {
			*prop_len = len;
		}
		return FAILURE;
	}

	class_name_len = zend_strnlen(mangled_property + 1, --len - 1) + 1;
	if (class_name_len >= len || mangled_property[class_name_len]!=0) {
		zend_error(E_NOTICE, "Corrupt member variable name");
		*prop_name = mangled_property;
		if (prop_len) {
			*prop_len = len + 1;
		}
		return FAILURE;
	}
	*class_name = mangled_property + 1;
	*prop_name = (*class_name) + class_name_len;
	if (prop_len) {
		*prop_len = len - class_name_len;
	}
	return SUCCESS;
}
/* }}} */

void zend_do_declare_property(const znode *var_name, const znode *value, zend_uint access_type TSRMLS_DC) /* {{{ */
{
	zval *property;
	zend_property_info *existing_property_info;
	char *comment = NULL;
	int comment_len = 0;

	if (CG(active_class_entry)->ce_flags & ZEND_ACC_INTERFACE) {
		zend_error_noreturn(E_COMPILE_ERROR, "Interfaces may not include member variables");
	}

	if (access_type & ZEND_ACC_ABSTRACT) {
		zend_error_noreturn(E_COMPILE_ERROR, "Properties cannot be declared abstract");
	}

	if (access_type & ZEND_ACC_FINAL) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot declare property %s::$%s final, the final modifier is allowed only for methods and classes",
				   CG(active_class_entry)->name, Z_STRVAL(var_name->u.constant));
	}

	if (zend_hash_find(&CG(active_class_entry)->properties_info, Z_STRVAL(var_name->u.constant), Z_STRLEN(var_name->u.constant)+1, (void **) &existing_property_info)==SUCCESS) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare %s::$%s", CG(active_class_entry)->name, Z_STRVAL(var_name->u.constant));
	}
	ALLOC_ZVAL(property);

	if (value) {
		*property = value->u.constant;
	} else {
		INIT_PZVAL(property);
		Z_TYPE_P(property) = IS_NULL;
	}

	if (CG(doc_comment)) {
		comment = CG(doc_comment);
		comment_len = CG(doc_comment_len);
		CG(doc_comment) = NULL;
		CG(doc_comment_len) = 0;
	}

	zend_declare_property_ex(CG(active_class_entry), zend_new_interned_string(Z_STRVAL(var_name->u.constant), Z_STRLEN(var_name->u.constant) + 1, 0 TSRMLS_CC), Z_STRLEN(var_name->u.constant), property, access_type, comment, comment_len TSRMLS_CC);
	efree(Z_STRVAL(var_name->u.constant));
}
/* }}} */

void zend_do_declare_class_constant(znode *var_name, const znode *value TSRMLS_DC) /* {{{ */
{
	zval *property;
	const char *cname = NULL;
	zend_ulong hash;

	if ((CG(active_class_entry)->ce_flags & ZEND_ACC_TRAIT) == ZEND_ACC_TRAIT) {
		zend_error_noreturn(E_COMPILE_ERROR, "Traits cannot have constants");
		return;
	}

	ALLOC_ZVAL(property);
	*property = value->u.constant;

	cname = zend_new_interned_string(Z_STRVAL(var_name->u.constant), Z_STRLEN(var_name->u.constant)+1, 0 TSRMLS_CC);
	hash = str_hash(cname, Z_STRLEN(var_name->u.constant));
	if (zend_hash_quick_add(&CG(active_class_entry)->constants_table, cname, Z_STRLEN(var_name->u.constant)+1, hash, &property, sizeof(zval *), NULL) == FAILURE) {
		FREE_ZVAL(property);
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot redefine class constant %s::%s", CG(active_class_entry)->name, Z_STRVAL(var_name->u.constant));
	}
	FREE_PNODE(var_name);

	if (CG(doc_comment)) {
		efree(CG(doc_comment));
		CG(doc_comment) = NULL;
		CG(doc_comment_len) = 0;
	}
}
/* }}} */

void zend_do_fetch_property(znode *result, znode *object, const znode *property TSRMLS_DC) /* {{{ */
{
	zend_op opline;
	zend_llist *fetch_list_ptr;

	zend_stack_top(&CG(bp_stack), (void **) &fetch_list_ptr);

	if (object->op_type == IS_CV) {
		if (object->u.op.var == CG(active_op_array)->this_var) {
			object->op_type = IS_UNUSED; /* this means $this for objects */
		}
	} else if (fetch_list_ptr->count == 1) {
		zend_llist_element *le = fetch_list_ptr->head;
		zend_op *opline_ptr = (zend_op *) le->data;

		if (opline_is_fetch_this(opline_ptr TSRMLS_CC)) {
			zend_del_literal(CG(active_op_array), opline_ptr->op1.constant);
			SET_UNUSED(opline_ptr->op1); /* this means $this for objects */
			SET_NODE(opline_ptr->op2, property);
			/* if it was usual fetch, we change it to object fetch */
			switch (opline_ptr->opcode) {
				case ZEND_FETCH_W:
					opline_ptr->opcode = ZEND_FETCH_OBJ_W;
					break;
				case ZEND_FETCH_R:
					opline_ptr->opcode = ZEND_FETCH_OBJ_R;
					break;
				case ZEND_FETCH_RW:
					opline_ptr->opcode = ZEND_FETCH_OBJ_RW;
					break;
				case ZEND_FETCH_IS:
					opline_ptr->opcode = ZEND_FETCH_OBJ_IS;
					break;
				case ZEND_FETCH_UNSET:
					opline_ptr->opcode = ZEND_FETCH_OBJ_UNSET;
					break;
				case ZEND_FETCH_FUNC_ARG:
					opline_ptr->opcode = ZEND_FETCH_OBJ_FUNC_ARG;
					break;
			}
			if (opline_ptr->op2_type == IS_CONST && Z_TYPE(CONSTANT(opline_ptr->op2.constant)) == IS_STRING) {
				CALCULATE_LITERAL_HASH(opline_ptr->op2.constant);
				GET_POLYMORPHIC_CACHE_SLOT(opline_ptr->op2.constant);
			}
			GET_NODE(result, opline_ptr->result);
			return;
		}
	}

	if (zend_is_function_or_method_call(object)) {
		init_op(&opline TSRMLS_CC);
		opline.opcode = ZEND_SEPARATE;
		SET_NODE(opline.op1, object);
		SET_UNUSED(opline.op2);
		opline.result_type = IS_VAR;
		opline.result.var = opline.op1.var;
		zend_llist_add_element(fetch_list_ptr, &opline);
	}

	init_op(&opline TSRMLS_CC);
	opline.opcode = ZEND_FETCH_OBJ_W;	/* the backpatching routine assumes W */
	opline.result_type = IS_VAR;
	opline.result.var = get_temporary_variable(CG(active_op_array));
	SET_NODE(opline.op1, object);
	SET_NODE(opline.op2, property);
	if (opline.op2_type == IS_CONST && Z_TYPE(CONSTANT(opline.op2.constant)) == IS_STRING) {
		CALCULATE_LITERAL_HASH(opline.op2.constant);
		GET_POLYMORPHIC_CACHE_SLOT(opline.op2.constant);
	}
	GET_NODE(result, opline.result);

	zend_llist_add_element(fetch_list_ptr, &opline);
}
/* }}} */

void zend_do_halt_compiler_register(TSRMLS_D) /* {{{ */
{
	char *name, *cfilename;
	char haltoff[] = "__COMPILER_HALT_OFFSET__";
	int len, clen;

	if (CG(has_bracketed_namespaces) && CG(in_namespace)) {
		zend_error_noreturn(E_COMPILE_ERROR, "__HALT_COMPILER() can only be used from the outermost scope");
	}

	cfilename = zend_get_compiled_filename(TSRMLS_C);
	clen = strlen(cfilename);
	zend_mangle_property_name(&name, &len, haltoff, sizeof(haltoff) - 1, cfilename, clen, 0);
	zend_register_long_constant(name, len+1, zend_get_scanned_file_offset(TSRMLS_C), CONST_CS, 0 TSRMLS_CC);
	pefree(name, 0);

	if (CG(in_namespace)) {
		zend_do_end_namespace(TSRMLS_C);
	}
}
/* }}} */

void zend_do_push_object(const znode *object TSRMLS_DC) /* {{{ */
{
	zend_stack_push(&CG(object_stack), object, sizeof(znode));
}
/* }}} */

void zend_do_pop_object(znode *object TSRMLS_DC) /* {{{ */
{
	if (object) {
		znode *tmp;

		zend_stack_top(&CG(object_stack), (void **) &tmp);
		*object = *tmp;
	}
	zend_stack_del_top(&CG(object_stack));
}
/* }}} */

void zend_do_begin_new_object(znode *new_token, znode *class_type TSRMLS_DC) /* {{{ */
{
	zend_op *opline;

	new_token->u.op.opline_num = get_next_op_number(CG(active_op_array));
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_NEW;
	opline->extended_value = CG(context).nested_calls;
	opline->result_type = IS_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	SET_NODE(opline->op1, class_type);
	SET_UNUSED(opline->op2);

	zend_push_function_call_entry(NULL TSRMLS_CC);
	if (++CG(context).nested_calls > CG(active_op_array)->nested_calls) {
		CG(active_op_array)->nested_calls = CG(context).nested_calls;
	}
}
/* }}} */

void zend_do_end_new_object(znode *result, const znode *new_token TSRMLS_DC) /* {{{ */
{
	znode ctor_result;

	zend_do_end_function_call(NULL, &ctor_result, 1, 0 TSRMLS_CC);
	zend_do_free(&ctor_result TSRMLS_CC);

	CG(active_op_array)->opcodes[new_token->u.op.opline_num].op2.opline_num = get_next_op_number(CG(active_op_array));
	GET_NODE(result, CG(active_op_array)->opcodes[new_token->u.op.opline_num].result);
}
/* }}} */

static zend_constant* zend_get_ct_const(const zval *const_name, int all_internal_constants_substitution TSRMLS_DC) /* {{{ */
{
	zend_constant *c = NULL;

	if (Z_STRVAL_P(const_name)[0] == '\\') {
		if (zend_hash_find(EG(zend_constants), Z_STRVAL_P(const_name)+1, Z_STRLEN_P(const_name), (void **) &c) == FAILURE) {
			char *lookup_name = zend_str_tolower_dup(Z_STRVAL_P(const_name)+1, Z_STRLEN_P(const_name)-1);

			if (zend_hash_find(EG(zend_constants), lookup_name, Z_STRLEN_P(const_name), (void **) &c)==SUCCESS) {
				if ((c->flags & CONST_CT_SUBST) && !(c->flags & CONST_CS)) {
					efree(lookup_name);
					return c;
				}
			}
			efree(lookup_name);
			return NULL;
		}
	} else if (zend_hash_find(EG(zend_constants), Z_STRVAL_P(const_name), Z_STRLEN_P(const_name)+1, (void **) &c) == FAILURE) {
		char *lookup_name = zend_str_tolower_dup(Z_STRVAL_P(const_name), Z_STRLEN_P(const_name));

		if (zend_hash_find(EG(zend_constants), lookup_name, Z_STRLEN_P(const_name)+1, (void **) &c)==SUCCESS) {
			if ((c->flags & CONST_CT_SUBST) && !(c->flags & CONST_CS)) {
				efree(lookup_name);
				return c;
			}
		}
		efree(lookup_name);
		return NULL;
	}
	if (c->flags & CONST_CT_SUBST) {
		return c;
	}
	if (all_internal_constants_substitution &&
	    (c->flags & CONST_PERSISTENT) &&
	    !(CG(compiler_options) & ZEND_COMPILE_NO_CONSTANT_SUBSTITUTION) &&
	    !IS_CONSTANT_TYPE(Z_TYPE(c->value))) {
		return c;
	}
	return NULL;
}
/* }}} */

static int zend_constant_ct_subst(znode *result, zval *const_name, int all_internal_constants_substitution TSRMLS_DC) /* {{{ */
{
	zend_constant *c = zend_get_ct_const(const_name, all_internal_constants_substitution TSRMLS_CC);

	if (c) {
		zval_dtor(const_name);
		result->op_type = IS_CONST;
		result->u.constant = c->value;
		zval_copy_ctor(&result->u.constant);
		INIT_PZVAL(&result->u.constant);
		return 1;
	}
	return 0;
}
/* }}} */

void zend_do_fetch_constant(znode *result, znode *constant_container, znode *constant_name, int mode, zend_bool check_namespace TSRMLS_DC) /* {{{ */
{
	znode tmp;
	zend_op *opline;
	int type;
	char *compound;
	ulong fetch_type = 0;

	if (constant_container) {
		switch (mode) {
			case ZEND_CT:
				/* this is a class constant */
				type = zend_get_class_fetch_type(Z_STRVAL(constant_container->u.constant), Z_STRLEN(constant_container->u.constant));

				if (ZEND_FETCH_CLASS_STATIC == type) {
					zend_error(E_ERROR, "\"static::\" is not allowed in compile-time constants");
				} else if (ZEND_FETCH_CLASS_DEFAULT == type) {
					zend_resolve_class_name(constant_container TSRMLS_CC);
				}
				zend_do_build_full_name(NULL, constant_container, constant_name, 1 TSRMLS_CC);
				*result = *constant_container;
				result->u.constant.type = IS_CONSTANT | fetch_type;
				break;
			case ZEND_RT:
				if (constant_container->op_type == IS_CONST &&
				ZEND_FETCH_CLASS_DEFAULT == zend_get_class_fetch_type(Z_STRVAL(constant_container->u.constant), Z_STRLEN(constant_container->u.constant))) {
					zend_resolve_class_name(constant_container TSRMLS_CC);
				} else {
					zend_do_fetch_class(&tmp, constant_container TSRMLS_CC);
					constant_container = &tmp;
				}
				opline = get_next_op(CG(active_op_array) TSRMLS_CC);
				opline->opcode = ZEND_FETCH_CONSTANT;
				opline->result_type = IS_TMP_VAR;
				opline->result.var = get_temporary_variable(CG(active_op_array));
				if (constant_container->op_type == IS_CONST) {
					opline->op1_type = IS_CONST;
					opline->op1.constant = zend_add_class_name_literal(CG(active_op_array), &constant_container->u.constant TSRMLS_CC);
				} else {
					SET_NODE(opline->op1, constant_container);
				}
				SET_NODE(opline->op2, constant_name);
				CALCULATE_LITERAL_HASH(opline->op2.constant);
				if (opline->op1_type == IS_CONST) {
					GET_CACHE_SLOT(opline->op2.constant);
				} else {
					GET_POLYMORPHIC_CACHE_SLOT(opline->op2.constant);
				}
				GET_NODE(result, opline->result);
				break;
		}
		return;
	}
	/* namespace constant */
	/* only one that did not contain \ from the start can be converted to string if unknown */
	switch (mode) {
		case ZEND_CT:
			compound = memchr(Z_STRVAL(constant_name->u.constant), '\\', Z_STRLEN(constant_name->u.constant));
			/* this is a namespace constant, or an unprefixed constant */

			if (zend_constant_ct_subst(result, &constant_name->u.constant, 0 TSRMLS_CC)) {
				break;
			}

			zend_resolve_const_name(constant_name, &check_namespace TSRMLS_CC);

			if(!compound) {
				fetch_type |= IS_CONSTANT_UNQUALIFIED;
			}

			*result = *constant_name;
			result->u.constant.type = IS_CONSTANT | fetch_type;
			break;
		case ZEND_RT:
			compound = memchr(Z_STRVAL(constant_name->u.constant), '\\', Z_STRLEN(constant_name->u.constant));

			zend_resolve_const_name(constant_name, &check_namespace TSRMLS_CC);

			if(zend_constant_ct_subst(result, &constant_name->u.constant, 1 TSRMLS_CC)) {
				break;
			}

			opline = get_next_op(CG(active_op_array) TSRMLS_CC);
			opline->opcode = ZEND_FETCH_CONSTANT;
			opline->result_type = IS_TMP_VAR;
			opline->result.var = get_temporary_variable(CG(active_op_array));
			GET_NODE(result, opline->result);
			SET_UNUSED(opline->op1);
			opline->op2_type = IS_CONST;
			if (compound) {
				/* the name is unambiguous */
				opline->extended_value = 0;
				opline->op2.constant = zend_add_const_name_literal(CG(active_op_array), &constant_name->u.constant, 0 TSRMLS_CC);
			} else {
				opline->extended_value = IS_CONSTANT_UNQUALIFIED;
				if (check_namespace && CG(current_namespace)) {
					opline->extended_value |= IS_CONSTANT_IN_NAMESPACE;
					opline->op2.constant = zend_add_const_name_literal(CG(active_op_array), &constant_name->u.constant, 1 TSRMLS_CC);
				} else {
					opline->op2.constant = zend_add_const_name_literal(CG(active_op_array), &constant_name->u.constant, 0 TSRMLS_CC);
				}
			}
			GET_CACHE_SLOT(opline->op2.constant);
			break;
	}
}
/* }}} */

void zend_do_shell_exec(znode *result, const znode *cmd TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	switch (cmd->op_type) {
		case IS_CONST:
		case IS_TMP_VAR:
			opline->opcode = ZEND_SEND_VAL;
			break;
		default:
			opline->opcode = ZEND_SEND_VAR;
			break;
	}
	SET_NODE(opline->op1, cmd);
	opline->op2.opline_num = 1;
	opline->extended_value = ZEND_DO_FCALL;
	SET_UNUSED(opline->op2);

	/* FIXME: exception support not added to this op2 */
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_DO_FCALL;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	opline->result_type = IS_VAR;
	LITERAL_STRINGL(opline->op1, estrndup("shell_exec", sizeof("shell_exec")-1), sizeof("shell_exec")-1, 0);
	CALCULATE_LITERAL_HASH(opline->op1.constant);
	opline->op1_type = IS_CONST;
	GET_CACHE_SLOT(opline->op1.constant);
	opline->extended_value = 1;
	SET_UNUSED(opline->op2);
	opline->op2.num = CG(context).nested_calls;
	GET_NODE(result, opline->result);

	if (CG(context).nested_calls + 1 > CG(active_op_array)->nested_calls) {
		CG(active_op_array)->nested_calls = CG(context).nested_calls + 1;
	}
	if (CG(context).used_stack + 2 > CG(active_op_array)->used_stack) {
		CG(active_op_array)->used_stack = CG(context).used_stack + 2;
	}
}
/* }}} */

void zend_do_init_array(znode *result, const znode *expr, const znode *offset, zend_bool is_ref TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_INIT_ARRAY;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	opline->result_type = IS_TMP_VAR;
	GET_NODE(result, opline->result);
	if (expr) {
		SET_NODE(opline->op1, expr);
		if (offset) {
			SET_NODE(opline->op2, offset);
			if (opline->op2_type == IS_CONST && Z_TYPE(CONSTANT(opline->op2.constant)) == IS_STRING) {
				ulong index;
				int numeric = 0;

				ZEND_HANDLE_NUMERIC_EX(Z_STRVAL(CONSTANT(opline->op2.constant)), Z_STRLEN(CONSTANT(opline->op2.constant))+1, index, numeric = 1);
				if (numeric) {
					zval_dtor(&CONSTANT(opline->op2.constant));
					ZVAL_LONG(&CONSTANT(opline->op2.constant), index);
				} else {
					CALCULATE_LITERAL_HASH(opline->op2.constant);
				}
			}
		} else {
			SET_UNUSED(opline->op2);
		}
	} else {
		SET_UNUSED(opline->op1);
		SET_UNUSED(opline->op2);
	}
	opline->extended_value = is_ref;
}
/* }}} */

void zend_do_add_array_element(znode *result, const znode *expr, const znode *offset, zend_bool is_ref TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_ADD_ARRAY_ELEMENT;
	SET_NODE(opline->result, result);
	SET_NODE(opline->op1, expr);
	if (offset) {
		SET_NODE(opline->op2, offset);
		if (opline->op2_type == IS_CONST && Z_TYPE(CONSTANT(opline->op2.constant)) == IS_STRING) {
			ulong index;
			int numeric = 0;

			ZEND_HANDLE_NUMERIC_EX(Z_STRVAL(CONSTANT(opline->op2.constant)), Z_STRLEN(CONSTANT(opline->op2.constant))+1, index, numeric = 1);
			if (numeric) {
				zval_dtor(&CONSTANT(opline->op2.constant));
				ZVAL_LONG(&CONSTANT(opline->op2.constant), index);
			} else {
				CALCULATE_LITERAL_HASH(opline->op2.constant);
			}
		}
	} else {
		SET_UNUSED(opline->op2);
	}
	opline->extended_value = is_ref;
}
/* }}} */

void zend_do_add_static_array_element(zval *result, zval *offset, zval *expr) /* {{{ */
{
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(result), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr, sizeof(zval *), NULL);
				zval_dtor(offset);
				break;
			case IS_NULL:
				zend_symtable_update(Z_ARRVAL_P(result), "", 1, &expr, sizeof(zval *), NULL);
				break;
			case IS_RESOURCE:
				zend_error(E_STRICT, "Resource ID#%ld used as offset, casting to integer (%ld)", Z_LVAL_P(offset), Z_LVAL_P(offset));
				/* break missing intentionally */
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(result), Z_LVAL_P(offset), &expr, sizeof(zval *), NULL);
				break;
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(result), zend_dval_to_lval(Z_DVAL_P(offset)), &expr, sizeof(zval *), NULL);
				break;
			case IS_ARRAY:
				zend_error(E_ERROR, "Illegal offset type");
				break;
		}
	} else {
		if (zend_hash_next_index_insert(Z_ARRVAL_P(result), &expr, sizeof(zval *), NULL) == FAILURE) {
			zend_error(E_WARNING, "Cannot add element to the array as the next element is already occupied");
			zval_ptr_dtor(&expr);
		}
	}
}
/* }}} */

void zend_do_add_list_element(const znode *element TSRMLS_DC) /* {{{ */
{
	list_llist_element lle;

	if (element) {
		zend_check_writable_variable(element);

		lle.var = *element;
		zend_llist_copy(&lle.dimensions, &CG(dimension_llist));
		zend_llist_prepend_element(&CG(list_llist), &lle);
	}
	(*((int *)CG(dimension_llist).tail->data))++;
}
/* }}} */

void zend_do_new_list_begin(TSRMLS_D) /* {{{ */
{
	int current_dimension = 0;
	zend_llist_add_element(&CG(dimension_llist), &current_dimension);
}
/* }}} */

void zend_do_new_list_end(TSRMLS_D) /* {{{ */
{
	zend_llist_remove_tail(&CG(dimension_llist));
	(*((int *)CG(dimension_llist).tail->data))++;
}
/* }}} */

void zend_do_list_init(TSRMLS_D) /* {{{ */
{
	zend_stack_push(&CG(list_stack), &CG(list_llist), sizeof(zend_llist));
	zend_stack_push(&CG(list_stack), &CG(dimension_llist), sizeof(zend_llist));
	zend_llist_init(&CG(list_llist), sizeof(list_llist_element), NULL, 0);
	zend_llist_init(&CG(dimension_llist), sizeof(int), NULL, 0);
	zend_do_new_list_begin(TSRMLS_C);
}
/* }}} */

void zend_do_list_end(znode *result, znode *expr TSRMLS_DC) /* {{{ */
{
	zend_llist_element *le;
	zend_llist_element *dimension;
	zend_op *opline;
	znode last_container;

	le = CG(list_llist).head;
	while (le) {
		zend_llist *tmp_dimension_llist = &((list_llist_element *)le->data)->dimensions;
		dimension = tmp_dimension_llist->head;
		while (dimension) {
			opline = get_next_op(CG(active_op_array) TSRMLS_CC);
			if (dimension == tmp_dimension_llist->head) { /* first */
				last_container = *expr;
				switch (expr->op_type) {
					case IS_VAR:
					case IS_CV:
						opline->opcode = ZEND_FETCH_DIM_R;
						break;
					case IS_TMP_VAR:
						opline->opcode = ZEND_FETCH_DIM_TMP_VAR;
						break;
					case IS_CONST: /* fetch_dim_tmp_var will handle this bogus fetch */
						zval_copy_ctor(&expr->u.constant);
						opline->opcode = ZEND_FETCH_DIM_TMP_VAR;
						break;
				}
				opline->extended_value |= ZEND_FETCH_ADD_LOCK;
			} else {
				opline->opcode = ZEND_FETCH_DIM_R;
			}
			opline->result_type = IS_VAR;
			opline->result.var = get_temporary_variable(CG(active_op_array));
			SET_NODE(opline->op1, &last_container);
			opline->op2_type = IS_CONST;
			LITERAL_LONG(opline->op2, *((int *) dimension->data));
			GET_NODE(&last_container, opline->result);
			dimension = dimension->next;
		}
		((list_llist_element *) le->data)->value = last_container;
		zend_llist_destroy(&((list_llist_element *) le->data)->dimensions);
		zend_do_assign(result, &((list_llist_element *) le->data)->var, &((list_llist_element *) le->data)->value TSRMLS_CC);
		zend_do_free(result TSRMLS_CC);
		le = le->next;
	}
	zend_llist_destroy(&CG(dimension_llist));
	zend_llist_destroy(&CG(list_llist));
	*result = *expr;
	{
		zend_llist *p;

		/* restore previous lists */
		zend_stack_top(&CG(list_stack), (void **) &p);
		CG(dimension_llist) = *p;
		zend_stack_del_top(&CG(list_stack));
		zend_stack_top(&CG(list_stack), (void **) &p);
		CG(list_llist) = *p;
		zend_stack_del_top(&CG(list_stack));
	}
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

void zend_do_fetch_static_variable(znode *varname, const znode *static_assignment, int fetch_type TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zend_op *opline;
	znode lval;
	znode result;

	ALLOC_ZVAL(tmp);

	if (static_assignment) {
		*tmp = static_assignment->u.constant;
	} else {
		INIT_ZVAL(*tmp);
	}
	if (!CG(active_op_array)->static_variables) {
		if (CG(active_op_array)->scope) {
			CG(active_op_array)->scope->ce_flags |= ZEND_HAS_STATIC_IN_METHODS;
		}
		ALLOC_HASHTABLE(CG(active_op_array)->static_variables);
		zend_hash_init(CG(active_op_array)->static_variables, 2, NULL, ZVAL_PTR_DTOR, 0);
	}
	zend_hash_update(CG(active_op_array)->static_variables, Z_STRVAL(varname->u.constant), Z_STRLEN(varname->u.constant)+1, &tmp, sizeof(zval *), NULL);

	if (varname->op_type == IS_CONST) {
		if (Z_TYPE(varname->u.constant) != IS_STRING) {
			convert_to_string(&varname->u.constant);
		}
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = (fetch_type == ZEND_FETCH_LEXICAL) ? ZEND_FETCH_R : ZEND_FETCH_W;		/* the default mode must be Write, since fetch_simple_variable() is used to define function arguments */
	opline->result_type = IS_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	SET_NODE(opline->op1, varname);
	if (opline->op1_type == IS_CONST) {
		CALCULATE_LITERAL_HASH(opline->op1.constant);
	}
	SET_UNUSED(opline->op2);
	opline->extended_value = ZEND_FETCH_STATIC;
	GET_NODE(&result, opline->result);

	if (varname->op_type == IS_CONST) {
		zval_copy_ctor(&varname->u.constant);
	}
	fetch_simple_variable(&lval, varname, 0 TSRMLS_CC); /* Relies on the fact that the default fetch is BP_VAR_W */

	if (fetch_type == ZEND_FETCH_LEXICAL) {
		znode dummy;

		zend_do_begin_variable_parse(TSRMLS_C);
		zend_do_assign(&dummy, &lval, &result TSRMLS_CC);
		zend_do_free(&dummy TSRMLS_CC);
	} else {
		zend_do_assign_ref(NULL, &lval, &result TSRMLS_CC);
	}
	CG(active_op_array)->opcodes[CG(active_op_array)->last-1].result_type |= EXT_TYPE_UNUSED;
}
/* }}} */

void zend_do_fetch_lexical_variable(znode *varname, zend_bool is_ref TSRMLS_DC) /* {{{ */
{
	znode value;

	if (Z_STRLEN(varname->u.constant) == sizeof("this") - 1 &&
	    memcmp(Z_STRVAL(varname->u.constant), "this", sizeof("this") - 1) == 0) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use $this as lexical variable");
		return;
	}

	value.op_type = IS_CONST;
	ZVAL_NULL(&value.u.constant);
	Z_TYPE(value.u.constant) |= is_ref ? IS_LEXICAL_REF : IS_LEXICAL_VAR;
	Z_SET_REFCOUNT_P(&value.u.constant, 1);
	Z_UNSET_ISREF_P(&value.u.constant);

	zend_do_fetch_static_variable(varname, &value, is_ref ? ZEND_FETCH_STATIC : ZEND_FETCH_LEXICAL TSRMLS_CC);
}
/* }}} */

void zend_do_fetch_global_variable(znode *varname, const znode *static_assignment, int fetch_type TSRMLS_DC) /* {{{ */
{
	zend_op *opline;
	znode lval;
	znode result;

	if (varname->op_type == IS_CONST) {
		if (Z_TYPE(varname->u.constant) != IS_STRING) {
			convert_to_string(&varname->u.constant);
		}
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_FETCH_W;		/* the default mode must be Write, since fetch_simple_variable() is used to define function arguments */
	opline->result_type = IS_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	SET_NODE(opline->op1, varname);
	if (opline->op1_type == IS_CONST) {
		CALCULATE_LITERAL_HASH(opline->op1.constant);
	}
	SET_UNUSED(opline->op2);
	opline->extended_value = fetch_type;
	GET_NODE(&result, opline->result);

	if (varname->op_type == IS_CONST) {
		zval_copy_ctor(&varname->u.constant);
	}
	fetch_simple_variable(&lval, varname, 0 TSRMLS_CC); /* Relies on the fact that the default fetch is BP_VAR_W */

	zend_do_assign_ref(NULL, &lval, &result TSRMLS_CC);
	CG(active_op_array)->opcodes[CG(active_op_array)->last-1].result_type |= EXT_TYPE_UNUSED;
}
/* }}} */

void zend_do_cast(znode *result, const znode *expr, int type TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_CAST;
	opline->result_type = IS_TMP_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	SET_NODE(opline->op1, expr);
	SET_UNUSED(opline->op2);
	opline->extended_value = type;
	GET_NODE(result, opline->result);
}
/* }}} */

void zend_do_include_or_eval(int type, znode *result, const znode *op1 TSRMLS_DC) /* {{{ */
{
	zend_do_extended_fcall_begin(TSRMLS_C);
	{
		zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

		opline->opcode = ZEND_INCLUDE_OR_EVAL;
		opline->result_type = IS_VAR;
		opline->result.var = get_temporary_variable(CG(active_op_array));
		SET_NODE(opline->op1, op1);
		SET_UNUSED(opline->op2);
		opline->extended_value = type;
		GET_NODE(result, opline->result);
	}
	zend_do_extended_fcall_end(TSRMLS_C);
}
/* }}} */

void zend_do_indirect_references(znode *result, const znode *num_references, znode *variable TSRMLS_DC) /* {{{ */
{
	int i;

	zend_do_end_variable_parse(variable, BP_VAR_R, 0 TSRMLS_CC);
	for (i=1; i<Z_LVAL(num_references->u.constant); i++) {
		fetch_simple_variable_ex(result, variable, 0, ZEND_FETCH_R TSRMLS_CC);
		*variable = *result;
	}
	zend_do_begin_variable_parse(TSRMLS_C);
	fetch_simple_variable(result, variable, 1 TSRMLS_CC);
	/* there is a chance someone is accessing $this */
	if (CG(active_op_array)->scope && CG(active_op_array)->this_var == -1) {
		CG(active_op_array)->this_var = lookup_cv(CG(active_op_array), estrndup("this", sizeof("this")-1), sizeof("this")-1, THIS_HASHVAL TSRMLS_CC);
	}
}
/* }}} */

void zend_do_unset(const znode *variable TSRMLS_DC) /* {{{ */
{
	zend_op *last_op;

	zend_check_writable_variable(variable);

	if (variable->op_type == IS_CV) {
		zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = ZEND_UNSET_VAR;
		SET_NODE(opline->op1, variable);
		SET_UNUSED(opline->op2);
		SET_UNUSED(opline->result);
		opline->extended_value = ZEND_FETCH_LOCAL | ZEND_QUICK_SET;
	} else {
		last_op = &CG(active_op_array)->opcodes[get_next_op_number(CG(active_op_array))-1];

		switch (last_op->opcode) {
			case ZEND_FETCH_UNSET:
				last_op->opcode = ZEND_UNSET_VAR;
				SET_UNUSED(last_op->result);
				break;
			case ZEND_FETCH_DIM_UNSET:
				last_op->opcode = ZEND_UNSET_DIM;
				SET_UNUSED(last_op->result);
				break;
			case ZEND_FETCH_OBJ_UNSET:
				last_op->opcode = ZEND_UNSET_OBJ;
				SET_UNUSED(last_op->result);
				break;

		}
	}
}
/* }}} */

void zend_do_isset_or_isempty(int type, znode *result, znode *variable TSRMLS_DC) /* {{{ */
{
	zend_op *last_op;

	zend_do_end_variable_parse(variable, BP_VAR_IS, 0 TSRMLS_CC);

	if (zend_is_function_or_method_call(variable)) {
		if (type == ZEND_ISEMPTY) {
			/* empty(func()) can be transformed to !func() */
			zend_do_unary_op(ZEND_BOOL_NOT, result, variable TSRMLS_CC);
		} else {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use isset() on the result of a function call (you can use \"null !== func()\" instead)");
		}

		return;
	}

	if (variable->op_type == IS_CV) {
		last_op = get_next_op(CG(active_op_array) TSRMLS_CC);
		last_op->opcode = ZEND_ISSET_ISEMPTY_VAR;
		SET_NODE(last_op->op1, variable);
		SET_UNUSED(last_op->op2);
		last_op->result.var = get_temporary_variable(CG(active_op_array));
		last_op->extended_value = ZEND_FETCH_LOCAL | ZEND_QUICK_SET;
	} else {
		last_op = &CG(active_op_array)->opcodes[get_next_op_number(CG(active_op_array))-1];

		switch (last_op->opcode) {
			case ZEND_FETCH_IS:
				last_op->opcode = ZEND_ISSET_ISEMPTY_VAR;
				break;
			case ZEND_FETCH_DIM_IS:
				last_op->opcode = ZEND_ISSET_ISEMPTY_DIM_OBJ;
				break;
			case ZEND_FETCH_OBJ_IS:
				last_op->opcode = ZEND_ISSET_ISEMPTY_PROP_OBJ;
				break;
		}
	}
	last_op->result_type = IS_TMP_VAR;
	last_op->extended_value |= type;

	GET_NODE(result, last_op->result);
}
/* }}} */

void zend_do_instanceof(znode *result, const znode *expr, const znode *class_znode, int type TSRMLS_DC) /* {{{ */
{
	int last_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline;

	if (last_op_number > 0) {
		opline = &CG(active_op_array)->opcodes[last_op_number-1];
		if (opline->opcode == ZEND_FETCH_CLASS) {
			opline->extended_value |= ZEND_FETCH_CLASS_NO_AUTOLOAD;
		}
	}

	if (expr->op_type == IS_CONST) {
		zend_error_noreturn(E_COMPILE_ERROR, "instanceof expects an object instance, constant given");
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_INSTANCEOF;
	opline->result_type = IS_TMP_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	SET_NODE(opline->op1, expr);

	SET_NODE(opline->op2, class_znode);

	GET_NODE(result, opline->result);
}
/* }}} */

void zend_do_foreach_begin(znode *foreach_token, znode *open_brackets_token, znode *array, znode *as_token, int variable TSRMLS_DC) /* {{{ */
{
	zend_op *opline;
	zend_bool is_variable;
	zend_op dummy_opline;

	if (variable) {
		if (zend_is_function_or_method_call(array)) {
			is_variable = 0;
		} else {
			is_variable = 1;
		}
		/* save the location of FETCH_W instruction(s) */
		open_brackets_token->u.op.opline_num = get_next_op_number(CG(active_op_array));
		zend_do_end_variable_parse(array, BP_VAR_W, 0 TSRMLS_CC);

		if (zend_is_function_or_method_call(array)) {
			opline = get_next_op(CG(active_op_array) TSRMLS_CC);
			opline->opcode = ZEND_SEPARATE;
			SET_NODE(opline->op1, array);
			SET_UNUSED(opline->op2);
			opline->result_type = IS_VAR;
			opline->result.var = opline->op1.var;
		}
	} else {
		is_variable = 0;
		open_brackets_token->u.op.opline_num = get_next_op_number(CG(active_op_array));
	}

	/* save the location of FE_RESET */
	foreach_token->u.op.opline_num = get_next_op_number(CG(active_op_array));

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	/* Preform array reset */
	opline->opcode = ZEND_FE_RESET;
	opline->result_type = IS_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	SET_NODE(opline->op1, array);
	SET_UNUSED(opline->op2);
	opline->extended_value = is_variable ? ZEND_FE_RESET_VARIABLE : 0;

	COPY_NODE(dummy_opline.result, opline->result);
	zend_stack_push(&CG(foreach_copy_stack), (void *) &dummy_opline, sizeof(zend_op));

	/* save the location of FE_FETCH */
	as_token->u.op.opline_num = get_next_op_number(CG(active_op_array));

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_FE_FETCH;
	opline->result_type = IS_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	COPY_NODE(opline->op1, dummy_opline.result);
	opline->extended_value = 0;
	SET_UNUSED(opline->op2);

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_OP_DATA;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
	SET_UNUSED(opline->result);
}
/* }}} */

void zend_do_foreach_cont(znode *foreach_token, const znode *open_brackets_token, const znode *as_token, znode *value, znode *key TSRMLS_DC) /* {{{ */
{
	zend_op *opline;
	znode dummy, value_node;
	zend_bool assign_by_ref=0;

	opline = &CG(active_op_array)->opcodes[as_token->u.op.opline_num];
	if (key->op_type != IS_UNUSED) {
		znode *tmp;

		/* switch between the key and value... */
		tmp = key;
		key = value;
		value = tmp;

		/* Mark extended_value in case both key and value are being used */
		opline->extended_value |= ZEND_FE_FETCH_WITH_KEY;
	}

	if ((key->op_type != IS_UNUSED)) {
		if (key->EA & ZEND_PARSED_REFERENCE_VARIABLE) {
			zend_error_noreturn(E_COMPILE_ERROR, "Key element cannot be a reference");
		}
		if (key->EA & ZEND_PARSED_LIST_EXPR) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use list as key element");
		}
	}

	if (value->EA & ZEND_PARSED_REFERENCE_VARIABLE) {
		assign_by_ref = 1;

		/* Mark extended_value for assign-by-reference */
		opline->extended_value |= ZEND_FE_FETCH_BYREF;
		CG(active_op_array)->opcodes[foreach_token->u.op.opline_num].extended_value |= ZEND_FE_RESET_REFERENCE;
	} else {
		zend_op *fetch = &CG(active_op_array)->opcodes[foreach_token->u.op.opline_num];
		zend_op	*end = &CG(active_op_array)->opcodes[open_brackets_token->u.op.opline_num];

		/* Change "write context" into "read context" */
		fetch->extended_value = 0;  /* reset ZEND_FE_RESET_VARIABLE */
		while (fetch != end) {
			--fetch;
			if (fetch->opcode == ZEND_FETCH_DIM_W && fetch->op2_type == IS_UNUSED) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot use [] for reading");
			}
			if (fetch->opcode == ZEND_SEPARATE) {
				MAKE_NOP(fetch);
			} else {
				fetch->opcode -= 3; /* FETCH_W -> FETCH_R */
			}
		}
	}

	GET_NODE(&value_node, opline->result);

	if (value->EA & ZEND_PARSED_LIST_EXPR) {
		if (!CG(list_llist).head) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use empty list");
		}
		zend_do_list_end(&dummy, &value_node TSRMLS_CC);
		zend_do_free(&dummy TSRMLS_CC);
	} else {
		if (assign_by_ref) {
			zend_do_end_variable_parse(value, BP_VAR_W, 0 TSRMLS_CC);
			/* Mark FE_FETCH as IS_VAR as it holds the data directly as a value */
			zend_do_assign_ref(NULL, value, &value_node TSRMLS_CC);
		} else {
			zend_do_assign(&dummy, value, &value_node TSRMLS_CC);
			zend_do_free(&dummy TSRMLS_CC);
		}
	}

	if (key->op_type != IS_UNUSED) {
		znode key_node;

		opline = &CG(active_op_array)->opcodes[as_token->u.op.opline_num+1];
		opline->result_type = IS_TMP_VAR;
		opline->result.opline_num = get_temporary_variable(CG(active_op_array));
		GET_NODE(&key_node, opline->result);

		zend_do_assign(&dummy, key, &key_node TSRMLS_CC);
		zend_do_free(&dummy TSRMLS_CC);
	}

	do_begin_loop(TSRMLS_C);
	INC_BPC(CG(active_op_array));
}
/* }}} */

void zend_do_foreach_end(const znode *foreach_token, const znode *as_token TSRMLS_DC) /* {{{ */
{
	zend_op *container_ptr;
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMP;
	opline->op1.opline_num = as_token->u.op.opline_num;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);

	CG(active_op_array)->opcodes[foreach_token->u.op.opline_num].op2.opline_num = get_next_op_number(CG(active_op_array)); /* FE_RESET */
	CG(active_op_array)->opcodes[as_token->u.op.opline_num].op2.opline_num = get_next_op_number(CG(active_op_array)); /* FE_FETCH */

	do_end_loop(as_token->u.op.opline_num, 1 TSRMLS_CC);

	zend_stack_top(&CG(foreach_copy_stack), (void **) &container_ptr);
	generate_free_foreach_copy(container_ptr TSRMLS_CC);
	zend_stack_del_top(&CG(foreach_copy_stack));

	DEC_BPC(CG(active_op_array));
}
/* }}} */

void zend_do_declare_begin(TSRMLS_D) /* {{{ */
{
	zend_stack_push(&CG(declare_stack), &CG(declarables), sizeof(zend_declarables));
}
/* }}} */

void zend_do_declare_stmt(znode *var, znode *val TSRMLS_DC) /* {{{ */
{
	if (!zend_binary_strcasecmp(Z_STRVAL(var->u.constant), Z_STRLEN(var->u.constant), "ticks", sizeof("ticks")-1)) {
		convert_to_long(&val->u.constant);
		CG(declarables).ticks = val->u.constant;
	} else if (!zend_binary_strcasecmp(Z_STRVAL(var->u.constant), Z_STRLEN(var->u.constant), "encoding", sizeof("encoding")-1)) {
		if ((Z_TYPE(val->u.constant) & IS_CONSTANT_TYPE_MASK) == IS_CONSTANT) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use constants as encoding");
		}

		/*
		 * Check that the pragma comes before any opcodes. If the compilation
		 * got as far as this, the previous portion of the script must have been
		 * parseable according to the .ini script_encoding setting. We still
		 * want to tell them to put declare() at the top.
		 */
		{
			int num = CG(active_op_array)->last;
			/* ignore ZEND_EXT_STMT and ZEND_TICKS */
			while (num > 0 &&
			       (CG(active_op_array)->opcodes[num-1].opcode == ZEND_EXT_STMT ||
			        CG(active_op_array)->opcodes[num-1].opcode == ZEND_TICKS)) {
				--num;
			}

			if (num > 0) {
				zend_error_noreturn(E_COMPILE_ERROR, "Encoding declaration pragma must be the very first statement in the script");
			}
		}

		if (CG(multibyte)) {
			const zend_encoding *new_encoding, *old_encoding;
			zend_encoding_filter old_input_filter;

			CG(encoding_declared) = 1;

			convert_to_string(&val->u.constant);
			new_encoding = zend_multibyte_fetch_encoding(Z_STRVAL(val->u.constant) TSRMLS_CC);
			if (!new_encoding) {
				zend_error(E_COMPILE_WARNING, "Unsupported encoding [%s]", Z_STRVAL(val->u.constant));
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
		} else {
			zend_error(E_COMPILE_WARNING, "declare(encoding=...) ignored because Zend multibyte feature is turned off by settings");
		}
		zval_dtor(&val->u.constant);
	} else {
		zend_error(E_COMPILE_WARNING, "Unsupported declare '%s'", Z_STRVAL(var->u.constant));
		zval_dtor(&val->u.constant);
	}
	zval_dtor(&var->u.constant);
}
/* }}} */

void zend_do_declare_end(const znode *declare_token TSRMLS_DC) /* {{{ */
{
	zend_declarables *declarables;

	zend_stack_top(&CG(declare_stack), (void **) &declarables);
	/* We should restore if there was more than (current - start) - (ticks?1:0) opcodes */
	if ((get_next_op_number(CG(active_op_array)) - declare_token->u.op.opline_num) - ((Z_LVAL(CG(declarables).ticks))?1:0)) {
		CG(declarables) = *declarables;
	}
}
/* }}} */

void zend_do_exit(znode *result, const znode *message TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_EXIT;
	SET_NODE(opline->op1, message);
	SET_UNUSED(opline->op2);

	result->op_type = IS_CONST;
	Z_TYPE(result->u.constant) = IS_BOOL;
	Z_LVAL(result->u.constant) = 1;
}
/* }}} */

void zend_do_begin_silence(znode *strudel_token TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_BEGIN_SILENCE;
	opline->result_type = IS_TMP_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
	GET_NODE(strudel_token, opline->result);
}
/* }}} */

void zend_do_end_silence(const znode *strudel_token TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_END_SILENCE;
	SET_NODE(opline->op1, strudel_token);
	SET_UNUSED(opline->op2);
}
/* }}} */

void zend_do_jmp_set(const znode *value, znode *jmp_token, znode *colon_token TSRMLS_DC) /* {{{ */
{
	int op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	if (value->op_type == IS_VAR || value->op_type == IS_CV) {
		opline->opcode = ZEND_JMP_SET_VAR;
		opline->result_type = IS_VAR;
	} else {
		opline->opcode = ZEND_JMP_SET;
		opline->result_type = IS_TMP_VAR;
	}
	opline->result.var = get_temporary_variable(CG(active_op_array));
	SET_NODE(opline->op1, value);
	SET_UNUSED(opline->op2);

	GET_NODE(colon_token, opline->result);

	jmp_token->u.op.opline_num = op_number;

	INC_BPC(CG(active_op_array));
}
/* }}} */

void zend_do_jmp_set_else(znode *result, const znode *false_value, const znode *jmp_token, const znode *colon_token TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	SET_NODE(opline->result, colon_token);
	if (colon_token->op_type == IS_TMP_VAR) {
		if (false_value->op_type == IS_VAR || false_value->op_type == IS_CV) {
			CG(active_op_array)->opcodes[jmp_token->u.op.opline_num].opcode = ZEND_JMP_SET_VAR;
			CG(active_op_array)->opcodes[jmp_token->u.op.opline_num].result_type = IS_VAR;
			opline->opcode = ZEND_QM_ASSIGN_VAR;
			opline->result_type = IS_VAR;
		} else {
			opline->opcode = ZEND_QM_ASSIGN;
		}
	} else {
		opline->opcode = ZEND_QM_ASSIGN_VAR;
	}
	opline->extended_value = 0;
	SET_NODE(opline->op1, false_value);
	SET_UNUSED(opline->op2);

	GET_NODE(result, opline->result);

	CG(active_op_array)->opcodes[jmp_token->u.op.opline_num].op2.opline_num = get_next_op_number(CG(active_op_array));

	DEC_BPC(CG(active_op_array));
}
/* }}} */

void zend_do_begin_qm_op(const znode *cond, znode *qm_token TSRMLS_DC) /* {{{ */
{
	int jmpz_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline;

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMPZ;
	SET_NODE(opline->op1, cond);
	SET_UNUSED(opline->op2);
	opline->op2.opline_num = jmpz_op_number;
	GET_NODE(qm_token, opline->op2);

	INC_BPC(CG(active_op_array));
}
/* }}} */

void zend_do_qm_true(const znode *true_value, znode *qm_token, znode *colon_token TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	CG(active_op_array)->opcodes[qm_token->u.op.opline_num].op2.opline_num = get_next_op_number(CG(active_op_array))+1; /* jmp over the ZEND_JMP */

	if (true_value->op_type == IS_VAR || true_value->op_type == IS_CV) {
		opline->opcode = ZEND_QM_ASSIGN_VAR;
		opline->result_type = IS_VAR;
	} else {
		opline->opcode = ZEND_QM_ASSIGN;
		opline->result_type = IS_TMP_VAR;
	}
	opline->result.var = get_temporary_variable(CG(active_op_array));
	SET_NODE(opline->op1, true_value);
	SET_UNUSED(opline->op2);

	GET_NODE(qm_token, opline->result);
	colon_token->u.op.opline_num = get_next_op_number(CG(active_op_array));

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_JMP;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}
/* }}} */

void zend_do_qm_false(znode *result, const znode *false_value, const znode *qm_token, const znode *colon_token TSRMLS_DC) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	SET_NODE(opline->result, qm_token);
	if (qm_token->op_type == IS_TMP_VAR) {
		if (false_value->op_type == IS_VAR || false_value->op_type == IS_CV) {
			CG(active_op_array)->opcodes[colon_token->u.op.opline_num - 1].opcode = ZEND_QM_ASSIGN_VAR;
			CG(active_op_array)->opcodes[colon_token->u.op.opline_num - 1].result_type = IS_VAR;
			opline->opcode = ZEND_QM_ASSIGN_VAR;
			opline->result_type = IS_VAR;
		} else {
			opline->opcode = ZEND_QM_ASSIGN;
		}
	} else {
		opline->opcode = ZEND_QM_ASSIGN_VAR;
	}
	SET_NODE(opline->op1, false_value);
	SET_UNUSED(opline->op2);

	CG(active_op_array)->opcodes[colon_token->u.op.opline_num].op1.opline_num = get_next_op_number(CG(active_op_array));

	GET_NODE(result, opline->result);

	DEC_BPC(CG(active_op_array));
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

void zend_do_ticks(TSRMLS_D) /* {{{ */
{
	zend_op *opline;

	/* This prevents a double TICK generated by the parser statement of "declare()" */
	if (CG(active_op_array)->last && CG(active_op_array)->opcodes[CG(active_op_array)->last - 1].opcode == ZEND_TICKS) {
		return;
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_TICKS;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
	opline->extended_value = Z_LVAL(CG(declarables).ticks);
}
/* }}} */

zend_bool zend_is_auto_global_quick(const char *name, uint name_len, ulong hash TSRMLS_DC) /* {{{ */
{
	zend_auto_global *auto_global;

	if (zend_hash_quick_find(CG(auto_globals), name, name_len+1, hash, (void **) &auto_global)==SUCCESS) {
		if (auto_global->armed) {
			auto_global->armed = auto_global->auto_global_callback(auto_global->name, auto_global->name_len TSRMLS_CC);
		}
		return 1;
	}
	return 0;
}
/* }}} */

zend_bool zend_is_auto_global(const char *name, uint name_len TSRMLS_DC) /* {{{ */
{
	return zend_is_auto_global_quick(name, name_len, zend_hash_func(name, name_len+1) TSRMLS_CC);
}
/* }}} */

int zend_register_auto_global(const char *name, uint name_len, zend_bool jit, zend_auto_global_callback auto_global_callback TSRMLS_DC) /* {{{ */
{
	zend_auto_global auto_global;

	auto_global.name = zend_new_interned_string((char*)name, name_len + 1, 0 TSRMLS_CC);
	auto_global.name_len = name_len;
	auto_global.auto_global_callback = auto_global_callback;
	auto_global.jit = jit;

	return zend_hash_add(CG(auto_globals), name, name_len+1, &auto_global, sizeof(zend_auto_global), NULL);
}
/* }}} */

static int zend_auto_global_init(zend_auto_global *auto_global TSRMLS_DC) /* {{{ */
{
	if (auto_global->jit) {
		auto_global->armed = 1;
	} else if (auto_global->auto_global_callback) {
		auto_global->armed = auto_global->auto_global_callback(auto_global->name, auto_global->name_len TSRMLS_CC);
	} else {
		auto_global->armed = 0;
	}
	return 0;
}
/* }}} */

ZEND_API void zend_activate_auto_globals(TSRMLS_D) /* {{{ */
{
	zend_hash_apply(CG(auto_globals), (apply_func_t) zend_auto_global_init TSRMLS_CC);
}
/* }}} */

int zendlex(znode *zendlval TSRMLS_DC) /* {{{ */
{
	int retval;

	if (CG(increment_lineno)) {
		CG(zend_lineno)++;
		CG(increment_lineno) = 0;
	}

again:
	Z_TYPE(zendlval->u.constant) = IS_LONG;
	retval = lex_scan(&zendlval->u.constant TSRMLS_CC);
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
			if (CG(has_bracketed_namespaces) && !CG(in_namespace)) {
				goto again;
			}
			retval = ';'; /* implicit ; */
			break;
		case T_OPEN_TAG_WITH_ECHO:
			retval = T_ECHO;
			break;
	}

	INIT_PZVAL(&zendlval->u.constant);
	zendlval->op_type = IS_CONST;
	return retval;
}
/* }}} */

ZEND_API void zend_initialize_class_data(zend_class_entry *ce, zend_bool nullify_handlers TSRMLS_DC) /* {{{ */
{
	zend_bool persistent_hashes = (ce->type == ZEND_INTERNAL_CLASS) ? 1 : 0;
	dtor_func_t zval_ptr_dtor_func = ((persistent_hashes) ? ZVAL_INTERNAL_PTR_DTOR : ZVAL_PTR_DTOR);

	ce->refcount = 1;
	ce->ce_flags = 0;

	ce->default_properties_table = NULL;
	ce->default_static_members_table = NULL;
	zend_hash_init_ex(&ce->properties_info, 0, NULL, (dtor_func_t) (persistent_hashes ? zend_destroy_property_info_internal : zend_destroy_property_info), persistent_hashes, 0);
	zend_hash_init_ex(&ce->constants_table, 0, NULL, zval_ptr_dtor_func, persistent_hashes, 0);
	zend_hash_init_ex(&ce->function_table, 0, NULL, ZEND_FUNCTION_DTOR, persistent_hashes, 0);

	if (ce->type == ZEND_INTERNAL_CLASS) {
#ifdef ZTS
		int n = zend_hash_num_elements(CG(class_table));

		if (CG(static_members_table) && n >= CG(last_static_member)) {
			/* Support for run-time declaration: dl() */
			CG(last_static_member) = n+1;
			CG(static_members_table) = realloc(CG(static_members_table), (n+1)*sizeof(zval**));
			CG(static_members_table)[n] = NULL;
		}
		ce->static_members_table = (zval**)(zend_intptr_t)n;
#else
		ce->static_members_table = NULL;
#endif
	} else {
		ce->static_members_table = ce->default_static_members_table;
		ce->info.user.doc_comment = NULL;
		ce->info.user.doc_comment_len = 0;
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

int zend_get_class_fetch_type(const char *class_name, uint class_name_len) /* {{{ */
{
	if ((class_name_len == sizeof("self")-1) &&
		!strncasecmp(class_name, "self", sizeof("self")-1)) {
		return ZEND_FETCH_CLASS_SELF;
	} else if ((class_name_len == sizeof("parent")-1) &&
		!strncasecmp(class_name, "parent", sizeof("parent")-1)) {
		return ZEND_FETCH_CLASS_PARENT;
	} else if ((class_name_len == sizeof("static")-1) &&
		!strncasecmp(class_name, "static", sizeof("static")-1)) {
		return ZEND_FETCH_CLASS_STATIC;
	} else {
		return ZEND_FETCH_CLASS_DEFAULT;
	}
}
/* }}} */

ZEND_API const char* zend_get_compiled_variable_name(const zend_op_array *op_array, zend_uint var, int* name_len) /* {{{ */
{
	if (name_len) {
		*name_len = op_array->vars[var].name_len;
	}
	return op_array->vars[var].name;
}
/* }}} */

void zend_do_build_namespace_name(znode *result, znode *prefix, znode *name TSRMLS_DC) /* {{{ */
{
	if (prefix) {
		*result = *prefix;
		if (Z_TYPE(result->u.constant) == IS_STRING &&
		    Z_STRLEN(result->u.constant) == 0) {
			/* namespace\ */
			if (CG(current_namespace)) {
				znode tmp;

				zval_dtor(&result->u.constant);
				tmp.op_type = IS_CONST;
				tmp.u.constant = *CG(current_namespace);
				zval_copy_ctor(&tmp.u.constant);
				zend_do_build_namespace_name(result, NULL, &tmp TSRMLS_CC);
			}
		}
	} else {
		result->op_type = IS_CONST;
		Z_TYPE(result->u.constant) = IS_STRING;
		Z_STRVAL(result->u.constant) = NULL;
		Z_STRLEN(result->u.constant) = 0;
	}
	/* prefix = result */
	zend_do_build_full_name(NULL, result, name, 0 TSRMLS_CC);
}
/* }}} */

void zend_do_begin_namespace(const znode *name, zend_bool with_bracket TSRMLS_DC) /* {{{ */
{
	char *lcname;

	/* handle mixed syntax declaration or nested namespaces */
	if (!CG(has_bracketed_namespaces)) {
		if (CG(current_namespace)) {
			/* previous namespace declarations were unbracketed */
			if (with_bracket) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot mix bracketed namespace declarations with unbracketed namespace declarations");
			}
		}
	} else {
		/* previous namespace declarations were bracketed */
		if (!with_bracket) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot mix bracketed namespace declarations with unbracketed namespace declarations");
		} else if (CG(current_namespace) || CG(in_namespace)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Namespace declarations cannot be nested");
		}
	}

	if (((!with_bracket && !CG(current_namespace)) || (with_bracket && !CG(has_bracketed_namespaces))) && CG(active_op_array)->last > 0) {
		/* ignore ZEND_EXT_STMT and ZEND_TICKS */
		int num = CG(active_op_array)->last;
		while (num > 0 &&
		       (CG(active_op_array)->opcodes[num-1].opcode == ZEND_EXT_STMT ||
		        CG(active_op_array)->opcodes[num-1].opcode == ZEND_TICKS)) {
			--num;
		}
		if (num > 0) {
			zend_error_noreturn(E_COMPILE_ERROR, "Namespace declaration statement has to be the very first statement in the script");
		}
	}

	CG(in_namespace) = 1;
	if (with_bracket) {
		CG(has_bracketed_namespaces) = 1;
	}

	if (name) {
		lcname = zend_str_tolower_dup(Z_STRVAL(name->u.constant), Z_STRLEN(name->u.constant));
		if (((Z_STRLEN(name->u.constant) == sizeof("self")-1) &&
		      !memcmp(lcname, "self", sizeof("self")-1)) ||
		    ((Z_STRLEN(name->u.constant) == sizeof("parent")-1) &&
	          !memcmp(lcname, "parent", sizeof("parent")-1))) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use '%s' as namespace name", Z_STRVAL(name->u.constant));
		}
		efree(lcname);

		if (CG(current_namespace)) {
			zval_dtor(CG(current_namespace));
		} else {
			ALLOC_ZVAL(CG(current_namespace));
		}
		*CG(current_namespace) = name->u.constant;
	} else {
		if (CG(current_namespace)) {
			zval_dtor(CG(current_namespace));
			FREE_ZVAL(CG(current_namespace));
			CG(current_namespace) = NULL;
		}
	}

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

	if (CG(doc_comment)) {
		efree(CG(doc_comment));
		CG(doc_comment) = NULL;
		CG(doc_comment_len) = 0;
	}
}
/* }}} */

void zend_do_use(znode *ns_name, znode *new_name TSRMLS_DC) /* {{{ */
{
	char *lcname;
	zval *name, *ns, tmp;
	zend_bool warn = 0;
	zend_class_entry **pce;

	if (!CG(current_import)) {
		CG(current_import) = emalloc(sizeof(HashTable));
		zend_hash_init(CG(current_import), 0, NULL, ZVAL_PTR_DTOR, 0);
	}

	MAKE_STD_ZVAL(ns);
	ZVAL_ZVAL(ns, &ns_name->u.constant, 0, 0);
	if (new_name) {
		name = &new_name->u.constant;
	} else {
		const char *p;

		/* The form "use A\B" is eqivalent to "use A\B as B".
		   So we extract the last part of compound name to use as a new_name */
		name = &tmp;
		p = zend_memrchr(Z_STRVAL_P(ns), '\\', Z_STRLEN_P(ns));
		if (p) {
			ZVAL_STRING(name, p+1, 1);
		} else {
			ZVAL_ZVAL(name, ns, 1, 0);
			warn = !CG(current_namespace);
		}
	}

	lcname = zend_str_tolower_dup(Z_STRVAL_P(name), Z_STRLEN_P(name));

	if (((Z_STRLEN_P(name) == sizeof("self")-1) &&
				!memcmp(lcname, "self", sizeof("self")-1)) ||
			((Z_STRLEN_P(name) == sizeof("parent")-1) &&
	   !memcmp(lcname, "parent", sizeof("parent")-1))) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use %s as %s because '%s' is a special class name", Z_STRVAL_P(ns), Z_STRVAL_P(name), Z_STRVAL_P(name));
	}

	if (CG(current_namespace)) {
		/* Prefix import name with current namespace name to avoid conflicts with classes */
		char *c_ns_name = emalloc(Z_STRLEN_P(CG(current_namespace)) + 1 + Z_STRLEN_P(name) + 1);

		zend_str_tolower_copy(c_ns_name, Z_STRVAL_P(CG(current_namespace)), Z_STRLEN_P(CG(current_namespace)));
		c_ns_name[Z_STRLEN_P(CG(current_namespace))] = '\\';
		memcpy(c_ns_name+Z_STRLEN_P(CG(current_namespace))+1, lcname, Z_STRLEN_P(name)+1);
		if (zend_hash_exists(CG(class_table), c_ns_name, Z_STRLEN_P(CG(current_namespace)) + 1 + Z_STRLEN_P(name)+1)) {
			char *tmp2 = zend_str_tolower_dup(Z_STRVAL_P(ns), Z_STRLEN_P(ns));

			if (Z_STRLEN_P(ns) != Z_STRLEN_P(CG(current_namespace)) + 1 + Z_STRLEN_P(name) ||
				memcmp(tmp2, c_ns_name, Z_STRLEN_P(ns))) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot use %s as %s because the name is already in use", Z_STRVAL_P(ns), Z_STRVAL_P(name));
			}
			efree(tmp2);
		}
		efree(c_ns_name);
	} else if (zend_hash_find(CG(class_table), lcname, Z_STRLEN_P(name)+1, (void**)&pce) == SUCCESS &&
	           (*pce)->type == ZEND_USER_CLASS &&
	           (*pce)->info.user.filename == CG(compiled_filename)) {
		char *c_tmp = zend_str_tolower_dup(Z_STRVAL_P(ns), Z_STRLEN_P(ns));

		if (Z_STRLEN_P(ns) != Z_STRLEN_P(name) ||
			memcmp(c_tmp, lcname, Z_STRLEN_P(ns))) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use %s as %s because the name is already in use", Z_STRVAL_P(ns), Z_STRVAL_P(name));
		}
		efree(c_tmp);
	}

	if (zend_hash_add(CG(current_import), lcname, Z_STRLEN_P(name)+1, &ns, sizeof(zval*), NULL) != SUCCESS) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use %s as %s because the name is already in use", Z_STRVAL_P(ns), Z_STRVAL_P(name));
	}
	if (warn) {
		if (!strcmp(Z_STRVAL_P(name), "strict")) {
			zend_error_noreturn(E_COMPILE_ERROR, "You seem to be trying to use a different language...");
		}
		zend_error(E_WARNING, "The use statement with non-compound name '%s' has no effect", Z_STRVAL_P(name));
	}
	efree(lcname);
	zval_dtor(name);
}
/* }}} */

void zend_do_use_non_class(znode *ns_name, znode *new_name, int is_function, zend_bool case_sensitive, HashTable *current_import_sub, HashTable *lookup_table TSRMLS_DC) /* {{{ */
{
	char *lookup_name;
	zval *name, *ns, tmp;
	zend_bool warn = 0;

	MAKE_STD_ZVAL(ns);
	ZVAL_ZVAL(ns, &ns_name->u.constant, 0, 0);
	if (new_name) {
		name = &new_name->u.constant;
	} else {
		const char *p;

		/* The form "use A\B" is eqivalent to "use A\B as B".
		   So we extract the last part of compound name to use as a new_name */
		name = &tmp;
		p = zend_memrchr(Z_STRVAL_P(ns), '\\', Z_STRLEN_P(ns));
		if (p) {
			ZVAL_STRING(name, p+1, 1);
		} else {
			ZVAL_ZVAL(name, ns, 1, 0);
			warn = !CG(current_namespace);
		}
	}

	if (case_sensitive) {
		lookup_name = estrndup(Z_STRVAL_P(name), Z_STRLEN_P(name));
	} else {
		lookup_name = zend_str_tolower_dup(Z_STRVAL_P(name), Z_STRLEN_P(name));
	}

	if (CG(current_namespace)) {
		/* Prefix import name with current namespace name to avoid conflicts with functions/consts */
		char *c_ns_name = emalloc(Z_STRLEN_P(CG(current_namespace)) + 1 + Z_STRLEN_P(name) + 1);

		zend_str_tolower_copy(c_ns_name, Z_STRVAL_P(CG(current_namespace)), Z_STRLEN_P(CG(current_namespace)));
		c_ns_name[Z_STRLEN_P(CG(current_namespace))] = '\\';
		memcpy(c_ns_name+Z_STRLEN_P(CG(current_namespace))+1, lookup_name, Z_STRLEN_P(name)+1);
		if (zend_hash_exists(lookup_table, c_ns_name, Z_STRLEN_P(CG(current_namespace)) + 1 + Z_STRLEN_P(name)+1)) {
			char *tmp2 = zend_str_tolower_dup(Z_STRVAL_P(ns), Z_STRLEN_P(ns));

			if (Z_STRLEN_P(ns) != Z_STRLEN_P(CG(current_namespace)) + 1 + Z_STRLEN_P(name) ||
				memcmp(tmp2, c_ns_name, Z_STRLEN_P(ns))) {
				zend_error(E_COMPILE_ERROR, "Cannot use %s %s as %s because the name is already in use", is_function ? "function" : "const", Z_STRVAL_P(ns), Z_STRVAL_P(name));
			}
			efree(tmp2);
		}
		efree(c_ns_name);
	} else if (is_function) {
		zend_function *function;

		if (zend_hash_find(lookup_table, lookup_name, Z_STRLEN_P(name)+1, (void **) &function) == SUCCESS && function->type == ZEND_USER_FUNCTION && strcmp(function->op_array.filename, CG(compiled_filename)) == 0) {
			char *c_tmp = zend_str_tolower_dup(Z_STRVAL_P(ns), Z_STRLEN_P(ns));

			if (Z_STRLEN_P(ns) != Z_STRLEN_P(name) ||
				memcmp(c_tmp, lookup_name, Z_STRLEN_P(ns))) {
				zend_error(E_COMPILE_ERROR, "Cannot use function %s as %s because the name is already in use", Z_STRVAL_P(ns), Z_STRVAL_P(name));
			}
			efree(c_tmp);
		}
	} else {
		const char *filename;

		if (zend_hash_find(lookup_table, lookup_name, Z_STRLEN_P(name)+1, (void **) &filename) == SUCCESS && strcmp(filename, CG(compiled_filename)) == 0) {
			char *c_tmp = zend_str_tolower_dup(Z_STRVAL_P(ns), Z_STRLEN_P(ns));

			if (Z_STRLEN_P(ns) != Z_STRLEN_P(name) ||
				memcmp(c_tmp, lookup_name, Z_STRLEN_P(ns))) {
				zend_error(E_COMPILE_ERROR, "Cannot use const %s as %s because the name is already in use", Z_STRVAL_P(ns), Z_STRVAL_P(name));
			}
			efree(c_tmp);
		}
	}

	if (zend_hash_add(current_import_sub, lookup_name, Z_STRLEN_P(name)+1, &ns, sizeof(zval*), NULL) != SUCCESS) {
		zend_error(E_COMPILE_ERROR, "Cannot use %s %s as %s because the name is already in use", is_function ? "function" : "const", Z_STRVAL_P(ns), Z_STRVAL_P(name));
	}
	if (warn) {
		zend_error(E_WARNING, "The use %s statement with non-compound name '%s' has no effect", is_function ? "function" : "const", Z_STRVAL_P(name));
	}
	efree(lookup_name);
	zval_dtor(name);
}
/* }}} */

void zend_do_use_function(znode *ns_name, znode *new_name TSRMLS_DC) /* {{{ */
{
	if (!CG(current_import_function)) {
		CG(current_import_function) = emalloc(sizeof(HashTable));
		zend_hash_init(CG(current_import_function), 0, NULL, ZVAL_PTR_DTOR, 0);
	}

	zend_do_use_non_class(ns_name, new_name, 1, 0, CG(current_import_function), CG(function_table) TSRMLS_CC);
}
/* }}} */

void zend_do_use_const(znode *ns_name, znode *new_name TSRMLS_DC) /* {{{ */
{
	if (!CG(current_import_const)) {
		CG(current_import_const) = emalloc(sizeof(HashTable));
		zend_hash_init(CG(current_import_const), 0, NULL, ZVAL_PTR_DTOR, 0);
	}

	zend_do_use_non_class(ns_name, new_name, 0, 1, CG(current_import_const), &CG(const_filenames) TSRMLS_CC);
}
/* }}} */

void zend_do_declare_constant(znode *name, znode *value TSRMLS_DC) /* {{{ */
{
	zend_op *opline;
	zval **ns_name;

	if (zend_get_ct_const(&name->u.constant, 0 TSRMLS_CC)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare constant '%s'", Z_STRVAL(name->u.constant));
	}

	if (CG(current_namespace)) {
		/* Prefix constant name with name of current namespace, lowercased */
		znode tmp;

		tmp.op_type = IS_CONST;
		tmp.u.constant = *CG(current_namespace);
		Z_STRVAL(tmp.u.constant) = zend_str_tolower_dup(Z_STRVAL(tmp.u.constant), Z_STRLEN(tmp.u.constant));
		zend_do_build_namespace_name(&tmp, &tmp, name TSRMLS_CC);
		*name = tmp;
	}

	/* Constant name must not conflict with import names */
	if (CG(current_import_const) &&
	    zend_hash_find(CG(current_import_const), Z_STRVAL(name->u.constant), Z_STRLEN(name->u.constant)+1, (void**)&ns_name) == SUCCESS) {

		char *tmp = estrndup(Z_STRVAL_PP(ns_name), Z_STRLEN_PP(ns_name));

		if (Z_STRLEN_PP(ns_name) != Z_STRLEN(name->u.constant) ||
			memcmp(tmp, Z_STRVAL(name->u.constant), Z_STRLEN(name->u.constant))) {
			zend_error(E_COMPILE_ERROR, "Cannot declare const %s because the name is already in use", Z_STRVAL(name->u.constant));
		}
		efree(tmp);
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_DECLARE_CONST;
	SET_UNUSED(opline->result);
	SET_NODE(opline->op1, name);
	SET_NODE(opline->op2, value);

	zend_hash_add(&CG(const_filenames), Z_STRVAL(name->u.constant), Z_STRLEN(name->u.constant)+1, CG(compiled_filename), strlen(CG(compiled_filename))+1, NULL);
}
/* }}} */

void zend_verify_namespace(TSRMLS_D) /* {{{ */
{
	if (CG(has_bracketed_namespaces) && !CG(in_namespace)) {
		zend_error_noreturn(E_COMPILE_ERROR, "No code may exist outside of namespace {}");
	}
}
/* }}} */

void zend_do_end_namespace(TSRMLS_D) /* {{{ */
{
	CG(in_namespace) = 0;
	if (CG(current_namespace)) {
		zval_dtor(CG(current_namespace));
		FREE_ZVAL(CG(current_namespace));
		CG(current_namespace) = NULL;
	}
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

void zend_do_end_compilation(TSRMLS_D) /* {{{ */
{
	CG(has_bracketed_namespaces) = 0;
	zend_do_end_namespace(TSRMLS_C);
}
/* }}} */

void zend_do_constant_expression(znode *result, zend_ast *ast TSRMLS_DC) /* {{{ */
{
	if (ast->kind == ZEND_CONST) {
		result->u.constant = *ast->u.val;
		efree(ast);
	} else if (zend_ast_is_ct_constant(ast)) {
		zend_ast_evaluate(&result->u.constant, ast, NULL TSRMLS_CC);
		zend_ast_destroy(ast);
	} else {
		Z_TYPE(result->u.constant) = IS_CONSTANT_AST;
		Z_AST(result->u.constant) = ast;
	}
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
