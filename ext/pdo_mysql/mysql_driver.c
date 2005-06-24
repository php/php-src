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
  | Author: George Schlossnagle <george@omniti.com>                      |
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
	case 2014: return "PDDRV"; /* out of sync */
	default: return "HY000";
	}
}

int _pdo_mysql_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, const char *file, int line TSRMLS_DC) /* {{{ */
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	pdo_error_type *pdo_err; 
	pdo_mysql_error_info *einfo;

	if (stmt) {
		pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;
		pdo_err = &stmt->error_code;
		einfo   = &S->einfo;
	} else {
		pdo_err = &dbh->error_code;
		einfo   = &H->einfo;
	}

	einfo->errcode = mysql_errno(H->server);
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
			einfo->errmsg = pestrdup("Cannot execute queries, while other unbuffered queries are active. To enable query buffering set PDO_MYSQL_ATTR_USE_BUFFERED_QUERY attribute.", dbh->is_persistent);
		}
	} else { /* no error */
		strcpy(*pdo_err, PDO_ERR_NONE);
		return 0;
	}

	strcpy(*pdo_err, pdo_mysql_get_sqlstate(einfo->errcode));

	if (!dbh->methods) {
		zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, "SQLSTATE[%s] [%d] %s",
				*pdo_err, einfo->errcode, einfo->errmsg);
	}

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

	S->H = H;
	S->result = NULL;

	stmt->driver_data = S;
	stmt->methods = &mysql_stmt_methods;
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
		return mysql_affected_rows(H->server);
	}
}

static char *pdo_mysql_last_insert_id(pdo_dbh_t *dbh, const char *name, unsigned int *len TSRMLS_DC)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	char *id = NULL;

	*len = spprintf(&id, 0, "%lld", mysql_insert_id(H->server));
	return id;
}

static int mysql_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, int unquotedlen, char **quoted, int *quotedlen, enum pdo_param_type paramtype  TSRMLS_DC)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	*quoted = emalloc(2*unquotedlen + 3);
	(*quoted)[0] = '"';
	*quotedlen = mysql_real_escape_string(H->server, *quoted + 1, unquoted, unquotedlen);
	(*quoted)[*quotedlen + 1] = '"';
	(*quoted)[*quotedlen + 2] = '\0';
	*quotedlen += 2;
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

		default:
			return 0;	
	}

	return 1;
}


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
	NULL /* check_liveness: TODO: ping */
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

	php_pdo_parse_data_source(dbh->data_source, dbh->data_source_len, vars, 4);

	H = pecalloc(1, sizeof(pdo_mysql_db_handle), dbh->is_persistent);
	
	H->einfo.errcode = 0;
	H->einfo.errmsg = NULL;

	/* allocate an environment */
	
	/* handle for the server */
	H->server = mysql_init(NULL);
	dbh->driver_data = H;

	/* handle MySQL options */
	if (driver_options) {
		long connect_timeout = pdo_attr_lval(driver_options, PDO_ATTR_TIMEOUT, 30 TSRMLS_CC);
		H->buffered = pdo_attr_lval(driver_options, PDO_MYSQL_ATTR_USE_BUFFERED_QUERY, 0 TSRMLS_CC);

		if (mysql_options(H->server, MYSQL_OPT_CONNECT_TIMEOUT, (const char *)&connect_timeout)) {
			pdo_mysql_error(dbh);
			goto cleanup;
		}
	}
	
	if (vars[2].optval && !strcmp("localhost", vars[2].optval)) {
		unix_socket = vars[4].optval;  
	} else {
		host = vars[2].optval;
		port = atoi(vars[3].optval); 
	}
	dbname = vars[1].optval;
	if (mysql_real_connect(H->server, host, dbh->username, dbh->password, dbname, port, unix_socket, 0) == NULL) {
		pdo_mysql_error(dbh);
		goto cleanup;
	}

	mysql_handle_autocommit(dbh TSRMLS_CC);

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
