/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
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


/* {{{ proto mixed mysqli_affected_rows(resource link)
   Get number of affected rows in previous MySQL operation */
PHP_FUNCTION(mysqli_affected_rows)
{
	MYSQL 			*mysql;
	zval  			*mysql_link;
	PR_MYSQL		*prmysql;
	my_ulonglong	rc;
	PR_COMMAND		*prcommand;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;	/* Should this be RETURN_FALSE? */
	}

	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	MYSQLI_PROFILER_COMMAND_START(prcommand, prmysql);
	rc = mysql_affected_rows(mysql);
	MYSQLI_PROFILER_COMMAND_RETURNLONG(prcommand, (long)rc);
	MYSQLI_RETURN_LONG_LONG(rc);
}
/* }}} */

/* {{{ proto bool mysqli_autocommit(resource link, bool mode)
   Turn auto commit on or of */
PHP_FUNCTION(mysqli_autocommit)
{
	MYSQL 			*mysql;
	zval  			*mysql_link;
	PR_MYSQL		*prmysql;
	PR_COMMAND		*prcommand;
	unsigned long	rc;
	unsigned long	automode;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ob", &mysql_link, mysqli_link_class_entry, &automode) == FAILURE) {
		return;	/* Should this be RETURN_FALSE? */
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	MYSQLI_PROFILER_COMMAND_START(prcommand, prmysql);
	rc = (long) mysql_autocommit(mysql, automode);
	MYSQLI_PROFILER_COMMAND_RETURNLONG(prcommand, rc);

	RETURN_BOOL(rc);
}
/* }}} */

/* {{{ proto bool mysqli_bind_param(resource stmt, mixed variable, int type [,mixed,....])
   Bind variables to a prepared statement as parameters */
PHP_FUNCTION(mysqli_bind_param)
{
	zval 			***args;
	int     		argc = ZEND_NUM_ARGS();
	int 	    	i;
	int				num_vars;
	int				start = 0;
	int				ofs;
	STMT			*stmt;
	MYSQL_BIND		*bind;
	zval			**object;
	PR_STMT			*prstmt;
	PR_COMMAND		*prcommand;
	unsigned long	rc;

	/* calculate and check number of parameters */
	num_vars = argc;
	if (!getThis()) {
		/* ignore handle parameter in procedural interface*/
		--num_vars; 
	}
	if (num_vars % 2) {
		/* we need variable/type pairs */
		WRONG_PARAM_COUNT;
	}
	if (num_vars < 2) {
		/* there has to be at least one pair */
		WRONG_PARAM_COUNT;
	}
	num_vars /= 2;
	   

	args = (zval ***)emalloc(argc * sizeof(zval **));

	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}

	if (!getThis()) {
		if (Z_TYPE_PP(args[0]) != IS_OBJECT) {
			efree(args);
			RETURN_FALSE;
		}
		MYSQLI_FETCH_RESOURCE(stmt, STMT *, prstmt, PR_STMT *, args[0], "mysqli_stmt"); 
		start = 1;
	} else {
		object = &(getThis());	
		MYSQLI_FETCH_RESOURCE(stmt, STMT *, prstmt, PR_STMT *, object, "mysqli_stmt"); 
	}


	/* prevent leak if variables are already bound */
#if HHOLZGRA_0
	/* this would prevent using both bind_param and bind_result on SELECT
		queries so it is disabled for now */
	if (stmt->var_cnt) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Variables already bound");
		efree(args);
		RETURN_FALSE;
	}
#endif

	stmt->is_null = ecalloc(num_vars, sizeof(char));
	bind = (MYSQL_BIND *)ecalloc(num_vars, sizeof(MYSQL_BIND));

	for (i=start; i < num_vars * 2 + start; i+=2) {
		ofs = (i - start) / 2;
		if (!PZVAL_IS_REF(*args[i]) && Z_LVAL_PP(args[i+1]) != MYSQLI_BIND_SEND_DATA) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Parameter %d wasn't passed by reference", i+1);
			efree(bind);
			efree(args);
			RETURN_FALSE;
		}

		/* set specified type */
		switch (Z_LVAL_PP(args[i+1])) {
			case MYSQLI_BIND_DOUBLE:
				bind[ofs].buffer_type = MYSQL_TYPE_DOUBLE;
				bind[ofs].buffer = (gptr)&Z_DVAL_PP(args[i]);
				bind[ofs].is_null = &stmt->is_null[ofs];
				break;

			case MYSQLI_BIND_INT:
				bind[ofs].buffer_type = MYSQL_TYPE_LONG;
				bind[ofs].buffer = (gptr)&Z_LVAL_PP(args[i]);
				bind[ofs].is_null = &stmt->is_null[ofs];
				break;

			case MYSQLI_BIND_SEND_DATA:
				bind[ofs].buffer_type = MYSQL_TYPE_VAR_STRING;
				bind[ofs].is_null = 0;
				bind[ofs].length = 0;
				break;

			case MYSQLI_BIND_STRING:
				bind[ofs].buffer_type = MYSQL_TYPE_VAR_STRING;
				bind[ofs].buffer = NULL; 
				bind[ofs].buffer_length = 0;
				bind[ofs].is_null = &stmt->is_null[ofs];
				break;

			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Undefined fieldtype %d (parameter %d)", Z_LVAL_PP(args[i]), i+1);
				efree(args);
				efree(bind);
				RETURN_FALSE; 
				break;
		}
	}
	
	MYSQLI_PROFILER_COMMAND_START(prcommand, prstmt);
	rc = mysql_bind_param(stmt->stmt, bind);
	MYSQLI_PROFILER_COMMAND_RETURNLONG(prcommand, rc);

	if (rc) {
		efree(args);
		efree(bind);
		RETURN_FALSE;
	}

	stmt->var_cnt = num_vars;
	stmt->type = FETCH_SIMPLE;
	stmt->vars = (zval **)emalloc(num_vars * sizeof(zval));
	for (i = 0; i < num_vars*2; i+=2) {
		if (Z_LVAL_PP(args[i+1+start]) != MYSQLI_BIND_SEND_DATA) {
			ZVAL_ADDREF(*args[i+start]);
			stmt->vars[i/2] = *args[i+start];
		} else {
			stmt->vars[i/2] = NULL;
		}
	}
	efree(args);
	efree(bind);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_bind_result(resource stmt, mixed var, int len [,mixed,int....])
   Bind variables to a prepared statement for result storage */

