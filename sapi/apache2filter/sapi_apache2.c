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
php_apache_sapi_ub_write(const char *str, uint str_length)
{
	ap_bucket *b;
	ap_bucket_brigade *bb;
	php_struct *ctx;
	uint now;
	SLS_FETCH();

	ctx = SG(server_context);

	bb = ap_brigade_create(ctx->f->r->pool);
	while (str_length > 0) {
		now = MIN(str_length, 4096);
		b = ap_bucket_create_transient(str, now);
		AP_BRIGADE_INSERT_TAIL(bb, b);
		str += now;
		str_length -= now;
	}
	ap_pass_brigade(ctx->f->next, bb);
	
	return str_length;
}

static int
php_apache_sapi_header_handler(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers SLS_DC)
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
php_apache_sapi_send_headers(sapi_headers_struct *sapi_headers SLS_DC)
{
	php_struct *ctx = SG(server_context);

	ctx->f->r->status = SG(sapi_headers).http_response_code;

	return SAPI_HEADER_SENT_SUCCESSFULLY;
}

static int
php_apache_sapi_read_post(char *buf, uint count_bytes SLS_DC)
{

	return 0;
}

static char *
php_apache_sapi_read_cookies(SLS_D)
{
	php_struct *ctx = SG(server_context);
	const char *http_cookie;

	http_cookie = apr_table_get(ctx->f->r->headers_in, "cookie");

	/* The SAPI interface should use 'const char *' */
	return (char *) http_cookie;
}

static void
php_apache_sapi_register_variables(zval *track_vars_array ELS_DC SLS_DC PLS_DC)
{
	php_struct *ctx = SG(server_context);
	apr_array_header_t *arr = apr_table_elts(ctx->f->r->subprocess_env);
	apr_table_entry_t *elts = (apr_table_entry_t *) arr->elts;
	int i;
	char *val;
	
	for (i = 0; i < arr->nelts; i++) {
		if (!(val = elts[i].val))
			val = empty_string;

		php_register_variable(elts[i].key, val, track_vars_array ELS_CC PLS_CC);
	}
	
	php_register_variable("PHP_SELF", ctx->f->r->uri, track_vars_array ELS_CC PLS_CC);
}

static void
php_apache_sapi_flush(void *server_context)
{
	php_struct *ctx = server_context;
	ap_bucket_brigade *bb;
	ap_bucket *b;

	/* Send a flush bucket down the filter chain. The current default
	 * handler seems to act on the first flush bucket, but ignores
	 * all further flush buckets.
	 */
	
	bb = ap_brigade_create(ctx->f->r->pool);
	b = ap_bucket_create_flush();
	AP_BRIGADE_INSERT_TAIL(bb, b);
	ap_pass_brigade(ctx->f->next, bb);
}

static void php_apache_sapi_log_message(char *msg)
{
	php_struct *ctx;
	SLS_FETCH();

	ctx = SG(server_context);

	apr_fprintf(ctx->f->r->server->error_log, "%s", msg);
}

