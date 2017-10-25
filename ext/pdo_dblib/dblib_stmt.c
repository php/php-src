/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
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
#include "ext/date/php_date.h"
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

static int pdo_dblib_stmt_cursor_closer(pdo_stmt_t *stmt)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;

	/* Cancel any pending results */
	dbcancel(H->link);

	pdo_dblib_err_dtor(&H->err);

	return 1;
}

static int pdo_dblib_stmt_dtor(pdo_stmt_t *stmt)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;

	pdo_dblib_err_dtor(&S->err);

	efree(S);

	return 1;
}

static int pdo_dblib_stmt_next_rowset_no_cancel(pdo_stmt_t *stmt)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;
	RETCODE ret;
	int num_fields;

	do {
		ret = dbresults(H->link);
		num_fields = dbnumcols(H->link);
	} while (H->skip_empty_rowsets && num_fields <= 0 && ret == SUCCEED);


	if (FAIL == ret) {
		pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "PDO_DBLIB: dbresults() returned FAIL");
		return 0;
	}

	if (NO_MORE_RESULTS == ret) {
		return 0;
	}

	if (H->skip_empty_rowsets && num_fields <= 0) {
		return 0;
	}

	stmt->row_count = DBCOUNT(H->link);
	stmt->column_count = num_fields;

	return 1;
}

static int pdo_dblib_stmt_next_rowset(pdo_stmt_t *stmt)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;
	RETCODE ret = SUCCESS;

	/* Ideally use dbcanquery here, but there is a bug in FreeTDS's implementation of dbcanquery
	 * It has been resolved but is currently only available in nightly builds
	 */
	while (NO_MORE_ROWS != ret) {
		ret = dbnextrow(H->link);

		if (FAIL == ret) {
			pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "PDO_DBLIB: dbnextrow() returned FAIL");
			return 0;
		}
	}

	return pdo_dblib_stmt_next_rowset_no_cancel(stmt);
}

static int pdo_dblib_stmt_execute(pdo_stmt_t *stmt)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;

	dbsetuserdata(H->link, (BYTE*) &S->err);

	pdo_dblib_stmt_cursor_closer(stmt);

	if (FAIL == dbcmd(H->link, stmt->active_query_string)) {
		return 0;
	}

	if (FAIL == dbsqlexec(H->link)) {
		return 0;
	}

	pdo_dblib_stmt_next_rowset_no_cancel(stmt);

	stmt->row_count = DBCOUNT(H->link);
	stmt->column_count = dbnumcols(H->link);

	return 1;
}

static int pdo_dblib_stmt_fetch(pdo_stmt_t *stmt,
	enum pdo_fetch_orientation ori, zend_long offset)
{

	RETCODE ret;

	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;

	ret = dbnextrow(H->link);

	if (FAIL == ret) {
		pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "PDO_DBLIB: dbnextrow() returned FAIL");
		return 0;
	}

	if(NO_MORE_ROWS == ret) {
		return 0;
	}

	return 1;
}

static int pdo_dblib_stmt_describe(pdo_stmt_t *stmt, int colno)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;
	struct pdo_column_data *col;
	char *fname;

	if(colno >= stmt->column_count || colno < 0)  {
		return FAILURE;
	}

	if (colno == 0) {
		S->computed_column_name_count = 0;
	}

	col = &stmt->columns[colno];
	fname = (char*)dbcolname(H->link, colno+1);

	if (fname && *fname) {
		col->name =  zend_string_init(fname, strlen(fname), 0);
	} else {
		if (S->computed_column_name_count > 0) {
			char buf[16];
			int len;

			len = snprintf(buf, sizeof(buf), "computed%d", S->computed_column_name_count);
			col->name = zend_string_init(buf, len, 0);
		} else {
			col->name = zend_string_init("computed", strlen("computed"), 0);
		}

		S->computed_column_name_count++;
	}

	col->maxlen = dbcollen(H->link, colno+1);
	col->param_type = PDO_PARAM_ZVAL;

	return 1;
}

