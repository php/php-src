/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Design:  Shane Caraveo <shane@caraveo.com>                           |
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

#include <ctype.h>

#include "SAPI.h"
#ifdef ZTS
#include "TSRM.h"
#endif

#include "rfc1867.h"

#if WIN32||WINNT
#define STRCASECMP stricmp
#else
#define STRCASECMP strcasecmp
#endif


SAPI_POST_READER_FUNC(sapi_read_standard_form_data);

#define DEFAULT_POST_CONTENT_TYPE "application/x-www-form-urlencoded"

static sapi_post_content_type_reader supported_post_content_types[] = {
	{ DEFAULT_POST_CONTENT_TYPE,	sizeof(DEFAULT_POST_CONTENT_TYPE)-1,	sapi_read_standard_form_data },
	{ MULTIPART_CONTENT_TYPE,		sizeof(MULTIPART_CONTENT_TYPE)-1,		rfc1867_post_reader	},
	{ NULL, 0, NULL }
};


static HashTable known_post_content_types;

SAPI_API void (*sapi_error)(int error_type, const char *message, ...);


#ifdef ZTS
SAPI_API int sapi_globals_id;
#else
sapi_globals_struct sapi_globals;
#endif


/* True globals (no need for thread safety) */
sapi_module_struct sapi_module;
SAPI_API void (*sapi_error)(int error_type, const char *message, ...);


SAPI_API void sapi_startup(sapi_module_struct *sf)
{
	sapi_post_content_type_reader *post_content_type_reader=supported_post_content_types;

	sapi_module = *sf;
	zend_hash_init(&known_post_content_types, 5, NULL, NULL, 1);
	while (post_content_type_reader->content_type) {
		sapi_register_post_reader(post_content_type_reader);
		post_content_type_reader++;
	}
#ifdef ZTS
	sapi_globals_id = ts_allocate_id(sizeof(sapi_globals_struct), NULL, NULL);
#endif
}


SAPI_API void sapi_shutdown()
{
	zend_hash_destroy(&known_post_content_types);
}


static void sapi_free_header(sapi_header_struct *sapi_header)
{
	efree(sapi_header->header);
}


#undef SAPI_POST_BLOCK_SIZE
#define SAPI_POST_BLOCK_SIZE 2


static void sapi_read_post_data(SLS_D)
{
	sapi_post_content_type_reader *post_content_type_reader;
	uint content_type_length = strlen(SG(request_info).content_type);
	char *content_type = estrndup(SG(request_info).content_type, content_type_length);
	char *p;
	char oldchar=0;

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

	if (zend_hash_find(&known_post_content_types, content_type, content_type_length+1, (void **) &post_content_type_reader)==FAILURE) {
		sapi_module.sapi_error(E_COMPILE_ERROR, "Unsupported content type:  '%s'", content_type);
		return;
	}
	if (oldchar) {
		*(p-1) = oldchar;
	}
	post_content_type_reader->post_reader(content_type SLS_CC);
	efree(content_type);
}


SAPI_POST_READER_FUNC(sapi_read_standard_form_data)
{
	int read_bytes, total_read_bytes=0;
	int allocated_bytes=SAPI_POST_BLOCK_SIZE+1;

	SG(request_info).post_data = emalloc(allocated_bytes);

	for (;;) {
		read_bytes = sapi_module.read_post(SG(request_info).post_data+total_read_bytes, SAPI_POST_BLOCK_SIZE SLS_CC);
		if (read_bytes<=0) {
			break;
		}
		total_read_bytes += read_bytes;
		if (read_bytes < SAPI_POST_BLOCK_SIZE) {
			break;
		}
		if (total_read_bytes+SAPI_POST_BLOCK_SIZE >= allocated_bytes) {
			allocated_bytes = total_read_bytes+SAPI_POST_BLOCK_SIZE+1;
			SG(request_info).post_data = erealloc(SG(request_info).post_data, allocated_bytes);
		}
	}
	SG(request_info).post_data[total_read_bytes] = 0;  /* terminating NULL */
}


SAPI_API void sapi_activate(SLS_D)
{
	zend_llist_init(&SG(sapi_headers).headers, sizeof(sapi_header_struct), (void (*)(void *)) sapi_free_header, 0);
	SG(sapi_headers).send_default_content_type = 1;
	SG(sapi_headers).http_response_code = 200;
	SG(sapi_headers).http_status_line = NULL;
	SG(headers_sent) = 0;
	SG(read_post_bytes) = 0;
	SG(request_info).post_data = NULL;

	if (SG(request_info).request_method && !strcmp(SG(request_info).request_method, "HEAD")) {
		SG(request_info).headers_only = 1;
	} else {
		SG(request_info).headers_only = 0;
	}

	if (SG(server_context)) {
		if (SG(request_info).request_method 
			&& !strcmp(SG(request_info).request_method, "POST")) {
			if (!SG(request_info).content_type) {
				sapi_module.sapi_error(E_COMPILE_ERROR, "No content-type in POST request");
			}
			sapi_read_post_data(SLS_C);
		}
		SG(request_info).cookie_data = sapi_module.read_cookies(SLS_C);
	}
}


