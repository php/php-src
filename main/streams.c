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
   | Authors:                                                             |
   | Wez Furlong (wez@thebrainroom.com)                                   |
   | Borrowed code from:                                                  |
   | Rasmus Lerdorf <rasmus@lerdorf.on.ca>                                |
   | Jim Winstead <jimw@php.net>                                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#define _GNU_SOURCE
#include "php.h"
#include "php_globals.h"
#include "php_network.h"
#include "php_open_temporary_file.h"
#include "ext/standard/file.h"
#include "ext/standard/basic_functions.h" /* for BG(mmap_file) (not strictly required) */
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif

#include <fcntl.h>

#ifndef MAP_FAILED
#define MAP_FAILED ((void *) -1)
#endif

#define CHUNK_SIZE	8192

#ifdef PHP_WIN32
#define EWOULDBLOCK WSAEWOULDBLOCK
#else
#include "build-defs.h"
#endif

#define STREAM_DEBUG 0

#define STREAM_WRAPPER_PLAIN_FILES	((php_stream_wrapper*)-1)

/* {{{ some macros to help track leaks */
#if ZEND_DEBUG
#define emalloc_rel_orig(size)	\
		( __php_stream_call_depth == 0 \
		? _emalloc((size) ZEND_FILE_LINE_CC ZEND_FILE_LINE_RELAY_CC) \
		: _emalloc((size) ZEND_FILE_LINE_CC ZEND_FILE_LINE_ORIG_RELAY_CC) )

#define erealloc_rel_orig(ptr, size)	\
		( __php_stream_call_depth == 0 \
		? _erealloc((ptr), (size), 0 ZEND_FILE_LINE_CC ZEND_FILE_LINE_RELAY_CC) \
		: _erealloc((ptr), (size), 0 ZEND_FILE_LINE_CC ZEND_FILE_LINE_ORIG_RELAY_CC) )


#define pemalloc_rel_orig(size, persistent)	((persistent) ? malloc((size)) : emalloc_rel_orig((size)))
#define perealloc_rel_orig(ptr, size, persistent)	((persistent) ? realloc((ptr), (size)) : erealloc_rel_orig((ptr), (size)))
#else
# define pemalloc_rel_orig(size, persistent)				pemalloc((size), (persistent))
# define perealloc_rel_orig(ptr, size, persistent)			perealloc((ptr), (size), (persistent))
# define emalloc_rel_orig(size)								emalloc((size))
#endif
/* }}} */

static HashTable url_stream_wrappers_hash;

/* allocate a new stream for a particular ops */
PHPAPI php_stream *_php_stream_alloc(php_stream_ops *ops, void *abstract, int persistent, const char *mode STREAMS_DC TSRMLS_DC) /* {{{ */
{
	php_stream *ret;
	
	ret = (php_stream*) pemalloc_rel_orig(sizeof(php_stream), persistent);

	memset(ret, 0, sizeof(php_stream));

#if STREAM_DEBUG
fprintf(stderr, "stream_alloc: %s:%p\n", ops->label, ret);
#endif
	
	ret->ops = ops;
	ret->abstract = abstract;
	ret->is_persistent = persistent;
	ret->rsrc_id = ZEND_REGISTER_RESOURCE(NULL, ret, php_file_le_stream());
	strlcpy(ret->mode, mode, sizeof(ret->mode));

	return ret;
}
/* }}} */

PHPAPI int _php_stream_free(php_stream *stream, int close_options TSRMLS_DC) /* {{{ */
{
	int ret = 1;

#if STREAM_DEBUG
fprintf(stderr, "stream_free: %s:%p in_free=%d opts=%08x\n", stream->ops->label, stream, stream->in_free, close_options);
#endif
	
	if (stream->in_free)
		return 1;

	stream->in_free++;

	php_stream_flush(stream);
	
	if ((close_options & PHP_STREAM_FREE_RSRC_DTOR) == 0) {
		/* Remove entry from the resource list */
		zend_list_delete(stream->rsrc_id);
	}
	if (close_options & PHP_STREAM_FREE_CALL_DTOR) {
		if (stream->fclose_stdiocast == PHP_STREAM_FCLOSE_FOPENCOOKIE) {
			/* calling fclose on an fopencookied stream will ultimately
				call this very same function.  If we were called via fclose,
				the cookie_closer unsets the fclose_stdiocast flags, so
				we can be sure that we only reach here when PHP code calls
				php_stream_free.
				Lets let the cookie code clean it all up.
			 */
			stream->in_free = 0;
			return fclose(stream->stdiocast);
		}

		ret = stream->ops->close(stream, close_options & PHP_STREAM_FREE_PRESERVE_HANDLE ? 0 : 1 TSRMLS_CC);
		stream->abstract = NULL;

		/* tidy up any FILE* that might have been fdopened */
		if (stream->fclose_stdiocast == PHP_STREAM_FCLOSE_FDOPEN && stream->stdiocast) {
			fclose(stream->stdiocast);
			stream->stdiocast = NULL;
		}
	}

	if (close_options & PHP_STREAM_FREE_RELEASE_STREAM) {
		
		while (stream->filterhead) {
			php_stream_filter_remove_head(stream, 1);
		}

		if (stream->wrapper && stream->wrapper->wops && stream->wrapper->wops->stream_closer) {
			stream->wrapper->wops->stream_closer(stream->wrapper, stream TSRMLS_CC);
			stream->wrapper = NULL;
		}

		if (stream->wrapperdata) {
			zval_ptr_dtor(&stream->wrapperdata);
			stream->wrapperdata = NULL;
		}

#if ZEND_DEBUG
		if ((close_options & PHP_STREAM_FREE_RSRC_DTOR) && (stream->__exposed == 0) && (EG(error_reporting) & E_WARNING)) {
			/* it leaked: Lets deliberately NOT pefree it so that the memory manager shows it
			 * as leaked; it will log a warning, but lets help it out and display what kind
			 * of stream it was. */
			char leakbuf[512];
			snprintf(leakbuf, sizeof(leakbuf), __FILE__ "(%d) : Stream of type '%s' 0x%08X (path:%s) was not closed\n", __LINE__, stream->ops->label, (unsigned int)stream, stream->__orig_path);

			STR_FREE(stream->__orig_path);
			
# if defined(PHP_WIN32)
			OutputDebugString(leakbuf);
# else
			fprintf(stderr, leakbuf);
# endif
		} else {
			STR_FREE(stream->__orig_path);
			pefree(stream, stream->is_persistent);
		}
#else
		pefree(stream, stream->is_persistent);
#endif
	}

	return ret;
}
/* }}} */

static HashTable stream_filters_hash;

PHPAPI int php_stream_filter_register_factory(const char *filterpattern, php_stream_filter_factory *factory TSRMLS_DC)
{
	return zend_hash_add(&stream_filters_hash, (char*)filterpattern, strlen(filterpattern), factory, sizeof(*factory), NULL);
}

PHPAPI int php_stream_filter_unregister_factory(const char *filterpattern TSRMLS_DC)
{
	return zend_hash_del(&stream_filters_hash, (char*)filterpattern, strlen(filterpattern));
}

