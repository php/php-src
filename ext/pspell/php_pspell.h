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
   | Authors: Vlad Krupin <phpdevel@echospace.com>                        |
   +----------------------------------------------------------------------+
 */

/* $Id$ */


#ifndef _PSPELL_H
#define _PSPELL_H
#if HAVE_PSPELL
extern zend_module_entry pspell_module_entry;
#define pspell_module_ptr &pspell_module_entry

PHP_MINIT_FUNCTION(pspell);
PHP_MINFO_FUNCTION(pspell);
PHP_FUNCTION(pspell_new);
PHP_FUNCTION(pspell_mode);
PHP_FUNCTION(pspell_runtogether);
PHP_FUNCTION(pspell_check);
PHP_FUNCTION(pspell_suggest);
#else
#define pspell_module_ptr NULL
#endif

#define phpext_pspell_ptr pspell_module_ptr

#endif /* _PSPELL_H */
