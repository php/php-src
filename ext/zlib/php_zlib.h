/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
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


typedef struct {
	int gzgetss_state;

	/* variables for transparent gzip encoding */
    int compression_coding;
    z_stream stream;
    uLong crc;
} php_zlib_globals;

extern zend_module_entry php_zlib_module_entry;
#define zlib_module_ptr &php_zlib_module_entry

PHP_MINIT_FUNCTION(zlib);
PHP_MSHUTDOWN_FUNCTION(zlib);
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

#ifdef ZTS
#define ZLIBLS_D php_zlib_globals *zlib_globals
#define ZLIBLS_DC , ZLIBLS_D
#define ZLIBLS_C zlib_globals
#define ZLIBLS_CC , ZLIBLS_C
#define ZLIBG(v) (zlib_globals->v)
#define ZLIBLS_FETCH() php_zlib_globals *zlib_globals = ts_resource(zlib_globals_id)
#else
#define ZLIBLS_D
#define ZLIBLS_DC
#define ZLIBLS_C
#define ZLIBLS_CC
#define ZLIBG(v) (zlib_globals.v)
#define ZLIBLS_FETCH()
#endif

#define phpext_zlib_ptr zlib_module_ptr

#endif /* PHP_ZLIB_H */
