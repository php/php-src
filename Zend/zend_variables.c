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

#include <stdio.h>
#include "zend.h"
#include "zend_API.h"
#include "zend_globals.h"
#include "zend_constants.h"
#include "zend_variables.h"
#include "zend_list.h"

ZEND_API char *empty_string = "";	/* in order to save emalloc() and efree() time for
									 * empty strings (usually used to denote empty
									 * return values in failed functions).
									 * The macro STR_FREE() will not efree() it.
									 */

ZEND_API char *undefined_variable_string = "\0";

/* this function MUST set the value for the variable to an empty string */
/* and empty strings must be evaluated as FALSE */
ZEND_API inline void var_reset(zval *var)
{
	var->type = IS_STRING;
	var->value.str.val = empty_string;
	var->value.str.len = 0;
}

ZEND_API inline void var_uninit(zval *var)
{
	var->type = IS_STRING;
	var->value.str.val = undefined_variable_string;
	var->value.str.len = 0;
}
		

ZEND_API int zval_dtor(zval *zvalue)
{
	if (zvalue->type==IS_LONG) {
		return 1;
	}
	switch(zvalue->type) {
		case IS_STRING:
		case IS_CONSTANT:
			STR_FREE(zvalue->value.str.val);
			break;
		case IS_ARRAY: {
				ELS_FETCH();

				if (zvalue->value.ht && (zvalue->value.ht != &EG(symbol_table))) {
#ifdef USE_AI_COUNT
					if (EG(AiCount)==0) {
						zend_hash_destroy(zvalue->value.ht);
						efree(zvalue->value.ht);
					} else {
						zend_ptr_stack_push(&EG(garbage), zvalue->value.ht);
					}
#else
					zend_hash_destroy(zvalue->value.ht);
					efree(zvalue->value.ht);
#endif
				}
			}
			break;
		case IS_OBJECT: {
#ifdef USE_AI_COUNT
			ELS_FETCH();

				if (EG(AiCount)==0) {
					zend_hash_destroy(zvalue->value.obj.properties);
					efree(zvalue->value.obj.properties);
				} else {
					zend_ptr_stack_push(&EG(garbage), zvalue->value.obj.properties);
				}
#else
				zend_hash_destroy(zvalue->value.obj.properties);
				efree(zvalue->value.obj.properties);
#endif
			}
			break;
		case IS_RESOURCE:
			/* destroy resource */
			zend_list_delete(zvalue->value.lval);
			break;
		case IS_LONG:
		case IS_DOUBLE:
		case IS_BOOL:
		default:
			return 1;
			break;
	}
	return 1;
}


void zval_add_ref(zval **p)
{
	(*p)->refcount++;
}




ZEND_API int zval_copy_ctor(zval *zvalue)
{
	switch (zvalue->type) {
		case IS_RESOURCE:
			zend_list_addref(zvalue->value.lval);
			break;
		case IS_BOOL:
		case IS_LONG:
			break;
		case IS_STRING:
			if (zvalue->value.str.val) {
				if (zvalue->value.str.len==0) {
					if (zvalue->value.str.val==undefined_variable_string) {
						zvalue->value.str.val = undefined_variable_string;
					} else {
						zvalue->value.str.val = empty_string;
					}
					return SUCCESS;
				}
			}
			zvalue->value.str.val = (char *) estrndup(zvalue->value.str.val, zvalue->value.str.len);
			break;
		case IS_ARRAY: {
				zval *tmp;
				HashTable *original_ht = zvalue->value.ht;
				ELS_FETCH();

				if (!zvalue->value.ht || zvalue->value.ht==&EG(symbol_table)) {
					var_reset(zvalue);
					return FAILURE;
				}
				zvalue->value.ht = (HashTable *) emalloc(sizeof(HashTable));
				zend_hash_init(zvalue->value.ht, 0, NULL, PVAL_PTR_DTOR, 0);
				zend_hash_copy(zvalue->value.ht, original_ht, (void (*)(void *)) zval_add_ref, (void *) &tmp, sizeof(zval *));
			}
			break;
		case IS_OBJECT: {
				zval *tmp;
				HashTable *original_ht = zvalue->value.obj.properties;

				zvalue->value.obj.properties = (HashTable *) emalloc(sizeof(HashTable));
				zend_hash_init(zvalue->value.obj.properties, 0, NULL, PVAL_PTR_DTOR, 0);
				zend_hash_copy(zvalue->value.obj.properties, original_ht, (void (*)(void *)) zval_add_ref, (void *) &tmp, sizeof(zval *));
			}
			break;
	}
	return SUCCESS;
}


ZEND_API int zend_print_variable(zval *var) 
{
	return zend_print_zval(var, 0);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
