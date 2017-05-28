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

/**
 * Generate and parse RFC 4122/DCE 1.1 Universally Unique Identifiers (UUIDs).
 *
 * This standard submodule provides generation and parsing capabilities for
 * Universally Unique Identifiers (UUIDs, also known as Globally Unique
 * Identifiers [GUIDs]) as specified in RFC 4122.
 *
 * UUIDs are 128 bit integers that guarantee uniqueness across space and time.
 * They are mainly used to assign identifiers to entities without requiring a
 * central authority. They are thus particularly useful in distributed systems.
 * They also allow very high allocation rates; up to 10 million per second per
 * machine, if necessary.
 *
 * There are different types of UUIDs, known as variants. This implementation
 * generates UUIDs according to the Leach-Salz variant; the one specified in
 * RFC 4122 as variant 1. Textual parsing supports both variant 1 (RFC 4122)
 * and variant 2 (Microsoft), and construction supports any kind of UUID.
 * However, note that the provided functions are **not** guaranteed to provide
 * meaningful results if any other variants than the Leach-Salz one is used.
 *
 * There are also different UUID generation algorithms, known as versions. This
 * implementation provides functions to generate version 3, 4, and 5 UUIDs.
 *
 * Versions 3 and 5 are meant for generating UUIDs from "names" that are drawn
 * from, and unique within, some "namespace". They generate the same UUID for
 * the same name in the same namespace across all RFC 4122 compliant
 * implementations.
 *
 * Version 4 UUIDs are random and result in a new UUID upon every generation.
 * The randomness is provided by PHP's random bytes implementation, and thus
 * uses the best available random source of the operating system.
 *
 * Versions 1 and 2 are not supported due to privacy/security concerns. Refer
 * to the Wikipedia article for more information.
 *
 * This implementation is inspired by many other implementations:
 * - [RFC 4122 Sample Implementation](https://tools.ietf.org/html/rfc4122#appendix-A)
 * - [Rust UUID](https://github.com/rust-lang-nursery/uuid)
 * - [util-linux LibUUID](https://github.com/karelzak/util-linux)
 * - [boost Uuid](http://www.boost.org/doc/libs/1_64_0/libs/uuid/)
 * - [D std.uuid](https://dlang.org/library/std/uuid.html)
 *
 * ## Examples
 * ```
 * php_uuid uuid;
 * 
 * // A random UUID with more randomness than the version 4 implementation.
 * // This should NOT be used in real-world applications!
 * php_random_bytes_silent(&uuid, PHP_UUID_LEN);
 * php_uuid_set_variant(&uuid, PHP_UUID_VARIANT_FUTURE_RESERVED);
 * 
 * php_uuid_create_v3(&uuid, &PHP_UUID_NAMESPACE_DNS, "php.net", sizeof("php.net") - 1);
 * assert(php_uuid_is_nil(uuid) == FALSE);
 * assert(php_uuid_get_variant(uuid) == PHP_UUID_VARIANT_RFC4122);
 * assert(php_uuid_get_version(uuid) == PHP_UUID_VERSION_3_NAME_BASED_MD5);
 * 
 * php_uuid_create_v4_silent(&uuid);
 * assert(php_uuid_is_nil(uuid) == FALSE);
 * assert(php_uuid_get_variant(uuid) == PHP_UUID_VARIANT_RFC4122);
 * assert(php_uuid_get_version(uuid) == PHP_UUID_VERSION_4_RANDOM);
 * 
 * php_uuid_create_v5(&uuid, &PHP_UUID_NAMESPACE_DNS, "php.net", sizeof("php.net") - 1);
 * assert(php_uuid_is_nil(uuid) == FALSE);
 * assert(php_uuid_get_variant(uuid) == PHP_UUID_VARIANT_RFC4122);
 * assert(php_uuid_get_version(uuid) == PHP_UUID_VERSION_5_NAME_BASED_SHA1);
 * 
 * php_uuid_parse_silent(&uuid, "urn:uuid:123E4567-E89B-12D3-A456-426655440000", sizeof("urn:uuid:123E4567-E89b-12D3-A456-426655440000") - 1);
 * assert(php_uuid_is_nil(uuid) == FALSE);
 * assert(php_uuid_get_variant(uuid) == PHP_UUID_VARIANT_RFC4122);
 * assert(php_uuid_get_version(uuid) == PHP_UUID_VERSION_1_TIME_BASED);
 * 
 * assert(memcmp(&uuid, "\x12\x3E\x45\x67\xE8\x9B\x12\xD3\xA4\x56\x42\x66\x55\x44\x00\x00", PHP_UUID_LEN) == 0);
 *
 * php_uuid_hex hex;
 * php_uuid_to_hex(&hex, uuid);
 * assert(memcmp(&hex, "123e4567e89b12d3a456426655440000", PHP_UUID_HEX_LEN) == 0);
 * 
 * php_uuid_string str;
 * php_uuid_to_string(&str, uuid);
 * assert(memcmp(&str, "123e4567-e89b-12d3-a456-426655440000", PHP_UUID_STRING_LEN) == 0);
 * ```
 *
 * @see https://en.wikipedia.org/wiki/Universally_unique_identifier
 * @see https://tools.ietf.org/html/rfc4122
 */

