/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Georg Richter <georg@php.net>                               |
  |          Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+

  $Id$
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <signal.h>

#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"
#include "ext/standard/php_smart_str.h"
#include "php_mysqli_structs.h"
#include "mysqli_priv.h"

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

	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

	rc = mysql_affected_rows(mysql->mysql);
	if (rc == (my_ulonglong) -1) {
		RETURN_LONG(-1);
	}
	MYSQLI_RETURN_LONG_LONG(rc);
}
/* }}} */


/* {{{ proto bool mysqli_autocommit(object link, bool mode)
   Turn auto commit on or of */
PHP_FUNCTION(mysqli_autocommit)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	zend_bool	automode;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ob", &mysql_link, mysqli_link_class_entry, &automode) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

	if (mysql_autocommit(mysql->mysql, (my_bool)automode)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ mysqli_stmt_bind_param_do_bind */
#ifndef MYSQLI_USE_MYSQLND
static
int mysqli_stmt_bind_param_do_bind(MY_STMT *stmt, unsigned int argc, unsigned int num_vars,
								   zval ***args, unsigned int start, const char * const types TSRMLS_DC)
{
	int				i, ofs;
	MYSQL_BIND		*bind;
	unsigned long	rc;

	/* prevent leak if variables are already bound */
	if (stmt->param.var_cnt) {
		php_free_stmt_bind_buffer(stmt->param, FETCH_SIMPLE);
	}

	stmt->param.is_null = ecalloc(num_vars, sizeof(char));
	bind = (MYSQL_BIND *) ecalloc(num_vars, sizeof(MYSQL_BIND));

	ofs = 0;
	for (i = start; i < argc; i++) {

		/* set specified type */
		switch (types[ofs]) {
			case 'd': /* Double */
				bind[ofs].buffer_type = MYSQL_TYPE_DOUBLE;
				bind[ofs].buffer = &Z_DVAL_PP(args[i]);
				bind[ofs].is_null = &stmt->param.is_null[ofs];
				break;

			case 'i': /* Integer */
#if SIZEOF_LONG==8
				bind[ofs].buffer_type = MYSQL_TYPE_LONGLONG;
#elif SIZEOF_LONG==4
				bind[ofs].buffer_type = MYSQL_TYPE_LONG;
#endif
				bind[ofs].buffer = &Z_LVAL_PP(args[i]);
				bind[ofs].is_null = &stmt->param.is_null[ofs];
				break;

			case 'b': /* Blob (send data) */
				bind[ofs].buffer_type = MYSQL_TYPE_LONG_BLOB;
				/* don't initialize is_null and length to 0 because we use ecalloc */
				break;

			case 's': /* string */
				bind[ofs].buffer_type = MYSQL_TYPE_VAR_STRING;
				/* don't initialize buffer and buffer_length because we use ecalloc */
				bind[ofs].is_null = &stmt->param.is_null[ofs];
				break;

			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Undefined fieldtype %c (parameter %d)", types[ofs], i+1);
				rc = 1;
				goto end_1;
		}
		ofs++;
	}
	rc = mysql_stmt_bind_param(stmt->stmt, bind);

end_1:
	if (rc) {
		efree(stmt->param.is_null);
	} else {
		stmt->param.var_cnt = num_vars;
		stmt->param.vars = (zval **)safe_emalloc(num_vars, sizeof(zval), 0);
		for (i = 0; i < num_vars; i++) {
			if (bind[i].buffer_type  != MYSQL_TYPE_LONG_BLOB) {
				Z_ADDREF_P(*args[i+start]);
				stmt->param.vars[i] = *args[i+start];
			} else {
				stmt->param.vars[i] = NULL;
			}
		}
	}
	efree(bind);

	return rc;
}
#else
static
int mysqli_stmt_bind_param_do_bind(MY_STMT *stmt, unsigned int argc, unsigned int num_vars,
								   zval ***args, unsigned int start, const char * const types TSRMLS_DC)
{
	unsigned int i;
	MYSQLND_PARAM_BIND	*params;
	enum_func_status	ret = FAIL;

	/* If no params -> skip binding and return directly */
	if (argc == start) {
		return PASS;
	}
	params = mysqlnd_stmt_alloc_param_bind(stmt->stmt);
	if (!params) {
		goto end;
	}
	for (i = 0; i < (argc - start); i++) {
		zend_uchar type;
		switch (types[i]) {
			case 'd': /* Double */
				type = MYSQL_TYPE_DOUBLE;
				break;
			case 'i': /* Integer */
#if SIZEOF_LONG==8
				type = MYSQL_TYPE_LONGLONG;
#elif SIZEOF_LONG==4
				type = MYSQL_TYPE_LONG;
#endif
				break;
			case 'b': /* Blob (send data) */
				type = MYSQL_TYPE_LONG_BLOB;
				break;
			case 's': /* string */
				type = MYSQL_TYPE_VAR_STRING;
				break;
			default:
				/* We count parameters from 1 */
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Undefined fieldtype %c (parameter %d)", types[i], i + start + 1);
				ret = FAIL;
				mysqlnd_stmt_free_param_bind(stmt->stmt, params);
				goto end;
		}
		params[i].zv = *(args[i + start]);
		params[i].type = type;
	}
	ret = mysqlnd_stmt_bind_param(stmt->stmt, params);

end:
	return ret;
}
#endif
/* }}} */

/* {{{ proto bool mysqli_stmt_bind_param(object stmt, string types, mixed variable [,mixed,....]) U
   Bind variables to a prepared statement as parameters */
PHP_FUNCTION(mysqli_stmt_bind_param)
{
	zval			***args;
	int				argc = ZEND_NUM_ARGS();
	int				num_vars;
	int				start = 2;
	MY_STMT			*stmt;
	zval			*mysql_stmt;
	char			*types;
	int				types_len;
	unsigned long	rc;

	/* calculate and check number of parameters */
	if (argc < 2) {
		/* there has to be at least one pair */
		WRONG_PARAM_COUNT;
	}

	if (zend_parse_method_parameters((getThis()) ? 1:2 TSRMLS_CC, getThis(), "Os", &mysql_stmt, mysqli_stmt_class_entry,
									&types, &types_len) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	num_vars = argc - 1;
	if (getThis()) {
		start = 1;
	} else {
		/* ignore handle parameter in procedural interface*/
		--num_vars;
	}
	if (!types_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid type or no types specified");
		RETURN_FALSE;
	}

	if (types_len != argc - start) {
		/* number of bind variables doesn't match number of elements in type definition string */
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number of elements in type definition string doesn't match number of bind variables");
		RETURN_FALSE;
	}

	if (types_len != mysql_stmt_param_count(stmt->stmt)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number of variables doesn't match number of parameters in prepared statement");
		RETURN_FALSE;
	}

	args = (zval ***)safe_emalloc(argc, sizeof(zval **), 0);

	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		zend_wrong_param_count(TSRMLS_C);
		rc = 1;
	} else {
		rc = mysqli_stmt_bind_param_do_bind(stmt, argc, num_vars, args, start, types TSRMLS_CC);
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
	}

	efree(args);

	RETURN_BOOL(!rc);
}
/* }}} */

/* {{{ mysqli_stmt_bind_result_do_bind */
#ifndef MYSQLI_USE_MYSQLND
/* TODO:
   do_alloca, free_alloca
*/
static int
mysqli_stmt_bind_result_do_bind(MY_STMT *stmt, zval ***args, unsigned int argc, unsigned int start TSRMLS_DC)
{
	MYSQL_BIND	*bind;
	int			i, ofs;
	int			var_cnt = argc - start;
	long		col_type;
	ulong		rc;

	/* prevent leak if variables are already bound */
	if (stmt->result.var_cnt) {
		php_free_stmt_bind_buffer(stmt->result, FETCH_RESULT);
	}

	bind = (MYSQL_BIND *)ecalloc(var_cnt, sizeof(MYSQL_BIND));
	{
		int size;
		char *p= emalloc(size= var_cnt * (sizeof(char) + sizeof(VAR_BUFFER)));
		stmt->result.buf = (VAR_BUFFER *) p;
		stmt->result.is_null = p + var_cnt * sizeof(VAR_BUFFER);
		memset(p, 0, size);
	}

	for (i=start; i < var_cnt + start ; i++) {
		ofs = i - start;
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

			case MYSQL_TYPE_NULL:
				stmt->result.buf[ofs].type = IS_NULL;
				/*
				  don't initialize to 0 :
				  1. stmt->result.buf[ofs].buflen
				  2. bind[ofs].buffer
				  3. bind[ofs].buffer_length
				  because memory was allocated with ecalloc
				*/
				bind[ofs].buffer_type = MYSQL_TYPE_NULL;
				bind[ofs].is_null = &stmt->result.is_null[ofs];
				break;

			case MYSQL_TYPE_SHORT:
			case MYSQL_TYPE_TINY:
			case MYSQL_TYPE_LONG:
			case MYSQL_TYPE_INT24:
			case MYSQL_TYPE_YEAR:
				convert_to_long_ex(args[i]);
				stmt->result.buf[ofs].type = IS_LONG;
				/* don't set stmt->result.buf[ofs].buflen to 0, we used ecalloc */
				stmt->result.buf[ofs].val = (char *)emalloc(sizeof(int));
				bind[ofs].buffer_type = MYSQL_TYPE_LONG;
				bind[ofs].buffer = stmt->result.buf[ofs].val;
				bind[ofs].is_null = &stmt->result.is_null[ofs];
				bind[ofs].is_unsigned = (stmt->stmt->fields[ofs].flags & UNSIGNED_FLAG) ? 1 : 0;
				break;

			case MYSQL_TYPE_LONGLONG:
#if MYSQL_VERSION_ID > 50002 || defined(MYSQLI_USE_MYSQLND)
			case MYSQL_TYPE_BIT:
#endif
				stmt->result.buf[ofs].type = IS_STRING;
				stmt->result.buf[ofs].buflen = sizeof(my_ulonglong);
				stmt->result.buf[ofs].val = (char *)emalloc(stmt->result.buf[ofs].buflen);
				bind[ofs].buffer_type = col_type;
				bind[ofs].buffer = stmt->result.buf[ofs].val;
				bind[ofs].is_null = &stmt->result.is_null[ofs];
				bind[ofs].buffer_length = stmt->result.buf[ofs].buflen;
				bind[ofs].is_unsigned = (stmt->stmt->fields[ofs].flags & UNSIGNED_FLAG) ? 1 : 0;
				bind[ofs].length = &stmt->result.buf[ofs].output_len;
				break;

			case MYSQL_TYPE_DATE:
			case MYSQL_TYPE_TIME:
			case MYSQL_TYPE_DATETIME:
			case MYSQL_TYPE_NEWDATE:
			case MYSQL_TYPE_VAR_STRING:
			case MYSQL_TYPE_STRING:
			case MYSQL_TYPE_TINY_BLOB:
			case MYSQL_TYPE_BLOB:
			case MYSQL_TYPE_MEDIUM_BLOB:
			case MYSQL_TYPE_LONG_BLOB:
			case MYSQL_TYPE_TIMESTAMP:
			case MYSQL_TYPE_DECIMAL:
			case MYSQL_TYPE_GEOMETRY:
#ifdef FIELD_TYPE_NEWDECIMAL
			case MYSQL_TYPE_NEWDECIMAL:
#endif
			{
#if MYSQL_VERSION_ID >= 50107
				/* Changed to my_bool in MySQL 5.1. See MySQL Bug #16144 */
				my_bool tmp;
#else
				ulong tmp = 0;
#endif
				stmt->result.buf[ofs].type = IS_STRING;
				/*
					If the user has called $stmt->store_result() then we have asked
					max_length to be updated. this is done only for BLOBS because we don't want to allocate
					big chunkgs of memory 2^16 or 2^24
				*/
				if (stmt->stmt->fields[ofs].max_length == 0 &&
					!mysql_stmt_attr_get(stmt->stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &tmp) && !tmp)
				{
					/*
					  Allocate directly 256 because it's easier to allocate a bit more
					  than update max length even for text columns. Try SELECT UNION SELECT UNION with
					  different lengths and you will see that we get different lengths in stmt->stmt->fields[ofs].length
					  The just take 256 and saves us from realloc-ing.
					*/
					stmt->result.buf[ofs].buflen =
						(stmt->stmt->fields) ? (stmt->stmt->fields[ofs].length) ? stmt->stmt->fields[ofs].length + 1: 256: 256;

				} else {
					/*
						the user has called store_result(). if he does not there is no way to determine the
						libmysql does not allow us to allocate 0 bytes for a buffer so we try 1
					*/
					if (!(stmt->result.buf[ofs].buflen = stmt->stmt->fields[ofs].max_length))
						++stmt->result.buf[ofs].buflen;
				}
				stmt->result.buf[ofs].val = (char *)emalloc(stmt->result.buf[ofs].buflen);
				bind[ofs].buffer_type = MYSQL_TYPE_STRING;
				bind[ofs].buffer = stmt->result.buf[ofs].val;
				bind[ofs].is_null = &stmt->result.is_null[ofs];
				bind[ofs].buffer_length = stmt->result.buf[ofs].buflen;
				bind[ofs].length = &stmt->result.buf[ofs].output_len;
				break;
			}
			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Server returned unknown type %ld. Probably your client library is incompatible with the server version you use!", col_type);
				break;
		}
	}

	rc = mysql_stmt_bind_result(stmt->stmt, bind);
	MYSQLI_REPORT_STMT_ERROR(stmt->stmt);

	if (rc) {
		/* dont close the statement or subsequent usage (for example ->execute()) will lead to crash */
		for (i=0; i < var_cnt ; i++) {
			if (stmt->result.buf[i].val) {
				efree(stmt->result.buf[i].val);
			}
		}
		/* Don't free stmt->result.is_null because is_null & buf are one block of memory  */
		efree(stmt->result.buf);
	} else {
		stmt->result.var_cnt = var_cnt;
		stmt->result.vars = (zval **)safe_emalloc((var_cnt), sizeof(zval), 0);
		for (i = start; i < var_cnt+start; i++) {
			ofs = i-start;
			Z_ADDREF_PP(args[i]);
			stmt->result.vars[ofs] = *args[i];
		}
	}
	efree(bind);

	return rc;
}
#else
static int
mysqli_stmt_bind_result_do_bind(MY_STMT *stmt, zval ***args, unsigned int argc, unsigned int start TSRMLS_DC)
{
	unsigned int i;
	MYSQLND_RESULT_BIND * params = mysqlnd_stmt_alloc_result_bind(stmt->stmt);
	if (params) {
		for (i = 0; i < (argc - start); i++) {
			params[i].zv = *(args[i + start]);
		}
		return mysqlnd_stmt_bind_result(stmt->stmt, params);
	}
	return FAIL;
}
#endif
/* }}} */

