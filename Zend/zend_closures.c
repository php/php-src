/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Christian Seiler <chris_se@gmx.net>                         |
   |          Dmitry Stogov <dmitry@php.net>                              |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_ast.h"
#include "zend_attributes.h"
#include "zend_closures.h"
#include "zend_exceptions.h"
#include "zend_execute.h"
#include "zend_interfaces.h"
#include "zend_objects.h"
#include "zend_objects_API.h"
#include "zend_globals.h"
#include "zend_closures_arginfo.h"

typedef struct _zend_closure {
	zend_object       std;
	zend_function     func;
	zval              this_ptr;
	zend_class_entry *called_scope;
	zif_handler       orig_internal_handler;
	/* The class whose constant expressions declared this closure, when it was
	 * created by evaluating a first-class callable in one of them (a fake
	 * closure carrying ZEND_ACC2_CONSTEXPR_CLOSURE). */
	zend_class_entry *constexpr_site;
} zend_closure;

/* non-static since it needs to be referenced */
ZEND_API zend_class_entry *zend_ce_closure;
static zend_object_handlers closure_handlers;

static zend_result zend_closure_get_closure(zend_object *obj, zend_class_entry **ce_ptr, zend_function **fptr_ptr, zend_object **obj_ptr, bool check_only);
static ZEND_COLD ZEND_NAMED_FUNCTION(zend_closure_uninitialized_handler);
static void zend_closure_init_ex(zend_closure *closure, zend_function *func, zend_class_entry *scope, zend_class_entry *called_scope, zval *this_ptr, bool is_fake);

ZEND_METHOD(Closure, __invoke) /* {{{ */
{
	zend_function *func = EX(func);
	zval *args;
	uint32_t num_args;
	HashTable *named_args;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC_WITH_NAMED(args, num_args, named_args)
	ZEND_PARSE_PARAMETERS_END();

	zend_fcall_info_cache fcc = {
		.closure = Z_OBJ_P(ZEND_THIS),
	};
	zend_closure_get_closure(Z_OBJ_P(ZEND_THIS), &fcc.calling_scope, &fcc.function_handler, &fcc.object, false);
	fcc.called_scope = fcc.calling_scope;
	zend_call_known_fcc(&fcc, return_value, num_args, args, named_args);

	/* destruct the function also, then - we have allocated it in get_method */
	zend_string_release_ex(func->internal_function.function_name, 0);
	efree(func);

	/* Set the func pointer to NULL. Prior to PHP 8.3, this was only done for debug builds,
	 * because debug builds check certain properties after the call and needed to know this
	 * had been freed.
	 * However, extensions can proxy zend_execute_internal, and it's a bit surprising to have
	 * an invalid func pointer sitting on there, so this was changed in PHP 8.3.
	 */
	execute_data->func = NULL;
}
/* }}} */

static bool zend_valid_closure_binding(
		zend_closure *closure, zval *newthis, zend_class_entry *scope) /* {{{ */
{
	zend_function *func = &closure->func;
	bool is_fake_closure = (func->common.fn_flags & ZEND_ACC_FAKE_CLOSURE) != 0;
	if (newthis) {
		if (func->common.fn_flags & ZEND_ACC_STATIC) {
			zend_error(E_WARNING, "Cannot bind an instance to a static closure, this will be an error in PHP 9");
			return false;
		}

		if (is_fake_closure && func->common.scope &&
				!instanceof_function(Z_OBJCE_P(newthis), func->common.scope)) {
			/* Binding incompatible $this to an internal method is not supported. */
			zend_error(E_WARNING, "Cannot bind method %s::%s() to object of class %s, this will be an error in PHP 9",
					ZSTR_VAL(func->common.scope->name),
					ZSTR_VAL(func->common.function_name),
					ZSTR_VAL(Z_OBJCE_P(newthis)->name));
			return false;
		}
	} else if (is_fake_closure && func->common.scope
			&& !(func->common.fn_flags & ZEND_ACC_STATIC)) {
		zend_error(E_WARNING, "Cannot unbind $this of method, this will be an error in PHP 9");
		return false;
	} else if (!is_fake_closure && !Z_ISUNDEF(closure->this_ptr)
			&& (func->common.fn_flags & ZEND_ACC_USES_THIS)) {
		zend_error(E_WARNING, "Cannot unbind $this of closure using $this, this will be an error in PHP 9");
		return false;
	}

	if (scope && scope != func->common.scope && scope->type == ZEND_INTERNAL_CLASS) {
		/* rebinding to internal class is not allowed */
		zend_error(E_WARNING, "Cannot bind closure to scope of internal class %s, this will be an error in PHP 9",
				ZSTR_VAL(scope->name));
		return false;
	}

	if (is_fake_closure && scope != func->common.scope) {
		if (func->common.scope == NULL) {
			zend_error(E_WARNING, "Cannot rebind scope of closure created from function, this will be an error in PHP 9");
		} else {
			zend_error(E_WARNING, "Cannot rebind scope of closure created from method, this will be an error in PHP 9");
		}
		return false;
	}

	return true;
}
/* }}} */

/* {{{ Call closure, binding to a given object with its class as the scope */
ZEND_METHOD(Closure, call)
{
	zval *newthis, closure_result;
	zend_closure *closure;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	zend_object *newobj;
	zend_class_entry *newclass;

	fci.param_count = 0;
	fci.params = NULL;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_OBJECT(newthis)
		Z_PARAM_VARIADIC_WITH_NAMED(fci.params, fci.param_count, fci.named_params)
	ZEND_PARSE_PARAMETERS_END();

	closure = (zend_closure *) Z_OBJ_P(ZEND_THIS);

	newobj = Z_OBJ_P(newthis);
	newclass = newobj->ce;

	if (!zend_valid_closure_binding(closure, newthis, newclass)) {
		return;
	}

	fci_cache.called_scope = newclass;
	fci_cache.object = fci.object = newobj;

	fci.size = sizeof(fci);
	fci.consumed_args = 0;
	ZVAL_OBJ(&fci.function_name, &closure->std);
	ZVAL_UNDEF(&closure_result);
	fci.retval = &closure_result;

	if (closure->func.common.fn_flags & ZEND_ACC_GENERATOR) {
		zval new_closure;
		zend_create_closure(&new_closure, &closure->func, newclass, closure->called_scope, newthis);
		closure = (zend_closure *) Z_OBJ(new_closure);
		fci_cache.function_handler = &closure->func;

		zend_call_function(&fci, &fci_cache);

		/* copied upon generator creation */
		GC_DELREF(&closure->std);
	} else {
		zend_closure *fake_closure;
		zend_function *my_function;

		fake_closure = emalloc(sizeof(zend_closure));
		memset(&fake_closure->std, 0, sizeof(fake_closure->std));
		fake_closure->std.gc.refcount = 1;
		fake_closure->std.gc.u.type_info = GC_NULL;
		ZVAL_UNDEF(&fake_closure->this_ptr);
		fake_closure->called_scope = NULL;
		my_function = &fake_closure->func;
		if (ZEND_USER_CODE(closure->func.type)) {
			memcpy(my_function, &closure->func, sizeof(zend_op_array));
		} else {
			memcpy(my_function, &closure->func, sizeof(zend_internal_function));
		}
		/* use scope of passed object */
		my_function->common.scope = newclass;
		if (closure->func.type == ZEND_INTERNAL_FUNCTION) {
			my_function->internal_function.handler = closure->orig_internal_handler;
		}
		fci_cache.function_handler = my_function;

		/* Runtime cache relies on bound scope to be immutable, hence we need a separate rt cache in case scope changed */
		if (ZEND_USER_CODE(my_function->type)
		 && (closure->func.common.scope != newclass
		  || (closure->func.common.fn_flags & ZEND_ACC_HEAP_RT_CACHE))) {
			void *ptr;

			my_function->op_array.fn_flags |= ZEND_ACC_HEAP_RT_CACHE;
			ptr = emalloc(my_function->op_array.cache_size);
			ZEND_MAP_PTR_INIT(my_function->op_array.run_time_cache, ptr);
			memset(ptr, 0, my_function->op_array.cache_size);
		}

		zend_call_function(&fci, &fci_cache);

		if (ZEND_USER_CODE(my_function->type)) {
			if (fci_cache.function_handler->common.fn_flags & ZEND_ACC_HEAP_RT_CACHE) {
				efree(ZEND_MAP_PTR(my_function->op_array.run_time_cache));
			}
		}
		efree_size(fake_closure, sizeof(zend_closure));
	}

	if (Z_TYPE(closure_result) != IS_UNDEF) {
		if (Z_ISREF(closure_result)) {
			zend_unwrap_reference(&closure_result);
		}
		ZVAL_COPY_VALUE(return_value, &closure_result);
	}
}
/* }}} */

