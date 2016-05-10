/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
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
#include "zend_exceptions.h"
#include "zend_execute.h"
#include "zend_variables.h"
#include "zend_operators.h"
#include "zend_globals.h"
#include "zend_API.h"

void free_zend_constant(zval *zv)
{
	zend_constant *c = Z_PTR_P(zv);

	if (!(c->flags & CONST_PERSISTENT)) {
		zval_ptr_dtor(&c->value);
	} else {
		zval_internal_dtor(&c->value);
	}
	if (c->name) {
		zend_string_release(c->name);
	}
	pefree(c, c->flags & CONST_PERSISTENT);
}


static void copy_zend_constant(zval *zv)
{
	zend_constant *c = Z_PTR_P(zv);

	Z_PTR_P(zv) = pemalloc(sizeof(zend_constant), c->flags & CONST_PERSISTENT);
	memcpy(Z_PTR_P(zv), c, sizeof(zend_constant));

	c = Z_PTR_P(zv);
	c->name = zend_string_copy(c->name);
	if (!(c->flags & CONST_PERSISTENT)) {
		zval_copy_ctor(&c->value);
	} else {
		if (Z_TYPE(c->value) == IS_STRING) {
			Z_STR(c->value) = zend_string_dup(Z_STR(c->value), 1);
		}
	}
}


void zend_copy_constants(HashTable *target, HashTable *source)
{
	zend_hash_copy(target, source, copy_zend_constant);
}


static int clean_non_persistent_constant(zval *zv)
{
	zend_constant *c = Z_PTR_P(zv);
	return (c->flags & CONST_PERSISTENT) ? ZEND_HASH_APPLY_STOP : ZEND_HASH_APPLY_REMOVE;
}


static int clean_non_persistent_constant_full(zval *zv)
{
	zend_constant *c = Z_PTR_P(zv);
	return (c->flags & CONST_PERSISTENT) ? 0 : 1;
}


static int clean_module_constant(zval *el, void *arg)
{
	zend_constant *c = (zend_constant *)Z_PTR_P(el);
	int module_number = *(int *)arg;

	if (c->module_number == module_number) {
		return 1;
	} else {
		return 0;
	}
}


void clean_module_constants(int module_number)
{
	zend_hash_apply_with_argument(EG(zend_constants), clean_module_constant, (void *) &module_number);
}


int zend_startup_constants(void)
{
	EG(zend_constants) = (HashTable *) malloc(sizeof(HashTable));

	zend_hash_init(EG(zend_constants), 128, NULL, ZEND_CONSTANT_DTOR, 1);
	return SUCCESS;
}



void zend_register_standard_constants(void)
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


int zend_shutdown_constants(void)
{
	zend_hash_destroy(EG(zend_constants));
	free(EG(zend_constants));
	return SUCCESS;
}


void clean_non_persistent_constants(void)
{
	if (EG(full_tables_cleanup)) {
		zend_hash_apply(EG(zend_constants), clean_non_persistent_constant_full);
	} else {
		zend_hash_reverse_apply(EG(zend_constants), clean_non_persistent_constant);
	}
}

ZEND_API void zend_register_null_constant(const char *name, size_t name_len, int flags, int module_number)
{
	zend_constant c;

	ZVAL_NULL(&c.value);
	c.flags = flags;
	c.name = zend_string_init(name, name_len, flags & CONST_PERSISTENT);
	c.module_number = module_number;
	zend_register_constant(&c);
}

ZEND_API void zend_register_bool_constant(const char *name, size_t name_len, zend_bool bval, int flags, int module_number)
{
	zend_constant c;

	ZVAL_BOOL(&c.value, bval);
	c.flags = flags;
	c.name = zend_string_init(name, name_len, flags & CONST_PERSISTENT);
	c.module_number = module_number;
	zend_register_constant(&c);
}

