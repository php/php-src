/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_constants.h"
#include "zend_exceptions.h"
#include "zend_execute.h"
#include "zend_variables.h"
#include "zend_operators.h"
#include "zend_globals.h"
#include "zend_API.h"
#include "zend_constants_arginfo.h"

/* Protection from recursive self-referencing class constants */
#define IS_CONSTANT_VISITED_MARK    0x80

#define IS_CONSTANT_VISITED(zv)     (Z_CONSTANT_FLAGS_P(zv) & IS_CONSTANT_VISITED_MARK)
#define MARK_CONSTANT_VISITED(zv)   Z_CONSTANT_FLAGS_P(zv) |= IS_CONSTANT_VISITED_MARK
#define RESET_CONSTANT_VISITED(zv)  Z_CONSTANT_FLAGS_P(zv) &= ~IS_CONSTANT_VISITED_MARK

/* Use for special null/true/false constants. */
static zend_constant *null_const, *true_const, *false_const;

void free_zend_constant(zval *zv)
{
	zend_constant *c = Z_PTR_P(zv);

	if (!(ZEND_CONSTANT_FLAGS(c) & CONST_PERSISTENT)) {
		zval_ptr_dtor_nogc(&c->value);
		if (c->name) {
			zend_string_release_ex(c->name, 0);
		}
		efree(c);
	} else {
		zval_internal_ptr_dtor(&c->value);
		if (c->name) {
			zend_string_release_ex(c->name, 1);
		}
		free(c);
	}
}


#ifdef ZTS
static void copy_zend_constant(zval *zv)
{
	zend_constant *c = Z_PTR_P(zv);

	ZEND_ASSERT(ZEND_CONSTANT_FLAGS(c) & CONST_PERSISTENT);
	Z_PTR_P(zv) = pemalloc(sizeof(zend_constant), 1);
	memcpy(Z_PTR_P(zv), c, sizeof(zend_constant));

	c = Z_PTR_P(zv);
	c->name = zend_string_copy(c->name);
	if (Z_TYPE(c->value) == IS_STRING) {
		Z_STR(c->value) = zend_string_dup(Z_STR(c->value), 1);
	}
}


void zend_copy_constants(HashTable *target, HashTable *source)
{
	zend_hash_copy(target, source, copy_zend_constant);
}
#endif


static int clean_module_constant(zval *el, void *arg)
{
	zend_constant *c = (zend_constant *)Z_PTR_P(el);
	int module_number = *(int *)arg;

	if (ZEND_CONSTANT_MODULE_NUMBER(c) == module_number) {
		return ZEND_HASH_APPLY_REMOVE;
	} else {
		return ZEND_HASH_APPLY_KEEP;
	}
}


void clean_module_constants(int module_number)
{
	zend_hash_apply_with_argument(EG(zend_constants), clean_module_constant, (void *) &module_number);
}

void zend_startup_constants(void)
{
	EG(zend_constants) = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init(EG(zend_constants), 128, NULL, ZEND_CONSTANT_DTOR, 1);
}



void zend_register_standard_constants(void)
{
	register_zend_constants_symbols(0);

	true_const = zend_hash_str_find_ptr(EG(zend_constants), "TRUE", sizeof("TRUE")-1);
	false_const = zend_hash_str_find_ptr(EG(zend_constants), "FALSE", sizeof("FALSE")-1);
	null_const = zend_hash_str_find_ptr(EG(zend_constants), "NULL", sizeof("NULL")-1);
}


void zend_shutdown_constants(void)
{
	zend_hash_destroy(EG(zend_constants));
	free(EG(zend_constants));
}

ZEND_API void zend_register_null_constant(const char *name, size_t name_len, int flags, int module_number)
{
	zend_constant c;

	ZVAL_NULL(&c.value);
	ZEND_CONSTANT_SET_FLAGS(&c, flags, module_number);
	c.name = zend_string_init_interned(name, name_len, flags & CONST_PERSISTENT);
	zend_register_constant(&c);
}

ZEND_API void zend_register_bool_constant(const char *name, size_t name_len, bool bval, int flags, int module_number)
{
	zend_constant c;

	ZVAL_BOOL(&c.value, bval);
	ZEND_CONSTANT_SET_FLAGS(&c, flags, module_number);
	c.name = zend_string_init_interned(name, name_len, flags & CONST_PERSISTENT);
	zend_register_constant(&c);
}

