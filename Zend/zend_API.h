/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
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


#ifndef ZEND_API_H
#define ZEND_API_H

#include "zend_modules.h"
#include "zend_list.h"
#include "zend_fast_cache.h"
#include "zend_operators.h"
#include "zend_variables.h"
#include "zend_execute.h"


BEGIN_EXTERN_C()
#define ZEND_FN(name) zif_##name
#define ZEND_NAMED_FUNCTION(name) void name(INTERNAL_FUNCTION_PARAMETERS)
#define ZEND_FUNCTION(name) ZEND_NAMED_FUNCTION(ZEND_FN(name))

#define ZEND_NAMED_FE(zend_name, name, arg_types) { #zend_name, name, arg_types },
#define ZEND_FE(name, arg_types) ZEND_NAMED_FE(name, ZEND_FN(name), arg_types)
#define ZEND_FALIAS(name, alias, arg_types) ZEND_NAMED_FE(name, ZEND_FN(alias), arg_types)
#define ZEND_STATIC_FE(name, impl_name, arg_types) { name, impl_name, arg_types },

/* Name macros */
#define ZEND_MODULE_STARTUP_N(module)       zm_startup_##module
#define ZEND_MODULE_SHUTDOWN_N(module)		zm_shutdown_##module
#define ZEND_MODULE_ACTIVATE_N(module)		zm_activate_##module
#define ZEND_MODULE_DEACTIVATE_N(module)	zm_deactivate_##module
#define ZEND_MODULE_INFO_N(module)			zm_info_##module

/* Declaration macros */
#define ZEND_MODULE_STARTUP_D(module)		int ZEND_MODULE_STARTUP_N(module)(INIT_FUNC_ARGS)
#define ZEND_MODULE_SHUTDOWN_D(module)		int ZEND_MODULE_SHUTDOWN_N(module)(SHUTDOWN_FUNC_ARGS)
#define ZEND_MODULE_ACTIVATE_D(module)		int ZEND_MODULE_ACTIVATE_N(module)(INIT_FUNC_ARGS)
#define ZEND_MODULE_DEACTIVATE_D(module)	int ZEND_MODULE_DEACTIVATE_N(module)(SHUTDOWN_FUNC_ARGS)
#define ZEND_MODULE_INFO_D(module)			void ZEND_MODULE_INFO_N(module)(ZEND_MODULE_INFO_FUNC_ARGS)

