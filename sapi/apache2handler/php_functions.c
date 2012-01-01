/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
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
#include "ap_mpm.h"
#if !defined(WIN32) && !defined(WINNT) && !defined(NETWARE)
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
	php_struct *ctx = SG(server_context);
	
	if (!filename || !ctx || !ctx->r) {
		return NULL;
	}

	return ap_sub_req_lookup_uri(filename, ctx->r, ctx->r->output_filters);
}

/* {{{ proto bool virtual(string uri)
 Perform an apache sub-request */
PHP_FUNCTION(virtual)
{
	char *filename;
	int filename_len;
	request_rec *rr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p", &filename, &filename_len) == FAILURE) {
		return;
	}

	if (!(rr = php_apache_lookup_uri(filename TSRMLS_CC))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include '%s' - URI lookup failed", filename);
		RETURN_FALSE;
	}

	if (rr->status != HTTP_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include '%s' - error finding URI", filename);
		ap_destroy_sub_req(rr);
		RETURN_FALSE;
	}

	/* Flush everything. */
	php_output_end_all(TSRMLS_C);
	php_header(TSRMLS_C);

	/* Ensure that the ap_r* layer for the main request is flushed, to
	 * work around http://issues.apache.org/bugzilla/show_bug.cgi?id=17629 */
	ap_rflush(rr->main);

	if (ap_run_sub_req(rr)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include '%s' - request execution failed", filename);
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
		add_property_long(return_value, #name, apr_time_sec(rr->name));
#define ADD_STRING(name) \
		if (rr->name) add_property_string(return_value, #name, (char *) rr->name, 1)

PHP_FUNCTION(apache_lookup_uri)
{
	request_rec *rr;
	char *filename;
	int filename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p", &filename, &filename_len) == FAILURE) {
		return;
	}

	if (!(rr = php_apache_lookup_uri(filename TSRMLS_CC))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include '%s' - URI lookup failed", filename);
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
	
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include '%s' - error finding URI", filename);
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

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	
	ctx = SG(server_context);
	arr = apr_table_elts(ctx->r->headers_in);

	APR_ARRAY_FOREACH_OPEN(arr, key, val)
		if (!val) val = "";
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

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	
	ctx = SG(server_context);
	arr = apr_table_elts(ctx->r->headers_out);

	APR_ARRAY_FOREACH_OPEN(arr, key, val)
		if (!val) val = "";
		add_assoc_string(return_value, key, val, 1);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &note_name, &note_name_len, &note_val, &note_val_len) == FAILURE) {
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
/*
 * XXX this doesn't look right. shouldn't it be the parent ?*/
PHP_FUNCTION(apache_setenv)
{
	php_struct *ctx;
	char *variable=NULL, *string_val=NULL;
	int variable_len, string_val_len;
	zend_bool walk_to_top = 0;
	int arg_count = ZEND_NUM_ARGS();
	request_rec *r;

	if (zend_parse_parameters(arg_count TSRMLS_CC, "ss|b", &variable, &variable_len, &string_val, &string_val_len, &walk_to_top) == FAILURE) {
		return;
	}

	ctx = SG(server_context);

	r = ctx->r;
	if (arg_count == 3) {
		if (walk_to_top) {
			while(r->prev) {
				r = r->prev;
			}
		}
	}

	apr_table_set(r->subprocess_env, variable, string_val);

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
	char *variable=NULL;
	int variable_len;
	zend_bool walk_to_top = 0;
	int arg_count = ZEND_NUM_ARGS();
	char *env_val=NULL;
	request_rec *r;

	if (zend_parse_parameters(arg_count TSRMLS_CC, "s|b", &variable, &variable_len, &walk_to_top) == FAILURE) {
		return;
	}

	ctx = SG(server_context);

	r = ctx->r;
	if (arg_count == 2) {
		if (walk_to_top) {
			while(r->prev) {
				r = r->prev;
			}
		}
	}

	env_val = (char*) apr_table_get(r->subprocess_env, variable);

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
	int n, max_requests;
	char *p;
	server_rec *serv = ((php_struct *) SG(server_context))->r->server;
#if !defined(WIN32) && !defined(WINNT) && !defined(NETWARE)
#if MODULE_MAGIC_NUMBER_MAJOR >= 20081201
	AP_DECLARE_DATA extern unixd_config_rec ap_unixd_config;
#else
	AP_DECLARE_DATA extern unixd_config_rec unixd_config;
#endif
#endif
	
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
	snprintf(tmp, sizeof(tmp), "%d", MODULE_MAGIC_NUMBER);
	php_info_print_table_row(2, "Apache API Version", tmp);
	
	if (serv->server_admin && *(serv->server_admin)) {
		php_info_print_table_row(2, "Server Administrator", serv->server_admin);
	}
	
	snprintf(tmp, sizeof(tmp), "%s:%u", serv->server_hostname, serv->port);
	php_info_print_table_row(2, "Hostname:Port", tmp);
	
#if !defined(WIN32) && !defined(WINNT) && !defined(NETWARE)
#if MODULE_MAGIC_NUMBER_MAJOR >= 20081201
	snprintf(tmp, sizeof(tmp), "%s(%d)/%d", ap_unixd_config.user_name, ap_unixd_config.user_id, ap_unixd_config.group_id);
#else
	snprintf(tmp, sizeof(tmp), "%s(%d)/%d", unixd_config.user_name, unixd_config.user_id, unixd_config.group_id);
#endif
	php_info_print_table_row(2, "User/Group", tmp);
#endif

	ap_mpm_query(AP_MPMQ_MAX_REQUESTS_DAEMON, &max_requests);
	snprintf(tmp, sizeof(tmp), "Per Child: %d - Keep Alive: %s - Max Per Connection: %d", max_requests, (serv->keep_alive ? "on":"off"), serv->keep_alive_max);
	php_info_print_table_row(2, "Max Requests", tmp);

	apr_snprintf(tmp, sizeof tmp,
				 "Connection: %" APR_TIME_T_FMT " - Keep-Alive: %" APR_TIME_T_FMT, 
				 apr_time_sec(serv->timeout), apr_time_sec(serv->keep_alive_timeout));
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
				val = "";
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
				val = "";
			}
		        php_info_print_table_row(2, key, val);
		APR_ARRAY_FOREACH_CLOSE()

		php_info_print_table_colspan_header(2, "HTTP Response Headers");
		arr = apr_table_elts(((php_struct *) SG(server_context))->r->headers_out);
		APR_ARRAY_FOREACH_OPEN(arr, key, val)
			if (!val) {
				val = "";
			}
		        php_info_print_table_row(2, key, val);
		APR_ARRAY_FOREACH_CLOSE()
		
		php_info_print_table_end();
	}
}

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_apache2handler_lookup_uri, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_apache2handler_virtual, 0, 0, 1)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_apache2handler_response_headers, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_apache2handler_getallheaders, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_apache2handler_note, 0, 0, 1)
	ZEND_ARG_INFO(0, note_name)
	ZEND_ARG_INFO(0, note_value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_apache2handler_setenv, 0, 0, 2)
	ZEND_ARG_INFO(0, variable)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, walk_to_top)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_apache2handler_getenv, 0, 0, 1)
	ZEND_ARG_INFO(0, variable)
	ZEND_ARG_INFO(0, walk_to_top)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_apache2handler_get_version, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_apache2handler_get_modules, 0)
