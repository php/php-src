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
  | Author: Ard Biesheuvel <abies@php.net>                               |
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
#include "php_pdo_firebird.h"
#include "php_pdo_firebird_int.h"

#include <time.h>

#define READ_AND_RETURN_USING_MEMCPY(type, sqldata) do { \
		type ret; \
		memcpy(&ret, sqldata, sizeof(ret)); \
		return ret; \
	} while (0);

static zend_always_inline ISC_INT64 php_get_isc_int64_from_sqldata(const ISC_SCHAR *sqldata)
{
	READ_AND_RETURN_USING_MEMCPY(ISC_INT64, sqldata);
}

static zend_always_inline ISC_LONG php_get_isc_long_from_sqldata(const ISC_SCHAR *sqldata)
{
	READ_AND_RETURN_USING_MEMCPY(ISC_LONG, sqldata);
}

static zend_always_inline double php_get_double_from_sqldata(const ISC_SCHAR *sqldata)
{
	READ_AND_RETURN_USING_MEMCPY(double, sqldata);
}

static zend_always_inline float php_get_float_from_sqldata(const ISC_SCHAR *sqldata)
{
	READ_AND_RETURN_USING_MEMCPY(float, sqldata);
}

static zend_always_inline ISC_TIMESTAMP php_get_isc_timestamp_from_sqldata(const ISC_SCHAR *sqldata)
{
	READ_AND_RETURN_USING_MEMCPY(ISC_TIMESTAMP, sqldata);
}

static zend_always_inline ISC_QUAD php_get_isc_quad_from_sqldata(const ISC_SCHAR *sqldata)
{
	READ_AND_RETURN_USING_MEMCPY(ISC_QUAD, sqldata);
}

/* free the allocated space for passing field values to the db and back */
static void php_firebird_free_sqlda(XSQLDA const *sqlda) /* {{{ */
{
	int i;

	for (i = 0; i < sqlda->sqld; ++i) {
		XSQLVAR const *var = &sqlda->sqlvar[i];

		if (var->sqlind) {
			efree(var->sqlind);
		}
	}
}
/* }}} */

/* called by PDO to clean up a statement handle */
static int pdo_firebird_stmt_dtor(pdo_stmt_t *stmt) /* {{{ */
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	int result = 1;

	/* release the statement */
	if (isc_dsql_free_statement(S->H->isc_status, &S->stmt, DSQL_drop)) {
		php_firebird_error_stmt(stmt);
		result = 0;
	}

	zend_hash_destroy(S->named_params);
	FREE_HASHTABLE(S->named_params);

	/* clean up the input descriptor */
	if (S->in_sqlda) {
		php_firebird_free_sqlda(S->in_sqlda);
		efree(S->in_sqlda);
	}

	php_firebird_free_sqlda(&S->out_sqlda);
	efree(S);

	return result;
}
/* }}} */

