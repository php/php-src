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
#include "php_lexbor.h"
#include "php_uri_common.h"
#include "Zend/zend_enum.h"
#include "Zend/zend_smart_str.h"
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

lxb_url_parser_t *lexbor_init_parser(void)
{
	lexbor_mraw_t *mraw = lexbor_mraw_create();
	lxb_status_t status = lexbor_mraw_init(mraw, 4096 * 2);
	if (status != LXB_STATUS_OK) {
		lexbor_mraw_destroy(mraw, true);
		return NULL;
	}

	lxb_url_parser_t *parser = lxb_url_parser_create();
	status = lxb_url_parser_init(parser, mraw);
	if (status != LXB_STATUS_OK) {
		lxb_url_parser_destroy(parser, true);
		return NULL;
	}

	return parser;
}

void lexbor_destroy_parser(lxb_url_parser_t *parser)
{
	lxb_url_parser_memory_destroy(parser);
	lxb_url_parser_destroy(parser, true);
}

static void lexbor_cleanup_parser(void)
{
	if (++URI_G(lexbor_urls) % 500 == 0) {
		lexbor_mraw_clean(URI_G(lexbor_parser)->mraw);
	}

	lxb_url_parser_clean(URI_G(lexbor_parser));
}

static void lexbor_scheme_to_zval(lxb_url_t *lxb_url, zval *scheme)
{
	if (lxb_url->scheme.type != LXB_URL_SCHEMEL_TYPE__UNDEF &&
		lxb_url->scheme.type != LXB_URL_SCHEMEL_TYPE__UNKNOWN &&
		lxb_url->scheme.type != LXB_URL_SCHEMEL_TYPE__LAST_ENTRY
			) {
		ZVAL_STRINGL(scheme, (const char *) lxb_url->scheme.name.data, lxb_url->scheme.name.length);
	} else {
		ZVAL_NULL(scheme);
	}
}

static void lexbor_host_to_zval(lxb_url_t *lxb_url, zval *host)
{
	if (lxb_url->host.type == LXB_URL_HOST_TYPE_IPV4) {
		char buffer[17] = {0};

		if (!inet_ntop(AF_INET, (const void *) &lxb_url->host.u.ipv4, buffer, sizeof(buffer))) {
			ZVAL_NULL(host);
		}

		ZVAL_STRING(host, buffer);
	} else if (lxb_url->host.type == LXB_URL_HOST_TYPE_IPV6) {
		char buffer[40];

		if (!inet_ntop(AF_INET6, (const void *) &lxb_url->host.u.ipv6, buffer, sizeof(buffer))) {
			ZVAL_NULL(host);
		}

		ZVAL_STRING(host, buffer);
	} else if (lxb_url->host.type != LXB_URL_HOST_TYPE_EMPTY && lxb_url->host.type != LXB_URL_HOST_TYPE__UNDEF) {
		ZVAL_STRINGL(host, (const char *) lxb_url->host.u.domain.data, lxb_url->host.u.domain.length);
	} else {
		ZVAL_NULL(host);
	}
}

static void lexbor_port_to_zval(lxb_url_t *lxb_url, zval *port)
{
	if (lxb_url->has_port) {
		ZVAL_LONG(port, lxb_url->port);
	} else {
		ZVAL_NULL(port);
	}
}

static void lexbor_user_to_zval(lxb_url_t *lxb_url, zval *user)
{
	if (lxb_url->username.length) {
		ZVAL_STRINGL(user, (const char *) lxb_url->username.data, lxb_url->username.length);
	} else {
		ZVAL_NULL(user);
	}
}

static void lexbor_password_to_zval(lxb_url_t *lxb_url, zval *password)
{
	if (lxb_url->password.length) {
		ZVAL_STRINGL(password, (const char *) lxb_url->password.data, lxb_url->password.length);
	} else {
		ZVAL_NULL(password);
	}
}

static void lexbor_path_to_zval(lxb_url_t *lxb_url, zval *path)
{
	if (lxb_url->path.length) {
		smart_str str = {0};

		smart_str_appendl(&str, (const char *) lxb_url->path.list[0]->data, lxb_url->path.list[0]->length);
		for (int i = 1; i < lxb_url->path.length; i++) {
			smart_str_appends(&str, "/");
			smart_str_appendl(&str, (const char *) lxb_url->path.list[i]->data, lxb_url->path.list[i]->length);
		}

		ZVAL_STR(path, smart_str_extract(&str));
	} else {
		ZVAL_NULL(path);
	}
}