ZEND_API void zend_register_long_constant(const char *name, size_t name_len, zend_long lval, int flags, int module_number)
{
	zend_constant c;

	ZVAL_LONG(&c.value, lval);
	c.flags = flags;
	c.name = zend_string_init(name, name_len, flags & CONST_PERSISTENT);
	c.module_number = module_number;
	zend_register_constant(&c);
}


ZEND_API void zend_register_double_constant(const char *name, size_t name_len, double dval, int flags, int module_number)
{
	zend_constant c;

	ZVAL_DOUBLE(&c.value, dval);
	c.flags = flags;
	c.name = zend_string_init(name, name_len, flags & CONST_PERSISTENT);
	c.module_number = module_number;
	zend_register_constant(&c);
}


ZEND_API void zend_register_stringl_constant(const char *name, size_t name_len, char *strval, size_t strlen, int flags, int module_number)
{
	zend_constant c;

	ZVAL_NEW_STR(&c.value, zend_string_init(strval, strlen, flags & CONST_PERSISTENT));
	c.flags = flags;
	c.name = zend_string_init(name, name_len, flags & CONST_PERSISTENT);
	c.module_number = module_number;
	zend_register_constant(&c);
}


ZEND_API void zend_register_string_constant(const char *name, size_t name_len, char *strval, int flags, int module_number)
{
	zend_register_stringl_constant(name, name_len, strval, strlen(strval), flags, module_number);
}

static zend_constant *zend_get_special_constant(const char *name, size_t name_len)
{
	zend_constant *c;
	static char haltoff[] = "__COMPILER_HALT_OFFSET__";

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
		zend_string_free(haltname);
		return c;
	} else {
		return NULL;
	}
}

ZEND_API int zend_verify_const_access(zend_class_constant *c, zend_class_entry *scope) /* {{{ */
{
	if (Z_ACCESS_FLAGS(c->value) & ZEND_ACC_PUBLIC) {
		return 1;
	} else if (Z_ACCESS_FLAGS(c->value) & ZEND_ACC_PRIVATE) {
		return (c->ce == scope);
	} else {
		ZEND_ASSERT(Z_ACCESS_FLAGS(c->value) & ZEND_ACC_PROTECTED);
		return zend_check_protected(c->ce, scope);
	}
}
/* }}} */

ZEND_API zval *zend_get_constant_str(const char *name, size_t name_len)
{
	zend_constant *c;
	ALLOCA_FLAG(use_heap)

	if ((c = zend_hash_str_find_ptr(EG(zend_constants), name, name_len)) == NULL) {
		char *lcname = do_alloca(name_len + 1, use_heap);
		zend_str_tolower_copy(lcname, name, name_len);
		if ((c = zend_hash_str_find_ptr(EG(zend_constants), lcname, name_len)) != NULL) {
			if (c->flags & CONST_CS) {
				c = NULL;
			}
		} else {
			c = zend_get_special_constant(name, name_len);
		}
		free_alloca(lcname, use_heap);
	}

	return c ? &c->value : NULL;
}

ZEND_API zval *zend_get_constant(zend_string *name)
{
	zend_constant *c;
	ALLOCA_FLAG(use_heap)

	if ((c = zend_hash_find_ptr(EG(zend_constants), name)) == NULL) {
		char *lcname = do_alloca(ZSTR_LEN(name) + 1, use_heap);
		zend_str_tolower_copy(lcname, ZSTR_VAL(name), ZSTR_LEN(name));
		if ((c = zend_hash_str_find_ptr(EG(zend_constants), lcname, ZSTR_LEN(name))) != NULL) {
			if (c->flags & CONST_CS) {
				c = NULL;
			}
		} else {
			c = zend_get_special_constant(ZSTR_VAL(name), ZSTR_LEN(name));
		}
		free_alloca(lcname, use_heap);
	}

	return c ? &c->value : NULL;
}

