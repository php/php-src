/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
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

/* $Id$ */

#define _GNU_SOURCE
#include "php.h"
#include "php_globals.h"
#include "php_network.h"
#include "php_open_temporary_file.h"
#include "ext/standard/file.h"
#include <stddef.h>
#include <fcntl.h>

#include "php_streams_int.h"

/* Under BSD, emulate fopencookie using funopen */
#if HAVE_FUNOPEN
typedef struct {
	int (*reader)(void *, char *, int);
	int (*writer)(void *, const char *, int);
	fpos_t (*seeker)(void *, fpos_t, int);
	int (*closer)(void *);
} COOKIE_IO_FUNCTIONS_T;

FILE *fopencookie(void *cookie, const char *mode, COOKIE_IO_FUNCTIONS_T *funcs)
{
	return funopen(cookie, funcs->reader, funcs->writer, funcs->seeker, funcs->closer);
}
# define HAVE_FOPENCOOKIE 1
# define PHP_STREAM_COOKIE_FUNCTIONS	&stream_cookie_functions
#elif HAVE_FOPENCOOKIE
# define PHP_STREAM_COOKIE_FUNCTIONS	stream_cookie_functions
#endif

/* {{{ STDIO with fopencookie */
#if HAVE_FUNOPEN
/* use our fopencookie emulation */
static int stream_cookie_reader(void *cookie, char *buffer, int size)
{
	int ret;
	TSRMLS_FETCH();
	ret = php_stream_read((php_stream*)cookie, buffer, size);
	return ret;
}

static int stream_cookie_writer(void *cookie, const char *buffer, int size)
{
	TSRMLS_FETCH();
	return php_stream_write((php_stream *)cookie, (char *)buffer, size);
}

static fpos_t stream_cookie_seeker(void *cookie, off_t position, int whence)
{
	TSRMLS_FETCH();
	return (fpos_t)php_stream_seek((php_stream *)cookie, position, whence);
}

static int stream_cookie_closer(void *cookie)
{
	php_stream *stream = (php_stream*)cookie;
	TSRMLS_FETCH();

	/* prevent recursion */
	stream->fclose_stdiocast = PHP_STREAM_FCLOSE_NONE;
	return php_stream_close(stream);
}

#elif HAVE_FOPENCOOKIE
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

#ifdef COOKIE_SEEKER_USES_OFF64_T
static int stream_cookie_seeker(void *cookie, __off64_t *position, int whence)
{
	TSRMLS_FETCH();

	*position = php_stream_seek((php_stream *)cookie, (off_t)*position, whence);

	if (*position == -1)
		return -1;
	return 0;
}
#else
static int stream_cookie_seeker(void *cookie, off_t position, int whence)
{
	TSRMLS_FETCH();
	return php_stream_seek((php_stream *)cookie, position, whence);
}
#endif

static int stream_cookie_closer(void *cookie)
{
	php_stream *stream = (php_stream*)cookie;
	TSRMLS_FETCH();

	/* prevent recursion */
	stream->fclose_stdiocast = PHP_STREAM_FCLOSE_NONE;
	return php_stream_close(stream);
}
#endif /* elif HAVE_FOPENCOOKIE */

#if HAVE_FOPENCOOKIE
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

	/* synchronize our buffer (if possible) */
	if (ret) {
		php_stream_flush(stream);
		if (stream->ops->seek && (stream->flags & PHP_STREAM_FLAG_NO_SEEK) == 0) {
			off_t dummy;

			stream->ops->seek(stream, stream->position, SEEK_SET, &dummy TSRMLS_CC);
			stream->readpos = stream->writepos = 0;
		}
	}
	
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

		*ret = fopencookie(stream, stream->mode, PHP_STREAM_COOKIE_FUNCTIONS);

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
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "fopencookie failed");
		return FAILURE;
#endif

		if (flags & PHP_STREAM_CAST_TRY_HARD) {
			php_stream *newstream;

			newstream = php_stream_fopen_tmpfile();
			if (newstream) {
				size_t copied = php_stream_copy_to_stream(stream, newstream, PHP_STREAM_COPY_ALL);

				if (copied == 0) {
					php_stream_close(newstream);
				} else {
					int retcode = php_stream_cast(newstream, castas | flags, ret, show_err);

					if (retcode == SUCCESS)
						rewind((FILE*)*ret);
					
					/* do some specialized cleanup */
					if ((flags & PHP_STREAM_CAST_RELEASE)) {
						php_stream_free(stream, PHP_STREAM_FREE_CLOSE_CASTED);
					}

					return retcode;
				}
			}
		}
	}

	if (stream->filterhead) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot cast a filtered stream on this system");
		return FAILURE;
	} else if (stream->ops->cast && stream->ops->cast(stream, castas, ret TSRMLS_CC) == SUCCESS) {
		goto exit_success;
	}

	if (show_err) {
		/* these names depend on the values of the PHP_STREAM_AS_XXX defines in php_streams.h */
		static const char *cast_names[3] = {
			"STDIO FILE*", "File Descriptor", "Socket Descriptor"
		};

		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot represent a stream of type %s as a %s",
			stream->ops->label,
			cast_names[castas]
			);
	}

	return FAILURE;

exit_success:

	if ((stream->writepos - stream->readpos) > 0 &&
			stream->fclose_stdiocast != PHP_STREAM_FCLOSE_FOPENCOOKIE &&
			(flags & PHP_STREAM_CAST_INTERNAL) == 0) {
		/* the data we have buffered will be lost to the third party library that
		 * will be accessing the stream.  Emit a warning so that the end-user will
		 * know that they should try something else */
		
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
				"%d bytes of buffered data lost during stream conversion!",
				stream->writepos - stream->readpos);
	}
	
	if (castas == PHP_STREAM_AS_STDIO && ret)
		stream->stdiocast = *ret;
	
	if (flags & PHP_STREAM_CAST_RELEASE) {
		php_stream_free(stream, PHP_STREAM_FREE_CLOSE_CASTED);
	}

	return SUCCESS;

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

#ifdef PHP_WIN32
	/* Avoid possible strange problems when working with socket based streams */
	if ((options & STREAM_OPEN_FOR_INCLUDE) && php_stream_is(stream, PHP_STREAM_IS_SOCKET)) {
		char buf[CHUNK_SIZE];

		fp = php_open_temporary_file(NULL, "php", NULL TSRMLS_CC);
		if (fp) {
			while (!php_stream_eof(stream)) {
				size_t didread = php_stream_read(stream, buf, sizeof(buf));
				if (didread > 0) {
					fwrite(buf, 1, didread, fp);
				} else {
					break;
				}
			}
			php_stream_close(stream);
			rewind(fp);
			return fp;
		}
	}
#endif
	
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
	
	if (((flags & PHP_STREAM_FORCE_CONVERSION) == 0) && origstream->ops->seek != NULL) {
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
