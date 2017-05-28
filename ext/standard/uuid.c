/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Richard Fussenegger <php@fleshgrinder.com>                   |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php_uuid.h"

#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "md5.h"
#include "php_random.h"
#include "sha1.h"

PHPAPI zend_class_entry *php_ce_UUID;
PHPAPI zend_class_entry *php_ce_UUIDParseException;

PHPAPI const php_uuid PHP_UUID_NAMESPACE_DNS            = "\x6b\xa7\xb8\x10\x9d\xad\x11\xd1\x80\xb4\x00\xc0\x4f\xd4\x30\xc8";
PHPAPI const php_uuid PHP_UUID_NAMESPACE_OID            = "\x6b\xa7\xb8\x12\x9d\xad\x11\xd1\x80\xb4\x00\xc0\x4f\xd4\x30\xc8";
PHPAPI const php_uuid PHP_UUID_NAMESPACE_URL            = "\x6b\xa7\xb8\x11\x9d\xad\x11\xd1\x80\xb4\x00\xc0\x4f\xd4\x30\xc8";
PHPAPI const php_uuid PHP_UUID_NAMESPACE_X500           = "\x6b\xa7\xb8\x14\x9d\xad\x11\xd1\x80\xb4\x00\xc0\x4f\xd4\x30\xc8";
PHPAPI const php_uuid PHP_UUID_NIL                      = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

static const uint8_t PHP_UUID_VERSION_MIN               = 0;
PHPAPI const uint8_t PHP_UUID_VERSION_1_TIME_BASED      = 1;
PHPAPI const uint8_t PHP_UUID_VERSION_2_DCE_SECURITY    = 2;
PHPAPI const uint8_t PHP_UUID_VERSION_3_NAME_BASED_MD5  = 3;
PHPAPI const uint8_t PHP_UUID_VERSION_4_RANDOM          = 4;
PHPAPI const uint8_t PHP_UUID_VERSION_5_NAME_BASED_SHA1 = 5;
static const uint8_t PHP_UUID_VERSION_MAX               = 15;

PHPAPI const uint8_t PHP_UUID_LEN                       = sizeof(php_uuid);
PHPAPI const uint8_t PHP_UUID_HEX_LEN                   = sizeof(php_uuid_hex) - 1;
PHPAPI const uint8_t PHP_UUID_STRING_LEN                = sizeof(php_uuid_string) - 1;

static const char UUID_BINARY_PROP[]                    = "binary";
static const uint8_t UUID_BINARY_PROP_LEN               = sizeof(UUID_BINARY_PROP) - 1;

static const char UUID_EX_INPUT_PROP[]                  = "input";
static const uint8_t UUID_EX_INPUT_PROP_LEN             = sizeof(UUID_EX_INPUT_PROP) - 1;
static const char UUID_EX_POSITION_PROP[]               = "position";
static const uint8_t UUID_EX_POSITON_PROP_LEN           = sizeof(UUID_EX_POSITION_PROP) - 1;

static const char URN_PREFIX[]                          = "urn:uuid:";
static const uint8_t URN_PREFIX_LEN                     = sizeof(URN_PREFIX) - 1;

/**
 * Set UUID variant to RFC 4122, the only supported variant.
 *
 * @param[out] uuid to set the variant.
 */
static zend_always_inline void set_variant_rfc4122(php_uuid *uuid)
{
	(*uuid)[8] = ((*uuid)[8] & 0x3F) | 0x80;
}

/**
 * Set UUID version.
 *
 * @see PHP_UUID_VERSION_1_TIME_BASED
 * @see PHP_UUID_VERSION_2_DCE_SECURITY
 * @see PHP_UUID_VERSION_3_NAME_BASED_MD5
 * @see PHP_UUID_VERSION_4_RANDOM
 * @see PHP_UUID_VERSION_5_NAME_BASED_SHA1
 * @param[out] uuid to set the version.
 * @param[in] version to set.
 */
static zend_always_inline void php_uuid_set_version(php_uuid *uuid, const uint8_t version)
{
	assert(PHP_UUID_VERSION_MIN <= version && version <= PHP_UUID_VERSION_MAX);

	(*uuid)[6] = ((*uuid)[6] & 0x0F) | (version << 4);
}