#ifndef PHP_UUID_H
#define PHP_UUID_H

#include "zend_types.h"
#include "php.h"

/** UUID class entry for usage by other modules. */
PHPAPI extern zend_class_entry *php_ce_UUID;

/** UUIDParseException class entry for usage by other modules. */
PHPAPI extern zend_class_entry *php_ce_UUIDParseException;

/** UUID binary representation length without terminating NUL. */
#define PHP_UUID_LEN 16

/** UUID binary representation to store the 128 bit number in 16 bytes. */
PHPAPI typedef struct php_uuid {
	uint8_t bytes[PHP_UUID_LEN];
} php_uuid;

/** UUID hexadecimal representation length with terminating NUL. */
#define PHP_UUID_HEX_LEN 33

/** UUID hexadecimal representation: `000000001111222233334444444444444444\0` */
PHPAPI typedef struct php_uuid_hex {
	char str[PHP_UUID_HEX_LEN];
} php_uuid_hex;

/** UUID string representation length with terminating NUL. */
#define PHP_UUID_STRING_LEN 37

/** UUID string representation: `00000000-1111-2222-3333-4444444444444444\0` */
PHPAPI typedef struct php_uuid_string {
	char str[PHP_UUID_STRING_LEN];
} php_uuid_string;

/**
 * Domain Name System (DNS) namespace UUID.
 *
 * @see https://tools.ietf.org/html/rfc4122#appendix-C
 * @see https://en.wikipedia.org/wiki/Domain_Name_System
 */
static const php_uuid PHP_UUID_NAMESPACE_DNS = { "\x6b\xa7\xb8\x10\x9d\xad\x11\xd1\x80\xb4\x00\xc0\x4f\xd4\x30\xc8" };

/**
* Object Identifier (OID) namespace UUID.
*
 * @see https://tools.ietf.org/html/rfc4122#appendix-C
 * @see https://en.wikipedia.org/wiki/Object_identifier
 */
static const php_uuid PHP_UUID_NAMESPACE_OID = { "\x6b\xa7\xb8\x12\x9d\xad\x11\xd1\x80\xb4\x00\xc0\x4f\xd4\x30\xc8" };

/**
 * Uniform Resource Locator (URL) namespace UUID.
 *
 * @see https://tools.ietf.org/html/rfc4122#appendix-C
 * @see https://en.wikipedia.org/wiki/URL
 */
static const php_uuid PHP_UUID_NAMESPACE_URL = { "\x6b\xa7\xb8\x11\x9d\xad\x11\xd1\x80\xb4\x00\xc0\x4f\xd4\x30\xc8" };

/**
 * X.500 Distinguished Names (X.500 DN) namespace UUID. The names that are to
 * be hashed in this namespace can be in DER or a text output format.
 *
 * @see https://tools.ietf.org/html/rfc4122#appendix-C
 * @see https://en.wikipedia.org/wiki/X.500
 * @see https://en.wikipedia.org/wiki/Lightweight_Directory_Access_Protocol
 */
