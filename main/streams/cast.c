/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include "php.h"
#include "php_globals.h"
#include "php_network.h"
#include "php_open_temporary_file.h"
#include "ext/standard/file.h"
#include <stddef.h>
#include <fcntl.h>

#include "php_streams_int.h"

/* Under BSD, emulate fopencookie using funopen */
#if defined(HAVE_FUNOPEN) && !defined(HAVE_FOPENCOOKIE)

/* NetBSD 6.0+ uses off_t instead of fpos_t in funopen */
# if defined(__NetBSD__) && (__NetBSD_Version__ >= 600000000)
#  define PHP_FPOS_T off_t
# else
#  define PHP_FPOS_T fpos_t
# endif

typedef struct {
	int (*reader)(void *, char *, int);
	int (*writer)(void *, const char *, int);
	PHP_FPOS_T (*seeker)(void *, PHP_FPOS_T, int);
	int (*closer)(void *);
} COOKIE_IO_FUNCTIONS_T;

FILE *fopencookie(void *cookie, const char *mode, COOKIE_IO_FUNCTIONS_T *funcs)
{
	return funopen(cookie, funcs->reader, funcs->writer, funcs->seeker, funcs->closer);
}
# define HAVE_FOPENCOOKIE 1
# define PHP_EMULATE_FOPENCOOKIE 1
# define PHP_STREAM_COOKIE_FUNCTIONS	&stream_cookie_functions
#elif defined(HAVE_FOPENCOOKIE)
# define PHP_STREAM_COOKIE_FUNCTIONS	stream_cookie_functions
#endif

/* {{{ STDIO with fopencookie */
#if defined(PHP_EMULATE_FOPENCOOKIE)
/* use our fopencookie emulation */
static int stream_cookie_reader(void *cookie, char *buffer, int size)
{
	int ret;

	ret = php_stream_read((php_stream*)cookie, buffer, size);
	return ret;
}

static int stream_cookie_writer(void *cookie, const char *buffer, int size)
{

	return php_stream_write((php_stream *)cookie, (char *)buffer, size);
}

static PHP_FPOS_T stream_cookie_seeker(void *cookie, zend_off_t position, int whence)
{

	return (PHP_FPOS_T)php_stream_seek((php_stream *)cookie, position, whence);
}

static int stream_cookie_closer(void *cookie)
{
	php_stream *stream = (php_stream*)cookie;

	/* prevent recursion */
	stream->fclose_stdiocast = PHP_STREAM_FCLOSE_NONE;
	return php_stream_free(stream,
		PHP_STREAM_FREE_CLOSE | PHP_STREAM_FREE_KEEP_RSRC | PHP_STREAM_FREE_RSRC_DTOR);
}
#elif defined(HAVE_FOPENCOOKIE)
static ssize_t stream_cookie_reader(void *cookie, char *buffer, size_t size)
{
	ssize_t ret;

	ret = php_stream_read(((php_stream *)cookie), buffer, size);
	return ret;
}

static ssize_t stream_cookie_writer(void *cookie, const char *buffer, size_t size)
{

	return php_stream_write(((php_stream *)cookie), (char *)buffer, size);
}

# ifdef COOKIE_SEEKER_USES_OFF64_T
static int stream_cookie_seeker(void *cookie, off64_t *position, int whence)
{

	*position = php_stream_seek((php_stream *)cookie, (zend_off_t)*position, whence);

	if (*position == -1) {
		return -1;
	}
	return 0;
}
# else
static int stream_cookie_seeker(void *cookie, zend_off_t position, int whence)
{

	return php_stream_seek((php_stream *)cookie, position, whence);
}
# endif

static int stream_cookie_closer(void *cookie)
{
	php_stream *stream = (php_stream*)cookie;

	/* prevent recursion */
	stream->fclose_stdiocast = PHP_STREAM_FCLOSE_NONE;
	return php_stream_free(stream,
		PHP_STREAM_FREE_CLOSE | PHP_STREAM_FREE_KEEP_RSRC | PHP_STREAM_FREE_RSRC_DTOR);
}
#endif /* elif defined(HAVE_FOPENCOOKIE) */

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

/* {{{ php_stream_mode_sanitize_fdopen_fopencookie
 * Result should have at least size 5, e.g. to write wbx+\0 */
