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
  | Author: George Schlossnagle <george@omniti.com>                      |
  |         Wez Furlong <wez@php.net>                                    |
  |         Johannes Schlueter <johannes@mysql.com>                      |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "php_pdo_mysql.h"
#include "php_pdo_mysql_int.h"
#ifndef PDO_USE_MYSQLND
#include <mysqld_error.h>
#endif
#include "zend_exceptions.h"

#if defined(PDO_USE_MYSQLND)
#	define pdo_mysql_init(persistent) mysqlnd_init(MYSQLND_CLIENT_NO_FLAG, persistent)
#else
#	define pdo_mysql_init(persistent) mysql_init(NULL)
#endif

/* {{{ _pdo_mysql_error */
int _pdo_mysql_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, const char *file, int line)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	pdo_error_type *pdo_err;
	pdo_mysql_error_info *einfo;
	pdo_mysql_stmt *S = NULL;

	PDO_DBG_ENTER("_pdo_mysql_error");
	PDO_DBG_INF_FMT("file=%s line=%d", file, line);
	if (stmt) {
		S = (pdo_mysql_stmt*)stmt->driver_data;
		pdo_err = &stmt->error_code;
		einfo   = &S->einfo;
	} else {
		pdo_err = &dbh->error_code;
		einfo   = &H->einfo;
	}

	if (S && S->stmt) {
		einfo->errcode = mysql_stmt_errno(S->stmt);
	} else {
		einfo->errcode = mysql_errno(H->server);
	}

	einfo->file = file;
	einfo->line = line;

	if (einfo->errmsg) {
		pefree(einfo->errmsg, dbh->is_persistent);
		einfo->errmsg = NULL;
	}

	if (einfo->errcode) {
		if (einfo->errcode == 2014) {
			einfo->errmsg = pestrdup(
				"Cannot execute queries while other unbuffered queries are active.  "
				"Consider using PDOStatement::fetchAll().  Alternatively, if your code "
				"is only ever going to run against mysql, you may enable query "
				"buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute.",
				dbh->is_persistent);
		} else if (einfo->errcode == 2057) {
			einfo->errmsg = pestrdup(
				"A stored procedure returning result sets of different size was called. "
				"This is not supported by libmysql",
				dbh->is_persistent);

		} else {
			einfo->errmsg = pestrdup(mysql_error(H->server), dbh->is_persistent);
		}
	} else { /* no error */
		strcpy(*pdo_err, PDO_ERR_NONE);
		PDO_DBG_RETURN(0);
	}

	if (S && S->stmt) {
		strcpy(*pdo_err, mysql_stmt_sqlstate(S->stmt));
	} else {
		strcpy(*pdo_err, mysql_sqlstate(H->server));
	}

	if (!dbh->methods) {
		PDO_DBG_INF("Throwing exception");
		zend_throw_exception_ex(php_pdo_get_exception(), einfo->errcode, "SQLSTATE[%s] [%d] %s",
				*pdo_err, einfo->errcode, einfo->errmsg);
	}

	PDO_DBG_RETURN(einfo->errcode);
}
/* }}} */

/* {{{ pdo_mysql_fetch_error_func */
static int pdo_mysql_fetch_error_func(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	pdo_mysql_error_info *einfo = &H->einfo;

	PDO_DBG_ENTER("pdo_mysql_fetch_error_func");
	PDO_DBG_INF_FMT("dbh=%p stmt=%p", dbh, stmt);
	if (stmt) {
		pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;
		einfo = &S->einfo;
	} else {
		einfo = &H->einfo;
	}

	if (einfo->errcode) {
		add_next_index_long(info, einfo->errcode);
		add_next_index_string(info, einfo->errmsg);
	}

	PDO_DBG_RETURN(1);
}
/* }}} */

/* {{{ mysql_handle_closer */
static int mysql_handle_closer(pdo_dbh_t *dbh)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;

	PDO_DBG_ENTER("mysql_handle_closer");
	PDO_DBG_INF_FMT("dbh=%p", dbh);
	if (H) {
		if (H->server) {
			mysql_close(H->server);
		}
		if (H->einfo.errmsg) {
			pefree(H->einfo.errmsg, dbh->is_persistent);
		}
		pefree(H, dbh->is_persistent);
		dbh->driver_data = NULL;
	}
	PDO_DBG_RETURN(0);
}
/* }}} */

