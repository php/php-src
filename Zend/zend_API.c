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


#include "zend.h"
#include "zend_execute.h"
#include "zend_API.h"
#include "modules.h"
#include "zend_constants.h"

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

/* these variables are true statics/globals, and have to be mutex'ed on every access */
static int module_count=0;
ZEND_API HashTable module_registry;

/* this function doesn't check for too many parameters */
ZEND_API int zend_get_parameters(int ht, int param_count, ...)
{
	void **p;
	int arg_count;
	va_list ptr;
	zval **param, *param_ptr;
	ELS_FETCH();

	p = EG(argument_stack).top_element-2;
	arg_count = (ulong) *p;

	if (param_count>arg_count) {
		return FAILURE;
	}

	va_start(ptr, param_count);

	while (param_count-->0) {
		param = va_arg(ptr, zval **);
		param_ptr = *(p-arg_count);
		if (!PZVAL_IS_REF(param_ptr) && param_ptr->refcount>1) {
			zval *new_tmp;

			ALLOC_ZVAL(new_tmp);
			*new_tmp = *param_ptr;
			zval_copy_ctor(new_tmp);
			INIT_PZVAL(new_tmp);
			param_ptr = new_tmp;
			((zval *) *(p-arg_count))->refcount--;
			*(p-arg_count) = param_ptr;
		}
		*param = param_ptr;
		arg_count--;
	}
	va_end(ptr);

	return SUCCESS;
}


ZEND_API int zend_get_parameters_array(int ht, int param_count, zval **argument_array)
{
	void **p;
	int arg_count;
	zval *param_ptr;
	ELS_FETCH();

	p = EG(argument_stack).top_element-2;
	arg_count = (ulong) *p;

	if (param_count>arg_count) {
		return FAILURE;
	}

	while (param_count-->0) {
		param_ptr = *(p-arg_count);
		if (!PZVAL_IS_REF(param_ptr) && param_ptr->refcount>1) {
			zval *new_tmp;

			ALLOC_ZVAL(new_tmp);
			*new_tmp = *param_ptr;
			zval_copy_ctor(new_tmp);
			INIT_PZVAL(new_tmp);
			param_ptr = new_tmp;
			((zval *) *(p-arg_count))->refcount--;
			*(p-arg_count) = param_ptr;
		}
		*(argument_array++) = param_ptr;
		arg_count--;
	}

	return SUCCESS;
}




/* Zend-optimized Extended functions */
/* this function doesn't check for too many parameters */
ZEND_API int zend_get_parameters_ex(int param_count, ...)
{
	void **p;
	int arg_count;
	va_list ptr;
	zval ***param;
	ELS_FETCH();

	p = EG(argument_stack).top_element-2;
	arg_count = (ulong) *p;

	if (param_count>arg_count) {
		return FAILURE;
	}

	va_start(ptr, param_count);
	while (param_count-->0) {
		param = va_arg(ptr, zval ***);
		*param = (zval **) p-(arg_count--);
	}
	va_end(ptr);

	return SUCCESS;
}


ZEND_API int zend_get_parameters_array_ex(int param_count, zval ***argument_array)
{
	void **p;
	int arg_count;
	ELS_FETCH();

	p = EG(argument_stack).top_element-2;
	arg_count = (ulong) *p;

	if (param_count>arg_count) {
		return FAILURE;
	}

	while (param_count-->0) {
		*(argument_array++) = (zval **) p-(arg_count--);
	}

	return SUCCESS;
}


ZEND_API int ParameterPassedByReference(int ht, uint n)
{
	void **p;
	ulong arg_count;
	zval *arg;
	ELS_FETCH();

	p = EG(argument_stack).elements+EG(argument_stack).top-2;
	arg_count = (ulong) *p;

	if (n>arg_count) {
		return FAILURE;
	}
	arg = (zval *) *(p-arg_count+n-1);
	return PZVAL_IS_REF(arg);
}


ZEND_API void wrong_param_count()
{
	zend_error(E_WARNING,"Wrong parameter count for %s()",get_active_function_name());
}

	
ZEND_API inline int _array_init(zval *arg ZEND_FILE_LINE_DC)
{
	ALLOC_HASHTABLE_REL(arg->value.ht);

	if (!arg->value.ht || zend_hash_init(arg->value.ht, 0, NULL, ZVAL_PTR_DTOR, 0)) {
		zend_error(E_ERROR, "Cannot allocate memory for array");
		return FAILURE;
	}
	arg->type = IS_ARRAY;
	return SUCCESS;
}


