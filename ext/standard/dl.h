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
   | Authors: Brian Schaffner <brian@tool.net>                            |
   |          Shane Caraveo <shane@caraveo.com>                           |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef _DL_H
#define _DL_H

void php3_dl(pval *file,int type,pval *return_value);


#if HAVE_LIBDL

extern php3_module_entry dl_module_entry;
#define dl_module_ptr &dl_module_entry

/* dynamic loading functions */
void dl(INTERNAL_FUNCTION_PARAMETERS);
extern int php3_minit_dl(INIT_FUNC_ARGS);
extern int php3_mshutdown_dl(SHUTDOWN_FUNC_ARGS);
extern int php3_rshutdown_dl(SHUTDOWN_FUNC_ARGS);
void php3_info_dl(ZEND_MODULE_INFO_FUNC_ARGS);

#else

#define dl_module_ptr NULL

#endif

#define phpext_dl_ptr dl_module_ptr

#endif /* _DL_H */
