/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
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
  |         Johannes Schlueter <johannes@mysql.com>                      |
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

#ifdef PDO_USE_MYSQLND
#	define pdo_mysql_stmt_execute_prepared(stmt) pdo_mysql_stmt_execute_prepared_mysqlnd(stmt TSRMLS_CC)
#	define pdo_free_bound_result(res) zval_dtor(res.zv)
#	define pdo_mysql_stmt_close(stmt) mysqlnd_stmt_close(stmt, 0)
#else
#	define pdo_mysql_stmt_execute_prepared(stmt) pdo_mysql_stmt_execute_prepared_libmysql(stmt TSRMLS_CC)
#	define pdo_free_bound_result(res) efree(res.buffer)
#	define pdo_mysql_stmt_close(stmt) mysql_stmt_close(stmt)
#endif



static int pdo_mysql_stmt_dtor(pdo_stmt_t *stmt TSRMLS_DC) /* {{{ */
{
	pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;

	PDO_DBG_ENTER("pdo_mysql_stmt_dtor");
	PDO_DBG_INF_FMT("stmt=%p", S->stmt);
	if (S->result) {
		/* free the resource */
		mysql_free_result(S->result);
		S->result = NULL;
	}
	if (S->einfo.errmsg) {
		pefree(S->einfo.errmsg, stmt->dbh->is_persistent);
		S->einfo.errmsg = NULL;
	}
	if (S->stmt) {
		pdo_mysql_stmt_close(S->stmt);
		S->stmt = NULL;
	}

#ifndef PDO_USE_MYSQLND
	if (S->params) {
		efree(S->params);
	}
	if (S->in_null) {
		efree(S->in_null);
	}
	if (S->in_length) {
		efree(S->in_length);
	}

	if (S->bound_result)
	{
		int i;
		for (i = 0; i < stmt->column_count; i++) {
			pdo_free_bound_result(S->bound_result[i]);
		}

		efree(S->bound_result);
		efree(S->out_null);
		efree(S->out_length);
	}
#endif


	if (S->H->server) {
		while (mysql_more_results(S->H->server)) {
			MYSQL_RES *res;
			if (mysql_next_result(S->H->server) != 0) {
				break;
			}

			res = mysql_store_result(S->H->server);
			if (res) {
				mysql_free_result(res);
			}
		}
	}

#if PDO_USE_MYSQLND
	if (!S->stmt && S->current_data) {
		mnd_free(S->current_data);
	}
#endif /* PDO_USE_MYSQLND */

	efree(S);
	PDO_DBG_RETURN(1);
}
/* }}} */

static void pdo_mysql_stmt_set_row_count(pdo_stmt_t *stmt TSRMLS_DC) /* {{{ */
{
	long row_count;
	pdo_mysql_stmt *S = stmt->driver_data;
	row_count = (long) mysql_stmt_affected_rows(S->stmt);
	if (row_count != (long)-1) {
		stmt->row_count = row_count;
	}
}
/* }}} */

static int pdo_mysql_fill_stmt_from_result(pdo_stmt_t *stmt TSRMLS_DC) /* {{{ */
{
	pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;
	pdo_mysql_db_handle *H = S->H;
	my_ulonglong row_count;
	PDO_DBG_ENTER("pdo_mysql_fill_stmt_from_result");

	row_count = mysql_affected_rows(H->server);
	if (row_count == (my_ulonglong)-1) {
		/* we either have a query that returned a result set or an error occurred
		   lets see if we have access to a result set */
		if (!H->buffered) {
			S->result = mysql_use_result(H->server);
		} else {
			S->result = mysql_store_result(H->server);
		}
		if (NULL == S->result) {
			pdo_mysql_error_stmt(stmt);
			PDO_DBG_RETURN(0);
		}

		stmt->row_count = (long) mysql_num_rows(S->result);
		stmt->column_count = (int) mysql_num_fields(S->result);
		S->fields = mysql_fetch_fields(S->result);
	} else {
		/* this was a DML or DDL query (INSERT, UPDATE, DELETE, ... */
		stmt->row_count = (long) row_count;
	}

	PDO_DBG_RETURN(1);
}
/* }}} */

