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
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   | (with helpful hints from Dean Gaudet <dgaudet@arctic.org>            |
   | PHP 4.0 patches by Zeev Suraski <zeev@zend.com>                      |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php_apache_http.h"
#include "http_conf_globals.h"

#ifdef NETWARE
#define SIGPIPE SIGINT
#endif

#undef shutdown

/* {{{ Prototypes
 */
int apache_php_module_main(request_rec *r, int display_source_mode TSRMLS_DC);
static void php_save_umask(void);
static void php_restore_umask(void);
static int sapi_apache_read_post(char *buffer, uint count_bytes TSRMLS_DC);
static char *sapi_apache_read_cookies(TSRMLS_D);
static int sapi_apache_header_handler(sapi_header_struct *sapi_header, sapi_header_op_enum op, sapi_headers_struct *sapi_headers TSRMLS_DC);
static int sapi_apache_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC);
static int send_php(request_rec *r, int display_source_mode, char *filename);
static int send_parsed_php(request_rec * r);
static int send_parsed_php_source(request_rec * r);
static int php_xbithack_handler(request_rec * r);
static void php_init_handler(server_rec *s, pool *p);
/* }}} */

#if MODULE_MAGIC_NUMBER >= 19970728
static void php_child_exit_handler(server_rec *s, pool *p);
#endif

#if MODULE_MAGIC_NUMBER > 19961007
#define CONST_PREFIX const
#else
#define CONST_PREFIX
#endif
static CONST_PREFIX char *php_apache_value_handler_ex(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2, int mode);
static CONST_PREFIX char *php_apache_value_handler(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2);
static CONST_PREFIX char *php_apache_admin_value_handler(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2);
static CONST_PREFIX char *php_apache_flag_handler(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2);
static CONST_PREFIX char *php_apache_flag_handler_ex(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2, int mode);
static CONST_PREFIX char *php_apache_admin_flag_handler(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2);

/* ### these should be defined in mod_php5.h or somewhere else */
#define USE_PATH 1
#define IGNORE_URL 2
#define MAX_STATUS_LENGTH sizeof("xxxx LONGEST POSSIBLE STATUS DESCRIPTION")

module MODULE_VAR_EXPORT php5_module;

int saved_umask;
static unsigned char apache_php_initialized;

typedef struct _php_per_dir_entry {
	char *key;
	char *value;
	uint key_length;
	uint value_length;
	int type;
	char htaccess;
} php_per_dir_entry;

/* some systems are missing these from their header files */

/* {{{ php_save_umask
 */
static void php_save_umask(void)
{
	saved_umask = umask(077);
	umask(saved_umask);
}
/* }}} */

/* {{{ sapi_apache_ub_write
 */
static int sapi_apache_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	int ret=0;
		
	if (SG(server_context)) {
		ret = rwrite(str, str_length, (request_rec *) SG(server_context));
	}
	if (ret != str_length) {
		php_handle_aborted_connection();
	}
	return ret;
}
/* }}} */

/* {{{ sapi_apache_flush
 */
static void sapi_apache_flush(void *server_context)
{
	if (server_context) {
#if MODULE_MAGIC_NUMBER > 19970110
		rflush((request_rec *) server_context);
#else
		bflush((request_rec *) server_context->connection->client);
#endif
	}
}
/* }}} */

/* {{{ sapi_apache_read_post
 */