ZEND_API void zend_register_long_constant(const char *name, size_t name_len, zend_long lval, int flags, int module_number)
{
	zend_constant c;

	ZVAL_LONG(&c.value, lval);
	ZEND_CONSTANT_SET_FLAGS(&c, flags, module_number);
	c.name = zend_string_init_interned(name, name_len, flags & CONST_PERSISTENT);
	zend_register_constant(&c);
}


ZEND_API void zend_register_double_constant(const char *name, size_t name_len, double dval, int flags, int module_number)
{
	zend_constant c;

	ZVAL_DOUBLE(&c.value, dval);
	ZEND_CONSTANT_SET_FLAGS(&c, flags, module_number);
	c.name = zend_string_init_interned(name, name_len, flags & CONST_PERSISTENT);
	zend_register_constant(&c);
}


ZEND_API void zend_register_stringl_constant(const char *name, size_t name_len, const char *strval, size_t strlen, int flags, int module_number)
{
	zend_constant c;

	ZVAL_STR(&c.value, zend_string_init_interned(strval, strlen, flags & CONST_PERSISTENT));
	ZEND_CONSTANT_SET_FLAGS(&c, flags, module_number);
	c.name = zend_string_init_interned(name, name_len, flags & CONST_PERSISTENT);
	zend_register_constant(&c);
}


ZEND_API void zend_register_string_constant(const char *name, size_t name_len, const char *strval, int flags, int module_number)
{
	zend_register_stringl_constant(name, name_len, strval, strlen(strval), flags, module_number);
}

static zend_constant *zend_get_halt_offset_constant(const char *name, size_t name_len)
{
	zend_constant *c;
	static const char haltoff[] = "__COMPILER_HALT_OFFSET__";

	if (!EG(current_execute_data)) {
		return NULL;
	} else if (name_len == sizeof("__COMPILER_HALT_OFFSET__")-1 &&
	          !memcmp(name, "__COMPILER_HALT_OFFSET__", sizeof("__COMPILER_HALT_OFFSET__")-1)) {
		const char *cfilename;
		zend_string *haltname;
		size_t clen;

		cfilename = zend_get_executed_filename();
		clen = strlen(cfilename);
		/* check for __COMPILER_HALT_OFFSET__ */
		haltname = zend_mangle_property_name(haltoff,
			sizeof("__COMPILER_HALT_OFFSET__") - 1, cfilename, clen, 0);
		c = zend_hash_find_ptr(EG(zend_constants), haltname);
		zend_string_efree(haltname);
		return c;
	} else {
		return NULL;
	}
}

ZEND_API zend_constant *_zend_get_special_const(const char *name, size_t len) /* {{{ */
{
	if (len == 4) {
		if ((name[0] == 'n' || name[0] == 'N') &&
			(name[1] == 'u' || name[1] == 'U') &&
			(name[2] == 'l' || name[2] == 'L') &&
			(name[3] == 'l' || name[3] == 'L')
		) {
			return null_const;
		}
		if ((name[0] == 't' || name[0] == 'T') &&
			(name[1] == 'r' || name[1] == 'R') &&
			(name[2] == 'u' || name[2] == 'U') &&
			(name[3] == 'e' || name[3] == 'E')
		) {
			return true_const;
		}
	} else {
		if ((name[0] == 'f' || name[0] == 'F') &&
			(name[1] == 'a' || name[1] == 'A') &&
			(name[2] == 'l' || name[2] == 'L') &&
			(name[3] == 's' || name[3] == 'S') &&
			(name[4] == 'e' || name[4] == 'E')
		) {
			return false_const;
		}
	}
	return NULL;
}
/* }}} */

ZEND_API bool zend_verify_const_access(zend_class_constant *c, zend_class_entry *scope) /* {{{ */
{
	if (ZEND_CLASS_CONST_FLAGS(c) & ZEND_ACC_PUBLIC) {
		return 1;
	} else if (ZEND_CLASS_CONST_FLAGS(c) & ZEND_ACC_PRIVATE) {
		return (c->ce == scope);
	} else {
		ZEND_ASSERT(ZEND_CLASS_CONST_FLAGS(c) & ZEND_ACC_PROTECTED);
		return zend_check_protected(c->ce, scope);
	}
}
/* }}} */

