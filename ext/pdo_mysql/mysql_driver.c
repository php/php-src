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

int _pdo_mysql_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, const char *file, int line TSRMLS_DC) /* {{{ */
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	enum pdo_error_type *pdo_err = stmt ? &stmt->error_code : &dbh->error_code;
	pdo_mysql_error_info *einfo = &H->einfo;

	einfo->errcode = mysql_errno(H->server);
	einfo->file = file;
	einfo->line = line;

	if (einfo->errmsg) {
		efree(einfo->errmsg);
		einfo->errmsg = NULL;
	}

	if (einfo->errcode) {
		einfo->errmsg = estrdup(mysql_error(H->server));
	} else { /* no error */
		*pdo_err = PDO_ERR_NONE;
		return 0;
	}

	switch (einfo->errcode) {
		case 1007: /* database already exists */
		case 1050: /* table already exists */
		case 1086: /* file already exists */
		case 1125: /* function already exists */
			*pdo_err = PDO_ERR_ALREADY_EXISTS;
			break;

		case 1008: /* database does not exist */
		case 1029: /* view does not exist */
		case 1072: /* key column does not exist */
		case 1091: /* column/key does not exist */
		case 1146: /* table does not exist */
		case 1176: /* key not found in table */
			*pdo_err = PDO_ERR_NOT_FOUND;
			break;	

		case 1152: /* aborted connection */
		case 1154: /* cannot read from connection pipe */
		case 1184: /* aborted new connection */
		case 1159: /* timeout */
		case 1160: /* timeout */
		case 1161: /* timeout */
			*pdo_err = PDO_ERR_DISCONNECTED;
			break;

		case 1089: /* unsupported sub-key */
		case 1163: /* blob/text not supported inside table */
		case 1164: /* no auto-incremenet support */
		case 1174: /* no RAID support */
		case 1178: /* table handler does not support something */
		case 1185: /* binary dump not supported */
		case 1214: /* FULLTEXT not supported */
		case 1235: /* something not supported by MySQL version */
			*pdo_err = PDO_ERR_NOT_IMPLEMENTED;
			break;

		case 1252: /* character set mismatch */
			*pdo_err = PDO_ERR_MISMATCH;
			break;

		case 1264: /* data truncated */
			*pdo_err = PDO_ERR_TRUNCATED;
			break;
	
		case 1169: /* unique constraint */
		case 1216: /* foreign key constraint */
		case 1217: /* foreign key constraint */
			*pdo_err = PDO_ERR_CONSTRAINT;
			break;

		case 1064: /* query parse error */
		case 1065: /* empty query */
		/* XXX: MySQL has all sorts of errors that can be considered syntax errors, specifically
			dealing with table creation & modifications, do we want to include them here?
		 */
			*pdo_err = PDO_ERR_SYNTAX;
			break;

		default:
			*pdo_err = PDO_ERR_CANT_MAP;
			break;
	}

	if (!dbh->methods) {
		zend_throw_exception_ex(php_pdo_get_exception(), *pdo_err TSRMLS_CC, "[%d] %s",
				einfo->errcode, einfo->errmsg);
	}
	
	return einfo->errcode;
}
/* }}} */

static int pdo_mysql_fetch_error_func(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info TSRMLS_DC)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	pdo_mysql_error_info *einfo = &H->einfo;

	if (einfo->errcode) {
		add_next_index_long(info, einfo->errcode);
		add_next_index_string(info, einfo->errmsg, 1);
	}

	return 1;
}

static int mysql_handle_closer(pdo_dbh_t *dbh TSRMLS_DC) /* {{{ */
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;

	if (H->server) {
		mysql_close(H->server);
		H->server = NULL;
	}
	if (H->einfo.errmsg) {
		efree(H->einfo.errmsg);
		H->einfo.errmsg = NULL;
	}
	return 0;
}
/* }}} */

static int mysql_handle_preparer(pdo_dbh_t *dbh, const char *sql, long sql_len, pdo_stmt_t *stmt, long options, zval *driver_options TSRMLS_DC)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	pdo_mysql_stmt *S = ecalloc(1, sizeof(pdo_mysql_stmt));

	S->H = H;
	stmt->driver_data = S;
	stmt->methods = &mysql_stmt_methods;
	
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

static long pdo_mysql_last_insert_id(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;

	return (long) mysql_insert_id(H->server);
}

static int mysql_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, int unquotedlen, char **quoted, int *quotedlen  TSRMLS_DC)
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
	NULL,
	NULL,
	NULL,
	NULL,
	pdo_mysql_last_insert_id,
	pdo_mysql_fetch_error_func,
	pdo_mysql_get_attribute
};

static int pdo_mysql_handle_factory(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC) /* {{{ */
{
	pdo_mysql_db_handle *H;
	int i, ret = 0;
	char *host = NULL, *unix_socket = NULL;
	unsigned int port;
	char *dbname;
	struct pdo_data_src_parser vars[] = {
		{ "charset",  NULL,	0 },
		{ "dbname",   "",	0 },
		{ "host",   "localhost",	0 },
		{ "port",   "3306",	0 },
#ifdef PDO_MYSQL_UNIX_ADDR
		{ "unix_socket",  PDO_MYSQL_UNIX_ADDR,	0 },
#endif
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

		if (mysql_options(H->server, MYSQL_OPT_CONNECT_TIMEOUT, (const char *)&connect_timeout)) {
			pdo_mysql_error(dbh);
			goto cleanup;
		}
	}

	if (vars[2].optval && strcmp("localhost", vars[2].optval)) {
		host = vars[2].optval;
		port = atoi(vars[3].optval); 
	} else {
		host = NULL;
		unix_socket = vars[4].optval;  
	}
	dbname = vars[1].optval;
	if (mysql_real_connect(H->server, host, dbh->username, dbh->password, dbname, port, unix_socket, 0) == NULL) {
		pdo_mysql_error(dbh);
		goto cleanup;
	}

	H->attached = 1;

	dbh->methods = &mysql_methods;
	dbh->alloc_own_columns = 1;
	dbh->supports_placeholders = 0;
	dbh->max_escaped_char_length = 2;

	ret = 1;
	
cleanup:
	for (i = 0; i < sizeof(vars)/sizeof(vars[0]); i++) {
		if (vars[i].freeme) {
			efree(vars[i].optval);
		}
	}

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
