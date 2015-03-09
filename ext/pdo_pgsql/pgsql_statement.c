/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
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
#include "php_pdo_pgsql.h"
#include "php_pdo_pgsql_int.h"
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

/* from postgresql/src/include/catalog/pg_type.h */
#define BOOLOID     16
#define BYTEAOID    17
#define INT8OID     20
#define INT2OID     21
#define INT4OID     23
#define TEXTOID     25
#define OIDOID      26

static int pgsql_stmt_dtor(pdo_stmt_t *stmt)
{
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;

	if (S->result) {
		/* free the resource */
		PQclear(S->result);
		S->result = NULL;
	}

	if (S->stmt_name) {
		pdo_pgsql_db_handle *H = S->H;
		char *q = NULL;
		PGresult *res;

		if (S->is_prepared) {
			spprintf(&q, 0, "DEALLOCATE %s", S->stmt_name);
			res = PQexec(H->server, q);
			efree(q);
			if (res) {
				PQclear(res);
			}
		}
		efree(S->stmt_name);
		S->stmt_name = NULL;
	}
	if (S->param_lengths) {
		efree(S->param_lengths);
		S->param_lengths = NULL;
	}
	if (S->param_values) {
		efree(S->param_values);
		S->param_values = NULL;
	}
	if (S->param_formats) {
		efree(S->param_formats);
		S->param_formats = NULL;
	}
	if (S->param_types) {
		efree(S->param_types);
		S->param_types = NULL;
	}
	if (S->query) {
		efree(S->query);
		S->query = NULL;
	}

	if (S->cursor_name) {
		pdo_pgsql_db_handle *H = S->H;
		char *q = NULL;
		PGresult *res;

		spprintf(&q, 0, "CLOSE %s", S->cursor_name);
		res = PQexec(H->server, q);
		efree(q);
		if (res) PQclear(res);
		efree(S->cursor_name);
		S->cursor_name = NULL;
	}

	if(S->cols) {
		efree(S->cols);
		S->cols = NULL;
	}
	efree(S);
	stmt->driver_data = NULL;
	return 1;
}

