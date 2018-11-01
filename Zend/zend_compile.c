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
   |          Nikita Popov <nikic@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include <zend_language_parser.h>
#include "zend.h"
#include "zend_compile.h"
#include "zend_constants.h"
#include "zend_llist.h"
#include "zend_API.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "zend_virtual_cwd.h"
#include "zend_multibyte.h"
#include "zend_language_scanner.h"
#include "zend_inheritance.h"
#include "zend_vm.h"

#define SET_NODE(target, src) do { \
		target ## _type = (src)->op_type; \
		if ((src)->op_type == IS_CONST) { \
			target.constant = zend_add_literal(CG(active_op_array), &(src)->u.constant); \
		} else { \
			target = (src)->u.op; \
		} \
	} while (0)

#define GET_NODE(target, src) do { \
		(target)->op_type = src ## _type; \
		if ((target)->op_type == IS_CONST) { \
			ZVAL_COPY_VALUE(&(target)->u.constant, CT_CONSTANT(src)); \
		} else { \
			(target)->u.op = src; \
		} \
	} while (0)

#define FC(member) (CG(file_context).member)

typedef struct _zend_loop_var {
	zend_uchar opcode;
	zend_uchar var_type;
	uint32_t   var_num;
	union {
		uint32_t try_catch_offset;
		uint32_t live_range_offset;
	} u;
} zend_loop_var;

static inline uint32_t zend_alloc_cache_slot(void) {
	zend_op_array *op_array = CG(active_op_array);
	uint32_t ret = op_array->cache_size;
	op_array->cache_size += sizeof(void*);
	return ret;
}

#define POLYMORPHIC_CACHE_SLOT_SIZE 2

static inline uint32_t zend_alloc_polymorphic_cache_slot(void) {
	zend_op_array *op_array = CG(active_op_array);
	uint32_t ret = op_array->cache_size;
	op_array->cache_size += POLYMORPHIC_CACHE_SLOT_SIZE * sizeof(void*);
	return ret;
}

ZEND_API zend_op_array *(*zend_compile_file)(zend_file_handle *file_handle, int type);
ZEND_API zend_op_array *(*zend_compile_string)(zval *source_string, char *filename);

#ifndef ZTS
ZEND_API zend_compiler_globals compiler_globals;
ZEND_API zend_executor_globals executor_globals;
#endif

static zend_op *zend_emit_op(znode *result, zend_uchar opcode, znode *op1, znode *op2);
static zend_bool zend_try_ct_eval_array(zval *result, zend_ast *ast);

static void init_op(zend_op *op)
{
	MAKE_NOP(op);
	op->extended_value = 0;
	op->lineno = CG(zend_lineno);
}

static zend_op *get_next_op(zend_op_array *op_array)
{
	uint32_t next_op_num = op_array->last++;
	zend_op *next_op;

	if (UNEXPECTED(next_op_num >= CG(context).opcodes_size)) {
		CG(context).opcodes_size *= 4;
		op_array->opcodes = erealloc(op_array->opcodes, CG(context).opcodes_size * sizeof(zend_op));
	}

	next_op = &(op_array->opcodes[next_op_num]);

	init_op(next_op);

	return next_op;
}

static zend_brk_cont_element *get_next_brk_cont_element(void)
{
	CG(context).last_brk_cont++;
	CG(context).brk_cont_array = erealloc(CG(context).brk_cont_array, sizeof(zend_brk_cont_element) * CG(context).last_brk_cont);
	return &CG(context).brk_cont_array[CG(context).last_brk_cont-1];
}

static void zend_destroy_property_info_internal(zval *zv) /* {{{ */
{
	zend_property_info *property_info = Z_PTR_P(zv);

	zend_string_release_ex(property_info->name, 1);
	free(property_info);
}
/* }}} */

static zend_string *zend_build_runtime_definition_key(zend_string *name, unsigned char *lex_pos) /* {{{ */
{
	zend_string *result;
	char char_pos_buf[32];
	size_t char_pos_len = zend_sprintf(char_pos_buf, "%p", lex_pos);
	zend_string *filename = CG(active_op_array)->filename;

	/* NULL, name length, filename length, last accepting char position length */
	result = zend_string_alloc(1 + ZSTR_LEN(name) + ZSTR_LEN(filename) + char_pos_len, 0);
 	sprintf(ZSTR_VAL(result), "%c%s%s%s", '\0', ZSTR_VAL(name), ZSTR_VAL(filename), char_pos_buf);
	return zend_new_interned_string(result);
}
/* }}} */

static zend_bool zend_get_unqualified_name(const zend_string *name, const char **result, size_t *result_len) /* {{{ */
{
	const char *ns_separator = zend_memrchr(ZSTR_VAL(name), '\\', ZSTR_LEN(name));
	if (ns_separator != NULL) {
		*result = ns_separator + 1;
		*result_len = ZSTR_VAL(name) + ZSTR_LEN(name) - *result;
		return 1;
	}

	return 0;
}
/* }}} */

struct reserved_class_name {
	const char *name;
	size_t len;
};
static const struct reserved_class_name reserved_class_names[] = {
	{ZEND_STRL("bool")},
	{ZEND_STRL("false")},
	{ZEND_STRL("float")},
	{ZEND_STRL("int")},
	{ZEND_STRL("null")},
	{ZEND_STRL("parent")},
	{ZEND_STRL("self")},
	{ZEND_STRL("static")},
	{ZEND_STRL("string")},
	{ZEND_STRL("true")},
	{ZEND_STRL("void")},
	{ZEND_STRL("iterable")},
	{ZEND_STRL("object")},
	{NULL, 0}
};

static zend_bool zend_is_reserved_class_name(const zend_string *name) /* {{{ */
{
	const struct reserved_class_name *reserved = reserved_class_names;

	const char *uqname = ZSTR_VAL(name);
	size_t uqname_len = ZSTR_LEN(name);
	zend_get_unqualified_name(name, &uqname, &uqname_len);

	for (; reserved->name; ++reserved) {
		if (uqname_len == reserved->len
			&& zend_binary_strcasecmp(uqname, uqname_len, reserved->name, reserved->len) == 0
		) {
			return 1;
		}
	}

	return 0;
}
/* }}} */

void zend_assert_valid_class_name(const zend_string *name) /* {{{ */
{
	if (zend_is_reserved_class_name(name)) {
		zend_error_noreturn(E_COMPILE_ERROR,
			"Cannot use '%s' as class name as it is reserved", ZSTR_VAL(name));
	}
}
/* }}} */

typedef struct _builtin_type_info {
	const char* name;
	const size_t name_len;
	const zend_uchar type;
} builtin_type_info;

static const builtin_type_info builtin_types[] = {
	{ZEND_STRL("int"), IS_LONG},
	{ZEND_STRL("float"), IS_DOUBLE},
	{ZEND_STRL("string"), IS_STRING},
	{ZEND_STRL("bool"), _IS_BOOL},
	{ZEND_STRL("void"), IS_VOID},
	{ZEND_STRL("iterable"), IS_ITERABLE},
	{ZEND_STRL("object"), IS_OBJECT},
	{NULL, 0, IS_UNDEF}
};


static zend_always_inline zend_uchar zend_lookup_builtin_type_by_name(const zend_string *name) /* {{{ */
{
	const builtin_type_info *info = &builtin_types[0];

	for (; info->name; ++info) {
		if (ZSTR_LEN(name) == info->name_len
			&& zend_binary_strcasecmp(ZSTR_VAL(name), ZSTR_LEN(name), info->name, info->name_len) == 0
		) {
			return info->type;
		}
	}

	return 0;
}
/* }}} */


void zend_oparray_context_begin(zend_oparray_context *prev_context) /* {{{ */
{
	*prev_context = CG(context);
	CG(context).opcodes_size = INITIAL_OP_ARRAY_SIZE;
	CG(context).vars_size = 0;
	CG(context).literals_size = 0;
	CG(context).backpatch_count = 0;
	CG(context).fast_call_var = -1;
	CG(context).try_catch_offset = -1;
	CG(context).current_brk_cont = -1;
	CG(context).last_brk_cont = 0;
	CG(context).brk_cont_array = NULL;
	CG(context).labels = NULL;
}
/* }}} */

void zend_oparray_context_end(zend_oparray_context *prev_context) /* {{{ */
{
	if (CG(context).brk_cont_array) {
		efree(CG(context).brk_cont_array);
		CG(context).brk_cont_array = NULL;
	}
	if (CG(context).labels) {
		zend_hash_destroy(CG(context).labels);
		FREE_HASHTABLE(CG(context).labels);
		CG(context).labels = NULL;
	}
	CG(context) = *prev_context;
}
/* }}} */

static void zend_reset_import_tables(void) /* {{{ */
{
	if (FC(imports)) {
		zend_hash_destroy(FC(imports));
		efree(FC(imports));
		FC(imports) = NULL;
	}

	if (FC(imports_function)) {
		zend_hash_destroy(FC(imports_function));
		efree(FC(imports_function));
		FC(imports_function) = NULL;
	}

	if (FC(imports_const)) {
		zend_hash_destroy(FC(imports_const));
		efree(FC(imports_const));
		FC(imports_const) = NULL;
	}
}
/* }}} */

static void zend_end_namespace(void) /* {{{ */ {
	FC(in_namespace) = 0;
	zend_reset_import_tables();
	if (FC(current_namespace)) {
		zend_string_release_ex(FC(current_namespace), 0);
		FC(current_namespace) = NULL;
	}
}
/* }}} */

void zend_file_context_begin(zend_file_context *prev_context) /* {{{ */
{
	*prev_context = CG(file_context);
	FC(imports) = NULL;
	FC(imports_function) = NULL;
	FC(imports_const) = NULL;
	FC(current_namespace) = NULL;
	FC(in_namespace) = 0;
	FC(has_bracketed_namespaces) = 0;
	FC(declarables).ticks = 0;
	zend_hash_init(&FC(seen_symbols), 8, NULL, NULL, 0);
}
/* }}} */

void zend_file_context_end(zend_file_context *prev_context) /* {{{ */
{
	zend_end_namespace();
	zend_hash_destroy(&FC(seen_symbols));
	CG(file_context) = *prev_context;
}
/* }}} */

void zend_init_compiler_data_structures(void) /* {{{ */
{
	zend_stack_init(&CG(loop_var_stack), sizeof(zend_loop_var));
	zend_stack_init(&CG(delayed_oplines_stack), sizeof(zend_op));
	CG(active_class_entry) = NULL;
	CG(in_compilation) = 0;
	CG(start_lineno) = 0;

	CG(encoding_declared) = 0;
}
/* }}} */

static void zend_register_seen_symbol(zend_string *name, uint32_t kind) {
	zval *zv = zend_hash_find(&FC(seen_symbols), name);
	if (zv) {
		Z_LVAL_P(zv) |= kind;
	} else {
		zval tmp;
		ZVAL_LONG(&tmp, kind);
		zend_hash_add_new(&FC(seen_symbols), name, &tmp);
	}
}

static zend_bool zend_have_seen_symbol(zend_string *name, uint32_t kind) {
	zval *zv = zend_hash_find(&FC(seen_symbols), name);
	return zv && (Z_LVAL_P(zv) & kind) != 0;
}

ZEND_API void file_handle_dtor(zend_file_handle *fh) /* {{{ */
{

	zend_file_handle_dtor(fh);
}
/* }}} */

void init_compiler(void) /* {{{ */
{
	CG(arena) = zend_arena_create(64 * 1024);
	CG(active_op_array) = NULL;
	memset(&CG(context), 0, sizeof(CG(context)));
	zend_init_compiler_data_structures();
	zend_init_rsrc_list();
	zend_hash_init(&CG(filenames_table), 8, NULL, ZVAL_PTR_DTOR, 0);
	zend_llist_init(&CG(open_files), sizeof(zend_file_handle), (void (*)(void *)) file_handle_dtor, 0);
	CG(unclean_shutdown) = 0;
}
/* }}} */

void shutdown_compiler(void) /* {{{ */
{
	zend_stack_destroy(&CG(loop_var_stack));
	zend_stack_destroy(&CG(delayed_oplines_stack));
	zend_hash_destroy(&CG(filenames_table));
	zend_arena_destroy(CG(arena));
}
/* }}} */

ZEND_API zend_string *zend_set_compiled_filename(zend_string *new_compiled_filename) /* {{{ */
{
	zval *p, rv;

	if ((p = zend_hash_find(&CG(filenames_table), new_compiled_filename))) {
		ZEND_ASSERT(Z_TYPE_P(p) == IS_STRING);
		CG(compiled_filename) = Z_STR_P(p);
		return Z_STR_P(p);
	}

	new_compiled_filename = zend_new_interned_string(zend_string_copy(new_compiled_filename));
	ZVAL_STR(&rv, new_compiled_filename);
	zend_hash_add_new(&CG(filenames_table), new_compiled_filename, &rv);

	CG(compiled_filename) = new_compiled_filename;
	return new_compiled_filename;
}
/* }}} */

ZEND_API void zend_restore_compiled_filename(zend_string *original_compiled_filename) /* {{{ */
{
	CG(compiled_filename) = original_compiled_filename;
}
/* }}} */

ZEND_API zend_string *zend_get_compiled_filename(void) /* {{{ */
{
	return CG(compiled_filename);
}
/* }}} */

ZEND_API int zend_get_compiled_lineno(void) /* {{{ */
{
	return CG(zend_lineno);
}
/* }}} */

ZEND_API zend_bool zend_is_compiling(void) /* {{{ */
{
	return CG(in_compilation);
}
/* }}} */

static uint32_t get_temporary_variable(zend_op_array *op_array) /* {{{ */
{
	return (uint32_t)op_array->T++;
}
/* }}} */

static int lookup_cv(zend_op_array *op_array, zend_string *name) /* {{{ */{
	int i = 0;
	zend_ulong hash_value = zend_string_hash_val(name);

	while (i < op_array->last_var) {
		if (ZSTR_VAL(op_array->vars[i]) == ZSTR_VAL(name) ||
		    (ZSTR_H(op_array->vars[i]) == hash_value &&
		     zend_string_equal_content(op_array->vars[i], name))) {
			return (int)(zend_intptr_t)ZEND_CALL_VAR_NUM(NULL, i);
		}
		i++;
	}
	i = op_array->last_var;
	op_array->last_var++;
	if (op_array->last_var > CG(context).vars_size) {
		CG(context).vars_size += 16; /* FIXME */
		op_array->vars = erealloc(op_array->vars, CG(context).vars_size * sizeof(zend_string*));
	}

	op_array->vars[i] = zend_string_copy(name);
	return (int)(zend_intptr_t)ZEND_CALL_VAR_NUM(NULL, i);
}
/* }}} */

void zend_del_literal(zend_op_array *op_array, int n) /* {{{ */
{
	zval_ptr_dtor_nogc(CT_CONSTANT_EX(op_array, n));
	if (n + 1 == op_array->last_literal) {
		op_array->last_literal--;
	} else {
		ZVAL_UNDEF(CT_CONSTANT_EX(op_array, n));
	}
}
/* }}} */

static inline zend_string *zval_make_interned_string(zval *zv) /* {{{ */
{
	ZEND_ASSERT(Z_TYPE_P(zv) == IS_STRING);
	Z_STR_P(zv) = zend_new_interned_string(Z_STR_P(zv));
	if (ZSTR_IS_INTERNED(Z_STR_P(zv))) {
		Z_TYPE_FLAGS_P(zv) = 0;
	}
	return Z_STR_P(zv);
}

/* Common part of zend_add_literal and zend_append_individual_literal */
static inline void zend_insert_literal(zend_op_array *op_array, zval *zv, int literal_position) /* {{{ */
{
	if (Z_TYPE_P(zv) == IS_STRING) {
		zval_make_interned_string(zv);
	}
	ZVAL_COPY_VALUE(CT_CONSTANT_EX(op_array, literal_position), zv);
}
/* }}} */

/* Is used while compiling a function, using the context to keep track
   of an approximate size to avoid to relocate to often.
   Literals are truncated to actual size in the second compiler pass (pass_two()). */
int zend_add_literal(zend_op_array *op_array, zval *zv) /* {{{ */
{
	int i = op_array->last_literal;
	op_array->last_literal++;
	if (i >= CG(context).literals_size) {
		while (i >= CG(context).literals_size) {
			CG(context).literals_size += 16; /* FIXME */
		}
		op_array->literals = (zval*)erealloc(op_array->literals, CG(context).literals_size * sizeof(zval));
	}
	zend_insert_literal(op_array, zv, i);
	return i;
}
/* }}} */

static inline int zend_add_literal_string(zend_op_array *op_array, zend_string **str) /* {{{ */
{
	int ret;
	zval zv;
	ZVAL_STR(&zv, *str);
	ret = zend_add_literal(op_array, &zv);
	*str = Z_STR(zv);
	return ret;
}
/* }}} */

static int zend_add_func_name_literal(zend_op_array *op_array, zend_string *name) /* {{{ */
{
	/* Original name */
	int ret = zend_add_literal_string(op_array, &name);

	/* Lowercased name */
	zend_string *lc_name = zend_string_tolower(name);
	zend_add_literal_string(op_array, &lc_name);

	return ret;
}
/* }}} */

static int zend_add_ns_func_name_literal(zend_op_array *op_array, zend_string *name) /* {{{ */
{
	const char *unqualified_name;
	size_t unqualified_name_len;

	/* Original name */
	int ret = zend_add_literal_string(op_array, &name);

	/* Lowercased name */
	zend_string *lc_name = zend_string_tolower(name);
	zend_add_literal_string(op_array, &lc_name);

	/* Lowercased unqualfied name */
	if (zend_get_unqualified_name(name, &unqualified_name, &unqualified_name_len)) {
		lc_name = zend_string_alloc(unqualified_name_len, 0);
		zend_str_tolower_copy(ZSTR_VAL(lc_name), unqualified_name, unqualified_name_len);
		zend_add_literal_string(op_array, &lc_name);
	}

	return ret;
}
/* }}} */

static int zend_add_class_name_literal(zend_op_array *op_array, zend_string *name) /* {{{ */
{
	/* Original name */
	int ret = zend_add_literal_string(op_array, &name);

	/* Lowercased name */
	zend_string *lc_name = zend_string_tolower(name);
	zend_add_literal_string(op_array, &lc_name);

	return ret;
}
/* }}} */

static int zend_add_const_name_literal(zend_op_array *op_array, zend_string *name, zend_bool unqualified) /* {{{ */
{
	zend_string *tmp_name;

	int ret = zend_add_literal_string(op_array, &name);

	size_t ns_len = 0, after_ns_len = ZSTR_LEN(name);
	const char *after_ns = zend_memrchr(ZSTR_VAL(name), '\\', ZSTR_LEN(name));
	if (after_ns) {
		after_ns += 1;
		ns_len = after_ns - ZSTR_VAL(name) - 1;
		after_ns_len = ZSTR_LEN(name) - ns_len - 1;

		/* lowercased namespace name & original constant name */
		tmp_name = zend_string_init(ZSTR_VAL(name), ZSTR_LEN(name), 0);
		zend_str_tolower(ZSTR_VAL(tmp_name), ns_len);
		zend_add_literal_string(op_array, &tmp_name);

		/* lowercased namespace name & lowercased constant name */
		tmp_name = zend_string_tolower(name);
		zend_add_literal_string(op_array, &tmp_name);

		if (!unqualified) {
			return ret;
		}
	} else {
		after_ns = ZSTR_VAL(name);
	}

	/* original unqualified constant name */
	tmp_name = zend_string_init(after_ns, after_ns_len, 0);
	zend_add_literal_string(op_array, &tmp_name);

	/* lowercased unqualified constant name */
	tmp_name = zend_string_alloc(after_ns_len, 0);
	zend_str_tolower_copy(ZSTR_VAL(tmp_name), after_ns, after_ns_len);
	zend_add_literal_string(op_array, &tmp_name);

	return ret;
}
/* }}} */

#define LITERAL_STR(op, str) do { \
		zval _c; \
		ZVAL_STR(&_c, str); \
		op.constant = zend_add_literal(CG(active_op_array), &_c); \
	} while (0)

void zend_stop_lexing(void)
{
	if (LANG_SCNG(on_event)) {
		LANG_SCNG(on_event)(ON_STOP, END, 0, LANG_SCNG(on_event_context));
	}

	LANG_SCNG(yy_cursor) = LANG_SCNG(yy_limit);
}

static uint32_t zend_start_live_range(zend_op_array *op_array, uint32_t start) /* {{{ */
{
	zend_live_range *range;

	op_array->last_live_range++;
	op_array->live_range = erealloc(op_array->live_range, sizeof(zend_live_range) * op_array->last_live_range);
	range = op_array->live_range + op_array->last_live_range - 1;
	range->start = start;
	return op_array->last_live_range - 1;
}
/* }}} */

static void zend_end_live_range(zend_op_array *op_array, uint32_t offset, uint32_t end, uint32_t kind, uint32_t var) /* {{{ */
{
	zend_live_range *range = op_array->live_range + offset;

	if (range->start == end && offset == (uint32_t)op_array->last_live_range - 1) {
		op_array->last_live_range--;
	} else {
		range->end = end;
		range->var = (var * sizeof(zval)) | kind;
	}
}
/* }}} */

static inline void zend_begin_loop(
		zend_uchar free_opcode, const znode *loop_var, zend_bool is_switch) /* {{{ */
{
	zend_brk_cont_element *brk_cont_element;
	int parent = CG(context).current_brk_cont;
	zend_loop_var info = {0};

	CG(context).current_brk_cont = CG(context).last_brk_cont;
	brk_cont_element = get_next_brk_cont_element();
	brk_cont_element->parent = parent;
	brk_cont_element->is_switch = is_switch;

	if (loop_var && (loop_var->op_type & (IS_VAR|IS_TMP_VAR))) {
		uint32_t start = get_next_op_number(CG(active_op_array));

		info.opcode = free_opcode;
		info.var_type = loop_var->op_type;
		info.var_num = loop_var->u.op.var;
		info.u.live_range_offset = zend_start_live_range(CG(active_op_array), start);
		brk_cont_element->start = start;
	} else {
		info.opcode = ZEND_NOP;
		/* The start field is used to free temporary variables in case of exceptions.
		 * We won't try to free something of we don't have loop variable.  */
		brk_cont_element->start = -1;
	}

	zend_stack_push(&CG(loop_var_stack), &info);
}
/* }}} */

static inline void zend_end_loop(int cont_addr, const znode *var_node) /* {{{ */
{
	uint32_t end = get_next_op_number(CG(active_op_array));
	zend_brk_cont_element *brk_cont_element
		= &CG(context).brk_cont_array[CG(context).current_brk_cont];
	brk_cont_element->cont = cont_addr;
	brk_cont_element->brk = end;
	CG(context).current_brk_cont = brk_cont_element->parent;

	if (brk_cont_element->start != -1) {
		zend_loop_var *loop_var = zend_stack_top(&CG(loop_var_stack));
		zend_end_live_range(CG(active_op_array), loop_var->u.live_range_offset, end,
			loop_var->opcode == ZEND_FE_FREE ? ZEND_LIVE_LOOP : ZEND_LIVE_TMPVAR,
			var_node->u.op.var);
	}

	zend_stack_del_top(&CG(loop_var_stack));
}
/* }}} */

