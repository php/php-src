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
   | Authors: Jim Winstead (jimw@php.net)                                 |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef _DBASE_H
#define _DBASE_H
#if DBASE
extern php3_module_entry dbase_module_entry;
#define dbase_module_ptr &dbase_module_entry

extern int php3_minit_dbase(INIT_FUNC_ARGS);
extern void php3_dbase_open(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dbase_create(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dbase_close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dbase_numrecords(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dbase_numfields(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dbase_add_record(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dbase_get_record(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dbase_delete_record(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dbase_pack(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dbase_get_record_with_names(INTERNAL_FUNCTION_PARAMETERS);
#else
#define dbase_module_ptr NULL
#endif
#endif /* _DBASE_H */
