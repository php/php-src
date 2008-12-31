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
  | Author: Wez Furlong <wez@php.net>                                    |
  +----------------------------------------------------------------------+
*/

/* $Id$ */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"

#ifdef PHP_SQLITE2_HAVE_PDO
#include "sqlite.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "zend_exceptions.h"

#define php_sqlite_encode_binary(in, n, out) sqlite_encode_binary((const unsigned char *)in, n, (unsigned char *)out)
#define php_sqlite_decode_binary(in, out)    sqlite_decode_binary((const unsigned char *)in, (unsigned char *)out)


typedef struct {
	const char *file;
	int line;
	unsigned int errcode;
	char *errmsg;
} pdo_sqlite2_error_info;

typedef struct {
	sqlite *db;
	pdo_sqlite2_error_info einfo;
} pdo_sqlite2_db_handle;

typedef struct {
	pdo_sqlite2_db_handle 	*H;
	sqlite_vm *vm;
	const char **rowdata, **colnames;
	int ncols;
	unsigned pre_fetched:1;
	unsigned done:1;
	pdo_sqlite2_error_info einfo;
} pdo_sqlite2_stmt;

extern int _pdo_sqlite2_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, char *errmsg, const char *file, int line TSRMLS_DC);
#define pdo_sqlite2_error(msg, s) _pdo_sqlite2_error(s, NULL, msg, __FILE__, __LINE__ TSRMLS_CC)
#define pdo_sqlite2_error_stmt(msg, s) _pdo_sqlite2_error(stmt->dbh, stmt, msg, __FILE__, __LINE__ TSRMLS_CC)

extern struct pdo_stmt_methods sqlite2_stmt_methods;

static int pdo_sqlite2_stmt_dtor(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_sqlite2_stmt *S = (pdo_sqlite2_stmt*)stmt->driver_data;

	if (S->vm) {
		char *errmsg = NULL;
		sqlite_finalize(S->vm, &errmsg);
		if (errmsg) {
			sqlite_freemem(errmsg);
		}
		S->vm = NULL;
	}
	if (S->einfo.errmsg) {
		pefree(S->einfo.errmsg, stmt->dbh->is_persistent);
	}
	efree(S);
	return 1;
}

static int pdo_sqlite2_stmt_execute(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_sqlite2_stmt *S = (pdo_sqlite2_stmt*)stmt->driver_data;
	char *errmsg = NULL;
	const char *tail;

	if (stmt->executed && !S->done) {
		sqlite_finalize(S->vm, &errmsg);
		pdo_sqlite2_error_stmt(errmsg, stmt);
		errmsg = NULL;
		S->vm = NULL;
	}

	S->einfo.errcode = sqlite_compile(S->H->db, stmt->active_query_string, &tail, &S->vm, &errmsg);
	if (S->einfo.errcode != SQLITE_OK) {
		pdo_sqlite2_error_stmt(errmsg, stmt);
		return 0;
	}

	S->done = 0;
	S->einfo.errcode = sqlite_step(S->vm, &S->ncols, &S->rowdata, &S->colnames);
	switch (S->einfo.errcode) {
		case SQLITE_ROW:
			S->pre_fetched = 1;
			stmt->column_count = S->ncols;
			return 1;

		case SQLITE_DONE:
			stmt->column_count = S->ncols;
			stmt->row_count = sqlite_changes(S->H->db);
			S->einfo.errcode = sqlite_reset(S->vm, &errmsg);
			if (S->einfo.errcode != SQLITE_OK) {
				pdo_sqlite2_error_stmt(errmsg, stmt);
			}
			S->done = 1;
			return 1;

		case SQLITE_ERROR:
		case SQLITE_MISUSE:
		case SQLITE_BUSY:
		default:
			pdo_sqlite2_error_stmt(errmsg, stmt);
			return 0;
	}
}

static int pdo_sqlite2_stmt_param_hook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param,
		enum pdo_param_event event_type TSRMLS_DC)
{
	return 1;
}

