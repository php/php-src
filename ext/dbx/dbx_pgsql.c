/*
   +----------------------------------------------------------------------+
   | stentor module version 1.0                                           |
   +----------------------------------------------------------------------+
   | Copyright (c) 2001 Guidance Rotterdam BV                             |
   +----------------------------------------------------------------------+
   | This source file is subject to version 1.0  of the STENTOR license,  |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at                              |
   | http://www.guidance.nl/php/dbx/license/1_00.txt.                     |
   | If you did not receive a copy of the STENTOR license and are unable  |
   | to obtain it through the world-wide-web, please send a note to       |
   | license@guidance.nl so we can mail you a copy immediately.           |
   +----------------------------------------------------------------------+
   | Author : Rui Hirokawa        <hirokawa@php.net>                      |
   +----------------------------------------------------------------------+
 */

#include "dbx.h"
#include "php_dbx.h"
#include "dbx_pgsql.h"
#include <string.h>

#ifdef ZTS
extern int dbx_globals_id;
#else
extern ZEND_DBX_API zend_dbx_globals dbx_globals;
#endif

#define PGSQL_ASSOC		1<<0
#define PGSQL_NUM		1<<1

int dbx_pgsql_connect(zval ** rv, zval ** host, zval ** db, zval ** username, zval ** password, INTERNAL_FUNCTION_PARAMETERS) {
    /* returns connection handle as resource on success or 0 
	   as long on failure */
    int nargs=5;
	char *port="5432", *connstring=NULL;
    zval **args[5], *rarg = NULL;
	zval *conn_zval = NULL;
    zval *returned_zval=NULL;

	MAKE_STD_ZVAL(conn_zval);

	if (Z_STRLEN_PP(username)>0 && Z_STRLEN_PP(password)>0){
		int len;

		len = Z_STRLEN_PP(host)+Z_STRLEN_PP(db)+strlen(port);
		len += Z_STRLEN_PP(username)+Z_STRLEN_PP(password)+35;
		connstring = (char *)emalloc(len+1);
		sprintf(connstring, "host=%s port=%s dbname=%s user=%s password=%s",
				Z_STRVAL_PP(host),port, Z_STRVAL_PP(db),
				Z_STRVAL_PP(username), Z_STRVAL_PP(password));
		ZVAL_STRING(conn_zval, connstring, 1);
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

    if (!returned_zval || returned_zval->type!=IS_RESOURCE) {
        if (returned_zval) zval_ptr_dtor(&returned_zval);
        return 0;
	}
    MOVE_RETURNED_TO_RV(rv, returned_zval);
	
    return 1;
}

int dbx_pgsql_pconnect(zval ** rv, zval ** host, zval ** db, zval ** username, zval ** password, INTERNAL_FUNCTION_PARAMETERS) {
    /* returns persistent connection handle as resource on success or 0 
	   as long on failure */
    int nargs=5;
	char *port="5432", *connstring=NULL;
    zval **args[5], *rarg = NULL;
	zval *conn_zval = NULL;
    zval *returned_zval=NULL;

	MAKE_STD_ZVAL(conn_zval);

	if (Z_STRLEN_PP(username)>0 && Z_STRLEN_PP(password)>0){
		int len;

		len = Z_STRLEN_PP(host)+Z_STRLEN_PP(db)+strlen(port);
		len += Z_STRLEN_PP(username)+Z_STRLEN_PP(password)+35;
		connstring = (char *)emalloc(len+1);
		sprintf(connstring, "host=%s port=%s dbname=%s user=%s password=%s",
				Z_STRVAL_PP(host),port, Z_STRVAL_PP(db),
				Z_STRVAL_PP(username), Z_STRVAL_PP(password));
		ZVAL_STRING(conn_zval, connstring, 1);
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

    if (!returned_zval || returned_zval->type!=IS_RESOURCE) {
        if (returned_zval) zval_ptr_dtor(&returned_zval);
        return 0;
	}
    MOVE_RETURNED_TO_RV(rv, returned_zval);
	
    return 1;
}

int dbx_pgsql_close(zval ** rv, zval ** dbx_handle, INTERNAL_FUNCTION_PARAMETERS) {
    /* returns 1 as long on success or 0 as long on failure */
    int number_of_arguments=1;
    zval ** arguments[1];
    zval * returned_zval=NULL;

    arguments[0]=dbx_handle;
    dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "pg_close", &returned_zval, number_of_arguments, arguments);
    if (!returned_zval || returned_zval->type!=IS_BOOL) {
        if (returned_zval) zval_ptr_dtor(&returned_zval);
        return 0;
	}
    MOVE_RETURNED_TO_RV(rv, returned_zval);
    return 1;
}

int dbx_pgsql_query(zval ** rv, zval ** dbx_handle, zval ** sql_statement, INTERNAL_FUNCTION_PARAMETERS) {
    /* returns 1 as long or a result identifier as resource on success  
	   or 0 as long on failure */
    int nargs=2;
    zval **args[2];
    zval *returned_zval=NULL, *num_rows_zval=NULL;

    args[0]=dbx_handle;
    args[1]=sql_statement;
    dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "pg_exec", &returned_zval, nargs, args);
    /* pg_query returns a bool for success or failure, 
	   or a result_identifier for select statements */
	if (!returned_zval || (returned_zval->type!=IS_BOOL && returned_zval->type!=IS_RESOURCE)) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		return 0;
	}
    MOVE_RETURNED_TO_RV(rv, returned_zval);

	if(strstr(Z_STRVAL_PP(sql_statement), "SELECT") ||
	   strstr(Z_STRVAL_PP(sql_statement), "select")){
		DBXG(row_count) = 0;

		args[0]=rv;
		nargs = 1;
		dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "pg_numrows", &num_rows_zval, nargs, args);
		DBXG(num_rows) = Z_LVAL_P(num_rows_zval);
	} 

    return 1;
}

