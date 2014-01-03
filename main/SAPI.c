/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Original design:  Shane Caraveo <shane@caraveo.com>                  |
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include <ctype.h>
#include <sys/stat.h>

#include "php.h"
#include "SAPI.h"
#include "php_variables.h"
#include "php_ini.h"
#include "ext/standard/php_string.h"
#include "ext/standard/pageinfo.h"
#if (HAVE_PCRE || HAVE_BUNDLED_PCRE) && !defined(COMPILE_DL_PCRE)
#include "ext/pcre/php_pcre.h"
#endif
#ifdef ZTS
#include "TSRM.h"
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#elif defined(PHP_WIN32)
#include "win32/time.h"
#endif

#include "rfc1867.h"

#ifdef PHP_WIN32
#define STRCASECMP stricmp
#else
#define STRCASECMP strcasecmp
#endif

#include "php_content_types.h"

#ifdef ZTS
SAPI_API int sapi_globals_id;
#else
sapi_globals_struct sapi_globals;
#endif

static void sapi_globals_ctor(sapi_globals_struct *sapi_globals TSRMLS_DC)
{
	memset(sapi_globals, 0, sizeof(*sapi_globals));
	zend_hash_init_ex(&sapi_globals->known_post_content_types, 5, NULL, NULL, 1, 0);
	php_setup_sapi_content_types(TSRMLS_C);
}

static void sapi_globals_dtor(sapi_globals_struct *sapi_globals TSRMLS_DC)
{
	zend_hash_destroy(&sapi_globals->known_post_content_types);
}

/* True globals (no need for thread safety) */
SAPI_API sapi_module_struct sapi_module;


SAPI_API void sapi_startup(sapi_module_struct *sf)
{
#ifdef ZEND_SIGNALS
	zend_signal_startup();
#endif

	sf->ini_entries = NULL;
	sapi_module = *sf;

#ifdef ZTS
	ts_allocate_id(&sapi_globals_id, sizeof(sapi_globals_struct), (ts_allocate_ctor) sapi_globals_ctor, (ts_allocate_dtor) sapi_globals_dtor);
# ifdef PHP_WIN32
	_configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
# endif
#else
	sapi_globals_ctor(&sapi_globals);
#endif

	virtual_cwd_startup(); /* Could use shutdown to free the main cwd but it would just slow it down for CGI */

#ifdef PHP_WIN32
	tsrm_win32_startup();
#endif

	reentrancy_startup();
}

SAPI_API void sapi_shutdown(void)
{
#ifdef ZTS
	ts_free_id(sapi_globals_id);
#else
	sapi_globals_dtor(&sapi_globals);
#endif

	reentrancy_shutdown();

	virtual_cwd_shutdown();

#ifdef PHP_WIN32
	tsrm_win32_shutdown();
#endif
}


SAPI_API void sapi_free_header(sapi_header_struct *sapi_header)
{
	efree(sapi_header->header);
}

/* {{{ proto bool header_register_callback(mixed callback)
   call a header function */
PHP_FUNCTION(header_register_callback)
{
	zval *callback_func;
	char *callback_name;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &callback_func) == FAILURE) {
		return;
	}
	
	if (!zend_is_callable(callback_func, 0, &callback_name TSRMLS_CC)) {
		efree(callback_name);
		RETURN_FALSE;
	}

	efree(callback_name);

	if (SG(callback_func)) {
		zval_ptr_dtor(&SG(callback_func));
		SG(fci_cache) = empty_fcall_info_cache;
	}

	SG(callback_func) = callback_func;

	Z_ADDREF_P(SG(callback_func));

	RETURN_TRUE;
}
/* }}} */

static void sapi_run_header_callback(TSRMLS_D)
{
	int   error;
	zend_fcall_info fci;
	char *callback_name = NULL;
	char *callback_error = NULL;
	zval *retval_ptr = NULL;
	
	if (zend_fcall_info_init(SG(callback_func), 0, &fci, &SG(fci_cache), &callback_name, &callback_error TSRMLS_CC) == SUCCESS) {
		fci.retval_ptr_ptr = &retval_ptr;
		
		error = zend_call_function(&fci, &SG(fci_cache) TSRMLS_CC);
		if (error == FAILURE) {
			goto callback_failed;
		} else if (retval_ptr) {
			zval_ptr_dtor(&retval_ptr);
		}
	} else {
callback_failed:
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not call the sapi_header_callback");
	}
	
	if (callback_name) {
		efree(callback_name);
	}
	if (callback_error) {
		efree(callback_error);
	}	
}