void zend_do_free(znode *op1) /* {{{ */
{
	if (op1->op_type == IS_TMP_VAR) {
		zend_op *opline = &CG(active_op_array)->opcodes[CG(active_op_array)->last-1];

		while (opline->opcode == ZEND_END_SILENCE) {
			opline--;
		}

		if (opline->result_type == IS_TMP_VAR && opline->result.var == op1->u.op.var) {
			if (opline->opcode == ZEND_BOOL || opline->opcode == ZEND_BOOL_NOT) {
				return;
			}
		}

		zend_emit_op(NULL, ZEND_FREE, op1, NULL);
	} else if (op1->op_type == IS_VAR) {
		zend_op *opline = &CG(active_op_array)->opcodes[CG(active_op_array)->last-1];
		while (opline->opcode == ZEND_END_SILENCE ||
				opline->opcode == ZEND_EXT_FCALL_END ||
				opline->opcode == ZEND_OP_DATA) {
			opline--;
		}
		if (opline->result_type == IS_VAR
			&& opline->result.var == op1->u.op.var) {
			if (opline->opcode == ZEND_FETCH_THIS) {
				opline->opcode = ZEND_NOP;
				opline->result_type = IS_UNUSED;
			} else {
				opline->result_type = IS_UNUSED;
			}
		} else {
			while (opline >= CG(active_op_array)->opcodes) {
				if ((opline->opcode == ZEND_FETCH_LIST_R ||
                     opline->opcode == ZEND_FETCH_LIST_W) &&
				    opline->op1_type == IS_VAR &&
				    opline->op1.var == op1->u.op.var) {
					zend_emit_op(NULL, ZEND_FREE, op1, NULL);
					return;
				}
				if (opline->result_type == IS_VAR
					&& opline->result.var == op1->u.op.var) {
					if (opline->opcode == ZEND_NEW) {
						zend_emit_op(NULL, ZEND_FREE, op1, NULL);
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

uint32_t zend_add_class_modifier(uint32_t flags, uint32_t new_flag) /* {{{ */
{
	uint32_t new_flags = flags | new_flag;
	if ((flags & ZEND_ACC_EXPLICIT_ABSTRACT_CLASS) && (new_flag & ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
		zend_throw_exception(zend_ce_compile_error,
			"Multiple abstract modifiers are not allowed", 0);
		return 0;
	}
	if ((flags & ZEND_ACC_FINAL) && (new_flag & ZEND_ACC_FINAL)) {
		zend_throw_exception(zend_ce_compile_error, "Multiple final modifiers are not allowed", 0);
		return 0;
	}
	if ((new_flags & ZEND_ACC_EXPLICIT_ABSTRACT_CLASS) && (new_flags & ZEND_ACC_FINAL)) {
		zend_throw_exception(zend_ce_compile_error,
			"Cannot use the final modifier on an abstract class", 0);
		return 0;
	}
	return new_flags;
}
/* }}} */

uint32_t zend_add_member_modifier(uint32_t flags, uint32_t new_flag) /* {{{ */
{
	uint32_t new_flags = flags | new_flag;
	if ((flags & ZEND_ACC_PPP_MASK) && (new_flag & ZEND_ACC_PPP_MASK)) {
		zend_throw_exception(zend_ce_compile_error,
			"Multiple access type modifiers are not allowed", 0);
		return 0;
	}
	if ((flags & ZEND_ACC_ABSTRACT) && (new_flag & ZEND_ACC_ABSTRACT)) {
		zend_throw_exception(zend_ce_compile_error, "Multiple abstract modifiers are not allowed", 0);
		return 0;
	}
	if ((flags & ZEND_ACC_STATIC) && (new_flag & ZEND_ACC_STATIC)) {
		zend_throw_exception(zend_ce_compile_error, "Multiple static modifiers are not allowed", 0);
		return 0;
	}
	if ((flags & ZEND_ACC_FINAL) && (new_flag & ZEND_ACC_FINAL)) {
		zend_throw_exception(zend_ce_compile_error, "Multiple final modifiers are not allowed", 0);
		return 0;
	}
	if ((new_flags & ZEND_ACC_ABSTRACT) && (new_flags & ZEND_ACC_FINAL)) {
		zend_throw_exception(zend_ce_compile_error,
			"Cannot use the final modifier on an abstract class member", 0);
		return 0;
	}
	return new_flags;
}
/* }}} */

zend_string *zend_concat3(char *str1, size_t str1_len, char *str2, size_t str2_len, char *str3, size_t str3_len) /* {{{ */
{
	size_t len = str1_len + str2_len + str3_len;
	zend_string *res = zend_string_alloc(len, 0);

	memcpy(ZSTR_VAL(res), str1, str1_len);
	memcpy(ZSTR_VAL(res) + str1_len, str2, str2_len);
	memcpy(ZSTR_VAL(res) + str1_len + str2_len, str3, str3_len);
	ZSTR_VAL(res)[len] = '\0';

	return res;
}

zend_string *zend_concat_names(char *name1, size_t name1_len, char *name2, size_t name2_len) {
	return zend_concat3(name1, name1_len, "\\", 1, name2, name2_len);
}

zend_string *zend_prefix_with_ns(zend_string *name) {
	if (FC(current_namespace)) {
		zend_string *ns = FC(current_namespace);
		return zend_concat_names(ZSTR_VAL(ns), ZSTR_LEN(ns), ZSTR_VAL(name), ZSTR_LEN(name));
	} else {
		return zend_string_copy(name);
	}
}

void *zend_hash_find_ptr_lc(HashTable *ht, const char *str, size_t len) {
	void *result;
	zend_string *lcname;
	ALLOCA_FLAG(use_heap);

	ZSTR_ALLOCA_ALLOC(lcname, len, use_heap);
	zend_str_tolower_copy(ZSTR_VAL(lcname), str, len);
	result = zend_hash_find_ptr(ht, lcname);
	ZSTR_ALLOCA_FREE(lcname, use_heap);

	return result;
}

zend_string *zend_resolve_non_class_name(
	zend_string *name, uint32_t type, zend_bool *is_fully_qualified,
	zend_bool case_sensitive, HashTable *current_import_sub
) {
	char *compound;
	*is_fully_qualified = 0;

	if (ZSTR_VAL(name)[0] == '\\') {
		/* Remove \ prefix (only relevant if this is a string rather than a label) */
		*is_fully_qualified = 1;
		return zend_string_init(ZSTR_VAL(name) + 1, ZSTR_LEN(name) - 1, 0);
	}

	if (type == ZEND_NAME_FQ) {
		*is_fully_qualified = 1;
		return zend_string_copy(name);
	}

	if (type == ZEND_NAME_RELATIVE) {
		*is_fully_qualified = 1;
		return zend_prefix_with_ns(name);
	}

	if (current_import_sub) {
		/* If an unqualified name is a function/const alias, replace it. */
		zend_string *import_name;
		if (case_sensitive) {
			import_name = zend_hash_find_ptr(current_import_sub, name);
		} else {
			import_name = zend_hash_find_ptr_lc(current_import_sub, ZSTR_VAL(name), ZSTR_LEN(name));
		}

		if (import_name) {
			*is_fully_qualified = 1;
			return zend_string_copy(import_name);
		}
	}

	compound = memchr(ZSTR_VAL(name), '\\', ZSTR_LEN(name));
	if (compound) {
		*is_fully_qualified = 1;
	}

	if (compound && FC(imports)) {
		/* If the first part of a qualified name is an alias, substitute it. */
		size_t len = compound - ZSTR_VAL(name);
		zend_string *import_name = zend_hash_find_ptr_lc(FC(imports), ZSTR_VAL(name), len);

		if (import_name) {
			return zend_concat_names(
				ZSTR_VAL(import_name), ZSTR_LEN(import_name), ZSTR_VAL(name) + len + 1, ZSTR_LEN(name) - len - 1);
		}
	}

	return zend_prefix_with_ns(name);
}
/* }}} */

zend_string *zend_resolve_function_name(zend_string *name, uint32_t type, zend_bool *is_fully_qualified) /* {{{ */
{
	return zend_resolve_non_class_name(
		name, type, is_fully_qualified, 0, FC(imports_function));
}
/* }}} */

zend_string *zend_resolve_const_name(zend_string *name, uint32_t type, zend_bool *is_fully_qualified) /* {{{ */ {
	return zend_resolve_non_class_name(
		name, type, is_fully_qualified, 1, FC(imports_const));
}
/* }}} */

zend_string *zend_resolve_class_name(zend_string *name, uint32_t type) /* {{{ */
{
	char *compound;

	if (type == ZEND_NAME_RELATIVE) {
		return zend_prefix_with_ns(name);
	}

	if (type == ZEND_NAME_FQ || ZSTR_VAL(name)[0] == '\\') {
		/* Remove \ prefix (only relevant if this is a string rather than a label) */
		if (ZSTR_VAL(name)[0] == '\\') {
			name = zend_string_init(ZSTR_VAL(name) + 1, ZSTR_LEN(name) - 1, 0);
		} else {
			zend_string_addref(name);
		}
		/* Ensure that \self, \parent and \static are not used */
		if (ZEND_FETCH_CLASS_DEFAULT != zend_get_class_fetch_type(name)) {
			zend_error_noreturn(E_COMPILE_ERROR, "'\\%s' is an invalid class name", ZSTR_VAL(name));
		}
		return name;
	}

	if (FC(imports)) {
		compound = memchr(ZSTR_VAL(name), '\\', ZSTR_LEN(name));
		if (compound) {
			/* If the first part of a qualified name is an alias, substitute it. */
			size_t len = compound - ZSTR_VAL(name);
			zend_string *import_name =
				zend_hash_find_ptr_lc(FC(imports), ZSTR_VAL(name), len);

			if (import_name) {
				return zend_concat_names(
					ZSTR_VAL(import_name), ZSTR_LEN(import_name), ZSTR_VAL(name) + len + 1, ZSTR_LEN(name) - len - 1);
			}
		} else {
			/* If an unqualified name is an alias, replace it. */
			zend_string *import_name
				= zend_hash_find_ptr_lc(FC(imports), ZSTR_VAL(name), ZSTR_LEN(name));

			if (import_name) {
				return zend_string_copy(import_name);
			}
		}
	}

	/* If not fully qualified and not an alias, prepend the current namespace */
	return zend_prefix_with_ns(name);
}
/* }}} */

zend_string *zend_resolve_class_name_ast(zend_ast *ast) /* {{{ */
{
	zval *class_name = zend_ast_get_zval(ast);
	if (Z_TYPE_P(class_name) != IS_STRING) {
		zend_error_noreturn(E_COMPILE_ERROR, "Illegal class name");
	}
	return zend_resolve_class_name(Z_STR_P(class_name), ast->attr);
}
/* }}} */

static void label_ptr_dtor(zval *zv) /* {{{ */
{
	efree_size(Z_PTR_P(zv), sizeof(zend_label));
}
/* }}} */

static void str_dtor(zval *zv)  /* {{{ */ {
	zend_string_release_ex(Z_STR_P(zv), 0);
}
/* }}} */

static zend_bool zend_is_call(zend_ast *ast);

static uint32_t zend_add_try_element(uint32_t try_op) /* {{{ */
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

		if (op_array->refcount) {
			(*op_array->refcount)++;
		}
		if (op_array->static_variables) {
			if (!(GC_FLAGS(op_array->static_variables) & IS_ARRAY_IMMUTABLE)) {
				GC_ADDREF(op_array->static_variables);
			}
		}
		op_array->run_time_cache = NULL;
	} else if (function->type == ZEND_INTERNAL_FUNCTION) {
		if (function->common.function_name) {
			zend_string_addref(function->common.function_name);
		}
	}
}
/* }}} */

ZEND_API int do_bind_function(const zend_op_array *op_array, const zend_op *opline, HashTable *function_table, zend_bool compile_time) /* {{{ */
{
	zend_function *function, *new_function;
	zval *lcname, *rtd_key, *zv;

	if (compile_time) {
		lcname = CT_CONSTANT_EX(op_array, opline->op1.constant);
		rtd_key = lcname + 1;
	} else {
		lcname = RT_CONSTANT(opline, opline->op1);
		rtd_key = lcname + 1;
	}

	zv = zend_hash_find_ex(function_table, Z_STR_P(rtd_key), 1);
	function = (zend_function*)Z_PTR_P(zv);
	new_function = zend_arena_alloc(&CG(arena), sizeof(zend_op_array));
	memcpy(new_function, function, sizeof(zend_op_array));
	if (zend_hash_add_ptr(function_table, Z_STR_P(lcname), new_function) == NULL) {
		int error_level = compile_time ? E_COMPILE_ERROR : E_ERROR;
		zend_function *old_function;

		zv = zend_hash_find_ex(function_table, Z_STR_P(lcname), 1);
		ZEND_ASSERT(zv != NULL);
		old_function = (zend_function*)Z_PTR_P(zv);
		if (old_function->type == ZEND_USER_FUNCTION
			&& old_function->op_array.last > 0) {
			zend_error_noreturn(error_level, "Cannot redeclare %s() (previously declared in %s:%d)",
						ZSTR_VAL(function->common.function_name),
						ZSTR_VAL(old_function->op_array.filename),
						old_function->op_array.opcodes[0].lineno);
		} else {
			zend_error_noreturn(error_level, "Cannot redeclare %s()", ZSTR_VAL(function->common.function_name));
		}
		return FAILURE;
	} else {
		if (function->op_array.refcount) {
			(*function->op_array.refcount)++;
		}
		if (!(function->op_array.fn_flags & ZEND_ACC_IMMUTABLE)) {
			function->op_array.static_variables = NULL; /* NULL out the unbound function */
		}
		return SUCCESS;
	}
}
/* }}} */

ZEND_API zend_class_entry *do_bind_class(const zend_op_array* op_array, const zend_op *opline, HashTable *class_table, zend_bool compile_time) /* {{{ */
{
	zend_class_entry *ce;
	zval *lcname, *rtd_key, *zv;

	if (compile_time) {
		lcname = CT_CONSTANT_EX(op_array, opline->op1.constant);
		rtd_key = lcname + 1;
	} else {
		lcname = RT_CONSTANT(opline, opline->op1);
		rtd_key = lcname + 1;
	}
	zv = zend_hash_find_ex(class_table, Z_STR_P(rtd_key), 1);
	ZEND_ASSERT(zv);
	ce = (zend_class_entry*)Z_PTR_P(zv);
	ce->refcount++;
	if (zend_hash_add_ptr(class_table, Z_STR_P(lcname), ce) == NULL) {
		ce->refcount--;
		if (!compile_time) {
			/* If we're in compile time, in practice, it's quite possible
			 * that we'll never reach this class declaration at runtime,
			 * so we shut up about it.  This allows the if (!defined('FOO')) { return; }
			 * approach to work.
			 */
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot declare %s %s, because the name is already in use", zend_get_object_type(ce), ZSTR_VAL(ce->name));
		}
		return NULL;
	} else {
		if (!(ce->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_IMPLEMENT_INTERFACES|ZEND_ACC_IMPLEMENT_TRAITS))) {
			zend_verify_abstract_class(ce);
		}
		return ce;
	}
}
/* }}} */

ZEND_API zend_class_entry *do_bind_inherited_class(const zend_op_array *op_array, const zend_op *opline, HashTable *class_table, zend_class_entry *parent_ce, zend_bool compile_time) /* {{{ */
{
	zend_class_entry *ce;
	zval *lcname, *rtd_key, *zv;

	if (compile_time) {
		lcname = CT_CONSTANT_EX(op_array, opline->op1.constant);
		rtd_key = lcname + 1;
	} else {
		lcname = RT_CONSTANT(opline, opline->op1);
		rtd_key = lcname + 1;
	}

	zv = zend_hash_find_ex(class_table, Z_STR_P(rtd_key), 1);

	if (!zv) {
		if (!compile_time) {
			/* If we're in compile time, in practice, it's quite possible
			 * that we'll never reach this class declaration at runtime,
			 * so we shut up about it.  This allows the if (!defined('FOO')) { return; }
			 * approach to work.
			 */
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot declare  %s, because the name is already in use", zend_get_object_type(Z_OBJCE_P(lcname)));
		}
		return NULL;
	}

	ce = (zend_class_entry*)Z_PTR_P(zv);

	if (zend_hash_exists(class_table, Z_STR_P(lcname))) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot declare %s %s, because the name is already in use", zend_get_object_type(ce), ZSTR_VAL(ce->name));
	}

	zend_do_inheritance(ce, parent_ce);

	ce->refcount++;

	/* Register the derived class */
	if (zend_hash_add_ptr(class_table, Z_STR_P(lcname), ce) == NULL) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot declare %s %s, because the name is already in use", zend_get_object_type(ce), ZSTR_VAL(ce->name));
	}
	return ce;
}
/* }}} */

void zend_do_early_binding(void) /* {{{ */
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
			if (do_bind_class(CG(active_op_array), opline, CG(class_table), 1) == NULL) {
				return;
			}
			table = CG(class_table);
			break;
		case ZEND_DECLARE_INHERITED_CLASS:
			{
				zval *parent_name;
				zend_class_entry *ce;

				parent_name = CT_CONSTANT(opline->op2);
				if (((ce = zend_lookup_class_ex(Z_STR_P(parent_name), parent_name + 1, 0)) == NULL) ||
				    ((CG(compiler_options) & ZEND_COMPILE_IGNORE_INTERNAL_CLASSES) &&
				     (ce->type == ZEND_INTERNAL_CLASS))) {
					if (CG(compiler_options) & ZEND_COMPILE_DELAYED_BINDING) {
						CG(active_op_array)->fn_flags |= ZEND_ACC_EARLY_BINDING;
						opline->opcode = ZEND_DECLARE_INHERITED_CLASS_DELAYED;
						opline->result_type = IS_UNUSED;
						opline->result.opline_num = -1;
					}
					return;
				}
				if (do_bind_inherited_class(CG(active_op_array), opline, CG(class_table), ce, 1) == NULL) {
					return;
				}
				zend_del_literal(CG(active_op_array), opline->op2.constant+1);
				zend_del_literal(CG(active_op_array), opline->op2.constant);

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

	zend_hash_del(table, Z_STR_P(CT_CONSTANT(opline->op1)+1));
	zend_del_literal(CG(active_op_array), opline->op1.constant+1);
	zend_del_literal(CG(active_op_array), opline->op1.constant);
	MAKE_NOP(opline);
}
/* }}} */

static void zend_mark_function_as_generator() /* {{{ */
{
	if (!CG(active_op_array)->function_name) {
		zend_error_noreturn(E_COMPILE_ERROR,
			"The \"yield\" expression can only be used inside a function");
	}

	if (CG(active_op_array)->fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
		zend_arg_info return_info = CG(active_op_array)->arg_info[-1];

		if (ZEND_TYPE_CODE(return_info.type) != IS_ITERABLE) {
			const char *msg = "Generators may only declare a return type of Generator, Iterator, Traversable, or iterable, %s is not permitted";

			if (!ZEND_TYPE_IS_CLASS(return_info.type)) {
				zend_error_noreturn(E_COMPILE_ERROR, msg, zend_get_type_by_const(ZEND_TYPE_CODE(return_info.type)));
			}

			if (!zend_string_equals_literal_ci(ZEND_TYPE_NAME(return_info.type), "Traversable")
				&& !zend_string_equals_literal_ci(ZEND_TYPE_NAME(return_info.type), "Iterator")
				&& !zend_string_equals_literal_ci(ZEND_TYPE_NAME(return_info.type), "Generator")) {
				zend_error_noreturn(E_COMPILE_ERROR, msg, ZSTR_VAL(ZEND_TYPE_NAME(return_info.type)));
			}
		}
	}

	CG(active_op_array)->fn_flags |= ZEND_ACC_GENERATOR;
}
/* }}} */

ZEND_API uint32_t zend_build_delayed_early_binding_list(const zend_op_array *op_array) /* {{{ */
{
	if (op_array->fn_flags & ZEND_ACC_EARLY_BINDING) {
		uint32_t  first_early_binding_opline = (uint32_t)-1;
		uint32_t *prev_opline_num = &first_early_binding_opline;
		zend_op  *opline = op_array->opcodes;
		zend_op  *end = opline + op_array->last;

		while (opline < end) {
			if (opline->opcode == ZEND_DECLARE_INHERITED_CLASS_DELAYED) {
				*prev_opline_num = opline - op_array->opcodes;
				prev_opline_num = &opline->result.opline_num;
			}
			++opline;
		}
		*prev_opline_num = -1;
		return first_early_binding_opline;
	}
	return (uint32_t)-1;
}
/* }}} */

ZEND_API void zend_do_delayed_early_binding(const zend_op_array *op_array, uint32_t first_early_binding_opline) /* {{{ */
{
	if (first_early_binding_opline != (uint32_t)-1) {
		zend_bool orig_in_compilation = CG(in_compilation);
		uint32_t opline_num = first_early_binding_opline;
		zend_class_entry *ce;

		CG(in_compilation) = 1;
		while (opline_num != (uint32_t)-1) {
			const zend_op *opline = &op_array->opcodes[opline_num];
			zval *parent_name = RT_CONSTANT(opline, opline->op2);
			if ((ce = zend_lookup_class_ex(Z_STR_P(parent_name), parent_name + 1, 0)) != NULL) {
				do_bind_inherited_class(op_array, &op_array->opcodes[opline_num], EG(class_table), ce, 0);
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

	ZSTR_VAL(prop_name)[0] = '\0';
	memcpy(ZSTR_VAL(prop_name) + 1, src1, src1_length+1);
	memcpy(ZSTR_VAL(prop_name) + 1 + src1_length + 1, src2, src2_length+1);
	return prop_name;
}
/* }}} */

static zend_always_inline size_t zend_strnlen(const char* s, size_t maxlen) /* {{{ */
{
	size_t len = 0;
	while (*s++ && maxlen--) len++;
	return len;
}
/* }}} */

ZEND_API int zend_unmangle_property_name_ex(const zend_string *name, const char **class_name, const char **prop_name, size_t *prop_len) /* {{{ */
{
	size_t class_name_len;
	size_t anonclass_src_len;

	*class_name = NULL;

	if (!ZSTR_LEN(name) || ZSTR_VAL(name)[0] != '\0') {
		*prop_name = ZSTR_VAL(name);
		if (prop_len) {
			*prop_len = ZSTR_LEN(name);
		}
		return SUCCESS;
	}
	if (ZSTR_LEN(name) < 3 || ZSTR_VAL(name)[1] == '\0') {
		zend_error(E_NOTICE, "Illegal member variable name");
		*prop_name = ZSTR_VAL(name);
		if (prop_len) {
			*prop_len = ZSTR_LEN(name);
		}
		return FAILURE;
	}

	class_name_len = zend_strnlen(ZSTR_VAL(name) + 1, ZSTR_LEN(name) - 2);
	if (class_name_len >= ZSTR_LEN(name) - 2 || ZSTR_VAL(name)[class_name_len + 1] != '\0') {
		zend_error(E_NOTICE, "Corrupt member variable name");
		*prop_name = ZSTR_VAL(name);
		if (prop_len) {
			*prop_len = ZSTR_LEN(name);
		}
		return FAILURE;
	}

	*class_name = ZSTR_VAL(name) + 1;
	anonclass_src_len = zend_strnlen(*class_name + class_name_len + 1, ZSTR_LEN(name) - class_name_len - 2);
	if (class_name_len + anonclass_src_len + 2 != ZSTR_LEN(name)) {
		class_name_len += anonclass_src_len + 1;
	}
	*prop_name = ZSTR_VAL(name) + class_name_len + 2;
	if (prop_len) {
		*prop_len = ZSTR_LEN(name) - class_name_len - 2;
	}
	return SUCCESS;
}
/* }}} */

static zend_constant *zend_lookup_reserved_const(const char *name, size_t len) /* {{{ */
{
	zend_constant *c = zend_hash_find_ptr_lc(EG(zend_constants), name, len);
	if (c && !(ZEND_CONSTANT_FLAGS(c) & CONST_CS) && (ZEND_CONSTANT_FLAGS(c) & CONST_CT_SUBST)) {
		return c;
	}
	return NULL;
}
/* }}} */

static zend_bool zend_try_ct_eval_const(zval *zv, zend_string *name, zend_bool is_fully_qualified) /* {{{ */
{
	zend_constant *c;

	/* Substitute case-sensitive (or lowercase) constants */
	c = zend_hash_find_ptr(EG(zend_constants), name);
	if (c && (
	      ((ZEND_CONSTANT_FLAGS(c) & CONST_PERSISTENT)
	      && !(CG(compiler_options) & ZEND_COMPILE_NO_PERSISTENT_CONSTANT_SUBSTITUTION)
	      && (!(ZEND_CONSTANT_FLAGS(c) & CONST_NO_FILE_CACHE) || !(CG(compiler_options) & ZEND_COMPILE_WITH_FILE_CACHE)))
	   || (Z_TYPE(c->value) < IS_OBJECT && !(CG(compiler_options) & ZEND_COMPILE_NO_CONSTANT_SUBSTITUTION))
	)) {
		ZVAL_COPY_OR_DUP(zv, &c->value);
		return 1;
	}

	{
		/* Substitute true, false and null (including unqualified usage in namespaces) */
		const char *lookup_name = ZSTR_VAL(name);
		size_t lookup_len = ZSTR_LEN(name);

		if (!is_fully_qualified) {
			zend_get_unqualified_name(name, &lookup_name, &lookup_len);
		}

		c = zend_lookup_reserved_const(lookup_name, lookup_len);
		if (c) {
			ZVAL_COPY_OR_DUP(zv, &c->value);
			return 1;
		}
	}

	return 0;
}
/* }}} */

static inline zend_bool zend_is_scope_known() /* {{{ */
{
	if (CG(active_op_array)->fn_flags & ZEND_ACC_CLOSURE) {
		/* Closures can be rebound to a different scope */
		return 0;
	}

	if (!CG(active_class_entry)) {
		/* The scope is known if we're in a free function (no scope), but not if we're in
		 * a file/eval (which inherits including/eval'ing scope). */
		return CG(active_op_array)->function_name != NULL;
	}

	/* For traits self etc refers to the using class, not the trait itself */
	return (CG(active_class_entry)->ce_flags & ZEND_ACC_TRAIT) == 0;
}
/* }}} */

static inline zend_bool class_name_refers_to_active_ce(zend_string *class_name, uint32_t fetch_type) /* {{{ */
{
	if (!CG(active_class_entry)) {
		return 0;
	}
	if (fetch_type == ZEND_FETCH_CLASS_SELF && zend_is_scope_known()) {
		return 1;
	}
	return fetch_type == ZEND_FETCH_CLASS_DEFAULT
		&& zend_string_equals_ci(class_name, CG(active_class_entry)->name);
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

static uint32_t zend_get_class_fetch_type_ast(zend_ast *name_ast) /* {{{ */
{
	/* Fully qualified names are always default refs */
	if (name_ast->attr == ZEND_NAME_FQ) {
		return ZEND_FETCH_CLASS_DEFAULT;
	}

	return zend_get_class_fetch_type(zend_ast_get_str(name_ast));
}
/* }}} */

static void zend_ensure_valid_class_fetch_type(uint32_t fetch_type) /* {{{ */
{
	if (fetch_type != ZEND_FETCH_CLASS_DEFAULT && !CG(active_class_entry) && zend_is_scope_known()) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use \"%s\" when no class scope is active",
			fetch_type == ZEND_FETCH_CLASS_SELF ? "self" :
			fetch_type == ZEND_FETCH_CLASS_PARENT ? "parent" : "static");
	}
}
/* }}} */

static zend_bool zend_try_compile_const_expr_resolve_class_name(zval *zv, zend_ast *class_ast, zend_ast *name_ast, zend_bool constant) /* {{{ */
{
	uint32_t fetch_type;

	if (name_ast->kind != ZEND_AST_ZVAL) {
		return 0;
	}

	if (!zend_string_equals_literal_ci(zend_ast_get_str(name_ast), "class")) {
		return 0;
	}

	if (class_ast->kind != ZEND_AST_ZVAL) {
		zend_error_noreturn(E_COMPILE_ERROR,
			"Dynamic class names are not allowed in compile-time ::class fetch");
	}

	fetch_type = zend_get_class_fetch_type(zend_ast_get_str(class_ast));
	zend_ensure_valid_class_fetch_type(fetch_type);

	switch (fetch_type) {
		case ZEND_FETCH_CLASS_SELF:
			if (constant || (CG(active_class_entry) && zend_is_scope_known())) {
				ZVAL_STR_COPY(zv, CG(active_class_entry)->name);
			} else {
				ZVAL_NULL(zv);
			}
			return 1;
		case ZEND_FETCH_CLASS_STATIC:
		case ZEND_FETCH_CLASS_PARENT:
			if (constant) {
				zend_error_noreturn(E_COMPILE_ERROR,
					"%s::class cannot be used for compile-time class name resolution",
					fetch_type == ZEND_FETCH_CLASS_STATIC ? "static" : "parent"
				);
			} else {
				ZVAL_NULL(zv);
			}
			return 1;
		case ZEND_FETCH_CLASS_DEFAULT:
			ZVAL_STR(zv, zend_resolve_class_name_ast(class_ast));
			return 1;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

static zend_bool zend_try_ct_eval_class_const(zval *zv, zend_string *class_name, zend_string *name) /* {{{ */
{
	uint32_t fetch_type = zend_get_class_fetch_type(class_name);
	zend_class_constant *cc;
	zval *c;

	if (class_name_refers_to_active_ce(class_name, fetch_type)) {
		cc = zend_hash_find_ptr(&CG(active_class_entry)->constants_table, name);
	} else if (fetch_type == ZEND_FETCH_CLASS_DEFAULT && !(CG(compiler_options) & ZEND_COMPILE_NO_CONSTANT_SUBSTITUTION)) {
		zend_class_entry *ce = zend_hash_find_ptr_lc(CG(class_table), ZSTR_VAL(class_name), ZSTR_LEN(class_name));
		if (ce) {
			cc = zend_hash_find_ptr(&ce->constants_table, name);
		} else {
			return 0;
		}
	} else {
		return 0;
	}

	if (CG(compiler_options) & ZEND_COMPILE_NO_PERSISTENT_CONSTANT_SUBSTITUTION) {
		return 0;
	}

	if (!cc || !zend_verify_const_access(cc, CG(active_class_entry))) {
		return 0;
	}

	c = &cc->value;

	/* Substitute case-sensitive (or lowercase) persistent class constants */
	if (Z_TYPE_P(c) < IS_OBJECT) {
		ZVAL_COPY_OR_DUP(zv, c);
		return 1;
	}

	return 0;
}
/* }}} */

static void zend_add_to_list(void *result, void *item) /* {{{ */
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

void zend_do_extended_info(void) /* {{{ */
{
	zend_op *opline;

	if (!(CG(compiler_options) & ZEND_COMPILE_EXTENDED_INFO)) {
		return;
	}

	opline = get_next_op(CG(active_op_array));

	opline->opcode = ZEND_EXT_STMT;
}
/* }}} */

void zend_do_extended_fcall_begin(void) /* {{{ */
{
	zend_op *opline;

	if (!(CG(compiler_options) & ZEND_COMPILE_EXTENDED_INFO)) {
		return;
	}

	opline = get_next_op(CG(active_op_array));

	opline->opcode = ZEND_EXT_FCALL_BEGIN;
}
/* }}} */

void zend_do_extended_fcall_end(void) /* {{{ */
{
	zend_op *opline;

	if (!(CG(compiler_options) & ZEND_COMPILE_EXTENDED_INFO)) {
		return;
	}

	opline = get_next_op(CG(active_op_array));

	opline->opcode = ZEND_EXT_FCALL_END;
}
/* }}} */

zend_bool zend_is_auto_global_str(char *name, size_t len) /* {{{ */ {
	zend_auto_global *auto_global;

	if ((auto_global = zend_hash_str_find_ptr(CG(auto_globals), name, len)) != NULL) {
		if (auto_global->armed) {
			auto_global->armed = auto_global->auto_global_callback(auto_global->name);
		}
		return 1;
	}
	return 0;
}
/* }}} */

zend_bool zend_is_auto_global(zend_string *name) /* {{{ */
{
	zend_auto_global *auto_global;

	if ((auto_global = zend_hash_find_ptr(CG(auto_globals), name)) != NULL) {
		if (auto_global->armed) {
			auto_global->armed = auto_global->auto_global_callback(auto_global->name);
		}
		return 1;
	}
	return 0;
}
/* }}} */

int zend_register_auto_global(zend_string *name, zend_bool jit, zend_auto_global_callback auto_global_callback) /* {{{ */
{
	zend_auto_global auto_global;
	int retval;

	auto_global.name = name;
	auto_global.auto_global_callback = auto_global_callback;
	auto_global.jit = jit;

	retval = zend_hash_add_mem(CG(auto_globals), auto_global.name, &auto_global, sizeof(zend_auto_global)) != NULL ? SUCCESS : FAILURE;

	return retval;
}
/* }}} */

ZEND_API void zend_activate_auto_globals(void) /* {{{ */
{
	zend_auto_global *auto_global;

	ZEND_HASH_FOREACH_PTR(CG(auto_globals), auto_global) {
		if (auto_global->jit) {
			auto_global->armed = 1;
		} else if (auto_global->auto_global_callback) {
			auto_global->armed = auto_global->auto_global_callback(auto_global->name);
		} else {
			auto_global->armed = 0;
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

int ZEND_FASTCALL zendlex(zend_parser_stack_elem *elem) /* {{{ */
{
	zval zv;

	if (CG(increment_lineno)) {
		CG(zend_lineno)++;
		CG(increment_lineno) = 0;
	}

	return lex_scan(&zv, elem);
}
/* }}} */

ZEND_API void zend_initialize_class_data(zend_class_entry *ce, zend_bool nullify_handlers) /* {{{ */
{
	zend_bool persistent_hashes = (ce->type == ZEND_INTERNAL_CLASS) ? 1 : 0;

	ce->refcount = 1;
	ce->ce_flags = ZEND_ACC_CONSTANTS_UPDATED;

	if (CG(compiler_options) & ZEND_COMPILE_GUARDS) {
		ce->ce_flags |= ZEND_ACC_USE_GUARDS;
	}

	ce->default_properties_table = NULL;
	ce->default_static_members_table = NULL;
	zend_hash_init_ex(&ce->properties_info, 8, NULL, (persistent_hashes ? zend_destroy_property_info_internal : NULL), persistent_hashes, 0);
	zend_hash_init_ex(&ce->constants_table, 8, NULL, NULL, persistent_hashes, 0);
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
		ce->iterator_funcs_ptr = NULL;
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
	size_t left_len = ZSTR_LEN(left);
	size_t len = left_len + ZSTR_LEN(right) + 1; /* left\right */

	result = zend_string_extend(left, len, 0);
	ZSTR_VAL(result)[left_len] = '\\';
	memcpy(&ZSTR_VAL(result)[left_len + 1], ZSTR_VAL(right), ZSTR_LEN(right));
	ZSTR_VAL(result)[len] = '\0';
	zend_string_release_ex(right, 0);

	ZVAL_STR(left_zv, result);
	return left_ast;
}
/* }}} */

zend_ast *zend_negate_num_string(zend_ast *ast) /* {{{ */
{
	zval *zv = zend_ast_get_zval(ast);
	if (Z_TYPE_P(zv) == IS_LONG) {
		if (Z_LVAL_P(zv) == 0) {
			ZVAL_NEW_STR(zv, zend_string_init("-0", sizeof("-0")-1, 0));
		} else {
			ZEND_ASSERT(Z_LVAL_P(zv) > 0);
			Z_LVAL_P(zv) *= -1;
		}
	} else if (Z_TYPE_P(zv) == IS_STRING) {
		size_t orig_len = Z_STRLEN_P(zv);
		Z_STR_P(zv) = zend_string_extend(Z_STR_P(zv), orig_len + 1, 0);
		memmove(Z_STRVAL_P(zv) + 1, Z_STRVAL_P(zv), orig_len + 1);
		Z_STRVAL_P(zv)[0] = '-';
	} else {
		ZEND_ASSERT(0);
	}
	return ast;
}
/* }}} */

void zend_verify_namespace(void) /* {{{ */
{
	if (FC(has_bracketed_namespaces) && !FC(in_namespace)) {
		zend_error_noreturn(E_COMPILE_ERROR, "No code may exist outside of namespace {}");
	}
}
/* }}} */

/* {{{ zend_dirname
   Returns directory name component of path */
ZEND_API size_t zend_dirname(char *path, size_t len)
{
	register char *end = path + len - 1;
	unsigned int len_adjust = 0;

#ifdef ZEND_WIN32
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
		path[0] = '.';
		path[1] = '\0';
		return 1 + len_adjust;
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

static void zend_adjust_for_fetch_type(zend_op *opline, znode *result, uint32_t type) /* {{{ */
{
	zend_uchar factor = (opline->opcode == ZEND_FETCH_STATIC_PROP_R) ? 1 : 3;

	switch (type) {
		case BP_VAR_R:
			opline->result_type = IS_TMP_VAR;
			result->op_type = IS_TMP_VAR;
			return;
		case BP_VAR_W:
			opline->opcode += 1 * factor;
			return;
		case BP_VAR_RW:
			opline->opcode += 2 * factor;
			return;
		case BP_VAR_IS:
			opline->result_type = IS_TMP_VAR;
			result->op_type = IS_TMP_VAR;
			opline->opcode += 3 * factor;
			return;
		case BP_VAR_FUNC_ARG:
			opline->opcode += 4 * factor;
			return;
		case BP_VAR_UNSET:
			opline->opcode += 5 * factor;
			return;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

static inline void zend_make_var_result(znode *result, zend_op *opline) /* {{{ */
{
	opline->result_type = IS_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	GET_NODE(result, opline->result);
}
/* }}} */

static inline void zend_make_tmp_result(znode *result, zend_op *opline) /* {{{ */
{
	opline->result_type = IS_TMP_VAR;
	opline->result.var = get_temporary_variable(CG(active_op_array));
	GET_NODE(result, opline->result);
}
/* }}} */

static void zend_find_live_range(zend_op *opline, zend_uchar type, uint32_t var) /* {{{ */
{
	zend_op *def = opline;

	while (def != CG(active_op_array)->opcodes) {
		def--;
		if (def->result_type == type && def->result.var == var) {
			if (def->opcode == ZEND_ADD_ARRAY_ELEMENT ||
			    def->opcode == ZEND_ROPE_ADD) {
			    /* not a real definition */
				continue;
			} else if (def->opcode == ZEND_JMPZ_EX ||
			           def->opcode == ZEND_JMPNZ_EX ||
			           def->opcode == ZEND_BOOL ||
			           def->opcode == ZEND_BOOL_NOT) {
				/* result IS_BOOL, it does't have to be destroyed */
				break;
			} else if (def->opcode == ZEND_DECLARE_CLASS ||
			           def->opcode == ZEND_DECLARE_INHERITED_CLASS ||
			           def->opcode == ZEND_DECLARE_INHERITED_CLASS_DELAYED ||
			           def->opcode == ZEND_DECLARE_ANON_CLASS ||
			           def->opcode == ZEND_DECLARE_ANON_INHERITED_CLASS) {
				/* classes don't have to be destroyed */
				break;
			} else if (def->opcode == ZEND_FAST_CALL) {
				/* fast_calls don't have to be destroyed */
				break;
			} else if (def->opcode == ZEND_NEW) {
				/* Objects created via ZEND_NEW are only fully initialized
				 * after the DO_FCALL (constructor call) */
				int level = 0;
				while (def + 1 != opline) {
					def++;
					if (def->opcode == ZEND_DO_FCALL) {
						if (level == 0) {
							break;
						}
						level--;
					} else {
						switch(def->opcode) {
							case ZEND_INIT_FCALL:
							case ZEND_INIT_FCALL_BY_NAME:
							case ZEND_INIT_NS_FCALL_BY_NAME:
							case ZEND_INIT_DYNAMIC_CALL:
							case ZEND_INIT_USER_CALL:
							case ZEND_INIT_METHOD_CALL:
							case ZEND_INIT_STATIC_METHOD_CALL:
							case ZEND_NEW:
								level++;
								break;
							case ZEND_DO_ICALL:
							case ZEND_DO_UCALL:
							case ZEND_DO_FCALL_BY_NAME:
								level--;
								break;
						}
					}
				}
				if (def + 1 == opline) {
					break;
				}
			}

	        zend_end_live_range(CG(active_op_array),
				zend_start_live_range(CG(active_op_array),
					def + 1 - CG(active_op_array)->opcodes),
				opline - CG(active_op_array)->opcodes,
				ZEND_LIVE_TMPVAR, var);
		    break;
		}
	}
}
/* }}} */

static zend_always_inline int zend_is_def_range(zend_op *opline, zend_uchar type, uint32_t var) /* {{{ */
{
	while (1) {
		if (opline->result_type == type && opline->result.var == var) {
			return opline->opcode != ZEND_ADD_ARRAY_ELEMENT &&
				opline->opcode != ZEND_ROPE_ADD;
		} else if (opline->opcode == ZEND_OP_DATA) {
			return (opline-1)->result_type == type &&
				(opline-1)->result.var == var;
		} else if (opline->opcode == ZEND_END_SILENCE ||
		           opline->opcode == ZEND_NOP ||
	    	       opline->opcode == ZEND_EXT_NOP ||
	        	   opline->opcode == ZEND_EXT_STMT ||
	        	   opline->opcode == ZEND_EXT_FCALL_BEGIN ||
	        	   opline->opcode == ZEND_EXT_FCALL_END ||
	        	   opline->opcode == ZEND_TICKS) {
			opline--;
		} else {
			return 0;
		}
	}
}
/* }}} */

static void zend_check_live_ranges(zend_op *opline) /* {{{ */
{
	if ((opline->op1_type & (IS_VAR|IS_TMP_VAR)) &&
		!zend_is_def_range(opline - 1, opline->op1_type, opline->op1.var)) {

		if (opline->opcode == ZEND_OP_DATA) {
			if (!zend_is_def_range(opline - 2, opline->op1_type, opline->op1.var)) {
				zend_find_live_range(opline - 1, opline->op1_type, opline->op1.var);
			}
		} else if (opline->opcode == ZEND_INIT_STATIC_METHOD_CALL ||
		           opline->opcode == ZEND_NEW ||
		           opline->opcode == ZEND_FETCH_CLASS_CONSTANT ||
		           opline->opcode == ZEND_ADD_INTERFACE ||
		           opline->opcode == ZEND_ADD_TRAIT ||
		           opline->opcode == ZEND_BIND_TRAITS ||
		           opline->opcode == ZEND_VERIFY_ABSTRACT_CLASS) {
			/* classes don't have to be destroyed */
		} else if (opline->opcode == ZEND_FAST_RET) {
			/* fast_calls don't have to be destroyed */
		} else if (opline->opcode == ZEND_CASE ||
		           opline->opcode == ZEND_SWITCH_LONG ||
		           opline->opcode == ZEND_SWITCH_STRING ||
		           opline->opcode == ZEND_FE_FETCH_R ||
		           opline->opcode == ZEND_FE_FETCH_RW ||
			       opline->opcode == ZEND_FE_FREE ||
			       opline->opcode == ZEND_ROPE_ADD ||
			       opline->opcode == ZEND_ROPE_END ||
			       opline->opcode == ZEND_END_SILENCE ||
			       opline->opcode == ZEND_FETCH_LIST_R ||
			       opline->opcode == ZEND_FETCH_LIST_W ||
			       opline->opcode == ZEND_VERIFY_RETURN_TYPE ||
			       opline->opcode == ZEND_BIND_LEXICAL) {
			/* these opcodes are handled separately */
		} else {
			zend_find_live_range(opline, opline->op1_type, opline->op1.var);
		}
	}

	if ((opline->op2_type & (IS_VAR|IS_TMP_VAR)) &&
		!zend_is_def_range(opline - 1, opline->op2_type, opline->op2.var)) {

		if (opline->opcode == ZEND_OP_DATA) {
			if (!zend_is_def_range(opline - 2, opline->op2_type, opline->op2.var)) {
				zend_find_live_range(opline-1, opline->op2_type, opline->op2.var);
			}
		} else if (opline->opcode == ZEND_FETCH_STATIC_PROP_R ||
		           opline->opcode == ZEND_FETCH_STATIC_PROP_W ||
		           opline->opcode == ZEND_FETCH_STATIC_PROP_RW ||
		           opline->opcode == ZEND_FETCH_STATIC_PROP_IS ||
		           opline->opcode == ZEND_FETCH_STATIC_PROP_FUNC_ARG ||
		           opline->opcode == ZEND_FETCH_STATIC_PROP_UNSET ||
		           opline->opcode == ZEND_UNSET_STATIC_PROP ||
		           opline->opcode == ZEND_ISSET_ISEMPTY_STATIC_PROP ||
		           opline->opcode == ZEND_INSTANCEOF) {
			/* classes don't have to be destroyed */
		} else {
			zend_find_live_range(opline, opline->op2_type, opline->op2.var);
		}
	}
}
/* }}} */

static zend_op *zend_emit_op(znode *result, zend_uchar opcode, znode *op1, znode *op2) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array));
	opline->opcode = opcode;

	if (op1 != NULL) {
		SET_NODE(opline->op1, op1);
	}

	if (op2 != NULL) {
		SET_NODE(opline->op2, op2);
	}

	zend_check_live_ranges(opline);

	if (result) {
		zend_make_var_result(result, opline);
	}
	return opline;
}
/* }}} */

static zend_op *zend_emit_op_tmp(znode *result, zend_uchar opcode, znode *op1, znode *op2) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array));
	opline->opcode = opcode;

	if (op1 != NULL) {
		SET_NODE(opline->op1, op1);
	}

	if (op2 != NULL) {
		SET_NODE(opline->op2, op2);
	}

	zend_check_live_ranges(opline);

	if (result) {
		zend_make_tmp_result(result, opline);
	}

	return opline;
}
/* }}} */

static void zend_emit_tick(void) /* {{{ */
{
	zend_op *opline;

	/* This prevents a double TICK generated by the parser statement of "declare()" */
	if (CG(active_op_array)->last && CG(active_op_array)->opcodes[CG(active_op_array)->last - 1].opcode == ZEND_TICKS) {
		return;
	}

	opline = get_next_op(CG(active_op_array));

	opline->opcode = ZEND_TICKS;
	opline->extended_value = FC(declarables).ticks;
}
/* }}} */

static inline zend_op *zend_emit_op_data(znode *value) /* {{{ */
{
	return zend_emit_op(NULL, ZEND_OP_DATA, value, NULL);
}
/* }}} */

static inline uint32_t zend_emit_jump(uint32_t opnum_target) /* {{{ */
{
	uint32_t opnum = get_next_op_number(CG(active_op_array));
	zend_op *opline = zend_emit_op(NULL, ZEND_JMP, NULL, NULL);
	opline->op1.opline_num = opnum_target;
	return opnum;
}
/* }}} */

ZEND_API int zend_is_smart_branch(zend_op *opline) /* {{{ */
{
	switch (opline->opcode) {
		case ZEND_IS_IDENTICAL:
		case ZEND_IS_NOT_IDENTICAL:
		case ZEND_IS_EQUAL:
		case ZEND_IS_NOT_EQUAL:
		case ZEND_IS_SMALLER:
		case ZEND_IS_SMALLER_OR_EQUAL:
		case ZEND_CASE:
		case ZEND_ISSET_ISEMPTY_CV:
		case ZEND_ISSET_ISEMPTY_VAR:
		case ZEND_ISSET_ISEMPTY_DIM_OBJ:
		case ZEND_ISSET_ISEMPTY_PROP_OBJ:
		case ZEND_ISSET_ISEMPTY_STATIC_PROP:
		case ZEND_INSTANCEOF:
		case ZEND_TYPE_CHECK:
		case ZEND_DEFINED:
		case ZEND_IN_ARRAY:
			return 1;
		default:
			return 0;
	}
}
/* }}} */

static inline uint32_t zend_emit_cond_jump(zend_uchar opcode, znode *cond, uint32_t opnum_target) /* {{{ */
{
	uint32_t opnum = get_next_op_number(CG(active_op_array));
	zend_op *opline;

	if ((cond->op_type & (IS_CV|IS_CONST))
	 && opnum > 0
	 && zend_is_smart_branch(CG(active_op_array)->opcodes + opnum - 1)) {
		/* emit extra NOP to avoid incorrect SMART_BRANCH in very rare cases */
		zend_emit_op(NULL, ZEND_NOP, NULL, NULL);
		opnum = get_next_op_number(CG(active_op_array));
	}
	opline = zend_emit_op(NULL, opcode, cond, NULL);
	opline->op2.opline_num = opnum_target;
	return opnum;
}
/* }}} */

static inline void zend_update_jump_target(uint32_t opnum_jump, uint32_t opnum_target) /* {{{ */
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
		case ZEND_JMP_SET:
			opline->op2.opline_num = opnum_target;
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

static inline void zend_update_jump_target_to_next(uint32_t opnum_jump) /* {{{ */
{
	zend_update_jump_target(opnum_jump, get_next_op_number(CG(active_op_array)));
}
/* }}} */

static inline zend_op *zend_delayed_emit_op(znode *result, zend_uchar opcode, znode *op1, znode *op2) /* {{{ */
{
	zend_op tmp_opline;

	init_op(&tmp_opline);

	tmp_opline.opcode = opcode;
	if (op1 != NULL) {
		SET_NODE(tmp_opline.op1, op1);
	}
	if (op2 != NULL) {
		SET_NODE(tmp_opline.op2, op2);
	}
	if (result) {
		zend_make_var_result(result, &tmp_opline);
	}

	zend_stack_push(&CG(delayed_oplines_stack), &tmp_opline);
	return zend_stack_top(&CG(delayed_oplines_stack));
}
/* }}} */

static inline uint32_t zend_delayed_compile_begin(void) /* {{{ */
{
	return zend_stack_count(&CG(delayed_oplines_stack));
}
/* }}} */

static zend_op *zend_delayed_compile_end(uint32_t offset) /* {{{ */
{
	zend_op *opline = NULL, *oplines = zend_stack_base(&CG(delayed_oplines_stack));
	uint32_t i, count = zend_stack_count(&CG(delayed_oplines_stack));

	ZEND_ASSERT(count >= offset);
	for (i = offset; i < count; ++i) {
		opline = get_next_op(CG(active_op_array));
		memcpy(opline, &oplines[i], sizeof(zend_op));
		zend_check_live_ranges(opline);
	}
	CG(delayed_oplines_stack).top = offset;
	return opline;
}
/* }}} */

static void zend_emit_return_type_check(
		znode *expr, zend_arg_info *return_info, zend_bool implicit) /* {{{ */
{
	if (ZEND_TYPE_IS_SET(return_info->type)) {
		zend_op *opline;

		/* `return ...;` is illegal in a void function (but `return;` isn't) */
		if (ZEND_TYPE_CODE(return_info->type) == IS_VOID) {
			if (expr) {
				if (expr->op_type == IS_CONST && Z_TYPE(expr->u.constant) == IS_NULL) {
					zend_error_noreturn(E_COMPILE_ERROR,
						"A void function must not return a value "
						"(did you mean \"return;\" instead of \"return null;\"?)");
				} else {
					zend_error_noreturn(E_COMPILE_ERROR, "A void function must not return a value");
				}
			}
			/* we don't need run-time check */
			return;
		}

		if (!expr && !implicit) {
			if (ZEND_TYPE_ALLOW_NULL(return_info->type)) {
				zend_error_noreturn(E_COMPILE_ERROR,
					"A function with return type must return a value "
					"(did you mean \"return null;\" instead of \"return;\"?)");
			} else {
				zend_error_noreturn(E_COMPILE_ERROR,
					"A function with return type must return a value");
			}
		}

		if (expr && expr->op_type == IS_CONST) {
			if ((ZEND_TYPE_CODE(return_info->type) == Z_TYPE(expr->u.constant))
			 ||((ZEND_TYPE_CODE(return_info->type) == _IS_BOOL)
			  && (Z_TYPE(expr->u.constant) == IS_FALSE
			   || Z_TYPE(expr->u.constant) == IS_TRUE))
			 || (ZEND_TYPE_ALLOW_NULL(return_info->type)
			  && Z_TYPE(expr->u.constant) == IS_NULL)) {
				/* we don't need run-time check */
				return;
			}
		}

		opline = zend_emit_op(NULL, ZEND_VERIFY_RETURN_TYPE, expr, NULL);
		if (expr && expr->op_type == IS_CONST) {
			opline->result_type = expr->op_type = IS_TMP_VAR;
			opline->result.var = expr->u.op.var = get_temporary_variable(CG(active_op_array));
		}
		if (ZEND_TYPE_IS_CLASS(return_info->type)) {
			opline->op2.num = CG(active_op_array)->cache_size;
			CG(active_op_array)->cache_size += sizeof(void*);
		} else {
			opline->op2.num = -1;
		}
	}
}
/* }}} */

void zend_emit_final_return(int return_one) /* {{{ */
{
	znode zn;
	zend_op *ret;
	zend_bool returns_reference = (CG(active_op_array)->fn_flags & ZEND_ACC_RETURN_REFERENCE) != 0;

	if (CG(active_op_array)->fn_flags & ZEND_ACC_HAS_RETURN_TYPE
			&& !(CG(active_op_array)->fn_flags & ZEND_ACC_GENERATOR)) {
		zend_emit_return_type_check(NULL, CG(active_op_array)->arg_info - 1, 1);
	}

	zn.op_type = IS_CONST;
	if (return_one) {
		ZVAL_LONG(&zn.u.constant, 1);
	} else {
		ZVAL_NULL(&zn.u.constant);
	}

	ret = zend_emit_op(NULL, returns_reference ? ZEND_RETURN_BY_REF : ZEND_RETURN, &zn, NULL);
	ret->extended_value = -1;
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
	return ast->kind == ZEND_AST_STMT_LIST || ast->kind == ZEND_AST_LABEL
		|| ast->kind == ZEND_AST_PROP_DECL || ast->kind == ZEND_AST_CLASS_CONST_DECL
		|| ast->kind == ZEND_AST_USE_TRAIT || ast->kind == ZEND_AST_METHOD;
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
	if (name_ast->kind != ZEND_AST_ZVAL) {
		return 0;
	}

	return ZEND_FETCH_CLASS_DEFAULT == zend_get_class_fetch_type_ast(name_ast);
}
/* }}} */

static inline void zend_handle_numeric_op(znode *node) /* {{{ */
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

static inline void zend_handle_numeric_dim(zend_op *opline, znode *dim_node) /* {{{ */
{
	if (Z_TYPE(dim_node->u.constant) == IS_STRING) {
		zend_ulong index;

		if (ZEND_HANDLE_NUMERIC(Z_STR(dim_node->u.constant), index)) {
			/* For numeric indexs we also keep the original value to use by ArrayAccess
			 * See bug #63217
			 */
			int c = zend_add_literal(CG(active_op_array), &dim_node->u.constant);
			ZEND_ASSERT(opline->op2.constant + 1 == c);
			ZVAL_LONG(CT_CONSTANT(opline->op2), index);
			Z_EXTRA_P(CT_CONSTANT(opline->op2)) = ZEND_EXTRA_VALUE;
			return;
		}
	}
	Z_EXTRA_P(CT_CONSTANT(opline->op2)) = 0;
}
/* }}} */

static inline void zend_set_class_name_op1(zend_op *opline, znode *class_node) /* {{{ */
{
	if (class_node->op_type == IS_CONST) {
		opline->op1_type = IS_CONST;
		opline->op1.constant = zend_add_class_name_literal(
			CG(active_op_array), Z_STR(class_node->u.constant));
	} else {
		SET_NODE(opline->op1, class_node);
	}
}
/* }}} */

static void zend_compile_class_ref_ex(znode *result, zend_ast *name_ast, uint32_t fetch_flags) /* {{{ */
{
	uint32_t fetch_type;

	if (name_ast->kind != ZEND_AST_ZVAL) {
		znode name_node;

		zend_compile_expr(&name_node, name_ast);

		if (name_node.op_type == IS_CONST) {
			zend_string *name;

			if (Z_TYPE(name_node.u.constant) != IS_STRING) {
				zend_error_noreturn(E_COMPILE_ERROR, "Illegal class name");
			}

			name = Z_STR(name_node.u.constant);
			fetch_type = zend_get_class_fetch_type(name);

			if (fetch_type == ZEND_FETCH_CLASS_DEFAULT) {
				result->op_type = IS_CONST;
				ZVAL_STR(&result->u.constant, zend_resolve_class_name(name, ZEND_NAME_FQ));
			} else {
				zend_ensure_valid_class_fetch_type(fetch_type);
				result->op_type = IS_UNUSED;
				result->u.op.num = fetch_type | fetch_flags;
			}

			zend_string_release_ex(name, 0);
		} else {
			zend_op *opline = zend_emit_op(result, ZEND_FETCH_CLASS, NULL, &name_node);
			opline->op1.num = ZEND_FETCH_CLASS_DEFAULT | fetch_flags;
		}
		return;
	}

	/* Fully qualified names are always default refs */
	if (name_ast->attr == ZEND_NAME_FQ) {
		result->op_type = IS_CONST;
		ZVAL_STR(&result->u.constant, zend_resolve_class_name_ast(name_ast));
		return;
	}

	fetch_type = zend_get_class_fetch_type(zend_ast_get_str(name_ast));
	if (ZEND_FETCH_CLASS_DEFAULT == fetch_type) {
		result->op_type = IS_CONST;
		ZVAL_STR(&result->u.constant, zend_resolve_class_name_ast(name_ast));
	} else {
		zend_ensure_valid_class_fetch_type(fetch_type);
		result->op_type = IS_UNUSED;
		result->u.op.num = fetch_type | fetch_flags;
	}
}
/* }}} */

static int zend_try_compile_cv(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *name_ast = ast->child[0];
	if (name_ast->kind == ZEND_AST_ZVAL) {
		zval *zv = zend_ast_get_zval(name_ast);
		zend_string *name;

		if (EXPECTED(Z_TYPE_P(zv) == IS_STRING)) {
			name = zval_make_interned_string(zv);
		} else {
			name = zend_new_interned_string(zval_get_string_func(zv));
		}

		if (zend_is_auto_global(name)) {
			return FAILURE;
		}

		result->op_type = IS_CV;
		result->u.op.var = lookup_cv(CG(active_op_array), name);

		if (UNEXPECTED(Z_TYPE_P(zv) != IS_STRING)) {
			zend_string_release_ex(name, 0);
		}

		return SUCCESS;
	}

	return FAILURE;
}
/* }}} */

static zend_op *zend_compile_simple_var_no_cv(znode *result, zend_ast *ast, uint32_t type, int delayed) /* {{{ */
{
	zend_ast *name_ast = ast->child[0];
	znode name_node;
	zend_op *opline;

	zend_compile_expr(&name_node, name_ast);
	if (name_node.op_type == IS_CONST) {
		convert_to_string(&name_node.u.constant);
	}

	if (delayed) {
		opline = zend_delayed_emit_op(result, ZEND_FETCH_R, &name_node, NULL);
	} else {
		opline = zend_emit_op(result, ZEND_FETCH_R, &name_node, NULL);
	}

	if (name_node.op_type == IS_CONST &&
	    zend_is_auto_global(Z_STR(name_node.u.constant))) {

		opline->extended_value = ZEND_FETCH_GLOBAL;
	} else {
		opline->extended_value = ZEND_FETCH_LOCAL;
	}

	zend_adjust_for_fetch_type(opline, result, type);
	return opline;
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

static void zend_compile_simple_var(znode *result, zend_ast *ast, uint32_t type, int delayed) /* {{{ */
{
	if (is_this_fetch(ast)) {
		zend_op *opline = zend_emit_op(result, ZEND_FETCH_THIS, NULL, NULL);
		if ((type == BP_VAR_R) || (type == BP_VAR_IS)) {
			opline->result_type = IS_TMP_VAR;
			result->op_type = IS_TMP_VAR;
		}
	} else if (zend_try_compile_cv(result, ast) == FAILURE) {
		zend_compile_simple_var_no_cv(result, ast, type, delayed);
	}
}
/* }}} */

static void zend_separate_if_call_and_write(znode *node, zend_ast *ast, uint32_t type) /* {{{ */
{
	if (type != BP_VAR_R && type != BP_VAR_IS && zend_is_call(ast)) {
		if (node->op_type == IS_VAR) {
			zend_op *opline = zend_emit_op(NULL, ZEND_SEPARATE, node, NULL);
			opline->result_type = IS_VAR;
			opline->result.var = opline->op1.var;
		} else {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use result of built-in function in write context");
		}
	}
}
/* }}} */

void zend_delayed_compile_var(znode *result, zend_ast *ast, uint32_t type);
void zend_compile_assign(znode *result, zend_ast *ast);

static inline void zend_emit_assign_znode(zend_ast *var_ast, znode *value_node) /* {{{ */
{
	znode dummy_node;
	zend_ast *assign_ast = zend_ast_create(ZEND_AST_ASSIGN, var_ast,
		zend_ast_create_znode(value_node));
	zend_compile_assign(&dummy_node, assign_ast);
	zend_do_free(&dummy_node);
}
/* }}} */

static zend_op *zend_delayed_compile_dim(znode *result, zend_ast *ast, uint32_t type) /* {{{ */
{
	zend_ast *var_ast = ast->child[0];
	zend_ast *dim_ast = ast->child[1];
	zend_op *opline;

	znode var_node, dim_node;

	zend_delayed_compile_var(&var_node, var_ast, type);
	zend_separate_if_call_and_write(&var_node, var_ast, type);

	if (dim_ast == NULL) {
		if (type == BP_VAR_R || type == BP_VAR_IS) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use [] for reading");
		}
		if (type == BP_VAR_UNSET) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use [] for unsetting");
		}
		dim_node.op_type = IS_UNUSED;
	} else {
		zend_compile_expr(&dim_node, dim_ast);
	}

	opline = zend_delayed_emit_op(result, ZEND_FETCH_DIM_R, &var_node, &dim_node);
	zend_adjust_for_fetch_type(opline, result, type);

	if (dim_node.op_type == IS_CONST) {
		zend_handle_numeric_dim(opline, &dim_node);
	}
	return opline;
}
/* }}} */

static zend_op *zend_compile_dim(znode *result, zend_ast *ast, uint32_t type) /* {{{ */
{
	uint32_t offset = zend_delayed_compile_begin();
	zend_delayed_compile_dim(result, ast, type);
	return zend_delayed_compile_end(offset);
}
/* }}} */

static zend_op *zend_delayed_compile_prop(znode *result, zend_ast *ast, uint32_t type) /* {{{ */
{
	zend_ast *obj_ast = ast->child[0];
	zend_ast *prop_ast = ast->child[1];

	znode obj_node, prop_node;
	zend_op *opline;

	if (is_this_fetch(obj_ast)) {
		obj_node.op_type = IS_UNUSED;
	} else {
		zend_delayed_compile_var(&obj_node, obj_ast, type);
		zend_separate_if_call_and_write(&obj_node, obj_ast, type);
	}
	zend_compile_expr(&prop_node, prop_ast);

	opline = zend_delayed_emit_op(result, ZEND_FETCH_OBJ_R, &obj_node, &prop_node);
	if (opline->op2_type == IS_CONST) {
		convert_to_string(CT_CONSTANT(opline->op2));
		opline->extended_value = zend_alloc_polymorphic_cache_slot();
	}

	zend_adjust_for_fetch_type(opline, result, type);
	return opline;
}
/* }}} */

static zend_op *zend_compile_prop(znode *result, zend_ast *ast, uint32_t type) /* {{{ */
{
	uint32_t offset = zend_delayed_compile_begin();
	zend_delayed_compile_prop(result, ast, type);
	return zend_delayed_compile_end(offset);
}
/* }}} */

zend_op *zend_compile_static_prop(znode *result, zend_ast *ast, uint32_t type, int delayed) /* {{{ */
{
	zend_ast *class_ast = ast->child[0];
	zend_ast *prop_ast = ast->child[1];

	znode class_node, prop_node;
	zend_op *opline;

	zend_compile_class_ref_ex(&class_node, class_ast, ZEND_FETCH_CLASS_EXCEPTION);

	zend_compile_expr(&prop_node, prop_ast);

	if (delayed) {
		opline = zend_delayed_emit_op(result, ZEND_FETCH_STATIC_PROP_R, &prop_node, NULL);
	} else {
		opline = zend_emit_op(result, ZEND_FETCH_STATIC_PROP_R, &prop_node, NULL);
	}
	if (opline->op1_type == IS_CONST) {
		convert_to_string(CT_CONSTANT(opline->op1));
		opline->extended_value = zend_alloc_polymorphic_cache_slot();
	}
	if (class_node.op_type == IS_CONST) {
		opline->op2_type = IS_CONST;
		opline->op2.constant = zend_add_class_name_literal(
			CG(active_op_array), Z_STR(class_node.u.constant));
		if (opline->op1_type != IS_CONST) {
			opline->extended_value = zend_alloc_cache_slot();
		}
	} else {
		SET_NODE(opline->op2, &class_node);
	}

	zend_adjust_for_fetch_type(opline, result, type);
	return opline;
}
/* }}} */

static void zend_verify_list_assign_target(zend_ast *var_ast, zend_bool old_style) /* {{{ */ {
	if (var_ast->kind == ZEND_AST_ARRAY) {
		if (var_ast->attr == ZEND_ARRAY_SYNTAX_LONG) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot assign to array(), use [] instead");
		}
		if (old_style != var_ast->attr) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot mix [] and list()");
		}
	} else if (!zend_can_write_to_variable(var_ast)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Assignments can only happen to writable values");
	}
}
/* }}} */

static inline void zend_emit_assign_ref_znode(zend_ast *var_ast, znode *value_node);

/* Propagate refs used on leaf elements to the surrounding list() structures. */
static zend_bool zend_propagate_list_refs(zend_ast *ast) { /* {{{ */
	zend_ast_list *list = zend_ast_get_list(ast);
	zend_bool has_refs = 0;
	uint32_t i;

	for (i = 0; i < list->children; ++i) {
		zend_ast *elem_ast = list->child[i];

		if (elem_ast) {
			zend_ast *var_ast = elem_ast->child[0];
			if (var_ast->kind == ZEND_AST_ARRAY) {
				elem_ast->attr = zend_propagate_list_refs(var_ast);
			}
			has_refs |= elem_ast->attr;
		}
	}

	return has_refs;
}
/* }}} */

static void zend_compile_list_assign(
		znode *result, zend_ast *ast, znode *expr_node, zend_bool old_style) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;
	zend_bool has_elems = 0;
	zend_bool is_keyed =
		list->children > 0 && list->child[0] != NULL && list->child[0]->child[1] != NULL;

	if (list->children && expr_node->op_type == IS_CONST && Z_TYPE(expr_node->u.constant) == IS_STRING) {
		zval_make_interned_string(&expr_node->u.constant);
	}

	for (i = 0; i < list->children; ++i) {
		zend_ast *elem_ast = list->child[i];
		zend_ast *var_ast, *key_ast;
		znode fetch_result, dim_node;
		zend_op *opline;

		if (elem_ast == NULL) {
			if (is_keyed) {
				zend_error(E_COMPILE_ERROR,
					"Cannot use empty array entries in keyed array assignment");
			} else {
				continue;
			}
		}

		var_ast = elem_ast->child[0];
		key_ast = elem_ast->child[1];
		has_elems = 1;

		if (is_keyed) {
			if (key_ast == NULL) {
				zend_error(E_COMPILE_ERROR,
					"Cannot mix keyed and unkeyed array entries in assignments");
			}

			zend_compile_expr(&dim_node, key_ast);
		} else {
			if (key_ast != NULL) {
				zend_error(E_COMPILE_ERROR,
					"Cannot mix keyed and unkeyed array entries in assignments");
			}

			dim_node.op_type = IS_CONST;
			ZVAL_LONG(&dim_node.u.constant, i);
		}

		if (expr_node->op_type == IS_CONST) {
			Z_TRY_ADDREF(expr_node->u.constant);
		}

		zend_verify_list_assign_target(var_ast, old_style);

		opline = zend_emit_op(&fetch_result,
			elem_ast->attr ? (expr_node->op_type == IS_CV ? ZEND_FETCH_DIM_W : ZEND_FETCH_LIST_W) : ZEND_FETCH_LIST_R, expr_node, &dim_node);

		if (dim_node.op_type == IS_CONST) {
			zend_handle_numeric_dim(opline, &dim_node);
		}

		if (var_ast->kind == ZEND_AST_ARRAY) {
			if (elem_ast->attr) {
				zend_emit_op(&fetch_result, ZEND_MAKE_REF, &fetch_result, NULL);
			}
			zend_compile_list_assign(NULL, var_ast, &fetch_result, var_ast->attr);
		} else if (elem_ast->attr) {
			zend_emit_assign_ref_znode(var_ast, &fetch_result);
		} else {
			zend_emit_assign_znode(var_ast, &fetch_result);
		}
	}

	if (has_elems == 0) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use empty list");
	}

	if (result) {
		*result = *expr_node;
	} else {
		zend_do_free(expr_node);
	}
}
/* }}} */

static void zend_ensure_writable_variable(const zend_ast *ast) /* {{{ */
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
zend_bool zend_is_assign_to_self(zend_ast *var_ast, zend_ast *expr_ast) /* {{{ */
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
		zend_string_release_ex(name1, 0);
		zend_string_release_ex(name2, 0);
		return result;
	}
}
/* }}} */

