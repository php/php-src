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
   | Authors: Stig Sæther Bakken <ssb@fast.no>                            |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

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

#if defined(min)
#undef min
#endif
#if defined(max)
#undef max
#endif

#include <oci.h>

typedef struct {
	int num;
	int persistent;
	int is_open;
	char *dbname;
    OCIServer *pServer;
#if 0
	OCIFocbkStruct failover;
#endif
} oci_server;

typedef struct {
	int num;
	int persistent;
	int is_open;
	int exclusive;
	char *hashed_details;
	oci_server *server;
	OCISession *pSession;
	OCIEnv *pEnv;				/* sessions own environment */
	ub2 charsetId;				/* sessions used character set (mostly this will be 0, so NLS_LANG will be used. */
} oci_session;

typedef struct {
	int id;
	int is_open;
	oci_session *session;
    OCISvcCtx *pServiceContext;
	sword error;
    OCIError *pError;
	int needs_commit;
} oci_connection;

typedef struct {
	int id;
	oci_connection *conn;
	dvoid *ocidescr;
	ub4 type;
} oci_descriptor;

typedef struct {
    int id;
    oci_connection *conn;
    OCIType     *tdo;
    OCITypeCode coll_typecode;
    OCIRef      *elem_ref;
    OCIType     *element_type;
    OCITypeCode element_typecode;
    OCIColl     *coll;
} oci_collection;

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
	int has_data;
	ub2 stmttype;
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

    HashTable *server;
	HashTable *user;

    OCIEnv *pEnv;

	int in_call;
} php_oci_globals;

extern zend_module_entry oci8_module_entry;
#define phpext_oci8_ptr &oci8_module_entry

#define OCI_MAX_NAME_LEN  64
#define OCI_MAX_DATA_SIZE INT_MAX
#define OCI_PIECE_SIZE    (64*1024)-1

#ifdef ZTS
#define OCI(v) TSRMG(oci_globals_id, php_oci_globals *, v)
#else
#define OCI(v) (oci_globals.v)
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