/* {{{ mysql_handle_preparer */
static int mysql_handle_preparer(pdo_dbh_t *dbh, const char *sql, size_t sql_len, pdo_stmt_t *stmt, zval *driver_options)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	pdo_mysql_stmt *S = ecalloc(1, sizeof(pdo_mysql_stmt));
	char *nsql = NULL;
	size_t nsql_len = 0;
	int ret;
	int server_version;

	PDO_DBG_ENTER("mysql_handle_preparer");
	PDO_DBG_INF_FMT("dbh=%p", dbh);
	PDO_DBG_INF_FMT("sql=%.*s", (int)sql_len, sql);

	S->H = H;
	stmt->driver_data = S;
	stmt->methods = &mysql_stmt_methods;

	if (H->emulate_prepare) {
		goto end;
	}

	server_version = mysql_get_server_version(H->server);
	if (server_version < 40100) {
		goto fallback;
	}
	stmt->supports_placeholders = PDO_PLACEHOLDER_POSITIONAL;
	ret = pdo_parse_params(stmt, (char*)sql, sql_len, &nsql, &nsql_len);

	if (ret == 1) {
		/* query was rewritten */
		sql = nsql;
		sql_len = nsql_len;
	} else if (ret == -1) {
		/* failed to parse */
		strcpy(dbh->error_code, stmt->error_code);
		PDO_DBG_RETURN(0);
	}

	if (!(S->stmt = mysql_stmt_init(H->server))) {
		pdo_mysql_error(dbh);
		if (nsql) {
			efree(nsql);
		}
		PDO_DBG_RETURN(0);
	}

	if (mysql_stmt_prepare(S->stmt, sql, sql_len)) {
		/* TODO: might need to pull statement specific info here? */
		/* if the query isn't supported by the protocol, fallback to emulation */
		if (mysql_errno(H->server) == 1295) {
			if (nsql) {
				efree(nsql);
			}
			goto fallback;
		}
		pdo_mysql_error(dbh);
		if (nsql) {
			efree(nsql);
		}
		PDO_DBG_RETURN(0);
	}
	if (nsql) {
		efree(nsql);
	}

	S->num_params = mysql_stmt_param_count(S->stmt);

	if (S->num_params) {
		S->params_given = 0;
#if defined(PDO_USE_MYSQLND)
		S->params = NULL;
#else
		S->params = ecalloc(S->num_params, sizeof(MYSQL_BIND));
		S->in_null = ecalloc(S->num_params, sizeof(my_bool));
		S->in_length = ecalloc(S->num_params, sizeof(zend_ulong));
#endif
	}
	dbh->alloc_own_columns = 1;

	S->max_length = pdo_attr_lval(driver_options, PDO_ATTR_MAX_COLUMN_LEN, 0);

	PDO_DBG_RETURN(1);

fallback:
end:
	stmt->supports_placeholders = PDO_PLACEHOLDER_NONE;

	PDO_DBG_RETURN(1);
}
/* }}} */

/* {{{ mysql_handle_doer */
static zend_long mysql_handle_doer(pdo_dbh_t *dbh, const char *sql, size_t sql_len)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	PDO_DBG_ENTER("mysql_handle_doer");
	PDO_DBG_INF_FMT("dbh=%p", dbh);
	PDO_DBG_INF_FMT("sql=%.*s", (int)sql_len, sql);

	if (mysql_real_query(H->server, sql, sql_len)) {
		pdo_mysql_error(dbh);
		PDO_DBG_RETURN(-1);
	} else {
		my_ulonglong c = mysql_affected_rows(H->server);
		if (c == (my_ulonglong) -1) {
			pdo_mysql_error(dbh);
			PDO_DBG_RETURN(H->einfo.errcode ? -1 : 0);
		} else {

			/* MULTI_QUERY support - eat up all unfetched result sets */
			MYSQL_RES* result;
			while (mysql_more_results(H->server)) {
				if (mysql_next_result(H->server)) {
					PDO_DBG_RETURN(1);
				}
				result = mysql_store_result(H->server);
				if (result) {
					mysql_free_result(result);
				}
			}
			PDO_DBG_RETURN((int)c);
		}
	}
}
/* }}} */

/* {{{ pdo_mysql_last_insert_id */
static char *pdo_mysql_last_insert_id(pdo_dbh_t *dbh, const char *name, size_t *len)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	char *id = php_pdo_int64_to_str(mysql_insert_id(H->server));
	PDO_DBG_ENTER("pdo_mysql_last_insert_id");
	*len = strlen(id);
	PDO_DBG_RETURN(id);
}
/* }}} */

