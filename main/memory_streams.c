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
   | Author:                                                              |
   | Marcus Boerger <helly@php.net>                                       |
   +----------------------------------------------------------------------+
 */

#define _GNU_SOURCE
#include "php.h"

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
	php_stream_memory_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
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
	php_stream_memory_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);

	if (ms->fpos + count > ms->fsize) {
		count = ms->fsize - ms->fpos;
	}
	if (count) {
		assert(ms->data!= NULL);
		assert(buf!= NULL);
		memcpy(buf, ms->data+ms->fpos, count);
		ms->fpos += count;
	} else {
		stream->eof = 1;
	}
	return count;
}
/* }}} */


/* {{{ */
static int php_stream_memory_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	php_stream_memory_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);

	if (ms->data && close_handle && ms->mode != TEMP_STREAM_READONLY) {
		efree(ms->data);
	}
	efree(ms);
	return 0;
}
/* }}} */


/* {{{ */
static int php_stream_memory_flush(php_stream *stream TSRMLS_DC) {
	/* nothing to do here */
	return 0;
}
/* }}} */


/* {{{ */
static int php_stream_memory_seek(php_stream *stream, off_t offset, int whence, off_t *newoffs TSRMLS_DC)
{
	php_stream_memory_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);

	switch(whence) {
		case SEEK_CUR:
			if (offset < 0) {
				if (ms->fpos < (size_t)(-offset)) {
					ms->fpos = 0;
					/*return EINVAL;*/
				} else {
					ms->fpos = ms->fpos + offset;
				}
			} else {
				if (ms->fpos < (size_t)(offset)) {
					ms->fpos = ms->fsize;
					/*return EINVAL;*/
				} else {
					ms->fpos = ms->fpos + offset;
				}
			}
			*newoffs = ms->fpos;
			return 0;
		case SEEK_SET:
			if (ms->fsize < (size_t)(offset)) {
				ms->fpos = ms->fsize;
				/*return EINVAL;*/
			} else {
				ms->fpos = offset;
			}
			*newoffs = ms->fpos;
			return 0;
		case SEEK_END:
			if (offset > 0) {
				ms->fpos = ms->fsize;
				/*return EINVAL;*/
			} else if (ms->fpos < (size_t)(-offset)) {
				ms->fpos = 0;
				/*return EINVAL;*/
			} else {
				ms->fpos = ms->fsize + offset;
			}
			*newoffs = ms->fpos;
			return 0;
		default:
			return 0;
			/*return EINVAL;*/
	}
}
/* }}} */

/* {{{ */
static int php_stream_memory_cast(php_stream *stream, int castas, void **ret TSRMLS_DC)
{
	return FAILURE;
}
/* }}} */


php_stream_ops	php_stream_memory_ops = {
	php_stream_memory_write, php_stream_memory_read,
	php_stream_memory_close, php_stream_memory_flush,
	"MEMORY",
	php_stream_memory_seek,
	php_stream_memory_cast,
	NULL, /* stat */
	NULL  /* set_option */
};


/* {{{ */
PHPAPI php_stream *_php_stream_memory_create(int mode STREAMS_DC TSRMLS_DC)
{
	php_stream_memory_data *self;
	php_stream *stream;

	self = emalloc(sizeof(*self));
	assert(self != NULL);
	self->data = NULL;
	self->fpos = 0;
	self->fsize = 0;
	self->smax = -1;
	self->mode = mode;
	
	stream = php_stream_alloc(&php_stream_memory_ops, self, 0, "rwb");
	stream->flags |= PHP_STREAM_FLAG_NO_BUFFER;
	return stream;
}
/* }}} */


