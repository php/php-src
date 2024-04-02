/*
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
  | Authors: Aaron Piotrowski <aaron@trowski.com>                         |
  +----------------------------------------------------------------------+
*/

#include "php_test.h"
#include "fiber.h"
#include "fiber_arginfo.h"
#include "zend_fibers.h"
#include "zend_exceptions.h"

static zend_class_entry *zend_test_fiber_class;
static zend_object_handlers zend_test_fiber_handlers;

static zend_fiber_transfer zend_test_fiber_switch_to(zend_fiber_context *context, zval *value, bool exception)
{
	zend_fiber_transfer transfer = {
			.context = context,
			.flags = exception ? ZEND_FIBER_TRANSFER_FLAG_ERROR : 0,
	};

	if (value) {
		ZVAL_COPY(&transfer.value, value);
	} else {
		ZVAL_NULL(&transfer.value);
	}

	zend_fiber_switch_context(&transfer);

	/* Forward bailout into current fiber. */
	if (UNEXPECTED(transfer.flags & ZEND_FIBER_TRANSFER_FLAG_BAILOUT)) {
		zend_bailout();
	}

	return transfer;
}

static zend_fiber_transfer zend_test_fiber_resume(zend_test_fiber *fiber, zval *value, bool exception)
{
	zend_test_fiber *previous = ZT_G(active_fiber);

	fiber->caller = EG(current_fiber_context);
	ZT_G(active_fiber) = fiber;

	zend_fiber_transfer transfer = zend_test_fiber_switch_to(fiber->previous, value, exception);

	ZT_G(active_fiber) = previous;

	return transfer;
}

static zend_fiber_transfer zend_test_fiber_suspend(zend_test_fiber *fiber, zval *value)
{
	ZEND_ASSERT(fiber->caller != NULL);

	zend_fiber_context *caller = fiber->caller;
	fiber->previous = EG(current_fiber_context);
	fiber->caller = NULL;

	return zend_test_fiber_switch_to(caller, value, false);
}

static ZEND_STACK_ALIGNED void zend_test_fiber_execute(zend_fiber_transfer *transfer)
{
	zend_test_fiber *fiber = ZT_G(active_fiber);
	zval retval;

	zend_execute_data *execute_data;

	EG(vm_stack) = NULL;
	transfer->flags = 0;

	zend_first_try {
		zend_vm_stack stack = zend_vm_stack_new_page(ZEND_FIBER_VM_STACK_SIZE, NULL);
		EG(vm_stack) = stack;
		EG(vm_stack_top) = stack->top + ZEND_CALL_FRAME_SLOT;
		EG(vm_stack_end) = stack->end;
		EG(vm_stack_page_size) = ZEND_FIBER_VM_STACK_SIZE;

		execute_data = (zend_execute_data *) stack->top;

		memset(execute_data, 0, sizeof(zend_execute_data));

		EG(current_execute_data) = execute_data;
		EG(jit_trace_num) = 0;

#ifdef ZEND_CHECK_STACK_LIMIT
		EG(stack_base) = zend_fiber_stack_base(fiber->context.stack);
		EG(stack_limit) = zend_fiber_stack_limit(fiber->context.stack);
#endif
		fiber->fci.retval = &retval;

		zend_call_function(&fiber->fci, &fiber->fci_cache);

		zval_ptr_dtor(&fiber->result); // Destroy param from symmetric coroutine.
		zval_ptr_dtor(&fiber->fci.function_name);

		if (EG(exception)) {
			if (!(fiber->flags & ZEND_FIBER_FLAG_DESTROYED)
				|| !(zend_is_graceful_exit(EG(exception)) || zend_is_unwind_exit(EG(exception)))
			) {
				fiber->flags |= ZEND_FIBER_FLAG_THREW;
				transfer->flags = ZEND_FIBER_TRANSFER_FLAG_ERROR;

				ZVAL_OBJ_COPY(&transfer->value, EG(exception));
			}

			zend_clear_exception();
		} else {
			ZVAL_COPY_VALUE(&fiber->result, &retval);
			ZVAL_COPY(&transfer->value, &fiber->result);
		}
	} zend_catch {
		fiber->flags |= ZEND_FIBER_FLAG_BAILOUT;
		transfer->flags = ZEND_FIBER_TRANSFER_FLAG_BAILOUT;
	} zend_end_try();

	zend_vm_stack_destroy();

	if (fiber->target) {
		zend_fiber_context *target = &fiber->target->context;
		zend_fiber_init_context(target, zend_test_fiber_class, zend_test_fiber_execute, EG(fiber_stack_size));
		transfer->context = target;

		ZVAL_COPY(&fiber->target->result, &fiber->result);
		fiber->target->fci.params = &fiber->target->result;
		fiber->target->fci.param_count = 1;

		fiber->target->caller = fiber->caller;
		ZT_G(active_fiber) = fiber->target;
	} else {
		transfer->context = fiber->caller;
	}

	fiber->caller = NULL;
}

