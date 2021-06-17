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
   | Authors: krakjoe <krakjoe@php.net>                                   |
   +----------------------------------------------------------------------+
*/
#include "zend.h"
#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_closures.h"
#include "zend_partial.h"

typedef struct _zend_partial {
	zend_object      std;
	zend_function    prototype;
	zval             This;
	zend_function    func;
	zend_function    trampoline;
	uint32_t         argc;
	zval            *argv;
	zend_array      *named;
} zend_partial;

static zend_object_handlers zend_partial_handlers;

static zend_arg_info zend_call_magic_arginfo[1];

#define Z_IS_PLACEHOLDER_ARG_P(p) \
	(Z_TYPE_P(p) == _IS_PLACEHOLDER_ARG)

#define Z_IS_PLACEHOLDER_VARIADIC_P(p) \
	(Z_TYPE_P(p) == _IS_PLACEHOLDER_VARIADIC)

#define Z_IS_PLACEHOLDER_P(p) \
	(Z_IS_PLACEHOLDER_ARG_P(p) || Z_IS_PLACEHOLDER_VARIADIC_P(p))

#define Z_IS_NOT_PLACEHOLDER_P(p) \
	(!Z_IS_PLACEHOLDER_ARG_P(p) && !Z_IS_PLACEHOLDER_VARIADIC_P(p))

#define ZEND_PARTIAL_IS_CALL_TRAMPOLINE(func) \
	UNEXPECTED(((func)->type == ZEND_USER_FUNCTION) && ((func)->op_array.opcodes == &EG(call_trampoline_op)))

#define ZEND_PARTIAL_FUNC_SIZE(func) \
	(((func)->type == ZEND_INTERNAL_FUNCTION) ? sizeof(zend_internal_function) : sizeof(zend_op_array))

#define ZEND_PARTIAL_FUNC_FLAG(func, flags) \
	((func)->common.fn_flags & flags)

#define ZEND_PARTIAL_FUNC_DEL(func, flag) \
	((func)->common.fn_flags &= ~flag)

#define ZEND_PARTIAL_FUNC_ADD(func, flag) \
	((func)->common.fn_flags |= flag)

#define ZEND_PARTIAL_CALL_FLAG(partial, flag) \
	(ZEND_CALL_INFO(partial) & flag)

static zend_always_inline uint32_t zend_partial_signature_size(zend_partial *partial) {
	uint32_t count = MAX(partial->func.common.num_args, partial->argc);

	if (ZEND_PARTIAL_FUNC_FLAG(&partial->func, ZEND_ACC_HAS_RETURN_TYPE)) {
		count++;
	}

	if (ZEND_PARTIAL_CALL_FLAG(partial, ZEND_APPLY_VARIADIC)||
		ZEND_PARTIAL_FUNC_FLAG(&partial->func, ZEND_ACC_VARIADIC)) {
		count++;
	}

	return count * sizeof(zend_arg_info);
}

#define ZEND_PARTIAL_SIGNATURE_SIZE(partial) zend_partial_signature_size(partial)

