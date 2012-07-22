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
#include "zend_exceptions.h"
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
			zend_clean_and_cache_symbol_table(execute_data->symbol_table TSRMLS_CC);
		}

		if (execute_data->current_this) {
			zval_ptr_dtor(&execute_data->current_this);
		}

		if (execute_data->object) {
			zval_ptr_dtor(&execute_data->object);
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

		/* We have added an additional stack frame in prev_execute_data, so we
		 * have to free it. It also contains the arguments passed to the
		 * generator (for func_get_args) so those have to be freed too. */
		{
			zend_execute_data *prev_execute_data = execute_data->prev_execute_data;
			void **arguments = prev_execute_data->function_state.arguments;

			if (arguments) {
				int arguments_count = (int) (zend_uintptr_t) *arguments;
				zval **arguments_start = (zval **) (arguments - arguments_count);
				int i;

				for (i = 0; i < arguments_count; ++i) {
					zval_ptr_dtor(arguments_start + i);
				}

				efree(arguments_start);
			}

			efree(prev_execute_data);
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

static void zend_generator_clone_storage(zend_generator *orig, zend_generator **clone_ptr) /* {{{ */
{
	zend_generator *clone = emalloc(sizeof(zend_generator));
	memcpy(clone, orig, sizeof(zend_generator));

	if (orig->execute_data) {
		/* Create a few shorter aliases to the old execution data */
		zend_execute_data *execute_data = orig->execute_data;
		zend_op_array *op_array = execute_data->op_array;
		HashTable *symbol_table = execute_data->symbol_table;

		/* Alloc separate execution context, as well as separate sections for
		 * compiled variables and temporary variables */
		size_t execute_data_size = ZEND_MM_ALIGNED_SIZE(sizeof(zend_execute_data));
		size_t CVs_size = ZEND_MM_ALIGNED_SIZE(sizeof(zval **) * op_array->last_var * (symbol_table ? 1 : 2));
		size_t Ts_size = ZEND_MM_ALIGNED_SIZE(sizeof(temp_variable)) * op_array->T;
		size_t total_size = execute_data_size + CVs_size + Ts_size;

		clone->execute_data = emalloc(total_size);

		/* Copy the zend_execute_data struct */
		memcpy(clone->execute_data, execute_data, execute_data_size);

		/* Set the pointers to the memory segments for the compiled and
		 * temporary variables (which are located after the execute_data) */
		clone->execute_data->CVs = (zval ***) ((char *) clone->execute_data + execute_data_size);
		clone->execute_data->Ts = (temp_variable *) ((char *) clone->execute_data->CVs + CVs_size);

		/* Zero out the compiled variables section */
		memset(clone->execute_data->CVs, 0, sizeof(zval **) * op_array->last_var);

		if (!symbol_table) {
			int i;

			/* Copy compiled variables */
			for (i = 0; i < op_array->last_var; i++) {
				if (execute_data->CVs[i]) {
					clone->execute_data->CVs[i] = (zval **) clone->execute_data->CVs + op_array->last_var + i;
					*clone->execute_data->CVs[i] = (zval *) orig->execute_data->CVs[op_array->last_var + i];
					Z_ADDREF_PP(clone->execute_data->CVs[i]);
				}
			}
		} else {
			/* Copy symbol table */
			ALLOC_HASHTABLE(clone->execute_data->symbol_table);
			zend_hash_init(clone->execute_data->symbol_table, zend_hash_num_elements(symbol_table), NULL, ZVAL_PTR_DTOR, 0);
			zend_hash_copy(clone->execute_data->symbol_table, symbol_table, (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval *));

			/* Update zval** pointers for compiled variables */
			{
				int i;
				for (i = 0; i < op_array->last_var; i++) {
					if (zend_hash_quick_find(clone->execute_data->symbol_table, op_array->vars[i].name, op_array->vars[i].name_len + 1, op_array->vars[i].hash_value, (void **) &clone->execute_data->CVs[i]) == FAILURE) {
						clone->execute_data->CVs[i] = NULL;
					}
				}
			}
		}

		/* Copy the temporary variables */
		memcpy(clone->execute_data->Ts, orig->execute_data->Ts, Ts_size);

		/* Add references to loop variables */
		{
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

					if (brk_opline->opcode == ZEND_SWITCH_FREE) {
						temp_variable *var = (temp_variable *) ((char *) execute_data->Ts + brk_opline->op1.var);

						Z_ADDREF_P(var->var.ptr);
					}
				}
			}
		}

		if (orig->backed_up_stack) {
			/* Copy backed up stack */
			clone->backed_up_stack = emalloc(orig->backed_up_stack_size);
			memcpy(clone->backed_up_stack, orig->backed_up_stack, orig->backed_up_stack_size);

			/* Add refs to stack variables */
			{
				zval **zvals = (zval **) orig->backed_up_stack;
				size_t zval_num = orig->backed_up_stack_size / sizeof(zval *);
				int i;

				for (i = 0; i < zval_num; i++) {
					Z_ADDREF_P(zvals[i]);
				}
			}
		}

		/* Update the send_target to use the temporary variable with the same
		 * offset as the original generator, but in our temporary variable
		 * memory segment. */
		if (orig->send_target) {
			size_t offset = (char *) orig->send_target - (char *) execute_data->Ts;
			clone->send_target = (temp_variable *) (
				(char *) clone->execute_data->Ts + offset
			);
			Z_ADDREF_P(clone->send_target->var.ptr);
		}

		if (execute_data->current_this) {
			Z_ADDREF_P(execute_data->current_this);
		}

		if (execute_data->object) {
			Z_ADDREF_P(execute_data->object);
		}

		/* Prev execute data contains an additional stack frame (for proper)
		 * backtraces) which has to be copied. */
		clone->execute_data->prev_execute_data = emalloc(sizeof(zend_execute_data));
		memcpy(clone->execute_data->prev_execute_data, execute_data->prev_execute_data, sizeof(zend_execute_data));

		/* It also contains the arguments passed to the generator, which also
		 * have to be copied */
		if (execute_data->prev_execute_data->function_state.arguments) {
			clone->execute_data->prev_execute_data->function_state.arguments
				= zend_copy_arguments(execute_data->prev_execute_data->function_state.arguments);
		}
	}

	/* The value and key are known not to be references, so simply add refs */
	if (orig->value) {
		Z_ADDREF_P(orig->value);
	}

	if (orig->key) {
		Z_ADDREF_P(orig->key);
	}

	*clone_ptr = clone;
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

	generator->is_currently_running = 0;

	zend_object_std_init(&generator->std, class_type TSRMLS_CC);

	object.handle = zend_objects_store_put(generator, NULL,
		(zend_objects_free_object_storage_t) zend_generator_free_storage,
		(zend_objects_store_clone_t)         zend_generator_clone_storage
		TSRMLS_CC
	);
	object.handlers = &zend_generator_handlers;

	return object;
}
/* }}} */

