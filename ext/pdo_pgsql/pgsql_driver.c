/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Edin Kadribasic <edink@emini.dk>                            |
  |          Ilia Alshanestsky <ilia@prohost.org>                        |
  |          Wez Furlong <wez@php.net>                                   |
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
#include "ext/standard/file.h"

#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#include "pg_config.h" /* needed for PG_VERSION */
#include "php_pdo_pgsql.h"
#include "php_pdo_pgsql_int.h"
#include "zend_exceptions.h"

static char * _pdo_pgsql_trim_message(const char *message, int persistent)
{
	register int i = strlen(message)-1;
	char *tmp;

	if (i>1 && (message[i-1] == '\r' || message[i-1] == '\n') && message[i] == '.') {
		--i;
	}
	while (i>0 && (message[i] == '\r' || message[i] == '\n')) {
		--i;
	}
	++i;
	tmp = pemalloc(i + 1, persistent);
	memcpy(tmp, message, i);
	tmp[i] = '\0';
	
	return tmp;
}

int _pdo_pgsql_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, int errcode, const char *sqlstate, const char *file, int line TSRMLS_DC) /* {{{ */
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	pdo_error_type *pdo_err = stmt ? &stmt->error_code : &dbh->error_code;
	pdo_pgsql_error_info *einfo = &H->einfo;
	char *errmsg = PQerrorMessage(H->server);

	einfo->errcode = errcode;
	einfo->file = file;
	einfo->line = line;

	if (einfo->errmsg) {
		pefree(einfo->errmsg, dbh->is_persistent);
		einfo->errmsg = NULL;
	}

	if (sqlstate == NULL || strlen(sqlstate) >= sizeof(pdo_error_type)) {
		strcpy(*pdo_err, "HY000");
	}
	else {
		strcpy(*pdo_err, sqlstate);
	}

	if (errmsg) {
		einfo->errmsg = _pdo_pgsql_trim_message(errmsg, dbh->is_persistent);
	}

	if (!dbh->methods) {
		zend_throw_exception_ex(php_pdo_get_exception(), einfo->errcode TSRMLS_CC, "SQLSTATE[%s] [%d] %s",
				*pdo_err, einfo->errcode, einfo->errmsg);
	}
	
	return errcode;
}
/* }}} */

static void _pdo_pgsql_notice(pdo_dbh_t *dbh, const char *message) /* {{{ */
{
/*	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data; */
}
/* }}} */

static int pdo_pgsql_fetch_error_func(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info TSRMLS_DC) /* {{{ */
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	pdo_pgsql_error_info *einfo = &H->einfo;

	if (einfo->errcode) {
		add_next_index_long(info, einfo->errcode);
		add_next_index_string(info, einfo->errmsg, 1);
	}

	return 1;
}
/* }}} */

/* {{{ pdo_pgsql_create_lob_stream */
static size_t pgsql_lob_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	struct pdo_pgsql_lob_self *self = (struct pdo_pgsql_lob_self*)stream->abstract;
	return lo_write(self->conn, self->lfd, (char*)buf, count);
}

static size_t pgsql_lob_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	struct pdo_pgsql_lob_self *self = (struct pdo_pgsql_lob_self*)stream->abstract;
	return lo_read(self->conn, self->lfd, buf, count);
}

static int pgsql_lob_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	struct pdo_pgsql_lob_self *self = (struct pdo_pgsql_lob_self*)stream->abstract;
	pdo_dbh_t *dbh = self->dbh;

	if (close_handle) {
		lo_close(self->conn, self->lfd);
	}
	efree(self);
	php_pdo_dbh_delref(dbh TSRMLS_CC);
	return 0;
}

static int pgsql_lob_flush(php_stream *stream TSRMLS_DC)
{
	return 0;
}

static int pgsql_lob_seek(php_stream *stream, off_t offset, int whence,
		off_t *newoffset TSRMLS_DC)
{
	struct pdo_pgsql_lob_self *self = (struct pdo_pgsql_lob_self*)stream->abstract;
	int pos = lo_lseek(self->conn, self->lfd, offset, whence);
	*newoffset = pos;
	return pos >= 0 ? 0 : -1;
}

php_stream_ops pdo_pgsql_lob_stream_ops = {
	pgsql_lob_write,
	pgsql_lob_read,
	pgsql_lob_close,
	pgsql_lob_flush,
	"pdo_pgsql lob stream",
	pgsql_lob_seek,
	NULL,
	NULL,
	NULL
};

