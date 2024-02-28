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
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Parts based on Apache 1.3 SAPI module by                    |
   |          Rasmus Lerdorf and Zeev Suraski                             |
   +----------------------------------------------------------------------+
 */

#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS

#include "php.h"
#ifdef strcasecmp
# undef strcasecmp
#endif
#ifdef strncasecmp
# undef strncasecmp
#endif
#include "php_main.h"
#include "php_ini.h"
#include "php_variables.h"
#include "SAPI.h"

#include <fcntl.h>

#include "zend_smart_str.h"
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

/* UnixWare define shutdown to _shutdown, which causes problems later
 * on when using a structure member named shutdown. Since this source
 * file does not use the system call shutdown, it is safe to #undef it.
 */
#undef shutdown

#define PHP_MAGIC_TYPE "application/x-httpd-php"
#define PHP_SOURCE_MAGIC_TYPE "application/x-httpd-php-source"
#define PHP_SCRIPT "php-script"

/* A way to specify the location of the php.ini dir in an apache directive */
char *apache2_php_ini_path_override = NULL;
#if defined(PHP_WIN32) && defined(ZTS)
ZEND_TSRMLS_CACHE_DEFINE()
#endif

static size_t
php_apache_sapi_ub_write(const char *str, size_t str_length)
{
	request_rec *r;
	php_struct *ctx;

	ctx = SG(server_context);
	r = ctx->r;

	if (ap_rwrite(str, str_length, r) < 0) {
		php_handle_aborted_connection();
	}

	return str_length; /* we always consume all the data passed to us. */
}

static int
php_apache_sapi_header_handler(sapi_header_struct *sapi_header, sapi_header_op_enum op, sapi_headers_struct *sapi_headers)
{
	php_struct *ctx;
	char *val, *ptr;

	ctx = SG(server_context);

	switch (op) {
		case SAPI_HEADER_DELETE:
			apr_table_unset(ctx->r->headers_out, sapi_header->header);
			return 0;

		case SAPI_HEADER_DELETE_ALL:
			apr_table_clear(ctx->r->headers_out);
			return 0;

		case SAPI_HEADER_ADD:
		case SAPI_HEADER_REPLACE:
			val = strchr(sapi_header->header, ':');

			if (!val) {
				return 0;
			}
			ptr = val;

			*val = '\0';

			do {
				val++;
			} while (*val == ' ');

			if (!strcasecmp(sapi_header->header, "content-type")) {
				if (ctx->content_type) {
					efree(ctx->content_type);
				}
				ctx->content_type = estrdup(val);
			} else if (!strcasecmp(sapi_header->header, "content-length")) {
				apr_off_t clen = 0;

				if (APR_SUCCESS != apr_strtoff(&clen, val, (char **) NULL, 10)) {
					/* We'll fall back to strtol, since that's what we used to
					 * do anyway. */
					clen = (apr_off_t) strtol(val, (char **) NULL, 10);
				}

				ap_set_content_length(ctx->r, clen);
			} else if (op == SAPI_HEADER_REPLACE) {
				apr_table_set(ctx->r->headers_out, sapi_header->header, val);
			} else {
				apr_table_add(ctx->r->headers_out, sapi_header->header, val);
			}

			*ptr = ':';

			return SAPI_HEADER_ADD;

		default:
			return 0;
	}
}

static int
php_apache_sapi_send_headers(sapi_headers_struct *sapi_headers)
{
	php_struct *ctx = SG(server_context);
	const char *sline = SG(sapi_headers).http_status_line;

	ctx->r->status = SG(sapi_headers).http_response_code;

	/* httpd requires that r->status_line is set to the first digit of
	 * the status-code: */
	if (sline && strlen(sline) > 12 && strncmp(sline, "HTTP/1.", 7) == 0 && sline[8] == ' ') {
		ctx->r->status_line = apr_pstrdup(ctx->r->pool, sline + 9);
		ctx->r->proto_num = 1000 + (sline[7]-'0');
		if ((sline[7]-'0') == 0) {
			apr_table_set(ctx->r->subprocess_env, "force-response-1.0", "true");
		}
	}

	/*	call ap_set_content_type only once, else each time we call it,
		configured output filters for that content type will be added */
	if (!ctx->content_type) {
		ctx->content_type = sapi_get_default_content_type();
	}
	ap_set_content_type(ctx->r, apr_pstrdup(ctx->r->pool, ctx->content_type));
	efree(ctx->content_type);
	ctx->content_type = NULL;

	return SAPI_HEADER_SENT_SUCCESSFULLY;
}

