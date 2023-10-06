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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Andrei Zmievski <andrei@php.net>                            |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_API_H
#define ZEND_API_H

#include "zend_modules.h"
#include "zend_list.h"
#include "zend_operators.h"
#include "zend_variables.h"
#include "zend_execute.h"
#include "zend_type_info.h"


BEGIN_EXTERN_C()

typedef struct _zend_function_entry {
	const char *fname;
	zif_handler handler;
	const struct _zend_internal_arg_info *arg_info;
	uint32_t num_args;
	uint32_t flags;
} zend_function_entry;

typedef struct _zend_fcall_info {
	size_t size;
	zval function_name;
	zval *retval;
	zval *params;
	zend_object *object;
	uint32_t param_count;
	/* This hashtable can also contain positional arguments (with integer keys),
	 * which will be appended to the normal params[]. This makes it easier to
	 * integrate APIs like call_user_func_array(). The usual restriction that
	 * there may not be position arguments after named arguments applies. */
	HashTable *named_params;
} zend_fcall_info;

typedef struct _zend_fcall_info_cache {
	zend_function *function_handler;
	zend_class_entry *calling_scope;
	zend_class_entry *called_scope;
	zend_object *object; /* Instance of object for method calls */
	zend_object *closure; /* Closure reference, only if the callable *is* the object */
} zend_fcall_info_cache;

#define ZEND_NS_NAME(ns, name)			ns "\\" name

/* ZEND_FN/ZEND_MN are inlined below to prevent pre-scan macro expansion,
 * which causes issues if the function name is also a macro name. */
#define ZEND_FN(name) zif_##name
#define ZEND_MN(name) zim_##name

#define ZEND_NAMED_FUNCTION(name)		void ZEND_FASTCALL name(INTERNAL_FUNCTION_PARAMETERS)
#define ZEND_FUNCTION(name)				ZEND_NAMED_FUNCTION(zif_##name)
#define ZEND_METHOD(classname, name)	ZEND_NAMED_FUNCTION(zim_##classname##_##name)

#define ZEND_FENTRY(zend_name, name, arg_info, flags)	{ #zend_name, name, arg_info, (uint32_t) (sizeof(arg_info)/sizeof(struct _zend_internal_arg_info)-1), flags },

#define ZEND_RAW_FENTRY(zend_name, name, arg_info, flags)   { zend_name, name, arg_info, (uint32_t) (sizeof(arg_info)/sizeof(struct _zend_internal_arg_info)-1), flags },

/* Same as ZEND_NAMED_FE */
#define ZEND_RAW_NAMED_FE(zend_name, name, arg_info) ZEND_RAW_FENTRY(#zend_name, name, arg_info, 0)

#define ZEND_NAMED_FE(zend_name, name, arg_info)	ZEND_RAW_FENTRY(#zend_name, name, arg_info, 0)
#define ZEND_FE(name, arg_info)						ZEND_RAW_FENTRY(#name, zif_##name, arg_info, 0)
#define ZEND_DEP_FE(name, arg_info)                 ZEND_RAW_FENTRY(#name, zif_##name, arg_info, ZEND_ACC_DEPRECATED)
#define ZEND_FALIAS(name, alias, arg_info)			ZEND_RAW_FENTRY(#name, zif_##alias, arg_info, 0)
#define ZEND_DEP_FALIAS(name, alias, arg_info)		ZEND_RAW_FENTRY(#name, zif_##alias, arg_info, ZEND_ACC_DEPRECATED)
#define ZEND_NAMED_ME(zend_name, name, arg_info, flags)	ZEND_FENTRY(zend_name, name, arg_info, flags)
#define ZEND_ME(classname, name, arg_info, flags)	ZEND_RAW_FENTRY(#name, zim_##classname##_##name, arg_info, flags)
#define ZEND_DEP_ME(classname, name, arg_info, flags) ZEND_RAW_FENTRY(#name, zim_##classname##_##name, arg_info, flags | ZEND_ACC_DEPRECATED)
#define ZEND_ABSTRACT_ME(classname, name, arg_info)	ZEND_RAW_FENTRY(#name, NULL, arg_info, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
#define ZEND_ABSTRACT_ME_WITH_FLAGS(classname, name, arg_info, flags)	ZEND_RAW_FENTRY(#name, NULL, arg_info, flags)
#define ZEND_MALIAS(classname, name, alias, arg_info, flags) ZEND_RAW_FENTRY(#name, zim_##classname##_##alias, arg_info, flags)
#define ZEND_ME_MAPPING(name, func_name, arg_info, flags) ZEND_RAW_FENTRY(#name, zif_##func_name, arg_info, flags)

#define ZEND_NS_FENTRY(ns, zend_name, name, arg_info, flags)		ZEND_RAW_FENTRY(ZEND_NS_NAME(ns, #zend_name), name, arg_info, flags)

#define ZEND_NS_RAW_FENTRY(ns, zend_name, name, arg_info, flags)	ZEND_RAW_FENTRY(ZEND_NS_NAME(ns, zend_name), name, arg_info, flags)
/**
 * Note that if you are asserting that a function is compile-time evaluable, you are asserting that
 *
 * 1. The function will always have the same result for the same arguments
 * 2. The function does not depend on global state such as ini settings or locale (e.g. mb_strtolower), number_format(), etc.
 * 3. The function does not have side effects. It is okay if they throw
 *    or warn on invalid arguments, as we detect this and will discard the evaluation result.
 * 4. The function will not take an unreasonable amount of time or memory to compute on code that may be seen in practice.
 *    (e.g. str_repeat is special cased to check the length instead of using this)
 */
#define ZEND_SUPPORTS_COMPILE_TIME_EVAL_FE(name, arg_info) ZEND_RAW_FENTRY(#name, zif_##name, arg_info, ZEND_ACC_COMPILE_TIME_EVAL)

/* Same as ZEND_NS_NAMED_FE */
#define ZEND_NS_RAW_NAMED_FE(ns, zend_name, name, arg_info)			ZEND_NS_RAW_FENTRY(ns, #zend_name, name, arg_info, 0)

#define ZEND_NS_NAMED_FE(ns, zend_name, name, arg_info)	ZEND_NS_RAW_FENTRY(ns, #zend_name, name, arg_info, 0)
#define ZEND_NS_FE(ns, name, arg_info)					ZEND_NS_RAW_FENTRY(ns, #name, zif_##name, arg_info, 0)
#define ZEND_NS_DEP_FE(ns, name, arg_info)				ZEND_NS_RAW_FENTRY(ns, #name, zif_##name, arg_info, ZEND_ACC_DEPRECATED)
#define ZEND_NS_FALIAS(ns, name, alias, arg_info)		ZEND_NS_RAW_FENTRY(ns, #name, zif_##alias, arg_info, 0)
#define ZEND_NS_DEP_FALIAS(ns, name, alias, arg_info)	ZEND_NS_RAW_FENTRY(ns, #name, zif_##alias, arg_info, ZEND_ACC_DEPRECATED)

#define ZEND_FE_END            { NULL, NULL, NULL, 0, 0 }

#define _ZEND_ARG_INFO_FLAGS(pass_by_ref, is_variadic, is_tentative) \
	(((pass_by_ref) << _ZEND_SEND_MODE_SHIFT) | ((is_variadic) ? _ZEND_IS_VARIADIC_BIT : 0) | ((is_tentative) ? _ZEND_IS_TENTATIVE_BIT : 0))

