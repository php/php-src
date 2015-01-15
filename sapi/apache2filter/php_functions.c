/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

/* $Id$ */


#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS

#include "php.h"
#include "zend_smart_str.h"
#include "ext/standard/info.h"
#include "SAPI.h"

#define CORE_PRIVATE
#include "apr_strings.h"
#include "apr_time.h"
#include "ap_config.h"
#include "util_filter.h"
#include "httpd.h"
#include "http_config.h"
#include "http_request.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_log.h"
#include "http_main.h"
#include "util_script.h"
#include "http_core.h"

#include "php_apache.h"

static request_rec *php_apache_lookup_uri(char *filename)
{
	php_struct *ctx;

	if (!filename) {
		return NULL;
	}

	ctx = SG(server_context);
	return ap_sub_req_lookup_uri(filename, ctx->f->r, ctx->f->next);
}

/* {{{ proto bool virtual(string uri)
 Perform an apache sub-request */
PHP_FUNCTION(virtual)
{
	char *filename;
	int filename_len;
	request_rec *rr;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p", &filename, &filename_len) == FAILURE) {
		return;
	}

	if (!(rr = php_apache_lookup_uri(filename))) {
		php_error_docref(NULL, E_WARNING, "Unable to include '%s' - URI lookup failed", filename);
		RETURN_FALSE;
	}

	if (rr->status == HTTP_OK) {
		if (ap_run_sub_req(rr)) {
			php_error_docref(NULL, E_WARNING, "Unable to include '%s' - request execution failed", filename);
			ap_destroy_sub_req(rr);
			RETURN_FALSE;
		}
		ap_destroy_sub_req(rr);
		RETURN_TRUE;
	}

	php_error_docref(NULL, E_WARNING, "Unable to include '%s' - error finding URI", filename);
	ap_destroy_sub_req(rr);
	RETURN_FALSE;
}
/* }}} */

