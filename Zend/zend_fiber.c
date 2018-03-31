/* fiber extension for PHP (c) 2017 Haitao Lv <php@lvht.net> */

#include "zend.h"
#include "zend_API.h"
#include "zend_closures.h"
#include "zend_exceptions.h"
#include "zend_fiber.h"
#include "zend_interfaces.h"
#include "zend_vm.h"

static zend_class_entry *zend_ce_fiber;
static zend_object_handlers zend_fiber_handlers;

ZEND_DECLARE_MODULE_GLOBALS(fiber)

static zend_object *zend_fiber_create(zend_class_entry *ce);
static void zend_fiber_close(zend_fiber *fiber);
static void fiber_interrupt_function(zend_execute_data *execute_data);

static void (*orig_interrupt_function)(zend_execute_data *execute_data);

static zend_op_array fiber_terminate_func;
static zend_try_catch_element fiber_terminate_try_catch_array = {0,1,0,0};
static zend_op fiber_terminate_op[2];

static void zend_fiber_cleanup_unfinished_execution(zend_execute_data *execute_data) /* {{{ */
{
	if (execute_data->opline != execute_data->func->op_array.opcodes) {
		/* -1 required because we want the last run opcode, not the next to-be-run one. */
		uint32_t op_num = execute_data->opline - execute_data->func->op_array.opcodes - 1;

		zend_cleanup_unfinished_execution(execute_data, op_num, 0);
	}
}
/* }}} */

static void zend_fiber_close(zend_fiber *fiber) /* {{{ */
{
	zend_vm_stack original_stack;
	size_t original_stack_page_size;
	zend_execute_data *execute_data;

	if (UNEXPECTED(fiber->status == ZEND_FIBER_STATUS_INIT)) {
		return;
	}

	original_stack = EG(vm_stack);
	original_stack->top = EG(vm_stack_top);
	original_stack->end = EG(vm_stack_end);
	original_stack_page_size = EG(vm_stack_page_size);

	EG(vm_stack_top) = fiber->stack->top;
	EG(vm_stack_end) = fiber->stack->end;
	EG(vm_stack) = fiber->stack;
	EG(vm_stack_page_size) = fiber->stack_size;

	if (EXPECTED(fiber->status == ZEND_FIBER_STATUS_FINISHED)) {
		goto LABEL_FREE_STACK;
	}

	if (UNEXPECTED(fiber->status == ZEND_FIBER_STATUS_DEAD)) {
		goto LABEL_FREE_STACK;
	}

	execute_data = fiber->execute_data;

	while (execute_data) {
		if (EX_CALL_INFO() & ZEND_CALL_HAS_SYMBOL_TABLE) {
			zend_clean_and_cache_symbol_table(execute_data->symbol_table);
		}

		zend_free_compiled_variables(execute_data);

		if (UNEXPECTED(EX_CALL_INFO() & ZEND_CALL_RELEASE_THIS)) {
			zend_object *object = Z_OBJ(execute_data->This);
			if (UNEXPECTED(EG(exception) != NULL) && (EX_CALL_INFO() & ZEND_CALL_CTOR)) {
				GC_DELREF(object);
				zend_object_store_ctor_failed(object);
			}
			OBJ_RELEASE(object);
		} else if (UNEXPECTED(EX_CALL_INFO() & ZEND_CALL_CLOSURE)) {
			OBJ_RELEASE(ZEND_CLOSURE_OBJECT(execute_data->func));
		}

		zend_vm_stack_free_extra_args(execute_data);

		/* A fatal error / die occurred during the fiber execution.
		 * Trying to clean up the stack may not be safe in this case. */
		if (UNEXPECTED(CG(unclean_shutdown))) {
			EG(vm_stack_top) = original_stack->top;
			EG(vm_stack_end) = original_stack->end;
			EG(vm_stack) = original_stack;
			EG(vm_stack_page_size) = original_stack_page_size;
			return;
		}

		/* Some cleanups are only necessary if the fiber was closed
		 * before it could finish execution (reach a return statement). */
		zend_fiber_cleanup_unfinished_execution(execute_data);

		zend_execute_data *call = execute_data;
		execute_data = execute_data->prev_execute_data;

		zend_vm_stack_free_call_frame(call);
	}

LABEL_FREE_STACK:
	zend_vm_stack_destroy();

	fiber->stack = NULL;

	EG(vm_stack_top) = original_stack->top;
	EG(vm_stack_end) = original_stack->end;
	EG(vm_stack) = original_stack;
	EG(vm_stack_page_size) = original_stack_page_size;
}
/* }}} */