static const php_uuid PHP_UUID_NAMESPACE_X500 = { "\x6b\xa7\xb8\x14\x9d\xad\x11\xd1\x80\xb4\x00\xc0\x4f\xd4\x30\xc8" };

/**
 * Special nil UUID that has all 128 bits set to zero.
 *
 * @see https://tools.ietf.org/html/rfc4122#section-4.1.7
 * @see https://en.wikipedia.org/wiki/Universally_unique_identifier#Nil_UUID
 */
static const php_uuid PHP_UUID_NIL = { "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" };

/**
 * UUID variants as defined in RFC 4122.
 *
 * @see https://tools.ietf.org/html/rfc4122#section-4.1.1
 */
PHPAPI typedef enum php_uuid_variant {
	PHP_UUID_VARIANT_NCS             = 0, /* 0b0xx */
	PHP_UUID_VARIANT_RFC4122         = 1, /* 0b10x */
	PHP_UUID_VARIANT_MICROSOFT       = 2, /* 0b110 */
	PHP_UUID_VARIANT_FUTURE_RESERVED = 3, /* 0b111 */
} php_uuid_variant;

/**
 * Version code for date-time and IEEE 802 MAC address UUIDs.
 *
 * Generation of this version is not supported by this implementation due
 * to security concerns. Version 4 UUIDs are a good replacement for version
 * 1 UUIDs without the privacy/security concerns (see Wikipedia).
 *
 * @see https://tools.ietf.org/html/rfc4122#section-4.2
 * @see https://en.wikipedia.org/wiki/Universally_unique_identifier#Version_1_.28date-time_and_MAC_address.29
 */
static const uint8_t PHP_UUID_VERSION_1_TIME_BASED = 1;

/**
 * Version code for date-time and IEEE 802 MAC address UUIDs (DCE security
 * algorithm).
 *
 * Generation of this version is not supported by this implementation due
 * to security concerns, and uniqueness limitations for applications with
 * high allocations. Version 4 UUIDs are a good replacement for version 2
 * UUIDs without the privacy/security concerns (see Wikipedia), and they
 * support high allocations.
 *
 * @see https://tools.ietf.org/html/rfc4122#section-4.2
 * @see https://en.wikipedia.org/wiki/Universally_unique_identifier#Version_2_.28date-time_and_MAC_Address.2C_DCE_security_version.29
 */
static const uint8_t PHP_UUID_VERSION_2_DCE_SECURITY = 2;

/**
 * Version code for namespace/name-based MD5 hashed UUIDs.
 *
 * @see php_uuid_create_v3
 * @see https://tools.ietf.org/html/rfc4122#section-4.3
 * @see https://en.wikipedia.org/wiki/Universally_unique_identifier#Versions_3_and_5_.28namespace_name-based.29
 */
static const uint8_t PHP_UUID_VERSION_3_NAME_BASED_MD5 = 3;

/**
 * Version code for random UUIDs.
 *
 * @see php_uuid_create_v4
 * @see https://tools.ietf.org/html/rfc4122#section-4.4
 * @see https://en.wikipedia.org/wiki/Universally_unique_identifier#Version_4_.28random.29
 */
static const uint8_t PHP_UUID_VERSION_4_RANDOM = 4;

/**
 * Version code for namespace/name-based SHA1 hashed UUIDs.
 *
 * @see php_uuid_create_v5
 * @see https://tools.ietf.org/html/rfc4122#section-4.3
 * @see https://en.wikipedia.org/wiki/Universally_unique_identifier#Versions_3_and_5_.28namespace_name-based.29
 */
static const uint8_t PHP_UUID_VERSION_5_NAME_BASED_SHA1 = 5;

