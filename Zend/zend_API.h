/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Andrei Zmievski <andrei@php.net>                            |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_API_H
#define ZEND_API_H

#include "zend_modules.h"
#include "zend_list.h"
#include "zend_operators.h"
#include "zend_variables.h"
#include "zend_execute.h"


BEGIN_EXTERN_C()

typedef struct _zend_function_entry {
	const char *fname;
	void (*handler)(INTERNAL_FUNCTION_PARAMETERS);
	const struct _zend_internal_arg_info *arg_info;
	uint32_t num_args;
	uint32_t flags;
} zend_function_entry;

typedef struct _zend_fcall_info {
	size_t size;
	HashTable *function_table;
	zval function_name;
	zend_array *symbol_table;
	zval *retval;
	zval *params;
	zend_object *object;
	zend_bool no_separation;
	uint32_t param_count;
} zend_fcall_info;

typedef struct _zend_fcall_info_cache {
	zend_bool initialized;
	zend_function *function_handler;
	zend_class_entry *calling_scope;
	zend_class_entry *called_scope;
	zend_object *object;
} zend_fcall_info_cache;

#define ZEND_NS_NAME(ns, name)			ns "\\" name

#define ZEND_FN(name) zif_##name
#define ZEND_MN(name) zim_##name
#define ZEND_NAMED_FUNCTION(name)		void name(INTERNAL_FUNCTION_PARAMETERS)
#define ZEND_FUNCTION(name)				ZEND_NAMED_FUNCTION(ZEND_FN(name))
#define ZEND_METHOD(classname, name)	ZEND_NAMED_FUNCTION(ZEND_MN(classname##_##name))

#define ZEND_FENTRY(zend_name, name, arg_info, flags)	{ #zend_name, name, arg_info, (uint32_t) (sizeof(arg_info)/sizeof(struct _zend_internal_arg_info)-1), flags },

#define ZEND_RAW_FENTRY(zend_name, name, arg_info, flags)   { zend_name, name, arg_info, (uint32_t) (sizeof(arg_info)/sizeof(struct _zend_internal_arg_info)-1), flags },
#define ZEND_RAW_NAMED_FE(zend_name, name, arg_info) ZEND_RAW_FENTRY(#zend_name, name, arg_info, 0)

