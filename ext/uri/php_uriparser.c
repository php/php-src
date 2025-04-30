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

static zend_result uriparser_init_parser(void);
static void *uriparser_parse_uri(const zend_string *uri_str, const void *base_uri, zval *errors);
static void uriparser_create_invalid_uri_exception(zval *exception_zv, zval *errors);
static void *uriparser_clone_uri(void *uri);
static zend_string *uriparser_uri_to_string(void *uri, uri_recomposition_mode_t recomposition_mode, bool exclude_fragment);
static void uriparser_free_uri(void *uri);
static zend_result uriparser_destroy_parser(void);

HashTable uriparser_property_handlers;
UriMemoryManager uriparser_memory_manager;

const uri_handler_t uriparser_uri_handler = {
	URI_PARSER_RFC3986,
	uriparser_init_parser,
	uriparser_parse_uri,
	uriparser_create_invalid_uri_exception,
	uriparser_clone_uri,
	uriparser_uri_to_string,
	uriparser_free_uri,
	uriparser_destroy_parser,
	&uriparser_property_handlers
};

#define URIPARSER_PARSE_STR(uri_str, internal_uri, errors) do { \
    zend_string *str = smart_str_extract(&uri_str); \
	uriparser_uris_t *new_uriparser_uris = uriparser_parse_uri(str, NULL, errors); \
	if (new_uriparser_uris == NULL) { \
		smart_str_free(&uri_str); \
		zend_string_release(str); \
		return FAILURE; \
	} \
	uriparser_free_uri(internal_uri->uri); \
	internal_uri->uri = (void *) new_uriparser_uris; \
	smart_str_free(&uri_str); \
	zend_string_release(str); \
	return SUCCESS; \
} while (0)

static void uriparser_copy_text_range(UriTextRangeA *text_range, UriTextRangeA *new_text_range, bool use_safe)
{
	if (text_range->first == NULL || text_range->afterLast == NULL || (text_range->first > text_range->afterLast && !use_safe)) {
		new_text_range->first = NULL;
		new_text_range->afterLast = NULL;
	} else if (text_range->first >= text_range->afterLast && use_safe) {
		new_text_range->first = uriSafeToPointToA;
		new_text_range->afterLast = uriSafeToPointToA;
	} else {
		size_t length = (size_t) (text_range->afterLast - text_range->first);
		char *dup = emalloc(length);
		memcpy(dup, text_range->first, length);

		new_text_range->first = dup;
		new_text_range->afterLast = dup + length;
	}
}

static UriUriA *uriparser_copy_uri(UriUriA *uriparser_uri)
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
	if (uriNormalizeSyntaxExMmA(uriparser_uri, (unsigned int)-1, &uriparser_memory_manager) != URI_SUCCESS) {
		return FAILURE;
	}

	return SUCCESS;
}

#define URIPARSER_READ_URI(uriparser_uri, uriparser_uris, read_mode) do { \
	if (read_mode == URI_COMPONENT_READ_RAW) { \
        uriparser_uri = (UriUriA *) uriparser_uris->uri; \
    } else if (read_mode == URI_COMPONENT_READ_NORMALIZED_UNICODE || read_mode == URI_COMPONENT_READ_NORMALIZED_ASCII) { \
        if (uriparser_uris->normalized_uri == NULL) { \
			uriparser_uris->normalized_uri = uriparser_copy_uri(uriparser_uris->uri); \
			if (uriparser_normalize_uri(uriparser_uris->normalized_uri) == FAILURE) { \
				return FAILURE; \
			} \
        } \
		uriparser_uri = uriparser_uris->normalized_uri; \
	} else { \
    	ZEND_UNREACHABLE(); \
	} \
} while (0)