/* Arginfo structures without type information */
#define ZEND_ARG_INFO(pass_by_ref, name) \
	{ #name, ZEND_TYPE_INIT_NONE(_ZEND_ARG_INFO_FLAGS(pass_by_ref, 0, 0)), NULL },
#define ZEND_ARG_INFO_WITH_DEFAULT_VALUE(pass_by_ref, name, default_value) \
	{ #name, ZEND_TYPE_INIT_NONE(_ZEND_ARG_INFO_FLAGS(pass_by_ref, 0, 0)), default_value },
#define ZEND_ARG_VARIADIC_INFO(pass_by_ref, name) \
	{ #name, ZEND_TYPE_INIT_NONE(_ZEND_ARG_INFO_FLAGS(pass_by_ref, 1, 0)), NULL },

/* Arginfo structures with simple type information */
#define ZEND_ARG_TYPE_INFO(pass_by_ref, name, type_hint, allow_null) \
	{ #name, ZEND_TYPE_INIT_CODE(type_hint, allow_null, _ZEND_ARG_INFO_FLAGS(pass_by_ref, 0, 0)), NULL },
#define ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(pass_by_ref, name, type_hint, allow_null, default_value) \
	{ #name, ZEND_TYPE_INIT_CODE(type_hint, allow_null, _ZEND_ARG_INFO_FLAGS(pass_by_ref, 0, 0)), default_value },
#define ZEND_ARG_VARIADIC_TYPE_INFO(pass_by_ref, name, type_hint, allow_null) \
	{ #name, ZEND_TYPE_INIT_CODE(type_hint, allow_null, _ZEND_ARG_INFO_FLAGS(pass_by_ref, 1, 0)), NULL },

/* Arginfo structures with complex type information */
#define ZEND_ARG_TYPE_MASK(pass_by_ref, name, type_mask, default_value) \
	{ #name, ZEND_TYPE_INIT_MASK(type_mask | _ZEND_ARG_INFO_FLAGS(pass_by_ref, 0, 0)), default_value },
#define ZEND_ARG_OBJ_TYPE_MASK(pass_by_ref, name, class_name, type_mask, default_value) \
	{ #name, ZEND_TYPE_INIT_CLASS_CONST_MASK(#class_name, type_mask | _ZEND_ARG_INFO_FLAGS(pass_by_ref, 0, 0)), default_value },
#define ZEND_ARG_VARIADIC_OBJ_TYPE_MASK(pass_by_ref, name, class_name, type_mask) \
	{ #name, ZEND_TYPE_INIT_CLASS_CONST_MASK(#class_name, type_mask | _ZEND_ARG_INFO_FLAGS(pass_by_ref, 1, 0)), NULL },

/* Arginfo structures with object type information */
#define ZEND_ARG_OBJ_INFO(pass_by_ref, name, class_name, allow_null) \
	{ #name, ZEND_TYPE_INIT_CLASS_CONST(#class_name, allow_null, _ZEND_ARG_INFO_FLAGS(pass_by_ref, 0, 0)), NULL },
#define ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(pass_by_ref, name, class_name, allow_null, default_value) \
	{ #name, ZEND_TYPE_INIT_CLASS_CONST(#class_name, allow_null, _ZEND_ARG_INFO_FLAGS(pass_by_ref, 0, 0)), default_value },
#define ZEND_ARG_VARIADIC_OBJ_INFO(pass_by_ref, name, class_name, allow_null) \
	{ #name, ZEND_TYPE_INIT_CLASS_CONST(#class_name, allow_null, _ZEND_ARG_INFO_FLAGS(pass_by_ref, 1, 0)), NULL },

/* Legacy arginfo structures */
#define ZEND_ARG_ARRAY_INFO(pass_by_ref, name, allow_null) \
	{ #name, ZEND_TYPE_INIT_CODE(IS_ARRAY, allow_null, _ZEND_ARG_INFO_FLAGS(pass_by_ref, 0, 0)), NULL },
#define ZEND_ARG_CALLABLE_INFO(pass_by_ref, name, allow_null) \
	{ #name, ZEND_TYPE_INIT_CODE(IS_CALLABLE, allow_null, _ZEND_ARG_INFO_FLAGS(pass_by_ref, 0, 0)), NULL },

#define ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX2(name, return_reference, required_num_args, class_name, allow_null, is_tentative_return_type) \
	static const zend_internal_arg_info name[] = { \
		{ (const char*)(uintptr_t)(required_num_args), \
			ZEND_TYPE_INIT_CLASS_CONST(#class_name, allow_null, _ZEND_ARG_INFO_FLAGS(return_reference, 0, is_tentative_return_type)), NULL },

#define ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(name, return_reference, required_num_args, class_name, allow_null) \
	ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX2(name, return_reference, required_num_args, class_name, allow_null, 0)

#define ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(name, return_reference, required_num_args, class_name, allow_null) \
	ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX2(name, return_reference, required_num_args, class_name, allow_null, 1)

#define ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO(name, class_name, allow_null) \
	ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX2(name, 0, -1, class_name, allow_null, 0)

#define ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX2(name, return_reference, required_num_args, type, is_tentative_return_type) \
	static const zend_internal_arg_info name[] = { \
		{ (const char*)(uintptr_t)(required_num_args), ZEND_TYPE_INIT_MASK(type | _ZEND_ARG_INFO_FLAGS(return_reference, 0, is_tentative_return_type)), NULL },

#define ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(name, return_reference, required_num_args, type) \
	ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX2(name, return_reference, required_num_args, type, 0)

#define ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(name, return_reference, required_num_args, type) \
	ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX2(name, return_reference, required_num_args, type, 1)

#define ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX2(name, return_reference, required_num_args, class_name, type, is_tentative_return_type) \
	static const zend_internal_arg_info name[] = { \
		{ (const char*)(uintptr_t)(required_num_args), ZEND_TYPE_INIT_CLASS_CONST_MASK(#class_name, type | _ZEND_ARG_INFO_FLAGS(return_reference, 0, is_tentative_return_type)), NULL },

#define ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(name, return_reference, required_num_args, class_name, type) \
	ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX2(name, return_reference, required_num_args, class_name, type, 0)

#define ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(name, return_reference, required_num_args, class_name, type) \
	ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX2(name, return_reference, required_num_args, class_name, type, 1)

#define ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX2(name, return_reference, required_num_args, type, allow_null, is_tentative_return_type) \
	static const zend_internal_arg_info name[] = { \
		{ (const char*)(uintptr_t)(required_num_args), ZEND_TYPE_INIT_CODE(type, allow_null, _ZEND_ARG_INFO_FLAGS(return_reference, 0, is_tentative_return_type)), NULL },

#define ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(name, return_reference, required_num_args, type, allow_null) \
	ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX2(name, return_reference, required_num_args, type, allow_null, 0)

#define ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(name, return_reference, required_num_args, type, allow_null) \
	ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX2(name, return_reference, required_num_args, type, allow_null, 1)

#define ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(name, type, allow_null) \
	ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX2(name, 0, -1, type, allow_null, 0)

#define ZEND_BEGIN_ARG_INFO_EX(name, _unused, return_reference, required_num_args)	\
	static const zend_internal_arg_info name[] = { \
		{ (const char*)(uintptr_t)(required_num_args), ZEND_TYPE_INIT_NONE(_ZEND_ARG_INFO_FLAGS(return_reference, 0, 0)), NULL },
#define ZEND_BEGIN_ARG_INFO(name, _unused)	\
	ZEND_BEGIN_ARG_INFO_EX(name, {}, ZEND_RETURN_VALUE, -1)
#define ZEND_END_ARG_INFO()		};

/* Name macros */
#define ZEND_MODULE_STARTUP_N(module)       zm_startup_##module
#define ZEND_MODULE_SHUTDOWN_N(module)		zm_shutdown_##module
#define ZEND_MODULE_ACTIVATE_N(module)		zm_activate_##module
#define ZEND_MODULE_DEACTIVATE_N(module)	zm_deactivate_##module
#define ZEND_MODULE_POST_ZEND_DEACTIVATE_N(module)	zm_post_zend_deactivate_##module
#define ZEND_MODULE_INFO_N(module)			zm_info_##module
#define ZEND_MODULE_GLOBALS_CTOR_N(module)  zm_globals_ctor_##module
#define ZEND_MODULE_GLOBALS_DTOR_N(module)  zm_globals_dtor_##module

/* Declaration macros */
#define ZEND_MODULE_STARTUP_D(module)		zend_result ZEND_MODULE_STARTUP_N(module)(INIT_FUNC_ARGS)
#define ZEND_MODULE_SHUTDOWN_D(module)		zend_result ZEND_MODULE_SHUTDOWN_N(module)(SHUTDOWN_FUNC_ARGS)
#define ZEND_MODULE_ACTIVATE_D(module)		zend_result ZEND_MODULE_ACTIVATE_N(module)(INIT_FUNC_ARGS)
#define ZEND_MODULE_DEACTIVATE_D(module)	zend_result ZEND_MODULE_DEACTIVATE_N(module)(SHUTDOWN_FUNC_ARGS)
#define ZEND_MODULE_POST_ZEND_DEACTIVATE_D(module)	zend_result ZEND_MODULE_POST_ZEND_DEACTIVATE_N(module)(void)
#define ZEND_MODULE_INFO_D(module)			ZEND_COLD void ZEND_MODULE_INFO_N(module)(ZEND_MODULE_INFO_FUNC_ARGS)
#define ZEND_MODULE_GLOBALS_CTOR_D(module)  void ZEND_MODULE_GLOBALS_CTOR_N(module)(zend_##module##_globals *module##_globals)
#define ZEND_MODULE_GLOBALS_DTOR_D(module)  void ZEND_MODULE_GLOBALS_DTOR_N(module)(zend_##module##_globals *module##_globals)

#define ZEND_GET_MODULE(name) \
    BEGIN_EXTERN_C()\
	ZEND_DLEXPORT zend_module_entry *get_module(void) { return &name##_module_entry; }\
    END_EXTERN_C()

#define ZEND_BEGIN_MODULE_GLOBALS(module_name)		\
	typedef struct _zend_##module_name##_globals {
#define ZEND_END_MODULE_GLOBALS(module_name)		\
	} zend_##module_name##_globals;

#ifdef ZTS

#define ZEND_DECLARE_MODULE_GLOBALS(module_name)							\
	ts_rsrc_id module_name##_globals_id;
#define ZEND_EXTERN_MODULE_GLOBALS(module_name)								\
	extern ts_rsrc_id module_name##_globals_id;
#define ZEND_INIT_MODULE_GLOBALS(module_name, globals_ctor, globals_dtor)	\
	ts_allocate_id(&module_name##_globals_id, sizeof(zend_##module_name##_globals), (ts_allocate_ctor) globals_ctor, (ts_allocate_dtor) globals_dtor);
#define ZEND_MODULE_GLOBALS_ACCESSOR(module_name, v) ZEND_TSRMG(module_name##_globals_id, zend_##module_name##_globals *, v)
#ifdef ZEND_ENABLE_STATIC_TSRMLS_CACHE
#define ZEND_MODULE_GLOBALS_BULK(module_name) TSRMG_BULK_STATIC(module_name##_globals_id, zend_##module_name##_globals *)
#else
#define ZEND_MODULE_GLOBALS_BULK(module_name) TSRMG_BULK(module_name##_globals_id, zend_##module_name##_globals *)
#endif

#else

#define ZEND_DECLARE_MODULE_GLOBALS(module_name)							\
	zend_##module_name##_globals module_name##_globals;
#define ZEND_EXTERN_MODULE_GLOBALS(module_name)								\
	extern zend_##module_name##_globals module_name##_globals;
#define ZEND_INIT_MODULE_GLOBALS(module_name, globals_ctor, globals_dtor)	\
	globals_ctor(&module_name##_globals);
#define ZEND_MODULE_GLOBALS_ACCESSOR(module_name, v) (module_name##_globals.v)
#define ZEND_MODULE_GLOBALS_BULK(module_name) (&module_name##_globals)

#endif

#define INIT_CLASS_ENTRY(class_container, class_name, functions) \
	INIT_CLASS_ENTRY_EX(class_container, class_name, strlen(class_name), functions)

#define INIT_CLASS_ENTRY_EX(class_container, class_name, class_name_len, functions) \
	{															\
		memset(&class_container, 0, sizeof(zend_class_entry)); \
		class_container.name = zend_string_init_interned(class_name, class_name_len, 1); \
		class_container.default_object_handlers = &std_object_handlers;	\
		class_container.info.internal.builtin_functions = functions;	\
	}

#define INIT_CLASS_ENTRY_INIT_METHODS(class_container, functions) \
	{															\
		class_container.default_object_handlers = &std_object_handlers;	\
		class_container.constructor = NULL;						\
		class_container.destructor = NULL;						\
		class_container.clone = NULL;							\
		class_container.serialize = NULL;						\
		class_container.unserialize = NULL;						\
		class_container.create_object = NULL;					\
		class_container.get_static_method = NULL;				\
		class_container.__call = NULL;							\
		class_container.__callstatic = NULL;					\
		class_container.__tostring = NULL;						\
		class_container.__get = NULL;							\
		class_container.__set = NULL;							\
		class_container.__unset = NULL;							\
		class_container.__isset = NULL;							\
		class_container.__debugInfo = NULL;						\
		class_container.__serialize = NULL;						\
		class_container.__unserialize = NULL;					\
		class_container.parent = NULL;							\
		class_container.num_interfaces = 0;						\
		class_container.trait_names = NULL;						\
		class_container.num_traits = 0;							\
		class_container.trait_aliases = NULL;					\
		class_container.trait_precedences = NULL;				\
		class_container.interfaces = NULL;						\
		class_container.get_iterator = NULL;					\
		class_container.iterator_funcs_ptr = NULL;				\
		class_container.arrayaccess_funcs_ptr = NULL;			\
		class_container.info.internal.module = NULL;			\
		class_container.info.internal.builtin_functions = functions;	\
	}


#define INIT_NS_CLASS_ENTRY(class_container, ns, class_name, functions) \
	INIT_CLASS_ENTRY(class_container, ZEND_NS_NAME(ns, class_name), functions)

#define CE_STATIC_MEMBERS(ce) \
	((zval*)ZEND_MAP_PTR_GET((ce)->static_members_table))

#define CE_CONSTANTS_TABLE(ce) \
	zend_class_constants_table(ce)

#define CE_DEFAULT_PROPERTIES_TABLE(ce) \
	zend_class_default_properties_table(ce)

#define CE_BACKED_ENUM_TABLE(ce) \
	zend_class_backed_enum_table(ce)

#define ZEND_FCI_INITIALIZED(fci) ((fci).size != 0)
#define ZEND_FCC_INITIALIZED(fcc) ((fcc).function_handler != NULL)

ZEND_API int zend_next_free_module(void);

BEGIN_EXTERN_C()
ZEND_API zend_result zend_get_parameters_array_ex(uint32_t param_count, zval *argument_array);

/* internal function to efficiently copy parameters when executing __call() */
ZEND_API zend_result zend_copy_parameters_array(uint32_t param_count, zval *argument_array);

#define zend_get_parameters_array(ht, param_count, argument_array) \
	zend_get_parameters_array_ex(param_count, argument_array)
#define zend_parse_parameters_none() \
	(EXPECTED(ZEND_NUM_ARGS() == 0) ? SUCCESS : (zend_wrong_parameters_none_error(), FAILURE))
#define zend_parse_parameters_none_throw() \
	zend_parse_parameters_none()

/* Parameter parsing API -- andrei */

#define ZEND_PARSE_PARAMS_THROW 0 /* No longer used, zpp always uses exceptions */
#define ZEND_PARSE_PARAMS_QUIET (1<<1)
ZEND_API zend_result zend_parse_parameters(uint32_t num_args, const char *type_spec, ...);
ZEND_API zend_result zend_parse_parameters_ex(int flags, uint32_t num_args, const char *type_spec, ...);
/* NOTE: This must have at least one value in __VA_ARGS__ for the expression to be valid */
#define zend_parse_parameters_throw(num_args, ...) \
	zend_parse_parameters(num_args, __VA_ARGS__)
ZEND_API const char *zend_zval_type_name(const zval *arg);
ZEND_API const char *zend_zval_value_name(const zval *arg);
ZEND_API zend_string *zend_zval_get_legacy_type(const zval *arg);

ZEND_API zend_result zend_parse_method_parameters(uint32_t num_args, zval *this_ptr, const char *type_spec, ...);
ZEND_API zend_result zend_parse_method_parameters_ex(int flags, uint32_t num_args, zval *this_ptr, const char *type_spec, ...);

ZEND_API zend_result zend_parse_parameter(int flags, uint32_t arg_num, zval *arg, const char *spec, ...);

/* End of parameter parsing API -- andrei */

ZEND_API zend_result zend_register_functions(zend_class_entry *scope, const zend_function_entry *functions, HashTable *function_table, int type);
ZEND_API void zend_unregister_functions(const zend_function_entry *functions, int count, HashTable *function_table);
ZEND_API zend_result zend_startup_module(zend_module_entry *module_entry);
ZEND_API zend_module_entry* zend_register_internal_module(zend_module_entry *module_entry);
ZEND_API zend_module_entry* zend_register_module_ex(zend_module_entry *module, int module_type);
ZEND_API zend_result zend_startup_module_ex(zend_module_entry *module);
ZEND_API void zend_startup_modules(void);
ZEND_API void zend_collect_module_handlers(void);
ZEND_API void zend_destroy_modules(void);
ZEND_API void zend_check_magic_method_implementation(
		const zend_class_entry *ce, const zend_function *fptr, zend_string *lcname, int error_type);
ZEND_API void zend_add_magic_method(zend_class_entry *ce, zend_function *fptr, zend_string *lcname);

ZEND_API zend_class_entry *zend_register_internal_class(zend_class_entry *class_entry);
ZEND_API zend_class_entry *zend_register_internal_class_ex(zend_class_entry *class_entry, zend_class_entry *parent_ce);
ZEND_API zend_class_entry *zend_register_internal_interface(zend_class_entry *orig_class_entry);
ZEND_API void zend_class_implements(zend_class_entry *class_entry, int num_interfaces, ...);

ZEND_API zend_result zend_register_class_alias_ex(const char *name, size_t name_len, zend_class_entry *ce, bool persistent);

static zend_always_inline zend_result zend_register_class_alias(const char *name, zend_class_entry *ce) {
	return zend_register_class_alias_ex(name, strlen(name), ce, 1);
}
#define zend_register_ns_class_alias(ns, name, ce) \
	zend_register_class_alias_ex(ZEND_NS_NAME(ns, name), sizeof(ZEND_NS_NAME(ns, name))-1, ce, 1)

ZEND_API void zend_disable_functions(const char *function_list);
ZEND_API zend_result zend_disable_class(const char *class_name, size_t class_name_length);

ZEND_API ZEND_COLD void zend_wrong_param_count(void);
ZEND_API ZEND_COLD void zend_wrong_property_read(zval *object, zval *property);

#define IS_CALLABLE_CHECK_SYNTAX_ONLY (1<<0)
#define IS_CALLABLE_SUPPRESS_DEPRECATIONS (1<<1)

ZEND_API void zend_release_fcall_info_cache(zend_fcall_info_cache *fcc);
ZEND_API zend_string *zend_get_callable_name_ex(zval *callable, zend_object *object);
ZEND_API zend_string *zend_get_callable_name(zval *callable);
ZEND_API bool zend_is_callable_at_frame(
		zval *callable, zend_object *object, zend_execute_data *frame,
		uint32_t check_flags, zend_fcall_info_cache *fcc, char **error);
ZEND_API bool zend_is_callable_ex(zval *callable, zend_object *object, uint32_t check_flags, zend_string **callable_name, zend_fcall_info_cache *fcc, char **error);
ZEND_API bool zend_is_callable(zval *callable, uint32_t check_flags, zend_string **callable_name);
ZEND_API bool zend_make_callable(zval *callable, zend_string **callable_name);
ZEND_API const char *zend_get_module_version(const char *module_name);
ZEND_API zend_result zend_get_module_started(const char *module_name);

ZEND_API zend_property_info *zend_declare_typed_property(zend_class_entry *ce, zend_string *name, zval *property, int access_type, zend_string *doc_comment, zend_type type);

ZEND_API void zend_declare_property_ex(zend_class_entry *ce, zend_string *name, zval *property, int access_type, zend_string *doc_comment);
ZEND_API void zend_declare_property(zend_class_entry *ce, const char *name, size_t name_length, zval *property, int access_type);
ZEND_API void zend_declare_property_null(zend_class_entry *ce, const char *name, size_t name_length, int access_type);
ZEND_API void zend_declare_property_bool(zend_class_entry *ce, const char *name, size_t name_length, zend_long value, int access_type);
ZEND_API void zend_declare_property_long(zend_class_entry *ce, const char *name, size_t name_length, zend_long value, int access_type);
ZEND_API void zend_declare_property_double(zend_class_entry *ce, const char *name, size_t name_length, double value, int access_type);
ZEND_API void zend_declare_property_string(zend_class_entry *ce, const char *name, size_t name_length, const char *value, int access_type);
ZEND_API void zend_declare_property_stringl(zend_class_entry *ce, const char *name, size_t name_length, const char *value, size_t value_len, int access_type);

ZEND_API zend_class_constant *zend_declare_typed_class_constant(zend_class_entry *ce, zend_string *name, zval *value, int access_type, zend_string *doc_comment, zend_type type);
ZEND_API zend_class_constant *zend_declare_class_constant_ex(zend_class_entry *ce, zend_string *name, zval *value, int access_type, zend_string *doc_comment);
ZEND_API void zend_declare_class_constant(zend_class_entry *ce, const char *name, size_t name_length, zval *value);
ZEND_API void zend_declare_class_constant_null(zend_class_entry *ce, const char *name, size_t name_length);
ZEND_API void zend_declare_class_constant_long(zend_class_entry *ce, const char *name, size_t name_length, zend_long value);
ZEND_API void zend_declare_class_constant_bool(zend_class_entry *ce, const char *name, size_t name_length, bool value);
ZEND_API void zend_declare_class_constant_double(zend_class_entry *ce, const char *name, size_t name_length, double value);
ZEND_API void zend_declare_class_constant_stringl(zend_class_entry *ce, const char *name, size_t name_length, const char *value, size_t value_length);
ZEND_API void zend_declare_class_constant_string(zend_class_entry *ce, const char *name, size_t name_length, const char *value);

ZEND_API zend_result zend_update_class_constant(zend_class_constant *c, const zend_string *name, zend_class_entry *scope);
ZEND_API zend_result zend_update_class_constants(zend_class_entry *class_type);
ZEND_API HashTable *zend_separate_class_constants_table(zend_class_entry *class_type);

static zend_always_inline HashTable *zend_class_constants_table(zend_class_entry *ce) {
	if ((ce->ce_flags & ZEND_ACC_HAS_AST_CONSTANTS) && ZEND_MAP_PTR(ce->mutable_data)) {
		zend_class_mutable_data *mutable_data =
			(zend_class_mutable_data*)ZEND_MAP_PTR_GET_IMM(ce->mutable_data);
		if (mutable_data && mutable_data->constants_table) {
			return mutable_data->constants_table;
		} else {
			return zend_separate_class_constants_table(ce);
		}
	} else {
		return &ce->constants_table;
	}
}

static zend_always_inline zval *zend_class_default_properties_table(zend_class_entry *ce) {
	if ((ce->ce_flags & ZEND_ACC_HAS_AST_PROPERTIES) && ZEND_MAP_PTR(ce->mutable_data)) {
		zend_class_mutable_data *mutable_data =
			(zend_class_mutable_data*)ZEND_MAP_PTR_GET_IMM(ce->mutable_data);
		return mutable_data->default_properties_table;
	} else {
		return ce->default_properties_table;
	}
}

static zend_always_inline void zend_class_set_backed_enum_table(zend_class_entry *ce, HashTable *backed_enum_table)
{
	if (ZEND_MAP_PTR(ce->mutable_data) && ce->type == ZEND_USER_CLASS) {
		zend_class_mutable_data *mutable_data = (zend_class_mutable_data*)ZEND_MAP_PTR_GET_IMM(ce->mutable_data);
		mutable_data->backed_enum_table = backed_enum_table;
	} else {
		ce->backed_enum_table = backed_enum_table;
	}
}

static zend_always_inline HashTable *zend_class_backed_enum_table(zend_class_entry *ce)
{
	if (ZEND_MAP_PTR(ce->mutable_data) && ce->type == ZEND_USER_CLASS) {
		zend_class_mutable_data *mutable_data = (zend_class_mutable_data*)ZEND_MAP_PTR_GET_IMM(ce->mutable_data);
		return mutable_data->backed_enum_table;
	} else {
		return ce->backed_enum_table;
	}
}

ZEND_API void zend_update_property_ex(zend_class_entry *scope, zend_object *object, zend_string *name, zval *value);
ZEND_API void zend_update_property(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length, zval *value);
ZEND_API void zend_update_property_null(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length);
ZEND_API void zend_update_property_bool(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length, zend_long value);
ZEND_API void zend_update_property_long(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length, zend_long value);
ZEND_API void zend_update_property_double(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length, double value);
ZEND_API void zend_update_property_str(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length, zend_string *value);
ZEND_API void zend_update_property_string(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length, const char *value);
ZEND_API void zend_update_property_stringl(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length, const char *value, size_t value_length);
ZEND_API void zend_unset_property(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length);

ZEND_API zend_result zend_update_static_property_ex(zend_class_entry *scope, zend_string *name, zval *value);
ZEND_API zend_result zend_update_static_property(zend_class_entry *scope, const char *name, size_t name_length, zval *value);
ZEND_API zend_result zend_update_static_property_null(zend_class_entry *scope, const char *name, size_t name_length);
ZEND_API zend_result zend_update_static_property_bool(zend_class_entry *scope, const char *name, size_t name_length, zend_long value);
ZEND_API zend_result zend_update_static_property_long(zend_class_entry *scope, const char *name, size_t name_length, zend_long value);
ZEND_API zend_result zend_update_static_property_double(zend_class_entry *scope, const char *name, size_t name_length, double value);
ZEND_API zend_result zend_update_static_property_string(zend_class_entry *scope, const char *name, size_t name_length, const char *value);
ZEND_API zend_result zend_update_static_property_stringl(zend_class_entry *scope, const char *name, size_t name_length, const char *value, size_t value_length);

ZEND_API zval *zend_read_property_ex(zend_class_entry *scope, zend_object *object, zend_string *name, bool silent, zval *rv);
ZEND_API zval *zend_read_property(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length, bool silent, zval *rv);

ZEND_API zval *zend_read_static_property_ex(zend_class_entry *scope, zend_string *name, bool silent);
ZEND_API zval *zend_read_static_property(zend_class_entry *scope, const char *name, size_t name_length, bool silent);

ZEND_API const char *zend_get_type_by_const(int type);

#define ZEND_THIS                           (&EX(This))

#define getThis()							((Z_TYPE_P(ZEND_THIS) == IS_OBJECT) ? ZEND_THIS : NULL)
#define ZEND_IS_METHOD_CALL()				(EX(func)->common.scope != NULL)

#define WRONG_PARAM_COUNT					ZEND_WRONG_PARAM_COUNT()
#define ZEND_NUM_ARGS()						EX_NUM_ARGS()
#define ZEND_WRONG_PARAM_COUNT()					{ zend_wrong_param_count(); return; }

#ifndef ZEND_WIN32
#define DLEXPORT
#endif

#define array_init(arg)				ZVAL_ARR((arg), zend_new_array(0))
#define array_init_size(arg, size)	ZVAL_ARR((arg), zend_new_array(size))
ZEND_API void object_init(zval *arg);
ZEND_API zend_result object_init_ex(zval *arg, zend_class_entry *ce);
ZEND_API zend_result object_and_properties_init(zval *arg, zend_class_entry *ce, HashTable *properties);
ZEND_API void object_properties_init(zend_object *object, zend_class_entry *class_type);
ZEND_API void object_properties_init_ex(zend_object *object, HashTable *properties);
ZEND_API void object_properties_load(zend_object *object, HashTable *properties);

ZEND_API void zend_merge_properties(zval *obj, HashTable *properties);

ZEND_API void add_assoc_long_ex(zval *arg, const char *key, size_t key_len, zend_long n);
ZEND_API void add_assoc_null_ex(zval *arg, const char *key, size_t key_len);
ZEND_API void add_assoc_bool_ex(zval *arg, const char *key, size_t key_len, bool b);
ZEND_API void add_assoc_resource_ex(zval *arg, const char *key, size_t key_len, zend_resource *r);
ZEND_API void add_assoc_double_ex(zval *arg, const char *key, size_t key_len, double d);
ZEND_API void add_assoc_str_ex(zval *arg, const char *key, size_t key_len, zend_string *str);
ZEND_API void add_assoc_string_ex(zval *arg, const char *key, size_t key_len, const char *str);
ZEND_API void add_assoc_stringl_ex(zval *arg, const char *key, size_t key_len, const char *str, size_t length);
ZEND_API void add_assoc_array_ex(zval *arg, const char *key, size_t key_len, zend_array *arr);
ZEND_API void add_assoc_object_ex(zval *arg, const char *key, size_t key_len, zend_object *obj);
ZEND_API void add_assoc_reference_ex(zval *arg, const char *key, size_t key_len, zend_reference *ref);
ZEND_API void add_assoc_zval_ex(zval *arg, const char *key, size_t key_len, zval *value);

static zend_always_inline void add_assoc_long(zval *arg, const char *key, zend_long n) {
	add_assoc_long_ex(arg, key, strlen(key), n);
}
static zend_always_inline void add_assoc_null(zval *arg, const char *key) {
	add_assoc_null_ex(arg, key, strlen(key));
}
static zend_always_inline void add_assoc_bool(zval *arg, const char *key, bool b) {
	add_assoc_bool_ex(arg, key, strlen(key), b);
}
static zend_always_inline void add_assoc_resource(zval *arg, const char *key, zend_resource *r) {
	add_assoc_resource_ex(arg, key, strlen(key), r);
}
static zend_always_inline void add_assoc_double(zval *arg, const char *key, double d) {
	add_assoc_double_ex(arg, key, strlen(key), d);
}
static zend_always_inline void add_assoc_str(zval *arg, const char *key, zend_string *str) {
	add_assoc_str_ex(arg, key, strlen(key), str);
}
static zend_always_inline void add_assoc_string(zval *arg, const char *key, const char *str) {
	add_assoc_string_ex(arg, key, strlen(key), str);
}
static zend_always_inline void add_assoc_stringl(zval *arg, const char *key, const char *str, size_t length) {
	add_assoc_stringl_ex(arg, key, strlen(key), str, length);
}
static zend_always_inline void add_assoc_array(zval *arg, const char *key, zend_array *arr) {
	add_assoc_array_ex(arg, key, strlen(key), arr);
}
static zend_always_inline void add_assoc_object(zval *arg, const char *key, zend_object *obj) {
	add_assoc_object_ex(arg, key, strlen(key), obj);
}
static zend_always_inline void add_assoc_reference(zval *arg, const char *key, zend_reference *ref) {
	add_assoc_reference_ex(arg, key, strlen(key), ref);
}
static zend_always_inline void add_assoc_zval(zval *arg, const char *key, zval *value) {
	add_assoc_zval_ex(arg, key, strlen(key), value);
}

ZEND_API void add_index_long(zval *arg, zend_ulong index, zend_long n);
ZEND_API void add_index_null(zval *arg, zend_ulong index);
ZEND_API void add_index_bool(zval *arg, zend_ulong index, bool b);
ZEND_API void add_index_resource(zval *arg, zend_ulong index, zend_resource *r);
ZEND_API void add_index_double(zval *arg, zend_ulong index, double d);
ZEND_API void add_index_str(zval *arg, zend_ulong index, zend_string *str);
ZEND_API void add_index_string(zval *arg, zend_ulong index, const char *str);
ZEND_API void add_index_stringl(zval *arg, zend_ulong index, const char *str, size_t length);
ZEND_API void add_index_array(zval *arg, zend_ulong index, zend_array *arr);
ZEND_API void add_index_object(zval *arg, zend_ulong index, zend_object *obj);
ZEND_API void add_index_reference(zval *arg, zend_ulong index, zend_reference *ref);

static zend_always_inline zend_result add_index_zval(zval *arg, zend_ulong index, zval *value)
{
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, value) ? SUCCESS : FAILURE;
}

ZEND_API zend_result add_next_index_long(zval *arg, zend_long n);
ZEND_API zend_result add_next_index_null(zval *arg);
ZEND_API zend_result add_next_index_bool(zval *arg, bool b);
ZEND_API zend_result add_next_index_resource(zval *arg, zend_resource *r);
ZEND_API zend_result add_next_index_double(zval *arg, double d);
ZEND_API zend_result add_next_index_str(zval *arg, zend_string *str);
ZEND_API zend_result add_next_index_string(zval *arg, const char *str);
ZEND_API zend_result add_next_index_stringl(zval *arg, const char *str, size_t length);
ZEND_API zend_result add_next_index_array(zval *arg, zend_array *arr);
ZEND_API zend_result add_next_index_object(zval *arg, zend_object *obj);
ZEND_API zend_result add_next_index_reference(zval *arg, zend_reference *ref);

static zend_always_inline zend_result add_next_index_zval(zval *arg, zval *value)
{
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), value) ? SUCCESS : FAILURE;
}

ZEND_API zend_result array_set_zval_key(HashTable *ht, zval *key, zval *value);

ZEND_API void add_property_long_ex(zval *arg, const char *key, size_t key_len, zend_long l);
ZEND_API void add_property_null_ex(zval *arg, const char *key, size_t key_len);
ZEND_API void add_property_bool_ex(zval *arg, const char *key, size_t key_len, zend_long b);
ZEND_API void add_property_resource_ex(zval *arg, const char *key, size_t key_len, zend_resource *r);
ZEND_API void add_property_double_ex(zval *arg, const char *key, size_t key_len, double d);
ZEND_API void add_property_str_ex(zval *arg, const char *key, size_t key_len, zend_string *str);
ZEND_API void add_property_string_ex(zval *arg, const char *key, size_t key_len, const char *str);
ZEND_API void add_property_stringl_ex(zval *arg, const char *key, size_t key_len,  const char *str, size_t length);
ZEND_API void add_property_array_ex(zval *arg, const char *key, size_t key_len, zend_array *arr);
ZEND_API void add_property_object_ex(zval *arg, const char *key, size_t key_len, zend_object *obj);
ZEND_API void add_property_reference_ex(zval *arg, const char *key, size_t key_len, zend_reference *ref);
ZEND_API void add_property_zval_ex(zval *arg, const char *key, size_t key_len, zval *value);

static zend_always_inline void add_property_long(zval *arg, const char *key, zend_long n) {
	add_property_long_ex(arg, key, strlen(key), n);
}
static zend_always_inline void add_property_null(zval *arg, const char *key) {
	add_property_null_ex(arg, key, strlen(key));
}
static zend_always_inline void add_property_bool(zval *arg, const char *key, bool b) {
	add_property_bool_ex(arg, key, strlen(key), b);
}
static zend_always_inline void add_property_resource(zval *arg, const char *key, zend_resource *r) {
	add_property_resource_ex(arg, key, strlen(key), r);
}
static zend_always_inline void add_property_double(zval *arg, const char *key, double d) {
	add_property_double_ex(arg, key, strlen(key), d);
}
static zend_always_inline void add_property_str(zval *arg, const char *key, zend_string *str) {
	add_property_str_ex(arg, key, strlen(key), str);
}
static zend_always_inline void add_property_string(zval *arg, const char *key, const char *str) {
	add_property_string_ex(arg, key, strlen(key), str);
}
static zend_always_inline void add_property_stringl(zval *arg, const char *key, const char *str, size_t length) {
	add_property_stringl_ex(arg, key, strlen(key), str, length);
}
static zend_always_inline void add_property_array(zval *arg, const char *key, zend_array *arr) {
	add_property_array_ex(arg, key, strlen(key), arr);
}
static zend_always_inline void add_property_object(zval *arg, const char *key, zend_object *obj) {
	add_property_object_ex(arg, key, strlen(key), obj);
}
static zend_always_inline void add_property_reference(zval *arg, const char *key, zend_reference *ref) {
	add_property_reference_ex(arg, key, strlen(key), ref);
}
static zend_always_inline void add_property_zval(zval *arg, const char *key, zval *value) {
	add_property_zval_ex(arg, key, strlen(key), value);
}

ZEND_API zend_result _call_user_function_impl(zval *object, zval *function_name, zval *retval_ptr, uint32_t param_count, zval params[], HashTable *named_params);

#define call_user_function(function_table, object, function_name, retval_ptr, param_count, params) \
	_call_user_function_impl(object, function_name, retval_ptr, param_count, params, NULL)

#define call_user_function_named(function_table, object, function_name, retval_ptr, param_count, params, named_params) \
	_call_user_function_impl(object, function_name, retval_ptr, param_count, params, named_params)

ZEND_API extern const zend_fcall_info empty_fcall_info;
ZEND_API extern const zend_fcall_info_cache empty_fcall_info_cache;

/** Build zend_call_info/cache from a zval*
 *
 * Caller is responsible to provide a return value (fci->retval), otherwise the we will crash.
 * In order to pass parameters the following members need to be set:
 * fci->param_count = 0;
 * fci->params = NULL;
 * The callable_name argument may be NULL.
 */
ZEND_API zend_result zend_fcall_info_init(zval *callable, uint32_t check_flags, zend_fcall_info *fci, zend_fcall_info_cache *fcc, zend_string **callable_name, char **error);

/** Clear arguments connected with zend_fcall_info *fci
 * If free_mem is not zero then the params array gets free'd as well
 */
ZEND_API void zend_fcall_info_args_clear(zend_fcall_info *fci, bool free_mem);

/** Save current arguments from zend_fcall_info *fci
 * params array will be set to NULL
 */
ZEND_API void zend_fcall_info_args_save(zend_fcall_info *fci, uint32_t *param_count, zval **params);

/** Free arguments connected with zend_fcall_info *fci and set back saved ones.
 */
ZEND_API void zend_fcall_info_args_restore(zend_fcall_info *fci, uint32_t param_count, zval *params);

/** Set or clear the arguments in the zend_call_info struct taking care of
 * refcount. If args is NULL and arguments are set then those are cleared.
 */
ZEND_API zend_result zend_fcall_info_args(zend_fcall_info *fci, zval *args);
ZEND_API zend_result zend_fcall_info_args_ex(zend_fcall_info *fci, zend_function *func, zval *args);

/** Set arguments in the zend_fcall_info struct taking care of refcount.
 * If argc is 0 the arguments which are set will be cleared, else pass
 * a variable amount of zval** arguments.
 */
ZEND_API void zend_fcall_info_argp(zend_fcall_info *fci, uint32_t argc, zval *argv);

/** Set arguments in the zend_fcall_info struct taking care of refcount.
 * If argc is 0 the arguments which are set will be cleared, else pass
 * a variable amount of zval** arguments.
 */
ZEND_API void zend_fcall_info_argv(zend_fcall_info *fci, uint32_t argc, va_list *argv);

/** Set arguments in the zend_fcall_info struct taking care of refcount.
 * If argc is 0 the arguments which are set will be cleared, else pass
 * a variable amount of zval** arguments.
 */
ZEND_API void zend_fcall_info_argn(zend_fcall_info *fci, uint32_t argc, ...);

/** Call a function using information created by zend_fcall_info_init()/args().
 * If args is given then those replace the argument info in fci is temporarily.
 */
ZEND_API zend_result zend_fcall_info_call(zend_fcall_info *fci, zend_fcall_info_cache *fcc, zval *retval, zval *args);

/* Zend FCC API to store and handle PHP userland functions */
static zend_always_inline bool zend_fcc_equals(const zend_fcall_info_cache* a, const zend_fcall_info_cache* b)
{
	if (UNEXPECTED((a->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) &&
		(b->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE))) {
		return a->object == b->object
			&& a->calling_scope == b->calling_scope
			&& a->closure == b->closure
			&& zend_string_equals(a->function_handler->common.function_name, b->function_handler->common.function_name)
		;
	}
	return a->function_handler == b->function_handler
		&& a->object == b->object
		&& a->calling_scope == b->calling_scope
		&& a->closure == b->closure
	;
}

static zend_always_inline void zend_fcc_addref(zend_fcall_info_cache *fcc)
{
	ZEND_ASSERT(ZEND_FCC_INITIALIZED(*fcc) && "FCC Not initialized, possibly refetch trampoline freed by ZPP?");
	/* If the cached trampoline is set, free it */
	if (UNEXPECTED(fcc->function_handler == &EG(trampoline))) {
		zend_function *copy = (zend_function*)emalloc(sizeof(zend_function));

		memcpy(copy, fcc->function_handler, sizeof(zend_function));
		fcc->function_handler->common.function_name = NULL;
		fcc->function_handler = copy;
	}
	if (fcc->object) {
		GC_ADDREF(fcc->object);
	}
	if (fcc->closure) {
		GC_ADDREF(fcc->closure);
	}
}

static zend_always_inline void zend_fcc_dup(/* restrict */ zend_fcall_info_cache *dest, const zend_fcall_info_cache *src)
{
	memcpy(dest, src, sizeof(zend_fcall_info_cache));
	zend_fcc_addref(dest);
}

static zend_always_inline void zend_fcc_dtor(zend_fcall_info_cache *fcc)
{
	ZEND_ASSERT(fcc->function_handler);
	if (fcc->object) {
		OBJ_RELEASE(fcc->object);
	}
	/* Need to free potential trampoline (__call/__callStatic) copied function handler before releasing the closure */
	zend_release_fcall_info_cache(fcc);
	if (fcc->closure) {
		OBJ_RELEASE(fcc->closure);
	}
	memcpy(fcc, &empty_fcall_info_cache, sizeof(zend_fcall_info_cache));
}

ZEND_API void zend_get_callable_zval_from_fcc(const zend_fcall_info_cache *fcc, zval *callable);

/* Moved out of zend_gc.h because zend_fcall_info_cache is an unknown type in that header */
static zend_always_inline void zend_get_gc_buffer_add_fcc(zend_get_gc_buffer *gc_buffer, zend_fcall_info_cache *fcc)
{
	ZEND_ASSERT(ZEND_FCC_INITIALIZED(*fcc));
	if (fcc->object) {
		zend_get_gc_buffer_add_obj(gc_buffer, fcc->object);
	}
	if (fcc->closure) {
		zend_get_gc_buffer_add_obj(gc_buffer, fcc->closure);
	}
}

/* Can only return FAILURE if EG(active) is false during late engine shutdown.
 * If the call or call setup throws, EG(exception) will be set and the retval
 * will be UNDEF. Otherwise, the retval will be a non-UNDEF value. */
ZEND_API zend_result zend_call_function(zend_fcall_info *fci, zend_fcall_info_cache *fci_cache);

/* Call the FCI/FCC pair while setting the call return value to the passed zval*. */
static zend_always_inline zend_result zend_call_function_with_return_value(
	zend_fcall_info *fci, zend_fcall_info_cache *fci_cache, zval *retval)
{
	ZEND_ASSERT(retval && "Use zend_call_function() directly if not providing a retval");
	fci->retval = retval;
	return zend_call_function(fci, fci_cache);
}

/* Call the provided zend_function with the given params.
 * If retval_ptr is NULL, the return value is discarded.
 * If object is NULL, this must be a free function or static call.
 * called_scope must be provided for instance and static method calls. */
ZEND_API void zend_call_known_function(
		zend_function *fn, zend_object *object, zend_class_entry *called_scope, zval *retval_ptr,
		uint32_t param_count, zval *params, HashTable *named_params);

static zend_always_inline void zend_call_known_fcc(
	zend_fcall_info_cache *fcc, zval *retval_ptr, uint32_t param_count, zval *params, HashTable *named_params)
{
	zend_function *func = fcc->function_handler;
	/* Need to copy trampolines as they get released after they are called */
	if (UNEXPECTED(func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
		func = (zend_function*) emalloc(sizeof(zend_function));
		memcpy(func, fcc->function_handler, sizeof(zend_function));
		zend_string_addref(func->op_array.function_name);
	}
	zend_call_known_function(func, fcc->object, fcc->called_scope, retval_ptr, param_count, params, named_params);
}

/* Call the provided zend_function instance method on an object. */
static zend_always_inline void zend_call_known_instance_method(
		zend_function *fn, zend_object *object, zval *retval_ptr,
		uint32_t param_count, zval *params)
{
	zend_call_known_function(fn, object, object->ce, retval_ptr, param_count, params, NULL);
}

static zend_always_inline void zend_call_known_instance_method_with_0_params(
		zend_function *fn, zend_object *object, zval *retval_ptr)
{
	zend_call_known_instance_method(fn, object, retval_ptr, 0, NULL);
}

static zend_always_inline void zend_call_known_instance_method_with_1_params(
		zend_function *fn, zend_object *object, zval *retval_ptr, zval *param)
{
	zend_call_known_instance_method(fn, object, retval_ptr, 1, param);
}

ZEND_API void zend_call_known_instance_method_with_2_params(
		zend_function *fn, zend_object *object, zval *retval_ptr, zval *param1, zval *param2);

/* Call method if it exists. Return FAILURE if method does not exist or call failed.
 * If FAILURE is returned, retval will be UNDEF. As such, destroying retval unconditionally
 * is legal. */
ZEND_API zend_result zend_call_method_if_exists(
		zend_object *object, zend_string *method_name, zval *retval,
		uint32_t param_count, zval *params);

ZEND_API zend_result zend_set_hash_symbol(zval *symbol, const char *name, size_t name_length, bool is_ref, int num_symbol_tables, ...);

ZEND_API zend_result zend_delete_global_variable(zend_string *name);

ZEND_API zend_array *zend_rebuild_symbol_table(void);
ZEND_API void zend_attach_symbol_table(zend_execute_data *execute_data);
ZEND_API void zend_detach_symbol_table(zend_execute_data *execute_data);
ZEND_API zend_result zend_set_local_var(zend_string *name, zval *value, bool force);
ZEND_API zend_result zend_set_local_var_str(const char *name, size_t len, zval *value, bool force);

static zend_always_inline zend_result zend_forbid_dynamic_call(void)
{
	zend_execute_data *ex = EG(current_execute_data);
	ZEND_ASSERT(ex != NULL && ex->func != NULL);

	if (ZEND_CALL_INFO(ex) & ZEND_CALL_DYNAMIC) {
		zend_string *function_or_method_name = get_active_function_or_method_name();
		zend_throw_error(NULL, "Cannot call %.*s() dynamically",
			(int) ZSTR_LEN(function_or_method_name), ZSTR_VAL(function_or_method_name));
		zend_string_release(function_or_method_name);
		return FAILURE;
	}

	return SUCCESS;
}

ZEND_API ZEND_COLD const char *zend_get_object_type_case(const zend_class_entry *ce, bool upper_case);

static zend_always_inline const char *zend_get_object_type(const zend_class_entry *ce)
{
	return zend_get_object_type_case(ce, false);
}

static zend_always_inline const char *zend_get_object_type_uc(const zend_class_entry *ce)
{
	return zend_get_object_type_case(ce, true);
}

ZEND_API bool zend_is_iterable(const zval *iterable);

ZEND_API bool zend_is_countable(const zval *countable);

ZEND_API zend_result zend_get_default_from_internal_arg_info(
		zval *default_value_zval, zend_internal_arg_info *arg_info);

END_EXTERN_C()

#if ZEND_DEBUG
#define CHECK_ZVAL_STRING(str) \
	ZEND_ASSERT(ZSTR_VAL(str)[ZSTR_LEN(str)] == '\0' && "String is not null-terminated");
#else
#define CHECK_ZVAL_STRING(z)
#endif

static zend_always_inline bool zend_str_has_nul_byte(const zend_string *str)
{
	return ZSTR_LEN(str) != strlen(ZSTR_VAL(str));
}
static zend_always_inline bool zend_char_has_nul_byte(const char *s, size_t known_length)
{
	return known_length != strlen(s);
}

/* Compatibility with PHP 8.1 and below */
#define CHECK_ZVAL_NULL_PATH(p) zend_str_has_nul_byte(Z_STR_P(p))
#define CHECK_NULL_PATH(p, l) zend_char_has_nul_byte(p, l)

#define ZVAL_STRINGL(z, s, l) do {				\
		ZVAL_NEW_STR(z, zend_string_init(s, l, 0));		\
	} while (0)

#define ZVAL_STRING(z, s) do {					\
		const char *_s = (s);					\
		ZVAL_STRINGL(z, _s, strlen(_s));		\
	} while (0)

#define ZVAL_EMPTY_STRING(z) do {				\
		ZVAL_INTERNED_STR(z, ZSTR_EMPTY_ALLOC());		\
	} while (0)

#define ZVAL_PSTRINGL(z, s, l) do {				\
		ZVAL_NEW_STR(z, zend_string_init(s, l, 1));		\
	} while (0)

#define ZVAL_PSTRING(z, s) do {					\
		const char *_s = (s);					\
		ZVAL_PSTRINGL(z, _s, strlen(_s));		\
	} while (0)

#define ZVAL_EMPTY_PSTRING(z) do {				\
		ZVAL_PSTRINGL(z, "", 0);				\
	} while (0)

#define ZVAL_CHAR(z, c)  do {		            \
		char _c = (c);                          \
		ZVAL_INTERNED_STR(z, ZSTR_CHAR((zend_uchar) _c));	\
	} while (0)

#define ZVAL_STRINGL_FAST(z, s, l) do {			\
		ZVAL_STR(z, zend_string_init_fast(s, l));	\
	} while (0)

#define ZVAL_STRING_FAST(z, s) do {				\
		const char *_s = (s);					\
		ZVAL_STRINGL_FAST(z, _s, strlen(_s));	\
	} while (0)

#define ZVAL_ZVAL(z, zv, copy, dtor) do {		\
		zval *__z = (z);						\
		zval *__zv = (zv);						\
		if (EXPECTED(!Z_ISREF_P(__zv))) {		\
			if (copy && !dtor) {				\
				ZVAL_COPY(__z, __zv);			\
			} else {							\
				ZVAL_COPY_VALUE(__z, __zv);		\
			}									\
		} else {								\
			ZVAL_COPY(__z, Z_REFVAL_P(__zv));	\
			if (dtor || !copy) {				\
				zval_ptr_dtor(__zv);			\
			}									\
		}										\
	} while (0)

#define RETVAL_BOOL(b)					ZVAL_BOOL(return_value, b)
#define RETVAL_NULL()					ZVAL_NULL(return_value)
#define RETVAL_LONG(l)					ZVAL_LONG(return_value, l)
#define RETVAL_DOUBLE(d)				ZVAL_DOUBLE(return_value, d)
#define RETVAL_STR(s)					ZVAL_STR(return_value, s)
#define RETVAL_INTERNED_STR(s)			ZVAL_INTERNED_STR(return_value, s)
#define RETVAL_NEW_STR(s)				ZVAL_NEW_STR(return_value, s)
#define RETVAL_STR_COPY(s)				ZVAL_STR_COPY(return_value, s)
#define RETVAL_STRING(s)				ZVAL_STRING(return_value, s)
#define RETVAL_STRINGL(s, l)			ZVAL_STRINGL(return_value, s, l)
#define RETVAL_STRING_FAST(s)			ZVAL_STRING_FAST(return_value, s)
#define RETVAL_STRINGL_FAST(s, l)		ZVAL_STRINGL_FAST(return_value, s, l)
#define RETVAL_EMPTY_STRING()			ZVAL_EMPTY_STRING(return_value)
#define RETVAL_CHAR(c)		            ZVAL_CHAR(return_value, c)
#define RETVAL_RES(r)					ZVAL_RES(return_value, r)
#define RETVAL_ARR(r)					ZVAL_ARR(return_value, r)
#define RETVAL_EMPTY_ARRAY()			ZVAL_EMPTY_ARRAY(return_value)
#define RETVAL_OBJ(r)					ZVAL_OBJ(return_value, r)
#define RETVAL_OBJ_COPY(r)				ZVAL_OBJ_COPY(return_value, r)
#define RETVAL_COPY(zv)					ZVAL_COPY(return_value, zv)
#define RETVAL_COPY_VALUE(zv)			ZVAL_COPY_VALUE(return_value, zv)
#define RETVAL_COPY_DEREF(zv)			ZVAL_COPY_DEREF(return_value, zv)
#define RETVAL_ZVAL(zv, copy, dtor)		ZVAL_ZVAL(return_value, zv, copy, dtor)
#define RETVAL_FALSE					ZVAL_FALSE(return_value)
#define RETVAL_TRUE						ZVAL_TRUE(return_value)

#define RETURN_BOOL(b)					do { RETVAL_BOOL(b); return; } while (0)
#define RETURN_NULL()					do { RETVAL_NULL(); return;} while (0)
#define RETURN_LONG(l)					do { RETVAL_LONG(l); return; } while (0)
#define RETURN_DOUBLE(d)				do { RETVAL_DOUBLE(d); return; } while (0)
#define RETURN_STR(s) 					do { RETVAL_STR(s); return; } while (0)
#define RETURN_INTERNED_STR(s)			do { RETVAL_INTERNED_STR(s); return; } while (0)
#define RETURN_NEW_STR(s)				do { RETVAL_NEW_STR(s); return; } while (0)
#define RETURN_STR_COPY(s)				do { RETVAL_STR_COPY(s); return; } while (0)
#define RETURN_STRING(s) 				do { RETVAL_STRING(s); return; } while (0)
#define RETURN_STRINGL(s, l) 			do { RETVAL_STRINGL(s, l); return; } while (0)
#define RETURN_STRING_FAST(s) 			do { RETVAL_STRING_FAST(s); return; } while (0)
#define RETURN_STRINGL_FAST(s, l)		do { RETVAL_STRINGL_FAST(s, l); return; } while (0)
#define RETURN_EMPTY_STRING() 			do { RETVAL_EMPTY_STRING(); return; } while (0)
#define RETURN_CHAR(c)		            do { RETVAL_CHAR(c); return; } while (0)
#define RETURN_RES(r)					do { RETVAL_RES(r); return; } while (0)
#define RETURN_ARR(r)					do { RETVAL_ARR(r); return; } while (0)
#define RETURN_EMPTY_ARRAY()			do { RETVAL_EMPTY_ARRAY(); return; } while (0)
#define RETURN_OBJ(r)					do { RETVAL_OBJ(r); return; } while (0)
#define RETURN_OBJ_COPY(r)				do { RETVAL_OBJ_COPY(r); return; } while (0)
#define RETURN_COPY(zv)					do { RETVAL_COPY(zv); return; } while (0)
#define RETURN_COPY_VALUE(zv)			do { RETVAL_COPY_VALUE(zv); return; } while (0)
#define RETURN_COPY_DEREF(zv)			do { RETVAL_COPY_DEREF(zv); return; } while (0)
#define RETURN_ZVAL(zv, copy, dtor)		do { RETVAL_ZVAL(zv, copy, dtor); return; } while (0)
#define RETURN_FALSE					do { RETVAL_FALSE; return; } while (0)
#define RETURN_TRUE						do { RETVAL_TRUE; return; } while (0)
#define RETURN_THROWS()					do { ZEND_ASSERT(EG(exception)); (void) return_value; return; } while (0)

#define HASH_OF(p) (Z_TYPE_P(p)==IS_ARRAY ? Z_ARRVAL_P(p) : ((Z_TYPE_P(p)==IS_OBJECT ? Z_OBJ_HT_P(p)->get_properties(Z_OBJ_P(p)) : NULL)))
#define ZVAL_IS_NULL(z) (Z_TYPE_P(z) == IS_NULL)

/* For compatibility */
#define ZEND_MINIT			ZEND_MODULE_STARTUP_N
#define ZEND_MSHUTDOWN		ZEND_MODULE_SHUTDOWN_N
#define ZEND_RINIT			ZEND_MODULE_ACTIVATE_N
#define ZEND_RSHUTDOWN		ZEND_MODULE_DEACTIVATE_N
#define ZEND_MINFO			ZEND_MODULE_INFO_N
#define ZEND_GINIT(module)		((void (*)(void*))(ZEND_MODULE_GLOBALS_CTOR_N(module)))
#define ZEND_GSHUTDOWN(module)	((void (*)(void*))(ZEND_MODULE_GLOBALS_DTOR_N(module)))

#define ZEND_MINIT_FUNCTION			ZEND_MODULE_STARTUP_D
#define ZEND_MSHUTDOWN_FUNCTION		ZEND_MODULE_SHUTDOWN_D
#define ZEND_RINIT_FUNCTION			ZEND_MODULE_ACTIVATE_D
#define ZEND_RSHUTDOWN_FUNCTION		ZEND_MODULE_DEACTIVATE_D
#define ZEND_MINFO_FUNCTION			ZEND_MODULE_INFO_D
#define ZEND_GINIT_FUNCTION			ZEND_MODULE_GLOBALS_CTOR_D
#define ZEND_GSHUTDOWN_FUNCTION		ZEND_MODULE_GLOBALS_DTOR_D

/* May modify arg in-place. Will free arg in failure case (and take ownership in success case).
 * Prefer using the ZEND_TRY_ASSIGN_* macros over these APIs. */
ZEND_API zend_result zend_try_assign_typed_ref_ex(zend_reference *ref, zval *zv, bool strict);
ZEND_API zend_result zend_try_assign_typed_ref(zend_reference *ref, zval *zv);

ZEND_API zend_result zend_try_assign_typed_ref_null(zend_reference *ref);
ZEND_API zend_result zend_try_assign_typed_ref_bool(zend_reference *ref, bool val);
ZEND_API zend_result zend_try_assign_typed_ref_long(zend_reference *ref, zend_long lval);
ZEND_API zend_result zend_try_assign_typed_ref_double(zend_reference *ref, double dval);
ZEND_API zend_result zend_try_assign_typed_ref_empty_string(zend_reference *ref);
ZEND_API zend_result zend_try_assign_typed_ref_str(zend_reference *ref, zend_string *str);
ZEND_API zend_result zend_try_assign_typed_ref_string(zend_reference *ref, const char *string);
ZEND_API zend_result zend_try_assign_typed_ref_stringl(zend_reference *ref, const char *string, size_t len);
ZEND_API zend_result zend_try_assign_typed_ref_arr(zend_reference *ref, zend_array *arr);
ZEND_API zend_result zend_try_assign_typed_ref_res(zend_reference *ref, zend_resource *res);
ZEND_API zend_result zend_try_assign_typed_ref_zval(zend_reference *ref, zval *zv);
ZEND_API zend_result zend_try_assign_typed_ref_zval_ex(zend_reference *ref, zval *zv, bool strict);

#define _ZEND_TRY_ASSIGN_NULL(zv, is_ref) do { \
	zval *_zv = zv; \
	if (is_ref || UNEXPECTED(Z_ISREF_P(_zv))) { \
		zend_reference *ref = Z_REF_P(_zv); \
		if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) { \
			zend_try_assign_typed_ref_null(ref); \
			break; \
		} \
		_zv = &ref->val; \
	} \
	zval_ptr_dtor(_zv); \
	ZVAL_NULL(_zv); \
} while (0)

#define ZEND_TRY_ASSIGN_NULL(zv) \
	_ZEND_TRY_ASSIGN_NULL(zv, 0)

#define ZEND_TRY_ASSIGN_REF_NULL(zv) do { \
	ZEND_ASSERT(Z_ISREF_P(zv)); \
	_ZEND_TRY_ASSIGN_NULL(zv, 1); \
} while (0)

#define _ZEND_TRY_ASSIGN_FALSE(zv, is_ref) do { \
	zval *_zv = zv; \
	if (is_ref || UNEXPECTED(Z_ISREF_P(_zv))) { \
		zend_reference *ref = Z_REF_P(_zv); \
		if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) { \
			zend_try_assign_typed_ref_bool(ref, 0); \
			break; \
		} \
		_zv = &ref->val; \
	} \
	zval_ptr_dtor(_zv); \
	ZVAL_FALSE(_zv); \
} while (0)

#define ZEND_TRY_ASSIGN_FALSE(zv) \
	_ZEND_TRY_ASSIGN_FALSE(zv, 0)

#define ZEND_TRY_ASSIGN_REF_FALSE(zv) do { \
	ZEND_ASSERT(Z_ISREF_P(zv)); \
	_ZEND_TRY_ASSIGN_FALSE(zv, 1); \
} while (0)

#define _ZEND_TRY_ASSIGN_TRUE(zv, is_ref) do { \
	zval *_zv = zv; \
	if (is_ref || UNEXPECTED(Z_ISREF_P(_zv))) { \
		zend_reference *ref = Z_REF_P(_zv); \
		if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) { \
			zend_try_assign_typed_ref_bool(ref, 1); \
			break; \
		} \
		_zv = &ref->val; \
	} \
	zval_ptr_dtor(_zv); \
	ZVAL_TRUE(_zv); \
} while (0)

#define ZEND_TRY_ASSIGN_TRUE(zv) \
	_ZEND_TRY_ASSIGN_TRUE(zv, 0)

#define ZEND_TRY_ASSIGN_REF_TRUE(zv) do { \
	ZEND_ASSERT(Z_ISREF_P(zv)); \
	_ZEND_TRY_ASSIGN_TRUE(zv, 1); \
} while (0)

#define _ZEND_TRY_ASSIGN_BOOL(zv, bval, is_ref) do { \
	zval *_zv = zv; \
	if (is_ref || UNEXPECTED(Z_ISREF_P(_zv))) { \
		zend_reference *ref = Z_REF_P(_zv); \
		if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) { \
			zend_try_assign_typed_ref_bool(ref, 1); \
			break; \
		} \
		_zv = &ref->val; \
	} \
	zval_ptr_dtor(_zv); \
	ZVAL_BOOL(_zv, bval); \
} while (0)

#define ZEND_TRY_ASSIGN_BOOL(zv, bval) \
	_ZEND_TRY_ASSIGN_BOOL(zv, bval, 0)

#define ZEND_TRY_ASSIGN_REF_BOOL(zv, bval) do { \
	ZEND_ASSERT(Z_ISREF_P(zv)); \
	_ZEND_TRY_ASSIGN_BOOL(zv, bval, 1); \
} while (0)

#define _ZEND_TRY_ASSIGN_LONG(zv, lval, is_ref) do { \
	zval *_zv = zv; \
	if (is_ref || UNEXPECTED(Z_ISREF_P(_zv))) { \
		zend_reference *ref = Z_REF_P(_zv); \
		if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) { \
			zend_try_assign_typed_ref_long(ref, lval); \
			break; \
		} \
		_zv = &ref->val; \
	} \
	zval_ptr_dtor(_zv); \
	ZVAL_LONG(_zv, lval); \
} while (0)

