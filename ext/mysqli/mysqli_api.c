/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2004 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Georg Richter <georg@php.net>                                |
  +----------------------------------------------------------------------+

  $Id$ 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <signal.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_mysqli.h"

/* {{{ proto mixed mysqli_affected_rows(object link)
   Get number of affected rows in previous MySQL operation */
PHP_FUNCTION(mysqli_affected_rows)
{
	MY_MYSQL 		*mysql;
	zval  			*mysql_link;
	my_ulonglong	rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	rc = mysql_affected_rows(mysql->mysql);
	MYSQLI_RETURN_LONG_LONG(rc);
}
/* }}} */

/* {{{ proto bool mysqli_autocommit(object link, bool mode)
   Turn auto commit on or of */
PHP_FUNCTION(mysqli_autocommit)
{
	MY_MYSQL 		*mysql;
	zval  			*mysql_link;
	unsigned long	rc;
	unsigned long	automode;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ob", &mysql_link, mysqli_link_class_entry, &automode) == FAILURE) {
		return;	
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	rc = (long) mysql_autocommit(mysql->mysql, automode);

	RETURN_BOOL(rc);
}
/* }}} */

/* {{{ proto bool mysqli_stmt_bind_param(object stmt, string types, mixed variable [,mixed,....])
   Bind variables to a prepared statement as parameters */
PHP_FUNCTION(mysqli_stmt_bind_param)
{
	zval 			***args;
	int     		argc = ZEND_NUM_ARGS();
	int 	    	i;
	int				num_vars;
	int				start = 2;
	int				ofs;
	MY_STMT			*stmt;
	zval 			*mysql_stmt;
	MYSQL_BIND		*bind;
	char			*types;
	int				typelen;
	unsigned long	rc;

	/* calculate and check number of parameters */
	num_vars = argc - 1;
	if (!getThis()) {
		/* ignore handle parameter in procedural interface*/
		--num_vars; 
	}
	if (argc < 2) {
		/* there has to be at least one pair */
		WRONG_PARAM_COUNT;
	}

	if (zend_parse_method_parameters((getThis()) ? 1:2 TSRMLS_CC, getThis(), "Os", &mysql_stmt, mysqli_stmt_class_entry, &types, &typelen) == FAILURE) {
		return;	
	}

	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt"); 

	if (getThis()) {
		start = 1;
	}

	if (strlen(types) != argc - start) {
		/* number of bind variables doesn't match number of elements in type definition string */
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number of elements in type definition string doesn't match number of bind variables");
	}

	if (argc - start != stmt->stmt->param_count) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number of variables doesn't match number of parameters in prepared statement");
		RETURN_FALSE;
	}

	/* prevent leak if variables are already bound */
	if (stmt->param.var_cnt) {
		php_free_stmt_bind_buffer(stmt->param, FETCH_SIMPLE);
	}

	args = (zval ***)safe_emalloc(argc, sizeof(zval **), 0);

	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}

	stmt->param.is_null = ecalloc(num_vars, sizeof(char));
	bind = (MYSQL_BIND *)ecalloc(num_vars, sizeof(MYSQL_BIND));

	ofs = 0;
	for (i=start; i < argc; i++) {

		/* set specified type */
		switch (types[ofs]) {
			case 'd': /* Double */
				bind[ofs].buffer_type = MYSQL_TYPE_DOUBLE;
				bind[ofs].buffer = (gptr)&Z_DVAL_PP(args[i]);
				bind[ofs].is_null = &stmt->param.is_null[ofs];
				break;

			case 'i': /* Integer */
				bind[ofs].buffer_type = MYSQL_TYPE_LONG;
				bind[ofs].buffer = (gptr)&Z_LVAL_PP(args[i]);
				bind[ofs].is_null = &stmt->param.is_null[ofs];
				break;

			case 'b': /* Blob (send data) */
				bind[ofs].buffer_type = MYSQL_TYPE_VAR_STRING;
				bind[ofs].is_null = 0;
				bind[ofs].length = 0;
				break;

			case 's': /* string */
				bind[ofs].buffer_type = MYSQL_TYPE_VAR_STRING;
				bind[ofs].buffer = NULL; 
				bind[ofs].buffer_length = 0;
				bind[ofs].is_null = &stmt->param.is_null[ofs];
				break;

			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Undefined fieldtype %c (parameter %d)", types[ofs], i+1);
				efree(args);
				efree(bind);
				RETURN_FALSE; 
				break;
		}
		ofs++;
	}
	rc = mysql_stmt_bind_param(stmt->stmt, bind);
	MYSQLI_REPORT_STMT_ERROR(stmt->stmt);

	if (rc) {
		efree(args);
		efree(bind);
		RETURN_FALSE;
	}

	stmt->param.var_cnt = num_vars;
	stmt->param.vars = (zval **)safe_emalloc(num_vars, sizeof(zval), 0);
	for (i = 0; i < num_vars; i++) {
		if (bind[i].buffer_type  != MYSQLI_BIND_SEND_DATA) {
			ZVAL_ADDREF(*args[i+start]);
			stmt->param.vars[i] = *args[i+start];
		} else {
			stmt->param.vars[i] = NULL;
		}
	}
	efree(args);
	efree(bind);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_stmt_bind_result(object stmt, mixed var, [,mixed, ...])
   Bind variables to a prepared statement for result storage */

/* TODO:
   do_alloca, free_alloca
*/

