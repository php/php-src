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

static void php_uri_parser_rfc3986_memory_manager_free(UriMemoryManager *memory_manager, void *ptr)
{
	efree(ptr);
}

static const UriMemoryManager php_uri_parser_rfc3986_memory_manager = {
	.malloc = php_uri_parser_rfc3986_memory_manager_malloc,
	.calloc = php_uri_parser_rfc3986_memory_manager_calloc,
	.realloc = php_uri_parser_rfc3986_memory_manager_realloc,
	.reallocarray = php_uri_parser_rfc3986_memory_manager_reallocarray,
	.free = php_uri_parser_rfc3986_memory_manager_free,
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

ZEND_ATTRIBUTE_NONNULL static UriUriA *get_uri_for_reading(php_uri_parser_rfc3986_uris *uriparser_uris, uri_component_read_mode_t read_mode)
{
	switch (read_mode) {
		case URI_COMPONENT_READ_RAW:
			return &uriparser_uris->uri;
		case URI_COMPONENT_READ_NORMALIZED_ASCII:
			ZEND_FALLTHROUGH;
		case URI_COMPONENT_READ_NORMALIZED_UNICODE:
			return get_normalized_uri(uriparser_uris);
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_parser_rfc3986_scheme_read(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(internal_uri->uri, read_mode);
	ZEND_ASSERT(uriparser_uri != NULL);

	if (uriparser_uri->scheme.first != NULL && uriparser_uri->scheme.afterLast != NULL) {
		zend_string *str = zend_string_init(uriparser_uri->scheme.first, get_text_range_length(&uriparser_uri->scheme), false);
		ZVAL_NEW_STR(retval, str);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL zend_result php_uri_parser_rfc3986_userinfo_read(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(internal_uri->uri, read_mode);
	ZEND_ASSERT(uriparser_uri != NULL);

	if (uriparser_uri->userInfo.first != NULL && uriparser_uri->userInfo.afterLast != NULL) {
		ZVAL_STRINGL(retval, uriparser_uri->userInfo.first, get_text_range_length(&uriparser_uri->userInfo));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_parser_rfc3986_username_read(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(internal_uri->uri, read_mode);
	ZEND_ASSERT(uriparser_uri != NULL);

	if (uriparser_uri->userInfo.first != NULL && uriparser_uri->userInfo.afterLast != NULL) {
		size_t length = get_text_range_length(&uriparser_uri->userInfo);
		const char *c = memchr(uriparser_uri->userInfo.first, ':', length);

		if (c == NULL && length > 0) {
			ZVAL_STRINGL(retval, uriparser_uri->userInfo.first, length);
		} else if (c != NULL && c - uriparser_uri->userInfo.first > 0) {
			ZVAL_STRINGL(retval, uriparser_uri->userInfo.first, c - uriparser_uri->userInfo.first);
		} else {
			ZVAL_NULL(retval);
		}
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_parser_rfc3986_password_read(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(internal_uri->uri, read_mode);
	ZEND_ASSERT(uriparser_uri != NULL);

	if (uriparser_uri->userInfo.first != NULL && uriparser_uri->userInfo.afterLast != NULL) {
		const char *c = memchr(uriparser_uri->userInfo.first, ':', get_text_range_length(&uriparser_uri->userInfo));

		if (c != NULL && uriparser_uri->userInfo.afterLast - c - 1 > 0) {
			ZVAL_STRINGL(retval, c + 1, uriparser_uri->userInfo.afterLast - c - 1);
		} else {
			ZVAL_NULL(retval);
		}
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_parser_rfc3986_host_read(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(internal_uri->uri, read_mode);
	ZEND_ASSERT(uriparser_uri != NULL);

	if (uriparser_uri->hostText.first != NULL && uriparser_uri->hostText.afterLast != NULL) {
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

ZEND_ATTRIBUTE_NONNULL static size_t str_to_int(const char *str, size_t len)
{
	size_t result = 0;

	for (size_t i = 0; i < len; ++i) {
		result = result * 10 + (str[i] - '0');
	}

	return result;
}

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_parser_rfc3986_port_read(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(internal_uri->uri, read_mode);
	ZEND_ASSERT(uriparser_uri != NULL);

	if (uriparser_uri->portText.first != NULL && uriparser_uri->portText.afterLast != NULL) {
		ZVAL_LONG(retval, str_to_int(uriparser_uri->portText.first, get_text_range_length(&uriparser_uri->portText)));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_parser_rfc3986_path_read(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(internal_uri->uri, read_mode);
	ZEND_ASSERT(uriparser_uri != NULL);

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

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_parser_rfc3986_query_read(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(internal_uri->uri, read_mode);
	ZEND_ASSERT(uriparser_uri != NULL);

	if (uriparser_uri->query.first != NULL && uriparser_uri->query.afterLast != NULL) {
		ZVAL_STRINGL(retval, uriparser_uri->query.first, get_text_range_length(&uriparser_uri->query));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_parser_rfc3986_fragment_read(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const UriUriA *uriparser_uri = get_uri_for_reading(internal_uri->uri, read_mode);
	ZEND_ASSERT(uriparser_uri != NULL);

	if (uriparser_uri->fragment.first != NULL && uriparser_uri->fragment.afterLast != NULL) {
		ZVAL_STRINGL(retval, uriparser_uri->fragment.first, get_text_range_length(&uriparser_uri->fragment));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
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
	if (uriParseSingleUriExMmA(&uri, uri_str, uri_str + uri_str_len, NULL, mm) != URI_SUCCESS) {
		if (!silent) {
			zend_throw_exception(uri_invalid_uri_exception_ce, "The specified URI is malformed", 0);
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
						zend_throw_exception(uri_invalid_uri_exception_ce, "The specified base URI must be absolute", 0);
						break;
					default:
						/* This should be unreachable in practice. */
						zend_throw_exception(uri_invalid_uri_exception_ce, "Failed to resolve the specified URI against the base URI", 0);
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

/* When calling a wither successfully, the normalized URI is surely invalidated, therefore
 * it doesn't make sense to copy it. In case of failure, an exception is thrown, and the URI object
 * is discarded altogether. */
ZEND_ATTRIBUTE_NONNULL static void *php_uri_parser_rfc3986_clone(void *uri)
{
	const php_uri_parser_rfc3986_uris *uriparser_uris = uri;

	php_uri_parser_rfc3986_uris *new_uriparser_uris = uriparser_create_uris();
	copy_uri(&new_uriparser_uris->uri, &uriparser_uris->uri);
	if (uriparser_uris->normalized_uri_initialized) {
		copy_uri(&new_uriparser_uris->normalized_uri, &uriparser_uris->normalized_uri);
		new_uriparser_uris->normalized_uri_initialized = true;
	}

	return new_uriparser_uris;
}

ZEND_ATTRIBUTE_NONNULL static zend_string *php_uri_parser_rfc3986_to_string(void *uri, uri_recomposition_mode_t recomposition_mode, bool exclude_fragment)
{
	php_uri_parser_rfc3986_uris *uriparser_uris = uri;
	const UriUriA *uriparser_uri;

	if (recomposition_mode == URI_RECOMPOSITION_RAW_ASCII || recomposition_mode == URI_RECOMPOSITION_RAW_UNICODE) {
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

ZEND_ATTRIBUTE_NONNULL static void php_uri_parser_rfc3986_free(void *uri)
{
	php_uri_parser_rfc3986_uris *uriparser_uris = uri;

	uriFreeUriMembersMmA(&uriparser_uris->uri, mm);
	uriFreeUriMembersMmA(&uriparser_uris->normalized_uri, mm);

	efree(uriparser_uris);
}

const uri_parser_t php_uri_parser_rfc3986 = {
	.name = PHP_URI_PARSER_RFC3986,
	.parse_uri = php_uri_parser_rfc3986_parse,
	.clone_uri = php_uri_parser_rfc3986_clone,
	.uri_to_string = php_uri_parser_rfc3986_to_string,
	.free_uri = php_uri_parser_rfc3986_free,
	{
		.scheme = {.read_func = php_uri_parser_rfc3986_scheme_read, .write_func = NULL},
		.username = {.read_func = php_uri_parser_rfc3986_username_read, .write_func = NULL},
		.password = {.read_func = php_uri_parser_rfc3986_password_read, .write_func = NULL},
		.host = {.read_func = php_uri_parser_rfc3986_host_read, .write_func = NULL},
		.port = {.read_func = php_uri_parser_rfc3986_port_read, .write_func = NULL},
		.path = {.read_func = php_uri_parser_rfc3986_path_read, .write_func = NULL},
		.query = {.read_func = php_uri_parser_rfc3986_query_read, .write_func = NULL},
		.fragment = {.read_func = php_uri_parser_rfc3986_fragment_read, .write_func = NULL},
	}
};