/**
 * Throw UUIDParseException.
 *
 * @param[in] input that could not be parsed.
 * @param[in] input_len
 * @param[in] position at which parsing failed.
 * @param[in] reason why parsing failed.
 * @param[in] ... arguments to format the reason.
 */
static ZEND_COLD zend_object *throw_uuid_parse_exception(const char *input, const size_t input_len, const size_t position, const char *reason, ...)
{
	va_list format_args;
	char *formatted_reason;
	zend_object *object;
	zval exception;

	va_start(format_args, reason);
	zend_vspprintf(&formatted_reason, 0, reason, format_args);
	va_end(format_args);
	object = zend_throw_exception(php_ce_UUIDParseException, formatted_reason, 0);
	efree(formatted_reason);

	ZVAL_OBJ(&exception, object);
	zend_update_property_stringl(php_ce_UUIDParseException, &exception, UUID_EX_INPUT_PROP, UUID_EX_INPUT_PROP_LEN, input, input_len);
	zend_update_property_long(php_ce_UUIDParseException, &exception, UUID_EX_POSITION_PROP, UUID_EX_POSITON_PROP_LEN, position);

	return object;
}

/**
 * Throw UUIDParseException because of insufficient input.
 *
 * @param[in] input that could not be parsed.
 * @param[in] input_len
 * @param[in] position at which parsing failed.
 * @param[in] actual amount of characters that were found.
 */
static zend_always_inline zend_object *throw_uuid_parse_exception_invalid_len(const char *input, const size_t input_len, const size_t position, const size_t actual)
{
	return throw_uuid_parse_exception(
		input,
		input_len,
		position,
		"Expected at least %u hexadecimal digits, but got %u",
		PHP_UUID_HEX_LEN,
		actual
	);
}

/**
 * Throw UUIDParseException because of an invalid character.
 *
 * @param[in] input that could not be parsed.
 * @param[in] input_len
 * @param[in] position at which parsing failed.
 */
static zend_always_inline zend_object *throw_uuid_parse_exception_invalid_char(const char *input, const size_t input_len, const size_t position)
{
	return throw_uuid_parse_exception(
		input,
		input_len,
		position,
		"Expected hexadecimal digit, but found '%c' (0x%02x)",
		input[position],
		input[position]
	);
}

PHPAPI int php_uuid_parse(php_uuid *uuid, const char *input, const size_t input_len, const zend_bool throw)
{
	char chr;
	size_t position = 0;
	size_t digit    = 0;
	size_t limit    = input_len - 1;
	uint8_t byte    = 0;

	while (input[position] == '-' || input[position] == ' ' || input[position] == '\t' || input[position] == '{') {
		++position;
	}

	if (memcmp(input + position, URN_PREFIX, URN_PREFIX_LEN) == 0) {
		position += URN_PREFIX_LEN;
	}

	while (input[limit] == '-' || input[limit] == ' ' || input[limit] == '\t' || input[limit] == '}') {
		--limit;
	}

	if ((limit - position + 1) < PHP_UUID_HEX_LEN) {
		if (throw) {
			throw_uuid_parse_exception_invalid_len(input, input_len, position, limit - position + 1);
		}
		return FAILURE;
	}

	for (; position <= limit; ++position) {
		chr = input[position];

		/* First digit of the byte. */
		if (digit % 2 == 0) {
			if ('0' <= chr && chr <= '9') {
				byte = chr - '0';
			}
			else if ('a' <= chr && chr <= 'f') {
				byte = chr - 'a' + 10;
			}
			else if ('A' <= chr && chr <= 'F') {
				byte = chr - 'A' + 10;
			}
			else if (chr == '-') {
				continue;
			}
			else {
				if (throw) {
					throw_uuid_parse_exception_invalid_char(input, input_len, position);
				}
				return FAILURE;
			}
		}
		/* Second digit of the byte. */
		else {
			/* Shift upper half. */
			byte *= 16;

			if ('0' <= chr && chr <= '9') {
				byte += chr - '0';
			}
			else if ('a' <= chr && chr <= 'f') {
				byte += chr - 'a' + 10;
			}
			else if ('A' <= chr && chr <= 'F') {
				byte += chr - 'A' + 10;
			}
			else if (chr == '-') {
				continue;
			}
			else {
				if (throw) {
					throw_uuid_parse_exception_invalid_char(input, input_len, position);
				}
				return FAILURE;
			}

			if (digit > PHP_UUID_HEX_LEN) {
				if (throw) {
					throw_uuid_parse_exception(
						input,
						input_len,
						position,
						"Expected no more than %u hexadecimal digits",
						PHP_UUID_HEX_LEN
					);
				}
				return FAILURE;
			}

			(*uuid)[digit / 2] = byte;
		}

		++digit;
	}

	if (digit < PHP_UUID_HEX_LEN) {
		if (throw) {
			throw_uuid_parse_exception_invalid_len(input, input_len, position, digit);
		}
		return FAILURE;
	}

	return SUCCESS;
}

