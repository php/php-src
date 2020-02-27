/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

static zend_jit_trace_info *zend_jit_traces = NULL;
static const void **zend_jit_exit_groups = NULL;

#define ZEND_JIT_COUNTER_NUM   zend_jit_traces[0].root
#define ZEND_JIT_TRACE_NUM     zend_jit_traces[0].id
#define ZEND_JIT_EXIT_NUM      zend_jit_traces[0].exit_count
#define ZEND_JIT_EXIT_COUNTERS zend_jit_traces[0].exit_counters

static int zend_jit_trace_startup(void)
{
	zend_jit_traces = (zend_jit_trace_info*)zend_shared_alloc(sizeof(zend_jit_trace_info) * ZEND_JIT_TRACE_MAX_TRACES);
	if (!zend_jit_traces) {
		return FAILURE;
	}
	zend_jit_exit_groups = (const void**)zend_shared_alloc(sizeof(void*) * (ZEND_JIT_TRACE_MAX_EXITS/ZEND_JIT_EXIT_POINTS_PER_GROUP));
	if (!zend_jit_exit_groups) {
		return FAILURE;
	}
	ZEND_JIT_TRACE_NUM = 1;
	ZEND_JIT_COUNTER_NUM = 0;
	ZEND_JIT_EXIT_NUM = 0;
	ZEND_JIT_EXIT_COUNTERS = 0;
	return SUCCESS;
}

static const void *zend_jit_trace_allocate_exit_group(uint32_t n)
{
	dasm_State* dasm_state = NULL;
	const void *entry;
	char name[32];

	dasm_init(&dasm_state, DASM_MAXSECTION);
	dasm_setupglobal(&dasm_state, dasm_labels, zend_lb_MAX);
	dasm_setup(&dasm_state, dasm_actions);
	zend_jit_trace_exit_group_stub(&dasm_state, n);

	sprintf(name, "jit$$trace_exit_%d", n);
	entry = dasm_link_and_encode(&dasm_state, NULL, NULL, NULL, NULL, name, 0);
	dasm_free(&dasm_state);

#ifdef HAVE_DISASM
    if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_ASM) {
		uint32_t i;

		for (i = 0; i < ZEND_JIT_EXIT_POINTS_PER_GROUP; i++) {
			sprintf(name, "jit$$trace_exit_%d", n + i);
			zend_jit_disasm_add_symbol(name, (uintptr_t)entry + (i * ZEND_JIT_EXIT_POINTS_SPACING), ZEND_JIT_EXIT_POINTS_SPACING);
		}
	}
#endif

	return entry;
}

static const void *zend_jit_trace_allocate_exit_point(uint32_t n)
{
	const void *group = NULL;

	if (UNEXPECTED(n >= ZEND_JIT_TRACE_MAX_EXITS)) {
		return NULL;
	}
	do {
		group = zend_jit_trace_allocate_exit_group(ZEND_JIT_EXIT_NUM);
		if (!group) {
			return NULL;
		}
		zend_jit_exit_groups[ZEND_JIT_EXIT_NUM / ZEND_JIT_EXIT_POINTS_PER_GROUP] =
			group;
		ZEND_JIT_EXIT_NUM += ZEND_JIT_EXIT_POINTS_PER_GROUP;
	} while (n >= ZEND_JIT_EXIT_NUM);
    return (const void*)
		((const char*)group +
		((n % ZEND_JIT_EXIT_POINTS_PER_GROUP) * ZEND_JIT_EXIT_POINTS_SPACING));
}

static const void *zend_jit_trace_get_exit_addr(uint32_t n)
{
	if (UNEXPECTED(n >= ZEND_JIT_EXIT_NUM)) {
		return zend_jit_trace_allocate_exit_point(n);
	}
    return (const void*)
		((const char*)zend_jit_exit_groups[n / ZEND_JIT_EXIT_POINTS_PER_GROUP] +
		((n % ZEND_JIT_EXIT_POINTS_PER_GROUP) * ZEND_JIT_EXIT_POINTS_SPACING));
}

static uint32_t zend_jit_trace_get_exit_point(const zend_op *from_opline, const zend_op *to_opline, zend_jit_trace_rec *trace)
{
	zend_jit_trace_info *t = &zend_jit_traces[ZEND_JIT_TRACE_NUM];
	uint32_t exit_point;
	const zend_op_array *op_array = &JIT_G(current_frame)->func->op_array;
	uint32_t stack_offset = (uint32_t)-1;
	uint32_t stack_size = op_array->last_var + op_array->T;
	uint8_t *stack = NULL;

	if (stack_size) {
		stack = JIT_G(current_frame)->stack;
		do {
			if (stack[stack_size-1] != IS_UNKNOWN) {
				break;
			}
			stack_size--;
		} while (stack_size);
	}

	/* Try to reuse exit points */
	if (to_opline != NULL && t->exit_count > 0) {
		uint32_t i = t->exit_count;

		do {
			i--;
			if (stack_size == 0
			 || (t->exit_info[i].stack_size >= stack_size
			  && memcmp(t->stack_map + t->exit_info[i].stack_offset, stack, stack_size) == 0)) {
				stack_offset = t->exit_info[i].stack_offset;
				if (t->exit_info[i].opline == to_opline) {
					return i;
				}
			}
		} while (i > 0);
	}

	exit_point = t->exit_count;
	if (exit_point < ZEND_JIT_TRACE_MAX_EXITS) {
		if (stack_size != 0 && stack_offset == (uint32_t)-1) {
			stack_offset = t->stack_map_size;
			t->stack_map_size += stack_size;
			// TODO: reduce number of reallocations ???
			t->stack_map = erealloc(t->stack_map, t->stack_map_size);
			memcpy(t->stack_map + stack_offset, stack, stack_size);
		}
		t->exit_count++;
		t->exit_info[exit_point].opline = to_opline;
		t->exit_info[exit_point].stack_size = stack_size;
		t->exit_info[exit_point].stack_offset = stack_offset;
	}

	return exit_point;
}

static void zend_jit_trace_add_code(const void *start, uint32_t size)
{
	zend_jit_trace_info *t = &zend_jit_traces[ZEND_JIT_TRACE_NUM];

	t->code_start = start;
	t->code_size  = size;
}

static uint32_t zend_jit_find_trace(const void *addr)
{
	uint32_t i;

	for (i = 1; i < ZEND_JIT_TRACE_NUM; i++) {
		if (zend_jit_traces[i].code_start == addr) {
			return i;
		}
	}
	ZEND_ASSERT(0);
	return 0;
}

static zend_string *zend_jit_trace_name(const zend_op_array *op_array, uint32_t lineno)
{
	smart_str buf = {0};

	smart_str_appends(&buf, TRACE_PREFIX);
	smart_str_append_long(&buf, (zend_long)ZEND_JIT_TRACE_NUM);
	smart_str_appendc(&buf, '$');
	if (op_array->function_name) {
		if (op_array->scope) {
			smart_str_appendl(&buf, ZSTR_VAL(op_array->scope->name), ZSTR_LEN(op_array->scope->name));
			smart_str_appends(&buf, "::");
			smart_str_appendl(&buf, ZSTR_VAL(op_array->function_name), ZSTR_LEN(op_array->function_name));
		} else {
			smart_str_appendl(&buf, ZSTR_VAL(op_array->function_name), ZSTR_LEN(op_array->function_name));
		}
	} else if (op_array->filename) {
		smart_str_appendl(&buf, ZSTR_VAL(op_array->filename), ZSTR_LEN(op_array->filename));
	}
	smart_str_appendc(&buf, '$');
	smart_str_append_long(&buf, (zend_long)lineno);
	smart_str_0(&buf);
	return buf.s;
}

static int zend_jit_trace_may_exit(const zend_op_array *op_array, const zend_op *opline, zend_jit_trace_rec *trace)
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
		case ZEND_ARRAY_KEY_EXISTS:
			if (opline->result_type & (IS_SMART_BRANCH_JMPNZ | IS_SMART_BRANCH_JMPZ)) {
				/* smart branch */
				return 1;
			}
			break;
		case ZEND_JMPZNZ:
		case ZEND_JMPZ:
		case ZEND_JMPNZ:
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
		case ZEND_JMP_SET:
		case ZEND_COALESCE:
		case ZEND_FE_RESET_R:
		case ZEND_FE_RESET_RW:
		case ZEND_ASSERT_CHECK:
		case ZEND_FE_FETCH_R:
		case ZEND_FE_FETCH_RW:
		case ZEND_SWITCH_LONG:
		case ZEND_SWITCH_STRING:
			/* branch opcdoes */
			return 1;
		case ZEND_NEW:
			if (opline->extended_value == 0 && (opline+1)->opcode == ZEND_DO_FCALL) {
				/* NEW may skip constructor without argumnts */
				return 1;
			}
			break;
		case ZEND_CATCH:
		case ZEND_FAST_CALL:
		case ZEND_FAST_RET:
		case ZEND_GENERATOR_CREATE:
		case ZEND_GENERATOR_RETURN:
		case ZEND_EXIT:
		case ZEND_YIELD:
		case ZEND_YIELD_FROM:
		case ZEND_INCLUDE_OR_EVAL:
			/* unsupported */
			return 1;
		case ZEND_DO_FCALL:
			/* potentialy polimorhic call */
			return 1;
#if 0
		case ZEND_DO_UCALL:
		case ZEND_DO_FCALL_BY_NAME:
			/* monomorthic call */
			// TODO: recompilation may change traget ???
			return 0;
#endif
		case ZEND_RETURN_BY_REF:
		case ZEND_RETURN:
			/* return */
			return trace->op == ZEND_JIT_TRACE_BACK && trace->recursive;
		default:
			break;
	}
	return 0;
}

#define STACK_VAR_TYPE(_opline, op) \
	stack[EX_VAR_TO_NUM((_opline)->op.var)]

#if 1
# define SET_STACK_VAR_TYPE(_opline, op, _type) do { \
		STACK_VAR_TYPE(_opline, op) = _type; \
	} while (0)
# define CHECK_OP_TRACE_TYPE(_opline, op, op_info, op_type) do { \
		if (op_type != IS_UNKNOWN) { \
			if ((op_info & (MAY_BE_ANY|MAY_BE_UNDEF)) != (1 << op_type) \
			 && op_type != STACK_VAR_TYPE(_opline, op)) { \
				if (!zend_jit_type_guard(&dasm_state, opline, (_opline)->op.var, op_type)) { \
					goto jit_failure; \
				} \
			} \
			SET_STACK_VAR_TYPE(_opline, op, op_type); \
			if (op_type < IS_STRING) { \
				op_info = (1 << op_type); \
			} else if (op_type != IS_ARRAY) { \
				op_info = (1 << op_type) | (op_info & (MAY_BE_RC1|MAY_BE_RCN)); \
			} else { \
				op_info = MAY_BE_ARRAY | (op_info & (MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_OF_REF|MAY_BE_ARRAY_KEY_ANY|MAY_BE_RC1|MAY_BE_RCN)); \
			} \
		} \
	} while (0)
# define USE_OP_TRACE_TYPE(_opline, op, op_info) do { \
		if ((_opline)->op ## _type & (IS_TMP_VAR|IS_VAR|IS_CV)) { \
			uint8_t type = STACK_VAR_TYPE(_opline, op); \
			if (type != IS_UNKNOWN \
			 && (op_info & (MAY_BE_ANY|MAY_BE_UNDEF)) != (1 << type)) { \
				if (type < IS_STRING) { \
					op_info = (1 << type); \
				} else if (type != IS_ARRAY) { \
					op_info = (1 << type) | (op_info & (MAY_BE_RC1|MAY_BE_RCN)); \
				} else { \
					op_info = MAY_BE_ARRAY | (op_info & (MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_OF_REF|MAY_BE_ARRAY_KEY_ANY|MAY_BE_RC1|MAY_BE_RCN)); \
				} \
			} \
		} \
	} while (0)
#else
# define SET_STACK_VAR_TYPE(_var, _type)
# define CHECK_OP_TRACE_TYPE(op)
#endif

#define CHECK_OP1_TRACE_TYPE() \
	CHECK_OP_TRACE_TYPE(opline, op1, op1_info, op1_type)
#define CHECK_OP2_TRACE_TYPE() \
	CHECK_OP_TRACE_TYPE(opline, op2, op2_info, op2_type)
#define CHECK_OP1_DATA_TRACE_TYPE() \
	CHECK_OP_TRACE_TYPE(opline+1, op1, op1_data_info, op3_type)
#define USE_OP1_TRACE_TYPE() \
	USE_OP_TRACE_TYPE(opline, op1, op1_info)
#define USE_OP2_TRACE_TYPE() \
	USE_OP_TRACE_TYPE(opline, op2, op2_info)
#define USE_OP1_DATA_TRACE_TYPE() \
	USE_OP_TRACE_TYPE(opline+1, op1, op1_data_info)
#define USE_RES_TRACE_TYPE() \
	USE_OP_TRACE_TYPE(opline, result, res_use_info)
#define SET_OP1_STACK_VAR_TYPE(_type) \
	SET_STACK_VAR_TYPE(opline, op1, _type)
