/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Edin Kadribasic <edink@emini.dk>                            |
  |          Ilia Alshanestsky <ilia@prohost.org>                        |
  |          Wez Furlong <wez@php.net>                                   |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "main/php_network.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "pdo/php_pdo_error.h"
#include "ext/standard/file.h"
#include "php_pdo_pgsql.h"
#include "php_pdo_pgsql_int.h"
#include "zend_exceptions.h"
#include "pgsql_driver_arginfo.h"

static bool pgsql_handle_in_transaction(pdo_dbh_t *dbh);

static char * _pdo_pgsql_trim_message(const char *message, int persistent)
{
	size_t i = strlen(message)-1;
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

static zend_string* _pdo_pgsql_escape_credentials(char *str)
{
	if (str) {
		return php_addcslashes_str(str, strlen(str), "\\'", sizeof("\\'"));
	}

	return NULL;
}

int _pdo_pgsql_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, int errcode, const char *sqlstate, const char *msg, const char *file, int line) /* {{{ */
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

	if (msg) {
		einfo->errmsg = pestrdup(msg, dbh->is_persistent);
	}
	else if (errmsg) {
		einfo->errmsg = _pdo_pgsql_trim_message(errmsg, dbh->is_persistent);
	}

	if (!dbh->methods) {
		pdo_throw_exception(einfo->errcode, einfo->errmsg, pdo_err);
	}

	return errcode;
}
/* }}} */

static void _pdo_pgsql_notice(pdo_dbh_t *dbh, const char *message) /* {{{ */
{
/*	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data; */
}
/* }}} */

static void pdo_pgsql_fetch_error_func(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info) /* {{{ */
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	pdo_pgsql_error_info *einfo = &H->einfo;

	if (einfo->errcode) {
		add_next_index_long(info, einfo->errcode);
	} else {
		/* Add null to respect expected info array structure */
		add_next_index_null(info);
	}
	if (einfo->errmsg) {
		add_next_index_string(info, einfo->errmsg);
	}
}
/* }}} */

/* {{{ pdo_pgsql_create_lob_stream */
static ssize_t pgsql_lob_write(php_stream *stream, const char *buf, size_t count)
{
	struct pdo_pgsql_lob_self *self = (struct pdo_pgsql_lob_self*)stream->abstract;
	return lo_write(self->conn, self->lfd, (char*)buf, count);
}

static ssize_t pgsql_lob_read(php_stream *stream, char *buf, size_t count)
{
	struct pdo_pgsql_lob_self *self = (struct pdo_pgsql_lob_self*)stream->abstract;
	return lo_read(self->conn, self->lfd, buf, count);
}

static int pgsql_lob_close(php_stream *stream, int close_handle)
{
	struct pdo_pgsql_lob_self *self = (struct pdo_pgsql_lob_self*)stream->abstract;
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)(Z_PDO_DBH_P(&self->dbh))->driver_data;

	if (close_handle) {
		lo_close(self->conn, self->lfd);
	}
	zend_hash_index_del(H->lob_streams, php_stream_get_resource_id(stream));
	zval_ptr_dtor(&self->dbh);
	efree(self);
	return 0;
}

static int pgsql_lob_flush(php_stream *stream)
{
	return 0;
}

static int pgsql_lob_seek(php_stream *stream, zend_off_t offset, int whence,
		zend_off_t *newoffset)
{
	struct pdo_pgsql_lob_self *self = (struct pdo_pgsql_lob_self*)stream->abstract;
#if defined(HAVE_PG_LO64) && defined(ZEND_ENABLE_ZVAL_LONG64)
	zend_off_t pos = lo_lseek64(self->conn, self->lfd, offset, whence);
#else
	zend_off_t pos = lo_lseek(self->conn, self->lfd, offset, whence);
#endif
	*newoffset = pos;
	return pos >= 0 ? 0 : -1;
}

const php_stream_ops pdo_pgsql_lob_stream_ops = {
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

php_stream *pdo_pgsql_create_lob_stream(zval *dbh, int lfd, Oid oid)
{
	php_stream *stm;
	struct pdo_pgsql_lob_self *self = ecalloc(1, sizeof(*self));
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)(Z_PDO_DBH_P(dbh))->driver_data;

	ZVAL_COPY_VALUE(&self->dbh, dbh);
	self->lfd = lfd;
	self->oid = oid;
	self->conn = H->server;

	stm = php_stream_alloc(&pdo_pgsql_lob_stream_ops, self, 0, "r+b");

	if (stm) {
		Z_ADDREF_P(dbh);
		zend_hash_index_add_ptr(H->lob_streams, php_stream_get_resource_id(stm), stm->res);
		return stm;
	}

	efree(self);
	return NULL;
}
/* }}} */

void pdo_pgsql_close_lob_streams(pdo_dbh_t *dbh)
{
	zend_resource *res;
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	if (H->lob_streams) {
		ZEND_HASH_REVERSE_FOREACH_PTR(H->lob_streams, res) {
			if (res->type >= 0) {
				zend_list_close(res);
			}
		} ZEND_HASH_FOREACH_END();
	}
}