/* {{{ proto bool mysqli_stmt_bind_result(object stmt, mixed var, [,mixed, ...]) U
   Bind variables to a prepared statement for result storage */
PHP_FUNCTION(mysqli_stmt_bind_result)
{
	zval		***args;
	int			argc = ZEND_NUM_ARGS();
	int			start = 1;
	ulong		rc;
	MY_STMT		*stmt;
	zval		*mysql_stmt;

	if (getThis()) {
		start = 0;
	}

	if (zend_parse_method_parameters((getThis()) ? 0:1 TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	if (argc < (getThis() ? 1 : 2)) {
		WRONG_PARAM_COUNT;
	}

	if ((argc - start) != mysql_stmt_field_count(stmt->stmt)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number of bind variables doesn't match number of fields in prepared statement");
		RETURN_FALSE;
	}

	args = (zval ***)safe_emalloc(argc, sizeof(zval **), 0);

	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}

	rc = mysqli_stmt_bind_result_do_bind(stmt, args, argc, start TSRMLS_CC);

	efree(args);

	RETURN_BOOL(!rc);
}
/* }}} */

/* {{{ proto bool mysqli_change_user(object link, string user, string password, string database)
   Change logged-in user of the active connection */
PHP_FUNCTION(mysqli_change_user)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link = NULL;
	char		*user, *password, *dbname;
	int			user_len, password_len, dbname_len;
	ulong		rc;
#if !defined(MYSQLI_USE_MYSQLND) && defined(HAVE_MYSQLI_SET_CHARSET)
	const		CHARSET_INFO * old_charset;
#endif

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Osss", &mysql_link, mysqli_link_class_entry, &user, &user_len, &password, &password_len, &dbname, &dbname_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

#if !defined(MYSQLI_USE_MYSQLND) && defined(HAVE_MYSQLI_SET_CHARSET)
	old_charset = mysql->mysql->charset;
#endif

#if defined(MYSQLI_USE_MYSQLND)
	rc = mysqlnd_change_user_ex(mysql->mysql, user, password, dbname, FALSE, (size_t) password_len);
#else
	rc = mysql_change_user(mysql->mysql, user, password, dbname);
#endif
	MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);

	if (rc) {
		RETURN_FALSE;
	}
