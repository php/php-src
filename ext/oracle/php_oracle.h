/* $Id$ */

/* synced with oracle.h,v 1.40 1999/06/01 08:11:04 thies Exp $ */

#ifndef PHP_ORACLE_H
#define PHP_ORACLE_H

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
#include "ociapr.h"

extern zend_module_entry oracle_module_entry;
#define phpext_oracle_ptr &oracle_module_entry

#ifdef ZTS
#define ORA(v) TSRMG(ora_globals_id, php_ora_globals *, v)
#else
#define ORA(v) (ora_globals.v)
#endif

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

/* Some SQL and ORA function codes */
#define FT_INSERT			3
#define FT_SELECT			4
#define FT_UPDATE			5
#define FT_DELETE			9

#define FC_OOPEN			14

typedef struct {
	int open;
	int persistent;
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
	int conn_id;
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
	HashTable *conns;
	oraConnection db_err_conn; 
} php_ora_globals;

#else

#define phpext_oracle_ptr NULL

#endif /* HAVE_ORACLE */

#endif /* PHP_ORACLE_H */

/*
 * Local Variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */




