/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Parts based on Apache 1.3 SAPI module by                    |
   |          Rasmus Lerdorf and Zeev Suraski                             |
   +----------------------------------------------------------------------+
 */

#include <fcntl.h>

#include "php.h"
#include "php_main.h"
#include "php_ini.h"
#include "php_variables.h"
#include "SAPI.h"

#include "ext/standard/php_smart_str.h"

#include "apr_strings.h"
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

static int
php_apache_sapi_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	apr_bucket *b;
	apr_bucket_brigade *bb;
	php_struct *ctx;
	uint now;

	ctx = SG(server_context);

	if (str_length == 0) return 0;
	
	bb = apr_brigade_create(ctx->f->r->pool);
	while (str_length > 0) {
		now = MIN(str_length, 4096);
		b = apr_bucket_transient_create(str, now);
		APR_BRIGADE_INSERT_TAIL(bb, b);
		str += now;
		str_length -= now;
	}
	if (ap_pass_brigade(ctx->f->next, bb) != APR_SUCCESS) {
		php_handle_aborted_connection();
	}
	
	return str_length;
}

static int
php_apache_sapi_header_handler(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	php_struct *ctx = SG(server_context);
	char *val;

	val = strchr(sapi_header->header, ':');

	if (!val) return 0;

	*val = '\0';
	
	do {
		val++;
	} while (*val == ' ');

	if (!strcasecmp(sapi_header->header, "content-type"))
		ctx->f->r->content_type = apr_pstrdup(ctx->f->r->pool, val);
	else if (sapi_header->replace)
		apr_table_set(ctx->f->r->headers_out, sapi_header->header, val);
	else
		apr_table_add(ctx->f->r->headers_out, sapi_header->header, val);
	
	sapi_free_header(sapi_header);

	return 0;
}

static int
php_apache_sapi_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	php_struct *ctx = SG(server_context);

	ctx->f->r->status = SG(sapi_headers).http_response_code;

	return SAPI_HEADER_SENT_SUCCESSFULLY;
}

static int
php_apache_sapi_read_post(char *buf, uint count_bytes TSRMLS_DC)
{
	int n;
	int to_read;
	php_struct *ctx = SG(server_context);

	to_read = ctx->post_len - ctx->post_idx;
	n = MIN(to_read, count_bytes);
	
	if (n > 0) {
		memcpy(buf, ctx->post_data + ctx->post_idx, n);
		ctx->post_idx += n;
	} else {
		if (ctx->post_data) free(ctx->post_data);
		ctx->post_data = NULL;
	}

	return n;
}

static char *
php_apache_sapi_read_cookies(TSRMLS_D)
{
	php_struct *ctx = SG(server_context);
	const char *http_cookie;

	http_cookie = apr_table_get(ctx->f->r->headers_in, "cookie");

	/* The SAPI interface should use 'const char *' */
	return (char *) http_cookie;
}

static void
php_apache_sapi_register_variables(zval *track_vars_array TSRMLS_DC)
{
	php_struct *ctx = SG(server_context);
	const apr_array_header_t *arr = apr_table_elts(ctx->f->r->subprocess_env);
	char *key, *val;
	
	APR_ARRAY_FOREACH_OPEN(arr, key, val)
		if (!val) val = empty_string;
		php_register_variable(key, val, track_vars_array TSRMLS_CC);
	APR_ARRAY_FOREACH_CLOSE()
		
	php_register_variable("PHP_SELF", ctx->f->r->uri, track_vars_array TSRMLS_CC);
}

static void
php_apache_sapi_flush(void *server_context)
{
	php_struct *ctx = server_context;
	apr_bucket_brigade *bb;
	apr_bucket *b;

	if (!server_context)
		return;
    
	/* Send a flush bucket down the filter chain. The current default
	 * handler seems to act on the first flush bucket, but ignores
	 * all further flush buckets.
	 */
	
	bb = apr_brigade_create(ctx->f->r->pool);
	b = apr_bucket_flush_create();
	APR_BRIGADE_INSERT_TAIL(bb, b);
	if (ap_pass_brigade(ctx->f->next, bb) != APR_SUCCESS) {
		php_handle_aborted_connection();
	}
}

static void php_apache_sapi_log_message(char *msg)
{
	php_struct *ctx;
	TSRMLS_FETCH();

	ctx = SG(server_context);
   
	/* We use APLOG_STARTUP because it keeps us from printing the
	 * data and time information at the beginning of the error log
	 * line.  Not sure if this is correct, but it mirrors what happens
	 * with Apache 1.3 -- rbb
	 */
	ap_log_error(APLOG_MARK, APLOG_ERR | APLOG_NOERRNO | APLOG_STARTUP, 0, ctx->f->r->server, "%s", msg);
}