#define ADD_LONG(name) \
		add_property_long(return_value, #name, rr->name)
#define ADD_TIME(name) \
		add_property_long(return_value, #name, apr_time_sec(rr->name));
#define ADD_STRING(name) \
		if (rr->name) add_property_string(return_value, #name, (char *) rr->name)

PHP_FUNCTION(apache_lookup_uri)
{
	request_rec *rr;
	char *filename;
	int filename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p", &filename, &filename_len) == FAILURE) {
		return;
	}

	if (!(rr = php_apache_lookup_uri(filename))) {
		php_error_docref(NULL, E_WARNING, "Unable to include '%s' - URI lookup failed", filename);
		RETURN_FALSE;
	}

	if (rr->status == HTTP_OK) {
		object_init(return_value);

		ADD_LONG(status);
		ADD_STRING(the_request);
		ADD_STRING(status_line);
		ADD_STRING(method);
		ADD_TIME(mtime);
		ADD_LONG(clength);
#if MODULE_MAGIC_NUMBER < 20020506
		ADD_STRING(boundary);
#endif
		ADD_STRING(range);
		ADD_LONG(chunked);
		ADD_STRING(content_type);
		ADD_STRING(handler);
		ADD_LONG(no_cache);
		ADD_LONG(no_local_copy);
		ADD_STRING(unparsed_uri);
		ADD_STRING(uri);
		ADD_STRING(filename);
		ADD_STRING(path_info);
		ADD_STRING(args);
		ADD_LONG(allowed);
		ADD_LONG(sent_bodyct);
		ADD_LONG(bytes_sent);
		ADD_LONG(mtime);
		ADD_TIME(request_time);

		ap_destroy_sub_req(rr);
		return;
	}

	php_error_docref(NULL, E_WARNING, "Unable to include '%s' - error finding URI", filename);
	ap_destroy_sub_req(rr);
	RETURN_FALSE;
}

/* {{{ proto array getallheaders(void)
   Fetch all HTTP request headers */
PHP_FUNCTION(apache_request_headers)
{
	php_struct *ctx;
	const apr_array_header_t *arr;
	char *key, *val;

	array_init(return_value);

	ctx = SG(server_context);
	arr = apr_table_elts(ctx->f->r->headers_in);

	APR_ARRAY_FOREACH_OPEN(arr, key, val)
		if (!val) val = "";
		add_assoc_string(return_value, key, val);
	APR_ARRAY_FOREACH_CLOSE()
}
/* }}} */

/* {{{ proto array apache_response_headers(void)
   Fetch all HTTP response headers */
PHP_FUNCTION(apache_response_headers)
{
	php_struct *ctx;
	const apr_array_header_t *arr;
	char *key, *val;

	array_init(return_value);

	ctx = SG(server_context);
	arr = apr_table_elts(ctx->f->r->headers_out);

	APR_ARRAY_FOREACH_OPEN(arr, key, val)
		if (!val) val = "";
		add_assoc_string(return_value, key, val);
	APR_ARRAY_FOREACH_CLOSE()
}
/* }}} */

/* {{{ proto string apache_note(string note_name [, string note_value])
   Get and set Apache request notes */
PHP_FUNCTION(apache_note)
{
	php_struct *ctx;
	char *note_name, *note_val = NULL;
	int note_name_len, note_val_len;
	char *old_note_val=NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s", &note_name, &note_name_len, &note_val, &note_val_len) == FAILURE) {
		return;
	}

	ctx = SG(server_context);

	old_note_val = (char *) apr_table_get(ctx->r->notes, note_name);

	if (note_val) {
		apr_table_set(ctx->r->notes, note_name, note_val);
	}

	if (old_note_val) {
		RETURN_STRING(old_note_val, 1);
	}

	RETURN_FALSE;
}
/* }}} */


/* {{{ proto bool apache_setenv(string variable, string value [, bool walk_to_top])
   Set an Apache subprocess_env variable */
PHP_FUNCTION(apache_setenv)
{
	php_struct *ctx;
	char *variable=NULL, *string_val=NULL;
	int variable_len, string_val_len;
	zend_bool walk_to_top = 0;
	int arg_count = ZEND_NUM_ARGS();

	if (zend_parse_parameters(arg_count, "ss|b", &variable, &variable_len, &string_val, &string_val_len, &walk_to_top) == FAILURE) {
		return;
	}

	ctx = SG(server_context);

	if (arg_count == 3 && walk_to_top) {
		while(ctx->f->r->prev) {
			ctx->f->r = ctx->f->r->prev;
		}
	}

	apr_table_set(ctx->r->subprocess_env, variable, string_val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool apache_getenv(string variable [, bool walk_to_top])
   Get an Apache subprocess_env variable */
PHP_FUNCTION(apache_getenv)
{
	php_struct *ctx;
	char *variable=NULL;
	int variable_len;
	zend_bool walk_to_top = 0;
	int arg_count = ZEND_NUM_ARGS();
	char *env_val=NULL;

	if (zend_parse_parameters(arg_count, "s|b", &variable, &variable_len, &walk_to_top) == FAILURE) {
		return;
	}

	ctx = SG(server_context);

	if (arg_count == 2 && walk_to_top) {
		while(ctx->f->r->prev) {
			ctx->f->r = ctx->f->r->prev;
		}
	}

	env_val = (char*) apr_table_get(ctx->r->subprocess_env, variable);
	if (env_val != NULL) {
		RETURN_STRING(env_val, 1);
	}

	RETURN_FALSE;
}
/* }}} */

static char *php_apache_get_version()
{
#if MODULE_MAGIC_NUMBER_MAJOR >= 20060905
	return (char *) ap_get_server_banner();
#else
	return (char *) ap_get_server_version();
#endif
}

/* {{{ proto string apache_get_version(void)
   Fetch Apache version */
PHP_FUNCTION(apache_get_version)
{
	char *apv = php_apache_get_version();

	if (apv && *apv) {
		RETURN_STRING(apv, 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array apache_get_modules(void)
   Get a list of loaded Apache modules */
PHP_FUNCTION(apache_get_modules)
{
	int n;
	char *p;

	array_init(return_value);

	for (n = 0; ap_loaded_modules[n]; ++n) {
		char *s = (char *) ap_loaded_modules[n]->name;
		if ((p = strchr(s, '.'))) {
			add_next_index_stringl(return_value, s, (p - s));
		} else {
			add_next_index_string(return_value, s);
		}
	}
}
/* }}} */

PHP_MINFO_FUNCTION(apache)
{
	char *apv = php_apache_get_version();
	smart_str tmp1 = {0};
	int n;
	char *p;

	for (n = 0; ap_loaded_modules[n]; ++n) {
		char *s = (char *) ap_loaded_modules[n]->name;
		if ((p = strchr(s, '.'))) {
			smart_str_appendl(&tmp1, s, (p - s));
		} else {
			smart_str_appends(&tmp1, s);
		}
		smart_str_appendc(&tmp1, ' ');
	}
	if ((tmp1.len - 1) >= 0) {
		tmp1.c[tmp1.len - 1] = '\0';
	}

	php_info_print_table_start();
	if (apv && *apv) {
		php_info_print_table_row(2, "Apache Version", apv);
	}
	php_info_print_table_row(2, "Loaded Modules", tmp1.c);
	smart_str_free(&tmp1);
	php_info_print_table_end();
}

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_apache2filter_lookup_uri, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_apache2filter_virtual, 0, 0, 1)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_apache2filter_getallheaders, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_apache2filter_response_headers, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_apache2filter_note, 0, 0, 1)
	ZEND_ARG_INFO(0, note_name)
	ZEND_ARG_INFO(0, note_value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_apache2filter_setenv, 0, 0, 2)
	ZEND_ARG_INFO(0, variable)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, walk_to_top)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_apache2filter_getenv, 0, 0, 1)
	ZEND_ARG_INFO(0, variable)
	ZEND_ARG_INFO(0, walk_to_top)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_apache2filter_get_version, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_apache2filter_get_modules, 0)
ZEND_END_ARG_INFO()
/* }}} */

static const zend_function_entry apache_functions[] = {
	PHP_FE(apache_lookup_uri, 		arginfo_apache2filter_lookup_uri)
	PHP_FE(virtual, 				arginfo_apache2filter_virtual)
	PHP_FE(apache_request_headers, 	arginfo_apache2filter_getallheaders)
	PHP_FE(apache_response_headers, arginfo_apache2filter_response_headers)
	PHP_FE(apache_setenv, 			arginfo_apache2filter_setenv)
	PHP_FE(apache_getenv, 			arginfo_apache2filter_getenv)
	PHP_FE(apache_note, 			arginfo_apache2filter_note)
	PHP_FE(apache_get_version, 		arginfo_apache2filter_get_version)
	PHP_FE(apache_get_modules, 		arginfo_apache2filter_get_modules)
	PHP_FALIAS(getallheaders, 		apache_request_headers, arginfo_apache2filter_getallheaders)
	{NULL, NULL, NULL}
};

zend_module_entry php_apache_module = {
	STANDARD_MODULE_HEADER,
	"apache2filter",
	apache_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(apache),
	NULL,
	STANDARD_MODULE_PROPERTIES
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
