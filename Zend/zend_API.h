/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2000 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.92 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_92.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef ZEND_API_H
#define ZEND_API_H

#include "modules.h"
#include "zend_list.h"
#include "zend_fast_cache.h"
#include "zend_operators.h"
#include "zend_variables.h"
#include "zend_execute.h"

#define ZEND_FN(name) zend_if_##name
#define ZEND_NAMED_FUNCTION(name) void name(INTERNAL_FUNCTION_PARAMETERS)
#define ZEND_FUNCTION(name) ZEND_NAMED_FUNCTION(ZEND_FN(name))

#define ZEND_NAMED_FE(zend_name, name, arg_types) { #zend_name, name, arg_types },
#define ZEND_FE(name, arg_types) ZEND_NAMED_FE(name, ZEND_FN(name), arg_types)
#define ZEND_FALIAS(name, alias, arg_types) ZEND_NAMED_FE(name, ZEND_FN(alias), arg_types)

#define ZEND_MINIT(module)       zend_minit_##module
#define ZEND_MSHUTDOWN(module)   zend_mshutdown_##module
#define ZEND_RINIT(module)       zend_rinit_##module
#define ZEND_RSHUTDOWN(module)   zend_rshutdown_##module
#define ZEND_MINFO(module)       zend_info_##module
#define ZEND_GINIT(module)       zend_ginit_##module
#define ZEND_GSHUTDOWN(module)   zend_gshutdown_##module

#define ZEND_MINIT_FUNCTION(module)      int ZEND_MINIT(module)(INIT_FUNC_ARGS)
#define ZEND_MSHUTDOWN_FUNCTION(module)  int ZEND_MSHUTDOWN(module)(SHUTDOWN_FUNC_ARGS)
#define ZEND_RINIT_FUNCTION(module)      int ZEND_RINIT(module)(INIT_FUNC_ARGS)
#define ZEND_RSHUTDOWN_FUNCTION(module)  int ZEND_RSHUTDOWN(module)(SHUTDOWN_FUNC_ARGS)
#define ZEND_MINFO_FUNCTION(module)      void ZEND_MINFO(module)(ZEND_MODULE_INFO_FUNC_ARGS)
#define ZEND_GINIT_FUNCTION(module)      int ZEND_GINIT(module)(GINIT_FUNC_ARGS)
#define ZEND_GSHUTDOWN_FUNCTION(module)  int ZEND_GSHUTDOWN(module)(void)

#define ZEND_GET_MODULE(name) \
	ZEND_DLEXPORT zend_module_entry *get_module(void) { return &name##_module_entry; }

#define ZEND_BEGIN_MODULE_GLOBALS(module_name)		\
	typedef struct _zend_##module_name##_globals {
#define ZEND_END_MODULE_GLOBALS(module_name)		\
	} zend_##module_name##_globals;

#ifdef ZTS

#define ZEND_DECLARE_MODULE_GLOBALS(module_name)							\
	static ts_rsrc_id module_name##_globals_id;
#define ZEND_INIT_MODULE_GLOBALS(module_name, globals_ctor, globals_dtor)	\
	module_name##_globals_id = ts_allocate_id(sizeof(zend_##module_name##_globals), (ts_allocate_ctor) globals_ctor, (ts_allocate_dtor) globals_dtor);

#else

#define ZEND_DECLARE_MODULE_GLOBALS(module_name)							\
	static zend_##module_name##_globals module_name##_globals;