ZEND_API inline int _object_init_ex(zval *arg, zend_class_entry *class_type ZEND_FILE_LINE_DC)
{
	zval *tmp;

	if (!class_type->constants_updated) {
		zend_hash_apply_with_argument(&class_type->default_properties, (int (*)(void *,void *)) zval_update_constant, (void *) 1);
		class_type->constants_updated = 1;
	}
	
	ALLOC_HASHTABLE_REL(arg->value.obj.properties);
	zend_hash_init(arg->value.obj.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(arg->value.obj.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	arg->type = IS_OBJECT;
	arg->value.obj.ce = class_type;
	return SUCCESS;
}


ZEND_API inline int _object_init(zval *arg ZEND_FILE_LINE_DC)
{
	return _object_init_ex(arg, &zend_standard_class_def ZEND_FILE_LINE_CC);
}


ZEND_API inline int add_assoc_function(zval *arg, char *key,void (*function_ptr)(INTERNAL_FUNCTION_PARAMETERS))
{
	zend_error(E_WARNING, "add_assoc_function() is no longer supported");
	return FAILURE;
}


ZEND_API inline int add_assoc_long(zval *arg, char *key, long n)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_LONG;
	tmp->value.lval = n;
	INIT_PZVAL(tmp);
	return zend_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_assoc_unset(zval *arg, char *key)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_NULL;
	INIT_PZVAL(tmp);
	return zend_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}

ZEND_API inline int add_assoc_bool(zval *arg, char *key, int b)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_BOOL;
	tmp->value.lval = b;
	INIT_PZVAL(tmp);
	return zend_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_assoc_resource(zval *arg, char *key, int r)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_RESOURCE;
	tmp->value.lval = r;
	INIT_PZVAL(tmp);
	return zend_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_assoc_double(zval *arg, char *key, double d)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_DOUBLE;
	tmp->value.dval = d;
	INIT_PZVAL(tmp);
	return zend_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_assoc_string(zval *arg, char *key, char *str, int duplicate)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_STRING;
	tmp->value.str.len = strlen(str);
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	INIT_PZVAL(tmp);
	return zend_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_assoc_stringl(zval *arg, char *key, char *str, uint length, int duplicate)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_STRING;
	tmp->value.str.len = length;
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	INIT_PZVAL(tmp);
	return zend_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_index_long(zval *arg, uint index, long n)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_LONG;
	tmp->value.lval = n;
	INIT_PZVAL(tmp);
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *),NULL);
}


ZEND_API inline int add_index_unset(zval *arg, uint index)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_NULL;
	INIT_PZVAL(tmp);
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_index_bool(zval *arg, uint index, int b)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_BOOL;
	tmp->value.lval = b;
	INIT_PZVAL(tmp);
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *),NULL);
}


ZEND_API inline int add_index_resource(zval *arg, uint index, int r)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_RESOURCE;
	tmp->value.lval = r;
	INIT_PZVAL(tmp);
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *),NULL);
}


ZEND_API inline int add_index_double(zval *arg, uint index, double d)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_DOUBLE;
	tmp->value.dval = d;
	INIT_PZVAL(tmp);
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *),NULL);
}


ZEND_API inline int add_index_string(zval *arg, uint index, char *str, int duplicate)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_STRING;
	tmp->value.str.len = strlen(str);
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	INIT_PZVAL(tmp);
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_index_stringl(zval *arg, uint index, char *str, uint length, int duplicate)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_STRING;
	tmp->value.str.len = length;
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	INIT_PZVAL(tmp);
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *),NULL);
}