/* called by PDO to execute a prepared query */
static int pdo_firebird_stmt_execute(pdo_stmt_t *stmt) /* {{{ */
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	pdo_firebird_db_handle *H = S->H;
	zend_ulong affected_rows = 0;
	static char info_count[] = {isc_info_sql_records};
	char result[64];

	do {
		/* named or open cursors should be closed first */
		if ((*S->name || S->cursor_open) && isc_dsql_free_statement(H->isc_status, &S->stmt, DSQL_close)) {
			break;
		}
		S->cursor_open = 0;

		/* allocate storage for the output data */
		if (S->out_sqlda.sqld) {
			unsigned int i;
			for (i = 0; i < S->out_sqlda.sqld; i++) {
				XSQLVAR *var = &S->out_sqlda.sqlvar[i];
				if (var->sqlind) {
					efree(var->sqlind);
				}
				var->sqlind = (void*)ecalloc(1, var->sqllen + 2 * sizeof(short));
				var->sqldata = &((char*)var->sqlind)[sizeof(short)];
			}
		}

		if (S->statement_type == isc_info_sql_stmt_exec_procedure) {
			if (isc_dsql_execute2(H->isc_status, &H->tr, &S->stmt, PDO_FB_SQLDA_VERSION, S->in_sqlda, &S->out_sqlda)) {
				break;
			}
		} else if (isc_dsql_execute(H->isc_status, &H->tr, &S->stmt, PDO_FB_SQLDA_VERSION, S->in_sqlda)) {
			break;
		}

		/* Determine how many rows have changed. In this case we are
		 * only interested in rows changed, not rows retrieved. That
		 * should be handled by the client when fetching. */
		stmt->row_count = affected_rows;

		switch (S->statement_type) {
			case isc_info_sql_stmt_insert:
			case isc_info_sql_stmt_update:
			case isc_info_sql_stmt_delete:
			case isc_info_sql_stmt_exec_procedure:
				if (isc_dsql_sql_info(H->isc_status, &S->stmt, sizeof ( info_count),
					info_count, sizeof(result), result)) {
					break;
				}
				if (result[0] == isc_info_sql_records) {
					unsigned i = 3, result_size = isc_vax_integer(&result[1], 2);
					if (result_size > sizeof(result)) {
						goto error;
					}
					while (result[i] != isc_info_end && i < result_size) {
						short len = (short) isc_vax_integer(&result[i + 1], 2);
						if (len != 1 && len != 2 && len != 4) {
							goto error;
						}
						if (result[i] != isc_info_req_select_count) {
							affected_rows += isc_vax_integer(&result[i + 3], len);
						}
						i += len + 3;
					}
					stmt->row_count = affected_rows;
				}
			/* TODO Dead code or assert one of the previous cases are hit? */
			default:
				;
		}

		/* commit? */
		if (stmt->dbh->auto_commit && isc_commit_retaining(H->isc_status, &H->tr)) {
			break;
		}

		*S->name = 0;
		S->cursor_open = S->out_sqlda.sqln && (S->statement_type != isc_info_sql_stmt_exec_procedure);
		S->exhausted = !S->out_sqlda.sqln; /* There are data to fetch */

		return 1;
	} while (0);

error:
	php_firebird_error_stmt(stmt);

	return 0;
}
/* }}} */

/* called by PDO to fetch the next row from a statement */
static int pdo_firebird_stmt_fetch(pdo_stmt_t *stmt, /* {{{ */
	enum pdo_fetch_orientation ori, zend_long offset)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	pdo_firebird_db_handle *H = S->H;

	if (!stmt->executed) {
		const char *msg = "Cannot fetch from a closed cursor";
		php_firebird_error_stmt_with_info(stmt, "HY000", strlen("HY000"), msg, strlen(msg));
	} else if (!S->exhausted) {
		if (S->statement_type == isc_info_sql_stmt_exec_procedure) {
			stmt->row_count = 1;
			S->exhausted = 1;
			return 1;
		}
		if (isc_dsql_fetch(H->isc_status, &S->stmt, PDO_FB_SQLDA_VERSION, &S->out_sqlda)) {
			if (H->isc_status[0] && H->isc_status[1]) {
				php_firebird_error_stmt(stmt);
			}
			S->exhausted = 1;
			return 0;
		}
		stmt->row_count++;
		return 1;
	}
	return 0;
}
/* }}} */

/* called by PDO to retrieve information about the fields being returned */
static int pdo_firebird_stmt_describe(pdo_stmt_t *stmt, int colno) /* {{{ */
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	struct pdo_column_data *col = &stmt->columns[colno];
	XSQLVAR *var = &S->out_sqlda.sqlvar[colno];
	int colname_len;
	char *cp;

	if ((var->sqltype & ~1) == SQL_TEXT) {
		var->sqltype = SQL_VARYING | (var->sqltype & 1);
	}
	colname_len = (S->H->fetch_table_names && var->relname_length)
					? (var->aliasname_length + var->relname_length + 1)
					: (var->aliasname_length);
	col->precision = -var->sqlscale;
	col->maxlen = var->sqllen;
	col->name = zend_string_alloc(colname_len, 0);
	cp = ZSTR_VAL(col->name);
	if (colname_len > var->aliasname_length) {
		memmove(cp, var->relname, var->relname_length);
		cp += var->relname_length;
		*cp++ = '.';
	}
	memmove(cp, var->aliasname, var->aliasname_length);
	*(cp+var->aliasname_length) = '\0';

	return 1;
}
/* }}} */

