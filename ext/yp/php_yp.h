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
   | Authors: Stephanie Wehner <_@r4k.net>                                |
   +----------------------------------------------------------------------+
 */
/* $Id$ */ 

#ifndef _PHP_YP_H
#define _PHP_YP_H

#if COMPILE_DL
#undef HAVE_YP
#define HAVE_YP 1
#endif

#if HAVE_YP

extern php3_module_entry yp_module_entry;
#define yp_module_ptr &yp_module_entry

/* yp.c functions */
int php3_minit_yp(INIT_FUNC_ARGS);
PHP_FUNCTION(yp_get_default_domain);
PHP_FUNCTION(yp_order);
PHP_FUNCTION(yp_master);
PHP_FUNCTION(yp_match);
PHP_FUNCTION(yp_first);
PHP_FUNCTION(yp_next);
void php3_info_yp(ZEND_MODULE_INFO_FUNC_ARGS);

#else

#define yp_module_ptr NULL

#endif /* HAVE_YP */

#define phpext_yp_ptr yp_module_ptr

#endif /* _PHP_YP_H */
