/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to the Zend license, that is bundled     |
   | with this package in the file LICENSE.  If you did not receive a     |
   | copy of the Zend license, please mail us at zend@zend.com so we can  |
   | send you a copy immediately.                                         |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_variables.h"
#include "zend_execute.h"
#include "zend_API.h"
#include "modules.h"
#include "zend_constants.h"
#include "zend_operators.h"

#if HAVE_STDARG_H
#include <stdarg.h>
#endif

/* these variables are true statics/globals, and have to be mutex'ed on every access */
static int module_count=0;
HashTable list_destructors, module_registry;

/* this function doesn't check for too many parameters */
ZEND_API int getParameters(int ht, int param_count,...)
{
	void **p;
	int arg_count;
	va_list ptr;
	zval **param, *param_ptr;
	ELS_FETCH();
	ALS_FETCH();

	p = EG(argument_stack).top_element-1;
	arg_count = (ulong) *p;

	if (param_count>arg_count) {
		return FAILURE;
	}

	va_start(ptr, param_count);

	while (param_count>0) {
		param = va_arg(ptr, zval **);
		param_ptr = *(p-param_count);
		if (!param_ptr->is_ref && param_ptr->refcount>1) {
			zval *new_tmp;

			new_tmp = (zval *) emalloc(sizeof(zval));
			*new_tmp = *param_ptr;
			zval_copy_ctor(new_tmp);
			new_tmp->refcount = 1;
			new_tmp->is_ref = 0;
			param_ptr = new_tmp;
			((zval *) *(p-param_count))->refcount--;
			*(p-param_count) = param_ptr;
		}
		*param = param_ptr;
		param_count--;
	}
	va_end(ptr);

	return SUCCESS;
}


ZEND_API int getParametersArray(int ht, int param_count, zval **argument_array)
{
	void **p;
	int arg_count;
	zval *param_ptr;
	ELS_FETCH();

	p = EG(argument_stack).top_element-1;
	arg_count = (ulong) *p;

	if (param_count>arg_count) {
		return FAILURE;
	}


	while (param_count>0) {
		param_ptr = *(p-param_count);
		if (!param_ptr->is_ref && param_ptr->refcount>1) {
			zval *new_tmp;

			new_tmp = (zval *) emalloc(sizeof(zval));
			*new_tmp = *param_ptr;
			zval_copy_ctor(new_tmp);
			new_tmp->refcount = 1;
			new_tmp->is_ref = 0;
			param_ptr = new_tmp;
			((zval *) *(p-param_count))->refcount--;
			*(p-param_count) = param_ptr;
		}
		*(argument_array++) = param_ptr;
		param_count--;
	}

	return SUCCESS;
}




/* Zend-optimized Extended functions */
/* this function doesn't check for too many parameters */
ZEND_API int getParametersEx(int param_count,...)
{
	void **p;
	int arg_count;
	va_list ptr;
	zval ***param;
	ELS_FETCH();

	p = EG(argument_stack).top_element-1;
	arg_count = (ulong) *p;

	if (param_count>arg_count) {
		return FAILURE;
	}

	va_start(ptr, param_count);
	while (param_count>0) {
		param = va_arg(ptr, zval ***);
		*param = (zval **) p-(param_count--);
	}
	va_end(ptr);

	return SUCCESS;
}


ZEND_API int getParametersArrayEx(int param_count, zval ***argument_array)
{
	void **p;
	int arg_count;
	ELS_FETCH();

	p = EG(argument_stack).top_element-1;
	arg_count = (ulong) *p;

	if (param_count>arg_count) {
		return FAILURE;
	}

	while (param_count>0) {
		*(argument_array++) = (zval **) p-(param_count--);
	}

	return SUCCESS;
}


ZEND_API int getThis(zval **this_ptr)
{
	/* NEEDS TO BE IMPLEMENTED FOR ZEND */
	/*
	zval *data;

	if (zend_hash_find(function_state.calling_symbol_table, "this", sizeof("this"), (void **)&data) == FAILURE) {
		return FAILURE;
	}

	*this = data;
	*/
	return SUCCESS;
}