/* {{{ mysql_handle_quoter */
static int mysql_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, size_t unquotedlen, char **quoted, size_t *quotedlen, enum pdo_param_type paramtype )
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	PDO_DBG_ENTER("mysql_handle_quoter");
	PDO_DBG_INF_FMT("dbh=%p", dbh);
	PDO_DBG_INF_FMT("unquoted=%.*s", (int)unquotedlen, unquoted);
	*quoted = safe_emalloc(2, unquotedlen, 3);
	*quotedlen = mysql_real_escape_string(H->server, *quoted + 1, unquoted, unquotedlen);
	(*quoted)[0] =(*quoted)[++*quotedlen] = '\'';
	(*quoted)[++*quotedlen] = '\0';
	PDO_DBG_INF_FMT("quoted=%.*s", (int)*quotedlen, *quoted);
	PDO_DBG_RETURN(1);
}
/* }}} */

/* {{{ mysql_handle_begin */
static int mysql_handle_begin(pdo_dbh_t *dbh)
{
	PDO_DBG_ENTER("mysql_handle_quoter");
	PDO_DBG_INF_FMT("dbh=%p", dbh);
	PDO_DBG_RETURN(0 <= mysql_handle_doer(dbh, ZEND_STRL("START TRANSACTION")));
}
/* }}} */

/* {{{ mysql_handle_commit */
static int mysql_handle_commit(pdo_dbh_t *dbh)
{
	PDO_DBG_ENTER("mysql_handle_commit");
	PDO_DBG_INF_FMT("dbh=%p", dbh);
#if MYSQL_VERSION_ID >= 40100 || defined(PDO_USE_MYSQLND)
	PDO_DBG_RETURN(0 <= mysql_commit(((pdo_mysql_db_handle *)dbh->driver_data)->server));
#else
	PDO_DBG_RETURN(0 <= mysql_handle_doer(dbh, ZEND_STRL("COMMIT")));
#endif
}
/* }}} */

/* {{{ mysql_handle_rollback */
static int mysql_handle_rollback(pdo_dbh_t *dbh)
{
	PDO_DBG_ENTER("mysql_handle_rollback");
	PDO_DBG_INF_FMT("dbh=%p", dbh);
#if MYSQL_VERSION_ID >= 40100 || defined(PDO_USE_MYSQLND)
	PDO_DBG_RETURN(0 <= mysql_rollback(((pdo_mysql_db_handle *)dbh->driver_data)->server));
#else
	PDO_DBG_RETURN(0 <= mysql_handle_doer(dbh, ZEND_STRL("ROLLBACK")));
#endif
}
/* }}} */

/* {{{ mysql_handle_autocommit */
static inline int mysql_handle_autocommit(pdo_dbh_t *dbh)
{
	PDO_DBG_ENTER("mysql_handle_autocommit");
	PDO_DBG_INF_FMT("dbh=%p", dbh);
	PDO_DBG_INF_FMT("dbh->autocommit=%d", dbh->auto_commit);
#if MYSQL_VERSION_ID >= 40100 || defined(PDO_USE_MYSQLND)
	PDO_DBG_RETURN(0 <= mysql_autocommit(((pdo_mysql_db_handle *)dbh->driver_data)->server, dbh->auto_commit));
#else
	if (dbh->auto_commit) {
		PDO_DBG_RETURN(0 <= mysql_handle_doer(dbh, ZEND_STRL("SET AUTOCOMMIT=1")));
	} else {
		PDO_DBG_RETURN(0 <= mysql_handle_doer(dbh, ZEND_STRL("SET AUTOCOMMIT=0")));
	}
#endif
}
/* }}} */

