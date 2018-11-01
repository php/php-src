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
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include <stdio.h>

#include "zend.h"
#include "zend_alloc.h"
#include "zend_compile.h"
#include "zend_extensions.h"
#include "zend_API.h"
#include "zend_sort.h"

#include "zend_vm.h"

static void zend_extension_op_array_ctor_handler(zend_extension *extension, zend_op_array *op_array)
{
	if (extension->op_array_ctor) {
		extension->op_array_ctor(op_array);
	}
}

static void zend_extension_op_array_dtor_handler(zend_extension *extension, zend_op_array *op_array)
{
	if (extension->op_array_dtor) {
		extension->op_array_dtor(op_array);
	}
}

void init_op_array(zend_op_array *op_array, zend_uchar type, int initial_ops_size)
{
	op_array->type = type;
	op_array->arg_flags[0] = 0;
	op_array->arg_flags[1] = 0;
	op_array->arg_flags[2] = 0;

	op_array->refcount = (uint32_t *) emalloc(sizeof(uint32_t));
	*op_array->refcount = 1;
	op_array->last = 0;
	op_array->opcodes = emalloc(initial_ops_size * sizeof(zend_op));

	op_array->last_var = 0;
	op_array->vars = NULL;

	op_array->T = 0;

	op_array->function_name = NULL;
	op_array->filename = zend_get_compiled_filename();
	op_array->doc_comment = NULL;

	op_array->arg_info = NULL;
	op_array->num_args = 0;
	op_array->required_num_args = 0;

	op_array->scope = NULL;
	op_array->prototype = NULL;

	op_array->live_range = NULL;
	op_array->try_catch_array = NULL;
	op_array->last_live_range = 0;

	op_array->static_variables = NULL;
	op_array->last_try_catch = 0;

	op_array->fn_flags = 0;

	op_array->last_literal = 0;
	op_array->literals = NULL;

	op_array->run_time_cache = NULL;
	op_array->cache_size = 0;

	memset(op_array->reserved, 0, ZEND_MAX_RESERVED_RESOURCES * sizeof(void*));

	if (zend_extension_flags & ZEND_EXTENSIONS_HAVE_OP_ARRAY_CTOR) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_op_array_ctor_handler, op_array);
	}
}

ZEND_API void destroy_zend_function(zend_function *function)
{
	zval tmp;

	ZVAL_PTR(&tmp, function);
	zend_function_dtor(&tmp);
}

ZEND_API void zend_function_dtor(zval *zv)
{
	zend_function *function = Z_PTR_P(zv);

	if (function->type == ZEND_USER_FUNCTION) {
		ZEND_ASSERT(function->common.function_name);
		destroy_op_array(&function->op_array);
		/* op_arrays are allocated on arena, so we don't have to free them */
	} else {
		ZEND_ASSERT(function->type == ZEND_INTERNAL_FUNCTION);
		ZEND_ASSERT(function->common.function_name);
		zend_string_release_ex(function->common.function_name, 1);

		if ((function->common.fn_flags & (ZEND_ACC_HAS_RETURN_TYPE|ZEND_ACC_HAS_TYPE_HINTS)) &&
		    !function->common.scope && function->common.arg_info) {

			uint32_t i;
			uint32_t num_args = function->common.num_args + 1;
			zend_arg_info *arg_info = function->common.arg_info - 1;

			if (function->common.fn_flags & ZEND_ACC_VARIADIC) {
				num_args++;
			}
			for (i = 0 ; i < num_args; i++) {
				if (ZEND_TYPE_IS_CLASS(arg_info[i].type)) {
					zend_string_release_ex(ZEND_TYPE_NAME(arg_info[i].type), 1);
				}
			}
			free(arg_info);
		}

		if (!(function->common.fn_flags & ZEND_ACC_ARENA_ALLOCATED)) {
			pefree(function, 1);
		}
	}
}