#define ZEND_NAMED_FE(zend_name, name, arg_info)	ZEND_FENTRY(zend_name, name, arg_info, 0)
#define ZEND_FE(name, arg_info)						ZEND_FENTRY(name, ZEND_FN(name), arg_info, 0)
#define ZEND_DEP_FE(name, arg_info)                 ZEND_FENTRY(name, ZEND_FN(name), arg_info, ZEND_ACC_DEPRECATED)
#define ZEND_FALIAS(name, alias, arg_info)			ZEND_FENTRY(name, ZEND_FN(alias), arg_info, 0)
#define ZEND_DEP_FALIAS(name, alias, arg_info)		ZEND_FENTRY(name, ZEND_FN(alias), arg_info, ZEND_ACC_DEPRECATED)
#define ZEND_NAMED_ME(zend_name, name, arg_info, flags)	ZEND_FENTRY(zend_name, name, arg_info, flags)
#define ZEND_ME(classname, name, arg_info, flags)	ZEND_FENTRY(name, ZEND_MN(classname##_##name), arg_info, flags)
#define ZEND_ABSTRACT_ME(classname, name, arg_info)	ZEND_FENTRY(name, NULL, arg_info, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
#define ZEND_MALIAS(classname, name, alias, arg_info, flags) \
                                                    ZEND_FENTRY(name, ZEND_MN(classname##_##alias), arg_info, flags)
#define ZEND_ME_MAPPING(name, func_name, arg_types, flags) ZEND_NAMED_ME(name, ZEND_FN(func_name), arg_types, flags)

#define ZEND_NS_FENTRY(ns, zend_name, name, arg_info, flags)		ZEND_RAW_FENTRY(ZEND_NS_NAME(ns, #zend_name), name, arg_info, flags)

#define ZEND_NS_RAW_FENTRY(ns, zend_name, name, arg_info, flags)	ZEND_RAW_FENTRY(ZEND_NS_NAME(ns, zend_name), name, arg_info, flags)
#define ZEND_NS_RAW_NAMED_FE(ns, zend_name, name, arg_info)			ZEND_NS_RAW_FENTRY(ns, #zend_name, name, arg_info, 0)

#define ZEND_NS_NAMED_FE(ns, zend_name, name, arg_info)	ZEND_NS_FENTRY(ns, zend_name, name, arg_info, 0)
#define ZEND_NS_FE(ns, name, arg_info)					ZEND_NS_FENTRY(ns, name, ZEND_FN(name), arg_info, 0)
#define ZEND_NS_DEP_FE(ns, name, arg_info)				ZEND_NS_FENTRY(ns, name, ZEND_FN(name), arg_info, ZEND_ACC_DEPRECATED)
#define ZEND_NS_FALIAS(ns, name, alias, arg_info)		ZEND_NS_FENTRY(ns, name, ZEND_FN(alias), arg_info, 0)
#define ZEND_NS_DEP_FALIAS(ns, name, alias, arg_info)	ZEND_NS_FENTRY(ns, name, ZEND_FN(alias), arg_info, ZEND_ACC_DEPRECATED)

#define ZEND_FE_END            { NULL, NULL, NULL, 0, 0 }

#define ZEND_ARG_INFO(pass_by_ref, name)                             { #name, NULL, 0, pass_by_ref, 0, 0 },
#define ZEND_ARG_PASS_INFO(pass_by_ref)                              { NULL,  NULL, 0, pass_by_ref, 0, 0 },
#define ZEND_ARG_OBJ_INFO(pass_by_ref, name, classname, allow_null)  { #name, #classname, IS_OBJECT, pass_by_ref, allow_null, 0 },
#define ZEND_ARG_ARRAY_INFO(pass_by_ref, name, allow_null)           { #name, NULL, IS_ARRAY, pass_by_ref, allow_null, 0 },
#define ZEND_ARG_CALLABLE_INFO(pass_by_ref, name, allow_null)        { #name, NULL, IS_CALLABLE, pass_by_ref, allow_null, 0 },
#define ZEND_ARG_TYPE_INFO(pass_by_ref, name, type_hint, allow_null) { #name, NULL, type_hint, pass_by_ref, allow_null, 0 },
#define ZEND_ARG_VARIADIC_INFO(pass_by_ref, name)                    { #name, NULL, 0, pass_by_ref, 0, 1 },


#define ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(name, return_reference, required_num_args, type, class_name, allow_null) \
	static const zend_internal_arg_info name[] = { \
	   	{ (const char*)(zend_uintptr_t)(required_num_args), class_name, type, return_reference, allow_null, 0 },
#define ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(name, type, class_name, allow_null) \
	ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(name, 0, -1, type, class_name, allow_null)

#define ZEND_BEGIN_ARG_INFO_EX(name, _unused, return_reference, required_num_args)	\
	static const zend_internal_arg_info name[] = { \
		{ (const char*)(zend_uintptr_t)(required_num_args), NULL, 0, return_reference, 0, 0 },
#define ZEND_BEGIN_ARG_INFO(name, _unused)	\
	ZEND_BEGIN_ARG_INFO_EX(name, 0, ZEND_RETURN_VALUE, -1)
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
#define ZEND_MODULE_STARTUP_D(module)		int ZEND_MODULE_STARTUP_N(module)(INIT_FUNC_ARGS)
#define ZEND_MODULE_SHUTDOWN_D(module)		int ZEND_MODULE_SHUTDOWN_N(module)(SHUTDOWN_FUNC_ARGS)
#define ZEND_MODULE_ACTIVATE_D(module)		int ZEND_MODULE_ACTIVATE_N(module)(INIT_FUNC_ARGS)
#define ZEND_MODULE_DEACTIVATE_D(module)	int ZEND_MODULE_DEACTIVATE_N(module)(SHUTDOWN_FUNC_ARGS)
#define ZEND_MODULE_POST_ZEND_DEACTIVATE_D(module)	int ZEND_MODULE_POST_ZEND_DEACTIVATE_N(module)(void)
#define ZEND_MODULE_INFO_D(module)			void ZEND_MODULE_INFO_N(module)(ZEND_MODULE_INFO_FUNC_ARGS)
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
#define ZEND_MODULE_GLOBALS_BULK(module_name) TSRMG_BULK(module_name##_globals_id, zend_##module_name##_globals *)

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
	INIT_OVERLOADED_CLASS_ENTRY(class_container, class_name, functions, NULL, NULL, NULL)

#define INIT_CLASS_ENTRY_EX(class_container, class_name, class_name_len, functions) \
	INIT_OVERLOADED_CLASS_ENTRY_EX(class_container, class_name, class_name_len, functions, NULL, NULL, NULL, NULL, NULL)

#define INIT_OVERLOADED_CLASS_ENTRY_EX(class_container, class_name, class_name_len, functions, handle_fcall, handle_propget, handle_propset, handle_propunset, handle_propisset) \
	{															\
		zend_string *cl_name;									\
		cl_name = zend_string_init(class_name, class_name_len, 1);		\
		class_container.name = zend_new_interned_string(cl_name);	\
		INIT_CLASS_ENTRY_INIT_METHODS(class_container, functions, handle_fcall, handle_propget, handle_propset, handle_propunset, handle_propisset) \
	}

#define INIT_CLASS_ENTRY_INIT_METHODS(class_container, functions, handle_fcall, handle_propget, handle_propset, handle_propunset, handle_propisset) \
	{															\
		class_container.constructor = NULL;						\
		class_container.destructor = NULL;						\
		class_container.clone = NULL;							\
		class_container.serialize = NULL;						\
		class_container.unserialize = NULL;						\
		class_container.create_object = NULL;					\
		class_container.interface_gets_implemented = NULL;		\
		class_container.get_static_method = NULL;				\
		class_container.__call = handle_fcall;					\
		class_container.__callstatic = NULL;					\
		class_container.__tostring = NULL;						\
		class_container.__get = handle_propget;					\
		class_container.__set = handle_propset;					\
		class_container.__unset = handle_propunset;				\
		class_container.__isset = handle_propisset;				\
		class_container.__debugInfo = NULL;					\
		class_container.serialize_func = NULL;					\
		class_container.unserialize_func = NULL;				\
		class_container.serialize = NULL;						\
		class_container.unserialize = NULL;						\
		class_container.parent = NULL;							\
		class_container.num_interfaces = 0;						\
		class_container.traits = NULL;							\
		class_container.num_traits = 0;							\
		class_container.trait_aliases = NULL;					\
		class_container.trait_precedences = NULL;				\
		class_container.interfaces = NULL;						\
		class_container.get_iterator = NULL;					\
		class_container.iterator_funcs.funcs = NULL;			\
		class_container.info.internal.module = NULL;			\
		class_container.info.internal.builtin_functions = functions;	\
	}

#define INIT_OVERLOADED_CLASS_ENTRY(class_container, class_name, functions, handle_fcall, handle_propget, handle_propset) \
	INIT_OVERLOADED_CLASS_ENTRY_EX(class_container, class_name, sizeof(class_name)-1, functions, handle_fcall, handle_propget, handle_propset, NULL, NULL)

#define INIT_NS_CLASS_ENTRY(class_container, ns, class_name, functions) \
	INIT_CLASS_ENTRY(class_container, ZEND_NS_NAME(ns, class_name), functions)
#define INIT_OVERLOADED_NS_CLASS_ENTRY_EX(class_container, ns, class_name, functions, handle_fcall, handle_propget, handle_propset, handle_propunset, handle_propisset) \
	INIT_OVERLOADED_CLASS_ENTRY_EX(class_container, ZEND_NS_NAME(ns, class_name), sizeof(ZEND_NS_NAME(ns, class_name))-1, functions, handle_fcall, handle_propget, handle_propset, handle_propunset, handle_propisset)
#define INIT_OVERLOADED_NS_CLASS_ENTRY(class_container, ns, class_name, functions, handle_fcall, handle_propget, handle_propset) \
	INIT_OVERLOADED_CLASS_ENTRY(class_container, ZEND_NS_NAME(ns, class_name), functions, handle_fcall, handle_propget, handle_propset)

#ifdef ZTS
#	define CE_STATIC_MEMBERS(ce) (((ce)->type==ZEND_USER_CLASS)?(ce)->static_members_table:CG(static_members_table)[(zend_intptr_t)(ce)->static_members_table])
#else
#	define CE_STATIC_MEMBERS(ce) ((ce)->static_members_table)
#endif

#define ZEND_FCI_INITIALIZED(fci) ((fci).size != 0)

ZEND_API int zend_next_free_module(void);

BEGIN_EXTERN_C()
ZEND_API int zend_get_parameters(int ht, int param_count, ...);
ZEND_API ZEND_ATTRIBUTE_DEPRECATED int zend_get_parameters_ex(int param_count, ...);
ZEND_API int _zend_get_parameters_array_ex(int param_count, zval *argument_array);

/* internal function to efficiently copy parameters when executing __call() */
ZEND_API int zend_copy_parameters_array(int param_count, zval *argument_array);

#define zend_get_parameters_array(ht, param_count, argument_array) \
	_zend_get_parameters_array_ex(param_count, argument_array)
#define zend_get_parameters_array_ex(param_count, argument_array) \
	_zend_get_parameters_array_ex(param_count, argument_array)
#define zend_parse_parameters_none() \
	(EXPECTED(ZEND_NUM_ARGS() == 0) ? SUCCESS : zend_parse_parameters(ZEND_NUM_ARGS(), ""))

/* Parameter parsing API -- andrei */

#define ZEND_PARSE_PARAMS_QUIET (1<<1)
#define ZEND_PARSE_PARAMS_THROW (1<<2)
ZEND_API int zend_parse_parameters(int num_args, const char *type_spec, ...);
ZEND_API int zend_parse_parameters_ex(int flags, int num_args, const char *type_spec, ...);
ZEND_API int zend_parse_parameters_throw(int num_args, const char *type_spec, ...);
ZEND_API char *zend_zval_type_name(const zval *arg);

ZEND_API int zend_parse_method_parameters(int num_args, zval *this_ptr, const char *type_spec, ...);
ZEND_API int zend_parse_method_parameters_ex(int flags, int num_args, zval *this_ptr, const char *type_spec, ...);

ZEND_API int zend_parse_parameter(int flags, int arg_num, zval *arg, const char *spec, ...);

/* End of parameter parsing API -- andrei */

ZEND_API int zend_register_functions(zend_class_entry *scope, const zend_function_entry *functions, HashTable *function_table, int type);
ZEND_API void zend_unregister_functions(const zend_function_entry *functions, int count, HashTable *function_table);
ZEND_API int zend_startup_module(zend_module_entry *module_entry);
ZEND_API zend_module_entry* zend_register_internal_module(zend_module_entry *module_entry);
ZEND_API zend_module_entry* zend_register_module_ex(zend_module_entry *module);
ZEND_API int zend_startup_module_ex(zend_module_entry *module);
ZEND_API int zend_startup_modules(void);
ZEND_API void zend_collect_module_handlers(void);
ZEND_API void zend_destroy_modules(void);
ZEND_API void zend_check_magic_method_implementation(const zend_class_entry *ce, const zend_function *fptr, int error_type);

ZEND_API zend_class_entry *zend_register_internal_class(zend_class_entry *class_entry);
ZEND_API zend_class_entry *zend_register_internal_class_ex(zend_class_entry *class_entry, zend_class_entry *parent_ce);
ZEND_API zend_class_entry *zend_register_internal_interface(zend_class_entry *orig_class_entry);
ZEND_API void zend_class_implements(zend_class_entry *class_entry, int num_interfaces, ...);

ZEND_API int zend_register_class_alias_ex(const char *name, size_t name_len, zend_class_entry *ce);

#define zend_register_class_alias(name, ce) \
	zend_register_class_alias_ex(name, sizeof(name)-1, ce)
#define zend_register_ns_class_alias(ns, name, ce) \
	zend_register_class_alias_ex(ZEND_NS_NAME(ns, name), sizeof(ZEND_NS_NAME(ns, name))-1, ce)

ZEND_API int zend_disable_function(char *function_name, size_t function_name_length);
ZEND_API int zend_disable_class(char *class_name, size_t class_name_length);

ZEND_API ZEND_COLD void zend_wrong_param_count(void);

#define IS_CALLABLE_CHECK_SYNTAX_ONLY (1<<0)
#define IS_CALLABLE_CHECK_NO_ACCESS   (1<<1)
#define IS_CALLABLE_CHECK_IS_STATIC   (1<<2)
#define IS_CALLABLE_CHECK_SILENT      (1<<3)

#define IS_CALLABLE_STRICT  (IS_CALLABLE_CHECK_IS_STATIC)

ZEND_API zend_bool zend_is_callable_ex(zval *callable, zend_object *object, uint check_flags, zend_string **callable_name, zend_fcall_info_cache *fcc, char **error);
ZEND_API zend_bool zend_is_callable(zval *callable, uint check_flags, zend_string **callable_name);
ZEND_API zend_bool zend_make_callable(zval *callable, zend_string **callable_name);
ZEND_API const char *zend_get_module_version(const char *module_name);
ZEND_API int zend_get_module_started(const char *module_name);
ZEND_API int zend_declare_property_ex(zend_class_entry *ce, zend_string *name, zval *property, int access_type, zend_string *doc_comment);
ZEND_API int zend_declare_property(zend_class_entry *ce, const char *name, size_t name_length, zval *property, int access_type);
ZEND_API int zend_declare_property_null(zend_class_entry *ce, const char *name, size_t name_length, int access_type);
ZEND_API int zend_declare_property_bool(zend_class_entry *ce, const char *name, size_t name_length, zend_long value, int access_type);
ZEND_API int zend_declare_property_long(zend_class_entry *ce, const char *name, size_t name_length, zend_long value, int access_type);
ZEND_API int zend_declare_property_double(zend_class_entry *ce, const char *name, size_t name_length, double value, int access_type);
ZEND_API int zend_declare_property_string(zend_class_entry *ce, const char *name, size_t name_length, const char *value, int access_type);
ZEND_API int zend_declare_property_stringl(zend_class_entry *ce, const char *name, size_t name_length, const char *value, size_t value_len, int access_type);

ZEND_API int zend_declare_class_constant(zend_class_entry *ce, const char *name, size_t name_length, zval *value);
ZEND_API int zend_declare_class_constant_null(zend_class_entry *ce, const char *name, size_t name_length);
ZEND_API int zend_declare_class_constant_long(zend_class_entry *ce, const char *name, size_t name_length, zend_long value);
ZEND_API int zend_declare_class_constant_bool(zend_class_entry *ce, const char *name, size_t name_length, zend_bool value);
ZEND_API int zend_declare_class_constant_double(zend_class_entry *ce, const char *name, size_t name_length, double value);
ZEND_API int zend_declare_class_constant_stringl(zend_class_entry *ce, const char *name, size_t name_length, const char *value, size_t value_length);
ZEND_API int zend_declare_class_constant_string(zend_class_entry *ce, const char *name, size_t name_length, const char *value);

ZEND_API int zend_update_class_constants(zend_class_entry *class_type);

ZEND_API void zend_update_property_ex(zend_class_entry *scope, zval *object, zend_string *name, zval *value);
ZEND_API void zend_update_property(zend_class_entry *scope, zval *object, const char *name, size_t name_length, zval *value);
ZEND_API void zend_update_property_null(zend_class_entry *scope, zval *object, const char *name, size_t name_length);
ZEND_API void zend_update_property_bool(zend_class_entry *scope, zval *object, const char *name, size_t name_length, zend_long value);
ZEND_API void zend_update_property_long(zend_class_entry *scope, zval *object, const char *name, size_t name_length, zend_long value);
ZEND_API void zend_update_property_double(zend_class_entry *scope, zval *object, const char *name, size_t name_length, double value);
ZEND_API void zend_update_property_str(zend_class_entry *scope, zval *object, const char *name, size_t name_length, zend_string *value);
ZEND_API void zend_update_property_string(zend_class_entry *scope, zval *object, const char *name, size_t name_length, const char *value);
ZEND_API void zend_update_property_stringl(zend_class_entry *scope, zval *object, const char *name, size_t name_length, const char *value, size_t value_length);

ZEND_API int zend_update_static_property(zend_class_entry *scope, const char *name, size_t name_length, zval *value);
ZEND_API int zend_update_static_property_null(zend_class_entry *scope, const char *name, size_t name_length);
ZEND_API int zend_update_static_property_bool(zend_class_entry *scope, const char *name, size_t name_length, zend_long value);
ZEND_API int zend_update_static_property_long(zend_class_entry *scope, const char *name, size_t name_length, zend_long value);
ZEND_API int zend_update_static_property_double(zend_class_entry *scope, const char *name, size_t name_length, double value);
ZEND_API int zend_update_static_property_string(zend_class_entry *scope, const char *name, size_t name_length, const char *value);
ZEND_API int zend_update_static_property_stringl(zend_class_entry *scope, const char *name, size_t name_length, const char *value, size_t value_length);

ZEND_API zval *zend_read_property(zend_class_entry *scope, zval *object, const char *name, size_t name_length, zend_bool silent, zval *rv);

ZEND_API zval *zend_read_static_property(zend_class_entry *scope, const char *name, size_t name_length, zend_bool silent);

ZEND_API char *zend_get_type_by_const(int type);

#define getThis()							(Z_OBJ(EX(This)) ? &EX(This) : NULL)
#define ZEND_IS_METHOD_CALL()				(EX(func)->common.scope != NULL)

#define WRONG_PARAM_COUNT					ZEND_WRONG_PARAM_COUNT()
#define WRONG_PARAM_COUNT_WITH_RETVAL(ret)	ZEND_WRONG_PARAM_COUNT_WITH_RETVAL(ret)
#define ARG_COUNT(dummy)					EX_NUM_ARGS()
#define ZEND_NUM_ARGS()						EX_NUM_ARGS()
#define ZEND_WRONG_PARAM_COUNT()					{ zend_wrong_param_count(); return; }
#define ZEND_WRONG_PARAM_COUNT_WITH_RETVAL(ret)		{ zend_wrong_param_count(); return ret; }

#ifndef ZEND_WIN32
#define DLEXPORT
#endif

#define array_init(arg)			_array_init((arg), 0 ZEND_FILE_LINE_CC)
#define array_init_size(arg, size) _array_init((arg), (size) ZEND_FILE_LINE_CC)
#define object_init(arg)		_object_init((arg) ZEND_FILE_LINE_CC)
#define object_init_ex(arg, ce)	_object_init_ex((arg), (ce) ZEND_FILE_LINE_CC)
#define object_and_properties_init(arg, ce, properties)	_object_and_properties_init((arg), (ce), (properties) ZEND_FILE_LINE_CC)
ZEND_API int _array_init(zval *arg, uint32_t size ZEND_FILE_LINE_DC);
ZEND_API int _object_init(zval *arg ZEND_FILE_LINE_DC);
ZEND_API int _object_init_ex(zval *arg, zend_class_entry *ce ZEND_FILE_LINE_DC);
ZEND_API int _object_and_properties_init(zval *arg, zend_class_entry *ce, HashTable *properties ZEND_FILE_LINE_DC);
ZEND_API void object_properties_init(zend_object *object, zend_class_entry *class_type);
ZEND_API void object_properties_init_ex(zend_object *object, HashTable *properties);
ZEND_API void object_properties_load(zend_object *object, HashTable *properties);

ZEND_API void zend_merge_properties(zval *obj, HashTable *properties);

ZEND_API int add_assoc_long_ex(zval *arg, const char *key, size_t key_len, zend_long n);
ZEND_API int add_assoc_null_ex(zval *arg, const char *key, size_t key_len);
ZEND_API int add_assoc_bool_ex(zval *arg, const char *key, size_t key_len, int b);
ZEND_API int add_assoc_resource_ex(zval *arg, const char *key, size_t key_len, zend_resource *r);
ZEND_API int add_assoc_double_ex(zval *arg, const char *key, size_t key_len, double d);
ZEND_API int add_assoc_str_ex(zval *arg, const char *key, size_t key_len, zend_string *str);
ZEND_API int add_assoc_string_ex(zval *arg, const char *key, size_t key_len, char *str);
ZEND_API int add_assoc_stringl_ex(zval *arg, const char *key, size_t key_len, char *str, size_t length);
ZEND_API int add_assoc_zval_ex(zval *arg, const char *key, size_t key_len, zval *value);

#define add_assoc_long(__arg, __key, __n) add_assoc_long_ex(__arg, __key, strlen(__key), __n)
#define add_assoc_null(__arg, __key) add_assoc_null_ex(__arg, __key, strlen(__key))
#define add_assoc_bool(__arg, __key, __b) add_assoc_bool_ex(__arg, __key, strlen(__key), __b)
#define add_assoc_resource(__arg, __key, __r) add_assoc_resource_ex(__arg, __key, strlen(__key), __r)
#define add_assoc_double(__arg, __key, __d) add_assoc_double_ex(__arg, __key, strlen(__key), __d)
#define add_assoc_str(__arg, __key, __str) add_assoc_str_ex(__arg, __key, strlen(__key), __str)
#define add_assoc_string(__arg, __key, __str) add_assoc_string_ex(__arg, __key, strlen(__key), __str)
#define add_assoc_stringl(__arg, __key, __str, __length) add_assoc_stringl_ex(__arg, __key, strlen(__key), __str, __length)
#define add_assoc_zval(__arg, __key, __value) add_assoc_zval_ex(__arg, __key, strlen(__key), __value)

/* unset() functions are only suported for legacy modules and null() functions should be used */
#define add_assoc_unset(__arg, __key) add_assoc_null_ex(__arg, __key, strlen(__key))
#define add_index_unset(__arg, __key) add_index_null(__arg, __key)
#define add_next_index_unset(__arg) add_next_index_null(__arg)
#define add_property_unset(__arg, __key) add_property_null(__arg, __key)

ZEND_API int add_index_long(zval *arg, zend_ulong idx, zend_long n);
ZEND_API int add_index_null(zval *arg, zend_ulong idx);
ZEND_API int add_index_bool(zval *arg, zend_ulong idx, int b);
ZEND_API int add_index_resource(zval *arg, zend_ulong idx, zend_resource *r);
ZEND_API int add_index_double(zval *arg, zend_ulong idx, double d);
ZEND_API int add_index_str(zval *arg, zend_ulong idx, zend_string *str);
ZEND_API int add_index_string(zval *arg, zend_ulong idx, const char *str);
ZEND_API int add_index_stringl(zval *arg, zend_ulong idx, const char *str, size_t length);
ZEND_API int add_index_zval(zval *arg, zend_ulong index, zval *value);

ZEND_API int add_next_index_long(zval *arg, zend_long n);
ZEND_API int add_next_index_null(zval *arg);
ZEND_API int add_next_index_bool(zval *arg, int b);
ZEND_API int add_next_index_resource(zval *arg, zend_resource *r);
ZEND_API int add_next_index_double(zval *arg, double d);
ZEND_API int add_next_index_str(zval *arg, zend_string *str);
ZEND_API int add_next_index_string(zval *arg, const char *str);
ZEND_API int add_next_index_stringl(zval *arg, const char *str, size_t length);
ZEND_API int add_next_index_zval(zval *arg, zval *value);

ZEND_API zval *add_get_assoc_string_ex(zval *arg, const char *key, uint key_len, const char *str);
ZEND_API zval *add_get_assoc_stringl_ex(zval *arg, const char *key, uint key_len, const char *str, size_t length);

#define add_get_assoc_string(__arg, __key, __str) add_get_assoc_string_ex(__arg, __key, strlen(__key), __str)
#define add_get_assoc_stringl(__arg, __key, __str, __length) add_get_assoc_stringl_ex(__arg, __key, strlen(__key), __str, __length)

ZEND_API zval *add_get_index_long(zval *arg, zend_ulong idx, zend_long l);
ZEND_API zval *add_get_index_double(zval *arg, zend_ulong idx, double d);
ZEND_API zval *add_get_index_str(zval *arg, zend_ulong index, zend_string *str);
ZEND_API zval *add_get_index_string(zval *arg, zend_ulong idx, const char *str);
ZEND_API zval *add_get_index_stringl(zval *arg, zend_ulong idx, const char *str, size_t length);

ZEND_API int array_set_zval_key(HashTable *ht, zval *key, zval *value);

ZEND_API int add_property_long_ex(zval *arg, const char *key, size_t key_len, zend_long l);
ZEND_API int add_property_null_ex(zval *arg, const char *key, size_t key_len);
ZEND_API int add_property_bool_ex(zval *arg, const char *key, size_t key_len, zend_long b);
ZEND_API int add_property_resource_ex(zval *arg, const char *key, size_t key_len, zend_resource *r);
ZEND_API int add_property_double_ex(zval *arg, const char *key, size_t key_len, double d);
ZEND_API int add_property_str_ex(zval *arg, const char *key, size_t key_len, zend_string *str);
ZEND_API int add_property_string_ex(zval *arg, const char *key, size_t key_len, const char *str);
ZEND_API int add_property_stringl_ex(zval *arg, const char *key, size_t key_len,  const char *str, size_t length);
ZEND_API int add_property_zval_ex(zval *arg, const char *key, size_t key_len, zval *value);

#define add_property_long(__arg, __key, __n) add_property_long_ex(__arg, __key, strlen(__key), __n)
#define add_property_null(__arg, __key) add_property_null_ex(__arg, __key, strlen(__key))
#define add_property_bool(__arg, __key, __b) add_property_bool_ex(__arg, __key, strlen(__key), __b)
#define add_property_resource(__arg, __key, __r) add_property_resource_ex(__arg, __key, strlen(__key), __r)
#define add_property_double(__arg, __key, __d) add_property_double_ex(__arg, __key, strlen(__key), __d)
#define add_property_str(__arg, __key, __str) add_property_str_ex(__arg, __key, strlen(__key), __str)
#define add_property_string(__arg, __key, __str) add_property_string_ex(__arg, __key, strlen(__key), __str)
#define add_property_stringl(__arg, __key, __str, __length) add_property_stringl_ex(__arg, __key, strlen(__key), __str, __length)
#define add_property_zval(__arg, __key, __value) add_property_zval_ex(__arg, __key, strlen(__key), __value)


ZEND_API int call_user_function(HashTable *function_table, zval *object, zval *function_name, zval *retval_ptr, uint32_t param_count, zval params[]);
ZEND_API int call_user_function_ex(HashTable *function_table, zval *object, zval *function_name, zval *retval_ptr, uint32_t param_count, zval params[], int no_separation, zend_array *symbol_table);

ZEND_API extern const zend_fcall_info empty_fcall_info;
ZEND_API extern const zend_fcall_info_cache empty_fcall_info_cache;

/** Build zend_call_info/cache from a zval*
 *
 * Caller is responsible to provide a return value (fci->retval), otherwise the we will crash.
 * In order to pass parameters the following members need to be set:
 * fci->param_count = 0;
 * fci->params = NULL;
 * The callable_name argument may be NULL.
 * Set check_flags to IS_CALLABLE_STRICT for every new usage!
 */
ZEND_API int zend_fcall_info_init(zval *callable, uint check_flags, zend_fcall_info *fci, zend_fcall_info_cache *fcc, zend_string **callable_name, char **error);

/** Clear arguments connected with zend_fcall_info *fci
 * If free_mem is not zero then the params array gets free'd as well
 */
ZEND_API void zend_fcall_info_args_clear(zend_fcall_info *fci, int free_mem);

/** Save current arguments from zend_fcall_info *fci
 * params array will be set to NULL
 */
ZEND_API void zend_fcall_info_args_save(zend_fcall_info *fci, int *param_count, zval **params);

/** Free arguments connected with zend_fcall_info *fci andset back saved ones.
 */
ZEND_API void zend_fcall_info_args_restore(zend_fcall_info *fci, int param_count, zval *params);

/** Set or clear the arguments in the zend_call_info struct taking care of
 * refcount. If args is NULL and arguments are set then those are cleared.
 */
ZEND_API int zend_fcall_info_args(zend_fcall_info *fci, zval *args);
ZEND_API int zend_fcall_info_args_ex(zend_fcall_info *fci, zend_function *func, zval *args);

/** Set arguments in the zend_fcall_info struct taking care of refcount.
 * If argc is 0 the arguments which are set will be cleared, else pass
 * a variable amount of zval** arguments.
 */
ZEND_API int zend_fcall_info_argp(zend_fcall_info *fci, int argc, zval *argv);

/** Set arguments in the zend_fcall_info struct taking care of refcount.
 * If argc is 0 the arguments which are set will be cleared, else pass
 * a variable amount of zval** arguments.
 */
ZEND_API int zend_fcall_info_argv(zend_fcall_info *fci, int argc, va_list *argv);

/** Set arguments in the zend_fcall_info struct taking care of refcount.
 * If argc is 0 the arguments which are set will be cleared, else pass
 * a variable amount of zval** arguments.
 */
ZEND_API int zend_fcall_info_argn(zend_fcall_info *fci, int argc, ...);

/** Call a function using information created by zend_fcall_info_init()/args().
 * If args is given then those replace the argument info in fci is temporarily.
 */
ZEND_API int zend_fcall_info_call(zend_fcall_info *fci, zend_fcall_info_cache *fcc, zval *retval, zval *args);

ZEND_API int zend_call_function(zend_fcall_info *fci, zend_fcall_info_cache *fci_cache);

ZEND_API int zend_set_hash_symbol(zval *symbol, const char *name, int name_length, zend_bool is_ref, int num_symbol_tables, ...);

ZEND_API int zend_delete_global_variable(zend_string *name);

ZEND_API zend_array *zend_rebuild_symbol_table(void);
ZEND_API void zend_attach_symbol_table(zend_execute_data *execute_data);
ZEND_API void zend_detach_symbol_table(zend_execute_data *execute_data);
ZEND_API int zend_set_local_var(zend_string *name, zval *value, int force);
ZEND_API int zend_set_local_var_str(const char *name, size_t len, zval *value, int force);

ZEND_API zend_string *zend_find_alias_name(zend_class_entry *ce, zend_string *name);
ZEND_API zend_string *zend_resolve_method_name(zend_class_entry *ce, zend_function *f);

ZEND_API const char *zend_get_object_type(const zend_class_entry *ce);

#define add_method(arg, key, method)	add_assoc_function((arg), (key), (method))

ZEND_API ZEND_FUNCTION(display_disabled_function);
ZEND_API ZEND_FUNCTION(display_disabled_class);
END_EXTERN_C()

#if ZEND_DEBUG
#define CHECK_ZVAL_STRING(str) \
	if (ZSTR_VAL(str)[ZSTR_LEN(str)] != '\0') { zend_error(E_WARNING, "String is not zero-terminated (%s)", ZSTR_VAL(str)); }
#define CHECK_ZVAL_STRING_REL(str) \
	if (ZSTR_VAL(str)[ZSTR_LEN(str)] != '\0') { zend_error(E_WARNING, "String is not zero-terminated (%s) (source: %s:%d)", ZSTR_VAL(str) ZEND_FILE_LINE_RELAY_CC); }
#else
#define CHECK_ZVAL_STRING(z)
#define CHECK_ZVAL_STRING_REL(z)
#endif

#define CHECK_ZVAL_NULL_PATH(p) (Z_STRLEN_P(p) != strlen(Z_STRVAL_P(p)))
#define CHECK_NULL_PATH(p, l) (strlen(p) != l)

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
#define RETVAL_NULL() 					ZVAL_NULL(return_value)
#define RETVAL_LONG(l) 					ZVAL_LONG(return_value, l)
#define RETVAL_DOUBLE(d) 				ZVAL_DOUBLE(return_value, d)
#define RETVAL_STR(s)			 		ZVAL_STR(return_value, s)
#define RETVAL_INTERNED_STR(s)	 		ZVAL_INTERNED_STR(return_value, s)
#define RETVAL_NEW_STR(s)		 		ZVAL_NEW_STR(return_value, s)
#define RETVAL_STR_COPY(s)		 		ZVAL_STR_COPY(return_value, s)
#define RETVAL_STRING(s)		 		ZVAL_STRING(return_value, s)
#define RETVAL_STRINGL(s, l)		 	ZVAL_STRINGL(return_value, s, l)
#define RETVAL_EMPTY_STRING() 			ZVAL_EMPTY_STRING(return_value)
#define RETVAL_RES(r)			 		ZVAL_RES(return_value, r)
#define RETVAL_ARR(r)			 		ZVAL_ARR(return_value, r)
#define RETVAL_OBJ(r)			 		ZVAL_OBJ(return_value, r)
#define RETVAL_ZVAL(zv, copy, dtor)		ZVAL_ZVAL(return_value, zv, copy, dtor)
#define RETVAL_FALSE  					ZVAL_FALSE(return_value)
#define RETVAL_TRUE   					ZVAL_TRUE(return_value)

#define RETURN_BOOL(b) 					{ RETVAL_BOOL(b); return; }
#define RETURN_NULL() 					{ RETVAL_NULL(); return;}
#define RETURN_LONG(l) 					{ RETVAL_LONG(l); return; }
#define RETURN_DOUBLE(d) 				{ RETVAL_DOUBLE(d); return; }
#define RETURN_STR(s) 					{ RETVAL_STR(s); return; }
#define RETURN_INTERNED_STR(s)			{ RETVAL_INTERNED_STR(s); return; }
#define RETURN_NEW_STR(s)				{ RETVAL_NEW_STR(s); return; }
#define RETURN_STR_COPY(s)				{ RETVAL_STR_COPY(s); return; }
#define RETURN_STRING(s) 				{ RETVAL_STRING(s); return; }
#define RETURN_STRINGL(s, l) 			{ RETVAL_STRINGL(s, l); return; }
#define RETURN_EMPTY_STRING() 			{ RETVAL_EMPTY_STRING(); return; }
#define RETURN_RES(r) 					{ RETVAL_RES(r); return; }
#define RETURN_ARR(r) 					{ RETVAL_ARR(r); return; }
#define RETURN_OBJ(r) 					{ RETVAL_OBJ(r); return; }
#define RETURN_ZVAL(zv, copy, dtor)		{ RETVAL_ZVAL(zv, copy, dtor); return; }
#define RETURN_FALSE  					{ RETVAL_FALSE; return; }
#define RETURN_TRUE   					{ RETVAL_TRUE; return; }

#define HASH_OF(p) (Z_TYPE_P(p)==IS_ARRAY ? Z_ARRVAL_P(p) : ((Z_TYPE_P(p)==IS_OBJECT ? Z_OBJ_HT_P(p)->get_properties((p)) : NULL)))
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

/* Fast parameter parsing API */

/* This API should be used only for really often used functions.
 * (Keep the original parsing code and wrap usage with #ifndef FAST_ZPP)
 */
#define FAST_ZPP 1

#ifdef FAST_ZPP

#define Z_EXPECTED_TYPES(_) \
	_(Z_EXPECTED_LONG,		"integer") \
	_(Z_EXPECTED_BOOL,		"boolean") \
	_(Z_EXPECTED_STRING,	"string") \
	_(Z_EXPECTED_ARRAY,		"array") \
	_(Z_EXPECTED_FUNC,		"valid callback") \
	_(Z_EXPECTED_RESOURCE,	"resource") \
	_(Z_EXPECTED_PATH,		"a valid path") \
	_(Z_EXPECTED_OBJECT,	"object") \
	_(Z_EXPECTED_DOUBLE,	"float")

#define Z_EXPECTED_TYPE_ENUM(id, str) id,
#define Z_EXPECTED_TYPE_STR(id, str)  str,

typedef enum _zend_expected_type {
	Z_EXPECTED_TYPES(Z_EXPECTED_TYPE_ENUM)
	Z_EXPECTED_LAST
} zend_expected_type;

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_paramers_count_error(int num_args, int min_num_args, int max_num_args);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_paramer_type_error(int num, zend_expected_type expected_type, zval *arg);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_paramer_class_error(int num, char *name, zval *arg);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_callback_error(int severity, int num, char *error);

#define ZPP_ERROR_OK             0
#define ZPP_ERROR_FAILURE        1
#define ZPP_ERROR_WRONG_CALLBACK 2
#define ZPP_ERROR_WRONG_CLASS    3
#define ZPP_ERROR_WRONG_ARG      4
#define ZPP_ERROR_WRONG_COUNT    5

#define ZEND_PARSE_PARAMETERS_START_EX(flags, min_num_args, max_num_args) do { \
		const int _flags = (flags); \
		int _min_num_args = (min_num_args); \
		int _max_num_args = (max_num_args); \
		int _num_args = EX_NUM_ARGS(); \
		int _i; \
		zval *_real_arg, *_arg = NULL; \
		zend_expected_type _expected_type = IS_UNDEF; \
		char *_error = NULL; \
		zend_bool _dummy; \
		zend_bool _optional = 0; \
		int error_code = ZPP_ERROR_OK; \
		((void)_i); \
		((void)_real_arg); \
		((void)_arg); \
		((void)_expected_type); \
		((void)_error); \
		((void)_dummy); \
		((void)_optional); \
		\
		do { \
			if (UNEXPECTED(_num_args < _min_num_args) || \
			    (UNEXPECTED(_num_args > _max_num_args) && \
			     EXPECTED(_max_num_args >= 0))) { \
				if (!(_flags & ZEND_PARSE_PARAMS_QUIET)) { \
					zend_wrong_paramers_count_error(_num_args, _min_num_args, _max_num_args); \
				} \
				error_code = ZPP_ERROR_FAILURE; \
				break; \
			} \
			_i = 0; \
			_real_arg = ZEND_CALL_ARG(execute_data, 0);

#define ZEND_PARSE_PARAMETERS_START(min_num_args, max_num_args) \
	ZEND_PARSE_PARAMETERS_START_EX(0, min_num_args, max_num_args)

#define ZEND_PARSE_PARAMETERS_END_EX(failure) \
		} while (0); \
		if (UNEXPECTED(error_code != ZPP_ERROR_OK)) { \
			if (!(_flags & ZEND_PARSE_PARAMS_QUIET)) { \
				if (error_code == ZPP_ERROR_WRONG_CALLBACK) { \
					zend_wrong_callback_error(E_WARNING, _i, _error); \
				} else if (error_code == ZPP_ERROR_WRONG_CLASS) { \
					zend_wrong_paramer_class_error(_i, _error, _arg); \
				} else if (error_code == ZPP_ERROR_WRONG_ARG) { \
					zend_wrong_paramer_type_error(_i, _expected_type, _arg); \
				} \
			} \
			failure; \
		} \
	} while (0)

#define ZEND_PARSE_PARAMETERS_END() \
	ZEND_PARSE_PARAMETERS_END_EX(return)

#define Z_PARAM_PROLOGUE(separate) \
	++_i; \
	ZEND_ASSERT(_i <= _min_num_args || _optional==1); \
	ZEND_ASSERT(_i >  _min_num_args || _optional==0); \
	if (_optional) { \
		if (UNEXPECTED(_i >_num_args)) break; \
	} \
	_real_arg++; \
	_arg = _real_arg; \
	ZVAL_DEREF(_arg); \
	if (separate) { \
		SEPARATE_ZVAL_NOREF(_arg); \
	}

/* old "|" */
#define Z_PARAM_OPTIONAL \
	_optional = 1;

/* old "a" */
#define Z_PARAM_ARRAY_EX(dest, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		if (UNEXPECTED(!zend_parse_arg_array(_arg, &dest, check_null, 0))) { \
			_expected_type = Z_EXPECTED_ARRAY; \
			error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_ARRAY(dest) \
	Z_PARAM_ARRAY_EX(dest, 0, 0)

/* old "A" */
#define Z_PARAM_ARRAY_OR_OBJECT_EX(dest, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		if (UNEXPECTED(!zend_parse_arg_array(_arg, &dest, check_null, 1))) { \
			_expected_type = Z_EXPECTED_ARRAY; \
			error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_ARRAY_OR_OBJECT(dest, check_null, separate) \
	Z_PARAM_ARRAY_OR_OBJECT_EX(dest, 0, 0)

/* old "b" */
#define Z_PARAM_BOOL_EX(dest, is_null, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		if (UNEXPECTED(!zend_parse_arg_bool(_arg, &dest, &is_null, check_null))) { \
			_expected_type = Z_EXPECTED_BOOL; \
			error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_BOOL(dest) \
	Z_PARAM_BOOL_EX(dest, _dummy, 0, 0)

/* old "C" */
#define Z_PARAM_CLASS_EX(dest, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		if (UNEXPECTED(!zend_parse_arg_class(_arg, &dest, _i, check_null))) { \
			error_code = ZPP_ERROR_FAILURE; \
			break; \
		}

#define Z_PARAM_CLASS(dest) \
	Z_PARAM_CLASS_EX(dest, 0, 0)

/* old "d" */
#define Z_PARAM_DOUBLE_EX(dest, is_null, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		if (UNEXPECTED(!zend_parse_arg_double(_arg, &dest, &is_null, check_null))) { \
			_expected_type = Z_EXPECTED_DOUBLE; \
			error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_DOUBLE(dest) \
	Z_PARAM_DOUBLE_EX(dest, _dummy, 0, 0)

/* old "f" */
#define Z_PARAM_FUNC_EX(dest_fci, dest_fcc, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		if (UNEXPECTED(!zend_parse_arg_func(_arg, &dest_fci, &dest_fcc, check_null, &_error))) { \
			if (!_error) { \
				_expected_type = Z_EXPECTED_FUNC; \
				error_code = ZPP_ERROR_WRONG_ARG; \
				break; \
			} else { \
				error_code = ZPP_ERROR_WRONG_CALLBACK; \
				break; \
			} \
		} else if (UNEXPECTED(_error != NULL)) { \
			zend_wrong_callback_error(E_DEPRECATED, _i, _error); \
		}

#define Z_PARAM_FUNC(dest_fci, dest_fcc) \
	Z_PARAM_FUNC_EX(dest_fci, dest_fcc, 0, 0)

/* old "h" */
#define Z_PARAM_ARRAY_HT_EX(dest, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		if (UNEXPECTED(!zend_parse_arg_array_ht(_arg, &dest, check_null, 0))) { \
			_expected_type = Z_EXPECTED_ARRAY; \
			error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_ARRAY_HT(dest) \
	Z_PARAM_ARRAY_HT_EX(dest, 0, 0)

/* old "H" */
#define Z_PARAM_ARRAY_OR_OBJECT_HT_EX(dest, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		if (UNEXPECTED(!zend_parse_arg_array_ht(_arg, &dest, check_null, 1))) { \
			_expected_type = Z_EXPECTED_ARRAY; \
			error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_ARRAY_OR_OBJECT_HT(dest) \
	Z_PARAM_ARRAY_OR_OBJECT_HT_EX(dest, 0, 0)

/* old "l" */
#define Z_PARAM_LONG_EX(dest, is_null, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		if (UNEXPECTED(!zend_parse_arg_long(_arg, &dest, &is_null, check_null, 0))) { \
			_expected_type = Z_EXPECTED_LONG; \
			error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_LONG(dest) \
	Z_PARAM_LONG_EX(dest, _dummy, 0, 0)

/* old "L" */
#define Z_PARAM_STRICT_LONG_EX(dest, is_null, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		if (UNEXPECTED(!zend_parse_arg_long(_arg, &dest, &is_null, check_null, 1))) { \
			_expected_type = Z_EXPECTED_LONG; \
			error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_STRICT_LONG(dest) \
	Z_PARAM_STRICT_LONG_EX(dest, _dummy, 0, 0)

/* old "o" */
#define Z_PARAM_OBJECT_EX(dest, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		if (UNEXPECTED(!zend_parse_arg_object(_arg, &dest, NULL, check_null))) { \
			_expected_type = Z_EXPECTED_OBJECT; \
			error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_OBJECT(dest) \
	Z_PARAM_OBJECT_EX(dest, 0, 0)

/* old "O" */
#define Z_PARAM_OBJECT_OF_CLASS_EX(dest, _ce, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		if (UNEXPECTED(!zend_parse_arg_object(_arg, &dest, _ce, check_null))) { \
			if (_ce) { \
				_error = ZSTR_VAL((_ce)->name); \
				error_code = ZPP_ERROR_WRONG_CLASS; \
				break; \
			} else { \
				_expected_type = Z_EXPECTED_OBJECT; \
				error_code = ZPP_ERROR_WRONG_ARG; \
				break; \
			} \
		}

