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
   | Authors: Rui Hirokawa <louis@cityfujisawa.ne.jp>                     |
   |          Stig Bakken <ssb@fast.no>                                   |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_ICONV_H
#define PHP_ICONV_H

#include <iconv.h>
#include "SAPI.h"

extern zend_module_entry iconv_module_entry;
#define phpext_iconv_ptr &iconv_module_entry

#ifdef PHP_WIN32
#define PHP_ICONV_API __declspec(dllexport)
#else
#define PHP_ICONV_API
#endif

PHP_MINIT_FUNCTION(iconv);
PHP_MSHUTDOWN_FUNCTION(iconv);
PHP_RINIT_FUNCTION(iconv);
PHP_RSHUTDOWN_FUNCTION(iconv);
PHP_MINFO_FUNCTION(iconv);

PHP_FUNCTION(iconv);
PHP_FUNCTION(ob_iconv_handler);
PHP_FUNCTION(iconv_set_encoding);

ZEND_BEGIN_MODULE_GLOBALS(iconv)
	char *iconv_internal_encoding;
	char *iconv_output_encoding;
	int global_variable;
ZEND_END_MODULE_GLOBALS(iconv)

/* In every function that needs to use variables in php_iconv_globals,
   do call ICONVLS_FETCH(); after declaring other variables used by
   that function, and always refer to them as ICONVG(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define ICONVG(v) (iconv_globals->v)
#define ICONVLS_FETCH() php_iconv_globals *iconv_globals = ts_resource(iconv_globals_id)
#else
#define ICONVG(v) (iconv_globals.v)
#define ICONVLS_FETCH()
#endif

#define ICONV_OUTPUT_ENCODING "ISO-8859-1"
#define ICONV_INTERNAL_ENCODING "ISO-8859-1" 

#endif	/* PHP_ICONV_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
