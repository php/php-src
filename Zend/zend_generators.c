/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
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
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "zend_generators.h"
#include "zend_closures.h"

ZEND_API zend_class_entry *zend_ce_generator;
ZEND_API zend_class_entry *zend_ce_ClosedGeneratorException;
static zend_object_handlers zend_generator_handlers;

static zend_object *zend_generator_create(zend_class_entry *class_type);

ZEND_API void zend_generator_restore_call_stack(zend_generator *generator) /* {{{ */
{
	zend_execute_data *call, *new_call, *prev_call = NULL;

	call = generator->frozen_call_stack;
	do {
		new_call = zend_vm_stack_push_call_frame(
			(ZEND_CALL_INFO(call) & ~ZEND_CALL_ALLOCATED),
			call->func,
			ZEND_CALL_NUM_ARGS(call),
			Z_PTR(call->This));
		memcpy(((zval*)new_call) + ZEND_CALL_FRAME_SLOT, ((zval*)call) + ZEND_CALL_FRAME_SLOT, ZEND_CALL_NUM_ARGS(call) * sizeof(zval));
		new_call->prev_execute_data = prev_call;
		prev_call = new_call;

		call = call->prev_execute_data;
	} while (call);
	generator->execute_data->call = prev_call;
	efree(generator->frozen_call_stack);
	generator->frozen_call_stack = NULL;
}
/* }}} */

ZEND_API zend_execute_data* zend_generator_freeze_call_stack(zend_execute_data *execute_data) /* {{{ */
{
	size_t used_stack;
	zend_execute_data *call, *new_call, *prev_call = NULL;
	zval *stack;

	/* calculate required stack size */
	used_stack = 0;
	call = EX(call);
	do {
		used_stack += ZEND_CALL_FRAME_SLOT + ZEND_CALL_NUM_ARGS(call);
		call = call->prev_execute_data;
	} while (call);

	stack = emalloc(used_stack * sizeof(zval));

	/* save stack, linking frames in reverse order */
	call = EX(call);
	do {
		size_t frame_size = ZEND_CALL_FRAME_SLOT + ZEND_CALL_NUM_ARGS(call);

		new_call = (zend_execute_data*)(stack + used_stack - frame_size);
		memcpy(new_call, call, frame_size * sizeof(zval));
		used_stack -= frame_size;
		new_call->prev_execute_data = prev_call;
		prev_call = new_call;

		new_call = call->prev_execute_data;
		zend_vm_stack_free_call_frame(call);
		call = new_call;
	} while (call);

	execute_data->call = NULL;
	ZEND_ASSERT(prev_call == (zend_execute_data*)stack);

	return prev_call;
}
/* }}} */

static void zend_generator_cleanup_unfinished_execution(
		zend_generator *generator, zend_execute_data *execute_data, uint32_t catch_op_num) /* {{{ */
{
	zend_op_array *op_array = &execute_data->func->op_array;
	if (execute_data->opline != op_array->opcodes) {
		/* -1 required because we want the last run opcode, not the next to-be-run one. */
		uint32_t op_num = execute_data->opline - op_array->opcodes - 1;

		if (UNEXPECTED(generator->frozen_call_stack)) {
			/* Temporarily restore generator->execute_data if it has been NULLed out already. */
			zend_execute_data *save_ex = generator->execute_data;
			generator->execute_data = execute_data;
			zend_generator_restore_call_stack(generator);
			generator->execute_data = save_ex;
		}

		zend_cleanup_unfinished_execution(execute_data, op_num, catch_op_num);
	}
}
/* }}} */

ZEND_API void zend_generator_close(zend_generator *generator, zend_bool finished_execution) /* {{{ */
{
	if (EXPECTED(generator->execute_data)) {
		zend_execute_data *execute_data = generator->execute_data;
		/* Null out execute_data early, to prevent double frees if GC runs while we're
		 * already cleaning up execute_data. */
		generator->execute_data = NULL;

		if (EX_CALL_INFO() & ZEND_CALL_HAS_SYMBOL_TABLE) {
			zend_clean_and_cache_symbol_table(execute_data->symbol_table);
		}
		/* always free the CV's, in the symtable are only not-free'd IS_INDIRECT's */
		zend_free_compiled_variables(execute_data);

		if (EX_CALL_INFO() & ZEND_CALL_RELEASE_THIS) {
			OBJ_RELEASE(Z_OBJ(execute_data->This));
		}

		/* A fatal error / die occurred during the generator execution.
		 * Trying to clean up the stack may not be safe in this case. */
		if (UNEXPECTED(CG(unclean_shutdown))) {
			generator->execute_data = NULL;
			return;
		}

		zend_vm_stack_free_extra_args(execute_data);

		/* Some cleanups are only necessary if the generator was closed
		 * before it could finish execution (reach a return statement). */
		if (UNEXPECTED(!finished_execution)) {
			zend_generator_cleanup_unfinished_execution(generator, execute_data, 0);
		}

		/* Free closure object */
		if (EX_CALL_INFO() & ZEND_CALL_CLOSURE) {
			OBJ_RELEASE(ZEND_CLOSURE_OBJECT(EX(func)));
		}

		/* Free GC buffer. GC for closed generators doesn't need an allocated buffer */
		if (generator->gc_buffer) {
			efree(generator->gc_buffer);
			generator->gc_buffer = NULL;
		}

		efree(execute_data);
	}
}
/* }}} */