/* TODO:
   do_alloca, free_alloca
*/

PHP_FUNCTION(mysqli_bind_result)
{
	zval 		***args;
	int     	argc = ZEND_NUM_ARGS();
	zval		**object;
	int     	i;
	int			start = 0;
	int			var_cnt;
	int			ofs;
	long		col_type;
	ulong		rc;
	STMT 		*stmt;
	MYSQL_BIND 	*bind;
	PR_STMT		*prstmt;
	PR_COMMAND	*prcommand;

	if (argc < (getThis() ? 1 : 2))  {
		WRONG_PARAM_COUNT;
	}
	
	args = (zval ***)emalloc(argc * sizeof(zval **));

	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}

	if (!getThis()) {
		if (Z_TYPE_PP(args[0]) != IS_OBJECT) {
			efree(args);
			RETURN_FALSE;
		}
		MYSQLI_FETCH_RESOURCE(stmt, STMT *, prstmt, PR_STMT *, args[0], "mysqli_stmt"); 
		start = 1;
	} else {
		object = &(getThis());	
		MYSQLI_FETCH_RESOURCE(stmt, STMT *, prstmt, PR_STMT *, object, "mysqli_stmt"); 
	}
	
	var_cnt = argc - start;

	/* prevent leak if variables are already bound */
#if HHOLZGRA_0
	/* this would prevent using both bind_param and bind_result on SELECT
		queries so it is disabled for now */
	if (stmt->var_cnt) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Variables already bound");
		efree(args);
		RETURN_FALSE;
	}
#endif

	bind = (MYSQL_BIND *)ecalloc(var_cnt, sizeof(MYSQL_BIND));
	stmt->bind = (BIND_BUFFER *)ecalloc(var_cnt,sizeof(BIND_BUFFER));
	stmt->is_null = (char *)ecalloc(var_cnt, sizeof(char));
	stmt->type = FETCH_RESULT;

	for (i=start; i < var_cnt + start ; i++) {
		ofs = i - start;
		stmt->is_null[ofs] = 0;
		if (!PZVAL_IS_REF(*args[i])) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Parameter %d wasn't passed by reference", i);
			efree(bind);
			efree(args);
			php_clear_stmt_bind(stmt);
			RETURN_FALSE;
		}

		col_type = (stmt->stmt->fields) ? stmt->stmt->fields[ofs].type : MYSQL_TYPE_STRING;

		switch (col_type) {
			case MYSQL_TYPE_DECIMAL:
			case MYSQL_TYPE_DOUBLE:
			case MYSQL_TYPE_FLOAT:
				convert_to_double_ex(args[i]);
				stmt->bind[ofs].type = IS_DOUBLE;
				stmt->bind[ofs].buflen = 0;
				bind[ofs].buffer_type = MYSQL_TYPE_DOUBLE;
				bind[ofs].buffer = (gptr)&Z_DVAL_PP(args[i]);
				bind[ofs].is_null = &stmt->is_null[ofs];
				break;

			case MYSQL_TYPE_SHORT:
			case MYSQL_TYPE_TINY:
			case MYSQL_TYPE_LONG:
			case MYSQL_TYPE_INT24:
			case MYSQL_TYPE_YEAR:
				convert_to_long_ex(args[i]);
				stmt->bind[ofs].type = IS_LONG;
				stmt->bind[ofs].buflen = 0;
				bind[ofs].buffer_type = MYSQL_TYPE_LONG;
				bind[ofs].buffer = (gptr)&Z_LVAL_PP(args[i]);
				bind[ofs].is_null = &stmt->is_null[ofs];
				break;

			case MYSQL_TYPE_LONGLONG:
				stmt->bind[ofs].type = IS_STRING; 
				stmt->bind[ofs].buflen = sizeof(my_ulonglong); 
				stmt->bind[ofs].buffer = (char *)emalloc(stmt->bind[ofs].buflen);
				bind[ofs].buffer_type = MYSQL_TYPE_LONGLONG;
				bind[ofs].buffer = stmt->bind[ofs].buffer;
				bind[ofs].is_null = &stmt->is_null[ofs];
				bind[ofs].buffer_length = stmt->bind[ofs].buflen;
				break;
			case MYSQL_TYPE_DATE:
			case MYSQL_TYPE_TIME:
			case MYSQL_TYPE_DATETIME:
			case MYSQL_TYPE_NEWDATE:
			case MYSQL_TYPE_VAR_STRING:
			case MYSQL_TYPE_STRING:
			case MYSQL_TYPE_BLOB:
			case MYSQL_TYPE_TIMESTAMP:
				stmt->bind[ofs].type = IS_STRING; 
				stmt->bind[ofs].buflen = (stmt->stmt->fields) ? stmt->stmt->fields[ofs].length + 1: 256;
				stmt->bind[ofs].buffer = (char *)emalloc(stmt->bind[ofs].buflen);
				bind[ofs].buffer_type = MYSQL_TYPE_STRING;
				bind[ofs].buffer = stmt->bind[ofs].buffer;
				bind[ofs].is_null = &stmt->is_null[ofs];
				bind[ofs].buffer_length = stmt->bind[ofs].buflen;
				bind[ofs].length = &stmt->bind[ofs].buflen;
				break;
		}
	}

	MYSQLI_PROFILER_COMMAND_START(prcommand, prstmt);
	rc = mysql_bind_result(stmt->stmt, bind);
	MYSQLI_PROFILER_COMMAND_RETURNLONG(prcommand, rc);

	if (rc) {
		efree(bind);
		efree(args);
		php_clear_stmt_bind(stmt);
		RETURN_FALSE;
	}

	stmt->var_cnt = var_cnt;
	stmt->vars = (zval **)emalloc((var_cnt) * sizeof(zval));
	for (i = start; i < var_cnt+start; i++) {
		ofs = i-start;
		ZVAL_ADDREF(*args[i]);
		stmt->vars[ofs] = *args[i];
	}

	efree(args);
	efree(bind);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_change_user(resource link, string user, string password, string database)
   Change logged-in user of the active connection */
