/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
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
	
	efree(stmt->columns); 
	stmt->columns = NULL;
	
	return 1;
}

static int pdo_dblib_stmt_dtor(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;

	efree(stmt->columns); 
	stmt->columns = NULL;

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
	
	if(colno >= stmt->column_count || colno < 0)  {
		return FAILURE;
	}
	
	struct pdo_column_data *col = &stmt->columns[colno];
	
	col->name = (char*)dbcolname(H->link, colno+1);
	col->maxlen = dbcollen(H->link, colno+1);
	col->namelen = strlen(col->name);
	col->param_type = PDO_PARAM_ZVAL;
		
	return 1;
}

static int pdo_dblib_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr,
	 unsigned long *len, int *caller_frees TSRMLS_DC)
{
	
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;
	
	int coltype;
	char *data, *tmp_data;
	unsigned int data_len, tmp_data_len;
	zval *zv = NULL;

	coltype = dbcoltype(H->link, colno+1);
	data = dbdata(H->link, colno+1);
	data_len = dbdatlen(H->link, colno+1);

	if (data_len != 0 && data != NULL) {
		if (stmt->dbh->stringify) {
			switch (coltype) {
        case SQLDATETIME:
				case SQLDATETIM4:
				case SQLFLT4:
				case SQLFLT8:
				case SQLINT4:
				case SQLINT2:
				case SQLINT1:
				case SQLBIT: {
					if (dbwillconvert(coltype, SQLCHAR)) {
						tmp_data_len = 32 + (2 * (data_len)); /* FIXME: We allocate more than we need here */
						tmp_data = emalloc(tmp_data_len);
						data_len = dbconvert(NULL, coltype, data, data_len, SQLCHAR, tmp_data, -1);

						MAKE_STD_ZVAL(zv);
						ZVAL_STRING(zv, tmp_data, 0);
					}
					break;
        }
			}
		}

		if (!zv) {
			switch (coltype) {
				case SQLCHAR:
				case SQLVARCHAR:
				case SQLTEXT:
#if ilia_0
					while (data_len>0 && data[data_len-1] == ' ') { // nuke trailing whitespace
						data_len--;
					}
#endif
				case SQLVARBINARY:
				case SQLBINARY:
				case SQLIMAGE: {
					MAKE_STD_ZVAL(zv);
					if (!data_len) {
						ZVAL_NULL(zv);
					} else {
						ZVAL_STRINGL(zv, data, data_len, 1);
					}

					break;
				}
				case SQLDATETIME:
				case SQLDATETIM4: {
					DBDATEREC dateinfo;

					if (coltype == SQLDATETIM4) {
						DBDATETIME temp;

						dbconvert(NULL, SQLDATETIM4, data, -1, SQLDATETIME, (LPBYTE)&temp, -1);
						dbdatecrack(H->link, &dateinfo, &temp);
					} else {
					  dbdatecrack(H->link, &dateinfo, (DBDATETIME *)data);
					}

					spprintf(&tmp_data, 0, "%d-%02d-%02d %02d:%02d:%02d", dateinfo.year, dateinfo.month, dateinfo.day, dateinfo.hour, dateinfo.minute, dateinfo.second);

					MAKE_STD_ZVAL(zv);
					ZVAL_STRINGL(zv, tmp_data, 19, 0);

					break;
				}
				case SQLFLT4: {
					MAKE_STD_ZVAL(zv);
					ZVAL_DOUBLE(zv, (double) (*(DBFLT4 *) data));

					break;
				}
				case SQLFLT8: {
					MAKE_STD_ZVAL(zv);
					ZVAL_DOUBLE(zv, (double) (*(DBFLT8 *) data));

					break;
				}
				case SQLINT4: {
					MAKE_STD_ZVAL(zv);
					ZVAL_LONG(zv, (long) ((int) *(DBINT *) data));

					break;
				}
				case SQLINT2: {
					MAKE_STD_ZVAL(zv);
					ZVAL_LONG(zv, (long) ((int) *(DBSMALLINT *) data));

					break;
				}
				case SQLINT1:
				case SQLBIT: {
					MAKE_STD_ZVAL(zv);
					ZVAL_LONG(zv, (long) ((int) *(DBTINYINT *) data));

					break;
				}
				case SQLMONEY:
				case SQLMONEY4:
				case SQLMONEYN: {
					DBFLT8 money_value;
					dbconvert(NULL, coltype, data, 8, SQLFLT8, (LPBYTE)&money_value, -1);

					MAKE_STD_ZVAL(zv);

					if (stmt->dbh->stringify) {
						tmp_data_len = spprintf(&tmp_data, 0, "%.4f", money_value);
						ZVAL_STRINGL(zv, tmp_data, tmp_data_len, 1);
					} else {
						ZVAL_DOUBLE(zv, money_value);
					}

					break;
				}
#ifdef SQLUNIQUE
				case SQLUNIQUE: {
#else
				case 36: { // FreeTDS hack
#endif
					MAKE_STD_ZVAL(zv);
					ZVAL_STRINGL(zv, data, 16, 1); // uniqueidentifier is a 16-byte binary number

					break;
				}
				default: {
					if (dbwillconvert(coltype, SQLCHAR)) {
						tmp_data_len = 32 + (2 * (data_len)); /* FIXME: We allocate more than we need here */
						tmp_data = emalloc(tmp_data_len);
						data_len = dbconvert(NULL, coltype, data, data_len, SQLCHAR, tmp_data, -1);

						MAKE_STD_ZVAL(zv);
						ZVAL_STRING(zv, tmp_data, 0);
					}
				}
			}
		}
	}

	if (zv != NULL) {
		*ptr = (char*)&zv;
		*len = sizeof(zval);
	} else {
		*ptr = NULL;
		*len = 0;
	}

	*caller_frees = 0;

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
	int coltype;

	if(colno >= stmt->column_count || colno < 0)  {
		return FAILURE;
	}

	array_init(return_value);

	dbtypeinfo = dbcoltypeinfo(H->link, colno+1);

	if(!dbtypeinfo) return FAILURE;

	coltype = dbcoltype(H->link, colno+1);

	add_assoc_long(return_value, "max_length", dbcollen(H->link, colno+1) );
	add_assoc_long(return_value, "precision", (int) dbtypeinfo->precision );
	add_assoc_long(return_value, "scale", (int) dbtypeinfo->scale );
	add_assoc_string(return_value, "column_source", dbcolsource(H->link, colno+1), 1);
	add_assoc_string(return_value, "native_type", pdo_dblib_get_field_name(coltype), 1);
	add_assoc_long(return_value, "native_type_id", coltype);
	add_assoc_long(return_value, "native_usertype_id", dbcolutype(H->link, colno+1));

	switch (coltype) {
		case SQLBIT:
		case SQLINT1:
		case SQLINT2:
		case SQLINT4:
		  add_assoc_long(return_value, "pdo_type", PDO_PARAM_INT);
		  break;
		default:
		  add_assoc_long(return_value, "pdo_type", PDO_PARAM_STR);
		  break;
	}

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

