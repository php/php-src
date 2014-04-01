/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
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
#include "zend_API.h"

void free_zend_constant(zval *zv)
{
	zend_constant *c = Z_PTR_P(zv);

	if (!(c->flags & CONST_PERSISTENT)) {
		zval_dtor(&c->value);
	} else {
		zval_internal_dtor(&c->value);
	}
	if (c->name) {
		STR_RELEASE(c->name);
	}
	free(c);
}


static void copy_zend_constant(zval *zv)
{
	zend_constant *c = Z_PTR_P(zv);

	Z_PTR_P(zv) = malloc(sizeof(zend_constant)/*, c->flags & CONST_PERSISTENT*/);
	memcpy(Z_PTR_P(zv), c, sizeof(zend_constant));
//???	c->name = STR_DUP(c->name, c->flags & CONST_PERSISTENT);
	c->name = STR_COPY(c->name);
//???	if (!(c->flags & CONST_PERSISTENT)) {
		zval_copy_ctor(&c->value);
//???	}
}


void zend_copy_constants(HashTable *target, HashTable *source)
{
	zend_hash_copy(target, source, copy_zend_constant);
}


static int clean_non_persistent_constant(zval *zv TSRMLS_DC)
{
	zend_constant *c = Z_PTR_P(zv);
	return (c->flags & CONST_PERSISTENT) ? ZEND_HASH_APPLY_STOP : ZEND_HASH_APPLY_REMOVE;
}


static int clean_non_persistent_constant_full(zval *zv TSRMLS_DC)
{
	zend_constant *c = Z_PTR_P(zv);
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
		REGISTER_MAIN_BOOL_CONSTANT("TRUE", 1, CONST_PERSISTENT | CONST_CT_SUBST);
		REGISTER_MAIN_BOOL_CONSTANT("FALSE", 0, CONST_PERSISTENT | CONST_CT_SUBST);
		REGISTER_MAIN_BOOL_CONSTANT("ZEND_THREAD_SAFE", ZTS_V, CONST_PERSISTENT | CONST_CS);
		REGISTER_MAIN_BOOL_CONSTANT("ZEND_DEBUG_BUILD", ZEND_DEBUG, CONST_PERSISTENT | CONST_CS);
	}
	REGISTER_MAIN_NULL_CONSTANT("NULL", CONST_PERSISTENT | CONST_CT_SUBST);
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
		zend_hash_apply(EG(zend_constants), clean_non_persistent_constant_full TSRMLS_CC);
	} else {
		zend_hash_reverse_apply(EG(zend_constants), clean_non_persistent_constant TSRMLS_CC);
	}
}

ZEND_API void zend_register_null_constant(const char *name, uint name_len, int flags, int module_number TSRMLS_DC)
{
	zend_constant c;
	
	ZVAL_NULL(&c.value);
	c.flags = flags;
	c.name = STR_INIT(name, name_len, flags & CONST_PERSISTENT);
	c.module_number = module_number;
	zend_register_constant(&c TSRMLS_CC);
}

ZEND_API void zend_register_bool_constant(const char *name, uint name_len, zend_bool bval, int flags, int module_number TSRMLS_DC)
{
	zend_constant c;
	
	ZVAL_BOOL(&c.value, bval);
	c.flags = flags;
	c.name = STR_INIT(name, name_len, flags & CONST_PERSISTENT);
	c.module_number = module_number;
	zend_register_constant(&c TSRMLS_CC);
}

ZEND_API void zend_register_long_constant(const char *name, uint name_len, long lval, int flags, int module_number TSRMLS_DC)
{
	zend_constant c;
	
	ZVAL_LONG(&c.value, lval);
	c.flags = flags;
	c.name = STR_INIT(name, name_len, flags & CONST_PERSISTENT);
	c.module_number = module_number;
	zend_register_constant(&c TSRMLS_CC);
}


ZEND_API void zend_register_double_constant(const char *name, uint name_len, double dval, int flags, int module_number TSRMLS_DC)
{
	zend_constant c;
	
	ZVAL_DOUBLE(&c.value, dval);
	c.flags = flags;
	c.name = STR_INIT(name, name_len, flags & CONST_PERSISTENT);
	c.module_number = module_number;
	zend_register_constant(&c TSRMLS_CC);
}