static sapi_module_struct apache2_sapi_module = {
	"apache2filter",
	"Apache 2.0 Filter",

	php_module_startup,							/* startup */
	php_module_shutdown_wrapper,			/* shutdown */

	NULL,									/* activate */
	NULL,									/* deactivate */

	php_apache_sapi_ub_write,					/* unbuffered write */
	php_apache_sapi_flush,					/* flush */
	NULL,									/* get uid */
	NULL,									/* getenv */

	php_error,								/* error handler */

	php_apache_sapi_header_handler,				/* header handler */
	php_apache_sapi_send_headers,				/* send headers handler */
	NULL,									/* send header handler */

	php_apache_sapi_read_post,					/* read POST data */
	php_apache_sapi_read_cookies,				/* read Cookies */

	php_apache_sapi_register_variables,
	php_apache_sapi_log_message,			/* Log message */

	NULL,									/* Block interruptions */
	NULL,									/* Unblock interruptions */

	STANDARD_SAPI_MODULE_PROPERTIES
};


AP_MODULE_DECLARE_DATA module php4_module;

#define INIT_CTX \
	if (ctx == NULL) { \
		/* Initialize filter context */ \
		SG(server_context) = ctx = apr_pcalloc(f->r->pool, sizeof(*ctx));  \
		ctx->bb = apr_brigade_create(f->c->pool); \
	}

static int php_input_filter(ap_filter_t *f, apr_bucket_brigade *bb, 
		ap_input_mode_t mode, apr_read_type_e block, apr_off_t readbytes)
{
	php_struct *ctx;
	long old_index;
	apr_bucket *b;
	const char *str;
	apr_ssize_t n;
	apr_status_t rv;
	TSRMLS_FETCH();

	if (f->r->proxyreq) {
		return ap_get_brigade(f->next, bb, mode, block, readbytes);
	}

	ctx = SG(server_context);

	INIT_CTX;

	if ((rv = ap_get_brigade(f->next, bb, mode, block, readbytes)) != APR_SUCCESS) {
		return rv;
	}

	APR_BRIGADE_FOREACH(b, bb) {
		apr_bucket_read(b, &str, &n, 1);
		if (n > 0) {
			old_index = ctx->post_len;
			ctx->post_len += n;
			ctx->post_data = realloc(ctx->post_data, ctx->post_len + 1);
			memcpy(ctx->post_data + old_index, str, n);
		}
	}
	return APR_SUCCESS;
}

static void php_apache_request_ctor(ap_filter_t *f, php_struct *ctx TSRMLS_DC)
{
	char *content_type;
	const char *auth;
	
	PG(during_request_startup) = 0;
	SG(sapi_headers).http_response_code = 200;
	SG(request_info).content_type = apr_table_get(f->r->headers_in, "Content-Type");
#undef safe_strdup
#define safe_strdup(x) ((x)?strdup((x)):NULL)	
	SG(request_info).query_string = safe_strdup(f->r->args);
	SG(request_info).request_method = f->r->method;
	SG(request_info).request_uri = safe_strdup(f->r->uri);
	f->r->no_local_copy = 1;
	content_type = sapi_get_default_content_type(TSRMLS_C);
	f->r->content_type = apr_pstrdup(f->r->pool, content_type);
	SG(request_info).post_data = ctx->post_data;
	SG(request_info).post_data_length = ctx->post_len;
	efree(content_type);
	apr_table_unset(f->r->headers_out, "Content-Length");
	apr_table_unset(f->r->headers_out, "Last-Modified");
	apr_table_unset(f->r->headers_out, "Expires");
	apr_table_unset(f->r->headers_out, "ETag");
	apr_table_unset(f->r->headers_in, "Connection");
	auth = apr_table_get(f->r->headers_in, "Authorization");
	php_handle_auth_data(auth TSRMLS_CC);

	php_request_startup(TSRMLS_C);
}

static void php_apache_request_dtor(ap_filter_t *f TSRMLS_DC)
{
	php_request_shutdown(NULL);

#undef safe_free
#define safe_free(x) ((x)?free((x)):0)
	safe_free(SG(request_info).query_string);
	safe_free(SG(request_info).request_uri);
}

