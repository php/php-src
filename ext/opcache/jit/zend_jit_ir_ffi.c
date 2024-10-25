/*
 *  +----------------------------------------------------------------------+
 *  | Zend JIT                                                             |
 *  +----------------------------------------------------------------------+
 *  | Copyright (c) The PHP Group                                          |
 *  +----------------------------------------------------------------------+
 *  | This source file is subject to version 3.01 of the PHP license,      |
 *  | that is bundled with this package in the file LICENSE, and is        |
 *  | available through the world-wide-web at the following url:           |
 *  | https://www.php.net/license/3_01.txt                                 |
 *  | If you did not receive a copy of the PHP license and are unable to   |
 *  | obtain it through the world-wide-web, please send a note to          |
 *  | license@php.net so we can mail you a copy immediately.               |
 *  +----------------------------------------------------------------------+
 *  | Authors: Dmitry Stogov <dmitry@php.net>                              |
 *  +----------------------------------------------------------------------+
 */

static ir_ref jit_FFI_CDATA_PTR(zend_jit_ctx *jit, ir_ref obj_ref)
{
	return ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_ffi_cdata, ptr)));
}

static ir_ref jit_FFI_CDATA_TYPE(zend_jit_ctx *jit, ir_ref obj_ref)
{
	return ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_ffi_cdata, type)));
}

static int zend_jit_ffi_symbols_guard(zend_jit_ctx       *jit,
                                      const zend_op      *opline,
                                      zend_ssa           *ssa,
                                      int                 use,
                                      int                 def,
                                      zend_jit_addr       addr,
                                      HashTable          *ffi_symbols,
                                      zend_jit_ffi_info  *ffi_info);

static int zend_jit_ffi_guard(zend_jit_ctx       *jit,
                              const zend_op      *opline,
                              zend_ssa           *ssa,
                              int                 use,
                              int                 def,
                              ir_ref              ref,
                              zend_ffi_type      *ffi_type,
                              zend_jit_ffi_info  *ffi_info);

static int zend_jit_ffi_init_call_sym(zend_jit_ctx         *jit,
                                      const zend_op        *opline,
                                      const zend_op_array  *op_array,
                                      zend_ssa             *ssa,
                                      const zend_ssa_op    *ssa_op,
                                      uint32_t              op1_info,
                                      zend_jit_addr         op1_addr,
                                      zend_ffi_symbol      *sym,
                                      HashTable            *op1_ffi_symbols,
                                      zend_jit_ffi_info    *ffi_info,
                                      ir_ref               *ffi_func_ref)
{
	zend_ffi_type *type;

	ZEND_ASSERT(sym->kind == ZEND_FFI_SYM_FUNC);
	type = ZEND_FFI_TYPE(sym->type);
	ZEND_ASSERT(type->kind == ZEND_FFI_TYPE_FUNC);

	if (!zend_jit_ffi_symbols_guard(jit, opline, ssa, ssa_op->op1_use, -1, op1_addr, op1_ffi_symbols, ffi_info)) {
		return 0;
	}

	if (type->func.abi == ZEND_FFI_ABI_FASTCALL) {
		*ffi_func_ref = ir_CONST_FC_FUNC(sym->addr);
	} else {
		*ffi_func_ref = ir_CONST_FUNC(sym->addr);
	}
	return 1;
}

static int zend_jit_ffi_init_call_obj(zend_jit_ctx         *jit,
                                      const zend_op        *opline,
                                      const zend_op_array  *op_array,
                                      zend_ssa             *ssa,
                                      const zend_ssa_op    *ssa_op,
                                      uint32_t              op1_info,
                                      zend_jit_addr         op1_addr,
                                      uint32_t              op2_info,
                                      zend_jit_addr         op2_addr,
                                      zend_ffi_type        *op2_ffi_type,
                                      zend_jit_ffi_info    *ffi_info,
                                      ir_ref               *ffi_func_ref)
{
	ir_ref obj_ref = jit_Z_PTR(jit, op2_addr);
	zend_ffi_type *type;

	ZEND_ASSERT(op2_ffi_type->kind == ZEND_FFI_TYPE_POINTER);
	type = ZEND_FFI_TYPE(op2_ffi_type->pointer.type);
	ZEND_ASSERT(type->kind == ZEND_FFI_TYPE_FUNC);

	if (!zend_jit_ffi_guard(jit, opline, ssa, ssa_op->op2_use, -1, obj_ref, op2_ffi_type, ffi_info)) {
		return 0;
	}

	*ffi_func_ref = obj_ref;

	return 1;
}