static int pdo_sqlite2_stmt_fetch(pdo_stmt_t *stmt,
	enum pdo_fetch_orientation ori, long offset TSRMLS_DC)
{
	pdo_sqlite2_stmt *S = (pdo_sqlite2_stmt*)stmt->driver_data;
	char *errmsg = NULL;

	if (!S->vm) {
		return 0;	
	}
	if (S->pre_fetched) {
		S->pre_fetched = 0;
		return 1;
	}
	if (S->done) {
		return 0;
	}

	S->einfo.errcode = sqlite_step(S->vm, &S->ncols, &S->rowdata, &S->colnames);
	switch (S->einfo.errcode) {
		case SQLITE_ROW:
			return 1;

		case SQLITE_DONE:
			S->done = 1;
			S->einfo.errcode = sqlite_reset(S->vm, &errmsg);
			if (S->einfo.errcode != SQLITE_OK) {
				pdo_sqlite2_error_stmt(errmsg, stmt);
				errmsg = NULL;
			}
			return 0;

		default:
			pdo_sqlite2_error_stmt(errmsg, stmt);
			return 0;
	}
}

static int pdo_sqlite2_stmt_describe(pdo_stmt_t *stmt, int colno TSRMLS_DC)
{
	pdo_sqlite2_stmt *S = (pdo_sqlite2_stmt*)stmt->driver_data;

	if(colno >= S->ncols) {
		/* error invalid column */
		pdo_sqlite2_error_stmt(NULL, stmt);
		return 0;
	}

	stmt->columns[colno].name = estrdup(S->colnames[colno]);
	stmt->columns[colno].namelen = strlen(stmt->columns[colno].name);
	stmt->columns[colno].maxlen = 0xffffffff;
	stmt->columns[colno].precision = 0;
	stmt->columns[colno].param_type = PDO_PARAM_STR;

	return 1;
}

static int pdo_sqlite2_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr, unsigned long *len, int *caller_frees TSRMLS_DC)
{
	pdo_sqlite2_stmt *S = (pdo_sqlite2_stmt*)stmt->driver_data;
	if (!S->vm) {
		return 0;
	}
	if(colno >= S->ncols) {
		/* error invalid column */
		pdo_sqlite2_error_stmt(NULL, stmt);
		return 0;
	}
	if (S->rowdata[colno]) {
		if (S->rowdata[colno][0] == '\x01') {
			/* encoded */
			*caller_frees = 1;
			*ptr = emalloc(strlen(S->rowdata[colno]));
			*len = php_sqlite_decode_binary(S->rowdata[colno]+1, *ptr);
			(*(char**)ptr)[*len] = '\0';
		} else {
			*ptr = (char*)S->rowdata[colno];
			*len = strlen(*ptr);
		}
	} else {
		*ptr = NULL;
		*len = 0;
	}
	return 1;
}

struct pdo_stmt_methods sqlite2_stmt_methods = {
	pdo_sqlite2_stmt_dtor,
	pdo_sqlite2_stmt_execute,
	pdo_sqlite2_stmt_fetch,
	pdo_sqlite2_stmt_describe,
	pdo_sqlite2_stmt_get_col,
	pdo_sqlite2_stmt_param_hook,
	NULL, /* set_attr */
	NULL, /* get_attr */
	NULL
};


