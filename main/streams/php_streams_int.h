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
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

#if ZEND_DEBUG

#define emalloc_rel_orig(size)	\
		( __php_stream_call_depth == 0 \
		? _emalloc((size) ZEND_FILE_LINE_CC ZEND_FILE_LINE_RELAY_CC) \
		: _emalloc((size) ZEND_FILE_LINE_CC ZEND_FILE_LINE_ORIG_RELAY_CC) )

#define erealloc_rel_orig(ptr, size)	\
		( __php_stream_call_depth == 0 \
		? _erealloc((ptr), (size), 0 ZEND_FILE_LINE_CC ZEND_FILE_LINE_RELAY_CC) \
		: _erealloc((ptr), (size), 0 ZEND_FILE_LINE_CC ZEND_FILE_LINE_ORIG_RELAY_CC) )

#define pemalloc_rel_orig(size, persistent)	((persistent) ? malloc((size)) : emalloc_rel_orig((size)))
#define perealloc_rel_orig(ptr, size, persistent)	((persistent) ? realloc((ptr), (size)) : erealloc_rel_orig((ptr), (size)))
#else
# define pemalloc_rel_orig(size, persistent)				pemalloc((size), (persistent))
# define perealloc_rel_orig(ptr, size, persistent)			perealloc((ptr), (size), (persistent))
# define emalloc_rel_orig(size)								emalloc((size))
#endif

#define STREAM_DEBUG 0
#define STREAM_WRAPPER_PLAIN_FILES	((php_stream_wrapper*)-1)

#ifndef MAP_FAILED
#define MAP_FAILED ((void *) -1)
#endif

#define CHUNK_SIZE	8192

#ifdef PHP_WIN32
# ifdef EWOULDBLOCK
#  undef EWOULDBLOCK
# endif
# define EWOULDBLOCK WSAEWOULDBLOCK
# ifdef EMSGSIZE
#  undef EMSGSIZE
# endif
# define EMSGSIZE WSAEMSGSIZE
#endif

/* This functions transforms the first char to 'w' if it's not 'r', 'a' or 'w'
 * and strips any subsequent chars except '+' and 'b'.
 * Use this to sanitize stream->mode if you call e.g. fdopen, fopencookie or
 * any other function that expects standard modes and you allow non-standard
 * ones. result should be a char[5]. */
void php_stream_mode_sanitize_fdopen_fopencookie(php_stream *stream, char *result);

void php_stream_tidy_wrapper_error_log(php_stream_wrapper *wrapper);
void php_stream_display_wrapper_errors(php_stream_wrapper *wrapper, const char *path, const char *caption);
