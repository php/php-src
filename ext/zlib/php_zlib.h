/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Stefan Röhrich <sr@linux.de>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_ZLIB_H
#define PHP_ZLIB_H

#include <zlib.h>

ZEND_BEGIN_MODULE_GLOBALS(zlib)
	int gzgetss_state;

	/* variables for transparent gzip encoding */
    int compression_coding;
    z_stream stream;
    uLong crc;
	int ob_gzhandler_status;
	int ob_gzip_coding;
	int output_compression;
ZEND_END_MODULE_GLOBALS(zlib)

extern zend_module_entry php_zlib_module_entry;
#define zlib_module_ptr &php_zlib_module_entry

PHP_MINIT_FUNCTION(zlib);
PHP_MSHUTDOWN_FUNCTION(zlib);
PHP_RINIT_FUNCTION(zlib);
PHP_MINFO_FUNCTION(zlib);
PHP_FUNCTION(gzopen);
PHP_FUNCTION(gzclose);
PHP_FUNCTION(gzeof);
PHP_FUNCTION(gzread);
PHP_FUNCTION(gzgetc);
PHP_FUNCTION(gzgets);
PHP_FUNCTION(gzgetss);
PHP_FUNCTION(gzwrite);
PHP_FUNCTION(gzrewind);
PHP_FUNCTION(gztell);
PHP_FUNCTION(gzseek);
PHP_FUNCTION(gzpassthru);
PHP_FUNCTION(readgzfile);
PHP_FUNCTION(gzfile);
PHP_FUNCTION(gzcompress);
PHP_FUNCTION(gzuncompress);
PHP_FUNCTION(gzdeflate);
PHP_FUNCTION(gzinflate);
PHP_FUNCTION(gzencode);
PHP_FUNCTION(ob_gzhandler);

FILE *zlib_fopen_wrapper(char *path, char *mode, int options, int *issock, int *socketd, char **opened_path TSRMLS_DC);
int php_enable_output_compression(int buffer_size);


#ifdef ZTS
#define ZLIBG(v) TSRMG(zlib_globals_id, zend_zlib_globals *, v)
#else
#define ZLIBG(v) (zlib_globals.v)
#endif

#define phpext_zlib_ptr zlib_module_ptr

#endif /* PHP_ZLIB_H */