/* {{{ pdo_mysql_set_attribute */
static int pdo_mysql_set_attribute(pdo_dbh_t *dbh, zend_long attr, zval *val)
{
	zend_long lval = zval_get_long(val);
	zend_bool bval = lval? 1 : 0;
	PDO_DBG_ENTER("pdo_mysql_set_attribute");
	PDO_DBG_INF_FMT("dbh=%p", dbh);
	PDO_DBG_INF_FMT("attr=%l", attr);
	switch (attr) {
		case PDO_ATTR_AUTOCOMMIT:
			/* ignore if the new value equals the old one */
			if (dbh->auto_commit ^ bval) {
				dbh->auto_commit = bval;
				mysql_handle_autocommit(dbh);
			}
			PDO_DBG_RETURN(1);

		case PDO_MYSQL_ATTR_USE_BUFFERED_QUERY:
			/* ignore if the new value equals the old one */
			((pdo_mysql_db_handle *)dbh->driver_data)->buffered = bval;
			PDO_DBG_RETURN(1);
		case PDO_MYSQL_ATTR_DIRECT_QUERY:
		case PDO_ATTR_EMULATE_PREPARES:
			/* ignore if the new value equals the old one */
			((pdo_mysql_db_handle *)dbh->driver_data)->emulate_prepare = bval;
			PDO_DBG_RETURN(1);
		case PDO_ATTR_FETCH_TABLE_NAMES:
			((pdo_mysql_db_handle *)dbh->driver_data)->fetch_table_names = bval;
			PDO_DBG_RETURN(1);
#ifndef PDO_USE_MYSQLND
		case PDO_MYSQL_ATTR_MAX_BUFFER_SIZE:
			if (lval < 0) {
				/* TODO: Johannes, can we throw a warning here? */
 				((pdo_mysql_db_handle *)dbh->driver_data)->max_buffer_size = 1024*1024;
				PDO_DBG_INF_FMT("Adjusting invalid buffer size to =%l", ((pdo_mysql_db_handle *)dbh->driver_data)->max_buffer_size);
			} else {
				((pdo_mysql_db_handle *)dbh->driver_data)->max_buffer_size = lval;
			}
			PDO_DBG_RETURN(1);
			break;
#endif

		default:
			PDO_DBG_RETURN(0);
	}
}
/* }}} */

/* {{{ pdo_mysql_get_attribute */
static int pdo_mysql_get_attribute(pdo_dbh_t *dbh, zend_long attr, zval *return_value)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;

	PDO_DBG_ENTER("pdo_mysql_get_attribute");
	PDO_DBG_INF_FMT("dbh=%p", dbh);
	PDO_DBG_INF_FMT("attr=%l", attr);
	switch (attr) {
		case PDO_ATTR_CLIENT_VERSION:
			ZVAL_STRING(return_value, (char *)mysql_get_client_info());
			break;

		case PDO_ATTR_SERVER_VERSION:
			ZVAL_STRING(return_value, (char *)mysql_get_server_info(H->server));
			break;

		case PDO_ATTR_CONNECTION_STATUS:
			ZVAL_STRING(return_value, (char *)mysql_get_host_info(H->server));
			break;
		case PDO_ATTR_SERVER_INFO: {
#if defined(PDO_USE_MYSQLND)
			zend_string *tmp;

			if (mysqlnd_stat(H->server, &tmp) == PASS) {
				ZVAL_STR(return_value, tmp);
#else
			char *tmp;
			if ((tmp = (char *)mysql_stat(H->server))) {
				ZVAL_STRING(return_value, tmp);
#endif
			} else {
				pdo_mysql_error(dbh);
				PDO_DBG_RETURN(-1);
			}
		}
			break;
		case PDO_ATTR_AUTOCOMMIT:
			ZVAL_LONG(return_value, dbh->auto_commit);
			break;

		case PDO_MYSQL_ATTR_USE_BUFFERED_QUERY:
			ZVAL_LONG(return_value, H->buffered);
			break;

		case PDO_ATTR_EMULATE_PREPARES:
		case PDO_MYSQL_ATTR_DIRECT_QUERY:
			ZVAL_LONG(return_value, H->emulate_prepare);
			break;

#ifndef PDO_USE_MYSQLND
		case PDO_MYSQL_ATTR_MAX_BUFFER_SIZE:
			ZVAL_LONG(return_value, H->max_buffer_size);
			break;
#endif

		default:
			PDO_DBG_RETURN(0);
	}

	PDO_DBG_RETURN(1);
}
/* }}} */

/* {{{ pdo_mysql_check_liveness */
static int pdo_mysql_check_liveness(pdo_dbh_t *dbh)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
#if MYSQL_VERSION_ID <= 32230
	void (*handler) (int);
	unsigned int my_errno;
#endif

	PDO_DBG_ENTER("pdo_mysql_check_liveness");
	PDO_DBG_INF_FMT("dbh=%p", dbh);