static zend_always_inline zend_function* zend_partial_signature_create(zend_partial *partial, zend_function *prototype) {
	zend_arg_info *signature = emalloc(ZEND_PARTIAL_SIGNATURE_SIZE(partial)), *info = signature;

	memcpy(&partial->prototype, prototype, ZEND_PARTIAL_FUNC_SIZE(prototype));

	if (ZEND_PARTIAL_FUNC_FLAG(prototype, ZEND_ACC_HAS_RETURN_TYPE)) {
		memcpy(info,
			prototype->common.arg_info - 1, sizeof(zend_arg_info));
		info++;
	}

	uint32_t offset = 0, num = 0, required = 0, limit = partial->argc;

	ZEND_PARTIAL_FUNC_DEL(&partial->prototype, ZEND_ACC_VARIADIC);

	while (offset < limit) {
		zval *arg = &partial->argv[offset];

		if (Z_IS_PLACEHOLDER_ARG_P(arg)) {
			if (offset < prototype->common.num_args) {
				num++;
				required++;
				memcpy(info,
					&prototype->common.arg_info[offset], 
					sizeof(zend_arg_info));
				ZEND_TYPE_FULL_MASK(info->type) &= ~_ZEND_IS_VARIADIC_BIT;
				info++;
			} else if (ZEND_PARTIAL_FUNC_FLAG(prototype, ZEND_ACC_VARIADIC)) {
				num++;
				required++;
				if (ZEND_PARTIAL_IS_CALL_TRAMPOLINE(prototype)) {
					memcpy(info, zend_call_magic_arginfo, sizeof(zend_arg_info));
				} else {
					memcpy(info,
						prototype->common.arg_info + prototype->common.num_args,
						sizeof(zend_arg_info));
				}

				ZEND_TYPE_FULL_MASK(info->type) &= ~_ZEND_IS_VARIADIC_BIT;
				info++;
			} else {
				break;
			}
		} else if (Z_IS_PLACEHOLDER_VARIADIC_P(arg) || Z_ISUNDEF_P(arg)) {
			if (offset < partial->func.common.num_args) {
				while (offset < partial->func.common.num_args) {
					if ((offset < partial->argc) &&
						!Z_IS_PLACEHOLDER_P(&partial->argv[offset]) && 
						!Z_ISUNDEF(partial->argv[offset])) {
						offset++;
						continue;
					}

					num++;
					memcpy(info,
						&partial->func.common.arg_info[offset], 
						sizeof(zend_arg_info));
					ZEND_TYPE_FULL_MASK(info->type) &= ~_ZEND_IS_VARIADIC_BIT;
					info++;
					offset++;
				}

				if (ZEND_PARTIAL_FUNC_FLAG(prototype, ZEND_ACC_VARIADIC)) {
					ZEND_ASSERT(!ZEND_PARTIAL_IS_CALL_TRAMPOLINE(prototype));
					memcpy(info,
							prototype->common.arg_info + prototype->common.num_args,
							sizeof(zend_arg_info));

					if (ZEND_TYPE_FULL_MASK(info->type) & _ZEND_IS_VARIADIC_BIT) {
						ZEND_PARTIAL_FUNC_ADD(&partial->prototype, ZEND_ACC_VARIADIC);
					}
					num++;
				}
				break;
			} else if (ZEND_PARTIAL_FUNC_FLAG(prototype, ZEND_ACC_VARIADIC)) {
				ZEND_PARTIAL_FUNC_ADD(&partial->prototype, ZEND_ACC_VARIADIC);
				num++;
				if (ZEND_PARTIAL_IS_CALL_TRAMPOLINE(prototype)) {
					memcpy(info, zend_call_magic_arginfo, sizeof(zend_arg_info));
				} else {
					memcpy(info,
						prototype->common.arg_info + prototype->common.num_args,
						sizeof(zend_arg_info));
				}
				info++;
				break;
			}
		}

		offset++;
	}

	if (ZEND_PARTIAL_FUNC_FLAG(&partial->prototype, ZEND_ACC_VARIADIC)) {
		num--;
	}

	partial->prototype.common.num_args = num;
	partial->prototype.common.required_num_args = required;
	partial->prototype.common.arg_info = signature;

	if (ZEND_PARTIAL_FUNC_FLAG(&partial->prototype, ZEND_ACC_HAS_RETURN_TYPE)) {
		partial->prototype.common.arg_info++;
	}

	ZEND_PARTIAL_FUNC_ADD(&partial->prototype, ZEND_ACC_PARTIAL);

	partial->prototype.common.prototype = &partial->func;

	if (prototype->type == ZEND_INTERNAL_FUNCTION) {
		return &partial->prototype;
	}

	zend_string *filename = zend_get_executed_filename_ex();

	if (!filename) {
		if (partial->prototype.op_array.filename) {
			zend_string_addref(partial->prototype.op_array.filename);
		}
		return &partial->prototype;
	}

	partial->prototype.op_array.filename = zend_string_copy(filename);
	partial->prototype.op_array.line_start =
		partial->prototype.op_array.line_end = zend_get_executed_lineno();

	return &partial->prototype;
}

static zend_always_inline zend_partial* zend_partial_fetch(zval *This) {
	if (!This || Z_TYPE_P(This) != IS_OBJECT) {
		return NULL;
	}

	if (UNEXPECTED(!instanceof_function(Z_OBJCE_P(This), zend_ce_closure))) {
		return NULL;
	}

	zend_partial *ptr = (zend_partial*) Z_OBJ_P(This);

	if (!ZEND_PARTIAL_FUNC_FLAG(&ptr->prototype, ZEND_ACC_PARTIAL)) {
		return NULL;
	}

	return ptr;
}