static int pdo_firebird_stmt_get_column_meta(pdo_stmt_t *stmt, zend_long colno, zval *return_value)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt *) stmt->driver_data;
	XSQLVAR *var = &S->out_sqlda.sqlvar[colno];

	enum pdo_param_type param_type;
	if (var->sqlscale < 0) {
		param_type = PDO_PARAM_STR;
	} else {
		switch (var->sqltype & ~1) {
			case SQL_SHORT:
			case SQL_LONG:
#if SIZEOF_ZEND_LONG >= 8
			case SQL_INT64:
#endif
				param_type = PDO_PARAM_INT;
				break;
#ifdef SQL_BOOLEAN
			case SQL_BOOLEAN:
				param_type = PDO_PARAM_BOOL;
				break;
#endif
			default:
				param_type = PDO_PARAM_STR;
				break;
		}
	}

	array_init(return_value);
	add_assoc_long(return_value, "pdo_type", param_type);
	return 1;
}

/* fetch a blob into a fetch buffer */
static int php_firebird_fetch_blob(pdo_stmt_t *stmt, int colno, zval *result, ISC_QUAD *blob_id)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	pdo_firebird_db_handle *H = S->H;
	isc_blob_handle blobh = PDO_FIREBIRD_HANDLE_INITIALIZER;
	char const bl_item = isc_info_blob_total_length;
	char bl_info[20];
	unsigned short i;
	int retval = 0;
	size_t len = 0;

	if (isc_open_blob(H->isc_status, &H->db, &H->tr, &blobh, blob_id)) {
		php_firebird_error_stmt(stmt);
		return 0;
	}

	if (isc_blob_info(H->isc_status, &blobh, 1, const_cast(&bl_item),
			sizeof(bl_info), bl_info)) {
		php_firebird_error_stmt(stmt);
		goto fetch_blob_end;
	}

	/* find total length of blob's data */
	for (i = 0; i < sizeof(bl_info); ) {
		unsigned short item_len;
		char item = bl_info[i++];

		if (item == isc_info_end || item == isc_info_truncated || item == isc_info_error
				|| i >= sizeof(bl_info)) {
			const char *msg = "Couldn't determine BLOB size";
			php_firebird_error_stmt_with_info(stmt, "HY000", strlen("HY000"), msg, strlen(msg));
			goto fetch_blob_end;
		}

		item_len = (unsigned short) isc_vax_integer(&bl_info[i], 2);

		if (item == isc_info_blob_total_length) {
			len = isc_vax_integer(&bl_info[i+2], item_len);
			break;
		}
		i += item_len+2;
	}

	/* we've found the blob's length, now fetch! */

	if (len) {
		zend_ulong cur_len;
		unsigned short seg_len;
		ISC_STATUS stat;
		zend_string *str;

		/* prevent overflow */
		if (len > ZSTR_MAX_LEN) {
			result = 0;
			goto fetch_blob_end;
		}

		str = zend_string_alloc(len, 0);

		for (cur_len = stat = 0; (!stat || stat == isc_segment) && cur_len < len; cur_len += seg_len) {

			unsigned short chunk_size = (len - cur_len) > USHRT_MAX ? USHRT_MAX
				: (unsigned short)(len - cur_len);

			stat = isc_get_segment(H->isc_status, &blobh, &seg_len, chunk_size, ZSTR_VAL(str) + cur_len);
		}

		ZSTR_VAL(str)[len] = '\0';
		ZVAL_STR(result, str);

		if (H->isc_status[0] == 1 && (stat != 0 && stat != isc_segstr_eof && stat != isc_segment)) {
			const char *msg = "Error reading from BLOB";
			php_firebird_error_stmt_with_info(stmt, "HY000", strlen("HY000"), msg, strlen(msg));
			goto fetch_blob_end;
		}
	}
	retval = 1;

fetch_blob_end:
	if (isc_close_blob(H->isc_status, &blobh)) {
		php_firebird_error_stmt(stmt);
		return 0;
	}
	return retval;
}
/* }}} */