static int sapi_apache_read_post(char *buffer, uint count_bytes TSRMLS_DC)
{
	int total_read_bytes=0, read_bytes;
	request_rec *r = (request_rec *) SG(server_context);
	void (*handler)(int);

	/*
	 * This handles the situation where the browser sends a Expect: 100-continue header
	 * and needs to recieve confirmation from the server on whether or not it can send
	 * the rest of the request. RFC 2616
	 *
	 */
	if (!SG(read_post_bytes) && !ap_should_client_block(r)) {
		return total_read_bytes;
	}
 
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
/* }}} */

/* {{{ sapi_apache_read_cookies
 */
static char *sapi_apache_read_cookies(TSRMLS_D)
{
	return (char *) table_get(((request_rec *) SG(server_context))->subprocess_env, "HTTP_COOKIE");
}
/* }}} */

/* {{{ sapi_apache_header_handler
 */
static int sapi_apache_header_handler(sapi_header_struct *sapi_header, sapi_header_op_enum op, sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	char *header_name, *header_content, *p;
	request_rec *r = (request_rec *) SG(server_context);
	if(!r) {
		return 0;
	}

	switch(op) {
		case SAPI_HEADER_DELETE_ALL:
			clear_table(r->headers_out);
			return 0;

		case SAPI_HEADER_DELETE:
			table_unset(r->headers_out, sapi_header->header);
			return 0;

		case SAPI_HEADER_ADD:
		case SAPI_HEADER_REPLACE:
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
                       } else if (!strcasecmp(header_name, "Content-Length")) {
                               ap_set_content_length(r, strtol(header_content, (char **)NULL, 10));
			} else if (!strcasecmp(header_name, "Set-Cookie")) {
				table_add(r->headers_out, header_name, header_content);
			} else if (op == SAPI_HEADER_REPLACE) {
				table_set(r->headers_out, header_name, header_content);
			} else {
				table_add(r->headers_out, header_name, header_content);
			}

			*p = ':';  /* a well behaved header handler shouldn't change its original arguments */

			return SAPI_HEADER_ADD;

		default:
			return 0;
	}
}
/* }}} */

/* {{{ sapi_apache_send_headers
 */
static int sapi_apache_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	request_rec *r = SG(server_context);
	const char *sline = SG(sapi_headers).http_status_line;
	int sline_len;

	if(r == NULL) { /* server_context is not here anymore */
		return SAPI_HEADER_SEND_FAILED;
	}

	r->status = SG(sapi_headers).http_response_code;

	/* httpd requires that r->status_line is set to the first digit of
	 * the status-code: */
	if (sline && ((sline_len = strlen(sline)) > 12) && strncmp(sline, "HTTP/1.", 7) == 0 && sline[8] == ' ' && sline[12] == ' ') {
		if ((sline_len - 9) > MAX_STATUS_LENGTH) {
			r->status_line = ap_pstrndup(r->pool, sline + 9, MAX_STATUS_LENGTH);
		} else {
			r->status_line = ap_pstrndup(r->pool, sline + 9, sline_len - 9);
		}
	}

	if(r->status==304) {
		send_error_response(r,0);
	} else {
		send_http_header(r);
	}
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}
/* }}} */

/* {{{ sapi_apache_register_server_variables
 */
static void sapi_apache_register_server_variables(zval *track_vars_array TSRMLS_DC)
{
	register int i;
	array_header *arr = table_elts(((request_rec *) SG(server_context))->subprocess_env);
	table_entry *elts = (table_entry *) arr->elts;
	zval **path_translated;
	HashTable *symbol_table;
	unsigned int new_val_len;

	for (i = 0; i < arr->nelts; i++) {
		char *val;
		int val_len;

		if (elts[i].val) {
			val = elts[i].val;
		} else {
			val = "";
		}
		val_len = strlen(val);
		if (sapi_module.input_filter(PARSE_SERVER, elts[i].key, &val, val_len, &new_val_len TSRMLS_CC)) {
			php_register_variable_safe(elts[i].key, val, new_val_len, track_vars_array TSRMLS_CC);
		}
	}

	/* If PATH_TRANSLATED doesn't exist, copy it from SCRIPT_FILENAME */
	if (track_vars_array) {
		symbol_table = track_vars_array->value.ht;
	} else {
		symbol_table = NULL;
	}
	if (symbol_table
		&& !zend_hash_exists(symbol_table, "PATH_TRANSLATED", sizeof("PATH_TRANSLATED"))
		&& zend_hash_find(symbol_table, "SCRIPT_FILENAME", sizeof("SCRIPT_FILENAME"), (void **) &path_translated)==SUCCESS) {
		php_register_variable("PATH_TRANSLATED", Z_STRVAL_PP(path_translated), track_vars_array TSRMLS_CC);
	}

	if (sapi_module.input_filter(PARSE_SERVER, "PHP_SELF", &((request_rec *) SG(server_context))->uri, strlen(((request_rec *) SG(server_context))->uri), &new_val_len TSRMLS_CC)) {
		php_register_variable("PHP_SELF", ((request_rec *) SG(server_context))->uri, track_vars_array TSRMLS_CC);
	}
}
/* }}} */

/* {{{ php_apache_startup
 */
static int php_apache_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, &apache_module_entry, 1) == FAILURE) {
		return FAILURE;
	} else {
		return SUCCESS;
	}
}
/* }}} */