/* Detects if list($a, $b, $c) contains variable with given name */
zend_bool zend_list_has_assign_to(zend_ast *list_ast, zend_string *name) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(list_ast);
	uint32_t i;
	for (i = 0; i < list->children; i++) {
		zend_ast *elem_ast = list->child[i];
		zend_ast *var_ast;

		if (!elem_ast) {
			continue;
		}
		var_ast = elem_ast->child[0];

		/* Recursively check nested list()s */
		if (var_ast->kind == ZEND_AST_ARRAY && zend_list_has_assign_to(var_ast, name)) {
			return 1;
		}

		if (var_ast->kind == ZEND_AST_VAR && var_ast->child[0]->kind == ZEND_AST_ZVAL) {
			zend_string *var_name = zval_get_string(zend_ast_get_zval(var_ast->child[0]));
			zend_bool result = zend_string_equals(var_name, name);
			zend_string_release_ex(var_name, 0);
			if (result) {
				return 1;
			}
		}
	}

	return 0;
}
/* }}} */

/* Detects patterns like list($a, $b, $c) = $a */
zend_bool zend_list_has_assign_to_self(zend_ast *list_ast, zend_ast *expr_ast) /* {{{ */
{
	/* Only check simple variables on the RHS, as only CVs cause issues with this. */
	if (expr_ast->kind == ZEND_AST_VAR && expr_ast->child[0]->kind == ZEND_AST_ZVAL) {
		zend_string *name = zval_get_string(zend_ast_get_zval(expr_ast->child[0]));
		zend_bool result = zend_list_has_assign_to(list_ast, name);
		zend_string_release_ex(name, 0);
		return result;
	}
	return 0;
}
/* }}} */

void zend_compile_assign(znode *result, zend_ast *ast) /* {{{ */
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
			offset = zend_delayed_compile_begin();
			zend_delayed_compile_var(&var_node, var_ast, BP_VAR_W);
			zend_compile_expr(&expr_node, expr_ast);
			zend_delayed_compile_end(offset);
			zend_emit_op(result, ZEND_ASSIGN, &var_node, &expr_node);
			return;
		case ZEND_AST_DIM:
			offset = zend_delayed_compile_begin();
			zend_delayed_compile_dim(result, var_ast, BP_VAR_W);

			if (zend_is_assign_to_self(var_ast, expr_ast)
			 && !is_this_fetch(expr_ast)) {
				/* $a[0] = $a should evaluate the right $a first */
				znode cv_node;

				if (zend_try_compile_cv(&cv_node, expr_ast) == FAILURE) {
					zend_compile_simple_var_no_cv(&expr_node, expr_ast, BP_VAR_R, 0);
				} else {
					zend_emit_op(&expr_node, ZEND_QM_ASSIGN, &cv_node, NULL);
				}
			} else {
				zend_compile_expr(&expr_node, expr_ast);
			}

			opline = zend_delayed_compile_end(offset);
			opline->opcode = ZEND_ASSIGN_DIM;

			opline = zend_emit_op_data(&expr_node);
			return;
		case ZEND_AST_PROP:
			offset = zend_delayed_compile_begin();
			zend_delayed_compile_prop(result, var_ast, BP_VAR_W);
			zend_compile_expr(&expr_node, expr_ast);

			opline = zend_delayed_compile_end(offset);
			opline->opcode = ZEND_ASSIGN_OBJ;

			zend_emit_op_data(&expr_node);
			return;
		case ZEND_AST_ARRAY:
			if (zend_propagate_list_refs(var_ast)) {
				if (!zend_is_variable(expr_ast)) {
					zend_error_noreturn(E_COMPILE_ERROR,
						"Cannot assign reference to non referencable value");
				}

				zend_compile_var(&expr_node, expr_ast, BP_VAR_W);
				/* MAKE_REF is usually not necessary for CVs. However, if there are
				 * self-assignments, this forces the RHS to evaluate first. */
				if (expr_node.op_type != IS_CV
						|| zend_list_has_assign_to_self(var_ast, expr_ast)) {
					zend_emit_op(&expr_node, ZEND_MAKE_REF, &expr_node, NULL);
				}
			} else {
				if (zend_list_has_assign_to_self(var_ast, expr_ast)) {
					/* list($a, $b) = $a should evaluate the right $a first */
					znode cv_node;

					if (zend_try_compile_cv(&cv_node, expr_ast) == FAILURE) {
						zend_compile_simple_var_no_cv(&expr_node, expr_ast, BP_VAR_R, 0);
					} else {
						zend_emit_op(&expr_node, ZEND_QM_ASSIGN, &cv_node, NULL);
					}
				} else {
					zend_compile_expr(&expr_node, expr_ast);
				}
			}

			zend_compile_list_assign(result, var_ast, &expr_node, var_ast->attr);
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}
/* }}} */

void zend_compile_assign_ref(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *target_ast = ast->child[0];
	zend_ast *source_ast = ast->child[1];

	znode target_node, source_node;
	zend_op *opline;
	uint32_t offset;

	if (is_this_fetch(target_ast)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot re-assign $this");
	}
	zend_ensure_writable_variable(target_ast);

	offset = zend_delayed_compile_begin();
	zend_delayed_compile_var(&target_node, target_ast, BP_VAR_W);
	zend_compile_var(&source_node, source_ast, BP_VAR_W);

	if ((target_ast->kind != ZEND_AST_VAR
	  || target_ast->child[0]->kind != ZEND_AST_ZVAL)
	 && source_node.op_type != IS_CV) {
		/* Both LHS and RHS expressions may modify the same data structure,
		 * and the modification during RHS evaluation may dangle the pointer
		 * to the result of the LHS evaluation.
		 * Use MAKE_REF instruction to replace direct pointer with REFERENCE.
		 * See: Bug #71539
		 */
		zend_emit_op(&source_node, ZEND_MAKE_REF, &source_node, NULL);
	}

	zend_delayed_compile_end(offset);

	if (source_node.op_type != IS_VAR && zend_is_call(source_ast)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use result of built-in function in write context");
	}

	opline = zend_emit_op(result, ZEND_ASSIGN_REF, &target_node, &source_node);

	if (zend_is_call(source_ast)) {
		opline->extended_value = ZEND_RETURNS_FUNCTION;
	}
}
/* }}} */

static inline void zend_emit_assign_ref_znode(zend_ast *var_ast, znode *value_node) /* {{{ */
{
	zend_ast *assign_ast = zend_ast_create(ZEND_AST_ASSIGN_REF, var_ast,
		zend_ast_create_znode(value_node));
	zend_compile_assign_ref(NULL, assign_ast);
}
/* }}} */