static void do_closure_bind(zval *return_value, zval *zclosure, zval *newthis, zend_object *scope_obj, zend_string *scope_str)
{
	zend_class_entry *ce, *called_scope;
	zend_closure *closure = (zend_closure *) Z_OBJ_P(zclosure);

	if (scope_obj) {
		ce = scope_obj->ce;
	} else if (scope_str) {
		if (zend_string_equals(scope_str, ZSTR_KNOWN(ZEND_STR_STATIC))) {
			ce = closure->func.common.scope;
		} else if ((ce = zend_lookup_class(scope_str)) == NULL) {
			zend_error(E_WARNING, "Class \"%s\" not found", ZSTR_VAL(scope_str));
			RETURN_NULL();
		}
	} else {
		ce = NULL;
	}

	if (!zend_valid_closure_binding(closure, newthis, ce)) {
		return;
	}

	if (newthis) {
		called_scope = Z_OBJCE_P(newthis);
	} else {
		called_scope = ce;
	}

	zend_create_closure(return_value, &closure->func, ce, called_scope, newthis);
}

/* {{{ Create a closure from another one and bind to another object and scope */
ZEND_METHOD(Closure, bind)
{
	zval *zclosure, *newthis;
	zend_object *scope_obj = NULL;
	zend_string *scope_str = ZSTR_KNOWN(ZEND_STR_STATIC);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJECT_OF_CLASS(zclosure, zend_ce_closure)
		Z_PARAM_OBJECT_OR_NULL(newthis)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OR_STR_OR_NULL(scope_obj, scope_str)
	ZEND_PARSE_PARAMETERS_END();

	do_closure_bind(return_value, zclosure, newthis, scope_obj, scope_str);
}

/* {{{ Create a closure from another one and bind to another object and scope */
ZEND_METHOD(Closure, bindTo)
{
	zval *newthis;
	zend_object *scope_obj = NULL;
	zend_string *scope_str = ZSTR_KNOWN(ZEND_STR_STATIC);

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJECT_OR_NULL(newthis)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OR_STR_OR_NULL(scope_obj, scope_str)
	ZEND_PARSE_PARAMETERS_END();

	do_closure_bind(return_value, ZEND_THIS, newthis, scope_obj, scope_str);
}

static void zend_copy_parameters_array(const uint32_t param_count, HashTable *argument_array) /* {{{ */
{
	zval *param_ptr = ZEND_CALL_ARG(EG(current_execute_data), 1);

	ZEND_ASSERT(param_count <= ZEND_CALL_NUM_ARGS(EG(current_execute_data)));

	for (uint32_t i = 0; i < param_count; i++) {
		Z_TRY_ADDREF_P(param_ptr);
		zend_hash_next_index_insert_new(argument_array, param_ptr);
		param_ptr++;
	}
}

static ZEND_NAMED_FUNCTION(zend_closure_call_magic) /* {{{ */ {
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval params[2];

	memset(&fci, 0, sizeof(zend_fcall_info));
	memset(&fcc, 0, sizeof(zend_fcall_info_cache));

	fci.size = sizeof(zend_fcall_info);
	fci.retval = return_value;

	fcc.function_handler = (EX(func)->internal_function.fn_flags & ZEND_ACC_STATIC) ?
		EX(func)->internal_function.scope->__callstatic : EX(func)->internal_function.scope->__call;
	fci.named_params = NULL;
	fci.params = params;
	fci.param_count = 2;
	ZVAL_STR(&fci.params[0], EX(func)->common.function_name);
	if (EX_CALL_INFO() & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS) {
		zend_string *name;
		zval *named_param_zval;
		array_init_size(&fci.params[1], ZEND_NUM_ARGS() + zend_hash_num_elements(EX(extra_named_params)));
		/* Avoid conversion from packed to mixed later. */
		zend_hash_real_init_mixed(Z_ARRVAL(fci.params[1]));
		zend_copy_parameters_array(ZEND_NUM_ARGS(), Z_ARRVAL(fci.params[1]));
		ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(EX(extra_named_params), name, named_param_zval) {
			Z_TRY_ADDREF_P(named_param_zval);
			zend_hash_add_new(Z_ARRVAL(fci.params[1]), name, named_param_zval);
		} ZEND_HASH_FOREACH_END();
	} else if (ZEND_NUM_ARGS()) {
		array_init_size(&fci.params[1], ZEND_NUM_ARGS());
		zend_copy_parameters_array(ZEND_NUM_ARGS(), Z_ARRVAL(fci.params[1]));
	} else {
		ZVAL_EMPTY_ARRAY(&fci.params[1]);
	}

	fcc.object = fci.object = Z_OBJ_P(ZEND_THIS);
	fcc.called_scope = zend_get_called_scope(EG(current_execute_data));

	zend_call_function(&fci, &fcc);
	zend_return_unwrap_ref(EG(current_execute_data), return_value);
	zval_ptr_dtor(&fci.params[1]);
}
/* }}} */

static zend_result zend_create_closure_from_callable(zval *return_value, zval *callable, char **error) /* {{{ */ {
	zend_fcall_info_cache fcc;
	zend_function *mptr;
	zval instance;
	zend_internal_function call;

	if (!zend_is_callable_ex(callable, NULL, 0, NULL, &fcc, error)) {
		return FAILURE;
	}

	mptr = fcc.function_handler;
	if (mptr->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
		/* For Closure::fromCallable([$closure, "__invoke"]) return $closure. */
		if (fcc.object && fcc.object->ce == zend_ce_closure
				&& zend_string_equals(mptr->common.function_name, ZSTR_KNOWN(ZEND_STR_MAGIC_INVOKE))) {
			RETVAL_OBJ_COPY(fcc.object);
			zend_free_trampoline(mptr);
			return SUCCESS;
		}

		if (!mptr->common.scope) {
			return FAILURE;
		}
		if (mptr->common.fn_flags & ZEND_ACC_STATIC) {
			if (!mptr->common.scope->__callstatic) {
				return FAILURE;
			}
		} else {
			if (!mptr->common.scope->__call) {
				return FAILURE;
			}
		}

		memset(&call, 0, sizeof(zend_internal_function));
		call.type = ZEND_INTERNAL_FUNCTION;
		call.fn_flags = mptr->common.fn_flags & (ZEND_ACC_STATIC | ZEND_ACC_DEPRECATED);
		call.handler = zend_closure_call_magic;
		call.function_name = mptr->common.function_name;
		call.scope = mptr->common.scope;
		call.doc_comment = NULL;
		call.attributes = mptr->common.attributes;

		zend_free_trampoline(mptr);
		mptr = (zend_function *) &call;
	}

	if (fcc.object) {
		ZVAL_OBJ(&instance, fcc.object);
		zend_create_fake_closure(return_value, mptr, mptr->common.scope, fcc.called_scope, &instance);
	} else {
		zend_create_fake_closure(return_value, mptr, mptr->common.scope, fcc.called_scope, NULL);
	}

	if (&mptr->internal_function == &call) {
		zend_string_release(mptr->common.function_name);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ Create a closure from a callable using the current scope. */
ZEND_METHOD(Closure, fromCallable)
{
	zval *callable;
	char *error = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(callable)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(callable) == IS_OBJECT && instanceof_function(Z_OBJCE_P(callable), zend_ce_closure)) {
		/* It's already a closure */
		RETURN_COPY(callable);
	}

	if (zend_create_closure_from_callable(return_value, callable, &error) == FAILURE) {
		if (error) {
			zend_type_error("Failed to create closure from callable: %s", error);
			efree(error);
		} else {
			zend_type_error("Failed to create closure from callable");
		}
	}
}
/* }}} */

ZEND_METHOD(Closure, getCurrent)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_execute_data *prev_ex = EX(prev_execute_data);

	if (!prev_ex
	 || !prev_ex->func
	 || (prev_ex->func->common.fn_flags & (ZEND_ACC_CLOSURE|ZEND_ACC_FAKE_CLOSURE)) != ZEND_ACC_CLOSURE) {
			zend_throw_error(NULL, "Current function is not a closure");
			RETURN_THROWS();
	}

	zend_object *obj = ZEND_CLOSURE_OBJECT(prev_ex->func);
	RETURN_OBJ_COPY(obj);
}

/* Closures declared in constant expressions
 *
 * Closures declared in constant expressions are static, capture no variables
 * and are fully described by their compiled op_array. This makes them
 * addressable by (class, id), where the id is the position of the closure in
 * a canonical walk over all constant expression ASTs that are reachable from
 * the class and that are not subject to in-place evaluation: attribute
 * arguments and parameter default values, including those of nested
 * functions. Class constant values and property default values are evaluated
 * (and their AST freed) in place, so closures declared there are not
 * addressable this way.
 *
 * The walk visits candidates in a deterministic order that only depends on
 * the compiled class, not on runtime evaluation state, so ids computed by one
 * process can be resolved by another process running the same code.
 */

/* Reflection-element kinds for the site descriptor. */
typedef enum _zend_constexpr_site_kind {
	ZEND_CEXPR_SITE_CLASS,
	ZEND_CEXPR_SITE_CONST,
	ZEND_CEXPR_SITE_PROP,
	ZEND_CEXPR_SITE_HOOK,
	ZEND_CEXPR_SITE_METHOD,
} zend_constexpr_site_kind;

