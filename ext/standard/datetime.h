/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef DATETIME_H
#define DATETIME_H

PHP_FUNCTION(time);
PHP_FUNCTION(mktime);
PHP_FUNCTION(gmmktime);
PHP_FUNCTION(date);
PHP_FUNCTION(gmdate);
PHP_FUNCTION(localtime);
PHP_FUNCTION(getdate);
PHP_FUNCTION(checkdate);
#if HAVE_STRFTIME
PHP_FUNCTION(strftime);
PHP_FUNCTION(gmstrftime);
#endif
PHP_FUNCTION(strtotime);

extern char *php_std_date(time_t t);
void php_mktime(INTERNAL_FUNCTION_PARAMETERS, int gm);
#if HAVE_STRFTIME
void _php_strftime(INTERNAL_FUNCTION_PARAMETERS, int gm);
#endif

#endif /* DATETIME_H */