ZEND_API void zend_cleanup_internal_class_data(zend_class_entry *ce)
{
	if (CE_STATIC_MEMBERS(ce)) {
		zval *static_members = CE_STATIC_MEMBERS(ce);
		zval *p = static_members;
		zval *end = p + ce->default_static_members_count;

#ifdef ZTS
		CG(static_members_table)[(zend_intptr_t)(ce->static_members_table)] = NULL;
#else
		ce->static_members_table = NULL;
#endif
		while (p != end) {
			i_zval_ptr_dtor(p ZEND_FILE_LINE_CC);
			p++;
		}
		efree(static_members);
	}
}

void _destroy_zend_class_traits_info(zend_class_entry *ce)
{
	if (ce->num_traits > 0 && ce->traits) {
		efree(ce->traits);
	}

	if (ce->trait_aliases) {
		size_t i = 0;
		while (ce->trait_aliases[i]) {
			if (ce->trait_aliases[i]->trait_method.method_name) {
				zend_string_release_ex(ce->trait_aliases[i]->trait_method.method_name, 0);
			}
			if (ce->trait_aliases[i]->trait_method.class_name) {
				zend_string_release_ex(ce->trait_aliases[i]->trait_method.class_name, 0);
			}

			if (ce->trait_aliases[i]->alias) {
				zend_string_release_ex(ce->trait_aliases[i]->alias, 0);
			}

			efree(ce->trait_aliases[i]);
			i++;
		}

		efree(ce->trait_aliases);
	}

	if (ce->trait_precedences) {
		int i = 0;
		int j;

		while (ce->trait_precedences[i]) {
			zend_string_release_ex(ce->trait_precedences[i]->trait_method.method_name, 0);
			zend_string_release_ex(ce->trait_precedences[i]->trait_method.class_name, 0);

			for (j = 0; j < ce->trait_precedences[i]->num_excludes; j++) {
				zend_string_release_ex(ce->trait_precedences[i]->exclude_class_names[j], 0);
			}
			efree(ce->trait_precedences[i]);
			i++;
		}
		efree(ce->trait_precedences);
	}
}

