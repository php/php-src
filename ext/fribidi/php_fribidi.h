/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Onn Ben-Zvi <onn@zend.com>, onnb@mercury.co.il               |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_FRIBIDI_H
#define PHP_FRIBIDI_H

#if HAVE_FRIBIDI

#include <fribidi/fribidi.h>

extern zend_module_entry fribidi_module_entry;
#define phpext_fribidi_ptr &fribidi_module_entry

#ifdef PHP_WIN32
#define PHP_FRIBIDI_API __declspec(dllexport)
#else
#define PHP_FRIBIDI_API
#endif

PHP_MINIT_FUNCTION(fribidi);
PHP_MSHUTDOWN_FUNCTION(fribidi);
PHP_RINIT_FUNCTION(fribidi);
PHP_RSHUTDOWN_FUNCTION(fribidi);
PHP_MINFO_FUNCTION(fribidi);

PHP_FUNCTION(fribidi_log2vis);
PHP_FUNCTION(fribidi_charset_info);
PHP_FUNCTION(fribidi_get_charsets);

#ifdef ZTS
#define FRIBIDIG(v) TSRMG(fribidi_globals_id, php_fribidi_globals *, v)
#else
#define FRIBIDIG(v) (fribidi_globals.v)
#endif

#else

#define phpext_fribidi_ptr NULL

#endif

#endif	/* PHP_FRIBIDI_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
