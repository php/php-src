/*
   +----------------------------------------------------------------------+
   | Zend Engine, Call Graph                                              |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id:$ */

#include "php.h"
#include "zend_compile.h"
#include "zend_extensions.h"
#include "Optimizer/zend_optimizer.h"
#include "zend_optimizer_internal.h"
#include "zend_inference.h"
#include "zend_call_graph.h"
#include "zend_func_info.h"
#include "zend_inference.h"
#include "zend_call_graph.h"

typedef int (*zend_op_array_func_t)(zend_call_graph *call_graph, zend_op_array *op_array);

static int zend_op_array_calc(zend_call_graph *call_graph, zend_op_array *op_array)
{
	(void) op_array;

	call_graph->op_arrays_count++;
	return SUCCESS;
}

static int zend_op_array_collect(zend_call_graph *call_graph, zend_op_array *op_array)
{
    zend_func_info *func_info = call_graph->func_infos + call_graph->op_arrays_count;

	ZEND_SET_FUNC_INFO(op_array, func_info);
	call_graph->op_arrays[call_graph->op_arrays_count] = op_array;
	func_info->num = call_graph->op_arrays_count;
	func_info->num_args = -1;
	func_info->return_value_used = -1;
	call_graph->op_arrays_count++;
	return SUCCESS;
}