static zend_object *zend_test_fiber_object_create(zend_class_entry *ce)
{
	zend_test_fiber *fiber;

	fiber = emalloc(sizeof(zend_test_fiber));
	memset(fiber, 0, sizeof(zend_test_fiber));

	zend_object_std_init(&fiber->std, ce);

	return &fiber->std;
}

static void zend_test_fiber_object_destroy(zend_object *object)
{
	zend_test_fiber *fiber = (zend_test_fiber *) object;

	if (fiber->context.status != ZEND_FIBER_STATUS_SUSPENDED) {
		return;
	}

	zend_object *exception = EG(exception);
	EG(exception) = NULL;

	fiber->flags |= ZEND_FIBER_FLAG_DESTROYED;

	zend_fiber_transfer transfer = zend_test_fiber_resume(fiber, NULL, false);

	if (transfer.flags & ZEND_FIBER_TRANSFER_FLAG_ERROR) {
		EG(exception) = Z_OBJ(transfer.value);

		if (!exception && EG(current_execute_data) && EG(current_execute_data)->func
			&& ZEND_USER_CODE(EG(current_execute_data)->func->common.type)) {
			zend_rethrow_exception(EG(current_execute_data));
		}

		zend_exception_set_previous(EG(exception), exception);

		if (!EG(current_execute_data)) {
			zend_exception_error(EG(exception), E_ERROR);
		}
	} else {
		zval_ptr_dtor(&transfer.value);
		EG(exception) = exception;
	}
}

static void zend_test_fiber_object_free(zend_object *object)
{
	zend_test_fiber *fiber = (zend_test_fiber *) object;

	if (fiber->context.status == ZEND_FIBER_STATUS_INIT) {
		// Fiber was never started, so we need to release the reference to the callback.
		zval_ptr_dtor(&fiber->fci.function_name);
	}

	if (fiber->target) {
		OBJ_RELEASE(&fiber->target->std);
	}

	zval_ptr_dtor(&fiber->result);

	zend_object_std_dtor(&fiber->std);
}

static zend_always_inline void delegate_transfer_result(
	zend_test_fiber *fiber, zend_fiber_transfer *transfer, INTERNAL_FUNCTION_PARAMETERS
) {
	if (transfer->flags & ZEND_FIBER_TRANSFER_FLAG_ERROR) {
		zend_throw_exception_internal(Z_OBJ(transfer->value));
		RETURN_THROWS();
	}

	if (fiber->context.status == ZEND_FIBER_STATUS_DEAD) {
		zval_ptr_dtor(&transfer->value);
		RETURN_NULL();
	}

	RETURN_COPY_VALUE(&transfer->value);
}

