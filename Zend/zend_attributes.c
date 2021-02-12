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
   | Authors: Benjamin Eberlei <kontakt@beberlei.de>                      |
   |          Martin Schröder <m.schroeder2007@gmail.com>                 |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_attributes.h"
#include "zend_attributes_arginfo.h"
#include "zend_smart_str.h"

ZEND_API zend_class_entry *zend_ce_attribute;

static HashTable internal_attributes;

void validate_attribute(zend_attribute *attr, uint32_t target, zend_class_entry *scope)
{
	// TODO: More proper signature validation: Too many args, incorrect arg names.
	if (attr->argc > 0) {
		zval flags;

		/* As this is run in the middle of compilation, fetch the attribute value without
		 * specifying a scope. The class is not fully linked yet, and we may seen an
		 * inconsistent state. */
		if (FAILURE == zend_get_attribute_value(&flags, attr, 0, NULL)) {
			return;
		}

		if (Z_TYPE(flags) != IS_LONG) {
			zend_error_noreturn(E_ERROR,
				"Attribute::__construct(): Argument #1 ($flags) must must be of type int, %s given",
				zend_zval_type_name(&flags)
			);
		}

		if (Z_LVAL(flags) & ~ZEND_ATTRIBUTE_FLAGS) {
			zend_error_noreturn(E_ERROR, "Invalid attribute flags specified");
		}

		zval_ptr_dtor(&flags);
	}
}

ZEND_METHOD(Attribute, __construct)
{
	zend_long flags = ZEND_ATTRIBUTE_TARGET_ALL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_LONG(OBJ_PROP_NUM(Z_OBJ_P(ZEND_THIS), 0), flags);
}

static zend_attribute *get_attribute(HashTable *attributes, zend_string *lcname, uint32_t offset)
{
	if (attributes) {
		zend_attribute *attr;

		ZEND_HASH_FOREACH_PTR(attributes, attr) {
			if (attr->offset == offset && zend_string_equals(attr->lcname, lcname)) {
				return attr;
			}
		} ZEND_HASH_FOREACH_END();
	}

	return NULL;
}

static zend_attribute *get_attribute_str(HashTable *attributes, const char *str, size_t len, uint32_t offset)
{
	if (attributes) {
		zend_attribute *attr;

		ZEND_HASH_FOREACH_PTR(attributes, attr) {
			if (attr->offset == offset && ZSTR_LEN(attr->lcname) == len) {
				if (0 == memcmp(ZSTR_VAL(attr->lcname), str, len)) {
					return attr;
				}
			}
		} ZEND_HASH_FOREACH_END();
	}

	return NULL;
}

ZEND_API zend_attribute *zend_get_attribute(HashTable *attributes, zend_string *lcname)
{
	return get_attribute(attributes, lcname, 0);
}

ZEND_API zend_attribute *zend_get_attribute_str(HashTable *attributes, const char *str, size_t len)
{
	return get_attribute_str(attributes, str, len, 0);
}

ZEND_API zend_attribute *zend_get_parameter_attribute(HashTable *attributes, zend_string *lcname, uint32_t offset)
{
	return get_attribute(attributes, lcname, offset + 1);
}

ZEND_API zend_attribute *zend_get_parameter_attribute_str(HashTable *attributes, const char *str, size_t len, uint32_t offset)
{
	return get_attribute_str(attributes, str, len, offset + 1);
}

ZEND_API zend_result zend_get_attribute_value(zval *ret, zend_attribute *attr, uint32_t i, zend_class_entry *scope)
{
	if (i >= attr->argc) {
		return FAILURE;
	}

	ZVAL_COPY_OR_DUP(ret, &attr->args[i].value);

	if (Z_TYPE_P(ret) == IS_CONSTANT_AST) {
		if (SUCCESS != zval_update_constant_ex(ret, scope)) {
			zval_ptr_dtor(ret);
			return FAILURE;
		}
	}

	return SUCCESS;
}

static const char *target_names[] = {
	"class",
	"function",
	"method",
	"property",
	"class constant",
	"parameter"
};

ZEND_API zend_string *zend_get_attribute_target_names(uint32_t flags)
{
	smart_str str = { 0 };

	for (uint32_t i = 0; i < (sizeof(target_names) / sizeof(char *)); i++) {
		if (flags & (1 << i)) {
			if (smart_str_get_len(&str)) {
				smart_str_appends(&str, ", ");
			}

			smart_str_appends(&str, target_names[i]);
		}
	}

	return smart_str_extract(&str);
}

ZEND_API bool zend_is_attribute_repeated(HashTable *attributes, zend_attribute *attr)
{
	zend_attribute *other;

	ZEND_HASH_FOREACH_PTR(attributes, other) {
		if (other != attr && other->offset == attr->offset) {
			if (zend_string_equals(other->lcname, attr->lcname)) {
				return 1;
			}
		}
	} ZEND_HASH_FOREACH_END();

	return 0;
}