#ifndef PDO_USE_MYSQLND
static int pdo_mysql_stmt_execute_prepared_libmysql(pdo_stmt_t *stmt TSRMLS_DC) /* {{{ */
{
	pdo_mysql_stmt *S = stmt->driver_data;
	pdo_mysql_db_handle *H = S->H;

	PDO_DBG_ENTER("pdo_mysql_stmt_execute_prepared_libmysql");

	/* (re)bind the parameters */
	if (mysql_stmt_bind_param(S->stmt, S->params) || mysql_stmt_execute(S->stmt)) {
		if (S->params) {
			memset(S->params, 0, S->num_params * sizeof(MYSQL_BIND));
		}
		pdo_mysql_error_stmt(stmt);
		if (mysql_stmt_errno(S->stmt) == 2057) {
			/* CR_NEW_STMT_METADATA makes the statement unusable */
			S->stmt = NULL;
		}
		PDO_DBG_RETURN(0);
	}

	if (!S->result) {
		int i;

		/* figure out the result set format, if any */
		S->result = mysql_stmt_result_metadata(S->stmt);
		if (S->result) {
			int calc_max_length = H->buffered && S->max_length == 1;
			S->fields = mysql_fetch_fields(S->result);
			if (S->bound_result) {
				int i;
				for (i = 0; i < stmt->column_count; i++) {
					efree(S->bound_result[i].buffer);
				}
				efree(S->bound_result);
				efree(S->out_null);
				efree(S->out_length);
			}

			stmt->column_count = (int)mysql_num_fields(S->result);
			S->bound_result = ecalloc(stmt->column_count, sizeof(MYSQL_BIND));
			S->out_null = ecalloc(stmt->column_count, sizeof(my_bool));
			S->out_length = ecalloc(stmt->column_count, sizeof(unsigned long));

			/* summon memory to hold the row */
			for (i = 0; i < stmt->column_count; i++) {
				if (calc_max_length && S->fields[i].type == FIELD_TYPE_BLOB) {
					my_bool on = 1;
					mysql_stmt_attr_set(S->stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &on);
					calc_max_length = 0;
				}
				switch (S->fields[i].type) {
					case FIELD_TYPE_INT24:
						S->bound_result[i].buffer_length = MAX_MEDIUMINT_WIDTH + 1;
						break;
					case FIELD_TYPE_LONG:
						S->bound_result[i].buffer_length = MAX_INT_WIDTH + 1;
						break;
					case FIELD_TYPE_LONGLONG:
						S->bound_result[i].buffer_length = MAX_BIGINT_WIDTH + 1;
						break;
					case FIELD_TYPE_TINY:
						S->bound_result[i].buffer_length = MAX_TINYINT_WIDTH + 1;
						break;
					case FIELD_TYPE_SHORT:
						S->bound_result[i].buffer_length = MAX_SMALLINT_WIDTH + 1;
						break;
					default:
						S->bound_result[i].buffer_length =
							S->fields[i].max_length? S->fields[i].max_length:
							S->fields[i].length;
						/* work-around for longtext and alike */
						if (S->bound_result[i].buffer_length > H->max_buffer_size) {
							S->bound_result[i].buffer_length = H->max_buffer_size;
						}
				}

				/* there are cases where the length reported by mysql is too short.
				 * eg: when describing a table that contains an enum column. Since
				 * we have no way of knowing the true length either, we'll bump up
				 * our buffer size to a reasonable size, just in case */
				if (S->fields[i].max_length == 0 && S->bound_result[i].buffer_length < 128 && MYSQL_TYPE_VAR_STRING) {
					S->bound_result[i].buffer_length = 128;
				}

				S->out_length[i] = 0;

				S->bound_result[i].buffer = emalloc(S->bound_result[i].buffer_length);
				S->bound_result[i].is_null = &S->out_null[i];
				S->bound_result[i].length = &S->out_length[i];
				S->bound_result[i].buffer_type = MYSQL_TYPE_STRING;
			}

			if (mysql_stmt_bind_result(S->stmt, S->bound_result)) {
				pdo_mysql_error_stmt(stmt);
				PDO_DBG_RETURN(0);
			}

			/* if buffered, pre-fetch all the data */
			if (H->buffered) {
				mysql_stmt_store_result(S->stmt);
			}
		}
	}

	pdo_mysql_stmt_set_row_count(stmt TSRMLS_CC);
	PDO_DBG_RETURN(1);
}
/* }}} */
#endif