/* {{{ php_apache_log_message
 */
static void php_apache_log_message(char *message TSRMLS_DC)
{
	if (SG(server_context)) {
#if MODULE_MAGIC_NUMBER >= 19970831
		aplog_error(NULL, 0, APLOG_ERR | APLOG_NOERRNO, ((request_rec *) SG(server_context))->server, "%s", message);
#else
		log_error(message, ((request_rec *) SG(server_context))->server);
#endif
	} else {
		fprintf(stderr, "%s\n", message);
	}
}
/* }}} */

/* {{{ php_apache_request_shutdown
 */
static void php_apache_request_shutdown(void *dummy)
{
	TSRMLS_FETCH();

	php_output_set_status(PHP_OUTPUT_DISABLED TSRMLS_CC);
	if (AP(in_request)) {
		AP(in_request) = 0;
		php_request_shutdown(dummy);
	}
	SG(server_context) = NULL; 
	/* 
	* The server context (request) is NOT invalid by the time 
	* run_cleanups() is called 
	*/
}
/* }}} */

/* {{{ php_apache_sapi_activate
 */
static int php_apache_sapi_activate(TSRMLS_D)
{
	request_rec *r = (request_rec *) SG(server_context); 

	/*
	 * For the Apache module version, this bit of code registers a cleanup
	 * function that gets triggered when our request pool is destroyed.
	 * We need this because at any point in our code we can be interrupted
	 * and that may happen before we have had time to free our memory.
	 * The php_request_shutdown function needs to free all outstanding allocated
	 * memory.  
	 */
	block_alarms();
	register_cleanup(r->pool, NULL, php_apache_request_shutdown, php_request_shutdown_for_exec);
	AP(in_request)=1;
	unblock_alarms();

	/* Override the default headers_only value - sometimes "GET" requests should actually only
	 * send headers.
	 */
	SG(request_info).headers_only = r->header_only;
	return SUCCESS;
}
/* }}} */

/* {{{ php_apache_get_stat
 */
static struct stat *php_apache_get_stat(TSRMLS_D)
{
	return &((request_rec *) SG(server_context))->finfo;
}
/* }}} */

/* {{{ php_apache_getenv
 */
static char *php_apache_getenv(char *name, size_t name_len TSRMLS_DC)
{
	if (SG(server_context) == NULL) {
		return NULL;
	}

	return (char *) table_get(((request_rec *) SG(server_context))->subprocess_env, name);
}
/* }}} */

/* {{{ sapi_apache_get_fd
 */
static int sapi_apache_get_fd(int *nfd TSRMLS_DC)
{
#if PHP_APACHE_HAVE_CLIENT_FD
	request_rec *r = SG(server_context);
	int fd;

	fd = r->connection->client->fd;
	
	if (fd >= 0) {
		if (nfd) *nfd = fd;
		return SUCCESS;
	}
#endif
	return FAILURE;
}
/* }}} */

/* {{{ sapi_apache_force_http_10
 */
static int sapi_apache_force_http_10(TSRMLS_D)
{
	request_rec *r = SG(server_context);
	
	r->proto_num = HTTP_VERSION(1,0);
	
	return SUCCESS;
}
/* }}} */

/* {{{ sapi_apache_get_target_uid
 */
static int sapi_apache_get_target_uid(uid_t *obj TSRMLS_DC)
{
	*obj = ap_user_id;
	return SUCCESS;
}
/* }}} */

/* {{{ sapi_apache_get_target_gid
 */
static int sapi_apache_get_target_gid(gid_t *obj TSRMLS_DC)
{
	*obj = ap_group_id;
	return SUCCESS;
}
/* }}} */

/* {{{ php_apache_get_request_time
 */
static double php_apache_get_request_time(TSRMLS_D)
{
	return (double) ((request_rec *)SG(server_context))->request_time;
}
/* }}} */

/* {{{ sapi_apache_child_terminate
 */
static void sapi_apache_child_terminate(TSRMLS_D)
{
#ifndef MULTITHREAD
	ap_child_terminate((request_rec *)SG(server_context));
#endif
}
/* }}} */

/* {{{ sapi_module_struct apache_sapi_module
 */