php_stream *pdo_pgsql_create_lob_stream(pdo_dbh_t *dbh, int lfd, Oid oid TSRMLS_DC)
{
	php_stream *stm;
	struct pdo_pgsql_lob_self *self = ecalloc(1, sizeof(*self));
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;

	self->dbh = dbh;
	self->lfd = lfd;
	self->oid = oid;
	self->conn = H->server;

	stm = php_stream_alloc(&pdo_pgsql_lob_stream_ops, self, 0, "r+b");

	if (stm) {
		php_pdo_dbh_addref(dbh TSRMLS_CC);
		return stm;
	}

	efree(self);
	return NULL;
}
/* }}} */

static int pgsql_handle_closer(pdo_dbh_t *dbh TSRMLS_DC) /* {{{ */
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	if (H) {
		if (H->server) {
			PQfinish(H->server);
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

static int pgsql_handle_preparer(pdo_dbh_t *dbh, const char *sql, long sql_len, pdo_stmt_t *stmt, zval *driver_options TSRMLS_DC)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	pdo_pgsql_stmt *S = ecalloc(1, sizeof(pdo_pgsql_stmt));
	int scrollable;
#if HAVE_PQPREPARE
	int ret;
	char *nsql = NULL;
	int nsql_len = 0;
	int emulate = 0;
#endif

	S->H = H;
	stmt->driver_data = S;
	stmt->methods = &pgsql_stmt_methods;

	scrollable = pdo_attr_lval(driver_options, PDO_ATTR_CURSOR,
		PDO_CURSOR_FWDONLY TSRMLS_CC) == PDO_CURSOR_SCROLL;

	if (scrollable) {
		if (S->cursor_name) {
			efree(S->cursor_name);
		}
		spprintf(&S->cursor_name, 0, "pdo_crsr_%08x", ++H->stmt_counter);
#if HAVE_PQPREPARE
		emulate = 1;
#endif
	}

#if HAVE_PQPREPARE
	else if (driver_options) {
		if (pdo_attr_lval(driver_options, PDO_PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT, H->disable_native_prepares TSRMLS_CC) == 1 ||
			pdo_attr_lval(driver_options, PDO_ATTR_EMULATE_PREPARES, H->emulate_prepares TSRMLS_CC) == 1) {
			emulate = 1;
		}
	} else {
		emulate = H->disable_native_prepares || H->emulate_prepares;
	}

	if (!emulate && PQprotocolVersion(H->server) > 2) {
		stmt->supports_placeholders = PDO_PLACEHOLDER_NAMED;
		stmt->named_rewrite_template = "$%d";
		ret = pdo_parse_params(stmt, (char*)sql, sql_len, &nsql, &nsql_len TSRMLS_CC);

		if (ret == 1) {
			/* query was re-written */
			sql = nsql;
		} else if (ret == -1) {
			/* couldn't grok it */
			strcpy(dbh->error_code, stmt->error_code);
			return 0;
		}

		spprintf(&S->stmt_name, 0, "pdo_stmt_%08x", ++H->stmt_counter);
		/* that's all for now; we'll defer the actual prepare until the first execute call */
	
		if (nsql) {
			S->query = nsql;
		} else {
			S->query = estrdup(sql);
		}

		return 1;
	}
#endif

	stmt->supports_placeholders = PDO_PLACEHOLDER_NONE;
	return 1;
}

static long pgsql_handle_doer(pdo_dbh_t *dbh, const char *sql, long sql_len TSRMLS_DC)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	PGresult *res;
	long ret = 1;
	ExecStatusType qs;
	
	if (!(res = PQexec(H->server, sql))) {
		/* fatal error */
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, NULL);
		return -1;
	}
	qs = PQresultStatus(res);
	if (qs != PGRES_COMMAND_OK && qs != PGRES_TUPLES_OK) {
		pdo_pgsql_error(dbh, qs, pdo_pgsql_sqlstate(res));
		PQclear(res);
		return -1;
	}
	H->pgoid = PQoidValue(res);
	ret = (qs == PGRES_COMMAND_OK) ? atol(PQcmdTuples(res)) : 0L;
	PQclear(res);

	return ret;
}