ZEND_API inline int add_next_index_long(zval *arg, long n)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_LONG;
	tmp->value.lval = n;
	INIT_PZVAL(tmp);
	return zend_hash_next_index_insert(arg->value.ht, &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_next_index_unset(zval *arg)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_NULL;
	INIT_PZVAL(tmp);
	return zend_hash_next_index_insert(arg->value.ht, &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_next_index_bool(zval *arg, int b)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_BOOL;
	tmp->value.lval = b;
	INIT_PZVAL(tmp);
	return zend_hash_next_index_insert(arg->value.ht, &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_next_index_resource(zval *arg, int r)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_RESOURCE;
	tmp->value.lval = r;
	INIT_PZVAL(tmp);
	return zend_hash_next_index_insert(arg->value.ht, &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_next_index_double(zval *arg, double d)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_DOUBLE;
	tmp->value.dval = d;
	INIT_PZVAL(tmp);
	return zend_hash_next_index_insert(arg->value.ht, &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_next_index_string(zval *arg, char *str, int duplicate)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_STRING;
	tmp->value.str.len = strlen(str);
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	INIT_PZVAL(tmp);
	return zend_hash_next_index_insert(arg->value.ht, &tmp, sizeof(zval *),NULL);
}


ZEND_API inline int add_next_index_stringl(zval *arg, char *str, uint length, int duplicate)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_STRING;
	tmp->value.str.len = length;
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	INIT_PZVAL(tmp);
	return zend_hash_next_index_insert(arg->value.ht, &tmp, sizeof(zval *),NULL);
}


ZEND_API inline int add_get_assoc_string(zval *arg, char *key, char *str, void **dest, int duplicate)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_STRING;
	tmp->value.str.len = strlen(str);
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	INIT_PZVAL(tmp);
	return zend_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), dest);
}


ZEND_API inline int add_get_assoc_stringl(zval *arg, char *key, char *str, uint length, void **dest, int duplicate)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_STRING;
	tmp->value.str.len = length;
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	INIT_PZVAL(tmp);
	return zend_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), dest);
}


ZEND_API inline int add_get_index_long(zval *arg, uint index, long l, void **dest)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_LONG;
	tmp->value.lval = l;
	INIT_PZVAL(tmp);
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *),dest);
}


ZEND_API inline int add_get_index_double(zval *arg, uint index, double d, void **dest)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_DOUBLE;
	tmp->value.dval= d;
	INIT_PZVAL(tmp);
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *),dest);
}


ZEND_API inline int add_get_index_string(zval *arg, uint index, char *str, void **dest, int duplicate)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_STRING;
	tmp->value.str.len = strlen(str);
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	INIT_PZVAL(tmp);
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *),dest);
}


ZEND_API inline int add_get_index_stringl(zval *arg, uint index, char *str, uint length, void **dest, int duplicate)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_STRING;
	tmp->value.str.len = length;
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	INIT_PZVAL(tmp);
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *),dest);
}


ZEND_API inline int add_property_long(zval *arg, char *key, long n)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_LONG;
	tmp->value.lval = n;
	INIT_PZVAL(tmp);
	return zend_hash_update(arg->value.obj.properties, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}

ZEND_API inline int add_property_bool(zval *arg, char *key, int b)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_BOOL;
	tmp->value.lval = b;
	INIT_PZVAL(tmp);
	return zend_hash_update(arg->value.obj.properties, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}

ZEND_API inline int add_property_unset(zval *arg, char *key)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_NULL;
	INIT_PZVAL(tmp);
	return zend_hash_update(arg->value.obj.properties, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}

