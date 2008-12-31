/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2009 The PHP Group                                |
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
#include <mysqld_error.h>
#include "zend_exceptions.h"


const char *pdo_mysql_get_sqlstate(unsigned int my_errno) {
	switch (my_errno) {
		/* import auto-generated case: code */
#include "php_pdo_mysql_sqlstate.h"
	default: return "HY000";
	}
}

int _pdo_mysql_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, const char *file, int line TSRMLS_DC) /* {{{ */
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	pdo_error_type *pdo_err; 
	pdo_mysql_error_info *einfo;
	pdo_mysql_stmt *S = NULL;

	if (stmt) {
		S = (pdo_mysql_stmt*)stmt->driver_data;
		pdo_err = &stmt->error_code;
		einfo   = &S->einfo;
	} else {
		pdo_err = &dbh->error_code;
		einfo   = &H->einfo;
	}

#if HAVE_MYSQL_STMT_PREPARE
	if (S && S->stmt) {
		einfo->errcode = mysql_stmt_errno(S->stmt);
	}
	else
#endif
	{
		einfo->errcode = mysql_errno(H->server);
	}

	einfo->file = file;
	einfo->line = line;

	if (einfo->errmsg) {
		pefree(einfo->errmsg, dbh->is_persistent);
		einfo->errmsg = NULL;
	}

	if (einfo->errcode) {
		if (2014 != einfo->errcode) {
			einfo->errmsg = pestrdup(mysql_error(H->server), dbh->is_persistent);
		} else {
			einfo->errmsg = pestrdup(
				"Cannot execute queries while other unbuffered queries are active.  "
				"Consider using PDOStatement::fetchAll().  Alternatively, if your code "
				"is only ever going to run against mysql, you may enable query "
				"buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute.",
				dbh->is_persistent);
		}
	} else { /* no error */
		strcpy(*pdo_err, PDO_ERR_NONE);
		return 0;
	}

#if HAVE_MYSQL_SQLSTATE
# if HAVE_MYSQL_STMT_PREPARE
	if (S && S->stmt) {
		strcpy(*pdo_err, mysql_stmt_sqlstate(S->stmt));
	} else
# endif
	{
		strcpy(*pdo_err, mysql_sqlstate(H->server));
	}
#else
	strcpy(*pdo_err, pdo_mysql_get_sqlstate(einfo->errcode));
#endif

	if (!dbh->methods) {
		zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, "SQLSTATE[%s] [%d] %s",
				*pdo_err, einfo->errcode, einfo->errmsg);
	}
/* printf("** [%s:%d] %s %s\n", file, line, *pdo_err, einfo->errmsg); */

	return einfo->errcode;
}
/* }}} */

static int pdo_mysql_fetch_error_func(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info TSRMLS_DC)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	pdo_mysql_error_info *einfo = &H->einfo;

	if (stmt) {
		pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;
		einfo = &S->einfo;
	} else {
		einfo = &H->einfo;
	}

	if (einfo->errcode) {
		add_next_index_long(info, einfo->errcode);
		add_next_index_string(info, einfo->errmsg, 1);
	}

	return 1;
}

static int mysql_handle_closer(pdo_dbh_t *dbh TSRMLS_DC) /* {{{ */
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	
	if (H) {
		if (H->server) {
			mysql_close(H->server);
			H->server = NULL;
		}
		if (H->einfo.errmsg) {
			pefree(H->einfo.errmsg, dbh->is_persistent);
			H->einfo.errmsg = NULL;
		}
		pefree(H, dbh->is_persistent);
		dbh->driver_data = NULL;
	}
	return 0;
}
/* }}} */

static int mysql_handle_preparer(pdo_dbh_t *dbh, const char *sql, long sql_len, pdo_stmt_t *stmt, zval *driver_options TSRMLS_DC)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	pdo_mysql_stmt *S = ecalloc(1, sizeof(pdo_mysql_stmt));
#if HAVE_MYSQL_STMT_PREPARE
	char *nsql = NULL;
	int nsql_len = 0;
	int ret;
	int server_version;
#endif
	
	S->H = H;
	stmt->driver_data = S;
	stmt->methods = &mysql_stmt_methods;

	if (H->emulate_prepare) {
		goto end;
	}

