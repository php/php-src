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

#ifndef PHP_STREAMS_H
#define PHP_STREAMS_H

#if HAVE_PHP_STREAM

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif


typedef struct _php_stream php_stream;

typedef struct _php_stream_ops  {
/* stdio like functions - these are mandatory! */
	size_t (*write)(php_stream * stream, const char * buf, size_t count);
	size_t (*read)(php_stream * stream, char * buf, size_t count);
	int    (*close)(php_stream * stream);
	int    (*flush)(php_stream * stream);
/* these are optional */
	int    (*seek)(php_stream * stream, off_t offset, int whence);
/* used only in unbuffered mode */
	char * (*gets)(php_stream * stream, char * buf, size_t size);
	int (*cast)(php_stream * stream, int castas, void ** ret);
	const char * label; /* label for this ops structure */
} php_stream_ops;

typedef struct _php_stream_buffer	{
	char * buffer;
	size_t buflen;

	int dirty;	/* 1 if we need to commit data */

	off_t readpos;
	off_t writepos;

	size_t chunksize;	/* amount to commit in one operation */
	int persistent;
} php_stream_buffer;

PHPAPI int php_stream_buf_init(php_stream_buffer * buffer, int persistent, size_t chunksize);
PHPAPI int php_stream_buf_cleanup(php_stream_buffer * buffer);
/* add data into buffer, growing it if required */
PHPAPI int php_stream_buf_append(php_stream_buffer * buffer, const char * buf, size_t size);
/* read data out of buffer */
PHPAPI size_t php_stream_buf_read(php_stream_buffer * buffer, char * buf, size_t size);
PHPAPI int php_stream_buf_overwrite(php_stream_buffer * buffer, const char * buf, size_t size);

struct _php_stream  {
	php_stream_ops * ops;
	void * abstract;  		/* convenience pointer for abstraction */
	int eof;

	/* for convenience for sockets */
	int is_blocked;
	struct timeval timeout;
	int timeout_event;

	int readahead;			/* number of chunks to read-ahead */

	int is_persistent;
	char mode[16];			/* "rwb" etc. ala stdio */
	/* the stream can be buffered  */
	int is_buffered;
	php_stream_buffer readbuf;

	FILE * stdiocast;    /* cache this, otherwise we might leak! */
}; /* php_stream */

/* allocate a new stream for a particular ops */
PHPAPI php_stream * php_stream_alloc(php_stream_ops * ops, void * abstract, size_t bufsize, int persistent, const char * mode);

PHPAPI int php_stream_free(php_stream * stream, int call_dtor);
#define php_stream_close(stream)	php_stream_free(stream, 1)

/* seeking is only supported for reading! */
PHPAPI int php_stream_seek(php_stream * stream, off_t offset, int whence);
PHPAPI off_t php_stream_tell(php_stream * stream);
PHPAPI size_t php_stream_read(php_stream * stream, char * buf, size_t count);
PHPAPI size_t php_stream_write(php_stream * stream, const char * buf, size_t count);
PHPAPI int php_stream_eof(php_stream * stream);
PHPAPI int php_stream_getc(php_stream * stream);
PHPAPI int php_stream_flush(php_stream * stream);
PHPAPI char *php_stream_gets(php_stream * stream, char *buf, size_t maxlen);

/* operations for a stdio FILE; the FILE * must be placed in stream->abstract */
extern php_stream_ops php_stream_stdio_ops;
/* like fopen, but returns a stream */
PHPAPI php_stream * php_stream_fopen(const char * filename, const char * mode);

/* coerce the stream into some other form */
/* cast as a stdio FILE * */
#define PHP_STREAM_AS_STDIO     0
/* cast as a POSIX fd or socketd */
#define PHP_STREAM_AS_FD		1
/* cast as a socketd */
#define PHP_STREAM_AS_SOCKETD	2

/* warning: once you have cast a stream as a FILE*, you probably should not use
   the php_stream_XXX api after that point, or you will confuse the buffering
   in FILE* and/or php_stream *
*/
PHPAPI int php_stream_cast(php_stream * stream, int castas, void ** ret, int show_err);
/* use this to check if a stream can be cast into another form */
#define php_stream_can_cast(stream, as)	php_stream_cast(stream, as, NULL, 0)

#endif /* HAVE_PHP_STREAM */

#endif