ZEND_API zval *zend_get_constant_ex(zend_string *cname, zend_class_entry *scope, uint32_t flags)
{
	zend_constant *c;
	const char *colon;
	zend_class_entry *ce = NULL;
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
		zend_string *class_name = zend_string_init(name, class_name_len, 0);
		zval *ret_constant = NULL;

		if (zend_string_equals_literal_ci(class_name, "self")) {
			if (UNEXPECTED(!scope)) {
				zend_throw_error(NULL, "Cannot access self:: when no class scope is active");
				goto failure;
			}
			ce = scope;
		} else if (zend_string_equals_literal_ci(class_name, "parent")) {
			if (UNEXPECTED(!scope)) {
				zend_throw_error(NULL, "Cannot access parent:: when no class scope is active");
				goto failure;
			} else if (UNEXPECTED(!scope->parent)) {
				zend_throw_error(NULL, "Cannot access parent:: when current class scope has no parent");
				goto failure;
			} else {
				ce = scope->parent;
			}
		} else if (zend_string_equals_literal_ci(class_name, "static")) {
			ce = zend_get_called_scope(EG(current_execute_data));
			if (UNEXPECTED(!ce)) {
				zend_throw_error(NULL, "Cannot access static:: when no class scope is active");
				goto failure;
			}
		} else {
			ce = zend_fetch_class(class_name, flags);
		}
		if (ce) {
			zend_class_constant *c = zend_hash_find_ptr(&ce->constants_table, constant_name);
			if (c == NULL) {
				if ((flags & ZEND_FETCH_CLASS_SILENT) == 0) {
					zend_throw_error(NULL, "Undefined class constant '%s::%s'", ZSTR_VAL(class_name), ZSTR_VAL(constant_name));
					goto failure;
				}
				ret_constant = NULL;
			} else {
				if (!zend_verify_const_access(c, scope)) {
					zend_throw_error(NULL, "Cannot access %s const %s::%s", zend_visibility_string(Z_ACCESS_FLAGS(c->value)), ZSTR_VAL(class_name), ZSTR_VAL(constant_name));
					goto failure;
				}
				ret_constant = &c->value;
			}
		}

		if (ret_constant && Z_CONSTANT_P(ret_constant)) {
			if (Z_TYPE_P(ret_constant) == IS_CONSTANT_AST) {
				if (IS_CONSTANT_VISITED(ret_constant)) {
					zend_throw_error(NULL, "Cannot declare self-referencing constant '%s::%s'", ZSTR_VAL(class_name), ZSTR_VAL(constant_name));
					goto failure;
				}
				MARK_CONSTANT_VISITED(ret_constant);
			}
			if (UNEXPECTED(zval_update_constant_ex(ret_constant, ce) != SUCCESS)) {
				RESET_CONSTANT_VISITED(ret_constant);
				goto failure;
			}
			RESET_CONSTANT_VISITED(ret_constant);
		}
failure:
		zend_string_release(class_name);
		zend_string_free(constant_name);
		return ret_constant;
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

		lcname_len = prefix_len + 1 + const_name_len;
		lcname = do_alloca(lcname_len + 1, use_heap);
		zend_str_tolower_copy(lcname, name, prefix_len);
		/* Check for namespace constant */

		lcname[prefix_len] = '\\';
		memcpy(lcname + prefix_len + 1, constant_name, const_name_len + 1);

		if ((c = zend_hash_str_find_ptr(EG(zend_constants), lcname, lcname_len)) == NULL) {
			/* try lowercase */
			zend_str_tolower(lcname + prefix_len + 1, const_name_len);
			if ((c = zend_hash_str_find_ptr(EG(zend_constants), lcname, lcname_len)) != NULL) {
				if ((c->flags & CONST_CS) != 0) {
					c = NULL;
				}
			}
		}
		free_alloca(lcname, use_heap);
		if (c) {
			return &c->value;
		}
		/* name requires runtime resolution, need to check non-namespaced name */
		if ((flags & IS_CONSTANT_UNQUALIFIED) != 0) {
			return zend_get_constant_str(constant_name, const_name_len);
		}
		return NULL;
	}

	if (cname) {
		return zend_get_constant(cname);
	} else {
		return zend_get_constant_str(name, name_len);
	}
}

