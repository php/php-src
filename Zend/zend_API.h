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

#ifndef _ZEND_API_H
#define _ZEND_API_H

#include "modules.h"
#include "zend_list.h"


int zend_next_free_module(void);

int getParameters(int ht, int param_count,...);
int getParametersArray(int ht, int param_count, zval **argument_array);
int getParametersEx(int param_count,...);
int getParametersArrayEx(int param_count, zval ***argument_array);

int getThis(zval **this);


int ParameterPassedByReference(int ht, uint n);
int register_functions(function_entry *functions);
void unregister_functions(function_entry *functions, int count);
int register_module(zend_module_entry *module_entry);
zend_class_entry *register_internal_class(zend_class_entry *class_entry);

ZEND_API void wrong_param_count(void);

#define WRONG_PARAM_COUNT { wrong_param_count(); return; }
#define WRONG_PARAM_COUNT_WITH_RETVAL(ret) { wrong_param_count(); return ret; }
#define ARG_COUNT(ht) (ht)

#define BYREF_NONE 0
#define BYREF_FORCE 1
#define BYREF_ALLOW 2

#if !MSVC5
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

#endif							/* _ZEND_API_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
