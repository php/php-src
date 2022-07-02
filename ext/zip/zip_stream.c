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
  | Author: Piere-Alain Joye <pierre@php.net>                            |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif
#include "php.h"
#ifdef HAVE_ZIP

#include "php_streams.h"
#include "ext/standard/file.h"
#include "ext/standard/php_string.h"
#include "fopen_wrappers.h"
#include "php_zip.h"

#include "ext/standard/url.h"

/* needed for ssize_t definition */
#include <sys/types.h>

struct php_zip_stream_data_t {
	struct zip *za;
	struct zip_file *zf;
	size_t cursor;
	php_stream *stream;
};

#define STREAM_DATA_FROM_STREAM() \
	struct php_zip_stream_data_t *self = (struct php_zip_stream_data_t *) stream->abstract;


/* {{{ php_zip_ops_read */
static ssize_t php_zip_ops_read(php_stream *stream, char *buf, size_t count)
{
	ssize_t n = 0;
	STREAM_DATA_FROM_STREAM();

	if (self->zf) {
		n = zip_fread(self->zf, buf, count);
		if (n < 0) {
#if LIBZIP_VERSION_MAJOR < 1
			int ze, se;
			zip_file_error_get(self->zf, &ze, &se);
			stream->eof = 1;
			php_error_docref(NULL, E_WARNING, "Zip stream error: %s", zip_file_strerror(self->zf));
#else
			zip_error_t *err;
			err = zip_file_get_error(self->zf);
			stream->eof = 1;
			php_error_docref(NULL, E_WARNING, "Zip stream error: %s", zip_error_strerror(err));
			zip_error_fini(err);
#endif
			return -1;
		}
		/* cast count to signed value to avoid possibly negative n
		 * being cast to unsigned value */
		if (n == 0 || n < (ssize_t)count) {
			stream->eof = 1;
		} else {
			self->cursor += n;
		}
	}
	return n;
}
/* }}} */

/* {{{ php_zip_ops_write */
static ssize_t php_zip_ops_write(php_stream *stream, const char *buf, size_t count)
{
	if (!stream) {
		return -1;
	}

	return count;
}
/* }}} */

/* {{{ php_zip_ops_close */
static int php_zip_ops_close(php_stream *stream, int close_handle)
{
	STREAM_DATA_FROM_STREAM();
	if (close_handle) {
		if (self->zf) {
			zip_fclose(self->zf);
			self->zf = NULL;
		}

		if (self->za) {
			zip_close(self->za);
			self->za = NULL;
		}
	}
	efree(self);
	stream->abstract = NULL;
	return EOF;
}
/* }}} */

/* {{{ php_zip_ops_flush */
static int php_zip_ops_flush(php_stream *stream)
{
	if (!stream) {
		return 0;
	}

	return 0;
}
/* }}} */

static int php_zip_ops_stat(php_stream *stream, php_stream_statbuf *ssb) /* {{{ */
{
	struct zip_stat sb;
	const char *path = stream->orig_path;
	size_t path_len = strlen(stream->orig_path);
	char file_dirname[MAXPATHLEN];
	struct zip *za;
	char *fragment;
	size_t fragment_len;
	int err;
	zend_string *file_basename;

	fragment = strchr(path, '#');
	if (!fragment) {
		return -1;
	}


	if (strncasecmp("zip://", path, 6) == 0) {
		path += 6;
	}

	fragment_len = strlen(fragment);

	if (fragment_len < 1) {
		return -1;
	}
	path_len = strlen(path);
	if (path_len >= MAXPATHLEN) {
		return -1;
	}

	memcpy(file_dirname, path, path_len - fragment_len);
	file_dirname[path_len - fragment_len] = '\0';

	file_basename = php_basename((char *)path, path_len - fragment_len, NULL, 0);
	fragment++;

	if (ZIP_OPENBASEDIR_CHECKPATH(file_dirname)) {
		zend_string_release_ex(file_basename, 0);
		return -1;
	}

	za = zip_open(file_dirname, ZIP_CREATE, &err);
	if (za) {
		memset(ssb, 0, sizeof(php_stream_statbuf));
		if (zip_stat(za, fragment, ZIP_FL_NOCASE, &sb) != 0) {
			zip_close(za);
			zend_string_release_ex(file_basename, 0);
			return -1;
		}
		zip_close(za);

		if (path[path_len-1] != '/') {
			ssb->sb.st_size = sb.size;
			ssb->sb.st_mode |= S_IFREG; /* regular file */
		} else {
			ssb->sb.st_size = 0;
			ssb->sb.st_mode |= S_IFDIR; /* regular directory */
		}

		ssb->sb.st_mtime = sb.mtime;
		ssb->sb.st_atime = sb.mtime;
		ssb->sb.st_ctime = sb.mtime;
		ssb->sb.st_nlink = 1;
		ssb->sb.st_rdev = -1;
#ifndef PHP_WIN32
		ssb->sb.st_blksize = -1;
		ssb->sb.st_blocks = -1;
#endif
		ssb->sb.st_ino = -1;
	}
	zend_string_release_ex(file_basename, 0);
	return 0;
}
/* }}} */