ZEND_API int ParameterPassedByReference(int ht, uint n)
{
	void **p;
	ulong arg_count;
	zval *arg;
	ELS_FETCH();

	p = EG(argument_stack).elements+EG(argument_stack).top-1;
	arg_count = (ulong) *p;

	if (n>arg_count) {
		return FAILURE;
	}
	arg = (zval *) *(p-arg_count+n-1);
	return arg->is_ref;
}


ZEND_API void wrong_param_count()
{
	zend_error(E_WARNING,"Wrong parameter count for %s()",get_active_function_name());
}

	
ZEND_API inline int array_init(zval *arg)
{
	arg->value.ht = (HashTable *) emalloc(sizeof(HashTable));
	if (!arg->value.ht || zend_hash_init(arg->value.ht, 0, NULL, PVAL_PTR_DTOR, 0)) {
		zend_error(E_CORE_ERROR, "Cannot allocate memory for array");
		return FAILURE;
	}
	arg->type = IS_ARRAY;
	return SUCCESS;
}


static void zval_update_const_and_ref(zval **p)
{
	zval_update_constant(*p);
	zval_add_ref(p);
}



ZEND_API inline int object_init_ex(zval *arg, zend_class_entry *class_type)
{
	zval *tmp;

	arg->value.obj.properties = (HashTable *) emalloc(sizeof(HashTable));
	zend_hash_init(arg->value.obj.properties, 0, NULL, PVAL_PTR_DTOR, 0);
	zend_hash_copy(arg->value.obj.properties, &class_type->default_properties, (void (*)(void *)) zval_update_const_and_ref, (void *) &tmp, sizeof(zval *));
	arg->type = IS_OBJECT;
	arg->value.obj.ce = class_type;
	return SUCCESS;
}


ZEND_API inline int object_init(zval *arg)
{
	return object_init_ex(arg, &standard_class);
}


ZEND_API inline int add_assoc_long(zval *arg, char *key, long n)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_LONG;
	tmp->value.lval = n;
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_assoc_double(zval *arg, char *key, double d)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_DOUBLE;
	tmp->value.dval = d;
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_assoc_string(zval *arg, char *key, char *str, int duplicate)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_STRING;
	tmp->value.str.len = strlen(str);
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_assoc_stringl(zval *arg, char *key, char *str, uint length, int duplicate)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_STRING;
	tmp->value.str.len = length;
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_assoc_function(zval *arg, char *key,void (*function_ptr)(INTERNAL_FUNCTION_PARAMETERS))
{
	zend_error(E_WARNING, "add_assoc_function() is no longer supported");
	return FAILURE;
}


ZEND_API inline int add_index_long(zval *arg, uint index, long n)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_LONG;
	tmp->value.lval = n;
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *),NULL);
}


ZEND_API inline int add_index_double(zval *arg, uint index, double d)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_DOUBLE;
	tmp->value.dval = d;
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *),NULL);
}


ZEND_API inline int add_index_string(zval *arg, uint index, char *str, int duplicate)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_STRING;
	tmp->value.str.len = strlen(str);
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *),NULL);
}


ZEND_API inline int add_index_stringl(zval *arg, uint index, char *str, uint length, int duplicate)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_STRING;
	tmp->value.str.len = length;
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *),NULL);
}


ZEND_API inline int add_next_index_long(zval *arg, long n)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_LONG;
	tmp->value.lval = n;
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_next_index_insert(arg->value.ht, &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_next_index_double(zval *arg, double d)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_DOUBLE;
	tmp->value.dval = d;
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_next_index_insert(arg->value.ht, &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_next_index_string(zval *arg, char *str, int duplicate)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_STRING;
	tmp->value.str.len = strlen(str);
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_next_index_insert(arg->value.ht, &tmp, sizeof(zval *),NULL);
}


ZEND_API inline int add_next_index_stringl(zval *arg, char *str, uint length, int duplicate)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_STRING;
	tmp->value.str.len = length;
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_next_index_insert(arg->value.ht, &tmp, sizeof(zval *),NULL);
}


