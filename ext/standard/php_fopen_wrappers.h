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
   |          Jim Winstead <jimw@php.net>                                 |
   |          Hartmut Holzgraefe <hholzgra@php.net>                       |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_FOPEN_WRAPPERS_H
#define PHP_FOPEN_WRAPPERS_H

FILE *php_fopen_url_wrap_http(const char *, char *, int, int *, int *, char ** TSRMLS_DC);
FILE *php_fopen_url_wrap_ftp(const char *, char *, int, int *, int *, char ** TSRMLS_DC);
FILE *php_fopen_url_wrap_php(const char *, char *, int, int *, int *, char ** TSRMLS_DC);

#endif
