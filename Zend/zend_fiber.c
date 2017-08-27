/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2017 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Nikita Popov <nikic@php.net>                                |
   |          Bob Weinand <bobwei9@hotmail.com>                           |
   |          Haitao Lv <php@lvht.net>                                    |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "zend_closures.h"
#include "zend_fiber.h"

ZEND_API zend_class_entry *zend_ce_fiber;
static zend_object_handlers zend_fiber_handlers;

static zend_object *zend_fiber_create(zend_class_entry *class_type);

static void zend_fiber_cleanup_unfinished_execution(zend_execute_data *execute_data,
		zend_fiber *fiber, uint32_t catch_op_num) /* {{{ */
{
	if (execute_data->opline != execute_data->func->op_array.opcodes) {
		/* -1 required because we want the last run opcode, not the next to-be-run one. */
		uint32_t op_num = execute_data->opline - execute_data->func->op_array.opcodes - 1;

		/* There may be calls to zend_vm_stack_free_call_frame(), which modifies the VM stack
		 * globals, so need to load/restore those. */
		zend_vm_stack original_stack = EG(vm_stack);
		original_stack->top = EG(vm_stack_top);
		EG(vm_stack_top) = fiber->stack->top;
		EG(vm_stack_end) = fiber->stack->end;
		EG(vm_stack) = fiber->stack;

		zend_cleanup_unfinished_execution(execute_data, op_num, catch_op_num);

		fiber->stack = EG(vm_stack);
		fiber->stack->top = EG(vm_stack_top);
		EG(vm_stack_top) = original_stack->top;
		EG(vm_stack_end) = original_stack->end;
		EG(vm_stack) = original_stack;
	}
}
/* }}} */

ZEND_API void zend_fiber_close(zend_fiber *fiber, zend_bool finished_execution) /* {{{ */
{
	if (fiber->status == ZEND_FIBER_STATUS_UNINITED) {
		return;
	}

	if (fiber->status == ZEND_FIBER_STATUS_DEAD) {
		return;
	}

	if (fiber->status == ZEND_FIBER_STATUS_FINISHED) {
		zend_vm_stack original_stack = EG(vm_stack);
		EG(vm_stack) = fiber->stack;

		zend_vm_stack_destroy();

		fiber->stack = NULL;
		EG(vm_stack) = original_stack;

		return;
	}

	zend_vm_stack original_stack = EG(vm_stack);
	original_stack->top = EG(vm_stack_top);

	EG(vm_stack_top) = fiber->stack->top;
	EG(vm_stack_end) = fiber->stack->end;
	EG(vm_stack) = fiber->stack;

	zend_execute_data *execute_data = fiber->execute_data;
	zend_execute_data *first_frame = (zend_execute_data*)fiber->first_frame;

	while (execute_data) {
		if (EX_CALL_INFO() & ZEND_CALL_HAS_SYMBOL_TABLE) {
			zend_clean_and_cache_symbol_table(execute_data->symbol_table);
		}
		/* always free the CV's, in the symtable are only not-free'd IS_INDIRECT's */
		zend_free_compiled_variables(execute_data);

		if (Z_OBJ(execute_data->This)) {
			OBJ_RELEASE(Z_OBJ(execute_data->This));
		}

		/* A fatal error / die occurred during the fiber execution.
		 * Trying to clean up the stack may not be safe in this case. */
		if (UNEXPECTED(CG(unclean_shutdown))) {
			return;
		}

		zend_vm_stack_free_extra_args(execute_data);

		/* Some cleanups are only necessary if the fiber was closed
		 * before it could finish execution (reach a return statement). */
		if (UNEXPECTED(!finished_execution)) {
			zend_fiber_cleanup_unfinished_execution(execute_data, fiber, 0);
		}

		/* Free closure object */
		if (EX_CALL_INFO() & ZEND_CALL_CLOSURE) {
			OBJ_RELEASE((zend_object *) EX(func)->common.prototype);
		}

		if (execute_data == first_frame) {
			break;
		}

		zend_execute_data *call = execute_data;
		execute_data = execute_data->prev_execute_data;

		zend_vm_stack_free_call_frame_ex(ZEND_CALL_INFO(call), call);
	}

	zend_vm_stack_destroy();

	fiber->stack = NULL;

	EG(vm_stack_top) = original_stack->top;
	EG(vm_stack_end) = original_stack->end;
	EG(vm_stack) = original_stack;
}
/* }}} */