zend_constant *zend_quick_get_constant(const zval *key, uint32_t flags)
{
	zend_constant *c;

	if ((c = zend_hash_find_ptr(EG(zend_constants), Z_STR_P(key))) == NULL) {
		key++;
		if ((c = zend_hash_find_ptr(EG(zend_constants), Z_STR_P(key))) == NULL ||
		    (c->flags & CONST_CS) != 0) {
			if ((flags & (IS_CONSTANT_IN_NAMESPACE|IS_CONSTANT_UNQUALIFIED)) == (IS_CONSTANT_IN_NAMESPACE|IS_CONSTANT_UNQUALIFIED)) {
				key++;
				if ((c = zend_hash_find_ptr(EG(zend_constants), Z_STR_P(key))) == NULL) {
				    key++;
					if ((c = zend_hash_find_ptr(EG(zend_constants), Z_STR_P(key))) == NULL ||
					    (c->flags & CONST_CS) != 0) {

						key--;
						c = NULL;
					}
				}
			} else {
				key--;
				c = NULL;
			}
		}
	}
	return c;
}

static void* zend_hash_add_constant(HashTable *ht, zend_string *key, zend_constant *c)
{
	void *ret;
	zend_constant *copy = pemalloc(sizeof(zend_constant), c->flags & CONST_PERSISTENT);

	memcpy(copy, c, sizeof(zend_constant));
	ret = zend_hash_add_ptr(ht, key, copy);
	if (!ret) {
		pefree(copy, c->flags & CONST_PERSISTENT);
	}
	return ret;
}

ZEND_API int zend_register_constant(zend_constant *c)
{
	zend_string *lowercase_name = NULL;
	zend_string *name;
	int ret = SUCCESS;

#if 0
	printf("Registering constant for module %d\n", c->module_number);
#endif

	if (!(c->flags & CONST_CS)) {
		lowercase_name = zend_string_alloc(ZSTR_LEN(c->name), c->flags & CONST_PERSISTENT);
		zend_str_tolower_copy(ZSTR_VAL(lowercase_name), ZSTR_VAL(c->name), ZSTR_LEN(c->name));
		lowercase_name = zend_new_interned_string(lowercase_name);
		name = lowercase_name;
	} else {
		char *slash = strrchr(ZSTR_VAL(c->name), '\\');
		if (slash) {
			lowercase_name = zend_string_init(ZSTR_VAL(c->name), ZSTR_LEN(c->name), c->flags & CONST_PERSISTENT);
			zend_str_tolower(ZSTR_VAL(lowercase_name), slash - ZSTR_VAL(c->name));
			lowercase_name = zend_new_interned_string(lowercase_name);
			name = lowercase_name;
		} else {
			name = c->name;
		}
	}

	/* Check if the user is trying to define the internal pseudo constant name __COMPILER_HALT_OFFSET__ */
	if ((ZSTR_LEN(c->name) == sizeof("__COMPILER_HALT_OFFSET__")-1
		&& !memcmp(ZSTR_VAL(name), "__COMPILER_HALT_OFFSET__", sizeof("__COMPILER_HALT_OFFSET__")-1))
		|| zend_hash_add_constant(EG(zend_constants), name, c) == NULL) {

		/* The internal __COMPILER_HALT_OFFSET__ is prefixed by NULL byte */
		if (ZSTR_VAL(c->name)[0] == '\0' && ZSTR_LEN(c->name) > sizeof("\0__COMPILER_HALT_OFFSET__")-1
			&& memcmp(ZSTR_VAL(name), "\0__COMPILER_HALT_OFFSET__", sizeof("\0__COMPILER_HALT_OFFSET__")) == 0) {
		}
		zend_error(E_NOTICE,"Constant %s already defined", ZSTR_VAL(name));
		zend_string_release(c->name);
		if (!(c->flags & CONST_PERSISTENT)) {
			zval_dtor(&c->value);
		}
		ret = FAILURE;
	}
	if (lowercase_name) {
		zend_string_release(lowercase_name);
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