static int zend_jit_ffi_send_val(zend_jit_ctx         *jit,
                                 const zend_op        *opline,
                                 const zend_op_array  *op_array,
                                 zend_ssa             *ssa,
                                 const zend_ssa_op    *ssa_op,
                                 uint32_t              op1_info,
                                 zend_jit_addr         op1_addr,
                                 zend_jit_addr         op1_def_addr,
                                 zend_ffi_type        *op1_ffi_type)
{
	zend_jit_trace_stack_frame *call = JIT_G(current_frame)->call;
	zend_jit_trace_stack *stack = call->stack;
	zend_ffi_type *type = (zend_ffi_type*)(void*)call->call_opline;
	ir_ref ref = IR_UNUSED;
	uint8_t arg_type = IS_UNDEF;
	uint8_t arg_flags = 0;

	if (TRACE_FRAME_FFI_FUNC(call)) {
		if (TRACE_FRAME_FFI_FUNC(call) == TRACE_FRAME_FFI_FUNC_ADDR
		 || TRACE_FRAME_FFI_FUNC(call) == TRACE_FRAME_FFI_FUNC_ALIGNOF
		 || TRACE_FRAME_FFI_FUNC(call) == TRACE_FRAME_FFI_FUNC_SIZEOF
		 || TRACE_FRAME_FFI_FUNC(call) == TRACE_FRAME_FFI_FUNC_TYPEOF
		 || TRACE_FRAME_FFI_FUNC(call) == TRACE_FRAME_FFI_FUNC_IS_NULL) {
			ZEND_ASSERT(opline->op2.num == 1);
			ZEND_ASSERT(op1_ffi_type);

			if (TRACE_FRAME_FFI_FUNC(call) == TRACE_FRAME_FFI_FUNC_IS_NULL) {
				ZEND_ASSERT(op1_ffi_type->kind == ZEND_FFI_TYPE_POINTER);
				arg_flags |= ZREG_FFI_PTR_LOAD;
			}

			if (opline->op1_type & (IS_VAR|IS_TMP_VAR)) {
				arg_flags |= ZREG_FFI_ZVAL_DTOR;
			}
			if (op1_info & MAY_BE_REF) {
				arg_flags |= ZREG_FFI_ZVAL_DEREF;
			}
			ref = jit_Z_PTR(jit, op1_addr);
			SET_STACK_TYPE(stack, 0, IS_OBJECT, 0);
			SET_STACK_REF_EX(stack, 0, ref, arg_flags);
		} else if (TRACE_FRAME_FFI_FUNC(call) == TRACE_FRAME_FFI_FUNC_STRING) {
			if (opline->op2.num == 1) {
				ZEND_ASSERT(op1_ffi_type);
				if (op1_ffi_type->kind == ZEND_FFI_TYPE_POINTER) {
					arg_flags |= ZREG_FFI_PTR_LOAD;
				}
				if (opline->op1_type & (IS_VAR|IS_TMP_VAR)) {
					arg_flags |= ZREG_FFI_ZVAL_DTOR;
				}
				if (op1_info & MAY_BE_REF) {
					arg_flags |= ZREG_FFI_ZVAL_DEREF;
				}
				ref = jit_Z_PTR(jit, op1_addr);
				SET_STACK_TYPE(stack, 0, IS_OBJECT, 0);
				SET_STACK_REF_EX(stack, 0, ref, arg_flags);
			} else {
				ZEND_ASSERT(opline->op2.num == 2);
				if (op1_info == MAY_BE_LONG) {
					ref = jit_Z_LVAL(jit, op1_addr);
				} else if (op1_info == MAY_BE_NULL) {
					ref = IR_NULL;
				} else {
					ZEND_ASSERT(0 && "NIY");
				}
				SET_STACK_TYPE(stack, 1, IS_LONG, 0);
				SET_STACK_REF_EX(stack, 1, ref, 0);
			}
		} else {
			ZEND_UNREACHABLE();
		}
		return 1;
	}
	ZEND_ASSERT(type->kind == ZEND_FFI_TYPE_FUNC);
	if (type->attr & ZEND_FFI_ATTR_VARIADIC) {
		ZEND_ASSERT(TRACE_FRAME_NUM_ARGS(call) >= zend_hash_num_elements(type->func.args));
	} else {
		ZEND_ASSERT(TRACE_FRAME_NUM_ARGS(call) == zend_hash_num_elements(type->func.args));
	}
	ZEND_ASSERT(opline->op2.num > 0 && opline->op2.num <= TRACE_FRAME_NUM_ARGS(call));

	if (op1_info & MAY_BE_REF) {
		op1_addr = jit_ZVAL_DEREF(jit, op1_addr);
		op1_info &= ~MAY_BE_REF;
		if (opline->op1_type & (IS_VAR|IS_TMP_VAR)) {
			// TODO: dtor ???
		}
	}

	if (opline->op2.num - 1 < zend_hash_num_elements(type->func.args)) {
		type = zend_hash_index_find_ptr(type->func.args, opline->op2.num - 1);
		type = ZEND_FFI_TYPE(type);
		zend_ffi_type_kind type_kind = type->kind;

		if (type_kind == ZEND_FFI_TYPE_ENUM) {
			type_kind = type->enumeration.kind;
		}
		switch (type_kind) {
			case ZEND_FFI_TYPE_FLOAT:
				if (op1_info == MAY_BE_LONG) {
					ref = ir_INT2F((jit_Z_LVAL(jit, op1_addr)));
				} else if (op1_info == MAY_BE_DOUBLE) {
					ref = ir_D2F(jit_Z_DVAL(jit, op1_addr));
				} else {
					ZEND_UNREACHABLE();
				}
				break;
			case ZEND_FFI_TYPE_DOUBLE:
				if (op1_info == MAY_BE_LONG) {
					ref = ir_INT2D((jit_Z_LVAL(jit, op1_addr)));
				} else if (op1_info == MAY_BE_DOUBLE) {
					ref = jit_Z_DVAL(jit, op1_addr);
				} else {
					ZEND_UNREACHABLE();
				}
				break;
			case ZEND_FFI_TYPE_UINT8:
				if (op1_info == MAY_BE_LONG) {
					ref = ir_TRUNC_U8(jit_Z_LVAL(jit, op1_addr));
				} else {
					ZEND_UNREACHABLE();
				}
				break;
			case ZEND_FFI_TYPE_SINT8:
				if (op1_info == MAY_BE_LONG) {
					ref = ir_TRUNC_I8(jit_Z_LVAL(jit, op1_addr));
				} else {
					ZEND_UNREACHABLE();
				}
				break;
			case ZEND_FFI_TYPE_UINT16:
				if (op1_info == MAY_BE_LONG) {
					ref = ir_TRUNC_U16(jit_Z_LVAL(jit, op1_addr));
				} else {
					ZEND_UNREACHABLE();
				}
				break;
			case ZEND_FFI_TYPE_SINT16:
				if (op1_info == MAY_BE_LONG) {
					ref = ir_TRUNC_I16(jit_Z_LVAL(jit, op1_addr));
				} else {
					ZEND_UNREACHABLE();
				}
				break;
#ifdef ZEND_ENABLE_ZVAL_LONG64
			case ZEND_FFI_TYPE_UINT32:
				if (op1_info == MAY_BE_LONG) {
					ref = ir_TRUNC_U32(jit_Z_LVAL(jit, op1_addr));
				} else {
					ZEND_UNREACHABLE();
				}
				break;
			case ZEND_FFI_TYPE_SINT32:
				if (op1_info == MAY_BE_LONG) {
					ref = ir_TRUNC_I32(jit_Z_LVAL(jit, op1_addr));
				} else {
					ZEND_UNREACHABLE();
				}
				break;
			case ZEND_FFI_TYPE_UINT64:
			case ZEND_FFI_TYPE_SINT64:
				if (op1_info == MAY_BE_LONG) {
					ref = jit_Z_LVAL(jit, op1_addr);
				} else {
					ZEND_UNREACHABLE();
				}
				break;
#else
			case ZEND_FFI_TYPE_UINT32:
			case ZEND_FFI_TYPE_SINT32:
				if (op1_info == MAY_BE_LONG) {
					ref = jit_Z_LVAL(jit, op1_addr);
				} else {
					ZEND_UNREACHABLE();
				}
				break;
#endif
			case ZEND_FFI_TYPE_BOOL:
				if (op1_info == MAY_BE_NULL || op1_info == MAY_BE_FALSE) {
					ref = IR_FALSE;
				} else if (op1_info == MAY_BE_TRUE) {
					ref = IR_TRUE;
				} else if (op1_info == (MAY_BE_FALSE|MAY_BE_TRUE)) {
					ref = ir_SUB_U8(jit_Z_TYPE(jit, op1_addr), ir_CONST_U8(IS_FALSE));
				} else if (op1_info == MAY_BE_LONG) {
					ref = ir_NE(jit_Z_LVAL(jit, op1_addr), ir_CONST_LONG(0));
				} else if (op1_info == MAY_BE_DOUBLE) {
					ref = ir_NE(jit_Z_DVAL(jit, op1_addr), ir_CONST_DOUBLE(0.0));
				} else {
					ZEND_UNREACHABLE();
				}
				break;
			case ZEND_FFI_TYPE_CHAR:
				if (op1_info == MAY_BE_LONG) {
					ref = ir_TRUNC_C(jit_Z_LVAL(jit, op1_addr));
				} else {
					ZEND_UNREACHABLE();
				}
				break;
			case ZEND_FFI_TYPE_POINTER:
				if ((op1_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_STRING
				 && ZEND_FFI_TYPE(type->pointer.type)->kind == ZEND_FFI_TYPE_CHAR) {
					arg_type = IS_STRING;
					ref = jit_Z_PTR(jit, op1_addr);
					if (opline->op1_type & (IS_VAR|IS_TMP_VAR)) {
						arg_flags |= ZREG_FFI_ZVAL_DTOR;
					}
				} else if ((op1_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_NULL) {
					arg_type = IS_NULL;
					ref = IR_NULL;
				} else if (op1_ffi_type
				 && op1_ffi_type->kind == ZEND_FFI_TYPE_POINTER
				 && (ZEND_FFI_TYPE(type->pointer.type) == ZEND_FFI_TYPE(op1_ffi_type->pointer.type)
				  || zend_ffi_api->is_compatible_type(ZEND_FFI_TYPE(op1_ffi_type->pointer.type), ZEND_FFI_TYPE(type->pointer.type)))) {
					// TODO: guards ???
					arg_type = IS_OBJECT;
					ref = jit_Z_PTR(jit, op1_addr);
					arg_flags |= ZREG_FFI_PTR_LOAD;
					if (opline->op1_type & (IS_VAR|IS_TMP_VAR)) {
						arg_flags |= ZREG_FFI_ZVAL_DTOR;
					}
				} else if (op1_ffi_type
				 && op1_ffi_type->kind == ZEND_FFI_TYPE_ARRAY
				 && (ZEND_FFI_TYPE(type->pointer.type) == ZEND_FFI_TYPE(op1_ffi_type->array.type)
				  || zend_ffi_api->is_compatible_type(ZEND_FFI_TYPE(op1_ffi_type->pointer.type), ZEND_FFI_TYPE(type->pointer.type)))) {
					// TODO: guards ???
					arg_type = IS_OBJECT;
					ref = jit_Z_PTR(jit, op1_addr);
					if (opline->op1_type & (IS_VAR|IS_TMP_VAR)) {
						arg_flags |= ZREG_FFI_ZVAL_DTOR;
					}
				} else {
					ZEND_UNREACHABLE();
				}
				break;
			default:
				ZEND_UNREACHABLE();
		}
	} else {
		if (op1_info == MAY_BE_NULL) {
			ref = IR_NULL;
		} else if (op1_info == MAY_BE_FALSE) {
			ref = IR_FALSE;
		} else if (op1_info == MAY_BE_TRUE) {
			ref = IR_TRUE;
		} else if (op1_info == (MAY_BE_FALSE|MAY_BE_TRUE)) {
			ref = ir_SUB_U8(jit_Z_TYPE(jit, op1_addr), ir_CONST_U8(IS_FALSE));
		} else if (op1_info == MAY_BE_LONG) {
			ref = jit_Z_LVAL(jit, op1_addr);
		} else if (op1_info == MAY_BE_DOUBLE) {
			ref = jit_Z_DVAL(jit, op1_addr);
		} else if ((op1_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_STRING) {
			arg_type = IS_STRING;
			ref = jit_Z_PTR(jit, op1_addr);
			if (opline->op1_type & (IS_VAR|IS_TMP_VAR)) {
				arg_flags |= ZREG_FFI_ZVAL_DTOR;
			}
		} else {
			ZEND_UNREACHABLE();
		}
	}

	SET_STACK_TYPE(stack, opline->op2.num - 1, arg_type, 0);
	SET_STACK_REF_EX(stack, opline->op2.num - 1, ref, arg_flags);

	return 1;
}

static ir_ref zend_jit_gc_deref(zend_jit_ctx *jit, ir_ref ref)
{
	ir_ref if_ref, ref2;

	if_ref = ir_IF(ir_EQ(
		ir_AND_U32(
			ir_LOAD_U32(ir_ADD_OFFSET(ref, offsetof(zend_refcounted_h, u.type_info))),
			ir_CONST_U32(GC_TYPE_MASK)),
		ir_CONST_U32(IS_REFERENCE)));
	ir_IF_TRUE(if_ref);
	ref2 = jit_Z_PTR_ref(jit, ir_ADD_OFFSET(ref, offsetof(zend_reference, val)));
	ir_MERGE_WITH_EMPTY_FALSE(if_ref);
	return ir_PHI_2(IR_ADDR, ref2, ref);
}

static int zend_jit_ffi_do_call(zend_jit_ctx         *jit,
                                const zend_op        *opline,
                                const zend_op_array  *op_array,
                                zend_ssa             *ssa,
                                const zend_ssa_op    *ssa_op,
                                zend_jit_addr         res_addr)
{
	zend_jit_trace_stack_frame *call = JIT_G(current_frame)->call;
	zend_jit_trace_stack *stack = call->stack;
	zend_ffi_type *type = (zend_ffi_type*)(void*)call->call_opline;
	ir_ref func_ref = (intptr_t)(void*)call->ce;
	uint32_t i, num_args = TRACE_FRAME_NUM_ARGS(call);;
	ir_type ret_type = IR_VOID;
	ir_ref ref = IR_UNUSED;
	zend_ffi_type_kind type_kind;

	if (TRACE_FRAME_FFI_FUNC(call)) {
		if (TRACE_FRAME_FFI_FUNC(call) == TRACE_FRAME_FFI_FUNC_ADDR) {
			ref = STACK_REF(stack, 0);
			if (STACK_FLAGS(stack, 0) & ZREG_FFI_ZVAL_DEREF) {
				// TODO: try to remove this dereference ???
				ref = zend_jit_gc_deref(jit, ref);
			}
			if (STACK_FLAGS(stack, 0) & ZREG_FFI_ZVAL_DTOR) {
				ref = ir_CALL_1(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_zval_ffi_addr_var), ref);
			} else {
				ref = ir_CALL_1(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_zval_ffi_addr), ref);
			}
			jit_set_Z_PTR(jit, res_addr, ref);
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_OBJECT_EX);
		} else if (TRACE_FRAME_FFI_FUNC(call) == TRACE_FRAME_FFI_FUNC_ALIGNOF) {
			ref = STACK_REF(stack, 0);
			if (STACK_FLAGS(stack, 0) & ZREG_FFI_ZVAL_DEREF) {
				// TODO: try to remove this dereference ???
				ref = zend_jit_gc_deref(jit, ref);
			}
			ref = jit_FFI_CDATA_TYPE(jit, ref);
			// TODO: type flags ???
			ref = ir_LOAD_U32(ir_ADD_OFFSET(ref, offsetof(zend_ffi_type, align)));
			if (sizeof(void*) == 8) {
				ref = ir_ZEXT_L(ref);
			}
			jit_set_Z_LVAL(jit, res_addr, ref);
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
		} else if (TRACE_FRAME_FFI_FUNC(call) == TRACE_FRAME_FFI_FUNC_SIZEOF) {
			ref = STACK_REF(stack, 0);
			if (STACK_FLAGS(stack, 0) & ZREG_FFI_ZVAL_DEREF) {
				// TODO: try to remove this dereference ???
				ref = zend_jit_gc_deref(jit, ref);
			}
			ref = jit_FFI_CDATA_TYPE(jit, ref);
			// TODO: type flags ???
			ref = ir_LOAD_U32(ir_ADD_OFFSET(ref, offsetof(zend_ffi_type, size)));
			if (sizeof(void*) == 8) {
				ref = ir_ZEXT_L(ref);
			}
			jit_set_Z_LVAL(jit, res_addr, ref);
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
		} else if (TRACE_FRAME_FFI_FUNC(call) == TRACE_FRAME_FFI_FUNC_TYPEOF) {
			ref = STACK_REF(stack, 0);
			if (STACK_FLAGS(stack, 0) & ZREG_FFI_ZVAL_DEREF) {
				// TODO: try to remove this dereference ???
				ref = zend_jit_gc_deref(jit, ref);
			}
			ref = jit_FFI_CDATA_TYPE(jit, ref);
			// TODO: type flags ???
			ref = ir_CALL_1(IR_ADDR, ir_CONST_FUNC(zend_ffi_api->ctype_create), ref);
			jit_set_Z_PTR(jit, res_addr, ref);
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_OBJECT_EX);
		} else if (TRACE_FRAME_FFI_FUNC(call) == TRACE_FRAME_FFI_FUNC_IS_NULL) {
			ref = STACK_REF(stack, 0);
			if (STACK_FLAGS(stack, 0) & ZREG_FFI_ZVAL_DEREF) {
				// TODO: try to remove this dereference ???
				ref = zend_jit_gc_deref(jit, ref);
			}
			ref = jit_FFI_CDATA_TYPE(jit, ref);
			if (STACK_FLAGS(stack, 0) & ZREG_FFI_PTR_LOAD) {
				ref = ir_LOAD_A(ref);
			}
			ref = ir_ADD_U32(ir_ZEXT_U32(ir_EQ(ref, IR_NULL)), ir_CONST_U32(IS_FALSE));
			jit_set_Z_TYPE_INFO_ex(jit, res_addr, ref);
		} else if (TRACE_FRAME_FFI_FUNC(call) == TRACE_FRAME_FFI_FUNC_STRING) {
			ZEND_ASSERT(num_args > 0 && STACK_TYPE(stack, 0) == IS_OBJECT);
			ref = STACK_REF(stack, 0);
			if (STACK_FLAGS(stack, 0) & ZREG_FFI_ZVAL_DEREF) {
				// TODO: try to remove this dereference ???
				ref = zend_jit_gc_deref(jit, ref);
			}
			ref = jit_FFI_CDATA_PTR(jit, ref);
			if (STACK_FLAGS(stack, 0) & ZREG_FFI_PTR_LOAD) {
				ref = ir_LOAD_A(ref);
			}
			if (num_args == 1 || STACK_REF(stack, 1) == IR_NULL) {
				ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_zval_string),
					jit_ZVAL_ADDR(jit, res_addr), ref);
			} else {
				ZEND_ASSERT(num_args == 2);
				ZEND_ASSERT(STACK_TYPE(stack, 1) == IS_LONG);
				ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_zval_stringl),
					jit_ZVAL_ADDR(jit, res_addr), ref, STACK_REF(stack, 1));
			}
		} else {
			ZEND_UNREACHABLE();
		}
		goto cleanup;
	}

	ZEND_ASSERT(type->kind == ZEND_FFI_TYPE_FUNC);

	type_kind = ZEND_FFI_TYPE(type->func.ret_type)->kind;
	if (type_kind == ZEND_FFI_TYPE_ENUM) {
		type_kind = ZEND_FFI_TYPE(type->func.ret_type)->enumeration.kind;
	}
	switch (type_kind) {
		case ZEND_FFI_TYPE_VOID:
			ret_type = IR_VOID;
			break;
		case ZEND_FFI_TYPE_FLOAT:
			ret_type = IR_FLOAT;
			break;
		case ZEND_FFI_TYPE_DOUBLE:
			ret_type = IR_DOUBLE;
			break;
		case ZEND_FFI_TYPE_UINT8:
			ret_type = IR_U8;
			break;
		case ZEND_FFI_TYPE_SINT8:
			ret_type = IR_I8;
			break;
		case ZEND_FFI_TYPE_UINT16:
			ret_type = IR_U16;
			break;
		case ZEND_FFI_TYPE_SINT16:
			ret_type = IR_I16;
			break;
		case ZEND_FFI_TYPE_UINT32:
			ret_type = IR_U32;
			break;
		case ZEND_FFI_TYPE_SINT32:
			ret_type = IR_I32;
			break;
#ifdef ZEND_ENABLE_ZVAL_LONG64
		case ZEND_FFI_TYPE_UINT64:
			ret_type = IR_U64;
			break;
		case ZEND_FFI_TYPE_SINT64:
			ret_type = IR_I64;
			break;
#endif
		case ZEND_FFI_TYPE_BOOL:
			ret_type = IR_BOOL;
			break;
		case ZEND_FFI_TYPE_CHAR:
			ret_type = IR_CHAR;
			break;
		case ZEND_FFI_TYPE_POINTER:
			ret_type = IR_ADDR;
			break;
		default:
			ZEND_UNREACHABLE();
	}

	if (!IR_IS_CONST_REF(func_ref)) {
		func_ref = ir_LOAD_A(jit_FFI_CDATA_PTR(jit, func_ref));
		if (type->func.abi == ZEND_FFI_ABI_FASTCALL) {
			func_ref = ir_CAST_FC_FUNC(func_ref);
		}
	}

	if (num_args) {
		ir_ref *args = alloca(sizeof(ir_ref) * num_args);

		for (i = 0; i < num_args; i++) {
			uint8_t type = STACK_TYPE(stack, i);
			ir_ref ref = STACK_REF(stack, i);

			if (type == IS_STRING) {
				ref = ir_ADD_OFFSET(ref, offsetof(zend_string, val));
			} else if (type == IS_OBJECT) {
				ref = jit_FFI_CDATA_PTR(jit, ref);
				if (STACK_FLAGS(stack, i) & ZREG_FFI_PTR_LOAD) {
					ref = ir_LOAD_A(ref);
				}
			}
			args[i] = ref;
		}
		ref = ir_CALL_N(ret_type, func_ref, num_args, args);
	} else {
		ZEND_ASSERT(!type->func.args);
		ref = ir_CALL(ret_type, func_ref);
	}

	if (RETURN_VALUE_USED(opline)) {
		zend_ffi_type *ret_type = ZEND_FFI_TYPE(type->func.ret_type);
		uint32_t res_type = IS_UNDEF;

		type_kind = ret_type->kind;
		if (type_kind == ZEND_FFI_TYPE_ENUM) {
			type_kind = ret_type->enumeration.kind;
		}
		switch (type_kind) {
			case ZEND_FFI_TYPE_VOID:
				res_type = IS_NULL;
				break;
			case ZEND_FFI_TYPE_FLOAT:
				jit_set_Z_DVAL(jit, res_addr, ir_F2D(ref));
				res_type = IS_DOUBLE;
				break;
			case ZEND_FFI_TYPE_DOUBLE:
				jit_set_Z_DVAL(jit, res_addr, ref);
				res_type = IS_DOUBLE;
				break;
			case ZEND_FFI_TYPE_UINT8:
				jit_set_Z_LVAL(jit, res_addr, ir_ZEXT_L(ref));
				res_type = IS_LONG;
				break;
			case ZEND_FFI_TYPE_SINT8:
				jit_set_Z_LVAL(jit, res_addr, ir_SEXT_L(ref));
				res_type = IS_LONG;
				break;
			case ZEND_FFI_TYPE_UINT16:
				jit_set_Z_LVAL(jit, res_addr, ir_ZEXT_L(ref));
				res_type = IS_LONG;
				break;
			case ZEND_FFI_TYPE_SINT16:
				jit_set_Z_LVAL(jit, res_addr, ir_SEXT_L(ref));
				res_type = IS_LONG;
				break;
#ifdef ZEND_ENABLE_ZVAL_LONG64
			case ZEND_FFI_TYPE_UINT32:
				jit_set_Z_LVAL(jit, res_addr, ir_ZEXT_L(ref));
				res_type = IS_LONG;
				break;
			case ZEND_FFI_TYPE_SINT32:
				jit_set_Z_LVAL(jit, res_addr, ir_SEXT_L(ref));
				res_type = IS_LONG;
				break;
			case ZEND_FFI_TYPE_UINT64:
				jit_set_Z_LVAL(jit, res_addr, ref);
				res_type = IS_LONG;
				break;
			case ZEND_FFI_TYPE_SINT64:
				jit_set_Z_LVAL(jit, res_addr, ref);
				res_type = IS_LONG;
				break;
#else
			case ZEND_FFI_TYPE_UINT32:
				jit_set_Z_LVAL(jit, res_addr, ref);
				res_type = IS_LONG;
				break;
			case ZEND_FFI_TYPE_SINT32:
				jit_set_Z_LVAL(jit, res_addr, ref);
				res_type = IS_LONG;
				break;
#endif
			case ZEND_FFI_TYPE_BOOL:
				jit_set_Z_TYPE_INFO(jit, res_addr,
					ir_ADD_U32(ir_ZEXT_U32(ref), ir_CONST_U32(IS_FALSE)));
				goto cleanup;
			case ZEND_FFI_TYPE_CHAR:
				jit_set_Z_PTR(jit, res_addr, ir_LOAD_A(
					ir_ADD_A(ir_CONST_ADDR(zend_one_char_string),
						ir_MUL_L(ir_ZEXT_L(ref), ir_CONST_LONG(sizeof(void*))))));
				res_type = IS_STRING;
				break;
			case ZEND_FFI_TYPE_POINTER:
				 if ((ret_type->attr & ZEND_FFI_ATTR_CONST)
				  && ZEND_FFI_TYPE(ret_type->pointer.type)->kind == ZEND_FFI_TYPE_CHAR) {
					ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_zval_string), jit_ZVAL_ADDR(jit, res_addr), ref);
					goto cleanup;
				 } else {
					ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_zval_ffi_ptr),
						jit_ZVAL_ADDR(jit, res_addr), ir_CONST_ADDR(ret_type), ref);
					goto cleanup;
				 }
				 break;
			default:
				ZEND_UNREACHABLE();
		}

		if (Z_MODE(res_addr) != IS_REG) {
			jit_set_Z_TYPE_INFO(jit, res_addr, res_type);
		}
	}