#if MYSQL_VERSION_ID > 32230
	if (mysql_ping(H->server)) {
		PDO_DBG_RETURN(FAILURE);
	}
#else /* no mysql_ping() */
	handler = signal(SIGPIPE, SIG_IGN);
	mysql_stat(H->server);
	switch (mysql_errno(H->server)) {
		case CR_SERVER_GONE_ERROR:
		case CR_SERVER_LOST:
			signal(SIGPIPE, handler);
			PDO_DBG_RETURN(FAILURE);
		default:
			break;
	}
	signal(SIGPIPE, handler);
#endif /* end mysql_ping() */
	PDO_DBG_RETURN(SUCCESS);
}
/* }}} */

/* {{{ mysql_methods */
static struct pdo_dbh_methods mysql_methods = {
	mysql_handle_closer,
	mysql_handle_preparer,
	mysql_handle_doer,
	mysql_handle_quoter,
	mysql_handle_begin,
	mysql_handle_commit,
	mysql_handle_rollback,
	pdo_mysql_set_attribute,
	pdo_mysql_last_insert_id,
	pdo_mysql_fetch_error_func,
	pdo_mysql_get_attribute,
	pdo_mysql_check_liveness
};
/* }}} */

#ifdef PHP_WIN32
# define PDO_DEFAULT_MYSQL_UNIX_ADDR	NULL
#else
# define PDO_DEFAULT_MYSQL_UNIX_ADDR	PDO_MYSQL_G(default_socket)
#endif

