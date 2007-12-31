/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
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

#if HAVE_BZ2

extern zend_module_entry bz2_module_entry;
#define phpext_bz2_ptr &bz2_module_entry

/* Bzip2 includes */
#include <bzlib.h>

PHP_MINIT_FUNCTION(bz2);
PHP_MSHUTDOWN_FUNCTION(bz2);
PHP_MINFO_FUNCTION(bz2);
PHP_FUNCTION(bzopen);
PHP_FUNCTION(bzread);
PHP_FUNCTION(bzerrno);
PHP_FUNCTION(bzerrstr);
PHP_FUNCTION(bzerror);
PHP_FUNCTION(bzcompress);
PHP_FUNCTION(bzdecompress);

#else
#define phpext_bz2_ptr NULL
#endif

#ifdef PHP_WIN32
# ifdef PHP_BZ2_EXPORTS
# define PHP_BZ2_API __declspec(dllexport)
# else
# define PHP_BZ2_API __declspec(dllimport)
# endif
#else
# define PHP_BZ2_API
#endif

PHP_BZ2_API PHPAPI php_stream *_php_stream_bz2open(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
PHP_BZ2_API PHPAPI php_stream *_php_stream_bz2open_from_BZFILE(BZFILE *bz, char *mode, php_stream *innerstream STREAMS_DC TSRMLS_DC);

#define php_stream_bz2open_from_BZFILE(bz, mode, innerstream)	_php_stream_bz2open_from_BZFILE((bz), (mode), (innerstream) STREAMS_CC TSRMLS_CC)
#define php_stream_bz2open(wrapper, path, mode, options, opened_path)	_php_stream_bz2open((wrapper), (path), (mode), (options), (opened_path), NULL STREAMS_CC TSRMLS_CC)

php_stream_ops php_stream_bz2io_ops;
#define PHP_STREAM_IS_BZIP2	&php_stream_bz2io_ops

#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
