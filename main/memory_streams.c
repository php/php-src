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
#include "php_streams.h"

/* {{{ ------- MEMORY stream implementation -------*/

typedef struct {
	char        *data;
	size_t      fpos;
	size_t      fsize;
	size_t      smax;
	php_stream  *file;
	int			mode;
} php_stream_temp_data;

static size_t php_stream_memory_write(php_stream *stream, const char *buf, size_t count STREAMS_DC) { /* {{{ */
	php_stream_temp_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);

	if ( ms->mode & TEMP_STREAM_READONLY) {
		return 0;
	}
	if ( ms->fpos + count > ms->fsize) {
		char *tmp;

		if ( !ms->data) {
			tmp = emalloc( ms->fpos + count);
		} else {
			tmp = erealloc( ms->data, ms->fpos + count);
		}
		if ( !tmp) {
			count = ms->fsize - ms->fpos + 1;
		} else {
			ms->data = tmp;
			ms->fsize = ms->fpos + count;
		}
	}
	if ( !ms->data)
	  count = 0;
	if ( count) {
		assert(buf!= NULL);
		memcpy( ms->data+ms->fpos, (char*)buf, count);
		ms->fpos += count;
	}
	return count;
} /* }}} */

static size_t php_stream_memory_read(php_stream *stream, char *buf, size_t count STREAMS_DC) { /* {{{ */
	php_stream_temp_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);

	if (buf == NULL && count == 0)	{
		/* check for EOF condition */
		if (ms->fpos >= ms->fsize)	{
			return EOF;
		}
		return 0;
	}

	if ( ms->fpos + count > ms->fsize) {
		count = ms->fsize - ms->fpos;
	}
	if ( count) {
		assert(data!= NULL);
		assert(buf!= NULL);
		memcpy( buf, ms->data+ms->fpos, count);
		ms->fpos += count;
	}
	return count;
} /* }}} */


static int php_stream_memory_close(php_stream *stream, int close_handle STREAMS_DC) { /* {{{ */
	php_stream_temp_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);

	if (ms->data) {
		efree(ms->data);
	}
	ms->data = NULL;
	ms->fsize = 0;
	ms->fpos = 0;
	return 0;
} /* }}} */


static int php_stream_memory_flush(php_stream *stream STREAMS_DC) { /* {{{ */
	// nothing to do here
	return 0;
} /* }}} */


static int php_stream_memory_seek(php_stream *stream, off_t offset, int whence STREAMS_DC) { /* {{{ */
	php_stream_temp_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);

	if (offset == 0 && whence == SEEK_CUR)
		return ms->fpos;
	switch( whence) {
		case SEEK_CUR:
			if ( offset < 0) {
				if ( ms->fpos < -offset) {
					ms->fpos = 0;
					/*return EINVAL;*/
				} else {
					ms->fpos = ms->fpos + offset;
				}
			} else {
				if ( ms->fpos < offset) {
					ms->fpos = ms->fsize;
					/*return EINVAL;*/
				} else {
					ms->fpos = ms->fpos + offset;
				}
			}
			return 0;
		case SEEK_SET:
			if ( offset > ms->fsize) {
				ms->fpos = ms->fsize;
				/*return EINVAL;*/
			} else {
				ms->fpos = offset;
			}
			return 0;
		case SEEK_END:
			if ( offset > 0) {
				ms->fpos = ms->fsize;
				/*return EINVAL;*/
			} else if ( ms->fpos < -offset) {
				ms->fpos = 0;
				/*return EINVAL;*/
			} else {
				ms->fpos = ms->fsize + offset;
			}
			return 0;
		default:
			return 0;
			/*return EINVAL;*/
	}
} /* }}} */

static char *php_stream_memory_gets(php_stream *stream, char *buf, size_t maxlen STREAMS_DC) { /* {{{ */
	size_t n = 1;
	char *c = buf;

	php_stream_temp_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);
	assert(buf!= NULL);
	assert(ms->data!= NULL);

	while(n < maxlen && ms->fpos<ms->fsize) {
		n++;
		if ( (*c = ms->data[ms->fpos++]) == '\n') {
			c++;
			break;
		}
		c++;
	}
	*c = 0;
	return buf;
} /* }}} */