#ifdef PDO_USE_MYSQLND
static int pdo_mysql_stmt_execute_prepared_mysqlnd(pdo_stmt_t *stmt TSRMLS_DC) /* {{{ */
{
	pdo_mysql_stmt *S = stmt->driver_data;
	pdo_mysql_db_handle *H = S->H;
	int i;

	PDO_DBG_ENTER("pdo_mysql_stmt_execute_prepared_mysqlnd");

	if (mysql_stmt_execute(S->stmt)) {
		pdo_mysql_error_stmt(stmt);
		PDO_DBG_RETURN(0);
	}

	if (S->result) {
		/* TODO: add a test to check if we really have zvals here... */
		mysql_free_result(S->result);
		S->result = NULL;
	}

	/* for SHOW/DESCRIBE and others the column/field count is not available before execute */
	stmt->column_count = mysql_stmt_field_count(S->stmt);
	for (i = 0; i < stmt->column_count; i++) {
		mysqlnd_stmt_bind_one_result(S->stmt, i);
	}

	S->result = mysqlnd_stmt_result_metadata(S->stmt);
	if (S->result) {
		S->fields = mysql_fetch_fields(S->result);
		/* if buffered, pre-fetch all the data */
		if (H->buffered) {
			if (mysql_stmt_store_result(S->stmt)) {
				PDO_DBG_RETURN(0);
			}
		}
	}

	pdo_mysql_stmt_set_row_count(stmt TSRMLS_CC);
	PDO_DBG_RETURN(1);
}
/* }}} */
#endif

static int pdo_mysql_stmt_execute(pdo_stmt_t *stmt TSRMLS_DC) /* {{{ */
{
	pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;
	pdo_mysql_db_handle *H = S->H;
	PDO_DBG_ENTER("pdo_mysql_stmt_execute");
	PDO_DBG_INF_FMT("stmt=%p", S->stmt);

	if (S->stmt) {
		PDO_DBG_RETURN(pdo_mysql_stmt_execute_prepared(stmt));
	}

	/* ensure that we free any previous unfetched results */
	if (S->result) {
		mysql_free_result(S->result);
		S->result = NULL;
	}

	if (mysql_real_query(H->server, stmt->active_query_string, stmt->active_query_stringlen) != 0) {
		pdo_mysql_error_stmt(stmt);
		PDO_DBG_RETURN(0);
	}

	PDO_DBG_RETURN(pdo_mysql_fill_stmt_from_result(stmt TSRMLS_CC));
}
/* }}} */