/**
 * Parse the given string as UUID.
 *
 * The following UUID representations are parsable:
 *
 * - hexadecimal (`00000000111122223333444444444444`),
 * - string (`00000000-1111-2222-3333-444444444444`),
 * - URNs (`urn:uuid:00000000-1111-2222-3333-444444444444`), and
 * - Microsoft (`{00000000-1111-2222-3333-444444444444}`).
 *
 * Leading and trailing whitespace, namely spaces (` `) and tabs (`\t`), is
 * ignored, so are leading opening braces (`{`) and trailing closing braces
 * (`}`). Hyphens (`-`) are ignored everywhere. The parsing algorithm
 * follows the [robustness
 * principle](https://en.wikipedia.org/wiki/Robustness_principle) and is
 * not meant for validation.
 *
 * ## Examples
 * ```
 * php_uuid uuid;
 *
 * // Parsing of canonical representations.
 * php_uuid_parse_silent(&uuid, "0123456789abcdef0123456789abcdef", sizeof("0123456789abcdef0123456789abcdef") - 1);
 * php_uuid_parse_silent(&uuid, "01234567-89ab-cdef-0123-456789abcdef", sizeof("01234567-89ab-cdef-0123-456789abcdef") - 1);
 * php_uuid_parse_silent(&uuid, "urn:uuid:01234567-89ab-cdef-0123-456789abcdef", sizeof("urn:uuid:01234567-89ab-cdef-0123-456789abcdef") - 1);
 * php_uuid_parse_silent(&uuid, "{01234567-89ab-cdef-0123-456789abcdef}", sizeof("{01234567-89ab-cdef-0123-456789abcdef}") - 1);
 *
 * // Leading and trailing garbage is ignored, so are extraneous hyphens.
 * php_uuid_parse_silent(&uuid, " \t ---- { urn:uuid:----0123-4567-89ab-cdef-0123-4567-89ab-cdef---- } ---- \t ", sizeof(" \t ---- { urn:uuid:----0123-4567-89ab-cdef-0123-4567-89ab-cdef---- } ---- \t ") - 1);
 *
 * // However, note that there cannot be whitespace or braces between the URN
 * // scheme and the UUID itself.
 * assert(php_uuid_parse_silent(
 *     &uuid,
 *     "urn:uuid:{01234567-89ab-cdef-0123-456789abcdef",
 *     sizeof("urn:uuid:{01234567-89ab-cdef-0123-456789abcdef") - 1
 * ) == FAILURE);
 * ```
 *
 * @see php_uuid_parse_silent
 * @see php_uuid_parse_throw
 * @param[out] uuid to store the result in.
 * @param[in] input to parse as UUID.
 * @param[in] input_len
 * @param[in] throw whether to throw PHP exceptions (`1`), or not (`0`).
 * @return `SUCCESS` if the string was parsed as UUID, `FAILURE` otherwise.
 * @throws UUIDParseException if throw is enabled and parsing fails.
 */
PHPAPI int php_uuid_parse(php_uuid *uuid, const char *input, const size_t input_len, const zend_bool throw);

/**
 * Silently parse the string as UUID.
 *
 * @see php_uuid_parse
 * @param[out] uuid to store the result in.
 * @param[in] input to parse as UUID.
 * @param[in] input_len
 * @return `SUCCESS` if the string was parsed as UUID, `FAILURE` otherwise.
 */
#define php_uuid_parse_silent(uuid, input, input_len) php_uuid_parse(uuid, input, input_len, 0)

/**
 * Parse the string as UUID and throw PHP exceptions on failures.
 *
 * @see php_uuid_parse
 * @param[out] uuid to store the result in.
 * @param[in] input to parse as UUID.
 * @param[in] input_len
 * @return `SUCCESS` if the string was parsed as UUID, `FAILURE` otherwise.
 * @throws UUIDParseException if throw is enabled and parsing fails.
 */
#define php_uuid_parse_throw(uuid, input, input_len) php_uuid_parse(uuid, input, input_len, 1)

