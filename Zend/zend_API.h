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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Andrei Zmievski <andrei@php.net>                            |
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
	const struct _zend_arg_info *arg_info;
	zend_uint num_args;
	zend_uint flags;
} zend_function_entry;

typedef struct _zend_fcall_info {
	size_t size;
	HashTable *function_table;
	zval *function_name;
	HashTable *symbol_table;
	zval **retval_ptr_ptr;
	zend_uint param_count;
	zval ***params;
	zval *object_ptr;
	zend_bool no_separation;
} zend_fcall_info;

typedef struct _zend_fcall_info_cache {
	zend_bool initialized;
	zend_function *function_handler;
	zend_class_entry *calling_scope;
	zend_class_entry *called_scope;
	zval *object_ptr;
} zend_fcall_info_cache;

#define ZEND_NS_NAME(ns, name)			ns"\\"name

#define ZEND_FN(name) zif_##name
#define ZEND_MN(name) zim_##name
#define ZEND_NAMED_FUNCTION(name)		void name(INTERNAL_FUNCTION_PARAMETERS)
#define ZEND_FUNCTION(name)				ZEND_NAMED_FUNCTION(ZEND_FN(name))
#define ZEND_METHOD(classname, name)	ZEND_NAMED_FUNCTION(ZEND_MN(classname##_##name))

#define ZEND_FENTRY(zend_name, name, arg_info, flags)	{ #zend_name, name, arg_info, (zend_uint) (sizeof(arg_info)/sizeof(struct _zend_arg_info)-1), flags },

#define ZEND_RAW_FENTRY(zend_name, name, arg_info, flags)   { zend_name, name, arg_info, (zend_uint) (sizeof(arg_info)/sizeof(struct _zend_arg_info)-1), flags },
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

#define ZEND_NS_NAMED_FE(ns, zend_name, name, arg_info)	ZEND_NS_FENTRY(ns, #zend_name, name, arg_info, 0)
#define ZEND_NS_FE(ns, name, arg_info)					ZEND_NS_FENTRY(ns, name, ZEND_FN(name), arg_info, 0)
#define ZEND_NS_DEP_FE(ns, name, arg_info)				ZEND_NS_FENTRY(ns, name, ZEND_FN(name), arg_info, ZEND_ACC_DEPRECATED)
#define ZEND_NS_FALIAS(ns, name, alias, arg_info)		ZEND_NS_FENTRY(ns, name, ZEND_FN(alias), arg_info, 0)
#define ZEND_NS_DEP_FALIAS(ns, name, alias, arg_info)	ZEND_NS_FENTRY(ns, name, ZEND_FN(alias), arg_info, ZEND_ACC_DEPRECATED)

#define ZEND_FE_END            { NULL, NULL, NULL, 0, 0 }