static apr_size_t
php_apache_sapi_read_post(char *buf, size_t count_bytes)
{
	apr_size_t len, tlen=0;
	php_struct *ctx = SG(server_context);
	request_rec *r;
	apr_bucket_brigade *brigade;
	apr_status_t status;

	r = ctx->r;
	brigade = ctx->brigade;
	len = count_bytes;

	/*
	 * This loop is needed because ap_get_brigade() can return us partial data
	 * which would cause premature termination of request read. Therefore we
	 * need to make sure that if data is available we fill the buffer completely.
	 */

	while ((status = ap_get_brigade(r->input_filters, brigade, AP_MODE_READBYTES, APR_BLOCK_READ, len)) == APR_SUCCESS) {
		apr_brigade_flatten(brigade, buf, &len);
		apr_brigade_cleanup(brigade);
		tlen += len;
		if (tlen == count_bytes || !len) {
			break;
		}
		buf += len;
		len = count_bytes - tlen;
	}

	if (status != APR_SUCCESS) {
		return 0;
	}

	return tlen;
}

static zend_stat_t*
php_apache_sapi_get_stat(void)
{
	php_struct *ctx = SG(server_context);

#ifdef PHP_WIN32
	ctx->finfo.st_uid = 0;
	ctx->finfo.st_gid = 0;
#else
	ctx->finfo.st_uid = ctx->r->finfo.user;
	ctx->finfo.st_gid = ctx->r->finfo.group;
#endif
	ctx->finfo.st_dev = ctx->r->finfo.device;
	ctx->finfo.st_ino = ctx->r->finfo.inode;
	ctx->finfo.st_atime = apr_time_sec(ctx->r->finfo.atime);
	ctx->finfo.st_mtime = apr_time_sec(ctx->r->finfo.mtime);
	ctx->finfo.st_ctime = apr_time_sec(ctx->r->finfo.ctime);
	ctx->finfo.st_size = ctx->r->finfo.size;
	ctx->finfo.st_nlink = ctx->r->finfo.nlink;

	return &ctx->finfo;
}

static char *
php_apache_sapi_read_cookies(void)
{
	php_struct *ctx = SG(server_context);
	const char *http_cookie;

	http_cookie = apr_table_get(ctx->r->headers_in, "cookie");

	/* The SAPI interface should use 'const char *' */
	return (char *) http_cookie;
}

static char *
php_apache_sapi_getenv(const char *name, size_t name_len)
{
	php_struct *ctx = SG(server_context);
	const char *env_var;

	if (ctx == NULL) {
		return NULL;
	}

	env_var = apr_table_get(ctx->r->subprocess_env, name);

	return (char *) env_var;
}

static void
php_apache_sapi_register_variables(zval *track_vars_array)
{
	php_struct *ctx = SG(server_context);
	const apr_array_header_t *arr = apr_table_elts(ctx->r->subprocess_env);
	char *key, *val;
	size_t new_val_len;

	APR_ARRAY_FOREACH_OPEN(arr, key, val)
		if (!val) {
			val = "";
		}
		if (sapi_module.input_filter(PARSE_SERVER, key, &val, strlen(val), &new_val_len)) {
			php_register_variable_safe(key, val, new_val_len, track_vars_array);
		}
	APR_ARRAY_FOREACH_CLOSE()

	if (sapi_module.input_filter(PARSE_SERVER, "PHP_SELF", &ctx->r->uri, strlen(ctx->r->uri), &new_val_len)) {
		php_register_variable_safe("PHP_SELF", ctx->r->uri, new_val_len, track_vars_array);
	}
}