/* We allow very simple pattern matching for filter factories:
 * if "charset.utf-8/sjis" is requested, we search first for an exact
 * match. If that fails, we try "charset.*".
 * This means that we don't need to clog up the hashtable with a zillion
 * charsets (for example) but still be able to provide them all as filters */
PHPAPI php_stream_filter *php_stream_filter_create(const char *filtername, const char *filterparams, int filterparamslen, int persistent TSRMLS_DC)
{
	php_stream_filter_factory *factory;
	php_stream_filter *filter = NULL;
	int n;
	char *period;

	n = strlen(filtername);
	
	if (SUCCESS == zend_hash_find(&stream_filters_hash, (char*)filtername, n, (void**)&factory)) {
		filter = factory->create_filter(filtername, filterparams, filterparamslen, persistent TSRMLS_CC);
	} else if ((period = strchr(filtername, '.'))) {
		/* try a wildcard */
		char wildname[128];

		PHP_STRLCPY(wildname, filtername, sizeof(wildname) - 1, period-filtername + 1);
		strcat(wildname, "*");
		
		if (SUCCESS == zend_hash_find(&stream_filters_hash, wildname, strlen(wildname), (void**)&factory)) {
			filter = factory->create_filter(filtername, filterparams, filterparamslen, persistent TSRMLS_CC);
		}
	}

	if (filter == NULL) {
		/* TODO: these need correct docrefs */
		if (factory == NULL)
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to locate filter \"%s\"", filtername);
		else
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to create or locate filter \"%s\"", filtername);
	}
	
	return filter;
}

PHPAPI php_stream_filter *_php_stream_filter_alloc(php_stream_filter_ops *fops, void *abstract, int persistent STREAMS_DC TSRMLS_DC)
{
	php_stream_filter *filter;

	filter = (php_stream_filter*) pemalloc_rel_orig(sizeof(php_stream_filter), persistent);
	memset(filter, 0, sizeof(php_stream_filter));

	filter->fops = fops;
	filter->abstract = abstract;
	filter->is_persistent = persistent;
	
	return filter;
}

PHPAPI void php_stream_filter_free(php_stream_filter *filter TSRMLS_DC)
{
	if (filter->fops->dtor)
		filter->fops->dtor(filter TSRMLS_CC);
	pefree(filter, filter->is_persistent);
}

PHPAPI void php_stream_filter_prepend(php_stream *stream, php_stream_filter *filter)
{
	filter->next = stream->filterhead;
	filter->prev = NULL;

	if (stream->filterhead) {
		stream->filterhead->prev = filter;
	} else {
		stream->filtertail = filter;
	}
	stream->filterhead = filter;
	filter->stream = stream;
}

PHPAPI void php_stream_filter_append(php_stream *stream, php_stream_filter *filter)
{
	filter->prev = stream->filtertail;
	filter->next = NULL;
	if (stream->filtertail) {
		stream->filtertail->next = filter;
	} else {
		stream->filterhead = filter;
	}
	stream->filtertail = filter;
	filter->stream = stream;
}

PHPAPI php_stream_filter *php_stream_filter_remove(php_stream *stream, php_stream_filter *filter, int call_dtor TSRMLS_DC)
{
	assert(stream == filter->stream);
	
	if (filter->prev) {
		filter->prev->next = filter->next;
	} else {
		stream->filterhead = filter->next;
	}
	if (filter->next) {
		filter->next->prev = filter->prev;
	} else {
		stream->filtertail = filter->prev;
	}
	if (call_dtor) {
		php_stream_filter_free(filter TSRMLS_CC);
		return NULL;
	}
	return filter;
}

/* {{{ generic stream operations */
PHPAPI size_t _php_stream_read(php_stream *stream, char *buf, size_t size TSRMLS_DC)
{
	if (stream->filterhead)
		return stream->filterhead->fops->read(stream, stream->filterhead, buf, size TSRMLS_CC);
	
	return stream->ops->read(stream, buf, size TSRMLS_CC);
}

PHPAPI int _php_stream_eof(php_stream *stream TSRMLS_DC)
{
	/* we define our stream reading function so that it
	   must return EOF when an EOF condition occurs, when
	   working in unbuffered mode and called with these args */

	if (stream->filterhead)
		return stream->filterhead->fops->eof(stream, stream->filterhead TSRMLS_CC);
	
	return stream->ops->read(stream, NULL, 0 TSRMLS_CC) == EOF ? 1 : 0;
}

PHPAPI int _php_stream_putc(php_stream *stream, int c TSRMLS_DC)
{
	unsigned char buf = c;

	if (php_stream_write(stream, &buf, 1) > 0) {
		return 1;
	}
	return EOF;
}

PHPAPI int _php_stream_getc(php_stream *stream TSRMLS_DC)
{
	char buf;

	if (php_stream_read(stream, &buf, 1) > 0) {
		return buf & 0xff;
	}
	return EOF;
}

PHPAPI int _php_stream_puts(php_stream *stream, char *buf TSRMLS_DC)
{
	int len;
	char newline[2] = "\n"; /* is this OK for Win? */
	len = strlen(buf);

	if (len > 0 && php_stream_write(stream, buf, len) && php_stream_write(stream, newline, 1)) {
		return 1;
	}
	return 0;
}

PHPAPI int _php_stream_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC)
{
	memset(ssb, 0, sizeof(*ssb));

	/* if the stream was wrapped, allow the wrapper to stat it */
	if (stream->wrapper && stream->wrapper->wops->stream_stat != NULL) {
		return stream->wrapper->wops->stream_stat(stream->wrapper, stream, ssb TSRMLS_CC);
	}

	/* if the stream doesn't directly support stat-ing, return with failure.
	 * We could try and emulate this by casting to a FD and fstat-ing it,
	 * but since the fd might not represent the actual underlying content
	 * this would give bogus results. */
	if (stream->ops->stat == NULL) {
		return -1;
	}

	return stream->ops->stat(stream, ssb TSRMLS_CC);
}

PHPAPI char *_php_stream_gets(php_stream *stream, char *buf, size_t maxlen TSRMLS_DC)
{
	if (maxlen == 0)
		return NULL;

	if (stream->filterhead || stream->ops->gets == NULL) {
		/* unbuffered fgets - performance not so good! */
		char *c = buf;

		/* TODO: look at error returns? */

		while (--maxlen > 0 && php_stream_read(stream, buf, 1) == 1 && *buf++ != '\n')
			;
		*buf = '\0';

		return c == buf && maxlen > 0 ? NULL : c;

	} else if (stream->ops->gets) {
		return stream->ops->gets(stream, buf, maxlen TSRMLS_CC);
	}
	/* should not happen */
	return NULL;
}

PHPAPI int _php_stream_flush(php_stream *stream TSRMLS_DC)
{
	if (stream->filterhead)
		stream->filterhead->fops->flush(stream, stream->filterhead TSRMLS_CC);

	if (stream->ops->flush) {
		return stream->ops->flush(stream TSRMLS_CC);
	}
	return 0;
}

PHPAPI size_t _php_stream_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	assert(stream);
	if (buf == NULL || count == 0 || stream->ops->write == NULL)
		return 0;

	if (stream->filterhead) {
		return stream->filterhead->fops->write(stream, stream->filterhead, buf, count TSRMLS_CC);
	} else {
		return stream->ops->write(stream, buf, count TSRMLS_CC);
	}
}