static zend_result uriparser_read_scheme(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	uriparser_uris_t *uriparser_uris = (uriparser_uris_t *) internal_uri->uri;
	UriUriA *uriparser_uri;
	URIPARSER_READ_URI(uriparser_uri, uriparser_uris, read_mode);

	if (uriparser_uri->scheme.first != NULL && uriparser_uri->scheme.afterLast != NULL) {
		ZVAL_STRINGL(retval, uriparser_uri->scheme.first, uriparser_uri->scheme.afterLast - uriparser_uri->scheme.first);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_read_userinfo(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	uriparser_uris_t *uriparser_uris = (uriparser_uris_t *) internal_uri->uri;
	UriUriA *uriparser_uri;
	URIPARSER_READ_URI(uriparser_uri, uriparser_uris, read_mode);

	if (uriparser_uri->userInfo.first != NULL && uriparser_uri->userInfo.afterLast != NULL) {
		ZVAL_STRINGL(retval, uriparser_uri->userInfo.first, uriparser_uri->userInfo.afterLast - uriparser_uri->userInfo.first);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_read_username(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	uriparser_uris_t *uriparser_uris = (uriparser_uris_t *) internal_uri->uri;
	UriUriA *uriparser_uri;
	URIPARSER_READ_URI(uriparser_uri, uriparser_uris, read_mode);

	if (uriparser_uri->userInfo.first != NULL && uriparser_uri->userInfo.afterLast != NULL) {
		char *c = strchr(uriparser_uri->userInfo.first, ':');
		if (c == NULL && uriparser_uri->userInfo.afterLast - uriparser_uri->userInfo.first > 0) {
			ZVAL_STRINGL(retval, uriparser_uri->userInfo.first, uriparser_uri->userInfo.afterLast - uriparser_uri->userInfo.first);
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
	uriparser_uris_t *uriparser_uris = (uriparser_uris_t *) internal_uri->uri;
	UriUriA *uriparser_uri;
	URIPARSER_READ_URI(uriparser_uri, uriparser_uris, read_mode);

	if (uriparser_uri->userInfo.first != NULL && uriparser_uri->userInfo.afterLast != NULL) {
		char *c = strchr(uriparser_uri->userInfo.first, ':');
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
	uriparser_uris_t *uriparser_uris = (uriparser_uris_t *) internal_uri->uri;
	UriUriA *uriparser_uri;
	URIPARSER_READ_URI(uriparser_uri, uriparser_uris, read_mode);

	if (uriparser_uri->hostText.first != NULL && uriparser_uri->hostText.afterLast != NULL && uriparser_uri->hostText.afterLast - uriparser_uri->hostText.first > 0) {
		if (uriparser_uri->hostData.ip6 != NULL) {
			smart_str host_str = {0};

			smart_str_appendc(&host_str, '[');
			smart_str_appendl(&host_str, uriparser_uri->hostText.first, uriparser_uri->hostText.afterLast - uriparser_uri->hostText.first);
			smart_str_appendc(&host_str, ']');

			ZVAL_STR(retval, smart_str_extract(&host_str));
		} else {
			ZVAL_STRINGL(retval, uriparser_uri->hostText.first, uriparser_uri->hostText.afterLast - uriparser_uri->hostText.first);
		}
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_read_port(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	uriparser_uris_t *uriparser_uris = (uriparser_uris_t *) internal_uri->uri;
	UriUriA *uriparser_uri = uriparser_uris->uri;

	if (uriparser_uri->portText.first != NULL && uriparser_uri->portText.afterLast != NULL) {
		ZVAL_LONG(retval, strtol(uriparser_uri->portText.first, NULL, 10));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_read_path(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	uriparser_uris_t *uriparser_uris = (uriparser_uris_t *) internal_uri->uri;
	UriUriA *uriparser_uri;
	URIPARSER_READ_URI(uriparser_uri, uriparser_uris, read_mode);

	if (uriparser_uri->pathHead != NULL) {
		const UriPathSegmentA *p;
		smart_str str = {0};

		if (uriparser_uri->absolutePath || uriIsHostSetA(uriparser_uri)) {
			smart_str_appends(&str, "/");
		}

		smart_str_appendl(&str, uriparser_uri->pathHead->text.first, (int) ((uriparser_uri->pathHead->text).afterLast - (uriparser_uri->pathHead->text).first));

		for (p = uriparser_uri->pathHead->next; p != NULL; p = p->next) {
			smart_str_appends(&str, "/");
			smart_str_appendl(&str, p->text.first, (int) ((p->text).afterLast - (p->text).first));
		}

		ZVAL_STR(retval, smart_str_extract(&str));
	} else if (uriparser_uri->absolutePath) {
		ZVAL_CHAR(retval, '/');
	} else {
		ZVAL_EMPTY_STRING(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_read_query(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	uriparser_uris_t *uriparser_uris = (uriparser_uris_t *) internal_uri->uri;
	UriUriA *uriparser_uri;
	URIPARSER_READ_URI(uriparser_uri, uriparser_uris, read_mode);

	if (uriparser_uri->query.first != NULL && uriparser_uri->query.afterLast != NULL) {
		ZVAL_STRINGL(retval, uriparser_uri->query.first, uriparser_uri->query.afterLast - uriparser_uri->query.first);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_read_fragment(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	uriparser_uris_t *uriparser_uris = (uriparser_uris_t *) internal_uri->uri;
	UriUriA *uriparser_uri;
	URIPARSER_READ_URI(uriparser_uri, uriparser_uris, read_mode);

	if (uriparser_uri->fragment.first != NULL && uriparser_uri->fragment.afterLast != NULL) {
		ZVAL_STRINGL(retval, uriparser_uri->fragment.first, uriparser_uri->fragment.afterLast - uriparser_uri->fragment.first);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static void uriparser_append_scheme(const uri_internal_t *internal_uri, smart_str *uri_str)
{
	zval tmp;

	uriparser_read_scheme(internal_uri, URI_COMPONENT_READ_RAW, &tmp);
	if (Z_TYPE(tmp) == IS_STRING && Z_STRLEN(tmp) > 0) {
		smart_str_append(uri_str, Z_STR(tmp));
		smart_str_appendl(uri_str, "://", sizeof("://") - 1); /* TODO apply the algorithm at https://datatracker.ietf.org/doc/html/rfc3986#section-5.3 */
	}

	zval_ptr_dtor(&tmp);
}

static void uriparser_append_userinfo(const uri_internal_t *internal_uri, smart_str *uri_str)
{
	uriparser_uris_t *uriparser_uris = (uriparser_uris_t *) internal_uri->uri;
	UriUriA *uriparser_uri = (UriUriA *) uriparser_uris->uri;

	if (uriparser_uri->userInfo.first != NULL && uriparser_uri->userInfo.afterLast != NULL && uriparser_uri->userInfo.afterLast - uriparser_uri->userInfo.first > 0) {
		smart_str_appendl(uri_str, uriparser_uri->userInfo.first, uriparser_uri->userInfo.afterLast - uriparser_uri->userInfo.first);
	}
}

static void uriparser_append_host(const uri_internal_t *internal_uri, smart_str *uri_str)
{
	zval tmp;

	uriparser_read_host(internal_uri, URI_COMPONENT_READ_RAW, &tmp);
	if (Z_TYPE(tmp) == IS_STRING && Z_STRLEN(tmp) > 0) {
		smart_str_append(uri_str, Z_STR(tmp));
	}

	zval_ptr_dtor(&tmp);
}

static void uriparser_append_port(const uri_internal_t *internal_uri, smart_str *uri_str)
{
	zval tmp;

	uriparser_read_port(internal_uri, URI_COMPONENT_READ_RAW, &tmp);
	if (Z_TYPE(tmp) == IS_LONG) {
		smart_str_appendc(uri_str, ':');
		smart_str_append_long(uri_str, Z_LVAL(tmp));
	}

	zval_ptr_dtor(&tmp);
}

static void uriparser_append_path(const uri_internal_t *internal_uri, smart_str *uri_str)
{
	zval tmp;

	uriparser_read_path(internal_uri, URI_COMPONENT_READ_RAW, &tmp);
	if (Z_TYPE(tmp) == IS_STRING && Z_STRLEN(tmp) > 0) {
		smart_str_append(uri_str, Z_STR(tmp));
	}

	zval_ptr_dtor(&tmp);
}

static void uriparser_append_query(const uri_internal_t *internal_uri, smart_str *uri_str)
{
	zval tmp;

	uriparser_read_query(internal_uri, URI_COMPONENT_READ_RAW, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		smart_str_appendc(uri_str, '?');
		if (Z_STRLEN(tmp) > 0) {
			smart_str_append(uri_str, Z_STR(tmp));
		}
	}

	zval_ptr_dtor(&tmp);
}

static void uriparser_append_fragment(const uri_internal_t *internal_uri, smart_str *uri_str)
{
	zval tmp;

	uriparser_read_fragment(internal_uri, URI_COMPONENT_READ_RAW, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		smart_str_appendc(uri_str, '#');
		if (Z_STRLEN(tmp) > 0) {
			smart_str_append(uri_str, Z_STR(tmp));
		}
	}

	zval_ptr_dtor(&tmp);
}

static zend_result uriparser_write_scheme(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	smart_str uri_str = {0};

	if (Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) > 0) {
		smart_str_append(&uri_str, Z_STR_P(value));
		smart_str_appends(&uri_str, "://");
	}

	uriparser_append_userinfo(internal_uri, &uri_str);
	uriparser_append_host(internal_uri, &uri_str);
	uriparser_append_port(internal_uri, &uri_str);
	uriparser_append_path(internal_uri, &uri_str);
	uriparser_append_query(internal_uri, &uri_str);
	uriparser_append_fragment(internal_uri, &uri_str);

	URIPARSER_PARSE_STR(uri_str, internal_uri, errors);
}

static zend_result uriparser_write_userinfo(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	smart_str uri_str = {0};

	uriparser_append_scheme(internal_uri, &uri_str);

	if (Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) > 0) {
		smart_str_append(&uri_str, Z_STR_P(value));
		smart_str_appends(&uri_str, "@");
	}

	uriparser_append_host(internal_uri, &uri_str);
	uriparser_append_port(internal_uri, &uri_str);
	uriparser_append_path(internal_uri, &uri_str);
	uriparser_append_query(internal_uri, &uri_str);
	uriparser_append_fragment(internal_uri, &uri_str);

	URIPARSER_PARSE_STR(uri_str, internal_uri, errors);
}

static zend_result uriparser_write_host(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	smart_str uri_str = {0};
	uriparser_append_scheme(internal_uri, &uri_str);
	uriparser_append_userinfo(internal_uri, &uri_str);

	if (Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) > 0) {
		smart_str_append(&uri_str, Z_STR_P(value));
	}

	uriparser_append_port(internal_uri, &uri_str);
	uriparser_append_path(internal_uri, &uri_str);
	uriparser_append_query(internal_uri, &uri_str);
	uriparser_append_fragment(internal_uri, &uri_str);

	URIPARSER_PARSE_STR(uri_str, internal_uri, errors);
}

static zend_result uriparser_write_port(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	smart_str uri_str = {0};
	uriparser_append_scheme(internal_uri, &uri_str);
	uriparser_append_userinfo(internal_uri, &uri_str);
	uriparser_append_host(internal_uri, &uri_str);

	if (Z_TYPE_P(value) == IS_LONG) {
		smart_str_appendc(&uri_str, ':');
		smart_str_append_long(&uri_str, Z_LVAL_P(value));
	}

	uriparser_append_path(internal_uri, &uri_str);
	uriparser_append_query(internal_uri, &uri_str);
	uriparser_append_fragment(internal_uri, &uri_str);

	URIPARSER_PARSE_STR(uri_str, internal_uri, errors);
}

static zend_result uriparser_write_path(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	smart_str uri_str = {0};
	uriparser_append_scheme(internal_uri, &uri_str);
	uriparser_append_userinfo(internal_uri, &uri_str);
	uriparser_append_host(internal_uri, &uri_str);
	uriparser_append_port(internal_uri, &uri_str);

	if (Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) > 0) {
		smart_str_append(&uri_str, Z_STR_P(value));
	}

	uriparser_append_query(internal_uri, &uri_str);
	uriparser_append_fragment(internal_uri, &uri_str);

	URIPARSER_PARSE_STR(uri_str, internal_uri, errors);
}

static zend_result uriparser_write_query(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	smart_str uri_str = {0};
	uriparser_append_scheme(internal_uri, &uri_str);
	uriparser_append_userinfo(internal_uri, &uri_str);
	uriparser_append_host(internal_uri, &uri_str);
	uriparser_append_port(internal_uri, &uri_str);
	uriparser_append_path(internal_uri, &uri_str);

	if (Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) > 0) {
		if (!zend_string_starts_with_literal(Z_STR_P(value), "?")) {
			smart_str_appendc(&uri_str, '?');
		}
		smart_str_append(&uri_str, Z_STR_P(value));
	}

	uriparser_append_fragment(internal_uri, &uri_str);

	URIPARSER_PARSE_STR(uri_str, internal_uri, errors);
}

static zend_result uriparser_write_fragment(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	smart_str uri_str = {0};
	uriparser_append_scheme(internal_uri, &uri_str);
	uriparser_append_userinfo(internal_uri, &uri_str);
	uriparser_append_host(internal_uri, &uri_str);
	uriparser_append_port(internal_uri, &uri_str);
	uriparser_append_path(internal_uri, &uri_str);
	uriparser_append_query(internal_uri, &uri_str);

	if (Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) > 0) {
		smart_str_append(&uri_str, Z_STR_P(value));
	}

	URIPARSER_PARSE_STR(uri_str, internal_uri, errors);
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
	return realloc(ptr, size);
}

static void *uriparser_reallocarray(UriMemoryManager *memory_manager, void *ptr, size_t nmemb, size_t size)
{
	const size_t total_size = nmemb * size;

	/* check for unsigned overflow */ \
	if ((nmemb != 0) && (total_size / nmemb != size)) {
		errno = ENOMEM;
		return NULL;
	}

	return erealloc(ptr, total_size);
}

static void uriparser_free(UriMemoryManager *memory_manager, void * ptr)
{
	efree(ptr);
}

static zend_result uriparser_init_parser(void)
{
	uriparser_memory_manager.calloc = uriparser_calloc;
	uriparser_memory_manager.malloc = uriparser_malloc;
	uriparser_memory_manager.realloc = uriparser_realloc;
	uriparser_memory_manager.reallocarray = uriparser_reallocarray;
	uriparser_memory_manager.free = uriparser_free;

	zend_hash_init(&uriparser_property_handlers, 8, NULL, NULL, true);

	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_SCHEME), uriparser_read_scheme, uriparser_write_scheme);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_USERINFO), uriparser_read_userinfo, uriparser_write_userinfo);
	URI_REGISTER_PROPERTY_READ_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_USERNAME), uriparser_read_username);
	URI_REGISTER_PROPERTY_READ_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_PASSWORD), uriparser_read_password);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_HOST), uriparser_read_host, uriparser_write_host);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_PORT), uriparser_read_port, uriparser_write_port);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_PATH), uriparser_read_path, uriparser_write_path);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_QUERY), uriparser_read_query, uriparser_write_query);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_FRAGMENT), uriparser_read_fragment, uriparser_write_fragment);

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