static void
php_apache_sapi_flush(void *server_context)
{
	php_struct *ctx;
	request_rec *r;

	ctx = server_context;

	/* If we haven't registered a server_context yet,
	 * then don't bother flushing. */
	if (!server_context) {
		return;
	}

	r = ctx->r;

	sapi_send_headers();

	r->status = SG(sapi_headers).http_response_code;
	SG(headers_sent) = 1;

	if (ap_rflush(r) < 0 || r->connection->aborted) {
		php_handle_aborted_connection();
	}
}

static void php_apache_sapi_log_message(const char *msg, int syslog_type_int)
{
	php_struct *ctx;
	int aplog_type = APLOG_ERR;

	ctx = SG(server_context);

	switch (syslog_type_int) {
#if LOG_EMERG != LOG_CRIT
		case LOG_EMERG:
			aplog_type = APLOG_EMERG;
			break;
#endif
#if LOG_ALERT != LOG_CRIT
		case LOG_ALERT:
			aplog_type = APLOG_ALERT;
			break;
#endif
		case LOG_CRIT:
			aplog_type = APLOG_CRIT;
			break;
		case LOG_ERR:
			aplog_type = APLOG_ERR;
			break;
		case LOG_WARNING:
			aplog_type = APLOG_WARNING;
			break;
		case LOG_NOTICE:
			aplog_type = APLOG_NOTICE;
			break;
#if LOG_INFO != LOG_NOTICE
		case LOG_INFO:
			aplog_type = APLOG_INFO;
			break;
#endif
#if LOG_NOTICE != LOG_DEBUG
		case LOG_DEBUG:
			aplog_type = APLOG_DEBUG;
			break;
#endif
	}

	if (ctx == NULL) { /* we haven't initialized our ctx yet, oh well */
		ap_log_error(APLOG_MARK, APLOG_ERR | APLOG_STARTUP, 0, NULL, "%s", msg);
	} else {
		ap_log_rerror(APLOG_MARK, aplog_type, 0, ctx->r, "%s", msg);
	}
}

static void php_apache_sapi_log_message_ex(const char *msg, request_rec *r)
{
	if (r) {
		ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, msg, r->filename);
	} else {
		php_apache_sapi_log_message(msg, -1);
	}
}

static zend_result php_apache_sapi_get_request_time(double *request_time)
{
	php_struct *ctx = SG(server_context);
	if (!ctx) {
		return FAILURE;
	}

	*request_time = ((double) ctx->r->request_time) / 1000000.0;
	return SUCCESS;
}

extern zend_module_entry php_apache_module;

static int php_apache2_startup(sapi_module_struct *sapi_module)
{
	return php_module_startup(sapi_module, &php_apache_module);
}

static int php_apache2_child_startup(sapi_module_struct *sapi_module)
{
	return php_module_child_startup(sapi_module);
}

static sapi_module_struct apache2_sapi_module = {
	"apache2handler",
	"Apache 2.0 Handler",

	php_apache2_startup,				/* startup */
	php_module_shutdown_wrapper,			/* shutdown */

	php_apache2_child_startup,			/* child_startup */

	NULL,						/* activate */
	NULL,						/* deactivate */

	php_apache_sapi_ub_write,			/* unbuffered write */
	php_apache_sapi_flush,				/* flush */
	php_apache_sapi_get_stat,			/* get uid */
	php_apache_sapi_getenv,				/* getenv */

	php_error,					/* error handler */

	php_apache_sapi_header_handler,			/* header handler */
	php_apache_sapi_send_headers,			/* send headers handler */
	NULL,						/* send header handler */

	php_apache_sapi_read_post,			/* read POST data */
	php_apache_sapi_read_cookies,			/* read Cookies */

	php_apache_sapi_register_variables,
	php_apache_sapi_log_message,			/* Log message */
	php_apache_sapi_get_request_time,		/* Request Time */
	NULL,						/* Child Terminate */

	STANDARD_SAPI_MODULE_PROPERTIES
};