static int pdo_mysql_stmt_next_rowset(pdo_stmt_t *stmt TSRMLS_DC) /* {{{ */
{
	pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;
	pdo_mysql_db_handle *H = S->H;
	long row_count;
	PDO_DBG_ENTER("pdo_mysql_stmt_next_rowset");
	PDO_DBG_INF_FMT("stmt=%p", S->stmt);

#if PDO_USE_MYSQLND
	if (!H->emulate_prepare) {
		if (!mysqlnd_stmt_more_results(S->stmt)) {
			PDO_DBG_RETURN(0);
		}
		if (mysqlnd_stmt_next_result(S->stmt)) {
			PDO_DBG_RETURN(0);
		}

		if (!mysqlnd_stmt_more_results(S->stmt)) {
			/*
			MySQL gives us n + 1 result sets for
			CALL proc() and n result sets returned by the proc itself.
			Result set n + 1 is about the procedure call itself.
			As the PDO emulation does not return it, we skip it as well
			*/
			PDO_DBG_RETURN(0);
		}

		/* TODO - this code is stolen from execute() - see above */
		if (S->result) {
			mysql_free_result(S->result);
			S->result = NULL;
		}
		{
			/* for SHOW/DESCRIBE and others the column/field count is not available before execute */
			int i;

			stmt->column_count = mysql_stmt_field_count(S->stmt);
			for (i = 0; i < stmt->column_count; i++) {
				mysqlnd_stmt_bind_one_result(S->stmt, i);
			}
		}

		S->result = mysqlnd_stmt_result_metadata(S->stmt);
		if (S->result) {
			S->fields = mysql_fetch_fields(S->result);

			/* if buffered, pre-fetch all the data */
			if (H->buffered) {
				if (mysql_stmt_store_result(S->stmt)) {
					PDO_DBG_RETURN(1);
				}
			}
		}
		row_count = (long) mysql_stmt_affected_rows(S->stmt);
		if (row_count != (long)-1) {
			stmt->row_count = row_count;
		}
		PDO_DBG_RETURN(1);
	}
#endif

/* ensure that we free any previous unfetched results */
#ifndef PDO_USE_MYSQLND
	if (S->stmt) {
		stmt->column_count = (int)mysql_num_fields(S->result);
		mysql_stmt_free_result(S->stmt);
	}
#endif
	if (S->result) {
		mysql_free_result(S->result);
		S->result = NULL;
	}

	if (!mysql_more_results(H->server)) {
		/* No more results */
		PDO_DBG_RETURN(0);	
	}
#if PDO_USE_MYSQLND
	if (mysql_next_result(H->server) == FAIL) {
		pdo_mysql_error_stmt(stmt);
		PDO_DBG_RETURN(0);
	} else {
		PDO_DBG_RETURN(pdo_mysql_fill_stmt_from_result(stmt TSRMLS_CC));
	}
#else
	if (mysql_next_result(H->server) > 0) {
		pdo_mysql_error_stmt(stmt);
		PDO_DBG_RETURN(0);
	} else {
		PDO_DBG_RETURN(pdo_mysql_fill_stmt_from_result(stmt TSRMLS_CC));
	}
#endif
}
/* }}} */


static const char * const pdo_param_event_names[] =
{
	"PDO_PARAM_EVT_ALLOC",
	"PDO_PARAM_EVT_FREE",
	"PDO_PARAM_EVT_EXEC_PRE",
	"PDO_PARAM_EVT_EXEC_POST",
	"PDO_PARAM_EVT_FETCH_PRE",
	"PDO_PARAM_EVT_FETCH_POST",
	"PDO_PARAM_EVT_NORMALIZE",
};


