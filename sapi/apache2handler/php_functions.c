/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "ext/standard/php_smart_str.h"
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

static request_rec *php_apache_lookup_uri(char *filename TSRMLS_DC)
{
	php_struct *ctx;
	
	if (!filename) {
		return NULL;
	}
	
	ctx = SG(server_context);
	return ap_sub_req_lookup_uri(filename, ctx->r, ctx->r->output_filters);
}

/*  proto bool virtual(string uri)
 Perform an apache sub-request */
PHP_FUNCTION(virtual)
{
	zval **filename;
	request_rec *rr;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(filename);
	

	if (!(rr = php_apache_lookup_uri(Z_STRVAL_PP(filename) TSRMLS_CC))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include '%s' - URI lookup failed", Z_STRVAL_PP(filename));
		RETURN_FALSE;
	}

	if (rr->status != HTTP_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include '%s' - error finding URI", Z_STRVAL_PP(filename));
		ap_destroy_sub_req(rr);
		RETURN_FALSE;
	}

	/* Flush everything. */
	php_end_ob_buffers(1 TSRMLS_CC);
	php_header();

	if (ap_run_sub_req(rr)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include '%s' - request execution failed", Z_STRVAL_PP(filename));
		ap_destroy_sub_req(rr);
		RETURN_FALSE;
	}
	ap_destroy_sub_req(rr);
	RETURN_TRUE;
}
/*  */

#define ADD_LONG(name) \
		add_property_long(return_value, #name, rr->name)
#define ADD_TIME(name) \
		add_property_long(return_value, #name, rr->name / APR_USEC_PER_SEC);
#define ADD_STRING(name) \
		if (rr->name) add_property_string(return_value, #name, (char *) rr->name, 1)

PHP_FUNCTION(apache_lookup_uri)
{
	request_rec *rr;
	zval **filename;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(filename);
	
	if (!(rr = php_apache_lookup_uri(Z_STRVAL_PP(filename) TSRMLS_CC))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include '%s' - URI lookup failed", Z_STRVAL_PP(filename));
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
		ADD_LONG(request_time);
		ADD_LONG(mtime);
		ADD_TIME(request_time);

		ap_destroy_sub_req(rr);
		return;
	}
	
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include '%s' - error finding URI", Z_STRVAL_PP(filename));
	ap_destroy_sub_req(rr);
	RETURN_FALSE;
}

/*  proto array getallheaders(void)
   Fetch all HTTP request headers */
PHP_FUNCTION(apache_request_headers)
{
	php_struct *ctx;
	const apr_array_header_t *arr;
	char *key, *val;

	array_init(return_value);
	
	ctx = SG(server_context);
	arr = apr_table_elts(ctx->r->headers_in);

	APR_ARRAY_FOREACH_OPEN(arr, key, val)
		if (!val) val = empty_string;
		add_assoc_string(return_value, key, val, 1);
	APR_ARRAY_FOREACH_CLOSE()
}
/*  */

/* {{{ proto array apache_response_headers(void)
   Fetch all HTTP response headers */
PHP_FUNCTION(apache_response_headers)
{
	php_struct *ctx;
	const apr_array_header_t *arr;
	char *key, *val;

	array_init(return_value);
	
	ctx = SG(server_context);
	arr = apr_table_elts(ctx->r->headers_out);

	APR_ARRAY_FOREACH_OPEN(arr, key, val)
		if (!val) val = empty_string;
		add_assoc_string(return_value, key, val, 1);
	APR_ARRAY_FOREACH_CLOSE()
}
/* }}} */

/* {{{ proto string apache_note(string note_name [, string note_value])
   Get and set Apache request notes */
PHP_FUNCTION(apache_note)
{
	php_struct *ctx;
	zval **note_name, **note_val;
	char *old_note_val=NULL;
	int arg_count = ZEND_NUM_ARGS();

	if (arg_count<1 || arg_count>2 ||
		zend_get_parameters_ex(arg_count, &note_name, &note_val) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ctx = SG(server_context);
	
	convert_to_string_ex(note_name);

	old_note_val = (char *) apr_table_get(ctx->r->notes, Z_STRVAL_PP(note_name));
	
	if (arg_count == 2) {
		convert_to_string_ex(note_val);
		apr_table_set(ctx->r->notes, Z_STRVAL_PP(note_name), Z_STRVAL_PP(note_val));
	}

	if (old_note_val) {
		RETURN_STRING(old_note_val, 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto bool apache_setenv(string variable, string value [, bool walk_to_top])
   Set an Apache subprocess_env variable */
/*
 * XXX this doesn't look right. shouldn't it be the parent ?*/
PHP_FUNCTION(apache_setenv)
{
	php_struct *ctx;
	zval **variable=NULL, **string_val=NULL, **walk_to_top=NULL;
	int arg_count = ZEND_NUM_ARGS();
    request_rec *r;

	if (arg_count<1 || arg_count>3 ||
		zend_get_parameters_ex(arg_count, &variable, &string_val, &walk_to_top) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ctx = SG(server_context);

	if (arg_count == 3 && Z_STRVAL_PP(walk_to_top)) {
        r = ctx->r;
		while(r->prev) {
			r = r->prev;
		}	
	}

	convert_to_string_ex(variable);
	convert_to_string_ex(string_val);
	
	apr_table_set(r->subprocess_env, Z_STRVAL_PP(variable), Z_STRVAL_PP(string_val));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool apache_getenv(string variable [, bool walk_to_top])
   Get an Apache subprocess_env variable */
/*
 * XXX: shouldn't this be the parent not the 'prev'
 */
PHP_FUNCTION(apache_getenv)
{
	php_struct *ctx;
	zval **variable=NULL, **walk_to_top=NULL;
	int arg_count = ZEND_NUM_ARGS();
	char *env_val=NULL;
    request_rec *r;

	if (arg_count<1 || arg_count>2 ||
		zend_get_parameters_ex(arg_count, &variable, &walk_to_top) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ctx = SG(server_context);

    r = ctx->r;
	if (arg_count == 2 && Z_STRVAL_PP(walk_to_top)) {
		while(r->prev) {
			r = r->prev;
		}	
	}

	convert_to_string_ex(variable);
	
	env_val = (char*) apr_table_get(r->subprocess_env, Z_STRVAL_PP(variable));
	if (env_val != NULL) {
		RETURN_STRING(env_val, 1);
	} else { 
		RETURN_FALSE;
	}	
}
/* }}} */

static char *php_apache_get_version()
{
	return (char *) ap_get_server_version();
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
			add_next_index_stringl(return_value, s, (p - s), 1);
		} else {
			add_next_index_string(return_value, s, 1);
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

static function_entry apache_functions[] = {
	PHP_FE(apache_lookup_uri, NULL)
	PHP_FE(virtual, NULL) 
	PHP_FE(apache_request_headers, NULL)
	PHP_FE(apache_response_headers, NULL)
	PHP_FE(apache_setenv, NULL)
	PHP_FE(apache_getenv, NULL)
	PHP_FE(apache_note, NULL)
	PHP_FE(apache_get_version, NULL)
	PHP_FE(apache_get_modules, NULL)
	PHP_FALIAS(getallheaders, apache_request_headers, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry php_apache_module = {
	STANDARD_MODULE_HEADER,
	"Apache 2.0",
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
