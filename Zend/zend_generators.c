/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "zend_generators.h"

ZEND_API zend_class_entry *zend_ce_generator;
static zend_object_handlers zend_generator_handlers;

static zend_object *zend_generator_create(zend_class_entry *class_type);

static void zend_generator_cleanup_unfinished_execution(zend_generator *generator) /* {{{ */
{
	zend_execute_data *execute_data = generator->execute_data;
	zend_op_array *op_array = &execute_data->func->op_array;

	if (generator->send_target) {
		if (Z_REFCOUNTED_P(generator->send_target)) Z_DELREF_P(generator->send_target);
		generator->send_target = NULL;
	}

	/* Manually free loop variables, as execution couldn't reach their
	 * SWITCH_FREE / FREE opcodes. */
	{
		/* -1 required because we want the last run opcode, not the
		 * next to-be-run one. */
		uint32_t op_num = execute_data->opline - op_array->opcodes - 1;

		int i;
		for (i = 0; i < op_array->last_brk_cont; ++i) {
			zend_brk_cont_element *brk_cont = op_array->brk_cont_array + i;

			if (brk_cont->start < 0) {
				continue;
			} else if ((uint32_t)brk_cont->start > op_num) {
				break;
			} else if (brk_cont->brk >= 0 && (uint32_t)brk_cont->brk > op_num) {
				zend_op *brk_opline = op_array->opcodes + brk_cont->brk;

				if (brk_opline->opcode == ZEND_FREE) {
					zval *var = EX_VAR(brk_opline->op1.var);
					zval_ptr_dtor_nogc(var);
				} else if (brk_opline->opcode == ZEND_FE_FREE) {
					zval *var = EX_VAR(brk_opline->op1.var);
					if (Z_TYPE_P(var) != IS_ARRAY && Z_FE_ITER_P(var) != (uint32_t)-1) {
						zend_hash_iterator_del(Z_FE_ITER_P(var));
					}
					zval_ptr_dtor_nogc(var);
				}
			}
		}
	}

	/* If yield was used as a function argument there may be active
	 * method calls those objects need to be freed */
	while (execute_data->call) {
		if (Z_OBJ(execute_data->call->This)) {
			OBJ_RELEASE(Z_OBJ(execute_data->call->This));
		}
		execute_data->call = execute_data->call->prev_execute_data;
	}
}
/* }}} */

ZEND_API void zend_generator_close(zend_generator *generator, zend_bool finished_execution) /* {{{ */
{
	if (Z_TYPE(generator->value) != IS_UNDEF) {
		zval_ptr_dtor(&generator->value);
		ZVAL_UNDEF(&generator->value);
	}

	if (Z_TYPE(generator->key) != IS_UNDEF) {
		zval_ptr_dtor(&generator->key);
		ZVAL_UNDEF(&generator->key);
	}

	if (generator->execute_data) {
		zend_execute_data *execute_data = generator->execute_data;
		zend_op_array *op_array = &execute_data->func->op_array;

		if (!execute_data->symbol_table) {
			zend_free_compiled_variables(execute_data);
		} else {
			zend_clean_and_cache_symbol_table(execute_data->symbol_table);
		}

		if (Z_OBJ(execute_data->This)) {
			OBJ_RELEASE(Z_OBJ(execute_data->This));
		}

		/* A fatal error / die occurred during the generator execution. Trying to clean
		 * up the stack may not be safe in this case. */
		if (CG(unclean_shutdown)) {
			generator->execute_data = NULL;
			return;
		}

		zend_vm_stack_free_extra_args(generator->execute_data);

		/* Some cleanups are only necessary if the generator was closued
		 * before it could finish execution (reach a return statement). */
		if (!finished_execution) {
			zend_generator_cleanup_unfinished_execution(generator);
		}

		/* Free a clone of closure */
		if (op_array->fn_flags & ZEND_ACC_CLOSURE) {
			destroy_op_array(op_array);
			efree_size(op_array, sizeof(zend_op_array));
		}

		efree(generator->stack);
		generator->execute_data = NULL;
	}
}
/* }}} */