static int pgsql_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, int unquotedlen, char **quoted, int *quotedlen, enum pdo_param_type paramtype TSRMLS_DC)
{
	unsigned char *escaped;
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	size_t tmp_len;
	
	switch (paramtype) {
		case PDO_PARAM_LOB:
			/* escapedlen returned by PQescapeBytea() accounts for trailing 0 */
#ifdef HAVE_PQESCAPE_BYTEA_CONN
			escaped = PQescapeByteaConn(H->server, unquoted, unquotedlen, &tmp_len);
#else
			escaped = PQescapeBytea(unquoted, unquotedlen, &tmp_len);
#endif
			*quotedlen = (int)tmp_len + 1;
			*quoted = emalloc(*quotedlen + 1);
			memcpy((*quoted)+1, escaped, *quotedlen-2);
			(*quoted)[0] = '\'';
			(*quoted)[*quotedlen-1] = '\'';
			(*quoted)[*quotedlen] = '\0';
			PQfreemem(escaped);
			break;
		default:
			*quoted = safe_emalloc(2, unquotedlen, 3);
			(*quoted)[0] = '\'';
#ifndef HAVE_PQESCAPE_CONN
			*quotedlen = PQescapeString(*quoted + 1, unquoted, unquotedlen);
#else
			*quotedlen = PQescapeStringConn(H->server, *quoted + 1, unquoted, unquotedlen, NULL);
#endif
			(*quoted)[*quotedlen + 1] = '\'';
			(*quoted)[*quotedlen + 2] = '\0';
			*quotedlen += 2;
	}
	return 1;
}

static char *pdo_pgsql_last_insert_id(pdo_dbh_t *dbh, const char *name, unsigned int *len TSRMLS_DC)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	char *id = NULL;

	if (name == NULL) {
		if (H->pgoid == InvalidOid) {
			return NULL;
		}
		*len = spprintf(&id, 0, "%ld", (long) H->pgoid);
	} else {
		PGresult *res;
		ExecStatusType status;
		const char *q[1];
		q[0] = name;
		res = PQexecParams(H->server, "SELECT CURRVAL($1)", 1, NULL, q, NULL, NULL, 0);
		status = PQresultStatus(res);

		if (res && (status == PGRES_TUPLES_OK)) {
			id = estrdup((char *)PQgetvalue(res, 0, 0));
			*len = PQgetlength(res, 0, 0);
		} else {
			pdo_pgsql_error(dbh, status, pdo_pgsql_sqlstate(res));
		}

		if (res) {
			PQclear(res);
		}
	}
	return id;
}

static int pdo_pgsql_get_attribute(pdo_dbh_t *dbh, long attr, zval *return_value TSRMLS_DC)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;

	switch (attr) {
		case PDO_ATTR_CLIENT_VERSION:
			ZVAL_STRING(return_value, PG_VERSION, 1);
			break;

		case PDO_ATTR_SERVER_VERSION:
			if (PQprotocolVersion(H->server) >= 3) { /* PostgreSQL 7.4 or later */
				ZVAL_STRING(return_value, (char*)PQparameterStatus(H->server, "server_version"), 1);
			} else /* emulate above via a query */
			{
				PGresult *res = PQexec(H->server, "SELECT VERSION()");
				if (res && PQresultStatus(res) == PGRES_TUPLES_OK) {
					ZVAL_STRING(return_value, (char *)PQgetvalue(res, 0, 0), 1);
				}

				if (res) {
					PQclear(res);
				}
			}
			break;

		case PDO_ATTR_CONNECTION_STATUS:
			switch (PQstatus(H->server)) {
				case CONNECTION_STARTED:
					ZVAL_STRINGL(return_value, "Waiting for connection to be made.", sizeof("Waiting for connection to be made.")-1, 1);
					break;

				case CONNECTION_MADE:
				case CONNECTION_OK:
					ZVAL_STRINGL(return_value, "Connection OK; waiting to send.", sizeof("Connection OK; waiting to send.")-1, 1);
					break;

				case CONNECTION_AWAITING_RESPONSE:
					ZVAL_STRINGL(return_value, "Waiting for a response from the server.", sizeof("Waiting for a response from the server.")-1, 1);
					break;

				case CONNECTION_AUTH_OK:
					ZVAL_STRINGL(return_value, "Received authentication; waiting for backend start-up to finish.", sizeof("Received authentication; waiting for backend start-up to finish.")-1, 1);
					break;
#ifdef CONNECTION_SSL_STARTUP
				case CONNECTION_SSL_STARTUP:
					ZVAL_STRINGL(return_value, "Negotiating SSL encryption.", sizeof("Negotiating SSL encryption.")-1, 1);
					break;
#endif
				case CONNECTION_SETENV:
					ZVAL_STRINGL(return_value, "Negotiating environment-driven parameter settings.", sizeof("Negotiating environment-driven parameter settings.")-1, 1);
					break;

				case CONNECTION_BAD:
				default:
					ZVAL_STRINGL(return_value, "Bad connection.", sizeof("Bad connection.")-1, 1);
					break;
			}
			break;

		case PDO_ATTR_SERVER_INFO: {
			int spid = PQbackendPID(H->server);
			char *tmp;
			spprintf(&tmp, 0, 
				"PID: %d; Client Encoding: %s; Is Superuser: %s; Session Authorization: %s; Date Style: %s", 
				spid,
				(char*)PQparameterStatus(H->server, "client_encoding"),
				(char*)PQparameterStatus(H->server, "is_superuser"),
				(char*)PQparameterStatus(H->server, "session_authorization"),
				(char*)PQparameterStatus(H->server, "DateStyle"));
			ZVAL_STRING(return_value, tmp, 0);
		}
			break;

		default:
			return 0;	
	}

	return 1;
}

