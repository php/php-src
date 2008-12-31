/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2009 Zend Technologies Ltd. (http://www.zend.com) |
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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_variables.h"
#include "zend_operators.h"
#include "zend_globals.h"


void free_zend_constant(zend_constant *c)
{
	if (!(c->flags & CONST_PERSISTENT)) {
		zval_dtor(&c->value);
	}
	free(c->name);
}


void copy_zend_constant(zend_constant *c)
{
	c->name = zend_strndup(c->name, c->name_len - 1);
	if (!(c->flags & CONST_PERSISTENT)) {
		zval_copy_ctor(&c->value);
	}
}


void zend_copy_constants(HashTable *target, HashTable *source)
{
	zend_constant tmp_constant;

	zend_hash_copy(target, source, (copy_ctor_func_t) copy_zend_constant, &tmp_constant, sizeof(zend_constant));
}


static int clean_non_persistent_constant(zend_constant *c TSRMLS_DC)
{
	return (c->flags & CONST_PERSISTENT) ? ZEND_HASH_APPLY_STOP : ZEND_HASH_APPLY_REMOVE;
}


static int clean_non_persistent_constant_full(zend_constant *c TSRMLS_DC)
{
	return (c->flags & CONST_PERSISTENT) ? 0 : 1;
}


static int clean_module_constant(zend_constant *c, int *module_number TSRMLS_DC)
{
	if (c->module_number == *module_number) {
		return 1;
	} else {
		return 0;
	}
}


void clean_module_constants(int module_number TSRMLS_DC)
{
	zend_hash_apply_with_argument(EG(zend_constants), (apply_func_arg_t) clean_module_constant, (void *) &module_number TSRMLS_CC);
}


int zend_startup_constants(TSRMLS_D)
{
	EG(zend_constants) = (HashTable *) malloc(sizeof(HashTable));

	if (zend_hash_init(EG(zend_constants), 20, NULL, ZEND_CONSTANT_DTOR, 1)==FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}



void zend_register_standard_constants(TSRMLS_D)
{
	REGISTER_MAIN_LONG_CONSTANT("E_ERROR", E_ERROR, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_RECOVERABLE_ERROR", E_RECOVERABLE_ERROR, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_WARNING", E_WARNING, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_PARSE", E_PARSE, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_NOTICE", E_NOTICE, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_STRICT", E_STRICT, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_CORE_ERROR", E_CORE_ERROR, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_CORE_WARNING", E_CORE_WARNING, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_COMPILE_ERROR", E_COMPILE_ERROR, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_COMPILE_WARNING", E_COMPILE_WARNING, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_USER_ERROR", E_USER_ERROR, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_USER_WARNING", E_USER_WARNING, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_USER_NOTICE", E_USER_NOTICE, CONST_PERSISTENT | CONST_CS);

	REGISTER_MAIN_LONG_CONSTANT("E_ALL", E_ALL, CONST_PERSISTENT | CONST_CS);

	/* true/false constants */
	{
		zend_constant c;
	
		c.flags = CONST_PERSISTENT | CONST_CT_SUBST;
		c.module_number = 0;

		c.name = zend_strndup(ZEND_STRL("TRUE"));
		c.name_len = sizeof("TRUE");
		c.value.value.lval = 1;
		c.value.type = IS_BOOL;
		zend_register_constant(&c TSRMLS_CC);
		
		c.name = zend_strndup(ZEND_STRL("FALSE"));
		c.name_len = sizeof("FALSE");
		c.value.value.lval = 0;
		c.value.type = IS_BOOL;
		zend_register_constant(&c TSRMLS_CC);

		c.name = zend_strndup(ZEND_STRL("NULL"));
		c.name_len = sizeof("NULL");
		c.value.type = IS_NULL;
		zend_register_constant(&c TSRMLS_CC);

		c.flags = CONST_PERSISTENT;

		c.name = zend_strndup(ZEND_STRL("ZEND_THREAD_SAFE"));
		c.name_len = sizeof("ZEND_THREAD_SAFE");
		c.value.value.lval = ZTS_V;
		c.value.type = IS_BOOL;
		zend_register_constant(&c TSRMLS_CC);
	}
}


int zend_shutdown_constants(TSRMLS_D)
{
	zend_hash_destroy(EG(zend_constants));
	free(EG(zend_constants));
	return SUCCESS;
}


void clean_non_persistent_constants(TSRMLS_D)
{
	if (EG(full_tables_cleanup)) {
		zend_hash_apply(EG(zend_constants), (apply_func_t) clean_non_persistent_constant_full TSRMLS_CC);
	} else {
		zend_hash_reverse_apply(EG(zend_constants), (apply_func_t) clean_non_persistent_constant TSRMLS_CC);
	}
}


ZEND_API void zend_register_long_constant(char *name, uint name_len, long lval, int flags, int module_number TSRMLS_DC)
{
	zend_constant c;
	
	c.value.type = IS_LONG;
	c.value.value.lval = lval;
	c.flags = flags;
	c.name = zend_strndup(name, name_len-1);
	c.name_len = name_len;
	c.module_number = module_number;
	zend_register_constant(&c TSRMLS_CC);
}


ZEND_API void zend_register_double_constant(char *name, uint name_len, double dval, int flags, int module_number TSRMLS_DC)
{
	zend_constant c;
	
	c.value.type = IS_DOUBLE;
	c.value.value.dval = dval;
	c.flags = flags;
	c.name = zend_strndup(name, name_len-1);
	c.name_len = name_len;
	c.module_number = module_number;
	zend_register_constant(&c TSRMLS_CC);
}


ZEND_API void zend_register_stringl_constant(char *name, uint name_len, char *strval, uint strlen, int flags, int module_number TSRMLS_DC)
{
	zend_constant c;
	
	c.value.type = IS_STRING;
	c.value.value.str.val = strval;
	c.value.value.str.len = strlen;
	c.flags = flags;
	c.name = zend_strndup(name, name_len-1);
	c.name_len = name_len;
	c.module_number = module_number;
	zend_register_constant(&c TSRMLS_CC);
}


ZEND_API void zend_register_string_constant(char *name, uint name_len, char *strval, int flags, int module_number TSRMLS_DC)
{
	zend_register_stringl_constant(name, name_len, strval, strlen(strval), flags, module_number TSRMLS_CC);
}


ZEND_API int zend_get_constant_ex(char *name, uint name_len, zval *result, zend_class_entry *scope TSRMLS_DC)
{
	zend_constant *c;
	int retval = 1;
	char *lookup_name;
	char *colon;

	if ((colon = memchr(name, ':', name_len)) && colon[1] == ':') {
		/* class constant */
		zend_class_entry **ce = NULL;
		int class_name_len = colon-name;
		int const_name_len = name_len - class_name_len - 2;
		char *constant_name = colon+2;
		zval **ret_constant;
		char *class_name;

		if (!scope) {
			if (EG(in_execution)) {
				scope = EG(scope);
			} else {
				scope = CG(active_class_entry);
			}
		}
	
		class_name = estrndup(name, class_name_len);

		if (class_name_len == sizeof("self")-1 && strcmp(class_name, "self") == 0) {
			if (scope) {
				ce = &scope;
			} else {
				zend_error(E_ERROR, "Cannot access self:: when no class scope is active");
				retval = 0;
			}
		} else if (class_name_len == sizeof("parent")-1 && strcmp(class_name, "parent") == 0) {
			if (!scope) {   	 
				zend_error(E_ERROR, "Cannot access parent:: when no class scope is active");
			} else if (!scope->parent) { 	 
				zend_error(E_ERROR, "Cannot access parent:: when current class scope has no parent"); 	 
			} else {
				ce = &scope->parent;
			}
		} else {
			if (zend_lookup_class(class_name, class_name_len, &ce TSRMLS_CC) != SUCCESS) {
				retval = 0;
			}
		}

		if (retval && ce) {
			if (zend_hash_find(&((*ce)->constants_table), constant_name, const_name_len+1, (void **) &ret_constant) != SUCCESS) {
				retval = 0;
			}
		} else {
			zend_error(E_ERROR, "Class '%s' not found", class_name);
			retval = 0;
		}
		efree(class_name);

		if (retval) {
			zval_update_constant_ex(ret_constant, (void*)1, *ce TSRMLS_CC);
			*result = **ret_constant;
			zval_copy_ctor(result);
			INIT_PZVAL(result);
		}

		return retval;
	}

	if (zend_hash_find(EG(zend_constants), name, name_len+1, (void **) &c) == FAILURE) {
		lookup_name = estrndup(name, name_len);
		zend_str_tolower(lookup_name, name_len);
		 
		if (zend_hash_find(EG(zend_constants), lookup_name, name_len+1, (void **) &c)==SUCCESS) {
			if ((c->flags & CONST_CS) && memcmp(c->name, name, name_len)!=0) {
				retval=0;
			}
		} else {
			char haltoff[] = "__COMPILER_HALT_OFFSET__";
			if (!EG(in_execution)) {
				retval = 0;
			} else if (name_len == sizeof("__COMPILER_HALT_OFFSET__") - 1 && memcmp(haltoff, name, name_len) == 0) {
				char *cfilename, *haltname;
				int len, clen;
				cfilename = zend_get_executed_filename(TSRMLS_C);
				clen = strlen(cfilename);
				/* check for __COMPILER_HALT_OFFSET__ */
				zend_mangle_property_name(&haltname, &len, haltoff,
					sizeof("__COMPILER_HALT_OFFSET__") - 1, cfilename, clen, 0);
				if (zend_hash_find(EG(zend_constants), haltname, len+1, (void **) &c) == SUCCESS) {
					retval = 1;
				} else {
					retval=0;
				}
				pefree(haltname, 0);
			} else {
				retval = 0;
			}
		}
		efree(lookup_name);
	}

	if (retval) {
		*result = c->value;
		zval_copy_ctor(result);
		result->refcount = 1;
		result->is_ref = 0;
	}

	return retval;
}

ZEND_API int zend_get_constant(char *name, uint name_len, zval *result TSRMLS_DC)
{
    return zend_get_constant_ex(name, name_len, result, NULL TSRMLS_CC);
}

ZEND_API int zend_register_constant(zend_constant *c TSRMLS_DC)
{
	char *lowercase_name = NULL;
	char *name;
	int ret = SUCCESS;

#if 0
	printf("Registering constant for module %d\n", c->module_number);
#endif

	if (!(c->flags & CONST_CS)) {
		/* keep in mind that c->name_len already contains the '\0' */
		lowercase_name = estrndup(c->name, c->name_len-1);
		zend_str_tolower(lowercase_name, c->name_len-1);
		name = lowercase_name;
	} else {
		name = c->name;
	}

	if ((strncmp(name, "__COMPILER_HALT_OFFSET__", sizeof("__COMPILER_HALT_OFFSET__") - 1) == 0) ||
			zend_hash_add(EG(zend_constants), name, c->name_len, (void *) c, sizeof(zend_constant), NULL)==FAILURE) {
		zend_error(E_NOTICE,"Constant %s already defined", name);
		free(c->name);
		if (!(c->flags & CONST_PERSISTENT)) {
			zval_dtor(&c->value);
		}
		ret = FAILURE;
	}
	if (lowercase_name) {
		efree(lowercase_name);
	}
	return ret;
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