static void zend_generator_dtor_storage(zend_object *object) /* {{{ */
{
	zend_generator *generator = (zend_generator*) object;
	zend_execute_data *ex = generator->execute_data;
	uint32_t op_num, finally_op_num, finally_op_end;
	int i;

	if (!ex || !(ex->func->op_array.fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK)) {
		return;
	}

	/* -1 required because we want the last run opcode, not the
	 * next to-be-run one. */
	op_num = ex->opline - ex->func->op_array.opcodes - 1;

	/* Find next finally block */
	finally_op_num = 0;
	finally_op_end = 0;
	for (i = 0; i < ex->func->op_array.last_try_catch; i++) {
		zend_try_catch_element *try_catch = &ex->func->op_array.try_catch_array[i];

		if (op_num < try_catch->try_op) {
			break;
		}

		if (op_num < try_catch->finally_op) {
			finally_op_num = try_catch->finally_op;
			finally_op_end = try_catch->finally_end;
		}
	}

	/* If a finally block was found we jump directly to it and
	 * resume the generator. */
	if (finally_op_num) {
		zval *fast_call = ZEND_CALL_VAR(ex, ex->func->op_array.opcodes[finally_op_end].op1.var);

		Z_OBJ_P(fast_call) = NULL;
		fast_call->u2.lineno = (uint32_t)-1;
		ex->opline = &ex->func->op_array.opcodes[finally_op_num];
		generator->flags |= ZEND_GENERATOR_FORCED_CLOSE;
		zend_generator_resume(generator);
	}
}
/* }}} */

static void zend_generator_free_storage(zend_object *object) /* {{{ */
{
	zend_generator *generator = (zend_generator*) object;

	zend_generator_close(generator, 0);

	zval_ptr_dtor(&generator->retval);
	zend_object_std_dtor(&generator->std);

	if (generator->iterator) {
		zend_iterator_dtor(generator->iterator);
	}
}
/* }}} */

static zend_object *zend_generator_create(zend_class_entry *class_type) /* {{{ */
{
	zend_generator *generator;

	generator = emalloc(sizeof(zend_generator));
	memset(generator, 0, sizeof(zend_generator));

	/* The key will be incremented on first use, so it'll start at 0 */
	generator->largest_used_integer_key = -1;

	ZVAL_UNDEF(&generator->retval);

	zend_object_std_init(&generator->std, class_type);
	generator->std.handlers = &zend_generator_handlers;

	return (zend_object*)generator;
}
/* }}} */

static int copy_closure_static_var(zval *var, int num_args, va_list args, zend_hash_key *key) /* {{{ */
{
	HashTable *target = va_arg(args, HashTable *);

	ZVAL_MAKE_REF(var);
	Z_ADDREF_P(var);
	zend_hash_update(target, key->key, var);
	return 0;
}
/* }}} */

/* Requires globals EG(scope), EG(This) and EG(current_execute_data). */
ZEND_API void zend_generator_create_zval(zend_execute_data *call, zend_op_array *op_array, zval *return_value) /* {{{ */
{
	zend_generator *generator;
	zend_execute_data *current_execute_data;
	zend_execute_data *execute_data;
	zend_vm_stack current_stack = EG(vm_stack);

	current_stack->top = EG(vm_stack_top);
	/* Create a clone of closure, because it may be destroyed */
	if (op_array->fn_flags & ZEND_ACC_CLOSURE) {
		zend_op_array *op_array_copy = (zend_op_array*)emalloc(sizeof(zend_op_array));
		*op_array_copy = *op_array;

		if (op_array->refcount) {
			(*op_array->refcount)++;
		}
		op_array->run_time_cache = NULL;
		if (op_array->static_variables) {
			ALLOC_HASHTABLE(op_array_copy->static_variables);
			zend_hash_init(
				op_array_copy->static_variables,
				zend_hash_num_elements(op_array->static_variables),
				NULL, ZVAL_PTR_DTOR, 0
			);
			zend_hash_apply_with_arguments(
				op_array->static_variables,
				copy_closure_static_var, 1,
				op_array_copy->static_variables
			);
		}

		op_array = op_array_copy;
	}

	/* Create new execution context. We have to back up and restore
	 * EG(current_execute_data) here. */
	current_execute_data = EG(current_execute_data);
	execute_data = zend_create_generator_execute_data(call, op_array, return_value);
	EG(current_execute_data) = current_execute_data;

	object_init_ex(return_value, zend_ce_generator);

	if (Z_OBJ(call->This)) {
		Z_ADDREF(call->This);
	}

	/* Save execution context in generator object. */
	generator = (zend_generator *) Z_OBJ_P(return_value);
	execute_data->prev_execute_data = NULL;
	generator->execute_data = execute_data;
	generator->stack = EG(vm_stack);
	generator->stack->top = EG(vm_stack_top);
	EG(vm_stack_top) = current_stack->top;
	EG(vm_stack_end) = current_stack->end;
	EG(vm_stack) = current_stack;

	/* EX(return_value) keeps pointer to zend_object (not a real zval) */
	execute_data->return_value = (zval*)generator;
}
/* }}} */