PHP_FUNCTION(mysqli_stmt_bind_result)
{
	zval 		***args;
	int     	argc = ZEND_NUM_ARGS();
	int     	i;
	int			start = 1;
	int			var_cnt;
	int			ofs;
	long		col_type;
	ulong		rc;
	MY_STMT 	*stmt;
	zval 		*mysql_stmt;
	MYSQL_BIND 	*bind;

	if (argc < (getThis() ? 1 : 2))  {
		WRONG_PARAM_COUNT;
	}

	args = (zval ***)safe_emalloc(argc, sizeof(zval **), 0);

	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}

	if (getThis()) {
		start = 0;
	}

	if (zend_parse_method_parameters((getThis()) ? 0:1 TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;	
	}

	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt"); 
	
	var_cnt = argc - start;

	if (var_cnt != mysql_stmt_field_count(stmt->stmt)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number of bind variables doesn't match number of fields in prepared statmement.");
		efree(args);
		RETURN_FALSE;
	}

	/* prevent leak if variables are already bound */
	if (stmt->result.var_cnt) {
		php_free_stmt_bind_buffer(stmt->result, FETCH_RESULT);
	}

	bind = (MYSQL_BIND *)ecalloc(var_cnt, sizeof(MYSQL_BIND));
	stmt->result.buf = (VAR_BUFFER *)ecalloc(var_cnt,sizeof(VAR_BUFFER));
	stmt->result.is_null = (char *)ecalloc(var_cnt, sizeof(char));

	for (i=start; i < var_cnt + start ; i++) {
		ofs = i - start;
		stmt->result.is_null[ofs] = 0;

		col_type = (stmt->stmt->fields) ? stmt->stmt->fields[ofs].type : MYSQL_TYPE_STRING;

		switch (col_type) {
			case MYSQL_TYPE_DOUBLE:
			case MYSQL_TYPE_FLOAT:
				convert_to_double_ex(args[i]);
				stmt->result.buf[ofs].type = IS_DOUBLE;
				stmt->result.buf[ofs].buflen = sizeof(double);
				
				/* allocate buffer for double */
				stmt->result.buf[ofs].val = (char *)emalloc(sizeof(double));
				bind[ofs].buffer_type = MYSQL_TYPE_DOUBLE;
				bind[ofs].buffer = stmt->result.buf[ofs].val;
				bind[ofs].is_null = &stmt->result.is_null[ofs];
				break;

			case MYSQL_TYPE_SHORT:
			case MYSQL_TYPE_TINY:
			case MYSQL_TYPE_LONG:
			case MYSQL_TYPE_INT24:
			case MYSQL_TYPE_YEAR:
				convert_to_long_ex(args[i]);
				stmt->result.buf[ofs].type = IS_LONG;
				stmt->result.buf[ofs].buflen = 0;
				stmt->result.buf[ofs].val = (char *)emalloc(sizeof(long));
				bind[ofs].buffer_type = MYSQL_TYPE_LONG;
				bind[ofs].buffer = stmt->result.buf[ofs].val;
				bind[ofs].is_null = &stmt->result.is_null[ofs];
				break;

			case MYSQL_TYPE_LONGLONG:
				stmt->result.buf[ofs].type = IS_STRING; 
				stmt->result.buf[ofs].buflen = sizeof(my_ulonglong); 
				stmt->result.buf[ofs].val = (char *)emalloc(stmt->result.buf[ofs].buflen);
				bind[ofs].buffer_type = MYSQL_TYPE_LONGLONG;
				bind[ofs].buffer = stmt->result.buf[ofs].val;
				bind[ofs].is_null = &stmt->result.is_null[ofs];
				bind[ofs].buffer_length = stmt->result.buf[ofs].buflen;
				break;

			case MYSQL_TYPE_DATE:
			case MYSQL_TYPE_TIME:
			case MYSQL_TYPE_DATETIME:
			case MYSQL_TYPE_NEWDATE:
			case MYSQL_TYPE_VAR_STRING:
			case MYSQL_TYPE_STRING:
			case MYSQL_TYPE_BLOB:
			case MYSQL_TYPE_TIMESTAMP:
			case MYSQL_TYPE_DECIMAL:
				stmt->result.buf[ofs].type = IS_STRING; 
				stmt->result.buf[ofs].buflen =
					(stmt->stmt->fields) ? (stmt->stmt->fields[ofs].length) ? stmt->stmt->fields[ofs].length + 1: 256: 256;
				stmt->result.buf[ofs].val = (char *)emalloc(stmt->result.buf[ofs].buflen);
				bind[ofs].buffer_type = MYSQL_TYPE_STRING;
				bind[ofs].buffer = stmt->result.buf[ofs].val;
				bind[ofs].is_null = &stmt->result.is_null[ofs];
				bind[ofs].buffer_length = stmt->result.buf[ofs].buflen;
				bind[ofs].length = &stmt->result.buf[ofs].buflen;
				break;
		}
	}

	rc = mysql_stmt_bind_result(stmt->stmt, bind);
	MYSQLI_REPORT_STMT_ERROR(stmt->stmt);

	if (rc) {
		efree(bind);
		efree(args);
		php_clear_stmt_bind(stmt);
		RETURN_FALSE;
	}

	stmt->result.var_cnt = var_cnt;
	stmt->result.vars = (zval **)safe_emalloc((var_cnt), sizeof(zval), 0);
	for (i = start; i < var_cnt+start; i++) {
		ofs = i-start;
		ZVAL_ADDREF(*args[i]);
		stmt->result.vars[ofs] = *args[i];
	}

	efree(args);
	efree(bind);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_change_user(object link, string user, string password, string database)
   Change logged-in user of the active connection */
PHP_FUNCTION(mysqli_change_user)
{
	MY_MYSQL	*mysql;
	zval  		*mysql_link = NULL;
	char  		*user, *password, *dbname;
	int   		user_len, password_len, dbname_len;
	ulong		rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Osss", &mysql_link, mysqli_link_class_entry, &user, &user_len, &password, &password_len, &dbname, &dbname_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	rc = mysql_change_user(mysql->mysql, user, password, dbname);
	MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);

	if (rc) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string mysqli_character_set_name(object link)
   Returns the name of the character set used for this connection */
PHP_FUNCTION(mysqli_character_set_name)
{
	MY_MYSQL 	*mysql;
	zval  		*mysql_link;
	char		*charsetname;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	charsetname = (char *)mysql_character_set_name(mysql->mysql);

	RETURN_STRING(charsetname, 1);
}
/* }}} */

/* {{{ proto bool mysqli_close(object link)
   Close connection */
PHP_FUNCTION(mysqli_close)
{
	zval 		*mysql_link;
	MY_MYSQL 	*mysql;
	
	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	mysql_close(mysql->mysql);

	MYSQLI_CLEAR_RESOURCE(&mysql_link);	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_commit(object link)
   Commit outstanding actions and close transaction */
PHP_FUNCTION(mysqli_commit)
{
	MY_MYSQL 	*mysql;
	zval 		*mysql_link;
	ulong		rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");
	rc = mysql_commit(mysql->mysql);
	RETURN_BOOL(rc);
}
/* }}} */

/* {{{ proto bool mysqli_data_seek(object result, int offset)
   Move internal result pointer */
PHP_FUNCTION(mysqli_data_seek)
{
	MYSQL_RES	*result;
	zval  		*mysql_result;
	long  		offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &mysql_result, mysqli_result_class_entry, &offset) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result"); 

	if (result->handle && result->handle->status == MYSQL_STATUS_USE_RESULT) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Function cannot be used with MYSQL_USE_RESULT");
		RETURN_FALSE;
	}

	if (offset < 0 || offset >= result->row_count) {
	   RETURN_FALSE;
	}

	mysql_data_seek(result, offset);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void mysqli_debug(string debug)
*/
PHP_FUNCTION(mysqli_debug)
{
	char		*debug;
	int			debug_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &debug, &debug_len) == FAILURE) {
		return;
	}
	
	mysql_debug(debug);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_dump_debug_info(object link)
