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

#include "php.h"
#include "uri_parser_rfc3986.h"
#include "php_uri_common.h"
#include "Zend/zend_smart_str.h"
#include "Zend/zend_exceptions.h"

#include <uriparser/Uri.h>

struct php_uri_parser_rfc3986_uris {
	UriUriA uri;
	UriUriA normalized_uri;
	bool normalized_uri_initialized;
};

static void *php_uri_parser_rfc3986_memory_manager_malloc(UriMemoryManager *memory_manager, size_t size)
{
	return emalloc(size);
}

static void *php_uri_parser_rfc3986_memory_manager_calloc(UriMemoryManager *memory_manager, size_t nmemb, size_t size)
{
	return ecalloc(nmemb, size);
}

static void *php_uri_parser_rfc3986_memory_manager_realloc(UriMemoryManager *memory_manager, void *ptr, size_t size)
{
	return erealloc(ptr, size);
}

static void *php_uri_parser_rfc3986_memory_manager_reallocarray(UriMemoryManager *memory_manager, void *ptr, size_t nmemb, size_t size)
{
	return safe_erealloc(ptr, nmemb, size, 0);
}

static void php_uri_parser_rfc3986_memory_manager_destroy(UriMemoryManager *memory_manager, void *ptr)
{
	efree(ptr);
}

static const UriMemoryManager php_uri_parser_rfc3986_memory_manager = {
	.malloc = php_uri_parser_rfc3986_memory_manager_malloc,
	.calloc = php_uri_parser_rfc3986_memory_manager_calloc,
	.realloc = php_uri_parser_rfc3986_memory_manager_realloc,
	.reallocarray = php_uri_parser_rfc3986_memory_manager_reallocarray,
	.free = php_uri_parser_rfc3986_memory_manager_destroy,
	.userData = NULL,
};

/* The library expects a pointer to a non-const UriMemoryManager, but does
 * not actually modify it (and neither does our implementation). Use a
 * const struct with a non-const pointer for convenience. */
static UriMemoryManager* const mm = (UriMemoryManager*)&php_uri_parser_rfc3986_memory_manager;

static inline size_t get_text_range_length(const UriTextRangeA *range)
{
	return range->afterLast - range->first;
}

static inline bool has_text_range(const UriTextRangeA *range)
{
	return range->first != NULL && range->afterLast != NULL;
}

ZEND_ATTRIBUTE_NONNULL static void copy_uri(UriUriA *new_uriparser_uri, const UriUriA *uriparser_uri)
{
	int result = uriCopyUriMmA(new_uriparser_uri, uriparser_uri, mm);
	ZEND_ASSERT(result == URI_SUCCESS);
}

ZEND_ATTRIBUTE_NONNULL static UriUriA *get_normalized_uri(php_uri_parser_rfc3986_uris *uriparser_uris) {
	if (!uriparser_uris->normalized_uri_initialized) {
		copy_uri(&uriparser_uris->normalized_uri, &uriparser_uris->uri);
		int result = uriNormalizeSyntaxExMmA(&uriparser_uris->normalized_uri, (unsigned int)-1, mm);
		ZEND_ASSERT(result == URI_SUCCESS);
		uriparser_uris->normalized_uri_initialized = true;
	}

	return &uriparser_uris->normalized_uri;
}