static int pdo_firebird_stmt_get_col(
		pdo_stmt_t *stmt, int colno, zval *result, enum pdo_param_type *type)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	XSQLVAR const *var = &S->out_sqlda.sqlvar[colno];

	if (*var->sqlind == -1) {
		ZVAL_NULL(result);
	} else {
		if (var->sqlscale < 0) {
			static ISC_INT64 const scales[] = { 1, 10, 100, 1000,
				10000,
				100000,
				1000000,
				10000000,
				100000000,
				1000000000,
				LL_LIT(10000000000),
				LL_LIT(100000000000),
				LL_LIT(1000000000000),
				LL_LIT(10000000000000),
				LL_LIT(100000000000000),
				LL_LIT(1000000000000000),
				LL_LIT(10000000000000000),
				LL_LIT(100000000000000000),
				LL_LIT(1000000000000000000)
			};
			ISC_INT64 n, f = scales[-var->sqlscale];
			zend_string *str;

			switch (var->sqltype & ~1) {
				case SQL_SHORT:
					n = *(short*)var->sqldata;
					break;
				case SQL_LONG:
					n = php_get_isc_long_from_sqldata(var->sqldata);
					break;
				case SQL_INT64:
					n = php_get_isc_int64_from_sqldata(var->sqldata);
					break;
				case SQL_DOUBLE:
					break;
				EMPTY_SWITCH_DEFAULT_CASE()
			}

			if ((var->sqltype & ~1) == SQL_DOUBLE) {
				str = zend_strpprintf(0, "%.*F", -var->sqlscale, php_get_double_from_sqldata(var->sqldata));
			} else if (n >= 0) {
				str = zend_strpprintf(0, "%" LL_MASK "d.%0*" LL_MASK "d",
					n / f, -var->sqlscale, n % f);
			} else if (n <= -f) {
				str = zend_strpprintf(0, "%" LL_MASK "d.%0*" LL_MASK "d",
					n / f, -var->sqlscale, -n % f);
			 } else {
				str = zend_strpprintf(0, "-0.%0*" LL_MASK "d", -var->sqlscale, -n % f);
			}
			ZVAL_STR(result, str);
		} else {
			switch (var->sqltype & ~1) {
				struct tm t;
				char *fmt;

				case SQL_VARYING:
					ZVAL_STRINGL_FAST(result, &var->sqldata[2], *(short*)var->sqldata);
					break;
				case SQL_TEXT:
					ZVAL_STRINGL_FAST(result, var->sqldata, var->sqllen);
					break;
				case SQL_SHORT:
					ZVAL_LONG(result, *(short*)var->sqldata);
					break;
				case SQL_LONG:
					ZVAL_LONG(result, php_get_isc_long_from_sqldata(var->sqldata));
					break;
				case SQL_INT64:
#if SIZEOF_ZEND_LONG >= 8
					ZVAL_LONG(result, php_get_isc_int64_from_sqldata(var->sqldata));
#else
					ZVAL_STR(result, zend_strpprintf(0, "%" LL_MASK "d", php_get_isc_int64_from_sqldata(var->sqldata)));
#endif
					break;
				case SQL_FLOAT:
					/* TODO: Why is this not returned as the native type? */
					ZVAL_STR(result, zend_strpprintf(0, "%F", php_get_float_from_sqldata(var->sqldata)));
					break;
				case SQL_DOUBLE:
					/* TODO: Why is this not returned as the native type? */
					ZVAL_STR(result, zend_strpprintf(0, "%F", php_get_double_from_sqldata(var->sqldata)));
					break;
#ifdef SQL_BOOLEAN
				case SQL_BOOLEAN:
					ZVAL_BOOL(result, *(FB_BOOLEAN*)var->sqldata);
					break;
#endif
				case SQL_TYPE_DATE:
					isc_decode_sql_date((ISC_DATE*)var->sqldata, &t);
					fmt = S->H->date_format ? S->H->date_format : PDO_FB_DEF_DATE_FMT;
					if (0) {
				case SQL_TYPE_TIME:
						isc_decode_sql_time((ISC_TIME*)var->sqldata, &t);
						fmt = S->H->time_format ? S->H->time_format : PDO_FB_DEF_TIME_FMT;
					} else if (0) {
				case SQL_TIMESTAMP:
						{
							ISC_TIMESTAMP timestamp = php_get_isc_timestamp_from_sqldata(var->sqldata);
							isc_decode_timestamp(&timestamp, &t);
						}
						fmt = S->H->timestamp_format ? S->H->timestamp_format : PDO_FB_DEF_TIMESTAMP_FMT;
					}
					/* convert the timestamp into a string */
					char buf[80];
					size_t len = strftime(buf, sizeof(buf), fmt, &t);
					ZVAL_STRINGL(result, buf, len);
					break;
				case SQL_BLOB: {
					ISC_QUAD quad = php_get_isc_quad_from_sqldata(var->sqldata);
					return php_firebird_fetch_blob(stmt, colno, result, &quad);
				}
			}
		}
	}
	return 1;
}