/* {{{ */
static int pdo_pgsql_check_liveness(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	if (PQstatus(H->server) == CONNECTION_BAD) {
		PQreset(H->server);
	}
	return (PQstatus(H->server) == CONNECTION_OK) ? SUCCESS : FAILURE;
}
/* }}} */

static int pdo_pgsql_transaction_cmd(const char *cmd, pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	PGresult *res;
	int ret = 1;

	res = PQexec(H->server, cmd);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		pdo_pgsql_error(dbh, PQresultStatus(res), pdo_pgsql_sqlstate(res));
		ret = 0;
	}

	PQclear(res);
	return ret;
}

static int pgsql_handle_begin(pdo_dbh_t *dbh TSRMLS_DC)
{
	return pdo_pgsql_transaction_cmd("BEGIN", dbh TSRMLS_CC);
}

static int pgsql_handle_commit(pdo_dbh_t *dbh TSRMLS_DC)
{
	return pdo_pgsql_transaction_cmd("COMMIT", dbh TSRMLS_CC);
}

static int pgsql_handle_rollback(pdo_dbh_t *dbh TSRMLS_DC)
{
	return pdo_pgsql_transaction_cmd("ROLLBACK", dbh TSRMLS_CC);
}

static int pgsql_handle_in_transaction(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_pgsql_db_handle *H;

	H = (pdo_pgsql_db_handle *)dbh->driver_data;

	return PQtransactionStatus(H->server);
}

/* {{{ proto string PDO::pgsqlCopyFromArray(string $table_name , array $rows [, string $delimiter [, string $null_as ] [, string $fields])
   Returns true if the copy worked fine or false if error */