#define ZEND_ARG_INFO(pass_by_ref, name)							{ #name, sizeof(#name)-1, NULL, 0, 0, 0, pass_by_ref},
#define ZEND_ARG_PASS_INFO(pass_by_ref)								{ NULL, 0, NULL, 0, 0, 0, pass_by_ref},
#define ZEND_ARG_OBJ_INFO(pass_by_ref, name, classname, allow_null) { #name, sizeof(#name)-1, #classname, sizeof(#classname)-1, IS_OBJECT, allow_null, pass_by_ref},
#define ZEND_ARG_ARRAY_INFO(pass_by_ref, name, allow_null) { #name, sizeof(#name)-1, NULL, 0, IS_ARRAY, allow_null, pass_by_ref},
#define ZEND_ARG_TYPE_INFO(pass_by_ref, name, type_hint, allow_null) { #name, sizeof(#name)-1, NULL, 0, type_hint, allow_null, pass_by_ref},
#define ZEND_BEGIN_ARG_INFO_EX(name, pass_rest_by_reference, return_reference, required_num_args)	\
	static const zend_arg_info name[] = {																		\
		{ NULL, 0, NULL, required_num_args, 0, return_reference, pass_rest_by_reference},
#define ZEND_BEGIN_ARG_INFO(name, pass_rest_by_reference)	\
	ZEND_BEGIN_ARG_INFO_EX(name, pass_rest_by_reference, ZEND_RETURN_VALUE, -1)
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
#define ZEND_MODULE_GLOBALS_CTOR_D(module)  void ZEND_MODULE_GLOBALS_CTOR_N(module)(zend_##module##_globals *module##_globals TSRMLS_DC)
#define ZEND_MODULE_GLOBALS_DTOR_D(module)  void ZEND_MODULE_GLOBALS_DTOR_N(module)(zend_##module##_globals *module##_globals TSRMLS_DC)

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

#else

#define ZEND_DECLARE_MODULE_GLOBALS(module_name)							\
	zend_##module_name##_globals module_name##_globals;
#define ZEND_EXTERN_MODULE_GLOBALS(module_name)								\
	extern zend_##module_name##_globals module_name##_globals;
#define ZEND_INIT_MODULE_GLOBALS(module_name, globals_ctor, globals_dtor)	\
	globals_ctor(&module_name##_globals);

#endif

#define INIT_CLASS_ENTRY(class_container, class_name, functions) \
	INIT_OVERLOADED_CLASS_ENTRY(class_container, class_name, functions, NULL, NULL, NULL)

#define INIT_CLASS_ENTRY_EX(class_container, class_name, class_name_len, functions) \
	INIT_OVERLOADED_CLASS_ENTRY_EX(class_container, class_name, class_name_len, functions, NULL, NULL, NULL, NULL, NULL)

#define INIT_OVERLOADED_CLASS_ENTRY_EX(class_container, class_name, class_name_len, functions, handle_fcall, handle_propget, handle_propset, handle_propunset, handle_propisset) \
	{															\
		const char *cl_name = class_name;								\
		int _len = class_name_len;								\
		class_container.name = zend_new_interned_string(cl_name, _len+1, 0 TSRMLS_CC);	\
		if (class_container.name == cl_name) {					\
			class_container.name = zend_strndup(cl_name, _len);	\
		}														\
		class_container.name_length = _len;						\
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

int zend_next_free_module(void);

BEGIN_EXTERN_C()
ZEND_API int zend_get_parameters(int ht, int param_count, ...);
ZEND_API int _zend_get_parameters_array(int ht, int param_count, zval **argument_array TSRMLS_DC);
ZEND_API ZEND_ATTRIBUTE_DEPRECATED int zend_get_parameters_ex(int param_count, ...);
ZEND_API int _zend_get_parameters_array_ex(int param_count, zval ***argument_array TSRMLS_DC);

/* internal function to efficiently copy parameters when executing __call() */
ZEND_API int zend_copy_parameters_array(int param_count, zval *argument_array TSRMLS_DC);

#define zend_get_parameters_array(ht, param_count, argument_array)			\
	_zend_get_parameters_array(ht, param_count, argument_array TSRMLS_CC)
#define zend_get_parameters_array_ex(param_count, argument_array)			\
	_zend_get_parameters_array_ex(param_count, argument_array TSRMLS_CC)
#define zend_parse_parameters_none()										\
	zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")

/* Parameter parsing API -- andrei */

#define ZEND_PARSE_PARAMS_QUIET 1<<1
ZEND_API int zend_parse_parameters(int num_args TSRMLS_DC, const char *type_spec, ...);
ZEND_API int zend_parse_parameters_ex(int flags, int num_args TSRMLS_DC, const char *type_spec, ...);
ZEND_API char *zend_zval_type_name(const zval *arg);

ZEND_API int zend_parse_method_parameters(int num_args TSRMLS_DC, zval *this_ptr, const char *type_spec, ...);
ZEND_API int zend_parse_method_parameters_ex(int flags, int num_args TSRMLS_DC, zval *this_ptr, const char *type_spec, ...);

/* End of parameter parsing API -- andrei */

ZEND_API int zend_register_functions(zend_class_entry *scope, const zend_function_entry *functions, HashTable *function_table, int type TSRMLS_DC);
ZEND_API void zend_unregister_functions(const zend_function_entry *functions, int count, HashTable *function_table TSRMLS_DC);
ZEND_API int zend_startup_module(zend_module_entry *module_entry);
ZEND_API zend_module_entry* zend_register_internal_module(zend_module_entry *module_entry TSRMLS_DC);
ZEND_API zend_module_entry* zend_register_module_ex(zend_module_entry *module TSRMLS_DC);
ZEND_API int zend_startup_module_ex(zend_module_entry *module TSRMLS_DC);
ZEND_API int zend_startup_modules(TSRMLS_D);
ZEND_API void zend_collect_module_handlers(TSRMLS_D);
ZEND_API void zend_destroy_modules(void);
ZEND_API void zend_check_magic_method_implementation(const zend_class_entry *ce, const zend_function *fptr, int error_type TSRMLS_DC);

ZEND_API zend_class_entry *zend_register_internal_class(zend_class_entry *class_entry TSRMLS_DC);
ZEND_API zend_class_entry *zend_register_internal_class_ex(zend_class_entry *class_entry, zend_class_entry *parent_ce, char *parent_name TSRMLS_DC);
ZEND_API zend_class_entry *zend_register_internal_interface(zend_class_entry *orig_class_entry TSRMLS_DC);
ZEND_API void zend_class_implements(zend_class_entry *class_entry TSRMLS_DC, int num_interfaces, ...);

ZEND_API int zend_register_class_alias_ex(const char *name, int name_len, zend_class_entry *ce TSRMLS_DC);

#define zend_register_class_alias(name, ce) \
	zend_register_class_alias_ex(name, sizeof(name)-1, ce TSRMLS_DC)
#define zend_register_ns_class_alias(ns, name, ce) \
	zend_register_class_alias_ex(ZEND_NS_NAME(ns, name), sizeof(ZEND_NS_NAME(ns, name))-1, ce TSRMLS_DC)

ZEND_API int zend_disable_function(char *function_name, uint function_name_length TSRMLS_DC);
ZEND_API int zend_disable_class(char *class_name, uint class_name_length TSRMLS_DC);

ZEND_API void zend_wrong_param_count(TSRMLS_D);

#define IS_CALLABLE_CHECK_SYNTAX_ONLY (1<<0)
#define IS_CALLABLE_CHECK_NO_ACCESS   (1<<1)
#define IS_CALLABLE_CHECK_IS_STATIC   (1<<2)
#define IS_CALLABLE_CHECK_SILENT      (1<<3)

#define IS_CALLABLE_STRICT  (IS_CALLABLE_CHECK_IS_STATIC)

ZEND_API zend_bool zend_is_callable_ex(zval *callable, zval *object_ptr, uint check_flags, char **callable_name, int *callable_name_len, zend_fcall_info_cache *fcc, char **error TSRMLS_DC);
ZEND_API zend_bool zend_is_callable(zval *callable, uint check_flags, char **callable_name TSRMLS_DC);
ZEND_API zend_bool zend_make_callable(zval *callable, char **callable_name TSRMLS_DC);
ZEND_API const char *zend_get_module_version(const char *module_name);
ZEND_API int zend_get_module_started(const char *module_name);
ZEND_API int zend_declare_property(zend_class_entry *ce, const char *name, int name_length, zval *property, int access_type TSRMLS_DC);
ZEND_API int zend_declare_property_ex(zend_class_entry *ce, const char *name, int name_length, zval *property, int access_type, const char *doc_comment, int doc_comment_len TSRMLS_DC);
ZEND_API int zend_declare_property_null(zend_class_entry *ce, const char *name, int name_length, int access_type TSRMLS_DC);
ZEND_API int zend_declare_property_bool(zend_class_entry *ce, const char *name, int name_length, long value, int access_type TSRMLS_DC);
ZEND_API int zend_declare_property_long(zend_class_entry *ce, const char *name, int name_length, long value, int access_type TSRMLS_DC);
ZEND_API int zend_declare_property_double(zend_class_entry *ce, const char *name, int name_length, double value, int access_type TSRMLS_DC);
ZEND_API int zend_declare_property_string(zend_class_entry *ce, const char *name, int name_length, const char *value, int access_type TSRMLS_DC);
ZEND_API int zend_declare_property_stringl(zend_class_entry *ce, const char *name, int name_length, const char *value, int value_len, int access_type TSRMLS_DC);

ZEND_API int zend_declare_class_constant(zend_class_entry *ce, const char *name, size_t name_length, zval *value TSRMLS_DC);
ZEND_API int zend_declare_class_constant_null(zend_class_entry *ce, const char *name, size_t name_length TSRMLS_DC);
ZEND_API int zend_declare_class_constant_long(zend_class_entry *ce, const char *name, size_t name_length, long value TSRMLS_DC);
ZEND_API int zend_declare_class_constant_bool(zend_class_entry *ce, const char *name, size_t name_length, zend_bool value TSRMLS_DC);
ZEND_API int zend_declare_class_constant_double(zend_class_entry *ce, const char *name, size_t name_length, double value TSRMLS_DC);
ZEND_API int zend_declare_class_constant_stringl(zend_class_entry *ce, const char *name, size_t name_length, const char *value, size_t value_length TSRMLS_DC);
ZEND_API int zend_declare_class_constant_string(zend_class_entry *ce, const char *name, size_t name_length, const char *value TSRMLS_DC);

ZEND_API void zend_update_class_constants(zend_class_entry *class_type TSRMLS_DC);
ZEND_API void zend_update_property(zend_class_entry *scope, zval *object, const char *name, int name_length, zval *value TSRMLS_DC);
ZEND_API void zend_update_property_null(zend_class_entry *scope, zval *object, const char *name, int name_length TSRMLS_DC);
ZEND_API void zend_update_property_bool(zend_class_entry *scope, zval *object, const char *name, int name_length, long value TSRMLS_DC);
ZEND_API void zend_update_property_long(zend_class_entry *scope, zval *object, const char *name, int name_length, long value TSRMLS_DC);
ZEND_API void zend_update_property_double(zend_class_entry *scope, zval *object, const char *name, int name_length, double value TSRMLS_DC);
ZEND_API void zend_update_property_string(zend_class_entry *scope, zval *object, const char *name, int name_length, const char *value TSRMLS_DC);
ZEND_API void zend_update_property_stringl(zend_class_entry *scope, zval *object, const char *name, int name_length, const char *value, int value_length TSRMLS_DC);

ZEND_API int zend_update_static_property(zend_class_entry *scope, const char *name, int name_length, zval *value TSRMLS_DC);
ZEND_API int zend_update_static_property_null(zend_class_entry *scope, const char *name, int name_length TSRMLS_DC);
ZEND_API int zend_update_static_property_bool(zend_class_entry *scope, const char *name, int name_length, long value TSRMLS_DC);
ZEND_API int zend_update_static_property_long(zend_class_entry *scope, const char *name, int name_length, long value TSRMLS_DC);
ZEND_API int zend_update_static_property_double(zend_class_entry *scope, const char *name, int name_length, double value TSRMLS_DC);
ZEND_API int zend_update_static_property_string(zend_class_entry *scope, const char *name, int name_length, const char *value TSRMLS_DC);
ZEND_API int zend_update_static_property_stringl(zend_class_entry *scope, const char *name, int name_length, const char *value, int value_length TSRMLS_DC);

ZEND_API zval *zend_read_property(zend_class_entry *scope, zval *object, const char *name, int name_length, zend_bool silent TSRMLS_DC);

ZEND_API zval *zend_read_static_property(zend_class_entry *scope, const char *name, int name_length, zend_bool silent TSRMLS_DC);

ZEND_API zend_class_entry *zend_get_class_entry(const zval *zobject TSRMLS_DC);
ZEND_API int zend_get_object_classname(const zval *object, const char **class_name, zend_uint *class_name_len TSRMLS_DC);
ZEND_API char *zend_get_type_by_const(int type);

#define getThis() (this_ptr)

#define WRONG_PARAM_COUNT					ZEND_WRONG_PARAM_COUNT()
#define WRONG_PARAM_COUNT_WITH_RETVAL(ret)	ZEND_WRONG_PARAM_COUNT_WITH_RETVAL(ret)
#define ARG_COUNT(dummy)	(ht)
#define ZEND_NUM_ARGS()		(ht)
#define ZEND_WRONG_PARAM_COUNT()					{ zend_wrong_param_count(TSRMLS_C); return; }
#define ZEND_WRONG_PARAM_COUNT_WITH_RETVAL(ret)		{ zend_wrong_param_count(TSRMLS_C); return ret; }

#ifndef ZEND_WIN32
#define DLEXPORT
#endif

#define array_init(arg)			_array_init((arg), 0 ZEND_FILE_LINE_CC)
#define array_init_size(arg, size) _array_init((arg), (size) ZEND_FILE_LINE_CC)
#define object_init(arg)		_object_init((arg) ZEND_FILE_LINE_CC TSRMLS_CC)
#define object_init_ex(arg, ce)	_object_init_ex((arg), (ce) ZEND_FILE_LINE_CC TSRMLS_CC)
#define object_and_properties_init(arg, ce, properties)	_object_and_properties_init((arg), (ce), (properties) ZEND_FILE_LINE_CC TSRMLS_CC)
ZEND_API int _array_init(zval *arg, uint size ZEND_FILE_LINE_DC);
ZEND_API int _object_init(zval *arg ZEND_FILE_LINE_DC TSRMLS_DC);
ZEND_API int _object_init_ex(zval *arg, zend_class_entry *ce ZEND_FILE_LINE_DC TSRMLS_DC);
ZEND_API int _object_and_properties_init(zval *arg, zend_class_entry *ce, HashTable *properties ZEND_FILE_LINE_DC TSRMLS_DC);
ZEND_API void object_properties_init(zend_object *object, zend_class_entry *class_type);

ZEND_API void zend_merge_properties(zval *obj, HashTable *properties, int destroy_ht TSRMLS_DC);

/* no longer supported */
ZEND_API int add_assoc_function(zval *arg, const char *key, void (*function_ptr)(INTERNAL_FUNCTION_PARAMETERS));

ZEND_API int add_assoc_long_ex(zval *arg, const char *key, uint key_len, long n);
ZEND_API int add_assoc_null_ex(zval *arg, const char *key, uint key_len);
ZEND_API int add_assoc_bool_ex(zval *arg, const char *key, uint key_len, int b);
ZEND_API int add_assoc_resource_ex(zval *arg, const char *key, uint key_len, int r);
ZEND_API int add_assoc_double_ex(zval *arg, const char *key, uint key_len, double d);
ZEND_API int add_assoc_string_ex(zval *arg, const char *key, uint key_len, char *str, int duplicate);
ZEND_API int add_assoc_stringl_ex(zval *arg, const char *key, uint key_len, char *str, uint length, int duplicate);
ZEND_API int add_assoc_zval_ex(zval *arg, const char *key, uint key_len, zval *value);

#define add_assoc_long(__arg, __key, __n) add_assoc_long_ex(__arg, __key, strlen(__key)+1, __n)
#define add_assoc_null(__arg, __key) add_assoc_null_ex(__arg, __key, strlen(__key) + 1)
#define add_assoc_bool(__arg, __key, __b) add_assoc_bool_ex(__arg, __key, strlen(__key)+1, __b)
#define add_assoc_resource(__arg, __key, __r) add_assoc_resource_ex(__arg, __key, strlen(__key)+1, __r)
#define add_assoc_double(__arg, __key, __d) add_assoc_double_ex(__arg, __key, strlen(__key)+1, __d)
#define add_assoc_string(__arg, __key, __str, __duplicate) add_assoc_string_ex(__arg, __key, strlen(__key)+1, __str, __duplicate)
#define add_assoc_stringl(__arg, __key, __str, __length, __duplicate) add_assoc_stringl_ex(__arg, __key, strlen(__key)+1, __str, __length, __duplicate)
#define add_assoc_zval(__arg, __key, __value) add_assoc_zval_ex(__arg, __key, strlen(__key)+1, __value)

/* unset() functions are only suported for legacy modules and null() functions should be used */
#define add_assoc_unset(__arg, __key) add_assoc_null_ex(__arg, __key, strlen(__key) + 1)
#define add_index_unset(__arg, __key) add_index_null(__arg, __key)
#define add_next_index_unset(__arg) add_next_index_null(__arg)
#define add_property_unset(__arg, __key) add_property_null(__arg, __key)

ZEND_API int add_index_long(zval *arg, ulong idx, long n);
ZEND_API int add_index_null(zval *arg, ulong idx);
ZEND_API int add_index_bool(zval *arg, ulong idx, int b);
ZEND_API int add_index_resource(zval *arg, ulong idx, int r);
ZEND_API int add_index_double(zval *arg, ulong idx, double d);
ZEND_API int add_index_string(zval *arg, ulong idx, const char *str, int duplicate);
ZEND_API int add_index_stringl(zval *arg, ulong idx, const char *str, uint length, int duplicate);
ZEND_API int add_index_zval(zval *arg, ulong index, zval *value);

ZEND_API int add_next_index_long(zval *arg, long n);
ZEND_API int add_next_index_null(zval *arg);
ZEND_API int add_next_index_bool(zval *arg, int b);
ZEND_API int add_next_index_resource(zval *arg, int r);
ZEND_API int add_next_index_double(zval *arg, double d);
ZEND_API int add_next_index_string(zval *arg, const char *str, int duplicate);
ZEND_API int add_next_index_stringl(zval *arg, const char *str, uint length, int duplicate);
ZEND_API int add_next_index_zval(zval *arg, zval *value);

ZEND_API int add_get_assoc_string_ex(zval *arg, const char *key, uint key_len, const char *str, void **dest, int duplicate);
ZEND_API int add_get_assoc_stringl_ex(zval *arg, const char *key, uint key_len, const char *str, uint length, void **dest, int duplicate);

#define add_get_assoc_string(__arg, __key, __str, __dest, __duplicate) add_get_assoc_string_ex(__arg, __key, strlen(__key)+1, __str, __dest, __duplicate)
#define add_get_assoc_stringl(__arg, __key, __str, __length, __dest, __duplicate) add_get_assoc_stringl_ex(__arg, __key, strlen(__key)+1, __str, __length, __dest, __duplicate)

ZEND_API int add_get_index_long(zval *arg, ulong idx, long l, void **dest);
ZEND_API int add_get_index_double(zval *arg, ulong idx, double d, void **dest);
ZEND_API int add_get_index_string(zval *arg, ulong idx, const char *str, void **dest, int duplicate);
ZEND_API int add_get_index_stringl(zval *arg, ulong idx, const char *str, uint length, void **dest, int duplicate);

ZEND_API int add_property_long_ex(zval *arg, const char *key, uint key_len, long l TSRMLS_DC);
ZEND_API int add_property_null_ex(zval *arg, const char *key, uint key_len TSRMLS_DC);
ZEND_API int add_property_bool_ex(zval *arg, const char *key, uint key_len, int b TSRMLS_DC);
ZEND_API int add_property_resource_ex(zval *arg, const char *key, uint key_len, long r TSRMLS_DC);
ZEND_API int add_property_double_ex(zval *arg, const char *key, uint key_len, double d TSRMLS_DC);
ZEND_API int add_property_string_ex(zval *arg, const char *key, uint key_len, const char *str, int duplicate TSRMLS_DC);
ZEND_API int add_property_stringl_ex(zval *arg, const char *key, uint key_len,  const char *str, uint length, int duplicate TSRMLS_DC);
ZEND_API int add_property_zval_ex(zval *arg, const char *key, uint key_len, zval *value TSRMLS_DC);

#define add_property_long(__arg, __key, __n) add_property_long_ex(__arg, __key, strlen(__key)+1, __n TSRMLS_CC)
#define add_property_null(__arg, __key) add_property_null_ex(__arg, __key, strlen(__key) + 1 TSRMLS_CC)
#define add_property_bool(__arg, __key, __b) add_property_bool_ex(__arg, __key, strlen(__key)+1, __b TSRMLS_CC)
#define add_property_resource(__arg, __key, __r) add_property_resource_ex(__arg, __key, strlen(__key)+1, __r TSRMLS_CC)
#define add_property_double(__arg, __key, __d) add_property_double_ex(__arg, __key, strlen(__key)+1, __d TSRMLS_CC)
#define add_property_string(__arg, __key, __str, __duplicate) add_property_string_ex(__arg, __key, strlen(__key)+1, __str, __duplicate TSRMLS_CC)
#define add_property_stringl(__arg, __key, __str, __length, __duplicate) add_property_stringl_ex(__arg, __key, strlen(__key)+1, __str, __length, __duplicate TSRMLS_CC)
#define add_property_zval(__arg, __key, __value) add_property_zval_ex(__arg, __key, strlen(__key)+1, __value TSRMLS_CC)       


ZEND_API int call_user_function(HashTable *function_table, zval **object_pp, zval *function_name, zval *retval_ptr, zend_uint param_count, zval *params[] TSRMLS_DC);
ZEND_API int call_user_function_ex(HashTable *function_table, zval **object_pp, zval *function_name, zval **retval_ptr_ptr, zend_uint param_count, zval **params[], int no_separation, HashTable *symbol_table TSRMLS_DC);

ZEND_API extern const zend_fcall_info empty_fcall_info;
ZEND_API extern const zend_fcall_info_cache empty_fcall_info_cache;

/** Build zend_call_info/cache from a zval*
 *
 * Caller is responsible to provide a return value, otherwise the we will crash. 
 * fci->retval_ptr_ptr = NULL;
 * In order to pass parameters the following members need to be set:
 * fci->param_count = 0;
 * fci->params = NULL;
 * The callable_name argument may be NULL.
 * Set check_flags to IS_CALLABLE_STRICT for every new usage!
 */
ZEND_API int zend_fcall_info_init(zval *callable, uint check_flags, zend_fcall_info *fci, zend_fcall_info_cache *fcc, char **callable_name, char **error TSRMLS_DC);

/** Clear argumens connected with zend_fcall_info *fci
 * If free_mem is not zero then the params array gets free'd as well
 */
ZEND_API void zend_fcall_info_args_clear(zend_fcall_info *fci, int free_mem);

/** Save current arguments from zend_fcall_info *fci
 * params array will be set to NULL
 */
ZEND_API void zend_fcall_info_args_save(zend_fcall_info *fci, int *param_count, zval ****params);

/** Free arguments connected with zend_fcall_info *fci andset back saved ones.
 */
ZEND_API void zend_fcall_info_args_restore(zend_fcall_info *fci, int param_count, zval ***params);

/** Set or clear the arguments in the zend_call_info struct taking care of
 * refcount. If args is NULL and arguments are set then those are cleared.
 */
ZEND_API int zend_fcall_info_args(zend_fcall_info *fci, zval *args TSRMLS_DC);

/** Set arguments in the zend_fcall_info struct taking care of refcount.
 * If argc is 0 the arguments which are set will be cleared, else pass
 * a variable amount of zval** arguments.
 */
ZEND_API int zend_fcall_info_argp(zend_fcall_info *fci TSRMLS_DC, int argc, zval ***argv);

/** Set arguments in the zend_fcall_info struct taking care of refcount.
 * If argc is 0 the arguments which are set will be cleared, else pass
 * a variable amount of zval** arguments.
 */
ZEND_API int zend_fcall_info_argv(zend_fcall_info *fci TSRMLS_DC, int argc, va_list *argv);

/** Set arguments in the zend_fcall_info struct taking care of refcount.
 * If argc is 0 the arguments which are set will be cleared, else pass
 * a variable amount of zval** arguments.
 */
ZEND_API int zend_fcall_info_argn(zend_fcall_info *fci TSRMLS_DC, int argc, ...);

/** Call a function using information created by zend_fcall_info_init()/args().
 * If args is given then those replace the arguement info in fci is temporarily.
 */
ZEND_API int zend_fcall_info_call(zend_fcall_info *fci, zend_fcall_info_cache *fcc, zval **retval, zval *args TSRMLS_DC);

ZEND_API int zend_call_function(zend_fcall_info *fci, zend_fcall_info_cache *fci_cache TSRMLS_DC);

ZEND_API int zend_set_hash_symbol(zval *symbol, const char *name, int name_length, zend_bool is_ref, int num_symbol_tables, ...);

ZEND_API void zend_delete_variable(zend_execute_data *ex, HashTable *ht, const char *name, int name_len, ulong hash_value TSRMLS_DC);

ZEND_API int zend_delete_global_variable(const char *name, int name_len TSRMLS_DC);

ZEND_API int zend_delete_global_variable_ex(const char *name, int name_len, ulong hash_value TSRMLS_DC);

ZEND_API void zend_reset_all_cv(HashTable *symbol_table TSRMLS_DC);

ZEND_API void zend_rebuild_symbol_table(TSRMLS_D);

#define add_method(arg, key, method)	add_assoc_function((arg), (key), (method))

ZEND_API ZEND_FUNCTION(display_disabled_function);
ZEND_API ZEND_FUNCTION(display_disabled_class);
END_EXTERN_C()

#if ZEND_DEBUG
#define CHECK_ZVAL_STRING(z) \
	if (Z_STRVAL_P(z)[ Z_STRLEN_P(z) ] != '\0') { zend_error(E_WARNING, "String is not zero-terminated (%s)", Z_STRVAL_P(z)); }
#define CHECK_ZVAL_STRING_REL(z) \
	if (Z_STRVAL_P(z)[ Z_STRLEN_P(z) ] != '\0') { zend_error(E_WARNING, "String is not zero-terminated (%s) (source: %s:%d)", Z_STRVAL_P(z) ZEND_FILE_LINE_RELAY_CC); }
#else
#define CHECK_ZVAL_STRING(z)
#define CHECK_ZVAL_STRING_REL(z)
#endif

#define CHECK_ZVAL_NULL_PATH(p) (Z_STRLEN_P(p) != strlen(Z_STRVAL_P(p)))
#define CHECK_NULL_PATH(p, l) (strlen(p) != l)

#define ZVAL_RESOURCE(z, l) do {	\
		zval *__z = (z);			\
		Z_LVAL_P(__z) = l;			\
		Z_TYPE_P(__z) = IS_RESOURCE;\
	} while (0)

#define ZVAL_BOOL(z, b) do {		\
		zval *__z = (z);			\
		Z_LVAL_P(__z) = ((b) != 0);	\
		Z_TYPE_P(__z) = IS_BOOL;	\
	} while (0)

#define ZVAL_NULL(z) {				\
		Z_TYPE_P(z) = IS_NULL;		\
	}

