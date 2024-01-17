/*
   +----------------------------------------------------------------------+
   | Zend Engine, Func Info                                               |
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
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   |          Xinchen Hui <laruence@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend_compile.h"
#include "zend_extensions.h"
#include "zend_ssa.h"
#include "zend_optimizer_internal.h"
#include "zend_inference.h"
#include "zend_call_graph.h"
#include "zend_func_info.h"
#include "zend_inference.h"
#ifdef _WIN32
#include "win32/ioutil.h"
#endif

typedef uint32_t (*info_func_t)(const zend_call_info *call_info, const zend_ssa *ssa);

typedef struct _func_info_t {
	const char *name;
	unsigned    name_len;
	uint32_t    info;
	info_func_t info_func;
} func_info_t;

#define F0(name, info) \
	{name, sizeof(name)-1, (info), NULL}
#define F1(name, info) \
	{name, sizeof(name)-1, (MAY_BE_RC1 | (info)), NULL}
#define FN(name, info) \
	{name, sizeof(name)-1, (MAY_BE_RC1 | MAY_BE_RCN | (info)), NULL}
#define FC(name, callback) \
	{name, sizeof(name)-1, 0, callback}

#include "zend_func_infos.h"

static uint32_t zend_range_info(const zend_call_info *call_info, const zend_ssa *ssa)
{
	if (!call_info->send_unpack
	 && (call_info->num_args == 2 || call_info->num_args == 3)
	 && ssa
	 && !(ssa->cfg.flags & ZEND_SSA_TSSA)) {
		zend_op_array *op_array = call_info->caller_op_array;
		uint32_t t1 = _ssa_op1_info(op_array, ssa, call_info->arg_info[0].opline,
			&ssa->ops[call_info->arg_info[0].opline - op_array->opcodes]);
		uint32_t t2 = _ssa_op1_info(op_array, ssa, call_info->arg_info[1].opline,
			&ssa->ops[call_info->arg_info[1].opline - op_array->opcodes]);
		uint32_t t3 = 0;
		uint32_t tmp = MAY_BE_RC1 | MAY_BE_ARRAY;

		if (call_info->num_args == 3) {
			t3 = _ssa_op1_info(op_array, ssa, call_info->arg_info[2].opline,
				&ssa->ops[call_info->arg_info[2].opline - op_array->opcodes]);
		}
		if ((t1 & MAY_BE_STRING) && (t2 & MAY_BE_STRING)) {
			tmp |= MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING;
		}
		if ((t1 & (MAY_BE_DOUBLE|MAY_BE_STRING))
				|| (t2 & (MAY_BE_DOUBLE|MAY_BE_STRING))
				|| (t3 & (MAY_BE_DOUBLE|MAY_BE_STRING))) {
			tmp |= MAY_BE_ARRAY_OF_DOUBLE;
		}
		if ((t1 & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_DOUBLE))
				&& (t2 & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_DOUBLE))) {
			tmp |= MAY_BE_ARRAY_OF_LONG;
		}
		if (tmp & MAY_BE_ARRAY_OF_ANY) {
			tmp |= MAY_BE_ARRAY_PACKED;
		}
		return tmp;
	} else {
		/* May throw */
		return MAY_BE_RC1 | MAY_BE_ARRAY | MAY_BE_ARRAY_EMPTY | MAY_BE_ARRAY_PACKED | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING;
	}
}

static const func_info_t old_func_infos[] = {
	FC("range",                        zend_range_info),
};

static HashTable func_info;
ZEND_API int zend_func_info_rid = -1;

uint32_t zend_get_internal_func_info(
		const zend_function *callee_func, const zend_call_info *call_info, const zend_ssa *ssa) {
	if (callee_func->common.scope) {
		/* This is a method, not a function. */
		return 0;
	}

	zend_string *name = callee_func->common.function_name;
	if (!name) {
		/* zend_pass_function has no name. */
		return 0;
	}

	zval *zv = zend_hash_find_known_hash(&func_info, name);
	if (!zv) {
		return 0;
	}

	func_info_t *info = Z_PTR_P(zv);
	if (info->info_func) {
		return call_info ? info->info_func(call_info, ssa) : 0;
	} else {
		uint32_t ret = info->info;

		if (ret & MAY_BE_ARRAY) {
			ret |= MAY_BE_ARRAY_EMPTY;
		}
		return ret;
	}
}

