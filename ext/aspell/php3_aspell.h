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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef _ASPELL_H
#define _ASPELL_H
#if HAVE_ASPELL
extern php3_module_entry aspell_module_entry;
#define php3_aspell_module_ptr &aspell_module_entry

extern int php3_minit_aspell(INIT_FUNC_ARGS);
extern void php3_info_aspell(void);
extern void php3_aspell_close();
void php3_aspell_new(INTERNAL_FUNCTION_PARAMETERS);
void php3_aspell_check(INTERNAL_FUNCTION_PARAMETERS);
void php3_aspell_check_raw(INTERNAL_FUNCTION_PARAMETERS);
void php3_aspell_suggest(INTERNAL_FUNCTION_PARAMETERS);

#else
#define php3_aspell_module_ptr NULL
#endif
#endif /* _ASPELL_H */
