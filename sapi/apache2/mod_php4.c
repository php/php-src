/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#define NO_REGEX_EXTRA_H
#ifdef WIN32
#include <winsock2.h>
#include <stddef.h>
#endif

#include "zend.h"
#include "php.h"
#include "php_variables.h"

#include "apr_strings.h"
#include "ap_buckets.h"
#include "util_filter.h"
#include "ap_config.h"
#include "httpd.h"
#include "http_config.h"
#include "http_request.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_main.h"
#include "http_log.h"
#include "util_script.h"
#include "http_conf_globals.h"

#include "php_ini.h"
#include "php_globals.h"
#include "SAPI.h"
#include "php_main.h"

#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_highlight.h"
#include "zend_indent.h"

#include "ext/standard/php_standard.h"

#include "util_script.h"

#include "mod_php4.h"

#undef shutdown

int apache_php_module_main(request_rec *r, int display_source_mode CLS_DC ELS_DC PLS_DC SLS_DC);
void php_save_umask(void);
void php_restore_umask(void);
int sapi_apache_read_post(char *buffer, uint count_bytes SLS_DC);
char *sapi_apache_read_cookies(SLS_D);
int sapi_apache_header_handler(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers SLS_DC);
int sapi_apache_send_headers(sapi_headers_struct *sapi_headers SLS_DC);
int send_php(request_rec *r, int display_source_mode, char *filename);
int send_parsed_php(request_rec * r);
int send_parsed_php_source(request_rec * r);
int php_xbithack_handler(request_rec * r);
void php_init_handler(server_rec *s, pool *p);

static void php_child_exit_handler(server_rec *s, pool *p);

#define CONST_PREFIX const
CONST_PREFIX char *php_apache_value_handler_ex(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2, int mode);
CONST_PREFIX char *php_apache_value_handler(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2);
CONST_PREFIX char *php_apache_admin_value_handler(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2);
CONST_PREFIX char *php_apache_flag_handler(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2);
CONST_PREFIX char *php_apache_flag_handler_ex(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2, int mode);
CONST_PREFIX char *php_apache_admin_flag_handler(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2);

/* ### these should be defined in mod_php4.h or somewhere else */
#define USE_PATH 1
#define IGNORE_URL 2

module MODULE_VAR_EXPORT php4_module;

int saved_umask;
static unsigned char apache_php_initialized;

/* per-thread globals (when the ZTS stuff gets added) */
typedef struct {
	ap_filter_t *server_filter;
} php_ap_globals_struct;

/* Needs to be per-thread when in threaded mode (per-server for now in pre-fork
 * mode) */
static php_ap_globals_struct *php_ap_globals;

typedef struct _php_per_dir_entry {
	char *key;
	char *value;
	uint key_length;
	uint value_length;
	int type;
} php_per_dir_entry;

php_apache_info_struct php_apache_info;		/* active config */

/* some systems are missing these from their header files */

void php_save_umask(void)
{
	saved_umask = umask(077);
	umask(saved_umask);
}


static int sapi_apache_ub_write(const char *str, uint str_length)
{
	int ret;
	ap_bucket *b;
	ap_bucket_brigade = *bb;
	SLS_FETCH();
	PLS_FETCH();
		
	if (SG(server_context)) {
		bb = ap_brigade_create(r->pool);
		b = ap_bucket_create_immortal(str, str_length);
		AP_BRIGADE_INSERT_TAIL(bb, b);
		ap_pass_brigade((php_ap_globals->server_filter)->next, bb);
	} else {
		ret = fwrite(str, 1, str_length, stderr);
	}
	if(ret != str_length) {
		PG(connection_status) = PHP_CONNECTION_ABORTED;
		if (!PG(ignore_user_abort)) {
			zend_bailout();
		}
	}
	return ret;
}



static void sapi_apache_flush(void *server_context)
{
	if (server_context) {
		ap_rflush((request_rec *) server_context);
	}
}