static void pgsql_handle_closer(pdo_dbh_t *dbh) /* {{{ */
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	if (H) {
		if (H->lob_streams) {
			pdo_pgsql_close_lob_streams(dbh);
			zend_hash_destroy(H->lob_streams);
			pefree(H->lob_streams, dbh->is_persistent);
			H->lob_streams = NULL;
		}
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
}
/* }}} */

static bool pgsql_handle_preparer(pdo_dbh_t *dbh, zend_string *sql, pdo_stmt_t *stmt, zval *driver_options)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	pdo_pgsql_stmt *S = ecalloc(1, sizeof(pdo_pgsql_stmt));
	int scrollable;
	int ret;
	zend_string *nsql = NULL;
	int emulate = 0;
	int execute_only = 0;

	S->H = H;
	stmt->driver_data = S;
	stmt->methods = &pgsql_stmt_methods;

	scrollable = pdo_attr_lval(driver_options, PDO_ATTR_CURSOR,
		PDO_CURSOR_FWDONLY) == PDO_CURSOR_SCROLL;

	if (scrollable) {
		if (S->cursor_name) {
			efree(S->cursor_name);
		}
		spprintf(&S->cursor_name, 0, "pdo_crsr_%08x", ++H->stmt_counter);
		emulate = 1;
	} else if (driver_options) {
		if (pdo_attr_lval(driver_options, PDO_ATTR_EMULATE_PREPARES, H->emulate_prepares) == 1) {
			emulate = 1;
		}
		if (pdo_attr_lval(driver_options, PDO_PGSQL_ATTR_DISABLE_PREPARES, H->disable_prepares) == 1) {
			execute_only = 1;
		}
	} else {
		emulate = H->disable_native_prepares || H->emulate_prepares;
		execute_only = H->disable_prepares;
	}

	if (emulate) {
		stmt->supports_placeholders = PDO_PLACEHOLDER_NONE;
	} else {
		stmt->supports_placeholders = PDO_PLACEHOLDER_NAMED;
		stmt->named_rewrite_template = "$%d";
	}

	ret = pdo_parse_params(stmt, sql, &nsql);

	if (ret == -1) {
		/* couldn't grok it */
		strcpy(dbh->error_code, stmt->error_code);
		return false;
	} else if (ret == 1) {
		/* query was re-written */
		S->query = nsql;
	} else {
		S->query = zend_string_copy(sql);
	}

	if (!emulate && !execute_only) {
		/* prepared query: set the query name and defer the
		   actual prepare until the first execute call */
		spprintf(&S->stmt_name, 0, "pdo_stmt_%08x", ++H->stmt_counter);
	}

	return true;
}

static zend_long pgsql_handle_doer(pdo_dbh_t *dbh, const zend_string *sql)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	PGresult *res;
	zend_long ret = 1;
	ExecStatusType qs;

	bool in_trans = pgsql_handle_in_transaction(dbh);

	if (!(res = PQexec(H->server, ZSTR_VAL(sql)))) {
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
	if (qs == PGRES_COMMAND_OK) {
		ret = ZEND_ATOL(PQcmdTuples(res));
	} else {
		ret = Z_L(0);
	}
	PQclear(res);
	if (in_trans && !pgsql_handle_in_transaction(dbh)) {
		pdo_pgsql_close_lob_streams(dbh);
	}

	return ret;
}

static zend_string* pgsql_handle_quoter(pdo_dbh_t *dbh, const zend_string *unquoted, enum pdo_param_type paramtype)
{
	unsigned char *escaped;
	char *quoted;
	size_t quotedlen;
	zend_string *quoted_str;
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	size_t tmp_len;

	switch (paramtype) {
		case PDO_PARAM_LOB:
			/* escapedlen returned by PQescapeBytea() accounts for trailing 0 */
			escaped = PQescapeByteaConn(H->server, (unsigned char *)ZSTR_VAL(unquoted), ZSTR_LEN(unquoted), &tmp_len);
			quotedlen = tmp_len + 1;
			quoted = emalloc(quotedlen + 1);
			memcpy(quoted+1, escaped, quotedlen-2);
			quoted[0] = '\'';
			quoted[quotedlen-1] = '\'';
			quoted[quotedlen] = '\0';
			PQfreemem(escaped);
			break;
		default:
			quoted = safe_emalloc(2, ZSTR_LEN(unquoted), 3);
			quoted[0] = '\'';
			quotedlen = PQescapeStringConn(H->server, quoted + 1, ZSTR_VAL(unquoted), ZSTR_LEN(unquoted), NULL);
			quoted[quotedlen + 1] = '\'';
			quoted[quotedlen + 2] = '\0';
			quotedlen += 2;
	}

	quoted_str = zend_string_init(quoted, quotedlen, 0);
	efree(quoted);
	return quoted_str;
}

