/* $Source$ */
/* $Id$ */

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
   | Authors: 								  |
   |  Initial version     by  Alex Barkov <bar@izhcom.ru>                 |
   |                      and Ramil Kalimullin <ram@izhcom.ru>            |
   |  Further development by  Sergey Kartashoff <gluke@biosys.net>        |
   +----------------------------------------------------------------------+
 */

#ifndef _PHP_MNOGO_H
#define _PHP_MNOGO_H

#if HAVE_MNOGOSEARCH

extern zend_module_entry mnogosearch_module_entry;
#define mnogosearch_module_ptr &mnogosearch_module_entry

#ifdef PHP_WIN32                            
#define PHP_MNOGO_API __declspec(dllexport)  
#else                                       
#define PHP_MNOGO_API                        
#endif                                      

#ifdef ZTS       
#include "TSRM.h"
#endif           

/* mnoGoSearch functions */
DLEXPORT PHP_MINIT_FUNCTION(mnogosearch);
DLEXPORT PHP_RINIT_FUNCTION(mnogosearch);
DLEXPORT PHP_MSHUTDOWN_FUNCTION(mnogosearch);
DLEXPORT PHP_MINFO_FUNCTION(mnogosearch);

DLEXPORT PHP_FUNCTION(udm_alloc_agent);
DLEXPORT PHP_FUNCTION(udm_set_agent_param);
DLEXPORT PHP_FUNCTION(udm_free_agent);

DLEXPORT PHP_FUNCTION(udm_error);
DLEXPORT PHP_FUNCTION(udm_errno);

DLEXPORT PHP_FUNCTION(udm_find);
DLEXPORT PHP_FUNCTION(udm_free_res);
DLEXPORT PHP_FUNCTION(udm_get_res_field);
DLEXPORT PHP_FUNCTION(udm_get_res_param);

#else

#define mnogosearch_module_ptr NULL

#endif

#define phpext_mnogosearch_ptr mnogosearch_module_ptr

#endif /* _PHP_MNOGO_H */