PHPAPI off_t _php_stream_tell(php_stream *stream TSRMLS_DC)
{
	off_t ret = -1;

	if (stream->ops->seek) {
		ret = stream->ops->seek(stream, 0, SEEK_CUR TSRMLS_CC);
	}
	return ret;
}

PHPAPI int _php_stream_seek(php_stream *stream, off_t offset, int whence TSRMLS_DC)
{
	if (stream->ops->seek) {

		if (stream->filterhead)
			stream->filterhead->fops->flush(stream, stream->filterhead TSRMLS_CC);
		
		return stream->ops->seek(stream, offset, whence TSRMLS_CC);
	}

	/* emulate forward moving seeks with reads */
	if (whence == SEEK_CUR && offset > 0) {
		char tmp[1024];
		while(offset >= sizeof(tmp)) {
			if (php_stream_read(stream, tmp, sizeof(tmp)) == 0)
				return -1;
			offset -= sizeof(tmp);
		}
		if (offset)	{
			if (php_stream_read(stream, tmp, offset) == 0)
				return -1;
		}
		return 0;
	}

	zend_error(E_WARNING, "streams of type %s do not support seeking", stream->ops->label);
	return -1;
}

PHPAPI int _php_stream_set_option(php_stream *stream, int option, int value, void *ptrparam TSRMLS_DC)
{
	if (stream->ops->set_option)
		return stream->ops->set_option(stream, option, value, ptrparam TSRMLS_CC);
	return -1;
}

PHPAPI size_t _php_stream_passthru(php_stream * stream STREAMS_DC TSRMLS_DC)
{
	size_t bcount = 0;
	int ready = 0;
	char buf[8192];
#ifdef HAVE_MMAP
	int fd;
#endif

#ifdef HAVE_MMAP
	if (!php_stream_is(stream, PHP_STREAM_IS_SOCKET)
			&& stream->filterhead == NULL
			&& SUCCESS == php_stream_cast(stream, PHP_STREAM_AS_FD, (void*)&fd, 0))
	{
		struct stat sbuf;
		off_t off;
		void *p;
		size_t len;

		fstat(fd, &sbuf);

		if (sbuf.st_size > sizeof(buf)) {
			off = php_stream_tell(stream);
			len = sbuf.st_size - off;
			p = mmap(0, len, PROT_READ, MAP_SHARED, fd, off);
			if (p != (void *) MAP_FAILED) {
				BG(mmap_file) = p;
				BG(mmap_len) = len;
				PHPWRITE(p, len);
				BG(mmap_file) = NULL;
				munmap(p, len);
				bcount += len;
				ready = 1;
			}
		}
	}
#endif
	if(!ready) {
		int b;

		while ((b = php_stream_read(stream, buf, sizeof(buf))) > 0) {
			PHPWRITE(buf, b);
			bcount += b;
		}
	}
	return bcount;
}


PHPAPI size_t _php_stream_copy_to_mem(php_stream *src, char **buf, size_t maxlen, int persistent STREAMS_DC TSRMLS_DC)
{
	size_t ret = 0;
	char *ptr;
	size_t len = 0, max_len;
	int step = CHUNK_SIZE;
	int min_room = CHUNK_SIZE / 4;
#if HAVE_MMAP
	int srcfd;
#endif

	if (buf)
		*buf = NULL;

	if (maxlen == 0)
		return 0;

	if (maxlen == PHP_STREAM_COPY_ALL)
		maxlen = 0;

#if HAVE_MMAP
	/* try and optimize the case where we are copying from the start of a plain file.
	 * We could probably make this work in more situations, but I don't trust the stdio
	 * buffering layer.
	 * */
	if ( php_stream_is(src, PHP_STREAM_IS_STDIO) &&
			src->filterhead == NULL &&
			php_stream_tell(src) == 0 &&
			SUCCESS == php_stream_cast(src, PHP_STREAM_AS_FD, (void**)&srcfd, 0))
	{
		struct stat sbuf;

		if (fstat(srcfd, &sbuf) == 0) {
			void *srcfile;

#if STREAM_DEBUG
			fprintf(stderr, "mmap attempt: maxlen=%d filesize=%d\n", maxlen, sbuf.st_size);
#endif
	
			if (maxlen > sbuf.st_size || maxlen == 0)
				maxlen = sbuf.st_size;
#if STREAM_DEBUG
			fprintf(stderr, "mmap attempt: will map maxlen=%d\n", maxlen);
#endif
		
			srcfile = mmap(NULL, maxlen, PROT_READ, MAP_SHARED, srcfd, 0);
			if (srcfile != (void*)MAP_FAILED) {

				*buf = pemalloc_rel_orig(maxlen + 1, persistent);

				if (*buf)	{
					memcpy(*buf, srcfile, maxlen);
					(*buf)[maxlen] = '\0';
					ret = maxlen;
				}

				munmap(srcfile, maxlen);

				return ret;
			}
		}
		/* fall through - we might be able to copy in smaller chunks */
	}
#endif

	ptr = *buf = pemalloc_rel_orig(step, persistent);
	max_len = step;

	while((ret = php_stream_read(src, ptr, max_len - len)))	{
		len += ret;
		if (len + min_room >= max_len) {
			*buf = perealloc_rel_orig(*buf, max_len + step, persistent);
			max_len += step;
			ptr = *buf + len;
		}
	}
	if (len) {
		*buf = perealloc_rel_orig(*buf, len + 1, persistent);
		(*buf)[len] = '\0';
	} else {
		pefree(*buf, persistent);
		*buf = NULL;
	}
	return len;
}

PHPAPI size_t _php_stream_copy_to_stream(php_stream *src, php_stream *dest, size_t maxlen STREAMS_DC TSRMLS_DC)
{
	char buf[CHUNK_SIZE];
	size_t readchunk;
	size_t haveread = 0;
	size_t didread;
#if HAVE_MMAP
	int srcfd;
#endif

	if (maxlen == 0)
		return 0;

	if (maxlen == PHP_STREAM_COPY_ALL)
		maxlen = 0;

#if HAVE_MMAP
	/* try and optimize the case where we are copying from the start of a plain file.
	 * We could probably make this work in more situations, but I don't trust the stdio
	 * buffering layer.
	 * */
	if ( php_stream_is(src, PHP_STREAM_IS_STDIO) &&
			src->filterhead == NULL &&
			php_stream_tell(src) == 0 &&
			SUCCESS == php_stream_cast(src, PHP_STREAM_AS_FD, (void**)&srcfd, 0))
	{
		struct stat sbuf;

		if (fstat(srcfd, &sbuf) == 0) {
			void *srcfile;

			/* in the event that the source file is 0 bytes, return 1 to indicate success
			 * because opening the file to write had already created a copy */

			if(sbuf.st_size ==0)
				return 1;

			if (maxlen > sbuf.st_size || maxlen == 0)
				maxlen = sbuf.st_size;

			srcfile = mmap(NULL, maxlen, PROT_READ, MAP_SHARED, srcfd, 0);
			if (srcfile != (void*)MAP_FAILED) {
				haveread = php_stream_write(dest, srcfile, maxlen);
				munmap(srcfile, maxlen);
				return haveread;
			}
		}
		/* fall through - we might be able to copy in smaller chunks */
	}
#endif

	while(1) {
		readchunk = sizeof(buf);

		if (maxlen && (maxlen - haveread) < readchunk)
			readchunk = maxlen - haveread;

		didread = php_stream_read(src, buf, readchunk);

		if (didread) {
			/* extra paranoid */
			size_t didwrite, towrite;
			char *writeptr;

			towrite = didread;
			writeptr = buf;
			haveread += didread;

			while(towrite) {
				didwrite = php_stream_write(dest, writeptr, towrite);
				if (didwrite == 0)
					return 0;	/* error */

				towrite -= didwrite;
				writeptr += didwrite;
			}
		} else {
			if (maxlen == 0) {
				return haveread;
			} else {
				return 0; /* error */
			}
		}

		if (maxlen - haveread == 0) {
			break;
		}
	}
	return haveread;

}
/* }}} */

