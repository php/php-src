/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
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
   +----------------------------------------------------------------------+
 */

#define _GNU_SOURCE
#include "php.h"

#if HAVE_PHP_STREAM

#define MAX_CHUNK_SIZE	8192

#define TOREAD(stream) ((stream)->readbuf.writepos - (stream)->readbuf.readpos)
#define TOWRITE(stream)	((stream)->readbuf.writepos - (stream)->readbuf.readpos)

#define READPTR(stream) ((stream)->readbuf.buffer + (stream)->readbuf.readpos)
#define WRITEPTR(stream) ((stream)->readbuf.buffer + (stream)->readbuf.writepos)

#define READ_MAX(stream, max) if (stream->is_blocked) stream_read_total(sock, max); else stream_readahead(sock)


PHPAPI int php_stream_buf_init(php_stream_buffer * buffer, int persistent, size_t chunksize)
{
	memset(buffer, 0, sizeof(php_stream_buffer));

	/* defer memory allocation until first use */
	buffer->persistent = persistent;
	buffer->chunksize = chunksize;

	return SUCCESS;
}

PHPAPI int php_stream_buf_cleanup(php_stream_buffer * buffer)
{
	if (buffer->buffer)	{
		pefree(buffer->buffer, buffer->persistent);
		buffer->buffer = NULL;
	}
	return SUCCESS;
}

/* append data to the buffer ready for reading */
PHPAPI int php_stream_buf_append(php_stream_buffer * buffer, const char * buf, size_t size)
{
	if (!buffer->dirty && buffer->buffer && (buffer->writepos + size > buffer->buflen))	{
		/* if a lot of memory is sitting idle, reclaim it, but only if we are "clean" */
		if (buffer->readpos > 4 * buffer->chunksize)	{
			memmove(buffer->buffer + buffer->readpos, buffer->buffer, buffer->writepos - buffer->readpos);

			buffer->writepos -= buffer->readpos;
			buffer->readpos = 0;
		}
	}
	while (buffer->writepos + size > buffer->buflen)	{
		/* grow it */
		buffer->buflen += buffer->chunksize;
		buffer->buffer = perealloc(buffer->buffer, buffer->buflen, buffer->persistent);
	}
	memcpy(buffer->buffer + buffer->writepos, buf, size);
	buffer->writepos += size;
	return SUCCESS;
}

/* write data into the buffer at the present read position.
   When done, if we overlapped the writepos, move it to so that
   it occurs just after the zone we wrote.
*/
PHPAPI int php_stream_buf_overwrite(php_stream_buffer * buffer, const char * buf, size_t size)
{
	/* ensure that there it enough memory */
	while (buffer->readpos + size > buffer->buflen)	{
		buffer->buflen += buffer->chunksize;
		buffer->buffer = perealloc(buffer->buffer, buffer->buflen, buffer->persistent);
	}
	memcpy(buffer->buffer + buffer->readpos, buf, size);
	if (buffer->readpos + size > buffer->writepos)
		buffer->writepos = buffer->readpos + size;

	buffer->dirty = 1;

	return SUCCESS;
}

/* read data out of buffer */
PHPAPI size_t php_stream_buf_read(php_stream_buffer * buffer, char * buf, size_t size)
{
	size_t ret;

	ret = MIN(size, buffer->writepos - buffer->readpos);

	if (ret == 0)	{
		if (buf)
			buf[0] = 0;
	}
	else	{
		if (buf)
			memcpy(buf, buffer->buffer + buffer->readpos, ret);
		buffer->readpos += ret;
	}
	return ret;
}



/* allocate a new stream for a particular ops */
PHPAPI php_stream * php_stream_alloc(php_stream_ops * ops, void * abstract, size_t bufsize, int persistent, const char * mode)
{
	php_stream * ret;

	ret = (php_stream*)pemalloc(sizeof(php_stream), persistent);

	memset(ret, 0, sizeof(php_stream));

	ret->ops = ops;
	ret->abstract = abstract;
	ret->is_persistent = persistent;

	strncpy(ret->mode, mode, sizeof(ret->mode));

	if (bufsize)	{
		ret->is_buffered = 1;
		php_stream_buf_init(&ret->readbuf, persistent, bufsize);
	}
	return ret;
}

PHPAPI int php_stream_free(php_stream * stream, int call_dtor)
{
	int ret = 1;

	if (call_dtor)	{
		ret = stream->ops->close(stream);
	}
	php_stream_buf_cleanup(&stream->readbuf);
	pefree(stream, stream->is_persistent);

	return ret;
}

/* get a chunk into the stream read buffer */
static size_t stream_read_chunk(php_stream * stream)
{
	size_t nr, ret = 0;
	char buf[MAX_CHUNK_SIZE];

	/* do timeout check here ? */

	nr = stream->ops->read(stream, buf, stream->readbuf.chunksize);

	if (nr > 0)	{
		if (php_stream_buf_append(&stream->readbuf, buf, nr))
			ret = nr;
	}
	else if (nr == 0 || (nr < 0 && errno != EWOULDBLOCK))	{
		stream->eof = 1;
	}
	return ret;
}