*/
PHP_FUNCTION(mysqli_dump_debug_info)
{
	MY_MYSQL 	*mysql;
	zval  		*mysql_link;
	ulong		rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	rc = mysql_dump_debug_info(mysql->mysql);

	if (rc) {
		RETURN_FALSE;
	}
	RETURN_TRUE;	
}
/* }}} */

/* {{{ proto int mysqli_errno(object link)
   Returns the numerical value of the error message from previous MySQL operation */
PHP_FUNCTION(mysqli_errno)
{
	MY_MYSQL 	*mysql;
	zval  		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");
	RETURN_LONG(mysql_errno(mysql->mysql));
}
/* }}} */

/* {{{ proto string mysqli_error(object link)
   Returns the text of the error message from previous MySQL operation */
PHP_FUNCTION(mysqli_error) 
{
	MY_MYSQL 	*mysql;
	zval  		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");
	RETURN_STRING((char *)mysql_error(mysql->mysql),1);
}
/* }}} */

/* {{{ proto bool mysqli_stmt_execute(object stmt)
   Execute a prepared statement */
PHP_FUNCTION(mysqli_stmt_execute)
{
	MY_STMT 		*stmt;
	zval 			*mysql_stmt;
	unsigned int 	i;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt"); 
	
	for (i = 0; i < stmt->param.var_cnt; i++) {		
		if (stmt->param.vars[i]) {
			stmt->param.is_null[i] = (stmt->param.vars[i]->type == IS_NULL);

			switch (stmt->stmt->params[i].buffer_type) {
				case MYSQL_TYPE_VAR_STRING:
					convert_to_string_ex(&stmt->param.vars[i]);
					stmt->stmt->params[i].buffer = Z_STRVAL_PP(&stmt->param.vars[i]);
					stmt->stmt->params[i].buffer_length = strlen(Z_STRVAL_PP(&stmt->param.vars[i]));
					break;
				case MYSQL_TYPE_DOUBLE:
					convert_to_double_ex(&stmt->param.vars[i]);
					stmt->stmt->params[i].buffer = (gptr)&Z_LVAL_PP(&stmt->param.vars[i]);
					break;
				case MYSQL_TYPE_LONG:
					convert_to_long_ex(&stmt->param.vars[i]);
					stmt->stmt->params[i].buffer = (gptr)&Z_LVAL_PP(&stmt->param.vars[i]);
					break;
				default:
					break;
			}	
		}
	}
	if (mysql_stmt_execute(stmt->stmt)) {
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
		RETURN_FALSE;
	}
	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
		php_mysqli_report_index(stmt->query, stmt->stmt->mysql->server_status TSRMLS_CC);
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed mysqli_stmt_fetch(object stmt)
   Fetch results from a prepared statement into the bound variables */
PHP_FUNCTION(mysqli_stmt_fetch)
{
	MY_STMT 		*stmt;
	zval 			*mysql_stmt;
	unsigned int 	i;
	ulong 			ret;
	long			lval;
	double			dval;
	my_ulonglong	llval;
	

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt"); 
	
	/* reset buffers */


	for (i = 0; i < stmt->result.var_cnt; i++) {
		if (stmt->result.buf[i].type == IS_STRING) {
			memset(stmt->result.buf[i].val, 0, stmt->result.buf[i].buflen);
		}
	}
	if (!(ret = mysql_stmt_fetch(stmt->stmt))) {
		for (i = 0; i < stmt->result.var_cnt; i++) {
			if (stmt->result.vars[i]->type == IS_STRING && stmt->result.vars[i]->value.str.len) {
		        efree(stmt->result.vars[i]->value.str.val);
			}
			if (!stmt->result.is_null[i]) {
				switch (stmt->result.buf[i].type) {
					case IS_LONG:
						memcpy(&lval, stmt->result.buf[i].val, sizeof(long));
						ZVAL_LONG(stmt->result.vars[i], lval);
						break;
					case IS_DOUBLE:
						memcpy(&dval, stmt->result.buf[i].val, sizeof(double));
						ZVAL_DOUBLE(stmt->result.vars[i], dval);
						break;
					case IS_STRING:
						if (stmt->stmt->bind[i].buffer_type == MYSQL_TYPE_LONGLONG) {
							char tmp[50];
							memcpy (&llval, stmt->result.buf[i].val, sizeof(my_ulonglong));
							if (llval != (long)llval) {
								/* even though lval is declared as unsigned, the value
								 * may be negative. Therefor we cannot use %llu and must
								 * use %lld.
								 */
								sprintf((char *)&tmp, "%lld", llval);
								ZVAL_STRING(stmt->result.vars[i], tmp, 1);
							} else {
								ZVAL_LONG(stmt->result.vars[i], llval);
							}
						} else {
							ZVAL_STRING(stmt->result.vars[i], stmt->result.buf[i].val, 1); 
						}
						break;
					default:
						break;	
				}
			} else {
				stmt->result.vars[i]->type = IS_NULL;
			}
		}
	} else {
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
	}

	switch (ret) {
		case 0:
			RETURN_TRUE;
		break;
		case 1:
			RETURN_FALSE;
		break;
		default:
			RETURN_NULL();
		break;
	}
}
/* }}} */

/* {{{ proto mixed mysqli_fetch_field (object result)
   Get column information from a result and return as an object */
PHP_FUNCTION(mysqli_fetch_field) 
{
	MYSQL_RES 	*result;
	zval  		*mysql_result;
	MYSQL_FIELD *field;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result"); 

	if (!(field = mysql_fetch_field(result))) {
		RETURN_FALSE;
	}

	object_init(return_value);

	add_property_string(return_value, "name",(field->name ? field->name : ""), 1);
	add_property_string(return_value, "orgname",(field->org_name ? field->org_name : ""), 1);
	add_property_string(return_value, "table",(field->table ? field->table : ""), 1);
	add_property_string(return_value, "orgtable",(field->org_table ? field->org_table : ""), 1);
	add_property_string(return_value, "def",(field->def ? field->def : ""), 1);
	add_property_long(return_value, "max_length", field->max_length);
	add_property_long(return_value, "flags", field->flags);
	add_property_long(return_value, "type", field->type);
	add_property_long(return_value, "decimals", field->decimals);
}
/* }}} */

/* {{{ proto mixed mysqli_fetch_fields (object result)
   Return array of objects containing field meta-data */
PHP_FUNCTION(mysqli_fetch_fields) 
{
	MYSQL_RES	*result;
	zval  		*mysql_result;
	MYSQL_FIELD	*field;
	zval 		*obj;

	unsigned int i;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result"); 

	if (!(field = mysql_fetch_field(result))) {
		RETURN_FALSE;
	}

	array_init(return_value);

	for (i = 0; i < mysql_num_fields(result); i++) {
		field = mysql_fetch_field_direct(result, i);


		MAKE_STD_ZVAL(obj);
		object_init(obj);

		add_property_string(obj, "name",(field->name ? field->name : ""), 1);
		add_property_string(obj, "orgname",(field->org_name ? field->org_name : ""), 1);
		add_property_string(obj, "table",(field->table ? field->table : ""), 1);
		add_property_string(obj, "orgtable",(field->org_table ? field->org_table : ""), 1);
		add_property_string(obj, "def",(field->def ? field->def : ""), 1);
		add_property_long(obj, "max_length", field->max_length);
		add_property_long(obj, "flags", field->flags);
		add_property_long(obj, "type", field->type);
		add_property_long(obj, "decimals", field->decimals);

		add_index_zval(return_value, i, obj);
	}
}
/* }}} */

/* {{{ proto mixed mysqli_fetch_field_direct (object result, int offset)
   Fetch meta-data for a single field */
PHP_FUNCTION(mysqli_fetch_field_direct) 
{
	MYSQL_RES	*result;
	zval		*mysql_result;
	MYSQL_FIELD *field;
	int 		offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &mysql_result, mysqli_result_class_entry, &offset) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result"); 

	if (!(field = mysql_fetch_field_direct(result,offset))) {
		RETURN_FALSE;
	}

	object_init(return_value);

	add_property_string(return_value, "name",(field->name ? field->name : ""), 1);
	add_property_string(return_value, "orgname",(field->org_name ? field->org_name : ""), 1);
	add_property_string(return_value, "table",(field->table ? field->table : ""), 1);
	add_property_string(return_value, "orgtable",(field->org_table ? field->org_table : ""), 1);
	add_property_string(return_value, "def",(field->def ? field->def : ""), 1);
	add_property_long(return_value, "max_length", field->max_length);
	add_property_long(return_value, "flags", field->flags);
	add_property_long(return_value, "type", field->type);
	add_property_long(return_value, "decimals", field->decimals);
}
/* }}} */