static zend_function *zend_generator_get_constructor(zend_object *object) /* {{{ */
{
	zend_error(E_EXCEPTION | E_ERROR, "The \"Generator\" class is reserved for internal use and cannot be manually instantiated");

	return NULL;
}
/* }}} */

ZEND_API void zend_generator_resume(zend_generator *generator) /* {{{ */
{
	/* The generator is already closed, thus can't resume */
	if (!generator->execute_data) {
		return;
	}

	if (generator->flags & ZEND_GENERATOR_CURRENTLY_RUNNING) {
		zend_error(E_ERROR, "Cannot resume an already running generator");
	}

	/* Drop the AT_FIRST_YIELD flag */
	generator->flags &= ~ZEND_GENERATOR_AT_FIRST_YIELD;

	{
		/* Backup executor globals */
		zend_execute_data *original_execute_data = EG(current_execute_data);
		zend_class_entry *original_scope = EG(scope);
		zend_vm_stack original_stack = EG(vm_stack);
		original_stack->top = EG(vm_stack_top);

		/* Set executor globals */
		EG(current_execute_data) = generator->execute_data;
		EG(scope) = generator->execute_data->func->common.scope;
		EG(vm_stack_top) = generator->stack->top;
		EG(vm_stack_end) = generator->stack->end;
		EG(vm_stack) = generator->stack;

		/* We want the backtrace to look as if the generator function was
		 * called from whatever method we are current running (e.g. next()).
		 * So we have to link generator call frame with caller call frame. */
		generator->execute_data->prev_execute_data = original_execute_data;

		/* Resume execution */
		generator->flags |= ZEND_GENERATOR_CURRENTLY_RUNNING;
		zend_execute_ex(generator->execute_data);
		generator->flags &= ~ZEND_GENERATOR_CURRENTLY_RUNNING;

		/* Unlink generator call_frame from the caller and backup vm_stack_top */
		if (generator->execute_data) {
			generator->stack = EG(vm_stack);
			generator->stack->top = EG(vm_stack_top);
			generator->execute_data->prev_execute_data = NULL;
		}

		/* Restore executor globals */
		EG(current_execute_data) = original_execute_data;
		EG(scope) = original_scope;
		EG(vm_stack_top) = original_stack->top;
		EG(vm_stack_end) = original_stack->end;
		EG(vm_stack) = original_stack;

		/* If an exception was thrown in the generator we have to internally
		 * rethrow it in the parent scope. */
		if (UNEXPECTED(EG(exception) != NULL)) {
			zend_throw_exception_internal(NULL);
		}
	}
}
/* }}} */

static void zend_generator_ensure_initialized(zend_generator *generator) /* {{{ */
{
	if (generator->execute_data && Z_TYPE(generator->value) == IS_UNDEF) {
		zend_generator_resume(generator);
		generator->flags |= ZEND_GENERATOR_AT_FIRST_YIELD;
	}
}
/* }}} */

static void zend_generator_rewind(zend_generator *generator) /* {{{ */
{
	zend_generator_ensure_initialized(generator);

	if (!(generator->flags & ZEND_GENERATOR_AT_FIRST_YIELD)) {
		zend_throw_exception(NULL, "Cannot rewind a generator that was already run", 0);
	}
}
/* }}} */

/* {{{ proto void Generator::rewind()
 * Rewind the generator */
ZEND_METHOD(Generator, rewind)
{
	zend_generator *generator;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	generator = (zend_generator *) Z_OBJ_P(getThis());

	zend_generator_rewind(generator);
}
/* }}} */

/* {{{ proto bool Generator::valid()
 * Check whether the generator is valid */
ZEND_METHOD(Generator, valid)
{
	zend_generator *generator;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	generator = (zend_generator *) Z_OBJ_P(getThis());

	zend_generator_ensure_initialized(generator);

	RETURN_BOOL(Z_TYPE(generator->value) != IS_UNDEF);
}
/* }}} */

/* {{{ proto mixed Generator::current()
 * Get the current value */
