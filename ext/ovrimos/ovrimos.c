/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,	    |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dimitris Souflis, Nikos Mavroyanopoulos                     |
   |          for Ovrimos S.A.                                            |
   |                                                                      |
   | Contact support@ovrimos.com for questions regarding this module      |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include <php.h>
#include <php_globals.h>
#include <zend_API.h>
#include "ext/standard/php_standard.h"
#include "ext/standard/info.h"
#include <sqlcli.h> /* ovrimos header 
                     */

#ifndef WIN32 /* stricmp is defined in sqlcli */
# define stricmp strcasecmp
#endif


PHP_MINFO_FUNCTION(ovrimos)
{
	php_printf("&quot;Ovrimos&quot; module<br />\n");
}

/* Main User Functions 
 */
 
/* ovrimos_connect() currently does not support secure (SSL/TLS) connections.
 * As an alternative you can use the unixODBC driver available at 
 * http://www.ovrimos.com/download which supports SSL.
 * Contact support@ovrimos.com for more information.
 */

/* 2001-07-27: ovrimos_close_all() function was removed in order 
 * for this module to be reentrant.
 */


/* structures introduced in order to support the old ovrimos-php-api with 
 * the new multi-threaded library (old works with the old library). 
 * This version is reentrant.
 *
 * The only limitation is that a connection ( as returned by ovrimos_connect()) 
 * may only be accessed by one thread (but this is the case in php now).
 */

typedef struct {
	SQLS statement;
	int longreadlen;
	struct _CON_STATE* con_state;
} STATEMENT;

typedef struct _CON_STATE {
	SQLH connection;
	STATEMENT * statements;
	int nstatements;
} CON_STATE;

typedef STATEMENT* PSTATEMENT;
typedef CON_STATE* PCON_STATE;

static void column_to_string(SQLS stmt, int i, char *buffer, int *len, PSTATEMENT pstmt);

/* {{{ proto int ovrimos_connect(string host, string db, string user, string password)
   Connect to an Ovrimos database */
PHP_FUNCTION(ovrimos_connect)
{
	pval *arg1, *arg2, *arg3, *arg4;
	PCON_STATE state;
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
	    (Z_STRVAL_P(arg1), Z_STRVAL_P(arg2), Z_STRVAL_P(arg3),
	     Z_STRVAL_P(arg4), &conn, 0)) {
		RETURN_LONG(0);
	}
	
	state = ecalloc( 1, sizeof(CON_STATE));
	if (state==NULL) RETURN_FALSE;

	state->connection = conn;
	state->statements = NULL;
	state->nstatements = 0;

	RETURN_LONG( (long)state);
}

/* }}} */

/* {{{ proto void ovrimos_close(int connection)
   Close a connection */
PHP_FUNCTION(ovrimos_close)
{
	pval *arg1;
	int i;
	PCON_STATE state;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE
	    || Z_TYPE_P(arg1) != IS_LONG) {
		WRONG_PARAM_COUNT;
	}

	state = (PCON_STATE) Z_LVAL_P(arg1);

	/* free all the statements associated with 
	 * the connection. (called results in php)
	 */

	for (i=0;i < state->nstatements;i++) {
		if ( state->statements[i].statement!=NULL) {
			sqlFreeStmt( state->statements[i].statement);
		}
	}
	if (state->statements!=NULL)
		efree( state->statements);

	/* close the SQL_Handle
	 */
	sqlDisconnect( state->connection);

	efree( state);
	
	return;
}

/* }}} */


/* {{{ proto int ovrimos_longreadlen(int result_id, int length)
   Handle LONG columns */
PHP_FUNCTION(ovrimos_longreadlen)
{
	pval *arg1, *arg2;
	PSTATEMENT stmt;
	
	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);

	stmt = (PSTATEMENT) Z_LVAL_P(arg1);

	stmt->longreadlen = Z_LVAL_P(arg2);
	RETURN_TRUE;
}

/* }}} */

#define DEFAULT_LONGREADLEN 0