void zend_compile_compound_assign(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *var_ast = ast->child[0];
	zend_ast *expr_ast = ast->child[1];
	uint32_t opcode = ast->attr;

	znode var_node, expr_node;
	zend_op *opline;
	uint32_t offset, cache_slot;

	zend_ensure_writable_variable(var_ast);

	switch (var_ast->kind) {
		case ZEND_AST_VAR:
		case ZEND_AST_STATIC_PROP:
			offset = zend_delayed_compile_begin();
			zend_delayed_compile_var(&var_node, var_ast, BP_VAR_RW);
			zend_compile_expr(&expr_node, expr_ast);
			zend_delayed_compile_end(offset);
			zend_emit_op(result, opcode, &var_node, &expr_node);
			return;
		case ZEND_AST_DIM:
			offset = zend_delayed_compile_begin();
			zend_delayed_compile_dim(result, var_ast, BP_VAR_RW);
			zend_compile_expr(&expr_node, expr_ast);

			opline = zend_delayed_compile_end(offset);
			opline->opcode = opcode;
			opline->extended_value = ZEND_ASSIGN_DIM;

			opline = zend_emit_op_data(&expr_node);
			return;
		case ZEND_AST_PROP:
			offset = zend_delayed_compile_begin();
			zend_delayed_compile_prop(result, var_ast, BP_VAR_RW);
			zend_compile_expr(&expr_node, expr_ast);

			opline = zend_delayed_compile_end(offset);
			cache_slot = opline->extended_value;
			opline->opcode = opcode;
			opline->extended_value = ZEND_ASSIGN_OBJ;

			opline = zend_emit_op_data(&expr_node);
			opline->extended_value = cache_slot;
			return;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

uint32_t zend_compile_args(zend_ast *ast, zend_function *fbc) /* {{{ */
{
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

		if (arg->kind == ZEND_AST_UNPACK) {
			uses_arg_unpack = 1;
			fbc = NULL;

			zend_compile_expr(&arg_node, arg->child[0]);
			opline = zend_emit_op(NULL, ZEND_SEND_UNPACK, &arg_node, NULL);
			opline->op2.num = arg_count;
			opline->result.var = (uint32_t)(zend_intptr_t)ZEND_CALL_ARG(NULL, arg_count);
			continue;
		}

		if (uses_arg_unpack) {
			zend_error_noreturn(E_COMPILE_ERROR,
				"Cannot use positional argument after argument unpacking");
		}

		arg_count++;
		if (zend_is_variable(arg)) {
			if (zend_is_call(arg)) {
				zend_compile_var(&arg_node, arg, BP_VAR_R);
				if (arg_node.op_type & (IS_CONST|IS_TMP_VAR)) {
					/* Function call was converted into builtin instruction */
					opcode = ZEND_SEND_VAL;
				} else {
					if (fbc) {
						if (ARG_MUST_BE_SENT_BY_REF(fbc, arg_num)) {
							opcode = ZEND_SEND_VAR_NO_REF;
						} else if (ARG_MAY_BE_SENT_BY_REF(fbc, arg_num)) {
							opcode = ZEND_SEND_VAL;
						} else {
							opcode = ZEND_SEND_VAR;
						}
					} else {
						opcode = ZEND_SEND_VAR_NO_REF_EX;
					}
				}
			} else if (fbc) {
				if (ARG_SHOULD_BE_SENT_BY_REF(fbc, arg_num)) {
					zend_compile_var(&arg_node, arg, BP_VAR_W);
					opcode = ZEND_SEND_REF;
				} else {
					zend_compile_var(&arg_node, arg, BP_VAR_R);
					opcode = (arg_node.op_type == IS_TMP_VAR) ? ZEND_SEND_VAL : ZEND_SEND_VAR;
				}
			} else {
				do {
					if (arg->kind == ZEND_AST_VAR) {
						CG(zend_lineno) = zend_ast_get_lineno(ast);
						if (is_this_fetch(arg)) {
							zend_emit_op(&arg_node, ZEND_FETCH_THIS, NULL, NULL);
							opcode = ZEND_SEND_VAR_EX;
							break;
						} else if (zend_try_compile_cv(&arg_node, arg) == SUCCESS) {
							opcode = ZEND_SEND_VAR_EX;
							break;
						}
					}
					opline = zend_emit_op(NULL, ZEND_CHECK_FUNC_ARG, NULL, NULL);
					opline->op2.num = arg_num;
					zend_compile_var(&arg_node, arg, BP_VAR_FUNC_ARG);
					opcode = ZEND_SEND_FUNC_ARG;
				} while (0);
			}
		} else {
			zend_compile_expr(&arg_node, arg);
			if (arg_node.op_type == IS_VAR) {
				/* pass ++$a or something similar */
				if (fbc) {
					if (ARG_MUST_BE_SENT_BY_REF(fbc, arg_num)) {
						opcode = ZEND_SEND_VAR_NO_REF;
					} else if (ARG_MAY_BE_SENT_BY_REF(fbc, arg_num)) {
						opcode = ZEND_SEND_VAL;
					} else {
						opcode = ZEND_SEND_VAR;
					}
				} else {
					opcode = ZEND_SEND_VAR_NO_REF_EX;
				}
			} else if (arg_node.op_type == IS_CV) {
				if (fbc) {
					if (ARG_SHOULD_BE_SENT_BY_REF(fbc, arg_num)) {
						opcode = ZEND_SEND_REF;
					} else {
						opcode = ZEND_SEND_VAR;
					}
				} else {
					opcode = ZEND_SEND_VAR_EX;
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

		opline = zend_emit_op(NULL, opcode, &arg_node, NULL);
		opline->op2.opline_num = arg_num;
		opline->result.var = (uint32_t)(zend_intptr_t)ZEND_CALL_ARG(NULL, arg_num);
	}

	return arg_count;
}
/* }}} */

ZEND_API zend_uchar zend_get_call_op(const zend_op *init_op, zend_function *fbc) /* {{{ */
{
	if (fbc) {
		if (fbc->type == ZEND_INTERNAL_FUNCTION) {
			if (init_op->opcode == ZEND_INIT_FCALL && !zend_execute_internal) {
				if (!(fbc->common.fn_flags & (ZEND_ACC_ABSTRACT|ZEND_ACC_DEPRECATED|ZEND_ACC_HAS_TYPE_HINTS|ZEND_ACC_RETURN_REFERENCE))) {
					return ZEND_DO_ICALL;
				} else {
					return ZEND_DO_FCALL_BY_NAME;
				}
			}
		} else {
			if (zend_execute_ex == execute_ex && !(fbc->common.fn_flags & ZEND_ACC_ABSTRACT)) {
				return ZEND_DO_UCALL;
			}
		}
	} else if (zend_execute_ex == execute_ex &&
	           !zend_execute_internal &&
	           (init_op->opcode == ZEND_INIT_FCALL_BY_NAME ||
	            init_op->opcode == ZEND_INIT_NS_FCALL_BY_NAME)) {
		return ZEND_DO_FCALL_BY_NAME;
	}
	return ZEND_DO_FCALL;
}
/* }}} */

void zend_compile_call_common(znode *result, zend_ast *args_ast, zend_function *fbc) /* {{{ */
{
	zend_op *opline;
	uint32_t opnum_init = get_next_op_number(CG(active_op_array)) - 1;
	uint32_t arg_count;
	uint32_t call_flags;

	zend_do_extended_fcall_begin();

	arg_count = zend_compile_args(args_ast, fbc);

	opline = &CG(active_op_array)->opcodes[opnum_init];
	opline->extended_value = arg_count;

	if (opline->opcode == ZEND_INIT_FCALL) {
		opline->op1.num = zend_vm_calc_used_stack(arg_count, fbc);
	}

	call_flags = (opline->opcode == ZEND_NEW ? ZEND_CALL_CTOR : 0);
	opline = zend_emit_op(result, zend_get_call_op(opline, fbc), NULL, NULL);
	opline->op1.num = call_flags;

	zend_do_extended_fcall_end();
}
/* }}} */

zend_bool zend_compile_function_name(znode *name_node, zend_ast *name_ast) /* {{{ */
{
	zend_string *orig_name = zend_ast_get_str(name_ast);
	zend_bool is_fully_qualified;

	name_node->op_type = IS_CONST;
	ZVAL_STR(&name_node->u.constant, zend_resolve_function_name(
		orig_name, name_ast->attr, &is_fully_qualified));

	return !is_fully_qualified && FC(current_namespace);
}
/* }}} */

void zend_compile_ns_call(znode *result, znode *name_node, zend_ast *args_ast) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array));
	opline->opcode = ZEND_INIT_NS_FCALL_BY_NAME;
	opline->op2_type = IS_CONST;
	opline->op2.constant = zend_add_ns_func_name_literal(
		CG(active_op_array), Z_STR(name_node->u.constant));
	opline->result.num = zend_alloc_cache_slot();

	zend_compile_call_common(result, args_ast, NULL);
}
/* }}} */

void zend_compile_dynamic_call(znode *result, znode *name_node, zend_ast *args_ast) /* {{{ */
{
	if (name_node->op_type == IS_CONST && Z_TYPE(name_node->u.constant) == IS_STRING) {
		const char *colon;
		zend_string *str = Z_STR(name_node->u.constant);
		if ((colon = zend_memrchr(ZSTR_VAL(str), ':', ZSTR_LEN(str))) != NULL && colon > ZSTR_VAL(str) && *(colon - 1) == ':') {
			zend_string *class = zend_string_init(ZSTR_VAL(str), colon - ZSTR_VAL(str) - 1, 0);
			zend_string *method = zend_string_init(colon + 1, ZSTR_LEN(str) - (colon - ZSTR_VAL(str)) - 1, 0);
			zend_op *opline = get_next_op(CG(active_op_array));

			opline->opcode = ZEND_INIT_STATIC_METHOD_CALL;
			opline->op1_type = IS_CONST;
			opline->op1.constant = zend_add_class_name_literal(CG(active_op_array), class);
			opline->op2_type = IS_CONST;
			opline->op2.constant = zend_add_func_name_literal(CG(active_op_array), method);
			/* 2 slots, for class and method */
			opline->result.num = zend_alloc_polymorphic_cache_slot();
			zval_ptr_dtor(&name_node->u.constant);
		} else {
			zend_op *opline = get_next_op(CG(active_op_array));

			opline->opcode = ZEND_INIT_FCALL_BY_NAME;
			opline->op2_type = IS_CONST;
			opline->op2.constant = zend_add_func_name_literal(CG(active_op_array), str);
			opline->result.num = zend_alloc_cache_slot();
		}
	} else {
		zend_emit_op(NULL, ZEND_INIT_DYNAMIC_CALL, NULL, name_node);
	}

	zend_compile_call_common(result, args_ast, NULL);
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

int zend_compile_func_strlen(znode *result, zend_ast_list *args) /* {{{ */
{
	znode arg_node;

	if ((CG(compiler_options) & ZEND_COMPILE_NO_BUILTIN_STRLEN)
		|| args->children != 1 || args->child[0]->kind == ZEND_AST_UNPACK
	) {
		return FAILURE;
	}

	zend_compile_expr(&arg_node, args->child[0]);
	if (arg_node.op_type == IS_CONST && Z_TYPE(arg_node.u.constant) == IS_STRING) {
		result->op_type = IS_CONST;
		ZVAL_LONG(&result->u.constant, Z_STRLEN(arg_node.u.constant));
		zval_ptr_dtor_str(&arg_node.u.constant);
	} else {
		zend_emit_op_tmp(result, ZEND_STRLEN, &arg_node, NULL);
	}
	return SUCCESS;
}
/* }}} */

int zend_compile_func_typecheck(znode *result, zend_ast_list *args, uint32_t type) /* {{{ */
{
	znode arg_node;
	zend_op *opline;

	if (args->children != 1 || args->child[0]->kind == ZEND_AST_UNPACK) {
		return FAILURE;
	}

	zend_compile_expr(&arg_node, args->child[0]);
	opline = zend_emit_op_tmp(result, ZEND_TYPE_CHECK, &arg_node, NULL);
	if (type != _IS_BOOL) {
		opline->extended_value = (1 << type);
	} else {
		opline->extended_value = (1 << IS_FALSE) | (1 << IS_TRUE);
	}
	return SUCCESS;
}
/* }}} */

int zend_compile_func_cast(znode *result, zend_ast_list *args, uint32_t type) /* {{{ */
{
	znode arg_node;
	zend_op *opline;

	if (args->children != 1 || args->child[0]->kind == ZEND_AST_UNPACK) {
		return FAILURE;
	}

	zend_compile_expr(&arg_node, args->child[0]);
	opline = zend_emit_op_tmp(result, ZEND_CAST, &arg_node, NULL);
	opline->extended_value = type;
	return SUCCESS;
}
/* }}} */

int zend_compile_func_defined(znode *result, zend_ast_list *args) /* {{{ */
{
	zend_string *name;
	zend_op *opline;

	if (args->children != 1 || args->child[0]->kind != ZEND_AST_ZVAL) {
		return FAILURE;
	}

	name = zval_get_string(zend_ast_get_zval(args->child[0]));
	if (zend_memrchr(ZSTR_VAL(name), '\\', ZSTR_LEN(name)) || zend_memrchr(ZSTR_VAL(name), ':', ZSTR_LEN(name))) {
		zend_string_release_ex(name, 0);
		return FAILURE;
	}

	if (zend_try_ct_eval_const(&result->u.constant, name, 0)) {
		zend_string_release_ex(name, 0);
		zval_ptr_dtor(&result->u.constant);
		ZVAL_TRUE(&result->u.constant);
		result->op_type = IS_CONST;
		return SUCCESS;
	}

	opline = zend_emit_op_tmp(result, ZEND_DEFINED, NULL, NULL);
	opline->op1_type = IS_CONST;
	LITERAL_STR(opline->op1, name);
	opline->extended_value = zend_alloc_cache_slot();

	/* Lowercase constant name in a separate literal */
	{
		zval c;
		zend_string *lcname = zend_string_tolower(name);
		ZVAL_NEW_STR(&c, lcname);
		zend_add_literal(CG(active_op_array), &c);
	}
	return SUCCESS;
}
/* }}} */

int zend_compile_func_chr(znode *result, zend_ast_list *args) /* {{{ */
{

	if (args->children == 1 &&
	    args->child[0]->kind == ZEND_AST_ZVAL &&
	    Z_TYPE_P(zend_ast_get_zval(args->child[0])) == IS_LONG) {

		zend_long c = Z_LVAL_P(zend_ast_get_zval(args->child[0])) & 0xff;

		result->op_type = IS_CONST;
		ZVAL_INTERNED_STR(&result->u.constant, ZSTR_CHAR(c));
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

int zend_compile_func_ord(znode *result, zend_ast_list *args) /* {{{ */
{
	if (args->children == 1 &&
	    args->child[0]->kind == ZEND_AST_ZVAL &&
	    Z_TYPE_P(zend_ast_get_zval(args->child[0])) == IS_STRING) {

		result->op_type = IS_CONST;
		ZVAL_LONG(&result->u.constant, (unsigned char)Z_STRVAL_P(zend_ast_get_zval(args->child[0]))[0]);
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */


static int zend_try_compile_ct_bound_init_user_func(zend_ast *name_ast, uint32_t num_args) /* {{{ */
{
	zend_string *name, *lcname;
	zend_function *fbc;
	zend_op *opline;

	if (name_ast->kind != ZEND_AST_ZVAL || Z_TYPE_P(zend_ast_get_zval(name_ast)) != IS_STRING) {
		return FAILURE;
	}

	name = zend_ast_get_str(name_ast);
	lcname = zend_string_tolower(name);

	fbc = zend_hash_find_ptr(CG(function_table), lcname);
	if (!fbc
	 || (fbc->type == ZEND_INTERNAL_FUNCTION && (CG(compiler_options) & ZEND_COMPILE_IGNORE_INTERNAL_FUNCTIONS))
	 || (fbc->type == ZEND_USER_FUNCTION && (CG(compiler_options) & ZEND_COMPILE_IGNORE_USER_FUNCTIONS))
	) {
		zend_string_release_ex(lcname, 0);
		return FAILURE;
	}

	opline = zend_emit_op(NULL, ZEND_INIT_FCALL, NULL, NULL);
	opline->extended_value = num_args;
	opline->op1.num = zend_vm_calc_used_stack(num_args, fbc);
	opline->op2_type = IS_CONST;
	LITERAL_STR(opline->op2, lcname);
	opline->result.num = zend_alloc_cache_slot();

	return SUCCESS;
}
/* }}} */

static void zend_compile_init_user_func(zend_ast *name_ast, uint32_t num_args, zend_string *orig_func_name) /* {{{ */
{
	zend_op *opline;
	znode name_node;

	if (zend_try_compile_ct_bound_init_user_func(name_ast, num_args) == SUCCESS) {
		return;
	}

	zend_compile_expr(&name_node, name_ast);

	opline = zend_emit_op(NULL, ZEND_INIT_USER_CALL, NULL, &name_node);
	opline->op1_type = IS_CONST;
	LITERAL_STR(opline->op1, zend_string_copy(orig_func_name));
	opline->extended_value = num_args;
}
/* }}} */

/* cufa = call_user_func_array */
int zend_compile_func_cufa(znode *result, zend_ast_list *args, zend_string *lcname) /* {{{ */
{
	znode arg_node;

	if (args->children != 2 || zend_args_contain_unpack(args)) {
		return FAILURE;
	}

	zend_compile_init_user_func(args->child[0], 0, lcname);
	if (args->child[1]->kind == ZEND_AST_CALL
	 && args->child[1]->child[0]->kind == ZEND_AST_ZVAL
	 && Z_TYPE_P(zend_ast_get_zval(args->child[1]->child[0])) == IS_STRING
	 && args->child[1]->child[1]->kind == ZEND_AST_ARG_LIST) {
		zend_string *orig_name = zend_ast_get_str(args->child[1]->child[0]);
		zend_ast_list *list = zend_ast_get_list(args->child[1]->child[1]);
		zend_bool is_fully_qualified;
		zend_string *name = zend_resolve_function_name(orig_name, args->child[1]->child[0]->attr, &is_fully_qualified);

		if (zend_string_equals_literal_ci(name, "array_slice")
		 && list->children == 3
		 && list->child[1]->kind == ZEND_AST_ZVAL) {
			zval *zv = zend_ast_get_zval(list->child[1]);

			if (Z_TYPE_P(zv) == IS_LONG
			 && Z_LVAL_P(zv) >= 0
			 && Z_LVAL_P(zv) <= 0x7fffffff) {
				zend_op *opline;
				znode len_node;

				zend_compile_expr(&arg_node, list->child[0]);
				zend_compile_expr(&len_node, list->child[2]);
				opline = zend_emit_op(NULL, ZEND_SEND_ARRAY, &arg_node, &len_node);
				opline->extended_value = Z_LVAL_P(zv);
				zend_emit_op(result, ZEND_DO_FCALL, NULL, NULL);
				zend_string_release_ex(name, 0);
				return SUCCESS;
			}
		}
		zend_string_release_ex(name, 0);
	}
	zend_compile_expr(&arg_node, args->child[1]);
	zend_emit_op(NULL, ZEND_SEND_ARRAY, &arg_node, NULL);
	zend_emit_op(result, ZEND_DO_FCALL, NULL, NULL);

	return SUCCESS;
}
/* }}} */

/* cuf = call_user_func */
int zend_compile_func_cuf(znode *result, zend_ast_list *args, zend_string *lcname) /* {{{ */
{
	uint32_t i;

	if (args->children < 1 || zend_args_contain_unpack(args)) {
		return FAILURE;
	}

	zend_compile_init_user_func(args->child[0], args->children - 1, lcname);
	for (i = 1; i < args->children; ++i) {
		zend_ast *arg_ast = args->child[i];
		znode arg_node;
		zend_op *opline;

		zend_compile_expr(&arg_node, arg_ast);

		opline = zend_emit_op(NULL, ZEND_SEND_USER, &arg_node, NULL);
		opline->op2.num = i;
		opline->result.var = (uint32_t)(zend_intptr_t)ZEND_CALL_ARG(NULL, i);
	}
	zend_emit_op(result, ZEND_DO_FCALL, NULL, NULL);

	return SUCCESS;
}
/* }}} */

static int zend_compile_assert(znode *result, zend_ast_list *args, zend_string *name, zend_function *fbc) /* {{{ */
{
	if (EG(assertions) >= 0) {
		znode name_node;
		zend_op *opline;
		uint32_t check_op_number = get_next_op_number(CG(active_op_array));

		zend_emit_op(NULL, ZEND_ASSERT_CHECK, NULL, NULL);

		if (fbc) {
			name_node.op_type = IS_CONST;
			ZVAL_STR_COPY(&name_node.u.constant, name);

			opline = zend_emit_op(NULL, ZEND_INIT_FCALL, NULL, &name_node);
		} else {
			opline = zend_emit_op(NULL, ZEND_INIT_NS_FCALL_BY_NAME, NULL, NULL);
			opline->op2_type = IS_CONST;
			opline->op2.constant = zend_add_ns_func_name_literal(
				CG(active_op_array), name);
		}
		opline->result.num = zend_alloc_cache_slot();

		if (args->children == 1 &&
		    (args->child[0]->kind != ZEND_AST_ZVAL ||
		     Z_TYPE_P(zend_ast_get_zval(args->child[0])) != IS_STRING)) {
			/* add "assert(condition) as assertion message */
			zend_ast_list_add((zend_ast*)args,
				zend_ast_create_zval_from_str(
					zend_ast_export("assert(", args->child[0], ")")));
		}

		zend_compile_call_common(result, (zend_ast*)args, fbc);

		opline = &CG(active_op_array)->opcodes[check_op_number];
		opline->op2.opline_num = get_next_op_number(CG(active_op_array));
		SET_NODE(opline->result, result);
	} else {
		if (!fbc) {
			zend_string_release_ex(name, 0);
		}
		result->op_type = IS_CONST;
		ZVAL_TRUE(&result->u.constant);
	}

	return SUCCESS;
}
/* }}} */

static int zend_compile_func_in_array(znode *result, zend_ast_list *args) /* {{{ */
{
	zend_bool strict = 0;
	znode array, needly;
	zend_op *opline;

	if (args->children == 3) {
		if (args->child[2]->kind == ZEND_AST_ZVAL) {
			strict = zend_is_true(zend_ast_get_zval(args->child[2]));
		} else if (args->child[2]->kind == ZEND_AST_CONST) {
			zval value;
			zend_ast *name_ast = args->child[2]->child[0];
			zend_bool is_fully_qualified;
			zend_string *resolved_name = zend_resolve_const_name(
				zend_ast_get_str(name_ast), name_ast->attr, &is_fully_qualified);

			if (!zend_try_ct_eval_const(&value, resolved_name, is_fully_qualified)) {
				zend_string_release_ex(resolved_name, 0);
				return FAILURE;
			}

			zend_string_release_ex(resolved_name, 0);
			strict = zend_is_true(&value);
			zval_ptr_dtor(&value);
		} else {
			return FAILURE;
		}
	} else if (args->children != 2) {
		return FAILURE;
	}

	if (args->child[1]->kind != ZEND_AST_ARRAY
	 || !zend_try_ct_eval_array(&array.u.constant, args->child[1])) {
		return FAILURE;
	}

	if (zend_hash_num_elements(Z_ARRVAL(array.u.constant)) > 0) {
		zend_bool ok = 1;
		zval *val, tmp;
		HashTable *src = Z_ARRVAL(array.u.constant);
		HashTable *dst = zend_new_array(zend_hash_num_elements(src));

		ZVAL_TRUE(&tmp);

		if (strict) {
			ZEND_HASH_FOREACH_VAL(src, val) {
				if (Z_TYPE_P(val) == IS_STRING) {
					zend_hash_add(dst, Z_STR_P(val), &tmp);
				} else if (Z_TYPE_P(val) == IS_LONG) {
					zend_hash_index_add(dst, Z_LVAL_P(val), &tmp);
				} else {
					zend_array_destroy(dst);
					ok = 0;
					break;
				}
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEND_HASH_FOREACH_VAL(src, val) {
				if (Z_TYPE_P(val) != IS_STRING
				 || is_numeric_string(Z_STRVAL_P(val), Z_STRLEN_P(val), NULL, NULL, 0)) {
					zend_array_destroy(dst);
					ok = 0;
					break;
				}
				zend_hash_add(dst, Z_STR_P(val), &tmp);
			} ZEND_HASH_FOREACH_END();
		}

		zend_array_destroy(src);
		if (!ok) {
			return FAILURE;
		}
		Z_ARRVAL(array.u.constant) = dst;
	}
	array.op_type = IS_CONST;

	zend_compile_expr(&needly, args->child[0]);

	opline = zend_emit_op_tmp(result, ZEND_IN_ARRAY, &needly, &array);
	opline->extended_value = strict;

	return SUCCESS;
}
/* }}} */

int zend_compile_func_count(znode *result, zend_ast_list *args) /* {{{ */
{
	znode arg_node;

	if (args->children != 1 || args->child[0]->kind == ZEND_AST_UNPACK) {
		return FAILURE;
	}

	zend_compile_expr(&arg_node, args->child[0]);
	zend_emit_op_tmp(result, ZEND_COUNT, &arg_node, NULL);
	return SUCCESS;
}
/* }}} */

int zend_compile_func_get_class(znode *result, zend_ast_list *args) /* {{{ */
{
	if (args->children == 0) {
		zend_emit_op_tmp(result, ZEND_GET_CLASS, NULL, NULL);
	} else {
		znode arg_node;

		if (args->children != 1 || args->child[0]->kind == ZEND_AST_UNPACK) {
			return FAILURE;
		}

		zend_compile_expr(&arg_node, args->child[0]);
		zend_emit_op_tmp(result, ZEND_GET_CLASS, &arg_node, NULL);
	}
	return SUCCESS;
}
/* }}} */

int zend_compile_func_get_called_class(znode *result, zend_ast_list *args) /* {{{ */
{
	if (args->children != 0) {
		return FAILURE;
	}

	zend_emit_op_tmp(result, ZEND_GET_CALLED_CLASS, NULL, NULL);
	return SUCCESS;
}
/* }}} */

int zend_compile_func_gettype(znode *result, zend_ast_list *args) /* {{{ */
{
	znode arg_node;

	if (args->children != 1 || args->child[0]->kind == ZEND_AST_UNPACK) {
		return FAILURE;
	}

	zend_compile_expr(&arg_node, args->child[0]);
	zend_emit_op_tmp(result, ZEND_GET_TYPE, &arg_node, NULL);
	return SUCCESS;
}
/* }}} */

int zend_compile_func_num_args(znode *result, zend_ast_list *args) /* {{{ */
{
	if (CG(active_op_array)->function_name && args->children == 0) {
		zend_emit_op_tmp(result, ZEND_FUNC_NUM_ARGS, NULL, NULL);
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

int zend_compile_func_get_args(znode *result, zend_ast_list *args) /* {{{ */
{
	if (CG(active_op_array)->function_name && args->children == 0) {
		zend_emit_op_tmp(result, ZEND_FUNC_GET_ARGS, NULL, NULL);
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

int zend_compile_func_array_slice(znode *result, zend_ast_list *args) /* {{{ */
{
	if (CG(active_op_array)->function_name
	 && args->children == 2
	 && args->child[0]->kind == ZEND_AST_CALL
	 && args->child[0]->child[0]->kind == ZEND_AST_ZVAL
	 && Z_TYPE_P(zend_ast_get_zval(args->child[0]->child[0])) == IS_STRING
	 && args->child[0]->child[1]->kind == ZEND_AST_ARG_LIST
	 && args->child[1]->kind == ZEND_AST_ZVAL) {

		zend_string *orig_name = zend_ast_get_str(args->child[0]->child[0]);
		zend_bool is_fully_qualified;
		zend_string *name = zend_resolve_function_name(orig_name, args->child[0]->child[0]->attr, &is_fully_qualified);
		zend_ast_list *list = zend_ast_get_list(args->child[0]->child[1]);
		zval *zv = zend_ast_get_zval(args->child[1]);
		znode first;

		if (zend_string_equals_literal_ci(name, "func_get_args")
		 && list->children == 0
		 && Z_TYPE_P(zv) == IS_LONG
		 && Z_LVAL_P(zv) >= 0) {
			first.op_type = IS_CONST;
			ZVAL_LONG(&first.u.constant, Z_LVAL_P(zv));
			zend_emit_op_tmp(result, ZEND_FUNC_GET_ARGS, &first, NULL);
			zend_string_release_ex(name, 0);
			return SUCCESS;
		}
		zend_string_release_ex(name, 0);
	}
	return FAILURE;
}
/* }}} */

int zend_try_compile_special_func(znode *result, zend_string *lcname, zend_ast_list *args, zend_function *fbc, uint32_t type) /* {{{ */
{
	if (fbc->internal_function.handler == ZEND_FN(display_disabled_function)) {
		return FAILURE;
	}

	if (zend_string_equals_literal(lcname, "assert")) {
		return zend_compile_assert(result, args, lcname, fbc);
	}

	if (CG(compiler_options) & ZEND_COMPILE_NO_BUILTINS) {
		return FAILURE;
	}

	if (zend_string_equals_literal(lcname, "strlen")) {
		return zend_compile_func_strlen(result, args);
	} else if (zend_string_equals_literal(lcname, "is_null")) {
		return zend_compile_func_typecheck(result, args, IS_NULL);
	} else if (zend_string_equals_literal(lcname, "is_bool")) {
		return zend_compile_func_typecheck(result, args, _IS_BOOL);
	} else if (zend_string_equals_literal(lcname, "is_long")
		|| zend_string_equals_literal(lcname, "is_int")
		|| zend_string_equals_literal(lcname, "is_integer")
	) {
		return zend_compile_func_typecheck(result, args, IS_LONG);
	} else if (zend_string_equals_literal(lcname, "is_float")
		|| zend_string_equals_literal(lcname, "is_double")
		|| zend_string_equals_literal(lcname, "is_real")
	) {
		return zend_compile_func_typecheck(result, args, IS_DOUBLE);
	} else if (zend_string_equals_literal(lcname, "is_string")) {
		return zend_compile_func_typecheck(result, args, IS_STRING);
	} else if (zend_string_equals_literal(lcname, "is_array")) {
		return zend_compile_func_typecheck(result, args, IS_ARRAY);
	} else if (zend_string_equals_literal(lcname, "is_object")) {
		return zend_compile_func_typecheck(result, args, IS_OBJECT);
	} else if (zend_string_equals_literal(lcname, "is_resource")) {
		return zend_compile_func_typecheck(result, args, IS_RESOURCE);
	} else if (zend_string_equals_literal(lcname, "boolval")) {
		return zend_compile_func_cast(result, args, _IS_BOOL);
	} else if (zend_string_equals_literal(lcname, "intval")) {
		return zend_compile_func_cast(result, args, IS_LONG);
	} else if (zend_string_equals_literal(lcname, "floatval")
		|| zend_string_equals_literal(lcname, "doubleval")
	) {
		return zend_compile_func_cast(result, args, IS_DOUBLE);
	} else if (zend_string_equals_literal(lcname, "strval")) {
		return zend_compile_func_cast(result, args, IS_STRING);
	} else if (zend_string_equals_literal(lcname, "defined")) {
		return zend_compile_func_defined(result, args);
	} else if (zend_string_equals_literal(lcname, "chr") && type == BP_VAR_R) {
		return zend_compile_func_chr(result, args);
	} else if (zend_string_equals_literal(lcname, "ord") && type == BP_VAR_R) {
		return zend_compile_func_ord(result, args);
	} else if (zend_string_equals_literal(lcname, "call_user_func_array")) {
		return zend_compile_func_cufa(result, args, lcname);
	} else if (zend_string_equals_literal(lcname, "call_user_func")) {
		return zend_compile_func_cuf(result, args, lcname);
	} else if (zend_string_equals_literal(lcname, "in_array")) {
		return zend_compile_func_in_array(result, args);
	} else if (zend_string_equals_literal(lcname, "count")) {
		return zend_compile_func_count(result, args);
	} else if (zend_string_equals_literal(lcname, "get_class")) {
		return zend_compile_func_get_class(result, args);
	} else if (zend_string_equals_literal(lcname, "get_called_class")) {
		return zend_compile_func_get_called_class(result, args);
	} else if (zend_string_equals_literal(lcname, "gettype")) {
		return zend_compile_func_gettype(result, args);
	} else if (zend_string_equals_literal(lcname, "func_num_args")) {
		return zend_compile_func_num_args(result, args);
	} else if (zend_string_equals_literal(lcname, "func_get_args")) {
		return zend_compile_func_get_args(result, args);
	} else if (zend_string_equals_literal(lcname, "array_slice")) {
		return zend_compile_func_array_slice(result, args);
	} else {
		return FAILURE;
	}
}
/* }}} */

void zend_compile_call(znode *result, zend_ast *ast, uint32_t type) /* {{{ */
{
	zend_ast *name_ast = ast->child[0];
	zend_ast *args_ast = ast->child[1];

	znode name_node;

	if (name_ast->kind != ZEND_AST_ZVAL || Z_TYPE_P(zend_ast_get_zval(name_ast)) != IS_STRING) {
		zend_compile_expr(&name_node, name_ast);
		zend_compile_dynamic_call(result, &name_node, args_ast);
		return;
	}

	{
		zend_bool runtime_resolution = zend_compile_function_name(&name_node, name_ast);
		if (runtime_resolution) {
			if (zend_string_equals_literal_ci(zend_ast_get_str(name_ast), "assert")) {
				zend_compile_assert(result, zend_ast_get_list(args_ast), Z_STR(name_node.u.constant), NULL);
			} else {
				zend_compile_ns_call(result, &name_node, args_ast);
			}
			return;
		}
	}

	{
		zval *name = &name_node.u.constant;
		zend_string *lcname;
		zend_function *fbc;
		zend_op *opline;

		lcname = zend_string_tolower(Z_STR_P(name));

		fbc = zend_hash_find_ptr(CG(function_table), lcname);
		if (!fbc
		 || (fbc->type == ZEND_INTERNAL_FUNCTION && (CG(compiler_options) & ZEND_COMPILE_IGNORE_INTERNAL_FUNCTIONS))
		 || (fbc->type == ZEND_USER_FUNCTION && (CG(compiler_options) & ZEND_COMPILE_IGNORE_USER_FUNCTIONS))
		) {
			zend_string_release_ex(lcname, 0);
			zend_compile_dynamic_call(result, &name_node, args_ast);
			return;
		}

		if (zend_try_compile_special_func(result, lcname,
				zend_ast_get_list(args_ast), fbc, type) == SUCCESS
		) {
			zend_string_release_ex(lcname, 0);
			zval_ptr_dtor(&name_node.u.constant);
			return;
		}

		zval_ptr_dtor(&name_node.u.constant);
		ZVAL_NEW_STR(&name_node.u.constant, lcname);

		opline = zend_emit_op(NULL, ZEND_INIT_FCALL, NULL, &name_node);
		opline->result.num = zend_alloc_cache_slot();

		zend_compile_call_common(result, args_ast, fbc);
	}
}
/* }}} */

void zend_compile_method_call(znode *result, zend_ast *ast, uint32_t type) /* {{{ */
{
	zend_ast *obj_ast = ast->child[0];
	zend_ast *method_ast = ast->child[1];
	zend_ast *args_ast = ast->child[2];

	znode obj_node, method_node;
	zend_op *opline;
	zend_function *fbc = NULL;

	if (is_this_fetch(obj_ast)) {
		obj_node.op_type = IS_UNUSED;
	} else {
		zend_compile_expr(&obj_node, obj_ast);
	}

	zend_compile_expr(&method_node, method_ast);
	opline = zend_emit_op(NULL, ZEND_INIT_METHOD_CALL, &obj_node, NULL);

	if (method_node.op_type == IS_CONST) {
		if (Z_TYPE(method_node.u.constant) != IS_STRING) {
			zend_error_noreturn(E_COMPILE_ERROR, "Method name must be a string");
		}

		opline->op2_type = IS_CONST;
		opline->op2.constant = zend_add_func_name_literal(CG(active_op_array),
			Z_STR(method_node.u.constant));
		opline->result.num = zend_alloc_polymorphic_cache_slot();
	} else {
		SET_NODE(opline->op2, &method_node);
	}

	/* Check if this calls a known method on $this */
	if (opline->op1_type == IS_UNUSED && opline->op2_type == IS_CONST &&
			CG(active_class_entry) && zend_is_scope_known()) {
		zend_string *lcname = Z_STR_P(CT_CONSTANT(opline->op2) + 1);
		fbc = zend_hash_find_ptr(&CG(active_class_entry)->function_table, lcname);

		/* We only know the exact method that is being called if it is either private or final.
		 * Otherwise an overriding method in a child class may be called. */
		if (fbc && !(fbc->common.fn_flags & (ZEND_ACC_PRIVATE|ZEND_ACC_FINAL))) {
			fbc = NULL;
		}
	}

	zend_compile_call_common(result, args_ast, fbc);
}
/* }}} */

static zend_bool zend_is_constructor(zend_string *name) /* {{{ */
{
	return zend_string_equals_literal_ci(name, ZEND_CONSTRUCTOR_FUNC_NAME);
}
/* }}} */

void zend_compile_static_call(znode *result, zend_ast *ast, uint32_t type) /* {{{ */
{
	zend_ast *class_ast = ast->child[0];
	zend_ast *method_ast = ast->child[1];
	zend_ast *args_ast = ast->child[2];

	znode class_node, method_node;
	zend_op *opline;
	zend_function *fbc = NULL;

	zend_compile_class_ref_ex(&class_node, class_ast, ZEND_FETCH_CLASS_EXCEPTION);

	zend_compile_expr(&method_node, method_ast);
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

	opline = get_next_op(CG(active_op_array));
	opline->opcode = ZEND_INIT_STATIC_METHOD_CALL;

	zend_set_class_name_op1(opline, &class_node);

	if (method_node.op_type == IS_CONST) {
		opline->op2_type = IS_CONST;
		opline->op2.constant = zend_add_func_name_literal(CG(active_op_array),
			Z_STR(method_node.u.constant));
		opline->result.num = zend_alloc_polymorphic_cache_slot();
	} else {
		if (opline->op1_type == IS_CONST) {
			opline->result.num = zend_alloc_cache_slot();
		}
		SET_NODE(opline->op2, &method_node);
	}
	zend_check_live_ranges(opline);

	/* Check if we already know which method we're calling */
	if (opline->op2_type == IS_CONST) {
		zend_class_entry *ce = NULL;
		if (opline->op1_type == IS_CONST) {
			zend_string *lcname = Z_STR_P(CT_CONSTANT(opline->op1) + 1);
			ce = zend_hash_find_ptr(CG(class_table), lcname);
			if (!ce && CG(active_class_entry)
					&& zend_string_equals_ci(CG(active_class_entry)->name, lcname)) {
				ce = CG(active_class_entry);
			}
		} else if (opline->op1_type == IS_UNUSED
				&& (opline->op1.num & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_SELF
				&& zend_is_scope_known()) {
			ce = CG(active_class_entry);
		}
		if (ce) {
			zend_string *lcname = Z_STR_P(CT_CONSTANT(opline->op2) + 1);
			fbc = zend_hash_find_ptr(&ce->function_table, lcname);
		}
	}

	zend_compile_call_common(result, args_ast, fbc);
}
/* }}} */

void zend_compile_class_decl(zend_ast *ast);

void zend_compile_new(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *class_ast = ast->child[0];
	zend_ast *args_ast = ast->child[1];

	znode class_node, ctor_result;
	zend_op *opline;

	if (class_ast->kind == ZEND_AST_CLASS) {
		uint32_t dcl_opnum = get_next_op_number(CG(active_op_array));
		zend_compile_class_decl(class_ast);
		/* jump over anon class declaration */
		opline = &CG(active_op_array)->opcodes[dcl_opnum];
		if (opline->opcode == ZEND_FETCH_CLASS) {
			opline++;
		}
		class_node.op_type = opline->result_type;
		class_node.u.op.var = opline->result.var;
		opline->extended_value = get_next_op_number(CG(active_op_array));
	} else {
		zend_compile_class_ref_ex(&class_node, class_ast, ZEND_FETCH_CLASS_EXCEPTION);
	}

	opline = zend_emit_op(result, ZEND_NEW, NULL, NULL);

	if (class_node.op_type == IS_CONST) {
		opline->op1_type = IS_CONST;
		opline->op1.constant = zend_add_class_name_literal(
			CG(active_op_array), Z_STR(class_node.u.constant));
		opline->op2.num = zend_alloc_cache_slot();
	} else {
		SET_NODE(opline->op1, &class_node);
	}

	zend_compile_call_common(&ctor_result, args_ast, NULL);
	zend_do_free(&ctor_result);
}
/* }}} */

void zend_compile_clone(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *obj_ast = ast->child[0];

	znode obj_node;
	zend_compile_expr(&obj_node, obj_ast);

	zend_emit_op_tmp(result, ZEND_CLONE, &obj_node, NULL);
}
/* }}} */

void zend_compile_global_var(zend_ast *ast) /* {{{ */
{
	zend_ast *var_ast = ast->child[0];
	zend_ast *name_ast = var_ast->child[0];

	znode name_node, result;

	zend_compile_expr(&name_node, name_ast);
	if (name_node.op_type == IS_CONST) {
		convert_to_string(&name_node.u.constant);
	}

	if (is_this_fetch(var_ast)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use $this as global variable");
	} else if (zend_try_compile_cv(&result, var_ast) == SUCCESS) {
		zend_op *opline = zend_emit_op(NULL, ZEND_BIND_GLOBAL, &result, &name_node);
		opline->extended_value = zend_alloc_cache_slot();
	} else {
		/* name_ast should be evaluated only. FETCH_GLOBAL_LOCK instructs FETCH_W
		 * to not free the name_node operand, so it can be reused in the following
		 * ASSIGN_REF, which then frees it. */
		zend_op *opline = zend_emit_op(&result, ZEND_FETCH_W, &name_node, NULL);
		opline->extended_value = ZEND_FETCH_GLOBAL_LOCK;

		if (name_node.op_type == IS_CONST) {
			zend_string_addref(Z_STR(name_node.u.constant));
		}

		zend_emit_assign_ref_znode(
			zend_ast_create(ZEND_AST_VAR, zend_ast_create_znode(&name_node)),
			&result
		);
	}
}
/* }}} */

static void zend_compile_static_var_common(zend_ast *var_ast, zval *value, uint32_t by_ref) /* {{{ */
{
	znode var_node;
	zend_op *opline;
	zend_string *var_name;

	if (var_ast->kind == ZEND_AST_ZVAL) {
		var_name = zval_make_interned_string(zend_ast_get_zval(var_ast));
		zend_compile_expr(&var_node, var_ast);
	} else {
		zend_compile_expr(&var_node, var_ast);
		var_name = zval_make_interned_string(&var_node.u.constant);
	}

	if (!CG(active_op_array)->static_variables) {
		if (CG(active_op_array)->scope) {
			CG(active_op_array)->scope->ce_flags |= ZEND_HAS_STATIC_IN_METHODS;
		}
		CG(active_op_array)->static_variables = zend_new_array(8);
	}

	if (GC_REFCOUNT(CG(active_op_array)->static_variables) > 1) {
		if (!(GC_FLAGS(CG(active_op_array)->static_variables) & IS_ARRAY_IMMUTABLE)) {
			GC_DELREF(CG(active_op_array)->static_variables);
		}
		CG(active_op_array)->static_variables = zend_array_dup(CG(active_op_array)->static_variables);
	}
	value = zend_hash_update(CG(active_op_array)->static_variables, var_name, value);

	if (zend_string_equals_literal(var_name, "this")) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use $this as static variable");
	}

	opline = zend_emit_op(NULL, ZEND_BIND_STATIC, NULL, &var_node);
	opline->op1_type = IS_CV;
	opline->op1.var = lookup_cv(CG(active_op_array), var_name);
	opline->extended_value = (uint32_t)((char*)value - (char*)CG(active_op_array)->static_variables->arData) | by_ref;
}
/* }}} */

void zend_compile_static_var(zend_ast *ast) /* {{{ */
{
	zend_ast *var_ast = ast->child[0];
	zend_ast *value_ast = ast->child[1];
	zval value_zv;

	if (value_ast) {
		zend_const_expr_to_zval(&value_zv, value_ast);
	} else {
		ZVAL_NULL(&value_zv);
	}

	zend_compile_static_var_common(var_ast, &value_zv, ZEND_BIND_REF);
}
/* }}} */

void zend_compile_unset(zend_ast *ast) /* {{{ */
{
	zend_ast *var_ast = ast->child[0];
	znode var_node;
	zend_op *opline;

	zend_ensure_writable_variable(var_ast);

	switch (var_ast->kind) {
		case ZEND_AST_VAR:
			if (is_this_fetch(var_ast)) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot unset $this");
			} else if (zend_try_compile_cv(&var_node, var_ast) == SUCCESS) {
				opline = zend_emit_op(NULL, ZEND_UNSET_CV, &var_node, NULL);
			} else {
				opline = zend_compile_simple_var_no_cv(NULL, var_ast, BP_VAR_UNSET, 0);
				opline->opcode = ZEND_UNSET_VAR;
			}
			return;
		case ZEND_AST_DIM:
			opline = zend_compile_dim(NULL, var_ast, BP_VAR_UNSET);
			opline->opcode = ZEND_UNSET_DIM;
			return;
		case ZEND_AST_PROP:
			opline = zend_compile_prop(NULL, var_ast, BP_VAR_UNSET);
			opline->opcode = ZEND_UNSET_OBJ;
			return;
		case ZEND_AST_STATIC_PROP:
			opline = zend_compile_static_prop(NULL, var_ast, BP_VAR_UNSET, 0);
			opline->opcode = ZEND_UNSET_STATIC_PROP;
			return;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

static int zend_handle_loops_and_finally_ex(zend_long depth, znode *return_value) /* {{{ */
{
	zend_loop_var *base;
	zend_loop_var *loop_var = zend_stack_top(&CG(loop_var_stack));

	if (!loop_var) {
		return 1;
	}
	base = zend_stack_base(&CG(loop_var_stack));
	for (; loop_var >= base; loop_var--) {
		if (loop_var->opcode == ZEND_FAST_CALL) {
			zend_op *opline = get_next_op(CG(active_op_array));

			opline->opcode = ZEND_FAST_CALL;
			opline->result_type = IS_TMP_VAR;
			opline->result.var = loop_var->var_num;
			if (return_value) {
				SET_NODE(opline->op2, return_value);
			}
			opline->op1.num = loop_var->u.try_catch_offset;
		} else if (loop_var->opcode == ZEND_DISCARD_EXCEPTION) {
			zend_op *opline = get_next_op(CG(active_op_array));
			opline->opcode = ZEND_DISCARD_EXCEPTION;
			opline->op1_type = IS_TMP_VAR;
			opline->op1.var = loop_var->var_num;
		} else if (loop_var->opcode == ZEND_RETURN) {
			/* Stack separator */
			break;
		} else if (depth <= 1) {
			return 1;
		} else if (loop_var->opcode == ZEND_NOP) {
			/* Loop doesn't have freeable variable */
			depth--;
		} else {
			zend_op *opline;

			ZEND_ASSERT(loop_var->var_type & (IS_VAR|IS_TMP_VAR));
			opline = get_next_op(CG(active_op_array));
			opline->opcode = loop_var->opcode;
			opline->op1_type = loop_var->var_type;
			opline->op1.var = loop_var->var_num;
			opline->extended_value = ZEND_FREE_ON_RETURN;
			depth--;
	    }
	}
	return (depth == 0);
}
/* }}} */

static int zend_handle_loops_and_finally(znode *return_value) /* {{{ */
{
	return zend_handle_loops_and_finally_ex(zend_stack_count(&CG(loop_var_stack)) + 1, return_value);
}
/* }}} */

static int zend_has_finally_ex(zend_long depth) /* {{{ */
{
	zend_loop_var *base;
	zend_loop_var *loop_var = zend_stack_top(&CG(loop_var_stack));

	if (!loop_var) {
		return 0;
	}
	base = zend_stack_base(&CG(loop_var_stack));
	for (; loop_var >= base; loop_var--) {
		if (loop_var->opcode == ZEND_FAST_CALL) {
			return 1;
		} else if (loop_var->opcode == ZEND_DISCARD_EXCEPTION) {
		} else if (loop_var->opcode == ZEND_RETURN) {
			/* Stack separator */
			return 0;
		} else if (depth <= 1) {
			return 0;
		} else {
			depth--;
	    }
	}
	return 0;
}
/* }}} */

static int zend_has_finally(void) /* {{{ */
{
	return zend_has_finally_ex(zend_stack_count(&CG(loop_var_stack)) + 1);
}
/* }}} */

void zend_compile_return(zend_ast *ast) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	zend_bool is_generator = (CG(active_op_array)->fn_flags & ZEND_ACC_GENERATOR) != 0;
	zend_bool by_ref = (CG(active_op_array)->fn_flags & ZEND_ACC_RETURN_REFERENCE) != 0;

	znode expr_node;
	zend_op *opline;

	if (is_generator) {
		/* For generators the by-ref flag refers to yields, not returns */
		by_ref = 0;
	}

	if (!expr_ast) {
		expr_node.op_type = IS_CONST;
		ZVAL_NULL(&expr_node.u.constant);
	} else if (by_ref && zend_is_variable(expr_ast) && !zend_is_call(expr_ast)) {
		zend_compile_var(&expr_node, expr_ast, BP_VAR_W);
	} else {
		zend_compile_expr(&expr_node, expr_ast);
	}

	if ((CG(active_op_array)->fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK)
	 && (expr_node.op_type == IS_CV || (by_ref && expr_node.op_type == IS_VAR))
	 && zend_has_finally()) {
		/* Copy return value into temporary VAR to avoid modification in finally code */
		if (by_ref) {
			zend_emit_op(&expr_node, ZEND_MAKE_REF, &expr_node, NULL);
		} else {
			zend_emit_op_tmp(&expr_node, ZEND_QM_ASSIGN, &expr_node, NULL);
		}
	}

	/* Generator return types are handled separately */
	if (!is_generator && CG(active_op_array)->fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
		zend_emit_return_type_check(
			expr_ast ? &expr_node : NULL, CG(active_op_array)->arg_info - 1, 0);
	}

	zend_handle_loops_and_finally((expr_node.op_type & (IS_TMP_VAR | IS_VAR)) ? &expr_node : NULL);

	opline = zend_emit_op(NULL, by_ref ? ZEND_RETURN_BY_REF : ZEND_RETURN,
		&expr_node, NULL);

	if (by_ref && expr_ast) {
		if (zend_is_call(expr_ast)) {
			opline->extended_value = ZEND_RETURNS_FUNCTION;
		} else if (!zend_is_variable(expr_ast)) {
			opline->extended_value = ZEND_RETURNS_VALUE;
		}
	}
}
/* }}} */

void zend_compile_echo(zend_ast *ast) /* {{{ */
{
	zend_op *opline;
	zend_ast *expr_ast = ast->child[0];

	znode expr_node;
	zend_compile_expr(&expr_node, expr_ast);

	opline = zend_emit_op(NULL, ZEND_ECHO, &expr_node, NULL);
	opline->extended_value = 0;
}
/* }}} */

void zend_compile_throw(zend_ast *ast) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];

	znode expr_node;
	zend_compile_expr(&expr_node, expr_ast);

	zend_emit_op(NULL, ZEND_THROW, &expr_node, NULL);
}
/* }}} */

void zend_compile_break_continue(zend_ast *ast) /* {{{ */
{
	zend_ast *depth_ast = ast->child[0];

	zend_op *opline;
	int depth;

	ZEND_ASSERT(ast->kind == ZEND_AST_BREAK || ast->kind == ZEND_AST_CONTINUE);

	if (depth_ast) {
		zval *depth_zv;
		if (depth_ast->kind != ZEND_AST_ZVAL) {
			zend_error_noreturn(E_COMPILE_ERROR, "'%s' operator with non-integer operand "
				"is no longer supported", ast->kind == ZEND_AST_BREAK ? "break" : "continue");
		}

		depth_zv = zend_ast_get_zval(depth_ast);
		if (Z_TYPE_P(depth_zv) != IS_LONG || Z_LVAL_P(depth_zv) < 1) {
			zend_error_noreturn(E_COMPILE_ERROR, "'%s' operator accepts only positive integers",
				ast->kind == ZEND_AST_BREAK ? "break" : "continue");
		}

		depth = Z_LVAL_P(depth_zv);
	} else {
		depth = 1;
	}

	if (CG(context).current_brk_cont == -1) {
		zend_error_noreturn(E_COMPILE_ERROR, "'%s' not in the 'loop' or 'switch' context",
			ast->kind == ZEND_AST_BREAK ? "break" : "continue");
	} else {
		if (!zend_handle_loops_and_finally_ex(depth, NULL)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot '%s' %d level%s",
				ast->kind == ZEND_AST_BREAK ? "break" : "continue",
				depth, depth == 1 ? "" : "s");
		}
	}

	if (ast->kind == ZEND_AST_CONTINUE) {
		int d, cur = CG(context).current_brk_cont;
		for (d = depth - 1; d > 0; d--) {
			cur = CG(context).brk_cont_array[cur].parent;
			ZEND_ASSERT(cur != -1);
		}

		if (CG(context).brk_cont_array[cur].is_switch) {
			if (depth == 1) {
				zend_error(E_WARNING,
					"\"continue\" targeting switch is equivalent to \"break\". " \
					"Did you mean to use \"continue %d\"?",
					depth + 1);
			} else {
				zend_error(E_WARNING,
					"\"continue %d\" targeting switch is equivalent to \"break %d\". " \
					"Did you mean to use \"continue %d\"?",
					depth, depth, depth + 1);
			}
		}
	}

	opline = zend_emit_op(NULL, ast->kind == ZEND_AST_BREAK ? ZEND_BRK : ZEND_CONT, NULL, NULL);
	opline->op1.num = CG(context).current_brk_cont;
	opline->op2.num = depth;
}
/* }}} */

void zend_resolve_goto_label(zend_op_array *op_array, zend_op *opline) /* {{{ */
{
	zend_label *dest;
	int current, remove_oplines = opline->op1.num;
	zval *label;
	uint32_t opnum = opline - op_array->opcodes;

	label = CT_CONSTANT_EX(op_array, opline->op2.constant);
	if (CG(context).labels == NULL ||
	    (dest = zend_hash_find_ptr(CG(context).labels, Z_STR_P(label))) == NULL
	) {
		CG(in_compilation) = 1;
		CG(active_op_array) = op_array;
		CG(zend_lineno) = opline->lineno;
		zend_error_noreturn(E_COMPILE_ERROR, "'goto' to undefined label '%s'", Z_STRVAL_P(label));
	}

	zval_ptr_dtor_str(label);
	ZVAL_NULL(label);

	current = opline->extended_value;
	for (; current != dest->brk_cont; current = CG(context).brk_cont_array[current].parent) {
		if (current == -1) {
			CG(in_compilation) = 1;
			CG(active_op_array) = op_array;
			CG(zend_lineno) = opline->lineno;
			zend_error_noreturn(E_COMPILE_ERROR, "'goto' into loop or switch statement is disallowed");
		}
		if (CG(context).brk_cont_array[current].start >= 0) {
			remove_oplines--;
		}
	}

	for (current = 0; current < op_array->last_try_catch; ++current) {
		zend_try_catch_element *elem = &op_array->try_catch_array[current];
		if (elem->try_op > opnum) {
			break;
		}
		if (elem->finally_op && opnum < elem->finally_op - 1
			&& (dest->opline_num > elem->finally_end || dest->opline_num < elem->try_op)
		) {
			remove_oplines--;
		}
	}

	opline->opcode = ZEND_JMP;
	opline->op1.opline_num = dest->opline_num;
	opline->extended_value = 0;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
	SET_UNUSED(opline->result);

	ZEND_ASSERT(remove_oplines >= 0);
	while (remove_oplines--) {
		opline--;
		MAKE_NOP(opline);
		ZEND_VM_SET_OPCODE_HANDLER(opline);
	}
}
/* }}} */

void zend_compile_goto(zend_ast *ast) /* {{{ */
{
	zend_ast *label_ast = ast->child[0];
	znode label_node;
	zend_op *opline;
	uint32_t opnum_start = get_next_op_number(CG(active_op_array));

	zend_compile_expr(&label_node, label_ast);

	/* Label resolution and unwinding adjustments happen in pass two. */
	zend_handle_loops_and_finally(NULL);
	opline = zend_emit_op(NULL, ZEND_GOTO, NULL, &label_node);
	opline->op1.num = get_next_op_number(CG(active_op_array)) - opnum_start - 1;
	opline->extended_value = CG(context).current_brk_cont;
}
/* }}} */

void zend_compile_label(zend_ast *ast) /* {{{ */
{
	zend_string *label = zend_ast_get_str(ast->child[0]);
	zend_label dest;

	if (!CG(context).labels) {
		ALLOC_HASHTABLE(CG(context).labels);
		zend_hash_init(CG(context).labels, 8, NULL, label_ptr_dtor, 0);
	}

	dest.brk_cont = CG(context).current_brk_cont;
	dest.opline_num = get_next_op_number(CG(active_op_array));

	if (!zend_hash_add_mem(CG(context).labels, label, &dest, sizeof(zend_label))) {
		zend_error_noreturn(E_COMPILE_ERROR, "Label '%s' already defined", ZSTR_VAL(label));
	}
}
/* }}} */

void zend_compile_while(zend_ast *ast) /* {{{ */
{
	zend_ast *cond_ast = ast->child[0];
	zend_ast *stmt_ast = ast->child[1];
	znode cond_node;
	uint32_t opnum_start, opnum_jmp, opnum_cond;

	opnum_jmp = zend_emit_jump(0);

	zend_begin_loop(ZEND_NOP, NULL, 0);

	opnum_start = get_next_op_number(CG(active_op_array));
	zend_compile_stmt(stmt_ast);

	opnum_cond = get_next_op_number(CG(active_op_array));
	zend_update_jump_target(opnum_jmp, opnum_cond);
	zend_compile_expr(&cond_node, cond_ast);

	zend_emit_cond_jump(ZEND_JMPNZ, &cond_node, opnum_start);

	zend_end_loop(opnum_cond, NULL);
}
/* }}} */

void zend_compile_do_while(zend_ast *ast) /* {{{ */
{
	zend_ast *stmt_ast = ast->child[0];
	zend_ast *cond_ast = ast->child[1];

	znode cond_node;
	uint32_t opnum_start, opnum_cond;

	zend_begin_loop(ZEND_NOP, NULL, 0);

	opnum_start = get_next_op_number(CG(active_op_array));
	zend_compile_stmt(stmt_ast);

	opnum_cond = get_next_op_number(CG(active_op_array));
	zend_compile_expr(&cond_node, cond_ast);

	zend_emit_cond_jump(ZEND_JMPNZ, &cond_node, opnum_start);

	zend_end_loop(opnum_cond, NULL);
}
/* }}} */

void zend_compile_expr_list(znode *result, zend_ast *ast) /* {{{ */
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

		zend_do_free(result);
		zend_compile_expr(result, expr_ast);
	}
}
/* }}} */

void zend_compile_for(zend_ast *ast) /* {{{ */
{
	zend_ast *init_ast = ast->child[0];
	zend_ast *cond_ast = ast->child[1];
	zend_ast *loop_ast = ast->child[2];
	zend_ast *stmt_ast = ast->child[3];

	znode result;
	uint32_t opnum_start, opnum_jmp, opnum_loop;

	zend_compile_expr_list(&result, init_ast);
	zend_do_free(&result);

	opnum_jmp = zend_emit_jump(0);

	zend_begin_loop(ZEND_NOP, NULL, 0);

	opnum_start = get_next_op_number(CG(active_op_array));
	zend_compile_stmt(stmt_ast);

	opnum_loop = get_next_op_number(CG(active_op_array));
	zend_compile_expr_list(&result, loop_ast);
	zend_do_free(&result);

	zend_update_jump_target_to_next(opnum_jmp);
	zend_compile_expr_list(&result, cond_ast);
	zend_do_extended_info();

	zend_emit_cond_jump(ZEND_JMPNZ, &result, opnum_start);

	zend_end_loop(opnum_loop, NULL);
}
/* }}} */

void zend_compile_foreach(zend_ast *ast) /* {{{ */
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
		if (key_ast->kind == ZEND_AST_ARRAY) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use list as key element");
		}
	}

	if (by_ref) {
		value_ast = value_ast->child[0];
	}

	if (value_ast->kind == ZEND_AST_ARRAY && zend_propagate_list_refs(value_ast)) {
		by_ref = 1;
	}

	if (by_ref && is_variable) {
		zend_compile_var(&expr_node, expr_ast, BP_VAR_W);
	} else {
		zend_compile_expr(&expr_node, expr_ast);
	}

	if (by_ref) {
		zend_separate_if_call_and_write(&expr_node, expr_ast, BP_VAR_W);
	}

	opnum_reset = get_next_op_number(CG(active_op_array));
	opline = zend_emit_op(&reset_node, by_ref ? ZEND_FE_RESET_RW : ZEND_FE_RESET_R, &expr_node, NULL);

	zend_begin_loop(ZEND_FE_FREE, &reset_node, 0);

	opnum_fetch = get_next_op_number(CG(active_op_array));
	opline = zend_emit_op(NULL, by_ref ? ZEND_FE_FETCH_RW : ZEND_FE_FETCH_R, &reset_node, NULL);

	if (is_this_fetch(value_ast)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot re-assign $this");
	} else if (value_ast->kind == ZEND_AST_VAR &&
		zend_try_compile_cv(&value_node, value_ast) == SUCCESS) {
		SET_NODE(opline->op2, &value_node);
	} else {
		opline->op2_type = IS_VAR;
		opline->op2.var = get_temporary_variable(CG(active_op_array));
		GET_NODE(&value_node, opline->op2);
		if (value_ast->kind == ZEND_AST_ARRAY) {
			zend_compile_list_assign(NULL, value_ast, &value_node, value_ast->attr);
		} else if (by_ref) {
			zend_emit_assign_ref_znode(value_ast, &value_node);
		} else {
			zend_emit_assign_znode(value_ast, &value_node);
		}
	}

	if (key_ast) {
		opline = &CG(active_op_array)->opcodes[opnum_fetch];
		zend_make_tmp_result(&key_node, opline);
		zend_emit_assign_znode(key_ast, &key_node);
	}

	zend_compile_stmt(stmt_ast);

	zend_emit_jump(opnum_fetch);

	opline = &CG(active_op_array)->opcodes[opnum_reset];
	opline->op2.opline_num = get_next_op_number(CG(active_op_array));

	opline = &CG(active_op_array)->opcodes[opnum_fetch];
	opline->extended_value = get_next_op_number(CG(active_op_array));

	zend_end_loop(opnum_fetch, &reset_node);

	opline = zend_emit_op(NULL, ZEND_FE_FREE, &reset_node, NULL);
}
/* }}} */

void zend_compile_if(zend_ast *ast) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;
	uint32_t *jmp_opnums = NULL;

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
			zend_compile_expr(&cond_node, cond_ast);
			opnum_jmpz = zend_emit_cond_jump(ZEND_JMPZ, &cond_node, 0);
		}

		zend_compile_stmt(stmt_ast);

		if (i != list->children - 1) {
			jmp_opnums[i] = zend_emit_jump(0);
		}

		if (cond_ast) {
			zend_update_jump_target_to_next(opnum_jmpz);
		}
	}

	if (list->children > 1) {
		for (i = 0; i < list->children - 1; ++i) {
			zend_update_jump_target_to_next(jmp_opnums[i]);
		}
		efree(jmp_opnums);
	}
}
/* }}} */