#if !defined(MYSQLI_USE_MYSQLND) && defined(HAVE_MYSQLI_SET_CHARSET)
	if (mysql_get_server_version(mysql->mysql) < 501023L) {
		/*
		  Request the current charset, or it will be reset to the system one.
		  5.0 doesn't support it. Support added in 5.1.23 by fixing the following bug :
		  Bug #30472 libmysql doesn't reset charset, insert_id after succ. mysql_change_user() call
		*/
		rc = mysql_set_character_set(mysql->mysql, old_charset->csname);
	}
#endif

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string mysqli_character_set_name(object link)
   Returns the name of the character set used for this connection */
PHP_FUNCTION(mysqli_character_set_name)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	const char	*cs_name;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);
	cs_name = mysql_character_set_name(mysql->mysql);
	if (cs_name) {
		RETURN_STRING(cs_name, 1);
	}
}
/* }}} */


/* {{{ php_mysqli_close */
void php_mysqli_close(MY_MYSQL * mysql, int close_type, int resource_status TSRMLS_DC)
{
	if (resource_status > MYSQLI_STATUS_INITIALIZED) {
		MyG(num_links)--;
	}

	if (!mysql->persistent) {
		mysqli_close(mysql->mysql, close_type);
	} else {
		zend_rsrc_list_entry *le;
		if (zend_hash_find(&EG(persistent_list), mysql->hash_key, strlen(mysql->hash_key) + 1, (void **)&le) == SUCCESS) {
			if (Z_TYPE_P(le) == php_le_pmysqli()) {
				mysqli_plist_entry *plist = (mysqli_plist_entry *) le->ptr;
#if defined(MYSQLI_USE_MYSQLND)
				mysqlnd_end_psession(mysql->mysql);
#endif
				zend_ptr_stack_push(&plist->free_links, mysql->mysql);

				MyG(num_active_persistent)--;
				MyG(num_inactive_persistent)++;
			}
		}
		mysql->persistent = FALSE;
	}
	mysql->mysql = NULL;

	php_clear_mysql(mysql);
}
/* }}} */


/* {{{ proto bool mysqli_close(object link)
   Close connection */
PHP_FUNCTION(mysqli_close)
{
	zval		*mysql_link;
	MY_MYSQL	*mysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_INITIALIZED);

	php_mysqli_close(mysql, MYSQLI_CLOSE_EXPLICIT, ((MYSQLI_RESOURCE *)((mysqli_object *)zend_object_store_get_object(mysql_link TSRMLS_CC))->ptr)->status TSRMLS_CC);
	((MYSQLI_RESOURCE *)((mysqli_object *)zend_object_store_get_object(mysql_link TSRMLS_CC))->ptr)->status = MYSQLI_STATUS_UNKNOWN;

	MYSQLI_CLEAR_RESOURCE(&mysql_link);
	efree(mysql);
	RETURN_TRUE;
}
/* }}} */


#if !defined(MYSQLI_USE_MYSQLND)
/* {{{ mysqli_tx_cor_options_to_string */
static void mysqli_tx_cor_options_to_string(const MYSQL * const conn, smart_str * str, const unsigned int mode)
{
	if (mode & TRANS_COR_AND_CHAIN && !(mode & TRANS_COR_AND_NO_CHAIN)) {
		if (str->len) {
			smart_str_appendl(str, ", ", sizeof(", ") - 1);
		}
		smart_str_appendl(str, "AND CHAIN", sizeof("AND CHAIN") - 1);
	} else if (mode & TRANS_COR_AND_NO_CHAIN && !(mode & TRANS_COR_AND_CHAIN)) {
		if (str->len) {
			smart_str_appendl(str, ", ", sizeof(", ") - 1);
		}
		smart_str_appendl(str, "AND NO CHAIN", sizeof("AND NO CHAIN") - 1);
	}

	if (mode & TRANS_COR_RELEASE && !(mode & TRANS_COR_NO_RELEASE)) {
		if (str->len) {
			smart_str_appendl(str, ", ", sizeof(", ") - 1);
		}
		smart_str_appendl(str, "RELEASE", sizeof("RELEASE") - 1);
	} else if (mode & TRANS_COR_NO_RELEASE && !(mode & TRANS_COR_RELEASE)) {
		if (str->len) {
			smart_str_appendl(str, ", ", sizeof(", ") - 1);
		}
		smart_str_appendl(str, "NO RELEASE", sizeof("NO RELEASE") - 1);
	}
	smart_str_0(str);
}
/* }}} */


/* {{{ proto bool mysqli_commit_or_rollback_libmysql */
static int mysqli_commit_or_rollback_libmysql(MYSQL * conn, zend_bool commit, const unsigned int mode, const char * const name)
{
	int ret;
	smart_str tmp_str = {0, 0, 0};
	mysqli_tx_cor_options_to_string(conn, &tmp_str, mode);
	smart_str_0(&tmp_str);

	{
		char * commented_name = NULL;
		unsigned int commented_name_len = name? spprintf(&commented_name, 0, " /*%s*/", name):0;
		char * query;
		unsigned int query_len = spprintf(&query, 0, (commit? "COMMIT%s %s":"ROLLBACK%s %s"),
										  commented_name? commented_name:"", tmp_str.c? tmp_str.c:"");
		smart_str_free(&tmp_str);

		ret = mysql_real_query(conn, query, query_len);
		efree(query);
		if (commented_name) {
			efree(commented_name);
		}
	}
}
/* }}} */
#endif


/* {{{ proto bool mysqli_commit(object link)
   Commit outstanding actions and close transaction */
PHP_FUNCTION(mysqli_commit)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	long		flags = TRANS_COR_NO_OPT;
	char *		name = NULL;
	int			name_len = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|ls", &mysql_link, mysqli_link_class_entry, &flags, &name, &name_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

#if !defined(MYSQLI_USE_MYSQLND)
	if (mysqli_commit_or_rollback_libmysql(mysql->mysql, TRUE, flags, name)) {
#else
	if (FAIL == mysqlnd_commit(mysql->mysql, flags, name)) {
#endif
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_data_seek(object result, int offset)
   Move internal result pointer */
PHP_FUNCTION(mysqli_data_seek)
{
	MYSQL_RES	*result;
	zval		*mysql_result;
	long		offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &mysql_result, mysqli_result_class_entry, &offset) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	if (mysqli_result_is_unbuffered(result)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Function cannot be used with MYSQL_USE_RESULT");
		RETURN_FALSE;
	}

	if (offset < 0 || offset >= mysql_num_rows(result)) {
		RETURN_FALSE;
	}

	mysql_data_seek(result, offset);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void mysqli_debug(string debug) U
*/
PHP_FUNCTION(mysqli_debug)
{
	char	*debug;
	int		debug_len;

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
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

	RETURN_BOOL(!mysql_dump_debug_info(mysql->mysql))
}
/* }}} */

/* {{{ proto int mysqli_errno(object link)
   Returns the numerical value of the error message from previous MySQL operation */
PHP_FUNCTION(mysqli_errno)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);
	RETURN_LONG(mysql_errno(mysql->mysql));
}
/* }}} */

/* {{{ proto string mysqli_error(object link)
   Returns the text of the error message from previous MySQL operation */
PHP_FUNCTION(mysqli_error)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	const char	*err;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);
	err = mysql_error(mysql->mysql);
	if (err) {
		RETURN_STRING(err, 1);
	}
}
/* }}} */

#ifndef MYSQLI_USE_MYSQLND
/* {{{ php_mysqli_stmt_copy_it */
static void
php_mysqli_stmt_copy_it(zval *** copies, zval *original, uint param_count, uint current)
{
	if (!*copies) {
		*copies = ecalloc(param_count, sizeof(zval *));
	}
	MAKE_STD_ZVAL((*copies)[current]);
	*(*copies)[current] = *original;
	Z_SET_REFCOUNT_P((*copies)[current], 1);
	zval_copy_ctor((*copies)[current]);
}
/* }}} */
#endif

/* {{{ proto bool mysqli_stmt_execute(object stmt)
   Execute a prepared statement */
PHP_FUNCTION(mysqli_stmt_execute)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;
#ifndef MYSQLI_USE_MYSQLND
	unsigned int	i;
	zval		**copies = NULL;
#endif

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

