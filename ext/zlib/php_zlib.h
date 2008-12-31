/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
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
   |          Stefan Röhrich <sr@linux.de>                                |
   |          Michael Wallner <mike@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_ZLIB_H
#define PHP_ZLIB_H

#include <zlib.h>

#define PHP_ZLIB_ENCODING_RAW		-0xf
#define PHP_ZLIB_ENCODING_GZIP		0x1f
#define PHP_ZLIB_ENCODING_DEFLATE	0x0f

#define PHP_ZLIB_ENCODING_ANY		0x2f

#define PHP_ZLIB_OUTPUT_HANDLER_NAME "zlib output compression"
#define PHP_ZLIB_BUFFER_SIZE_GUESS(in_len) (((size_t) ((double) in_len * (double) 1.015)) + 10 + 8 + 4 + 1)

ZEND_BEGIN_MODULE_GLOBALS(zlib)
	/* variables for transparent gzip encoding */
	int compression_coding;
	long output_compression;
	long output_compression_level;
	char *output_handler;
ZEND_END_MODULE_GLOBALS(zlib);

typedef struct _php_zlib_buffer {
	char *data;
	char *aptr;
	size_t used;
	size_t free;
	size_t size;
} php_zlib_buffer;

typedef struct _php_zlib_context {
	z_stream Z;
	php_zlib_buffer buffer;
} php_zlib_context;

int php_zlib_output_encoding(TSRMLS_D);
int php_zlib_encode(const char *in_buf, size_t in_len, char **out_buf, size_t *out_len, int encoding, int level TSRMLS_DC);
int php_zlib_decode(const char *in_buf, size_t in_len, char **out_buf, size_t *out_len, int encoding, size_t max_len TSRMLS_DC);
php_output_handler *php_zlib_output_handler_init(zval *handler_name, size_t chunk_size, int flags TSRMLS_DC);
int php_zlib_output_handler(void **handler_context, php_output_context *output_context);
void php_zlib_output_handler_dtor(void *opaq TSRMLS_DC);
int php_zlib_output_conflict_check(zval *handler_name TSRMLS_DC);

php_stream *php_stream_gzopen(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
extern php_stream_ops php_stream_gzio_ops;
extern php_stream_wrapper php_stream_gzip_wrapper;
extern php_stream_filter_factory php_zlib_filter_factory;
extern zend_module_entry php_zlib_module_entry;
#define zlib_module_ptr &php_zlib_module_entry
#define phpext_zlib_ptr zlib_module_ptr

PHP_MINIT_FUNCTION(zlib);
PHP_MSHUTDOWN_FUNCTION(zlib);
PHP_RINIT_FUNCTION(zlib);
PHP_MINFO_FUNCTION(zlib);

PHP_FUNCTION(gzfile);
PHP_FUNCTION(gzopen);
PHP_FUNCTION(readgzfile);

PHP_FUNCTION(gzcompress);
PHP_FUNCTION(gzuncompress);
PHP_FUNCTION(gzdeflate);
PHP_FUNCTION(gzinflate);
PHP_FUNCTION(gzencode);
PHP_FUNCTION(gzdecode);

PHP_FUNCTION(zlib_encode);
PHP_FUNCTION(zlib_decode);

PHP_FUNCTION(zlib_get_coding_type);


#ifdef ZTS
#	define ZLIBG(v) TSRMG(zlib_globals_id, zend_zlib_globals *, v)
#else
#	define ZLIBG(v) (zlib_globals.v)
#endif

#endif /* PHP_ZLIB_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