PHP_FUNCTION(mysqli_change_user)
{
	MYSQL 		*mysql;
	zval  		*mysql_link = NULL;
	char  		*user, *password, *dbname;
	int   		user_len, password_len, dbname_len;
	ulong		rc;
	PR_COMMAND	*prcommand;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Osss", &mysql_link, mysqli_link_class_entry, &user, &user_len, &password, &password_len, &dbname, &dbname_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	MYSQLI_PROFILER_COMMAND_START(prcommand, prmysql);
	rc = mysql_change_user(mysql, user, password, dbname);
	MYSQLI_PROFILER_COMMAND_RETURNLONG(prcommand, rc);
	if (rc) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string mysqli_character_set_name(resource link)
   Returns the name of the character set used for this connection */
PHP_FUNCTION(mysqli_character_set_name)
{
	MYSQL 		*mysql;
	zval  		*mysql_link;
	PR_MYSQL	*prmysql;
	char		*charsetname;
	PR_COMMAND	*prcommand;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	MYSQLI_PROFILER_COMMAND_START(prcommand, prmysql);
	charsetname = (char *)mysql_character_set_name(mysql);
	MYSQLI_PROFILER_COMMAND_RETURNSTRING(prcommand, charsetname);

	RETURN_STRING(charsetname, 1);
}
/* }}} */

/* {{{ proto bool mysqli_close(resource link)
   Close connection */
PHP_FUNCTION(mysqli_close)
{
	zval 		*mysql_link;
	PR_MYSQL	*prmysql;
	MYSQL 		*mysql;
	PR_COMMON	*current;
	
	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	mysql_close(mysql);

	if (MyG(profiler)) {
		prmysql->closed = 1;
	}
	current = (PR_COMMON *)prmysql;
	if (MyG(profiler)) {
		php_mysqli_profiler_report(current, 0);
	}
	MYSQLI_CLEAR_RESOURCE(&mysql_link);	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_commit(resource link)
   Commit outstanding actions and close transaction */
PHP_FUNCTION(mysqli_commit)
{
	MYSQL 		*mysql;
	zval 		*mysql_link;
	PR_MYSQL	*prmysql;
	PR_COMMAND	*prcommand;
	ulong		rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");
	MYSQLI_PROFILER_COMMAND_START(prcommand, prmysql);
	rc = mysql_commit(mysql);
	MYSQLI_PROFILER_COMMAND_RETURNLONG(prcommand, rc);
	RETURN_BOOL(rc);
}
/* }}} */

/* {{{ proto void mysqli_data_seek(resource result)
   Move internal result pointer */
PHP_FUNCTION(mysqli_data_seek)
{
	MYSQL_RES	*result;
	zval  		*mysql_result;
	PR_RESULT	*prresult;
	PR_COMMAND	*prcommand;
	long  		offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Orl", &mysql_result, mysqli_result_class_entry, &mysql_result, &offset) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, prresult, PR_RESULT *, &mysql_result, "mysqli_result"); 

	if (result->handle && result->handle->status == MYSQL_STATUS_USE_RESULT) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Function cannot be used with MYSQL_USE_RESULT");
		RETURN_LONG(0);
	}

	MYSQLI_PROFILER_COMMAND_START(prcommand, prresult);
	mysql_data_seek(result, offset);
	MYSQLI_PROFILER_COMMAND_RETURNSTRING(prcommand, NULL);
	return;
}
/* }}} */

/* {{{ proto void mysqli_debug(string debug)
*/
PHP_FUNCTION(mysqli_debug)
{
	char		*debug;
	int			debug_len;
	PR_COMMAND	*prcommand;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &debug, &debug_len) == FAILURE) {
		return;
	}
	
	MYSQLI_PROFILER_COMMAND_START(prcommand, prmain);
	mysql_debug(debug);
	MYSQLI_PROFILER_COMMAND_RETURNSTRING(prcommand, NULL);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void mysqli_disable_reads_from_master(resource link)