#ifndef MYSQLI_USE_MYSQLND
	if (stmt->param.var_cnt) {
		int j;
		for (i = 0; i < stmt->param.var_cnt; i++) {
			for (j = i + 1; j < stmt->param.var_cnt; j++) {
				/* Oops, someone binding the same variable - clone */
				if (stmt->param.vars[j] == stmt->param.vars[i] && stmt->param.vars[i]) {
					php_mysqli_stmt_copy_it(&copies, stmt->param.vars[i], stmt->param.var_cnt, i);
					break;
				}
			}
		}
	}
	for (i = 0; i < stmt->param.var_cnt; i++) {
		if (stmt->param.vars[i]) {
			if ( !(stmt->param.is_null[i] = (stmt->param.vars[i]->type == IS_NULL)) ) {
				zval *the_var = copies && copies[i]? copies[i]:stmt->param.vars[i];
				switch (stmt->stmt->params[i].buffer_type) {
					case MYSQL_TYPE_VAR_STRING:
						if (the_var == stmt->param.vars[i] && Z_TYPE_P(stmt->param.vars[i]) != IS_STRING) {
							php_mysqli_stmt_copy_it(&copies, stmt->param.vars[i], stmt->param.var_cnt, i);
							the_var = copies[i];
						}
						convert_to_string_ex(&the_var);
						stmt->stmt->params[i].buffer = Z_STRVAL_P(the_var);
						stmt->stmt->params[i].buffer_length = Z_STRLEN_P(the_var);
						break;
					case MYSQL_TYPE_DOUBLE:
						if (the_var == stmt->param.vars[i] && Z_TYPE_P(stmt->param.vars[i]) != IS_DOUBLE) {
							php_mysqli_stmt_copy_it(&copies, stmt->param.vars[i], stmt->param.var_cnt, i);
							the_var = copies[i];
						}
						convert_to_double_ex(&the_var);
						stmt->stmt->params[i].buffer = &Z_DVAL_P(the_var);
						break;
					case MYSQL_TYPE_LONGLONG:
					case MYSQL_TYPE_LONG:
						if (the_var == stmt->param.vars[i] && Z_TYPE_P(stmt->param.vars[i]) != IS_LONG) {
							php_mysqli_stmt_copy_it(&copies, stmt->param.vars[i], stmt->param.var_cnt, i);
							the_var = copies[i];
						}
						convert_to_long_ex(&the_var);
						stmt->stmt->params[i].buffer = &Z_LVAL_P(the_var);
						break;
					default:
						break;
				}
			}
		}
	}
#endif

	if (mysql_stmt_execute(stmt->stmt)) {
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
		RETVAL_FALSE;
	} else {
		RETVAL_TRUE;
	}

#ifndef MYSQLI_USE_MYSQLND
	if (copies) {
		for (i = 0; i < stmt->param.var_cnt; i++) {
			if (copies[i]) {
				zval_ptr_dtor(&copies[i]);
			}
		}
		efree(copies);
	}
#endif

	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
		php_mysqli_report_index(stmt->query, mysqli_stmt_server_status(stmt->stmt) TSRMLS_CC);
	}
}
/* }}} */

#ifndef MYSQLI_USE_MYSQLND
/* {{{ void mysqli_stmt_fetch_libmysql
   Fetch results from a prepared statement into the bound variables */
