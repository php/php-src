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
#ifndef NETWARE
#include "ext/standard/php_standard.h"
#else
#include "ext/standard/basic_functions.h"
#endif

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
                                                                                                                     
#define PHP_MAGIC_TYPE "application/x-httpd-php"
#define PHP_SOURCE_MAGIC_TYPE "application/x-httpd-php-source"
#define PHP_SCRIPT "php-script"

#ifdef NETWARE
#undef shutdown /* To avoid Winsock confusion */
#endif

/* A way to specify the location of the php.ini dir in an apache directive */
char *apache2_php_ini_path_override = NULL;

static int
php_apache_sapi_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	apr_bucket *b;
	apr_bucket_brigade *bb;
	apr_bucket_alloc_t *ba;
	php_struct *ctx;
    char* copy_str;

	ctx = SG(server_context);
	if (str_length == 0) 
        return 0;
    copy_str = apr_pmemdup( ctx->r->pool, str, str_length+1);
	b = apr_bucket_pool_create(copy_str, str_length, ctx->r->pool, ctx->c->bucket_alloc);
	APR_BRIGADE_INSERT_TAIL(ctx->bb, b);

#if 0
	/* Add a Flush bucket to the end of this brigade, so that
	 * the transient buckets above are more likely to make it out
	 * the end of the filter instead of having to be copied into
	 * someone's setaside. */
	b = apr_bucket_flush_create(ba);
	APR_BRIGADE_INSERT_TAIL(bb, b);
#endif
/*	
	if (ap_pass_brigade(f->next, bb) != APR_SUCCESS) {
		php_handle_aborted_connection();
	}
*/	
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
	php_struct *ctx;
	apr_status_t rv;
    apr_bucket_brigade *bb;
    apr_bucket *bucket;
    char *last=buf;
    int last_count=0;
    int seen_eos=0;

    ctx = SG(server_context);
    bb = apr_brigade_create(ctx->r->pool, ctx->c->bucket_alloc);

	if ((rv = ap_get_brigade(ctx->r->input_filters, bb, AP_MODE_READBYTES, 
                    APR_BLOCK_READ, count_bytes)) != APR_SUCCESS) {
		return 0;
	}
	
    APR_BRIGADE_FOREACH(bucket,bb) {
        const char*data;
        apr_size_t len;
        if (APR_BUCKET_IS_EOS(bucket)) {
           seen_eos = 1;
           break;
        }
        /* We can't do much with this. */
        if (APR_BUCKET_IS_FLUSH(bucket)) {
           continue;
        }

        apr_bucket_read(bucket, &data, &len, APR_BLOCK_READ);
        if (last_count +len >  count_bytes) {
           ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, ctx->r,
                      "PHP: Read too much post data raise a bug please: %s", ctx->r->uri);
           break;
        }
        memcpy( last, data, len);
        last += len;
        last_count += len;
    }

	return last_count;
}

static struct stat*
php_apache_sapi_get_stat(TSRMLS_D)
{
	php_struct *ctx = SG(server_context);

	ctx->finfo.st_uid = ctx->r->finfo.user;
	ctx->finfo.st_gid = ctx->r->finfo.group;
	ctx->finfo.st_ino = ctx->r->finfo.inode;
#if defined(NETWARE) && defined(CLIB_STAT_PATCH)
	ctx->finfo.st_atime.tv_sec = ctx->r->finfo.atime/1000000;
	ctx->finfo.st_mtime.tv_sec = ctx->r->finfo.mtime/1000000;
	ctx->finfo.st_ctime.tv_sec = ctx->r->finfo.ctime/1000000;
#else
	ctx->finfo.st_atime = ctx->r->finfo.atime/1000000;
	ctx->finfo.st_mtime = ctx->r->finfo.mtime/1000000;
	ctx->finfo.st_ctime = ctx->r->finfo.ctime/1000000;
#endif

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
	apr_bucket_alloc_t *ba;
	apr_bucket *b;
    /*
     * XXX: handle flush
     * this has issues has PHP is not re-entrant, and can introduce race issues
     * so until PHP is reentrant don't flush.
     */
    return;

#ifdef NEVER
	ctx = server_context;

	/* If we haven't registered a server_context yet,
	 * then don't bother flushing. */
	if (!server_context)
		return;
    
	/* Send a flush bucket down the filter chain. The current default
	 * handler seems to act on the first flush bucket, but ignores
	 * all further flush buckets.
	 */
	
	ba = ctx->c->bucket_alloc;
	b = apr_bucket_flush_create(ba);
	APR_BRIGADE_INSERT_TAIL(ctx->bb, b);
	if (ap_pass_brigade(r->output_filters, ctx->bb) != APR_SUCCESS) {
		php_handle_aborted_connection();
	}
#endif
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
		ap_log_rerror(APLOG_MARK, APLOG_ERR | APLOG_NOERRNO | APLOG_STARTUP,
					 0, ctx->r, "%s", msg);
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
	"apache2hook",
	"Apache 2.0 Hook",

	php_apache2_startup,					/* startup */
	php_module_shutdown_wrapper,			/* shutdown */

	NULL,									/* activate */
	NULL,									/* deactivate */

	php_apache_sapi_ub_write,				/* unbuffered write */
	php_apache_sapi_flush,					/* flush */
	php_apache_sapi_get_stat,				/* get uid */
	php_apache_sapi_getenv,					/* getenv */

	php_error,								/* error handler */

	php_apache_sapi_header_handler,			/* header handler */
	php_apache_sapi_send_headers,			/* send headers handler */
	NULL,									/* send header handler */

	php_apache_sapi_read_post,				/* read POST data */
	php_apache_sapi_read_cookies,			/* read Cookies */

	php_apache_sapi_register_variables,
	php_apache_sapi_log_message,			/* Log message */

	STANDARD_SAPI_MODULE_PROPERTIES
};

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
		ap_add_version_component(p, "PhP/" PHP_VERSION);
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
	const char *userdata_key = "apache2hook_post_config";

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