static zend_always_inline void zend_partial_trampoline_create(zend_partial *partial, zend_function *trampoline)
{
	const uint32_t keep_flags =
		ZEND_ACC_RETURN_REFERENCE | ZEND_ACC_VARIADIC | ZEND_ACC_HAS_RETURN_TYPE | ZEND_ACC_STRICT_TYPES;

	trampoline->common = partial->prototype.common;
	trampoline->type = ZEND_INTERNAL_FUNCTION;
	trampoline->internal_function.fn_flags =
		ZEND_ACC_PUBLIC | ZEND_ACC_CALL_VIA_HANDLER | ZEND_ACC_TRAMPOLINE_PERMANENT | (partial->func.common.fn_flags & keep_flags);
	if (partial->func.type != ZEND_INTERNAL_FUNCTION || (partial->func.common.fn_flags & ZEND_ACC_USER_ARG_INFO)) {
		trampoline->internal_function.fn_flags |=
			ZEND_ACC_USER_ARG_INFO;
	}
	trampoline->internal_function.handler = zend_partial_call_magic;
	trampoline->internal_function.module = 0;
	trampoline->internal_function.scope = zend_ce_closure;
	trampoline->internal_function.function_name = ZSTR_KNOWN(ZEND_STR_MAGIC_INVOKE);
}

static zend_always_inline zend_object* zend_partial_new(zend_class_entry *type, uint32_t info) {
	zend_partial *partial = ecalloc(1, sizeof(zend_partial));

	zend_object_std_init(&partial->std, type);

	partial->std.handlers = &zend_partial_handlers;

	ZEND_ADD_CALL_FLAG(partial, info);

	return (zend_object*) partial;
}

static zend_always_inline void zend_partial_debug_add(zend_function *function, HashTable *ht, zend_arg_info *info, zval *value) {
	if (function->type == ZEND_USER_FUNCTION || ZEND_PARTIAL_FUNC_FLAG(function, ZEND_ACC_USER_ARG_INFO)) {
		zend_hash_add(ht, info->name, value);
	} else {
		zend_internal_arg_info *internal = (zend_internal_arg_info*) info;

		zend_hash_str_add(ht, internal->name, strlen(internal->name), value);
	}
}

static zend_always_inline void zend_partial_debug_fill(zend_partial *partial, HashTable *ht) {
	zval *arg = partial->argv,
		 *aend = arg + partial->argc;
	zend_arg_info *info = partial->func.common.arg_info,
				  *iend = info + partial->func.common.num_args;

	while (info < iend) {
		zval param;

		ZVAL_NULL(&param);

		if (arg < aend) {
			if (Z_IS_NOT_PLACEHOLDER_P(arg)) {
				ZVAL_COPY_VALUE(&param, arg);
			}

			arg++;
		}

		zend_partial_debug_add(&partial->func, ht, info, &param);

		if (Z_OPT_REFCOUNTED(param)) {
			Z_ADDREF(param);
		}
		info++;
	}

	if (ZEND_PARTIAL_FUNC_FLAG(&partial->func, ZEND_ACC_VARIADIC)) {
		zval variadics;

		array_init(&variadics);

		zend_partial_debug_add(&partial->func, ht, info, &variadics);

		while (arg < aend) {
			if (Z_IS_NOT_PLACEHOLDER_P(arg)) {
				zend_hash_next_index_insert(Z_ARRVAL(variadics), arg);

				if (Z_OPT_REFCOUNTED_P(arg)) {
					Z_ADDREF_P(arg);
				}
			}

			arg++;
		}

		if (partial->named) {
			zend_hash_merge(Z_ARRVAL(variadics), partial->named, zval_copy_ctor, true);
		}
	}
}

static HashTable *zend_partial_debug(zend_object *object, int *is_temp) {
	zend_partial *partial = (zend_partial*) object;
	zval args;
	HashTable *ht;

	ht = zend_closure_get_debug_info(object, is_temp);

	array_init(&args);
	zend_hash_update(ht, ZSTR_KNOWN(ZEND_STR_ARGS), &args);

	zend_partial_debug_fill(partial, Z_ARRVAL(args));

	return ht;
}