static void lexbor_query_string_to_zval(lxb_url_t *lxb_url, zval *query)
{
	if (lxb_url->query.length) {
		ZVAL_STRINGL(query, (const char *) lxb_url->query.data, lxb_url->query.length);
	} else {
		ZVAL_NULL(query);
	}
}

static void lexbor_fragment_to_zval(lxb_url_t *lxb_url, zval *fragment)
{
	if (lxb_url->fragment.length) {
		ZVAL_STRINGL(fragment, (const char *) lxb_url->fragment.data, lxb_url->fragment.length);
	} else {
		ZVAL_NULL(fragment);
	}
}

void fill_errors(zval *errors)
{
	if (!errors || URI_G(lexbor_parser)->log == NULL) {
		return;
	}

	zval errors_tmp;
	array_init(&errors_tmp);

	lexbor_plog_entry_t *lxb_error;
	while ((lxb_error = lexbor_array_obj_pop(&URI_G(lexbor_parser)->log->list)) != NULL) {
		zval error;
		object_init_ex(&error, whatwg_error_ce);
		zend_update_property_string(whatwg_error_ce, Z_OBJ(error), "position", sizeof("position") - 1, (const char *) lxb_error->data);
		zend_update_property_long(whatwg_error_ce, Z_OBJ(error), "errorCode", sizeof("errorCode") - 1, lxb_error->id);

		add_next_index_zval(&errors_tmp, &error);
	}

	ZEND_TRY_ASSIGN_REF_COPY(errors, &errors_tmp);
}

void lexbor_parse_url(zend_string *url_str, zend_string *base_url_str, zval *return_value, zval *errors)
{
	zval tmp;
	lxb_url_t *url = NULL, *base_url = NULL;

	if (base_url_str) {
		if ((base_url = lxb_url_parse(URI_G(lexbor_parser), NULL, (unsigned char *) ZSTR_VAL(base_url_str), ZSTR_LEN(base_url_str))) == NULL) {
			fill_errors(errors);
			RETURN_NULL();
		}

		base_url = URI_G(lexbor_parser)->url;
	}

	if ((url = lxb_url_parse(URI_G(lexbor_parser), base_url, (unsigned char *) ZSTR_VAL(url_str), ZSTR_LEN(url_str))) == NULL) {
		fill_errors(errors);
		RETURN_NULL();
	}

	object_init_ex(return_value, uri_ce);

	lexbor_scheme_to_zval(url, &tmp);
	if (uri_object_write_property(Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_SCHEME), &tmp) == FAILURE) {
		zval_ptr_dtor(&tmp);
		RETURN_THROWS();
	}

	lexbor_host_to_zval(url, &tmp);
	if (uri_object_write_property(Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_HOST), &tmp) == FAILURE) {
		zval_ptr_dtor(&tmp);
		RETURN_THROWS();
	}

	lexbor_port_to_zval(url, &tmp);
	if (uri_object_write_property(Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_PORT), &tmp) == FAILURE) {
		zval_ptr_dtor(&tmp);
		RETURN_THROWS();
	}

	lexbor_user_to_zval(url, &tmp);
	if (uri_object_write_property(Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_USER), &tmp) == FAILURE) {
		zval_ptr_dtor(&tmp);
		RETURN_THROWS();
	}

	lexbor_password_to_zval(url, &tmp);
	if (uri_object_write_property(Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_PASSWORD), &tmp) == FAILURE) {
		zval_ptr_dtor(&tmp);
		RETURN_THROWS();
	}

	lexbor_path_to_zval(url, &tmp);
	if (uri_object_write_property(Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_PATH), &tmp) == FAILURE) {
		zval_ptr_dtor(&tmp);
		RETURN_THROWS();
	}

	lexbor_query_string_to_zval(url, &tmp);
	if (uri_object_write_property(Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_QUERY), &tmp) == FAILURE) {
		zval_ptr_dtor(&tmp);
		RETURN_THROWS();
	}

	lexbor_fragment_to_zval(url, &tmp);
	if (uri_object_write_property(Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_FRAGMENT), &tmp) == FAILURE) {
		zval_ptr_dtor(&tmp);
		RETURN_THROWS();
	}

	lexbor_cleanup_parser();
}
