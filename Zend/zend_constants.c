/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2008 Zend Technologies Ltd. (http://www.zend.com) |
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

void free_zend_constant(zend_constant *c) /* {{{ */
{
	if (!(c->flags & CONST_PERSISTENT)) {
		zval_dtor(&c->value);
	}
	free(c->name.v);
}
/* }}} */

void copy_zend_constant(zend_constant *c) /* {{{ */
{
	TSRMLS_FETCH();
	if (UG(unicode)) {
		c->name.u = zend_ustrndup(c->name.u, c->name_len - 1);
	} else {
		c->name.s = zend_strndup(c->name.s, c->name_len - 1);
	}
	if (!(c->flags & CONST_PERSISTENT)) {
		zval_copy_ctor(&c->value);
	}
}
/* }}} */

void zend_copy_constants(HashTable *target, HashTable *source) /* {{{ */
{
	zend_constant tmp_constant;

	zend_hash_copy(target, source, (copy_ctor_func_t) copy_zend_constant, &tmp_constant, sizeof(zend_constant));
}
/* }}} */

static int clean_non_persistent_constant(zend_constant *c TSRMLS_DC) /* {{{ */
{
	return (c->flags & CONST_PERSISTENT) ? ZEND_HASH_APPLY_STOP : ZEND_HASH_APPLY_REMOVE;
}
/* }}} */

static int clean_non_persistent_constant_full(zend_constant *c TSRMLS_DC) /* {{{ */
{
	return (c->flags & CONST_PERSISTENT) ? 0 : 1;
}
/* }}} */

static int clean_module_constant(zend_constant *c, int *module_number TSRMLS_DC) /* {{{ */
{
	if (c->module_number == *module_number) {
		return 1;
	} else {
		return 0;
	}
}
/* }}} */

void clean_module_constants(int module_number TSRMLS_DC) /* {{{ */
{
	zend_hash_apply_with_argument(EG(zend_constants), (apply_func_arg_t) clean_module_constant, (void *) &module_number TSRMLS_CC);
}
/* }}} */

int zend_startup_constants(TSRMLS_D) /* {{{ */
{
	EG(zend_constants) = (HashTable *) malloc(sizeof(HashTable));

	if (zend_u_hash_init(EG(zend_constants), 20, NULL, ZEND_CONSTANT_DTOR, 1, UG(unicode))==FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

void zend_register_standard_constants(TSRMLS_D) /* {{{ */
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

	REGISTER_MAIN_LONG_CONSTANT("U_CONV_ERROR_STOP", ZEND_CONV_ERROR_STOP, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("U_CONV_ERROR_SKIP", ZEND_CONV_ERROR_SKIP, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("U_CONV_ERROR_SUBST", ZEND_CONV_ERROR_SUBST, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("U_CONV_ERROR_ESCAPE_UNICODE", ZEND_CONV_ERROR_ESCAPE_UNICODE, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("U_CONV_ERROR_ESCAPE_ICU", ZEND_CONV_ERROR_ESCAPE_ICU, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("U_CONV_ERROR_ESCAPE_JAVA", ZEND_CONV_ERROR_ESCAPE_JAVA, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("U_CONV_ERROR_ESCAPE_XML_DEC", ZEND_CONV_ERROR_ESCAPE_XML_DEC, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("U_CONV_ERROR_ESCAPE_XML_HEX", ZEND_CONV_ERROR_ESCAPE_XML_HEX, CONST_PERSISTENT | CONST_CS);

	REGISTER_MAIN_LONG_CONSTANT("FROM_UNICODE", ZEND_FROM_UNICODE, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("TO_UNICODE", ZEND_TO_UNICODE, CONST_PERSISTENT | CONST_CS);

	/* true/false constants */
	{
		zend_constant c;

		Z_TYPE(c.value) = IS_BOOL;
		c.flags = CONST_PERSISTENT | CONST_CT_SUBST;
		c.module_number = 0;

		c.name.s = zend_strndup(ZEND_STRL("TRUE"));
		c.name_len = sizeof("TRUE");
		Z_LVAL(c.value) = 1;
		Z_TYPE(c.value) = IS_BOOL;
		zend_register_constant(&c TSRMLS_CC);

		c.name.s = zend_strndup(ZEND_STRL("FALSE"));
		c.name_len = sizeof("FALSE");
		Z_LVAL(c.value) = 0;
		Z_TYPE(c.value) = IS_BOOL;
		zend_register_constant(&c TSRMLS_CC);

		c.name.s = zend_strndup(ZEND_STRL("NULL"));
		c.name_len = sizeof("NULL");
		Z_TYPE(c.value) = IS_NULL;
		zend_register_constant(&c TSRMLS_CC);

		c.flags = CONST_PERSISTENT;

		c.name.s = zend_strndup(ZEND_STRL("ZEND_THREAD_SAFE"));
		c.name_len = sizeof("ZEND_THREAD_SAFE");
		Z_LVAL(c.value) = ZTS_V;
		Z_TYPE(c.value) = IS_BOOL;
		zend_register_constant(&c TSRMLS_CC);

		c.name.s = zend_strndup(ZEND_STRL("ZEND_DEBUG_BUILD"));
		c.name_len = sizeof("ZEND_DEBUG_BUILD");
		Z_LVAL(c.value) = ZEND_DEBUG;
		Z_TYPE(c.value) = IS_BOOL;
		zend_register_constant(&c TSRMLS_CC);
	}
}
/* }}} */

int zend_shutdown_constants(TSRMLS_D) /* {{{ */
{
	zend_hash_destroy(EG(zend_constants));
	free(EG(zend_constants));
	return SUCCESS;
}
/* }}} */

void clean_non_persistent_constants(TSRMLS_D) /* {{{ */
{
	if (EG(full_tables_cleanup)) {
		zend_hash_apply(EG(zend_constants), (apply_func_t) clean_non_persistent_constant_full TSRMLS_CC);
	} else {
		zend_hash_reverse_apply(EG(zend_constants), (apply_func_t) clean_non_persistent_constant TSRMLS_CC);
	}
}
/* }}} */

ZEND_API void zend_register_long_constant(char *name, uint name_len, long lval, int flags, int module_number TSRMLS_DC) /* {{{ */
{
	zend_constant c;

	if (UG(unicode)) {
		c.name.u = malloc(UBYTES(name_len));
		u_charsToUChars(name, c.name.u, name_len);
		c.name_len = name_len;
	} else {
		c.name.s = zend_strndup(name, name_len-1);
		c.name_len = name_len;
	}
	Z_TYPE(c.value) = IS_LONG;
	Z_LVAL(c.value) = lval;
	c.flags = flags;
	c.module_number = module_number;
	zend_u_register_constant(ZEND_STR_TYPE, &c TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_register_double_constant(char *name, uint name_len, double dval, int flags, int module_number TSRMLS_DC) /* {{{ */
{
	zend_constant c;

	if (UG(unicode)) {
		c.name.u = malloc(UBYTES(name_len));
		u_charsToUChars(name, c.name.u, name_len);
		c.name_len = name_len;
	} else {
		c.name.s = zend_strndup(name, name_len-1);
		c.name_len = name_len;
	}
	Z_TYPE(c.value) = IS_DOUBLE;
	Z_DVAL(c.value) = dval;
	c.flags = flags;
	c.module_number = module_number;
	zend_u_register_constant(ZEND_STR_TYPE, &c TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_register_stringl_constant(char *name, uint name_len, char *strval, uint strlen, int flags, int module_number TSRMLS_DC) /* {{{ */
{
	zend_constant c;

	if (UG(unicode)) {
		c.name.u = malloc(UBYTES(name_len));
		u_charsToUChars(name, c.name.u, name_len);
		c.name_len = name_len;
		Z_TYPE(c.value) = IS_UNICODE;
		if (flags & CONST_PERSISTENT) {
			Z_USTRVAL(c.value) = malloc(UBYTES(strlen+1));
		} else {
			Z_USTRVAL(c.value) = emalloc(UBYTES(strlen+1));
		}
		u_charsToUChars(strval, Z_USTRVAL(c.value), strlen+1);
		Z_USTRLEN(c.value) = strlen;
		if (!(flags & CONST_PERSISTENT)) {
			efree(strval);
		}
	} else {
		c.name.s = zend_strndup(name, name_len-1);
		c.name_len = name_len;
		Z_TYPE(c.value) = IS_STRING;
		Z_STRVAL(c.value) = strval;
		Z_STRLEN(c.value) = strlen;
	}
	c.flags = flags;
	c.module_number = module_number;
	zend_u_register_constant(ZEND_STR_TYPE, &c TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_register_string_constant(char *name, uint name_len, char *strval, int flags, int module_number TSRMLS_DC) /* {{{ */
{
	zend_register_stringl_constant(name, name_len, strval, strlen(strval), flags, module_number TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_u_get_constant(zend_uchar type, zstr name, uint name_len, zval *result TSRMLS_DC) /* {{{ */
{
	zend_constant *c;
	int retval = 1;
	zstr lookup_name;

	if (zend_u_hash_find(EG(zend_constants), type, name, name_len+1, (void **) &c) == FAILURE) {
		unsigned int lookup_name_len;

		lookup_name = zend_u_str_case_fold(type, name, name_len, 1, &lookup_name_len);

		if (zend_u_hash_find(EG(zend_constants), type, lookup_name, lookup_name_len+1, (void **) &c)==SUCCESS) {
			if ((c->flags & CONST_CS) && memcmp(c->name.v, name.v, UG(unicode)?UBYTES(name_len):name_len)!=0) {
				retval=0;
			}
		} else {
			if (!EG(in_execution)) {
				retval = 0;
			} else if (name_len == sizeof(ZEND_HALT_CONSTANT_NAME)-1 && ZEND_U_EQUAL(type, name, name_len, ZEND_HALT_CONSTANT_NAME, sizeof(ZEND_HALT_CONSTANT_NAME)-1)) {
				char *cfilename;
				zstr haltname;
				int len, clen;
				cfilename = zend_get_executed_filename(TSRMLS_C);
				clen = strlen(cfilename);
				/* check for __COMPILER_HALT_OFFSET__ */
				zend_mangle_property_name(&haltname.s, &len, ZEND_HALT_CONSTANT_NAME,
					sizeof(ZEND_HALT_CONSTANT_NAME) - 1, cfilename, clen, 0);
				if (zend_u_hash_find(EG(zend_constants), IS_STRING, haltname, len+1, (void **) &c) == SUCCESS) {
					retval=1;
				} else {
					retval=0;
				}
				pefree(haltname.s, 0);
			} else {
				retval=0;
			}
		}
		efree(lookup_name.v);
	}

	if (retval) {
		*result = c->value;
		zval_copy_ctor(result);
		Z_SET_REFCOUNT_P(result, 1);
		Z_UNSET_ISREF_P(result);
	}

	return retval;
}
/* }}} */

ZEND_API int zend_u_get_constant_ex(zend_uchar type, zstr name, uint name_len, zval *result, zend_class_entry *scope, ulong flags TSRMLS_DC) /* {{{ */
{
	zend_constant *c;
	int retval = 1;
	zstr colon;

	/* Skip leading :: */
	if (type == IS_UNICODE &&
	    name.u[0] == ':' &&
	    name.u[1] == ':') {
		name.u += 2;
		name_len -= 2;
		flags &= ZEND_FETCH_CLASS_SILENT;
	} else if (type == IS_STRING &&
	           name.s[0] == ':' &&
	           name.s[1] == ':') {
		name.s += 2;
		name_len -= 2;
		flags &= ZEND_FETCH_CLASS_SILENT;
	}

	if ((type == IS_UNICODE && (colon.u = u_memrchr(name.u, ':', name_len)) && colon.u > name.u && *(colon.u-1) == ':') ||
	    (!type == IS_STRING && (colon.s = zend_memrchr(name.s, ':', name_len))&& colon.s > name.s && *(colon.s-1) == ':')) {
		/* compound constant name */
		zend_class_entry *ce = NULL;
		int class_name_len = (type == IS_UNICODE) ? colon.u-name.u-1 : colon.s-name.s-1;
		int const_name_len = name_len - class_name_len - 2;
		zstr constant_name, class_name;
		zval **ret_constant;
		zstr lcname;
		unsigned int lcname_len;

		if (type == IS_UNICODE) {
			constant_name.u = colon.u + 1;
		} else {
			constant_name.s = colon.s + 1;
		}

		if (!scope) {
			if (EG(in_execution)) {
				scope = EG(scope);
			} else {
				scope = CG(active_class_entry);
			}
		}

		if (type == IS_UNICODE) {
			class_name.u = eustrndup(name.u, class_name_len);
		} else {
			class_name.s = estrndup(name.s, class_name_len);
		}

		lcname = zend_u_str_case_fold(type, class_name, class_name_len, 0, &lcname_len);

		if (lcname_len == sizeof("self")-1 &&
		    ZEND_U_EQUAL(type, lcname, lcname_len, "self", sizeof("self")-1)) {
			if (scope) {
				ce = scope;
			} else {
				zend_error(E_ERROR, "Cannot access self:: when no class scope is active");
				retval = 0;
			}
			efree(lcname.v);
		} else if (lcname_len == sizeof("parent")-1 &&
		           ZEND_U_EQUAL(type, lcname, lcname_len, "parent", sizeof("parent")-1)) {
			if (!scope) {
				zend_error(E_ERROR, "Cannot access parent:: when no class scope is active");
			} else if (!scope->parent) {
				zend_error(E_ERROR, "Cannot access parent:: when current class scope has no parent");
			} else {
				ce = scope->parent;
			}
			efree(lcname.v);
		} else if (lcname_len == sizeof("static")-1 &&
		           ZEND_U_EQUAL(type, lcname, lcname_len, "static", sizeof("static")-1)) {
			if (EG(called_scope)) {
				ce = EG(called_scope);
			} else {
				zend_error(E_ERROR, "Cannot access static:: when no class scope is active");
			}
			efree(lcname.v);
		} else {
			/* Check for namespace constant */
			zstr nsname;
			unsigned int nsname_len;

			/* Concatenate lowercase namespace name and constant name */
			if (type == IS_UNICODE) {
				lcname.u = erealloc(lcname.u, UBYTES(lcname_len + 2 + const_name_len + 1));
				lcname.u[lcname_len] = ':';
				lcname.u[lcname_len+1] = ':';
				memcpy(lcname.u+lcname_len+2, constant_name.u, UBYTES(const_name_len + 1));
			} else {
				lcname.s = erealloc(lcname.s, lcname_len + 2 + const_name_len + 1);
				lcname.s[lcname_len] = ':';
				lcname.s[lcname_len+1] = ':';
				memcpy(lcname.s+lcname_len+2, constant_name.s, const_name_len + 1);
			}
			lcname_len += 2 + const_name_len;

			nsname = lcname;
			nsname_len = lcname_len;
			if (flags & ZEND_FETCH_CLASS_RT_NS_NAME) {
				/* Remove namespace name */
				if (type == IS_UNICODE) {
					nsname.u = u_memchr(nsname.u, ':', nsname_len) + 2;
					nsname_len -= (nsname.u - lcname.u);
				} else {
					nsname.s = (char *)memchr(nsname.s, ':', nsname_len) + 2;
					nsname_len -= (nsname.s - lcname.s);
			    }
			}

			if (zend_u_hash_find(EG(zend_constants), type, nsname, nsname_len+1, (void **) &c) == SUCCESS) {
				efree(lcname.v);
				efree(class_name.v);
				*result = c->value;
				zval_update_constant_ex(&result, (void*)1, NULL TSRMLS_CC);
				zval_copy_ctor(result);
				Z_SET_REFCOUNT_P(result, 1);
				Z_UNSET_ISREF_P(result);
				return 1;
			}
			efree(lcname.v);

			if ((flags & IS_CONSTANT_RT_NS_CHECK) == 0) {
				/* Check for class */
				ce = zend_u_fetch_class(type, class_name, class_name_len, flags TSRMLS_CC);
			}
		}

		if (retval && ce) {
			if (zend_u_hash_find(&ce->constants_table, type, constant_name, const_name_len+1, (void **) &ret_constant) != SUCCESS) {
				retval = 0;
			}
		} else {
			if ((flags & ZEND_FETCH_CLASS_RT_NS_NAME) == 0) {
				if ((flags & IS_CONSTANT_RT_NS_CHECK) != 0) {
					name = constant_name;
					name_len = const_name_len;
					efree(class_name.v);
					retval = 1;
					return zend_u_get_constant(type, name, name_len, result TSRMLS_CC);
				}
				if ((flags & ZEND_FETCH_CLASS_SILENT) == 0) {
					zend_error(E_ERROR, "Class '%R' not found", type, class_name);
				}
			}
			retval = 0;
		}
		efree(class_name.v);

		if (retval) {
			zval_update_constant_ex(ret_constant, (void*)1, ce TSRMLS_CC);
			*result = **ret_constant;
			zval_copy_ctor(result);
			INIT_PZVAL(result);
		}

		return retval;
	}

	return zend_u_get_constant(type, name, name_len, result TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_get_constant(char *name, uint name_len, zval *result TSRMLS_DC) /* {{{ */
{
	return zend_u_get_constant_ex(IS_STRING, ZSTR(name), name_len, result, NULL, 0 TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_u_register_constant(zend_uchar type, zend_constant *c TSRMLS_DC) /* {{{ */
{
	unsigned int  lookup_name_len;
	zstr lookup_name;
	zstr name;
	int ret = SUCCESS;

#if 0
	printf("Registering constant for module %d\n", c->module_number);
#endif

	if (!(c->flags & CONST_CS)) {
		/* keep in mind that c->name_len already contains the '\0' */
		name = lookup_name = zend_u_str_case_fold(type, c->name, c->name_len-1, 1, &lookup_name_len);
		lookup_name_len++;
	} else {
		lookup_name_len = c->name_len;
		name = c->name;
		lookup_name = NULL_ZSTR;
	}

	if (EG(in_execution) && lookup_name_len == sizeof(ZEND_HALT_CONSTANT_NAME) && ZEND_U_EQUAL(type, name, lookup_name_len-1, ZEND_HALT_CONSTANT_NAME, sizeof(ZEND_HALT_CONSTANT_NAME)-1)) {
		zend_error(E_NOTICE,"Constant %R already defined", type, name);
		if (lookup_name.v) {
			efree(lookup_name.v);
		}
		ret = FAILURE;
	}
	if (zend_u_hash_add(EG(zend_constants), type, name, lookup_name_len, (void *) c, sizeof(zend_constant), NULL)==FAILURE) {
		zend_error(E_NOTICE,"Constant %R already defined", type, name);
		free(c->name.v);
		if (!(c->flags & CONST_PERSISTENT)) {
			zval_dtor(&c->value);
		}
		ret = FAILURE;
	}
	if (lookup_name.v) {
		efree(lookup_name.v);
	}
	return ret;
}
/* }}} */

ZEND_API int zend_register_constant(zend_constant *c TSRMLS_DC) /* {{{ */
{
	if (UG(unicode)) {
		UChar *ustr;

		if (c->name.s) {
			ustr = malloc(UBYTES(c->name_len));
			u_charsToUChars(c->name.s, ustr, c->name_len);
			free(c->name.s);
			c->name.u = ustr;
		}
		if (Z_TYPE(c->value) == IS_STRING || (Z_TYPE(c->value) & IS_CONSTANT_TYPE_MASK) == IS_CONSTANT) {
			ustr = pemalloc(UBYTES(Z_STRLEN(c->value)+1), c->flags & CONST_PERSISTENT);
			u_charsToUChars(Z_STRVAL(c->value), ustr, Z_STRLEN(c->value)+1);
			pefree(Z_STRVAL(c->value), c->flags & CONST_PERSISTENT);
			Z_USTRVAL(c->value) = ustr;
			if (Z_TYPE(c->value) == IS_STRING) Z_TYPE(c->value) = IS_UNICODE;
		}
		return zend_u_register_constant(IS_UNICODE, c TSRMLS_CC);
	} else {
		return zend_u_register_constant(IS_STRING, c TSRMLS_CC);
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