#define ZVAL_LONG(z, l) {			\
		zval *__z = (z);			\
		Z_LVAL_P(__z) = l;			\
		Z_TYPE_P(__z) = IS_LONG;	\
	}

#define ZVAL_DOUBLE(z, d) {			\
		zval *__z = (z);			\
		Z_DVAL_P(__z) = d;			\
		Z_TYPE_P(__z) = IS_DOUBLE;	\
	}

#define ZVAL_STRING(z, s, duplicate) do {	\
		const char *__s=(s);				\
		zval *__z = (z);					\
		Z_STRLEN_P(__z) = strlen(__s);		\
		Z_STRVAL_P(__z) = (duplicate?estrndup(__s, Z_STRLEN_P(__z)):(char*)__s);\
		Z_TYPE_P(__z) = IS_STRING;			\
	} while (0)

#define ZVAL_STRINGL(z, s, l, duplicate) do {	\
		const char *__s=(s); int __l=l;			\
		zval *__z = (z);						\
		Z_STRLEN_P(__z) = __l;					\
		Z_STRVAL_P(__z) = (duplicate?estrndup(__s, __l):(char*)__s);\
		Z_TYPE_P(__z) = IS_STRING;				\
	} while (0)

#define ZVAL_EMPTY_STRING(z) do {	\
		zval *__z = (z);			\
		Z_STRLEN_P(__z) = 0;		\
		Z_STRVAL_P(__z) = STR_EMPTY_ALLOC();\
		Z_TYPE_P(__z) = IS_STRING;	\
	} while (0)