ZEND_API inline int add_property_resource(zval *arg, char *key, long n)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_RESOURCE;
	tmp->value.lval = n;
	INIT_PZVAL(tmp);
	return zend_hash_update(arg->value.obj.properties, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_property_double(zval *arg, char *key, double d)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_DOUBLE;
	tmp->value.dval = d;
	INIT_PZVAL(tmp);
	return zend_hash_update(arg->value.obj.properties, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_property_string(zval *arg, char *key, char *str, int duplicate)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_STRING;
	tmp->value.str.len = strlen(str);
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	INIT_PZVAL(tmp);
	return zend_hash_update(arg->value.obj.properties, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_property_stringl(zval *arg, char *key, char *str, uint length, int duplicate)
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	tmp->type = IS_STRING;
	tmp->value.str.len = length;
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	INIT_PZVAL(tmp);
	return zend_hash_update(arg->value.obj.properties, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API int zend_startup_module(zend_module_entry *module)
{
	if (module) {
		module->module_number = zend_next_free_module();
		if (module->module_startup_func) {
			ELS_FETCH();

			if (module->module_startup_func(MODULE_PERSISTENT, module->module_number ELS_CC)==FAILURE) {
				zend_error(E_CORE_ERROR,"Unable to start %s module",module->name);
				return FAILURE;
			}
		}
		module->type = MODULE_PERSISTENT;
		zend_register_module(module);
	}
	return SUCCESS;
}


/* registers all functions in *library_functions in the function hash */
int zend_register_functions(zend_function_entry *functions, HashTable *function_table, int type)
{
	zend_function_entry *ptr = functions;
	zend_function function;
	zend_internal_function *internal_function = (zend_internal_function *)&function;
	int count=0,unload=0;
	HashTable *target_function_table = function_table;
	int error_type;
	CLS_FETCH();

	if (type==MODULE_PERSISTENT) {
		error_type = E_CORE_WARNING;
	} else {
		error_type = E_WARNING;
	}

	if (!target_function_table) {
		target_function_table = CG(function_table);
	}
	internal_function->type = ZEND_INTERNAL_FUNCTION;
	
	while (ptr->fname) {
		internal_function->handler = ptr->handler;
		internal_function->arg_types = ptr->func_arg_types;
		internal_function->function_name = ptr->fname;
		if (!internal_function->handler) {
			zend_error(error_type, "Null function defined as active function");
			zend_unregister_functions(functions, count, target_function_table);
			return FAILURE;
		}
		if (zend_hash_add(target_function_table, ptr->fname, strlen(ptr->fname)+1, &function, sizeof(zend_function), NULL) == FAILURE) {
			unload=1;
			break;
		}
		ptr++;
		count++;
	}
	if (unload) { /* before unloading, display all remaining bad function in the module */
		while (ptr->fname) {
			if (zend_hash_exists(target_function_table, ptr->fname, strlen(ptr->fname)+1)) {
				zend_error(error_type, "Function registration failed - duplicate name - %s",ptr->fname);
			}
			ptr++;
		}
		zend_unregister_functions(functions, count, target_function_table);
		return FAILURE;
	}
	return SUCCESS;
}

/* count=-1 means erase all functions, otherwise, 
 * erase the first count functions
 */
void zend_unregister_functions(zend_function_entry *functions, int count, HashTable *function_table)
{
	zend_function_entry *ptr = functions;
	int i=0;
	HashTable *target_function_table = function_table;
	CLS_FETCH();

	if (!target_function_table) {
		target_function_table = CG(function_table);
	}
	while (ptr->fname) {
		if (count!=-1 && i>=count) {
			break;
		}
#if 0
		zend_printf("Unregistering %s()\n", ptr->fname);
#endif
		zend_hash_del(target_function_table, ptr->fname, strlen(ptr->fname)+1);
		ptr++;
		i++;
	}
}


ZEND_API int zend_register_module(zend_module_entry *module)
{
#if 0
	zend_printf("%s:  Registering module %d\n",module->name, module->module_number);
#endif
	if (module->functions && zend_register_functions(module->functions, NULL, module->type)==FAILURE) {
		zend_error(E_CORE_WARNING,"%s:  Unable to register functions, unable to load",module->name);
		return FAILURE;
	}
	module->module_started=1;
	return zend_hash_add(&module_registry, module->name,strlen(module->name)+1,(void *)module,sizeof(zend_module_entry),NULL);
}


void module_destructor(zend_module_entry *module)
{
	if (module->type == MODULE_TEMPORARY) {
		zend_clean_module_rsrc_dtors(module->module_number);
		clean_module_constants(module->module_number);
		if (module->request_shutdown_func)
			module->request_shutdown_func(module->type, module->module_number);
	}

	if (module->module_started && module->module_shutdown_func) {
#if 0
		zend_printf("%s:  Module shutdown\n",module->name);
#endif
		module->module_shutdown_func(module->type, module->module_number);
	}
	module->module_started=0;
	if (module->functions) {
		zend_unregister_functions(module->functions, -1, NULL);
	}

#if HAVE_LIBDL
	if (module->handle) {
		dlclose(module->handle);
	}
#endif
}


/* call request startup for all modules */
int module_registry_request_startup(zend_module_entry *module)
{
	if (module->request_startup_func) {
		ELS_FETCH();

#if 0
		zend_printf("%s:  Request startup\n",module->name);
#endif
		if (module->request_startup_func(module->type, module->module_number ELS_CC)==FAILURE) {
			zend_error(E_WARNING, "request_startup() for %s module failed", module->name);
			exit(1);
		}
	}
	return 0;
}


/* for persistent modules - call request shutdown and flag NOT to erase
 * for temporary modules - do nothing, and flag to erase
 */
int module_registry_cleanup(zend_module_entry *module)
{
	switch(module->type) {
		case MODULE_PERSISTENT:
			if (module->request_shutdown_func) {
#if 0
				zend_printf("%s:  Request shutdown\n",module->name);
#endif
				module->request_shutdown_func(module->type, module->module_number);
			}
			return 0;
			break;
		case MODULE_TEMPORARY:
			return 1;
			break;
	}
	return 0;
}


/* return the next free module number */
int zend_next_free_module(void)
{
	return ++module_count;
}

/* If parent_ce is not NULL then it inherits from parent_ce
 * If parent_ce is NULL and parent_name isn't then it looks for the parent and inherits from it
 * If both parent_ce and parent_name are NULL it does a regular class registration
 * If parent_name is specified but not found NULL is returned
 */
ZEND_API zend_class_entry *zend_register_internal_class_ex(zend_class_entry *class_entry, zend_class_entry *parent_ce, char *parent_name)
{
	zend_class_entry *register_class;
	CLS_FETCH();

	if (!parent_ce && parent_name) {
			if (zend_hash_find(CG(class_table), parent_name, strlen(parent_name)+1, (void **) &parent_ce)==FAILURE) {
				return NULL;
			}
	}

	register_class = zend_register_internal_class(class_entry);

	if (parent_ce) {
		zend_do_inheritance(register_class, parent_ce);
	}
	return register_class;
}

ZEND_API zend_class_entry *zend_register_internal_class(zend_class_entry *class_entry)
{
	zend_class_entry *register_class;
	char *lowercase_name = zend_strndup(class_entry->name, class_entry->name_length);
	CLS_FETCH();

	zend_str_tolower(lowercase_name, class_entry->name_length);

	class_entry->type = ZEND_INTERNAL_CLASS;
	class_entry->parent = NULL;
	class_entry->refcount = (int *) malloc(sizeof(int));
	*class_entry->refcount = 1;
	class_entry->constants_updated = 0;
	zend_hash_init(&class_entry->default_properties, 0, NULL, ZVAL_PTR_DTOR, 1);
	zend_hash_init(&class_entry->function_table, 0, NULL, ZEND_FUNCTION_DTOR, 1);


	if (class_entry->builtin_functions) {
		zend_register_functions(class_entry->builtin_functions, &class_entry->function_table, MODULE_PERSISTENT);
	}

	zend_hash_update(CG(class_table), lowercase_name, class_entry->name_length+1, class_entry, sizeof(zend_class_entry), (void **) &register_class);
	free(lowercase_name);
	return register_class;
}


ZEND_API zend_module_entry *zend_get_module(int module_number)
{
	zend_module_entry *module;

	if (zend_hash_index_find(&module_registry, module_number, (void **) &module)==SUCCESS) {
		return module;
	} else {
		return NULL;
	}
}

ZEND_API int zend_set_hash_symbol(zval *symbol, char *name, int name_length,
                                  int is_ref, int num_symbol_tables, ...)
{
    HashTable  *symbol_table;
    va_list     symbol_table_list;

    if (num_symbol_tables <= 0) return FAILURE;

    symbol->is_ref = is_ref;

    va_start(symbol_table_list, num_symbol_tables);
    while(num_symbol_tables-- > 0) {
        symbol_table = va_arg(symbol_table_list, HashTable *);
        zend_hash_update(symbol_table, name, name_length + 1, &symbol, sizeof(zval *), NULL);
        zval_add_ref(&symbol);
    }
    va_end(symbol_table_list);
    return SUCCESS;
}




/* Disabled functions support */

static ZEND_FUNCTION(display_disabled_function)
{
	zend_error(E_WARNING, "%s() has been disabled for security reasons.", get_active_function_name());
}


static zend_function_entry disabled_function[] =  {
	ZEND_FE(display_disabled_function,			NULL)
	{ NULL, NULL, NULL }
};


ZEND_API int zend_disable_function(char *function_name, uint function_name_length)
{
	CLS_FETCH();

	if (zend_hash_del(CG(function_table), function_name, function_name_length+1)==FAILURE) {
		return FAILURE;
	}
	disabled_function[0].fname = function_name;
	return zend_register_functions(disabled_function, CG(function_table), MODULE_PERSISTENT);
}
