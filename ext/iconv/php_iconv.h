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
   | Authors: Rui Hirokawa <rui_hirokawa@ybb.ne.jp>                       |
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

#ifdef PHP_ATOM_INC
#include "php_have_iconv.h"
#include "php_have_libiconv.h"
#endif


#ifdef HAVE_ICONV
extern zend_module_entry iconv_module_entry;
#define iconv_module_ptr &iconv_module_entry

PHP_MINIT_FUNCTION(miconv);
PHP_MSHUTDOWN_FUNCTION(miconv);
PHP_MINFO_FUNCTION(miconv);

PHP_NAMED_FUNCTION(php_if_iconv);
PHP_FUNCTION(ob_iconv_handler);
PHP_FUNCTION(iconv_get_encoding);
PHP_FUNCTION(iconv_set_encoding);

ZEND_BEGIN_MODULE_GLOBALS(iconv)
	char *input_encoding;
	char *internal_encoding;
	char *output_encoding;
ZEND_END_MODULE_GLOBALS(iconv)

#ifdef ZTS
#define ICONVG(v) TSRMG(iconv_globals_id, zend_iconv_globals *, v)
#else
#define ICONVG(v) (iconv_globals.v)
#endif

#define ICONV_INPUT_ENCODING "ISO-8859-1" 
#define ICONV_OUTPUT_ENCODING "ISO-8859-1"
#define ICONV_INTERNAL_ENCODING "ISO-8859-1" 

#else

#define iconv_module_ptr NULL

#endif /* HAVE_ICONV */

#define phpext_iconv_ptr iconv_module_ptr

#endif	/* PHP_ICONV_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