void mysqli_stmt_fetch_libmysql(INTERNAL_FUNCTION_PARAMETERS)
{
	MY_STMT		*stmt;
	zval			*mysql_stmt;
	unsigned int	i;
	ulong			ret;
	unsigned int	uval;
	my_ulonglong	llval;


	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	/* reset buffers */
	for (i = 0; i < stmt->result.var_cnt; i++) {
		if (stmt->result.buf[i].type == IS_STRING) {
			memset(stmt->result.buf[i].val, 0, stmt->result.buf[i].buflen);
		}
	}
	ret = mysql_stmt_fetch(stmt->stmt);
#ifdef MYSQL_DATA_TRUNCATED
	if (!ret || ret == MYSQL_DATA_TRUNCATED) {
#else
	if (!ret) {
#endif
		for (i = 0; i < stmt->result.var_cnt; i++) {
			/*
			  QQ: Isn't it quite better to call zval_dtor(). What if the user has
			  assigned a resource, or an array to the bound variable? We are going
			  to leak probably. zval_dtor() will handle also Unicode/Non-unicode mode.
			*/
			/* Even if the string is of length zero there is one byte alloced so efree() in all cases */
			if (Z_TYPE_P(stmt->result.vars[i]) == IS_STRING) {
				STR_FREE(stmt->result.vars[i]->value.str.val);
			}
			if (!stmt->result.is_null[i]) {
				switch (stmt->result.buf[i].type) {
					case IS_LONG:
						if ((stmt->stmt->fields[i].type == MYSQL_TYPE_LONG)
						    && (stmt->stmt->fields[i].flags & UNSIGNED_FLAG))
						{
							/* unsigned int (11) */
							uval= *(unsigned int *) stmt->result.buf[i].val;
#if SIZEOF_LONG==4
							if (uval > INT_MAX) {
								char *tmp, *p;
								int j=10;
								tmp= emalloc(11);
								p= &tmp[9];
								do {
									*p-- = (uval % 10) + 48;
									uval = uval / 10;
								} while (--j > 0);
								tmp[10]= '\0';
								/* unsigned int > INT_MAX is 10 digits - ALWAYS */
								ZVAL_STRINGL(stmt->result.vars[i], tmp, 10, 0);
								break;
							}
#endif
						}
						if (stmt->stmt->fields[i].flags & UNSIGNED_FLAG) {
							ZVAL_LONG(stmt->result.vars[i], *(unsigned int *)stmt->result.buf[i].val);
						} else {
							ZVAL_LONG(stmt->result.vars[i], *(int *)stmt->result.buf[i].val);
						}
						break;
					case IS_DOUBLE:
						ZVAL_DOUBLE(stmt->result.vars[i], *(double *)stmt->result.buf[i].val);
						break;
					case IS_STRING:
						if (stmt->stmt->bind[i].buffer_type == MYSQL_TYPE_LONGLONG
#if MYSQL_VERSION_ID > 50002
						 || stmt->stmt->bind[i].buffer_type == MYSQL_TYPE_BIT
#endif
						 ) {
							my_bool uns= (stmt->stmt->fields[i].flags & UNSIGNED_FLAG)? 1:0;
#if MYSQL_VERSION_ID > 50002
							if (stmt->stmt->bind[i].buffer_type == MYSQL_TYPE_BIT) {
								switch (stmt->result.buf[i].output_len) {
									case 8:llval = (my_ulonglong)  bit_uint8korr(stmt->result.buf[i].val);break;
									case 7:llval = (my_ulonglong)  bit_uint7korr(stmt->result.buf[i].val);break;
									case 6:llval = (my_ulonglong)  bit_uint6korr(stmt->result.buf[i].val);break;
									case 5:llval = (my_ulonglong)  bit_uint5korr(stmt->result.buf[i].val);break;
									case 4:llval = (my_ulonglong)  bit_uint4korr(stmt->result.buf[i].val);break;
									case 3:llval = (my_ulonglong)  bit_uint3korr(stmt->result.buf[i].val);break;
									case 2:llval = (my_ulonglong)  bit_uint2korr(stmt->result.buf[i].val);break;
									case 1:llval = (my_ulonglong)  uint1korr(stmt->result.buf[i].val);break;
								}
							} else
#endif
							{
								llval= *(my_ulonglong *) stmt->result.buf[i].val;
							}
#if SIZEOF_LONG==8
							if (uns && llval > 9223372036854775807L) {
#elif SIZEOF_LONG==4
							if ((uns && llval > L64(2147483647)) ||
								(!uns && (( L64(2147483647) < (my_longlong) llval) ||
								(L64(-2147483648) > (my_longlong) llval))))
							{
#endif
								char tmp[22];
								/* even though lval is declared as unsigned, the value
								 * may be negative. Therefor we cannot use MYSQLI_LLU_SPEC and must
								 * use MYSQLI_LL_SPEC.
								 */
								snprintf(tmp, sizeof(tmp), (stmt->stmt->fields[i].flags & UNSIGNED_FLAG)? MYSQLI_LLU_SPEC : MYSQLI_LL_SPEC, llval);
								ZVAL_STRING(stmt->result.vars[i], tmp, 1);
							} else {
								ZVAL_LONG(stmt->result.vars[i], llval);
							}
						} else {
#if defined(MYSQL_DATA_TRUNCATED) && MYSQL_VERSION_ID > 50002
							if (ret == MYSQL_DATA_TRUNCATED && *(stmt->stmt->bind[i].error) != 0) {
								/* result was truncated */
								ZVAL_STRINGL(stmt->result.vars[i], stmt->result.buf[i].val,
											 stmt->stmt->bind[i].buffer_length, 1);
							} else {
#else
							{
#endif
								ZVAL_STRINGL(stmt->result.vars[i], stmt->result.buf[i].val,
											 stmt->result.buf[i].output_len, 1);
							}
						}
						break;
					default:
						break;
				}
			} else {
				ZVAL_NULL(stmt->result.vars[i]);
			}
		}
	} else {
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
	}

	switch (ret) {
		case 0:
#ifdef MYSQL_DATA_TRUNCATED
		/* according to SQL standard truncation (e.g. loss of precision is
		   not an error) - for detecting possible truncation you have to
		   check mysqli_stmt_warning
		*/
		case MYSQL_DATA_TRUNCATED:
#endif
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
#else
/* {{{ mixed mysqli_stmt_fetch_mysqlnd */
void mysqli_stmt_fetch_mysqlnd(INTERNAL_FUNCTION_PARAMETERS)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;
	zend_bool	fetched_anything;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	if (FAIL  == mysqlnd_stmt_fetch(stmt->stmt, &fetched_anything)) {
		RETURN_BOOL(FALSE);
	} else if (fetched_anything == TRUE) {
		RETURN_BOOL(TRUE);
	} else {
		RETURN_NULL();
	}
}
#endif
/* }}} */


/* {{{ proto mixed mysqli_stmt_fetch(object stmt) U
   Fetch results from a prepared statement into the bound variables */
PHP_FUNCTION(mysqli_stmt_fetch)
{
#if !defined(MYSQLI_USE_MYSQLND)
	mysqli_stmt_fetch_libmysql(INTERNAL_FUNCTION_PARAM_PASSTHRU);
#else
	mysqli_stmt_fetch_mysqlnd(INTERNAL_FUNCTION_PARAM_PASSTHRU);
#endif
}
/* }}} */

/* {{{  php_add_field_properties */
static void php_add_field_properties(zval *value, const MYSQL_FIELD *field TSRMLS_DC)
{
	add_property_string(value, "name",(field->name ? field->name : ""), 1);
	add_property_string(value, "orgname",(field->org_name ? field->org_name : ""), 1);
	add_property_string(value, "table",(field->table ? field->table : ""), 1);
	add_property_string(value, "orgtable",(field->org_table ? field->org_table : ""), 1);
	add_property_string(value, "def",(field->def ? field->def : ""), 1);
	add_property_string(value, "db",(field->db ? field->db : ""), 1);

	/* FIXME: manually set the catalog to "def" due to bug in
	 * libmysqlclient which does not initialize field->catalog
	 * and in addition, the catalog is always be "def"
	 */
	add_property_string(value, "catalog", "def", 1);

	add_property_long(value, "max_length", field->max_length);
	add_property_long(value, "length", field->length);
	add_property_long(value, "charsetnr", field->charsetnr);
	add_property_long(value, "flags", field->flags);
	add_property_long(value, "type", field->type);
	add_property_long(value, "decimals", field->decimals);
}
/* }}} */

/* {{{ proto mixed mysqli_fetch_field (object result)
   Get column information from a result and return as an object */
PHP_FUNCTION(mysqli_fetch_field)
{
	MYSQL_RES	*result;
	zval		*mysql_result;
	const MYSQL_FIELD	*field;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	if (!(field = mysql_fetch_field(result))) {
		RETURN_FALSE;
	}

	object_init(return_value);
	php_add_field_properties(return_value, field TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed mysqli_fetch_fields (object result)
   Return array of objects containing field meta-data */
PHP_FUNCTION(mysqli_fetch_fields)
{
	MYSQL_RES	*result;
	zval		*mysql_result;
	zval		*obj;

	unsigned int i;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	array_init(return_value);

	for (i = 0; i < mysql_num_fields(result); i++) {
		const MYSQL_FIELD *field = mysql_fetch_field_direct(result, i);

		MAKE_STD_ZVAL(obj);
		object_init(obj);

		php_add_field_properties(obj, field TSRMLS_CC);
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
	const MYSQL_FIELD	*field;
	long		offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &mysql_result, mysqli_result_class_entry, &offset) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	if (offset < 0 || offset >= (long) mysql_num_fields(result)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Field offset is invalid for resultset");
		RETURN_FALSE;
	}

	if (!(field = mysql_fetch_field_direct(result,offset))) {
		RETURN_FALSE;
	}

	object_init(return_value);
	php_add_field_properties(return_value, field TSRMLS_CC);
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

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

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
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

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
	unsigned long	fieldnr;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &mysql_result, mysqli_result_class_entry, &fieldnr) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	if (fieldnr < 0 || fieldnr >= mysql_num_fields(result)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid field offset");
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
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

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
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	mysqli_free_result(result, FALSE);
	MYSQLI_CLEAR_RESOURCE(&mysql_result);
}
/* }}} */

/* {{{ proto string mysqli_get_client_info(void)
   Get MySQL client info */
PHP_FUNCTION(mysqli_get_client_info)
{
	const char * info = mysql_get_client_info();
	if (info) {
		RETURN_STRING(info, 1);
	}
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
	zval		*mysql_link = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);
#if !defined(MYSQLI_USE_MYSQLND)
	RETURN_STRING((mysql->mysql->host_info) ? mysql->mysql->host_info : "", 1);
#else
	RETURN_STRING((mysql->mysql->data->host_info) ? mysql->mysql->data->host_info : "", 1);
#endif
}
/* }}} */

/* {{{ proto int mysqli_get_proto_info(object link)
   Get MySQL protocol information */
PHP_FUNCTION(mysqli_get_proto_info)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);
	RETURN_LONG(mysql_get_proto_info(mysql->mysql));
}
/* }}} */

/* {{{ proto string mysqli_get_server_info(object link)
   Get MySQL server info */
PHP_FUNCTION(mysqli_get_server_info)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link = NULL;
	const char	*info;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

	info = mysql_get_server_info(mysql->mysql);
	if (info) {
		RETURN_STRING(info, 1);
	}
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
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

	RETURN_LONG(mysql_get_server_version(mysql->mysql));
}
/* }}} */

/* {{{ proto string mysqli_info(object link)
   Get information about the most recent query */
PHP_FUNCTION(mysqli_info)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link = NULL;
	const char	*info;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

	info = mysql_info(mysql->mysql);
	if (info) {
		RETURN_STRING(info, 1);
	}
}
/* }}} */


/* {{{ php_mysqli_init() */
void php_mysqli_init(INTERNAL_FUNCTION_PARAMETERS)
{
	MYSQLI_RESOURCE *mysqli_resource;
	MY_MYSQL *mysql;

	if (getThis() && ((mysqli_object *) zend_object_store_get_object(getThis() TSRMLS_CC))->ptr) {
		return;
	}

	mysql = (MY_MYSQL *)ecalloc(1, sizeof(MY_MYSQL));

#if !defined(MYSQLI_USE_MYSQLND)
	if (!(mysql->mysql = mysql_init(NULL)))
#else
	/*
	  We create always persistent, as if the user want to connecto
	  to p:somehost, we can't convert the handle then
	*/
	if (!(mysql->mysql = mysql_init(TRUE)))
#endif
	{
		efree(mysql);
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)mysql;
	mysqli_resource->status = MYSQLI_STATUS_INITIALIZED;

	if (!getThis() || !instanceof_function(Z_OBJCE_P(getThis()), mysqli_link_class_entry TSRMLS_CC)) {
		MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_link_class_entry);
	} else {
		((mysqli_object *) zend_object_store_get_object(getThis() TSRMLS_CC))->ptr = mysqli_resource;
	}
}
/* }}} */


/* {{{ proto resource mysqli_init(void)
   Initialize mysqli and return a resource for use with mysql_real_connect */
PHP_FUNCTION(mysqli_init)
{
	php_mysqli_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto mixed mysqli_insert_id(object link)
   Get the ID generated from the previous INSERT operation */
PHP_FUNCTION(mysqli_insert_id)
{
	MY_MYSQL		*mysql;
	my_ulonglong	rc;
	zval			*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);
	rc = mysql_insert_id(mysql->mysql);
	MYSQLI_RETURN_LONG_LONG(rc)
}
/* }}} */

/* {{{ proto bool mysqli_kill(object link, int processid)
   Kill a mysql process on the server */
