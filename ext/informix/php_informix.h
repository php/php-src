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
   | Authors: Danny Heijl <Danny.Heijl@cevi.be>, initial cut (ODS 7)      |
   |          Christian Cartus <chc@idgruppe.de>, blobs, and IUS 9        |
   |          Jouni Ahto <jouni.ahto@exdec.fi>, configuration stuff       |
   | based on mysql code by: Zeev Suraski <zeev@php.net>                  |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_INFORMIX_H
#define PHP_INFORMIX_H

#ifdef PHP_WIN32
#define PHP_IFX_API __declspec(dllexport)
#else
#define PHP_IFX_API
#endif

#if HAVE_IFX                        /* with Informix */

#ifndef DLEXPORT
#define DLEXPORT
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

extern zend_module_entry ifx_module_entry;
#define ifx_module_ptr &ifx_module_entry

/* user functions */
PHP_MINIT_FUNCTION(ifx);
PHP_RINIT_FUNCTION(ifx);
PHP_MSHUTDOWN_FUNCTION(ifx);
PHP_MINFO_FUNCTION(ifx);
/* functions common to all Informix versions */
PHP_FUNCTION(ifx_connect);
PHP_FUNCTION(ifx_pconnect);
PHP_FUNCTION(ifx_close);
PHP_FUNCTION(ifx_query);
PHP_FUNCTION(ifx_prepare);
PHP_FUNCTION(ifx_do);
PHP_FUNCTION(ifx_error);
PHP_FUNCTION(ifx_errormsg);
PHP_FUNCTION(ifx_affected_rows);
PHP_FUNCTION(ifx_num_rows);
PHP_FUNCTION(ifx_num_fields);
PHP_FUNCTION(ifx_fetch_row);
PHP_FUNCTION(ifx_free_result);
PHP_FUNCTION(ifx_htmltbl_result);
PHP_FUNCTION(ifx_fieldtypes);
PHP_FUNCTION(ifx_fieldproperties);
PHP_FUNCTION(ifx_getsqlca);
/* BLOB related stuff, IDS & IUS only */
PHP_FUNCTION(ifx_create_blob);
PHP_FUNCTION(ifx_free_blob) ;
PHP_FUNCTION(ifx_get_blob);
PHP_FUNCTION(ifx_update_blob);
PHP_FUNCTION(ifx_blobinfile_mode);
PHP_FUNCTION(ifx_copy_blob);
PHP_FUNCTION(ifx_textasvarchar);
PHP_FUNCTION(ifx_byteasvarchar);
PHP_FUNCTION(ifx_nullformat);
PHP_FUNCTION(ifx_create_char);
PHP_FUNCTION(ifx_free_char) ;
PHP_FUNCTION(ifx_update_char);
PHP_FUNCTION(ifx_get_char);

/* SLOB, CLOB : IUS only functions */
#if HAVE_IFX_IUS
PHP_FUNCTION(ifxus_create_slob);
PHP_FUNCTION(ifxus_free_slob) ;
PHP_FUNCTION(ifxus_close_slob) ;
PHP_FUNCTION(ifxus_open_slob) ;
PHP_FUNCTION(ifxus_read_slob);
PHP_FUNCTION(ifxus_write_slob);
PHP_FUNCTION(ifxus_seek_slob);
PHP_FUNCTION(ifxus_tell_slob);
#endif

ZEND_BEGIN_MODULE_GLOBALS(ifx)
	long default_link;
	long num_links, num_persistent;
	long max_links, max_persistent;
	long allow_persistent;
	char *default_host, *default_user, *default_password;
	int connectionid;
	int cursorid;
	int sv_sqlcode;      /* saved informix SQLCODE value */
	long blobinfile;     /* 0=save in memory, 1=save in file */
	long textasvarchar;  /* 0=as id, 1=as varchar */
	long byteasvarchar;  /* 0=as id, 1=as varchar */
	long charasvarchar;  /* 0=don't strip trailing blanks, 1=strip */
	long nullformat;     /* 0=NULL as "", 1= NULL as "NULL" */
	char nullvalue[1];   /* "" */
	char nullstring[5];  /* "NULL" */
ZEND_END_MODULE_GLOBALS(ifx)

#ifdef ZTS
# define IFXG(v) TSRMG(ifx_globals_id, zend_ifx_globals *, v)
#else
# define IFXG(v) (ifx_globals.v)
#endif

#else /* not HAVE_IFX  */
#define ifx_module_ptr NULL
#endif
#define phpext_informix_ptr ifx_module_ptr
#endif /* PHP_INFORMIX_H */