#define SET_OP2_STACK_VAR_TYPE( _type) \
	SET_STACK_VAR_TYPE(opline, op2, _type)
#define SET_OP1_DATA_STACK_VAR_TYPE(_type) \
	SET_STACK_VAR_TYPE(opline+1, op1, _type)
#define SET_RES_STACK_VAR_TYPE(_type) \
	SET_STACK_VAR_TYPE(opline, result, _type)
#define RESET_OP1_STACK_VAR_TYPE() \
	SET_OP1_STACK_VAR_TYPE(IS_UNKNOWN)
#define RESET_OP2_STACK_VAR_TYPE() \
	SET_OP2_STACK_VAR_TYPE(IS_UNKNOWN)
#define RESET_RES_STACK_VAR_TYPE() \
	SET_RES_STACK_VAR_TYPE(IS_UNKNOWN)

static zend_always_inline size_t zend_jit_trace_frame_size(const zend_op_array *op_array)
{
	return offsetof(zend_jit_trace_stack_frame, stack) + ZEND_MM_ALIGNED_SIZE(op_array->last_var + op_array->T);
}

static zend_jit_trace_stack_frame* zend_jit_trace_allocate_stack(zend_jit_trace_rec *trace_buffer)
{
	int level, ret_level, call_level;
	size_t stack_top, stack_size, stack_bottom;
	zend_jit_trace_rec *p;
	const zend_op_array *op_array;
	uint8_t *stack;

	p = trace_buffer;
	ZEND_ASSERT(p->op == ZEND_JIT_TRACE_START);
	op_array = p->op_array;
	stack_size = stack_top = zend_jit_trace_frame_size(op_array);
	stack_bottom = 0;
	level = ret_level = call_level = 0;
	p += ZEND_JIT_TRACE_START_REC_SIZE;
	for (;;p++) {
		if (p->op == ZEND_JIT_TRACE_INIT_CALL) {
			if (p->func->type == ZEND_USER_FUNCTION) {
				if (level == 0) {
					call_level++;
				}
				level++;
				stack_top += zend_jit_trace_frame_size(p->op_array);
				if (stack_top > stack_size) {
					stack_size = stack_top;
				}
			}
		} else if (p->op == ZEND_JIT_TRACE_ENTER) {
			op_array = p->op_array;
			if (level == 0) {
				if (call_level) {
					call_level--;
				} else {
					level++;
					stack_top += zend_jit_trace_frame_size(op_array);
					if (stack_top > stack_size) {
						stack_size = stack_top;
					}
				}
			}
		} else if (p->op == ZEND_JIT_TRACE_BACK) {
			stack_top -= zend_jit_trace_frame_size(op_array);
			if (level == 0) {
				ZEND_ASSERT(stack_top == 0);
				call_level = 0;
				ret_level++;
				op_array = p->op_array;
				stack_top = zend_jit_trace_frame_size(op_array);
				stack_bottom += stack_top;
				if (stack_top > stack_size) {
					stack_size = stack_top;
				}
			} else {
				level--;
				op_array = p->op_array;
			}
		} else if (p->op == ZEND_JIT_TRACE_END) {
			break;
		}
	}

	stack = zend_arena_alloc(&CG(arena), stack_bottom + stack_size);
	return (zend_jit_trace_stack_frame*)(stack + stack_bottom);
}

static zend_jit_trace_stack_frame* zend_jit_trace_call_frame(zend_jit_trace_stack_frame *frame, const zend_op_array *op_array)
{
	return (zend_jit_trace_stack_frame*)((char*)frame + zend_jit_trace_frame_size(op_array));
}

static zend_jit_trace_stack_frame* zend_jit_trace_ret_frame(zend_jit_trace_stack_frame *frame, const zend_op_array *op_array)
{
	return (zend_jit_trace_stack_frame*)((char*)frame - zend_jit_trace_frame_size(op_array));
}

static void zend_jit_trace_send_type(const zend_op *opline, zend_jit_trace_stack_frame *call, zend_uchar type)
{
	uint8_t *stack = call->stack;
	const zend_op_array *op_array = &call->func->op_array;
	uint32_t arg_num = opline->op2.num;

	if (arg_num > op_array->num_args) {
		return;
	}
	if (op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS) {
		zend_arg_info *arg_info;

		ZEND_ASSERT(arg_num <= op_array->num_args);
		arg_info = &op_array->arg_info[arg_num-1];

		if (ZEND_TYPE_IS_SET(arg_info->type)) {
			if (!(ZEND_TYPE_FULL_MASK(arg_info->type) & (1u << type))) {
				return;
			}
		}
	}
	SET_STACK_VAR_TYPE(opline, result, type);
}

static zend_ssa *zend_jit_trace_build_ssa(const zend_op_array *op_array, zend_script *script)
{
	zend_jit_op_array_trace_extension *jit_extension;
	zend_ssa *ssa;

	jit_extension =
		(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);

	memset(&jit_extension->func_info, 0, sizeof(jit_extension->func_info));
	jit_extension->func_info.num_args = -1;
	jit_extension->func_info.return_value_used = -1;
	ssa = &jit_extension->func_info.ssa;
	if (zend_jit_op_array_analyze1(op_array, script, ssa) != SUCCESS) {
		return NULL;
	}

	if (zend_jit_level >= ZEND_JIT_LEVEL_OPT_FUNC) {
		if (zend_jit_level >= ZEND_JIT_LEVEL_OPT_FUNCS) {
			if (zend_analyze_calls(&CG(arena), script, ZEND_CALL_TREE, (zend_op_array*)op_array, &jit_extension->func_info) != SUCCESS) {
				return NULL;
			}
			jit_extension->func_info.call_map = zend_build_call_map(&CG(arena), &jit_extension->func_info, (zend_op_array*)op_array);
			if (op_array->fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
				zend_init_func_return_info(op_array, script, &jit_extension->func_info.return_info);
			}
		}

		if (zend_jit_op_array_analyze2(op_array, script, ssa) != SUCCESS) {
			return NULL;
		}

		if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_SSA) {
			zend_dump_op_array(op_array, ZEND_DUMP_HIDE_UNREACHABLE|ZEND_DUMP_RC_INFERENCE|ZEND_DUMP_SSA, "JIT", ssa);
		}
	}

	return ssa;
}

