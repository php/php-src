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
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* This file implements cURL based wrappers.
 * NOTE: If you are implementing your own streams that are intended to
 * work independently of wrappers, this is not a good example to follow!
 **/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_memory_streams.h"

#if HAVE_CURL

#include <stdio.h>
#include <string.h>

#ifdef PHP_WIN32
#include <winsock.h>
#include <sys/types.h>
#endif

#include <curl/curl.h>
#include <curl/easy.h>

#define SMART_STR_PREALLOC 4096

#include "ext/standard/php_smart_str.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "php_curl.h"

static size_t on_data_available(char *data, size_t size, size_t nmemb, void *ctx)
{
	php_stream *stream = (php_stream*)ctx;
	php_curl_stream *curlstream = (php_curl_stream*)stream->abstract;
	size_t wrote;
	TSRMLS_FETCH();

	/* TODO: I'd like to deprecate this.
	 * This code is here because until we start getting real data, we don't know
	 * if we have had all of the headers 
	 * */
	if (curlstream->readbuffer.writepos == 0) {
		zval *sym;

		MAKE_STD_ZVAL(sym);
		*sym = *curlstream->headers;
		zval_copy_ctor(sym);
		ZEND_SET_SYMBOL(EG(active_symbol_table), "http_response_header", sym);
	}
	
	php_stream_seek(curlstream->readbuffer.buf, curlstream->readbuffer.writepos, SEEK_SET);
	wrote = php_stream_write(curlstream->readbuffer.buf, data, size * nmemb);
	curlstream->readbuffer.writepos = php_stream_tell(curlstream->readbuffer.buf);

	return wrote;
}

/* cURL guarantees that headers are written as complete lines, with this function
 * called once for each header */
static size_t on_header_available(char *data, size_t size, size_t nmemb, void *ctx)
{
	size_t length = size * nmemb;
	zval *header;
	php_stream *stream = (php_stream*)ctx;
	php_curl_stream *curlstream = (php_curl_stream*)stream->abstract;
	TSRMLS_FETCH();

	MAKE_STD_ZVAL(header);
	Z_STRLEN_P(header) = length;
	Z_STRVAL_P(header) = estrndup(data, length);
	if (Z_STRVAL_P(header)[length-1] == '\n') {
		Z_STRVAL_P(header)[length-1] = '\0';
		Z_STRLEN_P(header)--;
		
		if (Z_STRVAL_P(header)[length-2] == '\r') {
			Z_STRVAL_P(header)[length-2] = '\0';
			Z_STRLEN_P(header)--;
		}
	}
	Z_TYPE_P(header) = IS_STRING;
	zend_hash_next_index_insert(Z_ARRVAL_P(curlstream->headers), &header, sizeof(zval *), NULL);

	/* based on the header, we might need to trigger a notification */
	if (!strncasecmp(data, "Location: ", 10)) {
		php_stream_notify_info(stream->context, PHP_STREAM_NOTIFY_REDIRECTED, data + 10, 0);
	} else if (!strncasecmp(data, "Content-Type: ", 14)) {
		php_stream_notify_info(stream->context, PHP_STREAM_NOTIFY_MIME_TYPE_IS, data + 14, 0);
	} else if (!strncasecmp(data, "Context-Length: ", 16)) {
		php_stream_notify_file_size(stream->context, atoi(data + 16), data, 0);
		php_stream_notify_progress_init(stream->context, 0, 0);
	}
	
	return length;
	
}

static int on_progress_avail(php_stream *stream, double dltotal, double dlnow, double ultotal, double ulnow)
{
	TSRMLS_FETCH();
	/* our notification system only works in a single direction; we should detect which
	 * direction is important and use the correct values in this call */
	php_stream_notify_progress(stream->context, dlnow, dltotal);
	return 0;
}

static size_t php_curl_stream_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	php_curl_stream *curlstream = (php_curl_stream*)stream->abstract;

	if (curlstream->writebuffer.buf) {
		return php_stream_write(curlstream->writebuffer.buf, buf, count);
	}
	
	return 0;
}