static HashTable *zend_partial_get_gc(zend_object *obj, zval **table, int *n)
{
	zend_partial *partial = (zend_partial *)obj;

	if (!partial->argc) {
		*table = Z_TYPE(partial->This) == IS_OBJECT ? &partial->This : NULL;
		*n = Z_TYPE(partial->This) == IS_OBJECT ? 1 : 0;
    } else {
		zend_get_gc_buffer *buffer = zend_get_gc_buffer_create();

		if (Z_TYPE(partial->This) == IS_OBJECT) {
			zend_get_gc_buffer_add_zval(buffer, &partial->This);
		}

		for (uint32_t arg = 0; arg < partial->argc; arg++) {
			zend_get_gc_buffer_add_zval(buffer, &partial->argv[arg]);
		}

		zend_get_gc_buffer_use(buffer, table, n);
    }

	return NULL;
}

static zend_function *zend_partial_get_method(zend_object **object, zend_string *method, const zval *key) /* {{{ */
{
	if (zend_string_equals_literal_ci(method, ZEND_INVOKE_FUNC_NAME)) {
		zend_partial *partial = (zend_partial*) *object;

		return &partial->trampoline;
	}

	return zend_std_get_method(object, method, key);
}
/* }}} */

static int zend_partial_get_closure(zend_object *obj, zend_class_entry **ce_ptr, zend_function **fptr_ptr, zend_object **obj_ptr, bool check_only)
{
	zend_partial *partial = (zend_partial*) obj;

	*fptr_ptr = &partial->trampoline;
	*obj_ptr  = (zend_object*) &partial->std;

	return SUCCESS;
}

zend_function *zend_partial_get_trampoline(zend_object *obj) {
	zend_partial *partial = (zend_partial*) obj;

	return &partial->trampoline;
}

static void zend_partial_free(zend_object *object) {
	zend_partial *partial = (zend_partial*) object;

	zval *arg = partial->argv,
		 *end = arg + partial->argc;

	while (arg < end) {
		if (Z_OPT_REFCOUNTED_P(arg)) {
			zval_ptr_dtor(arg);
		}
		arg++;
	}

	efree(partial->argv);

	if (partial->named) {
		zend_array_release(partial->named);
	}

	zend_arg_info *info = partial->prototype.common.arg_info;

	if (ZEND_PARTIAL_FUNC_FLAG(&partial->func, ZEND_ACC_HAS_RETURN_TYPE)) {
		info--;
	}

	efree(info);

	if (partial->func.type == ZEND_USER_FUNCTION) {
		if (partial->prototype.op_array.filename) {
			zend_string_release(partial->prototype.op_array.filename);
		}
		destroy_zend_function(&partial->func);
	}

	if (Z_TYPE(partial->This) == IS_OBJECT) {
		zval_ptr_dtor(&partial->This);
	}

	zend_object_std_dtor(object);
}

void zend_partial_startup(void) {
	memcpy(&zend_partial_handlers,
			&std_object_handlers, sizeof(zend_object_handlers));

	zend_partial_handlers.free_obj = zend_partial_free;
	zend_partial_handlers.get_debug_info = zend_partial_debug;
	zend_partial_handlers.get_gc = zend_partial_get_gc;
	zend_partial_handlers.get_closure = zend_partial_get_closure;
	zend_partial_handlers.get_method = zend_partial_get_method;

	memset(&zend_call_magic_arginfo, 0, sizeof(zend_arg_info) * 1);

	zend_call_magic_arginfo[0].name = ZSTR_KNOWN(ZEND_STR_ARGS);
	zend_call_magic_arginfo[0].type = (zend_type) ZEND_TYPE_INIT_NONE(_ZEND_IS_VARIADIC_BIT);
}

static zend_always_inline zend_string* zend_partial_function_name(zend_function *function) {
	if (function->type == ZEND_INTERNAL_FUNCTION) {
		if (function->internal_function.handler == zend_pass_function.handler) {
			return zend_strpprintf(0, "__construct");
		}
	}
	return zend_string_copy(function->common.function_name);
}

static zend_always_inline zend_string* zend_partial_scope_name(zend_execute_data *execute_data, zend_function *function) {
	if (function->type == ZEND_INTERNAL_FUNCTION) {
		if (function->internal_function.handler == zend_pass_function.handler) {
			if (Z_OBJ(EX(This))) {
				return Z_OBJCE(EX(This))->name;
			}
		}
	}

	if (function->common.scope) {
		return function->common.scope->name;
	}

	return NULL;
}