cleanup:
	if (num_args) {
		zend_jit_trace_stack *stack = call->stack;

		for (i = 0; i < num_args; i++) {
			if (STACK_FLAGS(stack, i) & ZREG_FFI_ZVAL_DTOR) {
				uint8_t type =  STACK_TYPE(stack, i);
				ir_ref ref = STACK_REF(stack, i);

				if (STACK_FLAGS(stack, i) & ZREG_FFI_ZVAL_DEREF) {
					ir_ref if_not_zero = ir_IF(jit_GC_DELREF(jit, ref));
					ir_IF_FALSE(if_not_zero);
					jit_ZVAL_DTOR(jit, ref, MAY_BE_REF|MAY_BE_ANY, opline);
					ir_MERGE_WITH_EMPTY_TRUE(if_not_zero); /* don't add to GC roots */
				} else if (type == IS_STRING) {
					ir_ref if_interned = ir_IF(ir_AND_U32(
						ir_LOAD_U32(ir_ADD_OFFSET(ref, offsetof(zend_refcounted, gc.u.type_info))),
						ir_CONST_U32(IS_STR_INTERNED)));
					ir_IF_FALSE(if_interned);
					ir_ref if_not_zero = ir_IF(jit_GC_DELREF(jit, ref));
					ir_IF_FALSE(if_not_zero);
					jit_ZVAL_DTOR(jit, ref, MAY_BE_STRING, opline);
					ir_MERGE_WITH_EMPTY_TRUE(if_not_zero);
					ir_MERGE_WITH_EMPTY_TRUE(if_interned);
				} else if (type == IS_OBJECT) {
					ir_ref if_not_zero = ir_IF(jit_GC_DELREF(jit, ref));
					ir_IF_FALSE(if_not_zero);
					jit_ZVAL_DTOR(jit, ref, MAY_BE_OBJECT, opline);
					ir_MERGE_WITH_EMPTY_TRUE(if_not_zero); /* don't add to GC roots */
				} else {
					ZEND_ASSERT(0);
				}
			}
		}
	}

	if (!TRACE_FRAME_FFI_FUNC(call)) {
		func_ref = (intptr_t)(void*)call->ce;
		if (!IR_IS_CONST_REF(func_ref)) {
			ir_ref if_not_zero = ir_IF(jit_GC_DELREF(jit, func_ref));
			ir_IF_FALSE(if_not_zero);
			jit_ZVAL_DTOR(jit, func_ref, MAY_BE_OBJECT, opline);
			ir_MERGE_WITH_EMPTY_TRUE(if_not_zero); /* don't add to GC roots */
		}
	}

	return 1;
}

static int zend_jit_ffi_type_guard(zend_jit_ctx *jit, const zend_op *opline, ir_ref obj_ref, zend_ffi_type *ffi_type)
{
	int32_t exit_point = zend_jit_trace_get_exit_point(opline, 0);
	const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

	if (!exit_addr) {
		return 0;
	}

	ir_GUARD(ir_EQ(ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_ffi_cdata, type))), ir_CONST_ADDR(ffi_type)),
		ir_CONST_ADDR(exit_addr));

	return 1;
}

static int zend_jit_ffi_abc(zend_jit_ctx       *jit,
                            const zend_op      *opline,
                            zend_ffi_type      *ffi_type,
                            uint32_t            op2_info,
                            zend_jit_addr       op2_addr,
                            zend_ssa_range     *op2_range)
{
	int32_t exit_point;
	const void *exit_addr;

	ZEND_ASSERT(op2_info == MAY_BE_LONG);
	if (ffi_type->kind == ZEND_FFI_TYPE_ARRAY
	 && !(ffi_type->attr & (ZEND_FFI_ATTR_VLA|ZEND_FFI_ATTR_INCOMPLETE_ARRAY))) {
		if (Z_MODE(op2_addr) == IS_CONST_ZVAL) {
			zval *zv = Z_ZV(op2_addr);
			ZEND_ASSERT(Z_TYPE_P(zv) == IS_LONG);
			if (Z_LVAL_P(zv) < 0 || Z_LVAL_P(zv) >= ffi_type->array.length) {
				/* Always out of range */
				exit_point = zend_jit_trace_get_exit_point(opline, 0);
				exit_addr = zend_jit_trace_get_exit_addr(exit_point);
				if (!exit_addr) {
					return 0;
				}
				jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
			}
		} else if (!op2_range || op2_range->min < 0 || op2_range->max >= ffi_type->array.length) {
			if (op2_range && (op2_range->max < 0 || op2_range->min >= ffi_type->array.length)) {
				/* Always out of range */
				exit_point = zend_jit_trace_get_exit_point(opline, 0);
				exit_addr = zend_jit_trace_get_exit_addr(exit_point);
				if (!exit_addr) {
					return 0;
				}
				jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
			} else {
				/* Array Bounds Check */
				exit_point = zend_jit_trace_get_exit_point(opline, 0);
				exit_addr = zend_jit_trace_get_exit_addr(exit_point);
				if (!exit_addr) {
					return 0;
				}
				ir_GUARD(ir_LT(jit_Z_LVAL(jit, op2_addr), ir_CONST_LONG(ffi_type->array.length)),
					ir_CONST_ADDR(exit_addr));
			}
		}
	}

	return 1;
}