ZEND_API void zend_register_stringl_constant(const char *name, uint name_len, char *strval, uint strlen, int flags, int module_number TSRMLS_DC)
{
	zend_constant c;
	
//???	ZVAL_STRINGL(&c.value, strval, strlen, 0);
	ZVAL_STR(&c.value, STR_INIT(strval, strlen, flags & CONST_PERSISTENT));
	c.flags = flags;
	c.name = STR_INIT(name, name_len, flags & CONST_PERSISTENT);
	c.module_number = module_number;
	zend_register_constant(&c TSRMLS_CC);
}


ZEND_API void zend_register_string_constant(const char *name, uint name_len, char *strval, int flags, int module_number TSRMLS_DC)
{
	zend_register_stringl_constant(name, name_len, strval, strlen(strval), flags, module_number TSRMLS_CC);
}

static zend_constant *zend_get_special_constant(const char *name, uint name_len TSRMLS_DC)
{
	zend_constant *c;
	static char haltoff[] = "__COMPILER_HALT_OFFSET__";

	if (!EG(in_execution)) {
		return NULL;
	} else if (name_len == sizeof("__CLASS__")-1 &&
	          !memcmp(name, "__CLASS__", sizeof("__CLASS__")-1)) {

		/* Returned constants may be cached, so they have to be stored */
		if (EG(scope) && EG(scope)->name) {
			int const_name_len;
			zend_string *const_name;
			
			const_name_len = sizeof("\0__CLASS__") + EG(scope)->name->len;
			const_name = STR_ALLOC(const_name_len, 0);
			memcpy(const_name->val, "\0__CLASS__", sizeof("\0__CLASS__")-1);
			zend_str_tolower_copy(const_name->val + sizeof("\0__CLASS__")-1, EG(scope)->name->val, EG(scope)->name->len);
			if ((c = zend_hash_find_ptr(EG(zend_constants), const_name)) == NULL) {
				c = emalloc(sizeof(zend_constant));
				memset(c, 0, sizeof(zend_constant));
				ZVAL_STR(&c->value, STR_COPY(EG(scope)->name));
				zend_hash_add_ptr(EG(zend_constants), const_name, c);
			}
			STR_RELEASE(const_name);
		} else {
			zend_string *const_name = STR_INIT("\0__CLASS__", sizeof("\0__CLASS__")-1, 0);
			if ((c = zend_hash_find_ptr(EG(zend_constants), const_name)) == NULL) {
				c = emalloc(sizeof(zend_constant));
				memset(c, 0, sizeof(zend_constant));
				ZVAL_EMPTY_STRING(&c->value);
				zend_hash_add_ptr(EG(zend_constants), const_name, c);
			}
			STR_RELEASE(const_name);
		}
		return c;
	} else if (name_len == sizeof("__COMPILER_HALT_OFFSET__")-1 &&
	          !memcmp(name, "__COMPILER_HALT_OFFSET__", sizeof("__COMPILER_HALT_OFFSET__")-1)) {
		const char *cfilename;
		zend_string *haltname;
		int clen;

		cfilename = zend_get_executed_filename(TSRMLS_C);
		clen = strlen(cfilename);
		/* check for __COMPILER_HALT_OFFSET__ */
		haltname = zend_mangle_property_name(haltoff,
			sizeof("__COMPILER_HALT_OFFSET__") - 1, cfilename, clen, 0);
		c = zend_hash_find_ptr(EG(zend_constants), haltname);
		STR_FREE(haltname);
		return c;
	} else {
		return NULL;
	}
}


ZEND_API int zend_get_constant(const char *name, uint name_len, zval *result TSRMLS_DC)
{
	zend_constant *c;

	if ((c = zend_hash_str_find_ptr(EG(zend_constants), name, name_len)) == NULL) {
		char *lcname = zend_str_tolower_dup(name, name_len);
		if ((c = zend_hash_str_find_ptr(EG(zend_constants), lcname, name_len)) != NULL) {
			if (c->flags & CONST_CS) {
				c = NULL;
			}
		} else {
			c = zend_get_special_constant(name, name_len TSRMLS_CC);
		}
		efree(lcname);
	}

	if (c) {
		ZVAL_DUP(result, &c->value);
		return 1;
	}

	return 0;
}

