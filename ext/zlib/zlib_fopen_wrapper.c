/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong <wez@thebrainroom.com>, based on work by:        |
   |         Hartmut Holzgraefe <hholzgra@php.net>                        |
   +----------------------------------------------------------------------+
 */

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include "php.h"
#include "php_zlib.h"
#include "fopen_wrappers.h"

#include "main/php_network.h"

struct php_gz_stream_data_t	{
	gzFile gz_file;
	php_stream *stream;
};

static void php_gziop_report_errors(php_stream *stream, size_t count, const char *verb)
{
	if (!(stream->flags & PHP_STREAM_FLAG_SUPPRESS_ERRORS)) {
		struct php_gz_stream_data_t *self = stream->abstract;
		int error = 0;
		gzerror(self->gz_file, &error);
		if (error == Z_ERRNO) {
			php_error_docref(NULL, E_NOTICE, "%s of %zu bytes failed with errno=%d %s", verb, count, errno, strerror(errno));
		}
	}
}

static ssize_t php_gziop_read(php_stream *stream, char *buf, size_t count)
{
	struct php_gz_stream_data_t *self = (struct php_gz_stream_data_t *) stream->abstract;
	ssize_t total_read = 0;

	/* Despite the count argument of gzread() being "unsigned int",
	 * the return value is "int". Error returns are values < 0, otherwise the count is returned.
	 * To properly distinguish error values from success value, we therefore need to cap at INT_MAX.
	 */
	do {
		unsigned int chunk_size = MIN(count, INT_MAX);
		int read = gzread(self->gz_file, buf, chunk_size);
		count -= chunk_size;

		if (gzeof(self->gz_file)) {
			stream->eof = 1;
		}

		if (UNEXPECTED(read < 0)) {
			php_gziop_report_errors(stream, chunk_size, "Read");
			return read;
		}

		total_read += read;
		buf += read;
	} while (count > 0 && !stream->eof);

	return total_read;
}

static ssize_t php_gziop_write(php_stream *stream, const char *buf, size_t count)
{
	struct php_gz_stream_data_t *self = (struct php_gz_stream_data_t *) stream->abstract;
	ssize_t total_written = 0;

	/* Despite the count argument of gzread() being "unsigned int",
	 * the return value is "int". Error returns are values < 0, otherwise the count is returned.
	 * To properly distinguish error values from success value, we therefore need to cap at INT_MAX.
	 */
	do {
		unsigned int chunk_size = MIN(count, INT_MAX);
		int written = gzwrite(self->gz_file, buf, chunk_size);
		count -= chunk_size;

		if (UNEXPECTED(written < 0)) {
			php_gziop_report_errors(stream, chunk_size, "Write");
			return written;
		}

		total_written += written;
		buf += written;
	} while (count > 0);

    return total_written;
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

static int php_gziop_set_option(php_stream *stream, int option, int value, void *ptrparam)
{
	struct php_gz_stream_data_t *self = stream->abstract;

	switch (option) {
		case PHP_STREAM_OPTION_LOCKING:
		case PHP_STREAM_OPTION_META_DATA_API:
			return self->stream->ops->set_option(self->stream, option, value, ptrparam);
		default:
			break;
	}

	return PHP_STREAM_OPTION_RETURN_NOTIMPL;
}

const php_stream_ops php_stream_gzio_ops = {
	php_gziop_write, php_gziop_read,
	php_gziop_close, php_gziop_flush,
	"ZLIB",
	php_gziop_seek,
	NULL, /* cast */
	NULL, /* stat */
	php_gziop_set_option  /* set_option */
};

php_stream *php_stream_gzopen(php_stream_wrapper *wrapper, const char *path, const char *mode, int options,
							  zend_string **opened_path, php_stream_context *context STREAMS_DC)
{
	struct php_gz_stream_data_t *self;
	php_stream *stream = NULL, *innerstream = NULL;

	/* sanity check the stream: it can be either read-only or write-only */
	if (strchr(mode, '+')) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL, E_WARNING, "Cannot open a zlib stream for reading and writing at the same time!");
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
				zval *zlevel = context ? php_stream_context_get_option(context, "zlib", "level") : NULL;
				if (zlevel && (Z_OK != gzsetparams(self->gz_file, zval_get_long(zlevel), Z_DEFAULT_STRATEGY))) {
					php_error(E_WARNING, "failed setting compression level");
				}

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

static const php_stream_wrapper_ops gzip_stream_wops = {
	php_stream_gzopen,
	NULL, /* close */
	NULL, /* stat */
	NULL, /* stat_url */
	NULL, /* opendir */
	"ZLIB",
	NULL, /* unlink */
	NULL, /* rename */
	NULL, /* mkdir */
	NULL, /* rmdir */
	NULL
};

const php_stream_wrapper php_stream_gzip_wrapper =	{
	&gzip_stream_wops,
	NULL,
	0, /* is_url */
};