/* Requires globals EG(scope), EG(current_scope), EG(This),
 * EG(active_symbol_table) and EG(current_execute_data). */
zval *zend_generator_create_zval(zend_op_array *op_array TSRMLS_DC) /* {{{ */
{
	zval *return_value;
	zend_generator *generator;

	/* Create new execution context. We have to back up and restore
	 * EG(current_execute_data) and EG(opline_ptr) here because the function
	 * modifies it. */
	zend_execute_data *current_execute_data = EG(current_execute_data);
	zend_op **opline_ptr = EG(opline_ptr);
	zend_execute_data *execute_data = zend_create_execute_data_from_op_array(op_array, 0 TSRMLS_CC);
	EG(current_execute_data) = current_execute_data;
	EG(opline_ptr) = opline_ptr;

	ALLOC_INIT_ZVAL(return_value);
	object_init_ex(return_value, zend_ce_generator);

	if (EG(This)) {
		Z_ADDREF_P(EG(This));
	}

	/* Back up executor globals. */
	execute_data->current_scope = EG(scope);
	execute_data->current_called_scope = EG(called_scope);
	execute_data->symbol_table = EG(active_symbol_table);
	execute_data->current_this = EG(This);

	/* Save execution context in generator object. */
	generator = (zend_generator *) zend_object_store_get_object(return_value TSRMLS_CC);
	generator->execute_data = execute_data;

	/* We have to add another stack frame so the generator function shows
	 * up in backtraces and func_get_all() can access the function
	 * arguments. */
	execute_data->prev_execute_data = emalloc(sizeof(zend_execute_data));
	memset(execute_data->prev_execute_data, 0, sizeof(zend_execute_data));
	execute_data->prev_execute_data->function_state.function = (zend_function *) op_array;
	if (EG(current_execute_data)) {
		execute_data->prev_execute_data->function_state.arguments = zend_copy_arguments(EG(current_execute_data)->function_state.arguments);
	} else {
		execute_data->prev_execute_data->function_state.arguments = NULL;
	}

	return return_value;
}
/* }}} */

