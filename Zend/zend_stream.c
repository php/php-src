/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 Zend Technologies Ltd. (http://www.zend.com) |
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

static size_t zend_stream_stdio_reader(void *handle, char *buf, size_t len) /* {{{ */
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
	return 0;
} /* }}} */

static void zend_stream_unmap(zend_stream *stream) { /* {{{ */
	if (stream->mmap.buf) {
		efree(stream->mmap.buf);
	}
	stream->mmap.len = 0;
	stream->mmap.buf = 0;
	stream->handle   = stream->mmap.old_handle;
} /* }}} */

static void zend_stream_mmap_closer(zend_stream *stream) /* {{{ */
{
	zend_stream_unmap(stream);
	if (stream->mmap.old_closer && stream->handle) {
		stream->mmap.old_closer(stream->handle);
	}
} /* }}} */

static inline int zend_stream_is_mmap(zend_file_handle *file_handle) { /* {{{ */
	return file_handle->type == ZEND_HANDLE_MAPPED;
} /* }}} */

static size_t zend_stream_fsize(zend_file_handle *file_handle) /* {{{ */
{
	zend_stat_t buf;

	if (zend_stream_is_mmap(file_handle)) {
		return file_handle->handle.stream.mmap.len;
	}
	if (file_handle->type == ZEND_HANDLE_STREAM || file_handle->type == ZEND_HANDLE_MAPPED) {
		return file_handle->handle.stream.fsizer(file_handle->handle.stream.handle);
	}
	if (file_handle->handle.fp && zend_fstat(fileno(file_handle->handle.fp), &buf) == 0) {
#ifdef S_ISREG
		if (!S_ISREG(buf.st_mode)) {
			return 0;
		}
#endif
		return buf.st_size;
	}

	return -1;
} /* }}} */

ZEND_API int zend_stream_open(const char *filename, zend_file_handle *handle) /* {{{ */
{
	if (zend_stream_open_function) {
		return zend_stream_open_function(filename, handle);
	}
	handle->type = ZEND_HANDLE_FP;
	handle->opened_path = NULL;
	handle->handle.fp = zend_fopen(filename, &handle->opened_path);
	handle->filename = filename;
	handle->free_filename = 0;
	memset(&handle->handle.stream.mmap, 0, sizeof(zend_mmap));

	return (handle->handle.fp) ? SUCCESS : FAILURE;
} /* }}} */

static int zend_stream_getc(zend_file_handle *file_handle) /* {{{ */
{
	char buf;

	if (file_handle->handle.stream.reader(file_handle->handle.stream.handle, &buf, sizeof(buf))) {
		return (int)buf;
	}
	return EOF;
} /* }}} */