SAPI_API void sapi_handle_post(void *arg TSRMLS_DC)
{
	if (SG(request_info).post_entry && SG(request_info).content_type_dup) {
		SG(request_info).post_entry->post_handler(SG(request_info).content_type_dup, arg TSRMLS_CC);
		if (SG(request_info).post_data) {
			efree(SG(request_info).post_data);
			SG(request_info).post_data = NULL;
		}
		efree(SG(request_info).content_type_dup);
		SG(request_info).content_type_dup = NULL;
	}
}

static void sapi_read_post_data(TSRMLS_D)
{
	sapi_post_entry *post_entry;
	uint content_type_length = strlen(SG(request_info).content_type);
	char *content_type = estrndup(SG(request_info).content_type, content_type_length);
	char *p;
	char oldchar=0;
	void (*post_reader_func)(TSRMLS_D) = NULL;


	/* dedicated implementation for increased performance:
	 * - Make the content type lowercase
	 * - Trim descriptive data, stay with the content-type only
	 */
	for (p=content_type; p<content_type+content_type_length; p++) {
		switch (*p) {
			case ';':
			case ',':
			case ' ':
				content_type_length = p-content_type;
				oldchar = *p;
				*p = 0;
				break;
			default:
				*p = tolower(*p);
				break;
		}
	}

	/* now try to find an appropriate POST content handler */
	if (zend_hash_find(&SG(known_post_content_types), content_type,
			content_type_length+1, (void **) &post_entry) == SUCCESS) {
		/* found one, register it for use */
		SG(request_info).post_entry = post_entry;
		post_reader_func = post_entry->post_reader;
	} else {
		/* fallback */
		SG(request_info).post_entry = NULL;
		if (!sapi_module.default_post_reader) {
			/* no default reader ? */
			SG(request_info).content_type_dup = NULL;
			sapi_module.sapi_error(E_WARNING, "Unsupported content type:  '%s'", content_type);
			return;
		}
	}
	if (oldchar) {
		*(p-1) = oldchar;
	}

	SG(request_info).content_type_dup = content_type;

	if(post_reader_func) {
		post_reader_func(TSRMLS_C);
	}

	if(sapi_module.default_post_reader) {
		sapi_module.default_post_reader(TSRMLS_C);
	}
}


SAPI_API SAPI_POST_READER_FUNC(sapi_read_standard_form_data)
{
	int read_bytes;
	int allocated_bytes=SAPI_POST_BLOCK_SIZE+1;

	if ((SG(post_max_size) > 0) && (SG(request_info).content_length > SG(post_max_size))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "POST Content-Length of %ld bytes exceeds the limit of %ld bytes",
					SG(request_info).content_length, SG(post_max_size));
		return;
	}
	SG(request_info).post_data = emalloc(allocated_bytes);

	for (;;) {
		read_bytes = sapi_module.read_post(SG(request_info).post_data+SG(read_post_bytes), SAPI_POST_BLOCK_SIZE TSRMLS_CC);
		if (read_bytes<=0) {
			break;
		}
		SG(read_post_bytes) += read_bytes;
		if ((SG(post_max_size) > 0) && (SG(read_post_bytes) > SG(post_max_size))) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Actual POST length does not match Content-Length, and exceeds %ld bytes", SG(post_max_size));
			break;
		}
		if (read_bytes < SAPI_POST_BLOCK_SIZE) {
			break;
		}
		if (SG(read_post_bytes)+SAPI_POST_BLOCK_SIZE >= allocated_bytes) {
			allocated_bytes = SG(read_post_bytes)+SAPI_POST_BLOCK_SIZE+1;
			SG(request_info).post_data = erealloc(SG(request_info).post_data, allocated_bytes);
		}
	}
	SG(request_info).post_data[SG(read_post_bytes)] = 0;  /* terminating NULL */
	SG(request_info).post_data_length = SG(read_post_bytes);
}


