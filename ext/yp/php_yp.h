/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stephanie Wehner <_@r4k.net>                                |
   |          Fredrik Ohrn                                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */ 

#ifndef PHP_YP_H
#define PHP_YP_H

#if HAVE_YP

#ifdef PHP_WIN32
#define PHP_YP_API __declspec(dllexport)
#else
#define PHP_YP_API
#endif

extern zend_module_entry yp_module_entry;
#define yp_module_ptr &yp_module_entry

/* yp.c functions */
PHP_FUNCTION(yp_get_default_domain);
PHP_FUNCTION(yp_order);
PHP_FUNCTION(yp_master);
PHP_FUNCTION(yp_match);
PHP_FUNCTION(yp_first);
PHP_FUNCTION(yp_next);
PHP_FUNCTION(yp_all);
PHP_FUNCTION(yp_cat);
PHP_FUNCTION(yp_errno);
PHP_FUNCTION(yp_err_string);
PHP_MINIT_FUNCTION(yp);
PHP_RINIT_FUNCTION(yp);
PHP_MINFO_FUNCTION(yp);

typedef struct {
	int error;
} php_yp_globals;

#ifdef ZTS
#define YP(v) TSRMG(yp_globals_id, php_yp_globals *, v)
#else
#define YP(v) (yp_globals.v)
#endif

#else

#define yp_module_ptr NULL

#endif /* HAVE_YP */

#define phpext_yp_ptr yp_module_ptr

#endif /* PHP_YP_H */