static int pdo_mysql_stmt_param_hook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param, enum pdo_param_event event_type TSRMLS_DC) /* {{{ */
{
#ifndef PDO_USE_MYSQLND
	PDO_MYSQL_PARAM_BIND *b;
#endif
	pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;

	PDO_DBG_ENTER("pdo_mysql_stmt_param_hook");
	PDO_DBG_INF_FMT("stmt=%p", S->stmt);
	PDO_DBG_INF_FMT("event = %s", pdo_param_event_names[event_type]);
	if (S->stmt && param->is_param) {
		switch (event_type) {
			case PDO_PARAM_EVT_ALLOC:
				/* sanity check parameter number range */
				if (param->paramno < 0 || param->paramno >= S->num_params) {
					strcpy(stmt->error_code, "HY093");
					PDO_DBG_RETURN(0);
				}
				S->params_given++;

#ifndef PDO_USE_MYSQLND
				b = &S->params[param->paramno];
				param->driver_data = b;
				b->is_null = &S->in_null[param->paramno];
				b->length = &S->in_length[param->paramno];
				/* recall how many parameters have been provided */
#endif
				PDO_DBG_RETURN(1);

			case PDO_PARAM_EVT_EXEC_PRE:
				if (S->params_given < (unsigned int) S->num_params) {
					/* too few parameter bound */
					PDO_DBG_ERR("too few parameters bound");
					strcpy(stmt->error_code, "HY093");
					PDO_DBG_RETURN(0);
				}

#if PDO_USE_MYSQLND
				if (PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_NULL ||
						Z_TYPE_P(param->parameter) == IS_NULL) {
					mysqlnd_stmt_bind_one_param(S->stmt, param->paramno, param->parameter, MYSQL_TYPE_NULL);
					PDO_DBG_RETURN(1);
				}
#else
				b = (PDO_MYSQL_PARAM_BIND*)param->driver_data;
				*b->is_null = 0;
				if (PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_NULL ||
						Z_TYPE_P(param->parameter) == IS_NULL) {
					*b->is_null = 1;
					b->buffer_type = MYSQL_TYPE_STRING;
					b->buffer = NULL;
					b->buffer_length = 0;
					*b->length = 0;
					PDO_DBG_RETURN(1);
				}
#endif /* PDO_USE_MYSQLND */

				switch (PDO_PARAM_TYPE(param->param_type)) {
					case PDO_PARAM_STMT:
						PDO_DBG_RETURN(0);
					case PDO_PARAM_LOB:
						PDO_DBG_INF("PDO_PARAM_LOB");
						if (Z_TYPE_P(param->parameter) == IS_RESOURCE) {
							php_stream *stm;
							php_stream_from_zval_no_verify(stm, &param->parameter);
							if (stm) {
								SEPARATE_ZVAL_IF_NOT_REF(&param->parameter);
								Z_TYPE_P(param->parameter) = IS_STRING;
								Z_STRLEN_P(param->parameter) = php_stream_copy_to_mem(stm,
									&Z_STRVAL_P(param->parameter), PHP_STREAM_COPY_ALL, 0);
							} else {
								pdo_raise_impl_error(stmt->dbh, stmt, "HY105", "Expected a stream resource" TSRMLS_CC);
								return 0;
							}
						}
						/* fall through */

					default:
						;
				}

#if PDO_USE_MYSQLND
				/* Is it really correct to check the zval's type? - But well, that's what the old code below does, too */
				PDO_DBG_INF_FMT("param->parameter->type=%d", Z_TYPE_P(param->parameter));
				switch (Z_TYPE_P(param->parameter)) {
					case IS_STRING:
						mysqlnd_stmt_bind_one_param(S->stmt, param->paramno, param->parameter, MYSQL_TYPE_VAR_STRING);
						break;
					case IS_LONG:
#if SIZEOF_LONG==8
						mysqlnd_stmt_bind_one_param(S->stmt, param->paramno, param->parameter, MYSQL_TYPE_LONGLONG);
#elif SIZEOF_LONG==4
						mysqlnd_stmt_bind_one_param(S->stmt, param->paramno, param->parameter, MYSQL_TYPE_LONG);
#endif /* SIZEOF_LONG */
						break;
					case IS_DOUBLE:
						mysqlnd_stmt_bind_one_param(S->stmt, param->paramno, param->parameter, MYSQL_TYPE_DOUBLE);
						break;
					default:
						PDO_DBG_RETURN(0);
				}

				PDO_DBG_RETURN(1);
#else
				PDO_DBG_INF_FMT("param->parameter->type=%d", Z_TYPE_P(param->parameter));
				switch (Z_TYPE_P(param->parameter)) {
					case IS_STRING:
						b->buffer_type = MYSQL_TYPE_STRING;
						b->buffer = Z_STRVAL_P(param->parameter);
						b->buffer_length = Z_STRLEN_P(param->parameter);
						*b->length = Z_STRLEN_P(param->parameter);
						PDO_DBG_RETURN(1);

					case IS_LONG:
						b->buffer_type = MYSQL_TYPE_LONG;
						b->buffer = &Z_LVAL_P(param->parameter);
						PDO_DBG_RETURN(1);

					case IS_DOUBLE:
						b->buffer_type = MYSQL_TYPE_DOUBLE;
						b->buffer = &Z_DVAL_P(param->parameter);
						PDO_DBG_RETURN(1);

					default:
						PDO_DBG_RETURN(0);
				}
#endif /* PDO_USE_MYSQLND */
		case PDO_PARAM_EVT_FREE:
		case PDO_PARAM_EVT_EXEC_POST:
		case PDO_PARAM_EVT_FETCH_PRE:
		case PDO_PARAM_EVT_FETCH_POST:
		case PDO_PARAM_EVT_NORMALIZE:
			/* do nothing */
			break;
		}
	}

	PDO_DBG_RETURN(1);
}
/* }}} */

