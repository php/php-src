/* $Id$ */
#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif
#include "php.h"
#if HAVE_ZIP
#ifdef ZEND_ENGINE_2

#include "lib/zip.h"

#include "php_streams.h"
#include "ext/standard/file.h"
#include "ext/standard/php_string.h"
#include "fopen_wrappers.h"
#include "php_zip.h"

#include "ext/standard/url.h"

struct php_zip_stream_data_t {
	struct zip *za;
	struct zip_file *zf;
	size_t cursor;
	php_stream *stream;
};

#define STREAM_DATA_FROM_STREAM() \
	struct php_zip_stream_data_t *self = (struct php_zip_stream_data_t *) stream->abstract;


/* {{{ php_zip_ops_read */
static size_t php_zip_ops_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	int n = 0;
	STREAM_DATA_FROM_STREAM();

	if (self->za && self->zf) {
		n = (size_t)zip_fread(self->zf, buf, (int)count);

		if (n == 0) {
			stream->eof = 1;
		} else {
			self->cursor += n;
		}
	}
	return n<1 ? 0 : n;
}
/* }}} */

/* {{{ php_zip_ops_write */
static size_t php_zip_ops_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	if (!stream) {
		return 0;
	}

	return count;
}
/* }}} */

/* {{{ php_zip_ops_close */
static int php_zip_ops_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	STREAM_DATA_FROM_STREAM();
	if (close_handle) {
		if (self->za) {
			zip_close(self->za);
			self->za = NULL;
		}
		if (self->zf) {
			zip_fclose(self->zf);
			self->zf = NULL;
		}
	}
	efree(self);
	stream->abstract = NULL;
	return EOF;
}
/* }}} */

/* {{{ php_zip_ops_flush */
static int php_zip_ops_flush(php_stream *stream TSRMLS_DC)
{
	if (!stream) {
		return 0;
	}

	return 0;
}
/* }}} */

php_stream_ops php_stream_zipio_ops = {
	php_zip_ops_write, php_zip_ops_read,
	php_zip_ops_close, php_zip_ops_flush,
	"zip",
	NULL, /* seek */
	NULL, /* cast */
	NULL, /* stat */
	NULL  /* set_option */
};

/* {{{ php_stream_zip_open */
php_stream *php_stream_zip_open(char *filename, char *path, char *mode STREAMS_DC TSRMLS_DC)
{
	struct zip_file *zf = NULL;
	int err = 0;

	php_stream *stream = NULL;
	struct php_zip_stream_data_t *self;
	struct zip *stream_za;

	if (strncmp(mode,"r", strlen("r")) != 0) {
		return NULL;
	}

	if (filename) {
		if (OPENBASEDIR_CHECKPATH(filename)) {
			return NULL;
		}

		/* duplicate to make the stream za independent (esp. for MSHUTDOWN) */
		stream_za = zip_open(filename, ZIP_CREATE, &err);
		if (!stream_za) {
			return NULL;
		}

		zf = zip_fopen(stream_za, path, 0);
		if (zf) {
			self = emalloc(sizeof(*self));

			self->za = stream_za;
			self->zf = zf; 
			self->stream = NULL;
			self->cursor = 0;
			stream = php_stream_alloc(&php_stream_zipio_ops, self, NULL, mode);
		} else {
			zip_close(stream_za);
		}
	}

	if (!stream) {
		return NULL;
	} else {
		return stream;
	}

}
/* }}} */

/* {{{ php_stream_zip_opener */
php_stream *php_stream_zip_opener(php_stream_wrapper *wrapper,
											char *path,
											char *mode,
											int options,
											char **opened_path,
											php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	int path_len;

	char *file_basename;
	size_t file_basename_len;
	char file_dirname[MAXPATHLEN];

	struct zip *za;
	struct zip_file *zf = NULL;
	char *fragment;
	int fragment_len;
	int err;

	php_stream *stream = NULL;
	struct php_zip_stream_data_t *self;

	fragment = strchr(path, '#');
	if (!fragment) {
		return NULL;
	}

	if (strncasecmp("zip://", path, 6) == 0) {
		path += 6;
	}

	fragment_len = strlen(fragment);

	if (fragment_len < 1) {
		return NULL;
	}
	path_len = strlen(path);
	if (path_len >= MAXPATHLEN || mode[0] != 'r') {
		return NULL;
	}

	memcpy(file_dirname, path, path_len - fragment_len);
	file_dirname[path_len - fragment_len] = '\0';

	php_basename(path, path_len - fragment_len, NULL, 0, &file_basename, &file_basename_len TSRMLS_CC);
	fragment++;

	if (OPENBASEDIR_CHECKPATH(file_dirname)) {
		efree(file_basename);
		return NULL;
	}

	za = zip_open(file_dirname, ZIP_CREATE, &err);
	if (za) {
		zf = zip_fopen(za, fragment, 0);
		if (zf) {
			self = emalloc(sizeof(*self));

			self->za = za;
			self->zf = zf; 
			self->stream = NULL;
			self->cursor = 0;
			stream = php_stream_alloc(&php_stream_zipio_ops, self, NULL, mode);

			if (opened_path) {
				*opened_path = estrdup(path);
			}
		} else {
			zip_close(za);
		}
	}

	efree(file_basename);

	if (!stream) {
		return NULL;
	} else {
		return stream;
	}
}
/* }}} */

static php_stream_wrapper_ops zip_stream_wops = {
	php_stream_zip_opener,
	NULL,	/* close */
	NULL,	/* fstat */
	NULL,	/* stat */
	NULL,	/* opendir */
	"zip wrapper",
	NULL,	/* unlink */
	NULL,	/* rename */
	NULL,	/* mkdir */
	NULL	/* rmdir */
};

php_stream_wrapper php_stream_zip_wrapper = {
	&zip_stream_wops,
	NULL,
	0 /* is_url */
};
#endif /* ZEND_ENGINE_2 */
#endif /* HAVE_ZIP */