PHP_FUNCTION(mysqli_kill)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	long		processid;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &mysql_link, mysqli_link_class_entry, &processid) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

	if (processid <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "processid should have positive value");
		RETURN_FALSE;
	}

	if (mysql_kill(mysql->mysql, processid)) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_more_results(object link)
   check if there any more query results from a multi query */
PHP_FUNCTION(mysqli_more_results)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

	RETURN_BOOL(mysql_more_results(mysql->mysql));
}
/* }}} */

/* {{{ proto bool mysqli_next_result(object link)
   read next result from multi_query */
PHP_FUNCTION(mysqli_next_result) {
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

	if (!mysql_more_results(mysql->mysql)) {
		php_error_docref(NULL TSRMLS_CC, E_STRICT, "There is no next result set. "
						"Please, call mysqli_more_results()/mysqli::more_results() to check "
						"whether to call this function/method");
	}

	RETURN_BOOL(!mysql_next_result(mysql->mysql));
}
/* }}} */

#if defined(HAVE_STMT_NEXT_RESULT) && defined(MYSQLI_USE_MYSQLND)
/* {{{ proto bool mysqli_stmt_next_result(object link)
   check if there any more query results from a multi query */
PHP_FUNCTION(mysqli_stmt_more_results)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	RETURN_BOOL(mysqlnd_stmt_more_results(stmt->stmt));
}
/* }}} */


/* {{{ proto bool mysqli_stmt_next_result(object link)
   read next result from multi_query */
PHP_FUNCTION(mysqli_stmt_next_result) {
	MY_STMT		*stmt;
	zval		*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	if (!mysqlnd_stmt_more_results(stmt->stmt)) {
		php_error_docref(NULL TSRMLS_CC, E_STRICT, "There is no next result set. "
						"Please, call mysqli_stmt_more_results()/mysqli_stmt::more_results() to check "
						"whether to call this function/method");
	}

	RETURN_BOOL(!mysql_stmt_next_result(stmt->stmt));
}
/* }}} */
#endif


/* {{{ proto int mysqli_num_fields(object result)
   Get number of fields in result */
PHP_FUNCTION(mysqli_num_fields)
{
	MYSQL_RES	*result;
	zval		*mysql_result;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

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
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	if (mysqli_result_is_unbuffered_and_not_everything_is_fetched(result)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Function cannot be used with MYSQL_USE_RESULT");
		RETURN_LONG(0);
	}

	MYSQLI_RETURN_LONG_LONG(mysql_num_rows(result));
}
/* }}} */

/* {{{ mysqli_options_get_option_zval_type */
static int mysqli_options_get_option_zval_type(int option)
{
	switch (option) {
#ifdef MYSQLI_USE_MYSQLND
#if PHP_MAJOR_VERSION >= 6
		case MYSQLND_OPT_NUMERIC_AND_DATETIME_AS_UNICODE:
#endif
		case MYSQLND_OPT_NET_CMD_BUFFER_SIZE:
		case MYSQLND_OPT_NET_READ_BUFFER_SIZE:
#ifdef MYSQLND_STRING_TO_INT_CONVERSION
		case MYSQLND_OPT_INT_AND_FLOAT_NATIVE:
#endif
#endif /* MYSQLI_USE_MYSQLND */
		case MYSQL_OPT_CONNECT_TIMEOUT:
#ifdef MYSQL_REPORT_DATA_TRUNCATION
		case MYSQL_REPORT_DATA_TRUNCATION:
#endif
		case MYSQL_OPT_LOCAL_INFILE:
		case MYSQL_OPT_NAMED_PIPE:
#ifdef MYSQL_OPT_PROTOCOL
                case MYSQL_OPT_PROTOCOL:
#endif /* MySQL 4.1.0 */
#ifdef MYSQL_OPT_READ_TIMEOUT
		case MYSQL_OPT_READ_TIMEOUT:
		case MYSQL_OPT_WRITE_TIMEOUT:
		case MYSQL_OPT_GUESS_CONNECTION:
		case MYSQL_OPT_USE_EMBEDDED_CONNECTION:
		case MYSQL_OPT_USE_REMOTE_CONNECTION:
		case MYSQL_SECURE_AUTH:
#endif /* MySQL 4.1.1 */
#ifdef MYSQL_OPT_RECONNECT
		case MYSQL_OPT_RECONNECT:
#endif /* MySQL 5.0.13 */
#ifdef MYSQL_OPT_SSL_VERIFY_SERVER_CERT
		case MYSQL_OPT_SSL_VERIFY_SERVER_CERT:
#endif /* MySQL 5.0.23 */
#ifdef MYSQL_OPT_COMPRESS
		case MYSQL_OPT_COMPRESS:
#endif /* mysqlnd @ PHP 5.3.2 */
#ifdef MYSQL_OPT_SSL_VERIFY_SERVER_CERT
	REGISTER_LONG_CONSTANT("MYSQLI_OPT_SSL_VERIFY_SERVER_CERT", MYSQL_OPT_SSL_VERIFY_SERVER_CERT, CONST_CS | CONST_PERSISTENT);
#endif /* MySQL 5.1.1., mysqlnd @ PHP 5.3.3 */
#if (MYSQL_VERSION_ID >= 50611 && defined(CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS)) || defined(MYSQLI_USE_MYSQLND)
		case MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS:
#endif
			return IS_LONG;

#ifdef MYSQL_SHARED_MEMORY_BASE_NAME
                case MYSQL_SHARED_MEMORY_BASE_NAME:
#endif /* MySQL 4.1.0 */
#ifdef MYSQL_SET_CLIENT_IP
		case MYSQL_SET_CLIENT_IP:
#endif /* MySQL 4.1.1 */
		case MYSQL_READ_DEFAULT_FILE:
		case MYSQL_READ_DEFAULT_GROUP:
		case MYSQL_INIT_COMMAND:
		case MYSQL_SET_CHARSET_NAME:
		case MYSQL_SET_CHARSET_DIR:
#if MYSQL_VERSION_ID > 50605 || defined(MYSQLI_USE_MYSQLND)
		case MYSQL_SERVER_PUBLIC_KEY:
#endif
			return IS_STRING;

		default:
			return IS_NULL;
	}
}
/* }}} */


/* {{{ proto bool mysqli_options(object link, int flags, mixed values)
   Set options */
PHP_FUNCTION(mysqli_options)
{
	MY_MYSQL		*mysql;
	zval			*mysql_link = NULL;
	zval			**mysql_value;
	long			mysql_option;
	unsigned int	l_value;
	long			ret;
	int				expected_type;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OlZ", &mysql_link, mysqli_link_class_entry, &mysql_option, &mysql_value) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_INITIALIZED);

#if PHP_API_VERSION < 20100412
	if ((PG(open_basedir) && PG(open_basedir)[0] != '\0') || PG(safe_mode)) {
#else
	if (PG(open_basedir) && PG(open_basedir)[0] != '\0') {
#endif
		if(mysql_option == MYSQL_OPT_LOCAL_INFILE) {
			RETURN_FALSE;
		}
	}
	expected_type = mysqli_options_get_option_zval_type(mysql_option);
	if (expected_type != Z_TYPE_PP(mysql_value)) {
		switch (expected_type) {
			case IS_STRING:
				convert_to_string_ex(mysql_value);
				break;
			case IS_LONG:
				convert_to_long_ex(mysql_value);
				break;
			default:
				break;
		}
	}
	switch (expected_type) {
		case IS_STRING:
			ret = mysql_options(mysql->mysql, mysql_option, Z_STRVAL_PP(mysql_value));
			break;
		case IS_LONG:
			l_value = Z_LVAL_PP(mysql_value);
			ret = mysql_options(mysql->mysql, mysql_option, (char *)&l_value);
			break;
		default:
			ret = 1;
			break;
	}

	RETURN_BOOL(!ret);
}
/* }}} */


/* {{{ proto bool mysqli_ping(object link)
   Ping a server connection or reconnect if there is no connection */
PHP_FUNCTION(mysqli_ping)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	long		rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);
	rc = mysql_ping(mysql->mysql);
	MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);

	RETURN_BOOL(!rc);
}
/* }}} */

/* {{{ proto mixed mysqli_prepare(object link, string query)
   Prepare a SQL statement for execution */
PHP_FUNCTION(mysqli_prepare)
{
	MY_MYSQL		*mysql;
	MY_STMT			*stmt;
	char			*query = NULL;
	int				query_len;
	zval			*mysql_link;
	MYSQLI_RESOURCE	*mysqli_resource;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os",&mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

#if !defined(MYSQLI_USE_MYSQLND)
	if (mysql->mysql->status == MYSQL_STATUS_GET_RESULT) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "All data must be fetched before a new statement prepare takes place");
		RETURN_FALSE;
	}