static int zend_jit_ffi_read(zend_jit_ctx       *jit,
                             zend_ffi_type      *ffi_type,
                             ir_ref              ptr,
                             zend_jit_addr       res_addr)
{
	uint32_t res_type;
	zend_ffi_type_kind type_kind = ffi_type->kind;

	if (type_kind == ZEND_FFI_TYPE_ENUM) {
		type_kind = ffi_type->enumeration.kind;
	}
	switch (type_kind) {
		case ZEND_FFI_TYPE_FLOAT:
			jit_set_Z_DVAL(jit, res_addr, ir_F2D(ir_LOAD_F(ptr)));
			res_type = IS_DOUBLE;
			break;
		case ZEND_FFI_TYPE_DOUBLE:
			jit_set_Z_DVAL(jit, res_addr, ir_LOAD_D(ptr));
			res_type = IS_DOUBLE;
			break;
		case ZEND_FFI_TYPE_UINT8:
			jit_set_Z_LVAL(jit, res_addr, ir_ZEXT_L(ir_LOAD_U8(ptr)));
			res_type = IS_LONG;
			break;
		case ZEND_FFI_TYPE_SINT8:
			jit_set_Z_LVAL(jit, res_addr, ir_SEXT_L(ir_LOAD_I8(ptr)));
			res_type = IS_LONG;
			break;
		case ZEND_FFI_TYPE_UINT16:
			jit_set_Z_LVAL(jit, res_addr, ir_ZEXT_L(ir_LOAD_U16(ptr)));
			res_type = IS_LONG;
			break;
		case ZEND_FFI_TYPE_SINT16:
			jit_set_Z_LVAL(jit, res_addr, ir_SEXT_L(ir_LOAD_I16(ptr)));
			res_type = IS_LONG;
			break;
#ifdef ZEND_ENABLE_ZVAL_LONG64
		case ZEND_FFI_TYPE_UINT32:
			jit_set_Z_LVAL(jit, res_addr, ir_ZEXT_L(ir_LOAD_U32(ptr)));
			res_type = IS_LONG;
			break;
		case ZEND_FFI_TYPE_SINT32:
			jit_set_Z_LVAL(jit, res_addr, ir_SEXT_L(ir_LOAD_I32(ptr)));
			res_type = IS_LONG;
			break;
		case ZEND_FFI_TYPE_UINT64:
			jit_set_Z_LVAL(jit, res_addr, ir_LOAD_U64(ptr));
			res_type = IS_LONG;
			break;
		case ZEND_FFI_TYPE_SINT64:
			jit_set_Z_LVAL(jit, res_addr, ir_LOAD_I64(ptr));
			res_type = IS_LONG;
			break;
#else
		case ZEND_FFI_TYPE_UINT32:
			jit_set_Z_LVAL(jit, res_addr, ir_LOAD_U32(ptr));
			res_type = IS_LONG;
			break;
		case ZEND_FFI_TYPE_SINT32:
			jit_set_Z_LVAL(jit, res_addr, ir_LOAD_I32(ptr));
			res_type = IS_LONG;
			break;
#endif
		case ZEND_FFI_TYPE_BOOL:
			jit_set_Z_TYPE_INFO(jit, res_addr,
				ir_ADD_U32(ir_ZEXT_U32(ir_LOAD_U8(ptr)), ir_CONST_U32(IS_FALSE)));
			return 1;
		case ZEND_FFI_TYPE_CHAR:
			jit_set_Z_PTR(jit, res_addr, ir_LOAD_A(
				ir_ADD_A(ir_CONST_ADDR(zend_one_char_string),
					ir_MUL_L(ir_ZEXT_L(ir_LOAD_U8(ptr)), ir_CONST_LONG(sizeof(void*))))));
			res_type = IS_STRING;
			break;
		case ZEND_FFI_TYPE_ARRAY:
		case ZEND_FFI_TYPE_STRUCT:
			ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_zval_ffi_obj),
				jit_ZVAL_ADDR(jit, res_addr), ir_CONST_ADDR(ffi_type), ptr);
			return 1;
		case ZEND_FFI_TYPE_POINTER:
			 if ((ffi_type->attr & ZEND_FFI_ATTR_CONST)
			  && ZEND_FFI_TYPE(ffi_type->pointer.type)->kind == ZEND_FFI_TYPE_CHAR) {
				ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_zval_string), jit_ZVAL_ADDR(jit, res_addr), ptr);
				return 1;
			 } else {
				ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_zval_ffi_ptr),
					jit_ZVAL_ADDR(jit, res_addr), ir_CONST_ADDR(ffi_type), ir_LOAD_A(ptr));
				return 1;
			 }
			 break;
			break;
		default:
			ZEND_UNREACHABLE();
	}

	if (Z_MODE(res_addr) != IS_REG) {
		jit_set_Z_TYPE_INFO(jit, res_addr, res_type);
	}

	return 1;
}

static int zend_jit_ffi_guard(zend_jit_ctx       *jit,
                              const zend_op      *opline,
                              zend_ssa           *ssa,
                              int                 use,
                              int                 def,
                              ir_ref              ref,
                              zend_ffi_type      *ffi_type,
                              zend_jit_ffi_info  *ffi_info)
{
	if (ssa->var_info
	 && use >= 0
	 && ssa->var_info[use].ce != zend_ffi_api->cdata_ce) {
		if (!zend_jit_class_guard(jit, opline, ref, zend_ffi_api->cdata_ce)) {
			return 0;
		}
		ssa->var_info[use].type |= MAY_BE_CLASS_GUARD;
		ssa->var_info[use].ce = zend_ffi_api->cdata_ce;
		ssa->var_info[use].is_instanceof = 0;
		if (def >= 0) {
			ssa->var_info[def].type |= MAY_BE_CLASS_GUARD;
			ssa->var_info[def].ce = zend_ffi_api->cdata_ce;
			ssa->var_info[def].is_instanceof = 0;
		}
	}

	if (ffi_info
	 && use >= 0
	 && (ffi_info[use].type != ffi_type
	  || (ffi_info[use].info & FFI_TYPE_GUARD))) {
		if (!zend_jit_ffi_type_guard(jit, opline, ref, ffi_type)) {
			return 0;
		}
		ffi_info[use].info &= ~FFI_TYPE_GUARD;
		ffi_info[use].type = ffi_type;
		if (def >= 0) {
			ffi_info[def].info &= ~FFI_TYPE_GUARD;
			ffi_info[def].type = ffi_type;
		}
	}

	return 1;
}

static int zend_jit_ffi_symbols_guard(zend_jit_ctx       *jit,
                                      const zend_op      *opline,
                                      zend_ssa           *ssa,
                                      int                 use,
                                      int                 def,
                                      zend_jit_addr       addr,
                                      HashTable          *ffi_symbols,
                                      zend_jit_ffi_info  *ffi_info)
{
	ir_ref ref = IR_UNUSED;

	if (ssa->var_info
	 && use >= 0
	 && ssa->var_info[use].ce != zend_ffi_api->scope_ce) {
		ref = jit_Z_PTR(jit, addr);
		if (!zend_jit_class_guard(jit, opline, ref, zend_ffi_api->scope_ce)) {
			return 0;
		}
		ssa->var_info[use].type |= MAY_BE_CLASS_GUARD;
		ssa->var_info[use].ce = zend_ffi_api->scope_ce;
		ssa->var_info[use].is_instanceof = 0;
		if (def >= 0) {
			ssa->var_info[def].type |= MAY_BE_CLASS_GUARD;
			ssa->var_info[def].ce = zend_ffi_api->scope_ce;
			ssa->var_info[def].is_instanceof = 0;
		}
	}

	if (ffi_info
	 && use >= 0
	 && (ffi_info[use].symbols != ffi_symbols
	  || (ffi_info[use].info & FFI_SYMBOLS_GUARD))) {
		if (!ref) {
			ref = jit_Z_PTR(jit, addr);
		}

		int32_t exit_point = zend_jit_trace_get_exit_point(opline, 0);
		const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

		if (!exit_addr) {
			return 0;
		}

		ir_GUARD(ir_EQ(ir_LOAD_A(ir_ADD_OFFSET(ref, offsetof(zend_ffi, symbols))), ir_CONST_ADDR(ffi_symbols)),
			ir_CONST_ADDR(exit_addr));

		ffi_info[use].info &= ~FFI_SYMBOLS_GUARD;
		ffi_info[use].symbols = ffi_symbols;
		if (def >= 0) {
			ffi_info[def].info &= ~FFI_SYMBOLS_GUARD;
			ffi_info[def].symbols = ffi_symbols;
		}
	}

	return 1;
}

static int zend_jit_ffi_fetch_dim(zend_jit_ctx       *jit,
                                  const zend_op      *opline,
                                  zend_ssa           *ssa,
                                  const zend_ssa_op  *ssa_op,
                                  uint32_t            op1_info,
                                  zend_jit_addr       op1_addr,
                                  bool                op1_indirect,
                                  bool                op1_avoid_refcounting,
                                  uint32_t            op2_info,
                                  zend_jit_addr       op2_addr,
                                  zend_ssa_range     *op2_range,
                                  uint32_t            res_info,
                                  zend_jit_addr       res_addr,
                                  zend_ffi_type      *op1_ffi_type,
                                  zend_jit_ffi_info  *ffi_info)
{
	zend_ffi_type *el_type = ZEND_FFI_TYPE(op1_ffi_type->array.type);
	ir_ref obj_ref = jit_Z_PTR(jit, op1_addr);

	if (!zend_jit_ffi_guard(jit, opline, ssa, ssa_op->op1_use, -1, obj_ref, op1_ffi_type, ffi_info)) {
		return 0;
	}

	if (!zend_jit_ffi_abc(jit, opline, op1_ffi_type, op2_info, op2_addr, op2_range)) {
		return 0;
	}

	ir_ref cdata_ref = jit_FFI_CDATA_PTR(jit, obj_ref);
//	ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_ffi_cdata, ptr)));

	if (op1_ffi_type->kind == ZEND_FFI_TYPE_POINTER) {
		cdata_ref = ir_LOAD_A(cdata_ref);
	}

	ir_ref ptr = ir_ADD_A(cdata_ref, ir_MUL_L(jit_Z_LVAL(jit, op2_addr), ir_CONST_LONG(el_type->size)));

	if (opline->opcode == ZEND_FETCH_DIM_W || opline->opcode == ZEND_FETCH_DIM_RW) {
		jit_set_Z_PTR(jit, res_addr,
			ir_CALL_2(IR_ADDR, ir_CONST_FUNC(zend_ffi_api->cdata_create),
				ptr, ir_CONST_ADDR(el_type)));
		jit_set_Z_TYPE_INFO(jit, res_addr, IS_OBJECT_EX);
	} else {
		if (!zend_jit_ffi_read(jit, el_type, ptr, res_addr)) {
			return 0;
		}
	}

	if (res_info & MAY_BE_GUARD) {
		// TODO: ???
		ssa->var_info[ssa_op->result_def].type &= ~MAY_BE_GUARD;
	}

	if (opline->opcode != ZEND_FETCH_LIST_R && !op1_avoid_refcounting && !op1_indirect) {
		if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
			jit_FREE_OP(jit,  opline->op1_type, opline->op1, op1_info, opline);
		}
	}

	return 1;
}