*/
PHP_FUNCTION(mysqli_disable_reads_from_master)
{
	MYSQL		*mysql;
	zval		*mysql_link;
	PR_MYSQL	*prmysql;
	PR_COMMAND	*prcommand;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");
	MYSQLI_PROFILER_COMMAND_START(prcommand, prmysql);
	mysql_disable_reads_from_master(mysql);
	MYSQLI_PROFILER_COMMAND_RETURNSTRING(prcommand, NULL);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void mysqli_disable_rpl_parse(resource link)
*/
PHP_FUNCTION(mysqli_disable_rpl_parse)
{
	MYSQL		*mysql;
	zval    	*mysql_link;
	PR_MYSQL	*prmysql;
	PR_COMMAND	*prcommand;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");
	MYSQLI_PROFILER_COMMAND_START(prcommand, prmysql);
	mysql_disable_rpl_parse(mysql);
	MYSQLI_PROFILER_COMMAND_RETURNSTRING(prcommand, NULL);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_dump_debug_info(resource link)
*/
PHP_FUNCTION(mysqli_dump_debug_info)
{
	MYSQL 		*mysql;
	zval  		*mysql_link;
	PR_MYSQL	*prmysql;
	PR_COMMAND	*prcommand;
	ulong		rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	MYSQLI_PROFILER_COMMAND_START(prcommand, prmysql);
	rc = mysql_dump_debug_info(mysql);
	MYSQLI_PROFILER_COMMAND_RETURNLONG(prcommand, rc);

	if (rc) {
		RETURN_FALSE;
	}
	RETURN_TRUE;	
}
/* }}} */

/* {{{ proto void mysqli_enable_reads_from_master(resource link)
*/
PHP_FUNCTION(mysqli_enable_reads_from_master)
{
	MYSQL		*mysql;
	zval		*mysql_link;
	PR_MYSQL	*prmysql;
	PR_COMMAND	*prcommand;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	MYSQLI_PROFILER_COMMAND_START(prcommand, prmysql);
	mysql_enable_reads_from_master(mysql);
	MYSQLI_PROFILER_COMMAND_RETURNSTRING(prcommand, NULL);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void mysqli_enable_rpl_parse(resource link)
*/
PHP_FUNCTION(mysqli_enable_rpl_parse)
{
	MYSQL		*mysql;
	zval		*mysql_link;
	PR_MYSQL	*prmysql;
	PR_COMMAND	*prcommand;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	MYSQLI_PROFILER_COMMAND_START(prcommand, prmysql);
	mysql_enable_rpl_parse(mysql);
	MYSQLI_PROFILER_COMMAND_RETURNSTRING(prcommand, NULL);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int mysqli_errno(resource link)
   Returns the numerical value of the error message from previous MySQL operation */
PHP_FUNCTION(mysqli_errno)
{
	MYSQL 		*mysql;
	zval  		*mysql_link;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");
	RETURN_LONG(mysql_errno(mysql));
}
/* }}} */

/* {{{ proto string mysqli_error(resource link)
   Returns the text of the error message from previous MySQL operation */
PHP_FUNCTION(mysqli_error) 
{
	MYSQL 		*mysql;
	zval  		*mysql_link;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");
	RETURN_STRING((char *)mysql_error(mysql),1);
}
/* }}} */

/* {{{ proto int mysqli_execute(resource stmt)
   Execute a prepared statement */
PHP_FUNCTION(mysqli_execute)
{
	STMT 			*stmt;
	zval 			*mysql_stmt;
	unsigned int 	i;
	PR_STMT			*prstmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, STMT *, prstmt, PR_STMT *, &mysql_stmt, "mysqli_stmt"); 
	
	if (stmt->type == FETCH_SIMPLE) {
		for (i = 0; i < stmt->var_cnt; i++) {		
			if (stmt->vars[i]) {
				stmt->is_null[i] = (stmt->vars[i]->type == IS_NULL);

				switch (stmt->stmt->params[i].buffer_type) {
					case MYSQL_TYPE_VAR_STRING:
						convert_to_string_ex(&stmt->vars[i]);
						stmt->stmt->params[i].buffer = Z_STRVAL_PP(&stmt->vars[i]);
						stmt->stmt->params[i].buffer_length = strlen(Z_STRVAL_PP(&stmt->vars[i]));
						break;
					case MYSQL_TYPE_DOUBLE:
						convert_to_double_ex(&stmt->vars[i]);
						stmt->stmt->params[i].buffer = (gptr)&Z_LVAL_PP(&stmt->vars[i]);
						break;
					case MYSQL_TYPE_LONG:
						convert_to_long_ex(&stmt->vars[i]);
						stmt->stmt->params[i].buffer = (gptr)&Z_LVAL_PP(&stmt->vars[i]);
						break;
					default:
						break;
				}	
			}
		}
	}

	RETURN_LONG(mysql_execute(stmt->stmt));
}
/* }}} */

/* {{{ proto int mysqli_fetch(resource stmt)
   Fetch results from a prepared statement into the bound variables */
PHP_FUNCTION(mysqli_fetch)
{
	STMT 			*stmt;
	zval 			*mysql_stmt;
	unsigned int 	i;
	ulong 			ret;
	PR_STMT			*prstmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, STMT *, prstmt, PR_STMT *, &mysql_stmt, "mysqli_stmt"); 
	
	/* reset buffers */


	for (i = 0; i < stmt->var_cnt; i++) {
		if (stmt->bind[i].type == IS_STRING) {
			memset(stmt->bind[i].buffer, 0, stmt->bind[i].buflen);
		}
	}
	
	if (!(ret = mysql_fetch(stmt->stmt))) {

		for (i = 0; i < stmt->var_cnt; i++) {
			if (!stmt->is_null[i]) {
				switch (stmt->bind[i].type) {
					case IS_LONG:
						stmt->vars[i]->type = IS_LONG;
						break;
					case IS_DOUBLE:
						stmt->vars[i]->type = IS_DOUBLE;
						break;
					case IS_STRING:
						if (stmt->stmt->bind[i].buffer_type == MYSQL_TYPE_LONGLONG) {
							char tmp[50];
							my_ulonglong lval;
							memcpy (&lval, stmt->bind[i].buffer, sizeof(my_ulonglong));
							if (lval != (long)lval) {
								/* even though lval is declared as unsigned, the value
								 * may be negative. Therefor we cannot use %llu and must
								 * user %lld.
								 */
								sprintf((char *)&tmp, "%lld", lval);
								ZVAL_STRING(stmt->vars[i], tmp, 1);
							} else {
								ZVAL_LONG(stmt->vars[i], lval);
							}
						} else {
							if (stmt->vars[i]->type == IS_STRING) {
								efree(stmt->vars[i]->value.str.val);
							} 
							ZVAL_STRING(stmt->vars[i], stmt->bind[i].buffer, 1); 
						}
						break;
					default:
						break;	
				}
			} else {
				stmt->vars[i]->type = IS_NULL;
			}
		}
	}

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto int mysqli_fetch_field (resource result)
   Get column information from a result and return as an object */
PHP_FUNCTION(mysqli_fetch_field) 
{
	MYSQL_RES 	*result;
	zval  		*mysql_result;
	MYSQL_FIELD *field;
	PR_RESULT 	*prresult;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, prresult, PR_RESULT *, &mysql_result, "mysqli_result"); 

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

/* {{{ proto int mysqli_fetch_fields (resource result)
   Return array of objects containing field meta-data */
PHP_FUNCTION(mysqli_fetch_fields) 
{
	MYSQL_RES	*result;
	zval  		*mysql_result;
	MYSQL_FIELD	*field;
	zval 		*obj;
	PR_RESULT 	*prresult;

	unsigned int i;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, prresult, PR_RESULT *, &mysql_result, "mysqli_result"); 

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

/* {{{ proto int mysqli_fetch_field_direct (resource result, int offset)
   Fetch meta-data for a single field */
PHP_FUNCTION(mysqli_fetch_field_direct) 
{
	MYSQL_RES	*result;
	zval		*mysql_result;
	MYSQL_FIELD *field;
	int 		offset;
	PR_RESULT 	*prresult;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &mysql_result, mysqli_result_class_entry, &offset) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, prresult, PR_RESULT *, &mysql_result, "mysqli_result"); 

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

/* {{{ proto array mysqli_fetch_lengths (resource result)
   Get the length of each output in a result */
PHP_FUNCTION(mysqli_fetch_lengths) 
{
	MYSQL_RES		*result;
	zval			*mysql_result;
	unsigned int	i;
	unsigned long	*ret;
	PR_RESULT 		*prresult;
	

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, prresult, PR_RESULT *, &mysql_result, "mysqli_result"); 

	if (!(ret = mysql_fetch_lengths(result))) {
		RETURN_FALSE;
	}

	array_init(return_value);

	for (i = 0; i < mysql_num_fields(result); i++) {
		add_index_long(return_value, i, ret[i]);	
	}
}
/* }}} */

/* {{{ proto array mysqli_fetch_row (resource result)
   Get a result row as an enumerated array */
PHP_FUNCTION(mysqli_fetch_row) 
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQLI_NUM);
}
/* }}} */