#define ZEND_TRY_ASSIGN_LONG(zv, lval) \
	_ZEND_TRY_ASSIGN_LONG(zv, lval, 0)

#define ZEND_TRY_ASSIGN_REF_LONG(zv, lval) do { \
	ZEND_ASSERT(Z_ISREF_P(zv)); \
	_ZEND_TRY_ASSIGN_LONG(zv, lval, 1); \
} while (0)

#define _ZEND_TRY_ASSIGN_DOUBLE(zv, dval, is_ref) do { \
	zval *_zv = zv; \
	if (is_ref || UNEXPECTED(Z_ISREF_P(_zv))) { \
		zend_reference *ref = Z_REF_P(_zv); \
		if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) { \
			zend_try_assign_typed_ref_double(ref, dval); \
			break; \
		} \
		_zv = &ref->val; \
	} \
	zval_ptr_dtor(_zv); \
	ZVAL_DOUBLE(_zv, dval); \
} while (0)

#define ZEND_TRY_ASSIGN_DOUBLE(zv, dval) \
	_ZEND_TRY_ASSIGN_DOUBLE(zv, dval, 0)

#define ZEND_TRY_ASSIGN_REF_DOUBLE(zv, dval) do { \
	ZEND_ASSERT(Z_ISREF_P(zv)); \
	_ZEND_TRY_ASSIGN_DOUBLE(zv, dval, 1); \
} while (0)