static sapi_module_struct apache_sapi_module = {
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
	php_apache_get_request_time,	/* Get request time */
	sapi_apache_child_terminate,

	NULL,							/* php.ini path override */

#ifdef PHP_WIN32
	NULL,
	NULL,
#else
	block_alarms,					/* Block interruptions */
	unblock_alarms,					/* Unblock interruptions */
#endif

	NULL,							/* default post reader */
	NULL,							/* treat data */
	NULL,							/* exe location */
	0,								/* ini ignore */
	0,								/* ini ignore cwd */
	sapi_apache_get_fd,
	sapi_apache_force_http_10,
	sapi_apache_get_target_uid,
	sapi_apache_get_target_gid
};
/* }}} */

/* {{{ php_restore_umask
 */
static void php_restore_umask(void)
{
	umask(saved_umask);
}
/* }}} */

/* {{{ init_request_info
 */
static void init_request_info(TSRMLS_D)
{
	request_rec *r = ((request_rec *) SG(server_context));
	char *content_length = (char *) table_get(r->subprocess_env, "CONTENT_LENGTH");
	const char *authorization=NULL;
	char *tmp, *tmp_user;

	SG(request_info).query_string = r->args;
	SG(request_info).path_translated = r->filename;
	SG(request_info).request_uri = r->uri;
	SG(request_info).request_method = (char *)r->method;
	SG(request_info).content_type = (char *) table_get(r->subprocess_env, "CONTENT_TYPE");
	SG(request_info).content_length = (content_length ? atol(content_length) : 0);
	SG(sapi_headers).http_response_code = r->status;
	SG(request_info).proto_num = r->proto_num;

	if (r->headers_in) {
		authorization = table_get(r->headers_in, "Authorization");
	}

	SG(request_info).auth_user = NULL;
	SG(request_info).auth_password = NULL;
	SG(request_info).auth_digest = NULL;

	if (authorization) {
		char *p = getword(r->pool, &authorization, ' ');
		if (!strcasecmp(p, "Basic")) {
			tmp = uudecode(r->pool, authorization);
			tmp_user = getword_nulls_nc(r->pool, &tmp, ':');
			if (tmp_user) {
				r->connection->user = pstrdup(r->connection->pool, tmp_user);
				r->connection->ap_auth_type = "Basic";
				SG(request_info).auth_user = estrdup(tmp_user);
			}
			if (tmp) {
				SG(request_info).auth_password = estrdup(tmp);
			}
		} else if (!strcasecmp(p, "Digest")) {
			r->connection->ap_auth_type = "Digest";
			SG(request_info).auth_digest = estrdup(authorization);
		}
	}
}
/* }}} */

/* {{{ php_apache_alter_ini_entries
 */
static int php_apache_alter_ini_entries(php_per_dir_entry *per_dir_entry TSRMLS_DC)
{
	zend_alter_ini_entry(per_dir_entry->key, per_dir_entry->key_length+1, per_dir_entry->value, per_dir_entry->value_length, per_dir_entry->type, per_dir_entry->htaccess?PHP_INI_STAGE_HTACCESS:PHP_INI_STAGE_ACTIVATE);
	return 0;
}
/* }}} */

/* {{{ php_apache_get_default_mimetype
 */
static char *php_apache_get_default_mimetype(request_rec *r TSRMLS_DC)
{
	
	char *mimetype;
	if (SG(default_mimetype) || SG(default_charset)) {
		/* Assume output will be of the default MIME type.  Individual
		   scripts may change this later. */
		char *tmpmimetype;
		tmpmimetype = sapi_get_default_content_type(TSRMLS_C);
		mimetype = pstrdup(r->pool, tmpmimetype);
		efree(tmpmimetype);
	} else {
		mimetype = SAPI_DEFAULT_MIMETYPE "; charset=" SAPI_DEFAULT_CHARSET;
	}
	return mimetype;
}
/* }}} */

/* {{{ send_php
 */