static zend_constant *zend_get_constant_str_impl(const char *name, size_t name_len)
{
	zend_constant *c = zend_hash_str_find_ptr(EG(zend_constants), name, name_len);
	if (c) {
		return c;
	}

	c = zend_get_halt_offset_constant(name, name_len);
	if (c) {
		return c;
	}

	return zend_get_special_const(name, name_len);
}

ZEND_API zval *zend_get_constant_str(const char *name, size_t name_len)
{
	zend_constant *c = zend_get_constant_str_impl(name, name_len);
	if (c) {
		return &c->value;
	}
	return NULL;
}

static zend_constant *zend_get_constant_impl(zend_string *name)
{
	zend_constant *c = zend_hash_find_ptr(EG(zend_constants), name);
	if (c) {
		return c;
	}

	c = zend_get_halt_offset_constant(ZSTR_VAL(name), ZSTR_LEN(name));
	if (c) {
		return c;
	}

	return zend_get_special_const(ZSTR_VAL(name), ZSTR_LEN(name));
}

ZEND_API zval *zend_get_constant(zend_string *name)
{
	zend_constant *c = zend_get_constant_impl(name);
	if (c) {
		return &c->value;
	}
	return NULL;
}

ZEND_API zval *zend_get_class_constant_ex(zend_string *class_name, zend_string *constant_name, zend_class_entry *scope, uint32_t flags)
{
	zend_class_entry *ce = NULL;
	zend_class_constant *c = NULL;
	zval *ret_constant = NULL;

	if (ZSTR_HAS_CE_CACHE(class_name)) {
		ce = ZSTR_GET_CE_CACHE(class_name);
		if (!ce) {
			ce = zend_fetch_class(class_name, flags);
		}
	} else if (zend_string_equals_literal_ci(class_name, "self")) {
		if (UNEXPECTED(!scope)) {
			zend_throw_error(NULL, "Cannot access \"self\" when no class scope is active");
			goto failure;
		}
		ce = scope;
	} else if (zend_string_equals_literal_ci(class_name, "parent")) {
		if (UNEXPECTED(!scope)) {
			zend_throw_error(NULL, "Cannot access \"parent\" when no class scope is active");
			goto failure;
		} else if (UNEXPECTED(!scope->parent)) {
			zend_throw_error(NULL, "Cannot access \"parent\" when current class scope has no parent");
			goto failure;
		} else {
			ce = scope->parent;
		}
	} else if (zend_string_equals_ci(class_name, ZSTR_KNOWN(ZEND_STR_STATIC))) {
		ce = zend_get_called_scope(EG(current_execute_data));
		if (UNEXPECTED(!ce)) {
			zend_throw_error(NULL, "Cannot access \"static\" when no class scope is active");
			goto failure;
		}
	} else {
		ce = zend_fetch_class(class_name, flags);
	}
	if (ce) {
		c = zend_hash_find_ptr(CE_CONSTANTS_TABLE(ce), constant_name);
		if (c == NULL) {
			if ((flags & ZEND_FETCH_CLASS_SILENT) == 0) {
				zend_throw_error(NULL, "Undefined constant %s::%s", ZSTR_VAL(class_name), ZSTR_VAL(constant_name));
				goto failure;
			}
			ret_constant = NULL;
		} else {
			if (!zend_verify_const_access(c, scope)) {
				if ((flags & ZEND_FETCH_CLASS_SILENT) == 0) {
					zend_throw_error(NULL, "Cannot access %s constant %s::%s", zend_visibility_string(ZEND_CLASS_CONST_FLAGS(c)), ZSTR_VAL(class_name), ZSTR_VAL(constant_name));
				}
				goto failure;
			}

			if (UNEXPECTED(ce->ce_flags & ZEND_ACC_TRAIT)) {
				/** Prevent accessing trait constants directly on cases like \defined() or \constant(), etc. */
				if ((flags & ZEND_FETCH_CLASS_SILENT) == 0) {
					zend_throw_error(NULL, "Cannot access trait constant %s::%s directly", ZSTR_VAL(class_name), ZSTR_VAL(constant_name));
				}
				goto failure;
			}

			if (UNEXPECTED(ZEND_CLASS_CONST_FLAGS(c) & ZEND_ACC_DEPRECATED)) {
				if ((flags & ZEND_FETCH_CLASS_SILENT) == 0) {
					zend_error(E_DEPRECATED, "Constant %s::%s is deprecated", ZSTR_VAL(class_name), ZSTR_VAL(constant_name));
					if (EG(exception)) {
						goto failure;
					}
				}
			}
			ret_constant = &c->value;
		}
	}

	if (ret_constant && Z_TYPE_P(ret_constant) == IS_CONSTANT_AST) {
		zend_result ret;

		if (IS_CONSTANT_VISITED(ret_constant)) {
			zend_throw_error(NULL, "Cannot declare self-referencing constant %s::%s", ZSTR_VAL(class_name), ZSTR_VAL(constant_name));
			ret_constant = NULL;
			goto failure;
		}

		MARK_CONSTANT_VISITED(ret_constant);
		ret = zend_update_class_constant(c, constant_name, c->ce);
		RESET_CONSTANT_VISITED(ret_constant);

		if (UNEXPECTED(ret != SUCCESS)) {
			ret_constant = NULL;
			goto failure;
		}
	}
failure:
	return ret_constant;
}

