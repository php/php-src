/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#define _GNU_SOURCE
#include "php.h"

PHPAPI int php_url_decode(char *str, int len);
PHPAPI unsigned char *php_base64_decode(const unsigned char *str, int length, int *ret_length);

/* Memory streams use a dynamic memory buffer to emulate a stream.
 * You can use php_stream_memory_open to create a readonly stream
 * from an existing memory buffer.
 */

/* Temp streams are streams that uses memory streams as long their
 * size is less than a given memory amount. When a write operation
 * exceeds that limit the content is written to a temporary file.
 */

/* {{{ ------- MEMORY stream implementation -------*/

typedef struct {
	char        *data;
	size_t      fpos;
	size_t      fsize;
	size_t      smax;
	int			mode;
} php_stream_memory_data;


/* {{{ */
static size_t php_stream_memory_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	php_stream_memory_data *ms = (php_stream_memory_data*)stream->abstract;
	assert(ms != NULL);

	if (ms->mode & TEMP_STREAM_READONLY) {
		return 0;
	}
	if (ms->fpos + count > ms->fsize) {
		char *tmp;

		if (!ms->data) {
			tmp = emalloc(ms->fpos + count);
		} else {
			tmp = erealloc(ms->data, ms->fpos + count);
		}
		if (!tmp) {
			count = ms->fsize - ms->fpos + 1;
		} else {
			ms->data = tmp;
			ms->fsize = ms->fpos + count;
		}
	}
	if (!ms->data)
		count = 0;
	if (count) {
		assert(buf!= NULL);
		memcpy(ms->data+ms->fpos, (char*)buf, count);
		ms->fpos += count;
	}
	return count;
}
/* }}} */


/* {{{ */
static size_t php_stream_memory_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	php_stream_memory_data *ms = (php_stream_memory_data*)stream->abstract;
	assert(ms != NULL);

	if (ms->fpos + count >= ms->fsize) {
		count = ms->fsize - ms->fpos;
		stream->eof = 1;
	}
	if (count) {
		assert(ms->data!= NULL);
		assert(buf!= NULL);
		memcpy(buf, ms->data+ms->fpos, count);
		ms->fpos += count;
	}
	return count;
}
/* }}} */


/* {{{ */
static int php_stream_memory_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	php_stream_memory_data *ms = (php_stream_memory_data*)stream->abstract;
	assert(ms != NULL);

	if (ms->data && close_handle && ms->mode != TEMP_STREAM_READONLY) {
		efree(ms->data);
	}
	efree(ms);
	return 0;
}
/* }}} */


/* {{{ */
static int php_stream_memory_flush(php_stream *stream TSRMLS_DC)
{
	/* nothing to do here */
	return 0;
}
/* }}} */


/* {{{ */
static int php_stream_memory_seek(php_stream *stream, off_t offset, int whence, off_t *newoffs TSRMLS_DC)
{
	php_stream_memory_data *ms = (php_stream_memory_data*)stream->abstract;
	assert(ms != NULL);

	switch(whence) {
		case SEEK_CUR:
			if (offset < 0) {
				if (ms->fpos < (size_t)(-offset)) {
					ms->fpos = 0;
					*newoffs = -1;
					return -1;
				} else {
					ms->fpos = ms->fpos + offset;
					*newoffs = ms->fpos;
					stream->eof = 0;
					return 0;
				}
			} else {
				if (ms->fpos + (size_t)(offset) > ms->fsize) {
					ms->fpos = ms->fsize;
					*newoffs = -1;
					return -1;
				} else {
					ms->fpos = ms->fpos + offset;
					*newoffs = ms->fpos;
					stream->eof = 0;
					return 0;
				}
			}
		case SEEK_SET:
			if (ms->fsize < (size_t)(offset)) {
				ms->fpos = ms->fsize;
				*newoffs = -1;
				return -1;
			} else {
				ms->fpos = offset;
				*newoffs = ms->fpos;
				stream->eof = 0;
				return 0;
			}
		case SEEK_END:
			if (offset > 0) {
				ms->fpos = ms->fsize;
				*newoffs = -1;
				return -1;
			} else if (ms->fsize < (size_t)(-offset)) {
				ms->fpos = 0;
				*newoffs = -1;
				return -1;
			} else {
				ms->fpos = ms->fsize + offset;
				*newoffs = ms->fpos;
				stream->eof = 0;
				return 0;
			}
		default:
			*newoffs = ms->fpos;
			return -1;
	}
}
/* }}} */