#if LIBZIP_ATLEAST(1,9,1)
/* {{{ php_zip_ops_seek */
static int php_zip_ops_seek(php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffset)
{
	int ret = -1;
	STREAM_DATA_FROM_STREAM();

	if (self->zf) {
		ret = zip_fseek(self->zf, offset, whence);
		*newoffset = zip_ftell(self->zf);
	}

	return ret;
}
/* }}} */

/* with seek command */
const php_stream_ops php_stream_zipio_seek_ops = {
	php_zip_ops_write, php_zip_ops_read,
	php_zip_ops_close, php_zip_ops_flush,
	"zip",
	php_zip_ops_seek, /* seek */
	NULL, /* cast */
	php_zip_ops_stat, /* stat */
	NULL  /* set_option */
};
#endif

/* without seek command */
const php_stream_ops php_stream_zipio_ops = {
	php_zip_ops_write, php_zip_ops_read,
	php_zip_ops_close, php_zip_ops_flush,
	"zip",
	NULL, /* seek */
	NULL, /* cast */
	php_zip_ops_stat, /* stat */
	NULL  /* set_option */
};

/* {{{ php_stream_zip_open */
php_stream *php_stream_zip_open(struct zip *arch, struct zip_stat *sb, const char *mode, zip_flags_t flags STREAMS_DC)
{
	struct zip_file *zf = NULL;

	php_stream *stream = NULL;
	struct php_zip_stream_data_t *self;

	if (strncmp(mode,"r", strlen("r")) != 0) {
		return NULL;
	}

	if (arch) {
		zf = zip_fopen_index(arch, sb->index, flags);
		if (zf) {
			self = emalloc(sizeof(*self));

			self->za = NULL; /* to keep it open on stream close */
			self->zf = zf;
			self->stream = NULL;
			self->cursor = 0;
#if LIBZIP_ATLEAST(1,9,1)
			if (zip_file_is_seekable(zf) > 0) {
				stream = php_stream_alloc(&php_stream_zipio_seek_ops, self, NULL, mode);
			} else
#endif
			{
				stream = php_stream_alloc(&php_stream_zipio_ops, self, NULL, mode);
			}
			stream->orig_path = estrdup(sb->name);
		}
	}

	return stream;
}
/* }}} */

/* {{{ php_stream_zip_opener */
php_stream *php_stream_zip_opener(php_stream_wrapper *wrapper,
											const char *path,
											const char *mode,
											int options,
											zend_string **opened_path,
											php_stream_context *context STREAMS_DC)
{
	size_t path_len;

	zend_string *file_basename;
	char file_dirname[MAXPATHLEN];

	struct zip *za;
	struct zip_file *zf = NULL;
	char *fragment;
	size_t fragment_len;
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

	file_basename = php_basename(path, path_len - fragment_len, NULL, 0);
	fragment++;

	if (ZIP_OPENBASEDIR_CHECKPATH(file_dirname)) {
		zend_string_release_ex(file_basename, 0);
		return NULL;
	}

	za = zip_open(file_dirname, ZIP_CREATE, &err);
	if (za) {
		zval *tmpzval;

		if (context && NULL != (tmpzval = php_stream_context_get_option(context, "zip", "password"))) {
			if (Z_TYPE_P(tmpzval) != IS_STRING || zip_set_default_password(za, Z_STRVAL_P(tmpzval))) {
				php_error_docref(NULL, E_WARNING, "Can't set zip password");
			}
		}

		zf = zip_fopen(za, fragment, 0);
		if (zf) {
			self = emalloc(sizeof(*self));

			self->za = za;
			self->zf = zf;
			self->stream = NULL;
			self->cursor = 0;
#if LIBZIP_ATLEAST(1,9,1)
			if (zip_file_is_seekable(zf) > 0) {
				stream = php_stream_alloc(&php_stream_zipio_seek_ops, self, NULL, mode);
			} else
#endif
			{
				stream = php_stream_alloc(&php_stream_zipio_ops, self, NULL, mode);
			}

			if (opened_path) {
				*opened_path = zend_string_init(path, strlen(path), 0);
			}
		} else {
			zip_close(za);
		}
	}

	zend_string_release_ex(file_basename, 0);

	if (!stream) {
		return NULL;
	} else {
		return stream;
	}
}
/* }}} */

static const php_stream_wrapper_ops zip_stream_wops = {
	php_stream_zip_opener,
	NULL,	/* close */
	NULL,	/* fstat */
	NULL,	/* stat */
	NULL,	/* opendir */
	"zip wrapper",
	NULL,	/* unlink */
	NULL,	/* rename */
	NULL,	/* mkdir */
	NULL,	/* rmdir */
	NULL	/* metadata */
};

const php_stream_wrapper php_stream_zip_wrapper = {
	&zip_stream_wops,
	NULL,
	0 /* is_url */
};
#endif /* HAVE_ZIP */
