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
   | Author: Rasmus Lerdorf <rasmus@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_HYPERWAVE_H
#define PHP_HYPERWAVE_H

#ifdef PHP_WIN32
#define PHP_HW_API __declspec(dllexport)
#else
#define PHP_HW_API
#endif

#if HYPERWAVE
#ifndef DLEXPORT
#define DLEXPORT
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include "hg_comm.h"

extern zend_module_entry hw_module_entry;
#define hw_module_ptr &hw_module_entry

typedef struct {
	long default_link;
	long default_port;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
} zend_hw_globals;

#ifdef ZTS
# define HwSG(v) TSRMG(hw_globals_id, zend_hw_globals *, v)
#else
# define HwSG(v) (hw_globals.v)
extern PHP_HW_API zend_hw_globals hw_globals;
#endif

/*extern hw_module php_hw_module;*/

typedef struct {
        int size;
        char *data;
        char *attributes;
        char *bodytag;
} hw_document;

PHP_MINIT_FUNCTION(hw);
PHP_MSHUTDOWN_FUNCTION(hw);
PHP_MINFO_FUNCTION(hw);

PHP_FUNCTION(hw_connect);
PHP_FUNCTION(hw_pconnect);
PHP_FUNCTION(hw_close);
PHP_FUNCTION(hw_root);
PHP_FUNCTION(hw_info);
PHP_FUNCTION(hw_error);
PHP_FUNCTION(hw_errormsg);
PHP_FUNCTION(hw_mv);
PHP_FUNCTION(hw_cp);
PHP_FUNCTION(hw_deleteobject);
PHP_FUNCTION(hw_changeobject);
PHP_FUNCTION(hw_modifyobject);
PHP_FUNCTION(hw_getparents);
PHP_FUNCTION(hw_getparentsobj);
PHP_FUNCTION(hw_children);
PHP_FUNCTION(hw_childrenobj);
PHP_FUNCTION(hw_getchildcoll);
PHP_FUNCTION(hw_getchildcollobj);
PHP_FUNCTION(hw_getobject);
PHP_FUNCTION(hw_getandlock);
PHP_FUNCTION(hw_unlock);
PHP_FUNCTION(hw_gettext);
PHP_FUNCTION(hw_edittext);
PHP_FUNCTION(hw_getcgi);
PHP_FUNCTION(hw_getremote);
PHP_FUNCTION(hw_getremotechildren);
PHP_FUNCTION(hw_pipedocument);
PHP_FUNCTION(hw_pipecgi);
PHP_FUNCTION(hw_insertdocument);
PHP_FUNCTION(hw_docbyanchorobj);
PHP_FUNCTION(hw_docbyanchor);
PHP_FUNCTION(hw_getobjectbyquery);
PHP_FUNCTION(hw_getobjectbyqueryobj);
PHP_FUNCTION(hw_getobjectbyquerycoll);
PHP_FUNCTION(hw_getobjectbyquerycollobj);
PHP_FUNCTION(hw_getobjectbyftquery);
PHP_FUNCTION(hw_getobjectbyftqueryobj);
PHP_FUNCTION(hw_getobjectbyftquerycoll);
PHP_FUNCTION(hw_getobjectbyftquerycollobj);
PHP_FUNCTION(hw_getchilddoccoll);
PHP_FUNCTION(hw_getchilddoccollobj);
PHP_FUNCTION(hw_getanchors);
PHP_FUNCTION(hw_getanchorsobj);
PHP_FUNCTION(hw_getusername);
PHP_FUNCTION(hw_setlinkroot);
PHP_FUNCTION(hw_inscoll);
PHP_FUNCTION(hw_incollections);
PHP_FUNCTION(hw_insertobject);
PHP_FUNCTION(hw_insdoc);
PHP_FUNCTION(hw_identify);
PHP_FUNCTION(hw_free_document);
PHP_FUNCTION(hw_new_document);
PHP_FUNCTION(hw_new_document_from_file);
PHP_FUNCTION(hw_output_document);
PHP_FUNCTION(hw_document_size);
PHP_FUNCTION(hw_document_attributes);
PHP_FUNCTION(hw_document_bodytag);
PHP_FUNCTION(hw_document_content);
PHP_FUNCTION(hw_document_setcontent);
PHP_FUNCTION(hw_objrec2array);
PHP_FUNCTION(hw_array2objrec);
PHP_FUNCTION(hw_connection_info);
PHP_FUNCTION(hw_getsrcbydestobj);
PHP_FUNCTION(hw_insertanchors);
PHP_FUNCTION(hw_getrellink);
PHP_FUNCTION(hw_dummy);
PHP_FUNCTION(hw_who);
PHP_FUNCTION(hw_stat);
PHP_FUNCTION(hw_mapid);

#else
#define hw_module_ptr NULL
#endif /* HYPERWAVE */
#define phpext_hyperwave_ptr hw_module_ptr
#endif /* PHP_HYPERWAVE_H */