static int send_php(request_rec *r, int display_source_mode, char *filename)
{
	int retval;
	HashTable *per_dir_conf;
	TSRMLS_FETCH();

	if (AP(in_request)) {
		zend_file_handle fh;

		fh.filename = r->filename;
		fh.opened_path = NULL;
		fh.free_filename = 0;
		fh.type = ZEND_HANDLE_FILENAME;

		zend_execute_scripts(ZEND_INCLUDE TSRMLS_CC, NULL, 1, &fh);
		return OK;
	}

	SG(server_context) = r;

	zend_first_try {

		/* Make sure file exists */
		if (filename == NULL && r->finfo.st_mode == 0) {
			return DECLINED;
		}

		per_dir_conf = (HashTable *) get_module_config(r->per_dir_config, &php5_module);
		if (per_dir_conf) {
			zend_hash_apply((HashTable *) per_dir_conf, (apply_func_t) php_apache_alter_ini_entries TSRMLS_CC);
		}
		
		/* If PHP parser engine has been turned off with an "engine off"
		 * directive, then decline to handle this request
		 */
		if (!AP(engine)) {
			r->content_type = php_apache_get_default_mimetype(r TSRMLS_CC);
			zend_try {
				zend_ini_deactivate(TSRMLS_C);
			} zend_end_try();
			return DECLINED;
		}
		if (filename == NULL) {
			filename = r->filename;
		}

		/* Apache 1.2 has a more complex mechanism for reading POST data */
#if MODULE_MAGIC_NUMBER > 19961007
		if ((retval = setup_client_block(r, REQUEST_CHUNKED_DECHUNK))) {
			zend_try {
				zend_ini_deactivate(TSRMLS_C);
			} zend_end_try();
			return retval;
		}
#endif

		if (AP(last_modified)) {
#if MODULE_MAGIC_NUMBER < 19970912
			if ((retval = set_last_modified(r, r->finfo.st_mtime))) {
				zend_try {
					zend_ini_deactivate(TSRMLS_C);
				} zend_end_try();
				return retval;
			}
#else
			update_mtime (r, r->finfo.st_mtime);
			set_last_modified(r);
			set_etag(r);
#endif
		}
		/* Assume output will be of the default MIME type.  Individual
		   scripts may change this later in the request. */
		r->content_type = php_apache_get_default_mimetype(r TSRMLS_CC);

		/* Init timeout */
		hard_timeout("send", r);

		php_save_umask();
		add_common_vars(r);
		add_cgi_vars(r);

		init_request_info(TSRMLS_C);
		apache_php_module_main(r, display_source_mode TSRMLS_CC);

		/* Done, restore umask, turn off timeout, close file and return */
		php_restore_umask();
		kill_timeout(r);
	} zend_end_try();

	return OK;
}
/* }}} */

/* {{{ send_parsed_php
 */
static int send_parsed_php(request_rec * r)
{
	int result = send_php(r, 0, NULL);
	TSRMLS_FETCH();
 
	ap_table_setn(r->notes, "mod_php_memory_usage",
		ap_psprintf(r->pool, "%lu", zend_memory_peak_usage(1 TSRMLS_CC)));

	return result;
}
/* }}} */

/* {{{ send_parsed_php_source
 */
static int send_parsed_php_source(request_rec * r)
{
	return send_php(r, 1, NULL);
}
/* }}} */

/* {{{ destroy_per_dir_entry
 */
static void destroy_per_dir_entry(php_per_dir_entry *per_dir_entry)
{
	free(per_dir_entry->key);
	free(per_dir_entry->value);
}
/* }}} */

/* {{{ copy_per_dir_entry
 */
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
/* }}} */

/* {{{ should_overwrite_per_dir_entry
 */
static zend_bool should_overwrite_per_dir_entry(HashTable *target_ht, php_per_dir_entry *new_per_dir_entry, zend_hash_key *hash_key, void *pData)
{
	php_per_dir_entry *orig_per_dir_entry;

	if (zend_hash_find(target_ht, hash_key->arKey, hash_key->nKeyLength, (void **) &orig_per_dir_entry)==FAILURE) {
		return 1; /* does not exist in dest, copy from source */
	}

	if (orig_per_dir_entry->type==PHP_INI_SYSTEM
		&& new_per_dir_entry->type!=PHP_INI_SYSTEM) {
		return 0;
	} else {
		return 1;
	}
}
/* }}} */

/* {{{ php_destroy_per_dir_info
 */
static void php_destroy_per_dir_info(HashTable *per_dir_info)
{
	zend_hash_destroy(per_dir_info);
	free(per_dir_info);
}
/* }}} */

/* {{{ php_create_dir
 */