static zend_string *pdo_pgsql_last_insert_id(pdo_dbh_t *dbh, const zend_string *name)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	zend_string *id = NULL;
	PGresult *res;
	ExecStatusType status;

	if (name == NULL) {
		res = PQexec(H->server, "SELECT LASTVAL()");
	} else {
		const char *q[1];
		q[0] = ZSTR_VAL(name);

		res = PQexecParams(H->server, "SELECT CURRVAL($1)", 1, NULL, q, NULL, NULL, 0);
	}
	status = PQresultStatus(res);

	if (res && (status == PGRES_TUPLES_OK)) {
		id = zend_string_init((char *)PQgetvalue(res, 0, 0), PQgetlength(res, 0, 0), 0);
	} else {
		pdo_pgsql_error(dbh, status, pdo_pgsql_sqlstate(res));
	}

	if (res) {
		PQclear(res);
	}

	return id;
}

void pdo_libpq_version(char *buf, size_t len)
{
	int version = PQlibVersion();
	int major = version / 10000;
	if (major >= 10) {
		int minor = version % 10000;
		snprintf(buf, len, "%d.%d", major, minor);
	} else {
		int minor = version / 100 % 100;
		int revision = version % 100;
		snprintf(buf, len, "%d.%d.%d", major, minor, revision);
	}
}

static int pdo_pgsql_get_attribute(pdo_dbh_t *dbh, zend_long attr, zval *return_value)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;

	switch (attr) {
		case PDO_ATTR_EMULATE_PREPARES:
			ZVAL_BOOL(return_value, H->emulate_prepares);
			break;

		case PDO_PGSQL_ATTR_DISABLE_PREPARES:
			ZVAL_BOOL(return_value, H->disable_prepares);
			break;

		case PDO_ATTR_CLIENT_VERSION: {
			char buf[16];
			pdo_libpq_version(buf, sizeof(buf));
			ZVAL_STRING(return_value, buf);
			break;
		}

		case PDO_ATTR_SERVER_VERSION:
			ZVAL_STRING(return_value, (char*)PQparameterStatus(H->server, "server_version"));
			break;

		case PDO_ATTR_CONNECTION_STATUS:
			switch (PQstatus(H->server)) {
				case CONNECTION_STARTED:
					ZVAL_STRINGL(return_value, "Waiting for connection to be made.", strlen("Waiting for connection to be made."));
					break;

				case CONNECTION_MADE:
				case CONNECTION_OK:
					ZVAL_STRINGL(return_value, "Connection OK; waiting to send.", strlen("Connection OK; waiting to send."));
					break;

				case CONNECTION_AWAITING_RESPONSE:
					ZVAL_STRINGL(return_value, "Waiting for a response from the server.", strlen("Waiting for a response from the server."));
					break;

				case CONNECTION_AUTH_OK:
					ZVAL_STRINGL(return_value, "Received authentication; waiting for backend start-up to finish.", strlen("Received authentication; waiting for backend start-up to finish."));
					break;
#ifdef CONNECTION_SSL_STARTUP
				case CONNECTION_SSL_STARTUP:
					ZVAL_STRINGL(return_value, "Negotiating SSL encryption.", strlen("Negotiating SSL encryption."));
					break;
#endif
				case CONNECTION_SETENV:
					ZVAL_STRINGL(return_value, "Negotiating environment-driven parameter settings.", strlen("Negotiating environment-driven parameter settings."));
					break;

#ifdef CONNECTION_CONSUME
				case CONNECTION_CONSUME:
					ZVAL_STRINGL(return_value, "Flushing send queue/consuming extra data.", strlen("Flushing send queue/consuming extra data."));
					break;
#endif
#ifdef CONNECTION_GSS_STARTUP
				case CONNECTION_SSL_STARTUP:
					ZVAL_STRINGL(return_value, "Negotiating GSSAPI.", strlen("Negotiating GSSAPI."));
					break;
#endif
#ifdef CONNECTION_CHECK_TARGET
				case CONNECTION_CHECK_TARGET:
					ZVAL_STRINGL(return_value, "Connection OK; checking target server properties.", strlen("Connection OK; checking target server properties."));
					break;
#endif
#ifdef CONNECTION_CHECK_STANDBY
				case CONNECTION_CHECK_STANDBY:
					ZVAL_STRINGL(return_value, "Connection OK; checking if server in standby.", strlen("Connection OK; checking if server in standby."));
					break;
#endif
				case CONNECTION_BAD:
				default:
					ZVAL_STRINGL(return_value, "Bad connection.", strlen("Bad connection."));
					break;
			}
			break;

		case PDO_ATTR_SERVER_INFO: {
			int spid = PQbackendPID(H->server);


			zend_string *str_info =
				strpprintf(0,
					"PID: %d; Client Encoding: %s; Is Superuser: %s; Session Authorization: %s; Date Style: %s",
					spid,
					(char*)PQparameterStatus(H->server, "client_encoding"),
					(char*)PQparameterStatus(H->server, "is_superuser"),
					(char*)PQparameterStatus(H->server, "session_authorization"),
					(char*)PQparameterStatus(H->server, "DateStyle"));

			ZVAL_STR(return_value, str_info);
			break;
		}

		default:
			return 0;
	}

	return 1;
}