ZEND_METHOD(Generator, current)
{
	zend_generator *generator;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	generator = (zend_generator *) Z_OBJ_P(getThis());

	zend_generator_ensure_initialized(generator);

	if (Z_TYPE(generator->value) != IS_UNDEF) {
		RETURN_ZVAL_FAST(&generator->value);
	}
}
/* }}} */

/* {{{ proto mixed Generator::key()
 * Get the current key */
ZEND_METHOD(Generator, key)
{
	zend_generator *generator;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	generator = (zend_generator *) Z_OBJ_P(getThis());

	zend_generator_ensure_initialized(generator);

	if (Z_TYPE(generator->key) != IS_UNDEF) {
		RETURN_ZVAL_FAST(&generator->key);
	}
}
/* }}} */

/* {{{ proto void Generator::next()
 * Advances the generator */
ZEND_METHOD(Generator, next)
{
	zend_generator *generator;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	generator = (zend_generator *) Z_OBJ_P(getThis());

	zend_generator_ensure_initialized(generator);

	zend_generator_resume(generator);
}
/* }}} */

/* {{{ proto mixed Generator::send(mixed $value)
 * Sends a value to the generator */
ZEND_METHOD(Generator, send)
{
	zval *value;
	zend_generator *generator;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
		return;
	}

	generator = (zend_generator *) Z_OBJ_P(getThis());

	zend_generator_ensure_initialized(generator);

	/* The generator is already closed, thus can't send anything */
	if (!generator->execute_data) {
		return;
	}

	/* Put sent value in the target VAR slot, if it is used */
	if (generator->send_target) {
		if (Z_REFCOUNTED_P(generator->send_target)) Z_DELREF_P(generator->send_target);
		ZVAL_COPY(generator->send_target, value);
	}

	zend_generator_resume(generator);

	if (Z_TYPE(generator->value) != IS_UNDEF) {
		RETURN_ZVAL_FAST(&generator->value);
	}
}
/* }}} */

/* {{{ proto mixed Generator::throw(Exception $exception)
 * Throws an exception into the generator */
ZEND_METHOD(Generator, throw)
{
	zval *exception, exception_copy;
	zend_generator *generator;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &exception) == FAILURE) {
		return;
	}

	ZVAL_DUP(&exception_copy, exception);

	generator = (zend_generator *) Z_OBJ_P(getThis());

	zend_generator_ensure_initialized(generator);

	if (generator->execute_data) {
		/* Throw the exception in the context of the generator */
		zend_execute_data *current_execute_data = EG(current_execute_data);
		EG(current_execute_data) = generator->execute_data;

		zend_throw_exception_object(&exception_copy);

		EG(current_execute_data) = current_execute_data;

		zend_generator_resume(generator);

		if (Z_TYPE(generator->value) != IS_UNDEF) {
			RETURN_ZVAL_FAST(&generator->value);
		}
	} else {
		/* If the generator is already closed throw the exception in the
		 * current context */
		zend_throw_exception_object(&exception_copy);
	}
}
/* }}} */

/* {{{ proto mixed Generator::getReturn()
 * Retrieves the return value of the generator */
ZEND_METHOD(Generator, getReturn)
{
	zend_generator *generator;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	generator = (zend_generator *) Z_OBJ_P(getThis());

	zend_generator_ensure_initialized(generator);
	if (EG(exception)) {
		return;
	}

	if (Z_ISUNDEF(generator->retval)) {
		/* Generator hasn't returned yet -> error! */
		zend_throw_exception(NULL,
			"Cannot get return value of a generator that hasn't returned", 0);
		return;
	}

	ZVAL_COPY(return_value, &generator->retval);
}
/* }}} */

/* {{{ proto void Generator::__wakeup()
 * Throws an Exception as generators can't be serialized */
ZEND_METHOD(Generator, __wakeup)
{
	/* Just specifying the zend_class_unserialize_deny handler is not enough,
	 * because it is only invoked for C unserialization. For O the error has
	 * to be thrown in __wakeup. */

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_throw_exception(NULL, "Unserialization of 'Generator' is not allowed", 0);
}
/* }}} */

/* get_iterator implementation */

static void zend_generator_iterator_dtor(zend_object_iterator *iterator) /* {{{ */
{
	zend_generator *generator = (zend_generator*)Z_OBJ(iterator->data);
	generator->iterator = NULL;
	zval_ptr_dtor(&iterator->data);
	zend_iterator_dtor(iterator);
}
/* }}} */