static inline char *get_default_content_type(uint prefix_len, uint *len TSRMLS_DC)
{
	char *mimetype, *charset, *content_type;
	uint mimetype_len, charset_len;

	if (SG(default_mimetype)) {
		mimetype = SG(default_mimetype);
		mimetype_len = strlen(SG(default_mimetype));
	} else {
		mimetype = SAPI_DEFAULT_MIMETYPE;
		mimetype_len = sizeof(SAPI_DEFAULT_MIMETYPE) - 1;
	}
	if (SG(default_charset)) {
		charset = SG(default_charset);
		charset_len = strlen(SG(default_charset));
	} else {
		charset = SAPI_DEFAULT_CHARSET;
		charset_len = sizeof(SAPI_DEFAULT_CHARSET) - 1;
	}

	if (*charset && strncasecmp(mimetype, "text/", 5) == 0) {
		char *p;

		*len = prefix_len + mimetype_len + sizeof("; charset=") - 1 + charset_len;
		content_type = (char*)emalloc(*len + 1);
		p = content_type + prefix_len;
		memcpy(p, mimetype, mimetype_len);
		p += mimetype_len;
		memcpy(p, "; charset=", sizeof("; charset=") - 1);
		p += sizeof("; charset=") - 1;
		memcpy(p, charset, charset_len + 1);
	} else {
		*len = prefix_len + mimetype_len;
		content_type = (char*)emalloc(*len + 1);
		memcpy(content_type + prefix_len, mimetype, mimetype_len + 1);
	}
	return content_type;
}


SAPI_API char *sapi_get_default_content_type(TSRMLS_D)
{
	uint len;

	return get_default_content_type(0, &len TSRMLS_CC);
}


SAPI_API void sapi_get_default_content_type_header(sapi_header_struct *default_header TSRMLS_DC)
{
    uint len;

	default_header->header = get_default_content_type(sizeof("Content-type: ")-1, &len TSRMLS_CC);
	default_header->header_len = len;
	memcpy(default_header->header, "Content-type: ", sizeof("Content-type: ") - 1);
}

/*
 * Add charset on content-type header if the MIME type starts with
 * "text/", the default_charset directive is not empty and
 * there is not already a charset option in there.
 *
 * If "mimetype" is non-NULL, it should point to a pointer allocated
 * with emalloc().  If a charset is added, the string will be
 * re-allocated and the new length is returned.  If mimetype is
 * unchanged, 0 is returned.
 *
 */
SAPI_API size_t sapi_apply_default_charset(char **mimetype, size_t len TSRMLS_DC)
{
	char *charset, *newtype;
	size_t newlen;
	charset = SG(default_charset) ? SG(default_charset) : SAPI_DEFAULT_CHARSET;

	if (*mimetype != NULL) {
		if (*charset && strncmp(*mimetype, "text/", 5) == 0 && strstr(*mimetype, "charset=") == NULL) {
			newlen = len + (sizeof(";charset=")-1) + strlen(charset);
			newtype = emalloc(newlen + 1);
	 		PHP_STRLCPY(newtype, *mimetype, newlen + 1, len);
			strlcat(newtype, ";charset=", newlen + 1);
			strlcat(newtype, charset, newlen + 1);
			efree(*mimetype);
			*mimetype = newtype;
			return newlen;
		}
	}
	return 0;
}

SAPI_API void sapi_activate_headers_only(TSRMLS_D)
{
	if (SG(request_info).headers_read == 1)
		return;
	SG(request_info).headers_read = 1;
	zend_llist_init(&SG(sapi_headers).headers, sizeof(sapi_header_struct), 
			(void (*)(void *)) sapi_free_header, 0);
	SG(sapi_headers).send_default_content_type = 1;

	/* SG(sapi_headers).http_response_code = 200; */ 
	SG(sapi_headers).http_status_line = NULL;
	SG(sapi_headers).mimetype = NULL;
	SG(read_post_bytes) = 0;
	SG(request_info).post_data = NULL;
	SG(request_info).raw_post_data = NULL;
	SG(request_info).current_user = NULL;
	SG(request_info).current_user_length = 0;
	SG(request_info).no_headers = 0;
	SG(request_info).post_entry = NULL;
	SG(global_request_time) = 0;

	/*
	 * It's possible to override this general case in the activate() callback, 
	 * if necessary.
	 */
	if (SG(request_info).request_method && !strcmp(SG(request_info).request_method, "HEAD")) {
		SG(request_info).headers_only = 1;
	} else {
		SG(request_info).headers_only = 0;
	}
	if (SG(server_context)) {
		SG(request_info).cookie_data = sapi_module.read_cookies(TSRMLS_C);
		if (sapi_module.activate) {
			sapi_module.activate(TSRMLS_C);
		}
	}
	if (sapi_module.input_filter_init ) {
		sapi_module.input_filter_init(TSRMLS_C);
	}
}

/*
 * Called from php_request_startup() for every request.
 */