#define ZEND_GET_MODULE(name) \
	ZEND_DLEXPORT zend_module_entry *get_module(void) { return &name##_module_entry; }

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



#define INIT_CLASS_ENTRY(class_container, class_name, functions) INIT_OVERLOADED_CLASS_ENTRY(class_container, class_name, functions, NULL, NULL, NULL)

#define INIT_OVERLOADED_CLASS_ENTRY(class_container, class_name, functions, handle_fcall, handle_propget, handle_propset) \
	{															\
		class_container.name = strdup(class_name);				\
		class_container.name_length = sizeof(class_name)-1;		\
		class_container.builtin_functions = functions;			\
		class_container.constructor = NULL;						\
		class_container.destructor = NULL;						\
		class_container.clone = NULL;							\
		class_container.create_object = NULL;	 				\
		class_container.handle_function_call = handle_fcall;	\
		class_container.handle_property_get = handle_propget;	\
		class_container.handle_property_set = handle_propset;	\
	}



int zend_next_free_module(void);

ZEND_API int zend_get_parameters(int ht, int param_count, ...);
ZEND_API int _zend_get_parameters_array(int ht, int param_count, zval **argument_array TSRMLS_DC);
ZEND_API int zend_get_parameters_ex(int param_count, ...);
ZEND_API int _zend_get_parameters_array_ex(int param_count, zval ***argument_array TSRMLS_DC);

#define zend_get_parameters_array(ht, param_count, argument_array)			\
	_zend_get_parameters_array(ht, param_count, argument_array TSRMLS_CC)
#define zend_get_parameters_array_ex(param_count, argument_array)			\
	_zend_get_parameters_array_ex(param_count, argument_array TSRMLS_CC)


/* Parameter parsing API -- andrei */

#define ZEND_PARSE_PARAMS_QUIET 1<<1
ZEND_API int zend_parse_parameters(int num_args TSRMLS_DC, char *type_spec, ...);
ZEND_API int zend_parse_parameters_ex(int flags, int num_args TSRMLS_DC, char *type_spec, ...);
ZEND_API char *zend_zval_type_name(zval *arg);

/* End of parameter parsing API -- andrei */

int zend_register_functions(zend_class_entry *scope, zend_function_entry *functions, HashTable *function_table, int type TSRMLS_DC);
void zend_unregister_functions(zend_function_entry *functions, int count, HashTable *function_table TSRMLS_DC);
ZEND_API int zend_register_module(zend_module_entry *module_entry);

ZEND_API zend_class_entry *zend_register_internal_class(zend_class_entry *class_entry TSRMLS_DC);
ZEND_API zend_class_entry *zend_register_internal_class_ex(zend_class_entry *class_entry, zend_class_entry *parent_ce, char *parent_name TSRMLS_DC);

ZEND_API zend_module_entry *zend_get_module(int module_number);
ZEND_API int zend_disable_function(char *function_name, uint function_name_length TSRMLS_DC);

ZEND_API void zend_wrong_param_count(TSRMLS_D);
ZEND_API zend_bool zend_is_callable(zval *callable, zend_bool syntax_only, char **callable_name);
ZEND_API char *zend_get_module_version(char *module_name);

ZEND_API zend_class_entry *zend_get_class_entry(zval *zobject);

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

ZEND_API int zend_startup_module(zend_module_entry *module);

#define array_init(arg)			_array_init((arg) ZEND_FILE_LINE_CC)
#define object_init(arg)		_object_init((arg) ZEND_FILE_LINE_CC TSRMLS_CC)
#define object_init_ex(arg, ce)	_object_init_ex((arg), (ce) ZEND_FILE_LINE_CC TSRMLS_CC)
#define object_and_properties_init(arg, ce, properties)	_object_and_properties_init((arg), (ce), (properties) ZEND_FILE_LINE_CC TSRMLS_CC)
ZEND_API int _array_init(zval *arg ZEND_FILE_LINE_DC);
ZEND_API int _object_init(zval *arg ZEND_FILE_LINE_DC TSRMLS_DC);
ZEND_API int _object_init_ex(zval *arg, zend_class_entry *ce ZEND_FILE_LINE_DC TSRMLS_DC);
ZEND_API int _object_and_properties_init(zval *arg, zend_class_entry *ce, HashTable *properties ZEND_FILE_LINE_DC TSRMLS_DC);

/* no longer supported */
ZEND_API int add_assoc_function(zval *arg, char *key, void (*function_ptr)(INTERNAL_FUNCTION_PARAMETERS));

ZEND_API int add_assoc_long_ex(zval *arg, char *key, uint key_len, long n);
ZEND_API int add_assoc_null_ex(zval *arg, char *key, uint key_len);
ZEND_API int add_assoc_bool_ex(zval *arg, char *key, uint key_len, int b);
ZEND_API int add_assoc_resource_ex(zval *arg, char *key, uint key_len, int r);
ZEND_API int add_assoc_double_ex(zval *arg, char *key, uint key_len, double d);
ZEND_API int add_assoc_string_ex(zval *arg, char *key, uint key_len, char *str, int duplicate);
ZEND_API int add_assoc_stringl_ex(zval *arg, char *key, uint key_len, char *str, uint length, int duplicate);
ZEND_API int add_assoc_zval_ex(zval *arg, char *key, uint key_len, zval *value);

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

ZEND_API int add_index_long(zval *arg, uint idx, long n);
ZEND_API int add_index_null(zval *arg, uint idx);
ZEND_API int add_index_bool(zval *arg, uint idx, int b);
ZEND_API int add_index_resource(zval *arg, uint idx, int r);
ZEND_API int add_index_double(zval *arg, uint idx, double d);
ZEND_API int add_index_string(zval *arg, uint idx, char *str, int duplicate);
ZEND_API int add_index_stringl(zval *arg, uint idx, char *str, uint length, int duplicate);
ZEND_API int add_index_zval(zval *arg, uint index, zval *value);

ZEND_API int add_next_index_long(zval *arg, long n);
ZEND_API int add_next_index_null(zval *arg);
ZEND_API int add_next_index_bool(zval *arg, int b);
ZEND_API int add_next_index_resource(zval *arg, int r);
ZEND_API int add_next_index_double(zval *arg, double d);
ZEND_API int add_next_index_string(zval *arg, char *str, int duplicate);
ZEND_API int add_next_index_stringl(zval *arg, char *str, uint length, int duplicate);
ZEND_API int add_next_index_zval(zval *arg, zval *value);

ZEND_API int add_get_assoc_string_ex(zval *arg, char *key, uint key_len, char *str, void **dest, int duplicate);
ZEND_API int add_get_assoc_stringl_ex(zval *arg, char *key, uint key_len, char *str, uint length, void **dest, int duplicate);

#define add_get_assoc_string(__arg, __key, __str, __dest, __duplicate) add_get_assoc_string_ex(__arg, __key, strlen(__key)+1, __str, __dest, __duplicate)
#define add_get_assoc_stringl(__arg, __key, __str, __length, __dest, __duplicate) add_get_assoc_stringl_ex(__arg, __key, strlen(__key)+1, __str, __length, __dest, __duplicate)

ZEND_API int add_get_index_long(zval *arg, uint idx, long l, void **dest);
ZEND_API int add_get_index_double(zval *arg, uint idx, double d, void **dest);
ZEND_API int add_get_index_string(zval *arg, uint idx, char *str, void **dest, int duplicate);
ZEND_API int add_get_index_stringl(zval *arg, uint idx, char *str, uint length, void **dest, int duplicate);

ZEND_API int add_property_long_ex(zval *arg, char *key, uint key_len, long l);
ZEND_API int add_property_null_ex(zval *arg, char *key, uint key_len);
ZEND_API int add_property_bool_ex(zval *arg, char *key, uint key_len, int b);
ZEND_API int add_property_resource_ex(zval *arg, char *key, uint key_len, long r);
ZEND_API int add_property_double_ex(zval *arg, char *key, uint key_len, double d);
ZEND_API int add_property_string_ex(zval *arg, char *key, uint key_len, char *str, int duplicate);
ZEND_API int add_property_stringl_ex(zval *arg, char *key, uint key_len,  char *str, uint length, int duplicate);
ZEND_API int add_property_zval_ex(zval *arg, char *key, uint key_len, zval *value);

#define add_property_long(__arg, __key, __n) add_property_long_ex(__arg, __key, strlen(__key)+1, __n)
#define add_property_null(__arg, __key) add_property_null_ex(__arg, __key, strlen(__key) + 1)
#define add_property_bool(__arg, __key, __b) add_property_bool_ex(__arg, __key, strlen(__key)+1, __b)
#define add_property_resource(__arg, __key, __r) add_property_resource_ex(__arg, __key, strlen(__key)+1, __r)
#define add_property_double(__arg, __key, __d) add_property_double_ex(__arg, __key, strlen(__key)+1, __d) 
#define add_property_string(__arg, __key, __str, __duplicate) add_property_string_ex(__arg, __key, strlen(__key)+1, __str, __duplicate)
#define add_property_stringl(__arg, __key, __str, __length, __duplicate) add_property_stringl_ex(__arg, __key, strlen(__key)+1, __str, __length, __duplicate)
#define add_property_zval(__arg, __key, __value) add_property_zval_ex(__arg, __key, strlen(__key)+1, __value)       

ZEND_API int call_user_function(HashTable *function_table, zval **object_pp, zval *function_name, zval *retval_ptr, int param_count, zval *params[] TSRMLS_DC);
ZEND_API int call_user_function_ex(HashTable *function_table, zval **object_pp, zval *function_name, zval **retval_ptr_ptr, int param_count, zval **params[], int no_separation, HashTable *symbol_table TSRMLS_DC);

ZEND_API int zend_set_hash_symbol(zval *symbol, char *name, int name_length,
                                  zend_bool is_ref, int num_symbol_tables, ...);

#define add_method(arg, key, method)	add_assoc_function((arg), (key), (method))

ZEND_API ZEND_FUNCTION(display_disabled_function);

#if ZEND_DEBUG
#define CHECK_ZVAL_STRING(z) \
	if ((z)->value.str.val[ (z)->value.str.len ] != '\0') { zend_error(E_WARNING, "String is not zero-terminated (%s)", (z)->value.str.val); }
#define CHECK_ZVAL_STRING_REL(z) \
	if ((z)->value.str.val[ (z)->value.str.len ] != '\0') { zend_error(E_WARNING, "String is not zero-terminated (%s) (source: %s:%d)", (z)->value.str.val ZEND_FILE_LINE_RELAY_CC); }
#else
#define CHECK_ZVAL_STRING(z)
#define CHECK_ZVAL_STRING_REL(z)
#endif

#define ZVAL_RESOURCE(z, l) {			\
		(z)->type = IS_RESOURCE;        \
		(z)->value.lval = l;	        \
	}