PHPAPI void php_uuid_create_v3(php_uuid *uuid, const php_uuid *namespace, const char *name, const size_t name_len)
{
	PHP_MD5_CTX context;

	PHP_MD5Init(&context);
	PHP_MD5Update(&context, (char *) namespace, PHP_UUID_LEN);
	PHP_MD5Update(&context, name, name_len);
	PHP_MD5Final((char *)uuid, &context);
	set_variant_rfc4122(uuid);
	php_uuid_set_version(uuid, PHP_UUID_VERSION_3_NAME_BASED_MD5);
}

PHPAPI int php_uuid_create_v4(php_uuid *uuid, const zend_bool throw)
{
	int result = php_random_bytes(uuid, PHP_UUID_LEN, throw);

	if (result == SUCCESS) {
		set_variant_rfc4122(uuid);
		php_uuid_set_version(uuid, PHP_UUID_VERSION_4_RANDOM);
	}

	return result;
}

PHPAPI void php_uuid_create_v5(php_uuid *uuid, const php_uuid *namespace, const char *name, const size_t name_len)
{
	PHP_SHA1_CTX context;
	char digest[20];

	PHP_SHA1Init(&context);
	PHP_SHA1Update(&context, (char *) namespace, PHP_UUID_LEN);
	PHP_SHA1Update(&context, name, name_len);
	PHP_SHA1Final(digest, &context);
	memcpy(uuid, digest, PHP_UUID_LEN);
	set_variant_rfc4122(uuid);
	php_uuid_set_version(uuid, PHP_UUID_VERSION_5_NAME_BASED_SHA1);
}

PHPAPI zend_always_inline const php_uuid_variant php_uuid_get_variant(const php_uuid uuid)
{
	if ((uuid[8] & 0xC0) == 0x80) return PHP_UUID_VARIANT_RFC4122;
	if ((uuid[8] & 0xE0) == 0xC0) return PHP_UUID_VARIANT_MICROSOFT;
	if ((uuid[8] & 0x80) == 0x00) return PHP_UUID_VARIANT_NCS;
	return PHP_UUID_VARIANT_FUTURE_RESERVED;
}

PHPAPI zend_always_inline const uint8_t php_uuid_get_version(const php_uuid uuid)
{
	return uuid[6] >> 4;
}

PHPAPI zend_always_inline const int php_uuid_is_nil(const php_uuid uuid)
{
	return memcmp(uuid, PHP_UUID_NIL, PHP_UUID_LEN) == 0;
}

PHPAPI zend_always_inline void php_uuid_to_hex(php_uuid_hex *buffer, const php_uuid uuid)
{
	sprintf(
		(char *) buffer,
		"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
		uuid[0], uuid[1], uuid[2], uuid[3],
		uuid[4], uuid[5],
		uuid[6], uuid[7],
		uuid[8], uuid[9],
		uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]
	);
}

PHPAPI zend_always_inline void php_uuid_to_string(php_uuid_string *buffer, const php_uuid uuid)
{
	sprintf(
		(char *) buffer,
		"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		uuid[0], uuid[1], uuid[2], uuid[3],
		uuid[4], uuid[5],
		uuid[6], uuid[7],
		uuid[8], uuid[9],
		uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]
	);
}

/**
 * Get the UUID's bytes from the private property.
 *
 * @param[in] object to get the bytes from.
 */
