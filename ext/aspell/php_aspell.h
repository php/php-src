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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_ASPELL_H
#define PHP_ASPELL_H
#if HAVE_ASPELL
extern zend_module_entry aspell_module_entry;
#define aspell_module_ptr &aspell_module_entry

PHP_MINIT_FUNCTION(aspell);
PHP_MINFO_FUNCTION(aspell);

PHP_FUNCTION(aspell_new);
PHP_FUNCTION(aspell_check);
PHP_FUNCTION(aspell_check_raw);
PHP_FUNCTION(aspell_suggest);

#else
#define aspell_module_ptr NULL
#endif

#define phpext_aspell_ptr aspell_module_ptr

#endif /* PHP_ASPELL_H */
