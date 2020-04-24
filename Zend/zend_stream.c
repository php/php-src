/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Wez Furlong <wez@thebrainroom.com>                          |
   |          Scott MacVicar <scottmac@php.net>                           |
   |          Nuno Lopes <nlopess@php.net>                                |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_compile.h"
#include "zend_stream.h"

ZEND_DLIMPORT int isatty(int fd);

static ssize_t zend_stream_stdio_reader(void *handle, char *buf, size_t len) /* {{{ */
{
	return fread(buf, 1, len, (FILE*)handle);
} /* }}} */

static void zend_stream_stdio_closer(void *handle) /* {{{ */
{
	if (handle && (FILE*)handle != stdin) {
		fclose((FILE*)handle);
	}
} /* }}} */

static size_t zend_stream_stdio_fsizer(void *handle) /* {{{ */
{
	zend_stat_t buf;
	if (handle && zend_fstat(fileno((FILE*)handle), &buf) == 0) {
#ifdef S_ISREG
		if (!S_ISREG(buf.st_mode)) {
			return 0;
		}
#endif
		return buf.st_size;
	}
	return -1;
} /* }}} */

static size_t zend_stream_fsize(zend_file_handle *file_handle) /* {{{ */
{
	ZEND_ASSERT(file_handle->type == ZEND_HANDLE_STREAM);
	if (file_handle->handle.stream.isatty) {
		return 0;
	}
	return file_handle->handle.stream.fsizer(file_handle->handle.stream.handle);
} /* }}} */

ZEND_API void zend_stream_init_fp(zend_file_handle *handle, FILE *fp, const char *filename) {
	memset(handle, 0, sizeof(zend_file_handle));
	handle->type = ZEND_HANDLE_FP;
	handle->handle.fp = fp;
	handle->filename = filename;
}

ZEND_API void zend_stream_init_filename(zend_file_handle *handle, const char *filename) {
	memset(handle, 0, sizeof(zend_file_handle));
	handle->type = ZEND_HANDLE_FILENAME;
	handle->filename = filename;
}

ZEND_API int zend_stream_open(const char *filename, zend_file_handle *handle) /* {{{ */
{
	zend_string *opened_path;
	if (zend_stream_open_function) {
		return zend_stream_open_function(filename, handle);
	}

	zend_stream_init_fp(handle, zend_fopen(filename, &opened_path), filename);
	handle->opened_path = opened_path;
	return handle->handle.fp ? SUCCESS : FAILURE;
} /* }}} */

static int zend_stream_getc(zend_file_handle *file_handle) /* {{{ */
{
	char buf;

	if (file_handle->handle.stream.reader(file_handle->handle.stream.handle, &buf, sizeof(buf))) {
		return (int)buf;
	}
	return EOF;
} /* }}} */

static ssize_t zend_stream_read(zend_file_handle *file_handle, char *buf, size_t len) /* {{{ */
{
	if (file_handle->handle.stream.isatty) {
		int c = '*';
		size_t n;

		for (n = 0; n < len && (c = zend_stream_getc(file_handle)) != EOF && c != '\n'; ++n)  {
			buf[n] = (char)c;
		}
		if (c == '\n') {
			buf[n++] = (char)c;
		}

		return n;
	}
	return file_handle->handle.stream.reader(file_handle->handle.stream.handle, buf, len);
} /* }}} */

