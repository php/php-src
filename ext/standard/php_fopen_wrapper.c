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

php_stream * php_stream_url_wrap_php(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	int fd = -1;
	php_stream *stream = NULL;

	if (!strncasecmp(path, "php://", 6))
		path += 6;
	
	if (!strcasecmp(path, "output")) {
		return php_stream_alloc(&php_stream_output_ops, NULL, 0, "wb");
	}
	
	if (!strcasecmp(path, "input")) {
		return php_stream_alloc(&php_stream_input_ops, NULL, 0, "rb");
	}  
	
	if (!strcasecmp(path, "stdin")) {
		fd = STDIN_FILENO;
	} else if (!strcasecmp(path, "stdout")) {
		fd = STDOUT_FILENO;
	} else if (!strcasecmp(path, "stderr")) {
		fd = STDERR_FILENO;
	}

	if (fd != -1) {
		int nfd = dup(fd);

		stream = php_stream_fopen_from_fd(nfd, mode, NULL);

		if (!stream) close(nfd);
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