/* {{{ proto mixed mysqli_fetch_lengths (object result)
   Get the length of each output in a result */
PHP_FUNCTION(mysqli_fetch_lengths) 
{
	MYSQL_RES		*result;
	zval			*mysql_result;
	unsigned int	i;
	unsigned long	*ret;
	
	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result"); 

	if (!(ret = mysql_fetch_lengths(result))) {
		RETURN_FALSE;
	}

	array_init(return_value);

	for (i = 0; i < mysql_num_fields(result); i++) {
		add_index_long(return_value, i, ret[i]);	
	}
}
/* }}} */

/* {{{ proto array mysqli_fetch_row (object result)
   Get a result row as an enumerated array */
PHP_FUNCTION(mysqli_fetch_row) 
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQLI_NUM, 0);
}
/* }}} */

/* {{{ proto int mysqli_field_count(object link)
   Fetch the number of fields returned by the last query for the given link
*/
PHP_FUNCTION(mysqli_field_count) 
{
	MY_MYSQL 	*mysql;
	zval  		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_LONG(mysql_field_count(mysql->mysql));
}
/* }}} */

/* {{{ proto int mysqli_field_seek(object result, int fieldnr)
   Set result pointer to a specified field offset
*/
PHP_FUNCTION(mysqli_field_seek)
{
	MYSQL_RES		*result;
	zval			*mysql_result;
	unsigned int	fieldnr;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &mysql_result, mysqli_result_class_entry, &fieldnr) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result"); 

	if (fieldnr < 0 || fieldnr >= mysql_num_fields(result)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Field offset is invalid for resultset");
		RETURN_FALSE; 
	}
	
	mysql_field_seek(result, fieldnr);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int mysqli_field_tell(object result)
   Get current field offset of result pointer */
PHP_FUNCTION(mysqli_field_tell)
{
	MYSQL_RES	*result;
	zval		*mysql_result;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result"); 
	
	RETURN_LONG(mysql_field_tell(result));
}
/* }}} */

/* {{{ proto void mysqli_free_result(object result)
   Free query result memory for the given result handle */
PHP_FUNCTION(mysqli_free_result) 
{
	MYSQL_RES	*result;
	zval		*mysql_result;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result"); 

	mysql_free_result(result);
	MYSQLI_CLEAR_RESOURCE(&mysql_result);	

	return;
}
/* }}} */

/* {{{ proto string mysqli_get_client_info(void) 
   Get MySQL client info */
PHP_FUNCTION(mysqli_get_client_info)
{
	RETURN_STRING((char *)mysql_get_client_info(), 1);
}
/* }}} */

/* {{{ proto int mysqli_get_client_version(void) 
   Get MySQL client info */
PHP_FUNCTION(mysqli_get_client_version)
{
	RETURN_LONG((long)mysql_get_client_version());
}
/* }}} */