static zend_generator *zend_generator_get_child(zend_generator_node *node, zend_generator *leaf);

static void zend_generator_dtor_storage(zend_object *object) /* {{{ */
{
	zend_generator *generator = (zend_generator*) object;
	zend_execute_data *ex = generator->execute_data;
	uint32_t op_num, try_catch_offset;
	int i;

	/* leave yield from mode to properly allow finally execution */
	if (UNEXPECTED(Z_TYPE(generator->values) != IS_UNDEF)) {
		zval_ptr_dtor(&generator->values);
		ZVAL_UNDEF(&generator->values);
	}

	if (EXPECTED(generator->node.children == 0)) {
		zend_generator *root = generator->node.ptr.root, *next;
		while (UNEXPECTED(root != generator)) {
			next = zend_generator_get_child(&root->node, generator);
			generator->node.ptr.root = next;
			next->node.parent = NULL;
			OBJ_RELEASE(&root->std);
			root = next;
		}
	}

	if (EXPECTED(!ex) || EXPECTED(!(ex->func->op_array.fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK))
			|| CG(unclean_shutdown)) {
		return;
	}

	/* -1 required because we want the last run opcode, not the
	 * next to-be-run one. */
	op_num = ex->opline - ex->func->op_array.opcodes - 1;
	try_catch_offset = -1;

	/* Find the innermost try/catch that we are inside of. */
	for (i = 0; i < ex->func->op_array.last_try_catch; i++) {
		zend_try_catch_element *try_catch = &ex->func->op_array.try_catch_array[i];
		if (op_num < try_catch->try_op) {
			break;
		}
		if (op_num < try_catch->catch_op || op_num < try_catch->finally_end) {
			try_catch_offset = i;
		}
	}

	/* Walk try/catch/finally structures upwards, performing the necessary actions. */
	while (try_catch_offset != (uint32_t) -1) {
		zend_try_catch_element *try_catch = &ex->func->op_array.try_catch_array[try_catch_offset];

		if (op_num < try_catch->finally_op) {
			/* Go to finally block */
			zval *fast_call =
				ZEND_CALL_VAR(ex, ex->func->op_array.opcodes[try_catch->finally_end].op1.var);

			zend_generator_cleanup_unfinished_execution(generator, ex, try_catch->finally_op);
			Z_OBJ_P(fast_call) = EG(exception);
			EG(exception) = NULL;
			Z_OPLINE_NUM_P(fast_call) = (uint32_t)-1;

			ex->opline = &ex->func->op_array.opcodes[try_catch->finally_op];
			generator->flags |= ZEND_GENERATOR_FORCED_CLOSE;
			zend_generator_resume(generator);

			/* TODO: If we hit another yield inside try/finally,
			 * should we also jump to the next finally block? */
			return;
		} else if (op_num < try_catch->finally_end) {
			zval *fast_call =
				ZEND_CALL_VAR(ex, ex->func->op_array.opcodes[try_catch->finally_end].op1.var);
			/* Clean up incomplete return statement */
			if (Z_OPLINE_NUM_P(fast_call) != (uint32_t) -1) {
				zend_op *retval_op = &ex->func->op_array.opcodes[Z_OPLINE_NUM_P(fast_call)];
				if (retval_op->op2_type & (IS_TMP_VAR | IS_VAR)) {
					zval_ptr_dtor(ZEND_CALL_VAR(ex, retval_op->op2.var));
				}
			}
			/* Clean up backed-up exception */
			if (Z_OBJ_P(fast_call)) {
				OBJ_RELEASE(Z_OBJ_P(fast_call));
			}
		}

		try_catch_offset--;
	}
}
/* }}} */

static void zend_generator_free_storage(zend_object *object) /* {{{ */
{
	zend_generator *generator = (zend_generator*) object;

	zend_generator_close(generator, 0);

	/* we can't immediately free them in zend_generator_close() else yield from won't be able to fetch it */
	zval_ptr_dtor(&generator->value);
	zval_ptr_dtor(&generator->key);

	if (EXPECTED(!Z_ISUNDEF(generator->retval))) {
		zval_ptr_dtor(&generator->retval);
	}

	if (UNEXPECTED(generator->node.children > 1)) {
		zend_hash_destroy(generator->node.child.ht);
		efree(generator->node.child.ht);
	}

	zend_object_std_dtor(&generator->std);

	if (generator->iterator) {
		zend_iterator_dtor(generator->iterator);
	}
}
/* }}} */

