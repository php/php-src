/*
   +----------------------------------------------------------------------+
   | PHP Version 4														|
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group								|
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,	  |
   | that is bundled with this package in the file LICENSE, and is		|
   | available at through the world-wide-web at						   |
   | http://www.php.net/license/2_02.txt.								 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to		  |
   | license@php.net so we can mail you a copy immediately.			   |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>							 |
   | (with helpful hints from Dean Gaudet <dgaudet@arctic.org>			|
   | PHP 4.0 patches by Zeev Suraski <zeev@zend.com>					  |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php_apache_http.h"

#ifdef NETWARE
#define SIGPIPE SIGINT
#endif

#if defined(ZEND_MULTIBYTE) && defined(HAVE_MBSTRING)
#include "ext/mbstring/mbstring.h"
#endif /* defined(ZEND_MULTIBYTE) && defined(HAVE_MBSTRING) */

#undef shutdown

/* {{{ Prototypes
 */
int apache_php_module_main(request_rec *r, int display_source_mode TSRMLS_DC);
static void php_save_umask(void);
static void php_restore_umask(void);
static int sapi_apache_read_post(char *buffer, uint count_bytes TSRMLS_DC);
static char *sapi_apache_read_cookies(TSRMLS_D);
static int sapi_apache_header_handler(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers TSRMLS_DC);
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


typedef struct _sapi_stack {
		int top, max, persistent;
		void **elements;
} sapi_stack;

typedef struct _php_per_dir_config {
	HashTable *ini_settings;
	sapi_stack headers_handlers;
	sapi_stack auth_handlers;
	sapi_stack access_handlers;
	sapi_stack type_handlers;
	sapi_stack fixup_handlers;
	sapi_stack logger_handlers;
	sapi_stack post_read_handlers;
	sapi_stack response_handlers;
} php_per_dir_config;

typedef struct _php_per_server_config {
	sapi_stack uri_handlers;
	sapi_stack requires;
} php_per_server_config;


static CONST_PREFIX char *php_apache_value_handler_ex(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2, int mode);
static CONST_PREFIX char *php_apache_value_handler(cmd_parms *cmd, php_per_dir_config *conf, char *arg1, char *arg2);
static CONST_PREFIX char *php_apache_admin_value_handler(cmd_parms *cmd, php_per_dir_config *conf, char *arg1, char *arg2);
static CONST_PREFIX char *php_apache_flag_handler(cmd_parms *cmd, php_per_dir_config *conf, char *arg1, char *arg2);
static CONST_PREFIX char *php_apache_flag_handler_ex(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2, int mode);
static CONST_PREFIX char *php_apache_admin_flag_handler(cmd_parms *cmd, php_per_dir_config *conf, char *arg1, char *arg2);

/* ### these should be defined in mod_php4.h or somewhere else */
#define USE_PATH 1
#define IGNORE_URL 2

module MODULE_VAR_EXPORT php4_module;

int saved_umask;
//static int setup_env = 0;
static unsigned char apache_php_initialized;

typedef struct _php_per_dir_entry {
	char *key;
	char *value;
	uint key_length;
	uint value_length;
	int type;
} php_per_dir_entry;

/* some systems are missing these from their header files */

/* {{{ zend stack utility functions
 */

/* This code is ripped part and parcel from zend_stack.[ch].  Assuming that the
   patch supporting zend_stack_init_ex is applied, all but the bottom two 
   module-specific iterators will be removed
 */

int sapi_stack_init_ex(sapi_stack *stack, int persistent)
{
		stack->top = 0;
		stack->persistent = persistent;
		stack->elements = (void **) pemalloc(sizeof(void **) * STACK_BLOCK_SIZE,  persistent);
		if (!stack->elements) {
				return FAILURE;
		} else {
				stack->max = STACK_BLOCK_SIZE;
				return SUCCESS;
		}
}
int sapi_stack_push(sapi_stack *stack, void *element)
{
		if (stack->top >= stack->max) {		 /* we need to allocate more memory */
				stack->elements = (void **) perealloc(stack->elements,
								   (sizeof(void **) * (stack->max += STACK_BLOCK_SIZE)), stack->persistent);
				if (!stack->elements) {
						return FAILURE;
				}
		}
		stack->elements[stack->top] = (void *) element;
		return stack->top++;
}
void* sapi_stack_pop(sapi_stack *stack) {
	if(stack->top == 0) {
		return NULL;
	}
	else {
		return stack->elements[--stack->top];
	}
}

int sapi_stack_destroy(sapi_stack *stack)
{
		return SUCCESS;
}

int sapi_stack_apply_with_argument_all(sapi_stack *stack, int type, int (*apply_function)(void *element, void *arg), void *arg)
{
		int i, retval;   

		switch (type) {				
				case ZEND_STACK_APPLY_TOPDOWN:
						for (i=stack->top-1; i>=0; i--) {
								retval = apply_function(stack->elements[i], arg); 
						}
						break;
				case ZEND_STACK_APPLY_BOTTOMUP:
						for (i=0; i<stack->top; i++) {		
								retval = apply_function(stack->elements[i], arg);
						}	  
						break;
		}
		return retval;
}


