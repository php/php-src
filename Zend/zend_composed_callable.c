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
   | Authors: Sara Golemon <pollita@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_composed_callable.h"
#include "zend_exceptions.h"

#define ZEND_COMPOSED_CALLABLE_INITIAL_CAPACITY 8

ZEND_API zend_class_entry *zend_ce_composed_callable;
static zend_object_handlers zend_composed_callable_handlers;

typedef struct {
	HashTable callables;
	zend_object std;
} zend_composed_callable;

static zend_fcall_info_cache* zend_composed_callable_fcc_from_zval(zval* pzv) {
	ZEND_ASSERT(Z_TYPE_P(pzv) == IS_PTR);
	return Z_PTR_P(pzv);
}

static zend_object* zend_composed_callable_to_zend_object(zend_composed_callable* objval) {
	return ((zend_object*)(objval + 1)) - 1;
}

static zend_composed_callable* zend_composed_callable_from_zend_object(zend_object* zobj) {
	return ((zend_composed_callable*)(zobj + 1)) - 1;
}

static zend_result zend_composed_callable_append1(HashTable *callables, zval *callable) {
	zend_fcall_info_cache *fcc = ecalloc(1, sizeof(zend_fcall_info_cache));
	char *error = NULL;
	zval ptr;

	if (!zend_is_callable_ex(callable, NULL, 0, NULL, fcc, &error)) {
		if (error) {
			zend_throw_exception_ex(zend_ce_value_error, 0, "Argument not valid callback: %s", error);
			efree(error);
		} else {
			zend_throw_exception(zend_ce_value_error, "Unable to initialize callback", 0);
		}
		efree(fcc);
		return FAILURE;
	}

	ZVAL_PTR(&ptr, fcc);
	zend_fcc_addref(fcc);
	zend_hash_next_index_insert(callables, &ptr);

	return SUCCESS;
}

static zend_result zend_composed_callable_splice(HashTable *dst, HashTable *src, zend_long pos, zval *insert);
static zend_result zend_composed_callable_append(HashTable *callables, zval *callable) {
	if (Z_TYPE_P(callable) == IS_ARRAY) {
		zval *fn;

		ZEND_HASH_FOREACH_VAL(Z_ARR_P(callable), fn) {
			if (zend_composed_callable_append1(callables, fn) == FAILURE) {
				return FAILURE;
			}
		} ZEND_HASH_FOREACH_END();

		return SUCCESS;
	} else if ((Z_TYPE_P(callable) == IS_OBJECT) && instanceof_function(Z_OBJCE_P(callable), zend_ce_composed_callable)) {
		HashTable *src_callables = &(zend_composed_callable_from_zend_object(Z_OBJ_P(callable))->callables);
		return zend_composed_callable_splice(callables, src_callables, 0, NULL);
	} else {
		return zend_composed_callable_append1(callables, callable);
	}
}

ZEND_METHOD(ComposedCallable, __construct) {
	HashTable *callables = &(zend_composed_callable_from_zend_object(Z_OBJ_P(getThis()))->callables);
	zval *callable;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &callable) == FAILURE) {
		RETURN_THROWS();
	}

	if (zend_composed_callable_append(callables, callable) == FAILURE) {
		RETURN_THROWS();
	}
}

ZEND_METHOD(ComposedCallable, __invoke) {
	HashTable *callables = &(zend_composed_callable_from_zend_object(Z_OBJ_P(getThis()))->callables);
	zval *argv, *callable;
	uint32_t argc = 0, i;

	if (zend_hash_num_elements(callables) < 1) {
		zend_throw_exception(zend_ce_value_error, "Attempt to invoke an empty ComposedCallable", 0);
		RETURN_THROWS();
	}

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "*", &argv, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	/* Prevent destruction of initial args during intermediate forwarding. */
	for (i = 0; i < argc; ++i) {
		Z_TRY_ADDREF(argv[i]);
	}

	ZEND_HASH_FOREACH_VAL(callables, callable) {
		zend_fcall_info_cache *fcc = zend_composed_callable_fcc_from_zval(callable);
		zval retval;

		zend_call_known_fcc(fcc, &retval, argc, argv, NULL);

		/* Clean up the prior call vars. */
		for (i = 0; i < argc; ++i) {
			zval_ptr_dtor(&(argv[i]));
		}

		/* Forward retval to next invocation. */
		argv[0] = retval;
		argc = 1;
	} ZEND_HASH_FOREACH_END();

	/* The check at the top of this method ensures we call at least one callable,
	 * and every callable has precisely one return value (even if it's NULL).
	 */
	ZEND_ASSERT(argc == 1);
	RETURN_ZVAL(&(argv[0]), 1, 0);
}

