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
*/

/**
 * Partial Function Application:
 *
 * A partial application is compiled to the usual sequence of function call
 * opcodes (INIT_FCALL, SEND_VAR, etc), but the sequence ends with a
 * CALLABLE_CONVERT_PARTIAL opcode instead of DO_FCALL, similarly to
 * first class callables. Placeholders are compiled to SEND_PLACEHOLDER opcodes:
 *
 * $f = f($a, ?)
 *
 * 0001 INIT_FCALL f
 * 0002 SEND_VAR CV($a)
 * 0003 SEND_PLACEHOLDER
 * 0004 CV($f) = CALLABLE_CONVERT_PARTIAL
 *
 * SEND_PLACEHOLDER sets the argument slot type to _IS_PLACEHOLDER.
 *
 * CALLABLE_CONVERT_PARTIAL uses the information available on the stack to
 * create a Closure and return it, consuming the stack frame in the process
 * like an internal function call.
 *
 * We create the Closure by generating the relevant AST and compiling it to an
 * op_array. The op_array is cached in the Opcache SHM and inline caches.
 *
 * This file implements the Closure generation logic
 * (see zend_partial_create(), zp_compile()).
 */

#include "zend.h"
#include "zend_API.h"
#include "zend_arena.h"
#include "zend_ast.h"
#include "zend_compile.h"
#include "zend_closures.h"
#include "zend_attributes.h"
#include "zend_exceptions.h"
#include "ext/opcache/ZendAccelerator.h"

#define Z_IS_PLACEHOLDER_P(p) (Z_TYPE_P(p) == _IS_PLACEHOLDER)

static zend_always_inline bool zp_is_static_closure(const zend_function *function) {
	return ((function->common.fn_flags & (ZEND_ACC_STATIC|ZEND_ACC_CLOSURE)) == (ZEND_ACC_STATIC|ZEND_ACC_CLOSURE));
}

static zend_never_inline ZEND_COLD void zp_args_underflow(
		const zend_function *function, uint32_t args, uint32_t expected)
{
	zend_string *symbol = get_function_or_method_name(function);
	const char *limit = function->common.num_args <= function->common.required_num_args ?
			"exactly" : "at least";

	zend_argument_count_error(
		"Partial application of %s() expects %s %d arguments, %d given",
		ZSTR_VAL(symbol), limit, expected, args);

	zend_string_release(symbol);
}

static zend_never_inline ZEND_COLD void zp_args_overflow(
		const zend_function *function, uint32_t args, uint32_t expected)
{
	zend_string *symbol = get_function_or_method_name(function);

	zend_argument_count_error(
		"Partial application of %s() expects at most %d arguments, %d given",
		ZSTR_VAL(symbol), expected, args);

	zend_string_release(symbol);
}

