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
   | Author: Wez Furlong (wez@thebrainroom.com)                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_STREAMS_H
#define PHP_STREAMS_H

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

PHPAPI int php_file_le_stream(void);
PHPAPI int php_file_le_pstream(void);

/* {{{ Streams memory debugging stuff */

#if ZEND_DEBUG
/* these have more of a dependency on the definitions of the zend macros than
 * I would prefer, but doing it this way saves loads of idefs :-/ */
# define STREAMS_D			int __php_stream_call_depth ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC 
# define STREAMS_C			0 ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC 
# define STREAMS_REL_C		__php_stream_call_depth + 1 ZEND_FILE_LINE_CC, \
	__php_stream_call_depth ? __zend_orig_filename : __zend_filename, \
	__php_stream_call_depth ? __zend_orig_lineno : __zend_lineno 

# define STREAMS_DC		, STREAMS_D
# define STREAMS_CC		, STREAMS_C
# define STREAMS_REL_CC	, STREAMS_REL_C

#else
# define STREAMS_D
# define STREAMS_C
# define STREAMS_REL_C
# define STREAMS_DC
# define STREAMS_CC
# define STREAMS_REL_CC
#endif

/* these functions relay the file/line number information. They are depth aware, so they will pass
 * the ultimate ancestor, which is useful, because there can be several layers of calls */
#define php_stream_alloc_rel(ops, thisptr, persistent, mode) _php_stream_alloc((ops), (thisptr), (persistent), (mode) STREAMS_REL_CC TSRMLS_CC)

#define php_stream_copy_to_mem_rel(src, buf, maxlen, persistent) _php_stream_copy_to_mem((src), (buf), (maxlen), (persistent) STREAMS_REL_CC TSRMLS_CC)
	
#define php_stream_fopen_rel(filename, mode, opened, options) _php_stream_fopen((filename), (mode), (opened), (options) STREAMS_REL_CC TSRMLS_CC)

#define php_stream_fopen_with_path_rel(filename, mode, path, opened, options) _php_stream_fopen_with_path((filename), (mode), (path), (opened), (options) STREAMS_REL_CC TSRMLS_CC)

#define php_stream_fopen_from_file_rel(file, mode)	 _php_stream_fopen_from_file((file), (mode) STREAMS_REL_CC TSRMLS_CC)
	
#define php_stream_fopen_from_pipe_rel(file, mode)	 _php_stream_fopen_from_pipe((file), (mode) STREAMS_REL_CC TSRMLS_CC)
	
#define php_stream_fopen_tmpfile_rel()	_php_stream_fopen_tmpfile(0 STREAMS_REL_CC TSRMLS_CC)

#define php_stream_fopen_temporary_file_rel(dir, pfx, opened_path)	_php_stream_fopen_temporary_file((dir), (pfx), (opened_path) STREAMS_REL_CC TSRMLS_CC)
	
#define php_stream_open_wrapper_rel(path, mode, options, opened) _php_stream_open_wrapper_ex((path), (mode), (options), (opened), NULL STREAMS_REL_CC TSRMLS_CC)
#define php_stream_open_wrapper_ex_rel(path, mode, options, opened, context) _php_stream_open_wrapper_ex((path), (mode), (options), (opened), (context) STREAMS_REL_CC TSRMLS_CC)

#define php_stream_make_seekable_rel(origstream, newstream, flags) _php_stream_make_seekable((origstream), (newstream), (flags) STREAMS_REL_CC TSRMLS_CC)

/* }}} */
	
/* The contents of the php_stream_ops and php_stream should only be accessed
 * using the functions/macros in this header.
 * If you need to get at something that doesn't have an API,
 * drop me a line <wez@thebrainroom.com> and we can sort out a way to do
 * it properly.
 * 
 * The only exceptions to this rule are that stream implementations can use
 * the php_stream->abstract pointer to hold their context, and streams
 * opened via stream_open_wrappers can use the zval ptr in
 * php_stream->wrapperdata to hold meta data for php scripts to
 * retrieve using file_get_wrapper_data(). */

typedef struct _php_stream php_stream;
typedef struct _php_stream_wrapper php_stream_wrapper;
typedef struct _php_stream_context php_stream_context;
typedef struct _php_stream_filter php_stream_filter;

/* callback for status notifications */
typedef void (*php_stream_notification_func)(php_stream_context *context,
		int notifycode, int severity,
		char *xmsg, int xcode,
		size_t bytes_sofar, size_t bytes_max,
		void * ptr TSRMLS_DC);