/* {{{ */
static zend_result pdo_pgsql_check_liveness(pdo_dbh_t *dbh)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	if (!PQconsumeInput(H->server) || PQstatus(H->server) == CONNECTION_BAD) {
		PQreset(H->server);
	}
	return (PQstatus(H->server) == CONNECTION_OK) ? SUCCESS : FAILURE;
}
/* }}} */

static bool pgsql_handle_in_transaction(pdo_dbh_t *dbh)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;

	return PQtransactionStatus(H->server) > PQTRANS_IDLE;
}

static bool pdo_pgsql_transaction_cmd(const char *cmd, pdo_dbh_t *dbh)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	PGresult *res;
	bool ret = true;

	res = PQexec(H->server, cmd);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		pdo_pgsql_error(dbh, PQresultStatus(res), pdo_pgsql_sqlstate(res));
		ret = false;
	}

	PQclear(res);
	return ret;
}

static bool pgsql_handle_begin(pdo_dbh_t *dbh)
{
	return pdo_pgsql_transaction_cmd("BEGIN", dbh);
}

static bool pgsql_handle_commit(pdo_dbh_t *dbh)
{
	bool ret = pdo_pgsql_transaction_cmd("COMMIT", dbh);

	/* When deferred constraints are used the commit could
	   fail, and a ROLLBACK implicitly ran. See bug #67462 */
	if (ret) {
		pdo_pgsql_close_lob_streams(dbh);
	} else {
		dbh->in_txn = pgsql_handle_in_transaction(dbh);
	}

	return ret;
}

static bool pgsql_handle_rollback(pdo_dbh_t *dbh)
{
	int ret = pdo_pgsql_transaction_cmd("ROLLBACK", dbh);

	if (ret) {
		pdo_pgsql_close_lob_streams(dbh);
	}

	return ret;
}

void pgsqlCopyFromArray_internal(INTERNAL_FUNCTION_PARAMETERS)
{
	pdo_dbh_t *dbh;
	pdo_pgsql_db_handle *H;

	zval *pg_rows;

	char *table_name, *pg_delim = NULL, *pg_null_as = NULL, *pg_fields = NULL;
	size_t table_name_len, pg_delim_len = 0, pg_null_as_len = 0, pg_fields_len;
	char *query;

	PGresult *pgsql_result;
	ExecStatusType status;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sa|sss!",
		&table_name, &table_name_len, &pg_rows,
		&pg_delim, &pg_delim_len, &pg_null_as, &pg_null_as_len, &pg_fields, &pg_fields_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (!zend_hash_num_elements(Z_ARRVAL_P(pg_rows))) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	dbh = Z_PDO_DBH_P(ZEND_THIS);
	PDO_CONSTRUCT_CHECK;
	PDO_DBH_CLEAR_ERR();

	/* using pre-9.0 syntax as PDO_pgsql is 7.4+ compatible */
	if (pg_fields) {
		spprintf(&query, 0, "COPY %s (%s) FROM STDIN WITH DELIMITER E'%c' NULL AS E'%s'", table_name, pg_fields, (pg_delim_len ? *pg_delim : '\t'), (pg_null_as_len ? pg_null_as : "\\\\N"));
	} else {
		spprintf(&query, 0, "COPY %s FROM STDIN WITH DELIMITER E'%c' NULL AS E'%s'", table_name, (pg_delim_len ? *pg_delim : '\t'), (pg_null_as_len ? pg_null_as : "\\\\N"));
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
		size_t buffer_len = 0;
		zval *tmp;

		PQclear(pgsql_result);
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(pg_rows), tmp) {
			size_t query_len;
			if (!try_convert_to_string(tmp)) {
				efree(query);
				RETURN_THROWS();
			}

			if (buffer_len < Z_STRLEN_P(tmp)) {
				buffer_len = Z_STRLEN_P(tmp);
				query = erealloc(query, buffer_len + 2); /* room for \n\0 */
			}
			query_len = Z_STRLEN_P(tmp);
			memcpy(query, Z_STRVAL_P(tmp), query_len);
			if (query[query_len - 1] != '\n') {
				query[query_len++] = '\n';
			}
			query[query_len] = '\0';
			if (PQputCopyData(H->server, query, query_len) != 1) {
				efree(query);
				pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, NULL);
				PDO_HANDLE_DBH_ERR();
				RETURN_FALSE;
			}
		} ZEND_HASH_FOREACH_END();
		if (query) {
			efree(query);
		}

		if (PQputCopyEnd(H->server, NULL) != 1) {
			pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, NULL);
			PDO_HANDLE_DBH_ERR();
			RETURN_FALSE;
		}

		while ((pgsql_result = PQgetResult(H->server))) {
			if (PGRES_COMMAND_OK != PQresultStatus(pgsql_result)) {
				pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, pdo_pgsql_sqlstate(pgsql_result));
				command_failed = 1;
			}
			PQclear(pgsql_result);
		}

		PDO_HANDLE_DBH_ERR();
		RETURN_BOOL(!command_failed);
	} else {
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, pdo_pgsql_sqlstate(pgsql_result));
		PQclear(pgsql_result);
		PDO_HANDLE_DBH_ERR();
		RETURN_FALSE;
	}
}

