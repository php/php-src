/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Jim Winstead <jimw@php.net>                                 |
   |          Hartmut Holzgraefe <hholzgra@php.net>                       |
   |          Wez Furlong <wez@thebrainroom.com>                          |
   |          Sara Golemon <pollita@php.net>                              |
   +----------------------------------------------------------------------+
 */
/* $Id$ */ 

#include "php.h"
#include "php_globals.h"
#include "php_streams.h"
#include "php_network.h"
#include "php_ini.h"
#include "ext/standard/basic_functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef PHP_WIN32
#define O_RDONLY _O_RDONLY
#include "win32/param.h"
#elif defined(NETWARE)
/*#include <ws2nlm.h>*/
/*#include <sys/socket.h>*/
#ifdef NEW_LIBC
#include <sys/param.h>
#else
#include "netware/param.h"
#endif
#else
#include <sys/param.h>
#endif

#include "php_standard.h"

#include <sys/types.h>
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef PHP_WIN32
#include <winsock2.h>
#elif defined(NETWARE) && defined(USE_WINSOCK)
/*#include <ws2nlm.h>*/
#include <novsock2.h>
#else
#include <netinet/in.h>
#include <netdb.h>
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#endif

#if defined(PHP_WIN32) || defined(__riscos__) || defined(NETWARE)
#undef AF_UNIX
#endif

#if defined(AF_UNIX)
#include <sys/un.h>
#endif

#include "php_fopen_wrappers.h"

#define HTTP_HEADER_BLOCK_SIZE		1024
#define PHP_URL_REDIRECT_MAX		20
#define HTTP_HEADER_USER_AGENT		1
#define HTTP_HEADER_HOST			2
#define HTTP_HEADER_AUTH			4
#define HTTP_HEADER_FROM			8
#define HTTP_HEADER_CONTENT_LENGTH	16
#define HTTP_HEADER_TYPE			32

/* 8 hexits plus \r\n\0 */
#define HTTP_CHUNK_SIZE_MAXLEN				11
#define HTTP_CHUNKED_ENCODING_BUFFER_LEN	(HTTP_CHUNK_SIZE_MAXLEN + 1)

typedef struct _php_http_chunked_encoding_data {
	int is_persistent;
	size_t chunk_remaining;
	char chunksize_buffer[HTTP_CHUNKED_ENCODING_BUFFER_LEN];
	char *chunksize_buffer_pos;
} php_http_chunked_encoding_data;