typedef struct _php_stream_statbuf {
#if defined(NETWARE) && defined(CLIB_STAT_PATCH)
	struct stat_libc sb; /* regular info */
#else
	struct stat sb; /* regular info */
#endif
	/* extended info to go here some day: content-type etc. etc. */
} php_stream_statbuf;

typedef struct _php_stream_dirent {
	char d_name[MAXPATHLEN];
} php_stream_dirent;

#define PHP_STREAM_NOTIFIER_PROGRESS	1

typedef struct _php_stream_notifier {
	php_stream_notification_func func;
	void *ptr;
	int mask;
	size_t progress, progress_max; /* position for progress notification */
} php_stream_notifier;

struct _php_stream_context {
	php_stream_notifier *notifier;
	zval *options;	/* hash keyed by wrapper family or specific wrapper */
};

typedef struct _php_stream_wrapper_options {
	int valid_options; /* PHP_STREAM_WRAPPER_OPT_XXX */
	
	php_stream_notification_func notifier;
	void *notifier_ptr;

	/* other info like user-agent, SSL cert and cookie information
	 * to go here some day */
} php_stream_wrapper_options;

/* operations on streams that are file-handles */
typedef struct _php_stream_ops  {
	/* stdio like functions - these are mandatory! */
	size_t (*write)(php_stream *stream, const char *buf, size_t count TSRMLS_DC);
	size_t (*read)(php_stream *stream, char *buf, size_t count TSRMLS_DC);
	int    (*close)(php_stream *stream, int close_handle TSRMLS_DC);
	int    (*flush)(php_stream *stream TSRMLS_DC);
	
	const char *label; /* label for this ops structure */
	
	/* these are optional */
	int (*seek)(php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC);
	int (*cast)(php_stream *stream, int castas, void **ret TSRMLS_DC);
	int (*stat)(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC);
	int (*set_option)(php_stream *stream, int option, int value, void *ptrparam TSRMLS_DC);
} php_stream_ops;

