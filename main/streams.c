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

#ifdef PHP_WIN32
#define EWOULDBLOCK WSAEWOULDBLOCK
#else
#include "build-defs.h"
#endif

#define MAX_CHUNK_SIZE	8192

#define TOREAD(stream) ((stream)->readbuf.writepos - (stream)->readbuf.readpos)
#define TOWRITE(stream)	((stream)->readbuf.writepos - (stream)->readbuf.readpos)

#define READPTR(stream) ((stream)->readbuf.buffer + (stream)->readbuf.readpos)
#define WRITEPTR(stream) ((stream)->readbuf.buffer + (stream)->readbuf.writepos)

#define READ_MAX(stream, max) if (stream->is_blocked) stream_read_total(sock, max); else stream_readahead(sock)

/* allocate a new stream for a particular ops */
PHPAPI php_stream * php_stream_alloc(php_stream_ops * ops, void * abstract, int persistent, const char * mode)
{
	php_stream * ret;

	ret = (php_stream*)pemalloc(sizeof(php_stream), persistent);

	memset(ret, 0, sizeof(php_stream));

	ret->ops = ops;
	ret->abstract = abstract;
	ret->is_persistent = persistent;

	strncpy(ret->mode, mode, sizeof(ret->mode));

	return ret;
}

PHPAPI int php_stream_free(php_stream * stream, int call_dtor)
{
	int ret = 1;

	if (call_dtor)	{

		if (stream->fclose_stdiocast == PHP_STREAM_FCLOSE_FOPENCOOKIE)	{
			/* calling fclose on an fopencookied stream will ultimately
				call this very same function.  If we were called via fclose,
				the cookie_closer unsets the fclose_stdiocast flags, so
				we can be sure that we only reach here when PHP code calls
				php_stream_free.
				Lets let the cookie code clean it all up.
			 */
			return fclose(stream->stdiocast);
		}
		
		php_stream_flush(stream);
		ret = stream->ops->close(stream);
	
		/* tidy up any FILE* that might have been fdopened */
		if (stream->fclose_stdiocast == PHP_STREAM_FCLOSE_FDOPEN && stream->stdiocast)
		{
			fclose(stream->stdiocast);
			stream->stdiocast = NULL;
		}
	}
	pefree(stream, stream->is_persistent);

	return ret;
}

PHPAPI size_t php_stream_read(php_stream * stream, char * buf, size_t size)
{
	return stream->ops->read(stream, buf, size);
}

PHPAPI int php_stream_eof(php_stream * stream)
{
	/* we define our stream reading function so that it
	   must return EOF when an EOF condition occurs, when
	   working in unbuffered mode and called with these args */
	return stream->ops->read(stream, NULL, 0) == EOF ? 1 : 0;
}

PHPAPI int php_stream_getc(php_stream * stream)
{
	char buf;

	if (php_stream_read(stream, &buf, 1) > 0)
		return buf;
	return EOF;
}

PHPAPI char *php_stream_gets(php_stream * stream, char *buf, size_t maxlen)
{

	if (maxlen == 0)	{
		buf[0] = 0;
		return buf;
	}

	if (stream->ops->gets)	{
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

PHPAPI int php_stream_flush(php_stream * stream)
{
	return stream->ops->flush(stream);
}

PHPAPI size_t php_stream_write(php_stream * stream, const char * buf, size_t count)
{
	if (strchr(stream->mode, 'w') == NULL)	{
		TSRMLS_FETCH();

		zend_error(E_WARNING, "%s(): stream was not opened for writing", get_active_function_name(TSRMLS_C));
		return 0;
	}
	
	return stream->ops->write(stream, buf, count);
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
	if (stream->ops->seek)	
		return stream->ops->seek(stream, offset, whence);

	zend_error(E_WARNING, "streams of type %s do not support seeking", stream->ops->label);
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
		php_stream * ret = php_stream_alloc(&php_stream_stdio_ops, fp, 0, mode);

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
	php_stream * stream = (php_stream*)cookie;
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

		if (*ret != NULL)	{
			stream->fclose_stdiocast = 1;
			goto exit_success;
		}

		{
			TSRMLS_FETCH();

			/* must be either:
		  		a) programmer error
		  		b) no memory
		  		-> lets bail
			 */
			zend_error(E_ERROR, "%s(): fopencookie failed", get_active_function_name(TSRMLS_C));
			return FAILURE;
		}
#endif

		goto exit_fail;
	}
	if (stream->ops->cast && stream->ops->cast(stream, castas, ret) == SUCCESS)
		goto exit_success;


exit_fail:
	if (show_err)	{
		/* these names depend on the values of the PHP_STREAM_AS_XXX defines in php_streams.h */
		static const char * cast_names[3] = {
			"STDIO FILE*", "File Descriptor", "Socket Descriptor"
		};
		TSRMLS_FETCH();

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

	return SUCCESS;

}

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 tw=78 fdm=marker
 * vim<600: noet sw=4 ts=4 tw=78
 */
