/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Stefan R�hrich <sr@linux.de>                                |
   |          Michael Wallner <mike@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_ZLIB_H
#define PHP_ZLIB_H

#include "php_version.h"
#define PHP_ZLIB_VERSION PHP_VERSION

#include <zlib.h>

#define PHP_ZLIB_ENCODING_RAW		-0xf
#define PHP_ZLIB_ENCODING_GZIP		0x1f
#define PHP_ZLIB_ENCODING_DEFLATE	0x0f

#define PHP_ZLIB_ENCODING_ANY		0x2f

#define PHP_ZLIB_OUTPUT_HANDLER_NAME "zlib output compression"
#define PHP_ZLIB_BUFFER_SIZE_GUESS(in_len) (((size_t) ((double) in_len * (double) 1.015)) + 10 + 8 + 4 + 1)

typedef struct _php_zlib_buffer {
	char *data;
	char *aptr;
	size_t used;
	size_t free;
	size_t size;
} php_zlib_buffer;

typedef struct _php_zlib_context {
	z_stream Z;
	char *inflateDict;
	size_t inflateDictlen;
	php_zlib_buffer buffer;
} php_zlib_context;

ZEND_BEGIN_MODULE_GLOBALS(zlib)
	/* variables for transparent gzip encoding */
	zend_long output_compression;
	zend_long output_compression_level;
	char *output_handler;
	php_zlib_context *ob_gzhandler;
	zend_long output_compression_default;
	zend_bool handler_registered;
	int compression_coding;
ZEND_END_MODULE_GLOBALS(zlib);

#define ZLIBG(v) ZEND_MODULE_GLOBALS_ACCESSOR(zlib, v)

php_stream *php_stream_gzopen(php_stream_wrapper *wrapper, const char *path, const char *mode, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC);
extern php_stream_ops php_stream_gzio_ops;
extern php_stream_wrapper php_stream_gzip_wrapper;
extern php_stream_filter_factory php_zlib_filter_factory;
extern zend_module_entry php_zlib_module_entry;
#define zlib_module_ptr &php_zlib_module_entry
#define phpext_zlib_ptr zlib_module_ptr

#endif /* PHP_ZLIB_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
