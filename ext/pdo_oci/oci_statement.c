/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
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
#include "php_pdo_oci.h"
#include "php_pdo_oci_int.h"
#include "Zend/zend_extensions.h"

#define PDO_OCI_LOBMAXSIZE (4294967295UL) /* OCI_LOBMAXSIZE */

#define STMT_CALL(name, params)											\
	do {																\
		S->last_err = name params;										\
		S->last_err = _oci_error(S->err, stmt->dbh, stmt, #name, S->last_err, FALSE, __FILE__, __LINE__); \
		if (S->last_err) {												\
			return 0;													\
		}																\
	} while(0)

#define STMT_CALL_MSG(name, msg, params)								\
	do { 																\
		S->last_err = name params;										\
		S->last_err = _oci_error(S->err, stmt->dbh, stmt, #name ": " #msg, S->last_err, FALSE, __FILE__, __LINE__); \
		if (S->last_err) {												\
			return 0;													\
		}																\
	} while(0)

static php_stream *oci_create_lob_stream(zval *dbh, pdo_stmt_t *stmt, OCILobLocator *lob);

#define OCI_TEMPLOB_CLOSE(envhp, svchp, errhp, lob)				\
	do															\
	{															\
		boolean isTempLOB;										\
		OCILobIsTemporary(envhp, errhp, lob, &isTempLOB);		\
		if (isTempLOB)											\
			OCILobFreeTemporary(svchp, errhp, lob);				\
	} while(0)

static int oci_stmt_dtor(pdo_stmt_t *stmt) /* {{{ */
{
	pdo_oci_stmt *S = (pdo_oci_stmt*)stmt->driver_data;
	HashTable *BC = stmt->bound_columns;
	HashTable *BP = stmt->bound_params;

	int i;

	if (S->stmt) {
		/* cancel server side resources for the statement if we didn't
		 * fetch it all */
		OCIStmtFetch(S->stmt, S->err, 0, OCI_FETCH_NEXT, OCI_DEFAULT);

		/* free the handle */
		OCIHandleFree(S->stmt, OCI_HTYPE_STMT);
		S->stmt = NULL;
	}
	if (S->err) {
		OCIHandleFree(S->err, OCI_HTYPE_ERROR);
		S->err = NULL;
	}

	/* need to ensure these go away now */
	if (BC) {
		zend_hash_destroy(BC);
		FREE_HASHTABLE(stmt->bound_columns);
		stmt->bound_columns = NULL;
	}

	if (BP) {
		zend_hash_destroy(BP);
		FREE_HASHTABLE(stmt->bound_params);
		stmt->bound_params = NULL;
	}

	if (S->einfo.errmsg) {
		pefree(S->einfo.errmsg, stmt->dbh->is_persistent);
		S->einfo.errmsg = NULL;
	}

	if (S->cols) {
		for (i = 0; i < stmt->column_count; i++) {
			if (S->cols[i].data) {
				switch (S->cols[i].dtype) {
					case SQLT_BLOB:
					case SQLT_CLOB:
						OCI_TEMPLOB_CLOSE(S->H->env, S->H->svc, S->H->err,
							(OCILobLocator *) S->cols[i].data);
						OCIDescriptorFree(S->cols[i].data, OCI_DTYPE_LOB);
						break;
					default:
						efree(S->cols[i].data);
				}
			}
		}
		efree(S->cols);
		S->cols = NULL;
	}
	efree(S);

	stmt->driver_data = NULL;

	return 1;
} /* }}} */

static int oci_stmt_execute(pdo_stmt_t *stmt) /* {{{ */
{
	pdo_oci_stmt *S = (pdo_oci_stmt*)stmt->driver_data;
	ub4 rowcount;
	b4 mode;

	if (!S->stmt_type) {
		STMT_CALL_MSG(OCIAttrGet, "OCI_ATTR_STMT_TYPE",
				(S->stmt, OCI_HTYPE_STMT, &S->stmt_type, 0, OCI_ATTR_STMT_TYPE, S->err));
	}

	if (stmt->executed) {
		/* ensure that we cancel the cursor from a previous fetch */
		OCIStmtFetch(S->stmt, S->err, 0, OCI_FETCH_NEXT, OCI_DEFAULT);
	}

#ifdef OCI_STMT_SCROLLABLE_READONLY /* needed for oci8 ? */
	if (S->exec_type == OCI_STMT_SCROLLABLE_READONLY) {
		mode = OCI_STMT_SCROLLABLE_READONLY;
	} else
#endif
	if (stmt->dbh->auto_commit && !stmt->dbh->in_txn) {
		mode = OCI_COMMIT_ON_SUCCESS;
	} else {
		mode = OCI_DEFAULT;
	}

	STMT_CALL(OCIStmtExecute, (S->H->svc, S->stmt, S->err,
				(S->stmt_type == OCI_STMT_SELECT && !S->have_blobs) ? 0 : 1, 0, NULL, NULL,
				mode));

	if (!stmt->executed) {
		ub4 colcount;
		/* do first-time-only definition of bind/mapping stuff */

		/* how many columns do we have ? */
		STMT_CALL_MSG(OCIAttrGet, "ATTR_PARAM_COUNT",
				(S->stmt, OCI_HTYPE_STMT, &colcount, 0, OCI_ATTR_PARAM_COUNT, S->err));

		stmt->column_count = (int)colcount;

		if (S->cols) {
			int i;
			for (i = 0; i < stmt->column_count; i++) {
				if (S->cols[i].data) {
					switch (S->cols[i].dtype) {
						case SQLT_BLOB:
						case SQLT_CLOB:
							/* do nothing */
							break;
						default:
							efree(S->cols[i].data);
					}
				}
			}
			efree(S->cols);
		}

		S->cols = ecalloc(colcount, sizeof(pdo_oci_column));
	}

	STMT_CALL_MSG(OCIAttrGet, "ATTR_ROW_COUNT",
			(S->stmt, OCI_HTYPE_STMT, &rowcount, 0, OCI_ATTR_ROW_COUNT, S->err));
	stmt->row_count = (long)rowcount;

	return 1;
} /* }}} */

static sb4 oci_bind_input_cb(dvoid *ctx, OCIBind *bindp, ub4 iter, ub4 index, dvoid **bufpp, ub4 *alenp, ub1 *piecep, dvoid **indpp) /* {{{ */
{
	struct pdo_bound_param_data *param = (struct pdo_bound_param_data*)ctx;
	pdo_oci_bound_param *P = (pdo_oci_bound_param*)param->driver_data;
	zval *parameter;

	if (!param) {
		php_error_docref(NULL, E_WARNING, "param is NULL in oci_bind_input_cb; this should not happen");
		return OCI_ERROR;
	}

	*indpp = &P->indicator;

    if (Z_ISREF(param->parameter))
		parameter = Z_REFVAL(param->parameter);
	else
		parameter = &param->parameter;

	if (P->thing) {
		*bufpp = P->thing;
		*alenp = sizeof(void*);
	} else if (ZVAL_IS_NULL(parameter)) {
		/* insert a NULL value into the column */
		P->indicator = -1; /* NULL */
		*bufpp = 0;
		*alenp = -1;
	} else if (!P->thing) {
		/* regular string bind */
		convert_to_string(parameter);
		*bufpp = Z_STRVAL_P(parameter);
		*alenp = (ub4) Z_STRLEN_P(parameter);
	}

	*piecep = OCI_ONE_PIECE;
	return OCI_CONTINUE;
} /* }}} */

static sb4 oci_bind_output_cb(dvoid *ctx, OCIBind *bindp, ub4 iter, ub4 index, dvoid **bufpp, ub4 **alenpp, ub1 *piecep, dvoid **indpp, ub2 **rcodepp) /* {{{ */
{
	struct pdo_bound_param_data *param = (struct pdo_bound_param_data*)ctx;
	pdo_oci_bound_param *P = (pdo_oci_bound_param*)param->driver_data;
	zval *parameter;

	if (!param) {
		php_error_docref(NULL, E_WARNING, "param is NULL in oci_bind_output_cb; this should not happen");
		return OCI_ERROR;
	}

	if (Z_ISREF(param->parameter))
        parameter = Z_REFVAL(param->parameter);
    else
        parameter = &param->parameter;

	if (PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_LOB) {
		P->actual_len = sizeof(OCILobLocator*);
		*bufpp = P->thing;
		*alenpp = &P->actual_len;
		*piecep = OCI_ONE_PIECE;
		*rcodepp = &P->retcode;
		*indpp = &P->indicator;
		return OCI_CONTINUE;
	}

	if (Z_TYPE_P(parameter) == IS_OBJECT || Z_TYPE_P(parameter) == IS_RESOURCE) {
		return OCI_CONTINUE;
	}

	convert_to_string(parameter);
	zval_dtor(parameter);

	Z_STR_P(parameter) = zend_string_alloc(param->max_value_len, 1);
	P->used_for_output = 1;

	P->actual_len = (ub4) Z_STRLEN_P(parameter);
	*alenpp = &P->actual_len;
	*bufpp = (Z_STR_P(parameter))->val;
	*piecep = OCI_ONE_PIECE;
	*rcodepp = &P->retcode;
	*indpp = &P->indicator;

	return OCI_CONTINUE;
} /* }}} */

static int oci_stmt_param_hook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param, enum pdo_param_event event_type) /* {{{ */
{
	pdo_oci_stmt *S = (pdo_oci_stmt*)stmt->driver_data;

	/* we're only interested in parameters for prepared SQL right now */
	if (param->is_param) {
		pdo_oci_bound_param *P;
		sb4 value_sz = -1;
		zval *parameter;

		if (Z_ISREF(param->parameter))
			parameter = Z_REFVAL(param->parameter);
		else
			parameter = &param->parameter;

		P = (pdo_oci_bound_param*)param->driver_data;

		switch (event_type) {
			case PDO_PARAM_EVT_FETCH_PRE:
			case PDO_PARAM_EVT_FETCH_POST:
			case PDO_PARAM_EVT_NORMALIZE:
				/* Do nothing */
				break;

			case PDO_PARAM_EVT_FREE:
				P = param->driver_data;
				if (P && P->thing) {
					OCI_TEMPLOB_CLOSE(S->H->env, S->H->svc, S->H->err, P->thing);
					OCIDescriptorFree(P->thing, OCI_DTYPE_LOB);
					P->thing = NULL;
					efree(P);
				}
				else if (P) {
					efree(P);
				}
				break;

			case PDO_PARAM_EVT_ALLOC:
				P = (pdo_oci_bound_param*)ecalloc(1, sizeof(pdo_oci_bound_param));
				param->driver_data = P;

				/* figure out what we're doing */
				switch (PDO_PARAM_TYPE(param->param_type)) {
					case PDO_PARAM_STMT:
						return 0;

					case PDO_PARAM_LOB:
						/* P->thing is now an OCILobLocator * */
						P->oci_type = SQLT_BLOB;
						value_sz = (sb4) sizeof(OCILobLocator*);
						break;

					case PDO_PARAM_STR:
					default:
						P->oci_type = SQLT_CHR;
						value_sz = (sb4) param->max_value_len;
						if (param->max_value_len == 0) {
							value_sz = (sb4) 1332; /* maximum size before value is interpreted as a LONG value */
						}

				}

				if (param->name) {
					STMT_CALL(OCIBindByName, (S->stmt,
							&P->bind, S->err, (text*)param->name->val,
							(sb4) param->name->len, 0, value_sz, P->oci_type,
							&P->indicator, 0, &P->retcode, 0, 0,
							OCI_DATA_AT_EXEC));
				} else {
					STMT_CALL(OCIBindByPos, (S->stmt,
							&P->bind, S->err, ((ub4)param->paramno)+1,
							0, value_sz, P->oci_type,
							&P->indicator, 0, &P->retcode, 0, 0,
							OCI_DATA_AT_EXEC));
				}

				STMT_CALL(OCIBindDynamic, (P->bind,
							S->err,
							param, oci_bind_input_cb,
							param, oci_bind_output_cb));

				return 1;

			case PDO_PARAM_EVT_EXEC_PRE:
				P->indicator = 0;
				P->used_for_output = 0;
				if (PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_LOB) {
					ub4 empty = 0;
					STMT_CALL(OCIDescriptorAlloc, (S->H->env, &P->thing, OCI_DTYPE_LOB, 0, NULL));
					STMT_CALL(OCIAttrSet, (P->thing, OCI_DTYPE_LOB, &empty, 0, OCI_ATTR_LOBEMPTY, S->err));
					S->have_blobs = 1;
				}
				return 1;

			case PDO_PARAM_EVT_EXEC_POST:
				/* fixup stuff set in motion in oci_bind_output_cb */
				if (P->used_for_output) {
					if (P->indicator == -1) {
						/* set up a NULL value */
						if (Z_TYPE_P(parameter) == IS_STRING) {
							/* OCI likes to stick non-terminated strings in things */
							*Z_STRVAL_P(parameter) = '\0';
						}
						zval_dtor(parameter);
						ZVAL_UNDEF(parameter);
					} else if (Z_TYPE_P(parameter) == IS_STRING) {
						Z_STR_P(parameter) = zend_string_init(Z_STRVAL_P(parameter), P->actual_len, 1);
					}
				} else if (PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_LOB && P->thing) {
					php_stream *stm;

					if (Z_TYPE_P(parameter) == IS_NULL) {
						/* if the param is NULL, then we assume that they
						 * wanted to bind a lob locator into it from the query
						 * */

						stm = oci_create_lob_stream(&stmt->database_object_handle, stmt, (OCILobLocator*)P->thing);
						if (stm) {
							OCILobOpen(S->H->svc, S->err, (OCILobLocator*)P->thing, OCI_LOB_READWRITE);
							php_stream_to_zval(stm, parameter);
						}
					} else {
						/* we're a LOB being used for insert; transfer the data now */
						size_t n;
						ub4 amt, offset = 1;
						char *consume;

						php_stream_from_zval_no_verify(stm, parameter);
						if (stm) {
							OCILobOpen(S->H->svc, S->err, (OCILobLocator*)P->thing, OCI_LOB_READWRITE);
							do {
								char buf[8192];
								n = php_stream_read(stm, buf, sizeof(buf));
								if ((int)n <= 0) {
									break;
								}
								consume = buf;
								do {
									amt = (ub4) n;
									OCILobWrite(S->H->svc, S->err, (OCILobLocator*)P->thing,
											&amt, offset, consume, (ub4) n,
											OCI_ONE_PIECE,
											NULL, NULL, 0, SQLCS_IMPLICIT);
									offset += amt;
									n -= amt;
									consume += amt;
								} while (n);
							} while (1);
							OCILobClose(S->H->svc, S->err, (OCILobLocator*)P->thing);
							OCILobFlushBuffer(S->H->svc, S->err, (OCILobLocator*)P->thing, 0);
						} else if (Z_TYPE_P(parameter) == IS_STRING) {
							/* stick the string into the LOB */
							consume = Z_STRVAL_P(parameter);
							n = Z_STRLEN_P(parameter);
							if (n) {
								OCILobOpen(S->H->svc, S->err, (OCILobLocator*)P->thing, OCI_LOB_READWRITE);
								while (n) {
									amt = (ub4) n;
									OCILobWrite(S->H->svc, S->err, (OCILobLocator*)P->thing,
											&amt, offset, consume, (ub4) n,
											OCI_ONE_PIECE,
											NULL, NULL, 0, SQLCS_IMPLICIT);
									consume += amt;
									n -= amt;
								}
								OCILobClose(S->H->svc, S->err, (OCILobLocator*)P->thing);
							}
						}
						OCI_TEMPLOB_CLOSE(S->H->env, S->H->svc, S->H->err, P->thing);
						OCIDescriptorFree(P->thing, OCI_DTYPE_LOB);
						P->thing = NULL;
					}
				}

				return 1;
		}
	}

	return 1;
} /* }}} */

static int oci_stmt_fetch(pdo_stmt_t *stmt, enum pdo_fetch_orientation ori,	zend_long offset) /* {{{ */
{
#if HAVE_OCISTMTFETCH2
	ub4 ociori;
#endif
	pdo_oci_stmt *S = (pdo_oci_stmt*)stmt->driver_data;

#if HAVE_OCISTMTFETCH2
	switch (ori) {
		case PDO_FETCH_ORI_NEXT:	ociori = OCI_FETCH_NEXT; break;
		case PDO_FETCH_ORI_PRIOR:	ociori = OCI_FETCH_PRIOR; break;
		case PDO_FETCH_ORI_FIRST:	ociori = OCI_FETCH_FIRST; break;
		case PDO_FETCH_ORI_LAST:	ociori = OCI_FETCH_LAST; break;
		case PDO_FETCH_ORI_ABS:		ociori = OCI_FETCH_ABSOLUTE; break;
		case PDO_FETCH_ORI_REL:		ociori = OCI_FETCH_RELATIVE; break;
	}
	S->last_err = OCIStmtFetch2(S->stmt, S->err, 1, ociori, (sb4) offset, OCI_DEFAULT);
#else
	S->last_err = OCIStmtFetch(S->stmt, S->err, 1, OCI_FETCH_NEXT, OCI_DEFAULT);
#endif

	if (S->last_err == OCI_NO_DATA) {
		/* no (more) data */
		return 0;
	}

	if (S->last_err == OCI_NEED_DATA) {
		oci_stmt_error("OCI_NEED_DATA");
		return 0;
	}

	if (S->last_err == OCI_SUCCESS_WITH_INFO || S->last_err == OCI_SUCCESS) {
		return 1;
	}

	oci_stmt_error("OCIStmtFetch");

	return 0;
} /* }}} */

static sb4 oci_define_callback(dvoid *octxp, OCIDefine *define, ub4 iter, dvoid **bufpp,
		ub4 **alenpp, ub1 *piecep, dvoid **indpp, ub2 **rcodepp)
{
	pdo_oci_column *col = (pdo_oci_column*)octxp;

	switch (col->dtype) {
		case SQLT_BLOB:
		case SQLT_CLOB:
			*piecep = OCI_ONE_PIECE;
			*bufpp = col->data;
			*alenpp = &col->datalen;
			*indpp = (dvoid *)&col->indicator;
			break;

		default:
			php_error_docref(NULL, E_WARNING,
				"unhandled datatype in oci_define_callback; this should not happen");
			return OCI_ERROR;
	}

	return OCI_CONTINUE;
}

static int oci_stmt_describe(pdo_stmt_t *stmt, int colno) /* {{{ */
{
	pdo_oci_stmt *S = (pdo_oci_stmt*)stmt->driver_data;
	OCIParam *param = NULL;
	text *colname;
	ub2 dtype, data_size, scale, precis;
	ub4 namelen;
	struct pdo_column_data *col = &stmt->columns[colno];
	zend_bool dyn = FALSE;

	/* describe the column */
	STMT_CALL(OCIParamGet, (S->stmt, OCI_HTYPE_STMT, S->err, (dvoid*)&param, colno+1));

	/* what type ? */
	STMT_CALL_MSG(OCIAttrGet, "OCI_ATTR_DATA_TYPE",
			(param, OCI_DTYPE_PARAM, &dtype, 0, OCI_ATTR_DATA_TYPE, S->err));

	/* how big ? */
	STMT_CALL_MSG(OCIAttrGet, "OCI_ATTR_DATA_SIZE",
			(param, OCI_DTYPE_PARAM, &data_size, 0, OCI_ATTR_DATA_SIZE, S->err));

	/* scale ? */
	STMT_CALL_MSG(OCIAttrGet, "OCI_ATTR_SCALE",
			(param, OCI_DTYPE_PARAM, &scale, 0, OCI_ATTR_SCALE, S->err));

	/* precision ? */
	STMT_CALL_MSG(OCIAttrGet, "OCI_ATTR_PRECISION",
			(param, OCI_DTYPE_PARAM, &precis, 0, OCI_ATTR_PRECISION, S->err));

	/* name ? */
	STMT_CALL_MSG(OCIAttrGet, "OCI_ATTR_NAME",
			(param, OCI_DTYPE_PARAM, &colname, &namelen, OCI_ATTR_NAME, S->err));

	col->precision = scale;
	col->maxlen = data_size;
	col->name = zend_string_init((char *)colname, namelen, 0);

	S->cols[colno].dtype = dtype;

	/* how much room do we need to store the field */
	switch (dtype) {
		case SQLT_LBI:
		case SQLT_LNG:
			if (dtype == SQLT_LBI) {
				dtype = SQLT_BIN;
			} else {
				dtype = SQLT_CHR;
			}
			S->cols[colno].datalen = 512; /* XXX should be INT_MAX and fetched by pieces */
			S->cols[colno].data = emalloc(S->cols[colno].datalen + 1);
			col->param_type = PDO_PARAM_STR;
			break;

		case SQLT_BLOB:
		case SQLT_CLOB:
			col->param_type = PDO_PARAM_LOB;
			STMT_CALL(OCIDescriptorAlloc, (S->H->env, (dvoid**)&S->cols[colno].data, OCI_DTYPE_LOB, 0, NULL));
			S->cols[colno].datalen = sizeof(OCILobLocator*);
			dyn = TRUE;
			break;

		case SQLT_BIN:
		default:
			if (dtype == SQLT_DAT || dtype == SQLT_NUM || dtype == SQLT_RDD
#ifdef SQLT_TIMESTAMP
					|| dtype == SQLT_TIMESTAMP
#endif
#ifdef SQLT_TIMESTAMP_TZ
					|| dtype == SQLT_TIMESTAMP_TZ
#endif
					) {
				/* should be big enough for most date formats and numbers */
				S->cols[colno].datalen = 512;
#if defined(SQLT_IBFLOAT) && defined(SQLT_IBDOUBLE)
			} else if (dtype == SQLT_IBFLOAT || dtype == SQLT_IBDOUBLE) {
				S->cols[colno].datalen = 1024;
#endif
			} else {
				S->cols[colno].datalen = (ub4) col->maxlen;
			}
			if (dtype == SQLT_BIN) {
				S->cols[colno].datalen *= 3;
			}
			S->cols[colno].data = emalloc(S->cols[colno].datalen + 1);
			dtype = SQLT_CHR;

			/* returning data as a string */
			col->param_type = PDO_PARAM_STR;
	}

	STMT_CALL(OCIDefineByPos, (S->stmt, &S->cols[colno].def, S->err, colno+1,
				S->cols[colno].data, S->cols[colno].datalen, dtype, &S->cols[colno].indicator,
				&S->cols[colno].fetched_len, &S->cols[colno].retcode, dyn ? OCI_DYNAMIC_FETCH : OCI_DEFAULT));

	if (dyn) {
		STMT_CALL(OCIDefineDynamic, (S->cols[colno].def, S->err, &S->cols[colno],
				oci_define_callback));
	}

	return 1;
} /* }}} */

struct _oci_lob_env {
	OCISvcCtx *svc;
	OCIError  *err;
};
typedef struct _oci_lob_env oci_lob_env;

struct oci_lob_self {
	zval dbh;
	pdo_stmt_t *stmt;
	pdo_oci_stmt *S;
	OCILobLocator *lob;
	oci_lob_env   *E;
	ub4 offset;
};

static size_t oci_blob_write(php_stream *stream, const char *buf, size_t count)
{
	struct oci_lob_self *self = (struct oci_lob_self*)stream->abstract;
	ub4 amt;
	sword r;

	amt = (ub4) count;
	r = OCILobWrite(self->E->svc, self->E->err, self->lob,
		&amt, self->offset, (char*)buf, (ub4) count,
		OCI_ONE_PIECE,
		NULL, NULL, 0, SQLCS_IMPLICIT);

	if (r != OCI_SUCCESS) {
		return (size_t)-1;
	}

	self->offset += amt;
	return amt;
}

static size_t oci_blob_read(php_stream *stream, char *buf, size_t count)
{
	struct oci_lob_self *self = (struct oci_lob_self*)stream->abstract;
	ub4 amt;
	sword r;

	amt = (ub4) count;
	r = OCILobRead(self->E->svc, self->E->err, self->lob,
		&amt, self->offset, buf, (ub4) count,
		NULL, NULL, 0, SQLCS_IMPLICIT);

	if (r != OCI_SUCCESS && r != OCI_NEED_DATA) {
		return (size_t)-1;
	}

	self->offset += amt;
	if (amt < count) {
		stream->eof = 1;
	}
	return amt;
}

static int oci_blob_close(php_stream *stream, int close_handle)
{
	struct oci_lob_self *self = (struct oci_lob_self *)stream->abstract;
	pdo_stmt_t *stmt = self->stmt;

	if (close_handle) {
		zend_object *obj = &stmt->std;

		OCILobClose(self->E->svc, self->E->err, self->lob);
		zval_ptr_dtor(&self->dbh);
		GC_REFCOUNT(obj)--;
		efree(self->E);
		efree(self);
	}

	/* php_pdo_free_statement(stmt); */
	return 0;
}

static int oci_blob_flush(php_stream *stream)
{
	struct oci_lob_self *self = (struct oci_lob_self*)stream->abstract;
	OCILobFlushBuffer(self->E->svc, self->E->err, self->lob, 0);
	return 0;
}

static int oci_blob_seek(php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffset)
{
	struct oci_lob_self *self = (struct oci_lob_self*)stream->abstract;

	if (offset >= PDO_OCI_LOBMAXSIZE) {
		return -1;
	} else {
		self->offset = (ub4) offset + 1;  /* Oracle LOBS are 1-based, but PHP is 0-based */
		return 0;
	}
}

static php_stream_ops oci_blob_stream_ops = {
	oci_blob_write,
	oci_blob_read,
	oci_blob_close,
	oci_blob_flush,
	"pdo_oci blob stream",
	oci_blob_seek,
	NULL,
	NULL,
	NULL
};

static php_stream *oci_create_lob_stream(zval *dbh, pdo_stmt_t *stmt, OCILobLocator *lob)
{
	php_stream *stm;
	struct oci_lob_self *self = ecalloc(1, sizeof(*self));

	ZVAL_COPY_VALUE(&self->dbh, dbh);
	self->lob = lob;
	self->offset = 1; /* 1-based */
	self->stmt = stmt;
	self->S = (pdo_oci_stmt*)stmt->driver_data;
	self->E = ecalloc(1, sizeof(oci_lob_env));
	self->E->svc = self->S->H->svc;
	self->E->err = self->S->err;

	stm = php_stream_alloc(&oci_blob_stream_ops, self, 0, "r+b");

	if (stm) {
		zend_object *obj;
		obj = &stmt->std;
		Z_ADDREF(self->dbh);
		GC_REFCOUNT(obj)++;
		return stm;
	}

	efree(self);
	return NULL;
}

static int oci_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr, size_t *len, int *caller_frees) /* {{{ */
{
	pdo_oci_stmt *S = (pdo_oci_stmt*)stmt->driver_data;
	pdo_oci_column *C = &S->cols[colno];

	/* check the indicator to ensure that the data is intact */
	if (C->indicator == -1) {
		/* A NULL value */
		*ptr = NULL;
		*len = 0;
		return 1;
	} else if (C->indicator == 0) {
		/* it was stored perfectly */

		if (C->dtype == SQLT_BLOB || C->dtype == SQLT_CLOB) {
			if (C->data) {
				*ptr = (char*)oci_create_lob_stream(&stmt->database_object_handle, stmt, (OCILobLocator*)C->data);
				OCILobOpen(S->H->svc, S->err, (OCILobLocator*)C->data, OCI_LOB_READONLY);
			}
			*len = (size_t) 0;
			return *ptr ? 1 : 0;
		}

		*ptr = C->data;
		*len = (size_t) C->fetched_len;
		return 1;
	} else {
		/* it was truncated */
		php_error_docref(NULL, E_WARNING, "column %d data was too large for buffer and was truncated to fit it", colno);

		*ptr = C->data;
		*len = (size_t) C->fetched_len;
		return 1;
	}
} /* }}} */

struct pdo_stmt_methods oci_stmt_methods = {
	oci_stmt_dtor,
	oci_stmt_execute,
	oci_stmt_fetch,
	oci_stmt_describe,
	oci_stmt_get_col,
	oci_stmt_param_hook
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