/* {{{ ------- STDIO stream implementation -------*/

typedef struct {
	FILE *file;
	int is_pipe;	/* use pclose */
#if HAVE_FLUSHIO
	char last_op;
#endif
} php_stdio_stream_data;

PHPAPI php_stream *_php_stream_fopen_temporary_file(const char *dir, const char *pfx, char **opened_path STREAMS_DC TSRMLS_DC)
{
	FILE *fp = php_open_temporary_file(dir, pfx, opened_path TSRMLS_CC);

	if (fp)	{
		php_stream *stream = php_stream_fopen_from_file_rel(fp, "wb");
		if (stream) {
			return stream;
		}
		fclose(fp);

		zend_error(E_WARNING, "%s(): unable to allocate stream", get_active_function_name(TSRMLS_C));

		return NULL;
	}
	return NULL;
}

PHPAPI php_stream *_php_stream_fopen_tmpfile(int dummy STREAMS_DC TSRMLS_DC)
{
	FILE *fp;
	php_stream *stream;

	fp = tmpfile();
	if (fp == NULL) {
		zend_error(E_WARNING, "tmpfile(): %s", strerror(errno));
		return NULL;
	}
	stream = php_stream_fopen_from_file_rel(fp, "r+");
	if (stream == NULL)	{
		zend_error(E_WARNING, "tmpfile(): %s", strerror(errno));
		fclose(fp);
		return NULL;
	}
	return stream;
}



PHPAPI php_stream *_php_stream_fopen_from_file(FILE *file, const char *mode STREAMS_DC TSRMLS_DC)
{
	php_stdio_stream_data *self;

	self = emalloc_rel_orig(sizeof(*self));
	self->file = file;
	self->is_pipe = 0;
	return php_stream_alloc_rel(&php_stream_stdio_ops, self, 0, mode);
}

PHPAPI php_stream *_php_stream_fopen_from_pipe(FILE *file, const char *mode STREAMS_DC TSRMLS_DC)
{
	php_stdio_stream_data *self;

	self = emalloc_rel_orig(sizeof(*self));
	self->file = file;
	self->is_pipe = 1;
	return php_stream_alloc_rel(&php_stream_stdio_ops, self, 0, mode);
}
static size_t php_stdiop_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	php_stdio_stream_data *data = (php_stdio_stream_data*)stream->abstract;

	assert(data != NULL);

#if HAVE_FLUSHIO
	if (data->last_op == 'r') {
		fseek(data->file, 0, SEEK_CUR);
	}
	data->last_op = 'w';
#endif

	return fwrite(buf, 1, count, data->file);
}

static size_t php_stdiop_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	php_stdio_stream_data *data = (php_stdio_stream_data*)stream->abstract;

	assert(data != NULL);

	if (buf == NULL && count == 0)	{
		/* check for EOF condition */
		if (feof(data->file))	{
			return EOF;
		}
		return 0;
	}

#if HAVE_FLUSHIO
	if (data->last_op == 'w')
		fseek(data->file, 0, SEEK_CUR);
	data->last_op = 'r';
#endif

	return fread(buf, 1, count, data->file);
}

static int php_stdiop_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	int ret;
	php_stdio_stream_data *data = (php_stdio_stream_data*)stream->abstract;

	assert(data != NULL);

	if (close_handle) {
		if (data->is_pipe) {
			ret = pclose(data->file);
		} else {
			ret = fclose(data->file);
		}
	} else {
		ret = 0;
	}

	/* STDIO streams are never persistent! */
	efree(data);

	return ret;
}

static int php_stdiop_flush(php_stream *stream TSRMLS_DC)
{
	php_stdio_stream_data *data = (php_stdio_stream_data*)stream->abstract;

	assert(data != NULL);

	return fflush(data->file);
}

static int php_stdiop_seek(php_stream *stream, off_t offset, int whence TSRMLS_DC)
{
	php_stdio_stream_data *data = (php_stdio_stream_data*)stream->abstract;

	assert(data != NULL);

	if (offset == 0 && whence == SEEK_CUR)
		return ftell(data->file);

	return fseek(data->file, offset, whence);
}

static char *php_stdiop_gets(php_stream *stream, char *buf, size_t size TSRMLS_DC)
{
	php_stdio_stream_data *data = (php_stdio_stream_data*)stream->abstract;

	assert(data != NULL);
#if HAVE_FLUSHIO
	if (data->last_op == 'w') {
		fseek(data->file, 0, SEEK_CUR);
	}
	data->last_op = 'r';
#endif

	return fgets(buf, size, data->file);
}
static int php_stdiop_cast(php_stream *stream, int castas, void **ret TSRMLS_DC)
{
	int fd;
	php_stdio_stream_data *data = (php_stdio_stream_data*) stream->abstract;

	assert(data != NULL);

	switch (castas)	{
		case PHP_STREAM_AS_STDIO:
			if (ret) {
				*ret = data->file;
			}
			return SUCCESS;

		case PHP_STREAM_AS_FD:
			fd = fileno(data->file);
			if (fd < 0) {
				return FAILURE;
			}
			if (ret) {
				fflush(data->file);
				*ret = (void*)fd;
			}
			return SUCCESS;
		default:
			return FAILURE;
	}
}

static int php_stdiop_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC)
{
	int fd;
	php_stdio_stream_data *data = (php_stdio_stream_data*) stream->abstract;

	assert(data != NULL);

	fd = fileno(data->file);

	return fstat(fd, &ssb->sb);
}