SAPI_API void sapi_deactivate(SLS_D)
{
	zend_llist_destroy(&SG(sapi_headers).headers);
	if (SG(request_info).post_data) {
		efree(SG(request_info).post_data);
	}
}


/* This function expects a *duplicated* string, that was previously emalloc()'d.
 * Pointers sent to this functions will be automatically freed by the framework.
 */
SAPI_API int sapi_add_header(char *header_line, uint header_line_len)
{
	int retval;
	sapi_header_struct sapi_header;
	char *colon_offset;
	SLS_FETCH();

	if (SG(headers_sent)) {
		sapi_module.sapi_error(E_WARNING, "Cannot add header information - headers already sent");
		efree(header_line);
		return FAILURE;
	}

	sapi_header.header = header_line;
	sapi_header.header_len = header_line_len;

	/* Check the header for a few cases that we have special support for in SAPI */
	if (!memcmp(header_line, "HTTP/", 5)) {
		SG(sapi_headers).http_status_line = header_line;
	} else {
		colon_offset = strchr(header_line, ':');
		if (colon_offset) {
			*colon_offset = 0;
			if (!STRCASECMP(header_line, "Content-Type")) {
				SG(sapi_headers).send_default_content_type = 0;
			} else if (!STRCASECMP(header_line, "Location")) {
				SG(sapi_headers).http_response_code = 302; /* redirect */
			} else if (!STRCASECMP(header_line, "WWW-Authenticate")) { /* HTTP Authentication */
				SG(sapi_headers).http_response_code = 401; /* authentication-required */
			}
			*colon_offset = ':';
		}
	}

	if (sapi_module.header_handler) {
		retval = sapi_module.header_handler(&sapi_header, &SG(sapi_headers) SLS_CC);
	} else {
		retval = SAPI_HEADER_ADD;
	}

	if (retval & SAPI_HEADER_DELETE_ALL) {
		zend_llist_clean(&SG(sapi_headers).headers);
	}
	if (retval & SAPI_HEADER_ADD) {
		zend_llist_add_element(&SG(sapi_headers).headers, (void *) &sapi_header);
	}
	return SUCCESS;
}


SAPI_API int sapi_send_headers()
{
	int retval;
	sapi_header_struct default_header = { SAPI_DEFAULT_CONTENT_TYPE, sizeof(SAPI_DEFAULT_CONTENT_TYPE)-1 };
	SLS_FETCH();

	if (SG(headers_sent)) {
		return SUCCESS;
	}

	if (sapi_module.send_headers) {
		retval = sapi_module.send_headers(&SG(sapi_headers) SLS_CC);
	} else {
		retval = SAPI_HEADER_DO_SEND;
	}

	switch (retval) {
		case SAPI_HEADER_SENT_SUCCESSFULLY:
			SG(headers_sent) = 1;
			return SUCCESS;
			break;
		case SAPI_HEADER_DO_SEND:
			if (SG(sapi_headers).http_status_line) {
				sapi_header_struct http_status_line;

				http_status_line.header = SG(sapi_headers).http_status_line;
				http_status_line.header_len = strlen(SG(sapi_headers).http_status_line);
				sapi_module.send_header(&http_status_line, SG(server_context));
				efree(SG(sapi_headers).http_status_line);
			}
			if (SG(sapi_headers).send_default_content_type) {
				sapi_module.send_header(&default_header, SG(server_context));
			}
			zend_llist_apply_with_argument(&SG(sapi_headers).headers, (void (*)(void *, void *)) sapi_module.send_header, SG(server_context));
			sapi_module.send_header(NULL, SG(server_context));
			SG(headers_sent) = 1;
			return SUCCESS;
			break;
		case SAPI_HEADER_SEND_FAILED:
			return FAILURE;
			break;
	}
	return FAILURE;
}


SAPI_API int sapi_register_post_reader(sapi_post_content_type_reader *post_content_type_reader)
{
	return zend_hash_add(&known_post_content_types, post_content_type_reader->content_type, post_content_type_reader->content_type_len+1, (void *) post_content_type_reader, sizeof(sapi_post_content_type_reader), NULL);
}


SAPI_API void sapi_unregister_post_reader(sapi_post_content_type_reader *post_content_type_reader)
{
	zend_hash_del(&known_post_content_types, post_content_type_reader->content_type, post_content_type_reader->content_type_len+1);
}