static size_t php_curl_stream_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	php_curl_stream *curlstream = (php_curl_stream*)stream->abstract;
	size_t didread = 0;
	
	if (curlstream->readbuffer.readpos >= curlstream->readbuffer.writepos && curlstream->pending) {
		/* we need to read some more data */
		struct timeval tv;

		/* fire up the connection */
		if (curlstream->readbuffer.writepos == 0) {
			while (CURLM_CALL_MULTI_PERFORM == curl_multi_perform(curlstream->multi, &curlstream->pending))
				;
		}
		
		do {
			/* get the descriptors from curl */
			curl_multi_fdset(curlstream->multi, &curlstream->readfds,
					&curlstream->writefds, &curlstream->excfds, &curlstream->maxfd);

			/* if we are in blocking mode, set a timeout */
			tv.tv_usec = 0;
			tv.tv_sec = 15; /* TODO: allow this to be configured from the script */

			/* wait for data */
			switch (select(curlstream->maxfd+1, &curlstream->readfds,
						&curlstream->writefds, &curlstream->excfds, &tv)) {
				case -1:
					/* error */
					return 0;
				case 0:
					/* no data yet: timed-out */
					return 0;
				default:
					/* fetch the data */
					do {
						curlstream->mcode = curl_multi_perform(curlstream->multi, &curlstream->pending);
					} while (curlstream->mcode == CURLM_CALL_MULTI_PERFORM);
			}
		} while(curlstream->readbuffer.readpos >= curlstream->readbuffer.writepos && curlstream->pending > 0);

	}

	/* if there is data in the buffer, try and read it */
	if (curlstream->readbuffer.writepos > 0 && curlstream->readbuffer.readpos < curlstream->readbuffer.writepos) {
		php_stream_seek(curlstream->readbuffer.buf, curlstream->readbuffer.readpos, SEEK_SET);
		didread = php_stream_read(curlstream->readbuffer.buf, buf, count);
		curlstream->readbuffer.readpos = php_stream_tell(curlstream->readbuffer.buf);

	}

	if (didread == 0) {
		stream->eof = 1;
	}
	
	return didread;
}

static int php_curl_stream_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	php_curl_stream *curlstream = (php_curl_stream*)stream->abstract;

	/* TODO: respect the close_handle flag here, so that casting to a FILE* on
	 * systems without fopencookie will work properly */
	
	curl_multi_remove_handle(curlstream->multi, curlstream->curl);
	curl_easy_cleanup(curlstream->curl);	
	curl_multi_cleanup(curlstream->multi);	

	/* we are not closing curlstream->readbuf here, because we export
	 * it as a zval with the wrapperdata - the engine will garbage collect it */

	efree(curlstream->url);
	efree(curlstream);
	
	return 0;
}

static int php_curl_stream_flush(php_stream *stream TSRMLS_DC)
{
	php_curl_stream *curlstream = (php_curl_stream*)stream->abstract;
	return 0;
}

static int php_curl_stream_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC)
{
	/* TODO: fill in details based on Data: and Content-Length: headers, and/or data
	 * from curl_easy_getinfo().
	 * For now, return -1 to indicate that it doesn't make sense to stat this stream */
	return -1;
}

static int php_curl_stream_cast(php_stream *stream, int castas, void **ret TSRMLS_DC)
{
	php_curl_stream *curlstream = (php_curl_stream*)stream->abstract;
	/* delegate to the readbuffer stream */
	return php_stream_cast(curlstream->readbuffer.buf, castas, ret, 0);
}

PHPAPI php_stream_ops php_curl_stream_ops = {
	php_curl_stream_write,
	php_curl_stream_read,
	php_curl_stream_close,
	php_curl_stream_flush,
	"cURL",
	NULL, /* seek */
	php_curl_stream_cast, /* cast */
	php_curl_stream_stat  /* stat */
};