/* {{{ */
static int php_stream_memory_cast(php_stream *stream, int castas, void **ret TSRMLS_DC)
{
	return FAILURE;
}
/* }}} */

static int php_stream_memory_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC) /* {{{ */
{
	time_t timestamp = 0;
	php_stream_memory_data *ms = (php_stream_memory_data*)stream->abstract;
	assert(ms != NULL);

	memset(ssb, 0, sizeof(php_stream_statbuf));
	/* read-only across the board */
	
	ssb->sb.st_mode = ms->mode & TEMP_STREAM_READONLY ? 0444 : 0666;

	ssb->sb.st_size = ms->fsize;
	ssb->sb.st_mode |= S_IFREG; /* regular file */

#ifdef NETWARE
	ssb->sb.st_mtime.tv_sec = timestamp;
	ssb->sb.st_atime.tv_sec = timestamp;
	ssb->sb.st_ctime.tv_sec = timestamp;
#else
	ssb->sb.st_mtime = timestamp;
	ssb->sb.st_atime = timestamp;
	ssb->sb.st_ctime = timestamp;
#endif

	ssb->sb.st_nlink = 1;
	ssb->sb.st_rdev = -1;
	/* this is only for APC, so use /dev/null device - no chance of conflict there! */
	ssb->sb.st_dev = 0xC;
	/* generate unique inode number for alias/filename, so no phars will conflict */
	ssb->sb.st_ino = 0;

#ifndef PHP_WIN32
	ssb->sb.st_blksize = -1;
#endif

#if !defined(PHP_WIN32) && !defined(__BEOS__)
	ssb->sb.st_blocks = -1;
#endif

	return 0;
}
/* }}} */

static int php_stream_memory_set_option(php_stream *stream, int option, int value, void *ptrparam TSRMLS_DC) /* {{{ */
{
	php_stream_memory_data *ms = (php_stream_memory_data*)stream->abstract;
	size_t newsize;
	
	switch(option) {
		case PHP_STREAM_OPTION_TRUNCATE_API:
			switch (value) {
				case PHP_STREAM_TRUNCATE_SUPPORTED:
					return PHP_STREAM_OPTION_RETURN_OK;

				case PHP_STREAM_TRUNCATE_SET_SIZE:
					if (ms->mode & TEMP_STREAM_READONLY) {
						return PHP_STREAM_OPTION_RETURN_ERR;
					}
					newsize = *(size_t*)ptrparam;
					if (newsize <= ms->fsize) {
						if (newsize < ms->fpos) {
							ms->fpos = newsize;
						}
					} else {
						ms->data = erealloc(ms->data, newsize);
						memset(ms->data+ms->fsize, 0, newsize - ms->fsize);
						ms->fsize = newsize;
					}
					ms->fsize = newsize;
					return PHP_STREAM_OPTION_RETURN_OK;
			}
		default:
			return PHP_STREAM_OPTION_RETURN_NOTIMPL;
	}
}
/* }}} */
	
PHPAPI php_stream_ops	php_stream_memory_ops = {
	php_stream_memory_write, php_stream_memory_read,
	php_stream_memory_close, php_stream_memory_flush,
	"MEMORY",
	php_stream_memory_seek,
	php_stream_memory_cast,
	php_stream_memory_stat,
	php_stream_memory_set_option
};


/* {{{ */
PHPAPI php_stream *_php_stream_memory_create(int mode STREAMS_DC TSRMLS_DC)
{
	php_stream_memory_data *self;
	php_stream *stream;

	self = emalloc(sizeof(*self));
	self->data = NULL;
	self->fpos = 0;
	self->fsize = 0;
	self->smax = ~0u;
	self->mode = mode;
	
	stream = php_stream_alloc_rel(&php_stream_memory_ops, self, 0, mode & TEMP_STREAM_READONLY ? "rb" : "w+b");
	stream->flags |= PHP_STREAM_FLAG_NO_BUFFER;
	return stream;
}
/* }}} */


/* {{{ */
PHPAPI php_stream *_php_stream_memory_open(int mode, char *buf, size_t length STREAMS_DC TSRMLS_DC)
{
	php_stream *stream;
	php_stream_memory_data *ms;

	if ((stream = php_stream_memory_create_rel(mode)) != NULL) {
		ms = (php_stream_memory_data*)stream->abstract;
		
		if (mode == TEMP_STREAM_READONLY || mode == TEMP_STREAM_TAKE_BUFFER) {
			/* use the buffer directly */
			ms->data = buf;
			ms->fsize = length;
		} else {
			if (length) {
				assert(buf != NULL);
				php_stream_write(stream, buf, length);
			}
		}
	}
	return stream;
}
/* }}} */