/* {{{ proto int mysqli_field_count(resource link)
   Fetch the number of fields returned by the last query for the given link
*/
PHP_FUNCTION(mysqli_field_count) 
{
	MYSQL 		*mysql;
	zval  		*mysql_link;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_LONG(mysql_field_count(mysql));
}
/* }}} */

/* {{{ proto int mysqli_field_seek(resource link, int fieldnr)
   Set result pointer to a specified field offset
*/
PHP_FUNCTION(mysqli_field_seek)
{
	MYSQL_RES		*result;
	zval			*mysql_result;
	unsigned int	fieldnr;
	PR_RESULT 		*prresult;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &mysql_result, mysqli_result_class_entry, &fieldnr) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, prresult, PR_RESULT *, &mysql_result, "mysqli_result"); 

	if (fieldnr < 0 || fieldnr >= mysql_num_fields(result)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Field offset is invalid for resultset");
		RETURN_FALSE; 
	}
	
	mysql_field_seek(result, fieldnr);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int mysqli_field_tell(resource result)
   Get current field offset of result pointer */
PHP_FUNCTION(mysqli_field_tell)
{
	MYSQL_RES	*result;
	zval		*mysql_result;
	PR_RESULT 	*prresult;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, prresult, PR_RESULT *, &mysql_result, "mysqli_result"); 
	
	RETURN_LONG(mysql_field_tell(result));
}
/* }}} */

/* {{{ proto int mysqli_free_result(resource result)
   Free query result memory for the given result handle */
PHP_FUNCTION(mysqli_free_result) 
{
	MYSQL_RES	*result;
	zval		*mysql_result;
	PR_RESULT	*prresult;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, prresult, PR_RESULT *, &mysql_result, "mysqli_result"); 

	mysql_free_result(result);
	if (MyG(profiler)) {
		prresult->closed = 1;
	}
	MYSQLI_CLEAR_RESOURCE(&mysql_result);	

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string mysqli_get_client_info(void) 
   Get MySQL client info */
PHP_FUNCTION(mysqli_get_client_info)
{
	RETURN_STRING((char *)mysql_get_client_info(), 1);
}
/* }}} */

/* {{{ proto string mysqli_get_host_info (resource link) 
   Get MySQL host info */
PHP_FUNCTION(mysqli_get_host_info)
{
	MYSQL 		*mysql;
	zval  		*mysql_link = NULL;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_STRING((char *)mysql_get_host_info(mysql), 1);
}
/* }}} */

/* {{{ proto int mysqli_get_proto_info 
   Get MySQL protocol information */
PHP_FUNCTION(mysqli_get_proto_info)
{
	MYSQL 		*mysql;
	zval  		*mysql_link = NULL;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_LONG(mysql_get_proto_info(mysql));
}
/* }}} */

/* {{{ proto string mysqli_get_server_info(resource link) 
   Get MySQL server info */
PHP_FUNCTION(mysqli_get_server_info)
{
	MYSQL		*mysql;
	zval	  	*mysql_link = NULL;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_STRING((char *)mysql_get_server_info(mysql), 1);
}

/* }}} */

/* {{{ proto int mysqli_get_server_version(resource link) 
   Return the MySQL version for the server referenced by the given link */
PHP_FUNCTION(mysqli_get_server_version)
{
	MYSQL	*mysql;
	zval  	*mysql_link = NULL;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_LONG(mysql_get_server_version(mysql));
}

/* }}} */

/* {{{ proto string mysqli_info(resource link)
   Get information about the most recent query */
PHP_FUNCTION(mysqli_info)
{
	MYSQL 		*mysql;
	char  		*info = NULL;
	zval  		*mysql_link = NULL;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	if (!(info = (char *)mysql_info(mysql))) {
		RETURN_FALSE;
	}

	RETURN_STRING(info, 1);
}
/* }}} */

/* {{{ proto resource mysqli_init(void)
   Initialize mysqli and return a resource for use with mysql_real_connect */