static zend_result zp_args_check(const zend_function *function,
		uint32_t argc, const zval *argv,
		const zend_array *extra_named_args,
		bool uses_variadic_placeholder) {

	if (extra_named_args) {
		zval *arg;
		zend_string *key;
		ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(extra_named_args, key, arg) {
			if (UNEXPECTED(Z_IS_PLACEHOLDER_P(arg))) {
				zend_throw_error(NULL,
						"Cannot use named placeholder for unknown or variadic parameter $%s",
						ZSTR_VAL(key));
				return FAILURE;
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (argc < function->common.required_num_args) {
		if (uses_variadic_placeholder) {
			/* Missing args will be turned into placeholders */
			return SUCCESS;
		}

		zp_args_underflow(
			function, argc, function->common.required_num_args);
		return FAILURE;
	} else if (argc > function->common.num_args &&
			!(function->common.fn_flags & ZEND_ACC_VARIADIC)) {
		zp_args_overflow(function, argc, function->common.num_args);
		return FAILURE;
	}

	return SUCCESS;
}

static bool zp_name_exists(zend_string **names, uint32_t num_names, const zend_string *name)
{
	for (uint32_t i = 0; i < num_names; i++) {
		if (names[i] && zend_string_equals(names[i], name)) {
			return true;
		}
	}
	return false;
}

static zend_string *zp_get_param_name(zend_function *function, uint32_t arg_offset)
{
	return zend_string_copy(function->common.arg_info[arg_offset].name);
}

/* Assign a name for every variable that will be used in the generated closure,
 * including params and used vars. */
static void zp_assign_names(zend_string **names, uint32_t num_names,
		uint32_t argc, zval *argv,
		zend_function *function, bool variadic_partial,
		zend_array *extra_named_params)
{
	/* Assign names for params. We never rename those. */
	for (uint32_t offset = 0; offset < MIN(argc, function->common.num_args); offset++) {
		if (Z_IS_PLACEHOLDER_P(&argv[offset])) {
			names[offset] = zp_get_param_name(function, offset);
		}
	}

	/* Assign name for the variadic param. Never renamed. */
	if (variadic_partial && (function->common.fn_flags & ZEND_ACC_VARIADIC)) {
		names[argc] = zp_get_param_name(function, function->common.num_args);
	}

	/* Assign names for placeholders that bind to the variadic param:
	 *
	 * function f($a, ...$args) {}
	 * f(?, ?, ...); // The second placeholder binds into the variadic param.
	 *
	 * By default these are named $origNameN with N the offset from the
	 * variadic param. In case of clash we increment N until a free name is
	 * found. */
	for (uint32_t offset = function->common.num_args; offset < argc; offset++) {
		ZEND_ASSERT(function->common.fn_flags & ZEND_ACC_VARIADIC);
		if (!Z_IS_PLACEHOLDER_P(&argv[offset])) {
			continue;
		}
		uint32_t n = offset - function->common.num_args;
		zend_string *orig_name = zp_get_param_name(function, function->common.num_args);
		zend_string *new_name;
		do {
			new_name = zend_strpprintf_unchecked(0, "%S%" PRIu32, orig_name, n);
			if (!zp_name_exists(names, num_names, new_name)) {
				break;
			}
			n++;
			zend_string_release(new_name);
		} while (true);
		names[offset] = new_name;
		zend_string_release(orig_name);
	}

	/* Assign names for pre-bound params (lexical vars).
	 * There may be clashes, we ensure to generate unique names. */
	for (uint32_t offset = 0; offset < argc; offset++) {
		if (Z_IS_PLACEHOLDER_P(&argv[offset]) || Z_ISUNDEF(argv[offset])) {
			continue;
		}
		uint32_t n = 0;
		zend_string *orig_name = zp_get_param_name(function, MIN(offset, function->common.num_args));
		zend_string *new_name = zend_string_copy(orig_name);
		while (zp_name_exists(names, num_names, new_name)) {
			zend_string_release(new_name);
			new_name = zend_strpprintf_unchecked(0, "%S%" PRIu32, orig_name, n);
			n++;
		}
		names[offset] = new_name;
		zend_string_release(orig_name);
	}

	/* Assign name for $extra_named_params */
	if (extra_named_params) {
		uint32_t n = 1;
		zend_string *new_name = ZSTR_INIT_LITERAL("extra_named_params", 0);
		while (zp_name_exists(names, num_names, new_name)) {
			zend_string_release(new_name);
			n++;
			new_name = zend_strpprintf(0, "%s%" PRIu32, "extra_named_params", n);
		}
		names[argc + variadic_partial] = new_name;
	}

	/* Assign name for $fn */
	if (function->common.fn_flags & ZEND_ACC_CLOSURE) {
		uint32_t n = 1;
		zend_string *new_name = ZSTR_INIT_LITERAL("fn", 0);
		while (zp_name_exists(names, num_names, new_name)) {
			zend_string_release(new_name);
			n++;
			new_name = zend_strpprintf(0, "%s%" PRIu32, "fn", n);
		}
		names[argc + variadic_partial + (extra_named_params != NULL)] = new_name;
	}
}

static inline bool zp_is_power_of_two(uint32_t x)
{
	return (x > 0) && !(x & (x - 1));
}

static bool zp_is_simple_type(uint32_t type_mask)
{
	ZEND_ASSERT(!(type_mask & ~_ZEND_TYPE_MAY_BE_MASK));

	return zp_is_power_of_two(type_mask)
		|| type_mask == MAY_BE_BOOL
		|| type_mask == MAY_BE_ANY;
}

static zend_ast *zp_simple_type_to_ast(uint32_t type)
{
	zend_string *name;

	switch (type) {
		case MAY_BE_NULL:
			name = ZSTR_KNOWN(ZEND_STR_NULL_LOWERCASE);
			break;
		case MAY_BE_TRUE:
			name = ZSTR_KNOWN(ZEND_STR_TRUE);
			break;
		case MAY_BE_FALSE:
			name = ZSTR_KNOWN(ZEND_STR_FALSE);
			break;
		case MAY_BE_LONG:
			name = ZSTR_KNOWN(ZEND_STR_INT);
			break;
		case MAY_BE_DOUBLE:
			name = ZSTR_KNOWN(ZEND_STR_FLOAT);
			break;
		case MAY_BE_STRING:
			name = ZSTR_KNOWN(ZEND_STR_STRING);
			break;
		case MAY_BE_BOOL:
			name = ZSTR_KNOWN(ZEND_STR_BOOL);
			break;
		case MAY_BE_VOID:
			name = ZSTR_KNOWN(ZEND_STR_VOID);
			break;
		case MAY_BE_NEVER:
			name = ZSTR_KNOWN(ZEND_STR_NEVER);
			break;
		case MAY_BE_OBJECT:
			name = ZSTR_KNOWN(ZEND_STR_OBJECT);
			break;
		case MAY_BE_ANY:
			name = ZSTR_KNOWN(ZEND_STR_MIXED);
			break;
		case MAY_BE_CALLABLE:
			return zend_ast_create_ex(ZEND_AST_TYPE, IS_CALLABLE);
		case MAY_BE_ARRAY:
			return zend_ast_create_ex(ZEND_AST_TYPE, IS_ARRAY);
		case MAY_BE_STATIC:
			return zend_ast_create_ex(ZEND_AST_TYPE, IS_STATIC);
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	zend_ast *ast = zend_ast_create_zval_from_str(name);
	ast->attr = ZEND_NAME_NOT_FQ;

	return ast;
}

static zend_ast *zp_type_name_to_ast(zend_string *name)
{
	zend_ast *ast = zend_ast_create_zval_from_str(name);

	if (zend_get_class_fetch_type(name) != ZEND_FETCH_CLASS_DEFAULT) {
		ast->attr = ZEND_NAME_NOT_FQ;
	} else {
		ast->attr = ZEND_NAME_FQ;
	}

	return ast;
}

static zend_ast *zp_type_to_ast(const zend_type type)
{
	if (!ZEND_TYPE_IS_SET(type)) {
		return NULL;
	}

	if (ZEND_TYPE_IS_UNION(type)
			|| (ZEND_TYPE_IS_COMPLEX(type) && ZEND_TYPE_PURE_MASK(type))
			|| (ZEND_TYPE_PURE_MASK(type) && !zp_is_simple_type(ZEND_TYPE_PURE_MASK(type)))) {
		zend_ast *type_ast = zend_ast_create_list(0, ZEND_AST_TYPE_UNION);
		if (ZEND_TYPE_HAS_LIST(type)) {
			const zend_type *type_ptr;
			ZEND_TYPE_LIST_FOREACH(ZEND_TYPE_LIST(type), type_ptr) {
				type_ast = zend_ast_list_add(type_ast, zp_type_to_ast(*type_ptr));
			} ZEND_TYPE_LIST_FOREACH_END();
		} else if (ZEND_TYPE_HAS_NAME(type)) {
			zend_ast *name_ast = zp_type_name_to_ast(
					zend_string_copy(ZEND_TYPE_NAME(type)));
			type_ast = zend_ast_list_add(type_ast, name_ast);
		} else {
			ZEND_ASSERT(!ZEND_TYPE_IS_COMPLEX(type));
		}
		uint32_t type_mask = ZEND_TYPE_PURE_MASK(type);
		if ((type_mask & MAY_BE_BOOL) == MAY_BE_BOOL) {
			type_ast = zend_ast_list_add(type_ast, zp_simple_type_to_ast(MAY_BE_BOOL));
			type_mask &= ~MAY_BE_BOOL;
		}
		for (uint32_t may_be_type = 1; may_be_type < _ZEND_TYPE_MAY_BE_MASK; may_be_type <<= 1) {
			if (type_mask & may_be_type) {
				type_ast = zend_ast_list_add(type_ast, zp_simple_type_to_ast(may_be_type));
			}
		}
		return type_ast;
	}

	if (ZEND_TYPE_IS_INTERSECTION(type)) {
		ZEND_ASSERT(!ZEND_TYPE_PURE_MASK(type));
		zend_ast *type_ast = zend_ast_create_list(0, ZEND_AST_TYPE_INTERSECTION);
		const zend_type *type_ptr;
		ZEND_TYPE_LIST_FOREACH(ZEND_TYPE_LIST(type), type_ptr) {
			type_ast = zend_ast_list_add(type_ast, zp_type_to_ast(*type_ptr));
		} ZEND_TYPE_LIST_FOREACH_END();
		return type_ast;
	}

	if (ZEND_TYPE_HAS_NAME(type)) {
		ZEND_ASSERT(!ZEND_TYPE_PURE_MASK(type));
		zend_ast *type_ast = zp_type_name_to_ast(
				zend_string_copy(ZEND_TYPE_NAME(type)));
		return type_ast;
	}

	ZEND_ASSERT(!ZEND_TYPE_IS_COMPLEX(type));

	uint32_t type_mask = ZEND_TYPE_PURE_MASK(type);
	ZEND_ASSERT(zp_is_simple_type(type_mask));

	return zp_simple_type_to_ast(type_mask);
}

/* Can not use zend_argument_error() as the function is not on the stack */
static zend_never_inline ZEND_COLD void zp_argument_error(zend_class_entry *error_ce,
		zend_function *function, uint32_t arg_num, const char *format, ...)
{
	zend_string *func_name = get_function_or_method_name(function);
	const char *arg_name = get_function_arg_name(function, arg_num);

	char *message = NULL;

	va_list va;
	va_start(va, format);
	zend_vspprintf(&message, 0, format, va);
	va_end(va);

	zend_throw_error(error_ce, "%s(): Argument #%d%s%s%s %s",
		ZSTR_VAL(func_name), arg_num,
		arg_name ? " ($" : "", arg_name ? arg_name : "", arg_name ? ")" : "", message
	);
	efree(message);
	zend_string_release(func_name);
}

static zend_result zp_get_param_default_value(zval *result, zend_function *function, uint32_t arg_offset)
{
	ZEND_ASSERT(arg_offset < function->common.num_args);

	if (function->type == ZEND_USER_FUNCTION) {
		zend_op *opline = &function->op_array.opcodes[arg_offset];
		if (EXPECTED(opline->opcode == ZEND_RECV_INIT)) {
			ZVAL_COPY(result, RT_CONSTANT(opline, opline->op2));
			return SUCCESS;
		} else {
			ZEND_ASSERT(opline->opcode == ZEND_RECV);
		}
	} else {
		if (function->common.fn_flags & ZEND_ACC_USER_ARG_INFO) {
			goto error;
		}

		const zend_arg_info *arg_info = &function->internal_function.arg_info[arg_offset];

		if (zend_get_default_from_internal_arg_info(result, arg_info) == SUCCESS) {
			return SUCCESS;
		}
	}

error:
	zp_argument_error(zend_ce_argument_count_error, function, arg_offset + 1,
			"must be passed explicitly, because the default value is not known");

	return FAILURE;
}

static bool zp_arg_must_be_sent_by_ref(zend_function *function, uint32_t arg_num)
{
	if (EXPECTED(arg_num <= MAX_ARG_FLAG_NUM)) {
		if (QUICK_ARG_MUST_BE_SENT_BY_REF(function, arg_num)) {
			return true;
		}
	} else if (ARG_MUST_BE_SENT_BY_REF(function, arg_num)) {
		return true;
	}
	return false;
}

static zend_ast *zp_attribute_to_ast(zend_attribute *attribute)
{
	zend_ast *args_ast;
	if (attribute->argc) {
		args_ast = zend_ast_create_arg_list(0, ZEND_AST_ARG_LIST);
		for (uint32_t i = 0; i < attribute->argc; i++) {
			zend_ast *arg_ast = zend_ast_create_zval(&attribute->args[i].value);
			if (attribute->args[i].name) {
				arg_ast = zend_ast_create(ZEND_AST_NAMED_ARG,
						zend_ast_create_zval_from_str(
							zend_string_copy(attribute->args[i].name)),
						arg_ast);
			}
			args_ast = zend_ast_list_add(args_ast, arg_ast);
		}
	} else {
		args_ast = NULL;
	}
	return zend_ast_create(ZEND_AST_ATTRIBUTE,
			zend_ast_create_zval_from_str(zend_string_copy(attribute->name)),
			args_ast);
}

static zend_ast *zp_param_attributes_to_ast(zend_function *function,
		uint32_t offset)
{
	zend_ast *attributes_ast = NULL;
	if (!function->common.attributes) {
		return NULL;
	}

	/* Inherit the SensitiveParameter attribute */
	zend_attribute *attr = zend_get_parameter_attribute_str(
			function->common.attributes,
			"sensitiveparameter", strlen("sensitiveparameter"), offset);
	if (attr) {
		attributes_ast = zend_ast_create_list(1, ZEND_AST_ATTRIBUTE_GROUP,
				zp_attribute_to_ast(attr));
		attributes_ast = zend_ast_create_list(1, ZEND_AST_ATTRIBUTE_LIST,
				attributes_ast);
	}

	return attributes_ast;
}

static zend_string *zp_pfa_name(const zend_op_array *declaring_op_array,
		const zend_op *declaring_opline)
{
	zend_string *filename = declaring_op_array->filename;
	uint32_t start_lineno = declaring_opline->lineno;

	zend_string *class = zend_empty_string;
	zend_string *separator = zend_empty_string;
	zend_string *function = filename;
	const char *parens = "";

	if (declaring_op_array->function_name) {
		if (declaring_op_array->fn_flags & ZEND_ACC_CLOSURE) {
			/* If the parent function is a closure, don't redundantly
			 * add the classname and parentheses.
			 */
			function = declaring_op_array->function_name;
		} else {
			function = declaring_op_array->function_name;
			parens = "()";

			if (declaring_op_array->scope && declaring_op_array->scope->name) {
				class = declaring_op_array->scope->name;
				separator = ZSTR_KNOWN(ZEND_STR_PAAMAYIM_NEKUDOTAYIM);
			}
		}
	}

	zend_string *name = zend_strpprintf_unchecked(
		0,
		"{closure:pfa:%S%S%S%s:%" PRIu32 "}",
		class,
		separator,
		function,
		parens,
		start_lineno
	);

	return name;
}

/* Generate the AST for calling the actual function */
static zend_ast *zp_compile_forwarding_call(
	zval *this_ptr, zend_function *function,
	uint32_t argc, zval *argv, zend_array *extra_named_params,
	zend_string **param_names, bool variadic_partial, uint32_t num_args,
	zend_class_entry *called_scope, zend_type return_type,
	bool forward_superfluous_args,
	zend_ast *stmts_ast)
{
	bool is_assert = zend_string_equals(function->common.function_name,
			ZSTR_KNOWN(ZEND_STR_ASSERT));

	zend_ast *args_ast = zend_ast_create_list(0, ZEND_AST_ARG_LIST);
	zend_ast *call_ast = NULL;

	if (is_assert) {
		/* We are going to call assert() dynamically (via call_user_func),
		 * otherwise assert() would print the generated AST on failure, which is
		 * irrelevant. */
		args_ast = zend_ast_list_add(args_ast,
				zend_ast_create_zval_from_str(ZSTR_KNOWN(ZEND_STR_ASSERT)));
	}

	for (uint32_t offset = 0; offset < argc; offset++) {
		if (Z_ISUNDEF(argv[offset])) {
			/* Argument was not passed. Pass its default value. */
			if (offset < function->common.required_num_args) {
				/* Required param was not passed. This can happen due to named
				 * args. Using the same exception CE and message as
				 * zend_handle_undef_args(). */
				zp_argument_error(zend_ce_argument_count_error, function,
						offset + 1, "not passed");
				goto error;
			}
			zval default_value;
			if (zp_get_param_default_value(&default_value, function, offset) == FAILURE) {
				ZEND_ASSERT(EG(exception));
				goto error;
			}
			zend_ast *default_value_ast;
			if (Z_TYPE(default_value) == IS_CONSTANT_AST) {
				/* Must dup AST because we are doing to destroy it */
				default_value_ast = zend_ast_dup(Z_ASTVAL(default_value));
			} else {
				default_value_ast = zend_ast_create_zval(&default_value);
			}
			args_ast = zend_ast_list_add(args_ast, default_value_ast);
		} else {
			args_ast = zend_ast_list_add(args_ast, zend_ast_create(ZEND_AST_VAR,
						zend_ast_create_zval_from_str(zend_string_copy(param_names[offset]))));
		}
	}
	if (extra_named_params) {
		args_ast = zend_ast_list_add(args_ast, zend_ast_create(ZEND_AST_UNPACK,
					zend_ast_create(ZEND_AST_VAR,
						zend_ast_create_zval_from_str(zend_string_copy(param_names[argc + variadic_partial])))));
	}
	if (variadic_partial) {
		if (function->common.fn_flags & ZEND_ACC_VARIADIC) {
			args_ast = zend_ast_list_add(args_ast, zend_ast_create(ZEND_AST_UNPACK,
						zend_ast_create(ZEND_AST_VAR,
							zend_ast_create_zval_from_str(zend_string_copy(param_names[argc])))));
		} else if (forward_superfluous_args) {
			/* When a '...' placeholder is used, and the underlying function is
			 * not variadic, superfluous arguments are forwarded.
			 * Add a ...array_slice(func_get_args(), n) argument, which should
			 * be compiled as ZEND_AST_UNPACK + ZEND_FUNC_GET_ARGS. */

			zend_ast *func_get_args_name_ast = zend_ast_create_zval_from_str(
					zend_string_copy(ZSTR_KNOWN(ZEND_STR_FUNC_GET_ARGS)));
			func_get_args_name_ast->attr = ZEND_NAME_FQ;

			zend_ast *array_slice_name_ast = zend_ast_create_zval_from_str(
					zend_string_copy(ZSTR_KNOWN(ZEND_STR_ARRAY_SLICE)));
			array_slice_name_ast->attr = ZEND_NAME_FQ;

			args_ast = zend_ast_list_add(args_ast,
				zend_ast_create(ZEND_AST_UNPACK,
					zend_ast_create(ZEND_AST_CALL,
							array_slice_name_ast,
							zend_ast_create_list(2, ZEND_AST_ARG_LIST,
								zend_ast_create(ZEND_AST_CALL,
									func_get_args_name_ast,
									zend_ast_create_list(0, ZEND_AST_ARG_LIST)),
								zend_ast_create_zval_from_long(num_args)))));
		}
	}

	if (is_assert) {
		zend_ast *func_name_ast = zend_ast_create_zval_from_str(ZSTR_KNOWN(ZEND_STR_CALL_USER_FUNC));
		func_name_ast->attr = ZEND_NAME_FQ;
		call_ast = zend_ast_create(ZEND_AST_CALL, func_name_ast, args_ast);
	} else if (function->common.fn_flags & ZEND_ACC_CLOSURE) {
		zend_ast *fn_ast = zend_ast_create(ZEND_AST_VAR,
					zend_ast_create_zval_from_str(zend_string_copy(param_names[argc + variadic_partial + (extra_named_params != NULL)])));
		call_ast = zend_ast_create(ZEND_AST_CALL, fn_ast, args_ast);
	} else if (Z_TYPE_P(this_ptr) == IS_OBJECT) {
		zend_ast *this_ast = zend_ast_create(ZEND_AST_VAR,
					zend_ast_create_zval_from_str(ZSTR_KNOWN(ZEND_STR_THIS)));
		zend_ast *method_name_ast = zend_ast_create_zval_from_str(
				zend_string_copy(function->common.function_name));
		call_ast = zend_ast_create(ZEND_AST_METHOD_CALL, this_ast,
				method_name_ast, args_ast);
	} else if (called_scope) {
		zend_ast *class_name_ast = zend_ast_create_zval_from_str(ZSTR_KNOWN(ZEND_STR_STATIC));
		class_name_ast->attr = ZEND_NAME_NOT_FQ;
		zend_ast *method_name_ast = zend_ast_create_zval_from_str(
				zend_string_copy(function->common.function_name));
		call_ast = zend_ast_create(ZEND_AST_STATIC_CALL, class_name_ast,
				method_name_ast, args_ast);
	} else {
		zend_ast *func_name_ast = zend_ast_create_zval_from_str(zend_string_copy(function->common.function_name));
		func_name_ast->attr = ZEND_NAME_FQ;
		call_ast = zend_ast_create(ZEND_AST_CALL, func_name_ast, args_ast);
	}

	/* Void functions can not 'return $expr' */
	if (ZEND_TYPE_FULL_MASK(return_type) & MAY_BE_VOID) {
		stmts_ast = zend_ast_list_add(stmts_ast, call_ast);
	} else {
		zend_ast *return_ast = zend_ast_create(ZEND_AST_RETURN, call_ast);
		stmts_ast = zend_ast_list_add(stmts_ast, return_ast);
	}

	return stmts_ast;

error:
	zend_ast_destroy(args_ast);
	zend_ast_destroy(call_ast);
	return NULL;
}

static uint32_t zp_compute_num_required(zend_function *function,
		uint32_t orig_offset, uint32_t new_offset, uint32_t num_required) {
	if (orig_offset < function->common.num_args) {
		if (orig_offset < function->common.required_num_args) {
			num_required = MAX(num_required, new_offset + 1);
		}
	} else {
		ZEND_ASSERT(function->common.fn_flags & ZEND_ACC_VARIADIC);
		/* Placeholders that run into the variadic portion become
		 * required and make all params before them required */
		ZEND_ASSERT(orig_offset >= num_required);
		num_required = new_offset + 1;
	}

	return num_required;
}

/* Functions that do not allow to be called dynamically */
static const zend_known_string_id zp_non_dynamic_call_funcs[] = {
	ZEND_STR_COMPACT,
	ZEND_STR_EXTRACT,
	ZEND_STR_FUNC_GET_ARG,
	ZEND_STR_FUNC_GET_ARGS,
	ZEND_STR_FUNC_NUM_ARGS,
	ZEND_STR_GET_DEFINED_VARS,
};

static bool zp_is_non_dynamic_call_func(zend_function *function)
{
	for (size_t i = 0; i < sizeof(zp_non_dynamic_call_funcs) / sizeof(zp_non_dynamic_call_funcs[0]); i++) {
		if (zend_string_equals(function->common.function_name, ZSTR_KNOWN(zp_non_dynamic_call_funcs[i]))) {
			return true;
		}
	}

	return false;
}

/* Compile PFA to an op_array */
static zend_op_array *zp_compile(zval *this_ptr, zend_function *function,
		uint32_t argc, zval *argv, zend_array *extra_named_params,
		const zend_array *named_positions,
		const zend_op_array *declaring_op_array,
		const zend_op *declaring_opline, void **cache_slot,
		bool uses_variadic_placeholder) {

	zend_op_array *op_array = NULL;

	if (UNEXPECTED(zp_is_non_dynamic_call_func(function))) {
		zend_throw_error(NULL, "Cannot call %.*s() dynamically",
			(int) ZSTR_LEN(function->common.function_name), ZSTR_VAL(function->common.function_name));
		return NULL;
	}

	if (UNEXPECTED(zp_args_check(function, argc, argv, extra_named_params, uses_variadic_placeholder) != SUCCESS)) {
		ZEND_ASSERT(EG(exception));
		return NULL;
	}

	zend_class_entry *called_scope;
	if (Z_TYPE_P(this_ptr) == IS_OBJECT) {
		called_scope = Z_OBJCE_P(this_ptr);
	} else {
		called_scope = Z_CE_P(this_ptr);
	}

	/* CG(ast_arena) is usually NULL, so we can't just make a snapshot */
	zend_arena *orig_ast_arena = CG(ast_arena);
	CG(ast_arena) = zend_arena_create(1024 * 4);

	uint32_t orig_lineno = CG(zend_lineno);
	CG(zend_lineno) = zend_get_executed_lineno();

	uint32_t new_argc = argc;

	if (uses_variadic_placeholder) {
		new_argc = MAX(new_argc, function->common.num_args);
	}

	zval *tmp = zend_arena_alloc(&CG(ast_arena), new_argc * sizeof(zval));
	memcpy(tmp, argv, argc * sizeof(zval));
	argv = tmp;

	/* Compute param positions and number of required args, add implicit
	 * placeholders.
	 *
	 * Parameters are placed in the following order:
	 * - Positional placeholders
	 * - Then named placeholders in their syntax order
	 * - Then implicit placeholders added by '...'
	 */
	uint32_t num_params = 0;
	uint32_t num_required = 0;
	uint32_t *arg_to_param_offset_map = zend_arena_alloc(&CG(ast_arena), sizeof(uint32_t*) * new_argc);
	{
		uint32_t num_positional = 0;

		/* First, we handle explicit placeholders */
		for (uint32_t arg_offset = 0; arg_offset < argc; arg_offset++) {
			if (!Z_IS_PLACEHOLDER_P(&argv[arg_offset])) {
				continue;
			}

			num_params++;

			zend_arg_info *arg_info = &function->common.arg_info[MIN(arg_offset, function->common.num_args)];
			zval *named_pos = named_positions ? zend_hash_find(named_positions, arg_info->name) : NULL;
			uint32_t param_offset;
			if (named_pos) {
				/* Placeholder is sent as named arg. 'num_positional' can not
				 * change at this point. */
				param_offset = num_positional + Z_LVAL_P(named_pos);
			} else {
				/* Placeholder is sent as positional */
				param_offset = num_positional++;
			}

			arg_to_param_offset_map[arg_offset] = param_offset;

			num_required = zp_compute_num_required(function,
					arg_offset, param_offset, num_required);
		}

		/* Handle implicit placeholders added by '...' */
		if (uses_variadic_placeholder) {
			for (uint32_t arg_offset = 0; arg_offset < new_argc; arg_offset++) {
				if (arg_offset < argc && !Z_ISUNDEF(argv[arg_offset])) {
					continue;
				}

				/* Unspecified parameters become placeholders */
				Z_TYPE_INFO(argv[arg_offset]) = _IS_PLACEHOLDER;

				num_params++;

				uint32_t param_offset = num_params - 1;

				arg_to_param_offset_map[arg_offset] = param_offset;

				num_required = zp_compute_num_required(function,
						arg_offset, param_offset, num_required);
			}
		}
	}

	argc = new_argc;

	/* Assign variable names */

	uint32_t num_names = argc + uses_variadic_placeholder + (extra_named_params != NULL)
		+ ((function->common.fn_flags & ZEND_ACC_CLOSURE) != 0);
	zend_string **param_names = zend_arena_calloc(&CG(ast_arena),
			num_names, sizeof(zend_string*));
	zp_assign_names(param_names, num_names, argc, argv, function,
			uses_variadic_placeholder, extra_named_params);

	/* Generate AST */

	zend_ast *lexical_vars_ast = zend_ast_create_list(0, ZEND_AST_CLOSURE_USES);
	zend_ast *params_ast = zend_ast_create_list(0, ZEND_AST_ARG_LIST);
	zend_ast *return_type_ast = NULL;
	zend_ast *stmts_ast = zend_ast_create_list(0, ZEND_AST_STMT_LIST);
	zend_ast *attributes_ast = NULL;

	/* Generate AST for params and lexical vars */
	{
		/* The inner Closure, if any, is assumed to be the first lexical var by
		 * do_closure_bind(). */
		if (function->common.fn_flags & ZEND_ACC_CLOSURE) {
			zend_ast *lexical_var_ast = zend_ast_create_zval_from_str(
					zend_string_copy(param_names[argc + uses_variadic_placeholder + (extra_named_params != NULL)]));
			lexical_vars_ast = zend_ast_list_add(lexical_vars_ast, lexical_var_ast);
		}

		zend_ast **params = zend_arena_calloc(&CG(ast_arena), num_params, sizeof(zend_ast*));
		for (uint32_t offset = 0; offset < argc; offset++) {
			if (Z_IS_PLACEHOLDER_P(&argv[offset])) {
				zend_arg_info *arg_info = &function->common.arg_info[MIN(offset, function->common.num_args)];

				int param_flags = 0;
				if (zp_arg_must_be_sent_by_ref(function, offset+1)) {
					param_flags |= ZEND_PARAM_REF;
				}

				uint32_t param_offset = arg_to_param_offset_map[offset];
				zend_ast *param_type_ast = zp_type_to_ast(arg_info->type);
				zend_ast *default_value_ast = NULL;
				if (param_offset >= num_required) {
					zval default_value;
					if (zp_get_param_default_value(&default_value, function, offset) == FAILURE) {
						for (uint32_t i = 0; i < num_params; i++) {
							zend_ast_destroy(params[i]);
						}
						goto error;
					}
					default_value_ast = zend_ast_create_zval(&default_value);
				}

				ZEND_ASSERT(offset < function->common.num_args || (function->common.fn_flags & ZEND_ACC_VARIADIC));

				zend_ast *attributes_ast = zp_param_attributes_to_ast(function, MIN(offset, function->common.num_args));
				params[param_offset] = zend_ast_create_ex(ZEND_AST_PARAM,
						param_flags, param_type_ast,
						zend_ast_create_zval_from_str(
							zend_string_copy(param_names[offset])),
						default_value_ast, attributes_ast, NULL, NULL);

			} else if (!Z_ISUNDEF(argv[offset])) {
				// TODO: If the pre-bound parameter is a literal, it can be a
				// literal in the function body instead of a lexical var.
				zend_ast *lexical_var_ast = zend_ast_create_zval_from_str(
						zend_string_copy(param_names[offset]));
				if (zp_arg_must_be_sent_by_ref(function, offset+1)) {
					lexical_var_ast->attr = ZEND_BIND_REF;
				}
				lexical_vars_ast = zend_ast_list_add(
						lexical_vars_ast, lexical_var_ast);
			}
		}

		for (uint32_t i = 0; i < num_params; i++) {
			params_ast = zend_ast_list_add(params_ast, params[i]);
		}
	}

	if (extra_named_params) {
		zend_ast *lexical_var_ast = zend_ast_create_zval_from_str(
				zend_string_copy(param_names[argc + uses_variadic_placeholder]));
		lexical_vars_ast = zend_ast_list_add(lexical_vars_ast, lexical_var_ast);
	}

	/* If we have a variadic placeholder and the underlying function is
	 * variadic, add a variadic param. */
	if (uses_variadic_placeholder
			&& (function->common.fn_flags & ZEND_ACC_VARIADIC)) {
		zend_arg_info *arg_info = &function->common.arg_info[function->common.num_args];
		int param_flags = ZEND_PARAM_VARIADIC;
		if (zp_arg_must_be_sent_by_ref(function, function->common.num_args+1)) {
			param_flags |= ZEND_PARAM_REF;
		}
		zend_ast *param_type_ast = zp_type_to_ast(arg_info->type);
		zend_ast *attributes_ast = zp_param_attributes_to_ast(function, function->common.num_args);
		params_ast = zend_ast_list_add(params_ast, zend_ast_create_ex(ZEND_AST_PARAM,
				param_flags, param_type_ast,
				zend_ast_create_zval_from_str(
					zend_string_copy(param_names[argc])),
				NULL, attributes_ast, NULL, NULL));
	}

	zend_type return_type = {0};
	if (function->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
		return_type = (function->common.arg_info-1)->type;
		return_type_ast = zp_type_to_ast(return_type);
	}

	/**
	 * Generate function body.
	 *
	 * If we may need to forward superflous arguments, do that conditionally, as
	 * it's faster:
	 *
	 * if (func_num_args() <= n) {
	 *    // normal call
	 * } else {
	 *    // call with superflous arg forwarding
	 * }
	 *
	 * The func_num_args() call should be compiled to a single FUNC_NUM_ARGS op.
	 */

	if (uses_variadic_placeholder && !(function->common.fn_flags & ZEND_ACC_VARIADIC)) {
		zend_ast *no_forwarding_ast = zend_ast_create_list(0, ZEND_AST_STMT_LIST);
		zend_ast *forwarding_ast = zend_ast_create_list(0, ZEND_AST_STMT_LIST);

		no_forwarding_ast = zp_compile_forwarding_call(this_ptr, function,
				argc, argv, extra_named_params,
				param_names, uses_variadic_placeholder, num_params,
				called_scope, return_type, false, no_forwarding_ast);

		if (!no_forwarding_ast) {
			ZEND_ASSERT(EG(exception));
			goto error;
		}

		forwarding_ast = zp_compile_forwarding_call(this_ptr, function,
				argc, argv, extra_named_params,
				param_names, uses_variadic_placeholder, num_params,
				called_scope, return_type, true, forwarding_ast);

		if (!forwarding_ast) {
			ZEND_ASSERT(EG(exception));
			zend_ast_destroy(no_forwarding_ast);
			goto error;
		}

		zend_ast *func_num_args_name_ast = zend_ast_create_zval_from_str(
				zend_string_copy(ZSTR_KNOWN(ZEND_STR_FUNC_NUM_ARGS)));
		func_num_args_name_ast->attr = ZEND_NAME_FQ;

		stmts_ast = zend_ast_list_add(stmts_ast,
			zend_ast_create_list(2, ZEND_AST_IF,
				zend_ast_create(ZEND_AST_IF_ELEM,
					zend_ast_create_binary_op(ZEND_IS_SMALLER_OR_EQUAL,
						zend_ast_create(ZEND_AST_CALL, func_num_args_name_ast,
							zend_ast_create_list(0, ZEND_AST_ARG_LIST)),
						zend_ast_create_zval_from_long(num_params)),
					no_forwarding_ast),
				zend_ast_create(ZEND_AST_IF_ELEM,
					NULL,
					forwarding_ast)));
	} else {
		stmts_ast = zp_compile_forwarding_call(this_ptr, function,
				argc, argv, extra_named_params,
				param_names, uses_variadic_placeholder, num_params,
				called_scope, return_type, false, stmts_ast);

		if (!stmts_ast) {
			ZEND_ASSERT(EG(exception));
			goto error;
		}
	}

	/* Inherit the NoDiscard attribute */
	if (function->common.attributes) {
		zend_attribute *attr = zend_get_attribute_str(
				function->common.attributes, "nodiscard", strlen("nodiscard"));
		if (attr) {
			attributes_ast = zend_ast_create_list(1, ZEND_AST_ATTRIBUTE_GROUP,
					zp_attribute_to_ast(attr));
			attributes_ast = zend_ast_create_list(1, ZEND_AST_ATTRIBUTE_LIST,
					attributes_ast);
		}
	}

	int closure_flags = function->common.fn_flags & ZEND_ACC_RETURN_REFERENCE;
	zend_ast *closure_ast = zend_ast_create_decl(ZEND_AST_CLOSURE,
			closure_flags, CG(zend_lineno), NULL,
			NULL, params_ast, lexical_vars_ast, stmts_ast,
			return_type_ast, attributes_ast);

	if (Z_TYPE_P(this_ptr) != IS_OBJECT || zp_is_static_closure(function)) {
		((zend_ast_decl*)closure_ast)->flags |= ZEND_ACC_STATIC;
	}

#if ZEND_DEBUG
	{
		const char *tmp = getenv("DUMP_PFA_AST");
		if (tmp && ZEND_ATOL(tmp)) {
			zend_string *str = zend_ast_export("", closure_ast, "");
			fprintf(stderr, "PFA AST: %s\n", ZSTR_VAL(str));
			zend_string_release(str);
		}
	}
#endif

	zend_string *pfa_name = zp_pfa_name(declaring_op_array, declaring_opline);

	op_array = zend_accel_compile_pfa(closure_ast, declaring_op_array,
			declaring_opline, function, pfa_name);

	zend_ast_destroy(closure_ast);

clean:
	for (uint32_t i = 0; i < num_names; i++) {
		if (param_names[i]) {
			zend_string_release(param_names[i]);
		}
	}

	zend_arena_destroy(CG(ast_arena));
	CG(ast_arena) = orig_ast_arena;
	CG(zend_lineno) = orig_lineno;

	return op_array;

error:
	zend_ast_destroy(lexical_vars_ast);
	zend_ast_destroy(params_ast);
	zend_ast_destroy(return_type_ast);
	zend_ast_destroy(stmts_ast);
	zend_ast_destroy(attributes_ast);
	goto clean;
}

static zend_op_array *zp_get_op_array(zval *this_ptr, zend_function *function,
		uint32_t argc, zval *argv, zend_array *extra_named_params,
		const zend_array *named_positions,
		const zend_op_array *declaring_op_array,
		const zend_op *declaring_opline, void **cache_slot,
		bool uses_variadic_placeholder) {

	if (EXPECTED(function->type == ZEND_INTERNAL_FUNCTION
					? cache_slot[0] == function
					: cache_slot[0] == function->op_array.opcodes)) {
		return cache_slot[1];
	}

	zend_op_array *op_array = zend_accel_pfa_cache_get(declaring_op_array,
			declaring_opline, function);

	if (UNEXPECTED(!op_array)) {
		op_array = zp_compile(this_ptr, function, argc, argv,
			extra_named_params, named_positions, declaring_op_array, declaring_opline,
			cache_slot, uses_variadic_placeholder);
	}

	if (EXPECTED(op_array) && !(function->common.fn_flags & ZEND_ACC_NEVER_CACHE)) {
		cache_slot[0] = function->type == ZEND_INTERNAL_FUNCTION
			? (void*)function
			: (void*)function->op_array.opcodes;
		cache_slot[1] = op_array;
	}

	return op_array;
}

/* Bind pre-bound arguments as lexical vars */
static void zp_bind(zval *result, zend_function *function, uint32_t argc, zval *argv,
		zend_array *extra_named_params) {

	zend_arg_info *arg_infos = function->common.arg_info;
	uint32_t bind_offset = 0;

	if (function->common.fn_flags & ZEND_ACC_CLOSURE) {
		zval var;
		ZVAL_OBJ(&var, ZEND_CLOSURE_OBJECT(function));
		Z_ADDREF(var);
		zend_closure_bind_var_ex(result, bind_offset, &var);
		bind_offset += sizeof(Bucket);
	}

	for (uint32_t offset = 0; offset < argc; offset++) {
		zval *var = &argv[offset];
		if (Z_IS_PLACEHOLDER_P(var) || Z_ISUNDEF_P(var)) {
			continue;
		}
		zend_arg_info *arg_info;
		if (offset < function->common.num_args) {
			arg_info = &arg_infos[offset];
		} else if (function->common.fn_flags & ZEND_ACC_VARIADIC) {
			arg_info = &arg_infos[function->common.num_args];
		} else {
			arg_info = NULL;
		}
		if (arg_info && ZEND_TYPE_IS_SET(arg_info->type)
				&& UNEXPECTED(!zend_check_type_ex(&arg_info->type, var, function->common.scope, 0, 0))) {
			zend_verify_arg_error(function, arg_info, offset+1, var);
			zval_ptr_dtor(result);
			ZVAL_NULL(result);
			return;
		}
		ZEND_ASSERT(zp_arg_must_be_sent_by_ref(function, offset+1) ? Z_ISREF_P(var) : !Z_ISREF_P(var));
		zend_closure_bind_var_ex(result, bind_offset, var);
		bind_offset += sizeof(Bucket);
	}

	if (extra_named_params) {
		zval var;
		ZVAL_ARR(&var, extra_named_params);
		Z_ADDREF(var);
		zend_closure_bind_var_ex(result, bind_offset, &var);
	}
}

void zend_partial_create(zval *result, zval *this_ptr, zend_function *function,
		uint32_t argc, zval *argv, zend_array *extra_named_params,
		const zend_array *named_positions,
		const zend_op_array *declaring_op_array,
		const zend_op *declaring_opline, void **cache_slot,
		bool uses_variadic_placeholder) {

	zend_op_array *op_array = zp_get_op_array(this_ptr, function, argc, argv,
			extra_named_params, named_positions,
			declaring_op_array, declaring_opline,
			cache_slot, uses_variadic_placeholder);

	if (UNEXPECTED(!op_array)) {
		ZEND_ASSERT(EG(exception));
		ZVAL_NULL(result);
		return;
	}

	zend_class_entry *called_scope;
	zval object;

	if (Z_TYPE_P(this_ptr) == IS_OBJECT) {
		called_scope = Z_OBJCE_P(this_ptr);
	} else {
		called_scope = Z_CE_P(this_ptr);
	}

	if (Z_TYPE_P(this_ptr) == IS_OBJECT && !zp_is_static_closure(function)) {
		ZVAL_COPY_VALUE(&object, this_ptr);
	} else {
		ZVAL_UNDEF(&object);
	}

	zend_create_partial_closure(result, (zend_function*)op_array,
			function->common.scope, called_scope, &object,
			(function->common.fn_flags & ZEND_ACC_CLOSURE) != 0);

	zp_bind(result, function, argc, argv, extra_named_params);
}

void zend_partial_op_array_dtor(zval *pDest)
{
	destroy_op_array(Z_PTR_P(pDest));
}