typedef struct _php_stream_wrapper_ops {
	/* open/create a wrapped stream */
	php_stream *(*stream_opener)(php_stream_wrapper *wrapper, char *filename, char *mode,
			int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
	/* close/destroy a wrapped stream */
	int (*stream_closer)(php_stream_wrapper *wrapper, php_stream *stream TSRMLS_DC);
	/* stat a wrapped stream */
	int (*stream_stat)(php_stream_wrapper *wrapper, php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC);
	/* stat a URL */
	int (*url_stat)(php_stream_wrapper *wrapper, char *url, php_stream_statbuf *ssb TSRMLS_DC);
	/* open a "directory" stream */
	php_stream *(*dir_opener)(php_stream_wrapper *wrapper, char *filename, char *mode,
			int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
	
	const char *label;
} php_stream_wrapper_ops;

struct _php_stream_wrapper	{
	php_stream_wrapper_ops *wops;	/* operations the wrapper can perform */
	void *abstract;					/* context for the wrapper */
	int is_url;						/* so that PG(allow_url_fopen) can be respected */

	/* support for wrappers to return (multiple) error messages to the stream opener */
	int err_count;
	char **err_stack;
};

typedef struct _php_stream_filter_ops {
	size_t (*write)(php_stream *stream, php_stream_filter *thisfilter,
			const char *buf, size_t count TSRMLS_DC);
	size_t (*read)(php_stream *stream, php_stream_filter *thisfilter,
			char *buf, size_t count TSRMLS_DC);
	int (*flush)(php_stream *stream, php_stream_filter *thisfilter, int closing TSRMLS_DC);
	int (*eof)(php_stream *stream, php_stream_filter *thisfilter TSRMLS_DC);
	void (*dtor)(php_stream_filter *thisfilter TSRMLS_DC);
	const char *label;
} php_stream_filter_ops;

struct _php_stream_filter {
	php_stream_filter_ops *fops;
	void *abstract; /* for use by filter implementation */
	php_stream_filter *next;
	php_stream_filter *prev;
	int is_persistent;
	php_stream *stream;
};

#define php_stream_filter_write_next(stream, thisfilter, buf, size) \
	(thisfilter)->next ? (thisfilter)->next->fops->write((stream), (thisfilter)->next, (buf), (size) TSRMLS_CC) \
	: (stream)->ops->write((stream), (buf), (size) TSRMLS_CC)

#define php_stream_filter_read_next(stream, thisfilter, buf, size) \
	(thisfilter)->next ? (thisfilter)->next->fops->read((stream), (thisfilter)->next, (buf), (size) TSRMLS_CC) \
	: (stream)->ops->read((stream), (buf), (size) TSRMLS_CC)

#define php_stream_filter_flush_next(stream, thisfilter, closing) \
	(thisfilter)->next ? (thisfilter)->next->fops->flush((stream), (thisfilter)->next, (closing) TSRMLS_CC) \
	: (stream)->ops->flush((stream) TSRMLS_CC)

#define php_stream_filter_eof_next(stream, thisfilter) \
	(thisfilter)->next ? (thisfilter)->next->fops->eof((stream), (thisfilter)->next TSRMLS_CC) \
	: (stream)->ops->read((stream), NULL, 0 TSRMLS_CC) == EOF ? 1 : 0

#define PHP_STREAM_FLAG_NO_SEEK						1
#define PHP_STREAM_FLAG_NO_BUFFER					2

#define PHP_STREAM_FLAG_EOL_UNIX					0 /* also includes DOS */
#define PHP_STREAM_FLAG_DETECT_EOL					4
#define PHP_STREAM_FLAG_EOL_MAC						8

/* set this when the stream might represent "interactive" data.
 * When set, the read buffer will avoid certain operations that
 * might otherwise cause the read to block for much longer than
 * is strictly required. */
#define PHP_STREAM_FLAG_AVOID_BLOCKING				16
	
struct _php_stream  {
	php_stream_ops *ops;
	void *abstract;  		/* convenience pointer for abstraction */

	php_stream_filter *filterhead;
	php_stream_filter *filtertail;
	
	php_stream_wrapper *wrapper; /* which wrapper was used to open the stream */
	void *wrapperthis;		/* convenience pointer for a instance of a wrapper */
	zval *wrapperdata;		/* fgetwrapperdata retrieves this */

	int fgetss_state;		/* for fgetss to handle multiline tags */
	int is_persistent;
	char mode[16];			/* "rwb" etc. ala stdio */
	int rsrc_id;			/* used for auto-cleanup */
	int in_free;			/* to prevent recursion during free */
	/* so we know how to clean it up correctly.  This should be set to
	 * PHP_STREAM_FCLOSE_XXX as appropriate */
	int fclose_stdiocast;
	FILE *stdiocast;    /* cache this, otherwise we might leak! */
#if ZEND_DEBUG
	int __exposed;	/* non-zero if exposed as a zval somewhere */
	char *__orig_path; /* it really helps when debugging "unclosed" streams */
#endif

	php_stream_context *context;
	int flags;	/* PHP_STREAM_FLAG_XXX */

	/* buffer */
	off_t position; /* of underlying stream */
	unsigned char *readbuf;
	size_t readbuflen;
	off_t readpos;
	off_t writepos;
	
	/* how much data to read when filling buffer */
	size_t chunk_size;

	int eof;

}; /* php_stream */
/* state definitions when closing down; these are private to streams.c */
#define PHP_STREAM_FCLOSE_NONE 0
#define PHP_STREAM_FCLOSE_FDOPEN	1
#define PHP_STREAM_FCLOSE_FOPENCOOKIE 2

/* allocate a new stream for a particular ops */
PHPAPI php_stream *_php_stream_alloc(php_stream_ops *ops, void *abstract,
		const char *persistent_id, const char *mode STREAMS_DC TSRMLS_DC);
#define php_stream_alloc(ops, thisptr, persistent_id, mode)	_php_stream_alloc((ops), (thisptr), (persistent_id), (mode) STREAMS_CC TSRMLS_CC)

/* stack filter onto a stream */
PHPAPI void php_stream_filter_prepend(php_stream *stream, php_stream_filter *filter);
PHPAPI void php_stream_filter_append(php_stream *stream, php_stream_filter *filter);
PHPAPI php_stream_filter *php_stream_filter_remove(php_stream *stream, php_stream_filter *filter, int call_dtor TSRMLS_DC);
PHPAPI void php_stream_filter_free(php_stream_filter *filter TSRMLS_DC);
PHPAPI php_stream_filter *_php_stream_filter_alloc(php_stream_filter_ops *fops, void *abstract, int persistent STREAMS_DC TSRMLS_DC);
PHPAPI char *php_stream_get_record(php_stream *stream, size_t maxlen, size_t *returned_len, char *delim, size_t delim_len TSRMLS_DC);
#define php_stream_filter_alloc(fops, thisptr, persistent) _php_stream_filter_alloc((fops), (thisptr), (persistent) STREAMS_CC TSRMLS_CC)
#define php_stream_filter_alloc_rel(fops, thisptr, persistent) _php_stream_filter_alloc((fops), (thisptr), (persistent) STREAMS_REL_CC TSRMLS_CC)

#define php_stream_filter_remove_head(stream, call_dtor)	php_stream_filter_remove((stream), (stream)->filterhead, (call_dtor) TSRMLS_CC)
#define php_stream_filter_remove_tail(stream, call_dtor)	php_stream_filter_remove((stream), (stream)->filtertail, (call_dtor) TSRMLS_CC)

typedef struct _php_stream_filter_factory {
	php_stream_filter *(*create_filter)(const char *filtername, const char *filterparams, int filterparamslen, int persistent TSRMLS_DC);
} php_stream_filter_factory;

PHPAPI int php_stream_filter_register_factory(const char *filterpattern, php_stream_filter_factory *factory TSRMLS_DC);
PHPAPI int php_stream_filter_unregister_factory(const char *filterpattern TSRMLS_DC);
PHPAPI php_stream_filter *php_stream_filter_create(const char *filtername, const char *filterparams, int filterparamslen, int persistent TSRMLS_DC);


#define php_stream_get_resource_id(stream)		(stream)->rsrc_id
#if ZEND_DEBUG
/* use this to tell the stream that it is OK if we don't explicitly close it */
# define php_stream_auto_cleanup(stream)	{ (stream)->__exposed++; }
/* use this to assign the stream to a zval and tell the stream that is
 * has been exported to the engine; it will expect to be closed automatically
 * when the resources are auto-destructed */
# define php_stream_to_zval(stream, zval)	{ ZVAL_RESOURCE(zval, (stream)->rsrc_id); (stream)->__exposed++; }
#else
# define php_stream_auto_cleanup(stream)	/* nothing */
# define php_stream_to_zval(stream, zval)	{ ZVAL_RESOURCE(zval, (stream)->rsrc_id); }
#endif

#define php_stream_from_zval(str, ppzval)	ZEND_FETCH_RESOURCE2((str), php_stream *, (ppzval), -1, "stream", php_file_le_stream(), php_file_le_pstream())
#define php_stream_from_zval_no_verify(str, ppzval)	(str) = (php_stream*)zend_fetch_resource((ppzval) TSRMLS_CC, -1, "stream", NULL, 2, php_file_le_stream(), php_file_le_pstream())

PHPAPI int php_stream_from_persistent_id(const char *persistent_id, php_stream **stream TSRMLS_DC);
#define PHP_STREAM_PERSISTENT_SUCCESS	0 /* id exists */
#define PHP_STREAM_PERSISTENT_FAILURE	1 /* id exists but is not a stream! */
#define PHP_STREAM_PERSISTENT_NOT_EXIST	2 /* id does not exist */

#define PHP_STREAM_FREE_CALL_DTOR			1 /* call ops->close */
#define PHP_STREAM_FREE_RELEASE_STREAM		2 /* pefree(stream) */
#define PHP_STREAM_FREE_PRESERVE_HANDLE		4 /* tell ops->close to not close it's underlying handle */
#define PHP_STREAM_FREE_RSRC_DTOR			8 /* called from the resource list dtor */
#define PHP_STREAM_FREE_CLOSE				(PHP_STREAM_FREE_CALL_DTOR | PHP_STREAM_FREE_RELEASE_STREAM)
#define PHP_STREAM_FREE_CLOSE_CASTED		(PHP_STREAM_FREE_CLOSE | PHP_STREAM_FREE_PRESERVE_HANDLE)
PHPAPI int _php_stream_free(php_stream *stream, int close_options TSRMLS_DC);
#define php_stream_free(stream, close_options)	_php_stream_free((stream), (close_options) TSRMLS_CC)
#define php_stream_close(stream)	_php_stream_free((stream), PHP_STREAM_FREE_CLOSE TSRMLS_CC)

PHPAPI int _php_stream_seek(php_stream *stream, off_t offset, int whence TSRMLS_DC);
#define php_stream_rewind(stream)	_php_stream_seek((stream), 0L, SEEK_SET TSRMLS_CC)
#define php_stream_seek(stream, offset, whence)	_php_stream_seek((stream), (offset), (whence) TSRMLS_CC)

PHPAPI off_t _php_stream_tell(php_stream *stream TSRMLS_DC);
#define php_stream_tell(stream)	_php_stream_tell((stream) TSRMLS_CC)

PHPAPI size_t _php_stream_read(php_stream *stream, char *buf, size_t count TSRMLS_DC);
#define php_stream_read(stream, buf, count)		_php_stream_read((stream), (buf), (count) TSRMLS_CC)

PHPAPI size_t _php_stream_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC);
#define php_stream_write_string(stream, str)	_php_stream_write(stream, str, strlen(str) TSRMLS_CC)
#define php_stream_write(stream, buf, count)	_php_stream_write(stream, (buf), (count) TSRMLS_CC)

PHPAPI size_t _php_stream_printf(php_stream *stream TSRMLS_DC, const char *fmt, ...);
/* php_stream_printf macro & function require TSRMLS_CC */
#define php_stream_printf _php_stream_printf

PHPAPI int _php_stream_eof(php_stream *stream TSRMLS_DC);
#define php_stream_eof(stream)	_php_stream_eof((stream) TSRMLS_CC)

PHPAPI int _php_stream_getc(php_stream *stream TSRMLS_DC);
#define php_stream_getc(stream)	_php_stream_getc((stream) TSRMLS_CC)

PHPAPI int _php_stream_putc(php_stream *stream, int c TSRMLS_DC);
#define php_stream_putc(stream, c)	_php_stream_putc((stream), (c) TSRMLS_CC)

PHPAPI int _php_stream_flush(php_stream *stream, int closing TSRMLS_DC);
#define php_stream_flush(stream)	_php_stream_flush((stream), 0 TSRMLS_CC)

PHPAPI char *_php_stream_get_line(php_stream *stream, char *buf, size_t maxlen, size_t *returned_len TSRMLS_DC);
#define php_stream_gets(stream, buf, maxlen)	_php_stream_get_line((stream), (buf), (maxlen), NULL TSRMLS_CC)

#define php_stream_get_line(stream, buf, maxlen, retlen) _php_stream_get_line((stream), (buf), (maxlen), (retlen) TSRMLS_CC)

/* CAREFUL! this is equivalent to puts NOT fputs! */
PHPAPI int _php_stream_puts(php_stream *stream, char *buf TSRMLS_DC);
#define php_stream_puts(stream, buf)	_php_stream_puts((stream), (buf) TSRMLS_CC)

PHPAPI int _php_stream_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC);
#define php_stream_stat(stream, ssb)	_php_stream_stat((stream), (ssb) TSRMLS_CC)