static void zend_fiber_free_storage(zend_object *object) /* {{{ */
{
	zend_fiber *fiber = (zend_fiber*) object;

	zend_fiber_close(fiber);

	zval_ptr_dtor(&fiber->callable);

	zend_object_std_dtor(&fiber->std);
}
/* }}} */

static zend_object *zend_fiber_create(zend_class_entry *ce) /* {{{ */
{
	zend_fiber *fiber;

	fiber = emalloc(sizeof(zend_fiber));
	memset(fiber, 0, sizeof(zend_fiber));

	zend_object_std_init(&fiber->std, ce);
	fiber->std.handlers = &zend_fiber_handlers;

	return &fiber->std;
}
/* }}} */

static int zend_fiber_start(zend_fiber *fiber, zval *params, uint32_t param_count) /* {{{ */
{
	zend_execute_data *call;
	zend_fcall_info_cache fci_cache;
	zend_function *func;
	char *error = NULL;
	zend_vm_stack stack;
	zend_vm_stack current_stack;
	size_t current_stack_page_size;
	zend_execute_data *fiber_frame;
	uint32_t i;

	if (!zend_is_callable_ex(&fiber->callable, NULL, IS_CALLABLE_CHECK_SILENT, NULL, &fci_cache, &error)) {
		zend_throw_error(NULL, "Attempt to start non callable Fiber, %s", error);
		efree(error);
		return FAILURE;
	}

	func = fci_cache.function_handler;

	if (param_count < func->common.required_num_args) {
		zend_throw_error(zend_ce_argument_count_error,
			"Too few arguments for callable given to Fiber, %d required, %d given",
			func->common.required_num_args,
			param_count);
		return FAILURE;
	}

	current_stack = EG(vm_stack);
	current_stack->top = EG(vm_stack_top);
	current_stack->end = EG(vm_stack_end);
	current_stack_page_size = EG(vm_stack_page_size);

	stack = (zend_vm_stack)emalloc(fiber->stack_size);
	stack->top = ZEND_VM_STACK_ELEMENTS(stack) + 1;
	stack->end = (zval*)((char*)stack + fiber->stack_size);
	stack->prev = NULL;

	EG(vm_stack) = stack;
	EG(vm_stack_top) = EG(vm_stack)->top;
	EG(vm_stack_end) = EG(vm_stack)->end;
	EG(vm_stack_page_size) = fiber->stack_size;

	fiber_frame = (zend_execute_data*)EG(vm_stack_top);
	EG(vm_stack_top) = (zval*)fiber_frame + ZEND_CALL_FRAME_SLOT;
	zend_vm_init_call_frame(fiber_frame, ZEND_CALL_TOP_FUNCTION, (zend_function*)&fiber_terminate_func, 0, NULL, NULL);
	fiber_frame->opline = fiber_terminate_op;
	fiber_frame->call = NULL;
	fiber_frame->return_value = NULL;
	fiber_frame->prev_execute_data = NULL;

	call = zend_vm_stack_push_call_frame(ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_DYNAMIC,
		func, param_count, fci_cache.called_scope, fci_cache.object);

	for (i = 0; i < param_count; i++) {
		zval *param;
		zval *arg = &params[i];

		if (ARG_SHOULD_BE_SENT_BY_REF(func, i + 1)) {
			if (UNEXPECTED(!Z_ISREF_P(arg))) {
				if (!ARG_MAY_BE_SENT_BY_REF(func, i + 1)) {
					/* By-value send is not allowed -- emit a warning,
					 * but still perform the call with a by-value send. */
					zend_error(E_WARNING,
						"Parameter %d to %s%s%s() expected to be a reference, value given", i+1,
						func->common.scope ? ZSTR_VAL(func->common.scope->name) : "",
						func->common.scope ? "::" : "",
						ZSTR_VAL(func->common.function_name));
					if (UNEXPECTED(EG(exception))) {
						ZEND_CALL_NUM_ARGS(call) = i;
						zend_vm_stack_free_args(call);
						zend_vm_stack_free_call_frame(call);

						// TODO: cleanup

						EG(vm_stack) = current_stack;
						EG(vm_stack_top) = current_stack->top;
						EG(vm_stack_end) = current_stack->end;
						EG(vm_stack_page_size) = current_stack_page_size;

						return FAILURE;
					}
				}
			}
		} else {
			if (Z_ISREF_P(arg) && !(func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
				/* don't separate references for __call */
				arg = Z_REFVAL_P(arg);
			}
		}

		param = ZEND_CALL_ARG(call, i+1);
		ZVAL_COPY(param, arg);
	}

	if (UNEXPECTED(func->op_array.fn_flags & ZEND_ACC_CLOSURE)) {
		uint32_t call_info;

		GC_ADDREF(ZEND_CLOSURE_OBJECT(func));
		call_info = ZEND_CALL_CLOSURE;
		if (func->common.fn_flags & ZEND_ACC_FAKE_CLOSURE) {
			call_info |= ZEND_CALL_FAKE_CLOSURE;
		}
		ZEND_ADD_CALL_FLAG(call, call_info);
	}

	zend_init_func_execute_data(call, &func->op_array, NULL);
	call->prev_execute_data = fiber_frame;

	fiber->root_execute_data = call;
	fiber->execute_data = call;
	fiber->status = ZEND_FIBER_STATUS_SUSPENDED;

	fiber->stack = EG(vm_stack);
	fiber->stack->top = EG(vm_stack_top);
	fiber->stack->end = EG(vm_stack_end);

	EG(vm_stack) = current_stack;
	EG(vm_stack_top) = current_stack->top;
	EG(vm_stack_end) = current_stack->end;
	EG(vm_stack_page_size) = current_stack_page_size;

	return SUCCESS;
}
/* }}} */

static void fiber_interrupt_function(zend_execute_data *execute_data)/*{{{*/
{
	zend_fiber *fiber;
	zval *exception;

	if (FIBER_G(pending_interrupt)) {
		FIBER_G(pending_interrupt) = 0;

		fiber = FIBER_G(current_fiber);
		if (fiber) {
			/* Suspend current fiber */
			if (EXPECTED(fiber->status == ZEND_FIBER_STATUS_RUNNING)) {
				fiber->execute_data = execute_data;
				fiber->status = ZEND_FIBER_STATUS_SUSPENDED;
			}
			fiber->stack = EG(vm_stack);
			fiber->stack->top = EG(vm_stack_top);
			fiber->stack->end = EG(vm_stack_end);
		} else {
			/* Backup main execution context */
			FIBER_G(orig_execute_data) = execute_data;
			FIBER_G(orig_stack) = EG(vm_stack);
			FIBER_G(orig_stack)->top = EG(vm_stack_top);
			FIBER_G(orig_stack)->end = EG(vm_stack_end);
			FIBER_G(orig_stack_page_size) = EG(vm_stack_page_size);
		}

		fiber = FIBER_G(next_fiber);
		if (fiber) {
			/* Resume next fiber */
			ZEND_ASSERT(fiber->status == ZEND_FIBER_STATUS_SUSPENDED);
			EG(current_execute_data) = fiber->execute_data;
			EG(vm_stack) = fiber->stack;
			EG(vm_stack_top) = fiber->stack->top;
			EG(vm_stack_end) = fiber->stack->end;
			EG(vm_stack_page_size) = fiber->stack_size;
			fiber->status = ZEND_FIBER_STATUS_RUNNING;

			if (UNEXPECTED(FIBER_G(exception))) {
				exception = FIBER_G(exception);
				FIBER_G(exception) = NULL;

				fiber->execute_data->opline--;
				zend_throw_exception_internal(exception);
				fiber->execute_data->opline++;
			}
		} else {
			/* Restore main execution context */
			EG(current_execute_data) = FIBER_G(orig_execute_data);
			EG(vm_stack) = FIBER_G(orig_stack);
			EG(vm_stack_top) = FIBER_G(orig_stack)->top;
			EG(vm_stack_end) = FIBER_G(orig_stack)->end;
			EG(vm_stack_page_size) = FIBER_G(orig_stack_page_size);
		}

		FIBER_G(current_fiber) = fiber;
		FIBER_G(next_fiber) = NULL;

		if (UNEXPECTED(EG(exception))) {
			zend_rethrow_exception(EG(current_execute_data));
		}
	}

	if (UNEXPECTED(FIBER_G(release_this))) {
		GC_DELREF((zend_object *)fiber);
		FIBER_G(release_this) = 0;
	}

	if (orig_interrupt_function) {
		orig_interrupt_function(execute_data);
	}
}/*}}}*/

static int fiber_terminate_opcode_handler(zend_execute_data *execute_data) /* {{{ */
{
	zend_fiber *fiber = FIBER_G(current_fiber);

	ZEND_ASSERT(fiber != NULL);

	FIBER_G(next_fiber) = fiber->original_fiber;
	FIBER_G(pending_interrupt) = 1;

	fiber->original_fiber = NULL;
	fiber->send_value = NULL;
	if (EG(exception)) {
		fiber->status = ZEND_FIBER_STATUS_DEAD;
	} else {
		fiber->status = ZEND_FIBER_STATUS_FINISHED;
	}

	fiber_interrupt_function(execute_data);

	return ZEND_USER_OPCODE_ENTER;
}
/* }}} */

/* {{{ proto Fiber Fiber::__construct(callable, int stack_size)
 * Create a Fiber from a callable. */
ZEND_METHOD(Fiber, __construct)
{
	zval *callable = NULL;
	zend_fiber *fiber = NULL;
	zend_long stack_size = EG(fiber_stack_page_size);

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(callable);
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(stack_size);
	ZEND_PARSE_PARAMETERS_END();

	fiber = (zend_fiber *) Z_OBJ_P(getThis());
	fiber->status = ZEND_FIBER_STATUS_INIT;
	fiber->stack_size = stack_size;

	if (callable) {
		ZVAL_COPY(&fiber->callable, callable);
	}
}
/* }}} */

/* {{{ proto mixed Fiber::resume(vars...)
 * Resume and send a value to the fiber */
ZEND_METHOD(Fiber, resume)
{
	zval *params;
	uint32_t param_count;
	zend_fiber *fiber;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC('+', params, param_count)
	ZEND_PARSE_PARAMETERS_END();

	fiber = (zend_fiber *) Z_OBJ_P(getThis());

	if (fiber->status == ZEND_FIBER_STATUS_INIT) {
		if (zend_fiber_start(fiber, params, param_count) != SUCCESS) {
			return;
		}
		fiber->zend_vm_id = EG(vm_count);
	} else if (fiber->status == ZEND_FIBER_STATUS_SUSPENDED) {
		if (param_count) {
			if (param_count == 1) {
				if (fiber->send_value) {
					ZVAL_COPY(fiber->send_value, params);
					fiber->send_value = NULL;
				}
			} else {
				zend_throw_error(zend_ce_argument_count_error,
					"Only one argument allowed when resuming an initialized Fiber");
				return;
			}
		}
	} else {
		zend_throw_error(NULL, "Attempt to resume non suspended Fiber");
		return;
	}

	fiber->root_execute_data->return_value = USED_RET() ? return_value : NULL;
	fiber->original_fiber = FIBER_G(current_fiber);
	FIBER_G(next_fiber) = fiber;
	FIBER_G(pending_interrupt) = 1;
	EG(vm_interrupt) = 1;

	if (UNEXPECTED(EX_CALL_INFO() & ZEND_CALL_RELEASE_THIS)) {
		GC_ADDREF((zend_object *)fiber);
		FIBER_G(release_this) = 1;
	}
}
/* }}} */

/* {{{ proto mixed Fiber::yield([var])
 * Pause and return a value to the fiber */
ZEND_METHOD(Fiber, yield)
{
	zend_fiber *fiber = FIBER_G(current_fiber);
	zval *ret = NULL;

	if (!fiber) {
		zend_throw_error(NULL, "Cannot call Fiber::yield out of Fiber");
		return;
	}

	if (UNEXPECTED(fiber->zend_vm_id != EG(vm_count))) {
		zend_throw_error(NULL, "Attempt to resume across internal call");
		return;
	}


	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(ret);
	ZEND_PARSE_PARAMETERS_END();

	if (ret && fiber->root_execute_data->return_value) {
		ZVAL_COPY(fiber->root_execute_data->return_value, ret);
		fiber->root_execute_data->return_value = NULL;
	}

	fiber->send_value = USED_RET() ? return_value : NULL;

	FIBER_G(next_fiber) = fiber->original_fiber;
	fiber->original_fiber = NULL;
	FIBER_G(pending_interrupt) = 1;
	EG(vm_interrupt) = 1;
}
/* }}} */

/* {{{ proto void Fiber::status()
 * Get fiber's status */
ZEND_METHOD(Fiber, status)
{
	zend_fiber *fiber = (zend_fiber *) Z_OBJ_P(getThis());
	RETURN_LONG(fiber->status);
}
/* }}} */

/* {{{ proto void Fiber::__wakeup()
 * Throws an Exception as fibers can't be serialized */
ZEND_METHOD(Fiber, __wakeup)
{
	/* Just specifying the zend_class_unserialize_deny handler is not enough,
	 * because it is only invoked for C unserialization. For O the error has
	 * to be thrown in __wakeup. */

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_throw_exception(NULL, "Unserialization of 'Fiber' is not allowed", 0);
}
/* }}} */

/* {{{ proto mixed Fiber::throw(Throwable exception)
 * Throws an exception into the fiber */
ZEND_METHOD(Fiber, throw)
{
	zval *exception;
	zend_fiber *fiber;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(exception)
	ZEND_PARSE_PARAMETERS_END();

	Z_TRY_ADDREF_P(exception);

	fiber = (zend_fiber *) Z_OBJ_P(getThis());

	if (fiber->status == ZEND_FIBER_STATUS_SUSPENDED) {
		FIBER_G(exception) = exception;
	} else {
		zend_throw_exception_object(exception);
		return;
	}

	fiber->root_execute_data->return_value = USED_RET() ? return_value : NULL;
	fiber->original_fiber = FIBER_G(current_fiber);
	FIBER_G(next_fiber) = fiber;
	FIBER_G(pending_interrupt) = 1;
	EG(vm_interrupt) = 1;

	if (UNEXPECTED(EX_CALL_INFO() & ZEND_CALL_RELEASE_THIS)) {
		GC_ADDREF((zend_object *)fiber);
		FIBER_G(release_this) = 1;
	}
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_fiber_create, 0, 0, 1)/*{{{*/
	ZEND_ARG_CALLABLE_INFO(0, callable, 0)
	ZEND_ARG_TYPE_INFO(0, stack_size, IS_LONG, 0)
ZEND_END_ARG_INFO()/*}}}*/

ZEND_BEGIN_ARG_INFO(arginfo_fiber_void, 0)/*{{{*/
ZEND_END_ARG_INFO()/*}}}*/

ZEND_BEGIN_ARG_INFO_EX(arginfo_fiber_resume, 0, 0, 0)/*{{{*/
	ZEND_ARG_VARIADIC_INFO(0, vars)
ZEND_END_ARG_INFO()/*}}}*/

ZEND_BEGIN_ARG_INFO(arginfo_fiber_yield, 0)/*{{{*/
	ZEND_ARG_INFO(0, ret)
ZEND_END_ARG_INFO()/*}}}*/

ZEND_BEGIN_ARG_INFO(arginfo_fiber_throw, 0)/*{{{*/
	 ZEND_ARG_OBJ_INFO(0, exception, Throwable, 0)
ZEND_END_ARG_INFO()/*}}}*/

static const zend_function_entry fiber_functions[] = {/*{{{*/
	ZEND_ME(Fiber, __construct, arginfo_fiber_create, ZEND_ACC_PUBLIC)
	ZEND_ME(Fiber, resume,      arginfo_fiber_resume, ZEND_ACC_PUBLIC)
	ZEND_ME(Fiber, yield,       arginfo_fiber_yield,  ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Fiber, throw,       arginfo_fiber_throw,  ZEND_ACC_PUBLIC)
	ZEND_ME(Fiber, status,      arginfo_fiber_void,   ZEND_ACC_PUBLIC)
	ZEND_ME(Fiber, __wakeup,    arginfo_fiber_void,   ZEND_ACC_PUBLIC)
	ZEND_FE_END
};/*}}}*/

/* {{{ PHP_MINIT_FUNCTION
 **/
void zend_register_fiber_ce()
{
	zend_class_entry ce;
	zend_uchar opcode = ZEND_VM_LAST_OPCODE + 1;

	/* Create new user opcode to terminate Fiber */
	while (1) {
		if (opcode == 255) {
			// TODO FAIL
			return;
		} else if (zend_get_user_opcode_handler(opcode) == NULL) {
			break;
		}
		opcode++;
	}
	zend_set_user_opcode_handler(opcode, fiber_terminate_opcode_handler);

	memset(fiber_terminate_op, 0, sizeof(fiber_terminate_op));
	/* Also set op1_type, op2_type and result_type to IS_UNUSED */

	fiber_terminate_op[0].opcode = opcode;
	zend_vm_set_opcode_handler_ex(fiber_terminate_op, 0, 0, 0);
	fiber_terminate_op[1].opcode = opcode;
	zend_vm_set_opcode_handler_ex(fiber_terminate_op + 1, 0, 0, 0);

	memset(&fiber_terminate_func, 0, sizeof(fiber_terminate_func));
	fiber_terminate_func.type = ZEND_USER_FUNCTION;
	fiber_terminate_func.function_name = zend_string_init("Fiber::__construct", sizeof("Fiber::__construct")-1, 1);
	fiber_terminate_func.filename = ZSTR_EMPTY_ALLOC();
	fiber_terminate_func.opcodes = fiber_terminate_op;
	fiber_terminate_func.last_try_catch = 1;
	fiber_terminate_func.try_catch_array = &fiber_terminate_try_catch_array;

	INIT_CLASS_ENTRY(ce, "Fiber", fiber_functions);
	zend_ce_fiber = zend_register_internal_class(&ce);
	zend_ce_fiber->ce_flags |= ZEND_ACC_FINAL;
	zend_ce_fiber->create_object = zend_fiber_create;
	zend_ce_fiber->serialize = zend_class_serialize_deny;
	zend_ce_fiber->unserialize = zend_class_unserialize_deny;

	memcpy(&zend_fiber_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	zend_fiber_handlers.free_obj = zend_fiber_free_storage;
	zend_fiber_handlers.clone_obj = NULL;

	REGISTER_FIBER_CLASS_CONST_LONG("STATUS_INIT", (zend_long)ZEND_FIBER_STATUS_INIT);
	REGISTER_FIBER_CLASS_CONST_LONG("STATUS_SUSPENDED", (zend_long)ZEND_FIBER_STATUS_SUSPENDED);
	REGISTER_FIBER_CLASS_CONST_LONG("STATUS_RUNNING", (zend_long)ZEND_FIBER_STATUS_RUNNING);
	REGISTER_FIBER_CLASS_CONST_LONG("STATUS_FINISHED", (zend_long)ZEND_FIBER_STATUS_FINISHED);
	REGISTER_FIBER_CLASS_CONST_LONG("STATUS_DEAD", (zend_long)ZEND_FIBER_STATUS_DEAD);

	orig_interrupt_function = zend_interrupt_function;
	zend_interrupt_function = fiber_interrupt_function;
}
/* }}} */

/*
 * vim: sw=4 ts=4
 * vim600: fdm=marker
 */