/**
 * Create version 3 UUID.
 *
 * > RFC 4122 recommends {@see v5} over this one and states that version 3
 * > UUIDs should be used if backwards compatibility is required only. This
 * > is because MD5 has a higher collision probability compared to SHA1,
 * > which is used by version 5; regardless of the truncation!
 *
 * Version 3 UUIDs are generated by MD5 hashing the concatenated namespace's
 * byte representation and the given name. The namespace itself must be
 * another UUID. This can be any UUID, or one of the predefined ones:
 *
 * - {@see PHP_UUID_NAMESPACE_DNS}
 * - {@see PHP_UUID_NAMESPACE_OID}
 * - {@see PHP_UUID_NAMESPACE_URL}
 * - {@see PHP_UUID_NAMESPACE_X500}
 *
 * A particular name within the same namespace always results in the same
 * version 3 UUID, across all RFC 4122 compliant UUID implementations.
 * However, the namespace and name cannot be determined from the UUID alone.
 *
 * ## Examples
 * ```
 * php_uuid uuid;
 *
 * php_uuid_create_v3(&uuid, PHP_UUID_NAMESPACE_DNS, "php.net", sizeof("php.net") - 1);
 *
 * assert(php_uuid_is_nil(uuid)      == FALSE);
 * assert(php_uuid_get_variant(uuid) == PHP_UUID_VARIANT_RFC4122);
 * assert(php_uuid_get_version(uuid) == PHP_UUID_VERSION_3_NAME_BASED_MD5);
 *
 * php_uuid_string str;
 * php_uuid_to_string(&str, uuid);
 * assert(memcmp(&str, "11a38b9a-b3da-360f-9353-a5a725514269", PHP_UUID_STRING_LEN) == 0);
 *
 * php_uuid tmp;
 * php_uuid_create_v3(&tmp, PHP_UUID_NAMESPACE_DNS, "php.net", sizeof("php.net") - 1);
 * assert(memcmp(&tmp, &uuid, PHP_UUID_LEN) == 0);
 * ```
 *
 * @see https://tools.ietf.org/html/rfc4122#section-4.3
 * @see https://en.wikipedia.org/wiki/Universally_unique_identifier#Versions_3_and_5_.28namespace_name-based.29
 * @param[out] uuid to store the result in.
 * @param[in] namespace to create the UUID in.
 * @param[in] name to create the UUID from.
 * @param[in] name_len
 */
PHPAPI void php_uuid_create_v3(php_uuid *uuid, const php_uuid *namespace, const char *name, const size_t name_len);

/**
 * Create version 4 UUID.
 *
 * Version 4 UUIDs are randomly generated from the best available random source.
 * The selection of that source is determined by PHP's random implementation.
 * Some systems may be bad at generating sufficient entropy, e.g. virtual
 * machines. This might lead to collisions faster than desired. If this is the
 * case, the {@see php_uuid_create_v5} version should be used.
 *
 * ## Examples
 * ```
 * php_uuid uuid;
 *
 * php_uuid_create_v4_silent(&uuid);
 *
 * assert(php_uuid_is_nil(uuid)      == FALSE);
 * assert(php_uuid_get_variant(uuid) == PHP_UUID_VARIANT_RFC4122);
 * assert(php_uuid_get_version(uuid) == PHP_UUID_VERSION_4_RANDOM);
 *
 * php_uuid tmp;
 * php_uuid_create_v4_silent(&uuid);
 * assert(memcmp(&tmp, &uuid, PHP_UUID_LEN) == 0);
 * ```
 *
 * @see https://tools.ietf.org/html/rfc4122#section-4.4
 * @see https://en.wikipedia.org/wiki/Universally_unique_identifier#Version_4_.28random.29
 * @see php_uuid_create_v4_silent
 * @see php_uuid_create_v4_throw
 * @param[out] uuid to store the result.
 * @param[in] throw whether to throw a PHP exception (`1`), or not (`0`).
 * @return `SUCCESS` if the generation succeeded, `FAILURE` if it was not
 *     possible to gather sufficient entropy.
 * @throws Exception if throw is enabled and it was not possible to gather
 *     sufficient entropy for generating random bytes.
 */
PHPAPI int php_uuid_create_v4(php_uuid *uuid, const zend_bool throw);