ZEND_API uint32_t zend_get_func_info(
		const zend_call_info *call_info, const zend_ssa *ssa,
		zend_class_entry **ce, bool *ce_is_instanceof)
{
	uint32_t ret = 0;
	const zend_function *callee_func = call_info->callee_func;
	*ce = NULL;
	*ce_is_instanceof = 0;

	if (callee_func->type == ZEND_INTERNAL_FUNCTION) {
		uint32_t internal_ret = zend_get_internal_func_info(callee_func, call_info, ssa);
#if !ZEND_DEBUG
		if (internal_ret) {
			return internal_ret;
		}
#endif

		ret = zend_get_return_info_from_signature_only(
			callee_func, /* script */ NULL, ce, ce_is_instanceof, /* use_tentative_return_info */ !call_info->is_prototype);

#if ZEND_DEBUG
		if (internal_ret) {
			zend_string *name = callee_func->common.function_name;
			/* Check whether the func_info information is a subset of the information we can
			 * compute from the specified return type, otherwise it contains redundant types. */
			if (internal_ret & ~ret) {
				fprintf(stderr, "Inaccurate func info for %s()\n", ZSTR_VAL(name));
			}
			/* Check whether the func info is completely redundant with arginfo. */
			if (internal_ret == ret) {
				fprintf(stderr, "Useless func info for %s()\n", ZSTR_VAL(name));
			}
			/* If the return type is not mixed, check that the types match exactly if we exclude
			 * RC and array information. */
			uint32_t ret_any = ret & MAY_BE_ANY, internal_ret_any = internal_ret & MAY_BE_ANY;
			if (ret_any != MAY_BE_ANY) {
				uint32_t diff = internal_ret_any ^ ret_any;
				/* Func info may contain "true" types as well as isolated "null" and "false". */
				if (diff && !(diff == MAY_BE_FALSE && (ret & MAY_BE_FALSE))
						&& (internal_ret_any & ~(MAY_BE_NULL|MAY_BE_FALSE))) {
					fprintf(stderr, "Incorrect func info for %s()\n", ZSTR_VAL(name));
				}
			}
			return internal_ret;
		}
#endif
	} else {
		if (!call_info->is_prototype) {
			// FIXME: the order of functions matters!!!
			zend_func_info *info = ZEND_FUNC_INFO((zend_op_array*)callee_func);
			if (info) {
				ret = info->return_info.type;
				*ce = info->return_info.ce;
				*ce_is_instanceof = info->return_info.is_instanceof;
			}
		}
		if (!ret) {
			ret = zend_get_return_info_from_signature_only(
				callee_func, /* TODO: script */ NULL, ce, ce_is_instanceof, /* use_tentative_return_info */ !call_info->is_prototype);
			/* It's allowed to override a method that return non-reference with a method that returns a reference */
			if (call_info->is_prototype && (ret & ~MAY_BE_REF)) {
				ret |= MAY_BE_REF;
				*ce = NULL;
			}
		}
	}
	return ret;
}

static void zend_func_info_add(const func_info_t *func_infos, size_t n)
{
	for (size_t i = 0; i < n; i++) {
		zend_string *key = zend_string_init_interned(func_infos[i].name, func_infos[i].name_len, 1);

		if (zend_hash_add_ptr(&func_info, key, (void**)&func_infos[i]) == NULL) {
			fprintf(stderr, "ERROR: Duplicate function info for \"%s\"\n", func_infos[i].name);
		}

		zend_string_release_ex(key, 1);
	}
}

zend_result zend_func_info_startup(void)
{
	if (zend_func_info_rid == -1) {
		zend_func_info_rid = zend_get_resource_handle("Zend Optimizer");
		if (zend_func_info_rid < 0) {
			return FAILURE;
		}

		zend_hash_init(&func_info, sizeof(old_func_infos)/sizeof(func_info_t) + sizeof(func_infos)/sizeof(func_info_t), NULL, NULL, 1);

		zend_func_info_add(old_func_infos, sizeof(old_func_infos)/sizeof(func_info_t));
		zend_func_info_add(func_infos, sizeof(func_infos)/sizeof(func_info_t));
	}

	return SUCCESS;
}

zend_result zend_func_info_shutdown(void)
{
	if (zend_func_info_rid != -1) {
		zend_hash_destroy(&func_info);
		zend_func_info_rid = -1;
	}
	return SUCCESS;
}
