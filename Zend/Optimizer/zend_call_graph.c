/*
   +----------------------------------------------------------------------+
   | Zend Engine, Call Graph                                              |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend_compile.h"
#include "zend_extensions.h"
#include "Optimizer/zend_optimizer.h"
#include "zend_optimizer_internal.h"
#include "zend_inference.h"
#include "zend_call_graph.h"
#include "zend_func_info.h"
#include "zend_inference.h"
#include "zend_call_graph.h"

static void zend_op_array_calc(zend_op_array *op_array, void *context)
{
	zend_call_graph *call_graph = context;
	call_graph->op_arrays_count++;
}

static void zend_op_array_collect(zend_op_array *op_array, void *context)
{
	zend_call_graph *call_graph = context;
	zend_func_info *func_info = call_graph->func_infos + call_graph->op_arrays_count;

	ZEND_SET_FUNC_INFO(op_array, func_info);
	call_graph->op_arrays[call_graph->op_arrays_count] = op_array;
	func_info->num = call_graph->op_arrays_count;
	call_graph->op_arrays_count++;
}

ZEND_API void zend_analyze_calls(zend_arena **arena, zend_script *script, uint32_t build_flags, zend_op_array *op_array, zend_func_info *func_info)
{
	zend_op *opline = op_array->opcodes;
	zend_op *end = opline + op_array->last;
	zend_function *func;
	zend_call_info *call_info;
	int call = 0;
	zend_call_info **call_stack;
	ALLOCA_FLAG(use_heap);
	bool is_prototype;

	call_stack = do_alloca((op_array->last / 2) * sizeof(zend_call_info*), use_heap);
	call_info = NULL;
	while (opline != end) {
		switch (opline->opcode) {
			case ZEND_INIT_FCALL:
			case ZEND_INIT_METHOD_CALL:
			case ZEND_INIT_STATIC_METHOD_CALL:
				call_stack[call] = call_info;
				func = zend_optimizer_get_called_func(
					script, op_array, opline, &is_prototype);
				if (func) {
					call_info = zend_arena_calloc(arena, 1, sizeof(zend_call_info) + (sizeof(zend_send_arg_info) * ((int)opline->extended_value - 1)));
					call_info->caller_op_array = op_array;
					call_info->caller_init_opline = opline;
					call_info->caller_call_opline = NULL;
					call_info->callee_func = func;
					call_info->num_args = opline->extended_value;
					call_info->next_callee = func_info->callee_info;
					call_info->is_prototype = is_prototype;
					call_info->is_frameless = false;
					func_info->callee_info = call_info;

					if (build_flags & ZEND_CALL_TREE) {
						call_info->next_caller = NULL;
					} else if (func->type == ZEND_INTERNAL_FUNCTION) {
						call_info->next_caller = NULL;
					} else {
						zend_func_info *callee_func_info = ZEND_FUNC_INFO(&func->op_array);
						if (callee_func_info) {
							call_info->next_caller = callee_func_info->caller_info;
							callee_func_info->caller_info = call_info;
						} else {
							call_info->next_caller = NULL;
						}
					}
				} else {
					call_info = NULL;
				}
				call++;
				break;
			case ZEND_INIT_FCALL_BY_NAME:
			case ZEND_INIT_NS_FCALL_BY_NAME:
			case ZEND_INIT_DYNAMIC_CALL:
			case ZEND_NEW:
			case ZEND_INIT_USER_CALL:
				call_stack[call] = call_info;
				call_info = NULL;
				call++;
				break;
			case ZEND_FRAMELESS_ICALL_0:
			case ZEND_FRAMELESS_ICALL_1:
			case ZEND_FRAMELESS_ICALL_2:
			case ZEND_FRAMELESS_ICALL_3: {
				func = ZEND_FLF_FUNC(opline);
				zend_call_info *call_info = zend_arena_calloc(arena, 1, sizeof(zend_call_info));
				call_info->caller_op_array = op_array;
				call_info->caller_init_opline = opline;
				call_info->caller_call_opline = NULL;
				call_info->callee_func = func;
				call_info->num_args = ZEND_FLF_NUM_ARGS(opline->opcode);
				call_info->next_callee = func_info->callee_info;
				call_info->is_prototype = false;
				call_info->is_frameless = true;
				call_info->next_caller = NULL;
				func_info->callee_info = call_info;
				break;
			}
			case ZEND_DO_FCALL:
			case ZEND_DO_ICALL:
			case ZEND_DO_UCALL:
			case ZEND_DO_FCALL_BY_NAME:
			case ZEND_CALLABLE_CONVERT:
				func_info->flags |= ZEND_FUNC_HAS_CALLS;
				if (call_info) {
					call_info->caller_call_opline = opline;
				}
				call--;
				call_info = call_stack[call];
				break;
			case ZEND_SEND_VAL:
			case ZEND_SEND_VAR:
			case ZEND_SEND_VAL_EX:
			case ZEND_SEND_VAR_EX:
			case ZEND_SEND_FUNC_ARG:
			case ZEND_SEND_REF:
			case ZEND_SEND_VAR_NO_REF:
			case ZEND_SEND_VAR_NO_REF_EX:
			case ZEND_SEND_USER:
				if (call_info) {
					if (opline->op2_type == IS_CONST) {
						call_info->named_args = 1;
						break;
					}

					uint32_t num = opline->op2.num;
					if (num > 0) {
						num--;
					}
					call_info->arg_info[num].opline = opline;
				}
				break;
			case ZEND_SEND_ARRAY:
			case ZEND_SEND_UNPACK:
				if (call_info) {
					call_info->send_unpack = 1;
				}
				break;
			case ZEND_EXIT:
				/* In this case the DO_CALL opcode may have been dropped
				 * and caller_call_opline will be NULL. */
				break;
		}
		opline++;
	}
	free_alloca(call_stack, use_heap);
}

