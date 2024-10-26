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
static void *uriparser_parse_uri(const zend_string *uri_str, const zend_string *base_uri_str, zval *errors);
static zend_class_entry *uriparser_get_uri_ce(void);
static void *uriparser_clone_uri(void *uri);
static zend_string *uriparser_uri_to_string(void *uri);
static void uriparser_free_uri(void *uri);
static zend_result uriparser_destroy_parser(void);

HashTable uriparser_property_handlers;

const uri_handler_t uriparser_uri_handler = {
	"rfc3986",
	uriparser_init_parser,
	uriparser_parse_uri,
	uriparser_get_uri_ce,
	uriparser_clone_uri,
	uriparser_uri_to_string,
	uriparser_free_uri,
	uriparser_destroy_parser,
	&uriparser_property_handlers
};

typedef enum {
	URIPARSER_APPEND_AFTER_SCHEME,
	URIPARSER_APPEND_AFTER_AUTHORITY,
	URIPARSER_APPEND_AFTER_HOST,
	URIPARSER_APPEND_AFTER_PORT,
	URIPARSER_APPEND_AFTER_PATH,
	URIPARSER_APPEND_AFTER_QUERY,
} uriparser_append_after_component;

#define URIPARSER_PARSE_STR(uri_str, internal_uri, uriparser_uri, errors) do { \
    zend_string *str = smart_str_extract(&uri_str); \
	UriUriA *new_uriparser_uri = uriparser_parse_uri(str, NULL, errors); \
	if (new_uriparser_uri == NULL) { \
		smart_str_free(&uri_str); \
		zend_string_free(str); \
		return FAILURE; \
	} \
	uriparser_free_uri(uriparser_uri); \
	internal_uri->uri = (void *) new_uriparser_uri; \
    smart_str_free(&uri_str); \
	return SUCCESS; \
} while (0)

static void uriparser_append_rest(UriUriA *uri, smart_str *uri_str, uriparser_append_after_component start,
	unsigned short int scheme_offset, unsigned short int authority_offset
) {
	if (start <= URIPARSER_APPEND_AFTER_SCHEME && uri->scheme.afterLast != NULL) {
		smart_str_appendl(uri_str, uri->scheme.afterLast + scheme_offset, strlen(uri->scheme.afterLast + scheme_offset));
	} else if (start <= URIPARSER_APPEND_AFTER_SCHEME && uri->userInfo.first != NULL) {
		smart_str_appendl(uri_str, uri->userInfo.first, strlen(uri->userInfo.first));
	} else if (start <= URIPARSER_APPEND_AFTER_AUTHORITY && uri->userInfo.afterLast != NULL) {
		smart_str_appendl(uri_str, uri->userInfo.afterLast + authority_offset, strlen(uri->userInfo.afterLast + authority_offset));
	} else if (start <= URIPARSER_APPEND_AFTER_AUTHORITY && uri->hostText.first != NULL) {
		smart_str_appendl(uri_str, uri->hostText.first, strlen(uri->hostText.first));
	} else if (start <= URIPARSER_APPEND_AFTER_HOST && uri->hostText.afterLast != NULL) {
		smart_str_appendl(uri_str, uri->hostText.afterLast, strlen(uri->hostText.afterLast));
	} else if (start <= URIPARSER_APPEND_AFTER_HOST && uri->portText.first != NULL) {
		smart_str_appendl(uri_str, uri->portText.first, strlen(uri->portText.first));
	} else if (start <= URIPARSER_APPEND_AFTER_PORT && uri->portText.afterLast != NULL) {
		smart_str_appendl(uri_str, uri->portText.afterLast, strlen(uri->portText.afterLast));
	} else if (start <= URIPARSER_APPEND_AFTER_PORT && uri->pathHead != NULL && uri->pathHead->text.first != NULL) {
		if (uri->absolutePath) { // TODO add / for URLs
			smart_str_appendc(uri_str, '/');
		}
		smart_str_appendl(uri_str, uri->pathHead->text.first, strlen(uri->pathHead->text.first));
	} else if (start <= URIPARSER_APPEND_AFTER_PATH && uri->pathHead != NULL && uri->pathTail->text.afterLast != NULL) {
		smart_str_appendl(uri_str, uri->pathTail->text.afterLast, strlen(uri->pathTail->text.afterLast));
	} else if (start <= URIPARSER_APPEND_AFTER_PATH && uri->query.first != NULL) {
		smart_str_appendl(uri_str, uri->query.first, strlen(uri->query.first));
	} else if (start <= URIPARSER_APPEND_AFTER_QUERY && uri->query.afterLast != NULL) {
		smart_str_appendl(uri_str, uri->query.afterLast, strlen(uri->query.afterLast));
	} else if (start <= URIPARSER_APPEND_AFTER_QUERY && uri->fragment.first != NULL) {
		smart_str_appendl(uri_str, uri->fragment.first, strlen(uri->fragment.first));
	}
}