ZEND_API int zend_get_constant_ex(const char *name, uint name_len, zval *result, zend_class_entry *scope, ulong flags TSRMLS_DC)
{
	zend_constant *c;
	int retval = 1;
	const char *colon;
	zend_class_entry *ce = NULL;
	zend_string *class_name;
	zval *ret_constant;

	/* Skip leading \\ */
	if (name[0] == '\\') {
		name += 1;
		name_len -= 1;
	}


	if ((colon = zend_memrchr(name, ':', name_len)) &&
	    colon > name && (*(colon - 1) == ':')) {
		int class_name_len = colon - name - 1;
		int const_name_len = name_len - class_name_len - 2;
		zend_string *constant_name = STR_INIT(colon + 1, const_name_len, 0);
		zend_string *lcname;

		class_name = STR_INIT(name, class_name_len, 0);
		lcname = STR_ALLOC(class_name_len, 0);
		zend_str_tolower_copy(lcname->val, name, class_name_len);
		if (!scope) {
			if (EG(in_execution)) {
				scope = EG(scope);
			} else {
				scope = CG(active_class_entry);
			}
		}

		if (class_name_len == sizeof("self")-1 &&
		    !memcmp(lcname->val, "self", sizeof("self")-1)) {
			if (scope) {
				ce = scope;
			} else {
				zend_error(E_ERROR, "Cannot access self:: when no class scope is active");
				retval = 0;
			}
			STR_FREE(lcname);
		} else if (class_name_len == sizeof("parent")-1 &&
		           !memcmp(lcname->val, "parent", sizeof("parent")-1)) {
			if (!scope) {
				zend_error(E_ERROR, "Cannot access parent:: when no class scope is active");
			} else if (!scope->parent) {
				zend_error(E_ERROR, "Cannot access parent:: when current class scope has no parent");
			} else {
				ce = scope->parent;
			}
			STR_FREE(lcname);
		} else if (class_name_len == sizeof("static")-1 &&
		           !memcmp(lcname->val, "static", sizeof("static")-1)) {
			if (EG(called_scope)) {
				ce = EG(called_scope);
			} else {
				zend_error(E_ERROR, "Cannot access static:: when no class scope is active");
			}
			STR_FREE(lcname);
		} else {
			STR_FREE(lcname);
			ce = zend_fetch_class(class_name, flags TSRMLS_CC);
		}
		if (retval && ce) {
			if ((ret_constant = zend_hash_find(&ce->constants_table, constant_name)) == NULL) {
				retval = 0;
				if ((flags & ZEND_FETCH_CLASS_SILENT) == 0) {
					zend_error(E_ERROR, "Undefined class constant '%s::%s'", class_name->val, constant_name->val);
				}
			}
		} else if (!ce) {
			retval = 0;
		}
		STR_FREE(class_name);
		STR_FREE(constant_name);
		goto finish;
	}

	/* non-class constant */
	if ((colon = zend_memrchr(name, '\\', name_len)) != NULL) {
		/* compound constant name */
		int prefix_len = colon - name;
		int const_name_len = name_len - prefix_len - 1;
		const char *constant_name = colon + 1;
		zend_string *lcname;
		int found_const = 0;

		lcname = STR_ALLOC(prefix_len + 1 + const_name_len, 0);
		zend_str_tolower_copy(lcname->val, name, prefix_len);
		/* Check for namespace constant */

		lcname->val[prefix_len] = '\\';
		memcpy(lcname->val + prefix_len + 1, constant_name, const_name_len + 1);

		if ((c = zend_hash_find_ptr(EG(zend_constants), lcname)) != NULL) {
			found_const = 1;
		} else {
			/* try lowercase */
			zend_str_tolower(lcname->val + prefix_len + 1, const_name_len);
			STR_FORGET_HASH_VAL(lcname);
			if ((c = zend_hash_find_ptr(EG(zend_constants), lcname)) != NULL) {
				if ((c->flags & CONST_CS) == 0) {
					found_const = 1;
				}
			}
		}
		STR_FREE(lcname);
		if (found_const) {
			ZVAL_COPY_VALUE(result, &c->value);
			zval_update_constant_ex(result, (void*)1, NULL TSRMLS_CC);
			zval_copy_ctor(result);
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
			ZVAL_DUP(result, ret_constant);
		}

		return retval;
	}

	return zend_get_constant(name, name_len, result TSRMLS_CC);
}