static int pgsql_stmt_execute(pdo_stmt_t *stmt)
{
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;
	pdo_pgsql_db_handle *H = S->H;
	ExecStatusType status;

	/* ensure that we free any previous unfetched results */
	if(S->result) {
		PQclear(S->result);
		S->result = NULL;
	}

	S->current_row = 0;

	if (S->cursor_name) {
		char *q = NULL;

		if (S->is_prepared) {
			spprintf(&q, 0, "CLOSE %s", S->cursor_name);
			S->result = PQexec(H->server, q);
			efree(q);
		}

		spprintf(&q, 0, "DECLARE %s SCROLL CURSOR WITH HOLD FOR %s", S->cursor_name, stmt->active_query_string);
		S->result = PQexec(H->server, q);
		efree(q);

		/* check if declare failed */
		status = PQresultStatus(S->result);
		if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
			pdo_pgsql_error_stmt(stmt, status, pdo_pgsql_sqlstate(S->result));
			return 0;
		}

		/* the cursor was declared correctly */
		S->is_prepared = 1;

		/* fetch to be able to get the number of tuples later, but don't advance the cursor pointer */
		spprintf(&q, 0, "FETCH FORWARD 0 FROM %s", S->cursor_name);
		S->result = PQexec(H->server, q);
		efree(q);
	} else if (S->stmt_name) {
		/* using a prepared statement */

		if (!S->is_prepared) {
stmt_retry:
			/* we deferred the prepare until now, because we didn't
			 * know anything about the parameter types; now we do */
			S->result = PQprepare(H->server, S->stmt_name, S->query,
						stmt->bound_params ? zend_hash_num_elements(stmt->bound_params) : 0,
						S->param_types);
			status = PQresultStatus(S->result);
			switch (status) {
				case PGRES_COMMAND_OK:
				case PGRES_TUPLES_OK:
					/* it worked */
					S->is_prepared = 1;
					PQclear(S->result);
					break;
				default: {
					char *sqlstate = pdo_pgsql_sqlstate(S->result);
					/* 42P05 means that the prepared statement already existed. this can happen if you use
					 * a connection pooling software line pgpool which doesn't close the db-connection once
					 * php disconnects. if php dies (no chance to run RSHUTDOWN) during execution it has no
					 * chance to DEALLOCATE the prepared statements it has created. so, if we hit a 42P05 we
					 * deallocate it and retry ONCE (thies 2005.12.15)
					 */
					if (sqlstate && !strcmp(sqlstate, "42P05")) {
						char buf[100]; /* stmt_name == "pdo_crsr_%08x" */
						PGresult *res;
						snprintf(buf, sizeof(buf), "DEALLOCATE %s", S->stmt_name);
						res = PQexec(H->server, buf);
						if (res) {
							PQclear(res);
						}
						goto stmt_retry;
					} else {
						pdo_pgsql_error_stmt(stmt, status, sqlstate);
						return 0;
					}
				}
			}
		}
		S->result = PQexecPrepared(H->server, S->stmt_name,
				stmt->bound_params ?
					zend_hash_num_elements(stmt->bound_params) :
					0,
				(const char**)S->param_values,
				S->param_lengths,
				S->param_formats,
				0);
	} else if (stmt->supports_placeholders == PDO_PLACEHOLDER_NAMED) {
		/* execute query with parameters */
		S->result = PQexecParams(H->server, S->query,
				stmt->bound_params ? zend_hash_num_elements(stmt->bound_params) : 0,
				S->param_types,
				(const char**)S->param_values,
				S->param_lengths,
				S->param_formats,
				0);
	} else {
		/* execute plain query (with embedded parameters) */
		S->result = PQexec(H->server, stmt->active_query_string);
	}
	status = PQresultStatus(S->result);

	if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
		pdo_pgsql_error_stmt(stmt, status, pdo_pgsql_sqlstate(S->result));
		return 0;
	}

	if (!stmt->executed && !stmt->column_count) {
		stmt->column_count = (int) PQnfields(S->result);
		S->cols = ecalloc(stmt->column_count, sizeof(pdo_pgsql_column));
	}

	if (status == PGRES_COMMAND_OK) {
		ZEND_ATOL(stmt->row_count, PQcmdTuples(S->result));
		H->pgoid = PQoidValue(S->result);
	} else {
		stmt->row_count = (zend_long)PQntuples(S->result);
	}

	return 1;
}

static int pgsql_stmt_param_hook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param,
		enum pdo_param_event event_type)
{
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;

