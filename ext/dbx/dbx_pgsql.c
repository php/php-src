/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author : Rui Hirokawa        <hirokawa@php.net>                      |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "dbx.h"
#include "php_dbx.h"
#include "dbx_pgsql.h"
#include <string.h>

#define PGSQL_ASSOC    1<<0
#define PGSQL_NUM      1<<1

int dbx_pgsql_connect(zval **rv, zval **host, zval **db, zval **username, zval **password, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns connection handle as resource on success or 0 as long on failure */
	int nargs=5;
	char *port="5432", *connstring=NULL;
	zval **args[5], *rarg = NULL;
	zval *conn_zval = NULL;
	zval *returned_zval=NULL;

	MAKE_STD_ZVAL(conn_zval);
	ZVAL_LONG(conn_zval, 0);

	if (Z_STRLEN_PP(username)>0) {
		int len;

		len = Z_STRLEN_PP(host)+Z_STRLEN_PP(db)+strlen(port);
		len += Z_STRLEN_PP(username)+Z_STRLEN_PP(password)+45;
		connstring = (char *)emalloc(len+1);
		sprintf(connstring, "host='%s' port='%s' dbname='%s' user='%s' password='%s'",
				Z_STRVAL_PP(host), port, Z_STRVAL_PP(db),
				Z_STRVAL_PP(username), Z_STRVAL_PP(password));
		ZVAL_STRING(conn_zval, connstring, 0);
		args[0] = &conn_zval;
		nargs = 1;
	} else {
		int k;

		args[0] = host;
		for (k=1;k<4;k++){
			MAKE_STD_ZVAL(rarg);
			ZVAL_EMPTY_STRING(rarg);
			args[k] = &rarg;
		}
		args[4] = db;
	}
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "pg_connect", &returned_zval, nargs, args);		
	zval_dtor(conn_zval);
	FREE_ZVAL(conn_zval);

	if (!returned_zval || Z_TYPE_P(returned_zval)!=IS_RESOURCE) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		return 0;
	}
	MOVE_RETURNED_TO_RV(rv, returned_zval);
	return 1;
}

int dbx_pgsql_pconnect(zval **rv, zval **host, zval **db, zval **username, zval **password, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns persistent connection handle as resource on success or 0 as long on failure */
	int nargs=5;
	char *port="5432", *connstring=NULL;
	zval **args[5], *rarg = NULL;
	zval *conn_zval = NULL;
	zval *returned_zval=NULL;

	MAKE_STD_ZVAL(conn_zval);
	ZVAL_LONG(conn_zval, 0);

	if (Z_STRLEN_PP(username)>0) {
		int len;

		len = Z_STRLEN_PP(host)+Z_STRLEN_PP(db)+strlen(port);
		len += Z_STRLEN_PP(username)+Z_STRLEN_PP(password)+45;
		connstring = (char *)emalloc(len+1);
		sprintf(connstring, "host='%s' port='%s' dbname='%s' user='%s' password='%s'",
				Z_STRVAL_PP(host), port, Z_STRVAL_PP(db),
				Z_STRVAL_PP(username), Z_STRVAL_PP(password));
		ZVAL_STRING(conn_zval, connstring, 0);
		args[0] = &conn_zval;
		nargs = 1;
	} else {
		int k;

		args[0] = host;
		for (k=1;k<4;k++){
			MAKE_STD_ZVAL(rarg);
			ZVAL_EMPTY_STRING(rarg);
			args[k] = &rarg;
		}
		args[4] = db;
	}
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "pg_pconnect", &returned_zval, nargs, args);
	zval_dtor(conn_zval);
	FREE_ZVAL(conn_zval);

	if (!returned_zval || Z_TYPE_P(returned_zval)!=IS_RESOURCE) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		return 0;
	}
	MOVE_RETURNED_TO_RV(rv, returned_zval);
	return 1;
}

int dbx_pgsql_close(zval **rv, zval **dbx_handle, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns 1 as long on success or 0 as long on failure */
	int number_of_arguments=1;
	zval **arguments[1];
	zval *returned_zval=NULL;

	arguments[0]=dbx_handle;
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "pg_close", &returned_zval, number_of_arguments, arguments);
	if (!returned_zval || Z_TYPE_P(returned_zval)!=IS_BOOL) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		return 0;
	}
	MOVE_RETURNED_TO_RV(rv, returned_zval);
	return 1;
}

int dbx_pgsql_query(zval **rv, zval **dbx_handle, zval **db_name, zval **sql_statement, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns 1 as long or a result identifier as resource on success  
	   or 0 as long on failure */
	int nargs=2;
	zval **args[2];
	zval *returned_zval=NULL;

	/* db_name is not used in this function */
	args[0]=dbx_handle;
	args[1]=sql_statement;
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "pg_exec", &returned_zval, nargs, args);
	/* pg_query returns a bool for success or failure, 
	   or a result_identifier for select statements */
	if (!returned_zval || (Z_TYPE_P(returned_zval)!=IS_BOOL && Z_TYPE_P(returned_zval)!=IS_RESOURCE)) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		return 0;
	}
	MOVE_RETURNED_TO_RV(rv, returned_zval);
	return 1;
}