/* These two functions are quite expensive. Some optimization may be
 * done in a later version.
 */
static int local_sqlAllocStmt( PCON_STATE state, SQLH conn, SQLS *stmt, PSTATEMENT* pstmt) {
int index, ret;
	
	ret = sqlAllocStmt( conn, stmt);
	if (!ret) return ret;

	state->nstatements++;
	state->statements = erealloc( state->statements,
		state->nstatements*sizeof( STATEMENT));

	if (state->statements==NULL) return 0;

	index = state->nstatements - 1;
	state->statements[ index].statement = (*stmt);
	state->statements[ index].longreadlen = DEFAULT_LONGREADLEN;
	state->statements[ index].con_state = state;

	*pstmt = &state->statements[ index];
		
	return 1;
}

static int local_sqlFreeStmt( PSTATEMENT statement, SQLS stmt) {
int j, i;
PSTATEMENT new_statements;
PCON_STATE state = statement->con_state;

	sqlFreeStmt( stmt);

	if (state->nstatements-1 == 0) {
		efree( state->statements);
		state->statements = NULL;
		state->nstatements--;
		
		return 1;
	}
	
	new_statements = emalloc( (state->nstatements-1) * sizeof(STATEMENT));
	if (new_statements==NULL) return 0;
	
	for (i=j=0;i<state->nstatements;i++) {
		if (state->statements->statement != stmt) {
			new_statements[j].statement = state->statements[i].statement;
			new_statements[j].longreadlen = state->statements[i].longreadlen;
			new_statements[j++].con_state = state->statements[i].con_state;
		}
	}

	efree( state->statements);
	state->statements = new_statements;
	state->nstatements--;

	return 1; /* true */
}

/* {{{ proto int ovrimos_prepare(int connection_id, string query)
   Prepares a statement for execution */