#define _ZEND_TRY_ASSIGN_EMPTY_STRING(zv, is_ref) do { \
	zval *_zv = zv; \
	if (is_ref || UNEXPECTED(Z_ISREF_P(_zv))) { \
		zend_reference *ref = Z_REF_P(_zv); \
		if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) { \
			zend_try_assign_typed_ref_empty_string(ref); \
			break; \
		} \
		_zv = &ref->val; \
	} \
	zval_ptr_dtor(_zv); \
	ZVAL_EMPTY_STRING(_zv); \
} while (0)

#define ZEND_TRY_ASSIGN_EMPTY_STRING(zv) \
	_ZEND_TRY_ASSIGN_EMPTY_STRING(zv, 0)

#define ZEND_TRY_ASSIGN_REF_EMPTY_STRING(zv) do { \
	ZEND_ASSERT(Z_ISREF_P(zv)); \
	_ZEND_TRY_ASSIGN_EMPTY_STRING(zv, 1); \
} while (0)

#define _ZEND_TRY_ASSIGN_STR(zv, str, is_ref) do { \
	zval *_zv = zv; \
	if (is_ref || UNEXPECTED(Z_ISREF_P(_zv))) { \
		zend_reference *ref = Z_REF_P(_zv); \
		if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) { \
			zend_try_assign_typed_ref_str(ref, str); \
			break; \
		} \
		_zv = &ref->val; \
	} \
	zval_ptr_dtor(_zv); \
	ZVAL_STR(_zv, str); \
} while (0)