void php_stream_mode_sanitize_fdopen_fopencookie(php_stream *stream, char *result)
{
	/* replace modes not supported by fdopen and fopencookie, but supported
	 * by PHP's fread(), so that their calls won't fail */
	const char *cur_mode = stream->mode;
	int         has_plus = 0,
		        has_bin  = 0,
				i,
				res_curs = 0;

	if (cur_mode[0] == 'r' || cur_mode[0] == 'w' || cur_mode[0] == 'a') {
		result[res_curs++] = cur_mode[0];
	} else {
		/* assume cur_mode[0] is 'c' or 'x'; substitute by 'w', which should not
		 * truncate anything in fdopen/fopencookie */
		result[res_curs++] = 'w';

		/* x is allowed (at least by glibc & compat), but not as the 1st mode
		 * as in PHP and in any case is (at best) ignored by fdopen and fopencookie */
	}

	/* assume current mode has at most length 4 (e.g. wbn+) */
	for (i = 1; i < 4 && cur_mode[i] != '\0'; i++) {
		if (cur_mode[i] == 'b') {
			has_bin = 1;
		} else if (cur_mode[i] == '+') {
			has_plus = 1;
		}
		/* ignore 'n', 't' or other stuff */
	}

	if (has_bin) {
		result[res_curs++] = 'b';
	}
	if (has_plus) {
		result[res_curs++] = '+';
	}

	result[res_curs] = '\0';
}
/* }}} */

static inline bool is_stream_user_stream(const php_stream *stream)
{
	return stream->ops == &php_stream_userspace_ops;
}

/* {{{ php_stream_cast */
PHPAPI int _php_stream_cast(php_stream *stream, int castas, void **ret, int show_err)
{
	int flags = castas & PHP_STREAM_CAST_MASK;
	castas &= ~PHP_STREAM_CAST_MASK;

	/* synchronize our buffer (if possible) */
	if (ret && castas != PHP_STREAM_AS_FD_FOR_SELECT) {
		php_stream_flush(stream);
		/* Do special handling for user streams as they may not implement the cast method */
		if (!show_err && is_stream_user_stream(stream)) {
			castas |= PHP_STREAM_FLAG_SUPPRESS_ERRORS;
		}
		if (stream->ops->seek && (stream->flags & PHP_STREAM_FLAG_NO_SEEK) == 0) {
			zend_off_t dummy;

			stream->ops->seek(stream, stream->position, SEEK_SET, &dummy);
			stream->readpos = stream->writepos = 0;
		}
	}

	/* filtered streams can only be cast as stdio, and only when fopencookie is present */

	if (castas == PHP_STREAM_AS_STDIO) {
		if (stream->stdiocast) {
			if (ret) {
				*(FILE**)ret = stream->stdiocast;
			}
			goto exit_success;
		}

		/* Do special handling for user streams as they may not implement the cast method */
		if (!show_err && is_stream_user_stream(stream)) {
			castas |= PHP_STREAM_FLAG_SUPPRESS_ERRORS;
		}

		/* if the stream is a stdio stream let's give it a chance to respond
		 * first, to avoid doubling up the layers of stdio with an fopencookie */
		if (php_stream_is(stream, PHP_STREAM_IS_STDIO) &&
			stream->ops->cast &&
			!php_stream_is_filtered(stream) &&
			stream->ops->cast(stream, castas, ret) == SUCCESS
		) {
			goto exit_success;
		}

#if HAVE_FOPENCOOKIE
		/* if just checking, say yes we can be a FILE*, but don't actually create it yet */
		if (ret == NULL) {
			goto exit_success;
		}

		{
			char fixed_mode[5];
			php_stream_mode_sanitize_fdopen_fopencookie(stream, fixed_mode);
			*(FILE**)ret = fopencookie(stream, fixed_mode, PHP_STREAM_COOKIE_FUNCTIONS);
		}

		if (*ret != NULL) {
			zend_off_t pos;

			stream->fclose_stdiocast = PHP_STREAM_FCLOSE_FOPENCOOKIE;

			/* If the stream position is not at the start, we need to force
			 * the stdio layer to believe it's real location. */
			pos = php_stream_tell(stream);
			if (pos > 0) {
				zend_fseek(*ret, pos, SEEK_SET);
			}

			goto exit_success;
		}

		/* must be either:
			a) programmer error
			b) no memory
			-> lets bail
		*/
		php_error_docref(NULL, E_ERROR, "fopencookie failed");
		return FAILURE;
#endif

		if (!php_stream_is_filtered(stream) && stream->ops->cast && stream->ops->cast(stream, castas, NULL) == SUCCESS) {
			if (FAILURE == stream->ops->cast(stream, castas, ret)) {
				return FAILURE;
			}
			goto exit_success;
		} else if (flags & PHP_STREAM_CAST_TRY_HARD) {
			php_stream *newstream;

			newstream = php_stream_fopen_tmpfile_ex(/* emit errors */ false);
			if (newstream) {
				int retcopy = php_stream_copy_to_stream_ex(stream, newstream, PHP_STREAM_COPY_ALL, NULL);

				if (retcopy != SUCCESS) {
					php_stream_close(newstream);
				} else {
					int retcast = php_stream_cast(newstream, castas | flags, (void **)ret, show_err);

					if (retcast == SUCCESS) {
						rewind(*(FILE**)ret);
					}

					/* do some specialized cleanup */
					if ((flags & PHP_STREAM_CAST_RELEASE)) {
						php_stream_free(stream, PHP_STREAM_FREE_CLOSE_CASTED);
					}

					/* TODO: we probably should be setting .stdiocast and .fclose_stdiocast or
					 * we may be leaking the FILE*. Needs investigation, though. */
					return retcast;
				}
			}
		}
	}

	if (php_stream_is_filtered(stream)) {
		if (show_err) {
			php_error_docref(NULL, E_WARNING, "Cannot cast a filtered stream on this system");
		}
		return FAILURE;
	} else if (stream->ops->cast) {
		/* Do special handling for user streams as they may not implement the cast method */
		if (!show_err && is_stream_user_stream(stream)) {
			castas |= PHP_STREAM_FLAG_SUPPRESS_ERRORS;
		}
		if (stream->ops->cast(stream, castas, ret) == SUCCESS) {
			goto exit_success;
		}
	}

	if (show_err) {
		/* these names depend on the values of the PHP_STREAM_AS_XXX defines in php_streams.h */
		static const char *cast_names[4] = {
			"STDIO FILE*",
			"File Descriptor",
			"Socket Descriptor",
			"select()able descriptor"
		};

		php_error_docref(NULL, E_WARNING, "Cannot represent a stream of type %s as a %s", stream->ops->label, cast_names[castas]);
	}

	return FAILURE;

exit_success:

	if ((stream->writepos - stream->readpos) > 0 &&
		stream->fclose_stdiocast != PHP_STREAM_FCLOSE_FOPENCOOKIE &&
		(flags & PHP_STREAM_CAST_INTERNAL) == 0
	) {
		/* the data we have buffered will be lost to the third party library that
		 * will be accessing the stream.  Emit a warning so that the end-user will
		 * know that they should try something else */

		php_error_docref(NULL, E_WARNING, ZEND_LONG_FMT " bytes of buffered data lost during stream conversion!", (zend_long)(stream->writepos - stream->readpos));
	}

	if (castas == PHP_STREAM_AS_STDIO && ret) {
		stream->stdiocast = *(FILE**)ret;
	}

	if (flags & PHP_STREAM_CAST_RELEASE) {
		php_stream_free(stream, PHP_STREAM_FREE_CLOSE_CASTED);
	}

	return SUCCESS;

}
/* }}} */

