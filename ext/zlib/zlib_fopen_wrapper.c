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
   | Author: Wez Furlong <wez@thebrainroom.com>, based on work by:        |
   |         Hartmut Holzgraefe <hartmut@six.de>                          |
   +----------------------------------------------------------------------+
 */
/* $Id$ */
#define IS_EXT_MODULE
#define _GNU_SOURCE

#include "php.h"
#include "php_zlib.h"
#include "fopen_wrappers.h"

struct php_gz_stream_data_t	{
	gzFile gz_file;
	php_stream *stream;
};

static size_t php_gziop_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	struct php_gz_stream_data_t *self = (struct php_gz_stream_data_t *)stream->abstract;

	if (buf == NULL && count == 0) {
		if (gzeof(self->gz_file))
			return EOF;
		return 0;
	}
	
	return gzread(self->gz_file, buf, count); 
}

static char *php_gziop_gets(php_stream *stream, char *buf, size_t size TSRMLS_DC)
{
	struct php_gz_stream_data_t *self = (struct php_gz_stream_data_t *)stream->abstract;
	return gzgets(self->gz_file, buf, size);
}


static size_t php_gziop_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	struct php_gz_stream_data_t *self = (struct php_gz_stream_data_t *)stream->abstract;
	return gzwrite(self->gz_file, (char*)buf, count); 
}

static int php_gziop_seek(php_stream *stream, off_t offset, int whence TSRMLS_DC)
{
	struct php_gz_stream_data_t *self = (struct php_gz_stream_data_t *)stream->abstract;

	assert(self != NULL);
	if (offset == 0 && whence == SEEK_CUR)
		return gztell(self->gz_file);
	
	return gzseek(self->gz_file, offset, whence);
}

static int php_gziop_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	struct php_gz_stream_data_t *self = (struct php_gz_stream_data_t *)stream->abstract;
	int ret = EOF;
	
	if (close_handle)
		ret = gzclose(self->gz_file);
	php_stream_free(self->stream, PHP_STREAM_FREE_CLOSE | (close_handle == 0 ? PHP_STREAM_FREE_PRESERVE_HANDLE : 0));
	efree(self);

	return ret;
}
static int php_gziop_flush(php_stream *stream TSRMLS_DC)
{
	struct php_gz_stream_data_t *self = (struct php_gz_stream_data_t *)stream->abstract;
	return gzflush(self->gz_file, Z_SYNC_FLUSH);
}


php_stream_ops php_stream_gzio_ops = {
	php_gziop_write, php_gziop_read,
	php_gziop_close, php_gziop_flush,
	php_gziop_seek, php_gziop_gets,
	NULL, "ZLIB"
};

php_stream *php_stream_gzopen(char *path, char *mode, int options, char **opened_path, void *wrappercontext STREAMS_DC TSRMLS_DC)
{
	struct php_gz_stream_data_t *self;
	php_stream *stream = NULL;
	
	self = emalloc(sizeof(*self));

	if (strncasecmp("zlib://", path, 7) == 0)
		path += 7;
	
	self->stream = php_stream_open_wrapper(path, mode, STREAM_MUST_SEEK|options, opened_path);
	
	if (self->stream) {
		int fd;
		if (SUCCESS == php_stream_cast(self->stream, PHP_STREAM_AS_FD, (void**)&fd, REPORT_ERRORS)) {
			self->gz_file = gzdopen(fd, mode);
			if (self->gz_file)	{
				stream = php_stream_alloc_rel(&php_stream_gzio_ops, self, 0, mode);
				if (stream)
					return stream;
				gzclose(self->gz_file);
			}
		}
	}
	if (stream)
		php_stream_close(stream);
	if (self && self->stream)
		php_stream_close(self->stream);
	if (self)
		efree(self);

	return NULL;
}

php_stream_wrapper php_stream_gzip_wrapper =	{
	php_stream_gzopen,
	NULL,
	NULL
};




/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