/* {{{ Returns true if the copy worked fine or false if error */
PHP_METHOD(PDO_PGSql_Ext, pgsqlCopyFromArray)
{
	pgsqlCopyFromArray_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

void pgsqlCopyFromFile_internal(INTERNAL_FUNCTION_PARAMETERS)
{
	pdo_dbh_t *dbh;
	pdo_pgsql_db_handle *H;

	char *table_name, *filename, *pg_delim = NULL, *pg_null_as = NULL, *pg_fields = NULL;
	size_t  table_name_len, filename_len, pg_delim_len = 0, pg_null_as_len = 0, pg_fields_len;
	char *query;
	PGresult *pgsql_result;
	ExecStatusType status;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sp|sss!",
		&table_name, &table_name_len, &filename, &filename_len,
		&pg_delim, &pg_delim_len, &pg_null_as, &pg_null_as_len, &pg_fields, &pg_fields_len) == FAILURE) {
		RETURN_THROWS();
	}

	/* Obtain db Handler */
	dbh = Z_PDO_DBH_P(ZEND_THIS);
	PDO_CONSTRUCT_CHECK;
	PDO_DBH_CLEAR_ERR();

	stream = php_stream_open_wrapper_ex(filename, "rb", 0, NULL, FG(default_context));
	if (!stream) {
		pdo_pgsql_error_msg(dbh, PGRES_FATAL_ERROR, "Unable to open the file");
		PDO_HANDLE_DBH_ERR();
		RETURN_FALSE;
	}

	/* using pre-9.0 syntax as PDO_pgsql is 7.4+ compatible */
	if (pg_fields) {
		spprintf(&query, 0, "COPY %s (%s) FROM STDIN WITH DELIMITER E'%c' NULL AS E'%s'", table_name, pg_fields, (pg_delim_len ? *pg_delim : '\t'), (pg_null_as_len ? pg_null_as : "\\\\N"));
	} else {
		spprintf(&query, 0, "COPY %s FROM STDIN WITH DELIMITER E'%c' NULL AS E'%s'", table_name, (pg_delim_len ? *pg_delim : '\t'), (pg_null_as_len ? pg_null_as : "\\\\N"));
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
				pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, NULL);
				php_stream_close(stream);
				PDO_HANDLE_DBH_ERR();
				RETURN_FALSE;
			}
			efree(buf);
		}
		php_stream_close(stream);

		if (PQputCopyEnd(H->server, NULL) != 1) {
			pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, NULL);
			PDO_HANDLE_DBH_ERR();
			RETURN_FALSE;
		}

		while ((pgsql_result = PQgetResult(H->server))) {
			if (PGRES_COMMAND_OK != PQresultStatus(pgsql_result)) {
				pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, pdo_pgsql_sqlstate(pgsql_result));
				command_failed = 1;
			}
			PQclear(pgsql_result);
		}

		PDO_HANDLE_DBH_ERR();
		RETURN_BOOL(!command_failed);
	} else {
		php_stream_close(stream);
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, pdo_pgsql_sqlstate(pgsql_result));
		PQclear(pgsql_result);
		PDO_HANDLE_DBH_ERR();
		RETURN_FALSE;
	}
}

/* {{{ Returns true if the copy worked fine or false if error */
PHP_METHOD(PDO_PGSql_Ext, pgsqlCopyFromFile)
{
	pgsqlCopyFromFile_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

void pgsqlCopyToFile_internal(INTERNAL_FUNCTION_PARAMETERS)
{
	pdo_dbh_t *dbh;
	pdo_pgsql_db_handle *H;

	char *table_name, *pg_delim = NULL, *pg_null_as = NULL, *pg_fields = NULL, *filename = NULL;
	size_t table_name_len, pg_delim_len = 0, pg_null_as_len = 0, pg_fields_len, filename_len;
	char *query;

	PGresult *pgsql_result;
	ExecStatusType status;

	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sp|sss!",
					&table_name, &table_name_len, &filename, &filename_len,
					&pg_delim, &pg_delim_len, &pg_null_as, &pg_null_as_len, &pg_fields, &pg_fields_len) == FAILURE) {
		RETURN_THROWS();
	}

	dbh = Z_PDO_DBH_P(ZEND_THIS);
	PDO_CONSTRUCT_CHECK;
	PDO_DBH_CLEAR_ERR();

	H = (pdo_pgsql_db_handle *)dbh->driver_data;

	stream = php_stream_open_wrapper_ex(filename, "wb", 0, NULL, FG(default_context));
	if (!stream) {
		pdo_pgsql_error_msg(dbh, PGRES_FATAL_ERROR, "Unable to open the file for writing");
		PDO_HANDLE_DBH_ERR();
		RETURN_FALSE;
	}

	while ((pgsql_result = PQgetResult(H->server))) {
		PQclear(pgsql_result);
	}

	/* using pre-9.0 syntax as PDO_pgsql is 7.4+ compatible */
	if (pg_fields) {
		spprintf(&query, 0, "COPY %s (%s) TO STDIN WITH DELIMITER E'%c' NULL AS E'%s'", table_name, pg_fields, (pg_delim_len ? *pg_delim : '\t'), (pg_null_as_len ? pg_null_as : "\\\\N"));
	} else {
		spprintf(&query, 0, "COPY %s TO STDIN WITH DELIMITER E'%c' NULL AS E'%s'", table_name, (pg_delim_len ? *pg_delim : '\t'), (pg_null_as_len ? pg_null_as : "\\\\N"));
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
				if (php_stream_write(stream, csv, ret) != (size_t)ret) {
					pdo_pgsql_error_msg(dbh, PGRES_FATAL_ERROR, "Unable to write to file");
					PQfreemem(csv);
					php_stream_close(stream);
					PDO_HANDLE_DBH_ERR();
					RETURN_FALSE;
				} else {
					PQfreemem(csv);
				}
			} else {
				pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, NULL);
				php_stream_close(stream);
				PDO_HANDLE_DBH_ERR();
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
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, pdo_pgsql_sqlstate(pgsql_result));
		PQclear(pgsql_result);
		PDO_HANDLE_DBH_ERR();
		RETURN_FALSE;
	}
}

