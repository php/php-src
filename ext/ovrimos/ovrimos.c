/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,	  |
   | that is bundled with this package in the file LICENSE, and is	  |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to	  |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dimitris Souflis <dsouflis@acm.org>, <dsouflis@ovrimos.com> |
   |       integrated to php by Nikos Mavroyanopoulos <nmav@altera.gr>    |
   +----------------------------------------------------------------------+
*/

#include <php.h>
#include <php_globals.h>
#include <zend_API.h>
#include "ext/standard/php_standard.h"
#include "ext/standard/info.h"
#include <sqlcli.h>
#if !defined(WIN32)
# define stricmp strcasecmp
#endif
static longreadlen = 0;
static void column_to_string(SQLS stmt, int i, char *buffer, int *len);

PHP_MINFO_FUNCTION(ovrimos)
{
	php_printf("&quot;Ovrimos&quot; module<br>\n");
}

/* Main User Functions */
/* {{{ proto int ovrimos_connect(string host, string db, string user, string password)
   Connect to an Ovrimos database */
PHP_FUNCTION(ovrimos_connect)
{
	pval *arg1, *arg2, *arg3, *arg4;
	SQLH conn = 0;
	if (ARG_COUNT(ht) != 4
	    || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg1);
	convert_to_string(arg2);
	convert_to_string(arg3);
	convert_to_string(arg4);
	if (!sqlConnect
	    (arg1->value.str.val, arg2->value.str.val, arg3->value.str.val,
	     arg4->value.str.val, &conn, 0)) {
		RETURN_LONG(0);
	}
	RETURN_LONG((int) conn + 1);
}

/* }}} */

/* {{{ proto void ovrimos_close(int connection)
   Close a connection */
PHP_FUNCTION(ovrimos_close)
{
	pval *arg1;
	SQLH conn;
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE
	    || arg1->type != IS_LONG) {
		WRONG_PARAM_COUNT;
	}
	conn = (SQLH) (arg1->value.lval - 1);
	(void) sqlDisConnect(conn);
}

/* }}} */

/* {{{ proto void ovrimos_close_all()
   Close all connections */
PHP_FUNCTION(ovrimos_close_all)
{
	pval *arg1;
	SQLH conn;
	(void) sqlDisconnectAll();
}

/* }}} */

/* {{{ proto int ovrimos_longreadlen(int result_id, int length)
   Handle LONG columns */
PHP_FUNCTION(ovrimos_longreadlen)
{
	pval *arg1, *arg2;
	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	longreadlen = arg2->value.lval;
	RETURN_TRUE;
}

/* }}} */

/* {{{ proto int ovrimos_prepare(int connection_id, string query)
   Prepares a statement for execution */
PHP_FUNCTION(ovrimos_prepare)
{
	pval *arg1, *arg2;
	SQLH conn;
	char *query;
	SQLS stmt;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_string(arg2);
	conn = (SQLH) (arg1->value.lval - 1);
	query = arg2->value.str.val;

	if (!sqlAllocStmt(conn, &stmt)) {
		RETURN_FALSE;
	}
	if (!sqlPrepare(stmt, query)) {
		sqlFreeStmt(stmt);
		RETURN_FALSE;
	}
	if (!sqlGetOutputColDescr(stmt)) {
		sqlFreeStmt(stmt);
		RETURN_FALSE;
	}
	if (!sqlGetParamDescr(stmt)) {
		sqlFreeStmt(stmt);
		RETURN_FALSE;
	}
	RETURN_LONG((int) stmt + 1);
}

/* }}} */

/*
 * Execute prepared SQL statement. Supports only input parameters.
 */
/* {{{ proto int ovrimos_execute(int result_id [, array parameters_array])
   Execute a prepared statement */