/* {{{ php_stream_open_wrapper_as_file */
PHPAPI FILE * _php_stream_open_wrapper_as_file(char *path, char *mode, int options, zend_string **opened_path STREAMS_DC)
{
	FILE *fp = NULL;
	php_stream *stream = NULL;

	stream = php_stream_open_wrapper_rel(path, mode, options|STREAM_WILL_CAST, opened_path);

	if (stream == NULL) {
		return NULL;
	}

	if (php_stream_cast(stream, PHP_STREAM_AS_STDIO|PHP_STREAM_CAST_TRY_HARD|PHP_STREAM_CAST_RELEASE, (void**)&fp, REPORT_ERRORS) == FAILURE) {
		php_stream_close(stream);
		if (opened_path && *opened_path) {
			zend_string_release_ex(*opened_path, 0);
		}
		return NULL;
	}
	return fp;
}
/* }}} */

/* {{{ php_stream_make_seekable */
PHPAPI int _php_stream_make_seekable(php_stream *origstream, php_stream **newstream, int flags STREAMS_DC)
{
	if (newstream == NULL) {
		return PHP_STREAM_FAILED;
	}
	*newstream = NULL;

	if (((flags & PHP_STREAM_FORCE_CONVERSION) == 0) && origstream->ops->seek != NULL) {
		*newstream = origstream;
		return PHP_STREAM_UNCHANGED;
	}

	/* Use a tmpfile and copy the old streams contents into it */

	if (flags & PHP_STREAM_PREFER_STDIO) {
		*newstream = php_stream_fopen_tmpfile();
	} else {
		*newstream = php_stream_temp_new();
	}

	if (*newstream == NULL) {
		return PHP_STREAM_FAILED;
	}

#if ZEND_DEBUG
	(*newstream)->open_filename = origstream->open_filename;
	(*newstream)->open_lineno = origstream->open_lineno;
#endif

	if (php_stream_copy_to_stream_ex(origstream, *newstream, PHP_STREAM_COPY_ALL, NULL) != SUCCESS) {
		php_stream_close(*newstream);
		*newstream = NULL;
		return PHP_STREAM_CRITICAL;
	}

	php_stream_close(origstream);
	php_stream_seek(*newstream, 0, SEEK_SET);

	return PHP_STREAM_RELEASED;
}
/* }}} */