int _pdo_sqlite2_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, char *errmsg, const char *file, int line TSRMLS_DC) /* {{{ */
{
	pdo_sqlite2_db_handle *H = (pdo_sqlite2_db_handle *)dbh->driver_data;
	pdo_error_type *pdo_err = stmt ? &stmt->error_code : &dbh->error_code;
	pdo_sqlite2_error_info *einfo = &H->einfo;
	pdo_sqlite2_stmt *S;

	if (stmt) {
		S = stmt->driver_data;
		einfo = &S->einfo;
	}

	einfo->file = file;
	einfo->line = line;

	if (einfo->errmsg) {
		pefree(einfo->errmsg, dbh->is_persistent);
		einfo->errmsg = NULL;
	}

	if (einfo->errcode != SQLITE_OK) {
		if (errmsg) {
			einfo->errmsg = pestrdup(errmsg, dbh->is_persistent);
			sqlite_freemem(errmsg);
		} else {
			einfo->errmsg = pestrdup(sqlite_error_string(einfo->errcode), dbh->is_persistent);
		}
	} else { /* no error */
		strcpy(*pdo_err, PDO_ERR_NONE);
		return 0;
	}
	switch (einfo->errcode) {
		case SQLITE_NOTFOUND:
			strcpy(*pdo_err, "42S02");
			break;	

		case SQLITE_INTERRUPT:
			strcpy(*pdo_err, "01002");
			break;

		case SQLITE_NOLFS:
			strcpy(*pdo_err, "HYC00");
			break;

		case SQLITE_TOOBIG:
			strcpy(*pdo_err, "22001");
			break;

		case SQLITE_CONSTRAINT:
			strcpy(*pdo_err, "23000");
			break;

		case SQLITE_ERROR:
		default:
			strcpy(*pdo_err, "HY000");
			break;
	}

	if (!dbh->methods) {
		zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, "SQLSTATE[%s] [%d] %s",
				*pdo_err, einfo->errcode, einfo->errmsg);
	}

	return einfo->errcode;
}
/* }}} */

static int pdo_sqlite2_fetch_error_func(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info TSRMLS_DC)
{
	pdo_sqlite2_db_handle *H = (pdo_sqlite2_db_handle *)dbh->driver_data;
	pdo_sqlite2_error_info *einfo = &H->einfo;
	pdo_sqlite2_stmt *S;

	if (stmt) {
		S = stmt->driver_data;
		einfo = &S->einfo;
	}

	if (einfo->errcode) {
		add_next_index_long(info, einfo->errcode);
		if (einfo->errmsg) {
			add_next_index_string(info, einfo->errmsg, 1);
		}
	}

	return 1;
}