static zend_result uriparser_read_scheme(const uri_internal_t *internal_uri, zval *retval)
{
	UriUriA *uriparser_uri = (UriUriA *) internal_uri->uri;

	if (uriparser_uri->scheme.first != NULL && uriparser_uri->scheme.afterLast != NULL) {
		ZVAL_STRINGL(retval, uriparser_uri->scheme.first, uriparser_uri->scheme.afterLast - uriparser_uri->scheme.first);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static void uriparser_append_scheme(const uri_internal_t *internal_uri, smart_str *uri_str)
{
	zval tmp;

	uriparser_read_scheme(internal_uri, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		smart_str_append(uri_str, Z_STR(tmp));
		smart_str_appendl(uri_str, "://", sizeof("://") - 1);
	}

	zval_ptr_dtor(&tmp);
}

static zend_result uriparser_write_scheme(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	UriUriA *uriparser_uri = (UriUriA *) internal_uri->uri;
	smart_str uri_str = {0};

	if (Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) > 0) {
		smart_str_append(&uri_str, Z_STR_P(value));
		smart_str_appends(&uri_str, "://");
	}

	unsigned short int offset = 0;
	if (uriparser_uri->scheme.first != NULL) {
		offset = 3;
	}

	uriparser_append_rest(uriparser_uri, &uri_str, URIPARSER_APPEND_AFTER_SCHEME, offset, 0);

	URIPARSER_PARSE_STR(uri_str, internal_uri, uriparser_uri, errors);
}

static zend_result uriparser_read_user(const uri_internal_t *internal_uri, zval *retval)
{
	UriUriA *uriparser_uri = (UriUriA *) internal_uri->uri;

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

static void uriparser_append_user(const uri_internal_t *internal_uri, smart_str *uri_str)
{
	zval tmp;

	uriparser_read_user(internal_uri, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		smart_str_append(uri_str, Z_STR(tmp));
	}

	zval_ptr_dtor(&tmp);
}

static zend_result uriparser_read_password(const uri_internal_t *internal_uri, zval *retval)
{
	UriUriA *uriparser_uri = (UriUriA *) internal_uri->uri;

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

static void uriparser_append_password(const uri_internal_t *internal_uri, smart_str *uri_str)
{
	zval tmp;

	uriparser_read_password(internal_uri, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		smart_str_append(uri_str, Z_STR(tmp));
	}

	zval_ptr_dtor(&tmp);
}

static void uriparser_append_authority(const uri_internal_t *internal_uri, smart_str *uri_str)
{
	UriUriA *uriparser_uri = (UriUriA *) internal_uri->uri;

	if (uriparser_uri->userInfo.first != NULL && uriparser_uri->userInfo.afterLast != NULL && uriparser_uri->userInfo.afterLast - uriparser_uri->userInfo.first > 0) {
		smart_str_appendl(uri_str, uriparser_uri->userInfo.first, uriparser_uri->userInfo.afterLast - uriparser_uri->userInfo.first);
	}
}

static zend_result uriparser_write_user(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	UriUriA *uriparser_uri = (UriUriA *) internal_uri->uri;
	smart_str uri_str = {0};

	uriparser_append_scheme(internal_uri, &uri_str);

	if (Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) > 0) {
		smart_str_append(&uri_str, Z_STR_P(value));
		// TODO add :
		uriparser_append_password(internal_uri, &uri_str);
		smart_str_appends(&uri_str, "@");
	}

	unsigned short int offset = 0;
	if (uriparser_uri->userInfo.first != NULL) {
		offset = 1;
	}

	uriparser_append_rest(uriparser_uri, &uri_str, URIPARSER_APPEND_AFTER_AUTHORITY, 0, offset);

	URIPARSER_PARSE_STR(uri_str, internal_uri, uriparser_uri, errors);
}

static zend_result uriparser_write_password(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	UriUriA *uriparser_uri = (UriUriA *) internal_uri->uri;
	smart_str uri_str = {0};

	uriparser_append_scheme(internal_uri, &uri_str);
	uriparser_append_user(internal_uri, &uri_str);

	if (Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) > 0) {
		smart_str_appends(&uri_str, ":");
		smart_str_append(&uri_str, Z_STR_P(value));
		smart_str_appends(&uri_str, "@");
	}

	unsigned short int offset = 0;
	if (uriparser_uri->userInfo.first != NULL) {
		offset = 1;
	}

	uriparser_append_rest(uriparser_uri, &uri_str, URIPARSER_APPEND_AFTER_AUTHORITY, 0, offset);

	URIPARSER_PARSE_STR(uri_str, internal_uri, uriparser_uri, errors);
}

static zend_result uriparser_read_host(const uri_internal_t *internal_uri, zval *retval)
{
	UriUriA *uriparser_uri = (UriUriA *) internal_uri->uri;

	if (uriparser_uri->hostText.first != NULL && uriparser_uri->hostText.afterLast != NULL && uriparser_uri->hostText.afterLast - uriparser_uri->hostText.first > 0) {
		ZVAL_STRINGL(retval, uriparser_uri->hostText.first, uriparser_uri->hostText.afterLast - uriparser_uri->hostText.first);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static void uriparser_append_host(const uri_internal_t *internal_uri, smart_str *uri_str)
{
	zval tmp;

	uriparser_read_host(internal_uri, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		smart_str_append(uri_str, Z_STR(tmp));
	}

	zval_ptr_dtor(&tmp);
}

static zend_result uriparser_write_host(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	UriUriA *uriparser_uri = (UriUriA *) internal_uri->uri;

	smart_str uri_str = {0};
	uriparser_append_scheme(internal_uri, &uri_str);
	uriparser_append_authority(internal_uri, &uri_str);

	if (Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) > 0) {
		smart_str_append(&uri_str, Z_STR_P(value));
	}

	uriparser_append_rest(uriparser_uri, &uri_str, URIPARSER_APPEND_AFTER_HOST, 0, 0);

	URIPARSER_PARSE_STR(uri_str, internal_uri, uriparser_uri, errors);
}

static zend_result uriparser_read_port(const uri_internal_t *internal_uri, zval *retval)
{
	UriUriA *uriparser_uri = (UriUriA *) internal_uri->uri;

	if (uriparser_uri->portText.first != NULL && uriparser_uri->portText.afterLast != NULL) {
		ZVAL_LONG(retval, strtol(uriparser_uri->portText.first, NULL, 10));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static void uriparser_append_port(const uri_internal_t *internal_uri, smart_str *uri_str)
{
	zval tmp;

	uriparser_read_port(internal_uri, &tmp);
	if (Z_TYPE(tmp) == IS_LONG) {
		smart_str_appendc(uri_str, ':');
		smart_str_append_long(uri_str, Z_LVAL(tmp));
	}

	zval_ptr_dtor(&tmp);
}

static zend_result uriparser_write_port(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	UriUriA *uriparser_uri = (UriUriA *) internal_uri->uri;

	smart_str uri_str = {0};
	uriparser_append_scheme(internal_uri, &uri_str);
	uriparser_append_authority(internal_uri, &uri_str);
	uriparser_append_host(internal_uri, &uri_str);

	if (Z_TYPE_P(value) == IS_LONG && Z_LVAL_P(value) != 0) {
		smart_str_appendc(&uri_str, ':');
		smart_str_append_long(&uri_str, Z_LVAL_P(value));
	}

	uriparser_append_rest(uriparser_uri, &uri_str, URIPARSER_APPEND_AFTER_PORT, 0, 0);

	URIPARSER_PARSE_STR(uri_str, internal_uri, uriparser_uri, errors);
}

static zend_result uriparser_read_path(const uri_internal_t *internal_uri, zval *retval)
{
	UriUriA *uriparser_uri = (UriUriA *) internal_uri->uri;

	if (uriparser_uri->pathHead != NULL) {
		const UriPathSegmentA *p;
		smart_str str = {0};

		smart_str_appendl(&str, uriparser_uri->pathHead->text.first, (int) ((uriparser_uri->pathHead->text).afterLast - (uriparser_uri->pathHead->text).first));

		for (p = uriparser_uri->pathHead->next; p; p = p->next) {
			smart_str_appends(&str, "/");
			smart_str_appendl(&str, p->text.first, (int) ((p->text).afterLast - (p->text).first));
		}

		ZVAL_STR(retval, smart_str_extract(&str));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static void uriparser_append_path(const uri_internal_t *internal_uri, smart_str *uri_str)
{
	zval tmp;

	uriparser_read_path(internal_uri, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		smart_str_append(uri_str, Z_STR(tmp));
	}

	zval_ptr_dtor(&tmp);
}

static zend_result uriparser_write_path(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	UriUriA *uriparser_uri = (UriUriA *) internal_uri->uri;

	smart_str uri_str = {0};
	uriparser_append_scheme(internal_uri, &uri_str);
	uriparser_append_authority(internal_uri, &uri_str);
	uriparser_append_host(internal_uri, &uri_str);
	uriparser_append_port(internal_uri, &uri_str);

	if (Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) > 0) {
		smart_str_append(&uri_str, Z_STR_P(value));
	}

	uriparser_append_rest(uriparser_uri, &uri_str, URIPARSER_APPEND_AFTER_PATH, 0, 0);

	URIPARSER_PARSE_STR(uri_str, internal_uri, uriparser_uri, errors);
}

static zend_result uriparser_read_query(const uri_internal_t *internal_uri, zval *retval)
{
	UriUriA *uriparser_uri = (UriUriA *) internal_uri->uri;

	if (uriparser_uri->query.first != NULL && uriparser_uri->query.afterLast != NULL) {
		ZVAL_STRINGL(retval, uriparser_uri->query.first, uriparser_uri->query.afterLast - uriparser_uri->query.first);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static void uriparser_append_query(const uri_internal_t *internal_uri, smart_str *uri_str)
{
	zval tmp;

	uriparser_read_query(internal_uri, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		smart_str_appendc(uri_str, '?');
		smart_str_append(uri_str, Z_STR(tmp));
	}

	zval_ptr_dtor(&tmp);
}

static zend_result uriparser_write_query(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	UriUriA *uriparser_uri = (UriUriA *) internal_uri->uri;

	smart_str uri_str = {0};
	uriparser_append_scheme(internal_uri, &uri_str);
	uriparser_append_authority(internal_uri, &uri_str);
	uriparser_append_host(internal_uri, &uri_str);
	uriparser_append_port(internal_uri, &uri_str);
	uriparser_append_path(internal_uri, &uri_str);

	if (Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) > 0) {
		smart_str_append(&uri_str, Z_STR_P(value));
	}

	uriparser_append_rest(uriparser_uri, &uri_str, URIPARSER_APPEND_AFTER_QUERY, 0, 0);

	URIPARSER_PARSE_STR(uri_str, internal_uri, uriparser_uri, errors);
}

static zend_result uriparser_read_fragment(const uri_internal_t *internal_uri, zval *retval)
{
	UriUriA *uriparser_uri = (UriUriA *) internal_uri->uri;

	if (uriparser_uri->fragment.first != NULL && uriparser_uri->fragment.afterLast != NULL) {
		ZVAL_STRINGL(retval, uriparser_uri->fragment.first, uriparser_uri->fragment.afterLast - uriparser_uri->fragment.first);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_write_fragment(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	UriUriA *uriparser_uri = (UriUriA *) internal_uri->uri;

	smart_str uri_str = {0};
	uriparser_append_scheme(internal_uri, &uri_str);
	uriparser_append_authority(internal_uri, &uri_str);
	uriparser_append_host(internal_uri, &uri_str);
	uriparser_append_port(internal_uri, &uri_str);
	uriparser_append_path(internal_uri, &uri_str);
	uriparser_append_query(internal_uri, &uri_str);

	if (Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) > 0) {
		smart_str_append(&uri_str, Z_STR_P(value));
	}

	URIPARSER_PARSE_STR(uri_str, internal_uri, uriparser_uri, errors);
}

static zend_result uriparser_init_parser(void)
{
	zend_hash_init(&uriparser_property_handlers, 8, NULL, NULL, true);

	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_SCHEME), uriparser_read_scheme, uriparser_write_scheme);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_USER), uriparser_read_user, uriparser_write_user);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_PASSWORD), uriparser_read_password, uriparser_write_password);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_HOST), uriparser_read_host, uriparser_write_host);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_PORT), uriparser_read_port, uriparser_write_port);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_PATH), uriparser_read_path, uriparser_write_path);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_QUERY), uriparser_read_query, uriparser_write_query);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_FRAGMENT), uriparser_read_fragment, uriparser_write_fragment);

	return SUCCESS;
}