static int php_stdiop_set_option(php_stream *stream, int option, int value, void *ptrparam TSRMLS_DC)
{
	php_stdio_stream_data *data = (php_stdio_stream_data*) stream->abstract;
	size_t size;
	int fd;
#ifdef O_NONBLOCK
	/* FIXME: make this work for win32 */
	int flags;
	int oldval;
#endif
	
	switch(option) {
		case PHP_STREAM_OPTION_BLOCKING:
			fd = fileno(data->file);

			if (fd == -1)
				return -1;
#ifdef O_NONBLOCK
			flags = fcntl(fd, F_GETFL, 0);
			oldval = (flags & O_NONBLOCK) ? 0 : 1;
			if (value)
				flags ^= O_NONBLOCK;
			else
				flags |= O_NONBLOCK;
			
			if (-1 == fcntl(fd, F_SETFL, flags))
				return -1;
			return oldval;
#else
			return -1; /* not yet implemented */
#endif
			
		case PHP_STREAM_OPTION_BUFFER:
			if (ptrparam)
				size = *(size_t *)ptrparam;
			else
				size = BUFSIZ;

			switch(value) {
				case PHP_STREAM_BUFFER_NONE:
					return setvbuf(data->file, NULL, _IONBF, 0);
					
				case PHP_STREAM_BUFFER_LINE:
					return setvbuf(data->file, NULL, _IOLBF, size);
					
				case PHP_STREAM_BUFFER_FULL:
					return setvbuf(data->file, NULL, _IOFBF, size);

				default:
					return -1;
			}
			break;
		default:
			return -1;
	}
}

PHPAPI php_stream_ops	php_stream_stdio_ops = {
	php_stdiop_write, php_stdiop_read,
	php_stdiop_close, php_stdiop_flush,
	"STDIO",
	php_stdiop_seek,
	php_stdiop_gets, php_stdiop_cast,
	php_stdiop_stat,
	php_stdiop_set_option
};
/* }}} */

/* {{{ php_stream_fopen_with_path */
PHPAPI php_stream *_php_stream_fopen_with_path(char *filename, char *mode, char *path, char **opened_path STREAMS_DC TSRMLS_DC)
{
	/* code ripped off from fopen_wrappers.c */
	char *pathbuf, *ptr, *end;
	char *exec_fname;
	char trypath[MAXPATHLEN];
	struct stat sb;
	php_stream *stream;
	int path_length;
	int filename_length;
	int exec_fname_length;

	if (opened_path) {
		*opened_path = NULL;
	}

	if(!filename) {
		return NULL;
	}

	filename_length = strlen(filename);

	/* Relative path open */
	if (*filename == '.') {
		if (PG(safe_mode) && (!php_checkuid(filename, mode, CHECKUID_CHECK_MODE_PARAM))) {
			return NULL;
		}
		return php_stream_fopen_rel(filename, mode, opened_path);
	}

	/*
	 * files in safe_mode_include_dir (or subdir) are excluded from
	 * safe mode GID/UID checks
	 */

	/* Absolute path open */
	if (IS_ABSOLUTE_PATH(filename, filename_length)) {
		if ((php_check_safe_mode_include_dir(filename TSRMLS_CC)) == 0)
			/* filename is in safe_mode_include_dir (or subdir) */
			return php_stream_fopen_rel(filename, mode, opened_path);

		if (PG(safe_mode) && (!php_checkuid(filename, mode, CHECKUID_CHECK_MODE_PARAM)))
			return NULL;

		return php_stream_fopen_rel(filename, mode, opened_path);
	}

	if (!path || (path && !*path)) {
		if (PG(safe_mode) && (!php_checkuid(filename, mode, CHECKUID_CHECK_MODE_PARAM))) {
			return NULL;
		}
		return php_stream_fopen_rel(filename, mode, opened_path);
	}

	/* check in provided path */
	/* append the calling scripts' current working directory
	 * as a fall back case
	 */
	if (zend_is_executing(TSRMLS_C)) {
		exec_fname = zend_get_executed_filename(TSRMLS_C);
		exec_fname_length = strlen(exec_fname);
		path_length = strlen(path);

		while ((--exec_fname_length >= 0) && !IS_SLASH(exec_fname[exec_fname_length]));
		if ((exec_fname && exec_fname[0] == '[')
				|| exec_fname_length<=0) {
			/* [no active file] or no path */
			pathbuf = estrdup(path);
		} else {
			pathbuf = (char *) emalloc(exec_fname_length + path_length +1 +1);
			memcpy(pathbuf, path, path_length);
			pathbuf[path_length] = DEFAULT_DIR_SEPARATOR;
			memcpy(pathbuf+path_length+1, exec_fname, exec_fname_length);
			pathbuf[path_length + exec_fname_length +1] = '\0';
		}
	} else {
		pathbuf = estrdup(path);
	}

	ptr = pathbuf;

	while (ptr && *ptr) {
		end = strchr(ptr, DEFAULT_DIR_SEPARATOR);
		if (end != NULL) {
			*end = '\0';
			end++;
		}
		snprintf(trypath, MAXPATHLEN, "%s/%s", ptr, filename);
		if (PG(safe_mode)) {
			if (VCWD_STAT(trypath, &sb) == 0) {
				/* file exists ... check permission */
				if ((php_check_safe_mode_include_dir(trypath TSRMLS_CC) == 0) ||
						php_checkuid(trypath, mode, CHECKUID_CHECK_MODE_PARAM)) {
					/* UID ok, or trypath is in safe_mode_include_dir */
					stream = php_stream_fopen_rel(trypath, mode, opened_path);
				} else {
					stream = NULL;
				}

				efree(pathbuf);
				return stream;
			}
		}
		stream = php_stream_fopen_rel(trypath, mode, opened_path);
		if (stream) {
			efree(pathbuf);
			return stream;
		}
		ptr = end;
	} /* end provided path */

	efree(pathbuf);
	return NULL;

}
/* }}} */

/* {{{ php_stream_fopen */
PHPAPI php_stream *_php_stream_fopen(const char *filename, const char *mode, char **opened_path STREAMS_DC TSRMLS_DC)
{
	FILE *fp;
	char *realpath = NULL;

	realpath = expand_filepath(filename, NULL TSRMLS_CC);

	fp = fopen(realpath, mode);

	if (fp)	{
		php_stream *ret = php_stream_fopen_from_file_rel(fp, mode);

		if (ret)	{
			if (opened_path)	{
				*opened_path = realpath;
				realpath = NULL;
			}
			if (realpath)
				efree(realpath);

			return ret;
		}

		fclose(fp);
	}
	efree(realpath);
	return NULL;
}
/* }}} */

/* {{{ STDIO with fopencookie */
#if HAVE_FOPENCOOKIE
static ssize_t stream_cookie_reader(void *cookie, char *buffer, size_t size)
{
	ssize_t ret;
	TSRMLS_FETCH();
	ret = php_stream_read(((php_stream *)cookie), buffer, size);
	return ret;
}

static ssize_t stream_cookie_writer(void *cookie, const char *buffer, size_t size)
{
	TSRMLS_FETCH();
	return php_stream_write(((php_stream *)cookie), (char *)buffer, size);
}

static int stream_cookie_seeker(void *cookie, off_t position, int whence)
{
	TSRMLS_FETCH();

	return php_stream_seek((php_stream *)cookie, position, whence);
}

static int stream_cookie_closer(void *cookie)
{
	php_stream *stream = (php_stream*)cookie;
	TSRMLS_FETCH();

	/* prevent recursion */
	stream->fclose_stdiocast = PHP_STREAM_FCLOSE_NONE;
	return php_stream_close(stream);
}