typedef struct zend_constexpr_closure_walk {
	/* Rank of the next closure WITHIN the current reflection element; reset to
	 * 0 at each element boundary by zend_constexpr_closure_walk_class. A
	 * reference is (element site, local rank), not a single class-global rank,
	 * so adding/removing/reordering closures in one element does not renumber
	 * closures in other elements. */
	uint32_t next_id;
	/* When by_id is true, search for target_id (a LOCAL rank among the
	 * anonymous closures of a single element; the caller has already
	 * positioned the walk on that element). When target_key is set, search
	 * for the first-class callable reference matching that "Called::method"
	 * / "function" key. Otherwise search for the op_array of an anonymous
	 * closure identified by its opcodes. Serialization and resolution both
	 * match first-class callables by key, so a closure is serializable
	 * exactly when its reference resolves. */
	bool by_id;
	uint32_t target_id;
	const char *target_key;
	size_t target_key_len;
	const zend_op *opcodes;
	/* The class being walked; used to resolve self/parent references. */
	zend_class_entry *site_class;
	/* The reflection element currently being walked, kept as a cheap
	 * (kind, borrowed name, hook) descriptor rather than a built string, so no
	 * allocation happens for the elements the walk merely passes through. The
	 * "<site>@<rank>" string is materialized once, from found_*, on a match. */
	zend_constexpr_site_kind cur_kind;
	zend_string *cur_name;
	uint8_t cur_hook;
	/* The found site: either a ZEND_AST_OP_ARRAY node, or a ZEND_AST_CALL /
	 * ZEND_AST_STATIC_CALL node whose argument list is a first-class callable
	 * placeholder. */
	zend_ast *found;
	uint32_t found_id;
	zend_constexpr_site_kind found_kind;
	zend_string *found_name;
	uint8_t found_hook;
} zend_constexpr_closure_walk;

static bool zend_constexpr_closure_walk_op_array(zend_constexpr_closure_walk *w, zend_op_array *op_array);

/* Whether the first-class callable declared by the given site matches a
 * "Called::method" / "function" key. Pure string comparisons: class
 * references are compile-time resolved names ('self'/'parent' resolve
 * against the walked class), so no class or function is ever loaded. */
static bool zend_constexpr_fcc_key_matches(const zend_constexpr_closure_walk *w, const zend_ast *ast)
{
	const char *key = w->target_key;
	size_t key_len = w->target_key_len;
	const char *sep = NULL;

	for (size_t i = 0; i + 1 < key_len; i++) {
		if (key[i] == ':' && key[i + 1] == ':') {
			sep = key + i;
			break;
		}
	}

	if (ast->kind == ZEND_AST_STATIC_CALL) {
		const zend_ast *class_ast = ast->child[0];
		zend_string *method = zend_ast_get_str(ast->child[1]);
		zend_string *ref_name;

		if (!sep) {
			return false;
		}

		switch (class_ast->attr >> ZEND_CONST_EXPR_NEW_FETCH_TYPE_SHIFT) {
			case ZEND_FETCH_CLASS_SELF:
				ref_name = w->site_class->name;
				break;
			case ZEND_FETCH_CLASS_PARENT:
				ref_name = w->site_class->parent ? w->site_class->parent->name : NULL;
				break;
			default:
				ref_name = zend_ast_get_str((zend_ast *) class_ast);
				break;
		}

		return ref_name
			&& ZSTR_LEN(ref_name) == (size_t) (sep - key)
			&& zend_binary_strcasecmp(ZSTR_VAL(ref_name), ZSTR_LEN(ref_name), key, sep - key) == 0
			&& ZSTR_LEN(method) == key_len - (sep - key) - 2
			&& zend_binary_strcasecmp(ZSTR_VAL(method), ZSTR_LEN(method), sep + 2, ZSTR_LEN(method)) == 0;
	}

	ZEND_ASSERT(ast->kind == ZEND_AST_CALL);

	if (sep) {
		return false;
	}

	zend_string *fname = zend_ast_get_str(ast->child[0]);
	if (ZSTR_LEN(fname) == key_len
	 && zend_binary_strcasecmp(ZSTR_VAL(fname), ZSTR_LEN(fname), key, key_len) == 0) {
		return true;
	}
	/* Non-fully-qualified names in namespaced code may resolve to the global
	 * function, whose name is what serialization emits as the key. */
	if (ast->child[0]->attr != ZEND_NAME_FQ) {
		const char *backslash = zend_memrchr(ZSTR_VAL(fname), '\\', ZSTR_LEN(fname));
		if (backslash) {
			size_t tail_len = ZSTR_LEN(fname) - (backslash + 1 - ZSTR_VAL(fname));
			return tail_len == key_len
				&& zend_binary_strcasecmp(backslash + 1, tail_len, key, key_len) == 0;
		}
	}
	return false;
}

static bool zend_constexpr_closure_visit_op_array(zend_constexpr_closure_walk *w, zend_ast *ast)
{
	zend_op_array *op_array = zend_ast_get_op_array(ast)->op_array;
	uint32_t id = w->next_id++;

	if (w->by_id ? w->target_id == id : (w->opcodes && w->opcodes == op_array->opcodes)) {
		w->found = ast;
		w->found_id = id;
		w->found_kind = w->cur_kind;
		w->found_name = w->cur_name;
		w->found_hook = w->cur_hook;
		return true;
	}

	/* The closure may itself declare closures in its attributes or in its
	 * parameter default values. */
	return zend_constexpr_closure_walk_op_array(w, op_array);
}

static bool zend_constexpr_closure_walk_ast(zend_constexpr_closure_walk *w, zend_ast *ast)
{
	if (!ast) {
		return false;
	}

	switch (ast->kind) {
		case ZEND_AST_OP_ARRAY:
			return zend_constexpr_closure_visit_op_array(w, ast);
		case ZEND_AST_ZVAL:
		case ZEND_AST_CONSTANT:
			return false;
		case ZEND_AST_CALL:
		case ZEND_AST_STATIC_CALL: {
			zend_ast *args = ast->kind == ZEND_AST_CALL ? ast->child[1] : ast->child[2];

			/* In constant expressions, calls only exist in their first-class
			 * callable form: each one is a closure declaration site, keyed by
			 * the name of its target. It does not consume a rank: adding or
			 * removing a callable reference never renumbers the anonymous
			 * closures of the element. */
			if (args && args->kind == ZEND_AST_CALLABLE_CONVERT) {
				if (w->target_key && zend_constexpr_fcc_key_matches(w, ast)) {
					w->found = ast;
					w->found_kind = w->cur_kind;
					w->found_name = w->cur_name;
					w->found_hook = w->cur_hook;
					return true;
				}
				return false;
			}
			break;
		}
		case ZEND_AST_CALLABLE_CONVERT:
			/* Visited through its enclosing call node. */
			return false;
		default:
			break;
	}

	if (zend_ast_is_list(ast)) {
		zend_ast_list *list = zend_ast_get_list(ast);
		for (uint32_t i = 0; i < list->children; i++) {
			if (zend_constexpr_closure_walk_ast(w, list->child[i])) {
				return true;
			}
		}
		return false;
	}

	if (zend_ast_is_decl(ast)) {
		/* Closure declarations in constant expressions are compiled to
		 * ZEND_AST_OP_ARRAY nodes, so declarations cannot appear here. */
		ZEND_UNREACHABLE();
		return false;
	}

	uint32_t children = zend_ast_get_num_children(ast);
	for (uint32_t i = 0; i < children; i++) {
		if (zend_constexpr_closure_walk_ast(w, ast->child[i])) {
			return true;
		}
	}
	return false;
}

static bool zend_constexpr_closure_walk_zval(zend_constexpr_closure_walk *w, zval *zv)
{
	if (Z_TYPE_P(zv) == IS_CONSTANT_AST) {
		return zend_constexpr_closure_walk_ast(w, Z_ASTVAL_P(zv));
	}
	return false;
}

static bool zend_constexpr_closure_walk_attributes(zend_constexpr_closure_walk *w, HashTable *attributes)
{
	if (!attributes) {
		return false;
	}

	/* Attribute lists are always packed (c.f. zend_get_attribute()). */
	ZEND_HASH_PACKED_FOREACH_PTR(attributes, zend_attribute *attr) {
		for (uint32_t i = 0; i < attr->argc; i++) {
			if (zend_constexpr_closure_walk_zval(w, &attr->args[i].value)) {
				return true;
			}
		}
	} ZEND_HASH_FOREACH_END();

	return false;
}

static bool zend_constexpr_closure_walk_op_array(zend_constexpr_closure_walk *w, zend_op_array *op_array)
{
	if (op_array->type != ZEND_USER_FUNCTION) {
		return false;
	}

	if (zend_constexpr_closure_walk_attributes(w, op_array->attributes)) {
		return true;
	}

	/* Parameter default values are the op2 of the RECV_INIT for each optional
	 * parameter, at the head of the op array in parameter order. */
	for (uint32_t i = 0; i < op_array->num_args; i++) {
		const zend_op *recv = &op_array->opcodes[i];
		if (recv->opcode == ZEND_RECV_INIT
		 && zend_constexpr_closure_walk_zval(w, RT_CONSTANT(recv, recv->op2))) {
			return true;
		}
	}

	for (uint32_t i = 0; i < op_array->num_dynamic_func_defs; i++) {
		if (zend_constexpr_closure_walk_op_array(w, op_array->dynamic_func_defs[i])) {
			return true;
		}
	}

	return false;
}

