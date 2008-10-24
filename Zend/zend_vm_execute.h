/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2008 Zend Technologies Ltd. (http://www.zend.com) |
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
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

static opcode_handler_t zend_user_opcode_handlers[256] = {(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL,(opcode_handler_t)NULL};

static zend_uchar zend_user_opcodes[256] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255};

static opcode_handler_t zend_vm_get_opcode_handler(zend_uchar opcode, zend_op* op);


#define ZEND_VM_CONTINUE()   return 0
#define ZEND_VM_RETURN()     return 1
#define ZEND_VM_DISPATCH(opcode, opline) return zend_vm_get_opcode_handler(opcode, opline)(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);

#define ZEND_OPCODE_HANDLER_ARGS_PASSTHRU_INTERNAL execute_data TSRMLS_CC

ZEND_API void execute(zend_op_array *op_array TSRMLS_DC)
{
	zend_execute_data execute_data;


	if (EG(exception)) {
		return;
	}

	/* Initialize execute_data */
	EX(fbc) = NULL;
	EX(object) = NULL;
	EX(old_error_reporting) = NULL;
	if (op_array->T < TEMP_VAR_STACK_LIMIT) {
		EX(Ts) = (temp_variable *) do_alloca(sizeof(temp_variable) * op_array->T);
	} else {
		EX(Ts) = (temp_variable *) safe_emalloc(sizeof(temp_variable), op_array->T, 0);
	}
	EX(CVs) = (zval***)do_alloca(sizeof(zval**) * op_array->last_var);
	memset(EX(CVs), 0, sizeof(zval**) * op_array->last_var);
	EX(op_array) = op_array;
	EX(original_in_execution) = EG(in_execution);
	EX(symbol_table) = EG(active_symbol_table);
	EX(prev_execute_data) = EG(current_execute_data);
	EG(current_execute_data) = &execute_data;

	EG(in_execution) = 1;
	if (op_array->start_op) {
		ZEND_VM_SET_OPCODE(op_array->start_op);
	} else {
		ZEND_VM_SET_OPCODE(op_array->opcodes);
	}

	if (op_array->uses_this && EG(This)) {
		EG(This)->refcount++; /* For $this pointer */
		if (zend_hash_add(EG(active_symbol_table), "this", sizeof("this"), &EG(This), sizeof(zval *), NULL)==FAILURE) {
			EG(This)->refcount--;
		}
	}

	EG(opline_ptr) = &EX(opline);

	EX(function_state).function = (zend_function *) op_array;
	EG(function_state_ptr) = &EX(function_state);
#if ZEND_DEBUG
	/* function_state.function_symbol_table is saved as-is to a stack,
	 * which is an intentional UMR.  Shut it up if we're in DEBUG.
	 */
	EX(function_state).function_symbol_table = NULL;
#endif
	
	while (1) {
#ifdef ZEND_WIN32
		if (EG(timed_out)) {
			zend_timeout(0);
		}
#endif

		if (EX(opline)->handler(&execute_data TSRMLS_CC) > 0) {
      return;
		}

	}
	zend_error_noreturn(E_ERROR, "Arrived at end of main loop which shouldn't happen");
}

#undef EX
#define EX(element) execute_data->element

static int ZEND_JMP_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
#if DEBUG_ZEND>=2
	printf("Jumping to %d\n", EX(opline)->op1.u.opline_num);
#endif
	ZEND_VM_SET_OPCODE(EX(opline)->op1.u.jmp_addr);
	ZEND_VM_CONTINUE(); /* CHECK_ME */
}

static int ZEND_INIT_STRING_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *tmp = &EX_T(EX(opline)->result.u.var).tmp_var;

	tmp->value.str.val = emalloc(1);
	tmp->value.str.val[0] = 0;
	tmp->value.str.len = 0;
	tmp->refcount = 1;
	tmp->type = IS_STRING;
	tmp->is_ref = 0;
	ZEND_VM_NEXT_OPCODE();
}

static int zend_do_fcall_common_helper_SPEC(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval **original_return_value;
	zend_class_entry *current_scope = NULL;
	zval *current_this = NULL;
	int return_value_used = RETURN_VALUE_USED(opline);
	zend_bool should_change_scope;
	zend_op *ctor_opline;

	if (EX(function_state).function->common.fn_flags & (ZEND_ACC_ABSTRACT|ZEND_ACC_DEPRECATED)) {
		if (EX(function_state).function->common.fn_flags & ZEND_ACC_ABSTRACT) {
			zend_error_noreturn(E_ERROR, "Cannot call abstract method %s::%s()", EX(function_state).function->common.scope->name, EX(function_state).function->common.function_name);
			ZEND_VM_NEXT_OPCODE(); /* Never reached */
		}
		if (EX(function_state).function->common.fn_flags & ZEND_ACC_DEPRECATED) {
			zend_error(E_STRICT, "Function %s%s%s() is deprecated",
				EX(function_state).function->common.scope ? EX(function_state).function->common.scope->name : "",
				EX(function_state).function->common.scope ? "::" : "",
				EX(function_state).function->common.function_name);
		}
	}

	zend_ptr_stack_2_push(&EG(argument_stack), (void *)(zend_uintptr_t)opline->extended_value, NULL);

	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;

	if (EX(function_state).function->type == ZEND_USER_FUNCTION
		|| EX(function_state).function->common.scope) {
		should_change_scope = 1;
		current_this = EG(This);
		EG(This) = EX(object);
		current_scope = EG(scope);
		EG(scope) = (EX(function_state).function->type == ZEND_USER_FUNCTION || !EX(object)) ? EX(function_state).function->common.scope : NULL;
	} else {
		should_change_scope = 0;
	}

	EX_T(opline->result.u.var).var.fcall_returned_reference = 0;

	if (EX(function_state).function->common.scope) {
		if (!EG(This) && !(EX(function_state).function->common.fn_flags & ZEND_ACC_STATIC)) {
			int severity;
			char *severity_word;
			if (EX(function_state).function->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
				severity = E_STRICT;
				severity_word = "should not";
			} else {
				severity = E_ERROR;
				severity_word = "cannot";
			}
			zend_error(severity, "Non-static method %s::%s() %s be called statically", EX(function_state).function->common.scope->name, EX(function_state).function->common.function_name, severity_word);
		}
	}
	if (EX(function_state).function->type == ZEND_INTERNAL_FUNCTION) {
		unsigned char return_reference = EX(function_state).function->common.return_reference;

		ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
		INIT_ZVAL(*(EX_T(opline->result.u.var).var.ptr));

		if (EX(function_state).function->common.arg_info) {
			zend_uint i=0;
			zval **p;
			ulong arg_count;

			p = (zval **) EG(argument_stack).top_element-2;
			arg_count = (ulong)(zend_uintptr_t) *p;

			while (arg_count>0) {
				zend_verify_arg_type(EX(function_state).function, ++i, *(p-arg_count) TSRMLS_CC);
				arg_count--;
			}
		}
		if (!zend_execute_internal) {
			/* saves one function call if zend_execute_internal is not used */
			((zend_internal_function *) EX(function_state).function)->handler(opline->extended_value, EX_T(opline->result.u.var).var.ptr, EX(function_state).function->common.return_reference?&EX_T(opline->result.u.var).var.ptr:NULL, EX(object), return_value_used TSRMLS_CC);
		} else {
			zend_execute_internal(execute_data, return_value_used TSRMLS_CC);
		}

		EG(current_execute_data) = execute_data;

/*	We shouldn't fix bad extensions here,
    because it can break proper ones (Bug #34045)
		if (!EX(function_state).function->common.return_reference) {
			EX_T(opline->result.u.var).var.ptr->is_ref = 0;
			EX_T(opline->result.u.var).var.ptr->refcount = 1;
		}
*/
		if (!return_value_used) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		} else {
			EX_T(opline->result.u.var).var.fcall_returned_reference = return_reference;
		}
	} else if (EX(function_state).function->type == ZEND_USER_FUNCTION) {
		EX_T(opline->result.u.var).var.ptr = NULL;
		if (EG(symtable_cache_ptr)>=EG(symtable_cache)) {
			/*printf("Cache hit!  Reusing %x\n", symtable_cache[symtable_cache_ptr]);*/
			EX(function_state).function_symbol_table = *(EG(symtable_cache_ptr)--);
		} else {
			ALLOC_HASHTABLE(EX(function_state).function_symbol_table);
			zend_hash_init(EX(function_state).function_symbol_table, 0, NULL, ZVAL_PTR_DTOR, 0);
			/*printf("Cache miss!  Initialized %x\n", function_state.function_symbol_table);*/
		}
		EG(active_symbol_table) = EX(function_state).function_symbol_table;
		original_return_value = EG(return_value_ptr_ptr);
		EG(return_value_ptr_ptr) = EX_T(opline->result.u.var).var.ptr_ptr;
		EG(active_op_array) = (zend_op_array *) EX(function_state).function;

		zend_execute(EG(active_op_array) TSRMLS_CC);
		EX_T(opline->result.u.var).var.fcall_returned_reference = EG(active_op_array)->return_reference;

		if (return_value_used && !EX_T(opline->result.u.var).var.ptr) {
			if (!EG(exception)) {
				ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
				INIT_ZVAL(*EX_T(opline->result.u.var).var.ptr);
			}
		} else if (!return_value_used && EX_T(opline->result.u.var).var.ptr) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}

		EG(opline_ptr) = &EX(opline);
		EG(active_op_array) = EX(op_array);
		EG(return_value_ptr_ptr)=original_return_value;
		if (EG(symtable_cache_ptr)>=EG(symtable_cache_limit)) {
			zend_hash_destroy(EX(function_state).function_symbol_table);
			FREE_HASHTABLE(EX(function_state).function_symbol_table);
		} else {
			/* clean before putting into the cache, since clean
			   could call dtors, which could use cached hash */
			zend_hash_clean(EX(function_state).function_symbol_table);
			*(++EG(symtable_cache_ptr)) = EX(function_state).function_symbol_table;
		}
		EG(active_symbol_table) = EX(symbol_table);
	} else { /* ZEND_OVERLOADED_FUNCTION */
		ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
		INIT_ZVAL(*(EX_T(opline->result.u.var).var.ptr));

			/* Not sure what should be done here if it's a static method */
		if (EX(object)) {
			Z_OBJ_HT_P(EX(object))->call_method(EX(fbc)->common.function_name, opline->extended_value, EX_T(opline->result.u.var).var.ptr, &EX_T(opline->result.u.var).var.ptr, EX(object), return_value_used TSRMLS_CC);
		} else {
			zend_error_noreturn(E_ERROR, "Cannot call overloaded function for non-object");
		}

		if (EX(function_state).function->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY) {
			efree(EX(function_state).function->common.function_name);
		}
		efree(EX(fbc));

		if (!return_value_used) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		} else {
			EX_T(opline->result.u.var).var.ptr->is_ref = 0;
			EX_T(opline->result.u.var).var.ptr->refcount = 1;
		}
	}

	EX(function_state).function = (zend_function *) EX(op_array);
	EG(function_state_ptr) = &EX(function_state);
	ctor_opline = (zend_op*)zend_ptr_stack_pop(&EG(arg_types_stack));

	if (EG(This)) {
		if (EG(exception) && ctor_opline) {
			if (RETURN_VALUE_USED(ctor_opline)) {
				EG(This)->refcount--;
			}
			if (EG(This)->refcount == 1) {
				zend_object_store_ctor_failed(EG(This) TSRMLS_CC);
			}
		}
		if (should_change_scope) {
			zval_ptr_dtor(&EG(This));
		}
	}

	if (should_change_scope) {
		EG(This) = current_this;
		EG(scope) = current_scope;
	}
	zend_ptr_stack_2_pop(&EG(arg_types_stack), (void**)&EX(object), (void**)&EX(fbc));

	zend_ptr_stack_clear_multiple(TSRMLS_C);

	if (EG(exception)) {
		zend_throw_exception_internal(NULL TSRMLS_CC);
		if (return_value_used && EX_T(opline->result.u.var).var.ptr) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	EX(function_state).function = EX(fbc);
	return zend_do_fcall_common_helper_SPEC(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_CATCH_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_class_entry *ce;

	/* Check whether an exception has been thrown, if not, jump over code */
	if (EG(exception) == NULL) {
		ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[opline->extended_value]);
		ZEND_VM_CONTINUE(); /* CHECK_ME */
	}
	ce = Z_OBJCE_P(EG(exception));
	if (ce != EX_T(opline->op1.u.var).class_entry) {
		if (!instanceof_function(ce, EX_T(opline->op1.u.var).class_entry TSRMLS_CC)) {
			if (opline->op1.u.EA.type) {
				zend_throw_exception_internal(NULL TSRMLS_CC);
				ZEND_VM_NEXT_OPCODE();
			}
			ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[opline->extended_value]);
			ZEND_VM_CONTINUE(); /* CHECK_ME */
		}
	}

	zend_hash_update(EG(active_symbol_table), opline->op2.u.constant.value.str.val,
		opline->op2.u.constant.value.str.len+1, &EG(exception), sizeof(zval *), (void **) NULL);
	EG(exception) = NULL;
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_RECV_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval **param;
	zend_uint arg_num = Z_LVAL(opline->op1.u.constant);

	if (zend_ptr_stack_get_arg(arg_num, (void **) &param TSRMLS_CC)==FAILURE) {
		char *space;
		char *class_name = get_active_class_name(&space TSRMLS_CC);
		zend_execute_data *ptr = EX(prev_execute_data);

		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, NULL TSRMLS_CC);
		if(ptr && ptr->op_array) {
			zend_error(E_WARNING, "Missing argument %ld for %s%s%s(), called in %s on line %d and defined", opline->op1.u.constant.value.lval, class_name, space, get_active_function_name(TSRMLS_C), ptr->op_array->filename, ptr->opline->lineno);
		} else {
			zend_error(E_WARNING, "Missing argument %ld for %s%s%s()", opline->op1.u.constant.value.lval, class_name, space, get_active_function_name(TSRMLS_C));
		}
		if (opline->result.op_type == IS_VAR) {
			PZVAL_UNLOCK_FREE(*EX_T(opline->result.u.var).var.ptr_ptr);
		}
	} else {
		zend_free_op free_res;
		zval **var_ptr;

		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, *param TSRMLS_CC);
		var_ptr = get_zval_ptr_ptr(&opline->result, EX(Ts), &free_res, BP_VAR_W);
		if (PZVAL_IS_REF(*param)) {
			zend_assign_to_variable_reference(var_ptr, param TSRMLS_CC);
		} else {
			zend_receive(var_ptr, *param TSRMLS_CC);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_NEW_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *object_zval;
	zend_function *constructor;

	if (EX_T(opline->op1.u.var).class_entry->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
		char *class_type;

		if (EX_T(opline->op1.u.var).class_entry->ce_flags & ZEND_ACC_INTERFACE) {
			class_type = "interface";
		} else {
			class_type = "abstract class";
		}
		zend_error_noreturn(E_ERROR, "Cannot instantiate %s %s", class_type,  EX_T(opline->op1.u.var).class_entry->name);
	}
	ALLOC_ZVAL(object_zval);
	object_init_ex(object_zval, EX_T(opline->op1.u.var).class_entry);
	INIT_PZVAL(object_zval);

	constructor = Z_OBJ_HT_P(object_zval)->get_constructor(object_zval TSRMLS_CC);

	if (constructor == NULL) {
		if (RETURN_VALUE_USED(opline)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
			EX_T(opline->result.u.var).var.ptr = object_zval;
		} else {
			zval_ptr_dtor(&object_zval);
		}
		ZEND_VM_JMP(EX(op_array)->opcodes + opline->op2.u.opline_num);
	} else {
		SELECTIVE_PZVAL_LOCK(object_zval, &opline->result);
		EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
		EX_T(opline->result.u.var).var.ptr = object_zval;

		zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), opline);

		/* We are not handling overloaded classes right now */
		EX(object) = object_zval;
		EX(fbc) = constructor;

		ZEND_VM_NEXT_OPCODE();
	}
}

static int ZEND_BEGIN_SILENCE_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = EG(error_reporting);
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_LONG;  /* shouldn't be necessary */
	if (EX(old_error_reporting) == NULL) {
		EX(old_error_reporting) = &EX_T(opline->result.u.var).tmp_var;
	}

	if (EG(error_reporting)) {
		zend_alter_ini_entry_ex("error_reporting", sizeof("error_reporting"), "0", 1, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME, 1);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_error_noreturn(E_ERROR, "Cannot call abstract method %s::%s()", EG(scope)->name, EX(op_array)->function_name);
	ZEND_VM_NEXT_OPCODE(); /* Never reached */
}

static int ZEND_EXT_STMT_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	if (!EG(no_extensions)) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_statement_handler, EX(op_array) TSRMLS_CC);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	if (!EG(no_extensions)) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_fcall_begin_handler, EX(op_array) TSRMLS_CC);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_EXT_FCALL_END_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	if (!EG(no_extensions)) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_fcall_end_handler, EX(op_array) TSRMLS_CC);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DECLARE_CLASS_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	EX_T(opline->result.u.var).class_entry = do_bind_class(opline, EG(class_table), 0 TSRMLS_CC);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	EX_T(opline->result.u.var).class_entry = do_bind_inherited_class(opline, EG(class_table), EX_T(opline->extended_value).class_entry, 0 TSRMLS_CC);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DECLARE_FUNCTION_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	do_bind_function(EX(opline), EG(function_table), 0);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_EXT_NOP_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_NOP_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_INTERFACE_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_class_entry *ce = EX_T(opline->op1.u.var).class_entry;
	zend_class_entry *iface = EX_T(opline->op2.u.var).class_entry;

	if (!(iface->ce_flags & ZEND_ACC_INTERFACE)) {
		zend_error_noreturn(E_ERROR, "%s cannot implement %s - it is not an interface", ce->name, iface->name);
	}

	zend_do_implement_interface(ce, iface TSRMLS_CC);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_HANDLE_EXCEPTION_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_uint op_num = EG(opline_before_exception)-EG(active_op_array)->opcodes;
	int i;
	zend_uint catch_op_num;
	int catched = 0;
	zval **stack_zval_pp;
	zval restored_error_reporting;

	stack_zval_pp = (zval **) EG(argument_stack).top_element - 1;
	while (*stack_zval_pp != NULL) {
		zval_ptr_dtor(stack_zval_pp);
		EG(argument_stack).top_element--;
		EG(argument_stack).top--;
		stack_zval_pp--;
	}

	for (i=0; i<EG(active_op_array)->last_try_catch; i++) {
		if (EG(active_op_array)->try_catch_array[i].try_op > op_num) {
			/* further blocks will not be relevant... */
			break;
		}
		if (op_num >= EG(active_op_array)->try_catch_array[i].try_op
			&& op_num < EG(active_op_array)->try_catch_array[i].catch_op) {
			catch_op_num = EX(op_array)->try_catch_array[i].catch_op;
			catched = 1;
		}
	}

	while (EX(fbc)) {
		zend_op *ctor_opline = (zend_op*)zend_ptr_stack_pop(&EG(arg_types_stack));

		if (EX(object)) {
			if (ctor_opline && RETURN_VALUE_USED(ctor_opline)) {
				EX(object)->refcount--;
			}
			zval_ptr_dtor(&EX(object));
		}
		zend_ptr_stack_2_pop(&EG(arg_types_stack), (void**)&EX(object), (void**)&EX(fbc));
	}

	for (i=0; i<EX(op_array)->last_brk_cont; i++) {
		if (EX(op_array)->brk_cont_array[i].start < 0) {
			continue;
		} else if (EX(op_array)->brk_cont_array[i].start > op_num) {
			/* further blocks will not be relevant... */
			break;
		} else if (op_num < EX(op_array)->brk_cont_array[i].brk) {
			if (!catched ||
			    catch_op_num >= EX(op_array)->brk_cont_array[i].brk) {
				zend_op *brk_opline = &EX(op_array)->opcodes[EX(op_array)->brk_cont_array[i].brk];

				switch (brk_opline->opcode) {
					case ZEND_SWITCH_FREE:
						zend_switch_free(brk_opline, EX(Ts) TSRMLS_CC);
						break;
					case ZEND_FREE:
						zendi_zval_dtor(EX_T(brk_opline->op1.u.var).tmp_var);
						break;
				}
			}
		}
	}

	/* restore previous error_reporting value */
	if (!EG(error_reporting) && EX(old_error_reporting) != NULL && Z_LVAL_P(EX(old_error_reporting)) != 0) {
		Z_TYPE(restored_error_reporting) = IS_LONG;
		Z_LVAL(restored_error_reporting) = Z_LVAL_P(EX(old_error_reporting));
		convert_to_string(&restored_error_reporting);
		zend_alter_ini_entry_ex("error_reporting", sizeof("error_reporting"), Z_STRVAL(restored_error_reporting), Z_STRLEN(restored_error_reporting), ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME, 1);
		zendi_zval_dtor(restored_error_reporting);
	}
	EX(old_error_reporting) = NULL;

	if (!catched) {
 		ZEND_VM_RETURN_FROM_EXECUTE_LOOP();
 	} else {
		ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[catch_op_num]);
 		ZEND_VM_CONTINUE();
 	}
}

static int ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_verify_abstract_class(EX_T(EX(opline)->op1.u.var).class_entry TSRMLS_CC);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_USER_OPCODE_SPEC_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	int ret = zend_user_opcode_handlers[EX(opline)->opcode](ZEND_OPCODE_HANDLER_ARGS_PASSTHRU_INTERNAL);

	switch (ret) {
		case ZEND_USER_OPCODE_CONTINUE:
			ZEND_VM_CONTINUE();
		case ZEND_USER_OPCODE_RETURN:
			ZEND_VM_RETURN();
		case ZEND_USER_OPCODE_DISPATCH:
			ZEND_VM_DISPATCH(EX(opline)->opcode, EX(opline));
		default:
			ZEND_VM_DISPATCH(ret & 0xff, EX(opline));
	}
}

static int ZEND_FETCH_CLASS_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *class_name;



	if (IS_CONST == IS_UNUSED) {
		EX_T(opline->result.u.var).class_entry = zend_fetch_class(NULL, 0, opline->extended_value TSRMLS_CC);
		ZEND_VM_NEXT_OPCODE();
	}

	class_name = &opline->op2.u.constant;

	switch (Z_TYPE_P(class_name)) {
		case IS_OBJECT:
			EX_T(opline->result.u.var).class_entry = Z_OBJCE_P(class_name);
			break;
		case IS_STRING:
			EX_T(opline->result.u.var).class_entry = zend_fetch_class(Z_STRVAL_P(class_name), Z_STRLEN_P(class_name), opline->extended_value TSRMLS_CC);
			break;
		default:
			zend_error_noreturn(E_ERROR, "Class name must be a valid object or a string");
			break;
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_STATIC_METHOD_CALL_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	zend_class_entry *ce;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	ce = EX_T(opline->op1.u.var).class_entry;
	if(IS_CONST != IS_UNUSED) {
		char *function_name_strval;
		int function_name_strlen;
		zend_bool is_const = (IS_CONST == IS_CONST);


		if (is_const) {
			function_name_strval = Z_STRVAL(opline->op2.u.constant);
			function_name_strlen = Z_STRLEN(opline->op2.u.constant);
		} else {
			function_name = &opline->op2.u.constant;

			if (Z_TYPE_P(function_name) != IS_STRING) {
				zend_error_noreturn(E_ERROR, "Function name must be a string");
			}
			function_name_strval = zend_str_tolower_dup(function_name->value.str.val, function_name->value.str.len);
			function_name_strlen = function_name->value.str.len;
		}

		EX(fbc) = zend_std_get_static_method(ce, function_name_strval, function_name_strlen TSRMLS_CC);

		if (!is_const) {
			efree(function_name_strval);

		}
	} else {
		if(!ce->constructor) {
			zend_error_noreturn(E_ERROR, "Can not call constructor");
		}
		if (EG(This) && Z_OBJCE_P(EG(This)) != ce->constructor->common.scope && (ce->constructor->common.fn_flags & ZEND_ACC_PRIVATE)) {
			zend_error(E_COMPILE_ERROR, "Cannot call private %s::__construct()", ce->name);
		}
		EX(fbc) = ce->constructor;
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (IS_CONST != IS_UNUSED &&
		    EG(This) &&
		    Z_OBJ_HT_P(EG(This))->get_class_entry &&
		    !instanceof_function(Z_OBJCE_P(EG(This)), ce TSRMLS_CC)) {
		    /* We are calling method of the other (incompatible) class,
		       but passing $this. This is done for compatibility with php-4. */
			int severity;
			char *verb;
			if (EX(fbc)->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
				severity = E_STRICT;
				verb = "should not";
			} else {
				/* An internal function assumes $this is present and won't check that. So PHP would crash by allowing the call. */
				severity = E_ERROR;
				verb = "cannot";
			}
			zend_error(severity, "Non-static method %s::%s() %s be called statically, assuming $this from incompatible context", EX(fbc)->common.scope->name, EX(fbc)->common.function_name, verb);

		}
		if ((EX(object) = EG(This))) {
			EX(object)->refcount++;
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_FCALL_BY_NAME_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	zend_function *function;
	char *function_name_strval, *lcname;
	int function_name_strlen;


	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	if (IS_CONST == IS_CONST) {
		function_name_strval = opline->op2.u.constant.value.str.val;
		function_name_strlen = opline->op2.u.constant.value.str.len;
	} else {
		function_name = &opline->op2.u.constant;

		if (Z_TYPE_P(function_name) != IS_STRING) {
			zend_error_noreturn(E_ERROR, "Function name must be a string");
		}
		function_name_strval = function_name->value.str.val;
		function_name_strlen = function_name->value.str.len;
	}

	lcname = zend_str_tolower_dup(function_name_strval, function_name_strlen);
	if (zend_hash_find(EG(function_table), lcname, function_name_strlen+1, (void **) &function)==FAILURE) {
		efree(lcname);
		zend_error_noreturn(E_ERROR, "Call to undefined function %s()", function_name_strval);
	}

	efree(lcname);
	if (IS_CONST != IS_CONST) {

	}

	EX(object) = NULL;

	EX(fbc) = function;

	ZEND_VM_NEXT_OPCODE();
}


static int ZEND_RECV_INIT_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval **param, *assignment_value;
	zend_uint arg_num = Z_LVAL(opline->op1.u.constant);
	zend_free_op free_res;

	if (zend_ptr_stack_get_arg(arg_num, (void **) &param TSRMLS_CC)==FAILURE) {
		if (Z_TYPE(opline->op2.u.constant) == IS_CONSTANT || Z_TYPE(opline->op2.u.constant)==IS_CONSTANT_ARRAY) {
			zval *default_value;

			ALLOC_ZVAL(default_value);
			*default_value = opline->op2.u.constant;
			default_value->refcount=1;
			zval_update_constant(&default_value, 0 TSRMLS_CC);
			default_value->refcount=0;
			default_value->is_ref=0;
			param = &default_value;
			assignment_value = default_value;
		} else {
			param = NULL;
			assignment_value = &opline->op2.u.constant;
		}
		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, assignment_value TSRMLS_CC);
		zend_assign_to_variable(NULL, &opline->result, NULL, assignment_value, IS_VAR, EX(Ts) TSRMLS_CC);
	} else {
		zval **var_ptr = get_zval_ptr_ptr(&opline->result, EX(Ts), &free_res, BP_VAR_W);

		assignment_value = *param;
		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, assignment_value TSRMLS_CC);
		if (PZVAL_IS_REF(assignment_value)) {
			zend_assign_to_variable_reference(var_ptr, param TSRMLS_CC);
		} else {
			zend_receive(var_ptr, assignment_value TSRMLS_CC);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BRK_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zend_brk_cont_element *el;

	el = zend_brk_cont(&opline->op2.u.constant, opline->op1.u.opline_num,
	                   EX(op_array), EX(Ts) TSRMLS_CC);

	ZEND_VM_JMP(EX(op_array)->opcodes + el->brk);
}

static int ZEND_CONT_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zend_brk_cont_element *el;

	el = zend_brk_cont(&opline->op2.u.constant, opline->op1.u.opline_num,
	                   EX(op_array), EX(Ts) TSRMLS_CC);

	ZEND_VM_JMP(EX(op_array)->opcodes + el->cont);
}

static int ZEND_FETCH_CLASS_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *class_name;
	zend_free_op free_op2;


	if (IS_TMP_VAR == IS_UNUSED) {
		EX_T(opline->result.u.var).class_entry = zend_fetch_class(NULL, 0, opline->extended_value TSRMLS_CC);
		ZEND_VM_NEXT_OPCODE();
	}

	class_name = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	switch (Z_TYPE_P(class_name)) {
		case IS_OBJECT:
			EX_T(opline->result.u.var).class_entry = Z_OBJCE_P(class_name);
			break;
		case IS_STRING:
			EX_T(opline->result.u.var).class_entry = zend_fetch_class(Z_STRVAL_P(class_name), Z_STRLEN_P(class_name), opline->extended_value TSRMLS_CC);
			break;
		default:
			zend_error_noreturn(E_ERROR, "Class name must be a valid object or a string");
			break;
	}

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_STATIC_METHOD_CALL_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	zend_class_entry *ce;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	ce = EX_T(opline->op1.u.var).class_entry;
	if(IS_TMP_VAR != IS_UNUSED) {
		char *function_name_strval;
		int function_name_strlen;
		zend_bool is_const = (IS_TMP_VAR == IS_CONST);
		zend_free_op free_op2;

		if (is_const) {
			function_name_strval = Z_STRVAL(opline->op2.u.constant);
			function_name_strlen = Z_STRLEN(opline->op2.u.constant);
		} else {
			function_name = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

			if (Z_TYPE_P(function_name) != IS_STRING) {
				zend_error_noreturn(E_ERROR, "Function name must be a string");
			}
			function_name_strval = zend_str_tolower_dup(function_name->value.str.val, function_name->value.str.len);
			function_name_strlen = function_name->value.str.len;
		}

		EX(fbc) = zend_std_get_static_method(ce, function_name_strval, function_name_strlen TSRMLS_CC);

		if (!is_const) {
			efree(function_name_strval);
			zval_dtor(free_op2.var);
		}
	} else {
		if(!ce->constructor) {
			zend_error_noreturn(E_ERROR, "Can not call constructor");
		}
		if (EG(This) && Z_OBJCE_P(EG(This)) != ce->constructor->common.scope && (ce->constructor->common.fn_flags & ZEND_ACC_PRIVATE)) {
			zend_error(E_COMPILE_ERROR, "Cannot call private %s::__construct()", ce->name);
		}
		EX(fbc) = ce->constructor;
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (IS_TMP_VAR != IS_UNUSED &&
		    EG(This) &&
		    Z_OBJ_HT_P(EG(This))->get_class_entry &&
		    !instanceof_function(Z_OBJCE_P(EG(This)), ce TSRMLS_CC)) {
		    /* We are calling method of the other (incompatible) class,
		       but passing $this. This is done for compatibility with php-4. */
			int severity;
			char *verb;
			if (EX(fbc)->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
				severity = E_STRICT;
				verb = "should not";
			} else {
				/* An internal function assumes $this is present and won't check that. So PHP would crash by allowing the call. */
				severity = E_ERROR;
				verb = "cannot";
			}
			zend_error(severity, "Non-static method %s::%s() %s be called statically, assuming $this from incompatible context", EX(fbc)->common.scope->name, EX(fbc)->common.function_name, verb);

		}
		if ((EX(object) = EG(This))) {
			EX(object)->refcount++;
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_FCALL_BY_NAME_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	zend_function *function;
	char *function_name_strval, *lcname;
	int function_name_strlen;
	zend_free_op free_op2;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	if (IS_TMP_VAR == IS_CONST) {
		function_name_strval = opline->op2.u.constant.value.str.val;
		function_name_strlen = opline->op2.u.constant.value.str.len;
	} else {
		function_name = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (Z_TYPE_P(function_name) != IS_STRING) {
			zend_error_noreturn(E_ERROR, "Function name must be a string");
		}
		function_name_strval = function_name->value.str.val;
		function_name_strlen = function_name->value.str.len;
	}

	lcname = zend_str_tolower_dup(function_name_strval, function_name_strlen);
	if (zend_hash_find(EG(function_table), lcname, function_name_strlen+1, (void **) &function)==FAILURE) {
		efree(lcname);
		zend_error_noreturn(E_ERROR, "Call to undefined function %s()", function_name_strval);
	}

	efree(lcname);
	if (IS_TMP_VAR != IS_CONST) {
		zval_dtor(free_op2.var);
	}

	EX(object) = NULL;

	EX(fbc) = function;

	ZEND_VM_NEXT_OPCODE();
}


static int ZEND_BRK_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zend_brk_cont_element *el;

	el = zend_brk_cont(_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC), opline->op1.u.opline_num,
	                   EX(op_array), EX(Ts) TSRMLS_CC);
	zval_dtor(free_op2.var);
	ZEND_VM_JMP(EX(op_array)->opcodes + el->brk);
}

static int ZEND_CONT_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zend_brk_cont_element *el;

	el = zend_brk_cont(_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC), opline->op1.u.opline_num,
	                   EX(op_array), EX(Ts) TSRMLS_CC);
	zval_dtor(free_op2.var);
	ZEND_VM_JMP(EX(op_array)->opcodes + el->cont);
}

static int ZEND_FETCH_CLASS_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *class_name;
	zend_free_op free_op2;


	if (IS_VAR == IS_UNUSED) {
		EX_T(opline->result.u.var).class_entry = zend_fetch_class(NULL, 0, opline->extended_value TSRMLS_CC);
		ZEND_VM_NEXT_OPCODE();
	}

	class_name = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	switch (Z_TYPE_P(class_name)) {
		case IS_OBJECT:
			EX_T(opline->result.u.var).class_entry = Z_OBJCE_P(class_name);
			break;
		case IS_STRING:
			EX_T(opline->result.u.var).class_entry = zend_fetch_class(Z_STRVAL_P(class_name), Z_STRLEN_P(class_name), opline->extended_value TSRMLS_CC);
			break;
		default:
			zend_error_noreturn(E_ERROR, "Class name must be a valid object or a string");
			break;
	}

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_STATIC_METHOD_CALL_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	zend_class_entry *ce;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	ce = EX_T(opline->op1.u.var).class_entry;
	if(IS_VAR != IS_UNUSED) {
		char *function_name_strval;
		int function_name_strlen;
		zend_bool is_const = (IS_VAR == IS_CONST);
		zend_free_op free_op2;

		if (is_const) {
			function_name_strval = Z_STRVAL(opline->op2.u.constant);
			function_name_strlen = Z_STRLEN(opline->op2.u.constant);
		} else {
			function_name = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

			if (Z_TYPE_P(function_name) != IS_STRING) {
				zend_error_noreturn(E_ERROR, "Function name must be a string");
			}
			function_name_strval = zend_str_tolower_dup(function_name->value.str.val, function_name->value.str.len);
			function_name_strlen = function_name->value.str.len;
		}

		EX(fbc) = zend_std_get_static_method(ce, function_name_strval, function_name_strlen TSRMLS_CC);

		if (!is_const) {
			efree(function_name_strval);
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		}
	} else {
		if(!ce->constructor) {
			zend_error_noreturn(E_ERROR, "Can not call constructor");
		}
		if (EG(This) && Z_OBJCE_P(EG(This)) != ce->constructor->common.scope && (ce->constructor->common.fn_flags & ZEND_ACC_PRIVATE)) {
			zend_error(E_COMPILE_ERROR, "Cannot call private %s::__construct()", ce->name);
		}
		EX(fbc) = ce->constructor;
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (IS_VAR != IS_UNUSED &&
		    EG(This) &&
		    Z_OBJ_HT_P(EG(This))->get_class_entry &&
		    !instanceof_function(Z_OBJCE_P(EG(This)), ce TSRMLS_CC)) {
		    /* We are calling method of the other (incompatible) class,
		       but passing $this. This is done for compatibility with php-4. */
			int severity;
			char *verb;
			if (EX(fbc)->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
				severity = E_STRICT;
				verb = "should not";
			} else {
				/* An internal function assumes $this is present and won't check that. So PHP would crash by allowing the call. */
				severity = E_ERROR;
				verb = "cannot";
			}
			zend_error(severity, "Non-static method %s::%s() %s be called statically, assuming $this from incompatible context", EX(fbc)->common.scope->name, EX(fbc)->common.function_name, verb);

		}
		if ((EX(object) = EG(This))) {
			EX(object)->refcount++;
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_FCALL_BY_NAME_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	zend_function *function;
	char *function_name_strval, *lcname;
	int function_name_strlen;
	zend_free_op free_op2;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	if (IS_VAR == IS_CONST) {
		function_name_strval = opline->op2.u.constant.value.str.val;
		function_name_strlen = opline->op2.u.constant.value.str.len;
	} else {
		function_name = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (Z_TYPE_P(function_name) != IS_STRING) {
			zend_error_noreturn(E_ERROR, "Function name must be a string");
		}
		function_name_strval = function_name->value.str.val;
		function_name_strlen = function_name->value.str.len;
	}

	lcname = zend_str_tolower_dup(function_name_strval, function_name_strlen);
	if (zend_hash_find(EG(function_table), lcname, function_name_strlen+1, (void **) &function)==FAILURE) {
		efree(lcname);
		zend_error_noreturn(E_ERROR, "Call to undefined function %s()", function_name_strval);
	}

	efree(lcname);
	if (IS_VAR != IS_CONST) {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}

	EX(object) = NULL;

	EX(fbc) = function;

	ZEND_VM_NEXT_OPCODE();
}


static int ZEND_BRK_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zend_brk_cont_element *el;

	el = zend_brk_cont(_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC), opline->op1.u.opline_num,
	                   EX(op_array), EX(Ts) TSRMLS_CC);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_JMP(EX(op_array)->opcodes + el->brk);
}

static int ZEND_CONT_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zend_brk_cont_element *el;

	el = zend_brk_cont(_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC), opline->op1.u.opline_num,
	                   EX(op_array), EX(Ts) TSRMLS_CC);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_JMP(EX(op_array)->opcodes + el->cont);
}

static int ZEND_FETCH_CLASS_SPEC_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *class_name;



	if (IS_UNUSED == IS_UNUSED) {
		EX_T(opline->result.u.var).class_entry = zend_fetch_class(NULL, 0, opline->extended_value TSRMLS_CC);
		ZEND_VM_NEXT_OPCODE();
	}

	class_name = NULL;

	switch (Z_TYPE_P(class_name)) {
		case IS_OBJECT:
			EX_T(opline->result.u.var).class_entry = Z_OBJCE_P(class_name);
			break;
		case IS_STRING:
			EX_T(opline->result.u.var).class_entry = zend_fetch_class(Z_STRVAL_P(class_name), Z_STRLEN_P(class_name), opline->extended_value TSRMLS_CC);
			break;
		default:
			zend_error_noreturn(E_ERROR, "Class name must be a valid object or a string");
			break;
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_STATIC_METHOD_CALL_SPEC_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	zend_class_entry *ce;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	ce = EX_T(opline->op1.u.var).class_entry;
	if(IS_UNUSED != IS_UNUSED) {
		char *function_name_strval;
		int function_name_strlen;
		zend_bool is_const = (IS_UNUSED == IS_CONST);


		if (is_const) {
			function_name_strval = Z_STRVAL(opline->op2.u.constant);
			function_name_strlen = Z_STRLEN(opline->op2.u.constant);
		} else {
			function_name = NULL;

			if (Z_TYPE_P(function_name) != IS_STRING) {
				zend_error_noreturn(E_ERROR, "Function name must be a string");
			}
			function_name_strval = zend_str_tolower_dup(function_name->value.str.val, function_name->value.str.len);
			function_name_strlen = function_name->value.str.len;
		}

		EX(fbc) = zend_std_get_static_method(ce, function_name_strval, function_name_strlen TSRMLS_CC);

		if (!is_const) {
			efree(function_name_strval);

		}
	} else {
		if(!ce->constructor) {
			zend_error_noreturn(E_ERROR, "Can not call constructor");
		}
		if (EG(This) && Z_OBJCE_P(EG(This)) != ce->constructor->common.scope && (ce->constructor->common.fn_flags & ZEND_ACC_PRIVATE)) {
			zend_error(E_COMPILE_ERROR, "Cannot call private %s::__construct()", ce->name);
		}
		EX(fbc) = ce->constructor;
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (IS_UNUSED != IS_UNUSED &&
		    EG(This) &&
		    Z_OBJ_HT_P(EG(This))->get_class_entry &&
		    !instanceof_function(Z_OBJCE_P(EG(This)), ce TSRMLS_CC)) {
		    /* We are calling method of the other (incompatible) class,
		       but passing $this. This is done for compatibility with php-4. */
			int severity;
			char *verb;
			if (EX(fbc)->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
				severity = E_STRICT;
				verb = "should not";
			} else {
				/* An internal function assumes $this is present and won't check that. So PHP would crash by allowing the call. */
				severity = E_ERROR;
				verb = "cannot";
			}
			zend_error(severity, "Non-static method %s::%s() %s be called statically, assuming $this from incompatible context", EX(fbc)->common.scope->name, EX(fbc)->common.function_name, verb);

		}
		if ((EX(object) = EG(This))) {
			EX(object)->refcount++;
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_CLASS_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *class_name;



	if (IS_CV == IS_UNUSED) {
		EX_T(opline->result.u.var).class_entry = zend_fetch_class(NULL, 0, opline->extended_value TSRMLS_CC);
		ZEND_VM_NEXT_OPCODE();
	}

	class_name = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	switch (Z_TYPE_P(class_name)) {
		case IS_OBJECT:
			EX_T(opline->result.u.var).class_entry = Z_OBJCE_P(class_name);
			break;
		case IS_STRING:
			EX_T(opline->result.u.var).class_entry = zend_fetch_class(Z_STRVAL_P(class_name), Z_STRLEN_P(class_name), opline->extended_value TSRMLS_CC);
			break;
		default:
			zend_error_noreturn(E_ERROR, "Class name must be a valid object or a string");
			break;
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_STATIC_METHOD_CALL_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	zend_class_entry *ce;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	ce = EX_T(opline->op1.u.var).class_entry;
	if(IS_CV != IS_UNUSED) {
		char *function_name_strval;
		int function_name_strlen;
		zend_bool is_const = (IS_CV == IS_CONST);


		if (is_const) {
			function_name_strval = Z_STRVAL(opline->op2.u.constant);
			function_name_strlen = Z_STRLEN(opline->op2.u.constant);
		} else {
			function_name = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

			if (Z_TYPE_P(function_name) != IS_STRING) {
				zend_error_noreturn(E_ERROR, "Function name must be a string");
			}
			function_name_strval = zend_str_tolower_dup(function_name->value.str.val, function_name->value.str.len);
			function_name_strlen = function_name->value.str.len;
		}

		EX(fbc) = zend_std_get_static_method(ce, function_name_strval, function_name_strlen TSRMLS_CC);

		if (!is_const) {
			efree(function_name_strval);

		}
	} else {
		if(!ce->constructor) {
			zend_error_noreturn(E_ERROR, "Can not call constructor");
		}
		if (EG(This) && Z_OBJCE_P(EG(This)) != ce->constructor->common.scope && (ce->constructor->common.fn_flags & ZEND_ACC_PRIVATE)) {
			zend_error(E_COMPILE_ERROR, "Cannot call private %s::__construct()", ce->name);
		}
		EX(fbc) = ce->constructor;
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (IS_CV != IS_UNUSED &&
		    EG(This) &&
		    Z_OBJ_HT_P(EG(This))->get_class_entry &&
		    !instanceof_function(Z_OBJCE_P(EG(This)), ce TSRMLS_CC)) {
		    /* We are calling method of the other (incompatible) class,
		       but passing $this. This is done for compatibility with php-4. */
			int severity;
			char *verb;
			if (EX(fbc)->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
				severity = E_STRICT;
				verb = "should not";
			} else {
				/* An internal function assumes $this is present and won't check that. So PHP would crash by allowing the call. */
				severity = E_ERROR;
				verb = "cannot";
			}
			zend_error(severity, "Non-static method %s::%s() %s be called statically, assuming $this from incompatible context", EX(fbc)->common.scope->name, EX(fbc)->common.function_name, verb);

		}
		if ((EX(object) = EG(This))) {
			EX(object)->refcount++;
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_FCALL_BY_NAME_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	zend_function *function;
	char *function_name_strval, *lcname;
	int function_name_strlen;


	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	if (IS_CV == IS_CONST) {
		function_name_strval = opline->op2.u.constant.value.str.val;
		function_name_strlen = opline->op2.u.constant.value.str.len;
	} else {
		function_name = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

		if (Z_TYPE_P(function_name) != IS_STRING) {
			zend_error_noreturn(E_ERROR, "Function name must be a string");
		}
		function_name_strval = function_name->value.str.val;
		function_name_strlen = function_name->value.str.len;
	}

	lcname = zend_str_tolower_dup(function_name_strval, function_name_strlen);
	if (zend_hash_find(EG(function_table), lcname, function_name_strlen+1, (void **) &function)==FAILURE) {
		efree(lcname);
		zend_error_noreturn(E_ERROR, "Call to undefined function %s()", function_name_strval);
	}

	efree(lcname);
	if (IS_CV != IS_CONST) {

	}

	EX(object) = NULL;

	EX(fbc) = function;

	ZEND_VM_NEXT_OPCODE();
}


static int ZEND_BRK_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zend_brk_cont_element *el;

	el = zend_brk_cont(_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC), opline->op1.u.opline_num,
	                   EX(op_array), EX(Ts) TSRMLS_CC);

	ZEND_VM_JMP(EX(op_array)->opcodes + el->brk);
}

static int ZEND_CONT_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zend_brk_cont_element *el;

	el = zend_brk_cont(_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC), opline->op1.u.opline_num,
	                   EX(op_array), EX(Ts) TSRMLS_CC);

	ZEND_VM_JMP(EX(op_array)->opcodes + el->cont);
}

static int ZEND_BW_NOT_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	bitwise_not_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant TSRMLS_CC);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_NOT_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	boolean_not_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant TSRMLS_CC);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ECHO_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval z_copy;
	zval *z = &opline->op1.u.constant;

	if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get_method != NULL &&
		zend_std_cast_object_tostring(z, &z_copy, IS_STRING TSRMLS_CC) == SUCCESS) {
		zend_print_variable(&z_copy);
		zval_dtor(&z_copy);
	} else {
		zend_print_variable(z);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRINT_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 1;
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_LONG;

	return ZEND_ECHO_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_fetch_var_address_helper_SPEC_CONST(int type, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *varname = &opline->op1.u.constant;
	zval **retval;
	zval tmp_varname;
	HashTable *target_symbol_table;

 	if (Z_TYPE_P(varname) != IS_STRING) {
		tmp_varname = *varname;
		zval_copy_ctor(&tmp_varname);
		convert_to_string(&tmp_varname);
		varname = &tmp_varname;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		retval = zend_std_get_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname), 0 TSRMLS_CC);
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), type, varname TSRMLS_CC);
/*
		if (!target_symbol_table) {
			ZEND_VM_NEXT_OPCODE();
		}
*/
		if (zend_hash_find(target_symbol_table, varname->value.str.val, varname->value.str.len+1, (void **) &retval) == FAILURE) {
			switch (type) {
				case BP_VAR_R:
				case BP_VAR_UNSET:
					zend_error(E_NOTICE,"Undefined variable: %s", Z_STRVAL_P(varname));
					/* break missing intentionally */
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval_ptr);
					break;
				case BP_VAR_RW:
					zend_error(E_NOTICE,"Undefined variable: %s", Z_STRVAL_P(varname));
					/* break missing intentionally */
				case BP_VAR_W: {
						zval *new_zval = &EG(uninitialized_zval);

						new_zval->refcount++;
						zend_hash_update(target_symbol_table, varname->value.str.val, varname->value.str.len+1, &new_zval, sizeof(zval *), (void **) &retval);
					}
					break;
				EMPTY_SWITCH_DEFAULT_CASE()
			}
		}
		switch (opline->op2.u.EA.type) {
			case ZEND_FETCH_GLOBAL:
				if (IS_CONST != IS_TMP_VAR) {

				}
				break;
			case ZEND_FETCH_LOCAL:

				break;
			case ZEND_FETCH_STATIC:
				zval_update_constant(retval, (void*) 1 TSRMLS_CC);
				break;
			case ZEND_FETCH_GLOBAL_LOCK:
				if (IS_CONST == IS_VAR && !free_op1.var) {
					PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
				}
				break;
		}
	}


	if (varname == &tmp_varname) {
		zval_dtor(varname);
	}
	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = retval;
		PZVAL_LOCK(*retval);
		switch (type) {
			case BP_VAR_R:
			case BP_VAR_IS:
				AI_USE_PTR(EX_T(opline->result.u.var).var);
				break;
			case BP_VAR_UNSET: {
				zend_free_op free_res;

				PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
				if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
					SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
				}
				PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
				FREE_OP_VAR_PTR(free_res);
				break;
			}
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_R_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_CONST(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_W_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_CONST(BP_VAR_W, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_RW_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_CONST(BP_VAR_RW, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_FUNC_ARG_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_CONST(ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), EX(opline)->extended_value)?BP_VAR_W:BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_UNSET_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_CONST(BP_VAR_UNSET, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_IS_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_CONST(BP_VAR_IS, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_JMPZ_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	int ret = i_zend_is_true(&opline->op1.u.constant);

	if (!ret) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_JMPNZ_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	int ret = i_zend_is_true(&opline->op1.u.constant);

	if (ret) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_JMPZNZ_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	int retval = i_zend_is_true(&opline->op1.u.constant);

	if (retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp on true to %d\n", opline->extended_value);
#endif
		ZEND_VM_JMP(&EX(op_array)->opcodes[opline->extended_value]);
	} else {
#if DEBUG_ZEND>=2
		printf("Conditional jmp on false to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(&EX(op_array)->opcodes[opline->op2.u.opline_num]);
	}
}

static int ZEND_JMPZ_EX_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	int retval = i_zend_is_true(&opline->op1.u.constant);

	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = retval;
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;
	if (!retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_JMPNZ_EX_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	int retval = i_zend_is_true(&opline->op1.u.constant);

	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = retval;
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;
	if (retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DO_FCALL_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *fname = &opline->op1.u.constant;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	if (zend_hash_find(EG(function_table), fname->value.str.val, fname->value.str.len+1, (void **) &EX(function_state).function)==FAILURE) {
		zend_error_noreturn(E_ERROR, "Call to undefined function %s()", fname->value.str.val);
	}
	EX(object) = NULL;

	return zend_do_fcall_common_helper_SPEC(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_RETURN_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *retval_ptr;
	zval **retval_ptr_ptr;


	if (EG(active_op_array)->return_reference == ZEND_RETURN_REF) {

		if (IS_CONST == IS_CONST || IS_CONST == IS_TMP_VAR) {
			/* Not supposed to happen, but we'll allow it */
			zend_error(E_NOTICE, "Only variable references should be returned by reference");
			goto return_by_value;
		}

		retval_ptr_ptr = NULL;

		if (!retval_ptr_ptr) {
			zend_error_noreturn(E_ERROR, "Cannot return string offsets by reference");
		}

		if (IS_CONST == IS_VAR && !(*retval_ptr_ptr)->is_ref) {
			if (opline->extended_value == ZEND_RETURNS_FUNCTION &&
			    EX_T(opline->op1.u.var).var.fcall_returned_reference) {
			} else if (EX_T(opline->op1.u.var).var.ptr_ptr == &EX_T(opline->op1.u.var).var.ptr) {
				if (IS_CONST == IS_VAR && !0) {
					PZVAL_LOCK(*retval_ptr_ptr); /* undo the effect of get_zval_ptr_ptr() */
				}
				zend_error(E_NOTICE, "Only variable references should be returned by reference");
				goto return_by_value;
			}
		}

		SEPARATE_ZVAL_TO_MAKE_IS_REF(retval_ptr_ptr);
		(*retval_ptr_ptr)->refcount++;

		(*EG(return_value_ptr_ptr)) = (*retval_ptr_ptr);
	} else {
return_by_value:

		retval_ptr = &opline->op1.u.constant;

		if (EG(ze1_compatibility_mode) && Z_TYPE_P(retval_ptr) == IS_OBJECT) {
			zval *ret;
			char *class_name;
			zend_uint class_name_len;
			int dup;

			ALLOC_ZVAL(ret);
			INIT_PZVAL_COPY(ret, retval_ptr);
			dup = zend_get_object_classname(retval_ptr, &class_name, &class_name_len TSRMLS_CC);
			if (Z_OBJ_HT_P(retval_ptr)->clone_obj == NULL) {
				zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object of class %s",  class_name);
			}
			zend_error(E_STRICT, "Implicit cloning object of class '%s' because of 'zend.ze1_compatibility_mode'", class_name);
			ret->value.obj = Z_OBJ_HT_P(retval_ptr)->clone_obj(retval_ptr TSRMLS_CC);
			*EG(return_value_ptr_ptr) = ret;
			if (!dup) {
				efree(class_name);
			}
		} else if (!0) { /* Not a temp var */
			if (EG(active_op_array)->return_reference == ZEND_RETURN_REF ||
			    (PZVAL_IS_REF(retval_ptr) && retval_ptr->refcount > 0)) {
				zval *ret;

				ALLOC_ZVAL(ret);
				INIT_PZVAL_COPY(ret, retval_ptr);
				zval_copy_ctor(ret);
				*EG(return_value_ptr_ptr) = ret;
			} else {
				*EG(return_value_ptr_ptr) = retval_ptr;
				retval_ptr->refcount++;
			}
		} else {
			zval *ret;

			ALLOC_ZVAL(ret);
			INIT_PZVAL_COPY(ret, retval_ptr);
			*EG(return_value_ptr_ptr) = ret;
		}
	}

	ZEND_VM_RETURN_FROM_EXECUTE_LOOP();
}

static int ZEND_THROW_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *value;
	zval *exception;


	value = &opline->op1.u.constant;

	if (Z_TYPE_P(value) != IS_OBJECT) {
		zend_error_noreturn(E_ERROR, "Can only throw objects");
	}
	/* Not sure if a complete copy is what we want here */
	ALLOC_ZVAL(exception);
	INIT_PZVAL_COPY(exception, value);
	if (!0) {
		zval_copy_ctor(exception);
	}

	zend_throw_exception_object(exception TSRMLS_CC);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SEND_VAL_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	if (opline->extended_value==ZEND_DO_FCALL_BY_NAME
		&& ARG_MUST_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
			zend_error_noreturn(E_ERROR, "Cannot pass parameter %d by reference", opline->op2.u.opline_num);
	}
	{
		zval *valptr;
		zval *value;


		value = &opline->op1.u.constant;

		ALLOC_ZVAL(valptr);
		INIT_PZVAL_COPY(valptr, value);
		if (!0) {
			zval_copy_ctor(valptr);
		}
		zend_ptr_stack_push(&EG(argument_stack), valptr);

	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	/* PHP 3.0 returned "" for false and 1 for true, here we use 0 and 1 for now */
	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = i_zend_is_true(&opline->op1.u.constant);
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CLONE_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *obj = &opline->op1.u.constant;
	zend_class_entry *ce;
	zend_function *clone;
	zend_object_clone_obj_t clone_call;

	if (!obj || Z_TYPE_P(obj) != IS_OBJECT) {
		zend_error_noreturn(E_ERROR, "__clone method called on non-object");
		EX_T(opline->result.u.var).var.ptr = EG(error_zval_ptr);
		EX_T(opline->result.u.var).var.ptr->refcount++;

		ZEND_VM_NEXT_OPCODE();
	}

	ce = Z_OBJCE_P(obj);
	clone = ce ? ce->clone : NULL;
	clone_call =  Z_OBJ_HT_P(obj)->clone_obj;
	if (!clone_call) {
		if (ce) {
			zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object of class %s", ce->name);
		} else {
			zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object");
		}
		EX_T(opline->result.u.var).var.ptr = EG(error_zval_ptr);
		EX_T(opline->result.u.var).var.ptr->refcount++;
	}

	if (ce && clone) {
		if (clone->op_array.fn_flags & ZEND_ACC_PRIVATE) {
			/* Ensure that if we're calling a private function, we're allowed to do so.
			 */
			if (ce != EG(scope)) {
				zend_error_noreturn(E_ERROR, "Call to private %s::__clone() from context '%s'", ce->name, EG(scope) ? EG(scope)->name : "");
			}
		} else if ((clone->common.fn_flags & ZEND_ACC_PROTECTED)) {
			/* Ensure that if we're calling a protected function, we're allowed to do so.
			 */
			if (!zend_check_protected(clone->common.scope, EG(scope))) {
				zend_error_noreturn(E_ERROR, "Call to protected %s::__clone() from context '%s'", ce->name, EG(scope) ? EG(scope)->name : "");
			}
		}
	}

	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	if (!EG(exception)) {
		ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
		Z_OBJVAL_P(EX_T(opline->result.u.var).var.ptr) = clone_call(obj TSRMLS_CC);
		Z_TYPE_P(EX_T(opline->result.u.var).var.ptr) = IS_OBJECT;
		EX_T(opline->result.u.var).var.ptr->refcount=1;
		EX_T(opline->result.u.var).var.ptr->is_ref=1;
		if (!RETURN_VALUE_USED(opline) || EG(exception)) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CAST_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *expr = &opline->op1.u.constant;
	zval *result = &EX_T(opline->result.u.var).tmp_var;

	if (opline->extended_value != IS_STRING) {
		*result = *expr;
		if (!0) {
			zendi_zval_copy_ctor(*result);
		}
	}
	switch (opline->extended_value) {
		case IS_NULL:
			convert_to_null(result);
			break;
		case IS_BOOL:
			convert_to_boolean(result);
			break;
		case IS_LONG:
			convert_to_long(result);
			break;
		case IS_DOUBLE:
			convert_to_double(result);
			break;
		case IS_STRING: {
			zval var_copy;
			int use_copy;

			zend_make_printable_zval(expr, &var_copy, &use_copy);
			if (use_copy) {
				*result = var_copy;
				if (0) {

				}
			} else {
				*result = *expr;
				if (!0) {
					zendi_zval_copy_ctor(*result);
				}
			}
			break;
		}
		case IS_ARRAY:
			convert_to_array(result);
			break;
		case IS_OBJECT:
			convert_to_object(result);
			break;
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INCLUDE_OR_EVAL_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op_array *new_op_array=NULL;
	zval **original_return_value = EG(return_value_ptr_ptr);
	int return_value_used;

	zval *inc_filename = &opline->op1.u.constant;
	zval tmp_inc_filename;
	zend_bool failure_retval=0;

	if (inc_filename->type!=IS_STRING) {
		tmp_inc_filename = *inc_filename;
		zval_copy_ctor(&tmp_inc_filename);
		convert_to_string(&tmp_inc_filename);
		inc_filename = &tmp_inc_filename;
	}

	return_value_used = RETURN_VALUE_USED(opline);

	switch (Z_LVAL(opline->op2.u.constant)) {
		case ZEND_INCLUDE_ONCE:
		case ZEND_REQUIRE_ONCE: {
				zend_file_handle file_handle;

				if (IS_ABSOLUTE_PATH(Z_STRVAL_P(inc_filename), Z_STRLEN_P(inc_filename))) {
					cwd_state state;

					state.cwd_length = 0;
					state.cwd = malloc(1);
					state.cwd[0] = 0;

					failure_retval = (!virtual_file_ex(&state, Z_STRVAL_P(inc_filename), NULL, 1) &&
						zend_hash_exists(&EG(included_files), state.cwd, state.cwd_length+1));

					free(state.cwd);
				}

				if (failure_retval) {
					/* do nothing */
				} else if (SUCCESS == zend_stream_open(Z_STRVAL_P(inc_filename), &file_handle TSRMLS_CC)) {

					if (!file_handle.opened_path) {
						file_handle.opened_path = estrndup(Z_STRVAL_P(inc_filename), Z_STRLEN_P(inc_filename));
					}

					if (zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path, strlen(file_handle.opened_path)+1)==SUCCESS) {
						new_op_array = zend_compile_file(&file_handle, (Z_LVAL(opline->op2.u.constant)==ZEND_INCLUDE_ONCE?ZEND_INCLUDE:ZEND_REQUIRE) TSRMLS_CC);
						zend_destroy_file_handle(&file_handle TSRMLS_CC);
					} else {
						zend_file_handle_dtor(&file_handle);
						failure_retval=1;
					}
				} else {
					if (Z_LVAL(opline->op2.u.constant)==ZEND_INCLUDE_ONCE) {
						zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, Z_STRVAL_P(inc_filename));
					} else {
						zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, Z_STRVAL_P(inc_filename));
					}
				}
			}
			break;
		case ZEND_INCLUDE:
		case ZEND_REQUIRE:
			new_op_array = compile_filename(Z_LVAL(opline->op2.u.constant), inc_filename TSRMLS_CC);
			break;
		case ZEND_EVAL: {
				char *eval_desc = zend_make_compiled_string_description("eval()'d code" TSRMLS_CC);

				new_op_array = zend_compile_string(inc_filename, eval_desc TSRMLS_CC);
				efree(eval_desc);
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	if (inc_filename==&tmp_inc_filename) {
		zval_dtor(&tmp_inc_filename);
	}
	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	if (new_op_array) {
		zval *saved_object;
		zend_function *saved_function;

		EG(return_value_ptr_ptr) = EX_T(opline->result.u.var).var.ptr_ptr;
		EG(active_op_array) = new_op_array;
		EX_T(opline->result.u.var).var.ptr = NULL;

		saved_object = EX(object);
		saved_function = EX(function_state).function;

		EX(function_state).function = (zend_function *) new_op_array;
		EX(object) = NULL;

		zend_execute(new_op_array TSRMLS_CC);

		EX(function_state).function = saved_function;
		EX(object) = saved_object;

		if (!return_value_used) {
			if (EX_T(opline->result.u.var).var.ptr) {
				zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
			}
		} else { /* return value is used */
			if (!EX_T(opline->result.u.var).var.ptr) { /* there was no return statement */
				ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
				INIT_PZVAL(EX_T(opline->result.u.var).var.ptr);
				Z_LVAL_P(EX_T(opline->result.u.var).var.ptr) = 1;
				Z_TYPE_P(EX_T(opline->result.u.var).var.ptr) = IS_BOOL;
			}
		}

		EG(opline_ptr) = &EX(opline);
		EG(active_op_array) = EX(op_array);
		EG(function_state_ptr) = &EX(function_state);
		destroy_op_array(new_op_array TSRMLS_CC);
		efree(new_op_array);
		if (EG(exception)) {
			zend_throw_exception_internal(NULL TSRMLS_CC);
		}
	} else {
		if (return_value_used) {
			ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
			INIT_ZVAL(*EX_T(opline->result.u.var).var.ptr);
			Z_LVAL_P(EX_T(opline->result.u.var).var.ptr) = failure_retval;
			Z_TYPE_P(EX_T(opline->result.u.var).var.ptr) = IS_BOOL;
		}
	}

	EG(return_value_ptr_ptr) = original_return_value;
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_UNSET_VAR_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval tmp, *varname;
	HashTable *target_symbol_table;


	varname = &opline->op1.u.constant;

	if (Z_TYPE_P(varname) != IS_STRING) {
		tmp = *varname;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		varname = &tmp;
	} else if (IS_CONST == IS_CV || IS_CONST == IS_VAR) {
		varname->refcount++;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		zend_std_unset_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname) TSRMLS_CC);
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), BP_VAR_IS, varname TSRMLS_CC);
		if (zend_hash_del(target_symbol_table, varname->value.str.val, varname->value.str.len+1) == SUCCESS) {
			zend_execute_data *ex = execute_data;
			ulong hash_value = zend_inline_hash_func(varname->value.str.val, varname->value.str.len+1);

			do {
				int i;

				if (ex->op_array) {
					for (i = 0; i < ex->op_array->last_var; i++) {
						if (ex->op_array->vars[i].hash_value == hash_value &&
							ex->op_array->vars[i].name_len == varname->value.str.len &&
							!memcmp(ex->op_array->vars[i].name, varname->value.str.val, varname->value.str.len)) {
							ex->CVs[i] = NULL;
							break;
						}
					}
				}
				ex = ex->prev_execute_data;
			} while (ex && ex->symbol_table == target_symbol_table);
		}
	}

	if (varname == &tmp) {
		zval_dtor(&tmp);
	} else if (IS_CONST == IS_CV || IS_CONST == IS_VAR) {
		zval_ptr_dtor(&varname);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FE_RESET_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *array_ptr, **array_ptr_ptr;
	HashTable *fe_ht;
	zend_object_iterator *iter = NULL;
	zend_class_entry *ce = NULL;
	zend_bool is_empty = 0;

	if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {
		array_ptr_ptr = NULL;
		if (array_ptr_ptr == NULL || array_ptr_ptr == &EG(uninitialized_zval_ptr)) {
			ALLOC_INIT_ZVAL(array_ptr);
		} else if (Z_TYPE_PP(array_ptr_ptr) == IS_OBJECT) {
			if(Z_OBJ_HT_PP(array_ptr_ptr)->get_class_entry == NULL) {
				zend_error(E_WARNING, "foreach() can not iterate over objects without PHP class");
				ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
			}

			ce = Z_OBJCE_PP(array_ptr_ptr);
			if (!ce || ce->get_iterator == NULL) {
				SEPARATE_ZVAL_IF_NOT_REF(array_ptr_ptr);
				(*array_ptr_ptr)->refcount++;
			}
			array_ptr = *array_ptr_ptr;
		} else {
			if (Z_TYPE_PP(array_ptr_ptr) == IS_ARRAY) {
				SEPARATE_ZVAL_IF_NOT_REF(array_ptr_ptr);
				if (opline->extended_value & ZEND_FE_FETCH_BYREF) {
					(*array_ptr_ptr)->is_ref = 1;
				}
			}
			array_ptr = *array_ptr_ptr;
			array_ptr->refcount++;
		}
	} else {
		array_ptr = &opline->op1.u.constant;
		if (0) { /* IS_TMP_VAR */
			zval *tmp;

			ALLOC_ZVAL(tmp);
			INIT_PZVAL_COPY(tmp, array_ptr);
			array_ptr = tmp;
		} else if (Z_TYPE_P(array_ptr) == IS_OBJECT) {
			ce = Z_OBJCE_P(array_ptr);
			if (!ce || !ce->get_iterator) {
				array_ptr->refcount++;
			}
		} else {
			if ((IS_CONST == IS_CV || IS_CONST == IS_VAR) &&
			    !array_ptr->is_ref &&
			    array_ptr->refcount > 1) {
				zval *tmp;

				ALLOC_ZVAL(tmp);
				INIT_PZVAL_COPY(tmp, array_ptr);
				zval_copy_ctor(tmp);
				array_ptr = tmp;
			} else {
				array_ptr->refcount++;
			}
		}
	}

	if (IS_CONST != IS_TMP_VAR && ce && ce->get_iterator) {
		iter = ce->get_iterator(ce, array_ptr, opline->extended_value & ZEND_FE_RESET_REFERENCE TSRMLS_CC);

		if (iter && !EG(exception)) {
			array_ptr = zend_iterator_wrap(iter TSRMLS_CC);
		} else {
			if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {

			} else {

			}
			if (!EG(exception)) {
				zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "Object of type %s did not create an Iterator", ce->name);
			}
			zend_throw_exception_internal(NULL TSRMLS_CC);
			ZEND_VM_NEXT_OPCODE();
		}
	}

	PZVAL_LOCK(array_ptr);
	EX_T(opline->result.u.var).var.ptr = array_ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;

	if (iter) {
		iter->index = 0;
		if (iter->funcs->rewind) {
			iter->funcs->rewind(iter TSRMLS_CC);
			if (EG(exception)) {
				array_ptr->refcount--;
				zval_ptr_dtor(&array_ptr);
				if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {

				} else {

				}
				ZEND_VM_NEXT_OPCODE();
			}
		}
		is_empty = iter->funcs->valid(iter TSRMLS_CC) != SUCCESS;
		if (EG(exception)) {
			array_ptr->refcount--;
			zval_ptr_dtor(&array_ptr);
			if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {

			} else {

			}
			ZEND_VM_NEXT_OPCODE();
		}
		iter->index = -1; /* will be set to 0 before using next handler */
	} else if ((fe_ht = HASH_OF(array_ptr)) != NULL) {
		zend_hash_internal_pointer_reset(fe_ht);
		if (ce) {
			zend_object *zobj = zend_objects_get_address(array_ptr TSRMLS_CC);
			while (zend_hash_has_more_elements(fe_ht) == SUCCESS) {
				char *str_key;
				uint str_key_len;
				ulong int_key;
				zend_uchar key_type;

				key_type = zend_hash_get_current_key_ex(fe_ht, &str_key, &str_key_len, &int_key, 0, NULL);
				if (key_type != HASH_KEY_NON_EXISTANT &&
					(key_type == HASH_KEY_IS_LONG ||
				     zend_check_property_access(zobj, str_key, str_key_len-1 TSRMLS_CC) == SUCCESS)) {
					break;
				}
				zend_hash_move_forward(fe_ht);
			}
		}
		is_empty = zend_hash_has_more_elements(fe_ht) != SUCCESS;
		zend_hash_get_pointer(fe_ht, &EX_T(opline->result.u.var).fe.fe_pos);
	} else {
		zend_error(E_WARNING, "Invalid argument supplied for foreach()");
		is_empty = 1;
	}

	if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {

	} else {

	}
	if (is_empty) {
		ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
	} else {
		ZEND_VM_NEXT_OPCODE();
	}
}

static int ZEND_ISSET_ISEMPTY_VAR_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval tmp, *varname = &opline->op1.u.constant;
	zval **value;
	zend_bool isset = 1;
	HashTable *target_symbol_table;

	if (Z_TYPE_P(varname) != IS_STRING) {
		tmp = *varname;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		varname = &tmp;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		value = zend_std_get_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname), 1 TSRMLS_CC);
		if (!value) {
			isset = 0;
		}
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), BP_VAR_IS, varname TSRMLS_CC);
		if (zend_hash_find(target_symbol_table, varname->value.str.val, varname->value.str.len+1, (void **) &value) == FAILURE) {
			isset = 0;
		}
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			if (isset && Z_TYPE_PP(value) == IS_NULL) {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 0;
			} else {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = isset;
			}
			break;
		case ZEND_ISEMPTY:
			if (!isset || !i_zend_is_true(*value)) {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 1;
			} else {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 0;
			}
			break;
	}

	if (varname == &tmp) {
		zval_dtor(&tmp);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_EXIT_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
#if 0 || (IS_CONST != IS_UNUSED)
	zend_op *opline = EX(opline);
	if (IS_CONST != IS_UNUSED) {

		zval *ptr = &opline->op1.u.constant;

		if (Z_TYPE_P(ptr) == IS_LONG) {
			EG(exit_status) = Z_LVAL_P(ptr);
		} else {
			zend_print_variable(ptr);
		}

	}
#endif
	zend_bailout();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_QM_ASSIGN_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *value = &opline->op1.u.constant;

	EX_T(opline->result.u.var).tmp_var = *value;
	if (!0) {
		zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_TICKS_SPEC_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	if (++EG(ticks_count)>=Z_LVAL(opline->op1.u.constant)) {
		EG(ticks_count)=0;
		if (zend_ticks_function) {
			zend_ticks_function(Z_LVAL(opline->op1.u.constant));
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	add_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SUB_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	sub_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MUL_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	mul_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DIV_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	div_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MOD_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	mod_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SL_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SR_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CONCAT_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	concat_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_IDENTICAL_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_IDENTICAL_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_EQUAL_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_EQUAL_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_OR_EQUAL_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_OR_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_AND_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_XOR_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_XOR_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_TMP_VAR_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *container = &opline->op1.u.constant;

	if (Z_TYPE_P(container) != IS_ARRAY) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		}
	} else {

		zval *dim = &opline->op2.u.constant;

		EX_T(opline->result.u.var).var.ptr_ptr = zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), dim, BP_VAR_R TSRMLS_CC);
		SELECTIVE_PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &opline->result);

	}
	AI_USE_PTR(EX_T(opline->result.u.var).var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CASE_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;


	if (IS_CONST==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 &opline->op1.u.constant,
				 &opline->op2.u.constant TSRMLS_CC);

	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */

		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_CONSTANT_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_class_entry *ce = NULL;
	zval **value;

	if (IS_CONST == IS_UNUSED) {
/* This seems to be a reminant of namespaces
		if (EG(scope)) {
			ce = EG(scope);
			if (zend_hash_find(&ce->constants_table, Z_STRVAL(opline->op2.u.constant), Z_STRLEN(opline->op2.u.constant)+1, (void **) &value) == SUCCESS) {
				zval_update_constant(value, (void *) 1 TSRMLS_CC);
				EX_T(opline->result.u.var).tmp_var = **value;
				zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
				ZEND_VM_NEXT_OPCODE();
			}
		}
*/
		if (!zend_get_constant(opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len, &EX_T(opline->result.u.var).tmp_var TSRMLS_CC)) {
			zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'",
						opline->op2.u.constant.value.str.val,
						opline->op2.u.constant.value.str.val);
			EX_T(opline->result.u.var).tmp_var = opline->op2.u.constant;
			zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
		}
		ZEND_VM_NEXT_OPCODE();
	}

	ce = EX_T(opline->op1.u.var).class_entry;

	if (zend_hash_find(&ce->constants_table, opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len+1, (void **) &value) == SUCCESS) {
		zend_class_entry *old_scope = EG(scope);

		EG(scope) = ce;
		zval_update_constant(value, (void *) 1 TSRMLS_CC);
		EG(scope) = old_scope;
		EX_T(opline->result.u.var).tmp_var = **value;
		zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
	} else {
		zend_error_noreturn(E_ERROR, "Undefined class constant '%s'", opline->op2.u.constant.value.str.val);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=&opline->op2.u.constant;

#if 0 || IS_CONST == IS_VAR || IS_CONST == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=NULL;
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=&opline->op1.u.constant;
	}
#else
	expr_ptr=&opline->op1.u.constant;
#endif

	if (0) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_CONST == IS_VAR || IS_CONST == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}

	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {

	} else {

	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_CONST == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_CONST != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_CONST_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_ADD_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	add_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SUB_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	sub_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MUL_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	mul_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DIV_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	div_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MOD_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	mod_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SL_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SR_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CONCAT_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	concat_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_IDENTICAL_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_IDENTICAL_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_EQUAL_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_EQUAL_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_OR_EQUAL_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_OR_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_AND_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_XOR_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_XOR_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CASE_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;
	zend_free_op free_op2;

	if (IS_CONST==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 &opline->op1.u.constant,
				 _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */

		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

#if 0 || IS_CONST == IS_VAR || IS_CONST == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=NULL;
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=&opline->op1.u.constant;
	}
#else
	expr_ptr=&opline->op1.u.constant;
#endif

	if (0) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_CONST == IS_VAR || IS_CONST == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}
		zval_dtor(free_op2.var);
	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {

	} else {

	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_CONST == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_CONST != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_CONST_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_ADD_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	add_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SUB_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	sub_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MUL_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	mul_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DIV_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	div_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MOD_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	mod_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SL_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SR_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CONCAT_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	concat_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_IDENTICAL_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_IDENTICAL_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_EQUAL_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_EQUAL_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_OR_EQUAL_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_OR_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_AND_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_XOR_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_XOR_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CASE_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;
	zend_free_op free_op2;

	if (IS_CONST==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 &opline->op1.u.constant,
				 _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */

		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

#if 0 || IS_CONST == IS_VAR || IS_CONST == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=NULL;
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=&opline->op1.u.constant;
	}
#else
	expr_ptr=&opline->op1.u.constant;
#endif

	if (0) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_CONST == IS_VAR || IS_CONST == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {

	} else {

	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_CONST == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_CONST != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_CONST_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_CONST_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=NULL;

#if 0 || IS_CONST == IS_VAR || IS_CONST == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=NULL;
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=&opline->op1.u.constant;
	}
#else
	expr_ptr=&opline->op1.u.constant;
#endif

	if (0) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_CONST == IS_VAR || IS_CONST == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}

	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {

	} else {

	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_CONST_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_CONST == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_CONST != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_CONST_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_ADD_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	add_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SUB_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	sub_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MUL_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	mul_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DIV_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	div_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MOD_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	mod_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SL_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SR_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CONCAT_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	concat_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_IDENTICAL_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_IDENTICAL_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_EQUAL_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_EQUAL_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_OR_EQUAL_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_OR_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_AND_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_XOR_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_XOR_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		&opline->op1.u.constant,
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CASE_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;


	if (IS_CONST==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 &opline->op1.u.constant,
				 _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);

	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */

		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

#if 0 || IS_CONST == IS_VAR || IS_CONST == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=NULL;
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=&opline->op1.u.constant;
	}
#else
	expr_ptr=&opline->op1.u.constant;
#endif

	if (0) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_CONST == IS_VAR || IS_CONST == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}

	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {

	} else {

	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_CONST == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_CONST != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_CONST_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_BW_NOT_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	bitwise_not_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_NOT_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	boolean_not_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ECHO_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval z_copy;
	zval *z = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get_method != NULL &&
		zend_std_cast_object_tostring(z, &z_copy, IS_STRING TSRMLS_CC) == SUCCESS) {
		zend_print_variable(&z_copy);
		zval_dtor(&z_copy);
	} else {
		zend_print_variable(z);
	}

	zval_dtor(free_op1.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRINT_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 1;
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_LONG;

	return ZEND_ECHO_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_fetch_var_address_helper_SPEC_TMP(int type, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *varname = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval **retval;
	zval tmp_varname;
	HashTable *target_symbol_table;

 	if (Z_TYPE_P(varname) != IS_STRING) {
		tmp_varname = *varname;
		zval_copy_ctor(&tmp_varname);
		convert_to_string(&tmp_varname);
		varname = &tmp_varname;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		retval = zend_std_get_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname), 0 TSRMLS_CC);
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), type, varname TSRMLS_CC);
/*
		if (!target_symbol_table) {
			ZEND_VM_NEXT_OPCODE();
		}
*/
		if (zend_hash_find(target_symbol_table, varname->value.str.val, varname->value.str.len+1, (void **) &retval) == FAILURE) {
			switch (type) {
				case BP_VAR_R:
				case BP_VAR_UNSET:
					zend_error(E_NOTICE,"Undefined variable: %s", Z_STRVAL_P(varname));
					/* break missing intentionally */
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval_ptr);
					break;
				case BP_VAR_RW:
					zend_error(E_NOTICE,"Undefined variable: %s", Z_STRVAL_P(varname));
					/* break missing intentionally */
				case BP_VAR_W: {
						zval *new_zval = &EG(uninitialized_zval);

						new_zval->refcount++;
						zend_hash_update(target_symbol_table, varname->value.str.val, varname->value.str.len+1, &new_zval, sizeof(zval *), (void **) &retval);
					}
					break;
				EMPTY_SWITCH_DEFAULT_CASE()
			}
		}
		switch (opline->op2.u.EA.type) {
			case ZEND_FETCH_GLOBAL:
				if (IS_TMP_VAR != IS_TMP_VAR) {
					zval_dtor(free_op1.var);
				}
				break;
			case ZEND_FETCH_LOCAL:
				zval_dtor(free_op1.var);
				break;
			case ZEND_FETCH_STATIC:
				zval_update_constant(retval, (void*) 1 TSRMLS_CC);
				break;
			case ZEND_FETCH_GLOBAL_LOCK:
				if (IS_TMP_VAR == IS_VAR && !free_op1.var) {
					PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
				}
				break;
		}
	}


	if (varname == &tmp_varname) {
		zval_dtor(varname);
	}
	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = retval;
		PZVAL_LOCK(*retval);
		switch (type) {
			case BP_VAR_R:
			case BP_VAR_IS:
				AI_USE_PTR(EX_T(opline->result.u.var).var);
				break;
			case BP_VAR_UNSET: {
				zend_free_op free_res;

				PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
				if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
					SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
				}
				PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
				FREE_OP_VAR_PTR(free_res);
				break;
			}
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_R_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_TMP(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_W_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_TMP(BP_VAR_W, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_RW_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_TMP(BP_VAR_RW, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_FUNC_ARG_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_TMP(ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), EX(opline)->extended_value)?BP_VAR_W:BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_UNSET_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_TMP(BP_VAR_UNSET, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_IS_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_TMP(BP_VAR_IS, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_JMPZ_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int ret = i_zend_is_true(_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC));

	zval_dtor(free_op1.var);
	if (!ret) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_JMPNZ_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int ret = i_zend_is_true(_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC));

	zval_dtor(free_op1.var);
	if (ret) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_JMPZNZ_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int retval = i_zend_is_true(_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC));

	zval_dtor(free_op1.var);
	if (retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp on true to %d\n", opline->extended_value);
#endif
		ZEND_VM_JMP(&EX(op_array)->opcodes[opline->extended_value]);
	} else {
#if DEBUG_ZEND>=2
		printf("Conditional jmp on false to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(&EX(op_array)->opcodes[opline->op2.u.opline_num]);
	}
}

static int ZEND_JMPZ_EX_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int retval = i_zend_is_true(_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC));

	zval_dtor(free_op1.var);
	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = retval;
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;
	if (!retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_JMPNZ_EX_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int retval = i_zend_is_true(_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC));

	zval_dtor(free_op1.var);
	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = retval;
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;
	if (retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FREE_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zendi_zval_dtor(EX_T(EX(opline)->op1.u.var).tmp_var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_RETURN_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *retval_ptr;
	zval **retval_ptr_ptr;
	zend_free_op free_op1;

	if (EG(active_op_array)->return_reference == ZEND_RETURN_REF) {

		if (IS_TMP_VAR == IS_CONST || IS_TMP_VAR == IS_TMP_VAR) {
			/* Not supposed to happen, but we'll allow it */
			zend_error(E_NOTICE, "Only variable references should be returned by reference");
			goto return_by_value;
		}

		retval_ptr_ptr = NULL;

		if (!retval_ptr_ptr) {
			zend_error_noreturn(E_ERROR, "Cannot return string offsets by reference");
		}

		if (IS_TMP_VAR == IS_VAR && !(*retval_ptr_ptr)->is_ref) {
			if (opline->extended_value == ZEND_RETURNS_FUNCTION &&
			    EX_T(opline->op1.u.var).var.fcall_returned_reference) {
			} else if (EX_T(opline->op1.u.var).var.ptr_ptr == &EX_T(opline->op1.u.var).var.ptr) {
				if (IS_TMP_VAR == IS_VAR && !1) {
					PZVAL_LOCK(*retval_ptr_ptr); /* undo the effect of get_zval_ptr_ptr() */
				}
				zend_error(E_NOTICE, "Only variable references should be returned by reference");
				goto return_by_value;
			}
		}

		SEPARATE_ZVAL_TO_MAKE_IS_REF(retval_ptr_ptr);
		(*retval_ptr_ptr)->refcount++;

		(*EG(return_value_ptr_ptr)) = (*retval_ptr_ptr);
	} else {
return_by_value:

		retval_ptr = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

		if (EG(ze1_compatibility_mode) && Z_TYPE_P(retval_ptr) == IS_OBJECT) {
			zval *ret;
			char *class_name;
			zend_uint class_name_len;
			int dup;

			ALLOC_ZVAL(ret);
			INIT_PZVAL_COPY(ret, retval_ptr);
			dup = zend_get_object_classname(retval_ptr, &class_name, &class_name_len TSRMLS_CC);
			if (Z_OBJ_HT_P(retval_ptr)->clone_obj == NULL) {
				zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object of class %s",  class_name);
			}
			zend_error(E_STRICT, "Implicit cloning object of class '%s' because of 'zend.ze1_compatibility_mode'", class_name);
			ret->value.obj = Z_OBJ_HT_P(retval_ptr)->clone_obj(retval_ptr TSRMLS_CC);
			*EG(return_value_ptr_ptr) = ret;
			if (!dup) {
				efree(class_name);
			}
		} else if (!1) { /* Not a temp var */
			if (EG(active_op_array)->return_reference == ZEND_RETURN_REF ||
			    (PZVAL_IS_REF(retval_ptr) && retval_ptr->refcount > 0)) {
				zval *ret;

				ALLOC_ZVAL(ret);
				INIT_PZVAL_COPY(ret, retval_ptr);
				zval_copy_ctor(ret);
				*EG(return_value_ptr_ptr) = ret;
			} else {
				*EG(return_value_ptr_ptr) = retval_ptr;
				retval_ptr->refcount++;
			}
		} else {
			zval *ret;

			ALLOC_ZVAL(ret);
			INIT_PZVAL_COPY(ret, retval_ptr);
			*EG(return_value_ptr_ptr) = ret;
		}
	}

	ZEND_VM_RETURN_FROM_EXECUTE_LOOP();
}

static int ZEND_THROW_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *value;
	zval *exception;
	zend_free_op free_op1;

	value = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (Z_TYPE_P(value) != IS_OBJECT) {
		zend_error_noreturn(E_ERROR, "Can only throw objects");
	}
	/* Not sure if a complete copy is what we want here */
	ALLOC_ZVAL(exception);
	INIT_PZVAL_COPY(exception, value);
	if (!1) {
		zval_copy_ctor(exception);
	}

	zend_throw_exception_object(exception TSRMLS_CC);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SEND_VAL_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	if (opline->extended_value==ZEND_DO_FCALL_BY_NAME
		&& ARG_MUST_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
			zend_error_noreturn(E_ERROR, "Cannot pass parameter %d by reference", opline->op2.u.opline_num);
	}
	{
		zval *valptr;
		zval *value;
		zend_free_op free_op1;

		value = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

		ALLOC_ZVAL(valptr);
		INIT_PZVAL_COPY(valptr, value);
		if (!1) {
			zval_copy_ctor(valptr);
		}
		zend_ptr_stack_push(&EG(argument_stack), valptr);

	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	/* PHP 3.0 returned "" for false and 1 for true, here we use 0 and 1 for now */
	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = i_zend_is_true(_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC));
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SWITCH_FREE_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_switch_free(EX(opline), EX(Ts) TSRMLS_CC);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CLONE_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *obj = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zend_class_entry *ce;
	zend_function *clone;
	zend_object_clone_obj_t clone_call;

	if (!obj || Z_TYPE_P(obj) != IS_OBJECT) {
		zend_error_noreturn(E_ERROR, "__clone method called on non-object");
		EX_T(opline->result.u.var).var.ptr = EG(error_zval_ptr);
		EX_T(opline->result.u.var).var.ptr->refcount++;

		ZEND_VM_NEXT_OPCODE();
	}

	ce = Z_OBJCE_P(obj);
	clone = ce ? ce->clone : NULL;
	clone_call =  Z_OBJ_HT_P(obj)->clone_obj;
	if (!clone_call) {
		if (ce) {
			zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object of class %s", ce->name);
		} else {
			zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object");
		}
		EX_T(opline->result.u.var).var.ptr = EG(error_zval_ptr);
		EX_T(opline->result.u.var).var.ptr->refcount++;
	}

	if (ce && clone) {
		if (clone->op_array.fn_flags & ZEND_ACC_PRIVATE) {
			/* Ensure that if we're calling a private function, we're allowed to do so.
			 */
			if (ce != EG(scope)) {
				zend_error_noreturn(E_ERROR, "Call to private %s::__clone() from context '%s'", ce->name, EG(scope) ? EG(scope)->name : "");
			}
		} else if ((clone->common.fn_flags & ZEND_ACC_PROTECTED)) {
			/* Ensure that if we're calling a protected function, we're allowed to do so.
			 */
			if (!zend_check_protected(clone->common.scope, EG(scope))) {
				zend_error_noreturn(E_ERROR, "Call to protected %s::__clone() from context '%s'", ce->name, EG(scope) ? EG(scope)->name : "");
			}
		}
	}

	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	if (!EG(exception)) {
		ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
		Z_OBJVAL_P(EX_T(opline->result.u.var).var.ptr) = clone_call(obj TSRMLS_CC);
		Z_TYPE_P(EX_T(opline->result.u.var).var.ptr) = IS_OBJECT;
		EX_T(opline->result.u.var).var.ptr->refcount=1;
		EX_T(opline->result.u.var).var.ptr->is_ref=1;
		if (!RETURN_VALUE_USED(opline) || EG(exception)) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CAST_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *expr = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *result = &EX_T(opline->result.u.var).tmp_var;

	if (opline->extended_value != IS_STRING) {
		*result = *expr;
		if (!1) {
			zendi_zval_copy_ctor(*result);
		}
	}
	switch (opline->extended_value) {
		case IS_NULL:
			convert_to_null(result);
			break;
		case IS_BOOL:
			convert_to_boolean(result);
			break;
		case IS_LONG:
			convert_to_long(result);
			break;
		case IS_DOUBLE:
			convert_to_double(result);
			break;
		case IS_STRING: {
			zval var_copy;
			int use_copy;

			zend_make_printable_zval(expr, &var_copy, &use_copy);
			if (use_copy) {
				*result = var_copy;
				if (1) {
					zval_dtor(free_op1.var);
				}
			} else {
				*result = *expr;
				if (!1) {
					zendi_zval_copy_ctor(*result);
				}
			}
			break;
		}
		case IS_ARRAY:
			convert_to_array(result);
			break;
		case IS_OBJECT:
			convert_to_object(result);
			break;
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INCLUDE_OR_EVAL_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op_array *new_op_array=NULL;
	zval **original_return_value = EG(return_value_ptr_ptr);
	int return_value_used;
	zend_free_op free_op1;
	zval *inc_filename = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval tmp_inc_filename;
	zend_bool failure_retval=0;

	if (inc_filename->type!=IS_STRING) {
		tmp_inc_filename = *inc_filename;
		zval_copy_ctor(&tmp_inc_filename);
		convert_to_string(&tmp_inc_filename);
		inc_filename = &tmp_inc_filename;
	}

	return_value_used = RETURN_VALUE_USED(opline);

	switch (Z_LVAL(opline->op2.u.constant)) {
		case ZEND_INCLUDE_ONCE:
		case ZEND_REQUIRE_ONCE: {
				zend_file_handle file_handle;

				if (IS_ABSOLUTE_PATH(Z_STRVAL_P(inc_filename), Z_STRLEN_P(inc_filename))) {
					cwd_state state;

					state.cwd_length = 0;
					state.cwd = malloc(1);
					state.cwd[0] = 0;

					failure_retval = (!virtual_file_ex(&state, Z_STRVAL_P(inc_filename), NULL, 1) &&
						zend_hash_exists(&EG(included_files), state.cwd, state.cwd_length+1));

					free(state.cwd);
				}

				if (failure_retval) {
					/* do nothing */
				} else if (SUCCESS == zend_stream_open(Z_STRVAL_P(inc_filename), &file_handle TSRMLS_CC)) {

					if (!file_handle.opened_path) {
						file_handle.opened_path = estrndup(Z_STRVAL_P(inc_filename), Z_STRLEN_P(inc_filename));
					}

					if (zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path, strlen(file_handle.opened_path)+1)==SUCCESS) {
						new_op_array = zend_compile_file(&file_handle, (Z_LVAL(opline->op2.u.constant)==ZEND_INCLUDE_ONCE?ZEND_INCLUDE:ZEND_REQUIRE) TSRMLS_CC);
						zend_destroy_file_handle(&file_handle TSRMLS_CC);
					} else {
						zend_file_handle_dtor(&file_handle);
						failure_retval=1;
					}
				} else {
					if (Z_LVAL(opline->op2.u.constant)==ZEND_INCLUDE_ONCE) {
						zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, Z_STRVAL_P(inc_filename));
					} else {
						zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, Z_STRVAL_P(inc_filename));
					}
				}
			}
			break;
		case ZEND_INCLUDE:
		case ZEND_REQUIRE:
			new_op_array = compile_filename(Z_LVAL(opline->op2.u.constant), inc_filename TSRMLS_CC);
			break;
		case ZEND_EVAL: {
				char *eval_desc = zend_make_compiled_string_description("eval()'d code" TSRMLS_CC);

				new_op_array = zend_compile_string(inc_filename, eval_desc TSRMLS_CC);
				efree(eval_desc);
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	if (inc_filename==&tmp_inc_filename) {
		zval_dtor(&tmp_inc_filename);
	}
	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	if (new_op_array) {
		zval *saved_object;
		zend_function *saved_function;

		EG(return_value_ptr_ptr) = EX_T(opline->result.u.var).var.ptr_ptr;
		EG(active_op_array) = new_op_array;
		EX_T(opline->result.u.var).var.ptr = NULL;

		saved_object = EX(object);
		saved_function = EX(function_state).function;

		EX(function_state).function = (zend_function *) new_op_array;
		EX(object) = NULL;

		zend_execute(new_op_array TSRMLS_CC);

		EX(function_state).function = saved_function;
		EX(object) = saved_object;

		if (!return_value_used) {
			if (EX_T(opline->result.u.var).var.ptr) {
				zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
			}
		} else { /* return value is used */
			if (!EX_T(opline->result.u.var).var.ptr) { /* there was no return statement */
				ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
				INIT_PZVAL(EX_T(opline->result.u.var).var.ptr);
				Z_LVAL_P(EX_T(opline->result.u.var).var.ptr) = 1;
				Z_TYPE_P(EX_T(opline->result.u.var).var.ptr) = IS_BOOL;
			}
		}

		EG(opline_ptr) = &EX(opline);
		EG(active_op_array) = EX(op_array);
		EG(function_state_ptr) = &EX(function_state);
		destroy_op_array(new_op_array TSRMLS_CC);
		efree(new_op_array);
		if (EG(exception)) {
			zend_throw_exception_internal(NULL TSRMLS_CC);
		}
	} else {
		if (return_value_used) {
			ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
			INIT_ZVAL(*EX_T(opline->result.u.var).var.ptr);
			Z_LVAL_P(EX_T(opline->result.u.var).var.ptr) = failure_retval;
			Z_TYPE_P(EX_T(opline->result.u.var).var.ptr) = IS_BOOL;
		}
	}
	zval_dtor(free_op1.var);
	EG(return_value_ptr_ptr) = original_return_value;
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_UNSET_VAR_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval tmp, *varname;
	HashTable *target_symbol_table;
	zend_free_op free_op1;

	varname = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (Z_TYPE_P(varname) != IS_STRING) {
		tmp = *varname;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		varname = &tmp;
	} else if (IS_TMP_VAR == IS_CV || IS_TMP_VAR == IS_VAR) {
		varname->refcount++;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		zend_std_unset_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname) TSRMLS_CC);
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), BP_VAR_IS, varname TSRMLS_CC);
		if (zend_hash_del(target_symbol_table, varname->value.str.val, varname->value.str.len+1) == SUCCESS) {
			zend_execute_data *ex = execute_data;
			ulong hash_value = zend_inline_hash_func(varname->value.str.val, varname->value.str.len+1);

			do {
				int i;

				if (ex->op_array) {
					for (i = 0; i < ex->op_array->last_var; i++) {
						if (ex->op_array->vars[i].hash_value == hash_value &&
							ex->op_array->vars[i].name_len == varname->value.str.len &&
							!memcmp(ex->op_array->vars[i].name, varname->value.str.val, varname->value.str.len)) {
							ex->CVs[i] = NULL;
							break;
						}
					}
				}
				ex = ex->prev_execute_data;
			} while (ex && ex->symbol_table == target_symbol_table);
		}
	}

	if (varname == &tmp) {
		zval_dtor(&tmp);
	} else if (IS_TMP_VAR == IS_CV || IS_TMP_VAR == IS_VAR) {
		zval_ptr_dtor(&varname);
	}
	zval_dtor(free_op1.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FE_RESET_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *array_ptr, **array_ptr_ptr;
	HashTable *fe_ht;
	zend_object_iterator *iter = NULL;
	zend_class_entry *ce = NULL;
	zend_bool is_empty = 0;

	if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {
		array_ptr_ptr = NULL;
		if (array_ptr_ptr == NULL || array_ptr_ptr == &EG(uninitialized_zval_ptr)) {
			ALLOC_INIT_ZVAL(array_ptr);
		} else if (Z_TYPE_PP(array_ptr_ptr) == IS_OBJECT) {
			if(Z_OBJ_HT_PP(array_ptr_ptr)->get_class_entry == NULL) {
				zend_error(E_WARNING, "foreach() can not iterate over objects without PHP class");
				ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
			}

			ce = Z_OBJCE_PP(array_ptr_ptr);
			if (!ce || ce->get_iterator == NULL) {
				SEPARATE_ZVAL_IF_NOT_REF(array_ptr_ptr);
				(*array_ptr_ptr)->refcount++;
			}
			array_ptr = *array_ptr_ptr;
		} else {
			if (Z_TYPE_PP(array_ptr_ptr) == IS_ARRAY) {
				SEPARATE_ZVAL_IF_NOT_REF(array_ptr_ptr);
				if (opline->extended_value & ZEND_FE_FETCH_BYREF) {
					(*array_ptr_ptr)->is_ref = 1;
				}
			}
			array_ptr = *array_ptr_ptr;
			array_ptr->refcount++;
		}
	} else {
		array_ptr = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
		if (1) { /* IS_TMP_VAR */
			zval *tmp;

			ALLOC_ZVAL(tmp);
			INIT_PZVAL_COPY(tmp, array_ptr);
			array_ptr = tmp;
		} else if (Z_TYPE_P(array_ptr) == IS_OBJECT) {
			ce = Z_OBJCE_P(array_ptr);
			if (!ce || !ce->get_iterator) {
				array_ptr->refcount++;
			}
		} else {
			if ((IS_TMP_VAR == IS_CV || IS_TMP_VAR == IS_VAR) &&
			    !array_ptr->is_ref &&
			    array_ptr->refcount > 1) {
				zval *tmp;

				ALLOC_ZVAL(tmp);
				INIT_PZVAL_COPY(tmp, array_ptr);
				zval_copy_ctor(tmp);
				array_ptr = tmp;
			} else {
				array_ptr->refcount++;
			}
		}
	}

	if (IS_TMP_VAR != IS_TMP_VAR && ce && ce->get_iterator) {
		iter = ce->get_iterator(ce, array_ptr, opline->extended_value & ZEND_FE_RESET_REFERENCE TSRMLS_CC);

		if (iter && !EG(exception)) {
			array_ptr = zend_iterator_wrap(iter TSRMLS_CC);
		} else {
			if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {

			} else {

			}
			if (!EG(exception)) {
				zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "Object of type %s did not create an Iterator", ce->name);
			}
			zend_throw_exception_internal(NULL TSRMLS_CC);
			ZEND_VM_NEXT_OPCODE();
		}
	}

	PZVAL_LOCK(array_ptr);
	EX_T(opline->result.u.var).var.ptr = array_ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;

	if (iter) {
		iter->index = 0;
		if (iter->funcs->rewind) {
			iter->funcs->rewind(iter TSRMLS_CC);
			if (EG(exception)) {
				array_ptr->refcount--;
				zval_ptr_dtor(&array_ptr);
				if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {

				} else {

				}
				ZEND_VM_NEXT_OPCODE();
			}
		}
		is_empty = iter->funcs->valid(iter TSRMLS_CC) != SUCCESS;
		if (EG(exception)) {
			array_ptr->refcount--;
			zval_ptr_dtor(&array_ptr);
			if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {

			} else {

			}
			ZEND_VM_NEXT_OPCODE();
		}
		iter->index = -1; /* will be set to 0 before using next handler */
	} else if ((fe_ht = HASH_OF(array_ptr)) != NULL) {
		zend_hash_internal_pointer_reset(fe_ht);
		if (ce) {
			zend_object *zobj = zend_objects_get_address(array_ptr TSRMLS_CC);
			while (zend_hash_has_more_elements(fe_ht) == SUCCESS) {
				char *str_key;
				uint str_key_len;
				ulong int_key;
				zend_uchar key_type;

				key_type = zend_hash_get_current_key_ex(fe_ht, &str_key, &str_key_len, &int_key, 0, NULL);
				if (key_type != HASH_KEY_NON_EXISTANT &&
					(key_type == HASH_KEY_IS_LONG ||
				     zend_check_property_access(zobj, str_key, str_key_len-1 TSRMLS_CC) == SUCCESS)) {
					break;
				}
				zend_hash_move_forward(fe_ht);
			}
		}
		is_empty = zend_hash_has_more_elements(fe_ht) != SUCCESS;
		zend_hash_get_pointer(fe_ht, &EX_T(opline->result.u.var).fe.fe_pos);
	} else {
		zend_error(E_WARNING, "Invalid argument supplied for foreach()");
		is_empty = 1;
	}

	if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {

	} else {

	}
	if (is_empty) {
		ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
	} else {
		ZEND_VM_NEXT_OPCODE();
	}
}

static int ZEND_ISSET_ISEMPTY_VAR_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval tmp, *varname = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval **value;
	zend_bool isset = 1;
	HashTable *target_symbol_table;

	if (Z_TYPE_P(varname) != IS_STRING) {
		tmp = *varname;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		varname = &tmp;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		value = zend_std_get_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname), 1 TSRMLS_CC);
		if (!value) {
			isset = 0;
		}
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), BP_VAR_IS, varname TSRMLS_CC);
		if (zend_hash_find(target_symbol_table, varname->value.str.val, varname->value.str.len+1, (void **) &value) == FAILURE) {
			isset = 0;
		}
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			if (isset && Z_TYPE_PP(value) == IS_NULL) {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 0;
			} else {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = isset;
			}
			break;
		case ZEND_ISEMPTY:
			if (!isset || !i_zend_is_true(*value)) {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 1;
			} else {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 0;
			}
			break;
	}

	if (varname == &tmp) {
		zval_dtor(&tmp);
	}
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_EXIT_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
#if 0 || (IS_TMP_VAR != IS_UNUSED)
	zend_op *opline = EX(opline);
	if (IS_TMP_VAR != IS_UNUSED) {
		zend_free_op free_op1;
		zval *ptr = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

		if (Z_TYPE_P(ptr) == IS_LONG) {
			EG(exit_status) = Z_LVAL_P(ptr);
		} else {
			zend_print_variable(ptr);
		}
		zval_dtor(free_op1.var);
	}
#endif
	zend_bailout();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_END_SILENCE_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval restored_error_reporting;

	if (!EG(error_reporting) && Z_LVAL(EX_T(opline->op1.u.var).tmp_var) != 0) {
		Z_TYPE(restored_error_reporting) = IS_LONG;
		Z_LVAL(restored_error_reporting) = Z_LVAL(EX_T(opline->op1.u.var).tmp_var);
		convert_to_string(&restored_error_reporting);
		zend_alter_ini_entry_ex("error_reporting", sizeof("error_reporting"), Z_STRVAL(restored_error_reporting), Z_STRLEN(restored_error_reporting), ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME, 1);
		zendi_zval_dtor(restored_error_reporting);
	}
	if (EX(old_error_reporting) == &EX_T(opline->op1.u.var).tmp_var) {
		EX(old_error_reporting) = NULL;
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_QM_ASSIGN_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *value = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	EX_T(opline->result.u.var).tmp_var = *value;
	if (!1) {
		zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INSTANCEOF_SPEC_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *expr = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zend_bool result;

	if (Z_TYPE_P(expr) == IS_OBJECT && Z_OBJ_HT_P(expr)->get_class_entry) {
		result = instanceof_function(Z_OBJCE_P(expr), EX_T(opline->op2.u.var).class_entry TSRMLS_CC);
	} else {
		result = 0;
	}
	ZVAL_BOOL(&EX_T(opline->result.u.var).tmp_var, result);
	zval_dtor(free_op1.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	add_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SUB_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	sub_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MUL_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	mul_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DIV_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	div_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MOD_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	mod_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SL_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SR_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CONCAT_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	concat_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_IDENTICAL_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_IDENTICAL_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_EQUAL_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_EQUAL_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_OR_EQUAL_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_OR_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_AND_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_XOR_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_XOR_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_TMP_VAR_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *container = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (Z_TYPE_P(container) != IS_ARRAY) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		}
	} else {

		zval *dim = &opline->op2.u.constant;

		EX_T(opline->result.u.var).var.ptr_ptr = zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), dim, BP_VAR_R TSRMLS_CC);
		SELECTIVE_PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &opline->result);

	}
	AI_USE_PTR(EX_T(opline->result.u.var).var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_CHAR_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	add_char_to_string(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant);
	/* FREE_OP is missing intentionally here - we're always working on the same temporary variable */
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_STRING_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	add_string_to_string(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant);
	/* FREE_OP is missing intentionally here - we're always working on the same temporary variable */
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_METHOD_CALL_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;
	zend_free_op free_op1;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	function_name = &opline->op2.u.constant;

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = Z_STRVAL_P(function_name);
	function_name_strlen = Z_STRLEN_P(function_name);

	EX(object) = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (EX(object) && Z_TYPE_P(EX(object)) == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(&EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CASE_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;
	zend_free_op free_op1;

	if (IS_TMP_VAR==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
				 &opline->op2.u.constant TSRMLS_CC);

	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */
		zval_dtor(free_op1.var);
		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=&opline->op2.u.constant;

#if 0 || IS_TMP_VAR == IS_VAR || IS_TMP_VAR == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=NULL;
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	}
#else
	expr_ptr=_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
#endif

	if (1) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_TMP_VAR == IS_VAR || IS_TMP_VAR == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}

	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {

	} else {

	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_TMP_VAR == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_TMP_VAR != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_TMP_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_ADD_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	add_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SUB_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	sub_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MUL_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	mul_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DIV_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	div_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MOD_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	mod_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SL_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SR_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CONCAT_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	concat_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_IDENTICAL_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_IDENTICAL_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_EQUAL_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_EQUAL_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_OR_EQUAL_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_OR_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_AND_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_XOR_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_XOR_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_VAR_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *var = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval var_copy;
	int use_copy = 0;

	if (Z_TYPE_P(var) != IS_STRING) {
		zend_make_printable_zval(var, &var_copy, &use_copy);

		if (use_copy) {
			var = &var_copy;
		}
	}
	add_string_to_string(	&EX_T(opline->result.u.var).tmp_var,
							_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
							var);
	if (use_copy) {
		zval_dtor(var);
	}
	/* original comment, possibly problematic:
	 * FREE_OP is missing intentionally here - we're always working on the same temporary variable
	 * (Zeev):  I don't think it's problematic, we only use variables
	 * which aren't affected by FREE_OP(Ts, )'s anyway, unless they're
	 * string offsets or overloaded objects
	 */
	zval_dtor(free_op2.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_METHOD_CALL_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;
	zend_free_op free_op1, free_op2;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	function_name = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = Z_STRVAL_P(function_name);
	function_name_strlen = Z_STRLEN_P(function_name);

	EX(object) = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (EX(object) && Z_TYPE_P(EX(object)) == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(&EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}

	zval_dtor(free_op2.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CASE_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;
	zend_free_op free_op1, free_op2;

	if (IS_TMP_VAR==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
				 _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */
		zval_dtor(free_op1.var);
		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

#if 0 || IS_TMP_VAR == IS_VAR || IS_TMP_VAR == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=NULL;
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	}
#else
	expr_ptr=_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
#endif

	if (1) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_TMP_VAR == IS_VAR || IS_TMP_VAR == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}
		zval_dtor(free_op2.var);
	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {

	} else {

	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_TMP_VAR == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_TMP_VAR != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_TMP_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_ADD_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	add_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SUB_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	sub_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MUL_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	mul_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DIV_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	div_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MOD_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	mod_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SL_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SR_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CONCAT_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	concat_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_IDENTICAL_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_IDENTICAL_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_EQUAL_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_EQUAL_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_OR_EQUAL_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_OR_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_AND_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_XOR_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_XOR_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_VAR_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *var = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval var_copy;
	int use_copy = 0;

	if (Z_TYPE_P(var) != IS_STRING) {
		zend_make_printable_zval(var, &var_copy, &use_copy);

		if (use_copy) {
			var = &var_copy;
		}
	}
	add_string_to_string(	&EX_T(opline->result.u.var).tmp_var,
							_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
							var);
	if (use_copy) {
		zval_dtor(var);
	}
	/* original comment, possibly problematic:
	 * FREE_OP is missing intentionally here - we're always working on the same temporary variable
	 * (Zeev):  I don't think it's problematic, we only use variables
	 * which aren't affected by FREE_OP(Ts, )'s anyway, unless they're
	 * string offsets or overloaded objects
	 */
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_METHOD_CALL_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;
	zend_free_op free_op1, free_op2;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	function_name = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = Z_STRVAL_P(function_name);
	function_name_strlen = Z_STRLEN_P(function_name);

	EX(object) = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (EX(object) && Z_TYPE_P(EX(object)) == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(&EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CASE_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;
	zend_free_op free_op1, free_op2;

	if (IS_TMP_VAR==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
				 _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */
		zval_dtor(free_op1.var);
		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

#if 0 || IS_TMP_VAR == IS_VAR || IS_TMP_VAR == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=NULL;
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	}
#else
	expr_ptr=_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
#endif

	if (1) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_TMP_VAR == IS_VAR || IS_TMP_VAR == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {

	} else {

	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_TMP_VAR == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_TMP_VAR != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_TMP_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_TMP_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=NULL;

#if 0 || IS_TMP_VAR == IS_VAR || IS_TMP_VAR == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=NULL;
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	}
#else
	expr_ptr=_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
#endif

	if (1) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_TMP_VAR == IS_VAR || IS_TMP_VAR == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}

	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {

	} else {

	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_TMP_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_TMP_VAR == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_TMP_VAR != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_TMP_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_ADD_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	add_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SUB_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	sub_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MUL_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	mul_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DIV_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	div_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MOD_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	mod_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SL_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SR_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CONCAT_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	concat_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_IDENTICAL_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_IDENTICAL_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_EQUAL_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_EQUAL_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_OR_EQUAL_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_OR_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_AND_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_XOR_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_XOR_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	zval_dtor(free_op1.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_VAR_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *var = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval var_copy;
	int use_copy = 0;

	if (Z_TYPE_P(var) != IS_STRING) {
		zend_make_printable_zval(var, &var_copy, &use_copy);

		if (use_copy) {
			var = &var_copy;
		}
	}
	add_string_to_string(	&EX_T(opline->result.u.var).tmp_var,
							_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
							var);
	if (use_copy) {
		zval_dtor(var);
	}
	/* original comment, possibly problematic:
	 * FREE_OP is missing intentionally here - we're always working on the same temporary variable
	 * (Zeev):  I don't think it's problematic, we only use variables
	 * which aren't affected by FREE_OP(Ts, )'s anyway, unless they're
	 * string offsets or overloaded objects
	 */

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_METHOD_CALL_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;
	zend_free_op free_op1;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	function_name = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = Z_STRVAL_P(function_name);
	function_name_strlen = Z_STRLEN_P(function_name);

	EX(object) = _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (EX(object) && Z_TYPE_P(EX(object)) == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(&EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CASE_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;
	zend_free_op free_op1;

	if (IS_TMP_VAR==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
				 _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);

	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */
		zval_dtor(free_op1.var);
		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

#if 0 || IS_TMP_VAR == IS_VAR || IS_TMP_VAR == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=NULL;
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	}
#else
	expr_ptr=_get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
#endif

	if (1) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_TMP_VAR == IS_VAR || IS_TMP_VAR == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}

	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {

	} else {

	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_TMP_VAR == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_TMP_VAR != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_TMP_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_BW_NOT_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	bitwise_not_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_NOT_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	boolean_not_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRE_INC_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **var_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		val->refcount++;
		increment_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		increment_function(*var_ptr);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRE_DEC_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **var_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		val->refcount++;
		decrement_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		decrement_function(*var_ptr);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_POST_INC_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **var_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).tmp_var = *EG(uninitialized_zval_ptr);
		}
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	}

	EX_T(opline->result.u.var).tmp_var = **var_ptr;
	zendi_zval_copy_ctor(EX_T(opline->result.u.var).tmp_var);

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		val->refcount++;
		increment_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		increment_function(*var_ptr);
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_POST_DEC_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **var_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).tmp_var = *EG(uninitialized_zval_ptr);
		}
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	}

	EX_T(opline->result.u.var).tmp_var = **var_ptr;
	zendi_zval_copy_ctor(EX_T(opline->result.u.var).tmp_var);

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		val->refcount++;
		decrement_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		decrement_function(*var_ptr);
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ECHO_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval z_copy;
	zval *z = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get_method != NULL &&
		zend_std_cast_object_tostring(z, &z_copy, IS_STRING TSRMLS_CC) == SUCCESS) {
		zend_print_variable(&z_copy);
		zval_dtor(&z_copy);
	} else {
		zend_print_variable(z);
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRINT_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 1;
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_LONG;

	return ZEND_ECHO_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_fetch_var_address_helper_SPEC_VAR(int type, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *varname = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval **retval;
	zval tmp_varname;
	HashTable *target_symbol_table;

 	if (Z_TYPE_P(varname) != IS_STRING) {
		tmp_varname = *varname;
		zval_copy_ctor(&tmp_varname);
		convert_to_string(&tmp_varname);
		varname = &tmp_varname;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		retval = zend_std_get_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname), 0 TSRMLS_CC);
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), type, varname TSRMLS_CC);
/*
		if (!target_symbol_table) {
			ZEND_VM_NEXT_OPCODE();
		}
*/
		if (zend_hash_find(target_symbol_table, varname->value.str.val, varname->value.str.len+1, (void **) &retval) == FAILURE) {
			switch (type) {
				case BP_VAR_R:
				case BP_VAR_UNSET:
					zend_error(E_NOTICE,"Undefined variable: %s", Z_STRVAL_P(varname));
					/* break missing intentionally */
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval_ptr);
					break;
				case BP_VAR_RW:
					zend_error(E_NOTICE,"Undefined variable: %s", Z_STRVAL_P(varname));
					/* break missing intentionally */
				case BP_VAR_W: {
						zval *new_zval = &EG(uninitialized_zval);

						new_zval->refcount++;
						zend_hash_update(target_symbol_table, varname->value.str.val, varname->value.str.len+1, &new_zval, sizeof(zval *), (void **) &retval);
					}
					break;
				EMPTY_SWITCH_DEFAULT_CASE()
			}
		}
		switch (opline->op2.u.EA.type) {
			case ZEND_FETCH_GLOBAL:
				if (IS_VAR != IS_TMP_VAR) {
					if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
				}
				break;
			case ZEND_FETCH_LOCAL:
				if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
				break;
			case ZEND_FETCH_STATIC:
				zval_update_constant(retval, (void*) 1 TSRMLS_CC);
				break;
			case ZEND_FETCH_GLOBAL_LOCK:
				if (IS_VAR == IS_VAR && !free_op1.var) {
					PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
				}
				break;
		}
	}


	if (varname == &tmp_varname) {
		zval_dtor(varname);
	}
	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = retval;
		PZVAL_LOCK(*retval);
		switch (type) {
			case BP_VAR_R:
			case BP_VAR_IS:
				AI_USE_PTR(EX_T(opline->result.u.var).var);
				break;
			case BP_VAR_UNSET: {
				zend_free_op free_res;

				PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
				if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
					SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
				}
				PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
				FREE_OP_VAR_PTR(free_res);
				break;
			}
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_R_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_VAR(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_W_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_VAR(BP_VAR_W, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_RW_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_VAR(BP_VAR_RW, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_FUNC_ARG_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_VAR(ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), EX(opline)->extended_value)?BP_VAR_W:BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_UNSET_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_VAR(BP_VAR_UNSET, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_IS_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_VAR(BP_VAR_IS, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_JMPZ_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int ret = i_zend_is_true(_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC));

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (!ret) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_JMPNZ_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int ret = i_zend_is_true(_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC));

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (ret) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_JMPZNZ_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int retval = i_zend_is_true(_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC));

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp on true to %d\n", opline->extended_value);
#endif
		ZEND_VM_JMP(&EX(op_array)->opcodes[opline->extended_value]);
	} else {
#if DEBUG_ZEND>=2
		printf("Conditional jmp on false to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(&EX(op_array)->opcodes[opline->op2.u.opline_num]);
	}
}

static int ZEND_JMPZ_EX_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int retval = i_zend_is_true(_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC));

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = retval;
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;
	if (!retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_JMPNZ_EX_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int retval = i_zend_is_true(_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC));

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = retval;
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;
	if (retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_RETURN_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *retval_ptr;
	zval **retval_ptr_ptr;
	zend_free_op free_op1;

	if (EG(active_op_array)->return_reference == ZEND_RETURN_REF) {

		if (IS_VAR == IS_CONST || IS_VAR == IS_TMP_VAR) {
			/* Not supposed to happen, but we'll allow it */
			zend_error(E_NOTICE, "Only variable references should be returned by reference");
			goto return_by_value;
		}

		retval_ptr_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

		if (!retval_ptr_ptr) {
			zend_error_noreturn(E_ERROR, "Cannot return string offsets by reference");
		}

		if (IS_VAR == IS_VAR && !(*retval_ptr_ptr)->is_ref) {
			if (opline->extended_value == ZEND_RETURNS_FUNCTION &&
			    EX_T(opline->op1.u.var).var.fcall_returned_reference) {
			} else if (EX_T(opline->op1.u.var).var.ptr_ptr == &EX_T(opline->op1.u.var).var.ptr) {
				if (IS_VAR == IS_VAR && !(free_op1.var != NULL)) {
					PZVAL_LOCK(*retval_ptr_ptr); /* undo the effect of get_zval_ptr_ptr() */
				}
				zend_error(E_NOTICE, "Only variable references should be returned by reference");
				goto return_by_value;
			}
		}

		SEPARATE_ZVAL_TO_MAKE_IS_REF(retval_ptr_ptr);
		(*retval_ptr_ptr)->refcount++;

		(*EG(return_value_ptr_ptr)) = (*retval_ptr_ptr);
	} else {
return_by_value:

		retval_ptr = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

		if (EG(ze1_compatibility_mode) && Z_TYPE_P(retval_ptr) == IS_OBJECT) {
			zval *ret;
			char *class_name;
			zend_uint class_name_len;
			int dup;

			ALLOC_ZVAL(ret);
			INIT_PZVAL_COPY(ret, retval_ptr);
			dup = zend_get_object_classname(retval_ptr, &class_name, &class_name_len TSRMLS_CC);
			if (Z_OBJ_HT_P(retval_ptr)->clone_obj == NULL) {
				zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object of class %s",  class_name);
			}
			zend_error(E_STRICT, "Implicit cloning object of class '%s' because of 'zend.ze1_compatibility_mode'", class_name);
			ret->value.obj = Z_OBJ_HT_P(retval_ptr)->clone_obj(retval_ptr TSRMLS_CC);
			*EG(return_value_ptr_ptr) = ret;
			if (!dup) {
				efree(class_name);
			}
		} else if (!0) { /* Not a temp var */
			if (EG(active_op_array)->return_reference == ZEND_RETURN_REF ||
			    (PZVAL_IS_REF(retval_ptr) && retval_ptr->refcount > 0)) {
				zval *ret;

				ALLOC_ZVAL(ret);
				INIT_PZVAL_COPY(ret, retval_ptr);
				zval_copy_ctor(ret);
				*EG(return_value_ptr_ptr) = ret;
			} else {
				*EG(return_value_ptr_ptr) = retval_ptr;
				retval_ptr->refcount++;
			}
		} else {
			zval *ret;

			ALLOC_ZVAL(ret);
			INIT_PZVAL_COPY(ret, retval_ptr);
			*EG(return_value_ptr_ptr) = ret;
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_RETURN_FROM_EXECUTE_LOOP();
}

static int ZEND_THROW_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *value;
	zval *exception;
	zend_free_op free_op1;

	value = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (Z_TYPE_P(value) != IS_OBJECT) {
		zend_error_noreturn(E_ERROR, "Can only throw objects");
	}
	/* Not sure if a complete copy is what we want here */
	ALLOC_ZVAL(exception);
	INIT_PZVAL_COPY(exception, value);
	if (!0) {
		zval_copy_ctor(exception);
	}

	zend_throw_exception_object(exception TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SEND_VAL_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	if (opline->extended_value==ZEND_DO_FCALL_BY_NAME
		&& ARG_MUST_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
			zend_error_noreturn(E_ERROR, "Cannot pass parameter %d by reference", opline->op2.u.opline_num);
	}
	{
		zval *valptr;
		zval *value;
		zend_free_op free_op1;

		value = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

		ALLOC_ZVAL(valptr);
		INIT_PZVAL_COPY(valptr, value);
		if (!0) {
			zval_copy_ctor(valptr);
		}
		zend_ptr_stack_push(&EG(argument_stack), valptr);
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	}
	ZEND_VM_NEXT_OPCODE();
}

static int zend_send_by_var_helper_SPEC_VAR(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *varptr;
	zend_free_op free_op1;
	varptr = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (varptr == &EG(uninitialized_zval)) {
		ALLOC_ZVAL(varptr);
		INIT_ZVAL(*varptr);
		varptr->refcount = 0;
	} else if (PZVAL_IS_REF(varptr)) {
		zval *original_var = varptr;

		ALLOC_ZVAL(varptr);
		*varptr = *original_var;
		varptr->is_ref = 0;
		varptr->refcount = 0;
		zval_copy_ctor(varptr);
	}
	varptr->refcount++;
	zend_ptr_stack_push(&EG(argument_stack), varptr);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};  /* for string offsets */

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SEND_VAR_NO_REF_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *varptr;

	if (opline->extended_value & ZEND_ARG_COMPILE_TIME_BOUND) { /* Had function_ptr at compile_time */
		if (!(opline->extended_value & ZEND_ARG_SEND_BY_REF)) {
			return zend_send_by_var_helper_SPEC_VAR(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
		}
	} else if (!ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
		return zend_send_by_var_helper_SPEC_VAR(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}

	if (IS_VAR == IS_VAR &&
		(opline->extended_value & ZEND_ARG_SEND_FUNCTION) &&
		EX_T(opline->op1.u.var).var.fcall_returned_reference &&
		EX_T(opline->op1.u.var).var.ptr) {
		varptr = EX_T(opline->op1.u.var).var.ptr;
		PZVAL_UNLOCK_EX(varptr, &free_op1, 0);
	} else {
		varptr = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	}
	if ((!(opline->extended_value & ZEND_ARG_SEND_FUNCTION) ||
	     EX_T(opline->op1.u.var).var.fcall_returned_reference) &&
	    varptr != &EG(uninitialized_zval) &&
	    (PZVAL_IS_REF(varptr) ||
	     (varptr->refcount == 1 && (IS_VAR == IS_CV || free_op1.var)))) {
		varptr->is_ref = 1;
		varptr->refcount++;
		zend_ptr_stack_push(&EG(argument_stack), varptr);
	} else {
		zval *valptr;

		zend_error(E_STRICT, "Only variables should be passed by reference");
		ALLOC_ZVAL(valptr);
		INIT_PZVAL_COPY(valptr, varptr);
		if (!0) {
			zval_copy_ctor(valptr);
		}
		zend_ptr_stack_push(&EG(argument_stack), valptr);
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SEND_REF_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **varptr_ptr;
	zval *varptr;
	varptr_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (!varptr_ptr) {
		zend_error_noreturn(E_ERROR, "Only variables can be passed by reference");
	}

	SEPARATE_ZVAL_TO_MAKE_IS_REF(varptr_ptr);
	varptr = *varptr_ptr;
	varptr->refcount++;
	zend_ptr_stack_push(&EG(argument_stack), varptr);

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SEND_VAR_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	if ((opline->extended_value == ZEND_DO_FCALL_BY_NAME)
		&& ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
		return ZEND_SEND_REF_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}
	return zend_send_by_var_helper_SPEC_VAR(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_BOOL_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	/* PHP 3.0 returned "" for false and 1 for true, here we use 0 and 1 for now */
	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = i_zend_is_true(_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC));
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SWITCH_FREE_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_switch_free(EX(opline), EX(Ts) TSRMLS_CC);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CLONE_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *obj = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zend_class_entry *ce;
	zend_function *clone;
	zend_object_clone_obj_t clone_call;

	if (!obj || Z_TYPE_P(obj) != IS_OBJECT) {
		zend_error_noreturn(E_ERROR, "__clone method called on non-object");
		EX_T(opline->result.u.var).var.ptr = EG(error_zval_ptr);
		EX_T(opline->result.u.var).var.ptr->refcount++;
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	}

	ce = Z_OBJCE_P(obj);
	clone = ce ? ce->clone : NULL;
	clone_call =  Z_OBJ_HT_P(obj)->clone_obj;
	if (!clone_call) {
		if (ce) {
			zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object of class %s", ce->name);
		} else {
			zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object");
		}
		EX_T(opline->result.u.var).var.ptr = EG(error_zval_ptr);
		EX_T(opline->result.u.var).var.ptr->refcount++;
	}

	if (ce && clone) {
		if (clone->op_array.fn_flags & ZEND_ACC_PRIVATE) {
			/* Ensure that if we're calling a private function, we're allowed to do so.
			 */
			if (ce != EG(scope)) {
				zend_error_noreturn(E_ERROR, "Call to private %s::__clone() from context '%s'", ce->name, EG(scope) ? EG(scope)->name : "");
			}
		} else if ((clone->common.fn_flags & ZEND_ACC_PROTECTED)) {
			/* Ensure that if we're calling a protected function, we're allowed to do so.
			 */
			if (!zend_check_protected(clone->common.scope, EG(scope))) {
				zend_error_noreturn(E_ERROR, "Call to protected %s::__clone() from context '%s'", ce->name, EG(scope) ? EG(scope)->name : "");
			}
		}
	}

	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	if (!EG(exception)) {
		ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
		Z_OBJVAL_P(EX_T(opline->result.u.var).var.ptr) = clone_call(obj TSRMLS_CC);
		Z_TYPE_P(EX_T(opline->result.u.var).var.ptr) = IS_OBJECT;
		EX_T(opline->result.u.var).var.ptr->refcount=1;
		EX_T(opline->result.u.var).var.ptr->is_ref=1;
		if (!RETURN_VALUE_USED(opline) || EG(exception)) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CAST_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *expr = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *result = &EX_T(opline->result.u.var).tmp_var;

	if (opline->extended_value != IS_STRING) {
		*result = *expr;
		if (!0) {
			zendi_zval_copy_ctor(*result);
		}
	}
	switch (opline->extended_value) {
		case IS_NULL:
			convert_to_null(result);
			break;
		case IS_BOOL:
			convert_to_boolean(result);
			break;
		case IS_LONG:
			convert_to_long(result);
			break;
		case IS_DOUBLE:
			convert_to_double(result);
			break;
		case IS_STRING: {
			zval var_copy;
			int use_copy;

			zend_make_printable_zval(expr, &var_copy, &use_copy);
			if (use_copy) {
				*result = var_copy;
				if (0) {
					if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
				}
			} else {
				*result = *expr;
				if (!0) {
					zendi_zval_copy_ctor(*result);
				}
			}
			break;
		}
		case IS_ARRAY:
			convert_to_array(result);
			break;
		case IS_OBJECT:
			convert_to_object(result);
			break;
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INCLUDE_OR_EVAL_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op_array *new_op_array=NULL;
	zval **original_return_value = EG(return_value_ptr_ptr);
	int return_value_used;
	zend_free_op free_op1;
	zval *inc_filename = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval tmp_inc_filename;
	zend_bool failure_retval=0;

	if (inc_filename->type!=IS_STRING) {
		tmp_inc_filename = *inc_filename;
		zval_copy_ctor(&tmp_inc_filename);
		convert_to_string(&tmp_inc_filename);
		inc_filename = &tmp_inc_filename;
	}

	return_value_used = RETURN_VALUE_USED(opline);

	switch (Z_LVAL(opline->op2.u.constant)) {
		case ZEND_INCLUDE_ONCE:
		case ZEND_REQUIRE_ONCE: {
				zend_file_handle file_handle;

				if (IS_ABSOLUTE_PATH(Z_STRVAL_P(inc_filename), Z_STRLEN_P(inc_filename))) {
					cwd_state state;

					state.cwd_length = 0;
					state.cwd = malloc(1);
					state.cwd[0] = 0;

					failure_retval = (!virtual_file_ex(&state, Z_STRVAL_P(inc_filename), NULL, 1) &&
						zend_hash_exists(&EG(included_files), state.cwd, state.cwd_length+1));

					free(state.cwd);
				}

				if (failure_retval) {
					/* do nothing */
				} else if (SUCCESS == zend_stream_open(Z_STRVAL_P(inc_filename), &file_handle TSRMLS_CC)) {

					if (!file_handle.opened_path) {
						file_handle.opened_path = estrndup(Z_STRVAL_P(inc_filename), Z_STRLEN_P(inc_filename));
					}

					if (zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path, strlen(file_handle.opened_path)+1)==SUCCESS) {
						new_op_array = zend_compile_file(&file_handle, (Z_LVAL(opline->op2.u.constant)==ZEND_INCLUDE_ONCE?ZEND_INCLUDE:ZEND_REQUIRE) TSRMLS_CC);
						zend_destroy_file_handle(&file_handle TSRMLS_CC);
					} else {
						zend_file_handle_dtor(&file_handle);
						failure_retval=1;
					}
				} else {
					if (Z_LVAL(opline->op2.u.constant)==ZEND_INCLUDE_ONCE) {
						zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, Z_STRVAL_P(inc_filename));
					} else {
						zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, Z_STRVAL_P(inc_filename));
					}
				}
			}
			break;
		case ZEND_INCLUDE:
		case ZEND_REQUIRE:
			new_op_array = compile_filename(Z_LVAL(opline->op2.u.constant), inc_filename TSRMLS_CC);
			break;
		case ZEND_EVAL: {
				char *eval_desc = zend_make_compiled_string_description("eval()'d code" TSRMLS_CC);

				new_op_array = zend_compile_string(inc_filename, eval_desc TSRMLS_CC);
				efree(eval_desc);
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	if (inc_filename==&tmp_inc_filename) {
		zval_dtor(&tmp_inc_filename);
	}
	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	if (new_op_array) {
		zval *saved_object;
		zend_function *saved_function;

		EG(return_value_ptr_ptr) = EX_T(opline->result.u.var).var.ptr_ptr;
		EG(active_op_array) = new_op_array;
		EX_T(opline->result.u.var).var.ptr = NULL;

		saved_object = EX(object);
		saved_function = EX(function_state).function;

		EX(function_state).function = (zend_function *) new_op_array;
		EX(object) = NULL;

		zend_execute(new_op_array TSRMLS_CC);

		EX(function_state).function = saved_function;
		EX(object) = saved_object;

		if (!return_value_used) {
			if (EX_T(opline->result.u.var).var.ptr) {
				zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
			}
		} else { /* return value is used */
			if (!EX_T(opline->result.u.var).var.ptr) { /* there was no return statement */
				ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
				INIT_PZVAL(EX_T(opline->result.u.var).var.ptr);
				Z_LVAL_P(EX_T(opline->result.u.var).var.ptr) = 1;
				Z_TYPE_P(EX_T(opline->result.u.var).var.ptr) = IS_BOOL;
			}
		}

		EG(opline_ptr) = &EX(opline);
		EG(active_op_array) = EX(op_array);
		EG(function_state_ptr) = &EX(function_state);
		destroy_op_array(new_op_array TSRMLS_CC);
		efree(new_op_array);
		if (EG(exception)) {
			zend_throw_exception_internal(NULL TSRMLS_CC);
		}
	} else {
		if (return_value_used) {
			ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
			INIT_ZVAL(*EX_T(opline->result.u.var).var.ptr);
			Z_LVAL_P(EX_T(opline->result.u.var).var.ptr) = failure_retval;
			Z_TYPE_P(EX_T(opline->result.u.var).var.ptr) = IS_BOOL;
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	EG(return_value_ptr_ptr) = original_return_value;
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_UNSET_VAR_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval tmp, *varname;
	HashTable *target_symbol_table;
	zend_free_op free_op1;

	varname = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (Z_TYPE_P(varname) != IS_STRING) {
		tmp = *varname;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		varname = &tmp;
	} else if (IS_VAR == IS_CV || IS_VAR == IS_VAR) {
		varname->refcount++;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		zend_std_unset_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname) TSRMLS_CC);
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), BP_VAR_IS, varname TSRMLS_CC);
		if (zend_hash_del(target_symbol_table, varname->value.str.val, varname->value.str.len+1) == SUCCESS) {
			zend_execute_data *ex = execute_data;
			ulong hash_value = zend_inline_hash_func(varname->value.str.val, varname->value.str.len+1);

			do {
				int i;

				if (ex->op_array) {
					for (i = 0; i < ex->op_array->last_var; i++) {
						if (ex->op_array->vars[i].hash_value == hash_value &&
							ex->op_array->vars[i].name_len == varname->value.str.len &&
							!memcmp(ex->op_array->vars[i].name, varname->value.str.val, varname->value.str.len)) {
							ex->CVs[i] = NULL;
							break;
						}
					}
				}
				ex = ex->prev_execute_data;
			} while (ex && ex->symbol_table == target_symbol_table);
		}
	}

	if (varname == &tmp) {
		zval_dtor(&tmp);
	} else if (IS_VAR == IS_CV || IS_VAR == IS_VAR) {
		zval_ptr_dtor(&varname);
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FE_RESET_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *array_ptr, **array_ptr_ptr;
	HashTable *fe_ht;
	zend_object_iterator *iter = NULL;
	zend_class_entry *ce = NULL;
	zend_bool is_empty = 0;

	if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {
		array_ptr_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
		if (array_ptr_ptr == NULL || array_ptr_ptr == &EG(uninitialized_zval_ptr)) {
			ALLOC_INIT_ZVAL(array_ptr);
		} else if (Z_TYPE_PP(array_ptr_ptr) == IS_OBJECT) {
			if(Z_OBJ_HT_PP(array_ptr_ptr)->get_class_entry == NULL) {
				zend_error(E_WARNING, "foreach() can not iterate over objects without PHP class");
				ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
			}

			ce = Z_OBJCE_PP(array_ptr_ptr);
			if (!ce || ce->get_iterator == NULL) {
				SEPARATE_ZVAL_IF_NOT_REF(array_ptr_ptr);
				(*array_ptr_ptr)->refcount++;
			}
			array_ptr = *array_ptr_ptr;
		} else {
			if (Z_TYPE_PP(array_ptr_ptr) == IS_ARRAY) {
				SEPARATE_ZVAL_IF_NOT_REF(array_ptr_ptr);
				if (opline->extended_value & ZEND_FE_FETCH_BYREF) {
					(*array_ptr_ptr)->is_ref = 1;
				}
			}
			array_ptr = *array_ptr_ptr;
			array_ptr->refcount++;
		}
	} else {
		array_ptr = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
		if (0) { /* IS_TMP_VAR */
			zval *tmp;

			ALLOC_ZVAL(tmp);
			INIT_PZVAL_COPY(tmp, array_ptr);
			array_ptr = tmp;
		} else if (Z_TYPE_P(array_ptr) == IS_OBJECT) {
			ce = Z_OBJCE_P(array_ptr);
			if (!ce || !ce->get_iterator) {
				array_ptr->refcount++;
			}
		} else {
			if ((IS_VAR == IS_CV || IS_VAR == IS_VAR) &&
			    !array_ptr->is_ref &&
			    array_ptr->refcount > 1) {
				zval *tmp;

				ALLOC_ZVAL(tmp);
				INIT_PZVAL_COPY(tmp, array_ptr);
				zval_copy_ctor(tmp);
				array_ptr = tmp;
			} else {
				array_ptr->refcount++;
			}
		}
	}

	if (IS_VAR != IS_TMP_VAR && ce && ce->get_iterator) {
		iter = ce->get_iterator(ce, array_ptr, opline->extended_value & ZEND_FE_RESET_REFERENCE TSRMLS_CC);

		if (iter && !EG(exception)) {
			array_ptr = zend_iterator_wrap(iter TSRMLS_CC);
		} else {
			if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {
				if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
			} else {
				if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
			}
			if (!EG(exception)) {
				zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "Object of type %s did not create an Iterator", ce->name);
			}
			zend_throw_exception_internal(NULL TSRMLS_CC);
			ZEND_VM_NEXT_OPCODE();
		}
	}

	PZVAL_LOCK(array_ptr);
	EX_T(opline->result.u.var).var.ptr = array_ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;

	if (iter) {
		iter->index = 0;
		if (iter->funcs->rewind) {
			iter->funcs->rewind(iter TSRMLS_CC);
			if (EG(exception)) {
				array_ptr->refcount--;
				zval_ptr_dtor(&array_ptr);
				if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {
					if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
				} else {
					if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
				}
				ZEND_VM_NEXT_OPCODE();
			}
		}
		is_empty = iter->funcs->valid(iter TSRMLS_CC) != SUCCESS;
		if (EG(exception)) {
			array_ptr->refcount--;
			zval_ptr_dtor(&array_ptr);
			if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {
				if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
			} else {
				if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
			}
			ZEND_VM_NEXT_OPCODE();
		}
		iter->index = -1; /* will be set to 0 before using next handler */
	} else if ((fe_ht = HASH_OF(array_ptr)) != NULL) {
		zend_hash_internal_pointer_reset(fe_ht);
		if (ce) {
			zend_object *zobj = zend_objects_get_address(array_ptr TSRMLS_CC);
			while (zend_hash_has_more_elements(fe_ht) == SUCCESS) {
				char *str_key;
				uint str_key_len;
				ulong int_key;
				zend_uchar key_type;

				key_type = zend_hash_get_current_key_ex(fe_ht, &str_key, &str_key_len, &int_key, 0, NULL);
				if (key_type != HASH_KEY_NON_EXISTANT &&
					(key_type == HASH_KEY_IS_LONG ||
				     zend_check_property_access(zobj, str_key, str_key_len-1 TSRMLS_CC) == SUCCESS)) {
					break;
				}
				zend_hash_move_forward(fe_ht);
			}
		}
		is_empty = zend_hash_has_more_elements(fe_ht) != SUCCESS;
		zend_hash_get_pointer(fe_ht, &EX_T(opline->result.u.var).fe.fe_pos);
	} else {
		zend_error(E_WARNING, "Invalid argument supplied for foreach()");
		is_empty = 1;
	}

	if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	} else {
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	}
	if (is_empty) {
		ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
	} else {
		ZEND_VM_NEXT_OPCODE();
	}
}

static int ZEND_FE_FETCH_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *array = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval **value;
	char *str_key;
	uint str_key_len;
	ulong int_key;
	HashTable *fe_ht;
	zend_object_iterator *iter = NULL;
	int key_type = 0;
	zend_bool use_key = (zend_bool)(opline->extended_value & ZEND_FE_FETCH_WITH_KEY);

	PZVAL_LOCK(array);

	switch (zend_iterator_unwrap(array, &iter TSRMLS_CC)) {
		default:
		case ZEND_ITER_INVALID:
			zend_error(E_WARNING, "Invalid argument supplied for foreach()");
			ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);

		case ZEND_ITER_PLAIN_OBJECT: {
			char *class_name, *prop_name;
			zend_object *zobj = zend_objects_get_address(array TSRMLS_CC);

			fe_ht = HASH_OF(array);
			zend_hash_set_pointer(fe_ht, &EX_T(opline->op1.u.var).fe.fe_pos);
			do {
				if (zend_hash_get_current_data(fe_ht, (void **) &value)==FAILURE) {
					/* reached end of iteration */
					ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
				}
				key_type = zend_hash_get_current_key_ex(fe_ht, &str_key, &str_key_len, &int_key, 0, NULL);

				zend_hash_move_forward(fe_ht);
			} while (key_type == HASH_KEY_NON_EXISTANT ||
			         (key_type != HASH_KEY_IS_LONG &&
			          zend_check_property_access(zobj, str_key, str_key_len-1 TSRMLS_CC) != SUCCESS));
			zend_hash_get_pointer(fe_ht, &EX_T(opline->op1.u.var).fe.fe_pos);
			if (use_key && key_type != HASH_KEY_IS_LONG) {
				zend_unmangle_property_name(str_key, str_key_len-1, &class_name, &prop_name);
				str_key_len = strlen(prop_name);
				str_key = estrndup(prop_name, str_key_len);
				str_key_len++;
			}
			break;
		}

		case ZEND_ITER_PLAIN_ARRAY:
			fe_ht = HASH_OF(array);
			zend_hash_set_pointer(fe_ht, &EX_T(opline->op1.u.var).fe.fe_pos);
			if (zend_hash_get_current_data(fe_ht, (void **) &value)==FAILURE) {
				/* reached end of iteration */
				ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
			}
			if (use_key) {
				key_type = zend_hash_get_current_key_ex(fe_ht, &str_key, &str_key_len, &int_key, 1, NULL);
			}
			zend_hash_move_forward(fe_ht);
			zend_hash_get_pointer(fe_ht, &EX_T(opline->op1.u.var).fe.fe_pos);
			break;

		case ZEND_ITER_OBJECT:
			/* !iter happens from exception */
			if (iter && ++iter->index > 0) {
				/* This could cause an endless loop if index becomes zero again.
				 * In case that ever happens we need an additional flag. */
				iter->funcs->move_forward(iter TSRMLS_CC);
				if (EG(exception)) {
					array->refcount--;
					zval_ptr_dtor(&array);
					ZEND_VM_NEXT_OPCODE();
				}
			}
			/* If index is zero we come from FE_RESET and checked valid() already. */
			if (!iter || (iter->index > 0 && iter->funcs->valid(iter TSRMLS_CC) == FAILURE)) {
				/* reached end of iteration */
				if (EG(exception)) {
					array->refcount--;
					zval_ptr_dtor(&array);
					ZEND_VM_NEXT_OPCODE();
				}
				ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
			}
			iter->funcs->get_current_data(iter, &value TSRMLS_CC);
			if (EG(exception)) {
				array->refcount--;
				zval_ptr_dtor(&array);
				ZEND_VM_NEXT_OPCODE();
			}
			if (!value) {
				/* failure in get_current_data */
				ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
			}
			if (use_key) {
				if (iter->funcs->get_current_key) {
					key_type = iter->funcs->get_current_key(iter, &str_key, &str_key_len, &int_key TSRMLS_CC);
					if (EG(exception)) {
						array->refcount--;
						zval_ptr_dtor(&array);
						ZEND_VM_NEXT_OPCODE();
					}
				} else {
					key_type = HASH_KEY_IS_LONG;
					int_key = iter->index;
				}
			}
			break;
	}

	if (opline->extended_value & ZEND_FE_FETCH_BYREF) {
		SEPARATE_ZVAL_IF_NOT_REF(value);
		(*value)->is_ref = 1;
		EX_T(opline->result.u.var).var.ptr_ptr = value;
		(*value)->refcount++;
	} else {
		EX_T(opline->result.u.var).var.ptr_ptr = value;
		PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	if (use_key) {
		zend_op *op_data = opline+1;
		zval *key = &EX_T(op_data->result.u.var).tmp_var;

		switch (key_type) {
			case HASH_KEY_IS_STRING:
				Z_STRVAL_P(key) = str_key;
				Z_STRLEN_P(key) = str_key_len-1;
				Z_TYPE_P(key) = IS_STRING;
				break;
			case HASH_KEY_IS_LONG:
				Z_LVAL_P(key) = int_key;
				Z_TYPE_P(key) = IS_LONG;
				break;
			default:
			case HASH_KEY_NON_EXISTANT:
				ZVAL_NULL(key);
				break;
		}
	}

	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ISSET_ISEMPTY_VAR_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval tmp, *varname = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval **value;
	zend_bool isset = 1;
	HashTable *target_symbol_table;

	if (Z_TYPE_P(varname) != IS_STRING) {
		tmp = *varname;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		varname = &tmp;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		value = zend_std_get_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname), 1 TSRMLS_CC);
		if (!value) {
			isset = 0;
		}
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), BP_VAR_IS, varname TSRMLS_CC);
		if (zend_hash_find(target_symbol_table, varname->value.str.val, varname->value.str.len+1, (void **) &value) == FAILURE) {
			isset = 0;
		}
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			if (isset && Z_TYPE_PP(value) == IS_NULL) {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 0;
			} else {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = isset;
			}
			break;
		case ZEND_ISEMPTY:
			if (!isset || !i_zend_is_true(*value)) {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 1;
			} else {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 0;
			}
			break;
	}

	if (varname == &tmp) {
		zval_dtor(&tmp);
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_EXIT_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
#if 0 || (IS_VAR != IS_UNUSED)
	zend_op *opline = EX(opline);
	if (IS_VAR != IS_UNUSED) {
		zend_free_op free_op1;
		zval *ptr = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

		if (Z_TYPE_P(ptr) == IS_LONG) {
			EG(exit_status) = Z_LVAL_P(ptr);
		} else {
			zend_print_variable(ptr);
		}
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	}
#endif
	zend_bailout();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_QM_ASSIGN_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *value = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	EX_T(opline->result.u.var).tmp_var = *value;
	if (!0) {
		zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INSTANCEOF_SPEC_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *expr = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zend_bool result;

	if (Z_TYPE_P(expr) == IS_OBJECT && Z_OBJ_HT_P(expr)->get_class_entry) {
		result = instanceof_function(Z_OBJCE_P(expr), EX_T(opline->op2.u.var).class_entry TSRMLS_CC);
	} else {
		result = 0;
	}
	ZVAL_BOOL(&EX_T(opline->result.u.var).tmp_var, result);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	add_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SUB_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	sub_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MUL_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	mul_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DIV_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	div_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MOD_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	mod_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SL_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SR_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CONCAT_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	concat_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_IDENTICAL_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_IDENTICAL_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_EQUAL_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_EQUAL_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_OR_EQUAL_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_OR_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_AND_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_XOR_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_XOR_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_obj_helper_SPEC_VAR_CONST(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1, free_op_data1;
	zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *object;
	zval *property = &opline->op2.u.constant;
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	znode *result = &opline->result;
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");

		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
	} else {
		/* here we are sure we are dealing with an object */
		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = *zptr;
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;

			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					if (Z_OBJ_HT_P(object)->read_property) {
						z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
				case ZEND_ASSIGN_DIM:
					if (Z_OBJ_HT_P(object)->read_dimension) {
						z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (z->refcount == 0) {
						zval_dtor(z);
						FREE_ZVAL(z);
					}
					z = value;
				}
				z->refcount++;
				SEPARATE_ZVAL_IF_NOT_REF(&z);
				binary_op(z, z, value TSRMLS_CC);
				switch (opline->extended_value) {
					case ZEND_ASSIGN_OBJ:
						Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
						break;
					case ZEND_ASSIGN_DIM:
						Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
						break;
				}
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = z;
					PZVAL_LOCK(*retval);
				}
				zval_ptr_dtor(&z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = EG(uninitialized_zval_ptr);
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (0) {
			zval_ptr_dtor(&property);
		} else {

		}
		FREE_OP(free_op_data1);
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_helper_SPEC_VAR_CONST(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op_data2, free_op_data1;
	zval **var_ptr;
	zval *value;
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			return zend_binary_assign_op_obj_helper_SPEC_VAR_CONST(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

				if (object_ptr && IS_VAR != IS_CV && !(free_op1.var != NULL)) {
					(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
				}

				if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
					return zend_binary_assign_op_obj_helper_SPEC_VAR_CONST(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
				} else {
					zend_op *op_data = opline+1;
					zval *dim = &opline->op2.u.constant;

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), &free_op_data2, BP_VAR_RW);
					increment_opline = 1;
				}
			}
			break;
		default:
			value = &opline->op2.u.constant;
			var_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		if (increment_opline) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	if (increment_opline) {
		ZEND_VM_INC_OPCODE();
		FREE_OP(free_op_data1);
		FREE_OP_VAR_PTR(free_op_data2);
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_ADD_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CONST(add_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SUB_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CONST(sub_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MUL_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CONST(mul_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_DIV_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CONST(div_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MOD_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CONST(mod_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SL_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CONST(shift_left_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SR_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CONST(shift_right_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_CONCAT_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CONST(concat_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_OR_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CONST(bitwise_or_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_AND_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CONST(bitwise_and_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_XOR_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CONST(bitwise_xor_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_pre_incdec_property_helper_SPEC_VAR_CONST(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *object;
	zval *property = &opline->op2.u.constant;
	zval **retval = &EX_T(opline->result.u.var).var.ptr;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");

		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			have_get_ptr = 1;
			incdec_op(*zptr);
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = *zptr;
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			z->refcount++;
			SEPARATE_ZVAL_IF_NOT_REF(&z);
			incdec_op(z);
			*retval = z;
			Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = EG(uninitialized_zval_ptr);
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRE_INC_OBJ_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_VAR_CONST(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_PRE_DEC_OBJ_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_VAR_CONST(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_post_incdec_property_helper_SPEC_VAR_CONST(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *object;
	zval *property = &opline->op2.u.constant;
	zval *retval = &EX_T(opline->result.u.var).tmp_var;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");

		*retval = *EG(uninitialized_zval_ptr);
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			have_get_ptr = 1;
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			*retval = **zptr;
			zendi_zval_copy_ctor(*retval);

			incdec_op(*zptr);

		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
			zval *z_copy;

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			*retval = *z;
			zendi_zval_copy_ctor(*retval);
			ALLOC_ZVAL(z_copy);
			*z_copy = *z;
			zendi_zval_copy_ctor(*z_copy);
			INIT_PZVAL(z_copy);
			incdec_op(z_copy);
			z->refcount++;
			Z_OBJ_HT_P(object)->write_property(object, property, z_copy TSRMLS_CC);
			zval_ptr_dtor(&z_copy);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			*retval = *EG(uninitialized_zval_ptr);
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_POST_INC_OBJ_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_VAR_CONST(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_POST_DEC_OBJ_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_VAR_CONST(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_DIM_R_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *dim = &opline->op2.u.constant;

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK &&
	    IS_VAR != IS_CV &&
	    EX_T(opline->op1.u.var).var.ptr_ptr) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_R TSRMLS_CC);

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_W_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *dim = &opline->op2.u.constant;

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_W TSRMLS_CC);

	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_RW_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *dim = &opline->op2.u.constant;

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_RW TSRMLS_CC);

	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_IS_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *dim = &opline->op2.u.constant;

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_IS TSRMLS_CC);

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_FUNC_ARG_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int type = ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)?BP_VAR_W:BP_VAR_R;
	zval *dim;

	if (IS_CONST == IS_UNUSED && type == BP_VAR_R) {
		zend_error_noreturn(E_ERROR, "Cannot use [] for reading");
	}
	dim = &opline->op2.u.constant;
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, type TSRMLS_CC);

	if (IS_VAR == IS_VAR && type == BP_VAR_W && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_UNSET_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *dim = &opline->op2.u.constant;

	/* Not needed in DIM_UNSET
	if (opline->extended_value == ZEND_FETCH_ADD_LOCK) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	*/
	if (IS_VAR == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, dim, 0, BP_VAR_UNSET TSRMLS_CC);

	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (EX_T(opline->result.u.var).var.ptr_ptr == NULL) {
		zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
	} else {
		zend_free_op free_res;

		PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
		if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
		}
		PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		FREE_OP_VAR_PTR(free_res);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int zend_fetch_property_address_read_helper_SPEC_VAR_CONST(int type, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *container;
	zval **retval;
	zend_free_op free_op1;

	retval = &EX_T(opline->result.u.var).var.ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = retval;

	container = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (container == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(error_zval_ptr);
			PZVAL_LOCK(*retval);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	}


	if (Z_TYPE_P(container) != IS_OBJECT || !Z_OBJ_HT_P(container)->read_property) {
		if (type != BP_VAR_IS) {
			zend_error(E_NOTICE, "Trying to get property of non-object");
		}
		*retval = EG(uninitialized_zval_ptr);
		SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	} else {

		zval *offset  = &opline->op2.u.constant;

		if (0) {
			MAKE_REAL_ZVAL_PTR(offset);
		}

		/* here we are sure we are dealing with an object */
		*retval = Z_OBJ_HT_P(container)->read_property(container, offset, type TSRMLS_CC);

		if (RETURN_VALUE_UNUSED(&opline->result) && ((*retval)->refcount == 0)) {
			zval_dtor(*retval);
			FREE_ZVAL(*retval);
		} else {
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		if (0) {
			zval_ptr_dtor(&offset);
		} else {

		}
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_R_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_VAR_CONST(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_W_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *property = &opline->op2.u.constant;

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK && IS_VAR != IS_CV) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
		EX_T(opline->op1.u.var).var.ptr = *EX_T(opline->op1.u.var).var.ptr_ptr;
	}

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), property, BP_VAR_W TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_RW_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *property = &opline->op2.u.constant;

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), property, BP_VAR_RW TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_IS_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_VAR_CONST(BP_VAR_IS, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_FUNC_ARG_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		/* Behave like FETCH_OBJ_W */
		zend_free_op free_op1;
		zval *property = &opline->op2.u.constant;

		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}
		zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), property, BP_VAR_W TSRMLS_CC);
		if (0) {
			zval_ptr_dtor(&property);
		} else {

		}
		if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
		    READY_TO_DESTROY(free_op1.var) &&
		    !RETURN_VALUE_UNUSED(&opline->result)) {
			AI_USE_PTR(EX_T(opline->result.u.var).var);
			if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
			    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
				SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
			}
		}
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	} else {
		return zend_fetch_property_address_read_helper_SPEC_VAR_CONST(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}
}

static int ZEND_FETCH_OBJ_UNSET_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_res;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *property = &opline->op2.u.constant;

	if (IS_VAR == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, property, BP_VAR_UNSET TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
	if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
		SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
	}
	PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	FREE_OP_VAR_PTR(free_res);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_OBJ_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1;
	zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_OBJ TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_DIM_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1;
	zval **object_ptr;

	if (IS_VAR == IS_CV || EX_T(opline->op1.u.var).var.ptr_ptr) {
		/* not an array offset */
		object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	} else {
		object_ptr = NULL;
	}

	if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
		zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_DIM TSRMLS_CC);
	} else {
		zend_free_op free_op_data1;
		zval *value;
		zval *dim = &opline->op2.u.constant;

		zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), object_ptr, dim, 0, BP_VAR_W TSRMLS_CC);

		value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	 	zend_assign_to_variable(&opline->result, &op_data->op2, &op_data->op1, value, (IS_TMP_FREE(free_op_data1)?IS_TMP_VAR:op_data->op1.op_type), EX(Ts) TSRMLS_CC);
	 	FREE_OP_IF_VAR(free_op_data1);
	}
 	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	/* assign_dim has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *value = &opline->op2.u.constant;

 	zend_assign_to_variable(&opline->result, &opline->op1, &opline->op2, value, (0?IS_TMP_VAR:IS_CONST), EX(Ts) TSRMLS_CC);
	/* zend_assign_to_variable() always takes care of op2, never free it! */

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_METHOD_CALL_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;
	zend_free_op free_op1;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	function_name = &opline->op2.u.constant;

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = Z_STRVAL_P(function_name);
	function_name_strlen = Z_STRLEN_P(function_name);

	EX(object) = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (EX(object) && Z_TYPE_P(EX(object)) == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(&EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CASE_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;
	zend_free_op free_op1;

	if (IS_VAR==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
				 &opline->op2.u.constant TSRMLS_CC);

	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=&opline->op2.u.constant;

#if 0 || IS_VAR == IS_VAR || IS_VAR == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=_get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	}
#else
	expr_ptr=_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
#endif

	if (0) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_VAR == IS_VAR || IS_VAR == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}

	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	} else {
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_VAR == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_VAR != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_UNSET_DIM_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *offset = &opline->op2.u.constant;
	long index;

	if (container) {
		if (IS_VAR == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		switch (Z_TYPE_PP(container)) {
			case IS_ARRAY: {
				HashTable *ht = Z_ARRVAL_PP(container);

				switch (Z_TYPE_P(offset)) {
					case IS_DOUBLE:
						index = (long) Z_DVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_RESOURCE:
					case IS_BOOL:
					case IS_LONG:
						index = Z_LVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_STRING:
						if (IS_CONST == IS_CV || IS_CONST == IS_VAR) {
							offset->refcount++;
						}
						if (zend_symtable_del(ht, offset->value.str.val, offset->value.str.len+1) == SUCCESS &&
					    ht == &EG(symbol_table)) {
							zend_execute_data *ex;
							ulong hash_value = zend_inline_hash_func(offset->value.str.val, offset->value.str.len+1);

							for (ex = execute_data; ex; ex = ex->prev_execute_data) {
								if (ex->op_array && ex->symbol_table == ht) {
									int i;

									for (i = 0; i < ex->op_array->last_var; i++) {
										if (ex->op_array->vars[i].hash_value == hash_value &&
										    ex->op_array->vars[i].name_len == offset->value.str.len &&
										    !memcmp(ex->op_array->vars[i].name, offset->value.str.val, offset->value.str.len)) {
											ex->CVs[i] = NULL;
											break;
										}
									}
								}
							}
						}
						if (IS_CONST == IS_CV || IS_CONST == IS_VAR) {
							zval_ptr_dtor(&offset);
						}
						break;
					case IS_NULL:
						zend_hash_del(ht, "", sizeof(""));
						break;
					default:
						zend_error(E_WARNING, "Illegal offset type in unset");
						break;
				}

				break;
			}
			case IS_OBJECT:
				if (!Z_OBJ_HT_P(*container)->unset_dimension) {
					zend_error_noreturn(E_ERROR, "Cannot use object as array");
				}
				if (0) {
					MAKE_REAL_ZVAL_PTR(offset);
				}
				Z_OBJ_HT_P(*container)->unset_dimension(*container, offset TSRMLS_CC);
				if (0) {
					zval_ptr_dtor(&offset);
				} else {

				}
				break;
			case IS_STRING:
				zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
				ZEND_VM_CONTINUE(); /* bailed out before */
			default:

				break;
		}
	} else {

	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_UNSET_OBJ_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *offset = &opline->op2.u.constant;

	if (container) {
		if (IS_VAR == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			Z_OBJ_HT_P(*container)->unset_property(*container, offset TSRMLS_CC);
			if (0) {
				zval_ptr_dtor(&offset);
			} else {

			}
		} else {

		}
	} else {

	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int zend_isset_isempty_dim_prop_obj_handler_SPEC_VAR_CONST(int prop_dim, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval **value = NULL;
	int result = 0;
	long index;

	if (container) {

		zval *offset = &opline->op2.u.constant;

		if (Z_TYPE_PP(container) == IS_ARRAY) {
			HashTable *ht;
			int isset = 0;

			ht = Z_ARRVAL_PP(container);

			switch (Z_TYPE_P(offset)) {
				case IS_DOUBLE:
					index = (long) Z_DVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_RESOURCE:
				case IS_BOOL:
				case IS_LONG:
					index = Z_LVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_STRING:
					if (zend_symtable_find(ht, offset->value.str.val, offset->value.str.len+1, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_NULL:
					if (zend_hash_find(ht, "", sizeof(""), (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				default:
					zend_error(E_WARNING, "Illegal offset type in isset or empty");

					break;
			}

			switch (opline->extended_value) {
				case ZEND_ISSET:
					if (isset && Z_TYPE_PP(value) == IS_NULL) {
						result = 0;
					} else {
						result = isset;
					}
					break;
				case ZEND_ISEMPTY:
					if (!isset || !i_zend_is_true(*value)) {
						result = 0;
					} else {
						result = 1;
					}
					break;
			}

		} else if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			if (prop_dim) {
				result = Z_OBJ_HT_P(*container)->has_property(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			} else {
				result = Z_OBJ_HT_P(*container)->has_dimension(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			}
			if (0) {
				zval_ptr_dtor(&offset);
			} else {

			}
		} else if ((*container)->type == IS_STRING && !prop_dim) { /* string offsets */
			zval tmp;

			if (Z_TYPE_P(offset) != IS_LONG) {
				tmp = *offset;
				zval_copy_ctor(&tmp);
				convert_to_long(&tmp);
				offset = &tmp;
			}
			if (Z_TYPE_P(offset) == IS_LONG) {
				switch (opline->extended_value) {
					case ZEND_ISSET:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container)) {
							result = 1;
						}
						break;
					case ZEND_ISEMPTY:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container) && Z_STRVAL_PP(container)[offset->value.lval] != '0') {
							result = 1;
						}
						break;
				}
			}

		} else {

		}
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = result;
			break;
		case ZEND_ISEMPTY:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = !result;
			break;
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_VAR_CONST(0, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_VAR_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_VAR_CONST(1, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ADD_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	add_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SUB_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	sub_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MUL_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	mul_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DIV_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	div_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MOD_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	mod_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SL_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SR_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CONCAT_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	concat_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_IDENTICAL_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_IDENTICAL_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_EQUAL_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_EQUAL_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_OR_EQUAL_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_OR_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_AND_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_XOR_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_XOR_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_obj_helper_SPEC_VAR_TMP(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1, free_op2, free_op_data1;
	zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	znode *result = &opline->result;
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");
		zval_dtor(free_op2.var);
		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
	} else {
		/* here we are sure we are dealing with an object */
		if (1) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = *zptr;
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;

			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					if (Z_OBJ_HT_P(object)->read_property) {
						z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
				case ZEND_ASSIGN_DIM:
					if (Z_OBJ_HT_P(object)->read_dimension) {
						z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (z->refcount == 0) {
						zval_dtor(z);
						FREE_ZVAL(z);
					}
					z = value;
				}
				z->refcount++;
				SEPARATE_ZVAL_IF_NOT_REF(&z);
				binary_op(z, z, value TSRMLS_CC);
				switch (opline->extended_value) {
					case ZEND_ASSIGN_OBJ:
						Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
						break;
					case ZEND_ASSIGN_DIM:
						Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
						break;
				}
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = z;
					PZVAL_LOCK(*retval);
				}
				zval_ptr_dtor(&z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = EG(uninitialized_zval_ptr);
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (1) {
			zval_ptr_dtor(&property);
		} else {
			zval_dtor(free_op2.var);
		}
		FREE_OP(free_op_data1);
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_helper_SPEC_VAR_TMP(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2, free_op_data2, free_op_data1;
	zval **var_ptr;
	zval *value;
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			return zend_binary_assign_op_obj_helper_SPEC_VAR_TMP(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

				if (object_ptr && IS_VAR != IS_CV && !(free_op1.var != NULL)) {
					(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
				}

				if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
					return zend_binary_assign_op_obj_helper_SPEC_VAR_TMP(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
				} else {
					zend_op *op_data = opline+1;
					zval *dim = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 1, BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), &free_op_data2, BP_VAR_RW);
					increment_opline = 1;
				}
			}
			break;
		default:
			value = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
			var_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		zval_dtor(free_op2.var);
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		if (increment_opline) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}
	zval_dtor(free_op2.var);

	if (increment_opline) {
		ZEND_VM_INC_OPCODE();
		FREE_OP(free_op_data1);
		FREE_OP_VAR_PTR(free_op_data2);
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_ADD_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_TMP(add_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SUB_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_TMP(sub_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MUL_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_TMP(mul_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_DIV_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_TMP(div_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MOD_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_TMP(mod_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SL_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_TMP(shift_left_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SR_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_TMP(shift_right_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_CONCAT_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_TMP(concat_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_OR_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_TMP(bitwise_or_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_AND_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_TMP(bitwise_and_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_XOR_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_TMP(bitwise_xor_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_pre_incdec_property_helper_SPEC_VAR_TMP(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval **retval = &EX_T(opline->result.u.var).var.ptr;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		zval_dtor(free_op2.var);
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (1) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			have_get_ptr = 1;
			incdec_op(*zptr);
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = *zptr;
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			z->refcount++;
			SEPARATE_ZVAL_IF_NOT_REF(&z);
			incdec_op(z);
			*retval = z;
			Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = EG(uninitialized_zval_ptr);
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (1) {
		zval_ptr_dtor(&property);
	} else {
		zval_dtor(free_op2.var);
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRE_INC_OBJ_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_VAR_TMP(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_PRE_DEC_OBJ_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_VAR_TMP(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_post_incdec_property_helper_SPEC_VAR_TMP(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval *retval = &EX_T(opline->result.u.var).tmp_var;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		zval_dtor(free_op2.var);
		*retval = *EG(uninitialized_zval_ptr);
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (1) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			have_get_ptr = 1;
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			*retval = **zptr;
			zendi_zval_copy_ctor(*retval);

			incdec_op(*zptr);

		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
			zval *z_copy;

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			*retval = *z;
			zendi_zval_copy_ctor(*retval);
			ALLOC_ZVAL(z_copy);
			*z_copy = *z;
			zendi_zval_copy_ctor(*z_copy);
			INIT_PZVAL(z_copy);
			incdec_op(z_copy);
			z->refcount++;
			Z_OBJ_HT_P(object)->write_property(object, property, z_copy TSRMLS_CC);
			zval_ptr_dtor(&z_copy);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			*retval = *EG(uninitialized_zval_ptr);
		}
	}

	if (1) {
		zval_ptr_dtor(&property);
	} else {
		zval_dtor(free_op2.var);
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_POST_INC_OBJ_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_VAR_TMP(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_POST_DEC_OBJ_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_VAR_TMP(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_DIM_R_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK &&
	    IS_VAR != IS_CV &&
	    EX_T(opline->op1.u.var).var.ptr_ptr) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 1, BP_VAR_R TSRMLS_CC);
	zval_dtor(free_op2.var);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_W_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 1, BP_VAR_W TSRMLS_CC);
	zval_dtor(free_op2.var);
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_RW_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 1, BP_VAR_RW TSRMLS_CC);
	zval_dtor(free_op2.var);
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_IS_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 1, BP_VAR_IS TSRMLS_CC);
	zval_dtor(free_op2.var);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_FUNC_ARG_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	int type = ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)?BP_VAR_W:BP_VAR_R;
	zval *dim;

	if (IS_TMP_VAR == IS_UNUSED && type == BP_VAR_R) {
		zend_error_noreturn(E_ERROR, "Cannot use [] for reading");
	}
	dim = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 1, type TSRMLS_CC);
	zval_dtor(free_op2.var);
	if (IS_VAR == IS_VAR && type == BP_VAR_W && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_UNSET_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *dim = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	/* Not needed in DIM_UNSET
	if (opline->extended_value == ZEND_FETCH_ADD_LOCK) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	*/
	if (IS_VAR == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, dim, 1, BP_VAR_UNSET TSRMLS_CC);
	zval_dtor(free_op2.var);
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (EX_T(opline->result.u.var).var.ptr_ptr == NULL) {
		zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
	} else {
		zend_free_op free_res;

		PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
		if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
		}
		PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		FREE_OP_VAR_PTR(free_res);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int zend_fetch_property_address_read_helper_SPEC_VAR_TMP(int type, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *container;
	zval **retval;
	zend_free_op free_op1;

	retval = &EX_T(opline->result.u.var).var.ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = retval;

	container = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (container == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(error_zval_ptr);
			PZVAL_LOCK(*retval);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	}


	if (Z_TYPE_P(container) != IS_OBJECT || !Z_OBJ_HT_P(container)->read_property) {
		if (type != BP_VAR_IS) {
			zend_error(E_NOTICE, "Trying to get property of non-object");
		}
		*retval = EG(uninitialized_zval_ptr);
		SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	} else {
		zend_free_op free_op2;
		zval *offset  = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (1) {
			MAKE_REAL_ZVAL_PTR(offset);
		}

		/* here we are sure we are dealing with an object */
		*retval = Z_OBJ_HT_P(container)->read_property(container, offset, type TSRMLS_CC);

		if (RETURN_VALUE_UNUSED(&opline->result) && ((*retval)->refcount == 0)) {
			zval_dtor(*retval);
			FREE_ZVAL(*retval);
		} else {
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		if (1) {
			zval_ptr_dtor(&offset);
		} else {
			zval_dtor(free_op2.var);
		}
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_R_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_VAR_TMP(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_W_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK && IS_VAR != IS_CV) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
		EX_T(opline->op1.u.var).var.ptr = *EX_T(opline->op1.u.var).var.ptr_ptr;
	}

	if (1) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), property, BP_VAR_W TSRMLS_CC);
	if (1) {
		zval_ptr_dtor(&property);
	} else {
		zval_dtor(free_op2.var);
	}
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_RW_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (1) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), property, BP_VAR_RW TSRMLS_CC);
	if (1) {
		zval_ptr_dtor(&property);
	} else {
		zval_dtor(free_op2.var);
	}
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_IS_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_VAR_TMP(BP_VAR_IS, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_FUNC_ARG_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		/* Behave like FETCH_OBJ_W */
		zend_free_op free_op1, free_op2;
		zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (1) {
			MAKE_REAL_ZVAL_PTR(property);
		}
		zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), property, BP_VAR_W TSRMLS_CC);
		if (1) {
			zval_ptr_dtor(&property);
		} else {
			zval_dtor(free_op2.var);
		}
		if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
		    READY_TO_DESTROY(free_op1.var) &&
		    !RETURN_VALUE_UNUSED(&opline->result)) {
			AI_USE_PTR(EX_T(opline->result.u.var).var);
			if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
			    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
				SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
			}
		}
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	} else {
		return zend_fetch_property_address_read_helper_SPEC_VAR_TMP(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}
}

static int ZEND_FETCH_OBJ_UNSET_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2, free_res;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (IS_VAR == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	if (1) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, property, BP_VAR_UNSET TSRMLS_CC);
	if (1) {
		zval_ptr_dtor(&property);
	} else {
		zval_dtor(free_op2.var);
	}
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
	if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
		SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
	}
	PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	FREE_OP_VAR_PTR(free_res);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_OBJ_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1;
	zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_OBJ TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_DIM_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1;
	zval **object_ptr;

	if (IS_VAR == IS_CV || EX_T(opline->op1.u.var).var.ptr_ptr) {
		/* not an array offset */
		object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	} else {
		object_ptr = NULL;
	}

	if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
		zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_DIM TSRMLS_CC);
	} else {
		zend_free_op free_op2, free_op_data1;
		zval *value;
		zval *dim = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), object_ptr, dim, 1, BP_VAR_W TSRMLS_CC);
		zval_dtor(free_op2.var);

		value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	 	zend_assign_to_variable(&opline->result, &op_data->op2, &op_data->op1, value, (IS_TMP_FREE(free_op_data1)?IS_TMP_VAR:op_data->op1.op_type), EX(Ts) TSRMLS_CC);
	 	FREE_OP_IF_VAR(free_op_data1);
	}
 	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	/* assign_dim has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval *value = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

 	zend_assign_to_variable(&opline->result, &opline->op1, &opline->op2, value, (1?IS_TMP_VAR:IS_TMP_VAR), EX(Ts) TSRMLS_CC);
	/* zend_assign_to_variable() always takes care of op2, never free it! */

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_METHOD_CALL_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;
	zend_free_op free_op1, free_op2;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	function_name = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = Z_STRVAL_P(function_name);
	function_name_strlen = Z_STRLEN_P(function_name);

	EX(object) = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (EX(object) && Z_TYPE_P(EX(object)) == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(&EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}

	zval_dtor(free_op2.var);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CASE_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;
	zend_free_op free_op1, free_op2;

	if (IS_VAR==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
				 _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

#if 0 || IS_VAR == IS_VAR || IS_VAR == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=_get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	}
#else
	expr_ptr=_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
#endif

	if (0) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_VAR == IS_VAR || IS_VAR == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}
		zval_dtor(free_op2.var);
	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	} else {
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_VAR == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_VAR != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_UNSET_DIM_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *offset = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	long index;

	if (container) {
		if (IS_VAR == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		switch (Z_TYPE_PP(container)) {
			case IS_ARRAY: {
				HashTable *ht = Z_ARRVAL_PP(container);

				switch (Z_TYPE_P(offset)) {
					case IS_DOUBLE:
						index = (long) Z_DVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_RESOURCE:
					case IS_BOOL:
					case IS_LONG:
						index = Z_LVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_STRING:
						if (IS_TMP_VAR == IS_CV || IS_TMP_VAR == IS_VAR) {
							offset->refcount++;
						}
						if (zend_symtable_del(ht, offset->value.str.val, offset->value.str.len+1) == SUCCESS &&
					    ht == &EG(symbol_table)) {
							zend_execute_data *ex;
							ulong hash_value = zend_inline_hash_func(offset->value.str.val, offset->value.str.len+1);

							for (ex = execute_data; ex; ex = ex->prev_execute_data) {
								if (ex->op_array && ex->symbol_table == ht) {
									int i;

									for (i = 0; i < ex->op_array->last_var; i++) {
										if (ex->op_array->vars[i].hash_value == hash_value &&
										    ex->op_array->vars[i].name_len == offset->value.str.len &&
										    !memcmp(ex->op_array->vars[i].name, offset->value.str.val, offset->value.str.len)) {
											ex->CVs[i] = NULL;
											break;
										}
									}
								}
							}
						}
						if (IS_TMP_VAR == IS_CV || IS_TMP_VAR == IS_VAR) {
							zval_ptr_dtor(&offset);
						}
						break;
					case IS_NULL:
						zend_hash_del(ht, "", sizeof(""));
						break;
					default:
						zend_error(E_WARNING, "Illegal offset type in unset");
						break;
				}
				zval_dtor(free_op2.var);
				break;
			}
			case IS_OBJECT:
				if (!Z_OBJ_HT_P(*container)->unset_dimension) {
					zend_error_noreturn(E_ERROR, "Cannot use object as array");
				}
				if (1) {
					MAKE_REAL_ZVAL_PTR(offset);
				}
				Z_OBJ_HT_P(*container)->unset_dimension(*container, offset TSRMLS_CC);
				if (1) {
					zval_ptr_dtor(&offset);
				} else {
					zval_dtor(free_op2.var);
				}
				break;
			case IS_STRING:
				zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
				ZEND_VM_CONTINUE(); /* bailed out before */
			default:
				zval_dtor(free_op2.var);
				break;
		}
	} else {
		zval_dtor(free_op2.var);
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_UNSET_OBJ_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *offset = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (container) {
		if (IS_VAR == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (1) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			Z_OBJ_HT_P(*container)->unset_property(*container, offset TSRMLS_CC);
			if (1) {
				zval_ptr_dtor(&offset);
			} else {
				zval_dtor(free_op2.var);
			}
		} else {
			zval_dtor(free_op2.var);
		}
	} else {
		zval_dtor(free_op2.var);
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int zend_isset_isempty_dim_prop_obj_handler_SPEC_VAR_TMP(int prop_dim, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval **value = NULL;
	int result = 0;
	long index;

	if (container) {
		zend_free_op free_op2;
		zval *offset = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (Z_TYPE_PP(container) == IS_ARRAY) {
			HashTable *ht;
			int isset = 0;

			ht = Z_ARRVAL_PP(container);

			switch (Z_TYPE_P(offset)) {
				case IS_DOUBLE:
					index = (long) Z_DVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_RESOURCE:
				case IS_BOOL:
				case IS_LONG:
					index = Z_LVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_STRING:
					if (zend_symtable_find(ht, offset->value.str.val, offset->value.str.len+1, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_NULL:
					if (zend_hash_find(ht, "", sizeof(""), (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				default:
					zend_error(E_WARNING, "Illegal offset type in isset or empty");

					break;
			}

			switch (opline->extended_value) {
				case ZEND_ISSET:
					if (isset && Z_TYPE_PP(value) == IS_NULL) {
						result = 0;
					} else {
						result = isset;
					}
					break;
				case ZEND_ISEMPTY:
					if (!isset || !i_zend_is_true(*value)) {
						result = 0;
					} else {
						result = 1;
					}
					break;
			}
			zval_dtor(free_op2.var);
		} else if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (1) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			if (prop_dim) {
				result = Z_OBJ_HT_P(*container)->has_property(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			} else {
				result = Z_OBJ_HT_P(*container)->has_dimension(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			}
			if (1) {
				zval_ptr_dtor(&offset);
			} else {
				zval_dtor(free_op2.var);
			}
		} else if ((*container)->type == IS_STRING && !prop_dim) { /* string offsets */
			zval tmp;

			if (Z_TYPE_P(offset) != IS_LONG) {
				tmp = *offset;
				zval_copy_ctor(&tmp);
				convert_to_long(&tmp);
				offset = &tmp;
			}
			if (Z_TYPE_P(offset) == IS_LONG) {
				switch (opline->extended_value) {
					case ZEND_ISSET:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container)) {
							result = 1;
						}
						break;
					case ZEND_ISEMPTY:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container) && Z_STRVAL_PP(container)[offset->value.lval] != '0') {
							result = 1;
						}
						break;
				}
			}
			zval_dtor(free_op2.var);
		} else {
			zval_dtor(free_op2.var);
		}
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = result;
			break;
		case ZEND_ISEMPTY:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = !result;
			break;
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_VAR_TMP(0, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_VAR_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_VAR_TMP(1, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ADD_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	add_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SUB_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	sub_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MUL_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	mul_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DIV_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	div_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MOD_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	mod_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SL_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SR_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CONCAT_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	concat_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_IDENTICAL_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_IDENTICAL_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_EQUAL_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_EQUAL_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_OR_EQUAL_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_OR_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_AND_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_XOR_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_XOR_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_obj_helper_SPEC_VAR_VAR(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1, free_op2, free_op_data1;
	zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	znode *result = &opline->result;
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
	} else {
		/* here we are sure we are dealing with an object */
		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = *zptr;
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;

			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					if (Z_OBJ_HT_P(object)->read_property) {
						z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
				case ZEND_ASSIGN_DIM:
					if (Z_OBJ_HT_P(object)->read_dimension) {
						z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (z->refcount == 0) {
						zval_dtor(z);
						FREE_ZVAL(z);
					}
					z = value;
				}
				z->refcount++;
				SEPARATE_ZVAL_IF_NOT_REF(&z);
				binary_op(z, z, value TSRMLS_CC);
				switch (opline->extended_value) {
					case ZEND_ASSIGN_OBJ:
						Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
						break;
					case ZEND_ASSIGN_DIM:
						Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
						break;
				}
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = z;
					PZVAL_LOCK(*retval);
				}
				zval_ptr_dtor(&z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = EG(uninitialized_zval_ptr);
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (0) {
			zval_ptr_dtor(&property);
		} else {
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		}
		FREE_OP(free_op_data1);
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_helper_SPEC_VAR_VAR(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2, free_op_data2, free_op_data1;
	zval **var_ptr;
	zval *value;
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			return zend_binary_assign_op_obj_helper_SPEC_VAR_VAR(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

				if (object_ptr && IS_VAR != IS_CV && !(free_op1.var != NULL)) {
					(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
				}

				if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
					return zend_binary_assign_op_obj_helper_SPEC_VAR_VAR(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
				} else {
					zend_op *op_data = opline+1;
					zval *dim = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), &free_op_data2, BP_VAR_RW);
					increment_opline = 1;
				}
			}
			break;
		default:
			value = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
			var_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		if (increment_opline) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};

	if (increment_opline) {
		ZEND_VM_INC_OPCODE();
		FREE_OP(free_op_data1);
		FREE_OP_VAR_PTR(free_op_data2);
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_ADD_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_VAR(add_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SUB_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_VAR(sub_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MUL_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_VAR(mul_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_DIV_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_VAR(div_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MOD_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_VAR(mod_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SL_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_VAR(shift_left_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SR_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_VAR(shift_right_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_CONCAT_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_VAR(concat_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_OR_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_VAR(bitwise_or_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_AND_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_VAR(bitwise_and_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_XOR_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_VAR(bitwise_xor_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_pre_incdec_property_helper_SPEC_VAR_VAR(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval **retval = &EX_T(opline->result.u.var).var.ptr;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			have_get_ptr = 1;
			incdec_op(*zptr);
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = *zptr;
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			z->refcount++;
			SEPARATE_ZVAL_IF_NOT_REF(&z);
			incdec_op(z);
			*retval = z;
			Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = EG(uninitialized_zval_ptr);
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRE_INC_OBJ_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_VAR_VAR(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_PRE_DEC_OBJ_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_VAR_VAR(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_post_incdec_property_helper_SPEC_VAR_VAR(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval *retval = &EX_T(opline->result.u.var).tmp_var;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		*retval = *EG(uninitialized_zval_ptr);
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			have_get_ptr = 1;
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			*retval = **zptr;
			zendi_zval_copy_ctor(*retval);

			incdec_op(*zptr);

		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
			zval *z_copy;

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			*retval = *z;
			zendi_zval_copy_ctor(*retval);
			ALLOC_ZVAL(z_copy);
			*z_copy = *z;
			zendi_zval_copy_ctor(*z_copy);
			INIT_PZVAL(z_copy);
			incdec_op(z_copy);
			z->refcount++;
			Z_OBJ_HT_P(object)->write_property(object, property, z_copy TSRMLS_CC);
			zval_ptr_dtor(&z_copy);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			*retval = *EG(uninitialized_zval_ptr);
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_POST_INC_OBJ_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_VAR_VAR(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_POST_DEC_OBJ_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_VAR_VAR(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_DIM_R_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK &&
	    IS_VAR != IS_CV &&
	    EX_T(opline->op1.u.var).var.ptr_ptr) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_R TSRMLS_CC);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_W_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_W TSRMLS_CC);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_RW_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_RW TSRMLS_CC);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_IS_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_IS TSRMLS_CC);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_FUNC_ARG_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	int type = ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)?BP_VAR_W:BP_VAR_R;
	zval *dim;

	if (IS_VAR == IS_UNUSED && type == BP_VAR_R) {
		zend_error_noreturn(E_ERROR, "Cannot use [] for reading");
	}
	dim = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, type TSRMLS_CC);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	if (IS_VAR == IS_VAR && type == BP_VAR_W && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_UNSET_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *dim = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	/* Not needed in DIM_UNSET
	if (opline->extended_value == ZEND_FETCH_ADD_LOCK) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	*/
	if (IS_VAR == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, dim, 0, BP_VAR_UNSET TSRMLS_CC);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (EX_T(opline->result.u.var).var.ptr_ptr == NULL) {
		zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
	} else {
		zend_free_op free_res;

		PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
		if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
		}
		PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		FREE_OP_VAR_PTR(free_res);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int zend_fetch_property_address_read_helper_SPEC_VAR_VAR(int type, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *container;
	zval **retval;
	zend_free_op free_op1;

	retval = &EX_T(opline->result.u.var).var.ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = retval;

	container = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (container == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(error_zval_ptr);
			PZVAL_LOCK(*retval);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	}


	if (Z_TYPE_P(container) != IS_OBJECT || !Z_OBJ_HT_P(container)->read_property) {
		if (type != BP_VAR_IS) {
			zend_error(E_NOTICE, "Trying to get property of non-object");
		}
		*retval = EG(uninitialized_zval_ptr);
		SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	} else {
		zend_free_op free_op2;
		zval *offset  = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (0) {
			MAKE_REAL_ZVAL_PTR(offset);
		}

		/* here we are sure we are dealing with an object */
		*retval = Z_OBJ_HT_P(container)->read_property(container, offset, type TSRMLS_CC);

		if (RETURN_VALUE_UNUSED(&opline->result) && ((*retval)->refcount == 0)) {
			zval_dtor(*retval);
			FREE_ZVAL(*retval);
		} else {
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		if (0) {
			zval_ptr_dtor(&offset);
		} else {
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		}
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_R_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_VAR_VAR(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_W_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK && IS_VAR != IS_CV) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
		EX_T(opline->op1.u.var).var.ptr = *EX_T(opline->op1.u.var).var.ptr_ptr;
	}

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), property, BP_VAR_W TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_RW_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), property, BP_VAR_RW TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_IS_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_VAR_VAR(BP_VAR_IS, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_FUNC_ARG_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		/* Behave like FETCH_OBJ_W */
		zend_free_op free_op1, free_op2;
		zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}
		zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), property, BP_VAR_W TSRMLS_CC);
		if (0) {
			zval_ptr_dtor(&property);
		} else {
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		}
		if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
		    READY_TO_DESTROY(free_op1.var) &&
		    !RETURN_VALUE_UNUSED(&opline->result)) {
			AI_USE_PTR(EX_T(opline->result.u.var).var);
			if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
			    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
				SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
			}
		}
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	} else {
		return zend_fetch_property_address_read_helper_SPEC_VAR_VAR(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}
}

static int ZEND_FETCH_OBJ_UNSET_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2, free_res;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (IS_VAR == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, property, BP_VAR_UNSET TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
	if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
		SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
	}
	PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	FREE_OP_VAR_PTR(free_res);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_OBJ_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1;
	zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_OBJ TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_DIM_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1;
	zval **object_ptr;

	if (IS_VAR == IS_CV || EX_T(opline->op1.u.var).var.ptr_ptr) {
		/* not an array offset */
		object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	} else {
		object_ptr = NULL;
	}

	if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
		zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_DIM TSRMLS_CC);
	} else {
		zend_free_op free_op2, free_op_data1;
		zval *value;
		zval *dim = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), object_ptr, dim, 0, BP_VAR_W TSRMLS_CC);
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};

		value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	 	zend_assign_to_variable(&opline->result, &op_data->op2, &op_data->op1, value, (IS_TMP_FREE(free_op_data1)?IS_TMP_VAR:op_data->op1.op_type), EX(Ts) TSRMLS_CC);
	 	FREE_OP_IF_VAR(free_op_data1);
	}
 	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	/* assign_dim has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval *value = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

 	zend_assign_to_variable(&opline->result, &opline->op1, &opline->op2, value, (0?IS_TMP_VAR:IS_VAR), EX(Ts) TSRMLS_CC);
	/* zend_assign_to_variable() always takes care of op2, never free it! */
 	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_REF_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **variable_ptr_ptr;
	zval **value_ptr_ptr = _get_zval_ptr_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (IS_VAR == IS_VAR &&
	    value_ptr_ptr &&
	    !(*value_ptr_ptr)->is_ref &&
	    opline->extended_value == ZEND_RETURNS_FUNCTION &&
	    !EX_T(opline->op2.u.var).var.fcall_returned_reference) {
		if (free_op2.var == NULL) {
			PZVAL_LOCK(*value_ptr_ptr); /* undo the effect of get_zval_ptr_ptr() */
		}
		zend_error(E_STRICT, "Only variables should be assigned by reference");
		if (EG(exception)) {
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
			ZEND_VM_NEXT_OPCODE();
		}
		return ZEND_ASSIGN_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	} else if (IS_VAR == IS_VAR && opline->extended_value == ZEND_RETURNS_NEW) {
		PZVAL_LOCK(*value_ptr_ptr);
	}
	if (IS_VAR == IS_VAR && EX_T(opline->op1.u.var).var.ptr_ptr == &EX_T(opline->op1.u.var).var.ptr) {
		zend_error(E_ERROR, "Cannot assign by reference to overloaded object");
	}

	variable_ptr_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zend_assign_to_variable_reference(variable_ptr_ptr, value_ptr_ptr TSRMLS_CC);

	if (IS_VAR == IS_VAR && opline->extended_value == ZEND_RETURNS_NEW) {
		(*variable_ptr_ptr)->refcount--;
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = variable_ptr_ptr;
		PZVAL_LOCK(*variable_ptr_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_METHOD_CALL_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;
	zend_free_op free_op1, free_op2;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	function_name = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = Z_STRVAL_P(function_name);
	function_name_strlen = Z_STRLEN_P(function_name);

	EX(object) = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (EX(object) && Z_TYPE_P(EX(object)) == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(&EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CASE_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;
	zend_free_op free_op1, free_op2;

	if (IS_VAR==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
				 _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

#if 0 || IS_VAR == IS_VAR || IS_VAR == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=_get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	}
#else
	expr_ptr=_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
#endif

	if (0) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_VAR == IS_VAR || IS_VAR == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	} else {
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_VAR == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_VAR != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_UNSET_DIM_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *offset = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	long index;

	if (container) {
		if (IS_VAR == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		switch (Z_TYPE_PP(container)) {
			case IS_ARRAY: {
				HashTable *ht = Z_ARRVAL_PP(container);

				switch (Z_TYPE_P(offset)) {
					case IS_DOUBLE:
						index = (long) Z_DVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_RESOURCE:
					case IS_BOOL:
					case IS_LONG:
						index = Z_LVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_STRING:
						if (IS_VAR == IS_CV || IS_VAR == IS_VAR) {
							offset->refcount++;
						}
						if (zend_symtable_del(ht, offset->value.str.val, offset->value.str.len+1) == SUCCESS &&
					    ht == &EG(symbol_table)) {
							zend_execute_data *ex;
							ulong hash_value = zend_inline_hash_func(offset->value.str.val, offset->value.str.len+1);

							for (ex = execute_data; ex; ex = ex->prev_execute_data) {
								if (ex->op_array && ex->symbol_table == ht) {
									int i;

									for (i = 0; i < ex->op_array->last_var; i++) {
										if (ex->op_array->vars[i].hash_value == hash_value &&
										    ex->op_array->vars[i].name_len == offset->value.str.len &&
										    !memcmp(ex->op_array->vars[i].name, offset->value.str.val, offset->value.str.len)) {
											ex->CVs[i] = NULL;
											break;
										}
									}
								}
							}
						}
						if (IS_VAR == IS_CV || IS_VAR == IS_VAR) {
							zval_ptr_dtor(&offset);
						}
						break;
					case IS_NULL:
						zend_hash_del(ht, "", sizeof(""));
						break;
					default:
						zend_error(E_WARNING, "Illegal offset type in unset");
						break;
				}
				if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
				break;
			}
			case IS_OBJECT:
				if (!Z_OBJ_HT_P(*container)->unset_dimension) {
					zend_error_noreturn(E_ERROR, "Cannot use object as array");
				}
				if (0) {
					MAKE_REAL_ZVAL_PTR(offset);
				}
				Z_OBJ_HT_P(*container)->unset_dimension(*container, offset TSRMLS_CC);
				if (0) {
					zval_ptr_dtor(&offset);
				} else {
					if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
				}
				break;
			case IS_STRING:
				zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
				ZEND_VM_CONTINUE(); /* bailed out before */
			default:
				if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
				break;
		}
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_UNSET_OBJ_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *offset = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (container) {
		if (IS_VAR == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			Z_OBJ_HT_P(*container)->unset_property(*container, offset TSRMLS_CC);
			if (0) {
				zval_ptr_dtor(&offset);
			} else {
				if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
			}
		} else {
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		}
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int zend_isset_isempty_dim_prop_obj_handler_SPEC_VAR_VAR(int prop_dim, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval **value = NULL;
	int result = 0;
	long index;

	if (container) {
		zend_free_op free_op2;
		zval *offset = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (Z_TYPE_PP(container) == IS_ARRAY) {
			HashTable *ht;
			int isset = 0;

			ht = Z_ARRVAL_PP(container);

			switch (Z_TYPE_P(offset)) {
				case IS_DOUBLE:
					index = (long) Z_DVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_RESOURCE:
				case IS_BOOL:
				case IS_LONG:
					index = Z_LVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_STRING:
					if (zend_symtable_find(ht, offset->value.str.val, offset->value.str.len+1, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_NULL:
					if (zend_hash_find(ht, "", sizeof(""), (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				default:
					zend_error(E_WARNING, "Illegal offset type in isset or empty");

					break;
			}

			switch (opline->extended_value) {
				case ZEND_ISSET:
					if (isset && Z_TYPE_PP(value) == IS_NULL) {
						result = 0;
					} else {
						result = isset;
					}
					break;
				case ZEND_ISEMPTY:
					if (!isset || !i_zend_is_true(*value)) {
						result = 0;
					} else {
						result = 1;
					}
					break;
			}
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		} else if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			if (prop_dim) {
				result = Z_OBJ_HT_P(*container)->has_property(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			} else {
				result = Z_OBJ_HT_P(*container)->has_dimension(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			}
			if (0) {
				zval_ptr_dtor(&offset);
			} else {
				if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
			}
		} else if ((*container)->type == IS_STRING && !prop_dim) { /* string offsets */
			zval tmp;

			if (Z_TYPE_P(offset) != IS_LONG) {
				tmp = *offset;
				zval_copy_ctor(&tmp);
				convert_to_long(&tmp);
				offset = &tmp;
			}
			if (Z_TYPE_P(offset) == IS_LONG) {
				switch (opline->extended_value) {
					case ZEND_ISSET:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container)) {
							result = 1;
						}
						break;
					case ZEND_ISEMPTY:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container) && Z_STRVAL_PP(container)[offset->value.lval] != '0') {
							result = 1;
						}
						break;
				}
			}
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		} else {
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		}
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = result;
			break;
		case ZEND_ISEMPTY:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = !result;
			break;
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_VAR_VAR(0, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_VAR_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_VAR_VAR(1, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_binary_assign_op_obj_helper_SPEC_VAR_UNUSED(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1, free_op_data1;
	zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *object;
	zval *property = NULL;
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	znode *result = &opline->result;
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");

		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
	} else {
		/* here we are sure we are dealing with an object */
		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = *zptr;
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;

			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					if (Z_OBJ_HT_P(object)->read_property) {
						z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
				case ZEND_ASSIGN_DIM:
					if (Z_OBJ_HT_P(object)->read_dimension) {
						z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (z->refcount == 0) {
						zval_dtor(z);
						FREE_ZVAL(z);
					}
					z = value;
				}
				z->refcount++;
				SEPARATE_ZVAL_IF_NOT_REF(&z);
				binary_op(z, z, value TSRMLS_CC);
				switch (opline->extended_value) {
					case ZEND_ASSIGN_OBJ:
						Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
						break;
					case ZEND_ASSIGN_DIM:
						Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
						break;
				}
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = z;
					PZVAL_LOCK(*retval);
				}
				zval_ptr_dtor(&z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = EG(uninitialized_zval_ptr);
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (0) {
			zval_ptr_dtor(&property);
		} else {

		}
		FREE_OP(free_op_data1);
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_helper_SPEC_VAR_UNUSED(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op_data2, free_op_data1;
	zval **var_ptr;
	zval *value;
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			return zend_binary_assign_op_obj_helper_SPEC_VAR_UNUSED(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

				if (object_ptr && IS_VAR != IS_CV && !(free_op1.var != NULL)) {
					(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
				}

				if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
					return zend_binary_assign_op_obj_helper_SPEC_VAR_UNUSED(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
				} else {
					zend_op *op_data = opline+1;
					zval *dim = NULL;

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), &free_op_data2, BP_VAR_RW);
					increment_opline = 1;
				}
			}
			break;
		default:
			value = NULL;
			var_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		if (increment_opline) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	if (increment_opline) {
		ZEND_VM_INC_OPCODE();
		FREE_OP(free_op_data1);
		FREE_OP_VAR_PTR(free_op_data2);
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_ADD_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_UNUSED(add_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SUB_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_UNUSED(sub_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MUL_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_UNUSED(mul_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_DIV_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_UNUSED(div_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MOD_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_UNUSED(mod_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SL_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_UNUSED(shift_left_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SR_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_UNUSED(shift_right_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_CONCAT_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_UNUSED(concat_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_OR_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_UNUSED(bitwise_or_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_AND_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_UNUSED(bitwise_and_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_XOR_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_UNUSED(bitwise_xor_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_DIM_W_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *dim = NULL;

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_W TSRMLS_CC);

	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_RW_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *dim = NULL;

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_RW TSRMLS_CC);

	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_FUNC_ARG_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int type = ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)?BP_VAR_W:BP_VAR_R;
	zval *dim;

	if (IS_UNUSED == IS_UNUSED && type == BP_VAR_R) {
		zend_error_noreturn(E_ERROR, "Cannot use [] for reading");
	}
	dim = NULL;
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, type TSRMLS_CC);

	if (IS_VAR == IS_VAR && type == BP_VAR_W && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_DIM_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1;
	zval **object_ptr;

	if (IS_VAR == IS_CV || EX_T(opline->op1.u.var).var.ptr_ptr) {
		/* not an array offset */
		object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	} else {
		object_ptr = NULL;
	}

	if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
		zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_DIM TSRMLS_CC);
	} else {
		zend_free_op free_op_data1;
		zval *value;
		zval *dim = NULL;

		zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), object_ptr, dim, 0, BP_VAR_W TSRMLS_CC);

		value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	 	zend_assign_to_variable(&opline->result, &op_data->op2, &op_data->op1, value, (IS_TMP_FREE(free_op_data1)?IS_TMP_VAR:op_data->op1.op_type), EX(Ts) TSRMLS_CC);
	 	FREE_OP_IF_VAR(free_op_data1);
	}
 	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	/* assign_dim has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=NULL;

#if 0 || IS_VAR == IS_VAR || IS_VAR == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=_get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	}
#else
	expr_ptr=_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
#endif

	if (0) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_VAR == IS_VAR || IS_VAR == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}

	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	} else {
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_VAR == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_VAR != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_VAR_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_ADD_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	add_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SUB_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	sub_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MUL_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	mul_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DIV_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	div_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MOD_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	mod_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SL_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SR_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CONCAT_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	concat_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_IDENTICAL_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_IDENTICAL_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_EQUAL_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_EQUAL_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_OR_EQUAL_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_OR_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_AND_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_XOR_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_XOR_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_obj_helper_SPEC_VAR_CV(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1, free_op_data1;
	zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	znode *result = &opline->result;
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");

		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
	} else {
		/* here we are sure we are dealing with an object */
		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = *zptr;
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;

			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					if (Z_OBJ_HT_P(object)->read_property) {
						z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
				case ZEND_ASSIGN_DIM:
					if (Z_OBJ_HT_P(object)->read_dimension) {
						z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (z->refcount == 0) {
						zval_dtor(z);
						FREE_ZVAL(z);
					}
					z = value;
				}
				z->refcount++;
				SEPARATE_ZVAL_IF_NOT_REF(&z);
				binary_op(z, z, value TSRMLS_CC);
				switch (opline->extended_value) {
					case ZEND_ASSIGN_OBJ:
						Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
						break;
					case ZEND_ASSIGN_DIM:
						Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
						break;
				}
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = z;
					PZVAL_LOCK(*retval);
				}
				zval_ptr_dtor(&z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = EG(uninitialized_zval_ptr);
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (0) {
			zval_ptr_dtor(&property);
		} else {

		}
		FREE_OP(free_op_data1);
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_helper_SPEC_VAR_CV(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op_data2, free_op_data1;
	zval **var_ptr;
	zval *value;
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			return zend_binary_assign_op_obj_helper_SPEC_VAR_CV(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

				if (object_ptr && IS_VAR != IS_CV && !(free_op1.var != NULL)) {
					(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
				}

				if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
					return zend_binary_assign_op_obj_helper_SPEC_VAR_CV(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
				} else {
					zend_op *op_data = opline+1;
					zval *dim = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), &free_op_data2, BP_VAR_RW);
					increment_opline = 1;
				}
			}
			break;
		default:
			value = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
			var_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		if (increment_opline) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	if (increment_opline) {
		ZEND_VM_INC_OPCODE();
		FREE_OP(free_op_data1);
		FREE_OP_VAR_PTR(free_op_data2);
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_ADD_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CV(add_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SUB_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CV(sub_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MUL_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CV(mul_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_DIV_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CV(div_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MOD_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CV(mod_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SL_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CV(shift_left_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SR_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CV(shift_right_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_CONCAT_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CV(concat_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_OR_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CV(bitwise_or_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_AND_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CV(bitwise_and_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_XOR_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_VAR_CV(bitwise_xor_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_pre_incdec_property_helper_SPEC_VAR_CV(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval **retval = &EX_T(opline->result.u.var).var.ptr;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");

		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			have_get_ptr = 1;
			incdec_op(*zptr);
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = *zptr;
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			z->refcount++;
			SEPARATE_ZVAL_IF_NOT_REF(&z);
			incdec_op(z);
			*retval = z;
			Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = EG(uninitialized_zval_ptr);
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRE_INC_OBJ_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_VAR_CV(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_PRE_DEC_OBJ_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_VAR_CV(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_post_incdec_property_helper_SPEC_VAR_CV(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval *retval = &EX_T(opline->result.u.var).tmp_var;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");

		*retval = *EG(uninitialized_zval_ptr);
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			have_get_ptr = 1;
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			*retval = **zptr;
			zendi_zval_copy_ctor(*retval);

			incdec_op(*zptr);

		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
			zval *z_copy;

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			*retval = *z;
			zendi_zval_copy_ctor(*retval);
			ALLOC_ZVAL(z_copy);
			*z_copy = *z;
			zendi_zval_copy_ctor(*z_copy);
			INIT_PZVAL(z_copy);
			incdec_op(z_copy);
			z->refcount++;
			Z_OBJ_HT_P(object)->write_property(object, property, z_copy TSRMLS_CC);
			zval_ptr_dtor(&z_copy);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			*retval = *EG(uninitialized_zval_ptr);
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_POST_INC_OBJ_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_VAR_CV(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_POST_DEC_OBJ_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_VAR_CV(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_DIM_R_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *dim = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK &&
	    IS_VAR != IS_CV &&
	    EX_T(opline->op1.u.var).var.ptr_ptr) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_R TSRMLS_CC);

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_W_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *dim = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_W TSRMLS_CC);

	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_RW_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *dim = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_RW TSRMLS_CC);

	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_IS_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *dim = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, BP_VAR_IS TSRMLS_CC);

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_FUNC_ARG_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int type = ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)?BP_VAR_W:BP_VAR_R;
	zval *dim;

	if (IS_CV == IS_UNUSED && type == BP_VAR_R) {
		zend_error_noreturn(E_ERROR, "Cannot use [] for reading");
	}
	dim = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), dim, 0, type TSRMLS_CC);

	if (IS_VAR == IS_VAR && type == BP_VAR_W && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_UNSET_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *dim = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	/* Not needed in DIM_UNSET
	if (opline->extended_value == ZEND_FETCH_ADD_LOCK) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	*/
	if (IS_VAR == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, dim, 0, BP_VAR_UNSET TSRMLS_CC);

	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	if (EX_T(opline->result.u.var).var.ptr_ptr == NULL) {
		zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
	} else {
		zend_free_op free_res;

		PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
		if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
		}
		PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		FREE_OP_VAR_PTR(free_res);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int zend_fetch_property_address_read_helper_SPEC_VAR_CV(int type, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *container;
	zval **retval;
	zend_free_op free_op1;

	retval = &EX_T(opline->result.u.var).var.ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = retval;

	container = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (container == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(error_zval_ptr);
			PZVAL_LOCK(*retval);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	}


	if (Z_TYPE_P(container) != IS_OBJECT || !Z_OBJ_HT_P(container)->read_property) {
		if (type != BP_VAR_IS) {
			zend_error(E_NOTICE, "Trying to get property of non-object");
		}
		*retval = EG(uninitialized_zval_ptr);
		SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	} else {

		zval *offset  = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

		if (0) {
			MAKE_REAL_ZVAL_PTR(offset);
		}

		/* here we are sure we are dealing with an object */
		*retval = Z_OBJ_HT_P(container)->read_property(container, offset, type TSRMLS_CC);

		if (RETURN_VALUE_UNUSED(&opline->result) && ((*retval)->refcount == 0)) {
			zval_dtor(*retval);
			FREE_ZVAL(*retval);
		} else {
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		if (0) {
			zval_ptr_dtor(&offset);
		} else {

		}
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_R_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_VAR_CV(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_W_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK && IS_VAR != IS_CV) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
		EX_T(opline->op1.u.var).var.ptr = *EX_T(opline->op1.u.var).var.ptr_ptr;
	}

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), property, BP_VAR_W TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_RW_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), property, BP_VAR_RW TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_IS_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_VAR_CV(BP_VAR_IS, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_FUNC_ARG_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		/* Behave like FETCH_OBJ_W */
		zend_free_op free_op1;
		zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}
		zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC), property, BP_VAR_W TSRMLS_CC);
		if (0) {
			zval_ptr_dtor(&property);
		} else {

		}
		if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
		    READY_TO_DESTROY(free_op1.var) &&
		    !RETURN_VALUE_UNUSED(&opline->result)) {
			AI_USE_PTR(EX_T(opline->result.u.var).var);
			if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
			    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
				SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
			}
		}
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		ZEND_VM_NEXT_OPCODE();
	} else {
		return zend_fetch_property_address_read_helper_SPEC_VAR_CV(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}
}

static int ZEND_FETCH_OBJ_UNSET_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_res;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (IS_VAR == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, property, BP_VAR_UNSET TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_VAR == IS_VAR && (free_op1.var != NULL) &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
	if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
		SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
	}
	PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	FREE_OP_VAR_PTR(free_res);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_OBJ_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1;
	zval **object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_OBJ TSRMLS_CC);
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_DIM_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1;
	zval **object_ptr;

	if (IS_VAR == IS_CV || EX_T(opline->op1.u.var).var.ptr_ptr) {
		/* not an array offset */
		object_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	} else {
		object_ptr = NULL;
	}

	if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
		zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_DIM TSRMLS_CC);
	} else {
		zend_free_op free_op_data1;
		zval *value;
		zval *dim = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

		zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), object_ptr, dim, 0, BP_VAR_W TSRMLS_CC);

		value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	 	zend_assign_to_variable(&opline->result, &op_data->op2, &op_data->op1, value, (IS_TMP_FREE(free_op_data1)?IS_TMP_VAR:op_data->op1.op_type), EX(Ts) TSRMLS_CC);
	 	FREE_OP_IF_VAR(free_op_data1);
	}
 	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	/* assign_dim has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *value = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

 	zend_assign_to_variable(&opline->result, &opline->op1, &opline->op2, value, (0?IS_TMP_VAR:IS_CV), EX(Ts) TSRMLS_CC);
	/* zend_assign_to_variable() always takes care of op2, never free it! */

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_REF_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **variable_ptr_ptr;
	zval **value_ptr_ptr = _get_zval_ptr_ptr_cv(&opline->op2, EX(Ts), BP_VAR_W TSRMLS_CC);

	if (IS_CV == IS_VAR &&
	    value_ptr_ptr &&
	    !(*value_ptr_ptr)->is_ref &&
	    opline->extended_value == ZEND_RETURNS_FUNCTION &&
	    !EX_T(opline->op2.u.var).var.fcall_returned_reference) {
		if (free_op2.var == NULL) {
			PZVAL_LOCK(*value_ptr_ptr); /* undo the effect of get_zval_ptr_ptr() */
		}
		zend_error(E_STRICT, "Only variables should be assigned by reference");
		if (EG(exception)) {

			ZEND_VM_NEXT_OPCODE();
		}
		return ZEND_ASSIGN_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	} else if (IS_CV == IS_VAR && opline->extended_value == ZEND_RETURNS_NEW) {
		PZVAL_LOCK(*value_ptr_ptr);
	}
	if (IS_VAR == IS_VAR && EX_T(opline->op1.u.var).var.ptr_ptr == &EX_T(opline->op1.u.var).var.ptr) {
		zend_error(E_ERROR, "Cannot assign by reference to overloaded object");
	}

	variable_ptr_ptr = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zend_assign_to_variable_reference(variable_ptr_ptr, value_ptr_ptr TSRMLS_CC);

	if (IS_CV == IS_VAR && opline->extended_value == ZEND_RETURNS_NEW) {
		(*variable_ptr_ptr)->refcount--;
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = variable_ptr_ptr;
		PZVAL_LOCK(*variable_ptr_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_METHOD_CALL_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;
	zend_free_op free_op1;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	function_name = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = Z_STRVAL_P(function_name);
	function_name_strlen = Z_STRLEN_P(function_name);

	EX(object) = _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);

	if (EX(object) && Z_TYPE_P(EX(object)) == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(&EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CASE_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;
	zend_free_op free_op1;

	if (IS_VAR==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC),
				 _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);

	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

#if 0 || IS_VAR == IS_VAR || IS_VAR == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=_get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	}
#else
	expr_ptr=_get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
#endif

	if (0) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_VAR == IS_VAR || IS_VAR == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}

	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	} else {
		if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_VAR == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_VAR != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_UNSET_DIM_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *offset = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
	long index;

	if (container) {
		if (IS_VAR == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		switch (Z_TYPE_PP(container)) {
			case IS_ARRAY: {
				HashTable *ht = Z_ARRVAL_PP(container);

				switch (Z_TYPE_P(offset)) {
					case IS_DOUBLE:
						index = (long) Z_DVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_RESOURCE:
					case IS_BOOL:
					case IS_LONG:
						index = Z_LVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_STRING:
						if (IS_CV == IS_CV || IS_CV == IS_VAR) {
							offset->refcount++;
						}
						if (zend_symtable_del(ht, offset->value.str.val, offset->value.str.len+1) == SUCCESS &&
					    ht == &EG(symbol_table)) {
							zend_execute_data *ex;
							ulong hash_value = zend_inline_hash_func(offset->value.str.val, offset->value.str.len+1);

							for (ex = execute_data; ex; ex = ex->prev_execute_data) {
								if (ex->op_array && ex->symbol_table == ht) {
									int i;

									for (i = 0; i < ex->op_array->last_var; i++) {
										if (ex->op_array->vars[i].hash_value == hash_value &&
										    ex->op_array->vars[i].name_len == offset->value.str.len &&
										    !memcmp(ex->op_array->vars[i].name, offset->value.str.val, offset->value.str.len)) {
											ex->CVs[i] = NULL;
											break;
										}
									}
								}
							}
						}
						if (IS_CV == IS_CV || IS_CV == IS_VAR) {
							zval_ptr_dtor(&offset);
						}
						break;
					case IS_NULL:
						zend_hash_del(ht, "", sizeof(""));
						break;
					default:
						zend_error(E_WARNING, "Illegal offset type in unset");
						break;
				}

				break;
			}
			case IS_OBJECT:
				if (!Z_OBJ_HT_P(*container)->unset_dimension) {
					zend_error_noreturn(E_ERROR, "Cannot use object as array");
				}
				if (0) {
					MAKE_REAL_ZVAL_PTR(offset);
				}
				Z_OBJ_HT_P(*container)->unset_dimension(*container, offset TSRMLS_CC);
				if (0) {
					zval_ptr_dtor(&offset);
				} else {

				}
				break;
			case IS_STRING:
				zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
				ZEND_VM_CONTINUE(); /* bailed out before */
			default:

				break;
		}
	} else {

	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_UNSET_OBJ_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval *offset = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (container) {
		if (IS_VAR == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			Z_OBJ_HT_P(*container)->unset_property(*container, offset TSRMLS_CC);
			if (0) {
				zval_ptr_dtor(&offset);
			} else {

			}
		} else {

		}
	} else {

	}
	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int zend_isset_isempty_dim_prop_obj_handler_SPEC_VAR_CV(int prop_dim, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **container = _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC);
	zval **value = NULL;
	int result = 0;
	long index;

	if (container) {

		zval *offset = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

		if (Z_TYPE_PP(container) == IS_ARRAY) {
			HashTable *ht;
			int isset = 0;

			ht = Z_ARRVAL_PP(container);

			switch (Z_TYPE_P(offset)) {
				case IS_DOUBLE:
					index = (long) Z_DVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_RESOURCE:
				case IS_BOOL:
				case IS_LONG:
					index = Z_LVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_STRING:
					if (zend_symtable_find(ht, offset->value.str.val, offset->value.str.len+1, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_NULL:
					if (zend_hash_find(ht, "", sizeof(""), (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				default:
					zend_error(E_WARNING, "Illegal offset type in isset or empty");

					break;
			}

			switch (opline->extended_value) {
				case ZEND_ISSET:
					if (isset && Z_TYPE_PP(value) == IS_NULL) {
						result = 0;
					} else {
						result = isset;
					}
					break;
				case ZEND_ISEMPTY:
					if (!isset || !i_zend_is_true(*value)) {
						result = 0;
					} else {
						result = 1;
					}
					break;
			}

		} else if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			if (prop_dim) {
				result = Z_OBJ_HT_P(*container)->has_property(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			} else {
				result = Z_OBJ_HT_P(*container)->has_dimension(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			}
			if (0) {
				zval_ptr_dtor(&offset);
			} else {

			}
		} else if ((*container)->type == IS_STRING && !prop_dim) { /* string offsets */
			zval tmp;

			if (Z_TYPE_P(offset) != IS_LONG) {
				tmp = *offset;
				zval_copy_ctor(&tmp);
				convert_to_long(&tmp);
				offset = &tmp;
			}
			if (Z_TYPE_P(offset) == IS_LONG) {
				switch (opline->extended_value) {
					case ZEND_ISSET:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container)) {
							result = 1;
						}
						break;
					case ZEND_ISEMPTY:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container) && Z_STRVAL_PP(container)[offset->value.lval] != '0') {
							result = 1;
						}
						break;
				}
			}

		} else {

		}
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = result;
			break;
		case ZEND_ISEMPTY:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = !result;
			break;
	}

	if (free_op1.var) {zval_ptr_dtor(&free_op1.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_VAR_CV(0, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_VAR_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_VAR_CV(1, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_CLONE_SPEC_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *obj = _get_obj_zval_ptr_unused(TSRMLS_C);
	zend_class_entry *ce;
	zend_function *clone;
	zend_object_clone_obj_t clone_call;

	if (!obj || Z_TYPE_P(obj) != IS_OBJECT) {
		zend_error_noreturn(E_ERROR, "__clone method called on non-object");
		EX_T(opline->result.u.var).var.ptr = EG(error_zval_ptr);
		EX_T(opline->result.u.var).var.ptr->refcount++;

		ZEND_VM_NEXT_OPCODE();
	}

	ce = Z_OBJCE_P(obj);
	clone = ce ? ce->clone : NULL;
	clone_call =  Z_OBJ_HT_P(obj)->clone_obj;
	if (!clone_call) {
		if (ce) {
			zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object of class %s", ce->name);
		} else {
			zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object");
		}
		EX_T(opline->result.u.var).var.ptr = EG(error_zval_ptr);
		EX_T(opline->result.u.var).var.ptr->refcount++;
	}

	if (ce && clone) {
		if (clone->op_array.fn_flags & ZEND_ACC_PRIVATE) {
			/* Ensure that if we're calling a private function, we're allowed to do so.
			 */
			if (ce != EG(scope)) {
				zend_error_noreturn(E_ERROR, "Call to private %s::__clone() from context '%s'", ce->name, EG(scope) ? EG(scope)->name : "");
			}
		} else if ((clone->common.fn_flags & ZEND_ACC_PROTECTED)) {
			/* Ensure that if we're calling a protected function, we're allowed to do so.
			 */
			if (!zend_check_protected(clone->common.scope, EG(scope))) {
				zend_error_noreturn(E_ERROR, "Call to protected %s::__clone() from context '%s'", ce->name, EG(scope) ? EG(scope)->name : "");
			}
		}
	}

	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	if (!EG(exception)) {
		ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
		Z_OBJVAL_P(EX_T(opline->result.u.var).var.ptr) = clone_call(obj TSRMLS_CC);
		Z_TYPE_P(EX_T(opline->result.u.var).var.ptr) = IS_OBJECT;
		EX_T(opline->result.u.var).var.ptr->refcount=1;
		EX_T(opline->result.u.var).var.ptr->is_ref=1;
		if (!RETURN_VALUE_USED(opline) || EG(exception)) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_EXIT_SPEC_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
#if 0 || (IS_UNUSED != IS_UNUSED)
	zend_op *opline = EX(opline);
	if (IS_UNUSED != IS_UNUSED) {

		zval *ptr = NULL;

		if (Z_TYPE_P(ptr) == IS_LONG) {
			EG(exit_status) = Z_LVAL_P(ptr);
		} else {
			zend_print_variable(ptr);
		}

	}
#endif
	zend_bailout();
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_obj_helper_SPEC_UNUSED_CONST(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op_data1;
	zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *object;
	zval *property = &opline->op2.u.constant;
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	znode *result = &opline->result;
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");

		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
	} else {
		/* here we are sure we are dealing with an object */
		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = *zptr;
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;

			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					if (Z_OBJ_HT_P(object)->read_property) {
						z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
				case ZEND_ASSIGN_DIM:
					if (Z_OBJ_HT_P(object)->read_dimension) {
						z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (z->refcount == 0) {
						zval_dtor(z);
						FREE_ZVAL(z);
					}
					z = value;
				}
				z->refcount++;
				SEPARATE_ZVAL_IF_NOT_REF(&z);
				binary_op(z, z, value TSRMLS_CC);
				switch (opline->extended_value) {
					case ZEND_ASSIGN_OBJ:
						Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
						break;
					case ZEND_ASSIGN_DIM:
						Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
						break;
				}
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = z;
					PZVAL_LOCK(*retval);
				}
				zval_ptr_dtor(&z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = EG(uninitialized_zval_ptr);
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (0) {
			zval_ptr_dtor(&property);
		} else {

		}
		FREE_OP(free_op_data1);
	}

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_helper_SPEC_UNUSED_CONST(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op_data2, free_op_data1;
	zval **var_ptr;
	zval *value;
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			return zend_binary_assign_op_obj_helper_SPEC_UNUSED_CONST(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);

				if (object_ptr && IS_UNUSED != IS_CV && !0) {
					(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
				}

				if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
					return zend_binary_assign_op_obj_helper_SPEC_UNUSED_CONST(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
				} else {
					zend_op *op_data = opline+1;
					zval *dim = &opline->op2.u.constant;

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), NULL, dim, 0, BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), &free_op_data2, BP_VAR_RW);
					increment_opline = 1;
				}
			}
			break;
		default:
			value = &opline->op2.u.constant;
			var_ptr = NULL;
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}


		if (increment_opline) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	if (increment_opline) {
		ZEND_VM_INC_OPCODE();
		FREE_OP(free_op_data1);
		FREE_OP_VAR_PTR(free_op_data2);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_ADD_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CONST(add_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SUB_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CONST(sub_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MUL_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CONST(mul_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_DIV_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CONST(div_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MOD_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CONST(mod_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SL_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CONST(shift_left_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SR_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CONST(shift_right_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_CONCAT_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CONST(concat_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_OR_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CONST(bitwise_or_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_AND_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CONST(bitwise_and_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_XOR_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CONST(bitwise_xor_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_pre_incdec_property_helper_SPEC_UNUSED_CONST(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *object;
	zval *property = &opline->op2.u.constant;
	zval **retval = &EX_T(opline->result.u.var).var.ptr;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");

		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}

		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			have_get_ptr = 1;
			incdec_op(*zptr);
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = *zptr;
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			z->refcount++;
			SEPARATE_ZVAL_IF_NOT_REF(&z);
			incdec_op(z);
			*retval = z;
			Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = EG(uninitialized_zval_ptr);
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRE_INC_OBJ_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_UNUSED_CONST(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_PRE_DEC_OBJ_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_UNUSED_CONST(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_post_incdec_property_helper_SPEC_UNUSED_CONST(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *object;
	zval *property = &opline->op2.u.constant;
	zval *retval = &EX_T(opline->result.u.var).tmp_var;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");

		*retval = *EG(uninitialized_zval_ptr);

		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			have_get_ptr = 1;
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			*retval = **zptr;
			zendi_zval_copy_ctor(*retval);

			incdec_op(*zptr);

		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
			zval *z_copy;

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			*retval = *z;
			zendi_zval_copy_ctor(*retval);
			ALLOC_ZVAL(z_copy);
			*z_copy = *z;
			zendi_zval_copy_ctor(*z_copy);
			INIT_PZVAL(z_copy);
			incdec_op(z_copy);
			z->refcount++;
			Z_OBJ_HT_P(object)->write_property(object, property, z_copy TSRMLS_CC);
			zval_ptr_dtor(&z_copy);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			*retval = *EG(uninitialized_zval_ptr);
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_POST_INC_OBJ_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_UNUSED_CONST(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_POST_DEC_OBJ_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_UNUSED_CONST(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_fetch_property_address_read_helper_SPEC_UNUSED_CONST(int type, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *container;
	zval **retval;


	retval = &EX_T(opline->result.u.var).var.ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = retval;

	container = _get_obj_zval_ptr_unused(TSRMLS_C);

	if (container == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(error_zval_ptr);
			PZVAL_LOCK(*retval);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		ZEND_VM_NEXT_OPCODE();
	}


	if (Z_TYPE_P(container) != IS_OBJECT || !Z_OBJ_HT_P(container)->read_property) {
		if (type != BP_VAR_IS) {
			zend_error(E_NOTICE, "Trying to get property of non-object");
		}
		*retval = EG(uninitialized_zval_ptr);
		SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	} else {

		zval *offset  = &opline->op2.u.constant;

		if (0) {
			MAKE_REAL_ZVAL_PTR(offset);
		}

		/* here we are sure we are dealing with an object */
		*retval = Z_OBJ_HT_P(container)->read_property(container, offset, type TSRMLS_CC);

		if (RETURN_VALUE_UNUSED(&opline->result) && ((*retval)->refcount == 0)) {
			zval_dtor(*retval);
			FREE_ZVAL(*retval);
		} else {
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		if (0) {
			zval_ptr_dtor(&offset);
		} else {

		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_R_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_UNUSED_CONST(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_W_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *property = &opline->op2.u.constant;

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK && IS_UNUSED != IS_CV) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
		EX_T(opline->op1.u.var).var.ptr = *EX_T(opline->op1.u.var).var.ptr_ptr;
	}

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_obj_zval_ptr_ptr_unused(TSRMLS_C), property, BP_VAR_W TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_UNUSED == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_RW_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *property = &opline->op2.u.constant;

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_obj_zval_ptr_ptr_unused(TSRMLS_C), property, BP_VAR_RW TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_UNUSED == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_IS_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_UNUSED_CONST(BP_VAR_IS, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_FUNC_ARG_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		/* Behave like FETCH_OBJ_W */
		zend_free_op free_op1;
		zval *property = &opline->op2.u.constant;

		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}
		zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_obj_zval_ptr_ptr_unused(TSRMLS_C), property, BP_VAR_W TSRMLS_CC);
		if (0) {
			zval_ptr_dtor(&property);
		} else {

		}
		if (IS_UNUSED == IS_VAR && 0 &&
		    READY_TO_DESTROY(free_op1.var) &&
		    !RETURN_VALUE_UNUSED(&opline->result)) {
			AI_USE_PTR(EX_T(opline->result.u.var).var);
			if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
			    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
				SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
			}
		}

		ZEND_VM_NEXT_OPCODE();
	} else {
		return zend_fetch_property_address_read_helper_SPEC_UNUSED_CONST(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}
}

static int ZEND_FETCH_OBJ_UNSET_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_res;
	zval **container = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *property = &opline->op2.u.constant;

	if (IS_UNUSED == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, property, BP_VAR_UNSET TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_UNUSED == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
	if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
		SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
	}
	PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	FREE_OP_VAR_PTR(free_res);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_OBJ_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;

	zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);

	zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_OBJ TSRMLS_CC);

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_METHOD_CALL_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;


	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	function_name = &opline->op2.u.constant;

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = Z_STRVAL_P(function_name);
	function_name_strlen = Z_STRLEN_P(function_name);

	EX(object) = _get_obj_zval_ptr_unused(TSRMLS_C);

	if (EX(object) && Z_TYPE_P(EX(object)) == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(&EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_CONSTANT_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_class_entry *ce = NULL;
	zval **value;

	if (IS_UNUSED == IS_UNUSED) {
/* This seems to be a reminant of namespaces
		if (EG(scope)) {
			ce = EG(scope);
			if (zend_hash_find(&ce->constants_table, Z_STRVAL(opline->op2.u.constant), Z_STRLEN(opline->op2.u.constant)+1, (void **) &value) == SUCCESS) {
				zval_update_constant(value, (void *) 1 TSRMLS_CC);
				EX_T(opline->result.u.var).tmp_var = **value;
				zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
				ZEND_VM_NEXT_OPCODE();
			}
		}
*/
		if (!zend_get_constant(opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len, &EX_T(opline->result.u.var).tmp_var TSRMLS_CC)) {
			zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'",
						opline->op2.u.constant.value.str.val,
						opline->op2.u.constant.value.str.val);
			EX_T(opline->result.u.var).tmp_var = opline->op2.u.constant;
			zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
		}
		ZEND_VM_NEXT_OPCODE();
	}

	ce = EX_T(opline->op1.u.var).class_entry;

	if (zend_hash_find(&ce->constants_table, opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len+1, (void **) &value) == SUCCESS) {
		zend_class_entry *old_scope = EG(scope);

		EG(scope) = ce;
		zval_update_constant(value, (void *) 1 TSRMLS_CC);
		EG(scope) = old_scope;
		EX_T(opline->result.u.var).tmp_var = **value;
		zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
	} else {
		zend_error_noreturn(E_ERROR, "Undefined class constant '%s'", opline->op2.u.constant.value.str.val);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_UNUSED == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_UNUSED != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_UNSET_DIM_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **container = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *offset = &opline->op2.u.constant;
	long index;

	if (container) {
		if (IS_UNUSED == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		switch (Z_TYPE_PP(container)) {
			case IS_ARRAY: {
				HashTable *ht = Z_ARRVAL_PP(container);

				switch (Z_TYPE_P(offset)) {
					case IS_DOUBLE:
						index = (long) Z_DVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_RESOURCE:
					case IS_BOOL:
					case IS_LONG:
						index = Z_LVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_STRING:
						if (IS_CONST == IS_CV || IS_CONST == IS_VAR) {
							offset->refcount++;
						}
						if (zend_symtable_del(ht, offset->value.str.val, offset->value.str.len+1) == SUCCESS &&
					    ht == &EG(symbol_table)) {
							zend_execute_data *ex;
							ulong hash_value = zend_inline_hash_func(offset->value.str.val, offset->value.str.len+1);

							for (ex = execute_data; ex; ex = ex->prev_execute_data) {
								if (ex->op_array && ex->symbol_table == ht) {
									int i;

									for (i = 0; i < ex->op_array->last_var; i++) {
										if (ex->op_array->vars[i].hash_value == hash_value &&
										    ex->op_array->vars[i].name_len == offset->value.str.len &&
										    !memcmp(ex->op_array->vars[i].name, offset->value.str.val, offset->value.str.len)) {
											ex->CVs[i] = NULL;
											break;
										}
									}
								}
							}
						}
						if (IS_CONST == IS_CV || IS_CONST == IS_VAR) {
							zval_ptr_dtor(&offset);
						}
						break;
					case IS_NULL:
						zend_hash_del(ht, "", sizeof(""));
						break;
					default:
						zend_error(E_WARNING, "Illegal offset type in unset");
						break;
				}

				break;
			}
			case IS_OBJECT:
				if (!Z_OBJ_HT_P(*container)->unset_dimension) {
					zend_error_noreturn(E_ERROR, "Cannot use object as array");
				}
				if (0) {
					MAKE_REAL_ZVAL_PTR(offset);
				}
				Z_OBJ_HT_P(*container)->unset_dimension(*container, offset TSRMLS_CC);
				if (0) {
					zval_ptr_dtor(&offset);
				} else {

				}
				break;
			case IS_STRING:
				zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
				ZEND_VM_CONTINUE(); /* bailed out before */
			default:

				break;
		}
	} else {

	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_UNSET_OBJ_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **container = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *offset = &opline->op2.u.constant;

	if (container) {
		if (IS_UNUSED == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			Z_OBJ_HT_P(*container)->unset_property(*container, offset TSRMLS_CC);
			if (0) {
				zval_ptr_dtor(&offset);
			} else {

			}
		} else {

		}
	} else {

	}

	ZEND_VM_NEXT_OPCODE();
}

static int zend_isset_isempty_dim_prop_obj_handler_SPEC_UNUSED_CONST(int prop_dim, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **container = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval **value = NULL;
	int result = 0;
	long index;

	if (container) {

		zval *offset = &opline->op2.u.constant;

		if (Z_TYPE_PP(container) == IS_ARRAY) {
			HashTable *ht;
			int isset = 0;

			ht = Z_ARRVAL_PP(container);

			switch (Z_TYPE_P(offset)) {
				case IS_DOUBLE:
					index = (long) Z_DVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_RESOURCE:
				case IS_BOOL:
				case IS_LONG:
					index = Z_LVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_STRING:
					if (zend_symtable_find(ht, offset->value.str.val, offset->value.str.len+1, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_NULL:
					if (zend_hash_find(ht, "", sizeof(""), (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				default:
					zend_error(E_WARNING, "Illegal offset type in isset or empty");

					break;
			}

			switch (opline->extended_value) {
				case ZEND_ISSET:
					if (isset && Z_TYPE_PP(value) == IS_NULL) {
						result = 0;
					} else {
						result = isset;
					}
					break;
				case ZEND_ISEMPTY:
					if (!isset || !i_zend_is_true(*value)) {
						result = 0;
					} else {
						result = 1;
					}
					break;
			}

		} else if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			if (prop_dim) {
				result = Z_OBJ_HT_P(*container)->has_property(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			} else {
				result = Z_OBJ_HT_P(*container)->has_dimension(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			}
			if (0) {
				zval_ptr_dtor(&offset);
			} else {

			}
		} else if ((*container)->type == IS_STRING && !prop_dim) { /* string offsets */
			zval tmp;

			if (Z_TYPE_P(offset) != IS_LONG) {
				tmp = *offset;
				zval_copy_ctor(&tmp);
				convert_to_long(&tmp);
				offset = &tmp;
			}
			if (Z_TYPE_P(offset) == IS_LONG) {
				switch (opline->extended_value) {
					case ZEND_ISSET:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container)) {
							result = 1;
						}
						break;
					case ZEND_ISEMPTY:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container) && Z_STRVAL_PP(container)[offset->value.lval] != '0') {
							result = 1;
						}
						break;
				}
			}

		} else {

		}
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = result;
			break;
		case ZEND_ISEMPTY:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = !result;
			break;
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_UNUSED_CONST(0, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_UNUSED_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_UNUSED_CONST(1, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_binary_assign_op_obj_helper_SPEC_UNUSED_TMP(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op2, free_op_data1;
	zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *object;
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	znode *result = &opline->result;
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");
		zval_dtor(free_op2.var);
		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
	} else {
		/* here we are sure we are dealing with an object */
		if (1) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = *zptr;
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;

			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					if (Z_OBJ_HT_P(object)->read_property) {
						z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
				case ZEND_ASSIGN_DIM:
					if (Z_OBJ_HT_P(object)->read_dimension) {
						z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (z->refcount == 0) {
						zval_dtor(z);
						FREE_ZVAL(z);
					}
					z = value;
				}
				z->refcount++;
				SEPARATE_ZVAL_IF_NOT_REF(&z);
				binary_op(z, z, value TSRMLS_CC);
				switch (opline->extended_value) {
					case ZEND_ASSIGN_OBJ:
						Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
						break;
					case ZEND_ASSIGN_DIM:
						Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
						break;
				}
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = z;
					PZVAL_LOCK(*retval);
				}
				zval_ptr_dtor(&z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = EG(uninitialized_zval_ptr);
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (1) {
			zval_ptr_dtor(&property);
		} else {
			zval_dtor(free_op2.var);
		}
		FREE_OP(free_op_data1);
	}

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_helper_SPEC_UNUSED_TMP(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2, free_op_data2, free_op_data1;
	zval **var_ptr;
	zval *value;
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			return zend_binary_assign_op_obj_helper_SPEC_UNUSED_TMP(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);

				if (object_ptr && IS_UNUSED != IS_CV && !0) {
					(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
				}

				if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
					return zend_binary_assign_op_obj_helper_SPEC_UNUSED_TMP(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
				} else {
					zend_op *op_data = opline+1;
					zval *dim = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), NULL, dim, 1, BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), &free_op_data2, BP_VAR_RW);
					increment_opline = 1;
				}
			}
			break;
		default:
			value = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
			var_ptr = NULL;
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		zval_dtor(free_op2.var);

		if (increment_opline) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}
	zval_dtor(free_op2.var);

	if (increment_opline) {
		ZEND_VM_INC_OPCODE();
		FREE_OP(free_op_data1);
		FREE_OP_VAR_PTR(free_op_data2);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_ADD_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_TMP(add_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SUB_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_TMP(sub_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MUL_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_TMP(mul_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_DIV_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_TMP(div_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MOD_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_TMP(mod_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SL_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_TMP(shift_left_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SR_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_TMP(shift_right_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_CONCAT_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_TMP(concat_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_OR_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_TMP(bitwise_or_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_AND_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_TMP(bitwise_and_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_XOR_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_TMP(bitwise_xor_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_pre_incdec_property_helper_SPEC_UNUSED_TMP(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *object;
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval **retval = &EX_T(opline->result.u.var).var.ptr;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		zval_dtor(free_op2.var);
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}

		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (1) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			have_get_ptr = 1;
			incdec_op(*zptr);
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = *zptr;
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			z->refcount++;
			SEPARATE_ZVAL_IF_NOT_REF(&z);
			incdec_op(z);
			*retval = z;
			Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = EG(uninitialized_zval_ptr);
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (1) {
		zval_ptr_dtor(&property);
	} else {
		zval_dtor(free_op2.var);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRE_INC_OBJ_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_UNUSED_TMP(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_PRE_DEC_OBJ_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_UNUSED_TMP(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_post_incdec_property_helper_SPEC_UNUSED_TMP(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *object;
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval *retval = &EX_T(opline->result.u.var).tmp_var;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		zval_dtor(free_op2.var);
		*retval = *EG(uninitialized_zval_ptr);

		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (1) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			have_get_ptr = 1;
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			*retval = **zptr;
			zendi_zval_copy_ctor(*retval);

			incdec_op(*zptr);

		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
			zval *z_copy;

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			*retval = *z;
			zendi_zval_copy_ctor(*retval);
			ALLOC_ZVAL(z_copy);
			*z_copy = *z;
			zendi_zval_copy_ctor(*z_copy);
			INIT_PZVAL(z_copy);
			incdec_op(z_copy);
			z->refcount++;
			Z_OBJ_HT_P(object)->write_property(object, property, z_copy TSRMLS_CC);
			zval_ptr_dtor(&z_copy);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			*retval = *EG(uninitialized_zval_ptr);
		}
	}

	if (1) {
		zval_ptr_dtor(&property);
	} else {
		zval_dtor(free_op2.var);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_POST_INC_OBJ_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_UNUSED_TMP(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_POST_DEC_OBJ_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_UNUSED_TMP(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_fetch_property_address_read_helper_SPEC_UNUSED_TMP(int type, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *container;
	zval **retval;


	retval = &EX_T(opline->result.u.var).var.ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = retval;

	container = _get_obj_zval_ptr_unused(TSRMLS_C);

	if (container == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(error_zval_ptr);
			PZVAL_LOCK(*retval);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		ZEND_VM_NEXT_OPCODE();
	}


	if (Z_TYPE_P(container) != IS_OBJECT || !Z_OBJ_HT_P(container)->read_property) {
		if (type != BP_VAR_IS) {
			zend_error(E_NOTICE, "Trying to get property of non-object");
		}
		*retval = EG(uninitialized_zval_ptr);
		SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	} else {
		zend_free_op free_op2;
		zval *offset  = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (1) {
			MAKE_REAL_ZVAL_PTR(offset);
		}

		/* here we are sure we are dealing with an object */
		*retval = Z_OBJ_HT_P(container)->read_property(container, offset, type TSRMLS_CC);

		if (RETURN_VALUE_UNUSED(&opline->result) && ((*retval)->refcount == 0)) {
			zval_dtor(*retval);
			FREE_ZVAL(*retval);
		} else {
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		if (1) {
			zval_ptr_dtor(&offset);
		} else {
			zval_dtor(free_op2.var);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_R_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_UNUSED_TMP(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_W_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK && IS_UNUSED != IS_CV) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
		EX_T(opline->op1.u.var).var.ptr = *EX_T(opline->op1.u.var).var.ptr_ptr;
	}

	if (1) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_obj_zval_ptr_ptr_unused(TSRMLS_C), property, BP_VAR_W TSRMLS_CC);
	if (1) {
		zval_ptr_dtor(&property);
	} else {
		zval_dtor(free_op2.var);
	}
	if (IS_UNUSED == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_RW_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (1) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_obj_zval_ptr_ptr_unused(TSRMLS_C), property, BP_VAR_RW TSRMLS_CC);
	if (1) {
		zval_ptr_dtor(&property);
	} else {
		zval_dtor(free_op2.var);
	}
	if (IS_UNUSED == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_IS_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_UNUSED_TMP(BP_VAR_IS, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_FUNC_ARG_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		/* Behave like FETCH_OBJ_W */
		zend_free_op free_op1, free_op2;
		zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (1) {
			MAKE_REAL_ZVAL_PTR(property);
		}
		zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_obj_zval_ptr_ptr_unused(TSRMLS_C), property, BP_VAR_W TSRMLS_CC);
		if (1) {
			zval_ptr_dtor(&property);
		} else {
			zval_dtor(free_op2.var);
		}
		if (IS_UNUSED == IS_VAR && 0 &&
		    READY_TO_DESTROY(free_op1.var) &&
		    !RETURN_VALUE_UNUSED(&opline->result)) {
			AI_USE_PTR(EX_T(opline->result.u.var).var);
			if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
			    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
				SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
			}
		}

		ZEND_VM_NEXT_OPCODE();
	} else {
		return zend_fetch_property_address_read_helper_SPEC_UNUSED_TMP(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}
}

static int ZEND_FETCH_OBJ_UNSET_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2, free_res;
	zval **container = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (IS_UNUSED == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	if (1) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, property, BP_VAR_UNSET TSRMLS_CC);
	if (1) {
		zval_ptr_dtor(&property);
	} else {
		zval_dtor(free_op2.var);
	}
	if (IS_UNUSED == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
	if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
		SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
	}
	PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	FREE_OP_VAR_PTR(free_res);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_OBJ_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;

	zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);

	zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_OBJ TSRMLS_CC);

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_METHOD_CALL_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;
	zend_free_op free_op2;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	function_name = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = Z_STRVAL_P(function_name);
	function_name_strlen = Z_STRLEN_P(function_name);

	EX(object) = _get_obj_zval_ptr_unused(TSRMLS_C);

	if (EX(object) && Z_TYPE_P(EX(object)) == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(&EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}

	zval_dtor(free_op2.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_UNUSED == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_UNUSED != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_UNSET_DIM_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **container = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *offset = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	long index;

	if (container) {
		if (IS_UNUSED == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		switch (Z_TYPE_PP(container)) {
			case IS_ARRAY: {
				HashTable *ht = Z_ARRVAL_PP(container);

				switch (Z_TYPE_P(offset)) {
					case IS_DOUBLE:
						index = (long) Z_DVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_RESOURCE:
					case IS_BOOL:
					case IS_LONG:
						index = Z_LVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_STRING:
						if (IS_TMP_VAR == IS_CV || IS_TMP_VAR == IS_VAR) {
							offset->refcount++;
						}
						if (zend_symtable_del(ht, offset->value.str.val, offset->value.str.len+1) == SUCCESS &&
					    ht == &EG(symbol_table)) {
							zend_execute_data *ex;
							ulong hash_value = zend_inline_hash_func(offset->value.str.val, offset->value.str.len+1);

							for (ex = execute_data; ex; ex = ex->prev_execute_data) {
								if (ex->op_array && ex->symbol_table == ht) {
									int i;

									for (i = 0; i < ex->op_array->last_var; i++) {
										if (ex->op_array->vars[i].hash_value == hash_value &&
										    ex->op_array->vars[i].name_len == offset->value.str.len &&
										    !memcmp(ex->op_array->vars[i].name, offset->value.str.val, offset->value.str.len)) {
											ex->CVs[i] = NULL;
											break;
										}
									}
								}
							}
						}
						if (IS_TMP_VAR == IS_CV || IS_TMP_VAR == IS_VAR) {
							zval_ptr_dtor(&offset);
						}
						break;
					case IS_NULL:
						zend_hash_del(ht, "", sizeof(""));
						break;
					default:
						zend_error(E_WARNING, "Illegal offset type in unset");
						break;
				}
				zval_dtor(free_op2.var);
				break;
			}
			case IS_OBJECT:
				if (!Z_OBJ_HT_P(*container)->unset_dimension) {
					zend_error_noreturn(E_ERROR, "Cannot use object as array");
				}
				if (1) {
					MAKE_REAL_ZVAL_PTR(offset);
				}
				Z_OBJ_HT_P(*container)->unset_dimension(*container, offset TSRMLS_CC);
				if (1) {
					zval_ptr_dtor(&offset);
				} else {
					zval_dtor(free_op2.var);
				}
				break;
			case IS_STRING:
				zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
				ZEND_VM_CONTINUE(); /* bailed out before */
			default:
				zval_dtor(free_op2.var);
				break;
		}
	} else {
		zval_dtor(free_op2.var);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_UNSET_OBJ_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **container = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *offset = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (container) {
		if (IS_UNUSED == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (1) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			Z_OBJ_HT_P(*container)->unset_property(*container, offset TSRMLS_CC);
			if (1) {
				zval_ptr_dtor(&offset);
			} else {
				zval_dtor(free_op2.var);
			}
		} else {
			zval_dtor(free_op2.var);
		}
	} else {
		zval_dtor(free_op2.var);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int zend_isset_isempty_dim_prop_obj_handler_SPEC_UNUSED_TMP(int prop_dim, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **container = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval **value = NULL;
	int result = 0;
	long index;

	if (container) {
		zend_free_op free_op2;
		zval *offset = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (Z_TYPE_PP(container) == IS_ARRAY) {
			HashTable *ht;
			int isset = 0;

			ht = Z_ARRVAL_PP(container);

			switch (Z_TYPE_P(offset)) {
				case IS_DOUBLE:
					index = (long) Z_DVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_RESOURCE:
				case IS_BOOL:
				case IS_LONG:
					index = Z_LVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_STRING:
					if (zend_symtable_find(ht, offset->value.str.val, offset->value.str.len+1, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_NULL:
					if (zend_hash_find(ht, "", sizeof(""), (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				default:
					zend_error(E_WARNING, "Illegal offset type in isset or empty");

					break;
			}

			switch (opline->extended_value) {
				case ZEND_ISSET:
					if (isset && Z_TYPE_PP(value) == IS_NULL) {
						result = 0;
					} else {
						result = isset;
					}
					break;
				case ZEND_ISEMPTY:
					if (!isset || !i_zend_is_true(*value)) {
						result = 0;
					} else {
						result = 1;
					}
					break;
			}
			zval_dtor(free_op2.var);
		} else if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (1) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			if (prop_dim) {
				result = Z_OBJ_HT_P(*container)->has_property(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			} else {
				result = Z_OBJ_HT_P(*container)->has_dimension(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			}
			if (1) {
				zval_ptr_dtor(&offset);
			} else {
				zval_dtor(free_op2.var);
			}
		} else if ((*container)->type == IS_STRING && !prop_dim) { /* string offsets */
			zval tmp;

			if (Z_TYPE_P(offset) != IS_LONG) {
				tmp = *offset;
				zval_copy_ctor(&tmp);
				convert_to_long(&tmp);
				offset = &tmp;
			}
			if (Z_TYPE_P(offset) == IS_LONG) {
				switch (opline->extended_value) {
					case ZEND_ISSET:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container)) {
							result = 1;
						}
						break;
					case ZEND_ISEMPTY:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container) && Z_STRVAL_PP(container)[offset->value.lval] != '0') {
							result = 1;
						}
						break;
				}
			}
			zval_dtor(free_op2.var);
		} else {
			zval_dtor(free_op2.var);
		}
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = result;
			break;
		case ZEND_ISEMPTY:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = !result;
			break;
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_UNUSED_TMP(0, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_UNUSED_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_UNUSED_TMP(1, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_binary_assign_op_obj_helper_SPEC_UNUSED_VAR(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op2, free_op_data1;
	zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *object;
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	znode *result = &opline->result;
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
	} else {
		/* here we are sure we are dealing with an object */
		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = *zptr;
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;

			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					if (Z_OBJ_HT_P(object)->read_property) {
						z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
				case ZEND_ASSIGN_DIM:
					if (Z_OBJ_HT_P(object)->read_dimension) {
						z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (z->refcount == 0) {
						zval_dtor(z);
						FREE_ZVAL(z);
					}
					z = value;
				}
				z->refcount++;
				SEPARATE_ZVAL_IF_NOT_REF(&z);
				binary_op(z, z, value TSRMLS_CC);
				switch (opline->extended_value) {
					case ZEND_ASSIGN_OBJ:
						Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
						break;
					case ZEND_ASSIGN_DIM:
						Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
						break;
				}
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = z;
					PZVAL_LOCK(*retval);
				}
				zval_ptr_dtor(&z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = EG(uninitialized_zval_ptr);
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (0) {
			zval_ptr_dtor(&property);
		} else {
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		}
		FREE_OP(free_op_data1);
	}

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_helper_SPEC_UNUSED_VAR(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2, free_op_data2, free_op_data1;
	zval **var_ptr;
	zval *value;
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			return zend_binary_assign_op_obj_helper_SPEC_UNUSED_VAR(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);

				if (object_ptr && IS_UNUSED != IS_CV && !0) {
					(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
				}

				if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
					return zend_binary_assign_op_obj_helper_SPEC_UNUSED_VAR(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
				} else {
					zend_op *op_data = opline+1;
					zval *dim = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), NULL, dim, 0, BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), &free_op_data2, BP_VAR_RW);
					increment_opline = 1;
				}
			}
			break;
		default:
			value = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
			var_ptr = NULL;
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};

		if (increment_opline) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};

	if (increment_opline) {
		ZEND_VM_INC_OPCODE();
		FREE_OP(free_op_data1);
		FREE_OP_VAR_PTR(free_op_data2);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_ADD_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_VAR(add_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SUB_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_VAR(sub_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MUL_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_VAR(mul_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_DIV_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_VAR(div_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MOD_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_VAR(mod_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SL_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_VAR(shift_left_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SR_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_VAR(shift_right_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_CONCAT_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_VAR(concat_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_OR_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_VAR(bitwise_or_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_AND_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_VAR(bitwise_and_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_XOR_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_VAR(bitwise_xor_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_pre_incdec_property_helper_SPEC_UNUSED_VAR(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *object;
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval **retval = &EX_T(opline->result.u.var).var.ptr;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}

		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			have_get_ptr = 1;
			incdec_op(*zptr);
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = *zptr;
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			z->refcount++;
			SEPARATE_ZVAL_IF_NOT_REF(&z);
			incdec_op(z);
			*retval = z;
			Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = EG(uninitialized_zval_ptr);
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRE_INC_OBJ_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_UNUSED_VAR(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_PRE_DEC_OBJ_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_UNUSED_VAR(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_post_incdec_property_helper_SPEC_UNUSED_VAR(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *object;
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval *retval = &EX_T(opline->result.u.var).tmp_var;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		*retval = *EG(uninitialized_zval_ptr);

		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			have_get_ptr = 1;
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			*retval = **zptr;
			zendi_zval_copy_ctor(*retval);

			incdec_op(*zptr);

		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
			zval *z_copy;

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			*retval = *z;
			zendi_zval_copy_ctor(*retval);
			ALLOC_ZVAL(z_copy);
			*z_copy = *z;
			zendi_zval_copy_ctor(*z_copy);
			INIT_PZVAL(z_copy);
			incdec_op(z_copy);
			z->refcount++;
			Z_OBJ_HT_P(object)->write_property(object, property, z_copy TSRMLS_CC);
			zval_ptr_dtor(&z_copy);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			*retval = *EG(uninitialized_zval_ptr);
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_POST_INC_OBJ_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_UNUSED_VAR(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_POST_DEC_OBJ_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_UNUSED_VAR(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_fetch_property_address_read_helper_SPEC_UNUSED_VAR(int type, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *container;
	zval **retval;


	retval = &EX_T(opline->result.u.var).var.ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = retval;

	container = _get_obj_zval_ptr_unused(TSRMLS_C);

	if (container == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(error_zval_ptr);
			PZVAL_LOCK(*retval);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		ZEND_VM_NEXT_OPCODE();
	}


	if (Z_TYPE_P(container) != IS_OBJECT || !Z_OBJ_HT_P(container)->read_property) {
		if (type != BP_VAR_IS) {
			zend_error(E_NOTICE, "Trying to get property of non-object");
		}
		*retval = EG(uninitialized_zval_ptr);
		SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	} else {
		zend_free_op free_op2;
		zval *offset  = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (0) {
			MAKE_REAL_ZVAL_PTR(offset);
		}

		/* here we are sure we are dealing with an object */
		*retval = Z_OBJ_HT_P(container)->read_property(container, offset, type TSRMLS_CC);

		if (RETURN_VALUE_UNUSED(&opline->result) && ((*retval)->refcount == 0)) {
			zval_dtor(*retval);
			FREE_ZVAL(*retval);
		} else {
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		if (0) {
			zval_ptr_dtor(&offset);
		} else {
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_R_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_UNUSED_VAR(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_W_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK && IS_UNUSED != IS_CV) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
		EX_T(opline->op1.u.var).var.ptr = *EX_T(opline->op1.u.var).var.ptr_ptr;
	}

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_obj_zval_ptr_ptr_unused(TSRMLS_C), property, BP_VAR_W TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}
	if (IS_UNUSED == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_RW_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_obj_zval_ptr_ptr_unused(TSRMLS_C), property, BP_VAR_RW TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}
	if (IS_UNUSED == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_IS_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_UNUSED_VAR(BP_VAR_IS, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_FUNC_ARG_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		/* Behave like FETCH_OBJ_W */
		zend_free_op free_op1, free_op2;
		zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}
		zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_obj_zval_ptr_ptr_unused(TSRMLS_C), property, BP_VAR_W TSRMLS_CC);
		if (0) {
			zval_ptr_dtor(&property);
		} else {
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		}
		if (IS_UNUSED == IS_VAR && 0 &&
		    READY_TO_DESTROY(free_op1.var) &&
		    !RETURN_VALUE_UNUSED(&opline->result)) {
			AI_USE_PTR(EX_T(opline->result.u.var).var);
			if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
			    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
				SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
			}
		}

		ZEND_VM_NEXT_OPCODE();
	} else {
		return zend_fetch_property_address_read_helper_SPEC_UNUSED_VAR(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}
}

static int ZEND_FETCH_OBJ_UNSET_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2, free_res;
	zval **container = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (IS_UNUSED == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, property, BP_VAR_UNSET TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}
	if (IS_UNUSED == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
	if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
		SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
	}
	PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	FREE_OP_VAR_PTR(free_res);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_OBJ_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;

	zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);

	zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_OBJ TSRMLS_CC);

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_METHOD_CALL_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;
	zend_free_op free_op2;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	function_name = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = Z_STRVAL_P(function_name);
	function_name_strlen = Z_STRLEN_P(function_name);

	EX(object) = _get_obj_zval_ptr_unused(TSRMLS_C);

	if (EX(object) && Z_TYPE_P(EX(object)) == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(&EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_UNUSED == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_UNUSED != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_UNSET_DIM_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **container = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *offset = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	long index;

	if (container) {
		if (IS_UNUSED == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		switch (Z_TYPE_PP(container)) {
			case IS_ARRAY: {
				HashTable *ht = Z_ARRVAL_PP(container);

				switch (Z_TYPE_P(offset)) {
					case IS_DOUBLE:
						index = (long) Z_DVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_RESOURCE:
					case IS_BOOL:
					case IS_LONG:
						index = Z_LVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_STRING:
						if (IS_VAR == IS_CV || IS_VAR == IS_VAR) {
							offset->refcount++;
						}
						if (zend_symtable_del(ht, offset->value.str.val, offset->value.str.len+1) == SUCCESS &&
					    ht == &EG(symbol_table)) {
							zend_execute_data *ex;
							ulong hash_value = zend_inline_hash_func(offset->value.str.val, offset->value.str.len+1);

							for (ex = execute_data; ex; ex = ex->prev_execute_data) {
								if (ex->op_array && ex->symbol_table == ht) {
									int i;

									for (i = 0; i < ex->op_array->last_var; i++) {
										if (ex->op_array->vars[i].hash_value == hash_value &&
										    ex->op_array->vars[i].name_len == offset->value.str.len &&
										    !memcmp(ex->op_array->vars[i].name, offset->value.str.val, offset->value.str.len)) {
											ex->CVs[i] = NULL;
											break;
										}
									}
								}
							}
						}
						if (IS_VAR == IS_CV || IS_VAR == IS_VAR) {
							zval_ptr_dtor(&offset);
						}
						break;
					case IS_NULL:
						zend_hash_del(ht, "", sizeof(""));
						break;
					default:
						zend_error(E_WARNING, "Illegal offset type in unset");
						break;
				}
				if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
				break;
			}
			case IS_OBJECT:
				if (!Z_OBJ_HT_P(*container)->unset_dimension) {
					zend_error_noreturn(E_ERROR, "Cannot use object as array");
				}
				if (0) {
					MAKE_REAL_ZVAL_PTR(offset);
				}
				Z_OBJ_HT_P(*container)->unset_dimension(*container, offset TSRMLS_CC);
				if (0) {
					zval_ptr_dtor(&offset);
				} else {
					if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
				}
				break;
			case IS_STRING:
				zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
				ZEND_VM_CONTINUE(); /* bailed out before */
			default:
				if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
				break;
		}
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_UNSET_OBJ_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **container = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *offset = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (container) {
		if (IS_UNUSED == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			Z_OBJ_HT_P(*container)->unset_property(*container, offset TSRMLS_CC);
			if (0) {
				zval_ptr_dtor(&offset);
			} else {
				if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
			}
		} else {
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		}
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}

	ZEND_VM_NEXT_OPCODE();
}

static int zend_isset_isempty_dim_prop_obj_handler_SPEC_UNUSED_VAR(int prop_dim, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **container = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval **value = NULL;
	int result = 0;
	long index;

	if (container) {
		zend_free_op free_op2;
		zval *offset = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (Z_TYPE_PP(container) == IS_ARRAY) {
			HashTable *ht;
			int isset = 0;

			ht = Z_ARRVAL_PP(container);

			switch (Z_TYPE_P(offset)) {
				case IS_DOUBLE:
					index = (long) Z_DVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_RESOURCE:
				case IS_BOOL:
				case IS_LONG:
					index = Z_LVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_STRING:
					if (zend_symtable_find(ht, offset->value.str.val, offset->value.str.len+1, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_NULL:
					if (zend_hash_find(ht, "", sizeof(""), (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				default:
					zend_error(E_WARNING, "Illegal offset type in isset or empty");

					break;
			}

			switch (opline->extended_value) {
				case ZEND_ISSET:
					if (isset && Z_TYPE_PP(value) == IS_NULL) {
						result = 0;
					} else {
						result = isset;
					}
					break;
				case ZEND_ISEMPTY:
					if (!isset || !i_zend_is_true(*value)) {
						result = 0;
					} else {
						result = 1;
					}
					break;
			}
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		} else if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			if (prop_dim) {
				result = Z_OBJ_HT_P(*container)->has_property(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			} else {
				result = Z_OBJ_HT_P(*container)->has_dimension(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			}
			if (0) {
				zval_ptr_dtor(&offset);
			} else {
				if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
			}
		} else if ((*container)->type == IS_STRING && !prop_dim) { /* string offsets */
			zval tmp;

			if (Z_TYPE_P(offset) != IS_LONG) {
				tmp = *offset;
				zval_copy_ctor(&tmp);
				convert_to_long(&tmp);
				offset = &tmp;
			}
			if (Z_TYPE_P(offset) == IS_LONG) {
				switch (opline->extended_value) {
					case ZEND_ISSET:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container)) {
							result = 1;
						}
						break;
					case ZEND_ISEMPTY:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container) && Z_STRVAL_PP(container)[offset->value.lval] != '0') {
							result = 1;
						}
						break;
				}
			}
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		} else {
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		}
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = result;
			break;
		case ZEND_ISEMPTY:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = !result;
			break;
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_UNUSED_VAR(0, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_UNUSED_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_UNUSED_VAR(1, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_binary_assign_op_obj_helper_SPEC_UNUSED_UNUSED(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op_data1;
	zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *object;
	zval *property = NULL;
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	znode *result = &opline->result;
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");

		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
	} else {
		/* here we are sure we are dealing with an object */
		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = *zptr;
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;

			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					if (Z_OBJ_HT_P(object)->read_property) {
						z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
				case ZEND_ASSIGN_DIM:
					if (Z_OBJ_HT_P(object)->read_dimension) {
						z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (z->refcount == 0) {
						zval_dtor(z);
						FREE_ZVAL(z);
					}
					z = value;
				}
				z->refcount++;
				SEPARATE_ZVAL_IF_NOT_REF(&z);
				binary_op(z, z, value TSRMLS_CC);
				switch (opline->extended_value) {
					case ZEND_ASSIGN_OBJ:
						Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
						break;
					case ZEND_ASSIGN_DIM:
						Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
						break;
				}
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = z;
					PZVAL_LOCK(*retval);
				}
				zval_ptr_dtor(&z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = EG(uninitialized_zval_ptr);
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (0) {
			zval_ptr_dtor(&property);
		} else {

		}
		FREE_OP(free_op_data1);
	}

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_helper_SPEC_UNUSED_UNUSED(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op_data2, free_op_data1;
	zval **var_ptr;
	zval *value;
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			return zend_binary_assign_op_obj_helper_SPEC_UNUSED_UNUSED(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);

				if (object_ptr && IS_UNUSED != IS_CV && !0) {
					(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
				}

				if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
					return zend_binary_assign_op_obj_helper_SPEC_UNUSED_UNUSED(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
				} else {
					zend_op *op_data = opline+1;
					zval *dim = NULL;

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), NULL, dim, 0, BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), &free_op_data2, BP_VAR_RW);
					increment_opline = 1;
				}
			}
			break;
		default:
			value = NULL;
			var_ptr = NULL;
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}


		if (increment_opline) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	if (increment_opline) {
		ZEND_VM_INC_OPCODE();
		FREE_OP(free_op_data1);
		FREE_OP_VAR_PTR(free_op_data2);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_ADD_SPEC_UNUSED_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_UNUSED(add_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SUB_SPEC_UNUSED_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_UNUSED(sub_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MUL_SPEC_UNUSED_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_UNUSED(mul_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_DIV_SPEC_UNUSED_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_UNUSED(div_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MOD_SPEC_UNUSED_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_UNUSED(mod_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SL_SPEC_UNUSED_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_UNUSED(shift_left_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SR_SPEC_UNUSED_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_UNUSED(shift_right_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_CONCAT_SPEC_UNUSED_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_UNUSED(concat_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_OR_SPEC_UNUSED_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_UNUSED(bitwise_or_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_AND_SPEC_UNUSED_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_UNUSED(bitwise_and_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_XOR_SPEC_UNUSED_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_UNUSED(bitwise_xor_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_INIT_ARRAY_SPEC_UNUSED_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_UNUSED == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_UNUSED != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_UNUSED_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int zend_binary_assign_op_obj_helper_SPEC_UNUSED_CV(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op_data1;
	zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *object;
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	znode *result = &opline->result;
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");

		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
	} else {
		/* here we are sure we are dealing with an object */
		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = *zptr;
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;

			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					if (Z_OBJ_HT_P(object)->read_property) {
						z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
				case ZEND_ASSIGN_DIM:
					if (Z_OBJ_HT_P(object)->read_dimension) {
						z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (z->refcount == 0) {
						zval_dtor(z);
						FREE_ZVAL(z);
					}
					z = value;
				}
				z->refcount++;
				SEPARATE_ZVAL_IF_NOT_REF(&z);
				binary_op(z, z, value TSRMLS_CC);
				switch (opline->extended_value) {
					case ZEND_ASSIGN_OBJ:
						Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
						break;
					case ZEND_ASSIGN_DIM:
						Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
						break;
				}
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = z;
					PZVAL_LOCK(*retval);
				}
				zval_ptr_dtor(&z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = EG(uninitialized_zval_ptr);
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (0) {
			zval_ptr_dtor(&property);
		} else {

		}
		FREE_OP(free_op_data1);
	}

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_helper_SPEC_UNUSED_CV(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op_data2, free_op_data1;
	zval **var_ptr;
	zval *value;
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			return zend_binary_assign_op_obj_helper_SPEC_UNUSED_CV(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);

				if (object_ptr && IS_UNUSED != IS_CV && !0) {
					(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
				}

				if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
					return zend_binary_assign_op_obj_helper_SPEC_UNUSED_CV(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
				} else {
					zend_op *op_data = opline+1;
					zval *dim = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), NULL, dim, 0, BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), &free_op_data2, BP_VAR_RW);
					increment_opline = 1;
				}
			}
			break;
		default:
			value = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
			var_ptr = NULL;
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}


		if (increment_opline) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	if (increment_opline) {
		ZEND_VM_INC_OPCODE();
		FREE_OP(free_op_data1);
		FREE_OP_VAR_PTR(free_op_data2);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_ADD_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CV(add_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SUB_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CV(sub_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MUL_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CV(mul_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_DIV_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CV(div_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MOD_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CV(mod_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SL_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CV(shift_left_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SR_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CV(shift_right_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_CONCAT_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CV(concat_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_OR_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CV(bitwise_or_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_AND_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CV(bitwise_and_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_XOR_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_UNUSED_CV(bitwise_xor_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_pre_incdec_property_helper_SPEC_UNUSED_CV(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *object;
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval **retval = &EX_T(opline->result.u.var).var.ptr;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");

		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}

		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			have_get_ptr = 1;
			incdec_op(*zptr);
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = *zptr;
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			z->refcount++;
			SEPARATE_ZVAL_IF_NOT_REF(&z);
			incdec_op(z);
			*retval = z;
			Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = EG(uninitialized_zval_ptr);
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRE_INC_OBJ_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_UNUSED_CV(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_PRE_DEC_OBJ_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_UNUSED_CV(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_post_incdec_property_helper_SPEC_UNUSED_CV(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *object;
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval *retval = &EX_T(opline->result.u.var).tmp_var;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");

		*retval = *EG(uninitialized_zval_ptr);

		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			have_get_ptr = 1;
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			*retval = **zptr;
			zendi_zval_copy_ctor(*retval);

			incdec_op(*zptr);

		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
			zval *z_copy;

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			*retval = *z;
			zendi_zval_copy_ctor(*retval);
			ALLOC_ZVAL(z_copy);
			*z_copy = *z;
			zendi_zval_copy_ctor(*z_copy);
			INIT_PZVAL(z_copy);
			incdec_op(z_copy);
			z->refcount++;
			Z_OBJ_HT_P(object)->write_property(object, property, z_copy TSRMLS_CC);
			zval_ptr_dtor(&z_copy);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			*retval = *EG(uninitialized_zval_ptr);
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_POST_INC_OBJ_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_UNUSED_CV(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_POST_DEC_OBJ_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_UNUSED_CV(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_fetch_property_address_read_helper_SPEC_UNUSED_CV(int type, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *container;
	zval **retval;


	retval = &EX_T(opline->result.u.var).var.ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = retval;

	container = _get_obj_zval_ptr_unused(TSRMLS_C);

	if (container == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(error_zval_ptr);
			PZVAL_LOCK(*retval);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		ZEND_VM_NEXT_OPCODE();
	}


	if (Z_TYPE_P(container) != IS_OBJECT || !Z_OBJ_HT_P(container)->read_property) {
		if (type != BP_VAR_IS) {
			zend_error(E_NOTICE, "Trying to get property of non-object");
		}
		*retval = EG(uninitialized_zval_ptr);
		SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	} else {

		zval *offset  = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

		if (0) {
			MAKE_REAL_ZVAL_PTR(offset);
		}

		/* here we are sure we are dealing with an object */
		*retval = Z_OBJ_HT_P(container)->read_property(container, offset, type TSRMLS_CC);

		if (RETURN_VALUE_UNUSED(&opline->result) && ((*retval)->refcount == 0)) {
			zval_dtor(*retval);
			FREE_ZVAL(*retval);
		} else {
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		if (0) {
			zval_ptr_dtor(&offset);
		} else {

		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_R_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_UNUSED_CV(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_W_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK && IS_UNUSED != IS_CV) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
		EX_T(opline->op1.u.var).var.ptr = *EX_T(opline->op1.u.var).var.ptr_ptr;
	}

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_obj_zval_ptr_ptr_unused(TSRMLS_C), property, BP_VAR_W TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_UNUSED == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_RW_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_obj_zval_ptr_ptr_unused(TSRMLS_C), property, BP_VAR_RW TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_UNUSED == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_IS_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_UNUSED_CV(BP_VAR_IS, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_FUNC_ARG_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		/* Behave like FETCH_OBJ_W */
		zend_free_op free_op1;
		zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}
		zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_obj_zval_ptr_ptr_unused(TSRMLS_C), property, BP_VAR_W TSRMLS_CC);
		if (0) {
			zval_ptr_dtor(&property);
		} else {

		}
		if (IS_UNUSED == IS_VAR && 0 &&
		    READY_TO_DESTROY(free_op1.var) &&
		    !RETURN_VALUE_UNUSED(&opline->result)) {
			AI_USE_PTR(EX_T(opline->result.u.var).var);
			if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
			    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
				SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
			}
		}

		ZEND_VM_NEXT_OPCODE();
	} else {
		return zend_fetch_property_address_read_helper_SPEC_UNUSED_CV(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}
}

static int ZEND_FETCH_OBJ_UNSET_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_res;
	zval **container = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (IS_UNUSED == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, property, BP_VAR_UNSET TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_UNUSED == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
	if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
		SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
	}
	PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	FREE_OP_VAR_PTR(free_res);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_OBJ_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;

	zval **object_ptr = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);

	zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_OBJ TSRMLS_CC);

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_METHOD_CALL_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;


	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	function_name = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = Z_STRVAL_P(function_name);
	function_name_strlen = Z_STRLEN_P(function_name);

	EX(object) = _get_obj_zval_ptr_unused(TSRMLS_C);

	if (EX(object) && Z_TYPE_P(EX(object)) == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(&EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_UNUSED == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_UNUSED != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_UNSET_DIM_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **container = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *offset = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
	long index;

	if (container) {
		if (IS_UNUSED == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		switch (Z_TYPE_PP(container)) {
			case IS_ARRAY: {
				HashTable *ht = Z_ARRVAL_PP(container);

				switch (Z_TYPE_P(offset)) {
					case IS_DOUBLE:
						index = (long) Z_DVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_RESOURCE:
					case IS_BOOL:
					case IS_LONG:
						index = Z_LVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_STRING:
						if (IS_CV == IS_CV || IS_CV == IS_VAR) {
							offset->refcount++;
						}
						if (zend_symtable_del(ht, offset->value.str.val, offset->value.str.len+1) == SUCCESS &&
					    ht == &EG(symbol_table)) {
							zend_execute_data *ex;
							ulong hash_value = zend_inline_hash_func(offset->value.str.val, offset->value.str.len+1);

							for (ex = execute_data; ex; ex = ex->prev_execute_data) {
								if (ex->op_array && ex->symbol_table == ht) {
									int i;

									for (i = 0; i < ex->op_array->last_var; i++) {
										if (ex->op_array->vars[i].hash_value == hash_value &&
										    ex->op_array->vars[i].name_len == offset->value.str.len &&
										    !memcmp(ex->op_array->vars[i].name, offset->value.str.val, offset->value.str.len)) {
											ex->CVs[i] = NULL;
											break;
										}
									}
								}
							}
						}
						if (IS_CV == IS_CV || IS_CV == IS_VAR) {
							zval_ptr_dtor(&offset);
						}
						break;
					case IS_NULL:
						zend_hash_del(ht, "", sizeof(""));
						break;
					default:
						zend_error(E_WARNING, "Illegal offset type in unset");
						break;
				}

				break;
			}
			case IS_OBJECT:
				if (!Z_OBJ_HT_P(*container)->unset_dimension) {
					zend_error_noreturn(E_ERROR, "Cannot use object as array");
				}
				if (0) {
					MAKE_REAL_ZVAL_PTR(offset);
				}
				Z_OBJ_HT_P(*container)->unset_dimension(*container, offset TSRMLS_CC);
				if (0) {
					zval_ptr_dtor(&offset);
				} else {

				}
				break;
			case IS_STRING:
				zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
				ZEND_VM_CONTINUE(); /* bailed out before */
			default:

				break;
		}
	} else {

	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_UNSET_OBJ_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **container = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval *offset = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (container) {
		if (IS_UNUSED == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			Z_OBJ_HT_P(*container)->unset_property(*container, offset TSRMLS_CC);
			if (0) {
				zval_ptr_dtor(&offset);
			} else {

			}
		} else {

		}
	} else {

	}

	ZEND_VM_NEXT_OPCODE();
}

static int zend_isset_isempty_dim_prop_obj_handler_SPEC_UNUSED_CV(int prop_dim, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **container = _get_obj_zval_ptr_ptr_unused(TSRMLS_C);
	zval **value = NULL;
	int result = 0;
	long index;

	if (container) {

		zval *offset = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

		if (Z_TYPE_PP(container) == IS_ARRAY) {
			HashTable *ht;
			int isset = 0;

			ht = Z_ARRVAL_PP(container);

			switch (Z_TYPE_P(offset)) {
				case IS_DOUBLE:
					index = (long) Z_DVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_RESOURCE:
				case IS_BOOL:
				case IS_LONG:
					index = Z_LVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_STRING:
					if (zend_symtable_find(ht, offset->value.str.val, offset->value.str.len+1, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_NULL:
					if (zend_hash_find(ht, "", sizeof(""), (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				default:
					zend_error(E_WARNING, "Illegal offset type in isset or empty");

					break;
			}

			switch (opline->extended_value) {
				case ZEND_ISSET:
					if (isset && Z_TYPE_PP(value) == IS_NULL) {
						result = 0;
					} else {
						result = isset;
					}
					break;
				case ZEND_ISEMPTY:
					if (!isset || !i_zend_is_true(*value)) {
						result = 0;
					} else {
						result = 1;
					}
					break;
			}

		} else if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			if (prop_dim) {
				result = Z_OBJ_HT_P(*container)->has_property(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			} else {
				result = Z_OBJ_HT_P(*container)->has_dimension(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			}
			if (0) {
				zval_ptr_dtor(&offset);
			} else {

			}
		} else if ((*container)->type == IS_STRING && !prop_dim) { /* string offsets */
			zval tmp;

			if (Z_TYPE_P(offset) != IS_LONG) {
				tmp = *offset;
				zval_copy_ctor(&tmp);
				convert_to_long(&tmp);
				offset = &tmp;
			}
			if (Z_TYPE_P(offset) == IS_LONG) {
				switch (opline->extended_value) {
					case ZEND_ISSET:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container)) {
							result = 1;
						}
						break;
					case ZEND_ISEMPTY:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container) && Z_STRVAL_PP(container)[offset->value.lval] != '0') {
							result = 1;
						}
						break;
				}
			}

		} else {

		}
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = result;
			break;
		case ZEND_ISEMPTY:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = !result;
			break;
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_UNUSED_CV(0, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_UNUSED_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_UNUSED_CV(1, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_BW_NOT_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	bitwise_not_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_NOT_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	boolean_not_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRE_INC_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **var_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC);

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		val->refcount++;
		increment_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		increment_function(*var_ptr);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRE_DEC_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **var_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC);

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		val->refcount++;
		decrement_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		decrement_function(*var_ptr);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_POST_INC_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **var_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC);

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).tmp_var = *EG(uninitialized_zval_ptr);
		}

		ZEND_VM_NEXT_OPCODE();
	}

	EX_T(opline->result.u.var).tmp_var = **var_ptr;
	zendi_zval_copy_ctor(EX_T(opline->result.u.var).tmp_var);

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		val->refcount++;
		increment_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		increment_function(*var_ptr);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_POST_DEC_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **var_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC);

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).tmp_var = *EG(uninitialized_zval_ptr);
		}

		ZEND_VM_NEXT_OPCODE();
	}

	EX_T(opline->result.u.var).tmp_var = **var_ptr;
	zendi_zval_copy_ctor(EX_T(opline->result.u.var).tmp_var);

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		val->refcount++;
		decrement_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		decrement_function(*var_ptr);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ECHO_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval z_copy;
	zval *z = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get_method != NULL &&
		zend_std_cast_object_tostring(z, &z_copy, IS_STRING TSRMLS_CC) == SUCCESS) {
		zend_print_variable(&z_copy);
		zval_dtor(&z_copy);
	} else {
		zend_print_variable(z);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRINT_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 1;
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_LONG;

	return ZEND_ECHO_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_fetch_var_address_helper_SPEC_CV(int type, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *varname = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval **retval;
	zval tmp_varname;
	HashTable *target_symbol_table;

 	if (Z_TYPE_P(varname) != IS_STRING) {
		tmp_varname = *varname;
		zval_copy_ctor(&tmp_varname);
		convert_to_string(&tmp_varname);
		varname = &tmp_varname;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		retval = zend_std_get_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname), 0 TSRMLS_CC);
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), type, varname TSRMLS_CC);
/*
		if (!target_symbol_table) {
			ZEND_VM_NEXT_OPCODE();
		}
*/
		if (zend_hash_find(target_symbol_table, varname->value.str.val, varname->value.str.len+1, (void **) &retval) == FAILURE) {
			switch (type) {
				case BP_VAR_R:
				case BP_VAR_UNSET:
					zend_error(E_NOTICE,"Undefined variable: %s", Z_STRVAL_P(varname));
					/* break missing intentionally */
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval_ptr);
					break;
				case BP_VAR_RW:
					zend_error(E_NOTICE,"Undefined variable: %s", Z_STRVAL_P(varname));
					/* break missing intentionally */
				case BP_VAR_W: {
						zval *new_zval = &EG(uninitialized_zval);

						new_zval->refcount++;
						zend_hash_update(target_symbol_table, varname->value.str.val, varname->value.str.len+1, &new_zval, sizeof(zval *), (void **) &retval);
					}
					break;
				EMPTY_SWITCH_DEFAULT_CASE()
			}
		}
		switch (opline->op2.u.EA.type) {
			case ZEND_FETCH_GLOBAL:
				if (IS_CV != IS_TMP_VAR) {

				}
				break;
			case ZEND_FETCH_LOCAL:

				break;
			case ZEND_FETCH_STATIC:
				zval_update_constant(retval, (void*) 1 TSRMLS_CC);
				break;
			case ZEND_FETCH_GLOBAL_LOCK:
				if (IS_CV == IS_VAR && !free_op1.var) {
					PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
				}
				break;
		}
	}


	if (varname == &tmp_varname) {
		zval_dtor(varname);
	}
	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = retval;
		PZVAL_LOCK(*retval);
		switch (type) {
			case BP_VAR_R:
			case BP_VAR_IS:
				AI_USE_PTR(EX_T(opline->result.u.var).var);
				break;
			case BP_VAR_UNSET: {
				zend_free_op free_res;

				PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
				if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
					SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
				}
				PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
				FREE_OP_VAR_PTR(free_res);
				break;
			}
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_R_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_CV(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_W_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_CV(BP_VAR_W, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_RW_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_CV(BP_VAR_RW, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_FUNC_ARG_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_CV(ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), EX(opline)->extended_value)?BP_VAR_W:BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_UNSET_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_CV(BP_VAR_UNSET, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_IS_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_var_address_helper_SPEC_CV(BP_VAR_IS, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_JMPZ_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	int ret = i_zend_is_true(_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC));

	if (!ret) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_JMPNZ_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	int ret = i_zend_is_true(_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC));

	if (ret) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_JMPZNZ_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	int retval = i_zend_is_true(_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC));

	if (retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp on true to %d\n", opline->extended_value);
#endif
		ZEND_VM_JMP(&EX(op_array)->opcodes[opline->extended_value]);
	} else {
#if DEBUG_ZEND>=2
		printf("Conditional jmp on false to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(&EX(op_array)->opcodes[opline->op2.u.opline_num]);
	}
}

static int ZEND_JMPZ_EX_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	int retval = i_zend_is_true(_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC));

	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = retval;
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;
	if (!retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_JMPNZ_EX_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	int retval = i_zend_is_true(_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC));

	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = retval;
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;
	if (retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_RETURN_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *retval_ptr;
	zval **retval_ptr_ptr;


	if (EG(active_op_array)->return_reference == ZEND_RETURN_REF) {

		if (IS_CV == IS_CONST || IS_CV == IS_TMP_VAR) {
			/* Not supposed to happen, but we'll allow it */
			zend_error(E_NOTICE, "Only variable references should be returned by reference");
			goto return_by_value;
		}

		retval_ptr_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);

		if (!retval_ptr_ptr) {
			zend_error_noreturn(E_ERROR, "Cannot return string offsets by reference");
		}

		if (IS_CV == IS_VAR && !(*retval_ptr_ptr)->is_ref) {
			if (opline->extended_value == ZEND_RETURNS_FUNCTION &&
			    EX_T(opline->op1.u.var).var.fcall_returned_reference) {
			} else if (EX_T(opline->op1.u.var).var.ptr_ptr == &EX_T(opline->op1.u.var).var.ptr) {
				if (IS_CV == IS_VAR && !0) {
					PZVAL_LOCK(*retval_ptr_ptr); /* undo the effect of get_zval_ptr_ptr() */
				}
				zend_error(E_NOTICE, "Only variable references should be returned by reference");
				goto return_by_value;
			}
		}

		SEPARATE_ZVAL_TO_MAKE_IS_REF(retval_ptr_ptr);
		(*retval_ptr_ptr)->refcount++;

		(*EG(return_value_ptr_ptr)) = (*retval_ptr_ptr);
	} else {
return_by_value:

		retval_ptr = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);

		if (EG(ze1_compatibility_mode) && Z_TYPE_P(retval_ptr) == IS_OBJECT) {
			zval *ret;
			char *class_name;
			zend_uint class_name_len;
			int dup;

			ALLOC_ZVAL(ret);
			INIT_PZVAL_COPY(ret, retval_ptr);
			dup = zend_get_object_classname(retval_ptr, &class_name, &class_name_len TSRMLS_CC);
			if (Z_OBJ_HT_P(retval_ptr)->clone_obj == NULL) {
				zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object of class %s",  class_name);
			}
			zend_error(E_STRICT, "Implicit cloning object of class '%s' because of 'zend.ze1_compatibility_mode'", class_name);
			ret->value.obj = Z_OBJ_HT_P(retval_ptr)->clone_obj(retval_ptr TSRMLS_CC);
			*EG(return_value_ptr_ptr) = ret;
			if (!dup) {
				efree(class_name);
			}
		} else if (!0) { /* Not a temp var */
			if (EG(active_op_array)->return_reference == ZEND_RETURN_REF ||
			    (PZVAL_IS_REF(retval_ptr) && retval_ptr->refcount > 0)) {
				zval *ret;

				ALLOC_ZVAL(ret);
				INIT_PZVAL_COPY(ret, retval_ptr);
				zval_copy_ctor(ret);
				*EG(return_value_ptr_ptr) = ret;
			} else {
				*EG(return_value_ptr_ptr) = retval_ptr;
				retval_ptr->refcount++;
			}
		} else {
			zval *ret;

			ALLOC_ZVAL(ret);
			INIT_PZVAL_COPY(ret, retval_ptr);
			*EG(return_value_ptr_ptr) = ret;
		}
	}

	ZEND_VM_RETURN_FROM_EXECUTE_LOOP();
}

static int ZEND_THROW_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *value;
	zval *exception;


	value = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (Z_TYPE_P(value) != IS_OBJECT) {
		zend_error_noreturn(E_ERROR, "Can only throw objects");
	}
	/* Not sure if a complete copy is what we want here */
	ALLOC_ZVAL(exception);
	INIT_PZVAL_COPY(exception, value);
	if (!0) {
		zval_copy_ctor(exception);
	}

	zend_throw_exception_object(exception TSRMLS_CC);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SEND_VAL_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	if (opline->extended_value==ZEND_DO_FCALL_BY_NAME
		&& ARG_MUST_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
			zend_error_noreturn(E_ERROR, "Cannot pass parameter %d by reference", opline->op2.u.opline_num);
	}
	{
		zval *valptr;
		zval *value;


		value = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);

		ALLOC_ZVAL(valptr);
		INIT_PZVAL_COPY(valptr, value);
		if (!0) {
			zval_copy_ctor(valptr);
		}
		zend_ptr_stack_push(&EG(argument_stack), valptr);

	}
	ZEND_VM_NEXT_OPCODE();
}

static int zend_send_by_var_helper_SPEC_CV(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *varptr;

	varptr = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (varptr == &EG(uninitialized_zval)) {
		ALLOC_ZVAL(varptr);
		INIT_ZVAL(*varptr);
		varptr->refcount = 0;
	} else if (PZVAL_IS_REF(varptr)) {
		zval *original_var = varptr;

		ALLOC_ZVAL(varptr);
		*varptr = *original_var;
		varptr->is_ref = 0;
		varptr->refcount = 0;
		zval_copy_ctor(varptr);
	}
	varptr->refcount++;
	zend_ptr_stack_push(&EG(argument_stack), varptr);
	;  /* for string offsets */

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SEND_VAR_NO_REF_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *varptr;

	if (opline->extended_value & ZEND_ARG_COMPILE_TIME_BOUND) { /* Had function_ptr at compile_time */
		if (!(opline->extended_value & ZEND_ARG_SEND_BY_REF)) {
			return zend_send_by_var_helper_SPEC_CV(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
		}
	} else if (!ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
		return zend_send_by_var_helper_SPEC_CV(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}

	if (IS_CV == IS_VAR &&
		(opline->extended_value & ZEND_ARG_SEND_FUNCTION) &&
		EX_T(opline->op1.u.var).var.fcall_returned_reference &&
		EX_T(opline->op1.u.var).var.ptr) {
		varptr = EX_T(opline->op1.u.var).var.ptr;
		PZVAL_UNLOCK_EX(varptr, &free_op1, 0);
	} else {
		varptr = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
	}
	if ((!(opline->extended_value & ZEND_ARG_SEND_FUNCTION) ||
	     EX_T(opline->op1.u.var).var.fcall_returned_reference) &&
	    varptr != &EG(uninitialized_zval) &&
	    (PZVAL_IS_REF(varptr) ||
	     (varptr->refcount == 1 && (IS_CV == IS_CV || free_op1.var)))) {
		varptr->is_ref = 1;
		varptr->refcount++;
		zend_ptr_stack_push(&EG(argument_stack), varptr);
	} else {
		zval *valptr;

		zend_error(E_STRICT, "Only variables should be passed by reference");
		ALLOC_ZVAL(valptr);
		INIT_PZVAL_COPY(valptr, varptr);
		if (!0) {
			zval_copy_ctor(valptr);
		}
		zend_ptr_stack_push(&EG(argument_stack), valptr);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SEND_REF_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **varptr_ptr;
	zval *varptr;
	varptr_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);

	if (!varptr_ptr) {
		zend_error_noreturn(E_ERROR, "Only variables can be passed by reference");
	}

	SEPARATE_ZVAL_TO_MAKE_IS_REF(varptr_ptr);
	varptr = *varptr_ptr;
	varptr->refcount++;
	zend_ptr_stack_push(&EG(argument_stack), varptr);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SEND_VAR_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	if ((opline->extended_value == ZEND_DO_FCALL_BY_NAME)
		&& ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
		return ZEND_SEND_REF_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}
	return zend_send_by_var_helper_SPEC_CV(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_BOOL_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	/* PHP 3.0 returned "" for false and 1 for true, here we use 0 and 1 for now */
	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = i_zend_is_true(_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC));
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CLONE_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *obj = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
	zend_class_entry *ce;
	zend_function *clone;
	zend_object_clone_obj_t clone_call;

	if (!obj || Z_TYPE_P(obj) != IS_OBJECT) {
		zend_error_noreturn(E_ERROR, "__clone method called on non-object");
		EX_T(opline->result.u.var).var.ptr = EG(error_zval_ptr);
		EX_T(opline->result.u.var).var.ptr->refcount++;

		ZEND_VM_NEXT_OPCODE();
	}

	ce = Z_OBJCE_P(obj);
	clone = ce ? ce->clone : NULL;
	clone_call =  Z_OBJ_HT_P(obj)->clone_obj;
	if (!clone_call) {
		if (ce) {
			zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object of class %s", ce->name);
		} else {
			zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object");
		}
		EX_T(opline->result.u.var).var.ptr = EG(error_zval_ptr);
		EX_T(opline->result.u.var).var.ptr->refcount++;
	}

	if (ce && clone) {
		if (clone->op_array.fn_flags & ZEND_ACC_PRIVATE) {
			/* Ensure that if we're calling a private function, we're allowed to do so.
			 */
			if (ce != EG(scope)) {
				zend_error_noreturn(E_ERROR, "Call to private %s::__clone() from context '%s'", ce->name, EG(scope) ? EG(scope)->name : "");
			}
		} else if ((clone->common.fn_flags & ZEND_ACC_PROTECTED)) {
			/* Ensure that if we're calling a protected function, we're allowed to do so.
			 */
			if (!zend_check_protected(clone->common.scope, EG(scope))) {
				zend_error_noreturn(E_ERROR, "Call to protected %s::__clone() from context '%s'", ce->name, EG(scope) ? EG(scope)->name : "");
			}
		}
	}

	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	if (!EG(exception)) {
		ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
		Z_OBJVAL_P(EX_T(opline->result.u.var).var.ptr) = clone_call(obj TSRMLS_CC);
		Z_TYPE_P(EX_T(opline->result.u.var).var.ptr) = IS_OBJECT;
		EX_T(opline->result.u.var).var.ptr->refcount=1;
		EX_T(opline->result.u.var).var.ptr->is_ref=1;
		if (!RETURN_VALUE_USED(opline) || EG(exception)) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CAST_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *expr = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval *result = &EX_T(opline->result.u.var).tmp_var;

	if (opline->extended_value != IS_STRING) {
		*result = *expr;
		if (!0) {
			zendi_zval_copy_ctor(*result);
		}
	}
	switch (opline->extended_value) {
		case IS_NULL:
			convert_to_null(result);
			break;
		case IS_BOOL:
			convert_to_boolean(result);
			break;
		case IS_LONG:
			convert_to_long(result);
			break;
		case IS_DOUBLE:
			convert_to_double(result);
			break;
		case IS_STRING: {
			zval var_copy;
			int use_copy;

			zend_make_printable_zval(expr, &var_copy, &use_copy);
			if (use_copy) {
				*result = var_copy;
				if (0) {

				}
			} else {
				*result = *expr;
				if (!0) {
					zendi_zval_copy_ctor(*result);
				}
			}
			break;
		}
		case IS_ARRAY:
			convert_to_array(result);
			break;
		case IS_OBJECT:
			convert_to_object(result);
			break;
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INCLUDE_OR_EVAL_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op_array *new_op_array=NULL;
	zval **original_return_value = EG(return_value_ptr_ptr);
	int return_value_used;

	zval *inc_filename = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval tmp_inc_filename;
	zend_bool failure_retval=0;

	if (inc_filename->type!=IS_STRING) {
		tmp_inc_filename = *inc_filename;
		zval_copy_ctor(&tmp_inc_filename);
		convert_to_string(&tmp_inc_filename);
		inc_filename = &tmp_inc_filename;
	}

	return_value_used = RETURN_VALUE_USED(opline);

	switch (Z_LVAL(opline->op2.u.constant)) {
		case ZEND_INCLUDE_ONCE:
		case ZEND_REQUIRE_ONCE: {
				zend_file_handle file_handle;

				if (IS_ABSOLUTE_PATH(Z_STRVAL_P(inc_filename), Z_STRLEN_P(inc_filename))) {
					cwd_state state;

					state.cwd_length = 0;
					state.cwd = malloc(1);
					state.cwd[0] = 0;

					failure_retval = (!virtual_file_ex(&state, Z_STRVAL_P(inc_filename), NULL, 1) &&
						zend_hash_exists(&EG(included_files), state.cwd, state.cwd_length+1));

					free(state.cwd);
				}

				if (failure_retval) {
					/* do nothing */
				} else if (SUCCESS == zend_stream_open(Z_STRVAL_P(inc_filename), &file_handle TSRMLS_CC)) {

					if (!file_handle.opened_path) {
						file_handle.opened_path = estrndup(Z_STRVAL_P(inc_filename), Z_STRLEN_P(inc_filename));
					}

					if (zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path, strlen(file_handle.opened_path)+1)==SUCCESS) {
						new_op_array = zend_compile_file(&file_handle, (Z_LVAL(opline->op2.u.constant)==ZEND_INCLUDE_ONCE?ZEND_INCLUDE:ZEND_REQUIRE) TSRMLS_CC);
						zend_destroy_file_handle(&file_handle TSRMLS_CC);
					} else {
						zend_file_handle_dtor(&file_handle);
						failure_retval=1;
					}
				} else {
					if (Z_LVAL(opline->op2.u.constant)==ZEND_INCLUDE_ONCE) {
						zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, Z_STRVAL_P(inc_filename));
					} else {
						zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, Z_STRVAL_P(inc_filename));
					}
				}
			}
			break;
		case ZEND_INCLUDE:
		case ZEND_REQUIRE:
			new_op_array = compile_filename(Z_LVAL(opline->op2.u.constant), inc_filename TSRMLS_CC);
			break;
		case ZEND_EVAL: {
				char *eval_desc = zend_make_compiled_string_description("eval()'d code" TSRMLS_CC);

				new_op_array = zend_compile_string(inc_filename, eval_desc TSRMLS_CC);
				efree(eval_desc);
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	if (inc_filename==&tmp_inc_filename) {
		zval_dtor(&tmp_inc_filename);
	}
	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	if (new_op_array) {
		zval *saved_object;
		zend_function *saved_function;

		EG(return_value_ptr_ptr) = EX_T(opline->result.u.var).var.ptr_ptr;
		EG(active_op_array) = new_op_array;
		EX_T(opline->result.u.var).var.ptr = NULL;

		saved_object = EX(object);
		saved_function = EX(function_state).function;

		EX(function_state).function = (zend_function *) new_op_array;
		EX(object) = NULL;

		zend_execute(new_op_array TSRMLS_CC);

		EX(function_state).function = saved_function;
		EX(object) = saved_object;

		if (!return_value_used) {
			if (EX_T(opline->result.u.var).var.ptr) {
				zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
			}
		} else { /* return value is used */
			if (!EX_T(opline->result.u.var).var.ptr) { /* there was no return statement */
				ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
				INIT_PZVAL(EX_T(opline->result.u.var).var.ptr);
				Z_LVAL_P(EX_T(opline->result.u.var).var.ptr) = 1;
				Z_TYPE_P(EX_T(opline->result.u.var).var.ptr) = IS_BOOL;
			}
		}

		EG(opline_ptr) = &EX(opline);
		EG(active_op_array) = EX(op_array);
		EG(function_state_ptr) = &EX(function_state);
		destroy_op_array(new_op_array TSRMLS_CC);
		efree(new_op_array);
		if (EG(exception)) {
			zend_throw_exception_internal(NULL TSRMLS_CC);
		}
	} else {
		if (return_value_used) {
			ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
			INIT_ZVAL(*EX_T(opline->result.u.var).var.ptr);
			Z_LVAL_P(EX_T(opline->result.u.var).var.ptr) = failure_retval;
			Z_TYPE_P(EX_T(opline->result.u.var).var.ptr) = IS_BOOL;
		}
	}

	EG(return_value_ptr_ptr) = original_return_value;
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_UNSET_VAR_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval tmp, *varname;
	HashTable *target_symbol_table;


	varname = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (Z_TYPE_P(varname) != IS_STRING) {
		tmp = *varname;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		varname = &tmp;
	} else if (IS_CV == IS_CV || IS_CV == IS_VAR) {
		varname->refcount++;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		zend_std_unset_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname) TSRMLS_CC);
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), BP_VAR_IS, varname TSRMLS_CC);
		if (zend_hash_del(target_symbol_table, varname->value.str.val, varname->value.str.len+1) == SUCCESS) {
			zend_execute_data *ex = execute_data;
			ulong hash_value = zend_inline_hash_func(varname->value.str.val, varname->value.str.len+1);

			do {
				int i;

				if (ex->op_array) {
					for (i = 0; i < ex->op_array->last_var; i++) {
						if (ex->op_array->vars[i].hash_value == hash_value &&
							ex->op_array->vars[i].name_len == varname->value.str.len &&
							!memcmp(ex->op_array->vars[i].name, varname->value.str.val, varname->value.str.len)) {
							ex->CVs[i] = NULL;
							break;
						}
					}
				}
				ex = ex->prev_execute_data;
			} while (ex && ex->symbol_table == target_symbol_table);
		}
	}

	if (varname == &tmp) {
		zval_dtor(&tmp);
	} else if (IS_CV == IS_CV || IS_CV == IS_VAR) {
		zval_ptr_dtor(&varname);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FE_RESET_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *array_ptr, **array_ptr_ptr;
	HashTable *fe_ht;
	zend_object_iterator *iter = NULL;
	zend_class_entry *ce = NULL;
	zend_bool is_empty = 0;

	if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {
		array_ptr_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
		if (array_ptr_ptr == NULL || array_ptr_ptr == &EG(uninitialized_zval_ptr)) {
			ALLOC_INIT_ZVAL(array_ptr);
		} else if (Z_TYPE_PP(array_ptr_ptr) == IS_OBJECT) {
			if(Z_OBJ_HT_PP(array_ptr_ptr)->get_class_entry == NULL) {
				zend_error(E_WARNING, "foreach() can not iterate over objects without PHP class");
				ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
			}

			ce = Z_OBJCE_PP(array_ptr_ptr);
			if (!ce || ce->get_iterator == NULL) {
				SEPARATE_ZVAL_IF_NOT_REF(array_ptr_ptr);
				(*array_ptr_ptr)->refcount++;
			}
			array_ptr = *array_ptr_ptr;
		} else {
			if (Z_TYPE_PP(array_ptr_ptr) == IS_ARRAY) {
				SEPARATE_ZVAL_IF_NOT_REF(array_ptr_ptr);
				if (opline->extended_value & ZEND_FE_FETCH_BYREF) {
					(*array_ptr_ptr)->is_ref = 1;
				}
			}
			array_ptr = *array_ptr_ptr;
			array_ptr->refcount++;
		}
	} else {
		array_ptr = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
		if (0) { /* IS_TMP_VAR */
			zval *tmp;

			ALLOC_ZVAL(tmp);
			INIT_PZVAL_COPY(tmp, array_ptr);
			array_ptr = tmp;
		} else if (Z_TYPE_P(array_ptr) == IS_OBJECT) {
			ce = Z_OBJCE_P(array_ptr);
			if (!ce || !ce->get_iterator) {
				array_ptr->refcount++;
			}
		} else {
			if ((IS_CV == IS_CV || IS_CV == IS_VAR) &&
			    !array_ptr->is_ref &&
			    array_ptr->refcount > 1) {
				zval *tmp;

				ALLOC_ZVAL(tmp);
				INIT_PZVAL_COPY(tmp, array_ptr);
				zval_copy_ctor(tmp);
				array_ptr = tmp;
			} else {
				array_ptr->refcount++;
			}
		}
	}

	if (IS_CV != IS_TMP_VAR && ce && ce->get_iterator) {
		iter = ce->get_iterator(ce, array_ptr, opline->extended_value & ZEND_FE_RESET_REFERENCE TSRMLS_CC);

		if (iter && !EG(exception)) {
			array_ptr = zend_iterator_wrap(iter TSRMLS_CC);
		} else {
			if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {

			} else {

			}
			if (!EG(exception)) {
				zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "Object of type %s did not create an Iterator", ce->name);
			}
			zend_throw_exception_internal(NULL TSRMLS_CC);
			ZEND_VM_NEXT_OPCODE();
		}
	}

	PZVAL_LOCK(array_ptr);
	EX_T(opline->result.u.var).var.ptr = array_ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;

	if (iter) {
		iter->index = 0;
		if (iter->funcs->rewind) {
			iter->funcs->rewind(iter TSRMLS_CC);
			if (EG(exception)) {
				array_ptr->refcount--;
				zval_ptr_dtor(&array_ptr);
				if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {

				} else {

				}
				ZEND_VM_NEXT_OPCODE();
			}
		}
		is_empty = iter->funcs->valid(iter TSRMLS_CC) != SUCCESS;
		if (EG(exception)) {
			array_ptr->refcount--;
			zval_ptr_dtor(&array_ptr);
			if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {

			} else {

			}
			ZEND_VM_NEXT_OPCODE();
		}
		iter->index = -1; /* will be set to 0 before using next handler */
	} else if ((fe_ht = HASH_OF(array_ptr)) != NULL) {
		zend_hash_internal_pointer_reset(fe_ht);
		if (ce) {
			zend_object *zobj = zend_objects_get_address(array_ptr TSRMLS_CC);
			while (zend_hash_has_more_elements(fe_ht) == SUCCESS) {
				char *str_key;
				uint str_key_len;
				ulong int_key;
				zend_uchar key_type;

				key_type = zend_hash_get_current_key_ex(fe_ht, &str_key, &str_key_len, &int_key, 0, NULL);
				if (key_type != HASH_KEY_NON_EXISTANT &&
					(key_type == HASH_KEY_IS_LONG ||
				     zend_check_property_access(zobj, str_key, str_key_len-1 TSRMLS_CC) == SUCCESS)) {
					break;
				}
				zend_hash_move_forward(fe_ht);
			}
		}
		is_empty = zend_hash_has_more_elements(fe_ht) != SUCCESS;
		zend_hash_get_pointer(fe_ht, &EX_T(opline->result.u.var).fe.fe_pos);
	} else {
		zend_error(E_WARNING, "Invalid argument supplied for foreach()");
		is_empty = 1;
	}

	if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {

	} else {

	}
	if (is_empty) {
		ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
	} else {
		ZEND_VM_NEXT_OPCODE();
	}
}

static int ZEND_ISSET_ISEMPTY_VAR_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval tmp, *varname = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_IS TSRMLS_CC);
	zval **value;
	zend_bool isset = 1;
	HashTable *target_symbol_table;

	if (Z_TYPE_P(varname) != IS_STRING) {
		tmp = *varname;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		varname = &tmp;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		value = zend_std_get_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname), 1 TSRMLS_CC);
		if (!value) {
			isset = 0;
		}
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), BP_VAR_IS, varname TSRMLS_CC);
		if (zend_hash_find(target_symbol_table, varname->value.str.val, varname->value.str.len+1, (void **) &value) == FAILURE) {
			isset = 0;
		}
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			if (isset && Z_TYPE_PP(value) == IS_NULL) {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 0;
			} else {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = isset;
			}
			break;
		case ZEND_ISEMPTY:
			if (!isset || !i_zend_is_true(*value)) {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 1;
			} else {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 0;
			}
			break;
	}

	if (varname == &tmp) {
		zval_dtor(&tmp);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_EXIT_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
#if 0 || (IS_CV != IS_UNUSED)
	zend_op *opline = EX(opline);
	if (IS_CV != IS_UNUSED) {

		zval *ptr = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);

		if (Z_TYPE_P(ptr) == IS_LONG) {
			EG(exit_status) = Z_LVAL_P(ptr);
		} else {
			zend_print_variable(ptr);
		}

	}
#endif
	zend_bailout();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_QM_ASSIGN_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *value = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);

	EX_T(opline->result.u.var).tmp_var = *value;
	if (!0) {
		zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INSTANCEOF_SPEC_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *expr = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
	zend_bool result;

	if (Z_TYPE_P(expr) == IS_OBJECT && Z_OBJ_HT_P(expr)->get_class_entry) {
		result = instanceof_function(Z_OBJCE_P(expr), EX_T(opline->op2.u.var).class_entry TSRMLS_CC);
	} else {
		result = 0;
	}
	ZVAL_BOOL(&EX_T(opline->result.u.var).tmp_var, result);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	add_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SUB_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	sub_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MUL_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	mul_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DIV_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	div_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MOD_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	mod_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SL_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SR_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CONCAT_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	concat_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_IDENTICAL_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_IDENTICAL_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_EQUAL_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_EQUAL_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_OR_EQUAL_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_OR_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_AND_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_XOR_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_XOR_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		&opline->op2.u.constant TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_obj_helper_SPEC_CV_CONST(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op_data1;
	zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	zval *object;
	zval *property = &opline->op2.u.constant;
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	znode *result = &opline->result;
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");

		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
	} else {
		/* here we are sure we are dealing with an object */
		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = *zptr;
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;

			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					if (Z_OBJ_HT_P(object)->read_property) {
						z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
				case ZEND_ASSIGN_DIM:
					if (Z_OBJ_HT_P(object)->read_dimension) {
						z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (z->refcount == 0) {
						zval_dtor(z);
						FREE_ZVAL(z);
					}
					z = value;
				}
				z->refcount++;
				SEPARATE_ZVAL_IF_NOT_REF(&z);
				binary_op(z, z, value TSRMLS_CC);
				switch (opline->extended_value) {
					case ZEND_ASSIGN_OBJ:
						Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
						break;
					case ZEND_ASSIGN_DIM:
						Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
						break;
				}
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = z;
					PZVAL_LOCK(*retval);
				}
				zval_ptr_dtor(&z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = EG(uninitialized_zval_ptr);
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (0) {
			zval_ptr_dtor(&property);
		} else {

		}
		FREE_OP(free_op_data1);
	}

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_helper_SPEC_CV_CONST(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op_data2, free_op_data1;
	zval **var_ptr;
	zval *value;
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			return zend_binary_assign_op_obj_helper_SPEC_CV_CONST(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);

				if (object_ptr && IS_CV != IS_CV && !0) {
					(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
				}

				if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
					return zend_binary_assign_op_obj_helper_SPEC_CV_CONST(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
				} else {
					zend_op *op_data = opline+1;
					zval *dim = &opline->op2.u.constant;

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC), dim, 0, BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), &free_op_data2, BP_VAR_RW);
					increment_opline = 1;
				}
			}
			break;
		default:
			value = &opline->op2.u.constant;
			var_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC);
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}


		if (increment_opline) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	if (increment_opline) {
		ZEND_VM_INC_OPCODE();
		FREE_OP(free_op_data1);
		FREE_OP_VAR_PTR(free_op_data2);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_ADD_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CONST(add_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SUB_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CONST(sub_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MUL_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CONST(mul_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_DIV_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CONST(div_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MOD_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CONST(mod_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SL_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CONST(shift_left_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SR_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CONST(shift_right_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_CONCAT_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CONST(concat_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_OR_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CONST(bitwise_or_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_AND_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CONST(bitwise_and_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_XOR_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CONST(bitwise_xor_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_pre_incdec_property_helper_SPEC_CV_CONST(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	zval *object;
	zval *property = &opline->op2.u.constant;
	zval **retval = &EX_T(opline->result.u.var).var.ptr;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");

		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}

		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			have_get_ptr = 1;
			incdec_op(*zptr);
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = *zptr;
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			z->refcount++;
			SEPARATE_ZVAL_IF_NOT_REF(&z);
			incdec_op(z);
			*retval = z;
			Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = EG(uninitialized_zval_ptr);
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRE_INC_OBJ_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_CV_CONST(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_PRE_DEC_OBJ_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_CV_CONST(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_post_incdec_property_helper_SPEC_CV_CONST(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	zval *object;
	zval *property = &opline->op2.u.constant;
	zval *retval = &EX_T(opline->result.u.var).tmp_var;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");

		*retval = *EG(uninitialized_zval_ptr);

		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			have_get_ptr = 1;
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			*retval = **zptr;
			zendi_zval_copy_ctor(*retval);

			incdec_op(*zptr);

		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
			zval *z_copy;

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			*retval = *z;
			zendi_zval_copy_ctor(*retval);
			ALLOC_ZVAL(z_copy);
			*z_copy = *z;
			zendi_zval_copy_ctor(*z_copy);
			INIT_PZVAL(z_copy);
			incdec_op(z_copy);
			z->refcount++;
			Z_OBJ_HT_P(object)->write_property(object, property, z_copy TSRMLS_CC);
			zval_ptr_dtor(&z_copy);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			*retval = *EG(uninitialized_zval_ptr);
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_POST_INC_OBJ_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_CV_CONST(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_POST_DEC_OBJ_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_CV_CONST(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_DIM_R_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *dim = &opline->op2.u.constant;

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK &&
	    IS_CV != IS_CV &&
	    EX_T(opline->op1.u.var).var.ptr_ptr) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC), dim, 0, BP_VAR_R TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_W_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *dim = &opline->op2.u.constant;

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC), dim, 0, BP_VAR_W TSRMLS_CC);

	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_RW_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *dim = &opline->op2.u.constant;

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC), dim, 0, BP_VAR_RW TSRMLS_CC);

	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_IS_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *dim = &opline->op2.u.constant;

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_IS TSRMLS_CC), dim, 0, BP_VAR_IS TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_FUNC_ARG_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int type = ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)?BP_VAR_W:BP_VAR_R;
	zval *dim;

	if (IS_CONST == IS_UNUSED && type == BP_VAR_R) {
		zend_error_noreturn(E_ERROR, "Cannot use [] for reading");
	}
	dim = &opline->op2.u.constant;
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), type TSRMLS_CC), dim, 0, type TSRMLS_CC);

	if (IS_CV == IS_VAR && type == BP_VAR_W && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_UNSET_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_UNSET TSRMLS_CC);
	zval *dim = &opline->op2.u.constant;

	/* Not needed in DIM_UNSET
	if (opline->extended_value == ZEND_FETCH_ADD_LOCK) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	*/
	if (IS_CV == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, dim, 0, BP_VAR_UNSET TSRMLS_CC);

	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	if (EX_T(opline->result.u.var).var.ptr_ptr == NULL) {
		zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
	} else {
		zend_free_op free_res;

		PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
		if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
		}
		PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		FREE_OP_VAR_PTR(free_res);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int zend_fetch_property_address_read_helper_SPEC_CV_CONST(int type, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *container;
	zval **retval;


	retval = &EX_T(opline->result.u.var).var.ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = retval;

	container = _get_zval_ptr_cv(&opline->op1, EX(Ts), type TSRMLS_CC);

	if (container == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(error_zval_ptr);
			PZVAL_LOCK(*retval);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		ZEND_VM_NEXT_OPCODE();
	}


	if (Z_TYPE_P(container) != IS_OBJECT || !Z_OBJ_HT_P(container)->read_property) {
		if (type != BP_VAR_IS) {
			zend_error(E_NOTICE, "Trying to get property of non-object");
		}
		*retval = EG(uninitialized_zval_ptr);
		SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	} else {

		zval *offset  = &opline->op2.u.constant;

		if (0) {
			MAKE_REAL_ZVAL_PTR(offset);
		}

		/* here we are sure we are dealing with an object */
		*retval = Z_OBJ_HT_P(container)->read_property(container, offset, type TSRMLS_CC);

		if (RETURN_VALUE_UNUSED(&opline->result) && ((*retval)->refcount == 0)) {
			zval_dtor(*retval);
			FREE_ZVAL(*retval);
		} else {
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		if (0) {
			zval_ptr_dtor(&offset);
		} else {

		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_R_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_CV_CONST(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_W_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *property = &opline->op2.u.constant;

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK && IS_CV != IS_CV) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
		EX_T(opline->op1.u.var).var.ptr = *EX_T(opline->op1.u.var).var.ptr_ptr;
	}

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC), property, BP_VAR_W TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_RW_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *property = &opline->op2.u.constant;

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC), property, BP_VAR_RW TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_IS_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_CV_CONST(BP_VAR_IS, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_FUNC_ARG_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		/* Behave like FETCH_OBJ_W */
		zend_free_op free_op1;
		zval *property = &opline->op2.u.constant;

		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}
		zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC), property, BP_VAR_W TSRMLS_CC);
		if (0) {
			zval_ptr_dtor(&property);
		} else {

		}
		if (IS_CV == IS_VAR && 0 &&
		    READY_TO_DESTROY(free_op1.var) &&
		    !RETURN_VALUE_UNUSED(&opline->result)) {
			AI_USE_PTR(EX_T(opline->result.u.var).var);
			if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
			    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
				SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
			}
		}

		ZEND_VM_NEXT_OPCODE();
	} else {
		return zend_fetch_property_address_read_helper_SPEC_CV_CONST(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}
}

static int ZEND_FETCH_OBJ_UNSET_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_res;
	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval *property = &opline->op2.u.constant;

	if (IS_CV == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, property, BP_VAR_UNSET TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
	if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
		SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
	}
	PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	FREE_OP_VAR_PTR(free_res);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_OBJ_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;

	zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);

	zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_OBJ TSRMLS_CC);

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_DIM_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;

	zval **object_ptr;

	if (IS_CV == IS_CV || EX_T(opline->op1.u.var).var.ptr_ptr) {
		/* not an array offset */
		object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	} else {
		object_ptr = NULL;
	}

	if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
		zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_DIM TSRMLS_CC);
	} else {
		zend_free_op free_op_data1;
		zval *value;
		zval *dim = &opline->op2.u.constant;

		zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), object_ptr, dim, 0, BP_VAR_W TSRMLS_CC);

		value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	 	zend_assign_to_variable(&opline->result, &op_data->op2, &op_data->op1, value, (IS_TMP_FREE(free_op_data1)?IS_TMP_VAR:op_data->op1.op_type), EX(Ts) TSRMLS_CC);
	 	FREE_OP_IF_VAR(free_op_data1);
	}

	/* assign_dim has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *value = &opline->op2.u.constant;

 	zend_assign_to_variable(&opline->result, &opline->op1, &opline->op2, value, (0?IS_TMP_VAR:IS_CONST), EX(Ts) TSRMLS_CC);
	/* zend_assign_to_variable() always takes care of op2, never free it! */

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_METHOD_CALL_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;


	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	function_name = &opline->op2.u.constant;

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = Z_STRVAL_P(function_name);
	function_name_strlen = Z_STRLEN_P(function_name);

	EX(object) = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (EX(object) && Z_TYPE_P(EX(object)) == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(&EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CASE_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;


	if (IS_CV==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
				 &opline->op2.u.constant TSRMLS_CC);

	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */

		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=&opline->op2.u.constant;

#if 0 || IS_CV == IS_VAR || IS_CV == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=_get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
	}
#else
	expr_ptr=_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
#endif

	if (0) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_CV == IS_VAR || IS_CV == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}

	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {

	} else {

	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_CV == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_CV != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_UNSET_DIM_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_UNSET TSRMLS_CC);
	zval *offset = &opline->op2.u.constant;
	long index;

	if (container) {
		if (IS_CV == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		switch (Z_TYPE_PP(container)) {
			case IS_ARRAY: {
				HashTable *ht = Z_ARRVAL_PP(container);

				switch (Z_TYPE_P(offset)) {
					case IS_DOUBLE:
						index = (long) Z_DVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_RESOURCE:
					case IS_BOOL:
					case IS_LONG:
						index = Z_LVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_STRING:
						if (IS_CONST == IS_CV || IS_CONST == IS_VAR) {
							offset->refcount++;
						}
						if (zend_symtable_del(ht, offset->value.str.val, offset->value.str.len+1) == SUCCESS &&
					    ht == &EG(symbol_table)) {
							zend_execute_data *ex;
							ulong hash_value = zend_inline_hash_func(offset->value.str.val, offset->value.str.len+1);

							for (ex = execute_data; ex; ex = ex->prev_execute_data) {
								if (ex->op_array && ex->symbol_table == ht) {
									int i;

									for (i = 0; i < ex->op_array->last_var; i++) {
										if (ex->op_array->vars[i].hash_value == hash_value &&
										    ex->op_array->vars[i].name_len == offset->value.str.len &&
										    !memcmp(ex->op_array->vars[i].name, offset->value.str.val, offset->value.str.len)) {
											ex->CVs[i] = NULL;
											break;
										}
									}
								}
							}
						}
						if (IS_CONST == IS_CV || IS_CONST == IS_VAR) {
							zval_ptr_dtor(&offset);
						}
						break;
					case IS_NULL:
						zend_hash_del(ht, "", sizeof(""));
						break;
					default:
						zend_error(E_WARNING, "Illegal offset type in unset");
						break;
				}

				break;
			}
			case IS_OBJECT:
				if (!Z_OBJ_HT_P(*container)->unset_dimension) {
					zend_error_noreturn(E_ERROR, "Cannot use object as array");
				}
				if (0) {
					MAKE_REAL_ZVAL_PTR(offset);
				}
				Z_OBJ_HT_P(*container)->unset_dimension(*container, offset TSRMLS_CC);
				if (0) {
					zval_ptr_dtor(&offset);
				} else {

				}
				break;
			case IS_STRING:
				zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
				ZEND_VM_CONTINUE(); /* bailed out before */
			default:

				break;
		}
	} else {

	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_UNSET_OBJ_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_UNSET TSRMLS_CC);
	zval *offset = &opline->op2.u.constant;

	if (container) {
		if (IS_CV == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			Z_OBJ_HT_P(*container)->unset_property(*container, offset TSRMLS_CC);
			if (0) {
				zval_ptr_dtor(&offset);
			} else {

			}
		} else {

		}
	} else {

	}

	ZEND_VM_NEXT_OPCODE();
}

static int zend_isset_isempty_dim_prop_obj_handler_SPEC_CV_CONST(int prop_dim, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_IS TSRMLS_CC);
	zval **value = NULL;
	int result = 0;
	long index;

	if (container) {

		zval *offset = &opline->op2.u.constant;

		if (Z_TYPE_PP(container) == IS_ARRAY) {
			HashTable *ht;
			int isset = 0;

			ht = Z_ARRVAL_PP(container);

			switch (Z_TYPE_P(offset)) {
				case IS_DOUBLE:
					index = (long) Z_DVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_RESOURCE:
				case IS_BOOL:
				case IS_LONG:
					index = Z_LVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_STRING:
					if (zend_symtable_find(ht, offset->value.str.val, offset->value.str.len+1, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_NULL:
					if (zend_hash_find(ht, "", sizeof(""), (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				default:
					zend_error(E_WARNING, "Illegal offset type in isset or empty");

					break;
			}

			switch (opline->extended_value) {
				case ZEND_ISSET:
					if (isset && Z_TYPE_PP(value) == IS_NULL) {
						result = 0;
					} else {
						result = isset;
					}
					break;
				case ZEND_ISEMPTY:
					if (!isset || !i_zend_is_true(*value)) {
						result = 0;
					} else {
						result = 1;
					}
					break;
			}

		} else if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			if (prop_dim) {
				result = Z_OBJ_HT_P(*container)->has_property(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			} else {
				result = Z_OBJ_HT_P(*container)->has_dimension(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			}
			if (0) {
				zval_ptr_dtor(&offset);
			} else {

			}
		} else if ((*container)->type == IS_STRING && !prop_dim) { /* string offsets */
			zval tmp;

			if (Z_TYPE_P(offset) != IS_LONG) {
				tmp = *offset;
				zval_copy_ctor(&tmp);
				convert_to_long(&tmp);
				offset = &tmp;
			}
			if (Z_TYPE_P(offset) == IS_LONG) {
				switch (opline->extended_value) {
					case ZEND_ISSET:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container)) {
							result = 1;
						}
						break;
					case ZEND_ISEMPTY:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container) && Z_STRVAL_PP(container)[offset->value.lval] != '0') {
							result = 1;
						}
						break;
				}
			}

		} else {

		}
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = result;
			break;
		case ZEND_ISEMPTY:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = !result;
			break;
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_CV_CONST(0, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_CV_CONST_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_CV_CONST(1, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ADD_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	add_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SUB_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	sub_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MUL_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	mul_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DIV_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	div_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MOD_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	mod_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SL_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SR_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CONCAT_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	concat_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_IDENTICAL_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_IDENTICAL_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_EQUAL_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_EQUAL_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_OR_EQUAL_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_OR_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_AND_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_XOR_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_XOR_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_obj_helper_SPEC_CV_TMP(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op2, free_op_data1;
	zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	znode *result = &opline->result;
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");
		zval_dtor(free_op2.var);
		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
	} else {
		/* here we are sure we are dealing with an object */
		if (1) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = *zptr;
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;

			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					if (Z_OBJ_HT_P(object)->read_property) {
						z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
				case ZEND_ASSIGN_DIM:
					if (Z_OBJ_HT_P(object)->read_dimension) {
						z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (z->refcount == 0) {
						zval_dtor(z);
						FREE_ZVAL(z);
					}
					z = value;
				}
				z->refcount++;
				SEPARATE_ZVAL_IF_NOT_REF(&z);
				binary_op(z, z, value TSRMLS_CC);
				switch (opline->extended_value) {
					case ZEND_ASSIGN_OBJ:
						Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
						break;
					case ZEND_ASSIGN_DIM:
						Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
						break;
				}
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = z;
					PZVAL_LOCK(*retval);
				}
				zval_ptr_dtor(&z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = EG(uninitialized_zval_ptr);
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (1) {
			zval_ptr_dtor(&property);
		} else {
			zval_dtor(free_op2.var);
		}
		FREE_OP(free_op_data1);
	}

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_helper_SPEC_CV_TMP(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2, free_op_data2, free_op_data1;
	zval **var_ptr;
	zval *value;
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			return zend_binary_assign_op_obj_helper_SPEC_CV_TMP(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);

				if (object_ptr && IS_CV != IS_CV && !0) {
					(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
				}

				if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
					return zend_binary_assign_op_obj_helper_SPEC_CV_TMP(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
				} else {
					zend_op *op_data = opline+1;
					zval *dim = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC), dim, 1, BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), &free_op_data2, BP_VAR_RW);
					increment_opline = 1;
				}
			}
			break;
		default:
			value = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
			var_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC);
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		zval_dtor(free_op2.var);

		if (increment_opline) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}
	zval_dtor(free_op2.var);

	if (increment_opline) {
		ZEND_VM_INC_OPCODE();
		FREE_OP(free_op_data1);
		FREE_OP_VAR_PTR(free_op_data2);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_ADD_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_TMP(add_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SUB_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_TMP(sub_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MUL_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_TMP(mul_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_DIV_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_TMP(div_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MOD_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_TMP(mod_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SL_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_TMP(shift_left_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SR_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_TMP(shift_right_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_CONCAT_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_TMP(concat_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_OR_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_TMP(bitwise_or_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_AND_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_TMP(bitwise_and_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_XOR_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_TMP(bitwise_xor_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_pre_incdec_property_helper_SPEC_CV_TMP(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval **retval = &EX_T(opline->result.u.var).var.ptr;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		zval_dtor(free_op2.var);
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}

		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (1) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			have_get_ptr = 1;
			incdec_op(*zptr);
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = *zptr;
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			z->refcount++;
			SEPARATE_ZVAL_IF_NOT_REF(&z);
			incdec_op(z);
			*retval = z;
			Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = EG(uninitialized_zval_ptr);
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (1) {
		zval_ptr_dtor(&property);
	} else {
		zval_dtor(free_op2.var);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRE_INC_OBJ_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_CV_TMP(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_PRE_DEC_OBJ_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_CV_TMP(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_post_incdec_property_helper_SPEC_CV_TMP(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval *retval = &EX_T(opline->result.u.var).tmp_var;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		zval_dtor(free_op2.var);
		*retval = *EG(uninitialized_zval_ptr);

		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (1) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			have_get_ptr = 1;
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			*retval = **zptr;
			zendi_zval_copy_ctor(*retval);

			incdec_op(*zptr);

		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
			zval *z_copy;

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			*retval = *z;
			zendi_zval_copy_ctor(*retval);
			ALLOC_ZVAL(z_copy);
			*z_copy = *z;
			zendi_zval_copy_ctor(*z_copy);
			INIT_PZVAL(z_copy);
			incdec_op(z_copy);
			z->refcount++;
			Z_OBJ_HT_P(object)->write_property(object, property, z_copy TSRMLS_CC);
			zval_ptr_dtor(&z_copy);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			*retval = *EG(uninitialized_zval_ptr);
		}
	}

	if (1) {
		zval_ptr_dtor(&property);
	} else {
		zval_dtor(free_op2.var);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_POST_INC_OBJ_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_CV_TMP(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_POST_DEC_OBJ_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_CV_TMP(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_DIM_R_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval *dim = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK &&
	    IS_CV != IS_CV &&
	    EX_T(opline->op1.u.var).var.ptr_ptr) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC), dim, 1, BP_VAR_R TSRMLS_CC);
	zval_dtor(free_op2.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_W_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC), dim, 1, BP_VAR_W TSRMLS_CC);
	zval_dtor(free_op2.var);
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_RW_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC), dim, 1, BP_VAR_RW TSRMLS_CC);
	zval_dtor(free_op2.var);
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_IS_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval *dim = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_IS TSRMLS_CC), dim, 1, BP_VAR_IS TSRMLS_CC);
	zval_dtor(free_op2.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_FUNC_ARG_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	int type = ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)?BP_VAR_W:BP_VAR_R;
	zval *dim;

	if (IS_TMP_VAR == IS_UNUSED && type == BP_VAR_R) {
		zend_error_noreturn(E_ERROR, "Cannot use [] for reading");
	}
	dim = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), type TSRMLS_CC), dim, 1, type TSRMLS_CC);
	zval_dtor(free_op2.var);
	if (IS_CV == IS_VAR && type == BP_VAR_W && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_UNSET_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_UNSET TSRMLS_CC);
	zval *dim = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	/* Not needed in DIM_UNSET
	if (opline->extended_value == ZEND_FETCH_ADD_LOCK) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	*/
	if (IS_CV == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, dim, 1, BP_VAR_UNSET TSRMLS_CC);
	zval_dtor(free_op2.var);
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	if (EX_T(opline->result.u.var).var.ptr_ptr == NULL) {
		zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
	} else {
		zend_free_op free_res;

		PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
		if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
		}
		PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		FREE_OP_VAR_PTR(free_res);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int zend_fetch_property_address_read_helper_SPEC_CV_TMP(int type, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *container;
	zval **retval;


	retval = &EX_T(opline->result.u.var).var.ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = retval;

	container = _get_zval_ptr_cv(&opline->op1, EX(Ts), type TSRMLS_CC);

	if (container == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(error_zval_ptr);
			PZVAL_LOCK(*retval);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		ZEND_VM_NEXT_OPCODE();
	}


	if (Z_TYPE_P(container) != IS_OBJECT || !Z_OBJ_HT_P(container)->read_property) {
		if (type != BP_VAR_IS) {
			zend_error(E_NOTICE, "Trying to get property of non-object");
		}
		*retval = EG(uninitialized_zval_ptr);
		SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	} else {
		zend_free_op free_op2;
		zval *offset  = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (1) {
			MAKE_REAL_ZVAL_PTR(offset);
		}

		/* here we are sure we are dealing with an object */
		*retval = Z_OBJ_HT_P(container)->read_property(container, offset, type TSRMLS_CC);

		if (RETURN_VALUE_UNUSED(&opline->result) && ((*retval)->refcount == 0)) {
			zval_dtor(*retval);
			FREE_ZVAL(*retval);
		} else {
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		if (1) {
			zval_ptr_dtor(&offset);
		} else {
			zval_dtor(free_op2.var);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_R_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_CV_TMP(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_W_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK && IS_CV != IS_CV) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
		EX_T(opline->op1.u.var).var.ptr = *EX_T(opline->op1.u.var).var.ptr_ptr;
	}

	if (1) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC), property, BP_VAR_W TSRMLS_CC);
	if (1) {
		zval_ptr_dtor(&property);
	} else {
		zval_dtor(free_op2.var);
	}
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_RW_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (1) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC), property, BP_VAR_RW TSRMLS_CC);
	if (1) {
		zval_ptr_dtor(&property);
	} else {
		zval_dtor(free_op2.var);
	}
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_IS_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_CV_TMP(BP_VAR_IS, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_FUNC_ARG_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		/* Behave like FETCH_OBJ_W */
		zend_free_op free_op1, free_op2;
		zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (1) {
			MAKE_REAL_ZVAL_PTR(property);
		}
		zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC), property, BP_VAR_W TSRMLS_CC);
		if (1) {
			zval_ptr_dtor(&property);
		} else {
			zval_dtor(free_op2.var);
		}
		if (IS_CV == IS_VAR && 0 &&
		    READY_TO_DESTROY(free_op1.var) &&
		    !RETURN_VALUE_UNUSED(&opline->result)) {
			AI_USE_PTR(EX_T(opline->result.u.var).var);
			if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
			    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
				SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
			}
		}

		ZEND_VM_NEXT_OPCODE();
	} else {
		return zend_fetch_property_address_read_helper_SPEC_CV_TMP(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}
}

static int ZEND_FETCH_OBJ_UNSET_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2, free_res;
	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval *property = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (IS_CV == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	if (1) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, property, BP_VAR_UNSET TSRMLS_CC);
	if (1) {
		zval_ptr_dtor(&property);
	} else {
		zval_dtor(free_op2.var);
	}
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
	if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
		SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
	}
	PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	FREE_OP_VAR_PTR(free_res);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_OBJ_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;

	zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);

	zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_OBJ TSRMLS_CC);

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_DIM_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;

	zval **object_ptr;

	if (IS_CV == IS_CV || EX_T(opline->op1.u.var).var.ptr_ptr) {
		/* not an array offset */
		object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	} else {
		object_ptr = NULL;
	}

	if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
		zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_DIM TSRMLS_CC);
	} else {
		zend_free_op free_op2, free_op_data1;
		zval *value;
		zval *dim = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), object_ptr, dim, 1, BP_VAR_W TSRMLS_CC);
		zval_dtor(free_op2.var);

		value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	 	zend_assign_to_variable(&opline->result, &op_data->op2, &op_data->op1, value, (IS_TMP_FREE(free_op_data1)?IS_TMP_VAR:op_data->op1.op_type), EX(Ts) TSRMLS_CC);
	 	FREE_OP_IF_VAR(free_op_data1);
	}

	/* assign_dim has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval *value = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

 	zend_assign_to_variable(&opline->result, &opline->op1, &opline->op2, value, (1?IS_TMP_VAR:IS_TMP_VAR), EX(Ts) TSRMLS_CC);
	/* zend_assign_to_variable() always takes care of op2, never free it! */

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_METHOD_CALL_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;
	zend_free_op free_op2;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	function_name = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = Z_STRVAL_P(function_name);
	function_name_strlen = Z_STRLEN_P(function_name);

	EX(object) = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (EX(object) && Z_TYPE_P(EX(object)) == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(&EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}

	zval_dtor(free_op2.var);

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CASE_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;
	zend_free_op free_op2;

	if (IS_CV==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
				 _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	zval_dtor(free_op2.var);
	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */

		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=_get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

#if 0 || IS_CV == IS_VAR || IS_CV == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=_get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
	}
#else
	expr_ptr=_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
#endif

	if (0) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_CV == IS_VAR || IS_CV == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}
		zval_dtor(free_op2.var);
	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {

	} else {

	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_CV == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_CV != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_UNSET_DIM_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_UNSET TSRMLS_CC);
	zval *offset = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	long index;

	if (container) {
		if (IS_CV == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		switch (Z_TYPE_PP(container)) {
			case IS_ARRAY: {
				HashTable *ht = Z_ARRVAL_PP(container);

				switch (Z_TYPE_P(offset)) {
					case IS_DOUBLE:
						index = (long) Z_DVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_RESOURCE:
					case IS_BOOL:
					case IS_LONG:
						index = Z_LVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_STRING:
						if (IS_TMP_VAR == IS_CV || IS_TMP_VAR == IS_VAR) {
							offset->refcount++;
						}
						if (zend_symtable_del(ht, offset->value.str.val, offset->value.str.len+1) == SUCCESS &&
					    ht == &EG(symbol_table)) {
							zend_execute_data *ex;
							ulong hash_value = zend_inline_hash_func(offset->value.str.val, offset->value.str.len+1);

							for (ex = execute_data; ex; ex = ex->prev_execute_data) {
								if (ex->op_array && ex->symbol_table == ht) {
									int i;

									for (i = 0; i < ex->op_array->last_var; i++) {
										if (ex->op_array->vars[i].hash_value == hash_value &&
										    ex->op_array->vars[i].name_len == offset->value.str.len &&
										    !memcmp(ex->op_array->vars[i].name, offset->value.str.val, offset->value.str.len)) {
											ex->CVs[i] = NULL;
											break;
										}
									}
								}
							}
						}
						if (IS_TMP_VAR == IS_CV || IS_TMP_VAR == IS_VAR) {
							zval_ptr_dtor(&offset);
						}
						break;
					case IS_NULL:
						zend_hash_del(ht, "", sizeof(""));
						break;
					default:
						zend_error(E_WARNING, "Illegal offset type in unset");
						break;
				}
				zval_dtor(free_op2.var);
				break;
			}
			case IS_OBJECT:
				if (!Z_OBJ_HT_P(*container)->unset_dimension) {
					zend_error_noreturn(E_ERROR, "Cannot use object as array");
				}
				if (1) {
					MAKE_REAL_ZVAL_PTR(offset);
				}
				Z_OBJ_HT_P(*container)->unset_dimension(*container, offset TSRMLS_CC);
				if (1) {
					zval_ptr_dtor(&offset);
				} else {
					zval_dtor(free_op2.var);
				}
				break;
			case IS_STRING:
				zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
				ZEND_VM_CONTINUE(); /* bailed out before */
			default:
				zval_dtor(free_op2.var);
				break;
		}
	} else {
		zval_dtor(free_op2.var);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_UNSET_OBJ_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_UNSET TSRMLS_CC);
	zval *offset = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (container) {
		if (IS_CV == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (1) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			Z_OBJ_HT_P(*container)->unset_property(*container, offset TSRMLS_CC);
			if (1) {
				zval_ptr_dtor(&offset);
			} else {
				zval_dtor(free_op2.var);
			}
		} else {
			zval_dtor(free_op2.var);
		}
	} else {
		zval_dtor(free_op2.var);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int zend_isset_isempty_dim_prop_obj_handler_SPEC_CV_TMP(int prop_dim, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_IS TSRMLS_CC);
	zval **value = NULL;
	int result = 0;
	long index;

	if (container) {
		zend_free_op free_op2;
		zval *offset = _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (Z_TYPE_PP(container) == IS_ARRAY) {
			HashTable *ht;
			int isset = 0;

			ht = Z_ARRVAL_PP(container);

			switch (Z_TYPE_P(offset)) {
				case IS_DOUBLE:
					index = (long) Z_DVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_RESOURCE:
				case IS_BOOL:
				case IS_LONG:
					index = Z_LVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_STRING:
					if (zend_symtable_find(ht, offset->value.str.val, offset->value.str.len+1, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_NULL:
					if (zend_hash_find(ht, "", sizeof(""), (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				default:
					zend_error(E_WARNING, "Illegal offset type in isset or empty");

					break;
			}

			switch (opline->extended_value) {
				case ZEND_ISSET:
					if (isset && Z_TYPE_PP(value) == IS_NULL) {
						result = 0;
					} else {
						result = isset;
					}
					break;
				case ZEND_ISEMPTY:
					if (!isset || !i_zend_is_true(*value)) {
						result = 0;
					} else {
						result = 1;
					}
					break;
			}
			zval_dtor(free_op2.var);
		} else if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (1) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			if (prop_dim) {
				result = Z_OBJ_HT_P(*container)->has_property(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			} else {
				result = Z_OBJ_HT_P(*container)->has_dimension(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			}
			if (1) {
				zval_ptr_dtor(&offset);
			} else {
				zval_dtor(free_op2.var);
			}
		} else if ((*container)->type == IS_STRING && !prop_dim) { /* string offsets */
			zval tmp;

			if (Z_TYPE_P(offset) != IS_LONG) {
				tmp = *offset;
				zval_copy_ctor(&tmp);
				convert_to_long(&tmp);
				offset = &tmp;
			}
			if (Z_TYPE_P(offset) == IS_LONG) {
				switch (opline->extended_value) {
					case ZEND_ISSET:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container)) {
							result = 1;
						}
						break;
					case ZEND_ISEMPTY:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container) && Z_STRVAL_PP(container)[offset->value.lval] != '0') {
							result = 1;
						}
						break;
				}
			}
			zval_dtor(free_op2.var);
		} else {
			zval_dtor(free_op2.var);
		}
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = result;
			break;
		case ZEND_ISEMPTY:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = !result;
			break;
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_CV_TMP(0, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_CV_TMP_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_CV_TMP(1, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ADD_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	add_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SUB_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	sub_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MUL_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	mul_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DIV_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	div_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MOD_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	mod_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SL_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SR_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CONCAT_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	concat_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_IDENTICAL_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_IDENTICAL_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_EQUAL_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_EQUAL_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_OR_EQUAL_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_OR_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_AND_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_XOR_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_XOR_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_obj_helper_SPEC_CV_VAR(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op2, free_op_data1;
	zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	znode *result = &opline->result;
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
	} else {
		/* here we are sure we are dealing with an object */
		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = *zptr;
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;

			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					if (Z_OBJ_HT_P(object)->read_property) {
						z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
				case ZEND_ASSIGN_DIM:
					if (Z_OBJ_HT_P(object)->read_dimension) {
						z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (z->refcount == 0) {
						zval_dtor(z);
						FREE_ZVAL(z);
					}
					z = value;
				}
				z->refcount++;
				SEPARATE_ZVAL_IF_NOT_REF(&z);
				binary_op(z, z, value TSRMLS_CC);
				switch (opline->extended_value) {
					case ZEND_ASSIGN_OBJ:
						Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
						break;
					case ZEND_ASSIGN_DIM:
						Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
						break;
				}
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = z;
					PZVAL_LOCK(*retval);
				}
				zval_ptr_dtor(&z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = EG(uninitialized_zval_ptr);
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (0) {
			zval_ptr_dtor(&property);
		} else {
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		}
		FREE_OP(free_op_data1);
	}

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_helper_SPEC_CV_VAR(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2, free_op_data2, free_op_data1;
	zval **var_ptr;
	zval *value;
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			return zend_binary_assign_op_obj_helper_SPEC_CV_VAR(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);

				if (object_ptr && IS_CV != IS_CV && !0) {
					(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
				}

				if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
					return zend_binary_assign_op_obj_helper_SPEC_CV_VAR(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
				} else {
					zend_op *op_data = opline+1;
					zval *dim = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC), dim, 0, BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), &free_op_data2, BP_VAR_RW);
					increment_opline = 1;
				}
			}
			break;
		default:
			value = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
			var_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC);
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};

		if (increment_opline) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};

	if (increment_opline) {
		ZEND_VM_INC_OPCODE();
		FREE_OP(free_op_data1);
		FREE_OP_VAR_PTR(free_op_data2);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_ADD_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_VAR(add_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SUB_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_VAR(sub_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MUL_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_VAR(mul_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_DIV_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_VAR(div_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MOD_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_VAR(mod_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SL_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_VAR(shift_left_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SR_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_VAR(shift_right_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_CONCAT_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_VAR(concat_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_OR_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_VAR(bitwise_or_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_AND_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_VAR(bitwise_and_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_XOR_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_VAR(bitwise_xor_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_pre_incdec_property_helper_SPEC_CV_VAR(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval **retval = &EX_T(opline->result.u.var).var.ptr;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}

		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			have_get_ptr = 1;
			incdec_op(*zptr);
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = *zptr;
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			z->refcount++;
			SEPARATE_ZVAL_IF_NOT_REF(&z);
			incdec_op(z);
			*retval = z;
			Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = EG(uninitialized_zval_ptr);
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRE_INC_OBJ_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_CV_VAR(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_PRE_DEC_OBJ_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_CV_VAR(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_post_incdec_property_helper_SPEC_CV_VAR(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zval *retval = &EX_T(opline->result.u.var).tmp_var;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		*retval = *EG(uninitialized_zval_ptr);

		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			have_get_ptr = 1;
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			*retval = **zptr;
			zendi_zval_copy_ctor(*retval);

			incdec_op(*zptr);

		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
			zval *z_copy;

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			*retval = *z;
			zendi_zval_copy_ctor(*retval);
			ALLOC_ZVAL(z_copy);
			*z_copy = *z;
			zendi_zval_copy_ctor(*z_copy);
			INIT_PZVAL(z_copy);
			incdec_op(z_copy);
			z->refcount++;
			Z_OBJ_HT_P(object)->write_property(object, property, z_copy TSRMLS_CC);
			zval_ptr_dtor(&z_copy);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			*retval = *EG(uninitialized_zval_ptr);
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_POST_INC_OBJ_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_CV_VAR(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_POST_DEC_OBJ_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_CV_VAR(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_DIM_R_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval *dim = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK &&
	    IS_CV != IS_CV &&
	    EX_T(opline->op1.u.var).var.ptr_ptr) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC), dim, 0, BP_VAR_R TSRMLS_CC);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_W_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC), dim, 0, BP_VAR_W TSRMLS_CC);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_RW_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC), dim, 0, BP_VAR_RW TSRMLS_CC);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_IS_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval *dim = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_IS TSRMLS_CC), dim, 0, BP_VAR_IS TSRMLS_CC);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_FUNC_ARG_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	int type = ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)?BP_VAR_W:BP_VAR_R;
	zval *dim;

	if (IS_VAR == IS_UNUSED && type == BP_VAR_R) {
		zend_error_noreturn(E_ERROR, "Cannot use [] for reading");
	}
	dim = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), type TSRMLS_CC), dim, 0, type TSRMLS_CC);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	if (IS_CV == IS_VAR && type == BP_VAR_W && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_UNSET_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_UNSET TSRMLS_CC);
	zval *dim = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	/* Not needed in DIM_UNSET
	if (opline->extended_value == ZEND_FETCH_ADD_LOCK) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	*/
	if (IS_CV == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, dim, 0, BP_VAR_UNSET TSRMLS_CC);
	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	if (EX_T(opline->result.u.var).var.ptr_ptr == NULL) {
		zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
	} else {
		zend_free_op free_res;

		PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
		if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
		}
		PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		FREE_OP_VAR_PTR(free_res);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int zend_fetch_property_address_read_helper_SPEC_CV_VAR(int type, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *container;
	zval **retval;


	retval = &EX_T(opline->result.u.var).var.ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = retval;

	container = _get_zval_ptr_cv(&opline->op1, EX(Ts), type TSRMLS_CC);

	if (container == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(error_zval_ptr);
			PZVAL_LOCK(*retval);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		ZEND_VM_NEXT_OPCODE();
	}


	if (Z_TYPE_P(container) != IS_OBJECT || !Z_OBJ_HT_P(container)->read_property) {
		if (type != BP_VAR_IS) {
			zend_error(E_NOTICE, "Trying to get property of non-object");
		}
		*retval = EG(uninitialized_zval_ptr);
		SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	} else {
		zend_free_op free_op2;
		zval *offset  = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (0) {
			MAKE_REAL_ZVAL_PTR(offset);
		}

		/* here we are sure we are dealing with an object */
		*retval = Z_OBJ_HT_P(container)->read_property(container, offset, type TSRMLS_CC);

		if (RETURN_VALUE_UNUSED(&opline->result) && ((*retval)->refcount == 0)) {
			zval_dtor(*retval);
			FREE_ZVAL(*retval);
		} else {
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		if (0) {
			zval_ptr_dtor(&offset);
		} else {
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_R_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_CV_VAR(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_W_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK && IS_CV != IS_CV) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
		EX_T(opline->op1.u.var).var.ptr = *EX_T(opline->op1.u.var).var.ptr_ptr;
	}

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC), property, BP_VAR_W TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_RW_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC), property, BP_VAR_RW TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_IS_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_CV_VAR(BP_VAR_IS, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_FUNC_ARG_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		/* Behave like FETCH_OBJ_W */
		zend_free_op free_op1, free_op2;
		zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}
		zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC), property, BP_VAR_W TSRMLS_CC);
		if (0) {
			zval_ptr_dtor(&property);
		} else {
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		}
		if (IS_CV == IS_VAR && 0 &&
		    READY_TO_DESTROY(free_op1.var) &&
		    !RETURN_VALUE_UNUSED(&opline->result)) {
			AI_USE_PTR(EX_T(opline->result.u.var).var);
			if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
			    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
				SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
			}
		}

		ZEND_VM_NEXT_OPCODE();
	} else {
		return zend_fetch_property_address_read_helper_SPEC_CV_VAR(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}
}

static int ZEND_FETCH_OBJ_UNSET_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2, free_res;
	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval *property = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (IS_CV == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, property, BP_VAR_UNSET TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
	if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
		SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
	}
	PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	FREE_OP_VAR_PTR(free_res);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_OBJ_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;

	zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);

	zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_OBJ TSRMLS_CC);

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_DIM_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;

	zval **object_ptr;

	if (IS_CV == IS_CV || EX_T(opline->op1.u.var).var.ptr_ptr) {
		/* not an array offset */
		object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	} else {
		object_ptr = NULL;
	}

	if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
		zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_DIM TSRMLS_CC);
	} else {
		zend_free_op free_op2, free_op_data1;
		zval *value;
		zval *dim = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), object_ptr, dim, 0, BP_VAR_W TSRMLS_CC);
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};

		value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	 	zend_assign_to_variable(&opline->result, &op_data->op2, &op_data->op1, value, (IS_TMP_FREE(free_op_data1)?IS_TMP_VAR:op_data->op1.op_type), EX(Ts) TSRMLS_CC);
	 	FREE_OP_IF_VAR(free_op_data1);
	}

	/* assign_dim has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval *value = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

 	zend_assign_to_variable(&opline->result, &opline->op1, &opline->op2, value, (0?IS_TMP_VAR:IS_VAR), EX(Ts) TSRMLS_CC);
	/* zend_assign_to_variable() always takes care of op2, never free it! */
 	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_REF_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **variable_ptr_ptr;
	zval **value_ptr_ptr = _get_zval_ptr_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (IS_VAR == IS_VAR &&
	    value_ptr_ptr &&
	    !(*value_ptr_ptr)->is_ref &&
	    opline->extended_value == ZEND_RETURNS_FUNCTION &&
	    !EX_T(opline->op2.u.var).var.fcall_returned_reference) {
		if (free_op2.var == NULL) {
			PZVAL_LOCK(*value_ptr_ptr); /* undo the effect of get_zval_ptr_ptr() */
		}
		zend_error(E_STRICT, "Only variables should be assigned by reference");
		if (EG(exception)) {
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
			ZEND_VM_NEXT_OPCODE();
		}
		return ZEND_ASSIGN_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	} else if (IS_VAR == IS_VAR && opline->extended_value == ZEND_RETURNS_NEW) {
		PZVAL_LOCK(*value_ptr_ptr);
	}
	if (IS_CV == IS_VAR && EX_T(opline->op1.u.var).var.ptr_ptr == &EX_T(opline->op1.u.var).var.ptr) {
		zend_error(E_ERROR, "Cannot assign by reference to overloaded object");
	}

	variable_ptr_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	zend_assign_to_variable_reference(variable_ptr_ptr, value_ptr_ptr TSRMLS_CC);

	if (IS_VAR == IS_VAR && opline->extended_value == ZEND_RETURNS_NEW) {
		(*variable_ptr_ptr)->refcount--;
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = variable_ptr_ptr;
		PZVAL_LOCK(*variable_ptr_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_METHOD_CALL_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;
	zend_free_op free_op2;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	function_name = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = Z_STRVAL_P(function_name);
	function_name_strlen = Z_STRLEN_P(function_name);

	EX(object) = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (EX(object) && Z_TYPE_P(EX(object)) == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(&EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CASE_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;
	zend_free_op free_op2;

	if (IS_CV==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
				 _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC) TSRMLS_CC);

	if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */

		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=_get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

#if 0 || IS_CV == IS_VAR || IS_CV == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=_get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
	}
#else
	expr_ptr=_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
#endif

	if (0) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_CV == IS_VAR || IS_CV == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {

	} else {

	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_CV == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_CV != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_UNSET_DIM_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_UNSET TSRMLS_CC);
	zval *offset = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);
	long index;

	if (container) {
		if (IS_CV == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		switch (Z_TYPE_PP(container)) {
			case IS_ARRAY: {
				HashTable *ht = Z_ARRVAL_PP(container);

				switch (Z_TYPE_P(offset)) {
					case IS_DOUBLE:
						index = (long) Z_DVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_RESOURCE:
					case IS_BOOL:
					case IS_LONG:
						index = Z_LVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_STRING:
						if (IS_VAR == IS_CV || IS_VAR == IS_VAR) {
							offset->refcount++;
						}
						if (zend_symtable_del(ht, offset->value.str.val, offset->value.str.len+1) == SUCCESS &&
					    ht == &EG(symbol_table)) {
							zend_execute_data *ex;
							ulong hash_value = zend_inline_hash_func(offset->value.str.val, offset->value.str.len+1);

							for (ex = execute_data; ex; ex = ex->prev_execute_data) {
								if (ex->op_array && ex->symbol_table == ht) {
									int i;

									for (i = 0; i < ex->op_array->last_var; i++) {
										if (ex->op_array->vars[i].hash_value == hash_value &&
										    ex->op_array->vars[i].name_len == offset->value.str.len &&
										    !memcmp(ex->op_array->vars[i].name, offset->value.str.val, offset->value.str.len)) {
											ex->CVs[i] = NULL;
											break;
										}
									}
								}
							}
						}
						if (IS_VAR == IS_CV || IS_VAR == IS_VAR) {
							zval_ptr_dtor(&offset);
						}
						break;
					case IS_NULL:
						zend_hash_del(ht, "", sizeof(""));
						break;
					default:
						zend_error(E_WARNING, "Illegal offset type in unset");
						break;
				}
				if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
				break;
			}
			case IS_OBJECT:
				if (!Z_OBJ_HT_P(*container)->unset_dimension) {
					zend_error_noreturn(E_ERROR, "Cannot use object as array");
				}
				if (0) {
					MAKE_REAL_ZVAL_PTR(offset);
				}
				Z_OBJ_HT_P(*container)->unset_dimension(*container, offset TSRMLS_CC);
				if (0) {
					zval_ptr_dtor(&offset);
				} else {
					if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
				}
				break;
			case IS_STRING:
				zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
				ZEND_VM_CONTINUE(); /* bailed out before */
			default:
				if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
				break;
		}
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_UNSET_OBJ_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_UNSET TSRMLS_CC);
	zval *offset = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

	if (container) {
		if (IS_CV == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			Z_OBJ_HT_P(*container)->unset_property(*container, offset TSRMLS_CC);
			if (0) {
				zval_ptr_dtor(&offset);
			} else {
				if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
			}
		} else {
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		}
	} else {
		if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
	}

	ZEND_VM_NEXT_OPCODE();
}

static int zend_isset_isempty_dim_prop_obj_handler_SPEC_CV_VAR(int prop_dim, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_IS TSRMLS_CC);
	zval **value = NULL;
	int result = 0;
	long index;

	if (container) {
		zend_free_op free_op2;
		zval *offset = _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC);

		if (Z_TYPE_PP(container) == IS_ARRAY) {
			HashTable *ht;
			int isset = 0;

			ht = Z_ARRVAL_PP(container);

			switch (Z_TYPE_P(offset)) {
				case IS_DOUBLE:
					index = (long) Z_DVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_RESOURCE:
				case IS_BOOL:
				case IS_LONG:
					index = Z_LVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_STRING:
					if (zend_symtable_find(ht, offset->value.str.val, offset->value.str.len+1, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_NULL:
					if (zend_hash_find(ht, "", sizeof(""), (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				default:
					zend_error(E_WARNING, "Illegal offset type in isset or empty");

					break;
			}

			switch (opline->extended_value) {
				case ZEND_ISSET:
					if (isset && Z_TYPE_PP(value) == IS_NULL) {
						result = 0;
					} else {
						result = isset;
					}
					break;
				case ZEND_ISEMPTY:
					if (!isset || !i_zend_is_true(*value)) {
						result = 0;
					} else {
						result = 1;
					}
					break;
			}
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		} else if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			if (prop_dim) {
				result = Z_OBJ_HT_P(*container)->has_property(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			} else {
				result = Z_OBJ_HT_P(*container)->has_dimension(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			}
			if (0) {
				zval_ptr_dtor(&offset);
			} else {
				if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
			}
		} else if ((*container)->type == IS_STRING && !prop_dim) { /* string offsets */
			zval tmp;

			if (Z_TYPE_P(offset) != IS_LONG) {
				tmp = *offset;
				zval_copy_ctor(&tmp);
				convert_to_long(&tmp);
				offset = &tmp;
			}
			if (Z_TYPE_P(offset) == IS_LONG) {
				switch (opline->extended_value) {
					case ZEND_ISSET:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container)) {
							result = 1;
						}
						break;
					case ZEND_ISEMPTY:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container) && Z_STRVAL_PP(container)[offset->value.lval] != '0') {
							result = 1;
						}
						break;
				}
			}
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		} else {
			if (free_op2.var) {zval_ptr_dtor(&free_op2.var);};
		}
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = result;
			break;
		case ZEND_ISEMPTY:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = !result;
			break;
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_CV_VAR(0, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_CV_VAR_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_CV_VAR(1, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_binary_assign_op_obj_helper_SPEC_CV_UNUSED(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op_data1;
	zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	zval *object;
	zval *property = NULL;
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	znode *result = &opline->result;
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");

		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
	} else {
		/* here we are sure we are dealing with an object */
		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = *zptr;
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;

			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					if (Z_OBJ_HT_P(object)->read_property) {
						z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
				case ZEND_ASSIGN_DIM:
					if (Z_OBJ_HT_P(object)->read_dimension) {
						z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (z->refcount == 0) {
						zval_dtor(z);
						FREE_ZVAL(z);
					}
					z = value;
				}
				z->refcount++;
				SEPARATE_ZVAL_IF_NOT_REF(&z);
				binary_op(z, z, value TSRMLS_CC);
				switch (opline->extended_value) {
					case ZEND_ASSIGN_OBJ:
						Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
						break;
					case ZEND_ASSIGN_DIM:
						Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
						break;
				}
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = z;
					PZVAL_LOCK(*retval);
				}
				zval_ptr_dtor(&z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = EG(uninitialized_zval_ptr);
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (0) {
			zval_ptr_dtor(&property);
		} else {

		}
		FREE_OP(free_op_data1);
	}

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_helper_SPEC_CV_UNUSED(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op_data2, free_op_data1;
	zval **var_ptr;
	zval *value;
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			return zend_binary_assign_op_obj_helper_SPEC_CV_UNUSED(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);

				if (object_ptr && IS_CV != IS_CV && !0) {
					(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
				}

				if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
					return zend_binary_assign_op_obj_helper_SPEC_CV_UNUSED(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
				} else {
					zend_op *op_data = opline+1;
					zval *dim = NULL;

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC), dim, 0, BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), &free_op_data2, BP_VAR_RW);
					increment_opline = 1;
				}
			}
			break;
		default:
			value = NULL;
			var_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC);
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}


		if (increment_opline) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	if (increment_opline) {
		ZEND_VM_INC_OPCODE();
		FREE_OP(free_op_data1);
		FREE_OP_VAR_PTR(free_op_data2);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_ADD_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_UNUSED(add_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SUB_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_UNUSED(sub_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MUL_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_UNUSED(mul_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_DIV_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_UNUSED(div_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MOD_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_UNUSED(mod_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SL_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_UNUSED(shift_left_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SR_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_UNUSED(shift_right_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_CONCAT_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_UNUSED(concat_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_OR_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_UNUSED(bitwise_or_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_AND_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_UNUSED(bitwise_and_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_XOR_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_UNUSED(bitwise_xor_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_DIM_W_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *dim = NULL;

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC), dim, 0, BP_VAR_W TSRMLS_CC);

	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_RW_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *dim = NULL;

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC), dim, 0, BP_VAR_RW TSRMLS_CC);

	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_FUNC_ARG_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int type = ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)?BP_VAR_W:BP_VAR_R;
	zval *dim;

	if (IS_UNUSED == IS_UNUSED && type == BP_VAR_R) {
		zend_error_noreturn(E_ERROR, "Cannot use [] for reading");
	}
	dim = NULL;
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), type TSRMLS_CC), dim, 0, type TSRMLS_CC);

	if (IS_CV == IS_VAR && type == BP_VAR_W && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_DIM_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;

	zval **object_ptr;

	if (IS_CV == IS_CV || EX_T(opline->op1.u.var).var.ptr_ptr) {
		/* not an array offset */
		object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	} else {
		object_ptr = NULL;
	}

	if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
		zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_DIM TSRMLS_CC);
	} else {
		zend_free_op free_op_data1;
		zval *value;
		zval *dim = NULL;

		zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), object_ptr, dim, 0, BP_VAR_W TSRMLS_CC);

		value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	 	zend_assign_to_variable(&opline->result, &op_data->op2, &op_data->op1, value, (IS_TMP_FREE(free_op_data1)?IS_TMP_VAR:op_data->op1.op_type), EX(Ts) TSRMLS_CC);
	 	FREE_OP_IF_VAR(free_op_data1);
	}

	/* assign_dim has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=NULL;

#if 0 || IS_CV == IS_VAR || IS_CV == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=_get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
	}
#else
	expr_ptr=_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
#endif

	if (0) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_CV == IS_VAR || IS_CV == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}

	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {

	} else {

	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_CV == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_CV != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_CV_UNUSED_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_ADD_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	add_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SUB_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	sub_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MUL_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	mul_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_DIV_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	div_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_MOD_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	mod_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SL_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_SR_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CONCAT_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	concat_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_IDENTICAL_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_IDENTICAL_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_EQUAL_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_NOT_EQUAL_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_IS_SMALLER_OR_EQUAL_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_OR_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_AND_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BW_XOR_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_BOOL_XOR_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);


	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
		_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_obj_helper_SPEC_CV_CV(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op_data1;
	zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	znode *result = &opline->result;
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");

		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
	} else {
		/* here we are sure we are dealing with an object */
		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = *zptr;
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;

			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					if (Z_OBJ_HT_P(object)->read_property) {
						z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
				case ZEND_ASSIGN_DIM:
					if (Z_OBJ_HT_P(object)->read_dimension) {
						z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (z->refcount == 0) {
						zval_dtor(z);
						FREE_ZVAL(z);
					}
					z = value;
				}
				z->refcount++;
				SEPARATE_ZVAL_IF_NOT_REF(&z);
				binary_op(z, z, value TSRMLS_CC);
				switch (opline->extended_value) {
					case ZEND_ASSIGN_OBJ:
						Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
						break;
					case ZEND_ASSIGN_DIM:
						Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
						break;
				}
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = z;
					PZVAL_LOCK(*retval);
				}
				zval_ptr_dtor(&z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = EG(uninitialized_zval_ptr);
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (0) {
			zval_ptr_dtor(&property);
		} else {

		}
		FREE_OP(free_op_data1);
	}

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int zend_binary_assign_op_helper_SPEC_CV_CV(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op_data2, free_op_data1;
	zval **var_ptr;
	zval *value;
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			return zend_binary_assign_op_obj_helper_SPEC_CV_CV(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);

				if (object_ptr && IS_CV != IS_CV && !0) {
					(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
				}

				if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
					return zend_binary_assign_op_obj_helper_SPEC_CV_CV(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
				} else {
					zend_op *op_data = opline+1;
					zval *dim = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC), dim, 0, BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), &free_op_data2, BP_VAR_RW);
					increment_opline = 1;
				}
			}
			break;
		default:
			value = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
			var_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC);
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}


		if (increment_opline) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	if (increment_opline) {
		ZEND_VM_INC_OPCODE();
		FREE_OP(free_op_data1);
		FREE_OP_VAR_PTR(free_op_data2);
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_ADD_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CV(add_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SUB_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CV(sub_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MUL_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CV(mul_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_DIV_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CV(div_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_MOD_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CV(mod_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SL_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CV(shift_left_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_SR_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CV(shift_right_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_CONCAT_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CV(concat_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_OR_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CV(bitwise_or_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_AND_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CV(bitwise_and_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ASSIGN_BW_XOR_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper_SPEC_CV_CV(bitwise_xor_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_pre_incdec_property_helper_SPEC_CV_CV(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval **retval = &EX_T(opline->result.u.var).var.ptr;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");

		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}

		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			have_get_ptr = 1;
			incdec_op(*zptr);
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = *zptr;
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			z->refcount++;
			SEPARATE_ZVAL_IF_NOT_REF(&z);
			incdec_op(z);
			*retval = z;
			Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = EG(uninitialized_zval_ptr);
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_PRE_INC_OBJ_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_CV_CV(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_PRE_DEC_OBJ_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_pre_incdec_property_helper_SPEC_CV_CV(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int zend_post_incdec_property_helper_SPEC_CV_CV(incdec_t incdec_op, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	zval *object;
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval *retval = &EX_T(opline->result.u.var).tmp_var;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");

		*retval = *EG(uninitialized_zval_ptr);

		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			have_get_ptr = 1;
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			*retval = **zptr;
			zendi_zval_copy_ctor(*retval);

			incdec_op(*zptr);

		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
			zval *z_copy;

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			*retval = *z;
			zendi_zval_copy_ctor(*retval);
			ALLOC_ZVAL(z_copy);
			*z_copy = *z;
			zendi_zval_copy_ctor(*z_copy);
			INIT_PZVAL(z_copy);
			incdec_op(z_copy);
			z->refcount++;
			Z_OBJ_HT_P(object)->write_property(object, property, z_copy TSRMLS_CC);
			zval_ptr_dtor(&z_copy);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			*retval = *EG(uninitialized_zval_ptr);
		}
	}

	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_POST_INC_OBJ_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_CV_CV(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_POST_DEC_OBJ_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_post_incdec_property_helper_SPEC_CV_CV(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_DIM_R_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *dim = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK &&
	    IS_CV != IS_CV &&
	    EX_T(opline->op1.u.var).var.ptr_ptr) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC), dim, 0, BP_VAR_R TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_W_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *dim = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC), dim, 0, BP_VAR_W TSRMLS_CC);

	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_RW_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *dim = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC), dim, 0, BP_VAR_RW TSRMLS_CC);

	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_IS_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *dim = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_IS TSRMLS_CC), dim, 0, BP_VAR_IS TSRMLS_CC);


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_FUNC_ARG_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int type = ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)?BP_VAR_W:BP_VAR_R;
	zval *dim;

	if (IS_CV == IS_UNUSED && type == BP_VAR_R) {
		zend_error_noreturn(E_ERROR, "Cannot use [] for reading");
	}
	dim = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), type TSRMLS_CC), dim, 0, type TSRMLS_CC);

	if (IS_CV == IS_VAR && type == BP_VAR_W && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_DIM_UNSET_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_UNSET TSRMLS_CC);
	zval *dim = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	/* Not needed in DIM_UNSET
	if (opline->extended_value == ZEND_FETCH_ADD_LOCK) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	*/
	if (IS_CV == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, dim, 0, BP_VAR_UNSET TSRMLS_CC);

	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	if (EX_T(opline->result.u.var).var.ptr_ptr == NULL) {
		zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
	} else {
		zend_free_op free_res;

		PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
		if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
		}
		PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		FREE_OP_VAR_PTR(free_res);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int zend_fetch_property_address_read_helper_SPEC_CV_CV(int type, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *container;
	zval **retval;


	retval = &EX_T(opline->result.u.var).var.ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = retval;

	container = _get_zval_ptr_cv(&opline->op1, EX(Ts), type TSRMLS_CC);

	if (container == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(error_zval_ptr);
			PZVAL_LOCK(*retval);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		ZEND_VM_NEXT_OPCODE();
	}


	if (Z_TYPE_P(container) != IS_OBJECT || !Z_OBJ_HT_P(container)->read_property) {
		if (type != BP_VAR_IS) {
			zend_error(E_NOTICE, "Trying to get property of non-object");
		}
		*retval = EG(uninitialized_zval_ptr);
		SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	} else {

		zval *offset  = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

		if (0) {
			MAKE_REAL_ZVAL_PTR(offset);
		}

		/* here we are sure we are dealing with an object */
		*retval = Z_OBJ_HT_P(container)->read_property(container, offset, type TSRMLS_CC);

		if (RETURN_VALUE_UNUSED(&opline->result) && ((*retval)->refcount == 0)) {
			zval_dtor(*retval);
			FREE_ZVAL(*retval);
		} else {
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}

		if (0) {
			zval_ptr_dtor(&offset);
		} else {

		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_R_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_CV_CV(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_W_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK && IS_CV != IS_CV) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
		EX_T(opline->op1.u.var).var.ptr = *EX_T(opline->op1.u.var).var.ptr_ptr;
	}

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC), property, BP_VAR_W TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_RW_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_RW TSRMLS_CC), property, BP_VAR_RW TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_FETCH_OBJ_IS_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_fetch_property_address_read_helper_SPEC_CV_CV(BP_VAR_IS, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_FETCH_OBJ_FUNC_ARG_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		/* Behave like FETCH_OBJ_W */
		zend_free_op free_op1;
		zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

		if (0) {
			MAKE_REAL_ZVAL_PTR(property);
		}
		zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC), property, BP_VAR_W TSRMLS_CC);
		if (0) {
			zval_ptr_dtor(&property);
		} else {

		}
		if (IS_CV == IS_VAR && 0 &&
		    READY_TO_DESTROY(free_op1.var) &&
		    !RETURN_VALUE_UNUSED(&opline->result)) {
			AI_USE_PTR(EX_T(opline->result.u.var).var);
			if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
			    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
				SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
			}
		}

		ZEND_VM_NEXT_OPCODE();
	} else {
		return zend_fetch_property_address_read_helper_SPEC_CV_CV(BP_VAR_R, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}
}

static int ZEND_FETCH_OBJ_UNSET_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_res;
	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval *property = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (IS_CV == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	if (0) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, property, BP_VAR_UNSET TSRMLS_CC);
	if (0) {
		zval_ptr_dtor(&property);
	} else {

	}
	if (IS_CV == IS_VAR && 0 &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}

	PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
	if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
		SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
	}
	PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	FREE_OP_VAR_PTR(free_res);
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_OBJ_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;

	zval **object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);

	zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_OBJ TSRMLS_CC);

	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_DIM_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;

	zval **object_ptr;

	if (IS_CV == IS_CV || EX_T(opline->op1.u.var).var.ptr_ptr) {
		/* not an array offset */
		object_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	} else {
		object_ptr = NULL;
	}

	if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
		zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_DIM TSRMLS_CC);
	} else {
		zend_free_op free_op_data1;
		zval *value;
		zval *dim = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

		zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), object_ptr, dim, 0, BP_VAR_W TSRMLS_CC);

		value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	 	zend_assign_to_variable(&opline->result, &op_data->op2, &op_data->op1, value, (IS_TMP_FREE(free_op_data1)?IS_TMP_VAR:op_data->op1.op_type), EX(Ts) TSRMLS_CC);
	 	FREE_OP_IF_VAR(free_op_data1);
	}

	/* assign_dim has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *value = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

 	zend_assign_to_variable(&opline->result, &opline->op1, &opline->op2, value, (0?IS_TMP_VAR:IS_CV), EX(Ts) TSRMLS_CC);
	/* zend_assign_to_variable() always takes care of op2, never free it! */

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ASSIGN_REF_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval **variable_ptr_ptr;
	zval **value_ptr_ptr = _get_zval_ptr_ptr_cv(&opline->op2, EX(Ts), BP_VAR_W TSRMLS_CC);

	if (IS_CV == IS_VAR &&
	    value_ptr_ptr &&
	    !(*value_ptr_ptr)->is_ref &&
	    opline->extended_value == ZEND_RETURNS_FUNCTION &&
	    !EX_T(opline->op2.u.var).var.fcall_returned_reference) {
		if (free_op2.var == NULL) {
			PZVAL_LOCK(*value_ptr_ptr); /* undo the effect of get_zval_ptr_ptr() */
		}
		zend_error(E_STRICT, "Only variables should be assigned by reference");
		if (EG(exception)) {

			ZEND_VM_NEXT_OPCODE();
		}
		return ZEND_ASSIGN_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	} else if (IS_CV == IS_VAR && opline->extended_value == ZEND_RETURNS_NEW) {
		PZVAL_LOCK(*value_ptr_ptr);
	}
	if (IS_CV == IS_VAR && EX_T(opline->op1.u.var).var.ptr_ptr == &EX_T(opline->op1.u.var).var.ptr) {
		zend_error(E_ERROR, "Cannot assign by reference to overloaded object");
	}

	variable_ptr_ptr = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	zend_assign_to_variable_reference(variable_ptr_ptr, value_ptr_ptr TSRMLS_CC);

	if (IS_CV == IS_VAR && opline->extended_value == ZEND_RETURNS_NEW) {
		(*variable_ptr_ptr)->refcount--;
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = variable_ptr_ptr;
		PZVAL_LOCK(*variable_ptr_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_METHOD_CALL_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;


	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	function_name = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = Z_STRVAL_P(function_name);
	function_name_strlen = Z_STRLEN_P(function_name);

	EX(object) = _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (EX(object) && Z_TYPE_P(EX(object)) == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(&EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}


	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_CASE_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;


	if (IS_CV==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 _get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC),
				 _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC) TSRMLS_CC);

	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */

		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ADD_ARRAY_ELEMENT_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=_get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

#if 0 || IS_CV == IS_VAR || IS_CV == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=_get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
	}
#else
	expr_ptr=_get_zval_ptr_cv(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
#endif

	if (0) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if 0 || IS_CV == IS_VAR || IS_CV == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), (long) Z_DVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}

	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {

	} else {

	}
	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_INIT_ARRAY_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (IS_CV == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if 0 || IS_CV != IS_UNUSED
	} else {
		return ZEND_ADD_ARRAY_ELEMENT_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
#endif
	}
}

static int ZEND_UNSET_DIM_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_UNSET TSRMLS_CC);
	zval *offset = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
	long index;

	if (container) {
		if (IS_CV == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		switch (Z_TYPE_PP(container)) {
			case IS_ARRAY: {
				HashTable *ht = Z_ARRVAL_PP(container);

				switch (Z_TYPE_P(offset)) {
					case IS_DOUBLE:
						index = (long) Z_DVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_RESOURCE:
					case IS_BOOL:
					case IS_LONG:
						index = Z_LVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_STRING:
						if (IS_CV == IS_CV || IS_CV == IS_VAR) {
							offset->refcount++;
						}
						if (zend_symtable_del(ht, offset->value.str.val, offset->value.str.len+1) == SUCCESS &&
					    ht == &EG(symbol_table)) {
							zend_execute_data *ex;
							ulong hash_value = zend_inline_hash_func(offset->value.str.val, offset->value.str.len+1);

							for (ex = execute_data; ex; ex = ex->prev_execute_data) {
								if (ex->op_array && ex->symbol_table == ht) {
									int i;

									for (i = 0; i < ex->op_array->last_var; i++) {
										if (ex->op_array->vars[i].hash_value == hash_value &&
										    ex->op_array->vars[i].name_len == offset->value.str.len &&
										    !memcmp(ex->op_array->vars[i].name, offset->value.str.val, offset->value.str.len)) {
											ex->CVs[i] = NULL;
											break;
										}
									}
								}
							}
						}
						if (IS_CV == IS_CV || IS_CV == IS_VAR) {
							zval_ptr_dtor(&offset);
						}
						break;
					case IS_NULL:
						zend_hash_del(ht, "", sizeof(""));
						break;
					default:
						zend_error(E_WARNING, "Illegal offset type in unset");
						break;
				}

				break;
			}
			case IS_OBJECT:
				if (!Z_OBJ_HT_P(*container)->unset_dimension) {
					zend_error_noreturn(E_ERROR, "Cannot use object as array");
				}
				if (0) {
					MAKE_REAL_ZVAL_PTR(offset);
				}
				Z_OBJ_HT_P(*container)->unset_dimension(*container, offset TSRMLS_CC);
				if (0) {
					zval_ptr_dtor(&offset);
				} else {

				}
				break;
			case IS_STRING:
				zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
				ZEND_VM_CONTINUE(); /* bailed out before */
			default:

				break;
		}
	} else {

	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_UNSET_OBJ_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_UNSET TSRMLS_CC);
	zval *offset = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	if (container) {
		if (IS_CV == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			Z_OBJ_HT_P(*container)->unset_property(*container, offset TSRMLS_CC);
			if (0) {
				zval_ptr_dtor(&offset);
			} else {

			}
		} else {

		}
	} else {

	}

	ZEND_VM_NEXT_OPCODE();
}

static int zend_isset_isempty_dim_prop_obj_handler_SPEC_CV_CV(int prop_dim, ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = EX(opline);

	zval **container = _get_zval_ptr_ptr_cv(&opline->op1, EX(Ts), BP_VAR_IS TSRMLS_CC);
	zval **value = NULL;
	int result = 0;
	long index;

	if (container) {

		zval *offset = _get_zval_ptr_cv(&opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

		if (Z_TYPE_PP(container) == IS_ARRAY) {
			HashTable *ht;
			int isset = 0;

			ht = Z_ARRVAL_PP(container);

			switch (Z_TYPE_P(offset)) {
				case IS_DOUBLE:
					index = (long) Z_DVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_RESOURCE:
				case IS_BOOL:
				case IS_LONG:
					index = Z_LVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_STRING:
					if (zend_symtable_find(ht, offset->value.str.val, offset->value.str.len+1, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_NULL:
					if (zend_hash_find(ht, "", sizeof(""), (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				default:
					zend_error(E_WARNING, "Illegal offset type in isset or empty");

					break;
			}

			switch (opline->extended_value) {
				case ZEND_ISSET:
					if (isset && Z_TYPE_PP(value) == IS_NULL) {
						result = 0;
					} else {
						result = isset;
					}
					break;
				case ZEND_ISEMPTY:
					if (!isset || !i_zend_is_true(*value)) {
						result = 0;
					} else {
						result = 1;
					}
					break;
			}

		} else if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (0) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			if (prop_dim) {
				result = Z_OBJ_HT_P(*container)->has_property(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			} else {
				result = Z_OBJ_HT_P(*container)->has_dimension(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			}
			if (0) {
				zval_ptr_dtor(&offset);
			} else {

			}
		} else if ((*container)->type == IS_STRING && !prop_dim) { /* string offsets */
			zval tmp;

			if (Z_TYPE_P(offset) != IS_LONG) {
				tmp = *offset;
				zval_copy_ctor(&tmp);
				convert_to_long(&tmp);
				offset = &tmp;
			}
			if (Z_TYPE_P(offset) == IS_LONG) {
				switch (opline->extended_value) {
					case ZEND_ISSET:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container)) {
							result = 1;
						}
						break;
					case ZEND_ISEMPTY:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container) && Z_STRVAL_PP(container)[offset->value.lval] != '0') {
							result = 1;
						}
						break;
				}
			}

		} else {

		}
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = result;
			break;
		case ZEND_ISEMPTY:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = !result;
			break;
	}

	ZEND_VM_NEXT_OPCODE();
}

static int ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_CV_CV(0, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_CV_CV_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler_SPEC_CV_CV(1, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

static int ZEND_NULL_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_error_noreturn(E_ERROR, "Invalid opcode %d/%d/%d.", EX(opline)->opcode, EX(opline)->op1.op_type, EX(opline)->op2.op_type);
	ZEND_VM_RETURN_FROM_EXECUTE_LOOP();
}


void zend_init_opcodes_handlers(void)
{
  static const opcode_handler_t labels[] = {
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_NOP_SPEC_HANDLER,
  	ZEND_ADD_SPEC_CONST_CONST_HANDLER,
  	ZEND_ADD_SPEC_CONST_TMP_HANDLER,
  	ZEND_ADD_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ADD_SPEC_CONST_CV_HANDLER,
  	ZEND_ADD_SPEC_TMP_CONST_HANDLER,
  	ZEND_ADD_SPEC_TMP_TMP_HANDLER,
  	ZEND_ADD_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ADD_SPEC_TMP_CV_HANDLER,
  	ZEND_ADD_SPEC_VAR_CONST_HANDLER,
  	ZEND_ADD_SPEC_VAR_TMP_HANDLER,
  	ZEND_ADD_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ADD_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ADD_SPEC_CV_CONST_HANDLER,
  	ZEND_ADD_SPEC_CV_TMP_HANDLER,
  	ZEND_ADD_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ADD_SPEC_CV_CV_HANDLER,
  	ZEND_SUB_SPEC_CONST_CONST_HANDLER,
  	ZEND_SUB_SPEC_CONST_TMP_HANDLER,
  	ZEND_SUB_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SUB_SPEC_CONST_CV_HANDLER,
  	ZEND_SUB_SPEC_TMP_CONST_HANDLER,
  	ZEND_SUB_SPEC_TMP_TMP_HANDLER,
  	ZEND_SUB_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SUB_SPEC_TMP_CV_HANDLER,
  	ZEND_SUB_SPEC_VAR_CONST_HANDLER,
  	ZEND_SUB_SPEC_VAR_TMP_HANDLER,
  	ZEND_SUB_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SUB_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SUB_SPEC_CV_CONST_HANDLER,
  	ZEND_SUB_SPEC_CV_TMP_HANDLER,
  	ZEND_SUB_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SUB_SPEC_CV_CV_HANDLER,
  	ZEND_MUL_SPEC_CONST_CONST_HANDLER,
  	ZEND_MUL_SPEC_CONST_TMP_HANDLER,
  	ZEND_MUL_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_MUL_SPEC_CONST_CV_HANDLER,
  	ZEND_MUL_SPEC_TMP_CONST_HANDLER,
  	ZEND_MUL_SPEC_TMP_TMP_HANDLER,
  	ZEND_MUL_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_MUL_SPEC_TMP_CV_HANDLER,
  	ZEND_MUL_SPEC_VAR_CONST_HANDLER,
  	ZEND_MUL_SPEC_VAR_TMP_HANDLER,
  	ZEND_MUL_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_MUL_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_MUL_SPEC_CV_CONST_HANDLER,
  	ZEND_MUL_SPEC_CV_TMP_HANDLER,
  	ZEND_MUL_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_MUL_SPEC_CV_CV_HANDLER,
  	ZEND_DIV_SPEC_CONST_CONST_HANDLER,
  	ZEND_DIV_SPEC_CONST_TMP_HANDLER,
  	ZEND_DIV_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_DIV_SPEC_CONST_CV_HANDLER,
  	ZEND_DIV_SPEC_TMP_CONST_HANDLER,
  	ZEND_DIV_SPEC_TMP_TMP_HANDLER,
  	ZEND_DIV_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_DIV_SPEC_TMP_CV_HANDLER,
  	ZEND_DIV_SPEC_VAR_CONST_HANDLER,
  	ZEND_DIV_SPEC_VAR_TMP_HANDLER,
  	ZEND_DIV_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_DIV_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_DIV_SPEC_CV_CONST_HANDLER,
  	ZEND_DIV_SPEC_CV_TMP_HANDLER,
  	ZEND_DIV_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_DIV_SPEC_CV_CV_HANDLER,
  	ZEND_MOD_SPEC_CONST_CONST_HANDLER,
  	ZEND_MOD_SPEC_CONST_TMP_HANDLER,
  	ZEND_MOD_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_MOD_SPEC_CONST_CV_HANDLER,
  	ZEND_MOD_SPEC_TMP_CONST_HANDLER,
  	ZEND_MOD_SPEC_TMP_TMP_HANDLER,
  	ZEND_MOD_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_MOD_SPEC_TMP_CV_HANDLER,
  	ZEND_MOD_SPEC_VAR_CONST_HANDLER,
  	ZEND_MOD_SPEC_VAR_TMP_HANDLER,
  	ZEND_MOD_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_MOD_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_MOD_SPEC_CV_CONST_HANDLER,
  	ZEND_MOD_SPEC_CV_TMP_HANDLER,
  	ZEND_MOD_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_MOD_SPEC_CV_CV_HANDLER,
  	ZEND_SL_SPEC_CONST_CONST_HANDLER,
  	ZEND_SL_SPEC_CONST_TMP_HANDLER,
  	ZEND_SL_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SL_SPEC_CONST_CV_HANDLER,
  	ZEND_SL_SPEC_TMP_CONST_HANDLER,
  	ZEND_SL_SPEC_TMP_TMP_HANDLER,
  	ZEND_SL_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SL_SPEC_TMP_CV_HANDLER,
  	ZEND_SL_SPEC_VAR_CONST_HANDLER,
  	ZEND_SL_SPEC_VAR_TMP_HANDLER,
  	ZEND_SL_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SL_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SL_SPEC_CV_CONST_HANDLER,
  	ZEND_SL_SPEC_CV_TMP_HANDLER,
  	ZEND_SL_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SL_SPEC_CV_CV_HANDLER,
  	ZEND_SR_SPEC_CONST_CONST_HANDLER,
  	ZEND_SR_SPEC_CONST_TMP_HANDLER,
  	ZEND_SR_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SR_SPEC_CONST_CV_HANDLER,
  	ZEND_SR_SPEC_TMP_CONST_HANDLER,
  	ZEND_SR_SPEC_TMP_TMP_HANDLER,
  	ZEND_SR_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SR_SPEC_TMP_CV_HANDLER,
  	ZEND_SR_SPEC_VAR_CONST_HANDLER,
  	ZEND_SR_SPEC_VAR_TMP_HANDLER,
  	ZEND_SR_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SR_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SR_SPEC_CV_CONST_HANDLER,
  	ZEND_SR_SPEC_CV_TMP_HANDLER,
  	ZEND_SR_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SR_SPEC_CV_CV_HANDLER,
  	ZEND_CONCAT_SPEC_CONST_CONST_HANDLER,
  	ZEND_CONCAT_SPEC_CONST_TMP_HANDLER,
  	ZEND_CONCAT_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_CONCAT_SPEC_CONST_CV_HANDLER,
  	ZEND_CONCAT_SPEC_TMP_CONST_HANDLER,
  	ZEND_CONCAT_SPEC_TMP_TMP_HANDLER,
  	ZEND_CONCAT_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_CONCAT_SPEC_TMP_CV_HANDLER,
  	ZEND_CONCAT_SPEC_VAR_CONST_HANDLER,
  	ZEND_CONCAT_SPEC_VAR_TMP_HANDLER,
  	ZEND_CONCAT_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_CONCAT_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_CONCAT_SPEC_CV_CONST_HANDLER,
  	ZEND_CONCAT_SPEC_CV_TMP_HANDLER,
  	ZEND_CONCAT_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_CONCAT_SPEC_CV_CV_HANDLER,
  	ZEND_BW_OR_SPEC_CONST_CONST_HANDLER,
  	ZEND_BW_OR_SPEC_CONST_TMP_HANDLER,
  	ZEND_BW_OR_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BW_OR_SPEC_CONST_CV_HANDLER,
  	ZEND_BW_OR_SPEC_TMP_CONST_HANDLER,
  	ZEND_BW_OR_SPEC_TMP_TMP_HANDLER,
  	ZEND_BW_OR_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BW_OR_SPEC_TMP_CV_HANDLER,
  	ZEND_BW_OR_SPEC_VAR_CONST_HANDLER,
  	ZEND_BW_OR_SPEC_VAR_TMP_HANDLER,
  	ZEND_BW_OR_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BW_OR_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BW_OR_SPEC_CV_CONST_HANDLER,
  	ZEND_BW_OR_SPEC_CV_TMP_HANDLER,
  	ZEND_BW_OR_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BW_OR_SPEC_CV_CV_HANDLER,
  	ZEND_BW_AND_SPEC_CONST_CONST_HANDLER,
  	ZEND_BW_AND_SPEC_CONST_TMP_HANDLER,
  	ZEND_BW_AND_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BW_AND_SPEC_CONST_CV_HANDLER,
  	ZEND_BW_AND_SPEC_TMP_CONST_HANDLER,
  	ZEND_BW_AND_SPEC_TMP_TMP_HANDLER,
  	ZEND_BW_AND_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BW_AND_SPEC_TMP_CV_HANDLER,
  	ZEND_BW_AND_SPEC_VAR_CONST_HANDLER,
  	ZEND_BW_AND_SPEC_VAR_TMP_HANDLER,
  	ZEND_BW_AND_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BW_AND_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BW_AND_SPEC_CV_CONST_HANDLER,
  	ZEND_BW_AND_SPEC_CV_TMP_HANDLER,
  	ZEND_BW_AND_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BW_AND_SPEC_CV_CV_HANDLER,
  	ZEND_BW_XOR_SPEC_CONST_CONST_HANDLER,
  	ZEND_BW_XOR_SPEC_CONST_TMP_HANDLER,
  	ZEND_BW_XOR_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BW_XOR_SPEC_CONST_CV_HANDLER,
  	ZEND_BW_XOR_SPEC_TMP_CONST_HANDLER,
  	ZEND_BW_XOR_SPEC_TMP_TMP_HANDLER,
  	ZEND_BW_XOR_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BW_XOR_SPEC_TMP_CV_HANDLER,
  	ZEND_BW_XOR_SPEC_VAR_CONST_HANDLER,
  	ZEND_BW_XOR_SPEC_VAR_TMP_HANDLER,
  	ZEND_BW_XOR_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BW_XOR_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BW_XOR_SPEC_CV_CONST_HANDLER,
  	ZEND_BW_XOR_SPEC_CV_TMP_HANDLER,
  	ZEND_BW_XOR_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BW_XOR_SPEC_CV_CV_HANDLER,
  	ZEND_BW_NOT_SPEC_CONST_HANDLER,
  	ZEND_BW_NOT_SPEC_CONST_HANDLER,
  	ZEND_BW_NOT_SPEC_CONST_HANDLER,
  	ZEND_BW_NOT_SPEC_CONST_HANDLER,
  	ZEND_BW_NOT_SPEC_CONST_HANDLER,
  	ZEND_BW_NOT_SPEC_TMP_HANDLER,
  	ZEND_BW_NOT_SPEC_TMP_HANDLER,
  	ZEND_BW_NOT_SPEC_TMP_HANDLER,
  	ZEND_BW_NOT_SPEC_TMP_HANDLER,
  	ZEND_BW_NOT_SPEC_TMP_HANDLER,
  	ZEND_BW_NOT_SPEC_VAR_HANDLER,
  	ZEND_BW_NOT_SPEC_VAR_HANDLER,
  	ZEND_BW_NOT_SPEC_VAR_HANDLER,
  	ZEND_BW_NOT_SPEC_VAR_HANDLER,
  	ZEND_BW_NOT_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BW_NOT_SPEC_CV_HANDLER,
  	ZEND_BW_NOT_SPEC_CV_HANDLER,
  	ZEND_BW_NOT_SPEC_CV_HANDLER,
  	ZEND_BW_NOT_SPEC_CV_HANDLER,
  	ZEND_BW_NOT_SPEC_CV_HANDLER,
  	ZEND_BOOL_NOT_SPEC_CONST_HANDLER,
  	ZEND_BOOL_NOT_SPEC_CONST_HANDLER,
  	ZEND_BOOL_NOT_SPEC_CONST_HANDLER,
  	ZEND_BOOL_NOT_SPEC_CONST_HANDLER,
  	ZEND_BOOL_NOT_SPEC_CONST_HANDLER,
  	ZEND_BOOL_NOT_SPEC_TMP_HANDLER,
  	ZEND_BOOL_NOT_SPEC_TMP_HANDLER,
  	ZEND_BOOL_NOT_SPEC_TMP_HANDLER,
  	ZEND_BOOL_NOT_SPEC_TMP_HANDLER,
  	ZEND_BOOL_NOT_SPEC_TMP_HANDLER,
  	ZEND_BOOL_NOT_SPEC_VAR_HANDLER,
  	ZEND_BOOL_NOT_SPEC_VAR_HANDLER,
  	ZEND_BOOL_NOT_SPEC_VAR_HANDLER,
  	ZEND_BOOL_NOT_SPEC_VAR_HANDLER,
  	ZEND_BOOL_NOT_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BOOL_NOT_SPEC_CV_HANDLER,
  	ZEND_BOOL_NOT_SPEC_CV_HANDLER,
  	ZEND_BOOL_NOT_SPEC_CV_HANDLER,
  	ZEND_BOOL_NOT_SPEC_CV_HANDLER,
  	ZEND_BOOL_NOT_SPEC_CV_HANDLER,
  	ZEND_BOOL_XOR_SPEC_CONST_CONST_HANDLER,
  	ZEND_BOOL_XOR_SPEC_CONST_TMP_HANDLER,
  	ZEND_BOOL_XOR_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BOOL_XOR_SPEC_CONST_CV_HANDLER,
  	ZEND_BOOL_XOR_SPEC_TMP_CONST_HANDLER,
  	ZEND_BOOL_XOR_SPEC_TMP_TMP_HANDLER,
  	ZEND_BOOL_XOR_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BOOL_XOR_SPEC_TMP_CV_HANDLER,
  	ZEND_BOOL_XOR_SPEC_VAR_CONST_HANDLER,
  	ZEND_BOOL_XOR_SPEC_VAR_TMP_HANDLER,
  	ZEND_BOOL_XOR_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BOOL_XOR_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BOOL_XOR_SPEC_CV_CONST_HANDLER,
  	ZEND_BOOL_XOR_SPEC_CV_TMP_HANDLER,
  	ZEND_BOOL_XOR_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BOOL_XOR_SPEC_CV_CV_HANDLER,
  	ZEND_IS_IDENTICAL_SPEC_CONST_CONST_HANDLER,
  	ZEND_IS_IDENTICAL_SPEC_CONST_TMP_HANDLER,
  	ZEND_IS_IDENTICAL_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_IDENTICAL_SPEC_CONST_CV_HANDLER,
  	ZEND_IS_IDENTICAL_SPEC_TMP_CONST_HANDLER,
  	ZEND_IS_IDENTICAL_SPEC_TMP_TMP_HANDLER,
  	ZEND_IS_IDENTICAL_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_IDENTICAL_SPEC_TMP_CV_HANDLER,
  	ZEND_IS_IDENTICAL_SPEC_VAR_CONST_HANDLER,
  	ZEND_IS_IDENTICAL_SPEC_VAR_TMP_HANDLER,
  	ZEND_IS_IDENTICAL_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_IDENTICAL_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_IDENTICAL_SPEC_CV_CONST_HANDLER,
  	ZEND_IS_IDENTICAL_SPEC_CV_TMP_HANDLER,
  	ZEND_IS_IDENTICAL_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_IDENTICAL_SPEC_CV_CV_HANDLER,
  	ZEND_IS_NOT_IDENTICAL_SPEC_CONST_CONST_HANDLER,
  	ZEND_IS_NOT_IDENTICAL_SPEC_CONST_TMP_HANDLER,
  	ZEND_IS_NOT_IDENTICAL_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_NOT_IDENTICAL_SPEC_CONST_CV_HANDLER,
  	ZEND_IS_NOT_IDENTICAL_SPEC_TMP_CONST_HANDLER,
  	ZEND_IS_NOT_IDENTICAL_SPEC_TMP_TMP_HANDLER,
  	ZEND_IS_NOT_IDENTICAL_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_NOT_IDENTICAL_SPEC_TMP_CV_HANDLER,
  	ZEND_IS_NOT_IDENTICAL_SPEC_VAR_CONST_HANDLER,
  	ZEND_IS_NOT_IDENTICAL_SPEC_VAR_TMP_HANDLER,
  	ZEND_IS_NOT_IDENTICAL_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_NOT_IDENTICAL_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_NOT_IDENTICAL_SPEC_CV_CONST_HANDLER,
  	ZEND_IS_NOT_IDENTICAL_SPEC_CV_TMP_HANDLER,
  	ZEND_IS_NOT_IDENTICAL_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_NOT_IDENTICAL_SPEC_CV_CV_HANDLER,
  	ZEND_IS_EQUAL_SPEC_CONST_CONST_HANDLER,
  	ZEND_IS_EQUAL_SPEC_CONST_TMP_HANDLER,
  	ZEND_IS_EQUAL_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_EQUAL_SPEC_CONST_CV_HANDLER,
  	ZEND_IS_EQUAL_SPEC_TMP_CONST_HANDLER,
  	ZEND_IS_EQUAL_SPEC_TMP_TMP_HANDLER,
  	ZEND_IS_EQUAL_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_EQUAL_SPEC_TMP_CV_HANDLER,
  	ZEND_IS_EQUAL_SPEC_VAR_CONST_HANDLER,
  	ZEND_IS_EQUAL_SPEC_VAR_TMP_HANDLER,
  	ZEND_IS_EQUAL_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_EQUAL_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_EQUAL_SPEC_CV_CONST_HANDLER,
  	ZEND_IS_EQUAL_SPEC_CV_TMP_HANDLER,
  	ZEND_IS_EQUAL_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_EQUAL_SPEC_CV_CV_HANDLER,
  	ZEND_IS_NOT_EQUAL_SPEC_CONST_CONST_HANDLER,
  	ZEND_IS_NOT_EQUAL_SPEC_CONST_TMP_HANDLER,
  	ZEND_IS_NOT_EQUAL_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_NOT_EQUAL_SPEC_CONST_CV_HANDLER,
  	ZEND_IS_NOT_EQUAL_SPEC_TMP_CONST_HANDLER,
  	ZEND_IS_NOT_EQUAL_SPEC_TMP_TMP_HANDLER,
  	ZEND_IS_NOT_EQUAL_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_NOT_EQUAL_SPEC_TMP_CV_HANDLER,
  	ZEND_IS_NOT_EQUAL_SPEC_VAR_CONST_HANDLER,
  	ZEND_IS_NOT_EQUAL_SPEC_VAR_TMP_HANDLER,
  	ZEND_IS_NOT_EQUAL_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_NOT_EQUAL_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_NOT_EQUAL_SPEC_CV_CONST_HANDLER,
  	ZEND_IS_NOT_EQUAL_SPEC_CV_TMP_HANDLER,
  	ZEND_IS_NOT_EQUAL_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_NOT_EQUAL_SPEC_CV_CV_HANDLER,
  	ZEND_IS_SMALLER_SPEC_CONST_CONST_HANDLER,
  	ZEND_IS_SMALLER_SPEC_CONST_TMP_HANDLER,
  	ZEND_IS_SMALLER_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_SMALLER_SPEC_CONST_CV_HANDLER,
  	ZEND_IS_SMALLER_SPEC_TMP_CONST_HANDLER,
  	ZEND_IS_SMALLER_SPEC_TMP_TMP_HANDLER,
  	ZEND_IS_SMALLER_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_SMALLER_SPEC_TMP_CV_HANDLER,
  	ZEND_IS_SMALLER_SPEC_VAR_CONST_HANDLER,
  	ZEND_IS_SMALLER_SPEC_VAR_TMP_HANDLER,
  	ZEND_IS_SMALLER_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_SMALLER_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_SMALLER_SPEC_CV_CONST_HANDLER,
  	ZEND_IS_SMALLER_SPEC_CV_TMP_HANDLER,
  	ZEND_IS_SMALLER_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_SMALLER_SPEC_CV_CV_HANDLER,
  	ZEND_IS_SMALLER_OR_EQUAL_SPEC_CONST_CONST_HANDLER,
  	ZEND_IS_SMALLER_OR_EQUAL_SPEC_CONST_TMP_HANDLER,
  	ZEND_IS_SMALLER_OR_EQUAL_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_SMALLER_OR_EQUAL_SPEC_CONST_CV_HANDLER,
  	ZEND_IS_SMALLER_OR_EQUAL_SPEC_TMP_CONST_HANDLER,
  	ZEND_IS_SMALLER_OR_EQUAL_SPEC_TMP_TMP_HANDLER,
  	ZEND_IS_SMALLER_OR_EQUAL_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_SMALLER_OR_EQUAL_SPEC_TMP_CV_HANDLER,
  	ZEND_IS_SMALLER_OR_EQUAL_SPEC_VAR_CONST_HANDLER,
  	ZEND_IS_SMALLER_OR_EQUAL_SPEC_VAR_TMP_HANDLER,
  	ZEND_IS_SMALLER_OR_EQUAL_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_SMALLER_OR_EQUAL_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_SMALLER_OR_EQUAL_SPEC_CV_CONST_HANDLER,
  	ZEND_IS_SMALLER_OR_EQUAL_SPEC_CV_TMP_HANDLER,
  	ZEND_IS_SMALLER_OR_EQUAL_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_IS_SMALLER_OR_EQUAL_SPEC_CV_CV_HANDLER,
  	ZEND_CAST_SPEC_CONST_HANDLER,
  	ZEND_CAST_SPEC_CONST_HANDLER,
  	ZEND_CAST_SPEC_CONST_HANDLER,
  	ZEND_CAST_SPEC_CONST_HANDLER,
  	ZEND_CAST_SPEC_CONST_HANDLER,
  	ZEND_CAST_SPEC_TMP_HANDLER,
  	ZEND_CAST_SPEC_TMP_HANDLER,
  	ZEND_CAST_SPEC_TMP_HANDLER,
  	ZEND_CAST_SPEC_TMP_HANDLER,
  	ZEND_CAST_SPEC_TMP_HANDLER,
  	ZEND_CAST_SPEC_VAR_HANDLER,
  	ZEND_CAST_SPEC_VAR_HANDLER,
  	ZEND_CAST_SPEC_VAR_HANDLER,
  	ZEND_CAST_SPEC_VAR_HANDLER,
  	ZEND_CAST_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_CAST_SPEC_CV_HANDLER,
  	ZEND_CAST_SPEC_CV_HANDLER,
  	ZEND_CAST_SPEC_CV_HANDLER,
  	ZEND_CAST_SPEC_CV_HANDLER,
  	ZEND_CAST_SPEC_CV_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_CONST_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_CONST_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_CONST_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_CONST_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_CONST_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_TMP_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_TMP_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_TMP_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_TMP_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_TMP_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_VAR_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_VAR_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_VAR_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_VAR_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_CV_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_CV_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_CV_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_CV_HANDLER,
  	ZEND_QM_ASSIGN_SPEC_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_ADD_SPEC_VAR_CONST_HANDLER,
  	ZEND_ASSIGN_ADD_SPEC_VAR_TMP_HANDLER,
  	ZEND_ASSIGN_ADD_SPEC_VAR_VAR_HANDLER,
  	ZEND_ASSIGN_ADD_SPEC_VAR_UNUSED_HANDLER,
  	ZEND_ASSIGN_ADD_SPEC_VAR_CV_HANDLER,
  	ZEND_ASSIGN_ADD_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_ASSIGN_ADD_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_ASSIGN_ADD_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_ASSIGN_ADD_SPEC_UNUSED_UNUSED_HANDLER,
  	ZEND_ASSIGN_ADD_SPEC_UNUSED_CV_HANDLER,
  	ZEND_ASSIGN_ADD_SPEC_CV_CONST_HANDLER,
  	ZEND_ASSIGN_ADD_SPEC_CV_TMP_HANDLER,
  	ZEND_ASSIGN_ADD_SPEC_CV_VAR_HANDLER,
  	ZEND_ASSIGN_ADD_SPEC_CV_UNUSED_HANDLER,
  	ZEND_ASSIGN_ADD_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_SUB_SPEC_VAR_CONST_HANDLER,
  	ZEND_ASSIGN_SUB_SPEC_VAR_TMP_HANDLER,
  	ZEND_ASSIGN_SUB_SPEC_VAR_VAR_HANDLER,
  	ZEND_ASSIGN_SUB_SPEC_VAR_UNUSED_HANDLER,
  	ZEND_ASSIGN_SUB_SPEC_VAR_CV_HANDLER,
  	ZEND_ASSIGN_SUB_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_ASSIGN_SUB_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_ASSIGN_SUB_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_ASSIGN_SUB_SPEC_UNUSED_UNUSED_HANDLER,
  	ZEND_ASSIGN_SUB_SPEC_UNUSED_CV_HANDLER,
  	ZEND_ASSIGN_SUB_SPEC_CV_CONST_HANDLER,
  	ZEND_ASSIGN_SUB_SPEC_CV_TMP_HANDLER,
  	ZEND_ASSIGN_SUB_SPEC_CV_VAR_HANDLER,
  	ZEND_ASSIGN_SUB_SPEC_CV_UNUSED_HANDLER,
  	ZEND_ASSIGN_SUB_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_MUL_SPEC_VAR_CONST_HANDLER,
  	ZEND_ASSIGN_MUL_SPEC_VAR_TMP_HANDLER,
  	ZEND_ASSIGN_MUL_SPEC_VAR_VAR_HANDLER,
  	ZEND_ASSIGN_MUL_SPEC_VAR_UNUSED_HANDLER,
  	ZEND_ASSIGN_MUL_SPEC_VAR_CV_HANDLER,
  	ZEND_ASSIGN_MUL_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_ASSIGN_MUL_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_ASSIGN_MUL_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_ASSIGN_MUL_SPEC_UNUSED_UNUSED_HANDLER,
  	ZEND_ASSIGN_MUL_SPEC_UNUSED_CV_HANDLER,
  	ZEND_ASSIGN_MUL_SPEC_CV_CONST_HANDLER,
  	ZEND_ASSIGN_MUL_SPEC_CV_TMP_HANDLER,
  	ZEND_ASSIGN_MUL_SPEC_CV_VAR_HANDLER,
  	ZEND_ASSIGN_MUL_SPEC_CV_UNUSED_HANDLER,
  	ZEND_ASSIGN_MUL_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_DIV_SPEC_VAR_CONST_HANDLER,
  	ZEND_ASSIGN_DIV_SPEC_VAR_TMP_HANDLER,
  	ZEND_ASSIGN_DIV_SPEC_VAR_VAR_HANDLER,
  	ZEND_ASSIGN_DIV_SPEC_VAR_UNUSED_HANDLER,
  	ZEND_ASSIGN_DIV_SPEC_VAR_CV_HANDLER,
  	ZEND_ASSIGN_DIV_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_ASSIGN_DIV_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_ASSIGN_DIV_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_ASSIGN_DIV_SPEC_UNUSED_UNUSED_HANDLER,
  	ZEND_ASSIGN_DIV_SPEC_UNUSED_CV_HANDLER,
  	ZEND_ASSIGN_DIV_SPEC_CV_CONST_HANDLER,
  	ZEND_ASSIGN_DIV_SPEC_CV_TMP_HANDLER,
  	ZEND_ASSIGN_DIV_SPEC_CV_VAR_HANDLER,
  	ZEND_ASSIGN_DIV_SPEC_CV_UNUSED_HANDLER,
  	ZEND_ASSIGN_DIV_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_MOD_SPEC_VAR_CONST_HANDLER,
  	ZEND_ASSIGN_MOD_SPEC_VAR_TMP_HANDLER,
  	ZEND_ASSIGN_MOD_SPEC_VAR_VAR_HANDLER,
  	ZEND_ASSIGN_MOD_SPEC_VAR_UNUSED_HANDLER,
  	ZEND_ASSIGN_MOD_SPEC_VAR_CV_HANDLER,
  	ZEND_ASSIGN_MOD_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_ASSIGN_MOD_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_ASSIGN_MOD_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_ASSIGN_MOD_SPEC_UNUSED_UNUSED_HANDLER,
  	ZEND_ASSIGN_MOD_SPEC_UNUSED_CV_HANDLER,
  	ZEND_ASSIGN_MOD_SPEC_CV_CONST_HANDLER,
  	ZEND_ASSIGN_MOD_SPEC_CV_TMP_HANDLER,
  	ZEND_ASSIGN_MOD_SPEC_CV_VAR_HANDLER,
  	ZEND_ASSIGN_MOD_SPEC_CV_UNUSED_HANDLER,
  	ZEND_ASSIGN_MOD_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_SL_SPEC_VAR_CONST_HANDLER,
  	ZEND_ASSIGN_SL_SPEC_VAR_TMP_HANDLER,
  	ZEND_ASSIGN_SL_SPEC_VAR_VAR_HANDLER,
  	ZEND_ASSIGN_SL_SPEC_VAR_UNUSED_HANDLER,
  	ZEND_ASSIGN_SL_SPEC_VAR_CV_HANDLER,
  	ZEND_ASSIGN_SL_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_ASSIGN_SL_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_ASSIGN_SL_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_ASSIGN_SL_SPEC_UNUSED_UNUSED_HANDLER,
  	ZEND_ASSIGN_SL_SPEC_UNUSED_CV_HANDLER,
  	ZEND_ASSIGN_SL_SPEC_CV_CONST_HANDLER,
  	ZEND_ASSIGN_SL_SPEC_CV_TMP_HANDLER,
  	ZEND_ASSIGN_SL_SPEC_CV_VAR_HANDLER,
  	ZEND_ASSIGN_SL_SPEC_CV_UNUSED_HANDLER,
  	ZEND_ASSIGN_SL_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_SR_SPEC_VAR_CONST_HANDLER,
  	ZEND_ASSIGN_SR_SPEC_VAR_TMP_HANDLER,
  	ZEND_ASSIGN_SR_SPEC_VAR_VAR_HANDLER,
  	ZEND_ASSIGN_SR_SPEC_VAR_UNUSED_HANDLER,
  	ZEND_ASSIGN_SR_SPEC_VAR_CV_HANDLER,
  	ZEND_ASSIGN_SR_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_ASSIGN_SR_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_ASSIGN_SR_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_ASSIGN_SR_SPEC_UNUSED_UNUSED_HANDLER,
  	ZEND_ASSIGN_SR_SPEC_UNUSED_CV_HANDLER,
  	ZEND_ASSIGN_SR_SPEC_CV_CONST_HANDLER,
  	ZEND_ASSIGN_SR_SPEC_CV_TMP_HANDLER,
  	ZEND_ASSIGN_SR_SPEC_CV_VAR_HANDLER,
  	ZEND_ASSIGN_SR_SPEC_CV_UNUSED_HANDLER,
  	ZEND_ASSIGN_SR_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_CONCAT_SPEC_VAR_CONST_HANDLER,
  	ZEND_ASSIGN_CONCAT_SPEC_VAR_TMP_HANDLER,
  	ZEND_ASSIGN_CONCAT_SPEC_VAR_VAR_HANDLER,
  	ZEND_ASSIGN_CONCAT_SPEC_VAR_UNUSED_HANDLER,
  	ZEND_ASSIGN_CONCAT_SPEC_VAR_CV_HANDLER,
  	ZEND_ASSIGN_CONCAT_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_ASSIGN_CONCAT_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_ASSIGN_CONCAT_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_ASSIGN_CONCAT_SPEC_UNUSED_UNUSED_HANDLER,
  	ZEND_ASSIGN_CONCAT_SPEC_UNUSED_CV_HANDLER,
  	ZEND_ASSIGN_CONCAT_SPEC_CV_CONST_HANDLER,
  	ZEND_ASSIGN_CONCAT_SPEC_CV_TMP_HANDLER,
  	ZEND_ASSIGN_CONCAT_SPEC_CV_VAR_HANDLER,
  	ZEND_ASSIGN_CONCAT_SPEC_CV_UNUSED_HANDLER,
  	ZEND_ASSIGN_CONCAT_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_BW_OR_SPEC_VAR_CONST_HANDLER,
  	ZEND_ASSIGN_BW_OR_SPEC_VAR_TMP_HANDLER,
  	ZEND_ASSIGN_BW_OR_SPEC_VAR_VAR_HANDLER,
  	ZEND_ASSIGN_BW_OR_SPEC_VAR_UNUSED_HANDLER,
  	ZEND_ASSIGN_BW_OR_SPEC_VAR_CV_HANDLER,
  	ZEND_ASSIGN_BW_OR_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_ASSIGN_BW_OR_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_ASSIGN_BW_OR_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_ASSIGN_BW_OR_SPEC_UNUSED_UNUSED_HANDLER,
  	ZEND_ASSIGN_BW_OR_SPEC_UNUSED_CV_HANDLER,
  	ZEND_ASSIGN_BW_OR_SPEC_CV_CONST_HANDLER,
  	ZEND_ASSIGN_BW_OR_SPEC_CV_TMP_HANDLER,
  	ZEND_ASSIGN_BW_OR_SPEC_CV_VAR_HANDLER,
  	ZEND_ASSIGN_BW_OR_SPEC_CV_UNUSED_HANDLER,
  	ZEND_ASSIGN_BW_OR_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_BW_AND_SPEC_VAR_CONST_HANDLER,
  	ZEND_ASSIGN_BW_AND_SPEC_VAR_TMP_HANDLER,
  	ZEND_ASSIGN_BW_AND_SPEC_VAR_VAR_HANDLER,
  	ZEND_ASSIGN_BW_AND_SPEC_VAR_UNUSED_HANDLER,
  	ZEND_ASSIGN_BW_AND_SPEC_VAR_CV_HANDLER,
  	ZEND_ASSIGN_BW_AND_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_ASSIGN_BW_AND_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_ASSIGN_BW_AND_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_ASSIGN_BW_AND_SPEC_UNUSED_UNUSED_HANDLER,
  	ZEND_ASSIGN_BW_AND_SPEC_UNUSED_CV_HANDLER,
  	ZEND_ASSIGN_BW_AND_SPEC_CV_CONST_HANDLER,
  	ZEND_ASSIGN_BW_AND_SPEC_CV_TMP_HANDLER,
  	ZEND_ASSIGN_BW_AND_SPEC_CV_VAR_HANDLER,
  	ZEND_ASSIGN_BW_AND_SPEC_CV_UNUSED_HANDLER,
  	ZEND_ASSIGN_BW_AND_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_BW_XOR_SPEC_VAR_CONST_HANDLER,
  	ZEND_ASSIGN_BW_XOR_SPEC_VAR_TMP_HANDLER,
  	ZEND_ASSIGN_BW_XOR_SPEC_VAR_VAR_HANDLER,
  	ZEND_ASSIGN_BW_XOR_SPEC_VAR_UNUSED_HANDLER,
  	ZEND_ASSIGN_BW_XOR_SPEC_VAR_CV_HANDLER,
  	ZEND_ASSIGN_BW_XOR_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_ASSIGN_BW_XOR_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_ASSIGN_BW_XOR_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_ASSIGN_BW_XOR_SPEC_UNUSED_UNUSED_HANDLER,
  	ZEND_ASSIGN_BW_XOR_SPEC_UNUSED_CV_HANDLER,
  	ZEND_ASSIGN_BW_XOR_SPEC_CV_CONST_HANDLER,
  	ZEND_ASSIGN_BW_XOR_SPEC_CV_TMP_HANDLER,
  	ZEND_ASSIGN_BW_XOR_SPEC_CV_VAR_HANDLER,
  	ZEND_ASSIGN_BW_XOR_SPEC_CV_UNUSED_HANDLER,
  	ZEND_ASSIGN_BW_XOR_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_PRE_INC_SPEC_VAR_HANDLER,
  	ZEND_PRE_INC_SPEC_VAR_HANDLER,
  	ZEND_PRE_INC_SPEC_VAR_HANDLER,
  	ZEND_PRE_INC_SPEC_VAR_HANDLER,
  	ZEND_PRE_INC_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_PRE_INC_SPEC_CV_HANDLER,
  	ZEND_PRE_INC_SPEC_CV_HANDLER,
  	ZEND_PRE_INC_SPEC_CV_HANDLER,
  	ZEND_PRE_INC_SPEC_CV_HANDLER,
  	ZEND_PRE_INC_SPEC_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_PRE_DEC_SPEC_VAR_HANDLER,
  	ZEND_PRE_DEC_SPEC_VAR_HANDLER,
  	ZEND_PRE_DEC_SPEC_VAR_HANDLER,
  	ZEND_PRE_DEC_SPEC_VAR_HANDLER,
  	ZEND_PRE_DEC_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_PRE_DEC_SPEC_CV_HANDLER,
  	ZEND_PRE_DEC_SPEC_CV_HANDLER,
  	ZEND_PRE_DEC_SPEC_CV_HANDLER,
  	ZEND_PRE_DEC_SPEC_CV_HANDLER,
  	ZEND_PRE_DEC_SPEC_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_POST_INC_SPEC_VAR_HANDLER,
  	ZEND_POST_INC_SPEC_VAR_HANDLER,
  	ZEND_POST_INC_SPEC_VAR_HANDLER,
  	ZEND_POST_INC_SPEC_VAR_HANDLER,
  	ZEND_POST_INC_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_POST_INC_SPEC_CV_HANDLER,
  	ZEND_POST_INC_SPEC_CV_HANDLER,
  	ZEND_POST_INC_SPEC_CV_HANDLER,
  	ZEND_POST_INC_SPEC_CV_HANDLER,
  	ZEND_POST_INC_SPEC_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_POST_DEC_SPEC_VAR_HANDLER,
  	ZEND_POST_DEC_SPEC_VAR_HANDLER,
  	ZEND_POST_DEC_SPEC_VAR_HANDLER,
  	ZEND_POST_DEC_SPEC_VAR_HANDLER,
  	ZEND_POST_DEC_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_POST_DEC_SPEC_CV_HANDLER,
  	ZEND_POST_DEC_SPEC_CV_HANDLER,
  	ZEND_POST_DEC_SPEC_CV_HANDLER,
  	ZEND_POST_DEC_SPEC_CV_HANDLER,
  	ZEND_POST_DEC_SPEC_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_SPEC_VAR_CONST_HANDLER,
  	ZEND_ASSIGN_SPEC_VAR_TMP_HANDLER,
  	ZEND_ASSIGN_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_SPEC_CV_CONST_HANDLER,
  	ZEND_ASSIGN_SPEC_CV_TMP_HANDLER,
  	ZEND_ASSIGN_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_REF_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_REF_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_REF_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_REF_SPEC_CV_CV_HANDLER,
  	ZEND_ECHO_SPEC_CONST_HANDLER,
  	ZEND_ECHO_SPEC_CONST_HANDLER,
  	ZEND_ECHO_SPEC_CONST_HANDLER,
  	ZEND_ECHO_SPEC_CONST_HANDLER,
  	ZEND_ECHO_SPEC_CONST_HANDLER,
  	ZEND_ECHO_SPEC_TMP_HANDLER,
  	ZEND_ECHO_SPEC_TMP_HANDLER,
  	ZEND_ECHO_SPEC_TMP_HANDLER,
  	ZEND_ECHO_SPEC_TMP_HANDLER,
  	ZEND_ECHO_SPEC_TMP_HANDLER,
  	ZEND_ECHO_SPEC_VAR_HANDLER,
  	ZEND_ECHO_SPEC_VAR_HANDLER,
  	ZEND_ECHO_SPEC_VAR_HANDLER,
  	ZEND_ECHO_SPEC_VAR_HANDLER,
  	ZEND_ECHO_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ECHO_SPEC_CV_HANDLER,
  	ZEND_ECHO_SPEC_CV_HANDLER,
  	ZEND_ECHO_SPEC_CV_HANDLER,
  	ZEND_ECHO_SPEC_CV_HANDLER,
  	ZEND_ECHO_SPEC_CV_HANDLER,
  	ZEND_PRINT_SPEC_CONST_HANDLER,
  	ZEND_PRINT_SPEC_CONST_HANDLER,
  	ZEND_PRINT_SPEC_CONST_HANDLER,
  	ZEND_PRINT_SPEC_CONST_HANDLER,
  	ZEND_PRINT_SPEC_CONST_HANDLER,
  	ZEND_PRINT_SPEC_TMP_HANDLER,
  	ZEND_PRINT_SPEC_TMP_HANDLER,
  	ZEND_PRINT_SPEC_TMP_HANDLER,
  	ZEND_PRINT_SPEC_TMP_HANDLER,
  	ZEND_PRINT_SPEC_TMP_HANDLER,
  	ZEND_PRINT_SPEC_VAR_HANDLER,
  	ZEND_PRINT_SPEC_VAR_HANDLER,
  	ZEND_PRINT_SPEC_VAR_HANDLER,
  	ZEND_PRINT_SPEC_VAR_HANDLER,
  	ZEND_PRINT_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_PRINT_SPEC_CV_HANDLER,
  	ZEND_PRINT_SPEC_CV_HANDLER,
  	ZEND_PRINT_SPEC_CV_HANDLER,
  	ZEND_PRINT_SPEC_CV_HANDLER,
  	ZEND_PRINT_SPEC_CV_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMP_SPEC_HANDLER,
  	ZEND_JMPZ_SPEC_CONST_HANDLER,
  	ZEND_JMPZ_SPEC_CONST_HANDLER,
  	ZEND_JMPZ_SPEC_CONST_HANDLER,
  	ZEND_JMPZ_SPEC_CONST_HANDLER,
  	ZEND_JMPZ_SPEC_CONST_HANDLER,
  	ZEND_JMPZ_SPEC_TMP_HANDLER,
  	ZEND_JMPZ_SPEC_TMP_HANDLER,
  	ZEND_JMPZ_SPEC_TMP_HANDLER,
  	ZEND_JMPZ_SPEC_TMP_HANDLER,
  	ZEND_JMPZ_SPEC_TMP_HANDLER,
  	ZEND_JMPZ_SPEC_VAR_HANDLER,
  	ZEND_JMPZ_SPEC_VAR_HANDLER,
  	ZEND_JMPZ_SPEC_VAR_HANDLER,
  	ZEND_JMPZ_SPEC_VAR_HANDLER,
  	ZEND_JMPZ_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_JMPZ_SPEC_CV_HANDLER,
  	ZEND_JMPZ_SPEC_CV_HANDLER,
  	ZEND_JMPZ_SPEC_CV_HANDLER,
  	ZEND_JMPZ_SPEC_CV_HANDLER,
  	ZEND_JMPZ_SPEC_CV_HANDLER,
  	ZEND_JMPNZ_SPEC_CONST_HANDLER,
  	ZEND_JMPNZ_SPEC_CONST_HANDLER,
  	ZEND_JMPNZ_SPEC_CONST_HANDLER,
  	ZEND_JMPNZ_SPEC_CONST_HANDLER,
  	ZEND_JMPNZ_SPEC_CONST_HANDLER,
  	ZEND_JMPNZ_SPEC_TMP_HANDLER,
  	ZEND_JMPNZ_SPEC_TMP_HANDLER,
  	ZEND_JMPNZ_SPEC_TMP_HANDLER,
  	ZEND_JMPNZ_SPEC_TMP_HANDLER,
  	ZEND_JMPNZ_SPEC_TMP_HANDLER,
  	ZEND_JMPNZ_SPEC_VAR_HANDLER,
  	ZEND_JMPNZ_SPEC_VAR_HANDLER,
  	ZEND_JMPNZ_SPEC_VAR_HANDLER,
  	ZEND_JMPNZ_SPEC_VAR_HANDLER,
  	ZEND_JMPNZ_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_JMPNZ_SPEC_CV_HANDLER,
  	ZEND_JMPNZ_SPEC_CV_HANDLER,
  	ZEND_JMPNZ_SPEC_CV_HANDLER,
  	ZEND_JMPNZ_SPEC_CV_HANDLER,
  	ZEND_JMPNZ_SPEC_CV_HANDLER,
  	ZEND_JMPZNZ_SPEC_CONST_HANDLER,
  	ZEND_JMPZNZ_SPEC_CONST_HANDLER,
  	ZEND_JMPZNZ_SPEC_CONST_HANDLER,
  	ZEND_JMPZNZ_SPEC_CONST_HANDLER,
  	ZEND_JMPZNZ_SPEC_CONST_HANDLER,
  	ZEND_JMPZNZ_SPEC_TMP_HANDLER,
  	ZEND_JMPZNZ_SPEC_TMP_HANDLER,
  	ZEND_JMPZNZ_SPEC_TMP_HANDLER,
  	ZEND_JMPZNZ_SPEC_TMP_HANDLER,
  	ZEND_JMPZNZ_SPEC_TMP_HANDLER,
  	ZEND_JMPZNZ_SPEC_VAR_HANDLER,
  	ZEND_JMPZNZ_SPEC_VAR_HANDLER,
  	ZEND_JMPZNZ_SPEC_VAR_HANDLER,
  	ZEND_JMPZNZ_SPEC_VAR_HANDLER,
  	ZEND_JMPZNZ_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_JMPZNZ_SPEC_CV_HANDLER,
  	ZEND_JMPZNZ_SPEC_CV_HANDLER,
  	ZEND_JMPZNZ_SPEC_CV_HANDLER,
  	ZEND_JMPZNZ_SPEC_CV_HANDLER,
  	ZEND_JMPZNZ_SPEC_CV_HANDLER,
  	ZEND_JMPZ_EX_SPEC_CONST_HANDLER,
  	ZEND_JMPZ_EX_SPEC_CONST_HANDLER,
  	ZEND_JMPZ_EX_SPEC_CONST_HANDLER,
  	ZEND_JMPZ_EX_SPEC_CONST_HANDLER,
  	ZEND_JMPZ_EX_SPEC_CONST_HANDLER,
  	ZEND_JMPZ_EX_SPEC_TMP_HANDLER,
  	ZEND_JMPZ_EX_SPEC_TMP_HANDLER,
  	ZEND_JMPZ_EX_SPEC_TMP_HANDLER,
  	ZEND_JMPZ_EX_SPEC_TMP_HANDLER,
  	ZEND_JMPZ_EX_SPEC_TMP_HANDLER,
  	ZEND_JMPZ_EX_SPEC_VAR_HANDLER,
  	ZEND_JMPZ_EX_SPEC_VAR_HANDLER,
  	ZEND_JMPZ_EX_SPEC_VAR_HANDLER,
  	ZEND_JMPZ_EX_SPEC_VAR_HANDLER,
  	ZEND_JMPZ_EX_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_JMPZ_EX_SPEC_CV_HANDLER,
  	ZEND_JMPZ_EX_SPEC_CV_HANDLER,
  	ZEND_JMPZ_EX_SPEC_CV_HANDLER,
  	ZEND_JMPZ_EX_SPEC_CV_HANDLER,
  	ZEND_JMPZ_EX_SPEC_CV_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_CONST_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_CONST_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_CONST_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_CONST_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_CONST_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_TMP_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_TMP_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_TMP_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_TMP_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_TMP_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_VAR_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_VAR_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_VAR_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_VAR_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_CV_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_CV_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_CV_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_CV_HANDLER,
  	ZEND_JMPNZ_EX_SPEC_CV_HANDLER,
  	ZEND_CASE_SPEC_CONST_CONST_HANDLER,
  	ZEND_CASE_SPEC_CONST_TMP_HANDLER,
  	ZEND_CASE_SPEC_CONST_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_CASE_SPEC_CONST_CV_HANDLER,
  	ZEND_CASE_SPEC_TMP_CONST_HANDLER,
  	ZEND_CASE_SPEC_TMP_TMP_HANDLER,
  	ZEND_CASE_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_CASE_SPEC_TMP_CV_HANDLER,
  	ZEND_CASE_SPEC_VAR_CONST_HANDLER,
  	ZEND_CASE_SPEC_VAR_TMP_HANDLER,
  	ZEND_CASE_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_CASE_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_CASE_SPEC_CV_CONST_HANDLER,
  	ZEND_CASE_SPEC_CV_TMP_HANDLER,
  	ZEND_CASE_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_CASE_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SWITCH_FREE_SPEC_TMP_HANDLER,
  	ZEND_SWITCH_FREE_SPEC_TMP_HANDLER,
  	ZEND_SWITCH_FREE_SPEC_TMP_HANDLER,
  	ZEND_SWITCH_FREE_SPEC_TMP_HANDLER,
  	ZEND_SWITCH_FREE_SPEC_TMP_HANDLER,
  	ZEND_SWITCH_FREE_SPEC_VAR_HANDLER,
  	ZEND_SWITCH_FREE_SPEC_VAR_HANDLER,
  	ZEND_SWITCH_FREE_SPEC_VAR_HANDLER,
  	ZEND_SWITCH_FREE_SPEC_VAR_HANDLER,
  	ZEND_SWITCH_FREE_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BRK_SPEC_CONST_HANDLER,
  	ZEND_BRK_SPEC_TMP_HANDLER,
  	ZEND_BRK_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BRK_SPEC_CV_HANDLER,
  	ZEND_BRK_SPEC_CONST_HANDLER,
  	ZEND_BRK_SPEC_TMP_HANDLER,
  	ZEND_BRK_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BRK_SPEC_CV_HANDLER,
  	ZEND_BRK_SPEC_CONST_HANDLER,
  	ZEND_BRK_SPEC_TMP_HANDLER,
  	ZEND_BRK_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BRK_SPEC_CV_HANDLER,
  	ZEND_BRK_SPEC_CONST_HANDLER,
  	ZEND_BRK_SPEC_TMP_HANDLER,
  	ZEND_BRK_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BRK_SPEC_CV_HANDLER,
  	ZEND_BRK_SPEC_CONST_HANDLER,
  	ZEND_BRK_SPEC_TMP_HANDLER,
  	ZEND_BRK_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BRK_SPEC_CV_HANDLER,
  	ZEND_CONT_SPEC_CONST_HANDLER,
  	ZEND_CONT_SPEC_TMP_HANDLER,
  	ZEND_CONT_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_CONT_SPEC_CV_HANDLER,
  	ZEND_CONT_SPEC_CONST_HANDLER,
  	ZEND_CONT_SPEC_TMP_HANDLER,
  	ZEND_CONT_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_CONT_SPEC_CV_HANDLER,
  	ZEND_CONT_SPEC_CONST_HANDLER,
  	ZEND_CONT_SPEC_TMP_HANDLER,
  	ZEND_CONT_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_CONT_SPEC_CV_HANDLER,
  	ZEND_CONT_SPEC_CONST_HANDLER,
  	ZEND_CONT_SPEC_TMP_HANDLER,
  	ZEND_CONT_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_CONT_SPEC_CV_HANDLER,
  	ZEND_CONT_SPEC_CONST_HANDLER,
  	ZEND_CONT_SPEC_TMP_HANDLER,
  	ZEND_CONT_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_CONT_SPEC_CV_HANDLER,
  	ZEND_BOOL_SPEC_CONST_HANDLER,
  	ZEND_BOOL_SPEC_CONST_HANDLER,
  	ZEND_BOOL_SPEC_CONST_HANDLER,
  	ZEND_BOOL_SPEC_CONST_HANDLER,
  	ZEND_BOOL_SPEC_CONST_HANDLER,
  	ZEND_BOOL_SPEC_TMP_HANDLER,
  	ZEND_BOOL_SPEC_TMP_HANDLER,
  	ZEND_BOOL_SPEC_TMP_HANDLER,
  	ZEND_BOOL_SPEC_TMP_HANDLER,
  	ZEND_BOOL_SPEC_TMP_HANDLER,
  	ZEND_BOOL_SPEC_VAR_HANDLER,
  	ZEND_BOOL_SPEC_VAR_HANDLER,
  	ZEND_BOOL_SPEC_VAR_HANDLER,
  	ZEND_BOOL_SPEC_VAR_HANDLER,
  	ZEND_BOOL_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BOOL_SPEC_CV_HANDLER,
  	ZEND_BOOL_SPEC_CV_HANDLER,
  	ZEND_BOOL_SPEC_CV_HANDLER,
  	ZEND_BOOL_SPEC_CV_HANDLER,
  	ZEND_BOOL_SPEC_CV_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_INIT_STRING_SPEC_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ADD_CHAR_SPEC_TMP_CONST_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ADD_STRING_SPEC_TMP_CONST_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ADD_VAR_SPEC_TMP_TMP_HANDLER,
  	ZEND_ADD_VAR_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ADD_VAR_SPEC_TMP_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_BEGIN_SILENCE_SPEC_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_END_SILENCE_SPEC_TMP_HANDLER,
  	ZEND_END_SILENCE_SPEC_TMP_HANDLER,
  	ZEND_END_SILENCE_SPEC_TMP_HANDLER,
  	ZEND_END_SILENCE_SPEC_TMP_HANDLER,
  	ZEND_END_SILENCE_SPEC_TMP_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_CONST_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_TMP_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_CV_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_CONST_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_TMP_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_CV_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_CONST_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_TMP_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_CV_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_CONST_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_TMP_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_CV_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_CONST_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_TMP_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_INIT_FCALL_BY_NAME_SPEC_CV_HANDLER,
  	ZEND_DO_FCALL_SPEC_CONST_HANDLER,
  	ZEND_DO_FCALL_SPEC_CONST_HANDLER,
  	ZEND_DO_FCALL_SPEC_CONST_HANDLER,
  	ZEND_DO_FCALL_SPEC_CONST_HANDLER,
  	ZEND_DO_FCALL_SPEC_CONST_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_DO_FCALL_BY_NAME_SPEC_HANDLER,
  	ZEND_RETURN_SPEC_CONST_HANDLER,
  	ZEND_RETURN_SPEC_CONST_HANDLER,
  	ZEND_RETURN_SPEC_CONST_HANDLER,
  	ZEND_RETURN_SPEC_CONST_HANDLER,
  	ZEND_RETURN_SPEC_CONST_HANDLER,
  	ZEND_RETURN_SPEC_TMP_HANDLER,
  	ZEND_RETURN_SPEC_TMP_HANDLER,
  	ZEND_RETURN_SPEC_TMP_HANDLER,
  	ZEND_RETURN_SPEC_TMP_HANDLER,
  	ZEND_RETURN_SPEC_TMP_HANDLER,
  	ZEND_RETURN_SPEC_VAR_HANDLER,
  	ZEND_RETURN_SPEC_VAR_HANDLER,
  	ZEND_RETURN_SPEC_VAR_HANDLER,
  	ZEND_RETURN_SPEC_VAR_HANDLER,
  	ZEND_RETURN_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_RETURN_SPEC_CV_HANDLER,
  	ZEND_RETURN_SPEC_CV_HANDLER,
  	ZEND_RETURN_SPEC_CV_HANDLER,
  	ZEND_RETURN_SPEC_CV_HANDLER,
  	ZEND_RETURN_SPEC_CV_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_SPEC_HANDLER,
  	ZEND_RECV_INIT_SPEC_CONST_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_RECV_INIT_SPEC_CONST_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_RECV_INIT_SPEC_CONST_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_RECV_INIT_SPEC_CONST_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_RECV_INIT_SPEC_CONST_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SEND_VAL_SPEC_CONST_HANDLER,
  	ZEND_SEND_VAL_SPEC_CONST_HANDLER,
  	ZEND_SEND_VAL_SPEC_CONST_HANDLER,
  	ZEND_SEND_VAL_SPEC_CONST_HANDLER,
  	ZEND_SEND_VAL_SPEC_CONST_HANDLER,
  	ZEND_SEND_VAL_SPEC_TMP_HANDLER,
  	ZEND_SEND_VAL_SPEC_TMP_HANDLER,
  	ZEND_SEND_VAL_SPEC_TMP_HANDLER,
  	ZEND_SEND_VAL_SPEC_TMP_HANDLER,
  	ZEND_SEND_VAL_SPEC_TMP_HANDLER,
  	ZEND_SEND_VAL_SPEC_VAR_HANDLER,
  	ZEND_SEND_VAL_SPEC_VAR_HANDLER,
  	ZEND_SEND_VAL_SPEC_VAR_HANDLER,
  	ZEND_SEND_VAL_SPEC_VAR_HANDLER,
  	ZEND_SEND_VAL_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SEND_VAL_SPEC_CV_HANDLER,
  	ZEND_SEND_VAL_SPEC_CV_HANDLER,
  	ZEND_SEND_VAL_SPEC_CV_HANDLER,
  	ZEND_SEND_VAL_SPEC_CV_HANDLER,
  	ZEND_SEND_VAL_SPEC_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SEND_VAR_SPEC_VAR_HANDLER,
  	ZEND_SEND_VAR_SPEC_VAR_HANDLER,
  	ZEND_SEND_VAR_SPEC_VAR_HANDLER,
  	ZEND_SEND_VAR_SPEC_VAR_HANDLER,
  	ZEND_SEND_VAR_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SEND_VAR_SPEC_CV_HANDLER,
  	ZEND_SEND_VAR_SPEC_CV_HANDLER,
  	ZEND_SEND_VAR_SPEC_CV_HANDLER,
  	ZEND_SEND_VAR_SPEC_CV_HANDLER,
  	ZEND_SEND_VAR_SPEC_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SEND_REF_SPEC_VAR_HANDLER,
  	ZEND_SEND_REF_SPEC_VAR_HANDLER,
  	ZEND_SEND_REF_SPEC_VAR_HANDLER,
  	ZEND_SEND_REF_SPEC_VAR_HANDLER,
  	ZEND_SEND_REF_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SEND_REF_SPEC_CV_HANDLER,
  	ZEND_SEND_REF_SPEC_CV_HANDLER,
  	ZEND_SEND_REF_SPEC_CV_HANDLER,
  	ZEND_SEND_REF_SPEC_CV_HANDLER,
  	ZEND_SEND_REF_SPEC_CV_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NEW_SPEC_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FREE_SPEC_TMP_HANDLER,
  	ZEND_FREE_SPEC_TMP_HANDLER,
  	ZEND_FREE_SPEC_TMP_HANDLER,
  	ZEND_FREE_SPEC_TMP_HANDLER,
  	ZEND_FREE_SPEC_TMP_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_CONST_CONST_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_CONST_TMP_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_CONST_VAR_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_CONST_UNUSED_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_CONST_CV_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_TMP_CONST_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_TMP_TMP_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_TMP_VAR_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_TMP_UNUSED_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_TMP_CV_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_VAR_CONST_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_VAR_TMP_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_VAR_VAR_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_VAR_UNUSED_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_VAR_CV_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_UNUSED_UNUSED_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_UNUSED_CV_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_CV_CONST_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_CV_TMP_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_CV_VAR_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_CV_UNUSED_HANDLER,
  	ZEND_INIT_ARRAY_SPEC_CV_CV_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_CONST_CONST_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_CONST_TMP_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_CONST_VAR_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_CONST_UNUSED_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_CONST_CV_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_TMP_CONST_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_TMP_TMP_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_TMP_VAR_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_TMP_UNUSED_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_TMP_CV_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_VAR_CONST_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_VAR_TMP_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_VAR_VAR_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_VAR_UNUSED_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_CV_CONST_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_CV_TMP_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_CV_VAR_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_CV_UNUSED_HANDLER,
  	ZEND_ADD_ARRAY_ELEMENT_SPEC_CV_CV_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_CONST_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_CONST_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_CONST_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_CONST_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_CONST_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_TMP_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_TMP_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_TMP_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_TMP_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_TMP_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_VAR_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_VAR_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_VAR_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_VAR_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_CV_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_CV_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_CV_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_CV_HANDLER,
  	ZEND_INCLUDE_OR_EVAL_SPEC_CV_HANDLER,
  	ZEND_UNSET_VAR_SPEC_CONST_HANDLER,
  	ZEND_UNSET_VAR_SPEC_CONST_HANDLER,
  	ZEND_UNSET_VAR_SPEC_CONST_HANDLER,
  	ZEND_UNSET_VAR_SPEC_CONST_HANDLER,
  	ZEND_UNSET_VAR_SPEC_CONST_HANDLER,
  	ZEND_UNSET_VAR_SPEC_TMP_HANDLER,
  	ZEND_UNSET_VAR_SPEC_TMP_HANDLER,
  	ZEND_UNSET_VAR_SPEC_TMP_HANDLER,
  	ZEND_UNSET_VAR_SPEC_TMP_HANDLER,
  	ZEND_UNSET_VAR_SPEC_TMP_HANDLER,
  	ZEND_UNSET_VAR_SPEC_VAR_HANDLER,
  	ZEND_UNSET_VAR_SPEC_VAR_HANDLER,
  	ZEND_UNSET_VAR_SPEC_VAR_HANDLER,
  	ZEND_UNSET_VAR_SPEC_VAR_HANDLER,
  	ZEND_UNSET_VAR_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_UNSET_VAR_SPEC_CV_HANDLER,
  	ZEND_UNSET_VAR_SPEC_CV_HANDLER,
  	ZEND_UNSET_VAR_SPEC_CV_HANDLER,
  	ZEND_UNSET_VAR_SPEC_CV_HANDLER,
  	ZEND_UNSET_VAR_SPEC_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_UNSET_DIM_SPEC_VAR_CONST_HANDLER,
  	ZEND_UNSET_DIM_SPEC_VAR_TMP_HANDLER,
  	ZEND_UNSET_DIM_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_UNSET_DIM_SPEC_VAR_CV_HANDLER,
  	ZEND_UNSET_DIM_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_UNSET_DIM_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_UNSET_DIM_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_UNSET_DIM_SPEC_UNUSED_CV_HANDLER,
  	ZEND_UNSET_DIM_SPEC_CV_CONST_HANDLER,
  	ZEND_UNSET_DIM_SPEC_CV_TMP_HANDLER,
  	ZEND_UNSET_DIM_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_UNSET_DIM_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_UNSET_OBJ_SPEC_VAR_CONST_HANDLER,
  	ZEND_UNSET_OBJ_SPEC_VAR_TMP_HANDLER,
  	ZEND_UNSET_OBJ_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_UNSET_OBJ_SPEC_VAR_CV_HANDLER,
  	ZEND_UNSET_OBJ_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_UNSET_OBJ_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_UNSET_OBJ_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_UNSET_OBJ_SPEC_UNUSED_CV_HANDLER,
  	ZEND_UNSET_OBJ_SPEC_CV_CONST_HANDLER,
  	ZEND_UNSET_OBJ_SPEC_CV_TMP_HANDLER,
  	ZEND_UNSET_OBJ_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_UNSET_OBJ_SPEC_CV_CV_HANDLER,
  	ZEND_FE_RESET_SPEC_CONST_HANDLER,
  	ZEND_FE_RESET_SPEC_CONST_HANDLER,
  	ZEND_FE_RESET_SPEC_CONST_HANDLER,
  	ZEND_FE_RESET_SPEC_CONST_HANDLER,
  	ZEND_FE_RESET_SPEC_CONST_HANDLER,
  	ZEND_FE_RESET_SPEC_TMP_HANDLER,
  	ZEND_FE_RESET_SPEC_TMP_HANDLER,
  	ZEND_FE_RESET_SPEC_TMP_HANDLER,
  	ZEND_FE_RESET_SPEC_TMP_HANDLER,
  	ZEND_FE_RESET_SPEC_TMP_HANDLER,
  	ZEND_FE_RESET_SPEC_VAR_HANDLER,
  	ZEND_FE_RESET_SPEC_VAR_HANDLER,
  	ZEND_FE_RESET_SPEC_VAR_HANDLER,
  	ZEND_FE_RESET_SPEC_VAR_HANDLER,
  	ZEND_FE_RESET_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FE_RESET_SPEC_CV_HANDLER,
  	ZEND_FE_RESET_SPEC_CV_HANDLER,
  	ZEND_FE_RESET_SPEC_CV_HANDLER,
  	ZEND_FE_RESET_SPEC_CV_HANDLER,
  	ZEND_FE_RESET_SPEC_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FE_FETCH_SPEC_VAR_HANDLER,
  	ZEND_FE_FETCH_SPEC_VAR_HANDLER,
  	ZEND_FE_FETCH_SPEC_VAR_HANDLER,
  	ZEND_FE_FETCH_SPEC_VAR_HANDLER,
  	ZEND_FE_FETCH_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_EXIT_SPEC_CONST_HANDLER,
  	ZEND_EXIT_SPEC_CONST_HANDLER,
  	ZEND_EXIT_SPEC_CONST_HANDLER,
  	ZEND_EXIT_SPEC_CONST_HANDLER,
  	ZEND_EXIT_SPEC_CONST_HANDLER,
  	ZEND_EXIT_SPEC_TMP_HANDLER,
  	ZEND_EXIT_SPEC_TMP_HANDLER,
  	ZEND_EXIT_SPEC_TMP_HANDLER,
  	ZEND_EXIT_SPEC_TMP_HANDLER,
  	ZEND_EXIT_SPEC_TMP_HANDLER,
  	ZEND_EXIT_SPEC_VAR_HANDLER,
  	ZEND_EXIT_SPEC_VAR_HANDLER,
  	ZEND_EXIT_SPEC_VAR_HANDLER,
  	ZEND_EXIT_SPEC_VAR_HANDLER,
  	ZEND_EXIT_SPEC_VAR_HANDLER,
  	ZEND_EXIT_SPEC_UNUSED_HANDLER,
  	ZEND_EXIT_SPEC_UNUSED_HANDLER,
  	ZEND_EXIT_SPEC_UNUSED_HANDLER,
  	ZEND_EXIT_SPEC_UNUSED_HANDLER,
  	ZEND_EXIT_SPEC_UNUSED_HANDLER,
  	ZEND_EXIT_SPEC_CV_HANDLER,
  	ZEND_EXIT_SPEC_CV_HANDLER,
  	ZEND_EXIT_SPEC_CV_HANDLER,
  	ZEND_EXIT_SPEC_CV_HANDLER,
  	ZEND_EXIT_SPEC_CV_HANDLER,
  	ZEND_FETCH_R_SPEC_CONST_HANDLER,
  	ZEND_FETCH_R_SPEC_CONST_HANDLER,
  	ZEND_FETCH_R_SPEC_CONST_HANDLER,
  	ZEND_FETCH_R_SPEC_CONST_HANDLER,
  	ZEND_FETCH_R_SPEC_CONST_HANDLER,
  	ZEND_FETCH_R_SPEC_TMP_HANDLER,
  	ZEND_FETCH_R_SPEC_TMP_HANDLER,
  	ZEND_FETCH_R_SPEC_TMP_HANDLER,
  	ZEND_FETCH_R_SPEC_TMP_HANDLER,
  	ZEND_FETCH_R_SPEC_TMP_HANDLER,
  	ZEND_FETCH_R_SPEC_VAR_HANDLER,
  	ZEND_FETCH_R_SPEC_VAR_HANDLER,
  	ZEND_FETCH_R_SPEC_VAR_HANDLER,
  	ZEND_FETCH_R_SPEC_VAR_HANDLER,
  	ZEND_FETCH_R_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_R_SPEC_CV_HANDLER,
  	ZEND_FETCH_R_SPEC_CV_HANDLER,
  	ZEND_FETCH_R_SPEC_CV_HANDLER,
  	ZEND_FETCH_R_SPEC_CV_HANDLER,
  	ZEND_FETCH_R_SPEC_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_R_SPEC_VAR_CONST_HANDLER,
  	ZEND_FETCH_DIM_R_SPEC_VAR_TMP_HANDLER,
  	ZEND_FETCH_DIM_R_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_R_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_R_SPEC_CV_CONST_HANDLER,
  	ZEND_FETCH_DIM_R_SPEC_CV_TMP_HANDLER,
  	ZEND_FETCH_DIM_R_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_R_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_R_SPEC_VAR_CONST_HANDLER,
  	ZEND_FETCH_OBJ_R_SPEC_VAR_TMP_HANDLER,
  	ZEND_FETCH_OBJ_R_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_R_SPEC_VAR_CV_HANDLER,
  	ZEND_FETCH_OBJ_R_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_FETCH_OBJ_R_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_FETCH_OBJ_R_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_R_SPEC_UNUSED_CV_HANDLER,
  	ZEND_FETCH_OBJ_R_SPEC_CV_CONST_HANDLER,
  	ZEND_FETCH_OBJ_R_SPEC_CV_TMP_HANDLER,
  	ZEND_FETCH_OBJ_R_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_R_SPEC_CV_CV_HANDLER,
  	ZEND_FETCH_W_SPEC_CONST_HANDLER,
  	ZEND_FETCH_W_SPEC_CONST_HANDLER,
  	ZEND_FETCH_W_SPEC_CONST_HANDLER,
  	ZEND_FETCH_W_SPEC_CONST_HANDLER,
  	ZEND_FETCH_W_SPEC_CONST_HANDLER,
  	ZEND_FETCH_W_SPEC_TMP_HANDLER,
  	ZEND_FETCH_W_SPEC_TMP_HANDLER,
  	ZEND_FETCH_W_SPEC_TMP_HANDLER,
  	ZEND_FETCH_W_SPEC_TMP_HANDLER,
  	ZEND_FETCH_W_SPEC_TMP_HANDLER,
  	ZEND_FETCH_W_SPEC_VAR_HANDLER,
  	ZEND_FETCH_W_SPEC_VAR_HANDLER,
  	ZEND_FETCH_W_SPEC_VAR_HANDLER,
  	ZEND_FETCH_W_SPEC_VAR_HANDLER,
  	ZEND_FETCH_W_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_W_SPEC_CV_HANDLER,
  	ZEND_FETCH_W_SPEC_CV_HANDLER,
  	ZEND_FETCH_W_SPEC_CV_HANDLER,
  	ZEND_FETCH_W_SPEC_CV_HANDLER,
  	ZEND_FETCH_W_SPEC_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_W_SPEC_VAR_CONST_HANDLER,
  	ZEND_FETCH_DIM_W_SPEC_VAR_TMP_HANDLER,
  	ZEND_FETCH_DIM_W_SPEC_VAR_VAR_HANDLER,
  	ZEND_FETCH_DIM_W_SPEC_VAR_UNUSED_HANDLER,
  	ZEND_FETCH_DIM_W_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_W_SPEC_CV_CONST_HANDLER,
  	ZEND_FETCH_DIM_W_SPEC_CV_TMP_HANDLER,
  	ZEND_FETCH_DIM_W_SPEC_CV_VAR_HANDLER,
  	ZEND_FETCH_DIM_W_SPEC_CV_UNUSED_HANDLER,
  	ZEND_FETCH_DIM_W_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_W_SPEC_VAR_CONST_HANDLER,
  	ZEND_FETCH_OBJ_W_SPEC_VAR_TMP_HANDLER,
  	ZEND_FETCH_OBJ_W_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_W_SPEC_VAR_CV_HANDLER,
  	ZEND_FETCH_OBJ_W_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_FETCH_OBJ_W_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_FETCH_OBJ_W_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_W_SPEC_UNUSED_CV_HANDLER,
  	ZEND_FETCH_OBJ_W_SPEC_CV_CONST_HANDLER,
  	ZEND_FETCH_OBJ_W_SPEC_CV_TMP_HANDLER,
  	ZEND_FETCH_OBJ_W_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_W_SPEC_CV_CV_HANDLER,
  	ZEND_FETCH_RW_SPEC_CONST_HANDLER,
  	ZEND_FETCH_RW_SPEC_CONST_HANDLER,
  	ZEND_FETCH_RW_SPEC_CONST_HANDLER,
  	ZEND_FETCH_RW_SPEC_CONST_HANDLER,
  	ZEND_FETCH_RW_SPEC_CONST_HANDLER,
  	ZEND_FETCH_RW_SPEC_TMP_HANDLER,
  	ZEND_FETCH_RW_SPEC_TMP_HANDLER,
  	ZEND_FETCH_RW_SPEC_TMP_HANDLER,
  	ZEND_FETCH_RW_SPEC_TMP_HANDLER,
  	ZEND_FETCH_RW_SPEC_TMP_HANDLER,
  	ZEND_FETCH_RW_SPEC_VAR_HANDLER,
  	ZEND_FETCH_RW_SPEC_VAR_HANDLER,
  	ZEND_FETCH_RW_SPEC_VAR_HANDLER,
  	ZEND_FETCH_RW_SPEC_VAR_HANDLER,
  	ZEND_FETCH_RW_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_RW_SPEC_CV_HANDLER,
  	ZEND_FETCH_RW_SPEC_CV_HANDLER,
  	ZEND_FETCH_RW_SPEC_CV_HANDLER,
  	ZEND_FETCH_RW_SPEC_CV_HANDLER,
  	ZEND_FETCH_RW_SPEC_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_RW_SPEC_VAR_CONST_HANDLER,
  	ZEND_FETCH_DIM_RW_SPEC_VAR_TMP_HANDLER,
  	ZEND_FETCH_DIM_RW_SPEC_VAR_VAR_HANDLER,
  	ZEND_FETCH_DIM_RW_SPEC_VAR_UNUSED_HANDLER,
  	ZEND_FETCH_DIM_RW_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_RW_SPEC_CV_CONST_HANDLER,
  	ZEND_FETCH_DIM_RW_SPEC_CV_TMP_HANDLER,
  	ZEND_FETCH_DIM_RW_SPEC_CV_VAR_HANDLER,
  	ZEND_FETCH_DIM_RW_SPEC_CV_UNUSED_HANDLER,
  	ZEND_FETCH_DIM_RW_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_RW_SPEC_VAR_CONST_HANDLER,
  	ZEND_FETCH_OBJ_RW_SPEC_VAR_TMP_HANDLER,
  	ZEND_FETCH_OBJ_RW_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_RW_SPEC_VAR_CV_HANDLER,
  	ZEND_FETCH_OBJ_RW_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_FETCH_OBJ_RW_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_FETCH_OBJ_RW_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_RW_SPEC_UNUSED_CV_HANDLER,
  	ZEND_FETCH_OBJ_RW_SPEC_CV_CONST_HANDLER,
  	ZEND_FETCH_OBJ_RW_SPEC_CV_TMP_HANDLER,
  	ZEND_FETCH_OBJ_RW_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_RW_SPEC_CV_CV_HANDLER,
  	ZEND_FETCH_IS_SPEC_CONST_HANDLER,
  	ZEND_FETCH_IS_SPEC_CONST_HANDLER,
  	ZEND_FETCH_IS_SPEC_CONST_HANDLER,
  	ZEND_FETCH_IS_SPEC_CONST_HANDLER,
  	ZEND_FETCH_IS_SPEC_CONST_HANDLER,
  	ZEND_FETCH_IS_SPEC_TMP_HANDLER,
  	ZEND_FETCH_IS_SPEC_TMP_HANDLER,
  	ZEND_FETCH_IS_SPEC_TMP_HANDLER,
  	ZEND_FETCH_IS_SPEC_TMP_HANDLER,
  	ZEND_FETCH_IS_SPEC_TMP_HANDLER,
  	ZEND_FETCH_IS_SPEC_VAR_HANDLER,
  	ZEND_FETCH_IS_SPEC_VAR_HANDLER,
  	ZEND_FETCH_IS_SPEC_VAR_HANDLER,
  	ZEND_FETCH_IS_SPEC_VAR_HANDLER,
  	ZEND_FETCH_IS_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_IS_SPEC_CV_HANDLER,
  	ZEND_FETCH_IS_SPEC_CV_HANDLER,
  	ZEND_FETCH_IS_SPEC_CV_HANDLER,
  	ZEND_FETCH_IS_SPEC_CV_HANDLER,
  	ZEND_FETCH_IS_SPEC_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_IS_SPEC_VAR_CONST_HANDLER,
  	ZEND_FETCH_DIM_IS_SPEC_VAR_TMP_HANDLER,
  	ZEND_FETCH_DIM_IS_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_IS_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_IS_SPEC_CV_CONST_HANDLER,
  	ZEND_FETCH_DIM_IS_SPEC_CV_TMP_HANDLER,
  	ZEND_FETCH_DIM_IS_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_IS_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_IS_SPEC_VAR_CONST_HANDLER,
  	ZEND_FETCH_OBJ_IS_SPEC_VAR_TMP_HANDLER,
  	ZEND_FETCH_OBJ_IS_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_IS_SPEC_VAR_CV_HANDLER,
  	ZEND_FETCH_OBJ_IS_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_FETCH_OBJ_IS_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_FETCH_OBJ_IS_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_IS_SPEC_UNUSED_CV_HANDLER,
  	ZEND_FETCH_OBJ_IS_SPEC_CV_CONST_HANDLER,
  	ZEND_FETCH_OBJ_IS_SPEC_CV_TMP_HANDLER,
  	ZEND_FETCH_OBJ_IS_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_IS_SPEC_CV_CV_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_CONST_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_CONST_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_CONST_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_CONST_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_CONST_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_TMP_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_TMP_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_TMP_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_TMP_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_TMP_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_VAR_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_VAR_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_VAR_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_VAR_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_CV_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_CV_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_CV_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_CV_HANDLER,
  	ZEND_FETCH_FUNC_ARG_SPEC_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_FUNC_ARG_SPEC_VAR_CONST_HANDLER,
  	ZEND_FETCH_DIM_FUNC_ARG_SPEC_VAR_TMP_HANDLER,
  	ZEND_FETCH_DIM_FUNC_ARG_SPEC_VAR_VAR_HANDLER,
  	ZEND_FETCH_DIM_FUNC_ARG_SPEC_VAR_UNUSED_HANDLER,
  	ZEND_FETCH_DIM_FUNC_ARG_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_FUNC_ARG_SPEC_CV_CONST_HANDLER,
  	ZEND_FETCH_DIM_FUNC_ARG_SPEC_CV_TMP_HANDLER,
  	ZEND_FETCH_DIM_FUNC_ARG_SPEC_CV_VAR_HANDLER,
  	ZEND_FETCH_DIM_FUNC_ARG_SPEC_CV_UNUSED_HANDLER,
  	ZEND_FETCH_DIM_FUNC_ARG_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_FUNC_ARG_SPEC_VAR_CONST_HANDLER,
  	ZEND_FETCH_OBJ_FUNC_ARG_SPEC_VAR_TMP_HANDLER,
  	ZEND_FETCH_OBJ_FUNC_ARG_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_FUNC_ARG_SPEC_VAR_CV_HANDLER,
  	ZEND_FETCH_OBJ_FUNC_ARG_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_FETCH_OBJ_FUNC_ARG_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_FETCH_OBJ_FUNC_ARG_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_FUNC_ARG_SPEC_UNUSED_CV_HANDLER,
  	ZEND_FETCH_OBJ_FUNC_ARG_SPEC_CV_CONST_HANDLER,
  	ZEND_FETCH_OBJ_FUNC_ARG_SPEC_CV_TMP_HANDLER,
  	ZEND_FETCH_OBJ_FUNC_ARG_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_FUNC_ARG_SPEC_CV_CV_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_CONST_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_CONST_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_CONST_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_CONST_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_CONST_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_TMP_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_TMP_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_TMP_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_TMP_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_TMP_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_VAR_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_VAR_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_VAR_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_VAR_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_CV_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_CV_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_CV_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_CV_HANDLER,
  	ZEND_FETCH_UNSET_SPEC_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_UNSET_SPEC_VAR_CONST_HANDLER,
  	ZEND_FETCH_DIM_UNSET_SPEC_VAR_TMP_HANDLER,
  	ZEND_FETCH_DIM_UNSET_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_UNSET_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_UNSET_SPEC_CV_CONST_HANDLER,
  	ZEND_FETCH_DIM_UNSET_SPEC_CV_TMP_HANDLER,
  	ZEND_FETCH_DIM_UNSET_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_UNSET_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_UNSET_SPEC_VAR_CONST_HANDLER,
  	ZEND_FETCH_OBJ_UNSET_SPEC_VAR_TMP_HANDLER,
  	ZEND_FETCH_OBJ_UNSET_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_UNSET_SPEC_VAR_CV_HANDLER,
  	ZEND_FETCH_OBJ_UNSET_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_FETCH_OBJ_UNSET_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_FETCH_OBJ_UNSET_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_UNSET_SPEC_UNUSED_CV_HANDLER,
  	ZEND_FETCH_OBJ_UNSET_SPEC_CV_CONST_HANDLER,
  	ZEND_FETCH_OBJ_UNSET_SPEC_CV_TMP_HANDLER,
  	ZEND_FETCH_OBJ_UNSET_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_OBJ_UNSET_SPEC_CV_CV_HANDLER,
  	ZEND_FETCH_DIM_TMP_VAR_SPEC_CONST_CONST_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_DIM_TMP_VAR_SPEC_TMP_CONST_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_CONSTANT_SPEC_CONST_CONST_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_FETCH_CONSTANT_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_STMT_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_BEGIN_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_FCALL_END_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_EXT_NOP_SPEC_HANDLER,
  	ZEND_TICKS_SPEC_CONST_HANDLER,
  	ZEND_TICKS_SPEC_CONST_HANDLER,
  	ZEND_TICKS_SPEC_CONST_HANDLER,
  	ZEND_TICKS_SPEC_CONST_HANDLER,
  	ZEND_TICKS_SPEC_CONST_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SEND_VAR_NO_REF_SPEC_VAR_HANDLER,
  	ZEND_SEND_VAR_NO_REF_SPEC_VAR_HANDLER,
  	ZEND_SEND_VAR_NO_REF_SPEC_VAR_HANDLER,
  	ZEND_SEND_VAR_NO_REF_SPEC_VAR_HANDLER,
  	ZEND_SEND_VAR_NO_REF_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_SEND_VAR_NO_REF_SPEC_CV_HANDLER,
  	ZEND_SEND_VAR_NO_REF_SPEC_CV_HANDLER,
  	ZEND_SEND_VAR_NO_REF_SPEC_CV_HANDLER,
  	ZEND_SEND_VAR_NO_REF_SPEC_CV_HANDLER,
  	ZEND_SEND_VAR_NO_REF_SPEC_CV_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_CATCH_SPEC_HANDLER,
  	ZEND_THROW_SPEC_CONST_HANDLER,
  	ZEND_THROW_SPEC_CONST_HANDLER,
  	ZEND_THROW_SPEC_CONST_HANDLER,
  	ZEND_THROW_SPEC_CONST_HANDLER,
  	ZEND_THROW_SPEC_CONST_HANDLER,
  	ZEND_THROW_SPEC_TMP_HANDLER,
  	ZEND_THROW_SPEC_TMP_HANDLER,
  	ZEND_THROW_SPEC_TMP_HANDLER,
  	ZEND_THROW_SPEC_TMP_HANDLER,
  	ZEND_THROW_SPEC_TMP_HANDLER,
  	ZEND_THROW_SPEC_VAR_HANDLER,
  	ZEND_THROW_SPEC_VAR_HANDLER,
  	ZEND_THROW_SPEC_VAR_HANDLER,
  	ZEND_THROW_SPEC_VAR_HANDLER,
  	ZEND_THROW_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_THROW_SPEC_CV_HANDLER,
  	ZEND_THROW_SPEC_CV_HANDLER,
  	ZEND_THROW_SPEC_CV_HANDLER,
  	ZEND_THROW_SPEC_CV_HANDLER,
  	ZEND_THROW_SPEC_CV_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_CONST_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_TMP_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_VAR_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_UNUSED_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_CV_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_CONST_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_TMP_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_VAR_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_UNUSED_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_CV_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_CONST_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_TMP_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_VAR_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_UNUSED_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_CV_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_CONST_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_TMP_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_VAR_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_UNUSED_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_CV_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_CONST_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_TMP_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_VAR_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_UNUSED_HANDLER,
  	ZEND_FETCH_CLASS_SPEC_CV_HANDLER,
  	ZEND_CLONE_SPEC_CONST_HANDLER,
  	ZEND_CLONE_SPEC_CONST_HANDLER,
  	ZEND_CLONE_SPEC_CONST_HANDLER,
  	ZEND_CLONE_SPEC_CONST_HANDLER,
  	ZEND_CLONE_SPEC_CONST_HANDLER,
  	ZEND_CLONE_SPEC_TMP_HANDLER,
  	ZEND_CLONE_SPEC_TMP_HANDLER,
  	ZEND_CLONE_SPEC_TMP_HANDLER,
  	ZEND_CLONE_SPEC_TMP_HANDLER,
  	ZEND_CLONE_SPEC_TMP_HANDLER,
  	ZEND_CLONE_SPEC_VAR_HANDLER,
  	ZEND_CLONE_SPEC_VAR_HANDLER,
  	ZEND_CLONE_SPEC_VAR_HANDLER,
  	ZEND_CLONE_SPEC_VAR_HANDLER,
  	ZEND_CLONE_SPEC_VAR_HANDLER,
  	ZEND_CLONE_SPEC_UNUSED_HANDLER,
  	ZEND_CLONE_SPEC_UNUSED_HANDLER,
  	ZEND_CLONE_SPEC_UNUSED_HANDLER,
  	ZEND_CLONE_SPEC_UNUSED_HANDLER,
  	ZEND_CLONE_SPEC_UNUSED_HANDLER,
  	ZEND_CLONE_SPEC_CV_HANDLER,
  	ZEND_CLONE_SPEC_CV_HANDLER,
  	ZEND_CLONE_SPEC_CV_HANDLER,
  	ZEND_CLONE_SPEC_CV_HANDLER,
  	ZEND_CLONE_SPEC_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_INIT_METHOD_CALL_SPEC_TMP_CONST_HANDLER,
  	ZEND_INIT_METHOD_CALL_SPEC_TMP_TMP_HANDLER,
  	ZEND_INIT_METHOD_CALL_SPEC_TMP_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_INIT_METHOD_CALL_SPEC_TMP_CV_HANDLER,
  	ZEND_INIT_METHOD_CALL_SPEC_VAR_CONST_HANDLER,
  	ZEND_INIT_METHOD_CALL_SPEC_VAR_TMP_HANDLER,
  	ZEND_INIT_METHOD_CALL_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_INIT_METHOD_CALL_SPEC_VAR_CV_HANDLER,
  	ZEND_INIT_METHOD_CALL_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_INIT_METHOD_CALL_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_INIT_METHOD_CALL_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_INIT_METHOD_CALL_SPEC_UNUSED_CV_HANDLER,
  	ZEND_INIT_METHOD_CALL_SPEC_CV_CONST_HANDLER,
  	ZEND_INIT_METHOD_CALL_SPEC_CV_TMP_HANDLER,
  	ZEND_INIT_METHOD_CALL_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_INIT_METHOD_CALL_SPEC_CV_CV_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_CONST_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_TMP_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_VAR_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_UNUSED_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_CV_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_CONST_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_TMP_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_VAR_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_UNUSED_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_CV_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_CONST_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_TMP_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_VAR_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_UNUSED_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_CV_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_CONST_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_TMP_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_VAR_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_UNUSED_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_CV_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_CONST_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_TMP_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_VAR_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_UNUSED_HANDLER,
  	ZEND_INIT_STATIC_METHOD_CALL_SPEC_CV_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_CONST_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_CONST_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_CONST_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_CONST_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_CONST_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_TMP_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_TMP_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_TMP_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_TMP_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_TMP_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_VAR_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_VAR_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_VAR_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_VAR_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_CV_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_CV_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_CV_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_CV_HANDLER,
  	ZEND_ISSET_ISEMPTY_VAR_SPEC_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_VAR_CONST_HANDLER,
  	ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_VAR_TMP_HANDLER,
  	ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_VAR_CV_HANDLER,
  	ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_UNUSED_CV_HANDLER,
  	ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_CV_CONST_HANDLER,
  	ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_CV_TMP_HANDLER,
  	ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_PRE_INC_OBJ_SPEC_VAR_CONST_HANDLER,
  	ZEND_PRE_INC_OBJ_SPEC_VAR_TMP_HANDLER,
  	ZEND_PRE_INC_OBJ_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_PRE_INC_OBJ_SPEC_VAR_CV_HANDLER,
  	ZEND_PRE_INC_OBJ_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_PRE_INC_OBJ_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_PRE_INC_OBJ_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_PRE_INC_OBJ_SPEC_UNUSED_CV_HANDLER,
  	ZEND_PRE_INC_OBJ_SPEC_CV_CONST_HANDLER,
  	ZEND_PRE_INC_OBJ_SPEC_CV_TMP_HANDLER,
  	ZEND_PRE_INC_OBJ_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_PRE_INC_OBJ_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_PRE_DEC_OBJ_SPEC_VAR_CONST_HANDLER,
  	ZEND_PRE_DEC_OBJ_SPEC_VAR_TMP_HANDLER,
  	ZEND_PRE_DEC_OBJ_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_PRE_DEC_OBJ_SPEC_VAR_CV_HANDLER,
  	ZEND_PRE_DEC_OBJ_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_PRE_DEC_OBJ_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_PRE_DEC_OBJ_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_PRE_DEC_OBJ_SPEC_UNUSED_CV_HANDLER,
  	ZEND_PRE_DEC_OBJ_SPEC_CV_CONST_HANDLER,
  	ZEND_PRE_DEC_OBJ_SPEC_CV_TMP_HANDLER,
  	ZEND_PRE_DEC_OBJ_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_PRE_DEC_OBJ_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_POST_INC_OBJ_SPEC_VAR_CONST_HANDLER,
  	ZEND_POST_INC_OBJ_SPEC_VAR_TMP_HANDLER,
  	ZEND_POST_INC_OBJ_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_POST_INC_OBJ_SPEC_VAR_CV_HANDLER,
  	ZEND_POST_INC_OBJ_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_POST_INC_OBJ_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_POST_INC_OBJ_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_POST_INC_OBJ_SPEC_UNUSED_CV_HANDLER,
  	ZEND_POST_INC_OBJ_SPEC_CV_CONST_HANDLER,
  	ZEND_POST_INC_OBJ_SPEC_CV_TMP_HANDLER,
  	ZEND_POST_INC_OBJ_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_POST_INC_OBJ_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_POST_DEC_OBJ_SPEC_VAR_CONST_HANDLER,
  	ZEND_POST_DEC_OBJ_SPEC_VAR_TMP_HANDLER,
  	ZEND_POST_DEC_OBJ_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_POST_DEC_OBJ_SPEC_VAR_CV_HANDLER,
  	ZEND_POST_DEC_OBJ_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_POST_DEC_OBJ_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_POST_DEC_OBJ_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_POST_DEC_OBJ_SPEC_UNUSED_CV_HANDLER,
  	ZEND_POST_DEC_OBJ_SPEC_CV_CONST_HANDLER,
  	ZEND_POST_DEC_OBJ_SPEC_CV_TMP_HANDLER,
  	ZEND_POST_DEC_OBJ_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_POST_DEC_OBJ_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_OBJ_SPEC_VAR_CONST_HANDLER,
  	ZEND_ASSIGN_OBJ_SPEC_VAR_TMP_HANDLER,
  	ZEND_ASSIGN_OBJ_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_OBJ_SPEC_VAR_CV_HANDLER,
  	ZEND_ASSIGN_OBJ_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_ASSIGN_OBJ_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_ASSIGN_OBJ_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_OBJ_SPEC_UNUSED_CV_HANDLER,
  	ZEND_ASSIGN_OBJ_SPEC_CV_CONST_HANDLER,
  	ZEND_ASSIGN_OBJ_SPEC_CV_TMP_HANDLER,
  	ZEND_ASSIGN_OBJ_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_OBJ_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_INSTANCEOF_SPEC_TMP_HANDLER,
  	ZEND_INSTANCEOF_SPEC_TMP_HANDLER,
  	ZEND_INSTANCEOF_SPEC_TMP_HANDLER,
  	ZEND_INSTANCEOF_SPEC_TMP_HANDLER,
  	ZEND_INSTANCEOF_SPEC_TMP_HANDLER,
  	ZEND_INSTANCEOF_SPEC_VAR_HANDLER,
  	ZEND_INSTANCEOF_SPEC_VAR_HANDLER,
  	ZEND_INSTANCEOF_SPEC_VAR_HANDLER,
  	ZEND_INSTANCEOF_SPEC_VAR_HANDLER,
  	ZEND_INSTANCEOF_SPEC_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_INSTANCEOF_SPEC_CV_HANDLER,
  	ZEND_INSTANCEOF_SPEC_CV_HANDLER,
  	ZEND_INSTANCEOF_SPEC_CV_HANDLER,
  	ZEND_INSTANCEOF_SPEC_CV_HANDLER,
  	ZEND_INSTANCEOF_SPEC_CV_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_INHERITED_CLASS_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_DECLARE_FUNCTION_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_RAISE_ABSTRACT_ERROR_SPEC_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_ADD_INTERFACE_SPEC_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_VERIFY_ABSTRACT_CLASS_SPEC_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_DIM_SPEC_VAR_CONST_HANDLER,
  	ZEND_ASSIGN_DIM_SPEC_VAR_TMP_HANDLER,
  	ZEND_ASSIGN_DIM_SPEC_VAR_VAR_HANDLER,
  	ZEND_ASSIGN_DIM_SPEC_VAR_UNUSED_HANDLER,
  	ZEND_ASSIGN_DIM_SPEC_VAR_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ASSIGN_DIM_SPEC_CV_CONST_HANDLER,
  	ZEND_ASSIGN_DIM_SPEC_CV_TMP_HANDLER,
  	ZEND_ASSIGN_DIM_SPEC_CV_VAR_HANDLER,
  	ZEND_ASSIGN_DIM_SPEC_CV_UNUSED_HANDLER,
  	ZEND_ASSIGN_DIM_SPEC_CV_CV_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_VAR_CONST_HANDLER,
  	ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_VAR_TMP_HANDLER,
  	ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_VAR_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_VAR_CV_HANDLER,
  	ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_UNUSED_CONST_HANDLER,
  	ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_UNUSED_TMP_HANDLER,
  	ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_UNUSED_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_UNUSED_CV_HANDLER,
  	ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_CV_CONST_HANDLER,
  	ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_CV_TMP_HANDLER,
  	ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_CV_VAR_HANDLER,
  	ZEND_NULL_HANDLER,
  	ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC_CV_CV_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_HANDLE_EXCEPTION_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_USER_OPCODE_SPEC_HANDLER,
  	ZEND_NULL_HANDLER
  };
  zend_opcode_handlers = (opcode_handler_t*)labels;
}
static opcode_handler_t zend_vm_get_opcode_handler(zend_uchar opcode, zend_op* op)
{
		static const int zend_vm_decode[] = {
			_UNUSED_CODE, /* 0              */
			_CONST_CODE,  /* 1 = IS_CONST   */
			_TMP_CODE,    /* 2 = IS_TMP_VAR */
			_UNUSED_CODE, /* 3              */
			_VAR_CODE,    /* 4 = IS_VAR     */
			_UNUSED_CODE, /* 5              */
			_UNUSED_CODE, /* 6              */
			_UNUSED_CODE, /* 7              */
			_UNUSED_CODE, /* 8 = IS_UNUSED  */
			_UNUSED_CODE, /* 9              */
			_UNUSED_CODE, /* 10             */
			_UNUSED_CODE, /* 11             */
			_UNUSED_CODE, /* 12             */
			_UNUSED_CODE, /* 13             */
			_UNUSED_CODE, /* 14             */
			_UNUSED_CODE, /* 15             */
			_CV_CODE      /* 16 = IS_CV     */
		};
		return zend_opcode_handlers[opcode * 25 + zend_vm_decode[op->op1.op_type] * 5 + zend_vm_decode[op->op2.op_type]];
}

ZEND_API void zend_vm_set_opcode_handler(zend_op* op)
{
	op->handler = zend_vm_get_opcode_handler(zend_user_opcodes[op->opcode], op);
}

ZEND_API int zend_do_fcall(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_do_fcall_common_helper_SPEC(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