/* {{{ proto string mysqli_get_host_info (object link) 
   Get MySQL host info */
PHP_FUNCTION(mysqli_get_host_info)
{
	MY_MYSQL	*mysql;
	zval  		*mysql_link = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_STRING((mysql->mysql->host_info) ? mysql->mysql->host_info : empty_string, 1);
}
/* }}} */

/* {{{ proto int mysqli_get_proto_info(object link)
   Get MySQL protocol information */
PHP_FUNCTION(mysqli_get_proto_info)
{
	MY_MYSQL 	*mysql;
	zval  		*mysql_link = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_LONG(mysql_get_proto_info(mysql->mysql));
}
/* }}} */

/* {{{ proto string mysqli_get_server_info(object link) 
   Get MySQL server info */
PHP_FUNCTION(mysqli_get_server_info)
{
	MY_MYSQL	*mysql;
	zval	  	*mysql_link = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_STRING((char *)mysql_get_server_info(mysql->mysql), 1);
}

/* }}} */

/* {{{ proto int mysqli_get_server_version(object link) 
   Return the MySQL version for the server referenced by the given link */
PHP_FUNCTION(mysqli_get_server_version)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_LONG(mysql_get_server_version(mysql->mysql));
}
/* }}} */

/* {{{ proto string mysqli_info(object link)
   Get information about the most recent query */
PHP_FUNCTION(mysqli_info)
{
	MY_MYSQL	*mysql;
	zval  		*mysql_link = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_STRING((mysql->mysql->info) ? mysql->mysql->info : empty_string, 1);
}
/* }}} */

/* {{{ proto resource mysqli_init(void)
   Initialize mysqli and return a resource for use with mysql_real_connect */
PHP_FUNCTION(mysqli_init)
{
	MY_MYSQL *mysql = (MY_MYSQL *)calloc(1, sizeof(MY_MYSQL));

	MYSQLI_RESOURCE *mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));

	if (!(mysql->mysql = mysql_init(NULL))) {
		efree(mysql);
		RETURN_FALSE;
	}
	mysqli_resource->ptr = (void *)mysql;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_link_class_entry);	
}
/* }}} */

/* {{{ proto mixed mysqli_insert_id(object link)
   Get the ID generated from the previous INSERT operation */
PHP_FUNCTION(mysqli_insert_id)
{
	MY_MYSQL		*mysql;
	my_ulonglong 	rc;
	zval  			*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");
	rc = mysql_insert_id(mysql->mysql);
	MYSQLI_RETURN_LONG_LONG(rc)
}
/* }}} */

/* {{{ proto bool mysqli_kill(object link, int processid)
   Kill a mysql process on the server */
PHP_FUNCTION(mysqli_kill)
{
	MY_MYSQL 	*mysql;
	zval  		*mysql_link;
	int   		processid;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &mysql_link, mysqli_link_class_entry, &processid) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");
	
	if (mysql_kill(mysql->mysql, processid)) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mysqli_set_local_infile_default(object link)
   unsets user defined handler for load local infile command */
PHP_FUNCTION(mysqli_set_local_infile_default)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	int			i;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	for (i=0; i < 3; i++) {
		if (&mysql->callback_func[i]) {
			zval_dtor(&mysql->callback_func[i]);
		}
	}

	mysql_set_local_infile_default(mysql->mysql);	
}
/* }}} */

/* {{{ proto bool mysqli_set_local_infile_handler(object link, callback init_func,
					callback read_func, callback end_func)
   Set callback functions for LOAD DATA LOCAL INFILE */
PHP_FUNCTION(mysqli_set_local_infile_handler)
{
	MY_MYSQL	*mysql;
	zval  		*mysql_link;
	char		*callback_name;
	zval		*callback_func[4];
	int			i;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ozzzz", &mysql_link, mysqli_link_class_entry,
			&callback_func[0], &callback_func[1], &callback_func[2], &callback_func[3]) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	/* check callback functions */
	for (i=0; i < 3; i++) {
		if (!zend_is_callable(callback_func[i], 0, &callback_name)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not a valid callback function %s", callback_name);
			efree(callback_name);
			RETURN_FALSE;		
		}
		efree(callback_name);
	}

	/* save callback functions */
	for (i=0; i < 3; i++) {
		ZVAL_STRING(&mysql->callback_func[i], callback_func[i]->value.str.val, 1);
	}

	/* register internal callback functions */
	mysql_set_local_infile_handler(mysql->mysql, &php_local_infile_init, &php_local_infile_read,
				&php_local_infile_end, &php_local_infile_error, (void *)mysql); 
}
/* }}} */

/* {{{ proto bool mysqli_more_results(object link)
   check if there any more query results from a multi query */