	if (stmt->supports_placeholders == PDO_PLACEHOLDER_NAMED && param->is_param) {
		switch (event_type) {
			case PDO_PARAM_EVT_FREE:
				if (param->driver_data) {
					efree(param->driver_data);
				}
				break;

			case PDO_PARAM_EVT_NORMALIZE:
				/* decode name from $1, $2 into 0, 1 etc. */
				if (param->name) {
					if (param->name->val[0] == '$') {
						ZEND_ATOL(param->paramno, param->name->val + 1);
					} else {
						/* resolve parameter name to rewritten name */
						char *namevar;

						if (stmt->bound_param_map && (namevar = zend_hash_find_ptr(stmt->bound_param_map,
								param->name)) != NULL) {
							ZEND_ATOL(param->paramno, namevar + 1);
							param->paramno--;
						} else {
							pdo_raise_impl_error(stmt->dbh, stmt, "HY093", param->name->val);
							return 0;
						}
					}
				}
				break;

			case PDO_PARAM_EVT_ALLOC:
			case PDO_PARAM_EVT_EXEC_POST:
			case PDO_PARAM_EVT_FETCH_PRE:
			case PDO_PARAM_EVT_FETCH_POST:
				/* work is handled by EVT_NORMALIZE */
				return 1;

			case PDO_PARAM_EVT_EXEC_PRE:
				if (!stmt->bound_param_map) {
					return 0;
				}
				if (!S->param_values) {
					S->param_values = ecalloc(
							zend_hash_num_elements(stmt->bound_param_map),
							sizeof(char*));
					S->param_lengths = ecalloc(
							zend_hash_num_elements(stmt->bound_param_map),
							sizeof(int));
					S->param_formats = ecalloc(
							zend_hash_num_elements(stmt->bound_param_map),
							sizeof(int));
					S->param_types = ecalloc(
							zend_hash_num_elements(stmt->bound_param_map),
							sizeof(Oid));
				}
				if (param->paramno >= 0) {
					zval *parameter;

					if (param->paramno >= zend_hash_num_elements(stmt->bound_param_map)) {
						pdo_pgsql_error_stmt(stmt, PGRES_FATAL_ERROR, "HY105");
						return 0;
					}

					if (Z_ISREF(param->parameter)) {
						parameter = Z_REFVAL(param->parameter);
					} else {
						parameter = &param->parameter;
					}

					if (PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_LOB &&
							Z_TYPE_P(parameter) == IS_RESOURCE) {
						php_stream *stm;
						php_stream_from_zval_no_verify(stm, parameter);
						if (stm) {
							if (php_stream_is(stm, &pdo_pgsql_lob_stream_ops)) {
								struct pdo_pgsql_lob_self *self = (struct pdo_pgsql_lob_self*)stm->abstract;
								pdo_pgsql_bound_param *P = param->driver_data;

								if (P == NULL) {
									P = ecalloc(1, sizeof(*P));
									param->driver_data = P;
								}
								P->oid = htonl(self->oid);
								S->param_values[param->paramno] = (char*)&P->oid;
								S->param_lengths[param->paramno] = sizeof(P->oid);
								S->param_formats[param->paramno] = 1;
								S->param_types[param->paramno] = OIDOID;
								return 1;
							} else {
								zend_string *str = php_stream_copy_to_mem(stm, PHP_STREAM_COPY_ALL, 0);
								if (str != NULL) {
									//??SEPARATE_ZVAL_IF_NOT_REF(&param->parameter);
									ZVAL_STR(parameter, str);
								} else {
									ZVAL_EMPTY_STRING(parameter);
								}
							}
						} else {
							/* expected a stream resource */
							pdo_pgsql_error_stmt(stmt, PGRES_FATAL_ERROR, "HY105");
							return 0;
						}
					}

					if (PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_NULL ||
							Z_TYPE_P(parameter) == IS_NULL) {
						S->param_values[param->paramno] = NULL;
						S->param_lengths[param->paramno] = 0;
					} else if (Z_TYPE_P(parameter) == IS_FALSE || Z_TYPE_P(parameter) == IS_TRUE) {
						S->param_values[param->paramno] = Z_TYPE_P(parameter) == IS_TRUE ? "t" : "f";
						S->param_lengths[param->paramno] = 1;
						S->param_formats[param->paramno] = 0;
					} else {
						//SEPARATE_ZVAL_IF_NOT_REF(&param->parameter);
						convert_to_string_ex(parameter);
						S->param_values[param->paramno] = Z_STRVAL_P(parameter);
						S->param_lengths[param->paramno] = Z_STRLEN_P(parameter);
						S->param_formats[param->paramno] = 0;
					}

					if (PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_LOB) {
						S->param_types[param->paramno] = 0;
						S->param_formats[param->paramno] = 1;
					} else {
						S->param_types[param->paramno] = 0;
					}
				}
				break;
		}
	} else if (param->is_param) {
		/* We need to manually convert to a pg native boolean value */
		if (PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_BOOL &&
			((param->param_type & PDO_PARAM_INPUT_OUTPUT) != PDO_PARAM_INPUT_OUTPUT)) {
			SEPARATE_ZVAL(&param->parameter);
			param->param_type = PDO_PARAM_STR;
			convert_to_boolean(&param->parameter);
			ZVAL_STRINGL(&param->parameter, Z_TYPE_P(&param->parameter) == IS_TRUE ? "t" : "f", 1);
		}
	}
	return 1;
}

