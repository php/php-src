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
   | Authors: Rui Hirokawa <louis@cityfujisawa.ne.jp>                     |
   |          Stig Bakken <ssb@fast.no>                                   |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_ICONV_H
#define PHP_ICONV_H

#ifdef PHP_WIN32
#define PHP_ICONV_API __declspec(dllexport)
#else
#define PHP_ICONV_API
#endif

extern zend_module_entry iconv_module_entry;
#define phpext_iconv_ptr &iconv_module_entry

PHP_MINIT_FUNCTION(miconv);
PHP_MSHUTDOWN_FUNCTION(miconv);
PHP_MINFO_FUNCTION(miconv);

PHP_FUNCTION(iconv);
PHP_FUNCTION(ob_iconv_handler);
PHP_FUNCTION(iconv_get_encoding);
PHP_FUNCTION(iconv_set_encoding);

ZEND_BEGIN_MODULE_GLOBALS(iconv)
	char *input_encoding;
	char *internal_encoding;
	char *output_encoding;
ZEND_END_MODULE_GLOBALS(iconv)

#ifdef ZTS
#define ICONVLS_D zend_iconv_globals *iconv_globals
#define ICONVLS_C iconv_globals
#define ICONVG(v) (iconv_globals->v)
#define ICONVLS_FETCH() zend_iconv_globals *iconv_globals = ts_resource(iconv_globals_id)
#else
#define ICONVLS_D
#define ICONVLS_C
#define ICONVG(v) (iconv_globals.v)
#define ICONVLS_FETCH()
#endif

#define ICONV_INPUT_ENCODING "ISO-8859-1" 
#define ICONV_OUTPUT_ENCODING "ISO-8859-1"
#define ICONV_INTERNAL_ENCODING "ISO-8859-1" 

#endif	/* PHP_ICONV_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