static int php_firebird_bind_blob(pdo_stmt_t *stmt, ISC_QUAD *blob_id, zval *param)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	pdo_firebird_db_handle *H = S->H;
	isc_blob_handle h = PDO_FIREBIRD_HANDLE_INITIALIZER;
	zval data;
	zend_ulong put_cnt = 0, rem_cnt;
	unsigned short chunk_size;
	int result = 1;

	if (isc_create_blob(H->isc_status, &H->db, &H->tr, &h, blob_id)) {
		php_firebird_error_stmt(stmt);
		return 0;
	}

	if (Z_TYPE_P(param) != IS_STRING) {
		ZVAL_STR(&data, zval_get_string_func(param));
	} else {
		ZVAL_COPY_VALUE(&data, param);
	}

	for (rem_cnt = Z_STRLEN(data); rem_cnt > 0; rem_cnt -= chunk_size) {
		chunk_size = rem_cnt > USHRT_MAX ? USHRT_MAX : (unsigned short)rem_cnt;
		if (isc_put_segment(H->isc_status, &h, chunk_size, &Z_STRVAL(data)[put_cnt])) {
			php_firebird_error_stmt(stmt);
			result = 0;
			break;
		}
		put_cnt += chunk_size;
	}

	if (Z_TYPE_P(param) != IS_STRING) {
		zval_ptr_dtor_str(&data);
	}

	if (isc_close_blob(H->isc_status, &h)) {
		php_firebird_error_stmt(stmt);
		return 0;
	}
	return result;
}

