/* $Source$ */
/* $Id$ */

/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: 								                                            |
   |  Initial version     by  Alex Barkov <bar@izhcom.ru>                 |
   |                      and Ramil Kalimullin <ram@izhcom.ru>            |
   |  Further development by  Sergey Kartashoff <gluke@mail.ru>           |
   +----------------------------------------------------------------------+
 */

#ifndef _PHP_MNOGO_H
#define _PHP_MNOGO_H

#if HAVE_MNOGOSEARCH

#include <udm_config.h>
#include <udmsearch.h>

#if UDM_VERSION_ID >= 30203
#include <udm_crc32.h>
#include <udm_store.h>
#endif

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

DLEXPORT PHP_FUNCTION(udm_api_version);
#if UDM_VERSION_ID >= 30200
DLEXPORT PHP_FUNCTION(udm_check_charset);
#if UDM_VERSION_ID == 30203
DLEXPORT PHP_FUNCTION(udm_open_stored);
DLEXPORT PHP_FUNCTION(udm_check_stored);
DLEXPORT PHP_FUNCTION(udm_close_stored);
#endif
#if UDM_VERSION_ID >= 30203
DLEXPORT PHP_FUNCTION(udm_crc32);
#endif
#if UDM_VERSION_ID >= 30204
DLEXPORT PHP_FUNCTION(udm_parse_query_string);
DLEXPORT PHP_FUNCTION(udm_make_excerpt);
DLEXPORT PHP_FUNCTION(udm_set_agent_param_ex);
DLEXPORT PHP_FUNCTION(udm_get_res_field_ex);
#endif
#endif

DLEXPORT PHP_FUNCTION(udm_alloc_agent);
DLEXPORT PHP_FUNCTION(udm_set_agent_param);

DLEXPORT PHP_FUNCTION(udm_load_ispell_data);
DLEXPORT PHP_FUNCTION(udm_free_ispell_data);

DLEXPORT PHP_FUNCTION(udm_add_search_limit);
DLEXPORT PHP_FUNCTION(udm_clear_search_limits);

DLEXPORT PHP_FUNCTION(udm_error);
DLEXPORT PHP_FUNCTION(udm_errno);

DLEXPORT PHP_FUNCTION(udm_find);
DLEXPORT PHP_FUNCTION(udm_get_res_field);
DLEXPORT PHP_FUNCTION(udm_get_res_param);

DLEXPORT PHP_FUNCTION(udm_cat_list);
DLEXPORT PHP_FUNCTION(udm_cat_path);

DLEXPORT PHP_FUNCTION(udm_free_res);
DLEXPORT PHP_FUNCTION(udm_free_agent);

#if UDM_VERSION_ID > 30110
DLEXPORT PHP_FUNCTION(udm_get_doc_count);
#endif

#else

#define mnogosearch_module_ptr NULL

#endif

#define phpext_mnogosearch_ptr mnogosearch_module_ptr

#endif /* _PHP_MNOGO_H */