#endif

	stmt = (MY_STMT *)ecalloc(1,sizeof(MY_STMT));

	if ((stmt->stmt = mysql_stmt_init(mysql->mysql))) {
		if (mysql_stmt_prepare(stmt->stmt, query, query_len)) {
			/* mysql_stmt_close() clears errors, so we have to store them temporarily */
#if !defined(MYSQLI_USE_MYSQLND)
			char  last_error[MYSQL_ERRMSG_SIZE];
			char  sqlstate[SQLSTATE_LENGTH+1];
			unsigned int last_errno;

			last_errno = stmt->stmt->last_errno;
			memcpy(last_error, stmt->stmt->last_error, MYSQL_ERRMSG_SIZE);
			memcpy(sqlstate, mysql->mysql->net.sqlstate, SQLSTATE_LENGTH+1);
#else
			MYSQLND_ERROR_INFO error_info = *mysql->mysql->data->error_info;
#endif
			mysqli_stmt_close(stmt->stmt, FALSE);
			stmt->stmt = NULL;

			/* restore error messages */
#if !defined(MYSQLI_USE_MYSQLND)
			mysql->mysql->net.last_errno = last_errno;
			memcpy(mysql->mysql->net.last_error, last_error, MYSQL_ERRMSG_SIZE);
			memcpy(mysql->mysql->net.sqlstate, sqlstate, SQLSTATE_LENGTH+1);
#else
			*mysql->mysql->data->error_info = error_info;
#endif
		}
	}

	/* don't initialize stmt->query with NULL, we ecalloc()-ed the memory */
	/* Get performance boost if reporting is switched off */
	if (stmt->stmt && query_len && (MyG(report_mode) & MYSQLI_REPORT_INDEX)) {
		stmt->query = (char *)emalloc(query_len + 1);
		memcpy(stmt->query, query, query_len);
		stmt->query[query_len] = '\0';
	}

	/* don't join to the previous if because it won't work if mysql_stmt_prepare_fails */
	if (!stmt->stmt) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		efree(stmt);
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)stmt;

	/* change status */
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_stmt_class_entry);
}
/* }}} */


/* {{{ proto bool mysqli_real_connect(object link [,string hostname [,string username [,string passwd [,string dbname [,int port [,string socket [,int flags]]]]]]])
   Open a connection to a mysql server */
PHP_FUNCTION(mysqli_real_connect)
{
	mysqli_common_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, TRUE, FALSE);
}
/* }}} */


/* {{{ proto bool mysqli_real_query(object link, string query)
   Binary-safe version of mysql_query() */
PHP_FUNCTION(mysqli_real_query)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	char		*query = NULL;
	int			query_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

	MYSQLI_DISABLE_MQ; /* disable multi statements/queries */

	if (mysql_real_query(mysql->mysql, query, query_len)) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}

	if (!mysql_field_count(mysql->mysql)) {
		if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
			php_mysqli_report_index(query, mysqli_server_status(mysql->mysql) TSRMLS_CC);
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
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

	newstr = safe_emalloc(2, escapestr_len, 1);
	newstr_len = mysql_real_escape_string(mysql->mysql, newstr, escapestr, escapestr_len);
	newstr = erealloc(newstr, newstr_len + 1);

	RETURN_STRINGL(newstr, newstr_len, 0);
}
/* }}} */


/* {{{ proto bool mysqli_rollback(object link)
   Undo actions from current transaction */
PHP_FUNCTION(mysqli_rollback)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	long		flags = TRANS_COR_NO_OPT;
	char *		name = NULL;
	int			name_len = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|ls", &mysql_link, mysqli_link_class_entry, &flags, &name, &name_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

#if !defined(MYSQLI_USE_MYSQLND)
	if (mysqli_commit_or_rollback_libmysql(mysql->mysql, FALSE, flags, name)) {
#else
	if (FAIL == mysqlnd_rollback(mysql->mysql, flags, name)) {
#endif
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_stmt_send_long_data(object stmt, int param_nr, string data)
*/
PHP_FUNCTION(mysqli_stmt_send_long_data)
{
	MY_STMT *stmt;
	zval	*mysql_stmt;
	char	*data;
	long	param_nr;
	int		data_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ols", &mysql_stmt, mysqli_stmt_class_entry, &param_nr, &data, &data_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

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


/* {{{ proto mixed mysqli_stmt_affected_rows(object stmt)
   Return the number of rows affected in the last query for the given link */
PHP_FUNCTION(mysqli_stmt_affected_rows)
{
	MY_STMT			*stmt;
	zval			*mysql_stmt;
	my_ulonglong	rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	rc = mysql_stmt_affected_rows(stmt->stmt);
	if (rc == (my_ulonglong) -1) {
		RETURN_LONG(-1);
	}
	MYSQLI_RETURN_LONG_LONG(rc)
}
/* }}} */

/* {{{ proto bool mysqli_stmt_close(object stmt)
   Close statement */
PHP_FUNCTION(mysqli_stmt_close)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	mysqli_stmt_close(stmt->stmt, FALSE);
	stmt->stmt = NULL;
	php_clear_stmt_bind(stmt TSRMLS_CC);
	MYSQLI_CLEAR_RESOURCE(&mysql_stmt);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void mysqli_stmt_data_seek(object stmt, int offset)
   Move internal result pointer */
PHP_FUNCTION(mysqli_stmt_data_seek)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;
	long		offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &mysql_stmt, mysqli_stmt_class_entry, &offset) == FAILURE) {
		return;
	}
	if (offset < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset must be positive");
		RETURN_FALSE;
	}

	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	mysql_stmt_data_seek(stmt->stmt, offset);
}
/* }}} */

/* {{{ proto int mysqli_stmt_field_count(object stmt) {
   Return the number of result columns for the given statement */
PHP_FUNCTION(mysqli_stmt_field_count)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	RETURN_LONG(mysql_stmt_field_count(stmt->stmt));
}
/* }}} */

/* {{{ proto void mysqli_stmt_free_result(object stmt)
   Free stored result memory for the given statement handle */
PHP_FUNCTION(mysqli_stmt_free_result)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	mysql_stmt_free_result(stmt->stmt);
}
/* }}} */

/* {{{ proto mixed mysqli_stmt_insert_id(object stmt)
   Get the ID generated from the previous INSERT operation */
PHP_FUNCTION(mysqli_stmt_insert_id)
{
	MY_STMT			*stmt;
	my_ulonglong	rc;
	zval			*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);
	rc = mysql_stmt_insert_id(stmt->stmt);
	MYSQLI_RETURN_LONG_LONG(rc)
}
/* }}} */

/* {{{ proto int mysqli_stmt_param_count(object stmt)
   Return the number of parameter for the given statement */
PHP_FUNCTION(mysqli_stmt_param_count)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	RETURN_LONG(mysql_stmt_param_count(stmt->stmt));
}
/* }}} */

/* {{{ proto bool mysqli_stmt_reset(object stmt)
   reset a prepared statement */
PHP_FUNCTION(mysqli_stmt_reset)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	if (mysql_stmt_reset(stmt->stmt)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed mysqli_stmt_num_rows(object stmt)
   Return the number of rows in statements result set */
PHP_FUNCTION(mysqli_stmt_num_rows)
{
	MY_STMT			*stmt;
	zval			*mysql_stmt;
	my_ulonglong	rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	rc = mysql_stmt_num_rows(stmt->stmt);
	MYSQLI_RETURN_LONG_LONG(rc)
}
/* }}} */

/* {{{ proto bool mysqli_select_db(object link, string dbname)
   Select a MySQL database */
PHP_FUNCTION(mysqli_select_db)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	char		*dbname;
	int			dbname_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &dbname, &dbname_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

	if (mysql_select_db(mysql->mysql, dbname)) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string mysqli_sqlstate(object link)
   Returns the SQLSTATE error from previous MySQL operation */
PHP_FUNCTION(mysqli_sqlstate)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	const char	*state;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);
	state = mysql_sqlstate(mysql->mysql);
	if (state) {
		RETURN_STRING(state, 1);
	}
}
/* }}} */

