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
  | Author: Wez Furlong <wez@php.net>                                    |
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
#include "php_pdo_sqlite.h"
#include "php_pdo_sqlite_int.h"


static int pdo_sqlite_stmt_dtor(pdo_stmt_t *stmt)
{
	pdo_sqlite_stmt *S = (pdo_sqlite_stmt*)stmt->driver_data;

	if (S->stmt) {
		sqlite3_finalize(S->stmt);
		S->stmt = NULL;
	}
	efree(S);
	return 1;
}

static int pdo_sqlite_stmt_execute(pdo_stmt_t *stmt)
{
	pdo_sqlite_stmt *S = (pdo_sqlite_stmt*)stmt->driver_data;

	if (stmt->executed && !S->done) {
		sqlite3_reset(S->stmt);
	}

	S->done = 0;
	switch (sqlite3_step(S->stmt)) {
		case SQLITE_ROW:
			S->pre_fetched = 1;
			stmt->column_count = sqlite3_data_count(S->stmt);
			return 1;

		case SQLITE_DONE:
			stmt->column_count = sqlite3_column_count(S->stmt);
			stmt->row_count = sqlite3_changes(S->H->db);
			sqlite3_reset(S->stmt);
			S->done = 1;
			return 1;

		case SQLITE_ERROR:
			sqlite3_reset(S->stmt);
		case SQLITE_MISUSE:
		case SQLITE_BUSY:
		default:
			pdo_sqlite_error_stmt(stmt);
			return 0;
	}
}

static int pdo_sqlite_stmt_param_hook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param,
		enum pdo_param_event event_type)
{
	pdo_sqlite_stmt *S = (pdo_sqlite_stmt*)stmt->driver_data;
	zval *parameter;

	switch (event_type) {
		case PDO_PARAM_EVT_EXEC_PRE:
			if (stmt->executed && !S->done) {
				sqlite3_reset(S->stmt);
				S->done = 1;
			}

			if (param->is_param) {

				if (param->paramno == -1) {
					param->paramno = sqlite3_bind_parameter_index(S->stmt, ZSTR_VAL(param->name)) - 1;
				}

				switch (PDO_PARAM_TYPE(param->param_type)) {
					case PDO_PARAM_STMT:
						return 0;

					case PDO_PARAM_NULL:
						if (sqlite3_bind_null(S->stmt, param->paramno + 1) == SQLITE_OK) {
							return 1;
						}
						pdo_sqlite_error_stmt(stmt);
						return 0;

					case PDO_PARAM_INT:
					case PDO_PARAM_BOOL:
						if (Z_ISREF(param->parameter)) {
							parameter = Z_REFVAL(param->parameter);
						} else {
							parameter = &param->parameter;
						}
						if (Z_TYPE_P(parameter) == IS_NULL) {
							if (sqlite3_bind_null(S->stmt, param->paramno + 1) == SQLITE_OK) {
								return 1;
							}
						} else {
							convert_to_long(parameter);
#if ZEND_LONG_MAX > 2147483647
							if (SQLITE_OK == sqlite3_bind_int64(S->stmt, param->paramno + 1, Z_LVAL_P(parameter))) {
								return 1;
							}
#else
							if (SQLITE_OK == sqlite3_bind_int(S->stmt, param->paramno + 1, Z_LVAL_P(parameter))) {
								return 1;
							}
#endif
						}
						pdo_sqlite_error_stmt(stmt);
						return 0;

					case PDO_PARAM_LOB:
						if (Z_ISREF(param->parameter)) {
							parameter = Z_REFVAL(param->parameter);
						} else {
							parameter = &param->parameter;
						}
						if (Z_TYPE_P(parameter) == IS_RESOURCE) {
							php_stream *stm = NULL;
							php_stream_from_zval_no_verify(stm, parameter);
							if (stm) {
								zend_string *mem = php_stream_copy_to_mem(stm, PHP_STREAM_COPY_ALL, 0);
								zval_ptr_dtor(parameter);
								ZVAL_STR(parameter, mem ? mem : ZSTR_EMPTY_ALLOC());
							} else {
								pdo_raise_impl_error(stmt->dbh, stmt, "HY105", "Expected a stream resource");
								return 0;
							}
						} else if (Z_TYPE_P(parameter) == IS_NULL) {
							if (sqlite3_bind_null(S->stmt, param->paramno + 1) == SQLITE_OK) {
								return 1;
							}
							pdo_sqlite_error_stmt(stmt);
							return 0;
						} else {
							convert_to_string(parameter);
						}

						if (SQLITE_OK == sqlite3_bind_blob(S->stmt, param->paramno + 1,
								Z_STRVAL_P(parameter),
								Z_STRLEN_P(parameter),
								SQLITE_STATIC)) {
							return 1;
						}
						return 0;

					case PDO_PARAM_STR:
					default:
						if (Z_ISREF(param->parameter)) {
							parameter = Z_REFVAL(param->parameter);
						} else {
							parameter = &param->parameter;
						}
						if (Z_TYPE_P(parameter) == IS_NULL) {
							if (sqlite3_bind_null(S->stmt, param->paramno + 1) == SQLITE_OK) {
								return 1;
							}
						} else {
							convert_to_string(parameter);
							if (SQLITE_OK == sqlite3_bind_text(S->stmt, param->paramno + 1,
									Z_STRVAL_P(parameter),
									Z_STRLEN_P(parameter),
									SQLITE_STATIC)) {
								return 1;
							}
						}
						pdo_sqlite_error_stmt(stmt);
						return 0;
				}
			}
			break;

		default:
			;
	}
	return 1;
}