static PHP_METHOD(PDO, pgsqlCopyFromArray)
{
	pdo_dbh_t *dbh;
	pdo_pgsql_db_handle *H;

	zval *pg_rows;

	char *table_name, *pg_delim = NULL, *pg_null_as = NULL, *pg_fields = NULL;
	int table_name_len, pg_delim_len = 0, pg_null_as_len = 0, pg_fields_len;
	char *query;

	PGresult *pgsql_result;
	ExecStatusType status;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s/a|sss",
					&table_name, &table_name_len, &pg_rows,
					&pg_delim, &pg_delim_len, &pg_null_as, &pg_null_as_len, &pg_fields, &pg_fields_len) == FAILURE) {
		return;
	}

	if (!zend_hash_num_elements(Z_ARRVAL_P(pg_rows))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot copy from an empty array");
		RETURN_FALSE;
	}

	dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	PDO_CONSTRUCT_CHECK;

	if (pg_fields) {
		spprintf(&query, 0, "COPY %s (%s) FROM STDIN DELIMITERS E'%c' WITH NULL AS E'%s'", table_name, pg_fields, (pg_delim_len ? *pg_delim : '\t'), (pg_null_as_len ? pg_null_as : "\\\\N"));
	} else {
		spprintf(&query, 0, "COPY %s FROM STDIN DELIMITERS E'%c' WITH NULL AS E'%s'", table_name, (pg_delim_len ? *pg_delim : '\t'), (pg_null_as_len ? pg_null_as : "\\\\N"));
	}

	/* Obtain db Handle */
	H = (pdo_pgsql_db_handle *)dbh->driver_data;

	while ((pgsql_result = PQgetResult(H->server))) {
		PQclear(pgsql_result);
	}
	pgsql_result = PQexec(H->server, query);

	efree(query);
	query = NULL;

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(H->server);
	}

	if (status == PGRES_COPY_IN && pgsql_result) {
		int command_failed = 0;
		int buffer_len = 0;
		zval **tmp;
		HashPosition pos;

		PQclear(pgsql_result);
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(pg_rows), &pos);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_P(pg_rows), (void **) &tmp, &pos) == SUCCESS) {
			int query_len;
			convert_to_string_ex(tmp);
		
			if (buffer_len < Z_STRLEN_PP(tmp)) {
				buffer_len = Z_STRLEN_PP(tmp);
				query = erealloc(query, buffer_len + 2); /* room for \n\0 */
			}
			memcpy(query, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
			query_len = Z_STRLEN_PP(tmp);
			if (query[query_len - 1] != '\n') {
				query[query_len++] = '\n';
			}
			query[query_len] = '\0';
			if (PQputCopyData(H->server, query, query_len) != 1) {
                        	efree(query);
	                        pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "copy failed");
        	                RETURN_FALSE;
                	}
			zend_hash_move_forward_ex(Z_ARRVAL_P(pg_rows), &pos);
                }
		if (query) {
			efree(query);
		}

		if (PQputCopyEnd(H->server, NULL) != 1) {
			pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "putcopyend failed");
			RETURN_FALSE;
		}

		while ((pgsql_result = PQgetResult(H->server))) {
			if (PGRES_COMMAND_OK != PQresultStatus(pgsql_result)) {
				pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "Copy command failed");
				command_failed = 1;
			}
			PQclear(pgsql_result);
		}

		RETURN_BOOL(!command_failed);
	} else {
		PQclear(pgsql_result);
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "Copy command failed");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string PDO::pgsqlCopyFromFile(string $table_name , string $filename [, string $delimiter [, string $null_as ] [, string $fields])
   Returns true if the copy worked fine or false if error */
static PHP_METHOD(PDO, pgsqlCopyFromFile)
{
	pdo_dbh_t *dbh;
	pdo_pgsql_db_handle *H;

	char *table_name, *filename, *pg_delim = NULL, *pg_null_as = NULL, *pg_fields = NULL;
	int  table_name_len, filename_len, pg_delim_len = 0, pg_null_as_len = 0, pg_fields_len;
	char *query;
	PGresult *pgsql_result;
	ExecStatusType status;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sp|sss",
				&table_name, &table_name_len, &filename, &filename_len,
				&pg_delim, &pg_delim_len, &pg_null_as, &pg_null_as_len, &pg_fields, &pg_fields_len) == FAILURE) {
		return;
	}

	/* Obtain db Handler */
	dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	PDO_CONSTRUCT_CHECK;

	stream = php_stream_open_wrapper_ex(filename, "rb", ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL, FG(default_context));
	if (!stream) {
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "Unable to open the file");
		RETURN_FALSE;
	}

	if (pg_fields) {
		spprintf(&query, 0, "COPY %s (%s) FROM STDIN DELIMITERS E'%c' WITH NULL AS E'%s'", table_name, pg_fields, (pg_delim_len ? *pg_delim : '\t'), (pg_null_as_len ? pg_null_as : "\\\\N"));
	} else {
		spprintf(&query, 0, "COPY %s FROM STDIN DELIMITERS E'%c' WITH NULL AS E'%s'", table_name, (pg_delim_len ? *pg_delim : '\t'), (pg_null_as_len ? pg_null_as : "\\\\N"));
	}

	H = (pdo_pgsql_db_handle *)dbh->driver_data;

	while ((pgsql_result = PQgetResult(H->server))) {
		PQclear(pgsql_result);
	}
	pgsql_result = PQexec(H->server, query);

	efree(query);

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(H->server);
	}

	if (status == PGRES_COPY_IN && pgsql_result) {
		char *buf;
		int command_failed = 0;
		size_t line_len = 0;

		PQclear(pgsql_result);
		while ((buf = php_stream_get_line(stream, NULL, 0, &line_len)) != NULL) {
			if (PQputCopyData(H->server, buf, line_len) != 1) {
	                        efree(buf);
        	                pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "copy failed");
				php_stream_close(stream);
				RETURN_FALSE;
			}
			efree(buf);
		}
		php_stream_close(stream);

		if (PQputCopyEnd(H->server, NULL) != 1) {
			pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "putcopyend failed");
			RETURN_FALSE;
		}

		while ((pgsql_result = PQgetResult(H->server))) {
			if (PGRES_COMMAND_OK != PQresultStatus(pgsql_result)) {
				pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "Copy command failed");
				command_failed = 1;
			}
			PQclear(pgsql_result);
		}

		RETURN_BOOL(!command_failed);
	} else {
		PQclear(pgsql_result);
		php_stream_close(stream);
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "Copy command failed");
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto string PDO::pgsqlCopyToFile(string $table_name , $filename, [string $delimiter [, string $null_as [, string $fields]]])
   Returns true if the copy worked fine or false if error */