/* {{{ Returns true if the copy worked fine or false if error */
PHP_METHOD(PDO_PGSql_Ext, pgsqlCopyToFile)
{
	pgsqlCopyToFile_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);

}
/* }}} */

void pgsqlCopyToArray_internal(INTERNAL_FUNCTION_PARAMETERS)
{
	pdo_dbh_t *dbh;
	pdo_pgsql_db_handle *H;

	char *table_name, *pg_delim = NULL, *pg_null_as = NULL, *pg_fields = NULL;
	size_t table_name_len, pg_delim_len = 0, pg_null_as_len = 0, pg_fields_len;
	char *query;

	PGresult *pgsql_result;
	ExecStatusType status;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|sss!",
		&table_name, &table_name_len,
		&pg_delim, &pg_delim_len, &pg_null_as, &pg_null_as_len, &pg_fields, &pg_fields_len) == FAILURE) {
		RETURN_THROWS();
	}

	dbh = Z_PDO_DBH_P(ZEND_THIS);
	PDO_CONSTRUCT_CHECK;
	PDO_DBH_CLEAR_ERR();

	H = (pdo_pgsql_db_handle *)dbh->driver_data;

	while ((pgsql_result = PQgetResult(H->server))) {
		PQclear(pgsql_result);
	}

	/* using pre-9.0 syntax as PDO_pgsql is 7.4+ compatible */
	if (pg_fields) {
		spprintf(&query, 0, "COPY %s (%s) TO STDIN WITH DELIMITER E'%c' NULL AS E'%s'", table_name, pg_fields, (pg_delim_len ? *pg_delim : '\t'), (pg_null_as_len ? pg_null_as : "\\\\N"));
	} else {
		spprintf(&query, 0, "COPY %s TO STDIN WITH DELIMITER E'%c' NULL AS E'%s'", table_name, (pg_delim_len ? *pg_delim : '\t'), (pg_null_as_len ? pg_null_as : "\\\\N"));
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
				add_next_index_stringl(return_value, csv, ret);
				PQfreemem(csv);
			} else {
				pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, NULL);
				PDO_HANDLE_DBH_ERR();
				RETURN_FALSE;
			}
		}

		while ((pgsql_result = PQgetResult(H->server))) {
			PQclear(pgsql_result);
		}
	} else {
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, pdo_pgsql_sqlstate(pgsql_result));
		PQclear(pgsql_result);
		PDO_HANDLE_DBH_ERR();
		RETURN_FALSE;
	}
}

/* {{{ Returns true if the copy worked fine or false if error */
PHP_METHOD(PDO_PGSql_Ext, pgsqlCopyToArray)
{
	pgsqlCopyToArray_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

void pgsqlLOBCreate_internal(INTERNAL_FUNCTION_PARAMETERS)
{
	pdo_dbh_t *dbh;
	pdo_pgsql_db_handle *H;
	Oid lfd;

	ZEND_PARSE_PARAMETERS_NONE();

	dbh = Z_PDO_DBH_P(ZEND_THIS);
	PDO_CONSTRUCT_CHECK;
	PDO_DBH_CLEAR_ERR();

	H = (pdo_pgsql_db_handle *)dbh->driver_data;
	lfd = lo_creat(H->server, INV_READ|INV_WRITE);

	if (lfd != InvalidOid) {
		zend_string *buf = strpprintf(0, ZEND_ULONG_FMT, (zend_long) lfd);

		RETURN_STR(buf);
	}

	pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, NULL);
	PDO_HANDLE_DBH_ERR();
	RETURN_FALSE;
}