static int pdo_firebird_stmt_param_hook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param, /* {{{ */
	enum pdo_param_event event_type)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	XSQLDA *sqlda = param->is_param ? S->in_sqlda : &S->out_sqlda;
	XSQLVAR *var;

	if (event_type == PDO_PARAM_EVT_FREE) { /* not used */
		return 1;
	}

	if (!sqlda || param->paramno >= sqlda->sqld) {
		const char *msg = "Invalid parameter index";
		php_firebird_error_stmt_with_info(stmt, "HY093", strlen("HY093"), msg, strlen(msg));
		return 0;
	}
	if (param->is_param && param->paramno == -1) {
		zval *index;

		/* try to determine the index by looking in the named_params hash */
		if ((index = zend_hash_find(S->named_params, param->name)) != NULL) {
			param->paramno = Z_LVAL_P(index);
		} else {
			/* ... or by looking in the input descriptor */
			int i;

			for (i = 0; i < sqlda->sqld; ++i) {
				XSQLVAR *var = &sqlda->sqlvar[i];

				if ((var->aliasname_length && !strncasecmp(ZSTR_VAL(param->name), var->aliasname,
						min(ZSTR_LEN(param->name), var->aliasname_length)))
						|| (var->sqlname_length && !strncasecmp(ZSTR_VAL(param->name), var->sqlname,
						min(ZSTR_LEN(param->name), var->sqlname_length)))) {
					param->paramno = i;
					break;
				}
			}
			if (i >= sqlda->sqld) {
				const char *msg = "Invalid parameter name";
				php_firebird_error_stmt_with_info(stmt, "HY093", strlen("HY093"), msg, strlen(msg));
				return 0;
			}
		}
	}

	var = &sqlda->sqlvar[param->paramno];

	switch (event_type) {
		zval *parameter;

		case PDO_PARAM_EVT_ALLOC:
			if (param->is_param) {
				/* allocate the parameter */
				if (var->sqlind) {
					efree(var->sqlind);
				}
				var->sqlind = (void*)emalloc(var->sqllen + 2*sizeof(short));
				var->sqldata = &((char*)var->sqlind)[sizeof(short)];
			}
			break;

		case PDO_PARAM_EVT_EXEC_PRE:
			if (!param->is_param) {
				break;
			}

			*var->sqlind = 0;
			if (Z_ISREF(param->parameter)) {
				parameter = Z_REFVAL(param->parameter);
			} else {
				parameter = &param->parameter;
			}

			if (Z_TYPE_P(parameter) == IS_RESOURCE) {
				php_stream *stm = NULL;

				php_stream_from_zval_no_verify(stm, parameter);
				if (stm) {
					zend_string *mem =  php_stream_copy_to_mem(stm, PHP_STREAM_COPY_ALL, 0);
					zval_ptr_dtor(parameter);
					ZVAL_STR(parameter, mem ? mem : ZSTR_EMPTY_ALLOC());
				} else {
					pdo_raise_impl_error(stmt->dbh, stmt, "HY105", "Expected a stream resource");
					return 0;
				}
			}

			switch (var->sqltype & ~1) {
				case SQL_ARRAY:
					{
						const char *msg = "Cannot bind to array field";
						php_firebird_error_stmt_with_info(stmt, "HY000", strlen("HY000"), msg, strlen(msg));
					}
					return 0;

				case SQL_BLOB: {
					if (Z_TYPE_P(parameter) == IS_NULL) {
						/* Check if field allow NULL values */
						if (~var->sqltype & 1) {
							const char *msg = "Parameter requires non-null value";
							php_firebird_error_stmt_with_info(stmt, "HY105", strlen("HY105"), msg, strlen(msg));
							return 0;
						}
						*var->sqlind = -1;
						return 1;
					}
					ISC_QUAD quad = php_get_isc_quad_from_sqldata(var->sqldata);
					if (php_firebird_bind_blob(stmt, &quad, parameter) != 0) {
						memcpy(var->sqldata, &quad, sizeof(quad));
						return 1;
					}
					return 0;
				}
			}

#ifdef SQL_BOOLEAN
			/* keep native BOOLEAN type */
			if ((var->sqltype & ~1) == SQL_BOOLEAN) {
				switch (Z_TYPE_P(parameter)) {
					case IS_LONG:
					case IS_DOUBLE:
					case IS_TRUE:
					case IS_FALSE:
						*(FB_BOOLEAN*)var->sqldata = zend_is_true(parameter) ? FB_TRUE : FB_FALSE;
						break;
					case IS_STRING:
						{
							zend_long lval;
							double dval;

							if (Z_STRLEN_P(parameter) == 0) {
								*(FB_BOOLEAN*)var->sqldata = FB_FALSE;
								break;
							}

							switch (is_numeric_string(Z_STRVAL_P(parameter), Z_STRLEN_P(parameter), &lval, &dval, 0)) {
								case IS_LONG:
									*(FB_BOOLEAN*)var->sqldata = (lval != 0) ? FB_TRUE : FB_FALSE;
									break;
								case IS_DOUBLE:
									*(FB_BOOLEAN*)var->sqldata = (dval != 0) ? FB_TRUE : FB_FALSE;
									break;
								default:
									if (!zend_binary_strncasecmp(Z_STRVAL_P(parameter), Z_STRLEN_P(parameter), "true", 4, 4)) {
										*(FB_BOOLEAN*)var->sqldata = FB_TRUE;
									} else if (!zend_binary_strncasecmp(Z_STRVAL_P(parameter), Z_STRLEN_P(parameter), "false", 5, 5)) {
										*(FB_BOOLEAN*)var->sqldata = FB_FALSE;
									} else {
										const char *msg = "Cannot convert string to boolean";
										php_firebird_error_stmt_with_info(stmt, "HY105", strlen("HY105"), msg, strlen(msg));
										return 0;
									}

							}
						}
						break;
					case IS_NULL:
						*var->sqlind = -1;
						break;
					default:
						{
							const char *msg = "Binding arrays/objects is not supported";
							php_firebird_error_stmt_with_info(stmt, "HY105", strlen("HY105"), msg, strlen(msg));
						}
						return 0;
				}
				break;
			}
#endif


			/* check if a NULL should be inserted */
			switch (Z_TYPE_P(parameter)) {
				int force_null;

				case IS_LONG:
					/* keep the allow-NULL flag */
					var->sqltype = (sizeof(zend_long) == 8 ? SQL_INT64 : SQL_LONG) | (var->sqltype & 1);
					var->sqldata = (void*)&Z_LVAL_P(parameter);
					var->sqllen = sizeof(zend_long);
					break;
				case IS_DOUBLE:
					/* keep the allow-NULL flag */
					var->sqltype = SQL_DOUBLE | (var->sqltype & 1);
					var->sqldata = (void*)&Z_DVAL_P(parameter);
					var->sqllen = sizeof(double);
					break;
				case IS_STRING:
					force_null = 0;

					/* for these types, an empty string can be handled like a NULL value */
					switch (var->sqltype & ~1) {
						case SQL_SHORT:
						case SQL_LONG:
						case SQL_INT64:
						case SQL_FLOAT:
						case SQL_DOUBLE:
						case SQL_TIMESTAMP:
						case SQL_TYPE_DATE:
						case SQL_TYPE_TIME:
							force_null = (Z_STRLEN_P(parameter) == 0);
					}
					if (!force_null) {
						/* keep the allow-NULL flag */
						var->sqltype = SQL_TEXT | (var->sqltype & 1);
						var->sqldata = Z_STRVAL_P(parameter);
						var->sqllen = Z_STRLEN_P(parameter);
						break;
					}
					ZEND_FALLTHROUGH;
				case IS_NULL:
					/* complain if this field doesn't allow NULL values */
					if (~var->sqltype & 1) {
						const char *msg = "Parameter requires non-null value";
						php_firebird_error_stmt_with_info(stmt, "HY105", strlen("HY105"), msg, strlen(msg));
						return 0;
					}
					*var->sqlind = -1;
					break;
				default:
					{
						const char *msg = "Binding arrays/objects is not supported";
						php_firebird_error_stmt_with_info(stmt, "HY105", strlen("HY105"), msg, strlen(msg));
					}
					return 0;
			}
			break;

		case PDO_PARAM_EVT_FETCH_POST:
			if (param->paramno == -1) {
				return 0;
			}
			if (param->is_param) {
				break;
			}
			if (Z_ISREF(param->parameter)) {
				parameter = Z_REFVAL(param->parameter);
			} else {
				parameter = &param->parameter;
			}
			zval_ptr_dtor(parameter);
			ZVAL_NULL(parameter);
			return pdo_firebird_stmt_get_col(stmt, param->paramno, parameter, NULL);
		default:
			;
	}
	return 1;
}
/* }}} */