static void *uriparser_parse_uri(const zend_string *uri_str, const void *base_url, zval *errors)
{
	UriUriA *uriparser_uri = emalloc(sizeof(UriUriA));

	/* uriparser keeps the originally passed in string, while lexbor may allocate a new one. */
	zend_string *original_uri_str = zend_string_init(ZSTR_VAL(uri_str), ZSTR_LEN(uri_str), false);
	if (ZSTR_LEN(original_uri_str) == 0 ||
		uriParseSingleUriExMmA(uriparser_uri, ZSTR_VAL(original_uri_str), ZSTR_VAL(original_uri_str) + ZSTR_LEN(original_uri_str), NULL, &uriparser_memory_manager) != URI_SUCCESS
	) {
		zend_string_release(original_uri_str);
		efree(uriparser_uri);
		return NULL;
	}

	if (base_url == NULL) {
		return uriparser_create_uris(uriparser_uri, original_uri_str, NULL, NULL);
	}

	uriparser_uris_t *uriparser_base_urls = (uriparser_uris_t *) base_url;
	UriUriA *uriparser_base_url = uriparser_copy_uri(uriparser_base_urls->uri);

	UriUriA *absolute_uri = emalloc(sizeof(UriUriA));

	if (uriAddBaseUriExMmA(absolute_uri, uriparser_uri, uriparser_base_url, URI_RESOLVE_STRICTLY, &uriparser_memory_manager) != URI_SUCCESS) {
		zend_string_release(original_uri_str);
		uriFreeUriMembersA(uriparser_uri);
		uriFreeUriMembersA(uriparser_base_url);
		efree(uriparser_uri);
		efree(uriparser_base_url);
		efree(absolute_uri);

		return NULL;
	}

	efree(uriparser_uri);
	efree(uriparser_base_url);

	return uriparser_create_uris(absolute_uri, original_uri_str, NULL, NULL);
}

