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
	char 	*data;
	size_t	fpos;
	size_t  fsize;
} php_memory_stream_data;

static size_t php_memory_stream_write(php_stream *stream, const char *buf, size_t count) {/* {{{ */
	php_memory_stream_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);
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

static size_t php_memory_stream_read(php_stream *stream, char *buf, size_t count) {/* {{{ */
	php_memory_stream_data *ms;

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
		assert(buf!= NULL);
		memcpy( buf, ms->data+ms->fpos, count);
		ms->fpos += count;
	}
	return count;
} /* }}} */


static int php_memory_stream_close(php_stream *stream, int close_handle) {/* {{{ */
	php_memory_stream_data *ms;

	assert(stream != NULL);
	ms = stream->abstract;
	assert(ms != NULL);
	if (ms->data) {
		efree(ms->data);
	}
	ms->data = NULL;
	ms->fsize = 0;
	ms->fpos = 0;
} /* }}} */


static int php_memory_stream_flush(php_stream *stream) {/* {{{ */
	// nothing to do here
	return 0;
} /* }}} */


static int php_memory_stream_seek(php_stream *stream, off_t offset, int whence) {/* {{{ */
	php_memory_stream_data *ms;

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
			return /*EINVAL*/;
	}
} /* }}} */

php_stream_ops	php_stream_memory_ops = {
	php_memory_stream_write, php_memory_stream_read,
	php_memory_stream_close, php_memory_stream_flush,
	php_memory_stream_seek,
	NULL,
	NULL,
	"MEMORY"
};

PHPAPI php_stream *php_memory_stream_create() {/* {{{ */
	php_memory_stream_data *self;

	self = emalloc(sizeof(*self));
	assert( self != NULL);
	self->data = NULL;
	self->fpos = 0;
	self->fsize = 0;
	return php_stream_alloc(&php_stream_memory_ops, self, 0, "rwb");
} /* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