static int pgsql_stmt_fetch(pdo_stmt_t *stmt,
	enum pdo_fetch_orientation ori, zend_long offset)
{
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;

	if (S->cursor_name) {
		char *ori_str = NULL;
		char *q = NULL;
		ExecStatusType status;

		switch (ori) {
			case PDO_FETCH_ORI_NEXT: 	spprintf(&ori_str, 0, "NEXT"); break;
			case PDO_FETCH_ORI_PRIOR:	spprintf(&ori_str, 0, "BACKWARD"); break;
			case PDO_FETCH_ORI_FIRST:	spprintf(&ori_str, 0, "FIRST"); break;
			case PDO_FETCH_ORI_LAST:	spprintf(&ori_str, 0, "LAST"); break;
			case PDO_FETCH_ORI_ABS:		spprintf(&ori_str, 0, "ABSOLUTE %pd", offset); break;
			case PDO_FETCH_ORI_REL:		spprintf(&ori_str, 0, "RELATIVE %pd", offset); break;
			default:
				return 0;
		}

		spprintf(&q, 0, "FETCH %s FROM %s", ori_str, S->cursor_name);
		efree(ori_str);
		S->result = PQexec(S->H->server, q);
		efree(q);
		status = PQresultStatus(S->result);

		if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
			pdo_pgsql_error_stmt(stmt, status, pdo_pgsql_sqlstate(S->result));
			return 0;
		}

		if (PQntuples(S->result)) {
			S->current_row = 1;
			return 1;
		} else {
			return 0;
		}
	} else {
		if (S->current_row < stmt->row_count) {
			S->current_row++;
			return 1;
		} else {
			return 0;
		}
	}
}

static int pgsql_stmt_describe(pdo_stmt_t *stmt, int colno)
{
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;
	struct pdo_column_data *cols = stmt->columns;
	struct pdo_bound_param_data *param;

	if (!S->result) {
		return 0;
	}

	cols[colno].name = estrdup(PQfname(S->result, colno));
	cols[colno].namelen = strlen(cols[colno].name);
	cols[colno].maxlen = PQfsize(S->result, colno);
	cols[colno].precision = PQfmod(S->result, colno);
	S->cols[colno].pgsql_type = PQftype(S->result, colno);

	switch (S->cols[colno].pgsql_type) {

		case BOOLOID:
			cols[colno].param_type = PDO_PARAM_BOOL;
			break;

		case OIDOID:
			/* did the user bind the column as a LOB ? */
			if (stmt->bound_columns && (
					(param = zend_hash_index_find_ptr(stmt->bound_columns, colno)) != NULL ||
					(param = zend_hash_str_find_ptr(stmt->bound_columns, cols[colno].name, cols[colno].namelen)) != NULL)) {

				if (PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_LOB) {
					cols[colno].param_type = PDO_PARAM_LOB;
					break;
				}
			}
			cols[colno].param_type = PDO_PARAM_INT;
			break;

		case INT2OID:
		case INT4OID:
			cols[colno].param_type = PDO_PARAM_INT;
			break;

		case INT8OID:
			if (sizeof(zend_long)>=8) {
				cols[colno].param_type = PDO_PARAM_INT;
			} else {
				cols[colno].param_type = PDO_PARAM_STR;
			}
			break;

		case BYTEAOID:
			cols[colno].param_type = PDO_PARAM_LOB;
			break;

		default:
			cols[colno].param_type = PDO_PARAM_STR;
	}

	return 1;
}

