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
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Parts based on Apache 1.3 SAPI module by                    |
   |          Rasmus Lerdorf and Zeev Suraski                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include <fcntl.h>

#include "php.h"
#include "php_main.h"
#include "php_ini.h"
#include "php_variables.h"
#include "SAPI.h"

#include "ext/standard/php_smart_str.h"
#include "ext/standard/php_standard.h"

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
#include "ap_mpm.h"

#include "php_apache.h"

/* UnixWare defines shutdown to _shutdown, which causes problems later
 * on when using a structure member named shutdown. Since this source
 * file does not use the system call shutdown, it is safe to #undef it.
 */
#undef shutdown

 
/* A way to specify the location of the php.ini dir in an apache directive */
char *apache2_php_ini_path_override = NULL;

static int
php_apache_sapi_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	apr_bucket *b;
	apr_bucket_brigade *bb;
	apr_bucket_alloc_t *ba;
	ap_filter_t *f; /* remaining output filters */
	php_struct *ctx;

	ctx = SG(server_context);
	f = ctx->f;

	if (str_length == 0) return 0;
	
	ba = f->c->bucket_alloc;
	bb = apr_brigade_create(ctx->r->pool, ba);

	b = apr_bucket_transient_create(str, str_length, ba);
	APR_BRIGADE_INSERT_TAIL(bb, b);

#if 0
	/* Add a Flush bucket to the end of this brigade, so that
	 * the transient buckets above are more likely to make it out
	 * the end of the filter instead of having to be copied into
	 * someone's setaside. */
	b = apr_bucket_flush_create(ba);
	APR_BRIGADE_INSERT_TAIL(bb, b);
#endif
	
	if (ap_pass_brigade(f->next, bb) != APR_SUCCESS) {
		php_handle_aborted_connection();
	}
	
	return str_length; /* we always consume all the data passed to us. */
}

static int
php_apache_sapi_header_handler(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	php_struct *ctx;
	ap_filter_t *f;
	char *val;

	ctx = SG(server_context);
	f = ctx->r->output_filters;

	val = strchr(sapi_header->header, ':');

	if (!val) {
		sapi_free_header(sapi_header);
		return 0;
	}

	*val = '\0';
	
	do {
		val++;
	} while (*val == ' ');

	if (!strcasecmp(sapi_header->header, "content-type"))
		ctx->r->content_type = apr_pstrdup(ctx->r->pool, val);
	else if (sapi_header->replace)
		apr_table_set(ctx->r->headers_out, sapi_header->header, val);
	else
		apr_table_add(ctx->r->headers_out, sapi_header->header, val);
	
	sapi_free_header(sapi_header);

	return 0;
}

static int
php_apache_sapi_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	php_struct *ctx = SG(server_context);

	ctx->r->status = SG(sapi_headers).http_response_code;

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

static struct stat*
php_apache_sapi_get_stat(TSRMLS_D)
{
	php_struct *ctx = SG(server_context);

	ctx->finfo.st_uid = ctx->r->finfo.user;
	ctx->finfo.st_gid = ctx->r->finfo.group;
	ctx->finfo.st_ino = ctx->r->finfo.inode;
	ctx->finfo.st_atime = ctx->r->finfo.atime/1000000;
	ctx->finfo.st_mtime = ctx->r->finfo.mtime/1000000;
	ctx->finfo.st_ctime = ctx->r->finfo.ctime/1000000;
	ctx->finfo.st_size = ctx->r->finfo.size;
	ctx->finfo.st_nlink = ctx->r->finfo.nlink;

	return &ctx->finfo;
}

static char *
php_apache_sapi_read_cookies(TSRMLS_D)
{
	php_struct *ctx = SG(server_context);
	const char *http_cookie;

	http_cookie = apr_table_get(ctx->r->headers_in, "cookie");

	/* The SAPI interface should use 'const char *' */
	return (char *) http_cookie;
}

static char *
php_apache_sapi_getenv(char *name, size_t name_len TSRMLS_DC)
{
	php_struct *ctx = SG(server_context);
	const char *env_var;
	
	env_var = apr_table_get(ctx->r->subprocess_env, name);

	return (char *) env_var;
}