static size_t zend_stream_read(zend_file_handle *file_handle, char *buf, size_t len) /* {{{ */
{
	if (!zend_stream_is_mmap(file_handle) && file_handle->handle.stream.isatty) {
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
	size_t size;
	zend_stream_type old_type;

	if (file_handle->type == ZEND_HANDLE_FILENAME) {
		if (zend_stream_open(file_handle->filename, file_handle) == FAILURE) {
			return FAILURE;
		}
	}

	switch (file_handle->type) {
		case ZEND_HANDLE_FD:
			file_handle->type = ZEND_HANDLE_FP;
			file_handle->handle.fp = fdopen(file_handle->handle.fd, "rb");
			/* no break; */
		case ZEND_HANDLE_FP:
			if (!file_handle->handle.fp) {
				return FAILURE;
			}
			memset(&file_handle->handle.stream.mmap, 0, sizeof(zend_mmap));
			file_handle->handle.stream.isatty     = isatty(fileno((FILE *)file_handle->handle.stream.handle)) ? 1 : 0;
			file_handle->handle.stream.reader     = (zend_stream_reader_t)zend_stream_stdio_reader;
			file_handle->handle.stream.closer     = (zend_stream_closer_t)zend_stream_stdio_closer;
			file_handle->handle.stream.fsizer     = (zend_stream_fsizer_t)zend_stream_stdio_fsizer;
			memset(&file_handle->handle.stream.mmap, 0, sizeof(file_handle->handle.stream.mmap));
			/* no break; */
		case ZEND_HANDLE_STREAM:
			/* nothing to do */
			break;

		case ZEND_HANDLE_MAPPED:
			*buf = file_handle->handle.stream.mmap.buf;
			*len = file_handle->handle.stream.mmap.len;
			return SUCCESS;

		default:
			return FAILURE;
	}

	size = zend_stream_fsize(file_handle);
	if (size == (size_t)-1) {
		return FAILURE;
	}

	old_type = file_handle->type;
	file_handle->type = ZEND_HANDLE_STREAM;  /* we might still be _FP but we need fsize() work */

	if (old_type == ZEND_HANDLE_FP && !file_handle->handle.stream.isatty && size) {
		file_handle->handle.stream.mmap.buf = *buf = safe_emalloc(1, size, ZEND_MMAP_AHEAD);
		file_handle->handle.stream.mmap.len = zend_stream_read(file_handle, *buf, size);
	} else {
		size_t read, remain = 4*1024;
		*buf = emalloc(remain);
		size = 0;

		while ((read = zend_stream_read(file_handle, *buf + size, remain)) > 0) {
			size   += read;
			remain -= read;

			if (remain == 0) {
				*buf   = safe_erealloc(*buf, size, 2, 0);
				remain = size;
			}
		}
		file_handle->handle.stream.mmap.len = size;
		if (size && remain < ZEND_MMAP_AHEAD) {
			*buf = safe_erealloc(*buf, size, 1, ZEND_MMAP_AHEAD);
		}
		file_handle->handle.stream.mmap.buf = *buf;
	}

	if (file_handle->handle.stream.mmap.len == 0) {
		*buf = erealloc(*buf, ZEND_MMAP_AHEAD);
		file_handle->handle.stream.mmap.buf = *buf;
	}

	if (ZEND_MMAP_AHEAD) {
		memset(file_handle->handle.stream.mmap.buf + file_handle->handle.stream.mmap.len, 0, ZEND_MMAP_AHEAD);
	}
	file_handle->type = ZEND_HANDLE_MAPPED;
	file_handle->handle.stream.mmap.old_handle = file_handle->handle.stream.handle;
	file_handle->handle.stream.mmap.old_closer = file_handle->handle.stream.closer;
	file_handle->handle.stream.handle          = &file_handle->handle.stream;
	file_handle->handle.stream.closer          = (zend_stream_closer_t)zend_stream_mmap_closer;

	*buf = file_handle->handle.stream.mmap.buf;
	*len = file_handle->handle.stream.mmap.len;

	return SUCCESS;
} /* }}} */

ZEND_API void zend_file_handle_dtor(zend_file_handle *fh) /* {{{ */
{
	switch (fh->type) {
		case ZEND_HANDLE_FD:
			/* nothing to do */
			break;
		case ZEND_HANDLE_FP:
			fclose(fh->handle.fp);
			break;
		case ZEND_HANDLE_STREAM:
		case ZEND_HANDLE_MAPPED:
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
		case ZEND_HANDLE_FD:
			return fh1->handle.fd == fh2->handle.fd;
		case ZEND_HANDLE_FP:
			return fh1->handle.fp == fh2->handle.fp;
		case ZEND_HANDLE_STREAM:
			return fh1->handle.stream.handle == fh2->handle.stream.handle;
		case ZEND_HANDLE_MAPPED:
			return (fh1->handle.stream.handle == &fh1->handle.stream &&
			        fh2->handle.stream.handle == &fh2->handle.stream &&
			        fh1->handle.stream.mmap.old_handle == fh2->handle.stream.mmap.old_handle)
				|| fh1->handle.stream.handle == fh2->handle.stream.handle;
		default:
			return 0;
	}
	return 0;
} /* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