static PHP_METHOD(PDO, pgsqlCopyToFile)
{
	pdo_dbh_t *dbh;
	pdo_pgsql_db_handle *H;

	char *table_name, *pg_delim = NULL, *pg_null_as = NULL, *pg_fields = NULL, *filename = NULL;
	int table_name_len, pg_delim_len = 0, pg_null_as_len = 0, pg_fields_len, filename_len;
	char *query;

	PGresult *pgsql_result;
	ExecStatusType status;

	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sp|sss",
					&table_name, &table_name_len, &filename, &filename_len,
					&pg_delim, &pg_delim_len, &pg_null_as, &pg_null_as_len, &pg_fields, &pg_fields_len) == FAILURE) {
		return;
	}

	dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	PDO_CONSTRUCT_CHECK;

	H = (pdo_pgsql_db_handle *)dbh->driver_data;

	stream = php_stream_open_wrapper_ex(filename, "wb", ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL, FG(default_context));
	if (!stream) {
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "Unable to open the file for writing");
		RETURN_FALSE;
	}

	while ((pgsql_result = PQgetResult(H->server))) {
		PQclear(pgsql_result);
	}

	if (pg_fields) {
		spprintf(&query, 0, "COPY %s (%s) TO STDIN DELIMITERS E'%c' WITH NULL AS E'%s'", table_name, pg_fields, (pg_delim_len ? *pg_delim : '\t'), (pg_null_as_len ? pg_null_as : "\\\\N"));
	} else {
		spprintf(&query, 0, "COPY %s TO STDIN DELIMITERS E'%c' WITH NULL AS E'%s'", table_name, (pg_delim_len ? *pg_delim : '\t'), (pg_null_as_len ? pg_null_as : "\\\\N"));
	}
	pgsql_result = PQexec(H->server, query);
	efree(query);

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(H->server);
	}

	if (status == PGRES_COPY_OUT && pgsql_result) {
		PQclear(pgsql_result);
		while (1) {
			char *csv = NULL;
			int ret = PQgetCopyData(H->server, &csv, 0);

			if (ret == -1) {
				break; /* done */
			} else if (ret > 0) {
				if (php_stream_write(stream, csv, ret) != ret) {
					pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "Unable to write to file");
					PQfreemem(csv);
					php_stream_close(stream);
					RETURN_FALSE;
				} else {
					PQfreemem(csv);
				}
			} else {
				pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "Copy command failed: getline failed");
				php_stream_close(stream);
				RETURN_FALSE;
			}
		}
		php_stream_close(stream);

		while ((pgsql_result = PQgetResult(H->server))) {
			PQclear(pgsql_result);
		}
		RETURN_TRUE;
	} else {
		php_stream_close(stream);
		PQclear(pgsql_result);
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "Copy command failed");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string PDO::pgsqlCopyToArray(string $table_name , [string $delimiter [, string $null_as [, string $fields]]])
   Returns true if the copy worked fine or false if error */