static int pdo_firebird_stmt_set_attribute(pdo_stmt_t *stmt, zend_long attr, zval *val) /* {{{ */
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;

	switch (attr) {
		default:
			return 0;
		case PDO_ATTR_CURSOR_NAME:
			if (!try_convert_to_string(val)) {
				return 0;
			}

			if (isc_dsql_set_cursor_name(S->H->isc_status, &S->stmt, Z_STRVAL_P(val),0)) {
				php_firebird_error_stmt(stmt);
				return 0;
			}
			strlcpy(S->name, Z_STRVAL_P(val), sizeof(S->name));
			break;
	}
	return 1;
}
/* }}} */

static int pdo_firebird_stmt_get_attribute(pdo_stmt_t *stmt, zend_long attr, zval *val) /* {{{ */
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;

	switch (attr) {
		default:
			return 0;
		case PDO_ATTR_CURSOR_NAME:
			if (*S->name) {
				ZVAL_STRING(val, S->name);
			} else {
				ZVAL_NULL(val);
			}
			break;
	}
	return 1;
}
/* }}} */

static int pdo_firebird_stmt_cursor_closer(pdo_stmt_t *stmt) /* {{{ */
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;

	/* close the statement handle */
	if ((*S->name || S->cursor_open) && isc_dsql_free_statement(S->H->isc_status, &S->stmt, DSQL_close)) {
		php_firebird_error_stmt(stmt);
		return 0;
	}
	*S->name = 0;
	S->cursor_open = 0;
	return 1;
}
/* }}} */


const struct pdo_stmt_methods firebird_stmt_methods = { /* {{{ */
	pdo_firebird_stmt_dtor,
	pdo_firebird_stmt_execute,
	pdo_firebird_stmt_fetch,
	pdo_firebird_stmt_describe,
	pdo_firebird_stmt_get_col,
	pdo_firebird_stmt_param_hook,
	pdo_firebird_stmt_set_attribute,
	pdo_firebird_stmt_get_attribute,
	pdo_firebird_stmt_get_column_meta,
	NULL, /* next_rowset_func */
	pdo_firebird_stmt_cursor_closer
};
/* }}} */