int sapi_stack_apply_with_argument_stop_if_equals(sapi_stack *stack, int type, int (*apply_function)(void *element, void *arg), void *arg, int stopval)
{
	int i;
	int ret = DECLINED;
	switch (type) {
		case ZEND_STACK_APPLY_TOPDOWN:
			for (i=stack->top-1; i>=0; i--) {
				if ((ret = apply_function(stack->elements[i], arg)) == stopval) {
					break;
				}
			}
			break;
		case ZEND_STACK_APPLY_BOTTOMUP:
			for (i=0; i<stack->top; i++) {
				if ((ret = apply_function(stack->elements[i], arg)) == stopval) {
					break;
				}
			}
			break;
	}
	return ret;
}

int sapi_stack_apply_with_argument_stop_if_http_error(sapi_stack *stack, int type, int (*apply_function)(void *element, void *arg), void *arg)
{
	int i;
	int ret = DECLINED;

	switch (type) {
		case ZEND_STACK_APPLY_TOPDOWN:
			for (i=stack->top-1; i>=0; i--) {
				if ((ret = apply_function(stack->elements[i], arg)) > 0) {
					break;
				}
			}
			break;
		case ZEND_STACK_APPLY_BOTTOMUP:
			for (i=0; i<stack->top; i++) {
				if ((ret = apply_function(stack->elements[i], arg)) > 0) {
					break;
				}
			}
			break;
	}
	return ret;
}