static void *php_create_dir(pool *p, char *dummy)
{
	HashTable *per_dir_info;

	per_dir_info = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init_ex(per_dir_info, 5, NULL, (void (*)(void *)) destroy_per_dir_entry, 1, 0);
	register_cleanup(p, (void *) per_dir_info, (void (*)(void *)) php_destroy_per_dir_info, (void (*)(void *)) zend_hash_destroy);

	return per_dir_info;
}
/* }}} */

/* {{{ php_merge_dir
 */
static void *php_merge_dir(pool *p, void *basev, void *addv)
{
	/* This function *must* not modify addv or basev */
	HashTable *new;

	/* need a copy of addv to merge */
	new = php_create_dir(p, "php_merge_dir");
	zend_hash_copy(new, (HashTable *) basev, (copy_ctor_func_t) copy_per_dir_entry, NULL, sizeof(php_per_dir_entry));

	zend_hash_merge_ex(new, (HashTable *) addv, (copy_ctor_func_t) copy_per_dir_entry, sizeof(php_per_dir_entry), (merge_checker_func_t) should_overwrite_per_dir_entry, NULL);
	return new;
}
/* }}} */

/* {{{ php_apache_value_handler_ex
 */
static CONST_PREFIX char *php_apache_value_handler_ex(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2, int mode)
{
	php_per_dir_entry per_dir_entry;

	if (!apache_php_initialized) {
		apache_php_initialized = 1;
#ifdef ZTS
		tsrm_startup(1, 1, 0, NULL);
#endif
		sapi_startup(&apache_sapi_module);
		php_apache_startup(&apache_sapi_module);
	}
	per_dir_entry.type = mode;
	per_dir_entry.htaccess = ((cmd->override & (RSRC_CONF|ACCESS_CONF)) == 0);

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

	zend_hash_update(conf, per_dir_entry.key, per_dir_entry.key_length, &per_dir_entry, sizeof(php_per_dir_entry), NULL);
	return NULL;
}
/* }}} */

/* {{{ php_apache_value_handler
 */
static CONST_PREFIX char *php_apache_value_handler(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2)
{
	return php_apache_value_handler_ex(cmd, conf, arg1, arg2, PHP_INI_PERDIR);
}
/* }}} */

/* {{{ php_apache_admin_value_handler
 */
static CONST_PREFIX char *php_apache_admin_value_handler(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2)
{
	return php_apache_value_handler_ex(cmd, conf, arg1, arg2, PHP_INI_SYSTEM);
}
/* }}} */

/* {{{ php_apache_flag_handler_ex
 */
static CONST_PREFIX char *php_apache_flag_handler_ex(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2, int mode)
{
	char bool_val[2];

	if (!strcasecmp(arg2, "On") || (arg2[0] == '1' && arg2[1] == '\0')) {
		bool_val[0] = '1';
	} else {
		bool_val[0] = '0';
	}
	bool_val[1] = 0;
	
	return php_apache_value_handler_ex(cmd, conf, arg1, bool_val, mode);
}
/* }}} */

/* {{{ php_apache_flag_handler
 */
static CONST_PREFIX char *php_apache_flag_handler(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2)
{
	return php_apache_flag_handler_ex(cmd, conf, arg1, arg2, PHP_INI_PERDIR);
}
/* }}} */

/* {{{ php_apache_admin_flag_handler
 */
static CONST_PREFIX char *php_apache_admin_flag_handler(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2)
{
	return php_apache_flag_handler_ex(cmd, conf, arg1, arg2, PHP_INI_SYSTEM);
}
/* }}} */

/* {{{ php_apache_phpini_set
 */
static CONST_PREFIX char *php_apache_phpini_set(cmd_parms *cmd, HashTable *conf, char *arg)
{
	if (apache_sapi_module.php_ini_path_override) {
		return "Only first PHPINIDir directive honored per configuration tree - subsequent ones ignored";
	}
	apache_sapi_module.php_ini_path_override = ap_server_root_relative(cmd->pool, arg);
	return NULL;
}
/* }}} */

/* {{{ int php_xbithack_handler(request_rec * r)
 */
static int php_xbithack_handler(request_rec * r)
{
	HashTable *per_dir_conf;
	TSRMLS_FETCH();

	if (!(r->finfo.st_mode & S_IXUSR)) {
		return DECLINED;
	}
	per_dir_conf = (HashTable *) get_module_config(r->per_dir_config, &php5_module);
	if (per_dir_conf) {
		zend_hash_apply((HashTable *) per_dir_conf, (apply_func_t) php_apache_alter_ini_entries TSRMLS_CC);
	}
	if(!AP(xbithack)) {
		zend_try {
			zend_ini_deactivate(TSRMLS_C);
		} zend_end_try();
		return DECLINED;
	}
	return send_parsed_php(r);
}
/* }}} */