PHP_FUNCTION(mysqli_more_results) {
	MY_MYSQL		*mysql;
	zval			*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	if (!mysql_more_results(mysql->mysql)) {
		RETURN_FALSE;
	}	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_next_result(object link)
   read next result from multi_query */
PHP_FUNCTION(mysqli_next_result) {
	MY_MYSQL		*mysql;
	zval			*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	if (mysql_next_result(mysql->mysql)) {
		RETURN_FALSE;
	}	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int mysqli_num_fields(object result)
   Get number of fields in result */
PHP_FUNCTION(mysqli_num_fields)
{
	MYSQL_RES	*result;
	zval		*mysql_result;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result"); 

	RETURN_LONG(mysql_num_fields(result));
}
/* }}} */

/* {{{ proto mixed mysqli_num_rows(object result)
   Get number of rows in result */
PHP_FUNCTION(mysqli_num_rows)
{
	MYSQL_RES	*result;
	zval		*mysql_result;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result"); 

	if (result->handle && result->handle->status == MYSQL_STATUS_USE_RESULT) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Function cannot be used with MYSQL_USE_RESULT");
		RETURN_LONG(0);
	}

	MYSQLI_RETURN_LONG_LONG(mysql_num_rows(result));
}
/* }}} */

/* {{{ proto bool mysqli_options(object link, int flags, mixed values)
   Set options */
PHP_FUNCTION(mysqli_options)
{
	MY_MYSQL 		*mysql;
	zval  			*mysql_link = NULL;
	zval  			*mysql_value;
	long  			mysql_option;
	unsigned int 	l_value;
	long  			ret;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Olz", &mysql_link, mysqli_link_class_entry, &mysql_option, &mysql_value) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	switch (Z_TYPE_PP(&mysql_value)) {
		case IS_STRING:
			ret = mysql_options(mysql->mysql, mysql_option, Z_STRVAL_PP(&mysql_value));
			break;
		default:
			convert_to_long_ex(&mysql_value);
			l_value = Z_LVAL_PP(&mysql_value);
			ret = mysql_options(mysql->mysql, mysql_option, (char *)&l_value);
			break;
	}

	if (ret != 0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}   
/* }}} */


/* {{{ proto bool mysqli_ping(object link)
   Ping a server connection or reconnect if there is no connection */
PHP_FUNCTION(mysqli_ping)
{
	MY_MYSQL 	*mysql;
	zval  		*mysql_link;
	long		rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");
	rc = mysql_ping(mysql->mysql);
	MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
	if (rc) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed mysqli_prepare(object link, string query)
   Prepare a SQL statement for execution */
PHP_FUNCTION(mysqli_prepare)
{
	MY_MYSQL		*mysql;
	MY_STMT 		*stmt;
	char			*query = NULL;
	unsigned int	query_len;
	zval			*mysql_link;
	MYSQLI_RESOURCE *mysqli_resource; 

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os",&mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	stmt = (MY_STMT *)ecalloc(1,sizeof(MY_STMT));

    if ((stmt->stmt = mysql_stmt_init(mysql->mysql))) {
		if (mysql_stmt_prepare(stmt->stmt, query, query_len)) {
			mysql_stmt_close(stmt->stmt);
			stmt->stmt = NULL;
		}
	} 
	
	if (!stmt->stmt) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		efree(stmt);
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)stmt;
	stmt->query = (query_len) ? (char *)emalloc(query_len + 1) : NULL;
	strcpy(stmt->query, query);
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_stmt_class_entry);
}
/* }}} */

/* {{{ proto bool mysqli_real_connect(object link [,string hostname [,string username [,string passwd [,string dbname [,int port [,string socket [,int flags]]]]]]])
   Open a connection to a mysql server */ 
PHP_FUNCTION(mysqli_real_connect)
{
	MY_MYSQL 		*mysql;
	char 			*hostname = NULL, *username=NULL, *passwd=NULL, *dbname=NULL, *socket=NULL;
	unsigned int 	hostname_len, username_len, passwd_len, dbname_len, socket_len;
	unsigned int 	port=0, flags=0;
	zval			*mysql_link;
	zval  			*object = getThis();

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|sssslsl", &mysql_link, mysqli_link_class_entry,
		&hostname, &hostname_len, &username, &username_len, &passwd, &passwd_len, &dbname, &dbname_len, &port, &socket, &socket_len,
		&flags) == FAILURE) {
		return;
	}

	if (!socket_len) {
		socket = NULL;
	}

	/* TODO: safe mode handling */
	if (PG(sql_safe_mode)) {
	} else {
		if (!passwd) {
			passwd = MyG(default_pw);
			if (!username){
				username = MyG(default_user);
				if (!hostname) {
					hostname = MyG(default_host);
				}
			}
		}
	}	

	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	/* remove some insecure options */
	flags ^= CLIENT_MULTI_STATEMENTS;   /* don't allow multi_queries via connect parameter */
	if (PG(open_basedir) && strlen(PG(open_basedir))) {
		flags ^= CLIENT_LOCAL_FILES;
	}

	if (mysql_real_connect(mysql->mysql,hostname,username,passwd,dbname,port,socket,flags) == NULL) {
		
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		php_mysqli_set_error(mysql_errno(mysql->mysql), (char *) mysql_error(mysql->mysql) TSRMLS_CC);

		if (!(MyG(report_mode) & MYSQLI_REPORT_ERROR)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", mysql_error(mysql->mysql));
		}
		RETURN_FALSE;
	}
	php_mysqli_set_error(mysql_errno(mysql->mysql), (char *)mysql_error(mysql->mysql) TSRMLS_CC);

	mysql->mysql->reconnect = MyG(reconnect);
	
	if (object) {
		((mysqli_object *) zend_object_store_get_object(object TSRMLS_CC))->valid = 1;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_real_query(object link, string query)
   Binary-safe version of mysql_query() */
PHP_FUNCTION(mysqli_real_query)
{
	MY_MYSQL		*mysql;
	zval			*mysql_link;
	char			*query = NULL;
	unsigned int 	query_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	MYSQLI_DISABLE_MQ; /* disable multi statements/queries */

	if (mysql_real_query(mysql->mysql, query, query_len)) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}

	if (!mysql_field_count(mysql->mysql)) {
		if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
			php_mysqli_report_index(query, mysql->mysql->server_status TSRMLS_CC);
		}
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string mysqli_real_escape_string(object link, string escapestr) 
   Escapes special characters in a string for use in a SQL statement, taking into account the current charset of the connection */
PHP_FUNCTION(mysqli_real_escape_string) {
	MY_MYSQL	*mysql;
	zval		*mysql_link = NULL;
	char		*escapestr, *newstr;
	int			escapestr_len, newstr_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &escapestr, &escapestr_len) == FAILURE) {
		return;
	}	
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	newstr = safe_emalloc(2, escapestr_len, 1);
	newstr_len = mysql_real_escape_string(mysql->mysql, newstr, escapestr, escapestr_len);
	newstr = erealloc(newstr, newstr_len + 1);
		
	RETURN_STRING(newstr, 0);
}
/* }}} */

/* {{{ proto bool mysqli_rollback(object link)
   Undo actions from current transaction */
PHP_FUNCTION(mysqli_rollback)
{
	MY_MYSQL	*mysql;
	zval  		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_BOOL(mysql_rollback(mysql->mysql));
}
/* }}} */