static apr_status_t php_apache_server_shutdown(void *tmp)
{
	apache2_sapi_module.shutdown(&apache2_sapi_module);
	sapi_shutdown();
#ifdef ZTS
	tsrm_shutdown();
#endif
	return APR_SUCCESS;
}

static apr_status_t php_apache_child_shutdown(void *tmp)
{
	apache2_sapi_module.shutdown(&apache2_sapi_module);
#if defined(ZTS) && !defined(PHP_WIN32)
	tsrm_shutdown();
#endif
	return APR_SUCCESS;
}

static void php_apache_add_version(apr_pool_t *p)
{
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
php_apache_server_startup(apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s)
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
		apr_pool_userdata_set((const void *)1, userdata_key, apr_pool_cleanup_null, s->process->pool);
		return OK;
	}

	/* Set up our overridden path. */
	if (apache2_php_ini_path_override) {
		apache2_sapi_module.php_ini_path_override = apache2_php_ini_path_override;
	}
#ifdef ZTS
	int expected_threads;
#ifdef AP_MPMQ_MAX_THREADS
	if (ap_mpm_query(AP_MPMQ_MAX_THREADS, &expected_threads) != APR_SUCCESS) {
		expected_threads = 1;
	}
#else
	expected_threads = 1;
#endif

	php_tsrm_startup_ex(expected_threads);
# ifdef PHP_WIN32
	ZEND_TSRMLS_CACHE_UPDATE();
# endif
#endif

	zend_signal_startup();

	sapi_startup(&apache2_sapi_module);
	if (apache2_sapi_module.startup(&apache2_sapi_module) != SUCCESS) {
		return DONE;
	}
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

static int php_apache_request_ctor(request_rec *r, php_struct *ctx)
{
	char *content_length;
	const char *auth;

	SG(sapi_headers).http_response_code = !r->status ? HTTP_OK : r->status;
	SG(request_info).content_type = apr_table_get(r->headers_in, "Content-Type");
	SG(request_info).query_string = apr_pstrdup(r->pool, r->args);
	SG(request_info).request_method = r->method;
	SG(request_info).proto_num = r->proto_num;
	SG(request_info).request_uri = apr_pstrdup(r->pool, r->uri);
	SG(request_info).path_translated = apr_pstrdup(r->pool, r->filename);
	r->no_local_copy = 1;

	content_length = (char *) apr_table_get(r->headers_in, "Content-Length");
	if (content_length) {
		SG(request_info).content_length = ZEND_ATOL(content_length);
	} else {
		SG(request_info).content_length = 0;
	}

	apr_table_unset(r->headers_out, "Content-Length");
	apr_table_unset(r->headers_out, "Last-Modified");
	apr_table_unset(r->headers_out, "Expires");
	apr_table_unset(r->headers_out, "ETag");

	auth = apr_table_get(r->headers_in, "Authorization");
	php_handle_auth_data(auth);

	if (SG(request_info).auth_user == NULL && r->user) {
		SG(request_info).auth_user = estrdup(r->user);
	}

	ctx->r->user = apr_pstrdup(ctx->r->pool, SG(request_info).auth_user);

	return php_request_startup();
}

static void php_apache_request_dtor(request_rec *r)
{
	php_request_shutdown(NULL);
}