static zend_uchar determine_switch_jumptable_type(zend_ast_list *cases) {
	uint32_t i;
	zend_uchar common_type = IS_UNDEF;
	for (i = 0; i < cases->children; i++) {
		zend_ast *case_ast = cases->child[i];
		zend_ast **cond_ast = &case_ast->child[0];
		zval *cond_zv;
		if (!case_ast->child[0]) {
			/* Skip default clause */
			continue;
		}

		zend_eval_const_expr(cond_ast);
		if ((*cond_ast)->kind != ZEND_AST_ZVAL) {
			/* Non-constant case */
			return IS_UNDEF;
		}

		cond_zv = zend_ast_get_zval(case_ast->child[0]);
		if (Z_TYPE_P(cond_zv) != IS_LONG && Z_TYPE_P(cond_zv) != IS_STRING) {
			/* We only optimize switched on integers and strings */
			return IS_UNDEF;
		}

		if (common_type == IS_UNDEF) {
			common_type = Z_TYPE_P(cond_zv);
		} else if (common_type != Z_TYPE_P(cond_zv)) {
			/* Non-uniform case types */
			return IS_UNDEF;
		}

		if (Z_TYPE_P(cond_zv) == IS_STRING
				&& is_numeric_string(Z_STRVAL_P(cond_zv), Z_STRLEN_P(cond_zv), NULL, NULL, 0)) {
			/* Numeric strings cannot be compared with a simple hash lookup */
			return IS_UNDEF;
		}
	}

	return common_type;
}

static zend_bool should_use_jumptable(zend_ast_list *cases, zend_uchar jumptable_type) {
	/* Thresholds are chosen based on when the average switch time for equidistributed
	 * input becomes smaller when using the jumptable optimization. */
	if (jumptable_type == IS_LONG) {
		return cases->children >= 5;
	} else {
		ZEND_ASSERT(jumptable_type == IS_STRING);
		return cases->children >= 2;
	}
}

void zend_compile_switch(zend_ast *ast) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	zend_ast_list *cases = zend_ast_get_list(ast->child[1]);

	uint32_t i;
	zend_bool has_default_case = 0;

	znode expr_node, case_node;
	zend_op *opline;
	uint32_t *jmpnz_opnums, opnum_default_jmp, opnum_switch;
	zend_uchar jumptable_type;
	HashTable *jumptable = NULL;

	zend_compile_expr(&expr_node, expr_ast);

	zend_begin_loop(ZEND_FREE, &expr_node, 1);

	case_node.op_type = IS_TMP_VAR;
	case_node.u.op.var = get_temporary_variable(CG(active_op_array));

	jumptable_type = determine_switch_jumptable_type(cases);
	if (jumptable_type != IS_UNDEF && should_use_jumptable(cases, jumptable_type)) {
		znode jumptable_op;

		ALLOC_HASHTABLE(jumptable);
		zend_hash_init(jumptable, cases->children, NULL, NULL, 0);
		jumptable_op.op_type = IS_CONST;
		ZVAL_ARR(&jumptable_op.u.constant, jumptable);

		opline = zend_emit_op(NULL,
			jumptable_type == IS_LONG ? ZEND_SWITCH_LONG : ZEND_SWITCH_STRING,
			&expr_node, &jumptable_op);
		if (opline->op1_type == IS_CONST) {
			Z_TRY_ADDREF_P(CT_CONSTANT(opline->op1));
		}
		opnum_switch = opline - CG(active_op_array)->opcodes;
	}

	jmpnz_opnums = safe_emalloc(sizeof(uint32_t), cases->children, 0);
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

		zend_compile_expr(&cond_node, cond_ast);

		if (expr_node.op_type == IS_CONST
			&& Z_TYPE(expr_node.u.constant) == IS_FALSE) {
			jmpnz_opnums[i] = zend_emit_cond_jump(ZEND_JMPZ, &cond_node, 0);
		} else if (expr_node.op_type == IS_CONST
			&& Z_TYPE(expr_node.u.constant) == IS_TRUE) {
			jmpnz_opnums[i] = zend_emit_cond_jump(ZEND_JMPNZ, &cond_node, 0);
		} else {
			opline = zend_emit_op(NULL,
				(expr_node.op_type & (IS_VAR|IS_TMP_VAR)) ? ZEND_CASE : ZEND_IS_EQUAL,
				&expr_node, &cond_node);
			SET_NODE(opline->result, &case_node);
			if (opline->op1_type == IS_CONST) {
				Z_TRY_ADDREF_P(CT_CONSTANT(opline->op1));
			}

			jmpnz_opnums[i] = zend_emit_cond_jump(ZEND_JMPNZ, &case_node, 0);
		}
	}

	opnum_default_jmp = zend_emit_jump(0);

	for (i = 0; i < cases->children; ++i) {
		zend_ast *case_ast = cases->child[i];
		zend_ast *cond_ast = case_ast->child[0];
		zend_ast *stmt_ast = case_ast->child[1];

		if (cond_ast) {
			zend_update_jump_target_to_next(jmpnz_opnums[i]);

			if (jumptable) {
				zval *cond_zv = zend_ast_get_zval(cond_ast);
				zval jmp_target;
				ZVAL_LONG(&jmp_target, get_next_op_number(CG(active_op_array)));

				ZEND_ASSERT(Z_TYPE_P(cond_zv) == jumptable_type);
				if (Z_TYPE_P(cond_zv) == IS_LONG) {
					zend_hash_index_add(jumptable, Z_LVAL_P(cond_zv), &jmp_target);
				} else {
					ZEND_ASSERT(Z_TYPE_P(cond_zv) == IS_STRING);
					zend_hash_add(jumptable, Z_STR_P(cond_zv), &jmp_target);
				}
			}
		} else {
			zend_update_jump_target_to_next(opnum_default_jmp);

			if (jumptable) {
				opline = &CG(active_op_array)->opcodes[opnum_switch];
				opline->extended_value = get_next_op_number(CG(active_op_array));
			}
		}

		zend_compile_stmt(stmt_ast);
	}

	if (!has_default_case) {
		zend_update_jump_target_to_next(opnum_default_jmp);

		if (jumptable) {
			opline = &CG(active_op_array)->opcodes[opnum_switch];
			opline->extended_value = get_next_op_number(CG(active_op_array));
		}
	}

	zend_end_loop(get_next_op_number(CG(active_op_array)), &expr_node);

	if (expr_node.op_type & (IS_VAR|IS_TMP_VAR)) {
		/* don't use emit_op() to prevent automatic live-range construction */
		opline = get_next_op(CG(active_op_array));
		opline->opcode = ZEND_FREE;
		SET_NODE(opline->op1, &expr_node);
	} else if (expr_node.op_type == IS_CONST) {
		zval_ptr_dtor_nogc(&expr_node.u.constant);
	}

	efree(jmpnz_opnums);
}
/* }}} */

void zend_compile_try(zend_ast *ast) /* {{{ */
{
	zend_ast *try_ast = ast->child[0];
	zend_ast_list *catches = zend_ast_get_list(ast->child[1]);
	zend_ast *finally_ast = ast->child[2];

	uint32_t i, j;
	zend_op *opline;
	uint32_t try_catch_offset;
	uint32_t *jmp_opnums = safe_emalloc(sizeof(uint32_t), catches->children, 0);
	uint32_t orig_fast_call_var = CG(context).fast_call_var;
	uint32_t orig_try_catch_offset = CG(context).try_catch_offset;

	if (catches->children == 0 && !finally_ast) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot use try without catch or finally");
	}

	/* label: try { } must not be equal to try { label: } */
	if (CG(context).labels) {
		zend_label *label;
		ZEND_HASH_REVERSE_FOREACH_PTR(CG(context).labels, label) {
			if (label->opline_num == get_next_op_number(CG(active_op_array))) {
				zend_emit_op(NULL, ZEND_NOP, NULL, NULL);
			}
			break;
		} ZEND_HASH_FOREACH_END();
	}

	try_catch_offset = zend_add_try_element(get_next_op_number(CG(active_op_array)));

	if (finally_ast) {
		zend_loop_var fast_call;
		if (!(CG(active_op_array)->fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK)) {
			CG(active_op_array)->fn_flags |= ZEND_ACC_HAS_FINALLY_BLOCK;
		}
		CG(context).fast_call_var = get_temporary_variable(CG(active_op_array));

		/* Push FAST_CALL on unwind stack */
		fast_call.opcode = ZEND_FAST_CALL;
		fast_call.var_type = IS_TMP_VAR;
		fast_call.var_num = CG(context).fast_call_var;
		fast_call.u.try_catch_offset = try_catch_offset;
		zend_stack_push(&CG(loop_var_stack), &fast_call);
	}

	CG(context).try_catch_offset = try_catch_offset;

	zend_compile_stmt(try_ast);

	if (catches->children != 0) {
		jmp_opnums[0] = zend_emit_jump(0);
	}

	for (i = 0; i < catches->children; ++i) {
		zend_ast *catch_ast = catches->child[i];
		zend_ast_list *classes = zend_ast_get_list(catch_ast->child[0]);
		zend_ast *var_ast = catch_ast->child[1];
		zend_ast *stmt_ast = catch_ast->child[2];
		zend_string *var_name = zval_make_interned_string(zend_ast_get_zval(var_ast));
		zend_bool is_last_catch = (i + 1 == catches->children);

		uint32_t *jmp_multicatch = safe_emalloc(sizeof(uint32_t), classes->children - 1, 0);
		uint32_t opnum_catch;

		CG(zend_lineno) = catch_ast->lineno;

		for (j = 0; j < classes->children; j++) {

			zend_ast *class_ast = classes->child[j];
			zend_bool is_last_class = (j + 1 == classes->children);

			if (!zend_is_const_default_class_ref(class_ast)) {
				zend_error_noreturn(E_COMPILE_ERROR, "Bad class name in the catch statement");
			}

			opnum_catch = get_next_op_number(CG(active_op_array));
			if (i == 0 && j == 0) {
				CG(active_op_array)->try_catch_array[try_catch_offset].catch_op = opnum_catch;
			}

			opline = get_next_op(CG(active_op_array));
			opline->opcode = ZEND_CATCH;
			opline->op1_type = IS_CONST;
			opline->op1.constant = zend_add_class_name_literal(CG(active_op_array),
					zend_resolve_class_name_ast(class_ast));
			opline->extended_value = zend_alloc_cache_slot();

			if (zend_string_equals_literal(var_name, "this")) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot re-assign $this");
			}

			opline->result_type = IS_CV;
			opline->result.var = lookup_cv(CG(active_op_array), var_name);

			if (is_last_catch && is_last_class) {
				opline->extended_value |= ZEND_LAST_CATCH;
			}

			if (!is_last_class) {
				jmp_multicatch[j] = zend_emit_jump(0);
				opline = &CG(active_op_array)->opcodes[opnum_catch];
				opline->op2.opline_num = get_next_op_number(CG(active_op_array));
			}
		}

		for (j = 0; j < classes->children - 1; j++) {
			zend_update_jump_target_to_next(jmp_multicatch[j]);
		}

		efree(jmp_multicatch);

		zend_compile_stmt(stmt_ast);

		if (!is_last_catch) {
			jmp_opnums[i + 1] = zend_emit_jump(0);
		}

		opline = &CG(active_op_array)->opcodes[opnum_catch];
		if (!is_last_catch) {
			opline->op2.opline_num = get_next_op_number(CG(active_op_array));
		}
	}

	for (i = 0; i < catches->children; ++i) {
		zend_update_jump_target_to_next(jmp_opnums[i]);
	}

	if (finally_ast) {
		zend_loop_var discard_exception;
		uint32_t opnum_jmp = get_next_op_number(CG(active_op_array)) + 1;

		/* Pop FAST_CALL from unwind stack */
		zend_stack_del_top(&CG(loop_var_stack));

		/* Push DISCARD_EXCEPTION on unwind stack */
		discard_exception.opcode = ZEND_DISCARD_EXCEPTION;
		discard_exception.var_type = IS_TMP_VAR;
		discard_exception.var_num = CG(context).fast_call_var;
		zend_stack_push(&CG(loop_var_stack), &discard_exception);

		CG(zend_lineno) = finally_ast->lineno;

		opline = zend_emit_op(NULL, ZEND_FAST_CALL, NULL, NULL);
		opline->op1.num = try_catch_offset;
		opline->result_type = IS_TMP_VAR;
		opline->result.var = CG(context).fast_call_var;

		zend_emit_op(NULL, ZEND_JMP, NULL, NULL);

		zend_compile_stmt(finally_ast);

		CG(active_op_array)->try_catch_array[try_catch_offset].finally_op = opnum_jmp + 1;
		CG(active_op_array)->try_catch_array[try_catch_offset].finally_end
			= get_next_op_number(CG(active_op_array));

		opline = zend_emit_op(NULL, ZEND_FAST_RET, NULL, NULL);
		opline->op1_type = IS_TMP_VAR;
		opline->op1.var = CG(context).fast_call_var;
		opline->op2.num = orig_try_catch_offset;

		zend_update_jump_target_to_next(opnum_jmp);

		CG(context).fast_call_var = orig_fast_call_var;

		/* Pop DISCARD_EXCEPTION from unwind stack */
		zend_stack_del_top(&CG(loop_var_stack));
	}

	CG(context).try_catch_offset = orig_try_catch_offset;

	efree(jmp_opnums);
}
/* }}} */

/* Encoding declarations must already be handled during parsing */
zend_bool zend_handle_encoding_declaration(zend_ast *ast) /* {{{ */
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
				zend_throw_exception(zend_ce_compile_error, "Encoding must be a literal", 0);
				return 0;
			}

			if (CG(multibyte)) {
				zend_string *encoding_name = zval_get_string(zend_ast_get_zval(value_ast));

				const zend_encoding *new_encoding, *old_encoding;
				zend_encoding_filter old_input_filter;

				CG(encoding_declared) = 1;

				new_encoding = zend_multibyte_fetch_encoding(ZSTR_VAL(encoding_name));
				if (!new_encoding) {
					zend_error(E_COMPILE_WARNING, "Unsupported encoding [%s]", ZSTR_VAL(encoding_name));
				} else {
					old_input_filter = LANG_SCNG(input_filter);
					old_encoding = LANG_SCNG(script_encoding);
					zend_multibyte_set_filter(new_encoding);

					/* need to re-scan if input filter changed */
					if (old_input_filter != LANG_SCNG(input_filter) ||
						 (old_input_filter && new_encoding != old_encoding)) {
						zend_multibyte_yyinput_again(old_input_filter, old_encoding);
					}
				}

				zend_string_release_ex(encoding_name, 0);
			} else {
				zend_error(E_COMPILE_WARNING, "declare(encoding=...) ignored because "
					"Zend multibyte feature is turned off by settings");
			}
		}
	}

	return 1;
}
/* }}} */

static int zend_declare_is_first_statement(zend_ast *ast) /* {{{ */
{
	uint32_t i = 0;
	zend_ast_list *file_ast = zend_ast_get_list(CG(ast));

	/* Check to see if this declare is preceded only by declare statements */
	while (i < file_ast->children) {
		if (file_ast->child[i] == ast) {
			return SUCCESS;
		} else if (file_ast->child[i] == NULL) {
			/* Empty statements are not allowed prior to a declare */
			return FAILURE;
		} else if (file_ast->child[i]->kind != ZEND_AST_DECLARE) {
			/* declares can only be preceded by other declares */
			return FAILURE;
		}
		i++;
	}
	return FAILURE;
}
/* }}} */

void zend_compile_declare(zend_ast *ast) /* {{{ */
{
	zend_ast_list *declares = zend_ast_get_list(ast->child[0]);
	zend_ast *stmt_ast = ast->child[1];
	zend_declarables orig_declarables = FC(declarables);
	uint32_t i;

	for (i = 0; i < declares->children; ++i) {
		zend_ast *declare_ast = declares->child[i];
		zend_ast *name_ast = declare_ast->child[0];
		zend_ast *value_ast = declare_ast->child[1];
		zend_string *name = zend_ast_get_str(name_ast);

		if (value_ast->kind != ZEND_AST_ZVAL) {
			zend_error_noreturn(E_COMPILE_ERROR, "declare(%s) value must be a literal", ZSTR_VAL(name));
		}

		if (zend_string_equals_literal_ci(name, "ticks")) {
			zval value_zv;
			zend_const_expr_to_zval(&value_zv, value_ast);
			FC(declarables).ticks = zval_get_long(&value_zv);
			zval_ptr_dtor_nogc(&value_zv);
		} else if (zend_string_equals_literal_ci(name, "encoding")) {

			if (FAILURE == zend_declare_is_first_statement(ast)) {
				zend_error_noreturn(E_COMPILE_ERROR, "Encoding declaration pragma must be "
					"the very first statement in the script");
			}
		} else if (zend_string_equals_literal_ci(name, "strict_types")) {
			zval value_zv;

			if (FAILURE == zend_declare_is_first_statement(ast)) {
				zend_error_noreturn(E_COMPILE_ERROR, "strict_types declaration must be "
					"the very first statement in the script");
			}

			if (ast->child[1] != NULL) {
				zend_error_noreturn(E_COMPILE_ERROR, "strict_types declaration must not "
					"use block mode");
			}

			zend_const_expr_to_zval(&value_zv, value_ast);

			if (Z_TYPE(value_zv) != IS_LONG || (Z_LVAL(value_zv) != 0 && Z_LVAL(value_zv) != 1)) {
				zend_error_noreturn(E_COMPILE_ERROR, "strict_types declaration must have 0 or 1 as its value");
			}

			if (Z_LVAL(value_zv) == 1) {
				CG(active_op_array)->fn_flags |= ZEND_ACC_STRICT_TYPES;
			}

		} else {
			zend_error(E_COMPILE_WARNING, "Unsupported declare '%s'", ZSTR_VAL(name));
		}
	}

	if (stmt_ast) {
		zend_compile_stmt(stmt_ast);

		FC(declarables) = orig_declarables;
	}
}
/* }}} */

void zend_compile_stmt_list(zend_ast *ast) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;
	for (i = 0; i < list->children; ++i) {
		zend_compile_stmt(list->child[i]);
	}
}
/* }}} */

ZEND_API void zend_set_function_arg_flags(zend_function *func) /* {{{ */
{
	uint32_t i, n;

	func->common.arg_flags[0] = 0;
	func->common.arg_flags[1] = 0;
	func->common.arg_flags[2] = 0;
	if (func->common.arg_info) {
		n = MIN(func->common.num_args, MAX_ARG_FLAG_NUM);
		i = 0;
		while (i < n) {
			ZEND_SET_ARG_FLAG(func, i + 1, func->common.arg_info[i].pass_by_reference);
			i++;
		}
		if (UNEXPECTED(func->common.fn_flags & ZEND_ACC_VARIADIC && func->common.arg_info[i].pass_by_reference)) {
			uint32_t pass_by_reference = func->common.arg_info[i].pass_by_reference;
			while (i < MAX_ARG_FLAG_NUM) {
				ZEND_SET_ARG_FLAG(func, i + 1, pass_by_reference);
				i++;
			}
		}
	}
}
/* }}} */

static void zend_compile_typename(zend_ast *ast, zend_arg_info *arg_info, zend_bool allow_null) /* {{{ */
{
	if (ast->kind == ZEND_AST_TYPE) {
		arg_info->type = ZEND_TYPE_ENCODE(ast->attr, allow_null);
	} else {
		zend_string *class_name = zend_ast_get_str(ast);
		zend_uchar type = zend_lookup_builtin_type_by_name(class_name);

		if (type != 0) {
			if ((ast->attr & ZEND_NAME_NOT_FQ) != ZEND_NAME_NOT_FQ) {
				zend_error_noreturn(E_COMPILE_ERROR,
					"Type declaration '%s' must be unqualified",
					ZSTR_VAL(zend_string_tolower(class_name)));
			}
			arg_info->type = ZEND_TYPE_ENCODE(type, allow_null);
		} else {
			uint32_t fetch_type = zend_get_class_fetch_type_ast(ast);
			if (fetch_type == ZEND_FETCH_CLASS_DEFAULT) {
				class_name = zend_resolve_class_name_ast(ast);
				zend_assert_valid_class_name(class_name);
			} else {
				zend_ensure_valid_class_fetch_type(fetch_type);
				zend_string_addref(class_name);
			}

			arg_info->type = ZEND_TYPE_ENCODE_CLASS(class_name, allow_null);
		}
	}
}
/* }}} */