static int zend_jit_ffi_write(zend_jit_ctx  *jit,
                              zend_ffi_type *ffi_type,
                              ir_ref         ptr,
                              uint32_t       val_info,
                              zend_jit_addr  val_addr,
                              zend_ffi_type *val_ffi_type,
                              zend_jit_addr  res_addr)
{
	ir_ref ref = IR_UNUSED;
	zend_ffi_type_kind type_kind = ffi_type->kind;

	if (type_kind == ZEND_FFI_TYPE_ENUM) {
		type_kind = ffi_type->enumeration.kind;
	}
	switch (type_kind) {
		case ZEND_FFI_TYPE_FLOAT:
			if (val_info == MAY_BE_LONG) {
				ref = ir_INT2F(jit_Z_LVAL(jit, val_addr));
			} else if (val_info == MAY_BE_DOUBLE) {
				ref = ir_D2F(jit_Z_DVAL(jit, val_addr));
			} else if (val_ffi_type && val_ffi_type->kind == ffi_type->kind) {
				ref = ir_LOAD_F(jit_FFI_CDATA_PTR(jit, jit_Z_PTR(jit, val_addr)));
			} else {
				ZEND_UNREACHABLE();
			}
			ir_STORE(ptr, ref);
			if (res_addr) {
				ref = ir_F2D(ref);
				jit_set_Z_DVAL(jit, res_addr, ref);
				if (Z_MODE(res_addr) != IS_REG) {
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_DOUBLE);
				}
			}
			break;
		case ZEND_FFI_TYPE_DOUBLE:
			if (val_info == MAY_BE_LONG) {
				ref = ir_INT2D(jit_Z_LVAL(jit, val_addr));
			} else if (val_info == MAY_BE_DOUBLE) {
				ref = jit_Z_DVAL(jit, val_addr);
			} else if (val_ffi_type && val_ffi_type->kind == ffi_type->kind) {
				ref = ir_LOAD_D(jit_FFI_CDATA_PTR(jit, jit_Z_PTR(jit, val_addr)));
			} else {
				ZEND_UNREACHABLE();
			}
			ir_STORE(ptr, ref);
			if (res_addr) {
				jit_set_Z_DVAL(jit, res_addr, ref);
				if (Z_MODE(res_addr) != IS_REG) {
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_DOUBLE);
				}
			}
			break;
		case ZEND_FFI_TYPE_BOOL:
			if (val_info == MAY_BE_FALSE) {
				ir_STORE(ptr, IR_FALSE);
				if (res_addr) {
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_FALSE);
				}
				return 1;
			} else if (val_info == MAY_BE_TRUE) {
				ir_STORE(ptr, IR_TRUE);
				if (res_addr) {
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_TRUE);
				}
				return 1;
			} else  if (val_info == (MAY_BE_FALSE|MAY_BE_TRUE)) {
				if (res_addr) {
					ref = jit_Z_TYPE_INFO(jit, val_addr);
					jit_set_Z_TYPE_INFO_ex(jit, res_addr, ref);
					ref = ir_TRUNC_U8(ref);
				} else {
					ref = jit_Z_TYPE(jit, val_addr);
				}
				ir_STORE(ptr, ir_SUB_U8(ref, ir_CONST_U8(IS_FALSE)));
				return 1;
			}
			ZEND_FALLTHROUGH;
		case ZEND_FFI_TYPE_UINT8:
			if (val_info == MAY_BE_LONG) {
				ref = ir_TRUNC_U8(jit_Z_LVAL(jit, val_addr));
			} else if (val_ffi_type && val_ffi_type->kind == ffi_type->kind) {
				ref = ir_LOAD_U8(jit_FFI_CDATA_PTR(jit, jit_Z_PTR(jit, val_addr)));
			} else {
				ZEND_UNREACHABLE();
			}
			ir_STORE(ptr, ref);
			if (res_addr) {
				ref = ir_ZEXT_L(ref);
				jit_set_Z_LVAL(jit, res_addr, ref);
				if (Z_MODE(res_addr) != IS_REG) {
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
				}
			}
			break;
		case ZEND_FFI_TYPE_CHAR:
			if ((val_info & (MAY_BE_GUARD|MAY_BE_REF|MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_STRING) {
				ir_ref str = jit_Z_PTR(jit, val_addr);

				// TODO: ZSTR_LEN() == 1 ???
				ref = ir_LOAD_C(ir_ADD_OFFSET(str, offsetof(zend_string, val)));
				ir_STORE(ptr, ref);
				if (res_addr) {
					ir_ref type_info = jit_Z_TYPE_INFO(jit, val_addr);
					ir_ref if_refcounted = ir_IF(ir_AND_U32(type_info, ir_CONST_U32(0xff00)));

					ir_IF_TRUE(if_refcounted);
					jit_GC_ADDREF(jit, str);
					ir_MERGE_WITH_EMPTY_FALSE(if_refcounted);
					jit_set_Z_PTR(jit, res_addr, str);
					jit_set_Z_TYPE_INFO_ex(jit, res_addr, type_info);
				}
				return 1;
			}
			ZEND_FALLTHROUGH;
		case ZEND_FFI_TYPE_SINT8:
			if (val_info == MAY_BE_LONG) {
				ref = ir_TRUNC_I8(jit_Z_LVAL(jit, val_addr));
			} else if (val_ffi_type && val_ffi_type->kind == ffi_type->kind) {
				ref = ir_LOAD_I8(jit_FFI_CDATA_PTR(jit, jit_Z_PTR(jit, val_addr)));
			} else {
				ZEND_UNREACHABLE();
			}
			ir_STORE(ptr, ref);
			if (res_addr) {
				ref = ir_SEXT_L(ref);
				jit_set_Z_LVAL(jit, res_addr, ref);
				if (Z_MODE(res_addr) != IS_REG) {
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
				}
			}
			break;
		case ZEND_FFI_TYPE_UINT16:
			if (val_info == MAY_BE_LONG) {
				ref = ir_TRUNC_U16(jit_Z_LVAL(jit, val_addr));
			} else if (val_ffi_type && val_ffi_type->kind == ffi_type->kind) {
				ref = ir_LOAD_U16(jit_FFI_CDATA_PTR(jit, jit_Z_PTR(jit, val_addr)));
			} else {
				ZEND_UNREACHABLE();
			}
			ir_STORE(ptr, ref);
			if (res_addr) {
				ref = ir_ZEXT_L(ref);
				jit_set_Z_LVAL(jit, res_addr, ref);
				if (Z_MODE(res_addr) != IS_REG) {
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
				}
			}
			break;
		case ZEND_FFI_TYPE_SINT16:
			if (val_info == MAY_BE_LONG) {
				ref = ir_TRUNC_I16(jit_Z_LVAL(jit, val_addr));
			} else if (val_ffi_type && val_ffi_type->kind == ffi_type->kind) {
				ref = ir_LOAD_I16(jit_FFI_CDATA_PTR(jit, jit_Z_PTR(jit, val_addr)));
			} else {
				ZEND_UNREACHABLE();
			}
			ir_STORE(ptr, ref);
			if (res_addr) {
				ref = ir_SEXT_L(ref);
				jit_set_Z_LVAL(jit, res_addr, ref);
				if (Z_MODE(res_addr) != IS_REG) {
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
				}
			}
			break;
#ifdef ZEND_ENABLE_ZVAL_LONG64
		case ZEND_FFI_TYPE_UINT32:
			if (val_info == MAY_BE_LONG) {
				ref = ir_TRUNC_U32(jit_Z_LVAL(jit, val_addr));
			} else if (val_ffi_type && val_ffi_type->kind == ffi_type->kind) {
				ref = ir_LOAD_U32(jit_FFI_CDATA_PTR(jit, jit_Z_PTR(jit, val_addr)));
			} else {
				ZEND_UNREACHABLE();
			}
			ir_STORE(ptr, ref);
			if (res_addr) {
				ref = ir_ZEXT_L(ref);
				jit_set_Z_LVAL(jit, res_addr, ref);
				if (Z_MODE(res_addr) != IS_REG) {
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
				}
			}
			break;
		case ZEND_FFI_TYPE_SINT32:
			if (val_info == MAY_BE_LONG) {
				ref = ir_TRUNC_I32(jit_Z_LVAL(jit, val_addr));
			} else if (val_ffi_type && val_ffi_type->kind == ffi_type->kind) {
				ref = ir_LOAD_I32(jit_FFI_CDATA_PTR(jit, jit_Z_PTR(jit, val_addr)));
			} else {
				ZEND_UNREACHABLE();
			}
			ir_STORE(ptr, ref);
			if (res_addr) {
				ref = ir_SEXT_L(ref);
				jit_set_Z_LVAL(jit, res_addr, ref);
				if (Z_MODE(res_addr) != IS_REG) {
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
				}
			}
			break;
		case ZEND_FFI_TYPE_UINT64:
		case ZEND_FFI_TYPE_SINT64:
			if (val_info == MAY_BE_LONG) {
				ref = jit_Z_LVAL(jit, val_addr);
			} else if (val_ffi_type && val_ffi_type->kind == ffi_type->kind) {
				ref = ir_LOAD_I64(jit_FFI_CDATA_PTR(jit, jit_Z_PTR(jit, val_addr)));
			} else {
				ZEND_UNREACHABLE();
			}
			ir_STORE(ptr, ref);
			if (res_addr) {
				jit_set_Z_LVAL(jit, res_addr, ref);
				if (Z_MODE(res_addr) != IS_REG) {
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
				}
			}
			break;
#else
		case ZEND_FFI_TYPE_UINT32:
		case ZEND_FFI_TYPE_SINT32:
			if (val_info == MAY_BE_LONG) {
				ref = jit_Z_LVAL(jit, val_addr);
			} else if (val_ffi_type && val_ffi_type->kind == ffi_type->kind) {
				ref = ir_LOAD_I32(jit_FFI_CDATA_PTR(jit, jit_Z_PTR(jit, val_addr)));
			} else {
				ZEND_UNREACHABLE();
			}
			ir_STORE(ptr, ref);
			if (res_addr) {
				jit_set_Z_LVAL(jit, res_addr, ref);
				if (Z_MODE(res_addr) != IS_REG) {
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
				}
			}
			break;
#endif
		case ZEND_FFI_TYPE_POINTER:
			if (val_info == MAY_BE_NULL) {
				ref = IR_NULL;
			} else if (val_ffi_type
			 && val_ffi_type->kind == ZEND_FFI_TYPE_POINTER
			 && (val_ffi_type == ffi_type
			  || ZEND_FFI_TYPE(val_ffi_type->pointer.type) == ZEND_FFI_TYPE(ffi_type->pointer.type)
			  || ZEND_FFI_TYPE(val_ffi_type->pointer.type)->kind == ZEND_FFI_TYPE_VOID
			  || ZEND_FFI_TYPE(ffi_type->pointer.type)->kind == ZEND_FFI_TYPE_VOID)) {
				ref = ir_LOAD_A(jit_FFI_CDATA_PTR(jit, jit_Z_PTR(jit, val_addr)));
			} else {
				ZEND_UNREACHABLE();
			}
			ir_STORE(ptr, ref);
			if (res_addr) {
				ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_zval_ffi_ptr),
					jit_ZVAL_ADDR(jit, res_addr), ir_CONST_ADDR(ffi_type), ref);
			}
			break;
		default:
			ZEND_UNREACHABLE();
	}

	return 1;
}