ZEND_API int zend_stream_fixup(zend_file_handle *file_handle, char **buf, size_t *len) /* {{{ */
{
	size_t file_size;

	if (file_handle->buf) {
		*buf = file_handle->buf;
		*len = file_handle->len;
		return SUCCESS;
	}

	if (file_handle->type == ZEND_HANDLE_FILENAME) {
		if (zend_stream_open(file_handle->filename, file_handle) == FAILURE) {
			return FAILURE;
		}
	}

	if (file_handle->type == ZEND_HANDLE_FP) {
		if (!file_handle->handle.fp) {
			return FAILURE;
		}

		file_handle->type = ZEND_HANDLE_STREAM;
		file_handle->handle.stream.handle = file_handle->handle.fp;
		file_handle->handle.stream.isatty = isatty(fileno((FILE *)file_handle->handle.stream.handle));
		file_handle->handle.stream.reader = (zend_stream_reader_t)zend_stream_stdio_reader;
		file_handle->handle.stream.closer = (zend_stream_closer_t)zend_stream_stdio_closer;
		file_handle->handle.stream.fsizer = (zend_stream_fsizer_t)zend_stream_stdio_fsizer;
	}

	file_size = zend_stream_fsize(file_handle);
	if (file_size == (size_t)-1) {
		return FAILURE;
	}

	if (file_size) {
		ssize_t read;
		size_t size = 0;
		*buf = safe_emalloc(1, file_size, ZEND_MMAP_AHEAD);
		while ((read = zend_stream_read(file_handle, *buf + size, file_size - size)) > 0) {
			size += read;
		}
		if (read < 0) {
			efree(*buf);
			return FAILURE;
		}
		file_handle->buf = *buf;
		file_handle->len = size;
	} else {
		size_t size = 0, remain = 4*1024;
		ssize_t read;
		*buf = emalloc(remain);

		while ((read = zend_stream_read(file_handle, *buf + size, remain)) > 0) {
			size   += read;
			remain -= read;

			if (remain == 0) {
				*buf   = safe_erealloc(*buf, size, 2, 0);
				remain = size;
			}
		}
		if (read < 0) {
			efree(*buf);
			return FAILURE;
		}

		file_handle->len = size;
		if (size && remain < ZEND_MMAP_AHEAD) {
			*buf = safe_erealloc(*buf, size, 1, ZEND_MMAP_AHEAD);
		}
		file_handle->buf = *buf;
	}

	if (file_handle->len == 0) {
		*buf = erealloc(*buf, ZEND_MMAP_AHEAD);
		file_handle->buf = *buf;
	}

	memset(file_handle->buf + file_handle->len, 0, ZEND_MMAP_AHEAD);

	*buf = file_handle->buf;
	*len = file_handle->len;

	return SUCCESS;
} /* }}} */

ZEND_API void zend_file_handle_dtor(zend_file_handle *fh) /* {{{ */
{
	switch (fh->type) {
		case ZEND_HANDLE_FP:
			fclose(fh->handle.fp);
			break;
		case ZEND_HANDLE_STREAM:
			if (fh->handle.stream.closer && fh->handle.stream.handle) {
				fh->handle.stream.closer(fh->handle.stream.handle);
			}
			fh->handle.stream.handle = NULL;
			break;
		case ZEND_HANDLE_FILENAME:
			/* We're only supposed to get here when destructing the used_files hash,
			 * which doesn't really contain open files, but references to their names/paths
			 */
			break;
	}
	if (fh->opened_path) {
		zend_string_release_ex(fh->opened_path, 0);
		fh->opened_path = NULL;
	}
	if (fh->buf) {
		efree(fh->buf);
		fh->buf = NULL;
	}
	if (fh->free_filename && fh->filename) {
		efree((char*)fh->filename);
		fh->filename = NULL;
	}
}
/* }}} */

ZEND_API int zend_compare_file_handles(zend_file_handle *fh1, zend_file_handle *fh2) /* {{{ */
{
	if (fh1->type != fh2->type) {
		return 0;
	}
	switch (fh1->type) {
		case ZEND_HANDLE_FILENAME:
			return strcmp(fh1->filename, fh2->filename) == 0;
		case ZEND_HANDLE_FP:
			return fh1->handle.fp == fh2->handle.fp;
		case ZEND_HANDLE_STREAM:
			return fh1->handle.stream.handle == fh2->handle.stream.handle;
		default:
			return 0;
	}
	return 0;
} /* }}} */