ZEND_API zval *zend_get_constant_ex(zend_string *cname, zend_class_entry *scope, uint32_t flags)
{
	zend_constant *c;
	const char *colon;
	const char *name = ZSTR_VAL(cname);
	size_t name_len = ZSTR_LEN(cname);

	/* Skip leading \\ */
	if (name[0] == '\\') {
		name += 1;
		name_len -= 1;
		cname = NULL;
	}

	if ((colon = zend_memrchr(name, ':', name_len)) &&
	    colon > name && (*(colon - 1) == ':')) {
		int class_name_len = colon - name - 1;
		size_t const_name_len = name_len - class_name_len - 2;
		zend_string *constant_name = zend_string_init(colon + 1, const_name_len, 0);
		zend_string *class_name = zend_string_init_interned(name, class_name_len, 0);
		zval *ret_constant = zend_get_class_constant_ex(class_name, constant_name, scope, flags);

		zend_string_release_ex(class_name, 0);
		zend_string_efree(constant_name);
		return ret_constant;
/*
		zend_class_entry *ce = NULL;
		zend_class_constant *c = NULL;
		zval *ret_constant = NULL;

		if (zend_string_equals_literal_ci(class_name, "self")) {
			if (UNEXPECTED(!scope)) {
				zend_throw_error(NULL, "Cannot access \"self\" when no class scope is active");
				goto failure;
			}
			ce = scope;
		} else if (zend_string_equals_literal_ci(class_name, "parent")) {
			if (UNEXPECTED(!scope)) {
				zend_throw_error(NULL, "Cannot access \"parent\" when no class scope is active");
				goto failure;
			} else if (UNEXPECTED(!scope->parent)) {
				zend_throw_error(NULL, "Cannot access \"parent\" when current class scope has no parent");
				goto failure;
			} else {
				ce = scope->parent;
			}
		} else if (zend_string_equals_ci(class_name, ZSTR_KNOWN(ZEND_STR_STATIC))) {
			ce = zend_get_called_scope(EG(current_execute_data));
			if (UNEXPECTED(!ce)) {
				zend_throw_error(NULL, "Cannot access \"static\" when no class scope is active");
				goto failure;
			}
		} else {
			ce = zend_fetch_class(class_name, flags);
		}
		if (ce) {
			c = zend_hash_find_ptr(CE_CONSTANTS_TABLE(ce), constant_name);
			if (c == NULL) {
				if ((flags & ZEND_FETCH_CLASS_SILENT) == 0) {
					zend_throw_error(NULL, "Undefined constant %s::%s", ZSTR_VAL(class_name), ZSTR_VAL(constant_name));
					goto failure;
				}
				ret_constant = NULL;
			} else {
				if (!zend_verify_const_access(c, scope)) {
					if ((flags & ZEND_FETCH_CLASS_SILENT) == 0) {
						zend_throw_error(NULL, "Cannot access %s constant %s::%s", zend_visibility_string(ZEND_CLASS_CONST_FLAGS(c)), ZSTR_VAL(class_name), ZSTR_VAL(constant_name));
					}
					goto failure;
				}
				ret_constant = &c->value;
			}
		}

		if (ret_constant && Z_TYPE_P(ret_constant) == IS_CONSTANT_AST) {
			zend_result ret;

			if (IS_CONSTANT_VISITED(ret_constant)) {
				zend_throw_error(NULL, "Cannot declare self-referencing constant %s::%s", ZSTR_VAL(class_name), ZSTR_VAL(constant_name));
				ret_constant = NULL;
				goto failure;
			}

			MARK_CONSTANT_VISITED(ret_constant);
			ret = zval_update_constant_ex(ret_constant, c->ce);
			RESET_CONSTANT_VISITED(ret_constant);

			if (UNEXPECTED(ret != SUCCESS)) {
				ret_constant = NULL;
				goto failure;
			}
		}
failure:
		zend_string_release_ex(class_name, 0);
		zend_string_efree(constant_name);
		return ret_constant;
*/
	}

	/* non-class constant */
	if ((colon = zend_memrchr(name, '\\', name_len)) != NULL) {
		/* compound constant name */
		int prefix_len = colon - name;
		size_t const_name_len = name_len - prefix_len - 1;
		const char *constant_name = colon + 1;
		char *lcname;
		size_t lcname_len;
		ALLOCA_FLAG(use_heap)

		/* Lowercase the namespace portion */
		lcname_len = prefix_len + 1 + const_name_len;
		lcname = do_alloca(lcname_len + 1, use_heap);
		zend_str_tolower_copy(lcname, name, prefix_len);

		lcname[prefix_len] = '\\';
		memcpy(lcname + prefix_len + 1, constant_name, const_name_len + 1);

		c = zend_hash_str_find_ptr(EG(zend_constants), lcname, lcname_len);
		free_alloca(lcname, use_heap);

		if (!c) {
			if (flags & IS_CONSTANT_UNQUALIFIED_IN_NAMESPACE) {
				/* name requires runtime resolution, need to check non-namespaced name */
				c = zend_get_constant_str_impl(constant_name, const_name_len);
			}
		}
	} else {
		if (cname) {
			c = zend_get_constant_impl(cname);
		} else {
			c = zend_get_constant_str_impl(name, name_len);
		}
	}

	if (!c) {
		if (!(flags & ZEND_FETCH_CLASS_SILENT)) {
			zend_throw_error(NULL, "Undefined constant \"%s\"", name);
		}
		return NULL;
	}

	if (!(flags & ZEND_FETCH_CLASS_SILENT) && (ZEND_CONSTANT_FLAGS(c) & CONST_DEPRECATED)) {
		zend_error(E_DEPRECATED, "Constant %s is deprecated", name);
	}
	return &c->value;
}