static void zend_fiber_free_storage(zend_object *object) /* {{{ */
{
	zend_fiber *fiber = (zend_fiber*) object;

	zend_fiber_close(fiber, 0);

	/* we can't immediately free them in zend_fiber_close() else yield from won't be able to fetch it */
	zval_ptr_dtor(&fiber->value);
	zval_ptr_dtor(&fiber->closure);

	zend_object_std_dtor(&fiber->std);
}
/* }}} */

static HashTable *zend_fiber_get_gc(zval *object, zval **table, int *n) /* {{{ */
{
	zend_fiber *fiber = (zend_fiber*) Z_OBJ_P(object);
	*table = &fiber->value;
	*n = 1;
	return NULL;
}
/* }}} */

static zend_object *zend_fiber_create(zend_class_entry *class_type) /* {{{ */
{
	zend_fiber *fiber;

	fiber = emalloc(sizeof(zend_fiber));
	memset(fiber, 0, sizeof(zend_fiber));

	zend_object_std_init(&fiber->std, class_type);
	fiber->std.handlers = &zend_fiber_handlers;

	return (zend_object*)fiber;
}
/* }}} */

ZEND_API void zend_fiber_resume(zend_fiber *fiber) /* {{{ */
{
	/* Backup executor globals */
	zend_execute_data *original_execute_data = EG(current_execute_data);
	zend_vm_stack original_stack = EG(vm_stack);
	zval *original_stack_top = EG(vm_stack_top);
	zval *original_stack_end = EG(vm_stack_end);
	zval *original_fiber = EG(vm_fiber);

	/* Set executor globals */
	EG(vm_stack_top) = fiber->stack_top;
	EG(vm_stack_end) = fiber->stack_end;
	EG(vm_stack) = fiber->stack;
	EG(vm_fiber) = (zval*)fiber;

	if (fiber->execute_data) {
		EG(current_execute_data) = fiber->execute_data;
	}

	/* Resume execution */
	fiber->status = ZEND_FIBER_STATUS_RUNNING;
	if (fiber->execute_data) {
		zend_execute_data *first_frame = (zend_execute_data*)fiber->first_frame;
		first_frame->prev_execute_data = original_execute_data;
		zend_execute_ex(fiber->execute_data);
	} else {
		int n_vars = fiber->n_vars;
		fiber->n_vars = 0;
		call_user_function(NULL, NULL, &fiber->closure, &fiber->value, n_vars, fiber->vars);
	}

	if (fiber->execute_data) {
		EG(current_execute_data) = original_execute_data;
	}
	/* Restore executor globals */
	EG(vm_stack_top) = original_stack_top;
	EG(vm_stack_end) = original_stack_end;
	EG(vm_stack) = original_stack;
	EG(vm_fiber) = original_fiber;

	/* If an exception was thrown in the fiber we have to internally
	 * rethrow it in the parent scope.*/
	if (UNEXPECTED(EG(exception) != NULL)) {
		zend_fiber_close(fiber, 0);
		fiber->status = ZEND_FIBER_STATUS_DEAD;
		if (EG(current_execute_data) &&
				EG(current_execute_data)->func &&
				ZEND_USER_CODE(EG(current_execute_data)->func->common.type)) {
			zend_rethrow_exception(EG(current_execute_data));
		}
	} else if (fiber->status != ZEND_FIBER_STATUS_SUSPENDED) {
		fiber->status = ZEND_FIBER_STATUS_FINISHED;
	}
}
/* }}} */

