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
   | Authors: Uwe Steinmann                                               |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifndef PHP_FDF_H
#define PHP_FDF_H

#if HAVE_FDFLIB
#ifdef PHP_WIN32
#else
#define UNIX_DEV
#endif

#include <FdfTk.h>

extern zend_module_entry fdf_module_entry;
#define fdf_module_ptr &fdf_module_entry

extern PHP_MINIT_FUNCTION(fdf);
extern PHP_MSHUTDOWN_FUNCTION(fdf);
PHP_MINFO_FUNCTION(fdf);

PHP_FUNCTION(fdf_open);
PHP_FUNCTION(fdf_close);
PHP_FUNCTION(fdf_create);
PHP_FUNCTION(fdf_save);
PHP_FUNCTION(fdf_get_value);
PHP_FUNCTION(fdf_set_value);
PHP_FUNCTION(fdf_next_field_name);
PHP_FUNCTION(fdf_set_ap);
PHP_FUNCTION(fdf_get_status);
PHP_FUNCTION(fdf_set_status);
PHP_FUNCTION(fdf_set_file);
PHP_FUNCTION(fdf_get_file);
PHP_FUNCTION(fdf_add_template);
#else
#define phpext_fdf_ptr NULL
#endif
#define phpext_fdf_ptr fdf_module_ptr
#endif /* PHP_FDF_H */