ZEND_API void destroy_zend_class(zval *zv)
{
	zend_property_info *prop_info;
	zend_class_entry *ce = Z_PTR_P(zv);
	zend_function *fn;

	if (--ce->refcount > 0) {
		return;
	}
	switch (ce->type) {
		case ZEND_USER_CLASS:
			if (ce->default_properties_table) {
				zval *p = ce->default_properties_table;
				zval *end = p + ce->default_properties_count;

				while (p != end) {
					i_zval_ptr_dtor(p ZEND_FILE_LINE_CC);
					p++;
				}
				efree(ce->default_properties_table);
			}
			if (ce->default_static_members_table) {
				zval *p = ce->default_static_members_table;
				zval *end = p + ce->default_static_members_count;

				while (p != end) {
					i_zval_ptr_dtor(p ZEND_FILE_LINE_CC);
					p++;
				}
				efree(ce->default_static_members_table);
			}
			ZEND_HASH_FOREACH_PTR(&ce->properties_info, prop_info) {
				if (prop_info->ce == ce || (prop_info->flags & ZEND_ACC_SHADOW)) {
					zend_string_release_ex(prop_info->name, 0);
					if (prop_info->doc_comment) {
						zend_string_release_ex(prop_info->doc_comment, 0);
					}
				}
			} ZEND_HASH_FOREACH_END();
			zend_hash_destroy(&ce->properties_info);
			zend_string_release_ex(ce->name, 0);
			zend_hash_destroy(&ce->function_table);
			if (zend_hash_num_elements(&ce->constants_table)) {
				zend_class_constant *c;

				ZEND_HASH_FOREACH_PTR(&ce->constants_table, c) {
					if (c->ce == ce) {
						zval_ptr_dtor_nogc(&c->value);
						if (c->doc_comment) {
							zend_string_release_ex(c->doc_comment, 0);
						}
					}
				} ZEND_HASH_FOREACH_END();
			}
			zend_hash_destroy(&ce->constants_table);
			if (ce->num_interfaces > 0 && ce->interfaces) {
				efree(ce->interfaces);
			}
			if (ce->info.user.doc_comment) {
				zend_string_release_ex(ce->info.user.doc_comment, 0);
			}

			_destroy_zend_class_traits_info(ce);

			break;
		case ZEND_INTERNAL_CLASS:
			if (ce->default_properties_table) {
				zval *p = ce->default_properties_table;
				zval *end = p + ce->default_properties_count;

				while (p != end) {
					zval_internal_ptr_dtor(p);
					p++;
				}
				free(ce->default_properties_table);
			}
			if (ce->default_static_members_table) {
				zval *p = ce->default_static_members_table;
				zval *end = p + ce->default_static_members_count;

				while (p != end) {
					zval_internal_ptr_dtor(p);
					p++;
				}
				free(ce->default_static_members_table);
			}
			zend_hash_destroy(&ce->properties_info);
			zend_string_release_ex(ce->name, 1);

			/* TODO: eliminate this loop for classes without functions with arg_info */
			ZEND_HASH_FOREACH_PTR(&ce->function_table, fn) {
				if ((fn->common.fn_flags & (ZEND_ACC_HAS_RETURN_TYPE|ZEND_ACC_HAS_TYPE_HINTS)) &&
				    fn->common.scope == ce) {
					/* reset function scope to allow arg_info removing */
					fn->common.scope = NULL;
				}
			} ZEND_HASH_FOREACH_END();

			zend_hash_destroy(&ce->function_table);
			if (zend_hash_num_elements(&ce->constants_table)) {
				zend_class_constant *c;

				ZEND_HASH_FOREACH_PTR(&ce->constants_table, c) {
					if (c->ce == ce) {
						zval_internal_ptr_dtor(&c->value);
						if (c->doc_comment) {
							zend_string_release_ex(c->doc_comment, 1);
						}
					}
					free(c);
				} ZEND_HASH_FOREACH_END();
				zend_hash_destroy(&ce->constants_table);
			}
			if (ce->iterator_funcs_ptr) {
				free(ce->iterator_funcs_ptr);
			}
			if (ce->num_interfaces > 0) {
				free(ce->interfaces);
			}
			free(ce);
			break;
	}
}

void zend_class_add_ref(zval *zv)
{
	zend_class_entry *ce = Z_PTR_P(zv);

	ce->refcount++;
}