/* {{{ proto bool mysqli_send_long_data(object stmt, int param_nr, string data)
*/
PHP_FUNCTION(mysqli_stmt_send_long_data)
{
	MY_STMT *stmt;
	zval  	*mysql_stmt;
	char	*data;
	long	param_nr, data_len;


	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ols", &mysql_stmt, mysqli_stmt_class_entry, &param_nr, &data, &data_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt"); 

	if (param_nr < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid parameter number");
		RETURN_FALSE;
	}
	if (mysql_stmt_send_long_data(stmt->stmt, param_nr, data, data_len)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

#ifdef HAVE_EMBEDDED_MYSQLI
/* {{{ proto bool mysqli_server_init(void)
 initialize embedded server */
PHP_FUNCTION(mysqli_server_init)
{
	zval	*server;
	zval	*groups;

	if (MyG(embedded)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Embedded server already initialized.");
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|aa", &server, &groups) == FAILURE) {
		return;
	}

	if (mysql_server_init(0, NULL, NULL)) {
			RETURN_FALSE;
	}
	MyG(embedded) = 1;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void mysqli_server_end(void)
*/
PHP_FUNCTION(mysqli_server_end)
{
	mysql_server_end();
}
/* }}} */
#endif

/* {{{ proto mixed mysqli_stmt_affected_rows(object stmt)
   Return the number of rows affected in the last query for the given link */
PHP_FUNCTION(mysqli_stmt_affected_rows)
{
	MY_STMT 		*stmt;
	zval    		*mysql_stmt;
	my_ulonglong	rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt");

	rc = mysql_stmt_affected_rows(stmt->stmt);
	MYSQLI_RETURN_LONG_LONG(rc)
}
/* }}} */

/* {{{ proto bool mysqli_stmt_close(object stmt) 
   Close statement */
PHP_FUNCTION(mysqli_stmt_close)
{
	MY_STMT 	*stmt;
	zval    	*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt"); 
	mysql_stmt_close(stmt->stmt);
	stmt->stmt = NULL;
	php_clear_stmt_bind(stmt); 
	MYSQLI_CLEAR_RESOURCE(&mysql_stmt);
	RETURN_TRUE;

}
/* }}} */

/* {{{ proto void mysqli_stmt_data_seek(object stmt, int offset)
   Move internal result pointer */
PHP_FUNCTION(mysqli_stmt_data_seek)
{
	MY_STMT 	*stmt;
	zval  		*mysql_stmt;
	long  		offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &mysql_stmt, mysqli_stmt_class_entry, &offset) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt");

	mysql_stmt_data_seek(stmt->stmt, offset);
	return;
}
/* }}} */

/* {{{ proto int mysqli_stmt_field_count(object stmt) {
   Return the number of result columns for the given statement */
PHP_FUNCTION(mysqli_stmt_field_count)
{
	MY_STMT 	*stmt;
	zval		*mysql_stmt;
	
	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt"); 
		
	RETURN_LONG(mysql_stmt_field_count(stmt->stmt));
}
/* }}} */

/* {{{ proto void mysqli_stmt_free_result(object stmt)
   Free stored result memory for the given statement handle */
PHP_FUNCTION(mysqli_stmt_free_result) 
{
	MY_STMT 		*stmt;
	zval    		*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt");

	mysql_stmt_free_result(stmt->stmt);

	return;
}
/* }}} */

/* {{{ proto mixed mysqli_stmt_insert_id(object stmt)
   Get the ID generated from the previous INSERT operation */
PHP_FUNCTION(mysqli_stmt_insert_id)
{
	MY_STMT		 	*stmt;
	my_ulonglong 	rc;
	zval  			*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt");
	rc = mysql_stmt_insert_id(stmt->stmt);
	MYSQLI_RETURN_LONG_LONG(rc)
}
/* }}} */

/* {{{ proto int mysqli_stmt_param_count(object stmt) {
   Return the number of parameter for the given statement */
PHP_FUNCTION(mysqli_stmt_param_count)
{
	MY_STMT 	*stmt;
	zval		*mysql_stmt;
	
	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt"); 
		
	RETURN_LONG(mysql_stmt_param_count(stmt->stmt));
}
/* }}} */

/* {{{ proto void mysqli_stmt_reset(object stmt)
   reset a prepared statement */
PHP_FUNCTION(mysqli_stmt_reset) 
{
	MY_STMT 		*stmt;
	zval    		*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt");

	mysql_stmt_reset(stmt->stmt);

	return;
}
/* }}} */

/* {{{ proto mixed mysqli_stmt_num_rows(object stmt)
   Return the number of rows in statements result set */
PHP_FUNCTION(mysqli_stmt_num_rows)
{
	MY_STMT 		*stmt;
	zval    		*mysql_stmt;
	my_ulonglong	rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt");

	rc = mysql_stmt_num_rows(stmt->stmt);
	MYSQLI_RETURN_LONG_LONG(rc)
}
/* }}} */

/* {{{ proto string mysqli_select_db(object link, string dbname)
   Select a MySQL database */
PHP_FUNCTION(mysqli_select_db) 
{
	MY_MYSQL	*mysql;
	zval  		*mysql_link;
	char  		*dbname;
	int   		dbname_len;


	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &dbname, &dbname_len) == FAILURE) {
		return;
	} 
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");
	
	if (!mysql_select_db(mysql->mysql, dbname)) {
		RETURN_TRUE;
	}
	
	MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string mysqli_sqlstate(object link)
   Returns the SQLSTATE error from previous MySQL operation */
PHP_FUNCTION(mysqli_sqlstate) 
{
	MY_MYSQL 	*mysql;
	zval  		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");
	RETURN_STRING((char *)mysql_sqlstate(mysql->mysql),1);
}
/* }}} */

/* {{{ proto bool mysqli_ssl_set(object link ,string key ,string cert ,string ca ,string capath ,string cipher])
*/
PHP_FUNCTION(mysqli_ssl_set)
{
	MY_MYSQL	*mysql;
	zval  		*mysql_link;
	char  		*ssl_parm[5];
	int			ssl_parm_len[5], i;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Osssss", &mysql_link, mysqli_link_class_entry, &ssl_parm[0], &ssl_parm_len[0], &ssl_parm[1], &ssl_parm_len[1], &ssl_parm[2], &ssl_parm_len[2], &ssl_parm[3], &ssl_parm_len[3], &ssl_parm[4], &ssl_parm_len[4])   == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	for (i=0; i < 5; i++) {
		if (!ssl_parm_len[i]) {
			ssl_parm[i] = NULL;
		}
	}

	mysql_ssl_set(mysql->mysql, ssl_parm[0], ssl_parm[1], ssl_parm[2], ssl_parm[3], ssl_parm[4]);

	RETURN_TRUE;
}
/* }}} */
  