#define Z_PARAM_OBJECT_OF_CLASS(dest, _ce) \
	Z_PARAM_OBJECT_OF_CLASS_EX(dest, _ce, 0, 0)

/* old "p" */
#define Z_PARAM_PATH_EX(dest, dest_len, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		if (UNEXPECTED(!zend_parse_arg_path(_arg, &dest, &dest_len, check_null))) { \
			_expected_type = Z_EXPECTED_PATH; \
			error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_PATH(dest, dest_len) \
	Z_PARAM_PATH_EX(dest, dest_len, 0, 0)

/* old "P" */
#define Z_PARAM_PATH_STR_EX(dest, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		if (UNEXPECTED(!zend_parse_arg_path_str(_arg, &dest, check_null))) { \
			_expected_type = Z_EXPECTED_PATH; \
			error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_PATH_STR(dest) \
	Z_PARAM_PATH_STR_EX(dest, 0, 0)

/* old "r" */
#define Z_PARAM_RESOURCE_EX(dest, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		if (UNEXPECTED(!zend_parse_arg_resource(_arg, &dest, check_null))) { \
			_expected_type = Z_EXPECTED_RESOURCE; \
			error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_RESOURCE(dest) \
	Z_PARAM_RESOURCE_EX(dest, 0, 0)

/* old "s" */
#define Z_PARAM_STRING_EX(dest, dest_len, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		if (UNEXPECTED(!zend_parse_arg_string(_arg, &dest, &dest_len, check_null))) { \
			_expected_type = Z_EXPECTED_STRING; \
			error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_STRING(dest, dest_len) \
	Z_PARAM_STRING_EX(dest, dest_len, 0, 0)