/* {{{ proto Fiber Fiber::__construct(Closure closure)
   Create a Fiber from a closure. */
ZEND_METHOD(Fiber, __construct)
{
	zval *closure;
	zend_fiber *fiber;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(closure, zend_ce_closure)
	ZEND_PARSE_PARAMETERS_END();

	fiber = (zend_fiber *) Z_OBJ_P(getThis());
	fiber->status = ZEND_FIBER_STATUS_SUSPENDED;

	zend_vm_stack current_stack = EG(vm_stack);
	zval *current_stack_top = EG(vm_stack_top);
	zval *current_stack_end = EG(vm_stack_end);

	zend_fiber_stack_init();

	fiber->stack = EG(vm_stack);
	fiber->stack_top = EG(vm_stack_top);
	fiber->first_frame = EG(vm_stack_top);
	fiber->stack_end = EG(vm_stack_end);

	ZVAL_COPY(&fiber->closure, closure);

	EG(vm_stack) = current_stack;
	EG(vm_stack_top) = current_stack_top;
	EG(vm_stack_end) = current_stack_end;
}
/* }}} */

/* {{{ proto mixed Fiber::resume(vars...)
 * Resume and send a value to the fiber */
ZEND_METHOD(Fiber, resume)
{
	zend_fiber *fiber = (zend_fiber *) Z_OBJ_P(getThis());
	if (fiber->status != ZEND_FIBER_STATUS_SUSPENDED) {
		RETURN_NULL();
	}

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC('+', fiber->vars, fiber->n_vars)
	ZEND_PARSE_PARAMETERS_END();

	/* Put sent value in the target VAR slot, if it is used */
	if (fiber->send_target && fiber->vars) {
		ZVAL_COPY(fiber->send_target, fiber->vars);
		fiber->n_vars = 0;
	}

	zend_fiber_resume(fiber);

	ZVAL_COPY(return_value, &fiber->value);
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_fiber_create, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, closure, Closure, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fiber_void, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fiber_resume, 0, 0, 0)
	ZEND_ARG_VARIADIC_INFO(0, vars)
ZEND_END_ARG_INFO()

static const zend_function_entry fiber_functions[] = {
	ZEND_ME(Fiber, __construct,   arginfo_fiber_create, ZEND_ACC_PUBLIC)
	ZEND_ME(Fiber, resume,   arginfo_fiber_resume, ZEND_ACC_PUBLIC)
	ZEND_ME(Fiber, status,   arginfo_fiber_void, ZEND_ACC_PUBLIC)
	ZEND_ME(Fiber, __wakeup, arginfo_fiber_void, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

void zend_register_fiber_ce(void) /* {{{ */
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "Fiber", fiber_functions);
	zend_ce_fiber = zend_register_internal_class(&ce);
	zend_ce_fiber->ce_flags |= ZEND_ACC_FINAL;
	zend_ce_fiber->create_object = zend_fiber_create;
	zend_ce_fiber->serialize = zend_class_serialize_deny;
	zend_ce_fiber->unserialize = zend_class_unserialize_deny;

	memcpy(&zend_fiber_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	zend_fiber_handlers.free_obj = zend_fiber_free_storage;
	zend_fiber_handlers.get_gc = zend_fiber_get_gc;
	zend_fiber_handlers.clone_obj = NULL;

	REGISTER_FIBER_CLASS_CONST_LONG("STATUS_SUSPENDED", (zend_long)ZEND_FIBER_STATUS_SUSPENDED);
	REGISTER_FIBER_CLASS_CONST_LONG("STATUS_RUNNING", (zend_long)ZEND_FIBER_STATUS_RUNNING);
	REGISTER_FIBER_CLASS_CONST_LONG("STATUS_FINISHED", (zend_long)ZEND_FIBER_STATUS_FINISHED);
	REGISTER_FIBER_CLASS_CONST_LONG("STATUS_DEAD", (zend_long)ZEND_FIBER_STATUS_DEAD);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