int dbx_pgsql_getcolumncount(zval **rv, zval **result_handle, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns column-count as long on success or 0 as long on failure */
	int number_of_arguments=1;
	zval **arguments[1];
	zval *returned_zval=NULL;

	arguments[0]=result_handle;
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "pg_numfields", &returned_zval, number_of_arguments, arguments);
	if (!returned_zval || Z_TYPE_P(returned_zval)!=IS_LONG) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		return 0;
	}
	MOVE_RETURNED_TO_RV(rv, returned_zval);
	return 1;
}

int dbx_pgsql_getcolumnname(zval **rv, zval **result_handle, long column_index, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns column-name as string on success or 0 as long on failure */
	int number_of_arguments=2;
	zval **arguments[2];
	zval *zval_column_index;
	zval *returned_zval=NULL;

	MAKE_STD_ZVAL(zval_column_index);
	ZVAL_LONG(zval_column_index, column_index);
	arguments[0]=result_handle;
	arguments[1]=&zval_column_index;
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "pg_fieldname", &returned_zval, number_of_arguments, arguments);
	/* pg_fieldname returns a string */
	if (!returned_zval || Z_TYPE_P(returned_zval)!=IS_STRING) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		FREE_ZVAL(zval_column_index);
		return 0;
	}
	FREE_ZVAL(zval_column_index);
	MOVE_RETURNED_TO_RV(rv, returned_zval);
	return 1;
}

int dbx_pgsql_getcolumntype(zval **rv, zval **result_handle, long column_index, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns column-type as string on success or 0 as long on failure */
	int number_of_arguments=2;
	zval **arguments[2];
	zval *zval_column_index;
	zval *returned_zval=NULL;

	MAKE_STD_ZVAL(zval_column_index);
	ZVAL_LONG(zval_column_index, column_index);
	arguments[0]=result_handle;
	arguments[1]=&zval_column_index;
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "pg_fieldtype", &returned_zval, number_of_arguments, arguments);
	/* pg_fieldtype returns a string */
	if (!returned_zval || Z_TYPE_P(returned_zval)!=IS_STRING) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		FREE_ZVAL(zval_column_index);
		return 0;
	}
	FREE_ZVAL(zval_column_index);

	MOVE_RETURNED_TO_RV(rv, returned_zval);
	return 1;
}

int dbx_pgsql_getrow(zval **rv, zval **result_handle, long row_number, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns array[0..columncount-1] as strings on success or 0 as long on failure */
	int number_of_arguments=2;
	int save_error_reporting=0;
	zval **arguments[2];
	zval *zval_row=NULL;
	zval *returned_zval=NULL;
	
	MAKE_STD_ZVAL(zval_row);
	ZVAL_LONG(zval_row, row_number);
	arguments[0]=result_handle;
	arguments[1]=&zval_row;

	if (EG(error_reporting) & E_WARNING){
		save_error_reporting = EG(error_reporting);
		EG(error_reporting) &= ~E_WARNING;
	}
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "pg_fetch_array", &returned_zval, number_of_arguments, arguments);
	if (save_error_reporting) {
		EG(error_reporting) = save_error_reporting;
	}
	if (!returned_zval || Z_TYPE_P(returned_zval)!=IS_ARRAY) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		FREE_ZVAL(zval_row);
		return 0;
	}
	FREE_ZVAL(zval_row);
	MOVE_RETURNED_TO_RV(rv, returned_zval);
	return 1;
}

int dbx_pgsql_error(zval **rv, zval **dbx_handle, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns string */
	int number_of_arguments=1;
	zval **arguments[1];
	zval *returned_zval=NULL;

	arguments[0]=dbx_handle;
	if (!dbx_handle) number_of_arguments=0;
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "pg_errormessage", &returned_zval, number_of_arguments, arguments);
	if (!returned_zval || Z_TYPE_P(returned_zval)!=IS_STRING) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		return 0;
	}
	MOVE_RETURNED_TO_RV(rv, returned_zval);
	return 1;
}

int dbx_pgsql_esc(zval **rv, zval **dbx_handle, zval **string, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns escaped string */
	/* replace \ with \\ */
	/*         ' with '' */
	char * str;
	int len;
	char * tmpstr;
	int tmplen;

	tmpstr = estrdup(Z_STRVAL_PP(string));
	tmplen = Z_STRLEN_PP(string);
	/* php_str_to_str uses a smart_str that allocates memory */
	/* this memory must be freed or passed on to rv */
	str = php_str_to_str(tmpstr, tmplen, "\\", 1, "\\\\", 2, &len);
	efree(tmpstr);
	tmpstr=str; tmplen=len;
	str = php_str_to_str(tmpstr, tmplen, "'", 1, "''", 2, &len);
	efree(tmpstr);

	ZVAL_STRINGL(*rv, str, len, 0);
	return 1;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