#define ZEND_TRY_ASSIGN_STR(zv, str) \
	_ZEND_TRY_ASSIGN_STR(zv, str, 0)

#define ZEND_TRY_ASSIGN_REF_STR(zv, str) do { \
	ZEND_ASSERT(Z_ISREF_P(zv)); \
	_ZEND_TRY_ASSIGN_STR(zv, str, 1); \
} while (0)

#define _ZEND_TRY_ASSIGN_NEW_STR(zv, str, is_str) do { \
	zval *_zv = zv; \
	if (is_str || UNEXPECTED(Z_ISREF_P(_zv))) { \
		zend_reference *ref = Z_REF_P(_zv); \
		if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) { \
			zend_try_assign_typed_ref_str(ref, str); \
			break; \
		} \
		_zv = &ref->val; \
	} \
	zval_ptr_dtor(_zv); \
	ZVAL_NEW_STR(_zv, str); \
} while (0)

#define ZEND_TRY_ASSIGN_NEW_STR(zv, str) \
	_ZEND_TRY_ASSIGN_NEW_STR(zv, str, 0)

#define ZEND_TRY_ASSIGN_REF_NEW_STR(zv, str) do { \
	ZEND_ASSERT(Z_ISREF_P(zv)); \
	_ZEND_TRY_ASSIGN_NEW_STR(zv, str, 1); \
} while (0)

#define _ZEND_TRY_ASSIGN_STRING(zv, string, is_ref) do { \
	zval *_zv = zv; \
	if (is_ref || UNEXPECTED(Z_ISREF_P(_zv))) { \
		zend_reference *ref = Z_REF_P(_zv); \
		if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) { \
			zend_try_assign_typed_ref_string(ref, string); \
			break; \
		} \
		_zv = &ref->val; \
	} \
	zval_ptr_dtor(_zv); \
	ZVAL_STRING(_zv, string); \
} while (0)

#define ZEND_TRY_ASSIGN_STRING(zv, string) \
	_ZEND_TRY_ASSIGN_STRING(zv, string, 0)

#define ZEND_TRY_ASSIGN_REF_STRING(zv, string) do { \
	ZEND_ASSERT(Z_ISREF_P(zv)); \
	_ZEND_TRY_ASSIGN_STRING(zv, string, 1); \
} while (0)

#define _ZEND_TRY_ASSIGN_STRINGL(zv, string, len, is_ref) do { \
	zval *_zv = zv; \
	if (is_ref || UNEXPECTED(Z_ISREF_P(_zv))) { \
		zend_reference *ref = Z_REF_P(_zv); \
		if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) { \
			zend_try_assign_typed_ref_stringl(ref, string, len); \
			break; \
		} \
		_zv = &ref->val; \
	} \
	zval_ptr_dtor(_zv); \
	ZVAL_STRINGL(_zv, string, len); \
} while (0)

#define ZEND_TRY_ASSIGN_STRINGL(zv, string, len) \
	_ZEND_TRY_ASSIGN_STRINGL(zv, string, len, 0)

#define ZEND_TRY_ASSIGN_REF_STRINGL(zv, string, len) do { \
	ZEND_ASSERT(Z_ISREF_P(zv)); \
	_ZEND_TRY_ASSIGN_STRINGL(zv, string, len, 1); \
} while (0)

#define _ZEND_TRY_ASSIGN_ARR(zv, arr, is_ref) do { \
	zval *_zv = zv; \
	if (is_ref || UNEXPECTED(Z_ISREF_P(_zv))) { \
		zend_reference *ref = Z_REF_P(_zv); \
		if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) { \
			zend_try_assign_typed_ref_arr(ref, arr); \
			break; \
		} \
		_zv = &ref->val; \
	} \
	zval_ptr_dtor(_zv); \
	ZVAL_ARR(_zv, arr); \
} while (0)

#define ZEND_TRY_ASSIGN_ARR(zv, arr) \
	_ZEND_TRY_ASSIGN_ARR(zv, arr, 0)

#define ZEND_TRY_ASSIGN_REF_ARR(zv, arr) do { \
	ZEND_ASSERT(Z_ISREF_P(zv)); \
	_ZEND_TRY_ASSIGN_ARR(zv, arr, 1); \
} while (0)

#define _ZEND_TRY_ASSIGN_RES(zv, res, is_ref) do { \
	zval *_zv = zv; \
	if (is_ref || UNEXPECTED(Z_ISREF_P(_zv))) { \
		zend_reference *ref = Z_REF_P(_zv); \
		if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) { \
			zend_try_assign_typed_ref_res(ref, res); \
			break; \
		} \
		_zv = &ref->val; \
	} \
	zval_ptr_dtor(_zv); \
	ZVAL_RES(_zv, res); \
} while (0)

#define ZEND_TRY_ASSIGN_RES(zv, res) \
	_ZEND_TRY_ASSIGN_RES(zv, res, 0)

#define ZEND_TRY_ASSIGN_REF_RES(zv, res) do { \
	ZEND_ASSERT(Z_ISREF_P(zv)); \
	_ZEND_TRY_ASSIGN_RES(zv, res, 1); \
} while (0)

#define _ZEND_TRY_ASSIGN_TMP(zv, other_zv, is_ref) do { \
	zval *_zv = zv; \
	if (is_ref || UNEXPECTED(Z_ISREF_P(_zv))) { \
		zend_reference *ref = Z_REF_P(_zv); \
		if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) { \
			zend_try_assign_typed_ref(ref, other_zv); \
			break; \
		} \
		_zv = &ref->val; \
	} \
	zval_ptr_dtor(_zv); \
	ZVAL_COPY_VALUE(_zv, other_zv); \
} while (0)

#define ZEND_TRY_ASSIGN_TMP(zv, other_zv) \
	_ZEND_TRY_ASSIGN_TMP(zv, other_zv, 0)

#define ZEND_TRY_ASSIGN_REF_TMP(zv, other_zv) do { \
	ZEND_ASSERT(Z_ISREF_P(zv)); \
	_ZEND_TRY_ASSIGN_TMP(zv, other_zv, 1); \
} while (0)

#define _ZEND_TRY_ASSIGN_VALUE(zv, other_zv, is_ref) do { \
	zval *_zv = zv; \
	if (is_ref || UNEXPECTED(Z_ISREF_P(_zv))) { \
		zend_reference *ref = Z_REF_P(_zv); \
		if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) { \
			zend_try_assign_typed_ref_zval(ref, other_zv); \
			break; \
		} \
		_zv = &ref->val; \
	} \
	zval_ptr_dtor(_zv); \
	ZVAL_COPY_VALUE(_zv, other_zv); \
} while (0)

#define ZEND_TRY_ASSIGN_VALUE(zv, other_zv) \
	_ZEND_TRY_ASSIGN_VALUE(zv, other_zv, 0)

#define ZEND_TRY_ASSIGN_REF_VALUE(zv, other_zv) do { \
	ZEND_ASSERT(Z_ISREF_P(zv)); \
	_ZEND_TRY_ASSIGN_VALUE(zv, other_zv, 1); \
} while (0)

#define ZEND_TRY_ASSIGN_COPY(zv, other_zv) do { \
	Z_TRY_ADDREF_P(other_zv); \
	ZEND_TRY_ASSIGN_VALUE(zv, other_zv); \
} while (0)

#define ZEND_TRY_ASSIGN_REF_COPY(zv, other_zv) do { \
	Z_TRY_ADDREF_P(other_zv); \
	ZEND_TRY_ASSIGN_REF_VALUE(zv, other_zv); \
} while (0)

#define _ZEND_TRY_ASSIGN_VALUE_EX(zv, other_zv, strict, is_ref) do { \
	zval *_zv = zv; \
	if (is_ref || UNEXPECTED(Z_ISREF_P(_zv))) { \
		zend_reference *ref = Z_REF_P(_zv); \
		if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) { \
			zend_try_assign_typed_ref_zval_ex(ref, other_zv, strict); \
			break; \
		} \
		_zv = &ref->val; \
	} \
	zval_ptr_dtor(_zv); \
	ZVAL_COPY_VALUE(_zv, other_zv); \
} while (0)

#define ZEND_TRY_ASSIGN_VALUE_EX(zv, other_zv, strict) \
	_ZEND_TRY_ASSIGN_VALUE_EX(zv, other_zv, strict, 0)

#define ZEND_TRY_ASSIGN_REF_VALUE_EX(zv, other_zv, strict) do { \
	ZEND_ASSERT(Z_ISREF_P(zv)); \
	_ZEND_TRY_ASSIGN_VALUE_EX(zv, other_zv, strict, 1); \
} while (0)

#define ZEND_TRY_ASSIGN_COPY_EX(zv, other_zv, strict) do { \
	Z_TRY_ADDREF_P(other_zv); \
	ZEND_TRY_ASSIGN_VALUE_EX(zv, other_zv, strict); \
} while (0)

#define ZEND_TRY_ASSIGN_REF_COPY_EX(zv, other_zv, strict) do { \
	Z_TRY_ADDREF_P(other_zv); \
	ZEND_TRY_ASSIGN_REF_VALUE_EX(zv, other_zv, strict); \
} while (0)

/* Initializes a reference to an empty array and returns dereferenced zval,
 * or NULL if the initialization failed. */
static zend_always_inline zval *zend_try_array_init_size(zval *zv, uint32_t size)
{
	zend_array *arr = zend_new_array(size);

	if (EXPECTED(Z_ISREF_P(zv))) {
		zend_reference *ref = Z_REF_P(zv);
		if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) {
			if (zend_try_assign_typed_ref_arr(ref, arr) == FAILURE) {
				return NULL;
			}
			return &ref->val;
		}
		zv = &ref->val;
	}
	zval_ptr_dtor(zv);
	ZVAL_ARR(zv, arr);
	return zv;
}

static zend_always_inline zval *zend_try_array_init(zval *zv)
{
	return zend_try_array_init_size(zv, 0);
}

/* Fast parameter parsing API */

/* Fast ZPP is always enabled now; this define is left in for compatibility
 * with any existing conditional compilation blocks.
 */