static uint32_t calc_gc_buffer_size(zend_generator *generator) /* {{{ */
{
	uint32_t size = 4; /* value, key, retval, values */
	if (generator->execute_data) {
		zend_execute_data *execute_data = generator->execute_data;
		zend_op_array *op_array = &EX(func)->op_array;

		/* Compiled variables */
		if (!(EX_CALL_INFO() & ZEND_CALL_HAS_SYMBOL_TABLE)) {
			size += op_array->last_var;
		}
		/* Extra args */
		if (EX_CALL_INFO() & ZEND_CALL_FREE_EXTRA_ARGS) {
			size += EX_NUM_ARGS() - op_array->num_args;
		}
		size += (EX_CALL_INFO() & ZEND_CALL_RELEASE_THIS) != 0; /* $this */
		size += (EX_CALL_INFO() & ZEND_CALL_CLOSURE) != 0; /* Closure object */

		/* Live vars */
		if (execute_data->opline != op_array->opcodes) {
			/* -1 required because we want the last run opcode, not the next to-be-run one. */
			uint32_t i, op_num = execute_data->opline - op_array->opcodes - 1;
			for (i = 0; i < op_array->last_live_range; i++) {
				const zend_live_range *range = &op_array->live_range[i];
				if (range->start > op_num) {
					/* Further ranges will not be relevant... */
					break;
				} else if (op_num < range->end) {
					/* LIVE_ROPE and LIVE_SILENCE not relevant for GC */
					uint32_t kind = range->var & ZEND_LIVE_MASK;
					if (kind == ZEND_LIVE_TMPVAR || kind == ZEND_LIVE_LOOP) {
						size++;
					}
				}
			}
		}

		/* Yield from root references */
		if (generator->node.children == 0) {
			zend_generator *root = generator->node.ptr.root;
			while (root != generator) {
				root = zend_generator_get_child(&root->node, generator);
				size++;
			}
		}
	}
	return size;
}
/* }}} */