/* old "S" */
#define Z_PARAM_STR_EX(dest, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		if (UNEXPECTED(!zend_parse_arg_str(_arg, &dest, check_null))) { \
			_expected_type = Z_EXPECTED_STRING; \
			error_code = ZPP_ERROR_WRONG_ARG; \
			break; \
		}

#define Z_PARAM_STR(dest) \
	Z_PARAM_STR_EX(dest, 0, 0)

/* old "z" */
#define Z_PARAM_ZVAL_EX(dest, check_null, separate) \
		if (separate) { \
			Z_PARAM_PROLOGUE(separate); \
			zend_parse_arg_zval_deref(_arg, &dest, check_null); \
		} else { \
			++_i; \
			ZEND_ASSERT(_i <= _min_num_args || _optional==1); \
			ZEND_ASSERT(_i >  _min_num_args || _optional==0); \
			if (_optional && UNEXPECTED(_i >_num_args)) break; \
			_real_arg++; \
			zend_parse_arg_zval(_real_arg, &dest, check_null); \
		}

#define Z_PARAM_ZVAL(dest) \
	Z_PARAM_ZVAL_EX(dest, 0, 0)

/* old "z" (with dereference) */
#define Z_PARAM_ZVAL_DEREF_EX(dest, check_null, separate) \
		Z_PARAM_PROLOGUE(separate); \
		zend_parse_arg_zval_deref(_arg, &dest, check_null);

