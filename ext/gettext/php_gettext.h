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
   | Author: Alex Plotnick <alex@wgate.com>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_GETTEXT_H
#define PHP_GETTEXT_H

#if HAVE_LIBINTL
#ifndef INIT_FUNC_ARGS
#include "zend_modules.h"
#endif

extern zend_module_entry php_gettext_module_entry;
#define gettext_module_ptr &php_gettext_module_entry

PHP_MINFO_FUNCTION(gettext);

PHP_FUNCTION(textdomain);
PHP_FUNCTION(gettext);
PHP_FUNCTION(dgettext);
PHP_FUNCTION(dcgettext);
PHP_FUNCTION(bindtextdomain);
#if HAVE_NGETTEXT
PHP_FUNCTION(ngettext);
#endif
#if HAVE_DNGETTEXT
PHP_FUNCTION(dngettext);
#endif
#if HAVE_DCNGETTEXT
PHP_FUNCTION(dcngettext);
#endif
#if HAVE_BIND_TEXTDOMAIN_CODESET
PHP_FUNCTION(bind_textdomain_codeset);
#endif

#else
#define gettext_module_ptr NULL
#endif /* HAVE_LIBINTL */

#define phpext_gettext_ptr gettext_module_ptr

#endif /* PHP_GETTEXT_H */