static void *uriparser_parse_uri(const zend_string *uri_str, const zend_string *base_uri_str, zval *errors)
{
	UriUriA *uriparser_uri = emalloc(sizeof(UriUriA));

	if (ZSTR_LEN(uri_str) == 0 || uriParseSingleUriA(uriparser_uri, ZSTR_VAL(uri_str), NULL) != URI_SUCCESS) {
		efree(uriparser_uri);
		return NULL;
	}

	if (base_uri_str == NULL) {
		return uriparser_uri;
	}

	UriUriA *base_uri = emalloc(sizeof(UriUriA));

	if (ZSTR_LEN(base_uri_str) == 0 || uriParseSingleUriA(base_uri, ZSTR_VAL(base_uri_str), NULL) != URI_SUCCESS) {
		uriFreeUriMembersA(uriparser_uri);
		efree(uriparser_uri);
		efree(base_uri);

		return NULL;
	}

	UriUriA *absolute_uri = emalloc(sizeof(UriUriA));

	if (uriAddBaseUriA(absolute_uri, uriparser_uri, base_uri) != URI_SUCCESS) {
		uriFreeUriMembersA(uriparser_uri);
		uriFreeUriMembersA(base_uri);
		efree(uriparser_uri);
		efree(base_uri);
		efree(absolute_uri);

		return NULL;
	}

	uriFreeUriMembersA(base_uri);
	uriFreeUriMembersA(uriparser_uri);
	efree(base_uri);
	efree(uriparser_uri);

	return absolute_uri;
}

static zend_class_entry *uriparser_get_uri_ce(void)
{
	return rfc3986_uri_ce;
}

static void *uriparser_clone_uri(void *uri)
{
	UriUriA *uriparser_uri = (UriUriA *) uri;

	// TODO Implement this properly
	UriUriA *new_uriparser_uri = ecalloc(1, sizeof(UriUriA));
	memcpy(new_uriparser_uri, uriparser_uri, sizeof(UriUriA));

	return new_uriparser_uri;
}

static zend_string *uriparser_uri_to_string(void *uri)
{
	UriUriA *uriparser_uri = (UriUriA *) uri;
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

	return uri_string;
}

static void uriparser_free_uri(void *uri)
{
	UriUriA *uriparser_uri = (UriUriA *) uri;

	uriFreeUriMembersA(uriparser_uri);
	efree(uriparser_uri);
}

static zend_result uriparser_destroy_parser(void)
{
	zend_hash_destroy(&uriparser_property_handlers);

	return SUCCESS;
}