#define ZVAL_ZVAL(z, zv, copy, dtor) {			\
		zend_uchar is_ref = Z_ISREF_P(z);		\
		zend_uint refcount = Z_REFCOUNT_P(z);	\
		ZVAL_COPY_VALUE(z, zv);					\
		if (copy) {								\
			zval_copy_ctor(z);					\
	    }										\
		if (dtor) {								\
			if (!copy) {						\
				ZVAL_NULL(zv);					\
			}									\
			zval_ptr_dtor(&zv);					\
	    }										\
		Z_SET_ISREF_TO_P(z, is_ref);			\
		Z_SET_REFCOUNT_P(z, refcount);			\
	}

#define ZVAL_FALSE(z)  					ZVAL_BOOL(z, 0)
#define ZVAL_TRUE(z)  					ZVAL_BOOL(z, 1)

#define RETVAL_RESOURCE(l)				ZVAL_RESOURCE(return_value, l)
#define RETVAL_BOOL(b)					ZVAL_BOOL(return_value, b)
#define RETVAL_NULL() 					ZVAL_NULL(return_value)
#define RETVAL_LONG(l) 					ZVAL_LONG(return_value, l)
#define RETVAL_DOUBLE(d) 				ZVAL_DOUBLE(return_value, d)
#define RETVAL_STRING(s, duplicate) 		ZVAL_STRING(return_value, s, duplicate)
#define RETVAL_STRINGL(s, l, duplicate) 	ZVAL_STRINGL(return_value, s, l, duplicate)
#define RETVAL_EMPTY_STRING() 			ZVAL_EMPTY_STRING(return_value)
#define RETVAL_ZVAL(zv, copy, dtor)		ZVAL_ZVAL(return_value, zv, copy, dtor)
#define RETVAL_FALSE  					ZVAL_BOOL(return_value, 0)
#define RETVAL_TRUE   					ZVAL_BOOL(return_value, 1)