static void* zend_hash_add_constant(HashTable *ht, zend_string *key, zend_constant *c)
{
	void *ret;
	zend_constant *copy = pemalloc(sizeof(zend_constant), ZEND_CONSTANT_FLAGS(c) & CONST_PERSISTENT);

	memcpy(copy, c, sizeof(zend_constant));
	ret = zend_hash_add_ptr(ht, key, copy);
	if (!ret) {
		pefree(copy, ZEND_CONSTANT_FLAGS(c) & CONST_PERSISTENT);
	}
	return ret;
}

ZEND_API zend_result zend_register_constant(zend_constant *c)
{
	zend_string *lowercase_name = NULL;
	zend_string *name;
	zend_result ret = SUCCESS;
	bool persistent = (ZEND_CONSTANT_FLAGS(c) & CONST_PERSISTENT) != 0;

#if 0
	printf("Registering constant for module %d\n", c->module_number);
#endif

	const char *slash = strrchr(ZSTR_VAL(c->name), '\\');
	if (slash) {
		lowercase_name = zend_string_init(ZSTR_VAL(c->name), ZSTR_LEN(c->name), persistent);
		zend_str_tolower(ZSTR_VAL(lowercase_name), slash - ZSTR_VAL(c->name));
		lowercase_name = zend_new_interned_string(lowercase_name);
		name = lowercase_name;
	} else {
		name = c->name;
	}

	/* Check if the user is trying to define any special constant */
	if (zend_string_equals_literal(name, "__COMPILER_HALT_OFFSET__")
		|| (!persistent && zend_get_special_const(ZSTR_VAL(name), ZSTR_LEN(name)))
		|| zend_hash_add_constant(EG(zend_constants), name, c) == NULL
	) {
		zend_error(E_WARNING, "Constant %s already defined", ZSTR_VAL(name));
		zend_string_release(c->name);
		if (!persistent) {
			zval_ptr_dtor_nogc(&c->value);
		}
		ret = FAILURE;
	}
	if (lowercase_name) {
		zend_string_release(lowercase_name);
	}
	return ret;
}