/* {{{ proto bool mysqli_ssl_set(object link ,string key ,string cert ,string ca ,string capath ,string cipher]) U
*/
PHP_FUNCTION(mysqli_ssl_set)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	char		*ssl_parm[5];
	int			ssl_parm_len[5], i;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Osssss", &mysql_link, mysqli_link_class_entry, &ssl_parm[0], &ssl_parm_len[0], &ssl_parm[1], &ssl_parm_len[1], &ssl_parm[2], &ssl_parm_len[2], &ssl_parm[3], &ssl_parm_len[3], &ssl_parm[4], &ssl_parm_len[4])   == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_INITIALIZED);

	for (i = 0; i < 5; i++) {
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
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	char		*stat;
#if defined(MYSQLI_USE_MYSQLND)
	uint		stat_len;
#endif

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

#if !defined(MYSQLI_USE_MYSQLND)
	if ((stat = (char *)mysql_stat(mysql->mysql)))
	{
		RETURN_STRING(stat, 1);
#else
	if (mysqlnd_stat(mysql->mysql, &stat, &stat_len) == PASS)
	{
		RETURN_STRINGL(stat, stat_len, 0);
#endif
	} else {
		RETURN_FALSE;
	}
}

/* }}} */

/* {{{ proto bool mysqli_refresh(object link, long options)
   Flush tables or caches, or reset replication server information */
PHP_FUNCTION(mysqli_refresh)
{
	MY_MYSQL *mysql;
	zval *mysql_link = NULL;
	long options;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &mysql_link, mysqli_link_class_entry, &options) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_INITIALIZED);
#ifdef MYSQLI_USE_MYSQLND
	RETURN_BOOL(!mysql_refresh(mysql->mysql, (uint8_t) options));
#else
	RETURN_BOOL(!mysql_refresh(mysql->mysql, options));
#endif
}
/* }}} */

/* {{{ proto int mysqli_stmt_attr_set(object stmt, long attr, long mode)
*/
PHP_FUNCTION(mysqli_stmt_attr_set)
{
	MY_STMT	*stmt;
	zval	*mysql_stmt;
	long	mode_in;
#if MYSQL_VERSION_ID >= 50107
	my_bool	mode_b;
#endif
	ulong	mode;
	ulong	attr;
	void	*mode_p;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oll", &mysql_stmt, mysqli_stmt_class_entry, &attr, &mode_in) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	if (mode_in < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "mode should be non-negative, %ld passed", mode_in);
		RETURN_FALSE;
	}

	switch (attr) {
#if MYSQL_VERSION_ID >= 50107
	case STMT_ATTR_UPDATE_MAX_LENGTH:
		mode_b = (my_bool) mode_in;
		mode_p = &mode_b;
		break;
#endif
	default:
		mode = mode_in;
		mode_p = &mode;
		break;
	}
#if !defined(MYSQLI_USE_MYSQLND)
	if (mysql_stmt_attr_set(stmt->stmt, attr, mode_p)) {
#else
	if (FAIL == mysql_stmt_attr_set(stmt->stmt, attr, mode_p)) {
#endif
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
	zval	*mysql_stmt;
	ulong	value = 0;
	ulong	attr;
	int		rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &mysql_stmt, mysqli_stmt_class_entry, &attr) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	if ((rc = mysql_stmt_attr_get(stmt->stmt, attr, &value))) {
		RETURN_FALSE;
	}

#if MYSQL_VERSION_ID >= 50107
	if (attr == STMT_ATTR_UPDATE_MAX_LENGTH)
		value = *((my_bool *)&value);
#endif
	RETURN_LONG((long)value);
}
/* }}} */

/* {{{ proto int mysqli_stmt_errno(object stmt)
*/
PHP_FUNCTION(mysqli_stmt_errno)
{
	MY_STMT	*stmt;
	zval	*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_INITIALIZED);

	RETURN_LONG(mysql_stmt_errno(stmt->stmt));
}
/* }}} */

/* {{{ proto string mysqli_stmt_error(object stmt)
*/
PHP_FUNCTION(mysqli_stmt_error)
{
	MY_STMT	*stmt;
	zval 	*mysql_stmt;
	const char * err;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_INITIALIZED);

	err = mysql_stmt_error(stmt->stmt);
	if (err) {
		RETURN_STRING(err, 1);
	}
}
/* }}} */

/* {{{ proto mixed mysqli_stmt_init(object link)
   Initialize statement object
*/
PHP_FUNCTION(mysqli_stmt_init)
{
	MY_MYSQL		*mysql;
	MY_STMT			*stmt;
	zval			*mysql_link;
	MYSQLI_RESOURCE	*mysqli_resource;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O",&mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

	stmt = (MY_STMT *)ecalloc(1,sizeof(MY_STMT));

	if (!(stmt->stmt = mysql_stmt_init(mysql->mysql))) {
		efree(stmt);
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->status = MYSQLI_STATUS_INITIALIZED;
	mysqli_resource->ptr = (void *)stmt;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_stmt_class_entry);
}
/* }}} */

/* {{{ proto bool mysqli_stmt_prepare(object stmt, string query)
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
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_INITIALIZED);

	if (mysql_stmt_prepare(stmt->stmt, query, query_len)) {
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
		RETURN_FALSE;
	}
	/* change status */
	MYSQLI_SET_STATUS(&mysql_stmt, MYSQLI_STATUS_VALID);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed mysqli_stmt_result_metadata(object stmt)
   return result set from statement */
PHP_FUNCTION(mysqli_stmt_result_metadata)
{
	MY_STMT			*stmt;
	MYSQL_RES		*result;
	zval			*mysql_stmt;
	MYSQLI_RESOURCE	*mysqli_resource;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	if (!(result = mysql_stmt_result_metadata(stmt->stmt))){
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_result_class_entry);
}
/* }}} */

/* {{{ proto bool mysqli_stmt_store_result(stmt)
*/
PHP_FUNCTION(mysqli_stmt_store_result)
{
	MY_STMT	*stmt;
	zval	*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

#if !defined(MYSQLI_USE_MYSQLND)
	{
		/*
		  If the user wants to store the data and we have BLOBs/TEXTs we try to allocate
		  not the maximal length of the type (which is 16MB even for LONGBLOB) but
		  the maximal length of the field in the result set. If he/she has quite big
		  BLOB/TEXT columns after calling store_result() the memory usage of PHP will
		  double - but this is a known problem of the simple MySQL API ;)
		*/
		int	i = 0;

		for (i = mysql_stmt_field_count(stmt->stmt) - 1; i >=0; --i) {
			if (stmt->stmt->fields && (stmt->stmt->fields[i].type == MYSQL_TYPE_BLOB ||
				stmt->stmt->fields[i].type == MYSQL_TYPE_MEDIUM_BLOB ||
				stmt->stmt->fields[i].type == MYSQL_TYPE_LONG_BLOB ||
				stmt->stmt->fields[i].type == MYSQL_TYPE_GEOMETRY))
			{
#if MYSQL_VERSION_ID >= 50107
				my_bool	tmp=1;
#else
				uint tmp=1;
#endif
				mysql_stmt_attr_set(stmt->stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &tmp);
				break;
			}
		}
	}
#endif

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
	MY_STMT	*stmt;
	zval	*mysql_stmt;
	const char * state;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, &mysql_stmt, MYSQLI_STATUS_VALID);

	state = mysql_stmt_sqlstate(stmt->stmt);
	if (state) {
		RETURN_STRING(state, 1);
	}
}
/* }}} */

/* {{{ proto object mysqli_store_result(object link)
   Buffer result set on client */
PHP_FUNCTION(mysqli_store_result)
{
	MY_MYSQL		*mysql;
	MYSQL_RES		*result;
	zval			*mysql_link;
	MYSQLI_RESOURCE	*mysqli_resource;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

	if (!(result = mysql_store_result(mysql->mysql))) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}
	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
		php_mysqli_report_index("from previous query", mysqli_server_status(mysql->mysql) TSRMLS_CC);
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_result_class_entry);
}
/* }}} */


/* {{{ proto int mysqli_thread_id(object link)
   Return the current thread ID */
PHP_FUNCTION(mysqli_thread_id)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

	RETURN_LONG((long) mysql_thread_id(mysql->mysql));
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
	MY_MYSQL		*mysql;
	MYSQL_RES		*result;
	zval			*mysql_link;
	MYSQLI_RESOURCE	*mysqli_resource;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

	if (!(result = mysql_use_result(mysql->mysql))) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}

	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
		php_mysqli_report_index("from previous query", mysqli_server_status(mysql->mysql) TSRMLS_CC);
	}
	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_result_class_entry);
}
/* }}} */

/* {{{ proto int mysqli_warning_count (object link)
   Return number of warnings from the last query for the given link */
PHP_FUNCTION(mysqli_warning_count)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, &mysql_link, MYSQLI_STATUS_VALID);

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