PHPAPI php_stream *php_curl_stream_opener(php_stream_wrapper *wrapper, char *filename, char *mode,
		int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	php_stream *stream;
	php_curl_stream *curlstream;
	zval *tmp;

	curlstream = emalloc(sizeof(php_curl_stream));
	memset(curlstream, 0, sizeof(php_curl_stream));

	stream = php_stream_alloc(&php_curl_stream_ops, curlstream, 0, mode);
	php_stream_context_set(stream, context);

	curlstream->curl = curl_easy_init();
	curlstream->multi = curl_multi_init();
	curlstream->pending = 1;

	/* if opening for an include statement, ensure that the local storage will
	 * have a FILE* associated with it.
	 * Otherwise, use the "smart" memory stream that will turn itself into a file
	 * when it gets large */
#if !HAVE_FOPENCOOKIE
	if (options & STREAM_WILL_CAST) {
		curlstream->readbuffer.buf = php_stream_fopen_tmpfile();
	} else
#endif
	{
		curlstream->readbuffer.buf = php_stream_temp_new();
	}
	
	/* curl requires the URL to be valid throughout it's operation, so dup it */
	curlstream->url = estrdup(filename);
	curl_easy_setopt(curlstream->curl, CURLOPT_URL, curlstream->url);

	/* feed curl data into our read buffer */	
	curl_easy_setopt(curlstream->curl, CURLOPT_WRITEFUNCTION, on_data_available);
	curl_easy_setopt(curlstream->curl, CURLOPT_FILE, stream);
	
	/* feed headers */
	curl_easy_setopt(curlstream->curl, CURLOPT_HEADERFUNCTION, on_header_available);
	curl_easy_setopt(curlstream->curl, CURLOPT_WRITEHEADER, stream);

	/* currently buggy (bug is in curl) */
	curl_easy_setopt(curlstream->curl, CURLOPT_FOLLOWLOCATION, 1);
	
	curl_easy_setopt(curlstream->curl, CURLOPT_ERRORBUFFER, curlstream->errstr);
	curl_easy_setopt(curlstream->curl, CURLOPT_VERBOSE, 0);

	/* enable progress notification */
	curl_easy_setopt(curlstream->curl, CURLOPT_PROGRESSFUNCTION, on_progress_avail);
	curl_easy_setopt(curlstream->curl, CURLOPT_PROGRESSDATA, stream);
	curl_easy_setopt(curlstream->curl, CURLOPT_NOPROGRESS, 0);

	curl_easy_setopt(curlstream->curl, CURLOPT_USERAGENT, "PHP/" PHP_VERSION);
	
	/* TODO: read cookies and options from context */

	/* prepare for "pull" mode */
	curl_multi_add_handle(curlstream->multi, curlstream->curl);

	/* Prepare stuff for file_get_wrapper_data: the data is an array:
	 *
	 * data = array(
	 *   "headers" => array("Content-Type: text/html", "Xxx: Yyy"),
	 *   "readbuf" => resource (equivalent to curlstream->readbuffer)
	 * );
	 * */
	MAKE_STD_ZVAL(stream->wrapperdata);
	array_init(stream->wrapperdata);
	
	MAKE_STD_ZVAL(curlstream->headers);
	array_init(curlstream->headers);
	
	add_assoc_zval(stream->wrapperdata, "headers", curlstream->headers);
	
	MAKE_STD_ZVAL(tmp);
	php_stream_to_zval(curlstream->readbuffer.buf, tmp);
	add_assoc_zval(stream->wrapperdata, "readbuf", tmp);

#if !HAVE_FOPENCOOKIE
	if (options & STREAM_WILL_CAST) {
		/* we will need to download the whole resource now,
		 * since we cannot get the actual FD for the download,
		 * so we won't be able to drive curl via stdio. */

/* TODO: this needs finishing */
		
		curl_easy_perform(curlstream->curl);
	}
	else
#endif
	{
		/* fire up the connection; we need to detect a connection error here,
		 * otherwise the curlstream we return ends up doing nothing useful. */
		CURLMcode m;

		while (CURLM_CALL_MULTI_PERFORM == 
				(m = curl_multi_perform(curlstream->multi, &curlstream->pending))
			  ) {
			; /* spin */
		}

		if (m != CURLM_OK) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "There was an error mcode=%d", m);
		}

	}
	
	return stream;
}

static php_stream_wrapper_ops php_curl_wrapper_ops = {
	php_curl_stream_opener,
	NULL, /* stream_close: curl streams know how to clean themselves up */
	NULL, /* stream_stat: curl streams know how to stat themselves */
	NULL, /* stat url */
	NULL  /* opendir */
};

php_stream_wrapper php_curl_wrapper = {
	&php_curl_wrapper_ops,
	NULL,
	1 /* is_url */
};


#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