static int php_output_filter(ap_filter_t *f, apr_bucket_brigade *bb)
{
	php_struct *ctx;
	apr_bucket *b;
	void *conf = ap_get_module_config(f->r->per_dir_config, &php4_module);
	TSRMLS_FETCH();

	if (f->r->proxyreq) {
		return ap_pass_brigade(f->next, bb);
	}

	/* setup standard CGI variables */
	ap_add_common_vars(f->r);
	ap_add_cgi_vars(f->r);

	ctx = SG(server_context);
	INIT_CTX;

	ctx->f = f;

	/* states:
	 * 0: request startup
	 * 1: collecting data
	 * 2: script execution and request shutdown
	 */
	if (ctx->state == 0) {
	
		apply_config(conf);
		
		ctx->state++;

		php_apache_request_ctor(f, ctx TSRMLS_CC);
	}

	/* moves all buckets from bb to ctx->bb */
	ap_save_brigade(f, &ctx->bb, &bb, f->r->pool);

	/* If we have received all data from the previous filters,
	 * we "flatten" the buckets by creating a single string buffer.
	 */
	if (ctx->state == 1 && APR_BUCKET_IS_EOS(APR_BRIGADE_LAST(ctx->bb))) {
		zend_file_handle zfd;
		apr_bucket *eos;

		/* We want to execute only one script per request.
		 * A bug in Apache or other filters could cause us
		 * to reenter php_filter during script execution, so
		 * we protect ourselves here.
		 */
		ctx->state = 2;

		/* Handle phpinfo/phpcredits/built-in images */
		if (!php_handle_special_queries(TSRMLS_C)) {

			b = APR_BRIGADE_FIRST(ctx->bb);
			
			if (APR_BUCKET_IS_FILE(b)) {
				const char *path;

				apr_file_name_get(&path, ((apr_bucket_file *) b->data)->fd);
				
				zfd.type = ZEND_HANDLE_FILENAME;
				zfd.filename = (char *) path;
				zfd.free_filename = 0;
				zfd.opened_path = NULL;

				php_execute_script(&zfd TSRMLS_CC);
			} else {
				
#define PHP_NO_DATA "The PHP Filter did not receive suitable input data"
				
				eos = apr_bucket_transient_create(PHP_NO_DATA, sizeof(PHP_NO_DATA)-1);
				APR_BRIGADE_INSERT_HEAD(bb, eos);
			}
		}
		
		php_apache_request_dtor(f TSRMLS_CC);

		SG(server_context) = 0;
		/* Pass EOS bucket to next filter to signal end of request */
		eos = apr_bucket_eos_create();
		APR_BRIGADE_INSERT_TAIL(bb, eos);
		
		return ap_pass_brigade(f->next, bb);
	} else
		apr_brigade_destroy(bb);

	return APR_SUCCESS;
}

static apr_status_t
php_apache_server_shutdown(void *tmp)
{
	apache2_sapi_module.shutdown(&apache2_sapi_module);
	sapi_shutdown();
	tsrm_shutdown();
	return APR_SUCCESS;
}

static void php_apache_add_version(apr_pool_t *p)
{
	TSRMLS_FETCH();
	if (PG(expose_php)) {
		ap_add_version_component(p, "PHP/" PHP_VERSION);
	}
}

static int
php_apache_server_startup(apr_pool_t *pconf, apr_pool_t *plog,
                          apr_pool_t *ptemp, server_rec *s)
{
	tsrm_startup(1, 1, 0, NULL);
	sapi_startup(&apache2_sapi_module);
	apache2_sapi_module.startup(&apache2_sapi_module);
	apr_pool_cleanup_register(pconf, NULL, php_apache_server_shutdown, apr_pool_cleanup_null);
	php_apache_register_module();
	php_apache_add_version(pconf);

	return OK;
}

static void php_add_filter(request_rec *r, ap_filter_t *f)
{
	int output = (f == r->output_filters);

	/* for those who still have Set*Filter PHP configured */
	while (f) {
		if (strcmp(f->frec->name, "PHP") == 0) {
			ap_log_error(APLOG_MARK, APLOG_WARNING | APLOG_NOERRNO,
				     0, r->server,
				     "\"Set%sFilter PHP\" already configured for %s",
				     output ? "Output" : "Input", r->uri);
			return;
		}
		f = f->next;
	}

	if (output) {
		ap_add_output_filter("PHP", NULL, r, r->connection);
	}
	else {
		ap_add_input_filter("PHP", NULL, r, r->connection);
	}
}

static void php_insert_filter(request_rec *r)
{
	if (r->content_type &&
	    strcmp(r->content_type, "application/x-httpd-php") == 0) {
		php_add_filter(r, r->output_filters);
		php_add_filter(r, r->input_filters);
	}
}

static void php_register_hook(apr_pool_t *p)
{
	ap_hook_post_config(php_apache_server_startup, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_insert_filter(php_insert_filter, NULL, NULL, APR_HOOK_MIDDLE);
	ap_register_output_filter("PHP", php_output_filter, AP_FTYPE_RESOURCE);
	ap_register_input_filter("PHP", php_input_filter, AP_FTYPE_RESOURCE);
}

AP_MODULE_DECLARE_DATA module php4_module = {
    STANDARD20_MODULE_STUFF,
    create_php_config,		/* create per-directory config structure */
    merge_php_config,      		/* merge per-directory config structures */
    NULL,			/* create per-server config structure */
    NULL,			/* merge per-server config structures */
    php_dir_cmds,			/* command apr_table_t */
    php_register_hook		/* register hooks */
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