#define RETURN_RESOURCE(l) 				{ RETVAL_RESOURCE(l); return; }
#define RETURN_BOOL(b) 					{ RETVAL_BOOL(b); return; }
#define RETURN_NULL() 					{ RETVAL_NULL(); return;}
#define RETURN_LONG(l) 					{ RETVAL_LONG(l); return; }
#define RETURN_DOUBLE(d) 				{ RETVAL_DOUBLE(d); return; }
#define RETURN_STRING(s, duplicate) 	{ RETVAL_STRING(s, duplicate); return; }
#define RETURN_STRINGL(s, l, duplicate) { RETVAL_STRINGL(s, l, duplicate); return; }
#define RETURN_EMPTY_STRING() 			{ RETVAL_EMPTY_STRING(); return; }
#define RETURN_ZVAL(zv, copy, dtor)		{ RETVAL_ZVAL(zv, copy, dtor); return; }
#define RETURN_FALSE  					{ RETVAL_FALSE; return; }
#define RETURN_TRUE   					{ RETVAL_TRUE; return; }

#define SET_VAR_STRING(n, v) {																				\
								{																			\
									zval *var;																\
									ALLOC_ZVAL(var);														\
									ZVAL_STRING(var, v, 0);													\
									ZEND_SET_GLOBAL_VAR(n, var);											\
								}																			\
							}

