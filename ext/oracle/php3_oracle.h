/* $Id$ */

#ifndef _PHP3_ORACLE_H
#define _PHP3_ORACLE_H

#if HAVE_ORACLE

#if (defined(__osf__) && defined(__alpha))
# ifndef A_OSF
#  define A_OSF
# endif
# ifndef OSF1
#  define OSF1
# endif
# ifndef _INTRINSICS
#  define _INTRINSICS
# endif
#endif /* osf alpha */

#include "oratypes.h"
#include "ocidfn.h"
#ifdef __STDC__
#include "ociapr.h"
#endif

extern php3_module_entry oracle_module_entry;
#define oracle_module_ptr &oracle_module_entry
#define phpext_oracle_ptr &oracle_module_entry

/* oparse flags */
#define  DEFER_PARSE        1
#define  NATIVE             1
#define  VERSION_7          2

#define ORANUMWIDTH			38

#if (defined(__osf__) && defined(__alpha)) || defined(CRAY) || defined(KSR)
#define HDA_SIZE 512
#else
#define HDA_SIZE 256
#endif

#define ORAUIDLEN 32
#define ORAPWLEN 32
#define ORANAMELEN 32
#define ORABUFLEN 2000

/* Some Oracle error codes */
#define VAR_NOT_IN_LIST			1007
#define NO_DATA_FOUND			1403
#define NULL_VALUE_RETURNED		1405

/* Some SQL and OCI function codes */
#define FT_INSERT			3
#define FT_SELECT			4
#define FT_UPDATE			5
#define FT_DELETE			9

#define FC_OOPEN			14

typedef struct {
	int open;
	Lda_Def lda;
	ub1 hda[HDA_SIZE];
} oraConnection;

typedef struct oraColumn {
	sb4 dbsize;
	sb2 dbtype;
	text cbuf[ORANAMELEN+1];
	sb4 cbufl;
	sb4 dsize;
	sb2 prec;
	sb2 scale;
	sb2 nullok;
	ub1 *buf;
	sb2 indp;
	ub2 col_retlen, col_retcode;
} oraColumn;

typedef struct oraParam {
	text *progv;
	sword progvl;
	sb2 inout;
	ub2 alen;
} oraParam;

typedef struct oraCursor {
	int open;
	Cda_Def cda;
	text *query;
	HashTable *params;
	int nparams;
	oraColumn *columns;
	int ncols;
	int fetched;
	oraConnection *conn_ptr;
} oraCursor;

typedef struct {
	char *defDB;
	char *defUser;
	char *defPW;
	long allow_persistent;
	long max_persistent;
	long max_links;
	long num_persistent;
	long num_links;
	int le_conn, le_pconn, le_cursor;
	HashTable *resource_list;
	HashTable *resource_plist;
} oracle_module;

PHP_FUNCTION(Ora_Bind);
PHP_FUNCTION(Ora_Close);
PHP_FUNCTION(Ora_Commit);
PHP_FUNCTION(Ora_CommitOff);
PHP_FUNCTION(Ora_CommitOn);
PHP_FUNCTION(Ora_Do);
PHP_FUNCTION(Ora_Error);
PHP_FUNCTION(Ora_ErrorCode);
PHP_FUNCTION(Ora_Exec);
PHP_FUNCTION(Ora_Fetch);
PHP_FUNCTION(Ora_FetchInto);
PHP_FUNCTION(Ora_ColumnType);
PHP_FUNCTION(Ora_ColumnName);
PHP_FUNCTION(Ora_ColumnSize);
PHP_FUNCTION(Ora_GetColumn);
PHP_FUNCTION(Ora_NumCols);
PHP_FUNCTION(Ora_NumRows);
PHP_FUNCTION(Ora_Logoff);
PHP_FUNCTION(Ora_Logon);
PHP_FUNCTION(Ora_PLogon);
PHP_FUNCTION(Ora_Open);
PHP_FUNCTION(Ora_Parse);
PHP_FUNCTION(Ora_Rollback);
extern int php3_minit_oracle(INIT_FUNC_ARGS);
extern int php3_mshutdown_oracle(SHUTDOWN_FUNC_ARGS);
extern int php3_rshutdown_oracle(SHUTDOWN_FUNC_ARGS);
void php3_info_oracle(ZEND_MODULE_INFO_FUNC_ARGS);
extern int php3_rinit_oracle(INIT_FUNC_ARGS);

#else

#define oracle_module_ptr NULL

#endif /* HAVE_ORACLE */

#endif /* _PHP3_ORACLE_H */

/*
 * Local Variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */




