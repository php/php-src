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


#include "locator.h"
#include "sqltypes.h"

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

typedef struct {
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
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
        char *nullvalue;     /* "" */
        char *nullstring;    /* "NULL" */
} php_ifx_globals;           /* formerly "ifx_module" in the php3 version */

typedef struct {
	int le_result;
        int le_link;
        int le_plink;
        int le_idresult;
} php_ifx_listids;

#define IFXL(v) (ifx_listids.v)

#ifndef ZTS
extern php_ifx_globals ifx_globals;
#endif

#ifdef ZTS
# define IFXLS_D	php_ifx_globals *ifx_globals
# define IFXLS_DC	, IFXLS_D
# define IFXLS_C	ifx_globals
# define IFXLS_CC , IFXLS_C
# define IFXG(v) (ifx_globals->v)
# define IFXLS_FETCH()	php_ifx_globals *ifx_globals = ts_resource(ifx_globals_id)
# define IFX_TLS_VARS char *globals; IFXLS_FETCH(); globals = (char *)ifx_globals;
#else
# define IFXLS_D
# define IFXLS_DC
# define IFXLS_C
# define IFXLS_CC
# define IFXG(v) (ifx_globals.v)
# define IFXLS_FETCH()
# define IFX_TLS_VARS char *globals = (char *)&ifx_globals
extern ZEND_API php_ifx_globals ifx_globals;
#endif


#define MAX_RESID          64
#define BLOBINFILE 0      /* 0=in memory, 1=in file */

/* query result set data */
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


#else                                /* not HAVE_IFX  */

#define ifx_module_ptr NULL

#endif

#define phpext_informix_ptr ifx_module_ptr

#endif /* PHP_INFORMIX_H */