int sapi_apache_read_post(char *buffer, uint count_bytes SLS_DC)
{
	uint total_read_bytes=0, read_bytes;
	request_rec *r = (request_rec *) SG(server_context);
	void (*handler)(int);

	handler = signal(SIGPIPE, SIG_IGN);
	while (total_read_bytes<count_bytes) {
		hard_timeout("Read POST information", r); /* start timeout timer */
		read_bytes = get_client_block(r, buffer+total_read_bytes, count_bytes-total_read_bytes);
		reset_timeout(r);
		if (read_bytes<=0) {
			break;
		}
		total_read_bytes += read_bytes;
	}
	signal(SIGPIPE, handler);	
	return total_read_bytes;
}


char *sapi_apache_read_cookies(SLS_D)
{
	return (char *) table_get(((request_rec *) SG(server_context))->subprocess_env, "HTTP_COOKIE");
}


int sapi_apache_header_handler(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers SLS_DC)
{
	char *header_name, *header_content, *p;
	request_rec *r = (request_rec *) SG(server_context);

	header_name = sapi_header->header;

	header_content = p = strchr(header_name, ':');
	if (!p) {
		return 0;
	}

	*p = 0;
	do {
		header_content++;
	} while (*header_content==' ');

	if (!strcasecmp(header_name, "Content-Type")) {
		r->content_type = pstrdup(r->pool, header_content);
	} else if (!strcasecmp(header_name, "Set-Cookie")) {
		table_add(r->headers_out, header_name, header_content);
	} else {
		table_set(r->headers_out, header_name, header_content);
	}

	*p = ':';  /* a well behaved header handler shouldn't change its original arguments */

	efree(sapi_header->header);
	
	return 0;  /* don't use the default SAPI mechanism, Apache duplicates this functionality */
}


int sapi_apache_send_headers(sapi_headers_struct *sapi_headers SLS_DC)
{
	if(SG(server_context) == NULL) { /* server_context is not here anymore */
		return SAPI_HEADER_SEND_FAILED;
	}

	((request_rec *) SG(server_context))->status = SG(sapi_headers).http_response_code;
	ap_send_http_header((request_rec *) SG(server_context));
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}


static void sapi_apache_register_server_variables(zval *track_vars_array ELS_DC SLS_DC PLS_DC)
{
	register int i;
	array_header *arr = table_elts(((request_rec *) SG(server_context))->subprocess_env);
	table_entry *elts = (table_entry *) arr->elts;
	char *script_filename=NULL;

	for (i = 0; i < arr->nelts; i++) {
		char *val;

		if (elts[i].val) {
			val = elts[i].val;
			if (!strcmp(elts[i].key, "SCRIPT_FILENAME")) {
				script_filename = val;
			}
		} else {
			val = empty_string;
		}
		php_register_variable(elts[i].key, val, track_vars_array  ELS_CC PLS_CC);
	}

	/* insert special variables */
	if (script_filename) {
		php_register_variable("PATH_TRANSLATED", script_filename, track_vars_array ELS_CC PLS_CC);
	}
	php_register_variable("PHP_SELF", ((request_rec *) SG(server_context))->uri, track_vars_array ELS_CC PLS_CC);
}

static int php_apache_startup(sapi_module_struct *sapi_module)
{
    if(php_module_startup(sapi_module) == FAILURE
            || zend_startup_module(&apache_module_entry) == FAILURE) {
        return FAILURE;
    } else {
        return SUCCESS;
    }
}


static void php_apache_log_message(char *message)
{
	SLS_FETCH();

	if (SG(server_context)) {
		aplog_error(NULL, 0, APLOG_ERR | APLOG_NOERRNO, ((request_rec *) SG(server_context))->server, "%s", message);
	} else {
		fprintf(stderr, "%s", message);
		fprintf(stderr, "\n");
	}
}


static void php_apache_request_shutdown(void *dummy)
{
	SLS_FETCH();

	SG(server_context) = NULL; /* The server context (request) is invalid by the time run_cleanups() is called */
	php_request_shutdown(dummy);
}


static int php_apache_sapi_activate(SLS_D)
{
	request_rec *r = ((request_rec *) SG(server_context));

	/*
	 * For the Apache module version, this bit of code registers a cleanup
	 * function that gets triggered when our request pool is destroyed.
	 * We need this because at any point in our code we can be interrupted
	 * and that may happen before we have had time to free our memory.
	 * The php_request_shutdown function needs to free all outstanding allocated
	 * memory.  
	 */
	block_alarms();
	register_cleanup(((request_rec *) SG(server_context))->pool, NULL, php_apache_request_shutdown, php_request_shutdown_for_exec);
	unblock_alarms();

	/* Override the default headers_only value - sometimes "GET" requests should actually only
	 * send headers.
	 */
	SG(request_info).headers_only = r->header_only;
	return SUCCESS;
}