/* {{{ Creates a new large object, returning its identifier.  Must be called inside a transaction. */
PHP_METHOD(PDO_PGSql_Ext, pgsqlLOBCreate)
{
	pgsqlLOBCreate_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

void pgsqlLOBOpen_internal(INTERNAL_FUNCTION_PARAMETERS)
{
	pdo_dbh_t *dbh;
	pdo_pgsql_db_handle *H;
	Oid oid;
	int lfd;
	char *oidstr;
	size_t oidstrlen;
	char *modestr = "rb";
	size_t modestrlen;
	int mode = INV_READ;
	char *end_ptr;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s|s",
				&oidstr, &oidstrlen, &modestr, &modestrlen)) {
		RETURN_THROWS();
	}

	oid = (Oid)strtoul(oidstr, &end_ptr, 10);
	if (oid == 0 && (errno == ERANGE || errno == EINVAL)) {
		RETURN_FALSE;
	}

	if (strpbrk(modestr, "+w")) {
		mode = INV_READ|INV_WRITE;
	}

	dbh = Z_PDO_DBH_P(ZEND_THIS);
	PDO_CONSTRUCT_CHECK;
	PDO_DBH_CLEAR_ERR();

	H = (pdo_pgsql_db_handle *)dbh->driver_data;

	lfd = lo_open(H->server, oid, mode);

	if (lfd >= 0) {
		php_stream *stream = pdo_pgsql_create_lob_stream(ZEND_THIS, lfd, oid);
		if (stream) {
			php_stream_to_zval(stream, return_value);
			return;
		}
	} else {
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, NULL);
	}

	PDO_HANDLE_DBH_ERR();
	RETURN_FALSE;
}

/* {{{ Opens an existing large object stream.  Must be called inside a transaction. */
PHP_METHOD(PDO_PGSql_Ext, pgsqlLOBOpen)
{
	pgsqlLOBOpen_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

void pgsqlLOBUnlink_internal(INTERNAL_FUNCTION_PARAMETERS)
{
	pdo_dbh_t *dbh;
	pdo_pgsql_db_handle *H;
	Oid oid;
	char *oidstr, *end_ptr;
	size_t oidlen;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&oidstr, &oidlen)) {
		RETURN_THROWS();
	}

	oid = (Oid)strtoul(oidstr, &end_ptr, 10);
	if (oid == 0 && (errno == ERANGE || errno == EINVAL)) {
		RETURN_FALSE;
	}

	dbh = Z_PDO_DBH_P(ZEND_THIS);
	PDO_CONSTRUCT_CHECK;
	PDO_DBH_CLEAR_ERR();

	H = (pdo_pgsql_db_handle *)dbh->driver_data;

	if (1 == lo_unlink(H->server, oid)) {
		RETURN_TRUE;
	}

	pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, NULL);
	PDO_HANDLE_DBH_ERR();
	RETURN_FALSE;
}

/* {{{ Deletes the large object identified by oid.  Must be called inside a transaction. */
PHP_METHOD(PDO_PGSql_Ext, pgsqlLOBUnlink)
{
	pgsqlLOBUnlink_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

void pgsqlGetNotify_internal(INTERNAL_FUNCTION_PARAMETERS)
{
	pdo_dbh_t *dbh;
	pdo_pgsql_db_handle *H;
	zend_long result_type = PDO_FETCH_USE_DEFAULT;
	zend_long ms_timeout = 0;
	PGnotify *pgsql_notify;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "|ll",
				&result_type, &ms_timeout)) {
		RETURN_THROWS();
	}

	dbh = Z_PDO_DBH_P(ZEND_THIS);
	PDO_CONSTRUCT_CHECK;

	if (result_type == PDO_FETCH_USE_DEFAULT) {
		result_type = dbh->default_fetch_type;
	}

	if (result_type != PDO_FETCH_BOTH && result_type != PDO_FETCH_ASSOC && result_type != PDO_FETCH_NUM) {
		zend_argument_value_error(1, "must be one of PDO::FETCH_BOTH, PDO::FETCH_ASSOC, or PDO::FETCH_NUM");
		RETURN_THROWS();
	}

	if (ms_timeout < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
#ifdef ZEND_ENABLE_ZVAL_LONG64
	} else if (ms_timeout > INT_MAX) {
		php_error_docref(NULL, E_WARNING, "Timeout was shrunk to %d", INT_MAX);
		ms_timeout = INT_MAX;
#endif
	}

	H = (pdo_pgsql_db_handle *)dbh->driver_data;

	if (!PQconsumeInput(H->server)) {
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, NULL);
		PDO_HANDLE_DBH_ERR();
		RETURN_FALSE;
	}
	pgsql_notify = PQnotifies(H->server);

	if (ms_timeout && !pgsql_notify) {
		php_pollfd_for_ms(PQsocket(H->server), PHP_POLLREADABLE, (int)ms_timeout);

		if (!PQconsumeInput(H->server)) {
			pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, NULL);
			PDO_HANDLE_DBH_ERR();
			RETURN_FALSE;
		}
		pgsql_notify = PQnotifies(H->server);
	}

	if (!pgsql_notify) {
		RETURN_FALSE;
	}

	array_init(return_value);
	if (result_type == PDO_FETCH_NUM || result_type == PDO_FETCH_BOTH) {
		add_index_string(return_value, 0, pgsql_notify->relname);
		add_index_long(return_value, 1, pgsql_notify->be_pid);
		if (pgsql_notify->extra && pgsql_notify->extra[0]) {
			add_index_string(return_value, 2, pgsql_notify->extra);
		}
	}
	if (result_type == PDO_FETCH_ASSOC || result_type == PDO_FETCH_BOTH) {
		add_assoc_string(return_value, "message", pgsql_notify->relname);
		add_assoc_long(return_value, "pid", pgsql_notify->be_pid);
		if (pgsql_notify->extra && pgsql_notify->extra[0]) {
			add_assoc_string(return_value, "payload", pgsql_notify->extra);
		}
	}

	PQfreemem(pgsql_notify);
}