void zend_compile_params(zend_ast *ast, zend_ast *return_type_ast) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;
	zend_op_array *op_array = CG(active_op_array);
	zend_arg_info *arg_infos;

	if (return_type_ast) {
		zend_bool allow_null = 0;

		/* Use op_array->arg_info[-1] for return type */
		arg_infos = safe_emalloc(sizeof(zend_arg_info), list->children + 1, 0);
		arg_infos->name = NULL;
		arg_infos->pass_by_reference = (op_array->fn_flags & ZEND_ACC_RETURN_REFERENCE) != 0;
		arg_infos->is_variadic = 0;
		arg_infos->type = 0;

		if (return_type_ast->attr & ZEND_TYPE_NULLABLE) {
			allow_null = 1;
			return_type_ast->attr &= ~ZEND_TYPE_NULLABLE;
		}

		zend_compile_typename(return_type_ast, arg_infos, allow_null);

		if (ZEND_TYPE_CODE(arg_infos->type) == IS_VOID && ZEND_TYPE_ALLOW_NULL(arg_infos->type)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Void type cannot be nullable");
		}

		arg_infos++;
		op_array->fn_flags |= ZEND_ACC_HAS_RETURN_TYPE;
	} else {
		if (list->children == 0) {
			return;
		}
		arg_infos = safe_emalloc(sizeof(zend_arg_info), list->children, 0);
	}

	for (i = 0; i < list->children; ++i) {
		zend_ast *param_ast = list->child[i];
		zend_ast *type_ast = param_ast->child[0];
		zend_ast *var_ast = param_ast->child[1];
		zend_ast *default_ast = param_ast->child[2];
		zend_string *name = zval_make_interned_string(zend_ast_get_zval(var_ast));
		zend_bool is_ref = (param_ast->attr & ZEND_PARAM_REF) != 0;
		zend_bool is_variadic = (param_ast->attr & ZEND_PARAM_VARIADIC) != 0;

		znode var_node, default_node;
		zend_uchar opcode;
		zend_op *opline;
		zend_arg_info *arg_info;

		if (zend_is_auto_global(name)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot re-assign auto-global variable %s",
				ZSTR_VAL(name));
		}

		var_node.op_type = IS_CV;
		var_node.u.op.var = lookup_cv(CG(active_op_array), name);

		if (EX_VAR_TO_NUM(var_node.u.op.var) != i) {
			zend_error_noreturn(E_COMPILE_ERROR, "Redefinition of parameter $%s",
				ZSTR_VAL(name));
		} else if (zend_string_equals_literal(name, "this")) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use $this as parameter");
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
			/* we cannot substitute constants here or it will break ReflectionParameter::getDefaultValueConstantName() and ReflectionParameter::isDefaultValueConstant() */
			uint32_t cops = CG(compiler_options);
			CG(compiler_options) |= ZEND_COMPILE_NO_CONSTANT_SUBSTITUTION | ZEND_COMPILE_NO_PERSISTENT_CONSTANT_SUBSTITUTION;
			opcode = ZEND_RECV_INIT;
			default_node.op_type = IS_CONST;
			zend_const_expr_to_zval(&default_node.u.constant, default_ast);
			CG(compiler_options) = cops;
		} else {
			opcode = ZEND_RECV;
			default_node.op_type = IS_UNUSED;
			op_array->required_num_args = i + 1;
		}

		opline = zend_emit_op(NULL, opcode, NULL, &default_node);
		SET_NODE(opline->result, &var_node);
		opline->op1.num = i + 1;

		arg_info = &arg_infos[i];
		arg_info->name = zend_string_copy(name);
		arg_info->pass_by_reference = is_ref;
		arg_info->is_variadic = is_variadic;
		/* TODO: Keep compatibility, but may be better reset "allow_null" ??? */
		arg_info->type = ZEND_TYPE_ENCODE(0, 1);

		if (type_ast) {
			zend_bool allow_null;
			zend_bool has_null_default = default_ast
				&& (Z_TYPE(default_node.u.constant) == IS_NULL
					|| (Z_TYPE(default_node.u.constant) == IS_CONSTANT_AST
						&& Z_ASTVAL(default_node.u.constant)->kind == ZEND_AST_CONSTANT
						&& strcasecmp(ZSTR_VAL(zend_ast_get_constant_name(Z_ASTVAL(default_node.u.constant))), "NULL") == 0));
			zend_bool is_explicitly_nullable = (type_ast->attr & ZEND_TYPE_NULLABLE) == ZEND_TYPE_NULLABLE;

			op_array->fn_flags |= ZEND_ACC_HAS_TYPE_HINTS;
			allow_null = has_null_default || is_explicitly_nullable;

			type_ast->attr &= ~ZEND_TYPE_NULLABLE;
			zend_compile_typename(type_ast, arg_info, allow_null);

			if (ZEND_TYPE_CODE(arg_info->type) == IS_VOID) {
				zend_error_noreturn(E_COMPILE_ERROR, "void cannot be used as a parameter type");
			}

			if (type_ast->kind == ZEND_AST_TYPE) {
				if (ZEND_TYPE_CODE(arg_info->type) == IS_ARRAY) {
					if (default_ast && !has_null_default
						&& Z_TYPE(default_node.u.constant) != IS_ARRAY
						&& Z_TYPE(default_node.u.constant) != IS_CONSTANT_AST
					) {
						zend_error_noreturn(E_COMPILE_ERROR, "Default value for parameters "
							"with array type can only be an array or NULL");
					}
				} else if (ZEND_TYPE_CODE(arg_info->type) == IS_CALLABLE && default_ast) {
					if (!has_null_default && Z_TYPE(default_node.u.constant) != IS_CONSTANT_AST) {
						zend_error_noreturn(E_COMPILE_ERROR, "Default value for parameters "
							"with callable type can only be NULL");
					}
				}
			} else {
				if (default_ast && !has_null_default && Z_TYPE(default_node.u.constant) != IS_CONSTANT_AST) {
					if (ZEND_TYPE_IS_CLASS(arg_info->type)) {
						zend_error_noreturn(E_COMPILE_ERROR, "Default value for parameters "
							"with a class type can only be NULL");
					} else switch (ZEND_TYPE_CODE(arg_info->type)) {
						case IS_DOUBLE:
							if (Z_TYPE(default_node.u.constant) != IS_DOUBLE && Z_TYPE(default_node.u.constant) != IS_LONG) {
								zend_error_noreturn(E_COMPILE_ERROR, "Default value for parameters "
									"with a float type can only be float, integer, or NULL");
							}
							break;

						case IS_ITERABLE:
							if (Z_TYPE(default_node.u.constant) != IS_ARRAY) {
								zend_error_noreturn(E_COMPILE_ERROR, "Default value for parameters "
									"with iterable type can only be an array or NULL");
							}
							break;

						case IS_OBJECT:
							zend_error_noreturn(E_COMPILE_ERROR, "Default value for parameters "
								"with an object type can only be NULL");
							break;

						default:
							if (!ZEND_SAME_FAKE_TYPE(ZEND_TYPE_CODE(arg_info->type), Z_TYPE(default_node.u.constant))) {
								zend_error_noreturn(E_COMPILE_ERROR, "Default value for parameters "
									"with a %s type can only be %s or NULL",
									zend_get_type_by_const(ZEND_TYPE_CODE(arg_info->type)), zend_get_type_by_const(ZEND_TYPE_CODE(arg_info->type)));
							}
							break;
					}
				}
			}

			/* Allocate cache slot to speed-up run-time class resolution */
			if (opline->opcode == ZEND_RECV_INIT) {
				if (ZEND_TYPE_IS_CLASS(arg_info->type)) {
					opline->extended_value = zend_alloc_cache_slot();
				}
			} else {
				if (ZEND_TYPE_IS_CLASS(arg_info->type)) {
					opline->op2.num = op_array->cache_size;
					op_array->cache_size += sizeof(void*);
				} else {
					opline->op2.num = -1;
				}
			}
		} else {
			if (opline->opcode != ZEND_RECV_INIT) {
				opline->op2.num = -1;
			}
		}
	}

	/* These are assigned at the end to avoid uninitialized memory in case of an error */
	op_array->num_args = list->children;
	op_array->arg_info = arg_infos;

	/* Don't count the variadic argument */
	if (op_array->fn_flags & ZEND_ACC_VARIADIC) {
		op_array->num_args--;
	}
	zend_set_function_arg_flags((zend_function*)op_array);
}
/* }}} */

static void zend_compile_closure_binding(znode *closure, zend_op_array *op_array, zend_ast *uses_ast) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(uses_ast);
	uint32_t i;

	if (!list->children) {
		return;
	}

	if (!op_array->static_variables) {
		op_array->static_variables = zend_new_array(8);
	}

	if (GC_REFCOUNT(op_array->static_variables) > 1) {
		if (!(GC_FLAGS(op_array->static_variables) & IS_ARRAY_IMMUTABLE)) {
			GC_DELREF(op_array->static_variables);
		}
		op_array->static_variables = zend_array_dup(op_array->static_variables);
	}

	for (i = 0; i < list->children; ++i) {
		zend_ast *var_name_ast = list->child[i];
		zend_string *var_name = zval_make_interned_string(zend_ast_get_zval(var_name_ast));
		uint32_t by_ref = var_name_ast->attr;
		zend_op *opline;
		zval *value;

		if (zend_string_equals_literal(var_name, "this")) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use $this as lexical variable");
		}

		if (zend_is_auto_global(var_name)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use auto-global as lexical variable");
		}

		value = zend_hash_add(op_array->static_variables, var_name, &EG(uninitialized_zval));
		if (!value) {
			zend_error_noreturn(E_COMPILE_ERROR,
				"Cannot use variable $%s twice", ZSTR_VAL(var_name));
		}

		opline = zend_emit_op(NULL, ZEND_BIND_LEXICAL, closure, NULL);
		opline->op2_type = IS_CV;
		opline->op2.var = lookup_cv(CG(active_op_array), var_name);
		opline->extended_value = (uint32_t)((char*)value - (char*)op_array->static_variables->arData) | by_ref;
	}
}
/* }}} */

void zend_compile_closure_uses(zend_ast *ast) /* {{{ */
{
	zend_op_array *op_array = CG(active_op_array);
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;

	for (i = 0; i < list->children; ++i) {
		zend_ast *var_ast = list->child[i];
		zend_string *var_name = zend_ast_get_str(var_ast);
		uint32_t by_ref = var_ast->attr;
		zval zv;
		ZVAL_NULL(&zv);

		{
			int i;
			for (i = 0; i < op_array->last_var; i++) {
				if (zend_string_equals(op_array->vars[i], var_name)) {
					zend_error_noreturn(E_COMPILE_ERROR,
						"Cannot use lexical variable $%s as a parameter name", ZSTR_VAL(var_name));
				}
			}
		}

		zend_compile_static_var_common(var_ast, &zv, by_ref);
	}
}
/* }}} */

void zend_begin_method_decl(zend_op_array *op_array, zend_string *name, zend_bool has_body) /* {{{ */
{
	zend_class_entry *ce = CG(active_class_entry);
	zend_bool in_interface = (ce->ce_flags & ZEND_ACC_INTERFACE) != 0;
	zend_bool in_trait = (ce->ce_flags & ZEND_ACC_TRAIT) != 0;
	zend_bool is_public = (op_array->fn_flags & ZEND_ACC_PUBLIC) != 0;
	zend_bool is_static = (op_array->fn_flags & ZEND_ACC_STATIC) != 0;

	zend_string *lcname;

	if (in_interface) {
		if (!is_public || (op_array->fn_flags & (ZEND_ACC_FINAL|ZEND_ACC_ABSTRACT))) {
			zend_error_noreturn(E_COMPILE_ERROR, "Access type for interface method "
				"%s::%s() must be omitted", ZSTR_VAL(ce->name), ZSTR_VAL(name));
		}
		op_array->fn_flags |= ZEND_ACC_ABSTRACT;
	}

	if (op_array->fn_flags & ZEND_ACC_ABSTRACT) {
		if (op_array->fn_flags & ZEND_ACC_PRIVATE) {
			zend_error_noreturn(E_COMPILE_ERROR, "%s function %s::%s() cannot be declared private",
				in_interface ? "Interface" : "Abstract", ZSTR_VAL(ce->name), ZSTR_VAL(name));
		}

		if (has_body) {
			zend_error_noreturn(E_COMPILE_ERROR, "%s function %s::%s() cannot contain body",
				in_interface ? "Interface" : "Abstract", ZSTR_VAL(ce->name), ZSTR_VAL(name));
		}

		ce->ce_flags |= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
	} else if (!has_body) {
		zend_error_noreturn(E_COMPILE_ERROR, "Non-abstract method %s::%s() must contain body",
			ZSTR_VAL(ce->name), ZSTR_VAL(name));
	}

	op_array->scope = ce;
	op_array->function_name = zend_string_copy(name);

	lcname = zend_string_tolower(name);
	lcname = zend_new_interned_string(lcname);

	if (zend_hash_add_ptr(&ce->function_table, lcname, op_array) == NULL) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare %s::%s()",
			ZSTR_VAL(ce->name), ZSTR_VAL(name));
	}

	if (in_interface) {
		if (ZSTR_VAL(lcname)[0] != '_' || ZSTR_VAL(lcname)[1] != '_') {
			/* pass */
		} else if (zend_string_equals_literal(lcname, ZEND_CALL_FUNC_NAME)) {
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
		if (!in_trait && zend_string_equals_ci(lcname, ce->name)) {
			if (!ce->constructor) {
				ce->constructor = (zend_function *) op_array;
			}
		} else if (ZSTR_VAL(lcname)[0] != '_' || ZSTR_VAL(lcname)[1] != '_') {
			if (!is_static) {
				op_array->fn_flags |= ZEND_ACC_ALLOW_STATIC;
			}
		} else if (zend_string_equals_literal(lcname, ZEND_CONSTRUCTOR_FUNC_NAME)) {
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
			ce->ce_flags |= ZEND_ACC_USE_GUARDS;
		} else if (zend_string_equals_literal(lcname, ZEND_SET_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __set() must have "
					"public visibility and cannot be static");
			}
			ce->__set = (zend_function *) op_array;
			ce->ce_flags |= ZEND_ACC_USE_GUARDS;
		} else if (zend_string_equals_literal(lcname, ZEND_UNSET_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __unset() must have "
					"public visibility and cannot be static");
			}
			ce->__unset = (zend_function *) op_array;
			ce->ce_flags |= ZEND_ACC_USE_GUARDS;
		} else if (zend_string_equals_literal(lcname, ZEND_ISSET_FUNC_NAME)) {
			if (!is_public || is_static) {
				zend_error(E_WARNING, "The magic method __isset() must have "
					"public visibility and cannot be static");
			}
			ce->__isset = (zend_function *) op_array;
			ce->ce_flags |= ZEND_ACC_USE_GUARDS;
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

	zend_string_release_ex(lcname, 0);
}
/* }}} */

static void zend_begin_func_decl(znode *result, zend_op_array *op_array, zend_ast_decl *decl) /* {{{ */
{
	zend_ast *params_ast = decl->child[0];
	zend_string *unqualified_name, *name, *lcname, *key;
	zend_op *opline;

	unqualified_name = decl->name;
	op_array->function_name = name = zend_prefix_with_ns(unqualified_name);
	lcname = zend_string_tolower(name);

	if (FC(imports_function)) {
		zend_string *import_name = zend_hash_find_ptr_lc(
			FC(imports_function), ZSTR_VAL(unqualified_name), ZSTR_LEN(unqualified_name));
		if (import_name && !zend_string_equals_ci(lcname, import_name)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot declare function %s "
				"because the name is already in use", ZSTR_VAL(name));
		}
	}

	if (zend_string_equals_literal(lcname, ZEND_AUTOLOAD_FUNC_NAME)) {
		if (zend_ast_get_list(params_ast)->children != 1) {
			zend_error_noreturn(E_COMPILE_ERROR, "%s() must take exactly 1 argument",
				ZEND_AUTOLOAD_FUNC_NAME);
		}

		zend_error(E_DEPRECATED, "__autoload() is deprecated, use spl_autoload_register() instead");
	}

	if (zend_string_equals_literal_ci(unqualified_name, "assert")) {
		zend_error(E_DEPRECATED,
			"Defining a custom assert() function is deprecated, "
			"as the function has special semantics");
	}

	key = zend_build_runtime_definition_key(lcname, decl->lex_pos);
	zend_hash_update_ptr(CG(function_table), key, op_array);
	zend_register_seen_symbol(lcname, ZEND_SYMBOL_FUNCTION);

	if (op_array->fn_flags & ZEND_ACC_CLOSURE) {
		opline = zend_emit_op_tmp(result, ZEND_DECLARE_LAMBDA_FUNCTION, NULL, NULL);
		opline->op1_type = IS_CONST;
		LITERAL_STR(opline->op1, key);
	} else {
		opline = get_next_op(CG(active_op_array));
		opline->opcode = ZEND_DECLARE_FUNCTION;
		opline->op1_type = IS_CONST;
		LITERAL_STR(opline->op1, zend_string_copy(lcname));
		/* RTD key is placed after lcname literal in op1 */
		zend_add_literal_string(CG(active_op_array), &key);
	}

	zend_string_release_ex(lcname, 0);
}
/* }}} */

void zend_compile_func_decl(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast_decl *decl = (zend_ast_decl *) ast;
	zend_ast *params_ast = decl->child[0];
	zend_ast *uses_ast = decl->child[1];
	zend_ast *stmt_ast = decl->child[2];
	zend_ast *return_type_ast = decl->child[3];
	zend_bool is_method = decl->kind == ZEND_AST_METHOD;

	zend_op_array *orig_op_array = CG(active_op_array);
	zend_op_array *op_array = zend_arena_alloc(&CG(arena), sizeof(zend_op_array));
	zend_oparray_context orig_oparray_context;

	init_op_array(op_array, ZEND_USER_FUNCTION, INITIAL_OP_ARRAY_SIZE);

	op_array->fn_flags |= (orig_op_array->fn_flags & ZEND_ACC_STRICT_TYPES);
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
		zend_begin_method_decl(op_array, decl->name, has_body);
	} else {
		zend_begin_func_decl(result, op_array, decl);
		if (uses_ast) {
			zend_compile_closure_binding(result, op_array, uses_ast);
		}
	}

	CG(active_op_array) = op_array;

	zend_oparray_context_begin(&orig_oparray_context);

	if (CG(compiler_options) & ZEND_COMPILE_EXTENDED_INFO) {
		zend_op *opline_ext = zend_emit_op(NULL, ZEND_EXT_NOP, NULL, NULL);
		opline_ext->lineno = decl->start_lineno;
	}

	{
		/* Push a separator to the loop variable stack */
		zend_loop_var dummy_var;
		dummy_var.opcode = ZEND_RETURN;

		zend_stack_push(&CG(loop_var_stack), (void *) &dummy_var);
	}

	zend_compile_params(params_ast, return_type_ast);
	if (CG(active_op_array)->fn_flags & ZEND_ACC_GENERATOR) {
		zend_mark_function_as_generator();
		zend_emit_op(NULL, ZEND_GENERATOR_CREATE, NULL, NULL);
	}
	if (uses_ast) {
		zend_compile_closure_uses(uses_ast);
	}
	zend_compile_stmt(stmt_ast);

	if (is_method) {
		zend_check_magic_method_implementation(
			CG(active_class_entry), (zend_function *) op_array, E_COMPILE_ERROR);
	}

	/* put the implicit return on the really last line */
	CG(zend_lineno) = decl->end_lineno;

	zend_do_extended_info();
	zend_emit_final_return(0);

	pass_two(CG(active_op_array));
	zend_oparray_context_end(&orig_oparray_context);

	/* Pop the loop variable stack separator */
	zend_stack_del_top(&CG(loop_var_stack));

	CG(active_op_array) = orig_op_array;
}
/* }}} */

void zend_compile_prop_decl(zend_ast *ast) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t flags = list->attr;
	zend_class_entry *ce = CG(active_class_entry);
	uint32_t i, children = list->children;

	if (ce->ce_flags & ZEND_ACC_INTERFACE) {
		zend_error_noreturn(E_COMPILE_ERROR, "Interfaces may not include member variables");
	}

	if (flags & ZEND_ACC_ABSTRACT) {
		zend_error_noreturn(E_COMPILE_ERROR, "Properties cannot be declared abstract");
	}

	for (i = 0; i < children; ++i) {
		zend_ast *prop_ast = list->child[i];
		zend_ast *name_ast = prop_ast->child[0];
		zend_ast *value_ast = prop_ast->child[1];
		zend_ast *doc_comment_ast = prop_ast->child[2];
		zend_string *name = zval_make_interned_string(zend_ast_get_zval(name_ast));
		zend_string *doc_comment = NULL;
		zval value_zv;

		/* Doc comment has been appended as last element in ZEND_AST_PROP_ELEM ast */
		if (doc_comment_ast) {
			doc_comment = zend_string_copy(zend_ast_get_str(doc_comment_ast));
		}

		if (flags & ZEND_ACC_FINAL) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot declare property %s::$%s final, "
				"the final modifier is allowed only for methods and classes",
				ZSTR_VAL(ce->name), ZSTR_VAL(name));
		}

		if (zend_hash_exists(&ce->properties_info, name)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare %s::$%s",
				ZSTR_VAL(ce->name), ZSTR_VAL(name));
		}

		if (value_ast) {
			zend_const_expr_to_zval(&value_zv, value_ast);
		} else {
			ZVAL_NULL(&value_zv);
		}

		zend_declare_property_ex(ce, name, &value_zv, flags, doc_comment);
	}
}
/* }}} */

void zend_compile_class_const_decl(zend_ast *ast) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	zend_class_entry *ce = CG(active_class_entry);
	uint32_t i;

	if ((ce->ce_flags & ZEND_ACC_TRAIT) != 0) {
		zend_error_noreturn(E_COMPILE_ERROR, "Traits cannot have constants");
		return;
	}

	for (i = 0; i < list->children; ++i) {
		zend_ast *const_ast = list->child[i];
		zend_ast *name_ast = const_ast->child[0];
		zend_ast *value_ast = const_ast->child[1];
		zend_ast *doc_comment_ast = const_ast->child[2];
		zend_string *name = zval_make_interned_string(zend_ast_get_zval(name_ast));
		zend_string *doc_comment = doc_comment_ast ? zend_string_copy(zend_ast_get_str(doc_comment_ast)) : NULL;
		zval value_zv;

		if (UNEXPECTED(ast->attr & (ZEND_ACC_STATIC|ZEND_ACC_ABSTRACT|ZEND_ACC_FINAL))) {
			if (ast->attr & ZEND_ACC_STATIC) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot use 'static' as constant modifier");
			} else if (ast->attr & ZEND_ACC_ABSTRACT) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot use 'abstract' as constant modifier");
			} else if (ast->attr & ZEND_ACC_FINAL) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot use 'final' as constant modifier");
			}
		}

		zend_const_expr_to_zval(&value_zv, value_ast);
		zend_declare_class_constant_ex(ce, name, &value_zv, ast->attr, doc_comment);
	}
}
/* }}} */

static void zend_compile_method_ref(zend_ast *ast, zend_trait_method_reference *method_ref) /* {{{ */
{
	zend_ast *class_ast = ast->child[0];
	zend_ast *method_ast = ast->child[1];

	method_ref->method_name = zend_string_copy(zend_ast_get_str(method_ast));

	if (class_ast) {
		method_ref->class_name = zend_resolve_class_name_ast(class_ast);
	} else {
		method_ref->class_name = NULL;
	}
}
/* }}} */

static void zend_compile_trait_precedence(zend_ast *ast) /* {{{ */
{
	zend_ast *method_ref_ast = ast->child[0];
	zend_ast *insteadof_ast = ast->child[1];
	zend_ast_list *insteadof_list = zend_ast_get_list(insteadof_ast);
	uint32_t i;

	zend_trait_precedence *precedence = emalloc(sizeof(zend_trait_precedence) + (insteadof_list->children - 1) * sizeof(zend_string*));
	zend_compile_method_ref(method_ref_ast, &precedence->trait_method);
	precedence->num_excludes = insteadof_list->children;

	for (i = 0; i < insteadof_list->children; ++i) {
		zend_ast *name_ast = insteadof_list->child[i];
		precedence->exclude_class_names[i] = zend_resolve_class_name_ast(name_ast);
	}

	zend_add_to_list(&CG(active_class_entry)->trait_precedences, precedence);
}
/* }}} */

static void zend_compile_trait_alias(zend_ast *ast) /* {{{ */
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
	zend_compile_method_ref(method_ref_ast, &alias->trait_method);
	alias->modifiers = modifiers;

	if (alias_ast) {
		alias->alias = zend_string_copy(zend_ast_get_str(alias_ast));
	} else {
		alias->alias = NULL;
	}

	zend_add_to_list(&CG(active_class_entry)->trait_aliases, alias);
}
/* }}} */

void zend_compile_use_trait(zend_ast *ast) /* {{{ */
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
				"%s is used in %s", ZSTR_VAL(name), ZSTR_VAL(ce->name));
		}

		switch (zend_get_class_fetch_type(name)) {
			case ZEND_FETCH_CLASS_SELF:
			case ZEND_FETCH_CLASS_PARENT:
			case ZEND_FETCH_CLASS_STATIC:
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot use '%s' as trait name "
					"as it is reserved", ZSTR_VAL(name));
				break;
		}

		opline = get_next_op(CG(active_op_array));
		opline->opcode = ZEND_ADD_TRAIT;
		SET_NODE(opline->op1, &FC(implementing_class));
		opline->op2_type = IS_CONST;
		opline->op2.constant = zend_add_class_name_literal(CG(active_op_array),
			zend_resolve_class_name_ast(trait_ast));

		ce->num_traits++;
	}

	if (!adaptations) {
		return;
	}

	for (i = 0; i < adaptations->children; ++i) {
		zend_ast *adaptation_ast = adaptations->child[i];
		switch (adaptation_ast->kind) {
			case ZEND_AST_TRAIT_PRECEDENCE:
				zend_compile_trait_precedence(adaptation_ast);
				break;
			case ZEND_AST_TRAIT_ALIAS:
				zend_compile_trait_alias(adaptation_ast);
				break;
			EMPTY_SWITCH_DEFAULT_CASE()
		}
	}
}
/* }}} */

void zend_compile_implements(znode *class_node, zend_ast *ast) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;
	for (i = 0; i < list->children; ++i) {
		zend_ast *class_ast = list->child[i];
		zend_string *name = zend_ast_get_str(class_ast);

		zend_op *opline;

		if (!zend_is_const_default_class_ref(class_ast)) {
			zend_error_noreturn(E_COMPILE_ERROR,
				"Cannot use '%s' as interface name as it is reserved", ZSTR_VAL(name));
		}

		opline = zend_emit_op(NULL, ZEND_ADD_INTERFACE, class_node, NULL);
		opline->op2_type = IS_CONST;
		opline->op2.constant = zend_add_class_name_literal(CG(active_op_array),
			zend_resolve_class_name_ast(class_ast));

		CG(active_class_entry)->num_interfaces++;
	}
}
/* }}} */

static zend_string *zend_generate_anon_class_name(unsigned char *lex_pos) /* {{{ */
{
	zend_string *result;
	char char_pos_buf[32];
	size_t char_pos_len = zend_sprintf(char_pos_buf, "%p", lex_pos);
	zend_string *filename = CG(active_op_array)->filename;

	/* NULL, name length, filename length, last accepting char position length */
	result = zend_string_alloc(sizeof("class@anonymous") + ZSTR_LEN(filename) + char_pos_len, 0);
	sprintf(ZSTR_VAL(result), "class@anonymous%c%s%s", '\0', ZSTR_VAL(filename), char_pos_buf);
	return zend_new_interned_string(result);
}
/* }}} */

void zend_compile_class_decl(zend_ast *ast) /* {{{ */
{
	zend_ast_decl *decl = (zend_ast_decl *) ast;
	zend_ast *extends_ast = decl->child[0];
	zend_ast *implements_ast = decl->child[1];
	zend_ast *stmt_ast = decl->child[2];
	zend_string *name, *lcname;
	zend_class_entry *ce = zend_arena_alloc(&CG(arena), sizeof(zend_class_entry));
	zend_op *opline;
	znode declare_node;
	int extends_const;

	zend_class_entry *original_ce = CG(active_class_entry);
	znode original_implementing_class = FC(implementing_class);

	if (EXPECTED((decl->flags & ZEND_ACC_ANON_CLASS) == 0)) {
		zend_string *unqualified_name = decl->name;

		if (CG(active_class_entry)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Class declarations may not be nested");
		}

		zend_assert_valid_class_name(unqualified_name);
		name = zend_prefix_with_ns(unqualified_name);
		name = zend_new_interned_string(name);
		lcname = zend_string_tolower(name);

		if (FC(imports)) {
			zend_string *import_name = zend_hash_find_ptr_lc(
				FC(imports), ZSTR_VAL(unqualified_name), ZSTR_LEN(unqualified_name));
			if (import_name && !zend_string_equals_ci(lcname, import_name)) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot declare class %s "
						"because the name is already in use", ZSTR_VAL(name));
			}
		}

		zend_register_seen_symbol(lcname, ZEND_SYMBOL_CLASS);
	} else {
		name = zend_generate_anon_class_name(decl->lex_pos);
		lcname = zend_string_tolower(name);
	}
	lcname = zend_new_interned_string(lcname);

	ce->type = ZEND_USER_CLASS;
	ce->name = name;
	zend_initialize_class_data(ce, 1);

	ce->ce_flags |= decl->flags;
	ce->info.user.filename = zend_get_compiled_filename();
	ce->info.user.line_start = decl->start_lineno;
	ce->info.user.line_end = decl->end_lineno;

	if (decl->doc_comment) {
		ce->info.user.doc_comment = zend_string_copy(decl->doc_comment);
	}

	if (UNEXPECTED((decl->flags & ZEND_ACC_ANON_CLASS))) {
		/* Serialization is not supported for anonymous classes */
		ce->serialize = zend_class_serialize_deny;
		ce->unserialize = zend_class_unserialize_deny;
	}

	if (extends_ast) {
		znode extends_node;
		zend_string *extends_name;

		if (!zend_is_const_default_class_ref(extends_ast)) {
			extends_name = zend_ast_get_str(extends_ast);
			zend_error_noreturn(E_COMPILE_ERROR,
				"Cannot use '%s' as class name as it is reserved", ZSTR_VAL(extends_name));
		}

		zend_compile_expr(&extends_node, extends_ast);
		if (extends_node.op_type != IS_CONST || Z_TYPE(extends_node.u.constant) != IS_STRING) {
			zend_error_noreturn(E_COMPILE_ERROR, "Illegal class name");
		}
		extends_name = Z_STR(extends_node.u.constant);
		extends_const = zend_add_class_name_literal(CG(active_op_array),
				zend_resolve_class_name(extends_name,
					extends_ast->kind == ZEND_AST_ZVAL ? extends_ast->attr : ZEND_NAME_FQ));
		zend_string_release_ex(extends_name, 0);
		ce->ce_flags |= ZEND_ACC_INHERITED;
	}

	opline = get_next_op(CG(active_op_array));
	zend_make_var_result(&declare_node, opline);

	GET_NODE(&FC(implementing_class), opline->result);

	opline->op1_type = IS_CONST;
	LITERAL_STR(opline->op1, lcname);

	if (decl->flags & ZEND_ACC_ANON_CLASS) {
		if (extends_ast) {
			opline->opcode = ZEND_DECLARE_ANON_INHERITED_CLASS;
			opline->op2_type = IS_CONST;
			opline->op2.constant = extends_const;
		} else {
			opline->opcode = ZEND_DECLARE_ANON_CLASS;
		}

		if (!zend_hash_exists(CG(class_table), lcname)) {
			zend_hash_add_ptr(CG(class_table), lcname, ce);
		} else {
			/* this anonymous class has been included */
			zval zv;
			ZVAL_PTR(&zv, ce);
			destroy_zend_class(&zv);
			return;
		}
	} else {
		zend_string *key;

		if (extends_ast) {
			opline->opcode = ZEND_DECLARE_INHERITED_CLASS;
			opline->op2_type = IS_CONST;
			opline->op2.constant = extends_const;
		} else {
			opline->opcode = ZEND_DECLARE_CLASS;
		}

		key = zend_build_runtime_definition_key(lcname, decl->lex_pos);
		/* RTD key is placed after lcname literal in op1 */
		zend_add_literal_string(CG(active_op_array), &key);

		zend_hash_update_ptr(CG(class_table), key, ce);
	}

	CG(active_class_entry) = ce;

	zend_compile_stmt(stmt_ast);

	/* Reset lineno for final opcodes and errors */
	CG(zend_lineno) = ast->lineno;

	if (ce->num_traits == 0) {
		/* For traits this check is delayed until after trait binding */
		zend_check_deprecated_constructor(ce);
	}

	if (ce->constructor) {
		ce->constructor->common.fn_flags |= ZEND_ACC_CTOR;
		if (ce->constructor->common.fn_flags & ZEND_ACC_STATIC) {
			zend_error_noreturn(E_COMPILE_ERROR, "Constructor %s::%s() cannot be static",
				ZSTR_VAL(ce->name), ZSTR_VAL(ce->constructor->common.function_name));
		}
		if (ce->constructor->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
			zend_error_noreturn(E_COMPILE_ERROR,
				"Constructor %s::%s() cannot declare a return type",
				ZSTR_VAL(ce->name), ZSTR_VAL(ce->constructor->common.function_name));
		}
	}
	if (ce->destructor) {
		ce->destructor->common.fn_flags |= ZEND_ACC_DTOR;
		if (ce->destructor->common.fn_flags & ZEND_ACC_STATIC) {
			zend_error_noreturn(E_COMPILE_ERROR, "Destructor %s::%s() cannot be static",
				ZSTR_VAL(ce->name), ZSTR_VAL(ce->destructor->common.function_name));
		} else if (ce->destructor->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
			zend_error_noreturn(E_COMPILE_ERROR,
				"Destructor %s::%s() cannot declare a return type",
				ZSTR_VAL(ce->name), ZSTR_VAL(ce->destructor->common.function_name));
		}
	}
	if (ce->clone) {
		if (ce->clone->common.fn_flags & ZEND_ACC_STATIC) {
			zend_error_noreturn(E_COMPILE_ERROR, "Clone method %s::%s() cannot be static",
				ZSTR_VAL(ce->name), ZSTR_VAL(ce->clone->common.function_name));
		} else if (ce->clone->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
			zend_error_noreturn(E_COMPILE_ERROR,
				"%s::%s() cannot declare a return type",
				ZSTR_VAL(ce->name), ZSTR_VAL(ce->clone->common.function_name));
		}
	}

	/* Check for traits and proceed like with interfaces.
	 * The only difference will be a combined handling of them in the end.
	 * Thus, we need another opcode here. */
	if (ce->num_traits > 0) {
		ce->traits = NULL;
		ce->num_traits = 0;
		ce->ce_flags |= ZEND_ACC_IMPLEMENT_TRAITS;

		zend_emit_op(NULL, ZEND_BIND_TRAITS, &declare_node, NULL);
	}

	if (implements_ast) {
		zend_compile_implements(&declare_node, implements_ast);
	}

	if (!(ce->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS))
		&& (extends_ast || implements_ast)
	) {
		zend_verify_abstract_class(ce);
		if (implements_ast) {
			zend_emit_op(NULL, ZEND_VERIFY_ABSTRACT_CLASS, &declare_node, NULL);
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

	FC(implementing_class) = original_implementing_class;
	CG(active_class_entry) = original_ce;
}
/* }}} */

static HashTable *zend_get_import_ht(uint32_t type) /* {{{ */
{
	switch (type) {
		case ZEND_SYMBOL_CLASS:
			if (!FC(imports)) {
				FC(imports) = emalloc(sizeof(HashTable));
				zend_hash_init(FC(imports), 8, NULL, str_dtor, 0);
			}
			return FC(imports);
		case ZEND_SYMBOL_FUNCTION:
			if (!FC(imports_function)) {
				FC(imports_function) = emalloc(sizeof(HashTable));
				zend_hash_init(FC(imports_function), 8, NULL, str_dtor, 0);
			}
			return FC(imports_function);
		case ZEND_SYMBOL_CONST:
			if (!FC(imports_const)) {
				FC(imports_const) = emalloc(sizeof(HashTable));
				zend_hash_init(FC(imports_const), 8, NULL, str_dtor, 0);
			}
			return FC(imports_const);
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	return NULL;
}
/* }}} */

static char *zend_get_use_type_str(uint32_t type) /* {{{ */
{
	switch (type) {
		case ZEND_SYMBOL_CLASS:
			return "";
		case ZEND_SYMBOL_FUNCTION:
			return " function";
		case ZEND_SYMBOL_CONST:
			return " const";
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	return " unknown";
}
/* }}} */

static void zend_check_already_in_use(uint32_t type, zend_string *old_name, zend_string *new_name, zend_string *check_name) /* {{{ */
{
	if (zend_string_equals_ci(old_name, check_name)) {
		return;
	}

	zend_error_noreturn(E_COMPILE_ERROR, "Cannot use%s %s as %s because the name "
		"is already in use", zend_get_use_type_str(type), ZSTR_VAL(old_name), ZSTR_VAL(new_name));
}
/* }}} */

void zend_compile_use(zend_ast *ast) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;
	zend_string *current_ns = FC(current_namespace);
	uint32_t type = ast->attr;
	HashTable *current_import = zend_get_import_ht(type);
	zend_bool case_sensitive = type == ZEND_SYMBOL_CONST;

	for (i = 0; i < list->children; ++i) {
		zend_ast *use_ast = list->child[i];
		zend_ast *old_name_ast = use_ast->child[0];
		zend_ast *new_name_ast = use_ast->child[1];
		zend_string *old_name = zend_ast_get_str(old_name_ast);
		zend_string *new_name, *lookup_name;

		if (new_name_ast) {
			new_name = zend_string_copy(zend_ast_get_str(new_name_ast));
		} else {
			const char *unqualified_name;
			size_t unqualified_name_len;
			if (zend_get_unqualified_name(old_name, &unqualified_name, &unqualified_name_len)) {
				/* The form "use A\B" is equivalent to "use A\B as B" */
				new_name = zend_string_init(unqualified_name, unqualified_name_len, 0);
			} else {
				new_name = zend_string_copy(old_name);

				if (!current_ns) {
					if (type == T_CLASS && zend_string_equals_literal(new_name, "strict")) {
						zend_error_noreturn(E_COMPILE_ERROR,
							"You seem to be trying to use a different language...");
					}

					zend_error(E_WARNING, "The use statement with non-compound name '%s' "
						"has no effect", ZSTR_VAL(new_name));
				}
			}
		}

		if (case_sensitive) {
			lookup_name = zend_string_copy(new_name);
		} else {
			lookup_name = zend_string_tolower(new_name);
		}

		if (type == ZEND_SYMBOL_CLASS && zend_is_reserved_class_name(new_name)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use %s as %s because '%s' "
				"is a special class name", ZSTR_VAL(old_name), ZSTR_VAL(new_name), ZSTR_VAL(new_name));
		}

		if (current_ns) {
			zend_string *ns_name = zend_string_alloc(ZSTR_LEN(current_ns) + 1 + ZSTR_LEN(new_name), 0);
			zend_str_tolower_copy(ZSTR_VAL(ns_name), ZSTR_VAL(current_ns), ZSTR_LEN(current_ns));
			ZSTR_VAL(ns_name)[ZSTR_LEN(current_ns)] = '\\';
			memcpy(ZSTR_VAL(ns_name) + ZSTR_LEN(current_ns) + 1, ZSTR_VAL(lookup_name), ZSTR_LEN(lookup_name) + 1);

			if (zend_have_seen_symbol(ns_name, type)) {
				zend_check_already_in_use(type, old_name, new_name, ns_name);
			}

			zend_string_efree(ns_name);
		} else {
			if (zend_have_seen_symbol(lookup_name, type)) {
				zend_check_already_in_use(type, old_name, new_name, lookup_name);
			}
		}

		zend_string_addref(old_name);
		if (!zend_hash_add_ptr(current_import, lookup_name, old_name)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use%s %s as %s because the name "
				"is already in use", zend_get_use_type_str(type), ZSTR_VAL(old_name), ZSTR_VAL(new_name));
		}

		zend_string_release_ex(lookup_name, 0);
		zend_string_release_ex(new_name, 0);
	}
}
/* }}} */