static ZEND_METHOD(_ZendTestFiber, __construct)
{
	zend_test_fiber *fiber = (zend_test_fiber *) Z_OBJ_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_FUNC(fiber->fci, fiber->fci_cache)
	ZEND_PARSE_PARAMETERS_END();

	// Keep a reference to closures or callable objects while the fiber is running.
	Z_TRY_ADDREF(fiber->fci.function_name);
}

static ZEND_METHOD(_ZendTestFiber, start)
{
	zend_test_fiber *fiber = (zend_test_fiber *) Z_OBJ_P(ZEND_THIS);
	zval *params;
	uint32_t param_count;
	zend_array *named_params;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC_WITH_NAMED(params, param_count, named_params);
	ZEND_PARSE_PARAMETERS_END();

	ZEND_ASSERT(fiber->context.status == ZEND_FIBER_STATUS_INIT);

	if (fiber->previous != NULL) {
		zend_throw_error(NULL, "Cannot start a fiber that is the target of another fiber");
		RETURN_THROWS();
	}

	fiber->fci.params = params;
	fiber->fci.param_count = param_count;
	fiber->fci.named_params = named_params;

	zend_fiber_init_context(&fiber->context, zend_test_fiber_class, zend_test_fiber_execute, EG(fiber_stack_size));

	fiber->previous = &fiber->context;

	zend_fiber_transfer transfer = zend_test_fiber_resume(fiber, NULL, false);

	delegate_transfer_result(fiber, &transfer, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static ZEND_METHOD(_ZendTestFiber, suspend)
{
	zval *value = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(value);
	ZEND_PARSE_PARAMETERS_END();

	zend_test_fiber *fiber = ZT_G(active_fiber);

	ZEND_ASSERT(fiber);

	zend_fiber_transfer transfer = zend_test_fiber_suspend(fiber, value);

	if (fiber->flags & ZEND_FIBER_FLAG_DESTROYED) {
		// This occurs when the test fiber is GC'ed while suspended.
		zval_ptr_dtor(&transfer.value);
		zend_throw_graceful_exit();
		RETURN_THROWS();
	}

	delegate_transfer_result(fiber, &transfer, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static ZEND_METHOD(_ZendTestFiber, resume)
{
	zend_test_fiber *fiber;
	zval *value = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(value);
	ZEND_PARSE_PARAMETERS_END();

	fiber = (zend_test_fiber *) Z_OBJ_P(ZEND_THIS);

	if (UNEXPECTED(fiber->context.status != ZEND_FIBER_STATUS_SUSPENDED || fiber->caller != NULL)) {
		zend_throw_error(NULL, "Cannot resume a fiber that is not suspended");
		RETURN_THROWS();
	}

	zend_fiber_transfer transfer = zend_test_fiber_resume(fiber, value, false);

	delegate_transfer_result(fiber, &transfer, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static ZEND_METHOD(_ZendTestFiber, pipeTo)
{
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_FUNC(fci, fci_cache)
	ZEND_PARSE_PARAMETERS_END();

	zend_test_fiber *fiber = (zend_test_fiber *) Z_OBJ_P(ZEND_THIS);
	zend_test_fiber *target = (zend_test_fiber *) zend_test_fiber_class->create_object(zend_test_fiber_class);

	target->fci = fci;
	target->fci_cache = fci_cache;
	Z_TRY_ADDREF(target->fci.function_name);

	target->previous = &fiber->context;

	if (fiber->target) {
		OBJ_RELEASE(&fiber->target->std);
	}

	fiber->target = target;

	RETURN_OBJ_COPY(&target->std);
}

void zend_test_fiber_init(void)
{
	zend_test_fiber_class = register_class__ZendTestFiber();
	zend_test_fiber_class->create_object = zend_test_fiber_object_create;
	zend_test_fiber_class->default_object_handlers = &zend_test_fiber_handlers;

	zend_test_fiber_handlers = std_object_handlers;
	zend_test_fiber_handlers.dtor_obj = zend_test_fiber_object_destroy;
	zend_test_fiber_handlers.free_obj = zend_test_fiber_object_free;
}