static struct stat *php_apache_get_stat(SLS_D)
{
	return &((request_rec *) SG(server_context))->finfo;
}


static char *php_apache_getenv(char *name, size_t name_len SLS_DC)
{
	return (char *) table_get(((request_rec *) SG(server_context))->subprocess_env, name);
}


static sapi_module_struct sapi_module_conf = {
	"apache",						/* name */
	"Apache",						/* pretty name */
									
	php_apache_startup,				/* startup */
	php_module_shutdown_wrapper,	/* shutdown */

	php_apache_sapi_activate,		/* activate */
	NULL,							/* deactivate */

	sapi_apache_ub_write,			/* unbuffered write */
	sapi_apache_flush,				/* flush */
	php_apache_get_stat,			/* get uid */
	php_apache_getenv,				/* getenv */

	php_error,						/* error handler */

	sapi_apache_header_handler,		/* header handler */
	sapi_apache_send_headers,		/* send headers handler */
	NULL,							/* send header handler */

	sapi_apache_read_post,			/* read POST data */
	sapi_apache_read_cookies,		/* read Cookies */

	sapi_apache_register_server_variables,		/* register server variables */
	php_apache_log_message,			/* Log message */

#ifdef PHP_WIN32
	NULL,
	NULL,
#else
	block_alarms,					/* Block interruptions */
	unblock_alarms,					/* Unblock interruptions */
#endif

	STANDARD_SAPI_MODULE_PROPERTIES
};


void php_restore_umask(void)
{
	umask(saved_umask);
}


static void init_request_info(SLS_D)
{
	request_rec *r = ((request_rec *) SG(server_context));
	char *content_length = (char *) table_get(r->subprocess_env, "CONTENT_LENGTH");
	const char *authorization=NULL;
	char *tmp;

	SG(request_info).query_string = r->args;
	SG(request_info).path_translated = r->filename;
	SG(request_info).request_uri = r->uri;
	SG(request_info).request_method = (char *)r->method;
	SG(request_info).content_type = (char *) table_get(r->subprocess_env, "CONTENT_TYPE");
	SG(request_info).content_length = (content_length ? atoi(content_length) : 0);
	SG(sapi_headers).http_response_code = r->status;

	if (r->headers_in) {
		authorization = table_get(r->headers_in, "Authorization");
	}
	if (authorization
/* 		&& !auth_type(r) */
		&& !strcasecmp(getword(r->pool, &authorization, ' '), "Basic")) {
		tmp = uudecode(r->pool, authorization);
		SG(request_info).auth_user = getword_nulls_nc(r->pool, &tmp, ':');
		if (SG(request_info).auth_user) {
			r->connection->user = pstrdup(r->connection->pool,SG(request_info).auth_user);
			r->connection->ap_auth_type = "Basic";
			SG(request_info).auth_user = estrdup(SG(request_info).auth_user);
		}
		SG(request_info).auth_password = tmp;
		if (SG(request_info).auth_password) {
			SG(request_info).auth_password = estrdup(SG(request_info).auth_password);
		}
	} else {
		SG(request_info).auth_user = NULL;
		SG(request_info).auth_password = NULL;
	}
}


static int php_apache_alter_ini_entries(php_per_dir_entry *per_dir_entry)
{
	php_alter_ini_entry(per_dir_entry->key, per_dir_entry->key_length+1, per_dir_entry->value, per_dir_entry->value_length+1, per_dir_entry->type, PHP_INI_STAGE_ACTIVATE);
	return 0;
}

static char *php_apache_get_default_mimetype(request_rec *r SLS_DC)
{
	
	char *mimetype;
	if (SG(default_mimetype) || SG(default_charset)) {
		/* Assume output will be of the default MIME type.  Individual
		   scripts may change this later. */
		char *tmpmimetype;
		tmpmimetype = sapi_get_default_content_type(SLS_C);
		mimetype = pstrdup(r->pool, tmpmimetype);
		efree(tmpmimetype);
	} else {
		mimetype = SAPI_DEFAULT_MIMETYPE "; charset=" SAPI_DEFAULT_CHARSET;
	}
	return mimetype;
}