static void attr_free(zval *v)
{
	zend_attribute *attr = Z_PTR_P(v);

	zend_string_release(attr->name);
	zend_string_release(attr->lcname);

	for (uint32_t i = 0; i < attr->argc; i++) {
		if (attr->args[i].name) {
			zend_string_release(attr->args[i].name);
		}
		zval_ptr_dtor(&attr->args[i].value);
	}

	pefree(attr, attr->flags & ZEND_ATTRIBUTE_PERSISTENT);
}

ZEND_API zend_attribute *zend_add_attribute(HashTable **attributes, zend_string *name, uint32_t argc, uint32_t flags, uint32_t offset, uint32_t lineno)
{
	bool persistent = flags & ZEND_ATTRIBUTE_PERSISTENT;
	if (*attributes == NULL) {
		*attributes = pemalloc(sizeof(HashTable), persistent);
		zend_hash_init(*attributes, 8, NULL, attr_free, persistent);
	}

	zend_attribute *attr = pemalloc(ZEND_ATTRIBUTE_SIZE(argc), persistent);

	if (persistent == ((GC_FLAGS(name) & IS_STR_PERSISTENT) != 0)) {
		attr->name = zend_string_copy(name);
	} else {
		attr->name = zend_string_dup(name, persistent);
	}

	attr->lcname = zend_string_tolower_ex(attr->name, persistent);
	attr->flags = flags;
	attr->lineno = lineno;
	attr->offset = offset;
	attr->argc = argc;

	/* Initialize arguments to avoid partial initialization in case of fatal errors. */
	for (uint32_t i = 0; i < argc; i++) {
		attr->args[i].name = NULL;
		ZVAL_UNDEF(&attr->args[i].value);
	}

	zend_hash_next_index_insert_ptr(*attributes, attr);

	return attr;
}

static void free_internal_attribute(zval *v)
{
	pefree(Z_PTR_P(v), 1);
}

ZEND_API zend_internal_attribute *zend_internal_attribute_register(zend_class_entry *ce, uint32_t flags)
{
	zend_internal_attribute *attr;

	if (ce->type != ZEND_INTERNAL_CLASS) {
		zend_error_noreturn(E_ERROR, "Only internal classes can be registered as compiler attribute");
	}

	attr = pemalloc(sizeof(zend_internal_attribute), 1);
	attr->ce = ce;
	attr->flags = flags;
	attr->validator = NULL;

	zend_string *lcname = zend_string_tolower_ex(ce->name, 1);

	zend_hash_update_ptr(&internal_attributes, lcname, attr);
	zend_add_class_attribute(ce, zend_ce_attribute->name, 0);
	zend_string_release(lcname);

	return attr;
}

ZEND_API zend_internal_attribute *zend_internal_attribute_get(zend_string *lcname)
{
	return zend_hash_find_ptr(&internal_attributes, lcname);
}

void zend_register_attribute_ce(void)
{
	zend_internal_attribute *attr;

	zend_hash_init(&internal_attributes, 8, NULL, free_internal_attribute, 1);

	zend_ce_attribute = register_class_Attribute();
	attr = zend_internal_attribute_register(zend_ce_attribute, ZEND_ATTRIBUTE_TARGET_CLASS);
	attr->validator = validate_attribute;

	zend_declare_class_constant_long(zend_ce_attribute, ZEND_STRL("TARGET_CLASS"), ZEND_ATTRIBUTE_TARGET_CLASS);
	zend_declare_class_constant_long(zend_ce_attribute, ZEND_STRL("TARGET_FUNCTION"), ZEND_ATTRIBUTE_TARGET_FUNCTION);
	zend_declare_class_constant_long(zend_ce_attribute, ZEND_STRL("TARGET_METHOD"), ZEND_ATTRIBUTE_TARGET_METHOD);
	zend_declare_class_constant_long(zend_ce_attribute, ZEND_STRL("TARGET_PROPERTY"), ZEND_ATTRIBUTE_TARGET_PROPERTY);
	zend_declare_class_constant_long(zend_ce_attribute, ZEND_STRL("TARGET_CLASS_CONSTANT"), ZEND_ATTRIBUTE_TARGET_CLASS_CONST);
	zend_declare_class_constant_long(zend_ce_attribute, ZEND_STRL("TARGET_PARAMETER"), ZEND_ATTRIBUTE_TARGET_PARAMETER);
	zend_declare_class_constant_long(zend_ce_attribute, ZEND_STRL("TARGET_ALL"), ZEND_ATTRIBUTE_TARGET_ALL);
	zend_declare_class_constant_long(zend_ce_attribute, ZEND_STRL("IS_REPEATABLE"), ZEND_ATTRIBUTE_IS_REPEATABLE);
}

void zend_attributes_shutdown(void)
{
	zend_hash_destroy(&internal_attributes);
}