SAPI_API void sapi_activate(TSRMLS_D)
{
	zend_llist_init(&SG(sapi_headers).headers, sizeof(sapi_header_struct), (void (*)(void *)) sapi_free_header, 0);
	SG(sapi_headers).send_default_content_type = 1;

	/*
	SG(sapi_headers).http_response_code = 200;
	*/
	SG(sapi_headers).http_status_line = NULL;
	SG(sapi_headers).mimetype = NULL;
	SG(headers_sent) = 0;
	SG(callback_run) = 0;
	SG(callback_func) = NULL;
	SG(read_post_bytes) = 0;
	SG(request_info).post_data = NULL;
	SG(request_info).raw_post_data = NULL;
	SG(request_info).current_user = NULL;
	SG(request_info).current_user_length = 0;
	SG(request_info).no_headers = 0;
	SG(request_info).post_entry = NULL;
	SG(request_info).proto_num = 1000; /* Default to HTTP 1.0 */
	SG(global_request_time) = 0;

	/* It's possible to override this general case in the activate() callback, if necessary. */
	if (SG(request_info).request_method && !strcmp(SG(request_info).request_method, "HEAD")) {
		SG(request_info).headers_only = 1;
	} else {
		SG(request_info).headers_only = 0;
	}
	SG(rfc1867_uploaded_files) = NULL;

	/* Handle request method */
	if (SG(server_context)) {
		if (PG(enable_post_data_reading) && SG(request_info).request_method) {
			if (SG(request_info).content_type && !strcmp(SG(request_info).request_method, "POST")) {
				/* HTTP POST may contain form data to be processed into variables
				 * depending on given content type */
				sapi_read_post_data(TSRMLS_C);
			} else {
				/* Any other method with content payload will fill $HTTP_RAW_POST_DATA 
				 * if it is enabled by always_populate_raw_post_data. 
				 * It's up to the webserver to decide whether to allow a method or not. */
				SG(request_info).content_type_dup = NULL;
				if (sapi_module.default_post_reader) {
					sapi_module.default_post_reader(TSRMLS_C);
				}
			}
		} else {
			SG(request_info).content_type_dup = NULL;
		}

		/* Cookies */
		SG(request_info).cookie_data = sapi_module.read_cookies(TSRMLS_C);

		if (sapi_module.activate) {
			sapi_module.activate(TSRMLS_C);
		}
	}
	if (sapi_module.input_filter_init) {
		sapi_module.input_filter_init(TSRMLS_C);
	}
}


static void sapi_send_headers_free(TSRMLS_D)
{
	if (SG(sapi_headers).http_status_line) {
		efree(SG(sapi_headers).http_status_line);
		SG(sapi_headers).http_status_line = NULL;
	}
}
	
SAPI_API void sapi_deactivate(TSRMLS_D)
{
	zend_llist_destroy(&SG(sapi_headers).headers);
	if (SG(request_info).post_data) {
		efree(SG(request_info).post_data);
	}  else 	if (SG(server_context)) {
		if(sapi_module.read_post) { 
			/* make sure we've consumed all request input data */
			char dummy[SAPI_POST_BLOCK_SIZE];
			int read_bytes;

			while((read_bytes = sapi_module.read_post(dummy, sizeof(dummy)-1 TSRMLS_CC)) > 0) {
				SG(read_post_bytes) += read_bytes;
			}
		}
	}
	if (SG(request_info).raw_post_data) {
		efree(SG(request_info).raw_post_data);
	} 
	if (SG(request_info).auth_user) {
		efree(SG(request_info).auth_user);
	}
	if (SG(request_info).auth_password) {
		efree(SG(request_info).auth_password);
	}
	if (SG(request_info).auth_digest) {
		efree(SG(request_info).auth_digest);
	}
	if (SG(request_info).content_type_dup) {
		efree(SG(request_info).content_type_dup);
	}
	if (SG(request_info).current_user) {
		efree(SG(request_info).current_user);
	}
	if (sapi_module.deactivate) {
		sapi_module.deactivate(TSRMLS_C);
	}
	if (SG(rfc1867_uploaded_files)) {
		destroy_uploaded_files_hash(TSRMLS_C);
	}
	if (SG(sapi_headers).mimetype) {
		efree(SG(sapi_headers).mimetype);
		SG(sapi_headers).mimetype = NULL;
	}
	sapi_send_headers_free(TSRMLS_C);
	SG(sapi_started) = 0;
	SG(headers_sent) = 0;
	SG(callback_run) = 0;
	if (SG(callback_func)) {
		zval_ptr_dtor(&SG(callback_func));
	}
	SG(request_info).headers_read = 0;
	SG(global_request_time) = 0;
}


SAPI_API void sapi_initialize_empty_request(TSRMLS_D)
{
	SG(server_context) = NULL;
	SG(request_info).request_method = NULL;
	SG(request_info).auth_digest = SG(request_info).auth_user = SG(request_info).auth_password = NULL;
	SG(request_info).content_type_dup = NULL;
}