#if HAVE_MYSQL_STMT_PREPARE
	server_version = mysql_get_server_version(H->server);
	if (server_version < 40100) {
		goto fallback;
	}
	stmt->supports_placeholders = PDO_PLACEHOLDER_POSITIONAL;
	ret = pdo_parse_params(stmt, (char*)sql, sql_len, &nsql, &nsql_len TSRMLS_CC);

	if (ret == 1) {
		/* query was rewritten */
		sql = nsql;
		sql_len = nsql_len;
	} else if (ret == -1) {
		/* failed to parse */
		strcpy(dbh->error_code, stmt->error_code);
		return 0;
	}

	if (!(S->stmt = mysql_stmt_init(H->server))) {
		pdo_mysql_error(dbh);
		if (nsql) {
			efree(nsql);
		}
		return 0;
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
		return 0;
	}
	if (nsql) {
		efree(nsql);
	}

	S->num_params = mysql_stmt_param_count(S->stmt);

	if (S->num_params) {
		S->params = ecalloc(S->num_params, sizeof(MYSQL_BIND));
		S->in_null = ecalloc(S->num_params, sizeof(my_bool));
		S->in_length = ecalloc(S->num_params, sizeof(unsigned long));
	}

	dbh->alloc_own_columns = 1;

	S->max_length = pdo_attr_lval(driver_options, PDO_ATTR_MAX_COLUMN_LEN, 0 TSRMLS_CC);

	return 1;

fallback:
#endif
end:
	stmt->supports_placeholders = PDO_PLACEHOLDER_NONE;
	
	return 1;
}

static long mysql_handle_doer(pdo_dbh_t *dbh, const char *sql, long sql_len TSRMLS_DC)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;

	if (mysql_real_query(H->server, sql, sql_len)) {
		pdo_mysql_error(dbh);
		return -1;
	} else {
		my_ulonglong c = mysql_affected_rows(H->server);
		if (c == (my_ulonglong) -1) {
			pdo_mysql_error(dbh);
			return (H->einfo.errcode ? -1 : 0);
		} else {
			return c;
		}
	}
}

static char *pdo_mysql_last_insert_id(pdo_dbh_t *dbh, const char *name, unsigned int *len TSRMLS_DC)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	char *id = php_pdo_int64_to_str(mysql_insert_id(H->server) TSRMLS_CC);
	*len = strlen(id);
	return id;
}

static int mysql_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, int unquotedlen, char **quoted, int *quotedlen, enum pdo_param_type paramtype  TSRMLS_DC)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	*quoted = safe_emalloc(2, unquotedlen, 3);
	*quotedlen = mysql_real_escape_string(H->server, *quoted + 1, unquoted, unquotedlen);
	(*quoted)[0] =(*quoted)[++*quotedlen] = '\'';
	(*quoted)[++*quotedlen] = '\0';
	return 1;
}

static int mysql_handle_begin(pdo_dbh_t *dbh TSRMLS_DC)
{
	return 0 <= mysql_handle_doer(dbh, ZEND_STRL("START TRANSACTION") TSRMLS_CC);
}

static int mysql_handle_commit(pdo_dbh_t *dbh TSRMLS_DC)
{
	return 0 <= mysql_handle_doer(dbh, ZEND_STRL("COMMIT") TSRMLS_CC);
}

static int mysql_handle_rollback(pdo_dbh_t *dbh TSRMLS_DC)
{
	return 0 <= mysql_handle_doer(dbh, ZEND_STRL("ROLLBACK") TSRMLS_CC);
}

static int mysql_handle_autocommit(pdo_dbh_t *dbh TSRMLS_DC)
{
	if (dbh->auto_commit) {
		return 0 <= mysql_handle_doer(dbh, ZEND_STRL("SET AUTOCOMMIT=1") TSRMLS_CC);
	} else {
		return 0 <= mysql_handle_doer(dbh, ZEND_STRL("SET AUTOCOMMIT=0") TSRMLS_CC);
	}
}