ZEND_API void destroy_op_array(zend_op_array *op_array)
{
	uint32_t i;

	if (op_array->static_variables &&
	    !(GC_FLAGS(op_array->static_variables) & IS_ARRAY_IMMUTABLE)) {
		if (GC_DELREF(op_array->static_variables) == 0) {
			zend_array_destroy(op_array->static_variables);
		}
	}

	if (op_array->run_time_cache && !op_array->function_name) {
		efree(op_array->run_time_cache);
		op_array->run_time_cache = NULL;
	}

	if (!op_array->refcount || --(*op_array->refcount) > 0) {
		return;
	}

	efree_size(op_array->refcount, sizeof(*(op_array->refcount)));

	if (op_array->vars) {
		i = op_array->last_var;
		while (i > 0) {
			i--;
			zend_string_release_ex(op_array->vars[i], 0);
		}
		efree(op_array->vars);
	}

	if (op_array->literals) {
		zval *literal = op_array->literals;
		zval *end = literal + op_array->last_literal;
	 	while (literal < end) {
			zval_ptr_dtor_nogc(literal);
			literal++;
		}
		if (ZEND_USE_ABS_CONST_ADDR
		 || !(op_array->fn_flags & ZEND_ACC_DONE_PASS_TWO)) {
			efree(op_array->literals);
		}
	}
	efree(op_array->opcodes);

	if (op_array->function_name) {
		zend_string_release_ex(op_array->function_name, 0);
	}
	if (op_array->doc_comment) {
		zend_string_release_ex(op_array->doc_comment, 0);
	}
	if (op_array->live_range) {
		efree(op_array->live_range);
	}
	if (op_array->try_catch_array) {
		efree(op_array->try_catch_array);
	}
	if (zend_extension_flags & ZEND_EXTENSIONS_HAVE_OP_ARRAY_DTOR) {
		if (op_array->fn_flags & ZEND_ACC_DONE_PASS_TWO) {
			zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_op_array_dtor_handler, op_array);
		}
	}
	if (op_array->arg_info) {
		uint32_t num_args = op_array->num_args;
		zend_arg_info *arg_info = op_array->arg_info;

		if (op_array->fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
			arg_info--;
			num_args++;
		}
		if (op_array->fn_flags & ZEND_ACC_VARIADIC) {
			num_args++;
		}
		for (i = 0 ; i < num_args; i++) {
			if (arg_info[i].name) {
				zend_string_release_ex(arg_info[i].name, 0);
			}
			if (ZEND_TYPE_IS_CLASS(arg_info[i].type)) {
				zend_string_release_ex(ZEND_TYPE_NAME(arg_info[i].type), 0);
			}
		}
		efree(arg_info);
	}
}

static void zend_update_extended_info(zend_op_array *op_array)
{
	zend_op *opline = op_array->opcodes, *end=opline+op_array->last;

	while (opline<end) {
		if (opline->opcode == ZEND_EXT_STMT) {
			if (opline+1<end) {
				if ((opline+1)->opcode == ZEND_EXT_STMT) {
					opline->opcode = ZEND_NOP;
					opline++;
					continue;
				}
				if (opline+1<end) {
					opline->lineno = (opline+1)->lineno;
				}
			} else {
				opline->opcode = ZEND_NOP;
			}
		}
		opline++;
	}
}

static void zend_extension_op_array_handler(zend_extension *extension, zend_op_array *op_array)
{
	if (extension->op_array_handler) {
		extension->op_array_handler(op_array);
	}
}

static void zend_check_finally_breakout(zend_op_array *op_array, uint32_t op_num, uint32_t dst_num)
{
	int i;

	for (i = 0; i < op_array->last_try_catch; i++) {
		if ((op_num < op_array->try_catch_array[i].finally_op ||
					op_num >= op_array->try_catch_array[i].finally_end)
				&& (dst_num >= op_array->try_catch_array[i].finally_op &&
					 dst_num <= op_array->try_catch_array[i].finally_end)) {
			CG(in_compilation) = 1;
			CG(active_op_array) = op_array;
			CG(zend_lineno) = op_array->opcodes[op_num].lineno;
			zend_error_noreturn(E_COMPILE_ERROR, "jump into a finally block is disallowed");
		} else if ((op_num >= op_array->try_catch_array[i].finally_op
					&& op_num <= op_array->try_catch_array[i].finally_end)
				&& (dst_num > op_array->try_catch_array[i].finally_end
					|| dst_num < op_array->try_catch_array[i].finally_op)) {
			CG(in_compilation) = 1;
			CG(active_op_array) = op_array;
			CG(zend_lineno) = op_array->opcodes[op_num].lineno;
			zend_error_noreturn(E_COMPILE_ERROR, "jump out of a finally block is disallowed");
		}
	}
}

static uint32_t zend_get_brk_cont_target(const zend_op_array *op_array, const zend_op *opline) {
	int nest_levels = opline->op2.num;
	int array_offset = opline->op1.num;
	zend_brk_cont_element *jmp_to;
	do {
		jmp_to = &CG(context).brk_cont_array[array_offset];
		if (nest_levels > 1) {
			array_offset = jmp_to->parent;
		}
	} while (--nest_levels > 0);

	return opline->opcode == ZEND_BRK ? jmp_to->brk : jmp_to->cont;
}