static sapi_module_struct sapi_module = {
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


module MODULE_VAR_EXPORT php4_module;

static int php_filter(ap_filter_t *f, ap_bucket_brigade *bb)
{
	php_struct *ctx = f->ctx;
	ap_bucket *b;
	apr_status_t rv;
	const char *str;
	apr_ssize_t n;
	void *conf = ap_get_module_config(f->r->per_dir_config,
			&php4_module);
	SLS_FETCH();

	if (ctx == NULL) {
		/* Initialize filter context */
		SG(server_context) = f->ctx = ctx = apr_pcalloc(f->r->pool, sizeof(*ctx));
		ctx->bb = ap_brigade_create(f->c->pool);
	}

	ctx->f = f;

	/* states:
	 * 0: request startup
	 * 1: collecting data
	 * 2: script execution and request shutdown
	 */
	if (ctx->state == 0) {
		char *content_type;
		const char *auth;
		CLS_FETCH();
		ELS_FETCH();
		PLS_FETCH();
		SLS_FETCH();
	
		apply_config(conf);
		php_request_startup(CLS_C ELS_CC PLS_CC SLS_CC);
		
		ctx->state++;

		/* XXX: Lots of startup crap. Should be moved into its own func */
		PG(during_request_startup) = 0;
		SG(sapi_headers).http_response_code = 200;
		SG(request_info).query_string = f->r->args;
		SG(request_info).request_method = f->r->method;
		SG(request_info).request_uri = f->r->uri;
		f->r->no_cache = f->r->no_local_copy = 1;
		content_type = sapi_get_default_content_type(SLS_C);
		f->r->content_type = apr_pstrdup(f->r->pool, content_type);
		efree(content_type);
		apr_table_unset(f->r->headers_out, "Content-Length");
		apr_table_unset(f->r->headers_out, "Last-Modified");
		apr_table_unset(f->r->headers_out, "Expires");
		auth = apr_table_get(f->r->headers_in, "Authorization");
		php_handle_auth_data(auth SLS_CC);
	}

	/* moves all buckets from bb to ctx->bb */
	ap_save_brigade(f, &ctx->bb, &bb);

	/* If we have received all data from the previous filters,
	 * we "flatten" the buckets by creating a single string buffer.
	 */
	if (ctx->state == 1 && AP_BUCKET_IS_EOS(AP_BRIGADE_LAST(ctx->bb))) {
		int fd;
		zend_file_handle zfd;
		smart_str content = {0};
		ap_bucket *eos;
		CLS_FETCH();
		ELS_FETCH();
		PLS_FETCH();

		/* We want to execute only one script per request.
		 * A bug in Apache or other filters could cause us
		 * to reenter php_filter during script execution, so
		 * we protect ourselves here.
		 */
		ctx->state = 2;

		/* Handle phpinfo/phpcredits/built-in images */
		if (php_handle_special_queries(SLS_C PLS_CC)) 
			goto ok;
	
		/* Loop over all buckets and put them into the buffer */	
		AP_BRIGADE_FOREACH(b, ctx->bb) {
			rv = ap_bucket_read(b, &str, &n, 1);
			if (rv == APR_SUCCESS && n > 0)
				smart_str_appendl(&content, str, n);
		}
		
		/* Empty script */
		if (!content.c) goto skip_execution;
		
		smart_str_0(&content);

		/* 
		 * This hack is used only for testing purposes and will
		 * go away when the scripting engine will be able to deal
		 * with something more complex than files/-handles.
		 */
		
#if 1
#define FFFF "/tmp/really_silly"
		fd = open(FFFF, O_WRONLY|O_TRUNC|O_CREAT, 0600);
		
		write(fd, content.c, content.len);

		close(fd);

		zfd.type = ZEND_HANDLE_FILENAME;
		zfd.filename = FFFF;
		zfd.free_filename = 0;
		zfd.opened_path = NULL;
		
		php_execute_script(&zfd CLS_CC ELS_CC PLS_CC);
#else
		CG(in_compilation) = 1;
		zend_eval_string(content, NULL, "foo" CLS_CC ELS_CC);
#endif

		smart_str_free(&content);
		goto ok;
skip_execution:
#define NO_DATA "php_filter did not get ANY data"
		eos = ap_bucket_create_transient(NO_DATA, sizeof(NO_DATA)-1);
		AP_BRIGADE_INSERT_HEAD(bb, eos);
ok:
		php_request_shutdown(NULL);

		/* Pass EOS bucket to next filter to signal end of request */
		eos = ap_bucket_create_flush();
		AP_BRIGADE_INSERT_TAIL(bb, eos);
		eos = ap_bucket_create_eos();
		AP_BRIGADE_INSERT_TAIL(bb, eos);
		ap_pass_brigade(f->next, bb);
	} else
		ap_brigade_destroy(bb);

	return APR_SUCCESS;
}

static apr_status_t
php_apache_server_shutdown(void *tmp)
{
	sapi_module.shutdown(&sapi_module);
	sapi_shutdown();
	tsrm_shutdown();
	return APR_SUCCESS;
}

static void
php_apache_server_startup(apr_pool_t *pchild, server_rec *s)
{
	tsrm_startup(1, 1, 0);
	sapi_startup(&sapi_module);
	sapi_module.startup(&sapi_module);
	apr_register_cleanup(pchild, NULL, php_apache_server_shutdown, NULL);
}

static void php_register_hook(void)
{
	ap_hook_child_init(php_apache_server_startup, NULL, NULL, AP_HOOK_MIDDLE);
    ap_register_output_filter("PHP", php_filter, AP_FTYPE_CONTENT);
}

module MODULE_VAR_EXPORT php4_module = {
    STANDARD20_MODULE_STUFF,
    create_php_config,		/* create per-directory config structure */
    merge_php_config,        		/* merge per-directory config structures */
    NULL,			/* create per-server config structure */
    NULL,			/* merge per-server config structures */
    dir_cmds,			/* command apr_table_t */
    NULL,          		/* handlers */
    php_register_hook		/* register hooks */
};