/* read 1 + readahead chunks into buffer, if possible */
static size_t stream_readahead(php_stream * stream)
{
	size_t nr_bytes;
	size_t nr_read = 0;
	int i;

	for(i = 0; !stream->eof && i < (stream->readahead + 1); i++) {
		nr_bytes = stream_read_chunk(stream);
		if(nr_bytes == 0)
			break;
		nr_read += nr_bytes;
	}
	return nr_read;
}

static void stream_read_total(php_stream * stream, size_t size)
{
	while(!stream->eof && TOREAD(stream) < size && !stream->timeout_event) {
		stream_readahead(stream);
	}
}


PHPAPI size_t php_stream_read(php_stream * stream, char * buf, size_t size)
{
	size_t ret = 0;

	if (stream->is_buffered)	{
		/* fill the buffer with enough bytes */
		stream_read_total(stream, size);

		if(size < 0)
			return ret;

		ret = php_stream_buf_read(&stream->readbuf, buf, size);
	}
	else
		ret = stream->ops->read(stream, buf, size);

	return ret;
}

PHPAPI int php_stream_eof(php_stream * stream)
{
	int ret = 0;

	if (stream->is_buffered)	{

		if(!stream->is_blocked)
			stream_read_chunk(stream);

		if(!TOREAD(stream) && stream->eof)
			ret = 1;
	}
	else	{
		/* we will define our stream reading function so that it
		   must return EOF when an EOF condition occurs, when
		   working in unbuffered mode and called with these args */
		ret = stream->ops->read(stream, NULL, 0) == EOF ? 1 : 0;
	}
	return ret;
}

PHPAPI int php_stream_getc(php_stream * stream)
{
	char buf;

	if (php_stream_read(stream, &buf, 1) > 0)
		return buf;
	return EOF;
}


#define SEARCHCR()	p = memchr(READPTR(stream), '\n', MIN(TOREAD(stream), maxlen))

PHPAPI char *php_stream_gets(php_stream * stream, char *buf, size_t maxlen)
{

	if (maxlen == 0)	{
		buf[0] = 0;
		return buf;
	}

	if (stream->is_buffered)	{
		/* buffered fgets */
		char * p = NULL;
		size_t amount = 0;

		SEARCHCR();

		if (!p)	{
			if (stream->is_blocked)	{
				while (!p && !stream->eof && !stream->timeout_event && TOREAD(stream) < maxlen)
				{
					stream_read_chunk(stream);
					SEARCHCR();
				}
			}
			else	{
				stream_read_chunk(stream);
				SEARCHCR();
			}
		}

		if (p)
			amount = (ptrdiff_t)p - (ptrdiff_t)READPTR(stream) + 1;
		else
			amount = TOREAD(stream);

		amount = MIN(amount, maxlen);
		php_stream_buf_read(&stream->readbuf, buf, amount);
		buf[amount] = '\0';

		/* signal error only if we don't return data from this call
			and there is not data to read and if the eof flag is set */

		if (amount || TOREAD(stream) || !stream->eof)	{
			return buf;
		}

		return NULL;
	}
	else if (stream->ops->gets)	{
		return stream->ops->gets(stream, buf, maxlen);
	}
	else	{
		/* unbuffered fgets - poor performance ! */
		size_t n = 0;
		char * c = buf;

		/* TODO: look at error returns? */

		while(n < maxlen && stream->ops->read(stream, c, 1) > 0)	{
			n++;
			if (*c == '\n')	{
				c++;
				break;
			}
			c++;
		}
		*c = 0;
		return buf;
	}
}

static int stream_commit(php_stream * stream)
{
	zend_error(E_WARNING, "buffered writes not yet implemented!");
	return FAILURE;
}

PHPAPI int php_stream_flush(php_stream * stream)
{
	if (!stream->is_buffered && stream->ops->flush)
	{
		return stream->ops->flush(stream);
	}
	zend_error(E_WARNING, "php_stream_flush is not yet implemented on buffered streams!");
	return EOF;
}

PHPAPI size_t php_stream_write(php_stream * stream, const char * buf, size_t count)
{
	size_t ret = 0;

	if (strchr(stream->mode, 'w') == NULL)	{
		zend_error(E_WARNING, "%s(): stream was not opened for writing", get_active_function_name());
		return 0;
	}
	
	if (stream->is_buffered)	{
		/* commit buffer before appending, to preserve memory */
		stream_commit(stream);

		/* dump it into the buffer */
		php_stream_buf_overwrite(&stream->readbuf, buf, count);

		/* commit if it makes sense */
		stream_commit(stream);

		ret = count;
	}
	else
		ret = stream->ops->write(stream, buf, count);

	return ret;
}

PHPAPI off_t php_stream_tell(php_stream * stream)
{
	off_t ret = -1;
	if (stream->ops->seek)	{
		ret = stream->ops->seek(stream, 0, SEEK_CUR);
	}
	return ret;
}