/* {{{ */
PHPAPI php_stream *_php_stream_memory_open(int mode, char *buf, size_t length STREAMS_DC TSRMLS_DC)
{	php_stream *stream;
	php_stream_memory_data *ms;

	if ((stream = php_stream_memory_create_rel(mode)) != NULL) {
		ms = stream->abstract;
		
		if (mode == TEMP_STREAM_READONLY) {
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
	php_stream_memory_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
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
} php_stream_temp_data;


/* {{{ */
static size_t php_stream_temp_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	php_stream_temp_data *ts;

	assert(stream != NULL);
	ts = stream->abstract;
	assert(ts != NULL);

	if (php_stream_is(ts->innerstream, PHP_STREAM_IS_MEMORY)) {
		size_t memsize;
		char *membuf = php_stream_memory_get_buffer(ts->innerstream, &memsize);

		if (memsize + count >= ts->smax) {
			php_stream *file = php_stream_fopen_tmpfile();
			php_stream_write(file, membuf, memsize);
			php_stream_close(ts->innerstream);
			ts->innerstream = file;
		}
	}
	return php_stream_write(ts->innerstream, buf, count);
}
/* }}} */


/* {{{ */
static size_t php_stream_temp_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	php_stream_temp_data *ts;

	assert(stream != NULL);
	ts = stream->abstract;
	assert(ts != NULL);

	return php_stream_read(ts->innerstream, buf, count);
}
/* }}} */


/* {{{ */
static int php_stream_temp_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	php_stream_temp_data *ts;
	int ret;

	assert(stream != NULL);
	ts = stream->abstract;
	assert(ts != NULL);

	ret = php_stream_free(ts->innerstream, PHP_STREAM_FREE_CLOSE | (close_handle ? 0 : PHP_STREAM_FREE_PRESERVE_HANDLE));

	efree(ts);

	return ret;
}
/* }}} */


/* {{{ */
static int php_stream_temp_flush(php_stream *stream TSRMLS_DC)
{
	php_stream_temp_data *ts;

	assert(stream != NULL);
	ts = stream->abstract;
	assert(ts != NULL);

	return php_stream_flush(ts->innerstream);
}
/* }}} */


/* {{{ */
static int php_stream_temp_seek(php_stream *stream, off_t offset, int whence, off_t *newoffs TSRMLS_DC)
{
	php_stream_temp_data *ts;
	int ret;

	assert(stream != NULL);
	ts = stream->abstract;
	assert(ts != NULL);

	ret = php_stream_seek(ts->innerstream, offset, whence);
	*newoffs = php_stream_tell(ts->innerstream);
	
	return ret;
}
/* }}} */

/* {{{ */
static int php_stream_temp_cast(php_stream *stream, int castas, void **ret TSRMLS_DC)
{
	php_stream_temp_data *ts;
	php_stream *file;
	size_t memsize;
	char *membuf;
	off_t pos;

	assert(stream != NULL);
	ts = stream->abstract;
	assert(ts != NULL);

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

	php_stream_close(ts->innerstream);
	ts->innerstream = file;
	php_stream_seek(ts->innerstream, pos, SEEK_SET);

	return php_stream_cast(ts->innerstream, castas, ret, 1);
}
/* }}} */

php_stream_ops	php_stream_temp_ops = {
	php_stream_temp_write, php_stream_temp_read,
	php_stream_temp_close, php_stream_temp_flush,
	"TEMP",
	php_stream_temp_seek,
	php_stream_temp_cast,
	NULL, /* stat */
	NULL /* set_option */
};


/* {{{ */
PHPAPI php_stream *_php_stream_temp_create(int mode, size_t max_memory_usage STREAMS_DC TSRMLS_DC)
{
	php_stream_temp_data *self;
	php_stream *stream;

	self = ecalloc(1, sizeof(*self));
	assert(self != NULL);
	self->smax = max_memory_usage;
	self->mode = mode;
	stream = php_stream_alloc(&php_stream_temp_ops, self, 0, "rwb");
	stream->flags |= PHP_STREAM_FLAG_NO_BUFFER;
	self->innerstream = php_stream_memory_create(mode);

	return stream;
}
/* }}} */


/* {{{ */
PHPAPI php_stream *_php_stream_temp_open(int mode, size_t max_memory_usage, char *buf, size_t length STREAMS_DC TSRMLS_DC)
{
	php_stream *stream;
	php_stream_temp_data *ms;

	if ((stream = php_stream_temp_create_rel(mode & ~TEMP_STREAM_READONLY, max_memory_usage)) != NULL) {
		if (length) {
			assert(buf != NULL);
			php_stream_temp_write(stream, buf, length TSRMLS_CC);
		}
		ms = stream->abstract;
		assert(ms != NULL);
		ms->mode = mode;
	}
	return stream;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
