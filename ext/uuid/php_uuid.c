/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Máté Kocsis <kocsismate@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php.h"
#include "Zend/zend_API.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_objects.h"
#include "Zend/zend_time.h"
#include "ext/date/php_date.h"
#include "ext/random/php_random.h"
#include "ext/standard/info.h"

#include "php_uuid.h"
#include "php_uuid_arginfo.h"
#include "uuidv7-h/uuidv7.h"

#define PHP_UUID_SERIALIZE_UUID_FIELD_NAME "uuid"

zend_class_entry *php_uuid_ce_uuidv7;

static zend_object_handlers object_handlers_uuidv7;

static const zend_module_dep uuid_deps[] = {
	ZEND_MOD_REQUIRED("date")
	ZEND_MOD_REQUIRED("random")
	ZEND_MOD_END
};

static inline php_uuid_v7_object *php_uuid_v7_object_from_obj(zend_object *object) {
	return (php_uuid_v7_object*)((char*)(object) - XtOffsetOf(php_uuid_v7_object, std));
}

#define Z_UUID_V7_OBJECT_P(zv) php_uuid_v7_object_from_obj(Z_OBJ_P((zv)))

ZEND_ATTRIBUTE_NONNULL_ARGS(1) PHPAPI zend_result php_uuid_v7_parse(const zend_string *uuid_str, php_uuid_v7 uuid)
{
	int result = uuidv7_from_string(ZSTR_VAL(uuid_str), uuid);

	return result == 0 ? SUCCESS : FAILURE;
}

ZEND_ATTRIBUTE_NONNULL PHPAPI zend_string *php_uuid_v7_to_string(const uint8_t *uuid)
{
	zend_string *uuid_string = zend_string_alloc(36, false);

	uuidv7_to_string(uuid, ZSTR_VAL(uuid_string));

	return uuid_string;
}

ZEND_ATTRIBUTE_NONNULL static HashTable *uuid_get_debug_properties(php_uuid_v7_object *uuid_object)
{
	const HashTable *std_properties = zend_std_get_properties(&uuid_object->std);
	HashTable *result = zend_array_dup(std_properties);

	zval tmp;
	ZVAL_STR_COPY(&tmp, php_uuid_v7_to_string(uuid_object->uuid));
	zend_hash_str_add(result, "uuid", sizeof("uuid") - 1, &tmp);
	zval_ptr_dtor(&tmp);

	return result;
}

PHP_METHOD(Uuid_UuidV7, parse)
{
	zend_string *uuid_str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(uuid_str)
	ZEND_PARSE_PARAMETERS_END();

	object_init_ex(return_value, Z_CE_P(ZEND_THIS));
	php_uuid_v7_object *uuid_object = Z_UUID_V7_OBJECT_P(return_value);

	if (uuidv7_from_string(ZSTR_VAL(uuid_str), uuid_object->uuid) == FAILURE) {
		zend_throw_exception(NULL, "The specified UUID v7 is malformed", 0);
		RETURN_THROWS();
	}

	uuid_object->is_initialized = false;
}

PHP_METHOD(Uuid_UuidV7, generate)
{
	zend_object *datetime_object = NULL, *random_engine_object = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OF_CLASS_OR_NULL(datetime_object, php_date_get_immutable_ce())
		Z_PARAM_OBJ_OF_CLASS_OR_NULL(random_engine_object, random_ce_Random_Engine)
	ZEND_PARSE_PARAMETERS_END();

	object_init_ex(return_value, Z_CE_P(ZEND_THIS));
	php_uuid_v7_object *uuid_object = Z_UUID_V7_OBJECT_P(return_value);

	uint64_t unix_time_ms;
	if (datetime_object == NULL) {
		unix_time_ms = zend_time_mono_fallback_nsec() / 1000000;
	} else {
		php_date_obj *datetime = php_date_obj_from_obj(datetime_object);
		if (!datetime->time) {
			zend_argument_error(NULL, 1, "is an unconstructed object");
			RETURN_THROWS();
		}

		zval zv_timestamp;
		zend_call_method_with_0_params(datetime_object, php_date_get_immutable_ce(), NULL, "gettimestamp", &zv_timestamp);
		if (Z_TYPE(zv_timestamp) != IS_LONG) {
			zend_throw_error(NULL, "Call to DateTimeImmutable::getTimestamp() failed");
			zval_ptr_dtor(&zv_timestamp);
			RETURN_THROWS();
		}

		unix_time_ms = (uint64_t) Z_LVAL(zv_timestamp) * 1000;
	}

	php_random_algo_with_state random_algo;
	if (random_engine_object == NULL) {
		random_algo = php_random_default_engine();
	} else {
		php_random_engine *random_engine = php_random_engine_from_obj(random_engine_object);
		random_algo = random_engine->engine;
	}

	uint8_t random_bytes[10];
	for (int i = 0; i < 10; i++) {
		random_bytes[i] = php_random_range(random_algo, 0, 127);
	}

	int8_t result = uuidv7_generate(uuid_object->uuid, unix_time_ms, random_bytes, NULL);
	switch (result) {
		case UUIDV7_STATUS_UNPRECEDENTED:
			ZEND_FALLTHROUGH;
		case UUIDV7_STATUS_NEW_TIMESTAMP:
			ZEND_FALLTHROUGH;
		case UUIDV7_STATUS_COUNTER_INC:
			ZEND_FALLTHROUGH;
		case UUIDV7_STATUS_TIMESTAMP_INC:
			ZEND_FALLTHROUGH;
		case UUIDV7_STATUS_CLOCK_ROLLBACK:
			ZEND_FALLTHROUGH;
		case UUIDV7_STATUS_ERR_TIMESTAMP:
			ZEND_FALLTHROUGH;
		case UUIDV7_STATUS_ERR_TIMESTAMP_OVERFLOW:
			break;
		default: ZEND_UNREACHABLE();
	}

	uuid_object->is_initialized = true;
}