static int sqlite2_handle_closer(pdo_dbh_t *dbh TSRMLS_DC) /* {{{ */
{
	pdo_sqlite2_db_handle *H = (pdo_sqlite2_db_handle *)dbh->driver_data;
	
	if (H) {
		if (H->db) {
			sqlite_close(H->db);
			H->db = NULL;
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

static int sqlite2_handle_preparer(pdo_dbh_t *dbh, const char *sql, long sql_len, pdo_stmt_t *stmt, zval *driver_options TSRMLS_DC)
{
	pdo_sqlite2_db_handle *H = (pdo_sqlite2_db_handle *)dbh->driver_data;
	pdo_sqlite2_stmt *S = ecalloc(1, sizeof(pdo_sqlite2_stmt));

	S->H = H;
	stmt->driver_data = S;
	stmt->methods = &sqlite2_stmt_methods;
	stmt->supports_placeholders = PDO_PLACEHOLDER_NONE;

	if (PDO_CURSOR_FWDONLY != pdo_attr_lval(driver_options, PDO_ATTR_CURSOR, PDO_CURSOR_FWDONLY TSRMLS_CC)) {
		H->einfo.errcode = SQLITE_ERROR;
		pdo_sqlite2_error(NULL, dbh);
		return 0;
	}

	return 1;
}

static long sqlite2_handle_doer(pdo_dbh_t *dbh, const char *sql, long sql_len TSRMLS_DC)
{
	pdo_sqlite2_db_handle *H = (pdo_sqlite2_db_handle *)dbh->driver_data;
	char *errmsg = NULL;

	if ((H->einfo.errcode = sqlite_exec(H->db, sql, NULL, NULL, &errmsg)) != SQLITE_OK) {
		pdo_sqlite2_error(errmsg, dbh);
		return -1;
	} else {
		return sqlite_changes(H->db);
	}
}

static char *pdo_sqlite2_last_insert_id(pdo_dbh_t *dbh, const char *name, unsigned int *len TSRMLS_DC)
{
	pdo_sqlite2_db_handle *H = (pdo_sqlite2_db_handle *)dbh->driver_data;
	char *id;
	
	id = php_pdo_int64_to_str(sqlite_last_insert_rowid(H->db) TSRMLS_CC);
	*len = strlen(id);
	return id;
}

static int sqlite2_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, int unquotedlen, char **quoted, int *quotedlen, enum pdo_param_type paramtype  TSRMLS_DC)
{
	char *ret;

	if (unquotedlen && (unquoted[0] == '\x01' || memchr(unquoted, '\0', unquotedlen) != NULL)) {
		/* binary string */
		int len;
		ret = safe_emalloc(1 + unquotedlen / 254, 257, 5);
		ret[0] = '\'';
		ret[1] = '\x01';
		len = php_sqlite_encode_binary(unquoted, unquotedlen, ret+2);
		ret[len + 2] = '\'';
		ret[len + 3] = '\0';
		*quoted = ret;
		*quotedlen = len + 3;
		/* fprintf(stderr, "Quoting:%d:%.*s:\n", *quotedlen, *quotedlen, *quoted); */
		return 1;
	} else if (unquotedlen) {
		ret = sqlite_mprintf("'%q'", unquoted);
		if (ret) {
			*quoted = estrdup(ret);
			*quotedlen = strlen(ret);
			sqlite_freemem(ret);
			return 1;
		}
		return 0;
	} else {
		*quoted = estrdup("''");
		*quotedlen = 2;
		return 1;
	}
}

static int sqlite2_handle_begin(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_sqlite2_db_handle *H = (pdo_sqlite2_db_handle *)dbh->driver_data;
	char *errmsg = NULL;

	if (sqlite_exec(H->db, "BEGIN", NULL, NULL, &errmsg) != SQLITE_OK) {
		pdo_sqlite2_error(errmsg, dbh);
		return 0;
	}
	return 1;
}

static int sqlite2_handle_commit(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_sqlite2_db_handle *H = (pdo_sqlite2_db_handle *)dbh->driver_data;
	char *errmsg = NULL;

	if (sqlite_exec(H->db, "COMMIT", NULL, NULL, &errmsg) != SQLITE_OK) {
		pdo_sqlite2_error(errmsg, dbh);
		return 0;
	}
	return 1;
}

static int sqlite2_handle_rollback(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_sqlite2_db_handle *H = (pdo_sqlite2_db_handle *)dbh->driver_data;
	char *errmsg = NULL;

	if (sqlite_exec(H->db, "ROLLBACK", NULL, NULL, &errmsg) != SQLITE_OK) {
		pdo_sqlite2_error(errmsg, dbh);
		return 0;
	}
	return 1;
}

static int pdo_sqlite2_get_attribute(pdo_dbh_t *dbh, long attr, zval *return_value TSRMLS_DC)
{
	switch (attr) {
		case PDO_ATTR_CLIENT_VERSION:
		case PDO_ATTR_SERVER_VERSION:
			ZVAL_STRING(return_value, (char *)sqlite_libversion(), 1);
			break;
		
		default:
			return 0;	
	}

	return 1;
}

static int pdo_sqlite2_set_attr(pdo_dbh_t *dbh, long attr, zval *val TSRMLS_DC)
{
	pdo_sqlite2_db_handle *H = (pdo_sqlite2_db_handle *)dbh->driver_data;

	switch (attr) {
		case PDO_ATTR_TIMEOUT:
			convert_to_long(val);
			sqlite_busy_timeout(H->db, Z_LVAL_P(val) * 1000);
			return 1;
	}
	return 0;
}

static PHP_FUNCTION(sqlite2_create_function)
{
	/* TODO: implement this stuff */
}

static zend_function_entry dbh_methods[] = {
	PHP_FE(sqlite2_create_function, NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry *get_driver_methods(pdo_dbh_t *dbh, int kind TSRMLS_DC)
{
	switch (kind) {
		case PDO_DBH_DRIVER_METHOD_KIND_DBH:
			return dbh_methods;

		default:
			return NULL;
	}
}

static struct pdo_dbh_methods sqlite2_methods = {
	sqlite2_handle_closer,
	sqlite2_handle_preparer,
	sqlite2_handle_doer,
	sqlite2_handle_quoter,
	sqlite2_handle_begin,
	sqlite2_handle_commit,
	sqlite2_handle_rollback,
	pdo_sqlite2_set_attr,
	pdo_sqlite2_last_insert_id,
	pdo_sqlite2_fetch_error_func,
	pdo_sqlite2_get_attribute,
	NULL,	/* check_liveness: not needed */
	get_driver_methods
};

static char *make_filename_safe(const char *filename TSRMLS_DC)
{
	if (strncmp(filename, ":memory:", sizeof(":memory:")-1)) {
		char *fullpath = expand_filepath(filename, NULL TSRMLS_CC);

		if (!fullpath) {
			return NULL;
		}

		if (PG(safe_mode) && (!php_checkuid(fullpath, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
			efree(fullpath);
			return NULL;
		}

		if (php_check_open_basedir(fullpath TSRMLS_CC)) {
			efree(fullpath);
			return NULL;
		}
		return fullpath;
	}
	return estrdup(filename);
}

static int authorizer(void *autharg, int access_type, const char *arg3, const char *arg4,
		const char *arg5, const char *arg6)
{
	char *filename;
	switch (access_type) {
		case SQLITE_COPY: {
			TSRMLS_FETCH();
			filename = make_filename_safe(arg4 TSRMLS_CC);
			if (!filename) {
				return SQLITE_DENY;
			}
			efree(filename);
			return SQLITE_OK;
		}

		case SQLITE_ATTACH: {
			TSRMLS_FETCH();
			filename = make_filename_safe(arg3 TSRMLS_CC);
			if (!filename) {
				return SQLITE_DENY;
			}
			efree(filename);
			return SQLITE_OK;
		}

		default:
			/* access allowed */
			return SQLITE_OK;
	}
}

static int pdo_sqlite2_handle_factory(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC) /* {{{ */
{
	pdo_sqlite2_db_handle *H;
	int ret = 0;
	long timeout = 60;
	char *filename;
	char *errmsg = NULL;

	H = pecalloc(1, sizeof(pdo_sqlite2_db_handle), dbh->is_persistent);

	H->einfo.errcode = 0;
	H->einfo.errmsg = NULL;
	dbh->driver_data = H;

	filename = make_filename_safe(dbh->data_source TSRMLS_CC);

	if (!filename) {
		zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC,
				"safe_mode/open_basedir prohibits opening %s",
				dbh->data_source);
		goto cleanup;
	}

	H->db = sqlite_open(filename, 0666, &errmsg);
	efree(filename);

	if (!H->db) {
		pdo_sqlite2_error(errmsg, dbh);
		goto cleanup;
	}

	sqlite_set_authorizer(H->db, authorizer, NULL);

	if (driver_options) {
		timeout = pdo_attr_lval(driver_options, PDO_ATTR_TIMEOUT, timeout TSRMLS_CC);
	}
	sqlite_busy_timeout(H->db, timeout * 1000);

	dbh->alloc_own_columns = 1;
	dbh->max_escaped_char_length = 2;

	ret = 1;

cleanup:
	dbh->methods = &sqlite2_methods;

	return ret;
}
/* }}} */

pdo_driver_t pdo_sqlite2_driver = {
	PDO_DRIVER_HEADER(sqlite2),
	pdo_sqlite2_handle_factory
};



#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