static int pdo_dblib_datetime_format(zval *output, zend_string *format, DBBIGINT bigdatetime)
{
	DBBIGINT             temp_ts;
	time_t               ts;
	zend_string         *buf;
	static const char   *default_format = "Y-m-d H:i:s";
	char                *ok_format = (char *)default_format;

	if (format && ZSTR_LEN(format) > 0) {
		ok_format = ZSTR_VAL(format);
	}

	/* convert bigdatetime to unix timestamp
	 *
	 * "bigdatetime is a 64 bit integer containing the number of microseconds since 01/01/0000"
	 *
	 * - substract 719528 (number of days from here to unix epoch) * 86400 * 1000000
	 * - integer division to seconds (via floor to support negative values)
	 */
	temp_ts = bigdatetime - 62167219200000000u;
	ts = floor(temp_ts / (double)1000000);

	/* replace u/f in format with micro/milli seconds
	 *
	 * Look for the first "u" and "f" in the format string.
	 * If found, replace it with micro and milli seconds.
	 * For example, if milliseconds is 124, the format string
	 * "Y-m-d H:i:s.f" would become
	 * "Y-m-d H:i:s.124".
	 *
	 * temp buffers are larger to accomodate for the added data: f => +2, u => +5
	 * microseconds = remainder of the integer division
	 * milliseconds = round(remainder / 1000)
	 *
	 * credits to FreeTDS's tds_strftime
	 */
	char *our_format;
	char *base_format;
	char *pz = NULL;
	our_format = emalloc(strlen(ok_format) + 2 + 5);
	base_format = emalloc(strlen(ok_format) + 2 + 5);
	strcpy(our_format, ok_format);
	strcpy(base_format, our_format);
	if ((pz = strstr(our_format, "u")) != NULL) {
		char micro[6];
		sprintf(micro, "%06d", temp_ts - ts * 1000000);
		memcpy(pz, micro, 6);
		strcpy(pz + 6, base_format + (pz - our_format) + 1);
		strcpy(base_format, our_format);
	}
	if ((pz = strstr(our_format, "f")) != NULL) {
		char milli[3];
		sprintf(milli, "%03.0f", (double)(temp_ts - ts * 1000000) / 1000);
		memcpy(pz, milli, 3);
		strcpy(pz + 3, base_format + (pz - our_format) + 1);
	}

	buf = php_format_date(our_format, strlen(our_format), ts, 0);
	ZVAL_STR(output, buf);

	efree(our_format);
	efree(base_format);

	return 1;
}