int dbx_pgsql_getcolumncount(zval ** rv, zval ** result_handle, INTERNAL_FUNCTION_PARAMETERS) {
    /* returns column-count as long on success or 0 as long on failure */
    int number_of_arguments=1;
    zval ** arguments[1];
    zval * returned_zval=NULL;

    arguments[0]=result_handle;
    dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "pg_numfields", &returned_zval, number_of_arguments, arguments);
    if (!returned_zval || returned_zval->type!=IS_LONG) {
        if (returned_zval) zval_ptr_dtor(&returned_zval);
        return 0;
        }
    MOVE_RETURNED_TO_RV(rv, returned_zval);
    return 1;
    }

int dbx_pgsql_getcolumnname(zval ** rv, zval ** result_handle, long column_index, INTERNAL_FUNCTION_PARAMETERS) {
    /* returns column-name as string on success or 0 as long on failure */
    int number_of_arguments=2;
    zval ** arguments[2];
    zval * zval_column_index;
    zval * returned_zval=NULL;

    MAKE_STD_ZVAL(zval_column_index);
    ZVAL_LONG(zval_column_index, column_index);
    arguments[0]=result_handle;
    arguments[1]=&zval_column_index;
    dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "pg_fieldname", &returned_zval, number_of_arguments, arguments);
    /*/ pgsql_field_name returns a string /*/
    if (!returned_zval || returned_zval->type!=IS_STRING) {
        if (returned_zval) zval_ptr_dtor(&returned_zval);
        FREE_ZVAL(zval_column_index);
        return 0;
        }
    FREE_ZVAL(zval_column_index);
    MOVE_RETURNED_TO_RV(rv, returned_zval);
    return 1;
    }

int dbx_pgsql_getcolumntype(zval ** rv, zval ** result_handle, long column_index, INTERNAL_FUNCTION_PARAMETERS) {
    /* returns column-type as string on success or 0 as long on failure */
    int number_of_arguments=2;
    zval ** arguments[2];
    zval * zval_column_index;
    zval * returned_zval=NULL;

    MAKE_STD_ZVAL(zval_column_index);
    ZVAL_LONG(zval_column_index, column_index);
    arguments[0]=result_handle;
    arguments[1]=&zval_column_index;
    dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "pg_fieldtype", &returned_zval, number_of_arguments, arguments);
    /* pgsql_field_name returns a string */
	if (!returned_zval || returned_zval->type!=IS_STRING) {
        if (returned_zval) zval_ptr_dtor(&returned_zval);
        FREE_ZVAL(zval_column_index);
        return 0;
	}
    FREE_ZVAL(zval_column_index);
    MOVE_RETURNED_TO_RV(rv, returned_zval);
    return 1;
}

int dbx_pgsql_getrow(zval ** rv, zval ** result_handle, INTERNAL_FUNCTION_PARAMETERS) {
    /* returns array[0..columncount-1] as strings on success or 0 
	   as long on failure */
    int number_of_arguments=2;
    zval ** arguments[2];
    zval * zval_row=NULL;
    zval * returned_zval=NULL;
	
    MAKE_STD_ZVAL(zval_row);
    ZVAL_LONG(zval_row, DBXG(row_count));
    arguments[0]=result_handle;
    arguments[1]=&zval_row;
	DBXG(row_count)++;
	if (DBXG(row_count)>DBXG(num_rows)){
		return 0;
	}
    dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "pg_fetch_array", &returned_zval, number_of_arguments, arguments);
    if (!returned_zval || returned_zval->type!=IS_ARRAY) {
        if (returned_zval) zval_ptr_dtor(&returned_zval);
        FREE_ZVAL(zval_row);
        return 0;
	}
    FREE_ZVAL(zval_row);
    MOVE_RETURNED_TO_RV(rv, returned_zval);
    return 1;
}

int dbx_pgsql_error(zval ** rv, zval ** dbx_handle, INTERNAL_FUNCTION_PARAMETERS) {
    /* returns string */
    int number_of_arguments=1;
    zval ** arguments[1];
    zval * returned_zval=NULL;

    arguments[0]=dbx_handle;
    if (!dbx_handle) number_of_arguments=0;
    dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "pg_errormessage", &returned_zval, number_of_arguments, arguments);
    if (!returned_zval || returned_zval->type!=IS_STRING) {
        if (returned_zval) zval_ptr_dtor(&returned_zval);
        return 0;
	}
    MOVE_RETURNED_TO_RV(rv, returned_zval);
    return 1;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