PHPAPI int _php_stream_stat_path(char *path, php_stream_statbuf *ssb TSRMLS_DC);
#define php_stream_stat_path(path, ssb)	_php_stream_stat_path((path), (ssb) TSRMLS_CC)

PHPAPI php_stream *_php_stream_opendir(char *path, int options, php_stream_context *context STREAMS_DC TSRMLS_DC);
#define php_stream_opendir(path, options, context)	_php_stream_opendir((path), (options), (context) STREAMS_CC TSRMLS_CC)
PHPAPI php_stream_dirent *_php_stream_readdir(php_stream *dirstream, php_stream_dirent *ent TSRMLS_DC);
#define php_stream_readdir(dirstream, dirent)	_php_stream_readdir((dirstream), (dirent) TSRMLS_CC)
#define php_stream_closedir(dirstream)	php_stream_close((dirstream))
#define php_stream_rewinddir(dirstream)	php_stream_rewind((dirstream))

PHPAPI int _php_stream_set_option(php_stream *stream, int option, int value, void *ptrparam TSRMLS_DC);
#define php_stream_set_option(stream, option, value, ptrvalue)	_php_stream_set_option((stream), (option), (value), (ptrvalue) TSRMLS_CC)

#define php_stream_set_chunk_size(stream, size) _php_stream_set_option((stream), PHP_STREAM_OPTION_SET_CHUNK_SIZE, (size), NULL TSRMLS_CC)