#define FAST_ZPP 1

#define Z_EXPECTED_TYPES(_) \
	_(Z_EXPECTED_LONG,				"of type int") \
	_(Z_EXPECTED_LONG_OR_NULL,		"of type ?int") \
	_(Z_EXPECTED_BOOL,				"of type bool") \
	_(Z_EXPECTED_BOOL_OR_NULL,		"of type ?bool") \
	_(Z_EXPECTED_STRING,			"of type string") \
	_(Z_EXPECTED_STRING_OR_NULL,	"of type ?string") \
	_(Z_EXPECTED_ARRAY,				"of type array") \
	_(Z_EXPECTED_ARRAY_OR_NULL,		"of type ?array") \
	_(Z_EXPECTED_ARRAY_OR_LONG,		"of type array|int") \
	_(Z_EXPECTED_ARRAY_OR_LONG_OR_NULL, "of type array|int|null") \
	_(Z_EXPECTED_ITERABLE,				"of type Traversable|array") \
	_(Z_EXPECTED_ITERABLE_OR_NULL,		"of type Traversable|array|null") \
	_(Z_EXPECTED_FUNC,				"a valid callback") \
	_(Z_EXPECTED_FUNC_OR_NULL,		"a valid callback or null") \
	_(Z_EXPECTED_RESOURCE,			"of type resource") \
	_(Z_EXPECTED_RESOURCE_OR_NULL,	"of type resource or null") \
	_(Z_EXPECTED_PATH,				"of type string") \
	_(Z_EXPECTED_PATH_OR_NULL,		"of type ?string") \
	_(Z_EXPECTED_OBJECT,			"of type object") \
	_(Z_EXPECTED_OBJECT_OR_NULL,	"of type ?object") \
	_(Z_EXPECTED_DOUBLE,			"of type float") \
	_(Z_EXPECTED_DOUBLE_OR_NULL,	"of type ?float") \
	_(Z_EXPECTED_NUMBER,			"of type int|float") \
	_(Z_EXPECTED_NUMBER_OR_NULL,	"of type int|float|null") \
	_(Z_EXPECTED_NUMBER_OR_STRING,			"of type string|int|float") \
	_(Z_EXPECTED_NUMBER_OR_STRING_OR_NULL,	"of type string|int|float|null") \
	_(Z_EXPECTED_ARRAY_OR_STRING,	"of type array|string") \
	_(Z_EXPECTED_ARRAY_OR_STRING_OR_NULL, "of type array|string|null") \
	_(Z_EXPECTED_STRING_OR_LONG,	"of type string|int") \
	_(Z_EXPECTED_STRING_OR_LONG_OR_NULL, "of type string|int|null") \
	_(Z_EXPECTED_OBJECT_OR_CLASS_NAME,	"an object or a valid class name") \
	_(Z_EXPECTED_OBJECT_OR_CLASS_NAME_OR_NULL, "an object, a valid class name, or null") \
	_(Z_EXPECTED_OBJECT_OR_STRING,	"of type object|string") \
	_(Z_EXPECTED_OBJECT_OR_STRING_OR_NULL, "of type object|string|null") \

#define Z_EXPECTED_TYPE

#define Z_EXPECTED_TYPE_ENUM(id, str) id,
#define Z_EXPECTED_TYPE_STR(id, str)  str,

typedef enum _zend_expected_type {
	Z_EXPECTED_TYPES(Z_EXPECTED_TYPE_ENUM)
	Z_EXPECTED_LAST
} zend_expected_type;

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameters_none_error(void);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameters_count_error(uint32_t min_num_args, uint32_t max_num_args);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_error(int error_code, uint32_t num, char *name, zend_expected_type expected_type, zval *arg);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_type_error(uint32_t num, zend_expected_type expected_type, zval *arg);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_class_error(uint32_t num, const char *name, zval *arg);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_class_or_null_error(uint32_t num, const char *name, zval *arg);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_class_or_long_error(uint32_t num, const char *name, zval *arg);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_class_or_long_or_null_error(uint32_t num, const char *name, zval *arg);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_class_or_string_error(uint32_t num, const char *name, zval *arg);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_class_or_string_or_null_error(uint32_t num, const char *name, zval *arg);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_callback_error(uint32_t num, char *error);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_callback_or_null_error(uint32_t num, char *error);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_unexpected_extra_named_error(void);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_argument_error_variadic(zend_class_entry *error_ce, uint32_t arg_num, const char *format, va_list va);
ZEND_API ZEND_COLD void zend_argument_error(zend_class_entry *error_ce, uint32_t arg_num, const char *format, ...);
ZEND_API ZEND_COLD void zend_argument_type_error(uint32_t arg_num, const char *format, ...);
ZEND_API ZEND_COLD void zend_argument_value_error(uint32_t arg_num, const char *format, ...);

#define ZPP_ERROR_OK                            0
#define ZPP_ERROR_FAILURE                       1
#define ZPP_ERROR_WRONG_CALLBACK                2
#define ZPP_ERROR_WRONG_CLASS                   3
#define ZPP_ERROR_WRONG_CLASS_OR_NULL           4
#define ZPP_ERROR_WRONG_CLASS_OR_STRING         5
#define ZPP_ERROR_WRONG_CLASS_OR_STRING_OR_NULL 6
#define ZPP_ERROR_WRONG_CLASS_OR_LONG           7
#define ZPP_ERROR_WRONG_CLASS_OR_LONG_OR_NULL   8
#define ZPP_ERROR_WRONG_ARG                     9
#define ZPP_ERROR_WRONG_COUNT                   10
#define ZPP_ERROR_UNEXPECTED_EXTRA_NAMED        11
#define ZPP_ERROR_WRONG_CALLBACK_OR_NULL        12

#define ZEND_PARSE_PARAMETERS_START_EX(flags, min_num_args, max_num_args) do { \
		const int _flags = (flags); \
		uint32_t _min_num_args = (min_num_args); \
		uint32_t _max_num_args = (uint32_t) (max_num_args); \
		uint32_t _num_args = EX_NUM_ARGS(); \
		uint32_t _i = 0; \
		zval *_real_arg, *_arg = NULL; \
		zend_expected_type _expected_type = Z_EXPECTED_LONG; \
		char *_error = NULL; \
		bool _dummy = 0; \
		bool _optional = 0; \
		int _error_code = ZPP_ERROR_OK; \
		((void)_i); \
		((void)_real_arg); \
		((void)_arg); \
		((void)_expected_type); \
		((void)_error); \
		((void)_optional); \
		((void)_dummy); \
		\
		do { \
			if (UNEXPECTED(_num_args < _min_num_args) || \
			    UNEXPECTED(_num_args > _max_num_args)) { \
				if (!(_flags & ZEND_PARSE_PARAMS_QUIET)) { \
					zend_wrong_parameters_count_error(_min_num_args, _max_num_args); \
				} \
				_error_code = ZPP_ERROR_FAILURE; \
				break; \
			} \
			_real_arg = ZEND_CALL_ARG(execute_data, 0);

#define ZEND_PARSE_PARAMETERS_START(min_num_args, max_num_args) \
	ZEND_PARSE_PARAMETERS_START_EX(0, min_num_args, max_num_args)

#define ZEND_PARSE_PARAMETERS_NONE() do { \
		if (UNEXPECTED(ZEND_NUM_ARGS() != 0)) { \
			zend_wrong_parameters_none_error(); \
			return; \
		} \
	} while (0)

#define ZEND_PARSE_PARAMETERS_END_EX(failure) \
			ZEND_ASSERT(_i == _max_num_args || _max_num_args == (uint32_t) -1); \
		} while (0); \
		if (UNEXPECTED(_error_code != ZPP_ERROR_OK)) { \
			if (!(_flags & ZEND_PARSE_PARAMS_QUIET)) { \
				zend_wrong_parameter_error(_error_code, _i, _error, _expected_type, _arg); \
			} \
			failure; \
		} \
	} while (0)

#define ZEND_PARSE_PARAMETERS_END() \
	ZEND_PARSE_PARAMETERS_END_EX(return)

#define Z_PARAM_PROLOGUE(deref, separate) \
	++_i; \
	ZEND_ASSERT(_i <= _min_num_args || _optional==1); \
	ZEND_ASSERT(_i >  _min_num_args || _optional==0); \
	if (_optional) { \
		if (UNEXPECTED(_i >_num_args)) break; \
	} \
	_real_arg++; \
	_arg = _real_arg; \
	if (deref) { \
		if (EXPECTED(Z_ISREF_P(_arg))) { \
			_arg = Z_REFVAL_P(_arg); \
		} \
	} \
	if (separate) { \
		SEPARATE_ZVAL_NOREF(_arg); \
	}

/* get the zval* for a previously parsed argument */
#define Z_PARAM_GET_PREV_ZVAL(dest) \
	zend_parse_arg_zval_deref(_arg, &dest, 0);

/* old "|" */
#define Z_PARAM_OPTIONAL \
	_optional = 1;

