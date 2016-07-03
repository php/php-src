/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@php.net>                                    |
  |         Frank M. Kromann <frank@kromann.info>                        |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "php_pdo_dblib.h"
#include "php_pdo_dblib_int.h"
#include "zend_exceptions.h"


/* {{{ pdo_dblib_get_field_name
 * 
 * Return the data type name for a given TDS number
 * 
 */
static char *pdo_dblib_get_field_name(int type)
{
	/* 
	 * I don't return dbprtype(type) because it does not fully describe the type 
	 * (example: varchar is reported as char by dbprtype)
	 * 
	 * FIX ME: Cache datatypes from server systypes table in pdo_dblib_handle_factory()
	 * 		   to make this future proof.
	 */
	 
	switch (type) {
		case 31: return "nvarchar";
		case 34: return "image";
		case 35: return "text";
		case 36: return "uniqueidentifier";
		case 37: return "varbinary"; /* & timestamp - Sybase AS12 */
		case 38: return "bigint"; /* & bigintn - Sybase AS12 */
		case 39: return "varchar"; /* & sysname & nvarchar - Sybase AS12 */
		case 40: return "date";
		case 41: return "time";
		case 42: return "datetime2";
		case 43: return "datetimeoffset";
		case 45: return "binary"; /* Sybase AS12 */
		case 47: return "char"; /* & nchar & uniqueidentifierstr Sybase AS12 */
		case 48: return "tinyint";
		case 50: return "bit"; /* Sybase AS12 */
		case 52: return "smallint";
		case 55: return "decimal"; /* Sybase AS12 */
		case 56: return "int";
		case 58: return "smalldatetime";
		case 59: return "real";
		case 60: return "money";
		case 61: return "datetime";
		case 62: return "float";
		case 63: return "numeric"; /* or uint, ubigint, usmallint Sybase AS12 */
		case 98: return "sql_variant";
		case 99: return "ntext";
		case 104: return "bit";
		case 106: return "decimal"; /* decimal n on sybase */
		case 108: return "numeric"; /* numeric n on sybase */
		case 122: return "smallmoney";
		case 127: return "bigint";
		case 165: return "varbinary";
		case 167: return "varchar";
		case 173: return "binary";
		case 175: return "char";
		case 189: return "timestamp";
		case 231: return "nvarchar";
		case 239: return "nchar";
		case 240: return "geometry";
		case 241: return "xml";
		default: return "unknown";
	}
}
/* }}} */

static int pdo_dblib_stmt_cursor_closer(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;

	/* Cancel any pending results */
	dbcancel(H->link);
	
	return 1;
}

static int pdo_dblib_stmt_dtor(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;

	efree(S);
		
	return 1;
}

static int pdo_dblib_stmt_next_rowset(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;
	RETCODE ret;
	
	ret = dbresults(H->link);
	
	if (FAIL == ret) {
		pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "PDO_DBLIB: dbresults() returned FAIL" TSRMLS_CC);		
		return 0;
	}
		
	if(NO_MORE_RESULTS == ret) {
		return 0;
	}
	
	stmt->row_count = DBCOUNT(H->link);
	stmt->column_count = dbnumcols(H->link);
	
	return 1;
}

static int pdo_dblib_stmt_execute(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;
	RETCODE ret;
	
	dbsetuserdata(H->link, (BYTE*) &S->err);
	
	pdo_dblib_stmt_cursor_closer(stmt TSRMLS_CC);
	
	if (FAIL == dbcmd(H->link, stmt->active_query_string)) {
		return 0;
	}
	
	if (FAIL == dbsqlexec(H->link)) {
		return 0;
	}
	
	ret = pdo_dblib_stmt_next_rowset(stmt TSRMLS_CC);
	
	stmt->row_count = DBCOUNT(H->link);
	stmt->column_count = dbnumcols(H->link);
	
	return 1;
}

static int pdo_dblib_stmt_fetch(pdo_stmt_t *stmt,
	enum pdo_fetch_orientation ori, long offset TSRMLS_DC)
{
	
	RETCODE ret;
	
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;
	
	ret = dbnextrow(H->link);
	
	if (FAIL == ret) {
		pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "PDO_DBLIB: dbnextrow() returned FAIL" TSRMLS_CC);
		return 0;
	}
		
	if(NO_MORE_ROWS == ret) {
		return 0;
	}
	
	return 1;	
}

static int pdo_dblib_stmt_describe(pdo_stmt_t *stmt, int colno TSRMLS_DC)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;
	struct pdo_column_data *col;
	char *fname;
	
	if(colno >= stmt->column_count || colno < 0)  {
		return FAILURE;
	}
	
	col = &stmt->columns[colno];
	fname = (char*)dbcolname(H->link, colno+1);

	if (fname && *fname) {
		col->name = estrdup(fname);
		col->namelen = strlen(col->name);
	} else {
		col->namelen = spprintf(&col->name, 0, "computed%d", colno);
	}
	col->maxlen = dbcollen(H->link, colno+1);
	col->param_type = PDO_PARAM_STR;
		
	return 1;
}