/**
 * Silently create version 4 UUID.
 *
 * @see php_uuid_create_v4
 * @param[out] uuid to store the result.
 * @return `SUCCESS` if the generation succeeded, `FAILURE` if it was not
 *     possible to gather sufficient entropy.
 */
#define php_uuid_create_v4_silent(uuid) php_uuid_create_v4(uuid, 0)

/**
 * Create version 4 UUID and throw PHP exception if it is not possible to
 * gather sufficient entropy.
 *
 * @see php_uuid_create_v4
 * @param[out] uuid to store the result.
 * @return `SUCCESS` if the generation succeeded, `FAILURE` if it was not
 *     possible to gather sufficient entropy.
 * @throws Exception if throw is enabled and it was not possible to gather
 *     sufficient entropy for generating random bytes.
 */
#define php_uuid_create_v4_throw(uuid) php_uuid_create_v4(uuid, 1)

/**
 * Create version 5 UUID.
 *
 * Version 5 UUIDs are generated by MD5 hashing the concatenated namespace's
 * byte representation and the given name. The namespace itself must be
 * another UUID. This can be any UUID, or one of the predefined ones:
 *
 * - {@see PHP_UUID_NAMESPACE_DNS}
 * - {@see PHP_UUID_NAMESPACE_OID}
 * - {@see PHP_UUID_NAMESPACE_URL}
 * - {@see PHP_UUID_NAMESPACE_X500}
 *
 * A particular name within the same namespace always results in the same
 * version 5 UUID, across all RFC 4122 compliant UUID implementations.
 * However, the namespace and name cannot be determined from the UUID alone.
 *
 * ## Examples
 * ```
 * php_uuid uuid;
 *
 * php_uuid_create_v5(&uuid, PHP_UUID_NAMESPACE_DNS, "php.net", sizeof("php.net") - 1);
 *
 * assert(php_uuid_is_nil(uuid)      == FALSE);
 * assert(php_uuid_get_variant(uuid) == PHP_UUID_VARIANT_RFC4122);
 * assert(php_uuid_get_version(uuid) == PHP_UUID_VERSION_5_NAME_BASED_SHA1);
 *
 * php_uuid_string str;
 * php_uuid_to_string(&str, uuid);
 * assert(memcmp(&str, "c4a760a8-dbcf-5254-a0d9-6a4474bd1b62", PHP_UUID_STRING_LEN) == 0);
 *
 * php_uuid tmp;
 * php_uuid_create_v5(&tmp, PHP_UUID_NAMESPACE_DNS, "php.net", sizeof("php.net") - 1);
 * assert(memcmp(&tmp, &uuid, PHP_UUID_LEN) == 0);
 * ```
 *
 * @see https://tools.ietf.org/html/rfc4122#section-4.3
 * @see https://en.wikipedia.org/wiki/Universally_unique_identifier#Versions_3_and_5_.28namespace_name-based.29
 * @param[out] uuid to store the result in.
 * @param[in] namespace to create the UUID in.
 * @param[in] name to create the UUID from.
 * @param[in] name_len
 */
PHPAPI void php_uuid_create_v5(php_uuid *uuid, const php_uuid *namespace, const char *name, const size_t name_len);

/**
 * Get the variant associated with this UUID.
 *
 * The variant specifies the internal data layout of a UUID. This
 * implementation generates {@see UUID::VARIANT_RFC4122} UUIDs only,
 * however, parsing and construction of other variants is supported.
 *
 * @see http://tools.ietf.org/html/rfc4122#section-4.1.1
 * @see https://en.wikipedia.org/wiki/Universally_unique_identifier#Variants
 * @see PHP_UUID_VARIANT_NCS
 * @see PHP_UUID_VARIANT_RFC4122
 * @see PHP_UUID_VARIANT_MICROSOFT
 * @see PHP_UUID_VARIANT_FUTURE_RESERVED
 * @param[in] uuid to get the variant from.
 * @return The variant of the given UUID.
 */
