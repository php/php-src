/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
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
   | Authors: Andrey Zmievski <andrey@ispi.net>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef _PHP_PCRE_H
#define _PHP_PCRE_H

#if HAVE_LIBPCRE

#include "pcre.h"

extern void php_info_pcre(ZEND_MODULE_INFO_FUNC_ARGS);
extern int php_rinit_pcre(INIT_FUNC_ARGS);

PHP_FUNCTION(pcre_match);
PHP_FUNCTION(pcre_replace);

extern zend_module_entry pcre_module_entry;
#define pcre_module_ptr &pcre_module_entry

#else

#define pcre_module_ptr NULL

#endif /* HAVE_LIBPCRE */

#define phpext_pcre_ptr pcre_module_ptr

#endif /* _PHP_PCRE_H */
