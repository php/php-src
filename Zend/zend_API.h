/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.91 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_91.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef _ZEND_API_H
#define _ZEND_API_H

#include "modules.h"
#include "zend_list.h"


#define ZEND_NAMED_FUNCTION(name) void name(INTERNAL_FUNCTION_PARAMETERS)
#define ZEND_FUNCTION(name) ZEND_NAMED_FUNCTION(zend_if_##name)

#define ZEND_NAMED_FE(runtime_name, name, arg_types) { #runtime_name, name, arg_types },
#define ZEND_FE(name, arg_types) ZEND_NAMED_FE(name, zend_if_##name, arg_types)


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

ZEND_API int getParameters(int ht, int param_count,...);
ZEND_API int getParametersArray(int ht, int param_count, zval **argument_array);
ZEND_API int getParametersEx(int param_count,...);
ZEND_API int getParametersArrayEx(int param_count, zval ***argument_array);

ZEND_API int ParameterPassedByReference(int ht, uint n);

int zend_register_functions(zend_function_entry *functions, HashTable *function_table);
void zend_unregister_functions(zend_function_entry *functions, int count, HashTable *function_table);
ZEND_API int zend_register_module(zend_module_entry *module_entry);
ZEND_API zend_class_entry *register_internal_class(zend_class_entry *class_entry);
ZEND_API zend_module_entry *zend_get_module(int module_number);

ZEND_API void wrong_param_count(void);

#define getThis() (this_ptr)

#define WRONG_PARAM_COUNT { wrong_param_count(); return; }
#define WRONG_PARAM_COUNT_WITH_RETVAL(ret) { wrong_param_count(); return ret; }
#define ARG_COUNT(ht) (ht)

#define BYREF_NONE 0
#define BYREF_FORCE 1
#define BYREF_ALLOW 2

#if !(WIN32||WINNT)
#define DLEXPORT
#endif

int zend_startup_module(zend_module_entry *module);

ZEND_API int array_init(zval *arg);
ZEND_API int object_init(zval *arg);
ZEND_API int object_init_ex(zval *arg, zend_class_entry *ce);
ZEND_API int add_assoc_long(zval *arg, char *key, long n);
ZEND_API int add_assoc_double(zval *arg, char *key, double d);
ZEND_API int add_assoc_string(zval *arg, char *key, char *str, int duplicate);
ZEND_API int add_assoc_stringl(zval *arg, char *key, char *str, uint length, int duplicate);
ZEND_API int add_assoc_function(zval *arg, char *key,void (*function_ptr)(INTERNAL_FUNCTION_PARAMETERS));
ZEND_API int add_index_long(zval *arg, uint idx, long n);
ZEND_API int add_index_double(zval *arg, uint idx, double d);
ZEND_API int add_index_string(zval *arg, uint idx, char *str, int duplicate);
ZEND_API int add_index_stringl(zval *arg, uint idx, char *str, uint length, int duplicate);
ZEND_API int add_next_index_long(zval *arg, long n);
ZEND_API int add_next_index_double(zval *arg, double d);
ZEND_API int add_next_index_string(zval *arg, char *str, int duplicate);
ZEND_API int add_next_index_stringl(zval *arg, char *str, uint length, int duplicate);

ZEND_API int add_get_assoc_string(zval *arg, char *key, char *str, void **dest, int duplicate);
ZEND_API int add_get_assoc_stringl(zval *arg, char *key, char *str, uint length, void **dest, int duplicate);
ZEND_API int add_get_index_long(zval *arg, uint idx, long l, void **dest);
ZEND_API int add_get_index_double(zval *arg, uint idx, double d, void **dest);
ZEND_API int add_get_index_string(zval *arg, uint idx, char *str, void **dest, int duplicate);
ZEND_API int add_get_index_stringl(zval *arg, uint idx, char *str, uint length, void **dest, int duplicate);

ZEND_API int call_user_function(HashTable *function_table, zval *object, zval *function_name, zval *retval, int param_count, zval *params[]);