void zend_compile_group_use(zend_ast *ast) /* {{{ */
{
	uint32_t i;
	zend_string *ns = zend_ast_get_str(ast->child[0]);
	zend_ast_list *list = zend_ast_get_list(ast->child[1]);

	for (i = 0; i < list->children; i++) {
		zend_ast *inline_use, *use = list->child[i];
		zval *name_zval = zend_ast_get_zval(use->child[0]);
		zend_string *name = Z_STR_P(name_zval);
		zend_string *compound_ns = zend_concat_names(ZSTR_VAL(ns), ZSTR_LEN(ns), ZSTR_VAL(name), ZSTR_LEN(name));
		zend_string_release_ex(name, 0);
		ZVAL_STR(name_zval, compound_ns);
		inline_use = zend_ast_create_list(1, ZEND_AST_USE, use);
		inline_use->attr = ast->attr ? ast->attr : use->attr;
		zend_compile_use(inline_use);
	}
}
/* }}} */

void zend_compile_const_decl(zend_ast *ast) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;
	for (i = 0; i < list->children; ++i) {
		zend_ast *const_ast = list->child[i];
		zend_ast *name_ast = const_ast->child[0];
		zend_ast *value_ast = const_ast->child[1];
		zend_string *unqualified_name = zend_ast_get_str(name_ast);

		zend_string *name;
		znode name_node, value_node;
		zval *value_zv = &value_node.u.constant;

		value_node.op_type = IS_CONST;
		zend_const_expr_to_zval(value_zv, value_ast);

		if (zend_lookup_reserved_const(ZSTR_VAL(unqualified_name), ZSTR_LEN(unqualified_name))) {
			zend_error_noreturn(E_COMPILE_ERROR,
				"Cannot redeclare constant '%s'", ZSTR_VAL(unqualified_name));
		}

		name = zend_prefix_with_ns(unqualified_name);
		name = zend_new_interned_string(name);

		if (FC(imports_const)) {
			zend_string *import_name = zend_hash_find_ptr(FC(imports_const), unqualified_name);
			if (import_name && !zend_string_equals(import_name, name)) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot declare const %s because "
					"the name is already in use", ZSTR_VAL(name));
			}
		}

		name_node.op_type = IS_CONST;
		ZVAL_STR(&name_node.u.constant, name);

		zend_emit_op(NULL, ZEND_DECLARE_CONST, &name_node, &value_node);

		zend_register_seen_symbol(name, ZEND_SYMBOL_CONST);
	}
}
/* }}}*/

void zend_compile_namespace(zend_ast *ast) /* {{{ */
{
	zend_ast *name_ast = ast->child[0];
	zend_ast *stmt_ast = ast->child[1];
	zend_string *name;
	zend_bool with_bracket = stmt_ast != NULL;

	/* handle mixed syntax declaration or nested namespaces */
	if (!FC(has_bracketed_namespaces)) {
		if (FC(current_namespace)) {
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
		} else if (FC(current_namespace) || FC(in_namespace)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Namespace declarations cannot be nested");
		}
	}

	if (((!with_bracket && !FC(current_namespace))
		 || (with_bracket && !FC(has_bracketed_namespaces))) && CG(active_op_array)->last > 0
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
				"the very first statement or after any declare call in the script");
		}
	}

	if (FC(current_namespace)) {
		zend_string_release_ex(FC(current_namespace), 0);
	}

	if (name_ast) {
		name = zend_ast_get_str(name_ast);

		if (ZEND_FETCH_CLASS_DEFAULT != zend_get_class_fetch_type(name)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot use '%s' as namespace name", ZSTR_VAL(name));
		}

		FC(current_namespace) = zend_string_copy(name);
	} else {
		FC(current_namespace) = NULL;
	}

	zend_reset_import_tables();

	FC(in_namespace) = 1;
	if (with_bracket) {
		FC(has_bracketed_namespaces) = 1;
	}

	if (stmt_ast) {
		zend_compile_top_stmt(stmt_ast);
		zend_end_namespace();
	}
}
/* }}} */

void zend_compile_halt_compiler(zend_ast *ast) /* {{{ */
{
	zend_ast *offset_ast = ast->child[0];
	zend_long offset = Z_LVAL_P(zend_ast_get_zval(offset_ast));

	zend_string *filename, *name;
	const char const_name[] = "__COMPILER_HALT_OFFSET__";

	if (FC(has_bracketed_namespaces) && FC(in_namespace)) {
		zend_error_noreturn(E_COMPILE_ERROR,
			"__HALT_COMPILER() can only be used from the outermost scope");
	}

	filename = zend_get_compiled_filename();
	name = zend_mangle_property_name(const_name, sizeof(const_name) - 1,
		ZSTR_VAL(filename), ZSTR_LEN(filename), 0);

	zend_register_long_constant(ZSTR_VAL(name), ZSTR_LEN(name), offset, CONST_CS, 0);
	zend_string_release_ex(name, 0);
}
/* }}} */

static zend_bool zend_try_ct_eval_magic_const(zval *zv, zend_ast *ast) /* {{{ */
{
	zend_op_array *op_array = CG(active_op_array);
	zend_class_entry *ce = CG(active_class_entry);

	switch (ast->attr) {
		case T_LINE:
			ZVAL_LONG(zv, ast->lineno);
			break;
		case T_FILE:
			ZVAL_STR_COPY(zv, CG(compiled_filename));
			break;
		case T_DIR:
		{
			zend_string *filename = CG(compiled_filename);
			zend_string *dirname = zend_string_init(ZSTR_VAL(filename), ZSTR_LEN(filename), 0);
#ifdef ZEND_WIN32
			ZSTR_LEN(dirname) = php_win32_ioutil_dirname(ZSTR_VAL(dirname), ZSTR_LEN(dirname));
#else
			ZSTR_LEN(dirname) = zend_dirname(ZSTR_VAL(dirname), ZSTR_LEN(dirname));
#endif

			if (strcmp(ZSTR_VAL(dirname), ".") == 0) {
				dirname = zend_string_extend(dirname, MAXPATHLEN, 0);
#if HAVE_GETCWD
				ZEND_IGNORE_VALUE(VCWD_GETCWD(ZSTR_VAL(dirname), MAXPATHLEN));
#elif HAVE_GETWD
				ZEND_IGNORE_VALUE(VCWD_GETWD(ZSTR_VAL(dirname)));
#endif
				ZSTR_LEN(dirname) = strlen(ZSTR_VAL(dirname));
			}

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
			if ((op_array && !op_array->scope && op_array->function_name) || (op_array->fn_flags & ZEND_ACC_CLOSURE)) {
				ZVAL_STR_COPY(zv, op_array->function_name);
			} else if (ce) {
				if (op_array && op_array->function_name) {
					ZVAL_NEW_STR(zv, zend_concat3(ZSTR_VAL(ce->name), ZSTR_LEN(ce->name), "::", 2,
						ZSTR_VAL(op_array->function_name), ZSTR_LEN(op_array->function_name)));
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
				if ((ce->ce_flags & ZEND_ACC_TRAIT) != 0) {
					return 0;
				} else {
					ZVAL_STR_COPY(zv, ce->name);
				}
			} else {
				ZVAL_EMPTY_STRING(zv);
			}
			break;
		case T_TRAIT_C:
			if (ce && (ce->ce_flags & ZEND_ACC_TRAIT) != 0) {
				ZVAL_STR_COPY(zv, ce->name);
			} else {
				ZVAL_EMPTY_STRING(zv);
			}
			break;
		case T_NS_C:
			if (FC(current_namespace)) {
				ZVAL_STR_COPY(zv, FC(current_namespace));
			} else {
				ZVAL_EMPTY_STRING(zv);
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	return 1;
}
/* }}} */

ZEND_API zend_bool zend_binary_op_produces_numeric_string_error(uint32_t opcode, zval *op1, zval *op2) /* {{{ */
{
	if (!(opcode == ZEND_ADD || opcode == ZEND_SUB || opcode == ZEND_MUL || opcode == ZEND_DIV
		|| opcode == ZEND_POW || opcode == ZEND_MOD || opcode == ZEND_SL || opcode == ZEND_SR
		|| opcode == ZEND_BW_OR || opcode == ZEND_BW_AND || opcode == ZEND_BW_XOR)) {
		return 0;
	}

	/* While basic arithmetic operators always produce numeric string errors,
	 * bitwise operators don't produce errors if both operands are strings */
	if ((opcode == ZEND_BW_OR || opcode == ZEND_BW_AND || opcode == ZEND_BW_XOR)
		&& Z_TYPE_P(op1) == IS_STRING && Z_TYPE_P(op2) == IS_STRING) {
		return 0;
	}

	if (Z_TYPE_P(op1) == IS_STRING
		&& !is_numeric_string(Z_STRVAL_P(op1), Z_STRLEN_P(op1), NULL, NULL, 0)) {
		return 1;
	}

	if (Z_TYPE_P(op2) == IS_STRING
		&& !is_numeric_string(Z_STRVAL_P(op2), Z_STRLEN_P(op2), NULL, NULL, 0)) {
		return 1;
	}

	return 0;
}
/* }}} */

static inline zend_bool zend_try_ct_eval_binary_op(zval *result, uint32_t opcode, zval *op1, zval *op2) /* {{{ */
{
	binary_op_type fn = get_binary_op(opcode);

	/* don't evaluate division by zero at compile-time */
	if ((opcode == ZEND_DIV || opcode == ZEND_MOD) &&
	    zval_get_long(op2) == 0) {
		return 0;
	} else if ((opcode == ZEND_SL || opcode == ZEND_SR) &&
	    zval_get_long(op2) < 0) {
		return 0;
	}

	/* don't evaluate numeric string error-producing operations at compile-time */
	if (zend_binary_op_produces_numeric_string_error(opcode, op1, op2)) {
		return 0;
	}

	fn(result, op1, op2);
	return 1;
}
/* }}} */

static inline void zend_ct_eval_unary_op(zval *result, uint32_t opcode, zval *op) /* {{{ */
{
	unary_op_type fn = get_unary_op(opcode);
	fn(result, op);
}
/* }}} */

static inline zend_bool zend_try_ct_eval_unary_pm(zval *result, zend_ast_kind kind, zval *op) /* {{{ */
{
	zval left;
	ZVAL_LONG(&left, (kind == ZEND_AST_UNARY_PLUS) ? 1 : -1);
	return zend_try_ct_eval_binary_op(result, ZEND_MUL, &left, op);
}
/* }}} */

static inline void zend_ct_eval_greater(zval *result, zend_ast_kind kind, zval *op1, zval *op2) /* {{{ */
{
	binary_op_type fn = kind == ZEND_AST_GREATER
		? is_smaller_function : is_smaller_or_equal_function;
	fn(result, op2, op1);
}
/* }}} */

static zend_bool zend_try_ct_eval_array(zval *result, zend_ast *ast) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t i;
	zend_bool is_constant = 1;

	if (ast->attr == ZEND_ARRAY_SYNTAX_LIST) {
		zend_error(E_COMPILE_ERROR, "Cannot use list() as standalone expression");
	}

	/* First ensure that *all* child nodes are constant and by-val */
	for (i = 0; i < list->children; ++i) {
		zend_ast *elem_ast = list->child[i];

		if (elem_ast == NULL) {
			zend_error(E_COMPILE_ERROR, "Cannot use empty array elements in arrays");
		}

		zend_eval_const_expr(&elem_ast->child[0]);
		zend_eval_const_expr(&elem_ast->child[1]);

		if (elem_ast->attr /* by_ref */ || elem_ast->child[0]->kind != ZEND_AST_ZVAL
			|| (elem_ast->child[1] && elem_ast->child[1]->kind != ZEND_AST_ZVAL)
		) {
			is_constant = 0;
		}
	}

	if (!is_constant) {
		return 0;
	}

	if (!list->children) {
		ZVAL_EMPTY_ARRAY(result);
		return 1;
	}

	array_init_size(result, list->children);
	for (i = 0; i < list->children; ++i) {
		zend_ast *elem_ast = list->child[i];
		zend_ast *value_ast = elem_ast->child[0];
		zend_ast *key_ast = elem_ast->child[1];

		zval *value = zend_ast_get_zval(value_ast);
		Z_TRY_ADDREF_P(value);

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
					zend_hash_update(Z_ARRVAL_P(result), ZSTR_EMPTY_ALLOC(), value);
					break;
				default:
					zend_error_noreturn(E_COMPILE_ERROR, "Illegal offset type");
					break;
			}
		} else {
			if (!zend_hash_next_index_insert(Z_ARRVAL_P(result), value)) {
				zval_ptr_dtor_nogc(value);
				zval_ptr_dtor(result);
				return 0;
			}
		}
	}

	return 1;
}
/* }}} */

void zend_compile_binary_op(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *left_ast = ast->child[0];
	zend_ast *right_ast = ast->child[1];
	uint32_t opcode = ast->attr;

	znode left_node, right_node;
	zend_compile_expr(&left_node, left_ast);
	zend_compile_expr(&right_node, right_ast);

	if (left_node.op_type == IS_CONST && right_node.op_type == IS_CONST) {
		if (zend_try_ct_eval_binary_op(&result->u.constant, opcode,
				&left_node.u.constant, &right_node.u.constant)
		) {
			result->op_type = IS_CONST;
			zval_ptr_dtor(&left_node.u.constant);
			zval_ptr_dtor(&right_node.u.constant);
			return;
		}
	}

	do {
		if (opcode == ZEND_IS_EQUAL || opcode == ZEND_IS_NOT_EQUAL) {
			if (left_node.op_type == IS_CONST) {
				if (Z_TYPE(left_node.u.constant) == IS_FALSE) {
					opcode = (opcode == ZEND_IS_NOT_EQUAL) ? ZEND_BOOL : ZEND_BOOL_NOT;
					zend_emit_op_tmp(result, opcode, &right_node, NULL);
					break;
				} else if (Z_TYPE(left_node.u.constant) == IS_TRUE) {
					opcode = (opcode == ZEND_IS_EQUAL) ? ZEND_BOOL : ZEND_BOOL_NOT;
					zend_emit_op_tmp(result, opcode, &right_node, NULL);
					break;
				}
			} else if (right_node.op_type == IS_CONST) {
				if (Z_TYPE(right_node.u.constant) == IS_FALSE) {
					opcode = (opcode == ZEND_IS_NOT_EQUAL) ? ZEND_BOOL : ZEND_BOOL_NOT;
					zend_emit_op_tmp(result, opcode, &left_node, NULL);
					break;
				} else if (Z_TYPE(right_node.u.constant) == IS_TRUE) {
					opcode = (opcode == ZEND_IS_EQUAL) ? ZEND_BOOL : ZEND_BOOL_NOT;
					zend_emit_op_tmp(result, opcode, &left_node, NULL);
					break;
				}
			}
		}
		if (opcode == ZEND_CONCAT) {
			/* convert constant operands to strings at compile-time */
			if (left_node.op_type == IS_CONST) {
				convert_to_string(&left_node.u.constant);
			}
			if (right_node.op_type == IS_CONST) {
				convert_to_string(&right_node.u.constant);
			}
			if (left_node.op_type == IS_CONST && right_node.op_type == IS_CONST) {
				opcode = ZEND_FAST_CONCAT;
			}
		}
		zend_emit_op_tmp(result, opcode, &left_node, &right_node);
	} while (0);
}
/* }}} */

/* We do not use zend_compile_binary_op for this because we want to retain the left-to-right
 * evaluation order. */
void zend_compile_greater(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *left_ast = ast->child[0];
	zend_ast *right_ast = ast->child[1];
	znode left_node, right_node;

	ZEND_ASSERT(ast->kind == ZEND_AST_GREATER || ast->kind == ZEND_AST_GREATER_EQUAL);

	zend_compile_expr(&left_node, left_ast);
	zend_compile_expr(&right_node, right_ast);

	if (left_node.op_type == IS_CONST && right_node.op_type == IS_CONST) {
		result->op_type = IS_CONST;
		zend_ct_eval_greater(&result->u.constant, ast->kind,
			&left_node.u.constant, &right_node.u.constant);
		zval_ptr_dtor(&left_node.u.constant);
		zval_ptr_dtor(&right_node.u.constant);
		return;
	}

	zend_emit_op_tmp(result,
		ast->kind == ZEND_AST_GREATER ? ZEND_IS_SMALLER : ZEND_IS_SMALLER_OR_EQUAL,
		&right_node, &left_node);
}
/* }}} */

void zend_compile_unary_op(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	uint32_t opcode = ast->attr;

	znode expr_node;
	zend_compile_expr(&expr_node, expr_ast);

	if (expr_node.op_type == IS_CONST) {
		result->op_type = IS_CONST;
		zend_ct_eval_unary_op(&result->u.constant, opcode,
			&expr_node.u.constant);
		zval_ptr_dtor(&expr_node.u.constant);
		return;
	}

	zend_emit_op_tmp(result, opcode, &expr_node, NULL);
}
/* }}} */

void zend_compile_unary_pm(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	znode expr_node;
	znode lefthand_node;

	ZEND_ASSERT(ast->kind == ZEND_AST_UNARY_PLUS || ast->kind == ZEND_AST_UNARY_MINUS);

	zend_compile_expr(&expr_node, expr_ast);

	if (expr_node.op_type == IS_CONST) {
		if (zend_try_ct_eval_unary_pm(&result->u.constant, ast->kind, &expr_node.u.constant)) {
			result->op_type = IS_CONST;
			zval_ptr_dtor(&expr_node.u.constant);
			return;
		}
	}

	lefthand_node.op_type = IS_CONST;
	ZVAL_LONG(&lefthand_node.u.constant, (ast->kind == ZEND_AST_UNARY_PLUS) ? 1 : -1);
	zend_emit_op_tmp(result, ZEND_MUL, &lefthand_node, &expr_node);
}
/* }}} */

void zend_compile_short_circuiting(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *left_ast = ast->child[0];
	zend_ast *right_ast = ast->child[1];

	znode left_node, right_node;
	zend_op *opline_jmpz, *opline_bool;
	uint32_t opnum_jmpz;

	ZEND_ASSERT(ast->kind == ZEND_AST_AND || ast->kind == ZEND_AST_OR);

	zend_compile_expr(&left_node, left_ast);

	if (left_node.op_type == IS_CONST) {
		if ((ast->kind == ZEND_AST_AND && !zend_is_true(&left_node.u.constant))
		 || (ast->kind == ZEND_AST_OR && zend_is_true(&left_node.u.constant))) {
			result->op_type = IS_CONST;
			ZVAL_BOOL(&result->u.constant, zend_is_true(&left_node.u.constant));
		} else {
			zend_compile_expr(&right_node, right_ast);

			if (right_node.op_type == IS_CONST) {
				result->op_type = IS_CONST;
				ZVAL_BOOL(&result->u.constant, zend_is_true(&right_node.u.constant));

				zval_ptr_dtor(&right_node.u.constant);
			} else {
				zend_emit_op_tmp(result, ZEND_BOOL, &right_node, NULL);
			}
		}

		zval_ptr_dtor(&left_node.u.constant);
		return;
	}

	opnum_jmpz = get_next_op_number(CG(active_op_array));
	opline_jmpz = zend_emit_op(NULL, ast->kind == ZEND_AST_AND ? ZEND_JMPZ_EX : ZEND_JMPNZ_EX,
		&left_node, NULL);

	if (left_node.op_type == IS_TMP_VAR) {
		SET_NODE(opline_jmpz->result, &left_node);
	} else {
		opline_jmpz->result.var = get_temporary_variable(CG(active_op_array));
		opline_jmpz->result_type = IS_TMP_VAR;
	}

	GET_NODE(result, opline_jmpz->result);
	zend_compile_expr(&right_node, right_ast);

	opline_bool = zend_emit_op(NULL, ZEND_BOOL, &right_node, NULL);
	SET_NODE(opline_bool->result, result);

	zend_update_jump_target_to_next(opnum_jmpz);
}
/* }}} */

void zend_compile_post_incdec(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *var_ast = ast->child[0];
	ZEND_ASSERT(ast->kind == ZEND_AST_POST_INC || ast->kind == ZEND_AST_POST_DEC);

	zend_ensure_writable_variable(var_ast);

	if (var_ast->kind == ZEND_AST_PROP) {
		zend_op *opline = zend_compile_prop(NULL, var_ast, BP_VAR_RW);
		opline->opcode = ast->kind == ZEND_AST_POST_INC ? ZEND_POST_INC_OBJ : ZEND_POST_DEC_OBJ;
		zend_make_tmp_result(result, opline);
	} else {
		znode var_node;
		zend_compile_var(&var_node, var_ast, BP_VAR_RW);
		zend_emit_op_tmp(result, ast->kind == ZEND_AST_POST_INC ? ZEND_POST_INC : ZEND_POST_DEC,
			&var_node, NULL);
	}
}
/* }}} */

void zend_compile_pre_incdec(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *var_ast = ast->child[0];
	ZEND_ASSERT(ast->kind == ZEND_AST_PRE_INC || ast->kind == ZEND_AST_PRE_DEC);

	zend_ensure_writable_variable(var_ast);

	if (var_ast->kind == ZEND_AST_PROP) {
		zend_op *opline = zend_compile_prop(result, var_ast, BP_VAR_RW);
		opline->opcode = ast->kind == ZEND_AST_PRE_INC ? ZEND_PRE_INC_OBJ : ZEND_PRE_DEC_OBJ;
	} else {
		znode var_node;
		zend_compile_var(&var_node, var_ast, BP_VAR_RW);
		zend_emit_op(result, ast->kind == ZEND_AST_PRE_INC ? ZEND_PRE_INC : ZEND_PRE_DEC,
			&var_node, NULL);
	}
}
/* }}} */

void zend_compile_cast(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	znode expr_node;
	zend_op *opline;

	zend_compile_expr(&expr_node, expr_ast);

	opline = zend_emit_op_tmp(result, ZEND_CAST, &expr_node, NULL);
	opline->extended_value = ast->attr;

	if (ast->attr == IS_NULL) {
		zend_error(E_DEPRECATED, "The (unset) cast is deprecated");
	}
}
/* }}} */

static void zend_compile_shorthand_conditional(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *cond_ast = ast->child[0];
	zend_ast *false_ast = ast->child[2];

	znode cond_node, false_node;
	zend_op *opline_qm_assign;
	uint32_t opnum_jmp_set;

	ZEND_ASSERT(ast->child[1] == NULL);

	zend_compile_expr(&cond_node, cond_ast);

	opnum_jmp_set = get_next_op_number(CG(active_op_array));
	zend_emit_op_tmp(result, ZEND_JMP_SET, &cond_node, NULL);

	zend_compile_expr(&false_node, false_ast);

	opline_qm_assign = zend_emit_op_tmp(NULL, ZEND_QM_ASSIGN, &false_node, NULL);
	SET_NODE(opline_qm_assign->result, result);

	zend_update_jump_target_to_next(opnum_jmp_set);
}
/* }}} */

void zend_compile_conditional(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *cond_ast = ast->child[0];
	zend_ast *true_ast = ast->child[1];
	zend_ast *false_ast = ast->child[2];

	znode cond_node, true_node, false_node;
	zend_op *opline_qm_assign2;
	uint32_t opnum_jmpz, opnum_jmp;

	if (!true_ast) {
		zend_compile_shorthand_conditional(result, ast);
		return;
	}

	zend_compile_expr(&cond_node, cond_ast);

	opnum_jmpz = zend_emit_cond_jump(ZEND_JMPZ, &cond_node, 0);

	zend_compile_expr(&true_node, true_ast);

	zend_emit_op_tmp(result, ZEND_QM_ASSIGN, &true_node, NULL);

	opnum_jmp = zend_emit_jump(0);

	zend_update_jump_target_to_next(opnum_jmpz);

	zend_compile_expr(&false_node, false_ast);

	opline_qm_assign2 = zend_emit_op(NULL, ZEND_QM_ASSIGN, &false_node, NULL);
	SET_NODE(opline_qm_assign2->result, result);

	zend_update_jump_target_to_next(opnum_jmp);
}
/* }}} */

void zend_compile_coalesce(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	zend_ast *default_ast = ast->child[1];

	znode expr_node, default_node;
	zend_op *opline;
	uint32_t opnum;

	zend_compile_var(&expr_node, expr_ast, BP_VAR_IS);

	opnum = get_next_op_number(CG(active_op_array));
	zend_emit_op_tmp(result, ZEND_COALESCE, &expr_node, NULL);

	zend_compile_expr(&default_node, default_ast);

	opline = zend_emit_op_tmp(NULL, ZEND_QM_ASSIGN, &default_node, NULL);
	SET_NODE(opline->result, result);

	opline = &CG(active_op_array)->opcodes[opnum];
	opline->op2.opline_num = get_next_op_number(CG(active_op_array));
}
/* }}} */

void zend_compile_print(znode *result, zend_ast *ast) /* {{{ */
{
	zend_op *opline;
	zend_ast *expr_ast = ast->child[0];

	znode expr_node;
	zend_compile_expr(&expr_node, expr_ast);

	opline = zend_emit_op(NULL, ZEND_ECHO, &expr_node, NULL);
	opline->extended_value = 1;

	result->op_type = IS_CONST;
	ZVAL_LONG(&result->u.constant, 1);
}
/* }}} */

void zend_compile_exit(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];

	if (expr_ast) {
		znode expr_node;
		zend_compile_expr(&expr_node, expr_ast);
		zend_emit_op(NULL, ZEND_EXIT, &expr_node, NULL);
	} else {
		zend_emit_op(NULL, ZEND_EXIT, NULL, NULL);
	}

	result->op_type = IS_CONST;
	ZVAL_BOOL(&result->u.constant, 1);
}
/* }}} */

void zend_compile_yield(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *value_ast = ast->child[0];
	zend_ast *key_ast = ast->child[1];

	znode value_node, key_node;
	znode *value_node_ptr = NULL, *key_node_ptr = NULL;
	zend_op *opline;
	zend_bool returns_by_ref = (CG(active_op_array)->fn_flags & ZEND_ACC_RETURN_REFERENCE) != 0;

	zend_mark_function_as_generator();

	if (key_ast) {
		zend_compile_expr(&key_node, key_ast);
		key_node_ptr = &key_node;
	}

	if (value_ast) {
		if (returns_by_ref && zend_is_variable(value_ast) && !zend_is_call(value_ast)) {
			zend_compile_var(&value_node, value_ast, BP_VAR_W);
		} else {
			zend_compile_expr(&value_node, value_ast);
		}
		value_node_ptr = &value_node;
	}

	opline = zend_emit_op(result, ZEND_YIELD, value_node_ptr, key_node_ptr);

	if (value_ast && returns_by_ref && zend_is_call(value_ast)) {
		opline->extended_value = ZEND_RETURNS_FUNCTION;
	}
}
/* }}} */

void zend_compile_yield_from(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	znode expr_node;

	zend_mark_function_as_generator();

	if (CG(active_op_array)->fn_flags & ZEND_ACC_RETURN_REFERENCE) {
		zend_error_noreturn(E_COMPILE_ERROR,
			"Cannot use \"yield from\" inside a by-reference generator");
	}

	zend_compile_expr(&expr_node, expr_ast);
	zend_emit_op_tmp(result, ZEND_YIELD_FROM, &expr_node, NULL);
}
/* }}} */

void zend_compile_instanceof(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *obj_ast = ast->child[0];
	zend_ast *class_ast = ast->child[1];

	znode obj_node, class_node;
	zend_op *opline;

	zend_compile_expr(&obj_node, obj_ast);
	if (obj_node.op_type == IS_CONST) {
		zend_do_free(&obj_node);
		result->op_type = IS_CONST;
		ZVAL_FALSE(&result->u.constant);
		return;
	}

	zend_compile_class_ref_ex(&class_node, class_ast,
		ZEND_FETCH_CLASS_NO_AUTOLOAD | ZEND_FETCH_CLASS_EXCEPTION);

	opline = zend_emit_op_tmp(result, ZEND_INSTANCEOF, &obj_node, NULL);

	if (class_node.op_type == IS_CONST) {
		opline->op2_type = IS_CONST;
		opline->op2.constant = zend_add_class_name_literal(
			CG(active_op_array), Z_STR(class_node.u.constant));
		opline->extended_value = zend_alloc_cache_slot();
	} else {
		SET_NODE(opline->op2, &class_node);
	}
}
/* }}} */

void zend_compile_include_or_eval(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	znode expr_node;
	zend_op *opline;

	zend_do_extended_fcall_begin();
	zend_compile_expr(&expr_node, expr_ast);

	opline = zend_emit_op(result, ZEND_INCLUDE_OR_EVAL, &expr_node, NULL);
	opline->extended_value = ast->attr;

	zend_do_extended_fcall_end();
}
/* }}} */

void zend_compile_isset_or_empty(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *var_ast = ast->child[0];

	znode var_node;
	zend_op *opline = NULL;

	ZEND_ASSERT(ast->kind == ZEND_AST_ISSET || ast->kind == ZEND_AST_EMPTY);

	if (!zend_is_variable(var_ast) || zend_is_call(var_ast)) {
		if (ast->kind == ZEND_AST_EMPTY) {
			/* empty(expr) can be transformed to !expr */
			zend_ast *not_ast = zend_ast_create_ex(ZEND_AST_UNARY_OP, ZEND_BOOL_NOT, var_ast);
			zend_compile_expr(result, not_ast);
			return;
		} else {
			zend_error_noreturn(E_COMPILE_ERROR,
				"Cannot use isset() on the result of an expression "
				"(you can use \"null !== expression\" instead)");
		}
	}

	switch (var_ast->kind) {
		case ZEND_AST_VAR:
			if (is_this_fetch(var_ast)) {
				opline = zend_emit_op(result, ZEND_ISSET_ISEMPTY_THIS, NULL, NULL);
			} else if (zend_try_compile_cv(&var_node, var_ast) == SUCCESS) {
				opline = zend_emit_op(result, ZEND_ISSET_ISEMPTY_CV, &var_node, NULL);
			} else {
				opline = zend_compile_simple_var_no_cv(result, var_ast, BP_VAR_IS, 0);
				opline->opcode = ZEND_ISSET_ISEMPTY_VAR;
			}
			break;
		case ZEND_AST_DIM:
			opline = zend_compile_dim(result, var_ast, BP_VAR_IS);
			opline->opcode = ZEND_ISSET_ISEMPTY_DIM_OBJ;
			break;
		case ZEND_AST_PROP:
			opline = zend_compile_prop(result, var_ast, BP_VAR_IS);
			opline->opcode = ZEND_ISSET_ISEMPTY_PROP_OBJ;
			break;
		case ZEND_AST_STATIC_PROP:
			opline = zend_compile_static_prop(result, var_ast, BP_VAR_IS, 0);
			opline->opcode = ZEND_ISSET_ISEMPTY_STATIC_PROP;
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	result->op_type = opline->result_type = IS_TMP_VAR;
	if (!(ast->kind == ZEND_AST_ISSET)) {
		opline->extended_value |= ZEND_ISEMPTY;
	}
}
/* }}} */

void zend_compile_silence(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];
	znode silence_node;
	uint32_t range;

	range = zend_start_live_range(CG(active_op_array), get_next_op_number(CG(active_op_array)));
	zend_emit_op_tmp(&silence_node, ZEND_BEGIN_SILENCE, NULL, NULL);

	if (expr_ast->kind == ZEND_AST_VAR) {
		/* For @$var we need to force a FETCH instruction, otherwise the CV access will
		 * happen outside the silenced section. */
		zend_compile_simple_var_no_cv(result, expr_ast, BP_VAR_R, 0 );
	} else {
		zend_compile_expr(result, expr_ast);
	}

	/* Store BEGIN_SILENCE/END_SILENCE pair to restore previous
	 * EG(error_reporting) value on exception */
	zend_end_live_range(CG(active_op_array), range, get_next_op_number(CG(active_op_array)),
		ZEND_LIVE_SILENCE, silence_node.u.op.var);

	zend_emit_op(NULL, ZEND_END_SILENCE, &silence_node, NULL);
}
/* }}} */

