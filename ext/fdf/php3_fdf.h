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

#ifndef _PHP3_FDF_H
#define _PHP3_FDF_H

#if HAVE_FDFLIB
#if WIN32|WINNT
#else
#define UNIX_DEV
#endif

#include <FdfTk.h>

extern php3_module_entry fdf_module_entry;
#define fdf_module_ptr &fdf_module_entry

extern int php3_minit_fdf(INIT_FUNC_ARGS);
extern int php3_mend_fdf(void);
extern void php3_info_fdf(void);
extern void php3_fdf_open(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fdf_close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fdf_create(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fdf_save(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fdf_get_value(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fdf_set_value(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fdf_next_field_name(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fdf_set_ap(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fdf_get_status(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fdf_set_status(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fdf_set_file(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fdf_get_file(INTERNAL_FUNCTION_PARAMETERS);
#else
#define fdf_module_ptr NULL
#endif
#endif /* _PHP3_FDF_H */
