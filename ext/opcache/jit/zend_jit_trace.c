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

static zend_op_array dummy_op_array;
static zend_jit_trace_info *zend_jit_traces = NULL;
static const void **zend_jit_exit_groups = NULL;

#define ZEND_JIT_COUNTER_NUM   zend_jit_traces[0].root
#define ZEND_JIT_TRACE_NUM     zend_jit_traces[0].id
#define ZEND_JIT_EXIT_NUM      zend_jit_traces[0].exit_count
#define ZEND_JIT_EXIT_COUNTERS zend_jit_traces[0].exit_counters

#define ZEND_JIT_TRACE_STOP_DESCRIPTION(name, description) \
	description,

static const char * zend_jit_trace_stop_description[] = {
	ZEND_JIT_TRACE_STOP(ZEND_JIT_TRACE_STOP_DESCRIPTION)
};

static zend_always_inline const char *zend_jit_trace_star_desc(uint8_t trace_flags)
{
	if (trace_flags & ZEND_JIT_TRACE_START_LOOP) {
		return "loop";
	} else if (trace_flags & ZEND_JIT_TRACE_START_ENTER) {
		return "enter";
	} else if (trace_flags & ZEND_JIT_TRACE_START_RETURN) {
		return "return";
	} else {
		ZEND_UNREACHABLE();
		return "???";
	}
}