static php_stream_filter_status_t php_http_chunked_encoding_filter(
    php_stream *stream,
    php_stream_filter *thisfilter,
    php_stream_bucket_brigade *buckets_in,
    php_stream_bucket_brigade *buckets_out,
    size_t *bytes_consumed,
    int flags
    TSRMLS_DC)
{
	php_stream_bucket *bucket;
	php_http_chunked_encoding_data *data = (php_http_chunked_encoding_data*)thisfilter->abstract;
	size_t consumed = 0;
	char *buf;
	size_t buflen;

	while (buckets_in->head) {
		char *e = NULL;
		size_t chunk_remaining;

		bucket = buckets_in->head;
		php_stream_bucket_unlink(bucket TSRMLS_CC);

		buf = bucket->buf;
		buflen = bucket->buflen;

continue_bucket:

		if (data->chunk_remaining > 0) {
			if ((data->chunk_remaining > buflen) && (bucket->buf == buf)) {
				/* This bucket is smaller than our remaining chunksize,
				   Pass it on unmolested */
				consumed += buflen;
				data->chunk_remaining -= buflen;
				php_stream_bucket_append(buckets_out, bucket TSRMLS_CC);

				/* Next bucket please */
				continue;
			} else if (data->chunk_remaining > buflen) {
				php_stream_bucket *newbucket;
				char *newbuf;

				/* Previously split bucket can be used en toto */
				consumed += buflen;
				data->chunk_remaining -= buflen;

				newbuf = estrndup(buf, buflen);
				newbucket = php_stream_bucket_new(stream, newbuf, buflen, 1, stream->is_persistent TSRMLS_CC);
				php_stream_bucket_append(buckets_out, newbucket TSRMLS_CC);
				php_stream_bucket_delref(bucket TSRMLS_CC);
				/* Next bucket please */
				continue;
			} else {
				php_stream_bucket *newbucket;
				char *newbuf;

				/* Consume enough of this bucket to satisfy the current chunk */
				newbuf = pemalloc(data->chunk_remaining, stream->is_persistent);
				memcpy(newbuf, buf, data->chunk_remaining);

				newbucket = php_stream_bucket_new(stream, newbuf, data->chunk_remaining, 1, stream->is_persistent TSRMLS_CC);
				php_stream_bucket_append(buckets_out, newbucket TSRMLS_CC);
				buf += data->chunk_remaining;
				buflen -= data->chunk_remaining;
				consumed += data->chunk_remaining;
				data->chunk_remaining = 0;
				/* Fall Through */
			}
		}

		while (buflen > 0 && (*buf == '\r' || *buf == '\n')) {
			buf++;
			buflen--;
		}

		if (buflen <= 0) {
			php_stream_bucket_delref(bucket TSRMLS_CC);
			continue;
		}

		if ((data->chunksize_buffer_pos - data->chunksize_buffer) < HTTP_CHUNK_SIZE_MAXLEN) {
			/* Copy buf into chunksize_buffer */
			if ((HTTP_CHUNKED_ENCODING_BUFFER_LEN - (data->chunksize_buffer_pos - data->chunksize_buffer)) > buflen) {
				/* Possible split chunklen */
				memcpy(data->chunksize_buffer_pos, buf, buflen);
				memset(data->chunksize_buffer_pos + buflen, 0, HTTP_CHUNK_SIZE_MAXLEN - buflen - (data->chunksize_buffer_pos - data->chunksize_buffer));
				chunk_remaining = strtoul(data->chunksize_buffer, &e, 16);
				if (*e != '\r') {
					if (!((*e >= '0' && *e <= '9') ||
						  (*e >= 'a' && *e <= 'f') ||
						  (*e >= 'A' && *e <= 'F'))) {
							/* Unexpected character */
						return PSFS_ERR_FATAL;
					}

					/* Not enough data to know the chunksize yet */
					php_stream_bucket_delref(bucket TSRMLS_CC);
					continue;
				}
			} else {
				memcpy(data->chunksize_buffer_pos, buf, HTTP_CHUNK_SIZE_MAXLEN - (data->chunksize_buffer_pos - data->chunksize_buffer));
				chunk_remaining = strtoul(data->chunksize_buffer, &e, 16);
				if (*e != '\r') {
					/* Invalid chunksize */
					return PSFS_ERR_FATAL;
				}
			}
		}

		data->chunk_remaining = chunk_remaining;
		/* Skip past the chunksize itself */
		buf += (e - data->chunksize_buffer) + sizeof("\r\n") - 1;
		buflen -= (e - data->chunksize_buffer) + sizeof("\r\n") - 1;
		consumed += (e - data->chunksize_buffer) + sizeof("\r\n") - 1;
		memset(data->chunksize_buffer, 0, sizeof(data->chunksize_buffer));
		data->chunksize_buffer_pos = data->chunksize_buffer;

		if (buflen > 0) {
			goto continue_bucket;
		}
		php_stream_bucket_delref(bucket TSRMLS_CC);
	}

	if (bytes_consumed) {
		*bytes_consumed = consumed;
	}

	if (consumed > 0) {
		return PSFS_PASS_ON;
	}

	return PSFS_FEED_ME;	
}

static void php_http_chunked_encoding_dtor(php_stream_filter *thisfilter TSRMLS_DC)
{
	if (thisfilter && thisfilter->abstract) {
		php_http_chunked_encoding_data *data = (php_http_chunked_encoding_data*)thisfilter->abstract;

		pefree(data, data->is_persistent);
		thisfilter->abstract = NULL;
	}
}