/* Enter a reflection element: record its (kind, borrowed name, hook) descriptor
 * and restart the local rank. No allocation for elements merely passed through;
 * the "<site>@<rank>" string is built once from found_* on a match. */
#define WALK_ELEMENT(kind_, name_, hook_) \
	(w->cur_kind = (kind_), w->cur_name = (name_), w->cur_hook = (hook_), w->next_id = 0)

static bool zend_constexpr_closure_walk_class(zend_constexpr_closure_walk *w, zend_class_entry *ce)
{
	zend_string *cname, *pname;
	zend_class_constant *c;
	zend_property_info *prop_info;
	zend_function *func;

	if (ce->type != ZEND_USER_CLASS) {
		return false;
	}

	WALK_ELEMENT(ZEND_CEXPR_SITE_CLASS, NULL, 0);
	if (zend_constexpr_closure_walk_attributes(w, ce->attributes)) {
		return true;
	}

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&ce->constants_table, cname, c) {
		WALK_ELEMENT(ZEND_CEXPR_SITE_CONST, cname, 0);
		if (zend_constexpr_closure_walk_attributes(w, c->attributes)) {
			return true;
		}
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&ce->properties_info, pname, prop_info) {
		WALK_ELEMENT(ZEND_CEXPR_SITE_PROP, pname, 0);
		if (zend_constexpr_closure_walk_attributes(w, prop_info->attributes)) {
			return true;
		}
		if (prop_info->hooks) {
			for (uint32_t i = 0; i < ZEND_PROPERTY_HOOK_COUNT; i++) {
				if (prop_info->hooks[i]) {
					WALK_ELEMENT(ZEND_CEXPR_SITE_HOOK, pname, (uint8_t) i);
					if (zend_constexpr_closure_walk_op_array(w, &prop_info->hooks[i]->op_array)) {
						return true;
					}
				}
			}
		}
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, func) {
		WALK_ELEMENT(ZEND_CEXPR_SITE_METHOD, func->common.function_name, 0);
		if (zend_constexpr_closure_walk_op_array(w, &func->op_array)) {
			return true;
		}
	} ZEND_HASH_FOREACH_END();

	return false;
}

#undef WALK_ELEMENT

/* Materialize the opaque "<site>@<key>" reference from a found descriptor.
 * The key is the local rank for an anonymous closure, and for a first-class
 * callable the "Called::method" / "function" name key the walk searched for. */
static zend_string *zend_constexpr_closure_build_ref(const zend_constexpr_closure_walk *w)
{
	zend_string *key, *ref;

	if (w->found->kind == ZEND_AST_OP_ARRAY) {
		/* The rank is suffixed with the closure's code hash, so that both
		 * unserialize() and Closure::fromConstExpr() can verify that the rank
		 * still names the same code. */
		key = zend_strpprintf(0, "%u#%08x", (unsigned) w->found_id,
			(unsigned) zend_ast_get_op_array(w->found)->code_hash);
	} else {
		key = zend_string_init(w->target_key, w->target_key_len, 0);
	}

	switch (w->found_kind) {
		case ZEND_CEXPR_SITE_CONST:
			ref = zend_strpprintf(0, "%s@%s", ZSTR_VAL(w->found_name), ZSTR_VAL(key));
			break;
		case ZEND_CEXPR_SITE_PROP:
			ref = zend_strpprintf(0, "$%s@%s", ZSTR_VAL(w->found_name), ZSTR_VAL(key));
			break;
		case ZEND_CEXPR_SITE_HOOK:
			ref = zend_strpprintf(0, "$%s::%s()@%s", ZSTR_VAL(w->found_name),
				w->found_hook == ZEND_PROPERTY_HOOK_GET ? "get" : "set", ZSTR_VAL(key));
			break;
		case ZEND_CEXPR_SITE_METHOD:
			ref = zend_strpprintf(0, "%s()@%s", ZSTR_VAL(w->found_name), ZSTR_VAL(key));
			break;
		case ZEND_CEXPR_SITE_CLASS:
			ref = zend_strpprintf(0, "@%s", ZSTR_VAL(key));
			break;
		default:
			ZEND_UNREACHABLE();
	}

	zend_string_release_ex(key, 0);
	return ref;
}

/* Resolve (element site, local rank) to a declaration-site AST: locate the one
 * named element by a direct hash lookup, then walk only it. This is where
 * element-scoping pays off on decode: instead of a class-global scan to the
 * Nth site, resolution is bounded by a single element's const-expr surface. */
static zend_ast *zend_constexpr_closure_site_by_element(
	zend_class_entry *ce, const char *site, size_t site_len,
	uint32_t rank, const char *key, size_t key_len)
{
	zend_constexpr_closure_walk w = {0};

	if (ce->type != ZEND_USER_CLASS) {
		return NULL;
	}

	if (key) {
		w.target_key = key;
		w.target_key_len = key_len;
	} else {
		w.by_id = true;
		w.target_id = rank;
	}
	w.site_class = ce;

	if (site_len == 0) {
		/* class attributes */
		zend_constexpr_closure_walk_attributes(&w, ce->attributes);
		return w.found;
	}

	if (site[0] == '$') {
		/* property "$name" or hook "$name::get()" / "$name::set()" */
		const char *sep = NULL;
		for (size_t i = 1; i + 1 < site_len; i++) {
			if (site[i] == ':' && site[i + 1] == ':') {
				sep = site + i;
				break;
			}
		}
		size_t name_len = (sep ? (size_t) (sep - site) : site_len) - 1;
		zend_property_info *prop = zend_hash_str_find_ptr(&ce->properties_info, site + 1, name_len);
		if (!prop) {
			return NULL;
		}
		if (!sep) {
			zend_constexpr_closure_walk_attributes(&w, prop->attributes);
			return w.found;
		}
		const char *spec = sep + 2;
		size_t spec_len = site_len - (spec - site);
		uint32_t hook_kind;
		if (spec_len == 5 && memcmp(spec, "get()", 5) == 0) {
			hook_kind = ZEND_PROPERTY_HOOK_GET;
		} else if (spec_len == 5 && memcmp(spec, "set()", 5) == 0) {
			hook_kind = ZEND_PROPERTY_HOOK_SET;
		} else {
			return NULL;
		}
		if (!prop->hooks || !prop->hooks[hook_kind]
		 || prop->hooks[hook_kind]->type != ZEND_USER_FUNCTION) {
			return NULL;
		}
		zend_constexpr_closure_walk_op_array(&w, &prop->hooks[hook_kind]->op_array);
		return w.found;
	}

	if (site_len >= 2 && site[site_len - 2] == '(' && site[site_len - 1] == ')') {
		/* method "name()" */
		zend_function *func = zend_hash_str_find_ptr_lc(&ce->function_table, site, site_len - 2);
		if (!func || func->type != ZEND_USER_FUNCTION) {
			return NULL;
		}
		zend_constexpr_closure_walk_op_array(&w, &func->op_array);
		return w.found;
	}

	/* class constant / enum case "NAME" */
	zend_class_constant *c = zend_hash_str_find_ptr(&ce->constants_table, site, site_len);
	if (!c) {
		return NULL;
	}
	zend_constexpr_closure_walk_attributes(&w, c->attributes);
	return w.found;
}

/* Parse the opaque "<site>@<key>[#<hash>]" reference and resolve it. An
 * all-digits key is the local rank of an anonymous closure; anything else is
 * the name of a first-class callable ("Called::method" or "function"; names
 * cannot start with a digit, so the two forms cannot collide). A rank may be
 * suffixed with a "#<hash>" of the closure's code; *verify_hash receives it,
 * or 0 when absent (a producer that cannot compute it may omit it, trading
 * verification for positional resolution). Name keys carry no hash. */
static zend_ast *zend_constexpr_closure_site_by_ref(zend_class_entry *ce, zend_string *id, uint32_t *verify_hash)
{
	const char *s = ZSTR_VAL(id);
	size_t len = ZSTR_LEN(id);

	*verify_hash = 0;
	const char *sharp = zend_memrchr(s, '#', len);
	if (sharp) {
		if (len - (sharp - s) - 1 != 8) {
			return NULL;
		}
		uint32_t h = 0;
		for (size_t i = 1; i <= 8; i++) {
			char c = sharp[i];
			if (c >= '0' && c <= '9') {
				h = h << 4 | (c - '0');
			} else if (c >= 'a' && c <= 'f') {
				h = h << 4 | (c - 'a' + 10);
			} else {
				return NULL;
			}
		}
		if (!h) {
			/* The engine never produces a zero hash. */
			return NULL;
		}
		*verify_hash = h;
		len = sharp - s;
	}

	const char *at = zend_memrchr(s, '@', len);
	if (!at) {
		return NULL;
	}
	size_t site_len = at - s;
	const char *key = at + 1;
	size_t key_len = len - site_len - 1;
	if (key_len == 0) {
		return NULL;
	}

	bool is_rank = true;
	uint64_t rank = 0;
	for (size_t i = 0; i < key_len; i++) {
		if (key[i] < '0' || key[i] > '9') {
			is_rank = false;
			break;
		}
		rank = rank * 10 + (key[i] - '0');
		if (rank > UINT32_MAX) {
			return NULL;
		}
	}
	if (is_rank) {
		if (key_len > 1 && key[0] == '0') {
			return NULL;
		}
		return zend_constexpr_closure_site_by_element(ce, s, site_len, (uint32_t) rank, NULL, 0);
	}

	if (*verify_hash) {
		/* Name keys cannot drift and carry no hash. */
		return NULL;
	}

	return zend_constexpr_closure_site_by_element(ce, s, site_len, 0, key, key_len);
}

