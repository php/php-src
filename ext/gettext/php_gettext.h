/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Alex Plotnick <alex@wgate.com>                               |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_GETTEXT_H
#define PHP_GETTEXT_H

#if HAVE_LIBINTL

extern zend_module_entry php_gettext_module_entry;
#define gettext_module_ptr &php_gettext_module_entry

#include "php_version.h"
#define PHP_GETTEXT_VERSION PHP_VERSION

PHP_MINFO_FUNCTION(php_gettext);

PHP_NAMED_FUNCTION(zif_textdomain);
PHP_NAMED_FUNCTION(zif_gettext);
PHP_NAMED_FUNCTION(zif_dgettext);
PHP_NAMED_FUNCTION(zif_dcgettext);
PHP_NAMED_FUNCTION(zif_bindtextdomain);
#if HAVE_NGETTEXT
PHP_NAMED_FUNCTION(zif_ngettext);
#endif
#if HAVE_DNGETTEXT
PHP_NAMED_FUNCTION(zif_dngettext);
#endif
#if HAVE_DCNGETTEXT
PHP_NAMED_FUNCTION(zif_dcngettext);
#endif
#if HAVE_BIND_TEXTDOMAIN_CODESET
PHP_NAMED_FUNCTION(zif_bind_textdomain_codeset);
#endif

#else
#define gettext_module_ptr NULL
#endif /* HAVE_LIBINTL */

#define phpext_gettext_ptr gettext_module_ptr

#endif /* PHP_GETTEXT_H */
