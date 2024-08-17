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

HashTable uriparser_property_handlers;

const uri_handler_t uriparser_uri_handler = {
	"rfc3986",
	uriparser_init_parser,
	uriparser_parse_uri,
	uriparser_instantiate_uri,
	uriparser_uri_to_string,
	uriparser_free_uri,
	uriparser_destroy_parser,
	&uriparser_property_handlers
};

static zend_result uriparser_read_scheme(void *uri_object_internal, zval *retval)
{
	UriUriA *uri = (UriUriA *) uri_object_internal;

	if (uri->scheme.first != NULL && uri->scheme.afterLast != NULL) {
		ZVAL_STRINGL(retval, uri->scheme.first, uri->scheme.afterLast - uri->scheme.first);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_read_user(void *uri_object_internal, zval *retval)
{
	UriUriA *uri = (UriUriA *) uri_object_internal;

	if (uri->userInfo.first != NULL && uri->userInfo.afterLast != NULL) {
		char *c = strchr(uri->userInfo.first, ':');
		if (c != NULL) {
			ZVAL_STRINGL(retval, uri->userInfo.first, c - uri->userInfo.first);
		} else {
			ZVAL_STRINGL(retval, uri->userInfo.first, uri->userInfo.afterLast - uri->userInfo.first);
		}
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_read_password(void *uri_object_internal, zval *retval)
{
	UriUriA *uri = (UriUriA *) uri_object_internal;

	if (uri->userInfo.first != NULL && uri->userInfo.afterLast != NULL) {
		char *c = strchr(uri->userInfo.first, ':');
		if (c != NULL) {
			ZVAL_STRINGL(retval, c + 1, uri->userInfo.afterLast - c - 1);
		} else {
			ZVAL_NULL(retval);
		}
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_read_host(void *uri_object_internal, zval *retval)
{
	UriUriA *uri = (UriUriA *) uri_object_internal;

	if (uri->hostText.first != NULL && uri->hostText.afterLast != NULL) {
		ZVAL_STRINGL(retval, uri->hostText.first, uri->hostText.afterLast - uri->hostText.first);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_read_port(void *uri_object_internal, zval *retval)
{
	UriUriA *uri = (UriUriA *) uri_object_internal;

	if (uri->portText.first != NULL && uri->portText.afterLast != NULL) {
		ZVAL_LONG(retval, strtol(uri->portText.first, NULL, 10));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_read_path(void *uri_object_internal, zval *retval)
{
	UriUriA *uri = (UriUriA *) uri_object_internal;

	if (uri->pathHead != NULL) {
		const UriPathSegmentA *p;
		smart_str str = {0};

		smart_str_appendl(&str, uri->pathHead->text.first, (int) ((uri->pathHead->text).afterLast - (uri->pathHead->text).first));

		for (p = uri->pathHead->next; p; p = p->next) {
			smart_str_appends(&str, "/");
			smart_str_appendl(&str, p->text.first, (int) ((p->text).afterLast - (p->text).first));
		}

		ZVAL_STR(retval, smart_str_extract(&str));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_read_query(void *uri_object_internal, zval *retval)
{
	UriUriA *uri = (UriUriA *) uri_object_internal;

	if (uri->query.first != NULL && uri->query.afterLast != NULL) {
		ZVAL_STRINGL(retval, uri->query.first, uri->query.afterLast - uri->query.first);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uriparser_read_fragment(void *uri_object_internal, zval *retval)
{
	UriUriA *uri = (UriUriA *) uri_object_internal;

	if (uri->fragment.first != NULL && uri->fragment.afterLast != NULL) {
		ZVAL_STRINGL(retval, uri->fragment.first, uri->fragment.afterLast - uri->fragment.first);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

zend_result uriparser_init_parser(void)
{
	zend_hash_init(&uriparser_property_handlers, 0, NULL, NULL, true);

	URI_REGISTER_PROPERTY_READ_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_SCHEME), uriparser_read_scheme);
	URI_REGISTER_PROPERTY_READ_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_USER), uriparser_read_user);
	URI_REGISTER_PROPERTY_READ_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_PASSWORD), uriparser_read_password);
	URI_REGISTER_PROPERTY_READ_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_HOST), uriparser_read_host);
	URI_REGISTER_PROPERTY_READ_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_PORT), uriparser_read_port);
	URI_REGISTER_PROPERTY_READ_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_PATH), uriparser_read_path);
	URI_REGISTER_PROPERTY_READ_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_QUERY), uriparser_read_query);
	URI_REGISTER_PROPERTY_READ_HANDLER(&uriparser_property_handlers, ZSTR_KNOWN(ZEND_STR_FRAGMENT), uriparser_read_fragment);

	return SUCCESS;
}

void *uriparser_parse_uri(const zend_string *uri_str, const zend_string *base_uri_str, zval *errors)
{
	UriUriA *uri = emalloc(sizeof(UriUriA));

	if (ZSTR_LEN(uri_str) == 0 || uriParseSingleUriA(uri, ZSTR_VAL(uri_str), NULL) != URI_SUCCESS) {
		efree(uri);
		return NULL;
	}

	if (base_uri_str == NULL) {
		return uri;
	} else {
		UriUriA *base_uri = emalloc(sizeof(UriUriA));

		if (ZSTR_LEN(base_uri_str) == 0 || uriParseSingleUriA(base_uri, ZSTR_VAL(base_uri_str), NULL) != URI_SUCCESS) {
			uriFreeUriMembersA(uri);
			efree(uri);
			efree(base_uri);
			return NULL;
		}

		UriUriA *absolute_uri = emalloc(sizeof(UriUriA));

		if (uriAddBaseUriA(absolute_uri, uri, base_uri) != URI_SUCCESS) {
			uriFreeUriMembersA(uri);
			uriFreeUriMembersA(base_uri);
			efree(uri);
			efree(base_uri);
			efree(absolute_uri);
			return NULL;
		}

		uriFreeUriMembersA(base_uri);
		uriFreeUriMembersA(uri);
		efree(base_uri);
		efree(uri);
		return absolute_uri;
	}
}

void uriparser_instantiate_uri(zval *zv)
{
	object_init_ex(zv, rfc3986_uri_ce);
}

zend_string *uriparser_uri_to_string(void *uri_object_internal)
{
	UriUriA *uri = (UriUriA *) uri_object_internal;
	int charsRequired;

	if (uriToStringCharsRequiredA(uri, &charsRequired) != URI_SUCCESS) {
		return NULL;
	}
	charsRequired++;

	zend_string *uri_string = zend_string_alloc(charsRequired - 1, false);
	if (uriToStringA(ZSTR_VAL(uri_string), uri, charsRequired, NULL) != URI_SUCCESS) {
		zend_string_release(uri_string);
		return NULL;
	}

	return uri_string;
}

void uriparser_free_uri(void *uri_object_internal)
{
	UriUriA *uri = (UriUriA *) uri_object_internal;

	uriFreeUriMembersA(uri);
	efree(uri);
}

zend_result uriparser_destroy_parser(void)
{
	zend_hash_destroy(&uriparser_property_handlers);

	return SUCCESS;
}