ZEND_API inline int add_get_assoc_string(zval *arg, char *key, char *str, void **dest, int duplicate)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_STRING;
	tmp->value.str.len = strlen(str);
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), dest);
}


ZEND_API inline int add_get_assoc_stringl(zval *arg, char *key, char *str, uint length, void **dest, int duplicate)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_STRING;
	tmp->value.str.len = length;
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), dest);
}


ZEND_API inline int add_get_index_long(zval *arg, uint index, long l, void **dest)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_LONG;
	tmp->value.lval= l;
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *),dest);
}


ZEND_API inline int add_get_index_double(zval *arg, uint index, double d, void **dest)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_DOUBLE;
	tmp->value.dval= d;
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *),dest);
}


ZEND_API inline int add_get_index_string(zval *arg, uint index, char *str, void **dest, int duplicate)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_STRING;
	tmp->value.str.len = strlen(str);
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *),dest);
}


ZEND_API inline int add_get_index_stringl(zval *arg, uint index, char *str, uint length, void **dest, int duplicate)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_STRING;
	tmp->value.str.len = length;
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(zval *),dest);
}


ZEND_API inline int add_property_long(zval *arg, char *key, long n)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_LONG;
	tmp->value.lval = n;
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_update(arg->value.obj.properties, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_property_double(zval *arg, char *key, double d)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_DOUBLE;
	tmp->value.dval = d;
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_update(arg->value.obj.properties, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_property_string(zval *arg, char *key, char *str, int duplicate)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_STRING;
	tmp->value.str.len = strlen(str);
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_update(arg->value.obj.properties, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API inline int add_property_stringl(zval *arg, char *key, char *str, uint length, int duplicate)
{
	zval *tmp = (zval *) emalloc(sizeof(zval));

	tmp->type = IS_STRING;
	tmp->value.str.len = length;
	if (duplicate) {
		tmp->value.str.val = estrndup(str,tmp->value.str.len);
	} else {
		tmp->value.str.val = str;
	}
	tmp->refcount=1;
	tmp->is_ref=0;
	return zend_hash_update(arg->value.obj.properties, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}


int zend_startup_module(zend_module_entry *module)
{
	if (module) {
		module->module_number = zend_next_free_module();
		if (module->module_startup_func) {
			if (module->module_startup_func(MODULE_PERSISTENT, module->module_number)==FAILURE) {
				zend_error(E_CORE_ERROR,"Unable to start %s module",module->name);
				return FAILURE;
			}
		}
		module->type = MODULE_PERSISTENT;
		zend_register_module(module);
	}
	return SUCCESS;
}


ZEND_API int _register_list_destructors(void (*list_destructor)(void *), void (*plist_destructor)(void *), int module_number)
{
	list_destructors_entry ld;
	
#if 0
	printf("Registering destructors %d for module %d\n", list_destructors.nNextFreeElement, module_number);
#endif
	
	ld.list_destructor=(void (*)(void *)) list_destructor;
	ld.plist_destructor=(void (*)(void *)) plist_destructor;
	ld.module_number = module_number;
	ld.resource_id = list_destructors.nNextFreeElement;
	
	if (zend_hash_next_index_insert(&list_destructors,(void *) &ld,sizeof(list_destructors_entry),NULL)==FAILURE) {
		return FAILURE;
	}
	return list_destructors.nNextFreeElement-1;
}


/* registers all functions in *library_functions in the function hash */
int zend_register_functions(zend_function_entry *functions)
{
	zend_function_entry *ptr = functions;
	zend_internal_function internal_function;
	int count=0,unload=0;
	CLS_FETCH();

	internal_function.type = ZEND_INTERNAL_FUNCTION;
	
	while (ptr->fname) {
		internal_function.handler = ptr->handler;
		internal_function.arg_types = ptr->func_arg_types;
		internal_function.function_name = ptr->fname;
		if (!internal_function.handler) {
			zend_error(E_CORE_WARNING,"Null function defined as active function");
			zend_unregister_functions(functions,count);
			return FAILURE;
		}
		if (zend_hash_add(CG(function_table), ptr->fname, strlen(ptr->fname)+1, &internal_function, sizeof(zend_internal_function), NULL) == FAILURE) {
			unload=1;
			break;
		}
		ptr++;
		count++;
	}
	if (unload) { /* before unloading, display all remaining bad function in the module */
		while (ptr->fname) {
			if (zend_hash_exists(CG(function_table), ptr->fname, strlen(ptr->fname)+1)) {
				zend_error(E_CORE_WARNING,"Module load failed - duplicate function name - %s",ptr->fname);
			}
			ptr++;
		}
		zend_unregister_functions(functions,count);
		return FAILURE;
	}
	return SUCCESS;
}

/* count=-1 means erase all functions, otherwise, 
 * erase the first count functions
 */
void zend_unregister_functions(zend_function_entry *functions,int count)
{
	zend_function_entry *ptr = functions;
	int i=0;
	CLS_FETCH();

	while (ptr->fname) {
		if (count!=-1 && i>=count) {
			break;
		}
#if 0
		zend_printf("Unregistering %s()\n",ptr->fname);
#endif
		zend_hash_del(CG(function_table),ptr->fname,strlen(ptr->fname)+1);
		ptr++;
		i++;
	}
}


ZEND_API int zend_register_module(zend_module_entry *module)
{
#if 0
	zend_printf("%s:  Registering module %d\n",module->name, module->module_number);
#endif
	if (module->functions && zend_register_functions(module->functions)==FAILURE) {
		zend_error(E_CORE_WARNING,"%s:  Unable to register functions, unable to load",module->name);
		return FAILURE;
	}
	module->module_started=1;
	return zend_hash_add(&module_registry, module->name,strlen(module->name)+1,(void *)module,sizeof(zend_module_entry),NULL);
}


void module_destructor(zend_module_entry *module)
{
	if (module->type == MODULE_TEMPORARY) {
		zend_hash_apply_with_argument(&list_destructors, (int (*)(void *,void *)) clean_module_resource_destructors, (void *) &(module->module_number));
		clean_module_constants(module->module_number);
	}

	if (module->request_started && module->request_shutdown_func) {
#if 0
		zend_printf("%s:  Request shutdown\n",module->name);
#endif
		module->request_shutdown_func(module->type, module->module_number);
	}
	module->request_started=0;
	if (module->module_started && module->module_shutdown_func) {
#if 0
		zend_printf("%s:  Module shutdown\n",module->name);
#endif
		module->module_shutdown_func(module->type, module->module_number);
	}
	module->module_started=0;
	if (module->functions) {
		zend_unregister_functions(module->functions,-1);
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
	if (!module->request_started && module->request_startup_func) {
#if 0
		zend_printf("%s:  Request startup\n",module->name);
#endif
		module->request_startup_func(module->type, module->module_number);
	}
	module->request_started=1;
	return 0;
}


/* for persistent modules - call request shutdown and flag NOT to erase
 * for temporary modules - do nothing, and flag to erase
 */
int module_registry_cleanup(zend_module_entry *module)
{
	switch(module->type) {
		case MODULE_PERSISTENT:
			if (module->request_started && module->request_shutdown_func) {
#if 0
				zend_printf("%s:  Request shutdown\n",module->name);
#endif
				module->request_shutdown_func(module->type, module->module_number);
			}
			module->request_started=0;
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


ZEND_API zend_class_entry *register_internal_class(zend_class_entry *class_entry)
{
	zend_class_entry *register_class;
	char *lowercase_name = zend_strndup(class_entry->name, class_entry->name_length);
	CLS_FETCH();

	zend_str_tolower(lowercase_name, class_entry->name_length);

	class_entry->type = ZEND_INTERNAL_CLASS;
	class_entry->parent = NULL;
	class_entry->refcount = (int *) malloc(sizeof(int));
	*class_entry->refcount = 1;
	zend_hash_init(&class_entry->default_properties, 0, NULL, PVAL_PTR_DTOR, 1);
	zend_hash_init(&class_entry->function_table, 0, NULL, (void (*)(void *)) destroy_zend_function, 1);

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