/* {{{ */
PHPAPI char *_php_stream_memory_get_buffer(php_stream *stream, size_t *length STREAMS_DC TSRMLS_DC)
{
	php_stream_memory_data *ms = (php_stream_memory_data*)stream->abstract;

	assert(ms != NULL);
	assert(length != 0);

	*length = ms->fsize;
	return ms->data;
}
/* }}} */

/* }}} */

/* {{{ ------- TEMP stream implementation -------*/

typedef struct {
	php_stream  *innerstream;
	size_t      smax;
	int			mode;
	zval*       meta;
} php_stream_temp_data;


/* {{{ */
static size_t php_stream_temp_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	php_stream_temp_data *ts = (php_stream_temp_data*)stream->abstract;
	assert(ts != NULL);

	if (!ts->innerstream) {
		return -1;
	}
	if (php_stream_is(ts->innerstream, PHP_STREAM_IS_MEMORY)) {
		size_t memsize;
		char *membuf = php_stream_memory_get_buffer(ts->innerstream, &memsize);

		if (memsize + count >= ts->smax) {
			php_stream *file = php_stream_fopen_tmpfile();
			php_stream_write(file, membuf, memsize);
			php_stream_free_enclosed(ts->innerstream, PHP_STREAM_FREE_CLOSE);
			ts->innerstream = file;
			php_stream_encloses(stream, ts->innerstream);
		}
	}
	return php_stream_write(ts->innerstream, buf, count);
}
/* }}} */


/* {{{ */
static size_t php_stream_temp_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	php_stream_temp_data *ts = (php_stream_temp_data*)stream->abstract;
	size_t got;

	assert(ts != NULL);

	if (!ts->innerstream) {
		return -1;
	}
	
	got = php_stream_read(ts->innerstream, buf, count);
	
	stream->eof = ts->innerstream->eof;
	
	return got;
}
/* }}} */


/* {{{ */
static int php_stream_temp_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	php_stream_temp_data *ts = (php_stream_temp_data*)stream->abstract;
	int ret;

	assert(ts != NULL);

	if (ts->innerstream) {
		ret = php_stream_free_enclosed(ts->innerstream, PHP_STREAM_FREE_CLOSE | (close_handle ? 0 : PHP_STREAM_FREE_PRESERVE_HANDLE));
	} else {
		ret = 0;
	}
	
	if (ts->meta) {
		zval_ptr_dtor(&ts->meta);
	}

	efree(ts);

	return ret;
}
/* }}} */


/* {{{ */
static int php_stream_temp_flush(php_stream *stream TSRMLS_DC)
{
	php_stream_temp_data *ts = (php_stream_temp_data*)stream->abstract;
	assert(ts != NULL);

	return ts->innerstream ? php_stream_flush(ts->innerstream) : -1;
}
/* }}} */


/* {{{ */
static int php_stream_temp_seek(php_stream *stream, off_t offset, int whence, off_t *newoffs TSRMLS_DC)
{
	php_stream_temp_data *ts = (php_stream_temp_data*)stream->abstract;
	int ret;

	assert(ts != NULL);

	if (!ts->innerstream) {
		*newoffs = -1;
		return -1;
	}
	ret = php_stream_seek(ts->innerstream, offset, whence);
	*newoffs = php_stream_tell(ts->innerstream);
	stream->eof = ts->innerstream->eof;
	
	return ret;
}
/* }}} */

/* {{{ */
static int php_stream_temp_cast(php_stream *stream, int castas, void **ret TSRMLS_DC)
{
	php_stream_temp_data *ts = (php_stream_temp_data*)stream->abstract;
	php_stream *file;
	size_t memsize;
	char *membuf;
	off_t pos;

	assert(ts != NULL);

	if (!ts->innerstream) {
		return FAILURE;
	}
	if (php_stream_is(ts->innerstream, PHP_STREAM_IS_STDIO)) {
		return php_stream_cast(ts->innerstream, castas, ret, 0);
	}

	/* we are still using a memory based backing. If they are if we can be
	 * a FILE*, say yes because we can perform the conversion.
	 * If they actually want to perform the conversion, we need to switch
	 * the memory stream to a tmpfile stream */

	if (ret == NULL && castas == PHP_STREAM_AS_STDIO) {
		return SUCCESS;
	}

	/* say "no" to other stream forms */
	if (ret == NULL) {
		return FAILURE;
	}

	/* perform the conversion and then pass the request on to the innerstream */
	membuf = php_stream_memory_get_buffer(ts->innerstream, &memsize);
	file = php_stream_fopen_tmpfile();
	php_stream_write(file, membuf, memsize);
	pos = php_stream_tell(ts->innerstream);
	
	php_stream_free_enclosed(ts->innerstream, PHP_STREAM_FREE_CLOSE);
	ts->innerstream = file;
	php_stream_encloses(stream, ts->innerstream);
	php_stream_seek(ts->innerstream, pos, SEEK_SET);

	return php_stream_cast(ts->innerstream, castas, ret, 1);
}
/* }}} */