#define Z_PARAM_ZVAL_DEREF(dest) \
	Z_PARAM_ZVAL_DEREF_EX(dest, 0, 0)

/* old "+" and "*" */
#define Z_PARAM_VARIADIC_EX(spec, dest, dest_num, post_varargs) do { \
		int _num_varargs = _num_args - _i - (post_varargs); \
		if (EXPECTED(_num_varargs > 0)) { \
			dest = _real_arg + 1; \
			dest_num = _num_varargs; \
			_i += _num_varargs; \
			_real_arg += _num_varargs; \
		} else { \
			dest = NULL; \
			dest_num = 0; \
		} \
	} while (0);

#define Z_PARAM_VARIADIC(spec, dest, dest_num) \
	Z_PARAM_VARIADIC_EX(spec, dest, dest_num, 0)

#endif /* FAST_ZPP */

/* End of new parameter parsing API */

/* Inlined implementations shared by new and old parameter parsing APIs */

ZEND_API int ZEND_FASTCALL zend_parse_arg_class(zval *arg, zend_class_entry **pce, int num, int check_null);
ZEND_API int ZEND_FASTCALL zend_parse_arg_bool_slow(zval *arg, zend_bool *dest);
ZEND_API int ZEND_FASTCALL zend_parse_arg_bool_weak(zval *arg, zend_bool *dest);
ZEND_API int ZEND_FASTCALL zend_parse_arg_long_slow(zval *arg, zend_long *dest);
ZEND_API int ZEND_FASTCALL zend_parse_arg_long_weak(zval *arg, zend_long *dest);
ZEND_API int ZEND_FASTCALL zend_parse_arg_long_cap_slow(zval *arg, zend_long *dest);
ZEND_API int ZEND_FASTCALL zend_parse_arg_long_cap_weak(zval *arg, zend_long *dest);
ZEND_API int ZEND_FASTCALL zend_parse_arg_double_slow(zval *arg, double *dest);
ZEND_API int ZEND_FASTCALL zend_parse_arg_double_weak(zval *arg, double *dest);
ZEND_API int ZEND_FASTCALL zend_parse_arg_str_slow(zval *arg, zend_string **dest);
ZEND_API int ZEND_FASTCALL zend_parse_arg_str_weak(zval *arg, zend_string **dest);