#define ZEND_INIT_MODULE_GLOBALS(module_name, globals_ctor, globals_dtor)	\
	globals_ctor(&module_name##_globals);

#endif



#define INIT_CLASS_ENTRY(class_container, class_name, functions)	\
	{																\
		class_container.name = strdup(class_name);					\
		class_container.name_length = sizeof(class_name)-1;			\
		class_container.builtin_functions = functions;				\
		class_container.handle_function_call = NULL;				\
		class_container.handle_property_get = NULL;					\
		class_container.handle_property_set = NULL;					\
	}

#define INIT_OVERLOADED_CLASS_ENTRY(class_container, class_name, functions, handle_fcall, handle_propget, handle_propset) \
	{															\
		class_container.name = strdup(class_name);				\
		class_container.name_length = sizeof(class_name)-1;		\
		class_container.builtin_functions = functions;			\
		class_container.handle_function_call = handle_fcall;	\
		class_container.handle_property_get = handle_propget;	\
		class_container.handle_property_set = handle_propset;	\
	}



int zend_next_free_module(void);

ZEND_API int zend_get_parameters(int ht, int param_count, ...);
ZEND_API int zend_get_parameters_array(int ht, int param_count, zval **argument_array);
ZEND_API int zend_get_parameters_ex(int param_count, ...);
ZEND_API int zend_get_parameters_array_ex(int param_count, zval ***argument_array);

ZEND_API int ParameterPassedByReference(int ht, uint n);

int zend_register_functions(zend_function_entry *functions, HashTable *function_table, int type);
void zend_unregister_functions(zend_function_entry *functions, int count, HashTable *function_table);
ZEND_API int zend_register_module(zend_module_entry *module_entry);

ZEND_API zend_class_entry *zend_register_internal_class(zend_class_entry *class_entry);
ZEND_API zend_class_entry *zend_register_internal_class_ex(zend_class_entry *class_entry, zend_class_entry *parent_ce, char *parent_name);

ZEND_API zend_module_entry *zend_get_module(int module_number);
ZEND_API int zend_disable_function(char *function_name, uint function_name_length);

ZEND_API void wrong_param_count(void);

#define getThis() (this_ptr)

#define WRONG_PARAM_COUNT					ZEND_WRONG_PARAM_COUNT()
#define WRONG_PARAM_COUNT_WITH_RETVAL(ret)	ZEND_WRONG_PARAM_COUNT_WITH_RETVAL(ret)
#define ARG_COUNT(dummy)	(ht)
#define ZEND_NUM_ARGS()		(ht)
#define ZEND_WRONG_PARAM_COUNT()					{ wrong_param_count(); return; }
#define ZEND_WRONG_PARAM_COUNT_WITH_RETVAL(ret)		{ wrong_param_count(); return ret; }

#ifndef ZEND_WIN32
#define DLEXPORT
#endif

ZEND_API int zend_startup_module(zend_module_entry *module);

#define array_init(arg)			_array_init((arg) ZEND_FILE_LINE_CC)
#define object_init(arg)		_object_init((arg) ZEND_FILE_LINE_CC)
#define object_init_ex(arg, ce)	_object_init_ex((arg), (ce) ZEND_FILE_LINE_CC)
ZEND_API int _array_init(zval *arg ZEND_FILE_LINE_DC);
ZEND_API int _object_init(zval *arg ZEND_FILE_LINE_DC);
ZEND_API int _object_init_ex(zval *arg, zend_class_entry *ce ZEND_FILE_LINE_DC);

/* no longer supported */
ZEND_API int add_assoc_function(zval *arg, char *key,void (*function_ptr)(INTERNAL_FUNCTION_PARAMETERS));

ZEND_API int add_assoc_long(zval *arg, char *key, long n);
ZEND_API int add_assoc_unset(zval *arg, char *key);
ZEND_API int add_assoc_bool(zval *arg, char *key, int b);
ZEND_API int add_assoc_resource(zval *arg, char *key, int r);
ZEND_API int add_assoc_double(zval *arg, char *key, double d);
ZEND_API int add_assoc_string(zval *arg, char *key, char *str, int duplicate);
ZEND_API int add_assoc_stringl(zval *arg, char *key, char *str, uint length, int duplicate);

ZEND_API int add_index_long(zval *arg, uint idx, long n);
ZEND_API int add_index_unset(zval *arg, uint idx);
ZEND_API int add_index_bool(zval *arg, uint idx, int b);
ZEND_API int add_index_resource(zval *arg, uint idx, int r);
ZEND_API int add_index_double(zval *arg, uint idx, double d);
ZEND_API int add_index_string(zval *arg, uint idx, char *str, int duplicate);
ZEND_API int add_index_stringl(zval *arg, uint idx, char *str, uint length, int duplicate);

ZEND_API int add_next_index_long(zval *arg, long n);
ZEND_API int add_next_index_unset(zval *arg);
ZEND_API int add_next_index_bool(zval *arg, int b);
ZEND_API int add_next_index_resource(zval *arg, int r);
ZEND_API int add_next_index_double(zval *arg, double d);
ZEND_API int add_next_index_string(zval *arg, char *str, int duplicate);
ZEND_API int add_next_index_stringl(zval *arg, char *str, uint length, int duplicate);

ZEND_API int add_get_index_long(zval *arg, uint idx, long l, void **dest);
ZEND_API int add_get_index_double(zval *arg, uint idx, double d, void **dest);
ZEND_API int add_get_assoc_string(zval *arg, char *key, char *str, void **dest, int duplicate);
ZEND_API int add_get_assoc_stringl(zval *arg, char *key, char *str, uint length, void **dest, int duplicate);
ZEND_API int add_get_index_string(zval *arg, uint idx, char *str, void **dest, int duplicate);
ZEND_API int add_get_index_stringl(zval *arg, uint idx, char *str, uint length, void **dest, int duplicate);

ZEND_API int call_user_function(HashTable *function_table, zval **object_pp, zval *function_name, zval *retval_ptr, int param_count, zval *params[]);
ZEND_API int call_user_function_ex(HashTable *function_table, zval **object_pp, zval *function_name, zval **retval_ptr_ptr, int param_count, zval **params[], int no_separation, HashTable *symbol_table);

ZEND_API int add_property_long(zval *arg, char *key, long l);
ZEND_API int add_property_unset(zval *arg, char *key);
ZEND_API int add_property_bool(zval *arg, char *key, int b);
ZEND_API int add_property_resource(zval *arg, char *key, long r);
ZEND_API int add_property_double(zval *arg, char *key, double d);
ZEND_API int add_property_string(zval *arg, char *key, char *str, int duplicate);
ZEND_API int add_property_stringl(zval *arg, char *key, char *str, uint length, int duplicate);

ZEND_API int zend_set_hash_symbol(zval *symbol, char *name, int name_length,
                                  int is_ref, int num_symbol_tables, ...);

#define add_method(arg,key,method)	add_assoc_function((arg),(key),(method))

#define ZVAL_RESOURCE(z,l) {			\
		(z)->type = IS_RESOURCE;        \
		(z)->value.lval = l;	        \
	}