/* {{{ pdo_mysql_handle_factory */
static int pdo_mysql_handle_factory(pdo_dbh_t *dbh, zval *driver_options)
{
	pdo_mysql_db_handle *H;
	int i, ret = 0;
	char *host = NULL, *unix_socket = NULL;
	unsigned int port = 3306;
	char *dbname;
	struct pdo_data_src_parser vars[] = {
		{ "charset",  NULL,	0 },
		{ "dbname",   "",	0 },
		{ "host",   "localhost",	0 },
		{ "port",   "3306",	0 },
		{ "unix_socket",  PDO_DEFAULT_MYSQL_UNIX_ADDR,	0 },
	};
	int connect_opts = 0
#ifdef CLIENT_MULTI_RESULTS
		|CLIENT_MULTI_RESULTS
#endif
		;
#if defined(PDO_USE_MYSQLND)
	size_t dbname_len = 0;
	size_t password_len = 0;
#endif

#ifdef CLIENT_MULTI_STATEMENTS
	if (!driver_options) {
		connect_opts |= CLIENT_MULTI_STATEMENTS;
	} else if (pdo_attr_lval(driver_options, PDO_MYSQL_ATTR_MULTI_STATEMENTS, 1)) {
		connect_opts |= CLIENT_MULTI_STATEMENTS;
	}
#endif

	PDO_DBG_ENTER("pdo_mysql_handle_factory");
	PDO_DBG_INF_FMT("dbh=%p", dbh);
#ifdef CLIENT_MULTI_RESULTS
	PDO_DBG_INF("multi results");
#endif

	php_pdo_parse_data_source(dbh->data_source, dbh->data_source_len, vars, 5);

	H = pecalloc(1, sizeof(pdo_mysql_db_handle), dbh->is_persistent);

	H->einfo.errcode = 0;
	H->einfo.errmsg = NULL;

	/* allocate an environment */

	/* handle for the server */
	if (!(H->server = pdo_mysql_init(dbh->is_persistent))) {
		pdo_mysql_error(dbh);
		goto cleanup;
	}

	dbh->driver_data = H;

#ifndef PDO_USE_MYSQLND
	H->max_buffer_size = 1024*1024;
#endif

	H->buffered = H->emulate_prepare = 1;

	/* handle MySQL options */
	if (driver_options) {
		zend_long connect_timeout = pdo_attr_lval(driver_options, PDO_ATTR_TIMEOUT, 30);
		zend_long local_infile = pdo_attr_lval(driver_options, PDO_MYSQL_ATTR_LOCAL_INFILE, 0);
		zend_string *init_cmd = NULL;
#ifndef PDO_USE_MYSQLND
		zend_string *default_file = NULL, *default_group = NULL;
#endif
		zend_long compress = 0;
		zend_string *ssl_key = NULL, *ssl_cert = NULL, *ssl_ca = NULL, *ssl_capath = NULL, *ssl_cipher = NULL;
		H->buffered = pdo_attr_lval(driver_options, PDO_MYSQL_ATTR_USE_BUFFERED_QUERY, 1);

		H->emulate_prepare = pdo_attr_lval(driver_options,
			PDO_MYSQL_ATTR_DIRECT_QUERY, H->emulate_prepare);
		H->emulate_prepare = pdo_attr_lval(driver_options,
			PDO_ATTR_EMULATE_PREPARES, H->emulate_prepare);

#ifndef PDO_USE_MYSQLND
		H->max_buffer_size = pdo_attr_lval(driver_options, PDO_MYSQL_ATTR_MAX_BUFFER_SIZE, H->max_buffer_size);
#endif

		if (pdo_attr_lval(driver_options, PDO_MYSQL_ATTR_FOUND_ROWS, 0)) {
			connect_opts |= CLIENT_FOUND_ROWS;
		}

		if (pdo_attr_lval(driver_options, PDO_MYSQL_ATTR_IGNORE_SPACE, 0)) {
			connect_opts |= CLIENT_IGNORE_SPACE;
		}

		if (mysql_options(H->server, MYSQL_OPT_CONNECT_TIMEOUT, (const char *)&connect_timeout)) {
			pdo_mysql_error(dbh);
			goto cleanup;
		}

#ifndef PDO_USE_MYSQLND
#if PHP_API_VERSION < 20100412
		if ((PG(open_basedir) && PG(open_basedir)[0] != '\0') || PG(safe_mode))
#else
		if (PG(open_basedir) && PG(open_basedir)[0] != '\0')
#endif
		{
			local_infile = 0;
		}
#endif
#if defined(MYSQL_OPT_LOCAL_INFILE) || defined(PDO_USE_MYSQLND)
		if (mysql_options(H->server, MYSQL_OPT_LOCAL_INFILE, (const char *)&local_infile)) {
			pdo_mysql_error(dbh);
			goto cleanup;
		}
#endif
#ifdef MYSQL_OPT_RECONNECT
		/* since 5.0.3, the default for this option is 0 if not specified.
		 * we want the old behaviour
		 * mysqlnd doesn't support reconnect, thus we don't have "|| defined(PDO_USE_MYSQLND)"
		*/
		{
			zend_long reconnect = 1;
			mysql_options(H->server, MYSQL_OPT_RECONNECT, (const char*)&reconnect);
		}
#endif
		init_cmd = pdo_attr_strval(driver_options, PDO_MYSQL_ATTR_INIT_COMMAND, NULL);
		if (init_cmd) {
			if (mysql_options(H->server, MYSQL_INIT_COMMAND, (const char *)ZSTR_VAL(init_cmd))) {
				zend_string_release(init_cmd);
				pdo_mysql_error(dbh);
				goto cleanup;
			}
			zend_string_release(init_cmd);
		}
#ifndef PDO_USE_MYSQLND
		default_file = pdo_attr_strval(driver_options, PDO_MYSQL_ATTR_READ_DEFAULT_FILE, NULL);
		if (default_file) {
			if (mysql_options(H->server, MYSQL_READ_DEFAULT_FILE, (const char *)ZSTR_VAL(default_file))) {
				zend_string_release(default_file);
				pdo_mysql_error(dbh);
				goto cleanup;
			}
			zend_string_release(default_file);
		}

		default_group = pdo_attr_strval(driver_options, PDO_MYSQL_ATTR_READ_DEFAULT_GROUP, NULL);
		if (default_group) {
			if (mysql_options(H->server, MYSQL_READ_DEFAULT_GROUP, (const char *)ZSTR_VAL(default_group))) {
				zend_string_release(default_group);
				pdo_mysql_error(dbh);
				goto cleanup;
			}
			zend_string_release(default_group);
		}
#endif
		compress = pdo_attr_lval(driver_options, PDO_MYSQL_ATTR_COMPRESS, 0);
		if (compress) {
			if (mysql_options(H->server, MYSQL_OPT_COMPRESS, 0)) {
				pdo_mysql_error(dbh);
				goto cleanup;
			}
		}

		ssl_key = pdo_attr_strval(driver_options, PDO_MYSQL_ATTR_SSL_KEY, NULL);
		ssl_cert = pdo_attr_strval(driver_options, PDO_MYSQL_ATTR_SSL_CERT, NULL);
		ssl_ca = pdo_attr_strval(driver_options, PDO_MYSQL_ATTR_SSL_CA, NULL);
		ssl_capath = pdo_attr_strval(driver_options, PDO_MYSQL_ATTR_SSL_CAPATH, NULL);
		ssl_cipher = pdo_attr_strval(driver_options, PDO_MYSQL_ATTR_SSL_CIPHER, NULL);

		if (ssl_key || ssl_cert || ssl_ca || ssl_capath || ssl_cipher) {
			mysql_ssl_set(H->server,
					ssl_key? ZSTR_VAL(ssl_key) : NULL,
					ssl_cert? ZSTR_VAL(ssl_cert) : NULL,
					ssl_ca? ZSTR_VAL(ssl_ca) : NULL,
					ssl_capath? ZSTR_VAL(ssl_capath) : NULL,
					ssl_cipher? ZSTR_VAL(ssl_cipher) : NULL);
			if (ssl_key) {
				zend_string_release(ssl_key);
			}
			if (ssl_cert) {
				zend_string_release(ssl_cert);
			}
			if (ssl_ca) {
				zend_string_release(ssl_ca);
			}
			if (ssl_capath) {
				zend_string_release(ssl_capath);
			}
			if (ssl_cipher) {
				zend_string_release(ssl_cipher);
			}
		}

#if MYSQL_VERSION_ID > 50605 || defined(PDO_USE_MYSQLND)
		{
			zend_string *public_key = pdo_attr_strval(driver_options, PDO_MYSQL_ATTR_SERVER_PUBLIC_KEY, NULL);
			if (public_key) {
				if (mysql_options(H->server, MYSQL_SERVER_PUBLIC_KEY, ZSTR_VAL(public_key))) {
					pdo_mysql_error(dbh);
					zend_string_release(public_key);
					goto cleanup;
				}
				zend_string_release(public_key);
			}
		}
#endif

#ifdef PDO_USE_MYSQLND
		{
			zend_long ssl_verify_cert = pdo_attr_lval(driver_options,
					PDO_MYSQL_ATTR_SSL_VERIFY_SERVER_CERT, -1);
			if (ssl_verify_cert != -1) {
				connect_opts |= ssl_verify_cert ?
					CLIENT_SSL_VERIFY_SERVER_CERT:
					CLIENT_SSL_DONT_VERIFY_SERVER_CERT;
			}
		}
#endif
	}

#ifdef PDO_MYSQL_HAS_CHARSET
	if (vars[0].optval && mysql_options(H->server, MYSQL_SET_CHARSET_NAME, vars[0].optval)) {
		pdo_mysql_error(dbh);
		goto cleanup;
	}
#endif

	dbname = vars[1].optval;
	host = vars[2].optval;
	if(vars[3].optval) {
		port = atoi(vars[3].optval);
	}

#ifdef PHP_WIN32
	if (vars[2].optval && !strcmp(".", vars[2].optval)) {
#else
	if (vars[2].optval && !strcmp("localhost", vars[2].optval)) {
#endif
		unix_socket = vars[4].optval;
	}

	/* TODO: - Check zval cache + ZTS */
#ifdef PDO_USE_MYSQLND
	if (dbname) {
		dbname_len = strlen(dbname);
	}

	if (dbh->password) {
		password_len = strlen(dbh->password);
	}

	if (mysqlnd_connect(H->server, host, dbh->username, dbh->password, password_len, dbname, dbname_len,
						port, unix_socket, connect_opts, MYSQLND_CLIENT_NO_FLAG) == NULL) {
#else
	if (mysql_real_connect(H->server, host, dbh->username, dbh->password, dbname, port, unix_socket, connect_opts) == NULL) {
#endif
		pdo_mysql_error(dbh);
		goto cleanup;
	}

	if (!dbh->auto_commit) {
		mysql_handle_autocommit(dbh);
	}

	H->attached = 1;

	dbh->alloc_own_columns = 1;
	dbh->max_escaped_char_length = 2;
	dbh->methods = &mysql_methods;

	ret = 1;

cleanup:
	for (i = 0; i < sizeof(vars)/sizeof(vars[0]); i++) {
		if (vars[i].freeme) {
			efree(vars[i].optval);
		}
	}

	dbh->methods = &mysql_methods;

	PDO_DBG_RETURN(ret);
}
/* }}} */

pdo_driver_t pdo_mysql_driver = {
	PDO_DRIVER_HEADER(mysql),
	pdo_mysql_handle_factory
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