/* Live ranges must be sorted by increasing start opline */
static int cmp_live_range(const zend_live_range *a, const zend_live_range *b) {
	return a->start - b->start;
}
static void swap_live_range(zend_live_range *a, zend_live_range *b) {
	zend_live_range tmp = *a;
	*a = *b;
	*b = tmp;
}
static void zend_sort_live_ranges(zend_op_array *op_array) {
	zend_sort(op_array->live_range, op_array->last_live_range, sizeof(zend_live_range),
			(compare_func_t) cmp_live_range, (swap_func_t) swap_live_range);
}

ZEND_API int pass_two(zend_op_array *op_array)
{
	zend_op *opline, *end;

	if (!ZEND_USER_CODE(op_array->type)) {
		return 0;
	}
	if (CG(compiler_options) & ZEND_COMPILE_EXTENDED_INFO) {
		zend_update_extended_info(op_array);
	}
	if (CG(compiler_options) & ZEND_COMPILE_HANDLE_OP_ARRAY) {
		if (zend_extension_flags & ZEND_EXTENSIONS_HAVE_OP_ARRAY_HANDLER) {
			zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_op_array_handler, op_array);
		}
	}

	if (CG(context).vars_size != op_array->last_var) {
		op_array->vars = (zend_string**) erealloc(op_array->vars, sizeof(zend_string*)*op_array->last_var);
		CG(context).vars_size = op_array->last_var;
	}

#if ZEND_USE_ABS_CONST_ADDR
	if (CG(context).opcodes_size != op_array->last) {
		op_array->opcodes = (zend_op *) erealloc(op_array->opcodes, sizeof(zend_op)*op_array->last);
		CG(context).opcodes_size = op_array->last;
	}
	if (CG(context).literals_size != op_array->last_literal) {
		op_array->literals = (zval*)erealloc(op_array->literals, sizeof(zval) * op_array->last_literal);
		CG(context).literals_size = op_array->last_literal;
	}
#else
	op_array->opcodes = (zend_op *) erealloc(op_array->opcodes,
		ZEND_MM_ALIGNED_SIZE_EX(sizeof(zend_op) * op_array->last, 16) +
		sizeof(zval) * op_array->last_literal);
	if (op_array->literals) {
		memcpy(((char*)op_array->opcodes) + ZEND_MM_ALIGNED_SIZE_EX(sizeof(zend_op) * op_array->last, 16),
			op_array->literals, sizeof(zval) * op_array->last_literal);
		efree(op_array->literals);
		op_array->literals = (zval*)(((char*)op_array->opcodes) + ZEND_MM_ALIGNED_SIZE_EX(sizeof(zend_op) * op_array->last, 16));
	}
	CG(context).opcodes_size = op_array->last;
	CG(context).literals_size = op_array->last_literal;