static int php_stream_temp_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC) /* {{{ */
{
	php_stream_temp_data *ts = (php_stream_temp_data*)stream->abstract;

	if (!ts || !ts->innerstream) {
		return -1;
	}
	return php_stream_stat(ts->innerstream, ssb);
}
/* }}} */

static int php_stream_temp_set_option(php_stream *stream, int option, int value, void *ptrparam TSRMLS_DC) /* {{{ */
{
	php_stream_temp_data *ts = (php_stream_temp_data*)stream->abstract;
	
	switch(option) {
		case PHP_STREAM_OPTION_META_DATA_API:
			if (ts->meta) {
				zend_hash_copy(Z_ARRVAL_P((zval*)ptrparam), Z_ARRVAL_P(ts->meta), (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval*));
			}
			return PHP_STREAM_OPTION_RETURN_OK;
		default:
			if (ts->innerstream) {
				return php_stream_set_option(ts->innerstream, option, value, ptrparam);
			}
			return PHP_STREAM_OPTION_RETURN_NOTIMPL;
	}
}
/* }}} */

PHPAPI php_stream_ops	php_stream_temp_ops = {
	php_stream_temp_write, php_stream_temp_read,
	php_stream_temp_close, php_stream_temp_flush,
	"TEMP",
	php_stream_temp_seek,
	php_stream_temp_cast,
	php_stream_temp_stat,
	php_stream_temp_set_option
};

/* }}} */

/* {{{ _php_stream_temp_create */
PHPAPI php_stream *_php_stream_temp_create(int mode, size_t max_memory_usage STREAMS_DC TSRMLS_DC)
{
	php_stream_temp_data *self;
	php_stream *stream;

	self = ecalloc(1, sizeof(*self));
	self->smax = max_memory_usage;
	self->mode = mode;
	self->meta = NULL;
	stream = php_stream_alloc_rel(&php_stream_temp_ops, self, 0, mode & TEMP_STREAM_READONLY ? "rb" : "w+b");
	stream->flags |= PHP_STREAM_FLAG_NO_BUFFER;
	self->innerstream = php_stream_memory_create_rel(mode);
	php_stream_encloses(stream, self->innerstream);

	return stream;
}
/* }}} */


/* {{{ _php_stream_temp_open */
PHPAPI php_stream *_php_stream_temp_open(int mode, size_t max_memory_usage, char *buf, size_t length STREAMS_DC TSRMLS_DC)
{
	php_stream *stream;
	php_stream_temp_data *ts;
	off_t newoffs;

	if ((stream = php_stream_temp_create_rel(mode, max_memory_usage)) != NULL) {
		if (length) {
			assert(buf != NULL);
			php_stream_temp_write(stream, buf, length TSRMLS_CC);
			php_stream_temp_seek(stream, 0, SEEK_SET, &newoffs TSRMLS_CC);
		}
		ts = (php_stream_temp_data*)stream->abstract;
		assert(ts != NULL);
		ts->mode = mode;
	}
	return stream;
}
/* }}} */

PHPAPI php_stream_ops php_stream_rfc2397_ops = {
	php_stream_temp_write, php_stream_temp_read,
	php_stream_temp_close, php_stream_temp_flush,
	"RFC2397",
	php_stream_temp_seek,
	php_stream_temp_cast,
	php_stream_temp_stat,
	php_stream_temp_set_option
};

