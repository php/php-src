/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundatbion; either version 2 of the License, or (at your option) |
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
   | Authors: Stig Sæther Bakken <ssb@fast.no>                            |
   |          Thies C. Arntzen <thies@digicol.de>                         |
   +----------------------------------------------------------------------+
 */

/* $Id */

#if HAVE_OCI8
# ifndef PHP_OCI8_H
#  define PHP_OCI8_H
# endif

# if (defined(__osf__) && defined(__alpha))
#  ifndef A_OSF
#   define A_OSF
#  endif
#  ifndef OSF1
#   define OSF1
#  endif
#  ifndef _INTRINSICS
#   define _INTRINSICS
#  endif
# endif /* osf alpha */

#ifdef PHP_WIN32
#define PHP_OCI_API __declspec(dllexport)
#else
#define PHP_OCI_API
#endif                                   

#include <oci.h>

typedef struct {
	int num;
	int persistent;
	int open;
	char *dbname;
    OCIServer *pServer;
#if 0
	OCIFocbkStruct failover;
#endif
} oci_server;

typedef struct {
	int num;
	int persistent;
	int open;
	int exclusive;
	char *hashed_details;
	oci_server *server;
	OCISession *pSession;
} oci_session;

typedef struct {
	int id;
	int open;
	oci_session *session;
    OCISvcCtx *pServiceContext;
	sword error;
    OCIError *pError;
} oci_connection;

typedef struct {
	int id;
	oci_connection *conn;
	dvoid *ocidescr;
	ub4 type;
} oci_descriptor;

typedef struct {
    zval *zval;
    text *name;
    ub4 name_len;
	ub4 type;
} oci_define;

typedef struct {
	int id;
	oci_connection *conn;
	sword error;
    OCIError *pError;
    OCIStmt *pStmt;
	char *last_query;
	HashTable *columns;
	HashTable *binds;
	HashTable *defines;
	int ncolumns;
	int executed;
} oci_statement;

typedef struct {
	OCIBind *pBind;
	zval *zval;
	dvoid *descr;		/* used for binding of LOBS etc */
    OCIStmt *pStmt;     /* used for binding REFCURSORs */
	sb2 indicator;
	ub2 retcode;
} oci_bind;

typedef struct {
	oci_statement *statement;
	OCIDefine *pDefine;
    char *name;
    ub4 name_len;
    ub2 data_type;
    ub2 data_size;
    ub4 storage_size4;
	sb2 indicator;
	ub2 retcode;
	ub2 retlen;
	ub4 retlen4;
	ub2 is_descr;
	ub2 is_cursor;
    int descr;
    oci_statement *pstmt;
	int stmtid;
	int descid;
	void *data;
	oci_define *define;
	int piecewise;
	ub4 cb_retlen;
   ub2 scale;
   ub2 precision;    	
} oci_out_column;

typedef struct {
	sword error;
    OCIError *pError;
		
	/*
    char *default_username;
    char *default_password;
    char *default_dbname;
	*/

    long debug_mode;

	int shutdown;

	/* XXX NYI
    long allow_persistent;
    long max_persistent;
    long max_links;
    long num_persistent;
    long num_links;
	*/

	int server_num;
    HashTable *server;
	int user_num;
	HashTable *user;

    OCIEnv *pEnv;
} php_oci_globals;

extern zend_module_entry oci8_module_entry;
#define phpext_oci8_ptr &oci8_module_entry

#define OCI_MAX_NAME_LEN  64
#define OCI_MAX_DATA_SIZE INT_MAX
#define OCI_PIECE_SIZE    (64*1024)-1

#ifdef ZTS
#define OCILS_D php_oci_globals *oci_globals
#define OCILS_DC , OCILS_D
#define OCILS_C oci_globals
#define OCILS_CC , OCILS_C
#define OCI(v) (oci_globals->v)
#define OCILS_FETCH() php_oci_globals *oci_globals = ts_resource(oci_globals_id)
#else
#define OCILS_D
#define OCILS_DC
#define OCILS_C
#define OCILS_CC
#define OCI(v) (oci_globals.v)
#define OCILS_FETCH()
#endif

#else /* !HAVE_OCI8 */

# define oci8_module_ptr NULL

#endif /* HAVE_OCI8 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