static zend_always_inline zend_string* zend_partial_symbol_name(zend_execute_data *call, zend_function *function) {
	zend_string *name = zend_partial_function_name(function),
				*scope = zend_partial_scope_name(call, function),
				*symbol;
 
	if (scope) {
		symbol = zend_create_member_string(scope, name);
	} else {
		symbol = zend_string_copy(name);
	}

	zend_string_release(name);
	return symbol;
}

static zend_always_inline void zend_partial_prototype_underflow(zend_function *function, zend_string *symbol, uint32_t args, uint32_t expected, bool variadic) {
	char *limit = variadic ? "at least" : "exactly";

	if (function->type == ZEND_USER_FUNCTION) {
		zend_throw_error(NULL,
			"not enough arguments for application of %s, "
			"%d given and %s %d expected, declared in %s on line %d",
			ZSTR_VAL(symbol), args, limit, expected,
			ZSTR_VAL(function->op_array.filename), function->op_array.line_start);
	} else {
		zend_throw_error(NULL,
			"not enough arguments for application of %s, %d given and %s %d expected",
			ZSTR_VAL(symbol), args, limit, expected);
	}
}

static zend_always_inline void zend_partial_prototype_overflow(zend_function *function, zend_string *symbol, uint32_t args, uint32_t expected) {
	if (function->type == ZEND_USER_FUNCTION) {
		zend_throw_error(NULL,
			"too many arguments for application of %s, "
			"%d given and a maximum of %d expected, declared in %s on line %d",
			ZSTR_VAL(symbol), args, expected,
			ZSTR_VAL(function->op_array.filename), function->op_array.line_start);
	} else {
		zend_throw_error(NULL,
			"too many arguments for application of %s, %d given and a maximum of %d expected",
			ZSTR_VAL(symbol), args, expected);
	}
}

static zend_always_inline void zend_partial_args_underflow(zend_function *function, zend_string *symbol, uint32_t args, uint32_t expected, bool calling, bool prototype) {
	const char *what = calling ?
			"arguments" : "arguments or placeholders";
	const char *from = prototype ?
		   "application" : "implementation";
	const char *limit = function->common.num_args <= function->common.required_num_args ?
			"exactly" : "at least";

	if (function->type == ZEND_USER_FUNCTION) {
		zend_throw_error(NULL,
			"not enough %s for %s of %s, "
			"%d given and %s %d expected, declared in %s on line %d",
			what, from, ZSTR_VAL(symbol), args, limit, expected,
			ZSTR_VAL(function->op_array.filename), function->op_array.line_start);
	} else {
		zend_throw_error(NULL,
			"not enough %s for %s of %s, %d given and %s %d expected",
			what, from, ZSTR_VAL(symbol), args, limit, expected);
	}
}

static zend_always_inline void zend_partial_args_overflow(zend_function *function, zend_string *symbol, uint32_t args, uint32_t expected, bool calling, bool prototype) {
	const char *what = calling ?
			"arguments" : "arguments or placeholders";
	const char *from = prototype ?
		   "application" : "implementation";

	if (function->type == ZEND_USER_FUNCTION) {
		zend_throw_error(NULL,
			"too many %s for %s of %s, "
			"%d given and a maximum of %d expected, declared in %s on line %d",
			what, from, ZSTR_VAL(symbol), args, expected,
			ZSTR_VAL(function->op_array.filename), function->op_array.line_start);
	} else {
		zend_throw_error(NULL,
			"too many %s for %s of %s, %d given and a maximum of %d expected",
			what, from, ZSTR_VAL(symbol), args, expected);
	}
}

void zend_partial_args_check(zend_execute_data *call) {
	/* this is invoked by VM before the creation of zend_partial */
	zend_function *function = call->func;

	uint32_t num = ZEND_CALL_NUM_ARGS(call) +
	    (ZEND_PARTIAL_CALL_FLAG(call, ZEND_CALL_VARIADIC_PLACEHOLDER) ? -1 : 0);
	
	if (num < function->common.required_num_args) {
		/* this check is delayed in the case of variadic application */
		if (ZEND_PARTIAL_CALL_FLAG(call, ZEND_CALL_VARIADIC_PLACEHOLDER)) {
			return;
		}

		zend_string *symbol = zend_partial_symbol_name(call, function);
		zend_partial_args_underflow(
			function, symbol, 
			num, function->common.required_num_args, false, true);
		zend_string_release(symbol);
	} else if (num > function->common.num_args && 
			!ZEND_PARTIAL_FUNC_FLAG(call->func, ZEND_ACC_VARIADIC)) {
		zend_string *symbol = zend_partial_symbol_name(call, function);
		zend_partial_args_overflow(
			function, symbol,
			num, function->common.num_args, false, true);
		zend_string_release(symbol);
	}
}