static PHP_METHOD(PDO, pgsqlCopyToArray)
{
	pdo_dbh_t *dbh;
	pdo_pgsql_db_handle *H;

	char *table_name, *pg_delim = NULL, *pg_null_as = NULL, *pg_fields = NULL;
	int table_name_len, pg_delim_len = 0, pg_null_as_len = 0, pg_fields_len;
	char *query;

	PGresult *pgsql_result;
	ExecStatusType status;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sss",
		&table_name, &table_name_len,
		&pg_delim, &pg_delim_len, &pg_null_as, &pg_null_as_len, &pg_fields, &pg_fields_len) == FAILURE) {
		return;
	}

	dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	PDO_CONSTRUCT_CHECK;

	H = (pdo_pgsql_db_handle *)dbh->driver_data;

	while ((pgsql_result = PQgetResult(H->server))) {
		PQclear(pgsql_result);
	}

	if (pg_fields) {
		spprintf(&query, 0, "COPY %s (%s) TO STDIN DELIMITERS E'%c' WITH NULL AS E'%s'", table_name, pg_fields, (pg_delim_len ? *pg_delim : '\t'), (pg_null_as_len ? pg_null_as : "\\\\N"));
	} else {
		spprintf(&query, 0, "COPY %s TO STDIN DELIMITERS E'%c' WITH NULL AS E'%s'", table_name, (pg_delim_len ? *pg_delim : '\t'), (pg_null_as_len ? pg_null_as : "\\\\N"));
	}
	pgsql_result = PQexec(H->server, query);
	efree(query);

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(H->server);
	}

	if (status == PGRES_COPY_OUT && pgsql_result) {
		PQclear(pgsql_result);
                array_init(return_value);

		while (1) {
			char *csv = NULL;
			int ret = PQgetCopyData(H->server, &csv, 0);
			if (ret == -1) {
				break; /* copy done */
			} else if (ret > 0) { 
				add_next_index_stringl(return_value, csv, ret, 1);
				PQfreemem(csv);
			} else {
				pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "Copy command failed: getline failed");
				RETURN_FALSE;
			}
		}

		while ((pgsql_result = PQgetResult(H->server))) {
			PQclear(pgsql_result);
		}
	} else {
		PQclear(pgsql_result);
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "Copy command failed");
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto string PDO::pgsqlLOBCreate()
   Creates a new large object, returning its identifier.  Must be called inside a transaction. */
static PHP_METHOD(PDO, pgsqlLOBCreate)
{
	pdo_dbh_t *dbh;
	pdo_pgsql_db_handle *H;
	Oid lfd;

	dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	PDO_CONSTRUCT_CHECK;

	H = (pdo_pgsql_db_handle *)dbh->driver_data;
	lfd = lo_creat(H->server, INV_READ|INV_WRITE);

	if (lfd != InvalidOid) {
		char *buf;
		spprintf(&buf, 0, "%lu", (long) lfd);
		RETURN_STRING(buf, 0);
	}
	
	pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "HY000");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto resource PDO::pgsqlLOBOpen(string oid [, string mode = 'rb'])
   Opens an existing large object stream.  Must be called inside a transaction. */
static PHP_METHOD(PDO, pgsqlLOBOpen)
{
	pdo_dbh_t *dbh;
	pdo_pgsql_db_handle *H;
	Oid oid;
	int lfd;
	char *oidstr;
	int oidstrlen;
	char *modestr = "rb";
	int modestrlen;
	int mode = INV_READ;
	char *end_ptr;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s",
				&oidstr, &oidstrlen, &modestr, &modestrlen)) {
		RETURN_FALSE;
	}

	oid = (Oid)strtoul(oidstr, &end_ptr, 10);
	if (oid == 0 && (errno == ERANGE || errno == EINVAL)) {
		RETURN_FALSE;
	}

	if (strpbrk(modestr, "+w")) {
		mode = INV_READ|INV_WRITE;
	}
	
	dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	PDO_CONSTRUCT_CHECK;

	H = (pdo_pgsql_db_handle *)dbh->driver_data;

	lfd = lo_open(H->server, oid, mode);

	if (lfd >= 0) {
		php_stream *stream = pdo_pgsql_create_lob_stream(dbh, lfd, oid TSRMLS_CC);
		if (stream) {
			php_stream_to_zval(stream, return_value);
			return;
		}
	} else {
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "HY000");
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool PDO::pgsqlLOBUnlink(string oid)
   Deletes the large object identified by oid.  Must be called inside a transaction. */
static PHP_METHOD(PDO, pgsqlLOBUnlink)
{
	pdo_dbh_t *dbh;
	pdo_pgsql_db_handle *H;
	Oid oid;
	char *oidstr, *end_ptr;
	int oidlen;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
				&oidstr, &oidlen)) {
		RETURN_FALSE;
	}

	oid = (Oid)strtoul(oidstr, &end_ptr, 10);
	if (oid == 0 && (errno == ERANGE || errno == EINVAL)) {
		RETURN_FALSE;
	}

	dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	PDO_CONSTRUCT_CHECK;

	H = (pdo_pgsql_db_handle *)dbh->driver_data;
	
	if (1 == lo_unlink(H->server, oid)) {
		RETURN_TRUE;
	}
	pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, "HY000");
	RETURN_FALSE;
}
/* }}} */