#define SET_VAR_STRINGL(n, v, l) {														\
									{													\
										zval *var;										\
										ALLOC_ZVAL(var);								\
										ZVAL_STRINGL(var, v, l, 0);						\
										ZEND_SET_GLOBAL_VAR(n, var);					\
									}													\
								}

#define SET_VAR_LONG(n, v)	{															\
								{														\
									zval *var;											\
									ALLOC_ZVAL(var);									\
									ZVAL_LONG(var, v);									\
									ZEND_SET_GLOBAL_VAR(n, var);						\
								}														\
							}

#define SET_VAR_DOUBLE(n, v) {															\
								{														\
									zval *var;											\
									ALLOC_ZVAL(var);									\
									ZVAL_DOUBLE(var, v);								\
									ZEND_SET_GLOBAL_VAR(n, var);						\
								}														\
							}


#define ZEND_SET_SYMBOL(symtable, name, var)										\
	{																				\
		char *_name = (name);														\
																					\
		ZEND_SET_SYMBOL_WITH_LENGTH(symtable, _name, strlen(_name)+1, var, 1, 0);	\
	}

#define ZEND_SET_SYMBOL_WITH_LENGTH(symtable, name, name_length, var, _refcount, _is_ref)				\
	{																									\
		zval **orig_var;																				\
																										\
		if (zend_hash_find(symtable, (name), (name_length), (void **) &orig_var)==SUCCESS				\
			&& PZVAL_IS_REF(*orig_var)) {																\
			Z_SET_REFCOUNT_P(var, Z_REFCOUNT_PP(orig_var));												\
			Z_SET_ISREF_P(var);																			\
																										\
			if (_refcount) {																			\
				Z_SET_REFCOUNT_P(var, Z_REFCOUNT_P(var) + _refcount - 1);								\
			}																							\
			zval_dtor(*orig_var);																		\
			**orig_var = *(var);																		\
			FREE_ZVAL(var);																				\
		} else {																						\
			Z_SET_ISREF_TO_P(var, _is_ref);																\
			if (_refcount) {																			\
				Z_SET_REFCOUNT_P(var, _refcount);														\
			}																							\
			zend_hash_update(symtable, (name), (name_length), &(var), sizeof(zval *), NULL);			\
		}																								\
	}


