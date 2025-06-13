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

void *uriparser_parse_uri(const zend_string *uri_str, const void *base_url, zval *errors, bool silent);
static void uriparser_free_uri(void *uri);
static void throw_invalid_uri_exception(void);

static inline size_t get_text_range_length(const UriTextRangeA *range)
{
	return range->afterLast - range->first;
}

static void uriparser_copy_text_range(UriTextRangeA *text_range, UriTextRangeA *new_text_range, bool use_safe)
{
	if (text_range->first == NULL || text_range->afterLast == NULL || (text_range->first > text_range->afterLast && !use_safe)) {
		new_text_range->first = NULL;
		new_text_range->afterLast = NULL;
	} else if (text_range->first >= text_range->afterLast && use_safe) {
		new_text_range->first = uriSafeToPointToA;
		new_text_range->afterLast = uriSafeToPointToA;
	} else {
		size_t length = get_text_range_length(text_range);
		char *dup = emalloc(length);
		memcpy(dup, text_range->first, length);

		new_text_range->first = dup;
		new_text_range->afterLast = dup + length;
	}
}

static UriUriA *uriparser_copy_uri(UriUriA *uriparser_uri) // TODO add to uriparser
{
	UriUriA *new_uriparser_uri = emalloc(sizeof(UriUriA));

	uriparser_copy_text_range(&uriparser_uri->scheme, &new_uriparser_uri->scheme, false);
	uriparser_copy_text_range(&uriparser_uri->userInfo, &new_uriparser_uri->userInfo, false);
	uriparser_copy_text_range(&uriparser_uri->hostText, &new_uriparser_uri->hostText, true);
	if (uriparser_uri->hostData.ip4 == NULL) {
		new_uriparser_uri->hostData.ip4 = NULL;
	} else {
		new_uriparser_uri->hostData.ip4 = emalloc(sizeof(UriIp4));
		*(new_uriparser_uri->hostData.ip4) = *(uriparser_uri->hostData.ip4);
	}
	if (uriparser_uri->hostData.ip6 == NULL) {
		new_uriparser_uri->hostData.ip6 = NULL;
	} else {
		new_uriparser_uri->hostData.ip6 = emalloc(sizeof(UriIp6));
		*(new_uriparser_uri->hostData.ip6) = *(uriparser_uri->hostData.ip6);
	}
	uriparser_copy_text_range(&uriparser_uri->hostData.ipFuture, &new_uriparser_uri->hostData.ipFuture, false);
	uriparser_copy_text_range(&uriparser_uri->portText, &new_uriparser_uri->portText, false);

	if (uriparser_uri->pathHead != NULL && uriparser_uri->pathTail != NULL) {
		new_uriparser_uri->pathHead = emalloc(sizeof(UriPathSegmentA));
		uriparser_copy_text_range(&uriparser_uri->pathHead->text, &new_uriparser_uri->pathHead->text, true);
		new_uriparser_uri->pathHead->reserved = NULL;

		UriPathSegmentA *p = uriparser_uri->pathHead->next;
		UriPathSegmentA *new_p = new_uriparser_uri->pathHead;
		while (p != NULL && (p->text.first != p->text.afterLast || p->text.first == uriSafeToPointToA)) {
			new_p->next = emalloc(sizeof(UriPathSegmentA));
			new_p = new_p->next;
			uriparser_copy_text_range(&p->text, &new_p->text, true);
			new_p->reserved = NULL;
			p = p->next;
		}
		new_p->next = NULL;
		new_uriparser_uri->pathTail = new_p;
	} else {
		new_uriparser_uri->pathHead = NULL;
		new_uriparser_uri->pathTail = NULL;
	}

	uriparser_copy_text_range(&uriparser_uri->query, &new_uriparser_uri->query, false);
	uriparser_copy_text_range(&uriparser_uri->fragment, &new_uriparser_uri->fragment, false);
	new_uriparser_uri->absolutePath = uriparser_uri->absolutePath;
	new_uriparser_uri->owner = true;
	new_uriparser_uri->reserved = NULL;

	return new_uriparser_uri;
}

static zend_result uriparser_normalize_uri(UriUriA *uriparser_uri)
{
	if (uriNormalizeSyntaxExA(uriparser_uri, (unsigned int)-1) != URI_SUCCESS) {
		return FAILURE;
	}

	return SUCCESS;
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
				if (uriparser_normalize_uri(uriparser_uris->normalized_uri) == FAILURE) {
					uriFreeUriMembersA(uriparser_uris->normalized_uri);
					efree(uriparser_uris->normalized_uri);
					uriparser_uris->normalized_uri = NULL;

					return NULL;
				}
			}

			return uriparser_uris->normalized_uri;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}

static zend_result uriparser_read_scheme(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	UriUriA *uriparser_uri = uriparser_read_uri(internal_uri->uri, read_mode);
	if (UNEXPECTED(uriparser_uri == NULL)) {
		return FAILURE;
	}

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
	if (UNEXPECTED(uriparser_uri == NULL)) {
		return FAILURE;
	}

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
	if (UNEXPECTED(uriparser_uri == NULL)) {
		return FAILURE;
	}

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
	if (UNEXPECTED(uriparser_uri == NULL)) {
		return FAILURE;
	}

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
	if (UNEXPECTED(uriparser_uri == NULL)) {
		return FAILURE;
	}

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
	if (UNEXPECTED(uriparser_uri == NULL)) {
		return FAILURE;
	}

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
	if (UNEXPECTED(uriparser_uri == NULL)) {
		return FAILURE;
	}

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
	if (UNEXPECTED(uriparser_uri == NULL)) {
		return FAILURE;
	}

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
	if (UNEXPECTED(uriparser_uri == NULL)) {
		return FAILURE;
	}

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
	UriUriA *uri, zend_string *uri_str,
	UriUriA *normalized_uri, zend_string *normalized_uri_str
) {
	uriparser_uris_t *uriparser_uris = emalloc(sizeof(uriparser_uris_t));

	uriparser_uris->uri = uri;
	uriparser_uris->uri_str = uri_str;
	uriparser_uris->normalized_uri = normalized_uri;
	uriparser_uris->normalized_uri_str = normalized_uri_str;

	return uriparser_uris;
}