#define ZVAL_BOOL(z,b) {				\
		(z)->type = IS_BOOL;	        \
		(z)->value.lval = b;	        \
	}
#define ZVAL_NULL(z) {					\
		(z)->type = IS_NULL;	        \
	}
#define ZVAL_LONG(z,l) {				\
		(z)->type = IS_LONG;	        \
		(z)->value.lval = l;	        \
	}
#define ZVAL_DOUBLE(z,d) {				\
		(z)->type = IS_DOUBLE;	        \
		(z)->value.dval = d;	        \
	}
#define ZVAL_STRING(z,s,duplicate) {	\
		char *__s=(s);					\
		(z)->value.str.len = strlen(__s);	\
		(z)->value.str.val = (duplicate?estrndup(__s,(z)->value.str.len):__s);	\
		(z)->type = IS_STRING;	        \
	}
#define ZVAL_STRINGL(z,s,l,duplicate) {	\
		char *__s=(s); int __l=l;		\
		(z)->value.str.len = __l;	    \
		(z)->value.str.val = (duplicate?estrndup(__s,__l):__s);	\
		(z)->type = IS_STRING;		    \
	}

#define ZVAL_EMPTY_STRING(z) {	        \
		(z)->value.str.len = 0;  	    \
		(z)->value.str.val = empty_string; \
		(z)->type = IS_STRING;		    \
	}

#define ZVAL_FALSE  { (z)->value.lval = 0;  (z)->type = IS_BOOL; }
#define ZVAL_TRUE   { (z)->value.lval = 1;  (z)->type = IS_BOOL; }

#define RETVAL_RESOURCE(l) {			\
		return_value->type = IS_RESOURCE;\
		return_value->value.lval = l;	\
	}
#define RETVAL_BOOL(b) {				\
		return_value->type = IS_BOOL;	\
		return_value->value.lval = b;	\
	}
#define RETVAL_NULL() {					\
		return_value->type = IS_NULL;	\
	}
#define RETVAL_LONG(l) {				\
		return_value->type = IS_LONG;	\
		return_value->value.lval = l;	\
	}
#define RETVAL_DOUBLE(d) {				\
		return_value->type = IS_DOUBLE;	\
		return_value->value.dval = d;	\
	}