zend_constant *zend_quick_get_constant(const zend_literal *key, ulong flags TSRMLS_DC)
{
	zend_constant *c;

	if ((c = zend_hash_find_ptr(EG(zend_constants), Z_STR(key->constant))) == NULL) {
		key++;
		if ((c = zend_hash_find_ptr(EG(zend_constants), Z_STR(key->constant))) == NULL ||
		    (c->flags & CONST_CS) != 0) {
			if ((flags & (IS_CONSTANT_IN_NAMESPACE|IS_CONSTANT_UNQUALIFIED)) == (IS_CONSTANT_IN_NAMESPACE|IS_CONSTANT_UNQUALIFIED)) {
				key++;
				if ((c = zend_hash_find_ptr(EG(zend_constants), Z_STR(key->constant))) == NULL) {
				    key++;
					if ((c = zend_hash_find_ptr(EG(zend_constants), Z_STR(key->constant))) == NULL ||
					    (c->flags & CONST_CS) != 0) {

						key--;
						c = zend_get_special_constant(Z_STRVAL(key->constant), Z_STRLEN(key->constant) TSRMLS_CC);
					}
				}
			} else {
				key--;
				c = zend_get_special_constant(Z_STRVAL(key->constant), Z_STRLEN(key->constant) TSRMLS_CC);
			}
		}
	}
	return c;
}

ZEND_API int zend_register_constant(zend_constant *c TSRMLS_DC)
{
	zend_string *lowercase_name = NULL;
	zend_string *name;
	int ret = SUCCESS;

#if 0
	printf("Registering constant for module %d\n", c->module_number);
#endif

	if (!(c->flags & CONST_CS)) {
//???		/* keep in mind that c->name_len already contains the '\0' */
		lowercase_name = STR_ALLOC(c->name->len, c->flags & CONST_PERSISTENT);
		zend_str_tolower_copy(lowercase_name->val, c->name->val, c->name->len);
		lowercase_name = zend_new_interned_string(lowercase_name TSRMLS_CC);
		name = lowercase_name;
	} else {
		char *slash = strrchr(c->name->val, '\\');
		if (slash) {
			lowercase_name = STR_INIT(c->name->val, c->name->len, c->flags & CONST_PERSISTENT);
			zend_str_tolower(lowercase_name->val, slash - c->name->val);
			lowercase_name = zend_new_interned_string(lowercase_name TSRMLS_CC);
			name = lowercase_name;
		} else {
			name = c->name;
		}
	}

	/* Check if the user is trying to define the internal pseudo constant name __COMPILER_HALT_OFFSET__ */
	if ((c->name->len == sizeof("__COMPILER_HALT_OFFSET__")-1
		&& !memcmp(name->val, "__COMPILER_HALT_OFFSET__", sizeof("__COMPILER_HALT_OFFSET__")-1))
		|| zend_hash_add_mem(EG(zend_constants), name, c, sizeof(zend_constant)) == NULL) {
		
		/* The internal __COMPILER_HALT_OFFSET__ is prefixed by NULL byte */
		if (c->name->val[0] == '\0' && c->name->len > sizeof("\0__COMPILER_HALT_OFFSET__")-1
			&& memcmp(name->val, "\0__COMPILER_HALT_OFFSET__", sizeof("\0__COMPILER_HALT_OFFSET__")) == 0) {
//???			name++;
		}
		zend_error(E_NOTICE,"Constant %s already defined", name->val);
		STR_RELEASE(c->name);
		if (!(c->flags & CONST_PERSISTENT)) {
			zval_dtor(&c->value);
		}
		ret = FAILURE;
	}
	if (lowercase_name) {
		STR_RELEASE(lowercase_name);
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