static int pdo_dblib_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr,
	 unsigned long *len, int *caller_frees TSRMLS_DC)
{
	
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;
	
	int coltype;
	unsigned int tmp_len;
	char *tmp_ptr = NULL;
	
	coltype = dbcoltype(H->link, colno+1);
	
	*len = dbdatlen(H->link, colno+1);
	*ptr = dbdata(H->link, colno+1);
	
	if (*len == 0 && *ptr == NULL) {
		return 1;
	}
	
	switch (coltype) {
		case SQLVARBINARY:
		case SQLBINARY:
		case SQLIMAGE:
		case SQLTEXT:
			/* FIXME: Above types should be returned as a stream as they can be VERY large */
		case SQLCHAR:
		case SQLVARCHAR:
			tmp_ptr = emalloc(*len + 1);
			memcpy(tmp_ptr, *ptr, *len);
			tmp_ptr[*len] = '\0';
			*ptr = tmp_ptr;
			break;
		case SQLMONEY:
		case SQLMONEY4:
		case SQLMONEYN: {
			DBFLT8 money_value;
			dbconvert(NULL, coltype, *ptr, *len, SQLFLT8, (LPBYTE)&money_value, 8);
			*len = spprintf(&tmp_ptr, 0, "%.4f", money_value);
			*ptr = tmp_ptr;
			break;
		}
		case SQLUNIQUE: {
			*len = 36+1;
			tmp_ptr = emalloc(*len + 1);

			/* uniqueidentifier is a 16-byte binary number, convert to 32 char hex string */
			*len = dbconvert(NULL, SQLUNIQUE, *ptr, *len, SQLCHAR, tmp_ptr, *len);
			php_strtoupper(tmp_ptr, *len);
			*ptr = tmp_ptr;
			break;
		}
		case SQLDATETIM4:
		case SQLDATETIME: {
			DBDATETIME dt;
			DBDATEREC di;

			dbconvert(H->link, coltype, (BYTE*) *ptr, -1, SQLDATETIME, (LPBYTE) &dt, -1);
			dbdatecrack(H->link, &di, &dt);

			*len = spprintf((char**) &tmp_ptr, 20, "%d-%02d-%02d %02d:%02d:%02d",
#if defined(PHP_DBLIB_IS_MSSQL) || defined(MSDBLIB)
					di.year,     di.month,       di.day,        di.hour,     di.minute,     di.second
#else
					di.dateyear, di.datemonth+1, di.datedmonth, di.datehour, di.dateminute, di.datesecond
#endif
				);

			*ptr = (char*) tmp_ptr;
			break;
		}
		default:
			if (dbwillconvert(coltype, SQLCHAR)) {
				tmp_len = 32 + (2 * (*len)); /* FIXME: We allocate more than we need here */
				tmp_ptr = emalloc(tmp_len);
				*len = dbconvert(NULL, coltype, *ptr, *len, SQLCHAR, tmp_ptr, -1);
				*ptr = tmp_ptr;
			} else {
				*len = 0; /* FIXME: Silently fails and returns null on conversion errors */
				*ptr = NULL;
			}
	}

	*caller_frees = 1;

	return 1;
}

static int pdo_dblib_stmt_param_hook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param,
		enum pdo_param_event event_type TSRMLS_DC)
{
	return 1;
}

static int pdo_dblib_stmt_get_column_meta(pdo_stmt_t *stmt, long colno, zval *return_value TSRMLS_DC)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;
	DBTYPEINFO* dbtypeinfo;

	if(colno >= stmt->column_count || colno < 0)  {
		return FAILURE;
	}

	array_init(return_value);

	dbtypeinfo = dbcoltypeinfo(H->link, colno+1);
	
	if(!dbtypeinfo) return FAILURE;
		
	add_assoc_long(return_value, "max_length", dbcollen(H->link, colno+1) );
	add_assoc_long(return_value, "precision", (int) dbtypeinfo->precision );
	add_assoc_long(return_value, "scale", (int) dbtypeinfo->scale );
	add_assoc_string(return_value, "column_source", dbcolsource(H->link, colno+1), 1);
	add_assoc_string(return_value, "native_type", pdo_dblib_get_field_name(dbcoltype(H->link, colno+1)), 1);
	add_assoc_long(return_value, "native_type_id", dbcoltype(H->link, colno+1));
	add_assoc_long(return_value, "native_usertype_id", dbcolutype(H->link, colno+1));

	return 1;
}


struct pdo_stmt_methods dblib_stmt_methods = {
	pdo_dblib_stmt_dtor,
	pdo_dblib_stmt_execute,
	pdo_dblib_stmt_fetch,
	pdo_dblib_stmt_describe,
	pdo_dblib_stmt_get_col,
	pdo_dblib_stmt_param_hook,
	NULL, /* set attr */
	NULL, /* get attr */
	pdo_dblib_stmt_get_column_meta, /* meta */
	pdo_dblib_stmt_next_rowset, /* nextrow */
	pdo_dblib_stmt_cursor_closer
};

