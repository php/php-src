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
   | Author: Jim Winstead <jimw@php.net>                                  |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PAGEINFO_H
#define PAGEINFO_H

PHP_FUNCTION(getmyuid);
PHP_FUNCTION(getmygid);
PHP_FUNCTION(getmypid);
PHP_FUNCTION(getmyinode);
PHP_FUNCTION(getlastmod);

PHPAPI void php_statpage(TSRMLS_D);
PHPAPI long php_getlastmod(TSRMLS_D);
extern long php_getuid(void);
extern long php_getgid(void);

#endif
