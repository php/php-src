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
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Jim Winstead <jimw@php.net>                                 |
   |          Hartmut Holzgraefe <hholzgra@php.net>                       |
   |          Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"
#include "php_globals.h"
#include "php_streams.h"
#include "php_network.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef PHP_WIN32
#include <windows.h>
#include <winsock.h>
#define O_RDONLY _O_RDONLY
#include "win32/param.h"
#else
#include <sys/param.h>
#endif

#include "php_standard.h"

#include <sys/types.h>
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef PHP_WIN32
#include <winsock.h>
#else
#include <netinet/in.h>
#include <netdb.h>
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#endif

#if defined(PHP_WIN32) || defined(__riscos__)
#undef AF_UNIX
#endif

#if defined(AF_UNIX)
#include <sys/un.h>
#endif

#include "php_fopen_wrappers.h"

#define HTTP_HEADER_BLOCK_SIZE		1024

php_stream *php_stream_url_wrap_http(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	php_stream *stream = NULL;
	php_url *resource = NULL;
	int use_ssl;
	char *scratch = NULL;
	char *tmp = NULL;
	int scratch_len = 0;
	int body = 0;
	char location[HTTP_HEADER_BLOCK_SIZE];
	zval *response_header = NULL;
	int reqok = 0;
	char *http_header_line = NULL;
	char tmp_line[128];
	size_t chunk_size = 0, file_size = 0;

	resource = php_url_parse(path);
	if (resource == NULL)
		return NULL;
	
	use_ssl = resource->scheme && resource->scheme[4] == 's';

	/* choose default ports */
	if (use_ssl && resource->port == 0)
		resource->port = 443;
	else if (resource->port == 0)
		resource->port = 80;

	stream = php_stream_sock_open_host(resource->host, resource->port, SOCK_STREAM, NULL, 0);
	if (stream == NULL)	
		goto out;

	/* avoid buffering issues while reading header */
	if (options & STREAM_WILL_CAST)
		chunk_size = php_stream_sock_set_chunk_size(stream, 1 TSRMLS_CC);
	
	php_stream_context_set(stream, context);

	php_stream_notify_info(context, PHP_STREAM_NOTIFY_CONNECT, NULL, 0);
	
#if HAVE_OPENSSL_EXT
	if (use_ssl)	{
		if (php_stream_sock_ssl_activate(stream, 1) == FAILURE)	{
			if (options & REPORT_ERRORS)	{
				zend_error(E_WARNING, "Unable to activate SSL mode");
			}
			php_stream_close(stream);
			stream = NULL;
			goto out;
		}
	}
#endif

	scratch_len = strlen(path) + 32;
	scratch = emalloc(scratch_len);

	strcpy(scratch, "GET ");

	/* file */
	if (resource->path && *resource->path)
		strlcat(scratch, resource->path, scratch_len);
	else
		strlcat(scratch, "/", scratch_len);
	
	/* query string */
	if (resource->query)	{
		strlcat(scratch, "?", scratch_len);
		strlcat(scratch, resource->query, scratch_len);
	}

	/* protocol version we are speaking */
	strlcat(scratch, " HTTP/1.0\r\n", scratch_len);

	/* send it */
	php_stream_write(stream, scratch, strlen(scratch));

	/* authz header if it was specified */
	if (resource->user && resource->pass)	{
		/* decode the strings first */
		php_url_decode(resource->user, strlen(resource->user));
		php_url_decode(resource->pass, strlen(resource->pass));

		/* scratch is large enough, since it was made large enough for the whole URL */
		strcpy(scratch, resource->user);
		strcat(scratch, ":");
		strcat(scratch, resource->pass);

		tmp = php_base64_encode((unsigned char*)scratch, strlen(scratch), NULL);
		
		if (snprintf(scratch, scratch_len, "Authorization: Basic %s\r\n", tmp) > 0) {
			php_stream_write(stream, scratch, strlen(scratch));
			php_stream_notify_info(context, PHP_STREAM_NOTIFY_AUTH_REQUIRED, NULL, 0);
		}

		efree(tmp);
		tmp = NULL;
	}

	/* if the user has configured who they are, send a From: line */
	if (cfg_get_string("from", &tmp) == SUCCESS)	{
		if (snprintf(scratch, scratch_len, "From: %s\r\n", tmp) > 0)
			php_stream_write(stream, scratch, strlen(scratch));
	}

	/* Send Host: header so name-based virtual hosts work */
	if ((use_ssl && resource->port != 443) || (!use_ssl && resource->port != 80))	{
		if (snprintf(scratch, scratch_len, "Host: %s:%i\r\n", resource->host, resource->port) > 0)
			php_stream_write(stream, scratch, strlen(scratch));
	}
	else if (snprintf(scratch, scratch_len, "Host: %s\r\n", resource->host) > 0)
		php_stream_write(stream, scratch, strlen(scratch));

	php_stream_write_string(stream, "User-Agent: PHP/" PHP_VERSION "\r\n\r\n");

	location[0] = '\0';

	MAKE_STD_ZVAL(response_header);
	array_init(response_header);

	if (!php_stream_eof(stream))	{
		/* get response header */

		if (php_stream_gets(stream, tmp_line, sizeof(tmp_line)-1) != NULL)	{
			zval *http_response;
			int response_code;

			MAKE_STD_ZVAL(http_response);
			response_code = atoi(tmp_line + 9);
			if (response_code == 200) {
				reqok = 1;
			} else {
				switch(response_code) {
					case 403:
						php_stream_notify_error(context, PHP_STREAM_NOTIFY_AUTH_RESULT,
								tmp_line, response_code);
						break;
					default:
						php_stream_notify_error(context, PHP_STREAM_NOTIFY_FAILURE,
								tmp_line, response_code);
				}
			}
			
			Z_STRLEN_P(http_response) = strlen(tmp_line);
			Z_STRVAL_P(http_response) = estrndup(tmp_line, Z_STRLEN_P(http_response));
			if (Z_STRVAL_P(http_response)[Z_STRLEN_P(http_response)-1]=='\n') {
				Z_STRVAL_P(http_response)[Z_STRLEN_P(http_response)-1]=0;
				Z_STRLEN_P(http_response)--;
				if (Z_STRVAL_P(http_response)[Z_STRLEN_P(http_response)-1]=='\r') {
					Z_STRVAL_P(http_response)[Z_STRLEN_P(http_response)-1]=0;
					Z_STRLEN_P(http_response)--;
				}
			}
			Z_TYPE_P(http_response) = IS_STRING;
			zend_hash_next_index_insert(Z_ARRVAL_P(response_header), &http_response, sizeof(zval *), NULL);
		}
	}
	
	/* read past HTTP headers */
	
	http_header_line = emalloc(HTTP_HEADER_BLOCK_SIZE);

	while (!body && !php_stream_eof(stream))	{
		
		if (php_stream_gets(stream, http_header_line, HTTP_HEADER_BLOCK_SIZE-1) != NULL)	{
			char *p;
			int found_eol = 0;
			int http_header_line_length;
			
			http_header_line[HTTP_HEADER_BLOCK_SIZE-1] = '\0';
			p = http_header_line;
			while(*p)	{
				while(*p == '\n' || *p == '\r')	{
					*p = '\0';
					p--;
					found_eol = 1;
				}
				if (found_eol)
					break;
				p++;
			}
			http_header_line_length = p-http_header_line+1;
		
			if (!strncasecmp(http_header_line, "Location: ", 10)) {
				strlcpy(location, http_header_line + 10, sizeof(location));
			} else if (!strncasecmp(http_header_line, "Content-Type: ", 14)) {
				php_stream_notify_info(context, PHP_STREAM_NOTIFY_MIME_TYPE_IS, http_header_line + 14, 0);
			} else if (!strncasecmp(http_header_line, "Content-Length: ", 16)) {
				file_size = atoi(http_header_line + 16);
				php_stream_notify_file_size(context, file_size, http_header_line, 0);
			}
	

			if (http_header_line[0] == '\0')
				body = 1;
			else	{
				zval *http_header;

				MAKE_STD_ZVAL(http_header);

				ZVAL_STRINGL(http_header, http_header_line, http_header_line_length, 1);
				
				zend_hash_next_index_insert(Z_ARRVAL_P(response_header), &http_header, sizeof(zval *), NULL);
			}
		}
		else
			break;
	}

	if (!reqok)	{
			
		if (location[0] != '\0')
			php_stream_notify_info(context, PHP_STREAM_NOTIFY_REDIRECTED, location, 0);

		php_stream_close(stream);
		stream = NULL;

		if (location[0] != '\0')	{

			zval *entry, **entryp;
			char new_path[HTTP_HEADER_BLOCK_SIZE];

			*new_path='\0';
			if (strlen(location)<8 || (strncasecmp(location, "http://", 7) && strncasecmp(location, "https://", 8))) {
				strcpy(new_path, resource->scheme);
				strlcat(new_path, resource->host, sizeof(new_path));
				if ((use_ssl && resource->port != 443) || (!use_ssl && resource->port != 80)) {
					snprintf(new_path+strlen(new_path), sizeof(new_path)-strlen(new_path)-1, ":%d", resource->port);
				}
				if (*location != '/') {
					php_dirname(resource->path, strlen(resource->path));
					snprintf (new_path+strlen(new_path), sizeof(new_path)-strlen(new_path)-1, "%s/", resource->path);
				}
				strlcat(new_path, location, sizeof(new_path));
			}
			else {
				strlcpy(new_path, location, sizeof(new_path));
			}
			stream = php_stream_url_wrap_http(NULL, new_path, mode, options, opened_path, context STREAMS_CC TSRMLS_CC);
			if (stream && stream->wrapperdata)	{
				entryp = &entry;
				MAKE_STD_ZVAL(entry);
				ZVAL_EMPTY_STRING(entry);
				zend_hash_next_index_insert(Z_ARRVAL_P(response_header), entryp, sizeof(zval *), NULL);
				zend_hash_internal_pointer_reset(Z_ARRVAL_P(stream->wrapperdata));
				while (zend_hash_get_current_data(Z_ARRVAL_P(stream->wrapperdata), (void **)&entryp) == SUCCESS) {
					zval_add_ref(entryp);
					zend_hash_next_index_insert(Z_ARRVAL_P(response_header), entryp, sizeof(zval *), NULL);
					zend_hash_move_forward(Z_ARRVAL_P(stream->wrapperdata));
				}
				zval_dtor(stream->wrapperdata);
				FREE_ZVAL(stream->wrapperdata);
			}
		} else {
			if (options & REPORT_ERRORS)
				zend_error(E_WARNING, "HTTP request failed! %s", tmp_line);
		}
	}
out:
	if (http_header_line)
		efree(http_header_line);
	if (scratch)
		efree(scratch);
	php_url_free(resource);

	if (stream)	{
		stream->wrapperdata = response_header;
		php_stream_notify_progress_init(context, 0, file_size);
		if (options & STREAM_WILL_CAST)
			php_stream_sock_set_chunk_size(stream, chunk_size TSRMLS_CC);
	}

	if (response_header)	{
		zval *sym;
		MAKE_STD_ZVAL(sym);
		*sym = *response_header;
		zval_copy_ctor(sym);
		ZEND_SET_SYMBOL(EG(active_symbol_table), "http_response_header", sym);
	}
	
	return stream;
}

static int php_stream_http_stream_stat(php_stream_wrapper *wrapper,
		php_stream *stream,
		php_stream_statbuf *ssb
		TSRMLS_DC)
{
	/* one day, we could fill in the details based on Date: and Content-Length:
	 * headers.  For now, we return with a failure code to prevent the underlying
	 * file's details from being used instead. */
	return -1;
}

static php_stream_wrapper_ops http_stream_wops = {
	php_stream_url_wrap_http,
	NULL,
	php_stream_http_stream_stat,
	NULL
};

php_stream_wrapper php_stream_http_wrapper =	{
	&http_stream_wops,
	NULL,
	1 /* is_url */
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