static void php_apache_ini_dtor(request_rec *r, request_rec *p)
{
	if (strcmp(r->protocol, "INCLUDED")) {
		zend_try { zend_ini_deactivate(); } zend_end_try();
	} else {
typedef struct {
	HashTable config;
} php_conf_rec;
		zend_string *str;
		php_conf_rec *c = ap_get_module_config(r->per_dir_config, &php_module);

		ZEND_HASH_MAP_FOREACH_STR_KEY(&c->config, str) {
			zend_restore_ini_entry(str, ZEND_INI_STAGE_SHUTDOWN);
		} ZEND_HASH_FOREACH_END();
	}
	if (p) {
		((php_struct *)SG(server_context))->r = p;
	} else {
		apr_pool_cleanup_run(r->pool, (void *)&SG(server_context), php_server_context_cleanup);
	}
}

static int php_handler(request_rec *r)
{
	php_struct * volatile ctx;
	void *conf;
	apr_bucket_brigade * volatile brigade;
	apr_bucket *bucket;
	apr_status_t rv;
	request_rec * volatile parent_req = NULL;
#ifdef ZTS
	/* initial resource fetch */
	(void)ts_resource(0);
# ifdef PHP_WIN32
	ZEND_TSRMLS_CACHE_UPDATE();
# endif
#endif

#define PHPAP_INI_OFF php_apache_ini_dtor(r, parent_req);

	conf = ap_get_module_config(r->per_dir_config, &php_module);

	/* apply_config() needs r in some cases, so allocate server_context early */
	ctx = SG(server_context);
	if (ctx == NULL || (ctx && ctx->request_processed && !strcmp(r->protocol, "INCLUDED"))) {
normal:
		ctx = SG(server_context) = apr_pcalloc(r->pool, sizeof(*ctx));
		/* register a cleanup so we clear out the SG(server_context)
		 * after each request. Note: We pass in the pointer to the
		 * server_context in case this is handled by a different thread.
		 */
		apr_pool_cleanup_register(r->pool, (void *)&SG(server_context), php_server_context_cleanup, apr_pool_cleanup_null);
		ctx->r = r;
		ctx = NULL; /* May look weird to null it here, but it is to catch the right case in the first_try later on */
	} else {
		parent_req = ctx->r;
		ctx->r = r;
	}
	apply_config(conf);

	if (strcmp(r->handler, PHP_MAGIC_TYPE) && strcmp(r->handler, PHP_SOURCE_MAGIC_TYPE) && strcmp(r->handler, PHP_SCRIPT)) {
		/* Check for xbithack in this case. */
		if (!AP2(xbithack) || strcmp(r->handler, "text/html") || !(r->finfo.protection & APR_UEXECUTE)) {
			PHPAP_INI_OFF;
			return DECLINED;
		}
	}

	/* Give a 404 if PATH_INFO is used but is explicitly disabled in
	 * the configuration; default behaviour is to accept. */
	if (r->used_path_info == AP_REQ_REJECT_PATH_INFO
		&& r->path_info && r->path_info[0]) {
		PHPAP_INI_OFF;
		return HTTP_NOT_FOUND;
	}

	/* handle situations where user turns the engine off */
	if (!AP2(engine)) {
		PHPAP_INI_OFF;
		return DECLINED;
	}

	if (r->finfo.filetype == 0) {
		php_apache_sapi_log_message_ex("script '%s' not found or unable to stat", r);
		PHPAP_INI_OFF;
		return HTTP_NOT_FOUND;
	}
	if (r->finfo.filetype == APR_DIR) {
		php_apache_sapi_log_message_ex("attempt to invoke directory '%s' as script", r);
		PHPAP_INI_OFF;
		return HTTP_FORBIDDEN;
	}

	/* Setup the CGI variables if this is the main request */
	if (r->main == NULL ||
		/* .. or if the sub-request environment differs from the main-request. */
		r->subprocess_env != r->main->subprocess_env
	) {
		/* setup standard CGI variables */
		ap_add_common_vars(r);
		ap_add_cgi_vars(r);
	}

zend_first_try {

	if (ctx == NULL) {
		brigade = apr_brigade_create(r->pool, r->connection->bucket_alloc);
		ctx = SG(server_context);
		ctx->brigade = brigade;

		if (php_apache_request_ctor(r, ctx)!=SUCCESS) {
			zend_bailout();
		}
	} else {
		if (!parent_req) {
			parent_req = ctx->r;
		}
		if (parent_req && parent_req->handler &&
				strcmp(parent_req->handler, PHP_MAGIC_TYPE) &&
				strcmp(parent_req->handler, PHP_SOURCE_MAGIC_TYPE) &&
				strcmp(parent_req->handler, PHP_SCRIPT)) {
			if (php_apache_request_ctor(r, ctx)!=SUCCESS) {
				zend_bailout();
			}
		}

		/*
		 * check if coming due to ErrorDocument
		 * We make a special exception of 413 (Invalid POST request) as the invalidity of the request occurs
		 * during processing of the request by PHP during POST processing. Therefore we need to re-use the exiting
		 * PHP instance to handle the request rather then creating a new one.
		*/
		if (parent_req && parent_req->status != HTTP_OK && parent_req->status != 413 && strcmp(r->protocol, "INCLUDED")) {
			parent_req = NULL;
			goto normal;
		}
		ctx->r = r;
		brigade = ctx->brigade;
	}

	if (AP2(last_modified)) {
		ap_update_mtime(r, r->finfo.mtime);
		ap_set_last_modified(r);
	}

	/* Determine if we need to parse the file or show the source */
	if (strncmp(r->handler, PHP_SOURCE_MAGIC_TYPE, sizeof(PHP_SOURCE_MAGIC_TYPE) - 1) == 0) {
		zend_syntax_highlighter_ini syntax_highlighter_ini;
		php_get_highlight_struct(&syntax_highlighter_ini);
		highlight_file((char *)r->filename, &syntax_highlighter_ini);
	} else {
		zend_file_handle zfd;
		zend_stream_init_filename(&zfd, (char *) r->filename);
		zfd.primary_script = 1;

		if (!parent_req) {
			php_execute_script(&zfd);
		} else {
			zend_execute_scripts(ZEND_INCLUDE, NULL, 1, &zfd);
		}
		zend_destroy_file_handle(&zfd);

		apr_table_set(r->notes, "mod_php_memory_usage",
			apr_psprintf(ctx->r->pool, "%" APR_SIZE_T_FMT, zend_memory_peak_usage(1)));
	}

} zend_end_try();

	if (!parent_req) {
		php_apache_request_dtor(r);
		ctx->request_processed = 1;
		apr_brigade_cleanup(brigade);
		bucket = apr_bucket_eos_create(r->connection->bucket_alloc);
		APR_BRIGADE_INSERT_TAIL(brigade, bucket);

		rv = ap_pass_brigade(r->output_filters, brigade);
		if (rv != APR_SUCCESS || r->connection->aborted) {
zend_first_try {
			php_handle_aborted_connection();
} zend_end_try();
		}
		apr_brigade_cleanup(brigade);
		apr_pool_cleanup_run(r->pool, (void *)&SG(server_context), php_server_context_cleanup);
	} else {
		ctx->r = parent_req;
	}

	return OK;
}

static void php_apache_child_init(apr_pool_t *pchild, server_rec *s)
{
	apr_pool_cleanup_register(pchild, NULL, php_apache_child_shutdown, apr_pool_cleanup_null);
	apache2_sapi_module.child_startup(&apache2_sapi_module);
}

#ifdef ZEND_SIGNALS
static void php_apache_signal_init(apr_pool_t *pchild, server_rec *s)
{
	zend_signal_init();
}
#endif

void php_ap2_register_hook(apr_pool_t *p)
{
	ap_hook_pre_config(php_pre_config, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_post_config(php_apache_server_startup, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_handler(php_handler, NULL, NULL, APR_HOOK_MIDDLE);
#ifdef ZEND_SIGNALS
	ap_hook_child_init(php_apache_signal_init, NULL, NULL, APR_HOOK_MIDDLE);
#endif
	ap_hook_child_init(php_apache_child_init, NULL, NULL, APR_HOOK_MIDDLE);
}