static zend_result zend_composed_callable_splice(HashTable *dst, HashTable *src, zend_long pos, zval *insert) {
	zval *src_callable;
	ZEND_HASH_FOREACH_VAL(src, src_callable) {
		if (insert && pos-- == 0) {
			if (zend_composed_callable_append(dst, insert) == FAILURE) {
				return FAILURE;
			}
			insert = NULL;
			/* fallthrough to continue splice */
		}

		{
			zend_fcall_info_cache *src_fcc = zend_composed_callable_fcc_from_zval(src_callable);
			zend_fcall_info_cache *dst_fcc = ecalloc(1, sizeof(zend_fcall_info_cache));
			zval dst_callable;
			zend_fcc_dup(dst_fcc, src_fcc);
			ZVAL_PTR(&dst_callable, dst_fcc);
			zend_hash_next_index_insert(dst, &dst_callable);
		}
	} ZEND_HASH_FOREACH_END();

	/* append splice */
	if (insert && (zend_composed_callable_append(dst, insert) == FAILURE)) {
		return FAILURE;
	}

	return SUCCESS;
}

static void zend_composed_callable_fcc_dtor(zval *ptr) {
	zend_fcall_info_cache *fcc = zend_composed_callable_fcc_from_zval(ptr);
	zend_fcc_dtor(fcc);
	efree(fcc);
}

static zend_result zend_composed_callable_do_insert(HashTable *callables, zend_long pos, zval *callable) {
	zend_long num_callables = zend_hash_num_elements(callables);

	if (pos < 0) {
		/* For "-2" meaning "right before the last one" kind of semantics */
		pos += num_callables;
	}

	if (pos < 0) {
		pos = 0;
		/* Maybe error? */
	}

	if (pos > num_callables) {
		pos = num_callables;
		/* Maybe error? */
	}

	if (pos == num_callables) {
		/* Quick method. This is just append() with more steps. */
		return zend_composed_callable_append(callables, callable);
	}

	/* Slow path, rebuild the hashtable :( */
	HashTable newtable;
	zend_hash_init(&newtable, ZEND_COMPOSED_CALLABLE_INITIAL_CAPACITY, NULL, zend_composed_callable_fcc_dtor, 0);
	if (zend_composed_callable_splice(&newtable, callables, pos, callable) == FAILURE) {
		zend_hash_destroy(&newtable);
		return FAILURE;
	}

	zend_hash_destroy(callables);
	*callables = newtable;
	return SUCCESS;
}

ZEND_METHOD(ComposedCallable, insert) {
	HashTable *callables = &(zend_composed_callable_from_zend_object(Z_OBJ_P(getThis()))->callables);
	zval *callable;
	zend_long pos;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zl", &callable, &pos) == FAILURE) {
		RETURN_THROWS();
	}

	if (zend_composed_callable_do_insert(callables, pos, callable) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(ComposedCallable, prepend) {
	HashTable *callables = &(zend_composed_callable_from_zend_object(Z_OBJ_P(getThis()))->callables);
	zval *callable;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &callable) == FAILURE) {
		RETURN_THROWS();
	}

	if (zend_composed_callable_do_insert(callables, 0, callable) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(ComposedCallable, append) {
    HashTable *callables = &(zend_composed_callable_from_zend_object(Z_OBJ_P(getThis()))->callables);
    zval *callable;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &callable) == FAILURE) {
        RETURN_THROWS();
    }

    if (zend_composed_callable_append(callables, callable) == FAILURE) {
        RETURN_THROWS();
    }

	RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_API zend_result zend_composed_callable_new_from_pair(zval *result, zval *callable1, zval *callable2) {
	zend_object *ret = zend_ce_composed_callable->create_object(zend_ce_composed_callable);
	HashTable *callables = &(zend_composed_callable_from_zend_object(ret)->callables);

	if ((zend_composed_callable_append(callables, callable1) == FAILURE) ||
		(zend_composed_callable_append(callables, callable2) == FAILURE)) {
		return FAILURE;
	}

	ZVAL_OBJ(result, ret);
	return SUCCESS;
}