static zend_always_inline int zend_parse_arg_bool(zval *arg, zend_bool *dest, zend_bool *is_null, int check_null)
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
		return zend_parse_arg_bool_slow(arg, dest);
	}
	return 1;
}

static zend_always_inline int zend_parse_arg_long(zval *arg, zend_long *dest, zend_bool *is_null, int check_null, int cap)
{
	if (check_null) {
		*is_null = 0;
	}
	if (EXPECTED(Z_TYPE_P(arg) == IS_LONG)) {
		*dest = Z_LVAL_P(arg);
	} else if (check_null && Z_TYPE_P(arg) == IS_NULL) {
		*is_null = 1;
		*dest = 0;
	} else if (cap) {
		return zend_parse_arg_long_cap_slow(arg, dest);
	} else {
		return zend_parse_arg_long_slow(arg, dest);
	}
	return 1;
}

static zend_always_inline int zend_parse_arg_double(zval *arg, double *dest, zend_bool *is_null, int check_null)
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
		return zend_parse_arg_double_slow(arg, dest);
	}
	return 1;
}

static zend_always_inline int zend_parse_arg_str(zval *arg, zend_string **dest, int check_null)
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_STRING)) {
		*dest = Z_STR_P(arg);
	} else if (check_null && Z_TYPE_P(arg) == IS_NULL) {
		*dest = NULL;
	} else {
		return zend_parse_arg_str_slow(arg, dest);
	}
	return 1;
}