static COOKIE_IO_FUNCTIONS_T stream_cookie_functions =
{
	stream_cookie_reader, stream_cookie_writer,
	stream_cookie_seeker, stream_cookie_closer
};
#else
/* TODO: use socketpair() to emulate fopencookie, as suggested by Hartmut ? */
#endif
/* }}} */

/* {{{ php_stream_cast */
PHPAPI int _php_stream_cast(php_stream *stream, int castas, void **ret, int show_err TSRMLS_DC)
{
	int flags = castas & PHP_STREAM_CAST_MASK;
	castas &= ~PHP_STREAM_CAST_MASK;

	/* filtered streams can only be cast as stdio, and only when fopencookie is present */
	
	if (castas == PHP_STREAM_AS_STDIO) {
		if (stream->stdiocast) {
			if (ret) {
				*ret = stream->stdiocast;
			}
			goto exit_success;
		}

		/* if the stream is a stdio stream let's give it a chance to respond
		 * first, to avoid doubling up the layers of stdio with an fopencookie */
		if (php_stream_is(stream, PHP_STREAM_IS_STDIO) &&
				stream->ops->cast &&
				stream->filterhead == NULL &&
				stream->ops->cast(stream, castas, ret TSRMLS_CC) == SUCCESS)
		{
			goto exit_success;
		}
		
#if HAVE_FOPENCOOKIE
		/* if just checking, say yes we can be a FILE*, but don't actually create it yet */
		if (ret == NULL)
			goto exit_success;

		*ret = fopencookie(stream, stream->mode, stream_cookie_functions);

		if (*ret != NULL) {
			off_t pos;
			
			stream->fclose_stdiocast = PHP_STREAM_FCLOSE_FOPENCOOKIE;

			/* If the stream position is not at the start, we need to force
			 * the stdio layer to believe it's real location. */
			pos = php_stream_tell(stream);
			if (pos > 0)
				fseek(*ret, pos, SEEK_SET);
			
			goto exit_success;
		}

		/* must be either:
			a) programmer error
			b) no memory
			-> lets bail
		*/
		zend_error(E_ERROR, "%s(): fopencookie failed", get_active_function_name(TSRMLS_C));
		return FAILURE;
#endif

	}

	if (stream->filterhead) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot cast a filtered stream on this system");
		return FAILURE;
	}
	
	if (stream->ops->cast && stream->ops->cast(stream, castas, ret TSRMLS_CC) == SUCCESS)
		goto exit_success;

	if (show_err) {
		/* these names depend on the values of the PHP_STREAM_AS_XXX defines in php_streams.h */
		static const char *cast_names[3] = {
			"STDIO FILE*", "File Descriptor", "Socket Descriptor"
		};

		zend_error(E_WARNING, "%s(): cannot represent a stream of type %s as a %s",
			get_active_function_name(TSRMLS_C),
			stream->ops->label,
			cast_names[castas]
			);
	}

	return FAILURE;

exit_success:
	if (castas == PHP_STREAM_AS_STDIO && ret)
		stream->stdiocast = *ret;
	
	if (flags & PHP_STREAM_CAST_RELEASE) {
		/* Something other than php_stream_close will be closing
		 * the underlying handle, so we should free the stream handle/data
		 * here now.  The stream may not be freed immediately (in the case
		 * of fopencookie), but the caller should still not touch their
		 * original stream pointer in any case. */
		if (stream->fclose_stdiocast != PHP_STREAM_FCLOSE_FOPENCOOKIE) {
			/* ask the implementation to release resources other than
			 * the underlying handle */
			php_stream_free(stream, PHP_STREAM_FREE_PRESERVE_HANDLE | PHP_STREAM_FREE_CLOSE);
		}
	}

	return SUCCESS;

}
/* }}} */

/* {{{ wrapper init and registration */
int php_init_stream_wrappers(TSRMLS_D)
{
	return zend_hash_init(&url_stream_wrappers_hash, 0, NULL, NULL, 1) == SUCCESS && zend_hash_init(&stream_filters_hash, 0, NULL, NULL, 1) == SUCCESS ? SUCCESS : FAILURE;
}

int php_shutdown_stream_wrappers(TSRMLS_D)
{
	zend_hash_destroy(&url_stream_wrappers_hash);
	zend_hash_destroy(&stream_filters_hash);
	return SUCCESS;
}

PHPAPI int php_register_url_stream_wrapper(char *protocol, php_stream_wrapper *wrapper TSRMLS_DC)
{
	return zend_hash_add(&url_stream_wrappers_hash, protocol, strlen(protocol), wrapper, sizeof(*wrapper), NULL);
}

PHPAPI int php_unregister_url_stream_wrapper(char *protocol TSRMLS_DC)
{
	return zend_hash_del(&url_stream_wrappers_hash, protocol, strlen(protocol));
}
/* }}} */


static size_t php_plain_files_dirstream_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	DIR *dir = (DIR*)stream->abstract;
	/* avoid libc5 readdir problems */
	char entry[sizeof(struct dirent)+MAXPATHLEN];
	struct dirent *result = (struct dirent *)&entry;
	php_stream_dirent *ent = (php_stream_dirent*)buf;

	/* avoid problems if someone mis-uses the stream */
	if (count != sizeof(php_stream_dirent))
		return 0;

	if (php_readdir_r(dir, (struct dirent *)entry, &result) == 0 && result) {
		PHP_STRLCPY(ent->d_name, result->d_name, sizeof(ent->d_name), strlen(result->d_name));
		return sizeof(php_stream_dirent);
	}
	return 0;
}

static int php_plain_files_dirstream_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	return closedir((DIR *)stream->abstract);
}

static int php_plain_files_dirstream_rewind(php_stream *stream, off_t offset, int whence TSRMLS_DC)
{
	rewinddir((DIR *)stream->abstract);
	return 0;
}

static php_stream_ops	php_plain_files_dirstream_ops = {
	NULL, php_plain_files_dirstream_read,
	php_plain_files_dirstream_close, NULL,
	"dir",
	php_plain_files_dirstream_rewind,
	NULL, NULL,
	NULL
};