#endif

	/* Needs to be set directly after the opcode/literal reallocation, to ensure destruction
	 * happens correctly if any of the following fixups generate a fatal error. */
	op_array->fn_flags |= ZEND_ACC_DONE_PASS_TWO;

	opline = op_array->opcodes;
	end = opline + op_array->last;
	while (opline < end) {
		switch (opline->opcode) {
			case ZEND_RECV_INIT:
				{
					zval *val = CT_CONSTANT(opline->op2);
					if (Z_TYPE_P(val) == IS_CONSTANT_AST) {
						uint32_t slot = ZEND_MM_ALIGNED_SIZE_EX(op_array->cache_size, 8);
						Z_CACHE_SLOT_P(val) = slot;
						op_array->cache_size += sizeof(zval);
					}
				}
				break;
			case ZEND_FAST_CALL:
				opline->op1.opline_num = op_array->try_catch_array[opline->op1.num].finally_op;
				ZEND_PASS_TWO_UPDATE_JMP_TARGET(op_array, opline, opline->op1);
				break;
			case ZEND_BRK:
			case ZEND_CONT:
				{
					uint32_t jmp_target = zend_get_brk_cont_target(op_array, opline);

					if (op_array->fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK) {
						zend_check_finally_breakout(op_array, opline - op_array->opcodes, jmp_target);
					}
					opline->opcode = ZEND_JMP;
					opline->op1.opline_num = jmp_target;
					opline->op2.num = 0;
					ZEND_PASS_TWO_UPDATE_JMP_TARGET(op_array, opline, opline->op1);
				}
				break;
			case ZEND_GOTO:
				zend_resolve_goto_label(op_array, opline);
				if (op_array->fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK) {
					zend_check_finally_breakout(op_array, opline - op_array->opcodes, opline->op1.opline_num);
				}
				/* break omitted intentionally */
			case ZEND_JMP:
				ZEND_PASS_TWO_UPDATE_JMP_TARGET(op_array, opline, opline->op1);
				break;
			case ZEND_JMPZNZ:
				/* absolute index to relative offset */
				opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, opline, opline->extended_value);
				/* break omitted intentionally */
			case ZEND_JMPZ:
			case ZEND_JMPNZ:
			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
			case ZEND_JMP_SET:
			case ZEND_COALESCE:
			case ZEND_FE_RESET_R:
			case ZEND_FE_RESET_RW:
				ZEND_PASS_TWO_UPDATE_JMP_TARGET(op_array, opline, opline->op2);
				break;
			case ZEND_ASSERT_CHECK:
			{
				/* If result of assert is unused, result of check is unused as well */
				zend_op *call = &op_array->opcodes[opline->op2.opline_num - 1];
				if (call->opcode == ZEND_EXT_FCALL_END) {
					call--;
				}
				if (call->result_type == IS_UNUSED) {
					opline->result_type = IS_UNUSED;
				}
				ZEND_PASS_TWO_UPDATE_JMP_TARGET(op_array, opline, opline->op2);
				break;
			}
			case ZEND_DECLARE_ANON_CLASS:
			case ZEND_DECLARE_ANON_INHERITED_CLASS:
			case ZEND_FE_FETCH_R:
			case ZEND_FE_FETCH_RW:
				/* absolute index to relative offset */
				opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, opline, opline->extended_value);
				break;
			case ZEND_CATCH:
				if (!(opline->extended_value & ZEND_LAST_CATCH)) {
					ZEND_PASS_TWO_UPDATE_JMP_TARGET(op_array, opline, opline->op2);
				}
				break;
			case ZEND_RETURN:
			case ZEND_RETURN_BY_REF:
				if (op_array->fn_flags & ZEND_ACC_GENERATOR) {
					opline->opcode = ZEND_GENERATOR_RETURN;
				}
				break;
			case ZEND_SWITCH_LONG:
			case ZEND_SWITCH_STRING:
			{
				/* absolute indexes to relative offsets */
				HashTable *jumptable = Z_ARRVAL_P(CT_CONSTANT(opline->op2));
				zval *zv;
				ZEND_HASH_FOREACH_VAL(jumptable, zv) {
					Z_LVAL_P(zv) = ZEND_OPLINE_NUM_TO_OFFSET(op_array, opline, Z_LVAL_P(zv));
				} ZEND_HASH_FOREACH_END();

				opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, opline, opline->extended_value);
				break;
			}
		}
		if (opline->op1_type == IS_CONST) {
			ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, opline, opline->op1);
		} else if (opline->op1_type & (IS_VAR|IS_TMP_VAR)) {
			opline->op1.var = (uint32_t)(zend_intptr_t)ZEND_CALL_VAR_NUM(NULL, op_array->last_var + opline->op1.var);
		}
		if (opline->op2_type == IS_CONST) {
			ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, opline, opline->op2);
		} else if (opline->op2_type & (IS_VAR|IS_TMP_VAR)) {
			opline->op2.var = (uint32_t)(zend_intptr_t)ZEND_CALL_VAR_NUM(NULL, op_array->last_var + opline->op2.var);
		}
		if (opline->result_type & (IS_VAR|IS_TMP_VAR)) {
			opline->result.var = (uint32_t)(zend_intptr_t)ZEND_CALL_VAR_NUM(NULL, op_array->last_var + opline->result.var);
		}
		ZEND_VM_SET_OPCODE_HANDLER(opline);
		opline++;
	}

	if (op_array->live_range) {
		int i;

		zend_sort_live_ranges(op_array);
		for (i = 0; i < op_array->last_live_range; i++) {
			op_array->live_range[i].var =
				(uint32_t)(zend_intptr_t)ZEND_CALL_VAR_NUM(NULL, op_array->last_var + (op_array->live_range[i].var / sizeof(zval))) |
				(op_array->live_range[i].var & ZEND_LIVE_MASK);
		}
	}

	return 0;
}

