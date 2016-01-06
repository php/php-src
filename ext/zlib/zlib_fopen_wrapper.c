/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong <wez@thebrainroom.com>, based on work by:        |
   |         Hartmut Holzgraefe <hholzgra@php.net>                        |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#define _GNU_SOURCE

#include "php.h"
#include "php_zlib.h"
#include "fopen_wrappers.h"

#include "main/php_network.h"

struct php_gz_stream_data_t	{
	gzFile gz_file;
	php_stream *stream;
};

static size_t php_gziop_read(php_stream *stream, char *buf, size_t count)
{
	struct php_gz_stream_data_t *self = (struct php_gz_stream_data_t *) stream->abstract;
	int read;

	/* XXX this needs to be looped for the case count > UINT_MAX */
	read = gzread(self->gz_file, buf, count);

	if (gzeof(self->gz_file)) {
		stream->eof = 1;
	}

	return (size_t)((read < 0) ? 0 : read);
}

static size_t php_gziop_write(php_stream *stream, const char *buf, size_t count)
{
	struct php_gz_stream_data_t *self = (struct php_gz_stream_data_t *) stream->abstract;
	int wrote;

	/* XXX this needs to be looped for the case count > UINT_MAX */
	wrote = gzwrite(self->gz_file, (char *) buf, count);

	return (size_t)((wrote < 0) ? 0 : wrote);
}

static int php_gziop_seek(php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffs)
{
	struct php_gz_stream_data_t *self = (struct php_gz_stream_data_t *) stream->abstract;

	assert(self != NULL);

	if (whence == SEEK_END) {
		php_error_docref(NULL, E_WARNING, "SEEK_END is not supported");
		return -1;
	}
	*newoffs = gzseek(self->gz_file, offset, whence);

	return (*newoffs < 0) ? -1 : 0;
}

static int php_gziop_close(php_stream *stream, int close_handle)
{
	struct php_gz_stream_data_t *self = (struct php_gz_stream_data_t *) stream->abstract;
	int ret = EOF;

	if (close_handle) {
		if (self->gz_file) {
			ret = gzclose(self->gz_file);
			self->gz_file = NULL;
		}
		if (self->stream) {
			php_stream_close(self->stream);
			self->stream = NULL;
		}
	}
	efree(self);

	return ret;
}

static int php_gziop_flush(php_stream *stream)
{
	struct php_gz_stream_data_t *self = (struct php_gz_stream_data_t *) stream->abstract;

	return gzflush(self->gz_file, Z_SYNC_FLUSH);
}

php_stream_ops php_stream_gzio_ops = {
	php_gziop_write, php_gziop_read,
	php_gziop_close, php_gziop_flush,
	"ZLIB",
	php_gziop_seek,
	NULL, /* cast */
	NULL, /* stat */
	NULL  /* set_option */
};

php_stream *php_stream_gzopen(php_stream_wrapper *wrapper, const char *path, const char *mode, int options,
							  zend_string **opened_path, php_stream_context *context STREAMS_DC)
{
	struct php_gz_stream_data_t *self;
	php_stream *stream = NULL, *innerstream = NULL;

	/* sanity check the stream: it can be either read-only or write-only */
	if (strchr(mode, '+')) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL, E_WARNING, "cannot open a zlib stream for reading and writing at the same time!");
		}
		return NULL;
	}

	if (strncasecmp("compress.zlib://", path, 16) == 0) {
		path += 16;
	} else if (strncasecmp("zlib:", path, 5) == 0) {
		path += 5;
	}

	innerstream = php_stream_open_wrapper_ex(path, mode, STREAM_MUST_SEEK | options | STREAM_WILL_CAST, opened_path, context);

	if (innerstream) {
		php_socket_t fd;

		if (SUCCESS == php_stream_cast(innerstream, PHP_STREAM_AS_FD, (void **) &fd, REPORT_ERRORS)) {
			self = emalloc(sizeof(*self));
			self->stream = innerstream;
			self->gz_file = gzdopen(dup(fd), mode);

			if (self->gz_file) {
				stream = php_stream_alloc_rel(&php_stream_gzio_ops, self, 0, mode);
				if (stream) {
					stream->flags |= PHP_STREAM_FLAG_NO_BUFFER;
					return stream;
				}

				gzclose(self->gz_file);
			}

			efree(self);
			if (options & REPORT_ERRORS) {
				php_error_docref(NULL, E_WARNING, "gzopen failed");
			}
		}

		php_stream_close(innerstream);
	}

	return NULL;
}

static php_stream_wrapper_ops gzip_stream_wops = {
	php_stream_gzopen,
	NULL, /* close */
	NULL, /* stat */
	NULL, /* stat_url */
	NULL, /* opendir */
	"ZLIB",
	NULL, /* unlink */
	NULL, /* rename */
	NULL, /* mkdir */
	NULL  /* rmdir */
};

php_stream_wrapper php_stream_gzip_wrapper =	{
	&gzip_stream_wops,
	NULL,
	0, /* is_url */
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