/* Instantiate the closure a declaration site declares, exactly as evaluating
 * its constant expression would: an anonymous closure from its op_array, a
 * first-class callable by re-evaluating the reference in the scope of the
 * declaring class, re-running its visibility checks. */
static zend_result zend_constexpr_closure_instantiate(zend_ast *site, zend_class_entry *ce, zval *result)
{
	if (site->kind == ZEND_AST_OP_ARRAY) {
		zend_create_closure(result, (zend_function *) zend_ast_get_op_array(site)->op_array, ce, ce, NULL);
		return SUCCESS;
	}
	return zend_ast_evaluate(result, site, ce);
}

ZEND_API zend_result zend_constexpr_closure_ref(zend_object *closure_obj, zend_class_entry **ce, zend_string **id)
{
	const zend_closure *closure = (const zend_closure *) closure_obj;
	zend_constexpr_closure_walk w = {0};
	zend_class_entry *site_class;
	zend_string *key = NULL;

	if (!(closure->func.common.fn_flags2 & ZEND_ACC2_CONSTEXPR_CLOSURE)) {
		return FAILURE;
	}

	if (!(closure->func.common.fn_flags & ZEND_ACC_FAKE_CLOSURE)) {
		/* Anonymous closure declared in a constant expression: identified by
		 * its op_array, found in the class it is scoped to. */
		if (closure->func.type != ZEND_USER_FUNCTION) {
			return FAILURE;
		}
		site_class = closure->func.common.scope;
		w.opcodes = closure->func.op_array.opcodes;
	} else {
		/* First-class callable evaluated in a constant expression: identified
		 * by the name key its reference serializes to, matched with the same
		 * comparison resolution uses, so the closure is serializable exactly
		 * when its reference resolves. */
		if (Z_TYPE(closure->this_ptr) != IS_UNDEF) {
			return FAILURE;
		}
		site_class = closure->constexpr_site;
		if (closure->func.common.scope && closure->called_scope) {
			zend_string *called = closure->called_scope->name;
			zend_string *method = closure->func.common.function_name;
			key = zend_string_concat3(
				ZSTR_VAL(called), ZSTR_LEN(called), "::", 2, ZSTR_VAL(method), ZSTR_LEN(method));
		} else if (!closure->func.common.scope) {
			key = zend_string_copy(closure->func.common.function_name);
		} else {
			return FAILURE;
		}
		w.target_key = ZSTR_VAL(key);
		w.target_key_len = ZSTR_LEN(key);
	}

	w.site_class = site_class;

	if (!site_class || site_class->type != ZEND_USER_CLASS || (site_class->ce_flags & ZEND_ACC_ANON_CLASS)
	 || !zend_constexpr_closure_walk_class(&w, site_class)) {
		if (key) {
			zend_string_release_ex(key, 0);
		}
		return FAILURE;
	}

	*ce = site_class;
	/* Opaque element-scoped reference "<site>@<key>[#<hash>]": site names the
	 * declaring reflection element; the key is a local rank, suffixed with a
	 * non-zero hash of the closure's code so that the reference cannot
	 * silently resolve to a different closure after the element's declarations
	 * were reordered, or a callable name, which cannot drift and carries no
	 * hash. Callers treat the whole string as an opaque token (pass it to
	 * Closure::fromConstExpr); the grammar is an engine detail. */
	*id = zend_constexpr_closure_build_ref(&w);
	if (key) {
		zend_string_release_ex(key, 0);
	}
	return SUCCESS;
}

ZEND_API void zend_closure_mark_as_constexpr_fcc(zval *closure_zv, zend_class_entry *site_class)
{
	zend_closure *closure = (zend_closure *) Z_OBJ_P(closure_zv);

	ZEND_ASSERT(closure->func.common.fn_flags & ZEND_ACC_FAKE_CLOSURE);
	closure->func.common.fn_flags2 |= ZEND_ACC2_CONSTEXPR_CLOSURE;
	closure->constexpr_site = site_class;
}

/* Serialize a closure as a reference to the declaration site of the
       constant expression that declared it: either an anonymous closure
       declaration, or a first-class callable reference */
ZEND_METHOD(Closure, __serialize)
{
	zend_class_entry *ce;
	zend_string *id;
	zval payload, tagged;

	ZEND_PARSE_PARAMETERS_NONE();

	if (zend_constexpr_closure_ref(Z_OBJ_P(ZEND_THIS), &ce, &id) == FAILURE) {
		/* Only closures declared in a constant expression are serializable
		 * (handled above); every other closure stays non-serializable, with
		 * the same message as before this feature existed. */
		zend_throw_exception(NULL, "Serialization of 'Closure' is not allowed", 0);
		RETURN_THROWS();
	}

	/* Tagged-union payload: [ <object properties>, [ <tag>, <tag payload> ] ].
	 * A Closure has no properties, so the first slot is an empty array; the
	 * tag names the reference kind so the format can grow new kinds without
	 * ambiguity. The "const-expr" payload is [class, id], the id carrying its
	 * own verification hash when the reference is positional. */
	array_init_size(&payload, 2);
	add_next_index_str(&payload, zend_string_copy(ce->name));
	add_next_index_str(&payload, id);

	array_init_size(&tagged, 2);
	add_next_index_string(&tagged, "const-expr");
	add_next_index_zval(&tagged, &payload);

	array_init_size(return_value, 2);
	add_next_index_array(return_value, zend_new_array(0));
	add_next_index_zval(return_value, &tagged);
}

/* Recreate a closure from a reference to its declaration site */
ZEND_METHOD(Closure, __unserialize)
{
	zend_closure *closure = (zend_closure *) Z_OBJ_P(ZEND_THIS);
	HashTable *data;
	zval *z_class, *z_id;
	zend_class_entry *ce;
	zend_ast *site;
	uint32_t verify_hash;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(data)
	ZEND_PARSE_PARAMETERS_END();

	if (closure->func.type != ZEND_INTERNAL_FUNCTION
	 || closure->func.internal_function.handler != zend_closure_uninitialized_handler) {
		zend_throw_exception(NULL, "Cannot unserialize an already initialized Closure", 0);
		RETURN_THROWS();
	}

	/* Tagged-union payload: [ <object properties>, [ <tag>, <tag payload> ] ].
	 * Only the "const-expr" tag is known; an unknown tag from a future format
	 * is rejected cleanly rather than misread. */
	zval *z_props, *z_tagged, *z_tag, *z_payload;
	HashTable *tagged, *payload;

	z_props = zend_hash_index_find(data, 0);
	z_tagged = zend_hash_index_find(data, 1);
	if (zend_hash_num_elements(data) != 2 || !z_props || !z_tagged) {
		zend_throw_exception(NULL, "Invalid serialization data for Closure object", 0);
		RETURN_THROWS();
	}
	ZVAL_DEREF(z_tagged);
	if (Z_TYPE_P(z_tagged) != IS_ARRAY) {
		zend_throw_exception(NULL, "Invalid serialization data for Closure object", 0);
		RETURN_THROWS();
	}
	tagged = Z_ARRVAL_P(z_tagged);
	z_tag = zend_hash_index_find(tagged, 0);
	z_payload = zend_hash_index_find(tagged, 1);
	if (zend_hash_num_elements(tagged) != 2 || !z_tag || !z_payload) {
		zend_throw_exception(NULL, "Invalid serialization data for Closure object", 0);
		RETURN_THROWS();
	}
	ZVAL_DEREF(z_tag);
	ZVAL_DEREF(z_payload);
	if (Z_TYPE_P(z_tag) != IS_STRING || !zend_string_equals_literal(Z_STR_P(z_tag), "const-expr")
	 || Z_TYPE_P(z_payload) != IS_ARRAY) {
		zend_throw_exception(NULL, "Invalid serialization data for Closure object", 0);
		RETURN_THROWS();
	}
	payload = Z_ARRVAL_P(z_payload);

	z_class = zend_hash_index_find(payload, 0);
	z_id = zend_hash_index_find(payload, 1);
	if (z_class) {
		ZVAL_DEREF(z_class);
	}
	if (z_id) {
		ZVAL_DEREF(z_id);
	}
	if (zend_hash_num_elements(payload) != 2
	 || !z_class || Z_TYPE_P(z_class) != IS_STRING
	 || !z_id || Z_TYPE_P(z_id) != IS_STRING) {
		zend_throw_exception(NULL, "Invalid serialization data for Closure object", 0);
		RETURN_THROWS();
	}

	/* Honor unserialize()'s allowed_classes: resolving the reference autoloads
	 * the named class and pulls a closure out of it, so a class the filter
	 * excludes must not be reachable through a Closure payload. Outside
	 * unserialize() (a direct __unserialize() call) the hook allows everything. */
	if (zend_unserialize_class_allowed && !zend_unserialize_class_allowed(Z_STR_P(z_class))) {
		zend_throw_exception_ex(NULL, 0,
			"Invalid serialization data for Closure object (class \"%s\" is not allowed)",
			Z_STRVAL_P(z_class));
		RETURN_THROWS();
	}

	ce = zend_lookup_class(Z_STR_P(z_class));
	if (!ce || ce->type != ZEND_USER_CLASS) {
		if (!EG(exception)) {
			zend_throw_exception_ex(NULL, 0,
				"Invalid serialization data for Closure object (cannot load class \"%s\")",
				Z_STRVAL_P(z_class));
		}
		RETURN_THROWS();
	}

	site = zend_constexpr_closure_site_by_ref(ce, Z_STR_P(z_id), &verify_hash);
	if (!site) {
		zend_throw_exception_ex(NULL, 0,
			"Invalid serialization data for Closure object (constant-expression closure \"%s\" of class %s not found)",
			Z_STRVAL_P(z_id), ZSTR_VAL(ce->name));
		RETURN_THROWS();
	}

	/* The id of a positional reference embeds a hash of the closure's code
	 * (the engine always writes one; a producer that cannot compute it may
	 * omit it): it guards against the rank now naming a different closure,
	 * e.g. after the element's declarations were reordered. */
	if (verify_hash && verify_hash != zend_ast_get_op_array(site)->code_hash) {
		zend_throw_exception_ex(NULL, 0,
			"Invalid serialization data for Closure object (constant-expression closure \"%s\" of class %s changed)",
			Z_STRVAL_P(z_id), ZSTR_VAL(ce->name));
		RETURN_THROWS();
	}

	zval tmp;
	zend_closure *tmp_closure;

	if (zend_constexpr_closure_instantiate(site, ce, &tmp) == FAILURE) {
		if (!EG(exception)) {
			zend_throw_exception(NULL, "Invalid serialization data for Closure object", 0);
		}
		RETURN_THROWS();
	}

	ZEND_ASSERT(Z_TYPE(tmp) == IS_OBJECT && Z_OBJCE(tmp) == zend_ce_closure);
	tmp_closure = (zend_closure *) Z_OBJ(tmp);

	/* Move the resolved closure into the object being unserialized. */
	bool is_fake = (tmp_closure->func.common.fn_flags & ZEND_ACC_FAKE_CLOSURE) != 0;
	zend_closure_init_ex(closure, &tmp_closure->func,
		tmp_closure->func.common.scope, tmp_closure->called_scope, NULL, is_fake);
	closure->constexpr_site = tmp_closure->constexpr_site;
	if (is_fake) {
		closure->func.common.fn_flags |= ZEND_ACC_FAKE_CLOSURE;
		GC_ADD_FLAGS(&closure->std, GC_NOT_COLLECTABLE);
	}

	zval_ptr_dtor(&tmp);
}