PHP_METHOD(Uuid_UuidV7, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_throw_error(NULL, "Cannot directly construct %s, use the static factory methods instead", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
}

PHP_METHOD(Uuid_UuidV7, equals)
{
	zend_object *that_object;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OF_CLASS(that_object, php_uuid_ce_uuidv7)
	ZEND_PARSE_PARAMETERS_END();

	php_uuid_v7_object *this_uuid_object = Z_UUID_V7_OBJECT_P(ZEND_THIS);
	php_uuid_v7_object *that_uuid_object = php_uuid_v7_object_from_obj(that_object);
	ZEND_ASSERT(this_uuid_object->uuid != NULL);
	ZEND_ASSERT(that_uuid_object->uuid != NULL);

	if (this_uuid_object->std.ce != that_uuid_object->std.ce &&
		!instanceof_function(this_uuid_object->std.ce, that_uuid_object->std.ce) &&
		!instanceof_function(that_uuid_object->std.ce, this_uuid_object->std.ce)
	) {
		RETURN_FALSE;
	}

	for (int i = 0; i < 16; i++) {
		if (this_uuid_object->uuid[i] != that_uuid_object->uuid[i]) {
			RETURN_FALSE;
		}
	}

	RETURN_TRUE;
}

PHP_METHOD(Uuid_UuidV7, toBytes)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uuid_v7_object *uuid_object = Z_UUID_V7_OBJECT_P(ZEND_THIS);

	RETURN_STR(php_uuid_v7_to_string(uuid_object->uuid));
}

PHP_METHOD(Uuid_UuidV7, toString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uuid_v7_object *uuid_object = Z_UUID_V7_OBJECT_P(ZEND_THIS);

	RETURN_STR(php_uuid_v7_to_string(uuid_object->uuid));
}

PHP_METHOD(Uuid_UuidV7, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uuid_v7_object *uuid_object = Z_UUID_V7_OBJECT_P(ZEND_THIS);
	ZEND_ASSERT(uuid_object->uuid != NULL);

	/* Serialize state: "uuid" key in the first array */
	zval tmp;
	ZVAL_STR(&tmp, php_uuid_v7_to_string(uuid_object->uuid));

	array_init(return_value);

	zval arr;
	array_init(&arr);
	zend_hash_str_add_new(Z_ARRVAL(arr), PHP_UUID_SERIALIZE_UUID_FIELD_NAME, sizeof(PHP_UUID_SERIALIZE_UUID_FIELD_NAME) - 1, &tmp);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &arr);

	/* Serialize regular properties: second array */
	ZVAL_EMPTY_ARRAY(&arr);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &arr);
}