static zend_always_inline uint32_t zend_partial_apply(
	zval *pStart, zval *pEnd,
	zval *cStart, zval *cEnd,
	zval *fParam, bool call) {

	uint32_t pCount = 0;

	/* this optimizes the most general case, partial variadic application followed by
		all arguments on call, and initial application */
	if ((pEnd - pStart) == 0 ||
		((pEnd - pStart) == 1 && Z_IS_PLACEHOLDER_VARIADIC_P(pStart))) {
		if ((pCount = (cEnd - cStart)) > 0) {
			memcpy(fParam, cStart, sizeof(zval) * pCount);
		}

		if (call) {
			cStart = fParam;
			cEnd   = cStart + pCount;
			while (cStart < cEnd) {
				if (Z_IS_PLACEHOLDER_P(cStart)) {
					ZVAL_UNDEF(cStart);
					pCount--;
				}
				cStart++;
			}
		}
		return pCount;
	}

	/* this optimizes the second most general case: all arguments supplied upon first
		application, followed by no arguments on call */
	if ((cEnd - cStart) == 0) {
		if ((pCount = (pEnd - pStart)) > 0) {
			memcpy(fParam, pStart, sizeof(zval) * pCount);
		}

		if (call) {
			cStart = fParam;
			cEnd   = cStart + pCount;
			while (cStart < cEnd) {
				if (Z_IS_PLACEHOLDER_P(cStart)) {
					ZVAL_UNDEF(cStart);
					pCount--;
				}
				cStart++;
			}
		}
		return pCount;
	}

	/* slow path is not able to handle all cases */
	while (pStart < pEnd) {
		if (Z_IS_PLACEHOLDER_P(pStart)) {
			if (cStart < cEnd) {
				ZVAL_COPY_VALUE(fParam, cStart);
				pCount++;
				fParam++;
				cStart++;
			}
		} else {
			if (cStart < cEnd && Z_ISUNDEF_P(pStart)) {
				ZVAL_COPY_VALUE(fParam, cStart);
				pCount++;
				fParam++;
				cStart++;
			} else {
				ZVAL_COPY_VALUE(fParam, pStart);
				pCount++;
				fParam++;
			}
		}
		pStart++;
	}

	while (cStart < cEnd) {
		ZVAL_COPY_VALUE(fParam, cStart);
		pCount++;
		fParam++;
		cStart++;
	}

	return pCount;
}