ZEND_API unary_op_type get_unary_op(int opcode)
{
	switch (opcode) {
		case ZEND_BW_NOT:
			return (unary_op_type) bitwise_not_function;
		case ZEND_BOOL_NOT:
			return (unary_op_type) boolean_not_function;
		default:
			return (unary_op_type) NULL;
	}
}

ZEND_API binary_op_type get_binary_op(int opcode)
{
	switch (opcode) {
		case ZEND_ADD:
		case ZEND_ASSIGN_ADD:
			return (binary_op_type) add_function;
		case ZEND_SUB:
		case ZEND_ASSIGN_SUB:
			return (binary_op_type) sub_function;
		case ZEND_MUL:
		case ZEND_ASSIGN_MUL:
			return (binary_op_type) mul_function;
		case ZEND_POW:
		case ZEND_ASSIGN_POW:
			return (binary_op_type) pow_function;
		case ZEND_DIV:
		case ZEND_ASSIGN_DIV:
			return (binary_op_type) div_function;
		case ZEND_MOD:
		case ZEND_ASSIGN_MOD:
			return (binary_op_type) mod_function;
		case ZEND_SL:
		case ZEND_ASSIGN_SL:
			return (binary_op_type) shift_left_function;
		case ZEND_SR:
		case ZEND_ASSIGN_SR:
			return (binary_op_type) shift_right_function;
		case ZEND_FAST_CONCAT:
		case ZEND_CONCAT:
		case ZEND_ASSIGN_CONCAT:
			return (binary_op_type) concat_function;
		case ZEND_IS_IDENTICAL:
			return (binary_op_type) is_identical_function;
		case ZEND_IS_NOT_IDENTICAL:
			return (binary_op_type) is_not_identical_function;
		case ZEND_IS_EQUAL:
		case ZEND_CASE:
			return (binary_op_type) is_equal_function;
		case ZEND_IS_NOT_EQUAL:
			return (binary_op_type) is_not_equal_function;
		case ZEND_IS_SMALLER:
			return (binary_op_type) is_smaller_function;
		case ZEND_IS_SMALLER_OR_EQUAL:
			return (binary_op_type) is_smaller_or_equal_function;
		case ZEND_SPACESHIP:
			return (binary_op_type) compare_function;
		case ZEND_BW_OR:
		case ZEND_ASSIGN_BW_OR:
			return (binary_op_type) bitwise_or_function;
		case ZEND_BW_AND:
		case ZEND_ASSIGN_BW_AND:
			return (binary_op_type) bitwise_and_function;
		case ZEND_BW_XOR:
		case ZEND_ASSIGN_BW_XOR:
			return (binary_op_type) bitwise_xor_function;
		case ZEND_BOOL_XOR:
			return (binary_op_type) boolean_xor_function;
		default:
			return (binary_op_type) NULL;
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