static int pdo_mysql_stmt_fetch(pdo_stmt_t *stmt, enum pdo_fetch_orientation ori, long offset TSRMLS_DC) /* {{{ */
{
	pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;
#if PDO_USE_MYSQLND
	zend_bool fetched_anything;

	PDO_DBG_ENTER("pdo_mysql_stmt_fetch");
	PDO_DBG_INF_FMT("stmt=%p", S->stmt);
	if (S->stmt) {
		if (FAIL == mysqlnd_stmt_fetch(S->stmt, &fetched_anything) || fetched_anything == FALSE) {
			PDO_DBG_RETURN(0);
		}

		PDO_DBG_RETURN(1);
	}
#else
	int ret;

	if (S->stmt) {
		ret = mysql_stmt_fetch(S->stmt);

#		ifdef MYSQL_DATA_TRUNCATED
		if (ret == MYSQL_DATA_TRUNCATED) {
			ret = 0;
		}
#		endif

		if (ret) {
			if (ret != MYSQL_NO_DATA) {
				pdo_mysql_error_stmt(stmt);
			}
			PDO_DBG_RETURN(0);
		}

		PDO_DBG_RETURN(1);
	}
#endif /* PDO_USE_MYSQLND */

	if (!S->result) {
		strcpy(stmt->error_code, "HY000");
		PDO_DBG_RETURN(0);
	}
#if PDO_USE_MYSQLND
	if (!S->stmt && S->current_data) {
		mnd_free(S->current_data);
	}
#endif /* PDO_USE_MYSQLND */

	if ((S->current_data = mysql_fetch_row(S->result)) == NULL) {
#if PDO_USE_MYSQLND
		if (S->result->unbuf && !S->result->unbuf->eof_reached && mysql_errno(S->H->server)) {
#else
		if (!S->result->eof && mysql_errno(S->H->server)) {
#endif
			pdo_mysql_error_stmt(stmt);
		}
		PDO_DBG_RETURN(0);
	}

	S->current_lengths = mysql_fetch_lengths(S->result);
	PDO_DBG_RETURN(1);
}
/* }}} */

static int pdo_mysql_stmt_describe(pdo_stmt_t *stmt, int colno TSRMLS_DC) /* {{{ */
{
	pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;
	struct pdo_column_data *cols = stmt->columns;
	int i;

	PDO_DBG_ENTER("pdo_mysql_stmt_describe");
	PDO_DBG_INF_FMT("stmt=%p", S->stmt);
	if (!S->result) {
		PDO_DBG_RETURN(0);
	}

	if (colno >= stmt->column_count) {
		/* error invalid column */
		PDO_DBG_RETURN(0);
	}

	/* fetch all on demand, this seems easiest
	** if we've been here before bail out
	*/
	if (cols[0].name) {
		PDO_DBG_RETURN(1);
	}
	for (i = 0; i < stmt->column_count; i++) {
		int namelen;

		if (S->H->fetch_table_names) {
			namelen = spprintf(&cols[i].name, 0, "%s.%s", S->fields[i].table, S->fields[i].name);
			cols[i].namelen = namelen;
		} else {
			namelen = strlen(S->fields[i].name);
			cols[i].namelen = namelen;
			cols[i].name = estrndup(S->fields[i].name, namelen);
		}

		cols[i].precision = S->fields[i].decimals;
		cols[i].maxlen = S->fields[i].length;

#ifdef PDO_USE_MYSQLND
		if (S->stmt) {
			cols[i].param_type = PDO_PARAM_ZVAL;
		} else
#endif
		{
			cols[i].param_type = PDO_PARAM_STR;
		}
	}
	PDO_DBG_RETURN(1);
}
/* }}} */

static int pdo_mysql_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr, unsigned long *len, int *caller_frees TSRMLS_DC) /* {{{ */
{
	pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;

	PDO_DBG_ENTER("pdo_mysql_stmt_get_col");
	PDO_DBG_INF_FMT("stmt=%p", S->stmt);
	if (!S->result) {
		PDO_DBG_RETURN(0);
	}

	/* With mysqlnd data is stored inside mysqlnd, not S->current_data */
	if (!S->stmt) {
		if (S->current_data == NULL || !S->result) {
			PDO_DBG_RETURN(0);
		}
	}

	if (colno >= stmt->column_count) {
		/* error invalid column */
		PDO_DBG_RETURN(0);
	}
#if PDO_USE_MYSQLND
	if (S->stmt) {
		Z_ADDREF_P(S->stmt->data->result_bind[colno].zv);
		*ptr = (char*)&S->stmt->data->result_bind[colno].zv;
		*len = sizeof(zval);
		PDO_DBG_RETURN(1);
	}
#else
	if (S->stmt) {
		if (S->out_null[colno]) {
			*ptr = NULL;
			*len = 0;
			PDO_DBG_RETURN(1);
		}
		*ptr = S->bound_result[colno].buffer;
		if (S->out_length[colno] > S->bound_result[colno].buffer_length) {
			/* mysql lied about the column width */
			strcpy(stmt->error_code, "01004"); /* truncated */
			S->out_length[colno] = S->bound_result[colno].buffer_length;
			*len = S->out_length[colno];
			PDO_DBG_RETURN(0);
		}
		*len = S->out_length[colno];
		PDO_DBG_RETURN(1);
	}
#endif
	*ptr = S->current_data[colno];
	*len = S->current_lengths[colno];
	PDO_DBG_RETURN(1);
} /* }}} */