PHPAPI int php_stream_seek(php_stream * stream, off_t offset, int whence)
{
	if (stream->is_buffered)	{
		/*TODO: implement!
		stream_commit(stream);
		stream->readbuf.readpos = 0;
		stream->readbuf.writepos = 0;
		if (stream->ops->seek)
			return stream->ops->seek(stream, offset, whence);
		*/
		goto cant_seek;
	}
	else if (stream->ops->seek)	{
		return stream->ops->seek(stream, offset, whence);
	}

cant_seek:
	zend_error(E_ERROR, "streams of type %s do not support seeking", stream->ops->label);
	return -1;
}


/*------- STDIO stream implementation -------*/

static size_t php_stdiop_write(php_stream * stream, const char * buf, size_t count)
{
	return fwrite(buf, 1, count, (FILE*)stream->abstract);
}

static size_t php_stdiop_read(php_stream * stream, char * buf, size_t count)
{
	if (buf == NULL && count == 0)	{
		/* check for EOF condition */
		if (feof((FILE*)stream->abstract))	{
			return EOF;
		}
		return 0;
	}
	return fread(buf, 1, count, (FILE*)stream->abstract);
}

static int php_stdiop_close(php_stream * stream)
{
	return fclose((FILE*)stream->abstract);
}

static int php_stdiop_flush(php_stream * stream)
{
	return fflush((FILE*)stream->abstract);
}

static int php_stdiop_seek(php_stream * stream, off_t offset, int whence)
{
	return fseek((FILE*)stream->abstract, offset, whence);
}

static char * php_stdiop_gets(php_stream * stream, char * buf, size_t size)
{
	return fgets(buf, size, (FILE*)stream->abstract);
}
static int php_stdiop_cast(php_stream * stream, int castas, void ** ret)
{
	int fd;

	switch (castas)	{
		case PHP_STREAM_AS_STDIO:
			if (ret)
				*ret = stream->abstract;
			return SUCCESS;

		case PHP_STREAM_AS_FD:
			fd = fileno((FILE*)stream->abstract);
			if (fd < 0)
				return FAILURE;
			if (ret)
				*ret = (void*)fd;
			return SUCCESS;
		default:
			return FAILURE;
	}
}

php_stream_ops	php_stream_stdio_ops = {
	php_stdiop_write, php_stdiop_read,
	php_stdiop_close, php_stdiop_flush, php_stdiop_seek,
	php_stdiop_gets, php_stdiop_cast,
	"STDIO"
};

PHPAPI php_stream * php_stream_fopen(const char * filename, const char * mode)
{
	FILE * fp = fopen(filename, mode);

	if (fp)	{
		php_stream * ret = php_stream_alloc(&php_stream_stdio_ops, fp, 0, 0, mode);

		if (ret)
			return ret;

		fclose(fp);
	}
	return NULL;
}

#if HAVE_FOPENCOOKIE
static ssize_t stream_cookie_reader(void *cookie, char *buffer, size_t size)
{
   return php_stream_read(((php_stream *)cookie), buffer, size);
}

static ssize_t stream_cookie_writer(void *cookie, const char *buffer, size_t size) {
   return php_stream_write(((php_stream *)cookie), (char *)buffer, size);
}

static int stream_cookie_seeker(void *cookie, off_t position, int whence) {
   return php_stream_seek(((php_stream *)cookie), position, whence);
}

static int stream_cookie_closer(void *cookie) {
   return php_stream_close(((php_stream *)cookie));
}

static COOKIE_IO_FUNCTIONS_T stream_cookie_functions =
{
	stream_cookie_reader, stream_cookie_writer,
	stream_cookie_seeker, stream_cookie_closer
};
#endif

PHPAPI int php_stream_cast(php_stream * stream, int castas, void ** ret, int show_err)
{
	if (castas == PHP_STREAM_AS_STDIO)	{
		if (stream->stdiocast)	{
			if (ret)
				*ret = stream->stdiocast;
			return SUCCESS;
		}

		if (stream->ops->cast && stream->ops->cast(stream, castas, ret) == SUCCESS)
			goto exit_success;


#if HAVE_FOPENCOOKIE
		/* if just checking, say yes we can be a FILE*, but don't actually create it yet */
		if (ret == NULL)
			goto exit_success;

		*ret = fopencookie(stream, stream->mode, stream_cookie_functions);

		if (*ret != NULL)
			goto exit_success;

		/* must be either:
		  	a) programmer error
		  	b) no memory
		  	-> lets bail
		 */
		zend_error(E_ERROR, "%s(): fopencookie failed", get_active_function_name());
		return FAILURE;
#endif

		goto exit_fail;
	}
	if (stream->ops->cast && stream->ops->cast(stream, castas, ret) == SUCCESS)
		goto exit_success;


exit_fail:
	if (show_err)	{
		const char * cast_names[3] = { "STDIO FILE*", "File Descriptor", "Socket Descriptor" };
		zend_error(E_WARNING, "%s(): cannot represent a stream of type %s as a %s",
			get_active_function_name(),
			stream->ops->label,
			cast_names[castas]
			);
	}

	return FAILURE;

exit_success:
	if (castas == PHP_STREAM_AS_STDIO && ret)
		stream->stdiocast = *ret;

	return SUCCESS;

}

#endif