static int zend_jit_ffi_assign_dim(zend_jit_ctx      *jit,
                                   const zend_op     *opline,
                                   zend_ssa          *ssa,
                                   const zend_ssa_op *ssa_op,
                                   uint32_t           op1_info,
                                   zend_jit_addr      op1_addr,
                                   bool               op1_indirect,
                                   uint32_t           op2_info,
                                   zend_jit_addr      op2_addr,
                                   zend_ssa_range    *op2_range,
                                   uint32_t           val_info,
                                   zend_jit_addr      val_addr,
                                   zend_jit_addr      val_def_addr,
                                   zend_jit_addr      res_addr,
                                   zend_ffi_type     *op1_ffi_type,
                                   zend_ffi_type     *val_ffi_type,
                                   zend_jit_ffi_info *ffi_info)
{
	zend_ffi_type *el_type = ZEND_FFI_TYPE(op1_ffi_type->array.type);
	ir_ref obj_ref = jit_Z_PTR(jit, op1_addr);

	if (!zend_jit_ffi_guard(jit, opline, ssa, ssa_op->op1_use, ssa_op->op1_def, obj_ref, op1_ffi_type, ffi_info)) {
		return 0;
	}

	if (val_addr != val_def_addr && val_def_addr) {
		if (!zend_jit_update_regs(jit, (opline+1)->op1.var, val_addr, val_def_addr, val_info)) {
			return 0;
		}
		if (Z_MODE(val_def_addr) == IS_REG && Z_MODE(val_addr) != IS_REG) {
			val_addr = val_def_addr;
		}
	}

	if (!zend_jit_ffi_abc(jit, opline, op1_ffi_type, op2_info, op2_addr, op2_range)) {
		return 0;
	}

	ir_ref cdata_ref = ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_ffi_cdata, ptr)));

	if (op1_ffi_type->kind == ZEND_FFI_TYPE_POINTER) {
		cdata_ref = ir_LOAD_A(cdata_ref);
	}

	ir_ref ptr = ir_ADD_A(cdata_ref, ir_MUL_L(jit_Z_LVAL(jit, op2_addr), ir_CONST_LONG(el_type->size)));

	ZEND_ASSERT(!res_addr || RETURN_VALUE_USED(opline));

	if (!zend_jit_ffi_write(jit, el_type, ptr, val_info, val_addr, val_ffi_type, res_addr)) {
		return 0;
	}

	jit_FREE_OP(jit, (opline+1)->op1_type, (opline+1)->op1, val_info, opline);

	if (!op1_indirect) {
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
	}

	return 1;
}

static int zend_jit_ffi_assign_op_helper(zend_jit_ctx   *jit,
                                         const zend_op  *opline,
                                         uint8_t         opcode,
                                         zend_ffi_type  *el_type,
                                         ir_ref          op1,
                                         uint32_t        op2_info,
                                         zend_jit_addr   op2_addr)
{
	ir_op op;
	ir_type type;
	ir_ref op2;

	switch (opcode) {
		case ZEND_ADD:
			op = IR_ADD;
			break;
		case ZEND_SUB:
			op = IR_SUB;
			break;
		case ZEND_MUL:
			op = IR_MUL;
			break;
		case ZEND_BW_OR:
			op = IR_OR;
			break;
		case ZEND_BW_AND:
			op = IR_AND;
			break;
		case ZEND_BW_XOR:
			op = IR_XOR;
			break;
		case ZEND_SL:
			// TODO: negative shift
			op = IR_SHL;
			break;
		case ZEND_SR:
			// TODO: negative shift
			op = IR_SAR; /* TODO: SAR or SHR */
			break;
		case ZEND_MOD:
			// TODO: mod by zero and -1
			op = IR_MOD;
			break;
		default:
			ZEND_UNREACHABLE();
			return 0;
	}

	zend_ffi_type_kind type_kind = el_type->kind;

	if (type_kind == ZEND_FFI_TYPE_ENUM) {
		type_kind = el_type->enumeration.kind;
	}
	switch (type_kind) {
		case ZEND_FFI_TYPE_FLOAT:
			ZEND_ASSERT(op == IR_ADD || op == IR_SUB || op == IR_MUL);
			type = IR_FLOAT;
			if (op2_info == MAY_BE_LONG) {
				op2 = ir_INT2F(jit_Z_LVAL(jit, op2_addr));
			} else if (op2_info == MAY_BE_DOUBLE) {
				op2 = ir_D2F(jit_Z_DVAL(jit, op2_addr));
			} else {
				ZEND_UNREACHABLE();
				return 0;
			}
			break;
		case ZEND_FFI_TYPE_DOUBLE:
			ZEND_ASSERT(op == IR_ADD || op == IR_SUB || op == IR_MUL);
			type = IR_DOUBLE;
			if (op2_info == MAY_BE_LONG) {
				op2 = ir_INT2D(jit_Z_LVAL(jit, op2_addr));
			} else if (op2_info == MAY_BE_DOUBLE) {
				op2 = jit_Z_DVAL(jit, op2_addr);
			} else {
				ZEND_UNREACHABLE();
				return 0;
			}
			break;
		case ZEND_FFI_TYPE_BOOL:
			type = IR_U8;
			ZEND_ASSERT(opcode == ZEND_BW_AND || opcode == ZEND_BW_OR);
			if (op2_info == MAY_BE_LONG) {
				op2 = ir_TRUNC_U8(jit_Z_LVAL(jit, op2_addr));
			} else {
				ZEND_UNREACHABLE();
				return 0;
			}
			break;
		case ZEND_FFI_TYPE_UINT8:
			type = IR_U8;
			if (op2_info == MAY_BE_LONG) {
				op2 = ir_TRUNC_U8(jit_Z_LVAL(jit, op2_addr));
			} else {
				ZEND_UNREACHABLE();
				return 0;
			}
			break;
		case ZEND_FFI_TYPE_SINT8:
		case ZEND_FFI_TYPE_CHAR:
			type = IR_I8;
			if (op2_info == MAY_BE_LONG) {
				op2 = ir_TRUNC_I8(jit_Z_LVAL(jit, op2_addr));
			} else {
				ZEND_UNREACHABLE();
				return 0;
			}
			break;
		case ZEND_FFI_TYPE_UINT16:
			type = IR_U16;
			if (op2_info == MAY_BE_LONG) {
				op2 = ir_TRUNC_U16(jit_Z_LVAL(jit, op2_addr));
			} else {
				ZEND_UNREACHABLE();
				return 0;
			}
			break;
		case ZEND_FFI_TYPE_SINT16:
			type = IR_I16;
			if (op2_info == MAY_BE_LONG) {
				op2 = ir_TRUNC_I16(jit_Z_LVAL(jit, op2_addr));
			} else {
				ZEND_UNREACHABLE();
				return 0;
			}
			break;
#ifdef ZEND_ENABLE_ZVAL_LONG64
		case ZEND_FFI_TYPE_UINT32:
			type = IR_U32;
			if (op2_info == MAY_BE_LONG) {
				op2 = ir_TRUNC_U32(jit_Z_LVAL(jit, op2_addr));
			} else {
				ZEND_UNREACHABLE();
				return 0;
			}
			break;
		case ZEND_FFI_TYPE_SINT32:
			type = IR_I32;
			if (op2_info == MAY_BE_LONG) {
				op2 = ir_TRUNC_I32(jit_Z_LVAL(jit, op2_addr));
			} else {
				ZEND_UNREACHABLE();
				return 0;
			}
			break;
		case ZEND_FFI_TYPE_UINT64:
		case ZEND_FFI_TYPE_SINT64:
			type = IR_I64;
			if (op2_info == MAY_BE_LONG) {
				op2 = jit_Z_LVAL(jit, op2_addr);
			} else {
				ZEND_UNREACHABLE();
				return 0;
			}
			break;
#else
		case ZEND_FFI_TYPE_UINT32:
		case ZEND_FFI_TYPE_SINT32:
			type = IR_I32;
			if (op2_info == MAY_BE_LONG) {
				op2 = jit_Z_LVAL(jit, op2_addr);
			} else {
				ZEND_UNREACHABLE();
				return 0;
			}
			break;
#endif
		case ZEND_FFI_TYPE_POINTER:
			ZEND_ASSERT(opcode == ZEND_ADD || opcode == ZEND_SUB);
			ZEND_ASSERT(ZEND_FFI_TYPE(el_type->pointer.type)->size != 0);
			type = IR_ADDR;
			if (op2_info == MAY_BE_LONG) {
				op2 = ir_MUL_A(jit_Z_LVAL(jit, op2_addr), ir_CONST_LONG(ZEND_FFI_TYPE(el_type->pointer.type)->size));
			} else {
				ZEND_UNREACHABLE();
				return 0;
			}
			break;
		default:
			ZEND_UNREACHABLE();
			return 0;
	}

	ir_STORE(op1, ir_BINARY_OP(op, type, ir_LOAD(type, op1), op2));

	return 1;
}

static int zend_jit_ffi_assign_dim_op(zend_jit_ctx      *jit,
                                      const zend_op     *opline,
                                      zend_ssa          *ssa,
                                      const zend_ssa_op *ssa_op,
                                      uint32_t           op1_info,
                                      uint32_t           op1_def_info,
                                      zend_jit_addr      op1_addr,
                                      bool               op1_indirect,
                                      uint32_t           op2_info,
                                      zend_jit_addr      op2_addr,
                                      zend_ssa_range    *op2_range,
                                      uint32_t           op1_data_info,
                                      zend_jit_addr      op3_addr,
                                      zend_ssa_range    *op1_data_range,
                                      zend_ffi_type     *op1_ffi_type,
                                      zend_jit_ffi_info *ffi_info)
{
	zend_ffi_type *el_type = ZEND_FFI_TYPE(op1_ffi_type->array.type);
	ir_ref obj_ref = jit_Z_PTR(jit, op1_addr);

	if (!zend_jit_ffi_guard(jit, opline, ssa, ssa_op->op1_use, ssa_op->op1_def, obj_ref, op1_ffi_type, ffi_info)) {
		return 0;
	}

	if (!zend_jit_ffi_abc(jit, opline, op1_ffi_type, op2_info, op2_addr, op2_range)) {
		return 0;
	}

	ir_ref cdata_ref = ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_ffi_cdata, ptr)));

	if (op1_ffi_type->kind == ZEND_FFI_TYPE_POINTER) {
		cdata_ref = ir_LOAD_A(cdata_ref);
	}

	ir_ref ptr = ir_ADD_A(cdata_ref, ir_MUL_L(jit_Z_LVAL(jit, op2_addr), ir_CONST_LONG(el_type->size)));

	if (!zend_jit_ffi_assign_op_helper(jit, opline, opline->extended_value,
			el_type, ptr, op1_data_info, op3_addr)) {
		return 0;
	}

	jit_FREE_OP(jit, (opline+1)->op1_type, (opline+1)->op1, op1_data_info, opline);

	if (!op1_indirect) {
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
	}

	return 1;
}

static int zend_jit_ffi_fetch_obj(zend_jit_ctx        *jit,
                                  const zend_op       *opline,
                                  const zend_op_array *op_array,
                                  zend_ssa            *ssa,
                                  const zend_ssa_op   *ssa_op,
                                  uint32_t             op1_info,
                                  zend_jit_addr        op1_addr,
                                  bool                 op1_indirect,
                                  bool                 op1_avoid_refcounting,
                                  zend_ffi_field      *field,
                                  zend_jit_addr        res_addr,
                                  zend_ffi_type       *op1_ffi_type,
                                  zend_jit_ffi_info   *ffi_info)
{
	uint32_t res_info = RES_INFO();
	zend_ffi_type *field_type = ZEND_FFI_TYPE(field->type);
	ir_ref obj_ref = jit_Z_PTR(jit, op1_addr);

	if (!zend_jit_ffi_guard(jit, opline, ssa, ssa_op->op1_use, -1, obj_ref, op1_ffi_type, ffi_info)) {
		return 0;
	}

	ir_ref cdata_ref = ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_ffi_cdata, ptr)));
	ir_ref ptr = ir_ADD_A(cdata_ref, ir_CONST_LONG(field->offset));

	if (opline->opcode == ZEND_FETCH_OBJ_W) {
		jit_set_Z_PTR(jit, res_addr,
			ir_CALL_2(IR_ADDR, ir_CONST_FUNC(zend_ffi_api->cdata_create),
				ptr, ir_CONST_ADDR(field_type)));
		jit_set_Z_TYPE_INFO(jit, res_addr, IS_OBJECT_EX);
	} else {
		if (!zend_jit_ffi_read(jit, field_type, ptr, res_addr)) {
			return 0;
		}
	}

	if (res_info & MAY_BE_GUARD) {
		// TODO: ???
		ssa->var_info[ssa_op->result_def].type &= ~MAY_BE_GUARD;
	}

	if (!op1_avoid_refcounting && !op1_indirect) {
		if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
			jit_FREE_OP(jit,  opline->op1_type, opline->op1, op1_info, opline);
		}
	}

	return 1;
}