static zend_always_inline int zend_parse_arg_string(zval *arg, char **dest, size_t *dest_len, int check_null)
{
	zend_string *str;

	if (!zend_parse_arg_str(arg, &str, check_null)) {
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

static zend_always_inline int zend_parse_arg_path_str(zval *arg, zend_string **dest, int check_null)
{
	if (!zend_parse_arg_str(arg, dest, check_null) ||
	    (*dest && UNEXPECTED(CHECK_NULL_PATH(ZSTR_VAL(*dest), ZSTR_LEN(*dest))))) {
		return 0;
	}
	return 1;
}

static zend_always_inline int zend_parse_arg_path(zval *arg, char **dest, size_t *dest_len, int check_null)
{
	zend_string *str;

	if (!zend_parse_arg_path_str(arg, &str, check_null)) {
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

static zend_always_inline int zend_parse_arg_array(zval *arg, zval **dest, int check_null, int or_object)
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

static zend_always_inline int zend_parse_arg_array_ht(zval *arg, HashTable **dest, int check_null, int or_object)
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_ARRAY)) {
		*dest = Z_ARRVAL_P(arg);
	} else if (or_object && EXPECTED(Z_TYPE_P(arg) == IS_OBJECT)) {
		*dest = Z_OBJ_HT_P(arg)->get_properties(arg);
	} else if (check_null && EXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		*dest = NULL;
	} else {
		return 0;
	}
	return 1;
}

static zend_always_inline int zend_parse_arg_object(zval *arg, zval **dest, zend_class_entry *ce, int check_null)
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

static zend_always_inline int zend_parse_arg_resource(zval *arg, zval **dest, int check_null)
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

static zend_always_inline int zend_parse_arg_func(zval *arg, zend_fcall_info *dest_fci, zend_fcall_info_cache *dest_fcc, int check_null, char **error)
{
	if (check_null && UNEXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		dest_fci->size = 0;
		dest_fcc->initialized = 0;
		*error = NULL;
	} else if (UNEXPECTED(zend_fcall_info_init(arg, 0, dest_fci, dest_fcc, NULL, error) != SUCCESS)) {
		return 0;
	}
	return 1;
}

static zend_always_inline void zend_parse_arg_zval(zval *arg, zval **dest, int check_null)
{
	*dest = (check_null &&
	    (UNEXPECTED(Z_TYPE_P(arg) == IS_NULL) ||
	     (UNEXPECTED(Z_ISREF_P(arg)) &&
	      UNEXPECTED(Z_TYPE_P(Z_REFVAL_P(arg)) == IS_NULL)))) ? NULL : arg;
}

static zend_always_inline void zend_parse_arg_zval_deref(zval *arg, zval **dest, int check_null)
{
	*dest = (check_null && UNEXPECTED(Z_TYPE_P(arg) == IS_NULL)) ? NULL : arg;
}

END_EXTERN_C()

#endif /* ZEND_API_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