static int zend_generator_iterator_valid(zend_object_iterator *iterator) /* {{{ */
{
	zend_generator *generator = (zend_generator*)Z_OBJ(iterator->data);

	zend_generator_ensure_initialized(generator);

	return Z_TYPE(generator->value) != IS_UNDEF ? SUCCESS : FAILURE;
}
/* }}} */

static zval *zend_generator_iterator_get_data(zend_object_iterator *iterator) /* {{{ */
{
	zend_generator *generator = (zend_generator*)Z_OBJ(iterator->data);

	zend_generator_ensure_initialized(generator);

	return &generator->value;
}
/* }}} */

static void zend_generator_iterator_get_key(zend_object_iterator *iterator, zval *key) /* {{{ */
{
	zend_generator *generator = (zend_generator*)Z_OBJ(iterator->data);

	zend_generator_ensure_initialized(generator);

	if (Z_TYPE(generator->key) != IS_UNDEF) {
		ZVAL_ZVAL(key, &generator->key, 1, 0);
	} else {
		ZVAL_NULL(key);
	}
}
/* }}} */

static void zend_generator_iterator_move_forward(zend_object_iterator *iterator) /* {{{ */
{
	zend_generator *generator = (zend_generator*)Z_OBJ(iterator->data);

	zend_generator_ensure_initialized(generator);

	zend_generator_resume(generator);
}
/* }}} */

static void zend_generator_iterator_rewind(zend_object_iterator *iterator) /* {{{ */
{
	zend_generator *generator = (zend_generator*)Z_OBJ(iterator->data);

	zend_generator_rewind(generator);
}
/* }}} */

static zend_object_iterator_funcs zend_generator_iterator_functions = {
	zend_generator_iterator_dtor,
	zend_generator_iterator_valid,
	zend_generator_iterator_get_data,
	zend_generator_iterator_get_key,
	zend_generator_iterator_move_forward,
	zend_generator_iterator_rewind
};

zend_object_iterator *zend_generator_get_iterator(zend_class_entry *ce, zval *object, int by_ref) /* {{{ */
{
	zend_object_iterator *iterator;
	zend_generator *generator = (zend_generator*)Z_OBJ_P(object);

	if (!generator->execute_data) {
		zend_throw_exception(NULL, "Cannot traverse an already closed generator", 0);
		return NULL;
	}

	if (by_ref && !(generator->execute_data->func->op_array.fn_flags & ZEND_ACC_RETURN_REFERENCE)) {
		zend_throw_exception(NULL, "You can only iterate a generator by-reference if it declared that it yields by-reference", 0);
		return NULL;
	}

	iterator = generator->iterator = emalloc(sizeof(zend_object_iterator));

	zend_iterator_init(iterator);

	iterator->funcs = &zend_generator_iterator_functions;
	ZVAL_COPY(&iterator->data, object);

	return iterator;
}
/* }}} */

ZEND_BEGIN_ARG_INFO(arginfo_generator_void, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_generator_send, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_generator_throw, 0, 0, 1)
	ZEND_ARG_INFO(0, exception)
ZEND_END_ARG_INFO()

static const zend_function_entry generator_functions[] = {
	ZEND_ME(Generator, rewind,   arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, valid,    arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, current,  arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, key,      arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, next,     arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, send,     arginfo_generator_send, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, throw,    arginfo_generator_throw, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, getReturn,arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, __wakeup, arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

void zend_register_generator_ce(void) /* {{{ */
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "Generator", generator_functions);
	zend_ce_generator = zend_register_internal_class(&ce);
	zend_ce_generator->ce_flags |= ZEND_ACC_FINAL;
	zend_ce_generator->create_object = zend_generator_create;
	zend_ce_generator->serialize = zend_class_serialize_deny;
	zend_ce_generator->unserialize = zend_class_unserialize_deny;

	/* get_iterator has to be assigned *after* implementing the inferface */
	zend_class_implements(zend_ce_generator, 1, zend_ce_iterator);
	zend_ce_generator->get_iterator = zend_generator_get_iterator;
	zend_ce_generator->iterator_funcs.funcs = &zend_generator_iterator_functions;

	memcpy(&zend_generator_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	zend_generator_handlers.free_obj = zend_generator_free_storage;
	zend_generator_handlers.dtor_obj = zend_generator_dtor_storage;
	zend_generator_handlers.clone_obj = NULL;
	zend_generator_handlers.get_constructor = zend_generator_get_constructor;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