int send_php(request_rec *r, int display_source_mode, char *filename)
{
	int retval;
	HashTable *per_dir_conf;
	SLS_FETCH();
	ELS_FETCH();
	CLS_FETCH();
	PLS_FETCH();

	if (setjmp(EG(bailout))!=0) {
		return OK;
	}
	per_dir_conf = (HashTable *) ap_get_module_config(r->per_dir_config, &php4_module);
	if (per_dir_conf) {
		zend_hash_apply((HashTable *) per_dir_conf, (int (*)(void *)) php_apache_alter_ini_entries);
	}


	/* Make sure file exists */
	if (filename == NULL && r->finfo.st_mode == 0) {
		return DECLINED;
	}

	/* If PHP parser engine has been turned off with an "engine off"
	 * directive, then decline to handle this request
	 */
	if (!php_apache_info.engine) {
		r->content_type = php_apache_get_default_mimetype(r SLS_CC);
		r->allowed |= (1 << METHODS) - 1;
		return DECLINED;
	}
	if (filename == NULL) {
		filename = r->filename;
	}

	if ((retval = ap_setup_client_block(r, REQUEST_CHUNKED_ERROR)))
		return retval;
	if (php_apache_info.last_modified) {
		ap_update_mtime (r, r->finfo.st_mtime);
		ap_set_last_modified(r);
		ap_set_etag(r);
	}
	/* Assume output will be of the default MIME type.  Individual
	   scripts may change this later in the request. */
	r->content_type = php_apache_get_default_mimetype(r SLS_CC);

	SG(server_context) = r;
	
	php_save_umask();
	ap_add_common_vars(r);
	ap_add_cgi_vars(r);

	init_request_info(SLS_C);
	apache_php_module_main(r, display_source_mode CLS_CC ELS_CC PLS_CC SLS_CC);

	/* Done, restore umask, turn off timeout, close file and return */
	php_restore_umask();
	return OK;
}


int send_parsed_php(request_rec * r)
{
	return send_php(r, 0, NULL);
}


int send_parsed_php_source(request_rec * r)
{
	return send_php(r, 1, NULL);
}


static void destroy_per_dir_entry(php_per_dir_entry *per_dir_entry)
{
	free(per_dir_entry->key);
	free(per_dir_entry->value);
}

static void copy_per_dir_entry(php_per_dir_entry *per_dir_entry)
{
	php_per_dir_entry tmp = *per_dir_entry;

	per_dir_entry->key = (char *) malloc(tmp.key_length+1);
	memcpy(per_dir_entry->key, tmp.key, tmp.key_length);
	per_dir_entry->key[per_dir_entry->key_length] = 0;

	per_dir_entry->value = (char *) malloc(tmp.value_length+1);
	memcpy(per_dir_entry->value, tmp.value, tmp.value_length);
	per_dir_entry->value[per_dir_entry->value_length] = 0;
}


static zend_bool should_overwrite_per_dir_entry(php_per_dir_entry *orig_per_dir_entry, php_per_dir_entry *new_per_dir_entry)
{
	if (new_per_dir_entry->type==PHP_INI_SYSTEM
		&& orig_per_dir_entry->type!=PHP_INI_SYSTEM) {
		return 1;
	} else {
		return 0;
	}
}


static void php_destroy_per_dir_info(HashTable *per_dir_info)
{
	zend_hash_destroy(per_dir_info);
	free(per_dir_info);
}


static void *php_create_dir(pool *p, char *dummy)
{
	HashTable *per_dir_info;

	per_dir_info = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init(per_dir_info, 5, NULL, (void (*)(void *)) destroy_per_dir_entry, 1);
	register_cleanup(p, (void *) per_dir_info, (void (*)(void *)) php_destroy_per_dir_info, (void (*)(void *)) zend_hash_destroy);

	return per_dir_info;
}