/* {{{ Get asynchronous notification */
PHP_METHOD(PDO_PGSql_Ext, pgsqlGetNotify)
{
	pgsqlGetNotify_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

void pgsqlGetPid_internal(INTERNAL_FUNCTION_PARAMETERS)
{
	pdo_dbh_t *dbh;
	pdo_pgsql_db_handle *H;

	ZEND_PARSE_PARAMETERS_NONE();

	dbh = Z_PDO_DBH_P(ZEND_THIS);
	PDO_CONSTRUCT_CHECK;

	H = (pdo_pgsql_db_handle *)dbh->driver_data;

	RETURN_LONG(PQbackendPID(H->server));
}

/* {{{ Get backend(server) pid */
PHP_METHOD(PDO_PGSql_Ext, pgsqlGetPid)
{
	pgsqlGetPid_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

static const zend_function_entry *pdo_pgsql_get_driver_methods(pdo_dbh_t *dbh, int kind)
{
	switch (kind) {
		case PDO_DBH_DRIVER_METHOD_KIND_DBH:
			return class_PDO_PGSql_Ext_methods;
		default:
			return NULL;
	}
}

static bool pdo_pgsql_set_attr(pdo_dbh_t *dbh, zend_long attr, zval *val)
{
	bool bval;
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;

	switch (attr) {
		case PDO_ATTR_EMULATE_PREPARES:
			if (!pdo_get_bool_param(&bval, val)) {
				return false;
			}
			H->emulate_prepares = bval;
			return true;
		case PDO_PGSQL_ATTR_DISABLE_PREPARES:
			if (!pdo_get_bool_param(&bval, val)) {
				return false;
			}
			H->disable_prepares = bval;
			return true;
		default:
			return false;
	}
}

static const struct pdo_dbh_methods pgsql_methods = {
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
	NULL /* get_gc */
};

static int pdo_pgsql_handle_factory(pdo_dbh_t *dbh, zval *driver_options) /* {{{ */
{
	pdo_pgsql_db_handle *H;
	int ret = 0;
	char *conn_str, *p, *e;
	zend_string *tmp_user, *tmp_pass;
	zend_long connect_timeout = 30;

	H = pecalloc(1, sizeof(pdo_pgsql_db_handle), dbh->is_persistent);
	dbh->driver_data = H;

	dbh->skip_param_evt =
		1 << PDO_PARAM_EVT_EXEC_POST |
		1 << PDO_PARAM_EVT_FETCH_PRE |
		1 << PDO_PARAM_EVT_FETCH_POST;

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
		connect_timeout = pdo_attr_lval(driver_options, PDO_ATTR_TIMEOUT, 30);
	}

	/* escape username and password, if provided */
	tmp_user = !strstr((char *) dbh->data_source, "user=") ? _pdo_pgsql_escape_credentials(dbh->username) : NULL;
	tmp_pass = !strstr((char *) dbh->data_source, "password=") ? _pdo_pgsql_escape_credentials(dbh->password) : NULL;

	/* support both full connection string & connection string + login and/or password */
	if (tmp_user && tmp_pass) {
		spprintf(&conn_str, 0, "%s user='%s' password='%s' connect_timeout=" ZEND_LONG_FMT, (char *) dbh->data_source, ZSTR_VAL(tmp_user), ZSTR_VAL(tmp_pass), connect_timeout);
	} else if (tmp_user) {
		spprintf(&conn_str, 0, "%s user='%s' connect_timeout=" ZEND_LONG_FMT, (char *) dbh->data_source, ZSTR_VAL(tmp_user), connect_timeout);
	} else if (tmp_pass) {
		spprintf(&conn_str, 0, "%s password='%s' connect_timeout=" ZEND_LONG_FMT, (char *) dbh->data_source, ZSTR_VAL(tmp_pass), connect_timeout);
	} else {
		spprintf(&conn_str, 0, "%s connect_timeout=" ZEND_LONG_FMT, (char *) dbh->data_source, connect_timeout);
	}

	H->server = PQconnectdb(conn_str);
	H->lob_streams = (HashTable *) pemalloc(sizeof(HashTable), dbh->is_persistent);
	zend_hash_init(H->lob_streams, 0, NULL, NULL, 1);

	if (tmp_user) {
		zend_string_release_ex(tmp_user, 0);
	}
	if (tmp_pass) {
		zend_string_release_ex(tmp_pass, 0);
	}

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
		pgsql_handle_closer(dbh);
	}

	return ret;
}
/* }}} */

const pdo_driver_t pdo_pgsql_driver = {
	PDO_DRIVER_HEADER(pgsql),
	pdo_pgsql_handle_factory
};