static int pdo_mysql_set_attribute(pdo_dbh_t *dbh, long attr, zval *val TSRMLS_DC)
{
	switch (attr) {
	case PDO_ATTR_AUTOCOMMIT:
		
		convert_to_boolean(val);
	
		/* ignore if the new value equals the old one */			
		if (dbh->auto_commit ^ Z_BVAL_P(val)) {
			dbh->auto_commit = Z_BVAL_P(val);
			mysql_handle_autocommit(dbh TSRMLS_CC);
		}
		return 1;

	case PDO_MYSQL_ATTR_USE_BUFFERED_QUERY:
		((pdo_mysql_db_handle *)dbh->driver_data)->buffered = Z_BVAL_P(val);
		return 1;
	case PDO_MYSQL_ATTR_DIRECT_QUERY:
	case PDO_ATTR_EMULATE_PREPARES:
		((pdo_mysql_db_handle *)dbh->driver_data)->emulate_prepare = Z_BVAL_P(val);
		return 1;
	default:
		return 0;
	}
}

static int pdo_mysql_get_attribute(pdo_dbh_t *dbh, long attr, zval *return_value TSRMLS_DC)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;

	switch (attr) {
		case PDO_ATTR_CLIENT_VERSION:
			ZVAL_STRING(return_value, (char *)mysql_get_client_info(), 1);
			break;

		case PDO_ATTR_SERVER_VERSION:
			ZVAL_STRING(return_value, (char *)mysql_get_server_info(H->server), 1);
			break;

		case PDO_ATTR_CONNECTION_STATUS:
			ZVAL_STRING(return_value, (char *)mysql_get_host_info(H->server), 1);
			break;

		case PDO_ATTR_SERVER_INFO: {
			char *tmp;

			if ((tmp = (char *)mysql_stat(H->server))) {
				ZVAL_STRING(return_value, tmp, 1);
			} else {
				pdo_mysql_error(dbh);
				return -1;
			}
		}
			break;

		case PDO_ATTR_AUTOCOMMIT:
			ZVAL_LONG(return_value, dbh->auto_commit);
			return 1;
			
		case PDO_MYSQL_ATTR_USE_BUFFERED_QUERY:
			ZVAL_LONG(return_value, H->buffered);
			return 1;

		case PDO_MYSQL_ATTR_DIRECT_QUERY:
			ZVAL_LONG(return_value, H->emulate_prepare);
			return 1;

		case PDO_MYSQL_ATTR_MAX_BUFFER_SIZE:
			ZVAL_LONG(return_value, H->max_buffer_size);
			return 1;

		default:
			return 0;	
	}

	return 1;
}

static int pdo_mysql_check_liveness(pdo_dbh_t *dbh TSRMLS_DC) /* {{{ */
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
#if MYSQL_VERSION_ID <= 32230
	void (*handler) (int);
	unsigned int my_errno;
#endif

#if MYSQL_VERSION_ID > 32230
	if (mysql_ping(H->server)) {
		return FAILURE;
	}
#else /* no mysql_ping() */
	handler=signal(SIGPIPE, SIG_IGN);
	mysql_stat(H->server);
	switch (mysql_errno(H->server)) {
		case CR_SERVER_GONE_ERROR:
		/* case CR_SERVER_LOST: I'm not sure this means the same as "gone" for us */
			signal(SIGPIPE, handler);
			return FAILURE;
		default:
			break;
	}
	signal(SIGPIPE, handler);
#endif /* end mysql_ping() */
	return SUCCESS;
} 
/* }}} */

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

#ifndef PDO_MYSQL_UNIX_ADDR
# ifdef PHP_WIN32
#  define PDO_MYSQL_UNIX_ADDR	"MySQL"
# else
#  define PDO_MYSQL_UNIX_ADDR	"/tmp/mysql.sock"
# endif
#endif

static int pdo_mysql_handle_factory(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC) /* {{{ */
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
		{ "unix_socket",  PDO_MYSQL_UNIX_ADDR,	0 },
	};
	int connect_opts = 0
#ifdef CLIENT_MULTI_RESULTS
		|CLIENT_MULTI_RESULTS
#endif
#ifdef CLIENT_MULTI_STATEMENTS
		|CLIENT_MULTI_STATEMENTS