static void
php_apache_sapi_register_variables(zval *track_vars_array TSRMLS_DC)
{
	php_struct *ctx = SG(server_context);
	const apr_array_header_t *arr = apr_table_elts(ctx->r->subprocess_env);
	char *key, *val;
	
	APR_ARRAY_FOREACH_OPEN(arr, key, val)
		if (!val) val = empty_string;
		php_register_variable(key, val, track_vars_array TSRMLS_CC);
	APR_ARRAY_FOREACH_CLOSE()
		
	php_register_variable("PHP_SELF", ctx->r->uri, track_vars_array TSRMLS_CC);
}

static void
php_apache_sapi_flush(void *server_context)
{
	php_struct *ctx;
	apr_bucket_brigade *bb;
	apr_bucket_alloc_t *ba;
	apr_bucket *b;
	ap_filter_t *f; /* output filters */

	ctx = server_context;

	/* If we haven't registered a server_context yet,
	 * then don't bother flushing. */
	if (!server_context)
		return;
    
	f = ctx->f;

	/* Send a flush bucket down the filter chain. The current default
	 * handler seems to act on the first flush bucket, but ignores
	 * all further flush buckets.
	 */
	
	ba = ctx->r->connection->bucket_alloc;
	bb = apr_brigade_create(ctx->r->pool, ba);
	b = apr_bucket_flush_create(ba);
	APR_BRIGADE_INSERT_TAIL(bb, b);
	if (ap_pass_brigade(f->next, bb) != APR_SUCCESS) {
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
	if (ctx == NULL) { /* we haven't initialized our ctx yet, oh well */
		ap_log_error(APLOG_MARK, APLOG_ERR | APLOG_NOERRNO | APLOG_STARTUP,
					 0, NULL, "%s", msg);
	}
	else {
		ap_log_error(APLOG_MARK, APLOG_ERR | APLOG_NOERRNO | APLOG_STARTUP,
					 0, ctx->r->server, "%s", msg);
	}
}

static int
php_apache_disable_caching(ap_filter_t *f)
{
	/* Identify PHP scripts as non-cacheable, thus preventing 
	 * Apache from sending a 304 status when the browser sends
	 * If-Modified-Since header.
	 */
	f->r->no_local_copy = 1;
	
	return OK;
}

extern zend_module_entry php_apache_module;

static int php_apache2_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, &php_apache_module, 1)==FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}

static sapi_module_struct apache2_sapi_module = {
	"apache2filter",
	"Apache 2.0 Filter",

	php_apache2_startup,						/* startup */
	php_module_shutdown_wrapper,			/* shutdown */

	NULL,									/* activate */
	NULL,									/* deactivate */

	php_apache_sapi_ub_write,				/* unbuffered write */
	php_apache_sapi_flush,					/* flush */
	php_apache_sapi_get_stat,						/* get uid */
	php_apache_sapi_getenv,					/* getenv */

	php_error,								/* error handler */

	php_apache_sapi_header_handler,			/* header handler */
	php_apache_sapi_send_headers,			/* send headers handler */
	NULL,									/* send header handler */

	php_apache_sapi_read_post,				/* read POST data */
	php_apache_sapi_read_cookies,			/* read Cookies */

	php_apache_sapi_register_variables,
	php_apache_sapi_log_message,			/* Log message */

    NULL,									/* php_ini_path_override */

	NULL,									/* Block interruptions */
	NULL,									/* Unblock interruptions */

	STANDARD_SAPI_MODULE_PROPERTIES
};