static const void *zend_jit_trace(zend_jit_trace_rec *trace_buffer, uint32_t parent_trace, uint32_t exit_num)
{
	const void *handler = NULL;
	dasm_State* dasm_state = NULL;
	zend_script *script = NULL;
	zend_lifetime_interval **ra = NULL;
	zend_string *name = NULL;
	void *checkpoint;
	const zend_op_array *op_array;
	zend_ssa *ssa;
	zend_jit_trace_rec *p;
	int call_level = -1; // TODO: proper support for inlined functions ???
	zend_jit_op_array_trace_extension *jit_extension;
	int num_op_arrays = 0;
	zend_jit_trace_info *t;
	const  zend_op_array *op_arrays[ZEND_JIT_TRACE_MAX_FUNCS];
	zend_uchar smart_branch_opcode;
	const void *exit_addr;
	uint32_t op1_info, op1_def_info, op2_info, res_info, res_use_info, op1_data_info;
	zend_bool send_result;
	zend_jit_addr op1_addr, op1_def_addr, op2_addr, op2_def_addr, res_addr;
	zend_class_entry *ce;
	uint32_t i;
	zend_jit_trace_stack_frame *frame, *top, *call;
	uint8_t *stack;
	zend_uchar res_type = IS_UNKNOWN;
	const zend_op *opline;

	checkpoint = zend_arena_checkpoint(CG(arena));

	p = trace_buffer;
	ZEND_ASSERT(p->op == ZEND_JIT_TRACE_START);
	op_array = p->op_array;
	JIT_G(current_frame) = frame = zend_jit_trace_allocate_stack(trace_buffer);
	top = zend_jit_trace_call_frame(frame, op_array);
	frame->call = NULL;
	frame->prev = NULL;
	frame->func = (const zend_function*)op_array;
	frame->return_value_used = -1;
	stack = frame->stack;
	i = 0;
	if (parent_trace) {
		i = MIN(zend_jit_traces[parent_trace].exit_info[exit_num].stack_size,
			op_array->last_var + op_array->T);
		if (i) {
			memcpy(stack,
				zend_jit_traces[parent_trace].stack_map +
				zend_jit_traces[parent_trace].exit_info[exit_num].stack_offset,
				i);
		}
	}
	while (i < op_array->last_var + op_array->T) {
		stack[i] = IS_UNKNOWN;
		i++;
	}
	opline = ((zend_jit_trace_start_rec*)p)->opline;
	name = zend_jit_trace_name(op_array, opline->lineno);
	p += ZEND_JIT_TRACE_START_REC_SIZE;

	dasm_init(&dasm_state, DASM_MAXSECTION);
	dasm_setupglobal(&dasm_state, dasm_labels, zend_lb_MAX);
	dasm_setup(&dasm_state, dasm_actions);

	/* Remember op_array to cleanup */
	op_arrays[num_op_arrays++] = op_array;

	/* Build SSA */
	ssa = zend_jit_trace_build_ssa(op_array, script);
	if (!ssa) {
		goto jit_failure;
	}

	// TODO: register allocation ???

	dasm_growpc(&dasm_state, 1); /* trace needs just one global lable for loop */

	zend_jit_align_func(&dasm_state);
	if (!parent_trace) {
		zend_jit_prologue(&dasm_state);
	}
	zend_jit_trace_begin(&dasm_state, ZEND_JIT_TRACE_NUM);

	if (!parent_trace) {
		zend_jit_set_opline(&dasm_state, opline);
	} else {
		if (!((uintptr_t)zend_jit_traces[parent_trace].exit_info[exit_num].opline & ~(ZEND_JIT_EXIT_JITED|ZEND_JIT_EXIT_BLACKLISTED))) {
			zend_jit_trace_opline_guard(&dasm_state, opline);
			zend_jit_set_opline(&dasm_state, opline);
		} else {
			zend_jit_reset_opline(&dasm_state, opline);
		}
	}

	if (trace_buffer->stop == ZEND_JIT_TRACE_STOP_LOOP
	 || trace_buffer->stop == ZEND_JIT_TRACE_STOP_RECURSIVE_CALL
	 || trace_buffer->stop == ZEND_JIT_TRACE_STOP_RECURSIVE_RET) {
		zend_jit_label(&dasm_state, 0); /* start of of trace loop */
		if (trace_buffer->stop != ZEND_JIT_TRACE_STOP_RECURSIVE_RET) {
			// TODO: interupt exit may require deoptimization through side exit ???
			zend_jit_check_timeout(&dasm_state, opline);
		}
	}

	for (;;p++) {
		if (p->op == ZEND_JIT_TRACE_VM) {
			uint8_t op1_type = p->op1_type;
			uint8_t op2_type = p->op2_type;
			uint8_t op3_type = p->op3_type;

			opline = p->opline;
			if (op1_type & IS_TRACE_REFERENCE) {
				op1_type = IS_UNKNOWN;
			}
			if (op2_type & IS_TRACE_REFERENCE) {
				op2_type = IS_UNKNOWN;
			}
			if (op3_type & IS_TRACE_REFERENCE) {
				op3_type = IS_UNKNOWN;
			}

			if ((p+1)->op == ZEND_JIT_TRACE_OP1_TYPE) {
				// TODO: support for recorded classes ???
				p++;
			}
			if ((p+1)->op == ZEND_JIT_TRACE_OP2_TYPE) {
				// TODO: support for recorded classes ???
				p++;
			}

#if 0
			// TODO: call level calculation doesn't work for traces ???
			switch (opline->opcode) {
				case ZEND_INIT_FCALL:
				case ZEND_INIT_FCALL_BY_NAME:
				case ZEND_INIT_NS_FCALL_BY_NAME:
				case ZEND_INIT_METHOD_CALL:
				case ZEND_INIT_DYNAMIC_CALL:
				case ZEND_INIT_STATIC_METHOD_CALL:
				case ZEND_INIT_USER_CALL:
				case ZEND_NEW:
					call_level++;
			}
#endif

			if (zend_jit_level >= ZEND_JIT_LEVEL_INLINE) {
				switch (opline->opcode) {
					case ZEND_PRE_INC:
					case ZEND_PRE_DEC:
					case ZEND_POST_INC:
					case ZEND_POST_DEC:
						if (opline->op1_type != IS_CV) {
							break;
						}
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						stack[EX_VAR_TO_NUM(opline->op1.var)] = IS_UNKNOWN;
						if (!(op1_info & MAY_BE_LONG)) {
							break;
						}
						if (opline->result_type != IS_UNUSED) {
							res_use_info = RES_USE_INFO();
							USE_RES_TRACE_TYPE();
							res_info = RES_INFO();
							res_addr = RES_REG_ADDR();
						} else {
							res_use_info = -1;
							res_info = -1;
							res_addr = 0;
						}
						op1_def_info = OP1_DEF_INFO();
						if (!zend_jit_inc_dec(&dasm_state, opline, op_array,
								op1_info, OP1_REG_ADDR(),
								op1_def_info, OP1_DEF_REG_ADDR(),
								res_use_info, res_info,
								res_addr,
								(op1_def_info & MAY_BE_LONG) && (op1_def_info & MAY_BE_DOUBLE) && zend_may_overflow(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_BW_OR:
					case ZEND_BW_AND:
					case ZEND_BW_XOR:
					case ZEND_SL:
					case ZEND_SR:
					case ZEND_MOD:
						op1_info = OP1_INFO();
						op2_info = OP2_INFO();
						CHECK_OP1_TRACE_TYPE();
						CHECK_OP2_TRACE_TYPE();
						if ((op1_info & MAY_BE_UNDEF) || (op2_info & MAY_BE_UNDEF)) {
							break;
						}
						if (!(op1_info & MAY_BE_LONG)
						 || !(op2_info & MAY_BE_LONG)) {
							break;
						}
						if (opline->result_type == IS_TMP_VAR
						 && (p+1)->op == ZEND_JIT_TRACE_VM
						 && (p+1)->opline == opline + 1
						 && (opline+1)->opcode == ZEND_SEND_VAL
						 && (opline+1)->op1_type == IS_TMP_VAR
						 && (opline+1)->op1.var == opline->result.var) {
							p++;
							if (frame->call) {
								uint8_t res_type = p->op1_type;
								if (res_type & IS_TRACE_REFERENCE) {
									res_type = IS_UNKNOWN;;
								}
								if (res_type != IS_UNKNOWN) {
									zend_jit_trace_send_type(opline+1, frame->call, res_type);
								}
							}
							while ((p+1)->op == ZEND_JIT_TRACE_OP1_TYPE ||
							      (p+1)->op == ZEND_JIT_TRACE_OP2_TYPE) {
								p++;
							}
							send_result = 1;
							res_use_info = -1;
							res_addr = 0; /* set inside backend */
						} else {
							send_result = 0;
							res_use_info = RES_USE_INFO();
							USE_RES_TRACE_TYPE();
							res_addr = RES_REG_ADDR();
						}
						res_info = RES_INFO();
						if (!zend_jit_long_math(&dasm_state, opline, op_array,
								op1_info, OP1_RANGE(), OP1_REG_ADDR(),
								op2_info, OP2_RANGE(), OP2_REG_ADDR(),
								res_use_info, res_info, res_addr,
								send_result,
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ADD:
					case ZEND_SUB:
					case ZEND_MUL:
//					case ZEND_DIV: // TODO: check for division by zero ???
						op1_info = OP1_INFO();
						op2_info = OP2_INFO();
						CHECK_OP1_TRACE_TYPE();
						CHECK_OP2_TRACE_TYPE();
						if ((op1_info & MAY_BE_UNDEF) || (op2_info & MAY_BE_UNDEF)) {
							break;
						}
						if (!(op1_info & (MAY_BE_LONG|MAY_BE_DOUBLE)) ||
						    !(op2_info & (MAY_BE_LONG|MAY_BE_DOUBLE))) {
							break;
						}
						if (opline->result_type == IS_TMP_VAR
						 && (p+1)->op == ZEND_JIT_TRACE_VM
						 && (p+1)->opline == opline + 1
						 && (opline+1)->opcode == ZEND_SEND_VAL
						 && (opline+1)->op1_type == IS_TMP_VAR
						 && (opline+1)->op1.var == opline->result.var) {
							p++;
							if (frame->call
							 && frame->call->func->type == ZEND_USER_FUNCTION) {
								uint8_t res_type = p->op1_type;
								if (res_type & IS_TRACE_REFERENCE) {
									res_type = IS_UNKNOWN;;
								}
								if (res_type != IS_UNKNOWN) {
									zend_jit_trace_send_type(opline+1, frame->call, res_type);
								}
							}
							while ((p+1)->op == ZEND_JIT_TRACE_OP1_TYPE ||
							      (p+1)->op == ZEND_JIT_TRACE_OP2_TYPE) {
								p++;
							}
							send_result = 1;
							res_use_info = -1;
							res_addr = 0; /* set inside backend */
						} else {
							send_result = 0;
							res_use_info = RES_USE_INFO();
							USE_RES_TRACE_TYPE();
							res_addr = RES_REG_ADDR();
						}
						res_info = RES_INFO();
						if (!zend_jit_math(&dasm_state, opline, op_array,
								op1_info, OP1_REG_ADDR(),
								op2_info, OP2_REG_ADDR(),
								res_use_info, res_info, res_addr,
								send_result,
								(res_info & MAY_BE_LONG) && (res_info & MAY_BE_DOUBLE) && zend_may_overflow(opline, op_array, ssa),
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_CONCAT:
					case ZEND_FAST_CONCAT:
						op1_info = OP1_INFO();
						op2_info = OP2_INFO();
						CHECK_OP1_TRACE_TYPE();
						CHECK_OP2_TRACE_TYPE();
						if ((op1_info & MAY_BE_UNDEF) || (op2_info & MAY_BE_UNDEF)) {
							break;
						}
						if (!(op1_info & MAY_BE_STRING) ||
						    !(op2_info & MAY_BE_STRING)) {
							break;
						}
						if (opline->result_type == IS_TMP_VAR
						 && (p+1)->op == ZEND_JIT_TRACE_VM
						 && (p+1)->opline == opline + 1
						 && (opline+1)->opcode == ZEND_SEND_VAL
						 && (opline+1)->op1_type == IS_TMP_VAR
						 && (opline+1)->op1.var == opline->result.var) {
							p++;
							if (frame->call
							 && frame->call->func->type == ZEND_USER_FUNCTION) {
								uint8_t res_type = p->op1_type;
								if (res_type & IS_TRACE_REFERENCE) {
									res_type = IS_UNKNOWN;;
								}
								if (res_type != IS_UNKNOWN) {
									zend_jit_trace_send_type(opline+1, frame->call, res_type);
								}
							}
							while ((p+1)->op == ZEND_JIT_TRACE_OP1_TYPE ||
							      (p+1)->op == ZEND_JIT_TRACE_OP2_TYPE) {
								p++;
							}
							send_result = 1;
						} else {
							send_result = 0;
						}
						res_info = RES_INFO();
						if (!zend_jit_concat(&dasm_state, opline, op_array,
								op1_info, op2_info, res_info, send_result,
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ASSIGN_OP:
						if (opline->extended_value == ZEND_POW
						 || opline->extended_value == ZEND_DIV) {
							// TODO: check for division by zero ???
							break;
						}
						if (opline->op1_type != IS_CV || opline->result_type != IS_UNUSED) {
							break;
						}
						op1_info = OP1_INFO();
						op2_info = OP2_INFO();
						CHECK_OP1_TRACE_TYPE();
						CHECK_OP2_TRACE_TYPE();
						if ((op1_info & MAY_BE_UNDEF) || (op2_info & MAY_BE_UNDEF)) {
							break;
						}
						if (opline->extended_value == ZEND_ADD
						 || opline->extended_value == ZEND_SUB
						 || opline->extended_value == ZEND_MUL
						 || opline->extended_value == ZEND_DIV) {
							if (!(op1_info & (MAY_BE_LONG|MAY_BE_DOUBLE))
							 || !(op2_info & (MAY_BE_LONG|MAY_BE_DOUBLE))) {
								break;
							}
						} else if (opline->extended_value == ZEND_BW_OR
						 || opline->extended_value == ZEND_BW_AND
						 || opline->extended_value == ZEND_BW_XOR
						 || opline->extended_value == ZEND_SL
						 || opline->extended_value == ZEND_SR
						 || opline->extended_value == ZEND_MOD) {
							if (!(op1_info & MAY_BE_LONG)
							 || !(op2_info & MAY_BE_LONG)) {
								break;
							}
						} else if (opline->extended_value == ZEND_CONCAT) {
							if (!(op1_info & MAY_BE_STRING)
							 || !(op2_info & MAY_BE_STRING)) {
								break;
							}
						}
						op1_def_info = OP1_DEF_INFO();
						if (!zend_jit_assign_op(&dasm_state, opline, op_array,
								op1_info, op1_def_info, OP1_RANGE(),
								op2_info, OP2_RANGE(),
								(op1_def_info & MAY_BE_LONG) && (op1_def_info & MAY_BE_DOUBLE) && zend_may_overflow(opline, op_array, ssa),
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ASSIGN_DIM_OP:
						if (opline->extended_value == ZEND_POW
						 || opline->extended_value == ZEND_DIV) {
							// TODO: check for division by zero ???
							break;
						}
						if (opline->op1_type != IS_CV || opline->result_type != IS_UNUSED) {
							break;
						}
						op1_info = OP1_INFO();
						op2_info = OP2_INFO();
						op1_data_info = OP1_DATA_INFO();
						CHECK_OP1_TRACE_TYPE();
						CHECK_OP2_TRACE_TYPE();
						CHECK_OP1_DATA_TRACE_TYPE();
						op1_def_info = OP1_DEF_INFO();
						if (!zend_jit_assign_dim_op(&dasm_state, opline, op_array,
								op1_info, op1_def_info, op2_info,
								op1_data_info, OP1_DATA_RANGE(),
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ASSIGN_DIM:
						if (opline->op1_type != IS_CV) {
							break;
						}
						op1_info = OP1_INFO();
						op2_info = OP2_INFO();
						op1_data_info = OP1_DATA_INFO();
						CHECK_OP1_TRACE_TYPE();
						CHECK_OP2_TRACE_TYPE();
						CHECK_OP1_DATA_TRACE_TYPE();
						if (!zend_jit_assign_dim(&dasm_state, opline, op_array,
								op1_info, op2_info, op1_data_info,
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ASSIGN:
						if (opline->op1_type != IS_CV) {
							break;
						}
						if (opline->result_type == IS_UNUSED) {
							res_addr = 0;
							res_info = -1;
						} else {
							res_addr = RES_REG_ADDR();
							res_info = RES_INFO();
						}
						op2_addr = OP2_REG_ADDR();
						if (ra
						 && ssa->ops[opline - op_array->opcodes].op2_def >= 0
						 && !ssa->vars[ssa->ops[opline - op_array->opcodes].op2_def].no_val) {
							op2_def_addr = OP2_DEF_REG_ADDR();
						} else {
							op2_def_addr = op2_addr;
						}
						op1_info = OP1_INFO();
						op2_info = OP2_INFO();
						op1_def_info = OP1_DEF_INFO();
						USE_OP1_TRACE_TYPE();
						CHECK_OP2_TRACE_TYPE();
						if (!zend_jit_assign(&dasm_state, opline, op_array,
								op1_info, OP1_REG_ADDR(),
								op1_def_info, OP1_DEF_REG_ADDR(),
								op2_info, op2_addr, op2_def_addr,
								res_info, res_addr,
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_QM_ASSIGN:
						op1_addr = OP1_REG_ADDR();
						if (ra
						 && ssa->ops[opline - op_array->opcodes].op1_def >= 0
						 && !ssa->vars[ssa->ops[opline - op_array->opcodes].op1_def].no_val) {
							op1_def_addr = OP1_DEF_REG_ADDR();
						} else {
							op1_def_addr = op1_addr;
						}
						op1_info = OP1_INFO();
						res_info = RES_INFO();
						CHECK_OP1_TRACE_TYPE();//???USE_OP1_TRACE_TYPE();
						if (!zend_jit_qm_assign(&dasm_state, opline, op_array,
								op1_info, op1_addr, op1_def_addr,
								res_info, RES_REG_ADDR())) {
							goto jit_failure;
						}
						goto done;
					case ZEND_INIT_FCALL:
					case ZEND_INIT_FCALL_BY_NAME:
						if (!zend_jit_init_fcall(&dasm_state, opline, ssa->cfg.map ? ssa->cfg.map[opline - op_array->opcodes] : -1, op_array, ssa, call_level, p + 1)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_SEND_VAL:
					case ZEND_SEND_VAL_EX:
						if (opline->opcode == ZEND_SEND_VAL_EX
						 && opline->op2.num > MAX_ARG_FLAG_NUM) {
							break;
						}
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE(); //???USE_OP1_TRACE_TYPE();
						if (!zend_jit_send_val(&dasm_state, opline, op_array,
								op1_info, OP1_REG_ADDR())) {
							goto jit_failure;
						}
						if (frame->call
						 && frame->call->func->type == ZEND_USER_FUNCTION) {
							if (opline->op1_type == IS_CONST) {
								zend_jit_trace_send_type(opline, frame->call, Z_TYPE_P(RT_CONSTANT(opline, opline->op1)));
							} else if (op1_type != IS_UNKNOWN) {
								zend_jit_trace_send_type(opline, frame->call, op1_type);
							}
						}
						goto done;
					case ZEND_SEND_REF:
						op1_info = OP1_INFO();
						USE_OP1_TRACE_TYPE();
						if (!zend_jit_send_ref(&dasm_state, opline, op_array,
								op1_info, 0)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_SEND_VAR:
					case ZEND_SEND_VAR_EX:
					case ZEND_SEND_VAR_NO_REF:
					case ZEND_SEND_VAR_NO_REF_EX:
						if ((opline->opcode == ZEND_SEND_VAR_EX
						  || opline->opcode == ZEND_SEND_VAR_NO_REF_EX)
						 && opline->op2.num > MAX_ARG_FLAG_NUM) {
							break;
						}
						op1_addr = OP1_REG_ADDR();
						if (ra
						 && ssa->ops[opline - op_array->opcodes].op1_def >= 0
						 && !ssa->vars[ssa->ops[opline - op_array->opcodes].op1_def].no_val) {
							op1_def_addr = OP1_DEF_REG_ADDR();
						} else {
							op1_def_addr = op1_addr;
						}
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE(); //???USE_OP1_TRACE_TYPE();
						if (!zend_jit_send_var(&dasm_state, opline, op_array,
								op1_info, op1_addr, op1_def_addr)) {
							goto jit_failure;
						}
						if (frame->call
						 && frame->call->func->type == ZEND_USER_FUNCTION) {
							if (opline->opcode == ZEND_SEND_VAR_EX
							 && ARG_SHOULD_BE_SENT_BY_REF(frame->call->func, opline->op2.num)) {
								// TODO: this may require invalidation, if caller is changed ???
								goto done;
							}
							if (op1_type != IS_UNKNOWN) {
								zend_jit_trace_send_type(opline, frame->call, op1_type);
							}
						}
						goto done;
					case ZEND_DO_UCALL:
					case ZEND_DO_ICALL:
					case ZEND_DO_FCALL_BY_NAME:
					case ZEND_DO_FCALL:
						if (!zend_jit_do_fcall(&dasm_state, opline, op_array, ssa, call_level, -1, p + 1)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_IS_EQUAL:
					case ZEND_IS_NOT_EQUAL:
					case ZEND_IS_SMALLER:
					case ZEND_IS_SMALLER_OR_EQUAL:
					case ZEND_CASE:
						op1_info = OP1_INFO();
						op2_info = OP2_INFO();
						CHECK_OP1_TRACE_TYPE();
						CHECK_OP2_TRACE_TYPE();
						if ((opline->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
							zend_bool exit_if_true = 0;
							const zend_op *exit_opline = zend_jit_trace_get_exit_opline(p + 1, opline + 1, &exit_if_true);
							uint32_t exit_point = zend_jit_trace_get_exit_point(opline, exit_opline, p + 1);

							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								goto jit_failure;
							}
							smart_branch_opcode = exit_if_true ? ZEND_JMPNZ : ZEND_JMPZ;
						} else {
							smart_branch_opcode = 0;
							exit_addr = NULL;
						}
						if (!zend_jit_cmp(&dasm_state, opline, op_array,
								op1_info, OP1_REG_ADDR(),
								op2_info, OP2_REG_ADDR(),
								RES_REG_ADDR(),
								zend_may_throw(opline, op_array, ssa),
								smart_branch_opcode, -1, -1, exit_addr)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_IS_IDENTICAL:
					case ZEND_IS_NOT_IDENTICAL:
						op1_info = OP1_INFO();
						op2_info = OP2_INFO();
						CHECK_OP1_TRACE_TYPE();
						CHECK_OP2_TRACE_TYPE();
						if ((opline->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
							zend_bool exit_if_true = 0;
							const zend_op *exit_opline = zend_jit_trace_get_exit_opline(p + 1, opline + 1, &exit_if_true);
							uint32_t exit_point = zend_jit_trace_get_exit_point(opline, exit_opline, p + 1);

							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								goto jit_failure;
							}
							if (opline->opcode == ZEND_IS_NOT_IDENTICAL) {
								exit_if_true = !exit_if_true;
							}
							smart_branch_opcode = exit_if_true ? ZEND_JMPNZ : ZEND_JMPZ;
						} else {
							smart_branch_opcode = 0;
							exit_addr = NULL;
						}
						if (!zend_jit_identical(&dasm_state, opline, op_array,
								op1_info, OP1_REG_ADDR(),
								op2_info, OP2_REG_ADDR(),
								RES_REG_ADDR(),
								zend_may_throw(opline, op_array, ssa),
								smart_branch_opcode, -1, -1, exit_addr)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_DEFINED:
						if ((opline->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
							zend_bool exit_if_true = 0;
							const zend_op *exit_opline = zend_jit_trace_get_exit_opline(p + 1, opline + 1, &exit_if_true);
							uint32_t exit_point = zend_jit_trace_get_exit_point(opline, exit_opline, p + 1);

							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								goto jit_failure;
							}
							smart_branch_opcode = exit_if_true ? ZEND_JMPNZ : ZEND_JMPZ;
						} else {
							smart_branch_opcode = 0;
							exit_addr = NULL;
						}
						if (!zend_jit_defined(&dasm_state, opline, op_array, smart_branch_opcode, -1, -1, exit_addr)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_TYPE_CHECK:
						if (opline->extended_value == MAY_BE_RESOURCE) {
							// TODO: support for is_resource() ???
							break;
						}
						op1_info = OP1_INFO();
						USE_OP1_TRACE_TYPE();
						if ((opline->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
							zend_bool exit_if_true = 0;
							const zend_op *exit_opline = zend_jit_trace_get_exit_opline(p + 1, opline + 1, &exit_if_true);
							uint32_t exit_point = zend_jit_trace_get_exit_point(opline, exit_opline, p + 1);

							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								goto jit_failure;
							}
							smart_branch_opcode = exit_if_true ? ZEND_JMPNZ : ZEND_JMPZ;
						} else {
							smart_branch_opcode = 0;
							exit_addr = NULL;
						}
						if (!zend_jit_type_check(&dasm_state, opline, op_array, op1_info, smart_branch_opcode, -1, -1, exit_addr)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_RETURN:
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						if (opline->op1_type == IS_CONST) {
							res_type = Z_TYPE_P(RT_CONSTANT(opline, opline->op1));
						} else if (op1_type != IS_UNKNOWN) {
							res_type = op1_type;
						}
						if (op_array->type == ZEND_EVAL_CODE
						 // TODO: support for top-level code
						 || !op_array->function_name
						 // TODO: support for IS_UNDEF ???
						 || (op1_info & MAY_BE_UNDEF)) {
							if (!zend_jit_tail_handler(&dasm_state, opline)) {
								goto jit_failure;
							}
						} else {
							int j;

							if (!zend_jit_return(&dasm_state, opline, op_array,
									op1_info, OP1_REG_ADDR())) {
								goto jit_failure;
							}
						    if (!zend_jit_leave_frame(&dasm_state)) {
								goto jit_failure;
						    }
							for (j = 0 ; j < op_array->last_var; j++) {
								uint32_t info = zend_ssa_cv_info(opline, op_array, ssa, j);
								zend_uchar type = stack[j];

								if (type != IS_UNKNOWN) {
									if (type < IS_STRING) {
										info = (1 << type);
									} else if (type != IS_ARRAY) {
										info = (1 << type) | (info & (MAY_BE_RC1|MAY_BE_RCN));
									} else {
										info = MAY_BE_ARRAY | (info & (MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_OF_REF|MAY_BE_ARRAY_KEY_ANY|MAY_BE_RC1|MAY_BE_RCN));
									}
								}
								if (info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF)) {
									if (!zend_jit_free_cv(&dasm_state, opline, op_array, info, j)) {
										goto jit_failure;
									}
								}
							}
							if (!zend_jit_leave_func(&dasm_state, opline, op_array, p + 1)) {
								goto jit_failure;
							}
						}
						goto done;
					case ZEND_BOOL:
					case ZEND_BOOL_NOT:
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						if (!zend_jit_bool_jmpznz(&dasm_state, opline, op_array,
								op1_info, OP1_REG_ADDR(), RES_REG_ADDR(),
								-1, -1,
								zend_may_throw(opline, op_array, ssa),
								opline->opcode, NULL)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_JMPZ:
					case ZEND_JMPNZ:
						if (/*opline > op_array->opcodes + ssa->cfg.blocks[b].start && ??? */
						    opline->op1_type == IS_TMP_VAR &&
						    ((opline-1)->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
							/* smart branch */
							break;
						}
						/* break missing intentionally */
					case ZEND_JMPZNZ:
					case ZEND_JMPZ_EX:
					case ZEND_JMPNZ_EX:
						if (opline->result_type == IS_UNDEF) {
							res_addr = 0;
						} else {
							res_addr = RES_REG_ADDR();
						}
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						if ((p+1)->op == ZEND_JIT_TRACE_VM || (p+1)->op == ZEND_JIT_TRACE_END) {
							const zend_op *exit_opline = NULL;
							uint32_t exit_point;

							if ((p+1)->opline == OP_JMP_ADDR(opline, opline->op2)) {
								/* taken branch */
								if (opline->opcode == ZEND_JMPNZ_EX) {
									smart_branch_opcode = ZEND_JMPZ_EX;
								} else if (opline->opcode == ZEND_JMPZ_EX) {
									smart_branch_opcode = ZEND_JMPNZ_EX;
								} else if (opline->opcode == ZEND_JMPNZ) {
									smart_branch_opcode = ZEND_JMPZ;
								} else {
									smart_branch_opcode = ZEND_JMPNZ;
								}
								exit_opline = (opline->opcode == ZEND_JMPZNZ) ?
									ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value) :
									opline + 1;
							} else if (opline->opcode == ZEND_JMPZNZ) {
								ZEND_ASSERT((p+1)->opline == ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value));
								smart_branch_opcode = ZEND_JMPZ;
								exit_opline = OP_JMP_ADDR(opline, opline->op2);
							} else if ((p+1)->opline == opline + 1) {
								/* not taken branch */
								smart_branch_opcode = opline->opcode;
								exit_opline = OP_JMP_ADDR(opline, opline->op2);
							} else {
								ZEND_ASSERT(0);
							}
							exit_point = zend_jit_trace_get_exit_point(opline, exit_opline, p+1);
							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								goto jit_failure;
							}
						} else  {
							ZEND_ASSERT(0);
						}
						if (!zend_jit_bool_jmpznz(&dasm_state, opline, op_array,
								op1_info, OP1_REG_ADDR(), res_addr,
								-1, -1,
								zend_may_throw(opline, op_array, ssa),
								smart_branch_opcode, exit_addr)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FETCH_DIM_R:
					case ZEND_FETCH_DIM_IS:
						op1_info = OP1_INFO();
						op2_info = OP2_INFO();
						CHECK_OP1_TRACE_TYPE();
						CHECK_OP2_TRACE_TYPE();
						res_info = RES_INFO();
						if (!zend_jit_fetch_dim_read(&dasm_state, opline, op_array,
								op1_info, op2_info, res_info,
								(
									(op1_info & MAY_BE_ANY) != MAY_BE_ARRAY ||
									(op2_info & (MAY_BE_ANY - (MAY_BE_LONG|MAY_BE_STRING))) != 0 ||
									((op1_info & MAY_BE_UNDEF) != 0 &&
										opline->opcode == ZEND_FETCH_DIM_R) ||
									((opline->op1_type & (IS_TMP_VAR|IS_VAR)) != 0 &&
										(op1_info & (MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_ARRAY_OF_RESOURCE|MAY_BE_ARRAY_OF_ARRAY)) != 0) ||
									(op2_info & MAY_BE_UNDEF) != 0 ||
									((opline->op2_type & (IS_TMP_VAR|IS_VAR)) != 0 &&
										(op2_info & (MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_ARRAY_OF_RESOURCE|MAY_BE_ARRAY_OF_ARRAY)) != 0)))) {
							goto jit_failure;
						}
						goto done;
						goto done;
					case ZEND_ISSET_ISEMPTY_DIM_OBJ:
						if ((opline->extended_value & ZEND_ISEMPTY)) {
							// TODO: support for empty() ???
							break;
						}
						op1_info = OP1_INFO();
						op2_info = OP2_INFO();
						CHECK_OP1_TRACE_TYPE();
						CHECK_OP2_TRACE_TYPE();
						if ((opline->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
							zend_bool exit_if_true = 0;
							const zend_op *exit_opline = zend_jit_trace_get_exit_opline(p + 1, opline + 1, &exit_if_true);
							uint32_t exit_point = zend_jit_trace_get_exit_point(opline, exit_opline, p + 1);

							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								goto jit_failure;
							}
							smart_branch_opcode = exit_if_true ? ZEND_JMPNZ : ZEND_JMPZ;
						} else {
							smart_branch_opcode = 0;
							exit_addr = NULL;
						}
						if (!zend_jit_isset_isempty_dim(&dasm_state, opline, op_array,
								op1_info, op2_info,
								zend_may_throw(opline, op_array, ssa),
								smart_branch_opcode, -1, -1,
								exit_addr)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FETCH_OBJ_R:
					case ZEND_FETCH_OBJ_IS:
						if (opline->op2_type != IS_CONST
						 || Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) != IS_STRING
						 || Z_STRVAL_P(RT_CONSTANT(opline, opline->op2))[0] == '\0') {
							break;
						}
						ce = NULL;
						if (opline->op1_type == IS_UNUSED) {
							op1_info = MAY_BE_OBJECT|MAY_BE_RC1|MAY_BE_RCN;
							ce = op_array->scope;
						} else {
							op1_info = OP1_INFO();
							if (ssa->var_info && ssa->ops) {
								zend_ssa_op *ssa_op = &ssa->ops[opline - op_array->opcodes];
								if (ssa_op->op1_use >= 0) {
									zend_ssa_var_info *op1_ssa = ssa->var_info + ssa_op->op1_use;
									if (op1_ssa->ce && !op1_ssa->is_instanceof && !op1_ssa->ce->create_object) {
										ce = op1_ssa->ce;
									}
								}
							}
						}
						if (!(op1_info & MAY_BE_OBJECT)) {
							break;
						}
						if (!zend_jit_fetch_obj_read(&dasm_state, opline, op_array,
								op1_info, ce,
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_BIND_GLOBAL:
						while (1) {
							if (!ssa->ops || !ssa->var_info) {
								op1_info = MAY_BE_ANY|MAY_BE_REF;
							} else {
								op1_info = OP1_INFO();
							}
							if (!zend_jit_bind_global(&dasm_state, opline, op_array, op1_info)) {
								goto jit_failure;
							}
							if ((opline+1)->opcode == ZEND_BIND_GLOBAL) {
								opline++;
							} else {
								break;
							}
						}
						goto done;
					case ZEND_RECV:
						if (!zend_jit_recv(&dasm_state, opline, op_array)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_RECV_INIT:
						while (1) {
							if (!zend_jit_recv_init(&dasm_state, opline, op_array,
									(opline + 1)->opcode != ZEND_RECV_INIT,
									zend_may_throw(opline, op_array, ssa))) {
								goto jit_failure;
							}
							if ((opline+1)->opcode == ZEND_RECV_INIT) {
								opline++;
							} else {
								break;
							}
						}
						goto done;
					case ZEND_FREE:
					case ZEND_FE_FREE:
						op1_info = OP1_INFO();
						USE_OP1_TRACE_TYPE();
						if (!zend_jit_free(&dasm_state, opline, op_array, op1_info,
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ECHO:
						if (opline->op1_type != IS_CONST
						 || Z_TYPE_P(RT_CONSTANT(opline, opline->op1)) != IS_STRING) {
							break;
						}
						if (!zend_jit_echo(&dasm_state, opline, op_array)) {
							goto jit_failure;
						}
						goto done;
#if 0
					case ZEND_SWITCH_LONG:
					case ZEND_SWITCH_STRING:
						if (!zend_jit_switch(&dasm_state, opline, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
#endif
					default:
						break;
				}
			}

			if (opline->opcode != ZEND_NOP && opline->opcode != ZEND_JMP) {
				if (!zend_jit_trace_handler(&dasm_state, op_array, opline, zend_may_throw(opline, op_array, ssa), p + 1)) {
					goto jit_failure;
				}
			}
done:
			/* TODO: reset types on abstract stack,
			   this should be replaced by type inference */
			if (opline->result_type & (IS_TMP_VAR|IS_VAR|IS_CV)) {
				if (opline->opcode == ZEND_QM_ASSIGN) {
					if (opline->op1_type == IS_CONST) {
						SET_RES_STACK_VAR_TYPE(Z_TYPE_P(RT_CONSTANT(opline, opline->op1)));
					} else {
						zend_uchar type = STACK_VAR_TYPE(opline, op1);
						if (type != IS_UNKNOWN) {
							SET_RES_STACK_VAR_TYPE(type);
						} else {
							RESET_RES_STACK_VAR_TYPE();
						}
					}
				} else if (opline->opcode == ZEND_ASSIGN) {
					if (opline->op2_type == IS_CONST) {
						SET_RES_STACK_VAR_TYPE(Z_TYPE_P(RT_CONSTANT(opline, opline->op2)));
					} else {
						zend_uchar type = STACK_VAR_TYPE(opline, op2);
						if (type != IS_UNKNOWN) {
							SET_RES_STACK_VAR_TYPE(type);
						} else {
							RESET_RES_STACK_VAR_TYPE();
						}
					}
				} else if ((opline->opcode == ZEND_POST_INC
				         || opline->opcode == ZEND_POST_DEC)
				        && p->op == ZEND_JIT_TRACE_VM) {
					zend_uchar type = p->op1_type;
					if (type & IS_TRACE_REFERENCE) {
						type = IS_UNKNOWN;
					}
					if (type != IS_UNKNOWN) {
						SET_RES_STACK_VAR_TYPE(type);
					} else {
						RESET_RES_STACK_VAR_TYPE();
					}
				} else {
					RESET_RES_STACK_VAR_TYPE();
				}
			}
			if (opline->op1_type == IS_CV) {
				if (opline->opcode == ZEND_ASSIGN) {
					if (!(OP1_INFO() & MAY_BE_REF)
					 || STACK_VAR_TYPE(opline, op1) != IS_UNKNOWN) {
						if (opline->op2_type == IS_CONST) {
							SET_OP1_STACK_VAR_TYPE(Z_TYPE_P(RT_CONSTANT(opline, opline->op2)));
						} else {
							zend_uchar type = STACK_VAR_TYPE(opline, op2);
							if (type != IS_UNKNOWN) {
								SET_OP1_STACK_VAR_TYPE(type);
							} else {
								RESET_OP1_STACK_VAR_TYPE();
							}
						}
					} else {
						RESET_OP1_STACK_VAR_TYPE();
					}
				} else if (opline->opcode == ZEND_ASSIGN_REF
				 || opline->opcode == ZEND_ASSIGN_DIM
				 || opline->opcode == ZEND_ASSIGN_OBJ
				 || opline->opcode == ZEND_ASSIGN_STATIC_PROP
				 || opline->opcode == ZEND_ASSIGN_OBJ_REF
				 || opline->opcode == ZEND_ASSIGN_STATIC_PROP_REF
				 || opline->opcode == ZEND_ASSIGN_OP
				 || opline->opcode == ZEND_ASSIGN_DIM_OP
				 || opline->opcode == ZEND_ASSIGN_OBJ_OP
				 || opline->opcode == ZEND_ASSIGN_STATIC_PROP_OP
				 || opline->opcode == ZEND_PRE_INC
				 || opline->opcode == ZEND_PRE_DEC
				 || opline->opcode == ZEND_POST_INC
				 || opline->opcode == ZEND_POST_DEC
				 || opline->opcode == ZEND_PRE_INC_OBJ
				 || opline->opcode == ZEND_PRE_DEC_OBJ
				 || opline->opcode == ZEND_POST_INC_OBJ
				 || opline->opcode == ZEND_POST_DEC_OBJ
				 || opline->opcode == ZEND_UNSET_CV
				 || opline->opcode == ZEND_UNSET_DIM
				 || opline->opcode == ZEND_UNSET_OBJ
				 || opline->opcode == ZEND_FETCH_DIM_W
				 || opline->opcode == ZEND_FETCH_DIM_RW
				 || opline->opcode == ZEND_FETCH_DIM_FUNC_ARG
				 || opline->opcode == ZEND_FETCH_DIM_UNSET
				 || opline->opcode == ZEND_FETCH_LIST_W
				 || opline->opcode == ZEND_SEND_VAR_EX
				 || opline->opcode == ZEND_SEND_FUNC_ARG
				 || opline->opcode == ZEND_SEND_REF
				 || opline->opcode == ZEND_SEND_UNPACK
				 || opline->opcode == ZEND_FE_RESET_RW
				 || opline->opcode == ZEND_MAKE_REF
				 || opline->opcode == ZEND_BIND_GLOBAL
				 || opline->opcode == ZEND_BIND_STATIC) {
					RESET_OP1_STACK_VAR_TYPE();
				} else if (opline->opcode == ZEND_ADD_ARRAY_ELEMENT
				 || opline->opcode == ZEND_INIT_ARRAY) {
					if (opline->extended_value & ZEND_ARRAY_ELEMENT_REF) {
						RESET_OP1_STACK_VAR_TYPE();
					}
				}
			}
			if (opline->op2_type == IS_CV) {
				if (opline->opcode == ZEND_ASSIGN_REF) {
					RESET_OP2_STACK_VAR_TYPE();
				} else if (opline->opcode == ZEND_BIND_LEXICAL) {
					if (opline->extended_value & ZEND_BIND_REF) {
						RESET_OP2_STACK_VAR_TYPE();
					}
				}
			}
#if 0
			// TODO: call level calculation doesn't work for traces ???
			switch (opline->opcode) {
				case ZEND_DO_FCALL:
				case ZEND_DO_ICALL:
				case ZEND_DO_UCALL:
				case ZEND_DO_FCALL_BY_NAME:
					call_level--;
			}
#endif
		} else if (p->op == ZEND_JIT_TRACE_ENTER) {
			op_array = (zend_op_array*)p->op_array;
			jit_extension =
				(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
			ssa = &jit_extension->func_info.ssa;
			if (ssa->cfg.blocks_count) {
				/* pass */
			} else if (num_op_arrays == ZEND_JIT_TRACE_MAX_FUNCS) {
				/* Too many functions in single trace */
				goto jit_failure;
			} else {
				/* Remember op_array to cleanup */
				op_arrays[num_op_arrays++] = op_array;

				/* Build SSA */
				ssa = zend_jit_trace_build_ssa(op_array, script);
				if (!ssa) {
					goto jit_failure;
				}
			}
			call = frame->call;
			if (!call) {
				/* Trace missed INIT_FCALL opcode */
				call = top;
				call->call = NULL;
				call->prev = NULL;
				call->func = (const zend_function*)op_array;
				top = zend_jit_trace_call_frame(top, op_array);
				for (i = 0; i < op_array->last_var + op_array->T; i++) {
					call->stack[i] = IS_UNKNOWN;
				}
			} else {
				ZEND_ASSERT(&call->func->op_array == op_array);
			}
			frame->call = call->prev;
			call->prev = frame;
			call->return_value_used = p->return_value_used;
			JIT_G(current_frame) = frame = call;
			stack = frame->stack;
			zend_jit_set_opline(&dasm_state, (p+1)->opline);
		} else if (p->op == ZEND_JIT_TRACE_BACK) {
			op_array = (zend_op_array*)p->op_array;
			jit_extension =
				(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
			ssa = &jit_extension->func_info.ssa;
			if (ssa->cfg.blocks_count) {
				/* pass */
			} else if (num_op_arrays == ZEND_JIT_TRACE_MAX_FUNCS) {
				/* Too many functions in single trace */
				goto jit_failure;
			} else {
				/* Remember op_array to cleanup */
				op_arrays[num_op_arrays++] = op_array;

				/* Build SSA */
				ssa = zend_jit_trace_build_ssa(op_array, script);
				if (!ssa) {
					goto jit_failure;
				}
			}
			top = frame;
			if (frame->prev) {
				frame = frame->prev;
				stack = frame->stack;
				ZEND_ASSERT(&frame->func->op_array == op_array);
			} else {
				frame = zend_jit_trace_ret_frame(frame, op_array);
				frame->call = NULL;
				frame->prev = NULL;
				frame->func = (const zend_function*)op_array;
				frame->return_value_used = -1;
				stack = frame->stack;
				for (i = 0; i < op_array->last_var + op_array->T; i++) {
					stack[i] = IS_UNKNOWN;
				}
			}
			JIT_G(current_frame) = frame;
			if (res_type != IS_UNKNOWN
			 && (p+1)->op == ZEND_JIT_TRACE_VM) {
				const zend_op *opline = (p+1)->opline - 1;
				if (opline->result_type != IS_UNUSED) {
				    SET_RES_STACK_VAR_TYPE(res_type);
				}
			}
			res_type = IS_UNKNOWN;
		} else if (p->op == ZEND_JIT_TRACE_END) {
			break;
		} else if (p->op == ZEND_JIT_TRACE_INIT_CALL) {
			call = top;
			call->call = NULL;
			call->prev = frame->call;
			call->func = p->func;
			frame->call = call;
			if (p->func->type == ZEND_USER_FUNCTION) {
				top = zend_jit_trace_call_frame(top, p->op_array);
				for (i = 0; i < p->op_array->last_var +p-> op_array->T; i++) {
					call->stack[i] = IS_UNKNOWN;
				}
			} else {
				top++;
			}
		} else if (p->op == ZEND_JIT_TRACE_DO_ICALL) {
			call = frame->call;
			if (call) {
				top = call;
				frame->call = call->prev;
			}
		} else {
			ZEND_ASSERT(0);
		}
	}

	ZEND_ASSERT(p->op == ZEND_JIT_TRACE_END);

	t = &zend_jit_traces[ZEND_JIT_TRACE_NUM];

	if (p->stop == ZEND_JIT_TRACE_STOP_LOOP
	 || p->stop == ZEND_JIT_TRACE_STOP_RECURSIVE_CALL
	 || p->stop == ZEND_JIT_TRACE_STOP_RECURSIVE_RET) {
		if (p->stop == ZEND_JIT_TRACE_STOP_LOOP) {
			if (!zend_jit_set_valid_ip(&dasm_state, p->opline)) {
				goto jit_failure;
			}
		}
		t->link = ZEND_JIT_TRACE_NUM;
		zend_jit_jmp(&dasm_state, 0); /* jump back to start of the trace loop */
	} else if (p->stop == ZEND_JIT_TRACE_STOP_LINK) {
		if (!zend_jit_set_valid_ip(&dasm_state, p->opline)) {
			goto jit_failure;
		}
		t->link = zend_jit_find_trace(p->opline->handler);
		zend_jit_trace_link_to_root(&dasm_state, p->opline->handler);
	} else if (p->stop == ZEND_JIT_TRACE_STOP_RETURN) {
		zend_jit_trace_return(&dasm_state);
	} else {
		// TODO: not implemented ???
		ZEND_ASSERT(0 && p->stop);
	}

	if (ZEND_JIT_EXIT_COUNTERS + t->exit_count >= ZEND_JIT_TRACE_MAX_EXIT_COUNTERS) {
		goto jit_failure;
	}

	handler = dasm_link_and_encode(&dasm_state, NULL, NULL, NULL, NULL, ZSTR_VAL(name), 1);

jit_failure:
	dasm_free(&dasm_state);

	if (name) {
		zend_string_release(name);
	}

	/* Clenup used op_arrays */
	while (num_op_arrays > 0) {
		op_array = op_arrays[--num_op_arrays];
		jit_extension =
			(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);

		memset(&jit_extension->func_info, 0, sizeof(jit_extension->func_info));
		jit_extension->func_info.num_args = -1;
		jit_extension->func_info.return_value_used = -1;
	}

	zend_arena_release(&CG(arena), checkpoint);

	JIT_G(current_frame) = NULL;

	return handler;
}

static int zend_jit_trace_exit_needs_deoptimization(uint32_t trace_num, uint32_t exit_num)
{
	const zend_op *opline = zend_jit_traces[trace_num].exit_info[exit_num].opline;

	opline = (const zend_op*)((uintptr_t)opline & ~(ZEND_JIT_EXIT_JITED|ZEND_JIT_EXIT_BLACKLISTED));
	if (opline) {
		return 1;
	}

	return 0;
}

static const void *zend_jit_trace_exit_to_vm(uint32_t trace_num, uint32_t exit_num)
{
	const void *handler = NULL;
	dasm_State* dasm_state = NULL;
	void *checkpoint;
	char name[32];
	const zend_op *opline;

	if (!zend_jit_trace_exit_needs_deoptimization(trace_num, exit_num)) {
		return dasm_labels[zend_lbtrace_escape];
	}

	checkpoint = zend_arena_checkpoint(CG(arena));;

	sprintf(name, "ESCAPE-%d-%d", trace_num, exit_num);

	dasm_init(&dasm_state, DASM_MAXSECTION);
	dasm_setupglobal(&dasm_state, dasm_labels, zend_lb_MAX);
	dasm_setup(&dasm_state, dasm_actions);
	dasm_growpc(&dasm_state, 0);

	zend_jit_align_func(&dasm_state);

	// TODO: Generate deoptimization code ???

	opline = zend_jit_traces[trace_num].exit_info[exit_num].opline;
	opline = (const zend_op*)((uintptr_t)opline & ~(ZEND_JIT_EXIT_JITED|ZEND_JIT_EXIT_BLACKLISTED));
	if (opline) {
		zend_jit_set_ip(&dasm_state, opline);
	}

	zend_jit_trace_return(&dasm_state);

	handler = dasm_link_and_encode(&dasm_state, NULL, NULL, NULL, NULL, name, 1);

	dasm_free(&dasm_state);

	zend_arena_release(&CG(arena), checkpoint);

	return handler;
}

static zend_jit_trace_stop zend_jit_compile_root_trace(zend_jit_trace_rec *trace_buffer, const zend_op *opline, size_t offset)
{
	zend_jit_trace_stop ret;
	const void *handler;
	zend_jit_trace_info *t;
	zend_jit_trace_exit_info exit_info[ZEND_JIT_TRACE_MAX_EXITS];

	zend_shared_alloc_lock();

	/* Checks under lock */
	if ((ZEND_OP_TRACE_INFO(opline, offset)->trace_flags & ZEND_JIT_TRACE_JITED)) {
		ret = ZEND_JIT_TRACE_STOP_ALREADY_DONE;
	} else if (ZEND_JIT_TRACE_NUM >= ZEND_JIT_TRACE_MAX_TRACES) {
		ret = ZEND_JIT_TRACE_STOP_TOO_MANY_TRACES;
	} else {
		SHM_UNPROTECT();
		zend_jit_unprotect();

		t = &zend_jit_traces[ZEND_JIT_TRACE_NUM];

		t->id = ZEND_JIT_TRACE_NUM;
		t->root = ZEND_JIT_TRACE_NUM;
		t->parent = 0;
		t->link = 0;
		t->exit_count = 0;
		t->child_count = 0;
		t->stack_map_size = 0;
		t->exit_info = exit_info;
		t->stack_map = NULL;

		handler = zend_jit_trace(trace_buffer, 0, 0);

		if (handler) {
			zend_jit_trace_exit_info *shared_exit_info = NULL;

			t->exit_info = NULL;
			if (t->exit_count) {
				/* reallocate exit_info into shared memory */
				shared_exit_info = (zend_jit_trace_exit_info*)zend_shared_alloc(
					sizeof(zend_jit_trace_exit_info) * t->exit_count);

				if (!shared_exit_info) {
				    if (t->stack_map) {
						efree(t->stack_map);
						t->stack_map = NULL;
					}
					ret = ZEND_JIT_TRACE_STOP_NO_SHM;
					goto exit;
				}
				memcpy(shared_exit_info, exit_info,
					sizeof(zend_jit_trace_exit_info) * t->exit_count);
				t->exit_info = shared_exit_info;
			}

		    if (t->stack_map_size) {
				uint8_t *shared_stack_map = (uint8_t*)zend_shared_alloc(t->stack_map_size);
				if (!shared_stack_map) {
					ret = ZEND_JIT_TRACE_STOP_NO_SHM;
					goto exit;
				}
				memcpy(shared_stack_map, t->stack_map, t->stack_map_size);
				efree(t->stack_map);
				t->stack_map = shared_stack_map;
		    }

			t->exit_counters = ZEND_JIT_EXIT_COUNTERS;
			ZEND_JIT_EXIT_COUNTERS += t->exit_count;

			((zend_op*)opline)->handler = handler;

			ZEND_JIT_TRACE_NUM++;
			ZEND_OP_TRACE_INFO(opline, offset)->trace_flags |= ZEND_JIT_TRACE_JITED;

			ret = ZEND_JIT_TRACE_STOP_COMPILED;
		} else if (t->exit_count >= ZEND_JIT_TRACE_MAX_EXITS ||
		           ZEND_JIT_EXIT_COUNTERS + t->exit_count >= ZEND_JIT_TRACE_MAX_EXIT_COUNTERS) {
		    if (t->stack_map) {
				efree(t->stack_map);
				t->stack_map = NULL;
			}
			ret = ZEND_JIT_TRACE_STOP_TOO_MANY_EXITS;
		} else {
		    if (t->stack_map) {
				efree(t->stack_map);
				t->stack_map = NULL;
			}
			ret = ZEND_JIT_TRACE_STOP_COMPILER_ERROR;
		}

exit:
		zend_jit_protect();
		SHM_PROTECT();
	}

	zend_shared_alloc_unlock();

	return ret;
}

static void zend_jit_blacklist_root_trace(const zend_op *opline, size_t offset)
{
	zend_shared_alloc_lock();

	if (!(ZEND_OP_TRACE_INFO(opline, offset)->trace_flags & ZEND_JIT_TRACE_BLACKLISTED)) {
		SHM_UNPROTECT();
		zend_jit_unprotect();

		((zend_op*)opline)->handler =
			ZEND_OP_TRACE_INFO(opline, offset)->orig_handler;

		ZEND_OP_TRACE_INFO(opline, offset)->trace_flags |= ZEND_JIT_TRACE_BLACKLISTED;

		zend_jit_protect();
		SHM_PROTECT();
	}

	zend_shared_alloc_unlock();
}

static zend_bool zend_jit_trace_is_bad_root(const zend_op *opline, zend_jit_trace_stop stop, size_t offset)
{
	const zend_op **cache_opline = JIT_G(bad_root_cache_opline);
	uint8_t *cache_count = JIT_G(bad_root_cache_count);
	uint8_t *cache_stop = JIT_G(bad_root_cache_stop);
	uint32_t cache_slot = JIT_G(bad_root_slot);
	uint32_t i;

	for (i = 0; i < ZEND_JIT_TRACE_BAD_ROOT_SLOTS; i++) {
		if (cache_opline[i] == opline) {
			if (cache_count[i] >= ZEND_JIT_TRACE_MAX_ROOT_FAILURES - 1) {
				cache_opline[i] = NULL;
				return 1;
			} else {
#if 0
				if (ZEND_OP_TRACE_INFO(opline, offset)->counter) {
					*ZEND_OP_TRACE_INFO(opline, offset)->counter =
						random() % ZEND_JIT_TRACE_COUNTER_MAX;
				}
#endif
				cache_count[i]++;
				cache_stop[i] = stop;
				return 0;
			}
		}
	}
	i = cache_slot;
	cache_opline[i] = opline;
	cache_count[i] = 1;
	cache_stop[i] = stop;
	cache_slot = (i + 1) % ZEND_JIT_TRACE_BAD_ROOT_SLOTS;
	JIT_G(bad_root_slot) = cache_slot;
	return 0;
}

#define ZEND_JIT_TRACE_STOP_DESCRIPTION(name, description) \
	description,

static const char * zend_jit_trace_stop_description[] = {
	ZEND_JIT_TRACE_STOP(ZEND_JIT_TRACE_STOP_DESCRIPTION)
};

static void zend_jit_dump_trace(zend_jit_trace_rec *trace_buffer)
{
	zend_jit_trace_rec *p = trace_buffer;
	const zend_op_array *op_array;
	uint32_t level = 1 + trace_buffer[0].level;

	ZEND_ASSERT(p->op == ZEND_JIT_TRACE_START);
	op_array = p->op_array;
	p += ZEND_JIT_TRACE_START_REC_SIZE;

	while (1) {
		if (p->op == ZEND_JIT_TRACE_VM) {
			uint8_t op1_type, op2_type, op3_type;

			fprintf(stderr, "%04d%*c",
				(int)(p->opline - op_array->opcodes),
				level, ' ');
			zend_dump_op(op_array, NULL, p->opline, ZEND_DUMP_NUMERIC_OPLINES, NULL);

			op1_type = p->op1_type;
			op2_type = p->op2_type;
			op3_type = p->op3_type;
			if (op1_type != IS_UNKNOWN || op2_type != IS_UNKNOWN || op3_type != IS_UNKNOWN) {
				fprintf(stderr, " ;");
				if (op1_type != IS_UNKNOWN) {
					const char *ref = (op1_type & IS_TRACE_REFERENCE) ? "&" : "";
					if ((p+1)->op == ZEND_JIT_TRACE_OP1_TYPE) {
						p++;
						fprintf(stderr, " op1(%sobject of class %s)", ref,
							ZSTR_VAL(p->ce->name));
					} else {
						const char *type = (op1_type == 0) ? "undef" : zend_get_type_by_const(op1_type & ~IS_TRACE_REFERENCE);
						fprintf(stderr, " op1(%s%s)", ref, type);
					}
				}
				if (op2_type != IS_UNKNOWN) {
					const char *ref = (op2_type & IS_TRACE_REFERENCE) ? "&" : "";
					if ((p+1)->op == ZEND_JIT_TRACE_OP2_TYPE) {
						p++;
						fprintf(stderr, " op2(%sobject of class %s)", ref,
							ZSTR_VAL(p->ce->name));
					} else {
						const char *type = (op2_type == 0) ? "undef" : zend_get_type_by_const(op2_type & ~IS_TRACE_REFERENCE);
						fprintf(stderr, " op2(%s%s)", ref, type);
					}
				}
				if (op3_type != IS_UNKNOWN) {
					const char *ref = (op3_type & IS_TRACE_REFERENCE) ? "&" : "";
					const char *type = (op3_type == 0) ? "undef" : zend_get_type_by_const(op3_type & ~IS_TRACE_REFERENCE);
					fprintf(stderr, " op3(%s%s)", ref, type);
				}
			}
			fprintf(stderr, "\n");

			if ((p->opline->result_type & (IS_SMART_BRANCH_JMPNZ | IS_SMART_BRANCH_JMPZ)) != 0
			 || p->opline->opcode == ZEND_ASSIGN_DIM
			 || p->opline->opcode == ZEND_ASSIGN_OBJ
			 || p->opline->opcode == ZEND_ASSIGN_STATIC_PROP
			 || p->opline->opcode == ZEND_ASSIGN_DIM_OP
			 || p->opline->opcode == ZEND_ASSIGN_OBJ_OP
			 || p->opline->opcode == ZEND_ASSIGN_STATIC_PROP_OP
			 || p->opline->opcode == ZEND_ASSIGN_OBJ_REF
			 || p->opline->opcode == ZEND_ASSIGN_STATIC_PROP_REF) {
				fprintf(stderr, "%04d%*c;",
					(int)((p->opline + 1) - op_array->opcodes),
					level, ' ');
				zend_dump_op(op_array, NULL, (p->opline + 1), ZEND_DUMP_NUMERIC_OPLINES, NULL);
				fprintf(stderr, "\n");
			}
		} else if (p->op == ZEND_JIT_TRACE_ENTER) {
			op_array = p->op_array;
			fprintf(stderr, "    %*c>enter %s%s%s\n",
				level, ' ',
				op_array->scope ? ZSTR_VAL(op_array->scope->name) : "",
				op_array->scope ? "::" : "",
				op_array->function_name ?
					ZSTR_VAL(op_array->function_name) :
					ZSTR_VAL(op_array->filename));
			level++;
		} else if (p->op == ZEND_JIT_TRACE_BACK) {
			op_array = p->op_array;
			level--;
			fprintf(stderr, "    %*c<back %s%s%s\n",
				level, ' ',
				op_array->scope ? ZSTR_VAL(op_array->scope->name) : "",
				op_array->scope ? "::" : "",
				op_array->function_name ?
					ZSTR_VAL(op_array->function_name) :
					ZSTR_VAL(op_array->filename));
		} else if (p->op == ZEND_JIT_TRACE_INIT_CALL) {
			if (p->func != (zend_function*)&zend_pass_function) {
				fprintf(stderr, p->fake ? "    %*c>fake_init %s%s%s\n" : "    %*c>init %s%s%s\n",
					level, ' ',
					p->func->common.scope ? ZSTR_VAL(p->func->common.scope->name) : "",
					p->func->common.scope ? "::" : "",
					ZSTR_VAL(p->func->common.function_name));
			} else {
				fprintf(stderr, "    %*c>skip\n",
					level, ' ');
			}
		} else if (p->op == ZEND_JIT_TRACE_DO_ICALL) {
			if (p->func != (zend_function*)&zend_pass_function) {
				fprintf(stderr, "    %*c>call %s%s%s\n",
					level, ' ',
					p->func->common.scope ? ZSTR_VAL(p->func->common.scope->name) : "",
					p->func->common.scope ? "::" : "",
					ZSTR_VAL(p->func->common.function_name));
			} else {
				fprintf(stderr, "    %*c>skip\n",
					level, ' ');
			}
		} else if (p->op == ZEND_JIT_TRACE_END) {
			break;
		}
		p++;
	}
}

static zend_always_inline const char *zend_jit_trace_star_desc(uint8_t trace_flags)
{
	if (trace_flags & ZEND_JIT_TRACE_START_LOOP) {
		return "loop";
	} else if (trace_flags & ZEND_JIT_TRACE_START_ENTER) {
		return "enter";
	} else if (trace_flags & ZEND_JIT_TRACE_START_RETURN) {
		return "return";
	} else {
		ZEND_ASSERT(0);
		return "???";
	}
}

int ZEND_FASTCALL zend_jit_trace_hot_root(zend_execute_data *execute_data, const zend_op *opline)
{
	const zend_op *orig_opline;
	zend_jit_trace_stop stop;
	zend_op_array *op_array;
	zend_jit_op_array_trace_extension *jit_extension;
	size_t offset;
	uint32_t trace_num;
	zend_jit_trace_rec trace_buffer[ZEND_JIT_TRACE_MAX_LENGTH];

	ZEND_ASSERT(EX(func)->type == ZEND_USER_FUNCTION);
	ZEND_ASSERT(opline >= EX(func)->op_array.opcodes &&
		opline < EX(func)->op_array.opcodes + EX(func)->op_array.last);

repeat:
	trace_num = ZEND_JIT_TRACE_NUM;
	orig_opline = opline;
	op_array = &EX(func)->op_array;
	jit_extension = (zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
	offset = jit_extension->offset;

	EX(opline) = opline;

	/* Lock-free check if the root trace was already JIT-ed or blacklist-ed in another process */
	if (ZEND_OP_TRACE_INFO(opline, offset)->trace_flags & (ZEND_JIT_TRACE_JITED|ZEND_JIT_TRACE_BLACKLISTED)) {
		return 0;
	}

	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_TRACE_START) {
		fprintf(stderr, "---- TRACE %d start (%s) %s() %s:%d\n",
			trace_num,
			zend_jit_trace_star_desc(ZEND_OP_TRACE_INFO(opline, offset)->trace_flags),
			EX(func)->op_array.function_name ?
				ZSTR_VAL(EX(func)->op_array.function_name) : "$main",
			ZSTR_VAL(EX(func)->op_array.filename),
			opline->lineno);
	}

	if (ZEND_JIT_TRACE_NUM >= ZEND_JIT_TRACE_MAX_TRACES) {
		stop = ZEND_JIT_TRACE_STOP_TOO_MANY_TRACES;
		goto abort;
	}

	stop = zend_jit_trace_execute(execute_data, opline, trace_buffer,
		ZEND_OP_TRACE_INFO(opline, offset)->trace_flags & ZEND_JIT_TRACE_START_MASK);

	if (stop == ZEND_JIT_TRACE_STOP_TOPLEVEL) {
		/* op_array may be already deallocated */
		if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_TRACE_ABORT) {
			fprintf(stderr, "---- TRACE %d abort (%s)\n",
				trace_num,
				zend_jit_trace_stop_description[stop]);
		}
		goto blacklist;
	}

	if (UNEXPECTED(ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_TRACE_BYTECODE)) {
		zend_jit_dump_trace(trace_buffer);
	}

	if (ZEND_JIT_TRACE_STOP_OK(stop)) {
		if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_TRACE_STOP) {
			if (stop == ZEND_JIT_TRACE_STOP_LINK) {
				uint32_t link_to = zend_jit_find_trace(EG(current_execute_data)->opline->handler);;
				fprintf(stderr, "---- TRACE %d stop (link to %d)\n",
					trace_num,
					link_to);
			} else {
				fprintf(stderr, "---- TRACE %d stop (%s)\n",
					trace_num,
					zend_jit_trace_stop_description[stop]);
			}
		}
		stop = zend_jit_compile_root_trace(trace_buffer, orig_opline, offset);
		if (EXPECTED(ZEND_JIT_TRACE_STOP_DONE(stop))) {
			if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_TRACE_COMPILED) {
				fprintf(stderr, "---- TRACE %d %s\n",
					trace_num,
					zend_jit_trace_stop_description[stop]);
			}
		} else {
			goto abort;
		}
	} else {
abort:
		if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_TRACE_ABORT) {
			fprintf(stderr, "---- TRACE %d abort (%s)\n",
				trace_num,
				zend_jit_trace_stop_description[stop]);
		}
blacklist:
		if (!ZEND_JIT_TRACE_STOP_MAY_RECOVER(stop)
		 || zend_jit_trace_is_bad_root(orig_opline, stop, offset)) {
			if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_TRACE_BLACKLIST) {
				fprintf(stderr, "---- TRACE %d blacklisted\n",
					trace_num);
			}
			zend_jit_blacklist_root_trace(orig_opline, offset);
		}
		if (ZEND_JIT_TRACE_STOP_REPEAT(stop)) {
			execute_data = EG(current_execute_data);
			opline = EX(opline);
			goto repeat;
		}
	}

	if (ZCG(accel_directives).jit_debug & (ZEND_JIT_DEBUG_TRACE_STOP|ZEND_JIT_DEBUG_TRACE_ABORT|ZEND_JIT_DEBUG_TRACE_COMPILED|ZEND_JIT_DEBUG_TRACE_BLACKLIST)) {
		fprintf(stderr, "\n");
	}

	return (stop == ZEND_JIT_TRACE_STOP_HALT) ? -1 : 0;
}

static void zend_jit_blacklist_trace_exit(uint32_t trace_num, uint32_t exit_num)
{
	const void *handler;

	zend_shared_alloc_lock();

	if (!((uintptr_t)zend_jit_traces[trace_num].exit_info[exit_num].opline & ZEND_JIT_EXIT_BLACKLISTED)) {
		SHM_UNPROTECT();
		zend_jit_unprotect();

		handler = zend_jit_trace_exit_to_vm(trace_num, exit_num);

		if (handler) {
			zend_jit_link_side_trace(
				zend_jit_traces[trace_num].code_start,
				zend_jit_traces[trace_num].code_size,
				exit_num,
				handler);
		}

		zend_jit_traces[trace_num].exit_info[exit_num].opline = (const zend_op*)
		    ((uintptr_t)zend_jit_traces[trace_num].exit_info[exit_num].opline | ZEND_JIT_EXIT_BLACKLISTED);

		zend_jit_protect();
		SHM_PROTECT();
	}

	zend_shared_alloc_unlock();
}

static zend_bool zend_jit_trace_exit_is_bad(uint32_t trace_num, uint32_t exit_num)
{
	uint8_t *counter = JIT_G(exit_counters) +
		zend_jit_traces[trace_num].exit_counters + exit_num;

	if (*counter + 1 >= ZEND_JIT_TRACE_HOT_SIDE_COUNT + ZEND_JIT_TRACE_MAX_SIDE_FAILURES) {
		return 1;
	}
	(*counter)++;
	return 0;
}

static zend_bool zend_jit_trace_exit_is_hot(uint32_t trace_num, uint32_t exit_num)
{
	uint8_t *counter = JIT_G(exit_counters) +
		zend_jit_traces[trace_num].exit_counters + exit_num;

	if (*counter + 1 >= ZEND_JIT_TRACE_HOT_SIDE_COUNT) {
		return 1;
	}
	(*counter)++;
	return 0;
}

static zend_jit_trace_stop zend_jit_compile_side_trace(zend_jit_trace_rec *trace_buffer, uint32_t parent_num, uint32_t exit_num)
{
	zend_jit_trace_stop ret;
	const void *handler;
	zend_jit_trace_info *t;
	zend_jit_trace_exit_info exit_info[ZEND_JIT_TRACE_MAX_EXITS];

	zend_shared_alloc_lock();

	/* Checks under lock */
	if (((uintptr_t)zend_jit_traces[parent_num].exit_info[exit_num].opline & ZEND_JIT_EXIT_JITED)) {
		ret = ZEND_JIT_TRACE_STOP_ALREADY_DONE;
	} else if (ZEND_JIT_TRACE_NUM >= ZEND_JIT_TRACE_MAX_TRACES) {
		ret = ZEND_JIT_TRACE_STOP_TOO_MANY_TRACES;
	} else if (zend_jit_traces[zend_jit_traces[parent_num].root].child_count >= ZEND_JIT_TRACE_MAX_SIDE_TRACES) {
		ret = ZEND_JIT_TRACE_STOP_TOO_MANY_CHILDREN;
	} else {
		SHM_UNPROTECT();
		zend_jit_unprotect();

		t = &zend_jit_traces[ZEND_JIT_TRACE_NUM];

		t->id = ZEND_JIT_TRACE_NUM;
		t->root = zend_jit_traces[parent_num].root;
		t->parent = parent_num;
		t->link = 0;
		t->exit_count = 0;
		t->child_count = 0;
		t->stack_map_size = 0;
		t->exit_info = exit_info;
		t->stack_map = NULL;

		handler = zend_jit_trace(trace_buffer, parent_num, exit_num);

		if (handler) {
			zend_jit_trace_exit_info *shared_exit_info = NULL;

			t->exit_info = NULL;
			if (t->exit_count) {
				/* reallocate exit_info into shared memory */
				shared_exit_info = (zend_jit_trace_exit_info*)zend_shared_alloc(
					sizeof(zend_jit_trace_exit_info) * t->exit_count);

				if (!shared_exit_info) {
				    if (t->stack_map) {
						efree(t->stack_map);
						t->stack_map = NULL;
					}
					ret = ZEND_JIT_TRACE_STOP_NO_SHM;
					goto exit;
				}
				memcpy(shared_exit_info, exit_info,
					sizeof(zend_jit_trace_exit_info) * t->exit_count);
				t->exit_info = shared_exit_info;
			}

		    if (t->stack_map_size) {
				uint8_t *shared_stack_map = (uint8_t*)zend_shared_alloc(t->stack_map_size);
				if (!shared_stack_map) {
					efree(t->stack_map);
					ret = ZEND_JIT_TRACE_STOP_NO_SHM;
					goto exit;
				}
				memcpy(shared_stack_map, t->stack_map, t->stack_map_size);
				efree(t->stack_map);
				t->stack_map = shared_stack_map;
		    }

			zend_jit_link_side_trace(
				zend_jit_traces[parent_num].code_start,
				zend_jit_traces[parent_num].code_size,
				exit_num,
				handler);

			t->exit_counters = ZEND_JIT_EXIT_COUNTERS;
			ZEND_JIT_EXIT_COUNTERS += t->exit_count;

			zend_jit_traces[zend_jit_traces[parent_num].root].child_count++;
			ZEND_JIT_TRACE_NUM++;
			zend_jit_traces[parent_num].exit_info[exit_num].opline = (const zend_op*)
			    ((uintptr_t)zend_jit_traces[parent_num].exit_info[exit_num].opline | ZEND_JIT_EXIT_JITED);

			ret = ZEND_JIT_TRACE_STOP_COMPILED;
		} else if (t->exit_count >= ZEND_JIT_TRACE_MAX_EXITS ||
		           ZEND_JIT_EXIT_COUNTERS + t->exit_count >= ZEND_JIT_TRACE_MAX_EXIT_COUNTERS) {
		    if (t->stack_map) {
				efree(t->stack_map);
				t->stack_map = NULL;
			}
			ret = ZEND_JIT_TRACE_STOP_TOO_MANY_EXITS;
		} else {
		    if (t->stack_map) {
				efree(t->stack_map);
				t->stack_map = NULL;
			}
			ret = ZEND_JIT_TRACE_STOP_COMPILER_ERROR;
		}

exit:
		zend_jit_protect();
		SHM_PROTECT();
	}

	zend_shared_alloc_unlock();

	return ret;
}

int ZEND_FASTCALL zend_jit_trace_hot_side(zend_execute_data *execute_data, uint32_t parent_num, uint32_t exit_num)
{
	zend_jit_trace_stop stop;
	uint32_t trace_num;
	zend_jit_trace_rec trace_buffer[ZEND_JIT_TRACE_MAX_LENGTH];

	trace_num = ZEND_JIT_TRACE_NUM;

	/* Lock-free check if the side trace was already JIT-ed or blacklist-ed in another process */
	if ((uintptr_t)zend_jit_traces[parent_num].exit_info[exit_num].opline & (ZEND_JIT_EXIT_JITED|ZEND_JIT_EXIT_BLACKLISTED)) {
		return 0;
	}

	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_TRACE_START) {
		fprintf(stderr, "---- TRACE %d start (side trace %d/%d) %s() %s:%d\n",
			trace_num, parent_num, exit_num,
			EX(func)->op_array.function_name ?
				ZSTR_VAL(EX(func)->op_array.function_name) : "$main",
			ZSTR_VAL(EX(func)->op_array.filename),
			EX(opline)->lineno);
	}

	if (ZEND_JIT_TRACE_NUM >= ZEND_JIT_TRACE_MAX_TRACES) {
		stop = ZEND_JIT_TRACE_STOP_TOO_MANY_TRACES;
		goto abort;
	}

	if (zend_jit_traces[zend_jit_traces[parent_num].root].child_count >= ZEND_JIT_TRACE_MAX_SIDE_TRACES) {
		stop = ZEND_JIT_TRACE_STOP_TOO_MANY_CHILDREN;
		goto abort;
	}

	stop = zend_jit_trace_execute(execute_data, EX(opline), trace_buffer, ZEND_JIT_TRACE_START_SIDE);

	if (stop == ZEND_JIT_TRACE_STOP_TOPLEVEL) {
		/* op_array may be already deallocated */
		if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_TRACE_ABORT) {
			fprintf(stderr, "---- TRACE %d abort (%s)\n",
				trace_num,
				zend_jit_trace_stop_description[stop]);
		}
		goto blacklist;
	}

	if (UNEXPECTED(ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_TRACE_BYTECODE)) {
		zend_jit_dump_trace(trace_buffer);
	}

	if (ZEND_JIT_TRACE_STOP_OK(stop)) {
		if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_TRACE_STOP) {
			if (stop == ZEND_JIT_TRACE_STOP_LINK) {
				uint32_t link_to = zend_jit_find_trace(EG(current_execute_data)->opline->handler);;
				fprintf(stderr, "---- TRACE %d stop (link to %d)\n",
					trace_num,
					link_to);
			} else {
				fprintf(stderr, "---- TRACE %d stop (%s)\n",
					trace_num,
					zend_jit_trace_stop_description[stop]);
			}
		}
		if (EXPECTED(stop != ZEND_JIT_TRACE_STOP_LOOP)) {
			stop = zend_jit_compile_side_trace(trace_buffer, parent_num, exit_num);
		} else {
			const zend_op_array *op_array = trace_buffer[0].op_array;
			zend_jit_op_array_trace_extension *jit_extension =
				(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
			const zend_op *opline = trace_buffer[1].opline;

			stop = zend_jit_compile_root_trace(trace_buffer, opline, jit_extension->offset);
		}
		if (EXPECTED(ZEND_JIT_TRACE_STOP_DONE(stop))) {
			if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_TRACE_COMPILED) {
				fprintf(stderr, "---- TRACE %d %s\n",
					trace_num,
					zend_jit_trace_stop_description[stop]);
			}
		} else {
			goto abort;
		}
	} else {
abort:
		if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_TRACE_ABORT) {
			fprintf(stderr, "---- TRACE %d abort (%s)\n",
				trace_num,
				zend_jit_trace_stop_description[stop]);
		}
blacklist:
		if (!ZEND_JIT_TRACE_STOP_MAY_RECOVER(stop)
		 || zend_jit_trace_exit_is_bad(parent_num, exit_num)) {
			zend_jit_blacklist_trace_exit(parent_num, exit_num);
			if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_TRACE_BLACKLIST) {
				fprintf(stderr, "---- EXIT %d/%d blacklisted\n",
					parent_num, exit_num);
			}
		}
	}

	if (ZCG(accel_directives).jit_debug & (ZEND_JIT_DEBUG_TRACE_STOP|ZEND_JIT_DEBUG_TRACE_ABORT|ZEND_JIT_DEBUG_TRACE_COMPILED|ZEND_JIT_DEBUG_TRACE_BLACKLIST)) {
		fprintf(stderr, "\n");
	}

	return (stop == ZEND_JIT_TRACE_STOP_HALT) ? -1 : 0;
}

int ZEND_FASTCALL zend_jit_trace_exit(uint32_t trace_num, uint32_t exit_num)
{
	zend_execute_data *execute_data = EG(current_execute_data);
	const zend_op *opline;
	zend_jit_trace_info *t = &zend_jit_traces[trace_num];

	// TODO: Deoptimizatoion of VM stack state ???

	/* Lock-free check if the side trace was already JIT-ed or blacklist-ed in another process */
	// TODO: We may remoive this, becaus of the same check in zend_jit_trace_hot_side() ???
	opline = t->exit_info[exit_num].opline;
	if ((uintptr_t)opline & (ZEND_JIT_EXIT_JITED|ZEND_JIT_EXIT_BLACKLISTED)) {
		opline = (const zend_op*)((uintptr_t)opline & ~(ZEND_JIT_EXIT_JITED|ZEND_JIT_EXIT_BLACKLISTED));
		if (opline) {
			/* Set VM opline to continue interpretation */
			EX(opline) = opline;
		}
		return 0;
	}

	if (opline) {
		/* Set VM opline to continue interpretation */
		EX(opline) = opline;
	}

	ZEND_ASSERT(EX(func)->type == ZEND_USER_FUNCTION);
	ZEND_ASSERT(EX(opline) >= EX(func)->op_array.opcodes &&
		EX(opline) < EX(func)->op_array.opcodes + EX(func)->op_array.last);

	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_TRACE_EXIT) {
		fprintf(stderr, "     TRACE %d exit %d %s() %s:%d\n",
			trace_num,
			exit_num,
			EX(func)->op_array.function_name ?
				ZSTR_VAL(EX(func)->op_array.function_name) : "$main",
			ZSTR_VAL(EX(func)->op_array.filename),
			EX(opline)->lineno);
	}

	if (zend_jit_trace_exit_is_hot(trace_num, exit_num)) {
		return zend_jit_trace_hot_side(execute_data, trace_num, exit_num);
	}

	return 0;
}

static zend_always_inline uint8_t zend_jit_trace_supported(const zend_op *opline)
{
	switch (opline->opcode) {
		case ZEND_CATCH:
		case ZEND_FAST_CALL:
		case ZEND_FAST_RET:
		case ZEND_GENERATOR_CREATE:
		case ZEND_GENERATOR_RETURN:
		case ZEND_EXIT:
		case ZEND_YIELD:
		case ZEND_YIELD_FROM:
		case ZEND_INCLUDE_OR_EVAL:
			return ZEND_JIT_TRACE_UNSUPPORTED;
		default:
			return ZEND_JIT_TRACE_SUPPORTED;
	}
}

static int zend_jit_setup_hot_trace_counters(zend_op_array *op_array)
{
	zend_op *opline;
	zend_jit_op_array_trace_extension *jit_extension;
	zend_cfg cfg;
	uint32_t i;

	ZEND_ASSERT(zend_jit_func_counter_handler != NULL);
	ZEND_ASSERT(zend_jit_ret_counter_handler != NULL);
	ZEND_ASSERT(zend_jit_loop_counter_handler != NULL);
	ZEND_ASSERT(sizeof(zend_op_trace_info) == sizeof(zend_op));

	if (zend_jit_build_cfg(op_array, &cfg) != SUCCESS) {
		return FAILURE;
	}

	jit_extension = (zend_jit_op_array_trace_extension*)zend_shared_alloc(sizeof(zend_jit_op_array_trace_extension) + (op_array->last - 1) * sizeof(zend_op_trace_info));
	memset(&jit_extension->func_info, 0, sizeof(jit_extension->func_info));
	jit_extension->func_info.num_args = -1;
	jit_extension->func_info.return_value_used = -1;
	jit_extension->offset = (char*)jit_extension->trace_info - (char*)op_array->opcodes;
	for (i = 0; i < op_array->last; i++) {
		jit_extension->trace_info[i].orig_handler = op_array->opcodes[i].handler;
		jit_extension->trace_info[i].call_handler = zend_get_opcode_handler_func(&op_array->opcodes[i]);
		jit_extension->trace_info[i].counter = NULL;
		jit_extension->trace_info[i].trace_flags =
			zend_jit_trace_supported(&op_array->opcodes[i]);
	}
	ZEND_SET_FUNC_INFO(op_array, (void*)jit_extension);

	opline = op_array->opcodes;
	if (!(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
		while (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT) {
			opline++;
		}
	}

	if (!(ZEND_OP_TRACE_INFO(opline, jit_extension->offset)->trace_flags & ZEND_JIT_TRACE_UNSUPPORTED)) {
		/* function entry */
		opline->handler = (const void*)zend_jit_func_counter_handler;
		ZEND_OP_TRACE_INFO(opline, jit_extension->offset)->counter =
			&zend_jit_hot_counters[ZEND_JIT_COUNTER_NUM];
		ZEND_JIT_COUNTER_NUM = (ZEND_JIT_COUNTER_NUM + 1) % ZEND_HOT_COUNTERS_COUNT;
		ZEND_OP_TRACE_INFO(opline, jit_extension->offset)->trace_flags |=
			ZEND_JIT_TRACE_START_ENTER;
	}

	for (i = 0; i < cfg.blocks_count; i++) {
		if (cfg.blocks[i].flags & ZEND_BB_REACHABLE) {
			if (cfg.blocks[i].flags & ZEND_BB_ENTRY) {
				/* continuation after return from function call */
				opline = op_array->opcodes + cfg.blocks[i].start;
				if (!(ZEND_OP_TRACE_INFO(opline, jit_extension->offset)->trace_flags & ZEND_JIT_TRACE_UNSUPPORTED)) {
					opline->handler = (const void*)zend_jit_ret_counter_handler;
					if (!ZEND_OP_TRACE_INFO(opline, jit_extension->offset)->counter) {
						ZEND_OP_TRACE_INFO(opline, jit_extension->offset)->counter =
							&zend_jit_hot_counters[ZEND_JIT_COUNTER_NUM];
						ZEND_JIT_COUNTER_NUM = (ZEND_JIT_COUNTER_NUM + 1) % ZEND_HOT_COUNTERS_COUNT;
					}
					ZEND_OP_TRACE_INFO(opline, jit_extension->offset)->trace_flags |=
						ZEND_JIT_TRACE_START_RETURN;
				}
			}
			if (cfg.blocks[i].flags & ZEND_BB_LOOP_HEADER) {
				/* loop header */
				opline = op_array->opcodes + cfg.blocks[i].start;
				if (!(ZEND_OP_TRACE_INFO(opline, jit_extension->offset)->trace_flags & ZEND_JIT_TRACE_UNSUPPORTED)) {
					opline->handler = (const void*)zend_jit_loop_counter_handler;
					if (!ZEND_OP_TRACE_INFO(opline, jit_extension->offset)->counter) {
						ZEND_OP_TRACE_INFO(opline, jit_extension->offset)->counter =
							&zend_jit_hot_counters[ZEND_JIT_COUNTER_NUM];
						ZEND_JIT_COUNTER_NUM = (ZEND_JIT_COUNTER_NUM + 1) % ZEND_HOT_COUNTERS_COUNT;
					}
					ZEND_OP_TRACE_INFO(opline, jit_extension->offset)->trace_flags |=
						ZEND_JIT_TRACE_START_LOOP;
				}
			}
		}
	}

	return SUCCESS;
}

static void zend_jit_trace_init_caches(void)
{
	memset(JIT_G(bad_root_cache_opline), 0, sizeof(JIT_G(bad_root_cache_opline)));
	memset(JIT_G(bad_root_cache_count), 0, sizeof(JIT_G(bad_root_cache_count)));
	memset(JIT_G(bad_root_cache_stop), 0, sizeof(JIT_G(bad_root_cache_count)));
	JIT_G(bad_root_slot) = 0;

	memset(JIT_G(exit_counters), 0, sizeof(JIT_G(exit_counters)));
}

static void zend_jit_trace_reset_caches(void)
{
}