static int pdo_sqlite_stmt_fetch(pdo_stmt_t *stmt,
	enum pdo_fetch_orientation ori, zend_long offset)
{
	pdo_sqlite_stmt *S = (pdo_sqlite_stmt*)stmt->driver_data;
	int i;
	if (!S->stmt) {
		return 0;
	}
	if (S->pre_fetched) {
		S->pre_fetched = 0;
		return 1;
	}
	if (S->done) {
		return 0;
	}
	i = sqlite3_step(S->stmt);
	switch (i) {
		case SQLITE_ROW:
			return 1;

		case SQLITE_DONE:
			S->done = 1;
			sqlite3_reset(S->stmt);
			return 0;

		case SQLITE_ERROR:
			sqlite3_reset(S->stmt);
		default:
			pdo_sqlite_error_stmt(stmt);
			return 0;
	}
}

static int pdo_sqlite_stmt_describe(pdo_stmt_t *stmt, int colno)
{
	pdo_sqlite_stmt *S = (pdo_sqlite_stmt*)stmt->driver_data;
	const char *str;

	if(colno >= sqlite3_column_count(S->stmt)) {
		/* error invalid column */
		pdo_sqlite_error_stmt(stmt);
		return 0;
	}

	str = sqlite3_column_name(S->stmt, colno);
	stmt->columns[colno].name = zend_string_init(str, strlen(str), 0);
	stmt->columns[colno].maxlen = 0xffffffff;
	stmt->columns[colno].precision = 0;

	switch (sqlite3_column_type(S->stmt, colno)) {
		case SQLITE_INTEGER:
		case SQLITE_FLOAT:
		case SQLITE3_TEXT:
		case SQLITE_BLOB:
		case SQLITE_NULL:
		default:
			stmt->columns[colno].param_type = PDO_PARAM_STR;
			break;
	}

	return 1;
}

static int pdo_sqlite_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr, zend_ulong *len, int *caller_frees)
{
	pdo_sqlite_stmt *S = (pdo_sqlite_stmt*)stmt->driver_data;
	if (!S->stmt) {
		return 0;
	}
	if(colno >= sqlite3_data_count(S->stmt)) {
		/* error invalid column */
		pdo_sqlite_error_stmt(stmt);
		return 0;
	}
	switch (sqlite3_column_type(S->stmt, colno)) {
		case SQLITE_NULL:
			*ptr = NULL;
			*len = 0;
			return 1;

		case SQLITE_BLOB:
			*ptr = (char*)sqlite3_column_blob(S->stmt, colno);
			*len = sqlite3_column_bytes(S->stmt, colno);
			return 1;

		default:
			*ptr = (char*)sqlite3_column_text(S->stmt, colno);
			*len = sqlite3_column_bytes(S->stmt, colno);
			return 1;
	}
}

static int pdo_sqlite_stmt_col_meta(pdo_stmt_t *stmt, zend_long colno, zval *return_value)
{
	pdo_sqlite_stmt *S = (pdo_sqlite_stmt*)stmt->driver_data;
	const char *str;
	zval flags;

	if (!S->stmt) {
		return FAILURE;
	}
	if(colno >= sqlite3_data_count(S->stmt)) {
		/* error invalid column */
		pdo_sqlite_error_stmt(stmt);
		return FAILURE;
	}

	array_init(return_value);
	array_init(&flags);

	switch (sqlite3_column_type(S->stmt, colno)) {
		case SQLITE_NULL:
			add_assoc_string(return_value, "native_type", "null");
			break;

		case SQLITE_FLOAT:
			add_assoc_string(return_value, "native_type", "double");
			break;

		case SQLITE_BLOB:
			add_next_index_string(&flags, "blob");
		case SQLITE_TEXT:
			add_assoc_string(return_value, "native_type", "string");
			break;

		case SQLITE_INTEGER:
			add_assoc_string(return_value, "native_type", "integer");
			break;
	}

	str = sqlite3_column_decltype(S->stmt, colno);
	if (str) {
		add_assoc_string(return_value, "sqlite:decl_type", (char *)str);
	}

#ifdef SQLITE_ENABLE_COLUMN_METADATA
	str = sqlite3_column_table_name(S->stmt, colno);
	if (str) {
		add_assoc_string(return_value, "table", (char *)str);
	}
#endif

	add_assoc_zval(return_value, "flags", &flags);

	return SUCCESS;
}

static int pdo_sqlite_stmt_cursor_closer(pdo_stmt_t *stmt)
{
	pdo_sqlite_stmt *S = (pdo_sqlite_stmt*)stmt->driver_data;
	sqlite3_reset(S->stmt);
	return 1;
}

struct pdo_stmt_methods sqlite_stmt_methods = {
	pdo_sqlite_stmt_dtor,
	pdo_sqlite_stmt_execute,
	pdo_sqlite_stmt_fetch,
	pdo_sqlite_stmt_describe,
	pdo_sqlite_stmt_get_col,
	pdo_sqlite_stmt_param_hook,
	NULL, /* set_attr */
	NULL, /* get_attr */
	pdo_sqlite_stmt_col_meta,
	NULL, /* next_rowset */
	pdo_sqlite_stmt_cursor_closer
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