/* old "a" */
#define Z_PARAM_ARRAY_EX2(dest, check_null, deref, separate) \
		Z_PARAM_PROLOGUE(deref, separate); \
		if (UNEXPECTED(!zend_parse_arg_array(_arg, &dest, check_null, 0))) { \
			_expected_type = check_null ? Z_EXPECTED_ARRAY_OR_NULL : Z_EXPECTED_ARRAY; \
			_error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_ARRAY_EX(dest, check_null, separate) \
	Z_PARAM_ARRAY_EX2(dest, check_null, separate, separate)

#define Z_PARAM_ARRAY(dest) \
	Z_PARAM_ARRAY_EX(dest, 0, 0)

#define Z_PARAM_ARRAY_OR_NULL(dest) \
	Z_PARAM_ARRAY_EX(dest, 1, 0)

/* old "A" */
#define Z_PARAM_ARRAY_OR_OBJECT_EX2(dest, check_null, deref, separate) \
		Z_PARAM_PROLOGUE(deref, separate); \
		if (UNEXPECTED(!zend_parse_arg_array(_arg, &dest, check_null, 1))) { \
			_expected_type = check_null ? Z_EXPECTED_ARRAY_OR_NULL : Z_EXPECTED_ARRAY; \
			_error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_ARRAY_OR_OBJECT_EX(dest, check_null, separate) \
	Z_PARAM_ARRAY_OR_OBJECT_EX2(dest, check_null, separate, separate)

#define Z_PARAM_ARRAY_OR_OBJECT(dest) \
	Z_PARAM_ARRAY_OR_OBJECT_EX(dest, 0, 0)

#define Z_PARAM_ITERABLE_EX(dest, check_null) \
	Z_PARAM_PROLOGUE(0, 0); \
	if (UNEXPECTED(!zend_parse_arg_iterable(_arg, &dest, check_null))) { \
		_expected_type = check_null ? Z_EXPECTED_ITERABLE_OR_NULL : Z_EXPECTED_ITERABLE; \
		_error_code = ZPP_ERROR_WRONG_ARG; \
		break; \
	}

#define Z_PARAM_ITERABLE(dest) \
	Z_PARAM_ITERABLE_EX(dest, 0)

#define Z_PARAM_ITERABLE_OR_NULL(dest) \
	Z_PARAM_ITERABLE_EX(dest, 1)

/* old "b" */
#define Z_PARAM_BOOL_EX(dest, is_null, check_null, deref) \
		Z_PARAM_PROLOGUE(deref, 0); \
		if (UNEXPECTED(!zend_parse_arg_bool(_arg, &dest, &is_null, check_null, _i))) { \
			_expected_type = check_null ? Z_EXPECTED_BOOL_OR_NULL : Z_EXPECTED_BOOL; \
			_error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_BOOL(dest) \
	Z_PARAM_BOOL_EX(dest, _dummy, 0, 0)

#define Z_PARAM_BOOL_OR_NULL(dest, is_null) \
	Z_PARAM_BOOL_EX(dest, is_null, 1, 0)

/* old "C" */
#define Z_PARAM_CLASS_EX(dest, check_null, deref) \
		Z_PARAM_PROLOGUE(deref, 0); \
		if (UNEXPECTED(!zend_parse_arg_class(_arg, &dest, _i, check_null))) { \
			_error_code = ZPP_ERROR_FAILURE; \
			break; \
		}

#define Z_PARAM_CLASS(dest) \
	Z_PARAM_CLASS_EX(dest, 0, 0)

#define Z_PARAM_CLASS_OR_NULL(dest) \
	Z_PARAM_CLASS_EX(dest, 1, 0)

#define Z_PARAM_OBJ_OR_CLASS_NAME_EX(dest, allow_null) \
	Z_PARAM_PROLOGUE(0, 0); \
	if (UNEXPECTED(!zend_parse_arg_obj_or_class_name(_arg, &dest, allow_null))) { \
		_expected_type = allow_null ? Z_EXPECTED_OBJECT_OR_CLASS_NAME_OR_NULL : Z_EXPECTED_OBJECT_OR_CLASS_NAME; \
		_error_code = ZPP_ERROR_WRONG_ARG; \
		break; \
	}

#define Z_PARAM_OBJ_OR_CLASS_NAME(dest) \
	Z_PARAM_OBJ_OR_CLASS_NAME_EX(dest, 0);

#define Z_PARAM_OBJ_OR_CLASS_NAME_OR_NULL(dest) \
	Z_PARAM_OBJ_OR_CLASS_NAME_EX(dest, 1);

#define Z_PARAM_OBJ_OR_STR_EX(destination_object, destination_string, allow_null) \
	Z_PARAM_PROLOGUE(0, 0); \
	if (UNEXPECTED(!zend_parse_arg_obj_or_str(_arg, &destination_object, NULL, &destination_string, allow_null, _i))) { \
		_expected_type = allow_null ? Z_EXPECTED_OBJECT_OR_STRING_OR_NULL : Z_EXPECTED_OBJECT_OR_STRING; \
		_error_code = ZPP_ERROR_WRONG_ARG; \
		break; \
	}

#define Z_PARAM_OBJ_OR_STR(destination_object, destination_string) \
	Z_PARAM_OBJ_OR_STR_EX(destination_object, destination_string, 0);

#define Z_PARAM_OBJ_OR_STR_OR_NULL(destination_object, destination_string) \
	Z_PARAM_OBJ_OR_STR_EX(destination_object, destination_string, 1);

#define Z_PARAM_OBJ_OF_CLASS_OR_STR_EX(destination_object, base_ce, destination_string, allow_null) \
	Z_PARAM_PROLOGUE(0, 0); \
	if (UNEXPECTED(!zend_parse_arg_obj_or_str(_arg, &destination_object, base_ce, &destination_string, allow_null, _i))) { \
		if (base_ce) { \
			_error = ZSTR_VAL((base_ce)->name); \
			_error_code = allow_null ? ZPP_ERROR_WRONG_CLASS_OR_STRING_OR_NULL : ZPP_ERROR_WRONG_CLASS_OR_STRING; \
			break; \
		} else { \
			_expected_type = allow_null ? Z_EXPECTED_OBJECT_OR_STRING_OR_NULL : Z_EXPECTED_OBJECT_OR_STRING; \
			_error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		} \
	}

#define Z_PARAM_OBJ_OF_CLASS_OR_STR(destination_object, base_ce, destination_string) \
	Z_PARAM_OBJ_OF_CLASS_OR_STR_EX(destination_object, base_ce, destination_string, 0);

#define Z_PARAM_OBJ_OF_CLASS_OR_STR_OR_NULL(destination_object, base_ce, destination_string) \
	Z_PARAM_OBJ_OF_CLASS_OR_STR_EX(destination_object, base_ce, destination_string, 1);

/* old "d" */
#define Z_PARAM_DOUBLE_EX(dest, is_null, check_null, deref) \
		Z_PARAM_PROLOGUE(deref, 0); \
		if (UNEXPECTED(!zend_parse_arg_double(_arg, &dest, &is_null, check_null, _i))) { \
			_expected_type = check_null ? Z_EXPECTED_DOUBLE_OR_NULL : Z_EXPECTED_DOUBLE; \
			_error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_DOUBLE(dest) \
	Z_PARAM_DOUBLE_EX(dest, _dummy, 0, 0)

#define Z_PARAM_DOUBLE_OR_NULL(dest, is_null) \
	Z_PARAM_DOUBLE_EX(dest, is_null, 1, 0)

/* old "f" */
#define Z_PARAM_FUNC_EX(dest_fci, dest_fcc, check_null, deref, free_trampoline) \
		Z_PARAM_PROLOGUE(deref, 0); \
		if (UNEXPECTED(!zend_parse_arg_func(_arg, &dest_fci, &dest_fcc, check_null, &_error, free_trampoline))) { \
			if (!_error) { \
				_expected_type = check_null ? Z_EXPECTED_FUNC_OR_NULL : Z_EXPECTED_FUNC; \
				_error_code = ZPP_ERROR_WRONG_ARG; \
			} else { \
				_error_code = check_null ? ZPP_ERROR_WRONG_CALLBACK_OR_NULL : ZPP_ERROR_WRONG_CALLBACK; \
			} \
			break; \
		} \

#define Z_PARAM_FUNC(dest_fci, dest_fcc) \
	Z_PARAM_FUNC_EX(dest_fci, dest_fcc, 0, 0, true)

#define Z_PARAM_FUNC_NO_TRAMPOLINE_FREE(dest_fci, dest_fcc) \
	Z_PARAM_FUNC_EX(dest_fci, dest_fcc, 0, 0, false)

#define Z_PARAM_FUNC_OR_NULL(dest_fci, dest_fcc) \
	Z_PARAM_FUNC_EX(dest_fci, dest_fcc, 1, 0, true)

#define Z_PARAM_FUNC_NO_TRAMPOLINE_FREE_OR_NULL(dest_fci, dest_fcc) \
	Z_PARAM_FUNC_EX(dest_fci, dest_fcc, 1, 0, false)

#define Z_PARAM_FUNC_OR_NULL_WITH_ZVAL(dest_fci, dest_fcc, dest_zp) \
	Z_PARAM_FUNC_EX(dest_fci, dest_fcc, 1, 0, true) \
	Z_PARAM_GET_PREV_ZVAL(dest_zp)

/* old "h" */
#define Z_PARAM_ARRAY_HT_EX2(dest, check_null, deref, separate) \
		Z_PARAM_PROLOGUE(deref, separate); \
		if (UNEXPECTED(!zend_parse_arg_array_ht(_arg, &dest, check_null, 0, separate))) { \
			_expected_type = check_null ? Z_EXPECTED_ARRAY_OR_NULL : Z_EXPECTED_ARRAY; \
			_error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_ARRAY_HT_EX(dest, check_null, separate) \
	Z_PARAM_ARRAY_HT_EX2(dest, check_null, separate, separate)

#define Z_PARAM_ARRAY_HT(dest) \
	Z_PARAM_ARRAY_HT_EX(dest, 0, 0)

#define Z_PARAM_ARRAY_HT_OR_NULL(dest) \
	Z_PARAM_ARRAY_HT_EX(dest, 1, 0)

#define Z_PARAM_ARRAY_HT_OR_LONG_EX(dest_ht, dest_long, is_null, allow_null) \
	Z_PARAM_PROLOGUE(0, 0); \
	if (UNEXPECTED(!zend_parse_arg_array_ht_or_long(_arg, &dest_ht, &dest_long, &is_null, allow_null, _i))) { \
		_expected_type = allow_null ? Z_EXPECTED_ARRAY_OR_LONG_OR_NULL : Z_EXPECTED_ARRAY_OR_LONG; \
		_error_code = ZPP_ERROR_WRONG_ARG; \
		break; \
	}

#define Z_PARAM_ARRAY_HT_OR_LONG(dest_ht, dest_long) \
	Z_PARAM_ARRAY_HT_OR_LONG_EX(dest_ht, dest_long, _dummy, 0)

#define Z_PARAM_ARRAY_HT_OR_LONG_OR_NULL(dest_ht, dest_long, is_null) \
	Z_PARAM_ARRAY_HT_OR_LONG_EX(dest_ht, dest_long, is_null, 1)

/* old "H" */
#define Z_PARAM_ARRAY_OR_OBJECT_HT_EX2(dest, check_null, deref, separate) \
		Z_PARAM_PROLOGUE(deref, separate); \
		if (UNEXPECTED(!zend_parse_arg_array_ht(_arg, &dest, check_null, 1, separate))) { \
			_expected_type = check_null ? Z_EXPECTED_ARRAY_OR_NULL : Z_EXPECTED_ARRAY; \
			_error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_ARRAY_OR_OBJECT_HT_EX(dest, check_null, separate) \
	Z_PARAM_ARRAY_OR_OBJECT_HT_EX2(dest, check_null, separate, separate)

#define Z_PARAM_ARRAY_OR_OBJECT_HT(dest) \
	Z_PARAM_ARRAY_OR_OBJECT_HT_EX(dest, 0, 0)

/* old "l" */
#define Z_PARAM_LONG_EX(dest, is_null, check_null, deref) \
		Z_PARAM_PROLOGUE(deref, 0); \
		if (UNEXPECTED(!zend_parse_arg_long(_arg, &dest, &is_null, check_null, _i))) { \
			_expected_type = check_null ? Z_EXPECTED_LONG_OR_NULL : Z_EXPECTED_LONG; \
			_error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_LONG(dest) \
	Z_PARAM_LONG_EX(dest, _dummy, 0, 0)

#define Z_PARAM_LONG_OR_NULL(dest, is_null) \
	Z_PARAM_LONG_EX(dest, is_null, 1, 0)

/* old "n" */
#define Z_PARAM_NUMBER_EX(dest, check_null) \
	Z_PARAM_PROLOGUE(0, 0); \
	if (UNEXPECTED(!zend_parse_arg_number(_arg, &dest, check_null, _i))) { \
		_expected_type = check_null ? Z_EXPECTED_NUMBER_OR_NULL : Z_EXPECTED_NUMBER; \
		_error_code = ZPP_ERROR_WRONG_ARG; \
		break; \
	}

#define Z_PARAM_NUMBER_OR_NULL(dest) \
	Z_PARAM_NUMBER_EX(dest, 1)

#define Z_PARAM_NUMBER(dest) \
	Z_PARAM_NUMBER_EX(dest, 0)

#define Z_PARAM_NUMBER_OR_STR_EX(dest, check_null) \
	Z_PARAM_PROLOGUE(0, 0); \
	if (UNEXPECTED(!zend_parse_arg_number_or_str(_arg, &dest, check_null, _i))) { \
		_expected_type = check_null ? Z_EXPECTED_NUMBER_OR_STRING_OR_NULL : Z_EXPECTED_NUMBER_OR_STRING; \
		_error_code = ZPP_ERROR_WRONG_ARG; \
		break; \
	}

#define Z_PARAM_NUMBER_OR_STR(dest) \
	Z_PARAM_NUMBER_OR_STR_EX(dest, false)

#define Z_PARAM_NUMBER_OR_STR_OR_NULL(dest) \
	Z_PARAM_NUMBER_OR_STR_EX(dest, true)

/* old "o" */
#define Z_PARAM_OBJECT_EX(dest, check_null, deref) \
		Z_PARAM_PROLOGUE(deref, 0); \
		if (UNEXPECTED(!zend_parse_arg_object(_arg, &dest, NULL, check_null))) { \
			_expected_type = check_null ? Z_EXPECTED_OBJECT_OR_NULL : Z_EXPECTED_OBJECT; \
			_error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_OBJECT(dest) \
	Z_PARAM_OBJECT_EX(dest, 0, 0)

#define Z_PARAM_OBJECT_OR_NULL(dest) \
	Z_PARAM_OBJECT_EX(dest, 1, 0)

/* The same as Z_PARAM_OBJECT_EX except that dest is a zend_object rather than a zval */
#define Z_PARAM_OBJ_EX(dest, check_null, deref) \
		Z_PARAM_PROLOGUE(deref, 0); \
		if (UNEXPECTED(!zend_parse_arg_obj(_arg, &dest, NULL, check_null))) { \
			_expected_type = check_null ? Z_EXPECTED_OBJECT_OR_NULL : Z_EXPECTED_OBJECT; \
			_error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_OBJ(dest) \
	Z_PARAM_OBJ_EX(dest, 0, 0)

#define Z_PARAM_OBJ_OR_NULL(dest) \
	Z_PARAM_OBJ_EX(dest, 1, 0)

/* old "O" */
#define Z_PARAM_OBJECT_OF_CLASS_EX(dest, _ce, check_null, deref) \
		Z_PARAM_PROLOGUE(deref, 0); \
		if (UNEXPECTED(!zend_parse_arg_object(_arg, &dest, _ce, check_null))) { \
			if (_ce) { \
				_error = ZSTR_VAL((_ce)->name); \
				_error_code = check_null ? ZPP_ERROR_WRONG_CLASS_OR_NULL : ZPP_ERROR_WRONG_CLASS; \
				break; \
			} else { \
				_expected_type = check_null ? Z_EXPECTED_OBJECT_OR_NULL : Z_EXPECTED_OBJECT; \
				_error_code = ZPP_ERROR_WRONG_ARG; \
				break; \
			} \
		}

#define Z_PARAM_OBJECT_OF_CLASS(dest, _ce) \
	Z_PARAM_OBJECT_OF_CLASS_EX(dest, _ce, 0, 0)

#define Z_PARAM_OBJECT_OF_CLASS_OR_NULL(dest, _ce) \
	Z_PARAM_OBJECT_OF_CLASS_EX(dest, _ce, 1, 0)

/* The same as Z_PARAM_OBJECT_OF_CLASS_EX except that dest is a zend_object rather than a zval */
#define Z_PARAM_OBJ_OF_CLASS_EX(dest, _ce, check_null, deref) \
		Z_PARAM_PROLOGUE(deref, 0); \
		if (UNEXPECTED(!zend_parse_arg_obj(_arg, &dest, _ce, check_null))) { \
			if (_ce) { \
				_error = ZSTR_VAL((_ce)->name); \
				_error_code = check_null ? ZPP_ERROR_WRONG_CLASS_OR_NULL : ZPP_ERROR_WRONG_CLASS; \
				break; \
			} else { \
				_expected_type = check_null ? Z_EXPECTED_OBJECT_OR_NULL : Z_EXPECTED_OBJECT; \
				_error_code = ZPP_ERROR_WRONG_ARG; \
				break; \
			} \
		}

#define Z_PARAM_OBJ_OF_CLASS(dest, _ce) \
	Z_PARAM_OBJ_OF_CLASS_EX(dest, _ce, 0, 0)

#define Z_PARAM_OBJ_OF_CLASS_OR_NULL(dest, _ce) \
	Z_PARAM_OBJ_OF_CLASS_EX(dest, _ce, 1, 0)

#define Z_PARAM_OBJ_OF_CLASS_OR_LONG_EX(dest_obj, _ce, dest_long, is_null, allow_null) \
		Z_PARAM_PROLOGUE(0, 0); \
		if (UNEXPECTED(!zend_parse_arg_obj_or_long(_arg, &dest_obj, _ce, &dest_long, &is_null, allow_null, _i))) { \
			_error = ZSTR_VAL((_ce)->name); \
			_error_code = allow_null ? ZPP_ERROR_WRONG_CLASS_OR_LONG_OR_NULL : ZPP_ERROR_WRONG_CLASS_OR_LONG; \
			break; \
		}

#define Z_PARAM_OBJ_OF_CLASS_OR_LONG(dest_obj, _ce, dest_long) \
	Z_PARAM_OBJ_OF_CLASS_OR_LONG_EX(dest_obj, _ce, dest_long, _dummy, 0)

#define Z_PARAM_OBJ_OF_CLASS_OR_LONG_OR_NULL(dest_obj, _ce, dest_long, is_null) \
	Z_PARAM_OBJ_OF_CLASS_OR_LONG_EX(dest_obj, _ce, dest_long, is_null, 1)

/* old "p" */
#define Z_PARAM_PATH_EX(dest, dest_len, check_null, deref) \
		Z_PARAM_PROLOGUE(deref, 0); \
		if (UNEXPECTED(!zend_parse_arg_path(_arg, &dest, &dest_len, check_null, _i))) { \
			_expected_type = check_null ? Z_EXPECTED_PATH_OR_NULL : Z_EXPECTED_PATH; \
			_error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_PATH(dest, dest_len) \
	Z_PARAM_PATH_EX(dest, dest_len, 0, 0)

#define Z_PARAM_PATH_OR_NULL(dest, dest_len) \
	Z_PARAM_PATH_EX(dest, dest_len, 1, 0)

/* old "P" */
#define Z_PARAM_PATH_STR_EX(dest, check_null, deref) \
		Z_PARAM_PROLOGUE(deref, 0); \
		if (UNEXPECTED(!zend_parse_arg_path_str(_arg, &dest, check_null, _i))) { \
			_expected_type = check_null ? Z_EXPECTED_PATH_OR_NULL : Z_EXPECTED_PATH; \
			_error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_PATH_STR(dest) \
	Z_PARAM_PATH_STR_EX(dest, 0, 0)

#define Z_PARAM_PATH_STR_OR_NULL(dest) \
	Z_PARAM_PATH_STR_EX(dest, 1, 0)

/* old "r" */
#define Z_PARAM_RESOURCE_EX(dest, check_null, deref) \
		Z_PARAM_PROLOGUE(deref, 0); \
		if (UNEXPECTED(!zend_parse_arg_resource(_arg, &dest, check_null))) { \
			_expected_type = check_null ? Z_EXPECTED_RESOURCE_OR_NULL : Z_EXPECTED_RESOURCE; \
			_error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_RESOURCE(dest) \
	Z_PARAM_RESOURCE_EX(dest, 0, 0)

#define Z_PARAM_RESOURCE_OR_NULL(dest) \
	Z_PARAM_RESOURCE_EX(dest, 1, 0)

/* old "s" */
#define Z_PARAM_STRING_EX(dest, dest_len, check_null, deref) \
		Z_PARAM_PROLOGUE(deref, 0); \
		if (UNEXPECTED(!zend_parse_arg_string(_arg, &dest, &dest_len, check_null, _i))) { \
			_expected_type = check_null ? Z_EXPECTED_STRING_OR_NULL : Z_EXPECTED_STRING; \
			_error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_STRING(dest, dest_len) \
	Z_PARAM_STRING_EX(dest, dest_len, 0, 0)

#define Z_PARAM_STRING_OR_NULL(dest, dest_len) \
	Z_PARAM_STRING_EX(dest, dest_len, 1, 0)

/* old "S" */
#define Z_PARAM_STR_EX(dest, check_null, deref) \
		Z_PARAM_PROLOGUE(deref, 0); \
		if (UNEXPECTED(!zend_parse_arg_str(_arg, &dest, check_null, _i))) { \
			_expected_type = check_null ? Z_EXPECTED_STRING_OR_NULL : Z_EXPECTED_STRING; \
			_error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_STR(dest) \
	Z_PARAM_STR_EX(dest, 0, 0)

#define Z_PARAM_STR_OR_NULL(dest) \
	Z_PARAM_STR_EX(dest, 1, 0)

/* old "z" */
#define Z_PARAM_ZVAL_EX2(dest, check_null, deref, separate) \
		Z_PARAM_PROLOGUE(deref, separate); \
		zend_parse_arg_zval_deref(_arg, &dest, check_null);

#define Z_PARAM_ZVAL_EX(dest, check_null, separate) \
	Z_PARAM_ZVAL_EX2(dest, check_null, separate, separate)

#define Z_PARAM_ZVAL(dest) \
	Z_PARAM_ZVAL_EX(dest, 0, 0)

#define Z_PARAM_ZVAL_OR_NULL(dest) \
	Z_PARAM_ZVAL_EX(dest, 1, 0)

/* old "+" and "*" */
#define Z_PARAM_VARIADIC_EX(spec, dest, dest_num, post_varargs) do { \
		uint32_t _num_varargs = _num_args - _i - (post_varargs); \
		if (EXPECTED(_num_varargs > 0)) { \
			dest = _real_arg + 1; \
			dest_num = _num_varargs; \
			_i += _num_varargs; \
			_real_arg += _num_varargs; \
		} else { \
			dest = NULL; \
			dest_num = 0; \
		} \
		if (UNEXPECTED(ZEND_CALL_INFO(execute_data) & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS)) { \
			_error_code = ZPP_ERROR_UNEXPECTED_EXTRA_NAMED; \
			break; \
		} \
	} while (0);

#define Z_PARAM_VARIADIC(spec, dest, dest_num) \
	Z_PARAM_VARIADIC_EX(spec, dest, dest_num, 0)

#define Z_PARAM_VARIADIC_WITH_NAMED(dest, dest_num, dest_named) do { \
		uint32_t _num_varargs = _num_args - _i; \
		if (EXPECTED(_num_varargs > 0)) { \
			dest = _real_arg + 1; \
			dest_num = _num_varargs; \
		} else { \
			dest = NULL; \
			dest_num = 0; \
		} \
		if (ZEND_CALL_INFO(execute_data) & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS) { \
			dest_named = execute_data->extra_named_params; \
		} else { \
			dest_named = NULL; \
		} \
	} while (0);

#define Z_PARAM_ARRAY_HT_OR_STR_EX(dest_ht, dest_str, allow_null) \
	Z_PARAM_PROLOGUE(0, 0); \
	if (UNEXPECTED(!zend_parse_arg_array_ht_or_str(_arg, &dest_ht, &dest_str, allow_null, _i))) { \
		_expected_type = allow_null ? Z_EXPECTED_ARRAY_OR_STRING_OR_NULL : Z_EXPECTED_ARRAY_OR_STRING; \
		_error_code = ZPP_ERROR_WRONG_ARG; \
		break; \
	}

#define Z_PARAM_ARRAY_HT_OR_STR(dest_ht, dest_str) \
	Z_PARAM_ARRAY_HT_OR_STR_EX(dest_ht, dest_str, 0);

#define Z_PARAM_ARRAY_HT_OR_STR_OR_NULL(dest_ht, dest_str) \
	Z_PARAM_ARRAY_HT_OR_STR_EX(dest_ht, dest_str, 1);

#define Z_PARAM_STR_OR_LONG_EX(dest_str, dest_long, is_null, allow_null) \
	Z_PARAM_PROLOGUE(0, 0); \
	if (UNEXPECTED(!zend_parse_arg_str_or_long(_arg, &dest_str, &dest_long, &is_null, allow_null, _i))) { \
		_expected_type = allow_null ? Z_EXPECTED_STRING_OR_LONG_OR_NULL : Z_EXPECTED_STRING_OR_LONG; \
		_error_code = ZPP_ERROR_WRONG_ARG; \
		break; \
	}

#define Z_PARAM_STR_OR_LONG(dest_str, dest_long) \
	Z_PARAM_STR_OR_LONG_EX(dest_str, dest_long, _dummy, 0);

#define Z_PARAM_STR_OR_LONG_OR_NULL(dest_str, dest_long, is_null) \
	Z_PARAM_STR_OR_LONG_EX(dest_str, dest_long, is_null, 1);

/* End of new parameter parsing API */

/* Inlined implementations shared by new and old parameter parsing APIs */

ZEND_API bool ZEND_FASTCALL zend_parse_arg_class(zval *arg, zend_class_entry **pce, uint32_t num, bool check_null);
ZEND_API bool ZEND_FASTCALL zend_parse_arg_bool_slow(const zval *arg, bool *dest, uint32_t arg_num);
ZEND_API bool ZEND_FASTCALL zend_parse_arg_bool_weak(const zval *arg, bool *dest, uint32_t arg_num);
ZEND_API bool ZEND_FASTCALL zend_parse_arg_long_slow(const zval *arg, zend_long *dest, uint32_t arg_num);
ZEND_API bool ZEND_FASTCALL zend_parse_arg_long_weak(const zval *arg, zend_long *dest, uint32_t arg_num);
ZEND_API bool ZEND_FASTCALL zend_parse_arg_double_slow(const zval *arg, double *dest, uint32_t arg_num);
ZEND_API bool ZEND_FASTCALL zend_parse_arg_double_weak(const zval *arg, double *dest, uint32_t arg_num);
ZEND_API bool ZEND_FASTCALL zend_parse_arg_str_slow(zval *arg, zend_string **dest, uint32_t arg_num);
ZEND_API bool ZEND_FASTCALL zend_parse_arg_str_weak(zval *arg, zend_string **dest, uint32_t arg_num);
ZEND_API bool ZEND_FASTCALL zend_parse_arg_number_slow(zval *arg, zval **dest, uint32_t arg_num);
ZEND_API bool ZEND_FASTCALL zend_parse_arg_number_or_str_slow(zval *arg, zval **dest, uint32_t arg_num);
ZEND_API bool ZEND_FASTCALL zend_parse_arg_str_or_long_slow(zval *arg, zend_string **dest_str, zend_long *dest_long, uint32_t arg_num);

static zend_always_inline bool zend_parse_arg_bool(const zval *arg, bool *dest, bool *is_null, bool check_null, uint32_t arg_num)
{
	if (check_null) {
		*is_null = 0;
	}
	if (EXPECTED(Z_TYPE_P(arg) == IS_TRUE)) {
		*dest = 1;
	} else if (EXPECTED(Z_TYPE_P(arg) == IS_FALSE)) {
		*dest = 0;
	} else if (check_null && Z_TYPE_P(arg) == IS_NULL) {
		*is_null = 1;
		*dest = 0;
	} else {
		return zend_parse_arg_bool_slow(arg, dest, arg_num);
	}
	return 1;
}

static zend_always_inline bool zend_parse_arg_long(zval *arg, zend_long *dest, bool *is_null, bool check_null, uint32_t arg_num)
{
	if (check_null) {
		*is_null = 0;
	}
	if (EXPECTED(Z_TYPE_P(arg) == IS_LONG)) {
		*dest = Z_LVAL_P(arg);
	} else if (check_null && Z_TYPE_P(arg) == IS_NULL) {
		*is_null = 1;
		*dest = 0;
	} else {
		return zend_parse_arg_long_slow(arg, dest, arg_num);
	}
	return 1;
}

static zend_always_inline bool zend_parse_arg_double(const zval *arg, double *dest, bool *is_null, bool check_null, uint32_t arg_num)
{
	if (check_null) {
		*is_null = 0;
	}
	if (EXPECTED(Z_TYPE_P(arg) == IS_DOUBLE)) {
		*dest = Z_DVAL_P(arg);
	} else if (check_null && Z_TYPE_P(arg) == IS_NULL) {
		*is_null = 1;
		*dest = 0.0;
	} else {
		return zend_parse_arg_double_slow(arg, dest, arg_num);
	}
	return 1;
}

static zend_always_inline bool zend_parse_arg_number(zval *arg, zval **dest, bool check_null, uint32_t arg_num)
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_LONG || Z_TYPE_P(arg) == IS_DOUBLE)) {
		*dest = arg;
	} else if (check_null && EXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		*dest = NULL;
	} else {
		return zend_parse_arg_number_slow(arg, dest, arg_num);
	}
	return 1;
}

static zend_always_inline bool zend_parse_arg_number_or_str(zval *arg, zval **dest, bool check_null, uint32_t arg_num)
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_LONG || Z_TYPE_P(arg) == IS_DOUBLE || Z_TYPE_P(arg) == IS_STRING)) {
		*dest = arg;
	} else if (check_null && EXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		*dest = NULL;
	} else {
		return zend_parse_arg_number_or_str_slow(arg, dest, arg_num);
	}
	return true;
}

static zend_always_inline bool zend_parse_arg_str(zval *arg, zend_string **dest, bool check_null, uint32_t arg_num)
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_STRING)) {
		*dest = Z_STR_P(arg);
	} else if (check_null && Z_TYPE_P(arg) == IS_NULL) {
		*dest = NULL;
	} else {
		return zend_parse_arg_str_slow(arg, dest, arg_num);
	}
	return 1;
}

static zend_always_inline bool zend_parse_arg_string(zval *arg, char **dest, size_t *dest_len, bool check_null, uint32_t arg_num)
{
	zend_string *str;

	if (!zend_parse_arg_str(arg, &str, check_null, arg_num)) {
		return 0;
	}
	if (check_null && UNEXPECTED(!str)) {
		*dest = NULL;
		*dest_len = 0;
	} else {
		*dest = ZSTR_VAL(str);
		*dest_len = ZSTR_LEN(str);
	}
	return 1;
}

static zend_always_inline bool zend_parse_arg_path_str(zval *arg, zend_string **dest, bool check_null, uint32_t arg_num)
{
	if (!zend_parse_arg_str(arg, dest, check_null, arg_num) ||
	    (*dest && UNEXPECTED(CHECK_NULL_PATH(ZSTR_VAL(*dest), ZSTR_LEN(*dest))))) {
		return 0;
	}
	return 1;
}

static zend_always_inline bool zend_parse_arg_path(zval *arg, char **dest, size_t *dest_len, bool check_null, uint32_t arg_num)
{
	zend_string *str;

	if (!zend_parse_arg_path_str(arg, &str, check_null, arg_num)) {
		return 0;
	}
	if (check_null && UNEXPECTED(!str)) {
		*dest = NULL;
		*dest_len = 0;
	} else {
		*dest = ZSTR_VAL(str);
		*dest_len = ZSTR_LEN(str);
	}
	return 1;
}

static zend_always_inline bool zend_parse_arg_iterable(zval *arg, zval **dest, bool check_null)
{
	if (EXPECTED(zend_is_iterable(arg))) {
		*dest = arg;
		return 1;
	}

	if (check_null && EXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		*dest = NULL;
		return 1;
	}

	return 0;
}

static zend_always_inline bool zend_parse_arg_array(zval *arg, zval **dest, bool check_null, bool or_object)
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_ARRAY) ||
		(or_object && EXPECTED(Z_TYPE_P(arg) == IS_OBJECT))) {
		*dest = arg;
	} else if (check_null && EXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		*dest = NULL;
	} else {
		return 0;
	}
	return 1;
}

