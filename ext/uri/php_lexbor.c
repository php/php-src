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

HashTable lexbor_property_handlers;

lxb_url_parser_t *lexbor_parser;
int lexbor_urls;

const uri_handler_t lexbor_uri_handler = {
	"whatwg",
	lexbor_init_parser,
	lexbor_parse_uri,
	lexbor_instantiate_uri,
	lexbor_to_string,
	lexbor_free_uri,
	lexbor_destroy_parser,
	&lexbor_property_handlers
};

static void lexbor_cleanup_parser(void)
{
	if (++lexbor_urls % 500 == 0) {
		lexbor_mraw_clean(lexbor_parser->mraw);
	}

	lxb_url_parser_clean(lexbor_parser);
}

static zend_result lexbor_read_scheme(void *uri_object_internal, zval *retval)
{
	lxb_url_t *lxb_url = (lxb_url_t *) uri_object_internal;

	if (lxb_url->scheme.type != LXB_URL_SCHEMEL_TYPE__UNDEF) {
		ZVAL_STRINGL(retval, (const char *) lxb_url->scheme.name.data, lxb_url->scheme.name.length);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result lexbor_read_user(void *uri_object_internal, zval *retval)
{
	lxb_url_t *lxb_url = (lxb_url_t *) uri_object_internal;

	if (lxb_url->username.length) {
		ZVAL_STRINGL(retval, (const char *) lxb_url->username.data, lxb_url->username.length);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result lexbor_read_password(void *uri_object_internal, zval *retval)
{
	lxb_url_t *lxb_url = (lxb_url_t *) uri_object_internal;

	if (lxb_url->password.length) {
		ZVAL_STRINGL(retval, (const char *) lxb_url->password.data, lxb_url->password.length);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result lexbor_read_host(void *uri_object_internal, zval *retval)
{
	lxb_url_t *lxb_url = (lxb_url_t *) uri_object_internal;

	if (lxb_url->host.type == LXB_URL_HOST_TYPE_IPV4) {
		char buffer[17] = {0};

		if (!inet_ntop(AF_INET, (const void *) &lxb_url->host.u.ipv4, buffer, sizeof(buffer))) {
			ZVAL_NULL(retval);
		}

		ZVAL_STRING(retval, buffer);
	} else if (lxb_url->host.type == LXB_URL_HOST_TYPE_IPV6) {
		char buffer[46] = {0};

		php_sprintf(buffer, "%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu", lxb_url->host.u.ipv6[0], lxb_url->host.u.ipv6[1],
			lxb_url->host.u.ipv6[2], lxb_url->host.u.ipv6[3], lxb_url->host.u.ipv6[4], lxb_url->host.u.ipv6[5],
			lxb_url->host.u.ipv6[6], lxb_url->host.u.ipv6[7]);

		ZVAL_STRING(retval, buffer);
	} else if (lxb_url->host.type != LXB_URL_HOST_TYPE_EMPTY && lxb_url->host.type != LXB_URL_HOST_TYPE__UNDEF) {
		ZVAL_STRINGL(retval, (const char *) lxb_url->host.u.domain.data, lxb_url->host.u.domain.length);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result lexbor_read_port(void *uri_object_internal, zval *retval)
{
	lxb_url_t *lxb_url = (lxb_url_t *) uri_object_internal;

	if (lxb_url->has_port && lxb_url->port != 0) {
		ZVAL_LONG(retval, lxb_url->port);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result lexbor_read_path(void *uri_object_internal, zval *retval)
{
	lxb_url_t *lxb_url = (lxb_url_t *) uri_object_internal;

	if (lxb_url->path.length) {
		ZVAL_STRINGL(retval, (const char *) lxb_url->path.str.data, lxb_url->path.str.length);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result lexbor_read_query(void *uri_object_internal, zval *retval)
{
	lxb_url_t *lxb_url = (lxb_url_t *) uri_object_internal;

	if (lxb_url->query.length) {
		ZVAL_STRINGL(retval, (const char *) lxb_url->query.data, lxb_url->query.length);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result lexbor_read_fragment(void *uri_object_internal, zval *retval)
{
	lxb_url_t *lxb_url = (lxb_url_t *) uri_object_internal;

	if (lxb_url->fragment.length) {
		ZVAL_STRINGL(retval, (const char *) lxb_url->fragment.data, lxb_url->fragment.length);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

zend_result lexbor_init_parser(void)
{
	lexbor_mraw_t *mraw = lexbor_mraw_create();
	lxb_status_t status = lexbor_mraw_init(mraw, 4096 * 2);
	if (status != LXB_STATUS_OK) {
		lexbor_mraw_destroy(mraw, true);
		return FAILURE;
	}

	lxb_url_parser_t *parser = lxb_url_parser_create();
	status = lxb_url_parser_init(parser, mraw);
	if (status != LXB_STATUS_OK) {
		lxb_url_parser_destroy(parser, true);
		return FAILURE;
	}

	lexbor_parser = parser;
	lexbor_urls = 0;

	zend_hash_init(&lexbor_property_handlers, 0, NULL, NULL, true);

	URI_REGISTER_PROPERTY_READ_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_SCHEME), lexbor_read_scheme);
	URI_REGISTER_PROPERTY_READ_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_USER), lexbor_read_user);
	URI_REGISTER_PROPERTY_READ_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_PASSWORD), lexbor_read_password);
	URI_REGISTER_PROPERTY_READ_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_HOST), lexbor_read_host);
	URI_REGISTER_PROPERTY_READ_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_PORT), lexbor_read_port);
	URI_REGISTER_PROPERTY_READ_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_PATH), lexbor_read_path);
	URI_REGISTER_PROPERTY_READ_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_QUERY), lexbor_read_query);
	URI_REGISTER_PROPERTY_READ_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_FRAGMENT), lexbor_read_fragment);

	return SUCCESS;
}