static char *type_to_name_native(int type) /* {{{ */
{
#define PDO_MYSQL_NATIVE_TYPE_NAME(x)	case FIELD_TYPE_##x: return #x;

    switch (type) {
        PDO_MYSQL_NATIVE_TYPE_NAME(STRING)
        PDO_MYSQL_NATIVE_TYPE_NAME(VAR_STRING)
#ifdef FIELD_TYPE_TINY
        PDO_MYSQL_NATIVE_TYPE_NAME(TINY)
#endif
#ifdef FIELD_TYPE_BIT
        PDO_MYSQL_NATIVE_TYPE_NAME(BIT)
#endif
        PDO_MYSQL_NATIVE_TYPE_NAME(SHORT)
        PDO_MYSQL_NATIVE_TYPE_NAME(LONG)
        PDO_MYSQL_NATIVE_TYPE_NAME(LONGLONG)
        PDO_MYSQL_NATIVE_TYPE_NAME(INT24)
        PDO_MYSQL_NATIVE_TYPE_NAME(FLOAT)
        PDO_MYSQL_NATIVE_TYPE_NAME(DOUBLE)
        PDO_MYSQL_NATIVE_TYPE_NAME(DECIMAL)
#ifdef FIELD_TYPE_NEWDECIMAL
        PDO_MYSQL_NATIVE_TYPE_NAME(NEWDECIMAL)
#endif
#ifdef FIELD_TYPE_GEOMETRY
        PDO_MYSQL_NATIVE_TYPE_NAME(GEOMETRY)
#endif
        PDO_MYSQL_NATIVE_TYPE_NAME(TIMESTAMP)
#ifdef FIELD_TYPE_YEAR
        PDO_MYSQL_NATIVE_TYPE_NAME(YEAR)
#endif
        PDO_MYSQL_NATIVE_TYPE_NAME(SET)
        PDO_MYSQL_NATIVE_TYPE_NAME(ENUM)
        PDO_MYSQL_NATIVE_TYPE_NAME(DATE)
#ifdef FIELD_TYPE_NEWDATE
        PDO_MYSQL_NATIVE_TYPE_NAME(NEWDATE)
#endif
        PDO_MYSQL_NATIVE_TYPE_NAME(TIME)
        PDO_MYSQL_NATIVE_TYPE_NAME(DATETIME)
        PDO_MYSQL_NATIVE_TYPE_NAME(TINY_BLOB)
        PDO_MYSQL_NATIVE_TYPE_NAME(MEDIUM_BLOB)
        PDO_MYSQL_NATIVE_TYPE_NAME(LONG_BLOB)
        PDO_MYSQL_NATIVE_TYPE_NAME(BLOB)
        PDO_MYSQL_NATIVE_TYPE_NAME(NULL)
        default:
            return NULL;
    }
#undef PDO_MYSQL_NATIVE_TYPE_NAME
} /* }}} */

