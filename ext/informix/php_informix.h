/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 4.0                     |
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
   | Authors: Danny Heijl <Danny.Heijl@cevi.be>, initial cut (ODS 7)      |
   |          Christian Cartus <chc@idgruppe.de>, blobs, and IUS 9        |
   |          Jouni Ahto <jah@mork.net>   : configuration stuff           |
   | based on mysql code by: Zeev Suraski <zeev@php.net>                  |
   +----------------------------------------------------------------------+
 */


#ifndef _PHP3_IFX_H
#define _PHP3_IFX_H


#if COMPILE_DL
#undef HAVE_IFX
#define HAVE_IFX 1
#endif

#if HAVE_IFX
#ifndef DLEXPORT
#define DLEXPORT
#endif

#include "locator.h"
#include "sqltypes.h"


extern php3_module_entry ifx_module_entry;
#define ifx_module_ptr &ifx_module_entry

extern int php3_minit_ifx(INIT_FUNC_ARGS);
extern int php3_rinit_ifx(INIT_FUNC_ARGS);
extern int php3_mshutdown_ifx(SHUTDOWN_FUNC_ARGS);
void php3_info_ifx(ZEND_MODULE_INFO_FUNC_ARGS);
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

typedef struct {
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
	int le_result,le_link,le_plink,le_idresult;
	char *default_host, *default_user, *default_password;
	int connectionid;
	int cursorid;
        int sv_sqlcode;      /* saved informix SQLCODE value */
        long blobinfile;     /* 0=save in memory, 1=save in file */
        long textasvarchar;  /* 0=as id, 1=as varchar */
        long byteasvarchar;  /* 0=as id, 1=as varchar */
        long charasvarchar;  /* 0=don't strip trailing blanks, 1=strip */
        long nullformat;     /* 0=NULL as "", 1= NULL as "NULL" */
        char *nullvalue;     /* "" */
        char *nullstring;    /* "NULL" */
} ifx_module;

#define MAX_RESID          64
#define BLOBINFILE 0      /* 0=in memory, 1=in file */

typedef struct ifx_res {
	char connecid[16];
	char cursorid[16];
	char descrpid[16];
	char statemid[16];
        int  isscroll;
        int  ishold;
        int  iscursory;
        int  paramquery;
	int  numcols;
	int  rowid;
        int  affected_rows;
        long sqlerrd[6];
        int res_id[MAX_RESID];
} IFX_RES;





typedef struct _IFX_IDRES {
 	  int type;
          union {
           struct {
  	    int mode;
	    loc_t blob_data;
	   } BLOBRES;
	   struct {
	     char *char_data;
	     int len;
 	   } CHARRES;
#if HAVE_IFX_IUS
	   struct {
            ifx_lo_t slob_data;
            ifx_lo_create_spec_t *createspec;
            int lofd;
 	   } SLOBRES;
#endif
	  } DATARES;
} IFX_IDRES;



#define BLOB DATARES.BLOBRES
#define CHAR DATARES.CHARRES

#if HAVE_IFX_IUS
#define SLOB DATARES.SLOBRES
#endif






#ifndef THREAD_SAFE
extern ifx_module php3_ifx_module;
#endif

#else

#define ifx_module_ptr NULL

#endif

#endif /* _PHP3_IFX_H */
