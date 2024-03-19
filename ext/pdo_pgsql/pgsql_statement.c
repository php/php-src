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
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "php_pdo_pgsql.h"
#include "php_pdo_pgsql_int.h"
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

/* from postgresql/src/include/catalog/pg_type.h */
#define BOOLLABEL   "bool"
#define BOOLOID     16
#define BYTEALABEL  "bytea"
#define BYTEAOID    17
#define DATELABEL   "date"
#define DATEOID     1082
#define INT2LABEL   "int2"
#define INT2OID     21
#define INT4LABEL   "int4"
#define INT4OID     23
#define INT8LABEL   "int8"
#define INT8OID     20
#define OIDOID      26
#define TEXTLABEL   "text"
#define TEXTOID     25
#define TIMESTAMPLABEL "timestamp"
#define TIMESTAMPOID   1114
#define VARCHARLABEL "varchar"
#define VARCHAROID   1043
#define FLOAT4LABEL "float4"
#define FLOAT4OID 700
#define FLOAT8LABEL "float8"
#define FLOAT8OID 701



static int pgsql_stmt_dtor(pdo_stmt_t *stmt)
{
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;
	bool server_obj_usable = !Z_ISUNDEF(stmt->database_object_handle)
		&& IS_OBJ_VALID(EG(objects_store).object_buckets[Z_OBJ_HANDLE(stmt->database_object_handle)])
		&& !(OBJ_FLAGS(Z_OBJ(stmt->database_object_handle)) & IS_OBJ_FREE_CALLED);

	if (S->result) {
		/* free the resource */
		PQclear(S->result);
		S->result = NULL;
	}

	if (S->stmt_name) {
		if (S->is_prepared && server_obj_usable) {
			pdo_pgsql_db_handle *H = S->H;
			char *q = NULL;
			PGresult *res;

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
		zend_string_release(S->query);
		S->query = NULL;
	}

	if (S->cursor_name) {
		if (server_obj_usable) {
			pdo_pgsql_db_handle *H = S->H;
			char *q = NULL;
			PGresult *res;

			spprintf(&q, 0, "CLOSE %s", S->cursor_name);
			res = PQexec(H->server, q);
			efree(q);
			if (res) PQclear(res);
		}
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

	bool in_trans = stmt->dbh->methods->in_transaction(stmt->dbh);

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
			PQclear(PQexec(H->server, q));
			efree(q);
		}

		spprintf(&q, 0, "DECLARE %s SCROLL CURSOR WITH HOLD FOR %s", S->cursor_name, ZSTR_VAL(stmt->active_query_string));
		S->result = PQexec(H->server, q);
		efree(q);

		/* check if declare failed */
		status = PQresultStatus(S->result);
		if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
			pdo_pgsql_error_stmt(stmt, status, pdo_pgsql_sqlstate(S->result));
			return 0;
		}
		PQclear(S->result);

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
			S->result = PQprepare(H->server, S->stmt_name, ZSTR_VAL(S->query),
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
		S->result = PQexecParams(H->server, ZSTR_VAL(S->query),
				stmt->bound_params ? zend_hash_num_elements(stmt->bound_params) : 0,
				S->param_types,
				(const char**)S->param_values,
				S->param_lengths,
				S->param_formats,
				0);
	} else {
		/* execute plain query (with embedded parameters) */
		S->result = PQexec(H->server, ZSTR_VAL(stmt->active_query_string));
	}
	status = PQresultStatus(S->result);

	if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
		pdo_pgsql_error_stmt(stmt, status, pdo_pgsql_sqlstate(S->result));
		return 0;
	}

	stmt->column_count = (int) PQnfields(S->result);
	if (S->cols == NULL) {
		S->cols = ecalloc(stmt->column_count, sizeof(pdo_pgsql_column));
	}

	if (status == PGRES_COMMAND_OK) {
		stmt->row_count = ZEND_ATOL(PQcmdTuples(S->result));
		H->pgoid = PQoidValue(S->result);
	} else {
		stmt->row_count = (zend_long)PQntuples(S->result);
	}

	if (in_trans && !stmt->dbh->methods->in_transaction(stmt->dbh)) {
		pdo_pgsql_close_lob_streams(stmt->dbh);
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
					if (ZSTR_VAL(param->name)[0] == '$') {
						param->paramno = ZEND_ATOL(ZSTR_VAL(param->name) + 1);
					} else {
						/* resolve parameter name to rewritten name */
						zend_string *namevar;

						if (stmt->bound_param_map && (namevar = zend_hash_find_ptr(stmt->bound_param_map,
								param->name)) != NULL) {
							param->paramno = ZEND_ATOL(ZSTR_VAL(namevar) + 1);
							param->paramno--;
						} else {
							pdo_pgsql_error_stmt_msg(stmt, 0, "HY093", ZSTR_VAL(param->name));
							return 0;
						}
					}
				}
				break;

			case PDO_PARAM_EVT_ALLOC:
				if (!stmt->bound_param_map) {
					return 1;
				}
				if (!zend_hash_index_exists(stmt->bound_param_map, param->paramno)) {
					pdo_pgsql_error_stmt_msg(stmt, 0, "HY093", "parameter was not defined");
					return 0;
				}
				ZEND_FALLTHROUGH;
			case PDO_PARAM_EVT_EXEC_POST:
			case PDO_PARAM_EVT_FETCH_PRE:
			case PDO_PARAM_EVT_FETCH_POST:
				/* work is handled by EVT_NORMALIZE */
				return 1;

			case PDO_PARAM_EVT_EXEC_PRE:
				if (!stmt->bound_param_map) {
					return 1;
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

					/*
					if (param->paramno >= zend_hash_num_elements(stmt->bound_params)) {
						pdo_raise_impl_error(stmt->dbh, stmt, "HY093", "parameter was not defined");
						return 0;
					}
					*/

					if (Z_ISREF(param->parameter)) {
						parameter = Z_REFVAL(param->parameter);
					} else {
						parameter = &param->parameter;
					}

exec_pre_retry:
					switch (PDO_PARAM_TYPE(param->param_type)) {
						case PDO_PARAM_NULL:
							S->param_values[param->paramno] = NULL;
							S->param_lengths[param->paramno] = 0;
							break;

						case PDO_PARAM_STMT:
							return 0;

						case PDO_PARAM_INT:
							if (Z_TYPE_P(parameter) == IS_NULL) {
								param->param_type = PDO_PARAM_NULL;
								goto exec_pre_retry;
							}
							convert_to_long(parameter);
							convert_to_string(parameter);
							S->param_values[param->paramno] = Z_STRVAL_P(parameter);
							S->param_lengths[param->paramno] = Z_STRLEN_P(parameter);
							S->param_formats[param->paramno] = 0;
#if SIZEOF_ZEND_LONG >= 8
							S->param_types[param->paramno] = INT8OID;
#else
							S->param_types[param->paramno] = INT4OID;
#endif
							break;

						case PDO_PARAM_BOOL:
							if (Z_TYPE_P(parameter) == IS_NULL) {
								param->param_type = PDO_PARAM_NULL;
								goto exec_pre_retry;
							}
							convert_to_boolean(parameter);
							S->param_values[param->paramno] = Z_TYPE_P(parameter) == IS_TRUE ? "t" : "f";
							S->param_lengths[param->paramno] = 1;
							S->param_formats[param->paramno] = 0;
							break;

						case PDO_PARAM_LOB:
							if (Z_TYPE_P(parameter) == IS_RESOURCE) {
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
							} else if (Z_TYPE_P(parameter) == IS_NULL) {
								param->param_type = PDO_PARAM_NULL;
								goto exec_pre_retry;
							} else {
								// fallback
								if (!try_convert_to_string(parameter)) {
									return 0;
								}
							}
							S->param_values[param->paramno] = Z_STRVAL_P(parameter);
							S->param_lengths[param->paramno] = Z_STRLEN_P(parameter);
							S->param_types[param->paramno] = 0;
							S->param_formats[param->paramno] = 1;
							break;

						case PDO_PARAM_STR:
						default:
							if (Z_TYPE_P(parameter) == IS_NULL) {
								param->param_type = PDO_PARAM_NULL;
								goto exec_pre_retry;
							}
							if (!try_convert_to_string(parameter)) {
								return 0;
							}
							S->param_values[param->paramno] = Z_STRVAL_P(parameter);
							S->param_lengths[param->paramno] = Z_STRLEN_P(parameter);
							S->param_formats[param->paramno] = 0;
							break;
					}
				}
				break;
		}
	} else if (param->is_param && event_type == PDO_PARAM_EVT_NORMALIZE) {
		/* We need to manually convert to a pg native boolean value */
		if (PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_BOOL &&
			Z_TYPE_P(&param->parameter) != IS_NULL &&
			((param->param_type & PDO_PARAM_INPUT_OUTPUT) != PDO_PARAM_INPUT_OUTPUT)) {
			const char *s = zend_is_true(&param->parameter) ? "t" : "f";
			param->param_type = PDO_PARAM_STR;
			zval_ptr_dtor(&param->parameter);
			ZVAL_STRINGL(&param->parameter, s, 1);
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
			case PDO_FETCH_ORI_ABS:		spprintf(&ori_str, 0, "ABSOLUTE " ZEND_LONG_FMT, offset); break;
			case PDO_FETCH_ORI_REL:		spprintf(&ori_str, 0, "RELATIVE " ZEND_LONG_FMT, offset); break;
			default:
				return 0;
		}

		if(S->result) {
			PQclear(S->result);
			S->result = NULL;
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
	char *str;

	if (!S->result) {
		return 0;
	}

	str = PQfname(S->result, colno);
	cols[colno].name = zend_string_init(str, strlen(str), 0);
	cols[colno].maxlen = PQfsize(S->result, colno);
	cols[colno].precision = PQfmod(S->result, colno);
	S->cols[colno].pgsql_type = PQftype(S->result, colno);

	return 1;
}

static int pgsql_stmt_get_col(pdo_stmt_t *stmt, int colno, zval *result, enum pdo_param_type *type)
{
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;
	if (!S->result) {
		return 0;
	}

	/* We have already increased count by 1 in pgsql_stmt_fetch() */
	if (PQgetisnull(S->result, S->current_row - 1, colno)) { /* Check if we got NULL */
		ZVAL_NULL(result);
	} else {
		char *ptr = PQgetvalue(S->result, S->current_row - 1, colno);
		size_t len = PQgetlength(S->result, S->current_row - 1, colno);

		switch (S->cols[colno].pgsql_type) {
			case BOOLOID:
				ZVAL_BOOL(result, *ptr == 't');
				break;

			case INT2OID:
			case INT4OID:
#if SIZEOF_ZEND_LONG >= 8
			case INT8OID:
#endif
				ZVAL_LONG(result, ZEND_ATOL(ptr));
				break;
			case FLOAT4OID:
			case FLOAT8OID:
                if (strncmp(ptr, "Infinity", len) == 0) {
                    ZVAL_DOUBLE(result, ZEND_INFINITY);
                } else if (strncmp(ptr, "-Infinity", len) == 0) {
                    ZVAL_DOUBLE(result, -ZEND_INFINITY);
                } else if (strncmp(ptr, "NaN", len) == 0) {
                    ZVAL_DOUBLE(result, ZEND_NAN);
                } else {
                    ZVAL_DOUBLE(result, zend_strtod(ptr, NULL));
                }
				break;

			case OIDOID: {
				char *end_ptr;
				Oid oid = (Oid)strtoul(ptr, &end_ptr, 10);
				if (type && *type == PDO_PARAM_LOB) {
					/* If column was bound as LOB, return a stream. */
					int loid = lo_open(S->H->server, oid, INV_READ);
					if (loid >= 0) {
						php_stream *stream = pdo_pgsql_create_lob_stream(&stmt->database_object_handle, loid, oid);
						if (stream) {
							php_stream_to_zval(stream, result);
							return 1;
						}
					}
					return 0;
				} else {
					/* Otherwise return OID as integer. */
					ZVAL_LONG(result, oid);
				}
				break;
			}

			case BYTEAOID: {
				size_t tmp_len;
				char *tmp_ptr = (char *)PQunescapeBytea((unsigned char *) ptr, &tmp_len);
				if (!tmp_ptr) {
					/* PQunescapeBytea returned an error */
					return 0;
				}

				zend_string *str = zend_string_init(tmp_ptr, tmp_len, 0);
				php_stream *stream = php_stream_memory_open(TEMP_STREAM_READONLY, str);
				php_stream_to_zval(stream, result);
				zend_string_release(str);
				PQfreemem(tmp_ptr);
				break;
			}

			default:
				ZVAL_STRINGL_FAST(result, ptr, len);
				break;
		}
	}

	return 1;
}

static zend_always_inline char * pdo_pgsql_translate_oid_to_table(Oid oid, PGconn *conn)
{
	char *table_name = NULL;
	PGresult *tmp_res;
	char *querystr = NULL;

	spprintf(&querystr, 0, "SELECT RELNAME FROM PG_CLASS WHERE OID=%d", oid);

	if ((tmp_res = PQexec(conn, querystr)) == NULL || PQresultStatus(tmp_res) != PGRES_TUPLES_OK) {
		if (tmp_res) {
			PQclear(tmp_res);
		}
		efree(querystr);
		return 0;
	}
	efree(querystr);

	if (1 == PQgetisnull(tmp_res, 0, 0) || (table_name = PQgetvalue(tmp_res, 0, 0)) == NULL) {
		PQclear(tmp_res);
		return 0;
	}

	table_name = estrdup(table_name);

	PQclear(tmp_res);
	return table_name;
}

static int pgsql_stmt_get_column_meta(pdo_stmt_t *stmt, zend_long colno, zval *return_value)
{
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;
	PGresult *res;
	char *q=NULL;
	ExecStatusType status;
	Oid table_oid;
	char *table_name=NULL;

	if (!S->result) {
		return FAILURE;
	}

	if (colno >= stmt->column_count) {
		return FAILURE;
	}

	array_init(return_value);
	add_assoc_long(return_value, "pgsql:oid", S->cols[colno].pgsql_type);

	table_oid = PQftable(S->result, colno);
	add_assoc_long(return_value, "pgsql:table_oid", table_oid);
	table_name = pdo_pgsql_translate_oid_to_table(table_oid, S->H->server);
	if (table_name) {
		add_assoc_string(return_value, "table", table_name);
		efree(table_name);
	}

	switch (S->cols[colno].pgsql_type) {
		case BOOLOID:
			add_assoc_string(return_value, "native_type", BOOLLABEL);
			break;
		case BYTEAOID:
			add_assoc_string(return_value, "native_type", BYTEALABEL);
			break;
		case INT8OID:
			add_assoc_string(return_value, "native_type", INT8LABEL);
			break;
		case INT2OID:
			add_assoc_string(return_value, "native_type", INT2LABEL);
			break;
		case INT4OID:
			add_assoc_string(return_value, "native_type", INT4LABEL);
			break;
		case FLOAT4OID:
			add_assoc_string(return_value, "native_type", FLOAT4LABEL);
			break;
		case FLOAT8OID:
			add_assoc_string(return_value, "native_type", FLOAT8LABEL);
			break;
		case TEXTOID:
			add_assoc_string(return_value, "native_type", TEXTLABEL);
			break;
		case VARCHAROID:
			add_assoc_string(return_value, "native_type", VARCHARLABEL);
			break;
		case DATEOID:
			add_assoc_string(return_value, "native_type", DATELABEL);
			break;
		case TIMESTAMPOID:
			add_assoc_string(return_value, "native_type", TIMESTAMPLABEL);
			break;
		default:
			/* Fetch metadata from Postgres system catalogue */
			spprintf(&q, 0, "SELECT TYPNAME FROM PG_TYPE WHERE OID=%u", S->cols[colno].pgsql_type);
			res = PQexec(S->H->server, q);
			efree(q);
			status = PQresultStatus(res);
			if (status == PGRES_TUPLES_OK && 1 == PQntuples(res)) {
				add_assoc_string(return_value, "native_type", PQgetvalue(res, 0, 0));
			}
			PQclear(res);
	}

	enum pdo_param_type param_type;
	switch (S->cols[colno].pgsql_type) {
		case BOOLOID:
			param_type = PDO_PARAM_BOOL;
			break;
		case INT2OID:
		case INT4OID:
		case INT8OID:
			param_type = PDO_PARAM_INT;
			break;
		case OIDOID:
		case BYTEAOID:
			param_type = PDO_PARAM_LOB;
			break;
		default:
			param_type = PDO_PARAM_STR;
	}
	add_assoc_long(return_value, "pdo_type", param_type);

	return 1;
}

static int pdo_pgsql_stmt_cursor_closer(pdo_stmt_t *stmt)
{
	return 1;
}

const struct pdo_stmt_methods pgsql_stmt_methods = {
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