static php_stream * php_stream_url_wrap_rfc2397(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC) /* {{{ */
{
	php_stream *stream;
	php_stream_temp_data *ts;
	char *comma, *semi, *sep, *key;
	size_t mlen, dlen, plen, vlen;
	off_t newoffs;
	zval *meta = NULL;
	int base64 = 0, ilen;

	if (memcmp(path, "data:", 5)) {
		return NULL;
	}

	path += 5;
	dlen = strlen(path);

	if (dlen >= 2 && path[0] == '/' && path[1] == '/') {
		dlen -= 2;
		path += 2;
	}

	if ((comma = memchr(path, ',', dlen)) == NULL) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "rfc2397: no comma in URL");
		return NULL;
	}

	if (comma != path) {
		/* meta info */
		mlen = comma - path;
		dlen -= mlen;
		semi = memchr(path, ';', mlen);
		sep = memchr(path, '/', mlen);
		
		if (!semi && !sep) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "rfc2397: illegal media type");
			return NULL;
		}

		MAKE_STD_ZVAL(meta);
		array_init(meta);
		if (!semi) { /* there is only a mime type */
			add_assoc_stringl(meta, "mediatype", path, mlen, 1);
			mlen = 0;
		} else if (sep && sep < semi) { /* there is a mime type */
			plen = semi - path;
			add_assoc_stringl(meta, "mediatype", path, plen, 1);
			mlen -= plen;
			path += plen;
		} else if (semi != path || mlen != sizeof(";base64")-1 || memcmp(path, ";base64", sizeof(";base64")-1)) { /* must be error since parameters are only allowed after mediatype */
			zval_ptr_dtor(&meta);
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "rfc2397: illegal media type");
			return NULL;
		}
		/* get parameters and potentially ';base64' */
		while(semi && (semi == path)) {
			path++;
			mlen--;
			sep = memchr(path, '=', mlen);
			semi = memchr(path, ';', mlen);
			if (!sep || (semi && semi < sep)) { /* must be ';base64' or failure */
				if (mlen != sizeof("base64")-1 || memcmp(path, "base64", sizeof("base64")-1)) {
					/* must be error since parameters are only allowed after mediatype and we have no '=' sign */
					zval_ptr_dtor(&meta);
					php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "rfc2397: illegal parameter");
					return NULL;
				}
				base64 = 1;
				mlen -= sizeof("base64") - 1;
				path += sizeof("base64") - 1;
				break;
			}
			/* found parameter ... the heart of cs ppl lies in +1/-1 or was it +2 this time? */
			plen = sep - path;
			vlen = (semi ? semi - sep : mlen - plen) - 1 /* '=' */;
			key = estrndup(path, plen);
			add_assoc_stringl_ex(meta, key, plen + 1, sep + 1, vlen, 1);
			efree(key);
			plen += vlen + 1;
			mlen -= plen;
			path += plen;
		}
		if (mlen) {
			zval_ptr_dtor(&meta);
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "rfc2397: illegal URL");
			return NULL;
		}
	} else {
		MAKE_STD_ZVAL(meta);
		array_init(meta);
	}
	add_assoc_bool(meta, "base64", base64);

	/* skip ',' */
	comma++;
	dlen--;

	if (base64) {
		comma = (char*)php_base64_decode((const unsigned char *)comma, dlen, &ilen);
		if (!comma) {
			zval_ptr_dtor(&meta);
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "rfc2397: unable to decode");
			return NULL;
		}
	} else {
		comma = estrndup(comma, dlen);
		ilen = dlen = php_url_decode(comma, dlen);
	}

	if ((stream = php_stream_temp_create_rel(0, ~0u)) != NULL) {
		/* store data */
		php_stream_temp_write(stream, comma, ilen TSRMLS_CC);
		php_stream_temp_seek(stream, 0, SEEK_SET, &newoffs TSRMLS_CC);
		/* set special stream stuff (enforce exact mode) */
		vlen = strlen(mode);
		if (vlen >= sizeof(stream->mode)) {
			vlen = sizeof(stream->mode) - 1;
		}
		memcpy(stream->mode, mode, vlen);
		stream->mode[vlen] = '\0';
		stream->ops = &php_stream_rfc2397_ops;
		ts = (php_stream_temp_data*)stream->abstract;
		assert(ts != NULL);
		ts->mode = mode && mode[0] == 'r' && mode[1] != '+' ? TEMP_STREAM_READONLY : 0;
		ts->meta = meta;
	}
	efree(comma);

	return stream;
}

PHPAPI php_stream_wrapper_ops php_stream_rfc2397_wops = {
	php_stream_url_wrap_rfc2397,
	NULL, /* close */
	NULL, /* fstat */
	NULL, /* stat */
	NULL, /* opendir */
	"RFC2397",
	NULL, /* unlink */
	NULL, /* rename */
	NULL, /* mkdir */
	NULL  /* rmdir */
};

PHPAPI php_stream_wrapper php_stream_rfc2397_wrapper =	{
	&php_stream_rfc2397_wops,
	NULL,
	1, /* is_url */
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
