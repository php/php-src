/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "ext/standard/php_var.h"
#include "zend_exceptions.h"
#include "zend_smart_str.h"
#include "serialization.h"
#include "serialization_arginfo.h"

static zend_class_entry *zend_test_sleep_object_ce;
static zend_class_entry *zend_test_sleep_object_with_custom_create_ce;
static zend_class_entry *zend_test_legacy_serialize_object_ce;
static zend_object_handlers zend_test_custom_create_object_handlers;

static zend_object *zend_test_custom_create_object_new(zend_class_entry *ce)
{
	zend_object *obj = zend_objects_new(ce);

	object_properties_init(obj, ce);
	obj->handlers = &zend_test_custom_create_object_handlers;

	return obj;
}

static void zend_test_initialized_property_names(zend_object *obj, zval *return_value)
{
	HashTable *props = zend_std_get_properties(obj);
	zend_string *key;
	const char *class_name, *prop_name;
	size_t prop_name_len;
	zval *value;

	array_init(return_value);

	ZEND_HASH_FOREACH_STR_KEY_VAL(props, key, value) {
		if (Z_TYPE_P(value) == IS_INDIRECT) {
			value = Z_INDIRECT_P(value);
		}

		if (key == NULL || Z_TYPE_P(value) == IS_UNDEF) {
			continue;
		}

		zend_unmangle_property_name_ex(key, &class_name, &prop_name, &prop_name_len);
		add_next_index_stringl(return_value, prop_name, prop_name_len);
	} ZEND_HASH_FOREACH_END();
}

static int zend_test_legacy_serialize(
		zval *object,
		unsigned char **buffer,
		size_t *buf_len,
		zend_serialize_data *data)
{
	php_serialize_data_t var_hash = (php_serialize_data_t) data;
	smart_str buf = {0};
	zval rv, *value;

	value = zend_read_property(zend_test_legacy_serialize_object_ce, Z_OBJ_P(object), ZEND_STRL("data"), true, &rv);

	php_var_serialize(&buf, value, &var_hash);

	if (EG(exception) || buf.s == NULL) {
		smart_str_free(&buf);

		return FAILURE;
	}

	*buffer = (unsigned char *) estrndup(ZSTR_VAL(buf.s), ZSTR_LEN(buf.s));
	*buf_len = ZSTR_LEN(buf.s);

	smart_str_free(&buf);

	return SUCCESS;
}

static int zend_test_legacy_unserialize(
		zval *object,
		zend_class_entry *ce,
		const unsigned char *buf,
		size_t buf_len,
		zend_unserialize_data *data)
{
	php_unserialize_data_t *var_hash = (php_unserialize_data_t *) data;
	const unsigned char *pos = buf;
	zval *value;

	if (object_init_ex(object, ce) != SUCCESS) {
		return FAILURE;
	}

	value = var_tmp_var(var_hash);
	if (!php_var_unserialize(value, &pos, buf + buf_len, var_hash) || pos != buf + buf_len) {
		if (!EG(exception)) {
			zend_throw_exception(NULL, "Invalid ZendTestLegacySerializeObject payload", 0);
		}

		return FAILURE;
	}

	zend_update_property(ce, Z_OBJ_P(object), ZEND_STRL("data"), value);

	return SUCCESS;
}

ZEND_METHOD(ZendTestSleepObject, __construct)
{
	zval *public_value = NULL, *protected_value = NULL, *private_value = NULL;
	zend_object *obj = Z_OBJ_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_START(0, 3)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(public_value)
		Z_PARAM_ZVAL(protected_value)
		Z_PARAM_ZVAL(private_value)
	ZEND_PARSE_PARAMETERS_END();

	if (public_value != NULL) {
		zend_update_property(zend_test_sleep_object_ce, obj, ZEND_STRL("public"), public_value);
	}

	if (protected_value != NULL) {
		zend_update_property(zend_test_sleep_object_ce, obj, ZEND_STRL("protected"), protected_value);
	}

	if (private_value != NULL) {
		zend_update_property(zend_test_sleep_object_ce, obj, ZEND_STRL("private"), private_value);
	}
}

ZEND_METHOD(ZendTestSleepObject, __sleep)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_test_initialized_property_names(Z_OBJ_P(ZEND_THIS), return_value);
}

ZEND_METHOD(ZendTestSleepObject, __wakeup)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_update_property_bool(zend_test_sleep_object_ce, Z_OBJ_P(ZEND_THIS), ZEND_STRL("wokenUp"), true);
}

ZEND_METHOD(ZendTestSleepObjectWithCustomCreate, __sleep)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_test_initialized_property_names(Z_OBJ_P(ZEND_THIS), return_value);
}

void zend_test_serialization_init(void)
{
	zend_test_sleep_object_ce = register_class_ZendTestSleepObject();

	memcpy(&zend_test_custom_create_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));

	zend_test_sleep_object_with_custom_create_ce = register_class_ZendTestSleepObjectWithCustomCreate();
	zend_test_sleep_object_with_custom_create_ce->create_object = zend_test_custom_create_object_new;

	zend_test_legacy_serialize_object_ce = register_class_ZendTestLegacySerializeObject();
	zend_test_legacy_serialize_object_ce->serialize = zend_test_legacy_serialize;
	zend_test_legacy_serialize_object_ce->unserialize = zend_test_legacy_unserialize;
}