PHP_FUNCTION(ovrimos_prepare)
{
	pval *arg1, *arg2;
	SQLH conn;
	char *query;
	SQLS stmt;
	PCON_STATE state;
	PSTATEMENT pstmt;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_string(arg2);
	
	state = (PCON_STATE) Z_LVAL_P(arg1);

	conn = (SQLH) state->connection;
	query = Z_STRVAL_P(arg2);

	if (!local_sqlAllocStmt( state, conn, &stmt, &pstmt)) {
		RETURN_FALSE;
	}
	if (!sqlPrepare(stmt, query)) {
		local_sqlFreeStmt( pstmt, stmt);
		RETURN_FALSE;
	}
	if (!sqlGetOutputColDescr(stmt)) {
		local_sqlFreeStmt( pstmt, stmt);
		RETURN_FALSE;
	}
	if (!sqlGetParamDescr(stmt)) {
		local_sqlFreeStmt( pstmt, stmt);
		RETURN_FALSE;
	}

	/* returns a result id which is actually a
	 * pointer to a STATEMENT structure;
	 */
	RETURN_LONG( (long)pstmt);
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
	PSTATEMENT pstmt;

	numArgs = ARG_COUNT(ht);
	
	if (getParameters(ht, numArgs, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	pstmt = (PSTATEMENT) Z_LVAL_P(arg1);

	stmt = pstmt->statement;
	colnb = sqlGetParamNb(stmt);

	if (colnb != 0) {
		pval **tmp;
		int arr_elem;

		if (Z_TYPE_P(arg2) != IS_ARRAY) {
			php_error(E_WARNING,
				  "Not an array in call to ovrimos_execute()");
			RETURN_FALSE;
		}
		arr_elem = zend_hash_num_elements(Z_ARRVAL_P(arg2));
		if (arr_elem < colnb) {
			php_error(E_WARNING,
				  "Not enough parameters in call to ovrimos_execute(): %d instead of %d",
				  arr_elem, colnb);
			RETURN_FALSE;
		}

		zend_hash_internal_pointer_reset(Z_ARRVAL_P(arg2));

		for (icol = 0; icol < colnb; icol++) {
			int len;
			cvt_error err;
			bool ret;
			char *msg;
			char buffer[10240];
			sql_type to_type = sqlGetParamSQLType(stmt, icol);
			sql_type from_type;

			if (zend_hash_get_current_data
			    (Z_ARRVAL_P(arg2), (void **) &tmp) == FAILURE) {
				php_error(E_WARNING,
					  "Error getting parameter %d in call to ovrimos_execute()",
					  icol);
				RETURN_FALSE;
			}
			convert_to_string(*tmp);
			if (Z_TYPE_PP(tmp) != IS_STRING) {
				php_error(E_WARNING,
					  "Error converting parameter %d to string in call to ovrimos_execute()",
					  icol);
				RETURN_FALSE;
			}

			/* PHP data to param type */
			Z_TYPE(from_type) = T_VARCHAR;
			from_type.u.length = Z_STRLEN_PP(tmp);

			*buffer = 0;
			memcpy(buffer + 1, Z_STRVAL_PP(tmp),
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
			default:
				msg = "Unknown error";
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
	PSTATEMENT pstmt;
	
	if (getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);

	pstmt = (PSTATEMENT) Z_LVAL_P(arg1);
	stmt = pstmt->statement;

	if (!sqlGetCursorName(stmt, cname)) {
		RETURN_FALSE;
	}
	RETURN_STRING(cname, 1);
}

/* }}} */

/* This function returns a result id. The result ID is
 * a pointer to a STATEMENT structure.
 * Every result is mapped to a statement.
 */
 
/* {{{ proto int ovrimos_exec(int connection_id, string query)
   Prepare and execute an SQL statement */
PHP_FUNCTION(ovrimos_exec)
{
	pval *arg1, *arg2;
	SQLH conn;
	SQLS stmt;
	int numArgs;
	char *query;
	PSTATEMENT pstmt;
	PCON_STATE state;
	
	numArgs = ARG_COUNT(ht);
	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);

	state = (PCON_STATE) Z_LVAL_P(arg1);
	conn = state->connection;
	query = Z_STRVAL_P(arg2);

	if (!local_sqlAllocStmt( state, conn, &stmt, &pstmt)) {
		RETURN_FALSE;
	}

	if (!sqlExecDirect(stmt, query)) {
		local_sqlFreeStmt( pstmt, stmt);
		RETURN_FALSE;
	}
	if (!sqlGetOutputColDescr(stmt)) {
		local_sqlFreeStmt( pstmt, stmt);
		RETURN_FALSE;
	}
	if (!sqlGetParamDescr(stmt)) {
		local_sqlFreeStmt( pstmt, stmt);
		RETURN_FALSE;
	}

	RETURN_LONG( (long)pstmt);
}

/* }}} */

/* {{{ column_to_string
 */
static void column_to_string(SQLS stmt, int i, char *buffer, int *len, PSTATEMENT pstmt)
{
	const char *bf = sqlColValue(stmt, i, 0);
	int longreadlen;
	
	longreadlen = pstmt->longreadlen;
	
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
		sprintf(buffer, "%11d", Read(sint32, bf));
		*len = strlen(buffer);
		break;
	case T_UINTEGER:
		sprintf(buffer, "%10u", Read(uint32, bf));
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
/* }}} */

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
	PSTATEMENT pstmt;
	int icol, colnb;
	bool ret=0;
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
	pstmt = (PSTATEMENT) Z_LVAL_P(arg_id);

	stmt = pstmt->statement;

	if (arg_how != 0) {
		if (Z_TYPE_P(arg_how) != IS_STRING) {
			php_error(E_WARNING,
				  "Third argument not string in ovrimos_fetch_into()");
			RETURN_FALSE;
		}
		s_how = Z_STRVAL_P(arg_how);
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
			rownum = Z_LVAL_P(arg_row);
			switch (how) {
			case h_next:
			case h_prev:
				rownum--;	/* Next 1 should send FUNC_CURSOR_NEXT(0) */
				break;
			default:
				break;
			}
		}
	}

	if (Z_TYPE_P(arr) != IS_ARRAY) {
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
		Z_TYPE_P(tmp) = IS_STRING;
		Z_STRLEN_P(tmp) = 0;

		/* Produce column value in 'tmp' ... */

		column_to_string(stmt, icol, buffer, &len, pstmt);
		Z_STRLEN_P(tmp) = len;
		Z_STRVAL_P(tmp) = estrndup(buffer, len);

		zend_hash_index_update(Z_ARRVAL_P(arr), icol, &tmp,
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
	int numArgs;
	char *s_how;
	typedef enum { h_next = 0, h_prev, h_first, h_last, h_absolute
	} h_type;
	h_type how = h_next;	/* default */
	sint32 rownum = 0;
	pval *arg_id, *arg_how = 0, *arg_row = 0;
	SQLS stmt;
	PSTATEMENT pstmt;
	bool ret = 0;
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

	pstmt = (PSTATEMENT) Z_LVAL_P(arg_id);
	stmt = (SQLS) pstmt->statement;

	if (arg_how != 0) {
		if (Z_TYPE_P(arg_how) != IS_STRING) {
			php_error(E_WARNING,
				  "Second argument not string in ovrimos_fetch_row()");
			RETURN_FALSE;
		}
		s_how = Z_STRVAL_P(arg_how);
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
			rownum = Z_LVAL_P(arg_row);
			switch (how) {
			case h_next:
			case h_prev:
				rownum--;	/* Next 1 should send FUNC_CURSOR_NEXT(0) */
				break;
			default:
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
	int icol=0, colnb;
	SQLS stmt;
	int len;
	PSTATEMENT pstmt;
	char buffer[1024];

	if (numArgs != 2
	    || getParameters(ht, 2, &arg_id,
			     &arg_field) == FAILURE) WRONG_PARAM_COUNT;

	convert_to_long(arg_id);
	pstmt = (PSTATEMENT) Z_LVAL_P(arg_id);
	stmt = (SQLS) pstmt->statement;

	colnb = sqlGetOutputColNb(stmt);

	if (Z_TYPE_P(arg_field) == IS_STRING) {
		int i;
		for (i = 0; i < colnb; i++) {
			if (!stricmp
			    (Z_STRVAL_P(arg_field),
			     sqlGetOutputColName(stmt, i))) {
				icol = i;
				break;
			}
		}
	} else if (Z_TYPE_P(arg_field) == IS_LONG) {
		icol = Z_LVAL_P(arg_field) - 1;
	} else {
		php_error(E_WARNING,
			  "Second argument neither number nor string in ovrimos_result()");
		RETURN_FALSE;
	}
	if (icol < 0 || icol > colnb) {
		php_error(E_WARNING, "Unknown column in ovrimos_result()");
		RETURN_FALSE;
	}
	column_to_string(stmt, icol, buffer, &len, pstmt);

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
	PSTATEMENT pstmt;
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
	pstmt = (PSTATEMENT) Z_LVAL_P(arg1);
	stmt = (SQLS) pstmt->statement;

	colnb = sqlGetOutputColNb(stmt);

	/* Start table tag */
	if (numArgs == 1) {
		php_printf("<table><tr>");
	} else {
		convert_to_string(arg2);
		php_printf("<table %s ><tr>", Z_STRVAL_P(arg2));
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
				column_to_string(stmt, icol, buffer, &len, pstmt);
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
	PSTATEMENT pstmt;
	
	if (getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	pstmt = (PSTATEMENT) Z_LVAL_P(arg1);
	stmt = (SQLS) pstmt->statement;

	sqlCloseCursor( stmt);
	local_sqlFreeStmt( pstmt, stmt);
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
	PSTATEMENT pstmt;

	if (getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	pstmt = (PSTATEMENT) Z_LVAL_P(arg1);
	stmt = (SQLS) pstmt->statement;


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
	PSTATEMENT pstmt;

	if (getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	pstmt = (PSTATEMENT) Z_LVAL_P(arg1);

	stmt = (SQLS) pstmt->statement;

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
	PSTATEMENT pstmt;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	pstmt = (PSTATEMENT) Z_LVAL_P(arg1);

	stmt = (SQLS) pstmt->statement;


	if (Z_LVAL_P(arg2) < 1) {
		php_error(E_WARNING,
			  "Field numbering starts at 1! in call to ovrimos_field_name()");
		RETURN_FALSE;
	}

	field = Z_LVAL_P(arg2) - 1;

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
	PSTATEMENT pstmt;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	pstmt = (PSTATEMENT) Z_LVAL_P(arg1);

	stmt = (SQLS) pstmt->statement;

	if (Z_LVAL_P(arg2) < 1) {
		php_error(E_WARNING,
			  "Field numbering starts at 1! in call to ovrimos_field_type()");
		RETURN_FALSE;
	}

	field = Z_LVAL_P(arg2) - 1;

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
	PSTATEMENT pstmt;
	int longreadlen;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	pstmt = (PSTATEMENT) Z_LVAL_P(arg1);

	longreadlen = pstmt->longreadlen;

	stmt = (SQLS) pstmt->statement;

	if (Z_LVAL_P(arg2) < 1) {
		php_error(E_WARNING,
			  "Field numbering starts at 1! in call to ovrimos_field_len()");
		RETURN_FALSE;
	}

	field = Z_LVAL_P(arg2) - 1;

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
	PSTATEMENT pstmt;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE
	    || Z_TYPE_P(arg2) != IS_STRING) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	pstmt = (PSTATEMENT) Z_LVAL_P(arg1);
	stmt = (SQLS) pstmt->statement;

	n = sqlGetOutputColNb(stmt);
	for (i = 0; i < n; i++) {
		if (!strcmp
		    (Z_STRVAL_P(arg2), sqlGetOutputColName(stmt, i))) {
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
	SQLS stmt;
	int i;
	PCON_STATE state;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE
	    || Z_TYPE_P(arg1) != IS_LONG) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long( arg1);
	state = (PCON_STATE) Z_LVAL_P(arg1);

	for (i=0;i<state->nstatements;i++) {
		stmt = state->statements[ i].statement;
		if (stmt==NULL) {
			continue;
		}
		if (!sqlCommit(stmt)) {
			RETURN_FALSE;
		}
	}
	RETURN_TRUE;
}

/* }}} */

/* {{{ proto int ovrimos_rollback(int connection_id)
   Rollback a transaction */
PHP_FUNCTION(ovrimos_rollback)
{
	pval *arg1;
	SQLS stmt;
	int i;
	PCON_STATE state;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE
	    || Z_TYPE_P(arg1) != IS_LONG) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long( arg1);
	state = (PCON_STATE) Z_LVAL_P(arg1);

	for (i=0;i<state->nstatements;i++) {
		stmt = (SQLS) state->statements[ i].statement;
		if (stmt==NULL) continue;

		if (!sqlRollback(stmt)) {
			RETURN_FALSE;
		}
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

/* {{{ ovrimos_functions[]
 */
function_entry ovrimos_functions[] = {
/*     PHP_FE(ovrimos_setoption, NULL)*/
/*     PHP_FE(ovrimos_autocommit, NULL)*/
	PHP_FE(ovrimos_close, NULL)
	    PHP_FE(ovrimos_commit, NULL)
	    PHP_FE(ovrimos_connect, NULL)
	    PHP_FE(ovrimos_cursor, NULL)
	    PHP_FE(ovrimos_exec, NULL)
	    PHP_FE(ovrimos_prepare, NULL)
	    PHP_FE(ovrimos_execute, NULL)
	    PHP_FE(ovrimos_fetch_row, NULL)
	    PHP_FE(ovrimos_fetch_into, second_arg_force_ref)
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
/* }}} */

zend_module_entry ovrimos_module_entry = {
	STANDARD_MODULE_HEADER,
	"ovrimos",
	ovrimos_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(ovrimos),
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

DLEXPORT zend_module_entry *get_module()
{
	return &ovrimos_module_entry;
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
