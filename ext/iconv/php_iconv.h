/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2007 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rui Hirokawa <rui_hirokawa@ybb.ne.jp>                       |
   |          Stig Bakken <ssb@fast.no>                                   |
   +----------------------------------------------------------------------+
 */

/* $Revision$ */

#ifndef PHP_ICONV_H
#define PHP_ICONV_H

#ifdef PHP_WIN32
#ifdef PHP_ICONV_EXPORTS
#define PHP_ICONV_API __declspec(dllexport)
#else
#define PHP_ICONV_API __declspec(dllimport)
#endif
#else
#define PHP_ICONV_API
#endif

#ifdef PHP_ATOM_INC
#include "ext/iconv/php_have_iconv.h"
#include "ext/iconv/php_have_libiconv.h"
#include "ext/iconv/php_have_glibc_iconv.h"
#include "ext/iconv/php_have_bsd_iconv.h"
#include "ext/iconv/php_iconv_supports_errno.h"
#include "ext/iconv/php_php_iconv_impl.h"
#include "ext/iconv/php_php_iconv_h_path.h"
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

/* {{{ typedef enum php_iconv_err_t */
typedef enum _php_iconv_err_t {
	PHP_ICONV_ERR_SUCCESS           = SUCCESS,
	PHP_ICONV_ERR_CONVERTER         = 1,
	PHP_ICONV_ERR_WRONG_CHARSET     = 2,
	PHP_ICONV_ERR_TOO_BIG           = 3,
	PHP_ICONV_ERR_ILLEGAL_SEQ       = 4,
	PHP_ICONV_ERR_ILLEGAL_CHAR      = 5,
	PHP_ICONV_ERR_UNKNOWN           = 6
} php_iconv_err_t;
/* }}} */

PHP_ICONV_API php_iconv_err_t php_iconv_string(const char * in_p, size_t in_len, char **out, size_t *out_len, const char *in_charset, const char *out_charset);

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