PHP_FUNCTION(mysqli_init)
{
	MYSQLI_RESOURCE *mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)mysql_init(NULL);
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_link_class_entry);	
}
/* }}} */

/* {{{ proto mixed mysqli_insert_id(resource link)
   Get the ID generated from the previous INSERT operation */
PHP_FUNCTION(mysqli_insert_id)
{
	MYSQL		 	*mysql;
	my_ulonglong 	rc;
	zval  			*mysql_link;
	PR_MYSQL		*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");
	rc = mysql_insert_id(mysql);
	MYSQLI_RETURN_LONG_LONG(rc)
}
/* }}} */

/* {{{ proto bool mysqli_kill(resource link, int processid)
   Kill a mysql process on the server */
PHP_FUNCTION(mysqli_kill)
{
	MYSQL 		*mysql;
	zval  		*mysql_link;
	int   		processid;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &mysql_link, mysqli_link_class_entry, &processid) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");
	
	if (mysql_kill(mysql, processid)) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_master_query(resource link, string query)
   Enforce execution of a query on the master in a master/slave setup */
PHP_FUNCTION(mysqli_master_query) {
	MYSQL			*mysql;
	zval			*mysql_link;
	char			*query = NULL;
	unsigned int 	query_len;
	PR_MYSQL		*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	if (mysql_master_query(mysql, query, query_len)) {
		RETURN_FALSE;
	}	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int mysqli_num_fields(resource result)
   Get number of fields in result */
PHP_FUNCTION(mysqli_num_fields)
{
	MYSQL_RES	*result;
	zval		*mysql_result;
	PR_RESULT	*prresult;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, prresult, PR_RESULT *, &mysql_result, "mysqli_result"); 


	RETURN_LONG(mysql_num_fields(result));
}
/* }}} */

/* {{{ proto int mysqli_num_rows(resource result)
   Get number of rows in result */
PHP_FUNCTION(mysqli_num_rows)
{
	MYSQL_RES	*result;
	zval		*mysql_result;
	PR_RESULT	*prresult;
	PR_COMMAND	*prcommand;
	ulong		rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, prresult, PR_RESULT *, &mysql_result, "mysqli_result"); 

	if (result->handle && result->handle->status == MYSQL_STATUS_USE_RESULT) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Function cannot be used with MYSQL_USE_RESULT");
		RETURN_LONG(0);
	}

	MYSQLI_PROFILER_COMMAND_START(prcommand, prresult);
	rc = mysql_num_rows(result);
	MYSQLI_PROFILER_COMMAND_RETURNLONG(prcommand, rc);
	RETURN_LONG(rc);
}
/* }}} */

/* {{{ proto bool mysqli_options(resource link, int flags, mixed values)
   Set options */
PHP_FUNCTION(mysqli_options)
{
	MYSQL 			*mysql;
	zval  			*mysql_link = NULL;
	zval  			*mysql_value;
	long  			mysql_option;
	unsigned int 	l_value;
	long  			ret;
	PR_MYSQL		*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oslz", &mysql_link, mysqli_link_class_entry, &mysql_option, &mysql_value) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	switch (Z_TYPE_PP(&mysql_value)) {
		case IS_STRING:
			ret = mysql_options(mysql, mysql_option, Z_STRVAL_PP(&mysql_value));
			break;
		default:
			convert_to_long_ex(&mysql_value);
			l_value = Z_LVAL_PP(&mysql_value);
			ret = mysql_options(mysql, mysql_option, (char *)&l_value);
			break;
	}

	if (ret != 0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}   
/* }}} */

/* {{{ proto int mysqli_param_count(resource stmt) {
   Return the number of parameter for the given statement */
PHP_FUNCTION(mysqli_param_count)
{
	STMT 		*stmt;
	zval		*mysql_stmt;
	PR_STMT		*prstmt;
	
	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, STMT *, prstmt, PR_STMT *, &mysql_stmt, "mysqli_stmt"); 
		
	RETURN_LONG(mysql_param_count(stmt->stmt));
}
/* }}} */

/* {{{ proto int mysqli_ping(resource link)
   Ping a server connection or reconnect if there is no connection */
PHP_FUNCTION(mysqli_ping)
{
	MYSQL 		*mysql;
	zval  		*mysql_link;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");
	RETURN_LONG(mysql_ping(mysql));
}
/* }}} */

/* {{{ proto resource mysqli_prepare(resource link, string query)
   Prepare a SQL statement for execution */
PHP_FUNCTION(mysqli_prepare)
{
	MYSQL			*mysql;
	STMT 			*stmt;
	PR_MYSQL		*prmysql;
	PR_STMT			*prstmt;
	char			*query = NULL;
	unsigned int	query_len;
	zval			*mysql_link;
	MYSQLI_RESOURCE *mysqli_resource; 

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os",&mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	stmt = (STMT *)ecalloc(1,sizeof(STMT));
	stmt->var_cnt = 0;

	/* profiling information */
	if (MyG(profiler)) {
		prstmt = (PR_STMT *)MYSQLI_PROFILER_NEW(prmysql, MYSQLI_PR_STMT, 1);
		prstmt->explain.query = my_estrdup(query);
		if (!strncasecmp("select", query, 6)){
			if (!(MYSQLI_PROFILER_EXPLAIN(&prstmt->explain, &prstmt->header, mysql, query))) {
				RETURN_FALSE;
			}
		}
	}
	stmt->stmt = mysql_prepare(mysql, query, query_len); 

	if (MyG(profiler)) {
		MYSQLI_PROFILER_ELAPSEDTIME(prstmt);
		if (!stmt->stmt) {
			prstmt->header.error = mysql_errno(mysql);
			prstmt->header.errormsg = my_estrdup(mysql_error(mysql));
		} else {
			prstmt->param_cnt = mysql_param_count(stmt->stmt);
			prstmt->field_cnt = stmt->stmt->field_count;
		}
	}

	if (!stmt->stmt) {
		efree(stmt);
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)stmt;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_stmt_class_entry);
}
/* }}} */