static php_stream_filter_ops php_http_chunked_encoding_filter_ops = {
    php_http_chunked_encoding_filter,
    php_http_chunked_encoding_dtor,
    "http.chunked.decode"
};

static php_stream_filter *php_http_chunked_encoding_filter_create(const char *filtername, zval *filterparams, int persistent TSRMLS_DC)
{
	php_http_chunked_encoding_data *data;

	data = pemalloc(sizeof(php_http_chunked_encoding_data), persistent);
	data->chunk_remaining = 0;
	data->is_persistent = persistent;
	memset(data->chunksize_buffer, 0, sizeof(data->chunksize_buffer));
	data->chunksize_buffer_pos = data->chunksize_buffer;

    return php_stream_filter_alloc(&php_http_chunked_encoding_filter_ops, data, persistent);
}

php_stream *php_stream_url_wrap_http_ex(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context, int redirect_max, int header_init STREAMS_DC TSRMLS_DC)
{
	php_stream *stream = NULL;
	php_url *resource = NULL;
	int use_ssl;
	char *scratch = NULL;
	char *tmp = NULL;
	char *ua_str = NULL;
	zval **ua_zval = NULL, **tmpzval = NULL;
	int scratch_len = 0;
	int body = 0;
	char location[HTTP_HEADER_BLOCK_SIZE];
	zval **response_header = NULL;
	int reqok = 0;
	char *http_header_line = NULL;
	char tmp_line[128];
	size_t chunk_size = 0, file_size = 0;
	int eol_detect = 0;
	char *transport_string, *errstr = NULL;
	int transport_len, have_header = 0, request_fulluri = 0;
	char *protocol_version = NULL;
	int protocol_version_len = 3; /* Default: "1.0" */
	int autofilter_chunked_encoding = 0;

	if (redirect_max < 1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Circular redirect, aborting.");
		return NULL;
	}

	resource = php_url_parse(path);
	if (resource == NULL) {
		return NULL;
	}

	if (strncasecmp(resource->scheme, "http", sizeof("http")) && strncasecmp(resource->scheme, "https", sizeof("https"))) {
		if (!context || 
			php_stream_context_get_option(context, wrapper->wops->label, "proxy", &tmpzval) == FAILURE ||
			Z_TYPE_PP(tmpzval) != IS_STRING ||
			Z_STRLEN_PP(tmpzval) <= 0) {
			php_url_free(resource);
			return php_stream_open_wrapper_ex(path, mode, ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL, context);
		}
		/* Called from a non-http wrapper with http proxying requested (i.e. ftp) */
		request_fulluri = 1;
		use_ssl = 0;

		transport_len = Z_STRLEN_PP(tmpzval);
		transport_string = estrndup(Z_STRVAL_PP(tmpzval), Z_STRLEN_PP(tmpzval));
	} else {
		/* Normal http request (possibly with proxy) */
	
		if (strpbrk(mode, "awx+")) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "HTTP wrapper does not support writeable connections.");
			return NULL;
		}

		use_ssl = resource->scheme && (strlen(resource->scheme) > 4) && resource->scheme[4] == 's';
		/* choose default ports */
		if (use_ssl && resource->port == 0)
			resource->port = 443;
		else if (resource->port == 0)
			resource->port = 80;

		if (context && !use_ssl &&
			php_stream_context_get_option(context, wrapper->wops->label, "proxy", &tmpzval) == SUCCESS &&
			Z_TYPE_PP(tmpzval) == IS_STRING &&
			Z_STRLEN_PP(tmpzval) > 0) {
			/* Don't use proxy server for SSL resources */
			transport_len = Z_STRLEN_PP(tmpzval);
			transport_string = estrndup(Z_STRVAL_PP(tmpzval), Z_STRLEN_PP(tmpzval));
		} else {
			transport_len = spprintf(&transport_string, 0, "%s://%s:%d", use_ssl ? "ssl" : "tcp", resource->host, resource->port);
		}
	}

	stream = php_stream_xport_create(transport_string, transport_len, options,
			STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT,
			NULL, NULL, context, &errstr, NULL);
			
	if (errstr) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "%s", errstr);
		efree(errstr);
		errstr = NULL;
	}

	efree(transport_string);

	if (stream == NULL)	
		goto out;

	/* avoid buffering issues while reading header */
	if (options & STREAM_WILL_CAST)
		chunk_size = php_stream_set_chunk_size(stream, 1);
	
	/* avoid problems with auto-detecting when reading the headers -> the headers
	 * are always in canonical \r\n format */
	eol_detect = stream->flags & (PHP_STREAM_FLAG_DETECT_EOL | PHP_STREAM_FLAG_EOL_MAC);
	stream->flags &= ~(PHP_STREAM_FLAG_DETECT_EOL | PHP_STREAM_FLAG_EOL_MAC);

	php_stream_context_set(stream, context);

	php_stream_notify_info(context, PHP_STREAM_NOTIFY_CONNECT, NULL, 0);

	if (context &&
		php_stream_context_get_option(context, "http", "method", &tmpzval) == SUCCESS) {
		if (Z_TYPE_PP(tmpzval) == IS_STRING && Z_STRLEN_PP(tmpzval) > 0) {
			scratch_len = strlen(path) + 29 + Z_STRLEN_PP(tmpzval);
			scratch = emalloc(scratch_len);
			strlcpy(scratch, Z_STRVAL_PP(tmpzval), Z_STRLEN_PP(tmpzval) + 1);
			strcat(scratch, " ");
		}
	}

	if (context &&
		php_stream_context_get_option(context, "http", "protocol_version", &tmpzval) == SUCCESS) {
		SEPARATE_ZVAL(tmpzval);
		convert_to_double_ex(tmpzval);
		protocol_version_len = spprintf(&protocol_version, 0, "%.1f", Z_DVAL_PP(tmpzval));
		zval_ptr_dtor(tmpzval);
	}

	if (!scratch) {
		scratch_len = strlen(path) + 29 + protocol_version_len;
		scratch = emalloc(scratch_len);
		strcpy(scratch, "GET ");
	}

	/* Should we send the entire path in the request line, default to no. */
	if (!request_fulluri &&
		context &&
		php_stream_context_get_option(context, "http", "request_fulluri", &tmpzval) == SUCCESS) {
		(*tmpzval)->refcount++;
		SEPARATE_ZVAL(tmpzval);
		convert_to_boolean_ex(tmpzval);
		request_fulluri = Z_BVAL_PP(tmpzval) ? 1 : 0;
		zval_ptr_dtor(tmpzval);
	}

	if (request_fulluri) {
		/* Ask for everything */
		strcat(scratch, path);
	} else {
		/* Send the traditional /path/to/file?query_string */

		/* file */
		if (resource->path && *resource->path) {
			strlcat(scratch, resource->path, scratch_len);
		} else {
			strlcat(scratch, "/", scratch_len);
		}

		/* query string */
		if (resource->query)	{
			strlcat(scratch, "?", scratch_len);
			strlcat(scratch, resource->query, scratch_len);
		}
	}

	/* protocol version we are speaking */
	if (protocol_version) {
		strlcat(scratch, " HTTP/", scratch_len);
		strlcat(scratch, protocol_version, scratch_len);
		strlcat(scratch, "\r\n", scratch_len);
		efree(protocol_version);
		protocol_version = NULL;
	} else {
		strlcat(scratch, " HTTP/1.0\r\n", scratch_len);
	}


	/* send it */
	php_stream_write(stream, scratch, strlen(scratch));

	if (context &&
		php_stream_context_get_option(context, "http", "header", &tmpzval) == SUCCESS &&
		Z_STRLEN_PP(tmpzval)) {
		/* Remove newlines and spaces from start and end,
		   php_trim will estrndup() */
		tmp = php_trim(Z_STRVAL_PP(tmpzval), Z_STRLEN_PP(tmpzval), NULL, 0, NULL, 3 TSRMLS_CC);
		if (strlen(tmp) > 0) {
			/* Output trimmed headers with \r\n at the end */
			php_stream_write(stream, tmp, strlen(tmp));
			php_stream_write(stream, "\r\n", sizeof("\r\n") - 1);

			/* Make lowercase for easy comparison against 'standard' headers */
			php_strtolower(tmp, strlen(tmp));
			if (strstr(tmp, "user-agent:")) {
				 have_header |= HTTP_HEADER_USER_AGENT;
			}
			if (strstr(tmp, "host:")) {
				 have_header |= HTTP_HEADER_HOST;
			}
			if (strstr(tmp, "from:")) {
				 have_header |= HTTP_HEADER_FROM;
				}
			if (strstr(tmp, "authorization:")) {
				 have_header |= HTTP_HEADER_AUTH;
			}
			if (strstr(tmp, "content-length:")) {
				 have_header |= HTTP_HEADER_CONTENT_LENGTH;
			}
			if (strstr(tmp, "content-type:")) {
				 have_header |= HTTP_HEADER_TYPE;
			}
		}
		efree(tmp);
	}

	/* auth header if it was specified */
	if (((have_header & HTTP_HEADER_AUTH) == 0) && resource->user && resource->pass)	{
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
	if (((have_header & HTTP_HEADER_FROM) == 0) && cfg_get_string("from", &tmp) == SUCCESS)	{
		if (snprintf(scratch, scratch_len, "From: %s\r\n", tmp) > 0)
			php_stream_write(stream, scratch, strlen(scratch));
	}

	/* Send Host: header so name-based virtual hosts work */
	if ((have_header & HTTP_HEADER_HOST) == 0) {
		if ((use_ssl && resource->port != 443 && resource->port != 0) || 
			(!use_ssl && resource->port != 80 && resource->port != 0))	{
			if (snprintf(scratch, scratch_len, "Host: %s:%i\r\n", resource->host, resource->port) > 0)
				php_stream_write(stream, scratch, strlen(scratch));
		} else {
			if (snprintf(scratch, scratch_len, "Host: %s\r\n", resource->host) > 0) {
				php_stream_write(stream, scratch, strlen(scratch));
			}
		}
	}

	if (context && 
	    php_stream_context_get_option(context, "http", "user_agent", &ua_zval) == SUCCESS) {
		ua_str = Z_STRVAL_PP(ua_zval);
	} else if (FG(user_agent)) {
		ua_str = FG(user_agent);
	}

	if (((have_header & HTTP_HEADER_USER_AGENT) == 0) && ua_str) {
#define _UA_HEADER "User-Agent: %s\r\n"
		char *ua;
		size_t ua_len;
		
		ua_len = sizeof(_UA_HEADER) + strlen(ua_str);
		
		/* ensure the header is only sent if user_agent is not blank */
		if (ua_len > sizeof(_UA_HEADER)) {
			ua = emalloc(ua_len + 1);
			if ((ua_len = snprintf(ua, ua_len, _UA_HEADER, ua_str)) > 0) {
				ua[ua_len] = 0;
				php_stream_write(stream, ua, ua_len);
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot construct User-agent header");
			}

			if (ua) {
				efree(ua);
			}
		}	
	}

	/* Request content, such as for POST requests */
	if (context &&
		php_stream_context_get_option(context, "http", "content", &tmpzval) == SUCCESS &&
		Z_STRLEN_PP(tmpzval) > 0) {
		if (!(have_header & HTTP_HEADER_CONTENT_LENGTH)) {
			scratch_len = snprintf(scratch, scratch_len, "Content-Length: %d\r\n", Z_STRLEN_PP(tmpzval));
			php_stream_write(stream, scratch, scratch_len);
		}
		if (!(have_header & HTTP_HEADER_TYPE)) {
			php_stream_write(stream, "Content-Type: application/x-www-form-urlencoded\r\n",
				sizeof("Content-Type: application/x-www-form-urlencoded\r\n") - 1);
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Content-type not specified assuming application/x-www-form-urlencoded");
		}
		php_stream_write(stream, "\r\n", sizeof("\r\n")-1);
		php_stream_write(stream, Z_STRVAL_PP(tmpzval), Z_STRLEN_PP(tmpzval));
		php_stream_write(stream, "\r\n\r\n", sizeof("\r\n\r\n")-1);
	} else {
		php_stream_write(stream, "\r\n", sizeof("\r\n")-1);
	}

	location[0] = '\0';

	if (!header_init && FAILURE == zend_hash_find(EG(active_symbol_table),
				"http_response_header", sizeof("http_response_header"), (void **) &response_header)) {
		header_init = 1;
	}

	if (header_init) {
		zval *tmp;
		MAKE_STD_ZVAL(tmp);
		array_init(tmp);
		ZEND_SET_SYMBOL(EG(active_symbol_table), "http_response_header", tmp);
	
		zend_hash_find(EG(active_symbol_table),
				"http_response_header", sizeof("http_response_header"), (void **) &response_header);
	}


	if (!php_stream_eof(stream)) {
		size_t tmp_line_len;
		/* get response header */

		if (_php_stream_get_line(stream, tmp_line, sizeof(tmp_line) - 1, &tmp_line_len TSRMLS_CC) != NULL) {
			zval *http_response;
			int response_code;

			MAKE_STD_ZVAL(http_response);
			ZVAL_NULL(http_response);

			if (tmp_line_len > 9) {
				response_code = atoi(tmp_line + 9);
			} else {
				response_code = 0;
			}
			switch(response_code) {
				case 200:
				case 302:
				case 301:
					reqok = 1;
					break;
				case 403:
					php_stream_notify_error(context, PHP_STREAM_NOTIFY_AUTH_RESULT,
							tmp_line, response_code);
					break;
				default:
					/* safety net in the event tmp_line == NULL */
					if (!tmp_line_len) {
						tmp_line[0] = '\0';
					}
					php_stream_notify_error(context, PHP_STREAM_NOTIFY_FAILURE,
							tmp_line, response_code);
			}
			
			Z_STRLEN_P(http_response) = tmp_line_len;
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
			zend_hash_next_index_insert(Z_ARRVAL_PP(response_header), &http_response, sizeof(zval *), NULL);
		}
	} else {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "HTTP request failed, unexpected end of socket!");
		goto out;
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
			while(*p) {
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
			} else if ((strncasecmp(http_header_line, "Transfer-Encoding: ", sizeof("Transfer-Encoding: ") - 1) == 0) &&
					   (strstr(http_header_line + sizeof("Transfer-Encoding: ") - 1, "chunked") != NULL)) {
				autofilter_chunked_encoding = 1;
			}

			if (http_header_line[0] == '\0') {
				body = 1;
			} else {
				zval *http_header;

				MAKE_STD_ZVAL(http_header);

				ZVAL_STRINGL(http_header, http_header_line, http_header_line_length, 1);
				
				zend_hash_next_index_insert(Z_ARRVAL_PP(response_header), &http_header, sizeof(zval *), NULL);
			}
		} else {
			break;
		}
	}
	
	if (!reqok || location[0] != '\0')	{		
		if (location[0] != '\0')
			php_stream_notify_info(context, PHP_STREAM_NOTIFY_REDIRECTED, location, 0);

		php_stream_close(stream);
		stream = NULL;

		if (location[0] != '\0')	{

			zval *entry, **entryp;
			char new_path[HTTP_HEADER_BLOCK_SIZE];
			char loc_path[HTTP_HEADER_BLOCK_SIZE];

			*new_path='\0';
			if (strlen(location)<8 || (strncasecmp(location, "http://", sizeof("http://")-1) && 
							strncasecmp(location, "https://", sizeof("https://")-1) && 
							strncasecmp(location, "ftp://", sizeof("ftp://")-1) && 
							strncasecmp(location, "ftps://", sizeof("ftps://")-1))) 
			{
				if (*location != '/') {
					if (*(location+1) != '\0' && resource->path) {		
						char *s = strrchr(resource->path, '/');
						if (!s) {
							s = resource->path;
							if (!s[0]) {
								efree(s);
								s = resource->path = estrdup("/");
							} else {
								*s = '/';
							}
						}
						s[1] = '\0'; 
						if (resource->path && *(resource->path) == '/' && *(resource->path + 1) == '\0') {
							snprintf(loc_path, sizeof(loc_path) - 1, "%s%s", resource->path, location);
						} else {
							snprintf(loc_path, sizeof(loc_path) - 1, "%s/%s", resource->path, location);
						}
					} else {
						snprintf(loc_path, sizeof(loc_path) - 1, "/%s", location);
					}
				} else {
					strlcpy(loc_path, location, sizeof(loc_path));
				}
				if ((use_ssl && resource->port != 443) || (!use_ssl && resource->port != 80)) {
					snprintf(new_path, sizeof(new_path) - 1, "%s://%s:%d%s", resource->scheme, resource->host, resource->port, loc_path);
				} else {
					snprintf(new_path, sizeof(new_path) - 1, "%s://%s%s", resource->scheme, resource->host, loc_path);
				}
			} else {
				strlcpy(new_path, location, sizeof(new_path));
			}
			stream = php_stream_url_wrap_http_ex(wrapper, new_path, mode, options, opened_path, context, --redirect_max, 0 STREAMS_CC TSRMLS_CC);
			if (stream && stream->wrapperdata)	{
				entryp = &entry;
				MAKE_STD_ZVAL(entry);
				ZVAL_EMPTY_STRING(entry);
				zend_hash_next_index_insert(Z_ARRVAL_PP(response_header), entryp, sizeof(zval *), NULL);
				zend_hash_internal_pointer_reset(Z_ARRVAL_P(stream->wrapperdata));
				while (zend_hash_get_current_data(Z_ARRVAL_P(stream->wrapperdata), (void **)&entryp) == SUCCESS) {
					zval_add_ref(entryp);
					zend_hash_next_index_insert(Z_ARRVAL_PP(response_header), entryp, sizeof(zval *), NULL);
					zend_hash_move_forward(Z_ARRVAL_P(stream->wrapperdata));
				}
				zval_dtor(stream->wrapperdata);
				FREE_ZVAL(stream->wrapperdata);
			}
		} else {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "HTTP request failed! %s", tmp_line);
		}
	}
