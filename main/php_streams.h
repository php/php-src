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
	
#define php_stream_fopen_rel(filename, mode, opened) _php_stream_fopen((filename), (mode), (opened) STREAMS_REL_CC TSRMLS_CC)

#define php_stream_fopen_with_path_rel(filename, mode, path, opened) _php_stream_fopen_with_path((filename), (mode), (path), (opened) STREAMS_REL_CC TSRMLS_CC)

#define php_stream_fopen_from_file_rel(file, mode)	 _php_stream_fopen_from_file((file), (mode) STREAMS_REL_CC TSRMLS_CC)
	
#define php_stream_fopen_from_pipe_rel(file, mode)	 _php_stream_fopen_from_pipe((file), (mode) STREAMS_REL_CC TSRMLS_CC)
	
#define php_stream_fopen_tmpfile_rel()	_php_stream_fopen_tmpfile(0 STREAMS_REL_CC TSRMLS_CC)

#define php_stream_fopen_temporary_file_rel(dir, pfx, opened_path)	_php_stream_fopen_temporary_file((dir), (pfx), (opened_path) STREAMS_REL_CC TSRMLS_CC)
	
#define php_stream_open_wrapper_rel(path, mode, options, opened) _php_stream_open_wrapper((path), (mode), (options), (opened) STREAMS_REL_CC TSRMLS_CC)

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
 * retrieve using fgetwrapperdata(). */

typedef struct _php_stream php_stream;

typedef struct _php_stream_ops  {
	/* stdio like functions - these are mandatory! */
	size_t (*write)(php_stream *stream, const char *buf, size_t count TSRMLS_DC);
	size_t (*read)(php_stream *stream, char *buf, size_t count TSRMLS_DC);
	int    (*close)(php_stream *stream, int close_handle TSRMLS_DC);
	int    (*flush)(php_stream *stream TSRMLS_DC);
	/* these are optional */
	int    (*seek)(php_stream *stream, off_t offset, int whence TSRMLS_DC);
	char *(*gets)(php_stream *stream, char *buf, size_t size TSRMLS_DC);
	int (*cast)(php_stream *stream, int castas, void **ret TSRMLS_DC);
	const char *label; /* label for this ops structure */
} php_stream_ops;

/* options uses the IGNORE_URL family of defines from fopen_wrappers.h */
typedef php_stream *(*php_stream_factory_func_t)(char *filename, char *mode, int options, char **opened_path, void * wrappercontext STREAMS_DC TSRMLS_DC);
typedef void (*php_stream_wrapper_dtor_func_t)(php_stream *stream TSRMLS_DC);

typedef struct _php_stream_wrapper	{
	php_stream_factory_func_t		create;
	php_stream_wrapper_dtor_func_t	destroy;
	void * wrappercontext;
} php_stream_wrapper;

struct _php_stream  {
	php_stream_ops *ops;
	void *abstract;  		/* convenience pointer for abstraction */

	php_stream_wrapper *wrapper; /* which wrapper was used to open the stream */
	void *wrapperthis;		/* convenience pointer for a instance of a wrapper */
	zval *wrapperdata;		/* fgetwrapperdata retrieves this */

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
#endif
}; /* php_stream */
/* state definitions when closing down; these are private to streams.c */
#define PHP_STREAM_FCLOSE_NONE 0
#define PHP_STREAM_FCLOSE_FDOPEN	1
#define PHP_STREAM_FCLOSE_FOPENCOOKIE 2

/* allocate a new stream for a particular ops */
PHPAPI php_stream *_php_stream_alloc(php_stream_ops *ops, void *abstract,
		int persistent, const char *mode STREAMS_DC TSRMLS_DC);
#define php_stream_alloc(ops, thisptr, persistent, mode)	_php_stream_alloc((ops), (thisptr), (persistent), (mode) STREAMS_CC TSRMLS_CC)

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

#define PHP_STREAM_FREE_CALL_DTOR			1 /* call ops->close */
#define PHP_STREAM_FREE_RELEASE_STREAM		2 /* pefree(stream) */
#define PHP_STREAM_FREE_PRESERVE_HANDLE		4 /* tell ops->close to not close it's underlying handle */
#define PHP_STREAM_FREE_RSRC_DTOR			8 /* called from the resource list dtor */
#define PHP_STREAM_FREE_CLOSE				(PHP_STREAM_FREE_CALL_DTOR | PHP_STREAM_FREE_RELEASE_STREAM)
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

PHPAPI int _php_stream_eof(php_stream *stream TSRMLS_DC);
#define php_stream_eof(stream)	_php_stream_eof((stream) TSRMLS_CC)

PHPAPI int _php_stream_getc(php_stream *stream TSRMLS_DC);
#define php_stream_getc(stream)	_php_stream_getc((stream) TSRMLS_CC)