PHP_METHOD(Uuid_UuidV7, __unserialize)
{
	HashTable *data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(data)
	ZEND_PARSE_PARAMETERS_END();

	php_uuid_v7_object *uuid_object = php_uuid_v7_object_from_obj(Z_OBJ_P(ZEND_THIS));
	if (uuid_object->is_initialized) {
		/* Intentionally throw two exceptions for proper chaining. */
		zend_throw_error(NULL, "Cannot modify readonly object of class %s", ZSTR_VAL(uuid_object->std.ce->name));
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uuid_object->std.ce->name));
		RETURN_THROWS();
	}

	/* Verify the expected number of elements, this implicitly ensures that no additional elements are present. */
	if (zend_hash_num_elements(data) != 2) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uuid_object->std.ce->name));
		RETURN_THROWS();
	}

	/* Unserialize state: "uuid" key in the first array */
	zval *arr = zend_hash_index_find(data, 0);
	if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uuid_object->std.ce->name));
		RETURN_THROWS();
	}

	/* Verify the expected number of elements inside the first array, this implicitly ensures that no additional elements are present. */
	if (zend_hash_num_elements(Z_ARRVAL_P(arr)) != 1) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uuid_object->std.ce->name));
		RETURN_THROWS();
	}

	zval *uuid_zv = zend_hash_str_find(Z_ARRVAL_P(arr), ZEND_STRL(PHP_UUID_SERIALIZE_UUID_FIELD_NAME));
	if (uuid_zv == NULL || Z_TYPE_P(uuid_zv) != IS_STRING) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uuid_object->std.ce->name));
		RETURN_THROWS();
	}

	if (php_uuid_v7_parse(Z_STR_P(uuid_zv), uuid_object->uuid) == FAILURE) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uuid_object->std.ce->name));
		RETURN_THROWS();
	}

	/* Unserialize regular properties: second array */
	arr = zend_hash_index_find(data, 1);
	if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uuid_object->std.ce->name));
		RETURN_THROWS();
	}

	/* Verify that there is no regular property in the second array, because the UUID classes have no properties and they are final. */
	if (zend_hash_num_elements(Z_ARRVAL_P(arr)) > 0) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uuid_object->std.ce->name));
		RETURN_THROWS();
	}
}

PHP_METHOD(Uuid_UuidV7, __debugInfo)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uuid_v7_object *uuid_object = Z_UUID_V7_OBJECT_P(ZEND_THIS);

	RETURN_ARR(uuid_get_debug_properties(uuid_object));
}

ZEND_ATTRIBUTE_NONNULL zend_object *php_uuid_v7_object_create_uuid_v7(zend_class_entry *class_type)
{
	php_uuid_v7_object *uuid_object = zend_object_alloc(sizeof(*uuid_object), class_type);

	zend_object_std_init(&uuid_object->std, class_type);
	object_properties_init(&uuid_object->std, class_type);

	uuid_object->is_initialized = false;

	return &uuid_object->std;
}

ZEND_ATTRIBUTE_NONNULL void php_uuid_v7_object_handler_free(zend_object *object)
{
	php_uuid_v7_object *uuid_object = php_uuid_v7_object_from_obj(object);

	uuid_object->is_initialized = false;

	zend_object_std_dtor(&uuid_object->std);
}

ZEND_ATTRIBUTE_NONNULL zend_object *php_uuid_v7_object_handler_clone(zend_object *object)
{
	php_uuid_v7_object *uuid_object = php_uuid_v7_object_from_obj(object);

	ZEND_ASSERT(uuid_object->uuid != NULL);

	php_uuid_v7_object *new_uuid_object = php_uuid_v7_object_from_obj(object->ce->create_object(object->ce));

	memcpy(new_uuid_object->uuid, uuid_object->uuid, sizeof(php_uuid_v7));
	zend_objects_clone_members(&new_uuid_object->std, &uuid_object->std);

	return &new_uuid_object->std;
}

static PHP_MINIT_FUNCTION(uuid)
{
	php_uuid_ce_uuidv7 = register_class_Uuid_UuidV7();
	php_uuid_ce_uuidv7->create_object = php_uuid_v7_object_create_uuid_v7;
	php_uuid_ce_uuidv7->default_object_handlers = &object_handlers_uuidv7;
	memcpy(&object_handlers_uuidv7, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	object_handlers_uuidv7.offset = XtOffsetOf(php_uuid_v7_object, std);
	object_handlers_uuidv7.free_obj = php_uuid_v7_object_handler_free;
	object_handlers_uuidv7.clone_obj = php_uuid_v7_object_handler_clone;

	return SUCCESS;
}

static PHP_MINFO_FUNCTION(uuid)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "UUID support", "active");
	php_info_print_table_end();
}

zend_module_entry uuid_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	uuid_deps,
	"uuid",                         /* Extension name */
	NULL,                           /* zend_function_entry */
	PHP_MINIT(uuid),                /* PHP_MINIT - Module initialization */
	NULL,                           /* PHP_MSHUTDOWN - Module shutdown */
	NULL,                           /* PHP_RINIT - Request initialization */
	NULL,                           /* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(uuid),                /* PHP_MINFO - Module info */
	PHP_VERSION,                    /* Version */
	NO_MODULE_GLOBALS,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
