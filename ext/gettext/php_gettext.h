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
   | Author: Alex Plotnick <alex@wgate.com>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_GETTEXT_H
#define PHP_GETTEXT_H

#if HAVE_LIBINTL
#ifndef INIT_FUNC_ARGS
#include "modules.h"
#endif

extern zend_module_entry php_gettext_module_entry;
#define gettext_module_ptr &php_gettext_module_entry

PHP_MINFO_FUNCTION(gettext);

PHP_FUNCTION(textdomain);
PHP_FUNCTION(gettext);
PHP_FUNCTION(dgettext);
PHP_FUNCTION(dcgettext);
PHP_FUNCTION(bindtextdomain);

#else
#define gettext_module_ptr NULL
#endif /* HAVE_LIBINTL */

#define phpext_gettext_ptr gettext_module_ptr

#endif /* PHP_GETTEXT_H */
