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
   |                                                                      |
   | Initial work sponsored by Thies Arntzen <thies@digicol.de> of        |
   | Digital Collections, http://www.digicol.de/                          |
   +----------------------------------------------------------------------+
 */

/* $Id */

#if HAVE_OCI8
# ifndef _PHP_OCI8_H
#  define _PHP_OCI8_H
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

#include <oci.h>

typedef struct {
	int num;
	int persistent;
	int open;
	char *dbname;
    OCIServer *pServer;
	OCIFocbkStruct failover;
} oci8_server;

typedef struct {
	int num;
	int persistent;
	int open;
	oci8_server *server;
	OCISession *pSession;
} oci8_session;

typedef struct {
	int id;
	int open;
	oci8_session *session;
    OCISvcCtx *pServiceContext;
	sword error;
    OCIError *pError;
	HashTable *descriptors;
	int descriptors_count;
} oci8_connection;

typedef struct {
	dvoid *ocidescr;
	ub4 type;
} oci8_descriptor;

typedef struct {
    pval *pval;
    text *name;
    ub4 name_len;
	ub4 type;
} oci8_define;

typedef struct {
	int id;
	oci8_connection *conn;
	sword error;
    OCIError *pError;
    OCIStmt *pStmt;
	char *last_query;
	HashTable *columns;
	int ncolumns;
	HashTable *binds;
	HashTable *defines;
	int executed;
} oci8_statement;

typedef struct {
	OCIBind *pBind;
	pval *value;
	dvoid *descr;		/* used for binding of LOBS etc */
    OCIStmt *pStmt;     /* used for binding REFCURSORs */
	ub4 maxsize;
	sb2 indicator;
	ub2 retcode;
} oci8_bind;

typedef struct {
	oci8_statement *statement;
	OCIDefine *pDefine;
    char *name;
    ub4 name_len;
    ub2 type;
    ub4 size4;
    ub4 storage_size4;
	ub2 size2;
	sb2 indicator;
	ub2 retcode;
	ub4 rlen;
	ub2 is_descr;
	ub2 is_cursor;
    int descr;
    oci8_descriptor *pdescr;
    oci8_statement *pstmt;
	int stmtid;
	void *data;
	oci8_define *define;

	/* for piecewise read */
	int piecewise;
	int cursize;
	int curoffs;
    ub4 piecesize;
} oci8_out_column;

typedef struct {
	sword error;
    OCIError *pError;
    char *default_username;
    char *default_password;
    char *default_dbname;

    long debug_mode;

	/* XXX NYI
    long allow_persistent;
    long max_persistent;
    long max_links;
    long num_persistent;
    long num_links;
	*/

    int le_conn; /* active connections */
    int le_stmt; /* active statements */
    int le_trans; /* active transactions */

	int server_num;
    HashTable *server;
	int user_num;
	HashTable *user;

    OCIEnv *pEnv;
} oci8_module;

extern php3_module_entry oci8_module_entry;
#define oci8_module_ptr &oci8_module_entry
#define phpext_oci8_ptr &oci8_module_entry

#define OCI8_MAX_NAME_LEN 64
#define OCI8_MAX_DATA_SIZE INT_MAX
#define OCI8_PIECE_SIZE (64*1024)-1
# define OCI8_CONN_TYPE(x) ((x)==OCI8_GLOBAL(php3_oci8_module).le_conn)
# define OCI8_STMT_TYPE(x) ((x)==OCI8_GLOBAL(php3_oci8_module).le_stmt)

# define RETURN_OUT_OF_MEMORY \
	php3_error(E_WARNING, "Out of memory");\
	RETURN_FALSE
# define OCI8_FAIL(c,f,r) \
	php3i_oci8_error((c)->pError,(f),(r));\
	RETURN_FALSE

#else /* !HAVE_OCI8 */

# define oci8_module_ptr NULL

#endif /* HAVE_OCI8 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
