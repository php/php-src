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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef _REG_H
#define _REG_H

extern php3_module_entry regexp_module_entry;
#define regexp_module_ptr &regexp_module_entry

extern char *_php3_regreplace(const char *pattern, const char *replace, const char *string, int icase, int extended);

extern void php3_ereg(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_eregi(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_eregireplace(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_eregreplace(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_split(INTERNAL_FUNCTION_PARAMETERS);
extern PHPAPI void php3_sql_regcase(INTERNAL_FUNCTION_PARAMETERS);

#endif /* _REG_H */