out:
	if (protocol_version) {
		efree(protocol_version);
	}

	if (http_header_line) {
		efree(http_header_line);
	}

	if (scratch) {
		efree(scratch);
	}

	php_url_free(resource);

	if (stream) {
		if (header_init) {
			stream->wrapperdata = *response_header;
			zval_add_ref(response_header);
		}
		php_stream_notify_progress_init(context, 0, file_size);
		/* Restore original chunk size now that we're done with headers */
		if (options & STREAM_WILL_CAST)
			php_stream_set_chunk_size(stream, chunk_size);

		/* restore the users auto-detect-line-endings setting */
		stream->flags |= eol_detect;
		
		/* as far as streams are concerned, we are now at the start of
		 * the stream */
		stream->position = 0;

		if (autofilter_chunked_encoding) {
			php_stream_filter *filter;

			filter = php_http_chunked_encoding_filter_create("http.chunked.decode", NULL, stream->is_persistent TSRMLS_CC);
			if (!filter) {
				/* Never actually happens */
				php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "Unable to apply chunked content decoding");
			} else {
				php_stream_filter_append(&stream->readfilters, filter);
			}
		}

	}

	return stream;
}

php_stream *php_stream_url_wrap_http(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	return php_stream_url_wrap_http_ex(wrapper, path, mode, options, opened_path, context, PHP_URL_REDIRECT_MAX, 1 STREAMS_CC TSRMLS_CC);
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
	NULL, /* stream_close */
	php_stream_http_stream_stat,
	NULL, /* stat_url */
	NULL, /* opendir */
	"http",
	NULL, /* unlink */
	NULL, /* rename */
	NULL, /* mkdir */
	NULL  /* rmdir */
};

PHPAPI php_stream_wrapper php_stream_http_wrapper =	{
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