static HashTable *zend_generator_get_gc(zval *object, zval **table, int *n) /* {{{ */
{
	zend_generator *generator = (zend_generator*) Z_OBJ_P(object);
	zend_execute_data *execute_data = generator->execute_data;
	zend_op_array *op_array;
	zval *gc_buffer;
	uint32_t gc_buffer_size;

	if (!execute_data) {
		/* If the generator has been closed, it can only hold on to three values: The value, key
		 * and retval. These three zvals are stored sequentially starting at &generator->value. */
		*table = &generator->value;
		*n = 3;
		return NULL;
	}

	op_array = &EX(func)->op_array;
	gc_buffer_size = calc_gc_buffer_size(generator);
	if (generator->gc_buffer_size < gc_buffer_size) {
		generator->gc_buffer = safe_erealloc(generator->gc_buffer, sizeof(zval), gc_buffer_size, 0);
		generator->gc_buffer_size = gc_buffer_size;
	}

	*n = gc_buffer_size;
	*table = gc_buffer = generator->gc_buffer;

	ZVAL_COPY_VALUE(gc_buffer++, &generator->value);
	ZVAL_COPY_VALUE(gc_buffer++, &generator->key);
	ZVAL_COPY_VALUE(gc_buffer++, &generator->retval);
	ZVAL_COPY_VALUE(gc_buffer++, &generator->values);

	if (!(EX_CALL_INFO() & ZEND_CALL_HAS_SYMBOL_TABLE)) {
		uint32_t i, num_cvs = EX(func)->op_array.last_var;
		for (i = 0; i < num_cvs; i++) {
			ZVAL_COPY_VALUE(gc_buffer++, EX_VAR_NUM(i));
		}
	}

	if (EX_CALL_INFO() & ZEND_CALL_FREE_EXTRA_ARGS) {
		zval *zv = EX_VAR_NUM(op_array->last_var + op_array->T);
		zval *end = zv + (EX_NUM_ARGS() - op_array->num_args);
		while (zv != end) {
			ZVAL_COPY_VALUE(gc_buffer++, zv++);
		}
	}

	if (EX_CALL_INFO() & ZEND_CALL_RELEASE_THIS) {
		ZVAL_OBJ(gc_buffer++, Z_OBJ(execute_data->This));
	}
	if (EX_CALL_INFO() & ZEND_CALL_CLOSURE) {
		ZVAL_OBJ(gc_buffer++, ZEND_CLOSURE_OBJECT(EX(func)));
	}

	if (execute_data->opline != op_array->opcodes) {
		uint32_t i, op_num = execute_data->opline - op_array->opcodes - 1;
		for (i = 0; i < op_array->last_live_range; i++) {
			const zend_live_range *range = &op_array->live_range[i];
			if (range->start > op_num) {
				break;
			} else if (op_num < range->end) {
				uint32_t kind = range->var & ZEND_LIVE_MASK;
				uint32_t var_num = range->var & ~ZEND_LIVE_MASK;
				zval *var = EX_VAR(var_num);
				if (kind == ZEND_LIVE_TMPVAR || kind == ZEND_LIVE_LOOP) {
					ZVAL_COPY_VALUE(gc_buffer++, var);
				}
			}
		}
	}

	if (generator->node.children == 0) {
		zend_generator *root = generator->node.ptr.root;
		while (root != generator) {
			ZVAL_OBJ(gc_buffer++, &root->std);
			root = zend_generator_get_child(&root->node, generator);
		}
	}

	if (EX_CALL_INFO() & ZEND_CALL_HAS_SYMBOL_TABLE) {
		return execute_data->symbol_table;
	} else {
		return NULL;
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
	ZVAL_UNDEF(&generator->values);

	/* By default we have a tree of only one node */
	generator->node.parent = NULL;
	generator->node.children = 0;
	generator->node.ptr.root = generator;

	zend_object_std_init(&generator->std, class_type);
	generator->std.handlers = &zend_generator_handlers;

	return (zend_object*)generator;
}
/* }}} */

static ZEND_COLD zend_function *zend_generator_get_constructor(zend_object *object) /* {{{ */
{
	zend_throw_error(NULL, "The \"Generator\" class is reserved for internal use and cannot be manually instantiated");

	return NULL;
}
/* }}} */

ZEND_API zend_execute_data *zend_generator_check_placeholder_frame(zend_execute_data *ptr)
{
	if (!ptr->func && Z_TYPE(ptr->This) == IS_OBJECT) {
		if (Z_OBJCE(ptr->This) == zend_ce_generator) {
			zend_generator *generator = (zend_generator *) Z_OBJ(ptr->This);
			zend_generator *root = (generator->node.children < 1 ? generator : generator->node.ptr.leaf)->node.ptr.root;
			zend_execute_data *prev = ptr->prev_execute_data;
			if (generator->node.parent != root) {
				do {
					generator->execute_data->prev_execute_data = prev;
					prev = generator->execute_data;
					generator = generator->node.parent;
				} while (generator->node.parent != root);
			}
			generator->execute_data->prev_execute_data = prev;
			ptr = generator->execute_data;
		}
	}
	return ptr;
}

static void zend_generator_throw_exception(zend_generator *generator, zval *exception)
{
	zend_execute_data *original_execute_data = EG(current_execute_data);

	/* if we don't stop an array/iterator yield from, the exception will only reach the generator after the values were all iterated over */
	if (UNEXPECTED(Z_TYPE(generator->values) != IS_UNDEF)) {
		zval_ptr_dtor(&generator->values);
		ZVAL_UNDEF(&generator->values);
	}

	/* Throw the exception in the context of the generator. Decrementing the opline
	 * to pretend the exception happened during the YIELD opcode. */
	EG(current_execute_data) = generator->execute_data;
	generator->execute_data->opline--;
	if (exception) {
		zend_throw_exception_object(exception);
	} else {
		zend_rethrow_exception(EG(current_execute_data));
	}
	generator->execute_data->opline++;
	EG(current_execute_data) = original_execute_data;
}

static zend_generator *zend_generator_get_child(zend_generator_node *node, zend_generator *leaf)
{
	if (node->children == 0) {
		return NULL;
	} else if (node->children == 1) {
		return node->child.single.child;
	} else {
		return zend_hash_index_find_ptr(node->child.ht, (zend_ulong) leaf);
	}
}

static zend_generator_node *zend_generator_search_multi_children_node(zend_generator_node *node)
{
	while (node->children == 1) {
		node = &node->child.single.child->node;
	}
	return node->children > 1 ? node : NULL;
}

static void zend_generator_add_single_child(zend_generator_node *node, zend_generator *child, zend_generator *leaf)
{
	if (node->children == 0) {
		node->child.single.leaf = leaf;
		node->child.single.child = child;
	} else {
		if (node->children == 1) {
			HashTable *ht = emalloc(sizeof(HashTable));
			zend_hash_init(ht, 0, NULL, NULL, 0);
			zend_hash_index_add_ptr(ht,
				(zend_ulong) node->child.single.leaf, node->child.single.child);
			node->child.ht = ht;
		}

		zend_hash_index_add_ptr(node->child.ht, (zend_ulong) leaf, child);
	}

	node->children++;
}

static void zend_generator_merge_child_nodes(zend_generator_node *dest, zend_generator_node *src, zend_generator *child)
{
	zend_ulong leaf;
	ZEND_ASSERT(src->children > 1);
	ZEND_HASH_FOREACH_NUM_KEY(src->child.ht, leaf) {
		zend_generator_add_single_child(dest, child, (zend_generator *) leaf);
	} ZEND_HASH_FOREACH_END();
}

/* Pay attention so that the root of each subtree of the Generators tree is referenced
 * once per leaf */
static void zend_generator_add_child(zend_generator *generator, zend_generator *child)
{
	zend_generator *leaf = child->node.children ? child->node.ptr.leaf : child;
	zend_generator_node *multi_children_node;
	zend_bool was_leaf = generator->node.children == 0;

	if (was_leaf) {
		zend_generator *next = generator->node.parent;
		leaf->node.ptr.root = generator->node.ptr.root;
		GC_ADDREF(&generator->std); /* we need to increment the generator refcount here as it became integrated into the tree (no leaf), but we must not increment the refcount of the *whole* path in tree */
		generator->node.ptr.leaf = leaf;

		while (next) {
			if (next->node.children > 1) {
				zend_generator *child = zend_hash_index_find_ptr(next->node.child.ht, (zend_ulong) generator);
				zend_hash_index_del(next->node.child.ht, (zend_ulong) generator);
				zend_hash_index_add_ptr(next->node.child.ht, (zend_ulong) leaf, child);
			}

			next->node.ptr.leaf = leaf;
			next = next->node.parent;
		}
	} else if (generator->node.children == 1) {
		multi_children_node = zend_generator_search_multi_children_node(&generator->node);
		if (multi_children_node) {
			generator->node.children = 0;
			zend_generator_merge_child_nodes(&generator->node, multi_children_node, generator->node.child.single.child);
		}
	}

	if (!was_leaf) {
		multi_children_node = zend_generator_search_multi_children_node(&child->node);
	} else {
		multi_children_node = (zend_generator_node *) 0x1;
	}

	{
		zend_generator *parent = generator->node.parent, *cur = generator;

		if (multi_children_node > (zend_generator_node *) 0x1) {
			zend_generator_merge_child_nodes(&generator->node, multi_children_node, child);
		} else {
			zend_generator_add_single_child(&generator->node, child, leaf);
		}
		while (parent) {
			if (parent->node.children > 1) {
				if (multi_children_node == (zend_generator_node *) 0x1) {
					multi_children_node = zend_generator_search_multi_children_node(&child->node);
				}
				if (multi_children_node) {
					zend_generator_merge_child_nodes(&parent->node, multi_children_node, cur);
				} else {
					zend_generator_add_single_child(&parent->node, cur, leaf);
				}
			}
			cur = parent;
			parent = parent->node.parent;
		}
	}
}

void zend_generator_yield_from(zend_generator *generator, zend_generator *from)
{
	zend_generator_add_child(from, generator);

	generator->node.parent = from;
	zend_generator_get_current(generator);
	GC_DELREF(&from->std);
	generator->flags |= ZEND_GENERATOR_DO_INIT;
}

ZEND_API zend_generator *zend_generator_update_current(zend_generator *generator, zend_generator *leaf)
{
	zend_generator *old_root, *root = leaf->node.ptr.root;

	/* generator at the root had stopped */
	if (root != generator) {
		old_root = root;
		root = zend_generator_get_child(&root->node, leaf);
	} else {
		old_root = NULL;
	}

	while (!root->execute_data && root != generator) {
		OBJ_RELEASE(&old_root->std);
		old_root = root;

		root = zend_generator_get_child(&root->node, leaf);
	}

	if (root->node.parent) {
		if (root->node.parent->execute_data == NULL) {
			if (EXPECTED(EG(exception) == NULL)) {
				zend_op *yield_from = (zend_op *) root->execute_data->opline - 1;

				if (yield_from->opcode == ZEND_YIELD_FROM) {
					if (Z_ISUNDEF(root->node.parent->retval)) {
						/* Throw the exception in the context of the generator */
						zend_execute_data *original_execute_data = EG(current_execute_data);
						EG(current_execute_data) = root->execute_data;

						if (root == generator) {
							root->execute_data->prev_execute_data = original_execute_data;
						} else {
							root->execute_data->prev_execute_data = &generator->execute_fake;
							generator->execute_fake.prev_execute_data = original_execute_data;
						}

						root->execute_data->opline--; /* ZEND_YIELD(_FROM) already advance, so decrement opline to throw from correct place */
						zend_throw_exception(zend_ce_ClosedGeneratorException, "Generator yielded from aborted, no return value available", 0);

						EG(current_execute_data) = original_execute_data;

						if (!((old_root ? old_root : generator)->flags & ZEND_GENERATOR_CURRENTLY_RUNNING)) {
							leaf->node.ptr.root = root;
							root->node.parent = NULL;
							if (old_root) {
								OBJ_RELEASE(&old_root->std);
							}
							zend_generator_resume(leaf);
							return leaf->node.ptr.root; /* this may be updated during zend_generator_resume! */
						}
					} else {
						zval_ptr_dtor(&root->value);
						ZVAL_COPY(&root->value, &root->node.parent->value);
						ZVAL_COPY(ZEND_CALL_VAR(root->execute_data, yield_from->result.var), &root->node.parent->retval);
					}
				}
			}

			root->node.parent = NULL;
		} else {
			do {
				root = root->node.parent;
				GC_ADDREF(&root->std);
			} while (root->node.parent);
		}
	}

	leaf->node.ptr.root = root;
	if (old_root) {
		OBJ_RELEASE(&old_root->std);
	}

	return root;
}

static int zend_generator_get_next_delegated_value(zend_generator *generator) /* {{{ */
{
	zval *value;
	if (Z_TYPE(generator->values) == IS_ARRAY) {
		HashTable *ht = Z_ARR(generator->values);
		HashPosition pos = Z_FE_POS(generator->values);

		Bucket *p;
		do {
			if (UNEXPECTED(pos >= ht->nNumUsed)) {
				/* Reached end of array */
				goto failure;
			}

			p = &ht->arData[pos];
			value = &p->val;
			if (Z_TYPE_P(value) == IS_INDIRECT) {
				value = Z_INDIRECT_P(value);
			}
			pos++;
		} while (Z_ISUNDEF_P(value));

		zval_ptr_dtor(&generator->value);
		ZVAL_COPY(&generator->value, value);

		zval_ptr_dtor(&generator->key);
		if (p->key) {
			ZVAL_STR_COPY(&generator->key, p->key);
		} else {
			ZVAL_LONG(&generator->key, p->h);
		}

		Z_FE_POS(generator->values) = pos;
	} else {
		zend_object_iterator *iter = (zend_object_iterator *) Z_OBJ(generator->values);

		if (iter->index++ > 0) {
			iter->funcs->move_forward(iter);
			if (UNEXPECTED(EG(exception) != NULL)) {
				goto exception;
			}
		}

		if (iter->funcs->valid(iter) == FAILURE) {
			/* reached end of iteration */
			goto failure;
		}

		value = iter->funcs->get_current_data(iter);
		if (UNEXPECTED(EG(exception) != NULL)) {
			goto exception;
		} else if (UNEXPECTED(!value)) {
			goto failure;
		}

		zval_ptr_dtor(&generator->value);
		ZVAL_COPY(&generator->value, value);

		zval_ptr_dtor(&generator->key);
		if (iter->funcs->get_current_key) {
			iter->funcs->get_current_key(iter, &generator->key);
			if (UNEXPECTED(EG(exception) != NULL)) {
				ZVAL_UNDEF(&generator->key);
				goto exception;
			}
		} else {
			ZVAL_LONG(&generator->key, iter->index);
		}
	}
	return SUCCESS;

exception:
	zend_rethrow_exception(generator->execute_data);

failure:
	zval_ptr_dtor(&generator->values);
	ZVAL_UNDEF(&generator->values);
	return FAILURE;
}
/* }}} */

ZEND_API void zend_generator_resume(zend_generator *orig_generator) /* {{{ */
{
	zend_generator *generator = zend_generator_get_current(orig_generator);

	/* The generator is already closed, thus can't resume */
	if (UNEXPECTED(!generator->execute_data)) {
		return;
	}

try_again:
	if (generator->flags & ZEND_GENERATOR_CURRENTLY_RUNNING) {
		zend_throw_error(NULL, "Cannot resume an already running generator");
		return;
	}

	if (UNEXPECTED((orig_generator->flags & ZEND_GENERATOR_DO_INIT) != 0 && !Z_ISUNDEF(generator->value))) {
		/* We must not advance Generator if we yield from a Generator being currently run */
		orig_generator->flags &= ~ZEND_GENERATOR_DO_INIT;
		return;
	}

	if (UNEXPECTED(!Z_ISUNDEF(generator->values))) {
		if (EXPECTED(zend_generator_get_next_delegated_value(generator) == SUCCESS)) {
			orig_generator->flags &= ~ZEND_GENERATOR_DO_INIT;
			return;
		}
		/* If there are no more deletegated values, resume the generator
		 * after the "yield from" expression. */
	}

	/* Drop the AT_FIRST_YIELD flag */
	orig_generator->flags &= ~ZEND_GENERATOR_AT_FIRST_YIELD;

	{
		/* Backup executor globals */
		zend_execute_data *original_execute_data = EG(current_execute_data);

		/* Set executor globals */
		EG(current_execute_data) = generator->execute_data;

		/* We want the backtrace to look as if the generator function was
		 * called from whatever method we are current running (e.g. next()).
		 * So we have to link generator call frame with caller call frame. */
		if (generator == orig_generator) {
			generator->execute_data->prev_execute_data = original_execute_data;
		} else {
			/* We need some execute_data placeholder in stacktrace to be replaced
			 * by the real stack trace when needed */
			generator->execute_data->prev_execute_data = &orig_generator->execute_fake;
			orig_generator->execute_fake.prev_execute_data = original_execute_data;
		}

		if (UNEXPECTED(generator->frozen_call_stack)) {
			/* Restore frozen call-stack */
			zend_generator_restore_call_stack(generator);
		}

		/* Resume execution */
		generator->flags |= ZEND_GENERATOR_CURRENTLY_RUNNING;
		zend_execute_ex(generator->execute_data);
		generator->flags &= ~ZEND_GENERATOR_CURRENTLY_RUNNING;

		generator->frozen_call_stack = NULL;
		if (EXPECTED(generator->execute_data) &&
		    UNEXPECTED(generator->execute_data->call)) {
			/* Frize call-stack */
			generator->frozen_call_stack = zend_generator_freeze_call_stack(generator->execute_data);
		}

		/* Restore executor globals */
		EG(current_execute_data) = original_execute_data;

		/* If an exception was thrown in the generator we have to internally
		 * rethrow it in the parent scope.
		 * In case we did yield from, the Exception must be rethrown into
		 * its calling frame (see above in if (check_yield_from). */
		if (UNEXPECTED(EG(exception) != NULL)) {
			if (generator == orig_generator) {
				zend_generator_close(generator, 0);
				if (!EG(current_execute_data)) {
					zend_throw_exception_internal(NULL);
				} else if (EG(current_execute_data)->func &&
						ZEND_USER_CODE(EG(current_execute_data)->func->common.type)) {
					zend_rethrow_exception(EG(current_execute_data));
				}
			} else {
				generator = zend_generator_get_current(orig_generator);
				zend_generator_throw_exception(generator, NULL);
				orig_generator->flags &= ~ZEND_GENERATOR_DO_INIT;
				goto try_again;
			}
		}

		/* yield from was used, try another resume. */
		if (UNEXPECTED((generator != orig_generator && !Z_ISUNDEF(generator->retval)) || (generator->execute_data && (generator->execute_data->opline - 1)->opcode == ZEND_YIELD_FROM))) {
			generator = zend_generator_get_current(orig_generator);
			goto try_again;
		}
	}

	orig_generator->flags &= ~ZEND_GENERATOR_DO_INIT;
}
/* }}} */

static inline void zend_generator_ensure_initialized(zend_generator *generator) /* {{{ */
{
	if (UNEXPECTED(Z_TYPE(generator->value) == IS_UNDEF) && EXPECTED(generator->execute_data) && EXPECTED(generator->node.parent == NULL)) {
		zend_generator_resume(generator);
		generator->flags |= ZEND_GENERATOR_AT_FIRST_YIELD;
	}
}
/* }}} */

static inline void zend_generator_rewind(zend_generator *generator) /* {{{ */
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

	generator = (zend_generator *) Z_OBJ_P(ZEND_THIS);

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

	generator = (zend_generator *) Z_OBJ_P(ZEND_THIS);

	zend_generator_ensure_initialized(generator);

	zend_generator_get_current(generator);

	RETURN_BOOL(EXPECTED(generator->execute_data != NULL));
}
/* }}} */