ZEND_NAMED_FUNCTION(zend_partial_call_magic)
{
	zend_partial *partial = (zend_partial*) Z_OBJ_P(ZEND_THIS);
	zend_object *object = NULL;
	zval *params;
	uint32_t num_params, num_all_params;
	HashTable *named_args;
	zend_fcall_info fci = empty_fcall_info;
	zend_fcall_info_cache fcc = empty_fcall_info_cache;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC_WITH_NAMED(params, num_params, named_args)
	ZEND_PARSE_PARAMETERS_END();

	num_all_params = num_params + (named_args ? named_args->nNumUsed : 0);

	if (num_all_params < partial->prototype.common.required_num_args) {
		zend_string *symbol = zend_partial_symbol_name(execute_data, &partial->prototype);
		zend_partial_prototype_underflow(
			&partial->prototype, symbol, num_all_params,
			partial->prototype.common.required_num_args, 
			ZEND_PARTIAL_CALL_FLAG(partial, ZEND_APPLY_VARIADIC));
		zend_string_release(symbol);
		if (ZEND_PARTIAL_IS_CALL_TRAMPOLINE(&partial->prototype)) {
			EG(trampoline).common.function_name = NULL;
		}
		RETURN_THROWS();
	} else if (num_all_params > partial->prototype.common.num_args && 
			  !ZEND_PARTIAL_CALL_FLAG(partial, ZEND_APPLY_VARIADIC)) {
		zend_string *symbol = zend_partial_symbol_name(execute_data, &partial->prototype);
		zend_partial_prototype_overflow(
			&partial->prototype, symbol, num_all_params,
			partial->prototype.common.num_args);
		zend_string_release(symbol);
		if (ZEND_PARTIAL_IS_CALL_TRAMPOLINE(&partial->prototype)) {
			EG(trampoline).common.function_name = NULL;
		}
		RETURN_THROWS();
	}

	fci.size = sizeof(zend_fcall_info);

	if (UNEXPECTED(ZEND_PARTIAL_CALL_FLAG(partial, ZEND_APPLY_FACTORY))) {
		zend_class_entry *type = Z_CE(partial->This);
		zval instance;

		if (type->create_object) {
			ZVAL_OBJ(&instance, type->create_object(type));
		} else {
			ZVAL_OBJ(&instance, zend_objects_new(type));

			object_properties_init(Z_OBJ(instance), type);
		}

		object = Z_OBJ(instance);

		if (ZEND_PARTIAL_CALL_FLAG(partial, ZEND_APPLY_PASS)) {
			/* nothing to pass any arguments too, so return immediately */
			RETURN_OBJ(object);
		}

		GC_ADD_FLAGS(object, IS_OBJ_DESTRUCTOR_CALLED);
	} else {
		if (Z_TYPE(partial->This) == IS_OBJECT) {
			object = Z_OBJ(partial->This);
		} else if (Z_TYPE(partial->This) == IS_UNDEF && Z_CE(partial->This)) {
			fcc.called_scope = Z_CE(partial->This);
		}
	}

	fci.retval = return_value;
	fcc.function_handler = &partial->func;

	if (object) {
		fci.object = object;
		fcc.object = object;
	}

	if (partial->named) {
		if (!named_args) {
			named_args = partial->named;
			GC_ADDREF(named_args);
		} else {
			HashTable *nested = zend_array_dup(partial->named);
			zend_hash_merge(nested, named_args, zval_copy_ctor, true);
			named_args = nested;
		}
		fci.named_params = named_args;
	} else {
		fci.named_params = named_args;
	}

	fci.params = ecalloc(sizeof(zval), partial->argc + num_params);
	fci.param_count = zend_partial_apply(
		partial->argv, partial->argv + partial->argc,
		params ? params : NULL,
		params ? params + num_params : NULL, fci.params, true);

	if (fci.param_count < partial->func.common.required_num_args) {
		/* doesn't satisfy implementation */
		zend_string *symbol = zend_partial_symbol_name(execute_data, &partial->func);
		zend_partial_args_underflow(
			&partial->func, symbol, fci.param_count,
			partial->func.common.required_num_args, true, false);
		zend_string_release(symbol);
	} else {
		zend_call_function(&fci, &fcc);
	}

	if (ZEND_PARTIAL_IS_CALL_TRAMPOLINE(&partial->prototype)) {
		EG(trampoline).common.function_name = NULL;
	}

	if (ZEND_PARTIAL_CALL_FLAG(partial, ZEND_APPLY_FACTORY)) {
		if (!EG(exception)) {
			GC_DEL_FLAGS(object, IS_OBJ_DESTRUCTOR_CALLED);
		}
		zval_ptr_dtor(return_value);
		RETVAL_OBJ(object);
	}

	if (partial->named) {
		zend_array_release(named_args);
	}
	efree(fci.params);
}