/* {{{ proto resource mysqli_prepare_result(resource stmt)
 */
PHP_FUNCTION(mysqli_prepare_result)
{
	STMT			*stmt;
	MYSQL_RES		*result;
	zval 			*mysql_stmt;
	PR_STMT			*prstmt = NULL;
	MYSQLI_RESOURCE	*mysqli_resource;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, STMT *, prstmt, PR_STMT *, &mysql_stmt, "mysqli_stmt"); 
	
	if (!(result = mysql_prepare_result(stmt->stmt))){
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->prinfo = (void *)prstmt;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_result_class_entry);	
}
/* }}} */

/* {{{ proto bool mysqli_read_query_result(resource link)
*/
PHP_FUNCTION(mysqli_read_query_result)
{
	MYSQL		*mysql;
	zval 		*mysql_link;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	if (mysql_read_query_result(mysql)) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_real_connect(resource link [,string hostname [,string username [,string passwd [,string dbname [,int port [,string socket [,int flags]]]]]]])
   Open a connection to a mysql server */ 
PHP_FUNCTION(mysqli_real_connect)
{
	MYSQL 		*mysql;
	char 		*hostname = NULL, *username=NULL, *passwd=NULL, *dbname=NULL, *socket=NULL;
	unsigned int hostname_len, username_len, passwd_len, dbname_len, socket_len;
	unsigned int port=0, flags=0;
	zval 		*mysql_link;
	PR_MYSQL	*prmysql;

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

	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	if (mysql_real_connect(mysql,hostname,username,passwd,dbname,port,socket,flags) == NULL) {
		/* Save error messages */
		php_mysqli_set_error(mysql_errno(mysql), (char *)mysql_error(mysql) TSRMLS_CC);

		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", mysql_error(mysql));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_real_query(resource link, string query)
   Binary-safe version of mysql_query() */
PHP_FUNCTION(mysqli_real_query)
{
	MYSQL			*mysql;
	zval			*mysql_link;
	char			*query = NULL;
	unsigned int 	query_len;
	PR_MYSQL		*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	if (mysql_real_query(mysql, query, query_len)) {
		RETURN_FALSE;
	}	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string mysqli_real_escape_string(resource link, string escapestr) 
   Escapes special characters in a string for use in a SQL statement, taking into account the current charset of the connection */
PHP_FUNCTION(mysqli_real_escape_string) {
	MYSQL 		*mysql;
	zval		*mysql_link = NULL;
	char		*escapestr, *newstr;
	int			escapestr_len, newstr_len;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &escapestr, &escapestr_len) == FAILURE) {
		return;
	}	
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	newstr = emalloc(2 * escapestr_len + 1);
	newstr_len = mysql_real_escape_string(mysql, newstr, escapestr, escapestr_len);
	newstr = erealloc(newstr, newstr_len + 1);
		
	RETURN_STRING(newstr, 0);
}
/* }}} */

/* {{{ proto bool mysqli_reload (resource link)
*/
PHP_FUNCTION(mysqli_reload)
{
	MYSQL 		*mysql;
	zval  		*mysql_link;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_BOOL(!mysql_reload(mysql));
}
/* }}} */

/* {{{ proto bool mysqli_rollback(resource link)
   Undo actions from current transaction */
PHP_FUNCTION(mysqli_rollback)
{
	MYSQL 		*mysql;
	zval  		*mysql_link;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_BOOL(mysql_rollback(mysql));
}
/* }}} */

/* {{{ proto int mysqli_rpl_parse_enabled(resource link)
*/
PHP_FUNCTION(mysqli_rpl_parse_enabled)
{
	MYSQL		*mysql;
	zval  		*mysql_link;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_LONG(mysql_rpl_parse_enabled(mysql));
}
/* }}} */

/* {{{ proto bool mysqli_rpl_probe(resource link)
*/
PHP_FUNCTION(mysqli_rpl_probe)
{
	MYSQL		*mysql;
	zval  		*mysql_link;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");


	if (mysql_rpl_probe(mysql)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int mysqli_rpl_query_type(string query)
*/
PHP_FUNCTION(mysqli_rpl_query_type)
{
	MYSQL		*mysql;
	zval		*mysql_link;
	char		*query;
	int	 		query_len;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_LONG(mysql_rpl_query_type(query, query_len));
}
/* }}} */

/* {{{ proto bool mysqli_send_long_data(resource stmt, int param_nr, string data)
*/
PHP_FUNCTION(mysqli_send_long_data)
{
	STMT 	*stmt;
	zval  	*mysql_stmt;
	PR_STMT	*prstmt;
	char  *data;
	long  param_nr, data_len;


	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ols", &mysql_stmt, mysqli_stmt_class_entry, &param_nr, &data, &data_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, STMT *, prstmt, PR_STMT *, &mysql_stmt, "mysqli_stmt"); 

	if (!param_nr) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid parameter number");
		RETURN_FALSE;
	}
	param_nr--;

	if (mysql_send_long_data(stmt->stmt, param_nr, data, data_len)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_send_query(resource link, string query)
*/
PHP_FUNCTION(mysqli_send_query)
{
	MYSQL			*mysql;
	zval			*mysql_link;
	char			*query = NULL;
	unsigned int 	query_len;
	PR_MYSQL		*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	if (mysql_send_query(mysql, query, query_len)) {
		RETURN_FALSE;
	}	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_slave_query(resource link, string query)
   Enforce execution of a query on a slave in a master/slave setup */
PHP_FUNCTION(mysqli_slave_query)
{
	MYSQL			*mysql;
	zval			*mysql_link;
	char			*query = NULL;
	unsigned int	query_len;
	PR_MYSQL		*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	if (mysql_slave_query(mysql, query, query_len)) {
		RETURN_FALSE;
	}	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed mysqli_stmt_affected_rows(resource stmt)
   Return the number of rows affected in the last query for the given link */
PHP_FUNCTION(mysqli_stmt_affected_rows)
{
	STMT 			*stmt;
	zval    		*mysql_stmt;
	my_ulonglong	rc;
	PR_STMT			*prstmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, STMT *, prstmt, PR_STMT *, &mysql_stmt, "mysqli_stmt");

	rc = mysql_stmt_affected_rows(stmt->stmt);
	MYSQLI_RETURN_LONG_LONG(rc)
}
/* }}} */

/* {{{ proto bool mysqli_stmt_close(resource stmt) 
   Close statement */
PHP_FUNCTION(mysqli_stmt_close)
{
	STMT 		*stmt;
	zval    	*mysql_stmt;
	PR_STMT		*prstmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, STMT *, prstmt, PR_STMT *, &mysql_stmt, "mysqli_stmt"); 
	mysql_stmt_close(stmt->stmt);
	stmt->stmt = NULL;
	php_clear_stmt_bind(stmt); 
	MYSQLI_CLEAR_RESOURCE(&mysql_stmt);
	RETURN_TRUE;

}
/* }}} */

/* {{{ proto string mysqli_select_db(resource link, string dbname)
   Select a MySQL database */
PHP_FUNCTION(mysqli_select_db) 
{
	MYSQL 		*mysql;
	zval  		*mysql_link;
	char  		*dbname;
	int   		dbname_len;
	PR_MYSQL	*prmysql;

//	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,  "Os", &mysql_link, mysqli_link_class_entry, &dbname, &dbname_len) == FAILURE) {
	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &dbname, &dbname_len) == FAILURE) {
		return;
	} 
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");
	
	if (!mysql_select_db(mysql, dbname)) {
		RETURN_TRUE;
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string mysqli_ssl_set(resource link [,string key [,string cert [,string ca [,string capath [,string cipher]]]]])
*/
PHP_FUNCTION(mysqli_ssl_set)
{
	MYSQL 		*mysql;
	zval  		*mysql_link;
	char  		*key=NULL, *cert=NULL, *ca=NULL, *capath=NULL, *cipher=NULL;
	int   		key_len, cert_len, ca_len, capath_len, cipher_len;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Osssss", &mysql_link, mysqli_link_class_entry, 
		&key, &key_len, &cert, &cert_len, &ca, &ca_len, &capath, &capath_len, &cipher, &cipher_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");
	
	mysql_ssl_set(mysql, key, cert, ca, capath, cipher);

	RETURN_TRUE;
}
/* }}} */
  
/* {{{ proto string mysqli_stat(resource link) 
   Get current system status */
PHP_FUNCTION(mysqli_stat)
{
	MYSQL 		*mysql;
	zval  		*mysql_link;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_STRING((char *)mysql_stat(mysql), 1);
}

/* }}} */
 
/* {{{ proto int mysqli_stmt_errno(resource stmt)
*/
PHP_FUNCTION(mysqli_stmt_errno)
{
	STMT 	*stmt;
	zval 	*mysql_stmt;
	PR_STMT	*prstmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, STMT *, prstmt, PR_STMT *, &mysql_stmt, "mysqli_stmt"); 

	RETURN_LONG(mysql_stmt_errno(stmt->stmt));
}
/* }}} */

/* {{{ proto string mysqli_stmt_error(resource stmt)
*/
PHP_FUNCTION(mysqli_stmt_error) 
{
	STMT 	*stmt;
	zval 	*mysql_stmt;
	PR_STMT	*prstmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, STMT *, prstmt, PR_STMT *, &mysql_stmt, "mysqli_stmt"); 
	
	RETURN_STRING((char *)mysql_stmt_error(stmt->stmt),1);
}
/* }}} */

/* {{{ proto resource mysqli_stmt_store_result(stmt)
*/
PHP_FUNCTION(mysqli_stmt_store_result)
{
	STMT 	*stmt;
	zval 	*mysql_stmt;
	PR_STMT	*prstmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, STMT *, prstmt, PR_STMT *, &mysql_stmt, "mysqli_stmt"); 
	
	if (mysql_stmt_store_result(stmt->stmt)){
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto resource mysqli_store_result(resource link)
   Buffer result set on client */
PHP_FUNCTION(mysqli_store_result)
{
	MYSQL 			*mysql;
	MYSQL_RES 		*result;
	zval  			*mysql_link;
	PR_MYSQL		*prmysql;
	MYSQLI_RESOURCE	*mysqli_resource;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	if (!(result = mysql_store_result(mysql))) {
		RETURN_FALSE;
	}
	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_result_class_entry);	
}
/* }}} */

/* {{{ proto int mysqli_thread_id(resource link)
   Return the current thread ID */
PHP_FUNCTION(mysqli_thread_id)
{
	MYSQL 		*mysql;
	zval  		*mysql_link;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_LONG(mysql_thread_id(mysql));
}
/* }}} */

/* {{{ proto bool mysqli_thread_safe(void)
   Return whether thread safety is given or not */
PHP_FUNCTION(mysqli_thread_safe)
{
	RETURN_BOOL(mysql_thread_safe());
}

/* }}} */

/* {{{ proto resource mysqli_use_result(resource link)
   Directly retrieve query results - do not buffer results on client side */
PHP_FUNCTION(mysqli_use_result)
{
	MYSQL 			*mysql;
	MYSQL_RES 		*result;
	zval  			*mysql_link;
	PR_MYSQL		*prmysql;
	MYSQLI_RESOURCE *mysqli_resource;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	if (!(result = mysql_use_result(mysql))) {
		RETURN_FALSE;
	}
	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_result_class_entry);	
}
/* }}} */

/* {{{ proto resource mysqli_warning_count (resource link)
   Return number of warnings from the last query for the given link */
PHP_FUNCTION(mysqli_warning_count)
{
	MYSQL 		*mysql;
	zval  		*mysql_link;
	PR_MYSQL	*prmysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	RETURN_LONG(mysql_warning_count(mysql));
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