/* Recreate a closure declared in a constant expression of the given class */
ZEND_METHOD(Closure, fromConstExpr)
{
	zend_string *class_name;
	zend_string *id;
	zend_class_entry *ce;
	zend_ast *site;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(class_name)
		Z_PARAM_STR(id)
	ZEND_PARSE_PARAMETERS_END();

	ce = zend_lookup_class(class_name);
	if (!ce) {
		if (!EG(exception)) {
			zend_throw_error(NULL, "Class \"%s\" not found", ZSTR_VAL(class_name));
		}
		RETURN_THROWS();
	}

	uint32_t verify_hash;
	site = zend_constexpr_closure_site_by_ref(ce, id, &verify_hash);
	if (!site) {
		zend_value_error("Closure::fromConstExpr(): Argument #2 ($id) does not refer to a constant-expression closure of class %s", ZSTR_VAL(ce->name));
		RETURN_THROWS();
	}

	/* Ids embed a hash of the closure's code; generated code (e.g. exported
	 * cache files) gets the same staleness protection as serialized payloads. */
	if (verify_hash && verify_hash != zend_ast_get_op_array(site)->code_hash) {
		zend_value_error("Closure::fromConstExpr(): Argument #2 ($id) refers to a constant-expression closure of class %s that has changed", ZSTR_VAL(ce->name));
		RETURN_THROWS();
	}

	if (zend_constexpr_closure_instantiate(site, ce, return_value) == FAILURE) {
		if (!EG(exception)) {
			zend_value_error("Closure::fromConstExpr(): Argument #2 ($id) does not refer to a constant-expression closure of class %s", ZSTR_VAL(ce->name));
		}
		RETURN_THROWS();
	}
}

static ZEND_COLD zend_function *zend_closure_get_constructor(zend_object *object) /* {{{ */
{
	zend_throw_error(NULL, "Instantiation of class Closure is not allowed");
	return NULL;
}
/* }}} */

/* int return due to Object Handler API */
static int zend_closure_compare(zval *o1, zval *o2) /* {{{ */
{
	ZEND_COMPARE_OBJECTS_FALLBACK(o1, o2);

	zend_closure *lhs = (zend_closure*) Z_OBJ_P(o1);
	zend_closure *rhs = (zend_closure*) Z_OBJ_P(o2);

	if (!((lhs->func.common.fn_flags & ZEND_ACC_FAKE_CLOSURE) && (rhs->func.common.fn_flags & ZEND_ACC_FAKE_CLOSURE))) {
		return ZEND_UNCOMPARABLE;
	}

	if (Z_TYPE(lhs->this_ptr) != Z_TYPE(rhs->this_ptr)) {
		return ZEND_UNCOMPARABLE;
	}

	if (Z_TYPE(lhs->this_ptr) == IS_OBJECT && Z_OBJ(lhs->this_ptr) != Z_OBJ(rhs->this_ptr)) {
		return ZEND_UNCOMPARABLE;
	}

	if (lhs->called_scope != rhs->called_scope) {
		return ZEND_UNCOMPARABLE;
	}

	if (lhs->func.type != rhs->func.type) {
		return ZEND_UNCOMPARABLE;
	}

	if (lhs->func.common.scope != rhs->func.common.scope) {
		return ZEND_UNCOMPARABLE;
	}

	if (!zend_string_equals(lhs->func.common.function_name, rhs->func.common.function_name)) {
		return ZEND_UNCOMPARABLE;
	}

	return 0;
}
/* }}} */

ZEND_API zend_function *zend_get_closure_invoke_method(zend_object *object) /* {{{ */
{
	zend_closure *closure = (zend_closure *)object;
	zend_function *invoke = (zend_function*)emalloc(sizeof(zend_function));
	const uint32_t keep_flags =
		ZEND_ACC_RETURN_REFERENCE | ZEND_ACC_VARIADIC | ZEND_ACC_HAS_RETURN_TYPE | ZEND_ACC_DEPRECATED;

	invoke->common = closure->func.common;
	/* We return ZEND_INTERNAL_FUNCTION, but arg_info representation is the
	 * same as for ZEND_USER_FUNCTION (uses zend_string* instead of char*).
	 * This is not a problem, because ZEND_ACC_HAS_TYPE_HINTS is never set,
	 * and we won't check arguments on internal function. We also set
	 * ZEND_ACC_USER_ARG_INFO flag to prevent invalid usage by Reflection */
	invoke->type = ZEND_INTERNAL_FUNCTION;
	invoke->internal_function.fn_flags =
		ZEND_ACC_PUBLIC | ZEND_ACC_CALL_VIA_HANDLER | ZEND_ACC_NEVER_CACHE | (closure->func.common.fn_flags & keep_flags);
	if (closure->func.type != ZEND_INTERNAL_FUNCTION || (closure->func.common.fn_flags & ZEND_ACC_USER_ARG_INFO)) {
		invoke->internal_function.fn_flags |=
			ZEND_ACC_USER_ARG_INFO;
	}
	invoke->internal_function.handler = ZEND_MN(Closure___invoke);
	invoke->internal_function.doc_comment = NULL;
	invoke->internal_function.module = 0;
	invoke->internal_function.scope = zend_ce_closure;
	invoke->internal_function.function_name = ZSTR_KNOWN(ZEND_STR_MAGIC_INVOKE);
	return invoke;
}
/* }}} */

ZEND_API const zend_function *zend_get_closure_method_def(zend_object *obj) /* {{{ */
{
	zend_closure *closure = (zend_closure *) obj;
	return &closure->func;
}
/* }}} */

ZEND_API zval* zend_get_closure_this_ptr(zval *obj) /* {{{ */
{
	zend_closure *closure = (zend_closure *)Z_OBJ_P(obj);
	return &closure->this_ptr;
}
/* }}} */

static zend_function *zend_closure_get_method(zend_object **object, zend_string *method, const zval *key) /* {{{ */
{
	if (zend_string_equals_literal_ci(method, ZEND_INVOKE_FUNC_NAME)) {
		return zend_get_closure_invoke_method(*object);
	}

	return zend_std_get_method(object, method, key);
}
/* }}} */