static int php_stream_memory_cast(php_stream *stream, int castas, void **ret STREAMS_DC) { /* {{{ */
	return FAILURE;
} /* }}} */

php_stream_ops	php_stream_memory_ops = {
	php_stream_memory_write, php_stream_memory_read,
	php_stream_memory_close, php_stream_memory_flush,
	php_stream_memory_seek,
	php_stream_memory_gets,
	php_stream_memory_cast,
	"MEMORY"
};

PHPAPI php_stream *_php_stream_memory_create(int mode STREAMS_DC) { /* {{{ */
	php_stream_temp_data *self;

	self = emalloc(sizeof(*self));
	assert( self != NULL);
	self->data = NULL;
	self->fpos = 0;
	self->fsize = 0;
	self->smax = -1;
	self->mode = mode;
	return _php_stream_alloc(&php_stream_memory_ops, self, 0, "rwb" STREAMS_REL_CC);
} /* }}} */

/* }}} */

PHPAPI php_stream *_php_stream_memory_open(int mode, char *buf, size_t length STREAMS_DC) { /* {{{ */
	php_stream *stream;
	php_stream_temp_data *ms;

	if ( (stream = _php_stream_memory_create(mode STREAMS_REL_CC)) != NULL) {
		if ( length) {
			ms = stream->abstract;
			ms->data = emalloc(length);
			assert( buf != NULL);
			memcpy(ms->data,buf,length);
		}
		ms->mode = mode;
	}
	return stream;
} /* }}} */

PHPAPI char *_php_stream_memory_get_buffer(php_stream *stream, size_t *length STREAMS_DC) { /* {{{ */
	php_stream_temp_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);
	assert(length != 0);

	*length = ms->fsize;
	return ms->data;
} /* }}} */

/* {{{ ------- TEMP stream implementation -------*/

static size_t php_stream_temp_write(php_stream *stream, const char *buf, size_t count STREAMS_DC) { /* {{{ */
	php_stream_temp_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);

	if ( ms->mode & TEMP_STREAM_READONLY) {
		return 0;
	}
	if ( !ms->file) {
		size_t	ret, fpos;
		php_stream	*file;

		ret = php_stream_memory_write(stream,buf,count STREAMS_REL_CC);
		if ( ms->fsize > ms->smax) {
			fpos = ms->fpos;
			ms->fpos = 0;
			file = php_stream_fopen_tmpfile();
			php_stream_copy_to_stream(stream, file, PHP_STREAM_COPY_ALL STREAMS_REL_CC);
			php_stream_seek(file,fpos,SEEK_SET STREAMS_REL_CC);
			ms->file = file;
			ms->fsize = 0;
			if ( ms->data) {
				efree(ms->data);
			}
			ms->data = NULL;
		}
		return ret;
	} else {
		return php_stream_write(ms->file,buf,count STREAMS_REL_CC);
	}
} /* }}} */

static size_t php_stream_temp_read(php_stream *stream, char *buf, size_t count STREAMS_DC) { /* {{{ */
	php_stream_temp_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);

	if ( !ms->file) {
		return php_stream_memory_read(stream,buf,count STREAMS_REL_CC);
	} else {
		return php_stream_read(ms->file,buf,count STREAMS_REL_CC);
	}
} /* }}} */


static int php_stream_temp_close(php_stream *stream, int close_handle STREAMS_DC) { /* {{{ */
	php_stream_temp_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);

	if ( !ms->file) {
		return php_stream_memory_close(stream,close_handle STREAMS_REL_CC);
	} else {
		int ret = php_stream_close(ms->file STREAMS_REL_CC);
		ms->file = NULL;
		php_stream_memory_close(stream,close_handle STREAMS_REL_CC);
		return ret;
	}
} /* }}} */


static int php_stream_temp_flush(php_stream *stream STREAMS_DC) { /* {{{ */
	php_stream_temp_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);

	if ( !ms->file) {
		return php_stream_memory_flush(stream STREAMS_REL_CC);
	} else {
		return php_stream_flush(ms->file STREAMS_REL_CC);
	}
} /* }}} */