static int pdo_dblib_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr,
	 zend_ulong *len, int *caller_frees)
{

	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;

	int coltype;
	LPBYTE data;
	DBCHAR *tmp_data;
	DBINT data_len, tmp_data_len;
	zval *zv = NULL;

	coltype = dbcoltype(H->link, colno+1);
	data = dbdata(H->link, colno+1);
	data_len = dbdatlen(H->link, colno+1);

	if (data_len != 0 || data != NULL) {
		/* force stringify if DBBIGINT won't fit in zend_long */
		/* this should only be an issue for 32-bit machines */
		if (stmt->dbh->stringify || (coltype == SQLINT8 && sizeof(zend_long) < sizeof(DBBIGINT))) {
			switch (coltype) {
				case SQLDECIMAL:
				case SQLNUMERIC:
				case SQLMONEY:
				case SQLMONEY4:
				case SQLMONEYN:
				case SQLFLT4:
				case SQLFLT8:
				case SQLINT8:
				case SQLINT4:
				case SQLINT2:
				case SQLINT1:
				case SQLBIT: {
					if (dbwillconvert(coltype, SQLCHAR)) {
						tmp_data_len = 32 + (2 * (data_len)); /* FIXME: We allocate more than we need here */
						tmp_data = emalloc(tmp_data_len);
						data_len = dbconvert(NULL, coltype, data, data_len, SQLCHAR, (LPBYTE) tmp_data, -1);

						zv = emalloc(sizeof(zval));
						ZVAL_STRING(zv, tmp_data);

						efree(tmp_data);
					}
					break;
				}
			}
		}

		if (!zv) {
			switch (coltype) {
				case SQLCHAR:
				case SQLVARCHAR:
				case SQLTEXT: {
#if ilia_0
					while (data_len>0 && data[data_len-1] == ' ') { /* nuke trailing whitespace */
						data_len--;
					}
#endif
				}
				case SQLVARBINARY:
				case SQLBINARY:
				case SQLIMAGE: {
					zv = emalloc(sizeof(zval));
					ZVAL_STRINGL(zv, (DBCHAR *) data, data_len);

					break;
				}
				case SQLDATETIME:
				case SQLDATETIM4: {
					if (H->datetime_convert) {
						tmp_data_len = 63; /* hardcoded maximum length in freetds */
						tmp_data = emalloc(tmp_data_len);
						tmp_data_len = dbconvert(NULL, coltype, data, data_len, SQLCHAR, tmp_data, tmp_data_len);
						if (tmp_data_len == (unsigned int) -1) { /* in case tmp_data_len wasn't enough */
							pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "PDO_DBLIB: dbconvert() from datetime error");
							efree(tmp_data);
							break;
						}

						zv = emalloc(sizeof(zval));
						ZVAL_STRINGL(zv, tmp_data, tmp_data_len);
						efree(tmp_data);
					} else {
						DBBIGINT bigdatetime;
						dbconvert(H->link, coltype, data, -1, SYBBIGDATETIME, (LPBYTE) &bigdatetime, -1);

						zv = emalloc(sizeof(zval));
						pdo_dblib_datetime_format(zv, H->datetime_format, bigdatetime);
					}
					break;
				}
				case SQLFLT4: {
					zv = emalloc(sizeof(zval));
					ZVAL_DOUBLE(zv, *(DBFLT4 *) data);

					break;
				}
				case SQLFLT8: {
					zv = emalloc(sizeof(zval));
					ZVAL_DOUBLE(zv, *(DBFLT8 *) data);

					break;
				}
				case SQLINT8: {
					zv = emalloc(sizeof(zval));
					ZVAL_LONG(zv, *(DBBIGINT *) data);

					break;
				}
				case SQLINT4: {
					zv = emalloc(sizeof(zval));
					ZVAL_LONG(zv, *(DBINT *) data);

					break;
				}
				case SQLINT2: {
					zv = emalloc(sizeof(zval));
					ZVAL_LONG(zv, *(DBSMALLINT *) data);

					break;
				}
				case SQLINT1:
				case SQLBIT: {
					zv = emalloc(sizeof(zval));
					ZVAL_LONG(zv, *(DBTINYINT *) data);

					break;
				}
				case SQLDECIMAL:
				case SQLNUMERIC:
				case SQLMONEY:
				case SQLMONEY4:
				case SQLMONEYN: {
					DBFLT8 float_value;
					dbconvert(NULL, coltype, data, 8, SQLFLT8, (LPBYTE) &float_value, -1);

					zv = emalloc(sizeof(zval));
					ZVAL_DOUBLE(zv, float_value);

					break;
				}

				case SQLUNIQUE: {
					if (H->stringify_uniqueidentifier) {
						/* 36-char hex string representation */
						tmp_data_len = 36;
						tmp_data = safe_emalloc(tmp_data_len, sizeof(char), 1);
						data_len = dbconvert(NULL, SQLUNIQUE, data, data_len, SQLCHAR, (LPBYTE) tmp_data, tmp_data_len);
						php_strtoupper(tmp_data, data_len);
						zv = emalloc(sizeof(zval));
						ZVAL_STRINGL(zv, tmp_data, data_len);
						efree(tmp_data);

					} else {
						/* 16-byte binary representation */
						zv = emalloc(sizeof(zval));
						ZVAL_STRINGL(zv, (DBCHAR *) data, 16);
					}
					break;
				}

				default: {
					if (dbwillconvert(coltype, SQLCHAR)) {
						tmp_data_len = 32 + (2 * (data_len)); /* FIXME: We allocate more than we need here */
						tmp_data = emalloc(tmp_data_len);
						data_len = dbconvert(NULL, coltype, data, data_len, SQLCHAR, (LPBYTE) tmp_data, -1);

						zv = emalloc(sizeof(zval));
						ZVAL_STRING(zv, tmp_data);

						efree(tmp_data);
					}

					break;
				}
			}
		}
	}

	if (zv != NULL) {
		*ptr = (char*)zv;
		*len = sizeof(zval);
	} else {
		*ptr = NULL;
		*len = 0;
	}

	*caller_frees = 1;

	return 1;
}

static int pdo_dblib_stmt_get_column_meta(pdo_stmt_t *stmt, zend_long colno, zval *return_value)
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
	add_assoc_string(return_value, "column_source", dbcolsource(H->link, colno+1));
	add_assoc_string(return_value, "native_type", pdo_dblib_get_field_name(coltype));
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
	NULL, /* param hook */
	NULL, /* set attr */
	NULL, /* get attr */
	pdo_dblib_stmt_get_column_meta, /* meta */
	pdo_dblib_stmt_next_rowset, /* nextrow */
	pdo_dblib_stmt_cursor_closer
};
