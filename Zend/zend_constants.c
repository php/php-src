/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
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
	str_free(c->name);
}


void copy_zend_constant(zend_constant *c)
{
	if (!IS_INTERNED(c->name)) {
		c->name = zend_strndup(c->name, c->name_len - 1);
	}
	if (!(c->flags & CONST_PERSISTENT)) {
		zval_copy_ctor(&c->value);
	}
}


void zend_copy_constants(HashTable *target, HashTable *source)
{
	zend_constant tmp_constant;

	zend_hash_copy(target, source, (copy_ctor_func_t) copy_zend_constant, &tmp_constant, sizeof(zend_constant));
}


static int clean_non_persistent_constant(const zend_constant *c TSRMLS_DC)
{
	return (c->flags & CONST_PERSISTENT) ? ZEND_HASH_APPLY_STOP : ZEND_HASH_APPLY_REMOVE;
}


static int clean_non_persistent_constant_full(const zend_constant *c TSRMLS_DC)
{
	return (c->flags & CONST_PERSISTENT) ? 0 : 1;
}


static int clean_module_constant(const zend_constant *c, int *module_number TSRMLS_DC)
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
	REGISTER_MAIN_LONG_CONSTANT("E_DEPRECATED", E_DEPRECATED, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_CORE_ERROR", E_CORE_ERROR, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_CORE_WARNING", E_CORE_WARNING, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_COMPILE_ERROR", E_COMPILE_ERROR, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_COMPILE_WARNING", E_COMPILE_WARNING, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_USER_ERROR", E_USER_ERROR, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_USER_WARNING", E_USER_WARNING, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_USER_NOTICE", E_USER_NOTICE, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_USER_DEPRECATED", E_USER_DEPRECATED, CONST_PERSISTENT | CONST_CS);

	REGISTER_MAIN_LONG_CONSTANT("E_ALL", E_ALL, CONST_PERSISTENT | CONST_CS);

	REGISTER_MAIN_LONG_CONSTANT("DEBUG_BACKTRACE_PROVIDE_OBJECT", DEBUG_BACKTRACE_PROVIDE_OBJECT, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("DEBUG_BACKTRACE_IGNORE_ARGS", DEBUG_BACKTRACE_IGNORE_ARGS, CONST_PERSISTENT | CONST_CS);
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

		c.flags = CONST_PERSISTENT | CONST_CS;

		c.name = zend_strndup(ZEND_STRL("ZEND_THREAD_SAFE"));
		c.name_len = sizeof("ZEND_THREAD_SAFE");
		c.value.value.lval = ZTS_V;
		c.value.type = IS_BOOL;
		zend_register_constant(&c TSRMLS_CC);

		c.name = zend_strndup(ZEND_STRL("ZEND_DEBUG_BUILD"));
		c.name_len = sizeof("ZEND_DEBUG_BUILD");
		c.value.value.lval = ZEND_DEBUG;
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


ZEND_API void zend_register_long_constant(const char *name, uint name_len, long lval, int flags, int module_number TSRMLS_DC)
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


ZEND_API void zend_register_double_constant(const char *name, uint name_len, double dval, int flags, int module_number TSRMLS_DC)
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


ZEND_API void zend_register_stringl_constant(const char *name, uint name_len, char *strval, uint strlen, int flags, int module_number TSRMLS_DC)
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


ZEND_API void zend_register_string_constant(const char *name, uint name_len, char *strval, int flags, int module_number TSRMLS_DC)
{
	zend_register_stringl_constant(name, name_len, strval, strlen(strval), flags, module_number TSRMLS_CC);
}

static int zend_get_special_constant(const char *name, uint name_len, zend_constant **c TSRMLS_DC)
{
	int ret;
	static char haltoff[] = "__COMPILER_HALT_OFFSET__";

	if (!EG(in_execution)) {
		return 0;
	} else if (name_len == sizeof("__CLASS__")-1 &&
	          !memcmp(name, "__CLASS__", sizeof("__CLASS__")-1)) {
		zend_constant tmp;

		/* Returned constants may be cached, so they have to be stored */
		if (EG(scope) && EG(scope)->name) {
			int const_name_len;
			char *const_name;
			ALLOCA_FLAG(use_heap)
			
			const_name_len = sizeof("\0__CLASS__") + EG(scope)->name_length;
			const_name = do_alloca(const_name_len, use_heap);
			memcpy(const_name, "\0__CLASS__", sizeof("\0__CLASS__")-1);
			zend_str_tolower_copy(const_name + sizeof("\0__CLASS__")-1, EG(scope)->name, EG(scope)->name_length);
			if (zend_hash_find(EG(zend_constants), const_name, const_name_len, (void**)c) == FAILURE) {
				zend_hash_add(EG(zend_constants), const_name, const_name_len, (void*)&tmp, sizeof(zend_constant), (void**)c);
				memset(*c, 0, sizeof(zend_constant));
				Z_STRVAL((**c).value) = estrndup(EG(scope)->name, EG(scope)->name_length);
				Z_STRLEN((**c).value) = EG(scope)->name_length;
				Z_TYPE((**c).value) = IS_STRING;
			}
			free_alloca(const_name, use_heap);
		} else {
			if (zend_hash_find(EG(zend_constants), "\0__CLASS__", sizeof("\0__CLASS__"), (void**)c) == FAILURE) {
				zend_hash_add(EG(zend_constants), "\0__CLASS__", sizeof("\0__CLASS__"), (void*)&tmp, sizeof(zend_constant), (void**)c);
				memset(*c, 0, sizeof(zend_constant));
				Z_STRVAL((**c).value) = estrndup("", 0);
				Z_STRLEN((**c).value) = 0;
				Z_TYPE((**c).value) = IS_STRING;
			}
		}
		return 1;
	} else if (name_len == sizeof("__COMPILER_HALT_OFFSET__")-1 &&
	          !memcmp(name, "__COMPILER_HALT_OFFSET__", sizeof("__COMPILER_HALT_OFFSET__")-1)) {
		const char *cfilename;
		char *haltname;
		int len, clen;

		cfilename = zend_get_executed_filename(TSRMLS_C);
		clen = strlen(cfilename);
		/* check for __COMPILER_HALT_OFFSET__ */
		zend_mangle_property_name(&haltname, &len, haltoff,
			sizeof("__COMPILER_HALT_OFFSET__") - 1, cfilename, clen, 0);
		ret = zend_hash_find(EG(zend_constants), haltname, len+1, (void **) c);
		efree(haltname);
		return (ret == SUCCESS);
	} else {
		return 0;
	}
}


ZEND_API int zend_get_constant(const char *name, uint name_len, zval *result TSRMLS_DC)
{
	zend_constant *c;
	int retval = 1;
	char *lookup_name;

	if (zend_hash_find(EG(zend_constants), name, name_len+1, (void **) &c) == FAILURE) {
		lookup_name = zend_str_tolower_dup(name, name_len);

		if (zend_hash_find(EG(zend_constants), lookup_name, name_len+1, (void **) &c)==SUCCESS) {
			if (c->flags & CONST_CS) {
				retval=0;
			}
		} else {
			retval = zend_get_special_constant(name, name_len, &c TSRMLS_CC);
		}
		efree(lookup_name);
	}

	if (retval) {
		*result = c->value;
		zval_copy_ctor(result);
		Z_SET_REFCOUNT_P(result, 1);
		Z_UNSET_ISREF_P(result);
	}

	return retval;
}

ZEND_API int zend_get_constant_ex(const char *name, uint name_len, zval *result, zend_class_entry *scope, ulong flags TSRMLS_DC)
{
	zend_constant *c;
	int retval = 1;
	const char *colon;
	zend_class_entry *ce = NULL;
	char *class_name;
	zval **ret_constant;

	/* Skip leading \\ */
	if (name[0] == '\\') {
		name += 1;
		name_len -= 1;
	}


	if ((colon = zend_memrchr(name, ':', name_len)) &&
	    colon > name && (*(colon - 1) == ':')) {
		int class_name_len = colon - name - 1;
		int const_name_len = name_len - class_name_len - 2;
		const char *constant_name = colon + 1;
		char *lcname;

		class_name = estrndup(name, class_name_len);
		lcname = zend_str_tolower_dup(class_name, class_name_len);
		if (!scope) {
			if (EG(in_execution)) {
				scope = EG(scope);
			} else {
				scope = CG(active_class_entry);
			}
		}

		if (class_name_len == sizeof("self")-1 &&
		    !memcmp(lcname, "self", sizeof("self")-1)) {
			if (scope) {
				ce = scope;
			} else {
				zend_error(E_ERROR, "Cannot access self:: when no class scope is active");
				retval = 0;
			}
			efree(lcname);
		} else if (class_name_len == sizeof("parent")-1 &&
		           !memcmp(lcname, "parent", sizeof("parent")-1)) {
			if (!scope) {
				zend_error(E_ERROR, "Cannot access parent:: when no class scope is active");
			} else if (!scope->parent) {
				zend_error(E_ERROR, "Cannot access parent:: when current class scope has no parent");
			} else {
				ce = scope->parent;
			}
			efree(lcname);
		} else if (class_name_len == sizeof("static")-1 &&
		           !memcmp(lcname, "static", sizeof("static")-1)) {
			if (EG(called_scope)) {
				ce = EG(called_scope);
			} else {
				zend_error(E_ERROR, "Cannot access static:: when no class scope is active");
			}
			efree(lcname);
		} else {
			efree(lcname);
			ce = zend_fetch_class(class_name, class_name_len, flags TSRMLS_CC);
		}
		if (retval && ce) {
			if (zend_hash_find(&ce->constants_table, constant_name, const_name_len+1, (void **) &ret_constant) != SUCCESS) {
				retval = 0;
				if ((flags & ZEND_FETCH_CLASS_SILENT) == 0) {
					zend_error(E_ERROR, "Undefined class constant '%s::%s'", class_name, constant_name);
				}
			}
		} else if (!ce) {
			retval = 0;
		}
		efree(class_name);
		goto finish;
	}

	/* non-class constant */
	if ((colon = zend_memrchr(name, '\\', name_len)) != NULL) {
		/* compound constant name */
		int prefix_len = colon - name;
		int const_name_len = name_len - prefix_len - 1;
		const char *constant_name = colon + 1;
		char *lcname;
		int found_const = 0;

		lcname = zend_str_tolower_dup(name, prefix_len);
		/* Check for namespace constant */

		/* Concatenate lowercase namespace name and constant name */
		lcname = erealloc(lcname, prefix_len + 1 + const_name_len + 1);
		lcname[prefix_len] = '\\';
		memcpy(lcname + prefix_len + 1, constant_name, const_name_len + 1);

		if (zend_hash_find(EG(zend_constants), lcname, prefix_len + 1 + const_name_len + 1, (void **) &c) == SUCCESS) {
			found_const = 1;
		} else {
			/* try lowercase */
			zend_str_tolower(lcname + prefix_len + 1, const_name_len);
			if (zend_hash_find(EG(zend_constants), lcname, prefix_len + 1 + const_name_len + 1, (void **) &c) == SUCCESS) {
				if ((c->flags & CONST_CS) == 0) {
					found_const = 1;
				}
			}
		}
		efree(lcname);
		if(found_const) {
			*result = c->value;
			zval_update_constant_ex(&result, (void*)1, NULL TSRMLS_CC);
			zval_copy_ctor(result);
			Z_SET_REFCOUNT_P(result, 1);
			Z_UNSET_ISREF_P(result);
			return 1;
		}
		/* name requires runtime resolution, need to check non-namespaced name */
		if ((flags & IS_CONSTANT_UNQUALIFIED) != 0) {
			name = constant_name;
			name_len = const_name_len;
			return zend_get_constant(name, name_len, result TSRMLS_CC);
		}
		retval = 0;
finish:
		if (retval) {
			zval_update_constant_ex(ret_constant, (void*)1, ce TSRMLS_CC);
			*result = **ret_constant;
			zval_copy_ctor(result);
			INIT_PZVAL(result);
		}

		return retval;
	}

	return zend_get_constant(name, name_len, result TSRMLS_CC);
}

zend_constant *zend_quick_get_constant(const zend_literal *key, ulong flags TSRMLS_DC)
{
	zend_constant *c;

	if (zend_hash_quick_find(EG(zend_constants), Z_STRVAL(key->constant), Z_STRLEN(key->constant) + 1, key->hash_value, (void **) &c) == FAILURE) {
		key++;
		if (zend_hash_quick_find(EG(zend_constants), Z_STRVAL(key->constant), Z_STRLEN(key->constant) + 1, key->hash_value, (void **) &c) == FAILURE ||
		    (c->flags & CONST_CS) != 0) {
			if ((flags & (IS_CONSTANT_IN_NAMESPACE|IS_CONSTANT_UNQUALIFIED)) == (IS_CONSTANT_IN_NAMESPACE|IS_CONSTANT_UNQUALIFIED)) {
				key++;
				if (zend_hash_quick_find(EG(zend_constants), Z_STRVAL(key->constant), Z_STRLEN(key->constant) + 1, key->hash_value, (void **) &c) == FAILURE) {
				    key++;
					if (zend_hash_quick_find(EG(zend_constants), Z_STRVAL(key->constant), Z_STRLEN(key->constant) + 1, key->hash_value, (void **) &c) == FAILURE ||
					    (c->flags & CONST_CS) != 0) {

						key--;
						if (!zend_get_special_constant(Z_STRVAL(key->constant), Z_STRLEN(key->constant), &c TSRMLS_CC)) {
							return NULL;
						}
					}
				}
			} else {
				key--;
				if (!zend_get_special_constant(Z_STRVAL(key->constant), Z_STRLEN(key->constant), &c TSRMLS_CC)) {
					return NULL;
				}
			}
		}
	}
	return c;
}

ZEND_API int zend_register_constant(zend_constant *c TSRMLS_DC)
{
	char *lowercase_name = NULL;
	char *name;
	int ret = SUCCESS;
	ulong chash = 0;

#if 0
	printf("Registering constant for module %d\n", c->module_number);
#endif

	if (!(c->flags & CONST_CS)) {
		/* keep in mind that c->name_len already contains the '\0' */
		lowercase_name = estrndup(c->name, c->name_len-1);
		zend_str_tolower(lowercase_name, c->name_len-1);
		lowercase_name = (char*)zend_new_interned_string(lowercase_name, c->name_len, 1 TSRMLS_CC);
		name = lowercase_name;
		chash = IS_INTERNED(lowercase_name) ? INTERNED_HASH(lowercase_name) : 0;
	} else {
		char *slash = strrchr(c->name, '\\');
		if(slash) {
			lowercase_name = estrndup(c->name, c->name_len-1);
			zend_str_tolower(lowercase_name, slash-c->name);
			lowercase_name = (char*)zend_new_interned_string(lowercase_name, c->name_len, 1 TSRMLS_CC);
			name = lowercase_name;
			
			chash = IS_INTERNED(lowercase_name) ? INTERNED_HASH(lowercase_name) : 0;
		} else {
			name = c->name;
		}
	}
	if (chash == 0) {
		chash = zend_hash_func(name, c->name_len);
	}

	/* Check if the user is trying to define the internal pseudo constant name __COMPILER_HALT_OFFSET__ */
	if ((c->name_len == sizeof("__COMPILER_HALT_OFFSET__")
		&& !memcmp(name, "__COMPILER_HALT_OFFSET__", sizeof("__COMPILER_HALT_OFFSET__")-1))
		|| zend_hash_quick_add(EG(zend_constants), name, c->name_len, chash, (void *) c, sizeof(zend_constant), NULL)==FAILURE) {
		
		/* The internal __COMPILER_HALT_OFFSET__ is prefixed by NULL byte */
		if (c->name[0] == '\0' && c->name_len > sizeof("\0__COMPILER_HALT_OFFSET__")
			&& memcmp(name, "\0__COMPILER_HALT_OFFSET__", sizeof("\0__COMPILER_HALT_OFFSET__")) == 0) {
			name++;
		}
		zend_error(E_NOTICE,"Constant %s already defined", name);
		str_free(c->name);
		if (!(c->flags & CONST_PERSISTENT)) {
			zval_dtor(&c->value);
		}
		ret = FAILURE;
	}
	if (lowercase_name && !IS_INTERNED(lowercase_name)) {
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