/* {{{ proto mixed Generator::current()
 * Get the current value */
ZEND_METHOD(Generator, current)
{
	zend_generator *generator, *root;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	generator = (zend_generator *) Z_OBJ_P(ZEND_THIS);

	zend_generator_ensure_initialized(generator);

	root = zend_generator_get_current(generator);
	if (EXPECTED(generator->execute_data != NULL && Z_TYPE(root->value) != IS_UNDEF)) {
		zval *value = &root->value;

		ZVAL_COPY_DEREF(return_value, value);
	}
}
/* }}} */

/* {{{ proto mixed Generator::key()
 * Get the current key */
ZEND_METHOD(Generator, key)
{
	zend_generator *generator, *root;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	generator = (zend_generator *) Z_OBJ_P(ZEND_THIS);

	zend_generator_ensure_initialized(generator);

	root = zend_generator_get_current(generator);
	if (EXPECTED(generator->execute_data != NULL && Z_TYPE(root->key) != IS_UNDEF)) {
		zval *key = &root->key;

		ZVAL_COPY_DEREF(return_value, key);
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

	generator = (zend_generator *) Z_OBJ_P(ZEND_THIS);

	zend_generator_ensure_initialized(generator);

	zend_generator_resume(generator);
}
/* }}} */

/* {{{ proto mixed Generator::send(mixed value)
 * Sends a value to the generator */
ZEND_METHOD(Generator, send)
{
	zval *value;
	zend_generator *generator, *root;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	generator = (zend_generator *) Z_OBJ_P(ZEND_THIS);

	zend_generator_ensure_initialized(generator);

	/* The generator is already closed, thus can't send anything */
	if (UNEXPECTED(!generator->execute_data)) {
		return;
	}

	root = zend_generator_get_current(generator);
	/* Put sent value in the target VAR slot, if it is used */
	if (root->send_target) {
		ZVAL_COPY(root->send_target, value);
	}

	zend_generator_resume(generator);

	root = zend_generator_get_current(generator);
	if (EXPECTED(generator->execute_data)) {
		zval *value = &root->value;

		ZVAL_COPY_DEREF(return_value, value);
	}
}
/* }}} */

/* {{{ proto mixed Generator::throw(Exception exception)
 * Throws an exception into the generator */
ZEND_METHOD(Generator, throw)
{
	zval *exception;
	zend_generator *generator;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(exception)
	ZEND_PARSE_PARAMETERS_END();

	Z_TRY_ADDREF_P(exception);

	generator = (zend_generator *) Z_OBJ_P(ZEND_THIS);

	zend_generator_ensure_initialized(generator);

	if (generator->execute_data) {
		zend_generator *root = zend_generator_get_current(generator);

		zend_generator_throw_exception(root, exception);

		zend_generator_resume(generator);

		root = zend_generator_get_current(generator);
		if (generator->execute_data) {
			zval *value = &root->value;

			ZVAL_COPY_DEREF(return_value, value);
		}
	} else {
		/* If the generator is already closed throw the exception in the
		 * current context */
		zend_throw_exception_object(exception);
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

	generator = (zend_generator *) Z_OBJ_P(ZEND_THIS);

	zend_generator_ensure_initialized(generator);
	if (UNEXPECTED(EG(exception))) {
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

/* get_iterator implementation */

static void zend_generator_iterator_dtor(zend_object_iterator *iterator) /* {{{ */
{
	zend_generator *generator = (zend_generator*)Z_OBJ(iterator->data);
	generator->iterator = NULL;
	zval_ptr_dtor(&iterator->data);
}
/* }}} */

static int zend_generator_iterator_valid(zend_object_iterator *iterator) /* {{{ */
{
	zend_generator *generator = (zend_generator*)Z_OBJ(iterator->data);

	zend_generator_ensure_initialized(generator);

	zend_generator_get_current(generator);

	return generator->execute_data ? SUCCESS : FAILURE;
}
/* }}} */

static zval *zend_generator_iterator_get_data(zend_object_iterator *iterator) /* {{{ */
{
	zend_generator *generator = (zend_generator*)Z_OBJ(iterator->data), *root;

	zend_generator_ensure_initialized(generator);

	root = zend_generator_get_current(generator);

	return &root->value;
}
/* }}} */

static void zend_generator_iterator_get_key(zend_object_iterator *iterator, zval *key) /* {{{ */
{
	zend_generator *generator = (zend_generator*)Z_OBJ(iterator->data), *root;

	zend_generator_ensure_initialized(generator);

	root = zend_generator_get_current(generator);

	if (EXPECTED(Z_TYPE(root->key) != IS_UNDEF)) {
		zval *zv = &root->key;

		ZVAL_COPY_DEREF(key, zv);
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

static const zend_object_iterator_funcs zend_generator_iterator_functions = {
	zend_generator_iterator_dtor,
	zend_generator_iterator_valid,
	zend_generator_iterator_get_data,
	zend_generator_iterator_get_key,
	zend_generator_iterator_move_forward,
	zend_generator_iterator_rewind,
	NULL
};

zend_object_iterator *zend_generator_get_iterator(zend_class_entry *ce, zval *object, int by_ref) /* {{{ */
{
	zend_object_iterator *iterator;
	zend_generator *generator = (zend_generator*)Z_OBJ_P(object);

	if (!generator->execute_data) {
		zend_throw_exception(NULL, "Cannot traverse an already closed generator", 0);
		return NULL;
	}

	if (UNEXPECTED(by_ref) && !(generator->execute_data->func->op_array.fn_flags & ZEND_ACC_RETURN_REFERENCE)) {
		zend_throw_exception(NULL, "You can only iterate a generator by-reference if it declared that it yields by-reference", 0);
		return NULL;
	}

	iterator = generator->iterator = emalloc(sizeof(zend_object_iterator));

	zend_iterator_init(iterator);

	iterator->funcs = &zend_generator_iterator_functions;
	Z_ADDREF_P(object);
	ZVAL_OBJ(&iterator->data, Z_OBJ_P(object));

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

	memcpy(&zend_generator_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	zend_generator_handlers.free_obj = zend_generator_free_storage;
	zend_generator_handlers.dtor_obj = zend_generator_dtor_storage;
	zend_generator_handlers.get_gc = zend_generator_get_gc;
	zend_generator_handlers.clone_obj = NULL;
	zend_generator_handlers.get_constructor = zend_generator_get_constructor;

	INIT_CLASS_ENTRY(ce, "ClosedGeneratorException", NULL);
	zend_ce_ClosedGeneratorException = zend_register_internal_class_ex(&ce, zend_ce_exception);
}
/* }}} */