static int zend_jit_ffi_fetch_val(zend_jit_ctx        *jit,
                                  const zend_op       *opline,
                                  const zend_op_array *op_array,
                                  zend_ssa            *ssa,
                                  const zend_ssa_op   *ssa_op,
                                  uint32_t             op1_info,
                                  zend_jit_addr        op1_addr,
                                  bool                 op1_indirect,
                                  bool                 op1_avoid_refcounting,
                                  zend_jit_addr        res_addr,
                                  zend_ffi_type       *op1_ffi_type,
                                  zend_jit_ffi_info   *ffi_info)
{
	uint32_t res_info = RES_INFO();
	ir_ref obj_ref = jit_Z_PTR(jit, op1_addr);

	if (!zend_jit_ffi_guard(jit, opline, ssa, ssa_op->op1_use, -1, obj_ref, op1_ffi_type, ffi_info)) {
		return 0;
	}

	ir_ref ptr = ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_ffi_cdata, ptr)));

	if (opline->opcode == ZEND_FETCH_OBJ_W) {
		jit_set_Z_PTR(jit, res_addr,
			ir_CALL_2(IR_ADDR, ir_CONST_FUNC(zend_ffi_api->cdata_create),
				ptr, ir_CONST_ADDR(op1_ffi_type)));
		jit_set_Z_TYPE_INFO(jit, res_addr, IS_OBJECT_EX);
	} else {
		if (!zend_jit_ffi_read(jit, op1_ffi_type, ptr, res_addr)) {
			return 0;
		}
	}

	if (res_info & MAY_BE_GUARD) {
		// TODO: ???
		ssa->var_info[ssa_op->result_def].type &= ~MAY_BE_GUARD;
	}

	if (!op1_avoid_refcounting && !op1_indirect) {
		if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
			jit_FREE_OP(jit,  opline->op1_type, opline->op1, op1_info, opline);
		}
	}

	return 1;
}

static int zend_jit_ffi_fetch_sym(zend_jit_ctx        *jit,
                                  const zend_op       *opline,
                                  const zend_op_array *op_array,
                                  zend_ssa            *ssa,
                                  const zend_ssa_op   *ssa_op,
                                  uint32_t             op1_info,
                                  zend_jit_addr        op1_addr,
                                  bool                 op1_indirect,
                                  bool                 op1_avoid_refcounting,
                                  zend_ffi_symbol     *sym,
                                  zend_jit_addr        res_addr,
                                  HashTable           *op1_ffi_symbols,
                                  zend_jit_ffi_info   *ffi_info)
{
	uint32_t res_info = RES_INFO();
	zend_ffi_type *sym_type = ZEND_FFI_TYPE(sym->type);

	if (!zend_jit_ffi_symbols_guard(jit, opline, ssa, ssa_op->op1_use, -1, op1_addr, op1_ffi_symbols, ffi_info)) {
		return 0;
	}

	ir_ref ptr = ir_CONST_ADDR(sym->addr);

	if (opline->opcode == ZEND_FETCH_OBJ_W) {
		jit_set_Z_PTR(jit, res_addr,
			ir_CALL_2(IR_ADDR, ir_CONST_FUNC(zend_ffi_api->cdata_create),
				ptr, ir_CONST_ADDR(sym_type)));
		jit_set_Z_TYPE_INFO(jit, res_addr, IS_OBJECT_EX);
	} else {
		if (!zend_jit_ffi_read(jit, sym_type, ptr, res_addr)) {
			return 0;
		}
	}

	if (res_info & MAY_BE_GUARD) {
		// TODO: ???
		ssa->var_info[ssa_op->result_def].type &= ~MAY_BE_GUARD;
	}

	if (!op1_avoid_refcounting && !op1_indirect) {
		if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
			jit_FREE_OP(jit,  opline->op1_type, opline->op1, op1_info, opline);
		}
	}

	return 1;
}

static int zend_jit_ffi_assign_obj(zend_jit_ctx        *jit,
                                   const zend_op       *opline,
                                   const zend_op_array *op_array,
                                   zend_ssa            *ssa,
                                   const zend_ssa_op   *ssa_op,
                                   uint32_t             op1_info,
                                   zend_jit_addr        op1_addr,
                                   bool                 op1_indirect,
                                   zend_ffi_field      *field,
                                   uint32_t             val_info,
                                   zend_jit_addr        val_addr,
                                   zend_jit_addr        val_def_addr,
                                   zend_jit_addr        res_addr,
                                   zend_ffi_type       *op1_ffi_type,
                                   zend_ffi_type       *val_ffi_type,
                                   zend_jit_ffi_info   *ffi_info)
{
	zend_ffi_type *field_type = ZEND_FFI_TYPE(field->type);
	ir_ref obj_ref = jit_Z_PTR(jit, op1_addr);

	if (!zend_jit_ffi_guard(jit, opline, ssa, ssa_op->op1_use, ssa_op->op1_def, obj_ref, op1_ffi_type, ffi_info)) {
		return 0;
	}

	if (val_addr != val_def_addr && val_def_addr) {
		if (!zend_jit_update_regs(jit, (opline+1)->op1.var, val_addr, val_def_addr, val_info)) {
			return 0;
		}
		if (Z_MODE(val_def_addr) == IS_REG && Z_MODE(val_addr) != IS_REG) {
			val_addr = val_def_addr;
		}
	}

	ir_ref cdata_ref = ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_ffi_cdata, ptr)));
	ir_ref ptr = ir_ADD_A(cdata_ref, ir_CONST_LONG(field->offset));

	ZEND_ASSERT(!res_addr || RETURN_VALUE_USED(opline));

	if (!zend_jit_ffi_write(jit, field_type, ptr, val_info, val_addr, val_ffi_type, res_addr)) {
		return 0;
	}

	jit_FREE_OP(jit, (opline+1)->op1_type, (opline+1)->op1, val_info, opline);

	if (!op1_indirect) {
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
	}

	return 1;
}

static int zend_jit_ffi_assign_val(zend_jit_ctx        *jit,
                                   const zend_op       *opline,
                                   const zend_op_array *op_array,
                                   zend_ssa            *ssa,
                                   const zend_ssa_op   *ssa_op,
                                   uint32_t             op1_info,
                                   zend_jit_addr        op1_addr,
                                   bool                 op1_indirect,
                                   uint32_t             val_info,
                                   zend_jit_addr        val_addr,
                                   zend_jit_addr        val_def_addr,
                                   zend_jit_addr        res_addr,
                                   zend_ffi_type       *op1_ffi_type,
                                   zend_ffi_type       *val_ffi_type,
                                   zend_jit_ffi_info   *ffi_info)
{
	ir_ref obj_ref = jit_Z_PTR(jit, op1_addr);

	if (!zend_jit_ffi_guard(jit, opline, ssa, ssa_op->op1_use, ssa_op->op1_def, obj_ref, op1_ffi_type, ffi_info)) {
		return 0;
	}

	if (val_addr != val_def_addr && val_def_addr) {
		if (!zend_jit_update_regs(jit, (opline+1)->op1.var, val_addr, val_def_addr, val_info)) {
			return 0;
		}
		if (Z_MODE(val_def_addr) == IS_REG && Z_MODE(val_addr) != IS_REG) {
			val_addr = val_def_addr;
		}
	}

	ir_ref ptr = ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_ffi_cdata, ptr)));

	ZEND_ASSERT(!res_addr || RETURN_VALUE_USED(opline));

	if (!zend_jit_ffi_write(jit, op1_ffi_type, ptr, val_info, val_addr, val_ffi_type, res_addr)) {
		return 0;
	}

	jit_FREE_OP(jit, (opline+1)->op1_type, (opline+1)->op1, val_info, opline);

	if (!op1_indirect) {
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
	}

	return 1;
}

static int zend_jit_ffi_assign_sym(zend_jit_ctx        *jit,
                                   const zend_op       *opline,
                                   const zend_op_array *op_array,
                                   zend_ssa            *ssa,
                                   const zend_ssa_op   *ssa_op,
                                   uint32_t             op1_info,
                                   zend_jit_addr        op1_addr,
                                   bool                 op1_indirect,
                                   zend_ffi_symbol     *sym,
                                   uint32_t             val_info,
                                   zend_jit_addr        val_addr,
                                   zend_jit_addr        val_def_addr,
                                   zend_jit_addr        res_addr,
                                   HashTable           *op1_ffi_symbols,
                                   zend_ffi_type       *val_ffi_type,
                                   zend_jit_ffi_info   *ffi_info)
{
	zend_ffi_type *sym_type = ZEND_FFI_TYPE(sym->type);

	if (!zend_jit_ffi_symbols_guard(jit, opline, ssa, ssa_op->op1_use, ssa_op->op1_def, op1_addr, op1_ffi_symbols, ffi_info)) {
		return 0;
	}

	if (val_addr != val_def_addr && val_def_addr) {
		if (!zend_jit_update_regs(jit, (opline+1)->op1.var, val_addr, val_def_addr, val_info)) {
			return 0;
		}
		if (Z_MODE(val_def_addr) == IS_REG && Z_MODE(val_addr) != IS_REG) {
			val_addr = val_def_addr;
		}
	}

	ZEND_ASSERT(!res_addr || RETURN_VALUE_USED(opline));

	ir_ref ptr = ir_CONST_ADDR(sym->addr);
	if (!zend_jit_ffi_write(jit, sym_type, ptr, val_info, val_addr, val_ffi_type, res_addr)) {
		return 0;
	}

	jit_FREE_OP(jit, (opline+1)->op1_type, (opline+1)->op1, val_info, opline);

	if (!op1_indirect) {
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
	}

	return 1;
}

static int zend_jit_ffi_assign_obj_op(zend_jit_ctx        *jit,
                                      const zend_op       *opline,
                                      const zend_op_array *op_array,
                                      zend_ssa            *ssa,
                                      const zend_ssa_op   *ssa_op,
                                      uint32_t             op1_info,
                                      zend_jit_addr        op1_addr,
                                      bool                 op1_indirect,
                                      zend_ffi_field      *field,
                                      uint32_t             val_info,
                                      zend_jit_addr        val_addr,
                                      zend_ffi_type       *op1_ffi_type,
                                      zend_jit_ffi_info   *ffi_info)
{
	zend_ffi_type *field_type = ZEND_FFI_TYPE(field->type);
	ir_ref obj_ref = jit_Z_PTR(jit, op1_addr);

	if (!zend_jit_ffi_guard(jit, opline, ssa, ssa_op->op1_use, ssa_op->op1_def, obj_ref, op1_ffi_type, ffi_info)) {
		return 0;
	}

	ir_ref cdata_ref = ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_ffi_cdata, ptr)));
	ir_ref ptr = ir_ADD_A(cdata_ref, ir_CONST_LONG(field->offset));

	if (!zend_jit_ffi_assign_op_helper(jit, opline, opline->extended_value,
			field_type, ptr, val_info, val_addr)) {
		return 0;
	}

	jit_FREE_OP(jit, (opline+1)->op1_type, (opline+1)->op1, val_info, opline);

	if (!op1_indirect) {
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
	}

	return 1;
}

static int zend_jit_ffi_assign_val_op(zend_jit_ctx        *jit,
                                      const zend_op       *opline,
                                      const zend_op_array *op_array,
                                      zend_ssa            *ssa,
                                      const zend_ssa_op   *ssa_op,
                                      uint32_t             op1_info,
                                      zend_jit_addr        op1_addr,
                                      bool                 op1_indirect,
                                      uint32_t             val_info,
                                      zend_jit_addr        val_addr,
                                      zend_ffi_type       *op1_ffi_type,
                                      zend_jit_ffi_info   *ffi_info)
{
	ir_ref obj_ref = jit_Z_PTR(jit, op1_addr);

	if (!zend_jit_ffi_guard(jit, opline, ssa, ssa_op->op1_use, ssa_op->op1_def, obj_ref, op1_ffi_type, ffi_info)) {
		return 0;
	}

	ir_ref ptr = ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_ffi_cdata, ptr)));

	if (!zend_jit_ffi_assign_op_helper(jit, opline, opline->extended_value,
			op1_ffi_type, ptr, val_info, val_addr)) {
		return 0;
	}

	jit_FREE_OP(jit, (opline+1)->op1_type, (opline+1)->op1, val_info, opline);

	if (!op1_indirect) {
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
	}

	return 1;
}

