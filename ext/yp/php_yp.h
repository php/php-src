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
   | Authors: Stephanie Wehner <_@r4k.net>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */ 

#ifndef PHP_YP_H
#define PHP_YP_H

#if HAVE_YP

extern zend_module_entry yp_module_entry;
#define yp_module_ptr &yp_module_entry

/* yp.c functions */
PHP_FUNCTION(yp_get_default_domain);
PHP_FUNCTION(yp_order);
PHP_FUNCTION(yp_master);
PHP_FUNCTION(yp_match);
PHP_FUNCTION(yp_first);
PHP_FUNCTION(yp_next);
PHP_MINFO_FUNCTION(yp);

#else

#define yp_module_ptr NULL

#endif /* HAVE_YP */

#define phpext_yp_ptr yp_module_ptr

#endif /* PHP_YP_H */