/* {{{ proto mixed mysqli_stat(object link) 
   Get current system status */
PHP_FUNCTION(mysqli_stat)
{
	MY_MYSQL 	*mysql;
	zval  		*mysql_link;
	char		*stat;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	if ((stat = (char *)mysql_stat(mysql->mysql))) {
		RETURN_STRING(stat, 1);
	}
	RETURN_FALSE;
}

/* }}} */
 
/* {{{ proto int mysqli_stmt_attr_set(object stmt, long attr, bool mode)
*/
PHP_FUNCTION(mysqli_stmt_attr_set)
{
	MY_STMT	*stmt;
	zval 	*mysql_stmt;
	ulong	mode;
	ulong	attr;
	int		rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Olb", &mysql_stmt, mysqli_stmt_class_entry, &attr, &mode) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt"); 

	if (rc = mysql_stmt_attr_set(stmt->stmt, attr, (void *)&mode)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int mysqli_stmt_attr_get(object stmt, long attr)
*/
PHP_FUNCTION(mysqli_stmt_attr_get)
{
	MY_STMT	*stmt;
	zval 	*mysql_stmt;
	ulong	value;
	ulong	attr;
	int		rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &mysql_stmt, mysqli_stmt_class_entry, &attr) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt"); 

	if (rc = mysql_stmt_attr_get(stmt->stmt, attr, &value)) {
		RETURN_FALSE;
	}
	RETURN_LONG(value);
}
/* }}} */

/* {{{ proto int mysqli_stmt_errno(object stmt)
*/
PHP_FUNCTION(mysqli_stmt_errno)
{
	MY_STMT	*stmt;
	zval 	*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt"); 

	RETURN_LONG(mysql_stmt_errno(stmt->stmt));
}
/* }}} */

/* {{{ proto string mysqli_stmt_error(object stmt)
*/
PHP_FUNCTION(mysqli_stmt_error) 
{
	MY_STMT	*stmt;
	zval 	*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt"); 
	
	RETURN_STRING((char *)mysql_stmt_error(stmt->stmt),1);
}
/* }}} */

/* {{{ proto object mysqli_stmt_init(object link)
   Initialize statement object
*/
PHP_FUNCTION(mysqli_stmt_init)
{
	MY_MYSQL		*mysql;
	MY_STMT 		*stmt;
	zval			*mysql_link;
	MYSQLI_RESOURCE *mysqli_resource; 

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O",&mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	stmt = (MY_STMT *)ecalloc(1,sizeof(MY_STMT));

	if (!(stmt->stmt = mysql_stmt_init(mysql->mysql))) {
		efree(stmt);
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)stmt;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_stmt_class_entry);
}
/* }}} */

/* {{{ proto bool mysqli_stmt_prepare(object link, string query)
   prepare server side statement with query
*/
PHP_FUNCTION(mysqli_stmt_prepare)
{
	MY_STMT	*stmt;
	zval 	*mysql_stmt;
	char	*query;
	int		query_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_stmt, mysqli_stmt_class_entry, &query, &query_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt");

	if (mysql_stmt_prepare(stmt->stmt, query, query_len)) {
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed mysqli_stmt_result_metadata(object stmt)
   return result set from statement */
PHP_FUNCTION(mysqli_stmt_result_metadata)
{
	MY_STMT		*stmt;
	MYSQL_RES		*result;
	zval 			*mysql_stmt;
	MYSQLI_RESOURCE	*mysqli_resource;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt"); 

	if (!(result = mysql_stmt_result_metadata(stmt->stmt))){
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_result_class_entry);	
}
/* }}} */

/* {{{ proto bool mysqli_stmt_store_result(stmt)
*/
PHP_FUNCTION(mysqli_stmt_store_result)
{
	MY_STMT *stmt;
	zval 	*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt"); 
	
	if (mysql_stmt_store_result(stmt->stmt)){
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string mysqli_stmt_sqlstate(object stmt)
*/
PHP_FUNCTION(mysqli_stmt_sqlstate) 
{
	MY_STMT *stmt;
	zval 	*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt"); 
	
	RETURN_STRING((char *)mysql_stmt_sqlstate(stmt->stmt),1);
}
/* }}} */

/* {{{ proto object mysqli_store_result(object link)
   Buffer result set on client */
PHP_FUNCTION(mysqli_store_result)
{
	MY_MYSQL 		*mysql;
	MYSQL_RES 		*result;
	zval  			*mysql_link;
	MYSQLI_RESOURCE	*mysqli_resource;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	if (!(result = mysql_store_result(mysql->mysql))) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}
	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
		php_mysqli_report_index("from previous query", mysql->mysql->server_status TSRMLS_CC);
	}
	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_result_class_entry);	
}
/* }}} */

/* {{{ proto int mysqli_thread_id(object link)
   Return the current thread ID */
PHP_FUNCTION(mysqli_thread_id)
{
	MY_MYSQL 	*mysql;
	zval  		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_LONG(mysql_thread_id(mysql->mysql));
}
/* }}} */

/* {{{ proto bool mysqli_thread_safe(void)
   Return whether thread safety is given or not */
PHP_FUNCTION(mysqli_thread_safe)
{
	RETURN_BOOL(mysql_thread_safe());
}

/* }}} */

/* {{{ proto mixed mysqli_use_result(object link)
   Directly retrieve query results - do not buffer results on client side */
PHP_FUNCTION(mysqli_use_result)
{
	MY_MYSQL 		*mysql;
	MYSQL_RES 		*result;
	zval  			*mysql_link;
	MYSQLI_RESOURCE *mysqli_resource;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	if (!(result = mysql_use_result(mysql->mysql))) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}

	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
		php_mysqli_report_index("from previous query", mysql->mysql->server_status TSRMLS_CC);
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_result_class_entry);	
}
/* }}} */

/* {{{ proto int mysqli_warning_count (object link)
   Return number of warnings from the last query for the given link */
PHP_FUNCTION(mysqli_warning_count)
{
	MY_MYSQL 	*mysql;
	zval  		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_LONG(mysql_warning_count(mysql->mysql));
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