/* change the blocking mode of stream: value == 1 => blocking, value == 0 => non-blocking. */
#define PHP_STREAM_OPTION_BLOCKING	1

/* change the buffering mode of stream. value is a PHP_STREAM_BUFFER_XXXX value, ptrparam is a ptr to a size_t holding
 * the required buffer size */
#define PHP_STREAM_OPTION_READ_BUFFER	2
#define PHP_STREAM_OPTION_WRITE_BUFFER	3

#define PHP_STREAM_BUFFER_NONE	0	/* unbuffered */
#define PHP_STREAM_BUFFER_LINE	1	/* line buffered */
#define PHP_STREAM_BUFFER_FULL	2	/* fully buffered */

/* set the timeout duration for reads on the stream. ptrparam is a pointer to a struct timeval * */
#define PHP_STREAM_OPTION_READ_TIMEOUT	4
#define PHP_STREAM_OPTION_SET_CHUNK_SIZE	5

#define PHP_STREAM_OPTION_RETURN_OK			 0 /* option set OK */
#define PHP_STREAM_OPTION_RETURN_ERR 		-1 /* problem setting option */
#define PHP_STREAM_OPTION_RETURN_NOTIMPL	-2 /* underlying stream does not implement; streams can handle it instead */

/* copy up to maxlen bytes from src to dest.  If maxlen is PHP_STREAM_COPY_ALL, copy until eof(src).
 * Uses mmap if the src is a plain file and at offset 0 */
