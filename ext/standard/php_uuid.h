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

#ifndef PHP_UUID_H
#define PHP_UUID_H

#include "zend_types.h"
#include "php.h"

PHPAPI extern zend_class_entry *php_ce_UUID;
PHPAPI extern zend_class_entry *php_ce_UUIDParseException;

#define PHP_UUID_LEN 16
typedef struct php_uuid {
	uint8_t bytes[PHP_UUID_LEN];
} php_uuid;

#define PHP_UUID_HEX_LEN 33
typedef struct php_uuid_hex {
	char str[PHP_UUID_HEX_LEN];
} php_uuid_hex;

#define PHP_UUID_STRING_LEN 37
typedef struct php_uuid_string {
	char str[PHP_UUID_STRING_LEN];
} php_uuid_string;

typedef enum php_uuid_variant {
	PHP_UUID_VARIANT_NCS             = 0,
	PHP_UUID_VARIANT_RFC4122         = 1,
	PHP_UUID_VARIANT_MICROSOFT       = 2,
	PHP_UUID_VARIANT_FUTURE_RESERVED = 3,
} php_uuid_variant;

static const uint8_t PHP_UUID_VERSION_1_TIME_BASED      = 1;
static const uint8_t PHP_UUID_VERSION_2_DCE_SECURITY    = 2;
static const uint8_t PHP_UUID_VERSION_3_NAME_BASED_MD5  = 3;
static const uint8_t PHP_UUID_VERSION_4_RANDOM          = 4;
static const uint8_t PHP_UUID_VERSION_5_NAME_BASED_SHA1 = 5;

BEGIN_EXTERN_C()

PHPAPI void php_uuid_create_v3(php_uuid *uuid, const php_uuid *namespace, const char *name, const size_t name_len);

PHPAPI int php_uuid_create_v4(php_uuid *uuid, const zend_bool throw);

static zend_always_inline int php_uuid_create_v4_silent(php_uuid *uuid)
{
	return php_uuid_create_v4(uuid, 0);
}

static zend_always_inline int php_uuid_create_v4_throw(php_uuid *uuid)
{
	return php_uuid_create_v4(uuid, 1);
}

PHPAPI void php_uuid_create_v5(php_uuid *uuid, const php_uuid *namespace, const char *name, const size_t name_len);

static const zend_always_inline php_uuid_variant php_uuid_get_variant(const php_uuid *uuid)
{
	if ((uuid->bytes[8] & 0xC0) == 0x80) return PHP_UUID_VARIANT_RFC4122;
	if ((uuid->bytes[8] & 0xE0) == 0xC0) return PHP_UUID_VARIANT_MICROSOFT;
	if ((uuid->bytes[8] & 0x80) == 0x00) return PHP_UUID_VARIANT_NCS;
	return PHP_UUID_VARIANT_FUTURE_RESERVED;
}

static const zend_always_inline uint8_t php_uuid_get_version(const php_uuid *uuid)
{
	return uuid->bytes[6] >> 4;
}

static const zend_always_inline php_uuid php_uuid_namespace_dns()
{
	return (php_uuid) { "\x6b\xa7\xb8\x10\x9d\xad\x11\xd1\x80\xb4\x00\xc0\x4f\xd4\x30\xc8" };
}

static const zend_always_inline php_uuid php_uuid_namespace_oid()
{
	return (php_uuid) { "\x6b\xa7\xb8\x12\x9d\xad\x11\xd1\x80\xb4\x00\xc0\x4f\xd4\x30\xc8" };
}

static const zend_always_inline php_uuid php_uuid_namespace_url()
{
	return (php_uuid) { "\x6b\xa7\xb8\x11\x9d\xad\x11\xd1\x80\xb4\x00\xc0\x4f\xd4\x30\xc8" };
}

static const zend_always_inline php_uuid php_uuid_namespace_x500()
{
	return (php_uuid) { "\x6b\xa7\xb8\x14\x9d\xad\x11\xd1\x80\xb4\x00\xc0\x4f\xd4\x30\xc8" };
}

static const zend_always_inline php_uuid php_uuid_nil()
{
	return (php_uuid) { "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" };
}

static const zend_always_inline int php_uuid_is_nil(const php_uuid *uuid)
{
	const php_uuid nil = php_uuid_nil();

	return memcmp(uuid->bytes, &nil, PHP_UUID_LEN) == 0;
}

PHPAPI int php_uuid_parse(php_uuid *uuid, const char *input, const size_t input_len, const zend_bool throw);

static zend_always_inline int php_uuid_parse_silent(php_uuid *uuid, const char *input, const size_t input_len)
{
	return php_uuid_parse(uuid, input, input_len, 0);
}

static zend_always_inline int php_uuid_parse_throw(php_uuid *uuid, const char *input, const size_t input_len)
{
	return php_uuid_parse(uuid, input, input_len, 1);
}

static zend_always_inline void php_uuid_to_hex(php_uuid_hex *buffer, const php_uuid *uuid)
{
	sprintf(
		buffer->str,
		"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
		uuid->bytes[0], uuid->bytes[1], uuid->bytes[2], uuid->bytes[3],
		uuid->bytes[4], uuid->bytes[5],
		uuid->bytes[6], uuid->bytes[7],
		uuid->bytes[8], uuid->bytes[9],
		uuid->bytes[10], uuid->bytes[11], uuid->bytes[12], uuid->bytes[13], uuid->bytes[14], uuid->bytes[15]
	);
}

static zend_always_inline void php_uuid_to_string(php_uuid_string *buffer, const php_uuid *uuid)
{
	sprintf(
		buffer->str,
		"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		uuid->bytes[0], uuid->bytes[1], uuid->bytes[2], uuid->bytes[3],
		uuid->bytes[4], uuid->bytes[5],
		uuid->bytes[6], uuid->bytes[7],
		uuid->bytes[8], uuid->bytes[9],
		uuid->bytes[10], uuid->bytes[11], uuid->bytes[12], uuid->bytes[13], uuid->bytes[14], uuid->bytes[15]
	);
}

END_EXTERN_C()

PHP_MINIT_FUNCTION(uuid);

#endif /* PHP_UUID_H */
