/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | dbx module version 1.0                                               |
   +----------------------------------------------------------------------+
   | Copyright (c) 2001 Guidance Rotterdam BV                             |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author : Marc Boeren         <marc@guidance.nl>                      |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "dbx.h"
#include "dbx_oci8.h"

#define OCI_ASSOC        1<<0
#define OCI_NUM          1<<1
#define OCI_RETURN_NULLS 1<<2
#define OCI_RETURN_LOBS  1<<3

int dbx_oci8_connect(zval **rv, zval **host, zval **db, zval **username, zval **password, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns connection handle as resource on success or 0 as long on failure */
	int number_of_arguments=3;
	zval **arguments[3];
	zval *returned_zval=NULL;

	arguments[0]=username;
	arguments[1]=password;
	arguments[2]=db;
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "OCILogon", &returned_zval, number_of_arguments, arguments);
	if (!returned_zval || returned_zval->type!=IS_RESOURCE) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		return 0;
	}
	MOVE_RETURNED_TO_RV(rv, returned_zval);
	return 1;
}

int dbx_oci8_pconnect(zval **rv, zval **host, zval **db, zval **username, zval **password, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns connection handle as resource on success or 0 as long on failure */
	int number_of_arguments=3;
	zval **arguments[3];
	zval *returned_zval=NULL;

	arguments[0]=username;
	arguments[1]=password;
	arguments[2]=db;
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "OCIPLogon", &returned_zval, number_of_arguments, arguments);
	if (!returned_zval || returned_zval->type!=IS_RESOURCE) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		return 0;
	}
	MOVE_RETURNED_TO_RV(rv, returned_zval);
	return 1;
}

int dbx_oci8_close(zval **rv, zval **dbx_handle, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns 1 as long on success or 0 as long on failure */
	int number_of_arguments=1;
	zval **arguments[1];
	zval *returned_zval=NULL;

	arguments[0]=dbx_handle;
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "OCILogOff", &returned_zval, number_of_arguments, arguments);
	if (!returned_zval || returned_zval->type!=IS_BOOL) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		return 0;
	}
	MOVE_RETURNED_TO_RV(rv, returned_zval);
	return 1;
}

int dbx_oci8_query(zval **rv, zval **dbx_handle, zval **db_name, zval **sql_statement, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns 1 as long or a result identifier as resource on success  or 0 as long on failure */
	int number_of_arguments=2;
	zval **arguments[2];
	zval *returned_zval=NULL;
	zval *execute_zval=NULL;

	arguments[0]=dbx_handle;
	arguments[1]=sql_statement;
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "OCIParse", &returned_zval, number_of_arguments, arguments);
	/* OCIParse returns a bool for failure, or a statement_identifier for valid sql_statements */
	if (!returned_zval || (returned_zval->type!=IS_BOOL && returned_zval->type!=IS_RESOURCE)) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		return 0;
	}
	number_of_arguments=1;
	arguments[0]=&returned_zval;
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "OCIExecute", &execute_zval, number_of_arguments, arguments);
	/* OCIExecute returns a bool for success or failure???? */
	if (!execute_zval || execute_zval->type!=IS_BOOL) {
		if (execute_zval) zval_ptr_dtor(&execute_zval);
		zval_ptr_dtor(&returned_zval);
		return 0;
	}
	MOVE_RETURNED_TO_RV(rv, returned_zval);
	return 1;
}

int dbx_oci8_getcolumncount(zval **rv, zval **result_handle, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns column-count as long on success or 0 as long on failure */
	int number_of_arguments=1;
	zval **arguments[1];
	zval *returned_zval=NULL;

	arguments[0]=result_handle;
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "OCINumCols", &returned_zval, number_of_arguments, arguments);
	if (!returned_zval || returned_zval->type!=IS_LONG) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		return 0;
	}
	MOVE_RETURNED_TO_RV(rv, returned_zval);
	return 1;
}