#define ZVAL_BOOL(z, b) {				\
		(z)->type = IS_BOOL;	        \
		(z)->value.lval = b;	        \
	}

#define ZVAL_NULL(z) {					\
		(z)->type = IS_NULL;	        \
	}

#define ZVAL_LONG(z, l) {				\
		(z)->type = IS_LONG;	        \
		(z)->value.lval = l;	        \
	}

#define ZVAL_DOUBLE(z, d) {				\
		(z)->type = IS_DOUBLE;	        \
		(z)->value.dval = d;	        \
	}

#define ZVAL_STRING(z, s, duplicate) {	\
		char *__s=(s);					\
		(z)->value.str.len = strlen(__s);	\
		(z)->value.str.val = (duplicate?estrndup(__s, (z)->value.str.len):__s);	\
		(z)->type = IS_STRING;	        \
	}

#define ZVAL_STRINGL(z, s, l, duplicate) {	\
		char *__s=(s); int __l=l;		\
		(z)->value.str.len = __l;	    \
		(z)->value.str.val = (duplicate?estrndup(__s, __l):__s);	\
		(z)->type = IS_STRING;		    \
	}

#define ZVAL_EMPTY_STRING(z) {	        \
		(z)->value.str.len = 0;  	    \
		(z)->value.str.val = empty_string; \
		(z)->type = IS_STRING;		    \
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
			(var)->refcount = (*orig_var)->refcount;													\
			(var)->is_ref = 1;																			\
																										\
			if (_refcount) {																			\
				(var)->refcount += _refcount-1;															\
			}																							\
			zval_dtor(*orig_var);																		\
			**orig_var = *(var);																		\
			FREE_ZVAL(var);																					\
		} else {																						\
			(var)->is_ref = _is_ref;																	\
			if (_refcount) {																			\
				(var)->refcount = _refcount;															\
			}																							\
			zend_hash_update(symtable, (name), (name_length), &(var), sizeof(zval *), NULL);			\
		}																								\
	}