static int sapi_extract_response_code(const char *header_line)
{
	int code = 200;
	const char *ptr;

	for (ptr = header_line; *ptr; ptr++) {
		if (*ptr == ' ' && *(ptr + 1) != ' ') {
			code = atoi(ptr + 1);
			break;
		}
	}
	
	return code;
}


static void sapi_update_response_code(int ncode TSRMLS_DC)
{
	/* if the status code did not change, we do not want
	   to change the status line, and no need to change the code */
	if (SG(sapi_headers).http_response_code == ncode) {
		return;
	}

	if (SG(sapi_headers).http_status_line) {
		efree(SG(sapi_headers).http_status_line);
		SG(sapi_headers).http_status_line = NULL;
	}
	SG(sapi_headers).http_response_code = ncode;
}

/* 
 * since zend_llist_del_element only remove one matched item once,
 * we should remove them by ourself
 */
static void sapi_remove_header(zend_llist *l, char *name, uint len) {
	sapi_header_struct *header;
	zend_llist_element *next;
	zend_llist_element *current=l->head;

	while (current) {
		header = (sapi_header_struct *)(current->data);
		next = current->next;
		if (header->header_len > len && header->header[len] == ':'
				&& !strncasecmp(header->header, name, len)) {
			if (current->prev) {
				current->prev->next = next;
			} else {
				l->head = next;
			}
			if (next) {
				next->prev = current->prev;
			} else {
				l->tail = current->prev;
			}
			sapi_free_header(header);
			efree(current);
			--l->count;
		}
		current = next;
	}
}

SAPI_API int sapi_add_header_ex(char *header_line, uint header_line_len, zend_bool duplicate, zend_bool replace TSRMLS_DC)
{
	sapi_header_line ctr = {0};
	int r;
	
	ctr.line = header_line;
	ctr.line_len = header_line_len;

	r = sapi_header_op(replace ? SAPI_HEADER_REPLACE : SAPI_HEADER_ADD,
			&ctr TSRMLS_CC);

	if (!duplicate)
		efree(header_line);

	return r;
}

static void sapi_header_add_op(sapi_header_op_enum op, sapi_header_struct *sapi_header TSRMLS_DC)
{
	if (!sapi_module.header_handler ||
		(SAPI_HEADER_ADD & sapi_module.header_handler(sapi_header, op, &SG(sapi_headers) TSRMLS_CC))) {
		if (op == SAPI_HEADER_REPLACE) {
			char *colon_offset = strchr(sapi_header->header, ':');

			if (colon_offset) {
				char sav = *colon_offset;

				*colon_offset = 0;
		        sapi_remove_header(&SG(sapi_headers).headers, sapi_header->header, strlen(sapi_header->header));
				*colon_offset = sav;
			}
		}
		zend_llist_add_element(&SG(sapi_headers).headers, (void *) sapi_header);
	} else {
		sapi_free_header(sapi_header);
	}
}