static const zend_function_entry dbh_methods[] = {
	PHP_ME(PDO, pgsqlLOBCreate, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(PDO, pgsqlLOBOpen, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(PDO, pgsqlLOBUnlink, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(PDO, pgsqlCopyFromArray, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(PDO, pgsqlCopyFromFile, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(PDO, pgsqlCopyToArray, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(PDO, pgsqlCopyToFile, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

static const zend_function_entry *pdo_pgsql_get_driver_methods(pdo_dbh_t *dbh, int kind TSRMLS_DC)
{
	switch (kind) {
		case PDO_DBH_DRIVER_METHOD_KIND_DBH:
			return dbh_methods;
		default:
			return NULL;
	}
}

static int pdo_pgsql_set_attr(pdo_dbh_t *dbh, long attr, zval *val TSRMLS_DC)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;

	switch (attr) {
#if HAVE_PQPREPARE
		case PDO_ATTR_EMULATE_PREPARES:
			H->emulate_prepares = Z_LVAL_P(val);
			return 1;
		case PDO_PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT:
			H->disable_native_prepares = Z_LVAL_P(val);
			return 1;
#endif

		default:
			return 0;
	}
}

static struct pdo_dbh_methods pgsql_methods = {
	pgsql_handle_closer,
	pgsql_handle_preparer,
	pgsql_handle_doer,
	pgsql_handle_quoter,
	pgsql_handle_begin,
	pgsql_handle_commit,
	pgsql_handle_rollback,
	pdo_pgsql_set_attr,
	pdo_pgsql_last_insert_id,
	pdo_pgsql_fetch_error_func,
	pdo_pgsql_get_attribute,
	pdo_pgsql_check_liveness,	/* check_liveness */
	pdo_pgsql_get_driver_methods,  /* get_driver_methods */
	NULL,
	pgsql_handle_in_transaction,
};

static int pdo_pgsql_handle_factory(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC) /* {{{ */
{
	pdo_pgsql_db_handle *H;
	int ret = 0;
	char *conn_str, *p, *e;
	long connect_timeout = 30;

	H = pecalloc(1, sizeof(pdo_pgsql_db_handle), dbh->is_persistent);
	dbh->driver_data = H;

	H->einfo.errcode = 0;
	H->einfo.errmsg = NULL;
	
	/* PostgreSQL wants params in the connect string to be separated by spaces,
	 * if the PDO standard semicolons are used, we convert them to spaces
	 */
	e = (char *) dbh->data_source + strlen(dbh->data_source);
	p = (char *) dbh->data_source;
	while ((p = memchr(p, ';', (e - p)))) {
		*p = ' ';
	}

	if (driver_options) {
		connect_timeout = pdo_attr_lval(driver_options, PDO_ATTR_TIMEOUT, 30 TSRMLS_CC);
	}

	/* support both full connection string & connection string + login and/or password */
	if (dbh->username && dbh->password) {
		spprintf(&conn_str, 0, "%s user=%s password=%s connect_timeout=%ld", dbh->data_source, dbh->username, dbh->password, connect_timeout);
	} else if (dbh->username) {
		spprintf(&conn_str, 0, "%s user=%s connect_timeout=%ld", dbh->data_source, dbh->username, connect_timeout);
	} else if (dbh->password) {
		spprintf(&conn_str, 0, "%s password=%s connect_timeout=%ld", dbh->data_source, dbh->password, connect_timeout);
	} else {
		spprintf(&conn_str, 0, "%s connect_timeout=%ld", (char *) dbh->data_source, connect_timeout);
	}

	H->server = PQconnectdb(conn_str);

	efree(conn_str);

	if (PQstatus(H->server) != CONNECTION_OK) {
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, PHP_PDO_PGSQL_CONNECTION_FAILURE_SQLSTATE);
		goto cleanup;
	}

	PQsetNoticeProcessor(H->server, (void(*)(void*,const char*))_pdo_pgsql_notice, (void *)&dbh);

	H->attached = 1;
	H->pgoid = -1;

	dbh->methods = &pgsql_methods;
	dbh->alloc_own_columns = 1;
	dbh->max_escaped_char_length = 2;

	ret = 1;
	
cleanup:
	dbh->methods = &pgsql_methods;
	if (!ret) {
		pgsql_handle_closer(dbh TSRMLS_CC);
	}

	return ret;
}
/* }}} */

pdo_driver_t pdo_pgsql_driver = {
	PDO_DRIVER_HEADER(pgsql),
	pdo_pgsql_handle_factory
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