static void zend_closure_free_storage(zend_object *object) /* {{{ */
{
	zend_closure *closure = (zend_closure *)object;

	zend_object_std_dtor(&closure->std);

	if (closure->func.type == ZEND_USER_FUNCTION) {
		/* We don't own the static variables of fake closures. */
		if (!(closure->func.op_array.fn_flags & ZEND_ACC_FAKE_CLOSURE)) {
			zend_destroy_static_vars(&closure->func.op_array);
		}
		destroy_op_array(&closure->func.op_array);
	} else if (closure->func.type == ZEND_INTERNAL_FUNCTION) {
		zend_string_release(closure->func.common.function_name);
	}

	if (Z_TYPE(closure->this_ptr) != IS_UNDEF) {
		zval_ptr_dtor(&closure->this_ptr);
	}
}
/* }}} */

static ZEND_COLD ZEND_NAMED_FUNCTION(zend_closure_uninitialized_handler) /* {{{ */
{
	zend_throw_error(NULL, "Cannot call an uninitialized Closure");
}
/* }}} */

static zend_object *zend_closure_new(zend_class_entry *class_type) /* {{{ */
{
	zend_closure *closure;

	closure = emalloc(sizeof(zend_closure));
	memset(closure, 0, sizeof(zend_closure));

	zend_object_std_init(&closure->std, class_type);

	/* Initialize the function as a safe placeholder. Until the closure is
	 * actually initialized through zend_closure_init_ex() it may be observed
	 * by userland code (e.g. while delayed __unserialize() calls are still
	 * pending) and must not crash any object handler. The placeholder mimics
	 * a fake internal closure with a handler that always throws. */
	closure->func.type = ZEND_INTERNAL_FUNCTION;
	closure->func.common.fn_flags = ZEND_ACC_PUBLIC | ZEND_ACC_CLOSURE | ZEND_ACC_FAKE_CLOSURE;
	closure->func.common.function_name = ZSTR_EMPTY_ALLOC();
	closure->func.internal_function.handler = zend_closure_uninitialized_handler;
	closure->orig_internal_handler = zend_closure_uninitialized_handler;

	return (zend_object*)closure;
}
/* }}} */

static zend_object *zend_closure_clone(zend_object *zobject) /* {{{ */
{
	zend_closure *closure = (zend_closure *)zobject;
	zval result;

	zend_create_closure(&result, &closure->func,
		closure->func.common.scope, closure->called_scope, &closure->this_ptr);
	((zend_closure *) Z_OBJ(result))->constexpr_site = closure->constexpr_site;
	return Z_OBJ(result);
}
/* }}} */

static zend_result zend_closure_get_closure(zend_object *obj, zend_class_entry **ce_ptr, zend_function **fptr_ptr, zend_object **obj_ptr, bool check_only) /* {{{ */
{
	zend_closure *closure = (zend_closure*)obj;

	*fptr_ptr = &closure->func;
	*ce_ptr = closure->called_scope;

	if (Z_TYPE(closure->this_ptr) != IS_UNDEF) {
		*obj_ptr = Z_OBJ(closure->this_ptr);
	} else {
		*obj_ptr = NULL;
	}

	return SUCCESS;
}
/* }}} */

/* *is_temp is int due to Object Handler API */
static HashTable *zend_closure_get_debug_info(zend_object *object, int *is_temp) /* {{{ */
{
	zend_closure *closure = (zend_closure *)object;
	zval val;
	struct _zend_arg_info *arg_info = closure->func.common.arg_info;
	HashTable *debug_info;

	*is_temp = 1;

	debug_info = zend_new_array(8);

	if (closure->func.op_array.fn_flags & ZEND_ACC_FAKE_CLOSURE) {
		if (closure->func.common.scope) {
			zend_string *class_name = closure->func.common.scope->name;
			zend_string *func_name = closure->func.common.function_name;
			zend_string *combined = zend_string_concat3(
				ZSTR_VAL(class_name), ZSTR_LEN(class_name),
				"::", strlen("::"),
				ZSTR_VAL(func_name), ZSTR_LEN(func_name)
			);
			ZVAL_STR(&val, combined);
		} else {
			ZVAL_STR_COPY(&val, closure->func.common.function_name);
		}
		zend_hash_update(debug_info, ZSTR_KNOWN(ZEND_STR_FUNCTION), &val);
	} else {
		ZVAL_STR_COPY(&val, closure->func.common.function_name);
		zend_hash_update(debug_info, ZSTR_KNOWN(ZEND_STR_NAME), &val);

		ZVAL_STR_COPY(&val, closure->func.op_array.filename);
		zend_hash_update(debug_info, ZSTR_KNOWN(ZEND_STR_FILE), &val);

		ZVAL_LONG(&val, closure->func.op_array.line_start);
		zend_hash_update(debug_info, ZSTR_KNOWN(ZEND_STR_LINE), &val);
	}

	if (closure->func.type == ZEND_USER_FUNCTION && closure->func.op_array.static_variables) {
		zval *var;
		zend_string *key;
		HashTable *static_variables = ZEND_MAP_PTR_GET(closure->func.op_array.static_variables_ptr);

		array_init(&val);

		ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(static_variables, key, var) {
			zval copy;

			if (Z_ISREF_P(var) && Z_REFCOUNT_P(var) == 1) {
				var = Z_REFVAL_P(var);
			}
			ZVAL_COPY(&copy, var);

			zend_hash_add_new(Z_ARRVAL(val), key, &copy);
		} ZEND_HASH_FOREACH_END();

		if (zend_hash_num_elements(Z_ARRVAL(val))) {
			zend_hash_update(debug_info, ZSTR_KNOWN(ZEND_STR_STATIC), &val);
		} else {
			zval_ptr_dtor(&val);
		}
	}

	if (Z_TYPE(closure->this_ptr) != IS_UNDEF) {
		Z_ADDREF(closure->this_ptr);
		zend_hash_update(debug_info, ZSTR_KNOWN(ZEND_STR_THIS), &closure->this_ptr);
	}

	if (arg_info &&
		(closure->func.common.num_args ||
		 (closure->func.common.fn_flags & ZEND_ACC_VARIADIC))) {
		uint32_t i, num_args, required = closure->func.common.required_num_args;

		array_init(&val);

		num_args = closure->func.common.num_args;
		if (closure->func.common.fn_flags & ZEND_ACC_VARIADIC) {
			num_args++;
		}
		for (i = 0; i < num_args; i++) {
			zend_string *name;
			zval info;
			ZEND_ASSERT(arg_info->name && "Argument should have name");
			name = zend_strpprintf(0, "%s$%s",
					ZEND_ARG_SEND_MODE(arg_info) ? "&" : "",
					ZSTR_VAL(arg_info->name));
			ZVAL_NEW_STR(&info, zend_strpprintf(0, "%s", i >= required ? "<optional>" : "<required>"));
			zend_hash_update(Z_ARRVAL(val), name, &info);
			zend_string_release_ex(name, 0);
			arg_info++;
		}
		zend_hash_str_update(debug_info, "parameter", sizeof("parameter")-1, &val);
	}

	return debug_info;
}
/* }}} */

static HashTable *zend_closure_get_gc(zend_object *obj, zval **table, int *n) /* {{{ */
{
	zend_closure *closure = (zend_closure *)obj;

	*table = Z_TYPE(closure->this_ptr) != IS_NULL ? &closure->this_ptr : NULL;
	*n = Z_TYPE(closure->this_ptr) != IS_NULL ? 1 : 0;
	/* Fake closures don't own the static variables they reference. */
	return (closure->func.type == ZEND_USER_FUNCTION
			&& !(closure->func.op_array.fn_flags & ZEND_ACC_FAKE_CLOSURE)) ?
		ZEND_MAP_PTR_GET(closure->func.op_array.static_variables_ptr) : NULL;
}
/* }}} */

/* {{{ Private constructor preventing instantiation */
ZEND_COLD ZEND_METHOD(Closure, __construct)
{
	zend_throw_error(NULL, "Instantiation of class Closure is not allowed");
}
/* }}} */