ZEND_END_ARG_INFO()
/* }}} */

static const zend_function_entry apache_functions[] = {
	PHP_FE(apache_lookup_uri, 		arginfo_apache2handler_lookup_uri)
	PHP_FE(virtual, 				arginfo_apache2handler_virtual) 
	PHP_FE(apache_request_headers, 	arginfo_apache2handler_getallheaders)
	PHP_FE(apache_response_headers, arginfo_apache2handler_response_headers)
	PHP_FE(apache_setenv, 		arginfo_apache2handler_setenv)
	PHP_FE(apache_getenv, 		arginfo_apache2handler_getenv)
	PHP_FE(apache_note, 		arginfo_apache2handler_note)
	PHP_FE(apache_get_version, 	arginfo_apache2handler_get_version)
	PHP_FE(apache_get_modules, 	arginfo_apache2handler_get_modules)
	PHP_FALIAS(getallheaders, 	apache_request_headers, arginfo_apache2handler_getallheaders)
	{NULL, NULL, NULL}
};

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("xbithack",		"0",	PHP_INI_ALL,	OnUpdateLong,	xbithack,	php_apache2_info_struct, php_apache2_info)
	STD_PHP_INI_ENTRY("engine",		"1",	PHP_INI_ALL,	OnUpdateLong,	engine, 	php_apache2_info_struct, php_apache2_info)
	STD_PHP_INI_ENTRY("last_modified",	"0",	PHP_INI_ALL,	OnUpdateLong,	last_modified,	php_apache2_info_struct, php_apache2_info)
PHP_INI_END()

static PHP_MINIT_FUNCTION(apache)
{
#ifdef ZTS
	ts_allocate_id(&php_apache2_info_id, sizeof(php_apache2_info_struct), (ts_allocate_ctor) NULL, NULL);
#endif
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
	"apache2handler",
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
