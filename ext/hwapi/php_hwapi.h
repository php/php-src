/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_HWAPI_H
#define PHP_HWAPI_H

#ifdef PHP_WIN32
#define PHP_HWAPI_API __declspec(dllexport)
#else
#define PHP_HWAPI_API
#endif

#if HAVE_HWAPI
#ifndef DLEXPORT
#define DLEXPORT
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

extern zend_module_entry hwapi_module_entry;
#define hwapi_module_ptr &hwapi_module_entry

typedef struct {
	long default_link;
	long default_port;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
	int le_socketp, le_psocketp, le_document;
} zend_hwapi_globals;

#ifdef ZTS
# define HwApiSLS_D        zend_hwapi_globals *hwapi_globals
# define HwApiSLS_DC       , HwApiSLS_D
# define HwApiSLS_C        hwapi_globals
# define HwApiSLS_CC , HwApiSLS_C
# define HwApiSG(v) (hwapi_globals->v)
# define HwApiSLS_FETCH()  zend_hwapi_globals *hwapi_globals = ts_resource(hwapi_globals_id)
#else
# define HwApiSLS_D
# define HwApiSLS_DC
# define HwApiSLS_C
# define HwApiSLS_CC
# define HwApiSG(v) (hwapi_globals.v)
# define HwApiSLS_FETCH()
extern PHP_HWAPI_API zend_hwapi_globals hwapi_globals;
#endif

extern PHP_MINIT_FUNCTION(hwapi);
extern PHP_MSHUTDOWN_FUNCTION(hwapi);
extern PHP_RINIT_FUNCTION(hwapi);
PHP_MINFO_FUNCTION(hwapi);

/* HW_API */
PHP_FUNCTION(hwapi_dummy);
PHP_FUNCTION(hwapi_init);
PHP_FUNCTION(hwapi_hgcsp);
PHP_FUNCTION(hwapi_object);
PHP_FUNCTION(hwapi_children);
PHP_FUNCTION(hwapi_mychildren);
PHP_FUNCTION(hwapi_parents);
PHP_FUNCTION(hwapi_find);
PHP_FUNCTION(hwapi_identify);
PHP_FUNCTION(hwapi_remove);
PHP_FUNCTION(hwapi_content);
PHP_FUNCTION(hwapi_copy);
PHP_FUNCTION(hwapi_link);
PHP_FUNCTION(hwapi_move);
PHP_FUNCTION(hwapi_lock);
PHP_FUNCTION(hwapi_unlock);
PHP_FUNCTION(hwapi_replace);
PHP_FUNCTION(hwapi_insert);
PHP_FUNCTION(hwapi_insertdocument);
PHP_FUNCTION(hwapi_insertcollection);

PHP_FUNCTION(hwapi_srcanchors);
PHP_FUNCTION(hwapi_dstanchors);
PHP_FUNCTION(hwapi_objectbyanchor);
PHP_FUNCTION(hwapi_dstofsrcanchor);
PHP_FUNCTION(hwapi_srcsofdst);

PHP_FUNCTION(hwapi_checkin);
PHP_FUNCTION(hwapi_checkout);
PHP_FUNCTION(hwapi_setcommittedversion);
PHP_FUNCTION(hwapi_revert);
PHP_FUNCTION(hwapi_history);
PHP_FUNCTION(hwapi_removeversion);
PHP_FUNCTION(hwapi_freeversion);

PHP_FUNCTION(hwapi_configurationhistory);
PHP_FUNCTION(hwapi_saveconfiguration);
PHP_FUNCTION(hwapi_restoreconfiguration);
PHP_FUNCTION(hwapi_mergeconfiguration);
PHP_FUNCTION(hwapi_removeconfiguration);

PHP_FUNCTION(hwapi_user);
PHP_FUNCTION(hwapi_userlist);
PHP_FUNCTION(hwapi_hwstat);
PHP_FUNCTION(hwapi_dcstat);
PHP_FUNCTION(hwapi_dbstat);
PHP_FUNCTION(hwapi_ftstat);
PHP_FUNCTION(hwapi_info);

/* HW_Object */
PHP_FUNCTION(hwapi_object_new);
PHP_FUNCTION(hwapi_object_count);
PHP_FUNCTION(hwapi_object_title);
PHP_FUNCTION(hwapi_object_attreditable);
PHP_FUNCTION(hwapi_object_assign);
PHP_FUNCTION(hwapi_object_attribute);
PHP_FUNCTION(hwapi_object_insert);
PHP_FUNCTION(hwapi_object_remove);
PHP_FUNCTION(hwapi_object_value);

/* HW_Attribute */
PHP_FUNCTION(hwapi_attribute_new);
PHP_FUNCTION(hwapi_attribute_key);
PHP_FUNCTION(hwapi_attribute_value);
PHP_FUNCTION(hwapi_attribute_values);
PHP_FUNCTION(hwapi_attribute_langdepvalue);

/* HW_Content */
PHP_FUNCTION(hwapi_content_new);
PHP_FUNCTION(hwapi_content_read);
PHP_FUNCTION(hwapi_content_mimetype);

/* HW_Error */
PHP_FUNCTION(hwapi_error_count);
PHP_FUNCTION(hwapi_error_reason);

/* HW_Reason */
PHP_FUNCTION(hwapi_reason_type);
PHP_FUNCTION(hwapi_reason_description);

#else
#define hwapi_module_ptr NULL
#endif /* HAVE_HWAPI */
#define phpext_hwapi_ptr hwapi_module_ptr
#endif /* PHP_HWAPI_H */