#define PHP_STREAM_COPY_ALL		-1
PHPAPI size_t _php_stream_copy_to_stream(php_stream *src, php_stream *dest, size_t maxlen STREAMS_DC TSRMLS_DC);
#define php_stream_copy_to_stream(src, dest, maxlen)	_php_stream_copy_to_stream((src), (dest), (maxlen) STREAMS_CC TSRMLS_CC)


/* read all data from stream and put into a buffer. Caller must free buffer when done.
 * The copy will use mmap if available. */
PHPAPI size_t _php_stream_copy_to_mem(php_stream *src, char **buf, size_t maxlen,
		int persistent STREAMS_DC TSRMLS_DC);
#define php_stream_copy_to_mem(src, buf, maxlen, persistent) _php_stream_copy_to_mem((src), (buf), (maxlen), (persistent) STREAMS_CC TSRMLS_CC)

/* output all data from a stream */
PHPAPI size_t _php_stream_passthru(php_stream * src STREAMS_DC TSRMLS_DC);
#define php_stream_passthru(stream)	_php_stream_passthru((stream) STREAMS_CC TSRMLS_CC)

/* operations for a stdio FILE; use the php_stream_fopen_XXX funcs below */
PHPAPI extern php_stream_ops php_stream_stdio_ops;
/* like fopen, but returns a stream */
PHPAPI php_stream *_php_stream_fopen(const char *filename, const char *mode, char **opened_path, int options STREAMS_DC TSRMLS_DC);
#define php_stream_fopen(filename, mode, opened)	_php_stream_fopen((filename), (mode), (opened), 0 STREAMS_CC TSRMLS_CC)

PHPAPI php_stream *_php_stream_fopen_with_path(char *filename, char *mode, char *path, char **opened_path, int options STREAMS_DC TSRMLS_DC);
#define php_stream_fopen_with_path(filename, mode, path, opened)	_php_stream_fopen_with_path((filename), (mode), (path), (opened) STREAMS_CC TSRMLS_CC)

PHPAPI php_stream *_php_stream_fopen_from_file(FILE *file, const char *mode STREAMS_DC TSRMLS_DC);
#define php_stream_fopen_from_file(file, mode)	_php_stream_fopen_from_file((file), (mode) STREAMS_CC TSRMLS_CC)

PHPAPI php_stream *_php_stream_fopen_from_pipe(FILE *file, const char *mode STREAMS_DC TSRMLS_DC);
#define php_stream_fopen_from_pipe(file, mode)	_php_stream_fopen_from_pipe((file), (mode) STREAMS_CC TSRMLS_CC)

PHPAPI php_stream *_php_stream_fopen_tmpfile(int dummy STREAMS_DC TSRMLS_DC);
#define php_stream_fopen_tmpfile()	_php_stream_fopen_tmpfile(0 STREAMS_CC TSRMLS_CC)

PHPAPI php_stream *_php_stream_fopen_temporary_file(const char *dir, const char *pfx, char **opened_path STREAMS_DC TSRMLS_DC);
#define php_stream_fopen_temporary_file(dir, pfx, opened_path)	_php_stream_fopen_temporary_file((dir), (pfx), (opened_path) STREAMS_CC TSRMLS_CC)

/* coerce the stream into some other form */
/* cast as a stdio FILE * */
#define PHP_STREAM_AS_STDIO     0
/* cast as a POSIX fd or socketd */
#define PHP_STREAM_AS_FD		1
/* cast as a socketd */
#define PHP_STREAM_AS_SOCKETD	2

/* try really, really hard to make sure the cast happens (avoid using this flag if possible) */
#define PHP_STREAM_CAST_TRY_HARD	0x80000000
#define PHP_STREAM_CAST_RELEASE		0x40000000	/* stream becomes invalid on success */
#define PHP_STREAM_CAST_INTERNAL	0x20000000	/* stream cast for internal use */
#define PHP_STREAM_CAST_MASK		(PHP_STREAM_CAST_TRY_HARD | PHP_STREAM_CAST_RELEASE | PHP_STREAM_CAST_INTERNAL)
PHPAPI int _php_stream_cast(php_stream *stream, int castas, void **ret, int show_err TSRMLS_DC);
/* use this to check if a stream can be cast into another form */
#define php_stream_can_cast(stream, as)	_php_stream_cast((stream), (as), NULL, 0 TSRMLS_CC)
#define php_stream_cast(stream, as, ret, show_err)	_php_stream_cast((stream), (as), (ret), (show_err) TSRMLS_CC)

