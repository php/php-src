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
   | Author: Wez Furlong (wez@thebrainroom.com)                           |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_STREAMS_H
#define PHP_STREAMS_H

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

/* See README.STREAMS in php4 root dir for more info about this stuff */

typedef struct _php_stream php_stream;

typedef struct _php_stream_ops  {
	/* stdio like functions - these are mandatory! */
	size_t (*write)(php_stream * stream, const char * buf, size_t count);
	size_t (*read)(php_stream * stream, char * buf, size_t count);
	int    (*close)(php_stream * stream);
	int    (*flush)(php_stream * stream);
	/* these are optional */
	int    (*seek)(php_stream * stream, off_t offset, int whence);
	char * (*gets)(php_stream * stream, char * buf, size_t size);
	int (*cast)(php_stream * stream, int castas, void ** ret);
	const char * label; /* label for this ops structure */
} php_stream_ops;

/* options uses the IGNORE_URL family of defines from fopen_wrappers.h */
typedef php_stream * (*php_stream_factory_func_t)(char * filename, char * mode, int options, char ** opened_path TSRMLS_DC);
typedef void (*php_stream_wrapper_dtor_func_t)(php_stream * stream);

typedef struct _php_stream_wrapper	{
	php_stream_factory_func_t		create;
	php_stream_wrapper_dtor_func_t	destroy;
} php_stream_wrapper;

struct _php_stream  {
	php_stream_ops * ops;
	void * abstract;  		/* convenience pointer for abstraction */

	php_stream_wrapper * wrapper; /* which wrapper was used to open the stream */
	void * wrapperthis;		/* convenience pointer for a instance of a wrapper */
	zval * wrapperdata;		/* fgetwrapperdata retrieves this */

	int is_persistent;
	char mode[16];			/* "rwb" etc. ala stdio */
	/* so we know how to clean it up correctly.  This should be set to
	 * PHP_STREAM_FCLOSE_XXX as appropriate */
	int fclose_stdiocast;
	FILE * stdiocast;    /* cache this, otherwise we might leak! */
}; /* php_stream */
#define PHP_STREAM_FCLOSE_NONE 0
#define PHP_STREAM_FCLOSE_FDOPEN	1
#define PHP_STREAM_FCLOSE_FOPENCOOKIE 2


/* allocate a new stream for a particular ops */
PHPAPI php_stream * php_stream_alloc(php_stream_ops * ops, void * abstract, int persistent, const char * mode);

PHPAPI int php_stream_free(php_stream * stream, int call_dtor);
#define php_stream_close(stream)	php_stream_free(stream, 1)

PHPAPI int php_stream_seek(php_stream * stream, off_t offset, int whence);
#define php_stream_rewind(stream)	php_stream_seek(stream, 0L, SEEK_SET)
PHPAPI off_t php_stream_tell(php_stream * stream);
PHPAPI size_t php_stream_read(php_stream * stream, char * buf, size_t count);
PHPAPI size_t php_stream_write(php_stream * stream, const char * buf, size_t count);
#define php_stream_write_string(stream, str)	php_stream_write(stream, str, strlen(str))
PHPAPI int php_stream_eof(php_stream * stream);
PHPAPI int php_stream_getc(php_stream * stream);
PHPAPI int php_stream_putc(php_stream * stream, int c);
PHPAPI int php_stream_flush(php_stream * stream);
PHPAPI char *php_stream_gets(php_stream * stream, char *buf, size_t maxlen);
PHPAPI int php_stream_puts(php_stream * stream, char * buf);

/* copy up to maxlen bytes from src to dest.  If maxlen is 0, copy until eof(src).
 * Uses mmap if the src is a plain file and at offset 0 */
PHPAPI size_t php_stream_copy_to_stream(php_stream * src, php_stream * dest, size_t maxlen);
/* read all data from stream and put into a buffer. Caller must free buffer when done,
 * according to allocopts.
 * The copy will use mmap if available. */
PHPAPI size_t php_stream_read_all(php_stream * src, char ** buf, int persistent);

/* maybe implement someday */
#define php_stream_error(stream)	(0)

/* operations for a stdio FILE; use the php_stream_fopen_XXX funcs below */
extern php_stream_ops php_stream_stdio_ops;
/* like fopen, but returns a stream */
PHPAPI php_stream * php_stream_fopen(const char * filename, const char * mode, char **opened_path TSRMLS_DC);
PHPAPI php_stream * php_stream_fopen_with_path(char * filename, char * mode, char * path, char **opened_path TSRMLS_DC);
PHPAPI php_stream * php_stream_fopen_from_file(FILE * file, const char * mode);
PHPAPI php_stream * php_stream_fopen_from_pipe(FILE * file, const char * mode);
PHPAPI php_stream * php_stream_fopen_tmpfile(void);
PHPAPI php_stream * php_stream_fopen_temporary_file(const char * dir, const char * pfx, char **opened_path TSRMLS_DC);

/* coerce the stream into some other form */
/* cast as a stdio FILE * */
#define PHP_STREAM_AS_STDIO     0
/* cast as a POSIX fd or socketd */
#define PHP_STREAM_AS_FD		1
/* cast as a socketd */
#define PHP_STREAM_AS_SOCKETD	2

/* try really, really hard to make sure the cast happens (socketpair) */
#define PHP_STREAM_CAST_TRY_HARD	0x80000000

PHPAPI int php_stream_cast(php_stream * stream, int castas, void ** ret, int show_err);
/* use this to check if a stream can be cast into another form */
#define php_stream_can_cast(stream, as)	php_stream_cast(stream, as, NULL, 0)

/* use this to check if a stream is of a particular type:
 * PHPAPI int php_stream_is(php_stream * stream, php_stream_ops * ops); */
#define php_stream_is(stream, anops)		((stream)->ops == anops)
#define PHP_STREAM_IS_STDIO &php_stream_stdio_ops

#define php_stream_is_persistent(stream)	(stream)->is_persistent

/* Wrappers support */
int php_init_stream_wrappers(TSRMLS_D);
int php_shutdown_stream_wrappers(TSRMLS_D);
PHPAPI int php_register_url_stream_wrapper(char * protocol, php_stream_wrapper * wrapper TSRMLS_DC);
PHPAPI int php_unregister_url_stream_wrapper(char * protocol TSRMLS_DC);

PHPAPI php_stream * php_stream_open_wrapper(char * path, char * mode, int options, char ** opened_path TSRMLS_DC);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