static zend_function *zend_generator_get_constructor(zval *object TSRMLS_DC) /* {{{ */
{
	zend_error(E_RECOVERABLE_ERROR, "The \"Generator\" class is reserved for internal use and cannot be manually instantiated");

	return NULL;
}
/* }}} */

static void zend_generator_resume(zend_generator *generator TSRMLS_DC) /* {{{ */
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

		zend_bool original_is_currently_running = generator->is_currently_running;

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
		EG(return_value_ptr_ptr) = (zval **) generator;

		/* Set executor globals */
		EG(current_execute_data) = generator->execute_data;
		EG(opline_ptr) = &generator->execute_data->opline;
		EG(active_op_array) = generator->execute_data->op_array;
		EG(active_symbol_table) = generator->execute_data->symbol_table;
		EG(This) = generator->execute_data->current_this;
		EG(scope) = generator->execute_data->current_scope;
		EG(called_scope) = generator->execute_data->current_called_scope;

		generator->is_currently_running = 1;

		/* We want the backtrace to look as if the generator function was
		 * called from whatever method we are current running (e.g. next()).
		 * The first prev_execute_data contains an additional stack frame,
		 * which makes the generator function show up in the backtrace and
		 * makes the arguments available to func_get_args(). So we have to
		 * set the prev_execute_data of that prev_execute_data :) */
		generator->execute_data->prev_execute_data->prev_execute_data = original_execute_data;

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

		generator->is_currently_running = original_is_currently_running;

		/* The stack top before and after the execution differ, i.e. there are
		 * arguments pushed to the stack. */
		if (generator->original_stack_top != zend_vm_stack_top(TSRMLS_C)) {
			generator->backed_up_stack_size = (zend_vm_stack_top(TSRMLS_C) - generator->original_stack_top) * sizeof(void *);
			generator->backed_up_stack = emalloc(generator->backed_up_stack_size);
			memcpy(generator->backed_up_stack, generator->original_stack_top, generator->backed_up_stack_size);
			zend_vm_stack_free(generator->original_stack_top TSRMLS_CC);
		}

		/* If an exception was thrown in the generator we have to internally
		 * rethrow it in the parent scope. */
		if (UNEXPECTED(EG(exception) != NULL)) {
			zend_throw_exception_internal(NULL TSRMLS_CC);
		}
	}
}
/* }}} */