#endif
		;

	php_pdo_parse_data_source(dbh->data_source, dbh->data_source_len, vars, 5);

	H = pecalloc(1, sizeof(pdo_mysql_db_handle), dbh->is_persistent);
	
	H->einfo.errcode = 0;
	H->einfo.errmsg = NULL;

	/* allocate an environment */

	/* handle for the server */
	if (!(H->server = mysql_init(NULL))) {
		pdo_mysql_error(dbh);
		goto cleanup;
	}
	
	dbh->driver_data = H;
	H->max_buffer_size = 1024*1024;
	H->buffered = H->emulate_prepare = 1;

	/* handle MySQL options */
	if (driver_options) {
		long connect_timeout = pdo_attr_lval(driver_options, PDO_ATTR_TIMEOUT, 30 TSRMLS_CC);
		long local_infile = pdo_attr_lval(driver_options, PDO_MYSQL_ATTR_LOCAL_INFILE, 0 TSRMLS_CC);
		char *init_cmd = NULL, *default_file = NULL, *default_group = NULL;

		H->buffered = pdo_attr_lval(driver_options, PDO_MYSQL_ATTR_USE_BUFFERED_QUERY, 1 TSRMLS_CC);

		H->emulate_prepare = pdo_attr_lval(driver_options,
			PDO_MYSQL_ATTR_DIRECT_QUERY, H->emulate_prepare TSRMLS_CC);
		H->emulate_prepare = pdo_attr_lval(driver_options, 
			PDO_ATTR_EMULATE_PREPARES, H->emulate_prepare TSRMLS_CC);

		H->max_buffer_size = pdo_attr_lval(driver_options, PDO_MYSQL_ATTR_MAX_BUFFER_SIZE, H->max_buffer_size TSRMLS_CC);

		if (mysql_options(H->server, MYSQL_OPT_CONNECT_TIMEOUT, (const char *)&connect_timeout)) {
			pdo_mysql_error(dbh);
			goto cleanup;
		}

		if ((PG(open_basedir) && PG(open_basedir)[0] != '\0') || PG(safe_mode)) {
			local_infile = 0;
		}
#ifdef MYSQL_OPT_LOCAL_INFILE
		if (mysql_options(H->server, MYSQL_OPT_LOCAL_INFILE, (const char *)&local_infile)) {
			pdo_mysql_error(dbh);
			goto cleanup;
		}
#endif
#ifdef MYSQL_OPT_RECONNECT
		/* since 5.0.3, the default for this option is 0 if not specified.
		 * we want the old behaviour */
		{
			long reconnect = 1;
			mysql_options(H->server, MYSQL_OPT_RECONNECT, (const char*)&reconnect);
		}
#endif

		init_cmd = pdo_attr_strval(driver_options, PDO_MYSQL_ATTR_INIT_COMMAND, NULL TSRMLS_CC);
		if (init_cmd) {
			if (mysql_options(H->server, MYSQL_INIT_COMMAND, (const char *)init_cmd)) {
				efree(init_cmd);
				pdo_mysql_error(dbh);
				goto cleanup;
			}
			efree(init_cmd);
		}
		
		default_file = pdo_attr_strval(driver_options, PDO_MYSQL_ATTR_READ_DEFAULT_FILE, NULL TSRMLS_CC);
		if (default_file) {
			if (mysql_options(H->server, MYSQL_READ_DEFAULT_FILE, (const char *)default_file)) {
				efree(default_file);
				pdo_mysql_error(dbh);
				goto cleanup;
			}
			efree(default_file);
		}
		
		default_group= pdo_attr_strval(driver_options, PDO_MYSQL_ATTR_READ_DEFAULT_GROUP, NULL TSRMLS_CC);
		if (default_group) {
			if (mysql_options(H->server, MYSQL_READ_DEFAULT_GROUP, (const char *)default_group)) {
				efree(default_group);
				pdo_mysql_error(dbh);
				goto cleanup;
			}
			efree(default_group);
		}
	}

	dbname = vars[1].optval;
	host = vars[2].optval;	
	if(vars[3].optval) {
		port = atoi(vars[3].optval);
	}
	if (vars[2].optval && !strcmp("localhost", vars[2].optval)) {
		unix_socket = vars[4].optval;  
	}
	if (mysql_real_connect(H->server, host, dbh->username, dbh->password, dbname, port, unix_socket, connect_opts) == NULL) {
		pdo_mysql_error(dbh);
		goto cleanup;
	}

	if (!dbh->auto_commit) {
		mysql_handle_autocommit(dbh TSRMLS_CC);
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

	return ret;
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
