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
#include "php_uriparser.h"
#include "php_uri_common.h"
#include "Zend/zend_smart_str.h"
#include "Zend/zend_exceptions.h"

static void uriparser_free_uri(void *uri);
static void throw_invalid_uri_exception(void);

static inline size_t get_text_range_length(const UriTextRangeA *range)
{
	return range->afterLast - range->first;
}

static UriUriA *uriparser_copy_uri(UriUriA *uriparser_uri)
{
	ZEND_ASSERT(uriparser_uri != NULL);

	UriUriA *new_uriparser_uri = emalloc(sizeof(UriUriA));

	int result = uriCopyUriA(new_uriparser_uri, uriparser_uri);
	ZEND_ASSERT(result == URI_SUCCESS && new_uriparser_uri != NULL);

	return new_uriparser_uri;
}

static void uriparser_normalize_uri(UriUriA *uriparser_uri)
{
	int result = uriNormalizeSyntaxExA(uriparser_uri, (unsigned int)-1);
	ZEND_ASSERT(result == URI_SUCCESS);
}

static UriUriA *uriparser_read_uri(uriparser_uris_t *uriparser_uris, uri_component_read_mode_t read_mode)
{
	switch (read_mode) {
		case URI_COMPONENT_READ_RAW:
			return uriparser_uris->uri;
		case URI_COMPONENT_READ_NORMALIZED_ASCII:
			ZEND_FALLTHROUGH;
		case URI_COMPONENT_READ_NORMALIZED_UNICODE:
			if (uriparser_uris->normalized_uri == NULL) {
				uriparser_uris->normalized_uri = uriparser_copy_uri(uriparser_uris->uri);
				uriparser_normalize_uri(uriparser_uris->normalized_uri);
			}

			return uriparser_uris->normalized_uri;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}

static zend_result uriparser_read_scheme(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	UriUriA *uriparser_uri = uriparser_read_uri(internal_uri->uri, read_mode);
	ZEND_ASSERT(uriparser_uri != NULL);

	if (uriparser_uri->scheme.first != NULL && uriparser_uri->scheme.afterLast != NULL) {
		zend_string *str = zend_string_init(uriparser_uri->scheme.first, get_text_range_length(&uriparser_uri->scheme), false);
		ZVAL_NEW_STR(retval, str);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

zend_result uriparser_read_userinfo(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	UriUriA *uriparser_uri = uriparser_read_uri(internal_uri->uri, read_mode);
	ZEND_ASSERT(uriparser_uri != NULL);

	if (uriparser_uri->userInfo.first != NULL && uriparser_uri->userInfo.afterLast != NULL) {
		ZVAL_STRINGL(retval, uriparser_uri->userInfo.first, get_text_range_length(&uriparser_uri->userInfo));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_read_username(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	UriUriA *uriparser_uri = uriparser_read_uri(internal_uri->uri, read_mode);
	ZEND_ASSERT(uriparser_uri != NULL);

	if (uriparser_uri->userInfo.first != NULL && uriparser_uri->userInfo.afterLast != NULL) {
		size_t length = get_text_range_length(&uriparser_uri->userInfo);
		char *c = memchr(uriparser_uri->userInfo.first, ':', length);

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

static zend_result uriparser_read_password(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	UriUriA *uriparser_uri = uriparser_read_uri(internal_uri->uri, read_mode);
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

static zend_result uriparser_read_host(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	UriUriA *uriparser_uri = uriparser_read_uri(internal_uri->uri, read_mode);
	ZEND_ASSERT(uriparser_uri != NULL);

	if (uriparser_uri->hostText.first != NULL && uriparser_uri->hostText.afterLast != NULL && get_text_range_length(&uriparser_uri->hostText) > 0) {
		if (uriparser_uri->hostData.ip6 != NULL) {
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

static int str_to_int(const char *str, int len)
{
	int result = 0;

	for (int i = 0; i < len; ++i) {
		result = result * 10 + (str[i] - '0');
	}

	return result;
}

static zend_result uriparser_read_port(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	UriUriA *uriparser_uri = uriparser_read_uri(internal_uri->uri, read_mode);
	ZEND_ASSERT(uriparser_uri != NULL);

	if (uriparser_uri->portText.first != NULL && uriparser_uri->portText.afterLast != NULL) {
		ZVAL_LONG(retval, str_to_int(uriparser_uri->portText.first, get_text_range_length(&uriparser_uri->portText)));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_read_path(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	UriUriA *uriparser_uri = uriparser_read_uri(internal_uri->uri, read_mode);
	ZEND_ASSERT(uriparser_uri != NULL);

	if (uriparser_uri->pathHead != NULL) {
		const UriPathSegmentA *p;
		smart_str str = {0};

		if (uriparser_uri->absolutePath || uriIsHostSetA(uriparser_uri)) {
			smart_str_appendc(&str, '/');
		}

		smart_str_appendl(&str, uriparser_uri->pathHead->text.first, get_text_range_length(&uriparser_uri->pathHead->text));

		for (p = uriparser_uri->pathHead->next; p != NULL; p = p->next) {
			smart_str_appendc(&str, '/');
			smart_str_appendl(&str, p->text.first, get_text_range_length(&p->text));
		}

		ZVAL_NEW_STR(retval, smart_str_extract(&str));
	} else if (uriparser_uri->absolutePath) {
		ZVAL_CHAR(retval, '/');
	} else {
		ZVAL_EMPTY_STRING(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_read_query(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	UriUriA *uriparser_uri = uriparser_read_uri(internal_uri->uri, read_mode);
	ZEND_ASSERT(uriparser_uri != NULL);

	if (uriparser_uri->query.first != NULL && uriparser_uri->query.afterLast != NULL) {
		ZVAL_STRINGL(retval, uriparser_uri->query.first, get_text_range_length(&uriparser_uri->query));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_read_fragment(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	UriUriA *uriparser_uri = uriparser_read_uri(internal_uri->uri, read_mode);
	ZEND_ASSERT(uriparser_uri != NULL);

	if (uriparser_uri->fragment.first != NULL && uriparser_uri->fragment.afterLast != NULL) {
		ZVAL_STRINGL(retval, uriparser_uri->fragment.first, get_text_range_length(&uriparser_uri->fragment));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static void *uriparser_malloc(UriMemoryManager *memory_manager, size_t size)
{
	return emalloc(size);
}

static void *uriparser_calloc(UriMemoryManager *memory_manager, size_t nmemb, size_t size)
{
	return ecalloc(nmemb, size);
}

static void *uriparser_realloc(UriMemoryManager *memory_manager, void *ptr, size_t size)
{
	return erealloc(ptr, size);
}

static void *uriparser_reallocarray(UriMemoryManager *memory_manager, void *ptr, size_t nmemb, size_t size)
{
	return safe_erealloc(ptr, nmemb, size, 0);
}

static void uriparser_free(UriMemoryManager *memory_manager, void *ptr)
{
	efree(ptr);
}

PHP_MINIT_FUNCTION(uri_uriparser)
{
	if (uri_handler_register(&uriparser_uri_handler) == FAILURE) {
		return FAILURE;
	}

	defaultMemoryManager.malloc = uriparser_malloc;
	defaultMemoryManager.calloc = uriparser_calloc;
	defaultMemoryManager.realloc = uriparser_realloc;
	defaultMemoryManager.reallocarray = uriparser_reallocarray;
	defaultMemoryManager.free = uriparser_free;

	return SUCCESS;
}

static uriparser_uris_t *uriparser_create_uris(
	UriUriA *uri,
	UriUriA *normalized_uri
) {
	uriparser_uris_t *uriparser_uris = emalloc(sizeof(uriparser_uris_t));

	uriparser_uris->uri = uri;
	uriparser_uris->normalized_uri = normalized_uri;

	return uriparser_uris;
}

static void throw_invalid_uri_exception(void)
{
	zend_throw_exception(uri_invalid_uri_exception_ce, "URI parsing failed", 0);
}

void *uriparser_parse_uri_ex(const zend_string *uri_str, const uriparser_uris_t *uriparser_base_urls, bool silent)
{
	UriUriA *uriparser_uri = emalloc(sizeof(UriUriA));

	if (ZSTR_LEN(uri_str) == 0 ||
		uriParseSingleUriExA(uriparser_uri, ZSTR_VAL(uri_str), ZSTR_VAL(uri_str) + ZSTR_LEN(uri_str), NULL) != URI_SUCCESS
	) {
		efree(uriparser_uri);
		if (!silent) {
			throw_invalid_uri_exception();
		}

		return NULL;
	}

	uriMakeOwnerA(uriparser_uri);
	if (uriparser_base_urls == NULL) {
		return uriparser_create_uris(uriparser_uri, NULL);
	}

	UriUriA *absolute_uri = emalloc(sizeof(UriUriA));

	if (uriAddBaseUriA(absolute_uri, uriparser_uri, uriparser_base_urls->uri) != URI_SUCCESS) {
		uriFreeUriMembersA(uriparser_uri);
		efree(uriparser_uri);
		efree(absolute_uri);

		if (!silent) {
			throw_invalid_uri_exception();
		}

		return NULL;
	}

	uriMakeOwnerA(absolute_uri);
	uriFreeUriMembersA(uriparser_uri);
	efree(uriparser_uri);

	return uriparser_create_uris(absolute_uri, NULL);
}

void *uriparser_parse_uri(const zend_string *uri_str, const void *base_url, zval *errors, bool silent)
{
	return uriparser_parse_uri_ex(uri_str, base_url, silent);
}

/* TODO make the clone handler accept a flag to distingish between clone() calls and withers.
 * When calling a wither successfully, the normalized URI is surely invalidated, therefore
 * it doesn't make sense to copy it. In case of failure, an exeption is thrown, and the URI object
 * is discarded altogether. */
static void *uriparser_clone_uri(void *uri)
{
	uriparser_uris_t *uriparser_uris = (uriparser_uris_t *) uri;

	return uriparser_create_uris(
		uriparser_copy_uri(uriparser_uris->uri),
		uriparser_uris->normalized_uri != NULL ? uriparser_copy_uri(uriparser_uris->normalized_uri) : NULL
	);
}

static zend_string *uriparser_uri_to_string(void *uri, uri_recomposition_mode_t recomposition_mode, bool exclude_fragment)
{
	uriparser_uris_t *uriparser_uris = (uriparser_uris_t *) uri;
	UriUriA *uriparser_uri;

	if (recomposition_mode == URI_RECOMPOSITION_RAW_ASCII || recomposition_mode == URI_RECOMPOSITION_RAW_UNICODE) {
		uriparser_uri = uriparser_uris->uri;
	} else {
		if (uriparser_uris->normalized_uri == NULL) {
			uriparser_uris->normalized_uri = uriparser_copy_uri(uriparser_uris->uri);
			uriparser_normalize_uri(uriparser_uris->normalized_uri);
		}
		uriparser_uri = uriparser_uris->normalized_uri;
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

static void uriparser_free_uri(void *uri)
{
	uriparser_uris_t *uriparser_uris = (uriparser_uris_t *) uri;

	if (uriparser_uris->uri != NULL) {
		uriFreeUriMembersA(uriparser_uris->uri);
		efree(uriparser_uris->uri);
		uriparser_uris->uri = NULL;
	}

	if (uriparser_uris->normalized_uri != NULL) {
		ZEND_ASSERT(uriparser_uris->normalized_uri->owner);

		uriFreeUriMembersA(uriparser_uris->normalized_uri);
		efree(uriparser_uris->normalized_uri);
		uriparser_uris->normalized_uri = NULL;
	}

	efree(uriparser_uris);
}

const uri_handler_t uriparser_uri_handler = {
	.name = URI_PARSER_RFC3986,
	.parse_uri = uriparser_parse_uri,
	.clone_uri = uriparser_clone_uri,
	.uri_to_string = uriparser_uri_to_string,
	.free_uri = uriparser_free_uri,
	{
		.scheme = {.read_func = uriparser_read_scheme, .write_func = NULL},
		.username = {.read_func = uriparser_read_username, .write_func = NULL},
		.password = {.read_func = uriparser_read_password, .write_func = NULL},
		.host = {.read_func = uriparser_read_host, .write_func = NULL},
		.port = {.read_func = uriparser_read_port, .write_func = NULL},
		.path = {.read_func = uriparser_read_path, .write_func = NULL},
		.query = {.read_func = uriparser_read_query, .write_func = NULL},
		.fragment = {.read_func = uriparser_read_fragment, .write_func = NULL},
	}
};
