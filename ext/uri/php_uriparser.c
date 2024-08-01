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

static void uriparser_scheme_to_zval(UriUriA uri, zval *scheme)
{
	if (uri.scheme.first != NULL && uri.scheme.afterLast != NULL) {
		ZVAL_STRINGL(scheme, uri.scheme.first, uri.scheme.afterLast - uri.scheme.first);
	} else {
		ZVAL_NULL(scheme);
	}
}

static void uriparser_host_to_zval(UriUriA uri, zval *host)
{
	if (uri.hostText.first != NULL && uri.hostText.afterLast != NULL) {
		ZVAL_STRINGL(host, uri.hostText.first, uri.hostText.afterLast - uri.hostText.first);
	} else {
		ZVAL_NULL(host);
	}
}

static void uriparser_port_to_zval(UriUriA uri, zval *port)
{
	if (uri.portText.first != NULL && uri.portText.afterLast != NULL) {
		ZVAL_LONG(port, strtol(uri.portText.first, NULL, 10));
	} else {
		ZVAL_NULL(port);
	}
}

static void uriparser_userinfo_to_zval(UriUriA uri, zval *user, zval *password)
{
	if (uri.userInfo.first != NULL && uri.userInfo.afterLast != NULL) {
		char *c = strchr(uri.userInfo.first, ':');
		if (c != NULL) {
			ZVAL_STRINGL(user, uri.userInfo.first, c - uri.userInfo.first);
			ZVAL_STRINGL(password, c + 1, uri.userInfo.afterLast - c - 1);
		} else {
			ZVAL_STRINGL(user, uri.userInfo.first, uri.userInfo.afterLast - uri.userInfo.first);
			ZVAL_NULL(password);
		}
	} else {
		ZVAL_NULL(user);
		ZVAL_NULL(password);
	}
}

static void uriparser_path_to_zval(UriUriA uri, zval *user)
{
	if (uri.pathHead != NULL) {
		const UriPathSegmentA *p;
		smart_str str = {0};

		smart_str_appendl(&str, uri.pathHead->text.first, (int) ((uri.pathHead->text).afterLast - (uri.pathHead->text).first));

		for (p = uri.pathHead->next; p; p = p->next) {
			smart_str_appends(&str, "/");
			smart_str_appendl(&str, p->text.first, (int) ((p->text).afterLast - (p->text).first));
		}

		ZVAL_STR(user, smart_str_extract(&str));
	} else {
		ZVAL_NULL(user);
	}
}

static void uriparser_query_string_to_zval(UriUriA uri, zval *query)
{
	if (uri.query.first != NULL && uri.query.afterLast != NULL) {
		ZVAL_STRINGL(query, uri.query.first, uri.query.afterLast - uri.query.first);
	} else {
		ZVAL_NULL(query);
	}
}

static void uriparser_fragment_to_zval(UriUriA uri, zval *fragment)
{
	if (uri.fragment.first != NULL && uri.fragment.afterLast != NULL) {
		ZVAL_STRINGL(fragment, uri.fragment.first, uri.fragment.afterLast - uri.fragment.first);
	} else {
		ZVAL_NULL(fragment);
	}
}

void uriparser_parse_uri(zend_string *uri_str, zend_string *base_uri_str, zval *return_value)
{
	UriUriA uri, base_uri;
	const char *errorPos;
	zval tmp, tmp2;

	if (ZSTR_LEN(uri_str) == 0 || uriParseSingleUriA(&uri, ZSTR_VAL(uri_str), &errorPos) != URI_SUCCESS) {
		RETURN_NULL();
	}

	if (base_uri_str != NULL) {
		if (ZSTR_LEN(base_uri_str) == 0 || uriParseSingleUriA(&base_uri, ZSTR_VAL(base_uri_str), &errorPos) != URI_SUCCESS) {
			uriFreeUriMembersA(&uri);
			RETURN_NULL();
		}

		if (uriAddBaseUriA(&uri, &uri, &base_uri) != URI_SUCCESS) {
			uriFreeUriMembersA(&uri);
			RETURN_NULL();
		}
	}

	object_init_ex(return_value, uri_ce);

	uriparser_scheme_to_zval(uri, &tmp);
	if (uri_object_write_property(Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_SCHEME), &tmp) == FAILURE) {
		zval_ptr_dtor(&tmp);
		RETURN_THROWS();
	}

	uriparser_host_to_zval(uri, &tmp);
	if (uri_object_write_property(Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_HOST), &tmp) == FAILURE) {
		zval_ptr_dtor(&tmp);
		RETURN_THROWS();
	}

	uriparser_port_to_zval(uri, &tmp);
	if (uri_object_write_property(Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_PORT), &tmp) == FAILURE) {
		zval_ptr_dtor(&tmp);
		RETURN_THROWS();
	}

	uriparser_userinfo_to_zval(uri, &tmp, &tmp2);
	if (uri_object_write_property(Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_USER), &tmp) == FAILURE) {
		zval_ptr_dtor(&tmp);
		RETURN_THROWS();
	}

	if (uri_object_write_property(Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_PASSWORD), &tmp2) == FAILURE) {
		zval_ptr_dtor(&tmp2);
		RETURN_THROWS();
	}

	uriparser_path_to_zval(uri, &tmp);
	if (uri_object_write_property(Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_PATH), &tmp) == FAILURE) {
		zval_ptr_dtor(&tmp);
		RETURN_THROWS();
	}

	uriparser_query_string_to_zval(uri, &tmp);
	if (uri_object_write_property(Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_QUERY), &tmp) == FAILURE) {
		zval_ptr_dtor(&tmp);
		RETURN_THROWS();
	}

	uriparser_fragment_to_zval(uri, &tmp);
	if (uri_object_write_property(Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_FRAGMENT), &tmp) == FAILURE) {
		zval_ptr_dtor(&tmp);
		RETURN_THROWS();
	}

	uriFreeUriMembersA(&uri);
}
