/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
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
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdio.h>
#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "php.h"
#include "php_globals.h"
#include "php_standard.h"
#include "php_fopen_wrappers.h"
#include "SAPI.h"

static size_t php_stream_output_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	PHPWRITE(buf, count);
	return count;
}

static size_t php_stream_output_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	stream->eof = 1;
	return 0;
}

static int php_stream_output_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	return 0;
}

static int php_stream_output_flush(php_stream *stream TSRMLS_DC)
{
	sapi_flush(TSRMLS_C);
	return 0;
}

php_stream_ops php_stream_output_ops = {
	php_stream_output_write,
	php_stream_output_read,
	php_stream_output_close,
	php_stream_output_flush,
	"Output",
	NULL, /* seek */
	NULL, /* cast */
	NULL, /* stat */
	NULL  /* set_option */
};

static size_t php_stream_input_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	return -1;
}

static size_t php_stream_input_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	size_t read_bytes = 0;
	if(!stream->eof) {
		if(SG(request_info).raw_post_data) { /* data has already been read by a post handler */
			read_bytes = SG(request_info).raw_post_data_length - stream->position;
			if(read_bytes <= count) {
				stream->eof = 1;
			} else {
				read_bytes = count;
			}
			if(read_bytes) {
				memcpy(buf, SG(request_info).raw_post_data + stream->position, read_bytes);
			}
		} else if(sapi_module.read_post) {
			read_bytes = sapi_module.read_post(buf, count TSRMLS_CC);
			if(read_bytes <= 0){
				stream->eof = 1;
				read_bytes = 0;
			}
		} else {
			stream->eof = 1;
		}
	}

	SG(read_post_bytes) += read_bytes;
    return read_bytes;
}

static int php_stream_input_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	return 0;
}

static int php_stream_input_flush(php_stream *stream TSRMLS_DC)
{
	return -1;
}

php_stream_ops php_stream_input_ops = {
	php_stream_input_write,
	php_stream_input_read,
	php_stream_input_close,
	php_stream_input_flush,
	"Input",
	NULL, /* seek */
	NULL, /* cast */
	NULL, /* stat */
	NULL  /* set_option */
};

static void php_stream_apply_filter_list(php_stream *stream, char *filterlist, int read_chain, int write_chain TSRMLS_DC) {
	char *p, *token;
	php_stream_filter *temp_filter;

	p = php_strtok_r(filterlist, "|", &token);
	while (p) {
		if (read_chain) {
			if (temp_filter = php_stream_filter_create(p, "", 0, php_stream_is_persistent(stream) TSRMLS_CC)) {
				php_stream_filter_append(&stream->readfilters, temp_filter);
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create filter (%s)\n", p);
			}
		}
		if (write_chain) {
			if (temp_filter = php_stream_filter_create(p, "", 0, php_stream_is_persistent(stream) TSRMLS_CC)) {
				php_stream_filter_append(&stream->writefilters, temp_filter);
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create filter (%s)\n", p);
			}
		}
		p = php_strtok_r(NULL, "|", &token);
	}
}


php_stream * php_stream_url_wrap_php(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	int fd = -1;
	php_stream * stream = NULL;
	char *p, *token, *pathdup;

	if (!strncasecmp(path, "php://", 6))
		path += 6;
	
	if (!strcasecmp(path, "output")) {
		return php_stream_alloc(&php_stream_output_ops, NULL, 0, "wb");
	}
	
	if (!strcasecmp(path, "input")) {
		return php_stream_alloc(&php_stream_input_ops, NULL, 0, "rb");
	}  
	
	if (!strcasecmp(path, "stdin")) {
		fd = dup(STDIN_FILENO);
	} else if (!strcasecmp(path, "stdout")) {
		fd = dup(STDOUT_FILENO);
	} else if (!strcasecmp(path, "stderr")) {
		fd = dup(STDERR_FILENO);
	}

	if (fd)	{
		stream = php_stream_fopen_from_fd(fd, mode);
		if (stream == NULL)
			close(fd);
	}

	if (!strncasecmp(path, "filter/", 7)) {
		pathdup = estrndup(path + 6, strlen(path + 6));
		p = strstr(pathdup, "/resource=");
		if (!p) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "No URL resource specified.");
			efree(pathdup);
			return NULL;
		}
		if (!(stream = php_stream_open_wrapper(p + 10, mode, options, opened_path))) {
			efree(pathdup);
			return NULL;
		}

		*p = '\0';

		p = php_strtok_r(pathdup + 1, "/", &token);
		while (p) {
			if (!strncasecmp(p, "read=", 5)) {
				php_stream_apply_filter_list(stream, p + 5, 1, 0 TSRMLS_CC);
			} else if (!strncasecmp(p, "write=", 6)) {
				php_stream_apply_filter_list(stream, p + 6, 0, 1 TSRMLS_CC);
			} else {
				php_stream_apply_filter_list(stream, p, 1, 1 TSRMLS_CC);
			}
			p = php_strtok_r(NULL, "/", &token);
		}
		efree(pathdup);
 	}
 
	return stream;
}

static php_stream_wrapper_ops php_stdio_wops = {
	php_stream_url_wrap_php,
	NULL, /* close */
	NULL, /* fstat */
	NULL, /* stat */
	NULL, /* opendir */
	"PHP"
};

php_stream_wrapper php_stream_php_wrapper =	{
	&php_stdio_wops,
	NULL,
	0, /* is_url */
};


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