static zend_always_inline bool zend_parse_arg_array_ht(const zval *arg, HashTable **dest, bool check_null, bool or_object, bool separate)
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_ARRAY)) {
		*dest = Z_ARRVAL_P(arg);
	} else if (or_object && EXPECTED(Z_TYPE_P(arg) == IS_OBJECT)) {
		zend_object *zobj = Z_OBJ_P(arg);
		if (separate
		 && zobj->properties
		 && UNEXPECTED(GC_REFCOUNT(zobj->properties) > 1)) {
			if (EXPECTED(!(GC_FLAGS(zobj->properties) & IS_ARRAY_IMMUTABLE))) {
				GC_DELREF(zobj->properties);
			}
			zobj->properties = zend_array_dup(zobj->properties);
		}
		*dest = zobj->handlers->get_properties(zobj);
	} else if (check_null && EXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		*dest = NULL;
	} else {
		return 0;
	}
	return 1;
}

static zend_always_inline bool zend_parse_arg_array_ht_or_long(
	zval *arg, HashTable **dest_ht, zend_long *dest_long, bool *is_null, bool allow_null, uint32_t arg_num
) {
	if (allow_null) {
		*is_null = 0;
	}

	if (EXPECTED(Z_TYPE_P(arg) == IS_ARRAY)) {
		*dest_ht = Z_ARRVAL_P(arg);
	} else if (EXPECTED(Z_TYPE_P(arg) == IS_LONG)) {
		*dest_ht = NULL;
		*dest_long = Z_LVAL_P(arg);
	} else if (allow_null && EXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		*dest_ht = NULL;
		*is_null = 1;
	} else {
		*dest_ht = NULL;
		return zend_parse_arg_long_slow(arg, dest_long, arg_num);
	}

	return 1;
}

static zend_always_inline bool zend_parse_arg_object(zval *arg, zval **dest, zend_class_entry *ce, bool check_null)
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_OBJECT) &&
	    (!ce || EXPECTED(instanceof_function(Z_OBJCE_P(arg), ce) != 0))) {
		*dest = arg;
	} else if (check_null && EXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		*dest = NULL;
	} else {
		return 0;
	}
	return 1;
}

static zend_always_inline bool zend_parse_arg_obj(const zval *arg, zend_object **dest, zend_class_entry *ce, bool check_null)
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_OBJECT) &&
	    (!ce || EXPECTED(instanceof_function(Z_OBJCE_P(arg), ce) != 0))) {
		*dest = Z_OBJ_P(arg);
	} else if (check_null && EXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		*dest = NULL;
	} else {
		return 0;
	}
	return 1;
}

static zend_always_inline bool zend_parse_arg_obj_or_long(
	zval *arg, zend_object **dest_obj, zend_class_entry *ce, zend_long *dest_long, bool *is_null, bool allow_null, uint32_t arg_num
) {
	if (allow_null) {
		*is_null = 0;
	}

	if (EXPECTED(Z_TYPE_P(arg) == IS_OBJECT) && EXPECTED(instanceof_function(Z_OBJCE_P(arg), ce) != 0)) {
		*dest_obj = Z_OBJ_P(arg);
	} else if (EXPECTED(Z_TYPE_P(arg) == IS_LONG)) {
		*dest_obj = NULL;
		*dest_long = Z_LVAL_P(arg);
	} else if (allow_null && EXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		*dest_obj = NULL;
		*is_null = 1;
	} else {
		*dest_obj = NULL;
		return zend_parse_arg_long_slow(arg, dest_long, arg_num);
	}

	return 1;
}

static zend_always_inline bool zend_parse_arg_resource(zval *arg, zval **dest, bool check_null)
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_RESOURCE)) {
		*dest = arg;
	} else if (check_null && EXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		*dest = NULL;
	} else {
		return 0;
	}
	return 1;
}

static zend_always_inline bool zend_parse_arg_func(zval *arg, zend_fcall_info *dest_fci, zend_fcall_info_cache *dest_fcc, bool check_null, char **error, bool free_trampoline)
{
	if (check_null && UNEXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		dest_fci->size = 0;
		dest_fcc->function_handler = NULL;
		*error = NULL;
	} else if (UNEXPECTED(zend_fcall_info_init(arg, 0, dest_fci, dest_fcc, NULL, error) != SUCCESS)) {
		return 0;
	}
	if (free_trampoline) {
		/* Release call trampolines: The function may not get called, in which case
		 * the trampoline will leak. Force it to be refetched during
		 * zend_call_function instead. */
		zend_release_fcall_info_cache(dest_fcc);
	}
	return 1;
}

static zend_always_inline void zend_parse_arg_zval(zval *arg, zval **dest, bool check_null)
{
	*dest = (check_null &&
	    (UNEXPECTED(Z_TYPE_P(arg) == IS_NULL) ||
	     (UNEXPECTED(Z_ISREF_P(arg)) &&
	      UNEXPECTED(Z_TYPE_P(Z_REFVAL_P(arg)) == IS_NULL)))) ? NULL : arg;
}

static zend_always_inline void zend_parse_arg_zval_deref(zval *arg, zval **dest, bool check_null)
{
	*dest = (check_null && UNEXPECTED(Z_TYPE_P(arg) == IS_NULL)) ? NULL : arg;
}

static zend_always_inline bool zend_parse_arg_array_ht_or_str(
		zval *arg, HashTable **dest_ht, zend_string **dest_str, bool allow_null, uint32_t arg_num)
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_STRING)) {
		*dest_ht = NULL;
		*dest_str = Z_STR_P(arg);
	} else if (EXPECTED(Z_TYPE_P(arg) == IS_ARRAY)) {
		*dest_ht = Z_ARRVAL_P(arg);
		*dest_str = NULL;
	} else if (allow_null && EXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		*dest_ht = NULL;
		*dest_str = NULL;
	} else {
		*dest_ht = NULL;
		return zend_parse_arg_str_slow(arg, dest_str, arg_num);
	}
	return 1;
}

static zend_always_inline bool zend_parse_arg_str_or_long(zval *arg, zend_string **dest_str, zend_long *dest_long,
	bool *is_null, bool allow_null, uint32_t arg_num)
{
	if (allow_null) {
		*is_null = 0;
	}
	if (EXPECTED(Z_TYPE_P(arg) == IS_STRING)) {
		*dest_str = Z_STR_P(arg);
	} else if (EXPECTED(Z_TYPE_P(arg) == IS_LONG)) {
		*dest_str = NULL;
		*dest_long = Z_LVAL_P(arg);
	} else if (allow_null && EXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		*dest_str = NULL;
		*is_null = 1;
	} else {
		return zend_parse_arg_str_or_long_slow(arg, dest_str, dest_long, arg_num);
	}
	return 1;
}

static zend_always_inline bool zend_parse_arg_obj_or_class_name(
	zval *arg, zend_class_entry **destination, bool allow_null
) {
	if (EXPECTED(Z_TYPE_P(arg) == IS_STRING)) {
		*destination = zend_lookup_class(Z_STR_P(arg));

		return *destination != NULL;
	} else if (EXPECTED(Z_TYPE_P(arg) == IS_OBJECT)) {
		*destination = Z_OBJ_P(arg)->ce;
	} else if (allow_null && EXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		*destination = NULL;
	} else {
		return 0;
	}

	return 1;
}

static zend_always_inline bool zend_parse_arg_obj_or_str(
	zval *arg, zend_object **destination_object, zend_class_entry *base_ce, zend_string **destination_string, bool allow_null, uint32_t arg_num
) {
	if (EXPECTED(Z_TYPE_P(arg) == IS_OBJECT)) {
		if (!base_ce || EXPECTED(instanceof_function(Z_OBJCE_P(arg), base_ce))) {
			*destination_object = Z_OBJ_P(arg);
			*destination_string = NULL;
			return 1;
		}
	}

	*destination_object = NULL;
	return zend_parse_arg_str(arg, destination_string, allow_null, arg_num);
}

END_EXTERN_C()

#endif /* ZEND_API_H */