#define ZEND_SET_GLOBAL_VAR(name, var)				\
	ZEND_SET_SYMBOL(&EG(symbol_table), name, var)

#define ZEND_SET_GLOBAL_VAR_WITH_LENGTH(name, name_length, var, _refcount, _is_ref)	\
	ZEND_SET_SYMBOL_WITH_LENGTH(&EG(symbol_table), name, name_length, var, _refcount, _is_ref)

#define ZEND_DEFINE_PROPERTY(class_ptr, name, value, mask)							\
{																					\
	char *_name = (name);															\
	int namelen = strlen(_name);													\
	zend_declare_property(class_ptr, _name, namelen, value, mask TSRMLS_CC);		\
}

#define HASH_OF(p) (Z_TYPE_P(p)==IS_ARRAY ? Z_ARRVAL_P(p) : ((Z_TYPE_P(p)==IS_OBJECT ? Z_OBJ_HT_P(p)->get_properties((p) TSRMLS_CC) : NULL)))
#define ZVAL_IS_NULL(z) (Z_TYPE_P(z)==IS_NULL)

/* For compatibility */
#define ZEND_MINIT			ZEND_MODULE_STARTUP_N
#define ZEND_MSHUTDOWN		ZEND_MODULE_SHUTDOWN_N
#define ZEND_RINIT			ZEND_MODULE_ACTIVATE_N
#define ZEND_RSHUTDOWN		ZEND_MODULE_DEACTIVATE_N
#define ZEND_MINFO			ZEND_MODULE_INFO_N
#define ZEND_GINIT(module)		((void (*)(void* TSRMLS_DC))(ZEND_MODULE_GLOBALS_CTOR_N(module)))
#define ZEND_GSHUTDOWN(module)	((void (*)(void* TSRMLS_DC))(ZEND_MODULE_GLOBALS_DTOR_N(module)))

#define ZEND_MINIT_FUNCTION			ZEND_MODULE_STARTUP_D
#define ZEND_MSHUTDOWN_FUNCTION		ZEND_MODULE_SHUTDOWN_D
#define ZEND_RINIT_FUNCTION			ZEND_MODULE_ACTIVATE_D
#define ZEND_RSHUTDOWN_FUNCTION		ZEND_MODULE_DEACTIVATE_D
#define ZEND_MINFO_FUNCTION			ZEND_MODULE_INFO_D
#define ZEND_GINIT_FUNCTION			ZEND_MODULE_GLOBALS_CTOR_D
#define ZEND_GSHUTDOWN_FUNCTION		ZEND_MODULE_GLOBALS_DTOR_D

END_EXTERN_C()

#endif /* ZEND_API_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