static int php_stream_temp_seek(php_stream *stream, off_t offset, int whence STREAMS_DC) { /* {{{ */
	php_stream_temp_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);

	if ( !ms->file) {
		return php_stream_memory_seek(stream,offset,whence STREAMS_REL_CC);
	} else {
		return php_stream_seek(ms->file,offset,whence STREAMS_REL_CC);
	}
} /* }}} */

char *php_stream_temp_gets(php_stream *stream, char *buf, size_t maxlen STREAMS_DC) { /* {{{ */
	php_stream_temp_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);

	if ( !ms->file) {
		return php_stream_memory_gets(stream,buf,maxlen STREAMS_REL_CC);
	} else {
		return php_stream_gets(ms->file,buf,maxlen STREAMS_REL_CC);
	}
} /* }}} */

static int php_stream_temp_cast(php_stream *stream, int castas, void **ret STREAMS_DC) { /* {{{ */
	php_stream_temp_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);

	if ( !ms->file) {
		return php_stream_memory_cast(stream,castas,ret STREAMS_REL_CC);
	} else {
		return php_stream_cast(ms->file,castas,ret,0 STREAMS_REL_CC);
	}
} /* }}} */

php_stream_ops	php_stream_temp_ops = {
	php_stream_temp_write, php_stream_temp_read,
	php_stream_temp_close, php_stream_temp_flush,
	php_stream_temp_seek,
	php_stream_temp_gets,
	php_stream_temp_cast,
	"TEMP"
};

PHPAPI php_stream *_php_stream_temp_create(int mode, size_t max_memory_usage STREAMS_DC) { /* {{{ */
	php_stream_temp_data *self;
	php_stream *stream;

	self = emalloc(sizeof(*self));
	assert( self != NULL);
	self->data = NULL;
	self->fpos = 0;
	self->fsize = 0;
	self->smax = max_memory_usage;
	self->mode = mode;
	if ( max_memory_usage)
		self->file = NULL;
	stream = _php_stream_alloc(&php_stream_temp_ops, self, 0, "rwb" STREAMS_REL_CC);
	if ( !max_memory_usage)
		php_stream_temp_write(stream,NULL,0 STREAMS_REL_CC);
	return stream;
} /* }}} */

PHPAPI php_stream *_php_stream_temp_open(int mode, size_t max_memory_usage, char *buf, size_t length STREAMS_DC) { /* {{{ */
	php_stream *stream;
	php_stream_temp_data *ms;

	if ( (stream = _php_stream_temp_create(mode & ~TEMP_STREAM_READONLY, max_memory_usage STREAMS_REL_CC)) != NULL) {
		if ( length) {
			assert(buf != NULL);
			php_stream_temp_write(stream,buf,length STREAMS_REL_CC);
		}
		ms = stream->abstract;
		assert(ms != NULL);
		ms->mode = mode;
	}
	return stream;
} /* }}} */

PHPAPI size_t _php_stream_temp_copy_buffer(php_stream *stream, char **buffer STREAMS_DC) { /* {{{ */
	size_t fpos, size;
	php_stream_temp_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);
	assert(buffer != NULL);
	assert(length != 0);

	if ( !ms->file) {
		*buffer = emalloc( ms->fsize);
		if ( !*buffer) {
			size = 0;
		} else {
			size = ms->fsize;
			memcpy(*buffer,ms->data,size);
		}
	} else {
		fpos = php_stream_tell(stream STREAMS_REL_CC);
		php_stream_seek(stream,0,SEEK_END STREAMS_REL_CC);
		size = php_stream_tell(stream STREAMS_REL_CC);
		*buffer = emalloc( size);
		if ( !*buffer) {
			size = 0;
		} else {
			php_stream_seek(stream,0,SEEK_SET STREAMS_REL_CC);
			size = php_stream_read(stream,*buffer,size STREAMS_REL_CC);
		}
		php_stream_seek(stream,fpos,SEEK_SET STREAMS_REL_CC);
	}
	return size;
} /* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