static int php_input_filter(ap_filter_t *f, apr_bucket_brigade *bb, 
		ap_input_mode_t mode, apr_read_type_e block, apr_off_t readbytes)
{
	php_struct *ctx;
	long old_index;
	apr_bucket *b;
	const char *str;
	apr_size_t n;
	apr_status_t rv;
	TSRMLS_FETCH();

	if (f->r->proxyreq) {
		return ap_get_brigade(f->next, bb, mode, block, readbytes);
	}

	ctx = SG(server_context);
	if (ctx == NULL) {
		ap_log_rerror(APLOG_MARK, APLOG_ERR|APLOG_NOERRNO, 0, f->r,
					 "php failed to get server context");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	if ((rv = ap_get_brigade(f->next, bb, mode, block, readbytes)) != APR_SUCCESS) {
		return rv;
	}

	for (b = APR_BRIGADE_FIRST(bb); b != APR_BRIGADE_SENTINEL(bb); b = APR_BUCKET_NEXT(b)) {
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
	SG(request_info).path_translated = safe_strdup(f->r->filename);
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
	if (!PG(safe_mode) || (PG(safe_mode) && !ap_auth_type(f->r))) {
		auth = apr_table_get(f->r->headers_in, "Authorization");
		php_handle_auth_data(auth TSRMLS_CC);
	} else {
		SG(request_info).auth_user = NULL;
		SG(request_info).auth_password = NULL;
	}

	php_request_startup(TSRMLS_C);
}

static void php_apache_request_dtor(ap_filter_t *f TSRMLS_DC)
{
	php_request_shutdown(NULL);

	if (SG(request_info).query_string) {
		free(SG(request_info).query_string);
	}
	if (SG(request_info).request_uri) {
		free(SG(request_info).request_uri);
	}
}

static int php_output_filter(ap_filter_t *f, apr_bucket_brigade *bb)
{
	php_struct *ctx;
	apr_bucket *b;
	void *conf = ap_get_module_config(f->r->per_dir_config, &php4_module);
	char *p = get_php_config(conf, "engine", sizeof("engine"));
	TSRMLS_FETCH();

	if (f->r->proxyreq) {
		return ap_pass_brigade(f->next, bb);
	}
	
	/* handle situations where user turns the engine off */
	if (*p == '0') {
		return ap_pass_brigade(f->next, bb);
	}

	/* setup standard CGI variables */
	ap_add_common_vars(f->r);
	ap_add_cgi_vars(f->r);

	ctx = SG(server_context);
	if (ctx == NULL) {
		ap_log_rerror(APLOG_MARK, APLOG_ERR|APLOG_NOERRNO, 0, f->r,
					 "php failed to get server context");
        return HTTP_INTERNAL_SERVER_ERROR;
	}
	ctx->f = f; /* save whatever filters are after us in the chain. */

	if (ctx->request_processed) {
		return ap_pass_brigade(f->next, bb);
	}

	for (b = APR_BRIGADE_FIRST(bb); b != APR_BRIGADE_SENTINEL(bb); b = APR_BUCKET_NEXT(b)) {
		zend_file_handle zfd = {0};

		if (!ctx->request_processed && APR_BUCKET_IS_FILE(b)) {
			const char *path;
			apr_bucket_brigade *prebb = bb;

			/* Split the brigade into two brigades before and after
			 * the file bucket. Leave the "after the FILE" brigade
			 * in the original bb, so it gets passed outside of this
			 * loop. */
			bb = apr_brigade_split(prebb, b);

			/* Pass the "before the FILE" brigade here
			 * (if it's non-empty). */
			if (!APR_BRIGADE_EMPTY(prebb)) {
				apr_status_t rv;
				rv = ap_pass_brigade(f->next, prebb);
				/* XXX: destroy the prebb, since we know we're
				 * done with it? */
				if (rv != APR_SUCCESS) {
					php_handle_aborted_connection();
				}
			}

			apply_config(conf);
			php_apache_request_ctor(f, ctx TSRMLS_CC);

			apr_file_name_get(&path, ((apr_bucket_file *) b->data)->fd);
			
			/* Determine if we need to parse the file or show the source */
			if (strncmp(ctx->r->handler, "application/x-httpd-php-source", sizeof("application/x-httpd-php-source"))) { 
				zfd.type = ZEND_HANDLE_FILENAME;
				zfd.filename = (char *) path;
				zfd.free_filename = 0;
				zfd.opened_path = NULL;

				php_execute_script(&zfd TSRMLS_CC);
			} else { 
				zend_syntax_highlighter_ini syntax_highlighter_ini;
				
				php_get_highlight_struct(&syntax_highlighter_ini);
				
 				highlight_file((char *)path, &syntax_highlighter_ini TSRMLS_CC);
			}	
			
			php_apache_request_dtor(f TSRMLS_CC);
			
			if (!f->r->main) {
				ctx->request_processed = 1;
			}

			/* Delete the FILE bucket from the brigade. */
			apr_bucket_delete(b);

			/* We won't handle any more buckets in this brigade, so
			 * it's ok to break out now. */
			break;
		}
	}

	/* Pass whatever is left on the brigade. */
	return ap_pass_brigade(f->next, bb);
}

static apr_status_t
php_apache_server_shutdown(void *tmp)
{
	apache2_sapi_module.shutdown(&apache2_sapi_module);
	sapi_shutdown();
#ifdef ZTS
	tsrm_shutdown();
#endif
	return APR_SUCCESS;
}

static void php_apache_add_version(apr_pool_t *p)
{
	TSRMLS_FETCH();
	if (PG(expose_php)) {
		ap_add_version_component(p, "PHP/" PHP_VERSION);
	}
}

static int php_pre_config(apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp)
{
#ifndef ZTS
	int threaded_mpm;

	ap_mpm_query(AP_MPMQ_IS_THREADED, &threaded_mpm);
	if(threaded_mpm) {
		ap_log_error(APLOG_MARK, APLOG_CRIT, 0, 0, "Apache is running a threaded MPM, but your PHP Module is not compiled to be threadsafe.  You need to recompile PHP.");
		return DONE;
	}
#endif
    /* When this is NULL, apache won't override the hard-coded default
     * php.ini path setting. */
    apache2_php_ini_path_override = NULL;
    return OK;
}

static int
php_apache_server_startup(apr_pool_t *pconf, apr_pool_t *plog,
                          apr_pool_t *ptemp, server_rec *s)
{
	void *data = NULL;
	const char *userdata_key = "apache2filter_post_config";

	/* Apache will load, unload and then reload a DSO module. This
	 * prevents us from starting PHP until the second load. */
	apr_pool_userdata_get(&data, userdata_key, s->process->pool);
	if (data == NULL) {
		/* We must use set() here and *not* setn(), otherwise the
		 * static string pointed to by userdata_key will be mapped
		 * to a different location when the DSO is reloaded and the
		 * pointers won't match, causing get() to return NULL when
		 * we expected it to return non-NULL. */
		apr_pool_userdata_set((const void *)1, userdata_key,
							  apr_pool_cleanup_null, s->process->pool);
		return OK;
	}

	/* Set up our overridden path. */
	if (apache2_php_ini_path_override) {
		apache2_sapi_module.php_ini_path_override = apache2_php_ini_path_override;
	}
#ifdef ZTS
	tsrm_startup(1, 1, 0, NULL);
#endif
	sapi_startup(&apache2_sapi_module);
	apache2_sapi_module.startup(&apache2_sapi_module);
	apr_pool_cleanup_register(pconf, NULL, php_apache_server_shutdown, apr_pool_cleanup_null);
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
	} else {
		ap_add_input_filter("PHP", NULL, r, r->connection);
	}
}

static void php_insert_filter(request_rec *r)
{
	int content_type_len = strlen("application/x-httpd-php");

	if (r->content_type && !strncmp(r->content_type, "application/x-httpd-php", content_type_len-1)) {
		if (r->content_type[content_type_len] == '\0' || !strncmp(r->content_type+content_type_len, "-source", sizeof("-source"))) { 
			php_add_filter(r, r->output_filters);
			php_add_filter(r, r->input_filters);
		}	
	}
}

static apr_status_t php_server_context_cleanup(void *data_)
{
	void **data = data_;
	*data = NULL;
	return APR_SUCCESS;
}

static int php_post_read_request(request_rec *r)
{
	php_struct *ctx;
	TSRMLS_FETCH();

	/* Initialize filter context */
	SG(server_context) = ctx = apr_pcalloc(r->pool, sizeof(*ctx));

	/* register a cleanup so we clear out the SG(server_context)
	 * after each request. Note: We pass in the pointer to the
	 * server_context in case this is handled by a different thread. */
	apr_pool_cleanup_register(r->pool, (void *)&SG(server_context),
							  php_server_context_cleanup,
							  apr_pool_cleanup_null);

	/* Save the entire request, so we can get the input or output
	 * filters if we need them. */
	ctx->r = r;

	return OK;
}

static void php_register_hook(apr_pool_t *p)
{
	ap_hook_pre_config(php_pre_config, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_post_config(php_apache_server_startup, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_insert_filter(php_insert_filter, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_post_read_request(php_post_read_request, NULL, NULL, APR_HOOK_MIDDLE);
	ap_register_output_filter("PHP", php_output_filter, php_apache_disable_caching, AP_FTYPE_RESOURCE);
	ap_register_input_filter("PHP", php_input_filter, php_apache_disable_caching, AP_FTYPE_RESOURCE);
}

AP_MODULE_DECLARE_DATA module php4_module = {
	STANDARD20_MODULE_STUFF,
	create_php_config,		/* create per-directory config structure */
	merge_php_config,		/* merge per-directory config structures */
	NULL,					/* create per-server config structure */
	NULL,					/* merge per-server config structures */
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