static void uriparser_create_invalid_uri_exception(zval *exception_zv, zval *errors)
{
	object_init_ex(exception_zv, invalid_uri_exception_ce);

	zend_update_property_string(
		invalid_uri_exception_ce,
		Z_OBJ_P(exception_zv),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_MESSAGE)),
		ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_MESSAGE)),
		"URI parsing failed"
	);
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
		zend_string_release(uri_string);
		return NULL;
	}

	if (exclude_fragment) {
		char *pos = strrchr(ZSTR_VAL(uri_string), '#');
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

		uriFreeUriMembersMmA(uriparser_uris->uri, &uriparser_memory_manager);
		efree(uriparser_uris->uri);
		uriparser_uris->uri = NULL;
	}

	if (uriparser_uris->normalized_uri != NULL) {
		ZEND_ASSERT(uriparser_uris->normalized_uri->owner);
		if (uriparser_uris->normalized_uri_str != NULL) {
			zend_string_release(uriparser_uris->normalized_uri_str);
			uriparser_uris->normalized_uri_str = NULL;
		}

		uriFreeUriMembersMmA(uriparser_uris->normalized_uri, &uriparser_memory_manager);
		efree(uriparser_uris->normalized_uri);
		uriparser_uris->normalized_uri = NULL;
	}

	efree(uriparser_uris);
}

static zend_result uriparser_destroy_parser(void)
{
	zend_hash_destroy(&uriparser_property_handlers);

	return SUCCESS;
}