static php_stream *php_plain_files_dir_opener(php_stream_wrapper *wrapper, char *path, char *mode,
		int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	DIR *dir = NULL;
	php_stream *stream = NULL;

	if (php_check_open_basedir(path TSRMLS_CC)) {
		return NULL;
	}
	
	if (PG(safe_mode) &&(!php_checkuid(path, NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		return NULL;
	}
	
	dir = VCWD_OPENDIR(path);

#ifdef PHP_WIN32
	if (dir && dir->finished) {
		closedir(dir);
		dir = NULL;
	}
#endif
	if (dir) {
		stream = php_stream_alloc(&php_plain_files_dirstream_ops, dir, 0, mode);
		if (stream == NULL)
			closedir(dir);
	}
		
	return stream;
}



static php_stream *php_plain_files_stream_opener(php_stream_wrapper *wrapper, char *path, char *mode,
		int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	if ((options & USE_PATH) && PG(include_path) != NULL) {
		return php_stream_fopen_with_path_rel(path, mode, PG(include_path), opened_path);
	}

	if ((options & ENFORCE_SAFE_MODE) && PG(safe_mode) && (!php_checkuid(path, mode, CHECKUID_CHECK_MODE_PARAM)))
		return NULL;

	return php_stream_fopen_rel(path, mode, opened_path);
}

static int php_plain_files_url_stater(php_stream_wrapper *wrapper, char *url, php_stream_statbuf *ssb TSRMLS_DC)
{
	return VCWD_STAT(url, &ssb->sb);
}

static php_stream_wrapper_ops php_plain_files_wrapper_ops = {
	php_plain_files_stream_opener,
	NULL,
	NULL,
	php_plain_files_url_stater,
	php_plain_files_dir_opener
};

static php_stream_wrapper php_plain_files_wrapper = {
	&php_plain_files_wrapper_ops,
	NULL,
	0
};

PHPAPI php_stream_wrapper *php_stream_locate_url_wrapper(const char *path, char **path_for_open, int options TSRMLS_DC)
{
	php_stream_wrapper *wrapper = NULL;
	const char *p, *protocol = NULL;
	int n = 0;

	if (path_for_open)
		*path_for_open = (char*)path;

	if (options & IGNORE_URL)
		return (options & STREAM_LOCATE_WRAPPERS_ONLY) ? NULL : &php_plain_files_wrapper;
	
	for (p = path; isalnum((int)*p) || *p == '+' || *p == '-' || *p == '.'; p++) {
		n++;
	}

	if ((*p == ':') && (n > 1) && !strncmp("://", p, 3)) {
		protocol = path;
	} else if (strncasecmp(path, "zlib:", 5) == 0) {
		/* BC with older php scripts and zlib wrapper */
		protocol = "compress.zlib";
		n = 13;
		zend_error(E_WARNING, "Use of \"zlib:\" wrapper is deprecated; please use \"compress.zlib://\" instead.");
	}

	if (protocol)	{
		if (FAILURE == zend_hash_find(&url_stream_wrappers_hash, (char*)protocol, n, (void**)&wrapper))	{
			char wrapper_name[32];

			if (n >= sizeof(wrapper_name))
				n = sizeof(wrapper_name) - 1;
			PHP_STRLCPY(wrapper_name, protocol, sizeof(wrapper_name), n);
			
			zend_error(E_NOTICE, "Unable to find the wrapper \"%s\" - did you forget to enable it when you configured PHP?",
					wrapper_name);

			wrapper = NULL;
			protocol = NULL;
		}
	}
	/* TODO: curl based streams probably support file:// properly */
	if (!protocol || !strncasecmp(protocol, "file", n))	{
		if (protocol && path[n+1] == '/' && path[n+2] == '/')	{
			if (options & REPORT_ERRORS)
				zend_error(E_WARNING, "remote host file access not supported, %s", path);
			return NULL;
		}
		if (protocol && path_for_open)
			*path_for_open = (char*)path + n + 1;
		
		/* fall back on regular file access */
		return (options & STREAM_LOCATE_WRAPPERS_ONLY) ? NULL : &php_plain_files_wrapper;
	}

	if (wrapper && wrapper->is_url && !PG(allow_url_fopen)) {
		if (options & REPORT_ERRORS)
			zend_error(E_WARNING, "URL file-access is disabled in the server configuration");
		return NULL;
	}
	
	return wrapper;
}

PHPAPI int _php_stream_stat_path(char *path, php_stream_statbuf *ssb TSRMLS_DC)
{
	php_stream_wrapper *wrapper = NULL;
	char *path_to_open = path;

	wrapper = php_stream_locate_url_wrapper(path, &path_to_open, ENFORCE_SAFE_MODE TSRMLS_CC);
	if (wrapper && wrapper->wops->url_stat) {
		return wrapper->wops->url_stat(wrapper, path_to_open, ssb TSRMLS_CC);
	}
	return -1;
}

/* {{{ php_stream_opendir */
PHPAPI php_stream *_php_stream_opendir(char *path, int options,
		php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	php_stream *stream = NULL;
	php_stream_wrapper *wrapper = NULL;
	char *path_to_open;
	
	if (!path || !*path)
		return NULL;

	path_to_open = path;

	wrapper = php_stream_locate_url_wrapper(path, &path_to_open, options TSRMLS_CC);

	if (wrapper && wrapper->wops->dir_opener)	{
		stream = wrapper->wops->dir_opener(wrapper,
				path_to_open, "r", options, NULL,
				context STREAMS_REL_CC TSRMLS_CC);

		if (stream)
			stream->wrapper = wrapper;
	}

	if (stream == NULL && (options & REPORT_ERRORS)) {
		char *tmp = estrdup(path);
		char *msg;

		if (wrapper)
			msg = strerror(errno);
		else
			msg = "no suitable wrapper could be found";
		
		php_strip_url_passwd(tmp);
		zend_error(E_WARNING, "%s(\"%s\") - %s", get_active_function_name(TSRMLS_C), tmp, msg);
		efree(tmp);
	}
	return stream;
}
/* }}} */

PHPAPI php_stream_dirent *_php_stream_readdir(php_stream *dirstream, php_stream_dirent *ent TSRMLS_DC)
{
	if (sizeof(php_stream_dirent) == php_stream_read(dirstream, (char*)ent, sizeof(php_stream_dirent)))
		return ent;
	
	return NULL;
}

PHPAPI void php_stream_wrapper_log_error(php_stream_wrapper *wrapper, int options TSRMLS_DC, const char *fmt, ...)
{
	va_list args;
	char *buffer = NULL;

	va_start(args, fmt);
	vspprintf(&buffer, 0, fmt, args);
	va_end(args);

	if (options & REPORT_ERRORS || wrapper == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, buffer);
		efree(buffer);
	} else {
		/* append to stack */
		wrapper->err_stack = erealloc(wrapper->err_stack, (wrapper->err_count + 1) * sizeof(char *));
		if (wrapper->err_stack)
			wrapper->err_stack[wrapper->err_count++] = buffer;
	}
}

/* {{{ php_stream_open_wrapper_ex */
PHPAPI php_stream *_php_stream_open_wrapper_ex(char *path, char *mode, int options,
		char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	php_stream *stream = NULL;
	php_stream_wrapper *wrapper = NULL;
	char *path_to_open;
#if ZEND_DEBUG
	char *copy_of_path = NULL;
#endif
	
	
	if (opened_path)
		*opened_path = NULL;

	if (!path || !*path)
		return NULL;

	path_to_open = path;

	wrapper = php_stream_locate_url_wrapper(path, &path_to_open, options TSRMLS_CC);

	if (wrapper)	{

		/* prepare error stack */
		wrapper->err_count = 0;
		wrapper->err_stack = NULL;
		
		stream = wrapper->wops->stream_opener(wrapper,
				path_to_open, mode, options ^ REPORT_ERRORS,
				opened_path, context STREAMS_REL_CC TSRMLS_CC);
		if (stream)
			stream->wrapper = wrapper;
	}

#if ZEND_DEBUG
	if (stream) {
		copy_of_path = estrdup(path);
		stream->__orig_path = copy_of_path;
	}
#endif
	
	if (stream != NULL && (options & STREAM_MUST_SEEK)) {
		php_stream *newstream;

		switch(php_stream_make_seekable_rel(stream, &newstream, PHP_STREAM_NO_PREFERENCE)) {
			case PHP_STREAM_UNCHANGED:
				return stream;
			case PHP_STREAM_RELEASED:
#if ZEND_DEBUG
				newstream->__orig_path = copy_of_path;
#endif
				return newstream;
			default:
				php_stream_close(stream);
				stream = NULL;
				if (options & REPORT_ERRORS) {
					char *tmp = estrdup(path);
					php_strip_url_passwd(tmp);
					php_error_docref1(NULL TSRMLS_CC, tmp, E_WARNING, "could not make seekable - %s",
							tmp, strerror(errno));
					efree(tmp);

					options ^= REPORT_ERRORS;
				}
		}
	}
	if (stream == NULL && (options & REPORT_ERRORS)) {
		char *tmp = estrdup(path);
		char *msg;
		int free_msg = 0;

		if (wrapper) {
			if (wrapper->err_count) {
				int i;
				size_t l;
				int brlen;
				char *br;

				if (PG(html_errors)) {
					brlen = 7;
					br = "<br />\n";
				} else {
					brlen = 1;
					br = "\n";
				}

				for (i = 0, l = 0; i < wrapper->err_count; i++) {
					l += strlen(wrapper->err_stack[i]);
					if (i < wrapper->err_count - 1)
						l += brlen;
				}
				msg = emalloc(l + 1);
				msg[0] = '\0';
				for (i = 0; i < wrapper->err_count; i++) {
					strcat(msg, wrapper->err_stack[i]);
					if (i < wrapper->err_count - 1)
						strcat(msg, br);
				}
				
				free_msg = 1;
			} else {
				msg = strerror(errno);
			}
		} else {
			msg = "no suitable wrapper could be found";
		}
		
		php_strip_url_passwd(tmp);
		php_error_docref1(NULL TSRMLS_CC, tmp, E_WARNING, "failed to create stream: %s", msg);
		efree(tmp);
		if (free_msg)
			efree(msg);
	}
	if (wrapper) {
		/* tidy up the error stack */
		int i;

		for (i = 0; i < wrapper->err_count; i++)
			efree(wrapper->err_stack[i]);
		if (wrapper->err_stack)
			efree(wrapper->err_stack);
		wrapper->err_stack = NULL;
	}
#if ZEND_DEBUG
	if (stream == NULL && copy_of_path != NULL)
		efree(copy_of_path);
#endif
	return stream;
}
/* }}} */

/* {{{ php_stream_open_wrapper_as_file */
PHPAPI FILE * _php_stream_open_wrapper_as_file(char *path, char *mode, int options, char **opened_path STREAMS_DC TSRMLS_DC)
{
	FILE *fp = NULL;
	php_stream *stream = NULL;

	stream = php_stream_open_wrapper_rel(path, mode, options|STREAM_WILL_CAST, opened_path);

	if (stream == NULL)
		return NULL;
	
	if (php_stream_cast(stream, PHP_STREAM_AS_STDIO|PHP_STREAM_CAST_TRY_HARD|PHP_STREAM_CAST_RELEASE,
				(void**)&fp, REPORT_ERRORS) == FAILURE)
	{
		php_stream_close(stream);
		if (opened_path && *opened_path)
			efree(*opened_path);
		return NULL;
	}
	return fp;
}
/* }}} */

/* {{{ php_stream_make_seekable */
PHPAPI int _php_stream_make_seekable(php_stream *origstream, php_stream **newstream, int flags STREAMS_DC TSRMLS_DC)
{
	assert(newstream != NULL);

	*newstream = NULL;
	
	if (origstream->ops->seek != NULL) {
		*newstream = origstream;
		return PHP_STREAM_UNCHANGED;
	}
	
	/* Use a tmpfile and copy the old streams contents into it */

	if (flags & PHP_STREAM_PREFER_STDIO)
		*newstream = php_stream_fopen_tmpfile();
	else
		*newstream = php_stream_temp_new();

	if (*newstream == NULL)
		return PHP_STREAM_FAILED;

	if (php_stream_copy_to_stream(origstream, *newstream, PHP_STREAM_COPY_ALL) == 0) {
		php_stream_close(*newstream);
		*newstream = NULL;
		return PHP_STREAM_CRITICAL;
	}

	php_stream_close(origstream);
	php_stream_seek(*newstream, 0, SEEK_SET);
	
	return PHP_STREAM_RELEASED;
}
/* }}} */

PHPAPI php_stream_context *php_stream_context_set(php_stream *stream, php_stream_context *context)
{
	php_stream_context *oldcontext = stream->context;
	stream->context = context;
	return oldcontext;
}

PHPAPI void php_stream_notification_notify(php_stream_context *context, int notifycode, int severity,
		char *xmsg, int xcode, size_t bytes_sofar, size_t bytes_max, void * ptr TSRMLS_DC)
{
	if (context && context->notifier)
		context->notifier->func(context, notifycode, severity, xmsg, xcode, bytes_sofar, bytes_max, ptr TSRMLS_CC);
}

PHPAPI void php_stream_context_free(php_stream_context *context)
{
	zval_ptr_dtor(&context->options);
	efree(context);
}

PHPAPI php_stream_context *php_stream_context_alloc(void)
{
	php_stream_context *context;
	
	context = ecalloc(1, sizeof(php_stream_context));
	MAKE_STD_ZVAL(context->options);
	array_init(context->options);

	return context;
}

PHPAPI php_stream_notifier *php_stream_notification_alloc(void)
{
	return ecalloc(1, sizeof(php_stream_notifier));
}

PHPAPI void php_stream_notification_free(php_stream_notifier *notifier)
{
	efree(notifier);
}

PHPAPI int php_stream_context_get_option(php_stream_context *context,
		const char *wrappername, const char *optionname, zval **optionvalue)
{
	zval **wrapperhash;
	
	if (FAILURE == zend_hash_find(Z_ARRVAL_P(context->options), (char*)wrappername, strlen(wrappername)+1, (void**)&wrapperhash))
		return FAILURE;

	return zend_hash_find(Z_ARRVAL_PP(wrapperhash), (char*)optionname, strlen(optionname)+1, (void**)&optionvalue);
}

PHPAPI int php_stream_context_set_option(php_stream_context *context,
		const char *wrappername, const char *optionname, zval *optionvalue)
{
	zval **wrapperhash;

	if (FAILURE == zend_hash_find(Z_ARRVAL_P(context->options), (char*)wrappername, strlen(wrappername)+1, (void**)&wrapperhash)) {
		
		MAKE_STD_ZVAL(*wrapperhash);
		array_init(*wrapperhash);
		if (FAILURE == zend_hash_update(Z_ARRVAL_P(context->options), (char*)wrappername, strlen(wrappername)+1, (void**)wrapperhash, sizeof(zval *), NULL))
			return FAILURE;

		ZVAL_ADDREF(optionvalue);
	}
	return zend_hash_update(Z_ARRVAL_PP(wrapperhash), (char*)optionname, strlen(optionname)+1, (void**)&optionvalue, sizeof(zval *), NULL);
}

PHPAPI HashTable *php_stream_get_url_stream_wrappers_hash()
{
	return &url_stream_wrappers_hash;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