void php_handler_stack_destroy(sapi_stack *stack)
{
	php_handler *ph;
	while((ph = (php_handler *)sapi_stack_pop(stack)) != NULL) {
		free(ph->name);
		free(ph);
	}
}
/* }}} */ 

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
	uint total_read_bytes=0, read_bytes;
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
static int sapi_apache_header_handler(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	char *header_name, *header_content, *p;
	request_rec *r = (request_rec *) SG(server_context);

	header_name = sapi_header->header;

	header_content = p = strchr(header_name, ':');
	if (!p) {
		efree(sapi_header->header);
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
/* }}} */

/* {{{ sapi_apache_send_headers
 */
static int sapi_apache_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	if(SG(server_context) == NULL) { /* server_context is not here anymore */
		return SAPI_HEADER_SEND_FAILED;
	}

	((request_rec *) SG(server_context))->status = SG(sapi_headers).http_response_code;
	/* check that we haven't sent headers already, we use our own
	 * headers_sent since we may send headers at anytime 
	 */
	if(!AP(headers_sent)) {
		send_http_header((request_rec *) SG(server_context));
		AP(headers_sent) = 1;
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

	for (i = 0; i < arr->nelts; i++) {
		char *val;

		if (elts[i].val) {
			val = elts[i].val;
		} else {
			val = empty_string;
		}
		php_register_variable(elts[i].key, val, track_vars_array  TSRMLS_CC);
	}

	/* If PATH_TRANSLATED doesn't exist, copy it from SCRIPT_FILENAME */
	if (track_vars_array) {
		symbol_table = track_vars_array->value.ht;
	} else if (PG(register_globals)) {
		/* should never happen nowadays */
		symbol_table = EG(active_symbol_table);
	} else {
		symbol_table = NULL;
	}
	if (symbol_table
		&& !zend_hash_exists(symbol_table, "PATH_TRANSLATED", sizeof("PATH_TRANSLATED"))
		&& zend_hash_find(symbol_table, "SCRIPT_FILENAME", sizeof("SCRIPT_FILENAME"), (void **) &path_translated)==SUCCESS) {
		php_register_variable("PATH_TRANSLATED", Z_STRVAL_PP(path_translated), track_vars_array TSRMLS_CC);
	}

	php_register_variable("PHP_SELF", ((request_rec *) SG(server_context))->uri, track_vars_array TSRMLS_CC);
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
static void php_apache_log_message(char *message)
{
	TSRMLS_FETCH();

	if (SG(server_context)) {
#if MODULE_MAGIC_NUMBER >= 19970831
		aplog_error(NULL, 0, APLOG_ERR | APLOG_NOERRNO, ((request_rec *) SG(server_context))->server, "%s", message);
#else
		log_error(message, ((request_rec *) SG(server_context))->server);
#endif
	} else {
		fprintf(stderr, "%s", message);
		fprintf(stderr, "\n");
	}
}
/* }}} */

/* {{{ php_apache_request_shutdown
 */
static void php_apache_request_shutdown(void *dummy)
{
	TSRMLS_FETCH();
	AP(current_hook) = AP_CLEANUP;
	php_output_set_status(0 TSRMLS_CC);
	SG(server_context) = NULL; /* The server context (request) is invalid by the time run_cleanups() is called */
	if(SG(sapi_started)) {
		php_request_shutdown(dummy);
		SG(sapi_started) = 0;
	}
	AP(in_request) = 0;
	if(AP(setup_env)) {
		AP(setup_env) = 0;
	}
	AP(current_hook) = AP_WAITING_FOR_REQUEST;
	AP(headers_sent) = 0;
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
	return (char *) table_get(((request_rec *) SG(server_context))->subprocess_env, name);
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

	NULL,							/* php.ini path override */

#ifdef PHP_WIN32
	NULL,
	NULL,
#else
	block_alarms,					/* Block interruptions */
	unblock_alarms,					/* Unblock interruptions */
#endif

	STANDARD_SAPI_MODULE_PROPERTIES
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
		&& !auth_type(r)
		&& !strcasecmp(getword(r->pool, &authorization, ' '), "Basic")) {
		tmp = uudecode(r->pool, authorization);
		SG(request_info).auth_user = getword_nulls_nc(r->pool, &tmp, ':');
		if (SG(request_info).auth_user) {
			r->connection->user = pstrdup(r->connection->pool, SG(request_info).auth_user);
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
/* }}} */

/* {{{ php_apache_alter_ini_entries
 */
static int php_apache_alter_ini_entries(php_per_dir_entry *per_dir_entry TSRMLS_DC)
{
	zend_alter_ini_entry(per_dir_entry->key, per_dir_entry->key_length+1, per_dir_entry->value, per_dir_entry->value_length, per_dir_entry->type, PHP_INI_STAGE_ACTIVATE);
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
	php_per_dir_config *per_dir_conf;
	TSRMLS_FETCH();
	if (AP(in_request)) {
		zend_file_handle fh;

		fh.filename = r->filename;
		fh.opened_path = NULL;
		fh.free_filename = 0;
		fh.type = ZEND_HANDLE_FILENAME;

#if defined(ZEND_MULTIBYTE) && defined(HAVE_MBSTRING)
		php_mbstring_set_zend_encoding(TSRMLS_C);
#endif /* defined(ZEND_MULTIBYTE) && defined(HAVE_MBSTRING) */

		zend_execute_scripts(ZEND_INCLUDE TSRMLS_CC, NULL, 1, &fh);
		return OK;
	}

	zend_first_try {

		/* Make sure file exists */
		if (filename == NULL && r->finfo.st_mode == 0) {
			return DECLINED;
		}

		per_dir_conf = (php_per_dir_config *) get_module_config(r->per_dir_config, &php4_module);
		if (per_dir_conf) {
			zend_hash_apply((HashTable *) per_dir_conf->ini_settings, (apply_func_t) php_apache_alter_ini_entries TSRMLS_CC);
		}

		/* If PHP parser engine has been turned off with an "engine off"
		 * directive, then decline to handle this request
		 */
		if (!AP(engine)) {
			r->content_type = php_apache_get_default_mimetype(r TSRMLS_CC);
			r->allowed |= (1 << METHODS) - 1;
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
		if ((retval = setup_client_block(r, REQUEST_CHUNKED_ERROR))) {
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

		SG(server_context) = r;
		
		php_save_umask();
		if(!AP(setup_env)) {
			add_common_vars(r);
			add_cgi_vars(r);
			AP(setup_env) = 1;
		}
		init_request_info(TSRMLS_C);
		fprintf(stderr, "%s:%d\n", __FILE__,__LINE__);
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

#if MEMORY_LIMIT
	{
		char *mem_usage;
		TSRMLS_FETCH();
 
		mem_usage = ap_psprintf(r->pool, "%u", AG(allocated_memory_peak));
		AG(allocated_memory_peak) = 0;
		ap_table_setn(r->notes, "mod_php_memory_usage", mem_usage);
	}
#endif

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
static zend_bool should_overwrite_per_dir_entry(php_per_dir_entry *orig_per_dir_entry, php_per_dir_entry *new_per_dir_entry)
{
	if (new_per_dir_entry->type==PHP_INI_SYSTEM
		&& orig_per_dir_entry->type!=PHP_INI_SYSTEM) {
		return 1;
	} else {
		return 0;
	}
}
/* }}} */
/* {{{ php_destroy_per_server_info
 */
static void php_destroy_per_server_info(php_per_server_config *conf)
{
	php_handler_stack_destroy(&conf->requires);
	php_handler_stack_destroy(&conf->uri_handlers);
}
/* }}} */

/* {{{ php_destroy_per_dir_info
 */
static void php_destroy_per_dir_info(php_per_dir_config *conf) 
{
	zend_hash_destroy(conf->ini_settings);
	php_handler_stack_destroy(&conf->response_handlers);
	php_handler_stack_destroy(&conf->auth_handlers);
	php_handler_stack_destroy(&conf->access_handlers);
	php_handler_stack_destroy(&conf->type_handlers);
	php_handler_stack_destroy(&conf->fixup_handlers);
	php_handler_stack_destroy(&conf->logger_handlers);
	php_handler_stack_destroy(&conf->post_read_handlers);
	php_handler_stack_destroy(&conf->headers_handlers);
	free(conf->ini_settings);
}
/* }}} */

/* {{{ php_create_server
 */
static void *php_create_server(pool *p, char *dummy)
{
	php_per_server_config *conf;
	conf = (php_per_server_config *) malloc(sizeof(php_per_server_config));
	register_cleanup(p, (void *) conf, (void (*)(void *)) php_destroy_per_server_info, (void (*)(void *)) php_destroy_per_server_info);
	
	sapi_stack_init_ex(&conf->requires, 1);
	sapi_stack_init_ex(&conf->uri_handlers, 1);
	return conf;
}
	
/* }}} */


/* {{{ php_create_dir
 */
static void *php_create_dir(pool *p, char *dummy)
{
	php_per_dir_config *conf;
	conf = (php_per_dir_config *) malloc(sizeof(php_per_dir_config));
	conf->ini_settings = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init_ex(conf->ini_settings, 5, NULL, (void (*)(void *)) destroy_per_dir_entry, 1, 0);
	sapi_stack_init_ex(&conf->response_handlers, 1);
	sapi_stack_init_ex(&conf->headers_handlers, 1);
	sapi_stack_init_ex(&conf->auth_handlers, 1);
	sapi_stack_init_ex(&conf->access_handlers, 1);
	sapi_stack_init_ex(&conf->type_handlers, 1);
	sapi_stack_init_ex(&conf->fixup_handlers, 1);
	sapi_stack_init_ex(&conf->logger_handlers, 1);
	sapi_stack_init_ex(&conf->post_read_handlers, 1);
	register_cleanup(p, (void *) conf, (void (*)(void *)) php_destroy_per_dir_info, (void (*)(void *)) php_destroy_per_dir_info);
	
	return conf;
}

/* }}} */

/* {{{ php_merge_dir
 */
static void *php_merge_dir(pool *p, void *basev, void *addv)
{
	php_per_dir_config *a = (php_per_dir_config *) addv;
	php_per_dir_config *b = (php_per_dir_config *) basev;
	/* This function *must* return addv, and not modify basev */
	zend_hash_merge_ex((HashTable *) a->ini_settings, (HashTable *) b->ini_settings, (copy_ctor_func_t) copy_per_dir_entry, sizeof(php_per_dir_entry), (zend_bool (*)(void *, void *)) should_overwrite_per_dir_entry);
	a->headers_handlers = (a->headers_handlers.top)?a->headers_handlers:b->headers_handlers;
	a->auth_handlers = (a->auth_handlers.top)?a->auth_handlers:b->auth_handlers;
	a->access_handlers = (a->access_handlers.top)?a->access_handlers:b->access_handlers;
	a->type_handlers = (a->type_handlers.top)?a->type_handlers:b->type_handlers;
	a->fixup_handlers = (a->fixup_handlers.top)?a->fixup_handlers:b->fixup_handlers;
	a->logger_handlers = (a->logger_handlers.top)?a->logger_handlers:b->logger_handlers;
	a->post_read_handlers = (a->post_read_handlers.top)?a->post_read_handlers:b->post_read_handlers;
	a->response_handlers = (a->response_handlers.top)?a->response_handlers:b->response_handlers;
	return a;
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

static CONST_PREFIX char *php_set_server_handler(server_rec *s, char *arg1, long handler_stage, long handler_type)
{
	php_per_server_config *conf;
	php_handler *handler;
	handler = (php_handler *) malloc(sizeof(php_handler));
	handler->type = handler_type;
	handler->stage = handler_stage;
	handler->name = strdup(arg1);
	conf = get_module_config(s->module_config, &php4_module);
	switch(handler_stage) {
		case AP_URI_TRANS:
			sapi_stack_push(&conf->uri_handlers, handler);
			break;
		default:
			sapi_stack_push(&conf->requires, handler);
			break;
	}
	return NULL;
}

static CONST_PREFIX char *php_set_dir_handler(php_per_dir_config *conf, char *arg1, long handler_stage, long handler_type)
{
	php_handler *handler;
	handler = (php_handler *) malloc(sizeof(php_handler));
	handler->type = handler_type;
	handler->stage = handler_stage;
	handler->name = strdup(arg1);
	switch(handler_stage) {
		case AP_POST_READ:
			sapi_stack_push(&conf->post_read_handlers, handler);
			break;
		case AP_HEADER_PARSE:
			sapi_stack_push(&conf->headers_handlers, handler);
			break;
		case AP_ACCESS_CONTROL:
			sapi_stack_push(&conf->access_handlers, handler);
			break;
		case AP_AUTHENTICATION:
			sapi_stack_push(&conf->auth_handlers, handler);
			break;
		case AP_AUTHORIZATION:
			break;
		case AP_TYPE_CHECKING:
			sapi_stack_push(&conf->type_handlers, handler);
			break;
		case AP_FIXUP:
			sapi_stack_push(&conf->fixup_handlers, handler);
			break;
		case AP_RESPONSE:
			sapi_stack_push(&conf->response_handlers, handler);
			break;
		case AP_LOGGING:
			sapi_stack_push(&conf->logger_handlers, handler);
			break;
		default:
			break;
	}
	return NULL;
}

/* {{{ php_set_uri_handler 
 */
static CONST_PREFIX char *php_set_uri_handler(cmd_parms *cmd, void *dummy, char *arg1)
{
	return php_set_server_handler(cmd->server, arg1, AP_URI_TRANS, AP_HANDLER_TYPE_FILE);
}
/* }}} */

/* {{{ php_set_uri_handler_code */
static CONST_PREFIX char *php_set_uri_handler_code(cmd_parms *cmd, void *dummy, char *arg1)
{
	return php_set_server_handler(cmd->server, arg1, AP_URI_TRANS, AP_HANDLER_TYPE_METHOD);
}
/* }}} */

/* {{{ php_set_header_handler
 */
static CONST_PREFIX char *php_set_header_handler(cmd_parms *cmd, php_per_dir_config *conf, char *arg1)
{
	return php_set_dir_handler(conf, arg1, AP_HEADER_PARSE, AP_HANDLER_TYPE_FILE);
}
static CONST_PREFIX char *php_set_header_handler_code(cmd_parms *cmd, php_per_dir_config *conf, char *arg1)
{
	return php_set_dir_handler(conf, arg1, AP_HEADER_PARSE, AP_HANDLER_TYPE_METHOD);
}
/* }}} */

/* {{{ php_set_auth_handler 
 */
static CONST_PREFIX char *php_set_auth_handler(cmd_parms *cmd, php_per_dir_config *conf, char *arg1)
{
	return php_set_dir_handler(conf, arg1, AP_AUTHENTICATION, AP_HANDLER_TYPE_FILE);
}
static CONST_PREFIX char *php_set_auth_handler_code(cmd_parms *cmd, php_per_dir_config *conf, char *arg1)
{
	return php_set_dir_handler(conf, arg1, AP_AUTHENTICATION, AP_HANDLER_TYPE_METHOD);
}

/* }}} */

/* {{{ php_set_access_handler
 */
static CONST_PREFIX char *php_set_access_handler(cmd_parms *cmd, php_per_dir_config *conf, char *arg1)
{
	return php_set_dir_handler(conf, arg1, AP_ACCESS_CONTROL, AP_HANDLER_TYPE_FILE);
}
static CONST_PREFIX char *php_set_access_handler_code(cmd_parms *cmd, php_per_dir_config *conf, char *arg1)
{
	return php_set_dir_handler(conf, arg1, AP_ACCESS_CONTROL, AP_HANDLER_TYPE_METHOD);
}

/* }}} */

/* {{{ php_set_type_handler
 */
static CONST_PREFIX char *php_set_type_handler(cmd_parms *cmd, php_per_dir_config *conf, char *arg1)
{
	return php_set_dir_handler(conf, arg1, AP_TYPE_CHECKING, AP_HANDLER_TYPE_FILE);
}
static CONST_PREFIX char *php_set_type_handler_code(cmd_parms *cmd, php_per_dir_config *conf, char *arg1)
{
	return php_set_dir_handler(conf, arg1, AP_TYPE_CHECKING, AP_HANDLER_TYPE_METHOD);
}

/* }}} */

/* {{{ php_set_fixup_handler
 */
static CONST_PREFIX char *php_set_fixup_handler(cmd_parms *cmd, php_per_dir_config *conf, char *arg1)
{
	return php_set_dir_handler(conf, arg1, AP_FIXUP, AP_HANDLER_TYPE_FILE);
}
static CONST_PREFIX char *php_set_fixup_handler_code(cmd_parms *cmd, php_per_dir_config *conf, char *arg1)
{
	return php_set_dir_handler(conf, arg1, AP_FIXUP, AP_HANDLER_TYPE_METHOD);
}
/* }}} */

/* {{{ php_set_logger_handler
 */
static CONST_PREFIX char *php_set_logger_handler(cmd_parms *cmd, php_per_dir_config *conf, char *arg1)
{
	return php_set_dir_handler(conf, arg1, AP_LOGGING, AP_HANDLER_TYPE_FILE);
}
static CONST_PREFIX char *php_set_logger_handler_code(cmd_parms *cmd, php_per_dir_config *conf, char *arg1)
{
	return php_set_dir_handler(conf, arg1, AP_LOGGING, AP_HANDLER_TYPE_METHOD);
}

/* }}} */ 

/* {{{ php_set_post_read_handler
 */
static CONST_PREFIX char *php_set_post_read_handler(cmd_parms *cmd, php_per_dir_config *conf, char *arg1)
{
	return php_set_dir_handler(conf, arg1, AP_POST_READ, AP_HANDLER_TYPE_FILE);
}
static CONST_PREFIX char *php_set_post_read_handler_code(cmd_parms *cmd, php_per_dir_config *conf, char *arg1)
{
	return php_set_dir_handler(conf, arg1, AP_POST_READ, AP_HANDLER_TYPE_METHOD);
}


/* }}} */

/* {{{ php_set_require
 */

static CONST_PREFIX char *php_set_require(cmd_parms *cmd, void *dummy, char *arg1)
{
	php_per_server_config *conf;
	return php_set_server_handler(cmd->server, arg1, 0, AP_HANDLER_TYPE_FILE);
}
/* }}} */

/* {{{ php_set_response_handler
 */
static CONST_PREFIX char *php_set_response_handler(cmd_parms *cmd, php_per_dir_config *conf, char *arg1)
{
	return php_set_dir_handler(conf, arg1, AP_RESPONSE, AP_HANDLER_TYPE_FILE);
}
static CONST_PREFIX char *php_set_response_handler_code(cmd_parms *cmd, php_per_dir_config *conf, char *arg1)
{
	return php_set_dir_handler(conf, arg1, AP_RESPONSE, AP_HANDLER_TYPE_METHOD);
}
/* }}} */
	
/* {{{ php_apache_value_handler
 */
static CONST_PREFIX char *php_apache_value_handler(cmd_parms *cmd, php_per_dir_config *conf, char *arg1, char *arg2)
{
	return php_apache_value_handler_ex(cmd, conf->ini_settings, arg1, arg2, PHP_INI_PERDIR);
}
/* }}} */

/* {{{ php_apache_admin_value_handler
 */
static CONST_PREFIX char *php_apache_admin_value_handler(cmd_parms *cmd, php_per_dir_config *conf, char *arg1, char *arg2)
{
	return php_apache_value_handler_ex(cmd, conf->ini_settings, arg1, arg2, PHP_INI_SYSTEM);
}
/* }}} */

/* {{{ php_apache_flag_handler_ex
 */
static CONST_PREFIX char *php_apache_flag_handler_ex(cmd_parms *cmd, HashTable *conf, char *arg1, char *arg2, int mode)
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
/* }}} */

/* {{{ php_apache_flag_handler
 */
static CONST_PREFIX char *php_apache_flag_handler(cmd_parms *cmd, php_per_dir_config *conf, char *arg1, char *arg2)
{
	return php_apache_flag_handler_ex(cmd, conf->ini_settings, arg1, arg2, PHP_INI_PERDIR);
}
/* }}} */

/* {{{ php_apache_admin_flag_handler
 */
static CONST_PREFIX char *php_apache_admin_flag_handler(cmd_parms *cmd, php_per_dir_config *conf, char *arg1, char *arg2)
{
	return php_apache_flag_handler_ex(cmd, conf->ini_settings, arg1, arg2, PHP_INI_SYSTEM);
}
/* }}} */

/* {{{ int php_xbithack_handler(request_rec * r)
 */
static int php_xbithack_handler(request_rec * r)
{
	php_per_dir_config *conf;
	TSRMLS_FETCH();

	if (!(r->finfo.st_mode & S_IXUSR)) {
		r->allowed |= (1 << METHODS) - 1;
		return DECLINED;
	}
	conf = (php_per_dir_config *) get_module_config(r->per_dir_config, &php4_module);
	if (conf) {
		zend_hash_apply((HashTable *) conf->ini_settings, (apply_func_t) php_apache_alter_ini_entries TSRMLS_CC);
	}
	if(!AP(xbithack)) {
		r->allowed |= (1 << METHODS) - 1;
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

static int php_run_hook(php_handler *handler, request_rec *r)
{
	zval *ret = NULL;
	php_per_dir_config *conf;

	TSRMLS_FETCH();
	if(!AP(apache_config_loaded)) {
		conf = (php_per_dir_config *) get_module_config(r->per_dir_config, &php4_module);
		if (conf)
			   zend_hash_apply((HashTable *)conf->ini_settings, (apply_func_t) php_apache_alter_ini_entries TSRMLS_CC);
		AP(apache_config_loaded) = 1;
	}
	if (!handler->name)
			return DECLINED;
	hard_timeout("send", r);
	SG(server_context) = r;
	php_save_umask();
	if (!AP(setup_env)) {
		add_common_vars(r);
		add_cgi_vars(r);
		AP(setup_env) = 1;
	}
	init_request_info(TSRMLS_C);
	apache_php_module_hook(r, handler, &ret TSRMLS_CC);
	php_restore_umask();
	kill_timeout(r);
	if (ret) {
		convert_to_long(ret);
		return Z_LVAL_P(ret);
	}
	return HTTP_INTERNAL_SERVER_ERROR;
}
 

static int php_uri_translation(request_rec *r)
{	
	TSRMLS_FETCH();
	php_per_server_config *conf;
	AP(current_hook) = AP_URI_TRANS;
	conf = (php_per_server_config *) get_module_config(r->server->module_config, &php4_module);
	return sapi_stack_apply_with_argument_stop_if_equals(&conf->uri_handlers, 
			ZEND_STACK_APPLY_BOTTOMUP, 
			(int (*)(void *element, void *)) php_run_hook, r, OK);
}

static int php_header_hook(request_rec *r)
{
	TSRMLS_FETCH();
	php_per_dir_config *conf;
	AP(current_hook) = AP_HEADER_PARSE;
	conf = (php_per_dir_config *) get_module_config(r->per_dir_config, &php4_module);
	return sapi_stack_apply_with_argument_stop_if_http_error(&conf->headers_handlers,
			ZEND_STACK_APPLY_BOTTOMUP,
			(int (*)(void *element, void *)) php_run_hook, r);
}

static int php_auth_hook(request_rec *r)
{
	TSRMLS_FETCH();
	php_per_dir_config *conf;
	AP(current_hook) = AP_AUTHENTICATION;
	conf = (php_per_dir_config *) get_module_config(r->per_dir_config, &php4_module);
	return sapi_stack_apply_with_argument_stop_if_equals(&conf->auth_handlers, 
			ZEND_STACK_APPLY_BOTTOMUP, 
			(int (*)(void *element, void *)) php_run_hook, r, OK);
}

static int php_access_hook(request_rec *r)
{
	TSRMLS_FETCH();
	php_per_dir_config *conf;
	AP(current_hook) = AP_ACCESS_CONTROL;
	conf = (php_per_dir_config *) get_module_config(r->per_dir_config, &php4_module);
	return sapi_stack_apply_with_argument_stop_if_http_error(&conf->access_handlers,
			ZEND_STACK_APPLY_BOTTOMUP,
			(int (*)(void *element, void *)) php_run_hook, r);

}

static int php_type_hook(request_rec *r)
{
	TSRMLS_FETCH();
	php_per_dir_config *conf;
	AP(current_hook) = AP_TYPE_CHECKING;
	conf = (php_per_dir_config *) get_module_config(r->per_dir_config, &php4_module);
	return sapi_stack_apply_with_argument_stop_if_equals(&conf->type_handlers,
			ZEND_STACK_APPLY_BOTTOMUP,
			(int (*)(void *element, void *)) php_run_hook,
			r, OK);
}

static int php_fixup_hook(request_rec *r)
{
	TSRMLS_FETCH();
	php_per_dir_config *conf;
	AP(current_hook) = AP_FIXUP;
	conf = (php_per_dir_config *) get_module_config(r->per_dir_config, &php4_module);
	return sapi_stack_apply_with_argument_stop_if_http_error(&conf->fixup_handlers,
			ZEND_STACK_APPLY_BOTTOMUP,
			(int (*)(void *element, void *)) php_run_hook,
			r);
}

static int php_logger_hook(request_rec *r)
{
	TSRMLS_FETCH();
	php_per_dir_config *conf;
	AP(current_hook) = AP_LOGGING;
	conf = (php_per_dir_config *) get_module_config(r->per_dir_config, &php4_module);
	return sapi_stack_apply_with_argument_stop_if_http_error(&conf->logger_handlers,
			ZEND_STACK_APPLY_BOTTOMUP,
			(int (*)(void *element, void *)) php_run_hook,
			r);
}
 
static int php_post_read_hook(request_rec *r)
{
	TSRMLS_FETCH();
	php_per_dir_config *conf;
	php_per_server_config *svr;
	AP(current_hook) = AP_POST_READ;
	svr = get_module_config(r->server->module_config, &php4_module);
	if(ap_is_initial_req(r)) {
		sapi_stack_apply_with_argument_all(&svr->requires, ZEND_STACK_APPLY_BOTTOMUP, (int (*)(void *element, void *)) php_run_hook, r);
	}
	conf = (php_per_dir_config *) get_module_config(r->per_dir_config, &php4_module);
	return sapi_stack_apply_with_argument_stop_if_http_error(&conf->post_read_handlers,
			ZEND_STACK_APPLY_BOTTOMUP,
			(int (*)(void *element, void *)) php_run_hook, r);
}

static int php_response_handler(request_rec *r)
{
	TSRMLS_FETCH();
	php_per_dir_config *conf;
	AP(current_hook) = AP_RESPONSE;
	conf = (php_per_dir_config *) get_module_config(r->per_dir_config, &php4_module);
	return sapi_stack_apply_with_argument_all(&conf->response_handlers, ZEND_STACK_APPLY_BOTTOMUP, (int (*)(void *element, void *)) php_run_hook, r);
}

/* {{{ handler_rec php_handlers[]
 */
handler_rec php_handlers[] =
{
	{"application/x-httpd-php", send_parsed_php},
	{"application/x-httpd-php-source", send_parsed_php_source},
	{"text/html", php_xbithack_handler},
		{"php-script", php_response_handler},
	{NULL}
};
/* }}} */

/* {{{ command_rec php_commands[]
 */
command_rec php_commands[] =
{
	{"php_value",		php_apache_value_handler, NULL, OR_OPTIONS, TAKE2, "PHP Value Modifier"},
	{"phpUriHandler",		php_set_uri_handler, NULL, RSRC_CONF, TAKE1, "PHP Value Modifier"},
	{"phpUriHandlerMethod",		php_set_uri_handler_code, NULL, RSRC_CONF, TAKE1, "PHP Value Modifier"},
#if MODULE_MAGIC_NUMBER >= 19970103
	{"phpHeaderHandler",		php_set_header_handler, NULL, OR_OPTIONS, TAKE1, "PHP Value Modifier"},
	{"phpHeaderHandlerMethod",		php_set_header_handler_code, NULL, OR_OPTIONS, TAKE1, "PHP Value Modifier"},
#endif
	{"phpAuthHandler",		php_set_auth_handler, NULL, OR_OPTIONS, TAKE1, "PHP Value Modifier"},
	{"phpAuthHandlerMethod",		php_set_auth_handler_code, NULL, OR_OPTIONS, TAKE1, "PHP Value Modifier"},
	{"phpAccessHandler",		php_set_access_handler, NULL, OR_OPTIONS, TAKE1, "PHP Value Modifier"},
	{"phpAccessHandlerMethod",		php_set_access_handler_code, NULL, OR_OPTIONS, TAKE1, "PHP Value Modifier"},
	{"phpTypeHandler",		php_set_type_handler, NULL, OR_OPTIONS, TAKE1, "PHP Value Modifier"},
	{"phpTypeHandlerMethod",		php_set_type_handler_code, NULL, OR_OPTIONS, TAKE1, "PHP Value Modifier"},
	{"phpFixupHandler",		php_set_fixup_handler, NULL, OR_OPTIONS, TAKE1, "PHP Value Modifier"},
	{"phpFixupHandlerMethod",		php_set_fixup_handler_code, NULL, OR_OPTIONS, TAKE1, "PHP Value Modifier"},
	{"phpLoggerHandler",			php_set_logger_handler, NULL, OR_OPTIONS, TAKE1, "PHP Value Modifier"},
	{"phpLoggerHandlerMethod",		php_set_logger_handler_code, NULL, OR_OPTIONS, TAKE1, "PHP Value Modifier"},
#if MODULE_MAGIC_NUMBER >= 19970902
	{"phpPostReadHandler",		php_set_post_read_handler, NULL, OR_OPTIONS, TAKE1, "PHP Value Modifier"},
	{"phpPostReadHandlerMethod",		php_set_post_read_handler_code, NULL, OR_OPTIONS, TAKE1, "PHP Value Modifier"},
	{"phpRequire",		php_set_require, NULL, OR_OPTIONS, TAKE1, "PHP Value Modifier"},
	{"phpResponseHandler",		php_set_response_handler, NULL, OR_OPTIONS, TAKE1, "PHP Value Modifier"},
	{"phpResponseHandlerMethod",		php_set_response_handler_code, NULL, OR_OPTIONS, TAKE1, "PHP Value Modifier"},
#endif
	{"php_flag",		php_apache_flag_handler, NULL, OR_OPTIONS, TAKE2, "PHP Flag Modifier"},
	{"php_admin_value",	php_apache_admin_value_handler, NULL, ACCESS_CONF|RSRC_CONF, TAKE2, "PHP Value Modifier (Admin)"},
	{"php_admin_flag",	php_apache_admin_flag_handler, NULL, ACCESS_CONF|RSRC_CONF, TAKE2, "PHP Flag Modifier (Admin)"},
	{NULL}
};
/* }}} */

/* {{{ module MODULE_VAR_EXPORT php4_module
 */
module MODULE_VAR_EXPORT php4_module =
{
	STANDARD_MODULE_STUFF,
	php_init_handler,			/* initializer */
	php_create_dir,				/* per-directory config creator */
	php_merge_dir,				/* dir merger */
	php_create_server,			/* per-server config creator */
	NULL, 						/* merge server config */
	php_commands,				/* command table */
	php_handlers,				/* handlers */
	php_uri_translation,		/* filename translation */
	NULL,						/* check_user_id */
	php_auth_hook,				/* check auth */
	php_access_hook,			/* check access */
	php_type_hook,				/* type_checker */
	php_fixup_hook,				/* fixups */
	php_logger_hook				/* logger */
#if MODULE_MAGIC_NUMBER >= 19970103
	, php_header_hook						/* header parser */
#endif
#if MODULE_MAGIC_NUMBER >= 19970719
	, NULL			 			/* child_init */
#endif
#if MODULE_MAGIC_NUMBER >= 19970728
	, php_child_exit_handler	/* child_exit */
#endif
#if MODULE_MAGIC_NUMBER >= 19970902
	, php_post_read_hook						/* post read-request */
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