static void *php_merge_dir(pool *p, void *basev, void *addv)
{
	php_per_dir_entry tmp;

	/* This function *must* return addv, and not modify basev */
	zend_hash_merge_ex((HashTable *) addv, (HashTable *) basev, (copy_ctor_func_t) copy_per_dir_entry, sizeof(php_per_dir_entry), (zend_bool (*)(void *, void *)) should_overwrite_per_dir_entry);
	/*zend_hash_merge((HashTable *) addv, (HashTable *) basev, (void (*)(void *)) copy_per_dir_entry, &tmp, sizeof(php_per_dir_entry), 0);*/
	return addv;
}


CONST_PREFIX char *php_apache_value_handler_ex(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2, int mode)
{
	php_per_dir_entry per_dir_entry;

	if (!apache_php_initialized) {
		apache_php_initialized = 1;
#ifdef ZTS
		tsrm_startup(1, 1, 0);
#endif
		sapi_startup(&sapi_module_conf);
		php_apache_startup(&sapi_module_conf);
	}
	per_dir_entry.type = mode;

	if (strcasecmp(arg2, "none") == 0) {
		arg2 = "";
	}

	per_dir_entry.key_length = strlen(arg1);
	per_dir_entry.value_length = strlen(arg2);

	per_dir_entry.key = (char *) malloc(per_dir_entry.key_length+1);
	memcpy(per_dir_entry.key, arg1, per_dir_entry.key_length);
	per_dir_entry.key[per_dir_entry.key_length] = 0;

	per_dir_entry.value = (char *) malloc(per_dir_entry.value_length+1);
	memcpy(per_dir_entry.value, arg2, per_dir_entry.value_length);
	per_dir_entry.value[per_dir_entry.value_length] = 0;

	zend_hash_update((HashTable *) conf, per_dir_entry.key, per_dir_entry.key_length, &per_dir_entry, sizeof(php_per_dir_entry), NULL);
	return NULL;
}


CONST_PREFIX char *php_apache_value_handler(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2)
{
	return php_apache_value_handler_ex(cmd, conf, arg1, arg2, PHP_INI_PERDIR);
}


CONST_PREFIX char *php_apache_admin_value_handler(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2)
{
	return php_apache_value_handler_ex(cmd, conf, arg1, arg2, PHP_INI_SYSTEM);
}


CONST_PREFIX char *php_apache_flag_handler_ex(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2, int mode)
{
	char bool_val[2];

	if (!strcasecmp(arg2, "On")) {
		bool_val[0] = '1';
	} else {
		bool_val[0] = '0';
	}
	bool_val[1] = 0;
	
	return php_apache_value_handler_ex(cmd, conf, arg1, bool_val, mode);
}


CONST_PREFIX char *php_apache_flag_handler(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2)
{
	return php_apache_flag_handler_ex(cmd, conf, arg1, arg2, PHP_INI_PERDIR);
}


CONST_PREFIX char *php_apache_admin_flag_handler(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2)
{
	return php_apache_flag_handler_ex(cmd, conf, arg1, arg2, PHP_INI_SYSTEM);
}




int php_xbithack_handler(request_rec * r)
{
	php_apache_info_struct *conf;

	conf = (php_apache_info_struct *) get_module_config(r->per_dir_config, &php4_module);
	if (!(r->finfo.st_mode & S_IXUSR)) {
		r->allowed |= (1 << METHODS) - 1;
		return DECLINED;
	}
	if (conf->xbithack == 0) {
		r->allowed |= (1 << METHODS) - 1;
		return DECLINED;
	}
	return send_parsed_php(r);
}

static void apache_php_module_shutdown_wrapper(void)
{
	apache_php_initialized = 0;
	sapi_module_conf.shutdown(&sapi_module_conf);

	/* This function is only called on server exit if the apache API
	 * child_exit handler exists, so shutdown globally 
	 */
	sapi_shutdown();

#ifdef ZTS
	tsrm_shutdown();
#endif
}

static void php_child_exit_handler(server_rec *s, pool *p)
{
/*	apache_php_initialized = 0; */
	sapi_module_conf.shutdown(&sapi_module_conf);

#ifdef ZTS
	tsrm_shutdown();
#endif
}

