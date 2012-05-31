/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2012 Zend Technologies Ltd. (http://www.zend.com) |
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
#include "zend_generators.h"

ZEND_API zend_class_entry *zend_ce_generator;
static zend_object_handlers zend_generator_handlers;

void zend_generator_close(zend_generator *generator, zend_bool finished_execution TSRMLS_DC) /* {{{ */
{
	if (generator->execute_data) {
		zend_execute_data *execute_data = generator->execute_data;

		if (!execute_data->symbol_table) {
			zend_free_compiled_variables(execute_data->CVs, execute_data->op_array->last_var);
		} else {
			if (EG(symtable_cache_ptr) >= EG(symtable_cache_limit)) {
				zend_hash_destroy(execute_data->symbol_table);
				FREE_HASHTABLE(execute_data->symbol_table);
			} else {
				zend_hash_clean(execute_data->symbol_table);
				*(++EG(symtable_cache_ptr)) = execute_data->symbol_table;
			}
		}

		if (execute_data->current_this) {
			zval_ptr_dtor(&execute_data->current_this);
		}

		/* If the generator is closed before it can finish execution (reach
		 * a return statement) we have to free loop variables manually, as
		 * we don't know whether the SWITCH_FREE / FREE opcodes have run */
		if (!finished_execution) {
			zend_op_array *op_array = execute_data->op_array;
			zend_uint op_num = execute_data->opline - op_array->opcodes;

			int i;
			for (i = 0; i < op_array->last_brk_cont; ++i) {
				zend_brk_cont_element *brk_cont = op_array->brk_cont_array + i;

				if (brk_cont->start < 0) {
					continue;
				} else if (brk_cont->start > op_num) {
					break;
				} else if (brk_cont->brk > op_num) {
					zend_op *brk_opline = op_array->opcodes + brk_cont->brk;

					switch (brk_opline->opcode) {
						case ZEND_SWITCH_FREE:
							{
								temp_variable *var = (temp_variable *) ((char *) execute_data->Ts + brk_opline->op1.var);
								zval_ptr_dtor(&var->var.ptr);
							}
							break;
						case ZEND_FREE:
							{
								temp_variable *var = (temp_variable *) ((char *) execute_data->Ts + brk_opline->op1.var);
								zval_dtor(&var->tmp_var);
							}
							break;
					}
				}
			}
		}

		/* Clear any backed up stack arguments */
		if (generator->backed_up_stack) {
			zval **zvals = (zval **) generator->backed_up_stack;
			size_t zval_num = generator->backed_up_stack_size / sizeof(zval *);
			int i;

			for (i = 0; i < zval_num; i++) {
				zval_ptr_dtor(&zvals[i]);
			}

			efree(generator->backed_up_stack);
		}

		efree(execute_data);
		generator->execute_data = NULL;
	}

	if (generator->value) {
		zval_ptr_dtor(&generator->value);
		generator->value = NULL;
	}

	if (generator->key) {
		zval_ptr_dtor(&generator->key);
		generator->key = NULL;
	}
}
/* }}} */

static void zend_generator_free_storage(zend_generator *generator TSRMLS_DC) /* {{{ */
{
	zend_generator_close(generator, 0 TSRMLS_CC);

	zend_object_std_dtor(&generator->std TSRMLS_CC);
	efree(generator);
}
/* }}} */

static zend_object_value zend_generator_create(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	zend_generator *generator;
	zend_object_value object;

	generator = emalloc(sizeof(zend_generator));
	memset(generator, 0, sizeof(zend_generator));

	/* The key will be incremented on first use, so it'll start at 0 */
	generator->largest_used_integer_key = -1;

	zend_object_std_init(&generator->std, class_type TSRMLS_CC);

	object.handle = zend_objects_store_put(generator, NULL,
		(zend_objects_free_object_storage_t) zend_generator_free_storage,
		NULL /* no clone handler for now */
		TSRMLS_CC
	);
	object.handlers = &zend_generator_handlers;

	return object;
}
/* }}} */

static zend_function *zend_generator_get_constructor(zval *object TSRMLS_DC) /* {{{ */
{
	zend_error(E_RECOVERABLE_ERROR, "The \"Generator\" class is reserved for internal use and cannot be manually instantiated");

	return NULL;
}
/* }}} */

static void zend_generator_resume(zval *object, zend_generator *generator TSRMLS_DC) /* {{{ */
{
	/* The generator is already closed, thus can't resume */
	if (!generator->execute_data) {
		return;
	}

	{
		/* Backup executor globals */
		zval **original_return_value_ptr_ptr = EG(return_value_ptr_ptr);
		zend_execute_data *original_execute_data = EG(current_execute_data);
		zend_op **original_opline_ptr = EG(opline_ptr);
		zend_op_array *original_active_op_array = EG(active_op_array);
		HashTable *original_active_symbol_table = EG(active_symbol_table);
		zval *original_This = EG(This);
		zend_class_entry *original_scope = EG(scope);
		zend_class_entry *original_called_scope = EG(called_scope);

		/* Remember the current stack position so we can back up pushed args */
		generator->original_stack_top = zend_vm_stack_top(TSRMLS_C);

		/* If there is a backed up stack copy it to the VM stack */
		if (generator->backed_up_stack) {
			void *stack = zend_vm_stack_alloc(generator->backed_up_stack_size TSRMLS_CC);
			memcpy(stack, generator->backed_up_stack, generator->backed_up_stack_size);
			efree(generator->backed_up_stack);
			generator->backed_up_stack = NULL;
		}

		/* We (mis)use the return_value_ptr_ptr to provide the generator object
		 * to the executor, so YIELD will be able to set the yielded value */
		EG(return_value_ptr_ptr) = &object;

		/* Set executor globals */
		EG(current_execute_data) = generator->execute_data;
		EG(opline_ptr) = &generator->execute_data->opline;
		EG(active_op_array) = generator->execute_data->op_array;
		EG(active_symbol_table) = generator->execute_data->symbol_table;
		EG(This) = generator->execute_data->current_this;
		EG(scope) = generator->execute_data->current_scope;
		EG(called_scope) = generator->execute_data->current_called_scope;

		/* Go to next opcode (we don't want to run the last one again) */
		generator->execute_data->opline++;

		/* Resume execution */
		execute_ex(generator->execute_data TSRMLS_CC);

		/* Restore executor globals */
		EG(return_value_ptr_ptr) = original_return_value_ptr_ptr;
		EG(current_execute_data) = original_execute_data;
		EG(opline_ptr) = original_opline_ptr;
		EG(active_op_array) = original_active_op_array;
		EG(active_symbol_table) = original_active_symbol_table;
		EG(This) = original_This;
		EG(scope) = original_scope;
		EG(called_scope) = original_called_scope;

		/* The stack top before and after the execution differ, i.e. there are
		 * arguments pushed to the stack. */
		if (generator->original_stack_top != zend_vm_stack_top(TSRMLS_C)) {
			generator->backed_up_stack_size = (zend_vm_stack_top(TSRMLS_C) - generator->original_stack_top) * sizeof(void *);
			generator->backed_up_stack = emalloc(generator->backed_up_stack_size);
			memcpy(generator->backed_up_stack, generator->original_stack_top, generator->backed_up_stack_size);
			zend_vm_stack_free(generator->original_stack_top TSRMLS_CC);
		}
	}
}
/* }}} */

static void zend_generator_ensure_initialized(zval *object, zend_generator *generator TSRMLS_DC) /* {{{ */
{
	if (!generator->value) {
		zend_generator_resume(object, generator TSRMLS_CC);
	}
}
/* }}} */

/* {{{ proto void Generator::rewind()
 * Rewind the generator */
ZEND_METHOD(Generator, rewind)
{
	zval *object;
	zend_generator *generator;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	object = getThis();
	generator = (zend_generator *) zend_object_store_get_object(object TSRMLS_CC);

	zend_generator_ensure_initialized(object, generator TSRMLS_CC);

	/* Generators aren't rewindable, so rewind() only has to make sure that
	 * the generator is initialized, nothing more */
}
/* }}} */

/* {{{ proto bool Generator::valid()
 * Check whether the generator is valid */
ZEND_METHOD(Generator, valid)
{
	zval *object;
	zend_generator *generator;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	object = getThis();
	generator = (zend_generator *) zend_object_store_get_object(object TSRMLS_CC);

	zend_generator_ensure_initialized(object, generator TSRMLS_CC);

	RETURN_BOOL(generator->value != NULL);
}
/* }}} */

/* {{{ proto mixed Generator::current()
 * Get the current value */
ZEND_METHOD(Generator, current)
{
	zval *object;
	zend_generator *generator;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	object = getThis();
	generator = (zend_generator *) zend_object_store_get_object(object TSRMLS_CC);

	zend_generator_ensure_initialized(object, generator TSRMLS_CC);

	if (generator->value) {
		RETURN_ZVAL(generator->value, 1, 0);
	}
}
/* }}} */

/* {{{ proto mixed Generator::key()
 * Get the current key */
ZEND_METHOD(Generator, key)
{
	zval *object;
	zend_generator *generator;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	object = getThis();
	generator = (zend_generator *) zend_object_store_get_object(object TSRMLS_CC);

	zend_generator_ensure_initialized(object, generator TSRMLS_CC);

	if (generator->key) {
		RETURN_ZVAL(generator->key, 1, 0);
	}
}
/* }}} */

/* {{{ proto void Generator::next()
 * Advances the generator */
ZEND_METHOD(Generator, next)
{
	zval *object;
	zend_generator *generator;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	object = getThis();
	generator = (zend_generator *) zend_object_store_get_object(object TSRMLS_CC);

	zend_generator_ensure_initialized(object, generator TSRMLS_CC);

	zend_generator_resume(object, generator TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed Generator::send()
 * Sends a value to the generator */
ZEND_METHOD(Generator, send)
{
	zval *object, *value;
	zend_generator *generator;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
		return;
	}

	object = getThis();
	generator = (zend_generator *) zend_object_store_get_object(object TSRMLS_CC);

	zend_generator_ensure_initialized(object, generator TSRMLS_CC); 

	/* The generator is already closed, thus can't send anything */
	if (!generator->execute_data) {
		return;
	}

	/* The sent value was initialized to NULL, so dtor that */
	zval_ptr_dtor(generator->send_target->var.ptr_ptr);

	/* Set new sent value */
	Z_ADDREF_P(value);
	generator->send_target->var.ptr = value;
	generator->send_target->var.ptr_ptr = &value;

	zend_generator_resume(object, generator TSRMLS_CC);

	if (generator->value) {
		RETURN_ZVAL(generator->value, 1, 0);
	}
}

/* {{{ proto void Generator::close()
 * Closes the generator */
ZEND_METHOD(Generator, close)
{
	zend_generator *generator;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	generator = (zend_generator *) zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_generator_close(generator, 0);
}
/* }}} */

ZEND_BEGIN_ARG_INFO(arginfo_generator_void, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_generator_send, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static const zend_function_entry generator_functions[] = {
	ZEND_ME(Generator, rewind,  arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, valid,   arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, current, arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, key,     arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, next,    arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, send,    arginfo_generator_send, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, close,   arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

void zend_register_generator_ce(TSRMLS_D) /* {{{ */
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "Generator", generator_functions);
	zend_ce_generator = zend_register_internal_class(&ce TSRMLS_CC);
	zend_ce_generator->ce_flags |= ZEND_ACC_FINAL_CLASS;
	zend_ce_generator->create_object = zend_generator_create;

	zend_class_implements(zend_ce_generator TSRMLS_CC, 1, zend_ce_iterator);

	memcpy(&zend_generator_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
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