static zend_result zend_composed_callable_do_operation(uint8_t opcode, zval *result, zval *op1, zval *op2) {
	bool append;

	if (opcode != ZEND_ADD) {
		return FAILURE;
	}

	if ((Z_TYPE_P(op1) == IS_OBJECT) && instanceof_function(Z_OBJCE_P(op1), zend_ce_composed_callable)) {
		/* composed + callableOrComposed
		 * Clone op1's invocations into new result,
		 * then do standard append on op2, whatever it is.
		 */
		append = true;
	} else {
		ZEND_ASSERT((Z_TYPE_P(op2) == IS_OBJECT) && instanceof_function(Z_OBJCE_P(op2), zend_ce_composed_callable));
		/* callable + composed
		 * Start with just the initial callable, then clone in rhs' entries.
		 */
		append = false;
	}

	{
		zend_object *composed_obj = append ? Z_OBJ_P(op1) : Z_OBJ_P(op2);
		HashTable *src_callables = &(zend_composed_callable_from_zend_object(composed_obj)->callables);
		uint32_t pos = append ? zend_hash_num_elements(src_callables) : 0;

		zend_object *ret = composed_obj->ce->create_object(composed_obj->ce);
		HashTable *dst_callables = &(zend_composed_callable_from_zend_object(ret)->callables);

		zval *insert_obj = append ? op2 : op1;

		ZVAL_OBJ(result, ret);
		if (zend_composed_callable_splice(dst_callables, src_callables, pos, insert_obj) == FAILURE) {
			zval_ptr_dtor(result);
			ZVAL_UNDEF(result);
			return FAILURE;
		}

		return SUCCESS;
	}
}

ZEND_METHOD(ComposedCallable, __debugInfo) {
	HashTable *callables = &(zend_composed_callable_from_zend_object(Z_OBJ_P(getThis()))->callables);

	if (zend_parse_parameters_none_throw() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);
	{
		zval callables_names, *callable;
		array_init(&callables_names);
		ZEND_HASH_FOREACH_VAL(callables, callable) {
			zend_string *name = zend_composed_callable_fcc_from_zval(callable)->function_handler->common.function_name;
			add_next_index_str(&callables_names, name);
		} ZEND_HASH_FOREACH_END();
		add_assoc_zval(return_value, "callables", &callables_names);
	}
}

static zend_object *zend_composed_callable_create(zend_class_entry *ce) {
	zend_composed_callable *objval = ecalloc(1, sizeof(zend_composed_callable) + zend_object_properties_size(ce));
	zend_object* ret = zend_composed_callable_to_zend_object(objval);
	zend_object_std_init(ret, ce);
	zend_hash_init(&objval->callables, ZEND_COMPOSED_CALLABLE_INITIAL_CAPACITY, NULL, zend_composed_callable_fcc_dtor, 0);
	ret->handlers = &zend_composed_callable_handlers;
	return ret;
}

static zend_object* zend_composed_callable_clone(zend_object* zsrc) {
    zend_object *zdst = zsrc->ce->create_object(zsrc->ce);
    zend_objects_clone_members(zdst, zsrc);

    zend_composed_callable *src = zend_composed_callable_from_zend_object(zsrc);
    zend_composed_callable *dst = zend_composed_callable_from_zend_object(zdst);

	zend_composed_callable_splice(&dst->callables, &src->callables, 0, NULL);
	return zdst;
}

static void zend_composed_callable_free(zend_object *zobj) {
	zend_composed_callable *obj = zend_composed_callable_from_zend_object(zobj);
	zend_object_std_dtor(zobj);
	zend_hash_destroy(&obj->callables);
	efree(obj);
}

void zend_register_composed_callable(zend_class_entry *ce) {
	zend_ce_composed_callable = ce;
	ce->create_object = zend_composed_callable_create;

	memcpy(&zend_composed_callable_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	zend_composed_callable_handlers.offset = XtOffsetOf(zend_composed_callable, std);
	zend_composed_callable_handlers.clone_obj = zend_composed_callable_clone;
	zend_composed_callable_handlers.free_obj = zend_composed_callable_free;

	zend_composed_callable_handlers.do_operation = zend_composed_callable_do_operation;
}