void zend_compile_shell_exec(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *expr_ast = ast->child[0];

	zval fn_name;
	zend_ast *name_ast, *args_ast, *call_ast;

	ZVAL_STRING(&fn_name, "shell_exec");
	name_ast = zend_ast_create_zval(&fn_name);
	args_ast = zend_ast_create_list(1, ZEND_AST_ARG_LIST, expr_ast);
	call_ast = zend_ast_create(ZEND_AST_CALL, name_ast, args_ast);

	zend_compile_expr(result, call_ast);

	zval_ptr_dtor(&fn_name);
}
/* }}} */

void zend_compile_array(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast_list *list = zend_ast_get_list(ast);
	zend_op *opline;
	uint32_t i, opnum_init = -1;
	zend_bool packed = 1;

	if (zend_try_ct_eval_array(&result->u.constant, ast)) {
		result->op_type = IS_CONST;
		return;
	}

	/* Empty arrays are handled at compile-time */
	ZEND_ASSERT(list->children > 0);

	for (i = 0; i < list->children; ++i) {
		zend_ast *elem_ast = list->child[i];
		zend_ast *value_ast, *key_ast;
		zend_bool by_ref;
		znode value_node, key_node, *key_node_ptr = NULL;

		if (elem_ast == NULL) {
			zend_error(E_COMPILE_ERROR, "Cannot use empty array elements in arrays");
		}

		value_ast = elem_ast->child[0];
		key_ast = elem_ast->child[1];
		by_ref = elem_ast->attr;

		if (key_ast) {
			zend_compile_expr(&key_node, key_ast);
			zend_handle_numeric_op(&key_node);
			key_node_ptr = &key_node;
		}

		if (by_ref) {
			zend_ensure_writable_variable(value_ast);
			zend_compile_var(&value_node, value_ast, BP_VAR_W);
		} else {
			zend_compile_expr(&value_node, value_ast);
		}

		if (i == 0) {
			opnum_init = get_next_op_number(CG(active_op_array));
			opline = zend_emit_op_tmp(result, ZEND_INIT_ARRAY, &value_node, key_node_ptr);
			opline->extended_value = list->children << ZEND_ARRAY_SIZE_SHIFT;
		} else {
			opline = zend_emit_op(NULL, ZEND_ADD_ARRAY_ELEMENT,
				&value_node, key_node_ptr);
			SET_NODE(opline->result, result);
		}
		opline->extended_value |= by_ref;

		if (key_ast && key_node.op_type == IS_CONST && Z_TYPE(key_node.u.constant) == IS_STRING) {
			packed = 0;
		}
	}

	/* Add a flag to INIT_ARRAY if we know this array cannot be packed */
	if (!packed) {
		ZEND_ASSERT(opnum_init != (uint32_t)-1);
		opline = &CG(active_op_array)->opcodes[opnum_init];
		opline->extended_value |= ZEND_ARRAY_NOT_PACKED;
	}
}
/* }}} */

void zend_compile_const(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *name_ast = ast->child[0];

	zend_op *opline;

	zend_bool is_fully_qualified;
	zend_string *orig_name = zend_ast_get_str(name_ast);
	zend_string *resolved_name = zend_resolve_const_name(orig_name, name_ast->attr, &is_fully_qualified);

	if (zend_string_equals_literal(resolved_name, "__COMPILER_HALT_OFFSET__") || (name_ast->attr != ZEND_NAME_RELATIVE && zend_string_equals_literal(orig_name, "__COMPILER_HALT_OFFSET__"))) {
		zend_ast *last = CG(ast);

		while (last->kind == ZEND_AST_STMT_LIST) {
			zend_ast_list *list = zend_ast_get_list(last);
			last = list->child[list->children-1];
		}
		if (last->kind == ZEND_AST_HALT_COMPILER) {
			result->op_type = IS_CONST;
			ZVAL_LONG(&result->u.constant, Z_LVAL_P(zend_ast_get_zval(last->child[0])));
			zend_string_release_ex(resolved_name, 0);
			return;
		}
	}

	if (zend_try_ct_eval_const(&result->u.constant, resolved_name, is_fully_qualified)) {
		result->op_type = IS_CONST;
		zend_string_release_ex(resolved_name, 0);
		return;
	}

	opline = zend_emit_op_tmp(result, ZEND_FETCH_CONSTANT, NULL, NULL);
	opline->op2_type = IS_CONST;

	if (is_fully_qualified) {
		opline->op2.constant = zend_add_const_name_literal(
			CG(active_op_array), resolved_name, 0);
	} else {
		opline->op1.num = IS_CONSTANT_UNQUALIFIED;
		if (FC(current_namespace)) {
			opline->op1.num |= IS_CONSTANT_IN_NAMESPACE;
			opline->op2.constant = zend_add_const_name_literal(
				CG(active_op_array), resolved_name, 1);
		} else {
			opline->op2.constant = zend_add_const_name_literal(
				CG(active_op_array), resolved_name, 0);
		}
	}
	opline->extended_value = zend_alloc_cache_slot();
}
/* }}} */

void zend_compile_class_const(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *class_ast = ast->child[0];
	zend_ast *const_ast = ast->child[1];

	znode class_node, const_node;
	zend_op *opline;

	if (zend_try_compile_const_expr_resolve_class_name(&result->u.constant, class_ast, const_ast, 0)) {
		if (Z_TYPE(result->u.constant) == IS_NULL) {
			zend_op *opline = zend_emit_op_tmp(result, ZEND_FETCH_CLASS_NAME, NULL, NULL);
			opline->op1.num = zend_get_class_fetch_type(zend_ast_get_str(class_ast));
		} else {
			result->op_type = IS_CONST;
		}
		return;
	}

	zend_eval_const_expr(&ast->child[0]);
	zend_eval_const_expr(&ast->child[1]);

	class_ast = ast->child[0];
	const_ast = ast->child[1];

	if (class_ast->kind == ZEND_AST_ZVAL) {
		zend_string *resolved_name;

		resolved_name = zend_resolve_class_name_ast(class_ast);
		if (const_ast->kind == ZEND_AST_ZVAL && zend_try_ct_eval_class_const(&result->u.constant, resolved_name, zend_ast_get_str(const_ast))) {
			result->op_type = IS_CONST;
			zend_string_release_ex(resolved_name, 0);
			return;
		}
		zend_string_release_ex(resolved_name, 0);
	}
	if (const_ast->kind == ZEND_AST_ZVAL && zend_string_equals_literal_ci(zend_ast_get_str(const_ast), "class")) {
		zend_error_noreturn(E_COMPILE_ERROR,
			"Dynamic class names are not allowed in compile-time ::class fetch");
	}

	zend_compile_class_ref_ex(&class_node, class_ast, ZEND_FETCH_CLASS_EXCEPTION);

	zend_compile_expr(&const_node, const_ast);

	opline = zend_emit_op_tmp(result, ZEND_FETCH_CLASS_CONSTANT, NULL, &const_node);

	zend_set_class_name_op1(opline, &class_node);

	opline->extended_value = zend_alloc_polymorphic_cache_slot();
}
/* }}} */

void zend_compile_resolve_class_name(znode *result, zend_ast *ast) /* {{{ */
{
	zend_ast *name_ast = ast->child[0];
	uint32_t fetch_type = zend_get_class_fetch_type(zend_ast_get_str(name_ast));
	zend_ensure_valid_class_fetch_type(fetch_type);

	switch (fetch_type) {
		case ZEND_FETCH_CLASS_SELF:
			if (CG(active_class_entry) && zend_is_scope_known()) {
				result->op_type = IS_CONST;
				ZVAL_STR_COPY(&result->u.constant, CG(active_class_entry)->name);
			} else {
				zend_op *opline = zend_emit_op_tmp(result, ZEND_FETCH_CLASS_NAME, NULL, NULL);
				opline->op1.num = fetch_type;
			}
			break;
		case ZEND_FETCH_CLASS_STATIC:
		case ZEND_FETCH_CLASS_PARENT:
			{
				zend_op *opline = zend_emit_op_tmp(result, ZEND_FETCH_CLASS_NAME, NULL, NULL);
				opline->op1.num = fetch_type;
			}
			break;
		case ZEND_FETCH_CLASS_DEFAULT:
			result->op_type = IS_CONST;
			ZVAL_STR(&result->u.constant, zend_resolve_class_name_ast(name_ast));
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

static zend_op *zend_compile_rope_add(znode *result, uint32_t num, znode *elem_node) /* {{{ */
{
	zend_op *opline = get_next_op(CG(active_op_array));

	if (num == 0) {
		result->op_type = IS_TMP_VAR;
		result->u.op.var = -1;
		opline->opcode = ZEND_ROPE_INIT;
	} else {
		opline->opcode = ZEND_ROPE_ADD;
		SET_NODE(opline->op1, result);
	}
	SET_NODE(opline->op2, elem_node);
	SET_NODE(opline->result, result);
	opline->extended_value = num;
	return opline;
}
/* }}} */

static void zend_compile_encaps_list(znode *result, zend_ast *ast) /* {{{ */
{
	uint32_t i, j;
	uint32_t rope_init_lineno = -1;
	zend_op *opline = NULL, *init_opline;
	znode elem_node, last_const_node;
	zend_ast_list *list = zend_ast_get_list(ast);

	ZEND_ASSERT(list->children > 0);

	j = 0;
	last_const_node.op_type = IS_UNUSED;
	for (i = 0; i < list->children; i++) {
		zend_compile_expr(&elem_node, list->child[i]);

		if (elem_node.op_type == IS_CONST) {
			convert_to_string(&elem_node.u.constant);

			if (Z_STRLEN(elem_node.u.constant) == 0) {
				zval_ptr_dtor(&elem_node.u.constant);
			} else if (last_const_node.op_type == IS_CONST) {
				concat_function(&last_const_node.u.constant, &last_const_node.u.constant, &elem_node.u.constant);
				zval_ptr_dtor(&elem_node.u.constant);
			} else {
				last_const_node.op_type = IS_CONST;
				ZVAL_COPY_VALUE(&last_const_node.u.constant, &elem_node.u.constant);
			}
			continue;
		} else {
			if (j == 0) {
				rope_init_lineno = get_next_op_number(CG(active_op_array));
			}
			if (last_const_node.op_type == IS_CONST) {
				zend_compile_rope_add(result, j++, &last_const_node);
				last_const_node.op_type = IS_UNUSED;
			}
			opline = zend_compile_rope_add(result, j++, &elem_node);
		}
	}

	if (j == 0) {
		result->op_type = IS_CONST;
		if (last_const_node.op_type == IS_CONST) {
			ZVAL_COPY_VALUE(&result->u.constant, &last_const_node.u.constant);
		} else {
			ZVAL_EMPTY_STRING(&result->u.constant);
			/* empty string */
		}
		return;
	} else if (last_const_node.op_type == IS_CONST) {
		opline = zend_compile_rope_add(result, j++, &last_const_node);
	}
	init_opline = CG(active_op_array)->opcodes + rope_init_lineno;
	if (j == 1) {
		if (opline->op2_type == IS_CONST) {
			GET_NODE(result, opline->op2);
			MAKE_NOP(opline);
		} else {
			opline->opcode = ZEND_CAST;
			opline->extended_value = IS_STRING;
			opline->op1_type = opline->op2_type;
			opline->op1 = opline->op2;
			opline->result_type = IS_TMP_VAR;
			opline->result.var = get_temporary_variable(CG(active_op_array));
			SET_UNUSED(opline->op2);
			GET_NODE(result, opline->result);
		}
	} else if (j == 2) {
		opline->opcode = ZEND_FAST_CONCAT;
		opline->extended_value = 0;
		opline->op1_type = init_opline->op2_type;
		opline->op1 = init_opline->op2;
		opline->result_type = IS_TMP_VAR;
		opline->result.var = get_temporary_variable(CG(active_op_array));
		MAKE_NOP(init_opline);
		GET_NODE(result, opline->result);
	} else {
		uint32_t var;
		uint32_t range = zend_start_live_range(CG(active_op_array), rope_init_lineno);

		init_opline->extended_value = j;
		opline->opcode = ZEND_ROPE_END;
		opline->result.var = get_temporary_variable(CG(active_op_array));
		var = opline->op1.var = get_temporary_variable(CG(active_op_array));
		GET_NODE(result, opline->result);

		/* Allocates the necessary number of zval slots to keep the rope */
		i = ((j * sizeof(zend_string*)) + (sizeof(zval) - 1)) / sizeof(zval);
		while (i > 1) {
			get_temporary_variable(CG(active_op_array));
			i--;
		}

		zend_end_live_range(CG(active_op_array), range, opline - CG(active_op_array)->opcodes,
			ZEND_LIVE_ROPE, var);

		/* Update all the previous opcodes to use the same variable */
		while (opline != init_opline) {
			opline--;
			if (opline->opcode == ZEND_ROPE_ADD &&
			    opline->result.var == (uint32_t)-1) {
				opline->op1.var = var;
				opline->result.var = var;
			} else if (opline->opcode == ZEND_ROPE_INIT &&
			           opline->result.var == (uint32_t)-1) {
				opline->result.var = var;
			}
		}
	}
}
/* }}} */

void zend_compile_magic_const(znode *result, zend_ast *ast) /* {{{ */
{
	zend_op *opline;

	if (zend_try_ct_eval_magic_const(&result->u.constant, ast)) {
		result->op_type = IS_CONST;
		return;
	}

	ZEND_ASSERT(ast->attr == T_CLASS_C &&
	            CG(active_class_entry) &&
	            (CG(active_class_entry)->ce_flags & ZEND_ACC_TRAIT) != 0);

	opline = zend_emit_op_tmp(result, ZEND_FETCH_CLASS_NAME, NULL, NULL);
	opline->op1.num = ZEND_FETCH_CLASS_SELF;
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
		|| kind == ZEND_AST_MAGIC_CONST || kind == ZEND_AST_COALESCE;
}
/* }}} */

void zend_compile_const_expr_class_const(zend_ast **ast_ptr) /* {{{ */
{
	zend_ast *ast = *ast_ptr;
	zend_ast *class_ast = ast->child[0];
	zend_ast *const_ast = ast->child[1];
	zend_string *class_name;
	zend_string *const_name = zend_ast_get_str(const_ast);
	zend_string *name;
	zval result;
	int fetch_type;

	if (class_ast->kind != ZEND_AST_ZVAL) {
		zend_error_noreturn(E_COMPILE_ERROR,
			"Dynamic class names are not allowed in compile-time class constant references");
	}

	if (zend_try_compile_const_expr_resolve_class_name(&result, class_ast, const_ast, 1)) {
		*ast_ptr = zend_ast_create_zval(&result);
		return;
	}

	class_name = zend_ast_get_str(class_ast);
	fetch_type = zend_get_class_fetch_type(class_name);

	if (ZEND_FETCH_CLASS_STATIC == fetch_type) {
		zend_error_noreturn(E_COMPILE_ERROR,
			"\"static::\" is not allowed in compile-time constants");
	}

	if (ZEND_FETCH_CLASS_DEFAULT == fetch_type) {
		class_name = zend_resolve_class_name_ast(class_ast);
	} else {
		zend_string_addref(class_name);
	}

	name = zend_concat3(
		ZSTR_VAL(class_name), ZSTR_LEN(class_name), "::", 2, ZSTR_VAL(const_name), ZSTR_LEN(const_name));

	zend_ast_destroy(ast);
	zend_string_release_ex(class_name, 0);

	*ast_ptr = zend_ast_create_constant(name, fetch_type | ZEND_FETCH_CLASS_EXCEPTION);
}
/* }}} */

void zend_compile_const_expr_const(zend_ast **ast_ptr) /* {{{ */
{
	zend_ast *ast = *ast_ptr;
	zend_ast *name_ast = ast->child[0];
	zend_string *orig_name = zend_ast_get_str(name_ast);
	zend_bool is_fully_qualified;
	zval result;
	zend_string *resolved_name;

	resolved_name = zend_resolve_const_name(
		orig_name, name_ast->attr, &is_fully_qualified);

	if (zend_try_ct_eval_const(&result, resolved_name, is_fully_qualified)) {
		zend_string_release_ex(resolved_name, 0);
		zend_ast_destroy(ast);
		*ast_ptr = zend_ast_create_zval(&result);
		return;
	}

	zend_ast_destroy(ast);
	*ast_ptr = zend_ast_create_constant(resolved_name, !is_fully_qualified ? IS_CONSTANT_UNQUALIFIED : 0);
}
/* }}} */

void zend_compile_const_expr_magic_const(zend_ast **ast_ptr) /* {{{ */
{
	zend_ast *ast = *ast_ptr;

	/* Other cases already resolved by constant folding */
	ZEND_ASSERT(ast->attr == T_CLASS_C &&
	            CG(active_class_entry) &&
	            (CG(active_class_entry)->ce_flags & ZEND_ACC_TRAIT) != 0);

	zend_ast_destroy(ast);
	*ast_ptr = zend_ast_create(ZEND_AST_CONSTANT_CLASS);
}
/* }}} */

void zend_compile_const_expr(zend_ast **ast_ptr) /* {{{ */
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
			zend_compile_const_expr_class_const(ast_ptr);
			break;
		case ZEND_AST_CONST:
			zend_compile_const_expr_const(ast_ptr);
			break;
		case ZEND_AST_MAGIC_CONST:
			zend_compile_const_expr_magic_const(ast_ptr);
			break;
		default:
			zend_ast_apply(ast, zend_compile_const_expr);
			break;
	}
}
/* }}} */

void zend_const_expr_to_zval(zval *result, zend_ast *ast) /* {{{ */
{
	zend_ast *orig_ast = ast;
	zend_eval_const_expr(&ast);
	zend_compile_const_expr(&ast);
	if (ast->kind == ZEND_AST_ZVAL) {
		ZVAL_COPY_VALUE(result, zend_ast_get_zval(ast));
	} else {
		ZVAL_AST(result, zend_ast_copy(ast));
		/* destroy the ast here, it might have been replaced */
		zend_ast_destroy(ast);
	}

	/* Kill this branch of the original AST, as it was already destroyed.
	 * It would be nice to find a better solution to this problem in the
	 * future. */
	orig_ast->kind = 0;
}
/* }}} */

/* Same as compile_stmt, but with early binding */
void zend_compile_top_stmt(zend_ast *ast) /* {{{ */
{
	if (!ast) {
		return;
	}

	if (ast->kind == ZEND_AST_STMT_LIST) {
		zend_ast_list *list = zend_ast_get_list(ast);
		uint32_t i;
		for (i = 0; i < list->children; ++i) {
			zend_compile_top_stmt(list->child[i]);
		}
		return;
	}

	zend_compile_stmt(ast);

	if (ast->kind != ZEND_AST_NAMESPACE && ast->kind != ZEND_AST_HALT_COMPILER) {
		zend_verify_namespace();
	}
	if (ast->kind == ZEND_AST_FUNC_DECL || ast->kind == ZEND_AST_CLASS) {
		CG(zend_lineno) = ((zend_ast_decl *) ast)->end_lineno;
		zend_do_early_binding();
	}
}
/* }}} */

void zend_compile_stmt(zend_ast *ast) /* {{{ */
{
	if (!ast) {
		return;
	}

	CG(zend_lineno) = ast->lineno;

	if ((CG(compiler_options) & ZEND_COMPILE_EXTENDED_INFO) && !zend_is_unticked_stmt(ast)) {
		zend_do_extended_info();
	}

	switch (ast->kind) {
		case ZEND_AST_STMT_LIST:
			zend_compile_stmt_list(ast);
			break;
		case ZEND_AST_GLOBAL:
			zend_compile_global_var(ast);
			break;
		case ZEND_AST_STATIC:
			zend_compile_static_var(ast);
			break;
		case ZEND_AST_UNSET:
			zend_compile_unset(ast);
			break;
		case ZEND_AST_RETURN:
			zend_compile_return(ast);
			break;
		case ZEND_AST_ECHO:
			zend_compile_echo(ast);
			break;
		case ZEND_AST_THROW:
			zend_compile_throw(ast);
			break;
		case ZEND_AST_BREAK:
		case ZEND_AST_CONTINUE:
			zend_compile_break_continue(ast);
			break;
		case ZEND_AST_GOTO:
			zend_compile_goto(ast);
			break;
		case ZEND_AST_LABEL:
			zend_compile_label(ast);
			break;
		case ZEND_AST_WHILE:
			zend_compile_while(ast);
			break;
		case ZEND_AST_DO_WHILE:
			zend_compile_do_while(ast);
			break;
		case ZEND_AST_FOR:
			zend_compile_for(ast);
			break;
		case ZEND_AST_FOREACH:
			zend_compile_foreach(ast);
			break;
		case ZEND_AST_IF:
			zend_compile_if(ast);
			break;
		case ZEND_AST_SWITCH:
			zend_compile_switch(ast);
			break;
		case ZEND_AST_TRY:
			zend_compile_try(ast);
			break;
		case ZEND_AST_DECLARE:
			zend_compile_declare(ast);
			break;
		case ZEND_AST_FUNC_DECL:
		case ZEND_AST_METHOD:
			zend_compile_func_decl(NULL, ast);
			break;
		case ZEND_AST_PROP_DECL:
			zend_compile_prop_decl(ast);
			break;
		case ZEND_AST_CLASS_CONST_DECL:
			zend_compile_class_const_decl(ast);
			break;
		case ZEND_AST_USE_TRAIT:
			zend_compile_use_trait(ast);
			break;
		case ZEND_AST_CLASS:
			zend_compile_class_decl(ast);
			break;
		case ZEND_AST_GROUP_USE:
			zend_compile_group_use(ast);
			break;
		case ZEND_AST_USE:
			zend_compile_use(ast);
			break;
		case ZEND_AST_CONST_DECL:
			zend_compile_const_decl(ast);
			break;
		case ZEND_AST_NAMESPACE:
			zend_compile_namespace(ast);
			break;
		case ZEND_AST_HALT_COMPILER:
			zend_compile_halt_compiler(ast);
			break;
		default:
		{
			znode result;
			zend_compile_expr(&result, ast);
			zend_do_free(&result);
		}
	}

	if (FC(declarables).ticks && !zend_is_unticked_stmt(ast)) {
		zend_emit_tick();
	}
}
/* }}} */

void zend_compile_expr(znode *result, zend_ast *ast) /* {{{ */
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
			zend_compile_var(result, ast, BP_VAR_R);
			return;
		case ZEND_AST_ASSIGN:
			zend_compile_assign(result, ast);
			return;
		case ZEND_AST_ASSIGN_REF:
			zend_compile_assign_ref(result, ast);
			return;
		case ZEND_AST_NEW:
			zend_compile_new(result, ast);
			return;
		case ZEND_AST_CLONE:
			zend_compile_clone(result, ast);
			return;
		case ZEND_AST_ASSIGN_OP:
			zend_compile_compound_assign(result, ast);
			return;
		case ZEND_AST_BINARY_OP:
			zend_compile_binary_op(result, ast);
			return;
		case ZEND_AST_GREATER:
		case ZEND_AST_GREATER_EQUAL:
			zend_compile_greater(result, ast);
			return;
		case ZEND_AST_UNARY_OP:
			zend_compile_unary_op(result, ast);
			return;
		case ZEND_AST_UNARY_PLUS:
		case ZEND_AST_UNARY_MINUS:
			zend_compile_unary_pm(result, ast);
			return;
		case ZEND_AST_AND:
		case ZEND_AST_OR:
			zend_compile_short_circuiting(result, ast);
			return;
		case ZEND_AST_POST_INC:
		case ZEND_AST_POST_DEC:
			zend_compile_post_incdec(result, ast);
			return;
		case ZEND_AST_PRE_INC:
		case ZEND_AST_PRE_DEC:
			zend_compile_pre_incdec(result, ast);
			return;
		case ZEND_AST_CAST:
			zend_compile_cast(result, ast);
			return;
		case ZEND_AST_CONDITIONAL:
			zend_compile_conditional(result, ast);
			return;
		case ZEND_AST_COALESCE:
			zend_compile_coalesce(result, ast);
			return;
		case ZEND_AST_PRINT:
			zend_compile_print(result, ast);
			return;
		case ZEND_AST_EXIT:
			zend_compile_exit(result, ast);
			return;
		case ZEND_AST_YIELD:
			zend_compile_yield(result, ast);
			return;
		case ZEND_AST_YIELD_FROM:
			zend_compile_yield_from(result, ast);
			return;
		case ZEND_AST_INSTANCEOF:
			zend_compile_instanceof(result, ast);
			return;
		case ZEND_AST_INCLUDE_OR_EVAL:
			zend_compile_include_or_eval(result, ast);
			return;
		case ZEND_AST_ISSET:
		case ZEND_AST_EMPTY:
			zend_compile_isset_or_empty(result, ast);
			return;
		case ZEND_AST_SILENCE:
			zend_compile_silence(result, ast);
			return;
		case ZEND_AST_SHELL_EXEC:
			zend_compile_shell_exec(result, ast);
			return;
		case ZEND_AST_ARRAY:
			zend_compile_array(result, ast);
			return;
		case ZEND_AST_CONST:
			zend_compile_const(result, ast);
			return;
		case ZEND_AST_CLASS_CONST:
			zend_compile_class_const(result, ast);
			return;
		case ZEND_AST_ENCAPS_LIST:
			zend_compile_encaps_list(result, ast);
			return;
		case ZEND_AST_MAGIC_CONST:
			zend_compile_magic_const(result, ast);
			return;
		case ZEND_AST_CLOSURE:
			zend_compile_func_decl(result, ast);
			return;
		default:
			ZEND_ASSERT(0 /* not supported */);
	}
}
/* }}} */

void zend_compile_var(znode *result, zend_ast *ast, uint32_t type) /* {{{ */
{
	CG(zend_lineno) = zend_ast_get_lineno(ast);

	switch (ast->kind) {
		case ZEND_AST_VAR:
			zend_compile_simple_var(result, ast, type, 0);
			return;
		case ZEND_AST_DIM:
			zend_compile_dim(result, ast, type);
			return;
		case ZEND_AST_PROP:
			zend_compile_prop(result, ast, type);
			return;
		case ZEND_AST_STATIC_PROP:
			zend_compile_static_prop(result, ast, type, 0);
			return;
		case ZEND_AST_CALL:
			zend_compile_call(result, ast, type);
			return;
		case ZEND_AST_METHOD_CALL:
			zend_compile_method_call(result, ast, type);
			return;
		case ZEND_AST_STATIC_CALL:
			zend_compile_static_call(result, ast, type);
			return;
		case ZEND_AST_ZNODE:
			*result = *zend_ast_get_znode(ast);
			return;
		default:
			if (type == BP_VAR_W || type == BP_VAR_RW || type == BP_VAR_UNSET) {
				zend_error_noreturn(E_COMPILE_ERROR,
					"Cannot use temporary expression in write context");
			}

			zend_compile_expr(result, ast);
			return;
	}
}
/* }}} */

void zend_delayed_compile_var(znode *result, zend_ast *ast, uint32_t type) /* {{{ */
{
	switch (ast->kind) {
		case ZEND_AST_VAR:
			zend_compile_simple_var(result, ast, type, 1);
			return;
		case ZEND_AST_DIM:
			zend_delayed_compile_dim(result, ast, type);
			return;
		case ZEND_AST_PROP:
			zend_delayed_compile_prop(result, ast, type);
			return;
		case ZEND_AST_STATIC_PROP:
			zend_compile_static_prop(result, ast, type, 1);
			return;
		default:
			zend_compile_var(result, ast, type);
			return;
	}
}
/* }}} */

void zend_eval_const_expr(zend_ast **ast_ptr) /* {{{ */
{
	zend_ast *ast = *ast_ptr;
	zval result;

	if (!ast) {
		return;
	}

	switch (ast->kind) {
		case ZEND_AST_BINARY_OP:
			zend_eval_const_expr(&ast->child[0]);
			zend_eval_const_expr(&ast->child[1]);
			if (ast->child[0]->kind != ZEND_AST_ZVAL || ast->child[1]->kind != ZEND_AST_ZVAL) {
				return;
			}

			if (!zend_try_ct_eval_binary_op(&result, ast->attr,
					zend_ast_get_zval(ast->child[0]), zend_ast_get_zval(ast->child[1]))
			) {
				return;
			}
			break;
		case ZEND_AST_GREATER:
		case ZEND_AST_GREATER_EQUAL:
			zend_eval_const_expr(&ast->child[0]);
			zend_eval_const_expr(&ast->child[1]);
			if (ast->child[0]->kind != ZEND_AST_ZVAL || ast->child[1]->kind != ZEND_AST_ZVAL) {
				return;
			}

			zend_ct_eval_greater(&result, ast->kind,
				zend_ast_get_zval(ast->child[0]), zend_ast_get_zval(ast->child[1]));
			break;
		case ZEND_AST_AND:
		case ZEND_AST_OR:
		{
			int i;
			for (i = 0; i <= 1; i++) {
				zend_eval_const_expr(&ast->child[i]);
				if (ast->child[i]->kind == ZEND_AST_ZVAL) {
					if (zend_is_true(zend_ast_get_zval(ast->child[i])) == (ast->kind == ZEND_AST_OR)) {
						ZVAL_BOOL(&result, ast->kind == ZEND_AST_OR);
						return;
					}
				}
			}

			if (ast->child[0]->kind != ZEND_AST_ZVAL || ast->child[1]->kind != ZEND_AST_ZVAL) {
				return;
			}

			if (ast->kind == ZEND_AST_OR) {
				ZVAL_BOOL(&result, zend_is_true(zend_ast_get_zval(ast->child[0])) || zend_is_true(zend_ast_get_zval(ast->child[1])));
			} else {
				ZVAL_BOOL(&result, zend_is_true(zend_ast_get_zval(ast->child[0])) && zend_is_true(zend_ast_get_zval(ast->child[1])));
			}
			break;
		}
		case ZEND_AST_UNARY_OP:
			zend_eval_const_expr(&ast->child[0]);
			if (ast->child[0]->kind != ZEND_AST_ZVAL) {
				return;
			}

			zend_ct_eval_unary_op(&result, ast->attr, zend_ast_get_zval(ast->child[0]));
			break;
		case ZEND_AST_UNARY_PLUS:
		case ZEND_AST_UNARY_MINUS:
			zend_eval_const_expr(&ast->child[0]);
			if (ast->child[0]->kind != ZEND_AST_ZVAL) {
				return;
			}

			if (!zend_try_ct_eval_unary_pm(&result, ast->kind, zend_ast_get_zval(ast->child[0]))) {
				return;
			}
			break;
		case ZEND_AST_COALESCE:
			/* Set isset fetch indicator here, opcache disallows runtime altering of the AST */
			if (ast->child[0]->kind == ZEND_AST_DIM) {
				ast->child[0]->attr = ZEND_DIM_IS;
			}
			zend_eval_const_expr(&ast->child[0]);

			if (ast->child[0]->kind != ZEND_AST_ZVAL) {
				/* ensure everything was compile-time evaluated at least once */
				zend_eval_const_expr(&ast->child[1]);
				return;
			}

			if (Z_TYPE_P(zend_ast_get_zval(ast->child[0])) == IS_NULL) {
				zend_eval_const_expr(&ast->child[1]);
				*ast_ptr = ast->child[1];
				ast->child[1] = NULL;
				zend_ast_destroy(ast);
			} else {
				*ast_ptr = ast->child[0];
				ast->child[0] = NULL;
				zend_ast_destroy(ast);
			}
			return;
		case ZEND_AST_CONDITIONAL:
		{
			zend_ast **child, *child_ast;
			zend_eval_const_expr(&ast->child[0]);
			if (ast->child[0]->kind != ZEND_AST_ZVAL) {
				/* ensure everything was compile-time evaluated at least once */
				if (ast->child[1]) {
					zend_eval_const_expr(&ast->child[1]);
				}
				zend_eval_const_expr(&ast->child[2]);
				return;
			}

			child = &ast->child[2 - zend_is_true(zend_ast_get_zval(ast->child[0]))];
			if (*child == NULL) {
				child--;
			}
			child_ast = *child;
			*child = NULL;
			zend_ast_destroy(ast);
			*ast_ptr = child_ast;
			zend_eval_const_expr(ast_ptr);
			return;
		}
		case ZEND_AST_DIM:
		{
			/* constant expression should be always read context ... */
			zval *container, *dim;

			if (ast->child[1] == NULL) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot use [] for reading");
			}

			/* Set isset fetch indicator here, opcache disallows runtime altering of the AST */
			if (ast->attr == ZEND_DIM_IS && ast->child[0]->kind == ZEND_AST_DIM) {
				ast->child[0]->attr = ZEND_DIM_IS;
			}

			zend_eval_const_expr(&ast->child[0]);
			zend_eval_const_expr(&ast->child[1]);
			if (ast->child[0]->kind != ZEND_AST_ZVAL || ast->child[1]->kind != ZEND_AST_ZVAL) {
				return;
			}

			container = zend_ast_get_zval(ast->child[0]);
			dim = zend_ast_get_zval(ast->child[1]);

			if (Z_TYPE_P(container) == IS_ARRAY) {
				zval *el;
				if (Z_TYPE_P(dim) == IS_LONG) {
					el = zend_hash_index_find(Z_ARR_P(container), Z_LVAL_P(dim));
					if (el) {
						ZVAL_COPY(&result, el);
					} else {
						return;
					}
				} else if (Z_TYPE_P(dim) == IS_STRING) {
					el = zend_symtable_find(Z_ARR_P(container), Z_STR_P(dim));
					if (el) {
						ZVAL_COPY(&result, el);
					} else {
						return;
					}
				} else {
					return; /* warning... handle at runtime */
				}
			} else if (Z_TYPE_P(container) == IS_STRING) {
				zend_long offset;
				zend_uchar c;
				if (Z_TYPE_P(dim) == IS_LONG) {
					offset = Z_LVAL_P(dim);
				} else if (Z_TYPE_P(dim) != IS_STRING || is_numeric_string(Z_STRVAL_P(dim), Z_STRLEN_P(dim), &offset, NULL, 1) != IS_LONG) {
					return;
				}
				if (offset < 0 || (size_t)offset >= Z_STRLEN_P(container)) {
					return;
				}
				c = (zend_uchar) Z_STRVAL_P(container)[offset];
				ZVAL_INTERNED_STR(&result, ZSTR_CHAR(c));
			} else if (Z_TYPE_P(container) <= IS_FALSE) {
				ZVAL_NULL(&result);
			} else {
				return;
			}
			break;
		}
		case ZEND_AST_ARRAY:
			if (!zend_try_ct_eval_array(&result, ast)) {
				return;
			}
			break;
		case ZEND_AST_MAGIC_CONST:
			if (!zend_try_ct_eval_magic_const(&result, ast)) {
				return;
			}
			break;
		case ZEND_AST_CONST:
		{
			zend_ast *name_ast = ast->child[0];
			zend_bool is_fully_qualified;
			zend_string *resolved_name = zend_resolve_const_name(
				zend_ast_get_str(name_ast), name_ast->attr, &is_fully_qualified);

			if (!zend_try_ct_eval_const(&result, resolved_name, is_fully_qualified)) {
				zend_string_release_ex(resolved_name, 0);
				return;
			}

			zend_string_release_ex(resolved_name, 0);
			break;
		}
		case ZEND_AST_CLASS_CONST:
		{
			zend_ast *class_ast = ast->child[0];
			zend_ast *name_ast = ast->child[1];
			zend_string *resolved_name;

			if (zend_try_compile_const_expr_resolve_class_name(&result, class_ast, name_ast, 0)) {
				if (Z_TYPE(result) == IS_NULL) {
					if (zend_get_class_fetch_type(zend_ast_get_str(class_ast)) == ZEND_FETCH_CLASS_SELF) {
						zend_ast_destroy(ast);
						*ast_ptr = zend_ast_create_ex(ZEND_AST_MAGIC_CONST, T_CLASS_C);
					}
					return;
				}
				break;
			}

			zend_eval_const_expr(&ast->child[0]);
			zend_eval_const_expr(&ast->child[1]);

			class_ast = ast->child[0];
			name_ast = ast->child[1];

			if (name_ast->kind == ZEND_AST_ZVAL && zend_string_equals_literal_ci(zend_ast_get_str(name_ast), "class")) {
				zend_error_noreturn(E_COMPILE_ERROR,
					"Dynamic class names are not allowed in compile-time ::class fetch");
			}

			if (class_ast->kind != ZEND_AST_ZVAL || name_ast->kind != ZEND_AST_ZVAL) {
				return;
			}

			resolved_name = zend_resolve_class_name_ast(class_ast);

			if (!zend_try_ct_eval_class_const(&result, resolved_name, zend_ast_get_str(name_ast))) {
				zend_string_release_ex(resolved_name, 0);
				return;
			}

			zend_string_release_ex(resolved_name, 0);
			break;
		}

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
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