#define ZEND_SET_GLOBAL_VAR(name, var)				\
	ZEND_SET_SYMBOL(&EG(symbol_table), name, var)

#define ZEND_SET_GLOBAL_VAR_WITH_LENGTH(name, name_length, var, _refcount, _is_ref)		\
	ZEND_SET_SYMBOL_WITH_LENGTH(&EG(symbol_table), name, name_length, var, _refcount, _is_ref)

#define HASH_OF(p) ((p)->type==IS_ARRAY ? (p)->value.ht : (((p)->type==IS_OBJECT ? Z_OBJ_HT_P(p)->get_properties((p) TSRMLS_CC) : NULL)))
#define ZVAL_IS_NULL(z) ((z)->type==IS_NULL)

/* For compatibility */
#define ZEND_MINIT			ZEND_MODULE_STARTUP_N
#define ZEND_MSHUTDOWN		ZEND_MODULE_SHUTDOWN_N
#define ZEND_RINIT			ZEND_MODULE_ACTIVATE_N
#define ZEND_RSHUTDOWN		ZEND_MODULE_DEACTIVATE_N
#define ZEND_MINFO			ZEND_MODULE_INFO_N

#define ZEND_MINIT_FUNCTION			ZEND_MODULE_STARTUP_D
#define ZEND_MSHUTDOWN_FUNCTION		ZEND_MODULE_SHUTDOWN_D
#define ZEND_RINIT_FUNCTION			ZEND_MODULE_ACTIVATE_D
#define ZEND_RSHUTDOWN_FUNCTION		ZEND_MODULE_DEACTIVATE_D
#define ZEND_MINFO_FUNCTION			ZEND_MODULE_INFO_D

END_EXTERN_C()
	
#endif /* ZEND_API_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