static void throw_invalid_uri_exception(void)
{
	zend_throw_exception(uri_invalid_uri_exception_ce, "URI parsing failed", 0);
}

void *uriparser_parse_uri_ex(const zend_string *uri_str, const uriparser_uris_t *uriparser_base_urls, bool silent)
{
	UriUriA *uriparser_uri = emalloc(sizeof(UriUriA));

	/* uriparser keeps the originally passed in string, while lexbor may allocate a new one. */
	zend_string *original_uri_str = zend_string_init(ZSTR_VAL(uri_str), ZSTR_LEN(uri_str), false);
	if (ZSTR_LEN(original_uri_str) == 0 ||
		uriParseSingleUriExA(uriparser_uri, ZSTR_VAL(original_uri_str), ZSTR_VAL(original_uri_str) + ZSTR_LEN(original_uri_str), NULL) != URI_SUCCESS
	) {
		efree(uriparser_uri);
		zend_string_release_ex(original_uri_str, false);
		if (!silent) {
			throw_invalid_uri_exception();
		}

		return NULL;
	}

	if (uriparser_base_urls == NULL) {
		return uriparser_create_uris(uriparser_uri, original_uri_str, NULL, NULL);
	}

	UriUriA *uriparser_base_url = uriparser_copy_uri(uriparser_base_urls->uri);

	UriUriA *absolute_uri = emalloc(sizeof(UriUriA));

	if (uriAddBaseUriExA(absolute_uri, uriparser_uri, uriparser_base_url, URI_RESOLVE_STRICTLY) != URI_SUCCESS) {
		zend_string_release_ex(original_uri_str, false);
		uriFreeUriMembersA(uriparser_uri);
		efree(uriparser_uri);
		uriFreeUriMembersA(uriparser_base_url);
		efree(uriparser_base_url);
		efree(absolute_uri);

		if (!silent) {
			throw_invalid_uri_exception();
		}

		return NULL;
	}

	/* TODO fix freeing: if the following code runs, then we'll have use-after-free-s because uriparser doesn't
	   copy the input. If we don't run the following code, then we'll have memory leaks...
	uriFreeUriMembersA(uriparser_base_url);
	efree(uriparser_base_url);
	uriFreeUriMembersA(uriparser_uri);
	efree(uriparser_uri);
	 */

	return uriparser_create_uris(absolute_uri, original_uri_str, NULL, NULL);
}

void *uriparser_parse_uri(const zend_string *uri_str, const void *base_url, zval *errors, bool silent)
{
	return uriparser_parse_uri_ex(uri_str, base_url, silent);
}

static void *uriparser_clone_uri(void *uri)
{
	uriparser_uris_t *uriparser_uris = (uriparser_uris_t *) uri;

	return uriparser_create_uris(
		uriparser_copy_uri(uriparser_uris->uri),
		zend_string_copy(uriparser_uris->uri_str),
		uriparser_uris->normalized_uri != NULL ? uriparser_copy_uri(uriparser_uris->normalized_uri) : NULL,
		uriparser_uris->normalized_uri_str != NULL ? zend_string_copy(uriparser_uris->normalized_uri_str) : NULL
	);
}

static zend_string *uriparser_uri_to_string(void *uri, uri_recomposition_mode_t recomposition_mode, bool exclude_fragment)
{
	uriparser_uris_t *uriparser_uris = (uriparser_uris_t *) uri;
	UriUriA *uriparser_uri = uriparser_uris->uri;

	if ((recomposition_mode == URI_RECOMPOSITION_NORMALIZED_UNICODE || recomposition_mode == URI_RECOMPOSITION_NORMALIZED_ASCII) &&
		uriparser_uris->normalized_uri == NULL
	) {
		uriparser_uris->normalized_uri = uriparser_copy_uri(uriparser_uris->uri);
		if (uriparser_normalize_uri(uriparser_uris->normalized_uri) == FAILURE) {
			return NULL;
		}
		uriparser_uri = uriparser_uris->normalized_uri;
	}

	int charsRequired;
	if (uriToStringCharsRequiredA(uriparser_uri, &charsRequired) != URI_SUCCESS) {
		return NULL;
	}

	charsRequired++;

	zend_string *uri_string = zend_string_alloc(charsRequired - 1, false);
	if (uriToStringA(ZSTR_VAL(uri_string), uriparser_uri, charsRequired, NULL) != URI_SUCCESS) {
		zend_string_efree(uri_string);
		return NULL;
	}

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
		if (uriparser_uris->uri_str != NULL) { // TODO can this double free? should we check uriparser_uris->uri->owner?
			zend_string_release(uriparser_uris->uri_str);
			uriparser_uris->uri_str = NULL;
		}

		uriFreeUriMembersA(uriparser_uris->uri);
		efree(uriparser_uris->uri);
		uriparser_uris->uri = NULL;
	}

	if (uriparser_uris->normalized_uri != NULL) {
		ZEND_ASSERT(uriparser_uris->normalized_uri->owner);
		if (uriparser_uris->normalized_uri_str != NULL) {
			zend_string_release(uriparser_uris->normalized_uri_str);
			uriparser_uris->normalized_uri_str = NULL;
		}

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