void zend_partial_create(zval *result, uint32_t info, zval *this_ptr, zend_function *function, uint32_t argc, zval *argv, zend_array *extra_named_params) {
	zend_function *prototype = NULL;

	ZVAL_OBJ(result, zend_partial_new(zend_ce_closure, info));

	zend_partial *applied, *partial = (zend_partial*) Z_OBJ_P(result);

	if ((applied = zend_partial_fetch(this_ptr))) {
		ZEND_ADD_CALL_FLAG(partial, ZEND_CALL_INFO(applied) & ~ZEND_APPLY_VARIADIC);

		function  = &applied->func;
		prototype = &applied->prototype;

		if (ZEND_PARTIAL_CALL_FLAG(partial, ZEND_APPLY_FACTORY)) {
			Z_CE(partial->This) = Z_CE(applied->This);
		}

		partial->argv = ecalloc(applied->argc + argc, sizeof(zval));
		partial->argc = zend_partial_apply(
			applied->argv, applied->argv + applied->argc,
			argv, argv + argc,
			partial->argv, false);

		if (extra_named_params) {
			if (applied->named) {
				partial->named = zend_array_dup(applied->named);

				zend_hash_merge(partial->named, extra_named_params, zval_copy_ctor, true);
			} else {
				partial->named = extra_named_params;
				GC_ADDREF(extra_named_params);
			}
		}
	} else {
		partial->argv = ecalloc(argc, sizeof(zval));
		partial->argc = zend_partial_apply(
			NULL, NULL,
			argv, argv + argc,
			partial->argv, false);

		if (extra_named_params) {
			partial->named = extra_named_params;
			GC_ADDREF(extra_named_params);
		}
	}

	memcpy(&partial->func, function, ZEND_PARTIAL_FUNC_SIZE(function));

	ZEND_PARTIAL_FUNC_DEL(&partial->func, ZEND_ACC_CLOSURE);
	ZEND_PARTIAL_FUNC_ADD(&partial->func, ZEND_ACC_IMMUTABLE);

	if (ZEND_PARTIAL_FUNC_FLAG(&partial->func, ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
		ZEND_PARTIAL_FUNC_ADD(&partial->func, ZEND_ACC_TRAMPOLINE_PERMANENT);
	}

	if (ZEND_PARTIAL_CALL_FLAG(partial, ZEND_APPLY_FACTORY)) {
		if (!Z_CE(partial->This)) {
			Z_CE(partial->This) = Z_OBJCE_P(this_ptr);
		}

		if (ZEND_PARTIAL_CALL_FLAG(partial, ZEND_APPLY_PASS)) {
			/* setting scope for pass function so that errors make sense */
			partial->func.common.scope = Z_CE(partial->This);
		}
	}

	ZEND_PARTIAL_FUNC_ADD(&partial->func, ZEND_ACC_FAKE_CLOSURE);

	if (partial->func.type == ZEND_USER_FUNCTION) {
		zend_string_addref(partial->func.common.function_name);

		partial->func.op_array.refcount = NULL;
	}

	if (!prototype) {
		prototype = &partial->func;
	}

	partial->func.common.prototype = zend_partial_signature_create(partial, prototype);

	if (!ZEND_PARTIAL_CALL_FLAG(partial, ZEND_APPLY_FACTORY)) {
		/* partial info may contain ZEND_APPLY_VARIADIC */
		uint32_t backup_info = ZEND_CALL_INFO(partial);

		if(Z_TYPE_P(this_ptr) == IS_UNDEF && Z_CE_P(this_ptr)) {
			ZVAL_COPY_VALUE(&partial->This, this_ptr);
		} else if (Z_TYPE_P(this_ptr) == IS_OBJECT) {
			zval *This;
			if (instanceof_function(Z_OBJCE_P(this_ptr), zend_ce_closure)) {
				if (zend_string_equals_ci(
						partial->func.common.function_name,
						ZSTR_KNOWN(ZEND_STR_MAGIC_INVOKE))) {
					This = this_ptr;
				} else {
					This = zend_get_closure_this_ptr(this_ptr);
				}
			} else {
				This = this_ptr;
			}
			ZVAL_COPY(&partial->This, This);
		}

		ZEND_ADD_CALL_FLAG(partial, backup_info);
	}

	zend_partial_trampoline_create(partial, &partial->trampoline);
}

void zend_partial_bind(zval *result, zval *partial, zval *this_ptr, zend_class_entry *scope) {
	zval This;
	zend_partial *object = (zend_partial*) Z_OBJ_P(partial);

	ZVAL_UNDEF(&This);

	if (!this_ptr || Z_TYPE_P(this_ptr) != IS_OBJECT) {
		ZEND_ASSERT(scope && "scope must be set");

		Z_CE(This) = scope;
	} else {
		ZVAL_COPY_VALUE(&This, this_ptr);
	}

	zend_partial_create(result, ZEND_CALL_INFO(object), &This,
		&object->func, object->argc, object->argv, object->named);

	zval *argv = object->argv,
		 *end = argv + object->argc;

	while (argv < end) {
		Z_TRY_ADDREF_P(argv);
		argv++;
	}
}