static zend_always_inline const php_uuid_variant php_uuid_get_variant(const php_uuid *uuid)
{
	if ((uuid->bytes[8] & 0xC0) == 0x80) return PHP_UUID_VARIANT_RFC4122;
	if ((uuid->bytes[8] & 0xE0) == 0xC0) return PHP_UUID_VARIANT_MICROSOFT;
	if ((uuid->bytes[8] & 0x80) == 0x00) return PHP_UUID_VARIANT_NCS;
	return PHP_UUID_VARIANT_FUTURE_RESERVED;
}

/**
 * Get the version associated with this UUID.
 *
 * The version specifies which algorithm was used to generate the UUID. Note
 * that the version might not be meaningful if another variant than the
 * {@see PHP_UUID_VARIANT_RFC4122} was used to generate the UUID. This
 * implementation generates {@see PHP_UUID_VARIANT_RFC4122} UUIDs only, but
 * allows parsing and construction of other variants.
 *
 * @see http://tools.ietf.org/html/rfc4122#section-4.1.3
 * @see https://en.wikipedia.org/wiki/Universally_unique_identifier#Versions
 * @see PHP_UUID_VERSION_1_TIME_BASED
 * @see PHP_UUID_VERSION_2_DCE_SECURITY
 * @see PHP_UUID_VERSION_3_NAME_BASED_MD5
 * @see PHP_UUID_VERSION_4_RANDOM
 * @see PHP_UUID_VERSION_5_NAME_BASED_SHA1
 * @param[in] uuid to get the version from.
 * @return The version of the given UUID, which is an integer in [0, 15], the
 *     values [1, 5] correspond to one of the `PHP_UUID_VERSION_*` constants.
 *     The others are not defined in RFC 4122.
 */
static zend_always_inline const uint8_t php_uuid_get_version(const php_uuid *uuid)
{
	return uuid->bytes[6] >> 4;
}

/**
 * Check if the given UUID is the special nil UUID that has all 128 bits set
 * to zero.
 *
 * @see https://tools.ietf.org/html/rfc4122#section-4.1.7
 * @see https://en.wikipedia.org/wiki/Universally_unique_identifier#Nil_UUID
 * @param[in] uuid to check.
 * @return TRUE if the UUID contains the special nil UUID; FALSE otherwise.
 */
static zend_always_inline const int php_uuid_is_nil(const php_uuid *uuid)
{
	return memcmp(uuid->bytes, &PHP_UUID_NIL, PHP_UUID_LEN) == 0;
}

/**
 * Convert the UUID to its hexadecimal representation.
 *
 * The hexadecimal representation of a UUID are 32 hexadecimal digits. The
 * hexadecimal digits `a` through `f` are always formatted as lower case
 * characters, in accordance with RFC 4122.
 *
 * ## Examples
 * ```
 * php_uuid_hex hex;
 *
 * php_uuid_to_hex(&hex, PHP_UUID_NAMESPACE_DNS);
 * assert(memcmp(&hex, "6ba7b8109dad11d180b400c04fd430c8", PHP_UUID_HEX_LEN) == 0);
 * ```
 *
 * @param[out] buffer to store the hexadecimal representation in.
 * @param[in] uuid to convert.
 */
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

/**
 * Convert the UUID to its string representation.
 *
 * The string representation of a UUID are 32 hexadecimal digits separated
 * by a hyphen into five groups of 8, 4, 4, 4, and 12 digits. The
 * hexadecimal digits `a` through `f` are always formatted as lower case
 * characters, in accordance with RFC 4122.
 *
 * ## Examples
 * ```
 * php_uuid_string str;
 *
 * php_uuid_to_hex(&str, PHP_UUID_NAMESPACE_DNS);
 * assert(memcmp(&str, "6ba7b810-9dad-11d1-80b4-00c04fd430c8", PHP_UUID_STRING_LEN) == 0);
 * ```
 *
 * @param[out] buffer to store the string representation in.
 * @param[in] uuid to convert.
 */
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

/**
 * Initialization function of the standard UUID submodule.
 *
 * @see ext/standard/basic_functions.c
 * @see ext/standard/php_standard.h
 */
PHP_MINIT_FUNCTION(uuid);

#endif /* PHP_UUID_H */