/* use this to check if a stream is of a particular type:
 * PHPAPI int php_stream_is(php_stream *stream, php_stream_ops *ops); */
#define php_stream_is(stream, anops)		((stream)->ops == anops)
#define PHP_STREAM_IS_STDIO &php_stream_stdio_ops

#define php_stream_is_persistent(stream)	(stream)->is_persistent

/* Wrappers support */

#define IGNORE_PATH			0
#define USE_PATH			1
#define IGNORE_URL			2
#define ENFORCE_SAFE_MODE 	4
#define REPORT_ERRORS		8
/* If you don't need to write to the stream, but really need to
 * be able to seek, use this flag in your options. */
#define STREAM_MUST_SEEK	16
/* If you are going to end up casting the stream into a FILE* or
 * a socket, pass this flag and the streams/wrappers will not use
 * buffering mechanisms while reading the headers, so that HTTP
 * wrapped streams will work consistently.
 * If you omit this flag, streams will use buffering and should end 
 * up working more optimally.
 * */
#define STREAM_WILL_CAST	32

/* this flag applies to php_stream_locate_url_wrapper */
#define STREAM_LOCATE_WRAPPERS_ONLY	64

/* this flag is only used by include/require functions */
#define STREAM_OPEN_FOR_INCLUDE		128

/* this flag tells streams to ONLY open urls */
#define STREAM_USE_URL			256

/* this flag is used when only the headers from HTTP request are to be fetched */
#define STREAM_ONLY_GET_HEADERS		512

/* Antique - no longer has meaning */
#define IGNORE_URL_WIN 0

int php_init_stream_wrappers(int module_number TSRMLS_DC);
int php_shutdown_stream_wrappers(int module_number TSRMLS_DC);
PHP_RSHUTDOWN_FUNCTION(streams);

PHPAPI int php_register_url_stream_wrapper(char *protocol, php_stream_wrapper *wrapper TSRMLS_DC);
PHPAPI int php_unregister_url_stream_wrapper(char *protocol TSRMLS_DC);
PHPAPI php_stream *_php_stream_open_wrapper_ex(char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
PHPAPI php_stream_wrapper *php_stream_locate_url_wrapper(const char *path, char **path_for_open, int options TSRMLS_DC);
PHPAPI char *php_stream_locate_eol(php_stream *stream, char *buf, size_t buf_len TSRMLS_DC);

#define php_stream_open_wrapper(path, mode, options, opened)	_php_stream_open_wrapper_ex((path), (mode), (options), (opened), NULL STREAMS_CC TSRMLS_CC)
#define php_stream_open_wrapper_ex(path, mode, options, opened, context)	_php_stream_open_wrapper_ex((path), (mode), (options), (opened), (context) STREAMS_CC TSRMLS_CC)

/* pushes an error message onto the stack for a wrapper instance */
PHPAPI void php_stream_wrapper_log_error(php_stream_wrapper *wrapper, int options TSRMLS_DC, const char *fmt, ...);


#define PHP_STREAM_UNCHANGED	0 /* orig stream was seekable anyway */
#define PHP_STREAM_RELEASED		1 /* newstream should be used; origstream is no longer valid */
#define PHP_STREAM_FAILED		2 /* an error occurred while attempting conversion */
#define PHP_STREAM_CRITICAL		3 /* an error occurred; origstream is in an unknown state; you should close origstream */
#define PHP_STREAM_NO_PREFERENCE	0
#define PHP_STREAM_PREFER_STDIO		1
#define PHP_STREAM_FORCE_CONVERSION	2
/* DO NOT call this on streams that are referenced by resources! */
PHPAPI int _php_stream_make_seekable(php_stream *origstream, php_stream **newstream, int flags STREAMS_DC TSRMLS_DC);
#define php_stream_make_seekable(origstream, newstream, flags)	_php_stream_make_seekable((origstream), (newstream), (flags) STREAMS_CC TSRMLS_CC)

/* This is a utility API for extensions that are opening a stream, converting it
 * to a FILE* and then closing it again.  Be warned that fileno() on the result
 * will most likely fail on systems with fopencookie. */
PHPAPI FILE * _php_stream_open_wrapper_as_file(char * path, char * mode, int options, char **opened_path STREAMS_DC TSRMLS_DC);
#define php_stream_open_wrapper_as_file(path, mode, options, opened_path) _php_stream_open_wrapper_as_file((path), (mode), (options), (opened_path) STREAMS_CC TSRMLS_CC)

/* for user-space streams */
PHPAPI extern php_stream_ops php_stream_userspace_ops;
PHPAPI extern php_stream_ops php_stream_userspace_dir_ops;
#define PHP_STREAM_IS_USERSPACE	&php_stream_userspace_ops
#define PHP_STREAM_IS_USERSPACE_DIR	&php_stream_userspace_dir_ops

PHPAPI void php_stream_context_free(php_stream_context *context);
PHPAPI php_stream_context *php_stream_context_alloc(void);
PHPAPI int php_stream_context_get_option(php_stream_context *context,
		const char *wrappername, const char *optionname, zval ***optionvalue);
PHPAPI int php_stream_context_set_option(php_stream_context *context,
		const char *wrappername, const char *optionname, zval *optionvalue);

PHPAPI php_stream_notifier *php_stream_notification_alloc(void);
PHPAPI void php_stream_notification_free(php_stream_notifier *notifier);

/* not all notification codes are implemented */
#define PHP_STREAM_NOTIFY_RESOLVE		1
#define PHP_STREAM_NOTIFY_CONNECT		2
#define PHP_STREAM_NOTIFY_AUTH_REQUIRED		3
#define PHP_STREAM_NOTIFY_MIME_TYPE_IS	4
#define PHP_STREAM_NOTIFY_FILE_SIZE_IS	5
#define PHP_STREAM_NOTIFY_REDIRECTED	6
#define PHP_STREAM_NOTIFY_PROGRESS		7
#define PHP_STREAM_NOTIFY_COMPLETED		8
#define PHP_STREAM_NOTIFY_FAILURE		9
#define PHP_STREAM_NOTIFY_AUTH_RESULT	10

#define PHP_STREAM_NOTIFY_SEVERITY_INFO	0
#define PHP_STREAM_NOTIFY_SEVERITY_WARN	1
#define PHP_STREAM_NOTIFY_SEVERITY_ERR	2

PHPAPI void php_stream_notification_notify(php_stream_context *context, int notifycode, int severity,
		char *xmsg, int xcode, size_t bytes_sofar, size_t bytes_max, void * ptr TSRMLS_DC);
PHPAPI php_stream_context *php_stream_context_set(php_stream *stream, php_stream_context *context);

#define php_stream_notify_info(context, code, xmsg, xcode)	do { if ((context) && (context)->notifier) { \
	php_stream_notification_notify((context), (code), PHP_STREAM_NOTIFY_SEVERITY_INFO, \
				(xmsg), (xcode), 0, 0, NULL TSRMLS_CC); } } while (0)
			
#define php_stream_notify_progress(context, bsofar, bmax) do { if ((context) && (context)->notifier) { \
	php_stream_notification_notify((context), PHP_STREAM_NOTIFY_PROGRESS, PHP_STREAM_NOTIFY_SEVERITY_INFO, \
			NULL, 0, (bsofar), (bmax), NULL TSRMLS_CC); } } while(0)

#define php_stream_notify_progress_init(context, sofar, bmax) do { if ((context) && (context)->notifier) { \
	(context)->notifier->progress = (sofar); \
	(context)->notifier->progress_max = (bmax); \
	(context)->notifier->mask |= PHP_STREAM_NOTIFIER_PROGRESS; \
	php_stream_notify_progress((context), (sofar), (bmax)); } } while (0)

#define php_stream_notify_progress_increment(context, dsofar, dmax) do { if ((context) && (context)->notifier && (context)->notifier->mask & PHP_STREAM_NOTIFIER_PROGRESS) { \
	(context)->notifier->progress += (dsofar); \
	(context)->notifier->progress_max += (dmax); \
	php_stream_notify_progress((context), (context)->notifier->progress, (context)->notifier->progress_max); } } while (0)

#define php_stream_notify_file_size(context, file_size, xmsg, xcode) do { if ((context) && (context)->notifier) { \
	php_stream_notification_notify((context), PHP_STREAM_NOTIFY_FILE_SIZE_IS, PHP_STREAM_NOTIFY_SEVERITY_INFO, \
			(xmsg), (xcode), 0, (file_size), NULL TSRMLS_CC); } } while(0)
	
#define php_stream_notify_error(context, code, xmsg, xcode) do { if ((context) && (context)->notifier) {\
	php_stream_notification_notify((context), (code), PHP_STREAM_NOTIFY_SEVERITY_ERR, \
			(xmsg), (xcode), 0, 0, NULL TSRMLS_CC); } } while(0)
	

/* Give other modules access to the url_stream_wrappers_hash and stream_filters_hash */
PHPAPI HashTable *php_stream_get_url_stream_wrappers_hash();
PHPAPI HashTable *php_get_stream_filters_hash();
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