static int zend_jit_ffi_assign_sym_op(zend_jit_ctx        *jit,
                                      const zend_op       *opline,
                                      const zend_op_array *op_array,
                                      zend_ssa            *ssa,
                                      const zend_ssa_op   *ssa_op,
                                      uint32_t             op1_info,
                                      zend_jit_addr        op1_addr,
                                      bool                 op1_indirect,
                                      zend_ffi_symbol     *sym,
                                      uint32_t             val_info,
                                      zend_jit_addr        val_addr,
                                      HashTable           *op1_ffi_symbols,
                                      zend_jit_ffi_info   *ffi_info)
{
	zend_ffi_type *sym_type = ZEND_FFI_TYPE(sym->type);

	if (!zend_jit_ffi_symbols_guard(jit, opline, ssa, ssa_op->op1_use, ssa_op->op1_def, op1_addr, op1_ffi_symbols, ffi_info)) {
		return 0;
	}

	ir_ref ptr = ir_CONST_ADDR(sym->addr);
	if (!zend_jit_ffi_assign_op_helper(jit, opline, opline->extended_value,
			sym_type, ptr, val_info, val_addr)) {
		return 0;
	}

	jit_FREE_OP(jit, (opline+1)->op1_type, (opline+1)->op1, val_info, opline);

	if (!op1_indirect) {
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
	}

	return 1;
}

static int zend_jit_ffi_incdec_helper(zend_jit_ctx   *jit,
                                      const zend_op  *opline,
                                      uint8_t         opcode,
                                      zend_ffi_type  *el_type,
                                      ir_ref          op1,
                                      zend_jit_addr   res_addr)
{
	ir_op op;
	ir_type type;
	ir_ref op2;
	ir_ref ref = IR_UNUSED;

	switch (opcode) {
		case ZEND_PRE_INC_OBJ:
		case ZEND_POST_INC_OBJ:
			op = IR_ADD;
			break;
		case ZEND_PRE_DEC_OBJ:
		case ZEND_POST_DEC_OBJ:
			op = IR_SUB;
			break;
		default:
			ZEND_UNREACHABLE();
			return 0;
	}

	zend_ffi_type_kind type_kind = el_type->kind;

	if (type_kind == ZEND_FFI_TYPE_ENUM) {
		type_kind = el_type->enumeration.kind;
	}
	switch (type_kind) {
		case ZEND_FFI_TYPE_FLOAT:
			type = IR_FLOAT;
			op2 = ir_CONST_FLOAT(1.0);
			break;
		case ZEND_FFI_TYPE_DOUBLE:
			type = IR_DOUBLE;
			op2 = ir_CONST_DOUBLE(1.0);
			break;
		case ZEND_FFI_TYPE_UINT8:
			type = IR_U8;
			op2 = ir_CONST_U8(1);
			break;
		case ZEND_FFI_TYPE_SINT8:
		case ZEND_FFI_TYPE_CHAR:
			type = IR_I8;
			op2 = ir_CONST_I8(1);
			break;
		case ZEND_FFI_TYPE_UINT16:
			type = IR_U16;
			op2 = ir_CONST_U16(1);
			break;
		case ZEND_FFI_TYPE_SINT16:
			type = IR_I16;
			op2 = ir_CONST_I16(1);
			break;
		case ZEND_FFI_TYPE_UINT32:
			type = IR_U32;
			op2 = ir_CONST_U32(1);
			break;
		case ZEND_FFI_TYPE_SINT32:
			type = IR_I32;
			op2 = ir_CONST_I32(1);
			break;
		case ZEND_FFI_TYPE_UINT64:
			type = IR_U64;
			op2 = ir_CONST_U64(1);
			break;
		case ZEND_FFI_TYPE_SINT64:
			type = IR_I64;
			op2 = ir_CONST_I64(1);
			break;
		case ZEND_FFI_TYPE_POINTER:
			ZEND_ASSERT(ZEND_FFI_TYPE(el_type->pointer.type)->size != 0);
			type = IR_ADDR;
			op2 = ir_CONST_LONG(ZEND_FFI_TYPE(el_type->pointer.type)->size);
			break;
		default:
			ZEND_UNREACHABLE();
			return 0;
	}

	ref = ir_LOAD(type, op1);

	if (res_addr && (opcode == ZEND_POST_INC_OBJ || opcode == ZEND_POST_DEC_OBJ)) {
		uint32_t res_type = IS_UNDEF;

		switch (type) {
			case IR_FLOAT:
				jit_set_Z_DVAL(jit, res_addr, ir_F2D(ref));
				res_type = IS_DOUBLE;
				break;
			case IR_DOUBLE:
				jit_set_Z_DVAL(jit, res_addr, ref);
				res_type = IS_DOUBLE;
				break;
			case IR_I8:
			case IR_I16:
#ifdef ZEND_ENABLE_ZVAL_LONG64
			case IR_I32:
				jit_set_Z_LVAL(jit, res_addr, ir_SEXT_L(ref));
				res_type = IS_LONG;
				break;
			case IR_I64:
#else
			case IR_I32:
#endif
				jit_set_Z_LVAL(jit, res_addr, ref);
				res_type = IS_LONG;
				break;
			case IR_U8:
			case IR_U16:
#ifdef ZEND_ENABLE_ZVAL_LONG64
			case IR_U32:
				jit_set_Z_LVAL(jit, res_addr, ir_ZEXT_L(ref));
				res_type = IS_LONG;
				break;
			case IR_U64:
#else
			case IR_U32:
#endif
				jit_set_Z_LVAL(jit, res_addr, ref);
				res_type = IS_LONG;
				break;
			case IR_ADDR:
				if ((el_type->attr & ZEND_FFI_ATTR_CONST)
				 && ZEND_FFI_TYPE(el_type->pointer.type)->kind == ZEND_FFI_TYPE_CHAR) {
					ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_zval_string), jit_ZVAL_ADDR(jit, res_addr), ref);
				} else {
					ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_zval_ffi_ptr),
						jit_ZVAL_ADDR(jit, res_addr), ir_CONST_ADDR(el_type), ref);
				}
				break;
			default:
				ZEND_UNREACHABLE();
				return 0;
		}
		if (res_type && Z_MODE(res_addr) != IS_REG) {
			jit_set_Z_TYPE_INFO(jit, res_addr, res_type);
		}
	}

	ref = ir_BINARY_OP(op, type, ref, op2);
	ir_STORE(op1, ref);

	if (res_addr && (opcode == ZEND_PRE_INC_OBJ || opcode == ZEND_PRE_DEC_OBJ)) {
		uint32_t res_type = IS_UNDEF;

		switch (type) {
			case IR_FLOAT:
				jit_set_Z_DVAL(jit, res_addr, ir_F2D(ref));
				res_type = IS_DOUBLE;
				break;
			case IR_DOUBLE:
				jit_set_Z_DVAL(jit, res_addr, ref);
				res_type = IS_DOUBLE;
				break;
			case IR_I8:
			case IR_I16:
#ifdef ZEND_ENABLE_ZVAL_LONG64
			case IR_I32:
				jit_set_Z_LVAL(jit, res_addr, ir_SEXT_L(ref));
				res_type = IS_LONG;
				break;
			case IR_I64:
#else
			case IR_I32:
#endif
				jit_set_Z_LVAL(jit, res_addr, ref);
				res_type = IS_LONG;
				break;
			case IR_U8:
			case IR_U16:
#ifdef ZEND_ENABLE_ZVAL_LONG64
			case IR_U32:
				jit_set_Z_LVAL(jit, res_addr, ir_ZEXT_L(ref));
				res_type = IS_LONG;
				break;
			case IR_U64:
#else
			case IR_U32:
#endif
				jit_set_Z_LVAL(jit, res_addr, ref);
				res_type = IS_LONG;
				break;
			case IR_ADDR:
				if ((el_type->attr & ZEND_FFI_ATTR_CONST)
				 && ZEND_FFI_TYPE(el_type->pointer.type)->kind == ZEND_FFI_TYPE_CHAR) {
					ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_zval_string), jit_ZVAL_ADDR(jit, res_addr), ref);
				} else {
					ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_zval_ffi_ptr),
						jit_ZVAL_ADDR(jit, res_addr), ir_CONST_ADDR(el_type), ref);
				}
				break;
			default:
				ZEND_UNREACHABLE();
				return 0;
		}
		if (res_type && Z_MODE(res_addr) != IS_REG) {
			jit_set_Z_TYPE_INFO(jit, res_addr, res_type);
		}
	}

	return 1;
}

static int zend_jit_ffi_incdec_obj(zend_jit_ctx        *jit,
                                   const zend_op       *opline,
                                   const zend_op_array *op_array,
                                   zend_ssa            *ssa,
                                   const zend_ssa_op   *ssa_op,
                                   uint32_t             op1_info,
                                   zend_jit_addr        op1_addr,
                                   bool                 op1_indirect,
                                   zend_ffi_field      *field,
                                   zend_jit_addr        res_addr,
                                   zend_ffi_type       *op1_ffi_type,
                                   zend_jit_ffi_info   *ffi_info)
{
	zend_ffi_type *field_type = ZEND_FFI_TYPE(field->type);
	ir_ref obj_ref = jit_Z_PTR(jit, op1_addr);

	if (!zend_jit_ffi_guard(jit, opline, ssa, ssa_op->op1_use, ssa_op->op1_def, obj_ref, op1_ffi_type, ffi_info)) {
		return 0;
	}

	ir_ref cdata_ref = ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_ffi_cdata, ptr)));
	ir_ref ptr = ir_ADD_A(cdata_ref, ir_CONST_LONG(field->offset));

	if (!zend_jit_ffi_incdec_helper(jit, opline, opline->opcode, field_type, ptr, res_addr)) {
		return 0;
	}

	if (!op1_indirect) {
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
	}

	return 1;
}

static int zend_jit_ffi_incdec_val(zend_jit_ctx        *jit,
                                   const zend_op       *opline,
                                   const zend_op_array *op_array,
                                   zend_ssa            *ssa,
                                   const zend_ssa_op   *ssa_op,
                                   uint32_t             op1_info,
                                   zend_jit_addr        op1_addr,
                                   bool                 op1_indirect,
                                   zend_jit_addr        res_addr,
                                   zend_ffi_type       *op1_ffi_type,
                                   zend_jit_ffi_info   *ffi_info)
{
	ir_ref obj_ref = jit_Z_PTR(jit, op1_addr);

	if (!zend_jit_ffi_guard(jit, opline, ssa, ssa_op->op1_use, ssa_op->op1_def, obj_ref, op1_ffi_type, ffi_info)) {
		return 0;
	}

	ir_ref ptr = ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_ffi_cdata, ptr)));

	if (!zend_jit_ffi_incdec_helper(jit, opline, opline->opcode, op1_ffi_type, ptr, res_addr)) {
		return 0;
	}

	if (!op1_indirect) {
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
	}

	return 1;
}

static int zend_jit_ffi_incdec_sym(zend_jit_ctx        *jit,
                                   const zend_op       *opline,
                                   const zend_op_array *op_array,
                                   zend_ssa            *ssa,
                                   const zend_ssa_op   *ssa_op,
                                   uint32_t             op1_info,
                                   zend_jit_addr        op1_addr,
                                   bool                 op1_indirect,
                                   zend_ffi_symbol     *sym,
                                   zend_jit_addr        res_addr,
                                   HashTable           *op1_ffi_symbols,
                                   zend_jit_ffi_info   *ffi_info)
{
	zend_ffi_type *sym_type = ZEND_FFI_TYPE(sym->type);

	if (!zend_jit_ffi_symbols_guard(jit, opline, ssa, ssa_op->op1_use, ssa_op->op1_def, op1_addr, op1_ffi_symbols, ffi_info)) {
		return 0;
	}

	ir_ref ptr = ir_CONST_ADDR(sym->addr);
	if (!zend_jit_ffi_incdec_helper(jit, opline, opline->opcode, sym_type, ptr, res_addr)) {
		return 0;
	}

	if (!op1_indirect) {
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
	}

	return 1;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