SAPI_API int sapi_header_op(sapi_header_op_enum op, void *arg TSRMLS_DC)
{
	sapi_header_struct sapi_header;
	char *colon_offset;
	char *header_line;
	uint header_line_len;
	int http_response_code;

	if (SG(headers_sent) && !SG(request_info).no_headers) {
		const char *output_start_filename = php_output_get_start_filename(TSRMLS_C);
		int output_start_lineno = php_output_get_start_lineno(TSRMLS_C);

		if (output_start_filename) {
			sapi_module.sapi_error(E_WARNING, "Cannot modify header information - headers already sent by (output started at %s:%d)",
				output_start_filename, output_start_lineno);
		} else {
			sapi_module.sapi_error(E_WARNING, "Cannot modify header information - headers already sent");
		}
		return FAILURE;
	}

	switch (op) {
		case SAPI_HEADER_SET_STATUS:
			sapi_update_response_code((int)(zend_intptr_t) arg TSRMLS_CC);
			return SUCCESS;

		case SAPI_HEADER_ADD:
		case SAPI_HEADER_REPLACE:
		case SAPI_HEADER_DELETE: {
				sapi_header_line *p = arg;

				if (!p->line || !p->line_len) {
					return FAILURE;
				}
				header_line = p->line;
				header_line_len = p->line_len;
				http_response_code = p->response_code;
				break;
			}

		case SAPI_HEADER_DELETE_ALL:
			if (sapi_module.header_handler) {
				sapi_module.header_handler(&sapi_header, op, &SG(sapi_headers) TSRMLS_CC);
			}
			zend_llist_clean(&SG(sapi_headers).headers);
			return SUCCESS;

		default:
			return FAILURE;
	}

	header_line = estrndup(header_line, header_line_len);

	/* cut of trailing spaces, linefeeds and carriage-returns */
	if (header_line_len && isspace(header_line[header_line_len-1])) {
		do {
			header_line_len--;
		} while(header_line_len && isspace(header_line[header_line_len-1]));
		header_line[header_line_len]='\0';
	}
	
	if (op == SAPI_HEADER_DELETE) {
		if (strchr(header_line, ':')) {
			efree(header_line);
			sapi_module.sapi_error(E_WARNING, "Header to delete may not contain colon.");
			return FAILURE;
		}
		if (sapi_module.header_handler) {
			sapi_header.header = header_line;
			sapi_header.header_len = header_line_len;
			sapi_module.header_handler(&sapi_header, op, &SG(sapi_headers) TSRMLS_CC);
		}
		sapi_remove_header(&SG(sapi_headers).headers, header_line, header_line_len);
		efree(header_line);
		return SUCCESS;
	} else {
		/* new line/NUL character safety check */
		int i;
		for (i = 0; i < header_line_len; i++) {
			/* RFC 2616 allows new lines if followed by SP or HT */
			int illegal_break =
					(header_line[i+1] != ' ' && header_line[i+1] != '\t')
					&& (
						header_line[i] == '\n'
						|| (header_line[i] == '\r' && header_line[i+1] != '\n'));
			if (illegal_break) {
				efree(header_line);
				sapi_module.sapi_error(E_WARNING, "Header may not contain "
						"more than a single header, new line detected");
				return FAILURE;
			}
			if (header_line[i] == '\0') {
				efree(header_line);
				sapi_module.sapi_error(E_WARNING, "Header may not contain NUL bytes");
				return FAILURE;
			}
		}
	}

	sapi_header.header = header_line;
	sapi_header.header_len = header_line_len;

	/* Check the header for a few cases that we have special support for in SAPI */
	if (header_line_len>=5 
		&& !strncasecmp(header_line, "HTTP/", 5)) {
		/* filter out the response code */
		sapi_update_response_code(sapi_extract_response_code(header_line) TSRMLS_CC);
		/* sapi_update_response_code doesn't free the status line if the code didn't change */
		if (SG(sapi_headers).http_status_line) {
			efree(SG(sapi_headers).http_status_line);
		}
		SG(sapi_headers).http_status_line = header_line;
		return SUCCESS;
	} else {
		colon_offset = strchr(header_line, ':');
		if (colon_offset) {
			*colon_offset = 0;
			if (!STRCASECMP(header_line, "Content-Type")) {
				char *ptr = colon_offset+1, *mimetype = NULL, *newheader;
				size_t len = header_line_len - (ptr - header_line), newlen;
				while (*ptr == ' ') {
					ptr++;
					len--;
				}

				/* Disable possible output compression for images */
				if (!strncmp(ptr, "image/", sizeof("image/")-1)) {
					zend_alter_ini_entry("zlib.output_compression", sizeof("zlib.output_compression"), "0", sizeof("0") - 1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
				}

				mimetype = estrdup(ptr);
				newlen = sapi_apply_default_charset(&mimetype, len TSRMLS_CC);
				if (!SG(sapi_headers).mimetype){
					SG(sapi_headers).mimetype = estrdup(mimetype);
				}

				if (newlen != 0) {
					newlen += sizeof("Content-type: ");
					newheader = emalloc(newlen);
					PHP_STRLCPY(newheader, "Content-type: ", newlen, sizeof("Content-type: ")-1);
					strlcat(newheader, mimetype, newlen);
					sapi_header.header = newheader;
					sapi_header.header_len = newlen - 1;
					efree(header_line);
				}
				efree(mimetype);
				SG(sapi_headers).send_default_content_type = 0;
			} else if (!STRCASECMP(header_line, "Content-Length")) {
				/* Script is setting Content-length. The script cannot reasonably
				 * know the size of the message body after compression, so it's best
				 * do disable compression altogether. This contributes to making scripts
				 * portable between setups that have and don't have zlib compression
				 * enabled globally. See req #44164 */
				zend_alter_ini_entry("zlib.output_compression", sizeof("zlib.output_compression"),
					"0", sizeof("0") - 1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
			} else if (!STRCASECMP(header_line, "Location")) {
				if ((SG(sapi_headers).http_response_code < 300 ||
					SG(sapi_headers).http_response_code > 307) &&
					SG(sapi_headers).http_response_code != 201) {
					/* Return a Found Redirect if one is not already specified */
					if (http_response_code) { /* user specified redirect code */
						sapi_update_response_code(http_response_code TSRMLS_CC);
					} else if (SG(request_info).proto_num > 1000 && 
					   SG(request_info).request_method && 
					   strcmp(SG(request_info).request_method, "HEAD") &&
					   strcmp(SG(request_info).request_method, "GET")) {
						sapi_update_response_code(303 TSRMLS_CC);
					} else {
						sapi_update_response_code(302 TSRMLS_CC);
					}
				}
			} else if (!STRCASECMP(header_line, "WWW-Authenticate")) { /* HTTP Authentication */
				sapi_update_response_code(401 TSRMLS_CC); /* authentication-required */
			}
			if (sapi_header.header==header_line) {
				*colon_offset = ':';
			}
		}
	}
	if (http_response_code) {
		sapi_update_response_code(http_response_code TSRMLS_CC);
	}
	sapi_header_add_op(op, &sapi_header TSRMLS_CC);
	return SUCCESS;
}


SAPI_API int sapi_send_headers(TSRMLS_D)
{
	int retval;
	int ret = FAILURE;

	if (SG(headers_sent) || SG(request_info).no_headers || SG(callback_run)) {
		return SUCCESS;
	}

	/* Success-oriented.  We set headers_sent to 1 here to avoid an infinite loop
	 * in case of an error situation.
	 */
	if (SG(sapi_headers).send_default_content_type && sapi_module.send_headers) {
		sapi_header_struct default_header;
	    uint len;

		SG(sapi_headers).mimetype = get_default_content_type(0, &len TSRMLS_CC);
		default_header.header_len = sizeof("Content-type: ") - 1 + len;
		default_header.header = emalloc(default_header.header_len + 1);
		memcpy(default_header.header, "Content-type: ", sizeof("Content-type: ") - 1);
		memcpy(default_header.header + sizeof("Content-type: ") - 1, SG(sapi_headers).mimetype, len + 1);
		sapi_header_add_op(SAPI_HEADER_ADD, &default_header TSRMLS_CC);
		SG(sapi_headers).send_default_content_type = 0;
	}

	if (SG(callback_func) && !SG(callback_run)) {
		SG(callback_run) = 1;
		sapi_run_header_callback(TSRMLS_C);
	}

	SG(headers_sent) = 1;

	if (sapi_module.send_headers) {
		retval = sapi_module.send_headers(&SG(sapi_headers) TSRMLS_CC);
	} else {
		retval = SAPI_HEADER_DO_SEND;
	}

	switch (retval) {
		case SAPI_HEADER_SENT_SUCCESSFULLY:
			ret = SUCCESS;
			break;
		case SAPI_HEADER_DO_SEND: {
				sapi_header_struct http_status_line;
				char buf[255];

				if (SG(sapi_headers).http_status_line) {
					http_status_line.header = SG(sapi_headers).http_status_line;
					http_status_line.header_len = strlen(SG(sapi_headers).http_status_line);
				} else {
					http_status_line.header = buf;
					http_status_line.header_len = slprintf(buf, sizeof(buf), "HTTP/1.0 %d X", SG(sapi_headers).http_response_code);
				}
				sapi_module.send_header(&http_status_line, SG(server_context) TSRMLS_CC);
			}
			zend_llist_apply_with_argument(&SG(sapi_headers).headers, (llist_apply_with_arg_func_t) sapi_module.send_header, SG(server_context) TSRMLS_CC);
			if(SG(sapi_headers).send_default_content_type) {
				sapi_header_struct default_header;

				sapi_get_default_content_type_header(&default_header TSRMLS_CC);
				sapi_module.send_header(&default_header, SG(server_context) TSRMLS_CC);
				sapi_free_header(&default_header);
			}
			sapi_module.send_header(NULL, SG(server_context) TSRMLS_CC);
			ret = SUCCESS;
			break;
		case SAPI_HEADER_SEND_FAILED:
			SG(headers_sent) = 0;
			ret = FAILURE;
			break;
	}

	sapi_send_headers_free(TSRMLS_C);

	return ret;
}


SAPI_API int sapi_register_post_entries(sapi_post_entry *post_entries TSRMLS_DC)
{
	sapi_post_entry *p=post_entries;

	while (p->content_type) {
		if (sapi_register_post_entry(p TSRMLS_CC) == FAILURE) {
			return FAILURE;
		}
		p++;
	}
	return SUCCESS;
}


SAPI_API int sapi_register_post_entry(sapi_post_entry *post_entry TSRMLS_DC)
{
	if (SG(sapi_started) && EG(in_execution)) {
		return FAILURE;
	}
	return zend_hash_add(&SG(known_post_content_types),
			post_entry->content_type, post_entry->content_type_len+1,
			(void *) post_entry, sizeof(sapi_post_entry), NULL);
}

SAPI_API void sapi_unregister_post_entry(sapi_post_entry *post_entry TSRMLS_DC)
{
	if (SG(sapi_started) && EG(in_execution)) {
		return;
	}
	zend_hash_del(&SG(known_post_content_types), post_entry->content_type,
			post_entry->content_type_len+1);
}


SAPI_API int sapi_register_default_post_reader(void (*default_post_reader)(TSRMLS_D) TSRMLS_DC)
{
	if (SG(sapi_started) && EG(in_execution)) {
		return FAILURE;
	}
	sapi_module.default_post_reader = default_post_reader;
	return SUCCESS;
}


SAPI_API int sapi_register_treat_data(void (*treat_data)(int arg, char *str, zval *destArray TSRMLS_DC) TSRMLS_DC)
{
	if (SG(sapi_started) && EG(in_execution)) {
		return FAILURE;
	}
	sapi_module.treat_data = treat_data;
	return SUCCESS;
}

SAPI_API int sapi_register_input_filter(unsigned int (*input_filter)(int arg, char *var, char **val, unsigned int val_len, unsigned int *new_val_len TSRMLS_DC), unsigned int (*input_filter_init)(TSRMLS_D) TSRMLS_DC)
{
	if (SG(sapi_started) && EG(in_execution)) {
		return FAILURE;
	}
	sapi_module.input_filter = input_filter;
	sapi_module.input_filter_init = input_filter_init;
	return SUCCESS;
}

SAPI_API int sapi_flush(TSRMLS_D)
{
	if (sapi_module.flush) {
		sapi_module.flush(SG(server_context));
		return SUCCESS;
	} else {
		return FAILURE;
	}
}

SAPI_API struct stat *sapi_get_stat(TSRMLS_D)
{
	if (sapi_module.get_stat) {
		return sapi_module.get_stat(TSRMLS_C);
	} else {
		if (!SG(request_info).path_translated || (VCWD_STAT(SG(request_info).path_translated, &SG(global_stat)) == -1)) {
			return NULL;
		}
		return &SG(global_stat);
	}
}

SAPI_API char *sapi_getenv(char *name, size_t name_len TSRMLS_DC)
{
	if (sapi_module.getenv) { 
		char *value, *tmp = sapi_module.getenv(name, name_len TSRMLS_CC);
		if (tmp) {
			value = estrdup(tmp);
		} else {
			return NULL;
		}
		if (sapi_module.input_filter) {
			sapi_module.input_filter(PARSE_STRING, name, &value, strlen(value), NULL TSRMLS_CC);
		}
		return value;
	}
	return NULL;
}

SAPI_API int sapi_get_fd(int *fd TSRMLS_DC)
{
	if (sapi_module.get_fd) {
		return sapi_module.get_fd(fd TSRMLS_CC);
	} else {
		return FAILURE;
	}
}

SAPI_API int sapi_force_http_10(TSRMLS_D)
{
	if (sapi_module.force_http_10) {
		return sapi_module.force_http_10(TSRMLS_C);
	} else {
		return FAILURE;
	}
}


SAPI_API int sapi_get_target_uid(uid_t *obj TSRMLS_DC)
{
	if (sapi_module.get_target_uid) {
		return sapi_module.get_target_uid(obj TSRMLS_CC);
	} else {
		return FAILURE;
	}
}

SAPI_API int sapi_get_target_gid(gid_t *obj TSRMLS_DC)
{
	if (sapi_module.get_target_gid) {
		return sapi_module.get_target_gid(obj TSRMLS_CC);
	} else {
		return FAILURE;
	}
}

SAPI_API double sapi_get_request_time(TSRMLS_D)
{
	if(SG(global_request_time)) return SG(global_request_time);

	if (sapi_module.get_request_time && SG(server_context)) {
		SG(global_request_time) = sapi_module.get_request_time(TSRMLS_C);
	} else {
		struct timeval tp = {0};
		if (!gettimeofday(&tp, NULL)) {
			SG(global_request_time) = (double)(tp.tv_sec + tp.tv_usec / 1000000.00);
		} else {
			SG(global_request_time) = (double)time(0);
		}
	}
	return SG(global_request_time);
}

SAPI_API void sapi_terminate_process(TSRMLS_D) {
	if (sapi_module.terminate_process) {
		sapi_module.terminate_process(TSRMLS_C);
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