void fill_errors(zval *errors)
{
	if (!errors || lexbor_parser->log == NULL) {
		return;
	}

	zval errors_tmp;
	array_init(&errors_tmp);

	lexbor_plog_entry_t *lxb_error;
	while ((lxb_error = lexbor_array_obj_pop(&lexbor_parser->log->list)) != NULL) {
		zval error;
		object_init_ex(&error, whatwg_error_ce);
		zend_update_property_string(whatwg_error_ce, Z_OBJ(error), "position", sizeof("position") - 1, (const char *) lxb_error->data);
		zend_update_property_long(whatwg_error_ce, Z_OBJ(error), "errorCode", sizeof("errorCode") - 1, lxb_error->id);

		add_next_index_zval(&errors_tmp, &error);
	}

	ZEND_TRY_ASSIGN_REF_COPY(errors, &errors_tmp);
	zval_ptr_dtor(&errors_tmp);
}

void *lexbor_parse_uri(const zend_string *url_str, const zend_string *base_url_str, zval *errors)
{
	lexbor_cleanup_parser();

	lxb_url_t *url = NULL, *base_url = NULL;

	if (base_url_str) {
		if ((base_url = lxb_url_parse(lexbor_parser, NULL, (unsigned char *) ZSTR_VAL(base_url_str), ZSTR_LEN(base_url_str))) == NULL) {
			fill_errors(errors);
			return NULL;
		}

		base_url = lexbor_parser->url;
	}

	if ((url = lxb_url_parse(lexbor_parser, base_url, (unsigned char *) ZSTR_VAL(url_str), ZSTR_LEN(url_str))) == NULL) {
		fill_errors(errors);
		return NULL;
	}

	return url;
}

void lexbor_instantiate_uri(zval *zv)
{
	object_init_ex(zv, whatwg_uri_ce);
}

lxb_status_t lexbor_serialize_callback(const lxb_char_t *data, size_t length, void *ctx)
{
	smart_str *uri_str = (smart_str *) ctx;

	smart_str_appendl(uri_str, (const char *) data, length);

	return LXB_STATUS_OK;
}

zend_string *lexbor_to_string(void *uri_object_internal)
{
	lxb_url_t *lxb_url = (lxb_url_t *) uri_object_internal;
	smart_str uri_str = {0};

	lxb_url_serialize(lxb_url, lexbor_serialize_callback, (void *) &uri_str, false);

	return smart_str_extract(&uri_str);
}

void lexbor_free_uri(void *uri_object_internal)
{
	//lxb_url_t *url = (lxb_url_t *) uri_object_internal;
}

zend_result lexbor_destroy_parser(void)
{
	lxb_url_parser_memory_destroy(lexbor_parser);
	lxb_url_parser_destroy(lexbor_parser, true);

	lexbor_parser = NULL;
	lexbor_urls = 0;

	zend_hash_destroy(&lexbor_property_handlers);

	return SUCCESS;
}
