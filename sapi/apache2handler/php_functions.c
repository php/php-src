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
#include "ext/standard/head.h"
#include "php_ini.h"
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
#if !defined(WIN32) && !defined(WINNT)
#include "unixd.h"
#endif

#include "php_apache.h"

#ifdef ZTS
int php_apache2_info_id;
#else
php_apache2_info_struct php_apache2_info;
#endif

#define SECTION(name)  PUTS("<h2>" name "</h2>\n")

static request_rec *php_apache_lookup_uri(char *filename TSRMLS_DC)
{
	php_struct *ctx;
	
	if (!filename) {
		return NULL;
	}
	
	ctx = SG(server_context);
	return ap_sub_req_lookup_uri(filename, ctx->r, ctx->r->output_filters);
}

/* {{{ proto bool virtual(string uri)
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
/* }}} */

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

/* {{{ proto array getallheaders(void)
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
	char tmp[1024];
	int n;
	char *p;
	server_rec *serv = ((php_struct *) SG(server_context))->r->server;
#if !defined(WIN32) && !defined(WINNT)
	AP_DECLARE_DATA extern unixd_config_rec unixd_config;
#endif
	extern int ap_max_requests_per_child;
	AP_DECLARE_DATA extern const char *ap_server_root;
	
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
	sprintf(tmp, "%d", MODULE_MAGIC_NUMBER);
	php_info_print_table_row(2, "Apache API Version", tmp);
	
	if (serv->server_admin && *(serv->server_admin)) {
		php_info_print_table_row(2, "Servert Administrator", serv->server_admin);
	}
	
	sprintf(tmp, "%s:%u", serv->server_hostname, serv->port);
	php_info_print_table_row(2, "Hostname:Port", tmp);
	
#if !defined(WIN32) && !defined(WINNT)
	sprintf(tmp, "%s(%d)/%d", unixd_config.user_name, unixd_config.user_id, unixd_config.group_id);
	php_info_print_table_row(2, "User/Group", tmp);
#endif	

	sprintf(tmp, "Per Child: %d - Keep Alive: %s - Max Per Connection: %d", ap_max_requests_per_child, (serv->keep_alive ? "on":"off"), serv->keep_alive_max);
	php_info_print_table_row(2, "Max Requests", tmp);

	sprintf(tmp, "Connection: %lld - Keep-Alive: %lld", (serv->timeout / 1000000), (serv->keep_alive_timeout / 1000000));
	php_info_print_table_row(2, "Timeouts", tmp);
	
	php_info_print_table_row(2, "Virtual Server", (serv->is_virtual ? "Yes" : "No"));
	php_info_print_table_row(2, "Server Root", ap_server_root);
	php_info_print_table_row(2, "Loaded Modules", tmp1.c);

	smart_str_free(&tmp1);
	php_info_print_table_end();
	
	DISPLAY_INI_ENTRIES();

	{
		const apr_array_header_t *arr = apr_table_elts(((php_struct *) SG(server_context))->r->subprocess_env);
		char *key, *val;
		
		SECTION("Apache Environment");
		php_info_print_table_start();	
		php_info_print_table_header(2, "Variable", "Value");
		APR_ARRAY_FOREACH_OPEN(arr, key, val)
			if (!val) {
				val = empty_string;
			}
			php_info_print_table_row(2, key, val);
		APR_ARRAY_FOREACH_CLOSE()
		                                                
		php_info_print_table_end();	
		
		SECTION("HTTP Headers Information");
		php_info_print_table_start();
		php_info_print_table_colspan_header(2, "HTTP Request Headers");
		php_info_print_table_row(2, "HTTP Request", ((php_struct *) SG(server_context))->r->the_request);
		
		arr = apr_table_elts(((php_struct *) SG(server_context))->r->headers_in);
		APR_ARRAY_FOREACH_OPEN(arr, key, val)
			if (!val) {
				val = empty_string;
			}
		        php_info_print_table_row(2, key, val);
		APR_ARRAY_FOREACH_CLOSE()

		php_info_print_table_colspan_header(2, "HTTP Response Headers");
		arr = apr_table_elts(((php_struct *) SG(server_context))->r->headers_out);
		APR_ARRAY_FOREACH_OPEN(arr, key, val)
			if (!val) {
				val = empty_string;
			}
		        php_info_print_table_row(2, key, val);
		APR_ARRAY_FOREACH_CLOSE()
		
		php_info_print_table_end();
	}
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

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("xbithack",		"0",	PHP_INI_ALL,	OnUpdateInt,	xbithack,		php_apache2_info_struct, php_apache2_info)
	STD_PHP_INI_ENTRY("engine",			"1",	PHP_INI_ALL,	OnUpdateInt,	engine, 		php_apache2_info_struct, php_apache2_info)
	STD_PHP_INI_ENTRY("last_modified",	"0",	PHP_INI_ALL,	OnUpdateInt,	last_modified,	php_apache2_info_struct, php_apache2_info)
PHP_INI_END()

static PHP_MINIT_FUNCTION(apache)
{
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(apache)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

zend_module_entry php_apache_module = {
	STANDARD_MODULE_HEADER,
	"Apache 2.0",
	apache_functions,
	PHP_MINIT(apache),
	PHP_MSHUTDOWN(apache),
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