static void zend_generator_ensure_initialized(zend_generator *generator TSRMLS_DC) /* {{{ */
{
	if (!generator->value) {
		zend_generator_resume(generator TSRMLS_CC);
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

	generator = (zend_generator *) zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_generator_ensure_initialized(generator TSRMLS_CC);

	/* Generators aren't rewindable, so rewind() only has to make sure that
	 * the generator is initialized, nothing more */
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

	generator = (zend_generator *) zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_generator_ensure_initialized(generator TSRMLS_CC);

	RETURN_BOOL(generator->value != NULL);
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

	generator = (zend_generator *) zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_generator_ensure_initialized(generator TSRMLS_CC);

	if (generator->value) {
		RETURN_ZVAL(generator->value, 1, 0);
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

	generator = (zend_generator *) zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_generator_ensure_initialized(generator TSRMLS_CC);

	if (generator->key) {
		RETURN_ZVAL(generator->key, 1, 0);
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

	generator = (zend_generator *) zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_generator_ensure_initialized(generator TSRMLS_CC);

	zend_generator_resume(generator TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed Generator::send()
 * Sends a value to the generator */
ZEND_METHOD(Generator, send)
{
	zval *value;
	zend_generator *generator;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
		return;
	}

	generator = (zend_generator *) zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_generator_ensure_initialized(generator TSRMLS_CC); 

	/* The generator is already closed, thus can't send anything */
	if (!generator->execute_data) {
		return;
	}

	/* The sent value was initialized to NULL, so dtor that */
	zval_ptr_dtor(&generator->send_target->var.ptr);

	/* Set new sent value */
	Z_ADDREF_P(value);
	generator->send_target->var.ptr = value;
	generator->send_target->var.ptr_ptr = &value;

	zend_generator_resume(generator TSRMLS_CC);

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

	if (generator->is_currently_running) {
		zend_error(E_WARNING, "A generator cannot be closed while it is running");
		return;
	}

	zend_generator_close(generator, 0 TSRMLS_CC);
}
/* }}} */

/* get_iterator implementation */

typedef struct _zend_generator_iterator {
	zend_object_iterator intern;

	/* The generator object zval has to be stored, because the iterator is
	 * holding a ref to it, which has to be dtored. */
	zval *object;
} zend_generator_iterator;

static void zend_generator_iterator_dtor(zend_object_iterator *iterator TSRMLS_DC) /* {{{ */
{
	zval *object = ((zend_generator_iterator *) iterator)->object;

	zval_ptr_dtor(&object);

	efree(iterator);
}
/* }}} */

static int zend_generator_iterator_valid(zend_object_iterator *iterator TSRMLS_DC) /* {{{ */
{
	zend_generator *generator = (zend_generator *) iterator->data;

	zend_generator_ensure_initialized(generator TSRMLS_CC);

	return generator->value != NULL ? SUCCESS : FAILURE;
}
/* }}} */

static void zend_generator_iterator_get_data(zend_object_iterator *iterator, zval ***data TSRMLS_DC) /* {{{ */
{
	zend_generator *generator = (zend_generator *) iterator->data;

	zend_generator_ensure_initialized(generator TSRMLS_CC);

	if (generator->value) {
		*data = &generator->value;
	} else {
		*data = NULL;
	}
}
/* }}} */

static int zend_generator_iterator_get_key(zend_object_iterator *iterator, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC) /* {{{ */
{
	zend_generator *generator = (zend_generator *) iterator->data;

	zend_generator_ensure_initialized(generator TSRMLS_CC);

	if (!generator->key) {
		return HASH_KEY_NON_EXISTANT;
	}

	if (Z_TYPE_P(generator->key) == IS_LONG) {
		*int_key = Z_LVAL_P(generator->key);
		return HASH_KEY_IS_LONG;
	}

	if (Z_TYPE_P(generator->key) == IS_STRING) {
		*str_key = estrndup(Z_STRVAL_P(generator->key), Z_STRLEN_P(generator->key));
		*str_key_len = Z_STRLEN_P(generator->key) + 1;
		return HASH_KEY_IS_STRING;
	}

	/* Waiting for Etienne's patch to allow arbitrary zval keys. Until then
	 * error out on non-int and non-string keys. */
	zend_error_noreturn(E_ERROR, "Currently only int and string keys can be yielded");
}
/* }}} */

static void zend_generator_iterator_move_forward(zend_object_iterator *iterator TSRMLS_DC) /* {{{ */
{
	zend_generator *generator = (zend_generator *) iterator->data;

	zend_generator_ensure_initialized(generator TSRMLS_CC);

	zend_generator_resume(generator TSRMLS_CC);
}
/* }}} */

static zend_object_iterator_funcs zend_generator_iterator_functions = {
	zend_generator_iterator_dtor,
	zend_generator_iterator_valid,
	zend_generator_iterator_get_data,
	zend_generator_iterator_get_key,
	zend_generator_iterator_move_forward,
	NULL
};

zend_object_iterator *zend_generator_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC) /* {{{ */
{
	zend_generator_iterator *iterator;
	zend_generator *generator;

	generator = (zend_generator *) zend_object_store_get_object(object TSRMLS_CC);

	if (by_ref && !(generator->execute_data->op_array->fn_flags & ZEND_ACC_RETURN_REFERENCE)) {
		zend_error(E_ERROR, "You can only iterate a generator by-reference if it declared that it yields by-reference");
	}

	iterator = emalloc(sizeof(zend_generator_iterator));
	iterator->intern.funcs = &zend_generator_iterator_functions;
	iterator->intern.data = (void *) generator;

	/* We have to keep a reference to the generator object zval around,
	 * otherwise the generator may be destroyed during iteration. */
	Z_ADDREF_P(object);
	iterator->object = object;

	return (zend_object_iterator *) iterator;
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
	zend_ce_generator->get_iterator = zend_generator_get_iterator;

	zend_class_implements(zend_ce_generator TSRMLS_CC, 1, zend_ce_iterator);

	memcpy(&zend_generator_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	zend_generator_handlers.get_constructor = zend_generator_get_constructor;
	zend_generator_handlers.clone_obj = zend_objects_store_clone_obj;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