#define RETVAL_STRING(s,duplicate) {	\
		char *__s=(s);					\
		return_value->value.str.len = strlen(__s);	\
		return_value->value.str.val = (duplicate?estrndup(__s,return_value->value.str.len):__s);	\
		return_value->type = IS_STRING;	\
	}
#define RETVAL_STRINGL(s,l,duplicate) {		\
		char *__s=(s); int __l=l;			\
		return_value->value.str.len = __l;	\
		return_value->value.str.val = (duplicate?estrndup(__s,__l):__s);	\
		return_value->type = IS_STRING;		\
	}

#define RETVAL_EMPTY_STRING() {	            \
		return_value->value.str.len = 0;  	\
		return_value->value.str.val = empty_string; \
		return_value->type = IS_STRING;		\
	}

#define RETVAL_FALSE  { return_value->value.lval = 0;  return_value->type = IS_BOOL; }
#define RETVAL_TRUE   { return_value->value.lval = 1;  return_value->type = IS_BOOL; }

#define RETURN_RESOURCE(l) {			\
		return_value->type = IS_RESOURCE;\
		return_value->value.lval = l;	\
		return;							\
	}

#define RETURN_BOOL(b) {				\
		return_value->type = IS_BOOL;	\
		return_value->value.lval = b;	\
		return;							\
	}

#define RETURN_NULL() {				\
		return_value->type = IS_NULL;	\
		return;							\
	}

#define RETURN_LONG(l) {				\
		return_value->type = IS_LONG;	\
		return_value->value.lval = l;	\
		return;							\
	}
#define RETURN_DOUBLE(d) {				\
		return_value->type = IS_DOUBLE;	\
		return_value->value.dval = d;	\
		return;							\
	}
#define RETURN_STRING(s,duplicate) {	\
		char *__s=(s);					\
		return_value->value.str.len = strlen(__s);	\
		return_value->value.str.val = (duplicate?estrndup(__s,return_value->value.str.len):__s);	\
		return_value->type = IS_STRING;	\
		return;							\
	}
#define RETURN_STRINGL(s,l,duplicate) {		\
		char *__s=(s); int __l=l;			\
		return_value->value.str.len = __l;	\
		return_value->value.str.val = (duplicate?estrndup(__s,__l):__s);	\
      	return_value->type = IS_STRING;		\
		return;								\
	}

#define RETURN_EMPTY_STRING() {	            \
		return_value->value.str.len = 0;  	\
		return_value->value.str.val = empty_string; \
		return_value->type = IS_STRING;		\
        return;                             \
	}

#define RETURN_FALSE  { RETVAL_FALSE; return; }
#define RETURN_TRUE   { RETVAL_TRUE; return; }

#define SET_VAR_STRING(n,v)	{																				\
								{																			\
									zval *var;																\
									char *str=(v); /* prevent 'v' from being evaluated more than once */	\
																											\
									ALLOC_ZVAL(var);														\
									var->value.str.val = (str);												\
									var->value.str.len = strlen((str));										\
									var->type = IS_STRING;													\
									ZEND_SET_GLOBAL_VAR(n, var);											\
								}																			\
							}

#define SET_VAR_STRINGL(n,v,l)	{														\
									{													\
										zval *var;										\
																						\
										ALLOC_ZVAL(var);								\
										var->value.str.val = (v);						\
										var->value.str.len = (l);						\
										var->type = IS_STRING;							\
										ZEND_SET_GLOBAL_VAR(n, var);					\
									}													\
								}

#define SET_VAR_LONG(n,v)	{															\
								{														\
									zval *var;											\
																						\
									ALLOC_ZVAL(var);									\
									var->value.lval = (v);								\
									var->type = IS_LONG;								\
									ZEND_SET_GLOBAL_VAR(n, var);						\
								}														\
							}

#define SET_VAR_DOUBLE(n,v)	{															\
								{														\
									zval *var;											\
																						\
									ALLOC_ZVAL(var);									\
									var->value.dval = (v);								\
									var->type = IS_DOUBLE;								\
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

#define HASH_OF(p) ((p)->type==IS_ARRAY ? (p)->value.ht : (((p)->type==IS_OBJECT ? (p)->value.obj.properties : NULL)))
#define ZVAL_IS_NULL(z) ((z)->type==IS_NULL)

#endif /* ZEND_API_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
