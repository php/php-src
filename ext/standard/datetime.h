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

#ifndef _DATETIME_H
#define _DATETIME_H

extern void php3_time(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_mktime(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_gmmktime(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_date(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_gmdate(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_getdate(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_checkdate(INTERNAL_FUNCTION_PARAMETERS);
#if HAVE_STRFTIME
extern void php3_strftime(INTERNAL_FUNCTION_PARAMETERS);
#endif

extern char *php3_std_date(time_t t);
void _php3_mktime(INTERNAL_FUNCTION_PARAMETERS, int gm);

#endif /* _DATETIME_H */