static apr_status_t php_server_context_cleanup(void *data_)
{
	void **data = data_;
	*data = NULL;
	return APR_SUCCESS;
}

static int php_handler(request_rec *r)
{
	int content_type_len = strlen("application/x-httpd-php");
	const char *auth;
    void *conf;
    char *enabled;
	php_struct *ctx;
	TSRMLS_FETCH();

    if (!r->content_type) {
        return DECLINED;
    }

    if (strncmp(r->handler, "application/x-httpd-php", content_type_len -1))  {
       return DECLINED;
    }
    /*
    if (strcmp(r->handler, PHP_MAGIC_TYPE) &&
        strcmp(r->handler, PHP_SOURCE_MAGIC_TYPE) &&
        strcmp(r->handler, PHP_SCRIPT)) {
        return DECLINED;
    }

    conf = ap_get_module_config(r->per_dir_config, &php4_module);
    enabled = get_php_config(conf, "engine", sizeof("engine"));
    */

    /* handle situations where user turns the engine off */
    if (*enabled == '0') {
        return DECLINED;
    }

    r->allowed |= (AP_METHOD_BIT << M_GET);
    r->allowed |= (AP_METHOD_BIT << M_POST);
    if (r->method_number != M_GET && r->method_number != M_POST ) {
        return DECLINED;
    }

    /* XXX not sure if we need this */
    if (r->finfo.filetype == 0) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
                "File does not exist: %s", r->filename);
        return HTTP_NOT_FOUND;
    }
    
	/* Initialize filter context */
	SG(server_context) = ctx = apr_pcalloc(r->pool, sizeof(*ctx));

	/* register a cleanup so we clear out the SG(server_context)
	 * after each request. Note: We pass in the pointer to the
	 * server_context in case this is handled by a different thread. */
	apr_pool_cleanup_register(r->pool, (void *)&SG(server_context),
							  php_server_context_cleanup,
							  apr_pool_cleanup_null);

	ap_add_common_vars(r);
	ap_add_cgi_vars(r);

	SG(sapi_headers).http_response_code = 200;
	SG(request_info).content_type = apr_table_get(r->headers_in, "Content-Type");
	SG(request_info).query_string = apr_pstrdup(r->pool, r->args);
	SG(request_info).request_method = r->method;
	SG(request_info).request_uri = apr_pstrdup(r->pool, r->uri);
	r->no_local_copy = 1;
	r->content_type = apr_pstrdup(r->pool, sapi_get_default_content_type(TSRMLS_C));
	apr_table_unset(r->headers_out, "Content-Length");
	apr_table_unset(r->headers_out, "Last-Modified");
	apr_table_unset(r->headers_out, "Expires");
	apr_table_unset(r->headers_out, "ETag");

	if (!PG(safe_mode)) {
		auth = apr_table_get(r->headers_in, "Authorization");
		php_handle_auth_data(auth TSRMLS_CC);
	} else {
		SG(request_info).auth_user = NULL;
		SG(request_info).auth_password = NULL;
	}
	/* setup standard CGI variables */
	ctx = SG(server_context);
	if (ctx == NULL) {
		ap_log_rerror(APLOG_MARK, APLOG_ERR|APLOG_NOERRNO, 0, r,
					 "php failed to get server context");
        return HTTP_INTERNAL_SERVER_ERROR;
	}

    ctx->r = r;
    ctx->c = r->connection;

	php_request_startup(TSRMLS_C);

    ctx->bb = apr_brigade_create(r->pool, ctx->c->bucket_alloc);
    if (strncmp(r->handler, PHP_SOURCE_MAGIC_TYPE, 
                sizeof(PHP_SOURCE_MAGIC_TYPE)) == 0) {
        zend_syntax_highlighter_ini syntax_highlighter_ini;
        php_get_highlight_struct(&syntax_highlighter_ini);
        highlight_file((char *)r->filename, &syntax_highlighter_ini TSRMLS_CC);
    }
    else {
        zend_file_handle zfd;

        zfd.type = ZEND_HANDLE_FILENAME;
        zfd.filename = r->filename;
        zfd.free_filename = 0;
        zfd.opened_path = NULL;
        php_execute_script(&zfd TSRMLS_CC);
#if MEMORY_LIMIT
        {
            char *mem_usage;

            mem_usage = apr_psprintf(r->pool, "%u", AG(allocated_memory_peak));
            AG(allocated_memory_peak) = 0;
            apr_table_set(r->notes, "mod_php_memory_usage", mem_usage);
        }
#endif
    }

	php_request_shutdown(NULL);
/*
 * handled by APR
	if (SG(request_info).query_string) {
		free(SG(request_info).query_string);
	}
	if (SG(request_info).request_uri) {
		free(SG(request_info).request_uri);
	}
*/
    APR_BRIGADE_INSERT_TAIL(ctx->bb, apr_bucket_eos_create(ctx->c->bucket_alloc));
    r->status = SG(sapi_headers).http_response_code;
    return ap_pass_brigade( r->output_filters, ctx->bb);
/*
 * XXX: check.. how do we set the response code ?
    return SG(sapi_headers).http_response_code;
 */

}
static void php_register_hook(apr_pool_t *p)
{
	ap_hook_pre_config(php_pre_config, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_post_config(php_apache_server_startup, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_handler(php_handler, NULL, NULL, APR_HOOK_MIDDLE);
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