static bool zend_is_indirectly_recursive(zend_op_array *root, zend_op_array *op_array, zend_bitset visited)
{
	zend_func_info *func_info;
	zend_call_info *call_info;
	bool ret = 0;

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
		for (; call_info; call_info = call_info->next_caller) {
			if (call_info->is_prototype) {
				/* Might be calling an overridden child method and not actually recursive. */
				continue;
			}
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
		}
	}

	free_alloca(visited, use_heap);
}

static void zend_sort_op_arrays(zend_call_graph *call_graph)
{
	(void) call_graph;

	// TODO: perform topological sort of cyclic call graph
}

ZEND_API void zend_build_call_graph(zend_arena **arena, zend_script *script, zend_call_graph *call_graph) /* {{{ */
{
	call_graph->op_arrays_count = 0;
	zend_foreach_op_array(script, zend_op_array_calc, call_graph);

	call_graph->op_arrays = (zend_op_array**)zend_arena_calloc(arena, call_graph->op_arrays_count, sizeof(zend_op_array*));
	call_graph->func_infos = (zend_func_info*)zend_arena_calloc(arena, call_graph->op_arrays_count, sizeof(zend_func_info));
	call_graph->op_arrays_count = 0;
	zend_foreach_op_array(script, zend_op_array_collect, call_graph);
}
/* }}} */

ZEND_API void zend_analyze_call_graph(zend_arena **arena, zend_script *script, zend_call_graph *call_graph) /* {{{ */
{
	int i;

	for (i = 0; i < call_graph->op_arrays_count; i++) {
		zend_analyze_calls(arena, script, 0, call_graph->op_arrays[i], call_graph->func_infos + i);
	}
	zend_analyze_recursion(call_graph);
	zend_sort_op_arrays(call_graph);
}
/* }}} */

ZEND_API zend_call_info **zend_build_call_map(zend_arena **arena, zend_func_info *info, const zend_op_array *op_array) /* {{{ */
{
	zend_call_info **map, *call;
	if (!info->callee_info) {
		/* Don't build call map if function contains no calls */
		return NULL;
	}

	map = zend_arena_calloc(arena, sizeof(zend_call_info *), op_array->last);
	for (call = info->callee_info; call; call = call->next_callee) {
		int i;
		map[call->caller_init_opline - op_array->opcodes] = call;
		if (call->caller_call_opline) {
			map[call->caller_call_opline - op_array->opcodes] = call;
		}
		if (!call->is_frameless) {
			for (i = 0; i < call->num_args; i++) {
				if (call->arg_info[i].opline) {
					map[call->arg_info[i].opline - op_array->opcodes] = call;
				}
			}
		}
	}
	return map;
}
/* }}} */
