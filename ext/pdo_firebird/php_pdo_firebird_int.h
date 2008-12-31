/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2009 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Ard Biesheuvel <abies@php.net>                               |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_PDO_FIREBIRD_INT_H
#define PHP_PDO_FIREBIRD_INT_H

#include <ibase.h>

#ifdef SQLDA_VERSION
#define PDO_FB_SQLDA_VERSION SQLDA_VERSION
#else
#define PDO_FB_SQLDA_VERSION 1
#endif

#define PDO_FB_DIALECT 3

#define SHORT_MAX (1 << (8*sizeof(short)-1))

#if SIZEOF_LONG == 8
# define LL_MASK "l"
# define LL_LIT(lit) lit ## L
#else
# ifdef PHP_WIN32
#  define LL_MASK "I64"
#  define LL_LIT(lit) lit ## I64
# else
#  define LL_MASK "ll"
#  define LL_LIT(lit) lit ## LL
# endif
#endif

/* Firebird API has a couple of missing const decls in its API */
#define const_cast(s) ((char*)(s))

#ifdef PHP_WIN32
typedef void (__stdcall *info_func_t)(char*);
#else
typedef void (*info_func_t)(char*);
#endif

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

typedef struct {

	/* the result of the last API call */
	ISC_STATUS isc_status[20];

	/* the connection handle */
	isc_db_handle db;

	/* the transaction handle */
	isc_tr_handle tr;

	/* the last error that didn't come from the API */
	char const *last_app_error;
	
} pdo_firebird_db_handle;


typedef struct {
	
	/* the link that owns this statement */
	pdo_firebird_db_handle *H;
	
	/* the statement handle */
	isc_stmt_handle stmt;
	
	/* the name of the cursor (if it has one) */
	char name[32];
	
	/* the type of statement that was issued */
	char statement_type:8;
	
	/* whether EOF was reached for this statement */
	unsigned exhausted:1;

	unsigned _reserved:23;

	/* the named params that were converted to ?'s by the driver */
	HashTable *named_params;
	
	/* allocated space to convert fields values to other types */
	char **fetch_buf;
	
	/* the input SQLDA */
	XSQLDA *in_sqlda;
	
	/* the output SQLDA */
	XSQLDA out_sqlda; /* last member */
	
} pdo_firebird_stmt;

extern pdo_driver_t pdo_firebird_driver;

extern struct pdo_stmt_methods firebird_stmt_methods;

void _firebird_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, char const *file, long line TSRMLS_DC);

#endif	/* PHP_PDO_FIREBIRD_INT_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