PHPAPI int _php_stream_putc(php_stream *stream, int c TSRMLS_DC);
#define php_stream_putc(stream, c)	_php_stream_putc((stream), (c) TSRMLS_CC)

PHPAPI int _php_stream_flush(php_stream *stream TSRMLS_DC);
#define php_stream_flush(stream)	_php_stream_flush((stream) TSRMLS_CC)

PHPAPI char *_php_stream_gets(php_stream *stream, char *buf, size_t maxlen TSRMLS_DC);
#define php_stream_gets(stream, buf, maxlen)	_php_stream_gets((stream), (buf), (maxlen) TSRMLS_CC)

PHPAPI int _php_stream_puts(php_stream *stream, char *buf TSRMLS_DC);
#define php_stream_puts(stream, buf)	_php_stream_puts((stream), (buf) TSRMLS_CC)

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


/* maybe implement someday */
#define php_stream_error(stream)	(0)

/* operations for a stdio FILE; use the php_stream_fopen_XXX funcs below */
extern php_stream_ops php_stream_stdio_ops;
/* like fopen, but returns a stream */
PHPAPI php_stream *_php_stream_fopen(const char *filename, const char *mode, char **opened_path STREAMS_DC TSRMLS_DC);
#define php_stream_fopen(filename, mode, opened)	_php_stream_fopen((filename), (mode), (opened) STREAMS_CC TSRMLS_CC)

PHPAPI php_stream *_php_stream_fopen_with_path(char *filename, char *mode, char *path, char **opened_path STREAMS_DC TSRMLS_DC);
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

/* try really, really hard to make sure the cast happens (socketpair) */
#define PHP_STREAM_CAST_TRY_HARD	0x80000000
#define PHP_STREAM_CAST_RELEASE		0x40000000	/* stream becomes invalid on success */
#define PHP_STREAM_CAST_MASK		(PHP_STREAM_CAST_TRY_HARD | PHP_STREAM_CAST_RELEASE)
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
/* There's no USE_URL. */
#define ENFORCE_SAFE_MODE 	4
#define REPORT_ERRORS		8
/* If you don't need to write to the stream, but really need to
 * be able to seek, use this flag in your options. */
#define STREAM_MUST_SEEK	16

#ifdef PHP_WIN32
# define IGNORE_URL_WIN IGNORE_URL
#else
# define IGNORE_URL_WIN 0
#endif

int php_init_stream_wrappers(TSRMLS_D);
int php_shutdown_stream_wrappers(TSRMLS_D);
PHPAPI int php_register_url_stream_wrapper(char *protocol, php_stream_wrapper *wrapper TSRMLS_DC);
PHPAPI int php_unregister_url_stream_wrapper(char *protocol TSRMLS_DC);
PHPAPI php_stream *_php_stream_open_wrapper(char *path, char *mode, int options, char **opened_path STREAMS_DC TSRMLS_DC);
#define php_stream_open_wrapper(path, mode, options, opened)	_php_stream_open_wrapper((path), (mode), (options), (opened) STREAMS_CC TSRMLS_CC)

#define PHP_STREAM_UNCHANGED	0 /* orig stream was seekable anyway */
#define PHP_STREAM_RELEASED		1 /* newstream should be used; origstream is no longer valid */
#define PHP_STREAM_FAILED		2 /* an error occurred while attempting conversion */
#define PHP_STREAM_CRITICAL		3 /* an error occurred; origstream is in an unknown state; you should close origstream */
/* DO NOT call this on streams that are referenced by resources! */
#define PHP_STREAM_NO_PREFERENCE	0
#define PHP_STREAM_PREFER_STDIO		1
PHPAPI int _php_stream_make_seekable(php_stream *origstream, php_stream **newstream, int flags STREAMS_DC TSRMLS_DC);
#define php_stream_make_seekable(origstream, newstream, flags)	_php_stream_make_seekable((origstream), (newstream), (flags) STREAMS_CC TSRMLS_CC)

/* This is a utility API for extensions that are opening a stream, converting it
 * to a FILE* and then closing it again.  Be warned that fileno() on the result
 * will most likely fail on systems with fopencookie. */
PHPAPI FILE * _php_stream_open_wrapper_as_file(char * path, char * mode, int options, char **opened_path STREAMS_DC TSRMLS_DC);
#define php_stream_open_wrapper_as_file(path, mode, options, opened_path) _php_stream_open_wrapper_as_file((path), (mode), (options), (opened_path) STREAMS_CC TSRMLS_CC)

/* for user-space streams */
extern php_stream_ops php_stream_userspace_ops;
#define PHP_STREAM_IS_USERSPACE	&php_stream_userspace_ops

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