static int zend_foreach_op_array(zend_call_graph *call_graph, zend_script *script, zend_op_array_func_t func)
{
	zend_class_entry *ce;
	zend_op_array *op_array;

	if (func(call_graph, &script->main_op_array) != SUCCESS) {
		return FAILURE;
	}

	ZEND_HASH_FOREACH_PTR(&script->function_table, op_array) {
		if (func(call_graph, op_array) != SUCCESS) {
			return FAILURE;
		}
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_FOREACH_PTR(&script->class_table, ce) {
		ZEND_HASH_FOREACH_PTR(&ce->function_table, op_array) {
			if (op_array->scope == ce) {
				if (func(call_graph, op_array) != SUCCESS) {
					return FAILURE;
				}
			}
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FOREACH_END();

	return SUCCESS;
}

static void zend_collect_args_info(zend_call_info *call_info)
{
	zend_op *opline = call_info->caller_init_opline;
	zend_op *end = call_info->caller_call_opline;
	uint32_t i;
	int num;
	int level = 0;

	ZEND_ASSERT(opline && end);
	if (!opline->extended_value) {
		return;
	}
	for (i = 0; i < opline->extended_value; i++) {
		call_info->arg_info[i].opline = NULL;
	}
	while (opline < end) {
		opline++;
		switch (opline->opcode) {
			case ZEND_SEND_VAL:
			case ZEND_SEND_VAR:
			case ZEND_SEND_VAL_EX:
			case ZEND_SEND_VAR_EX:
			case ZEND_SEND_REF:
			case ZEND_SEND_VAR_NO_REF:
			case ZEND_SEND_VAR_NO_REF_EX:
				num = opline->op2.num;
				if (num > 0) {
					num--;
				}
				if (!level) {
					call_info->arg_info[num].opline = opline;
				}
				break;
			case ZEND_SEND_ARRAY:
			case ZEND_SEND_USER:
			case ZEND_SEND_UNPACK:
				// ???
				break;
			case ZEND_INIT_FCALL:
			case ZEND_INIT_FCALL_BY_NAME:
			case ZEND_INIT_NS_FCALL_BY_NAME:
			case ZEND_INIT_DYNAMIC_CALL:
			case ZEND_NEW:
			case ZEND_INIT_METHOD_CALL:
			case ZEND_INIT_STATIC_METHOD_CALL:
			case ZEND_INIT_USER_CALL:
				level++;
				break;
			case ZEND_DO_FCALL:
			case ZEND_DO_ICALL:
			case ZEND_DO_UCALL:
			case ZEND_DO_FCALL_BY_NAME:
				level--;
				break;
		}
	}
}

static int zend_analyze_calls(zend_arena **arena, zend_script *script, uint32_t build_flags, zend_op_array *op_array, zend_func_info *func_info)
{
	zend_op *opline = op_array->opcodes;
	zend_op *end = opline + op_array->last;
	zend_function *func;
	zend_call_info *call_info;
	int call = 0;
	zend_call_info **call_stack;
	ALLOCA_FLAG(use_heap);

	call_stack = do_alloca((op_array->last / 2) * sizeof(zend_call_info*), use_heap);
	while (opline != end) {
		call_info = NULL;
		switch (opline->opcode) {
			case ZEND_INIT_FCALL:
			case ZEND_INIT_METHOD_CALL:
			case ZEND_INIT_STATIC_METHOD_CALL:
				func = zend_optimizer_get_called_func(
					script, op_array, opline, (build_flags & ZEND_RT_CONSTANTS) != 0);
				if (func) {
					call_info = zend_arena_calloc(arena, 1, sizeof(zend_call_info) + (sizeof(zend_send_arg_info) * ((int)opline->extended_value - 1)));
					call_info->caller_op_array = op_array;
					call_info->caller_init_opline = opline;
					call_info->caller_call_opline = NULL;
					call_info->callee_func = func;
					call_info->num_args = opline->extended_value;
					call_info->next_callee = func_info->callee_info;
					func_info->callee_info = call_info;

					if (func->type == ZEND_INTERNAL_FUNCTION) {
						call_info->next_caller = NULL;
					} else {
						zend_func_info *callee_func_info = ZEND_FUNC_INFO(&func->op_array);
						call_info->next_caller = callee_func_info ? callee_func_info->caller_info : NULL;
					}
				}
				/* break missing intentionally */
			case ZEND_INIT_FCALL_BY_NAME:
			case ZEND_INIT_NS_FCALL_BY_NAME:
			case ZEND_INIT_DYNAMIC_CALL:
			case ZEND_NEW:
			case ZEND_INIT_USER_CALL:
				call_stack[call] = call_info;
				call++;
				break;
			case ZEND_DO_FCALL:
			case ZEND_DO_ICALL:
			case ZEND_DO_UCALL:
			case ZEND_DO_FCALL_BY_NAME:
				func_info->flags |= ZEND_FUNC_HAS_CALLS;
				call--;
				if (call_stack[call]) {
					call_stack[call]->caller_call_opline = opline;
					zend_collect_args_info(call_stack[call]);
				}
				break;
		}
		opline++;
	}
	free_alloca(call_stack, use_heap);
	return SUCCESS;
}

static int zend_is_indirectly_recursive(zend_op_array *root, zend_op_array *op_array, zend_bitset visited)
{
	zend_func_info *func_info;
	zend_call_info *call_info;
	int ret = 0;

	if (op_array == root) {
		return 1;
	}

	func_info = ZEND_FUNC_INFO(op_array);
	if (zend_bitset_in(visited, func_info->num)) {
		return 0;
	}
	zend_bitset_incl(visited, func_info->num);
	call_info = func_info->caller_info;
	while (call_info) {
		if (zend_is_indirectly_recursive(root, call_info->caller_op_array, visited)) {
			call_info->recursive = 1;
			ret = 1;
		}
		call_info = call_info->next_caller;
	}
	return ret;
}

static void zend_analyze_recursion(zend_call_graph *call_graph)
{
	zend_op_array *op_array;
	zend_func_info *func_info;
	zend_call_info *call_info;
	int i;
	int set_len = zend_bitset_len(call_graph->op_arrays_count);
	zend_bitset visited;
	ALLOCA_FLAG(use_heap);

	visited = ZEND_BITSET_ALLOCA(set_len, use_heap);
	for (i = 0; i < call_graph->op_arrays_count; i++) {
		op_array = call_graph->op_arrays[i];
		func_info = call_graph->func_infos + i;
		call_info = func_info->caller_info;
		while (call_info) {
			if (call_info->caller_op_array == op_array) {
				call_info->recursive = 1;
				func_info->flags |= ZEND_FUNC_RECURSIVE | ZEND_FUNC_RECURSIVE_DIRECTLY;
			} else {
				memset(visited, 0, sizeof(zend_ulong) * set_len);
				if (zend_is_indirectly_recursive(op_array, call_info->caller_op_array, visited)) {
					call_info->recursive = 1;
					func_info->flags |= ZEND_FUNC_RECURSIVE | ZEND_FUNC_RECURSIVE_INDIRECTLY;
				}
			}
			call_info = call_info->next_caller;
		}
	}

	free_alloca(visited, use_heap);
}

static void zend_sort_op_arrays(zend_call_graph *call_graph)
{
	(void) call_graph;

	// TODO: perform topological sort of cyclic call graph
}

int zend_build_call_graph(zend_arena **arena, zend_script *script, uint32_t build_flags, zend_call_graph *call_graph) /* {{{ */
{
	int i;

	call_graph->op_arrays_count = 0;
	if (zend_foreach_op_array(call_graph, script, zend_op_array_calc) != SUCCESS) {
		return FAILURE;
	}
	call_graph->op_arrays = (zend_op_array**)zend_arena_calloc(arena, call_graph->op_arrays_count, sizeof(zend_op_array*));
	call_graph->func_infos = (zend_func_info*)zend_arena_calloc(arena, call_graph->op_arrays_count, sizeof(zend_func_info));
	call_graph->op_arrays_count = 0;
	if (zend_foreach_op_array(call_graph, script, zend_op_array_collect) != SUCCESS) {
		return FAILURE;
	}
	for (i = 0; i < call_graph->op_arrays_count; i++) {
		zend_analyze_calls(arena, script, build_flags, call_graph->op_arrays[i], call_graph->func_infos + i);
	}
	zend_analyze_recursion(call_graph);
	zend_sort_op_arrays(call_graph);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
