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
#include "zend_constants.h"
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


static int clean_non_persistent_constant(zend_constant *c)
{
	if (c->flags & CONST_PERSISTENT) {
		return 0;
	} else {
		return 1;
	}
}


static int clean_module_constant(zend_constant *c, int *module_number)
{
	if (c->module_number == *module_number) {
		return 1;
	} else {
		return 0;
	}
}


void clean_module_constants(int module_number)
{
	ELS_FETCH();

	zend_hash_apply_with_argument(EG(zend_constants), (int (*)(void *,void *)) clean_module_constant, (void *) &module_number);
}


int zend_startup_constants(ELS_D)
{
#if WIN32|WINNT
	DWORD dwBuild=0;
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
#endif


/* ZEND_FIX:  Move to PHP */
#if 0
#if WIN32|WINNT
	// Get build numbers for Windows NT or Win95
	if (dwVersion < 0x80000000){
		php3_os="WINNT";
	} else {
		php3_os="WIN32";
	}
#else
	php3_os=PHP_OS;
#endif
#endif


	EG(zend_constants) = (HashTable *) malloc(sizeof(HashTable));

	if (zend_hash_init(EG(zend_constants), 20, NULL, (void(*)(void *)) free_zend_constant, 1)==FAILURE) {
		return FAILURE;
	}

#if 0
	/* This should go back to PHP */
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_VERSION", PHP_VERSION, sizeof(PHP_VERSION)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_OS", php3_os, strlen(php3_os), CONST_PERSISTENT | CONST_CS);
#endif
	REGISTER_MAIN_LONG_CONSTANT("E_ERROR", E_ERROR, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_WARNING", E_WARNING, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_NOTICE", E_NOTICE, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_PARSE", E_PARSE, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_ALL", E_ALL, CONST_PERSISTENT | CONST_CS);

	/* true/false constants */
	{
		zend_constant c;
	
		c.value.type = IS_BOOL;
		c.flags = CONST_PERSISTENT;
		c.module_number = 0;

		c.name = zend_strndup("TRUE",4);
		c.name_len = 5;
		c.value.value.lval = 1;
		c.value.type = IS_BOOL;
		zend_register_constant(&c ELS_CC);
		
		c.name = zend_strndup("FALSE",5);
		c.name_len = 6;
		c.value.value.lval = 0;
		c.value.type = IS_BOOL;
		zend_register_constant(&c ELS_CC);
	}

	return SUCCESS;
}


int zend_shutdown_constants(ELS_D)
{
	zend_hash_destroy(EG(zend_constants));
	free(EG(zend_constants));
	return SUCCESS;
}


void clean_non_persistent_constants(void)
{
	ELS_FETCH();

	zend_hash_apply(EG(zend_constants), (int (*)(void *)) clean_non_persistent_constant);
}


ZEND_API void zend_register_long_constant(char *name, uint name_len, long lval, int flags, int module_number ELS_DC)
{
	zend_constant c;
	
	c.value.type = IS_LONG;
	c.value.value.lval = lval;
	c.flags = flags;
	c.name = zend_strndup(name,name_len);
	c.name_len = name_len;
	c.module_number = module_number;
	zend_register_constant(&c ELS_CC);
}


ZEND_API void zend_register_double_constant(char *name, uint name_len, double dval, int flags, int module_number ELS_DC)
{
	zend_constant c;
	
	c.value.type = IS_DOUBLE;
	c.value.value.dval = dval;
	c.flags = flags;
	c.name = zend_strndup(name,name_len);
	c.name_len = name_len;
	c.module_number = module_number;
	zend_register_constant(&c ELS_CC);
}


ZEND_API void zend_register_stringl_constant(char *name, uint name_len, char *strval, uint strlen, int flags, int module_number ELS_DC)
{
	zend_constant c;
	
	c.value.type = IS_STRING;
	c.value.value.str.val = strval;
	c.value.value.str.len = strlen;
	c.flags = flags;
	c.name = zend_strndup(name,name_len);
	c.name_len = name_len;
	c.module_number = module_number;
	zend_register_constant(&c ELS_CC);
}


ZEND_API void zend_register_string_constant(char *name, uint name_len, char *strval, int flags, int module_number ELS_DC)
{
	zend_register_stringl_constant(name, name_len, strval, strlen(strval), flags, module_number ELS_CC);
}


ZEND_API int zend_get_constant(char *name, uint name_len, zval *result)
{
	zend_constant *c;
	char *lookup_name = estrndup(name,name_len);
	int retval;
	ELS_FETCH();

	zend_str_tolower(lookup_name, name_len);

	if (zend_hash_find(EG(zend_constants), lookup_name, name_len+1, (void **) &c)==SUCCESS) {
		if ((c->flags & CONST_CS) && memcmp(c->name, name, name_len)!=0) {
			retval=0;
		} else {
			retval=1;
			*result = c->value;
			zval_copy_ctor(result);
		}
	} else {
		retval=0;
	}
	
	efree(lookup_name);
	return retval;
}


ZEND_API void zend_register_constant(zend_constant *c ELS_DC)
{
	char *lowercase_name = zend_strndup(c->name, c->name_len);

#if 0
	printf("Registering constant for module %d\n",c->module_number);
#endif

	zend_str_tolower(lowercase_name, c->name_len);
	if (zend_hash_add(EG(zend_constants), lowercase_name, c->name_len, (void *) c, sizeof(zend_constant), NULL)==FAILURE) {
		zval_dtor(&c->value);
	}
	free(lowercase_name);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