static int pgsql_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr, zend_ulong *len, int *caller_frees )
{
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;
	struct pdo_column_data *cols = stmt->columns;
	size_t tmp_len;

	if (!S->result) {
		return 0;
	}

	/* We have already increased count by 1 in pgsql_stmt_fetch() */
	if (PQgetisnull(S->result, S->current_row - 1, colno)) { /* Check if we got NULL */
		*ptr = NULL;
		*len = 0;
	} else {
		*ptr = PQgetvalue(S->result, S->current_row - 1, colno);
		*len = PQgetlength(S->result, S->current_row - 1, colno);

		switch (cols[colno].param_type) {

			case PDO_PARAM_INT:
				ZEND_ATOL(S->cols[colno].intval, *ptr);
				*ptr = (char *) &(S->cols[colno].intval);
				*len = sizeof(zend_long);
				break;

			case PDO_PARAM_BOOL:
				S->cols[colno].boolval = **ptr == 't' ? 1: 0;
				*ptr = (char *) &(S->cols[colno].boolval);
				*len = sizeof(zend_bool);
				break;

			case PDO_PARAM_LOB:
				if (S->cols[colno].pgsql_type == OIDOID) {
					/* ooo, a real large object */
					char *end_ptr;
					Oid oid = (Oid)strtoul(*ptr, &end_ptr, 10);
					int loid = lo_open(S->H->server, oid, INV_READ);
					if (loid >= 0) {
						*ptr = (char*)pdo_pgsql_create_lob_stream(&stmt->database_object_handle, loid, oid);
						*len = 0;
						return *ptr ? 1 : 0;
					}
					*ptr = NULL;
					*len = 0;
					return 0;
				} else {
					char *tmp_ptr = (char *)PQunescapeBytea((unsigned char *)*ptr, &tmp_len);
					if (!tmp_ptr) {
						/* PQunescapeBytea returned an error */
						*len = 0;
						return 0;
					}
					if (!tmp_len) {
						/* Empty string, return as empty stream */
						*ptr = (char *)php_stream_memory_open(TEMP_STREAM_READONLY, "", 0);
						PQfreemem(tmp_ptr);
						*len = 0;
					} else {
						*ptr = estrndup(tmp_ptr, tmp_len);
						PQfreemem(tmp_ptr);
						*len = tmp_len;
						*caller_frees = 1;
					}
				}
				break;
			case PDO_PARAM_NULL:
			case PDO_PARAM_STR:
			case PDO_PARAM_STMT:
			case PDO_PARAM_INPUT_OUTPUT:
			case PDO_PARAM_ZVAL:
			default:
				break;
		}
	}

	return 1;
}

static int pgsql_stmt_get_column_meta(pdo_stmt_t *stmt, zend_long colno, zval *return_value)
{
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;
	PGresult *res;
	char *q=NULL;
	ExecStatusType status;

	if (!S->result) {
		return FAILURE;
	}

	if (colno >= stmt->column_count) {
		return FAILURE;
	}

	array_init(return_value);
	add_assoc_long(return_value, "pgsql:oid", S->cols[colno].pgsql_type);

	/* Fetch metadata from Postgres system catalogue */
	spprintf(&q, 0, "SELECT TYPNAME FROM PG_TYPE WHERE OID=%u", S->cols[colno].pgsql_type);
	res = PQexec(S->H->server, q);
	efree(q);

	status = PQresultStatus(res);

	if (status != PGRES_TUPLES_OK) {
		/* Failed to get system catalogue, but return success
		 * with the data we have collected so far
		 */
		goto done;
	}

	/* We want exactly one row returned */
	if (1 != PQntuples(res)) {
		goto done;
	}

	add_assoc_string(return_value, "native_type", PQgetvalue(res, 0, 0));
done:
	PQclear(res);
	return 1;
}

static int pdo_pgsql_stmt_cursor_closer(pdo_stmt_t *stmt)
{
	return 1;
}

struct pdo_stmt_methods pgsql_stmt_methods = {
	pgsql_stmt_dtor,
	pgsql_stmt_execute,
	pgsql_stmt_fetch,
	pgsql_stmt_describe,
	pgsql_stmt_get_col,
	pgsql_stmt_param_hook,
	NULL, /* set_attr */
	NULL, /* get_attr */
	pgsql_stmt_get_column_meta,
	NULL,  /* next_rowset */
	pdo_pgsql_stmt_cursor_closer
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