static zend_always_inline php_uuid *get_bytes(/*const*/ zval *object)
{
	return (php_uuid *) Z_STRVAL_P(zend_read_property(php_ce_UUID, object, UUID_BINARY_PROP, UUID_BINARY_PROP_LEN, 1, NULL));
}

/**
 * Construct new UUID instance.
 *
 * @param[out] object to store the UUID instance int.
 * @param[in] uuid to construct the instance from.
 */
static zend_always_inline void new_uuid(zval *object, const php_uuid uuid)
{
	object_init_ex(object, php_ce_UUID);
	zend_update_property_stringl(php_ce_UUID, object, UUID_BINARY_PROP, UUID_BINARY_PROP_LEN, uuid, PHP_UUID_LEN);
}

/* private function __construct() {{{ */
PHP_METHOD(UUID, __construct)
{
	/* NOOP */
}
ZEND_BEGIN_ARG_INFO(UUID___construct_args, NULL)
ZEND_END_ARG_INFO()
/* }}} */

/* public static function fromBinary(string $input): self {{{ */
PHP_METHOD(UUID, fromBinary)
{
	char *input = NULL;
	size_t input_len;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "s", &input, &input_len) == FAILURE) {
		return;
	}

	if (input_len != PHP_UUID_LEN) {
		zend_throw_exception_ex(
			spl_ce_InvalidArgumentException,
			0,
			"Expected exactly %u bytes, but got %u",
			PHP_UUID_LEN,
			input_len
		);
		return;
	}

	new_uuid(return_value, input);
}
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(UUID_fromBinary_args, 0, 1, self, 0)
	ZEND_ARG_TYPE_INFO(0, input, IS_STRING, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* public static function parse(string $input): self {{{ */
PHP_METHOD(UUID, parse)
{
	char *input = NULL;
	size_t input_len;
	php_uuid uuid;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "s", &input, &input_len) == FAILURE) {
		return;
	}

	if (php_uuid_parse_throw(&uuid, input, input_len) == SUCCESS) {
		new_uuid(return_value, uuid);
	}
}
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(UUID_parse_args, 0, 1, self, 0)
	ZEND_ARG_TYPE_INFO(0, input, IS_STRING, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* public static function v3(self $namespace, string $name): self {{{ */
PHP_METHOD(UUID, v3)
{
	zval *namespace = NULL;
	char *name      = NULL;
	size_t name_len;
	php_uuid uuid;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zs", &namespace, &name, &name_len) == FAILURE) {
		return;
	}

	php_uuid_create_v3(&uuid, get_bytes(namespace), name, name_len);
	new_uuid(return_value, uuid);
}
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(UUID_v3_args, 0, 2, self, 0)
	ZEND_ARG_OBJ_INFO(0, namespace, self, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* public static function v4(): self {{{ */
PHP_METHOD(UUID, v4)
{
	php_uuid uuid;

	if (zend_parse_parameters_none_throw() == FAILURE) {
		return;
	}

	if (php_uuid_create_v4_throw(&uuid) == SUCCESS) {
		new_uuid(return_value, uuid);
	}
}
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO(UUID_v4_args, self, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* public static function v5(self $namespace, string $name): self {{{ */
PHP_METHOD(UUID, v5)
{
	zval *namespace = NULL;
	char *name      = NULL;
	size_t name_len;
	php_uuid uuid;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zs", &namespace, &name, &name_len) == FAILURE) {
		return;
	}

	php_uuid_create_v5(&uuid, get_bytes(namespace), name, name_len);
	new_uuid(return_value, uuid);
}
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(UUID_v5_args, 0, 2, self, 0)
	ZEND_ARG_OBJ_INFO(0, namespace, self, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* public static function NamespaceDNS(): self {{{ */
PHP_METHOD(UUID, NamespaceDNS)
{
	if (zend_parse_parameters_none_throw() == FAILURE) {
		return;
	}

	new_uuid(return_value, PHP_UUID_NAMESPACE_DNS);
}
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO(UUID_NamespaceDNS_args, self, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* public static function NamespaceOID(): self {{{ */
PHP_METHOD(UUID, NamespaceOID)
{
	if (zend_parse_parameters_none_throw() == FAILURE) {
		return;
	}

	new_uuid(return_value, PHP_UUID_NAMESPACE_OID);
}
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO(UUID_NamespaceOID_args, self, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* public static function NamespaceURL(): self {{{ */
PHP_METHOD(UUID, NamespaceURL)
{
	if (zend_parse_parameters_none_throw() == FAILURE) {
		return;
	}

	new_uuid(return_value, PHP_UUID_NAMESPACE_URL);
}
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO(UUID_NamespaceURL_args, self, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* public static function NamespaceX500(): self {{{ */
PHP_METHOD(UUID, NamespaceX500)
{
	if (zend_parse_parameters_none_throw() == FAILURE) {
		return;
	}

	new_uuid(return_value, PHP_UUID_NAMESPACE_X500);
}
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO(UUID_NamespaceX500_args, self, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* public static function Nil(): self {{{ */
PHP_METHOD(UUID, Nil)
{
	if (zend_parse_parameters_none_throw() == FAILURE) {
		return;
	}

	new_uuid(return_value, PHP_UUID_NIL);
}
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO(UUID_Nil_args, self, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* private function __clone() {{{ */
PHP_METHOD(UUID, __clone)
{
	zend_throw_error(zend_ce_error, "Cannot clone immutable %s object", ZSTR_VAL(php_ce_UUID->name));
}
ZEND_BEGIN_ARG_INFO(UUID___clone_args, NULL)
ZEND_END_ARG_INFO()
/* }}} */

/* public function __set($_, $__): void {{{ */
PHP_METHOD(UUID, __set)
{
	zend_throw_error(zend_ce_error, "Cannot set dynamic properties on immutable %s object", ZSTR_VAL(php_ce_UUID->name));
}
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(UUID___set_args, IS_VOID, 0)
	ZEND_ARG_INFO(0, _)
	ZEND_ARG_INFO(0, __)
ZEND_END_ARG_INFO()
/* }}} */

/* public function __wakeup(): void {{{ */
PHP_METHOD(UUID, __wakeup)
{
	size_t len = Z_STRLEN_P(zend_read_property(php_ce_UUID, &EX(This), UUID_BINARY_PROP, UUID_BINARY_PROP_LEN, 1, NULL));

	if (zend_parse_parameters_none_throw() == FAILURE) {
		return;
	}

	if (len != PHP_UUID_LEN) {
		zend_throw_exception_ex(
			spl_ce_UnexpectedValueException,
			0,
			"Expected exactly %u bytes, but found %u",
			PHP_UUID_LEN,
			len
		);
	}
}
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(UUID___wakeup_args, IS_VOID, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* public function getVaraitn(): int {{{ */
PHP_METHOD(UUID, getVariant)
{
	if (zend_parse_parameters_none_throw() == FAILURE) {
		return;
	}

	RETURN_LONG(php_uuid_get_variant(*get_bytes(&EX(This))));
}
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(UUID_getVariant_args, IS_LONG, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* public function getVersion(): int {{{ */
PHP_METHOD(UUID, getVersion)
{
	if (zend_parse_parameters_none_throw() == FAILURE) {
		return;
	}

	RETURN_LONG(php_uuid_get_version(*get_bytes(&EX(This))));
}
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(UUID_getVersion_args, IS_LONG, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* public function isNil(): bool {{{ */
PHP_METHOD(UUID, isNil)
{
	if (zend_parse_parameters_none_throw() == FAILURE) {
		return;
	}

	RETURN_BOOL(php_uuid_is_nil(*get_bytes(&EX(This))));
}
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(UUID_isNil_args, _IS_BOOL, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* public function toBinary(): string {{{ */
PHP_METHOD(UUID, toBinary)
{
	if (zend_parse_parameters_none_throw() == FAILURE) {
		return;
	}

	RETURN_STRINGL(*get_bytes(&EX(This)), PHP_UUID_LEN);
}
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(UUID_toBinary_args, IS_STRING, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* public function toHex(): string {{{ */
PHP_METHOD(UUID, toHex)
{
	php_uuid_hex buffer;

	if (zend_parse_parameters_none_throw() == FAILURE) {
		return;
	}

	php_uuid_to_hex(&buffer, *get_bytes(&EX(This)));
	RETURN_STRINGL(buffer, PHP_UUID_HEX_LEN);
}
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(UUID_toHex_args, IS_STRING, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* public function toString(): string {{{ */
PHP_METHOD(UUID, toString)
{
	php_uuid_string buffer;

	if (zend_parse_parameters_none_throw() == FAILURE) {
		return;
	}

	php_uuid_to_string(&buffer, *get_bytes(&EX(This)));
	RETURN_STRINGL(buffer, PHP_UUID_STRING_LEN);
}
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(UUID_toString_args, IS_STRING, 0)
ZEND_END_ARG_INFO()
/* }}} */

static const zend_function_entry uuid_methods[] = {
	PHP_ME(UUID, __construct,   UUID___construct_args,   ZEND_ACC_PRIVATE)
	PHP_ME(UUID, fromBinary,    UUID_fromBinary_args,    ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(UUID, parse,         UUID_parse_args,         ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(UUID, v3,            UUID_v3_args,            ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(UUID, v4,            UUID_v4_args,            ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(UUID, v5,            UUID_v5_args,            ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(UUID, NamespaceDNS,  UUID_NamespaceDNS_args,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(UUID, NamespaceOID,  UUID_NamespaceOID_args,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(UUID, NamespaceURL,  UUID_NamespaceURL_args,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(UUID, NamespaceX500, UUID_NamespaceX500_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(UUID, Nil,           UUID_Nil_args,           ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(UUID, __clone,       UUID___clone_args,       ZEND_ACC_PRIVATE)
	PHP_ME(UUID, __set,         UUID___set_args,         ZEND_ACC_PUBLIC)
	PHP_ME(UUID, __wakeup,      UUID___wakeup_args,      ZEND_ACC_PUBLIC)
	PHP_ME(UUID, getVariant,    UUID_getVariant_args,    ZEND_ACC_PUBLIC)
	PHP_ME(UUID, getVersion,    UUID_getVersion_args,    ZEND_ACC_PUBLIC)
	PHP_ME(UUID, isNil,         UUID_isNil_args,         ZEND_ACC_PUBLIC)
	PHP_ME(UUID, toBinary,      UUID_toBinary_args,      ZEND_ACC_PUBLIC)
	PHP_ME(UUID, toHex,         UUID_toHex_args,         ZEND_ACC_PUBLIC)
	PHP_ME(UUID, toString,      UUID_toString_args,      ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/* public function __construct(string $reason, string $input, int $position = 0, ?Throwable $previous = null) {{{ */
PHP_METHOD(UUIDParseException, __construct)
{
	zval *reason   = NULL;
	zval *input    = NULL;
	zval *position = NULL;
	zval *previous = NULL;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zz|zz!", &reason, &input, &position, &previous) == FAILURE) {
		return;
	}

	zend_update_property_ex(zend_ce_exception, &EX(This), ZSTR_KNOWN(ZEND_STR_MESSAGE), reason);
	zend_update_property(php_ce_UUIDParseException, &EX(This), UUID_EX_INPUT_PROP, UUID_EX_INPUT_PROP_LEN, input);

	if (position != NULL) {
		zend_update_property(php_ce_UUIDParseException, &EX(This), UUID_EX_POSITION_PROP, UUID_EX_POSITON_PROP_LEN, position);
	}
	else {
		zend_update_property_long(php_ce_UUIDParseException, &EX(This), UUID_EX_POSITION_PROP, UUID_EX_POSITON_PROP_LEN, 0);
	}

	if (previous != NULL) {
		zend_update_property_ex(zend_ce_exception, &EX(This), ZSTR_KNOWN(ZEND_STR_PREVIOUS), previous);
	}
}
ZEND_BEGIN_ARG_INFO_EX(UUIDParseException___construct_args, NULL, 0, 2)
	ZEND_ARG_TYPE_INFO(0, reason, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, input, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, position, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, previous, Throwable, 1)
ZEND_END_ARG_INFO()
/* }}} */

/* public function getInput(): string {{{ */
PHP_METHOD(UUIDParseException, getInput)
{
	if (zend_parse_parameters_none_throw() == FAILURE) {
		return;
	}

	RETURN_ZVAL(zend_read_property(
		php_ce_UUIDParseException,
		&EX(This),
		UUID_EX_INPUT_PROP,
		UUID_EX_INPUT_PROP_LEN,
		1,
		NULL
	), 1, 0);
}
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(UUIDParseException_getInput_args, IS_STRING, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* public function getPosition(): int {{{ */
PHP_METHOD(UUIDParseException, getPosition)
{
	if (zend_parse_parameters_none_throw() == FAILURE) {
		return;
	}

	RETURN_ZVAL(zend_read_property(
		php_ce_UUIDParseException,
		&EX(This),
		UUID_EX_POSITION_PROP,
		UUID_EX_POSITON_PROP_LEN,
		1,
		NULL
	), 1, 0);
}
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(UUIDParseException_getPosition_args, IS_LONG, 0)
ZEND_END_ARG_INFO()
/* }}} */

static const zend_function_entry uuid_parse_exception_methods[] = {
	PHP_ME(UUIDParseException, __construct, UUIDParseException___construct_args, ZEND_ACC_PUBLIC)
	PHP_ME(UUIDParseException, getInput,    UUIDParseException_getInput_args,    ZEND_ACC_PUBLIC)
	PHP_ME(UUIDParseException, getPosition, UUIDParseException_getPosition_args, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

PHP_MINIT_FUNCTION(uuid)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "UUID", uuid_methods);
	php_ce_UUID = zend_register_internal_class(&ce);
	php_ce_UUID->ce_flags |= ZEND_ACC_FINAL;

	zend_declare_class_constant_long(php_ce_UUID, "VARIANT_NCS", sizeof("VARIANT_NCS") - 1, PHP_UUID_VARIANT_NCS);
	zend_declare_class_constant_long(php_ce_UUID, "VARIANT_RFC4122", sizeof("VARIANT_RFC4122") - 1, PHP_UUID_VARIANT_RFC4122);
	zend_declare_class_constant_long(php_ce_UUID, "VARIANT_MICROSOFT", sizeof("VARIANT_MICROSOFT") - 1, PHP_UUID_VARIANT_MICROSOFT);
	zend_declare_class_constant_long(php_ce_UUID, "VARIANT_FUTURE_RESERVED", sizeof("VARIANT_FUTURE_RESERVED") - 1, PHP_UUID_VARIANT_FUTURE_RESERVED);

	zend_declare_class_constant_long(php_ce_UUID, "VERSION_1_TIME_BASED", sizeof("VERSION_1_TIME_BASED") - 1, PHP_UUID_VERSION_1_TIME_BASED);
	zend_declare_class_constant_long(php_ce_UUID, "VERSION_2_DCE_SECURITY", sizeof("VERSION_2_DCE_SECURITY") - 1, PHP_UUID_VERSION_2_DCE_SECURITY);
	zend_declare_class_constant_long(php_ce_UUID, "VERSION_3_NAME_BASED_MD5", sizeof("VERSION_3_NAME_BASED_MD5") - 1, PHP_UUID_VERSION_3_NAME_BASED_MD5);
	zend_declare_class_constant_long(php_ce_UUID, "VERSION_4_RANDOM", sizeof("VERSION_4_RANDOM") - 1, PHP_UUID_VERSION_4_RANDOM);
	zend_declare_class_constant_long(php_ce_UUID, "VERSION_5_NAME_BASED_SHA1", sizeof("VERSION_5_NAME_BASED_SHA1") - 1, PHP_UUID_VERSION_5_NAME_BASED_SHA1);

	zend_declare_property_null(php_ce_UUID, UUID_BINARY_PROP, UUID_BINARY_PROP_LEN, ZEND_ACC_PRIVATE);

	INIT_CLASS_ENTRY(ce, "UUIDParseException", uuid_parse_exception_methods);
	php_ce_UUIDParseException = zend_register_internal_class_ex(&ce, zend_ce_exception);
	php_ce_UUIDParseException->ce_flags |= ZEND_ACC_FINAL;
	php_ce_UUIDParseException->create_object = zend_ce_exception->create_object;

	zend_declare_property_null(php_ce_UUIDParseException, UUID_EX_INPUT_PROP, UUID_EX_INPUT_PROP_LEN, ZEND_ACC_PRIVATE);
	zend_declare_property_null(php_ce_UUIDParseException, UUID_EX_POSITION_PROP, UUID_EX_POSITON_PROP_LEN, ZEND_ACC_PRIVATE);

	return SUCCESS;
}