PHP_FUNCTION(ovrimos_execute)
{
	pval *arg1, *arg2;
	SQLS stmt;
	int numArgs;
	int icol, colnb;
	numArgs = ARG_COUNT(ht);

	if (getParameters(ht, numArgs, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	stmt = (SQLS) (arg1->value.lval - 1);
	colnb = sqlGetParamNb(stmt);

	if (colnb != 0) {
		pval **tmp;
		int arr_elem;

		if (arg2->type != IS_ARRAY) {
			php_error(E_WARNING,
				  "Not an array in call to ovrimos_execute()");
			RETURN_FALSE;
		}
		arr_elem = zend_hash_num_elements(arg2->value.ht);
		if (arr_elem < colnb) {
			php_error(E_WARNING,
				  "Not enough parameters in call to ovrimos_execute(): %d instead of %d",
				  arr_elem, colnb);
			RETURN_FALSE;
		}

		zend_hash_internal_pointer_reset(arg2->value.ht);

		for (icol = 0; icol < colnb; icol++) {
			int len;
			cvt_error err;
			bool ret;
			char *msg;
			char buffer[10240];
			sql_type to_type = sqlGetParamSQLType(stmt, icol);
			sql_type from_type;

			if (zend_hash_get_current_data
			    (arg2->value.ht, (void **) &tmp) == FAILURE) {
				php_error(E_WARNING,
					  "Error getting parameter %d in call to ovrimos_execute()",
					  icol);
				RETURN_FALSE;
			}
			convert_to_string(*tmp);
			if ((*tmp)->type != IS_STRING) {
				php_error(E_WARNING,
					  "Error converting parameter %d to string in call to ovrimos_execute()",
					  icol);
				RETURN_FALSE;
			}

			/* PHP data to param type */
			from_type.type = T_VARCHAR;
			from_type.u.length = (*tmp)->value.str.len;

			*buffer = 0;
			memcpy(buffer + 1, (*tmp)->value.str.val,
			       from_type.u.length);
			buffer[from_type.u.length + 1] = 0;

			ret =
			    type_convert(buffer, &from_type, &to_type, 0,
					 &err);
			switch (err) {
			case cvt_trunc:
				msg = "Data truncated";
				break;
			case cvt_range:
				msg = "Numeric value out of range";
				break;
			case cvt_prec:
				msg = "Precision lost";
				break;
			case cvt_incomp:
				msg =
				    "Restricted data type attribute violation";
				break;
			case cvt_no:
				msg = "Conversion failed";
				break;
			}
			if (!ret) {
				php_error(E_WARNING,
					  "Error converting parameter %d: %s in call to ovrimos_execute()",
					  icol, msg);
				RETURN_FALSE;
			}

			len = sql_type_size(to_type) - 1;
			if (!sqlPutParam(stmt, icol, buffer + 1, len)) {
				php_error(E_WARNING,
					  "Could send parameter %d (%d bytes) in call to ovrimos_execute()",
					  icol, len);
				RETURN_FALSE;
			}
		}
	}

	if (!sqlExec(stmt)) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto string ovrimos_cursor(int result_id)
   Get cursor name */
PHP_FUNCTION(ovrimos_cursor)
{
	char cname[126];
	pval *arg1;
	SQLS stmt;

	if (getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	stmt = (SQLS) (arg1->value.lval - 1);
	if (!sqlGetCursorName(stmt, cname)) {
		RETURN_FALSE;
	}
	RETURN_STRING(cname, 1);
}

/* }}} */

/* {{{ proto int ovrimos_exec(int connection_id, string query)
   Prepare and execute an SQL statement */
PHP_FUNCTION(ovrimos_exec)
{
	pval *arg1, *arg2;
	SQLH conn;
	SQLS stmt;
	int numArgs;
	char *query;
	numArgs = ARG_COUNT(ht);
	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	conn = (SQLH) (arg1->value.lval - 1);
	query = arg2->value.str.val;

	if (!sqlAllocStmt(conn, &stmt)) {
		RETURN_FALSE;
	}
	if (!sqlExecDirect(stmt, query)) {
		sqlFreeStmt(stmt);
		RETURN_FALSE;
	}
	if (!sqlGetOutputColDescr(stmt)) {
		sqlFreeStmt(stmt);
		RETURN_FALSE;
	}
	if (!sqlGetParamDescr(stmt)) {
		sqlFreeStmt(stmt);
		RETURN_FALSE;
	}
	RETURN_LONG((int) stmt + 1);
}

/* }}} */

static void column_to_string(SQLS stmt, int i, char *buffer, int *len)
{
	const char *bf = sqlColValue(stmt, i, 0);
	switch (sqlGetOutputColType(stmt, i)) {
	case T_BIGINT:
	case T_UBIGINT:{
			switch (sqlGetOutputColType(stmt, i)) {
			case T_BIGINT:
				printsint64(Read(sint64, bf), buffer);
				break;
			case T_UBIGINT:
				printuint64(Read(uint64, bf), buffer);
				break;
			}
			*len = strlen(buffer);
		}
		break;
	case T_INTEGER:
		sprintf(buffer, "%11ld", Read(sint32, bf));
		*len = strlen(buffer);
		break;
	case T_UINTEGER:
		sprintf(buffer, "%10lu", Read(uint32, bf));
		*len = strlen(buffer);
		break;
	case T_SMALLINT:
		sprintf(buffer, "%6hd", Read(sint16, bf));
		*len = strlen(buffer);
		break;
	case T_USMALLINT:
		sprintf(buffer, "%5hu", Read(uint16, bf));
		*len = strlen(buffer);
		break;
	case T_TINYINT:
		sprintf(buffer, "%4hd", (sint16) Read(sint8, bf));
		*len = strlen(buffer);
		break;
	case T_UTINYINT:
		sprintf(buffer, "%3hu", (uint16) Read(uint8, bf));
		*len = strlen(buffer);
		break;
	case T_BIT:
		sprintf(buffer, "%s",
			(Read(uint8, bf) == 0) ? "off" : "on");
		*len = strlen(buffer);
		break;
	case T_REAL:
		sprintf(buffer, "%9.7g", (double) Read(float, bf));
		*len = strlen(buffer);
		break;

	case T_FLOAT:
	case T_DOUBLE:
		sprintf(buffer, "%19.17g", Read(double, bf));
		*len = strlen(buffer);
		break;
	case T_DECIMAL:
	case T_NUMERIC:{
			int prec = sqlGetOutputColPrecision(stmt, i);
			int scale = sqlGetOutputColScale(stmt, i);
			sprintf(buffer, "%*.*f", prec + 2, scale,
				Read(double, bf));
			*len = strlen(buffer);
		} break;
	case T_CHAR:
	case T_VARCHAR:
		strcpy(buffer, bf);
		*len = strlen(buffer);
		break;
	case T_UNI_CHAR:
	case T_UNI_VARCHAR:
		uni_strcpy((uni_char *) buffer, (uni_char *) bf);
		*len = uni_strlen((uni_char *) buffer);
		break;
	case T_BINARY:{
			int sz = sqlGetOutputColLength(stmt, i);
			memcpy(buffer, bf, sz);
			*len = sz;
		} break;

	case T_VARBINARY:{
			int sz = Read(uint16, bf);
			memcpy(buffer, bf + 2, sz);
			*len = sz;
		} break;

	case T_DATE:{
			char db[11];
			if (!sql_date_to_str((uint32 *) bf, buffer)) {
				strcpy(buffer, "Error!");
			}
		}
		break;

	case T_TIME:{
			int prec = sqlGetOutputColPrecision(stmt, i);
			if (!sql_time_to_str
			    ((uint32 *) bf, prec, 0, buffer)) {
				strcpy(buffer, "Error!");
			}
		}
		break;

	case T_TIMESTAMP:{
			int prec = sqlGetOutputColPrecision(stmt, i);
			if (!sql_timestamp_to_str
			    ((uint32 *) bf, prec, 0, buffer)) {
				strcpy(buffer, "Error!");
			}

		}
		break;

	case T_LONGVARCHAR:
	case T_LONGVARBINARY:{
			if (longreadlen == 0) {
				*buffer = 0;
				*len = 0;
			} else
			    if (!sqlColValueLong
				(stmt, i, 0, 0, longreadlen, buffer, len)) {
				strcpy(buffer, "Error!");
			}
		}
		break;
	}
}

/* {{{ proto int ovrimos_fetch_into(int result_id, array result_array [, string how, [int rownumber]])
   Fetch one result row into an array
   how: 'Next' (default), 'Prev', 'First', 'Last', 'Absolute'
   */
PHP_FUNCTION(ovrimos_fetch_into)
{
	int numArgs;
	char *s_how;
	typedef enum { h_next = 0, h_prev, h_first, h_last, h_absolute
	} h_type;
	h_type how = h_next;	/* default */
	sint32 rownum = 0;
	pval *arg_id, *arg_how = 0, *arg_row = 0, *arr, *tmp;
	SQLS stmt;
	int icol, colnb;
	bool ret;
	numArgs = ARG_COUNT(ht);

	switch (numArgs) {
	case 2:
		if (getParameters(ht, 2, &arg_id, &arr) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	case 3:
		if (getParameters(ht, 3, &arg_id, &arr, &arg_how) ==
		    FAILURE) WRONG_PARAM_COUNT;
		break;
	case 4:
		if (getParameters(ht, 4, &arg_id, &arr, &arg_how, &arg_row)
		    == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg_id);
	stmt = (SQLS) (arg_id->value.lval - 1);

	if (arg_how != 0) {
		if (arg_how->type != IS_STRING) {
			php_error(E_WARNING,
				  "Third argument not string in ovrimos_fetch_into()");
			RETURN_FALSE;
		}
		s_how = arg_how->value.str.val;
		if (stricmp(s_how, "next") == 0) {
			how = h_next;
		} else if (stricmp(s_how, "prev") == 0) {
			how = h_prev;
		} else if (stricmp(s_how, "first") == 0) {
			how = h_first;
		} else if (stricmp(s_how, "last") == 0) {
			how = h_last;
		} else if (stricmp(s_how, "absolute") == 0) {
			how = h_absolute;
		} else {
			php_error(E_WARNING,
				  "Third argument not valid in ovrimos_fetch_into()");
			RETURN_FALSE;
		}
		if (arg_row == 0 && how == h_absolute) {
			php_error(E_WARNING,
				  "Fourth argument is required for ABSOLUTE in ovrimos_fetch_into()");
			RETURN_FALSE;
		}
		if (arg_row != 0) {
			convert_to_long(arg_row);
			rownum = arg_row->value.lval;
			switch (how) {
			case h_next:
			case h_prev:
				rownum--;	/* Next 1 should send FUNC_CURSOR_NEXT(0) */
				break;
			}
		}
	}

	if (!ParameterPassedByReference(ht, 2)) {	/* 1-based?... */
		php_error(E_WARNING,
			  "Array not passed by reference in call to ovrimos_fetch_into()");
		RETURN_FALSE;
	}

	if (arr->type != IS_ARRAY) {
		if (array_init(arr) == FAILURE) {
			php_error(E_WARNING,
				  "Can't convert to type Array");
			RETURN_FALSE;
		}
	}
	switch (how) {
	case h_absolute:
	case h_first:
		ret = sqlCursorFirst(stmt, rownum);
		break;
	case h_last:
		ret = sqlCursorLast(stmt, rownum);
		break;
	case h_next:
		ret = sqlCursorNext(stmt, rownum);
		break;
	case h_prev:
		ret = sqlCursorPrev(stmt, rownum);
		break;
	}
	if (!ret) {
		RETURN_FALSE;
	}
	colnb = sqlGetOutputColNb(stmt);
	for (icol = 0; icol < colnb; icol++) {
		int len;
		char buffer[10240];
		tmp = (pval *) emalloc(sizeof(pval));
		tmp->refcount = 1;
		tmp->type = IS_STRING;
		tmp->value.str.len = 0;

		/* Produce column value in 'tmp' ... */
		column_to_string(stmt, icol, buffer, &len);
		tmp->value.str.len = len;
		tmp->value.str.val = estrndup(buffer, len);

		zend_hash_index_update(arr->value.ht, icol, &tmp,
				       sizeof(pval *), NULL);
	}
	RETURN_TRUE;
}

/* }}} */

/* {{{ proto int ovrimos_fetch_row(int result_id [, int how, [int row_number]])
   how: 'Next' (default), 'Prev', 'First', 'Last', 'Absolute'
   Fetch a row */
PHP_FUNCTION(ovrimos_fetch_row)
{
	int numArgs, i;
	char *s_how;
	typedef enum { h_next = 0, h_prev, h_first, h_last, h_absolute
	} h_type;
	h_type how = h_next;	/* default */
	sint32 rownum = 0;
	pval *arg_id, *arg_how = 0, *arg_row = 0;
	SQLS stmt;
	bool ret;
	numArgs = ARG_COUNT(ht);

	switch (numArgs) {
	case 1:
		if (getParameters(ht, 1, &arg_id) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	case 2:
		if (getParameters(ht, 2, &arg_id, &arg_how) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	case 3:
		if (getParameters(ht, 3, &arg_id, &arg_how, &arg_row) ==
		    FAILURE) WRONG_PARAM_COUNT;
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg_id);
	stmt = (SQLS) (arg_id->value.lval - 1);

	if (arg_how != 0) {
		if (arg_how->type != IS_STRING) {
			php_error(E_WARNING,
				  "Second argument not string in ovrimos_fetch_row()");
			RETURN_FALSE;
		}
		s_how = arg_how->value.str.val;
		if (stricmp(s_how, "next") == 0) {
			how = h_next;
		} else if (stricmp(s_how, "prev") == 0) {
			how = h_prev;
		} else if (stricmp(s_how, "first") == 0) {
			how = h_first;
		} else if (stricmp(s_how, "last") == 0) {
			how = h_last;
		} else if (stricmp(s_how, "absolute") == 0) {
			how = h_absolute;
		} else {
			php_error(E_WARNING,
				  "Second argument not valid in ovrimos_fetch_row()");
			RETURN_FALSE;
		}
		if (arg_row == 0 && how == 4) {
			php_error(E_WARNING,
				  "Third argument is required for ABSOLUTE in ovrimos_fetch_row()");
			RETURN_FALSE;
		}
		if (arg_row != 0) {
			convert_to_long(arg_row);
			rownum = arg_row->value.lval;
			switch (how) {
			case h_next:
			case h_prev:
				rownum--;	/* Next 1 should send FUNC_CURSOR_NEXT(0) */
				break;
			}
		}
	}

	switch (how) {
	case h_absolute:
	case h_first:
		ret = sqlCursorFirst(stmt, rownum);
		break;
	case h_last:
		ret = sqlCursorLast(stmt, rownum);
		break;
	case h_next:
		ret = sqlCursorNext(stmt, rownum);
		break;
	case h_prev:
		ret = sqlCursorPrev(stmt, rownum);
		break;
	}
	if (!ret) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

/* }}} */

/* {{{ proto string ovrimos_result(int result_id, mixed field)
   Get result data */
PHP_FUNCTION(ovrimos_result)
{
	int numArgs = ARG_COUNT(ht);
	pval *arg_id, *arg_field;
	int icol, colnb;
	SQLS stmt;
	int len;
	char buffer[1024];

	if (numArgs != 2
	    || getParameters(ht, 2, &arg_id,
			     &arg_field) == FAILURE) WRONG_PARAM_COUNT;

	convert_to_long(arg_id);
	stmt = (SQLS) (arg_id->value.lval - 1);

	colnb = sqlGetOutputColNb(stmt);

	if (arg_field->type == IS_STRING) {
		int i;
		for (i = 0; i < colnb; i++) {
			if (!stricmp
			    (arg_field->value.str.val,
			     sqlGetOutputColName(stmt, i))) {
				icol = i;
				break;
			}
		}
	} else if (arg_field->type == IS_LONG) {
		icol = arg_field->value.lval - 1;
	} else {
		php_error(E_WARNING,
			  "Second argument neither number nor string in ovrimos_result()");
		RETURN_FALSE;
	}
	if (icol < 0 || icol > colnb) {
		php_error(E_WARNING, "Unknown column in ovrimos_result()");
		RETURN_FALSE;
	}
	column_to_string(stmt, icol, buffer, &len);

	RETURN_STRINGL(buffer, len, 1);
}

/* }}} */

/* {{{ proto int ovrimos_result_all(int result_id [, string format])
   Print result as HTML table */
PHP_FUNCTION(ovrimos_result_all)
{
	long fetched = 0;
	pval *arg1, *arg2;
	int numArgs;
	SQLS stmt;
	int icol, colnb;
	char buffer[1024];
	int len;

	numArgs = ARG_COUNT(ht);
	if (numArgs == 1) {
		if (getParameters(ht, 1, &arg1) == FAILURE)
			WRONG_PARAM_COUNT;
	} else {
		if (getParameters(ht, 2, &arg1, &arg2) == FAILURE)
			WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	stmt = (SQLS) (arg1->value.lval - 1);

	colnb = sqlGetOutputColNb(stmt);

	/* Start table tag */
	if (numArgs == 1) {
		php_printf("<table><tr>");
	} else {
		convert_to_string(arg2);
		php_printf("<table %s ><tr>", arg2->value.str.val);
	}

	for (icol = 0; icol < colnb; icol++) {
		php_printf("<th>%s</th>", sqlGetOutputColName(stmt, icol));
	}

	php_printf("</tr>\n");

	if (sqlCursorFirst(stmt, 0)) {
		do {
			fetched++;
			php_printf("<tr>");
			for (icol = 0; icol < colnb; icol++) {
				column_to_string(stmt, icol, buffer, &len);
				php_printf("<td>%s</td>", buffer);
			}
			php_printf("</tr>\n");
		} while (sqlCursorNext(stmt, 0));
	}
	php_printf("</table>\n");

	RETURN_LONG(fetched);
}

/* }}} */

/* {{{ proto int ovrimos_free_result(int result_id)
   Free resources associated with a result */
PHP_FUNCTION(ovrimos_free_result)
{
	pval *arg1;
	SQLS stmt;

	if (getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	stmt = (SQLS) (arg1->value.lval - 1);
	sqlCloseCursor(stmt);
	RETURN_TRUE;
}

/* }}} */

/* {{{ proto int ovrimos_num_rows(int result_id)
   Get number of rows in a result */
PHP_FUNCTION(ovrimos_num_rows)
{
	uint32 rows;
	pval *arg1;
	SQLS stmt;

	if (getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	stmt = (SQLS) (arg1->value.lval - 1);

	sqlGetRowCount(stmt, &rows);
	RETURN_LONG(rows);
}

/* }}} */

/* {{{ proto int ovrimos_num_fields(int result_id)
   Get number of columns in a result */
PHP_FUNCTION(ovrimos_num_fields)
{
	pval *arg1;
	SQLS stmt;

	if (getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	stmt = (SQLS) (arg1->value.lval - 1);

	RETURN_LONG(sqlGetOutputColNb(stmt));
}

/* }}} */

/* {{{ proto string ovrimos_field_name(int result_id, int field_number)
   Get a column name */
PHP_FUNCTION(ovrimos_field_name)
{
	pval *arg1, *arg2;
	SQLS stmt;
	int field;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	stmt = (SQLS) (arg1->value.lval - 1);

	if (arg2->value.lval < 1) {
		php_error(E_WARNING,
			  "Field numbering starts at 1! in call to ovrimos_field_name()");
		RETURN_FALSE;
	}

	field = arg2->value.lval - 1;

	if (field >= sqlGetOutputColNb(stmt)) {
		php_error(E_WARNING,
			  "No field at this index (%d) in call to ovrimos_field_name()",
			  field);
		RETURN_FALSE;
	}

	RETURN_STRING((char *) sqlGetOutputColName(stmt, field), 1);
}

/* }}} */

/* {{{ proto string ovrimos_field_type(int result_id, int field_number)
   Get the datatype of a column */
PHP_FUNCTION(ovrimos_field_type)
{
	pval *arg1, *arg2;
	SQLS stmt;
	int field;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	stmt = (SQLS) (arg1->value.lval - 1);

	if (arg2->value.lval < 1) {
		php_error(E_WARNING,
			  "Field numbering starts at 1! in call to ovrimos_field_type()");
		RETURN_FALSE;
	}

	field = arg2->value.lval - 1;

	if (field >= sqlGetOutputColNb(stmt)) {
		php_error(E_WARNING,
			  "No field at this index (%d) in call to ovrimos_field_type()",
			  field);
		RETURN_FALSE;
	}

	RETURN_LONG(sqlGetOutputColType(stmt, field));
}

/* }}} */

/* {{{ proto int ovrimos_field_len(int result_id, int field_number)
   Get the length of a column */
PHP_FUNCTION(ovrimos_field_len)
{
	pval *arg1, *arg2;
	SQLS stmt;
	int field;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	stmt = (SQLS) (arg1->value.lval - 1);

	if (arg2->value.lval < 1) {
		php_error(E_WARNING,
			  "Field numbering starts at 1! in call to ovrimos_field_len()");
		RETURN_FALSE;
	}

	field = arg2->value.lval - 1;

	if (field >= sqlGetOutputColNb(stmt)) {
		php_error(E_WARNING,
			  "No field at this index (%d) in call to ovrimos_field_len()",
			  field);
		RETURN_FALSE;
	}

	switch (sqlGetOutputColType(stmt, field)) {
	case T_LONGVARCHAR:
	case T_LONGVARBINARY:
		RETURN_LONG(longreadlen);
	default:
		RETURN_LONG(sqlGetOutputColLength(stmt, field));
	}
}

/* }}} */

/* {{{ proto int ovrimos_field_num(int result_id, string field_name)
   Return column number */
PHP_FUNCTION(ovrimos_field_num)
{
	pval *arg1, *arg2;
	SQLS stmt;
	int i, n;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE
	    || arg2->type != IS_STRING) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	stmt = (SQLS) (arg1->value.lval - 1);
	n = sqlGetOutputColNb(stmt);
	for (i = 0; i < n; i++) {
		if (!strcmp
		    (arg2->value.str.val, sqlGetOutputColName(stmt, i))) {
			RETURN_LONG(i + 1);
		}
	}
	RETURN_FALSE;
}

/* }}} */

#if 0
/* {{{ proto int ovrimos_autocommit(int connection_id, int OnOff)
   Toggle autocommit mode
   There can be problems with pconnections!*/
PHP_FUNCTION(ovrimos_autocommit)
{
}

/* }}} */
#endif

/* {{{ proto int ovrimos_commit(int connection_id)
   Commit an ovrimos transaction */
PHP_FUNCTION(ovrimos_commit)
{
	pval *arg1;
	SQLH conn;
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE
	    || arg1->type != IS_LONG) {
		WRONG_PARAM_COUNT;
	}
	conn = (SQLH) (arg1->value.lval - 1);
	if (!sqlTransactCommit(conn)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

/* }}} */

/* {{{ proto int ovrimos_rollback(int connection_id)
   Rollback a transaction */
PHP_FUNCTION(ovrimos_rollback)
{
	pval *arg1;
	SQLH conn;
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE
	    || arg1->type != IS_LONG) {
		WRONG_PARAM_COUNT;
	}
	conn = (SQLH) (arg1->value.lval - 1);
	if (!sqlTransactRollback(conn)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

/* }}} */

#if 0
/* {{{ proto int ovrimos_setoption(int conn_id|result_id, int which, int option, int value)
   Sets connection or statement options */
PHP_FUNCTION(ovrimos_setoption)
{
}

/* }}} */
#endif

function_entry ovrimos_functions[] = {
/*     PHP_FE(ovrimos_setoption, NULL)*/
/*     PHP_FE(ovrimos_autocommit, NULL)*/
	PHP_FE(ovrimos_close, NULL)
	    PHP_FE(ovrimos_close_all, NULL)
	    PHP_FE(ovrimos_commit, NULL)
	    PHP_FE(ovrimos_connect, NULL)
	    PHP_FE(ovrimos_cursor, NULL)
	    PHP_FE(ovrimos_exec, NULL)
	    PHP_FE(ovrimos_prepare, NULL)
	    PHP_FE(ovrimos_execute, NULL)
	    PHP_FE(ovrimos_fetch_row, NULL)
	    PHP_FE(ovrimos_fetch_into, NULL)
	    PHP_FE(ovrimos_field_len, NULL)
	    PHP_FE(ovrimos_field_name, NULL)
	    PHP_FE(ovrimos_field_type, NULL)
	    PHP_FE(ovrimos_field_num, NULL)
	    PHP_FE(ovrimos_free_result, NULL)
	    PHP_FE(ovrimos_num_fields, NULL)
	    PHP_FE(ovrimos_num_rows, NULL)
	    PHP_FE(ovrimos_result, NULL)
	    PHP_FE(ovrimos_result_all, NULL)
	    PHP_FE(ovrimos_rollback, NULL)
/*     PHP_FE(ovrimos_binmode, NULL)*/
	    PHP_FE(ovrimos_longreadlen, NULL)
	PHP_FALIAS(ovrimos_do, ovrimos_exec, NULL) {NULL, NULL, NULL}
};

zend_module_entry ovrimos_module_entry = {
	"Ovrimos",
	ovrimos_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(ovrimos),
	STANDARD_MODULE_PROPERTIES
};

DLEXPORT zend_module_entry *get_module()
{
	return &ovrimos_module_entry;
};