void php_init_handler(server_rec *s, pool *p)
{
	register_cleanup(p, NULL, (void (*)(void *))apache_php_module_shutdown_wrapper, (void (*)(void *))php_module_shutdown_for_exec);
	if (!apache_php_initialized) {
		apache_php_initialized = 1;
#ifdef ZTS
		tsrm_startup(1, 1, 0);
#endif
		sapi_startup(&sapi_module_conf);
		php_apache_startup(&sapi_module_conf);
	}
	{
		PLS_FETCH();
		if (PG(expose_php)) {
			ap_add_version_component("PHP/" PHP_VERSION);
		}
	}
}

typedef struct PHP_OUTPUT_FILTER_CTX {
	ap_bucket_brigade *bb;
} php_output_filter_ctx_t;

static int php_filter(ap_filter_t *f, ap_bucket_brigade *bb) {
	request_rec *r = f->r;
	ap_bucket *b;
	conn_rec *c = f->c;
	php_output_filter_ctx_t *ctx = f->ctx;
	apr_status_t rv;
	const char *str;
	apr_ssize_t n;
	long size = 0L;
	char *content, *p;

	/* We don't accept OPTIONS requests, but take everything else */
	if (r->method_number == M_OPTIONS) {
		r->allowed |= (1 << METHODS) - 1;
		return ap_pass_brigade(f->next, bb);
	}
	
	if (ctx == NULL) {
		f->ctx = ctx = apr_pcalloc(c->pool, sizeof(php_output_filter_ctx_t));
		ctx->b = ap_brigade_create(c->pool); /* create an initial empty brigade */
		php_ap_globals = palloc(sizeof *php_ap_globals);
		php_ap_globals->server_filter = f;
	}

	ap_save_brigade(f, ctx->bb, bb);

	if(AP_BUCKET_IS_EOS(AP_BRIGADE_LAST(b))) {
		/* Ok, we have all of our brigades, time to munch on the buckets */
		AP_BRIGADE_FOREACH(b, bb) {
			rv = ap_bucket_read(b, &str, &n, 1);
		}
		/* Because some of our buckets may be pipes, we can't actually get the
		 * total size of our brigade on our first pass, so run through them all
		 * again to get the total size of the brigade */
		AP_BRIGADE_FOREACH(b, bb) {
			size += b->length;
		}
		/* Now that we have the size we can allocate a big chunk of memory
		 * where we will memcpy all of the buckets into. */
		content = p = apr_pcalloc(c->pool, size+1);		
		/* And now we can copy the buckets into our buffer */
		AP_BRIGADE_FOREACH(b, bb) {
			memcpy(p, b->data, b->length);
			p += e->length;
		}
		/* We should now have a flat buffer in 'content' that we somehow have
		 * to get PHP to parse... */
	}
}

static void register_php_hooks (void) {
	ap_register_output_filter("PHP", php_filter, AP_FTYPE_CONTENT);	
}

static const handler_rec php_handlers[] =
{
	{"application/x-httpd-php", send_parsed_php},
	{"application/x-httpd-php-source", send_parsed_php_source},
	{"text/html", php_xbithack_handler},
	{NULL}
};

command_rec php_commands[] =
{
	{"php_value",		php_apache_value_handler, NULL, OR_OPTIONS, TAKE2, "PHP Value Modifier"},
	{"php_flag",		php_apache_flag_handler, NULL, OR_OPTIONS, TAKE2, "PHP Flag Modifier"},
	{"php_admin_value",	php_apache_admin_value_handler, NULL, ACCESS_CONF|RSRC_CONF, TAKE2, "PHP Value Modifier (Admin)"},
	{"php_admin_flag",	php_apache_admin_flag_handler, NULL, ACCESS_CONF|RSRC_CONF, TAKE2, "PHP Flag Modifier (Admin)"},
	{NULL}
};

module AP_MODULE_DECLARE_DATA php_module =
{
	STANDARD20_MODULE_STUFF,
	php_create_dir,          /* dir config creater */
	php_merge_dir,           /* dir merger --- default is to override */
	NULL,                    /* server config */
	NULL,                    /* merge server config */
	php_commands,            /* command apr_table_t */
	php_handlers,            /* handlers */
	register_php_hooks       /* register hooks */
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