ZEND_ATTRIBUTE_NONNULL static UriUriA *get_uri_for_reading(php_uri_parser_rfc3986_uris *uriparser_uris, php_uri_component_read_mode read_mode)
{
	switch (read_mode) {
		case PHP_URI_COMPONENT_READ_MODE_RAW:
			return &uriparser_uris->uri;
		case PHP_URI_COMPONENT_READ_MODE_NORMALIZED_ASCII:
			ZEND_FALLTHROUGH;
		case PHP_URI_COMPONENT_READ_MODE_NORMALIZED_UNICODE:
			return get_normalized_uri(uriparser_uris);
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}

ZEND_ATTRIBUTE_NONNULL static UriUriA *get_uri_for_writing(php_uri_parser_rfc3986_uris *uriparser_uris)
{
	return &uriparser_uris->uri;
}

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_parser_rfc3986_scheme_read(void *uri, php_uri_component_read_mode read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(uri, read_mode);

	if (has_text_range(&uriparser_uri->scheme)) {
		ZVAL_STRINGL(retval, uriparser_uri->scheme.first, get_text_range_length(&uriparser_uri->scheme));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_rfc3986_scheme_write(void *uri, zval *value, zval *errors)
{
	UriUriA *uriparser_uri = get_uri_for_writing(uri);
	int result;

	if (Z_TYPE_P(value) == IS_NULL) {
		result = uriSetSchemeMmA(uriparser_uri, NULL, NULL, mm);
	} else {
		result = uriSetSchemeMmA(uriparser_uri, Z_STRVAL_P(value), Z_STRVAL_P(value) + Z_STRLEN_P(value), mm);
	}

	switch (result) {
		case URI_SUCCESS:
			return SUCCESS;
		case URI_ERROR_SYNTAX:
			zend_throw_exception(php_uri_ce_invalid_uri_exception, "The specified scheme is malformed", 0);
			return FAILURE;
		default:
			/* This should be unreachable in practice. */
			zend_throw_exception(php_uri_ce_error, "Failed to update the scheme", 0);
			return FAILURE;
	}
}

ZEND_ATTRIBUTE_NONNULL zend_result php_uri_parser_rfc3986_userinfo_read(void *uri, php_uri_component_read_mode read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(uri, read_mode);

	if (has_text_range(&uriparser_uri->userInfo)) {
		ZVAL_STRINGL(retval, uriparser_uri->userInfo.first, get_text_range_length(&uriparser_uri->userInfo));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

zend_result php_uri_parser_rfc3986_userinfo_write(void *uri, zval *value, zval *errors)
{
	UriUriA *uriparser_uri = get_uri_for_writing(uri);
	int result;

	if (Z_TYPE_P(value) == IS_NULL) {
		result = uriSetUserInfoMmA(uriparser_uri, NULL, NULL, mm);
	} else {
		result = uriSetUserInfoMmA(uriparser_uri, Z_STRVAL_P(value), Z_STRVAL_P(value) + Z_STRLEN_P(value), mm);
	}

	switch (result) {
		case URI_SUCCESS:
			return SUCCESS;
		case URI_ERROR_SETUSERINFO_HOST_NOT_SET:
			zend_throw_exception(php_uri_ce_invalid_uri_exception, "Cannot set a userinfo without having a host", 0);
			return FAILURE;
		case URI_ERROR_SYNTAX:
			zend_throw_exception(php_uri_ce_invalid_uri_exception, "The specified userinfo is malformed", 0);
			return FAILURE;
		default:
			/* This should be unreachable in practice. */
			zend_throw_exception(php_uri_ce_error, "Failed to update the userinfo", 0);
			return FAILURE;
	}
}

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_parser_rfc3986_username_read(void *uri, php_uri_component_read_mode read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(uri, read_mode);

	if (has_text_range(&uriparser_uri->userInfo)) {
		size_t length = get_text_range_length(&uriparser_uri->userInfo);
		const char *c = memchr(uriparser_uri->userInfo.first, ':', length);

		if (c == NULL && length > 0) {
			ZVAL_STRINGL(retval, uriparser_uri->userInfo.first, length);
		} else if (c != NULL && c - uriparser_uri->userInfo.first > 0) {
			ZVAL_STRINGL(retval, uriparser_uri->userInfo.first, c - uriparser_uri->userInfo.first);
		} else {
			ZVAL_EMPTY_STRING(retval);
		}
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_parser_rfc3986_password_read(void *uri, php_uri_component_read_mode read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(uri, read_mode);

	if (has_text_range(&uriparser_uri->userInfo)) {
		const char *c = memchr(uriparser_uri->userInfo.first, ':', get_text_range_length(&uriparser_uri->userInfo));

		if (c != NULL && uriparser_uri->userInfo.afterLast - c - 1 > 0) {
			ZVAL_STRINGL(retval, c + 1, uriparser_uri->userInfo.afterLast - c - 1);
		} else {
			ZVAL_EMPTY_STRING(retval);
		}
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_parser_rfc3986_host_read(void *uri, php_uri_component_read_mode read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(uri, read_mode);

	if (has_text_range(&uriparser_uri->hostText)) {
		if (uriparser_uri->hostData.ip6 != NULL || uriparser_uri->hostData.ipFuture.first != NULL) {
			/* the textual representation of the host is always accessible in the .hostText field no matter what the host is */
			smart_str host_str = {0};

			smart_str_appendc(&host_str, '[');
			smart_str_appendl(&host_str, uriparser_uri->hostText.first, get_text_range_length(&uriparser_uri->hostText));
			smart_str_appendc(&host_str, ']');

			ZVAL_NEW_STR(retval, smart_str_extract(&host_str));
		} else {
			ZVAL_STRINGL(retval, uriparser_uri->hostText.first, get_text_range_length(&uriparser_uri->hostText));
		}
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_rfc3986_host_write(void *uri, zval *value, zval *errors)
{
	UriUriA *uriparser_uri = get_uri_for_writing(uri);
	int result;

	if (Z_TYPE_P(value) == IS_NULL) {
		result = uriSetHostAutoMmA(uriparser_uri, NULL, NULL, mm);
	} else {
		result = uriSetHostAutoMmA(uriparser_uri, Z_STRVAL_P(value), Z_STRVAL_P(value) + Z_STRLEN_P(value), mm);
	}

	switch (result) {
		case URI_SUCCESS:
			return SUCCESS;
		case URI_ERROR_SETHOST_PORT_SET:
			zend_throw_exception(php_uri_ce_invalid_uri_exception, "Cannot remove the host from a URI that has a port", 0);
			return FAILURE;
		case URI_ERROR_SETHOST_USERINFO_SET:
			zend_throw_exception(php_uri_ce_invalid_uri_exception, "Cannot remove the host from a URI that has a userinfo", 0);
			return FAILURE;
		case URI_ERROR_SYNTAX:
			zend_throw_exception(php_uri_ce_invalid_uri_exception, "The specified host is malformed", 0);
			return FAILURE;
		default:
			/* This should be unreachable in practice. */
			zend_throw_exception(php_uri_ce_error, "Failed to update the host", 0);
			return FAILURE;
	}
}

ZEND_ATTRIBUTE_NONNULL static zend_long port_str_to_zend_long_checked(const char *str, size_t len)
{
	if (len > MAX_LENGTH_OF_LONG) {
		return -1;
	}

	char buf[MAX_LENGTH_OF_LONG + 1];
	*(char*)zend_mempcpy(buf, str, len) = 0;

	zend_ulong result = ZEND_STRTOUL(buf, NULL, 10);

	if (result > ZEND_LONG_MAX) {
		return -1;
	}

	return (zend_long)result;
}

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_parser_rfc3986_port_read(void *uri, php_uri_component_read_mode read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(uri, read_mode);

	if (has_text_range(&uriparser_uri->portText) && get_text_range_length(&uriparser_uri->portText) > 0) {
		ZVAL_LONG(retval, port_str_to_zend_long_checked(uriparser_uri->portText.first, get_text_range_length(&uriparser_uri->portText)));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_rfc3986_port_write(void *uri, zval *value, zval *errors)
{
	UriUriA *uriparser_uri = get_uri_for_writing(uri);
	int result;

	if (Z_TYPE_P(value) == IS_NULL) {
		result = uriSetPortTextMmA(uriparser_uri, NULL, NULL, mm);
	} else {
		zend_string *tmp = zend_long_to_str(Z_LVAL_P(value));
		result = uriSetPortTextMmA(uriparser_uri, ZSTR_VAL(tmp), ZSTR_VAL(tmp) + ZSTR_LEN(tmp), mm);
		zend_string_release_ex(tmp, false);
	}

	switch (result) {
		case URI_SUCCESS:
			return SUCCESS;
		case URI_ERROR_SETPORT_HOST_NOT_SET:
			zend_throw_exception(php_uri_ce_invalid_uri_exception, "Cannot set a port without having a host", 0);
			return FAILURE;
		case URI_ERROR_SYNTAX:
			zend_throw_exception(php_uri_ce_invalid_uri_exception, "The specified port is malformed", 0);
			return FAILURE;
		default:
			/* This should be unreachable in practice. */
			zend_throw_exception(php_uri_ce_error, "Failed to update the port", 0);
			return FAILURE;
	}
}

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_parser_rfc3986_path_read(void *uri, php_uri_component_read_mode read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(uri, read_mode);

	if (uriparser_uri->pathHead != NULL) {
		smart_str str = {0};

		if (uriparser_uri->absolutePath || uriHasHostA(uriparser_uri)) {
			smart_str_appendc(&str, '/');
		}

		for (const UriPathSegmentA *p = uriparser_uri->pathHead; p; p = p->next) {
			smart_str_appendl(&str, p->text.first, get_text_range_length(&p->text));
			if (p->next) {
				smart_str_appendc(&str, '/');
			}
		}

		ZVAL_NEW_STR(retval, smart_str_extract(&str));
	} else if (uriparser_uri->absolutePath) {
		ZVAL_CHAR(retval, '/');
	} else {
		ZVAL_EMPTY_STRING(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_rfc3986_path_write(void *uri, zval *value, zval *errors)
{
	UriUriA *uriparser_uri = get_uri_for_writing(uri);
	int result;

	if (Z_STRLEN_P(value) == 0) {
		result = uriSetPathMmA(uriparser_uri, NULL, NULL, mm);
	} else {
		result = uriSetPathMmA(uriparser_uri, Z_STRVAL_P(value), Z_STRVAL_P(value) + Z_STRLEN_P(value), mm);
	}

	switch (result) {
		case URI_SUCCESS:
			return SUCCESS;
		case URI_ERROR_SYNTAX:
			zend_throw_exception(php_uri_ce_invalid_uri_exception, "The specified path is malformed", 0);
			return FAILURE;
		default:
			/* This should be unreachable in practice. */
			zend_throw_exception(php_uri_ce_error, "Failed to update the path", 0);
			return FAILURE;
	}
}

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_parser_rfc3986_query_read(void *uri, php_uri_component_read_mode read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(uri, read_mode);

	if (has_text_range(&uriparser_uri->query)) {
		ZVAL_STRINGL(retval, uriparser_uri->query.first, get_text_range_length(&uriparser_uri->query));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_rfc3986_query_write(void *uri, zval *value, zval *errors)
{
	UriUriA *uriparser_uri = get_uri_for_writing(uri);
	int result;

	if (Z_TYPE_P(value) == IS_NULL) {
		result = uriSetQueryMmA(uriparser_uri, NULL, NULL, mm);
	} else {
		result = uriSetQueryMmA(uriparser_uri, Z_STRVAL_P(value), Z_STRVAL_P(value) + Z_STRLEN_P(value), mm);
	}

	switch (result) {
		case URI_SUCCESS:
			return SUCCESS;
		case URI_ERROR_SYNTAX:
			zend_throw_exception(php_uri_ce_invalid_uri_exception, "The specified query is malformed", 0);
			return FAILURE;
		default:
			/* This should be unreachable in practice. */
			zend_throw_exception(php_uri_ce_error, "Failed to update the query", 0);
			return FAILURE;
	}
}

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_parser_rfc3986_fragment_read(void *uri, php_uri_component_read_mode read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(uri, read_mode);

	if (has_text_range(&uriparser_uri->fragment)) {
		ZVAL_STRINGL(retval, uriparser_uri->fragment.first, get_text_range_length(&uriparser_uri->fragment));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_rfc3986_fragment_write(void *uri, zval *value, zval *errors)
{
	UriUriA *uriparser_uri = get_uri_for_writing(uri);
	int result;

	if (Z_TYPE_P(value) == IS_NULL) {
		result = uriSetFragmentMmA(uriparser_uri, NULL, NULL, mm);
	} else {
		result = uriSetFragmentMmA(uriparser_uri, Z_STRVAL_P(value), Z_STRVAL_P(value) + Z_STRLEN_P(value), mm);
	}

	switch (result) {
		case URI_SUCCESS:
			return SUCCESS;
		case URI_ERROR_SYNTAX:
			zend_throw_exception(php_uri_ce_invalid_uri_exception, "The specified fragment is malformed", 0);
			return FAILURE;
		default:
			/* This should be unreachable in practice. */
			zend_throw_exception(php_uri_ce_error, "Failed to update the fragment", 0);
			return FAILURE;
	}
}

static php_uri_parser_rfc3986_uris *uriparser_create_uris(void)
{
	php_uri_parser_rfc3986_uris *uriparser_uris = ecalloc(1, sizeof(*uriparser_uris));
	uriparser_uris->normalized_uri_initialized = false;

	return uriparser_uris;
}

php_uri_parser_rfc3986_uris *php_uri_parser_rfc3986_parse_ex(const char *uri_str, size_t uri_str_len, const php_uri_parser_rfc3986_uris *uriparser_base_urls, bool silent)
{
	UriUriA uri = {0};

	/* Parse the URI. */
	int result = uriParseSingleUriExMmA(&uri, uri_str, uri_str + uri_str_len, NULL, mm);
	if (result != URI_SUCCESS) {
		if (!silent) {
			switch (result) {
				case URI_ERROR_SYNTAX:
					zend_throw_exception(php_uri_ce_invalid_uri_exception, "The specified URI is malformed", 0);
					break;
				default:
					/* This should be unreachable in practice. */
					zend_throw_exception(php_uri_ce_error, "Failed to parse the specified URI", 0);
					break;
			}
		}

		goto fail;
	}

	if (uriparser_base_urls != NULL) {
		UriUriA tmp = {0};

		/* Combine the parsed URI with the base URI and store the result in 'tmp',
		 * since the target and source URLs must be distinct. */
		int result = uriAddBaseUriExMmA(&tmp, &uri, &uriparser_base_urls->uri, URI_RESOLVE_STRICTLY, mm);
		if (result != URI_SUCCESS) {
			if (!silent) {
				switch (result) {
					case URI_ERROR_ADDBASE_REL_BASE:
						zend_throw_exception(php_uri_ce_invalid_uri_exception, "The specified base URI must be absolute", 0);
						break;
					default:
						/* This should be unreachable in practice. */
						zend_throw_exception(php_uri_ce_error, "Failed to resolve the specified URI against the base URI", 0);
						break;
				}
			}

			goto fail;
		}

		/* Store the combined URI back into 'uri'. */
		uriFreeUriMembersMmA(&uri, mm);
		uri = tmp;
	}

	/* Make the resulting URI independent of the 'uri_str'. */
	uriMakeOwnerMmA(&uri, mm);

	if (has_text_range(&uri.portText) && get_text_range_length(&uri.portText) > 0) {
		if (port_str_to_zend_long_checked(uri.portText.first, get_text_range_length(&uri.portText)) == -1) {
			if (!silent) {
				zend_throw_exception(php_uri_ce_invalid_uri_exception, "The port is out of range", 0);
			}

			goto fail;
		}
	}

	php_uri_parser_rfc3986_uris *uriparser_uris = uriparser_create_uris();
	uriparser_uris->uri = uri;

	return uriparser_uris;

 fail:

	uriFreeUriMembersMmA(&uri, mm);

	return NULL;
}

void *php_uri_parser_rfc3986_parse(const char *uri_str, size_t uri_str_len, const void *base_url, zval *errors, bool silent)
{
	return php_uri_parser_rfc3986_parse_ex(uri_str, uri_str_len, base_url, silent);
}

ZEND_ATTRIBUTE_NONNULL static void *php_uri_parser_rfc3986_clone(void *uri)
{
	const php_uri_parser_rfc3986_uris *uriparser_uris = uri;

	php_uri_parser_rfc3986_uris *new_uriparser_uris = uriparser_create_uris();
	copy_uri(&new_uriparser_uris->uri, &uriparser_uris->uri);
	/* Do not copy the normalized URI: The expected action after cloning is
	 * modifying the cloned URI (which will invalidate the cached normalized
	 * URI). */

	return new_uriparser_uris;
}

ZEND_ATTRIBUTE_NONNULL static zend_string *php_uri_parser_rfc3986_to_string(void *uri, php_uri_recomposition_mode recomposition_mode, bool exclude_fragment)
{
	php_uri_parser_rfc3986_uris *uriparser_uris = uri;
	const UriUriA *uriparser_uri;

	if (recomposition_mode == PHP_URI_RECOMPOSITION_MODE_RAW_ASCII || recomposition_mode == PHP_URI_RECOMPOSITION_MODE_RAW_UNICODE) {
		uriparser_uri = &uriparser_uris->uri;
	} else {
		uriparser_uri = get_normalized_uri(uriparser_uris);
	}

	int charsRequired = 0;
	int result = uriToStringCharsRequiredA(uriparser_uri, &charsRequired);
	ZEND_ASSERT(result == URI_SUCCESS);

	charsRequired++;

	zend_string *uri_string = zend_string_alloc(charsRequired - 1, false);
	result = uriToStringA(ZSTR_VAL(uri_string), uriparser_uri, charsRequired, NULL);
	ZEND_ASSERT(result == URI_SUCCESS);

	if (exclude_fragment) {
		const char *pos = zend_memrchr(ZSTR_VAL(uri_string), '#', ZSTR_LEN(uri_string));
		if (pos != NULL) {
			uri_string = zend_string_truncate(uri_string, (pos - ZSTR_VAL(uri_string)), false);
		}
	}

	return uri_string;
}

static void php_uri_parser_rfc3986_destroy(void *uri)
{
	php_uri_parser_rfc3986_uris *uriparser_uris = uri;

	if (UNEXPECTED(uriparser_uris == NULL)) {
		return;
	}

	uriFreeUriMembersMmA(&uriparser_uris->uri, mm);
	uriFreeUriMembersMmA(&uriparser_uris->normalized_uri, mm);

	efree(uriparser_uris);
}

PHPAPI const php_uri_parser php_uri_parser_rfc3986 = {
	.name = PHP_URI_PARSER_RFC3986,
	.parse = php_uri_parser_rfc3986_parse,
	.clone = php_uri_parser_rfc3986_clone,
	.to_string = php_uri_parser_rfc3986_to_string,
	.destroy = php_uri_parser_rfc3986_destroy,
	{
		.scheme = {.read = php_uri_parser_rfc3986_scheme_read, .write = php_uri_parser_rfc3986_scheme_write},
		.username = {.read = php_uri_parser_rfc3986_username_read, .write = NULL},
		.password = {.read = php_uri_parser_rfc3986_password_read, .write = NULL},
		.host = {.read = php_uri_parser_rfc3986_host_read, .write = php_uri_parser_rfc3986_host_write},
		.port = {.read = php_uri_parser_rfc3986_port_read, .write = php_uri_parser_rfc3986_port_write},
		.path = {.read = php_uri_parser_rfc3986_path_read, .write = php_uri_parser_rfc3986_path_write},
		.query = {.read = php_uri_parser_rfc3986_query_read, .write = php_uri_parser_rfc3986_query_write},
		.fragment = {.read = php_uri_parser_rfc3986_fragment_read, .write = php_uri_parser_rfc3986_fragment_write},
	}
};