void zend_register_closure_ce(void) /* {{{ */
{
	zend_ce_closure = register_class_Closure();
	zend_ce_closure->create_object = zend_closure_new;
	zend_ce_closure->default_object_handlers = &closure_handlers;

	memcpy(&closure_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	closure_handlers.free_obj = zend_closure_free_storage;
	closure_handlers.get_constructor = zend_closure_get_constructor;
	closure_handlers.get_method = zend_closure_get_method;
	closure_handlers.compare = zend_closure_compare;
	closure_handlers.clone_obj = zend_closure_clone;
	closure_handlers.get_debug_info = zend_closure_get_debug_info;
	closure_handlers.get_closure = zend_closure_get_closure;
	closure_handlers.get_gc = zend_closure_get_gc;
}
/* }}} */

static ZEND_NAMED_FUNCTION(zend_closure_internal_handler) /* {{{ */
{
	zend_closure *closure = (zend_closure*)ZEND_CLOSURE_OBJECT(EX(func));
	closure->orig_internal_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	ZEND_ASSERT(!(closure->func.common.fn_flags2 & ZEND_ACC2_FORBID_DYN_CALLS) || EG(exception));
	// Assign to EX(this) so that it is released after observer checks etc.
	ZEND_ADD_CALL_FLAG(execute_data, ZEND_CALL_RELEASE_THIS);
	Z_OBJ(EX(This)) = &closure->std;
}
/* }}} */

static void zend_closure_init_ex(zend_closure *closure, zend_function *func, zend_class_entry *scope, zend_class_entry *called_scope, zval *this_ptr, bool is_fake) /* {{{ */
{
	void *ptr;

	if ((scope == NULL) && this_ptr && (Z_TYPE_P(this_ptr) != IS_UNDEF)) {
		/* use dummy scope if we're binding an object without specifying a scope */
		/* maybe it would be better to create one for this purpose */
		scope = zend_ce_closure;
	}

	if (func->type == ZEND_USER_FUNCTION) {
		memcpy(&closure->func, func, sizeof(zend_op_array));
		closure->func.common.fn_flags |= ZEND_ACC_CLOSURE;
		closure->func.common.fn_flags &= ~ZEND_ACC_IMMUTABLE;

		zend_string_addref(closure->func.op_array.function_name);
		if (closure->func.op_array.refcount) {
			(*closure->func.op_array.refcount)++;
		}

		/* For fake closures, we want to reuse the static variables of the original function. */
		HashTable *ht = ZEND_MAP_PTR_GET(func->op_array.static_variables_ptr);
		if (!is_fake) {
			if (!ht) {
				ht = closure->func.op_array.static_variables;
			}
			ZEND_MAP_PTR_INIT(closure->func.op_array.static_variables_ptr,
				ht ? zend_array_dup(ht) : NULL);
		} else if (func->op_array.static_variables) {
			if (!ht) {
				ht = zend_array_dup(func->op_array.static_variables);
				ZEND_MAP_PTR_SET(func->op_array.static_variables_ptr, ht);
			}
			ZEND_MAP_PTR_INIT(closure->func.op_array.static_variables_ptr, ht);
		}

		/* Runtime cache is scope-dependent, so we cannot reuse it if the scope changed */
		ptr = ZEND_MAP_PTR_GET(func->op_array.run_time_cache);
		if (!ptr
			|| func->common.scope != scope
			|| (func->common.fn_flags & ZEND_ACC_HEAP_RT_CACHE)
		) {
			if (!ptr
			 && (func->common.fn_flags & ZEND_ACC_CLOSURE)
			 && (func->common.scope == scope ||
			     !(func->common.fn_flags & ZEND_ACC_IMMUTABLE))) {
				/* If a real closure is used for the first time, we create a shared runtime cache
				 * and remember which scope it is for. */
				if (func->common.scope != scope) {
					func->common.scope = scope;
				}
				ptr = zend_arena_alloc(&CG(arena), func->op_array.cache_size);
				ZEND_MAP_PTR_SET(func->op_array.run_time_cache, ptr);
				closure->func.op_array.fn_flags &= ~ZEND_ACC_HEAP_RT_CACHE;
			} else {
				/* Otherwise, we use a non-shared runtime cache */
				ptr = emalloc(func->op_array.cache_size);
				closure->func.op_array.fn_flags |= ZEND_ACC_HEAP_RT_CACHE;
			}
			memset(ptr, 0, func->op_array.cache_size);
		}
		ZEND_MAP_PTR_INIT(closure->func.op_array.run_time_cache, ptr);
	} else {
		memcpy(&closure->func, func, sizeof(zend_internal_function));
		closure->func.common.fn_flags |= ZEND_ACC_CLOSURE;
		/* wrap internal function handler to avoid memory leak */
		if (UNEXPECTED(closure->func.internal_function.handler == zend_closure_internal_handler)) {
			/* avoid infinity recursion, by taking handler from nested closure */
			zend_closure *nested = ZEND_CONTAINER_OF(func, zend_closure, func);
			ZEND_ASSERT(nested->std.ce == zend_ce_closure);
			closure->orig_internal_handler = nested->orig_internal_handler;
		} else {
			closure->orig_internal_handler = closure->func.internal_function.handler;
		}
		closure->func.internal_function.handler = zend_closure_internal_handler;
		zend_string_addref(closure->func.op_array.function_name);
		if (!func->common.scope) {
			/* if it's a free function, we won't set scope & this since they're meaningless */
			this_ptr = NULL;
			scope = NULL;
		}
	}

	ZVAL_UNDEF(&closure->this_ptr);
	/* Invariant:
	 * If the closure is unscoped or static, it has no bound object. */
	closure->func.common.scope = scope;
	closure->called_scope = called_scope;
	if (scope) {
		closure->func.common.fn_flags |= ZEND_ACC_PUBLIC;
		if (this_ptr && Z_TYPE_P(this_ptr) == IS_OBJECT && (closure->func.common.fn_flags & ZEND_ACC_STATIC) == 0) {
			ZVAL_OBJ_COPY(&closure->this_ptr, Z_OBJ_P(this_ptr));
		}
	}
}
/* }}} */

static void zend_create_closure_ex(zval *res, zend_function *func, zend_class_entry *scope, zend_class_entry *called_scope, zval *this_ptr, bool is_fake) /* {{{ */
{
	object_init_ex(res, zend_ce_closure);
	zend_closure_init_ex((zend_closure *) Z_OBJ_P(res), func, scope, called_scope, this_ptr, is_fake);
}
/* }}} */

ZEND_API void zend_create_closure(zval *res, zend_function *func, zend_class_entry *scope, zend_class_entry *called_scope, zval *this_ptr)
{
	zend_create_closure_ex(res, func, scope, called_scope, this_ptr,
		/* is_fake */ (func->common.fn_flags & ZEND_ACC_FAKE_CLOSURE) != 0);
}

ZEND_API void zend_create_fake_closure(zval *res, zend_function *func, zend_class_entry *scope, zend_class_entry *called_scope, zval *this_ptr) /* {{{ */
{
	zend_closure *closure;

	zend_create_closure_ex(res, func, scope, called_scope, this_ptr, /* is_fake */ true);

	closure = (zend_closure *)Z_OBJ_P(res);
	closure->func.common.fn_flags |= ZEND_ACC_FAKE_CLOSURE;
	if (Z_TYPE(closure->this_ptr) != IS_OBJECT) {
		GC_ADD_FLAGS(&closure->std, GC_NOT_COLLECTABLE);
	}
}
/* }}} */

void zend_closure_from_frame(zval *return_value, const zend_execute_data *call) { /* {{{ */
	zval instance;
	zend_internal_function trampoline;
	zend_function *mptr = call->func;

	if (ZEND_CALL_INFO(call) & ZEND_CALL_CLOSURE) {
		RETURN_OBJ(ZEND_CLOSURE_OBJECT(mptr));
	}

	if (mptr->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
		if ((ZEND_CALL_INFO(call) & ZEND_CALL_HAS_THIS) &&
			(Z_OBJCE(call->This) == zend_ce_closure)
			&& zend_string_equals(mptr->common.function_name, ZSTR_KNOWN(ZEND_STR_MAGIC_INVOKE))) {
	        zend_free_trampoline(mptr);
	        RETURN_OBJ_COPY(Z_OBJ(call->This));
	    }

		memset(&trampoline, 0, sizeof(zend_internal_function));
		trampoline.type = ZEND_INTERNAL_FUNCTION;
		trampoline.fn_flags = mptr->common.fn_flags & (ZEND_ACC_STATIC | ZEND_ACC_VARIADIC | ZEND_ACC_RETURN_REFERENCE | ZEND_ACC_DEPRECATED);
		trampoline.handler = zend_closure_call_magic;
		trampoline.function_name = mptr->common.function_name;
		trampoline.scope = mptr->common.scope;
		trampoline.doc_comment = NULL;
		trampoline.arg_info = mptr->common.arg_info;
		trampoline.attributes = mptr->common.attributes;

		zend_free_trampoline(mptr);
		mptr = (zend_function *) &trampoline;
	}

	if (ZEND_CALL_INFO(call) & ZEND_CALL_HAS_THIS) {
		ZVAL_OBJ(&instance, Z_OBJ(call->This));

		zend_create_fake_closure(return_value, mptr, mptr->common.scope, Z_OBJCE(instance), &instance);
	} else {
		zend_create_fake_closure(return_value, mptr, mptr->common.scope, Z_CE(call->This), NULL);
	}

	if (&mptr->internal_function == &trampoline) {
		zend_string_release(mptr->common.function_name);
	}
} /* }}} */

void zend_closure_bind_var(zval *closure_zv, zend_string *var_name, zval *var) /* {{{ */
{
	zend_closure *closure = (zend_closure *) Z_OBJ_P(closure_zv);
	HashTable *static_variables = ZEND_MAP_PTR_GET(closure->func.op_array.static_variables_ptr);
	zend_hash_update(static_variables, var_name, var);
}
/* }}} */

void zend_closure_bind_var_ex(zval *closure_zv, uint32_t offset, zval *val) /* {{{ */
{
	zend_closure *closure = (zend_closure *) Z_OBJ_P(closure_zv);
	HashTable *static_variables = ZEND_MAP_PTR_GET(closure->func.op_array.static_variables_ptr);
	zval *var = (zval*)((char*)static_variables->arData + offset);
	zval_ptr_dtor(var);
	ZVAL_COPY_VALUE(var, val);
}
/* }}} */