int dbx_oci8_getcolumnname(zval **rv, zval **result_handle, long column_index, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns column-name as string on success or 0 as long on failure */
	int number_of_arguments=2;
	zval **arguments[2];
	zval *zval_column_index;
	zval *returned_zval=NULL;

	MAKE_STD_ZVAL(zval_column_index);
	/* dbx uses 0-based column-indices, oci8 uses 1-based indices... */
	ZVAL_LONG(zval_column_index, column_index+1);
	arguments[0]=result_handle;
	arguments[1]=&zval_column_index;
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "OCIColumnName", &returned_zval, number_of_arguments, arguments);
	/* OCIColumnName returns a string */
	if (!returned_zval || returned_zval->type!=IS_STRING) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		FREE_ZVAL(zval_column_index);
		return 0;
	}
	FREE_ZVAL(zval_column_index);
	MOVE_RETURNED_TO_RV(rv, returned_zval);
	return 1;
}

int dbx_oci8_getcolumntype(zval **rv, zval **result_handle, long column_index, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns column-type as string on success or 0 as long on failure */
	int number_of_arguments=2;
	zval **arguments[2];
	zval *zval_column_index;
	zval *returned_zval=NULL;

	MAKE_STD_ZVAL(zval_column_index);
	/* dbx uses 0-based column-indices, oci8 uses 1-based indices... */
	ZVAL_LONG(zval_column_index, column_index+1);
	arguments[0]=result_handle;
	arguments[1]=&zval_column_index;
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "OCIColumnType", &returned_zval, number_of_arguments, arguments);
	/* OCIColumnType returns a string??? */
	if (!returned_zval || returned_zval->type!=IS_STRING) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		FREE_ZVAL(zval_column_index);
		return 0;
	}
	FREE_ZVAL(zval_column_index);

	MOVE_RETURNED_TO_RV(rv, returned_zval);
	return 1;
}

int dbx_oci8_getrow(zval **rv, zval **result_handle, long row_number, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns array[0..columncount-1] as strings on success or 0 as long on failure */
	int number_of_arguments=3;
	zval **arguments[3];
	zval *zval_resulttype=NULL;
	zval *zval_returned_array=NULL;
	zval *returned_zval=NULL;

	MAKE_STD_ZVAL(zval_resulttype);
	ZVAL_LONG(zval_resulttype, OCI_NUM | OCI_RETURN_NULLS); /* no ASSOC, dbx handles that part */
	arguments[0]=result_handle;
	arguments[1]=&zval_returned_array;
	arguments[2]=&zval_resulttype;
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "OCIFetchInto", &returned_zval, number_of_arguments, arguments);
	/* OCIFetchInto returns an integer, but the actual array is passed back in arg[1] */
	/* I'm not sure how this will work, Thies, so this is something that should be especially tested! */
	if (!returned_zval || returned_zval->type!=IS_BOOL || returned_zval->value.lval==0) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		FREE_ZVAL(zval_resulttype);
		return 0;
	}
	FREE_ZVAL(zval_resulttype);
	zval_ptr_dtor(&returned_zval);
	MOVE_RETURNED_TO_RV(rv, zval_returned_array);
	return 1;
}

int dbx_oci8_error(zval **rv, zval **dbx_handle, INTERNAL_FUNCTION_PARAMETERS)
{
	/* returns string */
	int number_of_arguments=1;
	zval **arguments[1];
	zval *returned_zval=NULL;
	zval *returned_message_zval=NULL;
	arguments[0]=dbx_handle;
	if (!dbx_handle) number_of_arguments=0;
	dbx_call_any_function(INTERNAL_FUNCTION_PARAM_PASSTHRU, "OCIError", &returned_zval, number_of_arguments, arguments);
	/* OCIError should returns an assoc array containing code & message, dbx needs the message */
	if (!returned_zval || returned_zval->type!=IS_ARRAY) {
		if (returned_zval) zval_ptr_dtor(&returned_zval);
		return 0;
	}
	/* get the messagestring here */
	if (zend_hash_find(returned_zval->value.ht, "message", strlen("message")+1, (void **) &returned_message_zval)==FAILURE) {
		/* oops! no msg? */
		zval_ptr_dtor(&returned_zval);
		return 0;
	}
	MOVE_RETURNED_TO_RV(rv, returned_message_zval);
	zval_ptr_dtor(&returned_zval);
	return 1;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