static int zend_jit_trace_startup(void)
{
	zend_jit_traces = (zend_jit_trace_info*)zend_shared_alloc(sizeof(zend_jit_trace_info) * JIT_G(max_root_traces));
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

	memset(&dummy_op_array, 0, sizeof(dummy_op_array));
	dummy_op_array.fn_flags = ZEND_ACC_DONE_PASS_TWO;

	JIT_G(exit_counters) = calloc(JIT_G(max_exit_counters), 1);
	if (JIT_G(exit_counters) == NULL) {
		return FAILURE;
	}

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
	if (JIT_G(debug) & ZEND_JIT_DEBUG_ASM) {
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

static uint32_t zend_jit_trace_get_exit_point(const zend_op *to_opline, uint32_t flags)
{
	zend_jit_trace_info *t = &zend_jit_traces[ZEND_JIT_TRACE_NUM];
	uint32_t exit_point;
	const zend_op_array *op_array;
	uint32_t stack_offset = (uint32_t)-1;
	uint32_t stack_size;
	zend_jit_trace_stack *stack = NULL;

	if (delayed_call_chain) {
		assert(to_opline != NULL); /* CALL and IP share the same register */
		flags |= ZEND_JIT_EXIT_RESTORE_CALL;
	}
	if (JIT_G(current_frame)) {
		op_array = &JIT_G(current_frame)->func->op_array;
		stack_size = op_array->last_var + op_array->T;
		if (stack_size) {
			stack = JIT_G(current_frame)->stack;
			do {
				if (STACK_TYPE(stack, stack_size-1) != IS_UNKNOWN
				 || STACK_REG(stack, stack_size-1) != ZREG_NONE) {
					break;
				}
				stack_size--;
			} while (stack_size);
		}
	} else {
		op_array = NULL;
		stack_size = 0;
	}

	/* Try to reuse exit points */
	if (to_opline != NULL && t->exit_count > 0) {
		uint32_t i = t->exit_count;

		do {
			i--;
			if (stack_size == 0
			 || (t->exit_info[i].stack_size >= stack_size
			  && memcmp(t->stack_map + t->exit_info[i].stack_offset, stack, stack_size * sizeof(zend_jit_trace_stack)) == 0)) {
				stack_offset = t->exit_info[i].stack_offset;
				if (t->exit_info[i].opline == to_opline
				 && t->exit_info[i].flags == flags
				 && t->exit_info[i].stack_size == stack_size) {
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
			t->stack_map = erealloc(t->stack_map, t->stack_map_size * sizeof(zend_jit_trace_stack));
			memcpy(t->stack_map + stack_offset, stack, stack_size * sizeof(zend_jit_trace_stack));
		}
		t->exit_count++;
		t->exit_info[exit_point].opline = to_opline;
		t->exit_info[exit_point].op_array = op_array;
		t->exit_info[exit_point].flags = flags;
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
	ZEND_UNREACHABLE();
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

static int zend_jit_trace_may_exit(const zend_op_array *op_array, const zend_op *opline)
{
	switch (opline->opcode) {
		case ZEND_IS_IDENTICAL:
		case ZEND_IS_NOT_IDENTICAL:
		case ZEND_IS_EQUAL:
		case ZEND_IS_NOT_EQUAL:
		case ZEND_IS_SMALLER:
		case ZEND_IS_SMALLER_OR_EQUAL:
		case ZEND_CASE:
		case ZEND_CASE_STRICT:
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
		case ZEND_JMP_NULL:
		case ZEND_FE_RESET_R:
		case ZEND_FE_RESET_RW:
		case ZEND_ASSERT_CHECK:
		case ZEND_FE_FETCH_R:
		case ZEND_FE_FETCH_RW:
		case ZEND_SWITCH_LONG:
		case ZEND_SWITCH_STRING:
		case ZEND_MATCH:
			/* branch opcodes */
			return 1;
		case ZEND_NEW:
			if (opline->extended_value == 0 && (opline+1)->opcode == ZEND_DO_FCALL) {
				/* NEW may skip constructor without arguments */
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
		case ZEND_MATCH_ERROR:
			/* unsupported */
			return 1;
		case ZEND_DO_FCALL:
			/* potentially polymorphic call */
			return 1;
#if 0
		case ZEND_DO_UCALL:
		case ZEND_DO_FCALL_BY_NAME:
			/* monomorphic call */
			// TODO: recompilation may change target ???
			return 0;
#endif
		case ZEND_RETURN_BY_REF:
		case ZEND_RETURN:
			/* return */
			return !JIT_G(current_frame) || TRACE_FRAME_IS_UNKNOWN_RETURN(JIT_G(current_frame));
		default:
			break;
	}
	return 0;
}

static zend_always_inline uint32_t zend_jit_trace_type_to_info_ex(zend_uchar type, uint32_t info)
{
	if (type == IS_UNKNOWN) {
		return info;
	}
	ZEND_ASSERT(info & (1 << type));
	if (type < IS_STRING) {
		return (1 << type);
	} else if (type != IS_ARRAY) {
		return (1 << type) | (info & (MAY_BE_RC1|MAY_BE_RCN));
	} else {
		return MAY_BE_ARRAY | (info & (MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_OF_REF|MAY_BE_ARRAY_KEY_ANY|MAY_BE_RC1|MAY_BE_RCN));
	}
}

static zend_always_inline uint32_t zend_jit_trace_type_to_info(zend_uchar type)
{
	return zend_jit_trace_type_to_info_ex(type, -1);
}

static zend_always_inline zend_ssa_alias_kind zend_jit_var_may_alias(const zend_op_array *op_array, const zend_ssa *ssa, uint32_t var)
{
	if (var >= op_array->last_var) {
		return NO_ALIAS;
	} else if ((!op_array->function_name || (ssa->cfg.flags & ZEND_FUNC_INDIRECT_VAR_ACCESS))) {
		return SYMTABLE_ALIAS;
	} else if (ssa->vars) {
		return ssa->vars[var].alias;
	} else if (zend_string_equals_literal(op_array->vars[var], "http_response_header")) {
		return HTTP_RESPONSE_HEADER_ALIAS;
	}
	return NO_ALIAS;
}

#define STACK_VAR_TYPE(_var) \
	STACK_TYPE(stack, EX_VAR_TO_NUM(_var))

#define SET_STACK_VAR_TYPE(_var, _type) do { \
		SET_STACK_TYPE(stack, EX_VAR_TO_NUM(_var), _type); \
	} while (0)



static zend_always_inline void zend_jit_trace_add_op_guard(const zend_op_array  *op_array,
                                                           const zend_ssa       *ssa,
                                                           zend_ssa             *tssa,
                                                           uint32_t              var,
                                                           int                   ssa_var,
                                                           uint8_t               op_type)
{
	zend_ssa_var_info *info = &tssa->var_info[ssa_var];

	if (zend_jit_var_may_alias(op_array, ssa, EX_VAR_TO_NUM(var)) != NO_ALIAS) {
		info->type = MAY_BE_GUARD | zend_jit_trace_type_to_info(op_type);
	} else if ((info->type & (MAY_BE_ANY|MAY_BE_UNDEF)) != (1 << op_type)) {
		info->type = MAY_BE_GUARD | zend_jit_trace_type_to_info_ex(op_type, info->type);
	}
}

#define ADD_OP_GUARD(_var, _ssa_var, _op_type) do { \
		if (_ssa_var >= 0 && _op_type != IS_UNKNOWN) { \
			zend_jit_trace_add_op_guard(op_array, ssa, tssa, _var, _ssa_var, _op_type); \
		} \
	} while (0)

#define CHECK_OP_TRACE_TYPE(_var, _ssa_var, op_info, op_type) do { \
		if (op_type != IS_UNKNOWN) { \
			if ((op_info & MAY_BE_GUARD) != 0 \
			 && op_type != STACK_VAR_TYPE(_var)) { \
				if (!zend_jit_type_guard(&dasm_state, opline, _var, op_type)) { \
					goto jit_failure; \
				} \
				if (zend_jit_var_may_alias(op_array, op_array_ssa, _var) != NO_ALIAS) { \
					SET_STACK_VAR_TYPE(_var, IS_UNKNOWN); \
				} else { \
					SET_STACK_VAR_TYPE(_var, op_type); \
					op_info &= ~MAY_BE_GUARD; \
				} \
				ssa->var_info[_ssa_var].type &= op_info; \
			} \
		} \
	} while (0)

#define ADD_OP1_TRACE_GUARD() \
	ADD_OP_GUARD(opline->op1.var, tssa->ops[idx].op1_use, op1_type)
#define ADD_OP2_TRACE_GUARD() \
	ADD_OP_GUARD(opline->op2.var, tssa->ops[idx].op2_use, op2_type)
#define ADD_OP1_DATA_TRACE_GUARD() \
	ADD_OP_GUARD((opline+1)->op1.var, tssa->ops[idx+1].op1_use, op3_type)

#define CHECK_OP1_TRACE_TYPE() \
	CHECK_OP_TRACE_TYPE(opline->op1.var, ssa_op->op1_use, op1_info, op1_type)
#define CHECK_OP2_TRACE_TYPE() \
	CHECK_OP_TRACE_TYPE(opline->op2.var, ssa_op->op2_use, op2_info, op2_type)
#define CHECK_OP1_DATA_TRACE_TYPE() \
	CHECK_OP_TRACE_TYPE((opline+1)->op1.var, (ssa_op+1)->op1_use, op1_data_info, op3_type)

#define SET_OP1_STACK_VAR_TYPE(_type) \
	SET_STACK_VAR_TYPE(opline->op1.var, _type)
#define SET_OP2_STACK_VAR_TYPE( _type) \
	SET_STACK_VAR_TYPE(opline->op2.var, _type)
#define SET_OP1_DATA_STACK_VAR_TYPE(_type) \
	SET_STACK_VAR_TYPE((opline+1)->op1.var, _type)
#define SET_RES_STACK_VAR_TYPE(_type) \
	SET_STACK_VAR_TYPE(opline->result.var, _type)

static zend_always_inline size_t zend_jit_trace_frame_size(const zend_op_array *op_array)
{
	if (op_array && op_array->type == ZEND_USER_FUNCTION) {
		return offsetof(zend_jit_trace_stack_frame, stack) + ZEND_MM_ALIGNED_SIZE((op_array->last_var + op_array->T) * sizeof(zend_jit_trace_stack));
	} else {
		return offsetof(zend_jit_trace_stack_frame, stack);
	}
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
	zend_jit_trace_stack *stack = call->stack;
	const zend_op_array *op_array = &call->func->op_array;
	uint32_t arg_num = opline->op2.num;

	if (op_array->type != ZEND_USER_FUNCTION
	 || arg_num > op_array->num_args) {
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
	SET_STACK_VAR_TYPE(opline->result.var, type);
}

static zend_ssa *zend_jit_trace_build_ssa(const zend_op_array *op_array, zend_script *script)
{
	zend_jit_op_array_trace_extension *jit_extension;
	zend_ssa *ssa;

	jit_extension =
		(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
    jit_extension->func_info.num = 0;
	jit_extension->func_info.flags &= ZEND_FUNC_JIT_ON_FIRST_EXEC
		| ZEND_FUNC_JIT_ON_PROF_REQUEST
		| ZEND_FUNC_JIT_ON_HOT_COUNTERS
		| ZEND_FUNC_JIT_ON_HOT_TRACE;
	memset(&jit_extension->func_info.ssa, 0, sizeof(zend_func_info) - offsetof(zend_func_info, ssa));
	ssa = &jit_extension->func_info.ssa;

	if (JIT_G(opt_level) >= ZEND_JIT_LEVEL_OPT_FUNC) {
		do {
			if (zend_jit_op_array_analyze1(op_array, script, ssa) != SUCCESS) {
				break;
			}

			if (JIT_G(opt_level) >= ZEND_JIT_LEVEL_OPT_FUNCS) {
				if (zend_analyze_calls(&CG(arena), script, ZEND_CALL_TREE, (zend_op_array*)op_array, &jit_extension->func_info) != SUCCESS) {
					break;
				}
				jit_extension->func_info.call_map = zend_build_call_map(&CG(arena), &jit_extension->func_info, op_array);
				if (op_array->fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
					zend_init_func_return_info(op_array, script, &jit_extension->func_info.return_info);
				}
			}

			if (zend_jit_op_array_analyze2(op_array, script, ssa, 0) != SUCCESS) {
				break;
			}

			if (JIT_G(debug) & ZEND_JIT_DEBUG_SSA) {
				zend_dump_op_array(op_array, ZEND_DUMP_HIDE_UNREACHABLE|ZEND_DUMP_RC_INFERENCE|ZEND_DUMP_SSA, "JIT", ssa);
			}
			return ssa;
		} while (0);
	}

	memset(ssa, 0, sizeof(zend_ssa));
	ssa->cfg.blocks_count = 1;
	return ssa;
}

static void zend_jit_dump_trace(zend_jit_trace_rec *trace_buffer, zend_ssa *tssa);
static void zend_jit_dump_exit_info(zend_jit_trace_info *t);

static zend_always_inline int zend_jit_trace_op_len(const zend_op *opline)
{
	int len;

	switch (opline->opcode) {
		case ZEND_ASSIGN_DIM:
		case ZEND_ASSIGN_OBJ:
		case ZEND_ASSIGN_STATIC_PROP:
		case ZEND_ASSIGN_DIM_OP:
		case ZEND_ASSIGN_OBJ_OP:
		case ZEND_ASSIGN_STATIC_PROP_OP:
		case ZEND_ASSIGN_OBJ_REF:
		case ZEND_ASSIGN_STATIC_PROP_REF:
			return 2; /* OP_DATA */
		case ZEND_RECV_INIT:
			len = 1;
			opline++;
			while (opline->opcode == ZEND_RECV_INIT) {
				len++;
				opline++;
			}
			return len;
		case ZEND_BIND_GLOBAL:
			len = 1;
			opline++;
			while (opline->opcode == ZEND_BIND_GLOBAL) {
				len++;
				opline++;
			}
			return len;
//		case ZEND_IS_IDENTICAL:
//		case ZEND_IS_NOT_IDENTICAL:
//		case ZEND_IS_EQUAL:
//		case ZEND_IS_NOT_EQUAL:
//		case ZEND_IS_SMALLER:
//		case ZEND_IS_SMALLER_OR_EQUAL:
//		case ZEND_CASE:
//		case ZEND_ISSET_ISEMPTY_CV:
//		case ZEND_ISSET_ISEMPTY_VAR:
//		case ZEND_ISSET_ISEMPTY_DIM_OBJ:
//		case ZEND_ISSET_ISEMPTY_PROP_OBJ:
//		case ZEND_ISSET_ISEMPTY_STATIC_PROP:
//		case ZEND_INSTANCEOF:
//		case ZEND_TYPE_CHECK:
//		case ZEND_DEFINED:
//		case ZEND_IN_ARRAY:
//		case ZEND_ARRAY_KEY_EXISTS:
		default:
			if ((opline->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
				return 2; /* JMPZ/JMPNZ */
			}
			return 1;
	}
}

static int zend_jit_trace_add_phis(zend_jit_trace_rec *trace_buffer, uint32_t ssa_vars_count, zend_ssa *tssa, zend_jit_trace_stack *stack)
{
	const zend_op_array *op_array;
	zend_jit_trace_rec *p;
	int k, vars_count;
	zend_bitset use, def;
	uint32_t build_flags = ZEND_SSA_RC_INFERENCE | ZEND_SSA_USE_CV_RESULTS;
	uint32_t set_size;
	zend_ssa_phi *prev = NULL;
	int level = 0;
	ALLOCA_FLAG(use_heap);

	op_array = trace_buffer->op_array;
	set_size = zend_bitset_len(op_array->last_var + op_array->T);
	use = ZEND_BITSET_ALLOCA(set_size * 2, use_heap);
	memset(use, 0, set_size * 2 * ZEND_BITSET_ELM_SIZE);
	def = use + set_size;
	p = trace_buffer + ZEND_JIT_TRACE_START_REC_SIZE;
	for (;;p++) {
		if (p->op == ZEND_JIT_TRACE_VM && level == 0) {
			const zend_op *opline = p->opline;
			int len;

			zend_dfg_add_use_def_op(op_array, opline, build_flags, use, def);
			len = zend_jit_trace_op_len(opline);
			while (len > 1) {
				opline++;
				if (opline->opcode != ZEND_OP_DATA) {
					zend_dfg_add_use_def_op(op_array, opline, build_flags, use, def);
				}
				len--;
			}
		} else if (p->op == ZEND_JIT_TRACE_INIT_CALL) {
		} else if (p->op == ZEND_JIT_TRACE_DO_ICALL) {
		} else if (p->op == ZEND_JIT_TRACE_ENTER) {
			level++;
		} else if (p->op == ZEND_JIT_TRACE_BACK) {
			if (level == 0) {
				// Phi for recursive calls and returns are not supported yet ???
				assert(0);
			} else {
				level--;
			}
		} else if (p->op == ZEND_JIT_TRACE_END) {
			break;
		}
	}

	zend_bitset_intersection(use, def, set_size);

	if (trace_buffer->start == ZEND_JIT_TRACE_START_ENTER) {
		vars_count = op_array->last_var;
	} else {
		vars_count = op_array->last_var + op_array->T;
	}
	for (k = 0; k < vars_count; k++) {
		if (zend_bitset_in(use, k)) {
			zend_ssa_phi *phi = zend_arena_calloc(&CG(arena), 1,
				ZEND_MM_ALIGNED_SIZE(sizeof(zend_ssa_phi)) +
				ZEND_MM_ALIGNED_SIZE(sizeof(int) * 2) +
				sizeof(void*) * 2);
			phi->sources = (int*)(((char*)phi) + ZEND_MM_ALIGNED_SIZE(sizeof(zend_ssa_phi)));
			phi->sources[0] = STACK_VAR(stack, k);
			phi->sources[1] = -1;
			phi->use_chains = (zend_ssa_phi**)(((char*)phi->sources) + ZEND_MM_ALIGNED_SIZE(sizeof(int) * 2));
			phi->pi = -1;
			phi->var = k;
			phi->ssa_var = ssa_vars_count;
			SET_STACK_VAR(stack, k, ssa_vars_count);
			ssa_vars_count++;
			phi->block = 1;
			if (prev) {
				prev->next = phi;
			} else {
				tssa->blocks[1].phis = phi;
			}
			prev = phi;
		}
	}

	free_alloca(use, use_heap);

	return ssa_vars_count;
}

static int zend_jit_trace_copy_ssa_var_info(const zend_op_array *op_array, const zend_ssa *ssa, const zend_op **tssa_opcodes, zend_ssa *tssa, int ssa_var)
{
	int var, use;
	zend_ssa_op *op;
	zend_ssa_var_info *info;
	unsigned int no_val;

	if (tssa->vars[ssa_var].phi_use_chain) {
		// TODO: this may be incorrect ???
		var = tssa->vars[ssa_var].phi_use_chain->ssa_var;
	} else {
		var = ssa_var;
	}
	use = tssa->vars[var].use_chain;
	if (use >= 0) {
		ZEND_ASSERT((tssa_opcodes[use] - op_array->opcodes) < op_array->last);
		op = ssa->ops + (tssa_opcodes[use] - op_array->opcodes);
		if (tssa->ops[use].op1_use == var) {
			no_val = ssa->vars[op->op1_use].no_val;
			info = ssa->var_info + op->op1_use;
		} else if (tssa->ops[use].op2_use == var) {
			no_val = ssa->vars[op->op2_use].no_val;
			info = ssa->var_info + op->op2_use;
		} else if (tssa->ops[use].result_use == var) {
			no_val = ssa->vars[op->result_use].no_val;
			info = ssa->var_info + op->result_use;
		} else {
			assert(0);
		}
		tssa->vars[ssa_var].no_val = no_val;
		memcpy(&tssa->var_info[ssa_var], info, sizeof(zend_ssa_var_info));
		return 1;
	}
	return 0;
}

static int zend_jit_trace_copy_ssa_var_range(const zend_op_array *op_array, const zend_ssa *ssa, const zend_op **tssa_opcodes, zend_ssa *tssa, int ssa_var)
{
	int def;
	zend_ssa_op *op;
	zend_ssa_var_info *info;
	unsigned int no_val;

	def = tssa->vars[ssa_var].definition;
	if (def >= 0) {
		ZEND_ASSERT((tssa_opcodes[def] - op_array->opcodes) < op_array->last);
		op = ssa->ops + (tssa_opcodes[def] - op_array->opcodes);
		if (tssa->ops[def].op1_def == ssa_var) {
			no_val = ssa->vars[op->op1_def].no_val;
			info = ssa->var_info + op->op1_def;
		} else if (tssa->ops[def].op2_def == ssa_var) {
			no_val = ssa->vars[op->op2_def].no_val;
			info = ssa->var_info + op->op2_def;
		} else if (tssa->ops[def].result_def == ssa_var) {
			no_val = ssa->vars[op->result_def].no_val;
			info = ssa->var_info + op->result_def;
		} else {
			assert(0);
		}

		tssa->vars[ssa_var].no_val = no_val;

		if (info->has_range) {
			if (tssa->var_info[ssa_var].has_range) {
				tssa->var_info[ssa_var].range.min = MAX(tssa->var_info[ssa_var].range.min, info->range.min);
				tssa->var_info[ssa_var].range.max = MIN(tssa->var_info[ssa_var].range.max, info->range.max);
				tssa->var_info[ssa_var].range.underflow = tssa->var_info[ssa_var].range.underflow && info->range.underflow;
				tssa->var_info[ssa_var].range.overflow = tssa->var_info[ssa_var].range.overflow && info->range.overflow;
			} else {
				tssa->var_info[ssa_var].has_range = 1;
				tssa->var_info[ssa_var].range = info->range;
			}
		}
		return 1;
	}
	return 0;
}

static int zend_jit_trace_restrict_ssa_var_info(const zend_op_array *op_array, const zend_ssa *ssa, const zend_op **tssa_opcodes, zend_ssa *tssa, int ssa_var)
{
	int def;
	zend_ssa_op *op;
	zend_ssa_var_info *info;

	def = tssa->vars[ssa_var].definition;
	if (def >= 0) {
		ZEND_ASSERT((tssa_opcodes[def] - op_array->opcodes) < op_array->last);
		op = ssa->ops + (tssa_opcodes[def] - op_array->opcodes);
		if (tssa->ops[def].op1_def == ssa_var) {
			info = ssa->var_info + op->op1_def;
		} else if (tssa->ops[def].op2_def == ssa_var) {
			info = ssa->var_info + op->op2_def;
		} else if (tssa->ops[def].result_def == ssa_var) {
			info = ssa->var_info + op->result_def;
		} else {
			assert(0);
		}
		tssa->var_info[ssa_var].type &= info->type;
		if (info->ce) {
			if (tssa->var_info[ssa_var].ce) {
				if (tssa->var_info[ssa_var].ce != info->ce) {
					if (instanceof_function(tssa->var_info[ssa_var].ce, info->ce)) {
						/* everything fine */
					} else if (instanceof_function(info->ce, tssa->var_info[ssa_var].ce)) {
						// TODO: TSSA may miss Pi() functions and corresponding instanceof() constraints ???
					} else {
						ZEND_UNREACHABLE();
					}
				}
				tssa->var_info[ssa_var].is_instanceof =
					tssa->var_info[ssa_var].is_instanceof && info->is_instanceof;
			} else {
				tssa->var_info[ssa_var].ce = info->ce;
				tssa->var_info[ssa_var].is_instanceof = info->is_instanceof;
			}
		}
		if (info->has_range) {
			if (tssa->var_info[ssa_var].has_range) {
				tssa->var_info[ssa_var].range.min = MAX(tssa->var_info[ssa_var].range.min, info->range.min);
				tssa->var_info[ssa_var].range.max = MIN(tssa->var_info[ssa_var].range.max, info->range.max);
				tssa->var_info[ssa_var].range.underflow = tssa->var_info[ssa_var].range.underflow && info->range.underflow;
				tssa->var_info[ssa_var].range.overflow = tssa->var_info[ssa_var].range.overflow && info->range.overflow;
			} else {
				tssa->var_info[ssa_var].has_range = 1;
				tssa->var_info[ssa_var].range = info->range;
			}
		}
		return 1;
	}
	return 0;
}

static int find_return_ssa_var(zend_jit_trace_rec *p, zend_ssa_op *ssa_op)
{
	while (1) {
		if (p->op == ZEND_JIT_TRACE_VM) {
			if (p->opline->opcode == ZEND_DO_UCALL
			 || p->opline->opcode == ZEND_DO_FCALL_BY_NAME
			 || p->opline->opcode == ZEND_DO_FCALL) {
				if (p->opline->result_type != IS_UNUSED) {
					return ssa_op->result_def;
				}
			}
			return -1;
		} else if (p->op == ZEND_JIT_TRACE_OP1_TYPE || p->op == ZEND_JIT_TRACE_OP2_TYPE) {
			/*skip */
		} else {
			return -1;
		}
		p--;
	}
}

static const zend_op *zend_jit_trace_find_init_fcall_op(zend_jit_trace_rec *p, const zend_op_array *op_array)
{
	if (!(p->info & ZEND_JIT_TRACE_FAKE_INIT_CALL)) {
		p--;
		while (1) {
			if (p->op == ZEND_JIT_TRACE_VM) {
				if (p->opline->opcode == ZEND_INIT_FCALL
				 || p->opline->opcode == ZEND_INIT_FCALL_BY_NAME
				 || p->opline->opcode == ZEND_INIT_NS_FCALL_BY_NAME
				 || p->opline->opcode == ZEND_INIT_DYNAMIC_CALL
				 || p->opline->opcode == ZEND_INIT_USER_CALL
				 || p->opline->opcode == ZEND_NEW
				 || p->opline->opcode == ZEND_INIT_METHOD_CALL
				 || p->opline->opcode == ZEND_INIT_STATIC_METHOD_CALL) {
					return p->opline;
				}
				return NULL;
			} else if (p->op == ZEND_JIT_TRACE_OP1_TYPE || p->op == ZEND_JIT_TRACE_OP2_TYPE) {
				/*skip */
			} else {
				return NULL;
			}
			p--;
		}
	} else {
		const zend_op *opline = NULL;
		int call_level = 0;

		p++;
		while (1) {
			if (p->op == ZEND_JIT_TRACE_VM) {
				opline = p->opline;
				break;
			} else if (p->op == ZEND_JIT_TRACE_INIT_CALL) {
				call_level++;
				/*skip */
			} else {
				return NULL;
			}
			p--;
		}
		if (opline) {
			while (opline > op_array->opcodes) {
				opline--;
				switch (opline->opcode) {
					case ZEND_INIT_FCALL:
					case ZEND_INIT_FCALL_BY_NAME:
					case ZEND_INIT_NS_FCALL_BY_NAME:
					case ZEND_INIT_METHOD_CALL:
					case ZEND_INIT_DYNAMIC_CALL:
					case ZEND_INIT_STATIC_METHOD_CALL:
					case ZEND_INIT_USER_CALL:
					case ZEND_NEW:
						if (call_level == 0) {
							return opline;
						}
						call_level--;
						break;
					case ZEND_DO_FCALL:
					case ZEND_DO_ICALL:
					case ZEND_DO_UCALL:
					case ZEND_DO_FCALL_BY_NAME:
						call_level++;
						break;
				}
			}
		}
	}
	return NULL;
}

static int is_checked_guard(const zend_ssa *tssa, const zend_op **ssa_opcodes, uint32_t var, uint32_t phi_var)
{
	if ((tssa->var_info[phi_var].type & MAY_BE_ANY) == MAY_BE_LONG) {
		int idx = tssa->vars[var].definition;

		if (idx >= 0) {
			if (tssa->ops[idx].op1_def == var) {
				const zend_op *opline = ssa_opcodes[idx];
				if (opline->opcode == ZEND_PRE_DEC
				 || opline->opcode == ZEND_PRE_INC
				 || opline->opcode == ZEND_POST_DEC
				 || opline->opcode == ZEND_POST_INC) {
					return 1;
				} else if (opline->opcode == ZEND_ASSIGN_OP
				 && (opline->extended_value == ZEND_ADD
				  || opline->extended_value == ZEND_SUB
				  || opline->extended_value == ZEND_MUL)) {
					return 1;
				}
			}
			if (tssa->ops[idx].result_def == var) {
				const zend_op *opline = ssa_opcodes[idx];
				if (opline->opcode == ZEND_ADD
				 || opline->opcode == ZEND_SUB
				 || opline->opcode == ZEND_MUL
				 || opline->opcode == ZEND_PRE_DEC
				 || opline->opcode == ZEND_PRE_INC
				 || opline->opcode == ZEND_POST_DEC
				 || opline->opcode == ZEND_POST_INC) {
					return 1;
				}
			}
		}
	}
	return 0;
}

typedef struct _zend_tssa {
	zend_ssa        ssa;
	const zend_op **tssa_opcodes;
} zend_tssa;

static const zend_op _nop_opcode = {0};

static zend_ssa *zend_jit_trace_build_tssa(zend_jit_trace_rec *trace_buffer, uint32_t parent_trace, uint32_t exit_num, zend_script *script, const zend_op_array **op_arrays, int *num_op_arrays_ptr)
{
	zend_ssa *tssa;
	zend_ssa_op *ssa_ops, *op;
	zend_ssa_var *ssa_vars;
	zend_ssa_var_info *ssa_var_info;
	const zend_op_array *op_array;
	const zend_op *opline;
	const zend_op **ssa_opcodes;
	zend_jit_trace_rec *p;
	int i, v, idx, len, ssa_ops_count, vars_count, ssa_vars_count;
	zend_jit_trace_stack *stack;
	uint32_t build_flags = ZEND_SSA_RC_INFERENCE | ZEND_SSA_USE_CV_RESULTS;
	uint32_t optimization_level = 0;
	int call_level, level, num_op_arrays;
	size_t frame_size, stack_top, stack_size, stack_bottom;
	zend_jit_op_array_trace_extension *jit_extension;
	zend_ssa *ssa;
	zend_jit_trace_stack_frame *frame, *top, *call;
	zend_ssa_var_info return_value_info;

	/* 1. Count number of TSSA opcodes;
	 *    Count number of activation frames;
	 *    Calculate size of abstract stack;
	 *    Construct regular SSA for involved op_array */
	op_array = trace_buffer->op_array;
	stack_top = stack_size = zend_jit_trace_frame_size(op_array);
	stack_bottom = 0;
	p = trace_buffer + ZEND_JIT_TRACE_START_REC_SIZE;
	ssa_ops_count = 0;
	call_level = 0;
	level = 0;
	num_op_arrays = 0;
	/* Remember op_array to cleanup */
	op_arrays[num_op_arrays++] = op_array;
	/* Build SSA */
	ssa = zend_jit_trace_build_ssa(op_array, script);
	for (;;p++) {
		if (p->op == ZEND_JIT_TRACE_VM) {
			if (JIT_G(opt_level) < ZEND_JIT_LEVEL_OPT_FUNC) {
				const zend_op *opline = p->opline;

				switch (opline->opcode) {
					case ZEND_INCLUDE_OR_EVAL:
						ssa->cfg.flags |= ZEND_FUNC_INDIRECT_VAR_ACCESS;
						break;
					case ZEND_FETCH_R:
					case ZEND_FETCH_W:
					case ZEND_FETCH_RW:
					case ZEND_FETCH_FUNC_ARG:
					case ZEND_FETCH_IS:
					case ZEND_FETCH_UNSET:
					case ZEND_UNSET_VAR:
					case ZEND_ISSET_ISEMPTY_VAR:
						if (opline->extended_value & ZEND_FETCH_LOCAL) {
							ssa->cfg.flags |= ZEND_FUNC_INDIRECT_VAR_ACCESS;
						} else if ((opline->extended_value & (ZEND_FETCH_GLOBAL | ZEND_FETCH_GLOBAL_LOCK)) &&
						           !op_array->function_name) {
							ssa->cfg.flags |= ZEND_FUNC_INDIRECT_VAR_ACCESS;
						}
						break;
				}
			}
			ssa_ops_count += zend_jit_trace_op_len(p->opline);
		} else if (p->op == ZEND_JIT_TRACE_INIT_CALL) {
			call_level++;
			stack_top += zend_jit_trace_frame_size(p->op_array);
			if (stack_top > stack_size) {
				stack_size = stack_top;
			}
		} else if (p->op == ZEND_JIT_TRACE_DO_ICALL) {
			if (JIT_G(opt_level) < ZEND_JIT_LEVEL_OPT_FUNC) {
				if (p->func != (zend_function*)&zend_pass_function
				 && (zend_string_equals_literal(p->func->common.function_name, "extract")
				  || zend_string_equals_literal(p->func->common.function_name, "compact")
				  || zend_string_equals_literal(p->func->common.function_name, "get_defined_vars"))) {
					ssa->cfg.flags |= ZEND_FUNC_INDIRECT_VAR_ACCESS;
				}
			}
			frame_size = zend_jit_trace_frame_size(p->op_array);
			if (call_level == 0) {
				if (stack_top + frame_size > stack_size) {
					stack_size = stack_top + frame_size;
				}
			} else {
				call_level--;
				stack_top -= frame_size;
			}
		} else if (p->op == ZEND_JIT_TRACE_ENTER) {
			op_array = p->op_array;
			if (call_level == 0) {
				stack_top += zend_jit_trace_frame_size(op_array);
				if (stack_top > stack_size) {
					stack_size = stack_top;
				}
			} else {
				call_level--;
			}
			level++;
			jit_extension =
				(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
			ssa = &jit_extension->func_info.ssa;
			if (ssa->cfg.blocks_count) {
				/* pass */
			} else if (num_op_arrays == ZEND_JIT_TRACE_MAX_FUNCS) {
				/* Too many functions in single trace */
				*num_op_arrays_ptr = num_op_arrays;
				return NULL;
			} else {
				/* Remember op_array to cleanup */
				op_arrays[num_op_arrays++] = op_array;
				/* Build SSA */
				ssa = zend_jit_trace_build_ssa(op_array, script);
			}
		} else if (p->op == ZEND_JIT_TRACE_BACK) {
			if (level == 0) {
				stack_bottom += zend_jit_trace_frame_size(p->op_array);
				jit_extension =
					(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
				ssa = &jit_extension->func_info.ssa;
				if (ssa->cfg.blocks_count) {
					/* pass */
				} else if (num_op_arrays == ZEND_JIT_TRACE_MAX_FUNCS) {
					/* Too many functions in single trace */
					*num_op_arrays_ptr = num_op_arrays;
					return NULL;
				} else {
					/* Remember op_array to cleanup */
					op_arrays[num_op_arrays++] = op_array;
					/* Build SSA */
					ssa = zend_jit_trace_build_ssa(op_array, script);
				}
			} else {
				stack_top -= zend_jit_trace_frame_size(op_array);
				level--;
			}
			op_array = p->op_array;
		} else if (p->op == ZEND_JIT_TRACE_END) {
			break;
		}
	}
	*num_op_arrays_ptr = num_op_arrays;

	/* Allocate space for abstract stack */
	JIT_G(current_frame) = frame = (zend_jit_trace_stack_frame*)((char*)zend_arena_alloc(&CG(arena), stack_bottom + stack_size) + stack_bottom);

	/* 2. Construct TSSA */
	tssa = zend_arena_calloc(&CG(arena), 1, sizeof(zend_tssa));
	tssa->cfg.flags = ZEND_SSA_TSSA;
	tssa->cfg.blocks = zend_arena_calloc(&CG(arena), 2, sizeof(zend_basic_block));
	tssa->blocks = zend_arena_calloc(&CG(arena), 2, sizeof(zend_ssa_block));
	tssa->cfg.predecessors = zend_arena_calloc(&CG(arena), 2, sizeof(int));

	if (trace_buffer->stop == ZEND_JIT_TRACE_STOP_LOOP) {
		tssa->cfg.blocks_count = 2;
		tssa->cfg.edges_count = 2;

		tssa->cfg.predecessors[0] = 0;
		tssa->cfg.predecessors[1] = 1;

		tssa->cfg.blocks[0].flags = ZEND_BB_START|ZEND_BB_REACHABLE;
		tssa->cfg.blocks[0].successors_count = 1;
		tssa->cfg.blocks[0].predecessors_count = 0;
		tssa->cfg.blocks[0].successors = tssa->cfg.blocks[0].successors_storage;
		tssa->cfg.blocks[0].successors[0] = 1;

		tssa->cfg.blocks[0].flags = ZEND_BB_FOLLOW|ZEND_BB_TARGET|ZEND_BB_LOOP_HEADER|ZEND_BB_REACHABLE;
		tssa->cfg.blocks[1].successors_count = 1;
		tssa->cfg.blocks[1].predecessors_count = 2;
		tssa->cfg.blocks[1].successors = tssa->cfg.blocks[1].successors_storage;
		tssa->cfg.blocks[1].successors[1] = 1;
	} else {
		tssa->cfg.blocks_count = 1;
		tssa->cfg.edges_count = 0;

		tssa->cfg.blocks[0].flags = ZEND_BB_START|ZEND_BB_EXIT|ZEND_BB_REACHABLE;
		tssa->cfg.blocks[0].successors_count = 0;
		tssa->cfg.blocks[0].predecessors_count = 0;
	}

	if (JIT_G(opt_level) < ZEND_JIT_LEVEL_INLINE) {
		return tssa;
	}

	tssa->ops = ssa_ops = zend_arena_alloc(&CG(arena), ssa_ops_count * sizeof(zend_ssa_op));
	memset(ssa_ops, -1, ssa_ops_count * sizeof(zend_ssa_op));
	ssa_opcodes = zend_arena_calloc(&CG(arena), ssa_ops_count + 1, sizeof(zend_op*));
	((zend_tssa*)tssa)->tssa_opcodes = ssa_opcodes;
	ssa_opcodes[ssa_ops_count] = &_nop_opcode;

	op_array = trace_buffer->op_array;
	if (trace_buffer->start == ZEND_JIT_TRACE_START_ENTER) {
		ssa_vars_count = op_array->last_var;
	} else {
		ssa_vars_count = op_array->last_var + op_array->T;
	}
	stack = frame->stack;
	for (i = 0; i < ssa_vars_count; i++) {
		SET_STACK_VAR(stack, i, i);
	}

	if (trace_buffer->stop == ZEND_JIT_TRACE_STOP_LOOP) {
		// TODO: For tracing, it's possible, to create pseudo Phi functions
		//       at the end of loop, without this additional pass (like LuaJIT) ???
		ssa_vars_count = zend_jit_trace_add_phis(trace_buffer, ssa_vars_count, tssa, stack);
	}

	p = trace_buffer + ZEND_JIT_TRACE_START_REC_SIZE;
	idx = 0;
	level = 0;
	for (;;p++) {
		if (p->op == ZEND_JIT_TRACE_VM) {
			opline = p->opline;
			ssa_opcodes[idx] = opline;
			ssa_vars_count = zend_ssa_rename_op(op_array, opline, idx, build_flags, ssa_vars_count, ssa_ops, (int*)stack);
			idx++;
			len = zend_jit_trace_op_len(p->opline);
			while (len > 1) {
				opline++;
				ssa_opcodes[idx] = opline;
				if (opline->opcode != ZEND_OP_DATA) {
					ssa_vars_count = zend_ssa_rename_op(op_array, opline, idx, build_flags, ssa_vars_count, ssa_ops, (int*)stack);
				}
				idx++;
				len--;
			}
		} else if (p->op == ZEND_JIT_TRACE_ENTER) {
			frame = zend_jit_trace_call_frame(frame, op_array);
			stack = frame->stack;
			op_array = p->op_array;
			level++;
			if (ssa_vars_count >= ZEND_JIT_TRACE_MAX_SSA_VAR) {
				return NULL;
			}
			ZEND_JIT_TRACE_SET_FIRST_SSA_VAR(p->info, ssa_vars_count);
			for (i = 0; i < op_array->last_var; i++) {
				SET_STACK_VAR(stack, i, ssa_vars_count++);
			}
		} else if (p->op == ZEND_JIT_TRACE_BACK) {
			op_array = p->op_array;
			frame = zend_jit_trace_ret_frame(frame, op_array);
			stack = frame->stack;
			if (level == 0) {
				if (ssa_vars_count >= ZEND_JIT_TRACE_MAX_SSA_VAR) {
					return NULL;
				}
				ZEND_JIT_TRACE_SET_FIRST_SSA_VAR(p->info, ssa_vars_count);
				for (i = 0; i < op_array->last_var + op_array->T; i++) {
					SET_STACK_VAR(stack, i, ssa_vars_count++);
				}
			} else {
				level--;
			}
		} else if (p->op == ZEND_JIT_TRACE_END) {
			break;
		}
	}

	op_array = trace_buffer->op_array;
	tssa->vars_count = ssa_vars_count;
	tssa->vars = ssa_vars = zend_arena_calloc(&CG(arena), tssa->vars_count, sizeof(zend_ssa_var));
	if (trace_buffer->start == ZEND_JIT_TRACE_START_ENTER) {
		vars_count = op_array->last_var;
	} else {
		vars_count = op_array->last_var + op_array->T;
	}
	i = 0;
	while (i < vars_count) {
		ssa_vars[i].var = i;
		ssa_vars[i].scc = -1;
		ssa_vars[i].definition = -1;
		ssa_vars[i].use_chain = -1;
		i++;
	}
	while (i < tssa->vars_count) {
		ssa_vars[i].var = -1;
		ssa_vars[i].scc = -1;
		ssa_vars[i].definition = -1;
		ssa_vars[i].use_chain = -1;
		i++;
	}

	if (trace_buffer->stop == ZEND_JIT_TRACE_STOP_LOOP) {
		/* Update Phi sources */
		zend_ssa_phi *phi = tssa->blocks[1].phis;

		while (phi) {
			phi->sources[1] = STACK_VAR(stack, phi->var);
			ssa_vars[phi->ssa_var].var = phi->var;
			ssa_vars[phi->ssa_var].definition_phi = phi;
			ssa_vars[phi->sources[0]].phi_use_chain = phi;
			ssa_vars[phi->sources[1]].phi_use_chain = phi;
			phi = phi->next;
		}
	}

	/* 3. Compute use-def chains */
	idx = (ssa_ops_count - 1);
	op = ssa_ops + idx;
	while (idx >= 0) {
		opline = ssa_opcodes[idx];
		if (op->op1_use >= 0) {
			op->op1_use_chain = ssa_vars[op->op1_use].use_chain;
			ssa_vars[op->op1_use].use_chain = idx;
		}
		if (op->op2_use >= 0 && op->op2_use != op->op1_use) {
			op->op2_use_chain = ssa_vars[op->op2_use].use_chain;
			ssa_vars[op->op2_use].use_chain = idx;
		}
		if (op->result_use >= 0 && op->result_use != op->op1_use && op->result_use != op->op2_use) {
			op->res_use_chain = ssa_vars[op->result_use].use_chain;
			ssa_vars[op->result_use].use_chain = idx;
		}
		if (op->op1_def >= 0) {
			ssa_vars[op->op1_def].var = EX_VAR_TO_NUM(opline->op1.var);
			ssa_vars[op->op1_def].definition = idx;
		}
		if (op->op2_def >= 0) {
			ssa_vars[op->op2_def].var = EX_VAR_TO_NUM(opline->op2.var);
			ssa_vars[op->op2_def].definition = idx;
		}
		if (op->result_def >= 0) {
			ssa_vars[op->result_def].var = EX_VAR_TO_NUM(opline->result.var);
			ssa_vars[op->result_def].definition = idx;
		}
		op--;
		idx--;
	}

	/* 4. Type inference */
	op_array = trace_buffer->op_array;
	jit_extension =
		(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
	ssa = &jit_extension->func_info.ssa;

	tssa->var_info = ssa_var_info = zend_arena_calloc(&CG(arena), tssa->vars_count, sizeof(zend_ssa_var_info));

	if (trace_buffer->start == ZEND_JIT_TRACE_START_ENTER) {
		i = 0;
		while (i < op_array->last_var) {
			if (i < op_array->num_args) {
				if (ssa->var_info
				 && zend_jit_trace_copy_ssa_var_info(op_array, ssa, ssa_opcodes, tssa, i)) {
					/* pass */
				} else if (op_array->arg_info) {
					zend_arg_info *arg_info = &op_array->arg_info[i];
					zend_class_entry *ce;
					uint32_t tmp = zend_fetch_arg_info_type(script, arg_info, &ce);

					if (ZEND_ARG_SEND_MODE(arg_info)) {
						tmp |= MAY_BE_REF;
					}
					ssa_var_info[i].type = tmp;
					ssa_var_info[i].ce = ce;
					ssa_var_info[i].is_instanceof = 1;
				} else {
					ssa_var_info[i].type = MAY_BE_RC1 | MAY_BE_RCN | MAY_BE_REF | MAY_BE_ANY  | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF;
				}
			} else if (op_array->function_name
			        && zend_jit_var_may_alias(op_array, ssa, i) == NO_ALIAS) {
				ssa_vars[i].no_val = ssa->vars ? ssa->vars[i].no_val : 0;
				ssa_var_info[i].type = MAY_BE_UNDEF;
			} else {
				ssa_var_info[i].type = MAY_BE_UNDEF | MAY_BE_RC1 | MAY_BE_RCN | MAY_BE_REF | MAY_BE_ANY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF;
			}
			i++;
		}
	} else {
		int parent_vars_count = 0;
		zend_jit_trace_stack *parent_stack = NULL;

		i = 0;
		if (parent_trace) {
			parent_vars_count = MIN(zend_jit_traces[parent_trace].exit_info[exit_num].stack_size,
				op_array->last_var + op_array->T);
			if (parent_vars_count) {
				parent_stack =
					zend_jit_traces[parent_trace].stack_map +
					zend_jit_traces[parent_trace].exit_info[exit_num].stack_offset;
			}
		}
		while (i < op_array->last_var + op_array->T) {
			if (!ssa->var_info
			 || !zend_jit_trace_copy_ssa_var_info(op_array, ssa, ssa_opcodes, tssa, i)) {
				if (i < op_array->last_var) {
					ssa_var_info[i].type = MAY_BE_UNDEF | MAY_BE_RC1 | MAY_BE_RCN | MAY_BE_REF | MAY_BE_ANY  | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF;
				} else {
					ssa_var_info[i].type = MAY_BE_RC1 | MAY_BE_RCN | MAY_BE_REF | MAY_BE_ANY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF;
				}
			}
			if (i < parent_vars_count) {
				/* Initialize TSSA variable from parent trace */
				zend_uchar op_type = STACK_TYPE(parent_stack, i);

				if (op_type != IS_UNKNOWN) {
					ssa_var_info[i].type &= zend_jit_trace_type_to_info(op_type);
				}
			}
			i++;
		}
	}

	if (trace_buffer->stop == ZEND_JIT_TRACE_STOP_LOOP) {
		/* Propagate initial value through Phi functions */
		zend_ssa_phi *phi = tssa->blocks[1].phis;

		while (phi) {
			if (!ssa->var_info
			 || !zend_jit_trace_copy_ssa_var_info(op_array, ssa, ssa_opcodes, tssa, phi->ssa_var)) {
				ssa_var_info[phi->ssa_var].type = ssa_var_info[phi->sources[0]].type;
			}
			phi = phi->next;
		}
	}

	frame = JIT_G(current_frame);
	top = zend_jit_trace_call_frame(frame, op_array);
	TRACE_FRAME_INIT(frame, op_array, 0, 0);
	TRACE_FRAME_SET_RETURN_SSA_VAR(frame, -1);
	for (i = 0; i < op_array->last_var + op_array->T; i++) {
		SET_STACK_TYPE(frame->stack, i, IS_UNKNOWN);
	}
	memset(&return_value_info, 0, sizeof(return_value_info));

	p = trace_buffer + ZEND_JIT_TRACE_START_REC_SIZE;
	idx = 0;
	level = 0;
	for (;;p++) {
		if (p->op == ZEND_JIT_TRACE_VM) {
			uint8_t orig_op1_type, orig_op2_type, op1_type, op2_type, op3_type;
//			zend_class_entry *op1_ce = NULL;
			zend_class_entry *op2_ce = NULL;

			// TODO: range inference ???
			opline = p->opline;

			op1_type = orig_op1_type = p->op1_type;
			op2_type = orig_op2_type = p->op2_type;
			op3_type = p->op3_type;
			if (op1_type & (IS_TRACE_REFERENCE|IS_TRACE_INDIRECT)) {
				op1_type = IS_UNKNOWN;
			}
			if (op1_type != IS_UNKNOWN) {
				op1_type &= ~IS_TRACE_PACKED;
			}
			if (op2_type & (IS_TRACE_REFERENCE|IS_TRACE_INDIRECT)) {
				op2_type = IS_UNKNOWN;
			}
			if (op3_type & (IS_TRACE_REFERENCE|IS_TRACE_INDIRECT)) {
				op3_type = IS_UNKNOWN;
			}

			if ((p+1)->op == ZEND_JIT_TRACE_OP1_TYPE) {
//				op1_ce = (zend_class_entry*)(p+1)->ce;
				p++;
			}
			if ((p+1)->op == ZEND_JIT_TRACE_OP2_TYPE) {
				op2_ce = (zend_class_entry*)(p+1)->ce;
				p++;
			}

			switch (opline->opcode) {
				case ZEND_ASSIGN_OP:
					if (opline->extended_value == ZEND_POW
					 || opline->extended_value == ZEND_DIV) {
						// TODO: check for division by zero ???
						break;
					}
					if (opline->op1_type != IS_CV || opline->result_type != IS_UNUSED) {
						break;
					}
					ADD_OP1_TRACE_GUARD();
					ADD_OP2_TRACE_GUARD();
					break;
				case ZEND_ASSIGN_DIM_OP:
					if (opline->extended_value == ZEND_POW
					 || opline->extended_value == ZEND_DIV) {
						// TODO: check for division by zero ???
						break;
					}
					if (opline->result_type != IS_UNUSED) {
						break;
					}
					/* break missing intentionally */
				case ZEND_ASSIGN_DIM:
					if (opline->op1_type == IS_CV) {
						ADD_OP1_DATA_TRACE_GUARD();
						ADD_OP2_TRACE_GUARD();
						ADD_OP1_TRACE_GUARD();
					} else if (orig_op1_type != IS_UNKNOWN
					        && (orig_op1_type & IS_TRACE_INDIRECT)
					        && opline->result_type == IS_UNUSED) {
						if (opline->opcode == ZEND_ASSIGN_DIM_OP) {
							ADD_OP1_DATA_TRACE_GUARD();
						}
						ADD_OP2_TRACE_GUARD();
					}
					break;
				case ZEND_ASSIGN_OBJ_OP:
					if (opline->extended_value == ZEND_POW
					 || opline->extended_value == ZEND_DIV) {
						// TODO: check for division by zero ???
						break;
					}
					if (opline->result_type != IS_UNUSED) {
						break;
					}
					/* break missing intentionally */
				case ZEND_ASSIGN_OBJ:
				case ZEND_PRE_INC_OBJ:
				case ZEND_PRE_DEC_OBJ:
				case ZEND_POST_INC_OBJ:
				case ZEND_POST_DEC_OBJ:
					if (opline->op2_type != IS_CONST
					 || Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) != IS_STRING
					 || Z_STRVAL_P(RT_CONSTANT(opline, opline->op2))[0] == '\0') {
						break;
					}
					if (opline->opcode == ZEND_ASSIGN_OBJ_OP) {
						ADD_OP1_DATA_TRACE_GUARD();
					}
					ADD_OP1_TRACE_GUARD();
					break;
				case ZEND_IS_EQUAL:
				case ZEND_IS_NOT_EQUAL:
				case ZEND_IS_SMALLER:
				case ZEND_IS_SMALLER_OR_EQUAL:
				case ZEND_CASE:
				case ZEND_IS_IDENTICAL:
				case ZEND_IS_NOT_IDENTICAL:
				case ZEND_CASE_STRICT:
				case ZEND_BW_OR:
				case ZEND_BW_AND:
				case ZEND_BW_XOR:
				case ZEND_SL:
				case ZEND_SR:
				case ZEND_MOD:
				case ZEND_ADD:
				case ZEND_SUB:
				case ZEND_MUL:
//				case ZEND_DIV: // TODO: check for division by zero ???
				case ZEND_CONCAT:
				case ZEND_FAST_CONCAT:
					ADD_OP2_TRACE_GUARD();
					/* break missing intentionally */
				case ZEND_ECHO:
				case ZEND_STRLEN:
				case ZEND_QM_ASSIGN:
				case ZEND_FE_RESET_R:
				case ZEND_FE_FETCH_R:
					ADD_OP1_TRACE_GUARD();
					break;
				case ZEND_VERIFY_RETURN_TYPE:
					if (opline->op1_type == IS_UNUSED) {
						/* Always throws */
						break;
					}
					if (opline->op1_type == IS_CONST) {
						/* TODO Different instruction format, has return value */
						break;
					}
					if (op_array->fn_flags & ZEND_ACC_RETURN_REFERENCE) {
						/* Not worth bothering with */
						break;
					}
					ADD_OP1_TRACE_GUARD();
					break;
				case ZEND_FETCH_DIM_FUNC_ARG:
					if (!frame
					 || !frame->call
					 || !frame->call->func
					 || !TRACE_FRAME_IS_LAST_SEND_BY_VAL(frame->call)) {
						break;
					}
					ADD_OP2_TRACE_GUARD();
					ADD_OP1_TRACE_GUARD();
					break;
				case ZEND_PRE_INC:
				case ZEND_PRE_DEC:
				case ZEND_POST_INC:
				case ZEND_POST_DEC:
					if (opline->op1_type != IS_CV) {
						break;
					}
					ADD_OP1_TRACE_GUARD();
					break;
				case ZEND_ASSIGN:
					if (opline->op1_type != IS_CV) {
						break;
					}
					ADD_OP2_TRACE_GUARD();
					break;
				case ZEND_CAST:
					if (opline->extended_value != op1_type) {
						break;
					}
					ADD_OP1_TRACE_GUARD();
					break;
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
				case ZEND_BOOL:
				case ZEND_BOOL_NOT:
					ADD_OP1_TRACE_GUARD();
					break;
				case ZEND_ISSET_ISEMPTY_CV:
					if ((opline->extended_value & ZEND_ISEMPTY)) {
						// TODO: support for empty() ???
						break;
					}
					ADD_OP1_TRACE_GUARD();
					break;
				case ZEND_IN_ARRAY:
					if (opline->op1_type == IS_VAR || opline->op1_type == IS_TMP_VAR) {
						break;
					}
					ADD_OP1_TRACE_GUARD();
					break;
				case ZEND_ISSET_ISEMPTY_DIM_OBJ:
					if ((opline->extended_value & ZEND_ISEMPTY)) {
						// TODO: support for empty() ???
						break;
					}
					/* break missing intentionally */
				case ZEND_FETCH_DIM_R:
				case ZEND_FETCH_DIM_IS:
				case ZEND_FETCH_LIST_R:
					ADD_OP1_TRACE_GUARD();
					ADD_OP2_TRACE_GUARD();

					if (opline->op1_type != IS_CONST
					 && op1_type == IS_ARRAY
					 && ((opline->op2_type == IS_CONST
					   && Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) == IS_LONG)
					  || (opline->op2_type != IS_CONST
					   && op2_type == IS_LONG))) {

						zend_ssa_var_info *info = &tssa->var_info[tssa->ops[idx].op1_use];

						if ((info->type & MAY_BE_ARRAY_PACKED)
						 && (info->type & MAY_BE_ARRAY_HASH)
						 && orig_op1_type != IS_UNKNOWN
						 && !(orig_op1_type & IS_TRACE_REFERENCE)) {
							info->type |= MAY_BE_PACKED_GUARD;
							if (orig_op1_type & IS_TRACE_PACKED) {
								info->type &= ~MAY_BE_ARRAY_HASH;
							} else {
								info->type &= ~MAY_BE_ARRAY_PACKED;
							}
						}
					}
					break;
				case ZEND_FETCH_DIM_W:
				case ZEND_FETCH_DIM_RW:
//				case ZEND_FETCH_DIM_UNSET:
				case ZEND_FETCH_LIST_W:
					if (opline->op1_type != IS_CV
					 && (orig_op1_type == IS_UNKNOWN
					  || !(orig_op1_type & IS_TRACE_INDIRECT))) {
						break;
					}
					ADD_OP1_TRACE_GUARD();
					ADD_OP2_TRACE_GUARD();
					break;
				case ZEND_SEND_VAL_EX:
				case ZEND_SEND_VAR_EX:
				case ZEND_SEND_VAR_NO_REF_EX:
					if (opline->op2.num > MAX_ARG_FLAG_NUM) {
						goto propagate_arg;
					}
					/* break missing intentionally */
				case ZEND_SEND_VAL:
				case ZEND_SEND_VAR:
				case ZEND_SEND_VAR_NO_REF:
				case ZEND_SEND_FUNC_ARG:
					ADD_OP1_TRACE_GUARD();
propagate_arg:
					/* Propagate argument type */
					if (frame->call
					 && frame->call->func
					 && frame->call->func->type == ZEND_USER_FUNCTION
					 && opline->op2.num <= frame->call->func->op_array.num_args) {
						uint32_t info;

						if (opline->op1_type == IS_CONST) {
							info = _const_op_type(RT_CONSTANT(opline, opline->op1));
						} else {
							ZEND_ASSERT(ssa_ops[idx].op1_use >= 0);
							info = ssa_var_info[ssa_ops[idx].op1_use].type & ~MAY_BE_GUARD;
						}
						if (frame->call->func->op_array.fn_flags & ZEND_ACC_HAS_TYPE_HINTS) {
							zend_arg_info *arg_info;

							ZEND_ASSERT(frame->call->func->op_array.arg_info);
							arg_info = &frame->call->func->op_array.arg_info[opline->op2.num - 1];
							if (ZEND_TYPE_IS_SET(arg_info->type)) {
								zend_class_entry *ce;
								uint32_t tmp = zend_fetch_arg_info_type(script, arg_info, &ce);
								info &= tmp;
								if (!info) {
									break;
								}
							}
						}
						if (info & MAY_BE_UNDEF) {
							info |= MAY_BE_NULL;
							info &= ~MAY_BE_UNDEF;
						}
						if (ARG_SHOULD_BE_SENT_BY_REF(frame->call->func, opline->op2.num)) {
							info |= MAY_BE_REF|MAY_BE_RC1|MAY_BE_RCN|MAY_BE_ANY|MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_KEY_ANY;
						}
						SET_STACK_INFO(frame->call->stack, opline->op2.num - 1, info);
					}
					break;
				case ZEND_RETURN:
					ADD_OP1_TRACE_GUARD();
					/* Propagate return value types */
					if (opline->op1_type == IS_UNUSED) {
						return_value_info.type = MAY_BE_NULL;
					} else if (opline->op1_type == IS_CONST) {
						return_value_info.type = _const_op_type(RT_CONSTANT(opline, opline->op1));
					} else {
						ZEND_ASSERT(ssa_ops[idx].op1_use >= 0);
						return_value_info = ssa_var_info[ssa_ops[idx].op1_use];
						if (return_value_info.type & MAY_BE_UNDEF) {
							return_value_info.type &= ~MAY_BE_UNDEF;
							return_value_info.type |= MAY_BE_NULL;
						}
						return_value_info.type &= ~MAY_BE_GUARD;
					}
					break;
				case ZEND_CHECK_FUNC_ARG:
					if (frame
					 && frame->call
					 && frame->call->func) {
						uint32_t arg_num = opline->op2.num;

						if (ARG_SHOULD_BE_SENT_BY_REF(frame->call->func, arg_num)) {
							TRACE_FRAME_SET_LAST_SEND_BY_REF(frame->call);
						} else {
							TRACE_FRAME_SET_LAST_SEND_BY_VAL(frame->call);
						}
					}
					break;
				case ZEND_FETCH_OBJ_FUNC_ARG:
					if (!frame
					 || !frame->call
					 || !frame->call->func
					 || !TRACE_FRAME_IS_LAST_SEND_BY_VAL(frame->call)) {
						break;
					}
					/* break missing intentionally */
				case ZEND_FETCH_OBJ_R:
				case ZEND_FETCH_OBJ_IS:
				case ZEND_FETCH_OBJ_W:
					if (opline->op2_type != IS_CONST
					 || Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) != IS_STRING
					 || Z_STRVAL_P(RT_CONSTANT(opline, opline->op2))[0] == '\0') {
						break;
					}
					ADD_OP1_TRACE_GUARD();
					break;
				case ZEND_INIT_METHOD_CALL:
					if (opline->op2_type != IS_CONST
					 || Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) != IS_STRING) {
						break;
					}
					ADD_OP1_TRACE_GUARD();
					break;
				case ZEND_INIT_DYNAMIC_CALL:
					if (orig_op2_type == IS_OBJECT && op2_ce == zend_ce_closure) {
						ADD_OP2_TRACE_GUARD();
					}
					break;
				default:
					break;
			}
			len = zend_jit_trace_op_len(opline);
			if (ssa->var_info) {
				/* Add statically inferred ranges */
				if (ssa_ops[idx].op1_def >= 0) {
					zend_jit_trace_copy_ssa_var_range(op_array, ssa, ssa_opcodes, tssa, ssa_ops[idx].op1_def);
				}
				if (ssa_ops[idx].op2_def >= 0) {
					zend_jit_trace_copy_ssa_var_range(op_array, ssa, ssa_opcodes, tssa, ssa_ops[idx].op2_def);
				}
				if (ssa_ops[idx].result_def >= 0) {
					zend_jit_trace_copy_ssa_var_range(op_array, ssa, ssa_opcodes, tssa, ssa_ops[idx].result_def);
				}
				if (len == 2 && (opline+1)->opcode == ZEND_OP_DATA) {
					if (ssa_ops[idx+1].op1_def >= 0) {
						zend_jit_trace_copy_ssa_var_range(op_array, ssa, ssa_opcodes, tssa, ssa_ops[idx+1].op1_def);
					}
					if (ssa_ops[idx+1].op2_def >= 0) {
						zend_jit_trace_copy_ssa_var_range(op_array, ssa, ssa_opcodes, tssa, ssa_ops[idx+1].op2_def);
					}
					if (ssa_ops[idx+1].result_def >= 0) {
						zend_jit_trace_copy_ssa_var_range(op_array, ssa, ssa_opcodes, tssa, ssa_ops[idx+1].result_def);
					}
				}
			}
			if (opline->opcode == ZEND_RECV_INIT
			 && !(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
				/* RECV_INIT always copy the constant */
				ssa_var_info[ssa_ops[idx].result_def].type = _const_op_type(RT_CONSTANT(opline, opline->op2));
			} else if ((opline->opcode == ZEND_FE_FETCH_R || opline->opcode == ZEND_FE_FETCH_RW)
			 && ssa_opcodes[idx + 1] == ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value)) {
				if (ssa_ops[idx].op2_use >= 0 && ssa_ops[idx].op2_def >= 0) {
					ssa_var_info[ssa_ops[idx].op2_def] = ssa_var_info[ssa_ops[idx].op2_use];
				}
			} else {
				if (zend_update_type_info(op_array, tssa, script, (zend_op*)opline, ssa_ops + idx, ssa_opcodes, optimization_level) == FAILURE) {
					// TODO:
					assert(0);
				}
			}
			if (ssa->var_info) {
				/* Add statically inferred restrictions */
				if (ssa_ops[idx].op1_def >= 0) {
					if ((opline->opcode == ZEND_SEND_VAR_EX
					  || opline->opcode == ZEND_FETCH_DIM_FUNC_ARG
					  || opline->opcode == ZEND_FETCH_OBJ_FUNC_ARG)
					 && frame
					 && frame->call
					 && frame->call->func
					 && !ARG_SHOULD_BE_SENT_BY_REF(frame->call->func, opline->op2.num)) {
						ssa_var_info[ssa_ops[idx].op1_def] = ssa_var_info[ssa_ops[idx].op1_use];
						if (opline->opcode == ZEND_SEND_VAR_EX) {
							ssa_var_info[ssa_ops[idx].op1_def].type &= ~MAY_BE_GUARD;
						}
						if (ssa_var_info[ssa_ops[idx].op1_def].type & MAY_BE_RC1) {
							ssa_var_info[ssa_ops[idx].op1_def].type |= MAY_BE_RCN;
						}
					} else {
						zend_jit_trace_restrict_ssa_var_info(op_array, ssa, ssa_opcodes, tssa, ssa_ops[idx].op1_def);
					}
				}
				if (ssa_ops[idx].op2_def >= 0) {
					if ((opline->opcode != ZEND_FE_FETCH_R && opline->opcode != ZEND_FE_FETCH_RW)
					 || ssa_opcodes[idx + 1] != ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value)) {
						zend_jit_trace_restrict_ssa_var_info(op_array, ssa, ssa_opcodes, tssa, ssa_ops[idx].op2_def);
					}
				}
				if (ssa_ops[idx].result_def >= 0) {
					zend_jit_trace_restrict_ssa_var_info(op_array, ssa, ssa_opcodes, tssa, ssa_ops[idx].result_def);
				}
			}
			idx++;
			while (len > 1) {
				opline++;
				if (opline->opcode != ZEND_OP_DATA) {
					if (ssa->var_info) {
						/* Add statically inferred ranges */
						if (ssa_ops[idx].op1_def >= 0) {
							zend_jit_trace_copy_ssa_var_range(op_array, ssa, ssa_opcodes, tssa, ssa_ops[idx].op1_def);
						}
						if (ssa_ops[idx].op2_def >= 0) {
							zend_jit_trace_copy_ssa_var_range(op_array, ssa, ssa_opcodes, tssa, ssa_ops[idx].op2_def);
						}
						if (ssa_ops[idx].result_def >= 0) {
							zend_jit_trace_copy_ssa_var_range(op_array, ssa, ssa_opcodes, tssa, ssa_ops[idx].result_def);
						}
					}
					if (opline->opcode == ZEND_RECV_INIT
					 && !(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
						/* RECV_INIT always copy the constant */
						ssa_var_info[ssa_ops[idx].result_def].type = _const_op_type(RT_CONSTANT(opline, opline->op2));
					} else {
						if (zend_update_type_info(op_array, tssa, script, (zend_op*)opline, ssa_ops + idx, ssa_opcodes, optimization_level) == FAILURE) {
							// TODO:
							assert(0);
						}
					}
				}
				if (ssa->var_info) {
					/* Add statically inferred restrictions */
					if (ssa_ops[idx].op1_def >= 0) {
						zend_jit_trace_restrict_ssa_var_info(op_array, ssa, ssa_opcodes, tssa, ssa_ops[idx].op1_def);
					}
					if (ssa_ops[idx].op2_def >= 0) {
						zend_jit_trace_restrict_ssa_var_info(op_array, ssa, ssa_opcodes, tssa, ssa_ops[idx].op2_def);
					}
					if (ssa_ops[idx].result_def >= 0) {
						zend_jit_trace_restrict_ssa_var_info(op_array, ssa, ssa_opcodes, tssa, ssa_ops[idx].result_def);
					}
				}
				idx++;
				len--;
			}

		} else if (p->op == ZEND_JIT_TRACE_ENTER) {
			op_array = p->op_array;
			jit_extension =
				(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
			ssa = &jit_extension->func_info.ssa;

			call = frame->call;
			if (!call) {
				/* Trace missed INIT_FCALL opcode */
				call = top;
				TRACE_FRAME_INIT(call, op_array, 0, 0);
				top = zend_jit_trace_call_frame(top, op_array);
				for (i = 0; i < op_array->last_var + op_array->T; i++) {
					SET_STACK_TYPE(call->stack, i, IS_UNKNOWN);
				}
			} else {
				ZEND_ASSERT(&call->func->op_array == op_array);
			}
			frame->call = call->prev;
			call->prev = frame;
			TRACE_FRAME_SET_RETURN_SSA_VAR(call, find_return_ssa_var(p - 1, ssa_ops + (idx - 1)));
			frame = call;

			level++;
			i = 0;
			v = ZEND_JIT_TRACE_GET_FIRST_SSA_VAR(p->info);
			while (i < op_array->last_var) {
				ssa_vars[v].var = i;
				if (i < op_array->num_args) {
					if (ssa->var_info
					 && zend_jit_trace_copy_ssa_var_info(op_array, ssa, ssa_opcodes, tssa, v)) {
						/* pass */
					} else if (op_array->arg_info) {
						zend_arg_info *arg_info = &op_array->arg_info[i];
						zend_class_entry *ce;
						uint32_t tmp = zend_fetch_arg_info_type(script, arg_info, &ce);

						if (ZEND_ARG_SEND_MODE(arg_info)) {
							tmp |= MAY_BE_REF;
						}
						ssa_var_info[v].type = tmp;
						ssa_var_info[v].ce = ce;
						ssa_var_info[v].is_instanceof = 1;
					} else {
						ssa_var_info[v].type = MAY_BE_RC1 | MAY_BE_RCN | MAY_BE_REF | MAY_BE_ANY  | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF;
					}
				} else if (op_array->function_name
				        && zend_jit_var_may_alias(op_array, ssa, i) == NO_ALIAS) {
					ssa_vars[v].no_val = ssa->vars ? ssa->vars[i].no_val : 0;
					ssa_var_info[v].type = MAY_BE_UNDEF;
				} else {
					ssa_var_info[v].type = MAY_BE_UNDEF | MAY_BE_RC1 | MAY_BE_RCN | MAY_BE_REF | MAY_BE_ANY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF;
				}
				if (!(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)
				 && i < op_array->num_args) {
					/* Propagate argument type */
					ssa_var_info[v].type &= STACK_INFO(frame->stack, i);
				}
				i++;
				v++;
			}
		} else if (p->op == ZEND_JIT_TRACE_BACK) {
			op_array = p->op_array;
			jit_extension =
				(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
			ssa = &jit_extension->func_info.ssa;
			if (level == 0) {
				i = 0;
				v = ZEND_JIT_TRACE_GET_FIRST_SSA_VAR(p->info);
				while (i < op_array->last_var) {
					ssa_vars[v].var = i;
					if (!ssa->var_info
					 || !zend_jit_trace_copy_ssa_var_info(op_array, ssa, ssa_opcodes, tssa, v)) {
						ssa_var_info[v].type = MAY_BE_UNDEF | MAY_BE_RC1 | MAY_BE_RCN | MAY_BE_REF | MAY_BE_ANY  | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF;
					}
					i++;
					v++;
				}
				while (i < op_array->last_var + op_array->T) {
					ssa_vars[v].var = i;
					if (!ssa->var_info
					 || !zend_jit_trace_copy_ssa_var_info(op_array, ssa, ssa_opcodes, tssa, v)) {
						ssa_var_info[v].type = MAY_BE_RC1 | MAY_BE_RCN | MAY_BE_REF | MAY_BE_ANY  | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF;
					}
					i++;
					v++;
				}
				if (return_value_info.type != 0) {
					zend_jit_trace_rec *q = p + 1;
					while (q->op == ZEND_JIT_TRACE_INIT_CALL) {
						q++;
					}
					if (q->op == ZEND_JIT_TRACE_VM) {
						const zend_op *opline = q->opline - 1;
						if (opline->result_type != IS_UNUSED) {
							ssa_var_info[
								ZEND_JIT_TRACE_GET_FIRST_SSA_VAR(p->info) +
								EX_VAR_TO_NUM(opline->result.var)] = return_value_info;
						}
					}
					memset(&return_value_info, 0, sizeof(return_value_info));
				}
			} else {
				level--;
				if (return_value_info.type != 0) {
					if ((p+1)->op == ZEND_JIT_TRACE_VM) {
						const zend_op *opline = (p+1)->opline - 1;
						if (opline->result_type != IS_UNUSED) {
							if (TRACE_FRAME_RETURN_SSA_VAR(frame) >= 0) {
								ssa_var_info[TRACE_FRAME_RETURN_SSA_VAR(frame)] = return_value_info;
							}
						}
					}
					memset(&return_value_info, 0, sizeof(return_value_info));
				}
			}

			top = frame;
			if (frame->prev) {
				frame = frame->prev;
				ZEND_ASSERT(&frame->func->op_array == op_array);
			} else {
				frame = zend_jit_trace_ret_frame(frame, op_array);
				TRACE_FRAME_INIT(frame, op_array, 0, 0);
				TRACE_FRAME_SET_RETURN_SSA_VAR(frame, -1);
				for (i = 0; i < op_array->last_var + op_array->T; i++) {
					SET_STACK_TYPE(frame->stack, i, IS_UNKNOWN);
				}
			}

		} else if (p->op == ZEND_JIT_TRACE_INIT_CALL) {
			call = top;
			TRACE_FRAME_INIT(call, p->func, 0, 0);
			call->prev = frame->call;
			frame->call = call;
			top = zend_jit_trace_call_frame(top, p->op_array);
			if (p->func && p->func->type == ZEND_USER_FUNCTION) {
				for (i = 0; i < p->op_array->last_var + p->op_array->T; i++) {
					SET_STACK_INFO(call->stack, i, -1);
				}
			}
		} else if (p->op == ZEND_JIT_TRACE_DO_ICALL) {
			call = frame->call;
			if (call) {
				top = call;
				frame->call = call->prev;
			}
		} else if (p->op == ZEND_JIT_TRACE_END) {
			break;
		}
	}

	if (trace_buffer->stop == ZEND_JIT_TRACE_STOP_LOOP) {
		/* Propagate guards through Phi sources */
		zend_ssa_phi *phi = tssa->blocks[1].phis;

		op_array = trace_buffer->op_array;
		jit_extension =
			(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
		ssa = &jit_extension->func_info.ssa;

		while (phi) {
			uint32_t t  = ssa_var_info[phi->ssa_var].type;
			uint32_t t0 = ssa_var_info[phi->sources[0]].type;
			uint32_t t1 = ssa_var_info[phi->sources[1]].type;

			if (t & MAY_BE_GUARD) {
				if (zend_jit_var_may_alias(op_array, ssa, phi->sources[0]) != NO_ALIAS) {
					/* pass */
				} else if (((t0 | t1) & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF)) == (t & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF))) {
					if (!((t0 | t1) & MAY_BE_GUARD)) {
						ssa_var_info[phi->ssa_var].type = t & ~MAY_BE_GUARD;
					}
				} else if ((t1 & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF)) == (t & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF))) {
					if (!(t1 & MAY_BE_GUARD)
					 || is_checked_guard(tssa, ssa_opcodes, phi->sources[1], phi->ssa_var)) {
						ssa_var_info[phi->ssa_var].type = t & ~MAY_BE_GUARD;
						ssa_var_info[phi->sources[0]].type = t | MAY_BE_GUARD;
					}
				} else {
					if ((t0 & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF)) != (t & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF))) {
						ssa_var_info[phi->sources[0]].type = MAY_BE_GUARD | (t & t0);
					}
					if ((t1 & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF)) != (t & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF))) {
						if (is_checked_guard(tssa, ssa_opcodes, phi->sources[1], phi->ssa_var)) {
							ssa_var_info[phi->sources[1]].type = MAY_BE_GUARD | (t & t1);
							ssa_var_info[phi->ssa_var].type = t & ~MAY_BE_GUARD;
						}
					}
				}
			}
			phi = phi->next;
		}
	}

	if (UNEXPECTED(JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_TSSA)) {
		if (parent_trace) {
			fprintf(stderr, "---- TRACE %d TSSA start (side trace %d/%d) %s%s%s() %s:%d\n",
				ZEND_JIT_TRACE_NUM,
				parent_trace,
				exit_num,
				trace_buffer->op_array->scope ? ZSTR_VAL(trace_buffer->op_array->scope->name) : "",
				trace_buffer->op_array->scope ? "::" : "",
				trace_buffer->op_array->function_name ?
					ZSTR_VAL(trace_buffer->op_array->function_name) : "$main",
				ZSTR_VAL(trace_buffer->op_array->filename),
				trace_buffer[1].opline->lineno);
		} else {
			fprintf(stderr, "---- TRACE %d TSSA start (%s) %s%s%s() %s:%d\n",
				ZEND_JIT_TRACE_NUM,
				zend_jit_trace_star_desc(trace_buffer->start),
				trace_buffer->op_array->scope ? ZSTR_VAL(trace_buffer->op_array->scope->name) : "",
				trace_buffer->op_array->scope ? "::" : "",
				trace_buffer->op_array->function_name ?
					ZSTR_VAL(trace_buffer->op_array->function_name) : "$main",
				ZSTR_VAL(trace_buffer->op_array->filename),
				trace_buffer[1].opline->lineno);
		}
		zend_jit_dump_trace(trace_buffer, tssa);
		if (trace_buffer->stop == ZEND_JIT_TRACE_STOP_LINK) {
			uint32_t idx = trace_buffer[1].last;
			uint32_t link_to = zend_jit_find_trace(trace_buffer[idx].opline->handler);
			fprintf(stderr, "---- TRACE %d TSSA stop (link to %d)\n",
				ZEND_JIT_TRACE_NUM,
				link_to);
		} else {
			fprintf(stderr, "---- TRACE %d TSSA stop (%s)\n",
				ZEND_JIT_TRACE_NUM,
				zend_jit_trace_stop_description[trace_buffer->stop]);
		}
	}

	return tssa;
}

static void zend_jit_close_var(zend_jit_trace_stack *stack, uint32_t n, int *start, int *end, uint8_t *flags, int line)
{
	int32_t var = STACK_VAR(stack, n);

	if (var >= 0 && start[var] >= 0 && !(flags[var] & ZREG_LAST_USE)) {
		// TODO: shrink interval to last side exit ????
		end[var] = line;
	}
}

static void zend_jit_trace_use_var(int line, int var, int def, int use_chain, int *start, int *end, uint8_t *flags, const zend_ssa *ssa, const zend_op **ssa_opcodes, const zend_op_array *op_array, const zend_ssa *op_array_ssa)
{
	ZEND_ASSERT(start[var] >= 0);
	ZEND_ASSERT(!(flags[var] & ZREG_LAST_USE));
	end[var] = line;
	if (def >= 0) {
		flags[var] |= ZREG_LAST_USE;
	} else if (use_chain < 0 && (flags[var] & (ZREG_LOAD|ZREG_STORE))) {
		flags[var] |= ZREG_LAST_USE;
	} else if (use_chain >= 0 && !zend_ssa_is_no_val_use(ssa_opcodes[use_chain], ssa->ops + use_chain, var)) {
		/* pass */
	} else if (op_array_ssa->vars) {
		uint32_t use = ssa_opcodes[line] - op_array->opcodes;

		if (ssa->ops[line].op1_use == var) {
			if (zend_ssa_is_last_use(op_array, op_array_ssa, op_array_ssa->ops[use].op1_use, use)) {
				flags[var] |= ZREG_LAST_USE;
			}
		} else if (ssa->ops[line].op2_use == var) {
			if (zend_ssa_is_last_use(op_array, op_array_ssa, op_array_ssa->ops[use].op2_use, use)) {
				flags[var] |= ZREG_LAST_USE;
			}
		} else if (ssa->ops[line].result_use == var) {
			if (zend_ssa_is_last_use(op_array, op_array_ssa, op_array_ssa->ops[use].result_use, use)) {
				flags[var] |= ZREG_LAST_USE;
			}
		}
	}
}

static zend_lifetime_interval** zend_jit_trace_allocate_registers(zend_jit_trace_rec *trace_buffer, zend_ssa *ssa, uint32_t parent_trace, uint32_t exit_num)
{
	const zend_op **ssa_opcodes = ((zend_tssa*)ssa)->tssa_opcodes;
	zend_jit_trace_rec *p;
	const zend_op_array *op_array;
	zend_jit_op_array_trace_extension *jit_extension;
	const zend_ssa *op_array_ssa;
	const zend_ssa_op *ssa_op;
	int i, j, idx, count, level;
	int *start, *end;
	uint8_t *flags;
	const zend_op_array **vars_op_array;
	zend_lifetime_interval **intervals, *list, *ival;
	void *checkpoint;
	zend_jit_trace_stack_frame *frame;
	zend_jit_trace_stack *stack;
	uint32_t parent_vars_count = parent_trace ?
		zend_jit_traces[parent_trace].exit_info[exit_num].stack_size : 0;
	zend_jit_trace_stack *parent_stack = parent_trace ?
		zend_jit_traces[parent_trace].stack_map +
		zend_jit_traces[parent_trace].exit_info[exit_num].stack_offset : NULL;
	ALLOCA_FLAG(use_heap);

	ZEND_ASSERT(ssa->var_info != NULL);

	start = do_alloca(sizeof(int) * ssa->vars_count * 2 +
		ZEND_MM_ALIGNED_SIZE(sizeof(uint8_t) * ssa->vars_count) +
		ZEND_MM_ALIGNED_SIZE(sizeof(zend_op_array*) * ssa->vars_count),
		use_heap);
	if (!start) {
		return NULL;
	}
	end = start + ssa->vars_count;
	flags = (uint8_t*)(end + ssa->vars_count);
	vars_op_array = (const zend_op_array**)(flags + ZEND_MM_ALIGNED_SIZE(sizeof(uint8_t) * ssa->vars_count));

	memset(start, -1, sizeof(int) * ssa->vars_count * 2);
	memset(flags, 0, sizeof(uint8_t) * ssa->vars_count);
	memset(ZEND_VOIDP(vars_op_array), 0, sizeof(zend_op_array*) * ssa->vars_count);

	op_array = trace_buffer->op_array;
	jit_extension =
		(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
	op_array_ssa = &jit_extension->func_info.ssa;
	frame = JIT_G(current_frame);
	frame->prev = NULL;
	frame->func = (const zend_function*)op_array;
	stack = frame->stack;

	count = 0;

	i = 0;
	j = op_array->last_var;
	if (trace_buffer->start != ZEND_JIT_TRACE_START_ENTER) {
		j += op_array->T;
	}
	while (i < j) {
		SET_STACK_VAR(stack, i, i);
		vars_op_array[i] = op_array;
		/* We don't start intervals for variables used in Phi */
		if ((ssa->vars[i].use_chain >= 0 /*|| ssa->vars[i].phi_use_chain*/)
		 && zend_jit_var_supports_reg(ssa, i)
		 && zend_jit_var_may_alias(op_array, op_array_ssa, i) == NO_ALIAS) {
			start[i] = 0;
			if (i < parent_vars_count
			 && STACK_REG(parent_stack, i) != ZREG_NONE
			 && STACK_REG(parent_stack, i) < ZREG_NUM) {
				/* We will try to reuse register from parent trace */
				count += 2;
			} else {
				flags[i] = ZREG_LOAD;
				count++;
			}
		}
		i++;
	}

	if (trace_buffer->start == ZEND_JIT_TRACE_START_ENTER) {
		j = op_array->last_var + op_array->T;
		while (i < j) {
			SET_STACK_VAR(stack, i, -1);
			i++;
		}
	}

	if (trace_buffer->stop == ZEND_JIT_TRACE_STOP_LOOP) {
		zend_ssa_phi *phi = ssa->blocks[1].phis;

		while (phi) {
			SET_STACK_VAR(stack, phi->var, phi->ssa_var);
			vars_op_array[phi->ssa_var] = op_array;
			if (ssa->vars[phi->ssa_var].use_chain >= 0
			 && zend_jit_var_supports_reg(ssa, phi->ssa_var)
			 && zend_jit_var_may_alias(op_array, op_array_ssa, phi->sources[0]) == NO_ALIAS) {
				start[phi->ssa_var] = 0;
				count++;
			}
			phi = phi->next;
		}
	}

	p = trace_buffer + ZEND_JIT_TRACE_START_REC_SIZE;
	level = 0;
	ssa_op = ssa->ops;
	idx = 0;
	for (;;p++) {
		if (p->op == ZEND_JIT_TRACE_VM) {
			const zend_op *opline = p->opline;
			int len;
			zend_bool support_opline;

			support_opline =
				zend_jit_opline_supports_reg(op_array, ssa, opline, ssa_op, p);
			if (ssa_op->op1_use >= 0
			 && start[ssa_op->op1_use] >= 0
			 && !zend_ssa_is_no_val_use(opline, ssa_op, ssa_op->op1_use)) {
				if (support_opline) {
					zend_jit_trace_use_var(idx, ssa_op->op1_use, ssa_op->op1_def, ssa_op->op1_use_chain, start, end, flags, ssa, ssa_opcodes, op_array, op_array_ssa);
					if (opline->op1_type != IS_CV) {
						if (opline->opcode == ZEND_CASE
						 || opline->opcode == ZEND_CASE_STRICT
						 || opline->opcode == ZEND_SWITCH_LONG
						 || opline->opcode == ZEND_MATCH
						 || opline->opcode == ZEND_FETCH_LIST_R
						 || opline->opcode == ZEND_COPY_TMP
						 || opline->opcode == ZEND_SWITCH_STRING
						 || opline->opcode == ZEND_FE_FETCH_R
						 || opline->opcode == ZEND_FE_FETCH_RW
						 || opline->opcode == ZEND_FETCH_LIST_W
						 || opline->opcode == ZEND_VERIFY_RETURN_TYPE
						 || opline->opcode == ZEND_BIND_LEXICAL
						 || opline->opcode == ZEND_ROPE_ADD) {
							/* The value is kept alive and may be used outside of the trace */
							flags[ssa_op->op1_use] |= ZREG_STORE;
						} else {
							flags[ssa_op->op1_use] |= ZREG_LAST_USE;
						}
					}
				} else {
					start[ssa_op->op1_use] = -1;
					end[ssa_op->op1_use] = -1;
					count--;
				}
			}
			if (ssa_op->op2_use >= 0
			 && ssa_op->op2_use != ssa_op->op1_use
			 && start[ssa_op->op2_use] >= 0
			 && !zend_ssa_is_no_val_use(opline, ssa_op, ssa_op->op2_use)) {
				if (support_opline) {
					zend_jit_trace_use_var(idx, ssa_op->op2_use, ssa_op->op2_def, ssa_op->op2_use_chain, start, end, flags, ssa, ssa_opcodes, op_array, op_array_ssa);
					if (opline->op2_type != IS_CV) {
						flags[ssa_op->op2_use] |= ZREG_LAST_USE;
					}
				} else {
					start[ssa_op->op2_use] = -1;
					end[ssa_op->op2_use] = -1;
					count--;
				}
			}
			if (ssa_op->result_use >= 0
			 && ssa_op->result_use != ssa_op->op1_use
			 && ssa_op->result_use != ssa_op->op2_use
			 && start[ssa_op->result_use] >= 0
			 && !zend_ssa_is_no_val_use(opline, ssa_op, ssa_op->result_use)) {
				if (support_opline) {
					zend_jit_trace_use_var(idx, ssa_op->result_use, ssa_op->result_def, ssa_op->res_use_chain, start, end, flags, ssa, ssa_opcodes, op_array, op_array_ssa);
				} else {
					start[ssa_op->result_use] = -1;
					end[ssa_op->result_use] = -1;
					count--;
				}
			}

			if (ssa_op->op1_def >= 0) {
				zend_jit_close_var(stack, EX_VAR_TO_NUM(opline->op1.var), start, end, flags, idx);
				SET_STACK_VAR(stack, EX_VAR_TO_NUM(opline->op1.var), ssa_op->op1_def);
			}
			if (ssa_op->op2_def >= 0) {
				zend_jit_close_var(stack, EX_VAR_TO_NUM(opline->op2.var), start, end, flags, idx);
				SET_STACK_VAR(stack, EX_VAR_TO_NUM(opline->op2.var), ssa_op->op2_def);
			}
			if (ssa_op->result_def >= 0) {
				zend_jit_close_var(stack, EX_VAR_TO_NUM(opline->result.var), start, end, flags, idx);
				SET_STACK_VAR(stack, EX_VAR_TO_NUM(opline->result.var), ssa_op->result_def);
			}

			if (support_opline) {
				if (ssa_op->result_def >= 0
				 && (ssa->vars[ssa_op->result_def].use_chain >= 0
			      || ssa->vars[ssa_op->result_def].phi_use_chain)
				 && zend_jit_var_supports_reg(ssa, ssa_op->result_def)
				 && zend_jit_var_may_alias(op_array, op_array_ssa, EX_VAR_TO_NUM(opline->result.var)) == NO_ALIAS) {
					if (!(ssa->var_info[ssa_op->result_def].type & MAY_BE_GUARD)
					 || opline->opcode == ZEND_PRE_INC
					 || opline->opcode == ZEND_PRE_DEC
					 || opline->opcode == ZEND_POST_INC
					 || opline->opcode == ZEND_POST_DEC
					 || opline->opcode == ZEND_ADD
					 || opline->opcode == ZEND_SUB
					 || opline->opcode == ZEND_MUL
					 || opline->opcode == ZEND_FETCH_DIM_R) {
						if (!(ssa->var_info[ssa_op->result_def].type & MAY_BE_DOUBLE)
						 || (opline->opcode != ZEND_PRE_INC && opline->opcode != ZEND_PRE_DEC)) {
							start[ssa_op->result_def] = idx;
							vars_op_array[ssa_op->result_def] = op_array;
							count++;
						}
					}
				}
				if (ssa_op->op1_def >= 0
				 && (ssa->vars[ssa_op->op1_def].use_chain >= 0
			      || ssa->vars[ssa_op->op1_def].phi_use_chain)
				 && zend_jit_var_supports_reg(ssa, ssa_op->op1_def)
				 && zend_jit_var_may_alias(op_array, op_array_ssa, EX_VAR_TO_NUM(opline->op1.var)) == NO_ALIAS) {
					start[ssa_op->op1_def] = idx;
					vars_op_array[ssa_op->op1_def] = op_array;
					count++;
				}
				if (ssa_op->op2_def >= 0
				 && (ssa->vars[ssa_op->op2_def].use_chain >= 0
			      || ssa->vars[ssa_op->op2_def].phi_use_chain)
				 && zend_jit_var_supports_reg(ssa, ssa_op->op2_def)
				 && zend_jit_var_may_alias(op_array, op_array_ssa, EX_VAR_TO_NUM(opline->op2.var)) == NO_ALIAS) {
					start[ssa_op->op2_def] = idx;
					vars_op_array[ssa_op->op2_def] = op_array;
					count++;
				}
			}

			len = zend_jit_trace_op_len(opline);
			switch (opline->opcode) {
				case ZEND_ASSIGN_DIM:
				case ZEND_ASSIGN_OBJ:
				case ZEND_ASSIGN_STATIC_PROP:
				case ZEND_ASSIGN_DIM_OP:
				case ZEND_ASSIGN_OBJ_OP:
				case ZEND_ASSIGN_STATIC_PROP_OP:
				case ZEND_ASSIGN_OBJ_REF:
				case ZEND_ASSIGN_STATIC_PROP_REF:
					/* OP_DATA */
					ssa_op++;
					opline++;
					if (ssa_op->op1_use >= 0
					 && start[ssa_op->op1_use] >= 0
					 && !zend_ssa_is_no_val_use(opline, ssa_op, ssa_op->op1_use)) {
						if (support_opline) {
							zend_jit_trace_use_var(idx, ssa_op->op1_use, ssa_op->op1_def, ssa_op->op1_use_chain, start, end, flags, ssa, ssa_opcodes, op_array, op_array_ssa);
							if (opline->op1_type != IS_CV) {
								flags[ssa_op->op1_use] |= ZREG_LAST_USE;
							}
						} else {
							start[ssa_op->op1_use] = -1;
							end[ssa_op->op1_use] = -1;
							count--;
						}
					}
					if (ssa_op->op1_def >= 0) {
						zend_jit_close_var(stack, EX_VAR_TO_NUM(opline->op1.var), start, end, flags, idx);
						SET_STACK_VAR(stack, EX_VAR_TO_NUM(opline->op1.var), ssa_op->op1_def);
						if (support_opline
						 && (ssa->vars[ssa_op->op1_def].use_chain >= 0
					      || ssa->vars[ssa_op->op1_def].phi_use_chain)
						 && zend_jit_var_supports_reg(ssa, ssa_op->op1_def)
						 && zend_jit_var_may_alias(op_array, op_array_ssa, EX_VAR_TO_NUM(opline->op1.var)) == NO_ALIAS) {
							start[ssa_op->op1_def] = idx;
							vars_op_array[ssa_op->op1_def] = op_array;
							count++;
						}
					}
					ssa_op++;
					opline++;
					idx+=2;
					break;
				case ZEND_RECV_INIT:
				    ssa_op++;
					opline++;
					idx++;
					while (opline->opcode == ZEND_RECV_INIT) {
						/* RECV_INIT doesn't support registers */
						if (ssa_op->result_def >= 0) {
							zend_jit_close_var(stack, EX_VAR_TO_NUM(opline->result.var), start, end, flags, idx);
							SET_STACK_VAR(stack, EX_VAR_TO_NUM(opline->result.var), ssa_op->result_def);
						}
						ssa_op++;
						opline++;
						idx++;
					}
					break;
				case ZEND_BIND_GLOBAL:
					ssa_op++;
					opline++;
					idx++;
					while (opline->opcode == ZEND_BIND_GLOBAL) {
						/* BIND_GLOBAL doesn't support registers */
						if (ssa_op->op1_def >= 0) {
							zend_jit_close_var(stack, EX_VAR_TO_NUM(opline->op1.var), start, end, flags, idx);
							SET_STACK_VAR(stack, EX_VAR_TO_NUM(opline->op1.var), ssa_op->op1_def);
						}
						ssa_op++;
						opline++;
						idx++;
					}
					break;
				default:
					ssa_op += len;
					idx += len;
					break;
			}
		} else if (p->op == ZEND_JIT_TRACE_ENTER) {
			/* New call frames */
			zend_jit_trace_stack_frame *prev_frame = frame;

			frame = zend_jit_trace_call_frame(frame, op_array);
			frame->prev = prev_frame;
			frame->func = (const zend_function*)p->op_array;
			stack = frame->stack;
			op_array = p->op_array;
			jit_extension =
				(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
			op_array_ssa = &jit_extension->func_info.ssa;
			j = ZEND_JIT_TRACE_GET_FIRST_SSA_VAR(p->info);
			for (i = 0; i < op_array->last_var; i++) {
				SET_STACK_VAR(stack, i, j);
				vars_op_array[j] = op_array;
				if (ssa->vars[j].use_chain >= 0
				 && zend_jit_var_supports_reg(ssa, j)
				 && zend_jit_var_may_alias(op_array, op_array_ssa, i) == NO_ALIAS) {
					start[j] = idx;
					flags[j] = ZREG_LOAD;
					count++;
				}
				j++;
			}
			for (i = op_array->last_var; i < op_array->last_var + op_array->T; i++) {
				SET_STACK_VAR(stack, i, -1);
			}
			level++;
		} else if (p->op == ZEND_JIT_TRACE_BACK) {
			/* Close exiting call frames */
			for (i = 0; i < op_array->last_var; i++) {
				zend_jit_close_var(stack, i, start, end, flags, idx-1);
			}
			op_array = p->op_array;
			jit_extension =
				(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
			op_array_ssa = &jit_extension->func_info.ssa;
			frame = zend_jit_trace_ret_frame(frame, op_array);
			stack = frame->stack;
			if (level == 0) {
				/* New return frames */
				frame->prev = NULL;
				frame->func = (const zend_function*)op_array;
				j = ZEND_JIT_TRACE_GET_FIRST_SSA_VAR(p->info);
				for (i = 0; i < op_array->last_var + op_array->T; i++) {
					SET_STACK_VAR(stack, i, j);
					vars_op_array[j] = op_array;
					if (ssa->vars[j].use_chain >= 0
					 && zend_jit_var_supports_reg(ssa, j)
					 && zend_jit_var_may_alias(op_array, op_array_ssa, i) == NO_ALIAS) {
						start[j] = idx;
						flags[j] = ZREG_LOAD;
						count++;
					}
					j++;
				}
			} else {
				level--;
			}
		} else if (p->op == ZEND_JIT_TRACE_END) {
			break;
		}
	}

	if (trace_buffer->stop == ZEND_JIT_TRACE_STOP_LOOP) {
		zend_ssa_phi *phi = ssa->blocks[1].phis;

		while (phi) {
			i = phi->sources[1];
			if (start[i] >= 0) {
				end[i] = idx;
				flags[i] &= ~ZREG_LAST_USE;
			}
			phi = phi->next;
		}

		for (i = 0; i < op_array->last_var; i++) {
			if (start[i] >= 0 && !ssa->vars[i].phi_use_chain) {
				end[i] = idx;
				flags[i] &= ~ZREG_LAST_USE;
			}
		}
	} else {
		for (i = 0; i < op_array->last_var; i++) {
			zend_jit_close_var(stack, i, start, end, flags, idx);
		}
		while (frame->prev) {
			frame = frame->prev;
			op_array = &frame->func->op_array;
			stack = frame->stack;
			for (i = 0; i < op_array->last_var; i++) {
				zend_jit_close_var(stack, i, start, end, flags, idx);
			}
		}
	}

	if (!count) {
		free_alloca(start, use_heap);
		return NULL;
	}

	checkpoint = zend_arena_checkpoint(CG(arena));
	intervals = zend_arena_calloc(&CG(arena), ssa->vars_count, sizeof(zend_lifetime_interval));
	memset(intervals, 0, sizeof(zend_lifetime_interval*) * ssa->vars_count);
	list = zend_arena_alloc(&CG(arena), sizeof(zend_lifetime_interval) * count);
	j = 0;
	for (i = 0; i < ssa->vars_count; i++) {
		if (start[i] >= 0 && end[i] >= 0) {
			ZEND_ASSERT(j < count);
			if ((flags[i] & ZREG_LOAD) &&
			    (flags[i] & ZREG_LAST_USE) &&
			    end[i] == ssa->vars[i].use_chain) {
				/* skip life range with single use */
				continue;
			}
			intervals[i] = &list[j];
			list[j].ssa_var = i;
			list[j].reg = ZREG_NONE;
			list[j].flags = flags[i];
			list[j].range.start = start[i];
			list[j].range.end = end[i];
			list[j].range.next = NULL;
			list[j].hint = NULL;
			list[j].used_as_hint = NULL;
			list[j].list_next = NULL;
			j++;
		}
	}
	count = j;
	free_alloca(start, use_heap);
	start = end = NULL;

	if (!count) {
		zend_arena_release(&CG(arena), checkpoint);
		return NULL;
	}

	/* Add hints */
	if (parent_vars_count) {
		i = trace_buffer->op_array->last_var;
		if (trace_buffer->start != ZEND_JIT_TRACE_START_ENTER) {
			i += trace_buffer->op_array->T;
		}
		if ((uint32_t)i > parent_vars_count) {
			i = parent_vars_count;
		}
		while (i > 0) {
			i--;
			if (intervals[i]
			 && STACK_REG(parent_stack, i) != ZREG_NONE
			 && STACK_REG(parent_stack, i) < ZREG_NUM) {
				list[j].ssa_var = - 1;
				list[j].reg = STACK_REG(parent_stack, i);
				list[j].flags = 0;
				list[j].range.start = -1;
				list[j].range.end = -1;
				list[j].range.next = NULL;
				list[j].hint = NULL;
				list[j].used_as_hint = NULL;
				list[j].list_next = NULL;
				intervals[i]->hint = &list[j];
				j++;
			}
		}
	}

	if (trace_buffer->stop == ZEND_JIT_TRACE_STOP_LOOP) {
		zend_ssa_phi *phi = ssa->blocks[1].phis;

		while (phi) {
			if (intervals[phi->ssa_var]) {
				if (intervals[phi->sources[1]]) {
					intervals[phi->sources[1]]->hint = intervals[phi->ssa_var];
				}
			}
			phi = phi->next;
		}
	}

	for (i = 0; i < ssa->vars_count; i++) {
		if (intervals[i] && !intervals[i]->hint) {

			if (ssa->vars[i].definition >= 0) {
				uint32_t line = ssa->vars[i].definition;
				const zend_op *opline = ssa_opcodes[line];

				switch (opline->opcode) {
					case ZEND_QM_ASSIGN:
					case ZEND_POST_INC:
					case ZEND_POST_DEC:
						if (ssa->ops[line].op1_use >= 0 &&
						    intervals[ssa->ops[line].op1_use] &&
						    (i == ssa->ops[line].op1_def ||
						     (i == ssa->ops[line].result_def &&
						      (ssa->ops[line].op1_def < 0 ||
						       !intervals[ssa->ops[line].op1_def])))) {
							zend_jit_add_hint(intervals, i, ssa->ops[line].op1_use);
						}
						break;
					case ZEND_SEND_VAR:
					case ZEND_PRE_INC:
					case ZEND_PRE_DEC:
						if (i == ssa->ops[line].op1_def &&
						    ssa->ops[line].op1_use >= 0 &&
						    intervals[ssa->ops[line].op1_use]) {
							zend_jit_add_hint(intervals, i, ssa->ops[line].op1_use);
						}
						break;
					case ZEND_ASSIGN:
						if (ssa->ops[line].op2_use >= 0 &&
						    intervals[ssa->ops[line].op2_use] &&
						    (i == ssa->ops[line].op2_def ||
							 (i == ssa->ops[line].op1_def &&
						      (ssa->ops[line].op2_def < 0 ||
						       !intervals[ssa->ops[line].op2_def])) ||
							 (i == ssa->ops[line].result_def &&
						      (ssa->ops[line].op2_def < 0 ||
						       !intervals[ssa->ops[line].op2_def]) &&
						      (ssa->ops[line].op1_def < 0 ||
						       !intervals[ssa->ops[line].op1_def])))) {
							zend_jit_add_hint(intervals, i, ssa->ops[line].op2_use);
						}
						break;
				}
			}
		}
	}

	list = zend_jit_sort_intervals(intervals, ssa->vars_count);

	if (list) {
		ival = list;
		while (ival) {
			if (ival->hint) {
				ival->hint->used_as_hint = ival;
			}
			ival = ival->list_next;
		}
	}

	if (list) {
		if (JIT_G(debug) & ZEND_JIT_DEBUG_REG_ALLOC) {
			fprintf(stderr, "---- TRACE %d Live Ranges\n", ZEND_JIT_TRACE_NUM);
			ival = list;
			while (ival) {
				zend_jit_dump_lifetime_interval(vars_op_array[ival->ssa_var], ssa, ival);
				ival = ival->list_next;
			}
		}
	}

	/* Linear Scan Register Allocation (op_array is not actually used, only fn_flags matters) */
	list = zend_jit_linear_scan(&dummy_op_array, ssa_opcodes, ssa, list);

	if (list) {
		zend_lifetime_interval *ival, *next;

		memset(intervals, 0, ssa->vars_count * sizeof(zend_lifetime_interval*));
		ival = list;
		count = 0;
		while (ival != NULL) {
			ZEND_ASSERT(ival->reg != ZREG_NONE);
			count++;
			next = ival->list_next;
			ival->list_next = intervals[ival->ssa_var];
			intervals[ival->ssa_var] = ival;
			ival = next;
		}

		if (!count) {
			zend_arena_release(&CG(arena), checkpoint);
			return NULL;
		}

		/* Add LOAD flag to registers that can't reuse register from parent trace */
		if (parent_vars_count) {
			i = trace_buffer->op_array->last_var;
			if (trace_buffer->start != ZEND_JIT_TRACE_START_ENTER) {
				i += trace_buffer->op_array->T;
			}
			if ((uint32_t)i > parent_vars_count) {
				i = parent_vars_count;
			}
			while (i > 0) {
				i--;
				if (intervals[i] && intervals[i]->reg != STACK_REG(parent_stack, i)) {
					intervals[i]->flags |= ZREG_LOAD;
				}
			}
		}

		/* SSA resolution */
		if (trace_buffer->stop == ZEND_JIT_TRACE_STOP_LOOP) {
			zend_ssa_phi *phi = ssa->blocks[1].phis;

			while (phi) {
				int def = phi->ssa_var;
				int use = phi->sources[1];

				if (intervals[def]) {
					if (!intervals[use]) {
						intervals[def]->flags |= ZREG_LOAD;
						if ((intervals[def]->flags & ZREG_LAST_USE)
						 && ssa->vars[def].use_chain >= 0
						 && ssa->vars[def].use_chain == intervals[def]->range.end) {
							/* remove interval used once */
							intervals[def] = NULL;
							count--;
						}
					} else if (intervals[def]->reg != intervals[use]->reg) {
						intervals[def]->flags |= ZREG_LOAD;
						if (ssa->vars[use].use_chain >= 0) {
							intervals[use]->flags |= ZREG_STORE;
						} else {
							intervals[use] = NULL;
							count--;
						}
					} else {
						use = phi->sources[0];
						ZEND_ASSERT(!intervals[use]);
						intervals[use] = zend_arena_alloc(&CG(arena), sizeof(zend_lifetime_interval));
						intervals[use]->ssa_var = phi->sources[0];
						intervals[use]->reg = intervals[def]->reg;
						intervals[use]->flags = ZREG_LOAD;
						intervals[use]->range.start = 0;
						intervals[use]->range.end = 0;
						intervals[use]->range.next = NULL;
						intervals[use]->hint = NULL;
						intervals[use]->used_as_hint = NULL;
						intervals[use]->list_next = NULL;
					}
				} else if (intervals[use] && !ssa->vars[phi->ssa_var].no_val) {
					if (ssa->vars[use].use_chain >= 0) {
						intervals[use]->flags |= ZREG_STORE;
					} else {
						intervals[use] = NULL;
						count--;
					}
				}
				phi = phi->next;
			}
		}

		if (!count) {
			zend_arena_release(&CG(arena), checkpoint);
			return NULL;
		}

		if (JIT_G(debug) & ZEND_JIT_DEBUG_REG_ALLOC) {
			fprintf(stderr, "---- TRACE %d Allocated Live Ranges\n", ZEND_JIT_TRACE_NUM);
			for (i = 0; i < ssa->vars_count; i++) {
				ival = intervals[i];
				while (ival) {
					zend_jit_dump_lifetime_interval(vars_op_array[ival->ssa_var], ssa, ival);
					ival = ival->list_next;
				}
			}
		}

		return intervals;
	}

	zend_arena_release(&CG(arena), checkpoint); //???
	return NULL;
}

static void zend_jit_trace_clenup_stack(zend_jit_trace_stack *stack, const zend_op *opline, const zend_ssa_op *ssa_op, const zend_ssa *ssa, zend_lifetime_interval **ra)
{
	uint32_t line = ssa_op - ssa->ops;

	if (ssa_op->op1_use >= 0
	 && ra[ssa_op->op1_use]
	 && ra[ssa_op->op1_use]->range.end == line) {
		SET_STACK_REG(stack, EX_VAR_TO_NUM(opline->op1.var), ZREG_NONE);
	}
	if (ssa_op->op2_use >= 0
	 && ra[ssa_op->op2_use]
	 && ra[ssa_op->op2_use]->range.end == line) {
		SET_STACK_REG(stack, EX_VAR_TO_NUM(opline->op2.var), ZREG_NONE);
	}
	if (ssa_op->result_use >= 0
	 && ra[ssa_op->result_use]
	 && ra[ssa_op->result_use]->range.end == line) {
		SET_STACK_REG(stack, EX_VAR_TO_NUM(opline->result.var), ZREG_NONE);
	}
}

static void zend_jit_trace_setup_ret_counter(const zend_op *opline, size_t offset)
{
	zend_op *next_opline = (zend_op*)(opline + 1);

	if (JIT_G(hot_return) && !ZEND_OP_TRACE_INFO(next_opline, offset)->trace_flags) {
		ZEND_ASSERT(zend_jit_ret_trace_counter_handler != NULL);
		if (!ZEND_OP_TRACE_INFO(next_opline, offset)->counter) {
			ZEND_OP_TRACE_INFO(next_opline, offset)->counter =
				&zend_jit_hot_counters[ZEND_JIT_COUNTER_NUM];
			ZEND_JIT_COUNTER_NUM = (ZEND_JIT_COUNTER_NUM + 1) % ZEND_HOT_COUNTERS_COUNT;
		}
		ZEND_OP_TRACE_INFO(next_opline, offset)->trace_flags = ZEND_JIT_TRACE_START_RETURN;
		next_opline->handler = (const void*)zend_jit_ret_trace_counter_handler;
	}
}

static zend_bool zend_jit_may_delay_fetch_this(zend_ssa *ssa, const zend_op **ssa_opcodes, int var)
{
	int i;
	int use = ssa->vars[var].use_chain;
	const zend_op *opline;

	if (use < 0
	 || ssa->vars[var].phi_use_chain
	 || ssa->ops[use].op1_use != var
	 || ssa->ops[use].op1_use_chain != -1) {
		return 0;
	}

	opline = ssa_opcodes[use];
	if (opline->opcode == ZEND_INIT_METHOD_CALL) {
		return (opline->op2_type == IS_CONST &&
			Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) == IS_STRING);
	} else if (opline->opcode == ZEND_FETCH_OBJ_FUNC_ARG) {
		if (!JIT_G(current_frame)
		 || !JIT_G(current_frame)->call
		 || !JIT_G(current_frame)->call->func
		 || !TRACE_FRAME_IS_LAST_SEND_BY_VAL(JIT_G(current_frame)->call)) {
			return 0;
		}
	} else if (opline->opcode != ZEND_FETCH_OBJ_R
			&& opline->opcode != ZEND_FETCH_OBJ_IS
			&& opline->opcode != ZEND_FETCH_OBJ_W
			&& opline->opcode != ZEND_ASSIGN_OBJ
			&& opline->opcode != ZEND_ASSIGN_OBJ_OP
			&& opline->opcode != ZEND_PRE_INC_OBJ
			&& opline->opcode != ZEND_PRE_DEC_OBJ
			&& opline->opcode != ZEND_POST_INC_OBJ
			&& opline->opcode != ZEND_POST_DEC_OBJ) {
		return 0;
	}

	if (opline->op2_type != IS_CONST
	 || Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) != IS_STRING
	 || Z_STRVAL_P(RT_CONSTANT(opline, opline->op2))[0] == '\0') {
		return 0;
	}

	for (i = ssa->vars[var].definition; i < use; i++) {
		if (ssa_opcodes[i]->opcode == ZEND_DO_UCALL
		 || ssa_opcodes[i]->opcode == ZEND_DO_FCALL_BY_NAME
		 || ssa_opcodes[i]->opcode == ZEND_DO_FCALL
		 || ssa_opcodes[i]->opcode == ZEND_INCLUDE_OR_EVAL) {
			return 0;
		}
	}

	return 1;
}

static int zend_jit_trace_stack_needs_deoptimization(zend_jit_trace_stack *stack, uint32_t stack_size)
{
	uint32_t i;

	for (i = 0; i < stack_size; i++) {
		if (STACK_REG(stack, i) != ZREG_NONE) {
			return 1;
		}
	}
	return 0;
}

static int zend_jit_trace_exit_needs_deoptimization(uint32_t trace_num, uint32_t exit_num)
{
	const zend_op *opline = zend_jit_traces[trace_num].exit_info[exit_num].opline;
	uint32_t flags = zend_jit_traces[trace_num].exit_info[exit_num].flags;
	uint32_t stack_size;
	zend_jit_trace_stack *stack;

	if (opline || (flags & (ZEND_JIT_EXIT_RESTORE_CALL|ZEND_JIT_EXIT_FREE_OP1|ZEND_JIT_EXIT_FREE_OP2))) {
		return 1;
	}

	stack_size = zend_jit_traces[trace_num].exit_info[exit_num].stack_size;
	stack = zend_jit_traces[trace_num].stack_map + zend_jit_traces[trace_num].exit_info[exit_num].stack_offset;
	return zend_jit_trace_stack_needs_deoptimization(stack, stack_size);
}

static int zend_jit_trace_deoptimization(dasm_State             **Dst,
                                         uint32_t                 flags,
                                         const zend_op           *opline,
                                         zend_jit_trace_stack    *parent_stack,
                                         int                      parent_vars_count,
                                         zend_jit_trace_stack    *stack,
                                         zend_lifetime_interval **ra)
{
	int i;
	zend_bool has_constants = 0;
	zend_bool has_unsaved_vars = 0;

	// TODO: Merge this loop with the following register LOAD loop to implement parallel move ???
	for (i = 0; i < parent_vars_count; i++) {
		int8_t reg = STACK_REG(parent_stack, i);

		if (reg != ZREG_NONE) {
			if (reg < ZREG_NUM) {
				if (ra && ra[i] && ra[i]->reg == reg) {
				    /* register already loaded by parent trace */
				    if (stack) {
						SET_STACK_REG(stack, i, reg);
					}
					has_unsaved_vars = 1;
				} else if (!zend_jit_store_var(Dst, 1 << STACK_TYPE(parent_stack, i), i, reg)) {
					return 0;
				}
			} else {
				/* delay custom deoptimization instructions to prevent register clobbering */
				has_constants = 1;
			}
		}
	}

	if (has_unsaved_vars
	 && (has_constants
	  || (flags & (ZEND_JIT_EXIT_RESTORE_CALL|ZEND_JIT_EXIT_FREE_OP1|ZEND_JIT_EXIT_FREE_OP2)))) {
		for (i = 0; i < parent_vars_count; i++) {
			int8_t reg = STACK_REG(parent_stack, i);

			if (reg != ZREG_NONE) {
				if (reg < ZREG_NUM) {
					if (ra && ra[i] && ra[i]->reg == reg) {
					    if (stack) {
							SET_STACK_REG(stack, i, ZREG_NONE);
						}
						if (!zend_jit_store_var(Dst, 1 << STACK_TYPE(parent_stack, i), i, reg)) {
							return 0;
						}
					}
				}
			}
		}
	}

	if (has_constants) {
		for (i = 0; i < parent_vars_count; i++) {
			int8_t reg = STACK_REG(parent_stack, i);

			if (reg != ZREG_NONE) {
				if (reg < ZREG_NUM) {
					/* pass */
				} else if (reg == ZREG_THIS) {
					if (!zend_jit_load_this(Dst, EX_NUM_TO_VAR(i))) {
						return 0;
					}
				} else {
					if (reg == ZREG_ZVAL_COPY_R0
					 &&!zend_jit_escape_if_undef_r0(Dst, i, flags, opline)) {
						return 0;
					}
					if (!zend_jit_store_const(Dst, i, reg)) {
						return 0;
					}
				}
			}
		}
	}

	if (flags & ZEND_JIT_EXIT_RESTORE_CALL) {
		if (!zend_jit_save_call_chain(Dst, -1)) {
			return 0;
		}
	}

	if (flags & ZEND_JIT_EXIT_FREE_OP2) {
		const zend_op *op = opline - 1;

		if (!zend_jit_free_op(Dst, op, -1, op->op2.var)) {
			return 0;
		}
	}

	if (flags & ZEND_JIT_EXIT_FREE_OP1) {
		const zend_op *op = opline - 1;

		if (!zend_jit_free_op(Dst, op, -1, op->op1.var)) {
			return 0;
		}
	}

	if (flags & (ZEND_JIT_EXIT_FREE_OP1|ZEND_JIT_EXIT_FREE_OP2)) {
		if (!zend_jit_check_exception(Dst)) {
			return 0;
		}
	}

	return 1;
}

static void zend_jit_trace_set_var_range(zend_ssa_var_info *info, zend_long min, zend_long max)
{
	info->has_range = 1;
	info->range.min = min;
	info->range.max = max;
	info->range.underflow = 0;
	info->range.overflow = 0;
}

static void zend_jit_trace_update_condition_ranges(const zend_op *opline, const zend_ssa_op *ssa_op, const zend_op_array *op_array, zend_ssa *ssa, zend_bool exit_if_true)
{
	zend_long op1_min, op1_max, op2_min, op2_max;

	if ((OP1_INFO() & MAY_BE_ANY) != MAY_BE_LONG
	 || (OP1_INFO() & MAY_BE_ANY) != MAY_BE_LONG) {
		return;
	}

	op1_min = OP1_MIN_RANGE();
	op1_max = OP1_MAX_RANGE();
	op2_min = OP2_MIN_RANGE();
	op2_max = OP2_MAX_RANGE();

	switch (opline->opcode) {
		case ZEND_IS_EQUAL:
		case ZEND_CASE:
		case ZEND_IS_IDENTICAL:
		case ZEND_CASE_STRICT:
			if (!exit_if_true) {
				/* op1 == op2 */
				if (ssa_op->op1_use >= 0) {
					zend_jit_trace_set_var_range(
						&ssa->var_info[ssa_op->op1_use],
						MAX(op1_min, op2_min),
						MIN(op1_max, op2_max));
				}
				if (ssa_op->op2_use >= 0) {
					zend_jit_trace_set_var_range(
						&ssa->var_info[ssa_op->op2_use],
						MAX(op2_min, op1_min),
						MIN(op2_max, op1_max));
				}
			}
			break;
		case ZEND_IS_NOT_EQUAL:
		case ZEND_IS_NOT_IDENTICAL:
			if (exit_if_true) {
				/* op1 == op2 */
				if (ssa_op->op1_use >= 0) {
					zend_jit_trace_set_var_range(
						&ssa->var_info[ssa_op->op1_use],
						MAX(op1_min, op2_min),
						MIN(op1_max, op2_max));
				}
				if (ssa_op->op2_use >= 0) {
					zend_jit_trace_set_var_range(
						&ssa->var_info[ssa_op->op2_use],
						MAX(op2_min, op1_min),
						MIN(op2_max, op1_max));
				}
			}
			break;
		case ZEND_IS_SMALLER_OR_EQUAL:
			if (!exit_if_true) {
				/* op1 <= op2 */
				if (ssa_op->op1_use >= 0) {
					zend_jit_trace_set_var_range(
						&ssa->var_info[ssa_op->op1_use],
						op1_min,
						MIN(op1_max, op2_max));
				}
				if (ssa_op->op2_use >= 0) {
					zend_jit_trace_set_var_range(
						&ssa->var_info[ssa_op->op2_use],
						MAX(op2_min, op1_min),
						op2_max);
				}
			} else {
				/* op1 > op2 */
				if (ssa_op->op1_use >= 0) {
					zend_jit_trace_set_var_range(
						&ssa->var_info[ssa_op->op1_use],
						op2_min != ZEND_LONG_MAX ? MAX(op1_min, op2_min + 1) : op1_min,
						op1_max);
				}
				if (ssa_op->op2_use >= 0) {
					zend_jit_trace_set_var_range(
						&ssa->var_info[ssa_op->op2_use],
						op2_min,
						op2_max != ZEND_LONG_MIN ?MIN(op2_max, op1_max - 1) : op1_max);
				}
			}
			break;
		case ZEND_IS_SMALLER:
			if (!exit_if_true) {
				/* op1 < op2 */
				if (ssa_op->op1_use >= 0) {
					zend_jit_trace_set_var_range(
						&ssa->var_info[ssa_op->op1_use],
						op1_min,
						op2_max != ZEND_LONG_MIN ? MIN(op1_max, op2_max - 1) : op1_max);
				}
				if (ssa_op->op2_use >= 0) {
					zend_jit_trace_set_var_range(
						&ssa->var_info[ssa_op->op2_use],
						op1_min != ZEND_LONG_MAX ? MAX(op2_min, op1_min + 1) : op2_min,
						op2_max);
				}
			} else {
				/* op1 >= op2 */
				if (ssa_op->op1_use >= 0) {
					zend_jit_trace_set_var_range(
						&ssa->var_info[ssa_op->op1_use],
						MAX(op1_min, op2_min),
						op1_max);
				}
				if (ssa_op->op2_use >= 0) {
					zend_jit_trace_set_var_range(
						&ssa->var_info[ssa_op->op2_use],
						op2_min,
						MIN(op2_max, op1_max));
				}
			}
			break;
	}
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
	zend_ssa *ssa, *op_array_ssa;
	const zend_op **ssa_opcodes;
	zend_jit_trace_rec *p;
	zend_jit_op_array_trace_extension *jit_extension;
	int num_op_arrays = 0;
	zend_jit_trace_info *t;
	const zend_op_array *op_arrays[ZEND_JIT_TRACE_MAX_FUNCS];
	zend_uchar smart_branch_opcode;
	const void *exit_addr;
	uint32_t op1_info, op1_def_info, op2_info, res_info, res_use_info, op1_data_info;
	zend_bool send_result = 0;
	zend_jit_addr op1_addr, op1_def_addr, op2_addr, op2_def_addr, res_addr;
	zend_class_entry *ce;
	zend_bool ce_is_instanceof;
	zend_bool delayed_fetch_this = 0;
	zend_bool avoid_refcounting = 0;
	uint32_t i;
	zend_jit_trace_stack_frame *frame, *top, *call;
	zend_jit_trace_stack *stack;
	zend_uchar res_type = IS_UNKNOWN;
	const zend_op *opline, *orig_opline;
	const zend_ssa_op *ssa_op, *orig_ssa_op;

	JIT_G(current_trace) = trace_buffer;

	checkpoint = zend_arena_checkpoint(CG(arena));

	ssa = zend_jit_trace_build_tssa(trace_buffer, parent_trace, exit_num, script, op_arrays, &num_op_arrays);

	if (!ssa) {
		goto jit_cleanup;
	}

	ssa_opcodes = ((zend_tssa*)ssa)->tssa_opcodes;

	/* Register allocation */
	if ((JIT_G(opt_flags) & (ZEND_JIT_REG_ALLOC_LOCAL|ZEND_JIT_REG_ALLOC_GLOBAL))
	 && JIT_G(opt_level) >= ZEND_JIT_LEVEL_INLINE) {
		ra = zend_jit_trace_allocate_registers(trace_buffer, ssa, parent_trace, exit_num);
	}

	p = trace_buffer;
	ZEND_ASSERT(p->op == ZEND_JIT_TRACE_START);
	op_array = p->op_array;
	frame = JIT_G(current_frame);
	top = zend_jit_trace_call_frame(frame, op_array);
	TRACE_FRAME_INIT(frame, op_array, TRACE_FRAME_MASK_UNKNOWN_RETURN, -1);
	stack = frame->stack;
	for (i = 0; i < op_array->last_var + op_array->T; i++) {
		SET_STACK_TYPE(stack, i, IS_UNKNOWN);
	}

	opline = p[1].opline;
	name = zend_jit_trace_name(op_array, opline->lineno);
	p += ZEND_JIT_TRACE_START_REC_SIZE;

	dasm_init(&dasm_state, DASM_MAXSECTION);
	dasm_setupglobal(&dasm_state, dasm_labels, zend_lb_MAX);
	dasm_setup(&dasm_state, dasm_actions);

	jit_extension =
		(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
	op_array_ssa = &jit_extension->func_info.ssa;

	dasm_growpc(&dasm_state, 1); /* trace needs just one global label for loop */

	zend_jit_align_func(&dasm_state);
	if (!parent_trace) {
		zend_jit_prologue(&dasm_state);
	}
	zend_jit_trace_begin(&dasm_state, ZEND_JIT_TRACE_NUM,
		parent_trace ? &zend_jit_traces[parent_trace] : NULL, exit_num);

	if (!parent_trace) {
		zend_jit_set_last_valid_opline(opline);
		zend_jit_track_last_valid_opline();
	} else {
		if (zend_jit_traces[parent_trace].exit_info[exit_num].opline == NULL) {
			zend_jit_trace_opline_guard(&dasm_state, opline);
		} else {
			zend_jit_reset_last_valid_opline();
		}
	}

	if (JIT_G(opt_level) >= ZEND_JIT_LEVEL_INLINE) {
		int last_var;
		int parent_vars_count = 0;
		zend_jit_trace_stack *parent_stack = NULL;

		if (parent_trace) {
			parent_vars_count = MIN(zend_jit_traces[parent_trace].exit_info[exit_num].stack_size,
				op_array->last_var + op_array->T);
			if (parent_vars_count) {
				parent_stack =
					zend_jit_traces[parent_trace].stack_map +
					zend_jit_traces[parent_trace].exit_info[exit_num].stack_offset;
			}
		}

		last_var = op_array->last_var;
		if (trace_buffer->start != ZEND_JIT_TRACE_START_ENTER) {
			last_var += op_array->T;
		}

		for (i = 0; i < last_var; i++) {
			uint32_t info = ssa->var_info[i].type;

			if (!(info & MAY_BE_GUARD) && has_concrete_type(info)) {
				SET_STACK_TYPE(stack, i, concrete_type(info));
			} else if (zend_jit_var_may_alias(op_array, op_array_ssa, i) != NO_ALIAS) {
				SET_STACK_TYPE(stack, i, IS_UNKNOWN);
			} else if (i < parent_vars_count
			 && STACK_TYPE(parent_stack, i) != IS_UNKNOWN) {
				/* This must be already handled by trace type inference */
				ZEND_UNREACHABLE();
				SET_STACK_TYPE(stack, i, STACK_TYPE(parent_stack, i));
			} else if ((info & MAY_BE_GUARD) != 0
			 && trace_buffer->stop == ZEND_JIT_TRACE_STOP_LOOP
			 && (ssa->vars[i].use_chain != -1
			  || (ssa->vars[i].phi_use_chain
			   && !(ssa->var_info[ssa->vars[i].phi_use_chain->ssa_var].type & MAY_BE_GUARD)))) {
				/* Check loop-invariant variable type */
				if (!zend_jit_type_guard(&dasm_state, opline, EX_NUM_TO_VAR(i), concrete_type(info))) {
					goto jit_failure;
				}
				info &= ~MAY_BE_GUARD;
				ssa->var_info[i].type = info;
				SET_STACK_TYPE(stack, i, concrete_type(info));
			} else if (trace_buffer->start == ZEND_JIT_TRACE_START_ENTER
			 && op_array->function_name
			 && i >= op_array->num_args) {
				/* This must be already handled by trace type inference */
				ZEND_UNREACHABLE();
				SET_STACK_TYPE(stack, i, IS_UNDEF);
			} else {
				SET_STACK_TYPE(stack, i, IS_UNKNOWN);
			}

			if ((info & MAY_BE_PACKED_GUARD) != 0
			 && trace_buffer->stop == ZEND_JIT_TRACE_STOP_LOOP
			 && ssa->vars[i].use_chain != -1) {
				if (!zend_jit_packed_guard(&dasm_state, opline, EX_NUM_TO_VAR(i), info)) {
					goto jit_failure;
				}
				info &= ~MAY_BE_PACKED_GUARD;
				ssa->var_info[i].type = info;
			}
		}

		if (parent_trace) {
			/* Deoptimization */
			if (!zend_jit_trace_deoptimization(&dasm_state,
					zend_jit_traces[parent_trace].exit_info[exit_num].flags,
					zend_jit_traces[parent_trace].exit_info[exit_num].opline,
					parent_stack, parent_vars_count, stack, ra)) {
				goto jit_failure;
			}
		}

		if (ra
		 && trace_buffer->stop != ZEND_JIT_TRACE_STOP_RECURSIVE_CALL
		 && trace_buffer->stop != ZEND_JIT_TRACE_STOP_RECURSIVE_RET) {
			for (i = 0; i < last_var; i++) {
				if (ra[i] && (ra[i]->flags & ZREG_LOAD) != 0) {
					//SET_STACK_REG(stack, i, ra[i]->reg);
					if (!zend_jit_load_var(&dasm_state, ssa->var_info[i].type, i, ra[i]->reg)) {
						goto jit_failure;
					}
				}
			}
		}
	}

	if (trace_buffer->stop == ZEND_JIT_TRACE_STOP_LOOP
	 || trace_buffer->stop == ZEND_JIT_TRACE_STOP_RECURSIVE_CALL
	 || trace_buffer->stop == ZEND_JIT_TRACE_STOP_RECURSIVE_RET) {

		zend_jit_label(&dasm_state, 0); /* start of of trace loop */

		if (ra && trace_buffer->stop == ZEND_JIT_TRACE_STOP_LOOP) {
			zend_ssa_phi *phi = ssa->blocks[1].phis;

			while (phi) {
				zend_lifetime_interval *ival = ra[phi->ssa_var];

				if (ival) {
					if (ival->flags & ZREG_LOAD) {
						ZEND_ASSERT(ival->reg != ZREG_NONE);

						if (!zend_jit_load_var(&dasm_state, ssa->var_info[phi->ssa_var].type, ssa->vars[phi->ssa_var].var, ival->reg)) {
							goto jit_failure;
						}
					} else if (ival->flags & ZREG_STORE) {
						ZEND_ASSERT(ival->reg != ZREG_NONE);

						if (!zend_jit_store_var(&dasm_state, ssa->var_info[phi->ssa_var].type, ssa->vars[phi->ssa_var].var, ival->reg)) {
							goto jit_failure;
						}
					} else {
						/* Register has to be written back on side exit */
						SET_STACK_REG(stack, phi->var, ival->reg);
					}
				}
				phi = phi->next;
			}
		}

//		if (trace_buffer->stop != ZEND_JIT_TRACE_STOP_RECURSIVE_RET) {
//			if (ra && zend_jit_trace_stack_needs_deoptimization(stack, op_array->last_var + op_array->T)) {
//				uint32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
//
//				timeout_exit_addr = zend_jit_trace_get_exit_addr(exit_point);
//				if (!timeout_exit_addr) {
//					goto jit_failure;
//				}
//			}
//		}

		if (ra && trace_buffer->stop != ZEND_JIT_TRACE_STOP_LOOP) {
			int last_var = op_array->last_var;

			if (trace_buffer->start != ZEND_JIT_TRACE_START_ENTER) {
				last_var += op_array->T;
			}
			for (i = 0; i < last_var; i++) {
				if (ra && ra[i] && (ra[i]->flags & ZREG_LOAD) != 0) {
					//SET_STACK_REG(stack, i, ra[i]->reg);
					if (!zend_jit_load_var(&dasm_state, ssa->var_info[i].type, i, ra[i]->reg)) {
						goto jit_failure;
					}
				}
			}
		}
	}

	ssa_op = (JIT_G(opt_level) >= ZEND_JIT_LEVEL_INLINE) ? ssa->ops : NULL;
	for (;;p++) {
		if (p->op == ZEND_JIT_TRACE_VM) {
			uint8_t op1_type = p->op1_type;
			uint8_t op2_type = p->op2_type;
			uint8_t op3_type = p->op3_type;
			uint8_t orig_op1_type = op1_type;
			uint8_t orig_op2_type = op2_type;
			zend_bool op1_indirect;
			zend_class_entry *op1_ce = NULL;
			zend_class_entry *op2_ce = NULL;

			opline = p->opline;
			if (op1_type & (IS_TRACE_REFERENCE|IS_TRACE_INDIRECT)) {
				op1_type = IS_UNKNOWN;
			}
			if (op1_type != IS_UNKNOWN) {
				op1_type &= ~IS_TRACE_PACKED;
			}
			if (op2_type & (IS_TRACE_REFERENCE|IS_TRACE_INDIRECT)) {
				op2_type = IS_UNKNOWN;
			}
			if (op3_type & (IS_TRACE_REFERENCE|IS_TRACE_INDIRECT)) {
				op3_type = IS_UNKNOWN;
			}

			if ((p+1)->op == ZEND_JIT_TRACE_OP1_TYPE) {
				op1_ce = (zend_class_entry*)(p+1)->ce;
				p++;
			}
			if ((p+1)->op == ZEND_JIT_TRACE_OP2_TYPE) {
				op2_ce = (zend_class_entry*)(p+1)->ce;
				p++;
			}

			switch (opline->opcode) {
				case ZEND_INIT_FCALL:
				case ZEND_INIT_FCALL_BY_NAME:
				case ZEND_INIT_NS_FCALL_BY_NAME:
				case ZEND_INIT_METHOD_CALL:
				case ZEND_INIT_DYNAMIC_CALL:
				case ZEND_INIT_STATIC_METHOD_CALL:
				case ZEND_INIT_USER_CALL:
				case ZEND_NEW:
					frame->call_level++;
			}

			if (JIT_G(opt_level) >= ZEND_JIT_LEVEL_INLINE) {
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
						if (!(op1_info & MAY_BE_LONG)) {
							break;
						}
						if (opline->result_type != IS_UNUSED) {
							if (opline->result_type == IS_CV) {
								res_use_info = RES_USE_INFO();
							} else {
#if USE_ABSTRACT_STACK_FOR_RES_USE_INFO
								res_use_info = zend_jit_trace_type_to_info(STACK_VAR_TYPE(opline->result.var));
#else
								res_use_info = -1;
#endif
							}
							res_info = RES_INFO();
							res_addr = RES_REG_ADDR();
						} else {
							res_use_info = -1;
							res_info = -1;
							res_addr = 0;
						}
						op1_def_info = OP1_DEF_INFO();
						if (!zend_jit_inc_dec(&dasm_state, opline,
								op1_info, OP1_REG_ADDR(),
								op1_def_info, OP1_DEF_REG_ADDR(),
								res_use_info, res_info,
								res_addr,
								(op1_def_info & (MAY_BE_DOUBLE|MAY_BE_GUARD)) && zend_may_overflow(opline, ssa_op, op_array, ssa),
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						if ((op1_def_info & (MAY_BE_ANY|MAY_BE_GUARD)) == (MAY_BE_LONG|MAY_BE_GUARD)) {
							ssa->var_info[ssa_op->op1_def].type &= ~MAY_BE_GUARD;
							if (opline->result_type != IS_UNUSED) {
								ssa->var_info[ssa_op->result_def].type &= ~MAY_BE_GUARD;
							}
						}
						if (opline->result_type != IS_UNUSED
						 && (res_info & (MAY_BE_ANY|MAY_BE_GUARD)) == (MAY_BE_LONG|MAY_BE_GUARD)) {
							ssa->var_info[ssa_op->result_def].type &= ~MAY_BE_GUARD;
						}
						goto done;
					case ZEND_BW_OR:
					case ZEND_BW_AND:
					case ZEND_BW_XOR:
					case ZEND_SL:
					case ZEND_SR:
					case ZEND_MOD:
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						op2_info = OP2_INFO();
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
									res_type = IS_UNKNOWN;
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
							if (opline->result_type == IS_CV) {
								res_use_info = RES_USE_INFO();
							} else {
#if USE_ABSTRACT_STACK_FOR_RES_USE_INFO
								res_use_info = zend_jit_trace_type_to_info(STACK_VAR_TYPE(opline->result.var));
#else
								res_use_info = -1;
#endif
							}
							res_addr = RES_REG_ADDR();
						}
						res_info = RES_INFO();
						if (!zend_jit_long_math(&dasm_state, opline,
								op1_info, OP1_RANGE(), OP1_REG_ADDR(),
								op2_info, OP2_RANGE(), OP2_REG_ADDR(),
								res_use_info, res_info, res_addr,
								send_result,
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ADD:
					case ZEND_SUB:
					case ZEND_MUL:
//					case ZEND_DIV: // TODO: check for division by zero ???
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						op2_info = OP2_INFO();
						CHECK_OP2_TRACE_TYPE();
						if ((op1_info & MAY_BE_UNDEF) || (op2_info & MAY_BE_UNDEF)) {
							break;
						}
						if (opline->opcode == ZEND_ADD &&
						    (op1_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_ARRAY &&
						    (op2_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_ARRAY) {
							if (!zend_jit_add_arrays(&dasm_state, opline, op1_info, op2_info)) {
								goto jit_failure;
							}
							goto done;
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
							 && frame->call->func
							 && frame->call->func->type == ZEND_USER_FUNCTION) {
								uint8_t res_type = p->op1_type;
								if (res_type & IS_TRACE_REFERENCE) {
									res_type = IS_UNKNOWN;
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
							if (opline->result_type == IS_CV) {
								res_use_info = RES_USE_INFO();
							} else {
#if USE_ABSTRACT_STACK_FOR_RES_USE_INFO
								res_use_info = zend_jit_trace_type_to_info(STACK_VAR_TYPE(opline->result.var));
#else
								res_use_info = -1;
#endif
							}
							res_addr = RES_REG_ADDR();
						}
						res_info = RES_INFO();
						if (!zend_jit_math(&dasm_state, opline,
								op1_info, OP1_REG_ADDR(),
								op2_info, OP2_REG_ADDR(),
								res_use_info, res_info, res_addr,
								send_result,
								(op1_info & MAY_BE_LONG) && (op2_info & MAY_BE_LONG) && (res_info & (MAY_BE_DOUBLE|MAY_BE_GUARD)) && zend_may_overflow(opline, ssa_op, op_array, ssa),
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						if ((res_info & (MAY_BE_ANY|MAY_BE_GUARD)) == (MAY_BE_LONG|MAY_BE_GUARD)) {
							ssa->var_info[ssa_op->result_def].type &= ~MAY_BE_GUARD;
						}
						goto done;
					case ZEND_CONCAT:
					case ZEND_FAST_CONCAT:
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						op2_info = OP2_INFO();
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
							 && frame->call->func
							 && frame->call->func->type == ZEND_USER_FUNCTION) {
								uint8_t res_type = p->op1_type;
								if (res_type & IS_TRACE_REFERENCE) {
									res_type = IS_UNKNOWN;
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
						if (!zend_jit_concat(&dasm_state, opline,
								op1_info, op2_info, send_result,
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
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
						CHECK_OP1_TRACE_TYPE();
						op2_info = OP2_INFO();
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
						if (!zend_jit_assign_op(&dasm_state, opline,
								op1_info, op1_def_info, OP1_RANGE(),
								op2_info, OP2_RANGE(),
								(op1_info & MAY_BE_LONG) && (op2_info & MAY_BE_LONG) && (op1_def_info & (MAY_BE_DOUBLE|MAY_BE_GUARD)) && zend_may_overflow(opline, ssa_op, op_array, ssa),
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						if ((op1_def_info & (MAY_BE_ANY|MAY_BE_GUARD)) == (MAY_BE_LONG|MAY_BE_GUARD)) {
							ssa->var_info[ssa_op->op1_def].type &= ~MAY_BE_GUARD;
							if (opline->result_type != IS_UNUSED) {
								ssa->var_info[ssa_op->result_def].type &= ~MAY_BE_GUARD;
							}
						}
						goto done;
					case ZEND_ASSIGN_DIM_OP:
						if (opline->extended_value == ZEND_POW
						 || opline->extended_value == ZEND_DIV) {
							// TODO: check for division by zero ???
							break;
						}
						if (opline->result_type != IS_UNUSED) {
							break;
						}
						op1_info = OP1_INFO();
						op1_addr = OP1_REG_ADDR();
						if (opline->op1_type == IS_VAR) {
							if (orig_op1_type != IS_UNKNOWN
							 && (orig_op1_type & IS_TRACE_INDIRECT)) {
								if (!zend_jit_fetch_indirect_var(&dasm_state, opline, orig_op1_type,
										&op1_info, &op1_addr, !ssa->var_info[ssa_op->op1_use].indirect_reference)) {
									goto jit_failure;
								}
							} else {
								break;
							}
						}
						if (orig_op1_type != IS_UNKNOWN
						 && (orig_op1_type & IS_TRACE_REFERENCE)) {
							if (!zend_jit_fetch_reference(&dasm_state, opline, orig_op1_type, &op1_info, &op1_addr,
									!ssa->var_info[ssa_op->op1_use].guarded_reference, 1)) {
								goto jit_failure;
							}
							if (opline->op1_type == IS_CV
							 && zend_jit_var_may_alias(op_array, op_array_ssa, EX_VAR_TO_NUM(opline->op1.var)) == NO_ALIAS) {
								ssa->var_info[ssa_op->op1_use].guarded_reference = 1;
							}
						} else {
							CHECK_OP1_TRACE_TYPE();
						}
						op2_info = OP2_INFO();
						CHECK_OP2_TRACE_TYPE();
						op1_data_info = OP1_DATA_INFO();
						CHECK_OP1_DATA_TRACE_TYPE();
						op1_def_info = OP1_DEF_INFO();
						if (!zend_jit_assign_dim_op(&dasm_state, opline,
								op1_info, op1_def_info, op1_addr, op2_info,
								op1_data_info, OP1_DATA_RANGE(),
								zend_may_throw_ex(opline, ssa_op, op_array, ssa, op1_info, op2_info))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_PRE_INC_OBJ:
					case ZEND_PRE_DEC_OBJ:
					case ZEND_POST_INC_OBJ:
					case ZEND_POST_DEC_OBJ:
						if (opline->op2_type != IS_CONST
						 || Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) != IS_STRING
						 || Z_STRVAL_P(RT_CONSTANT(opline, opline->op2))[0] == '\0') {
							break;
						}
						ce = NULL;
						ce_is_instanceof = 0;
						delayed_fetch_this = 0;
						op1_indirect = 0;
						if (opline->op1_type == IS_UNUSED) {
							op1_info = MAY_BE_OBJECT|MAY_BE_RC1|MAY_BE_RCN;
							ce = op_array->scope;
							ce_is_instanceof = (ce->ce_flags & ZEND_ACC_FINAL) != 0;
							op1_addr = 0;
						} else {
							if (ssa_op->op1_use >= 0) {
								delayed_fetch_this = ssa->var_info[ssa_op->op1_use].delayed_fetch_this;
							}
							op1_info = OP1_INFO();
							if (!(op1_info & MAY_BE_OBJECT)) {
								break;
							}
							op1_addr = OP1_REG_ADDR();
							if (opline->op1_type == IS_VAR) {
								if (orig_op1_type != IS_UNKNOWN
								 && (orig_op1_type & IS_TRACE_INDIRECT)) {
									op1_indirect = 1;
									if (!zend_jit_fetch_indirect_var(&dasm_state, opline, orig_op1_type,
											&op1_info, &op1_addr, !ssa->var_info[ssa_op->op1_use].indirect_reference)) {
										goto jit_failure;
									}
								}
							}
							if (orig_op1_type != IS_UNKNOWN
							 && (orig_op1_type & IS_TRACE_REFERENCE)) {
								if (!zend_jit_fetch_reference(&dasm_state, opline, orig_op1_type, &op1_info, &op1_addr,
										!ssa->var_info[ssa_op->op1_use].guarded_reference, 1)) {
									goto jit_failure;
								}
								if (opline->op1_type == IS_CV
								 && zend_jit_var_may_alias(op_array, op_array_ssa, EX_VAR_TO_NUM(opline->op1.var)) == NO_ALIAS) {
									ssa->var_info[ssa_op->op1_use].guarded_reference = 1;
								}
							} else {
								CHECK_OP1_TRACE_TYPE();
							}
							if (ssa->var_info && ssa->ops) {
								if (ssa_op->op1_use >= 0) {
									zend_ssa_var_info *op1_ssa = ssa->var_info + ssa_op->op1_use;
									if (op1_ssa->ce && !op1_ssa->ce->create_object) {
										ce = op1_ssa->ce;
										ce_is_instanceof = op1_ssa->is_instanceof;
									}
								}
							}
						}
						if (!zend_jit_incdec_obj(&dasm_state, opline, op_array, ssa, ssa_op,
								op1_info, op1_addr,
								op1_indirect, ce, ce_is_instanceof, delayed_fetch_this, op1_ce,
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ASSIGN_OBJ_OP:
						if (opline->extended_value == ZEND_POW
						 || opline->extended_value == ZEND_DIV) {
							// TODO: check for division by zero ???
							break;
						}
						if (opline->result_type != IS_UNUSED) {
							break;
						}
						if (opline->op2_type != IS_CONST
						 || Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) != IS_STRING
						 || Z_STRVAL_P(RT_CONSTANT(opline, opline->op2))[0] == '\0') {
							break;
						}
						ce = NULL;
						ce_is_instanceof = 0;
						delayed_fetch_this = 0;
						op1_indirect = 0;
						if (opline->op1_type == IS_UNUSED) {
							op1_info = MAY_BE_OBJECT|MAY_BE_RC1|MAY_BE_RCN;
							ce = op_array->scope;
							ce_is_instanceof = (ce->ce_flags & ZEND_ACC_FINAL) != 0;
							op1_addr = 0;
						} else {
							if (ssa_op->op1_use >= 0) {
								delayed_fetch_this = ssa->var_info[ssa_op->op1_use].delayed_fetch_this;
							}
							op1_info = OP1_INFO();
							if (!(op1_info & MAY_BE_OBJECT)) {
								break;
							}
							op1_addr = OP1_REG_ADDR();
							if (opline->op1_type == IS_VAR) {
								if (orig_op1_type != IS_UNKNOWN
								 && (orig_op1_type & IS_TRACE_INDIRECT)) {
									op1_indirect = 1;
									if (!zend_jit_fetch_indirect_var(&dasm_state, opline, orig_op1_type,
											&op1_info, &op1_addr, !ssa->var_info[ssa_op->op1_use].indirect_reference)) {
										goto jit_failure;
									}
								}
							}
							if (orig_op1_type != IS_UNKNOWN
							 && (orig_op1_type & IS_TRACE_REFERENCE)) {
								if (!zend_jit_fetch_reference(&dasm_state, opline, orig_op1_type, &op1_info, &op1_addr,
										!ssa->var_info[ssa_op->op1_use].guarded_reference, 1)) {
									goto jit_failure;
								}
								if (opline->op1_type == IS_CV
								 && zend_jit_var_may_alias(op_array, op_array_ssa, EX_VAR_TO_NUM(opline->op1.var)) == NO_ALIAS) {
									ssa->var_info[ssa_op->op1_use].guarded_reference = 1;
								}
							} else {
								CHECK_OP1_TRACE_TYPE();
							}
							if (ssa->var_info && ssa->ops) {
								if (ssa_op->op1_use >= 0) {
									zend_ssa_var_info *op1_ssa = ssa->var_info + ssa_op->op1_use;
									if (op1_ssa->ce && !op1_ssa->ce->create_object) {
										ce = op1_ssa->ce;
										ce_is_instanceof = op1_ssa->is_instanceof;
									}
								}
							}
						}
						op1_data_info = OP1_DATA_INFO();
						CHECK_OP1_DATA_TRACE_TYPE();
						if (!zend_jit_assign_obj_op(&dasm_state, opline, op_array, ssa, ssa_op,
								op1_info, op1_addr, op1_data_info, OP1_DATA_RANGE(),
								op1_indirect, ce, ce_is_instanceof, delayed_fetch_this, op1_ce,
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ASSIGN_OBJ:
						if (opline->op2_type != IS_CONST
						 || Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) != IS_STRING
						 || Z_STRVAL_P(RT_CONSTANT(opline, opline->op2))[0] == '\0') {
							break;
						}
						ce = NULL;
						ce_is_instanceof = 0;
						delayed_fetch_this = 0;
						op1_indirect = 0;
						if (opline->op1_type == IS_UNUSED) {
							op1_info = MAY_BE_OBJECT|MAY_BE_RC1|MAY_BE_RCN;
							ce = op_array->scope;
							ce_is_instanceof = (ce->ce_flags & ZEND_ACC_FINAL) != 0;
							op1_addr = 0;
						} else {
							if (ssa_op->op1_use >= 0) {
								delayed_fetch_this = ssa->var_info[ssa_op->op1_use].delayed_fetch_this;
							}
							op1_info = OP1_INFO();
							if (!(op1_info & MAY_BE_OBJECT)) {
								break;
							}
							op1_addr = OP1_REG_ADDR();
							if (opline->op1_type == IS_VAR) {
								if (orig_op1_type != IS_UNKNOWN
								 && (orig_op1_type & IS_TRACE_INDIRECT)) {
									op1_indirect = 1;
									if (!zend_jit_fetch_indirect_var(&dasm_state, opline, orig_op1_type,
											&op1_info, &op1_addr, !ssa->var_info[ssa_op->op1_use].indirect_reference)) {
										goto jit_failure;
									}
								}
							}
							if (orig_op1_type != IS_UNKNOWN
							 && (orig_op1_type & IS_TRACE_REFERENCE)) {
								if (!zend_jit_fetch_reference(&dasm_state, opline, orig_op1_type, &op1_info, &op1_addr,
										!ssa->var_info[ssa_op->op1_use].guarded_reference, 1)) {
									goto jit_failure;
								}
								if (opline->op1_type == IS_CV
								 && zend_jit_var_may_alias(op_array, op_array_ssa, EX_VAR_TO_NUM(opline->op1.var)) == NO_ALIAS) {
									ssa->var_info[ssa_op->op1_use].guarded_reference = 1;
								}
							} else {
								CHECK_OP1_TRACE_TYPE();
							}
							if (ssa->var_info && ssa->ops) {
								if (ssa_op->op1_use >= 0) {
									zend_ssa_var_info *op1_ssa = ssa->var_info + ssa_op->op1_use;
									if (op1_ssa->ce && !op1_ssa->ce->create_object) {
										ce = op1_ssa->ce;
										ce_is_instanceof = op1_ssa->is_instanceof;
									}
								}
							}
						}
						op1_data_info = OP1_DATA_INFO();
						CHECK_OP1_DATA_TRACE_TYPE();
						if (!zend_jit_assign_obj(&dasm_state, opline, op_array, ssa, ssa_op,
								op1_info, op1_addr, op1_data_info,
								op1_indirect, ce, ce_is_instanceof, delayed_fetch_this, op1_ce,
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ASSIGN_DIM:
						op1_info = OP1_INFO();
						op1_addr = OP1_REG_ADDR();
						if (opline->op1_type == IS_VAR) {
							if (orig_op1_type != IS_UNKNOWN
							 && (orig_op1_type & IS_TRACE_INDIRECT)
							 && opline->result_type == IS_UNUSED) {
								if (!zend_jit_fetch_indirect_var(&dasm_state, opline, orig_op1_type,
										&op1_info, &op1_addr, !ssa->var_info[ssa_op->op1_use].indirect_reference)) {
									goto jit_failure;
								}
							} else {
								break;
							}
						}
						if (orig_op1_type != IS_UNKNOWN
						 && (orig_op1_type & IS_TRACE_REFERENCE)) {
							if (!zend_jit_fetch_reference(&dasm_state, opline, orig_op1_type, &op1_info, &op1_addr,
									!ssa->var_info[ssa_op->op1_use].guarded_reference, 1)) {
								goto jit_failure;
							}
							if (opline->op1_type == IS_CV
							 && zend_jit_var_may_alias(op_array, op_array_ssa, EX_VAR_TO_NUM(opline->op1.var)) == NO_ALIAS) {
								ssa->var_info[ssa_op->op1_use].guarded_reference = 1;
							}
						} else {
							CHECK_OP1_TRACE_TYPE();
						}
						op2_info = OP2_INFO();
						CHECK_OP2_TRACE_TYPE();
						op1_data_info = OP1_DATA_INFO();
						CHECK_OP1_DATA_TRACE_TYPE();
						if (!zend_jit_assign_dim(&dasm_state, opline,
								op1_info, op1_addr, op2_info, op1_data_info,
								zend_may_throw_ex(opline, ssa_op, op_array, ssa, op1_info, op2_info))) {
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
						 && ssa_op->op2_def >= 0
						 && !ssa->vars[ssa_op->op2_def].no_val) {
							op2_def_addr = OP2_DEF_REG_ADDR();
						} else {
							op2_def_addr = op2_addr;
						}
						op2_info = OP2_INFO();
						CHECK_OP2_TRACE_TYPE();
						op1_info = OP1_INFO();
						op1_def_info = OP1_DEF_INFO();
						op1_addr = OP1_REG_ADDR();
						op1_def_addr = OP1_DEF_REG_ADDR();
						if (orig_op1_type != IS_UNKNOWN) {
							if (orig_op1_type & IS_TRACE_REFERENCE) {
								if (!zend_jit_fetch_reference(&dasm_state, opline, orig_op1_type, &op1_info, &op1_addr,
										!ssa->var_info[ssa_op->op1_use].guarded_reference, 0)) {
									goto jit_failure;
								}
								if (opline->op1_type == IS_CV
								 && zend_jit_var_may_alias(op_array, op_array_ssa, EX_VAR_TO_NUM(opline->op1.var)) == NO_ALIAS) {
									ssa->var_info[ssa_op->op1_use].guarded_reference = 1;
								}
								if (!zend_jit_assign_to_typed_ref(&dasm_state, opline, opline->op2_type, op2_addr, 1)) {
									goto jit_failure;
								}
								op1_def_addr = op1_addr;
								op1_def_info &= ~MAY_BE_REF;
							} else if (op1_info & MAY_BE_REF) {
								if (!zend_jit_noref_guard(&dasm_state, opline, op1_addr)) {
									goto jit_failure;
								}
								op1_info &= ~MAY_BE_REF;
								op1_def_info &= ~MAY_BE_REF;
							}
						}
						if (!zend_jit_assign(&dasm_state, opline,
								op1_info, op1_addr,
								op1_def_info, op1_def_addr,
								op2_info, op2_addr, op2_def_addr,
								res_info, res_addr,
								zend_may_throw_ex(opline, ssa_op, op_array, ssa, op1_info, op2_info))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_CAST:
						if (opline->extended_value != op1_type) {
							break;
						}
						/* break missing intentionally */
					case ZEND_QM_ASSIGN:
						op1_addr = OP1_REG_ADDR();
						if (ra
						 && ssa_op->op1_def >= 0
						 && !ssa->vars[ssa_op->op1_def].no_val) {
							op1_def_addr = OP1_DEF_REG_ADDR();
						} else {
							op1_def_addr = op1_addr;
						}
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						res_info = RES_INFO();
						if (!zend_jit_qm_assign(&dasm_state, opline,
								op1_info, op1_addr, op1_def_addr,
								res_info, RES_REG_ADDR())) {
							goto jit_failure;
						}
						goto done;
					case ZEND_INIT_FCALL:
					case ZEND_INIT_FCALL_BY_NAME:
					case ZEND_INIT_NS_FCALL_BY_NAME:
						if (!zend_jit_init_fcall(&dasm_state, opline, op_array_ssa->cfg.map ? op_array_ssa->cfg.map[opline - op_array->opcodes] : -1, op_array, ssa, ssa_op, frame->call_level, p + 1)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_SEND_VAL:
					case ZEND_SEND_VAL_EX:
						if (opline->op2_type == IS_CONST) {
							/* Named parameters not supported in JIT */
							break;
						}
						if (opline->opcode == ZEND_SEND_VAL_EX
						 && opline->op2.num > MAX_ARG_FLAG_NUM) {
							break;
						}
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						if (!zend_jit_send_val(&dasm_state, opline,
								op1_info, OP1_REG_ADDR())) {
							goto jit_failure;
						}
						if (frame->call
						 && frame->call->func
						 && frame->call->func->type == ZEND_USER_FUNCTION) {
							if (opline->op1_type == IS_CONST) {
								zend_jit_trace_send_type(opline, frame->call, Z_TYPE_P(RT_CONSTANT(opline, opline->op1)));
							} else if (op1_type != IS_UNKNOWN) {
								if (op1_type == IS_UNDEF) {
									op1_type = IS_NULL;
								}
								zend_jit_trace_send_type(opline, frame->call, op1_type);
							}
						}
						goto done;
					case ZEND_SEND_REF:
						if (opline->op2_type == IS_CONST) {
							/* Named parameters not supported in JIT */
							break;
						}
						op1_info = OP1_INFO();
						if (!zend_jit_send_ref(&dasm_state, opline, op_array,
								op1_info, 0)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_SEND_VAR:
					case ZEND_SEND_VAR_EX:
					case ZEND_SEND_VAR_NO_REF:
					case ZEND_SEND_VAR_NO_REF_EX:
					case ZEND_SEND_FUNC_ARG:
						if (opline->op2_type == IS_CONST) {
							/* Named parameters not supported in JIT */
							break;
						}
						if ((opline->opcode == ZEND_SEND_VAR_EX
						  || opline->opcode == ZEND_SEND_VAR_NO_REF_EX)
						 && opline->op2.num > MAX_ARG_FLAG_NUM) {
							break;
						}
						op1_addr = OP1_REG_ADDR();
						if (ra
						 && ssa_op->op1_def >= 0
						 && !ssa->vars[ssa_op->op1_def].no_val) {
							op1_def_addr = OP1_DEF_REG_ADDR();
						} else {
							op1_def_addr = op1_addr;
						}
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						if (!zend_jit_send_var(&dasm_state, opline, op_array,
								op1_info, op1_addr, op1_def_addr)) {
							goto jit_failure;
						}
						if (frame->call
						 && frame->call->func
						 && frame->call->func->type == ZEND_USER_FUNCTION) {
							if ((opline->opcode == ZEND_SEND_VAR_EX
							  || opline->opcode == ZEND_SEND_FUNC_ARG)
							 && ARG_SHOULD_BE_SENT_BY_REF(frame->call->func, opline->op2.num)) {
								goto done;
							}
							if (op1_type != IS_UNKNOWN) {
								if (op1_type == IS_UNDEF) {
									op1_type = IS_NULL;
								}
								zend_jit_trace_send_type(opline, frame->call, op1_type);
							}
						}
						goto done;
					case ZEND_CHECK_FUNC_ARG:
						if (opline->op2_type == IS_CONST) {
							/* Named parameters not supported in JIT */
							break;
						}
						if (opline->op2.num > MAX_ARG_FLAG_NUM
						 && (!JIT_G(current_frame)
						  || !JIT_G(current_frame)->call
						  || !JIT_G(current_frame)->call->func)) {
							break;
						}
						if (!zend_jit_check_func_arg(&dasm_state, opline)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_CHECK_UNDEF_ARGS:
						if (!zend_jit_check_undef_args(&dasm_state, opline)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_DO_UCALL:
					case ZEND_DO_ICALL:
					case ZEND_DO_FCALL_BY_NAME:
					case ZEND_DO_FCALL:
						if (!zend_jit_do_fcall(&dasm_state, opline, op_array, op_array_ssa, frame->call_level, -1, p + 1)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_IS_EQUAL:
					case ZEND_IS_NOT_EQUAL:
					case ZEND_IS_SMALLER:
					case ZEND_IS_SMALLER_OR_EQUAL:
					case ZEND_CASE:
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						op2_info = OP2_INFO();
						CHECK_OP2_TRACE_TYPE();
						if ((opline->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
							zend_bool exit_if_true = 0;
							const zend_op *exit_opline = zend_jit_trace_get_exit_opline(p + 1, opline + 1, &exit_if_true);
							uint32_t exit_point;

							if (ra) {
								zend_jit_trace_clenup_stack(stack, opline, ssa_op, ssa, ra);
							}
							exit_point = zend_jit_trace_get_exit_point(exit_opline, 0);
							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								goto jit_failure;
							}
							smart_branch_opcode = exit_if_true ? ZEND_JMPNZ : ZEND_JMPZ;
							zend_jit_trace_update_condition_ranges(opline, ssa_op, op_array, ssa, exit_if_true);
						} else {
							smart_branch_opcode = 0;
							exit_addr = NULL;
						}
						if (!zend_jit_cmp(&dasm_state, opline,
								op1_info, OP1_REG_ADDR(),
								op2_info, OP2_REG_ADDR(),
								RES_REG_ADDR(),
								zend_may_throw(opline, ssa_op, op_array, ssa),
								smart_branch_opcode, -1, -1, exit_addr)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_IS_IDENTICAL:
					case ZEND_IS_NOT_IDENTICAL:
					case ZEND_CASE_STRICT:
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						op2_info = OP2_INFO();
						CHECK_OP2_TRACE_TYPE();
						if ((opline->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
							zend_bool exit_if_true = 0;
							const zend_op *exit_opline = zend_jit_trace_get_exit_opline(p + 1, opline + 1, &exit_if_true);
							uint32_t exit_point;

							if (ra) {
								zend_jit_trace_clenup_stack(stack, opline, ssa_op, ssa, ra);
							}
							exit_point = zend_jit_trace_get_exit_point(exit_opline, 0);
							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								goto jit_failure;
							}
							if (opline->opcode == ZEND_IS_NOT_IDENTICAL) {
								exit_if_true = !exit_if_true;
							}
							smart_branch_opcode = exit_if_true ? ZEND_JMPNZ : ZEND_JMPZ;
							zend_jit_trace_update_condition_ranges(opline, ssa_op, op_array, ssa, exit_if_true);
						} else {
							smart_branch_opcode = 0;
							exit_addr = NULL;
						}
						if (!zend_jit_identical(&dasm_state, opline,
								op1_info, OP1_REG_ADDR(),
								op2_info, OP2_REG_ADDR(),
								RES_REG_ADDR(),
								zend_may_throw(opline, ssa_op, op_array, ssa),
								smart_branch_opcode, -1, -1, exit_addr)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_DEFINED:
						if ((opline->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
							zend_bool exit_if_true = 0;
							const zend_op *exit_opline = zend_jit_trace_get_exit_opline(p + 1, opline + 1, &exit_if_true);
							uint32_t exit_point = zend_jit_trace_get_exit_point(exit_opline, 0);

							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								goto jit_failure;
							}
							smart_branch_opcode = exit_if_true ? ZEND_JMPNZ : ZEND_JMPZ;
						} else {
							smart_branch_opcode = 0;
							exit_addr = NULL;
						}
						if (!zend_jit_defined(&dasm_state, opline, smart_branch_opcode, -1, -1, exit_addr)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_TYPE_CHECK:
						if (opline->extended_value == MAY_BE_RESOURCE) {
							// TODO: support for is_resource() ???
							break;
						}
						op1_info = OP1_INFO();
						if ((opline->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
							zend_bool exit_if_true = 0;
							const zend_op *exit_opline = zend_jit_trace_get_exit_opline(p + 1, opline + 1, &exit_if_true);
							uint32_t exit_point;

							if (ra) {
								zend_jit_trace_clenup_stack(stack, opline, ssa_op, ssa, ra);
							}
							exit_point = zend_jit_trace_get_exit_point(exit_opline, 0);
							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								goto jit_failure;
							}
							smart_branch_opcode = exit_if_true ? ZEND_JMPNZ : ZEND_JMPZ;
						} else {
							smart_branch_opcode = 0;
							exit_addr = NULL;
						}
						if (!zend_jit_type_check(&dasm_state, opline, op1_info, smart_branch_opcode, -1, -1, exit_addr)) {
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
							if (!zend_jit_trace_handler(&dasm_state, op_array, opline, zend_may_throw(opline, ssa_op, op_array, ssa), p + 1)) {
								goto jit_failure;
							}
						} else {
							int j;
							int may_throw = 0;

							if (!zend_jit_return(&dasm_state, opline, op_array,
									op1_info, OP1_REG_ADDR())) {
								goto jit_failure;
							}
						    if (!zend_jit_leave_frame(&dasm_state)) {
								goto jit_failure;
						    }
							for (j = 0 ; j < op_array->last_var; j++) {
								uint32_t info;
								zend_uchar type;

								if (opline->op1_type == IS_CV
								 && EX_VAR_TO_NUM(opline->op1.var) == j
								 && !(op1_info & MAY_BE_REF)
								 && JIT_G(current_frame)
								 && TRACE_FRAME_IS_RETURN_VALUE_USED(JIT_G(current_frame))) {
									continue;
								}
								info = zend_ssa_cv_info(op_array, op_array_ssa, j);
								type = STACK_TYPE(stack, j);
								info = zend_jit_trace_type_to_info_ex(type, info);
								if (info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF)) {
									if (!zend_jit_free_cv(&dasm_state, info, j)) {
										goto jit_failure;
									}
									if (info & (MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_ARRAY_OF_ARRAY|MAY_BE_ARRAY_OF_RESOURCE)) {
										if (info & MAY_BE_RC1) {
											may_throw = 1;
										}
									}
								}
							}
							if (!zend_jit_leave_func(&dasm_state, op_array, opline, op1_info,
									p + 1, &zend_jit_traces[ZEND_JIT_TRACE_NUM],
									(op_array_ssa->cfg.flags & ZEND_FUNC_INDIRECT_VAR_ACCESS) != 0, may_throw)) {
								goto jit_failure;
							}
						}
						goto done;
					case ZEND_BOOL:
					case ZEND_BOOL_NOT:
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						if (!zend_jit_bool_jmpznz(&dasm_state, opline,
								op1_info, OP1_REG_ADDR(), RES_REG_ADDR(),
								-1, -1,
								zend_may_throw(opline, ssa_op, op_array, ssa),
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
								ZEND_UNREACHABLE();
							}
							if (ra) {
								zend_jit_trace_clenup_stack(stack, opline, ssa_op, ssa, ra);
							}
							if (opline->result_type == IS_TMP_VAR) {
								zend_jit_trace_stack *stack = JIT_G(current_frame)->stack;
								uint32_t old_info = STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var));

								SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->result.var), IS_UNKNOWN);
								exit_point = zend_jit_trace_get_exit_point(exit_opline, 0);
								SET_STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var), old_info);
							} else {
								exit_point = zend_jit_trace_get_exit_point(exit_opline, 0);
							}
							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								goto jit_failure;
							}
						} else  {
							ZEND_UNREACHABLE();
						}
						if (opline->result_type == IS_UNDEF) {
							res_addr = 0;
						} else {
							res_addr = RES_REG_ADDR();
						}
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						if (!zend_jit_bool_jmpznz(&dasm_state, opline,
								op1_info, OP1_REG_ADDR(), res_addr,
								-1, -1,
								zend_may_throw(opline, ssa_op, op_array, ssa),
								smart_branch_opcode, exit_addr)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ISSET_ISEMPTY_CV:
						if ((opline->extended_value & ZEND_ISEMPTY)) {
							// TODO: support for empty() ???
							break;
						}
						op1_info = OP1_INFO();
						op1_addr = OP1_REG_ADDR();
						if (orig_op1_type != IS_UNKNOWN
						 && (orig_op1_type & IS_TRACE_REFERENCE)) {
							if (!zend_jit_fetch_reference(&dasm_state, opline, orig_op1_type, &op1_info, &op1_addr,
									!ssa->var_info[ssa_op->op1_use].guarded_reference, 1)) {
								goto jit_failure;
							}
							if (opline->op1_type == IS_CV
							 && zend_jit_var_may_alias(op_array, op_array_ssa, EX_VAR_TO_NUM(opline->op1.var)) == NO_ALIAS) {
								ssa->var_info[ssa_op->op1_use].guarded_reference = 1;
							}
						} else {
							CHECK_OP1_TRACE_TYPE();
						}
						if ((opline->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
							zend_bool exit_if_true = 0;
							const zend_op *exit_opline = zend_jit_trace_get_exit_opline(p + 1, opline + 1, &exit_if_true);
							uint32_t exit_point = zend_jit_trace_get_exit_point(exit_opline, 0);

							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								goto jit_failure;
							}
							smart_branch_opcode = exit_if_true ? ZEND_JMPNZ : ZEND_JMPZ;
						} else {
							smart_branch_opcode = 0;
							exit_addr = NULL;
						}
						if (!zend_jit_isset_isempty_cv(&dasm_state, opline,
								op1_info, op1_addr,
								smart_branch_opcode, -1, -1, exit_addr)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_IN_ARRAY:
						if (opline->op1_type == IS_VAR || opline->op1_type == IS_TMP_VAR) {
							break;
						}
						op1_info = OP1_INFO();
						op1_addr = OP1_REG_ADDR();
						CHECK_OP1_TRACE_TYPE();
						if ((op1_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF)) != MAY_BE_STRING) {
							break;
						}
						if ((opline->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
							zend_bool exit_if_true = 0;
							const zend_op *exit_opline = zend_jit_trace_get_exit_opline(p + 1, opline + 1, &exit_if_true);
							uint32_t exit_point = zend_jit_trace_get_exit_point(exit_opline, 0);

							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								goto jit_failure;
							}
							smart_branch_opcode = exit_if_true ? ZEND_JMPNZ : ZEND_JMPZ;
						} else {
							smart_branch_opcode = 0;
							exit_addr = NULL;
						}
						if (!zend_jit_in_array(&dasm_state, opline,
								op1_info, op1_addr,
								smart_branch_opcode, -1, -1, exit_addr)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FETCH_DIM_FUNC_ARG:
						if (!JIT_G(current_frame)
						 || !JIT_G(current_frame)->call
						 || !JIT_G(current_frame)->call->func
						 || !TRACE_FRAME_IS_LAST_SEND_BY_VAL(JIT_G(current_frame)->call)) {
							break;
						}
						/* break missing intentionally */
					case ZEND_FETCH_DIM_R:
					case ZEND_FETCH_DIM_IS:
					case ZEND_FETCH_LIST_R:
						op1_info = OP1_INFO();
						op1_addr = OP1_REG_ADDR();
						if (orig_op1_type != IS_UNKNOWN
						 && (orig_op1_type & IS_TRACE_REFERENCE)) {
							if (!zend_jit_fetch_reference(&dasm_state, opline, orig_op1_type, &op1_info, &op1_addr,
									!ssa->var_info[ssa_op->op1_use].guarded_reference, 1)) {
								goto jit_failure;
							}
							if (opline->op1_type == IS_CV
							 && zend_jit_var_may_alias(op_array, op_array_ssa, EX_VAR_TO_NUM(opline->op1.var)) == NO_ALIAS) {
								ssa->var_info[ssa_op->op1_use].guarded_reference = 1;
							}
						} else {
							CHECK_OP1_TRACE_TYPE();
						}
						op2_info = OP2_INFO();
						CHECK_OP2_TRACE_TYPE();
						res_info = RES_INFO();
						avoid_refcounting =
							ssa_op->op1_use >= 0 &&
							ssa->var_info[ssa_op->op1_use].avoid_refcounting;
						if (op1_info & MAY_BE_PACKED_GUARD) {
							ssa->var_info[ssa_op->op1_use].type &= ~MAY_BE_PACKED_GUARD;
						} else if ((op2_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_LONG
								&& (op1_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_ARRAY
								&& (op1_info & MAY_BE_ARRAY_PACKED)
								&& (op1_info & MAY_BE_ARRAY_HASH)
								&& orig_op1_type != IS_UNKNOWN) {
							op1_info |= MAY_BE_PACKED_GUARD;
							if (orig_op1_type & IS_TRACE_PACKED) {
								op1_info &= ~MAY_BE_ARRAY_HASH;
							} else {
								op1_info &= ~MAY_BE_ARRAY_PACKED;
							}
						}
						if (!zend_jit_fetch_dim_read(&dasm_state, opline, ssa, ssa_op,
								op1_info, op1_addr, avoid_refcounting,
								op2_info, res_info, RES_REG_ADDR(),
								(
									(op1_info & MAY_BE_ANY) != MAY_BE_ARRAY ||
									(op2_info & (MAY_BE_ANY - (MAY_BE_LONG|MAY_BE_STRING))) != 0 ||
									((op1_info & MAY_BE_UNDEF) != 0 &&
										opline->opcode != ZEND_FETCH_DIM_IS) ||
									(opline->opcode != ZEND_FETCH_LIST_R &&
										(opline->op1_type & (IS_TMP_VAR|IS_VAR)) != 0 &&
										(op1_info & MAY_BE_RC1) &&
										(op1_info & (MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_ARRAY_OF_RESOURCE|MAY_BE_ARRAY_OF_ARRAY)) != 0) ||
									(op2_info & MAY_BE_UNDEF) != 0 ||
									((opline->op2_type & (IS_TMP_VAR|IS_VAR)) != 0 &&
										(op2_info & MAY_BE_RC1) &&
										(op2_info & (MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_ARRAY_OF_RESOURCE|MAY_BE_ARRAY_OF_ARRAY)) != 0)))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FETCH_DIM_W:
					case ZEND_FETCH_DIM_RW:
//					case ZEND_FETCH_DIM_UNSET:
					case ZEND_FETCH_LIST_W:
						if (opline->op1_type != IS_CV
						 && (orig_op1_type == IS_UNKNOWN
						  || !(orig_op1_type & IS_TRACE_INDIRECT))) {
							break;
						}
						op1_info = OP1_INFO();
						op1_addr = OP1_REG_ADDR();
						if (opline->op1_type == IS_VAR) {
							if (orig_op1_type != IS_UNKNOWN
							 && (orig_op1_type & IS_TRACE_INDIRECT)) {
								if (!zend_jit_fetch_indirect_var(&dasm_state, opline, orig_op1_type,
										&op1_info, &op1_addr, !ssa->var_info[ssa_op->op1_use].indirect_reference)) {
									goto jit_failure;
								}
							} else {
								break;
							}
						}
						if (orig_op1_type != IS_UNKNOWN
						 && (orig_op1_type & IS_TRACE_REFERENCE)) {
							if (!zend_jit_fetch_reference(&dasm_state, opline, orig_op1_type, &op1_info, &op1_addr,
									!ssa->var_info[ssa_op->op1_use].guarded_reference, 1)) {
								goto jit_failure;
							}
							if (opline->op1_type == IS_CV
							 && zend_jit_var_may_alias(op_array, op_array_ssa, EX_VAR_TO_NUM(opline->op1.var)) == NO_ALIAS) {
								ssa->var_info[ssa_op->op1_use].guarded_reference = 1;
							}
						} else {
							CHECK_OP1_TRACE_TYPE();
						}
						op2_info = OP2_INFO();
						CHECK_OP2_TRACE_TYPE();
						op1_def_info = OP1_DEF_INFO();
						if (!zend_jit_fetch_dim(&dasm_state, opline,
								op1_info, op1_addr, op2_info, RES_REG_ADDR(),
								(opline->opcode == ZEND_FETCH_DIM_RW
								 || opline->op2_type == IS_UNUSED
								 || (op1_info & (MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))
								 || (op2_info & (MAY_BE_UNDEF|MAY_BE_RESOURCE|MAY_BE_ARRAY|MAY_BE_OBJECT))
								 || (opline->op1_type == IS_VAR
								  && (op1_info & MAY_BE_UNDEF)
								  && !ssa->var_info[ssa_op->op1_use].indirect_reference)))) {
							goto jit_failure;
						}
						if (ssa_op->result_def > 0
						 && (opline->opcode == ZEND_FETCH_DIM_W || opline->opcode == ZEND_FETCH_LIST_W)
						 && !(op1_info & (MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))
						 && !(op2_info & (MAY_BE_UNDEF|MAY_BE_RESOURCE|MAY_BE_ARRAY|MAY_BE_OBJECT))) {
							ssa->var_info[ssa_op->result_def].indirect_reference = 1;
						}
						goto done;
					case ZEND_ISSET_ISEMPTY_DIM_OBJ:
						if ((opline->extended_value & ZEND_ISEMPTY)) {
							// TODO: support for empty() ???
							break;
						}
						op1_info = OP1_INFO();
						op1_addr = OP1_REG_ADDR();
						if (orig_op1_type != IS_UNKNOWN
						 && (orig_op1_type & IS_TRACE_REFERENCE)) {
							if (!zend_jit_fetch_reference(&dasm_state, opline, orig_op1_type, &op1_info, &op1_addr,
									!ssa->var_info[ssa_op->op1_use].guarded_reference, 1)) {
								goto jit_failure;
							}
							if (opline->op1_type == IS_CV
							 && zend_jit_var_may_alias(op_array, op_array_ssa, EX_VAR_TO_NUM(opline->op1.var)) == NO_ALIAS) {
								ssa->var_info[ssa_op->op1_use].guarded_reference = 1;
							}
						} else {
							CHECK_OP1_TRACE_TYPE();
						}
						op2_info = OP2_INFO();
						CHECK_OP2_TRACE_TYPE();
						if ((opline->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
							zend_bool exit_if_true = 0;
							const zend_op *exit_opline = zend_jit_trace_get_exit_opline(p + 1, opline + 1, &exit_if_true);
							uint32_t exit_point;

							if (ra) {
								zend_jit_trace_clenup_stack(stack, opline, ssa_op, ssa, ra);
							}
							if (ssa_op->op1_use >= 0
							 && ssa->var_info[ssa_op->op1_use].avoid_refcounting) {
								/* Temporary reset ZREG_ZVAL_TRY_ADDREF */
								zend_jit_trace_stack *stack = JIT_G(current_frame)->stack;
								uint32_t old_info = STACK_INFO(stack, EX_VAR_TO_NUM(opline->op1.var));

								SET_STACK_REG(stack, EX_VAR_TO_NUM(opline->op1.var), ZREG_NONE);
								exit_point = zend_jit_trace_get_exit_point(exit_opline, 0);
								SET_STACK_INFO(stack, EX_VAR_TO_NUM(opline->op1.var), old_info);
							} else {
								exit_point = zend_jit_trace_get_exit_point(exit_opline, 0);
							}
							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								goto jit_failure;
							}
							smart_branch_opcode = exit_if_true ? ZEND_JMPNZ : ZEND_JMPZ;
						} else {
							smart_branch_opcode = 0;
							exit_addr = NULL;
						}
						avoid_refcounting =
							ssa_op->op1_use >= 0 &&
							ssa->var_info[ssa_op->op1_use].avoid_refcounting;
						if (op1_info & MAY_BE_PACKED_GUARD) {
							ssa->var_info[ssa_op->op1_use].type &= ~MAY_BE_PACKED_GUARD;
						} else if ((op2_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_LONG
								&& (op1_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_ARRAY
								&& (op1_info & MAY_BE_ARRAY_PACKED)
								&& (op1_info & MAY_BE_ARRAY_HASH)
								&& orig_op1_type != IS_UNKNOWN) {
							op1_info |= MAY_BE_PACKED_GUARD;
							if (orig_op1_type & IS_TRACE_PACKED) {
								op1_info &= ~MAY_BE_ARRAY_HASH;
							} else {
								op1_info &= ~MAY_BE_ARRAY_PACKED;
							}
						}
						if (!zend_jit_isset_isempty_dim(&dasm_state, opline,
								op1_info, op1_addr, avoid_refcounting,
								op2_info,
								zend_may_throw_ex(opline, ssa_op, op_array, ssa, op1_info, op2_info),
								smart_branch_opcode, -1, -1,
								exit_addr)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FETCH_OBJ_FUNC_ARG:
						if (!JIT_G(current_frame)
						 || !JIT_G(current_frame)->call
						 || !JIT_G(current_frame)->call->func
						 || !TRACE_FRAME_IS_LAST_SEND_BY_VAL(JIT_G(current_frame)->call)) {
							break;
						}
						/* break missing intentionally */
					case ZEND_FETCH_OBJ_R:
					case ZEND_FETCH_OBJ_IS:
					case ZEND_FETCH_OBJ_W:
						delayed_fetch_this = 0;
						avoid_refcounting = 0;
						if (opline->op2_type != IS_CONST
						 || Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) != IS_STRING
						 || Z_STRVAL_P(RT_CONSTANT(opline, opline->op2))[0] == '\0') {
							break;
						}
						ce = NULL;
						ce_is_instanceof = 0;
						op1_indirect = 0;
						if (opline->op1_type == IS_UNUSED) {
							op1_info = MAY_BE_OBJECT|MAY_BE_RC1|MAY_BE_RCN;
							ce = op_array->scope;
							ce_is_instanceof = (ce->ce_flags & ZEND_ACC_FINAL) != 0;
							op1_addr = 0;
						} else {
							op1_info = OP1_INFO();
							if (!(op1_info & MAY_BE_OBJECT)) {
								break;
							}
							op1_addr = OP1_REG_ADDR();
							if (opline->op1_type == IS_VAR
							 && opline->opcode == ZEND_FETCH_OBJ_W) {
								if (orig_op1_type != IS_UNKNOWN
								 && (orig_op1_type & IS_TRACE_INDIRECT)) {
									op1_indirect = 1;
									if (!zend_jit_fetch_indirect_var(&dasm_state, opline, orig_op1_type,
											&op1_info, &op1_addr, !ssa->var_info[ssa_op->op1_use].indirect_reference)) {
										goto jit_failure;
									}
								}
							}
							if (orig_op1_type != IS_UNKNOWN
							 && (orig_op1_type & IS_TRACE_REFERENCE)) {
								if (!zend_jit_fetch_reference(&dasm_state, opline, orig_op1_type, &op1_info, &op1_addr,
										!ssa->var_info[ssa_op->op1_use].guarded_reference, 1)) {
									goto jit_failure;
								}
								if (opline->op1_type == IS_CV
								 && zend_jit_var_may_alias(op_array, op_array_ssa, EX_VAR_TO_NUM(opline->op1.var)) == NO_ALIAS) {
									ssa->var_info[ssa_op->op1_use].guarded_reference = 1;
								}
							} else {
								CHECK_OP1_TRACE_TYPE();
							}
							if (ssa->var_info && ssa->ops) {
								if (ssa_op->op1_use >= 0) {
									zend_ssa_var_info *op1_ssa = ssa->var_info + ssa_op->op1_use;
									if (op1_ssa->ce && !op1_ssa->ce->create_object) {
										ce = op1_ssa->ce;
										ce_is_instanceof = op1_ssa->is_instanceof;
									}
								}
							}
							if (ssa_op->op1_use >= 0) {
								delayed_fetch_this = ssa->var_info[ssa_op->op1_use].delayed_fetch_this;
								avoid_refcounting = ssa->var_info[ssa_op->op1_use].avoid_refcounting;
							}
						}
						if (!zend_jit_fetch_obj(&dasm_state, opline, op_array, ssa, ssa_op,
								op1_info, op1_addr, op1_indirect, ce, ce_is_instanceof,
								delayed_fetch_this, avoid_refcounting, op1_ce,
								zend_may_throw_ex(opline, ssa_op, op_array, ssa, op1_info, MAY_BE_STRING))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_BIND_GLOBAL:
						orig_opline = opline;
						orig_ssa_op = ssa_op;
						while (1) {
							if (!ssa->ops || !ssa->var_info) {
								op1_info = MAY_BE_ANY|MAY_BE_REF;
							} else {
								op1_info = OP1_INFO();
							}
							if (!zend_jit_bind_global(&dasm_state, opline, op1_info)) {
								goto jit_failure;
							}
							if ((opline+1)->opcode == ZEND_BIND_GLOBAL) {
								opline++;
								ssa_op++;
							} else {
								break;
							}
						}
						opline = orig_opline;
						ssa_op = orig_ssa_op;
						goto done;
					case ZEND_RECV:
						if (!zend_jit_recv(&dasm_state, opline, op_array)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_RECV_INIT:
						orig_opline = opline;
						orig_ssa_op = ssa_op;
						while (1) {
							if (!zend_jit_recv_init(&dasm_state, opline, op_array,
									(opline + 1)->opcode != ZEND_RECV_INIT,
									zend_may_throw(opline, ssa_op, op_array, ssa))) {
								goto jit_failure;
							}
							if ((opline+1)->opcode == ZEND_RECV_INIT) {
								opline++;
								ssa_op++;
							} else {
								break;
							}
						}
						opline = orig_opline;
						ssa_op = orig_ssa_op;
						goto done;
					case ZEND_FREE:
					case ZEND_FE_FREE:
						op1_info = OP1_INFO();
						if (!zend_jit_free(&dasm_state, opline, op1_info,
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ECHO:
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						if ((op1_info & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)) != MAY_BE_STRING) {
							break;
						}
						if (!zend_jit_echo(&dasm_state, opline, op1_info)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_STRLEN:
						op1_info = OP1_INFO();
						op1_addr = OP1_REG_ADDR();
						if (orig_op1_type == (IS_TRACE_REFERENCE|IS_STRING)) {
							if (!zend_jit_fetch_reference(&dasm_state, opline, orig_op1_type, &op1_info, &op1_addr,
									!ssa->var_info[ssa_op->op1_use].guarded_reference, 1)) {
								goto jit_failure;
							}
							if (opline->op1_type == IS_CV
							 && zend_jit_var_may_alias(op_array, op_array_ssa, EX_VAR_TO_NUM(opline->op1.var)) == NO_ALIAS) {
								ssa->var_info[ssa_op->op1_use].guarded_reference = 1;
							}
						} else {
							CHECK_OP1_TRACE_TYPE();
							if ((op1_info & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)) != MAY_BE_STRING) {
								break;
							}
						}
						if (!zend_jit_strlen(&dasm_state, opline, op1_info, op1_addr)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FETCH_THIS:
						delayed_fetch_this = 0;
						if (ssa_op->result_def >= 0) {
							if (zend_jit_may_delay_fetch_this(ssa, ssa_opcodes, ssa_op->result_def)) {
								ssa->var_info[ssa_op->result_def].delayed_fetch_this = 1;
								delayed_fetch_this = 1;
							}
						}
						if (!zend_jit_fetch_this(&dasm_state, opline, op_array, delayed_fetch_this)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_SWITCH_LONG:
					case ZEND_SWITCH_STRING:
					case ZEND_MATCH:
						if (!zend_jit_switch(&dasm_state, opline, op_array, op_array_ssa, p+1, &zend_jit_traces[ZEND_JIT_TRACE_NUM])) {
							goto jit_failure;
						}
						goto done;
					case ZEND_VERIFY_RETURN_TYPE:
						if (opline->op1_type == IS_UNUSED) {
							/* Always throws */
							break;
						}
						if (opline->op1_type == IS_CONST) {
							/* TODO Different instruction format, has return value */
							break;
						}
						if (op_array->fn_flags & ZEND_ACC_RETURN_REFERENCE) {
							/* Not worth bothering with */
							break;
						}
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						if (op1_info & MAY_BE_REF) {
							/* TODO May need reference unwrapping. */
							break;
						}
						if (!zend_jit_verify_return_type(&dasm_state, opline, op_array, op1_info)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FE_RESET_R:
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						if ((op1_info & (MAY_BE_ANY|MAY_BE_REF)) != MAY_BE_ARRAY) {
							break;
						}
						if (!zend_jit_fe_reset(&dasm_state, opline, op1_info)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FE_FETCH_R:
						op1_info = OP1_INFO();
						CHECK_OP1_TRACE_TYPE();
						if ((op1_info & MAY_BE_ANY) != MAY_BE_ARRAY) {
							break;
						}
						if ((p+1)->op == ZEND_JIT_TRACE_VM || (p+1)->op == ZEND_JIT_TRACE_END) {
							const zend_op *exit_opline = ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value);
							uint32_t exit_point;

							if ((p+1)->opline == exit_opline) {
								/* taken branch (exit from loop) */
								exit_opline = opline;
								smart_branch_opcode = ZEND_NOP;
							} else if ((p+1)->opline == opline + 1) {
								/* not taken branch (loop) */
								smart_branch_opcode = ZEND_JMP;
							} else {
								ZEND_UNREACHABLE();
							}
							exit_point = zend_jit_trace_get_exit_point(exit_opline, 0);
							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								goto jit_failure;
							}
						} else  {
							ZEND_UNREACHABLE();
						}
						if (!zend_jit_fe_fetch(&dasm_state, opline, op1_info, OP2_INFO(),
								-1, smart_branch_opcode, exit_addr)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FETCH_CONSTANT:
						if (!zend_jit_fetch_constant(&dasm_state, opline)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_INIT_METHOD_CALL:
						if (opline->op2_type != IS_CONST
						 || Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) != IS_STRING) {
							goto generic_dynamic_call;
						}
						delayed_fetch_this = 0;
						ce = NULL;
						ce_is_instanceof = 0;
						if (opline->op1_type == IS_UNUSED) {
							op1_info = MAY_BE_OBJECT|MAY_BE_RC1|MAY_BE_RCN;
							ce = op_array->scope;
							ce_is_instanceof = (ce->ce_flags & ZEND_ACC_FINAL) != 0;
							op1_addr = 0;
						} else {
							op1_info = OP1_INFO();
							op1_addr = OP1_REG_ADDR();
							if (orig_op1_type != IS_UNKNOWN
							 && (orig_op1_type & IS_TRACE_REFERENCE)) {
								if (!zend_jit_fetch_reference(&dasm_state, opline, orig_op1_type, &op1_info, &op1_addr,
										!ssa->var_info[ssa_op->op1_use].guarded_reference, 1)) {
									goto jit_failure;
								}
								if (opline->op1_type == IS_CV
								 && zend_jit_var_may_alias(op_array, op_array_ssa, EX_VAR_TO_NUM(opline->op1.var)) == NO_ALIAS) {
									ssa->var_info[ssa_op->op1_use].guarded_reference = 1;
								}
							} else {
								CHECK_OP1_TRACE_TYPE();
							}
							if (ssa->var_info && ssa->ops) {
								if (ssa_op->op1_use >= 0) {
									zend_ssa_var_info *op1_ssa = ssa->var_info + ssa_op->op1_use;
									if (op1_ssa->ce && !op1_ssa->ce->create_object) {
										ce = op1_ssa->ce;
										ce_is_instanceof = op1_ssa->is_instanceof;
									}
								}
							}
							if (ssa_op->op1_use >= 0) {
								delayed_fetch_this = ssa->var_info[ssa_op->op1_use].delayed_fetch_this;
							}
						}
						if (!zend_jit_init_method_call(&dasm_state, opline,
								op_array_ssa->cfg.map ? op_array_ssa->cfg.map[opline - op_array->opcodes] : -1,
								op_array, ssa, ssa_op, frame->call_level,
								op1_info, op1_addr, ce, ce_is_instanceof, delayed_fetch_this, op1_ce,
								p + 1)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_INIT_DYNAMIC_CALL:
						if (orig_op2_type != IS_OBJECT || op2_ce != zend_ce_closure) {
							goto generic_dynamic_call;
						}
						op2_info = OP2_INFO();
						CHECK_OP2_TRACE_TYPE();
						if (!zend_jit_init_closure_call(&dasm_state, opline, op_array_ssa->cfg.map ? op_array_ssa->cfg.map[opline - op_array->opcodes] : -1, op_array, ssa, ssa_op, frame->call_level, p + 1)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_INIT_STATIC_METHOD_CALL:
generic_dynamic_call:
						if (!zend_jit_trace_handler(&dasm_state, op_array, opline, zend_may_throw(opline, ssa_op, op_array, ssa), p + 1)) {
							goto jit_failure;
						}
						if ((opline->opcode != ZEND_INIT_STATIC_METHOD_CALL
						  || opline->op1_type != IS_CONST
						  || opline->op2_type != IS_CONST)
						 && (p+1)->op == ZEND_JIT_TRACE_INIT_CALL && (p+1)->func) {
							if (!zend_jit_init_fcall_guard(&dasm_state, 0, (p+1)->func, opline+1)) {
								goto jit_failure;
							}
						}
						goto done;
					case ZEND_INIT_USER_CALL:
						if (!zend_jit_trace_handler(&dasm_state, op_array, opline, zend_may_throw(opline, ssa_op, op_array, ssa), p + 1)) {
							goto jit_failure;
						}
						if (opline->op2_type != IS_CONST
						 && (p+1)->op == ZEND_JIT_TRACE_INIT_CALL && (p+1)->func) {
							if (!zend_jit_init_fcall_guard(&dasm_state, 0, (p+1)->func, opline+1)) {
								goto jit_failure;
							}
						}
						goto done;
					case ZEND_NEW:
						if (!zend_jit_trace_handler(&dasm_state, op_array, opline, zend_may_throw(opline, ssa_op, op_array, ssa), p + 1)) {
							goto jit_failure;
						}
						if (opline->op1_type != IS_CONST
						 && (p+1)->op == ZEND_JIT_TRACE_INIT_CALL && (p+1)->func) {
							SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->result.var), IS_OBJECT);
							if (!zend_jit_init_fcall_guard(&dasm_state, 0, (p+1)->func, opline+1)) {
								goto jit_failure;
							}
						}
						goto done;
					case ZEND_SEND_ARRAY:
					case ZEND_SEND_UNPACK:
						if (JIT_G(current_frame)
						 && JIT_G(current_frame)->call) {
							TRACE_FRAME_SET_UNKNOWN_NUM_ARGS(JIT_G(current_frame)->call);
						}
						break;
					default:
						break;
				}
			}

			if (opline->opcode != ZEND_NOP && opline->opcode != ZEND_JMP) {
				if (!zend_jit_trace_handler(&dasm_state, op_array, opline, zend_may_throw(opline, ssa_op, op_array, ssa), p + 1)) {
					goto jit_failure;
				}
			}

done:
			switch (opline->opcode) {
				case ZEND_DO_FCALL:
				case ZEND_DO_ICALL:
				case ZEND_DO_UCALL:
				case ZEND_DO_FCALL_BY_NAME:
					frame->call_level--;
			}

			if (ra) {
				zend_jit_trace_clenup_stack(stack, opline, ssa_op, ssa, ra);
			}

			if ((opline->op1_type & (IS_VAR|IS_TMP_VAR))
			 && STACK_REG(stack, EX_VAR_TO_NUM(opline->op1.var)) > ZREG_NUM) {
				SET_STACK_REG(stack, EX_VAR_TO_NUM(opline->op1.var), ZREG_NONE);
			}

			if (ssa_op) {
				/* Keep information about known types on abstract stack */
				if (ssa_op->result_def >= 0) {
					zend_uchar type = IS_UNKNOWN;

					if ((opline->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0
					 || send_result) {
						/* we didn't set result variable */
						type = IS_UNKNOWN;
					} else if (!(ssa->var_info[ssa_op->result_def].type & MAY_BE_GUARD)
					 && has_concrete_type(ssa->var_info[ssa_op->result_def].type)) {
						type = concrete_type(ssa->var_info[ssa_op->result_def].type);
					} else if (opline->opcode == ZEND_QM_ASSIGN) {
						if (opline->op1_type != IS_CONST) {
							/* copy */
							type = STACK_VAR_TYPE(opline->op1.var);
						}
					} else if (opline->opcode == ZEND_ASSIGN) {
						if (opline->op2_type != IS_CONST
						 && ssa_op->op1_use >= 0
						 /* assignment to typed reference may cause conversion */
						 && (ssa->var_info[ssa_op->op1_use].type & MAY_BE_REF) == 0) {
							/* copy */
							type = STACK_VAR_TYPE(opline->op2.var);
						}
					} else if (opline->opcode == ZEND_POST_INC
			         || opline->opcode == ZEND_POST_DEC) {
						/* copy */
						type = STACK_VAR_TYPE(opline->op1.var);
					}
					SET_RES_STACK_VAR_TYPE(type);
					if (type != IS_UNKNOWN) {
						ssa->var_info[ssa_op->result_def].type &= ~MAY_BE_GUARD;
						if (opline->opcode == ZEND_FETCH_THIS
						 && delayed_fetch_this) {
							SET_STACK_REG(stack, EX_VAR_TO_NUM(opline->result.var), ZREG_THIS);
						} else if (ssa->var_info[ssa_op->result_def].avoid_refcounting) {
							SET_STACK_REG(stack, EX_VAR_TO_NUM(opline->result.var), ZREG_ZVAL_TRY_ADDREF);
						} else if (ra && ra[ssa_op->result_def]) {
							SET_STACK_REG(stack, EX_VAR_TO_NUM(opline->result.var), ra[ssa_op->result_def]->reg);
						}
					}
				}
				if (ssa_op->op1_def >= 0) {
					zend_uchar type = IS_UNKNOWN;

					if (!(ssa->var_info[ssa_op->op1_def].type & MAY_BE_GUARD)
					 && has_concrete_type(ssa->var_info[ssa_op->op1_def].type)) {
						type = concrete_type(ssa->var_info[ssa_op->op1_def].type);
					} else if (opline->opcode == ZEND_ASSIGN) {
						if (!(OP1_INFO() & MAY_BE_REF)
						 || STACK_VAR_TYPE(opline->op1.var) != IS_UNKNOWN) {
							if (opline->op2_type != IS_CONST) {
								/* copy */
								type = STACK_VAR_TYPE(opline->op2.var);
							}
						}
					} else if (opline->opcode == ZEND_SEND_VAR
					 || opline->opcode == ZEND_CAST
					 || opline->opcode == ZEND_QM_ASSIGN
					 || opline->opcode == ZEND_JMP_SET
					 || opline->opcode == ZEND_COALESCE
					 || opline->opcode == ZEND_JMP_NULL
					 || opline->opcode == ZEND_FE_RESET_R) {
						/* keep old value */
						type = STACK_VAR_TYPE(opline->op1.var);
					}
					SET_OP1_STACK_VAR_TYPE(type);
					if (type != IS_UNKNOWN) {
						ssa->var_info[ssa_op->op1_def].type &= ~MAY_BE_GUARD;
						if (ra && ra[ssa_op->op1_def]) {
							SET_STACK_REG(stack, EX_VAR_TO_NUM(opline->op1.var), ra[ssa_op->op1_def]->reg);
						}
					}
				}
				if (ssa_op->op2_def >= 0) {
					zend_uchar type = IS_UNKNOWN;

					if (!(ssa->var_info[ssa_op->op2_def].type & MAY_BE_GUARD)
					 && has_concrete_type(ssa->var_info[ssa_op->op2_def].type)) {
						type = concrete_type(ssa->var_info[ssa_op->op2_def].type);
					} else if (opline->opcode == ZEND_ASSIGN) {
						/* keep old value */
						type = STACK_VAR_TYPE(opline->op2.var);
					}
					SET_OP2_STACK_VAR_TYPE(type);
					if (type != IS_UNKNOWN) {
						ssa->var_info[ssa_op->op2_def].type &= ~MAY_BE_GUARD;
						if (ra && ra[ssa_op->op2_def]) {
							SET_STACK_REG(stack, EX_VAR_TO_NUM(opline->op2.var), ra[ssa_op->op2_def]->reg);
						}
					}
				}

				switch (opline->opcode) {
					case ZEND_ASSIGN_DIM:
					case ZEND_ASSIGN_OBJ:
					case ZEND_ASSIGN_STATIC_PROP:
					case ZEND_ASSIGN_DIM_OP:
					case ZEND_ASSIGN_OBJ_OP:
					case ZEND_ASSIGN_STATIC_PROP_OP:
					case ZEND_ASSIGN_OBJ_REF:
					case ZEND_ASSIGN_STATIC_PROP_REF:
						/* OP_DATA */
						ssa_op++;
						opline++;
						if (ssa_op->op1_def >= 0) {
							zend_uchar type = IS_UNKNOWN;

							if (!(ssa->var_info[ssa_op->op1_def].type & MAY_BE_GUARD)
							 && has_concrete_type(ssa->var_info[ssa_op->op1_def].type)) {
								type = concrete_type(ssa->var_info[ssa_op->op1_def].type);
							} else if ((opline-1)->opcode == ZEND_ASSIGN_DIM
							 || (opline-1)->opcode == ZEND_ASSIGN_OBJ
							 || (opline-1)->opcode == ZEND_ASSIGN_STATIC_PROP) {
								/* keep old value */
								type = STACK_VAR_TYPE(opline->op1.var);
							}
							SET_OP1_STACK_VAR_TYPE(type);
							if (type != IS_UNKNOWN) {
								ssa->var_info[ssa_op->op1_def].type &= ~MAY_BE_GUARD;
								if (ra && ra[ssa_op->op1_def]) {
									SET_STACK_REG(stack, EX_VAR_TO_NUM(opline->op1.var), ra[ssa_op->op1_def]->reg);
								}
							}
						}
						ssa_op++;
						break;
					case ZEND_RECV_INIT:
					    ssa_op++;
						opline++;
						while (opline->opcode == ZEND_RECV_INIT) {
							if (ssa_op->result_def >= 0) {
								zend_uchar type = IS_UNKNOWN;

								if (!(ssa->var_info[ssa_op->result_def].type & MAY_BE_GUARD)
								 && has_concrete_type(ssa->var_info[ssa_op->result_def].type)) {
									type = concrete_type(ssa->var_info[ssa_op->result_def].type);
								}
								SET_RES_STACK_VAR_TYPE(type);
								if (ra && ra[ssa_op->result_def]) {
									SET_STACK_REG(stack, EX_VAR_TO_NUM(opline->result.var), ra[ssa_op->result_def]->reg);
								}
							}
							ssa_op++;
							opline++;
						}
						break;
					case ZEND_BIND_GLOBAL:
						ssa_op++;
						opline++;
						while (opline->opcode == ZEND_BIND_GLOBAL) {
							if (ssa_op->op1_def >= 0) {
								zend_uchar type = IS_UNKNOWN;

								if (!(ssa->var_info[ssa_op->op1_def].type & MAY_BE_GUARD)
								 && has_concrete_type(ssa->var_info[ssa_op->op1_def].type)) {
									type = concrete_type(ssa->var_info[ssa_op->op1_def].type);
								}
								SET_OP1_STACK_VAR_TYPE(type);
								if (ra && ra[ssa_op->op1_def]) {
									SET_STACK_REG(stack, EX_VAR_TO_NUM(opline->op1.var), ra[ssa_op->op1_def]->reg);
								}
							}
							ssa_op++;
							opline++;
						}
						break;
					default:
						ssa_op += zend_jit_trace_op_len(opline);
						break;
				}

				if (send_result) {
					ssa_op++;
					send_result = 0;
				}
			}
		} else if (p->op == ZEND_JIT_TRACE_ENTER) {
			call = frame->call;
			assert(call && &call->func->op_array == p->op_array);

			if (opline->opcode == ZEND_DO_UCALL
			 || opline->opcode == ZEND_DO_FCALL_BY_NAME
			 || opline->opcode == ZEND_DO_FCALL) {

				frame->call_opline = opline;

				/* Check if SEND_UNPACK/SEND_ARRAY may cause enter at different opline */
				if (opline > op_array->opcodes) {
					const zend_op *prev_opline = opline - 1;

					while (prev_opline->opcode == ZEND_EXT_FCALL_BEGIN || prev_opline->opcode == ZEND_TICKS) {
						prev_opline--;
					}
					JIT_G(current_frame) = call;
					if ((prev_opline->opcode == ZEND_SEND_ARRAY
					  || prev_opline->opcode == ZEND_SEND_UNPACK
					  || prev_opline->opcode == ZEND_CHECK_UNDEF_ARGS)
					 && p->op_array->num_args
					 && (p->op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS) == 0
					 && ((p+1)->op == ZEND_JIT_TRACE_VM
					  || (p+1)->op == ZEND_JIT_TRACE_END)
					 && (TRACE_FRAME_NUM_ARGS(call) < 0
					  || TRACE_FRAME_NUM_ARGS(call) < p->op_array->num_args)
					 && !zend_jit_trace_opline_guard(&dasm_state, (p+1)->opline)) {
						goto jit_failure;
					}
					JIT_G(current_frame) = frame;
				}
			}

			if ((p+1)->op == ZEND_JIT_TRACE_END) {
				p++;
				break;
			}
			op_array = (zend_op_array*)p->op_array;
			jit_extension =
				(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
			op_array_ssa = &jit_extension->func_info.ssa;
			frame->call = call->prev;
			call->prev = frame;
			if (p->info & ZEND_JIT_TRACE_RETRUN_VALUE_USED) {
				TRACE_FRAME_SET_RETURN_VALUE_USED(call);
			} else {
				TRACE_FRAME_SET_RETURN_VALUE_UNUSED(call);
			}
			JIT_G(current_frame) = frame = call;
			stack = frame->stack;
			if (ra) {
				int j = ZEND_JIT_TRACE_GET_FIRST_SSA_VAR(p->info);

				for (i = 0; i < op_array->last_var; i++,j++) {
					if (ra[j] && (ra[j]->flags & ZREG_LOAD) != 0) {
						//SET_STACK_REG(stack, i, ra[j]->reg);
						if (!zend_jit_load_var(&dasm_state, ssa->var_info[j].type, i, ra[j]->reg)) {
							goto jit_failure;
						}
					}
				}
			}
		} else if (p->op == ZEND_JIT_TRACE_BACK) {
			op_array = (zend_op_array*)p->op_array;
			jit_extension =
				(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
			op_array_ssa = &jit_extension->func_info.ssa;
			top = frame;
			if (frame->prev) {
				frame = frame->prev;
				stack = frame->stack;
				ZEND_ASSERT(&frame->func->op_array == op_array);
			} else {
				frame = zend_jit_trace_ret_frame(frame, op_array);
				TRACE_FRAME_INIT(frame, op_array, TRACE_FRAME_MASK_UNKNOWN_RETURN, -1);
				stack = frame->stack;
				if (JIT_G(opt_level) >= ZEND_JIT_LEVEL_INLINE) {
					uint32_t j = ZEND_JIT_TRACE_GET_FIRST_SSA_VAR(p->info);

					for (i = 0; i < op_array->last_var + op_array->T; i++, j++) {
						/* Initialize abstract stack using SSA */
						if (!(ssa->var_info[j].type & MAY_BE_GUARD)
						 && has_concrete_type(ssa->var_info[j].type)) {
							SET_STACK_TYPE(stack, i, concrete_type(ssa->var_info[j].type));
						} else {
							SET_STACK_TYPE(stack, i, IS_UNKNOWN);
						}
					}
					if (ra) {
						j = ZEND_JIT_TRACE_GET_FIRST_SSA_VAR(p->info);
						for (i = 0; i < op_array->last_var + op_array->T; i++, j++) {
							if (ra[j] && (ra[j]->flags & ZREG_LOAD) != 0) {
								//SET_STACK_REG(stack, i, ra[j]->reg);
								if (!zend_jit_load_var(&dasm_state, ssa->var_info[j].type, i, ra[j]->reg)) {
									goto jit_failure;
								}
							}
						}
					}
				} else {
					for (i = 0; i < op_array->last_var + op_array->T; i++) {
						SET_STACK_TYPE(stack, i, IS_UNKNOWN);
					}
				}
				opline = NULL;
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
			const zend_op *init_opline = zend_jit_trace_find_init_fcall_op(p, op_array);
			int num_args = -1;

			if (init_opline
			 && init_opline->extended_value <= TRACE_FRAME_MAX_NUM_ARGS) {
				num_args = init_opline->extended_value;
			}

			call = top;
			TRACE_FRAME_INIT(call, p->func, TRACE_FRAME_MASK_NESTED, num_args);
			call->prev = frame->call;
			if (!(p->info & ZEND_JIT_TRACE_FAKE_INIT_CALL)) {
				TRACE_FRAME_SET_LAST_SEND_BY_VAL(call);
			}
			if (init_opline
			 && init_opline->opcode != ZEND_NEW
			 && (init_opline->opcode != ZEND_INIT_METHOD_CALL
			  || init_opline->op1_type == IS_UNDEF)
			 && (init_opline->opcode != ZEND_INIT_USER_CALL
			  || init_opline->op2_type == IS_CONST) /* no closure */
			 && (init_opline->opcode != ZEND_INIT_DYNAMIC_CALL
			  || init_opline->op2_type == IS_CONST) /* no closure */
			) {
				TRACE_FRAME_SET_NO_NEED_RELEASE_THIS(call);
			}
			frame->call = call;
			top = zend_jit_trace_call_frame(top, p->op_array);
			if (p->func) {
				if (p->func->type == ZEND_USER_FUNCTION) {
					if (JIT_G(opt_level) >= ZEND_JIT_LEVEL_INLINE) {
						zend_jit_op_array_trace_extension *jit_extension =
							(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(p->op_array);

						i = 0;
						while (i < p->op_array->num_args) {
							/* Types of arguments are going to be stored in abstract stack when processing SEV instruction */
							SET_STACK_TYPE(call->stack, i, IS_UNKNOWN);
							i++;
						}
						while (i < p->op_array->last_var) {
							if (jit_extension
							 && zend_jit_var_may_alias(p->op_array, &jit_extension->func_info.ssa, i) != NO_ALIAS) {
								SET_STACK_TYPE(call->stack, i, IS_UNKNOWN);
							} else {
								SET_STACK_TYPE(call->stack, i, IS_UNDEF);
							}
							i++;
						}
						while (i < p->op_array->last_var + p->op_array->T) {
							SET_STACK_TYPE(call->stack, i, IS_UNKNOWN);
							i++;
						}
					} else {
						for (i = 0; i < p->op_array->last_var + p->op_array->T; i++) {
							SET_STACK_TYPE(call->stack, i, IS_UNKNOWN);
						}
					}
				}
				if (p->info & ZEND_JIT_TRACE_FAKE_INIT_CALL) {
					int skip_guard = 0;

					if (init_opline) {
						zend_call_info *call_info = jit_extension->func_info.callee_info;

						while (call_info) {
							if (call_info->caller_init_opline == init_opline) {
								if (op_array->fn_flags & ZEND_ACC_TRAIT_CLONE) {
									if (init_opline->opcode == ZEND_INIT_STATIC_METHOD_CALL
									 && init_opline->op1_type != IS_CONST) {
										break;
									} else if (init_opline->opcode == ZEND_INIT_METHOD_CALL) {
										break;
									}
								}
								skip_guard = 1;
								break;
							}
							call_info = call_info->next_callee;
						}
						if (!skip_guard
						 && !zend_jit_may_be_polymorphic_call(init_opline)) {
							// TODO: recompilation may change target ???
							skip_guard = 1;
						}
					}

					if (!skip_guard) {
						if (!opline) {
							zend_jit_trace_rec *q = p + 1;
							while (q->op != ZEND_JIT_TRACE_VM && q->op != ZEND_JIT_TRACE_END) {
								q++;
							}
							opline = q->opline;
							ZEND_ASSERT(opline != NULL);
						}
						if (!zend_jit_init_fcall_guard(&dasm_state,
								ZEND_JIT_TRACE_FAKE_LEVEL(p->info), p->func, opline)) {
							goto jit_failure;
						}
					}
				}
			}
			if (p->info & ZEND_JIT_TRACE_FAKE_INIT_CALL) {
				frame->call_level++;
			}
		} else if (p->op == ZEND_JIT_TRACE_DO_ICALL) {
			call = frame->call;
			if (call) {
				top = call;
				frame->call = call->prev;
			}
		} else {
			ZEND_UNREACHABLE();
		}
	}

	ZEND_ASSERT(p->op == ZEND_JIT_TRACE_END);

	t = &zend_jit_traces[ZEND_JIT_TRACE_NUM];

	if (!parent_trace && zend_jit_trace_uses_initial_ip()) {
		t->flags |= ZEND_JIT_TRACE_USES_INITIAL_IP;
	}

	if (p->stop == ZEND_JIT_TRACE_STOP_LOOP
	 || p->stop == ZEND_JIT_TRACE_STOP_RECURSIVE_CALL
	 || p->stop == ZEND_JIT_TRACE_STOP_RECURSIVE_RET) {
		if (p->stop != ZEND_JIT_TRACE_STOP_RECURSIVE_RET) {
			ZEND_ASSERT(!parent_trace);
			if ((t->flags & ZEND_JIT_TRACE_USES_INITIAL_IP)
			 && !zend_jit_set_ip(&dasm_state, p->opline)) {
				goto jit_failure;
			}
		}
		t->link = ZEND_JIT_TRACE_NUM;
		if (p->stop != ZEND_JIT_TRACE_STOP_RECURSIVE_RET) {
			t->flags |= ZEND_JIT_TRACE_CHECK_INTERRUPT;
		}
		if (!(t->flags & ZEND_JIT_TRACE_LOOP)) {
			const void *timeout_exit_addr = NULL;

			t->flags |= ZEND_JIT_TRACE_LOOP;

			if (trace_buffer->stop != ZEND_JIT_TRACE_STOP_RECURSIVE_RET) {
				if (!(t->flags & ZEND_JIT_TRACE_USES_INITIAL_IP)
				 || (ra
				  && zend_jit_trace_stack_needs_deoptimization(stack, op_array->last_var + op_array->T))) {
					uint32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);

					timeout_exit_addr = zend_jit_trace_get_exit_addr(exit_point);
					if (!timeout_exit_addr) {
						goto jit_failure;
					}
				} else {
					timeout_exit_addr = dasm_labels[zend_lbinterrupt_handler];
				}
			}

			zend_jit_trace_end_loop(&dasm_state, 0, timeout_exit_addr); /* jump back to start of the trace loop */
		}
	} else if (p->stop == ZEND_JIT_TRACE_STOP_LINK
	        || p->stop == ZEND_JIT_TRACE_STOP_INTERPRETER) {
		if (!zend_jit_trace_deoptimization(&dasm_state, 0, NULL,
				stack, op_array->last_var + op_array->T, NULL, NULL)) {
			goto jit_failure;
		}
		if (p->stop == ZEND_JIT_TRACE_STOP_LINK) {
			const void *timeout_exit_addr = NULL;

			t->link = zend_jit_find_trace(p->opline->handler);
			if ((zend_jit_traces[t->link].flags & ZEND_JIT_TRACE_USES_INITIAL_IP)
			 && !zend_jit_set_ip(&dasm_state, p->opline)) {
				goto jit_failure;
			}
			if (!parent_trace && zend_jit_trace_uses_initial_ip()) {
				t->flags |= ZEND_JIT_TRACE_USES_INITIAL_IP;
			}
			if (parent_trace
			 && (zend_jit_traces[t->link].flags & ZEND_JIT_TRACE_CHECK_INTERRUPT)
			 && zend_jit_traces[parent_trace].root == t->link) {
				if (!(zend_jit_traces[t->link].flags & ZEND_JIT_TRACE_USES_INITIAL_IP)) {
					uint32_t exit_point;

					for (i = 0; i < op_array->last_var + op_array->T; i++) {
						SET_STACK_TYPE(stack, i, IS_UNKNOWN);
					}
					exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
					timeout_exit_addr = zend_jit_trace_get_exit_addr(exit_point);
					if (!timeout_exit_addr) {
						goto jit_failure;
					}
				} else {
					timeout_exit_addr = dasm_labels[zend_lbinterrupt_handler];
				}
			}
			zend_jit_trace_link_to_root(&dasm_state, &zend_jit_traces[t->link], timeout_exit_addr);
		} else {
			zend_jit_trace_return(&dasm_state, 0);
		}
	} else if (p->stop == ZEND_JIT_TRACE_STOP_RETURN) {
		zend_jit_trace_return(&dasm_state, 0);
	} else {
		// TODO: not implemented ???
		ZEND_ASSERT(0 && p->stop);
	}

	if (ZEND_JIT_EXIT_COUNTERS + t->exit_count >= JIT_G(max_exit_counters)) {
		goto jit_failure;
	}

	handler = dasm_link_and_encode(&dasm_state, NULL, NULL, NULL, NULL, ZSTR_VAL(name), ZEND_JIT_TRACE_NUM);

	if (handler) {
		if (p->stop == ZEND_JIT_TRACE_STOP_RECURSIVE_CALL) {
			const zend_op_array *rec_op_array;

			rec_op_array = op_array = trace_buffer->op_array;
			jit_extension =
				(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
			p = trace_buffer + ZEND_JIT_TRACE_START_REC_SIZE;
			for (;;p++) {
				if (p->op == ZEND_JIT_TRACE_VM) {
					opline = p->opline;
				} else if (p->op == ZEND_JIT_TRACE_ENTER) {
					if (p->op_array == rec_op_array) {
						zend_jit_trace_setup_ret_counter(opline, jit_extension->offset);
					}
					op_array = p->op_array;
					jit_extension =
						(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
				} else if (p->op == ZEND_JIT_TRACE_BACK) {
					op_array = p->op_array;
					jit_extension =
						(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
				} else if (p->op == ZEND_JIT_TRACE_END) {
					break;
				}
			}
		} else if (p->stop == ZEND_JIT_TRACE_STOP_LINK
		        || p->stop == ZEND_JIT_TRACE_STOP_INTERPRETER) {
			if (opline
			 && (opline->opcode == ZEND_DO_UCALL
			  || opline->opcode == ZEND_DO_FCALL
			  || opline->opcode == ZEND_DO_FCALL_BY_NAME
			  || opline->opcode == ZEND_YIELD
			  || opline->opcode == ZEND_YIELD_FROM
			  || opline->opcode == ZEND_INCLUDE_OR_EVAL)) {
				zend_jit_trace_setup_ret_counter(opline, jit_extension->offset);
			}
			if (JIT_G(current_frame)
			 && JIT_G(current_frame)->prev) {
				frame = JIT_G(current_frame)->prev;
				do {
					if (frame->call_opline) {
						op_array = &frame->func->op_array;
						jit_extension =
							(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
						zend_jit_trace_setup_ret_counter(frame->call_opline, jit_extension->offset);
					}
					frame = frame->prev;
				} while (frame);
			}
		}
	}

jit_failure:
	dasm_free(&dasm_state);

	if (name) {
		zend_string_release(name);
	}

jit_cleanup:
	/* Clenup used op_arrays */
	while (num_op_arrays > 0) {
		op_array = op_arrays[--num_op_arrays];
		jit_extension =
			(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);

	    jit_extension->func_info.num = 0;
		jit_extension->func_info.flags &= ZEND_FUNC_JIT_ON_FIRST_EXEC
			| ZEND_FUNC_JIT_ON_PROF_REQUEST
			| ZEND_FUNC_JIT_ON_HOT_COUNTERS
			| ZEND_FUNC_JIT_ON_HOT_TRACE;
		memset(&jit_extension->func_info.ssa, 0, sizeof(zend_func_info) - offsetof(zend_func_info, ssa));
	}

	zend_arena_release(&CG(arena), checkpoint);

	JIT_G(current_frame) = NULL;
	JIT_G(current_trace) = NULL;

	return handler;
}

static const void *zend_jit_trace_exit_to_vm(uint32_t trace_num, uint32_t exit_num)
{
	const void *handler = NULL;
	dasm_State* dasm_state = NULL;
	void *checkpoint;
	char name[32];
	const zend_op *opline;
	uint32_t stack_size;
	zend_jit_trace_stack *stack;
	zend_bool original_handler = 0;

	if (!zend_jit_trace_exit_needs_deoptimization(trace_num, exit_num)) {
		return dasm_labels[zend_lbtrace_escape];
	}

	checkpoint = zend_arena_checkpoint(CG(arena));;

	sprintf(name, "ESCAPE-%d-%d", trace_num, exit_num);

	dasm_init(&dasm_state, DASM_MAXSECTION);
	dasm_setupglobal(&dasm_state, dasm_labels, zend_lb_MAX);
	dasm_setup(&dasm_state, dasm_actions);

	zend_jit_align_func(&dasm_state);

	/* Deoptimization */
	stack_size = zend_jit_traces[trace_num].exit_info[exit_num].stack_size;
	stack = zend_jit_traces[trace_num].stack_map + zend_jit_traces[trace_num].exit_info[exit_num].stack_offset;

	if (!zend_jit_trace_deoptimization(&dasm_state,
			zend_jit_traces[trace_num].exit_info[exit_num].flags,
			zend_jit_traces[trace_num].exit_info[exit_num].opline,
			stack, stack_size, NULL, NULL)) {
		goto jit_failure;
	}

	opline = zend_jit_traces[trace_num].exit_info[exit_num].opline;
	if (opline) {
		zend_jit_set_ip(&dasm_state, opline);
		if (opline == zend_jit_traces[zend_jit_traces[trace_num].root].opline) {
			/* prevent endless loop */
			original_handler = 1;
		}
	}

	zend_jit_trace_return(&dasm_state, original_handler);

	handler = dasm_link_and_encode(&dasm_state, NULL, NULL, NULL, NULL, name, ZEND_JIT_TRACE_NUM);

jit_failure:
	dasm_free(&dasm_state);
	zend_arena_release(&CG(arena), checkpoint);
	return handler;
}

static zend_jit_trace_stop zend_jit_compile_root_trace(zend_jit_trace_rec *trace_buffer, const zend_op *opline, size_t offset)
{
	zend_jit_trace_stop ret;
	const void *handler;
	uint8_t orig_trigger;
	zend_jit_trace_info *t = NULL;
	zend_jit_trace_exit_info exit_info[ZEND_JIT_TRACE_MAX_EXITS];

	zend_shared_alloc_lock();

	/* Checks under lock */
	if ((ZEND_OP_TRACE_INFO(opline, offset)->trace_flags & ZEND_JIT_TRACE_JITED)) {
		ret = ZEND_JIT_TRACE_STOP_ALREADY_DONE;
	} else if (ZEND_JIT_TRACE_NUM >= JIT_G(max_root_traces)) {
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
		t->flags = 0;
		t->polymorphism = 0;
		t->jmp_table_size = 0;
		t->opline = trace_buffer[1].opline;
		t->exit_info = exit_info;
		t->stack_map = NULL;

		orig_trigger = JIT_G(trigger);
		JIT_G(trigger) = ZEND_JIT_ON_HOT_TRACE;

		handler = zend_jit_trace(trace_buffer, 0, 0);

		JIT_G(trigger) = orig_trigger;

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
				zend_jit_trace_stack *shared_stack_map = (zend_jit_trace_stack*)zend_shared_alloc(t->stack_map_size * sizeof(zend_jit_trace_stack));
				if (!shared_stack_map) {
					ret = ZEND_JIT_TRACE_STOP_NO_SHM;
					goto exit;
				}
				memcpy(shared_stack_map, t->stack_map, t->stack_map_size * sizeof(zend_jit_trace_stack));
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
		           ZEND_JIT_EXIT_COUNTERS + t->exit_count >= JIT_G(max_exit_counters)) {
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

	if ((JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_EXIT_INFO) != 0
	 && ret == ZEND_JIT_TRACE_STOP_COMPILED
	 && t->exit_count > 0) {
		zend_jit_dump_exit_info(t);
	}

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
			if (cache_count[i] >= JIT_G(blacklist_root_trace) - 1) {
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

static void zend_jit_dump_trace(zend_jit_trace_rec *trace_buffer, zend_ssa *tssa)
{
	zend_jit_trace_rec *p = trace_buffer;
	const zend_op_array *op_array;
	const zend_op *opline;
	uint32_t level = 1 + trace_buffer[0].level;
	int idx, len, i, v, vars_count, call_level;

	ZEND_ASSERT(p->op == ZEND_JIT_TRACE_START);
	op_array = p->op_array;
	p += ZEND_JIT_TRACE_START_REC_SIZE;
	idx = 0;
	call_level = 0;

	if (tssa && tssa->var_info) {
		if (trace_buffer->start == ZEND_JIT_TRACE_START_ENTER) {
			vars_count = op_array->last_var;
		} else {
			vars_count = op_array->last_var + op_array->T;
		}
		for (i = 0; i < vars_count; i++) {
			if (tssa->vars[i].use_chain >= 0 || tssa->vars[i].phi_use_chain) {
				fprintf(stderr, "    %*c;", level, ' ');
				zend_dump_ssa_var(op_array, tssa, i, 0, i, ZEND_DUMP_RC_INFERENCE);
				fprintf(stderr, "\n");
			}
		}
		if (trace_buffer->stop == ZEND_JIT_TRACE_STOP_LOOP
		 || trace_buffer->stop == ZEND_JIT_TRACE_STOP_RECURSIVE_CALL
		 || trace_buffer->stop == ZEND_JIT_TRACE_STOP_RECURSIVE_RET) {
			fprintf(stderr, "LOOP:\n");
			if (trace_buffer->stop == ZEND_JIT_TRACE_STOP_LOOP) {
				zend_ssa_phi *p = tssa->blocks[1].phis;

				while (p) {
					fprintf(stderr, "     ;");
					zend_dump_ssa_var(op_array, tssa, p->ssa_var, 0, p->var, ZEND_DUMP_RC_INFERENCE);
					fprintf(stderr, " = Phi(");
					zend_dump_ssa_var(op_array, tssa, p->sources[0], 0, p->var, ZEND_DUMP_RC_INFERENCE);
					fprintf(stderr, ", ");
					zend_dump_ssa_var(op_array, tssa, p->sources[1], 0, p->var, ZEND_DUMP_RC_INFERENCE);
					fprintf(stderr, ")\n");
					p = p->next;
				}
			}
		}
	}

	while (1) {
		if (p->op == ZEND_JIT_TRACE_VM) {
			uint8_t op1_type, op2_type, op3_type;

			opline = p->opline;
			fprintf(stderr, "%04d%*c",
				(int)(opline - op_array->opcodes),
				level, ' ');
			zend_dump_op(op_array, NULL, opline, ZEND_DUMP_RC_INFERENCE, tssa, (tssa && tssa->ops) ? tssa->ops + idx : NULL);

			op1_type = p->op1_type;
			op2_type = p->op2_type;
			op3_type = p->op3_type;
			if (op1_type != IS_UNKNOWN || op2_type != IS_UNKNOWN || op3_type != IS_UNKNOWN) {
				fprintf(stderr, " ;");
				if (op1_type != IS_UNKNOWN) {
					const char *ref = (op1_type & IS_TRACE_INDIRECT) ?
						((op1_type & IS_TRACE_REFERENCE) ? "*&" : "*") :
						((op1_type & IS_TRACE_REFERENCE) ? "&" : "");
					if ((p+1)->op == ZEND_JIT_TRACE_OP1_TYPE) {
						p++;
						fprintf(stderr, " op1(%sobject of class %s)", ref,
							ZSTR_VAL(p->ce->name));
					} else {
						const char *type = (op1_type == 0) ? "undef" : zend_get_type_by_const(op1_type & ~(IS_TRACE_REFERENCE|IS_TRACE_INDIRECT|IS_TRACE_PACKED));
						fprintf(stderr, " op1(%s%s%s)", ref, (op1_type & IS_TRACE_PACKED) ? "packed " : "", type);
					}
				}
				if (op2_type != IS_UNKNOWN) {
					const char *ref = (op2_type & IS_TRACE_INDIRECT) ?
						((op2_type & IS_TRACE_REFERENCE) ? "*&" : "*") :
						((op2_type & IS_TRACE_REFERENCE) ? "&" : "");
					if ((p+1)->op == ZEND_JIT_TRACE_OP2_TYPE) {
						p++;
						fprintf(stderr, " op2(%sobject of class %s)", ref,
							ZSTR_VAL(p->ce->name));
					} else {
						const char *type = (op2_type == 0) ? "undef" : zend_get_type_by_const(op2_type & ~(IS_TRACE_REFERENCE|IS_TRACE_INDIRECT));
						fprintf(stderr, " op2(%s%s)", ref, type);
					}
				}
				if (op3_type != IS_UNKNOWN) {
					const char *ref = (op3_type & IS_TRACE_INDIRECT) ?
						((op3_type & IS_TRACE_REFERENCE) ? "*&" : "*") :
						((op3_type & IS_TRACE_REFERENCE) ? "&" : "");
					const char *type = (op3_type == 0) ? "undef" : zend_get_type_by_const(op3_type & ~(IS_TRACE_REFERENCE|IS_TRACE_INDIRECT));
					fprintf(stderr, " op3(%s%s)", ref, type);
				}
			}
			fprintf(stderr, "\n");
			idx++;

			len = zend_jit_trace_op_len(opline);
			while (len > 1) {
				opline++;
				fprintf(stderr, "%04d%*c;",
					(int)(opline - op_array->opcodes),
					level, ' ');
				zend_dump_op(op_array, NULL, opline, ZEND_DUMP_RC_INFERENCE, tssa, (tssa && tssa->ops) ? tssa->ops + idx : NULL);
				idx++;
				len--;
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
			if (tssa && tssa->var_info) {
				call_level++;
				v = ZEND_JIT_TRACE_GET_FIRST_SSA_VAR(p->info);
				vars_count = op_array->last_var;
				for (i = 0; i < vars_count; i++, v++) {
					if (tssa->vars[v].use_chain >= 0 || tssa->vars[v].phi_use_chain) {
						fprintf(stderr, "    %*c;", level, ' ');
						zend_dump_ssa_var(op_array, tssa, v, 0, i, ZEND_DUMP_RC_INFERENCE);
						fprintf(stderr, "\n");
					}
				}
			}
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
			if (tssa && tssa->var_info) {
				if (call_level == 0) {
					v = ZEND_JIT_TRACE_GET_FIRST_SSA_VAR(p->info);
					vars_count = op_array->last_var + op_array->T;
					for (i = 0; i < vars_count; i++, v++) {
						if (tssa->vars[v].use_chain >= 0 || tssa->vars[v].phi_use_chain) {
							fprintf(stderr, "    %*c;", level, ' ');
							zend_dump_ssa_var(op_array, tssa, v, 0, i, ZEND_DUMP_RC_INFERENCE);
							fprintf(stderr, "\n");
						}
					}
				} else {
					call_level--;
				}
			}
		} else if (p->op == ZEND_JIT_TRACE_INIT_CALL) {
			if (p->func != (zend_function*)&zend_pass_function) {
				fprintf(stderr, (p->info & ZEND_JIT_TRACE_FAKE_INIT_CALL) ? "    %*c>fake_init %s%s%s\n" : "    %*c>init %s%s%s\n",
					level, ' ',
					(p->func && p->func->common.scope) ? ZSTR_VAL(p->func->common.scope->name) : "",
					(p->func && p->func->common.scope) ? "::" : "",
					p->func ? ZSTR_VAL(p->func->common.function_name) : "???");
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

static void zend_jit_dump_exit_info(zend_jit_trace_info *t)
{
	int i, j;

	fprintf(stderr, "---- TRACE %d exit info\n", t->id);
	for (i = 0; i < t->exit_count; i++) {
		const zend_op_array *op_array = t->exit_info[i].op_array;
		uint32_t stack_size = t->exit_info[i].stack_size;
		zend_jit_trace_stack *stack = t->stack_map + t->exit_info[i].stack_offset;

		fprintf(stderr, "     exit_%d:", i);
		if (t->exit_info[i].opline) {
			fprintf(stderr, " %04d/", (int)(t->exit_info[i].opline - op_array->opcodes));
		} else {
			fprintf(stderr, " ----/");
		}
		if (t->exit_info[i].stack_size) {
			fprintf(stderr, "%04d/%d", t->exit_info[i].stack_offset, t->exit_info[i].stack_size);
		} else {
			fprintf(stderr, "----/0");
		}
		if (t->exit_info[i].flags & ZEND_JIT_EXIT_TO_VM) {
			fprintf(stderr, "/VM");
		}
		if (t->exit_info[i].flags & ZEND_JIT_EXIT_RESTORE_CALL) {
			fprintf(stderr, "/CALL");
		}
		if (t->exit_info[i].flags & (ZEND_JIT_EXIT_POLYMORPHISM|ZEND_JIT_EXIT_DYNAMIC_CALL)) {
			fprintf(stderr, "/POLY");
		}
		if (t->exit_info[i].flags & ZEND_JIT_EXIT_FREE_OP1) {
			fprintf(stderr, "/FREE_OP1");
		}
		if (t->exit_info[i].flags & ZEND_JIT_EXIT_FREE_OP2) {
			fprintf(stderr, "/FREE_OP2");
		}
		for (j = 0; j < stack_size; j++) {
			zend_uchar type = STACK_TYPE(stack, j);
			if (type != IS_UNKNOWN) {
				fprintf(stderr, " ");
				zend_dump_var(op_array, (j < op_array->last_var) ? IS_CV : 0, j);
				fprintf(stderr, ":");
				if (type == IS_UNDEF) {
					fprintf(stderr, "undef");
				} else {
					fprintf(stderr, "%s", zend_get_type_by_const(type));
				}
				if (STACK_REG(stack, j) != ZREG_NONE) {
					if (STACK_REG(stack, j) < ZREG_NUM) {
						fprintf(stderr, "(%s)", zend_reg_name[STACK_REG(stack, j)]);
					} else if (STACK_REG(stack, j) == ZREG_THIS) {
						fprintf(stderr, "(this)");
					} else if (STACK_REG(stack, j) == ZREG_ZVAL_TRY_ADDREF) {
						fprintf(stderr, "(zval_try_addref)");
					} else {
						fprintf(stderr, "(const_%d)", STACK_REG(stack, j) - ZREG_NUM);
					}
				}
			} else if (STACK_REG(stack, j) == ZREG_ZVAL_TRY_ADDREF) {
				fprintf(stderr, " ");
				zend_dump_var(op_array, (j < op_array->last_var) ? IS_CV : 0, j);
				fprintf(stderr, ":unknown(zval_try_addref)");
			} else if (STACK_REG(stack, j) == ZREG_ZVAL_COPY_R0) {
				fprintf(stderr, " ");
				zend_dump_var(op_array, (j < op_array->last_var) ? IS_CV : 0, j);
				fprintf(stderr, ":unknown(zval_copy(%s))", zend_reg_name[0]);
			}
		}
		fprintf(stderr, "\n");
	}
}

int ZEND_FASTCALL zend_jit_trace_hot_root(zend_execute_data *execute_data, const zend_op *opline)
{
	const zend_op *orig_opline;
	zend_jit_trace_stop stop;
	int ret = 0;
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

	if (JIT_G(tracing)) {
		++(*ZEND_OP_TRACE_INFO(opline, offset)->counter);
		return 0;
	}

	if (JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_START) {
		fprintf(stderr, "---- TRACE %d start (%s) %s%s%s() %s:%d\n",
			trace_num,
			zend_jit_trace_star_desc(ZEND_OP_TRACE_INFO(opline, offset)->trace_flags),
			EX(func)->op_array.scope ? ZSTR_VAL(EX(func)->op_array.scope->name) : "",
			EX(func)->op_array.scope ? "::" : "",
			EX(func)->op_array.function_name ?
				ZSTR_VAL(EX(func)->op_array.function_name) : "$main",
			ZSTR_VAL(EX(func)->op_array.filename),
			opline->lineno);
	}

	if (ZEND_JIT_TRACE_NUM >= JIT_G(max_root_traces)) {
		stop = ZEND_JIT_TRACE_STOP_TOO_MANY_TRACES;
		goto abort;
	}

	JIT_G(tracing) = 1;
	stop = zend_jit_trace_execute(execute_data, opline, trace_buffer,
		ZEND_OP_TRACE_INFO(opline, offset)->trace_flags & ZEND_JIT_TRACE_START_MASK, 0);
	JIT_G(tracing) = 0;

	if (stop & ZEND_JIT_TRACE_HALT) {
		ret = -1;
	}
	stop &= ~ZEND_JIT_TRACE_HALT;

	if (UNEXPECTED(trace_buffer[1].opline != orig_opline)) {
		orig_opline = trace_buffer[1].opline;
		op_array = (zend_op_array*)trace_buffer[0].op_array;
		jit_extension = (zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
		offset = jit_extension->offset;
		if (JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_START) {
			const zend_op_array *op_array = trace_buffer[0].op_array;
			const zend_op *opline = trace_buffer[1].opline;
			zend_jit_op_array_trace_extension *jit_extension =
				(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
			size_t offset = jit_extension->offset;

			fprintf(stderr, "---- TRACE %d start (%s) %s%s%s() %s:%d\n",
				trace_num,
				zend_jit_trace_star_desc(ZEND_OP_TRACE_INFO(opline, offset)->trace_flags),
				op_array->scope ? ZSTR_VAL(op_array->scope->name) : "",
				op_array->scope ? "::" : "",
				op_array->function_name ?
					ZSTR_VAL(op_array->function_name) : "$main",
				ZSTR_VAL(op_array->filename),
				opline->lineno);
		}
	}

	if (UNEXPECTED(JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_BYTECODE)) {
		zend_jit_dump_trace(trace_buffer, NULL);
	}

	if (ZEND_JIT_TRACE_STOP_OK(stop)) {
		if (JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_STOP) {
			if (stop == ZEND_JIT_TRACE_STOP_LINK) {
				uint32_t idx = trace_buffer[1].last;
				uint32_t link_to = zend_jit_find_trace(trace_buffer[idx].opline->handler);
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
			if (JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_COMPILED) {
				fprintf(stderr, "---- TRACE %d %s\n",
					trace_num,
					zend_jit_trace_stop_description[stop]);
			}
		} else {
			goto abort;
		}
	} else {
abort:
		if (JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_ABORT) {
			fprintf(stderr, "---- TRACE %d abort (%s)\n",
				trace_num,
				zend_jit_trace_stop_description[stop]);
		}
		if (!ZEND_JIT_TRACE_STOP_MAY_RECOVER(stop)
		 || zend_jit_trace_is_bad_root(orig_opline, stop, offset)) {
			if (JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_BLACKLIST) {
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

	if (JIT_G(debug) & (ZEND_JIT_DEBUG_TRACE_STOP|ZEND_JIT_DEBUG_TRACE_ABORT|ZEND_JIT_DEBUG_TRACE_COMPILED|ZEND_JIT_DEBUG_TRACE_BLACKLIST)) {
		fprintf(stderr, "\n");
	}

	return ret;
}

static void zend_jit_blacklist_trace_exit(uint32_t trace_num, uint32_t exit_num)
{
	const void *handler;

	zend_shared_alloc_lock();

	if (!(zend_jit_traces[trace_num].exit_info[exit_num].flags & (ZEND_JIT_EXIT_JITED|ZEND_JIT_EXIT_BLACKLISTED))) {
		SHM_UNPROTECT();
		zend_jit_unprotect();

		handler = zend_jit_trace_exit_to_vm(trace_num, exit_num);

		if (handler) {
			zend_jit_link_side_trace(
				zend_jit_traces[trace_num].code_start,
				zend_jit_traces[trace_num].code_size,
				zend_jit_traces[trace_num].jmp_table_size,
				exit_num,
				handler);
		}

		zend_jit_traces[trace_num].exit_info[exit_num].flags |= ZEND_JIT_EXIT_BLACKLISTED;

		zend_jit_protect();
		SHM_PROTECT();
	}

	zend_shared_alloc_unlock();
}

static zend_bool zend_jit_trace_exit_is_bad(uint32_t trace_num, uint32_t exit_num)
{
	uint8_t *counter = JIT_G(exit_counters) +
		zend_jit_traces[trace_num].exit_counters + exit_num;

	if (*counter + 1 >= JIT_G(hot_side_exit) + JIT_G(blacklist_side_trace)) {
		return 1;
	}
	(*counter)++;
	return 0;
}

static zend_bool zend_jit_trace_exit_is_hot(uint32_t trace_num, uint32_t exit_num)
{
	uint8_t *counter = JIT_G(exit_counters) +
		zend_jit_traces[trace_num].exit_counters + exit_num;

	if (*counter + 1 >= JIT_G(hot_side_exit)) {
		return 1;
	}
	(*counter)++;
	return 0;
}

static zend_jit_trace_stop zend_jit_compile_side_trace(zend_jit_trace_rec *trace_buffer, uint32_t parent_num, uint32_t exit_num, uint32_t polymorphism)
{
	zend_jit_trace_stop ret;
	const void *handler;
	uint8_t orig_trigger;
	zend_jit_trace_info *t;
	zend_jit_trace_exit_info exit_info[ZEND_JIT_TRACE_MAX_EXITS];

	zend_shared_alloc_lock();

	/* Checks under lock */
	if (zend_jit_traces[parent_num].exit_info[exit_num].flags & (ZEND_JIT_EXIT_JITED|ZEND_JIT_EXIT_BLACKLISTED)) {
		ret = ZEND_JIT_TRACE_STOP_ALREADY_DONE;
	} else if (ZEND_JIT_TRACE_NUM >= JIT_G(max_root_traces)) {
		ret = ZEND_JIT_TRACE_STOP_TOO_MANY_TRACES;
	} else if (zend_jit_traces[zend_jit_traces[parent_num].root].child_count >= JIT_G(max_side_traces)) {
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
		t->flags = 0;
		t->polymorphism = polymorphism;
		t->jmp_table_size = 0;
		t->opline = NULL;
		t->exit_info = exit_info;
		t->stack_map = NULL;

		orig_trigger = JIT_G(trigger);
		JIT_G(trigger) = ZEND_JIT_ON_HOT_TRACE;

		handler = zend_jit_trace(trace_buffer, parent_num, exit_num);

		JIT_G(trigger) = orig_trigger;

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
				zend_jit_trace_stack *shared_stack_map = (zend_jit_trace_stack*)zend_shared_alloc(t->stack_map_size * sizeof(zend_jit_trace_stack));
				if (!shared_stack_map) {
					efree(t->stack_map);
					ret = ZEND_JIT_TRACE_STOP_NO_SHM;
					goto exit;
				}
				memcpy(shared_stack_map, t->stack_map, t->stack_map_size * sizeof(zend_jit_trace_stack));
				efree(t->stack_map);
				t->stack_map = shared_stack_map;
		    }

			zend_jit_link_side_trace(
				zend_jit_traces[parent_num].code_start,
				zend_jit_traces[parent_num].code_size,
				zend_jit_traces[parent_num].jmp_table_size,
				exit_num,
				handler);

			t->exit_counters = ZEND_JIT_EXIT_COUNTERS;
			ZEND_JIT_EXIT_COUNTERS += t->exit_count;

			zend_jit_traces[zend_jit_traces[parent_num].root].child_count++;
			ZEND_JIT_TRACE_NUM++;
			zend_jit_traces[parent_num].exit_info[exit_num].flags |= ZEND_JIT_EXIT_JITED;

			ret = ZEND_JIT_TRACE_STOP_COMPILED;
		} else if (t->exit_count >= ZEND_JIT_TRACE_MAX_EXITS ||
		           ZEND_JIT_EXIT_COUNTERS + t->exit_count >= JIT_G(max_exit_counters)) {
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

	if ((JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_EXIT_INFO) != 0
	 && ret == ZEND_JIT_TRACE_STOP_COMPILED
	 && t->exit_count > 0) {
		zend_jit_dump_exit_info(t);
	}

	return ret;
}

int ZEND_FASTCALL zend_jit_trace_hot_side(zend_execute_data *execute_data, uint32_t parent_num, uint32_t exit_num)
{
	zend_jit_trace_stop stop;
	int ret = 0;
	uint32_t trace_num;
	zend_jit_trace_rec trace_buffer[ZEND_JIT_TRACE_MAX_LENGTH];
	uint32_t is_megamorphic = 0;
	uint32_t polymorphism = 0;

	trace_num = ZEND_JIT_TRACE_NUM;

	/* Lock-free check if the side trace was already JIT-ed or blacklist-ed in another process */
	if (zend_jit_traces[parent_num].exit_info[exit_num].flags & (ZEND_JIT_EXIT_JITED|ZEND_JIT_EXIT_BLACKLISTED)) {
		return 0;
	}

	if (JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_START) {
		fprintf(stderr, "---- TRACE %d start (side trace %d/%d) %s%s%s() %s:%d\n",
			trace_num, parent_num, exit_num,
			EX(func)->op_array.scope ? ZSTR_VAL(EX(func)->op_array.scope->name) : "",
			EX(func)->op_array.scope ? "::" : "",
			EX(func)->op_array.function_name ?
				ZSTR_VAL(EX(func)->op_array.function_name) : "$main",
			ZSTR_VAL(EX(func)->op_array.filename),
			EX(opline)->lineno);
	}

	if (ZEND_JIT_TRACE_NUM >= JIT_G(max_root_traces)) {
		stop = ZEND_JIT_TRACE_STOP_TOO_MANY_TRACES;
		goto abort;
	}

	if (zend_jit_traces[zend_jit_traces[parent_num].root].child_count >= JIT_G(max_side_traces)) {
		stop = ZEND_JIT_TRACE_STOP_TOO_MANY_CHILDREN;
		goto abort;
	}

	if (JIT_G(max_polymorphic_calls) > 0) {
		if ((zend_jit_traces[parent_num].exit_info[exit_num].flags & ZEND_JIT_EXIT_DYNAMIC_CALL)
		 || ((zend_jit_traces[parent_num].exit_info[exit_num].flags & ZEND_JIT_EXIT_POLYMORPHISM)
		  && EX(call))) {
			if (zend_jit_traces[parent_num].polymorphism >= JIT_G(max_polymorphic_calls) - 1) {
				is_megamorphic = zend_jit_traces[parent_num].exit_info[exit_num].flags &
					(ZEND_JIT_EXIT_DYNAMIC_CALL | ZEND_JIT_EXIT_POLYMORPHISM);
			} else if (!zend_jit_traces[parent_num].polymorphism) {
				polymorphism = 1;
			} else if (exit_num == 0) {
				polymorphism = zend_jit_traces[parent_num].polymorphism + 1;
			}
		}
	}

	JIT_G(tracing) = 1;
	stop = zend_jit_trace_execute(execute_data, EX(opline), trace_buffer, ZEND_JIT_TRACE_START_SIDE, is_megamorphic);
	JIT_G(tracing) = 0;

	if (stop & ZEND_JIT_TRACE_HALT) {
		ret = -1;
	}
	stop &= ~ZEND_JIT_TRACE_HALT;

	if (UNEXPECTED(trace_buffer->start != ZEND_JIT_TRACE_START_SIDE)) {
		if (JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_START) {
			const zend_op_array *op_array = trace_buffer[0].op_array;
			const zend_op *opline = trace_buffer[1].opline;
			zend_jit_op_array_trace_extension *jit_extension =
				(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
			size_t offset = jit_extension->offset;

			fprintf(stderr, "---- TRACE %d start (%s) %s%s%s() %s:%d\n",
				trace_num,
				zend_jit_trace_star_desc(ZEND_OP_TRACE_INFO(opline, offset)->trace_flags),
				op_array->scope ? ZSTR_VAL(op_array->scope->name) : "",
				op_array->scope ? "::" : "",
				op_array->function_name ?
					ZSTR_VAL(op_array->function_name) : "$main",
				ZSTR_VAL(op_array->filename),
				opline->lineno);
		}
	}

	if (UNEXPECTED(JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_BYTECODE)) {
		zend_jit_dump_trace(trace_buffer, NULL);
	}

	if (ZEND_JIT_TRACE_STOP_OK(stop)) {
		if (JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_STOP) {
			if (stop == ZEND_JIT_TRACE_STOP_LINK) {
				uint32_t idx = trace_buffer[1].last;
				uint32_t link_to = zend_jit_find_trace(trace_buffer[idx].opline->handler);;
				fprintf(stderr, "---- TRACE %d stop (link to %d)\n",
					trace_num,
					link_to);
			} else {
				fprintf(stderr, "---- TRACE %d stop (%s)\n",
					trace_num,
					zend_jit_trace_stop_description[stop]);
			}
		}
		if (EXPECTED(trace_buffer->start == ZEND_JIT_TRACE_START_SIDE)) {
			stop = zend_jit_compile_side_trace(trace_buffer, parent_num, exit_num, polymorphism);
		} else {
			const zend_op_array *op_array = trace_buffer[0].op_array;
			zend_jit_op_array_trace_extension *jit_extension =
				(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
			const zend_op *opline = trace_buffer[1].opline;

			stop = zend_jit_compile_root_trace(trace_buffer, opline, jit_extension->offset);
		}
		if (EXPECTED(ZEND_JIT_TRACE_STOP_DONE(stop))) {
			if (JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_COMPILED) {
				fprintf(stderr, "---- TRACE %d %s\n",
					trace_num,
					zend_jit_trace_stop_description[stop]);
			}
		} else {
			goto abort;
		}
	} else {
abort:
		if (JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_ABORT) {
			fprintf(stderr, "---- TRACE %d abort (%s)\n",
				trace_num,
				zend_jit_trace_stop_description[stop]);
		}
		if (!ZEND_JIT_TRACE_STOP_MAY_RECOVER(stop)
		 || zend_jit_trace_exit_is_bad(parent_num, exit_num)) {
			zend_jit_blacklist_trace_exit(parent_num, exit_num);
			if (JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_BLACKLIST) {
				fprintf(stderr, "---- EXIT %d/%d blacklisted\n",
					parent_num, exit_num);
			}
		}
		if (ZEND_JIT_TRACE_STOP_REPEAT(stop)) {
			execute_data = EG(current_execute_data);
			return zend_jit_trace_hot_root(execute_data, EX(opline));
		}
	}

	if (JIT_G(debug) & (ZEND_JIT_DEBUG_TRACE_STOP|ZEND_JIT_DEBUG_TRACE_ABORT|ZEND_JIT_DEBUG_TRACE_COMPILED|ZEND_JIT_DEBUG_TRACE_BLACKLIST)) {
		fprintf(stderr, "\n");
	}

	return ret;
}

int ZEND_FASTCALL zend_jit_trace_exit(uint32_t exit_num, zend_jit_registers_buf *regs)
{
	uint32_t trace_num = EG(jit_trace_num);
	zend_execute_data *execute_data = EG(current_execute_data);
	const zend_op *orig_opline = EX(opline);
	const zend_op *opline;
	zend_jit_trace_info *t = &zend_jit_traces[trace_num];

	/* Deoptimizatoion of VM stack state */
	uint32_t i;
	uint32_t stack_size = t->exit_info[exit_num].stack_size;
	zend_jit_trace_stack *stack = t->stack_map + t->exit_info[exit_num].stack_offset;

	if (t->exit_info[exit_num].flags & ZEND_JIT_EXIT_RESTORE_CALL) {
		zend_execute_data *call = (zend_execute_data *)regs->r[ZREG_RX];
		call->prev_execute_data = EX(call);
		EX(call) = call;
	}

	for (i = 0; i < stack_size; i++) {
		if (STACK_REG(stack, i) != ZREG_NONE) {
			if (STACK_TYPE(stack, i) == IS_LONG) {
				zend_long val;

				if (STACK_REG(stack, i) < ZREG_NUM) {
					val = regs->r[STACK_REG(stack, i)];
				} else if (STACK_REG(stack, i) == ZREG_LONG_MIN) {
					val = ZEND_LONG_MIN;
				} else if (STACK_REG(stack, i) == ZREG_LONG_MAX) {
					val = ZEND_LONG_MAX;
				} else {
					ZEND_UNREACHABLE();
				}
				ZVAL_LONG(EX_VAR_NUM(i), val);
			} else if (STACK_TYPE(stack, i) == IS_DOUBLE) {
				double val;

				if (STACK_REG(stack, i) < ZREG_NUM) {
					val = regs->xmm[STACK_REG(stack, i) - ZREG_XMM0];
				} else if (STACK_REG(stack, i) == ZREG_LONG_MIN_MINUS_1) {
					val = (double)ZEND_LONG_MIN - 1.0;
				} else if (STACK_REG(stack, i) == ZREG_LONG_MAX_PLUS_1) {
					val = (double)ZEND_LONG_MAX + 1.0;
				} else {
					ZEND_UNREACHABLE();
				}
				ZVAL_DOUBLE(EX_VAR_NUM(i), val);
			} else if (STACK_REG(stack, i) == ZREG_THIS) {
				zend_object *obj = Z_OBJ(EX(This));

				GC_ADDREF(obj);
				ZVAL_OBJ(EX_VAR_NUM(i), obj);
			} else if (STACK_REG(stack, i) == ZREG_NULL) {
				ZVAL_NULL(EX_VAR_NUM(i));
			} else if (STACK_REG(stack, i) == ZREG_ZVAL_TRY_ADDREF) {
				Z_TRY_ADDREF_P(EX_VAR_NUM(i));
			} else if (STACK_REG(stack, i) == ZREG_ZVAL_COPY_R0) {
				zval *val = (zval*)regs->r[0];

				if (UNEXPECTED(Z_TYPE_P(val) == IS_UNDEF)) {
					/* Undefined array index or property */
					if (JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_EXIT) {
						fprintf(stderr, "     TRACE %d exit %d %s%s%s() %s:%d\n",
							trace_num,
							exit_num,
							EX(func)->op_array.scope ? ZSTR_VAL(EX(func)->op_array.scope->name) : "",
							EX(func)->op_array.scope ? "::" : "",
							EX(func)->op_array.function_name ?
								ZSTR_VAL(EX(func)->op_array.function_name) : "$main",
							ZSTR_VAL(EX(func)->op_array.filename),
							EX(opline)->lineno);
					}
					EX(opline) = t->exit_info[exit_num].opline - 1;
					return 0;
				} else {
					ZVAL_COPY(EX_VAR_NUM(i), val);
				}
			} else {
				ZEND_UNREACHABLE();
			}
		}
	}

	opline = t->exit_info[exit_num].opline;

	if (opline) {
		if (t->exit_info[exit_num].flags & ZEND_JIT_EXIT_FREE_OP2) {
			ZEND_ASSERT((opline-1)->opcode == ZEND_FETCH_DIM_R
					|| (opline-1)->opcode == ZEND_FETCH_DIM_IS
					|| (opline-1)->opcode == ZEND_FETCH_LIST_R
					|| (opline-1)->opcode == ZEND_FETCH_DIM_FUNC_ARG);
			EX(opline) = opline-1;
			zval_ptr_dtor_nogc(EX_VAR((opline-1)->op2.var));
		}
		if (t->exit_info[exit_num].flags & ZEND_JIT_EXIT_FREE_OP1) {
			ZEND_ASSERT((opline-1)->opcode == ZEND_FETCH_DIM_R
					|| (opline-1)->opcode == ZEND_FETCH_DIM_IS
					|| (opline-1)->opcode == ZEND_FETCH_DIM_FUNC_ARG
					|| (opline-1)->opcode == ZEND_FETCH_OBJ_R
					|| (opline-1)->opcode == ZEND_FETCH_OBJ_IS
					|| (opline-1)->opcode == ZEND_FETCH_OBJ_FUNC_ARG);
			EX(opline) = opline-1;
			zval_ptr_dtor_nogc(EX_VAR((opline-1)->op1.var));
		}
		if (t->exit_info[exit_num].flags & (ZEND_JIT_EXIT_FREE_OP1|ZEND_JIT_EXIT_FREE_OP2)) {
			if (EG(exception)) {
				return 1;
			}
		}

		/* Set VM opline to continue interpretation */
		EX(opline) = opline;
	}

	if (EG(vm_interrupt) || JIT_G(tracing)) {
		return 1;
	/* Lock-free check if the side trace was already JIT-ed or blacklist-ed in another process */
	} else if (t->exit_info[exit_num].flags & (ZEND_JIT_EXIT_JITED|ZEND_JIT_EXIT_BLACKLISTED)) {
		return 0;
	}

	ZEND_ASSERT(EX(func)->type == ZEND_USER_FUNCTION);
	ZEND_ASSERT(EX(opline) >= EX(func)->op_array.opcodes &&
		EX(opline) < EX(func)->op_array.opcodes + EX(func)->op_array.last);

	if (JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_EXIT) {
		fprintf(stderr, "     TRACE %d exit %d %s%s%s() %s:%d\n",
			trace_num,
			exit_num,
			EX(func)->op_array.scope ? ZSTR_VAL(EX(func)->op_array.scope->name) : "",
			EX(func)->op_array.scope ? "::" : "",
			EX(func)->op_array.function_name ?
				ZSTR_VAL(EX(func)->op_array.function_name) : "$main",
			ZSTR_VAL(EX(func)->op_array.filename),
			EX(opline)->lineno);
	}

	if (t->exit_info[exit_num].flags & ZEND_JIT_EXIT_TO_VM) {
		if (zend_jit_trace_exit_is_bad(trace_num, exit_num)) {
			zend_jit_blacklist_trace_exit(trace_num, exit_num);
			if (JIT_G(debug) & ZEND_JIT_DEBUG_TRACE_BLACKLIST) {
				fprintf(stderr, "---- EXIT %d/%d blacklisted\n",
					trace_num, exit_num);
			}
			return 0;
		}
	} else if (JIT_G(hot_side_exit) && zend_jit_trace_exit_is_hot(trace_num, exit_num)) {
		return zend_jit_trace_hot_side(execute_data, trace_num, exit_num);
	}

	/* Return 1 to call original handler instead of the same JIT-ed trace */
	return (orig_opline == t->opline && EX(opline) == orig_opline);
}

static zend_always_inline uint8_t zend_jit_trace_supported(const zend_op *opline)
{
	switch (opline->opcode) {
		case ZEND_CATCH:
		case ZEND_FAST_CALL:
		case ZEND_FAST_RET:
			return ZEND_JIT_TRACE_UNSUPPORTED;
		default:
			return ZEND_JIT_TRACE_SUPPORTED;
	}
}

static int zend_jit_restart_hot_trace_counters(zend_op_array *op_array)
{
	zend_jit_op_array_trace_extension *jit_extension;
	uint32_t i;

	jit_extension = (zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
	for (i = 0; i < op_array->last; i++) {
		jit_extension->trace_info[i].trace_flags &=
			ZEND_JIT_TRACE_START_LOOP | ZEND_JIT_TRACE_START_ENTER | ZEND_JIT_TRACE_UNSUPPORTED;
		if (jit_extension->trace_info[i].trace_flags == ZEND_JIT_TRACE_START_LOOP) {
			op_array->opcodes[i].handler = (const void*)zend_jit_loop_trace_counter_handler;
		} else if (jit_extension->trace_info[i].trace_flags == ZEND_JIT_TRACE_START_ENTER) {
			op_array->opcodes[i].handler = (const void*)zend_jit_func_trace_counter_handler;
		} else {
			op_array->opcodes[i].handler = jit_extension->trace_info[i].orig_handler;
		}
	}
	return SUCCESS;
}

static int zend_jit_setup_hot_trace_counters(zend_op_array *op_array)
{
	zend_op *opline;
	zend_jit_op_array_trace_extension *jit_extension;
	uint32_t i;

	ZEND_ASSERT(sizeof(zend_op_trace_info) == sizeof(zend_op));

	jit_extension = (zend_jit_op_array_trace_extension*)zend_shared_alloc(sizeof(zend_jit_op_array_trace_extension) + (op_array->last - 1) * sizeof(zend_op_trace_info));
	memset(&jit_extension->func_info, 0, sizeof(zend_func_info));
	jit_extension->func_info.flags = ZEND_FUNC_JIT_ON_HOT_TRACE;
	jit_extension->op_array = op_array;
	jit_extension->offset = (char*)jit_extension->trace_info - (char*)op_array->opcodes;
	for (i = 0; i < op_array->last; i++) {
		jit_extension->trace_info[i].orig_handler = op_array->opcodes[i].handler;
		jit_extension->trace_info[i].call_handler = zend_get_opcode_handler_func(&op_array->opcodes[i]);
		jit_extension->trace_info[i].counter = NULL;
		jit_extension->trace_info[i].trace_flags =
			zend_jit_trace_supported(&op_array->opcodes[i]);
	}
	ZEND_SET_FUNC_INFO(op_array, (void*)jit_extension);

	if (JIT_G(hot_loop)) {
		zend_cfg cfg;

		ZEND_ASSERT(zend_jit_loop_trace_counter_handler != NULL);

		if (zend_jit_build_cfg(op_array, &cfg) != SUCCESS) {
			return FAILURE;
		}

		for (i = 0; i < cfg.blocks_count; i++) {
			if (cfg.blocks[i].flags & ZEND_BB_REACHABLE) {
				if (cfg.blocks[i].flags & ZEND_BB_LOOP_HEADER) {
					/* loop header */
					opline = op_array->opcodes + cfg.blocks[i].start;
					if (!(ZEND_OP_TRACE_INFO(opline, jit_extension->offset)->trace_flags & ZEND_JIT_TRACE_UNSUPPORTED)) {
						opline->handler = (const void*)zend_jit_loop_trace_counter_handler;
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
	}

	if (JIT_G(hot_func)) {
		ZEND_ASSERT(zend_jit_func_trace_counter_handler != NULL);
		opline = op_array->opcodes;
		if (!(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
			while (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT) {
				opline++;
			}
		}

		if (!ZEND_OP_TRACE_INFO(opline, jit_extension->offset)->trace_flags) {
			/* function entry */
			opline->handler = (const void*)zend_jit_func_trace_counter_handler;
			ZEND_OP_TRACE_INFO(opline, jit_extension->offset)->counter =
				&zend_jit_hot_counters[ZEND_JIT_COUNTER_NUM];
			ZEND_JIT_COUNTER_NUM = (ZEND_JIT_COUNTER_NUM + 1) % ZEND_HOT_COUNTERS_COUNT;
			ZEND_OP_TRACE_INFO(opline, jit_extension->offset)->trace_flags |=
				ZEND_JIT_TRACE_START_ENTER;
		}
	}

	zend_shared_alloc_register_xlat_entry(op_array->opcodes, jit_extension);

	return SUCCESS;
}

static void zend_jit_trace_init_caches(void)
{
	memset(ZEND_VOIDP(JIT_G(bad_root_cache_opline)), 0, sizeof(JIT_G(bad_root_cache_opline)));
	memset(JIT_G(bad_root_cache_count), 0, sizeof(JIT_G(bad_root_cache_count)));
	memset(JIT_G(bad_root_cache_stop), 0, sizeof(JIT_G(bad_root_cache_count)));
	JIT_G(bad_root_slot) = 0;

	if (JIT_G(exit_counters)) {
		memset(JIT_G(exit_counters), 0, JIT_G(max_exit_counters));
	}
}

static void zend_jit_trace_reset_caches(void)
{
	JIT_G(tracing) = 0;
}

static void zend_jit_trace_restart(void)
{
	ZEND_JIT_TRACE_NUM = 1;
	ZEND_JIT_COUNTER_NUM = 0;
	ZEND_JIT_EXIT_NUM = 0;
	ZEND_JIT_EXIT_COUNTERS = 0;

	zend_jit_trace_init_caches();
}