ZEND_API int add_property_long(zval *arg, char *key, long l);
ZEND_API int add_property_double(zval *arg, char *key, double d);
ZEND_API int add_property_string(zval *arg, char *key, char *str, int duplicate);
ZEND_API int add_property_stringl(zval *arg, char *key, char *str, uint length, int duplicate);

#define add_method(arg,key,method)	add_assoc_function((arg),(key),(method))

#define RETVAL_RESOURCE(l) {			\
		return_value->type = IS_RESOURCE;\
		return_value->value.lval = l;	\
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

#define RETVAL_FALSE  { return_value->value.lval = 0;  return_value->type = IS_BOOL; }
#define RETVAL_TRUE   { return_value->value.lval = 1;  return_value->type = IS_BOOL; }

#define RETURN_RESOURCE(l) {			\
		return_value->type = IS_RESOURCE;\
		return_value->value.lval = l;	\
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

#define RETURN_FALSE  { RETVAL_FALSE; return; }
#define RETURN_TRUE   { RETVAL_TRUE; return; }

#define SET_VAR_STRING(n,v)	{																				\
								{																			\
									zval *var = (zval *) emalloc(sizeof(zval));								\
									char *str=(v); /* prevent 'v' from being evaluated more than once */	\
																											\
									var->value.str.val = (str);												\
									var->value.str.len = strlen((str));										\
									var->type = IS_STRING;													\
									ZEND_SET_GLOBAL_VAR(name, var);											\
								}																			\
							}

#define SET_VAR_STRINGL(n,v,l)	{														\
									{													\
										zval *var = (zval *) emalloc(sizeof(zval));		\
																						\
										var->value.str.val = (v);						\
										var->value.str.len = (l);						\
										var->type = IS_STRING;							\
										ZEND_SET_GLOBAL_VAR(n, var);					\
									}													\
								}

#define SET_VAR_LONG(n,v)	{															\
								{														\
									zval *var = (zval *) emalloc(sizeof(zval));			\
																						\
									var->value.lval = (v);								\
									var->type = IS_LONG;								\
									ZEND_SET_GLOBAL_VAR(n, var);						\
								}														\
							}

#define SET_VAR_DOUBLE(n,v)	{															\
								{														\
									zval *var = (zval *) emalloc(sizeof(zval));			\
																						\
									var->value.dval = (v);								\
									var->type = IS_DOUBLE;								\
									ZEND_SET_GLOBAL_VAR(n, var);						\
								}														\
							}


#define ZEND_SET_SYMBOL(symtable, name, var)									\
	{																			\
		char *_name = (name);													\
																				\
		ZEND_SET_SYMBOL_WITH_LENGTH(symtable, _name, strlen(_name)+1, var);		\
	}


#define ZEND_SET_SYMBOL_WITH_LENGTH(symtable, name, name_length, var)									\
	{																									\
		zval **orig_var;																				\
																										\
		if (zend_hash_find(symtable, (name), (name_length), (void **) &orig_var)==SUCCESS				\
			&& PZVAL_IS_REF(*orig_var)) {																\
			int locks = (*orig_var)->EA.locks;															\
			int refcount = (*orig_var)->refcount;														\
																										\
			var->refcount = (*orig_var)->refcount;														\
			var->EA.locks = (*orig_var)->EA.locks;														\
			var->EA.is_ref = 1;																			\
																										\
			zval_dtor(*orig_var);																		\
			**orig_var = *var;																			\
			efree(var);																					\
		} else {																						\
			INIT_PZVAL(var);																			\
			zend_hash_update(symtable, (name), (name_length), &var, sizeof(zval *), NULL);				\
		}																								\
	}


#define ZEND_SET_GLOBAL_VAR(name, var)				\
	ZEND_SET_SYMBOL(&EG(symbol_table), name, var)

#define ZEND_SET_GLOBAL_VAR_WITH_LENGTH(name, name_length, var)		\
	ZEND_SET_SYMBOL_WITH_LENGTH(&EG(symbol_table), name, name_length, var)

#endif							/* _ZEND_API_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
