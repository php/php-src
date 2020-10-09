/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Sterling Hughes <sterling@php.net>                           |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_BZ2_H
#define PHP_BZ2_H

#ifdef HAVE_BZ2

extern zend_module_entry bz2_module_entry;
#define phpext_bz2_ptr &bz2_module_entry

/* Bzip2 includes */
#include <bzlib.h>

#else
#define phpext_bz2_ptr NULL
#endif

#ifdef PHP_WIN32
#	ifdef PHP_BZ2_EXPORTS
#		define PHP_BZ2_API __declspec(dllexport)
#	elif defined(COMPILE_DL_BZ2)
#		define PHP_BZ2_API __declspec(dllimport)
#	else
#		define PHP_BZ2_API /* nothing special */
#	endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_BZ2_API __attribute__ ((visibility("default")))
#else
#	define PHP_BZ2_API
#endif

#include "php_version.h"
#define PHP_BZ2_VERSION PHP_VERSION

PHP_BZ2_API php_stream *_php_stream_bz2open(php_stream_wrapper *wrapper, const char *path, const char *mode, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC);
PHP_BZ2_API php_stream *_php_stream_bz2open_from_BZFILE(BZFILE *bz, const char *mode, php_stream *innerstream STREAMS_DC);

#define php_stream_bz2open_from_BZFILE(bz, mode, innerstream)	_php_stream_bz2open_from_BZFILE((bz), (mode), (innerstream) STREAMS_CC)
#define php_stream_bz2open(wrapper, path, mode, options, opened_path)	_php_stream_bz2open((wrapper), (path), (mode), (options), (opened_path), NULL STREAMS_CC)

extern const php_stream_filter_factory php_bz2_filter_factory;
extern const php_stream_ops php_stream_bz2io_ops;
#define PHP_STREAM_IS_BZIP2	&php_stream_bz2io_ops

/* 400kb */
#define PHP_BZ2_FILTER_DEFAULT_BLOCKSIZE        4

/* BZ2 Internal Default */
#define PHP_BZ2_FILTER_DEFAULT_WORKFACTOR       0

#endif