/* {{{ apache_php_module_shutdown_wrapper
 */
static void apache_php_module_shutdown_wrapper(void)
{
	apache_php_initialized = 0;
	apache_sapi_module.shutdown(&apache_sapi_module);

#if MODULE_MAGIC_NUMBER >= 19970728
	/* This function is only called on server exit if the apache API
	 * child_exit handler exists, so shutdown globally 
	 */
	sapi_shutdown();
#endif

#ifdef ZTS
	tsrm_shutdown();
#endif
}
/* }}} */

#if MODULE_MAGIC_NUMBER >= 19970728
/* {{{ php_child_exit_handler
 */
static void php_child_exit_handler(server_rec *s, pool *p)
{
/*	apache_php_initialized = 0; */
	apache_sapi_module.shutdown(&apache_sapi_module);

#ifdef ZTS
	tsrm_shutdown();
#endif
}
/* }}} */
#endif

/* {{{ void php_init_handler(server_rec *s, pool *p)
 */
static void php_init_handler(server_rec *s, pool *p)
{
	register_cleanup(p, NULL, (void (*)(void *))apache_php_module_shutdown_wrapper, (void (*)(void *))php_module_shutdown_for_exec);
	if (!apache_php_initialized) {
		apache_php_initialized = 1;
#ifdef ZTS
		tsrm_startup(1, 1, 0, NULL);
#endif
		sapi_startup(&apache_sapi_module);
		php_apache_startup(&apache_sapi_module);
	}
#if MODULE_MAGIC_NUMBER >= 19980527
	{
		TSRMLS_FETCH();
		if (PG(expose_php)) {
			ap_add_version_component("PHP/" PHP_VERSION);
		}
	}
#endif
}
/* }}} */

/* {{{ handler_rec php_handlers[]
 */
handler_rec php_handlers[] =
{
	{"application/x-httpd-php", send_parsed_php},
	{"application/x-httpd-php-source", send_parsed_php_source},
	{"text/html", php_xbithack_handler},
	{NULL}
};
/* }}} */

/* {{{ command_rec php_commands[]
 */
command_rec php_commands[] =
{
	{"php_value",		php_apache_value_handler, NULL, OR_OPTIONS, TAKE2, "PHP Value Modifier"},
	{"php_flag",		php_apache_flag_handler, NULL, OR_OPTIONS, TAKE2, "PHP Flag Modifier"},
	{"php_admin_value",	php_apache_admin_value_handler, NULL, ACCESS_CONF|RSRC_CONF, TAKE2, "PHP Value Modifier (Admin)"},
	{"php_admin_flag",	php_apache_admin_flag_handler, NULL, ACCESS_CONF|RSRC_CONF, TAKE2, "PHP Flag Modifier (Admin)"},
	{"PHPINIDir",		php_apache_phpini_set, NULL, RSRC_CONF, TAKE1, "Directory containing the php.ini file"},
	{NULL}
};
/* }}} */

/* {{{ odule MODULE_VAR_EXPORT php5_module
 */
module MODULE_VAR_EXPORT php5_module =
{
	STANDARD_MODULE_STUFF,
	php_init_handler,			/* initializer */
	php_create_dir,				/* per-directory config creator */
	php_merge_dir,				/* dir merger */
	NULL,						/* per-server config creator */
	NULL, 						/* merge server config */
	php_commands,				/* command table */
	php_handlers,				/* handlers */
	NULL,						/* filename translation */
	NULL,						/* check_user_id */
	NULL,						/* check auth */
	NULL,						/* check access */
	NULL,						/* type_checker */
	NULL,						/* fixups */
	NULL						/* logger */
#if MODULE_MAGIC_NUMBER >= 19970103
	, NULL						/* header parser */
#endif
#if MODULE_MAGIC_NUMBER >= 19970719
	, NULL						/* child_init */
#endif
#if MODULE_MAGIC_NUMBER >= 19970728
	, php_child_exit_handler		/* child_exit */
#endif
#if MODULE_MAGIC_NUMBER >= 19970902
	, NULL						/* post read-request */
#endif
};
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