static int pdo_mysql_stmt_col_meta(pdo_stmt_t *stmt, long colno, zval *return_value TSRMLS_DC) /* {{{ */
{
	pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;
	const MYSQL_FIELD *F;
	zval *flags;
	char *str;

	PDO_DBG_ENTER("pdo_mysql_stmt_col_meta");
	PDO_DBG_INF_FMT("stmt=%p", S->stmt);
	if (!S->result) {
		PDO_DBG_RETURN(FAILURE);
	}
	if (colno >= stmt->column_count) {
		/* error invalid column */
		PDO_DBG_RETURN(FAILURE);
	}

	array_init(return_value);
	MAKE_STD_ZVAL(flags);
	array_init(flags);

	F = S->fields + colno;

	if (F->def) {
		add_assoc_string(return_value, "mysql:def", F->def, 1);
	}
	if (IS_NOT_NULL(F->flags)) {
		add_next_index_string(flags, "not_null", 1);
	}
	if (IS_PRI_KEY(F->flags)) {
		add_next_index_string(flags, "primary_key", 1);
	}
	if (F->flags & MULTIPLE_KEY_FLAG) {
		add_next_index_string(flags, "multiple_key", 1);
	}
	if (F->flags & UNIQUE_KEY_FLAG) {
		add_next_index_string(flags, "unique_key", 1);
	}
	if (IS_BLOB(F->flags)) {
		add_next_index_string(flags, "blob", 1);
	}
	str = type_to_name_native(F->type);
	if (str) {
		add_assoc_string(return_value, "native_type", str, 1);
	}

#ifdef PDO_USE_MYSQLND
	switch (F->type) {
		case MYSQL_TYPE_BIT:
		case MYSQL_TYPE_YEAR:
		case MYSQL_TYPE_TINY:
		case MYSQL_TYPE_SHORT:
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONG:
#if SIZEOF_LONG==8
		case MYSQL_TYPE_LONGLONG:
#endif
			add_assoc_long(return_value, "pdo_type", PDO_PARAM_INT);
			break;
		default:
			add_assoc_long(return_value, "pdo_type", PDO_PARAM_STR);
			break;
	}
#endif

	add_assoc_zval(return_value, "flags", flags);
	add_assoc_string(return_value, "table", (char *) (F->table?F->table:""), 1);
	PDO_DBG_RETURN(SUCCESS);
} /* }}} */

static int pdo_mysql_stmt_cursor_closer(pdo_stmt_t *stmt TSRMLS_DC) /* {{{ */
{
	pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;

	PDO_DBG_ENTER("pdo_mysql_stmt_cursor_closer");
	PDO_DBG_INF_FMT("stmt=%p", S->stmt);
	if (S->result) {
		mysql_free_result(S->result);
		S->result = NULL;
	}
	if (S->stmt) {
		int retval;
		retval = mysql_stmt_free_result(S->stmt);
		PDO_DBG_RETURN(retval ? 0 : 1);
	}

	while (mysql_more_results(S->H->server)) {
		MYSQL_RES *res;
		if (mysql_next_result(S->H->server) != 0) {
			break;
		}
		res = mysql_store_result(S->H->server);
		if (res) {
			mysql_free_result(res);
		}
	}
	PDO_DBG_RETURN(1);
}
/* }}} */

struct pdo_stmt_methods mysql_stmt_methods = {
	pdo_mysql_stmt_dtor,
	pdo_mysql_stmt_execute,
	pdo_mysql_stmt_fetch,
	pdo_mysql_stmt_describe,
	pdo_mysql_stmt_get_col,
	pdo_mysql_stmt_param_hook,
	NULL, /* set_attr */
	NULL, /* get_attr */
	pdo_mysql_stmt_col_meta,
	pdo_mysql_stmt_next_rowset,
	pdo_mysql_stmt_cursor_closer
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
